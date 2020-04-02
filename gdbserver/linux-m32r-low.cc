/* GNU/Linux/m32r specific low level interface, for the remote server for GDB.
   Copyright (C) 2005-2020 Free Software Foundation, Inc.

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

#ifdef HAVE_SYS_REG_H
#include <sys/reg.h>
#endif

/* Linux target op definitions for the m32r architecture.  */

class m32r_target : public linux_process_target
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

static m32r_target the_m32r_target;

bool
m32r_target::low_supports_breakpoints ()
{
  return true;
}

CORE_ADDR
m32r_target::low_get_pc (regcache *regcache)
{
  return linux_get_pc_32bit (regcache);
}

void
m32r_target::low_set_pc (regcache *regcache, CORE_ADDR pc)
{
  linux_set_pc_32bit (regcache, pc);
}

/* Defined in auto-generated file reg-m32r.c.  */
void init_registers_m32r (void);
extern const struct target_desc *tdesc_m32r;

#define m32r_num_regs 25

static int m32r_regmap[] = {
#ifdef PT_R0
  PT_R0, PT_R1, PT_R2, PT_R3, PT_R4, PT_R5, PT_R6, PT_R7,
  PT_R8, PT_R9, PT_R10, PT_R11, PT_R12, PT_FP, PT_LR, PT_SPU,
  PT_PSW, PT_CBR, PT_SPI, PT_SPU, PT_BPC, PT_PC, PT_ACCL, PT_ACCH, PT_EVB
#else
  4 * 4, 4 * 5, 4 * 6, 4 * 7, 4 * 0, 4 * 1, 4 * 2, 4 * 8,
  4 * 9, 4 * 10, 4 * 11, 4 * 12, 4 * 13, 4 * 24, 4 * 25, 4 * 23,
  4 * 19, 4 * 31, 4 * 26, 4 * 23, 4 * 20, 4 * 30, 4 * 16, 4 * 15, 4 * 32
#endif
};

bool
m32r_target::low_cannot_store_register (int regno)
{
  return (regno >= m32r_num_regs);
}

bool
m32r_target::low_cannot_fetch_register (int regno)
{
  return (regno >= m32r_num_regs);
}

static const unsigned short m32r_breakpoint = 0x10f1;
#define m32r_breakpoint_len 2

/* Implementation of target ops method "sw_breakpoint_from_kind".  */

const gdb_byte *
m32r_target::sw_breakpoint_from_kind (int kind, int *size)
{
  *size = m32r_breakpoint_len;
  return (const gdb_byte *) &m32r_breakpoint;
}

bool
m32r_target::low_breakpoint_at (CORE_ADDR where)
{
  unsigned short insn;

  read_memory (where, (unsigned char *) &insn, m32r_breakpoint_len);
  if (insn == m32r_breakpoint)
    return true;

  /* If necessary, recognize more trap instructions here.  GDB only uses the
     one.  */
  return false;
}

void
m32r_target::low_arch_setup ()
{
  current_process ()->tdesc = tdesc_m32r;
}

static struct usrregs_info m32r_usrregs_info =
  {
    m32r_num_regs,
    m32r_regmap,
  };

static struct regs_info myregs_info =
  {
    NULL, /* regset_bitmap */
    &m32r_usrregs_info,
  };

const regs_info *
m32r_target::get_regs_info ()
{
  return &myregs_info;
}

/* The linux target ops object.  */

linux_process_target *the_linux_target = &the_m32r_target;

void
initialize_low_arch (void)
{
  init_registers_m32r ();
}
