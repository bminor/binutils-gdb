/* Remote target communications for serial-line targets in custom GDB protocol
   Copyright 1988, 1991, 1992 Free Software Foundation, Inc.

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

	or...		TAAPPPPPPPPFFFFFFFF
					where AA is the signal number,
					PPPPPPPP is the PC (PC_REGNUM), and
					FFFFFFFF is the frame ptr (FP_REGNUM).

	kill req	k
*/

#include "defs.h"
#include <string.h>
#include "serial.h"
#include "frame.h"
#include "inferior.h"
#include "target.h"
#include "wait.h"
#include "terminal.h"
#include "gdbcmd.h"

#include <signal.h>

static int kiodebug = 0;
static int timeout = 5;

#define	PBUFSIZ	1024

/* Maximum number of bytes to read/write at once.  The value here
   is chosen to fill up a packet (the headers account for the 32).  */
#define MAXBUFBYTES ((PBUFSIZ-32)/2)

/* Round up PBUFSIZ to hold all the registers, at least.  */
#if REGISTER_BYTES > MAXBUFBYTES
#undef	PBUFSIZ
#define	PBUFSIZ	(REGISTER_BYTES * 2 + 32)
#endif

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
    printf ("Ending remote debugging.\n");
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
  char ch;
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

  do {
    if (kiodebug)
      {
	*p = '\0';
	printf ("Sending packet: %s...", buf2);  fflush(stdout);
      }
    serial_write (buf2, p - buf2);

    /* read until either a timeout occurs (\0) or '+' is read */
    do {
      ch = readchar ();
      if (kiodebug) {
	if (ch == '+')
	  printf("Ack\n");
	else
	  printf ("%02X%c ", ch&0xFF, ch);
      }
    } while ((ch != '+') && (ch != '\0'));
  } while (ch != '+');
}

/* Read a packet from the remote machine, with error checking,
   and store it in BUF.  BUF is expected to be of size PBUFSIZ.  */

static void
getpkt (buf)
     char *buf;
{
  char *bp;
  unsigned char csum;
  int c;
  unsigned char c1, c2;

  while (1)
    {
      /* Force csum to be zero here because of possible error retry.  */
      csum = 0;
      
      while ((c = readchar()) != '$');

      bp = buf;
      while (1)
	{
	  c = readchar ();
	  if (c == '#')
	    break;
	  if (bp >= buf+PBUFSIZ-1)
	  {
	    *bp = '\0';
	    printf_filtered ("Remote packet too long: %s\n", buf);
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
      printf_filtered ("Bad checksum, sentsum=0x%x, csum=0x%x, buf=%s\n",
	      (c1 << 4) + c2, csum & 0xff, buf);
      /* Try the whole thing again.  */
whole:
      serial_write ("-", 1);
    }

#if 0
  immediate_quit--;
#endif

  serial_write ("+", 1);

  if (kiodebug)
    fprintf (stderr,"Packet received: %s\n", buf);
}

static void
remote_resume (step, siggnal)
     int step, siggnal;
{
  char buf[PBUFSIZ];

  if (siggnal)
    error ("Can't send signals to a remote system.  Try `handle %d ignore'.",
	   siggnal);

#if 0
  dcache_flush ();
#endif

  strcpy (buf, step ? "s": "c");

  putpkt (buf);
}

/* Send ^C to target to halt it.  Target will respond, and send us a
   packet.  */

void remote_interrupt(signo)
     int signo;
{
  
  if (kiodebug)
    printf ("remote_interrupt called\n");

  serial_write ("\003", 1);	/* Send a ^C */
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
  void (*ofunc)();
  unsigned char *p;
  int i;
  char regs[REGISTER_RAW_SIZE (PC_REGNUM) + REGISTER_RAW_SIZE (FP_REGNUM)];

  WSETEXIT ((*status), 0);

  ofunc = (void (*)()) signal (SIGINT, remote_interrupt);
  getpkt ((char *) buf);
  signal (SIGINT, ofunc);

  if (buf[0] == 'E')
    error ("Remote failure reply: %s", buf);
  if (buf[0] == 'T')
    {
      /* Expedited reply, containing Signal, PC, and FP.  */
      p = &buf[3];		/* after Txx */
      for (i = 0; i < sizeof (regs); i++)
	{
	  if (p[0] == 0 || p[1] == 0)
	    error ("Remote reply is too short: %s", buf);
	  regs[i] = fromhex (p[0]) * 16 + fromhex (p[1]);
	  p += 2;
	}
      supply_register (PC_REGNUM, &regs[0]);
      supply_register (FP_REGNUM, &regs[REGISTER_RAW_SIZE (PC_REGNUM)]);
    }
  else if (buf[0] != 'S')
    error ("Invalid remote reply: %s", buf);

  WSETSTOP ((*status), (((fromhex (buf[1])) << 4) + (fromhex (buf[2]))));

  return 0;
}

/* Send the command in BUF to the remote machine,
   and read the reply into BUF.
   Report an error if we get an error reply.  */

static void
remote_send (buf)
     char *buf;
{

  putpkt (buf);
  getpkt (buf);

  if (buf[0] == 'E')
    error ("Remote failure reply: %s", buf);
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
  remote_fetch_registers (-1);
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
  printf ("Debugging a target over a serial line.\n");
}

/*

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

*/

/* Read a single character from the remote end.
   (If supported, we actually read many characters and buffer them up.)  */

static int
readchar ()
{
  static int inbuf_index, inbuf_count;
#define	INBUFSIZE	PBUFSIZ
  static char inbuf[INBUFSIZE];

  if (inbuf_index >= inbuf_count)
    {
      /* Time to do another read... */
      inbuf_index = 0;
      inbuf_count = 0;
      inbuf[0] = 0;		/* Just in case */
      if ((inbuf[inbuf_index] = serial_readchar (timeout)) < 0)
	inbuf_count = -1;
      else
	inbuf_count = 1;
    }

  /* Just return the next character from the buffer.  */
  return inbuf[inbuf_index++] & 0x7f;
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

/* Define the target subroutine names */

struct target_ops remote_ops = {
  "remote-ser",			/* to_shortname */
  "Remote serial target in gdb-specific protocol with serial-packaging routines",	/* to_longname */
  "Use a remote computer via a serial line, using a gdb-specific protocol.\n\
Specify the serial device it is connected to (e.g. /dev/ttya).",  /* to_doc */
  serial_open,			/* to_open */
  serial_close,			/* to_close */
  NULL,				/* to_attach */
  remote_detach,		/* to_detach */
  remote_resume,		/* to_resume */
  remote_wait,			/* to_wait */
  remote_fetch_registers,	/* to_fetch_registers */
  remote_store_registers,	/* to_store_registers */
  remote_prepare_to_store,	/* to_prepare_to_store */
  NULL,				/* to_convert_to_virtual */
  NULL,				/* to_convert_from_virtual */
  remote_xfer_memory,		/* to_xfer_memory */
  remote_files_info,		/* to_files_info */
  NULL,				/* to_insert_breakpoint */
  NULL,				/* to_remove_breakpoint */
  NULL,				/* to_terminal_init */
  NULL,				/* to_terminal_inferior */
  NULL,				/* to_terminal_ours_for_output */
  NULL,				/* to_terminal_ours */
  NULL,				/* to_terminal_info */
  NULL,				/* to_kill */
  NULL,				/* to_load */
  NULL,				/* to_lookup_symbol */
  NULL,				/* to_create_inferior */
  NULL,				/* to_mourn_inferior */
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

