/* Native-dependent code for Motorola 68000 BSD's.

   Copyright 2004 Free Software Foundation, Inc.

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
#include <sys/types.h>
#include <sys/ptrace.h>
#include <machine/reg.h>

#include "m68k-tdep.h"

static int
m68kbsd_gregset_supplies_p (int regnum)
{
  return (regnum >= M68K_D0_REGNUM && regnum <= M68K_PC_REGNUM);
}

static int
m68kbsd_fpregset_supplies_p (int regnum)
{
  return (regnum >= M68K_FP0_REGNUM && regnum <= M68K_FPI_REGNUM);
}

/* Supply the general-purpose registers stored in GREGS to REGCACHE.  */

static void
m68kbsd_supply_gregset (struct regcache *regcache, const void *gregs)
{
  const char *regs = gregs;
  int regnum;

  for (regnum = M68K_D0_REGNUM; regnum <= M68K_PC_REGNUM; regnum++)
    regcache_raw_supply (regcache, regnum, regs + regnum * 4);
}

/* Supply the floating-point registers stored in FPREGS to REGCACHE.  */

static void
m68kbsd_supply_fpregset (struct regcache *regcache, const void *fpregs)
{
  const char *regs = fpregs;
  int regnum;

  for (regnum = M68K_FP0_REGNUM; regnum <= M68K_FPI_REGNUM; regnum++)
    regcache_raw_supply (regcache, regnum,
			 regs + m68kbsd_fpreg_offset (regnum));
}

/* Collect the general-purpose registers from REGCACHE and store them
   in GREGS.  */

static void
m68kbsd_collect_gregset (const struct regcache *regcache,
			 void *gregs, int regnum)
{
  char *regs = gregs;
  int i;

  for (i = M68K_D0_REGNUM; i <= M68K_PC_REGNUM; i++)
    {
      if (regnum == -1 || regnum == i)
	regcache_raw_collect (regcache, i, regs + i * 4);
    }
}

/* Collect the floating-point registers from REGCACHE and store them
   in FPREGS.  */

static void
m68kbsd_collect_fpregset (struct regcache *regcache,
			  void *fpregs, int regnum)
{
  char *regs = fpregs;
  int i;

  for (i = M68K_FP0_REGNUM; i <= M68K_FPI_REGNUM; i++)
    {
      if (regnum == -1 || regnum == i)
	regcache_raw_collect (regcache, i, regs + m68kbsd_fpreg_offset (i));
    }
}


/* Fetch register REGNUM from the inferior.  If REGNUM is -1, do this
   for all registers (including the floating-point registers).  */

void
fetch_inferior_registers (int regnum)
{
  if (regnum == -1 || m68kbsd_gregset_supplies_p (regnum))
    {
      struct reg regs;

      if (ptrace (PT_GETREGS, PIDGET (inferior_ptid),
		  (PTRACE_ARG3_TYPE) &regs, 0) == -1)
	perror_with_name ("Couldn't get registers");

      m68kbsd_supply_gregset (current_regcache, &regs);
    }

  if (regnum == -1 || m68kbsd_fpregset_supplies_p (regnum))
    {
      struct fpreg fpregs;

      if (ptrace (PT_GETFPREGS, PIDGET (inferior_ptid),
		  (PTRACE_ARG3_TYPE) &fpregs, 0) == -1)
	perror_with_name ("Couldn't get floating point status");

      m68kbsd_supply_fpregset (current_regcache, &fpregs);
    }
}

/* Store register REGNUM back into the inferior.  If REGNUM is -1, do
   this for all registers (including the floating-point registers).  */

void
store_inferior_registers (int regnum)
{
  if (regnum == -1 || m68kbsd_gregset_supplies_p (regnum))
    {
      struct reg regs;

      if (ptrace (PT_GETREGS, PIDGET (inferior_ptid),
                  (PTRACE_ARG3_TYPE) &regs, 0) == -1)
        perror_with_name ("Couldn't get registers");

      m68kbsd_collect_gregset (current_regcache, &regs, regnum);

      if (ptrace (PT_SETREGS, PIDGET (inferior_ptid),
	          (PTRACE_ARG3_TYPE) &regs, 0) == -1)
        perror_with_name ("Couldn't write registers");
    }

  if (regnum == -1 || m68kbsd_fpregset_supplies_p (regnum))
    {
      struct fpreg fpregs;

      if (ptrace (PT_GETFPREGS, PIDGET (inferior_ptid),
		  (PTRACE_ARG3_TYPE) &fpregs, 0) == -1)
	perror_with_name ("Couldn't get floating point status");

      m68kbsd_collect_fpregset (current_regcache, &fpregs, regnum);

      if (ptrace (PT_SETFPREGS, PIDGET (inferior_ptid),
		  (PTRACE_ARG3_TYPE) &fpregs, 0) == -1)
	perror_with_name ("Couldn't write floating point status");
    }
}
