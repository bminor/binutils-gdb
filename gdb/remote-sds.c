/* Remote target communications for serial-line targets using SDS' protocol.
   Copyright 1997 Free Software Foundation, Inc.

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

/* This interface was written by studying the behavior of the SDS
   monitor on an ADS 821/860 board, and by consulting the
   documentation of the monitor that is available on Motorola's web
   site.  -sts 8/13/97 */

#include "defs.h"
#include "gdb_string.h"
#include <fcntl.h>
#include "frame.h"
#include "inferior.h"
#include "bfd.h"
#include "symfile.h"
#include "target.h"
#include "wait.h"
#include "gdbcmd.h"
#include "objfiles.h"
#include "gdb-stabs.h"
#include "gdbthread.h"

#include "dcache.h"

#ifdef USG
#include <sys/types.h>
#endif

#include <signal.h>
#include "serial.h"

/* Declarations of local functions. */

static int sds_write_bytes PARAMS ((CORE_ADDR, char *, int));

static int sds_read_bytes PARAMS ((CORE_ADDR, char *, int));

static void sds_files_info PARAMS ((struct target_ops *ignore));

static int sds_xfer_memory PARAMS ((CORE_ADDR, char *,
				    int, int, struct target_ops *));

static void sds_prepare_to_store PARAMS ((void));

static void sds_fetch_registers PARAMS ((int));

static void sds_resume PARAMS ((int, int, enum target_signal));

static int sds_start_remote PARAMS ((char *));

static void sds_open PARAMS ((char *, int));

static void sds_close PARAMS ((int));

static void sds_store_registers PARAMS ((int));

static void sds_mourn PARAMS ((void));

static void sds_restart PARAMS ((void));

static void sds_create_inferior PARAMS ((char *, char *, char **));

static int getmessage PARAMS ((unsigned char *, int));

static int putmessage PARAMS ((unsigned char *, int));

static int sds_send PARAMS ((unsigned char *, int));

static int readchar PARAMS ((int));

static int sds_wait PARAMS ((int, struct target_waitstatus *));

static void sds_kill PARAMS ((void));

static int tohex PARAMS ((int));

static int fromhex PARAMS ((int));

static void sds_detach PARAMS ((char *, int));

static void sds_interrupt PARAMS ((int));

static void sds_interrupt_twice PARAMS ((int));

static void interrupt_query PARAMS ((void));

static int read_frame PARAMS ((char *));

static int sds_insert_breakpoint PARAMS ((CORE_ADDR, char *));

static int sds_remove_breakpoint PARAMS ((CORE_ADDR, char *));


static struct target_ops sds_ops;	/* Forward decl */

/* This was 5 seconds, which is a long time to sit and wait.
   Unless this is going though some terminal server or multiplexer or
   other form of hairy serial connection, I would think 2 seconds would
   be plenty.  */

/* Changed to allow option to set timeout value.
   was static int sds_timeout = 2; */
static int sds_timeout = 2;

/* This variable chooses whether to send a ^C or a break when the user
   requests program interruption.  Although ^C is usually what remote
   systems expect, and that is the default here, sometimes a break is
   preferable instead.  */

static int sds_break;

/* Descriptor for I/O to remote machine.  Initialize it to NULL so
   that sds_open knows that we don't have a file open when the program
   starts.  */

static serial_t sds_desc = NULL;

/* Having this larger than 400 causes us to be incompatible with m68k-stub.c
   and i386-stub.c.  Normally, no one would notice because it only matters
   for writing large chunks of memory (e.g. in downloads).  Also, this needs
   to be more than 400 if required to hold the registers (see below, where
   we round it up based on REGISTER_BYTES).  */
#define	PBUFSIZ	400

/* Maximum number of bytes to read/write at once.  The value here
   is chosen to fill up a packet (the headers account for the 32).  */
#define MAXBUFBYTES ((PBUFSIZ-32)/2)

/* Round up PBUFSIZ to hold all the registers, at least.  */
/* The blank line after the #if seems to be required to work around a
   bug in HP's PA compiler.  */
#if REGISTER_BYTES > MAXBUFBYTES

#undef PBUFSIZ
#define	PBUFSIZ	(REGISTER_BYTES * 2 + 32)
#endif

static int next_msg_id;

static int just_started;

static int message_pending;


/*  Restart the remote side; this is an extended protocol operation.  */

static void
sds_restart ()
{
}

/* Clean up connection to a remote debugger.  */

/* ARGSUSED */
static void
sds_close (quitting)
     int quitting;
{
  if (sds_desc)
    SERIAL_CLOSE (sds_desc);
  sds_desc = NULL;
}

/* Stub for catch_errors.  */

static int
sds_start_remote (dummy)
     char *dummy;
{
  char c;
  unsigned char buf[200];

  immediate_quit = 1;		/* Allow user to interrupt it */

  /* Ack any packet which the remote side has already sent.  */
  SERIAL_WRITE (sds_desc, "{#*\r\n", 5);
  SERIAL_WRITE (sds_desc, "{#}\r\n", 5);

  while ((c = readchar (1)) >= 0)
    printf_unfiltered ("%c");
  printf_unfiltered ("\n");

  next_msg_id = 251;

  buf[0] = 26;
  sds_send (buf, 1);

  buf[0] = 0;
  sds_send (buf, 1);

  immediate_quit = 0;

  start_remote ();		/* Initialize gdb process mechanisms */
  return 1;
}

/* Open a connection to a remote debugger.
   NAME is the filename used for communication.  */

static DCACHE *sds_dcache;

static void
sds_open (name, from_tty)
     char *name;
     int from_tty;
{
  if (name == 0)
    error ("To open a remote debug connection, you need to specify what serial\n\
device is attached to the remote system (e.g. /dev/ttya).");

  target_preopen (from_tty);

  unpush_target (&sds_ops);

  sds_dcache = dcache_init (sds_read_bytes, sds_write_bytes);

  sds_desc = SERIAL_OPEN (name);
  if (!sds_desc)
    perror_with_name (name);

  if (baud_rate != -1)
    {
      if (SERIAL_SETBAUDRATE (sds_desc, baud_rate))
	{
	  SERIAL_CLOSE (sds_desc);
	  perror_with_name (name);
	}
    }


  SERIAL_RAW (sds_desc);

  /* If there is something sitting in the buffer we might take it as a
     response to a command, which would be bad.  */
  SERIAL_FLUSH_INPUT (sds_desc);

  if (from_tty)
    {
      puts_filtered ("Remote debugging using ");
      puts_filtered (name);
      puts_filtered ("\n");
    }
  push_target (&sds_ops);	/* Switch to using remote target now */

  /* Without this, some commands which require an active target (such
     as kill) won't work.  This variable serves (at least) double duty
     as both the pid of the target process (if it has such), and as a
     flag indicating that a target is active.  These functions should
     be split out into seperate variables, especially since GDB will
     someday have a notion of debugging several processes.  */

  inferior_pid = 42000;

  just_started = 1;

  /* Start the remote connection; if error (0), discard this target.
     In particular, if the user quits, be sure to discard it (we'd be
     in an inconsistent state otherwise).  */
  if (!catch_errors (sds_start_remote, (char *)0, 
		     "Couldn't establish connection to remote target\n",
		     RETURN_MASK_ALL))
    pop_target ();
}

/* This takes a program previously attached to and detaches it.  After
   this is done, GDB can be used to debug some other program.  We
   better not have left any breakpoints in the target program or it'll
   die when it hits one.  */

static void
sds_detach (args, from_tty)
     char *args;
     int from_tty;
{
  char buf[PBUFSIZ];

  if (args)
    error ("Argument given to \"detach\" when remotely debugging.");

#if 0
  /* Tell the remote target to detach.  */
  strcpy (buf, "D");
  sds_send (buf, 1);
#endif

  pop_target ();
  if (from_tty)
    puts_filtered ("Ending remote debugging.\n");
}

/* Convert hex digit A to a number.  */

static int
fromhex (a)
     int a;
{
  if (a >= '0' && a <= '9')
    return a - '0';
  else if (a >= 'a' && a <= 'f')
    return a - 'a' + 10;
  else 
    error ("Reply contains invalid hex digit %d", a);
}

/* Convert number NIB to a hex digit.  */

static int
tohex (nib)
     int nib;
{
  if (nib < 10)
    return '0'+nib;
  else
    return 'a'+nib-10;
}

static int
tob64 (inbuf, outbuf, len)
     unsigned char *inbuf;
     char *outbuf;
     int len;
{
  int i, sum;

  if (len % 3 != 0)
    error ("bad length");

  for (i = 0; i < len; i += 3)
    {
      /* Collect the next three bytes into a number.  */
      sum = ((long) *inbuf++) << 16;
      sum |= ((long) *inbuf++) <<  8;
      sum |= ((long) *inbuf++);

      /* Spit out 4 6-bit encodings.  */
      *outbuf++ = ((sum >> 18) & 0x3f) + '0';
      *outbuf++ = ((sum >> 12) & 0x3f) + '0';
      *outbuf++ = ((sum >>  6) & 0x3f) + '0';
      *outbuf++ = (sum & 0x3f) + '0';
    }
}

static int
fromb64 (inbuf, outbuf, len)
     char *inbuf, *outbuf;
     int len;
{
  int i, sum;

  if (len % 4 != 0)
    error ("bad length");

  for (i = 0; i < len; i += 4)
    {
      /* Collect 4 6-bit digits.  */
      sum  = (*inbuf++ - '0') << 18;
      sum |= (*inbuf++ - '0') << 12;
      sum |= (*inbuf++ - '0') <<  6;
      sum |= (*inbuf++ - '0');

      /* Now take the resulting 24-bit number and get three bytes out
         of it.  */
      *outbuf++ = (sum >> 16) & 0xff;
      *outbuf++ = (sum >>  8) & 0xff;
      *outbuf++ = sum & 0xff;
    }

  return (len / 4) * 3;
}


/* Tell the remote machine to resume.  */

static enum target_signal last_sent_signal = TARGET_SIGNAL_0;
int last_sent_step;

static void
sds_resume (pid, step, siggnal)
     int pid, step;
     enum target_signal siggnal;
{
  unsigned char buf[PBUFSIZ];

  dcache_flush (sds_dcache);

  last_sent_signal = siggnal;
  last_sent_step = step;

  buf[0] = (step ? 21 : 20);
  buf[1] = 0;  /* (should be signal?) */

  sds_send (buf, 2);
}

/* Send ^C to target to halt it.  Target will respond, and send us a
   packet.  */

static void
sds_interrupt (signo)
     int signo;
{
  /* If this doesn't work, try more severe steps.  */
  signal (signo, sds_interrupt_twice);
  
  if (remote_debug)
    printf_unfiltered ("sds_interrupt called\n");

  /* Send a break or a ^C, depending on user preference.  */
  if (sds_break)
    SERIAL_SEND_BREAK (sds_desc);
  else
    SERIAL_WRITE (sds_desc, "\003", 1);
}

static void (*ofunc)();

/* The user typed ^C twice.  */
static void
sds_interrupt_twice (signo)
     int signo;
{
  signal (signo, ofunc);
  
  interrupt_query ();

  signal (signo, sds_interrupt);
}

/* Ask the user what to do when an interrupt is received.  */

static void
interrupt_query ()
{
  target_terminal_ours ();

  if (query ("Interrupted while waiting for the program.\n\
Give up (and stop debugging it)? "))
    {
      target_mourn_inferior ();
      return_to_top_level (RETURN_QUIT);
    }

  target_terminal_inferior ();
}

/* If nonzero, ignore the next kill.  */
int kill_kludge;

/* Wait until the remote machine stops, then return, storing status in
   STATUS just as `wait' would.  Returns "pid" (though it's not clear
   what, if anything, that means in the case of this target).  */

static int
sds_wait (pid, status)
     int pid;
     struct target_waitstatus *status;
{
  unsigned char buf[PBUFSIZ];
  int retlen;

  status->kind = TARGET_WAITKIND_EXITED;
  status->value.integer = 0;

  ofunc = (void (*)()) signal (SIGINT, sds_interrupt);

  signal (SIGINT, ofunc);

  if (just_started)
    {
      just_started = 0;
      status->kind = TARGET_WAITKIND_STOPPED;
      return inferior_pid;
    }

  while (1)
    {
      getmessage (buf, 1);

      if (message_pending)
	{
	  buf[0] = 26;
	  retlen = sds_send (buf, 1);
	  message_pending = 0;
	  status->kind = TARGET_WAITKIND_STOPPED;
	  goto got_status;
	}
    }
 got_status:
  return inferior_pid;
}

/* Number of bytes of registers this stub implements.  */
static int register_bytes_found;

/* Read the remote registers into the block REGS.  */
/* Currently we just read all the registers, so we don't use regno.  */

/* ARGSUSED */
static void
sds_fetch_registers (regno)
     int regno;
{
  unsigned char buf[PBUFSIZ];
  int i, len;
  char *p;
  char regs[REGISTER_BYTES];

  /* Unimplemented registers read as all bits zero.  */
  memset (regs, 0, REGISTER_BYTES);

  buf[0] = 18;
  buf[1] = 2;
  buf[2] = 0;
  len = sds_send (buf, 3);

  /* Reply describes registers byte by byte.  Suck them all up, then
     supply them to the register cacheing/storage mechanism.  */

  for (i = 0; i < len; i++)
    regs[i] = buf[i];

  buf[0] = 18;
  buf[1] = 1;
  buf[2] = 0;
  len = sds_send (buf, 3);

  for (i = 0; i < 4 * 6; i++)
    {
      regs[i + 4 * 32 + 8 * 32] = buf[i];
    }

  /* (should warn about reply too short) */

 supply_them:
  for (i = 0; i < NUM_REGS; i++)
    supply_register (i, &regs[REGISTER_BYTE(i)]);
}

/* Prepare to store registers.  Since we may send them all (using a
   'G' request), we have to read out the ones we don't want to change
   first.  */

static void 
sds_prepare_to_store ()
{
  /* Make sure the entire registers array is valid.  */
  read_register_bytes (0, (char *)NULL, REGISTER_BYTES);
}

/* Store register REGNO, or all registers if REGNO == -1, from the contents
   of REGISTERS.  FIXME: ignores errors.  */

static void
sds_store_registers (regno)
     int regno;
{
  unsigned char buf[PBUFSIZ];
  int i;
  char *p;

  buf[0] = 19;
  buf[1] = 2;
  buf[2] = 0;
  buf[3] = 0;

  p = buf + 4;

  for (i = 0; i < 4 * 32; i++)
    p[i] = registers[i];

  sds_send (buf, 4 + 4 * 32);

  buf[0] = 19;
  buf[1] = 1;
  buf[2] = 0;
  buf[3] = 0;

  p = buf + 4;

  for (i = 0; i < 4 * 10; i++)
    p[i] = registers[i];

  sds_send (buf, 4 + 4 * 10);
}

/* 
   Use of the data cache *used* to be disabled because it loses for looking at
   and changing hardware I/O ports and the like.  Accepting `volatile'
   would perhaps be one way to fix it.  Another idea would be to use the
   executable file for the text segment (for all SEC_CODE sections?
   For all SEC_READONLY sections?).  This has problems if you want to
   actually see what the memory contains (e.g. self-modifying code,
   clobbered memory, user downloaded the wrong thing).  

   Because it speeds so much up, it's now enabled, if you're playing
   with registers you turn it of (set remotecache 0)
*/

/* Read a word from remote address ADDR and return it.
   This goes through the data cache.  */

#if 0	/* unused? */
static int
sds_fetch_word (addr)
     CORE_ADDR addr;
{
  return dcache_fetch (sds_dcache, addr);
}

/* Write a word WORD into remote address ADDR.
   This goes through the data cache.  */

static void
sds_store_word (addr, word)
     CORE_ADDR addr;
     int word;
{
  dcache_poke (sds_dcache, addr, word);
}
#endif	/* 0 (unused?) */


/* Write memory data directly to the remote machine.  This does not
   inform the data cache; the data cache uses this.  MEMADDR is the
   address in the remote memory space.  MYADDR is the address of the
   buffer in our space.  LEN is the number of bytes.

   Returns number of bytes transferred, or 0 for error.  */

static int
sds_write_bytes (memaddr, myaddr, len)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
{
  int max_buf_size;		/* Max size of packet output buffer */
  int origlen;
  unsigned char buf[PBUFSIZ];
  int todo;
  int i;

  /* Chop the transfer down if necessary */

  max_buf_size = 150;

  origlen = len;
  while (len > 0)
    {
      todo = min (len, max_buf_size);

      buf[0] = 13;
      buf[1] = 0;
      buf[2] = (int) (memaddr >> 24) & 0xff;
      buf[3] = (int) (memaddr >> 16) & 0xff;
      buf[4] = (int) (memaddr >>  8) & 0xff;
      buf[5] = (int) (memaddr      ) & 0xff;
      buf[6] = 1;
      buf[7] = 0;

      for (i = 0; i < todo; i++)
	buf[i + 8] = myaddr[i];

      sds_send (buf, 8 + todo);

      /* (should look at result) */

      myaddr += todo;
      memaddr += todo;
      len -= todo;
    }
  return origlen;
}

/* Read memory data directly from the remote machine.  This does not
   use the data cache; the data cache uses this.  MEMADDR is the
   address in the remote memory space.  MYADDR is the address of the
   buffer in our space.  LEN is the number of bytes.

   Returns number of bytes transferred, or 0 for error.  */

static int
sds_read_bytes (memaddr, myaddr, len)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
{
  int max_buf_size;		/* Max size of packet output buffer */
  int origlen, retlen;
  unsigned char buf[PBUFSIZ];
  int todo;
  int i;

  /* Chop the transfer down if necessary */

  max_buf_size = 150;

  origlen = len;
  while (len > 0)
    {
      todo = min (len, max_buf_size);

      buf[0] = 12;
      buf[1] = 0;
      buf[2] = (int) (memaddr >> 24) & 0xff;
      buf[3] = (int) (memaddr >> 16) & 0xff;
      buf[4] = (int) (memaddr >>  8) & 0xff;
      buf[5] = (int) (memaddr      ) & 0xff;
      buf[6] = (int) (todo >> 8) & 0xff;
      buf[7] = (int) (todo     ) & 0xff;
      buf[8] = 1;

      retlen = sds_send (buf, 9);

      if (retlen - 2 != todo)
	{
	  return 0;
	}

      /* Reply describes memory byte by byte. */

      for (i = 0; i < todo; i++)
	myaddr[i] = buf[i + 2];

      myaddr += todo;
      memaddr += todo;
      len -= todo;
    }

  return origlen;
}

/* Read or write LEN bytes from inferior memory at MEMADDR,
   transferring to or from debugger address MYADDR.  Write to inferior
   if SHOULD_WRITE is nonzero.  Returns length of data written or
   read; 0 for error.  */

/* ARGSUSED */
static int
sds_xfer_memory(memaddr, myaddr, len, should_write, target)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
     int should_write;
     struct target_ops *target;			/* ignored */
{
  return dcache_xfer_memory (sds_dcache, memaddr, myaddr, len, should_write);
}


static void
sds_files_info (ignore)
     struct target_ops *ignore;
{
  puts_filtered ("Debugging a target over a serial line.\n");
}

/* Stuff for dealing with the packets which are part of this protocol.
   See comment at top of file for details.  */

/* Read a single character from the remote end, masking it down to 7 bits. */

static int
readchar (timeout)
     int timeout;
{
  int ch;

  ch = SERIAL_READCHAR (sds_desc, timeout);

  if (remote_debug > 1 && ch >= 0)
    printf_unfiltered("%c(%x)", ch, ch);

  switch (ch)
    {
    case SERIAL_EOF:
      error ("Remote connection closed");
    case SERIAL_ERROR:
      perror_with_name ("Remote communication error");
    case SERIAL_TIMEOUT:
      return ch;
    default:
      return ch & 0x7f;
    }
}

static int
compute_checksum (csum, buf, len)
     int csum, len;
     char *buf;
{
  int i;

  for (i = 0; i < len; ++i)
    csum += (unsigned char) buf[i];

  csum %= 253;
  return csum;
}

/* Send the command in BUF to the remote machine, and read the reply
   into BUF also.  */

static int
sds_send (buf, len)
     unsigned char *buf;
     int len;
{
  putmessage (buf, len);

  return getmessage (buf, 0);
}

/* Send a message to the remote machine.  */

static int
putmessage (buf, len)
     unsigned char *buf;
     int len;
{
  int i;
  unsigned char csum = 0;
  char buf2[PBUFSIZ], buf3[PBUFSIZ];
  unsigned char header[3];
  int ch;
  int tcount = 0;
  char *p;

  /* Copy the packet into buffer BUF2, encapsulating it
     and giving it a checksum.  */

  if (len > (int) sizeof (buf2) - 5)		/* Prosanity check */
    abort();

  if (remote_debug)
    {
      fprintf_unfiltered (gdb_stderr, "Message to send: \"");
      for (i = 0; i < len; ++i)
	fprintf_unfiltered (gdb_stderr, "%02x", buf[i]);
      fprintf_unfiltered (gdb_stderr, "\"\n");
    }

  p = buf2;
  *p++ = '$';

  header[1] = next_msg_id;

  if (len % 3 != 0)
    {
      buf[len] = '\0';
      buf[len+1] = '\0';
    }

  len = ((len + 2) / 3) * 3;

  header[2] = len;

  csum = compute_checksum (csum, buf, len);
  csum = compute_checksum (csum, header+1, 2);

  header[0] = csum;

  tob64 (header, p, 3);
  p += 4;
  tob64 (buf, buf3, len);

  for (i = 0; i < (len / 3) * 4; i++)
    {
      *p++ = buf3[i];
    }
  *p++ = '\r';
  *p++ = '\n';

  next_msg_id = (next_msg_id + 3) % 245;

  /* Send it over and over until we get a positive ack.  */

  while (1)
    {
      int started_error_output = 0;

      if (remote_debug)
	{
	  *p = '\0';
	  printf_unfiltered ("Sending encoded: \"%s\"", buf2);
	  printf_unfiltered ("  (Checksum %d, id %d, length %d)\n",
			     header[0], header[1], header[2]);
	  gdb_flush (gdb_stdout);
	}
      if (SERIAL_WRITE (sds_desc, buf2, p - buf2))
	perror_with_name ("putmessage: write failed");

      return 1;

    }

}

/* Come here after finding the start of the frame.  Collect the rest
   into BUF.  Returns 0 on any error, 1 on success.  */

static int
read_frame (buf)
     char *buf;
{
  char *bp;
  int c;

  bp = buf;

  while (1)
    {
      c = readchar (sds_timeout);

      switch (c)
	{
	case SERIAL_TIMEOUT:
	  if (remote_debug)
	    puts_filtered ("Timeout in mid-message, retrying\n");
	  return 0;
	case '$':
	  if (remote_debug)
	    puts_filtered ("Saw new packet start in middle of old one\n");
	  return 0;		/* Start a new packet, count retries */
	case '\r':
	  break;

	case '\n':
	  {
	    *bp = '\000';
	    if (remote_debug)
	      printf_filtered ("Received encoded: \"%s\"\n", buf);
	    return 1;
	  }

	default:
	  if (bp < buf + PBUFSIZ - 1)
	    {
	      *bp++ = c;
	      continue;
	    }

	  *bp = '\0';
	  puts_filtered ("Message too long: ");
	  puts_filtered (buf);
	  puts_filtered ("\n");

	  return 0;
	}
    }
}

/* Read a packet from the remote machine, with error checking,
   and store it in BUF.  BUF is expected to be of size PBUFSIZ.
   If FOREVER, wait forever rather than timing out; this is used
   while the target is executing user code.  */

static int
getmessage (buf, forever)
     unsigned char *buf;
     int forever;
{
  int c, c2, c3;
  int tries;
  int timeout;
  int val, i, len, csum;
  unsigned char header[3];
  unsigned char inbuf[500];

  strcpy (buf, "timeout");

  if (forever)
    {
#ifdef MAINTENANCE_CMDS
      timeout = watchdog > 0 ? watchdog : -1;
#else
      timeout = -1;
#endif
    }

  else
    timeout = sds_timeout;

#define MAX_TRIES 3

  for (tries = 1; tries <= MAX_TRIES; tries++)
    {
      /* This can loop forever if the remote side sends us characters
	 continuously, but if it pauses, we'll get a zero from readchar
	 because of timeout.  Then we'll count that as a retry.  */

      /* Note that we will only wait forever prior to the start of a packet.
	 After that, we expect characters to arrive at a brisk pace.  They
	 should show up within sds_timeout intervals.  */

      do
	{
	  c = readchar (timeout);

	  if (c == SERIAL_TIMEOUT)
	    {
#ifdef MAINTENANCE_CMDS
	      if (forever)	/* Watchdog went off.  Kill the target. */
		{
		  target_mourn_inferior ();
		  error ("Watchdog has expired.  Target detached.\n");
		}
#endif
	      if (remote_debug)
		puts_filtered ("Timed out.\n");
	      goto retry;
	    }
	}
      while (c != '$' && c != '{');

      /* We might have seen a "trigraph", a sequence of three characters
	 that indicate various sorts of communication state.  */

      if (c == '{')
	{
	  /* Read the other two chars of the trigraph. */
	  c2 = readchar (timeout);
	  c3 = readchar (timeout);
	  if (remote_debug)
	    fprintf_unfiltered (gdb_stderr, "Trigraph %c%c%c received\n",
				c, c2, c3);
	  if (c3 == '+')
	    {
	      message_pending = 1;
	      return;
	    }
	  continue;
	}

      val = read_frame (inbuf);

      if (val == 1)
	{
	  fromb64 (inbuf, header, 4);
	  /* (should check out other bits) */
	  fromb64 (inbuf + 4, buf, strlen (inbuf) - 4);

	  len = header[2];

	  csum = 0;
	  csum = compute_checksum (csum, buf, len);
	  csum = compute_checksum (csum, header + 1, 2);

	  if (csum != header[0])
	    fprintf_unfiltered (gdb_stderr,
				"Checksum mismatch: computed %d, received %d\n",
				csum, header[0]);

	  if (header[2] == 0xff)
	    fprintf_unfiltered (gdb_stderr, "Requesting resend...\n");

	  if (remote_debug)
	    {
	      fprintf_unfiltered (gdb_stderr,
				  "... (Got checksum %d, id %d, length %d)\n",
				  header[0], header[1], header[2]);
	      fprintf_unfiltered (gdb_stderr, "Message received: \"");
	      for (i = 0; i < len; ++i)
		{
		  fprintf_unfiltered (gdb_stderr, "%02x", (unsigned char) buf[i]);
		}
	      fprintf_unfiltered (gdb_stderr, "\"\n");
	    }

	  /* no ack required? */
	  return len;
	}

      /* Try the whole thing again.  */
    retry:
      /* need to do something here */
    }

  /* We have tried hard enough, and just can't receive the packet.  Give up. */

  printf_unfiltered ("Ignoring packet error, continuing...\n");
}

static void
sds_kill ()
{
  /* For some mysterious reason, wait_for_inferior calls kill instead of
     mourn after it gets TARGET_WAITKIND_SIGNALLED.  Work around it.  */
  if (kill_kludge)
    {
      kill_kludge = 0;
      target_mourn_inferior ();
      return;
    }

#if 0 /* fix to use 1-arg fn */
  /* Use catch_errors so the user can quit from gdb even when we aren't on
     speaking terms with the remote system.  */
  catch_errors (putmessage, "k", "", RETURN_MASK_ERROR);
#endif

  /* Don't wait for it to die.  I'm not really sure it matters whether
     we do or not.  For the existing stubs, kill is a noop.  */
  target_mourn_inferior ();
}

static void
sds_mourn ()
{
  unpush_target (&sds_ops);
  generic_mourn_inferior ();
}

static void
sds_create_inferior (exec_file, args, env)
     char *exec_file;
     char *args;
     char **env;
{
  /* Rip out the breakpoints; we'll reinsert them after restarting
     the remote server.  */
  remove_breakpoints ();

  /* Now restart the remote server.  */
  sds_restart ();

  /* Now put the breakpoints back in.  This way we're safe if the
     restart function works via a unix fork on the remote side.  */
  insert_breakpoints ();

  /* Clean up from the last time we were running.  */
  clear_proceed_status ();

  /* Let the remote process run.  */
  proceed (-1, TARGET_SIGNAL_0, 0);
}


/* The SDS monitor has commands for breakpoint insertion, although it
   it doesn't actually manage the breakpoints, it just returns the
   replaced instruction back to the debugger.  */

static int
sds_insert_breakpoint (addr, contents_cache)
     CORE_ADDR addr;
     char *contents_cache;
{
  int retlen;
  unsigned char buf[PBUFSIZ];

  buf[0] = 16;
  buf[1] = 0;

  retlen = sds_send (buf, 7);
}

static int
sds_remove_breakpoint (addr, contents_cache)
     CORE_ADDR addr;
     char *contents_cache;
{
  int retlen;
  unsigned char buf[PBUFSIZ];

  buf[0] = 17;
  buf[1] = 0;

  retlen = sds_send (buf, 7);
}

/* Define the target operations vector. */

static struct target_ops sds_ops =
{
  "sds",			/* to_shortname */
  "Remote serial target with SDS protocol",	/* to_longname */
  "Use a remote computer via a serial line, using the SDS protocol.\n\
Specify the serial device it is connected to (e.g. /dev/ttya).",  /* to_doc */
  sds_open,			/* to_open */
  sds_close,			/* to_close */
  NULL,				/* to_attach */
  sds_detach,			/* to_detach */
  sds_resume,			/* to_resume */
  sds_wait,			/* to_wait */
  sds_fetch_registers,		/* to_fetch_registers */
  sds_store_registers,		/* to_store_registers */
  sds_prepare_to_store,		/* to_prepare_to_store */
  sds_xfer_memory,		/* to_xfer_memory */
  sds_files_info,		/* to_files_info */
  sds_insert_breakpoint,	/* to_insert_breakpoint */
  sds_remove_breakpoint,	/* to_remove_breakpoint */
  NULL,				/* to_terminal_init */
  NULL,				/* to_terminal_inferior */
  NULL,				/* to_terminal_ours_for_output */
  NULL,				/* to_terminal_ours */
  NULL,				/* to_terminal_info */
  sds_kill,			/* to_kill */
  generic_load,			/* to_load */
  NULL,				/* to_lookup_symbol */
  sds_create_inferior,		/* to_create_inferior */
  sds_mourn,			/* to_mourn_inferior */
  0,				/* to_can_run */
  0,				/* to_notice_signals */
  0,				/* to_thread_alive */
  0,				/* to_stop */
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

/* Put a command string, in args, out to the monitor and display the
   reply message.  */

static void
sds_command (args, from_tty)
     char *args;
     int from_tty;
{
  char *p;
  int i, len, resp_len;
  unsigned char buf[1000];

  /* Convert hexadecimal chars into a byte buffer.  */
  p = args;
  len = 0;
  while (*p != '\0')
    {
      buf[len++] = fromhex (p[0]) * 16 + fromhex (p[1]);
      if (p[1] == '\0')
	break;
      p += 2;
    }

  len = sds_send (buf, len);

  printf_filtered ("Reply is ");
  for (i = 0; i < len; ++i)
    {
      printf_filtered ("%02x", buf[i]);
    }  
  printf_filtered ("\n");
}

void
_initialize_remote_sds ()
{
  add_target (&sds_ops);

  add_show_from_set (add_set_cmd ("sdstimeout", no_class,
				  var_integer, (char *)&sds_timeout,
				  "Set timeout value for sds read.\n", &setlist),
		     &showlist);

  add_com ("sds", class_obscure, sds_command,
	   "Send a command to the SDS monitor."); 
}
