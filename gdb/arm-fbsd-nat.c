/* Native-dependent code for FreeBSD/arm.

   Copyright (C) 2017-2018 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include "defs.h"
#include "target.h"

#include <sys/types.h>
#include <sys/ptrace.h>
#include <machine/reg.h>

#include "fbsd-nat.h"
#include "arm-tdep.h"
#include "arm-fbsd-tdep.h"
#include "inf-ptrace.h"

/* Determine if PT_GETREGS fetches REGNUM.  */

static bool
getregs_supplies (struct gdbarch *gdbarch, int regnum)
{
  return ((regnum >= ARM_A1_REGNUM && regnum <= ARM_PC_REGNUM)
	  || regnum == ARM_PS_REGNUM);
}

#ifdef PT_GETVFPREGS
/* Determine if PT_GETVFPREGS fetches REGNUM.  */

static bool
getvfpregs_supplies (struct gdbarch *gdbarch, int regnum)
{
  return ((regnum >= ARM_D0_REGNUM && regnum <= ARM_D31_REGNUM)
	  || regnum == ARM_FPSCR_REGNUM);
}
#endif

/* Fetch register REGNUM from the inferior.  If REGNUM is -1, do this
   for all registers.  */

static void
arm_fbsd_fetch_inferior_registers (struct target_ops *ops,
				    struct regcache *regcache, int regnum)
{
  pid_t pid = get_ptrace_pid (regcache_get_ptid (regcache));

  struct gdbarch *gdbarch = regcache->arch ();
  if (regnum == -1 || getregs_supplies (gdbarch, regnum))
    {
      struct reg regs;

      if (ptrace (PT_GETREGS, pid, (PTRACE_TYPE_ARG3) &regs, 0) == -1)
	perror_with_name (_("Couldn't get registers"));

      regcache->supply_regset (&arm_fbsd_gregset, regnum, &regs,
			       sizeof (regs));
    }

#ifdef PT_GETVFPREGS
  if (regnum == -1 || getvfpregs_supplies (gdbarch, regnum))
    {
      struct vfpreg vfpregs;

      if (ptrace (PT_GETVFPREGS, pid, (PTRACE_TYPE_ARG3) &vfpregs, 0) == -1)
	perror_with_name (_("Couldn't get floating point status"));

      regcache->supply_regset (&arm_fbsd_vfpregset, regnum, &vfpregs,
			       sizeof (vfpregs));
    }
#endif
}

/* Store register REGNUM back into the inferior.  If REGNUM is -1, do
   this for all registers.  */

static void
arm_fbsd_store_inferior_registers (struct target_ops *ops,
				    struct regcache *regcache, int regnum)
{
  pid_t pid = get_ptrace_pid (regcache_get_ptid (regcache));

  struct gdbarch *gdbarch = regcache->arch ();
  if (regnum == -1 || getregs_supplies (gdbarch, regnum))
    {
      struct reg regs;

      if (ptrace (PT_GETREGS, pid, (PTRACE_TYPE_ARG3) &regs, 0) == -1)
	perror_with_name (_("Couldn't get registers"));

      regcache->collect_regset (&arm_fbsd_gregset, regnum, &regs,
			       sizeof (regs));

      if (ptrace (PT_SETREGS, pid, (PTRACE_TYPE_ARG3) &regs, 0) == -1)
	perror_with_name (_("Couldn't write registers"));
    }

#ifdef PT_GETVFPREGS
  if (regnum == -1 || getvfpregs_supplies (gdbarch, regnum))
    {
      struct vfpreg vfpregs;

      if (ptrace (PT_GETVFPREGS, pid, (PTRACE_TYPE_ARG3) &vfpregs, 0) == -1)
	perror_with_name (_("Couldn't get floating point status"));

      regcache->collect_regset (&arm_fbsd_vfpregset, regnum, &vfpregs,
				sizeof (vfpregs));

      if (ptrace (PT_SETVFPREGS, pid, (PTRACE_TYPE_ARG3) &vfpregs, 0) == -1)
	perror_with_name (_("Couldn't write floating point status"));
    }
#endif
}

/* Implement the to_read_description method.  */

static const struct target_desc *
arm_fbsd_read_description (struct target_ops *ops)
{
  const struct target_desc *desc;

  desc = arm_fbsd_read_description_auxv (ops);
  if (desc == NULL)
    desc = ops->beneath->to_read_description (ops->beneath);
  return desc;
}

void
_initialize_arm_fbsd_nat (void)
{
  struct target_ops *t;

  t = inf_ptrace_target ();
  t->to_fetch_registers = arm_fbsd_fetch_inferior_registers;
  t->to_store_registers = arm_fbsd_store_inferior_registers;
  t->to_read_description = arm_fbsd_read_description;
  fbsd_nat_add_target (t);
}
