/* Remote target communications for serial-line targets in custom GDB protocol
   Copyright 1988, 1991, 1992, 1993 Free Software Foundation, Inc.

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

/* Remote communication protocol.

   A debug packet whose contents are <data>
   is encapsulated for transmission in the form:

	$ <data> # CSUM1 CSUM2

	<data> must be ASCII alphanumeric and cannot include characters
	'$' or '#'

	CSUM1 and CSUM2 are ascii hex representation of an 8-bit 
	checksum of <data>, the most significant nibble is sent first.
	the hex digits 0-9,a-f are used.

   Receiver responds with:

	+	- if CSUM is correct and ready for next packet
	-	- if CSUM is incorrect

   <data> is as follows:
   All values are encoded in ascii hex digits.

	Request		Packet

	read registers  g
	reply		XX....X		Each byte of register data
					is described by two hex digits.
					Registers are in the internal order
					for GDB, and the bytes in a register
					are in the same order the machine uses.
			or ENN		for an error.

	write regs	GXX..XX		Each byte of register data
					is described by two hex digits.
	reply		OK		for success
			ENN		for an error

	read mem	mAA..AA,LLLL	AA..AA is address, LLLL is length.
	reply		XX..XX		XX..XX is mem contents
			or ENN		NN is errno

	write mem	MAA..AA,LLLL:XX..XX
					AA..AA is address,
					LLLL is number of bytes,
					XX..XX is data
	reply		OK		for success
			ENN		for an error

	cont		cAA..AA		AA..AA is address to resume
					If AA..AA is omitted,
					resume at same address.

	step		sAA..AA		AA..AA is address to resume
					If AA..AA is omitted,
					resume at same address.

	last signal     ?               Reply the current reason for stopping.
                                        This is the same reply as is generated
					for step or cont : SAA where AA is the
					signal number.

	There is no immediate reply to step or cont.
	The reply comes when the machine stops.
	It is		SAA		AA is the "signal number"

	or...		TAAn...:r...;n:r...;n...:r...;
					AA = signal number
					n... = register number
					r... = register contents

	kill req	k
*/

#include "defs.h"
#include <string.h>
#include <fcntl.h>
#include "frame.h"
#include "inferior.h"
#include "bfd.h"
#include "symfile.h"
#include "target.h"
#include "wait.h"
#include "terminal.h"
#include "gdbcmd.h"

#if !defined(DONT_USE_REMOTE)
#ifdef USG
#include <sys/types.h>
#endif

#include <signal.h>
#include "serial.h"

/* Prototypes for local functions */

static void
remote_write_bytes PARAMS ((CORE_ADDR, char *, int));

static void
remote_read_bytes PARAMS ((CORE_ADDR, char *, int));

static void
remote_files_info PARAMS ((struct target_ops *));

static int
remote_xfer_memory PARAMS ((CORE_ADDR, char *, int, int, struct target_ops *));

static void 
remote_prepare_to_store PARAMS ((void));

static void
remote_fetch_registers PARAMS ((int));

static void
remote_resume PARAMS ((int, int));

static int
remote_start_remote PARAMS ((char *));

static void
remote_open PARAMS ((char *, int));

static void
remote_close PARAMS ((int));

static void
remote_store_registers PARAMS ((int));

static void
getpkt PARAMS ((char *, int));

static void
putpkt PARAMS ((char *));

static void
remote_send PARAMS ((char *));

static int
readchar PARAMS ((void));

static int
remote_wait PARAMS ((WAITTYPE *));

static int
tohex PARAMS ((int));

static int
fromhex PARAMS ((int));

static void
remote_detach PARAMS ((char *, int));

extern struct target_ops remote_ops;	/* Forward decl */

static int kiodebug = 0;
/* This was 5 seconds, which is a long time to sit and wait.
   Unless this is going though some terminal server or multiplexer or
   other form of hairy serial connection, I would think 2 seconds would
   be plenty.  */
static int timeout = 2;

#if 0
int icache;
#endif

/* Descriptor for I/O to remote machine.  Initialize it to -1 so that
   remote_open knows that we don't have a file open when the program
   starts.  */
serial_t remote_desc = NULL;

#define	PBUFSIZ	1024

/* Maximum number of bytes to read/write at once.  The value here
   is chosen to fill up a packet (the headers account for the 32).  */
#define MAXBUFBYTES ((PBUFSIZ-32)/2)

/* Round up PBUFSIZ to hold all the registers, at least.  */
#if REGISTER_BYTES > MAXBUFBYTES
#undef	PBUFSIZ
#define	PBUFSIZ	(REGISTER_BYTES * 2 + 32)
#endif

/* Clean up connection to a remote debugger.  */

/* ARGSUSED */
static void
remote_close (quitting)
     int quitting;
{
  if (remote_desc)
    SERIAL_CLOSE (remote_desc);
  remote_desc = NULL;
}

/* Stub for catch_errors.  */

static int
remote_start_remote (dummy)
     char *dummy;
{
  /* Ack any packet which the remote side has already sent.  */
  SERIAL_WRITE (remote_desc, "+\r", 2);
  putpkt ("?");			/* initiate a query from remote machine */

  start_remote ();		/* Initialize gdb process mechanisms */
  return 1;
}

/* Open a connection to a remote debugger.
   NAME is the filename used for communication.  */

static void
remote_open (name, from_tty)
     char *name;
     int from_tty;
{
  if (name == 0)
    error (
"To open a remote debug connection, you need to specify what serial\n\
device is attached to the remote system (e.g. /dev/ttya).");

  target_preopen (from_tty);

  unpush_target (&remote_ops);

#if 0
  dcache_init ();
#endif

  remote_desc = SERIAL_OPEN (name);
  if (!remote_desc)
    perror_with_name (name);

  if (baud_rate)
    {
      int rate;

      if (sscanf (baud_rate, "%d", &rate) == 1)
	if (SERIAL_SETBAUDRATE (remote_desc, rate))
	  {
	    SERIAL_CLOSE (remote_desc);
	    perror_with_name (name);
	  }
    }

  SERIAL_RAW (remote_desc);

  if (from_tty)
    {
      puts_filtered ("Remote debugging using ");
      puts_filtered (name);
      puts_filtered ("\n");
    }
  push_target (&remote_ops);	/* Switch to using remote target now */

  /* Start the remote connection; if error (0), discard this target. */
  immediate_quit++;		/* Allow user to interrupt it */
  if (!catch_errors (remote_start_remote, (char *)0, 
	"Couldn't establish connection to remote target\n", RETURN_MASK_ALL))
    pop_target();
}

/* remote_detach()
   takes a program previously attached to and detaches it.
   We better not have left any breakpoints
   in the program or it'll die when it hits one.
   Close the open connection to the remote debugger.
   Use this when you want to detach and do something else
   with your gdb.  */

static void
remote_detach (args, from_tty)
     char *args;
     int from_tty;
{
  if (args)
    error ("Argument given to \"detach\" when remotely debugging.");
  
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
    error ("Reply contains invalid hex digit");
  return -1;
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

/* Tell the remote machine to resume.  */

static void
remote_resume (step, siggnal)
     int step, siggnal;
{
  char buf[PBUFSIZ];

  if (siggnal)
    {
      char *name;
      target_terminal_ours_for_output ();
      printf_filtered ("Can't send signals to a remote system.  ");
      name = strsigno (siggnal);
      if (name)
	printf_filtered (name);
      else
	printf_filtered ("Signal %d", siggnal);
      printf_filtered (" not sent.\n");
      target_terminal_inferior ();
    }

#if 0
  dcache_flush ();
#endif

  strcpy (buf, step ? "s": "c");

  putpkt (buf);
}

static void remote_interrupt_twice PARAMS ((int));
static void (*ofunc)();

/* Send ^C to target to halt it.  Target will respond, and send us a
   packet.  */

void remote_interrupt(signo)
     int signo;
{
  /* If this doesn't work, try more severe steps.  */
  signal (signo, remote_interrupt_twice);
  
  if (kiodebug)
    printf ("remote_interrupt called\n");

  SERIAL_WRITE (remote_desc, "\003", 1); /* Send a ^C */
}

/* The user typed ^C twice.  */
static void
remote_interrupt_twice (signo)
     int signo;
{
  signal (signo, ofunc);
  
  target_terminal_ours ();
  if (query ("Interrupted while waiting for the program.\n\
Give up (and stop debugging it)? "))
    {
      target_mourn_inferior ();
      return_to_top_level (RETURN_QUIT);
    }
  else
    {
      signal (signo, remote_interrupt);
      target_terminal_inferior ();
    }
}

/* Wait until the remote machine stops, then return,
   storing status in STATUS just as `wait' would.
   Returns "pid" (though it's not clear what, if anything, that
   means in the case of this target).  */

static int
remote_wait (status)
     WAITTYPE *status;
{
  unsigned char buf[PBUFSIZ];
  unsigned char *p;
  int i;
  long regno;
  char regs[MAX_REGISTER_RAW_SIZE];

  WSETEXIT ((*status), 0);

  ofunc = (void (*)()) signal (SIGINT, remote_interrupt);
  getpkt ((char *) buf, 1);
  signal (SIGINT, ofunc);

  if (buf[0] == 'E')
    error ("Remote failure reply: %s", buf);
  if (buf[0] == 'T')
    {
      /* Expedited reply, containing Signal, {regno, reg} repeat */
      /*  format is:  'Tssn...:r...;n...:r...;n...:r...;#cc', where
	  ss = signal number
	  n... = register number
	  r... = register contents
	  */

      p = &buf[3];		/* after Txx */

      while (*p)
	{
	  regno = strtol (p, &p, 16); /* Read the register number */

	  if (*p++ != ':'
	      || regno >= NUM_REGS)
	    error ("Remote sent bad register number %s", buf);

	  for (i = 0; i < REGISTER_RAW_SIZE (regno); i++)
	    {
	      if (p[0] == 0 || p[1] == 0)
		error ("Remote reply is too short: %s", buf);
	      regs[i] = fromhex (p[0]) * 16 + fromhex (p[1]);
	      p += 2;
	    }

	  if (*p++ != ';')
	    error("Remote register badly formatted: %s", buf);

	  supply_register (regno, regs);
	}
    }
  else if (buf[0] != 'S')
    error ("Invalid remote reply: %s", buf);

  WSETSTOP ((*status), (((fromhex (buf[1])) << 4) + (fromhex (buf[2]))));

  return 0;
}

/* Read the remote registers into the block REGS.  */
/* Currently we just read all the registers, so we don't use regno.  */
/* ARGSUSED */
static void
remote_fetch_registers (regno)
     int regno;
{
  char buf[PBUFSIZ];
  int i;
  char *p;
  char regs[REGISTER_BYTES];

  sprintf (buf, "g");
  remote_send (buf);

  /* Reply describes registers byte by byte, each byte encoded as two
     hex characters.  Suck them all up, then supply them to the
     register cacheing/storage mechanism.  */

  p = buf;
  for (i = 0; i < REGISTER_BYTES; i++)
    {
      if (p[0] == 0 || p[1] == 0)
	error ("Remote reply is too short: %s", buf);
      regs[i] = fromhex (p[0]) * 16 + fromhex (p[1]);
      p += 2;
    }
  for (i = 0; i < NUM_REGS; i++)
    supply_register (i, &regs[REGISTER_BYTE(i)]);
}

/* Prepare to store registers.  Since we send them all, we have to
   read out the ones we don't want to change first.  */

static void 
remote_prepare_to_store ()
{
  /* Make sure the entire registers array is valid.  */
  read_register_bytes (0, (char *)NULL, REGISTER_BYTES);
}

/* Store the remote registers from the contents of the block REGISTERS. 
   FIXME, eventually just store one register if that's all that is needed.  */

/* ARGSUSED */
static void
remote_store_registers (regno)
     int regno;
{
  char buf[PBUFSIZ];
  int i;
  char *p;

  buf[0] = 'G';
  
  /* Command describes registers byte by byte,
     each byte encoded as two hex characters.  */

  p = buf + 1;
  for (i = 0; i < REGISTER_BYTES; i++)
    {
      *p++ = tohex ((registers[i] >> 4) & 0xf);
      *p++ = tohex (registers[i] & 0xf);
    }
  *p = '\0';

  remote_send (buf);
}

#if 0
/* Read a word from remote address ADDR and return it.
   This goes through the data cache.  */

int
remote_fetch_word (addr)
     CORE_ADDR addr;
{
  if (icache)
    {
      extern CORE_ADDR text_start, text_end;

      if (addr >= text_start && addr < text_end)
	{
	  int buffer;
	  xfer_core_file (addr, &buffer, sizeof (int));
	  return buffer;
	}
    }
  return dcache_fetch (addr);
}

/* Write a word WORD into remote address ADDR.
   This goes through the data cache.  */

void
remote_store_word (addr, word)
     CORE_ADDR addr;
     int word;
{
  dcache_poke (addr, word);
}
#endif /* 0 */

/* Write memory data directly to the remote machine.
   This does not inform the data cache; the data cache uses this.
   MEMADDR is the address in the remote memory space.
   MYADDR is the address of the buffer in our space.
   LEN is the number of bytes.  */

static void
remote_write_bytes (memaddr, myaddr, len)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
{
  char buf[PBUFSIZ];
  int i;
  char *p;

  if (len > PBUFSIZ / 2 - 20)
    abort ();

  sprintf (buf, "M%x,%x:", memaddr, len);

  /* We send target system values byte by byte, in increasing byte addresses,
     each byte encoded as two hex characters.  */

  p = buf + strlen (buf);
  for (i = 0; i < len; i++)
    {
      *p++ = tohex ((myaddr[i] >> 4) & 0xf);
      *p++ = tohex (myaddr[i] & 0xf);
    }
  *p = '\0';

  remote_send (buf);
}

/* Read memory data directly from the remote machine.
   This does not use the data cache; the data cache uses this.
   MEMADDR is the address in the remote memory space.
   MYADDR is the address of the buffer in our space.
   LEN is the number of bytes.  */

static void
remote_read_bytes (memaddr, myaddr, len)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
{
  char buf[PBUFSIZ];
  int i;
  char *p;

  if (len > PBUFSIZ / 2 - 1)
    abort ();

  sprintf (buf, "m%x,%x", memaddr, len);
  remote_send (buf);

  /* Reply describes memory byte by byte,
     each byte encoded as two hex characters.  */

  p = buf;
  for (i = 0; i < len; i++)
    {
      if (p[0] == 0 || p[1] == 0)
	error ("Remote reply is too short: %s", buf);
      myaddr[i] = fromhex (p[0]) * 16 + fromhex (p[1]);
      p += 2;
    }
}

/* Read or write LEN bytes from inferior memory at MEMADDR, transferring
   to or from debugger address MYADDR.  Write to inferior if SHOULD_WRITE is
   nonzero.  Returns length of data written or read; 0 for error.  */

/* ARGSUSED */
static int
remote_xfer_memory(memaddr, myaddr, len, should_write, target)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
     int should_write;
     struct target_ops *target;			/* ignored */
{
  int origlen = len;
  int xfersize;
  while (len > 0)
    {
      if (len > MAXBUFBYTES)
	xfersize = MAXBUFBYTES;
      else
	xfersize = len;

      if (should_write)
        remote_write_bytes(memaddr, myaddr, xfersize);
      else
	remote_read_bytes (memaddr, myaddr, xfersize);
      memaddr += xfersize;
      myaddr  += xfersize;
      len     -= xfersize;
    }
  return origlen; /* no error possible */
}

static void
remote_files_info (ignore)
struct target_ops *ignore;
{
  puts_filtered ("Debugging a target over a serial line.\n");
}

/* Stuff for dealing with the packets which are part of this protocol.
   See comment at top of file for details.  */

/* Read a single character from the remote end, masking it down to 7 bits. */

static int
readchar ()
{
  int ch;

  ch = SERIAL_READCHAR (remote_desc, timeout);

  if (ch < 0)
    return ch;

  return ch & 0x7f;
}

/* Send the command in BUF to the remote machine,
   and read the reply into BUF.
   Report an error if we get an error reply.  */

static void
remote_send (buf)
     char *buf;
{

  putpkt (buf);
  getpkt (buf, 0);

  if (buf[0] == 'E')
    error ("Remote failure reply: %s", buf);
}

/* Send a packet to the remote machine, with error checking.
   The data of the packet is in BUF.  */

static void
putpkt (buf)
     char *buf;
{
  int i;
  unsigned char csum = 0;
  char buf2[PBUFSIZ];
  int cnt = strlen (buf);
  int ch;
  char *p;

  /* Copy the packet into buffer BUF2, encapsulating it
     and giving it a checksum.  */

  if (cnt > sizeof(buf2) - 5)		/* Prosanity check */
    abort();

  p = buf2;
  *p++ = '$';

  for (i = 0; i < cnt; i++)
    {
      csum += buf[i];
      *p++ = buf[i];
    }
  *p++ = '#';
  *p++ = tohex ((csum >> 4) & 0xf);
  *p++ = tohex (csum & 0xf);

  /* Send it over and over until we get a positive ack.  */

  while (1)
    {
      if (kiodebug)
	{
	  *p = '\0';
	  printf ("Sending packet: %s...", buf2);  fflush(stdout);
	}
      if (SERIAL_WRITE (remote_desc, buf2, p - buf2))
	perror_with_name ("putpkt: write failed");

      /* read until either a timeout occurs (-2) or '+' is read */
      while (1)
	{
	  ch = readchar ();

	  switch (ch)
	    {
	    case '+':
	      if (kiodebug)
		printf("Ack\n");
	      return;
	    case SERIAL_TIMEOUT:
	      break;		/* Retransmit buffer */
	    case SERIAL_ERROR:
	      perror_with_name ("putpkt: couldn't read ACK");
	    case SERIAL_EOF:
	      error ("putpkt: EOF while trying to read ACK");
	    default:
	      if (kiodebug)
		printf ("%02X %c ", ch&0xFF, ch);
	      continue;
	    }
	  break;		/* Here to retransmit */
	}
    }
}

/* Read a packet from the remote machine, with error checking,
   and store it in BUF.  BUF is expected to be of size PBUFSIZ.
   If FOREVER, wait forever rather than timing out; this is used
   while the target is executing user code.  */

static void
getpkt (buf, forever)
     char *buf;
     int forever;
{
  char *bp;
  unsigned char csum;
  int c = 0;
  unsigned char c1, c2;
  int retries = 0;
#define MAX_RETRIES	10

  while (1)
    {
      /* This can loop forever if the remote side sends us characters
	 continuously, but if it pauses, we'll get a zero from readchar
	 because of timeout.  Then we'll count that as a retry.  */

      c = readchar();
      if (c > 0 && c != '$')
	continue;

      if (c == SERIAL_TIMEOUT)
	{
	  if (forever)
	    continue;
	  if (++retries >= MAX_RETRIES)
	    if (kiodebug) puts_filtered ("Timed out.\n");
	  goto out;
	}

      if (c == SERIAL_EOF)
	error ("Remote connection closed");
      if (c == SERIAL_ERROR)
	perror_with_name ("Remote communication error");

      /* Force csum to be zero here because of possible error retry.  */
      csum = 0;
      bp = buf;

      while (1)
	{
	  c = readchar ();
	  if (c == SERIAL_TIMEOUT)
	    {
	      if (kiodebug)
		puts_filtered ("Timeout in mid-packet, retrying\n");
	      goto whole;		/* Start a new packet, count retries */
	    } 
	  if (c == '$')
	    {
	      if (kiodebug)
		puts_filtered ("Saw new packet start in middle of old one\n");
	      goto whole;		/* Start a new packet, count retries */
	    }
	  if (c == '#')
	    break;
	  if (bp >= buf+PBUFSIZ-1)
	  {
	    *bp = '\0';
	    puts_filtered ("Remote packet too long: ");
	    puts_filtered (buf);
	    puts_filtered ("\n");
	    goto whole;
	  }
	  *bp++ = c;
	  csum += c;
	}
      *bp = 0;

      c1 = fromhex (readchar ());
      c2 = fromhex (readchar ());
      if ((csum & 0xff) == (c1 << 4) + c2)
	break;
      printf_filtered ("Bad checksum, sentsum=0x%x, csum=0x%x, buf=",
	      (c1 << 4) + c2, csum & 0xff);
      puts_filtered (buf);
      puts_filtered ("\n");

      /* Try the whole thing again.  */
whole:
      if (++retries < MAX_RETRIES)
	{
	  SERIAL_WRITE (remote_desc, "-", 1);
	}
      else
	{
	  printf ("Ignoring packet error, continuing...\n");
	  break;
	}
    }

out:

  SERIAL_WRITE (remote_desc, "+", 1);

  if (kiodebug)
    fprintf (stderr,"Packet received: %s\n", buf);
}

/* The data cache leads to incorrect results because it doesn't know about
   volatile variables, thus making it impossible to debug functions which
   use hardware registers.  Therefore it is #if 0'd out.  Effect on
   performance is some, for backtraces of functions with a few
   arguments each.  For functions with many arguments, the stack
   frames don't fit in the cache blocks, which makes the cache less
   helpful.  Disabling the cache is a big performance win for fetching
   large structures, because the cache code fetched data in 16-byte
   chunks.  */
#if 0
/* The data cache records all the data read from the remote machine
   since the last time it stopped.

   Each cache block holds 16 bytes of data
   starting at a multiple-of-16 address.  */

#define DCACHE_SIZE 64		/* Number of cache blocks */

struct dcache_block {
	struct dcache_block *next, *last;
	unsigned int addr;	/* Address for which data is recorded.  */
	int data[4];
};

struct dcache_block dcache_free, dcache_valid;

/* Free all the data cache blocks, thus discarding all cached data.  */ 

static void
dcache_flush ()
{
  register struct dcache_block *db;

  while ((db = dcache_valid.next) != &dcache_valid)
    {
      remque (db);
      insque (db, &dcache_free);
    }
}

/*
 * If addr is present in the dcache, return the address of the block 
 * containing it.
 */

struct dcache_block *
dcache_hit (addr)
{
  register struct dcache_block *db;

  if (addr & 3)
    abort ();

  /* Search all cache blocks for one that is at this address.  */
  db = dcache_valid.next;
  while (db != &dcache_valid)
    {
      if ((addr & 0xfffffff0) == db->addr)
	return db;
      db = db->next;
    }
  return NULL;
}

/*  Return the int data at address ADDR in dcache block DC.  */

int
dcache_value (db, addr)
     struct dcache_block *db;
     unsigned int addr;
{
  if (addr & 3)
    abort ();
  return (db->data[(addr>>2)&3]);
}

/* Get a free cache block, put it on the valid list,
   and return its address.  The caller should store into the block
   the address and data that it describes.  */

struct dcache_block *
dcache_alloc ()
{
  register struct dcache_block *db;

  if ((db = dcache_free.next) == &dcache_free)
    /* If we can't get one from the free list, take last valid */
    db = dcache_valid.last;

  remque (db);
  insque (db, &dcache_valid);
  return (db);
}

/* Return the contents of the word at address ADDR in the remote machine,
   using the data cache.  */

int
dcache_fetch (addr)
     CORE_ADDR addr;
{
  register struct dcache_block *db;

  db = dcache_hit (addr);
  if (db == 0)
    {
      db = dcache_alloc ();
      remote_read_bytes (addr & ~0xf, db->data, 16);
      db->addr = addr & ~0xf;
    }
  return (dcache_value (db, addr));
}

/* Write the word at ADDR both in the data cache and in the remote machine.  */

dcache_poke (addr, data)
     CORE_ADDR addr;
     int data;
{
  register struct dcache_block *db;

  /* First make sure the word is IN the cache.  DB is its cache block.  */
  db = dcache_hit (addr);
  if (db == 0)
    {
      db = dcache_alloc ();
      remote_read_bytes (addr & ~0xf, db->data, 16);
      db->addr = addr & ~0xf;
    }

  /* Modify the word in the cache.  */
  db->data[(addr>>2)&3] = data;

  /* Send the changed word.  */
  remote_write_bytes (addr, &data, 4);
}

/* Initialize the data cache.  */

dcache_init ()
{
  register i;
  register struct dcache_block *db;

  db = (struct dcache_block *) xmalloc (sizeof (struct dcache_block) * 
					DCACHE_SIZE);
  dcache_free.next = dcache_free.last = &dcache_free;
  dcache_valid.next = dcache_valid.last = &dcache_valid;
  for (i=0;i<DCACHE_SIZE;i++,db++)
    insque (db, &dcache_free);
}
#endif /* 0 */

static void
remote_kill ()
{
  putpkt ("k");
  /* Don't wait for it to die.  I'm not really sure it matters whether
     we do or not.  For the existing stubs, kill is a noop.  */
  target_mourn_inferior ();
}

static void
remote_mourn ()
{
  unpush_target (&remote_ops);
  generic_mourn_inferior ();
}

/* Define the target subroutine names */

struct target_ops remote_ops = {
  "remote",			/* to_shortname */
  "Remote serial target in gdb-specific protocol",	/* to_longname */
  "Use a remote computer via a serial line, using a gdb-specific protocol.\n\
Specify the serial device it is connected to (e.g. /dev/ttya).",  /* to_doc */
  remote_open,			/* to_open */
  remote_close,			/* to_close */
  NULL,				/* to_attach */
  remote_detach,		/* to_detach */
  remote_resume,		/* to_resume */
  remote_wait,			/* to_wait */
  remote_fetch_registers,	/* to_fetch_registers */
  remote_store_registers,	/* to_store_registers */
  remote_prepare_to_store,	/* to_prepare_to_store */
  remote_xfer_memory,		/* to_xfer_memory */
  remote_files_info,		/* to_files_info */
  NULL,				/* to_insert_breakpoint */
  NULL,				/* to_remove_breakpoint */
  NULL,				/* to_terminal_init */
  NULL,				/* to_terminal_inferior */
  NULL,				/* to_terminal_ours_for_output */
  NULL,				/* to_terminal_ours */
  NULL,				/* to_terminal_info */
  remote_kill,			/* to_kill */
  generic_load,			/* to_load */
  NULL,				/* to_lookup_symbol */
  NULL,				/* to_create_inferior */
  remote_mourn,			/* to_mourn_inferior */
  0,				/* to_can_run */
  0,				/* to_notice_signals */
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
_initialize_remote ()
{
  add_target (&remote_ops);

  add_show_from_set (
    add_set_cmd ("remotedebug", no_class, var_boolean, (char *)&kiodebug,
		   "Set debugging of remote serial I/O.\n\
When enabled, each packet sent or received with the remote target\n\
is displayed.", &setlist),
	&showlist);
}

#endif
