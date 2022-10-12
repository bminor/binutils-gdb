/* Native-dependent code for FreeBSD/riscv.

   Copyright (C) 2018-2022 Free Software Foundation, Inc.

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
#include "inferior.h"
#include "target.h"

#include <sys/types.h>
#include <sys/ptrace.h>
#include <machine/reg.h>

#include "fbsd-nat.h"
#include "riscv-tdep.h"
#include "riscv-fbsd-tdep.h"
#include "inf-ptrace.h"

struct riscv_fbsd_nat_target final : public fbsd_nat_target
{
  void fetch_registers (struct regcache *, int) override;
  void store_registers (struct regcache *, int) override;
#if __has_feature(capabilities)
  const struct target_desc *read_description () override;
#endif
};

static riscv_fbsd_nat_target the_riscv_fbsd_nat_target;

/* Fetch register REGNUM from the inferior.  If REGNUM is -1, do this
   for all registers.  */

void
riscv_fbsd_nat_target::fetch_registers (struct regcache *regcache,
					int regnum)
{
  if (regnum == -1 || regnum == RISCV_ZERO_REGNUM)
    regcache->raw_supply_zeroed (RISCV_ZERO_REGNUM);
  fetch_register_set<struct reg> (regcache, regnum, PT_GETREGS,
				  &riscv_fbsd_gregset);
  fetch_register_set<struct fpreg> (regcache, regnum, PT_GETFPREGS,
				    &riscv_fbsd_fpregset);
#if __has_feature(capabilities)
  if (riscv_isa_clen (regcache->arch ()) != 0)
    fetch_register_set<struct capreg> (regcache, regnum, PT_GETCAPREGS,
				       &riscv_fbsd_capregset);
#endif
}

/* Store register REGNUM back into the inferior.  If REGNUM is -1, do
   this for all registers.  */

void
riscv_fbsd_nat_target::store_registers (struct regcache *regcache,
					int regnum)
{
  store_register_set<struct reg> (regcache, regnum, PT_GETREGS, PT_SETREGS,
				  &riscv_fbsd_gregset);
  store_register_set<struct fpreg> (regcache, regnum, PT_GETFPREGS,
				    PT_SETFPREGS, &riscv_fbsd_fpregset);
#ifdef notyet
  if (riscv_isa_clen (regcache->arch ()) != 0)
    store_register_set<struct capreg> (regcache, regnum, PT_GETCAPREGS,
				       PT_SETCAPREGS, &riscv_fbsd_capregset);
#endif
}

#if __has_feature(capabilities)
/* Implement the read_description method.  */

const struct target_desc *
riscv_fbsd_nat_target::read_description ()
{
  struct riscv_gdbarch_features features;
  struct reg *reg;

  features.xlen = sizeof (reg->ra);
  features.flen = sizeof (uint64_t);
  if (have_register_set<struct capreg> (inferior_ptid, PT_GETCAPREGS))
    features.clen = sizeof (uintcap_t);

  return riscv_lookup_target_description (features);
}
#endif

void _initialize_riscv_fbsd_nat ();
void
_initialize_riscv_fbsd_nat ()
{
  add_inf_child_target (&the_riscv_fbsd_nat_target);
}
