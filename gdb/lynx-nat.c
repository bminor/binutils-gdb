/* Native-dependent code for LynxOS.
   Copyright 1993 Free Software Foundation, Inc.

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
#include "target.h"

#include <sys/ptrace.h>
#include "/usr/include/sys/wait.h"

static unsigned long registers_addr PARAMS ((int pid));

#define X(ENTRY)(offsetof(struct econtext, ENTRY))

#ifdef I386
/* Mappings from tm-i386v.h */

static int regmap[] =
{
  X(eax),
  X(ecx),
  X(edx),
  X(ebx),
  X(esp),			/* sp */
  X(ebp),			/* fp */
  X(esi),
  X(edi),
  X(eip),			/* pc */
  X(flags),			/* ps */
  X(cs),
  X(ss),
  X(ds),
  X(es),
  X(ecode),			/* Lynx doesn't give us either fs or gs, so */
  X(fault),			/* we just substitute these two in the hopes
				   that they are useful. */
};
#endif

#ifdef M68K
/* Mappings from tm-m68k.h */

static int regmap[] =
{
  X(regs[0]),			/* d0 */
  X(regs[1]),			/* d1 */
  X(regs[2]),			/* d2 */
  X(regs[3]),			/* d3 */
  X(regs[4]),			/* d4 */
  X(regs[5]),			/* d5 */
  X(regs[6]),			/* d6 */
  X(regs[7]),			/* d7 */
  X(regs[8]),			/* a0 */
  X(regs[9]),			/* a1 */
  X(regs[10]),			/* a2 */
  X(regs[11]),			/* a3 */
  X(regs[12]),			/* a4 */
  X(regs[13]),			/* a5 */
  X(regs[14]),			/* fp */
  offsetof (st_t, usp) - offsetof (st_t, ec), /* sp */
  X(status),			/* ps */
  X(pc),

  X(fregs[0*3]),		/* fp0 */
  X(fregs[1*3]),		/* fp1 */
  X(fregs[2*3]),		/* fp2 */
  X(fregs[3*3]),		/* fp3 */
  X(fregs[4*3]),		/* fp4 */
  X(fregs[5*3]),		/* fp5 */
  X(fregs[6*3]),		/* fp6 */
  X(fregs[7*3]),		/* fp7 */

  X(fcregs[0]),			/* fpcontrol */
  X(fcregs[1]),			/* fpstatus */
  X(fcregs[2]),			/* fpiaddr */
  X(ssw),			/* fpcode */
  X(fault),			/* fpflags */
};
#endif

/* Return the offset relative to the start of the per-thread data to the
   saved context block.  */

static unsigned long
registers_addr(pid)
     int pid;
{
  CORE_ADDR stblock;
  int ecpoff = offsetof(st_t, ecp);
  CORE_ADDR ecp;

  errno = 0;
  stblock = (CORE_ADDR) ptrace (PTRACE_THREADUSER, pid, (PTRACE_ARG3_TYPE)0,
				0);
  if (errno)
    perror_with_name ("registers_addr(PTRACE_THREADUSER)");

  ecp = (CORE_ADDR) ptrace (PTRACE_PEEKTHREAD, pid, (PTRACE_ARG3_TYPE)ecpoff,
			    0);
  if (errno)
    perror_with_name ("registers_addr(PTRACE_PEEKTHREAD)");

  return ecp - stblock;
}

/* Fetch one or more registers from the inferior.  REGNO == -1 to get
   them all.  We actually fetch more than requested, when convenient,
   marking them as valid so we won't fetch them again.  */

void
fetch_inferior_registers (regno)
     int regno;
{
  int reglo, reghi;
  int i;
  unsigned long ecp;

  if (regno == -1)
    {
      reglo = 0;
      reghi = NUM_REGS - 1;
    }
  else
    reglo = reghi = regno;

  ecp = registers_addr (inferior_pid);

  for (regno = reglo; regno <= reghi; regno++)
    {
      char buf[MAX_REGISTER_RAW_SIZE];
      int ptrace_fun = PTRACE_PEEKTHREAD;

#ifdef PTRACE_PEEKUSP
      ptrace_fun = regno == SP_REGNUM ? PTRACE_PEEKUSP : PTRACE_PEEKTHREAD;
#endif

      for (i = 0; i < REGISTER_RAW_SIZE (regno); i += sizeof (int))
	{
	  unsigned int reg;

	  errno = 0;
	  reg = ptrace (ptrace_fun, inferior_pid,
			(PTRACE_ARG3_TYPE) (ecp + regmap[regno] + i), 0);
	  if (errno)
	    perror_with_name ("fetch_inferior_registers(ptrace)");
  
	  *(int *)&buf[i] = reg;
	}
      supply_register (regno, buf);
    }
}

/* Store our register values back into the inferior.
   If REGNO is -1, do this for all registers.
   Otherwise, REGNO specifies which register (so we can save time).  */

void
store_inferior_registers (regno)
     int regno;
{
  int reglo, reghi;
  int i;
  unsigned long ecp;

  if (regno == -1)
    {
      reglo = 0;
      reghi = NUM_REGS - 1;
    }
  else
    reglo = reghi = regno;

  ecp = registers_addr (inferior_pid);

  for (regno = reglo; regno <= reghi; regno++)
    {
      int ptrace_fun = PTRACE_POKEUSER;

#ifdef PTRACE_POKEUSP
      ptrace_fun = regno == SP_REGNUM ? PTRACE_POKEUSP : PTRACE_POKEUSER;
#endif

      for (i = 0; i < REGISTER_RAW_SIZE (regno); i += sizeof (int))
	{
	  unsigned int reg;

	  reg = *(unsigned int *)&registers[REGISTER_BYTE (regno) + i];

	  errno = 0;
	  ptrace (ptrace_fun, inferior_pid,
		  (PTRACE_ARG3_TYPE) (ecp + regmap[regno] + i), reg);
	  if (errno)
	    perror_with_name ("PTRACE_POKEUSER");
	}
    }
}

/* Wait for child to do something.  Return pid of child, or -1 in case
   of error; store status through argument pointer OURSTATUS.  */

int
child_wait (pid, status)
     int pid;
     struct target_waitstatus *ourstatus;
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
	  fprintf_unfiltered (gdb_stderr, "Child process unexpectedly missing: %s.\n",
		   safe_strerror (save_errno));
	  /* Claim it exited with unknown signal.  */
	  ourstatus->kind = TARGET_WAITKIND_SIGNALLED;
	  ourstatus->value.sig = TARGET_SIGNAL_UNKNOWN;
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

      store_waitstatus (ourstatus, status);

      return pid;
    }
}

/* Convert a Lynx process ID to a string.  Returns the string in a static
   buffer.  */

char *
lynx_pid_to_str (pid)
     int pid;
{
  static char buf[40];

  sprintf (buf, "process %d thread %d", PIDGET (pid), TIDGET (pid));

  return buf;
}

/* Extract the register values out of the core file and store
   them where `read_register' will find them.

   CORE_REG_SECT points to the register values themselves, read into memory.
   CORE_REG_SIZE is the size of that area.
   WHICH says which set of registers we are handling (0 = int, 2 = float
         on machines where they are discontiguous).
   REG_ADDR is the offset from u.u_ar0 to the register values relative to
            core_reg_sect.  This is used with old-fashioned core files to
	    locate the registers in a large upage-plus-stack ".reg" section.
	    Original upage address X is at location core_reg_sect+x+reg_addr.
 */

void
fetch_core_registers (core_reg_sect, core_reg_size, which, reg_addr)
     char *core_reg_sect;
     unsigned core_reg_size;
     int which;
     unsigned reg_addr;
{
  struct st_entry s;
  unsigned int regno;

  for (regno = 0; regno < NUM_REGS; regno++)
    supply_register (regno, core_reg_sect + offsetof (st_t, ec)
		     + regmap[regno]);
}
