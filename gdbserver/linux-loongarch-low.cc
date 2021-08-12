/* Copyright (C) 2021 Free Software Foundation, Inc.

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

#include "nat/gdb_ptrace.h"

#include "gdb_proc_service.h"
#include "arch/loongarch.h"
#include "arch/loongarch-linux-nat.h"
#include "elf/common.h"
#include "tdesc.h"

class loongarch_target : public linux_process_target
{
public:
  const regs_info *get_regs_info () override;

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

static loongarch_target the_loongarch_target;

static const unsigned int loongarch_breakpoint = 0x002a0005;
#define loongarch_breakpoint_len 4

void
loongarch_target::low_arch_setup ()
{
  int pid = lwpid_of (current_thread);
  struct target_desc *tdesc = loongarch_linux_read_description_runtime (pid);
  init_target_desc (tdesc, loongarch_expedite_regs);
  current_process ()->tdesc = tdesc;
}

bool
loongarch_target::low_cannot_fetch_register (int regno)
{
  return false;
}

bool
loongarch_target::low_cannot_store_register (int regno)
{
  return false;
}

bool
loongarch_target::low_fetch_register (regcache *regcache, int regno)
{
  const struct target_desc *tdesc = current_process ()->tdesc;

  if (find_regno (tdesc, "r0") == regno)
    {
      supply_register_zeroed (regcache, regno);
      return true;
    }

  return false;
}

bool
loongarch_target::low_supports_breakpoints ()
{
  return true;
}

CORE_ADDR
loongarch_target::low_get_pc (regcache *regcache)
{
  int regno = find_regno (regcache->tdesc, "pc");
  if (register_size (regcache->tdesc, regno) == 4)
    {
      int32_t pc;
      collect_register (regcache, regno, &pc);
      return pc;
    }
  else
    {
      int64_t pc;
      collect_register (regcache, regno, &pc);
      return pc;
    }
}

void
loongarch_target::low_set_pc (regcache *regcache, CORE_ADDR pc)
{
  supply_register_by_name (regcache, "pc", &pc);
}

const gdb_byte *
loongarch_target::sw_breakpoint_from_kind (int kind, int *size)
{
  *size = loongarch_breakpoint_len;
  return (const gdb_byte *) &loongarch_breakpoint;
}

bool
loongarch_target::low_breakpoint_at (CORE_ADDR where)
{
  uint32_t insn;

  read_memory (where, (unsigned char *) &insn, sizeof (insn));
  if (insn == loongarch_breakpoint)
    return true;

  return false;
}

static void
loongarch_fill_gregset (struct regcache *regcache, void *buf)
{
  int i, r = find_regno (regcache->tdesc, "r0");

  for (i = 0; i < 32; i++)
    collect_register (regcache, r + i, (uint64_t *) buf + i);
  collect_register_by_name (regcache, "pc", (uint64_t *) buf + 32);
}

static void
loongarch_store_gregset (struct regcache *regcache, const void *buf)
{
  int i, r = find_regno (regcache->tdesc, "r0");

  supply_register_zeroed (regcache, r);
  for (i = 1; i < 32; i++)
    supply_register (regcache, r + i, (const uint64_t *) buf + i);
  supply_register_by_name (regcache, "pc", (const uint64_t *) buf + 32);
}

static void
loongarch_fill_fpregset (struct regcache *regcache, void *buf)
{
  int i, f, fcc;
  f = find_regno (regcache->tdesc, "f0");
  fcc = find_regno (regcache->tdesc, "fcc0");
  uint8_t *fccs = (uint8_t *) ((uint64_t *) buf + 32);

  for (i = 0; i < 32; i++)
    collect_register (regcache, f + i, (uint64_t *) buf + i);
  for (i = 0; i < 8; i++)
    collect_register (regcache, fcc + i, fccs + i);
  collect_register_by_name (regcache, "fcsr", (uint64_t *) buf + 33);
}

static void
loongarch_store_fpregset (struct regcache *regcache, const void *buf)
{
  int i, f, fcc;
  f = find_regno (regcache->tdesc, "f0");
  fcc = find_regno (regcache->tdesc, "fcc0");
  const uint8_t *fccs = (const uint8_t *) ((const uint64_t *) buf + 32);

  for (i = 0; i < 32; i++)
    supply_register (regcache, f + i, (const uint64_t *) buf + i);
  for (i = 0; i < 8; i++)
    supply_register (regcache, fcc + i, fccs + i);
  supply_register_by_name (regcache, "fcsr", (const uint64_t *) buf + 33);
}

static void
loongarch_fill_lbtregset (struct regcache *regcache, void *buf)
{
  int i, scr = find_regno (regcache->tdesc, "scr0");

  for (i = 0; i < 4; i++)
    collect_register (regcache, scr + i, (uint64_t *) buf + i);
  collect_register_by_name (regcache, "EFLAG", (uint64_t *) buf + 4);
  collect_register_by_name (regcache, "x86_top", (uint32_t *) buf + 9);
}

static void
loongarch_store_lbtregset (struct regcache *regcache, const void *buf)
{
  int i, scr = find_regno (regcache->tdesc, "scr0");

  for (i = 0; i < 4; i++)
    supply_register (regcache, scr + i, (const uint64_t *) buf + i);
  supply_register_by_name (regcache, "EFLAG", (const uint64_t *) buf + 4);
  supply_register_by_name (regcache, "x86_top", (const uint32_t *) buf + 9);
}

static void
loongarch_fill_lsxregset (struct regcache *regcache, void *buf)
{
  int i, vr = find_regno (regcache->tdesc, "vr0");

  for (i = 0; i < 32; i++)
    collect_register (regcache, vr + i, (uint64_t *) buf + 2 * i);
}

static void
loongarch_store_lsxregset (struct regcache *regcache, const void *buf)
{
  int i, vr = find_regno (regcache->tdesc, "vr0");

  for (i = 1; i < 32; i++)
    supply_register (regcache, vr + i, (const uint64_t *) buf + 2 * i);
}

static void
loongarch_fill_lasxregset (struct regcache *regcache, void *buf)
{
  int i, xr = find_regno (regcache->tdesc, "xr0");

  for (i = 0; i < 32; i++)
    collect_register (regcache, xr + i, (uint64_t *) buf + 4 * i);
}

static void
loongarch_store_lasxregset (struct regcache *regcache, const void *buf)
{
  int i, xr = find_regno (regcache->tdesc, "xr0");

  for (i = 1; i < 32; i++)
    supply_register (regcache, xr + i, (const uint64_t *) buf + 4 * i);
}

static struct regset_info loongarch_regsets[] =
{
  { PTRACE_GETREGSET, PTRACE_SETREGSET, NT_PRSTATUS, 33 * 8, GENERAL_REGS,
    loongarch_fill_gregset, loongarch_store_gregset },
  { PTRACE_GETREGSET, PTRACE_SETREGSET, NT_FPREGSET, 34 * 8, FP_REGS,
    loongarch_fill_fpregset, loongarch_store_fpregset },
  { PTRACE_GETREGSET, PTRACE_SETREGSET, NT_LARCH_LBT, 5 * 8, EXTENDED_REGS,
    loongarch_fill_lbtregset, loongarch_store_lbtregset },
  { PTRACE_GETREGSET, PTRACE_SETREGSET, NT_LARCH_LSX, 32 * 16, EXTENDED_REGS,
    loongarch_fill_lsxregset, loongarch_store_lsxregset },
  { PTRACE_GETREGSET, PTRACE_SETREGSET, NT_LARCH_LASX, 32 * 32, EXTENDED_REGS,
    loongarch_fill_lasxregset, loongarch_store_lasxregset },
  NULL_REGSET
};

static struct regsets_info loongarch_regsets_info =
{
  loongarch_regsets, 	/* regsets.  */
  0,		 	/* num_regsets.  */
  NULL,	      		/* disabled_regsets.  */
};

static regs_info loongarch_regs_info = { NULL, /* regset_bitmap.  */
					 NULL, &loongarch_regsets_info };

const regs_info *
loongarch_target::get_regs_info ()
{
  return &loongarch_regs_info;
}

/* The linux target ops object.  */

linux_process_target *the_linux_target = &the_loongarch_target;

void initialize_low_arch ();
void
initialize_low_arch ()
{
  initialize_regsets_info (&loongarch_regsets_info);
}
