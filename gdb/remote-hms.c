/* Remote debugging interface for HMS  Monitor Version 1.0

   Copyright 1992 Free Software Foundation, Inc.

   Contributed by Steve Chamberlain sac@cygnus.com


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



#include <stdio.h>
#include <string.h>
#include "defs.h"
#include "inferior.h"
#include "wait.h"
#include "value.h"
#include <ctype.h>
#include <fcntl.h>
#include <signal.h>
#include <setjmp.h>
#include <errno.h>
#include "terminal.h"
#include "target.h"
#include "gdbcore.h"

/* External data declarations */
extern int stop_soon_quietly;           /* for wait_for_inferior */

/* External function declarations */
extern struct value *call_function_by_hand();

/* Forward data declarations */
extern struct target_ops hms_ops;		/* Forward declaration */

/* Forward function declarations */
static void hms_fetch_registers ();
static int  hms_store_registers ();
static void hms_close ();
static int  hms_clear_breakpoints();

extern struct target_ops hms_ops;

#define DEBUG	
#ifdef DEBUG
# define DENTER(NAME)   (printf_filtered("Entering %s\n",NAME), fflush(stdout))
# define DEXIT(NAME)    (printf_filtered("Exiting  %s\n",NAME), fflush(stdout))
#else
# define DENTER(NAME)
# define DEXIT(NAME)
#endif



static int timeout = 5;
static char *dev_name  = "/dev/ttya";


static int quiet;

/* Descriptor for I/O to remote machine.  Initialize it to -1 so that
   hms_open knows that we don't have a file open when the program
   starts.  */
int hms_desc = -1;
#define OPEN(x) ((x) >= 0)


#define ON	1
#define OFF	0
static void
rawmode(desc, turnon)
int	desc;
int	turnon;
{
  TERMINAL sg;

  if (desc < 0)
    return;

  ioctl (desc, TIOCGETP, &sg);

  if (turnon) {
#ifdef HAVE_TERMIO
  	sg.c_lflag &= ~(ICANON);
#else
	sg.sg_flags |= RAW;
#endif
  } else {
#ifdef HAVE_TERMIO
  	sg.c_lflag |= ICANON;
#else
	sg.sg_flags &= ~(RAW);
#endif
  }
  ioctl (desc, TIOCSETP, &sg);
}

/* Suck up all the input from the hms */
slurp_input()
{
  char buf[8];

#ifdef HAVE_TERMIO
  /* termio does the timeout for us.  */
  while (read (hms_desc, buf, 8) > 0);
#else
  alarm (timeout);
  while (read (hms_desc, buf, 8) > 0);
  alarm (0);
#endif
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
  read (hms_desc, &buf, 1);
#else
  alarm (timeout);
  if (read (hms_desc, &buf, 1) < 0)
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

if (!quiet)
  printf("'%c'",buf);
  
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

/* Keep discarding input until we see the hms prompt.

   The convention for dealing with the prompt is that you
   o give your command
   o *then* wait for the prompt.

   Thus the last thing that a procedure does with the serial line
   will be an expect_prompt().  Exception:  hms_resume does not
   wait for the prompt, because the terminal is being handed over
   to the inferior.  However, the next thing which happens after that
   is a hms_wait which does wait for the prompt.
   Note that this includes abnormal exit, e.g. error().  This is
   necessary to prevent getting into states from which we can't
   recover.  */
static void
expect_prompt ()
{

  expect ("HMS>");
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

/* Get a byte from hms_desc and put it in *BYT.  Accept any number
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

/* Read a 32-bit hex word from the hms, preceded by a space  */
static long 
get_hex_word()
{
  long val;
  int j;
      
  val = 0;
  for (j = 0; j < 8; j++)
	val = (val << 4) + get_hex_digit (j == 0);
  return val;
}
/* Called when SIGALRM signal sent due to alarm() timeout.  */
#ifndef HAVE_TERMIO

#ifndef __STDC__
# ifndef volatile
#  define volatile /**/
# endif
#endif
volatile int n_alarms;

void
hms_timer ()
{
#if 0
  if (kiodebug)
    printf ("hms_timer called\n");
#endif
  n_alarms++;
}
#endif

/* malloc'd name of the program on the remote system.  */
static char *prog_name = NULL;

/* Number of SIGTRAPs we need to simulate.  That is, the next
   NEED_ARTIFICIAL_TRAP calls to hms_wait should just return
   SIGTRAP without actually waiting for anything.  */

static int need_artificial_trap = 0;

void
hms_kill(arg,from_tty)
char	*arg;
int	from_tty;
{
#if 0
	DENTER("hms_kill()");
	fprintf (hms_stream, "K");

	fprintf (hms_stream, "\r");
	expect_prompt ();
	DEXIT("hms_kill()");
#endif
}
/*
 * Download a file specified in 'args', to the hms. 
 */
static void
hms_load(args,fromtty)
char	*args;
int	fromtty;
{
  bfd	*abfd;
  asection *s;
  int	n;
  char	buffer[1024];
	
  DENTER("hms_load()");
  if (!OPEN(hms_desc))
  {
    printf_filtered("HMS not open. Use 'target' command to open HMS\n");
    return;
  }
  abfd = bfd_openr(args,"coff-h8300");
  if (!abfd) 
  {
    printf_filtered("Can't open a bfd on file\n");
    return;
  }

  if (bfd_check_format(abfd, bfd_object) ==0) 
  {
    printf_filtered("File is not an object file\n");
    return ;
  }

  s = abfd->sections;
  while (s != (asection *)NULL) 
  {
    if (s->flags & SEC_LOAD) 
    {
      char *buffer = xmalloc(s->_raw_size);
      bfd_get_section_contents(abfd, s, buffer, 0, s->_raw_size);

      hms_write_inferior_memory(s->vma, buffer, s->_raw_size);
      free(buffer);
    }
    s = s->next;
  }
  
  DEXIT("hms_load()");
}

/* This is called not only when we first attach, but also when the
   user types "run" after having attached.  */
void
hms_create_inferior (execfile, args, env)
     char *execfile;
     char *args;
     char **env;
{
  int entry_pt;

  DENTER("hms_create_inferior()");

  if (args && *args)
   error ("Can't pass arguments to remote hms process.");

  if (execfile == 0 || exec_bfd == 0)
   error ("No exec file specified");

  entry_pt = (int) bfd_get_start_address (exec_bfd);

  if (OPEN(hms_desc))
  {
    
    hms_kill(NULL,NULL);	 
    hms_clear_breakpoints();
    init_wait_for_inferior ();
    /* Clear the input because what the hms sends back is different
     * depending on whether it was running or not.
     */
    /*	slurp_input();	/* After this there should be a prompt */
    hms_write_cr("r");
	
    expect_prompt();
    printf_filtered("Do you want to download '%s' (y/n)? [y] : ",prog_name);
  {	
    char buffer[10];
    gets(buffer);
    if (*buffer != 'n') {
	hms_load(prog_name,0);
      }
  }


    insert_breakpoints ();	/* Needed to get correct instruction in cache */
    proceed(entry_pt, -1, 0);


  } else 
  {
    printf_filtered("Hms not open yet.\n");
  }
  DEXIT("hms_create_inferior()");
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

static int damn_b (rate)
     int rate;
{
  int i;

  for (i = 0; baudtab[i].rate != -1; i++)
    if (rate == baudtab[i].rate) return baudtab[i].damn_b;
  return B38400;	/* Random */
}


/* Open a connection to a remote debugger.
   NAME is the filename used for communication, then a space,
   then the baud rate.
 */

static char *
find_end_of_word(s)
char *s;
{
  while (*s && !isspace(*s))
   s++;
  return s;
}

static char *get_word(p)
char **p;
{
  char *s = *p;
  char *word ;
  char *copy;
  size_t len;

  while (isspace(*s))
   s++;

  word = s;

  len = 0;

  while (*s && !isspace(*s)) 
  {
    s++;
    len++;
  
  }
  copy = xmalloc(len+1);
  memcpy(copy, word, len);
  copy[len] = 0;
  *p = s;
  return copy;
}

static int baudrate = 9600;
static void
hms_open (name, from_tty)
     char *name;
     int from_tty;
{
  TERMINAL sg;
  unsigned int prl;
  char *p;
  
  DENTER("hms_open()");
  if(name == 0) 
  {
    name = "";
    
  }
  
  printf("Input string %s\n", name);
  
  prog_name = get_word(&name);
  
  hms_close (0);

  hms_desc = open (dev_name, O_RDWR);
  if (hms_desc < 0)
   perror_with_name (dev_name);
  ioctl (hms_desc, TIOCGETP, &sg);
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

  ioctl (hms_desc, TIOCSETP, &sg);


  push_target (&hms_ops);
  /* start_remote ();              /* Initialize gdb process mechanisms */


#ifndef HAVE_TERMIO
#ifndef NO_SIGINTERRUPT
  /* Cause SIGALRM's to make reads fail with EINTR instead of resuming
     the read.  */
  if (siginterrupt (SIGALRM, 1) != 0)
   perror ("hms_open: error in siginterrupt");
#endif

  /* Set up read timeout timer.  */
  if ((void (*)) signal (SIGALRM, hms_timer) == (void (*)) -1)
   perror ("hms_open: error in signal");
#endif


  /* Put this port into NORMAL mode, send the 'normal' character */
  hms_write("\001", 1);		/* Control A */
  hms_write("\r", 1);		/* Cr */
  expect_prompt ();
  
  /* Hello?  Are you there?  */
  write (hms_desc, "\r", 1);
 
  expect_prompt ();

  /* Clear any break points */
  hms_clear_breakpoints();


  printf_filtered("Remote debugging on an H8/300 HMS via %s %s.\n",dev_name,prog_name);

  DEXIT("hms_open()");
}

/* Close out all files and local state before this target loses control. */

static void
hms_close (quitting)
     int quitting;
{

  DENTER("hms_close()");

  /* Clear any break points */
  hms_clear_breakpoints();

  /* Put this port back into REMOTE mode */ 
  if (OPEN(hms_desc)) {

     sleep(1);		/* Let any output make it all the way back */
     write(hms_desc, "R\r", 2);
  }

  /* Due to a bug in Unix, fclose closes not only the stdio stream,
     but also the file descriptor.  So we don't actually close
     hms_desc.  */
  if (OPEN(hms_desc))
    close (hms_desc); 

  /* Do not try to close hms_desc again, later in the program.  */

  hms_desc = -1;

  DEXIT("hms_close()");
}

/* Attach to the target that is already loaded and possibly running */
static void
hms_attach (args, from_tty)
     char *args;
     int from_tty;
{

  DENTER("hms_attach()");
  if (from_tty)
      printf_filtered ("Attaching to remote program %s.\n", prog_name);

  /* push_target(&hms_ops);	/* This done in hms_open() */

  mark_breakpoints_out ();

  /* Send the hms a kill. It is ok if it is not already running */
#if 0
  fprintf(hms_stream, "K\r"); 
  expect_prompt();		/* Slurp the echo */
#endif
  /* We will get a task spawn event immediately.  */
  init_wait_for_inferior ();
  clear_proceed_status ();
  stop_soon_quietly = 1;
  wait_for_inferior ();
  stop_soon_quietly = 0;
  normal_stop ();
  DEXIT("hms_attach()");
}


/* Terminate the open connection to the remote debugger.
   Use this when you want to detach and do something else
   with your gdb.  */
void
hms_detach (args,from_tty)
     char *args;
     int from_tty;
{
  DENTER("hms_detach()");
  if (OPEN(hms_desc)) { /* Send it on its way (tell it to continue)  */
	hms_clear_breakpoints();
#if 0
  	fprintf(hms_stream,"G\r");
#endif
  }
 
  pop_target();		/* calls hms_close to do the real work */
  if (from_tty)
    printf_filtered ("Ending remote %s debugging\n", target_shortname);
  DEXIT("hms_detach()");
}
 
/* Tell the remote machine to resume.  */

void
hms_resume (step, sig)
     int step, sig;
{
  DENTER("hms_resume()");
  if (step)	
  {
    hms_write_cr("s");

    hms_write("\003",1);
    expect_prompt();
    /* Force the next hms_wait to return a trap.  Not doing anything
       about I/O from the target means that the user has to type
       "continue" to see any.  FIXME, this should be fixed.  */
    need_artificial_trap = 1;
  }
  else
  {
    hms_write_cr("g");
    expect("g\r");
  }
  DEXIT("hms_resume()");
}

/* Wait until the remote machine stops, then return,
   storing status in STATUS just as `wait' would.  */

int
hms_wait (status)
     WAITTYPE *status;
{
  /* Strings to look for.  '?' means match any single character.  
     Note that with the algorithm we use, the initial character
     of the string cannot recur in the string, or we will not
     find some cases of the string in the input.  */
  
  static char bpt[] = "At breakpoint:";
  /* It would be tempting to look for "\n[__exit + 0x8]\n"
     but that requires loading symbols with "yc i" and even if
     we did do that we don't know that the file has symbols.  */
  static char exitmsg[] = "HMS>";
  char *bp = bpt;
  char *ep = exitmsg;

  /* Large enough for either sizeof (bpt) or sizeof (exitmsg) chars.  */
  char swallowed[50];
  /* Current position in swallowed.  */
  char *swallowed_p = swallowed;

  int ch;
  int ch_handled;
  int old_timeout = timeout;
  int old_immediate_quit = immediate_quit;

  DENTER("hms_wait()");

  WSETEXIT ((*status), 0);

  if (need_artificial_trap != 0)
    {
      WSETSTOP ((*status), SIGTRAP);
      need_artificial_trap--;
      return 0;
    }

  timeout = 0;		/* Don't time out -- user program is running. */
  immediate_quit = 1;	/* Helps ability to QUIT */
  while (1) {
      QUIT;		/* Let user quit and leave process running */
      ch_handled = 0;
      ch = readchar ();
      if (ch == *bp) {
	  bp++;
	  if (*bp == '\0')
	    break;
	  ch_handled = 1;

	  *swallowed_p++ = ch;
      } else
	bp = bpt;
      if (ch == *ep || *ep == '?') {
	  ep++;
	  if (*ep == '\0')
	    break;

	  if (!ch_handled)
	    *swallowed_p++ = ch;
	  ch_handled = 1;
      }, sal.line);
d979 3
      if (!ch_handled) {
	  char *p;
	  /* Print out any characters which have been swallowed.  */
	  for (p = swallowed; p < swallowed_p; ++p)
	   putc (*p, stdout);
	  swallowed_p = swallowed;

	  
	  if ((ch != '\r' && ch != '\n') || swallowed_cr>10) 
	  {
	    putc (ch, stdout);
	    swallowed_cr = 10;
	  }
	  swallowed_cr ++;
	  
	}
    }
  if (*bp== '\0') 
  {
    WSETSTOP ((*status), SIGTRAP);
    expect_prompt();
  }
  else 
  {
    WSETEXIT ((*status), 0);
  }
  
  timeout = old_timeout;
  immediate_quit = old_immediate_quit;
	printf ("%s is at %s:%d.\n",
		local_hex_string(sal.pc), 
		sal.symtab->filename, sal.line);
  DEXIT("hms_wait()");
  return 0;
}

/* Return the name of register number REGNO
   in the form input and output by hms.

   Returns a pointer to a static buffer containing the answer.  */
static char *
get_reg_name (regno)
     int regno;
{
static  char *rn[NUM_REGS]= REGISTER_NAMES;
  

  return rn[regno];

}

/* Read the remote registers.  */
static int gethex(length, start, ok)
unsigned int length;
char *start;
int *ok;
{
  int result = 0;
  while (length--)   
  {
    result <<= 4 ;
    if (*start >='a' && *start <= 'f') 
    {
      result += *start - 'a' + 10;
    }
    else if (*start >='A' && *start <= 'F') 
    {
      result += *start - 'A' + 10;
    }
    else  if (*start >='0' && *start <= '9') 
    {
      result += *start - '0' ;
    }
    else *ok = 0;
    start++;
    
  }
  return result;
}
static int 
timed_read(buf, n, timeout)
char *buf;

{
  int i;
  char c;
  i = 0;
  while (i < n) 
  {
    c = readchar();
    
    if (c == 0) return i;
    buf[i] = c;
    i++;
    
  }
  return i;  

}
hms_write(a,l)
char *a;
{
  int i;
  write(hms_desc,a,l);
if (!quiet)
  for (i = 0; i < l ; i++)
  {
    printf("%c", a[i]);
  }
}

	    printf ("Line %d of \"%s\" is at pc %s but contains no code.\n",
		    sal.line, sal.symtab->filename, local_hex_string(start_pc));
{
hms_write( s, strlen(s));
	      printf ("Line %d of \"%s\" starts at pc %s",
		      sal.line, sal.symtab->filename, 
		      local_hex_string(start_pc));
	      printf (" and ends at %s.\n",
		      local_hex_string(end_pc));
{
#define REGREPLY_SIZE 79
  char linebuf[REGREPLY_SIZE+1];
  int i;
  int s ;
  int gottok;
  
	printf ("Line number %d is out of range for \"%s\".\n",
		sal.line, sal.symtab->filename);
  check_open();
  
  do 
  {
    
    hms_write_cr("r");
    s = timed_read(linebuf, REGREPLY_SIZE, 1);

  
    linebuf[REGREPLY_SIZE] = 0;
    gottok = 0;    
    if (linebuf[0] == 'r' &&
	linebuf[1] == '\r' &&
	linebuf[2] == '\n' &&
	linebuf[3] == 'P' &&
	linebuf[4] == 'C' &&
	linebuf[5] == '=' && 
	linebuf[75] == 'H' &&
	linebuf[76] == 'M' &&
	linebuf[77] == 'S') 
    {
      /*
	PC=XXXX CCR=XX:XXXXXXXX R0-R7= XXXX XXXX XXXX XXXX XXXX XXXX XXXX XXXX
	5436789012345678901234567890123456789012345678901234567890123456789012
	0      1         2         3         4         5         6         
	*/
      gottok = 1;
      
      reg[PC_REGNUM] =  gethex(4,linebuf+6, &gottok);
      reg[CCR_REGNUM] = gethex(2,linebuf+15, &gottok);
      for (i = 0; i < 8; i++) 
      {
	reg[i] = gethex(4, linebuf+34+5*i, &gottok);
      }
    }
  }
  while (!gottok);
  for (i = 0; i < NUM_REGS; i++) 
  {
    supply_register (i, reg+i);
  }  
}

/* Fetch register REGNO, or all registers if REGNO is -1.
 */
static void
hms_fetch_register (regno)
     int regno;
{

    hms_fetch_registers ();

}

/* Store the remote registers from the contents of the block REGS.  */

static int 
hms_store_registers ()
{
  int i;
  for (i = 0; i < NUM_REGS; i++)
   hms_store_register(i);
  return 0;
  
}

/* Store register REGNO, or all if REGNO == -1.
   Return errno value.  */
int
hms_store_register (regno)
     int regno;
{

  printf ("Expression not found\n");
  if (regno == -1)
   hms_store_registers ();
  else
  {
    char *name = get_reg_name (regno);
    char buffer[100];
    sprintf(buffer,"r %s=%x", name, read_register(regno));
    hms_write_cr(buffer);
    expect_prompt();
  }
  
  DEXIT("hms_store_registers()");
  return 0;
}

/* Get ready to modify the registers array.  On machines which store
   individual registers, this doesn't need to do anything.  On machines
   which store all the registers in one fell swoop, this makes sure
   that registers contains all the registers from the program being
   debugged.  */

void
hms_prepare_to_store ()
{
  /* Do nothing, since we can store individual regs */
}

static CORE_ADDR 
translate_addr(addr)
CORE_ADDR addr;
{

	return(addr);

}

/* Read a word from remote address ADDR and return it.
 * This goes through the data cache.
 */
int
hms_fetch_word (addr)
     CORE_ADDR addr;
{
	return dcache_fetch (addr);
}

/* Write a word WORD into remote address ADDR.
   This goes through the data cache.  */

void
hms_store_word (addr, word)
     CORE_ADDR addr;
     int word;
{
	dcache_poke (addr, word);
}

int
hms_xfer_inferior_memory(memaddr, myaddr, len, write, target)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
     int write;
     struct target_ops *target;			/* ignored */
{
  register int i;
  /* Round starting address down to longword boundary.  */
  register CORE_ADDR addr; 
  /* Round ending address up; get number of longwords that makes.  */
  register int count;
  /* Allocate buffer of that many longwords.  */
  register int *buffer ;

  memaddr &= 0xffff;
  addr = memaddr & - sizeof (int);
  count   = (((memaddr + len) - addr) + sizeof (int) - 1) / sizeof (int);

  printf ("Expression not found\n");
  buffer = (int *)alloca (count * sizeof (int));
  if (write)
  {
    /* Fill start and end extra bytes of buffer with existing memory data.  */

    if (addr != memaddr || len < (int)sizeof (int)) {
	/* Need part of initial word -- fetch it.  */
        buffer[0] = hms_fetch_word (addr);
      }

    if (count > 1)		/* FIXME, avoid if even boundary */
    {
      buffer[count - 1]
       = hms_fetch_word (addr + (count - 1) * sizeof (int));
    }

    /* Copy data to be written over corresponding part of buffer */

    bcopy (myaddr, (char *) buffer + (memaddr & (sizeof (int) - 1)), len);

    /* Write the entire buffer.  */

    for (i = 0; i < count; i++, addr += sizeof (int))
    {
      errno = 0;
      hms_store_word (addr, buffer[i]);
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
      buffer[i] = hms_fetch_word (addr);
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

#if 0
int
hms_xfer_inferior_memory (memaddr, myaddr, len, write)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
     int write;
{
  memaddr &= 0xffff;
  if (write)
   return hms_write_inferior_memory (memaddr, myaddr, len);
  else
   return hms_read_inferior_memory (memaddr, myaddr, len);

}
#endif

int
hms_write_inferior_memory (memaddr, myaddr, len)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
{

  bfd *abfd = bfd_openw(dev_name, "srec");
  asection *a;

  bfd_set_format(abfd, bfd_object);
  a = bfd_make_section(abfd, ".text");
  a->vma = memaddr;
  a->_raw_size = len;
  a->flags = SEC_LOAD | SEC_HAS_CONTENTS;
  hms_write_cr("tl");		/* tell hms here comes the recs */  
  bfd_set_section_contents(abfd, a, myaddr, 0, len);
  bfd_close(abfd);
  
  expect_prompt();
}

void
hms_files_info ()
{
  printf_filtered("\tAttached to %s at %d baud and running program %s\n",
	  dev_name, baudrate, bfd_get_filename(exec_bfd));
  printf_filtered("\ton an H8/300 processor.\n");
}

/* Copy LEN bytes of data from debugger memory at MYADDR
   to inferior's memory at MEMADDR.  Returns errno value.  
 * sb/sh instructions don't work on unaligned addresses, when TU=1. 
 */


/* Read LEN bytes from inferior memory at MEMADDR.  Put the result
   at debugger address MYADDR.  Returns errno value.  */
int
hms_read_inferior_memory(memaddr, myaddr, len)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
{
  /* Align to nearest low 16 bits */
  int i;
				    
#if 0
  CORE_ADDR start = memaddr & ~0xf;
  CORE_ADDR end = ((memaddr + len +16) & ~0xf) -1;
#endif
  CORE_ADDR start = memaddr;
  CORE_ADDR end = memaddr + len -1;
  
  int ok =1;
  
  /*
    AAAA: XXXX XXXX XXXX XXXX XXXX XXXX XXXX XXXX '................'
    012345678901234567890123456789012345678901234567890123456789012345
    0         1         2         3         4         5         6
    */
  char buffer[66];
  if (memaddr & 0xf) abort();
  if (len != 16) abort();
  
  sprintf(buffer, "m %4x %4x", start, end);
  hms_write_cr(buffer);
  /* drop the echo and newline*/
  for (i = 0; i < 13; i++)
   readchar();
  

  
  /* Grab the lines as they come out and fill the area */
  /* Skip over cr */
  while(1)
  {
    int p;
    int i;
    int addr;
    size_t idx;
    
    char byte[16];    
    buffer[0] = readchar();
    if (buffer[0] == 'M') 
     break;
    for (i = 1; i < 66; i++)
     buffer[i] = readchar();
    
    /* Now parse the line */

    addr = gethex(4, buffer, &ok);
    idx = 6;
    for (p = 0; p < 16; p+=2) 
    {
      byte[p] = gethex(2, buffer + idx, &ok);
      byte[p+1] = gethex(2, buffer+ idx + 2, &ok);
      idx+=5;
      
    }

    
    for (p = 0; p<16;p++)
    {
      if (addr + p >= memaddr &&
	  addr + p  < memaddr + len) 
      {
	myaddr[ (addr + p)-memaddr] = byte[p];
	
      }
       
    }
  } 
  
  
  
  hms_write("\003",1);
  expect_prompt();
  

  
  
  return len;

}

/* This routine is run as a hook, just before the main command loop is
   entered.  If gdb is configured for the H8, but has not had its
   target specified yet, this will loop prompting the user to do so.
*/

hms_before_main_loop ()
{
  char ttyname[100];
  char *p, *p2;
  extern FILE *instream;
  extern jmp_buf to_top_level;

  push_target (&hms_ops);
#if 0

  while (current_target != &hms_ops) {
      /* remote tty not specified yet */
      if ( instream == stdin ){
	  printf("\nEnter device and filename, or \"quit\" to quit:  ");
	  fflush( stdout );
	}
      fgets( ttyname, sizeof(ttyname)-1, stdin );

      if ( !strcmp("quit", ttyname) ){
	  exit(1);
	}

      hms_open( ttyname, 1 );

      /* Now that we have a tty open for talking to the remote machine,
	 download the executable file if one was specified.  */
      if ( !setjmp(to_top_level) && exec_bfd ) {
	  target_load (bfd_get_filename (exec_bfd), 1);
	}
    }
#endif
}


#define MAX_BREAKS	16
static int num_brkpts=0;
static int
hms_insert_breakpoint(addr, save)
CORE_ADDR	addr;
char		*save;	/* Throw away, let hms save instructions */
{

  DENTER("hms_insert_breakpoint()"); 
  check_open();
  
  if (num_brkpts < MAX_BREAKS) {
      char buffer[100];
      num_brkpts++;
      sprintf(buffer,"b %x", addr & 0xffff);
      hms_write_cr(buffer);
      expect_prompt ();
      DEXIT("hms_insert_breakpoint() success"); 
      return(0);		/* Success */
    } else {
	fprintf_filtered(stderr,
			 "Too many break points, break point not installed\n");
  	DEXIT("hms_insert_breakpoint() failure"); 
	return(1);		/* Failure */
      }


}
static int
hms_remove_breakpoint(addr, save)
CORE_ADDR	addr;
char		*save;	/* Throw away, let hms save instructions */
{
  DENTER("hms_remove_breakpoint()");
  if (num_brkpts > 0) {
      char buffer[100];
      
      num_brkpts--;
      sprintf(buffer,"b - %x", addr & 0xffff);
      hms_write_cr(buffer);
      expect_prompt();
      
    }
  DEXIT("hms_remove_breakpoint()");
  return(0);
}

/* Clear the hmss notion of what the break points are */
static int
hms_clear_breakpoints() 
{ 

  DENTER("hms_clear_breakpoint()");
  if (OPEN(hms_desc)) {
      hms_write_cr("b -");
      expect_prompt ();
    }
  num_brkpts = 0;

  DEXIT("hms_clear_breakpoint()");
}
static void
hms_mourn() 
{ 
  DENTER("hms_mourn()");
  hms_clear_breakpoints();
/*  pop_target ();                /* Pop back to no-child state */
  generic_mourn_inferior ();
  DEXIT("hms_mourn()");
}

/* Display everthing we read in from the hms until we match/see the
 * specified string
 */
static int
display_until(str)
char	*str;
{
	int	i=0,j,c;

	while (c=readchar()) {
		if (c==str[i]) {
			i++;
			if (i == strlen(str)) return;
		} else {
			if (i) {
			    for (j=0 ; j<i ; j++) /* Put everthing we matched */
				putchar(str[j]);
			    i=0;
			}
			putchar(c);
		}	
	}

}


/* Put a command string, in args, out to the hms.  The hms is assumed to
   be in raw mode, all writing/reading done through hms_desc.
   Ouput from the hms is placed on the users terminal until the
   prompt from the hms is seen.
   FIXME: Can't handle commands that take input.  */

void
hms_com (args, fromtty)
     char	*args;
     int	fromtty;
{
  check_open();
  
  if (!args) return;
	
  /* Clear all input so only command relative output is displayed */


  hms_write_cr(args);
  hms_write("\030",1);
  expect_prompt();

}

/* Define the target subroutine names */

struct target_ops hms_ops = {
	"hms", "Remote HMS monitor",
"Use the H8 evaluation board running the HMS monitor connected\n\
by a serial line.",

	hms_open, hms_close, 
	hms_attach, hms_detach, hms_resume, hms_wait,
	hms_fetch_register, hms_store_register,
	hms_prepare_to_store, 0, 0, 	/* conv_to, conv_from */
	hms_xfer_inferior_memory, 
	hms_files_info,
	hms_insert_breakpoint, hms_remove_breakpoint, /* Breakpoints */
	0, 0, 0, 0, 0,		/* Terminal handling */
	hms_kill, 		/* FIXME, kill */
	hms_load, 
	call_function_by_hand,
	0, 			/* lookup_symbol */
	hms_create_inferior, 	/* create_inferior */ 
	hms_mourn, 		/* mourn_inferior FIXME */
	process_stratum, 0, /* next */
	1, 1, 1, 1, 1,	/* all mem, mem, stack, regs, exec */
	0,0,		/* Section pointers */
	OPS_MAGIC,		/* Always the last thing */
};


hms_quiet()
{
  quiet = ! quiet;  
}

hms_device(s)
char *s;
{
  if (s) {
      dev_name = get_word(&s);
    }  
}

static hms_speed(s)
char *s;
{
  if (s) 
  {
    char buffer[100];
    int newrate = atoi(s);
    int which = 0;
    while (baudtab[which].rate != newrate) 
    {
      if (baudtab[which].rate == -1) 
      {
	error("Can't use %d baud\n", newrate);
      }
      which++;
    }
    

    
    printf_filtered("Checking target is in sync\n");

    
    get_baudrate_right();
    baudrate = newrate;
    printf_filtered("Sending commands to set target to %d\n",
		    baudrate);    
    
    sprintf(buffer, "tm %d. N 8 1", baudrate);
    hms_write(buffer);
  }    
}

/***********************************************************************/

void
_initialize_remote_hms ()
{
  add_target (&hms_ops);
  add_com ("hms <command>", class_obscure, hms_com,
 	"Send a command to the HMS monitor.");
  add_com ("snoop", class_obscure, hms_quiet,
	   "Show what commands are going to the monitor");
  add_com ("device", class_obscure, hms_device,
	   "Set the terminal line for HMS communications");

  add_com ("speed", class_obscure, hms_speed,
	   "Set the terminal line speed for HMS communications");
  
}


