/* Native-dependent code for OpenBSD/mips64.

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
#include "target.h"

#include <sys/types.h>
#include <sys/ptrace.h>
#include <machine/reg.h>

#include "inf-ptrace.h"

#define MIPS64OBSD_NUM_REGS 73

/* Supply the general-purpose registers stored in GREGS to REGCACHE.  */

static void
mips64obsd_supply_gregset (struct regcache *regcache, const void *gregs)
{
  const char *regs = gregs;
  int regnum;

  for (regnum = 0; regnum < MIPS64OBSD_NUM_REGS; regnum++)
    regcache_raw_supply (regcache, regnum, regs + regnum * 8);
}

/* Collect the general-purpose registers from REGCACHE and store them
   in GREGS.  */

static void
mips64obsd_collect_gregset (const struct regcache *regcache,
			    void *gregs, int regnum)
{
  char *regs = gregs;
  int i;

  for (i = 0; i <= MIPS64OBSD_NUM_REGS; i++)
    {
      if (regnum == -1 || regnum == i)
	regcache_raw_collect (regcache, i, regs + i * 8);
    }
}


/* Fetch register REGNUM from the inferior.  If REGNUM is -1, do this
   for all registers.  */

static void
mips64obsd_fetch_inferior_registers (int regnum)
{
  struct reg regs;

  if (ptrace (PT_GETREGS, PIDGET (inferior_ptid),
	      (PTRACE_TYPE_ARG3) &regs, 0) == -1)
    perror_with_name ("Couldn't get registers");

  mips64obsd_supply_gregset (current_regcache, &regs);
}

/* Store register REGNUM back into the inferior.  If REGNUM is -1, do
   this for all registers.  */

static void
mips64obsd_store_inferior_registers (int regnum)
{
  struct reg regs;

  if (ptrace (PT_GETREGS, PIDGET (inferior_ptid),
	      (PTRACE_TYPE_ARG3) &regs, 0) == -1)
    perror_with_name ("Couldn't get registers");

  mips64obsd_collect_gregset (current_regcache, &regs, regnum);

  if (ptrace (PT_SETREGS, PIDGET (inferior_ptid),
	      (PTRACE_TYPE_ARG3) &regs, 0) == -1)
    perror_with_name ("Couldn't write registers");
}


/* Provide a prototype to silence -Wmissing-prototypes.  */
void _initialize_mips64obsd_nat (void);

void
_initialize_mips64obsd_nat (void)
{
  struct target_ops *t;

  t = inf_ptrace_target ();
  t->to_fetch_registers = mips64obsd_fetch_inferior_registers;
  t->to_store_registers = mips64obsd_store_inferior_registers;
  add_target (t);
}
