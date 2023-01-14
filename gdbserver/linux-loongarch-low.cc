/* GNU/Linux/LoongArch specific low level interface, for the remote server
   for GDB.
   Copyright (C) 2022 Free Software Foundation, Inc.

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

#include "server.h"
#include "linux-low.h"
#include "tdesc.h"
#include "elf/common.h"
#include "arch/loongarch.h"

/* Linux target ops definitions for the LoongArch architecture.  */

class loongarch_target : public linux_process_target
{
public:

  const regs_info *get_regs_info () override;

  int breakpoint_kind_from_pc (CORE_ADDR *pcptr) override;

  const gdb_byte *sw_breakpoint_from_kind (int kind, int *size) override;

protected:

  void low_arch_setup () override;

  bool low_cannot_fetch_register (int regno) override;

  bool low_cannot_store_register (int regno) override;

  bool low_fetch_register (regcache *regcache, int regno) override;

  bool low_supports_breakpoints () override;

  CORE_ADDR low_get_pc (regcache *regcache) override;

  void low_set_pc (regcache *regcache, CORE_ADDR newpc) override;

  bool low_breakpoint_at (CORE_ADDR pc) override;
};

/* The singleton target ops object.  */

static loongarch_target the_loongarch_target;

bool
loongarch_target::low_cannot_fetch_register (int regno)
{
  gdb_assert_not_reached ("linux target op low_cannot_fetch_register "
			  "is not implemented by the target");
}

bool
loongarch_target::low_cannot_store_register (int regno)
{
  gdb_assert_not_reached ("linux target op low_cannot_store_register "
			  "is not implemented by the target");
}

/* Implementation of linux target ops method "low_arch_setup".  */

void
loongarch_target::low_arch_setup ()
{
  static const char *expedite_regs[] = { "r3", "pc", NULL };
  loongarch_gdbarch_features features;
  target_desc_up tdesc;

  features.xlen = sizeof (elf_greg_t);
  tdesc = loongarch_create_target_description (features);

  if (!tdesc->expedite_regs)
    init_target_desc (tdesc.get (), expedite_regs);
  current_process ()->tdesc = tdesc.release ();
}

/* Collect GPRs from REGCACHE into BUF.  */

static void
loongarch_fill_gregset (struct regcache *regcache, void *buf)
{
  const struct target_desc *tdesc = regcache->tdesc;
  elf_gregset_t *regset = (elf_gregset_t *) buf;
  int regno = find_regno (tdesc, "r0");
  int i;

  for (i = 1; i < 32; i++)
    collect_register (regcache, regno + i, *regset + i);
  collect_register_by_name (regcache, "pc", *regset + 32);
  collect_register_by_name (regcache, "badv", *regset + 33);
}

/* Supply GPRs from BUF into REGCACHE.  */

static void
loongarch_store_gregset (struct regcache *regcache, const void *buf)
{
  const struct target_desc *tdesc = regcache->tdesc;
  const elf_gregset_t *regset = (const elf_gregset_t *) buf;
  int regno = find_regno (tdesc, "r0");
  int i;

  supply_register_zeroed (regcache, regno);
  for (i = 1; i < 32; i++)
    supply_register (regcache, regno + i, *regset + i);
  supply_register_by_name (regcache, "pc", *regset + 32);
  supply_register_by_name (regcache, "badv", *regset + 33);
}

/* LoongArch/Linux regsets.  */
static struct regset_info loongarch_regsets[] = {
  { PTRACE_GETREGSET, PTRACE_SETREGSET, NT_PRSTATUS, sizeof (elf_gregset_t),
    GENERAL_REGS, loongarch_fill_gregset, loongarch_store_gregset },
  NULL_REGSET
};

/* LoongArch/Linux regset information.  */
static struct regsets_info loongarch_regsets_info =
  {
    loongarch_regsets, /* regsets */
    0, /* num_regsets */
    NULL, /* disabled_regsets */
  };

/* Definition of linux_target_ops data member "regs_info".  */
static struct regs_info loongarch_regs =
  {
    NULL, /* regset_bitmap */
    NULL, /* usrregs */
    &loongarch_regsets_info,
  };

/* Implementation of linux target ops method "get_regs_info".  */

const regs_info *
loongarch_target::get_regs_info ()
{
  return &loongarch_regs;
}

/* Implementation of linux target ops method "low_fetch_register".  */

bool
loongarch_target::low_fetch_register (regcache *regcache, int regno)
{
  const struct target_desc *tdesc = regcache->tdesc;

  if (regno != find_regno (tdesc, "r0"))
    return false;
  supply_register_zeroed (regcache, regno);
  return true;
}

bool
loongarch_target::low_supports_breakpoints ()
{
  return true;
}

/* Implementation of linux target ops method "low_get_pc".  */

CORE_ADDR
loongarch_target::low_get_pc (regcache *regcache)
{
  if (register_size (regcache->tdesc, 0) == 8)
    return linux_get_pc_64bit (regcache);
  else
    return linux_get_pc_32bit (regcache);
}

/* Implementation of linux target ops method "low_set_pc".  */

void
loongarch_target::low_set_pc (regcache *regcache, CORE_ADDR newpc)
{
  if (register_size (regcache->tdesc, 0) == 8)
    linux_set_pc_64bit (regcache, newpc);
  else
    linux_set_pc_32bit (regcache, newpc);
}

#define loongarch_breakpoint_len 4

/* LoongArch BRK software debug mode instruction.
   This instruction needs to match gdb/loongarch-tdep.c
   (loongarch_default_breakpoint).  */
static const gdb_byte loongarch_breakpoint[] = {0x05, 0x00, 0x2a, 0x00};

/* Implementation of target ops method "breakpoint_kind_from_pc".  */

int
loongarch_target::breakpoint_kind_from_pc (CORE_ADDR *pcptr)
{
  return loongarch_breakpoint_len;
}

/* Implementation of target ops method "sw_breakpoint_from_kind".  */

const gdb_byte *
loongarch_target::sw_breakpoint_from_kind (int kind, int *size)
{
  *size = loongarch_breakpoint_len;
  return (const gdb_byte *) &loongarch_breakpoint;
}

/* Implementation of linux target ops method "low_breakpoint_at".  */

bool
loongarch_target::low_breakpoint_at (CORE_ADDR pc)
{
  gdb_byte insn[loongarch_breakpoint_len];

  read_memory (pc, (unsigned char *) &insn, loongarch_breakpoint_len);
  if (memcmp (insn, loongarch_breakpoint, loongarch_breakpoint_len) == 0)
    return true;

  return false;
}

/* The linux target ops object.  */

linux_process_target *the_linux_target = &the_loongarch_target;

/* Initialize the LoongArch/Linux target.  */

void
initialize_low_arch ()
{
  initialize_regsets_info (&loongarch_regsets_info);
}
