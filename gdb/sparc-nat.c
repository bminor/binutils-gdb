/* Native-dependent code for SPARC.

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

/* We need a data structure for use with ptrace(2).  SunOS 4 has
   `struct regs' and `struct fp_status' in <machine/reg.h>.  BSD's
   have `struct reg' and `struct fpreg' in <machine/reg.h>.  GNU/Linux
   has the same structures as SunOS 4, but they're in <asm/reg.h>,
   which is a kernel header.  As a general rule we avoid including
   GNU/Linux kernel headers.  Fortunately GNU/Linux has a `gregset_t'
   and a `fpregset_t' that are equivalent to `struct regs' and `struct
   fp_status' in <sys/ucontext.h>, which is automatically included by
   <signal.h>.  Settling on using the `gregset_t' and `fpregset_t'
   typedefs, providing them for the other systems, therefore solves
   the puzzle.  */

#include <signal.h>
#include <sys/ptrace.h>
#include "gdb_wait.h"
#ifdef HAVE_MACHINE_REG_H
#include <machine/reg.h>
#ifdef HAVE_STRUCT_REG
typedef struct reg gregset_t;
typedef struct fpreg fpregset_t;
#else 
typedef struct regs gregset_t;
typedef struct fp_status fpregset_t;
#endif
#endif

#include "sparc-tdep.h"

/* Register set description.  */
const struct sparc_gregset *sparc_gregset;

/* Determine whether `gregset_t' contains register REGNUM.  */

int
sparc_gregset_supplies_p (int regnum)
{
  /* Integer registers.  */
  if ((regnum >= SPARC_G1_REGNUM && regnum <= SPARC_G7_REGNUM)
      || (regnum >= SPARC_O0_REGNUM && regnum <= SPARC_O7_REGNUM)
      || (regnum >= SPARC_L0_REGNUM && regnum <= SPARC_L7_REGNUM)
      || (regnum >= SPARC_I0_REGNUM && regnum <= SPARC_I7_REGNUM))
    return 1;

  /* Control registers.  */
  if (regnum == SPARC32_PC_REGNUM
      || regnum == SPARC32_NPC_REGNUM
      || regnum == SPARC32_PSR_REGNUM
      || regnum == SPARC32_Y_REGNUM)
    return 1;

  return 0;
}

/* Determine whether `fpregset_t' contains register REGNUM.  */

static int
sparc_fpregset_supplies_p (int regnum)
{
  /* Floating-point registers.  */
  if (regnum >= SPARC_F0_REGNUM && regnum <= SPARC_F31_REGNUM)
    return 1;

  /* Control registers.  */
  if (regnum == SPARC32_FSR_REGNUM)
    return 1;

  return 0;
}

/* Fetch register REGNUM from the inferior.  If REGNUM is -1, do this
   for all registers (including the floating-point registers).  */

void
fetch_inferior_registers (int regnum)
{
  struct regcache *regcache = current_regcache;

  if (regnum == SPARC_G0_REGNUM)
    {
      regcache_raw_supply (regcache, SPARC_G0_REGNUM, NULL);
      return;
    }

  if (regnum == -1 || sparc_gregset_supplies_p (regnum))
    {
      gregset_t regs;

      if (ptrace (PTRACE_GETREGS, PIDGET (inferior_ptid),
		  (PTRACE_ARG3_TYPE) &regs, 0) == -1)
	perror_with_name ("Couldn't get registers");

      sparc_supply_gregset (sparc_gregset, regcache, -1, &regs);
      if (regnum != -1)
	return;
    }

  if (regnum == -1 || sparc_fpregset_supplies_p (regnum))
    {
      fpregset_t fpregs;

      if (ptrace (PTRACE_GETFPREGS, PIDGET (inferior_ptid),
		  (PTRACE_ARG3_TYPE) &fpregs, 0) == -1)
	perror_with_name ("Couldn't get floating point status");

      sparc_supply_fpregset (regcache, -1, &fpregs);
    }
}

void
store_inferior_registers (int regnum)
{
  struct regcache *regcache = current_regcache;

  if (regnum == -1 || sparc_gregset_supplies_p (regnum))
    {
      gregset_t regs;

      if (ptrace (PTRACE_GETREGS, PIDGET (inferior_ptid),
		  (PTRACE_ARG3_TYPE) &regs, 0) == -1)
	perror_with_name ("Couldn't get registers");

      sparc_collect_gregset (sparc_gregset, regcache, regnum, &regs);

      if (ptrace (PTRACE_SETREGS, PIDGET (inferior_ptid),
		  (PTRACE_ARG3_TYPE) &regs, 0) == -1)
	perror_with_name ("Couldn't write registers");

      /* Deal with the stack regs.  */
      if (regnum == -1 || regnum == SPARC_SP_REGNUM
	  || (regnum >= SPARC_L0_REGNUM && regnum <= SPARC_I7_REGNUM))
	{
	  ULONGEST sp;

	  regcache_cooked_read_unsigned (regcache, SPARC_SP_REGNUM, &sp);
	  sparc_collect_rwindow (regcache, sp, regnum);
	}

      if (regnum != -1)
	return;
    }

  if (regnum == -1 || sparc_fpregset_supplies_p (regnum))
    {
      fpregset_t fpregs;

      if (ptrace (PTRACE_GETFPREGS, PIDGET (inferior_ptid),
		  (PTRACE_ARG3_TYPE) &fpregs, 0) == -1)
	perror_with_name ("Couldn't get floating-point registers");

      sparc_collect_fpregset (regcache, regnum, &fpregs);

      if (ptrace (PTRACE_SETFPREGS, PIDGET (inferior_ptid),
		  (PTRACE_ARG3_TYPE) &fpregs, 0) == -1)
	perror_with_name ("Couldn't write floating-point registers");

      if (regnum != -1)
	return;
    }
}


/* Provide a prototype to silence -Wmissing-prototypes.  */
void _initialize_sparc_nat (void);

void
_initialize_sparc_nat (void)
{
  /* Deafult to using SunOS 4 register sets.  */
  if (sparc_gregset == NULL)
    sparc_gregset = &sparc32_sunos4_gregset;
}
