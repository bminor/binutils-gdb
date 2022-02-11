/* Native-dependent code for GNU/Linux on LoongArch processors.

   Copyright (C) 2022 Free Software Foundation, Inc.
   Contributed by Loongson Ltd.

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
#include "elf/common.h"
#include "gregset.h"
#include "inferior.h"
#include "linux-nat-trad.h"
#include "loongarch-tdep.h"
#include "nat/gdb_ptrace.h"
#include "target-descriptions.h"

#include <asm/ptrace.h>

/* LoongArch Linux native additions to the default Linux support.  */

class loongarch_linux_nat_target final : public linux_nat_trad_target
{
public:
  /* Add our register access methods.  */
  void fetch_registers (struct regcache *, int) override;
  void store_registers (struct regcache *, int) override;

protected:
  /* Override linux_nat_trad_target methods.  */
  CORE_ADDR register_u_offset (struct gdbarch *gdbarch, int regno,
                               int store_p) override;
};

/* Fill GDB's register array with the general-purpose, pc and badv
   register values from the current thread.  */

static void
fetch_gregs_from_thread (struct regcache *regcache, int regno, pid_t tid)
{
  loongarch_gdbarch_tdep *tdep
    = (loongarch_gdbarch_tdep *) gdbarch_tdep (regcache->arch ());
  auto regs = tdep->regs;
  elf_gregset_t regset;

  if (regno == -1 || (regs.r <= regno && regno < regs.r + 32)
      || regs.pc == regno || regs.badv == regno)
  {
    struct iovec iov;

    iov.iov_base = &regset;
    iov.iov_len = sizeof (regset);

    if (ptrace (PTRACE_GETREGSET, tid, NT_PRSTATUS, (long) &iov) < 0)
      perror_with_name (_("Couldn't get NT_PRSTATUS registers"));
    else
      loongarch_gregset.supply_regset (nullptr, regcache, regno,
				       &regset, sizeof (regset));
  }
}

/* Store to the current thread the valid general-purpose, pc and badv
   register values in the GDB's register array.  */

static void
store_gregs_to_thread (struct regcache *regcache, int regno, pid_t tid)
{
  loongarch_gdbarch_tdep *tdep
    = (loongarch_gdbarch_tdep *) gdbarch_tdep (regcache->arch ());
  auto regs = tdep->regs;
  elf_gregset_t regset;

  if (regno == -1 || (regs.r <= regno && regno < regs.r + 32)
      || regs.pc == regno || regs.badv == regno)
  {
    struct iovec iov;

    iov.iov_base = &regset;
    iov.iov_len = sizeof (regset);

    if (ptrace (PTRACE_GETREGSET, tid, NT_PRSTATUS, (long) &iov) < 0)
      perror_with_name (_("Couldn't get NT_PRSTATUS registers"));
    else
      {
	loongarch_gregset.collect_regset (nullptr, regcache, regno,
					  &regset, sizeof (regset));
	if (ptrace (PTRACE_SETREGSET, tid, NT_PRSTATUS, (long) &iov) < 0)
	  perror_with_name (_("Couldn't set NT_PRSTATUS registers"));
      }
  }
}

/* Implement the "fetch_registers" target_ops method.  */

void
loongarch_linux_nat_target::fetch_registers (struct regcache *regcache,
					     int regno)
{
  pid_t tid = get_ptrace_pid (regcache->ptid ());

  fetch_gregs_from_thread(regcache, regno, tid);
}

/* Implement the "store_registers" target_ops method.  */

void
loongarch_linux_nat_target::store_registers (struct regcache *regcache,
					     int regno)
{
  pid_t tid = get_ptrace_pid (regcache->ptid ());

  store_gregs_to_thread (regcache, regno, tid);
}

/* Return the address in the core dump or inferior of register REGNO.  */

CORE_ADDR
loongarch_linux_nat_target::register_u_offset (struct gdbarch *gdbarch,
					       int regno, int store_p)
{
  loongarch_gdbarch_tdep *tdep
    = (loongarch_gdbarch_tdep *) gdbarch_tdep (gdbarch);
  auto regs = tdep->regs;

  /* According to <asm/ptrace.h> */
  if (0 <= regs.r && regs.r <= regno && regno < regs.r + GPR_NUM)
    return GPR_BASE + regno - regs.r;
  else if (regs.pc == regno)
    return PC;
  else
    return -1;
}

static loongarch_linux_nat_target the_loongarch_linux_nat_target;

/* Wrapper functions.  These are only used by libthread_db.  */

void
supply_gregset (struct regcache *regcache, const gdb_gregset_t *gregset)
{
  loongarch_gregset.supply_regset (nullptr, regcache, -1, gregset,
				   sizeof (gdb_gregset_t));
}

void
fill_gregset (const struct regcache *regcache, gdb_gregset_t *gregset,
	      int regno)
{
  loongarch_gregset.collect_regset (nullptr, regcache, regno, gregset,
				    sizeof (gdb_gregset_t));
}

void
supply_fpregset (struct regcache *regcache, const gdb_fpregset_t *fpregset)
{
}

void
fill_fpregset (const struct regcache *regcache, gdb_fpregset_t *fpregset,
	       int regno)
{
}

/* Initialize LoongArch Linux native support.  */

void _initialize_loongarch_linux_nat ();
void
_initialize_loongarch_linux_nat ()
{
  linux_target = &the_loongarch_linux_nat_target;
  add_inf_child_target (&the_loongarch_linux_nat_target);
}
