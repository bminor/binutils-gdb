/* THIS FILE HAS NOT HAD ITS COPYRIGHT CHECKED...FSF SHOULD NOT
   DISTRIBUTE IT UNTIL THIS HAPPENS.  */

/* Memory-access and commands for inferior process, for GDB.
*/

#include "defs.h"
#include <sys/errno.h>
#include <setjmp.h>
#include "frame.h"
#include "value.h"
#include "inferior.h"
#include "symtab.h"

#undef WSTOPSIG
#undef WTERMSIG
#include "wait.h"

#ifdef USG
#include <sys/types.h>
#include <fcntl.h>
#endif

#include <signal.h>
#include <sys/file.h>

#include <termios.h>
#define TERMINAL struct termios

#define LONGTIMEOUT 5
#define SHORTTIMEOUT 1

#define KD_MINUTAE 1
#define KD_LINEDISCIPLINE 2
#define KD_RETRY 4
#define KD_BLOCKTRANSFER 8

#ifndef STDIN
#define STDIN 0
#endif

#define GL_READING 0 /* get line is reading data */
#define GL_OK 1 /* Getline saw the "ok" string */
#define GL_SUCCESS 2 /* Get line got data */
#define GL_TIMEOUT 3 /* Get line timed out */
#define GL_OVERRUN 4 /* Get line filled up the buffer */
#define GL_EXCEPTION 5 /* Get line saw "Exception" */
#define GL_PROMLINE 6 /* Get line saw prom specific info */
#define GL_BLANKLINE 7 /* Get line saw a blank line */

static int kiodebug /* = KD_RETRY | KD_BLOCKTRANSFER */;

static CORE_ADDR remote_pc = 0;
static CORE_ADDR remote_next_pc = 0;
static CORE_ADDR remove_thisbp_next_pc = 0;
static CORE_ADDR remove_thisbp_target = 0;

enum showDrainage {DONTSHOW , SHOW} ;


/* Descriptor for I/O to remote machine.  Initialize it to -1 so that
   remote_open knows that we don't have a file open when the program
   starts.  */
int remote_desc = -1;

int dontskipcrs = 0;

#define	PBUFSIZ	400

unsigned char ignorebuf[PBUFSIZ];
#define IGNORE &ignorebuf[0]

/* Maximum number of bytes to read/write at once.  The value here
   is chosen to fill up a packet (the headers account for the 32).  */
#define MAXBUFBYTES ((PBUFSIZ-32)/2)

static void remote_send ();
static void putpkt ();
static int getpkt ();


/* Open a connection to a remote debugger.
   NAME is the filename used for communication.  */
CORE_ADDR breakpoint_regs_addr;


void
remote_open (name, from_tty)
     char *name;
     int from_tty;
{
  extern int frame_file_full_name;
  unsigned char buf[PBUFSIZ];
  TERMINAL sg;

  remote_debugging = 0;

  if (remote_desc >= 0)
    close (remote_desc);

  breakpoint_regs_addr = parse_and_eval_address("&breakpoint_regs");

  dontskipcrs = !frame_file_full_name; /* if we are running inside of
					 emacs, this will be true. 
					 then skip carriage returns */

  remote_desc = open (name, O_RDWR);
  if (remote_desc < 0)
    perror_with_name (name);

  setup_remote();

  if (from_tty)
    printf ("Remote debugging using %s\n", name);
  remote_debugging = 1;


}
static char *boot_cmd = 0;

static print_boot_cmd()
{
    fprintf(stderr, "boot command set to be \"%s\"\n", boot_cmd);
}

remote_start()
{
  WAITTYPE ignoredWaitType;

  if (boot_cmd)
    {
    sendbreak();
    remote_wait (&ignoredWaitType);
    putpkt ("reset");
    sleep(10);
    sendbreak();
    remote_wait (&ignoredWaitType);
    sleep(10);
    print_boot_cmd();
    putpkt(boot_cmd);
    fprintf(stderr, "rgdb and nucleus synchronized, booting....\n");
    }
  else
    {
    error("The boot command is null. Cannot start the remote kernel/nucleus");
    }
}

/* Close the open connection to the remote debugger.
   Use this when you want to detach and do something else
   with your gdb.  */
void
remote_close (from_tty)
     int from_tty;
{
  if (!remote_debugging)
    error ("Can't close remote connection: not debugging remotely.");
  
  close (remote_desc);		/* This should never be called if
				   there isn't something valid in
				   remote_desc.  */

  /* Do not try to close remote_desc again, later in the program.  */
  remote_desc = -1;

  if (from_tty)
    printf ("Ending remote debugging\n");

  remote_debugging = 0;
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

extern int one_stepped;		/* From machine dependent code */
static int remote_set_one_stepped;

int
remote_resume (pid, step, signal)
     int pid, step, signal;
{
  if (step)
    {
      remote_single_step();
    }
  remote_set_one_stepped = step;
  putpkt("go");
}

/* Wait until the remote machine stops, then return,
   storing status in STATUS just as `wait' would.  */

int
remote_wait (status)
     WAITTYPE *status;
{
  char last, this;
  int pend, saveTheOh = 0;

  user_terminal_raw();

  WSETEXIT ((*status), 0177);
  last = this = 0;

  while (1)
      {
	char buf[PBUFSIZ];
	int readUser, readProm, state;

	doselect(&readUser, &readProm);
	if (readProm)
	  {
	    switch (state = getline(buf, PBUFSIZ, SHORTTIMEOUT))
	      {
	      case GL_BLANKLINE:
		if (remote_set_one_stepped)
		  break;
		
		/* fall through */

	      default:
	      case GL_READING:
	      case GL_SUCCESS:
	      case GL_OVERRUN:
	      case GL_TIMEOUT:
		if (kiodebug & KD_LINEDISCIPLINE)
		  fprintf(stderr, "%d<%s>\n", state, buf);
		else
		  {
		    fprintf(stderr, "%s", buf);
		    fflush(stderr);
		  }
		break;
	      case GL_OK:
		remote_cleanup_after_stop();
		WSETSTOP ((*status), SIGTRAP);
		return;
	      case GL_PROMLINE:
		break;
	      }
	  }
	if (readUser)
	  shuffleFromUserToProm();
      }
}
static TERMINAL userterminal;
	
user_terminal_restore()
{
#if 0
  int in_desc = fileno (stdin);
  ioctl (in_desc, TCSETS, &userterminal);
#endif
}
static void set_term_raw();

user_terminal_raw()
{
#if 0
  TERMINAL tempterminal;
  int in_desc = fileno (stdin);
  ioctl (in_desc, TCGETS, &userterminal);
  tempterminal = userterminal;

  tempterminal.c_lflag &= ~(ICANON|ISIG|IEXTEN);
  tempterminal.c_cc[VMIN] = 1;
  tempterminal.c_cc[VTIME] = 0;
  tempterminal.c_iflag &= ~(INPCK|IXON|IXOFF);
  tempterminal.c_oflag = 0;

  ioctl (in_desc, TCSETS, &tempterminal);
#endif
}

doselect(pReadUser, pReadProm)
     int *pReadUser, *pReadProm;
{
  extern FILE *instream;
  int in_desc = fileno (stdin);
  int instreammask = 1 << in_desc;
  int remotemask = 1 << remote_desc;
  int rfds = instreammask | remotemask;

  select (32, &rfds, 0, 0, (struct timeval *) 0); /* 0 = Block indefinitely */
  *pReadUser = (rfds & instreammask) == instreammask;
  *pReadProm = (rfds & remotemask) == remotemask;
}



/* Read the remote registers into the block pRegisters.
implementation copied largely from fetch_inferior_registers () 
in sparc-dep.c */

void
remote_fetch_registers(ignored)
int *ignored;
{
  struct regs inferior_registers;
  extern char registers[];
  CORE_ADDR breakpoint_regs_target;

  if (breakpoint_regs_addr == 0)
    {
      error("no address for breakpoint_regs\n");
      return;
    }
  remote_read_inferior_memory(breakpoint_regs_addr, &breakpoint_regs_target,
			      sizeof(breakpoint_regs_target));

  bzero(registers, REGISTER_BYTES);
  registers[REGISTER_BYTE (0)] = 0;

  if (breakpoint_regs_target)
    {
      remote_read_inferior_memory(breakpoint_regs_target, &inferior_registers,
				  sizeof(inferior_registers));
      registers[REGISTER_BYTE (0)] = 0;
      bcopy (&inferior_registers.r_g1, &registers[REGISTER_BYTE (1)], 15 * 4);
      *(int *)&registers[REGISTER_BYTE (PS_REGNUM)] = inferior_registers.r_ps; 
      *(int *)&registers[REGISTER_BYTE (PC_REGNUM)] = inferior_registers.r_pc;
      *(int *)&registers[REGISTER_BYTE (NPC_REGNUM)] = inferior_registers.r_npc;
      *(int *)&registers[REGISTER_BYTE (Y_REGNUM)] = inferior_registers.r_y;
      remote_pc =  inferior_registers.r_pc; 
      remote_next_pc =  inferior_registers.r_npc; 
      remote_read_inferior_memory (inferior_registers.r_sp,
				   &registers[REGISTER_BYTE (16)],
				   16*4);
    }
  else
    {
      error("breakpoint_regs == 0\n");
    }
}




/* Write memory data directly to the remote machine.
   This does not inform the data cache; the data cache uses this.
   MEMADDR is the address in the remote memory space.
   MYADDR is the address of the buffer in our space.
   LEN is the number of bytes.  */

int
remote_write_bytes (memaddr, myaddr, len)
     CORE_ADDR memaddr;
     unsigned char *myaddr;
     int len;
{
  char buf[PBUFSIZ];
  int i;

  /* Command describes registers byte by byte,
     each byte encoded as two hex characters.  */

  for (i = 0; i < len; i++)
    {
    sprintf(buf, "%x %x c!", myaddr[i], memaddr + i); 
    remote_send (buf, buf);
    if (strstr(buf, "Exception"))
      {
	return EFAULT;	  
      }
    }
  return 0;	  
}

/* Copy LEN bytes of data from debugger memory at MYADDR
   to inferior's memory at MEMADDR.  Returns errno value.  */
int
remote_write_inferior_memory (memaddr, myaddr, len)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
{
  int xfersize;
  int retval;

  while (len > 0)
    {
      if (len > MAXBUFBYTES)
	xfersize = MAXBUFBYTES;
      else
	xfersize = len;
      
      retval = remote_write_bytes(memaddr, myaddr, xfersize);
      if (retval)
	return retval; /* error */
      
      memaddr += xfersize;
      myaddr  += xfersize;
      len     -= xfersize;
    }
  return 0; /* no error */
}


/* read a single character */

static int
readCharFromProm ()
{
  char buf;

  buf = '\0';
  /* termio does the timeout for us.  */
  read (remote_desc, &buf, 1);
  return buf & 0x7f;
}

/* Send the command in BUF to the remote machine,
   and read the reply into BUF.
   Report an error if we get an error reply.  */

static void
remote_send (buf, buf2)
     char *buf, *buf2;
{
  putpkt (buf);
  getpkt (buf2);
}

/* Send a single character out over the wire */

static void
putcharacter (ch)
     char ch;
{

  while (1)
    {
      int i;

      write(remote_desc, &ch, 1);
      for (i = 0; i < 100; i++)
	{
	  char nch = 0;

	  if (read (remote_desc, &nch, 1) == 0)
	    i++;
	  if ((ch == nch)
	      || (ch == '\n' && nch == '\r')
	      || (ch == '\r' && nch == '\n'))
	    return;
	  if (kiodebug & KD_MINUTAE)
	    fprintf (stderr, "Sent %c(%d) Received %c(%d)\n", ch, ch, nch, nch);
	}
    }
}

/* Send a packet to the remote machine, with error checking.
   The data of the packet is in BUF.  */

static void
putpkt (buf)
     char *buf;
{
  int i;
  int cnt = strlen (buf);
  char ch;

  if (kiodebug & KD_LINEDISCIPLINE)
    fprintf(stderr, "putpkt(%s)\n", buf);

  for (i = 0; i < cnt; i++)
      putcharacter (buf[i]);
  putcharacter ('\n');
}

jmp_buf getline_jmpbuf;

/* Read a line from the remote machine, and store it in BUF.  */
getline_timer()
{
  alarm(0);
  
  if (kiodebug & KD_RETRY)
    fprintf(stderr, "getline timed out\n");
  longjmp(getline_jmpbuf, 1);
}

static int
getline (buf, size, timeout)
     char *buf;
     int size, timeout;
{
  int cnt = 0;
  int state;
  int isspace_state = 1;

  if ((void (*)) signal (SIGALRM, getline_timer) == (void (*)) -1)
    perror ("remote_open: error in signal");

  --size; /* back it up one so that we can read */

  state = GL_READING;

  if (setjmp(getline_jmpbuf))
    state = GL_TIMEOUT;
  else
    {
      alarm (timeout);
      do
	{
	  char ch = readCharFromProm();
	  isspace_state = isspace_state && isspace(ch);
	  if (ch && (dontskipcrs || ch != '\r'))
	    {
	      buf[cnt++] = ch;
	      buf[cnt] = '\0';
	    }
	  if (kiodebug & KD_MINUTAE)
	    fprintf (stderr,"letter received :%c\n", buf[cnt - 1]);
	  if (cnt >= 2 && buf[cnt - 2] == 'o' && buf[cnt - 1] == 'k')
	      state = GL_OK;
	  else if (buf[cnt - 1] == '\n' )
	    state = isspace_state ?  GL_BLANKLINE : GL_SUCCESS;
	  else if (cnt == size)
	    state = GL_OVERRUN;
	  else if (strstr(buf, "Type  'go' to resume"))
	    state = GL_PROMLINE;
	  else if (strstr(buf, "Type  help  for more information"))
	    state = GL_PROMLINE;
	  else if (strstr(buf, "Exception"))
	    state = GL_EXCEPTION;
	}
      while (state == GL_READING);
    }
  alarm (0);
  
  if (kiodebug & KD_LINEDISCIPLINE)
    fprintf (stderr,"Line received :%s\n", buf);
  return state;
}


/* Read a packet from the remote machine, and store it in BUF.  */

static int
getpkt (buf)
     char *buf;
{
  int cnt = 0;

    do
      {
      char ch = readCharFromProm();
      if (ch)
	buf[cnt++] = ch;
      if (kiodebug & KD_MINUTAE)
	fprintf (stderr,"letter received :%c\n", buf[cnt - 1]);
      }
    while (cnt < 2 ||
      buf[cnt - 2] != 'o' &&
      buf[cnt - 1] != 'k');

    buf[cnt] = '\0';
    if (kiodebug& KD_LINEDISCIPLINE)
      fprintf (stderr,"Packet received :%s\n", buf);
    return cnt;
}

void remote_fetch_word (addr)
     CORE_ADDR addr;
{
  error ("Internal error: remote_fetch_word is obsolete.\n");
}
void remote_store_word (addr)
     CORE_ADDR addr;
{
  error ("Internal error: remote_store_word is obsolete.\n");
}
#include <termio.h>

draininput(showit)
enum showDrainage showit;
{
  unsigned char buf[PBUFSIZ];
  int cnt;

  while ((cnt = read(remote_desc, buf, PBUFSIZ)) > 0)
    {
      buf[cnt] = 0;
      if (kiodebug& KD_LINEDISCIPLINE)
	fprintf (stderr,"Draining :%s\n", buf);
      else
	if (showit == SHOW)
	  fprintf (stderr,"%s", buf);
    }
  if (kiodebug& KD_LINEDISCIPLINE)
    fprintf (stderr,"Drained\n");
}
sendbreak()
{
  if (kiodebug & KD_RETRY)
    fprintf (stderr,"rgdb sending break to target...\n");
  else
    {
      fprintf (stderr,"=");
      fflush(stderr);
    }
    
  ioctl (remote_desc, TCSBRK, 0);
  sleep(5);
}


/* shuffle a character from the user to remote debugger */

int 
shuffleFromUserToProm()
{
  char ch;
  static int escape = 0;
    
  extern FILE *instream;
  
  ch = 0;
  if (read(STDIN, &ch , 1) != 1 || ch == 0)
    return;

  if (escape) {
    if (ch == '#')
      sendbreak();
    else if (ch == '.')
	{
	  while (ch != '\n')	
	    read(STDIN, &ch , 1);
	  return 1;
	}
    else {
      static char tilde = '~';

      putcharacter(tilde);
      putcharacter(ch);
    }
    escape = 0;
  } else /* not escape */ {
    if (ch == '~')
      escape = 1;
    else
      putcharacter(ch);
  }
  return 0;
}
  
  

/* Tell  the Prom put a breakpoint at memaddr */
remote_insert_breakpoint(memaddr)
     CORE_ADDR memaddr;
{
  char buf[PBUFSIZ];

  /* Command describes registers byte by byte,
     each byte encoded as two hex characters.  */

  sprintf(buf, "%x +bp", memaddr);
  remote_send(buf, buf);
  if (strstr(buf, "Exception"))
    {
      return EFAULT;	  
    }
  else
    {
      return 0;	  
    }
}

/* Tell  the Prom remove the the breakpoint at memaddr */
remote_remove_breakpoint(memaddr)
     CORE_ADDR memaddr;
{
  char buf[PBUFSIZ];

  /* Command describes registers byte by byte,
     each byte encoded as two hex characters.  */

  sprintf(buf, "%x -bp", memaddr);
  remote_send(buf, buf);
  if (strstr(buf, "Exception"))
    {
      return EFAULT;	  
    }
  else
    {
      return 0;	  
    }
}





/* Read memory data directly from the remote machine.
   This does not use the data cache; the data cache uses this.
   MEMADDR is the address in the remote memory space.
   MYADDR is the address of the buffer in our space.
   LEN is the number of words.  */

long
remote_read(memaddr, myaddr, len, increment, promcommand)
     CORE_ADDR memaddr;
     unsigned char *myaddr;
     int len, increment;
     char *promcommand;
{
  char buf[PBUFSIZ];
  char buf2[PBUFSIZ];
  int i;
  unsigned long num;

  /* Command describes registers byte by byte,
     each byte encoded as two hex characters.  */

  for (i = 0; i < len; i += increment)
    {
      sprintf(buf, promcommand, memaddr + i) ;
      remote_send(buf, buf2);
      remote_send(".", buf);
      if (strstr(buf2, "Exception"))
	{
	  bzero(&myaddr[i], len - i);
	  return -i;	  
	}
      else
	{
	  char *pBuf;
	  for (pBuf = &buf[0]; *pBuf == '\r' || *pBuf == '\n'; pBuf++)
	    ;
	  sscanf(pBuf, "%x\n", &num);
	  switch (increment)
	    {
	    case 1: myaddr[i] = num;
	      if (num > 255)
		fprintf(stderr, "number out of bounds %x truncating to %x\n",
			num, myaddr[i]);
	      break;
	    case 4: {unsigned long *p;
		     p = (unsigned long *) &myaddr[i];
		     *p = num;
		     }
	      break;
	    default: fprintf(stderr, "unknown increment\n"); break;
	    }
	}
    }
  return i;
}



/* Read LEN bytes from inferior memory at MEMADDR.  Put the result
   at debugger address MYADDR.  Returns errno value.  */
int
remote_read_inferior_memory(memaddr, myaddr, len)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
{
  int xfersize;
  while (len > 0)
    {
      int mod;

      if (len > MAXBUFBYTES)
	xfersize = MAXBUFBYTES;
      else
	xfersize = len;

      mod = memaddr % 4;
      if  (mod == 0 && xfersize >= 4)
	if  (mod == 0 && xfersize >= 16)
	  {
	    xfersize = remote_read_many(memaddr, myaddr, (len & ~3));
	    getpkt(IGNORE);
	  }
	else
	  xfersize = remote_read(memaddr, myaddr, 4, 4, "%x @");
      else
	xfersize = remote_read(memaddr, myaddr, max(mod, 1), 1, "%x c@");
      if (xfersize <= 0)
	return EFAULT; /* error */
      memaddr += xfersize;
      myaddr  += xfersize;
      len     -= xfersize;
    }
  return 0; /* no error */
}
static int baud_rate=B38400;

static void set_term_raw(pTermio)
     TERMINAL *pTermio;
{
  pTermio->c_cflag &= (CREAD|HUPCL|CLOCAL);
  pTermio->c_cflag |= baud_rate | CS8;
  pTermio->c_iflag = ISTRIP /* | IXON | IXOFF */;
  pTermio->c_oflag = 0;
  pTermio->c_lflag = 0;
  pTermio->c_cc[VMIN] = 0;
  pTermio->c_cc[VTIME] = 1;
}

/* setup the remote termio stream */
setup_remote()
{
  TERMINAL temptempio;

  ioctl(remote_desc, TCGETS, &temptempio);
  set_term_raw(&temptempio);
  ioctl(remote_desc, TCSETS, &temptempio);
}

/* step one machine instruction */
remote_single_step ()
{
  CORE_ADDR next_pc, npc4, target, pc;
  typedef enum
    {
      Error, not_branch, bicc, bicca, ba, baa, ticc, ta
    } branch_type;
  branch_type br, isbranch ();

  npc4 = remote_next_pc + 4; /* branch not taken */

  /* Always set breakpoint for NPC.  */

  remote_insert_breakpoint(remote_next_pc);
  remove_thisbp_next_pc = remote_next_pc;

  /* printf ("set break at %x\n",remote_next_pc); */
  
  br = isbranch (remote_pc, &target);
  
  if (br == bicca)
    {
      /* Conditional annulled branch will either end up at
	 npc (if taken) or at npc+4 (if not taken).
	 Trap npc+4.  */
      remote_insert_breakpoint(npc4);
      remove_thisbp_target = npc4;
    }
  else if (br == baa && target != remote_next_pc)
    {
      /* Unconditional annulled branch will always end up at
	 the target.  */
      remote_insert_breakpoint(target);
      remove_thisbp_target = target;
    }
}




/* read many words of memory */
long
remote_read_many(memaddr, myaddr, len)
     CORE_ADDR memaddr;
     unsigned char *myaddr;
     int len;
{
#define BLOCKSIZE 1024
  static int max_number_of_blocks = 24;

  char buf[PBUFSIZ];
  char buf2[PBUFSIZ];
  int i;
  unsigned long *p;
/* Command describes registers byte by byte,
     each byte encoded as two hex characters.  */

  len = min(len, max_number_of_blocks * BLOCKSIZE);

  sprintf(buf, "%x %x do i @ . cr 4 +loop", memaddr + len, memaddr);
  putpkt(buf);
  getline(buf2, PBUFSIZ, LONGTIMEOUT); /* I don't care */

  p = (unsigned long *) myaddr;
  for (i = 0; i < len; i += 4, p++)
    {
      extern int InspectIt;

      if (!InspectIt && ((i % BLOCKSIZE) == 0))
	  fprintf(stderr, "+"); /* let 'em know that we are working */
      switch (getline(buf2, PBUFSIZ, LONGTIMEOUT))
	{
	default:
	case GL_PROMLINE:
	case GL_READING:
	case GL_OK:
	case GL_OVERRUN:
	case GL_TIMEOUT:
	case GL_BLANKLINE:
	  /* resync and retry */
	  max_number_of_blocks = max(1, i / BLOCKSIZE);
	  fprintf(stderr, "-"); /* let 'em know that we are working */
	  
	  if (kiodebug & KD_BLOCKTRANSFER)
	    fprintf(stderr, "failed read_many %d %d/%d (%s)\n",
		    max_number_of_blocks, i, len, buf2);
	  sendbreak();
	  return remote_read_many(memaddr, myaddr, len);
	case GL_EXCEPTION:
	  return -i;	  
	case GL_SUCCESS:
	  sscanf(buf2, "%x\n", p);
	  break;
	}
    }
  if (kiodebug & KD_BLOCKTRANSFER)
    fprintf(stderr, "success read_many %d %d/%d (%s)\n", max_number_of_blocks,
	    i, len, buf2);
  return i;
}
/*
 * allow the user to type directly to the prom ! 
 */
prom_command()
{
  int readUser, readProm;

  user_terminal_raw();
  fprintf(stderr, "entering prom mode...\n");
  while (1)
    {
      doselect(&readUser, &readProm);
      if (readUser)
	if (shuffleFromUserToProm())
	  {
	  fprintf(stderr, "exiting prom mode\n");
	  user_terminal_restore();
	  return;
	}
      if (readProm)
	fprintf(stderr, "%c", readCharFromProm ());
    }
}
static char *boot_set_msg = "boot needs a string in quotes of the form \"boot vmunix\"  ";
static char *baud_set_msg = "baud rate should be of the form \"set baud=9600\"";

static void
set_boot (arg, from_tty)
     char *arg;
     int from_tty;
{
  int h, i;

  if (!arg)
    {
    print_boot_cmd();
    error_no_arg (boot_set_msg);
    }
  
  arg = tilde_expand (arg);
  make_cleanup (free, arg);

  i = strlen (arg) - 1;
  
  free (boot_cmd);
  
  h = 0;
  while (*arg && h < i && (arg[h] == ' ' || arg[h] == '\t'))
    {
    h++;
    arg++;
    }
  while (i > 0 && (arg[i] == ' ' || arg[i] == '\t'))
    i--;

  if (h >= i || !*arg || arg[h] != '"' || arg[i] != '"')
    error (boot_set_msg);
  else
    {
    boot_cmd = savestring (++arg, i);
    boot_cmd[i - 1] = '\0';
    }
 if (from_tty)
    print_boot_cmd();
}

static int bauds[] = {
	0, 50, 75, 110, 134, 150, 200, 300, 600,
	1200, 1800, 2400, 4800, 9600, 19200, 38400, -1
};


static int convert_to_baud_B(n)
	int n;
{
	register int *p;

	for (p = bauds; *p != -1;  p++)
		if (*p != 0 && *p == n)
			return (p - bauds);
	return (NULL);
}

static void print_acceptable_bauds()
{
	register int *p;

	for (p = bauds; *p != -1;  p++)
		if (*p != 0 )
			fprintf(stderr, "%d\n", *p);
}

static void print_baud()
{
fprintf(stderr, "the baud rate is now %d\n", bauds[baud_rate]);
}

static void
set_baud (arg, from_tty)
     char *arg;
     int from_tty;
{
  int temp_baud_rate;

  if (!arg)
    {
    print_baud();
    print_acceptable_bauds();
    error_no_arg (baud_set_msg);
    return;
    }

  while (*arg && !isdigit(*arg))
    arg++;
  
  if (*arg  && (temp_baud_rate = convert_to_baud_B(atoi(arg))) != NULL)
    {
	baud_rate = temp_baud_rate;
	if (remote_debugging)
	  setup_remote();
    }
  else
    {
      fprintf(stderr, "bad baud rate %s, acceptable values are\n", arg);
      print_acceptable_bauds();
    }

  print_baud();
}




void
_initialize_remote()
{
/* Chain containing all defined set subcommands */

extern struct cmd_list_element *setlist;


  add_com ("prom", class_obscure, prom_command,
	   "Conduct a dialogue directly with the prom. \
only useful after an attach\n\
Terminate by typing ~.");

  add_cmd ("boot_cmd", class_support, set_boot, boot_set_msg, &setlist);

  add_cmd ("baud", class_support, set_baud, baud_set_msg, &setlist);

  set_boot ("\"boot nucleus -d\"", 0);
  }


/* Store the remote registers from the contents of the block REGS.  */

void
remote_store_registers (registers)
     char *registers;
{
  CORE_ADDR core;
  struct regs inferior_registers;

  core = parse_and_eval_address("breakpoint_regs");

  bcopy (&registers[REGISTER_BYTE (1)],
	 &inferior_registers.r_g1, 15 * 4);

  inferior_registers.r_ps =
    *(int *)&registers[REGISTER_BYTE (PS_REGNUM)];
  inferior_registers.r_pc =
    *(int *)&registers[REGISTER_BYTE (PC_REGNUM)];
  inferior_registers.r_npc =
    *(int *)&registers[REGISTER_BYTE (NPC_REGNUM)];
  inferior_registers.r_y =
    *(int *)&registers[REGISTER_BYTE (Y_REGNUM)];

  remote_write_inferior_memory (*(int *)&registers[REGISTER_BYTE (SP_REGNUM)],
				&registers[REGISTER_BYTE (16)],
				16*4);
  remote_write_inferior_memory (core,
				&inferior_registers, 
				sizeof(inferior_registers));
}



/* we have stopped. do some cleanup */
remote_cleanup_after_stop()
{
  if (remove_thisbp_next_pc)
    {
      remote_remove_breakpoint (remove_thisbp_next_pc);
      remove_thisbp_next_pc = 0;
    }
  if (remove_thisbp_target)
    {
      remote_remove_breakpoint (remove_thisbp_target);
      remove_thisbp_target = 0;
    }
  user_terminal_restore();
  
  one_stepped = remote_set_one_stepped;
}
