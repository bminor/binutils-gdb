/* Native-dependent code for Motorola 680x0 running LynxOS.
   Copyright 1986, 1987, 1989, 1991, 1993 Free Software Foundation, Inc.

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
#include "frame.h"
#include "inferior.h"
#include "gdbcore.h"
#include "target.h"

#include <sys/param.h>
#include <sys/dir.h>
#include <signal.h>
#include <sys/types.h>
#include <itimer.h>
#include <mem.h>
#include <sys/time.h>
#include <resource.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/ptrace.h>
#include <kernel.h>
#include <conf.h>
#include <proc.h>
#include <errno.h>

/* Return the address in the core dump or inferior of register REGNO.
   BLOCKEND is the address of the end of the user structure.  */

static unsigned int
core_register_addr (regno, blockend)
     unsigned int regno, blockend;
{
  struct st_entry s;
  unsigned int offset;

  if (regno >= 0 && regno <= FP_REGNUM)
    offset = (char *) &s.ec.regs[regno] - (char *) &s;
  else if (regno == SP_REGNUM)
    offset = (char *) &s.stackp - (char *) &s;
  else if (regno == PS_REGNUM)
    offset = (char *) &s.ec.status - (char *) &s;
  else if (regno == PC_REGNUM)
    offset = (char *) &s.ec.pc - (char *) &s;
  else if (regno >= FP0_REGNUM && regno <= (FPC_REGNUM - 1))
    offset = (char *) &s.ec.fregs[(regno - FP0_REGNUM) * 3] - (char *) &s;
  else if (regno >= FPC_REGNUM && regno <= FPI_REGNUM)
    offset = (char *) &s.ec.fcregs[regno - FPC_REGNUM] - (char *) &s;

  return blockend + offset;
}

unsigned int
register_addr (regno, blockend)
     int regno, blockend;
{
  struct st_entry s;
  unsigned int offset;

  if (regno >= 0 && regno <= FP_REGNUM)
    offset = (char *) &s.ec.regs[regno] - (char *) &s.ec;
  else if (regno == SP_REGNUM)
    offset = (char *) &s.stackp - (char *) &s;
  else if (regno == PS_REGNUM)
    offset = (char *) &s.ec.status - (char *) &s.ec;
  else if (regno == PC_REGNUM)
    offset = (char *) &s.ec.pc - (char *) &s.ec;
  else if (regno >= FP0_REGNUM && regno <= (FPC_REGNUM - 1))
    offset = (char *) &s.ec.fregs[(regno - FP0_REGNUM) * 3] - (char *) &s.ec;
  else if (regno >= FPC_REGNUM && regno <= FPI_REGNUM)
    offset = (char *) &s.ec.fcregs[regno - FPC_REGNUM] - (char *) &s.ec;

  return blockend + offset;
}

#ifdef FETCH_INFERIOR_REGISTERS

/* Fetch one register. */

static void
fetch_register (regno, offset, bpid)
     int regno, bpid;
     unsigned int offset;
{
  char buf[MAX_REGISTER_RAW_SIZE], mess[128];
  unsigned int regaddr, i, ptrace_request;

  ptrace_request = regno != SP_REGNUM ? PTRACE_PEEKDATA : PTRACE_PEEKUSP;
  regaddr = register_addr (regno, offset);
  for (i = 0; i < REGISTER_RAW_SIZE (regno); i += sizeof (int))
    {
      errno = 0;
      *(int *) &buf[i] = ptrace (ptrace_request, bpid,
				 (PTRACE_ARG3_TYPE) regaddr, 0);
      if (errno != 0)
        {
	  sprintf (mess, "reading register %s (#%d)", reg_names[regno], regno);
	  perror_with_name (mess);
        }
      regaddr += sizeof (int);
    }
  supply_register (regno, buf);
}

static void
store_register (regno, offset, bpid)
     int regno, bpid;
     unsigned int offset;
{
  unsigned int i, regaddr, ptrace_request;
  char mess[128];

  ptrace_request = regno != SP_REGNUM ? PTRACE_POKEUSER : PTRACE_POKEUSP;
  regaddr = register_addr (regno, offset);
  for (i = 0; i < REGISTER_RAW_SIZE (regno); i += sizeof (int))
    {
      errno = 0;
      ptrace (ptrace_request, bpid, (PTRACE_ARG3_TYPE) regaddr,
	      *(int *) &registers[REGISTER_BYTE (regno) + i]);
      if (errno != 0)
        {
	  sprintf (mess, "writing register %s (#%d)", reg_names[regno], regno);
	  perror_with_name (mess);
        }
      regaddr += sizeof (int);
    }
}

static unsigned int
fetch_offset (pid, write)
    int pid, write;
{
  struct st_entry s;
  unsigned int specpage_off, offset = (char *) &s.ecp - (char *) &s;

  if (write)
    {
      errno = 0;
      specpage_off = ptrace (PTRACE_THREADUSER, pid, (PTRACE_ARG3_TYPE) 0, 0);
      if (errno != 0)
	perror_with_name ("ptrace");
      errno = 0;
      offset = ptrace (PTRACE_PEEKTHREAD, pid, (PTRACE_ARG3_TYPE) offset, 0)
	  - specpage_off;
      if (errno != 0)
	perror_with_name ("ptrace");
    }
  else
    {
      errno = 0;
      offset = ptrace (PTRACE_PEEKTHREAD, pid, (PTRACE_ARG3_TYPE) offset, 0);
      if (errno != 0)
	perror_with_name ("ptrace");
    }
  return offset;
}

void
fetch_inferior_registers (regno)
     int regno;
{
  unsigned int offset = fetch_offset (inferior_pid, 0);
  
  if (regno == -1)
    {
      for (regno = 0; regno < NUM_REGS; regno++)
	fetch_register (regno, offset, inferior_pid);
    }
  else 
    fetch_register (regno, offset, inferior_pid);
}

void
store_inferior_registers (regno)
     int regno;
{
    unsigned int offset = fetch_offset (inferior_pid, 1);

    if (regno == -1)
      {
	for (regno = 0; regno < NUM_REGS; regno++)
	  store_register (regno, offset, inferior_pid);
      }
    else
      store_register (regno, offset, inferior_pid);
}

void
fetch_core_registers (core_reg_sect, core_reg_size, which, reg_addr)
     char *core_reg_sect;
     unsigned core_reg_size;
     int which;
     unsigned int reg_addr;	/* Unused in this version */
{
  unsigned int regno;

  for (regno = 0; regno < NUM_REGS; regno++)
    {
      unsigned int addr;

      addr = core_register_addr (regno, (unsigned) 0);
      supply_register (regno, core_reg_sect + addr);
    }
}
#endif	/* FETCH_INFERIOR_REGISTERS */

/* Wait for child to do something.  Return pid of child, or -1 in case
   of error; store status through argument pointer STATUS.  */

int
child_wait (pid, status)
     int pid;
     int *status;
{
  int save_errno;
  int thread;

  while (1)
    {
      int sig;

      if (attach_flag)
	set_sigint_trap();	/* Causes SIGINT to be passed on to the
				   attached process. */
      pid = wait (status);
      save_errno = errno;

      if (attach_flag)
	clear_sigint_trap();

      if (pid == -1)
	{
	  if (save_errno == EINTR)
	    continue;
	  fprintf (stderr, "Child process unexpectedly missing: %s.\n",
		   safe_strerror (save_errno));
	  *status = 42;		/* Claim it exited with signal 42 */
	  return -1;
	}

      if (pid != PIDGET (inferior_pid))	/* Some other process?!? */
	continue;

/*      thread = WIFTID (*status);*/
      thread = *status >> 16;

      /* Initial thread value can only be acquired via wait, so we have to
	 resort to this hack.  */

      if (TIDGET (inferior_pid) == 0)
	{
	  inferior_pid = BUILDPID (inferior_pid, thread);
	  add_thread (inferior_pid);
	}

      pid = BUILDPID (pid, thread);

      return pid;
    }
}
