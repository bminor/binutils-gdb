/* Remote debugging interface for Tandem ST2000 phone switch, for GDB.
   Copyright 1990, 1991, 1992 Free Software Foundation, Inc.
   Contributed by Cygnus Support.  Written by Jim Kingdon for Cygnus.

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

/* This file was derived from remote-eb.c, which did a similar job, but for
   an AMD-29K running EBMON.  That file was in turn derived from remote.c
   as mentioned in the following comment (left in for comic relief):

  "This is like remote.c but is for an esoteric situation--
   having a 29k board in a PC hooked up to a unix machine with
   a serial line, and running ctty com1 on the PC, through which
   the unix machine can run ebmon.  Not to mention that the PC
   has PC/NFS, so it can access the same executables that gdb can,
   over the net in real time."

   In reality, this module talks to a debug monitor called 'STDEBUG', which
   runs in a phone switch.  We communicate with STDEBUG via either a direct
   serial line, or a TCP (or possibly TELNET) stream to a terminal multiplexor,
   which in turn talks to the phone switch. */

#include "defs.h"
#include "gdbcore.h"
#include "terminal.h"
#include "target.h"
#include "wait.h"
#include <varargs.h>
#include <signal.h>
#include <string.h>

extern struct target_ops st2000_ops;		/* Forward declaration */

static void st2000_close();
static void st2000_fetch_register();
static void st2000_store_register();

#define LOG_FILE "st2000.log"
#if defined (LOG_FILE)
FILE *log_file;
#endif

static int timeout = 24;

/* Descriptor for I/O to remote machine.  Initialize it to -1 so that
   st2000_open knows that we don't have a file open when the program
   starts.  */
int st2000_desc = -1;

/* stream which is fdopen'd from st2000_desc.  Only valid when
   st2000_desc != -1.  */
FILE *st2000_stream;

/* Send data to stdebug.  Works just like printf. */

static void
printf_stdebug(va_alist)
     va_dcl
{
  va_list args;
  char *pattern;

  va_start(args);

  pattern = va_arg(args, char *);

  vfprintf(st2000_stream, pattern, args);
  fflush(st2000_stream);
}

/* Read a character from the remote system, doing all the fancy
   timeout stuff.  */
static int
readchar ()
{
  char buf;

  buf = '\0';
#ifdef HAVE_TERMIO
  /* termio does the timeout for us.  */
  read (st2000_desc, &buf, 1);
#else
  alarm (timeout);
  if (read (st2000_desc, &buf, 1) < 0)
    {
      if (errno == EINTR)
	error ("Timeout reading from remote system.");
      else
	perror_with_name ("remote");
    }
  alarm (0);
#endif

  if (buf == '\0')
    error ("Timeout reading from remote system.");
#if defined (LOG_FILE)
  putc (buf & 0x7f, log_file);
#endif
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
  while (1)
    {
      if (readchar() == *p)
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

/* Keep discarding input until we see the STDEBUG prompt.

   The convention for dealing with the prompt is that you
   o give your command
   o *then* wait for the prompt.

   Thus the last thing that a procedure does with the serial line
   will be an expect_prompt().  Exception:  st2000_resume does not
   wait for the prompt, because the terminal is being handed over
   to the inferior.  However, the next thing which happens after that
   is a st2000_wait which does wait for the prompt.
   Note that this includes abnormal exit, e.g. error().  This is
   necessary to prevent getting into states from which we can't
   recover.  */
static void
expect_prompt ()
{
#if defined (LOG_FILE)
  /* This is a convenient place to do this.  The idea is to do it often
     enough that we never lose much data if we terminate abnormally.  */
  fflush (log_file);
#endif
  expect ("dbug> ");
}

/* Get a hex digit from the remote system & return its value.
   If ignore_space is nonzero, ignore spaces (not newline, tab, etc).  */
static int
get_hex_digit (ignore_space)
     int ignore_space;
{
  int ch;
  while (1)
    {
      ch = readchar ();
      if (ch >= '0' && ch <= '9')
	return ch - '0';
      else if (ch >= 'A' && ch <= 'F')
	return ch - 'A' + 10;
      else if (ch >= 'a' && ch <= 'f')
	return ch - 'a' + 10;
      else if (ch == ' ' && ignore_space)
	;
      else
	{
	  expect_prompt ();
	  error ("Invalid hex digit from remote system.");
	}
    }
}

/* Get a byte from st2000_desc and put it in *BYT.  Accept any number
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

/* Get N 32-bit words from remote, each preceded by a space,
   and put them in registers starting at REGNO.  */
static void
get_hex_regs (n, regno)
     int n;
     int regno;
{
  long val;
  int i;

  for (i = 0; i < n; i++)
    {
      int j;
      
      val = 0;
      for (j = 0; j < 8; j++)
	val = (val << 4) + get_hex_digit (j == 0);
      supply_register (regno++, (char *) &val);
    }
}

/* Called when SIGALRM signal sent due to alarm() timeout.  */
#ifndef HAVE_TERMIO

#ifndef __STDC__
#define volatile /**/
#endif
volatile int n_alarms;

static void
st2000_timer ()
{
  n_alarms++;
}
#endif

/* This is called not only when we first attach, but also when the
   user types "run" after having attached.  */
static void
st2000_create_inferior (execfile, args, env)
     char *execfile;
     char *args;
     char **env;
{
  int entry_pt;

  if (args && *args)
    error ("Can't pass arguments to remote STDEBUG process");

  if (execfile == 0 || exec_bfd == 0)
    error ("No exec file specified");

  entry_pt = (int) bfd_get_start_address (exec_bfd);

#ifdef CREATE_INFERIOR_HOOK
  CREATE_INFERIOR_HOOK (0);		/* No process-ID */
#endif  

/* The "process" (board) is already stopped awaiting our commands, and
   the program is already downloaded.  We just set its PC and go.  */

  clear_proceed_status ();

  /* Tell wait_for_inferior that we've started a new process.  */
  init_wait_for_inferior ();

  /* Set up the "saved terminal modes" of the inferior
     based on what modes we are starting it with.  */
  target_terminal_init ();

  /* Install inferior's terminal modes.  */
  target_terminal_inferior ();

  /* insert_step_breakpoint ();  FIXME, do we need this?  */
  proceed ((CORE_ADDR)entry_pt, -1, 0);		/* Let 'er rip... */
}

/* Translate baud rates from integers to damn B_codes.  Unix should
   have outgrown this crap years ago, but even POSIX wouldn't buck it.  */

#ifndef B19200
#define B19200 EXTA
#endif
#ifndef B38400
#define B38400 EXTB
#endif

static struct {int rate, damn_b;} baudtab[] = {
	{0, B0},
	{50, B50},
	{75, B75},
	{110, B110},
	{134, B134},
	{150, B150},
	{200, B200},
	{300, B300},
	{600, B600},
	{1200, B1200},
	{1800, B1800},
	{2400, B2400},
	{4800, B4800},
	{9600, B9600},
	{19200, B19200},
	{38400, B38400},
	{-1, -1},
};

static int
damn_b (rate)
     int rate;
{
  int i;

  for (i = 0; baudtab[i].rate != -1; i++)
    if (rate == baudtab[i].rate) return baudtab[i].damn_b;
  return B38400;	/* Random */
}


/* Open a connection to a remote debugger.
   NAME is the filename used for communication.  */

static int baudrate = 9600;
static char *dev_name;

static TERMINAL old_sg;

static void
st2000_open (name, from_tty)
     char *name;
     int from_tty;
{
  TERMINAL sg;

  char *p;

  target_preopen (from_tty);
  
  if (!name)
    goto erroid;

  /* Find the first whitespace character, it separates dev_name from
     the baud rate.  */

  for (p = name; *p && !isspace (*p); p++)
    ;
  if (*p == '\0')
erroid:
    error ("\
Please include the name of the device for the serial port, and the baud rate.");

  dev_name = alloca (p - name + 1);
  strncpy (dev_name, name, p - name);
  dev_name[p - name] = '\0';

  /* Skip over the whitespace after dev_name */
  for (; isspace (*p); p++)
    /*EMPTY*/;
  
  if (1 != sscanf (p, "%d ", &baudrate))
    goto erroid;

  st2000_close (0);

  st2000_desc = open (dev_name, O_RDWR);
  if (st2000_desc < 0)
    perror_with_name (dev_name);
  ioctl (st2000_desc, TIOCGETP, &sg);
  old_sg = sg;

#ifdef HAVE_TERMIO
  sg.c_cc[VMIN] = 0;		/* read with timeout.  */
  sg.c_cc[VTIME] = timeout * 10;
  sg.c_lflag &= ~(ICANON | ECHO);
  sg.c_cflag = (sg.c_cflag & ~CBAUD) | damn_b (baudrate);
#else
  sg.sg_ispeed = damn_b (baudrate);
  sg.sg_ospeed = damn_b (baudrate);
  sg.sg_flags |= RAW | ANYP;
  sg.sg_flags &= ~ECHO;
#endif

  ioctl (st2000_desc, TIOCSETP, &sg);
  st2000_stream = fdopen (st2000_desc, "r+");

  push_target (&st2000_ops);

#ifndef HAVE_TERMIO
#ifndef NO_SIGINTERRUPT
  /* Cause SIGALRM's to make reads fail with EINTR instead of resuming
     the read.  */
  if (siginterrupt (SIGALRM, 1) != 0)
    perror ("st2000_open: error in siginterrupt");
#endif

  /* Set up read timeout timer.  */
  if ((void (*)) signal (SIGALRM, st2000_timer) == (void (*)) -1)
    perror ("st2000_open: error in signal");
#endif

#if defined (LOG_FILE)
  log_file = fopen (LOG_FILE, "w");
  if (log_file == NULL)
    perror_with_name (LOG_FILE);
#endif

  /* Hello?  Are you there?  */
  printf_stdebug ("\r");
  
  expect_prompt ();

  if (from_tty)
    printf ("Remote %s connected to %s\n", target_shortname,
	    dev_name);
}

/* Close out all files and local state before this target loses control. */

static void
st2000_close (quitting)
     int quitting;
{

  /* Reset the terminal to its original settings. */

  ioctl (st2000_desc, TIOCSETP, &old_sg);

  /* Due to a bug in Unix, fclose closes not only the stdio stream,
     but also the file descriptor.  So we don't actually close
     st2000_desc.  */
  if (st2000_stream)
    fclose (st2000_stream);	/* This also closes st2000_desc */

  /* Do not try to close st2000_desc again, later in the program.  */
  st2000_stream = NULL;
  st2000_desc = -1;

#if defined (LOG_FILE)
  if (log_file) {
    if (ferror (log_file))
      printf ("Error writing log file.\n");
    if (fclose (log_file) != 0)
      printf ("Error closing log file.\n");
  }
#endif
}

/* Terminate the open connection to the remote debugger.
   Use this when you want to detach and do something else
   with your gdb.  */
static void
st2000_detach (from_tty)
     int from_tty;
{
  pop_target();		/* calls st2000_close to do the real work */
  if (from_tty)
    printf ("Ending remote %s debugging\n", target_shortname);
}
 
/* Tell the remote machine to resume.  */

static void
st2000_resume (step, sig)
     int step, sig;
{
  if (step)
    {
      printf_stdebug ("ST\r");
      /* Wait for the echo.  */
      expect ("ST\r");
    }
  else
    {
      printf_stdebug ("GO\r");
      /* Swallow the echo.  */
      expect ("GO\r");
    }
}

/* Wait until the remote machine stops, then return,
   storing status in STATUS just as `wait' would.  */

static int
st2000_wait (status)
     WAITTYPE *status;
{
  /* FIXME --- USE A REAL STRING MATCHING ALGORITHM HERE!!! */

  static char bpt[] = "dbug> ";
  char *bp = bpt;

  /* Large enough for either sizeof (bpt) or sizeof (exitmsg) chars.  */
  char swallowed[50];
  /* Current position in swallowed.  */
  char *swallowed_p = swallowed;

  int ch;
  int ch_handled;

  int old_timeout = timeout;

  WSETEXIT ((*status), 0);

  timeout = 0;		/* Don't time out -- user program is running. */
  while (1)
    {
      ch_handled = 0;
      ch = readchar ();
      if (ch == *bp)
	{
	  bp++;
	  if (*bp == '\0')
	    break;
	  ch_handled = 1;

	  *swallowed_p++ = ch;
	}
      else
	bp = bpt;

      if (!ch_handled)
	{
	  char *p;

	  /* Print out any characters which have been swallowed.  */
	  for (p = swallowed; p < swallowed_p; ++p)
	    putc (*p, stdout);
	  swallowed_p = swallowed;
	  
	  putc (ch, stdout);
	}
    }
  if (*bp == '\000')
    WSETSTOP ((*status), SIGTRAP);
  else
    WSETEXIT ((*status), 0);
  timeout = old_timeout;

  return 0;
}

/* Return the name of register number REGNO in the form input and output by
   STDEBUG.  Currently, REGISTER_NAMES just happens to contain exactly what
   STDEBUG wants.  Lets take advantage of that just as long as possible! */

static char *
get_reg_name (regno)
     int regno;
{
  static char buf[50];
  char *p, *b;

  b = buf;

  for (p = reg_names[regno]; *p; p++)
    *b++ = toupper(*p);
  *b = '\000';

  return buf;
}

/* Read the remote registers into the block REGS.  */

static void
st2000_fetch_registers ()
{
  int regno;

  /* Yeah yeah, I know this is horribly inefficient.  But it isn't done
     very often...  I'll clean it up later.  */

  for (regno = 0; regno <= PC_REGNUM; regno++)
    st2000_fetch_register(regno);
}

/* Fetch register REGNO, or all registers if REGNO is -1.
   Returns errno value.  */
static void
st2000_fetch_register (regno)
     int regno;
{
  if (regno == -1)
    st2000_fetch_registers ();
  else
    {
      char *name = get_reg_name (regno);
      printf_stdebug ("DR %s\r", name);
      expect (name);
      expect (" : ");
      get_hex_regs (1, regno);
      expect_prompt ();
    }
  return;
}

/* Store the remote registers from the contents of the block REGS.  */

static void
st2000_store_registers ()
{
  int regno;

  for (regno = 0; regno <= PC_REGNUM; regno++)
    st2000_store_register(regno);

  registers_changed ();
}

/* Store register REGNO, or all if REGNO == 0.
   Return errno value.  */
static void
st2000_store_register (regno)
     int regno;
{
  if (regno == -1)
    st2000_store_registers ();
  else
    {
      printf_stdebug ("PR %s %x\r", get_reg_name (regno),
		      read_register (regno));

      expect_prompt ();
    }
}

/* Get ready to modify the registers array.  On machines which store
   individual registers, this doesn't need to do anything.  On machines
   which store all the registers in one fell swoop, this makes sure
   that registers contains all the registers from the program being
   debugged.  */

static void
st2000_prepare_to_store ()
{
  /* Do nothing, since we can store individual regs */
}

static void
st2000_files_info ()
{
  printf ("\tAttached to %s at %d baud.\n",
	  dev_name, baudrate);
}

/* Copy LEN bytes of data from debugger memory at MYADDR
   to inferior's memory at MEMADDR.  Returns length moved.  */
static int
st2000_write_inferior_memory (memaddr, myaddr, len)
     CORE_ADDR memaddr;
     unsigned char *myaddr;
     int len;
{
  int i;

  for (i = 0; i < len; i++)
    {
      printf_stdebug ("PM.B %x %x\r", memaddr + i, myaddr[i]);
      expect_prompt ();
    }
  return len;
}

/* Read LEN bytes from inferior memory at MEMADDR.  Put the result
   at debugger address MYADDR.  Returns length moved.  */
static int
st2000_read_inferior_memory(memaddr, myaddr, len)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
{
  int i;

  /* Number of bytes read so far.  */
  int count;

  /* Starting address of this pass.  */
  unsigned long startaddr;

  /* Number of bytes to read in this pass.  */
  int len_this_pass;

  /* Note that this code works correctly if startaddr is just less
     than UINT_MAX (well, really CORE_ADDR_MAX if there was such a
     thing).  That is, something like
     st2000_read_bytes (CORE_ADDR_MAX - 4, foo, 4)
     works--it never adds len to memaddr and gets 0.  */
  /* However, something like
     st2000_read_bytes (CORE_ADDR_MAX - 3, foo, 4)
     doesn't need to work.  Detect it and give up if there's an attempt
     to do that.  */
  if (((memaddr - 1) + len) < memaddr) {
    errno = EIO;
    return 0;
  }
  
  startaddr = memaddr;
  count = 0;
  while (count < len)
    {
      len_this_pass = 16;
      if ((startaddr % 16) != 0)
	len_this_pass -= startaddr % 16;
      if (len_this_pass > (len - count))
	len_this_pass = (len - count);

      printf_stdebug ("DI.L %x %x\r", startaddr, len_this_pass);
      expect (":  ");

      for (i = 0; i < len_this_pass; i++)
	get_hex_byte (&myaddr[count++]);

      expect_prompt ();

      startaddr += len_this_pass;
    }
  return len;
}

/* FIXME-someday!  Merge these two.  */
static int
st2000_xfer_inferior_memory (memaddr, myaddr, len, write, target)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
     int write;
     struct target_ops *target;		/* ignored */
{
  if (write)
    return st2000_write_inferior_memory (memaddr, myaddr, len);
  else
    return st2000_read_inferior_memory (memaddr, myaddr, len);
}

static void
st2000_kill (args, from_tty)
     char *args;
     int from_tty;
{
  return;		/* Ignore attempts to kill target system */
}

/* Clean up when a program exits.

   The program actually lives on in the remote processor's RAM, and may be
   run again without a download.  Don't leave it full of breakpoint
   instructions.  */

static void
st2000_mourn_inferior ()
{
  remove_breakpoints ();
  generic_mourn_inferior ();	/* Do all the proper things now */
}

#define MAX_STDEBUG_BREAKPOINTS 16

extern int memory_breakpoint_size;
static CORE_ADDR breakaddr[MAX_STDEBUG_BREAKPOINTS] = {0};

static int
st2000_insert_breakpoint (addr, shadow)
     CORE_ADDR addr;
     char *shadow;
{
  int i;

  for (i = 0; i <= MAX_STDEBUG_BREAKPOINTS; i++)
    if (breakaddr[i] == 0)
      {
	breakaddr[i] = addr;

	st2000_read_inferior_memory(addr, shadow, memory_breakpoint_size);
	printf_stdebug("BR %x H\r", addr);
	expect_prompt();
	return 0;
      }

  fprintf(stderr, "Too many breakpoints (> 16) for STDBUG\n");
  return 1;
}

static int
st2000_remove_breakpoint (addr, shadow)
     CORE_ADDR addr;
     char *shadow;
{
  int i;

  for (i = 0; i < MAX_STDEBUG_BREAKPOINTS; i++)
    if (breakaddr[i] == addr)
      {
	breakaddr[i] = 0;

	printf_stdebug("CB %d\r", i);
	expect_prompt();
	return 0;
      }

  fprintf(stderr, "Can't find breakpoint associated with 0x%x\n", addr);
  return 1;
}

/* Define the target subroutine names */

static struct target_ops st2000_ops = {
	"st2000", "Remote serial Tandem ST2000 target",
	"Use a remote computer running STDEBUG connected by a serial line,\n\
or a network connection.\n\
Arguments are the name of the device for the serial line,\n\
the speed to connect at in bits per second.",
	st2000_open, st2000_close, 
	0, st2000_detach, st2000_resume, st2000_wait,
	st2000_fetch_register, st2000_store_register,
	st2000_prepare_to_store, 0, 0, 	/* conv_to, conv_from */
	st2000_xfer_inferior_memory, st2000_files_info,
	st2000_insert_breakpoint, st2000_remove_breakpoint, /* Breakpoints */
	0, 0, 0, 0, 0,	/* Terminal handling */
	st2000_kill,
	0,	/* load */
	0, /* lookup_symbol */
	st2000_create_inferior,
	st2000_mourn_inferior,
	process_stratum, 0, /* next */
	1, 1, 1, 1, 1,	/* all mem, mem, stack, regs, exec */
	0, 0,			/* Section pointers */
	OPS_MAGIC,		/* Always the last thing */
};

void
_initialize_remote_st2000 ()
{
  add_target (&st2000_ops);
}
