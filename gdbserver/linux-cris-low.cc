/* GNU/Linux/CRIS specific low level interface, for the remote server for GDB.
   Copyright (C) 1995-2020 Free Software Foundation, Inc.

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

/* Linux target op definitions for the CRIS architecture.  */

class cris_target : public linux_process_target
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

static cris_target the_cris_target;

bool
cris_target::low_supports_breakpoints ()
{
  return true;
}

CORE_ADDR
cris_target::low_get_pc (regcache *regcache)
{
  return linux_get_pc_32bit (regcache);
}

void
cris_target::low_set_pc (regcache *regcache, CORE_ADDR pc)
{
  linux_set_pc_32bit (regcache, pc);
}

/* Defined in auto-generated file reg-cris.c.  */
void init_registers_cris (void);
extern const struct target_desc *tdesc_cris;

/* CRISv10 */
#define cris_num_regs 32

/* Locations need to match <include/asm/arch/ptrace.h>.  */
static int cris_regmap[] = {
  15*4, 14*4, 13*4, 12*4,
  11*4, 10*4, 9*4, 8*4,
  7*4, 6*4, 5*4, 4*4,
  3*4, 2*4, 23*4, 19*4,

  -1, -1, -1, -1,
  -1, 17*4, -1, 16*4,
  -1, -1, -1, 18*4,
  -1, 17*4, -1, -1

};

bool
cris_target::low_cannot_store_register (int regno)
{
  if (cris_regmap[regno] == -1)
    return true;

  return (regno >= cris_num_regs);
}

bool
cris_target::low_cannot_fetch_register (int regno)
{
  if (cris_regmap[regno] == -1)
    return true;

  return (regno >= cris_num_regs);
}

static const unsigned short cris_breakpoint = 0xe938;
#define cris_breakpoint_len 2

/* Implementation of target ops method "sw_breakpoint_from_kind".  */

const gdb_byte *
cris_target::sw_breakpoint_from_kind (int kind, int *size)
{
  *size = cris_breakpoint_len;
  return (const gdb_byte *) &cris_breakpoint;
}

bool
cris_target::low_breakpoint_at (CORE_ADDR where)
{
  unsigned short insn;

  read_memory (where, (unsigned char *) &insn, cris_breakpoint_len);
  if (insn == cris_breakpoint)
    return true;

  /* If necessary, recognize more trap instructions here.  GDB only uses the
     one.  */
  return false;
}

void
cris_target::low_arch_setup ()
{
  current_process ()->tdesc = tdesc_cris;
}

static struct usrregs_info cris_usrregs_info =
  {
    cris_num_regs,
    cris_regmap,
  };

static struct regs_info myregs_info =
  {
    NULL, /* regset_bitmap */
    &cris_usrregs_info,
  };

const regs_info *
cris_target::get_regs_info ()
{
  return &myregs_info;
}

/* The linux target ops object.  */

linux_process_target *the_linux_target = &the_cris_target;

void
initialize_low_arch (void)
{
  init_registers_cris ();
}
