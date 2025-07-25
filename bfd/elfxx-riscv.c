/* RISC-V-specific support for ELF.
   Copyright (C) 2011-2025 Free Software Foundation, Inc.

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

#define MINUS_ONE ((bfd_vma)0 - 1)

/* Special handler for ADD/SUB relocations that allows them to be filled out
   both in the pre-linked and post-linked file.  This is necessary to make
   pre-linked debug info work, as due to linker relaxations we need to emit
   relocations for the debug info.  */
static bfd_reloc_status_type riscv_elf_add_sub_reloc
  (bfd *, arelent *, asymbol *, void *, asection *, bfd *, char **);
static bfd_reloc_status_type riscv_elf_ignore_reloc
  (bfd *, arelent *, asymbol *, void *, asection *, bfd *, char **);

/* The relocation table used for SHT_RELA sections.  */

static reloc_howto_type howto_table[] =
{
  /* No relocation.  */
  HOWTO (R_RISCV_NONE,			/* type */
	 0,				/* rightshift */
	 0,				/* size */
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
	 4,				/* size */
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
	 8,				/* size */
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
	 4,				/* size */
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
	 8,				/* size */
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
	 4,				/* size */
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
	 8,				/* size */
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
	 4,				/* size */
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
	 8,				/* size */
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
	 4,				/* size */
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
	 8,				/* size */
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

  HOWTO (R_RISCV_TLSDESC,		/* type */
	 0,				/* rightshift */
	 0,				/* size is handled by dynamic linker */
	 0,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_TLSDESC",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 MINUS_ONE,			/* dst_mask */
	 false),			/* pcrel_offset */

  EMPTY_HOWTO (13),
  EMPTY_HOWTO (14),
  EMPTY_HOWTO (15),

  /* 12-bit PC-relative branch offset.  */
  HOWTO (R_RISCV_BRANCH,		/* type */
	 0,				/* rightshift */
	 4,				/* size */
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
	 4,				/* size */
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
	 8,				/* size */
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
	 8,				/* size */
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
	 4,				/* size */
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
	 4,				/* size */
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
	 4,				/* size */
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
	 4,				/* size */
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
	 4,				/* size */
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
	 4,				/* size */
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
	 4,				/* size */
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
	 4,				/* size */
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
	 4,				/* size */
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
	 4,				/* size */
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
	 4,				/* size */
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
	 4,				/* size */
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
	 0,				/* size */
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
	 1,				/* size */
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
	 2,				/* size */
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
	 4,				/* size */
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
	 8,				/* size */
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
	 1,				/* size */
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
	 2,				/* size */
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
	 4,				/* size */
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
	 8,				/* size */
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

  EMPTY_HOWTO (41),
  EMPTY_HOWTO (42),

  /* Indicates an alignment statement.  The addend field encodes how many
     bytes of NOPs follow the statement.  The desired alignment is the
     addend rounded up to the next power of two.  */
  HOWTO (R_RISCV_ALIGN,			/* type */
	 0,				/* rightshift */
	 0,				/* size */
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
	 2,				/* size */
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
	 2,				/* size */
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

  EMPTY_HOWTO (46),
  EMPTY_HOWTO (47),
  EMPTY_HOWTO (48),
  EMPTY_HOWTO (49),
  EMPTY_HOWTO (50),

  /* The paired relocation may be relaxed.  */
  HOWTO (R_RISCV_RELAX,			/* type */
	 0,				/* rightshift */
	 0,				/* size */
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
	 1,				/* size */
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
	 1,				/* size */
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
	 1,				/* size */
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
	 2,				/* size */
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
	 4,				/* size */
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
	 4,				/* size */
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
	 4,				/* size */
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

  /* Reserved for R_RISCV_PLT32.  */
  EMPTY_HOWTO (59),

  /* N-bit in-place setting, for unsigned-leb128 local label subtraction.  */
  HOWTO (R_RISCV_SET_ULEB128,		/* type */
	 0,				/* rightshift */
	 0,				/* size */
	 0,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 riscv_elf_ignore_reloc,	/* special_function */
	 "R_RISCV_SET_ULEB128",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 0,				/* dst_mask */
	 false),			/* pcrel_offset */

  /* N-bit in-place addition, for unsigned-leb128 local label subtraction.  */
  HOWTO (R_RISCV_SUB_ULEB128,		/* type */
	 0,				/* rightshift */
	 0,				/* size */
	 0,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 riscv_elf_ignore_reloc,	/* special_function */
	 "R_RISCV_SUB_ULEB128",		/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 0,				/* dst_mask */
	 false),			/* pcrel_offset */

  HOWTO (R_RISCV_TLSDESC_HI20,		/* type */
	 0,				/* rightshift */
	 4,				/* size */
	 32,				/* bitsize */
	 true,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_TLSDESC_HI20",	/* name */
	 true,				/* partial_inplace */
	 0,				/* src_mask */
	 ENCODE_UTYPE_IMM (-1U),	/* dst_mask */
	 false),			/* pcrel_offset */

  HOWTO (R_RISCV_TLSDESC_LOAD_LO12,	/* type */
	 0,				/* rightshift */
	 4,				/* size */
	 32,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_TLSDESC_LOAD_LO12",	/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 ENCODE_ITYPE_IMM (-1U),	/* dst_mask */
	 false),			/* pcrel_offset */

  HOWTO (R_RISCV_TLSDESC_ADD_LO12,	/* type */
	 0,				/* rightshift */
	 4,				/* size */
	 32,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_TLSDESC_ADD_LO12",	/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 ENCODE_ITYPE_IMM (-1U),	/* dst_mask */
	 false), 			/* pcrel_offset */

  HOWTO (R_RISCV_TLSDESC_CALL,		/* type */
	 0,				/* rightshift */
	 0,				/* size */
	 0,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_RISCV_TLSDESC_CALL",	/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 ENCODE_ITYPE_IMM (-1U),	/* dst_mask */
	 false) 			/* pcrel_offset */
};

static reloc_howto_type howto_table_internal[] =
{
  /* R_RISCV_DELETE.  */
  EMPTY_HOWTO (0),

  /* High 6 bits of 18-bit absolute address.  */
  HOWTO (R_RISCV_RVC_LUI,		/* type */
	 0,				/* rightshift */
	 2,				/* size */
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
	 4,				/* size */
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
	 4,				/* size */
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
	 4,				/* size */
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
	 4,				/* size */
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
  { BFD_RELOC_RISCV_TLSDESC_HI20, R_RISCV_TLSDESC_HI20 },
  { BFD_RELOC_RISCV_TLSDESC_LOAD_LO12, R_RISCV_TLSDESC_LOAD_LO12 },
  { BFD_RELOC_RISCV_TLSDESC_ADD_LO12, R_RISCV_TLSDESC_ADD_LO12 },
  { BFD_RELOC_RISCV_TLSDESC_CALL, R_RISCV_TLSDESC_CALL },
  { BFD_RELOC_RISCV_ALIGN, R_RISCV_ALIGN },
  { BFD_RELOC_RISCV_RVC_BRANCH, R_RISCV_RVC_BRANCH },
  { BFD_RELOC_RISCV_RVC_JUMP, R_RISCV_RVC_JUMP },
  { BFD_RELOC_RISCV_RELAX, R_RISCV_RELAX },
  { BFD_RELOC_RISCV_SUB6, R_RISCV_SUB6 },
  { BFD_RELOC_RISCV_SET6, R_RISCV_SET6 },
  { BFD_RELOC_RISCV_SET8, R_RISCV_SET8 },
  { BFD_RELOC_RISCV_SET16, R_RISCV_SET16 },
  { BFD_RELOC_RISCV_SET32, R_RISCV_SET32 },
  { BFD_RELOC_RISCV_32_PCREL, R_RISCV_32_PCREL },
  { BFD_RELOC_RISCV_SET_ULEB128, R_RISCV_SET_ULEB128 },
  { BFD_RELOC_RISCV_SUB_ULEB128, R_RISCV_SUB_ULEB128 },
};

struct riscv_profiles
{
  const char *profile_name;
  const char *profile_string;
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
  if (r_type < ARRAY_SIZE (howto_table))
    return &howto_table[r_type];
  else if (r_type < R_RISCV_max + ARRAY_SIZE (howto_table_internal))
    return &howto_table_internal[r_type - R_RISCV_max];
  else
    {
      (*_bfd_error_handler) (_("%pB: unsupported relocation type %#x"),
			     abfd, r_type);
      bfd_set_error (bfd_error_bad_value);
      return NULL;
    }
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

  bfd_size_type octets = reloc_entry->address
    * bfd_octets_per_byte (abfd, input_section);
  if (!bfd_reloc_offset_in_range (reloc_entry->howto, abfd,
				  input_section, octets))
    return bfd_reloc_outofrange;

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
      relocation = (old_value & ~howto->dst_mask)
		   | (((old_value & howto->dst_mask) - relocation)
		      & howto->dst_mask);
      break;
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

/* Special handler for relocations which don't have to be relocated.
   This function just simply return bfd_reloc_ok.  */

static bfd_reloc_status_type
riscv_elf_ignore_reloc (bfd *abfd ATTRIBUTE_UNUSED,
			arelent *reloc_entry,
			asymbol *symbol ATTRIBUTE_UNUSED,
			void *data ATTRIBUTE_UNUSED,
			asection *input_section,
			bfd *output_bfd,
			char **error_message ATTRIBUTE_UNUSED)
{
  if (output_bfd != NULL)
    reloc_entry->address += input_section->output_offset;
  return bfd_reloc_ok;
}

/* Always add implicit extensions for the SUBSET.  */

static bool
check_implicit_always (riscv_parse_subset_t *rps ATTRIBUTE_UNUSED,
		       const riscv_subset_t *subset ATTRIBUTE_UNUSED)
{
  return true;
}

/* Add implicit extensions only when the version of SUBSET less than 2.1.  */

static bool
check_implicit_for_i (riscv_parse_subset_t *rps ATTRIBUTE_UNUSED,
		      const riscv_subset_t *subset ATTRIBUTE_UNUSED)
{
  return (subset->major_version < 2
	  || (subset->major_version == 2
	      && subset->minor_version < 1));
}

/* Add the IMPLICIT only when the 'f' extension is also available
   and XLEN is 32.  */

static bool
check_implicit_for_zcf (riscv_parse_subset_t *rps,
			const riscv_subset_t *subset ATTRIBUTE_UNUSED)
{
  return (rps != NULL
	  && rps->xlen != NULL
	  && *rps->xlen == 32
	  && riscv_subset_supports (rps, "f"));
}

/* Add the implicit only when 'd' extension is also available.  */

static bool
check_implicit_for_zcd (riscv_parse_subset_t *rps,
			const riscv_subset_t *subset ATTRIBUTE_UNUSED)
{
  return (rps != NULL
	  && riscv_subset_supports (rps, "d"));
}

/* Record all implicit information for the subsets.  */
struct riscv_implicit_subset
{
  const char *ext;
  const char *implicit_exts;
  /* A function to determine if we need to add the implicit subsets.  */
  bool (*check_func) (riscv_parse_subset_t *,
		      const riscv_subset_t *);
};
/* Please added in order since this table is only run once time.  */
static struct riscv_implicit_subset riscv_implicit_subsets[] =
{
  {"g", "+i,+m,+a,+f,+d,+zicsr,+zifencei", check_implicit_always},
  {"e", "+i", check_implicit_always},
  {"i", "+zicsr,+zifencei", check_implicit_for_i},
  {"zicntr", "+zicsr", check_implicit_always},
  {"zihpm", "+zicsr", check_implicit_always},

  {"m", "+zmmul", check_implicit_always},

  {"zabha", "+zaamo", check_implicit_always},
  {"zacas", "+zaamo", check_implicit_always},
  {"a", "+zaamo,+zalrsc", check_implicit_always},

  {"xsfvcp", "+zve32x", check_implicit_always},
  {"xsfvqmaccqoq", "+zve32x,+zvl256b", check_implicit_always},
  {"xsfvqmaccdod", "+zve32x,+zvl128b", check_implicit_always},
  {"xsfvfnrclipxfqf", "+zve32f", check_implicit_always},

  {"xtheadvector", "+zicsr", check_implicit_always},
  {"xtheadzvamo", "+zaamo", check_implicit_always},

  {"v", "+zve64d,+zvl128b", check_implicit_always},
  {"zvfh", "+zvfhmin,+zfhmin", check_implicit_always},
  {"zvfhmin", "+zve32f", check_implicit_always},
  {"zvfbfwma", "+zve32f,+zfbfmin", check_implicit_always},
  {"zvfbfmin", "+zve32f", check_implicit_always},
  {"zve64d", "+d,+zve64f", check_implicit_always},
  {"zve64f", "+zve32f,+zve64x,+zvl64b", check_implicit_always},
  {"zve32f", "+f,+zve32x,+zvl32b", check_implicit_always},
  {"zve64x", "+zve32x,+zvl64b", check_implicit_always},
  {"zve32x", "+zvl32b,+zicsr", check_implicit_always},
  {"zvl65536b", "+zvl32768b", check_implicit_always},
  {"zvl32768b", "+zvl16384b", check_implicit_always},
  {"zvl16384b", "+zvl8192b", check_implicit_always},
  {"zvl8192b", "+zvl4096b", check_implicit_always},
  {"zvl4096b", "+zvl2048b", check_implicit_always},
  {"zvl2048b", "+zvl1024b", check_implicit_always},
  {"zvl1024b", "+zvl512b", check_implicit_always},
  {"zvl512b", "+zvl256b", check_implicit_always},
  {"zvl256b", "+zvl128b", check_implicit_always},
  {"zvl128b", "+zvl64b", check_implicit_always},
  {"zvl64b", "+zvl32b", check_implicit_always},

  {"zicfilp", "+zicsr", check_implicit_always},
  {"zicfiss", "+zimop,+zicsr", check_implicit_always},
  {"zclsd", "+zca,+zilsd", check_implicit_always},

  {"sha", "+h,+ssstateen,+shcounterenw,+shvstvala,+shtvala,+shvstvecd,+shvsatpa,+shgatpa", check_implicit_always},

  {"shcounterenw", "+h", check_implicit_always},
  {"shgatpa", "+h", check_implicit_always},
  {"shtvala", "+h", check_implicit_always},
  {"shvsatpa", "+h", check_implicit_always},
  {"shvstvala", "+h", check_implicit_always},
  {"shvstvecd", "+h", check_implicit_always},
  {"h", "+zicsr", check_implicit_always},
  {"zhinx", "+zhinxmin", check_implicit_always},
  {"zhinxmin", "+zfinx", check_implicit_always},

  {"zcd", "+d,+zca", check_implicit_always},
  {"zcf", "+f,+zca", check_implicit_always},

  {"q", "+d", check_implicit_always},
  {"zqinx", "+zdinx", check_implicit_always},

  {"d", "+f", check_implicit_always},
  {"zdinx", "+zfinx", check_implicit_always},

  {"zfa", "+f", check_implicit_always},
  {"zfbfmin", "+zfhmin", check_implicit_always},
  {"zfh", "+zfhmin", check_implicit_always},
  {"zfhmin", "+f", check_implicit_always},
  {"zfinx", "+zicsr", check_implicit_always},
  {"f", "+zicsr", check_implicit_always},

  {"zce", "+zcb,+zcmp,+zcmt", check_implicit_always},
  {"zce", "+zcf", check_implicit_for_zcf},
  {"zcb", "+zca", check_implicit_always},
  {"zcmp", "+zca", check_implicit_always},
  {"zcmop", "+zca", check_implicit_always},
  {"zcmt", "+zca,+zicsr", check_implicit_always},
  {"c", "+zcf", check_implicit_for_zcf},
  {"c", "+zcd", check_implicit_for_zcd},
  {"c", "+zca", check_implicit_always},

  {"b", "+zba,+zbb,+zbs", check_implicit_always},

  {"zk", "+zkn,+zkr,+zkt", check_implicit_always},
  {"zkn", "+zbkb,+zbkc,+zbkx,+zkne,+zknd,+zknh", check_implicit_always},
  {"zks", "+zbkb,+zbkc,+zbkx,+zksed,+zksh", check_implicit_always},
  {"zvbb", "+zvkb", check_implicit_always},
  {"zvkng", "+zvkn,+zvkg", check_implicit_always},
  {"zvknc", "+zvkn,+zvbc", check_implicit_always},
  {"zvkn", "+zvkned,+zvknhb,+zvkb,+zvkt", check_implicit_always},
  {"zvksg", "+zvks,+zvkg", check_implicit_always},
  {"zvksc", "+zvks,+zvbc", check_implicit_always},
  {"zvks", "+zvksed,+zvksh,+zvkb,+zvkt", check_implicit_always},

  {"smaia", "+ssaia", check_implicit_always},
  {"smcdeleg", "+ssccfg", check_implicit_always},
  {"smcsrind", "+sscsrind", check_implicit_always},
  {"smcntrpmf", "+zicsr", check_implicit_always},
  {"smctr", "+zicsr", check_implicit_always},
  {"smrnmi", "+zicsr", check_implicit_always},
  {"smstateen", "+ssstateen", check_implicit_always},
  {"smepmp", "+zicsr", check_implicit_always},
  {"smdbltrp", "+zicsr", check_implicit_always},
  {"smnpm", "+zicsr", check_implicit_always},
  {"smmpm", "+zicsr", check_implicit_always},

  {"ssaia", "+zicsr", check_implicit_always},
  {"ssccfg", "+sscsrind", check_implicit_always},
  {"sscsrind", "+zicsr", check_implicit_always},
  {"sscofpmf", "+zicsr", check_implicit_always},
  {"sscounterenw", "+zicsr", check_implicit_always},
  {"ssctr", "+zicsr", check_implicit_always},
  {"ssstateen", "+zicsr", check_implicit_always},
  {"sstc", "+zicsr", check_implicit_always},
  {"sstvala", "+zicsr", check_implicit_always},
  {"sstvecd", "+zicsr", check_implicit_always},
  {"ssu64xl", "+zicsr", check_implicit_always},
  {"ssdbltrp", "+zicsr", check_implicit_always},
  {"ssnpm", "+zicsr", check_implicit_always},

  {"svade", "+zicsr", check_implicit_always},
  {"svadu", "+zicsr", check_implicit_always},
  {"svbare", "+zicsr", check_implicit_always},
  {NULL, NULL, NULL}
};

/* This table records the mapping form RISC-V Profiles into march string.  */
static struct riscv_profiles riscv_profiles_table[] =
{
  /* RVI20U only contains the base extension 'i' as mandatory extension.  */
  {"rvi20u64", "rv64i"},
  {"rvi20u32", "rv32i"},

  /* RVA20U contains the 'i,m,a,f,d,c,zicsr,zicntr,ziccif,ziccrse,ziccamoa,
     zicclsm,za128rs' as mandatory extensions.  */
  {"rva20u64", "rv64imafdc_zicsr_zicntr_ziccif_ziccrse_ziccamoa"
   "_zicclsm_za128rs"},

  /* RVA22U contains the 'i,m,a,f,d,c,zicsr,zihintpause,zba,zbb,zbs,zicntr,
     zihpm,ziccif,ziccrse,ziccamoa, zicclsm,zic64b,za64rs,zicbom,zicbop,zicboz,
     zfhmin,zkt' as mandatory extensions.  */
  {"rva22u64", "rv64imafdc_zicsr_zicntr_ziccif_ziccrse_ziccamoa"
   "_zicclsm_zic64b_za64rs_zihintpause_zba_zbb_zbs_zicbom_zicbop"
   "_zicboz_zfhmin_zkt"},

  /* RVA23 contains all mandatory base ISA for RVA22U64 and the new extension
     'v,zihintntl,zvfhmin,zvbb,zvkt,zicond,zimop,zcmop,zfa,zawrs' as mandatory
     extensions.  */
  {"rva23u64", "rv64imafdcbv_zicsr_zicntr_zihpm_ziccif_ziccrse_ziccamoa"
   "_zicclsm_zic64b_za64rs_zihintpause_zba_zbb_zbs_zicbom_zicbop"
   "_zicboz_zfhmin_zkt_zvfhmin_zvbb_zvkt_zihintntl_zicond_zimop_zcmop_zcb"
   "_zfa_zawrs_supm"},

  /* RVA23S contains all mandatory base ISA for RVA23U64 and the privileged
     extensions as mandatory extensions.  */
  {"rva23s64", "rv64imafdcbv_zicsr_zicntr_zihpm_ziccif_ziccrse_ziccamoa"
   "_zicclsm_zic64b_za64rs_zihintpause_zba_zbb_zbs_zicbom_zicbop"
   "_zicboz_zfhmin_zkt_zvfhmin_zvbb_zvkt_zihintntl_zicond_zimop_zcmop_zcb"
   "_zfa_zawrs_supm_svbare_svade_ssccptr_sstvecd_sstvala_sscounterenw_svpbmt"
   "_svinval_svnapot_sstc_sscofpmf_ssnpm_ssu64xl_sha"},

  /* RVB23 contains all mandatory base ISA for RVA22U64 and the new extension
     'zihintntl,zicond,zimop,zcmop,zfa,zawrs' as mandatory
     extensions.  */
  {"rvb23u64", "rv64imafdcb_zicsr_zicntr_zihpm_ziccif_ziccrse_ziccamoa"
   "_zicclsm_zic64b_za64rs_zihintpause_zba_zbb_zbs_zicbom_zicbop"
   "_zicboz_zfhmin_zkt_zihintntl_zicond_zimop_zcmop_zcb"
   "_zfa_zawrs_supm"},

  /* RVB23S contains all mandatory base ISA for RVB23U64 and the privileged
     extensions as mandatory extensions.  */
  {"rvb23s64", "rv64imafdcb_zicsr_zicntr_zihpm_ziccif_ziccrse_ziccamoa"
   "_zicclsm_zic64b_za64rs_zihintpause_zba_zbb_zbs_zicbom_zicbop"
   "_zicboz_zfhmin_zkt_zvfhmin_zvbb_zvkt_zihintntl_zicond_zimop_zcmop_zcb"
   "_zfa_zawrs_supm_svbare_svade_ssccptr_sstvecd_sstvala_sscounterenw_svpbmt"
   "_svinval_svnapot_sstc_sscofpmf_ssu64xl"},

  /* Terminate the list.  */
  {NULL, NULL}
};

/* For default_enable field, decide if the extension should
   be enbaled by default.  */

#define EXT_DEFAULT   0x1

/* List all extensions that binutils should know about.  */

struct riscv_supported_ext
{
  const char *name;
  enum riscv_spec_class isa_spec_class;
  int major_version;
  int minor_version;
  unsigned long default_enable;
};

/* The standard extensions must be added in canonical order.  */

static struct riscv_supported_ext riscv_supported_std_ext[] =
{
  {"e",		ISA_SPEC_CLASS_20191213,	1, 9, 0 },
  {"e",		ISA_SPEC_CLASS_20190608,	1, 9, 0 },
  {"e",		ISA_SPEC_CLASS_2P2,		1, 9, 0 },
  {"i",		ISA_SPEC_CLASS_20191213,	2, 1, 0 },
  {"i",		ISA_SPEC_CLASS_20190608,	2, 1, 0 },
  {"i",		ISA_SPEC_CLASS_2P2,		2, 0, 0 },
  /* The g is a special case which we don't want to output it,
     but still need it when adding implicit extensions.  */
  {"g",		ISA_SPEC_CLASS_NONE, RISCV_UNKNOWN_VERSION, RISCV_UNKNOWN_VERSION, EXT_DEFAULT },
  {"m",		ISA_SPEC_CLASS_20191213,	2, 0, 0 },
  {"m",		ISA_SPEC_CLASS_20190608,	2, 0, 0 },
  {"m",		ISA_SPEC_CLASS_2P2,		2, 0, 0 },
  {"a",		ISA_SPEC_CLASS_20191213,	2, 1, 0 },
  {"a",		ISA_SPEC_CLASS_20190608,	2, 0, 0 },
  {"a",		ISA_SPEC_CLASS_2P2,		2, 0, 0 },
  {"f",		ISA_SPEC_CLASS_20191213,	2, 2, 0 },
  {"f",		ISA_SPEC_CLASS_20190608,	2, 2, 0 },
  {"f",		ISA_SPEC_CLASS_2P2,		2, 0, 0 },
  {"d",		ISA_SPEC_CLASS_20191213,	2, 2, 0 },
  {"d",		ISA_SPEC_CLASS_20190608,	2, 2, 0 },
  {"d",		ISA_SPEC_CLASS_2P2,		2, 0, 0 },
  {"q",		ISA_SPEC_CLASS_20191213,	2, 2, 0 },
  {"q",		ISA_SPEC_CLASS_20190608,	2, 2, 0 },
  {"q",		ISA_SPEC_CLASS_2P2,		2, 0, 0 },
  {"c",		ISA_SPEC_CLASS_20191213,	2, 0, 0 },
  {"c",		ISA_SPEC_CLASS_20190608,	2, 0, 0 },
  {"c",		ISA_SPEC_CLASS_2P2,		2, 0, 0 },
  {"b",		ISA_SPEC_CLASS_DRAFT,		1, 0, 0 },
  {"v",		ISA_SPEC_CLASS_DRAFT,		1, 0, 0 },
  {"h",		ISA_SPEC_CLASS_DRAFT,		1, 0, 0 },
  {NULL, 0, 0, 0, 0}
};

static struct riscv_supported_ext riscv_supported_std_z_ext[] =
{
  {"zic64b",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"ziccamoa",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"ziccif",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zicclsm",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"ziccrse",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zicbom",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zicbop",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zicboz",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zicond",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zicntr",		ISA_SPEC_CLASS_DRAFT,		2, 0,  0 },
  {"zicsr",		ISA_SPEC_CLASS_20191213,	2, 0,  0 },
  {"zicsr",		ISA_SPEC_CLASS_20190608,	2, 0,  0 },
  {"zifencei",		ISA_SPEC_CLASS_20191213,	2, 0,  0 },
  {"zifencei",		ISA_SPEC_CLASS_20190608,	2, 0,  0 },
  {"zihintntl",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zihintpause",	ISA_SPEC_CLASS_DRAFT,		2, 0,  0 },
  {"zihpm",		ISA_SPEC_CLASS_DRAFT,		2, 0,  0 },
  {"zimop",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zicfiss",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zicfilp",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zilsd",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zmmul",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"za64rs",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"za128rs",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zaamo",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zabha",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zacas",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zalrsc",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zawrs",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zfbfmin",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zfa",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zfh",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zfhmin",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zfinx",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zdinx",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zqinx",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zhinx",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zhinxmin",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zbb",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zba",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zbc",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zbs",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zbkb",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zbkc",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zbkx",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zk",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zkn",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zknd",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zkne",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zknh",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zkr",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zks",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zksed",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zksh",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zkt",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zve32x",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zve32f",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zve64x",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zve64f",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zve64d",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zvbb",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zvbc",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zvfbfmin",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zvfbfwma",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zvfh",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zvfhmin",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zvkb",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zvkg",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zvkn",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zvkng",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zvknc",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zvkned",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zvknha",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zvknhb",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zvksed",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zvksh",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zvks",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zvksg",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zvksc",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zvkt",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zvl32b",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zvl64b",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zvl128b",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zvl256b",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zvl512b",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zvl1024b",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zvl2048b",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zvl4096b",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zvl8192b",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zvl16384b",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zvl32768b",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zvl65536b",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"ztso",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zca",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zcb",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zce",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zcf",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zcd",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zcmop",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zcmp",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zcmt",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {"zclsd",		ISA_SPEC_CLASS_DRAFT,		1, 0,  0 },
  {NULL, 0, 0, 0, 0}
};

static struct riscv_supported_ext riscv_supported_std_s_ext[] =
{
  {"sha",		ISA_SPEC_CLASS_DRAFT,		1, 0, 0 },
  {"shcounterenw",	ISA_SPEC_CLASS_DRAFT,		1, 0, 0 },
  {"shgatpa",		ISA_SPEC_CLASS_DRAFT,		1, 0, 0 },
  {"shtvala",		ISA_SPEC_CLASS_DRAFT,		1, 0, 0 },
  {"shvsatpa",		ISA_SPEC_CLASS_DRAFT,		1, 0, 0 },
  {"shvstvala",		ISA_SPEC_CLASS_DRAFT,		1, 0, 0 },
  {"shvstvecd",		ISA_SPEC_CLASS_DRAFT,		1, 0, 0 },
  {"smaia",		ISA_SPEC_CLASS_DRAFT,		1, 0, 0 },
  {"smcdeleg",		ISA_SPEC_CLASS_DRAFT,		1, 0, 0 },
  {"smcsrind",		ISA_SPEC_CLASS_DRAFT,		1, 0, 0 },
  {"smcntrpmf",		ISA_SPEC_CLASS_DRAFT,		1, 0, 0 },
  {"smctr",		ISA_SPEC_CLASS_DRAFT,		1, 0, 0 },
  {"smepmp",		ISA_SPEC_CLASS_DRAFT,		1, 0, 0 },
  {"smrnmi",		ISA_SPEC_CLASS_DRAFT,		1, 0, 0 },
  {"smstateen",		ISA_SPEC_CLASS_DRAFT,		1, 0, 0 },
  {"smdbltrp",		ISA_SPEC_CLASS_DRAFT,		1, 0, 0 },
  {"ssaia",		ISA_SPEC_CLASS_DRAFT,		1, 0, 0 },
  {"ssccfg",		ISA_SPEC_CLASS_DRAFT,		1, 0, 0 },
  {"ssccptr",		ISA_SPEC_CLASS_DRAFT,		1, 0, 0 },
  {"sscsrind",		ISA_SPEC_CLASS_DRAFT,		1, 0, 0 },
  {"sscofpmf",		ISA_SPEC_CLASS_DRAFT,		1, 0, 0 },
  {"sscounterenw",	ISA_SPEC_CLASS_DRAFT,		1, 0, 0 },
  {"ssctr",		ISA_SPEC_CLASS_DRAFT,		1, 0, 0 },
  {"ssstateen",		ISA_SPEC_CLASS_DRAFT,		1, 0, 0 },
  {"sstc",		ISA_SPEC_CLASS_DRAFT,		1, 0, 0 },
  {"sstvala",		ISA_SPEC_CLASS_DRAFT,		1, 0, 0 },
  {"sstvecd",		ISA_SPEC_CLASS_DRAFT,		1, 0, 0 },
  {"ssu64xl",		ISA_SPEC_CLASS_DRAFT,		1, 0, 0 },
  {"ssdbltrp",		ISA_SPEC_CLASS_DRAFT,		1, 0, 0 },
  {"svade",		ISA_SPEC_CLASS_DRAFT,		1, 0, 0 },
  {"svadu",		ISA_SPEC_CLASS_DRAFT,		1, 0, 0 },
  {"svbare",		ISA_SPEC_CLASS_DRAFT,		1, 0, 0 },
  {"svinval",		ISA_SPEC_CLASS_DRAFT,		1, 0, 0 },
  {"svnapot",		ISA_SPEC_CLASS_DRAFT,		1, 0, 0 },
  {"svpbmt",		ISA_SPEC_CLASS_DRAFT,		1, 0, 0 },
  {"svvptc",		ISA_SPEC_CLASS_DRAFT,		1, 0, 0 },
  {"ssqosid",		ISA_SPEC_CLASS_DRAFT,		1, 0, 0 },
  {"ssnpm",		ISA_SPEC_CLASS_DRAFT,		1, 0, 0 },
  {"smnpm",		ISA_SPEC_CLASS_DRAFT,		1, 0, 0 },
  {"smmpm",		ISA_SPEC_CLASS_DRAFT,		1, 0, 0 },
  {"sspm",		ISA_SPEC_CLASS_DRAFT,		1, 0, 0 },
  {"supm",		ISA_SPEC_CLASS_DRAFT,		1, 0, 0 },
  {NULL, 0, 0, 0, 0}
};

static struct riscv_supported_ext riscv_supported_std_zxm_ext[] =
{
  {NULL, 0, 0, 0, 0}
};

static struct riscv_supported_ext riscv_supported_vendor_x_ext[] =
{
  {"xcvalu",		ISA_SPEC_CLASS_DRAFT,	1, 0, 0 },
  {"xcvbi",		ISA_SPEC_CLASS_DRAFT,	1, 0, 0 },
  {"xcvbitmanip",	ISA_SPEC_CLASS_DRAFT,   1, 0, 0 },
  {"xcvelw",		ISA_SPEC_CLASS_DRAFT,   1, 0, 0 },
  {"xcvmac",		ISA_SPEC_CLASS_DRAFT,   1, 0, 0 },
  {"xcvmem",		ISA_SPEC_CLASS_DRAFT,	1, 0, 0 },
  {"xcvsimd",		ISA_SPEC_CLASS_DRAFT,	1, 0, 0 },
  {"xtheadba",		ISA_SPEC_CLASS_DRAFT,	1, 0, 0 },
  {"xtheadbb",		ISA_SPEC_CLASS_DRAFT,	1, 0, 0 },
  {"xtheadbs",		ISA_SPEC_CLASS_DRAFT,	1, 0, 0 },
  {"xtheadcmo",		ISA_SPEC_CLASS_DRAFT,	1, 0, 0 },
  {"xtheadcondmov",	ISA_SPEC_CLASS_DRAFT,	1, 0, 0 },
  {"xtheadfmemidx",	ISA_SPEC_CLASS_DRAFT,	1, 0, 0 },
  {"xtheadfmv",		ISA_SPEC_CLASS_DRAFT,	1, 0, 0 },
  {"xtheadint",		ISA_SPEC_CLASS_DRAFT,	1, 0, 0 },
  {"xtheadmac",		ISA_SPEC_CLASS_DRAFT,	1, 0, 0 },
  {"xtheadmemidx",	ISA_SPEC_CLASS_DRAFT,	1, 0, 0 },
  {"xtheadmempair",	ISA_SPEC_CLASS_DRAFT,	1, 0, 0 },
  {"xtheadsync",	ISA_SPEC_CLASS_DRAFT,	1, 0, 0 },
  {"xtheadvector",	ISA_SPEC_CLASS_DRAFT,	1, 0, 0 },
  {"xtheadvdot",	ISA_SPEC_CLASS_DRAFT,	1, 0, 0 },
  {"xtheadzvamo",	ISA_SPEC_CLASS_DRAFT,	1, 0, 0 },
  {"xventanacondops",	ISA_SPEC_CLASS_DRAFT,	1, 0, 0 },
  {"xsfvcp",		ISA_SPEC_CLASS_DRAFT,	1, 0, 0 },
  {"xsfcease",		ISA_SPEC_CLASS_DRAFT,	1, 0, 0 },
  {"xsfvqmaccqoq",	ISA_SPEC_CLASS_DRAFT,	1, 0, 0},
  {"xsfvqmaccdod",	ISA_SPEC_CLASS_DRAFT,	1, 0, 0},
  {"xsfvfnrclipxfqf",	ISA_SPEC_CLASS_DRAFT,	1, 0, 0},
  {"xmipscbop",		ISA_SPEC_CLASS_DRAFT,	1, 0, 0 },
  {"xmipscmov",		ISA_SPEC_CLASS_DRAFT,	1, 0, 0 },
  {"xmipsexectl",	ISA_SPEC_CLASS_DRAFT,	1, 0, 0 },
  {"xmipslsp",		ISA_SPEC_CLASS_DRAFT,	1, 0, 0 },
  {NULL, 0, 0, 0, 0}
};

const struct riscv_supported_ext *riscv_all_supported_ext[] =
{
  riscv_supported_std_ext,
  riscv_supported_std_z_ext,
  riscv_supported_std_s_ext,
  riscv_supported_std_zxm_ext,
  riscv_supported_vendor_x_ext,
  NULL
};

/* ISA extension prefixed name class.  Must define them in parsing order.  */
enum riscv_prefix_ext_class
{
  RV_ISA_CLASS_Z = 1,
  RV_ISA_CLASS_S,
  RV_ISA_CLASS_ZXM,
  RV_ISA_CLASS_X,
  RV_ISA_CLASS_SINGLE
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
  {RV_ISA_CLASS_X, "x"},
  {RV_ISA_CLASS_SINGLE, NULL}
};

/* Get the prefixed name class for the extensions, the class also
   means the order of the prefixed extensions.  */

static enum riscv_prefix_ext_class
riscv_get_prefix_class (const char *arch)
{
  int i = 0;
  while (parse_config[i].class != RV_ISA_CLASS_SINGLE)
    {
      if (strncmp (arch, parse_config[i].prefix,
		   strlen (parse_config[i].prefix)) == 0)
	return parse_config[i].class;
      i++;
    }
  return RV_ISA_CLASS_SINGLE;
}

/* Check KNOWN_EXTS to see if the EXT is supported.  */

static bool
riscv_known_prefixed_ext (const char *ext,
			  struct riscv_supported_ext *known_exts)
{
  size_t i;
  for (i = 0; known_exts[i].name != NULL; ++i)
    if (strcmp (ext, known_exts[i].name) == 0)
      return true;
  return false;
}

/* Check whether the prefixed extension is recognized or not.  Return
   true if recognized, otehrwise return false.  */

static bool
riscv_recognized_prefixed_ext (const char *ext)
{
  enum riscv_prefix_ext_class class = riscv_get_prefix_class (ext);
  switch (class)
  {
  case RV_ISA_CLASS_Z:
    return riscv_known_prefixed_ext (ext, riscv_supported_std_z_ext);
  case RV_ISA_CLASS_ZXM:
    return riscv_known_prefixed_ext (ext, riscv_supported_std_zxm_ext);
  case RV_ISA_CLASS_S:
    return riscv_known_prefixed_ext (ext, riscv_supported_std_s_ext);
  case RV_ISA_CLASS_X:
    /* Only the single x is unrecognized.  */
    if (strcmp (ext, "x") != 0)
      return true;
  default:
    break;
  }
  return false;
}

/* Canonical order for single letter extensions.  */
static const char riscv_ext_canonical_order[] = "eigmafdqlcbkjtpvnh";

/* Array is used to compare the orders of standard extensions quickly.  */
static int riscv_ext_order[26] = {0};

/* Init the riscv_ext_order array.  */

static void
riscv_init_ext_order (void)
{
  static bool inited = false;
  if (inited)
    return;

  /* The orders of all standard extensions are positive.  */
  int order = 1;

  for (const char *ext = &riscv_ext_canonical_order[0]; *ext; ++ext)
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

  if (class1 != RV_ISA_CLASS_SINGLE)
    order1 = - (int) class1;
  if (class2 != RV_ISA_CLASS_SINGLE)
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

  /* If the subset is added in order, then just add it at the tail.  */
  if (subset_list->tail != NULL
      && riscv_compare_subsets (subset_list->tail->name, subset) < 0)
    {
      *current = subset_list->tail;
      return false;
    }

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

/* Add the extension to the subset list.  Search the
   list first, and then find the right place to add.  */

void
riscv_add_subset (riscv_subset_list_t *subset_list,
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

  if (new->next == NULL)
    subset_list->tail = new;
}

/* Get the default versions from the riscv_supported_*ext tables.  */

static void
riscv_get_default_ext_version (enum riscv_spec_class *default_isa_spec,
			       const char *name,
			       int *major_version,
			       int *minor_version)
{
  if (name == NULL
      || default_isa_spec == NULL
      || *default_isa_spec == ISA_SPEC_CLASS_NONE)
    return;

  struct riscv_supported_ext *table = NULL;
  enum riscv_prefix_ext_class class = riscv_get_prefix_class (name);
  switch (class)
    {
    case RV_ISA_CLASS_ZXM: table = riscv_supported_std_zxm_ext; break;
    case RV_ISA_CLASS_Z: table = riscv_supported_std_z_ext; break;
    case RV_ISA_CLASS_S: table = riscv_supported_std_s_ext; break;
    case RV_ISA_CLASS_X: table = riscv_supported_vendor_x_ext; break;
    default:
      table = riscv_supported_std_ext;
    }

  int i = 0;
  while (table != NULL && table[i].name != NULL)
    {
      if (strcmp (table[i].name, name) == 0
	  && (table[i].isa_spec_class == ISA_SPEC_CLASS_DRAFT
	      || table[i].isa_spec_class == *default_isa_spec))
	{
	  *major_version = table[i].major_version;
	  *minor_version = table[i].minor_version;
	  return;
	}
      i++;
    }
}

/* Find the default versions for the extension before adding them to
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

  if (major_version == RISCV_UNKNOWN_VERSION
       || minor_version == RISCV_UNKNOWN_VERSION)
    riscv_get_default_ext_version (rps->isa_spec, subset,
				   &major_version, &minor_version);

  /* We don't care the versions of the implicit extensions.  */
  if (!implicit
      && (major_version == RISCV_UNKNOWN_VERSION
	  || minor_version == RISCV_UNKNOWN_VERSION))
    {
      if (subset[0] == 'x')
	rps->error_handler
	  (_("x ISA extension `%s' must be set with the versions"),
	   subset);
      /* Allow old ISA spec can recognize zicsr and zifencei.  */
      else if (strcmp (subset, "zicsr") != 0
	       && strcmp (subset, "zifencei") != 0)
	rps->error_handler
	  (_("cannot find default versions of the ISA extension `%s'"),
	   subset);
      return;
    }

  riscv_add_subset (rps->subset_list, subset,
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

  if (subset_list->arch_str != NULL)
    {
      free ((void*) subset_list->arch_str);
      subset_list->arch_str = NULL;
    }
}

/* Parsing extension version.

   Return Value:
     Points to the end of version

   Arguments:
     `p`: Curent parsing position.
     `major_version`: Parsed major version.
     `minor_version`: Parsed minor version.  */

static const char *
riscv_parsing_subset_version (const char *p,
			      int *major_version,
			      int *minor_version)
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

	  /* Might be beginning of `p` extension.  */
	  if (!ISDIGIT (np))
	    break;

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

/* Parsing function for both standard and prefixed extensions.

   Return Value:
     Points to the end of extensions.

   Arguments:
     `rps`: Hooks and status for parsing extensions.
     `arch`: Full ISA string.
     `p`: Curent parsing position.  */

static const char *
riscv_parse_extensions (riscv_parse_subset_t *rps,
			const char *arch,
			const char *p,
			bool profile)
{
  /* First letter must start with i, e, g or a profile.  */
  if (*p != 'e' && *p != 'i' && *p != 'g' && !profile)
    {
      rps->error_handler
	(_("%s: first ISA extension must be `e', `i' or `g'"),
	 arch);
      return NULL;
    }

  while (*p != '\0')
    {
      if (*p == '_')
	{
	  p++;
	  continue;
	}

      char *subset = xstrdup (p);
      char *q = subset;	/* Start of version.  */
      const char *end_of_version;
      bool implicit = false;

      enum riscv_prefix_ext_class class = riscv_get_prefix_class (p);
      if (class == RV_ISA_CLASS_SINGLE)
	{
	  if (riscv_ext_order[(*subset - 'a')] == 0)
	    {
	      rps->error_handler
		(_("%s: unknown standard ISA extension or prefix class `%c'"),
		 arch, *subset);
	      free (subset);
	      return NULL;
	    }
	  q++;
	}
      else
	{
	  /* Extract the whole prefixed extension by '_'.  */
	  while (*++q != '\0' && *q != '_')
	    ;
	  /* Look forward to the first letter which is not <major>p<minor>.  */
	  bool find_any_version = false;
	  bool find_minor_version = false;
	  while (1)
	    {
	      q--;
	      if (ISDIGIT (*q))
		find_any_version = true;
	      else if (find_any_version
		       && !find_minor_version
		       && *q == 'p'
		       && ISDIGIT (*(q - 1)))
	      find_minor_version = true;
	      else
		break;
	    }
	  q++;

	  /* Check if the end of extension is 'p' or not.  If yes, then
	     the second letter from the end cannot be number.  */
	  if (*(q - 1) == 'p' && ISDIGIT (*(q - 2)))
	    {
	      *q = '\0';
	      rps->error_handler
		(_("%s: invalid prefixed ISA extension `%s' ends with <number>p"),
		 arch, subset);
	      free (subset);
	      return NULL;
	    }
	}

      int major_version = RISCV_UNKNOWN_VERSION;
      int minor_version = RISCV_UNKNOWN_VERSION;
      end_of_version =
	riscv_parsing_subset_version (q, &major_version, &minor_version);
      *q = '\0';
      if (end_of_version == NULL)
	{
	  free (subset);
	  return NULL;
	}

      /* Check if the prefixed extension name is well-formed.  */
      if (class != RV_ISA_CLASS_SINGLE
	  && rps->check_unknown_prefixed_ext
	  && !riscv_recognized_prefixed_ext (subset))
	{
	  rps->error_handler
	    (_("%s: unknown prefixed ISA extension `%s'"),
	     arch, subset);
	  free (subset);
	  return NULL;
	}

      /* Added g as an implicit extension.  */
      if (class == RV_ISA_CLASS_SINGLE
	  && strcmp (subset, "g") == 0)
	{
	  implicit = true;
	  major_version = RISCV_UNKNOWN_VERSION;
	  minor_version = RISCV_UNKNOWN_VERSION;
	}
      riscv_parse_add_subset (rps, subset,
			      major_version,
			      minor_version, implicit);
      p += end_of_version - subset;
      free (subset);

      if (class != RV_ISA_CLASS_SINGLE
	  && *p != '\0' && *p != '_')
	{
	  rps->error_handler
	    (_("%s: prefixed ISA extension must separate with _"),
	     arch);
	  return NULL;
	}
    }

  return p;
}

static bool
riscv_update_subset1 (riscv_parse_subset_t *, riscv_subset_t *, const char *);

/* Add the implicit extensions.  */

static void
riscv_parse_add_implicit_subsets (riscv_parse_subset_t *rps)
{
  struct riscv_implicit_subset *t = riscv_implicit_subsets;
  for (; t->ext; t++)
    {
      riscv_subset_t *subset = NULL;
      if (riscv_lookup_subset (rps->subset_list, t->ext, &subset)
	&& t->check_func (rps, subset))
      riscv_update_subset1 (rps, subset, t->implicit_exts);
    }
}

/* Check extensions conflicts.  */

static bool
riscv_parse_check_conflicts (riscv_parse_subset_t *rps)
{
  riscv_subset_t *subset = NULL;
  int xlen = *rps->xlen;
  bool no_conflict = true;

  if (riscv_subset_supports (rps, "e")
      && riscv_subset_supports (rps, "h"))
    {
      rps->error_handler
	(_("rv%de does not support the `h' extension"), xlen);
      no_conflict = false;
    }
  if (riscv_lookup_subset (rps->subset_list, "q", &subset)
      && (subset->major_version < 2 || (subset->major_version == 2
					&& subset->minor_version < 2))
      && xlen < 64)
    {
      rps->error_handler (_("rv%d does not support the `q' extension"), xlen);
      no_conflict = false;
    }
  if (riscv_subset_supports (rps, "zcmp")
      && riscv_subset_supports (rps, "zcd"))
    {
      rps->error_handler
	(_("zcmp' is incompatible with `d' and `c', or `zcd' extension"));
      no_conflict = false;
    }
  if (riscv_lookup_subset (rps->subset_list, "zcf", &subset)
      && xlen > 32)
    {
      rps->error_handler
	(_("rv%d does not support the `zcf' extension"), xlen);
      no_conflict = false;
    }
  if (riscv_lookup_subset (rps->subset_list, "zfinx", &subset)
      && riscv_lookup_subset (rps->subset_list, "f", &subset))
    {
      rps->error_handler
	(_("`zfinx' is conflict with the `f/d/q/zfh/zfhmin' extension"));
      no_conflict = false;
    }
  if (riscv_lookup_subset (rps->subset_list, "xtheadvector", &subset)
      && riscv_lookup_subset (rps->subset_list, "zve32x", &subset))
    {
      rps->error_handler
	(_("`xtheadvector' is conflict with the `v/zve32x' extension"));
      no_conflict = false;
    }
  if (riscv_lookup_subset (rps->subset_list, "zclsd", &subset)
      && ((riscv_lookup_subset (rps->subset_list, "c", &subset)
	   && riscv_lookup_subset (rps->subset_list, "f", &subset))
	  || riscv_lookup_subset (rps->subset_list, "zcf", &subset)))
    {
      rps->error_handler
	(_("`zclsd' is conflict with the `c+f'/ `zcf' extension"));
      no_conflict = false;
    }
  if (riscv_lookup_subset (rps->subset_list, "ssnpm", &subset) && xlen != 64)
    {
      rps->error_handler (_ ("rv%d does not support the `ssnpm' extension"),
			  xlen);
      no_conflict = false;
    }
  if (riscv_lookup_subset (rps->subset_list, "smnpm", &subset) && xlen != 64)
    {
      rps->error_handler (_ ("rv%d does not support the `smnpm' extension"),
			  xlen);
      no_conflict = false;
    }
  if (riscv_lookup_subset (rps->subset_list, "smmpm", &subset) && xlen != 64)
    {
      rps->error_handler (_ ("rv%d does not support the `smmpm' extension"),
			  xlen);
      no_conflict = false;
    }
  if (riscv_lookup_subset (rps->subset_list, "sspm", &subset) && xlen != 64)
    {
      rps->error_handler (_ ("rv%d does not support the `sspm' extension"),
			  xlen);
      no_conflict = false;
    }
  if (riscv_lookup_subset (rps->subset_list, "supm", &subset) && xlen != 64)
    {
      rps->error_handler (_ ("rv%d does not support the `supm' extension"),
			  xlen);
      no_conflict = false;
    }

  bool support_zve = false;
  bool support_zvl = false;
  riscv_subset_t *s = rps->subset_list->head;
  for (; s != NULL; s = s->next)
    {
      if (!support_zve
	  && strncmp (s->name, "zve", 3) == 0)
	support_zve = true;
      if (!support_zvl
	  && strncmp (s->name, "zvl", 3) == 0)
	support_zvl = true;
      if (support_zve && support_zvl)
	break;
    }
  if (support_zvl && !support_zve)
    {
      rps->error_handler
	(_("zvl*b extensions need to enable either `v' or `zve' extension"));
      no_conflict = false;
    }

  return no_conflict;
}

/* Set the default subset list according to the default_enable field
   of riscv_supported_*ext tables.  */

static void
riscv_set_default_arch (riscv_parse_subset_t *rps)
{
  unsigned long enable = EXT_DEFAULT;
  int i, j;
  for (i = 0; riscv_all_supported_ext[i] != NULL; i++)
    {
      const struct riscv_supported_ext *table = riscv_all_supported_ext[i];
      for (j = 0; table[j].name != NULL; j++)
	{
	  bool implicit = false;
	  if (strcmp (table[j].name, "g") == 0)
	    implicit = true;
	  if (table[j].default_enable & enable)
	    riscv_parse_add_subset (rps, table[j].name,
				    RISCV_UNKNOWN_VERSION,
				    RISCV_UNKNOWN_VERSION, implicit);
	}
    }
}

static bool
riscv_find_profiles (riscv_parse_subset_t *rps, const char **pp)
{
  const char *p = *pp;

  /* Checking if input string contains a Profiles.
     There are two cases use Profiles in -march option:

      1. Only use Profiles in '-march' as input
      2. Mixed Profiles with other extensions

      Use '_' to split Profiles and other extensions.  */

  for (int i = 0; riscv_profiles_table[i].profile_name != NULL; ++i)
    {
      /* Find profile at the begin.  */
      if (startswith (p, riscv_profiles_table[i].profile_name))
	{
	  /* Handle the profile string.  */
	  riscv_parse_subset (rps, riscv_profiles_table[i].profile_string);
	  p += strlen (riscv_profiles_table[i].profile_name);
	  /* Handle string after profiles if exists.  If missing underline
	     bewteen profile and other extensions, warn the user but not deal
	     as an error.  */
	  if (*p != '\0' && *p != '_')
	    _bfd_error_handler
	      (_("Warning: should use \"_\" to contact Profiles with other "
		 "extensions"));
	  *pp = p;
	  return true;
	}
    }
  /* Not found profile, return directly.  */
  return false;
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
  const char *p;

  /* Init the riscv_ext_order array to compare the order of extensions
     quickly.  */
  riscv_init_ext_order ();

  if (arch == NULL)
    {
      riscv_set_default_arch (rps);
      riscv_parse_add_implicit_subsets (rps);
      return riscv_parse_check_conflicts (rps);
    }

  for (p = arch; *p != '\0'; p++)
    {
      if (ISUPPER (*p))
	{
	  rps->error_handler
	    (_("%s: ISA string cannot contain uppercase letters"),
	     arch);
	  return false;
	}
    }

  bool profile = false;
  p = arch;
  if (riscv_find_profiles (rps, &p))
    {
      /* Check if using Profiles.  */
      profile = true;
    }
  else if (startswith (p, "rv32"))
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
      /* ISA string shouldn't be NULL or empty here.  For linker,
	 it might be empty when we failed to merge the ISA string
	 in the riscv_merge_attributes.  For assembler, we might
	 give an empty string by .attribute arch, "" or -march=.
	 However, We have already issued the correct error message
	 in another side, so do not issue this error when the ISA
	 string is empty.  */
      if (strlen (arch))
	rps->error_handler (
	  _("%s: ISA string must begin with rv32, rv64 or Profiles"),
	  arch);
      return false;
    }

  /* Parse single standard and prefixed extensions.  */
  if (riscv_parse_extensions (rps, arch, p, profile) == NULL)
    return false;

  /* Finally add implicit extensions according to the current
     extensions.  */
  riscv_parse_add_implicit_subsets (rps);

  /* Check the conflicts.  */
  return riscv_parse_check_conflicts (rps);
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
riscv_arch_str (unsigned xlen, riscv_subset_list_t *subset, bool update)
{
  size_t arch_str_len = riscv_estimate_arch_strlen (subset);
  char *attr_str = xmalloc (arch_str_len);
  char *buf = xmalloc (arch_str_len);

  snprintf (attr_str, arch_str_len, "rv%u", xlen);

  riscv_arch_str1 (subset->head, attr_str, buf, arch_str_len);
  free (buf);

  if (update)
    {
      if (subset->arch_str != NULL)
	free ((void *) subset->arch_str);
      subset->arch_str = attr_str;
    }

  return attr_str;
}

/* Copy the subset in the subset list.  */

static struct riscv_subset_t *
riscv_copy_subset (riscv_subset_list_t *subset_list,
		   riscv_subset_t *subset)
{
  if (subset == NULL)
    return NULL;

  riscv_subset_t *new = xmalloc (sizeof *new);
  new->name = xstrdup (subset->name);
  new->major_version = subset->major_version;
  new->minor_version = subset->minor_version;
  new->next = riscv_copy_subset (subset_list, subset->next);

  if (subset->next == NULL)
    subset_list->tail = new;

  return new;
}

/* Copy the subset list.  */

riscv_subset_list_t *
riscv_copy_subset_list (riscv_subset_list_t *subset_list)
{
  riscv_subset_list_t *new = xmalloc (sizeof *new);
  new->head = riscv_copy_subset (new, subset_list->head);
  new->arch_str = strdup (subset_list->arch_str);
  return new;
}

/* Remove the SUBSET from the subset list.  */

static void
riscv_remove_subset (riscv_subset_list_t *subset_list,
		     const char *subset)
{
  riscv_subset_t *current = subset_list->head;
  riscv_subset_t *pre = NULL;
  for (; current != NULL; pre = current, current = current->next)
    {
      if (strcmp (current->name, subset) == 0)
	{
	  if (pre == NULL)
	    subset_list->head = current->next;
	  else
	    pre->next = current->next;
	  if (current->next == NULL)
	    subset_list->tail = pre;
	  free ((void *) current->name);
	  free (current);
	  break;
	}
    }
}

/* Auxiliary to add/remove extensions to/from the subset list.
   This is called from riscv_update_subset or riscv_parse_add_implicit_subsets.

   The EXPLICIT_SUBSET, the corresponding explicit extension.  It is NULL means
   called from riscv_update_subset./

   The IMPLICIT_EXTS, +extension[version] [,...,+extension_n[version_n]]
		      (Deprecated) -extension [,...,-extension_n],
		      full ISA.  */

static bool
riscv_update_subset1 (riscv_parse_subset_t *rps,
		      riscv_subset_t *explicit_subset,
		      const char *implicit_exts)
{
  const char *p = implicit_exts;
  const char *errmsg_internal = explicit_subset == NULL ? "" : _("internal: ");
  const char *errmsg_caller = explicit_subset == NULL
			      ? ".option arch" : "riscv_implicit_subsets";

  do
    {
      int major_version = RISCV_UNKNOWN_VERSION;
      int minor_version = RISCV_UNKNOWN_VERSION;

      bool removed = false;
      switch (*p)
	{
	case '+': removed = false; break;
	case '-': removed = true; break;
	default:
	  riscv_release_subset_list (rps->subset_list);
	  return riscv_parse_subset (rps, p);
	}
      ++p;

      char *subset = xstrdup (p);
      char *q = subset;
      const char *end_of_version;
      /* Extract the whole prefixed extension by ','.  */
      while (*q != '\0' && *q != ',')
        q++;

      /* Look forward to the first letter which is not <major>p<minor>.  */
      bool find_any_version = false;
      bool find_minor_version = false;
      size_t len = q - subset;
      size_t i;
      for (i = len; i > 0; i--)
        {
	  q--;
	  if (ISDIGIT (*q))
	    find_any_version = true;
	  else if (find_any_version
		   && !find_minor_version
		   && *q == 'p'
		   && ISDIGIT (*(q - 1)))
	    find_minor_version = true;
	  else
	    break;
	}
      if (len > 0)
	q++;

      /* Check if the end of extension is 'p' or not.  If yes, then
	 the second letter from the end cannot be number.  */
      if (len > 1 && *(q - 1) == 'p' && ISDIGIT (*(q - 2)))
	{
	  *q = '\0';
	  rps->error_handler
	    (_("%sinvalid ISA extension ends with <number>p in %s `%s'"),
	       errmsg_internal, errmsg_caller, implicit_exts);
	  free (subset);
	  return false;
	}

      end_of_version =
	riscv_parsing_subset_version (q, &major_version, &minor_version);
      *q = '\0';
      if (end_of_version == NULL)
	{
	  free (subset);
	  return false;
	}

      if (strlen (subset) == 0
	  || (strlen (subset) == 1
	      && riscv_ext_order[(*subset - 'a')] == 0)
	  || (strlen (subset) > 1
	      && rps->check_unknown_prefixed_ext
	      && !riscv_recognized_prefixed_ext (subset)))
	{
	  rps->error_handler
	    (_("%sunknown ISA extension `%s' in %s `%s'"),
	     errmsg_internal, subset, errmsg_caller, implicit_exts);
	  free (subset);
	  return false;
	}

      if (explicit_subset == NULL)
	{
	  if (removed)
	    {
	      rps->error_handler
		(_("%sdeprecated - extension `%s' in %s `%s'"),
		   errmsg_internal, subset, errmsg_caller, implicit_exts);
	      free (subset);
	      return false;
	    }
	  else if (strcmp (subset, "i") == 0
		   || strcmp (subset, "e") == 0
		   || strcmp (subset, "g") == 0)
	    {
	      rps->error_handler
		(_("%scannot + base extension `%s' in %s `%s'"),
		   errmsg_internal, subset, errmsg_caller, implicit_exts);
	      free (subset);
	      return false;
	    }
	}

      if (removed)
	riscv_remove_subset (rps->subset_list, subset);
      else
	{
	  riscv_subset_t *isubset = NULL;
	  if (!riscv_lookup_subset (rps->subset_list, subset, &isubset))
	    riscv_parse_add_subset (rps, subset, major_version, minor_version,
				    true/* implicit */);
	}
      p += end_of_version - subset;
      free (subset);
    }
  while (*p++ == ',');

  bool no_conflict = true;
  if (explicit_subset == NULL)
    {
      riscv_parse_add_implicit_subsets (rps);
      no_conflict = riscv_parse_check_conflicts (rps);
    }
  return no_conflict;
}

/* Add an extension to/from the subset list.  This is used for the .option rvc
   and .option arch directives.  */

bool
riscv_update_subset (riscv_parse_subset_t *rps,
		     const char *str)
{
  return riscv_update_subset1 (rps, NULL, str);
}

/* Called from .option norvc directives.  */

bool
riscv_update_subset_norvc (riscv_parse_subset_t *rps)
{
  return riscv_update_subset1 (rps, rps->subset_list->head,
			       "-c,-zca,-zcd,-zcf,-zcb,-zce,-zcmp,-zcmt,"
			       "-zcmop,-zclsd");
}

/* Check if the FEATURE subset is supported or not in the subset list.
   Return true if it is supported; Otherwise, return false.  */

bool
riscv_subset_supports (riscv_parse_subset_t *rps,
		       const char *feature)
{
  struct riscv_subset_t *subset;
  return riscv_lookup_subset (rps->subset_list, feature, &subset);
}

/* Each instuction is belonged to an instruction class INSN_CLASS_*.
   Call riscv_subset_supports to make sure if the instuction is valid.  */

bool
riscv_multi_subset_supports (riscv_parse_subset_t *rps,
			     enum riscv_insn_class insn_class)
{
  switch (insn_class)
    {
    case INSN_CLASS_I:
      return riscv_subset_supports (rps, "i");
    case INSN_CLASS_ZICBOM:
      return riscv_subset_supports (rps, "zicbom");
    case INSN_CLASS_ZICBOP:
      return riscv_subset_supports (rps, "zicbop");
    case INSN_CLASS_ZICBOZ:
      return riscv_subset_supports (rps, "zicboz");
    case INSN_CLASS_ZICOND:
      return riscv_subset_supports (rps, "zicond");
    case INSN_CLASS_ZICSR:
      return riscv_subset_supports (rps, "zicsr");
    case INSN_CLASS_ZIFENCEI:
      return riscv_subset_supports (rps, "zifencei");
    case INSN_CLASS_ZIHINTNTL:
      return riscv_subset_supports (rps, "zihintntl");
    case INSN_CLASS_ZIHINTNTL_AND_ZCA:
      return riscv_subset_supports (rps, "zihintntl")
	     && riscv_subset_supports (rps, "zca");
    case INSN_CLASS_ZIHINTPAUSE:
      return riscv_subset_supports (rps, "zihintpause");
    case INSN_CLASS_ZIMOP:
      return riscv_subset_supports (rps, "zimop");
    case INSN_CLASS_ZICFISS:
      return riscv_subset_supports (rps, "zicfiss");
    case INSN_CLASS_ZICFISS_AND_ZCMOP:
      return riscv_subset_supports (rps, "zicfiss")
	     && riscv_subset_supports (rps, "zcmop");
    case INSN_CLASS_ZICFILP:
      return riscv_subset_supports (rps, "zicfilp");
    case INSN_CLASS_M:
      return riscv_subset_supports (rps, "m");
    case INSN_CLASS_ZMMUL:
      return riscv_subset_supports (rps, "zmmul");
    case INSN_CLASS_ZAAMO:
      return riscv_subset_supports (rps, "zaamo");
    case INSN_CLASS_ZABHA:
      return riscv_subset_supports (rps, "zabha");
    case INSN_CLASS_ZACAS:
      return riscv_subset_supports (rps, "zacas");
    case INSN_CLASS_ZABHA_AND_ZACAS:
      return (riscv_subset_supports (rps, "zabha")
	      && riscv_subset_supports (rps, "zacas"));
    case INSN_CLASS_ZALRSC:
      return riscv_subset_supports (rps, "zalrsc");
    case INSN_CLASS_ZAWRS:
      return riscv_subset_supports (rps, "zawrs");
    case INSN_CLASS_F:
      return riscv_subset_supports (rps, "f");
    case INSN_CLASS_D:
      return riscv_subset_supports (rps, "d");
    case INSN_CLASS_Q:
      return riscv_subset_supports (rps, "q");
    case INSN_CLASS_ZCA:
      return riscv_subset_supports (rps, "zca");
    case INSN_CLASS_ZCF:
      return riscv_subset_supports (rps, "zcf");
    case INSN_CLASS_ZCD:
      return riscv_subset_supports (rps, "zcd");
    case INSN_CLASS_F_INX:
      return (riscv_subset_supports (rps, "f")
	      || riscv_subset_supports (rps, "zfinx"));
    case INSN_CLASS_D_INX:
      return (riscv_subset_supports (rps, "d")
	      || riscv_subset_supports (rps, "zdinx"));
    case INSN_CLASS_Q_INX:
      return (riscv_subset_supports (rps, "q")
	      || riscv_subset_supports (rps, "zqinx"));
    case INSN_CLASS_ZFH_INX:
      return (riscv_subset_supports (rps, "zfh")
	      || riscv_subset_supports (rps, "zhinx"));
    case INSN_CLASS_ZFHMIN:
      return riscv_subset_supports (rps, "zfhmin");
    case INSN_CLASS_ZFHMIN_INX:
      return (riscv_subset_supports (rps, "zfhmin")
	      || riscv_subset_supports (rps, "zhinxmin"));
    case INSN_CLASS_ZFHMIN_AND_D_INX:
      return ((riscv_subset_supports (rps, "zfhmin")
	       && riscv_subset_supports (rps, "d"))
	      || (riscv_subset_supports (rps, "zhinxmin")
		  && riscv_subset_supports (rps, "zdinx")));
    case INSN_CLASS_ZFHMIN_AND_Q_INX:
      return ((riscv_subset_supports (rps, "zfhmin")
	       && riscv_subset_supports (rps, "q"))
	      || (riscv_subset_supports (rps, "zhinxmin")
		  && riscv_subset_supports (rps, "zqinx")));
    case INSN_CLASS_ZFBFMIN:
      return riscv_subset_supports (rps, "zfbfmin");
    case INSN_CLASS_ZFA:
      return riscv_subset_supports (rps, "zfa");
    case INSN_CLASS_D_AND_ZFA:
      return riscv_subset_supports (rps, "d")
	     && riscv_subset_supports (rps, "zfa");
    case INSN_CLASS_Q_AND_ZFA:
      return riscv_subset_supports (rps, "q")
	     && riscv_subset_supports (rps, "zfa");
    case INSN_CLASS_ZFH_AND_ZFA:
      return riscv_subset_supports (rps, "zfh")
	     && riscv_subset_supports (rps, "zfa");
    case INSN_CLASS_ZFH_OR_ZVFH_AND_ZFA:
      return (riscv_subset_supports (rps, "zfh")
	      || riscv_subset_supports (rps, "zvfh"))
	     && riscv_subset_supports (rps, "zfa");
    case INSN_CLASS_ZBA:
      return riscv_subset_supports (rps, "zba");
    case INSN_CLASS_ZBB:
      return riscv_subset_supports (rps, "zbb");
    case INSN_CLASS_ZBC:
      return riscv_subset_supports (rps, "zbc");
    case INSN_CLASS_ZBS:
      return riscv_subset_supports (rps, "zbs");
    case INSN_CLASS_ZBKB:
      return riscv_subset_supports (rps, "zbkb");
    case INSN_CLASS_ZBKC:
      return riscv_subset_supports (rps, "zbkc");
    case INSN_CLASS_ZBKX:
      return riscv_subset_supports (rps, "zbkx");
    case INSN_CLASS_ZBB_OR_ZBKB:
      return (riscv_subset_supports (rps, "zbb")
	      || riscv_subset_supports (rps, "zbkb"));
    case INSN_CLASS_ZBC_OR_ZBKC:
      return (riscv_subset_supports (rps, "zbc")
	      || riscv_subset_supports (rps, "zbkc"));
    case INSN_CLASS_ZKND:
      return riscv_subset_supports (rps, "zknd");
    case INSN_CLASS_ZKNE:
      return riscv_subset_supports (rps, "zkne");
    case INSN_CLASS_ZKNH:
      return riscv_subset_supports (rps, "zknh");
    case INSN_CLASS_ZKND_OR_ZKNE:
      return (riscv_subset_supports (rps, "zknd")
	      || riscv_subset_supports (rps, "zkne"));
    case INSN_CLASS_ZKSED:
      return riscv_subset_supports (rps, "zksed");
    case INSN_CLASS_ZKSH:
      return riscv_subset_supports (rps, "zksh");
    case INSN_CLASS_V:
      return (riscv_subset_supports (rps, "v")
	      || riscv_subset_supports (rps, "zve64x")
	      || riscv_subset_supports (rps, "zve32x"));
    case INSN_CLASS_ZVEF:
      return (riscv_subset_supports (rps, "v")
	      || riscv_subset_supports (rps, "zve64d")
	      || riscv_subset_supports (rps, "zve64f")
	      || riscv_subset_supports (rps, "zve32f"));
    case INSN_CLASS_ZVBB:
      return riscv_subset_supports (rps, "zvbb");
    case INSN_CLASS_ZVBC:
      return riscv_subset_supports (rps, "zvbc");
    case INSN_CLASS_ZVFBFMIN:
      return riscv_subset_supports (rps, "zvfbfmin");
    case INSN_CLASS_ZVFBFWMA:
      return riscv_subset_supports (rps, "zvfbfwma");
    case INSN_CLASS_ZVKB:
      return riscv_subset_supports (rps, "zvkb");
    case INSN_CLASS_ZVKG:
      return riscv_subset_supports (rps, "zvkg");
    case INSN_CLASS_ZVKNED:
      return riscv_subset_supports (rps, "zvkned");
    case INSN_CLASS_ZVKNHA_OR_ZVKNHB:
      return (riscv_subset_supports (rps, "zvknha")
	      || riscv_subset_supports (rps, "zvknhb"));
    case INSN_CLASS_ZVKSED:
      return riscv_subset_supports (rps, "zvksed");
    case INSN_CLASS_ZVKSH:
      return riscv_subset_supports (rps, "zvksh");
    case INSN_CLASS_ZCB:
      return riscv_subset_supports (rps, "zcb");
    case INSN_CLASS_ZCB_AND_ZBB:
      return (riscv_subset_supports (rps, "zcb")
	      && riscv_subset_supports (rps, "zbb"));
    case INSN_CLASS_ZCB_AND_ZBA:
      return (riscv_subset_supports (rps, "zcb")
	      && riscv_subset_supports (rps, "zba"));
    case INSN_CLASS_ZCB_AND_ZMMUL:
      return (riscv_subset_supports (rps, "zcb")
	      && riscv_subset_supports (rps, "zmmul"));
    case INSN_CLASS_ZCMOP:
      return riscv_subset_supports (rps, "zcmop");
    case INSN_CLASS_ZCMP:
      return riscv_subset_supports (rps, "zcmp");
    case INSN_CLASS_ZCMT:
      return riscv_subset_supports (rps, "zcmt");
    case INSN_CLASS_SMCTR_OR_SSCTR:
      return (riscv_subset_supports (rps, "smctr")
	      || riscv_subset_supports (rps, "ssctr"));
    case INSN_CLASS_ZILSD:
      return riscv_subset_supports (rps, "zilsd");
    case INSN_CLASS_ZCLSD:
      return riscv_subset_supports (rps, "zclsd");
    case INSN_CLASS_SMRNMI:
      return riscv_subset_supports (rps, "smrnmi");
    case INSN_CLASS_SVINVAL:
      return riscv_subset_supports (rps, "svinval");
    case INSN_CLASS_H:
      return riscv_subset_supports (rps, "h");
    case INSN_CLASS_XCVALU:
      return riscv_subset_supports (rps, "xcvalu");
    case INSN_CLASS_XCVBI:
      return riscv_subset_supports (rps, "xcvbi");
    case INSN_CLASS_XCVBITMANIP:
      return riscv_subset_supports (rps, "xcvbitmanip");
    case INSN_CLASS_XCVELW:
      return riscv_subset_supports (rps, "xcvelw");
    case INSN_CLASS_XCVMAC:
      return riscv_subset_supports (rps, "xcvmac");
    case INSN_CLASS_XCVMEM:
      return riscv_subset_supports (rps, "xcvmem");
    case INSN_CLASS_XCVSIMD:
      return riscv_subset_supports (rps, "xcvsimd");
    case INSN_CLASS_XTHEADBA:
      return riscv_subset_supports (rps, "xtheadba");
    case INSN_CLASS_XTHEADBB:
      return riscv_subset_supports (rps, "xtheadbb");
    case INSN_CLASS_XTHEADBS:
      return riscv_subset_supports (rps, "xtheadbs");
    case INSN_CLASS_XTHEADCMO:
      return riscv_subset_supports (rps, "xtheadcmo");
    case INSN_CLASS_XTHEADCONDMOV:
      return riscv_subset_supports (rps, "xtheadcondmov");
    case INSN_CLASS_XTHEADFMEMIDX:
      return riscv_subset_supports (rps, "xtheadfmemidx");
    case INSN_CLASS_XTHEADFMV:
      return riscv_subset_supports (rps, "xtheadfmv");
    case INSN_CLASS_XTHEADINT:
      return riscv_subset_supports (rps, "xtheadint");
    case INSN_CLASS_XTHEADMAC:
      return riscv_subset_supports (rps, "xtheadmac");
    case INSN_CLASS_XTHEADMEMIDX:
      return riscv_subset_supports (rps, "xtheadmemidx");
    case INSN_CLASS_XTHEADMEMPAIR:
      return riscv_subset_supports (rps, "xtheadmempair");
    case INSN_CLASS_XTHEADSYNC:
      return riscv_subset_supports (rps, "xtheadsync");
    case INSN_CLASS_XTHEADVECTOR:
      return riscv_subset_supports (rps, "xtheadvector");
    case INSN_CLASS_XTHEADVDOT:
      return riscv_subset_supports (rps, "xtheadvdot");
    case INSN_CLASS_XTHEADZVAMO:
      return riscv_subset_supports (rps, "xtheadzvamo");
    case INSN_CLASS_XVENTANACONDOPS:
      return riscv_subset_supports (rps, "xventanacondops");
    case INSN_CLASS_XSFVCP:
      return riscv_subset_supports (rps, "xsfvcp");
    case INSN_CLASS_XSFCEASE:
      return riscv_subset_supports (rps, "xsfcease");
    case INSN_CLASS_XSFVQMACCQOQ:
      return riscv_subset_supports (rps, "xsfvqmaccqoq");
    case INSN_CLASS_XSFVQMACCDOD:
      return riscv_subset_supports (rps, "xsfvqmaccdod");
    case INSN_CLASS_XSFVFNRCLIPXFQF:
      return riscv_subset_supports (rps, "xsfvfnrclipxfqf");
    case INSN_CLASS_XMIPSCBOP:
      return riscv_subset_supports (rps, "xmipscbop");
    case INSN_CLASS_XMIPSCMOV:
      return riscv_subset_supports (rps, "xmipscmov");
    case INSN_CLASS_XMIPSEXECTL:
      return riscv_subset_supports (rps, "xmipsexectl");
    case INSN_CLASS_XMIPSLSP:
      return riscv_subset_supports (rps, "xmipslsp");
    default:
      rps->error_handler
        (_("internal: unreachable INSN_CLASS_*"));
      return false;
    }
}

/* Each instuction is belonged to an instruction class INSN_CLASS_*.
   Call riscv_subset_supports_ext to determine the missing extension.  */

const char *
riscv_multi_subset_supports_ext (riscv_parse_subset_t *rps,
				 enum riscv_insn_class insn_class)
{
  switch (insn_class)
    {
    case INSN_CLASS_I:
      return "i";
    case INSN_CLASS_ZICBOM:
      return "zicbom";
    case INSN_CLASS_ZICBOP:
      return "zicbop";
    case INSN_CLASS_ZICBOZ:
      return "zicboz";
    case INSN_CLASS_ZICOND:
      return "zicond";
    case INSN_CLASS_ZICSR:
      return "zicsr";
    case INSN_CLASS_ZIFENCEI:
      return "zifencei";
    case INSN_CLASS_ZICFISS:
      return "zicfiss";
    case INSN_CLASS_ZICFISS_AND_ZCMOP:
      if (!riscv_subset_supports (rps, "zicfiss"))
	{
	  if (!riscv_subset_supports (rps, "zcmop"))
	    return _("zicfiss' and `zcmop");
	  else
	    return "zicfiss";
	}
      return "zcmop";
    case INSN_CLASS_ZICFILP:
      return "zicfilp";
    case INSN_CLASS_ZIHINTNTL:
      return "zihintntl";
    case INSN_CLASS_ZIHINTNTL_AND_ZCA:
      if (!riscv_subset_supports (rps, "zihintntl"))
	{
	  if (!riscv_subset_supports (rps, "zca"))
	    return _("zihintntl' and `c', or `zihintntl' and `zca");
	  else
	    return "zihintntl";
	}
      else
	return _("c' or `zca");
    case INSN_CLASS_ZIHINTPAUSE:
      return "zihintpause";
    case INSN_CLASS_ZIMOP:
      return "zimop";
    case INSN_CLASS_M:
      return "m";
    case INSN_CLASS_ZMMUL:
      return _ ("m' or `zmmul");
    case INSN_CLASS_ZAAMO:
      return "zaamo";
    case INSN_CLASS_ZABHA:
      return "zabha";
    case INSN_CLASS_ZACAS:
      return "zacas";
    case INSN_CLASS_ZALRSC:
      return "zalrsc";
    case INSN_CLASS_ZAWRS:
      return "zawrs";
    case INSN_CLASS_F:
      return "f";
    case INSN_CLASS_D:
      return "d";
    case INSN_CLASS_Q:
      return "q";
    case INSN_CLASS_ZCA:
      return _("c' or `zca");
    case INSN_CLASS_ZCF:
      return _("f' and `c', or `zcf");
    case INSN_CLASS_ZCD:
      return _("d' and `c', or `zcd");
    case INSN_CLASS_F_INX:
      return _("f' or `zfinx");
    case INSN_CLASS_D_INX:
      return _("d' or `zdinx");
    case INSN_CLASS_Q_INX:
      return _("q' or `zqinx");
    case INSN_CLASS_ZFH_INX:
      return _("zfh' or `zhinx");
    case INSN_CLASS_ZFHMIN:
      return "zfhmin";
    case INSN_CLASS_ZFHMIN_INX:
      return _("zfhmin' or `zhinxmin");
    case INSN_CLASS_ZFHMIN_AND_D_INX:
      if (riscv_subset_supports (rps, "zfhmin"))
	return "d";
      else if (riscv_subset_supports (rps, "d"))
	return "zfhmin";
      else if (riscv_subset_supports (rps, "zhinxmin"))
	return "zdinx";
      else if (riscv_subset_supports (rps, "zdinx"))
	return "zhinxmin";
      else
	return _("zfhmin' and `d', or `zhinxmin' and `zdinx");
    case INSN_CLASS_ZFHMIN_AND_Q_INX:
      if (riscv_subset_supports (rps, "zfhmin"))
	return "q";
      else if (riscv_subset_supports (rps, "q"))
	return "zfhmin";
      else if (riscv_subset_supports (rps, "zhinxmin"))
	return "zqinx";
      else if (riscv_subset_supports (rps, "zqinx"))
	return "zhinxmin";
      else
	return _("zfhmin' and `q', or `zhinxmin' and `zqinx");
    case INSN_CLASS_ZFBFMIN:
      return "zfbfmin";
    case INSN_CLASS_ZFA:
      return "zfa";
    case INSN_CLASS_D_AND_ZFA:
      if (!riscv_subset_supports (rps, "d")
	  && !riscv_subset_supports (rps, "zfa"))
	return _("d' and `zfa");
      else if (!riscv_subset_supports (rps, "d"))
	return "d";
      else
	return "zfa";
    case INSN_CLASS_Q_AND_ZFA:
      if (!riscv_subset_supports (rps, "q")
	  && !riscv_subset_supports (rps, "zfa"))
	return _("q' and `zfa");
      else if (!riscv_subset_supports (rps, "q"))
	return "q";
      else
	return "zfa";
    case INSN_CLASS_ZFH_AND_ZFA:
      if (!riscv_subset_supports (rps, "zfh")
	  && !riscv_subset_supports (rps, "zfa"))
	return _("zfh' and `zfa");
      else if (!riscv_subset_supports (rps, "zfh"))
	return "zfh";
      else
	return "zfa";
    case INSN_CLASS_ZFH_OR_ZVFH_AND_ZFA:
      if (!riscv_subset_supports (rps, "zfa"))
	{
	  if (!riscv_subset_supports (rps, "zfh")
	      && !riscv_subset_supports (rps, "zvfh"))
	    return _("zfh' and `zfa', or `zvfh' and `zfa");
	  else
	    return "zfa";
	}
      else
	return _("zfh' or `zvfh");
    case INSN_CLASS_ZBA:
      return "zba";
    case INSN_CLASS_ZBB:
      return "zbb";
    case INSN_CLASS_ZBC:
      return "zbc";
    case INSN_CLASS_ZBS:
      return "zbs";
    case INSN_CLASS_ZBKB:
      return "zbkb";
    case INSN_CLASS_ZBKC:
      return "zbkc";
    case INSN_CLASS_ZBKX:
      return "zbkx";
    case INSN_CLASS_ZBB_OR_ZBKB:
      return _("zbb' or `zbkb");
    case INSN_CLASS_ZBC_OR_ZBKC:
      return _("zbc' or `zbkc");
    case INSN_CLASS_ZKND:
      return "zknd";
    case INSN_CLASS_ZKNE:
      return "zkne";
    case INSN_CLASS_ZKNH:
      return "zknh";
    case INSN_CLASS_ZKND_OR_ZKNE:
      return _("zknd' or `zkne");
    case INSN_CLASS_ZKSED:
      return "zksed";
    case INSN_CLASS_ZKSH:
      return "zksh";
    case INSN_CLASS_V:
      return _("v' or `zve64x' or `zve32x");
    case INSN_CLASS_ZVEF:
      return _("v' or `zve64d' or `zve64f' or `zve32f");
    case INSN_CLASS_ZVBB:
      return _("zvbb");
    case INSN_CLASS_ZVBC:
      return _("zvbc");
    case INSN_CLASS_ZVFBFMIN:
      return "zvfbfmin";
    case INSN_CLASS_ZVFBFWMA:
      return "zvfbfwma";
    case INSN_CLASS_ZVKB:
      return _("zvkb");
    case INSN_CLASS_ZVKG:
      return _("zvkg");
    case INSN_CLASS_ZVKNED:
      return _("zvkned");
    case INSN_CLASS_ZVKNHA_OR_ZVKNHB:
      return _("zvknha' or `zvknhb");
    case INSN_CLASS_ZVKSED:
      return _("zvksed");
    case INSN_CLASS_ZVKSH:
      return _("zvksh");
    case INSN_CLASS_ZCB:
      return "zcb";
    case INSN_CLASS_ZCB_AND_ZBA:
      return _("zcb' and `zba");
    case INSN_CLASS_ZCB_AND_ZBB:
      return _("zcb' and `zbb");
    case INSN_CLASS_ZCB_AND_ZMMUL:
      return _("zcb' and `zmmul', or `zcb' and `m");
    case INSN_CLASS_ZCMOP:
      return "zcmop";
    case INSN_CLASS_ZCMP:
      return "zcmp";
    case INSN_CLASS_ZCMT:
      return "zcmt";
    case INSN_CLASS_SMCTR_OR_SSCTR:
      return _("smctr' or `ssctr");
    case INSN_CLASS_ZILSD:
     return "zilsd";
    case INSN_CLASS_ZCLSD:
      return "zclsd";
    case INSN_CLASS_SMRNMI:
      return "smrnmi";
    case INSN_CLASS_SVINVAL:
      return "svinval";
    case INSN_CLASS_H:
      return _("h");
    case INSN_CLASS_XCVALU:
      return "xcvalu";
    case INSN_CLASS_XCVBI:
      return "xcvbi";
    case INSN_CLASS_XCVBITMANIP:
      return "xcvbitmanip";
    case INSN_CLASS_XCVELW:
      return "xcvelw";
    case INSN_CLASS_XCVMAC:
      return "xcvmac";
    case INSN_CLASS_XCVMEM:
      return "xcvmem";
    case INSN_CLASS_XCVSIMD:
      return "xcvsimd";
    case INSN_CLASS_XTHEADBA:
      return "xtheadba";
    case INSN_CLASS_XTHEADBB:
      return "xtheadbb";
    case INSN_CLASS_XTHEADBS:
      return "xtheadbs";
    case INSN_CLASS_XTHEADCMO:
      return "xtheadcmo";
    case INSN_CLASS_XTHEADCONDMOV:
      return "xtheadcondmov";
    case INSN_CLASS_XTHEADFMEMIDX:
      return "xtheadfmemidx";
    case INSN_CLASS_XTHEADFMV:
      return "xtheadfmv";
    case INSN_CLASS_XTHEADINT:
      return "xtheadint";
    case INSN_CLASS_XTHEADMAC:
      return "xtheadmac";
    case INSN_CLASS_XTHEADMEMIDX:
      return "xtheadmemidx";
    case INSN_CLASS_XTHEADMEMPAIR:
      return "xtheadmempair";
    case INSN_CLASS_XTHEADSYNC:
      return "xtheadsync";
    case INSN_CLASS_XTHEADVECTOR:
      return "xtheadvector";
    case INSN_CLASS_XTHEADVDOT:
      return "xtheadvdot";
    case INSN_CLASS_XTHEADZVAMO:
      return "xtheadzvamo";
    case INSN_CLASS_XSFCEASE:
      return "xsfcease";
    default:
      rps->error_handler
        (_("internal: unreachable INSN_CLASS_*"));
      return NULL;
    }
}

/* Print supported extensions with versions if -march=help.  */

void
riscv_print_extensions (void)
{
  /* Record the previous printed extension.
     Print the current one if they are not the same.  */
  const struct riscv_supported_ext *cur = NULL, *prev = NULL;
  int i, j;

  printf ("All available -march extensions for RISC-V:");

  for (i = 0; riscv_all_supported_ext[i] != NULL; i++)
    {
      const struct riscv_supported_ext *exts = riscv_all_supported_ext[i];
      prev = NULL;
      for (j = 0; exts[j].name != NULL; j++)
	{
	  cur = &exts[j];
	  /* Unclear version information, skip.  */
	  if (cur->isa_spec_class == ISA_SPEC_CLASS_NONE
	      || cur->major_version == RISCV_UNKNOWN_VERSION
	      || cur->minor_version == RISCV_UNKNOWN_VERSION)
	    continue;

	  /* Same extension.  */
	  if (prev && strcmp (prev->name, cur->name) == 0)
	    {
	      /* Same version, skip.  */
	      if (prev->major_version == cur->major_version
		  && prev->minor_version == cur->minor_version)
		continue;
	      /* Different version, print version with comma.  */
	      else
		printf (", %d.%d", cur->major_version, cur->minor_version);
	    }
	  /* Different extension, print extension and version with newline.  */
	  else
	    printf ("\n\t%-40s%d.%d", cur->name, cur->major_version,
		    cur->minor_version);
	  prev = &exts[j];
	}
    }
  printf ("\n");
}

/* Find the first input bfd with GNU property and merge it with GPROP.  If no
   such input is found, add it to a new section at the last input.  Update
   GPROP accordingly.  */

bfd *
_bfd_riscv_elf_link_setup_gnu_properties (struct bfd_link_info *info,
					  uint32_t *and_prop_p)
{
  asection *sec;
  bfd *pbfd;
  bfd *ebfd = NULL;
  elf_property *prop;

  uint32_t and_prop = *and_prop_p;

  /* Find a normal input file with GNU property note.  */
  for (pbfd = info->input_bfds; pbfd != NULL; pbfd = pbfd->link.next)
    if (bfd_get_flavour (pbfd) == bfd_target_elf_flavour
	&& bfd_count_sections (pbfd) != 0)
      {
	ebfd = pbfd;

	if (elf_properties (pbfd) != NULL)
	  break;
      }

  /* If ebfd != NULL it is either an input with property note or the last
     input.  Either way if we have and_prop, we should add it (by
     creating a section if needed).  */
  if (ebfd != NULL && (and_prop))
    {
      prop = _bfd_elf_get_property (ebfd, GNU_PROPERTY_RISCV_FEATURE_1_AND, 4);

      prop->u.number |= and_prop;
      prop->pr_kind = property_number;

      /* pbfd being NULL implies ebfd is the last input.  Create the GNU
	 property note section.  */
      if (pbfd == NULL)
	{
	  sec
	    = bfd_make_section_with_flags (ebfd, NOTE_GNU_PROPERTY_SECTION_NAME,
					   (SEC_ALLOC | SEC_LOAD | SEC_IN_MEMORY
					    | SEC_READONLY | SEC_HAS_CONTENTS
					    | SEC_DATA));
	  if (sec == NULL)
	    info->callbacks->einfo (
	      _ ("%F%P: failed to create GNU property section\n"));

	  elf_section_type (sec) = SHT_NOTE;
	}
    }

  pbfd = _bfd_elf_link_setup_gnu_properties (info);

  if (bfd_link_relocatable (info))
    return pbfd;

  /* If pbfd has any GNU_PROPERTY_RISCV_FEATURE_1_AND properties, update
  and_prop accordingly.  */
  if (pbfd != NULL)
    {
      elf_property_list *p;
      elf_property_list *plist = elf_properties (pbfd);

      if ((p = _bfd_elf_find_property (plist, GNU_PROPERTY_RISCV_FEATURE_1_AND,
				       NULL))
	  != NULL)
	and_prop = p->property.u.number
		   & (GNU_PROPERTY_RISCV_FEATURE_1_CFI_LP_UNLABELED
		      | GNU_PROPERTY_RISCV_FEATURE_1_CFI_SS);
    }

  *and_prop_p = and_prop;
  return pbfd;
}

/* Define elf_backend_parse_gnu_properties for RISC-V.  */

enum elf_property_kind
_bfd_riscv_elf_parse_gnu_properties (bfd *abfd, unsigned int type,
				     bfd_byte *ptr, unsigned int datasz)
{
  elf_property *prop;

  switch (type)
    {
    case GNU_PROPERTY_RISCV_FEATURE_1_AND:
      if (datasz != 4)
	{
	  _bfd_error_handler (_ (
				"error: %pB: <corrupt RISC-V used size: 0x%x>"),
			      abfd, datasz);
	  return property_corrupt;
	}
      prop = _bfd_elf_get_property (abfd, type, datasz);
      /* Combine properties of the same type.  */
      prop->u.number |= bfd_h_get_32 (abfd, ptr);
      prop->pr_kind = property_number;
      break;

    default:
      return property_ignored;
    }

  return property_number;
}

/* Merge RISC-V GNU property BPROP with APROP also accounting for PROP.
   If APROP isn't NULL, merge it with BPROP and/or PROP.  Vice-versa if BROP
   isn't NULL.  Return TRUE if there is any update to APROP or if BPROP should
   be merge with ABFD.  */

bool
_bfd_riscv_elf_merge_gnu_properties
  (struct bfd_link_info *info ATTRIBUTE_UNUSED, bfd *abfd ATTRIBUTE_UNUSED,
   elf_property *aprop, elf_property *bprop, uint32_t and_prop)
{
  unsigned int orig_number;
  bool updated = false;
  unsigned int pr_type = aprop != NULL ? aprop->pr_type : bprop->pr_type;

  switch (pr_type)
    {
      case GNU_PROPERTY_RISCV_FEATURE_1_AND: {
	if (aprop != NULL && bprop != NULL)
	  {
	    orig_number = aprop->u.number;
	    aprop->u.number = (orig_number & bprop->u.number) | and_prop;
	    updated = orig_number != aprop->u.number;
	    /* Remove the property if all feature bits are cleared.  */
	    if (aprop->u.number == 0)
	      aprop->pr_kind = property_remove;
	    break;
	  }
	/* If either is NULL, the AND would be 0 so, if there is
	   any PROP, asign it to the input that is not NULL.  */
	if (and_prop)
	  {
	    if (aprop != NULL)
	      {
		orig_number = aprop->u.number;
		aprop->u.number = and_prop;
		updated = orig_number != aprop->u.number;
	      }
	    else if (bprop != NULL)
	      {
		bprop->u.number = and_prop;
		updated = true;
	      }
	    /* Shouldn't happen because we checked one of APROP or BPROP !=
	     * NULL. */
	    else
	      abort ();
	  }
	/* No PROP and BPROP is NULL, so remove APROP.  */
	else if (!and_prop && bprop == NULL && aprop != NULL)
	  {
	    aprop->pr_kind = property_remove;
	    updated = true;
	  }
      }
      break;

    default:
      abort ();
    }

  return updated;
}
