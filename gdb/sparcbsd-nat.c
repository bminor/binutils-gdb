/* Native-dependent code for SPARC BSD's.

   Copyright 2002, 2003 Free Software Foundation, Inc.
   Based on code contributed by Wasabi Systems, Inc.

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

/* FIXME: Should be changed to sparc-tdep.h when the old code is gone.  */
#include "sparc64-tdep.h"
#include "sparcbsd-nat.h"

#include <sys/types.h>
#include <sys/ptrace.h>
#include <machine/reg.h>

/* Functions translating between `struct reg' and `struct fpreg' and
   GDB's register cache.  */
void (*sparcbsd_supply_reg)(const char *, int);
void (*sparcbsd_fill_reg)(char *, int);
void (*sparcbsd_supply_fpreg)(const char *, int);
void (*sparcbsd_fill_fpreg)(char *, int);

/* Functions indication whether `struct reg' or `struct fpreg' provides
   a certain register.  */
int (*sparcbsd_reg_supplies_p)(int);
int (*sparcbsd_fpreg_supplies_p)(int);

void
fetch_inferior_registers (int regnum)
{
  if (regnum == -1 || sparcbsd_reg_supplies_p (regnum))
    {
      struct reg regs;

      if (ptrace (PT_GETREGS, PIDGET (inferior_ptid),
		  (PTRACE_ARG3_TYPE) &regs, 0) == -1)
        perror_with_name ("Couldn't get registers");

      sparcbsd_supply_reg ((char *) &regs, regnum);
      if (regnum != -1)
	return;
    }

  if (regnum == -1 || sparcbsd_fpreg_supplies_p (regnum))
    {
      struct fpreg fpregs;

      if (ptrace (PT_GETFPREGS, PIDGET (inferior_ptid),
		  (PTRACE_ARG3_TYPE) &fpregs, 0) == -1)
        perror_with_name ("Couldn't get floating-point registers");

      sparcbsd_supply_fpreg ((char *) &fpregs, regnum);
      if (regnum != -1)
	return;
    }
}

void
store_inferior_registers (int regnum)
{
  if (regnum == -1 || sparcbsd_reg_supplies_p (regnum))
    {
      struct reg regs;

      if (ptrace (PT_GETREGS, PIDGET (inferior_ptid),
		  (PTRACE_ARG3_TYPE) &regs, 0) == -1)
	perror_with_name ("Couldn't get registers");

      sparcbsd_fill_reg ((char *) &regs, regnum);

      if (ptrace (PT_SETREGS, PIDGET (inferior_ptid),
		  (PTRACE_ARG3_TYPE) &regs, 0) == -1)
	perror_with_name ("Couldn't write registers");

      /* Deal with the stack regs.  */
      if (regnum == -1 || regnum == SPARC_SP_REGNUM
	  || (regnum >= SPARC_L0_REGNUM && regnum <= SPARC_I7_REGNUM))
	{
	  ULONGEST sp;

	  regcache_cooked_read_unsigned (current_regcache,
					 SPARC_SP_REGNUM, &sp);
	  sparc_fill_rwindow (sp, regnum);
	}

      if (regnum != -1)
	return;
    }

  if (regnum == -1 || sparcbsd_fpreg_supplies_p (regnum))
    {
      struct fpreg fpregs;

      if (ptrace (PT_GETFPREGS, PIDGET (inferior_ptid),
		  (PTRACE_ARG3_TYPE) &fpregs, 0) == -1)
	perror_with_name ("Couldn't get floating-point registers");

      sparcbsd_fill_fpreg ((char *) &fpregs, regnum);

      if (ptrace (PT_SETFPREGS, PIDGET (inferior_ptid),
		  (PTRACE_ARG3_TYPE) &fpregs, 0) == -1)
	perror_with_name ("Couldn't write floating-point registers");

      if (regnum != -1)
	return;
    }
}
