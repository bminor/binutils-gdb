/* Native-dependent code for FreeBSD/amd64.
   Copyright 2003 Free Software Foundation, Inc.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include "defs.h"
#include "inferior.h"
#include "regcache.h"

#include "gdb_assert.h"
#include <signal.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/ptrace.h>
#include <sys/sysctl.h>
#include <machine/reg.h>

#ifdef HAVE_SYS_PROCFS_H
#include <sys/procfs.h>
#endif

#ifndef HAVE_GREGSET_T
typedef struct reg gregset_t;
#endif

#ifndef HAVE_FPREGSET_T
typedef struct fpreg fpregset_t;
#endif

#include "gregset.h"
#include "x86-64-tdep.h"


/* Offset to the gregset_t location where REG is stored.  */
#define REG_OFFSET(reg) offsetof (gregset_t, reg)

/* At reg_offset[REGNO] you'll find the offset to the gregset_t
   location where the GDB register REGNO is stored.  Unsupported
   registers are marked with `-1'.  */
static int reg_offset[] =
{
  REG_OFFSET (r_rax),
  REG_OFFSET (r_rbx),
  REG_OFFSET (r_rcx),
  REG_OFFSET (r_rdx),
  REG_OFFSET (r_rsi),
  REG_OFFSET (r_rdi),
  REG_OFFSET (r_rbp),
  REG_OFFSET (r_rsp),
  REG_OFFSET (r_r8),
  REG_OFFSET (r_r9),
  REG_OFFSET (r_r10),
  REG_OFFSET (r_r11),
  REG_OFFSET (r_r12),
  REG_OFFSET (r_r13),
  REG_OFFSET (r_r14),
  REG_OFFSET (r_r15),
  REG_OFFSET (r_rip),
  REG_OFFSET (r_rflags),
  -1,
  -1,
  -1,
  -1
};

#define REG_ADDR(regset, regno) ((char *) (regset) + reg_offset[regno])

/* Macro to determine if a register is fetched with PT_GETREGS.  */
#define GETREGS_SUPPLIES(regno) \
  ((0 <= (regno) && (regno) < X86_64_NUM_GREGS))


/* Transfering the registers between GDB, inferiors and core files.  */

/* Fill GDB's register array with the general-purpose register values
   in *GREGSETP.  */

void
supply_gregset (gregset_t *gregsetp)
{
  int i;

  for (i = 0; i < X86_64_NUM_GREGS; i++)
    {
      if (reg_offset[i] == -1)
	supply_register (i, NULL);
      else
	supply_register (i, REG_ADDR (gregsetp, i));
    }
}

/* Fill register REGNO (if it is a general-purpose register) in
   *GREGSETPS with the value in GDB's register array.  If REGNO is -1,
   do this for all registers.  */

void
fill_gregset (gregset_t *gregsetp, int regno)
{
  int i;

  for (i = 0; i < X86_64_NUM_GREGS; i++)
    if ((regno == -1 || regno == i) && reg_offset[i] != -1)
      regcache_collect (i, REG_ADDR (gregsetp, i));
}

/* Fill GDB's register array with the floating-point register values
   in *FPREGSETP.  */

void
supply_fpregset (fpregset_t *fpregsetp)
{
  x86_64_supply_fxsave ((char *) fpregsetp);
}

/* Fill register REGNO (if it is a floating-point register) in
   *FPREGSETP with the value in GDB's register array.  If REGNO is -1,
   do this for all registers.  */

void
fill_fpregset (fpregset_t *fpregsetp, int regno)
{
  x86_64_fill_fxsave ((char *) fpregsetp, regno);
}

/* Fetch register REGNO from the inferior.  If REGNO is -1, do this
   for all registers (including the floating point registers).  */

void
fetch_inferior_registers (int regno)
{
  if (regno == -1 || GETREGS_SUPPLIES (regno))
    {
      gregset_t gregs;

      if (ptrace (PT_GETREGS, PIDGET (inferior_ptid),
		  (PTRACE_ARG3_TYPE) &gregs, 0) == -1)
	perror_with_name ("Couldn't get registers");

      supply_gregset (&gregs);
      if (regno != -1)
	return;
    }

  if (regno == -1 || regno >= FP0_REGNUM)
    {
      fpregset_t fpregs;

      if (ptrace (PT_GETFPREGS, PIDGET (inferior_ptid),
		  (PTRACE_ARG3_TYPE) &fpregs, 0) == -1)
	perror_with_name ("Couldn't get floating point status");

      supply_fpregset (&fpregs);
    }
}

/* Store register REGNO back into the inferior.  If REGNO is -1, do
   this for all registers (including the floating point registers).  */

void
store_inferior_registers (int regno)
{
  if (regno == -1 || GETREGS_SUPPLIES (regno))
    {
      gregset_t gregs;

      if (ptrace (PT_GETREGS, PIDGET (inferior_ptid),
                  (PTRACE_ARG3_TYPE) &gregs, 0) == -1)
        perror_with_name ("Couldn't get registers");

      fill_gregset (&gregs, regno);

      if (ptrace (PT_SETREGS, PIDGET (inferior_ptid),
	          (PTRACE_ARG3_TYPE) &gregs, 0) == -1)
        perror_with_name ("Couldn't write registers");

      if (regno != -1)
	return;
    }

  if (regno == -1 || regno >= FP0_REGNUM)
    {
      fpregset_t fpregs;

      if (ptrace (PT_GETFPREGS, PIDGET (inferior_ptid),
		  (PTRACE_ARG3_TYPE) &fpregs, 0) == -1)
	perror_with_name ("Couldn't get floating point status");

      fill_fpregset (&fpregs, regno);
  
      if (ptrace (PT_SETFPREGS, PIDGET (inferior_ptid),
		  (PTRACE_ARG3_TYPE) &fpregs, 0) == -1)
	perror_with_name ("Couldn't write floating point status");
    }
}


/* Provide a prototype to silence -Wmissing-prototypes.  */
void _initialize_amd64fbsd_nat (void);

void
_initialize_am64fbsd_nat (void)
{
  int offset;

  /* To support the recognition of signal handlers, i386bsd-tdep.c
     hardcodes some constants.  Inclusion of this file means that we
     are compiling a native debugger, which means that we can use the
     system header files and sysctl(3) to get at the relevant
     information.  */

  extern int amd64fbsd_sc_reg_offset[];
#define SC_REG_OFFSET amd64fbsd_sc_reg_offset

  /* We only check the program counter, stack pointer and frame
     pointer since these members of `struct sigcontext' are essential
     for providing backtraces.  */

#define SC_RIP_OFFSET SC_REG_OFFSET[X86_64_RIP_REGNUM]
#define SC_RSP_OFFSET SC_REG_OFFSET[X86_64_RSP_REGNUM]
#define SC_RBP_OFFSET SC_REG_OFFSET[X86_64_RBP_REGNUM]

  /* Override the default value for the offset of the program counter
     in the sigcontext structure.  */
  offset = offsetof (struct sigcontext, sc_rip);

  if (SC_RIP_OFFSET != offset)
    {
      warning ("\
offsetof (struct sigcontext, sc_rip) yields %d instead of %d.\n\
Please report this to <bug-gdb@gnu.org>.", 
	       offset, SC_RIP_OFFSET);
    }

  SC_RIP_OFFSET = offset;

  /* Likewise for the stack pointer.  */
  offset = offsetof (struct sigcontext, sc_rsp);

  if (SC_RSP_OFFSET != offset)
    {
      warning ("\
offsetof (struct sigcontext, sc_rsp) yields %d instead of %d.\n\
Please report this to <bug-gdb@gnu.org>.",
	       offset, SC_RSP_OFFSET);
    }

  SC_RSP_OFFSET = offset;

  /* And the frame pointer.  */
  offset = offsetof (struct sigcontext, sc_rbp);

  if (SC_RBP_OFFSET != offset)
    {
      warning ("\
offsetof (struct sigcontext, sc_rbp) yields %d instead of %d.\n\
Please report this to <bug-gdb@gnu.org>.",
	       offset, SC_RBP_OFFSET);
    }

  SC_RBP_OFFSET = offset;

  /* FreeBSD provides a kern.ps_strings sysctl that we can use to
     locate the sigtramp.  That way we can still recognize a sigtramp
     if its location is changed in a new kernel.  Of course this is
     still based on the assumption that the sigtramp is placed
     directly under the location where the program arguments and
     environment can be found.  */
  {
    int mib[2];
    int ps_strings;
    size_t len;

    extern CORE_ADDR amd64fbsd_sigtramp_start;
    extern CORE_ADDR amd64fbsd_sigtramp_end;

    mib[0] = CTL_KERN;
    mib[1] = KERN_PS_STRINGS;
    len = sizeof (ps_strings);
    if (sysctl (mib, 2, &ps_strings, &len, NULL, 0) == 0)
      {
	amd64fbsd_sigtramp_start = ps_strings - 32;
	amd64fbsd_sigtramp_end = ps_strings;
      }
  }
}
