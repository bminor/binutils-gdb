/* Remote debugging interface for Motorola's MVME187BUG monitor, an embedded
   monitor for the m88k.

   Copyright 1992, 1993 Free Software Foundation, Inc.
   Contributed by Cygnus Support.  Written by K. Richard Pixley.

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
#include "wait.h"
#include "value.h"

#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <signal.h>
#include <setjmp.h>
#include <errno.h>

#include "terminal.h"
#include "target.h"
#include "gdbcore.h"
#include "serial.h"
#include "gdbcmd.h"

#include "dcache.h"

extern int sleep();
extern int bcopy();

/* External data declarations */
extern int stop_soon_quietly;	/* for wait_for_inferior */

/* Forward data declarations */
extern struct target_ops bug_ops;	/* Forward declaration */

/* Forward function declarations */

static int bug_clear_breakpoints PARAMS((void));
static void bug_close PARAMS((int quitting));
static void bug_write_cr PARAMS((char *string));

static int bug_read_inferior_memory PARAMS((CORE_ADDR memaddr,
					    unsigned char *myaddr,
					    int len));

static int bug_write_inferior_memory PARAMS((CORE_ADDR memaddr,
					     unsigned char *myaddr,
					     int len));

/* This is the serial descriptor to our target.  */

static serial_t desc = NULL;

/* This is our data cache. */

static DCACHE *bug_dcache;

/* This variable is somewhat arbitrary.  It's here so that it can be
   set from within a running gdb.  */

static int srec_max_retries = 3;

/* Each S-record download to the target consists of an S0 header
   record, some number of S3 data records, and one S7 termination
   record.  I call this download a "frame".  Srec_frame says how many
   bytes will be represented in each frame.  */

static int srec_frame = 160;

/* This variable determines how many bytes will be represented in each
   S3 s-record.  */

static int srec_bytes = 40;

/* At one point it appeared to me as though the bug monitor could not
   really be expected to receive two sequential characters at 9600
   baud reliably.  Echo-pacing is an attempt to force data across the
   line even in this condition.  Specifically, in echo-pace mode, each
   character is sent one at a time and we look for the echo before
   sending the next.  This is excruciatingly slow.  */

static int srec_echo_pace = 0;

/* How long to wait after an srec for a possible error message.
   Similar to the above, I tried sleeping after sending each S3 record
   in hopes that I might actually see error messages from the bug
   monitor.  This might actually work if we were to use sleep
   intervals smaller than 1 second.  */

static int srec_sleep = 0;

/* Every srec_noise records, flub the checksum.  This is a debugging
   feature.  Set the variable to something other than 1 in order to
   inject *deliberate* checksum errors.  One might do this if one
   wanted to test error handling and recovery.  */

static int srec_noise = 0;

/***********************************************************************
 * I/O stuff stolen from remote-eb.c
 ***********************************************************************/

/* with a timeout of 2, we time out waiting for the prompt after an
   s-record dump. */
static int timeout = 4;

static const char *dev_name;

/* Descriptor for I/O to remote machine.  Initialize it to -1 so that
   bug_open knows that we don't have a file open when the program
   starts.  */

static int is_open = 0;
static int
check_open ()
{
  if (!is_open)
    {
      error ("remote device not open");
    }

  return(1);
}

#define ON	1
#define OFF	0

/* Read a character from the remote system, doing all the fancy
   timeout stuff.  */
static int
readchar ()
{
  int buf;

  buf = SERIAL_READCHAR (desc, timeout);

  if (buf == SERIAL_TIMEOUT)
    error ("Timeout reading from remote system.");

  if (remote_debug)
    printf ("%c", buf);

  return buf & 0x7f;
}

static int
readchar_nofail ()
{
  int buf;

  buf = SERIAL_READCHAR (desc, timeout);
  if (buf == SERIAL_TIMEOUT)
    buf = 0;
  if (remote_debug)
    if (buf)
      printf ("%c", buf);
    else
      printf ("<timeout>");

  return buf & 0x7f;

}

static int
pollchar()
{
  int buf;

  buf = SERIAL_READCHAR (desc, 0);
  if (buf == SERIAL_TIMEOUT)
    buf = 0;
  if (remote_debug)
    if (buf)
      printf ("%c", buf);
    else
      printf ("<empty poll>");

  return buf & 0x7f;
}

/* Keep discarding input from the remote system, until STRING is found.
   Let the user break out immediately.  */
static void
expect (string)
     char *string;
{
  char *p = string;

  immediate_quit = 1;
  for (;;)
    {
      if (readchar () == *p)
	{
	  p++;
	  if (*p == '\0')
	    {
	      immediate_quit = 0;
	      return;
	    }
	}
      else
	p = string;
    }
}

/* Keep discarding input until we see the bug prompt.

   The convention for dealing with the prompt is that you
   o give your command
   o *then* wait for the prompt.

   Thus the last thing that a procedure does with the serial line
   will be an expect_prompt().  Exception:  bug_resume does not
   wait for the prompt, because the terminal is being handed over
   to the inferior.  However, the next thing which happens after that
   is a bug_wait which does wait for the prompt.
   Note that this includes abnormal exit, e.g. error().  This is
   necessary to prevent getting into states from which we can't
   recover.  */
static void
expect_prompt ()
{
  expect ("Bug>");
}

/* Get a hex digit from the remote system & return its value.
   If ignore_space is nonzero, ignore spaces (not newline, tab, etc).  */
static int
get_hex_digit (ignore_space)
     int ignore_space;
{
  int ch;

  for (;;)
    {
      ch = readchar ();
      if (ch >= '0' && ch <= '9')
	return ch - '0';
      else if (ch >= 'A' && ch <= 'F')
	return ch - 'A' + 10;
      else if (ch >= 'a' && ch <= 'f')
	return ch - 'a' + 10;
      else if (ch != ' ' || !ignore_space)
	{
	  expect_prompt ();
	  error ("Invalid hex digit from remote system.");
	}
    }
}

/* Get a byte from bug_desc and put it in *BYT.  Accept any number
   leading spaces.  */
static void
get_hex_byte (byt)
     char *byt;
{
  int val;

  val = get_hex_digit (1) << 4;
  val |= get_hex_digit (0);
  *byt = val;
}

/* Read a 32-bit hex word from the bug, preceded by a space  */
static long
get_hex_word ()
{
  long val;
  int j;

  val = 0;
  for (j = 0; j < 8; j++)
    val = (val << 4) + get_hex_digit (j == 0);
  return val;
}

/* Called when SIGALRM signal sent due to alarm() timeout.  */

/* Number of SIGTRAPs we need to simulate.  That is, the next
   NEED_ARTIFICIAL_TRAP calls to bug_wait should just return
   SIGTRAP without actually waiting for anything.  */

static int need_artificial_trap = 0;

void
bug_kill (arg, from_tty)
     char *arg;
     int from_tty;
{

}

/*
 * Download a file specified in 'args', to the bug.
 */

static void
bug_load (args, fromtty)
     char *args;
     int fromtty;
{
  bfd *abfd;
  asection *s;
  char buffer[1024];

  check_open ();

  dcache_flush (bug_dcache);
  inferior_pid = 0;
  abfd = bfd_openr (args, 0);
  if (!abfd)
    {
      printf_filtered ("Unable to open file %s\n", args);
      return;
    }

  if (bfd_check_format (abfd, bfd_object) == 0)
    {
      printf_filtered ("File is not an object file\n");
      return;
    }

  s = abfd->sections;
  while (s != (asection *) NULL)
    {
      if (s->flags & SEC_LOAD)
	{
	  int i;

	  char *buffer = xmalloc (srec_frame);

	  printf_filtered ("%s\t: 0x%4x .. 0x%4x  ", s->name, s->vma, s->vma + s->_raw_size);
	  fflush (stdout);
	  for (i = 0; i < s->_raw_size; i += srec_frame)
	    {
	      if (srec_frame > s->_raw_size - i)
		srec_frame = s->_raw_size - i;

	      bfd_get_section_contents (abfd, s, buffer, i, srec_frame);
	      bug_write_inferior_memory (s->vma + i, buffer, srec_frame);
	      printf_filtered ("*");
	      fflush (stdout);
	    }
	  printf_filtered ("\n");
	  free (buffer);
	}
      s = s->next;
    }
  sprintf (buffer, "rs ip %lx", (unsigned long) abfd->start_address);
  bug_write_cr (buffer);
  expect_prompt ();
}

/* This is called not only when we first attach, but also when the
   user types "run" after having attached.  */
void
bug_create_inferior (execfile, args, env)
     char *execfile;
     char *args;
     char **env;
{
  int entry_pt;

  if (args && *args)
    error ("Can't pass arguments to remote bug process.");

  if (execfile == 0 || exec_bfd == 0)
    error ("No exec file specified");

  entry_pt = (int) bfd_get_start_address (exec_bfd);
  check_open ();

  bug_kill (NULL, NULL);
  bug_clear_breakpoints ();
  init_wait_for_inferior ();
  bug_write_cr ("");
  expect_prompt ();

  insert_breakpoints ();	/* Needed to get correct instruction in cache */
  proceed (entry_pt, -1, 0);
}

static char *
get_word (p)
     char **p;
{
  char *s = *p;
  char *word;
  char *copy;
  size_t len;

  while (isspace (*s))
    s++;

  word = s;

  len = 0;

  while (*s && !isspace (*s))
    {
      s++;
      len++;

    }
  copy = xmalloc (len + 1);
  memcpy (copy, word, len);
  copy[len] = 0;
  *p = s;
  return copy;
}

static int baudrate = 9600;

static void
bug_open (name, from_tty)
     char *name;
     int from_tty;
{
  push_target (&bug_ops);

  if (name == 0)
    {
      name = "";
    }
  if (is_open)
    bug_close (0);
  dev_name = strdup (name);

  if (!(desc = SERIAL_OPEN (dev_name)))
    perror_with_name ((char *) dev_name);

  SERIAL_RAW (desc);
  is_open = 1;

  bug_dcache = dcache_init (bug_read_inferior_memory, bug_write_inferior_memory);

  /* Hello?  Are you there?  */
  SERIAL_WRITE (desc, "\r", 1);
  expect_prompt ();

  /* Clear any break points */
  bug_clear_breakpoints ();

  printf_filtered ("Connected to remote 187bug system.\n");
}

/* Close out all files and local state before this target loses control. */

static void
bug_close (quitting)
     int quitting;
{
  /* Clear any break points */
  bug_clear_breakpoints ();

  if (is_open)
    SERIAL_CLOSE (desc);

  is_open = 0;
}

/* Terminate the open connection to the remote debugger.
   Use this when you want to detach and do something else
   with your gdb.  */
void
bug_detach (args, from_tty)
     char *args;
     int from_tty;
{
  if (is_open)
    bug_clear_breakpoints ();

  pop_target ();		/* calls bug_close to do the real work */

  if (from_tty)
    printf_filtered ("Ending remote %s debugging\n", target_shortname);
}

/* Tell the remote machine to resume.  */

void
bug_resume (pid, step, sig)
     int pid, step, sig;
{
  dcache_flush (bug_dcache);

  if (step)
    {
      bug_write_cr("t");

      /* Force the next bug_wait to return a trap.  Not doing anything
       about I/O from the target means that the user has to type
       "continue" to see any.  FIXME, this should be fixed.  */
      need_artificial_trap = 1;
    }
  else
      bug_write_cr ("g");

  return;
}

/* Given a null terminated list of strings LIST, read the input until we find one of
   them.  Return the index of the string found or -1 on error.  '?' means match
   any single character. Note that with the algorithm we use, the initial
   character of the string cannot recur in the string, or we will not find some
   cases of the string in the input.  If PASSTHROUGH is non-zero, then
   pass non-matching data on.  */

static int
multi_scan (list, passthrough)
     char *list[];
     int passthrough;
{
  char *swallowed = NULL; /* holding area */
  char *swallowed_p = swallowed; /* Current position in swallowed.  */
  int ch;
  int ch_handled;
  int i;
  int string_count;
  int max_length;
  char **plist;

  /* Look through the strings.  Count them.  Find the largest one so we can
     allocate a holding area.  */

  for (max_length = string_count = i = 0;
       list[i] != NULL;
       ++i, ++string_count)
    {
      int length = strlen(list[i]);

      if (length > max_length)
	max_length = length;
    }

  /* if we have no strings, then something is wrong. */
  if (string_count == 0)
    return(-1);

  /* otherwise, we will need a holding area big enough to hold almost two
     copies of our largest string.  */
  swallowed_p = swallowed = alloca(max_length << 1);

  /* and a list of pointers to current scan points. */
  plist = alloca(string_count * sizeof(*plist));

  /* and initialize */
  for (i = 0; i < string_count; ++i)
    plist[i] = list[i];

  for (ch = readchar(); /* loop forever */ ; ch = readchar())
    {
      QUIT; /* Let user quit and leave process running */
      ch_handled = 0;

      for (i = 0; i < string_count; ++i)
	{
	  if (ch == *plist[i] || *plist[i] == '?')
	    {
	      ++plist[i];
	      if (*plist[i] == '\0')
		return(i);

	      if (!ch_handled)
		*swallowed_p++ = ch;

	      ch_handled = 1;
	    }
	  else
	    plist[i] = list[i];
	}

      if (!ch_handled)
	{
	  char *p;

	  /* Print out any characters which have been swallowed.  */
	  if (passthrough)
	    {
	      for (p = swallowed; p < swallowed_p; ++p)
		putc (*p, stdout);

	      putc (ch, stdout);
	    }

	  swallowed_p = swallowed;
	}
    }

  return(-1);
}

/* Wait until the remote machine stops, then return,
   storing status in STATUS just as `wait' would.  */

static char *wait_strings[] = {
  "At Breakpoint",
  "Exception: Data Access Fault (Local Bus Timeout)",
  "\r8???-Bug>",
  NULL,
};

int
bug_wait (status)
     WAITTYPE *status;
{
  int old_timeout = timeout;
  int old_immediate_quit = immediate_quit;

  WSETEXIT ((*status), 0);

  /* read off leftovers from resume so that the rest can be passed
     back out as stdout.  */
  if (need_artificial_trap == 0)
    {
      expect("Effective address: ");
      (void) get_hex_word();
      expect ("\r\n");
    }

  timeout = -1;	/* Don't time out -- user program is running. */
  immediate_quit = 1; /* Helps ability to QUIT */

  switch (multi_scan(wait_strings, need_artificial_trap == 0))
    {
    case 0: /* breakpoint case */
      WSETSTOP ((*status), SIGTRAP);
      /* user output from the target can be discarded here. (?) */
      expect_prompt();
      break;

    case 1: /* bus error */
      WSETSTOP ((*status), SIGBUS);
      /* user output from the target can be discarded here. (?) */
      expect_prompt();
      break;

    case 2: /* normal case */
      if (need_artificial_trap != 0)
	{
	  /* stepping */
	  WSETSTOP ((*status), SIGTRAP);
	  need_artificial_trap--;
	  break;
	}
      else
	{
	  /* exit case */
	  WSETEXIT ((*status), 0);
	  break;
	}

    case -1: /* trouble */
    default:
      fprintf_filtered (stderr,
			"Trouble reading target during wait\n");
      break;
    }

  timeout = old_timeout;
  immediate_quit = old_immediate_quit;
  return 0;
}

/* Return the name of register number REGNO
   in the form input and output by bug.

   Returns a pointer to a static buffer containing the answer.  */
static char *
get_reg_name (regno)
     int regno;
{
  static char *rn[] = {
    "r00", "r01", "r02", "r03", "r04", "r05", "r06", "r07",
    "r08", "r09", "r10", "r11", "r12", "r13", "r14", "r15",
    "r16", "r17", "r18", "r19", "r20", "r21", "r22", "r23",
    "r24", "r25", "r26", "r27", "r28", "r29", "r30", "r31",

    /* these get confusing because we omit a few and switch some ordering around. */

    "cr01",  /* 32 = psr */
    "fcr62", /* 33 = fpsr*/
    "fcr63", /* 34 = fpcr */
    "ip",			/* this is something of a cheat. */
  /* 35 = sxip */
    "cr05", /* 36 = snip */
    "cr06", /* 37 = sfip */
  };

  return rn[regno];
}

static int
bug_write (a, l)
     char *a;
     int l;
{
  int i;

  SERIAL_WRITE (desc, a, l);

  if (remote_debug)
    for (i = 0; i < l; i++)
      {
	printf ("%c", a[i]);
      }

  return(0);
}

static void
bug_write_cr (s)
     char *s;
{
  bug_write (s, strlen (s));
  bug_write ("\r", 1);
  return;
}

#if 0 /* not currently used */
/* Read from remote while the input matches STRING.  Return zero on
   success, -1 on failure.  */

static int
bug_scan (s)
     char *s;
{
  int c;

  while (*s)
    {
      c = readchar();
      if (c != *s++)
	{
	  fflush(stdout);
	  printf("\nNext character is '%c' - %d and s is \"%s\".\n", c, c, --s);
	  return(-1);
	}
    }

  return(0);
}
#endif /* never */

static int
bug_srec_write_cr (s)
     char *s;
{
  char *p = s;

  if (srec_echo_pace)
    for (p = s; *p; ++p)
      {
	if (remote_debug)
	  printf ("%c", *p);

	do
	  SERIAL_WRITE(desc, p, 1);
	while (readchar_nofail() != *p);
      }
  else
    {  
      bug_write_cr (s);
/*       return(bug_scan (s) || bug_scan ("\n")); */
    }

  return(0);
}

/* Store register REGNO, or all if REGNO == -1. */

static void
bug_fetch_register(regno)
     int regno;
{
  REGISTER_TYPE regval;
  check_open();

  if (regno == -1)
    {
      int i;

      for (i = 0; i < NUM_REGS; ++i)
	bug_fetch_register(i);
    }
  else
    {
      bug_write("rs ", 3);
      bug_write_cr(get_reg_name(regno));
      expect("=");
      regval = get_hex_word();
      expect_prompt();

      /* the following registers contain flag bits in the lower to bit slots.
	 Mask them off */
      if (regno == PC_REGNUM	/* aka sxip */
	  || regno == NPC_REGNUM /* aka snip */
	  || regno == SFIP_REGNUM)	/* aka sfip */
	regval &= ~0x3;

      supply_register(regno, (char *) &regval);
    }

  return;
}

/* Store register REGNO, or all if REGNO == -1. */

static void
bug_store_register (regno)
     int regno;
{
  char buffer[1024];
  check_open();

  if (regno == -1)
    {
      int i;

      for (i = 0; i < NUM_REGS; ++i)
	bug_store_register(i);
    }
  else
    {
      char *regname;

      regname = (get_reg_name(regno));

      sprintf(buffer, "rs %s %08x",
	      regname,
	      read_register(regno));

      bug_write_cr(buffer);
      expect_prompt();
    }

  return;
}

/* Get ready to modify the registers array.  On machines which store
   individual registers, this doesn't need to do anything.  On machines
   which store all the registers in one fell swoop, this makes sure
   that registers contains all the registers from the program being
   debugged.  */

void
bug_prepare_to_store ()
{
  /* Do nothing, since we can store individual regs */
}

/* Read a word from remote address ADDR and return it.
 * This goes through the data cache.
 */
int
bug_fetch_word (addr)
     CORE_ADDR addr;
{
  return dcache_fetch (bug_dcache, addr);
}

/* Write a word WORD into remote address ADDR.
   This goes through the data cache.  */

void
bug_store_word (addr, word)
     CORE_ADDR addr;
     int word;
{
  dcache_poke (bug_dcache, addr, word);
}

int
bug_xfer_inferior_memory (memaddr, myaddr, len, write, target)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
     int write;
     struct target_ops *target;	/* ignored */
{
  register int i;

  /* Round starting address down to longword boundary.  */
  register CORE_ADDR addr;

  /* Round ending address up; get number of longwords that makes.  */
  register int count;

  /* Allocate buffer of that many longwords.  */
  register int *buffer;

  addr = memaddr & -sizeof (int);
  count = (((memaddr + len) - addr) + sizeof (int) - 1) / sizeof (int);

  buffer = (int *) alloca (count * sizeof (int));

  if (write)
    {
      /* Fill start and end extra bytes of buffer with existing memory data.  */

      if (addr != memaddr || len < (int) sizeof (int))
	{
	  /* Need part of initial word -- fetch it.  */
	  buffer[0] = bug_fetch_word (addr);
	}

      if (count > 1)		/* FIXME, avoid if even boundary */
	{
	  buffer[count - 1]
	    = bug_fetch_word (addr + (count - 1) * sizeof (int));
	}

      /* Copy data to be written over corresponding part of buffer */

      bcopy (myaddr, (char *) buffer + (memaddr & (sizeof (int) - 1)), len);

      /* Write the entire buffer.  */

      for (i = 0; i < count; i++, addr += sizeof (int))
	{
	  errno = 0;
	  bug_store_word (addr, buffer[i]);
	  if (errno)
	    {

	      return 0;
	    }

	}
    }
  else
    {
      /* Read all the longwords */
      for (i = 0; i < count; i++, addr += sizeof (int))
	{
	  errno = 0;
	  buffer[i] = bug_fetch_word (addr);
	  if (errno)
	    {
	      return 0;
	    }
	  QUIT;
	}

      /* Copy appropriate bytes out of the buffer.  */
      bcopy ((char *) buffer + (memaddr & (sizeof (int) - 1)), myaddr, len);
    }

  return len;
}

static void
start_load()
{
  char *command;

  command = (srec_echo_pace ? "lo 0 ;x" : "lo 0");

  bug_write_cr (command);
  expect (command);
  expect ("\r\n");
  bug_srec_write_cr ("S0030000FC");
  return;
}

/* This is an extremely vulnerable and fragile function.  I've made
   considerable attempts to make this deterministic, but I've
   certainly forgotten something.  The trouble is that S-records are
   only a partial file format, not a protocol.  Worse, apparently the
   m88k bug monitor does not run in real time while receiving
   S-records.  Hence, we must pay excruciating attention to when and
   where error messages are returned, and what has actually been sent.

   Each call represents a chunk of memory to be sent to the target.
   We break that chunk into an S0 header record, some number of S3
   data records each containing srec_bytes, and an S7 termination
   record.  */

static char *srecord_strings[] = {
  "S-RECORD",
  "8???-Bug>",
  NULL,
};

static int
bug_write_inferior_memory (memaddr, myaddr, len)
     CORE_ADDR memaddr;
     unsigned char *myaddr;
     int len;
{
  int done;
  int checksum;
  int x;
  int retries;
  char buffer[(srec_bytes + 8) << 1];

  retries = 0;

  do
    {
      done = 0;

      if (retries > srec_max_retries)
	return(-1);

      if (retries > 0)
	{
	  if (remote_debug)
	    printf("\n<retrying...>\n");

	  /* This expect_prompt call is extremely important.  Without
	     it, we will tend to resend our packet so fast that it
	     will arrive before the bug monitor is ready to receive
	     it.  This would lead to a very ugly resend loop.  */

	  expect_prompt();
	}

      start_load();

      while (done < len)
	{
	  int thisgo;
	  int idx;
	  char *buf = buffer;
	  CORE_ADDR address;

	  checksum = 0;
	  thisgo = len - done;
	  if (thisgo > srec_bytes)
	    thisgo = srec_bytes;

	  address = memaddr + done;
	  sprintf (buf, "S3%02X%08X", thisgo + 4 + 1, address);
	  buf += 12;

	  checksum += (thisgo + 4 + 1
		       + (address & 0xff)
		       + ((address >>  8) & 0xff)
		       + ((address >> 16) & 0xff)
		       + ((address >> 24) & 0xff));

	  for (idx = 0; idx < thisgo; idx++)
	    {
	      sprintf (buf, "%02X", myaddr[idx + done]);
	      checksum += myaddr[idx + done];
	      buf += 2;
	    }

	  if (srec_noise > 0)
	    {
	      /* FIXME-NOW: insert a deliberate error every now and then.
		 This is intended for testing/debugging the error handling
		 stuff.  */
	      static int counter = 0;
	      if (++counter > srec_noise)
		{
		  counter = 0;
		  ++checksum;
		}
	    }

	  sprintf(buf, "%02X", ~checksum & 0xff);
	  bug_srec_write_cr (buffer);

	  if (srec_sleep != 0)
	    sleep(srec_sleep);

	  /* This pollchar is probably redundant to the multi_scan
	     below.  Trouble is, we can't be sure when or where an
	     error message will appear.  Apparently, when running at
	     full speed from a typical sun4, error messages tend to
	     appear to arrive only *after* the s7 record.   */

	  if ((x = pollchar()) != 0)
	    {
	      if (remote_debug)
		printf("\n<retrying...>\n");

	      ++retries;

	      /* flush any remaining input and verify that we are back
		 at the prompt level. */
	      expect_prompt();
	      /* start all over again. */
	      start_load();
	      done = 0;
	      continue;
	    }

	  done += thisgo;
	}

      bug_srec_write_cr("S7060000000000F9");
      ++retries;

      /* Having finished the load, we need to figure out whether we
	 had any errors.  */
    } while (multi_scan(srecord_strings, 0) == 0);;

  return(0);
}

void
bug_files_info ()
{
  char *file = "nothing";

  if (exec_bfd)
    file = bfd_get_filename (exec_bfd);

  if (exec_bfd)
#ifdef __GO32__
    printf_filtered ("\tAttached to DOS asynctsr and running program %s\n", file);
#else
    printf_filtered ("\tAttached to %s at %d baud and running program %s\n", dev_name, baudrate, file);
#endif
  printf_filtered ("\ton an m88k processor.\n");
}

/* Copy LEN bytes of data from debugger memory at MYADDR
   to inferior's memory at MEMADDR.  Returns errno value.
 * sb/sh instructions don't work on unaligned addresses, when TU=1.
 */

/* Read LEN bytes from inferior memory at MEMADDR.  Put the result
   at debugger address MYADDR.  Returns errno value.  */
static int
bug_read_inferior_memory (memaddr, myaddr, len)
     CORE_ADDR memaddr;
     unsigned char *myaddr;
     int len;
{
  char request[100];
  char *buffer;
  char *p;
  char type;
  char size;
  unsigned char c;
  unsigned int inaddr;
  unsigned int checksum;

  sprintf(request, "du 0 %x:&%d", memaddr, len);
  bug_write_cr(request);

  p = buffer = alloca(len);

  /* scan up through the header */
  expect("S0030000FC");

  while (p < buffer + len)
    {
      /* scan off any white space. */
      while (readchar() != 'S') ;;

      /* what kind of s-rec? */
      type = readchar();

      /* scan record size */
      get_hex_byte(&size);
      checksum = size;
      --size;
      inaddr = 0;

      switch (type)
	{
	case '7':
	case '8':
	case '9':
	  goto done;

	case '3':
	  get_hex_byte(&c);
	  inaddr = (inaddr << 8) + c;
	  checksum += c;
	  --size;
	  /* intentional fall through */
	case '2':
	  get_hex_byte(&c);
	  inaddr = (inaddr << 8) + c;
	  checksum += c;
	  --size;
	  /* intentional fall through */
	case '1':
	  get_hex_byte(&c);
	  inaddr = (inaddr << 8) + c;
	  checksum += c;
	  --size;
	  get_hex_byte(&c);
	  inaddr = (inaddr << 8) + c;
	  checksum += c;
	  --size;
	  break;

	default:
	  /* bonk */
	  error("reading s-records.");
	}

      if (inaddr < memaddr
	  || (memaddr + len) < (inaddr + size))
	error("srec out of memory range.");

      if (p != buffer + inaddr - memaddr)
	error("srec out of sequence.");

      for (; size; --size, ++p)
	{
	  get_hex_byte(p);
	  checksum += *p;
	}

      get_hex_byte(&c);
      if (c != (~checksum & 0xff))
	error("bad s-rec checksum");
    }

 done:
  expect_prompt();
  if (p != buffer + len)
    return(1);

  memcpy(myaddr, buffer, len);
  return(0);
}

#define MAX_BREAKS	16
static int num_brkpts = 0;
static int
bug_insert_breakpoint (addr, save)
     CORE_ADDR addr;
     char *save;		/* Throw away, let bug save instructions */
{
  check_open ();

  if (num_brkpts < MAX_BREAKS)
    {
      char buffer[100];

      num_brkpts++;
      sprintf (buffer, "br %x", addr);
      bug_write_cr (buffer);
      expect_prompt ();
      return(0);
    }
  else
    {
      fprintf_filtered (stderr,
		      "Too many break points, break point not installed\n");
      return(1);
    }

}
static int
bug_remove_breakpoint (addr, save)
     CORE_ADDR addr;
     char *save;		/* Throw away, let bug save instructions */
{
  if (num_brkpts > 0)
    {
      char buffer[100];

      num_brkpts--;
      sprintf (buffer, "nobr %x", addr);
      bug_write_cr (buffer);
      expect_prompt ();

    }
  return (0);
}

/* Clear the bugs notion of what the break points are */
static int
bug_clear_breakpoints ()
{

  if (is_open)
    {
      bug_write_cr ("nobr");
      expect("nobr");
      expect_prompt ();
    }
  num_brkpts = 0;
  return(0);
}

static void
bug_mourn ()
{
  bug_clear_breakpoints ();
  generic_mourn_inferior ();
}

/* Put a command string, in args, out to the bug.  The bug is assumed to
   be in raw mode, all writing/reading done through desc.
   Ouput from the bug is placed on the users terminal until the
   prompt from the bug is seen.
   FIXME: Can't handle commands that take input.  */

void
bug_com (args, fromtty)
     char *args;
     int fromtty;
{
  check_open ();

  if (!args)
    return;

  /* Clear all input so only command relative output is displayed */

  bug_write_cr (args);
  bug_write ("\030", 1);
  expect_prompt ();
}

static void
bug_device (args, fromtty)
     char *args;
     int fromtty;
{
  if (args)
    dev_name = get_word (&args);

  return;
}

#if 0
static
bug_speed (s)
     char *s;
{
  check_open ();

  if (s)
    {
      char buffer[100];
      int newrate = atoi (s);
      int which = 0;

      if (SERIAL_SETBAUDRATE (desc, newrate))
	error ("Can't use %d baud\n", newrate);

      printf_filtered ("Checking target is in sync\n");

      printf_filtered ("Sending commands to set target to %d\n",
		       baudrate);

      sprintf (buffer, "tm %d. N 8 1", baudrate);
      bug_write_cr (buffer);
    }
}
#endif /* 0 */

struct target_ops bug_ops =
{
  "bug", "Remote BUG monitor",
  "Use the mvme187 board running the BUG monitor connected\n\
by a serial line.",

  bug_open, bug_close,
  0, bug_detach, bug_resume, bug_wait,	/* attach */
  bug_fetch_register, bug_store_register,
  bug_prepare_to_store,
  bug_xfer_inferior_memory,
  bug_files_info,
  bug_insert_breakpoint, bug_remove_breakpoint,	/* Breakpoints */
  0, 0, 0, 0, 0,		/* Terminal handling */
  bug_kill,			/* FIXME, kill */
  bug_load,
  0,				/* lookup_symbol */
  bug_create_inferior,		/* create_inferior */
  bug_mourn,			/* mourn_inferior FIXME */
  0,				/* can_run */
  0,				/* notice_signals */
  process_stratum, 0,		/* next */
  1, 1, 1, 1, 1,		/* all mem, mem, stack, regs, exec */
  0, 0,				/* Section pointers */
  OPS_MAGIC,			/* Always the last thing */
};

void
_initialize_remote_bug ()
{
  add_target (&bug_ops);

  add_com ("bug <command>", class_obscure, bug_com,
	   "Send a command to the BUG monitor.");

  add_com ("device", class_obscure, bug_device,
	   "Set the terminal line for BUG communications");

#if 0
  add_com ("speed", class_obscure, bug_speed,
	   "Set the terminal line speed for BUG communications");
#endif /* 0 */

  add_show_from_set
    (add_set_cmd ("srec-bytes", class_support, var_uinteger,
		  (char *) &srec_bytes,
		  "\
Set the number of bytes represented in each S-record.\n\
This affects the communication protocol with the remote target.",
		  &setlist),
     &showlist);

  add_show_from_set
    (add_set_cmd ("srec-max-retries", class_support, var_uinteger,
		  (char *) &srec_max_retries,
		  "\
Set the number of retries for shipping S-records.\n\
This affects the communication protocol with the remote target.",
		  &setlist),
     &showlist);

  add_show_from_set
    (add_set_cmd ("srec-frame", class_support, var_uinteger,
		  (char *) &srec_frame,
		  "\
Set the number of bytes in an S-record frame.\n\
This affects the communication protocol with the remote target.",
		  &setlist),
     &showlist);

  add_show_from_set
    (add_set_cmd ("srec-noise", class_support, var_zinteger,
		  (char *) &srec_noise,
		  "\
Set number of S-record to send before deliberately flubbing a checksum.\n\
Zero means flub none at all.  This affects the communication protocol\n\
with the remote target.",
		  &setlist),
     &showlist);

  add_show_from_set
    (add_set_cmd ("srec-sleep", class_support, var_zinteger,
		  (char *) &srec_sleep,
		  "\
Set number of seconds to sleep after an S-record for a possible error message to arrive.\n\
This affects the communication protocol with the remote target.",
		  &setlist),
     &showlist);

  add_show_from_set
    (add_set_cmd ("srec-echo-pace", class_support, var_boolean,
		  (char *) &srec_echo_pace,
		  "\
Set echo-verification.\n\
When on, use verification by echo when downloading S-records.  This is\n\
much slower, but generally more reliable.", 
		  &setlist),
     &showlist);

  dev_name = NULL;
}
