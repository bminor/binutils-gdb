/* Remote debugging interface for MIPS remote debugging protocol.
   Copyright 1993, 1994 Free Software Foundation, Inc.
   Contributed by Cygnus Support.  Written by Ian Lance Taylor
   <ian@cygnus.com>.

This file is part of GDB.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "defs.h"
#include "inferior.h"
#include "bfd.h"
#include "symfile.h"
#include "wait.h"
#include "gdbcmd.h"
#include "gdbcore.h"
#include "serial.h"
#include "target.h"
#include "remote-utils.h"

#include <signal.h>
#include <varargs.h>

/* Prototypes for local functions.  */

static int
mips_readchar PARAMS ((int timeout));

static int
mips_receive_header PARAMS ((unsigned char *hdr, int *pgarbage, int ch,
			     int timeout));

static int
mips_receive_trailer PARAMS ((unsigned char *trlr, int *pgarbage, int *pch,
			      int timeout));

static int mips_cksum PARAMS ((const unsigned char *hdr,
			       const unsigned char *data,
			       int len));

static void
mips_send_packet PARAMS ((const char *s, int get_ack));

static int mips_receive_packet PARAMS ((char *buff, int throw_error,
					int timeout));

static int
mips_request PARAMS ((char cmd, unsigned int addr, unsigned int data,
		      int *perr, int timeout));

static void
mips_initialize PARAMS ((void));

static void
mips_open PARAMS ((char *name, int from_tty));

static void
mips_close PARAMS ((int quitting));

static void
mips_detach PARAMS ((char *args, int from_tty));

static void mips_resume PARAMS ((int pid, int step,
				 enum target_signal siggnal));

static int
mips_wait PARAMS ((int pid, struct target_waitstatus *status));

static int
mips_map_regno PARAMS ((int regno));

static void
mips_fetch_registers PARAMS ((int regno));

static void
mips_prepare_to_store PARAMS ((void));

static void
mips_store_registers PARAMS ((int regno));

static int
mips_fetch_word PARAMS ((CORE_ADDR addr));

static int
mips_store_word PARAMS ((CORE_ADDR addr, int value, char *old_contents));

static int
mips_xfer_memory PARAMS ((CORE_ADDR memaddr, char *myaddr, int len,
			  int write, struct target_ops *ignore));

static void
mips_files_info PARAMS ((struct target_ops *ignore));

static void
mips_load PARAMS ((char *args, int from_tty));

static void
mips_create_inferior PARAMS ((char *execfile, char *args, char **env));

static void
mips_mourn_inferior PARAMS ((void));

/* A forward declaration.  */
extern struct target_ops mips_ops;

/* The MIPS remote debugging interface is built on top of a simple
   packet protocol.  Each packet is organized as follows:

   SYN	The first character is always a SYN (ASCII 026, or ^V).  SYN
	may not appear anywhere else in the packet.  Any time a SYN is
	seen, a new packet should be assumed to have begun.

   TYPE_LEN
	This byte contains the upper five bits of the logical length
	of the data section, plus a single bit indicating whether this
	is a data packet or an acknowledgement.  The documentation
	indicates that this bit is 1 for a data packet, but the actual
	board uses 1 for an acknowledgement.  The value of the byte is
		0x40 + (ack ? 0x20 : 0) + (len >> 6)
	(we always have 0 <= len < 1024).  Acknowledgement packets do
	not carry data, and must have a data length of 0.

   LEN1 This byte contains the lower six bits of the logical length of
	the data section.  The value is
	 	0x40 + (len & 0x3f)

   SEQ	This byte contains the six bit sequence number of the packet.
	The value is
		0x40 + seq
	An acknowlegment packet contains the sequence number of the
	packet being acknowledged plus 1 module 64.  Data packets are
	transmitted in sequence.  There may only be one outstanding
	unacknowledged data packet at a time.  The sequence numbers
	are independent in each direction.  If an acknowledgement for
	the previous packet is received (i.e., an acknowledgement with
	the sequence number of the packet just sent) the packet just
	sent should be retransmitted.  If no acknowledgement is
	received within a timeout period, the packet should be
	retransmitted.  This has an unfortunate failure condition on a
	high-latency line, as a delayed acknowledgement may lead to an
	endless series of duplicate packets.

   DATA	The actual data bytes follow.  The following characters are
	escaped inline with DLE (ASCII 020, or ^P):
		SYN (026)	DLE S
		DLE (020)	DLE D
		^C  (003)	DLE C
		^S  (023)	DLE s
		^Q  (021)	DLE q
	The additional DLE characters are not counted in the logical
	length stored in the TYPE_LEN and LEN1 bytes.

   CSUM1
   CSUM2
   CSUM3
	These bytes contain an 18 bit checksum of the complete
	contents of the packet excluding the SEQ byte and the
	CSUM[123] bytes.  The checksum is simply the twos complement
	addition of all the bytes treated as unsigned characters.  The
	values of the checksum bytes are:
		CSUM1: 0x40 + ((cksum >> 12) & 0x3f)
		CSUM2: 0x40 + ((cksum >> 6) & 0x3f)
		CSUM3: 0x40 + (cksum & 0x3f)

   It happens that the MIPS remote debugging protocol always
   communicates with ASCII strings.  Because of this, this
   implementation doesn't bother to handle the DLE quoting mechanism,
   since it will never be required.  */

/* The SYN character which starts each packet.  */
#define SYN '\026'

/* The 0x40 used to offset each packet (this value ensures that all of
   the header and trailer bytes, other than SYN, are printable ASCII
   characters).  */
#define HDR_OFFSET 0x40

/* The indices of the bytes in the packet header.  */
#define HDR_INDX_SYN 0
#define HDR_INDX_TYPE_LEN 1
#define HDR_INDX_LEN1 2
#define HDR_INDX_SEQ 3
#define HDR_LENGTH 4

/* The data/ack bit in the TYPE_LEN header byte.  */
#define TYPE_LEN_DA_BIT 0x20
#define TYPE_LEN_DATA 0
#define TYPE_LEN_ACK TYPE_LEN_DA_BIT

/* How to compute the header bytes.  */
#define HDR_SET_SYN(data, len, seq) (SYN)
#define HDR_SET_TYPE_LEN(data, len, seq) \
  (HDR_OFFSET \
   + ((data) ? TYPE_LEN_DATA : TYPE_LEN_ACK) \
   + (((len) >> 6) & 0x1f))
#define HDR_SET_LEN1(data, len, seq) (HDR_OFFSET + ((len) & 0x3f))
#define HDR_SET_SEQ(data, len, seq) (HDR_OFFSET + (seq))

/* Check that a header byte is reasonable.  */
#define HDR_CHECK(ch) (((ch) & HDR_OFFSET) == HDR_OFFSET)

/* Get data from the header.  These macros evaluate their argument
   multiple times.  */
#define HDR_IS_DATA(hdr) \
  (((hdr)[HDR_INDX_TYPE_LEN] & TYPE_LEN_DA_BIT) == TYPE_LEN_DATA)
#define HDR_GET_LEN(hdr) \
  ((((hdr)[HDR_INDX_TYPE_LEN] & 0x1f) << 6) + (((hdr)[HDR_INDX_LEN1] & 0x3f)))
#define HDR_GET_SEQ(hdr) ((hdr)[HDR_INDX_SEQ] & 0x3f)

/* The maximum data length.  */
#define DATA_MAXLEN 1023

/* The trailer offset.  */
#define TRLR_OFFSET HDR_OFFSET

/* The indices of the bytes in the packet trailer.  */
#define TRLR_INDX_CSUM1 0
#define TRLR_INDX_CSUM2 1
#define TRLR_INDX_CSUM3 2
#define TRLR_LENGTH 3

/* How to compute the trailer bytes.  */
#define TRLR_SET_CSUM1(cksum) (TRLR_OFFSET + (((cksum) >> 12) & 0x3f))
#define TRLR_SET_CSUM2(cksum) (TRLR_OFFSET + (((cksum) >>  6) & 0x3f))
#define TRLR_SET_CSUM3(cksum) (TRLR_OFFSET + (((cksum)      ) & 0x3f))

/* Check that a trailer byte is reasonable.  */
#define TRLR_CHECK(ch) (((ch) & TRLR_OFFSET) == TRLR_OFFSET)

/* Get data from the trailer.  This evaluates its argument multiple
   times.  */
#define TRLR_GET_CKSUM(trlr) \
  ((((trlr)[TRLR_INDX_CSUM1] & 0x3f) << 12) \
   + (((trlr)[TRLR_INDX_CSUM2] & 0x3f) <<  6) \
   + ((trlr)[TRLR_INDX_CSUM3] & 0x3f))

/* The sequence number modulos.  */
#define SEQ_MODULOS (64)

/* Set to 1 if the target is open.  */
static int mips_is_open;

/* Set to 1 while the connection is being initialized.  */
static int mips_initializing;

/* The next sequence number to send.  */
static int mips_send_seq;

/* The next sequence number we expect to receive.  */
static int mips_receive_seq;

/* The time to wait before retransmitting a packet, in seconds.  */
static int mips_retransmit_wait = 3;

/* The number of times to try retransmitting a packet before giving up.  */
static int mips_send_retries = 10;

/* The number of garbage characters to accept when looking for an
   SYN for the next packet.  */
static int mips_syn_garbage = 1050;

/* The time to wait for a packet, in seconds.  */
static int mips_receive_wait = 5;

/* Set if we have sent a packet to the board but have not yet received
   a reply.  */
static int mips_need_reply = 0;

/* Handle used to access serial I/O stream.  */
static serial_t mips_desc;

/* Handle low-level error that we can't recover from.  Note that just
   error()ing out from target_wait or some such low-level place will cause
   all hell to break loose--the rest of GDB will tend to get left in an
   inconsistent state.  */

static void NORETURN
mips_error (va_alist)
     va_dcl
{
  va_list args;
  char *string;

  va_start (args);
  target_terminal_ours ();
  wrap_here("");			/* Force out any buffered output */
  gdb_flush (gdb_stdout);
  if (error_pre_print)
    fprintf_filtered (gdb_stderr, error_pre_print);
  string = va_arg (args, char *);
  vfprintf_filtered (gdb_stderr, string, args);
  fprintf_filtered (gdb_stderr, "\n");
  va_end (args);

  /* Clean up in such a way that mips_close won't try to talk to the
     board (it almost surely won't work since we weren't able to talk to
     it).  */
  mips_is_open = 0;
  SERIAL_CLOSE (mips_desc);

  printf_unfiltered ("Ending remote MIPS debugging.\n");
  target_mourn_inferior ();

  return_to_top_level (RETURN_ERROR);
}

/* Read a character from the remote, aborting on error.  Returns
   SERIAL_TIMEOUT on timeout (since that's what SERIAL_READCHAR
   returns).  FIXME: If we see the string "<IDT>" from the board, then
   we are debugging on the main console port, and we have somehow
   dropped out of remote debugging mode.  In this case, we
   automatically go back in to remote debugging mode.  This is a hack,
   put in because I can't find any way for a program running on the
   remote board to terminate without also ending remote debugging
   mode.  I assume users won't have any trouble with this; for one
   thing, the IDT documentation generally assumes that the remote
   debugging port is not the console port.  This is, however, very
   convenient for DejaGnu when you only have one connected serial
   port.  */

static int
mips_readchar (timeout)
     int timeout;
{
  int ch;
  static int state = 0;
  static char nextstate[5] = { '<', 'I', 'D', 'T', '>' };

  ch = SERIAL_READCHAR (mips_desc, timeout);
  if (ch == SERIAL_EOF)
    mips_error ("End of file from remote");
  if (ch == SERIAL_ERROR)
    mips_error ("Error reading from remote: %s", safe_strerror (errno));
  if (sr_get_debug () > 1)
    {
      /* Don't use _filtered; we can't deal with a QUIT out of
	 target_wait, and I think this might be called from there.  */
      if (ch != SERIAL_TIMEOUT)
	printf_unfiltered ("Read '%c' %d 0x%x\n", ch, ch, ch);
      else
	printf_unfiltered ("Timed out in read\n");
    }

  /* If we have seen <IDT> and we either time out, or we see a @
     (which was echoed from a packet we sent), reset the board as
     described above.  The first character in a packet after the SYN
     (which is not echoed) is always an @ unless the packet is more
     than 64 characters long, which ours never are.  */
  if ((ch == SERIAL_TIMEOUT || ch == '@')
      && state == 5
      && ! mips_initializing)
    {
      if (sr_get_debug () > 0)
	/* Don't use _filtered; we can't deal with a QUIT out of
	   target_wait, and I think this might be called from there.  */
	printf_unfiltered ("Reinitializing MIPS debugging mode\n");
      SERIAL_WRITE (mips_desc, "\015db tty0\015", sizeof "\015db tty0\015" - 1);
      sleep (1);

      mips_need_reply = 0;
      mips_initialize ();

      state = 0;

      mips_error ("Remote board reset");
    }

  if (ch == nextstate[state])
    ++state;
  else
    state = 0;

  return ch;
}

/* Get a packet header, putting the data in the supplied buffer.
   PGARBAGE is a pointer to the number of garbage characters received
   so far.  CH is the last character received.  Returns 0 for success,
   or -1 for timeout.  */

static int
mips_receive_header (hdr, pgarbage, ch, timeout)
     unsigned char *hdr;
     int *pgarbage;
     int ch;
     int timeout;
{
  int i;

  while (1)
    {
      /* Wait for a SYN.  mips_syn_garbage is intended to prevent
	 sitting here indefinitely if the board sends us one garbage
	 character per second.  ch may already have a value from the
	 last time through the loop.  */
      while (ch != SYN)
	{
	  ch = mips_readchar (timeout);
	  if (ch == SERIAL_TIMEOUT)
	    return -1;
	  if (ch != SYN)
	    {
	      /* Printing the character here lets the user of gdb see
		 what the program is outputting, if the debugging is
		 being done on the console port.  Don't use _filtered;
		 we can't deal with a QUIT out of target_wait.  */
	      if (! mips_initializing || sr_get_debug () > 0)
		{
		  if (ch < 0x20 && ch != '\n')
		    {
		      putchar_unfiltered ('^');
		      putchar_unfiltered (ch + 0x40);
		    }
		  else
		    putchar_unfiltered (ch);
		  gdb_flush (gdb_stdout);
		}

	      ++*pgarbage;
	      if (*pgarbage > mips_syn_garbage)
		mips_error ("Remote debugging protocol failure");
	    }
	}

      /* Get the packet header following the SYN.  */
      for (i = 1; i < HDR_LENGTH; i++)
	{
	  ch = mips_readchar (timeout);
	  if (ch == SERIAL_TIMEOUT)
	    return -1;

	  /* Make sure this is a header byte.  */
	  if (ch == SYN || ! HDR_CHECK (ch))
	    break;

	  hdr[i] = ch;
	}

      /* If we got the complete header, we can return.  Otherwise we
	 loop around and keep looking for SYN.  */
      if (i >= HDR_LENGTH)
	return 0;
    }
}

/* Get a packet header, putting the data in the supplied buffer.
   PGARBAGE is a pointer to the number of garbage characters received
   so far.  The last character read is returned in *PCH.  Returns 0
   for success, -1 for timeout, -2 for error.  */

static int
mips_receive_trailer (trlr, pgarbage, pch, timeout)
     unsigned char *trlr;
     int *pgarbage;
     int *pch;
     int timeout;
{
  int i;
  int ch;

  for (i = 0; i < TRLR_LENGTH; i++)
    {
      ch = mips_readchar (timeout);
      *pch = ch;
      if (ch == SERIAL_TIMEOUT)
	return -1;
      if (! TRLR_CHECK (ch))
	return -2;
      trlr[i] = ch;
    }
  return 0;
}

/* Get the checksum of a packet.  HDR points to the packet header.
   DATA points to the packet data.  LEN is the length of DATA.  */

static int
mips_cksum (hdr, data, len)
     const unsigned char *hdr;
     const unsigned char *data;
     int len;
{
  register const unsigned char *p;
  register int c;
  register int cksum;

  cksum = 0;

  /* The initial SYN is not included in the checksum.  */
  c = HDR_LENGTH - 1;
  p = hdr + 1;
  while (c-- != 0)
    cksum += *p++;
  
  c = len;
  p = data;
  while (c-- != 0)
    cksum += *p++;

  return cksum;
}

/* Send a packet containing the given ASCII string.  */

static void
mips_send_packet (s, get_ack)
     const char *s;
     int get_ack;
{
  unsigned int len;
  unsigned char *packet;
  register int cksum;
  int try;

  len = strlen (s);
  if (len > DATA_MAXLEN)
    mips_error ("MIPS protocol data packet too long: %s", s);

  packet = (unsigned char *) alloca (HDR_LENGTH + len + TRLR_LENGTH + 1);

  packet[HDR_INDX_SYN] = HDR_SET_SYN (1, len, mips_send_seq);
  packet[HDR_INDX_TYPE_LEN] = HDR_SET_TYPE_LEN (1, len, mips_send_seq);
  packet[HDR_INDX_LEN1] = HDR_SET_LEN1 (1, len, mips_send_seq);
  packet[HDR_INDX_SEQ] = HDR_SET_SEQ (1, len, mips_send_seq);

  memcpy (packet + HDR_LENGTH, s, len);

  cksum = mips_cksum (packet, packet + HDR_LENGTH, len);
  packet[HDR_LENGTH + len + TRLR_INDX_CSUM1] = TRLR_SET_CSUM1 (cksum);
  packet[HDR_LENGTH + len + TRLR_INDX_CSUM2] = TRLR_SET_CSUM2 (cksum);
  packet[HDR_LENGTH + len + TRLR_INDX_CSUM3] = TRLR_SET_CSUM3 (cksum);

  /* Increment the sequence number.  This will set mips_send_seq to
     the sequence number we expect in the acknowledgement.  */
  mips_send_seq = (mips_send_seq + 1) % SEQ_MODULOS;

  if (! get_ack)
    return;

  /* We can only have one outstanding data packet, so we just wait for
     the acknowledgement here.  Keep retransmitting the packet until
     we get one, or until we've tried too many times.  */
  for (try = 0; try < mips_send_retries; try++)
    {
      int garbage;
      int ch;

      if (sr_get_debug () > 0)
	{
	  /* Don't use _filtered; we can't deal with a QUIT out of
	     target_wait, and I think this might be called from there.  */
	  packet[HDR_LENGTH + len + TRLR_LENGTH] = '\0';
	  printf_unfiltered ("Writing \"%s\"\n", packet + 1);
	}

      if (SERIAL_WRITE (mips_desc, packet,
			HDR_LENGTH + len + TRLR_LENGTH) != 0)
	mips_error ("write to target failed: %s", safe_strerror (errno));

      garbage = 0;
      ch = 0;
      while (1)
	{
	  unsigned char hdr[HDR_LENGTH + 1];
	  unsigned char trlr[TRLR_LENGTH + 1];
	  int err;
	  int seq;

	  /* Get the packet header.  If we time out, resend the data
	     packet.  */
	  err = mips_receive_header (hdr, &garbage, ch, mips_retransmit_wait);
	  if (err != 0)
	    break;

	  ch = 0;

	  /* If we get a data packet, assume it is a duplicate and
	     ignore it.  FIXME: If the acknowledgement is lost, this
	     data packet may be the packet the remote sends after the
	     acknowledgement.  */
	  if (HDR_IS_DATA (hdr))
	    continue;

	  /* If the length is not 0, this is a garbled packet.  */
	  if (HDR_GET_LEN (hdr) != 0)
	    continue;

	  /* Get the packet trailer.  */
	  err = mips_receive_trailer (trlr, &garbage, &ch,
				      mips_retransmit_wait);

	  /* If we timed out, resend the data packet.  */
	  if (err == -1)
	    break;

	  /* If we got a bad character, reread the header.  */
	  if (err != 0)
	    continue;

	  /* If the checksum does not match the trailer checksum, this
	     is a bad packet; ignore it.  */
	  if (mips_cksum (hdr, (unsigned char *) NULL, 0)
	      != TRLR_GET_CKSUM (trlr))
	    continue;

	  if (sr_get_debug () > 0)
	    {
	      hdr[HDR_LENGTH] = '\0';
	      trlr[TRLR_LENGTH] = '\0';
	      /* Don't use _filtered; we can't deal with a QUIT out of
		 target_wait, and I think this might be called from there.  */
	      printf_unfiltered ("Got ack %d \"%s%s\"\n",
			       HDR_GET_SEQ (hdr), hdr + 1, trlr);
	    }

	  /* If this ack is for the current packet, we're done.  */
	  seq = HDR_GET_SEQ (hdr);
	  if (seq == mips_send_seq)
	    return;

	  /* If this ack is for the last packet, resend the current
	     packet.  */
	  if ((seq + 1) % SEQ_MODULOS == mips_send_seq)
	    break;

	  /* Otherwise this is a bad ack; ignore it.  Increment the
	     garbage count to ensure that we do not stay in this loop
	     forever.  */
	  ++garbage;
	}
    }

  mips_error ("Remote did not acknowledge packet");
}

/* Receive and acknowledge a packet, returning the data in BUFF (which
   should be DATA_MAXLEN + 1 bytes).  The protocol documentation
   implies that only the sender retransmits packets, so this code just
   waits silently for a packet.  It returns the length of the received
   packet.  If THROW_ERROR is nonzero, call error() on errors.  If not,
   don't print an error message and return -1.  */

static int
mips_receive_packet (buff, throw_error, timeout)
     char *buff;
     int throw_error;
     int timeout;
{
  int ch;
  int garbage;
  int len;
  unsigned char ack[HDR_LENGTH + TRLR_LENGTH + 1];
  int cksum;

  ch = 0;
  garbage = 0;
  while (1)
    {
      unsigned char hdr[HDR_LENGTH];
      unsigned char trlr[TRLR_LENGTH];
      int i;
      int err;

      if (mips_receive_header (hdr, &garbage, ch, timeout) != 0)
	{
	  if (throw_error)
	    mips_error ("Timed out waiting for remote packet");
	  else
	    return -1;
	}

      ch = 0;

      /* An acknowledgement is probably a duplicate; ignore it.  */
      if (! HDR_IS_DATA (hdr))
	{
	  /* Don't use _filtered; we can't deal with a QUIT out of
	     target_wait, and I think this might be called from there.  */
	  if (sr_get_debug () > 0)
	    printf_unfiltered ("Ignoring unexpected ACK\n");
	  continue;
	}

      /* If this is the wrong sequence number, ignore it.  */
      if (HDR_GET_SEQ (hdr) != mips_receive_seq)
	{
	  /* Don't use _filtered; we can't deal with a QUIT out of
	     target_wait, and I think this might be called from there.  */
	  if (sr_get_debug () > 0)
	    printf_unfiltered ("Ignoring sequence number %d (want %d)\n",
			     HDR_GET_SEQ (hdr), mips_receive_seq);
	  continue;
	}

      len = HDR_GET_LEN (hdr);

      for (i = 0; i < len; i++)
	{
	  int rch;

	  rch = mips_readchar (timeout);
	  if (rch == SYN)
	    {
	      ch = SYN;
	      break;
	    }
	  if (rch == SERIAL_TIMEOUT)
	    {
	      if (throw_error)
		mips_error ("Timed out waiting for remote packet");
	      else
		return -1;
	    }
	  buff[i] = rch;
	}

      if (i < len)
	{
	  /* Don't use _filtered; we can't deal with a QUIT out of
	     target_wait, and I think this might be called from there.  */
	  if (sr_get_debug () > 0)
	    printf_unfiltered ("Got new SYN after %d chars (wanted %d)\n",
			     i, len);
	  continue;
	}

      err = mips_receive_trailer (trlr, &garbage, &ch, timeout);
      if (err == -1)
	{
	  if (throw_error)
	    mips_error ("Timed out waiting for packet");
	  else
	    return -1;
	}
      if (err == -2)
	{
	  /* Don't use _filtered; we can't deal with a QUIT out of
	     target_wait, and I think this might be called from there.  */
	  if (sr_get_debug () > 0)
	    printf_unfiltered ("Got SYN when wanted trailer\n");
	  continue;
	}

      if (mips_cksum (hdr, buff, len) == TRLR_GET_CKSUM (trlr))
	break;

      if (sr_get_debug () > 0)
	/* Don't use _filtered; we can't deal with a QUIT out of
	   target_wait, and I think this might be called from there.  */
	printf_unfiltered ("Bad checksum; data %d, trailer %d\n",
			 mips_cksum (hdr, buff, len),
			 TRLR_GET_CKSUM (trlr));

      /* The checksum failed.  Send an acknowledgement for the
	 previous packet to tell the remote to resend the packet.  */
      ack[HDR_INDX_SYN] = HDR_SET_SYN (0, 0, mips_receive_seq);
      ack[HDR_INDX_TYPE_LEN] = HDR_SET_TYPE_LEN (0, 0, mips_receive_seq);
      ack[HDR_INDX_LEN1] = HDR_SET_LEN1 (0, 0, mips_receive_seq);
      ack[HDR_INDX_SEQ] = HDR_SET_SEQ (0, 0, mips_receive_seq);

      cksum = mips_cksum (ack, (unsigned char *) NULL, 0);

      ack[HDR_LENGTH + TRLR_INDX_CSUM1] = TRLR_SET_CSUM1 (cksum);
      ack[HDR_LENGTH + TRLR_INDX_CSUM2] = TRLR_SET_CSUM2 (cksum);
      ack[HDR_LENGTH + TRLR_INDX_CSUM3] = TRLR_SET_CSUM3 (cksum);

      if (sr_get_debug () > 0)
	{
	  ack[HDR_LENGTH + TRLR_LENGTH] = '\0';
	  /* Don't use _filtered; we can't deal with a QUIT out of
	     target_wait, and I think this might be called from there.  */
	  printf_unfiltered ("Writing ack %d \"%s\"\n", mips_receive_seq,
			   ack + 1);
	}

      if (SERIAL_WRITE (mips_desc, ack, HDR_LENGTH + TRLR_LENGTH) != 0)
	{
	  if (throw_error)
	    mips_error ("write to target failed: %s", safe_strerror (errno));
	  else
	    return -1;
	}
    }

  if (sr_get_debug () > 0)
    {
      buff[len] = '\0';
      /* Don't use _filtered; we can't deal with a QUIT out of
	 target_wait, and I think this might be called from there.  */
      printf_unfiltered ("Got packet \"%s\"\n", buff);
    }

  /* We got the packet.  Send an acknowledgement.  */
  mips_receive_seq = (mips_receive_seq + 1) % SEQ_MODULOS;

  ack[HDR_INDX_SYN] = HDR_SET_SYN (0, 0, mips_receive_seq);
  ack[HDR_INDX_TYPE_LEN] = HDR_SET_TYPE_LEN (0, 0, mips_receive_seq);
  ack[HDR_INDX_LEN1] = HDR_SET_LEN1 (0, 0, mips_receive_seq);
  ack[HDR_INDX_SEQ] = HDR_SET_SEQ (0, 0, mips_receive_seq);

  cksum = mips_cksum (ack, (unsigned char *) NULL, 0);

  ack[HDR_LENGTH + TRLR_INDX_CSUM1] = TRLR_SET_CSUM1 (cksum);
  ack[HDR_LENGTH + TRLR_INDX_CSUM2] = TRLR_SET_CSUM2 (cksum);
  ack[HDR_LENGTH + TRLR_INDX_CSUM3] = TRLR_SET_CSUM3 (cksum);

  if (sr_get_debug () > 0)
    {
      ack[HDR_LENGTH + TRLR_LENGTH] = '\0';
      /* Don't use _filtered; we can't deal with a QUIT out of
	 target_wait, and I think this might be called from there.  */
      printf_unfiltered ("Writing ack %d \"%s\"\n", mips_receive_seq,
		       ack + 1);
    }

  if (SERIAL_WRITE (mips_desc, ack, HDR_LENGTH + TRLR_LENGTH) != 0)
    {
      if (throw_error)
	mips_error ("write to target failed: %s", safe_strerror (errno));
      else
	return -1;
    }

  return len;
}

/* Optionally send a request to the remote system and optionally wait
   for the reply.  This implements the remote debugging protocol,
   which is built on top of the packet protocol defined above.  Each
   request has an ADDR argument and a DATA argument.  The following
   requests are defined:

   \0	don't send a request; just wait for a reply
   i	read word from instruction space at ADDR
   d	read word from data space at ADDR
   I	write DATA to instruction space at ADDR
   D	write DATA to data space at ADDR
   r	read register number ADDR
   R	set register number ADDR to value DATA
   c	continue execution (if ADDR != 1, set pc to ADDR)
   s	single step (if ADDR != 1, set pc to ADDR)

   The read requests return the value requested.  The write requests
   return the previous value in the changed location.  The execution
   requests return a UNIX wait value (the approximate signal which
   caused execution to stop is in the upper eight bits).

   If PERR is not NULL, this function waits for a reply.  If an error
   occurs, it sets *PERR to 1 and sets errno according to what the
   target board reports.  */

static int
mips_request (cmd, addr, data, perr, timeout)
     char cmd;
     unsigned int addr;
     unsigned int data;
     int *perr;
     int timeout;
{
  char buff[DATA_MAXLEN + 1];
  int len;
  int rpid;
  char rcmd;
  int rerrflg;
  int rresponse;
  
  if (cmd != '\0')
    {
      if (mips_need_reply)
	fatal ("mips_request: Trying to send command before reply");
      sprintf (buff, "0x0 %c 0x%x 0x%x", cmd, addr, data);
      mips_send_packet (buff, 1);
      mips_need_reply = 1;
    }

  if (perr == (int *) NULL)
    return 0;

  if (! mips_need_reply)
    fatal ("mips_request: Trying to get reply before command");

  mips_need_reply = 0;

  len = mips_receive_packet (buff, 1, timeout);
  buff[len] = '\0';

  if (sscanf (buff, "0x%x %c 0x%x 0x%x",
	      &rpid, &rcmd, &rerrflg, &rresponse) != 4
      || (cmd != '\0' && rcmd != cmd))
    mips_error ("Bad response from remote board");

  if (rerrflg != 0)
    {
      *perr = 1;

      /* FIXME: This will returns MIPS errno numbers, which may or may
	 not be the same as errno values used on other systems.  If
	 they stick to common errno values, they will be the same, but
	 if they don't, they must be translated.  */
      errno = rresponse;

      return 0;
    }

  *perr = 0;
  return rresponse;
}

static void
mips_initialize_cleanups (arg)
     PTR arg;
{
  mips_initializing = 0;
}

/* Initialize a new connection to the MIPS board, and make sure we are
   really connected.  */

static void
mips_initialize ()
{
  char cr;
  char buff[DATA_MAXLEN + 1];
  int err;
  struct cleanup *old_cleanups = make_cleanup (mips_initialize_cleanups, NULL);

  /* What is this code doing here?  I don't see any way it can happen, and
     it might mean mips_initializing didn't get cleared properly.
     So I'll make it a warning.  */
  if (mips_initializing)
    {
      warning ("internal error: mips_initialize called twice");
      return;
    }

  mips_initializing = 1;

  mips_send_seq = 0;
  mips_receive_seq = 0;

  /* The board seems to want to send us a packet.  I don't know what
     it means.  The packet seems to be triggered by a carriage return
     character, although perhaps any character would do.  */
  cr = '\015';
  /* FIXME check the result from this */
  SERIAL_WRITE (mips_desc, &cr, 1);

  if (mips_receive_packet (buff, 0, 3) < 0)
    {
      char cc;

      /* We did not receive the packet we expected; try resetting the
	 board and trying again.  */
      printf_filtered ("Failed to initialize; trying to reset board\n");
      cc = '\003';
      SERIAL_WRITE (mips_desc, &cc, 1);
      sleep (2);
      SERIAL_WRITE (mips_desc, "\015db tty0\015", sizeof "\015db tty0\015" - 1);
      sleep (1);
      cr = '\015';
      SERIAL_WRITE (mips_desc, &cr, 1);
    }
  mips_receive_packet (buff, 1, 3);

  do_cleanups (old_cleanups);

  /* If this doesn't call error, we have connected; we don't care if
     the request itself succeeds or fails.  */
  mips_request ('r', (unsigned int) 0, (unsigned int) 0, &err,
		mips_receive_wait);
}

/* Open a connection to the remote board.  */

static void
mips_open (name, from_tty)
     char *name;
     int from_tty;
{
  if (name == 0)
    error (
"To open a MIPS remote debugging connection, you need to specify what serial\n\
device is attached to the target board (e.g., /dev/ttya).");

  target_preopen (from_tty);

  if (mips_is_open)
    unpush_target (&mips_ops);

  mips_desc = SERIAL_OPEN (name);
  if (mips_desc == (serial_t) NULL)
    perror_with_name (name);

  SERIAL_RAW (mips_desc);

  mips_is_open = 1;

  mips_initialize ();

  if (from_tty)
    printf_unfiltered ("Remote MIPS debugging using %s\n", name);
  push_target (&mips_ops);	/* Switch to using remote target now */

  /* FIXME: Should we call start_remote here?  */
}

/* Close a connection to the remote board.  */

static void
mips_close (quitting)
     int quitting;
{
  if (mips_is_open)
    {
      int err;

      mips_is_open = 0;

      /* Get the board out of remote debugging mode.  */
      mips_request ('x', (unsigned int) 0, (unsigned int) 0, &err,
		    mips_receive_wait);

      SERIAL_CLOSE (mips_desc);
    }
}

/* Detach from the remote board.  */

static void
mips_detach (args, from_tty)
     char *args;
     int from_tty;
{
  if (args)
    error ("Argument given to \"detach\" when remotely debugging.");

  pop_target ();
  if (from_tty)
    printf_unfiltered ("Ending remote MIPS debugging.\n");
}

/* Tell the target board to resume.  This does not wait for a reply
   from the board.  */

static void
mips_resume (pid, step, siggnal)
     int pid, step;
     enum target_signal siggnal;
{
  if (siggnal != TARGET_SIGNAL_0)
    warning
      ("Can't send signals to a remote system.  Try `handle %s ignore'.",
       target_signal_to_name (siggnal));

  mips_request (step ? 's' : 'c',
		(unsigned int) 1,
		(unsigned int) 0,
		(int *) NULL,
		mips_receive_wait);
}

/* Return the signal corresponding to SIG, where SIG is the number which
   the MIPS protocol uses for the signal.  */
enum target_signal
mips_signal_from_protocol (sig)
     int sig;
{
  /* We allow a few more signals than the IDT board actually returns, on
     the theory that there is at least *some* hope that perhaps the numbering
     for these signals is widely agreed upon.  */
  if (sig <= 0
      || sig > 31)
    return TARGET_SIGNAL_UNKNOWN;

  /* Don't want to use target_signal_from_host because we are converting
     from MIPS signal numbers, not host ones.  Our internal numbers
     match the MIPS numbers for the signals the board can return, which
     are: SIGINT, SIGSEGV, SIGBUS, SIGILL, SIGFPE, SIGTRAP.  */
  return (enum target_signal) sig;
}

/* Wait until the remote stops, and return a wait status.  */

static int
mips_wait (pid, status)
     int pid;
     struct target_waitstatus *status;
{
  int rstatus;
  int err;

  /* If we have not sent a single step or continue command, then the
     board is waiting for us to do something.  Return a status
     indicating that it is stopped.  */
  if (! mips_need_reply)
    {
      status->kind = TARGET_WAITKIND_STOPPED;
      status->value.sig = TARGET_SIGNAL_TRAP;
      return 0;
    }

  /* No timeout; we sit here as long as the program continues to execute.  */
  rstatus = mips_request ('\0', (unsigned int) 0, (unsigned int) 0, &err, -1);
  if (err)
    mips_error ("Remote failure: %s", safe_strerror (errno));

  /* Translate a MIPS waitstatus.  We use constants here rather than WTERMSIG
     and so on, because the constants we want here are determined by the
     MIPS protocol and have nothing to do with what host we are running on.  */
  if ((rstatus & 0377) == 0)
    {
      status->kind = TARGET_WAITKIND_EXITED;
      status->value.integer = (((rstatus) >> 8) & 0377);
    }
  else if ((rstatus & 0377) == 0177)
    {
      status->kind = TARGET_WAITKIND_STOPPED;
      status->value.sig = mips_signal_from_protocol (((rstatus) >> 8) & 0377);
    }
  else
    {
      status->kind = TARGET_WAITKIND_SIGNALLED;
      status->value.sig = mips_signal_from_protocol (rstatus & 0177);
    }

  return 0;
}

/* We have to map between the register numbers used by gdb and the
   register numbers used by the debugging protocol.  This function
   assumes that we are using tm-mips.h.  */

#define REGNO_OFFSET 96

static int
mips_map_regno (regno)
     int regno;
{
  if (regno < 32)
    return regno;
  if (regno >= FP0_REGNUM && regno < FP0_REGNUM + 32)
    return regno - FP0_REGNUM + 32;
  switch (regno)
    {
    case PC_REGNUM:
      return REGNO_OFFSET + 0;
    case CAUSE_REGNUM:
      return REGNO_OFFSET + 1;
    case HI_REGNUM:
      return REGNO_OFFSET + 2;
    case LO_REGNUM:
      return REGNO_OFFSET + 3;
    case FCRCS_REGNUM:
      return REGNO_OFFSET + 4;
    case FCRIR_REGNUM:
      return REGNO_OFFSET + 5;
    default:
      /* FIXME: Is there a way to get the status register?  */
      return 0;
    }
}

/* Fetch the remote registers.  */

static void
mips_fetch_registers (regno)
     int regno;
{
  unsigned LONGEST val;
  int err;

  if (regno == -1)
    {
      for (regno = 0; regno < NUM_REGS; regno++)
	mips_fetch_registers (regno);
      return;
    }

  if (regno == FP_REGNUM || regno == ZERO_REGNUM)
    /* FP_REGNUM on the mips is a hack which is just supposed to read
       zero (see also mips-nat.c).  */
    val = 0;
  else
    {
      val = mips_request ('r', (unsigned int) mips_map_regno (regno),
			  (unsigned int) 0, &err, mips_receive_wait);
      if (err)
	mips_error ("Can't read register %d: %s", regno,
		    safe_strerror (errno));
    }

  {
    char buf[MAX_REGISTER_RAW_SIZE];

    /* We got the number the register holds, but gdb expects to see a
       value in the target byte ordering.  */
    store_unsigned_integer (buf, REGISTER_RAW_SIZE (regno), val);
    supply_register (regno, buf);
  }
}

/* Prepare to store registers.  The MIPS protocol can store individual
   registers, so this function doesn't have to do anything.  */

static void
mips_prepare_to_store ()
{
}

/* Store remote register(s).  */

static void
mips_store_registers (regno)
     int regno;
{
  int err;

  if (regno == -1)
    {
      for (regno = 0; regno < NUM_REGS; regno++)
	mips_store_registers (regno);
      return;
    }

  mips_request ('R', (unsigned int) mips_map_regno (regno),
		(unsigned int) read_register (regno),
		&err, mips_receive_wait);
  if (err)
    mips_error ("Can't write register %d: %s", regno, safe_strerror (errno));
}

/* Fetch a word from the target board.  */

static int 
mips_fetch_word (addr)
     CORE_ADDR addr;
{
  int val;
  int err;

  val = mips_request ('d', (unsigned int) addr, (unsigned int) 0, &err,
		      mips_receive_wait);
  if (err)
    {
      /* Data space failed; try instruction space.  */
      val = mips_request ('i', (unsigned int) addr, (unsigned int) 0, &err,
			  mips_receive_wait);
      if (err)
	mips_error ("Can't read address 0x%x: %s", addr, safe_strerror (errno));
    }
  return val;
}

/* Store a word to the target board.  Returns errno code or zero for
   success.  If OLD_CONTENTS is non-NULL, put the old contents of that
   memory location there.  */

static int
mips_store_word (addr, val, old_contents)
     CORE_ADDR addr;
     int val;
     char *old_contents;
{
  int err;
  unsigned int oldcontents;

  oldcontents = mips_request ('D', (unsigned int) addr, (unsigned int) val,
			      &err,
			      mips_receive_wait);
  if (err)
    {
      /* Data space failed; try instruction space.  */
      oldcontents = mips_request ('I', (unsigned int) addr,
				  (unsigned int) val, &err,
				  mips_receive_wait);
      if (err)
	return errno;
    }
  if (old_contents != NULL)
    store_unsigned_integer (old_contents, 4, oldcontents);
  return 0;
}

/* Read or write LEN bytes from inferior memory at MEMADDR,
   transferring to or from debugger address MYADDR.  Write to inferior
   if SHOULD_WRITE is nonzero.  Returns length of data written or
   read; 0 for error.  Note that protocol gives us the correct value
   for a longword, since it transfers values in ASCII.  We want the
   byte values, so we have to swap the longword values.  */

static int
mips_xfer_memory (memaddr, myaddr, len, write, ignore)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
     int write;
     struct target_ops *ignore;
{
  register int i;
  /* Round starting address down to longword boundary.  */
  register CORE_ADDR addr = memaddr &~ 3;
  /* Round ending address up; get number of longwords that makes.  */
  register int count = (((memaddr + len) - addr) + 3) / 4;
  /* Allocate buffer of that many longwords.  */
  register char *buffer = alloca (count * 4);

  int status;

  if (write)
    {
      /* Fill start and end extra bytes of buffer with existing data.  */
      if (addr != memaddr || len < 4)
	{
	  /* Need part of initial word -- fetch it.  */
	  store_unsigned_integer (&buffer[0], 4, mips_fetch_word (addr));
	}

      if (count > 1)
	{
	  /* Need part of last word -- fetch it.  FIXME: we do this even
	     if we don't need it.  */
	  store_unsigned_integer (&buffer[(count - 1) * 4], 4,
				  mips_fetch_word (addr + (count - 1) * 4));
	}

      /* Copy data to be written over corresponding part of buffer */

      memcpy ((char *) buffer + (memaddr & 3), myaddr, len);

      /* Write the entire buffer.  */

      for (i = 0; i < count; i++, addr += 4)
	{
	  status = mips_store_word (addr,
				    extract_unsigned_integer (&buffer[i*4], 4),
				    NULL);
	  /* Report each kilobyte (we download 32-bit words at a time) */
	  if (i % 256 == 255) 
	    {
	      printf_unfiltered ("*");
	      fflush (stdout);
	    }
	  if (status)
	    {
	      errno = status;
	      return 0;
	    }
	  /* FIXME: Do we want a QUIT here?  */
	}
      if (count >= 256)
	printf_unfiltered ("\n");
    }
  else
    {
      /* Read all the longwords */
      for (i = 0; i < count; i++, addr += 4)
	{
	  store_unsigned_integer (&buffer[i*4], 4, mips_fetch_word (addr));
	  QUIT;
	}

      /* Copy appropriate bytes out of the buffer.  */
      memcpy (myaddr, buffer + (memaddr & 3), len);
    }
  return len;
}

/* Print info on this target.  */

static void
mips_files_info (ignore)
     struct target_ops *ignore;
{
  printf_unfiltered ("Debugging a MIPS board over a serial line.\n");
}

/* Kill the process running on the board.  This will actually only
   work if we are doing remote debugging over the console input.  I
   think that if IDT/sim had the remote debug interrupt enabled on the
   right port, we could interrupt the process with a break signal.  */

static void
mips_kill ()
{
#if 0
  if (mips_is_open)
    {
      char cc;

      /* Send a ^C.  */
      cc = '\003';
      SERIAL_WRITE (mips_desc, &cc, 1);
      sleep (1);
      target_mourn_inferior ();
    }
#endif
}

/* Start running on the target board.  */

static void
mips_create_inferior (execfile, args, env)
     char *execfile;
     char *args;
     char **env;
{
  CORE_ADDR entry_pt;

  if (args && *args)
    {
      warning ("\
Can't pass arguments to remote MIPS board; arguments ignored.");
      /* And don't try to use them on the next "run" command.  */
      execute_command ("set args", 0);
    }

  if (execfile == 0 || exec_bfd == 0)
    error ("No executable file specified");

  entry_pt = (CORE_ADDR) bfd_get_start_address (exec_bfd);

  init_wait_for_inferior ();

  /* FIXME: Should we set inferior_pid here?  */

  proceed (entry_pt, TARGET_SIGNAL_DEFAULT, 0);
}

/* Clean up after a process.  Actually nothing to do.  */

static void
mips_mourn_inferior ()
{
  unpush_target (&mips_ops);
  generic_mourn_inferior ();
}

/* We can write a breakpoint and read the shadow contents in one
   operation.  */

/* The IDT board uses an unusual breakpoint value, and sometimes gets
   confused when it sees the usual MIPS breakpoint instruction.  */

#if TARGET_BYTE_ORDER == BIG_ENDIAN
static unsigned char break_insn[] = {0, 0, 0x0a, 0x0d};
#else
static unsigned char break_insn[] = {0x0d, 0x0a, 0, 0};
#endif

/* Insert a breakpoint on targets that don't have any better breakpoint
   support.  We read the contents of the target location and stash it,
   then overwrite it with a breakpoint instruction.  ADDR is the target
   location in the target machine.  CONTENTS_CACHE is a pointer to 
   memory allocated for saving the target contents.  It is guaranteed
   by the caller to be long enough to save sizeof BREAKPOINT bytes (this
   is accomplished via BREAKPOINT_MAX).  */

static int
mips_insert_breakpoint (addr, contents_cache)
     CORE_ADDR addr;
     char *contents_cache;
{
  int status;

  return
    mips_store_word (addr,
		     extract_unsigned_integer (break_insn, sizeof break_insn),
		     contents_cache);
}

static int
mips_remove_breakpoint (addr, contents_cache)
     CORE_ADDR addr;
     char *contents_cache;
{
  return target_write_memory (addr, contents_cache, sizeof break_insn);
}

/* The target vector.  */

struct target_ops mips_ops =
{
  "mips",			/* to_shortname */
  "Remote MIPS debugging over serial line",	/* to_longname */
  "\
Debug a board using the MIPS remote debugging protocol over a serial line.\n\
The argument is the device it is connected to or, if it contains a colon,\n\
HOST:PORT to access a board over a network",  /* to_doc */
  mips_open,			/* to_open */
  mips_close,			/* to_close */
  NULL,				/* to_attach */
  mips_detach,			/* to_detach */
  mips_resume,			/* to_resume */
  mips_wait,			/* to_wait */
  mips_fetch_registers,		/* to_fetch_registers */
  mips_store_registers,		/* to_store_registers */
  mips_prepare_to_store,	/* to_prepare_to_store */
  mips_xfer_memory,		/* to_xfer_memory */
  mips_files_info,		/* to_files_info */
  mips_insert_breakpoint,	/* to_insert_breakpoint */
  mips_remove_breakpoint,	/* to_remove_breakpoint */
  NULL,				/* to_terminal_init */
  NULL,				/* to_terminal_inferior */
  NULL,				/* to_terminal_ours_for_output */
  NULL,				/* to_terminal_ours */
  NULL,				/* to_terminal_info */
  mips_kill,			/* to_kill */
  generic_load,			/* to_load */
  NULL,				/* to_lookup_symbol */
  mips_create_inferior,		/* to_create_inferior */
  mips_mourn_inferior,		/* to_mourn_inferior */
  NULL,				/* to_can_run */
  NULL,				/* to_notice_signals */
  process_stratum,		/* to_stratum */
  NULL,				/* to_next */
  1,				/* to_has_all_memory */
  1,				/* to_has_memory */
  1,				/* to_has_stack */
  1,				/* to_has_registers */
  1,				/* to_has_execution */
  NULL,				/* sections */
  NULL,				/* sections_end */
  OPS_MAGIC			/* to_magic */
};

void
_initialize_remote_mips ()
{
  add_target (&mips_ops);

  add_show_from_set (
    add_set_cmd ("timeout", no_class, var_zinteger,
		 (char *) &mips_receive_wait,
		 "Set timeout in seconds for remote MIPS serial I/O.",
		 &setlist),
	&showlist);

  add_show_from_set (
    add_set_cmd ("retransmit-timeout", no_class, var_zinteger,
		 (char *) &mips_retransmit_wait,
	 "Set retransmit timeout in seconds for remote MIPS serial I/O.\n\
This is the number of seconds to wait for an acknowledgement to a packet\n\
before resending the packet.", &setlist),
	&showlist);
}
