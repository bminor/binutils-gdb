/* Remote debugging interface for boot monitors, for GDB.
   Copyright 1990, 1991, 1992, 1993, 1995 Free Software Foundation, Inc.
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
   which in turn talks to the target board.  */

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
#include "gdbcmd.h"
#include "inferior.h"

static void monitor_command PARAMS ((char *args, int fromtty));
static void monitor_load_srec PARAMS ((char *args, int protocol));
static int getacknak PARAMS ((int byte));

static void make_xmodem_packet PARAMS ((unsigned char *packet,
					unsigned char *data,
					int len));
static void print_xmodem_packet PARAMS ((char *packet));

static void monitor_load_ascii_srec PARAMS ((char *file, int fromtty));

static int monitor_make_srec PARAMS ((char *buffer, int type,
				      CORE_ADDR memaddr,
				      unsigned char *myaddr, int len));

static void monitor_fetch_register PARAMS ((int regno));
static void monitor_store_register PARAMS ((int regno));

static int from_hex PARAMS ((int a));
static unsigned long get_hex_word PARAMS ((void));

static struct monitor_ops *current_monitor;

static char *loadtype_str = "srec";
static char *loadproto_str = "none";

static int hashmark;				/* flag set by "set hash" */

static int timeout = 30;

static int expect PARAMS ((char *string, char *buf, int buflen));
static int expect_prompt PARAMS ((char *buf, int buflen));

/* Having this larger than 400 causes us to be incompatible with m68k-stub.c
   and i386-stub.c.  Normally, no one would notice because it only matters
   for writing large chunks of memory (e.g. in downloads).  Also, this needs
   to be more than 400 if required to hold the registers (see below, where
   we round it up based on REGISTER_BYTES).  */

#define PBUFSIZ 400

/* Descriptor for I/O to remote machine.  Initialize it to NULL so
   that monitor_open knows that we don't have a file open when the
   program starts.  */

static serial_t monitor_desc = NULL;

/* These definitions are for xmodem protocol. */

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

static unsigned char output_buf[0x200];
static int obp;

static void
debug_save_output (buf, len)
     unsigned char *buf;
     int len;
{
#if 0
  for (; len > 0; len--)
    output_buf[obp++ & 0x1ff] = *buf++;
#else
  fputs_unfiltered (buf, gdb_stdout);
#endif
}

static unsigned char input_buf[0x200];
static int ibp;

static void
debug_save_input_char (c)
     int c;
{
#if 0
  input_buf[ibp++ & 0x1ff] = c;
#else
  fputc_unfiltered (c, gdb_stdout);
#endif
}

/* printf_monitor -- send data to monitor.  Works just like printf. */

static void
printf_monitor (va_alist)
     va_dcl
{
  va_list args;
  char *pattern;
  char buf[PBUFSIZ];

  va_start (args);

  pattern = va_arg (args, char *);

  vsprintf (buf, pattern, args);

  if (remote_debug > 0)
    debug_save_output (buf, strlen (buf));

  if (strlen (buf) > PBUFSIZ)
    error ("printf_monitor(): string too long");
  if (SERIAL_WRITE(monitor_desc, buf, strlen (buf)))
    fprintf_unfiltered (stderr, "SERIAL_WRITE failed: %s\n", safe_strerror (errno));
}

/* Send raw data to monitor.  */

static void
write_monitor (data, len)
     char *data;
     int len;
{
  if (SERIAL_WRITE (monitor_desc, data, len))
    fprintf_unfiltered (stderr, "SERIAL_WRITE failed: %s\n", safe_strerror(errno));
 
  *(data + len + 1) = '\0';
}

/* Read a character from the remote system, doing all the fancy
   timeout stuff.  */

static int
readchar (timeout)
     int timeout;
{
  int c;

  c = SERIAL_READCHAR (monitor_desc, timeout);

  if (remote_debug > 0)
    debug_save_input_char (c & 0x7f);

  if (c >= 0)
    return c & 0x7f;

  if (c == SERIAL_TIMEOUT)
    {
      if (timeout == 0)
	return c;		/* Polls shouldn't generate timeout errors */
      error ("Timeout reading from remote system.");
    }
  perror_with_name ("remote-monitor");
}

/* Scan input from the remote system, until STRING is found.  If BUF is non-
   zero, then collect input until either STRING has been collected or BUFLEN
   chars have been collected.  If input overflows BUF because STRING can't be
   found, return -1, else return number of chars in BUF (including STRING).  */

static int
expect (string, buf, buflen)
     char *string;
     char *buf;
     int buflen;
{
  char *p = string;
  int obuflen = buflen;
  int c;

  immediate_quit = 1;
  while (1)
    {
      if (buf)
	{
	  if (buflen <= 0)
	    {
	      immediate_quit = 0;
	      return -1;
	    }

	  c = readchar (timeout);
	  *buf++ = c;
	  buflen--;
	}
      else
	c = readchar (timeout);

      if (c == *p++)
	{
	  if (*p == '\0')
	    {
	      immediate_quit = 0;

	      return obuflen - buflen;
	    }
	}
      else
	{
	  p = string;
	  if (c == *p)
	    p++;
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

static int
expect_prompt (buf, buflen)
     char *buf;
     int buflen;
{
  return expect (PROMPT, buf, buflen);
}

/* Get N 32-bit words from remote, each preceded by a space, and put
   them in registers starting at REGNO.  */

static unsigned long
get_hex_word ()
{
  unsigned long val;
  int i;
  int ch;

  do
    ch = readchar (timeout);
  while (isspace(ch));

  val = from_hex (ch);

  for (i = 7; i >= 1; i--)
    {
      ch = readchar (timeout);
      if (!isxdigit (ch))
	break;
      val = (val << 4) | from_hex (ch);
    }

  return val;
}

/* Open a connection to a remote debugger. NAME is the filename used
   for communication.  */

static char *dev_name;
static struct target_ops *targ_ops;

void
monitor_open (args, mon_ops, from_tty)
     char *args;
     struct monitor_ops *mon_ops;
     int from_tty;
{
  char *name;
  int i;

  targ_ops = mon_ops->target;
  name = targ_ops->to_shortname;

  if (!args)
    error ("Use `target %s DEVICE-NAME' to use a serial port, or \n\
`target %s HOST-NAME:PORT-NUMBER' to use a network connection.", name, name);

  target_preopen (from_tty);

  unpush_target (targ_ops);

  if (dev_name)
    free (dev_name);
  dev_name = strsave (args);

  monitor_desc = SERIAL_OPEN (dev_name);

  if (!monitor_desc)
    perror_with_name (dev_name);

  if (baud_rate != -1)
    {
      if (SERIAL_SETBAUDRATE (monitor_desc, baud_rate))
	{
	  SERIAL_CLOSE (monitor_desc);
	  perror_with_name (dev_name);
	}
    }
  
  SERIAL_RAW (monitor_desc);

  SERIAL_FLUSH_INPUT (monitor_desc);

  /* some systems only work with 2 stop bits */

  SERIAL_SETSTOPBITS (monitor_desc, mon_ops->stopbits);

  current_monitor = mon_ops;

  /* see if the target is alive. For a ROM monitor, we can just try to
     force the prompt to print a few times.  */

  /* wake up the monitor and see if it's alive */
  printf_monitor (mon_ops->init);
  expect_prompt (NULL, 0);	/* See if we get a prompt */

  /* try again to be sure */
  printf_monitor (mon_ops->init);
  expect_prompt (NULL, 0);	/* See if we get a prompt */

  /* Setup the suffixes for the `set remoteloadtype' command */

  add_show_from_set (add_set_enum_cmd ("remoteloadtype", no_class,
				       mon_ops->loadtypes,
				       (char *)&loadtype_str,
				       "Set the remote load type.",
				       &setlist),
		     &showlist);

  /* Setup the suffixes for the `set remoteloadprotocol' command */

  add_show_from_set (add_set_enum_cmd ("remoteloadprotocol", no_class,
				       mon_ops->loadprotos,
				       (char *)&loadproto_str,
				       "Set the remote load protocol.",
				       &setlist),
		     &showlist);

  if (from_tty)
    printf_unfiltered ("Remote target %s connected to %s\n", name, dev_name);

  push_target (targ_ops);

  inferior_pid = 42000;		/* Make run command think we are busy... */

  printf_monitor ("\r");

  start_remote ();
}

/* Close out all files and local state before this target loses
   control.  */

void
monitor_close (quitting)
     int quitting;
{
  if (monitor_desc)
    SERIAL_CLOSE (monitor_desc);
  monitor_desc = NULL;
}

/* Terminate the open connection to the remote debugger.  Use this
   when you want to detach and do something else with your gdb.  */

void
monitor_detach (args, from_tty)
     char *args;
     int from_tty;
{
  pop_target ();		/* calls monitor_close to do the real work */
  if (from_tty)
    printf_unfiltered ("Ending remote %s debugging\n", target_shortname);
}

/* Tell the remote machine to resume.  */

void
monitor_resume (pid, step, sig)
     int pid, step;
     enum target_signal sig;
{
  if (step)
    printf_monitor (STEP_CMD);
  else
    printf_monitor (CONT_CMD);
}

/* Wait until the remote machine stops, then return, storing status in
   status just as `wait' would.  */

int
monitor_wait (pid, status)
     int pid;
     struct target_waitstatus *status;
{
  int old_timeout = timeout;

  status->kind = TARGET_WAITKIND_EXITED;
  status->value.integer = 0;

  timeout = -1;		/* Don't time out -- user program is running. */

  expect_prompt (NULL, 0);	/* Wait for prompt, outputting extraneous text */

  status->kind = TARGET_WAITKIND_STOPPED;
  status->value.sig = TARGET_SIGNAL_TRAP;

  timeout = old_timeout;

  return inferior_pid;
}

/* Fetch register REGNO, or all registers if REGNO is -1. Returns
   errno value.  */

static void
monitor_fetch_register (regno)
     int regno;
{
  unsigned LONGEST val;
  unsigned char regbuf[MAX_REGISTER_RAW_SIZE];
  char buf[200];
  char *p, *p1;
  char *name;
  int resp_len;

  name = REGNAMES (regno);

  if (!name)
    return;

 /* send the register examine command */

  printf_monitor (current_monitor->getreg.cmd, name);

/* If TERM is present, we wait for that to show up.  Also, (if TERM is
   present), we will send TERM_CMD if that is present.  In any case, we collect
   all of the output into buf, and then wait for the normal prompt.  */

  if (current_monitor->getreg.term)
    {
      resp_len = expect (current_monitor->getreg.term, buf, sizeof buf); /* get response */

      if (resp_len <= 0)
	error ("monitor_fetch_register (%d):  excessive response from monitor: %.*s.",
	       regno, resp_len, buf);

      if (current_monitor->getreg.term_cmd)
	{
	  SERIAL_WRITE (monitor_desc, current_monitor->getreg.term_cmd,
			strlen (current_monitor->getreg.term_cmd));
	  expect_prompt (NULL, 0);
	}
    }
  else
    resp_len = expect_prompt (buf, sizeof buf); /* get response */


  /* If RESP_DELIM is specified, we search for that as a leading delimiter for
     the register value.  Otherwise, we just start searching from the start of
     the buf.  */

  if (current_monitor->getreg.resp_delim)
    {
      p = strstr (buf, current_monitor->getreg.resp_delim);
      if (!p)
	error ("monitor_fetch_register (%d):  bad response from monitor: %.*s.",
	       regno, resp_len, buf);
      p += strlen (current_monitor->getreg.resp_delim);
    }
  else
    p = buf;

  val = strtoul (p, &p1, 16);

  if (val == 0 && p == p1)
    error ("monitor_fetch_register (%d):  bad value from monitor: %.*s.",
	   regno, resp_len, buf);

  /* supply register stores in target byte order, so swap here */

  store_unsigned_integer (regbuf, REGISTER_RAW_SIZE (regno), val);

  supply_register (regno, regbuf);
}

/* Read the remote registers into the block regs.  */

void
monitor_fetch_registers (regno)
     int regno;
{
  if (regno >= 0)
    {
      monitor_fetch_register (regno);
      return;
    }

  for (regno = 0; regno < NUM_REGS; regno++)
    monitor_fetch_register (regno);
}

/* Store register REGNO, or all if REGNO == 0.  Return errno value.  */

static void
monitor_store_register (regno)
     int regno;
{
  char *name;
  unsigned LONGEST val;

  name = REGNAMES (regno);
  if (!name)
    return;

  val = read_register (regno);

 /* send the register deposit command */

  printf_monitor (current_monitor->setreg.cmd, name, val);

/* It's possible that there are actually some monitors out there that will
   prompt you when you set a register.  In that case, you may need to add some
   code here to deal with TERM and TERM_CMD (see monitor_fetch_register to get
   an idea of what's needed...) */

  expect_prompt (NULL, 0);
}

/* Store the remote registers.  */

void
monitor_store_registers (regno)
     int regno;
{
  if (regno >= 0)
    {
      monitor_store_register (regno);
      return;
    }

  for (regno = 0; regno < NUM_REGS; regno++)
    monitor_store_register (regno);
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
  printf_unfiltered ("\tAttached to %s at %d baud.\n", dev_name, baud_rate);
}

static int
monitor_write_memory (memaddr, myaddr, len)
     CORE_ADDR memaddr;
     unsigned char *myaddr;
     int len;
{
 /* send the memory deposit command */

  printf_monitor (current_monitor->setmem.cmd, memaddr, *myaddr);

/* It's possible that there are actually some monitors out there that will
   prompt you when you deposit to memory.  In that case, you may need to add
   some code here to deal with TERM and TERM_CMD (see monitor_read_memory to
   get an idea of what's needed...) */

  expect_prompt (NULL, 0);

  return 1;
}

/* Copy LEN bytes of data from debugger memory at MYADDR to inferior's memory
   at MEMADDR.  Returns length moved.  Currently, we only do one byte at a
   time.  */

static int
monitor_read_memory (memaddr, myaddr, len)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
{
  unsigned LONGEST val;
  unsigned char regbuf[MAX_REGISTER_RAW_SIZE];
  char buf[200];
  char *p, *p1;
  char *name;
  int resp_len;

 /* send the memory examine command */

  printf_monitor (current_monitor->getmem.cmd, memaddr);

/* If TERM is present, we wait for that to show up.  Also, (if TERM is
   present), we will send TERM_CMD if that is present.  In any case, we collect
   all of the output into buf, and then wait for the normal prompt.  */

  if (current_monitor->getmem.term)
    {
      resp_len = expect (current_monitor->getmem.term, buf, sizeof buf); /* get response */

      if (resp_len <= 0)
	error ("monitor_read_memory (0x%x):  excessive response from monitor: %.*s.",
	       memaddr, resp_len, buf);

      if (current_monitor->getmem.term_cmd)
	{
	  SERIAL_WRITE (monitor_desc, current_monitor->getmem.term_cmd,
			strlen (current_monitor->getmem.term_cmd));
	  expect_prompt (NULL, 0);
	}
    }
  else
    resp_len = expect_prompt (buf, sizeof buf); /* get response */

  /* If RESP_DELIM is specified, we search for that as a leading delimiter for
     the register value.  Otherwise, we just start searching from the start of
     the buf.  */

  if (current_monitor->getmem.resp_delim)
    {
      p = strstr (buf, current_monitor->getmem.resp_delim);
      if (!p)
	error ("monitor_read_memory (0x%x):  bad response from monitor: %.*s.",
	       memaddr, resp_len, buf);
      p += strlen (current_monitor->getmem.resp_delim);
    }
  else
    p = buf;

  val = strtoul (p, &p1, 16);

  if (val == 0 && p == p1)
    error ("monitor_read_memory (0x%x):  bad value from monitor: %.*s.", memaddr,
	   resp_len, buf);

  *myaddr = val;

  return 1;			/* Got 1 byte */
}

/* FIXME-someday!  merge these two.  */

int
monitor_xfer_memory (memaddr, myaddr, len, write, target)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
     int write;
     struct target_ops *target;		/* ignored */
{
  if (write)
    return monitor_write_memory (memaddr, myaddr, len);
  else
    return monitor_read_memory (memaddr, myaddr, len);
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
  unpush_target (targ_ops);
  generic_mourn_inferior ();	/* Do all the proper things now */
}

#define NUM_MONITOR_BREAKPOINTS 8

static CORE_ADDR breakaddr[NUM_MONITOR_BREAKPOINTS] = {0};

/* Tell the monitor to add a breakpoint.  */

int
monitor_insert_breakpoint (addr, shadow)
     CORE_ADDR addr;
     char *shadow;
{
  int i;
  static unsigned char break_insn[] = BREAKPOINT;

  for (i = 0; i < NUM_MONITOR_BREAKPOINTS; i++)
    {
      if (breakaddr[i] == 0)
	{
	  breakaddr[i] = addr;
	  monitor_read_memory (addr, shadow, sizeof (break_insn));
	  printf_monitor (SET_BREAK_CMD, addr);
	  expect_prompt (NULL, 0);
	  return 0;
	}
    }

  error ("Too many breakpoints (> %d) for monitor.", NUM_MONITOR_BREAKPOINTS);
}

/* Tell the monitor to remove a breakpoint.  */

int
monitor_remove_breakpoint (addr, shadow)
     CORE_ADDR addr;
     char *shadow;
{
  int i;

  for (i = 0; i < NUM_MONITOR_BREAKPOINTS; i++)
    {
      if (breakaddr[i] == addr)
	{
	  breakaddr[i] = 0;
	  /* some monitors remove breakpoints based on the address */
	  if (CLR_BREAK_ADDR)   
	    printf_monitor(CLR_BREAK_CMD, addr);
	  else
	    printf_monitor(CLR_BREAK_CMD, i);
	  expect_prompt (NULL, 0);
	  return 0;
	}
    }
  fprintf_unfiltered (stderr, "Can't find breakpoint associated with 0x%x\n", addr);
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
  /* default, load a binary */
  if (STREQ (loadtype_str, "default"))
    {
      error ("default load type not supported.");
    }

  /* load an srecord by converting */
  if ((STREQ (loadtype_str, "srec")) && STREQ (loadproto_str, "xmodem"))
    {
      monitor_load_srec (file, XMODEM);
      return;
    }

  /* load an srecord by converting */
  if (STREQ (loadtype_str, "srec"))
    {
      monitor_load_srec (file, 0);			/* if from a binary */
      return;
    }

  /* load an srecord by converting */
  if (STREQ (loadtype_str, "none"))
    {
      error ("Unimplemented");
      return;
    }

  /* load an srecord file */
  if (STREQ (loadproto_str, "none"))
    {
      monitor_load_ascii_srec (file, fromtty);		/* if from a binary */
      return;
    }

  if (STREQ (loadproto_str, "xmodem"))
    {
      monitor_load_srec (file, XMODEM);
      return;
    }
}

/* Download an ASCII srecord file.  */

#define DOWNLOAD_LINE_SIZE 100

static void
monitor_load_ascii_srec (file, fromtty)
    char *file;
    int fromtty;
{
  FILE *download;
  char buf[DOWNLOAD_LINE_SIZE];
  int i, bytes_read;

  download = fopen (file, "r");
  if (download == NULL)
    {
      error ("%s does not exist", file);
      return;
    }

  printf_monitor (LOAD_CMD);
  sleep (1);
  while (!feof (download))
    {
      bytes_read = fread (buf, sizeof (char), DOWNLOAD_LINE_SIZE, download);
      if (hashmark)
	{
	  putchar_unfiltered ('.');
	  gdb_flush (gdb_stdout);
	}
      if (SERIAL_WRITE (monitor_desc, buf, bytes_read))
	{
	  fprintf_unfiltered (stderr, "SERIAL_WRITE failed: (while downloading) %s\n",
		   safe_strerror (errno));
	  break;
	}
      i = 0;
      while (i++ <=200) {} ;	/* Ugly HACK, probably needs flow control */
      if (bytes_read < DOWNLOAD_LINE_SIZE)
	{
	  if (!feof (download))
	    error ("Only read %d bytes\n", bytes_read);
	  break;
	}
    }
  
  if (hashmark)
    putchar_unfiltered ('\n');

  if (!feof (download))
    error ("Never got EOF while downloading");
  expect_prompt (NULL, 0);
  fclose (download);
}

/* Put a command string, in args, out to MONITOR.  Output from MONITOR
   is placed on the users terminal until the prompt is seen. FIXME: We
   read the characters ourseleves here cause of a nasty echo.  */

static void
monitor_command (args, fromtty)
     char *args;
     int fromtty;
{
  char *p;

  p = PROMPT;

  if (monitor_desc == NULL)
    error ("monitor target not open.");

  /* Send the command.  Note that if no args were supplied, then we're
     just sending the monitor a newline, which is sometimes useful.  */

  printf_monitor ("%s\n", (args ? args : ""));

  expect_prompt (NULL, 0);
}

/*  Download a binary file by converting it to srecords. This
    will also use xmodem to download the resulting file.
  
  	A download goes like this when using xmodem:
 	Receiver:		Sender
  	NAK ---------->
  		<-------- (packet)	[SOH|1|1|data|SUM]
  	ACK ---------->
  		<-------- (packet)	[SOH|2|2|data|SUM]
  	ACK ---------->
  		<-------- EOT
  	ACK ---------->
  
  	ACK = 0x06
  	NAK = 0x15
  	EOT = 0x04
   */

static void
monitor_load_srec (args, protocol)
     char *args;
     int protocol;
{
  bfd *abfd;
  asection *s;
  char *buffer, srec[1024];
  char packet[XMODEM_PACKETSIZE];
  int i;
  int retries;
  int type = 0;			/* default to a type 0, header record */
  int srec_frame = 57;		/* FIXME: this must be 57 There is 12 bytes
				 of header, and 2 bytes of checksum at the end.
				 The problem is an xmodem packet holds exactly
				 128 bytes. */

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
  
  printf_monitor (LOAD_CMD);			/* tell the monitor to load */
  sleep (3);
  /* get the NAK from the target */
  if (protocol == XMODEM)
    {
      if (!GETNAK)
	{
	  printf_monitor ("%c", EOT);
	  error ("Never got the NAK to start loading");
	}
    }
  
  s = abfd->sections;
  while (s != (asection *) NULL)
    {
      if (s->flags & SEC_LOAD)
	{
	  buffer = xmalloc (srec_frame);

	  printf_filtered ("%s\t: 0x%4x .. 0x%4x  ",
			   s->name, s->vma, s->vma + s->_raw_size);
	  gdb_flush (gdb_stdout);
	  for (i = 0; i < s->_raw_size; i += srec_frame)
	    {
	      if (srec_frame > s->_raw_size - i)
		srec_frame = s->_raw_size - i;
	
	      bfd_get_section_contents (abfd, s, buffer, i, srec_frame);
	      monitor_make_srec (srec, type, s->vma + i, buffer, srec_frame);
	      /* send a packet using xmodem */
	      if (protocol == XMODEM)
		{
		  make_xmodem_packet (packet, srec, XMODEM_DATASIZE);
		  write_monitor (packet, XMODEM_PACKETSIZE+1);
		  retries = 0;
		  while (retries++ <= 3)
		    {
		      /* Resend packet */
		      if (GETNAK)
			{
			  sleep (1);
			  /* send it again */
			  write_monitor (packet, XMODEM_PACKETSIZE+1);
			  if (GETACK)	/* ACKnowledged, get next data chunk */
			    break;
			}
		      else
			{			/* assume we got an ACK */
			  if (hashmark)
			    {
			      putchar_unfiltered ('#');
			      gdb_flush (gdb_stdout);
			    }
			  break;
			}
		    }
		  if (retries >= 4)
		    {			/* too many tries, must be hosed */
		      printf_monitor ("%c", EOT);
		      error ("Never got a ACK after sending an xmodem packet");
		    }
		}
	      else
		{				/* no protocols at all */
		  printf_monitor ("%s\n", srec);
		}
	      if (hashmark)
		{
		  putchar_unfiltered ('#');
		  gdb_flush (gdb_stdout);
		}
	      type = 3;				/* switch to a 4 byte address record */
	      gdb_flush (gdb_stdout);
	    }
	  free (buffer);
	}
      s = s->next;
    }
  putchar_unfiltered ('\n');
  
  /* Write a type 7 terminator record. no data for a type 7, and there
     is no data, so len is 0.  */

  if (protocol == XMODEM)
    {
      /* send a packet using xmodem */
      monitor_make_srec (srec, 7, abfd->start_address, "", 0);
      make_xmodem_packet (packet, srec, XMODEM_DATASIZE);  
      write_monitor (packet, XMODEM_PACKETSIZE+1);
    }
  else
    {
      monitor_make_srec (srec, 7, abfd->start_address, "", 0);
      printf_monitor ("%s\n", srec);
    }
  if (protocol == XMODEM)
    {
      printf_monitor ("%c", EOT);
      if (!GETACK)
	error ("Never got ACK after sending EOT");
    }

  if (hashmark) 
    putchar_unfiltered ('\n');
  
  expect_prompt (NULL, 0);

/* Finally, make the PC point at the start address */

  write_register (PC_REGNUM, bfd_get_start_address (abfd));
}

/* Get an ACK or a NAK from the target.  returns 1 (true) or 0 (false)
   This is for xmodem. ANy string starting with "***" is an error
   message from the target.  Here's a few from the WinBond w89k
   "Cougar" PA board:
   *** Too many errors found.
   *** Bad command
   *** Command syntax error
   */

static int
getacknak (byte)
     int byte;
{
  char character;
  int i;
  
  i = 0;
  while (i++ < 60)
    {
      character = (char) readchar (0);
      if ((character == 0xfffffffe) || (character == 0x7f))
	{		/* empty uart */
	  sleep (1);
	  continue;
	}
      if (character == CANCEL)
	{			/* target aborted load */
	  expect_prompt (NULL, 0);
	  error ("Got a CANCEL from the target.");
	}
    if (character == '*')
      {			/* look for missed error message */
	expect_prompt (NULL, 0);
	error ("Got an error message from the target");
      }
      if (character == byte)			/* got what we wanted */
	return 1;
      if (character == ((byte == ACK) ? NAK : ACK))
	return 0;
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

static int
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

  checksum = 0;
  
  /* Create the header for the srec. 4 is the number of bytes in the address,
     and 1 is the number of bytes in the count.  */

  if (type == 0)				/* FIXME: type 0 is optional */
    type = 3;					/* so use data as it works */
  sprintf (buf, "S%d%02X%08X", type, len + 4 + 1, memaddr);
  buf += 12;
  
  checksum += (len + 4 + 1			/* calculate the checksum */
	       + (memaddr & 0xff)
	       + ((memaddr >>  8) & 0xff)
	       + ((memaddr >> 16) & 0xff)
	       + ((memaddr >> 24) & 0xff));
  
  /* build the srecord */
  for (i = 0; i < len; i++)
    {
      sprintf (buf, "%02X", myaddr[i]);
      checksum += myaddr[i];
      buf += 2;
    }

  sprintf(buf, "%02X", ~checksum & 0xff);	/* add the checksum */
  
  return 0;
}

/* Take 128 bytes of data and make a packet out of it.
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

static void
make_xmodem_packet (packet, data, len)
     unsigned char *packet;
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
  packet[2] = ~sequence++;	/* the complement is the sequence checksum */
#endif
  
  sum = 0;				/* calculate the data checksum */
  for (i = 3; i <= len + 2; i++) {
    packet[i] = *buf;
    sum += *buf;
    buf++;
  }

  /* add padding for the rest of the packet */
  for (i = len+1 ; i <= XMODEM_DATASIZE ; i++)
    packet[i] = '0';

  packet[XMODEM_PACKETSIZE] = sum & 0xff;	/* add the checksum */
}

/* Print the packet as a debug check.  */

static void
print_xmodem_packet (packet)
     char *packet;
{
  int i;
  static int lastseq;
  int sum;

  /* take apart the packet header the packet header */
  if (packet[0] == SOH)
    printf_unfiltered ("SOH");
  else
    error ("xmodem: SOH is wrong");
  
  /* check the sequence */
  if (packet[1] != 0)
    {
      lastseq = packet[1];
      if (packet[2] != ~lastseq)
	error ("xmodem: Sequence checksum is wrong");
      else
	printf_filtered (" %d %d", lastseq, ~lastseq);
    }
  
  /* check the data checksum */
  sum = 0;
  for (i = 3; i <= XMODEM_DATASIZE; i++)
    sum += packet[i];

  /* ignore the data */
#if 0
  printf_unfiltered (" [128 bytes of data] %d\n", sum & 0xff);
#endif
  printf_filtered (" [%s] %d\n", packet, sum & 0xff);

  if ((packet[XMODEM_PACKETSIZE] & 0xff) != (sum & 0xff))
    printf_unfiltered ("xmodem: data checksum wrong, got a %d",
		       packet[XMODEM_PACKETSIZE] & 0xff);

  putchar_unfiltered ('\n');
}

/* Convert hex digit A to a number.  */

static int
from_hex (a)
     int a;
{  
  if (a >= '0' && a <= '9')
    return a - '0';
  if (a >= 'a' && a <= 'f')
    return a - 'a' + 10;
  if (a >= 'A' && a <= 'F')
    return a - 'A' + 10;

  error ("Reply contains invalid hex digit 0x%x", a);
}

/* Define additional commands that are usually only used by monitors.  */

void
_initialize_remote_monitors ()
{
  add_show_from_set (add_set_cmd ("hash", no_class, var_boolean,
                                  (char *)&hashmark,
				  "Set display of activity while downloading a file.\n\
When enabled, a hashmark \'#\' is displayed.",
                                  &setlist),
		     &showlist);

  add_com ("monitor", class_obscure, monitor_command,
	   "Send a command to the debug monitor."); 
}
