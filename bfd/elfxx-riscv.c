/* RISC-V-specific support for ELF.
   Copyright (C) 2011-2021 Free Software Foundation, Inc.

   Contributed by Andrew Waterman (andrew@sifive.com).
   Based on TILE-Gx and MIPS targets.

   This file is part of BFD, the Binary File Descriptor library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING3. If not,
   see <http://www.gnu.org/licenses/>.  */

#include "sysdep.h"
#include "bfd.h"
#include "libbfd.h"
#include "elf-bfd.h"
#include "elf/riscv.h"
#include "opcode/riscv.h"
#include "libiberty.h"
#include "elfxx-riscv.h"
#include "safe-ctype.h"
#include "cpu-riscv.h"

#define MINUS_ONE ((bfd_vma)0 - 1)

/* Special handler for ADD/SUB relocations that allows them to be filled out
   both in the pre-linked and post-linked file.  This is necessary to make
   pre-linked debug info work, as due to linker relaxations we need to emit
   relocations for the debug info.  */
static bfd_reloc_status_type riscv_elf_add_sub_reloc
  (bfd *, arelent *, asymbol *, void *, asection *, bfd *, char **);

/* The relocation table used for SHT_RELA sections.  */

static reloc_howto_type howto_table[] =
{
  /* No relocation.  */
  HOWTO (R_RISCV_NONE,			/* type */
	 0,				/* rightshift */
	 3,				/* size */
	 0,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_NONE",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 0,				/* dst_mask */
	 false),			/* pcrel_offset */

  /* 32 bit relocation.  */
  HOWTO (R_RISCV_32,			/* type */
	 0,				/* rightshift */
	 2,				/* size */
	 32,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_32",			/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 0xffffffff,			/* dst_mask */
	 false),			/* pcrel_offset */

  /* 64 bit relocation.  */
  HOWTO (R_RISCV_64,			/* type */
	 0,				/* rightshift */
	 4,				/* size */
	 64,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_64",			/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 MINUS_ONE,			/* dst_mask */
	 false),			/* pcrel_offset */

  /* Relocation against a local symbol in a shared object.  */
  HOWTO (R_RISCV_RELATIVE,		/* type */
	 0,				/* rightshift */
	 2,				/* size */
	 32,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_RELATIVE",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 0xffffffff,			/* dst_mask */
	 false),			/* pcrel_offset */

  HOWTO (R_RISCV_COPY,			/* type */
	 0,				/* rightshift */
	 0,				/* this one is variable size */
	 0,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_bitfield,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_COPY",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 0,				/* dst_mask */
	 false),			/* pcrel_offset */

  HOWTO (R_RISCV_JUMP_SLOT,		/* type */
	 0,				/* rightshift */
	 4,				/* size */
	 64,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_bitfield,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_JUMP_SLOT",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 0,				/* dst_mask */
	 false),			/* pcrel_offset */

  /* Dynamic TLS relocations.  */
  HOWTO (R_RISCV_TLS_DTPMOD32,		/* type */
	 0,				/* rightshift */
	 2,				/* size */
	 32,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_TLS_DTPMOD32",	/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 0xffffffff,			/* dst_mask */
	 false),			/* pcrel_offset */

  HOWTO (R_RISCV_TLS_DTPMOD64,		/* type */
	 0,				/* rightshift */
	 4,				/* size */
	 64,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_TLS_DTPMOD64",	/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 MINUS_ONE,			/* dst_mask */
	 false),			/* pcrel_offset */

  HOWTO (R_RISCV_TLS_DTPREL32,		/* type */
	 0,				/* rightshift */
	 2,				/* size */
	 32,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_TLS_DTPREL32",	/* name */
	 true,				/* partial_inplace */
	 0,				/* src_mask */
	 0xffffffff,			/* dst_mask */
	 false),			/* pcrel_offset */

  HOWTO (R_RISCV_TLS_DTPREL64,		/* type */
	 0,				/* rightshift */
	 4,				/* size */
	 64,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_TLS_DTPREL64",	/* name */
	 true,				/* partial_inplace */
	 0,				/* src_mask */
	 MINUS_ONE,			/* dst_mask */
	 false),			/* pcrel_offset */

  HOWTO (R_RISCV_TLS_TPREL32,		/* type */
	 0,				/* rightshift */
	 2,				/* size */
	 32,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_TLS_TPREL32",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 0xffffffff,			/* dst_mask */
	 false),			/* pcrel_offset */

  HOWTO (R_RISCV_TLS_TPREL64,		/* type */
	 0,				/* rightshift */
	 4,				/* size */
	 64,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_TLS_TPREL64",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 MINUS_ONE,			/* dst_mask */
	 false),			/* pcrel_offset */

  /* Reserved for future relocs that the dynamic linker must understand.  */
  EMPTY_HOWTO (12),
  EMPTY_HOWTO (13),
  EMPTY_HOWTO (14),
  EMPTY_HOWTO (15),

  /* 12-bit PC-relative branch offset.  */
  HOWTO (R_RISCV_BRANCH,		/* type */
	 0,				/* rightshift */
	 2,				/* size */
	 32,				/* bitsize */
	 true,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_signed,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_BRANCH",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 ENCODE_BTYPE_IMM (-1U),	/* dst_mask */
	 true),				/* pcrel_offset */

  /* 20-bit PC-relative jump offset.  */
  HOWTO (R_RISCV_JAL,			/* type */
	 0,				/* rightshift */
	 2,				/* size */
	 32,				/* bitsize */
	 true,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_JAL",			/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 ENCODE_JTYPE_IMM (-1U),	/* dst_mask */
	 true),				/* pcrel_offset */

  /* 32-bit PC-relative function call (AUIPC/JALR).  */
  HOWTO (R_RISCV_CALL,			/* type */
	 0,				/* rightshift */
	 4,				/* size */
	 64,				/* bitsize */
	 true,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_CALL",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 ENCODE_UTYPE_IMM (-1U) | ((bfd_vma) ENCODE_ITYPE_IMM (-1U) << 32),
					/* dst_mask */
	 true),				/* pcrel_offset */

  /* Like R_RISCV_CALL, but not locally binding.  */
  HOWTO (R_RISCV_CALL_PLT,		/* type */
	 0,				/* rightshift */
	 4,				/* size */
	 64,				/* bitsize */
	 true,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_CALL_PLT",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 ENCODE_UTYPE_IMM (-1U) | ((bfd_vma) ENCODE_ITYPE_IMM (-1U) << 32),
					/* dst_mask */
	 true),				/* pcrel_offset */

  /* High 20 bits of 32-bit PC-relative GOT access.  */
  HOWTO (R_RISCV_GOT_HI20,		/* type */
	 0,				/* rightshift */
	 2,				/* size */
	 32,				/* bitsize */
	 true,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_GOT_HI20",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 ENCODE_UTYPE_IMM (-1U),	/* dst_mask */
	 false),			/* pcrel_offset */

  /* High 20 bits of 32-bit PC-relative TLS IE GOT access.  */
  HOWTO (R_RISCV_TLS_GOT_HI20,		/* type */
	 0,				/* rightshift */
	 2,				/* size */
	 32,				/* bitsize */
	 true,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_TLS_GOT_HI20",	/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 ENCODE_UTYPE_IMM (-1U),	/* dst_mask */
	 false),			/* pcrel_offset */

  /* High 20 bits of 32-bit PC-relative TLS GD GOT reference.  */
  HOWTO (R_RISCV_TLS_GD_HI20,		/* type */
	 0,				/* rightshift */
	 2,				/* size */
	 32,				/* bitsize */
	 true,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_TLS_GD_HI20",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 ENCODE_UTYPE_IMM (-1U),	/* dst_mask */
	 false),			/* pcrel_offset */

  /* High 20 bits of 32-bit PC-relative reference.  */
  HOWTO (R_RISCV_PCREL_HI20,		/* type */
	 0,				/* rightshift */
	 2,				/* size */
	 32,				/* bitsize */
	 true,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_PCREL_HI20",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 ENCODE_UTYPE_IMM (-1U),	/* dst_mask */
	 true),				/* pcrel_offset */

  /* Low 12 bits of a 32-bit PC-relative load or add.  */
  HOWTO (R_RISCV_PCREL_LO12_I,		/* type */
	 0,				/* rightshift */
	 2,				/* size */
	 32,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_PCREL_LO12_I",	/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 ENCODE_ITYPE_IMM (-1U),	/* dst_mask */
	 false),			/* pcrel_offset */

  /* Low 12 bits of a 32-bit PC-relative store.  */
  HOWTO (R_RISCV_PCREL_LO12_S,		/* type */
	 0,				/* rightshift */
	 2,				/* size */
	 32,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_PCREL_LO12_S",	/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 ENCODE_STYPE_IMM (-1U),	/* dst_mask */
	 false),			/* pcrel_offset */

  /* High 20 bits of 32-bit absolute address.  */
  HOWTO (R_RISCV_HI20,			/* type */
	 0,				/* rightshift */
	 2,				/* size */
	 32,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_HI20",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 ENCODE_UTYPE_IMM (-1U),	/* dst_mask */
	 false),			/* pcrel_offset */

  /* High 12 bits of 32-bit load or add.  */
  HOWTO (R_RISCV_LO12_I,		/* type */
	 0,				/* rightshift */
	 2,				/* size */
	 32,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_LO12_I",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 ENCODE_ITYPE_IMM (-1U),	/* dst_mask */
	 false),			/* pcrel_offset */

  /* High 12 bits of 32-bit store.  */
  HOWTO (R_RISCV_LO12_S,		/* type */
	 0,				/* rightshift */
	 2,				/* size */
	 32,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_LO12_S",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 ENCODE_STYPE_IMM (-1U),	/* dst_mask */
	 false),			/* pcrel_offset */

  /* High 20 bits of TLS LE thread pointer offset.  */
  HOWTO (R_RISCV_TPREL_HI20,		/* type */
	 0,				/* rightshift */
	 2,				/* size */
	 32,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_signed,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_TPREL_HI20",		/* name */
	 true,				/* partial_inplace */
	 0,				/* src_mask */
	 ENCODE_UTYPE_IMM (-1U),	/* dst_mask */
	 false),			/* pcrel_offset */

  /* Low 12 bits of TLS LE thread pointer offset for loads and adds.  */
  HOWTO (R_RISCV_TPREL_LO12_I,		/* type */
	 0,				/* rightshift */
	 2,				/* size */
	 32,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_signed,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_TPREL_LO12_I",	/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 ENCODE_ITYPE_IMM (-1U),	/* dst_mask */
	 false),			/* pcrel_offset */

  /* Low 12 bits of TLS LE thread pointer offset for stores.  */
  HOWTO (R_RISCV_TPREL_LO12_S,		/* type */
	 0,				/* rightshift */
	 2,				/* size */
	 32,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_signed,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_TPREL_LO12_S",	/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 ENCODE_STYPE_IMM (-1U),	/* dst_mask */
	 false),			/* pcrel_offset */

  /* TLS LE thread pointer usage.  May be relaxed.  */
  HOWTO (R_RISCV_TPREL_ADD,		/* type */
	 0,				/* rightshift */
	 3,				/* size */
	 0,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_TPREL_ADD",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 0,				/* dst_mask */
	 false),			/* pcrel_offset */

  /* 8-bit in-place addition, for local label subtraction.  */
  HOWTO (R_RISCV_ADD8,			/* type */
	 0,				/* rightshift */
	 0,				/* size */
	 8,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 riscv_elf_add_sub_reloc,	/* special_function */
	 "R_RISCV_ADD8",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 0xff,				/* dst_mask */
	 false),			/* pcrel_offset */

  /* 16-bit in-place addition, for local label subtraction.  */
  HOWTO (R_RISCV_ADD16,			/* type */
	 0,				/* rightshift */
	 1,				/* size */
	 16,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 riscv_elf_add_sub_reloc,	/* special_function */
	 "R_RISCV_ADD16",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 0xffff,			/* dst_mask */
	 false),			/* pcrel_offset */

  /* 32-bit in-place addition, for local label subtraction.  */
  HOWTO (R_RISCV_ADD32,			/* type */
	 0,				/* rightshift */
	 2,				/* size */
	 32,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 riscv_elf_add_sub_reloc,	/* special_function */
	 "R_RISCV_ADD32",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 0xffffffff,			/* dst_mask */
	 false),			/* pcrel_offset */

  /* 64-bit in-place addition, for local label subtraction.  */
  HOWTO (R_RISCV_ADD64,			/* type */
	 0,				/* rightshift */
	 4,				/* size */
	 64,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 riscv_elf_add_sub_reloc,	/* special_function */
	 "R_RISCV_ADD64",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 MINUS_ONE,			/* dst_mask */
	 false),			/* pcrel_offset */

  /* 8-bit in-place addition, for local label subtraction.  */
  HOWTO (R_RISCV_SUB8,			/* type */
	 0,				/* rightshift */
	 0,				/* size */
	 8,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 riscv_elf_add_sub_reloc,	/* special_function */
	 "R_RISCV_SUB8",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 0xff,				/* dst_mask */
	 false),			/* pcrel_offset */

  /* 16-bit in-place addition, for local label subtraction.  */
  HOWTO (R_RISCV_SUB16,			/* type */
	 0,				/* rightshift */
	 1,				/* size */
	 16,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 riscv_elf_add_sub_reloc,	/* special_function */
	 "R_RISCV_SUB16",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 0xffff,			/* dst_mask */
	 false),			/* pcrel_offset */

  /* 32-bit in-place addition, for local label subtraction.  */
  HOWTO (R_RISCV_SUB32,			/* type */
	 0,				/* rightshift */
	 2,				/* size */
	 32,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 riscv_elf_add_sub_reloc,	/* special_function */
	 "R_RISCV_SUB32",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 0xffffffff,			/* dst_mask */
	 false),			/* pcrel_offset */

  /* 64-bit in-place addition, for local label subtraction.  */
  HOWTO (R_RISCV_SUB64,			/* type */
	 0,				/* rightshift */
	 4,				/* size */
	 64,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 riscv_elf_add_sub_reloc,	/* special_function */
	 "R_RISCV_SUB64",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 MINUS_ONE,			/* dst_mask */
	 false),			/* pcrel_offset */

  /* GNU extension to record C++ vtable hierarchy */
  HOWTO (R_RISCV_GNU_VTINHERIT,		/* type */
	 0,				/* rightshift */
	 4,				/* size */
	 0,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 NULL,				/* special_function */
	 "R_RISCV_GNU_VTINHERIT",	/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 0,				/* dst_mask */
	 false),			/* pcrel_offset */

  /* GNU extension to record C++ vtable member usage */
  HOWTO (R_RISCV_GNU_VTENTRY,		/* type */
	 0,				/* rightshift */
	 4,				/* size */
	 0,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 _bfd_elf_rel_vtable_reloc_fn,	/* special_function */
	 "R_RISCV_GNU_VTENTRY",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 0,				/* dst_mask */
	 false),			/* pcrel_offset */

  /* Indicates an alignment statement.  The addend field encodes how many
     bytes of NOPs follow the statement.  The desired alignment is the
     addend rounded up to the next power of two.  */
  HOWTO (R_RISCV_ALIGN,			/* type */
	 0,				/* rightshift */
	 3,				/* size */
	 0,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_ALIGN",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 0,				/* dst_mask */
	 false),			/* pcrel_offset */

  /* 8-bit PC-relative branch offset.  */
  HOWTO (R_RISCV_RVC_BRANCH,		/* type */
	 0,				/* rightshift */
	 1,				/* size */
	 16,				/* bitsize */
	 true,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_signed,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_RVC_BRANCH",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 ENCODE_CBTYPE_IMM (-1U),	/* dst_mask */
	 true),				/* pcrel_offset */

  /* 11-bit PC-relative jump offset.  */
  HOWTO (R_RISCV_RVC_JUMP,		/* type */
	 0,				/* rightshift */
	 1,				/* size */
	 16,				/* bitsize */
	 true,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_RVC_JUMP",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 ENCODE_CJTYPE_IMM (-1U),	/* dst_mask */
	 true),				/* pcrel_offset */

  /* High 6 bits of 18-bit absolute address.  */
  HOWTO (R_RISCV_RVC_LUI,		/* type */
	 0,				/* rightshift */
	 1,				/* size */
	 16,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_RVC_LUI",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 ENCODE_CITYPE_IMM (-1U),	/* dst_mask */
	 false),			/* pcrel_offset */

  /* GP-relative load.  */
  HOWTO (R_RISCV_GPREL_I,		/* type */
	 0,				/* rightshift */
	 2,				/* size */
	 32,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_GPREL_I",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 ENCODE_ITYPE_IMM (-1U),	/* dst_mask */
	 false),			/* pcrel_offset */

  /* GP-relative store.  */
  HOWTO (R_RISCV_GPREL_S,		/* type */
	 0,				/* rightshift */
	 2,				/* size */
	 32,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_GPREL_S",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 ENCODE_STYPE_IMM (-1U),	/* dst_mask */
	 false),			/* pcrel_offset */

  /* TP-relative TLS LE load.  */
  HOWTO (R_RISCV_TPREL_I,		/* type */
	 0,				/* rightshift */
	 2,				/* size */
	 32,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_signed,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_TPREL_I",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 ENCODE_ITYPE_IMM (-1U),	/* dst_mask */
	 false),			/* pcrel_offset */

  /* TP-relative TLS LE store.  */
  HOWTO (R_RISCV_TPREL_S,		/* type */
	 0,				/* rightshift */
	 2,				/* size */
	 32,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_signed,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_TPREL_S",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 ENCODE_STYPE_IMM (-1U),	/* dst_mask */
	 false),			/* pcrel_offset */

  /* The paired relocation may be relaxed.  */
  HOWTO (R_RISCV_RELAX,			/* type */
	 0,				/* rightshift */
	 3,				/* size */
	 0,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_RELAX",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 0,				/* dst_mask */
	 false),			/* pcrel_offset */

  /* 6-bit in-place addition, for local label subtraction.  */
  HOWTO (R_RISCV_SUB6,			/* type */
	 0,				/* rightshift */
	 0,				/* size */
	 8,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 riscv_elf_add_sub_reloc,	/* special_function */
	 "R_RISCV_SUB6",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 0x3f,				/* dst_mask */
	 false),			/* pcrel_offset */

  /* 6-bit in-place setting, for local label subtraction.  */
  HOWTO (R_RISCV_SET6,			/* type */
	 0,				/* rightshift */
	 0,				/* size */
	 8,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_SET6",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 0x3f,				/* dst_mask */
	 false),			/* pcrel_offset */

  /* 8-bit in-place setting, for local label subtraction.  */
  HOWTO (R_RISCV_SET8,			/* type */
	 0,				/* rightshift */
	 0,				/* size */
	 8,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_SET8",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 0xff,				/* dst_mask */
	 false),			/* pcrel_offset */

  /* 16-bit in-place setting, for local label subtraction.  */
  HOWTO (R_RISCV_SET16,			/* type */
	 0,				/* rightshift */
	 1,				/* size */
	 16,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_SET16",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 0xffff,			/* dst_mask */
	 false),			/* pcrel_offset */

  /* 32-bit in-place setting, for local label subtraction.  */
  HOWTO (R_RISCV_SET32,			/* type */
	 0,				/* rightshift */
	 2,				/* size */
	 32,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_SET32",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 0xffffffff,			/* dst_mask */
	 false),			/* pcrel_offset */

  /* 32-bit PC relative.  */
  HOWTO (R_RISCV_32_PCREL,		/* type */
	 0,				/* rightshift */
	 2,				/* size */
	 32,				/* bitsize */
	 true,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_32_PCREL",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 0xffffffff,			/* dst_mask */
	 false),			/* pcrel_offset */

  /* Relocation against a local ifunc symbol in a shared object.  */
  HOWTO (R_RISCV_IRELATIVE,		/* type */
	 0,				/* rightshift */
	 2,				/* size */
	 32,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_IRELATIVE",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 0xffffffff,			/* dst_mask */
	 false),			/* pcrel_offset */
};

/* A mapping from BFD reloc types to RISC-V ELF reloc types.  */
struct elf_reloc_map
{
  bfd_reloc_code_real_type bfd_val;
  enum elf_riscv_reloc_type elf_val;
};

static const struct elf_reloc_map riscv_reloc_map[] =
{
  { BFD_RELOC_NONE, R_RISCV_NONE },
  { BFD_RELOC_32, R_RISCV_32 },
  { BFD_RELOC_64, R_RISCV_64 },
  { BFD_RELOC_RISCV_ADD8, R_RISCV_ADD8 },
  { BFD_RELOC_RISCV_ADD16, R_RISCV_ADD16 },
  { BFD_RELOC_RISCV_ADD32, R_RISCV_ADD32 },
  { BFD_RELOC_RISCV_ADD64, R_RISCV_ADD64 },
  { BFD_RELOC_RISCV_SUB8, R_RISCV_SUB8 },
  { BFD_RELOC_RISCV_SUB16, R_RISCV_SUB16 },
  { BFD_RELOC_RISCV_SUB32, R_RISCV_SUB32 },
  { BFD_RELOC_RISCV_SUB64, R_RISCV_SUB64 },
  { BFD_RELOC_CTOR, R_RISCV_64 },
  { BFD_RELOC_12_PCREL, R_RISCV_BRANCH },
  { BFD_RELOC_RISCV_HI20, R_RISCV_HI20 },
  { BFD_RELOC_RISCV_LO12_I, R_RISCV_LO12_I },
  { BFD_RELOC_RISCV_LO12_S, R_RISCV_LO12_S },
  { BFD_RELOC_RISCV_PCREL_LO12_I, R_RISCV_PCREL_LO12_I },
  { BFD_RELOC_RISCV_PCREL_LO12_S, R_RISCV_PCREL_LO12_S },
  { BFD_RELOC_RISCV_CALL, R_RISCV_CALL },
  { BFD_RELOC_RISCV_CALL_PLT, R_RISCV_CALL_PLT },
  { BFD_RELOC_RISCV_PCREL_HI20, R_RISCV_PCREL_HI20 },
  { BFD_RELOC_RISCV_JMP, R_RISCV_JAL },
  { BFD_RELOC_RISCV_GOT_HI20, R_RISCV_GOT_HI20 },
  { BFD_RELOC_RISCV_TLS_DTPMOD32, R_RISCV_TLS_DTPMOD32 },
  { BFD_RELOC_RISCV_TLS_DTPREL32, R_RISCV_TLS_DTPREL32 },
  { BFD_RELOC_RISCV_TLS_DTPMOD64, R_RISCV_TLS_DTPMOD64 },
  { BFD_RELOC_RISCV_TLS_DTPREL64, R_RISCV_TLS_DTPREL64 },
  { BFD_RELOC_RISCV_TLS_TPREL32, R_RISCV_TLS_TPREL32 },
  { BFD_RELOC_RISCV_TLS_TPREL64, R_RISCV_TLS_TPREL64 },
  { BFD_RELOC_RISCV_TPREL_HI20, R_RISCV_TPREL_HI20 },
  { BFD_RELOC_RISCV_TPREL_ADD, R_RISCV_TPREL_ADD },
  { BFD_RELOC_RISCV_TPREL_LO12_S, R_RISCV_TPREL_LO12_S },
  { BFD_RELOC_RISCV_TPREL_LO12_I, R_RISCV_TPREL_LO12_I },
  { BFD_RELOC_RISCV_TLS_GOT_HI20, R_RISCV_TLS_GOT_HI20 },
  { BFD_RELOC_RISCV_TLS_GD_HI20, R_RISCV_TLS_GD_HI20 },
  { BFD_RELOC_RISCV_ALIGN, R_RISCV_ALIGN },
  { BFD_RELOC_RISCV_RVC_BRANCH, R_RISCV_RVC_BRANCH },
  { BFD_RELOC_RISCV_RVC_JUMP, R_RISCV_RVC_JUMP },
  { BFD_RELOC_RISCV_RVC_LUI, R_RISCV_RVC_LUI },
  { BFD_RELOC_RISCV_GPREL_I, R_RISCV_GPREL_I },
  { BFD_RELOC_RISCV_GPREL_S, R_RISCV_GPREL_S },
  { BFD_RELOC_RISCV_TPREL_I, R_RISCV_TPREL_I },
  { BFD_RELOC_RISCV_TPREL_S, R_RISCV_TPREL_S },
  { BFD_RELOC_RISCV_RELAX, R_RISCV_RELAX },
  { BFD_RELOC_RISCV_SUB6, R_RISCV_SUB6 },
  { BFD_RELOC_RISCV_SET6, R_RISCV_SET6 },
  { BFD_RELOC_RISCV_SET8, R_RISCV_SET8 },
  { BFD_RELOC_RISCV_SET16, R_RISCV_SET16 },
  { BFD_RELOC_RISCV_SET32, R_RISCV_SET32 },
  { BFD_RELOC_RISCV_32_PCREL, R_RISCV_32_PCREL },
};

/* Given a BFD reloc type, return a howto structure.  */

reloc_howto_type *
riscv_reloc_type_lookup (bfd *abfd ATTRIBUTE_UNUSED,
			 bfd_reloc_code_real_type code)
{
  unsigned int i;

  for (i = 0; i < ARRAY_SIZE (riscv_reloc_map); i++)
    if (riscv_reloc_map[i].bfd_val == code)
      return &howto_table[(int) riscv_reloc_map[i].elf_val];

  bfd_set_error (bfd_error_bad_value);
  return NULL;
}

reloc_howto_type *
riscv_reloc_name_lookup (bfd *abfd ATTRIBUTE_UNUSED, const char *r_name)
{
  unsigned int i;

  for (i = 0; i < ARRAY_SIZE (howto_table); i++)
    if (howto_table[i].name && strcasecmp (howto_table[i].name, r_name) == 0)
      return &howto_table[i];

  return NULL;
}

reloc_howto_type *
riscv_elf_rtype_to_howto (bfd *abfd, unsigned int r_type)
{
  if (r_type >= ARRAY_SIZE (howto_table))
    {
      (*_bfd_error_handler) (_("%pB: unsupported relocation type %#x"),
			     abfd, r_type);
      bfd_set_error (bfd_error_bad_value);
      return NULL;
    }
  return &howto_table[r_type];
}

/* Special_function of RISCV_ADD and RISCV_SUB relocations.  */

static bfd_reloc_status_type
riscv_elf_add_sub_reloc (bfd *abfd,
			 arelent *reloc_entry,
			 asymbol *symbol,
			 void *data,
			 asection *input_section,
			 bfd *output_bfd,
			 char **error_message ATTRIBUTE_UNUSED)
{
  reloc_howto_type *howto = reloc_entry->howto;
  bfd_vma relocation;

  if (output_bfd != NULL
      && (symbol->flags & BSF_SECTION_SYM) == 0
      && (!reloc_entry->howto->partial_inplace || reloc_entry->addend == 0))
    {
      reloc_entry->address += input_section->output_offset;
      return bfd_reloc_ok;
    }

  if (output_bfd != NULL)
    return bfd_reloc_continue;

  relocation = symbol->value + symbol->section->output_section->vma
    + symbol->section->output_offset + reloc_entry->addend;
  bfd_vma old_value = bfd_get (howto->bitsize, abfd,
			       data + reloc_entry->address);

  switch (howto->type)
    {
    case R_RISCV_ADD8:
    case R_RISCV_ADD16:
    case R_RISCV_ADD32:
    case R_RISCV_ADD64:
      relocation = old_value + relocation;
      break;
    case R_RISCV_SUB6:
    case R_RISCV_SUB8:
    case R_RISCV_SUB16:
    case R_RISCV_SUB32:
    case R_RISCV_SUB64:
      relocation = old_value - relocation;
      break;
    }
  bfd_put (howto->bitsize, abfd, relocation, data + reloc_entry->address);

  return bfd_reloc_ok;
}

/* Always add the IMPLICIT for the SUBSET.  */

static bool
check_implicit_always (const char *implicit ATTRIBUTE_UNUSED,
		       riscv_subset_t *subset ATTRIBUTE_UNUSED)
{
  return true;
}

/* Add the IMPLICIT only when the version of SUBSET less than 2.1.  */

static bool
check_implicit_for_i (const char *implicit ATTRIBUTE_UNUSED,
		      riscv_subset_t *subset)
{
  return (subset->major_version < 2
	  || (subset->major_version == 2
	      && subset->minor_version < 1));
}

/* Record all implicit information for the subsets.  */
struct riscv_implicit_subset
{
  const char *subset_name;
  const char *implicit_name;
  /* A function to determine if we need to add the implicit subset.  */
  bool (*check_func) (const char *, riscv_subset_t *);
};
static struct riscv_implicit_subset riscv_implicit_subsets[] =
{
  {"e", "i",		check_implicit_always},
  {"i", "zicsr",	check_implicit_for_i},
  {"i", "zifencei",	check_implicit_for_i},
  {"g", "zicsr",	check_implicit_always},
  {"g", "zifencei",	check_implicit_always},
  {"q", "d",		check_implicit_always},
  {"d", "f",		check_implicit_always},
  {"f", "zicsr",	check_implicit_always},
  {NULL, NULL, NULL}
};

/* Lists of prefixed class extensions that binutils should know about.
   Whether or not a particular entry is in these lists will dictate if
   gas/ld will accept its presence in the architecture string.

   Please add the extensions to the lists in lower case.  However, keep
   these subsets in alphabetical order in these tables is recommended,
   although there is no impact on the current implementation.  */

static const char * const riscv_std_z_ext_strtab[] =
{
  "zba", "zbb", "zbc", "zicsr", "zifencei", "zihintpause", NULL
};

static const char * const riscv_std_s_ext_strtab[] =
{
  NULL
};

static const char * const riscv_std_h_ext_strtab[] =
{
  NULL
};

static const char * const riscv_std_zxm_ext_strtab[] =
{
  NULL
};

/* ISA extension prefixed name class.  Must define them in parsing order.  */
enum riscv_prefix_ext_class
{
  RV_ISA_CLASS_Z = 1,
  RV_ISA_CLASS_S,
  RV_ISA_CLASS_H,
  RV_ISA_CLASS_ZXM,
  RV_ISA_CLASS_X,
  RV_ISA_CLASS_UNKNOWN
};

/* Record the strings of the prefixed extensions, and their corresponding
   classes.  The more letters of the prefix string, the more forward it must
   be defined.  Otherwise, the riscv_get_prefix_class will map it to the
   wrong classes.  */
struct riscv_parse_prefix_config
{
  /* Class of the extension. */
  enum riscv_prefix_ext_class class;

  /* Prefix string for error printing and internal parser usage.  */
  const char *prefix;
};
static const struct riscv_parse_prefix_config parse_config[] =
{
  {RV_ISA_CLASS_ZXM, "zxm"},
  {RV_ISA_CLASS_Z, "z"},
  {RV_ISA_CLASS_S, "s"},
  {RV_ISA_CLASS_H, "h"},
  {RV_ISA_CLASS_X, "x"},
  {RV_ISA_CLASS_UNKNOWN, NULL}
};

/* Get the prefixed name class for the extensions, the class also
   means the order of the prefixed extensions.  */

static enum riscv_prefix_ext_class
riscv_get_prefix_class (const char *arch)
{
  int i = 0;
  while (parse_config[i].class != RV_ISA_CLASS_UNKNOWN)
    {
      if (strncmp (arch, parse_config[i].prefix,
		   strlen (parse_config[i].prefix)) == 0)
	return parse_config[i].class;
      i++;
    }
  return RV_ISA_CLASS_UNKNOWN;
}

/* Check KNOWN_EXTS to see if the EXT is supported.  */

static bool
riscv_known_prefixed_ext (const char *ext,
			  const char *const *known_exts)
{
  size_t i;
  for (i = 0; known_exts[i]; ++i)
    if (strcmp (ext, known_exts[i]) == 0)
      return true;
  return false;
}

/* Check whether the prefixed extension is valid or not.  Return
   true if valid, otehrwise return false.  */

static bool
riscv_valid_prefixed_ext (const char *ext)
{
  enum riscv_prefix_ext_class class = riscv_get_prefix_class (ext);
  switch (class)
  {
  case RV_ISA_CLASS_Z:
    return riscv_known_prefixed_ext (ext, riscv_std_z_ext_strtab);
  case RV_ISA_CLASS_ZXM:
    return riscv_known_prefixed_ext (ext, riscv_std_zxm_ext_strtab);
  case RV_ISA_CLASS_S:
    return riscv_known_prefixed_ext (ext, riscv_std_s_ext_strtab);
  case RV_ISA_CLASS_H:
    return riscv_known_prefixed_ext (ext, riscv_std_h_ext_strtab);
  case RV_ISA_CLASS_X:
    /* Only the single x is invalid.  */
    if (strcmp (ext, "x") != 0)
      return true;
  default:
    break;
  }
  return false;
}

/* Array is used to compare the orders of standard extensions quickly.  */
static int riscv_ext_order[26] = {0};

/* Init the riscv_ext_order array.  */

static void
riscv_init_ext_order (void)
{
  static bool inited = false;
  const char *std_base_exts = "eig";
  const char *std_remain_exts = riscv_supported_std_ext ();
  const char *ext;
  int order;

  if (inited)
    return;

  /* The orders of all standard extensions are positive.  */
  order = 1;

  /* Init the standard base extensions first.  */
  for (ext = std_base_exts; *ext; ext++)
    riscv_ext_order[(*ext - 'a')] = order++;

  /* Init the standard remaining extensions.  */
  for (ext = std_remain_exts; *ext; ext++)
    riscv_ext_order[(*ext - 'a')] = order++;

  /* Some of the prefixed keyword are not single letter, so we set
     their prefixed orders in the riscv_compare_subsets directly,
     not through the riscv_ext_order.  */

  inited = true;
}

/* Similar to the strcmp.  It returns an integer less than, equal to,
   or greater than zero if `subset2` is found, respectively, to be less
   than, to match, or be greater than `subset1`.

   The order values,
   Zero: Preserved keywords.
   Positive number: Standard extensions.
   Negative number: Prefixed keywords.  */

int
riscv_compare_subsets (const char *subset1, const char *subset2)
{
  int order1 = riscv_ext_order[(*subset1 - 'a')];
  int order2 = riscv_ext_order[(*subset2 - 'a')];

  /* Compare the standard extension first.  */
  if (order1 > 0 && order2 > 0)
    return order1 - order2;

  /* Set the prefixed orders to negative numbers.  */
  enum riscv_prefix_ext_class class1 = riscv_get_prefix_class (subset1);
  enum riscv_prefix_ext_class class2 = riscv_get_prefix_class (subset2);

  if (class1 != RV_ISA_CLASS_UNKNOWN)
    order1 = - (int) class1;
  if (class2 != RV_ISA_CLASS_UNKNOWN)
    order2 = - (int) class2;

  if (order1 == order2)
    {
      /* Compare the standard addition z extensions.  */
      if (class1 == RV_ISA_CLASS_Z)
	{
	  order1 = riscv_ext_order[(*++subset1 - 'a')];
	  order2 = riscv_ext_order[(*++subset2 - 'a')];
	  if (order1 != order2)
	    return order1 - order2;
	}
      return strcasecmp (++subset1, ++subset2);
    }

  return order2 - order1;
}

/* Find subset in the list.  Return TRUE and set `current` to the subset
   if it is found.  Otherwise, return FALSE and set `current` to the place
   where we should insert the subset.  However, return FALSE with the NULL
   `current` means we should insert the subset at the head of subset list,
   if needed.  */

bool
riscv_lookup_subset (const riscv_subset_list_t *subset_list,
		     const char *subset,
		     riscv_subset_t **current)
{
  riscv_subset_t *s, *pre_s = NULL;

  for (s = subset_list->head;
       s != NULL;
       pre_s = s, s = s->next)
    {
      int cmp = riscv_compare_subsets (s->name, subset);
      if (cmp == 0)
	{
	  *current = s;
	  return true;
	}
      else if (cmp > 0)
	break;
    }
  *current = pre_s;
  return false;
}

/* Add extension from ISA string to the last of the subset list.  */

void
riscv_add_subset (riscv_subset_list_t *subset_list,
		  const char *subset,
		  int major,
		  int minor)
{
  riscv_subset_t *s = xmalloc (sizeof *s);

  if (subset_list->head == NULL)
    subset_list->head = s;

  s->name = xstrdup (subset);
  s->major_version = major;
  s->minor_version = minor;
  s->next = NULL;

  if (subset_list->tail != NULL)
    subset_list->tail->next = s;
  subset_list->tail = s;
}

/* Add the implicit extension to the subset list.  Search the
   list first, and then find the right place to add.  */

static void
riscv_add_implicit_subset (riscv_subset_list_t *subset_list,
			   const char *subset,
			   int major,
			   int minor)
{
  riscv_subset_t *current, *new;

  if (riscv_lookup_subset (subset_list, subset, &current))
    return;

  new = xmalloc (sizeof *new);
  new->name = xstrdup (subset);
  new->major_version = major;
  new->minor_version = minor;
  new->next = NULL;

  if (current != NULL)
    {
      new->next = current->next;
      current->next = new;
    }
  else
    {
      new->next = subset_list->head;
      subset_list->head = new;
    }
}

/* We have to add all extensions from ISA string first, and then start to
   add their implicit extensions.  The extensions from ISA string must be
   set in order, so we can add them to the last of the subset list
   directly, without searching.

   Find the default versions for the extension before adding them to
   the subset list, if their versions are RISCV_UNKNOWN_VERSION.
   Afterwards, report errors if we can not find their default versions.  */

static void
riscv_parse_add_subset (riscv_parse_subset_t *rps,
			const char *subset,
			int major,
			int minor,
			bool implicit)
{
  int major_version = major;
  int minor_version = minor;

  if ((major_version == RISCV_UNKNOWN_VERSION
       || minor_version == RISCV_UNKNOWN_VERSION)
      && rps->get_default_version != NULL)
    rps->get_default_version (subset, &major_version, &minor_version);

  /* We don't care the versions of the implicit extensions.  */
  if (!implicit
      && (major_version == RISCV_UNKNOWN_VERSION
	  || minor_version == RISCV_UNKNOWN_VERSION))
    {
      if (subset[0] == 'x')
	rps->error_handler
	  (_("x ISA extension `%s' must be set with the versions"),
	   subset);
      else
	rps->error_handler
	  (_("cannot find default versions of the ISA extension `%s'"),
	   subset);
      return;
    }

  if (!implicit)
    riscv_add_subset (rps->subset_list, subset,
		      major_version, minor_version);
  else
    riscv_add_implicit_subset (rps->subset_list, subset,
			       major_version, minor_version);
}

/* Release subset list.  */

void
riscv_release_subset_list (riscv_subset_list_t *subset_list)
{
   while (subset_list->head != NULL)
    {
      riscv_subset_t *next = subset_list->head->next;
      free ((void *)subset_list->head->name);
      free (subset_list->head);
      subset_list->head = next;
    }

  subset_list->tail = NULL;
}

/* Parsing extension version.

   Return Value:
     Points to the end of version

   Arguments:
     `rps`: Hooks and status for parsing extensions.
     `march`: Full ISA string.
     `p`: Curent parsing position.
     `major_version`: Parsed major version.
     `minor_version`: Parsed minor version.
     `std_ext_p`: True if parsing standard extension.  */

static const char *
riscv_parsing_subset_version (riscv_parse_subset_t *rps,
			      const char *march,
			      const char *p,
			      int *major_version,
			      int *minor_version,
			      bool std_ext_p)
{
  bool major_p = true;
  int version = 0;
  char np;

  *major_version = 0;
  *minor_version = 0;
  for (; *p; ++p)
    {
      if (*p == 'p')
	{
	  np = *(p + 1);

	  if (!ISDIGIT (np))
	    {
	      /* Might be beginning of `p` extension.  */
	      if (std_ext_p)
		{
		  *major_version = version;
		  *minor_version = 0;
		  return p;
		}
	      else
		{
		  rps->error_handler
		    (_("-march=%s: expect number after `%dp'"),
		     march, version);
		  return NULL;
		}
	    }

	  *major_version = version;
	  major_p = false;
	  version = 0;
	}
      else if (ISDIGIT (*p))
	version = (version * 10) + (*p - '0');
      else
	break;
    }

  if (major_p)
    *major_version = version;
  else
    *minor_version = version;

  /* We can not find any version in string.  */
  if (*major_version == 0 && *minor_version == 0)
    {
      *major_version = RISCV_UNKNOWN_VERSION;
      *minor_version = RISCV_UNKNOWN_VERSION;
    }

  return p;
}

/* Return string which contain all supported standard extensions in
   canonical order.  */

const char *
riscv_supported_std_ext (void)
{
  return "mafdqlcbjtpvn";
}

/* Parsing function for standard extensions.

   Return Value:
     Points to the end of extensions.

   Arguments:
     `rps`: Hooks and status for parsing extensions.
     `march`: Full ISA string.
     `p`: Curent parsing position.  */

static const char *
riscv_parse_std_ext (riscv_parse_subset_t *rps,
		     const char *march,
		     const char *p)
{
  const char *all_std_exts = riscv_supported_std_ext ();
  const char *std_exts = all_std_exts;
  int major_version;
  int minor_version;
  char subset[2] = {0, 0};

  /* First letter must start with i, e or g.  */
  switch (*p)
    {
      case 'i':
	p = riscv_parsing_subset_version (rps, march, ++p,
					  &major_version,
					  &minor_version, true);
	riscv_parse_add_subset (rps, "i",
				major_version,
				minor_version, false);
	break;

      case 'e':
	p = riscv_parsing_subset_version (rps, march, ++p,
					  &major_version,
					  &minor_version, true);
	riscv_parse_add_subset (rps, "e",
				major_version,
				minor_version, false);
	if (*rps->xlen > 32)
	  {
	    rps->error_handler
	      (_("-march=%s: rv%de is not a valid base ISA"),
	       march, *rps->xlen);
	    return NULL;
	  }
	break;

      case 'g':
	p = riscv_parsing_subset_version (rps, march, ++p,
					  &major_version,
					  &minor_version, true);
	/* Expand g to imafd.  */
	riscv_parse_add_subset (rps, "i",
				RISCV_UNKNOWN_VERSION,
				RISCV_UNKNOWN_VERSION, false);
	for ( ; *std_exts != 'q'; std_exts++)
	  {
	    subset[0] = *std_exts;
	    riscv_parse_add_subset (rps, subset,
				    RISCV_UNKNOWN_VERSION,
				    RISCV_UNKNOWN_VERSION, false);
	  }
	/* Add g as an implicit extension.  */
	riscv_parse_add_subset (rps, "g",
				RISCV_UNKNOWN_VERSION,
				RISCV_UNKNOWN_VERSION, true);
	break;

      default:
	rps->error_handler
	  (_("-march=%s: first ISA extension must be `e', `i' or `g'"),
	   march);
	return NULL;
    }

  while (p != NULL && *p != '\0')
    {
      /* Stop when we parsed the known prefix class.  */
      enum riscv_prefix_ext_class class = riscv_get_prefix_class (p);
      if (class != RV_ISA_CLASS_UNKNOWN)
	break;

      if (*p == '_')
	{
	  p++;
	  continue;
	}

      /* Checking canonical order.  */
      char std_ext = *p;
      while (*std_exts && std_ext != *std_exts)
	std_exts++;

      if (std_ext != *std_exts)
	{
	  if (riscv_ext_order[(std_ext - 'a')] == 0)
	    rps->error_handler
	      (_("-march=%s: unknown standard and prefixed ISA "
		 "extension `%s'"), march, p);
	  else
	    rps->error_handler
	      (_("-march=%s: standard ISA extension `%c' is not "
		 "in canonical order"), march, std_ext);
	  return NULL;
	}

      std_exts++;
      subset[0] = std_ext;
      p = riscv_parsing_subset_version (rps, march, ++p,
					&major_version,
					&minor_version, true);
      riscv_parse_add_subset (rps, subset,
			      major_version,
			      minor_version, false);
    }

  return p;
}

/* Parsing function for prefixed extensions.

   Return Value:
     Points to the end of extension.

   Arguments:
     `rps`: Hooks and status for parsing extensions.
     `march`: Full ISA string.
     `p`: Curent parsing position.  */

static const char *
riscv_parse_prefixed_ext (riscv_parse_subset_t *rps,
			  const char *march,
			  const char *p)
{
  int major_version;
  int minor_version;
  const char *last_name;
  enum riscv_prefix_ext_class class;

  while (*p)
    {
      if (*p == '_')
	{
	  p++;
	  continue;
	}

      class = riscv_get_prefix_class (p);
      if (class == RV_ISA_CLASS_UNKNOWN)
	{
	  rps->error_handler
	    (_("-march=%s: unknown prefix class for the ISA extension `%s'"),
	     march, p);
	  return NULL;
	}

      char *subset = xstrdup (p);
      char *q = subset;
      const char *end_of_version;

      while (*++q != '\0' && *q != '_' && !ISDIGIT (*q))
	;

      end_of_version =
	riscv_parsing_subset_version (rps, march, q,
				      &major_version,
				      &minor_version, false);
      *q = '\0';

      if (end_of_version == NULL)
	{
	  free (subset);
	  return NULL;
	}

      /* Check if the prefix extension is known.
	 For 'x', anything goes but it cannot simply be 'x'.
	 For other prefixed extensions, it must be known from a list
	 and cannot simply be the prefixed name.  */

      /* Check that the extension name is well-formed.  */
      if (rps->check_unknown_prefixed_ext
	  && !riscv_valid_prefixed_ext (subset))
	{
	  rps->error_handler
	    (_("-march=%s: unknown prefixed ISA extension `%s'"),
	     march, subset);
	  free (subset);
	  return NULL;
	}

      /* Check that the extension isn't duplicate.  */
      last_name = rps->subset_list->tail->name;
      if (!strcasecmp (last_name, subset))
	{
	  rps->error_handler
	    (_("-march=%s: duplicate prefixed ISA extension `%s'"),
	     march, subset);
	  free (subset);
	  return NULL;
	}

      /* Check that the extension is in expected order.  */
      if (riscv_compare_subsets (last_name, subset) > 0)
	{
	  rps->error_handler
	    (_("-march=%s: prefixed ISA extension `%s' is not in expected "
	       "order.  It must come before `%s'"),
	     march, subset, last_name);
	  free (subset);
	  return NULL;
	}

      riscv_parse_add_subset (rps, subset,
			      major_version,
			      minor_version, false);
      p += end_of_version - subset;
      free (subset);

      if (*p != '\0' && *p != '_')
	{
	  rps->error_handler
	    (_("-march=%s: prefixed ISA extension must separate with _"),
	     march);
	  return NULL;
	}
    }

  return p;
}

/* Add the implicit extensions.  */

static void
riscv_parse_add_implicit_subsets (riscv_parse_subset_t *rps)
{
  struct riscv_implicit_subset *t = riscv_implicit_subsets;
  for (; t->subset_name; t++)
    {
      riscv_subset_t *subset = NULL;
      if (riscv_lookup_subset (rps->subset_list, t->subset_name, &subset)
	  && t->check_func (t->implicit_name, subset))
	riscv_parse_add_subset (rps, t->implicit_name,
				RISCV_UNKNOWN_VERSION,
				RISCV_UNKNOWN_VERSION, true);
    }
}

/* Function for parsing ISA string.

   Return Value:
     Return TRUE on success.

   Arguments:
     `rps`: Hooks and status for parsing extensions.
     `arch`: Full ISA string.  */

bool
riscv_parse_subset (riscv_parse_subset_t *rps,
		    const char *arch)
{
  riscv_subset_t *subset = NULL;
  const char *p;
  bool no_conflict = true;

  for (p = arch; *p != '\0'; p++)
    {
      if (ISUPPER (*p))
	{
	  rps->error_handler
	    (_("-march=%s: ISA string cannot contain uppercase letters"),
	     arch);
	  return false;
	}
    }

  p = arch;
  if (startswith (p, "rv32"))
    {
      *rps->xlen = 32;
      p += 4;
    }
  else if (startswith (p, "rv64"))
    {
      *rps->xlen = 64;
      p += 4;
    }
  else
    {
      /* ISA string shouldn't be NULL or empty here.  However,
	 it might be empty only when we failed to merge the ISA
	 string in the riscv_merge_attributes.  We have already
	 issued the correct error message in another side, so do
	 not issue this error when the ISA string is empty.  */
      if (strlen (arch))
	rps->error_handler (
	  _("-march=%s: ISA string must begin with rv32 or rv64"),
	  arch);
      return false;
    }

  /* Init the riscv_ext_order array to compare the order of extensions
     quickly.  */
  riscv_init_ext_order ();

  /* Parsing standard extension.  */
  p = riscv_parse_std_ext (rps, arch, p);

  if (p == NULL)
    return false;

  /* Parse the different classes of extensions in the specified order.  */
  while (*p != '\0')
    {
      p = riscv_parse_prefixed_ext (rps, arch, p);

      if (p == NULL)
        return false;
    }

  /* Finally add implicit extensions according to the current
     extensions.  */
  riscv_parse_add_implicit_subsets (rps);

  /* Check the conflicts.  */
  if (riscv_lookup_subset (rps->subset_list, "e", &subset)
      && riscv_lookup_subset (rps->subset_list, "f", &subset))
    {
      rps->error_handler
	(_("-march=%s: rv32e does not support the `f' extension"),
	 arch);
      no_conflict = false;
    }
  if (riscv_lookup_subset (rps->subset_list, "q", &subset)
      && *rps->xlen < 64)
    {
      rps->error_handler
	(_("-march=%s: rv32 does not support the `q' extension"),
	 arch);
      no_conflict = false;
    }
  return no_conflict;
}

/* Return the number of digits for the input.  */

size_t
riscv_estimate_digit (unsigned num)
{
  size_t digit = 0;
  if (num == 0)
    return 1;

  for (digit = 0; num ; num /= 10)
    digit++;

  return digit;
}

/* Auxiliary function to estimate string length of subset list.  */

static size_t
riscv_estimate_arch_strlen1 (const riscv_subset_t *subset)
{
  if (subset == NULL)
    return 6; /* For rv32/rv64/rv128 and string terminator.  */

  return riscv_estimate_arch_strlen1 (subset->next)
	 + strlen (subset->name)
	 + riscv_estimate_digit (subset->major_version)
	 + 1 /* For version seperator 'p'.  */
	 + riscv_estimate_digit (subset->minor_version)
	 + 1 /* For underscore.  */;
}

/* Estimate the string length of this subset list.  */

static size_t
riscv_estimate_arch_strlen (const riscv_subset_list_t *subset_list)
{
  return riscv_estimate_arch_strlen1 (subset_list->head);
}

/* Auxiliary function to convert subset info to string.  */

static void
riscv_arch_str1 (riscv_subset_t *subset,
		 char *attr_str, char *buf, size_t bufsz)
{
  const char *underline = "_";
  riscv_subset_t *subset_t = subset;

  if (subset_t == NULL)
    return;

  /* No underline between rvXX and i/e.  */
  if ((strcasecmp (subset_t->name, "i") == 0)
      || (strcasecmp (subset_t->name, "e") == 0))
    underline = "";

  snprintf (buf, bufsz, "%s%s%dp%d",
	    underline,
	    subset_t->name,
	    subset_t->major_version,
	    subset_t->minor_version);

  strncat (attr_str, buf, bufsz);

  /* Skip 'i' extension after 'e', or skip extensions which
     versions are unknown.  */
  while (subset_t->next
	 && ((strcmp (subset_t->name, "e") == 0
	      && strcmp (subset_t->next->name, "i") == 0)
	     || subset_t->next->major_version == RISCV_UNKNOWN_VERSION
	     || subset_t->next->minor_version == RISCV_UNKNOWN_VERSION))
    subset_t = subset_t->next;

  riscv_arch_str1 (subset_t->next, attr_str, buf, bufsz);
}

/* Convert subset information into string with explicit versions.  */

char *
riscv_arch_str (unsigned xlen, const riscv_subset_list_t *subset)
{
  size_t arch_str_len = riscv_estimate_arch_strlen (subset);
  char *attr_str = xmalloc (arch_str_len);
  char *buf = xmalloc (arch_str_len);

  snprintf (attr_str, arch_str_len, "rv%u", xlen);

  riscv_arch_str1 (subset->head, attr_str, buf, arch_str_len);
  free (buf);

  return attr_str;
}
