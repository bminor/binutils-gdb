/* Low level interface to ptrace, for GDB when running under Unix.
   Copyright (C) 1986, 1987 Free Software Foundation, Inc.
*/

#include "defs.h"
#include "wait.h"
#include "frame.h"
#include "inferior.h"
/***************************
#include "initialize.h"
****************************/

#include <stdio.h>
#include <sys/param.h>
#include <sys/dir.h>
#include <sys/user.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sgtty.h>
#include <fcntl.h>

/***************Begin MY defs*********************/
int quit_flag = 0;
char registers[REGISTER_BYTES];

/* Index within `registers' of the first byte of the space for
   register N.  */


char buf2[MAX_REGISTER_RAW_SIZE];
/***************End MY defs*********************/

#include <sys/ptrace.h>
#include <machine/reg.h>

extern char **environ;
extern int errno;
extern int inferior_pid;
void error (), quit (), perror_with_name ();
int query ();
void supply_register (), write_register ();
CORE_ADDR read_register ();

/* Nonzero if we are debugging an attached outside process
   rather than an inferior.  */


/* Start an inferior process and returns its pid.
   ALLARGS is a vector of program-name and args.
   ENV is the environment vector to pass.  */

int
create_inferior (allargs, env)
     char **allargs;
     char **env;
{
  int pid;
  extern int sys_nerr;
  extern char *sys_errlist[];
  extern int errno;
  char status;
  char execbuf[1024];

  /* exec is said to fail if the executable is open.  */
  /****************close_exec_file ();*****************/

  sprintf (execbuf, "exec %s", allargs);

  pid = vfork ();
  if (pid < 0)
    perror_with_name ("vfork");

  if (pid == 0)
    {
      /* Run inferior in a separate process group.  */
      setpgrp (getpid (), getpid ());

      errno = 0;
      ptrace (PTRACE_TRACEME);

      execle ("/bin/sh", "sh", "-c", execbuf, 0, env);

      fprintf (stderr, "Cannot exec /bin/sh: %s.\n",
	       errno < sys_nerr ? sys_errlist[errno] : "unknown error");
      fflush (stderr);
      _exit (0177);
    }

  return pid;
}

/* Kill the inferior process.  Make us have no inferior.  */

void
kill_inferior ()
{
  if (inferior_pid == 0)
    return;
  ptrace (8, inferior_pid, 0, 0);
  wait (0);
  /*************inferior_died ();****VK**************/
}

/* Resume execution of the inferior process.
   If STEP is nonzero, single-step it.
   If SIGNAL is nonzero, give it that signal.  */

unsigned char
myresume (step, signal, status)
     int step;
     int signal;
     char *status;
{
  int pid;
  WAITTYPE w;

  errno = 0;
  ptrace (step ? 9 : 7, inferior_pid, 1, signal);
  if (errno)
    perror_with_name ("ptrace");
  pid = wait (&w);
  if (pid != inferior_pid)
    perror_with_name ("wait");

  fetch_inferior_registers (0);

  if (WIFEXITED (w))
    {
      printf ("\nChild exited with retcode = %x \n", WEXITSTATUS (w));
      *status = 'E';
      return ((unsigned char) WEXITSTATUS (w));
    }
  else if (!WIFSTOPPED (w))
    {
      printf ("\nChild terminated with signal = %x \n", WTERMSIG (w));
      *status = 'T';
      return ((unsigned char) WTERMSIG (w));
    }
  else
    {
      printf ("\nChild stopped with signal = %x \n", WSTOPSIG (w));
      *status = 'S';
      return ((unsigned char) WSTOPSIG (w));
    }
}

#define	INT_REGS	1
#define	STACK_REGS	2
#define	FP_REGS		4

/* Fetch one or more registers from the inferior.  REGNO == -1 to get
   them all.  We actually fetch more than requested, when convenient,
   marking them as valid so we won't fetch them again.  */

void
fetch_inferior_registers (ignored)
     int ignored;
{
  struct regs inferior_registers;
  struct fp_status inferior_fp_registers;
  int i;

  /* Global and Out regs are fetched directly, as well as the control
     registers.  If we're getting one of the in or local regs,
     and the stack pointer has not yet been fetched,
     we have to do that first, since they're found in memory relative
     to the stack pointer.  */

  if (ptrace (PTRACE_GETREGS, inferior_pid,
	      (PTRACE_ARG3_TYPE) &inferior_registers, 0))
    perror("ptrace_getregs");
      
  registers[REGISTER_BYTE (0)] = 0;
  memcpy (&registers[REGISTER_BYTE (1)], &inferior_registers.r_g1,
	  15 * REGISTER_RAW_SIZE (G0_REGNUM));
  *(int *)&registers[REGISTER_BYTE (PS_REGNUM)] = inferior_registers.r_ps; 
  *(int *)&registers[REGISTER_BYTE (PC_REGNUM)] = inferior_registers.r_pc;
  *(int *)&registers[REGISTER_BYTE (NPC_REGNUM)] = inferior_registers.r_npc;
  *(int *)&registers[REGISTER_BYTE (Y_REGNUM)] = inferior_registers.r_y;

  /* Floating point registers */

  if (ptrace (PTRACE_GETFPREGS, inferior_pid,
	      (PTRACE_ARG3_TYPE) &inferior_fp_registers,
	      0))
    perror("ptrace_getfpregs");
  memcpy (&registers[REGISTER_BYTE (FP0_REGNUM)], &inferior_fp_registers,
	  sizeof inferior_fp_registers.fpu_fr);

  /* These regs are saved on the stack by the kernel.  Only read them
     all (16 ptrace calls!) if we really need them.  */

  read_inferior_memory (*(CORE_ADDR*)&registers[REGISTER_BYTE (SP_REGNUM)],
			&registers[REGISTER_BYTE (L0_REGNUM)],
			16*REGISTER_RAW_SIZE (L0_REGNUM));
}

/* Store our register values back into the inferior.
   If REGNO is -1, do this for all registers.
   Otherwise, REGNO specifies which register (so we can save time).  */

void
store_inferior_registers (ignored)
     int ignored;
{
  struct regs inferior_registers;
  struct fp_status inferior_fp_registers;
  CORE_ADDR sp = *(CORE_ADDR *)&registers[REGISTER_BYTE (SP_REGNUM)];

  write_inferior_memory (sp, &registers[REGISTER_BYTE (L0_REGNUM)],
			 16*REGISTER_RAW_SIZE (L0_REGNUM));

  memcpy (&inferior_registers.r_g1, &registers[REGISTER_BYTE (G1_REGNUM)],
	  15 * REGISTER_RAW_SIZE (G1_REGNUM));

  inferior_registers.r_ps =
    *(int *)&registers[REGISTER_BYTE (PS_REGNUM)];
  inferior_registers.r_pc =
    *(int *)&registers[REGISTER_BYTE (PC_REGNUM)];
  inferior_registers.r_npc =
    *(int *)&registers[REGISTER_BYTE (NPC_REGNUM)];
  inferior_registers.r_y =
    *(int *)&registers[REGISTER_BYTE (Y_REGNUM)];

  if (ptrace (PTRACE_SETREGS, inferior_pid,
	      (PTRACE_ARG3_TYPE) &inferior_registers, 0))
    perror("ptrace_setregs");

  memcpy (&inferior_fp_registers, &registers[REGISTER_BYTE (FP0_REGNUM)],
	  sizeof inferior_fp_registers.fpu_fr);

  if (ptrace (PTRACE_SETFPREGS, inferior_pid,
	      (PTRACE_ARG3_TYPE) &inferior_fp_registers, 0))
    perror("ptrace_setfpregs");
}

#if 0
void
fetch_inferior_registers ()
{
  struct regs inferior_registers;
  struct fp_status inferior_fp_registers;
  extern char registers[];

  ptrace (PTRACE_GETREGS, inferior_pid, &inferior_registers);
  if (errno)
    perror_with_name ("ptrace");
  /**********debugging begin **********/
  print_some_registers (&inferior_registers);
  /**********debugging end **********/
  ptrace (PTRACE_GETFPREGS, inferior_pid, &inferior_fp_registers);
  if (errno)
    perror_with_name ("ptrace");

  bcopy (&inferior_registers, registers, 16 * 4);
  bcopy (&inferior_fp_registers, &registers[REGISTER_BYTE (FP0_REGNUM)],
	 sizeof inferior_fp_registers.fpu_regs);
  *(int *) &registers[REGISTER_BYTE (PS_REGNUM)] = inferior_registers.r_ps;
  *(int *) &registers[REGISTER_BYTE (PC_REGNUM)] = inferior_registers.r_pc;
  bcopy (&inferior_fp_registers.fpu_flags,
	 &registers[REGISTER_BYTE (FPC_REGNUM)],
      sizeof inferior_fp_registers - sizeof inferior_fp_registers.fpu_regs);
}

/* Store our register values back into the inferior.
   If REGNO is -1, do this for all registers.
   Otherwise, REGNO specifies which register (so we can save time).  */

store_inferior_registers (regno)
     int regno;
{
  struct regs inferior_registers;
  struct fp_status inferior_fp_registers;
  extern char registers[];

  bcopy (registers, &inferior_registers, 16 * 4);
  bcopy (&registers[REGISTER_BYTE (FP0_REGNUM)], &inferior_fp_registers,
	 sizeof inferior_fp_registers.fps_regs);
  inferior_registers.r_ps = *(int *) &registers[REGISTER_BYTE (PS_REGNUM)];
  inferior_registers.r_pc = *(int *) &registers[REGISTER_BYTE (PC_REGNUM)];
  bcopy (&registers[REGISTER_BYTE (FPC_REGNUM)],
	 &inferior_fp_registers.fps_control,
      sizeof inferior_fp_registers - sizeof inferior_fp_registers.fps_regs);

  ptrace (PTRACE_SETREGS, inferior_pid, &inferior_registers);
  if (errno)
    perror_with_name ("ptrace");
  ptrace (PTRACE_SETFPREGS, inferior_pid, &inferior_fp_registers);
  if (errno)
    perror_with_name ("ptrace");
}
#endif /* 0 */

/* NOTE! I tried using PTRACE_READDATA, etc., to read and write memory
   in the NEW_SUN_PTRACE case.
   It ought to be straightforward.  But it appears that writing did
   not write the data that I specified.  I cannot understand where
   it got the data that it actually did write.  */

/* Copy LEN bytes from inferior's memory starting at MEMADDR
   to debugger memory starting at MYADDR.  */

read_inferior_memory (memaddr, myaddr, len)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
{
  register int i;
  /* Round starting address down to longword boundary.  */
  register CORE_ADDR addr = memaddr & -sizeof (int);
  /* Round ending address up; get number of longwords that makes.  */
  register int count
  = (((memaddr + len) - addr) + sizeof (int) - 1) / sizeof (int);
  /* Allocate buffer of that many longwords.  */
  register int *buffer = (int *) alloca (count * sizeof (int));

  /* Read all the longwords */
  for (i = 0; i < count; i++, addr += sizeof (int))
    {
      buffer[i] = ptrace (1, inferior_pid, addr, 0);
    }

  /* Copy appropriate bytes out of the buffer.  */
  bcopy ((char *) buffer + (memaddr & (sizeof (int) - 1)), myaddr, len);
}

/* Copy LEN bytes of data from debugger memory at MYADDR
   to inferior's memory at MEMADDR.
   On failure (cannot write the inferior)
   returns the value of errno.  */

int
write_inferior_memory (memaddr, myaddr, len)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
{
  register int i;
  /* Round starting address down to longword boundary.  */
  register CORE_ADDR addr = memaddr & -sizeof (int);
  /* Round ending address up; get number of longwords that makes.  */
  register int count
  = (((memaddr + len) - addr) + sizeof (int) - 1) / sizeof (int);
  /* Allocate buffer of that many longwords.  */
  register int *buffer = (int *) alloca (count * sizeof (int));
  extern int errno;

  /* Fill start and end extra bytes of buffer with existing memory data.  */

  buffer[0] = ptrace (1, inferior_pid, addr, 0);

  if (count > 1)
    {
      buffer[count - 1]
	= ptrace (1, inferior_pid,
		  addr + (count - 1) * sizeof (int), 0);
    }

  /* Copy data to be written over corresponding part of buffer */

  bcopy (myaddr, (char *) buffer + (memaddr & (sizeof (int) - 1)), len);

  /* Write the entire buffer.  */

  for (i = 0; i < count; i++, addr += sizeof (int))
    {
      errno = 0;
      ptrace (4, inferior_pid, addr, buffer[i]);
      if (errno)
	return errno;
    }

  return 0;
}

void
try_writing_regs_command ()
{
  register int i;
  register int val;

  if (inferior_pid == 0)
    error ("There is no inferior process now.");

  fetch_inferior_registers (0);
  for (i = 0; i < 18; i++)
    {
      QUIT;
      errno = 0;
      val = read_register (i);
      write_register (i, val);
      if (errno == 0)
	{
	  printf (" Succeeded with register %d; value 0x%x (%d).\n",
		  i, val, val);
	}
      else
	printf (" Failed with register %d.\n", i);
    }
}

void
initialize ()
{

  inferior_pid = 0;


}


/* Return the contents of register REGNO,
   regarding it as an integer.  */

CORE_ADDR
read_register (regno)
     int regno;
{
  /* This loses when REGISTER_RAW_SIZE (regno) != sizeof (int) */
  return *(int *) &registers[REGISTER_BYTE (regno)];
}

/* Store VALUE in the register number REGNO, regarded as an integer.  */

void
write_register (regno, val)
     int regno, val;
{
  /* This loses when REGISTER_RAW_SIZE (regno) != sizeof (int) */
  *(int *) &registers[REGISTER_BYTE (regno)] = val;

  if (have_inferior_p ())
    store_inferior_registers (regno);
}


int
have_inferior_p ()
{
  return inferior_pid != 0;
}

print_some_registers (regs)
     int regs[];
{
  register int i;
  for (i = 0; i < 18; i++)
    {
      printf ("reg[%d] = %x\n", i, regs[i]);
    }
}
