/* Low level interface to ptrace, for the remote server for GDB.
   Copyright (C) 1986, 1987, 1993 Free Software Foundation, Inc.

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

#include "server.h"
#include "frame.h"
#include "inferior.h"

#include <stdio.h>
#include <sys/param.h>
#include <sys/dir.h>
#define LYNXOS
#include <sys/mem.h>
#include <sys/signal.h>
#include <sys/file.h>
#include <sys/kernel.h>
#include <sys/itimer.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/proc.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sgtty.h>
#include <fcntl.h>
#include "/usr/include/wait.h"

char registers[REGISTER_BYTES];

#include <sys/ptrace.h>

/* Start an inferior process and returns its pid.
   ALLARGS is a vector of program-name and args. */

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
      int pgrp;

      /* Switch child to it's own process group so that signals won't
	 directly affect gdbserver. */

      pgrp = getpid();
      setpgrp(0, pgrp);
      ioctl (0, TIOCSPGRP, &pgrp);

      ptrace (PTRACE_TRACEME);

      execv (program, allargs);

      fprintf (stderr, "GDBserver (process %d):  Cannot exec %s: %s.\n",
	       getpid(), program,
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
  ptrace (PTRACE_KILL, inferior_pid, 0, 0);
  wait (0);

  inferior_pid = 0;
}

/* Wait for process, returns status */

unsigned char
mywait (status)
     char *status;
{
  int pid;
  union wait w;

  enable_async_io();

  pid = wait (&w);

  disable_async_io();

  if (pid != PIDGET(inferior_pid))
    perror_with_name ("wait");

  inferior_pid = BUILDPID (inferior_pid, w.w_tid);

  if (WIFEXITED (w))
    {
      fprintf (stderr, "\nChild exited with status %d\n", WEXITSTATUS (w));
      fprintf (stderr, "GDBserver exiting\n");
      exit (0);
    }
  else if (!WIFSTOPPED (w))
    {
      fprintf (stderr, "\nChild terminated with signal = %x \n", WTERMSIG (w));
      *status = 'T';
      return ((unsigned char) WTERMSIG (w));
    }

  fetch_inferior_registers (0);

  *status = 'S';
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
  ptrace (step ? PTRACE_SINGLESTEP : PTRACE_CONT, inferior_pid, 1, signal);
  if (errno)
    perror_with_name ("ptrace");
}

#undef offsetof
#define offsetof(TYPE, MEMBER) ((unsigned long) &((TYPE *)0)->MEMBER)

static struct econtext *
lynx_registers_addr()
{
  st_t *stblock;
  int ecpoff = offsetof(st_t, ecp);
  CORE_ADDR ecp;

  errno = 0;
  stblock = (st_t *) ptrace (PTRACE_THREADUSER, inferior_pid,
			     (PTRACE_ARG3_TYPE)0, 0);
  if (errno)
    perror_with_name ("PTRACE_THREADUSER");

  ecp = (CORE_ADDR) ptrace (PTRACE_PEEKTHREAD, inferior_pid,
			    (PTRACE_ARG3_TYPE)ecpoff, 0);
  ecp -= (CORE_ADDR)stblock;
  if (errno)
    perror_with_name ("lynx_registers_addr(PTRACE_PEEKTHREAD)");

  return (struct econtext *)ecp;
}

static struct econtext *ecp;

/* Mapping between GDB register #s and offsets into econtext.  Must be
   consistent with REGISTER_NAMES macro in tm-i386v.h. */

#define X(ENTRY)(offsetof(struct econtext, ENTRY) / 4)
static int regmap[] = {
  X(eax),
  X(ecx),
  X(edx),
  X(ebx),
  X(esp),
  X(ebp),
  X(esi),
  X(edi),
  X(eip),
  X(flags),			/* ps */
  X(cs),
  X(ss),
  X(ds),
  X(es),
  X(ecode),			/* Lynx doesn't give us either fs or gs, so */
  X(fault)			/* we just substitute these two in the hopes
				   that they are useful. */
  };

/* Fetch one or more registers from the inferior.  REGNO == -1 to get
   them all.  We actually fetch more than requested, when convenient,
   marking them as valid so we won't fetch them again.  */

void
fetch_inferior_registers (ignored)
     int ignored;
{
  int regno;
  unsigned long reg;
  struct econtext *ecp;

  ecp = lynx_registers_addr();

  for (regno = 0; regno < NUM_REGS; regno++)
    {
      errno = 0;
      reg = ptrace (PTRACE_PEEKTHREAD, inferior_pid,
		    (PTRACE_ARG3_TYPE) (&ecp->fault + regmap[regno]), 0);
      if (errno)
	perror_with_name ("fetch_inferior_registers(PTRACE_PEEKTHREAD)");
  
      *(unsigned long *)&registers[REGISTER_BYTE (regno)] = reg;
    }
}

/* Store our register values back into the inferior.
   If REGNO is -1, do this for all registers.
   Otherwise, REGNO specifies which register (so we can save time).  */

void
store_inferior_registers (ignored)
     int ignored;
{
  int regno;
  unsigned long reg;
  struct econtext *ecp;

  ecp = lynx_registers_addr();

  for (regno = 0; regno < NUM_REGS; regno++)
    {
      reg = *(unsigned long *)&registers[REGISTER_BYTE (regno)];

      errno = 0;
      ptrace (PTRACE_POKEUSER, inferior_pid,
	      (PTRACE_ARG3_TYPE) (&ecp->fault + regmap[regno]), reg);
      if (errno)
	perror_with_name ("PTRACE_POKEUSER");
    }
}

/* NOTE! I tried using PTRACE_READDATA, etc., to read and write memory
   in the NEW_SUN_PTRACE case.
   It ought to be straightforward.  But it appears that writing did
   not write the data that I specified.  I cannot understand where
   it got the data that it actually did write.  */

/* Copy LEN bytes from inferior's memory starting at MEMADDR
   to debugger memory starting at MYADDR.  */

void
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
      buffer[i] = ptrace (PTRACE_PEEKTEXT, inferior_pid, addr, 0);
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

  buffer[0] = ptrace (PTRACE_PEEKTEXT, inferior_pid, addr, 0);

  if (count > 1)
    {
      buffer[count - 1]
	= ptrace (PTRACE_PEEKTEXT, inferior_pid,
		  addr + (count - 1) * sizeof (int), 0);
    }

  /* Copy data to be written over corresponding part of buffer */

  bcopy (myaddr, (char *) buffer + (memaddr & (sizeof (int) - 1)), len);

  /* Write the entire buffer.  */

  for (i = 0; i < count; i++, addr += sizeof (int))
    {
      while (1)
	{
	  errno = 0;
	  ptrace (PTRACE_POKETEXT, inferior_pid, addr, buffer[i]);
	  if (errno)
	    {
	      fprintf(stderr, "ptrace (PTRACE_POKETEXT): errno=%d, inferior_pid=0x%x, addr=0x%x, buffer[i] = 0x%x\n", errno, inferior_pid, addr, buffer[i]);
	      fprintf(stderr, "Sleeping for 1 second\n");
	      sleep(1);
	    }
	  else
	    break;
	}
    }

  return 0;
}
