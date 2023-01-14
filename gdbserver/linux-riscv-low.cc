/* GNU/Linux/RISC-V specific low level interface, for the remote server
   for GDB.
   Copyright (C) 2020 Free Software Foundation, Inc.

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
#include "nat/riscv-linux-tdesc.h"
#include "opcode/riscv.h"

/* Work around glibc header breakage causing ELF_NFPREG not to be usable.  */
#ifndef NFPREG
# define NFPREG 33
#endif

/* Implementation of linux_target_ops method "arch_setup".  */

static void
riscv_arch_setup ()
{
  static const char *expedite_regs[] = { "sp", "pc", NULL };

  const riscv_gdbarch_features features
    = riscv_linux_read_features (lwpid_of (current_thread));
  target_desc *tdesc = riscv_create_target_description (features);

  if (!tdesc->expedite_regs)
    init_target_desc (tdesc, expedite_regs);
  current_process ()->tdesc = tdesc;
}

/* Collect GPRs from REGCACHE into BUF.  */

static void
riscv_fill_gregset (struct regcache *regcache, void *buf)
{
  const struct target_desc *tdesc = regcache->tdesc;
  elf_gregset_t *regset = (elf_gregset_t *) buf;
  int regno = find_regno (tdesc, "zero");
  int i;

  collect_register_by_name (regcache, "pc", *regset);
  for (i = 1; i < ARRAY_SIZE (*regset); i++)
    collect_register (regcache, regno + i, *regset + i);
}

/* Supply GPRs from BUF into REGCACHE.  */

static void
riscv_store_gregset (struct regcache *regcache, const void *buf)
{
  const elf_gregset_t *regset = (const elf_gregset_t *) buf;
  const struct target_desc *tdesc = regcache->tdesc;
  int regno = find_regno (tdesc, "zero");
  int i;

  supply_register_by_name (regcache, "pc", *regset);
  supply_register_zeroed (regcache, regno);
  for (i = 1; i < ARRAY_SIZE (*regset); i++)
    supply_register (regcache, regno + i, *regset + i);
}

/* Collect FPRs from REGCACHE into BUF.  */

static void
riscv_fill_fpregset (struct regcache *regcache, void *buf)
{
  const struct target_desc *tdesc = regcache->tdesc;
  int regno = find_regno (tdesc, "ft0");
  int flen = register_size (regcache->tdesc, regno);
  gdb_byte *regbuf = (gdb_byte *) buf;
  int i;

  for (i = 0; i < ELF_NFPREG - 1; i++, regbuf += flen)
    collect_register (regcache, regno + i, regbuf);
  collect_register_by_name (regcache, "fcsr", regbuf);
}

/* Supply FPRs from BUF into REGCACHE.  */

static void
riscv_store_fpregset (struct regcache *regcache, const void *buf)
{
  const struct target_desc *tdesc = regcache->tdesc;
  int regno = find_regno (tdesc, "ft0");
  int flen = register_size (regcache->tdesc, regno);
  const gdb_byte *regbuf = (const gdb_byte *) buf;
  int i;

  for (i = 0; i < ELF_NFPREG - 1; i++, regbuf += flen)
    supply_register (regcache, regno + i, regbuf);
  supply_register_by_name (regcache, "fcsr", regbuf);
}

/* RISC-V/Linux regsets.  FPRs are optional and come in different sizes,
   so define multiple regsets for them marking them all as OPTIONAL_REGS
   rather than FP_REGS, so that "regsets_fetch_inferior_registers" picks
   the right one according to size.  */
static struct regset_info riscv_regsets[] = {
  { PTRACE_GETREGSET, PTRACE_SETREGSET, NT_PRSTATUS,
    sizeof (elf_gregset_t), GENERAL_REGS,
    riscv_fill_gregset, riscv_store_gregset },
  { PTRACE_GETREGSET, PTRACE_SETREGSET, NT_FPREGSET,
    sizeof (struct __riscv_mc_q_ext_state), OPTIONAL_REGS,
    riscv_fill_fpregset, riscv_store_fpregset },
  { PTRACE_GETREGSET, PTRACE_SETREGSET, NT_FPREGSET,
    sizeof (struct __riscv_mc_d_ext_state), OPTIONAL_REGS,
    riscv_fill_fpregset, riscv_store_fpregset },
  { PTRACE_GETREGSET, PTRACE_SETREGSET, NT_FPREGSET,
    sizeof (struct __riscv_mc_f_ext_state), OPTIONAL_REGS,
    riscv_fill_fpregset, riscv_store_fpregset },
  NULL_REGSET
};

/* RISC-V/Linux regset information.  */
static struct regsets_info riscv_regsets_info =
  {
    riscv_regsets, /* regsets */
    0, /* num_regsets */
    NULL, /* disabled_regsets */
  };

/* Definition of linux_target_ops data member "regs_info".  */
static struct regs_info riscv_regs =
  {
    NULL, /* regset_bitmap */
    NULL, /* usrregs */
    &riscv_regsets_info,
  };

/* Implementation of linux_target_ops method "regs_info".  */

static const struct regs_info *
riscv_regs_info ()
{
  return &riscv_regs;
}

/* Implementation of linux_target_ops method "fetch_register".  */

static int
riscv_fetch_register (struct regcache *regcache, int regno)
{
  const struct target_desc *tdesc = regcache->tdesc;

  if (regno != find_regno (tdesc, "zero"))
    return 0;
  supply_register_zeroed (regcache, regno);
  return 1;
}

/* Implementation of linux_target_ops method "get_pc".  */

static CORE_ADDR
riscv_get_pc (struct regcache *regcache)
{
  elf_gregset_t regset;

  if (sizeof (regset[0]) == 8)
    return linux_get_pc_64bit (regcache);
  else
    return linux_get_pc_32bit (regcache);
}

/* Implementation of linux_target_ops method "set_pc".  */

static void
riscv_set_pc (struct regcache *regcache, CORE_ADDR newpc)
{
  elf_gregset_t regset;

  if (sizeof (regset[0]) == 8)
    linux_set_pc_64bit (regcache, newpc);
  else
    linux_set_pc_32bit (regcache, newpc);
}

/* Correct in either endianness.  */
static const uint16_t riscv_ibreakpoint[] = { 0x0073, 0x0010 };
static const uint16_t riscv_cbreakpoint = 0x9002;

/* Implementation of linux_target_ops method "breakpoint_kind_from_pc".  */

static int
riscv_breakpoint_kind_from_pc (CORE_ADDR *pcptr)
{
  union
    {
      gdb_byte bytes[2];
      uint16_t insn;
    }
  buf;

  if (target_read_memory (*pcptr, buf.bytes, sizeof (buf.insn)) == 0
      && riscv_insn_length (buf.insn == sizeof (riscv_ibreakpoint)))
    return sizeof (riscv_ibreakpoint);
  else
    return sizeof (riscv_cbreakpoint);
}

/* Implementation of linux_target_ops method "sw_breakpoint_from_kind".  */

static const gdb_byte *
riscv_sw_breakpoint_from_kind (int kind, int *size)
{
  *size = kind;
  switch (kind)
    {
      case sizeof (riscv_ibreakpoint):
	return (const gdb_byte *) &riscv_ibreakpoint;
      default:
	return (const gdb_byte *) &riscv_cbreakpoint;
    }
}

/* Implementation of linux_target_ops method "breakpoint_at".  */

static int
riscv_breakpoint_at (CORE_ADDR pc)
{
  union
    {
      gdb_byte bytes[2];
      uint16_t insn;
    }
  buf;

  if (target_read_memory (pc, buf.bytes, sizeof (buf.insn)) == 0
      && (buf.insn == riscv_cbreakpoint
	  || (buf.insn == riscv_ibreakpoint[0]
	      && target_read_memory (pc + sizeof (buf.insn), buf.bytes,
				     sizeof (buf.insn)) == 0
	      && buf.insn == riscv_ibreakpoint[1])))
    return 1;
  else
    return 0;
}

/* RISC-V/Linux target operations.  */
struct linux_target_ops the_low_target =
{
  riscv_arch_setup,
  riscv_regs_info,
  NULL, /* cannot_fetch_register */
  NULL, /* cannot_store_register */
  riscv_fetch_register,
  riscv_get_pc,
  riscv_set_pc,
  riscv_breakpoint_kind_from_pc,
  riscv_sw_breakpoint_from_kind,
  NULL, /* get_next_pcs */
  0,    /* decr_pc_after_break */
  riscv_breakpoint_at,
};

/* Initialize the RISC-V/Linux target.  */

void
initialize_low_arch ()
{
  initialize_regsets_info (&riscv_regsets_info);
}
