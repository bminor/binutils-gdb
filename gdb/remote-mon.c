/* Remote debugging interface for boot monitors, for GDB.
   Copyright 1990, 1991, 1992, 1993 Free Software Foundation, Inc.
   Contributed by Cygnus Support. Written by Rob Savoye for Cygnus.

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
   having an a29k board in a PC hooked up to a unix machine with
   a serial line, and running ctty com1 on the PC, through which
   the unix machine can run ebmon.  Not to mention that the PC
   has PC/NFS, so it can access the same executables that gdb can,
   over the net in real time."

   In reality, this module talks to a debug monitor called 'MONITOR', which
   We communicate with MONITOR via either a direct serial line, or a TCP
   (or possibly TELNET) stream to a terminal multiplexor,
   which in turn talks to the target board.

   This is based on remote-st2000.c. I left in the above note here for histerical
   reasons.
*/

#include "defs.h"
#include "gdbcore.h"
#include "target.h"
#include "wait.h"
#include <varargs.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include "command.h"
#include "serial.h"
#include "monitor.h"
#include "remote-utils.h"

#ifdef HAVE_TERMIO
#  define TERMINAL struct termios
#else
#  define TERMINAL struct sgttyb
#endif

struct monitor_ops *current_monitor;
extern struct target_ops rom68k_ops;		/* Forward declaration */
extern struct target_ops mon68_ops;		/* Forward declaration */
extern struct target_ops monitor_bug_ops;	/* Forward declaration */
extern struct monitor_ops rom68k_cmds;		/* Forward declaration */
extern struct monitor_ops mon68_cmds;		/* Forward declaration */
extern struct monitor_ops bug_cmds;		/* Forward declaration */
extern struct cmd_list_element *setlist;
extern struct cmd_list_element *unsetlist;
struct cmd_list_element *showlist;

static void monitor_close();
static void monitor_fetch_register();
static void monitor_store_register();
#if 0
static int sr_get_debug();			/* flag set by "set remotedebug" */
#endif
static int hashmark;				/* flag set by "set hash" */

/* FIXME: Replace with sr_get_debug ().  */
#define LOG_FILE "monitor.log"
#if defined (LOG_FILE)
FILE *log_file;
#endif

static int timeout = 24;

/* Descriptor for I/O to remote machine.  Initialize it to NULL so that
   monitor_open knows that we don't have a file open when the program starts.
   */
static serial_t monitor_desc = NULL;

/* Send data to monitor.  Works just like printf. */

static void
printf_monitor(va_alist)
     va_dcl
{
  va_list args;
  char *pattern;
  char buf[200];
  int i;

  va_start(args);

  pattern = va_arg(args, char *);

  vsprintf(buf, pattern, args);

  if (SERIAL_WRITE(monitor_desc, buf, strlen(buf)))
    fprintf(stderr, "SERIAL_WRITE failed: %s\n", safe_strerror(errno));
}

/* Read a character from the remote system, doing all the fancy
   timeout stuff.  */
static int
readchar(timeout)
     int timeout;
{
  int c;

  c = SERIAL_READCHAR(monitor_desc, timeout);

  if (sr_get_debug())
    putchar(c & 0x7f);

#ifdef LOG_FILE
  if (isascii (c))
    putc(c & 0x7f, log_file);
#endif

  if (c >= 0)
    return c & 0x7f;

  if (c == SERIAL_TIMEOUT)
    {
      if (timeout == 0)
	return c;		/* Polls shouldn't generate timeout errors */

      error("Timeout reading from remote system.");
    }

  perror_with_name("remote-monitor");
}

/* Scan input from the remote system, until STRING is found.  If DISCARD is
   non-zero, then discard non-matching input, else print it out.
   Let the user break out immediately.  */
static void
expect(string, discard)
     char *string;
     int discard;
{
  char *p = string;
  int c;

  if (sr_get_debug())
    printf ("Expecting \"%s\"\n", string);

  immediate_quit = 1;
  while (1)
    {
      c = readchar(timeout);
      if (!isascii (c))
	continue;
      if (c == *p++)
	{
	  if (*p == '\0')
	    {
	      immediate_quit = 0;
	      if (sr_get_debug())
		printf ("\nMatched\n");
	      return;
	    }
	}
      else
	{
	  if (!discard)
	    {
	      fwrite(string, 1, (p - 1) - string, stdout);
	      putchar((char)c);
	      fflush(stdout);
	    }
	  p = string;
	}
    }
}

/* Keep discarding input until we see the MONITOR prompt.

   The convention for dealing with the prompt is that you
   o give your command
   o *then* wait for the prompt.

   Thus the last thing that a procedure does with the serial line
   will be an expect_prompt().  Exception:  monitor_resume does not
   wait for the prompt, because the terminal is being handed over
   to the inferior.  However, the next thing which happens after that
   is a monitor_wait which does wait for the prompt.
   Note that this includes abnormal exit, e.g. error().  This is
   necessary to prevent getting into states from which we can't
   recover.  */
static void
expect_prompt(discard)
     int discard;
{
#if defined (LOG_FILE)
  /* This is a convenient place to do this.  The idea is to do it often
     enough that we never lose much data if we terminate abnormally.  */
  fflush(log_file);
#endif
  expect (PROMPT, discard);
}

/* Get a hex digit from the remote system & return its value.
   If ignore_space is nonzero, ignore spaces (not newline, tab, etc).  */
static int
get_hex_digit(ignore_space)
     int ignore_space;
{
  int ch;
  while (1)
    {
      ch = readchar(timeout);
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
	  expect_prompt(1);
	  error("Invalid hex digit from remote system.");
	}
    }
}

/* Get a byte from monitor and put it in *BYT.  Accept any number
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

/* This is called not only when we first attach, but also when the
   user types "run" after having attached.  */
static void
monitor_create_inferior (execfile, args, env)
     char *execfile;
     char *args;
     char **env;
{
  int entry_pt;

  if (args && *args)
    error("Can't pass arguments to remote MONITOR process");

  if (execfile == 0 || exec_bfd == 0)
    error("No exec file specified");

  entry_pt = (int) bfd_get_start_address (exec_bfd);

#ifdef LOG_FILE
  fputs ("\nIn Create_inferior()", log_file);
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

  /* Let 'er rip... */
  proceed ((CORE_ADDR)entry_pt, TARGET_SIGNAL_DEFAULT, 0);
}

/* Open a connection to a remote debugger.
   NAME is the filename used for communication.  */

static int baudrate = 9600;
static char dev_name[100];

static void
general_open(args, name, from_tty)
     char *args;
     char *name;
     int from_tty;
{
  if (args == NULL)
    error ("Use `target %s DEVICE-NAME' to use a serial port, or \n\
`target %s HOST-NAME:PORT-NUMBER' to use a network connection.", name, name);

  target_preopen(from_tty);

/*  if (is_open) */
    monitor_close(0);

  strcpy(dev_name, args);
  monitor_desc = SERIAL_OPEN(dev_name);

  if (monitor_desc == NULL)
    perror_with_name(dev_name);

  if (baud_rate != -1)
    {
      if (SERIAL_SETBAUDRATE (monitor_desc, baud_rate))
	{
	  SERIAL_CLOSE (monitor_desc);
	  perror_with_name (name);
	}
    }

  SERIAL_RAW(monitor_desc);

#if defined (LOG_FILE)
  log_file = fopen (LOG_FILE, "w");
  if (log_file == NULL)
    perror_with_name (LOG_FILE);
#endif

  /* Hello?  Are you there?  */
  printf_monitor("\r");	/* CR wakes up monitor */
  
  expect_prompt(1);

  if (from_tty)
    printf("Remote %s connected to %s\n", target_shortname,
	   dev_name);
}

static void
rom68k_open(args, from_tty)
     char *args;
     int from_tty;
{
  push_target(&rom68k_ops);
  push_monitor (&rom68k_cmds);

  general_open (args, "rom68k", from_tty);
}

static void
mon68_open(args, from_tty)
     char *args;
     int from_tty;
{
  push_target(&mon68_ops);
  push_monitor (&mon68_cmds);

  general_open (args, "mon68", from_tty);
}

static void
bug_open(args, from_tty)
     char *args;
     int from_tty;
{
  push_target(&monitor_bug_ops);
  push_monitor (&bug_cmds);

  general_open (args, "bug", from_tty);
}

/*
 * _close -- Close out all files and local state before this target loses control.
 */

static void
monitor_close (quitting)
     int quitting;
{
  SERIAL_CLOSE(monitor_desc);
  monitor_desc = NULL;

#if defined (LOG_FILE)
  if (log_file) {
    if (ferror(log_file))
      fprintf(stderr, "Error writing log file.\n");
    if (fclose(log_file) != 0)
      fprintf(stderr, "Error closing log file.\n");
  }
#endif
}

/* Terminate the open connection to the remote debugger.
   Use this when you want to detach and do something else
   with your gdb.  */
static void
monitor_detach (from_tty)
     int from_tty;
{
  pop_target();		/* calls monitor_close to do the real work */
  if (from_tty)
    printf ("Ending remote %s debugging\n", target_shortname);
}
 
/*
 * _resume -- Tell the remote machine to resume.
 */
static void
monitor_resume (pid, step, sig)
     int pid, step;
     enum target_signal sig;
{
#ifdef LOG_FILE
  fprintf (log_file, "\nIn Resume (step=%d, sig=%d)\n", step, sig);
#endif

  if (step)
    {
      printf_monitor (STEP_CMD);
      /* wait for the echo.  */
      expect (STEP_CMD, 1);
    }
  else
    {
      printf_monitor (GO_CMD);
      /* swallow the echo.  */
      expect (GO_CMD, 1);
    }
}

/*
 * _wait -- Wait until the remote machine stops, then return,
 *          storing status in status just as `wait' would.
 */

static int
monitor_wait (pid, status)
     int pid;
     struct target_waitstatus *status;
{
  int old_timeout = timeout;
#ifdef LOG_FILE
  fputs ("\nIn wait ()", log_file);
#endif

  status->kind = TARGET_WAITKIND_EXITED;
  status->value.integer = 0;

  timeout = 0;		/* Don't time out -- user program is running. */

  expect_prompt(0);    /* Wait for prompt, outputting extraneous text */

  status->kind = TARGET_WAITKIND_STOPPED;
  status->value.sig = TARGET_SIGNAL_TRAP;

  timeout = old_timeout;

  return 0;
}

/* Return the name of register number regno in the form input and output by
   monitor.  Currently, register_names just happens to contain exactly what
   monitor wants.  Lets take advantage of that just as long as possible! */

static char *
get_reg_name (regno)
     int regno;
{
  static char buf[50];
  const char *p;
  char *b;

  b = buf;

  if (regno < 0)
    return ("");
  for (p = reg_names[regno]; *p; p++)
    *b++ = toupper(*p);
  *b = '\000';

  return buf;
}

/* read the remote registers into the block regs.  */

static void
monitor_fetch_registers ()
{
  int regno;

  /* yeah yeah, i know this is horribly inefficient.  but it isn't done
     very often...  i'll clean it up later.  */

  for (regno = 0; regno <= PC_REGNUM; regno++)
    monitor_fetch_register(regno);
}

/* Fetch register REGNO, or all registers if REGNO is -1.
   Returns errno value.  */
static void
monitor_fetch_register (regno)
     int regno;
{
  int val, j;

#ifdef LOG_FILE
  fprintf (log_file, "\nIn Fetch Register (reg=%s)\n", get_reg_name (regno));
  fflush (log_file);
#endif

  if (regno < 0)
    {
      monitor_fetch_registers ();
    }
  else
    {
      char *name = get_reg_name (regno);
      printf_monitor (GET_REG, name);
      expect (name, 1);
      expect (REG_DELIM, 1);
      if (strcasecmp (name, "SR") == 0)
	{
	  val = 0;
	  for (j = 0; j < 4; j++)
	    val = (val << 4) + get_hex_digit (j == 0);
	  supply_register (regno, (char *) &val);
	}
      else
	{
	  get_hex_regs (1, regno);
	}
      if (CMD_END) 
	{
	  expect (CMD_DELIM);
	  printf_monitor (CMD_END);
	}
      expect_prompt (1);
    }
  return;
}

/* Store the remote registers from the contents of the block REGS.  */

static void
monitor_store_registers ()
{
  int regno;

  for (regno = 0; regno <= PC_REGNUM; regno++)
    monitor_store_register(regno);

  registers_changed ();
}

/* Store register REGNO, or all if REGNO == 0.
   return errno value.  */
static void
monitor_store_register (regno)
     int regno;
{
#ifdef LOG_FILE
  fprintf (log_file, "\nIn Store_register (regno=%d)\n", regno);
#endif
  if (regno == -1)
    monitor_store_registers ();
  else
    {
      if (sr_get_debug())
	printf ("Setting register %s to 0x%x\n", get_reg_name (regno), read_register (regno));

      printf_monitor (SET_REG, get_reg_name (regno),
		      read_register (regno));

      expect_prompt (1);
    }
}

/* Get ready to modify the registers array.  On machines which store
   individual registers, this doesn't need to do anything.  On machines
   which store all the registers in one fell swoop, this makes sure
   that registers contains all the registers from the program being
   debugged.  */

static void
monitor_prepare_to_store ()
{
  /* Do nothing, since we can store individual regs */
}

static void
monitor_files_info ()
{
  printf ("\tAttached to %s at %d baud.\n",
	  dev_name, baudrate);
}

/* Copy LEN bytes of data from debugger memory at MYADDR
   to inferior's memory at MEMADDR.  Returns length moved.  */
static int
monitor_write_inferior_memory (memaddr, myaddr, len)
     CORE_ADDR memaddr;
     unsigned char *myaddr;
     int len;
{
  int i;
  char buf[10];

#ifdef LOG_FILE
  fprintf (log_file, "\nIn Write_inferior_memory (memaddr=%x, len=%d)\n", memaddr, len);
#endif
  for (i = 0; i < len; i++)
    {
      printf_monitor (MEM_SET_CMD, memaddr + i);
      expect (sprintf (buf, MEM_PROMPT, memaddr + i), 1); 
      expect (CMD_DELIM);
      printf_monitor ("%x", myaddr[i]);
      if (sr_get_debug())
	printf ("\nSet 0x%x to 0x%x\n", memaddr + i, myaddr[i]);
      if (CMD_END)
	{
/***	  expect (sprintf (buf, MEM_PROMPT, memaddr + i +1), 1); 	  
	  expect (CMD_DELIM); ***/
	  printf_monitor (CMD_END);
	}
      expect_prompt (1);
    }
  return len;
}

/* Read LEN bytes from inferior memory at MEMADDR.  Put the result
   at debugger address MYADDR.  Returns length moved.  */
static int
monitor_read_inferior_memory(memaddr, myaddr, len)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
{
  int i, j;
  char buf[20];

  /* Number of bytes read so far.  */
  int count;

  /* Starting address of this pass.  */
  unsigned long startaddr;

  /* Number of bytes to read in this pass.  */
  int len_this_pass;

#ifdef LOG_FILE
  fprintf (log_file, "\nIn Read_inferior_memory (memaddr=%x, len=%d)\n", memaddr, len);
#endif

  /* Note that this code works correctly if startaddr is just less
     than UINT_MAX (well, really CORE_ADDR_MAX if there was such a
     thing).  That is, something like
     monitor_read_bytes (CORE_ADDR_MAX - 4, foo, 4)
     works--it never adds len To memaddr and gets 0.  */
  /* However, something like
     monitor_read_bytes (CORE_ADDR_MAX - 3, foo, 4)
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
      if (sr_get_debug())
	printf ("\nDisplay %d bytes at %x\n", len_this_pass, startaddr);

      for (i = 0; i < len_this_pass; i++)
	{
	  printf_monitor (MEM_DIS_CMD, startaddr);
	  expect (sprintf(buf, MEM_PROMPT, startaddr), 1);
	  get_hex_byte (&myaddr[count++]);
	  if (sr_get_debug())
	    printf ("\nRead a 0x%x from 0x%x\n", myaddr[count-1], startaddr);
	  if (CMD_END) 
	    {
	      expect (CMD_DELIM);
	      printf_monitor (CMD_END);
	    }
	  expect_prompt (1);
	  startaddr += 1;
	}
    }
  return len;
}

/* FIXME-someday!  merge these two.  */
static int
monitor_xfer_inferior_memory (memaddr, myaddr, len, write, target)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
     int write;
     struct target_ops *target;		/* ignored */
{
  if (write)
    return monitor_write_inferior_memory (memaddr, myaddr, len);
  else
    return monitor_read_inferior_memory (memaddr, myaddr, len);
}

static void
monitor_kill (args, from_tty)
     char *args;
     int from_tty;
{
  return;		/* ignore attempts to kill target system */
}

/* Clean up when a program exits.
   The program actually lives on in the remote processor's RAM, and may be
   run again without a download.  Don't leave it full of breakpoint
   instructions.  */

static void
monitor_mourn_inferior ()
{
  remove_breakpoints ();
  generic_mourn_inferior ();	/* Do all the proper things now */
}

#define MAX_MONITOR_BREAKPOINTS 16

extern int memory_breakpoint_size;
static CORE_ADDR breakaddr[MAX_MONITOR_BREAKPOINTS] = {0};

static int
monitor_insert_breakpoint (addr, shadow)
     CORE_ADDR addr;
     char *shadow;
{
  int i;

#ifdef LOG_FILE
  fprintf (log_file, "\nIn Insert_breakpoint (addr=%x)\n", addr);
#endif
  for (i = 0; i <= MAX_MONITOR_BREAKPOINTS; i++)
    if (breakaddr[i] == 0)
      {
	breakaddr[i] = addr;
	if (sr_get_debug())
	  printf ("Breakpoint at %x\n", addr);
	monitor_read_inferior_memory(addr, shadow, memory_breakpoint_size);
	printf_monitor(SET_BREAK_CMD, addr);
	expect_prompt(1);
	return 0;
      }

  fprintf(stderr, "Too many breakpoints (> 16) for monitor\n");
  return 1;
}

/*
 * _remove_breakpoint -- Tell the monitor to remove a breakpoint
 */
static int
monitor_remove_breakpoint (addr, shadow)
     CORE_ADDR addr;
     char *shadow;
{
  int i;

#ifdef LOG_FILE
  fprintf (log_file, "\nIn Remove_breakpoint (addr=%x)\n", addr);
#endif
  for (i = 0; i < MAX_MONITOR_BREAKPOINTS; i++)
    if (breakaddr[i] == addr)
      {
	breakaddr[i] = 0;
	/* some monitors remove breakpoints based on the address */
	if (strcasecmp (target_shortname, "bug") == 0)   
	    printf_monitor(CLR_BREAK_CMD, addr);
	  else
	    printf_monitor(CLR_BREAK_CMD, i);
	expect_prompt(1);
	return 0;
      }

  fprintf(stderr, "Can't find breakpoint associated with 0x%x\n", addr);
  return 1;
}

/* Load a file. This is usually an srecord, which is ascii. No 
   protocol, just sent line by line. */

#define DOWNLOAD_LINE_SIZE 100
static void
monitor_load (arg)
    char	*arg;
{
  FILE *download;
  char buf[DOWNLOAD_LINE_SIZE];
  int i, bytes_read;

  if (sr_get_debug())
    printf ("Loading %s to monitor\n", arg);

  download = fopen (arg, "r");
  if (download == NULL)
    {
    error (sprintf (buf, "%s Does not exist", arg));
    return;
  }

  printf_monitor (LOAD_CMD);
/*  expect ("Waiting for S-records from host... ", 1); */

  while (!feof (download))
    {
      bytes_read = fread (buf, sizeof (char), DOWNLOAD_LINE_SIZE, download);
      if (hashmark)
	{
	  putchar ('.');
	  fflush (stdout);
	}

      if (SERIAL_WRITE(monitor_desc, buf, bytes_read)) {
	fprintf(stderr, "SERIAL_WRITE failed: (while downloading) %s\n", safe_strerror(errno));
	break;
      }
      i = 0;
      while (i++ <=200000) {} ;     			/* Ugly HACK, probably needs flow control */
      if (bytes_read < DOWNLOAD_LINE_SIZE)
	{
	  if (!feof (download))
	    error ("Only read %d bytes\n", bytes_read);
	  break;
	}
    }

  if (hashmark)
    {
      putchar ('\n');
    }
  if (!feof (download))
    error ("Never got EOF while downloading");
  fclose (download);
}

/* Put a command string, in args, out to MONITOR.  Output from MONITOR is placed
   on the users terminal until the prompt is seen. */

static void
monitor_command (args, fromtty)
     char	*args;
     int	fromtty;
{
#ifdef LOG_FILE
  fprintf (log_file, "\nIn command (args=%s)\n", args);
#endif
  if (monitor_desc == NULL)
    error("monitor target not open.");
  
  if (!args)
    error("Missing command.");
	
  printf_monitor("%s\r", args);
  expect_prompt(0);
}

#if 0

/* Connect the user directly to MONITOR.  This command acts just like the
   'cu' or 'tip' command.  Use <CR>~. or <CR>~^D to break out.  */

static struct ttystate ttystate;

static void
cleanup_tty()
{  printf("\r\n[Exiting connect mode]\r\n");
  /*SERIAL_RESTORE(0, &ttystate);*/
}

static void
connect_command (args, fromtty)
     char	*args;
     int	fromtty;
{
  fd_set readfds;
  int numfds;
  int c;
  char cur_esc = 0;

  dont_repeat();

  if (monitor_desc == NULL)
    error("monitor target not open.");
  
  if (args)
    fprintf("This command takes no args.  They have been ignored.\n");
	
  printf("[Entering connect mode.  Use ~. or ~^D to escape]\n");

  serial_raw(0, &ttystate);

  make_cleanup(cleanup_tty, 0);

  FD_ZERO(&readfds);

  while (1)
    {
      do
	{
	  FD_SET(0, &readfds);
	  FD_SET(monitor_desc, &readfds);
	  numfds = select(sizeof(readfds)*8, &readfds, 0, 0, 0);
	}
      while (numfds == 0);

      if (numfds < 0)
	perror_with_name("select");

      if (FD_ISSET(0, &readfds))
	{			/* tty input, send to monitor */
	  c = getchar();
	  if (c < 0)
	    perror_with_name("connect");

	  printf_monitor("%c", c);
	  switch (cur_esc)
	    {
	    case 0:
	      if (c == '\r')
		cur_esc = c;
	      break;
	    case '\r':
	      if (c == '~')
		cur_esc = c;
	      else
		cur_esc = 0;
	      break;
	    case '~':
	      if (c == '.' || c == '\004')
		return;
	      else
		cur_esc = 0;
	    }
	}

      if (FD_ISSET(monitor_desc, &readfds))
	{
	  while (1)
	    {
	      c = readchar(0);
	      if (c < 0)
		break;
	      putchar(c);
	    }
	  fflush(stdout);
	}
    }
}
#endif

/*
 * Define the monitor command strings. Since these are passed directly
 * through to a printf style function, we need can include formatting
 * strings. We also need a CR or LF on the end.
 */
struct monitor_ops rom68k_cmds = {
  "go \r",				/* execute or usually GO command */
  "go \r",				/* continue command */
  "st \r",				/* single step */
  "db %x\r",				/* set a breakpoint */
  "cb %x\r",				/* clear a breakpoint */
  "pm %x\r",				/* set memory to a value */
  "pm %x\r",				/* display memory */
  "-%08X  ",				/* prompt memory commands use */
  "pr %s %x\r",				/* set a register */
  ":  ",				/* delimiter between registers */
  "pr %s\r",				/* read a register */
  "dc \r",				/* download command */
  "ROM68K :->",				/* monitor command prompt */
  "=",					/* end-of-command delimitor */
  ".\r"					/* optional command terminator */
};

struct monitor_ops bug_cmds = {
  "go \r",				/* execute or usually GO command */
  "go \r",				/* continue command */
  "gn \r",				/* single step */
  "br %x\r",				/* set a breakpoint */
  "nobr %x\r",				/* clear a breakpoint */
  "mm %x\r",				/* set memory to a value */
  "mm %x\r",				/* display memory */
  "%08X",				/* prompt memory commands use */
  "rs %s %x\r",				/* set a register */
  "=",					/* delimiter between registers */
  "rm %s\r",				/* read a register */
  "lo 0\r",				/* download command */
  "Bug>",				/* monitor command prompt */
  "? ",					/* end-of-command delimitor */
  ".\r"					/* optional command terminator */
};

/* Define the target subroutine names */
struct monitor_ops mon68_cmds = {
  "",				/* execute or usually GO command */
  "",				/* continue command */
  "",				/* single step */
  "",				/* set a breakpoint */
  "",				/* clear a breakpoint */
  "",				/* set memory to a value */
  "",				/* display memory */
  "",				/* set a register */
  "",				/* delimiter between registers */	
  "",				/* read a register */
  "",				/* download command */
  ">",				/* monitor command prompt */
  "",					/* end-of-command delimitor */
  ""					/* optional command terminator */
};

struct target_ops rom68k_ops = {
  "rom68k",
  "Integrated System's ROM68K remote debug monitor",
  "Use a remote computer running the ROM68K debug monitor.\n\
Specify the serial device it is connected to (e.g. /dev/ttya).",
  rom68k_open,
  monitor_close, 
  0,
  monitor_detach,
  monitor_resume,
  monitor_wait,
  monitor_fetch_register,
  monitor_store_register,
  monitor_prepare_to_store,
  monitor_xfer_inferior_memory,
  monitor_files_info,
  monitor_insert_breakpoint,
  monitor_remove_breakpoint,	/* Breakpoints */
  0,
  0,
  0,
  0,
  0,				/* Terminal handling */
  monitor_kill,
  monitor_load,			/* load */
  0,				/* lookup_symbol */
  monitor_create_inferior,
  monitor_mourn_inferior,
  0,				/* can_run */
  0, 				/* notice_signals */
  process_stratum,
  0,				/* next */
  1,
  1,
  1,
  1,
  1,				/* all mem, mem, stack, regs, exec */
  0,
  0,				/* Section pointers */
  OPS_MAGIC,			/* Always the last thing */
};

struct target_ops monitor_bug_ops = {
  "bug",
  "Motorola's BUG remote serial debug monitor",
  "Use a remote computer running Motorola's BUG debug monitor.\n\
Specify the serial device it is connected to (e.g. /dev/ttya).",
  bug_open,
  monitor_close, 
  0,
  monitor_detach,
  monitor_resume,
  monitor_wait,
  monitor_fetch_register,
  monitor_store_register,
  monitor_prepare_to_store,
  monitor_xfer_inferior_memory,
  monitor_files_info,
  monitor_insert_breakpoint,
  monitor_remove_breakpoint,	/* Breakpoints */
  0,
  0,
  0,
  0,
  0,				/* Terminal handling */
  monitor_kill,
  monitor_load,			/* load */
  0,				/* lookup_symbol */
  monitor_create_inferior,
  monitor_mourn_inferior,
  0,				/* can_run */
  0, 				/* notice_signals */
  process_stratum,
  0,				/* next */
  1,
  1,
  1,
  1,
  1,				/* all mem, mem, stack, regs, exec */
  0,
  0,				/* Section pointers */
  OPS_MAGIC,			/* Always the last thing */
};

struct target_ops mon68_ops = {
  "mon68",
  "Intermetric's MON68 remote serial debug monitor",
  "Use a remote computer running the MON68 debug monitor.\n\
Specify the serial device it is connected to (e.g. /dev/ttya).",
  mon68_open,
  monitor_close, 
  0,
  monitor_detach,
  monitor_resume,
  monitor_wait,
  monitor_fetch_register,
  monitor_store_register,
  monitor_prepare_to_store,
  monitor_xfer_inferior_memory,
  monitor_files_info,
  monitor_insert_breakpoint,
  monitor_remove_breakpoint,	/* Breakpoints */
  0,
  0,
  0,
  0,
  0,				/* Terminal handling */
  monitor_kill,
  monitor_load,			/* load */
  0,				/* lookup_symbol */
  monitor_create_inferior,
  monitor_mourn_inferior,
  0,				/* can_run */
  0, 				/* notice_signals */
  process_stratum,
  0,				/* next */
  1,
  1,
  1,
  1,
  1,				/* all mem, mem, stack, regs, exec */
  0,
  0,				/* Section pointers */
  OPS_MAGIC,			/* Always the last thing */
};

void
_initialize_remote_monitors ()
{
  add_show_from_set (
                     add_set_cmd ("hash", no_class, var_boolean,
                                  (char *)&hashmark,
				  "Set display of activity while downloading a file.\n\
When enabled, a period \'.\' is displayed.",
                                  &setlist),
		     &showlist);

  /* generic monitor command */
  add_com ("monitor <command>", class_obscure, monitor_command,
	   "Send a command to the debug monitor."); 
#if 0
  add_com ("connect", class_obscure, connect_command,
	   "Connect the terminal directly up to a serial based command monitor.\n\
Use <CR>~. or <CR>~^D to break out.");
#endif

  add_target (&rom68k_ops);
/*  add_target (&mon68_ops); */
  add_target (&monitor_bug_ops);
}
