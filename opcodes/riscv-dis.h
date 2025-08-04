/* riscv-dis.h -- Header file for riscv-dis.c.
   Copyright (C) 2012-2025 Free Software Foundation, Inc.
   Contributed by ARM Ltd.

   This file is part of the GNU opcodes library.

   This library is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   It is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING3. If not,
   see <http://www.gnu.org/licenses/>.  */

#ifndef OPCODES_RISCV_DIS_H
#define OPCODES_RISCV_DIS_H
#include "opcode/riscv.h"
#include "elf-bfd.h"
#include "elf/riscv.h"
#include "elfxx-riscv.h"


struct riscv_private_data
{
  bfd_vma gp;
  bfd_vma print_addr;
  bfd_vma hi_addr[OP_MASK_RD + 1];
  bool to_print_addr;
  bool has_gp;
  /* Current XLEN for the disassembler.  */
  unsigned xlen;
  /* Default ISA specification version.  */
  enum riscv_spec_class default_isa_spec;
  /* Default privileged specification.  */
  enum riscv_spec_class default_priv_spec;
  /* Used for architecture parser.  */
  riscv_parse_subset_t riscv_rps_dis;
  /* Default architecture string for the object file.  It will be changed once
     elf architecture attribute exits.  This is used for mapping symbol $x.  */
  const char* default_arch;
  /* Used for mapping symbols.  */
  int last_map_symbol;
  bfd_vma last_stop_offset;
  bfd_vma last_map_symbol_boundary;
  enum riscv_seg_mstate last_map_state;
  asection *last_map_section;
  /* Register names as used by the disassembler.  */
  const char (*riscv_gpr_names)[NRC];
  const char (*riscv_fpr_names)[NRC];
  /* If set, disassemble as most general instruction.  */
  bool no_aliases;
  /* If set, disassemble without checking architecture string, just like what
     we did at the beginning.  */
  bool all_ext;
};

#undef RISCV_DECL_OPD_EXTRACTOR

#endif /* OPCODES_RISCV_DIS_H */
