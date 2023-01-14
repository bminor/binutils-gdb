/* Native-dependent code for FreeBSD/aarch64.

   Copyright (C) 2017-2022 Free Software Foundation, Inc.

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
#include "regcache.h"
#include "target.h"

#include <sys/types.h>
#include <sys/ptrace.h>
#include <machine/reg.h>

#include "fbsd-nat.h"
#include "aarch64-fbsd-tdep.h"
#include "inf-ptrace.h"

struct aarch64_fbsd_nat_target final : public fbsd_nat_target
{
  void fetch_registers (struct regcache *, int) override;
  void store_registers (struct regcache *, int) override;
};

static aarch64_fbsd_nat_target the_aarch64_fbsd_nat_target;

/* Fetch register REGNUM from the inferior.  If REGNUM is -1, do this
   for all registers.  */

void
aarch64_fbsd_nat_target::fetch_registers (struct regcache *regcache,
					  int regnum)
{
  fetch_register_set<struct reg> (regcache, regnum, PT_GETREGS,
				  &aarch64_fbsd_gregset);
  fetch_register_set<struct fpreg> (regcache, regnum, PT_GETFPREGS,
				    &aarch64_fbsd_fpregset);
}

/* Store register REGNUM back into the inferior.  If REGNUM is -1, do
   this for all registers.  */

void
aarch64_fbsd_nat_target::store_registers (struct regcache *regcache,
					  int regnum)
{
  store_register_set<struct reg> (regcache, regnum, PT_GETREGS, PT_SETREGS,
				  &aarch64_fbsd_gregset);
  store_register_set<struct fpreg> (regcache, regnum, PT_GETFPREGS,
				    PT_SETFPREGS, &aarch64_fbsd_fpregset);
}

void _initialize_aarch64_fbsd_nat ();
void
_initialize_aarch64_fbsd_nat ()
{
  add_inf_child_target (&the_aarch64_fbsd_nat_target);
}
