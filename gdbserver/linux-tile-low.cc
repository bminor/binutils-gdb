/* GNU/Linux/TILE-Gx specific low level interface, GDBserver.

   Copyright (C) 2012-2020 Free Software Foundation, Inc.

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

#include <arch/abi.h>
#include "nat/gdb_ptrace.h"

/* Linux target op definitions for the TILE-Gx architecture.  */

class tile_target : public linux_process_target
{
public:

  const regs_info *get_regs_info () override;

  const gdb_byte *sw_breakpoint_from_kind (int kind, int *size) override;

protected:

  void low_arch_setup () override;

  bool low_cannot_fetch_register (int regno) override;

  bool low_cannot_store_register (int regno) override;

  bool low_supports_breakpoints () override;

  CORE_ADDR low_get_pc (regcache *regcache) override;

  void low_set_pc (regcache *regcache, CORE_ADDR newpc) override;

  bool low_breakpoint_at (CORE_ADDR pc) override;
};

/* The singleton target ops object.  */

static tile_target the_tile_target;

bool
tile_target::low_supports_breakpoints ()
{
  return true;
}

CORE_ADDR
tile_target::low_get_pc (regcache *regcache)
{
  return linux_get_pc_64bit (regcache);
}

void
tile_target::low_set_pc (regcache *regcache, CORE_ADDR pc)
{
  linux_set_pc_64bit (regcache, pc);
}

/* Defined in auto-generated file reg-tilegx.c.  */
void init_registers_tilegx (void);
extern const struct target_desc *tdesc_tilegx;

/* Defined in auto-generated file reg-tilegx32.c.  */
void init_registers_tilegx32 (void);
extern const struct target_desc *tdesc_tilegx32;

#define tile_num_regs 65

static int tile_regmap[] =
{
   0,  1,  2,  3,  4,  5,  6,  7,
   8,  9, 10, 11, 12, 13, 14, 15,
  16, 17, 18, 19, 20, 21, 22, 23,
  24, 25, 26, 27, 28, 29, 30, 31,
  32, 33, 34, 35, 36, 37, 38, 39,
  40, 41, 42, 43, 44, 45, 46, 47,
  48, 49, 50, 51, 52, 53, 54, 55,
  -1, -1, -1, -1, -1, -1, -1, -1,
  56
};

bool
tile_target::low_cannot_fetch_register (int regno)
{
  if (regno >= 0 && regno < 56)
    return false;
  else if (regno == 64)
    return false;
  else
    return true;
}

bool
tile_target::low_cannot_store_register (int regno)
{
  if (regno >= 0 && regno < 56)
    return false;
  else if (regno == 64)
    return false;
  else
    return true;
}

static uint64_t tile_breakpoint = 0x400b3cae70166000ULL;
#define tile_breakpoint_len 8

/* Implementation of target ops method "sw_breakpoint_from_kind".  */

const gdb_byte *
tile_target::sw_breakpoint_from_kind (int kind, int *size)
{
  *size = tile_breakpoint_len;
  return (const gdb_byte *) &tile_breakpoint;
}

bool
tile_target::low_breakpoint_at (CORE_ADDR where)
{
  uint64_t insn;

  read_memory (where, (unsigned char *) &insn, 8);
  if (insn == tile_breakpoint)
    return true;

  /* If necessary, recognize more trap instructions here.  GDB only uses the
     one.  */
  return false;
}

static void
tile_fill_gregset (struct regcache *regcache, void *buf)
{
  int i;

  for (i = 0; i < tile_num_regs; i++)
    if (tile_regmap[i] != -1)
      collect_register (regcache, i, ((uint_reg_t *) buf) + tile_regmap[i]);
}

static void
tile_store_gregset (struct regcache *regcache, const void *buf)
{
  int i;

  for (i = 0; i < tile_num_regs; i++)
    if (tile_regmap[i] != -1)
      supply_register (regcache, i, ((uint_reg_t *) buf) + tile_regmap[i]);
}

static struct regset_info tile_regsets[] =
{
  { PTRACE_GETREGS, PTRACE_SETREGS, 0, tile_num_regs * 8,
    GENERAL_REGS, tile_fill_gregset, tile_store_gregset },
  NULL_REGSET
};

static struct regsets_info tile_regsets_info =
  {
    tile_regsets, /* regsets */
    0, /* num_regsets */
    NULL, /* disabled_regsets */
  };

static struct usrregs_info tile_usrregs_info =
  {
    tile_num_regs,
    tile_regmap,
  };

static struct regs_info myregs_info =
  {
    NULL, /* regset_bitmap */
    &tile_usrregs_info,
    &tile_regsets_info,
  };

const regs_info *
tile_target::get_regs_info ()
{
  return &myregs_info;
}

void
tile_target::low_arch_setup ()
{
  int pid = pid_of (current_thread);
  unsigned int machine;
  int is_elf64 = linux_pid_exe_is_elf_64_file (pid, &machine);

  if (sizeof (void *) == 4)
    if (is_elf64 > 0)
      error (_("Can't debug 64-bit process with 32-bit GDBserver"));

  if (!is_elf64)
    current_process ()->tdesc = tdesc_tilegx32;
  else
    current_process ()->tdesc = tdesc_tilegx;
}

/* The linux target ops object.  */

linux_process_target *the_linux_target = &the_tile_target;

void
initialize_low_arch (void)
{
  init_registers_tilegx32();
  init_registers_tilegx();

  initialize_regsets_info (&tile_regsets_info);
}
