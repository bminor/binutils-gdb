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

/* This file was derived from various remote-* modules. It is a collection
   of generic support functions so GDB can talk directly to a ROM based
   monitor. This saves use from having to hack an exception based handler
   into existance, and makes for quick porting.

   This module talks to a debug monitor called 'MONITOR', which
   We communicate with MONITOR via either a direct serial line, or a TCP
   (or possibly TELNET) stream to a terminal multiplexor,
   which in turn talks to the target board.
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

extern void make_xmodem_packet();
extern void print_xmodem_packet();

struct monitor_ops *current_monitor;
extern struct cmd_list_element *setlist;
extern struct cmd_list_element *unsetlist;
struct cmd_list_element *showlist;
extern char *version;
extern char *host_name;
extern char *target_name;

static int hashmark;				/* flag set by "set hash" */

#define LOG_FILE "monitor.log"
#if defined (LOG_FILE)
FILE *log_file;
#endif

static int timeout = 24;

/* 
 * Descriptor for I/O to remote machine.  Initialize it to NULL so that
 * monitor_open knows that we don't have a file open when the program starts.
 */
static serial_t monitor_desc = NULL;

/* sets the download protocol, choices are srec, generic, boot */
char *loadtype;
static char *loadtype_str;
static void set_loadtype_command();
static void monitor_load_srec();
static int monitor_write_srec();

/*
 * these definitions are for xmodem protocol
 */
#define SOH	0x01
#define ACK	0x06
#define NAK	0x15
#define EOT	0x04
#define CANCEL	0x18
#define GETACK		getacknak(ACK)
#define GETNAK		getacknak(NAK)
#define XMODEM_DATASIZE	128		/* the data   size is ALWAYS 128 */
#define XMODEM_PACKETSIZE	131	/* the packet size is ALWAYS 132 (zero based) */
#define XMODEM		1

/*
 * set_loadtype_command -- set the type for downloading. Check to make
 *	sure you have a support protocol for this target.
 */
static void
set_loadtype_command (ignore, from_tty, c)
     char *ignore;
     int from_tty;
     struct cmd_list_element *c;
{
  char *tmp;
  char *type;
  if (STREQ (LOADTYPES, "")) {
    error ("No loadtype set");
    return;
  }
  
  tmp = savestring (LOADTYPES, strlen(LOADTYPES));
  type = strtok(tmp, ",");
  if (STREQ (type, (*(char **) c->var))) {
      loadtype_str = savestring (*(char **) c->var, strlen (*(char **) c->var));
      return;
    }
  
  while ((type = strtok (NULL, ",")) != (char *)NULL) {
    if (STREQ (type, (*(char **) c->var)))
      loadtype_str = savestring (*(char **) c->var, strlen (*(char **) c->var));
    return;
  }
  free (tmp);
  error ("Loadtype \"%s\" does not exist.", (*(char **) c->var));
}

/*
 * printf_monitor -- send data to monitor.  Works just like printf.
 */
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

  debuglogs (1, "printf_monitor(), Sending: \"%s\".", buf);

  if (SERIAL_WRITE(monitor_desc, buf, strlen(buf)))
    fprintf(stderr, "SERIAL_WRITE failed: %s\n", safe_strerror(errno));
}
/*
 * write_monitor -- send raw data to monitor.
 */
static void
write_monitor(data, len)
     char data[];
     int len;
{
  if (SERIAL_WRITE(monitor_desc, data, len))
    fprintf(stderr, "SERIAL_WRITE failed: %s\n", safe_strerror(errno));
 
  *(data + len+1) = '\0';
  debuglogs (1, "write_monitor(), Sending: \"%s\".", data);

}

/*
 * debuglogs -- deal with debugging info to multiple sources. This takes
 *	two real args, the first one is the level to be compared against 
 *	the sr_get_debug() value, the second arg is a printf buffer and args
 *	to be formatted and printed. A CR is added after each string is printed.
 */
static void
debuglogs(va_alist)
     va_dcl
{
  va_list args;
  char *pattern, *p;
  char buf[200];
  char newbuf[300];
  int level, i;

  va_start(args);

  level = va_arg(args, int);			/* get the debug level */
  if ((level <0) || (level > 100)) {
    error ("Bad argument passed to debuglogs(), needs debug level");
    return;
  }
      
  pattern = va_arg(args, char *);		/* get the printf style pattern */

  vsprintf(buf, pattern, args);			/* format the string */
  
  /* convert some characters so it'll look right in the log */
  p = newbuf;
  for (i=0 ; buf[i] != '\0'; i++) {
    switch (buf[i]) {
    case '\n':					/* newlines */
      *p++ = '\\';
      *p++ = 'n';
      continue;
    case '\r':					/* carriage returns */
      *p++ = '\\';
      *p++ = 'r';
      continue;
    case '\033':				/* escape */
      *p++ = '\\';
      *p++ = 'e';
      continue;
    case '\t':					/* tab */
      *p++ = '\\';
      *p++ = 't';
      continue;
    case '\b':					/* backspace */
      *p++ = '\\';
      *p++ = 'b';
      continue;
    default:					/* no change */
      *p++ = buf[i];
    }

    if (buf[i] < 26) {				/* modify control characters */
      *p++ = '^';
      *p++ = buf[i] + 'A';
      continue;
    }
  }
  *p = '\0';					/* terminate the string */

  if (sr_get_debug() > level)
    puts (newbuf);

#ifdef LOG_FILE					/* write to the monitor log */
  if (log_file != 0x0) {
    fputs (newbuf, log_file);
    fputc ('\n', log_file);
    fflush (log_file);
  }
#endif
}

/* readchar -- read a character from the remote system, doing all the fancy
 *	timeout stuff.
 */
static int
readchar(timeout)
     int timeout;
{
  int c;

  c = SERIAL_READCHAR(monitor_desc, timeout);

  if (sr_get_debug() > 5)
    putchar(c & 0x7f);

#ifdef LOG_FILE
  if (isascii (c))
    putc(c & 0x7f, log_file);
#endif

  if (c >= 0)
    return c & 0x7f;

  if (c == SERIAL_TIMEOUT) {
    if (timeout == 0)
      return c;		/* Polls shouldn't generate timeout errors */
    error("Timeout reading from remote system.");
#ifdef LOG_FILE
      fputs ("ERROR: Timeout reading from remote system", log_file);
#endif
  }
  perror_with_name("remote-monitor");
}

/* 
 * expect --  scan input from the remote system, until STRING is found.
 *	If DISCARD is non-zero, then discard non-matching input, else print
 *	it out. Let the user break out immediately.
 */
static void
expect (string, discard)
     char *string;
     int discard;
{
  char *p = string;
  int c;


  debuglogs (1, "Expecting \"%s\".", string);

  immediate_quit = 1;
  while (1) {
    c = readchar(timeout);
    if (!isascii (c))
      continue;
    if (c == *p++) {
      if (*p == '\0') {
	immediate_quit = 0;
	debuglogs (4, "Matched");
	return;
      }
    } else {
      if (!discard) {
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
  expect (PROMPT, discard);
}

/*
 * junk -- ignore junk characters. Returns a 1 if junk, 0 otherwise
 */
static int
junk(ch)
     char ch;
{
  switch (ch) {
  case '\0':
  case ' ':
  case '-':
  case '\t':
  case '\r':
  case '\n':
    if (sr_get_debug() > 5)
      debuglogs (5, "Ignoring \'%c\'.", ch);
    return 1;
  default:
    if (sr_get_debug() > 5)
      debuglogs (5, "Accepting \'%c\'.", ch);
    return 0;
  }
}

/* 
 *  get_hex_digit -- Get a hex digit from the remote system & return its value.
 *		If ignore is nonzero, ignore spaces, newline & tabs.
 */
static int
get_hex_digit(ignore)
     int ignore;
{
  static int ch;
  while (1) {
    ch = readchar(timeout);
    if (junk(ch))
      continue;
    if (sr_get_debug() > 4)
      debuglogs (4, "get_hex_digit() got a 0x%x(%c)", ch, ch);

    if (ch >= '0' && ch <= '9')
      return ch - '0';
    else if (ch >= 'A' && ch <= 'F')
      return ch - 'A' + 10;
    else if (ch >= 'a' && ch <= 'f')
      return ch - 'a' + 10;
    else if (ch == ' ' && ignore)
      ;
    else {
      expect_prompt(1);
      error("Invalid hex digit from remote system. (0x%x)", ch);
    }
  }
}

/* get_hex_byte -- Get a byte from monitor and put it in *BYT. 
 *	Accept any number leading spaces.
 */
static void
get_hex_byte (byt)
     char *byt;
{
  int val;

  val = get_hex_digit (1) << 4;
  debuglogs (4, "get_hex_digit() -- Read first nibble 0x%x", val);
 
  val |= get_hex_digit (0);
  debuglogs (4, "get_hex_digit() -- Read second nibble 0x%x", val);
  *byt = val;
  
  debuglogs (4, "get_hex_digit() -- Read a 0x%x", val);
}

/* 
 * get_hex_word --  Get N 32-bit words from remote, each preceded by a space,
 *	and put them in registers starting at REGNO.
 */
static int
get_hex_word ()
{
  long val;
  int i;

  val = 0;
  for (i = 0; i < 8; i++)
    val = (val << 4) + get_hex_digit (i == 0);
  
  debuglogs (4, "get_hex_word() got a 0x%x.", val);

  return val;
}

/* This is called not only when we first attach, but also when the
   user types "run" after having attached.  */
void
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

  debuglogs (1, "create_inferior(exexfile=%s, args=%s, env=%s)", execfile, args, env);

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

/*
 * monitor_open -- open a connection to a remote debugger.
 *	NAME is the filename used for communication.
 */
static int baudrate = 9600;
static char dev_name[100];

void
monitor_open(args, name, from_tty)
     char *args;
     char *name;
     int from_tty;
{

  if (args == NULL)
    error ("Use `target %s DEVICE-NAME' to use a serial port, or \n\
`target %s HOST-NAME:PORT-NUMBER' to use a network connection.", name, name);

/*  if (is_open) */
    monitor_close(0);

  strcpy(dev_name, args);
  monitor_desc = SERIAL_OPEN(dev_name);

  if (monitor_desc == NULL)
    perror_with_name(dev_name);

  if (baud_rate != -1) {
    if (SERIAL_SETBAUDRATE (monitor_desc, baud_rate)) {
      SERIAL_CLOSE (monitor_desc);
      perror_with_name (name);
    }
  }
  
  SERIAL_RAW(monitor_desc);

#if defined (LOG_FILE)
  log_file = fopen (LOG_FILE, "w");
  if (log_file == NULL)
    perror_with_name (LOG_FILE);
  fprintf_filtered (log_file, "GDB %s (%s", version, host_name);
  fprintf_filtered (log_file, " --target %s)\n", target_name);
  fprintf_filtered (log_file, "Remote target %s connected to %s\n\n", TARGET_NAME, dev_name);
#endif

  /* wake up the monitor and see if it's alive */
  printf_monitor(INIT_CMD);
  expect_prompt(1);		/* See if we get a prompt */

  /* try again to be sure */
  printf_monitor(INIT_CMD);
  expect_prompt(1);		/* See if we get a prompt */

  if (from_tty)
    printf("Remote target %s connected to %s\n", TARGET_NAME, dev_name);
}

/*
 * monitor_close -- Close out all files and local state before this
 *	target loses control.
 */

void
monitor_close (quitting)
     int quitting;
{
  SERIAL_CLOSE(monitor_desc);
  monitor_desc = NULL;

  debuglogs (1, "monitor_close (quitting=%d)", quitting);

#if defined (LOG_FILE)
  if (log_file) {
    if (ferror(log_file))
      fprintf(stderr, "Error writing log file.\n");
    if (fclose(log_file) != 0)
      fprintf(stderr, "Error closing log file.\n");
  }
#endif
}

/* 
 * monitor_detach -- terminate the open connection to the remote
 *	debugger. Use this when you want to detach and do something
 *	else with your gdb.
 */
void
monitor_detach (from_tty)
     int from_tty;
{

  debuglogs (1, "monitor_detach ()");

  pop_target();		/* calls monitor_close to do the real work */
  if (from_tty)
    printf ("Ending remote %s debugging\n", target_shortname);
}

/*
 * monitor_attach -- attach GDB to the target.
 */
void
monitor_attach (args, from_tty)
     char *args;
     int from_tty;
{
  if (from_tty)
    printf ("Starting remote %s debugging\n", target_shortname);
 
  debuglogs (1, "monitor_attach (args=%s)", args);
  
  printf_monitor (GO_CMD);
  /* swallow the echo.  */
  expect (GO_CMD, 1);
}
  
/*
 * monitor_resume -- Tell the remote machine to resume.
 */
void
monitor_resume (pid, step, sig)
     int pid, step;
     enum target_signal sig;
{
  debuglogs (1, "monitor_resume (step=%d, sig=%d)", step, sig);

  if (step) {
    printf_monitor (STEP_CMD);
  } else {
    printf_monitor (CONT_CMD);
  }
}

/*
 * monitor_wait -- Wait until the remote machine stops, then return,
 *          storing status in status just as `wait' would.
 */
int
monitor_wait (pid, status)
     int pid;
     struct target_waitstatus *status;
{
  int old_timeout = timeout;

  debuglogs(1, "monitor_wait (), printing extraneous text.");
  
  status->kind = TARGET_WAITKIND_EXITED;
  status->value.integer = 0;

  timeout = 0;		/* Don't time out -- user program is running. */

  expect_prompt(0);    /* Wait for prompt, outputting extraneous text */
  debuglogs (4, "monitor_wait(), got the prompt.");

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

  for (p = REGNAMES(regno); *p; p++)
    *b++ = tolower(*p);

  *b = '\000';

  debuglogs (5, "Got name \"%s\" from regno #%d.", buf, regno);

  return buf;
}

/*
 * monitor_fetch_registers -- read the remote registers into the
 *	block regs.
 */
void
monitor_fetch_registers ()
{
  int regno;

  /* yeah yeah, i know this is horribly inefficient.  but it isn't done
     very often...  i'll clean it up later.  */

  for (regno = 0; regno <= PC_REGNUM; regno++)
    monitor_fetch_register(regno);
}

/* 
 * monitor_fetch_register -- fetch register REGNO, or all registers if REGNO
 *	is -1. Returns errno value.
 */
void
monitor_fetch_register (regno)
     int regno;
{
  int val, j;

  debuglogs (1, "monitor_fetch_register (reg=%s)", get_reg_name (regno));

  if (regno < 0) {
    monitor_fetch_registers ();
  } else {
    char *name = get_reg_name (regno);
    if (STREQ(name, ""))
      return;
    printf_monitor (ROMCMD(GET_REG), name);	/* send the command */
    expect (name, 1);				/* then strip the leading garbage */
    if (*ROMDELIM(GET_REG) != 0) {		/* if there's a delimiter */
      expect (ROMDELIM(GET_REG), 1);
    }
    
    val =  get_hex_word();			/* get the value, ignore junk */
    supply_register (regno, (char *) &val);
    
    if (*ROMDELIM(GET_REG) != 0) {
/***  expect (ROMRES(GET_REG)); ***/
      printf_monitor (CMD_END);
    }
    expect_prompt (1);
  }
  return;
}

/* Store the remote registers from the contents of the block REGS.  */

void
monitor_store_registers ()
{
  int regno;

  debuglogs (1, "monitor_store_registers()");

  for (regno = 0; regno <= PC_REGNUM; regno++)
    monitor_store_register(regno);

  registers_changed ();
}

/* 
 * monitor_store_register -- store register REGNO, or all if REGNO == 0.
 *	return errno value.
 */
void
monitor_store_register (regno)
     int regno;
{
  char *name;
  int i;

  i = read_register(regno);

  debuglogs (1, "monitor_store_register (regno=%d)", regno);

  if (regno < 0)
    monitor_store_registers ();
  else {
      debuglogs (3, "Setting register %s to 0x%x", get_reg_name (regno), read_register (regno));
    
    name = get_reg_name (regno);
    if (STREQ(name, ""))
      return;
    printf_monitor (ROMCMD(SET_REG), name, read_register(regno));
    expect (name, 1);				/* strip the leading garbage */
    if (*ROMDELIM(SET_REG) != 0) {		/* if there's a delimiter */
      expect (ROMDELIM(SET_REG), 1);
      get_hex_word(1);
      printf_monitor ("%d%s\n", i, CMD_END);
    }
    expect_prompt (1);
  }
  return;
  
#if 0
      printf_monitor (SET_REG, get_reg_name (regno),
		      read_register (regno));
      expect_prompt (1);
    }
#endif
}

/* Get ready to modify the registers array.  On machines which store
   individual registers, this doesn't need to do anything.  On machines
   which store all the registers in one fell swoop, this makes sure
   that registers contains all the registers from the program being
   debugged.  */

void
monitor_prepare_to_store ()
{
  /* Do nothing, since we can store individual regs */
}

void
monitor_files_info ()
{
  printf ("\tAttached to %s at %d baud.\n",
	  dev_name, baudrate);
}

/*
 * monitor_write_inferior_memory -- Copy LEN bytes of data from debugger
 *	memory at MYADDR to inferior's memory at MEMADDR.  Returns length moved.
 */
int
monitor_write_inferior_memory (memaddr, myaddr, len)
     CORE_ADDR memaddr;
     unsigned char *myaddr;
     int len;
{
  int i;
  char buf[10];

  debuglogs (1, "monitor_write_inferior_memory (memaddr=0x%x, myaddr=0x%x, len=%d)", memaddr, myaddr, len);

  for (i = 0; i < len; i++) {
    printf_monitor (ROMCMD(SET_MEM), memaddr + i, myaddr[i] );
    if (*ROMDELIM(SET_MEM) != 0) {		/* if there's a delimiter */
      expect (ROMDELIM(SET_MEM), 1);
      expect (CMD_DELIM);
      printf_monitor ("%x", myaddr[i]);
    }
/***    printf_monitor ("%x", myaddr[i]); ***/
    if (sr_get_debug() > 1)
      printf ("\nSet 0x%x to 0x%x\n", memaddr + i, myaddr[i]);
    if (*ROMDELIM(SET_MEM) != 0) {
      expect (CMD_DELIM);
      printf_monitor (CMD_END);
    }
    expect_prompt (1);
  }
  return len;
}

/*
 * monitor_read_inferior_memory -- read LEN bytes from inferior memory
 *	at MEMADDR.  Put the result at debugger address MYADDR.  Returns
 *	length moved.
 */
int
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

  debuglogs (1, "monitor_read_inferior_memory (memaddr=0x%x, myaddr=0x%x, len=%d)", memaddr, myaddr, len);

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
  while (count < len) {
    len_this_pass = 16;
    if ((startaddr % 16) != 0)
      len_this_pass -= startaddr % 16;
    if (len_this_pass > (len - count))
      len_this_pass = (len - count);

    debuglogs (3, "Display %d bytes at %x", len_this_pass, startaddr);
    
    for (i = 0; i < len_this_pass; i++) {
      printf_monitor (ROMCMD(GET_MEM), startaddr, startaddr);
      sprintf (buf, ROMCMD(GET_MEM), startaddr, startaddr);
      if (*ROMDELIM(GET_MEM) != 0) {		/* if there's a delimiter */
	expect (ROMDELIM(GET_MEM), 1);
      } else {
	sprintf (buf, ROMCMD(GET_MEM), startaddr, startaddr);
	expect (buf,1);				/* get the command echo */
	get_hex_word(1);			/* strip away the address */
      }
      get_hex_byte (&myaddr[count++]);		/* get the value at this address */

      if (*ROMDELIM(GET_MEM) != 0) {
	printf_monitor (CMD_END);
      }
      expect_prompt (1);
      startaddr += 1;
    }
  }
  return len;
}

/* FIXME-someday!  merge these two.  */
int
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

void
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

void
monitor_mourn_inferior ()
{
  remove_breakpoints ();
  generic_mourn_inferior ();	/* Do all the proper things now */
}

#define MAX_MONITOR_BREAKPOINTS 16

extern int memory_breakpoint_size;
static CORE_ADDR breakaddr[MAX_MONITOR_BREAKPOINTS] = {0};

/*
 * monitor_insert_breakpoint -- add a breakpoint
 */
int
monitor_insert_breakpoint (addr, shadow)
     CORE_ADDR addr;
     char *shadow;
{
  int i;

  debuglogs (1, "monitor_insert_breakpoint() addr = 0x%x", addr);

  for (i = 0; i <= MAX_MONITOR_BREAKPOINTS; i++) {
    if (breakaddr[i] == 0) {
      breakaddr[i] = addr;
      if (sr_get_debug() > 4)
	printf ("Breakpoint at %x\n", addr);
      monitor_read_inferior_memory(addr, shadow, memory_breakpoint_size);
      printf_monitor(SET_BREAK_CMD, addr);
      expect_prompt(1);
      return 0;
    }
  }

  fprintf(stderr, "Too many breakpoints (> 16) for monitor\n");
  return 1;
}

/*
 * _remove_breakpoint -- Tell the monitor to remove a breakpoint
 */
int
monitor_remove_breakpoint (addr, shadow)
     CORE_ADDR addr;
     char *shadow;
{
  int i;

  debuglogs (1, "monitor_remove_breakpoint() addr = 0x%x", addr);

  for (i = 0; i < MAX_MONITOR_BREAKPOINTS; i++) {
    if (breakaddr[i] == addr) {
      breakaddr[i] = 0;
      /* some monitors remove breakpoints based on the address */
      if (CLR_BREAK_ADDR)   
	printf_monitor(CLR_BREAK_CMD, addr);
      else
	printf_monitor(CLR_BREAK_CMD, i);
      expect_prompt(1);
      return 0;
    }
  }
  fprintf(stderr, "Can't find breakpoint associated with 0x%x\n", addr);
  return 1;
}

/* monitor_load -- load a file. This file determines which of the
 *	supported formats to use. The current types are:
 *	FIXME: not all types supported yet.
 *	default - reads any file using bfd and writes it to memory. This
 *		is really slow.
 *	srec    - reads binary file using bfd and writes it as an
 *		ascii srecord.
 *	xmodem-bin - reads a binary file using bfd, and  downloads it
 *		 using xmodem protocol.
 *	xmodem-srec - reads a binary file using bfd, and after converting
 *		 it downloads it as an srecord using xmodem protocol.
 *	ascii-srec - reads a ascii srecord file and downloads it
 *		without a change.
 *	ascii-xmodem - reads a ascii file and downloads using xmodem
 *		protocol.
 */
void
monitor_load (file, fromtty)
    char *file;
    int  fromtty;
{
  FILE *download;
  int i, bytes_read;

  debuglogs (1, "Loading %s to monitor", file);

  if (STREQ (loadtype_str, "default")) {	/* default, load a binary */
    gr_load_image (file, fromtty);		/* by writing it into memory */
  }

  if (STREQ (loadtype_str, "srec")) {		/* load an srecord by converting */
    monitor_load_srec(file, 0);			/* if from a binary */
  }

  if (STREQ (loadtype_str, "ascii-srec")) {	/* load an srecord file */
    monitor_load_ascii_srec(file, fromtty);		/* if from a binary */
  }

  if (STREQ (loadtype_str, "xmodem-srec")) {	/* load an srecord using the */
    monitor_load_srec(file, XMODEM);
  }
}

/*
 * monitor_load_ascii_srec -- download an ASCII srecord file.
 */
#define DOWNLOAD_LINE_SIZE 100
int
monitor_load_ascii_srec (file, fromtty)
    char *file;
    int fromtty;
{
  FILE *download;
  char buf[DOWNLOAD_LINE_SIZE];
  int i, bytes_read;

  debuglogs (1, "Loading an ASCII srecord file, %s.", file);

  download = fopen (file, "r");
  if (download == NULL) {
    error ("%s Does not exist", file);
    return;
  }

  printf_monitor (LOAD_CMD);
  sleep(1);
  while (!feof (download)) {
    bytes_read = fread (buf, sizeof (char), DOWNLOAD_LINE_SIZE, download);
    if (hashmark) {
      putchar ('.');
      fflush (stdout);
    }
    if (SERIAL_WRITE(monitor_desc, buf, bytes_read)) {
      fprintf(stderr, "SERIAL_WRITE failed: (while downloading) %s\n", safe_strerror(errno));
      break;
    }
    i = 0;
    while (i++ <=200) {} ;     			/* Ugly HACK, probably needs flow control */
    if (bytes_read < DOWNLOAD_LINE_SIZE) {
      if (!feof (download))
	error ("Only read %d bytes\n", bytes_read);
      break;
    }
  }
  
  if (hashmark) {
    putchar ('\n');
  }
  if (!feof (download))
    error ("Never got EOF while downloading");
  expect_prompt(1);
  fclose (download);
}

/* 
 * monitor_command -- put a command string, in args, out to MONITOR.
 *	Output from MONITOR is placed on the users terminal until the
 *	prompt is seen. FIXME: We read the charcters ourseleves here
 *	cause of a nasty echo.
 */
void
monitor_command (args, fromtty)
     char	*args;
     int	fromtty;
{

  char *p;
  char c, cp;
  p = PROMPT;

  debuglogs (1, "monitor_command (args=%s)", args);

  if (monitor_desc == NULL)
    error("monitor target not open.");

  if (!args)
    error("Missing command.");
	
  printf_monitor ("%s\n", args);

  expect_prompt(0);
}

/*
 * monitor_load_srec -- download a binary file by converting it to srecords. This
 *	will also use xmodem to download the resulting file.
 *
 *	A download goes like this when using xmodem:
 *	Receiver:		Sender
 *	NAK ---------->
 *		<-------- (packet)	[SOH|1|1|data|SUM]
 *	ACK ---------->
 *		<-------- (packet)	[SOH|2|2|data|SUM]
 *	ACK ---------->
 *		<-------- EOT
 *	ACK ---------->
 *
 *	ACK = 0x06
 *	NAK = 0x15
 *	EOT = 0x04
 *
 */
static void
monitor_load_srec (args, protocol)
     char *args;
     int protocol;
{
  bfd *abfd;
  asection *s;
  char buffer[1024];
  char srec[1024];
  char packet[XMODEM_PACKETSIZE];
  int i;
  int retries;
  int type = 0;					/* default to a type 0, header record */
  int srec_frame = 57;				/* FIXME: this must be 57 There is 12 bytes
						 of header, and 2 bytes of checksum at the end.
						 The problem is an xmodem packet holds exactly
						 128 bytes. */

  abfd = bfd_openr (args, 0);
  if (!abfd) {
    printf_filtered ("Unable to open file %s\n", args);
    return;
  }

  if (bfd_check_format (abfd, bfd_object) == 0) {
    printf_filtered ("File is not an object file\n");
    return;
  }
  
  printf_monitor (LOAD_CMD);			/* tell the monitor to load */
  if (protocol == XMODEM) {			/* get the NAK from the target */
    if (GETNAK) {
      debuglogs (3, "Got the NAK to start loading");
    } else {
      printf_monitor ("%c", EOT);
      debuglogs (3, "Never got the NAK to start loading");
      error ("Never got the NAK to start loading");
    }
  }
  
  s = abfd->sections;
  while (s != (asection *) NULL) {
    if (s->flags & SEC_LOAD) {
      char *buffer = xmalloc (srec_frame);
      printf_filtered ("%s\t: 0x%4x .. 0x%4x  ", s->name, s->vma, s->vma + s->_raw_size);
      fflush (stdout);
      for (i = 0; i < s->_raw_size; i += srec_frame) {
	if (srec_frame > s->_raw_size - i)
	  srec_frame = s->_raw_size - i;
	
	bfd_get_section_contents (abfd, s, buffer, i, srec_frame);
	monitor_make_srec (srec, type, s->vma + i, buffer, srec_frame);
	if (protocol == XMODEM) {		/* send a packet using xmodem */
	  make_xmodem_packet (packet, srec, XMODEM_DATASIZE);
	  write_monitor (packet, XMODEM_PACKETSIZE+1);
	  retries = 0;
	  while (retries++ <= 3) {
	    if (GETNAK) {			/* Resend packet */
	      debuglogs (3, "Got a NAK, resending packet");
	      sleep(1);
	      write_monitor (packet, XMODEM_PACKETSIZE+1); /* send it again */
	      if (GETACK)			/* ACKnowledged, get next data chunk */
		break;
	    }
	  }
	  if (retries >= 4) {		/* too many tries, must be hosed */
	    printf_monitor ("%c", EOT);
	    error ("Never got a ACK after sending an xmodem packet");
	  }
	} else {				/* no protocols at all */
	  printf_monitor ("%s\n", srec);
	}
	if (hashmark)
	  printf_filtered ("#");
	type = 3;				/* switch to a 4 byte address record */
	fflush (stdout);
      }
      printf_filtered ("\n");
      free (buffer);
    } else {
      debuglogs (3, "%s doesn't need to be loaded", s->name);
    }
    s = s->next;
  }
  
  /*
     write a type 7 terminator record. no data for a type 7,
     and there is no data, so len is 0. 
   */
  monitor_make_srec (srec, 7, abfd->start_address, "", 0);
  printf_monitor ("%s\n", srec);
  if (protocol == XMODEM) {
    printf_monitor ("%c", EOT);
    if (!GETACK)
      error ("Never got ACK after sending EOT");
  }

  if (hashmark) 
    putchar ('\n');
  
  expect_prompt ();
}

/*
 * getacknak -- get an ACK or a NAK from the target.
 *		returns 1 (true) or 0 (false) This is
 *		for xmodem. ANy string starting with "***"
 *		is an error message from the target.
 *	Here's a few from the WinBond w89k "Cougar" PA board.
 *		*** Too many errors found.
 *		*** Bad command
 *		*** Command syntax error
 */
int
getacknak (byte)
     int byte;
{
  char character;
  int i;
  
  i = 0;
  while (i++ < 60) {
    character = (char)readchar (0);
    if (character == 0xfffffffe) {		/* empty uart */
      if (sr_get_debug() > 3)
	putchar ('.');
      fflush (stdout);
      sleep (1);
      continue;
    }
    if (character == CANCEL) {			/* target aborted load */
      expect_prompt (0);
      error ("Got a CANCEL from the target.");
    }
    if (character == '*') {			/* look for missed error message */
      expect_prompt (0);
      error ("Got an error message from the target");
    }
    debuglogs (3, "Got a %s (0x%x or \'%c\'), expecting a %s.\n",
	       (character == ACK) ? "ACK" : (character == NAK) ? "NAK" : "BOGUS",
	       character,  character, (byte == ACK) ? "ACK" : "NAK");
    if (character == byte)			/* got what we wanted */
      return 1;
    if (character == ((byte == ACK) ? NAK : ACK)) {	/* got the opposite */
      debuglogs (3, "Got the opposite, wanted 0x%x, got a 0x%x", byte, character);
      return 0;
    }
    sleep (1); 
  }
  return 0;
}

/*
 * monitor_make_srec -- make an srecord. This writes each line, one at a
 *	time, each with it's own header and trailer line.
 *	An srecord looks like this:
 *
 * byte count-+     address
 * start ---+ |        |       data        +- checksum
 *	    | |        |                   |
 *	  S01000006F6B692D746573742E73726563E4
 *	  S315000448600000000000000000FC00005900000000E9
 *	  S31A0004000023C1400037DE00F023604000377B009020825000348D
 *	  S30B0004485A0000000000004E
 *	  S70500040000F6
 *
 *	S<type><length><address><data><checksum>
 *
 *      Where
 *      - length
 *        is the number of bytes following upto the checksum. Note that
 *        this is not the number of chars following, since it takes two
 *        chars to represent a byte.
 *      - type
 *        is one of:
 *        0) header record
 *        1) two byte address data record
 *        2) three byte address data record
 *        3) four byte address data record
 *        7) four byte address termination record
 *        8) three byte address termination record
 *        9) two byte address termination record
 *       
 *      - address
 *        is the start address of the data following, or in the case of
 *        a termination record, the start address of the image
 *      - data
 *        is the data.
 *      - checksum
 *	  is the sum of all the raw byte data in the record, from the length
 *        upwards, modulo 256 and subtracted from 255.
 */
int
monitor_make_srec (buffer, type, memaddr, myaddr, len)
     char *buffer;
     int type;
     CORE_ADDR memaddr;
     unsigned char *myaddr;
     int len;
{
  int checksum;
  int i;
  char *buf;

  buf = buffer;
  debuglogs (4, "monitor_make_srec (buffer=0x%x, type=%d, memaddr=0x%x, len=%d",
	                            buffer, type, memaddr, len); 
  checksum = 0;
  
  /*
     create the header for the srec. 4 is the number of bytes in the address,
     and 1 is the number of bytes in the count.
   */
  if (type == 0)				/* FIXME: type 0 is optional */
    type = 3;					/* so use data as it works */
  sprintf (buf, "S%d%02X%08X", type, len + 4 + 1, memaddr);
  buf += 12;
  
  checksum += (len + 4 + 1			/* calculate the checksum */
	       + (memaddr & 0xff)
	       + ((memaddr >>  8) & 0xff)
	       + ((memaddr >> 16) & 0xff)
	       + ((memaddr >> 24) & 0xff));
  
  for (i = 0; i < len; i++) {		/* build the srecord */
    sprintf (buf, "%02X", myaddr[i]);
    checksum += myaddr[i];
    buf += 2;
  }

  sprintf(buf, "%02X", ~checksum & 0xff);	/* add the checksum */
  debuglogs (3, "srec is \"%s\"", buffer);
  
  return(0);
}

/*
 * make_xmodem_packet -- this takes a 128 bytes of data and makes a packet
 *	out of it.
 *
 *	Each packet looks like this:
 *	+-----+-------+-------+------+-----+
 *	| SOH | Seq1. | Seq2. | data | SUM |
 *	+-----+-------+-------+------+-----+
 *	SOH  = 0x01
 *	Seq1 = The sequence number.
 *	Seq2 = The complement of the sequence number.
 *	Data = A 128 bytes of data.
 *	SUM  = Add the contents of the 128 bytes and use the low-order
 *	       8 bits of the result.
 */
void
make_xmodem_packet (packet, data, len)
     unsigned char packet[];
     unsigned char *data;
     int len;
{
  static int sequence = 1;
  int i, sum;
  unsigned char *buf;
  
  buf = data;
  /* build the packet header */
  packet[0] = SOH;
  packet[1] = sequence;
  packet[2] = 255 - sequence;
  sequence++;
#if 0
  packet[2] = ~sequence++;			/* the complement is the sequence checksum */
#endif
  
  sum = 0;					/* calculate the data checksum */
  for (i = 3; i <= len + 2; i++) {
    packet[i] = *buf;
    sum += *buf;
    buf++;
  }

  for (i = len+1 ; i <= XMODEM_DATASIZE ; i++) {	/* add padding for the rest of the packet */
    packet[i] = '0';
  }

  packet[XMODEM_PACKETSIZE] = sum & 0xff;	/* add the checksum */

  if (sr_get_debug() > 4)
    debuglogs (4, "The xmodem checksum is %d (0x%x)\n", sum & 0xff, sum & 0xff);
    print_xmodem_packet (packet);
}

/*
 * print_xmodem_packet -- print the packet as a debug check
 */
void
print_xmodem_packet(packet)
     char packet[];
{
  int i;
  static int lastseq;
  int sum;

  /* take apart the packet header the packet header */
  if (packet[0] == SOH) {
     ("SOH");
  } else {
    error ("xmodem: SOH is wrong");
  }
  
  /* check the sequence */
  if (packet[1] != 0) {
    lastseq = packet[1];
    if (packet[2] != ~lastseq)
      error ("xmodem: Sequence checksum is wrong");
    else
      printf_filtered (" %d %d", lastseq, ~lastseq);
  }
  
  /* check the data checksum */
  sum = 0;
  for (i = 3; i <= XMODEM_DATASIZE; i++) {
    sum += packet[i];
  }

  /* ignore the data */
#if 0
  printf (" [128 bytes of data] %d\n", sum & 0xff);
#endif
  printf_filtered (" [%s] %d\n", packet, sum & 0xff);

  if ((packet[XMODEM_PACKETSIZE] & 0xff) != (sum & 0xff)) {
    debuglogs (4, "xmodem: data checksum wrong, got a %d", packet[XMODEM_PACKETSIZE] & 0xff);
  }
  putchar ('\n');
}

/*
 * _initialize_remote_monitors -- setup a few addtitional commands that
 *		are usually only used by monitors.
 */
void
_initialize_remote_monitors ()
{
  struct cmd_list_element *c;

  /* this sets the type of download protocol */
  c = add_set_cmd ("loadtype", no_class, var_string, (char *)&loadtype_str,
       "Set the type of the remote load protocol.\n", &setlist);
  c->function.sfunc =  set_loadtype_command;
  add_show_from_set (c, &showlist);
  loadtype_str = savestring ("default", 8);

  add_show_from_set (add_set_cmd ("hash", no_class, var_boolean,
                                  (char *)&hashmark,
				  "Set display of activity while downloading a file.\n\
When enabled, a period \'.\' is displayed.",
                                  &setlist),
		     &showlist);

  /* generic monitor command */
  add_com ("monitor", class_obscure, monitor_command,
	   "Send a command to the debug monitor."); 
}
