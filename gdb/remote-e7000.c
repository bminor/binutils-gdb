/* Remote debugging interface for Hitachi E7000 ICE, for GDB.
   Copyright 1993 Free Software Foundation, Inc.
   Contributed by Cygnus Support.  Written by Steve Chamberlain for Cygnus.

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
#include "gdbcore.h"
#include "target.h"
#include "wait.h"
#include <varargs.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include "serial.h"


/* The E7000 is an in-circuit emulator for the Hitachi H8/300-H and
Hitachi-SH processor.  It has serial port and a lan port.  

The monitor command set makes it difficult to load large ammounts of
data over the lan without using ftp - so try not to issue load
commands when communicating over ethernet; use the ftpload command.

The monitor pauses for a second when dumping srecords to the serial
line too, so we use a slower per byte mechanism but without the
startup overhead.  Even so, it's pretty slow... */

int using_tcp; /* nonzero if using the tcp serial driver */

extern struct target_ops e7000_ops;	/* Forward declaration */
#define CTRLC 0x03
#define ENQ  0x05
#define ACK  0x06
#define CTRLZ 0x1a

char *ENQSTRING = "\005";

int echo;
static int echo_index;
static void e7000_close ();
static void e7000_fetch_register ();
static void e7000_store_register ();

static int timeout = 5;

static void expect PARAMS ((char *));
static void expect_full_prompt PARAMS (());
static void expect_prompt PARAMS (());
static serial_t e7000_desc;


/* Send data to e7000debug.  Works just like printf. */

static void
printf_e7000debug (va_alist)
     va_dcl
{
  va_list args;
  char *pattern;
  char buf[200];

  va_start (args);

  pattern = va_arg (args, char *);

  vsprintf (buf, pattern, args);
  if (SERIAL_WRITE (e7000_desc, buf, strlen (buf)))
    fprintf (stderr, "SERIAL_WRITE failed: %s\n", safe_strerror (errno));

  /* And expect to see it echoed */
  expect (buf);
}

static void
putchar_e7000 (x)
{
  char b[1];
  b[0] = x;
  SERIAL_WRITE (e7000_desc, b, 1);
}

static void
write_e7000 (s)
     char *s;
{
  SERIAL_WRITE (e7000_desc, s, strlen (s));
}

/* Read a character from the remote system, doing all the fancy timeout
   stuff.  */

static int
readchar (timeout)
     int timeout;
{
  int c;
  do
    {
      c = SERIAL_READCHAR (e7000_desc, timeout);
    }
  while (c > 127);
  if (c == SERIAL_TIMEOUT)
    {
      if (timeout == 0)
	return c;		/* Polls shouldn't generate timeout errors */

      error ("Timeout reading from remote system.");
    }
  return c;
}

/* Scan input from the remote system, until STRING is found.  If DISCARD is
   non-zero, then discard non-matching input, else print it out.
   Let the user break out immediately.  */
static void
expect (string)
     char *string;
{
  char *p = string;
  int c;

  immediate_quit = 1;
  while (1)

    {
      c = readchar (timeout);
      if (c == SERIAL_ERROR)
	{
	  error ("Serial communication error");
	}
      if (echo_index)
	{
	  if (c != '\r')
	    putchar (c);
	  fflush (stdout);
	}
      if (c == *p++)
	{
	  if (*p == '\0')
	    {
	      immediate_quit = 0;
	      return;
	    }
	}
      else
	{
	  p = string;
	}
    }
}

/* Keep discarding input until we see the e7000 prompt.

   The convention for dealing with the prompt is that you
   o give your command
   o *then* wait for the prompt.

   Thus the last thing that a procedure does with the serial line
   will be an expect_prompt().  Exception:  e7000_resume does not
   wait for the prompt, because the terminal is being handed over
   to the inferior.  However, the next thing which happens after that
   is a e7000_wait which does wait for the prompt.
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
  expect (":");
}
static void
expect_full_prompt ()
{
#if defined (LOG_FILE)
  /* This is a convenient place to do this.  The idea is to do it often
     enough that we never lose much data if we terminate abnormally.  */
  fflush (log_file);
#endif
  expect ("\n:");
}

static int
get_hex_digit (ch)
{
  if (ch >= '0' && ch <= '9')
    return ch - '0';
  else if (ch >= 'A' && ch <= 'F')
    return ch - 'A' + 10;
  else if (ch >= 'a' && ch <= 'f')
    return ch - 'a' + 10;
  return -1;

}



static int
get_hex (start)
     int *start;
{
  int value = get_hex_digit (*start);
  int try;

  *start = readchar (timeout);
  while ((try = get_hex_digit (*start)) >= 0)
    {
      value <<= 4;
      value += try;
      *start = readchar (timeout);
    }
  return value;
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
e7000_create_inferior (execfile, args, env)
     char *execfile;
     char *args;
     char **env;
{
  int entry_pt;

  if (args && *args)
    error ("Can't pass arguments to remote E7000DEBUG process");

  if (execfile == 0 || exec_bfd == 0)
    error ("No exec file specified");

  entry_pt = (int) bfd_get_start_address (exec_bfd);

#ifdef CREATE_INFERIOR_HOOK
  CREATE_INFERIOR_HOOK (0);	/* No process-ID */
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
  proceed ((CORE_ADDR) entry_pt, -1, 0);	/* Let 'er rip... */
}

/* Open a connection to a remote debugger.
   NAME is the filename used for communication.  */

static int baudrate = 9600;
static char dev_name[100];

static char *machine = "";
static char *user = "";
static char *passwd = "";
static char *dir = "";

/* Grab the next token and buy some space for it */
static char *
next (ptr)
     char **ptr;
{
  char *p = *ptr;
  char *s;
  char *r;
  int l = 0;
  while (*p && *p == ' ')
    {
      p++;
    }
  s = p;
  while (*p && (*p != ' ' && *p != '\t'))
    {
      l++;
      p++;
    }
  r = xmalloc (l + 1);
  memcpy (r, s, l);
  r[l] = 0;
  *ptr = p;
  return r;
}

static
e7000_login (args, from_tty)
     char *args;
     int from_tty;
{
  if (args)
    {
      machine = next (&args);
      user = next (&args);
      passwd = next (&args);
      dir = next (&args);
      if (from_tty)
	{
	  printf ("Set info to %s %s %s %s\n", machine, user, passwd, dir);
	}
    }
  else
    {
      error ("Syntax is ftplogin <machine> <user> <passwd> <directory>");
    }
}

/* Start an ftp transfer from the E7000 to a host */

static
e7000_ftp (args, from_tty)
     char *args;
     int from_tty;
{
  int oldtimeout = timeout;
  timeout = 10;
  echo_index++;
  printf_e7000debug ("ftp %s\r", machine);
  expect (" Username : ");
  printf_e7000debug ("%s\r", user);
  expect (" Password : ");
  write_e7000 (passwd);
  write_e7000 ("\r");
  expect ("success\r");
  expect ("FTP>");
  printf_e7000debug ("cd %s\r", dir);
  expect ("FTP>");
  printf_e7000debug ("ll 0;s:%s\r", args);
  expect ("FTP>");
  printf_e7000debug ("bye\r");
  expect (":");
  echo_index--;
  timeout = oldtimeout;
}

static void
e7000_open (args, from_tty)
     char *args;
     int from_tty;
{
  int n;
  char junk[100];
  int sync;
  target_preopen (from_tty);

  if (args)
    n = sscanf (args, " %s %d %s", dev_name, &baudrate, junk);
  else
    n = 0;
  if (n != 1 && n != 2)
    error ("Bad arguments.  Usage:\ttarget e7000 <device> <speed>\n\
or \t\ttarget e7000 <host>[:<port>]\n");

  if (n == 1 && strchr (dev_name, ':') == 0)
    {
      /* Default to normal telnet port */
      strcat (dev_name, ":23");
    }

  push_target (&e7000_ops);
  e7000_desc = SERIAL_OPEN (dev_name);


  if (!e7000_desc)
    perror_with_name (dev_name);

  using_tcp = strcmp (e7000_desc->ops->name, "tcp") == 0;

  SERIAL_SETBAUDRATE (e7000_desc, baudrate);
  SERIAL_RAW (e7000_desc);

  /* Hello?  Are you there?  */
  sync = 0;
  while (!sync)
    {
      int c;
      if (from_tty)
	printf_unfiltered ("[waiting for e7000...]\n");
      write_e7000 ("\r\n");
      c = SERIAL_READCHAR (e7000_desc, 3);
      while (c != SERIAL_TIMEOUT)
	{
	  /* Dont echo cr's */
	  if (from_tty && c != '\r')
	    {
	      putchar (c);
	      fflush (stdout);
	    }
	  if (c == ':')
	    {
	      sync = 1;
	    }
	  c = SERIAL_READCHAR (e7000_desc, 3);
	  if (quit_flag)
	    {
	      putchar_e7000 (CTRLC);
	      quit_flag = 0;
	    }
	}
    }
  printf_e7000debug ("\r\n");
  expect_prompt ();

  if (from_tty)
    printf_filtered ("Remote %s connected to %s\n", target_shortname,
		     dev_name);

}

/* Close out all files and local state before this target loses control. */

static void
e7000_close (quitting)
     int quitting;
{
  if (e7000_desc)
    {
      SERIAL_CLOSE (e7000_desc);
      e7000_desc = 0;
    }
}

/* Terminate the open connection to the remote debugger.
   Use this when you want to detach and do something else
   with your gdb.  */
static void
e7000_detach (from_tty)
     int from_tty;
{
  pop_target ();		/* calls e7000_close to do the real work */
  if (from_tty)
    printf ("Ending remote %s debugging\n", target_shortname);
}

/* Tell the remote machine to resume.  */

static void
e7000_resume (pid, step, sig)
     int pid, step, sig;
{
  if (step)
    {
      printf_e7000debug ("S\r");
    }
  else
    {
      printf_e7000debug ("G\r");
    }
}

/* Read the remote registers into the block REGS.  

   A reg dump looks like:

 */

#ifdef GDB_TARGET_IS_H8300
char *want = "\n\
 PC=%p CCR=%c\n\
 ER0 - ER3  %0 %1 %2 %3\n\
 ER4 - ER7  %4 %5 %6 %7\n\
:";

#endif
#ifdef GDB_TARGET_IS_SH
char *want = "\n\PC=%16 SR=%22\n\
 PR=%17 GBR=%18 VBR=%19\n\
 MACH=%20 MACL=%21\n\
 R0-7  %0 %1 %2 %3 %4 %5 %6 %7\n\
 R8-15 %8 %9 %10 %11 %12 %13 %14 %15\n\
:";

char *want_nopc = "%16 SR=%22\n\
 PR=%17 GBR=%18 VBR=%19\n\
 MACH=%20 MACL=%21\n\
 R0-7  %0 %1 %2 %3 %4 %5 %6 %7\n\
 R8-15 %8 %9 %10 %11 %12 %13 %14 %15";


#endif

static
int
gch ()
{
  int c = readchar (timeout);
  if (echo)
    {
      if (c >= ' ')
	printf ("%c", c);
      else if (c == '\n')
	printf ("\n", c);
    }
  return c;
}


static
unsigned int
gbyte ()
{
  int high = get_hex_digit (gch ());
  int low = get_hex_digit (gch ());
  return (high << 4) + low;
}

void
fetch_regs_from_dump (nextchar, want)
     int (*nextchar)();
     char *want;
{
  int regno;
  char buf[MAX_REGISTER_RAW_SIZE];

  int  thischar = nextchar();
  
  while (*want)
    {
      switch (*want)
	{
	case '\n':
	  while (thischar != '\n')
	    thischar = nextchar();
	  thischar = nextchar();
	  while (thischar == '\r')
	    thischar = nextchar();
	  want++;
	  break;

	case ' ':
	  while (thischar == ' ' || thischar == '\t')
	    thischar = nextchar();
	  want++;
	  break;
	  
	default:
	  if (*want == thischar)
	    {
	      want++;
	      if (*want)
		thischar = nextchar();
	      
	    }
	  else if (thischar == ' ')
	    {
	      thischar = nextchar();
	    }
	  else {
	    error("out of sync in fetch registers");
	  }
    
	  break;
	case '%':
	  /* Got a register command */
	  want++;
	  switch (*want)
	    {
#ifdef PC_REGNUM
	    case 'p':
	      regno = PC_REGNUM;
	      want++;
	      break;
#endif
#ifdef CCR_REGNUM
	    case 'c':
	      regno = CCR_REGNUM;
	      want++;
	      break;
#endif
#ifdef SP_REGNUM
	    case 's':
	      regno = SP_REGNUM;
	      want++;
	      break;
#endif
#ifdef FP_REGNUM
	    case 'f':
	      regno = FP_REGNUM;
	      want++;
	      break;
#endif


	    default:
	      if (isdigit(want[0])) 
		{
		  if (isdigit(want[1]))
		    {
		      regno = (want[0] - '0') * 10 + want[1] - '0';
		      want+=2;
		    }
		  else 
		    {
		      regno = want[0] - '0';
		      want++;
		    }
		}
	      
	      else
		abort();
	    }
	  store_signed_integer (buf,
				REGISTER_RAW_SIZE(regno),
				(LONGEST)get_hex(&thischar, nextchar));
	  supply_register (regno, buf);
	  break;
	}
    }
}

static void
e7000_fetch_registers ()
{
  int regno;

  printf_e7000debug ("R\r");
  fetch_regs_from_dump (gch, want);

  /* And supply the extra ones the simulator uses */
  for (regno = NUM_REALREGS; regno < NUM_REGS; regno++)
    {
      int buf = 0;
      supply_register (regno, (char *) (&buf));
    }
}

/* Fetch register REGNO, or all registers if REGNO is -1.
   Returns errno value.  */

static
void
e7000_fetch_register (regno)
     int regno;
{
  e7000_fetch_registers ();
}

/* Store the remote registers from the contents of the block REGS.  */

static void
e7000_store_registers ()
{
  int regno;

  for (regno = 0; regno < NUM_REALREGS; regno++)
    e7000_store_register (regno);

  registers_changed ();
}

/* Store register REGNO, or all if REGNO == 0.
   Return errno value.  */
static void
e7000_store_register (regno)
     int regno;
{
  if (regno == -1)
    {
      e7000_store_registers ();
      return;
    }
#ifdef GDB_TARGET_IS_H8300
  if (regno <= 7)
    {
      printf_e7000debug (".ER%d %x\r", regno,
			 read_register (regno));

    }
  else if (regno == PC_REGNUM)
    {
      printf_e7000debug (".PC %x\r",
			 read_register (regno));
    }
  else if (regno == CCR_REGNUM)
    {
      printf_e7000debug (".CCR %x\r",
			 read_register (regno));
    }
#endif

#ifdef  GDB_TARGET_IS_SH
  switch (regno)
    {
    default:
      printf_e7000debug (".R%d %x\r", regno,
			 read_register (regno));

      break;
    case PC_REGNUM:
      printf_e7000debug (".PC %x\r",
			 read_register (regno));
      break;
    case SR_REGNUM:
      printf_e7000debug (".SR %x\r",
			 read_register (regno));
      break;

    case PR_REGNUM:
      printf_e7000debug (".PR %x\r",
			 read_register (regno));
      break;

    case GBR_REGNUM:
      printf_e7000debug (".GBR %x\r",
			 read_register (regno));
      break;

    case VBR_REGNUM:
      printf_e7000debug (".VBR %x\r",
			 read_register (regno));
      break;

    case MACH_REGNUM:
      printf_e7000debug (".MACH %x\r",
			 read_register (regno));
      break;

    case MACL_REGNUM:
      printf_e7000debug (".MACL %x\r",
			 read_register (regno));
      break;
    }

#endif
  expect_prompt ();
}

/* Get ready to modify the registers array.  On machines which store
   individual registers, this doesn't need to do anything.  On machines
   which store all the registers in one fell swoop, this makes sure
   that registers contains all the registers from the program being
   debugged.  */

static void
e7000_prepare_to_store ()
{
  /* Do nothing, since we can store individual regs */
}

static void
e7000_files_info ()
{
  printf ("\tAttached to %s at %d baud.\n",
	  dev_name, baudrate);
}

static
int
stickbyte (where, what)
     char *where;
     unsigned int what;
{
  static CONST char digs[] = "0123456789ABCDEF";
  where[0] = digs[(what >> 4) & 0xf];
  where[1] = digs[(what & 0xf) & 0xf];
  return what;
}

/* Write a small ammount of memory */
static int
write_small (memaddr, myaddr, len)
     CORE_ADDR memaddr;
     unsigned char *myaddr;
     int len;
{
  int i;
  for (i = 0; i < len; i++)
    {
      if (((memaddr + i) & 3) == 0
	  && (i + 3 < len))
	{
	  /* Can be done with a long word */
	  printf_e7000debug ("m %x %x%02x%02x%02x;l\r",
			     memaddr + i,
			     myaddr[i],
			     myaddr[i + 1],
			     myaddr[i + 2],
			     myaddr[i + 3]);
	  i += 3;
	}
      else
	{
	  printf_e7000debug ("m %x %x\r", memaddr + i, myaddr[i]);
	}
    }
  expect_prompt ();
  return len;
}
/* Write a large ammount of memory, this only works with the serial mode enabled.
   Command is sent as
	il ;s:s\r	 ->
			<- il ;s:s\r
			<-  	ENQ
	ACK		->
			<- LO s\r
	Srecords...
	^Z		->
			<-	ENQ
	ACK		->  
			<-	:   	
 */

static int
write_large (memaddr, myaddr, len)
     CORE_ADDR memaddr;
     unsigned char *myaddr;
     int len;
{
  int i;
#define maxstride  128
  int stride;

  printf_e7000debug ("il ;s:s\r");
  expect (ENQSTRING);
  putchar_e7000 (ACK);
  expect ("LO s\r");

  for (i = 0; i < len; i += stride)
    {
      char compose[maxstride * 2 + 50];
      int address = i + memaddr;
      int j;
      int check_sum;
      int where = 0;
      int alen;
      stride = len - i;
      if (stride > maxstride)
	stride = maxstride;

      compose[where++] = 'S';
      check_sum = 0;
      if (address >= 0xffffff)
	{
	  alen = 4;
	}
      else if (address >= 0xffff)
	{
	  alen = 3;
	}
      else
	alen = 2;
      compose[where++] = alen - 1 + '0';	/* insert type */
      check_sum += stickbyte (compose + where, alen + stride + 1);	/* Insert length */
      where += 2;
      while (alen > 0)
	{
	  alen--;
	  check_sum += stickbyte (compose + where, address >> (8 * (alen)));
	  where += 2;
	}

      for (j = 0; j < stride; j++)
	{
	  check_sum += stickbyte (compose + where, myaddr[i + j]);
	  where += 2;
	}

      stickbyte (compose + where, ~check_sum);

      where += 2;
      compose[where++] = '\r';
      SERIAL_WRITE (e7000_desc, compose, where);
      j = SERIAL_READCHAR (e7000_desc, 0);
      if (j == SERIAL_TIMEOUT)
	{
	  /* This is ok - nothing there */
	}
      else if (j == ENQ)
	{
	  /* Hmm, it's trying to tell us something */
	  expect (":");
	  error ("Error writing memory");
	}
      else
	{
	  printf ("Whats this %d\n", j);
	}

    }
  /* Send the trailer record */
  write_e7000 ("S70500000000FA\r");
  putchar_e7000 (CTRLZ);
  expect (ENQSTRING);
  putchar_e7000 (ACK);
  expect (":");
  return len;
}

/* Copy LEN bytes of data from debugger memory at MYADDR
   to inferior's memory at MEMADDR.  Returns length moved.  

   Can't use the Srecord load over ethernet, so dont use 
   fast method then.
 */
static int
e7000_write_inferior_memory (memaddr, myaddr, len)
     CORE_ADDR memaddr;
     unsigned char *myaddr;
     int len;
{
  if (len < 16 || using_tcp)
    {
      return write_small (memaddr, myaddr, len);
    }
  else
    {
      return write_large (memaddr, myaddr, len);
    }
}

/* Read LEN bytes from inferior memory at MEMADDR.  Put the result
   at debugger address MYADDR.  Returns length moved. 

   Done by requesting an srecord dump from the E7000.
 */



/* Read LEN bytes from inferior memory at MEMADDR.  Put the result
   at debugger address MYADDR.  Returns length moved. 


  Small transactions we send
  m <addr>;l
  and receive
    00000000 12345678 ?

 */

static int
e7000_read_inferior_memory (memaddr, myaddr, len)
     CORE_ADDR memaddr;
     unsigned char *myaddr;
     int len;
{
  int count;
  int c;
  int i;
  /* Starting address of this pass.  */

  if (((memaddr - 1) + len) < memaddr)
    {
      errno = EIO;
      return 0;
    }

  printf_e7000debug ("m %x;l\r", memaddr);

  for (count = 0; count < len; count += 4) 
    {
      /* Suck away the address */
      c = gch();	
      while (c != ' ')
	c = gch();	
      c = gch();
      if (c == '*') 
	{			/* Some kind of error */
	  expect_prompt();
	  return -1;
	}
      while (c != ' ')
	c = gch();	

      /* Now read in the data */
      for (i = 0; i < 4; i++) 
	{
	  int b = gbyte();
	  if (count + i < len) {
	    myaddr[count + i] = b;
	  }
	}

      /* Skip the trailing ? and send a . to end and a cr for more */
      gch();	
      gch();
      if (count + 4 >= len)
	printf_e7000debug(".\r");
      else
	printf_e7000debug("\r");
    }
  expect_prompt();
}


#if 0
/*
  For large transfers we used to send


  d <addr> <endaddr>\r

  and receive
   <ADDR>              <    D   A   T   A    >               <   ASCII CODE   >
   000000  5F FD FD FF DF 7F DF FF  01 00 01 00 02 00 08 04  "_..............."
   000010  FF D7 FF 7F D7 F1 7F FF  00 05 00 00 08 00 40 00  "..............@."
   000020  7F FD FF F7 7F FF FF F7  00 00 00 00 00 00 00 00  "................"

  A cost in chars for each transaction of 80 + 5*n-bytes. 


  Large transactions could be done with the srecord load code, but
  there is a pause for a second before dumping starts, which slows the
  average rate down!
*/

static int
e7000_read_inferior_memory (memaddr, myaddr, len)
     CORE_ADDR memaddr;
     unsigned char *myaddr;
     int len;
{
  int count;
  int c;

  /* Starting address of this pass.  */

  if (((memaddr - 1) + len) < memaddr)
    {
      errno = EIO;
      return 0;
    }

  printf_e7000debug ("d %x %x\r", memaddr, memaddr + len - 1);

  count = 0;
  c = gch ();

  /* First skip the command */
  while (c == '\n')
    c = gch ();

  while (c == ' ')
    c = gch ();
  if (c == '*')
    {
      expect ("\r");
      return -1;
    }

  /* Skip the title line */
  while (c != '\n')
    c = gch ();
  c = gch ();
  while (count < len)
    {
      /* Skip the address */
      while (c <= ' ')
	c = gch ();

      get_hex (&c);

      /* read in the bytes on the line */
      while (c != '"' && count < len)
	{
	  if (c == ' ')
	    c = gch ();
	  else
	    {
	      myaddr[count++] = get_hex (&c);
	    }
	}

      while (c != '\n')
	c = gch ();
    }

  while (c != ':')
    c = gch ();

  return len;
}

static int
fast_but_for_the_pause_e7000_read_inferior_memory (memaddr, myaddr, len)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
{
  int loop;
  int c;

  if (((memaddr - 1) + len) < memaddr)
    {
      errno = EIO;
      return 0;
    }

  printf_e7000debug ("is %x@%x:s\r", memaddr, len);
  gch ();
  c = gch ();
  if (c != ENQ)
    {
      /* Got an error */
      error ("Memory read error");
    }
  putchar_e7000 (ACK);
  expect ("SV s");
  loop = 1;
  while (loop)
    {
      int type;
      int length;
      int addr;
      int i;
      c = gch ();
      switch (c)
	{
	case ENQ:		/* ENQ, at the end */
	  loop = 0;
	  break;
	case 'S':
	  /* Start of an Srecord */
	  type = gch ();
	  length = gbyte ();
	  switch (type)
	    {
	    case '7':		/* Termination record, ignore */
	    case '0':
	    case '8':
	    case '9':
	      /* Header record - ignore it */
	      while (length--)
		{
		  gbyte ();
		}
	      break;
	    case '1':
	    case '2':
	    case '3':
	      {
		int alen;
		alen = type - '0' + 1;
		addr = 0;
		while (alen--)
		  {
		    addr = (addr << 8) + gbyte ();
		    length--;
		  }

		for (i = 0; i < length - 1; i++)
		  {
		    myaddr[i + addr - memaddr] = gbyte ();
		  }
		gbyte ();	/* Ignore checksum */
	      }
	    }
	}
    }
  putchar_e7000 (ACK);
  expect ("TOP ADDRESS =");
  expect ("END ADDRESS =");
  expect (":");

  return len;
}

#endif

static int
e7000_xfer_inferior_memory (memaddr, myaddr, len, write, target)
     CORE_ADDR memaddr;
     unsigned char *myaddr;
     int len;
     int write;
     struct target_ops *target;	/* ignored */
{
  if (write)
    {
      return e7000_write_inferior_memory( memaddr, myaddr, len);
    }
  else
    {
      return e7000_read_inferior_memory( memaddr, myaddr, len);
    }
}

static void
e7000_kill (args, from_tty)
     char *args;
     int from_tty;
{

}

/* Clean up when a program exits.

   The program actually lives on in the remote processor's RAM, and may be
   run again without a download.  Don't leave it full of breakpoint
   instructions.  */

static void
e7000_mourn_inferior ()
{
  remove_breakpoints ();
  unpush_target (&e7000_ops);
  generic_mourn_inferior ();	/* Do all the proper things now */
}

#define MAX_E7000DEBUG_BREAKPOINTS 200

extern int memory_breakpoint_size;
static CORE_ADDR breakaddr[MAX_E7000DEBUG_BREAKPOINTS] =
{0};

static int
e7000_insert_breakpoint (addr, shadow)
     CORE_ADDR addr;
     unsigned char *shadow;
{
  int i;
  static char nop[2] =
    {0x20, 0x0b};

  for (i = 0; i <= MAX_E7000DEBUG_BREAKPOINTS; i++)
    if (breakaddr[i] == 0)
      {
	breakaddr[i] = addr;
	/* Save old contents, and insert a nop in the space */
	e7000_read_inferior_memory (addr, shadow, 2);
	e7000_write_inferior_memory (addr, nop, 2);
	printf_e7000debug ("B %x\r", addr);
	expect_prompt ();
	return 0;
      }

  error("Too many breakpoints ( > %d) for the E7000\n", MAX_E7000DEBUG_BREAKPOINTS);
  return 1;
}

static int
e7000_remove_breakpoint (addr, shadow)
     CORE_ADDR addr;
     unsigned char *shadow;
{
  int i;

  for (i = 0; i < MAX_E7000DEBUG_BREAKPOINTS; i++)
    if (breakaddr[i] == addr)
      {
	breakaddr[i] = 0;
	printf_e7000debug ("B - %x\r", addr);
	expect_prompt ();
	/* Replace the insn under the break */
	e7000_write_inferior_memory (addr, shadow, 2);
	return 0;
      }

  fprintf (stderr, "Can't find breakpoint associated with 0x%x\n", addr);
  return 1;
}


/* Put a command string, in args, out to STDBUG.  Output from STDBUG is placed
   on the users terminal until the prompt is seen. */

static void
e7000_command (args, fromtty)
     char *args;
     int fromtty;
{

  if (!e7000_desc)
    error ("e7000 target not open.");
  if (!args)
    {
      printf_e7000debug ("\r");
    }
  else
    {
      printf_e7000debug ("%s\r", args);
    }
  echo_index++;
  expect_full_prompt ();
  echo_index--;
  printf_unfiltered ("\n");
}

static void
e7000_load (args, fromtty)
     char *args;
     int fromtty;
{
  load_target_image (args, fromtty);
}

static void
e7000_drain (args, fromtty)
     char *args;
     int fromtty;

{
  int c;

  while ((c = SERIAL_READCHAR (e7000_desc, 2) != SERIAL_TIMEOUT))
    {
      if (c > ' ' && c < 127)
	printf ("%c", c & 0xff);
      else
	printf ("<%x>", c & 0xff);
    }
}

e7000_noecho ()
{
  echo = !echo;
}

/* Wait until the remote machine stops, then return,
   storing status in STATUS just as `wait' would.  */

static int
e7000_wait (pid, status)
     int pid;
     WAITTYPE *status;
{
  int c;
  int regno;
  int loop = 1;
  int time = 0;
  WSETSTOP ((*status), 0);
  /* Then echo chars until PC= string seen */
  gch ();			/* Drop cr */
  gch ();			/* and space */
  while (loop)
    {
      c = SERIAL_READCHAR (e7000_desc, 1);
      if (c < 0)
	{
	  time++;
	  if (time == 5)
	    {
	      printf_unfiltered ("[waiting for e7000..]\n");
	      time = 0;
	    }
	}
      if (quit_flag)
	{
	  quit_flag = 0;
	  putchar_e7000 (CTRLC);		/* interrupt the running program */
	}
      if (c == 'P')
	{
	  c = SERIAL_READCHAR (e7000_desc, 1);
	  if (c == 'C')
	    {
	      c = SERIAL_READCHAR (e7000_desc, 1);
	      if (c == '=')
		{
		  /* Got break */
		  loop = 0;
		}
	      else
		{
		  printf ("PC");
		}
	    }
	  else
	    {
	      printf ("P");
	    }
	}
      else
	{
	  if (c > 0)
	    {
	      putchar (c);
	      fflush (stdout);
	    }
	}
    }
  fetch_regs_from_dump (gch, want_nopc);

  /* And supply the extra ones the simulator uses */
  for (regno = NUM_REALREGS; regno < NUM_REGS; regno++)
    {
      int buf = 0;
      supply_register (regno, (char *) &buf);
    }

  expect_full_prompt ();
  WSETSTOP ((*status), SIGTRAP);

  return 0;
}

/* Define the target subroutine names */

struct target_ops e7000_ops =
{
  "e7000",
  "Remote Hitachi e7000 target",
  "Use a remote Hitachi e7000 ICE connected by a serial line,\n\
or a network connection.\n\
Arguments are the name of the device for the serial line,\n\
the speed to connect at in bits per second.\n\
eg\n\
target e7000 /dev/ttya 9600\n\
target e7000 foobar",
  e7000_open,
  e7000_close,
  0,
  e7000_detach,
  e7000_resume,
  e7000_wait,
  e7000_fetch_register,
  e7000_store_register,
  e7000_prepare_to_store,
  e7000_xfer_inferior_memory,
  e7000_files_info,
  e7000_insert_breakpoint,
  e7000_remove_breakpoint,	/* Breakpoints */
  0,
  0,
  0,
  0,
  0,				/* Terminal handling */
  e7000_kill,
  e7000_load,			/* load */
  0,				/* lookup_symbol */
  e7000_create_inferior,
  e7000_mourn_inferior,
  0,				/* can_run */
  0,				/* notice_signals */
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
_initialize_remote_e7000 ()
{
  add_target (&e7000_ops);
  add_com ("e7000 <command>", class_obscure, e7000_command,
	   "Send a command to the e7000 monitor.");

  add_com ("ftplogin <machine> <name> <passwd> <dir>", class_obscure, e7000_login,
	   "Login to machine and change to directory.");

  add_com ("ftpload <file>", class_obscure, e7000_ftp,
	   "Fetch and load a file from previously described place.");

  add_com ("drain", class_obscure, e7000_drain,
	   "Drain pending e7000 text buffers.");
  add_com ("echo", class_obscure, e7000_noecho, "Toggle monitor echo.");

}
