/* Low level interface to ptrace, for the remote server for GDB.
   Copyright (C) 1986, 1987, 1993, 2000 Free Software Foundation, Inc.

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include "defs.h"
#include <sys/types.h>
#include <sys/wait.h>
#include "frame.h"
#include "inferior.h"

#include <stdio.h>
#include <errno.h>

/***************Begin MY defs*********************/
int quit_flag = 0;
static char my_registers[REGISTER_BYTES];
char *registers = my_registers;

/* Index within `registers' of the first byte of the space for
   register N.  */

char buf2[MAX_REGISTER_RAW_SIZE];
/***************End MY defs*********************/

#include <sys/ptrace.h>
#include <machine/reg.h>

extern int sys_nerr;
// extern char **sys_errlist;
extern char **environ;
extern int inferior_pid;
void quit (), perror_with_name ();

#define RF(dst, src) \
	memcpy(&registers[REGISTER_BYTE(dst)], &src, sizeof(src))

#define RS(src, dst) \
	memcpy(&dst, &registers[REGISTER_BYTE(src)], sizeof(dst))

#ifdef __i386__
struct env387
  {
    unsigned short control;
    unsigned short r0;
    unsigned short status;
    unsigned short r1;
    unsigned short tag;  
    unsigned short r2;
    unsigned long eip;
    unsigned short code_seg;
    unsigned short opcode;
    unsigned long operand; 
    unsigned short operand_seg;
    unsigned short r3;
    unsigned char regs[8][10];
  };

/* i386_register_raw_size[i] is the number of bytes of storage in the
   actual machine representation for register i.  */
int i386_register_raw_size[MAX_NUM_REGS] = {
   4,  4,  4,  4,
   4,  4,  4,  4,
   4,  4,  4,  4,  
   4,  4,  4,  4,
  10, 10, 10, 10, 
  10, 10, 10, 10,
   4,  4,  4,  4,
   4,  4,  4,  4, 
  16, 16, 16, 16,
  16, 16, 16, 16, 
  4
}; 
   
int i386_register_byte[MAX_NUM_REGS];

static void       
initialize_arch()
{
  /* Initialize the table saying where each register starts in the
     register file.  */
  {
    int i, offset;

    offset = 0;
    for (i = 0; i < MAX_NUM_REGS; i++)
      {
        i386_register_byte[i] = offset;
        offset += i386_register_raw_size[i];
      }
  }   
}       
#endif	/* !__i386__ */

#ifdef __powerpc__
static void
initialize_arch()
{
}
#endif	/* !__powerpc__ */


/* Start an inferior process and returns its pid.
   ALLARGS is a vector of program-name and args.
   ENV is the environment vector to pass.  */

int
create_inferior (program, allargs)
     char *program;
     char **allargs;
{
  int pid;

  pid = fork ();
  if (pid < 0)
    perror_with_name ("fork");

  if (pid == 0)
    {
      ptrace (PT_TRACE_ME, 0, 0, 0);

      execv (program, allargs);

      fprintf (stderr, "Cannot exec %s: %s.\n", program,
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
  ptrace (PT_KILL, inferior_pid, 0, 0);
  wait (0);
  /*************inferior_died ();****VK**************/
}

/* Return nonzero if the given thread is still alive.  */
int
mythread_alive (pid)
     int pid;
{
  return 1;
}

/* Wait for process, returns status */

unsigned char
mywait (status)
     char *status;
{
  int pid;
  int w;

  pid = wait (&w);
  if (pid != inferior_pid)
    perror_with_name ("wait");

  if (WIFEXITED (w))
    {
      fprintf (stderr, "\nChild exited with retcode = %x \n", WEXITSTATUS (w));
      *status = 'W';
      return ((unsigned char) WEXITSTATUS (w));
    }
  else if (!WIFSTOPPED (w))
    {
      fprintf (stderr, "\nChild terminated with signal = %x \n", WTERMSIG (w));
      *status = 'X';
      return ((unsigned char) WTERMSIG (w));
    }

  fetch_inferior_registers (0);

  *status = 'T';
  return ((unsigned char) WSTOPSIG (w));
}

/* Resume execution of the inferior process.
   If STEP is nonzero, single-step it.
   If SIGNAL is nonzero, give it that signal.  */

void
myresume (step, signal)
     int step;
     int signal;
{
  errno = 0;
  ptrace (step ? PT_STEP : PT_CONTINUE, inferior_pid, 
	  (PTRACE_ARG3_TYPE) 1, signal);
  if (errno)
    perror_with_name ("ptrace");
}


#ifdef __i386__
/* Fetch one or more registers from the inferior.  REGNO == -1 to get
   them all.  We actually fetch more than requested, when convenient,
   marking them as valid so we won't fetch them again.  */

void
fetch_inferior_registers (ignored)
     int ignored;
{
  struct reg inferior_registers;
  struct env387 inferior_fp_registers;

  ptrace (PT_GETREGS, inferior_pid,
	  (PTRACE_ARG3_TYPE) &inferior_registers, 0);
  ptrace (PT_GETFPREGS, inferior_pid,
	  (PTRACE_ARG3_TYPE) &inferior_fp_registers, 0);

  RF ( 0, inferior_registers.r_eax); 
  RF ( 1, inferior_registers.r_ecx);
  RF ( 2, inferior_registers.r_edx);
  RF ( 3, inferior_registers.r_ebx);
  RF ( 4, inferior_registers.r_esp);
  RF ( 5, inferior_registers.r_ebp);
  RF ( 6, inferior_registers.r_esi);
  RF ( 7, inferior_registers.r_edi);
  RF ( 8, inferior_registers.r_eip);
  RF ( 9, inferior_registers.r_eflags);
  RF (10, inferior_registers.r_cs);
  RF (11, inferior_registers.r_ss);
  RF (12, inferior_registers.r_ds);
  RF (13, inferior_registers.r_es);
  RF (14, inferior_registers.r_fs);
  RF (15, inferior_registers.r_gs);

  RF (FP0_REGNUM,     inferior_fpregisters.regs[0]);
  RF (FP0_REGNUM + 1, inferior_fpregisters.regs[1]);
  RF (FP0_REGNUM + 2, inferior_fpregisters.regs[2]);
  RF (FP0_REGNUM + 3, inferior_fpregisters.regs[3]);
  RF (FP0_REGNUM + 4, inferior_fpregisters.regs[4]);
  RF (FP0_REGNUM + 5, inferior_fpregisters.regs[5]);
  RF (FP0_REGNUM + 6, inferior_fpregisters.regs[6]);
  RF (FP0_REGNUM + 7, inferior_fpregisters.regs[7]);
  
  RF (FCTRL_REGNUM,   inferior_fpregisters.control);
  RF (FSTAT_REGNUM,   inferior_fpregisters.status);
  RF (FTAG_REGNUM,    inferior_fpregisters.tag);
  RF (FCS_REGNUM,     inferior_fpregisters.code_seg);
  RF (FCOFF_REGNUM,   inferior_fpregisters.eip);
  RF (FDS_REGNUM,     inferior_fpregisters.operand_seg);
  RF (FDOFF_REGNUM,   inferior_fpregisters.operand);
  RF (FOP_REGNUM,     inferior_fpregisters.opcode);
}

/* Store our register values back into the inferior.
   If REGNO is -1, do this for all registers.
   Otherwise, REGNO specifies which register (so we can save time).  */

void
store_inferior_registers (ignored)
     int ignored;
{
  struct reg inferior_registers;
  struct env387 inferior_fp_registers;

  RS ( 0, inferior_registers.r_eax); 
  RS ( 1, inferior_registers.r_ecx);
  RS ( 2, inferior_registers.r_edx);
  RS ( 3, inferior_registers.r_ebx);
  RS ( 4, inferior_registers.r_esp);
  RS ( 5, inferior_registers.r_ebp);
  RS ( 6, inferior_registers.r_esi);
  RS ( 7, inferior_registers.r_edi);
  RS ( 8, inferior_registers.r_eip);
  RS ( 9, inferior_registers.r_eflags);
  RS (10, inferior_registers.r_cs);
  RS (11, inferior_registers.r_ss);
  RS (12, inferior_registers.r_ds);
  RS (13, inferior_registers.r_es);
  RS (14, inferior_registers.r_fs);
  RS (15, inferior_registers.r_gs);

  RS (FP0_REGNUM,     inferior_fpregisters.regs[0]);
  RS (FP0_REGNUM + 1, inferior_fpregisters.regs[1]);
  RS (FP0_REGNUM + 2, inferior_fpregisters.regs[2]);
  RS (FP0_REGNUM + 3, inferior_fpregisters.regs[3]);
  RS (FP0_REGNUM + 4, inferior_fpregisters.regs[4]);
  RS (FP0_REGNUM + 5, inferior_fpregisters.regs[5]);
  RS (FP0_REGNUM + 6, inferior_fpregisters.regs[6]);
  RS (FP0_REGNUM + 7, inferior_fpregisters.regs[7]);
  
  RS (FCTRL_REGNUM,   inferior_fpregisters.control);
  RS (FSTAT_REGNUM,   inferior_fpregisters.status);
  RS (FTAG_REGNUM,    inferior_fpregisters.tag);
  RS (FCS_REGNUM,     inferior_fpregisters.code_seg);
  RS (FCOFF_REGNUM,   inferior_fpregisters.eip);
  RS (FDS_REGNUM,     inferior_fpregisters.operand_seg);
  RS (FDOFF_REGNUM,   inferior_fpregisters.operand);
  RS (FOP_REGNUM,     inferior_fpregisters.opcode);

  ptrace (PT_SETREGS, inferior_pid,
	  (PTRACE_ARG3_TYPE) &inferior_registers, 0);
  ptrace (PT_SETFPREGS, inferior_pid,
	  (PTRACE_ARG3_TYPE) &inferior_fp_registers, 0);
}
#endif	/* !__i386__ */

#ifdef __powerpc__
/* Fetch one or more registers from the inferior.  REGNO == -1 to get
   them all.  We actually fetch more than requested, when convenient,
   marking them as valid so we won't fetch them again.  */

void
fetch_inferior_registers (regno)
     int regno;
{
  struct reg inferior_registers;
  struct fpreg inferior_fp_registers;
  int i;

  ptrace (PT_GETREGS, inferior_pid,
	  (PTRACE_ARG3_TYPE) & inferior_registers, 0);
  ptrace (PT_GETFPREGS, inferior_pid,
	  (PTRACE_ARG3_TYPE) & inferior_fp_registers, 0);

  for (i = 0; i < 32; i++)
    RF (i, inferior_registers.fixreg[i]);
  RF (LR_REGNUM, inferior_registers.lr);
  RF (CR_REGNUM, inferior_registers.cr);
  RF (XER_REGNUM, inferior_registers.xer);
  RF (CTR_REGNUM, inferior_registers.ctr);
  RF (PC_REGNUM, inferior_registers.pc);

  for (i = 0; i < 32; i++)
    RF (FP0_REGNUM + i, inferior_fp_registers.r_regs[i]);
}

/* Store our register values back into the inferior.
   If REGNO is -1, do this for all registers.
   Otherwise, REGNO specifies which register (so we can save time).  */

void
store_inferior_registers (regno)
     int regno;
{
  struct reg inferior_registers;
  struct fpreg inferior_fp_registers;
  int i;

  for (i = 0; i < 32; i++)
    RS (i, inferior_registers.fixreg[i]);
  RS (LR_REGNUM, inferior_registers.lr);
  RS (CR_REGNUM, inferior_registers.cr);
  RS (XER_REGNUM, inferior_registers.xer);
  RS (CTR_REGNUM, inferior_registers.ctr);
  RS (PC_REGNUM, inferior_registers.pc);

  for (i = 0; i < 32; i++)
    RS (FP0_REGNUM + i, inferior_fp_registers.r_regs[i]);

  ptrace (PT_SETREGS, inferior_pid,
	  (PTRACE_ARG3_TYPE) & inferior_registers, 0);
  ptrace (PT_SETFPREGS, inferior_pid,
	  (PTRACE_ARG3_TYPE) & inferior_fp_registers, 0);
}
#endif	/* !__powerpc__ */

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
      buffer[i] = ptrace (PT_READ_D, inferior_pid, (PTRACE_ARG3_TYPE) addr, 0);
    }

  /* Copy appropriate bytes out of the buffer.  */
  memcpy (myaddr, (char *) buffer + (memaddr & (sizeof (int) - 1)), len);
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

  buffer[0] = ptrace (PT_READ_D, inferior_pid, (PTRACE_ARG3_TYPE) addr, 0);

  if (count > 1)
    {
      buffer[count - 1]
	= ptrace (PT_READ_D, inferior_pid,
		  (PTRACE_ARG3_TYPE) addr + (count - 1) * sizeof (int), 0);
    }

  /* Copy data to be written over corresponding part of buffer */

  memcpy ((char *) buffer + (memaddr & (sizeof (int) - 1)), myaddr, len);

  /* Write the entire buffer.  */

  for (i = 0; i < count; i++, addr += sizeof (int))
    {
      errno = 0;
      ptrace (PT_WRITE_D, inferior_pid, (PTRACE_ARG3_TYPE) addr, buffer[i]);
      if (errno)
	return errno;
    }

  return 0;
}

void 
initialize_low ()
{
  initialize_arch ();
}
