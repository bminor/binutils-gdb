/* MMIX-specific support for 64-bit ELF.
   Copyright 2001, 2002 Free Software Foundation, Inc.
   Contributed by Hans-Peter Nilsson <hp@bitrange.com>

This file is part of BFD, the Binary File Descriptor library.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

/* No specific ABI or "processor-specific supplement" defined.  */

/* TODO:
   - Linker relaxation.
   - On-demand register allocation (from R_MMIX_BASE_PLUS_OFFSET).  */

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"
#include "elf-bfd.h"
#include "elf/mmix.h"
#include "opcode/mmix.h"

#define MINUS_ONE	(((bfd_vma) 0) - 1)

/* Put these everywhere in new code.  */
#define FATAL_DEBUG						\
 _bfd_abort (__FILE__, __LINE__,				\
	     "Internal: Non-debugged code (test-case missing)")

#define BAD_CASE(x)				\
 _bfd_abort (__FILE__, __LINE__,		\
	     "bad case for " #x)

static boolean mmix_elf_link_output_symbol_hook
  PARAMS ((bfd *, struct bfd_link_info *, const char *,
	   Elf_Internal_Sym *, asection *));

static bfd_reloc_status_type mmix_elf_reloc
  PARAMS ((bfd *, arelent *, asymbol *, PTR, asection *, bfd *, char **));

static reloc_howto_type *bfd_elf64_bfd_reloc_type_lookup
  PARAMS ((bfd *, bfd_reloc_code_real_type));

static void mmix_info_to_howto_rela
  PARAMS ((bfd *, arelent *, Elf64_Internal_Rela *));

static int mmix_elf_sort_relocs PARAMS ((const PTR, const PTR));

static boolean mmix_elf_check_relocs
  PARAMS ((bfd *, struct bfd_link_info *, asection *,
	   const Elf_Internal_Rela *));

static boolean mmix_elf_relocate_section
  PARAMS ((bfd *, struct bfd_link_info *, bfd *, asection *, bfd_byte *,
	   Elf_Internal_Rela *, Elf_Internal_Sym *, asection **));

static asection * mmix_elf_gc_mark_hook
  PARAMS ((bfd *, struct bfd_link_info *, Elf_Internal_Rela *,
	   struct elf_link_hash_entry *, Elf_Internal_Sym *));

static bfd_reloc_status_type mmix_final_link_relocate
  PARAMS ((reloc_howto_type *, asection *, bfd_byte *,
	   bfd_vma, bfd_signed_vma, bfd_vma, const char *, asection *));

static bfd_reloc_status_type mmix_elf_perform_relocation
  PARAMS ((asection *, reloc_howto_type *, PTR, bfd_vma, bfd_vma));

static boolean mmix_elf_section_from_bfd_section
  PARAMS ((bfd *, asection *, int *));

static boolean mmix_elf_add_symbol_hook
  PARAMS ((bfd *, struct bfd_link_info *, const Elf_Internal_Sym *,
	   const char **, flagword *, asection **, bfd_vma *));

static boolean mmix_elf_is_local_label_name
  PARAMS ((bfd *, const char *));

extern boolean mmix_elf_final_link PARAMS ((bfd *, struct bfd_link_info *));

extern void mmix_elf_symbol_processing PARAMS ((bfd *, asymbol *));

/* Watch out: this currently needs to have elements with the same index as
   their R_MMIX_ number.  */
static reloc_howto_type elf_mmix_howto_table[] =
 {
  /* This reloc does nothing.  */
  HOWTO (R_MMIX_NONE,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_MMIX_NONE",		/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 false),		/* pcrel_offset */

  /* An 8 bit absolute relocation.  */
  HOWTO (R_MMIX_8,		/* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 8,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_MMIX_8",		/* name */
	 false,			/* partial_inplace */
	 0xff,			/* src_mask */
	 0xff,			/* dst_mask */
	 false),		/* pcrel_offset */

  /* An 16 bit absolute relocation.  */
  HOWTO (R_MMIX_16,		/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_MMIX_16",		/* name */
	 false,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* An 24 bit absolute relocation.  */
  HOWTO (R_MMIX_24,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 24,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_MMIX_24",		/* name */
	 false,			/* partial_inplace */
	 0xffffff,		/* src_mask */
	 0xffffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* A 32 bit absolute relocation.  */
  HOWTO (R_MMIX_32,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_MMIX_32",		/* name */
	 false,			/* partial_inplace */
	 0xffffffff,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* 64 bit relocation.  */
  HOWTO (R_MMIX_64,		/* type */
	 0,			/* rightshift */
	 4,			/* size (0 = byte, 1 = short, 2 = long) */
	 64,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_MMIX_64",		/* name */
	 false,			/* partial_inplace */
	 MINUS_ONE,		/* src_mask */
	 MINUS_ONE,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* An 8 bit PC-relative relocation.  */
  HOWTO (R_MMIX_PC_8,		/* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 8,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_MMIX_PC_8",		/* name */
	 false,			/* partial_inplace */
	 0xff,			/* src_mask */
	 0xff,			/* dst_mask */
	 true),			/* pcrel_offset */

  /* An 16 bit PC-relative relocation.  */
  HOWTO (R_MMIX_PC_16,		/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_MMIX_PC_16",	/* name */
	 false,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 true),			/* pcrel_offset */

  /* An 24 bit PC-relative relocation.  */
  HOWTO (R_MMIX_PC_24,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 24,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_MMIX_PC_24",	/* name */
	 false,			/* partial_inplace */
	 0xffffff,		/* src_mask */
	 0xffffff,		/* dst_mask */
	 true),			/* pcrel_offset */

  /* A 32 bit absolute PC-relative relocation.  */
  HOWTO (R_MMIX_PC_32,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_MMIX_PC_32",	/* name */
	 false,			/* partial_inplace */
	 0xffffffff,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 true),			/* pcrel_offset */

  /* 64 bit PC-relative relocation.  */
  HOWTO (R_MMIX_PC_64,		/* type */
	 0,			/* rightshift */
	 4,			/* size (0 = byte, 1 = short, 2 = long) */
	 64,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_MMIX_PC_64",	/* name */
	 false,			/* partial_inplace */
	 MINUS_ONE,		/* src_mask */
	 MINUS_ONE,		/* dst_mask */
	 true),			/* pcrel_offset */

  /* GNU extension to record C++ vtable hierarchy.  */
  HOWTO (R_MMIX_GNU_VTINHERIT, /* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 0,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 NULL,			/* special_function */
	 "R_MMIX_GNU_VTINHERIT", /* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 true),			/* pcrel_offset */

  /* GNU extension to record C++ vtable member usage.  */
  HOWTO (R_MMIX_GNU_VTENTRY,	/* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 0,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 _bfd_elf_rel_vtable_reloc_fn,	/* special_function */
	 "R_MMIX_GNU_VTENTRY", /* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 false),		/* pcrel_offset */

  /* The GETA relocation is supposed to get any address that could
     possibly be reached by the GETA instruction.  It can silently expand
     to get a 64-bit operand, but will complain if any of the two least
     significant bits are set.  The howto members reflect a simple GETA.  */
  HOWTO (R_MMIX_GETA,		/* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 19,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 mmix_elf_reloc,	/* special_function */
	 "R_MMIX_GETA",		/* name */
	 false,			/* partial_inplace */
	 0x0100ffff,		/* src_mask */
	 0x0100ffff,		/* dst_mask */
	 true),			/* pcrel_offset */

  HOWTO (R_MMIX_GETA_1,		/* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 19,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 mmix_elf_reloc,	/* special_function */
	 "R_MMIX_GETA_1",		/* name */
	 false,			/* partial_inplace */
	 0x0100ffff,		/* src_mask */
	 0x0100ffff,		/* dst_mask */
	 true),			/* pcrel_offset */

  HOWTO (R_MMIX_GETA_2,		/* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 19,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 mmix_elf_reloc,	/* special_function */
	 "R_MMIX_GETA_2",		/* name */
	 false,			/* partial_inplace */
	 0x0100ffff,		/* src_mask */
	 0x0100ffff,		/* dst_mask */
	 true),			/* pcrel_offset */

  HOWTO (R_MMIX_GETA_3,		/* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 19,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 mmix_elf_reloc,	/* special_function */
	 "R_MMIX_GETA_3",		/* name */
	 false,			/* partial_inplace */
	 0x0100ffff,		/* src_mask */
	 0x0100ffff,		/* dst_mask */
	 true),			/* pcrel_offset */

  /* The conditional branches are supposed to reach any (code) address.
     It can silently expand to a 64-bit operand, but will emit an error if
     any of the two least significant bits are set.  The howto members
     reflect a simple branch.  */
  HOWTO (R_MMIX_CBRANCH,	/* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 19,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 mmix_elf_reloc,	/* special_function */
	 "R_MMIX_CBRANCH",	/* name */
	 false,			/* partial_inplace */
	 0x0100ffff,		/* src_mask */
	 0x0100ffff,		/* dst_mask */
	 true),		       	/* pcrel_offset */

  HOWTO (R_MMIX_CBRANCH_J,	/* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 19,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 mmix_elf_reloc,	/* special_function */
	 "R_MMIX_CBRANCH_J",	/* name */
	 false,			/* partial_inplace */
	 0x0100ffff,		/* src_mask */
	 0x0100ffff,		/* dst_mask */
	 true),			/* pcrel_offset */

  HOWTO (R_MMIX_CBRANCH_1,	/* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 19,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 mmix_elf_reloc,	/* special_function */
	 "R_MMIX_CBRANCH_1",	/* name */
	 false,			/* partial_inplace */
	 0x0100ffff,		/* src_mask */
	 0x0100ffff,		/* dst_mask */
	 true),			/* pcrel_offset */

  HOWTO (R_MMIX_CBRANCH_2,	/* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 19,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 mmix_elf_reloc,	/* special_function */
	 "R_MMIX_CBRANCH_2",	/* name */
	 false,			/* partial_inplace */
	 0x0100ffff,		/* src_mask */
	 0x0100ffff,		/* dst_mask */
	 true),			/* pcrel_offset */

  HOWTO (R_MMIX_CBRANCH_3,	/* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 19,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 mmix_elf_reloc,	/* special_function */
	 "R_MMIX_CBRANCH_3",	/* name */
	 false,			/* partial_inplace */
	 0x0100ffff,		/* src_mask */
	 0x0100ffff,		/* dst_mask */
	 true),			/* pcrel_offset */

  /* The PUSHJ instruction can reach any (code) address, as long as it's
     the beginning of a function (no usable restriction).  It can silently
     expand to a 64-bit operand, but will emit an error if any of the two
     least significant bits are set.  The howto members reflect a simple
     PUSHJ.  */
  HOWTO (R_MMIX_PUSHJ,		/* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 19,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 mmix_elf_reloc,	/* special_function */
	 "R_MMIX_PUSHJ",	/* name */
	 false,			/* partial_inplace */
	 0x0100ffff,		/* src_mask */
	 0x0100ffff,		/* dst_mask */
	 true),			/* pcrel_offset */

  HOWTO (R_MMIX_PUSHJ_1,	/* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 19,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 mmix_elf_reloc,	/* special_function */
	 "R_MMIX_PUSHJ_1",	/* name */
	 false,			/* partial_inplace */
	 0x0100ffff,		/* src_mask */
	 0x0100ffff,		/* dst_mask */
	 true),			/* pcrel_offset */

  HOWTO (R_MMIX_PUSHJ_2,	/* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 19,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 mmix_elf_reloc,	/* special_function */
	 "R_MMIX_PUSHJ_2",	/* name */
	 false,			/* partial_inplace */
	 0x0100ffff,		/* src_mask */
	 0x0100ffff,		/* dst_mask */
	 true),			/* pcrel_offset */

  HOWTO (R_MMIX_PUSHJ_3,	/* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 19,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 mmix_elf_reloc,	/* special_function */
	 "R_MMIX_PUSHJ_3",	/* name */
	 false,			/* partial_inplace */
	 0x0100ffff,		/* src_mask */
	 0x0100ffff,		/* dst_mask */
	 true),			/* pcrel_offset */

  /* A JMP is supposed to reach any (code) address.  By itself, it can
     reach +-64M; the expansion can reach all 64 bits.  Note that the 64M
     limit is soon reached if you link the program in wildly different
     memory segments.  The howto members reflect a trivial JMP.  */
  HOWTO (R_MMIX_JMP,		/* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 27,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 mmix_elf_reloc,	/* special_function */
	 "R_MMIX_JMP",		/* name */
	 false,			/* partial_inplace */
	 0x1ffffff,		/* src_mask */
	 0x1ffffff,		/* dst_mask */
	 true),			/* pcrel_offset */

  HOWTO (R_MMIX_JMP_1,		/* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 27,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 mmix_elf_reloc,	/* special_function */
	 "R_MMIX_JMP_1",	/* name */
	 false,			/* partial_inplace */
	 0x1ffffff,		/* src_mask */
	 0x1ffffff,		/* dst_mask */
	 true),			/* pcrel_offset */

  HOWTO (R_MMIX_JMP_2,		/* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 27,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 mmix_elf_reloc,	/* special_function */
	 "R_MMIX_JMP_2",	/* name */
	 false,			/* partial_inplace */
	 0x1ffffff,		/* src_mask */
	 0x1ffffff,		/* dst_mask */
	 true),			/* pcrel_offset */

  HOWTO (R_MMIX_JMP_3,		/* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 27,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 mmix_elf_reloc,	/* special_function */
	 "R_MMIX_JMP_3",	/* name */
	 false,			/* partial_inplace */
	 0x1ffffff,		/* src_mask */
	 0x1ffffff,		/* dst_mask */
	 true),			/* pcrel_offset */

  /* When we don't emit link-time-relaxable code from the assembler, or
     when relaxation has done all it can do, these relocs are used.  For
     GETA/PUSHJ/branches.  */
  HOWTO (R_MMIX_ADDR19,		/* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 19,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 mmix_elf_reloc,	/* special_function */
	 "R_MMIX_ADDR19",	/* name */
	 false,			/* partial_inplace */
	 0x0100ffff,		/* src_mask */
	 0x0100ffff,		/* dst_mask */
	 true),			/* pcrel_offset */

  /* For JMP.  */
  HOWTO (R_MMIX_ADDR27,		/* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 27,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 mmix_elf_reloc,	/* special_function */
	 "R_MMIX_ADDR27",	/* name */
	 false,			/* partial_inplace */
	 0x1ffffff,		/* src_mask */
	 0x1ffffff,		/* dst_mask */
	 true),			/* pcrel_offset */

  /* A general register or the value 0..255.  If a value, then the
     instruction (offset -3) needs adjusting.  */
  HOWTO (R_MMIX_REG_OR_BYTE,	/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 8,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 mmix_elf_reloc,	/* special_function */
	 "R_MMIX_REG_OR_BYTE",	/* name */
	 false,			/* partial_inplace */
	 0xff,			/* src_mask */
	 0xff,			/* dst_mask */
	 false),		/* pcrel_offset */

  /* A general register.  */
  HOWTO (R_MMIX_REG,		/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 8,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 mmix_elf_reloc,	/* special_function */
	 "R_MMIX_REG",		/* name */
	 false,			/* partial_inplace */
	 0xff,			/* src_mask */
	 0xff,			/* dst_mask */
	 false),		/* pcrel_offset */

  /* A register plus an index, corresponding to the relocation expression.
     The sizes must correspond to the valid range of the expression, while
     the bitmasks correspond to what we store in the image.  */
  HOWTO (R_MMIX_BASE_PLUS_OFFSET,	/* type */
	 0,			/* rightshift */
	 4,			/* size (0 = byte, 1 = short, 2 = long) */
	 64,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 mmix_elf_reloc,	/* special_function */
	 "R_MMIX_BASE_PLUS_OFFSET", /* name */
	 false,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* A "magic" relocation for a LOCAL expression, asserting that the
     expression is less than the number of global registers.  No actual
     modification of the contents is done.  Implementing this as a
     relocation was less intrusive than e.g. putting such expressions in a
     section to discard *after* relocation.  */
  HOWTO (R_MMIX_LOCAL,		/* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 0,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 mmix_elf_reloc,	/* special_function */
	 "R_MMIX_LOCAL",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 false),		/* pcrel_offset */
 };


/* Map BFD reloc types to MMIX ELF reloc types.  */

struct mmix_reloc_map
  {
    bfd_reloc_code_real_type bfd_reloc_val;
    enum elf_mmix_reloc_type elf_reloc_val;
  };


static const struct mmix_reloc_map mmix_reloc_map[] =
  {
    {BFD_RELOC_NONE, R_MMIX_NONE},
    {BFD_RELOC_8, R_MMIX_8},
    {BFD_RELOC_16, R_MMIX_16},
    {BFD_RELOC_24, R_MMIX_24},
    {BFD_RELOC_32, R_MMIX_32},
    {BFD_RELOC_64, R_MMIX_64},
    {BFD_RELOC_8_PCREL, R_MMIX_PC_8},
    {BFD_RELOC_16_PCREL, R_MMIX_PC_16},
    {BFD_RELOC_24_PCREL, R_MMIX_PC_24},
    {BFD_RELOC_32_PCREL, R_MMIX_PC_32},
    {BFD_RELOC_64_PCREL, R_MMIX_PC_64},
    {BFD_RELOC_VTABLE_INHERIT, R_MMIX_GNU_VTINHERIT},
    {BFD_RELOC_VTABLE_ENTRY, R_MMIX_GNU_VTENTRY},
    {BFD_RELOC_MMIX_GETA, R_MMIX_GETA},
    {BFD_RELOC_MMIX_CBRANCH, R_MMIX_CBRANCH},
    {BFD_RELOC_MMIX_PUSHJ, R_MMIX_PUSHJ},
    {BFD_RELOC_MMIX_JMP, R_MMIX_JMP},
    {BFD_RELOC_MMIX_ADDR19, R_MMIX_ADDR19},
    {BFD_RELOC_MMIX_ADDR27, R_MMIX_ADDR27},
    {BFD_RELOC_MMIX_REG_OR_BYTE, R_MMIX_REG_OR_BYTE},
    {BFD_RELOC_MMIX_REG, R_MMIX_REG},
    {BFD_RELOC_MMIX_BASE_PLUS_OFFSET, R_MMIX_BASE_PLUS_OFFSET},
    {BFD_RELOC_MMIX_LOCAL, R_MMIX_LOCAL}
  };

static reloc_howto_type *
bfd_elf64_bfd_reloc_type_lookup (abfd, code)
     bfd *abfd ATTRIBUTE_UNUSED;
     bfd_reloc_code_real_type code;
{
  unsigned int i;

  for (i = 0;
       i < sizeof (mmix_reloc_map) / sizeof (mmix_reloc_map[0]);
       i++)
    {
      if (mmix_reloc_map[i].bfd_reloc_val == code)
	return &elf_mmix_howto_table[mmix_reloc_map[i].elf_reloc_val];
    }

  return NULL;
}


/* This function performs the actual bitfiddling and sanity check for a
   final relocation.  Each relocation gets its *worst*-case expansion
   in size when it arrives here; any reduction in size should have been
   caught in linker relaxation earlier.  When we get here, the relocation
   looks like the smallest instruction with SWYM:s (nop:s) appended to the
   max size.  We fill in those nop:s.

   R_MMIX_GETA: (FIXME: Relaxation should break this up in 1, 2, 3 tetra)
    GETA $N,foo
   ->
    SETL $N,foo & 0xffff
    INCML $N,(foo >> 16) & 0xffff
    INCMH $N,(foo >> 32) & 0xffff
    INCH $N,(foo >> 48) & 0xffff

   R_MMIX_CBRANCH: (FIXME: Relaxation should break this up, but
   condbranches needing relaxation might be rare enough to not be
   worthwhile.)
    [P]Bcc $N,foo
   ->
    [~P]B~cc $N,.+20
    SETL $255,foo & ...
    INCML ...
    INCMH ...
    INCH ...
    GO $255,$255,0

   R_MMIX_PUSHJ: (FIXME: Relaxation...)
    PUSHJ $N,foo
   ->
    SETL $255,foo & ...
    INCML ...
    INCMH ...
    INCH ...
    PUSHGO $N,$255,0

   R_MMIX_JMP: (FIXME: Relaxation...)
    JMP foo
   ->
    SETL $255,foo & ...
    INCML ...
    INCMH ...
    INCH ...
    GO $255,$255,0

   R_MMIX_ADDR19 and R_MMIX_ADDR27 are just filled in.  */

static bfd_reloc_status_type
mmix_elf_perform_relocation (isec, howto, datap, addr, value)
     asection *isec;
     reloc_howto_type *howto;
     PTR datap;
     bfd_vma addr ATTRIBUTE_UNUSED;
     bfd_vma value;
{
  bfd *abfd = isec->owner;
  bfd_reloc_status_type flag = bfd_reloc_ok;
  bfd_reloc_status_type r;
  int offs = 0;
  int reg = 255;

  /* The worst case bits are all similar SETL/INCML/INCMH/INCH sequences.
     We handle the differences here and the common sequence later.  */
  switch (howto->type)
    {
    case R_MMIX_GETA:
      offs = 0;
      reg = bfd_get_8 (abfd, (bfd_byte *) datap + 1);

      /* We change to an absolute value.  */
      value += addr;
      break;

    case R_MMIX_CBRANCH:
      {
	int in1 = bfd_get_16 (abfd, (bfd_byte *) datap) << 16;

	/* Invert the condition and prediction bit, and set the offset
	   to five instructions ahead.

	   We *can* do better if we want to.  If the branch is found to be
	   within limits, we could leave the branch as is; there'll just
	   be a bunch of NOP:s after it.  But we shouldn't see this
	   sequence often enough that it's worth doing it.  */

	bfd_put_32 (abfd,
		    (((in1 ^ ((PRED_INV_BIT | COND_INV_BIT) << 24)) & ~0xffff)
		     | (24/4)),
		    (bfd_byte *) datap);

	/* Put a "GO $255,$255,0" after the common sequence.  */
	bfd_put_32 (abfd,
		    ((GO_INSN_BYTE | IMM_OFFSET_BIT) << 24) | 0xffff00,
		    (bfd_byte *) datap + 20);

	/* Common sequence starts at offset 4.  */
	offs = 4;

	/* We change to an absolute value.  */
	value += addr;
      }
      break;

    case R_MMIX_PUSHJ:
      {
	int inreg = bfd_get_8 (abfd, (bfd_byte *) datap + 1);

	/* Put a "PUSHGO $N,$255,0" after the common sequence.  */
	bfd_put_32 (abfd,
		    ((PUSHGO_INSN_BYTE | IMM_OFFSET_BIT) << 24)
		    | (inreg << 16)
		    | 0xff00,
		    (bfd_byte *) datap + 16);

	/* We change to an absolute value.  */
	value += addr;
      }
      break;

    case R_MMIX_JMP:
      /* This one is a little special.  If we get here on a non-relaxing
	 link, and the destination is actually in range, we don't need to
	 execute the nops.
	 If so, we fall through to the bit-fiddling relocs.

	 FIXME: bfd_check_overflow seems broken; the relocation is
	 rightshifted before testing, so supply a zero rightshift.  */

      if (! ((value & 3) == 0
	     && (r = bfd_check_overflow (complain_overflow_signed,
					 howto->bitsize,
					 0,
					 bfd_arch_bits_per_address (abfd),
					 value)) == bfd_reloc_ok))
	{
	  /* If the relocation doesn't fit in a JMP, we let the NOP:s be
	     modified below, and put a "GO $255,$255,0" after the
	     address-loading sequence.  */
	  bfd_put_32 (abfd,
		      ((GO_INSN_BYTE | IMM_OFFSET_BIT) << 24)
		      | 0xffff00,
		      (bfd_byte *) datap + 16);

	  /* We change to an absolute value.  */
	  value += addr;
	  break;
	}
      /* FALLTHROUGH. */
    case R_MMIX_ADDR19:
    case R_MMIX_ADDR27:
      /* These must be in range, or else we emit an error.  */
      if ((value & 3) == 0
	  /* Note rightshift 0; see above.  */
	  && (r = bfd_check_overflow (complain_overflow_signed,
				      howto->bitsize,
				      0,
				      bfd_arch_bits_per_address (abfd),
				      value)) == bfd_reloc_ok)
	{
	  bfd_vma in1
	    = bfd_get_32 (abfd, (bfd_byte *) datap);
	  bfd_vma highbit;

	  if ((bfd_signed_vma) value < 0)
	    {
	      highbit = (1 << 24);
	      value += (1 << (howto->bitsize - 1));
	    }
	  else
	    highbit = 0;

	  value >>= 2;

	  bfd_put_32 (abfd,
		      (in1 & ~howto->src_mask)
		      | highbit
		      | (value & howto->dst_mask),
		      (bfd_byte *) datap);

	  return bfd_reloc_ok;
	}
      else
	return bfd_reloc_overflow;

    case R_MMIX_REG_OR_BYTE:
    case R_MMIX_REG:
      if (value > 255)
	return bfd_reloc_overflow;
      bfd_put_8 (abfd, value, datap);
      return bfd_reloc_ok;

    default:
      BAD_CASE (howto->type);
    }

  /* This code adds the common SETL/INCML/INCMH/INCH worst-case
     sequence.  */

  /* Lowest two bits must be 0.  We return bfd_reloc_overflow for
     everything that looks strange.  */
  if (value & 3)
    flag = bfd_reloc_overflow;

  bfd_put_32 (abfd,
	      (SETL_INSN_BYTE << 24) | (value & 0xffff) | (reg << 16),
	      (bfd_byte *) datap + offs);
  bfd_put_32 (abfd,
	      (INCML_INSN_BYTE << 24) | ((value >> 16) & 0xffff) | (reg << 16),
	      (bfd_byte *) datap + offs + 4);
  bfd_put_32 (abfd,
	      (INCMH_INSN_BYTE << 24) | ((value >> 32) & 0xffff) | (reg << 16),
	      (bfd_byte *) datap + offs + 8);
  bfd_put_32 (abfd,
	      (INCH_INSN_BYTE << 24) | ((value >> 48) & 0xffff) | (reg << 16),
	      (bfd_byte *) datap + offs + 12);

  return flag;
}

/* Set the howto pointer for an MMIX ELF reloc (type RELA).  */

static void
mmix_info_to_howto_rela (abfd, cache_ptr, dst)
     bfd *abfd ATTRIBUTE_UNUSED;
     arelent *cache_ptr;
     Elf64_Internal_Rela *dst;
{
  unsigned int r_type;

  r_type = ELF64_R_TYPE (dst->r_info);
  BFD_ASSERT (r_type < (unsigned int) R_MMIX_max);
  cache_ptr->howto = &elf_mmix_howto_table[r_type];
}

/* Any MMIX-specific relocation gets here at assembly time or when linking
   to other formats (such as mmo); this is the relocation function from
   the reloc_table.  We don't get here for final pure ELF linking.  */

static bfd_reloc_status_type
mmix_elf_reloc (abfd, reloc_entry, symbol, data, input_section,
		output_bfd, error_message)
     bfd *abfd;
     arelent *reloc_entry;
     asymbol *symbol;
     PTR data;
     asection *input_section;
     bfd *output_bfd;
     char **error_message ATTRIBUTE_UNUSED;
{
  bfd_vma relocation;
  bfd_reloc_status_type r;
  asection *reloc_target_output_section;
  bfd_reloc_status_type flag = bfd_reloc_ok;
  bfd_vma output_base = 0;
  bfd_vma addr;

  r = bfd_elf_generic_reloc (abfd, reloc_entry, symbol, data,
			     input_section, output_bfd, error_message);

  /* If that was all that was needed (i.e. this isn't a final link, only
     some segment adjustments), we're done.  */
  if (r != bfd_reloc_continue)
    return r;

  if (bfd_is_und_section (symbol->section)
      && (symbol->flags & BSF_WEAK) == 0
      && output_bfd == (bfd *) NULL)
    return bfd_reloc_undefined;

  /* Is the address of the relocation really within the section?  */
  if (reloc_entry->address > input_section->_cooked_size)
    return bfd_reloc_outofrange;

  /* Work out which section the relocation is targetted at and the
     initial relocation command value.  */

  /* Get symbol value.  (Common symbols are special.)  */
  if (bfd_is_com_section (symbol->section))
    relocation = 0;
  else
    relocation = symbol->value;

  reloc_target_output_section = bfd_get_output_section (symbol);

  /* Here the variable relocation holds the final address of the symbol we
     are relocating against, plus any addend.  */
  if (output_bfd)
    output_base = 0;
  else
    output_base = reloc_target_output_section->vma;

  relocation += output_base + symbol->section->output_offset;

  /* Get position of relocation.  */
  addr = (reloc_entry->address + input_section->output_section->vma
	  + input_section->output_offset);
  if (output_bfd != (bfd *) NULL)
    {
      /* Add in supplied addend.  */
      relocation += reloc_entry->addend;

      /* This is a partial relocation, and we want to apply the
	 relocation to the reloc entry rather than the raw data.
	 Modify the reloc inplace to reflect what we now know.  */
      reloc_entry->addend = relocation;
      reloc_entry->address += input_section->output_offset;
      return flag;
    }

  return mmix_final_link_relocate (reloc_entry->howto, input_section,
				   data, reloc_entry->address,
				   reloc_entry->addend, relocation,
				   bfd_asymbol_name (symbol),
				   reloc_target_output_section);
}

/* Relocate an MMIX ELF section.  Modified from elf32-fr30.c; look to it
   for guidance if you're thinking of copying this.  */

static boolean
mmix_elf_relocate_section (output_bfd, info, input_bfd, input_section,
			   contents, relocs, local_syms, local_sections)
     bfd *output_bfd ATTRIBUTE_UNUSED;
     struct bfd_link_info *info;
     bfd *input_bfd;
     asection *input_section;
     bfd_byte *contents;
     Elf_Internal_Rela *relocs;
     Elf_Internal_Sym *local_syms;
     asection **local_sections;
{
  Elf_Internal_Shdr *symtab_hdr;
  struct elf_link_hash_entry **sym_hashes;
  Elf_Internal_Rela *rel;
  Elf_Internal_Rela *relend;

  symtab_hdr = &elf_tdata (input_bfd)->symtab_hdr;
  sym_hashes = elf_sym_hashes (input_bfd);
  relend = relocs + input_section->reloc_count;

  for (rel = relocs; rel < relend; rel ++)
    {
      reloc_howto_type *howto;
      unsigned long r_symndx;
      Elf_Internal_Sym *sym;
      asection *sec;
      struct elf_link_hash_entry *h;
      bfd_vma relocation;
      bfd_reloc_status_type r;
      const char *name = NULL;
      int r_type;
      boolean undefined_signalled = false;

      r_type = ELF64_R_TYPE (rel->r_info);

      if (r_type == R_MMIX_GNU_VTINHERIT
	  || r_type == R_MMIX_GNU_VTENTRY)
	continue;

      r_symndx = ELF64_R_SYM (rel->r_info);

      if (info->relocateable)
	{
	  /* This is a relocateable link.  We don't have to change
             anything, unless the reloc is against a section symbol,
             in which case we have to adjust according to where the
             section symbol winds up in the output section.  */
	  if (r_symndx < symtab_hdr->sh_info)
	    {
	      sym = local_syms + r_symndx;

	      if (ELF_ST_TYPE (sym->st_info) == STT_SECTION)
		{
		  sec = local_sections [r_symndx];
		  rel->r_addend += sec->output_offset + sym->st_value;
		}
	    }

	  continue;
	}

      /* This is a final link.  */
      howto = elf_mmix_howto_table + ELF64_R_TYPE (rel->r_info);
      h = NULL;
      sym = NULL;
      sec = NULL;

      if (r_symndx < symtab_hdr->sh_info)
	{
	  sym = local_syms + r_symndx;
	  sec = local_sections [r_symndx];
	  relocation = _bfd_elf_rela_local_sym (output_bfd, sym, sec, rel);

	  name = bfd_elf_string_from_elf_section
	    (input_bfd, symtab_hdr->sh_link, sym->st_name);
	  name = (name == NULL) ? bfd_section_name (input_bfd, sec) : name;
	}
      else
	{
	  h = sym_hashes [r_symndx - symtab_hdr->sh_info];

	  while (h->root.type == bfd_link_hash_indirect
		 || h->root.type == bfd_link_hash_warning)
	    h = (struct elf_link_hash_entry *) h->root.u.i.link;

	  name = h->root.root.string;

	  if (h->root.type == bfd_link_hash_defined
	      || h->root.type == bfd_link_hash_defweak)
	    {
	      sec = h->root.u.def.section;
	      relocation = (h->root.u.def.value
			    + sec->output_section->vma
			    + sec->output_offset);
	    }
	  else if (h->root.type == bfd_link_hash_undefweak)
	    relocation = 0;
	  else if (info->shared
		   && ELF_ST_VISIBILITY (h->other) == STV_DEFAULT)
	    relocation = 0;
	  else
	    {
	      /* The test on undefined_signalled is redundant at the
		 moment, but kept for symmetry.  */
	      if (! undefined_signalled
		  && ! ((*info->callbacks->undefined_symbol)
			(info, h->root.root.string, input_bfd,
			 input_section, rel->r_offset, true)))
		return false;
	      undefined_signalled = true;
	      relocation = 0;
	    }
	}

      r = mmix_final_link_relocate (howto, input_section,
				    contents, rel->r_offset,
				    rel->r_addend, relocation, name, sec);

      if (r != bfd_reloc_ok)
	{
	  boolean check_ok = true;
	  const char * msg = (const char *) NULL;

	  switch (r)
	    {
	    case bfd_reloc_overflow:
	      check_ok = info->callbacks->reloc_overflow
		(info, name, howto->name, (bfd_vma) 0,
		 input_bfd, input_section, rel->r_offset);
	      break;

	    case bfd_reloc_undefined:
	      /* We may have sent this message above.  */
	      if (! undefined_signalled)
		check_ok = info->callbacks->undefined_symbol
		  (info, name, input_bfd, input_section, rel->r_offset,
		   true);
	      undefined_signalled = true;
	      break;

	    case bfd_reloc_outofrange:
	      msg = _("internal error: out of range error");
	      break;

	    case bfd_reloc_notsupported:
	      msg = _("internal error: unsupported relocation error");
	      break;

	    case bfd_reloc_dangerous:
	      msg = _("internal error: dangerous relocation");
	      break;

	    default:
	      msg = _("internal error: unknown error");
	      break;
	    }

	  if (msg)
	    check_ok = info->callbacks->warning
	      (info, msg, name, input_bfd, input_section, rel->r_offset);

	  if (! check_ok)
	    return false;
	}
    }

  return true;
}

/* Perform a single relocation.  By default we use the standard BFD
   routines.  A few relocs we have to do ourselves.  */

static bfd_reloc_status_type
mmix_final_link_relocate (howto, input_section, contents,
			  r_offset, r_addend, relocation, symname, symsec)
     reloc_howto_type *howto;
     asection *input_section;
     bfd_byte *contents;
     bfd_vma r_offset;
     bfd_signed_vma r_addend;
     bfd_vma relocation;
     const char *symname;
     asection *symsec;
{
  bfd_reloc_status_type r = bfd_reloc_ok;
  bfd_vma addr
    = (input_section->output_section->vma
       + input_section->output_offset
       + r_offset);
  bfd_signed_vma srel
    = (bfd_signed_vma) relocation + r_addend;

  switch (howto->type)
    {
      /* All these are PC-relative.  */
    case R_MMIX_PUSHJ:
    case R_MMIX_CBRANCH:
    case R_MMIX_ADDR19:
    case R_MMIX_GETA:
    case R_MMIX_ADDR27:
    case R_MMIX_JMP:
      contents += r_offset;

      srel -= (input_section->output_section->vma
	       + input_section->output_offset
	       + r_offset);

      r = mmix_elf_perform_relocation (input_section, howto, contents,
				       addr, srel);
      break;

    case R_MMIX_REG_OR_BYTE:
    case R_MMIX_REG:
      /* For now, we handle these alike.  They must refer to an register
	 symbol, which is either relative to the register section and in
	 the range 0..255, or is in the register contents section with vma
	 regno * 8.  */

      /* FIXME: A better way to check for reg contents section?
	 FIXME: Postpone section->scaling to mmix_elf_perform_relocation? */
      if (symsec == NULL)
	return bfd_reloc_undefined;

      if (strcmp (bfd_get_section_name (symsec->owner, symsec),
		  MMIX_REG_CONTENTS_SECTION_NAME) == 0)
	{
	  if ((srel & 7) != 0 || srel < 32*8 || srel > 255*8)
	    {
	      /* The bfd_reloc_outofrange return value, though intuitively
		 a better value, will not get us an error.  */
	      return bfd_reloc_overflow;
	    }
	  srel /= 8;
	}
      else if (strcmp (bfd_get_section_name (symsec->owner, symsec),
		       MMIX_REG_SECTION_NAME) == 0)
	{
	  if (srel < 0 || srel > 255)
	    /* The bfd_reloc_outofrange return value, though intuitively a
	       better value, will not get us an error.  */
	    return bfd_reloc_overflow;
	}
      else
	{
	  (*_bfd_error_handler)
	    (_("%s: register relocation against non-register symbol: %s in %s"),
	     bfd_get_filename (input_section->owner),
	     symname == NULL || *symname == 0 ? _("(unknown)") : symname,
	     bfd_get_section_name (symsec->owner, symsec));

	  /* The bfd_reloc_outofrange return value, though intuitively a
	     better value, will not get us an error.  */
	  return bfd_reloc_overflow;
	}
      contents += r_offset;
      r = mmix_elf_perform_relocation (input_section, howto, contents,
				       addr, srel);
      break;

    case R_MMIX_LOCAL:
      /* This isn't a real relocation, it's just an assertion that the
	 final relocation value corresponds to a local register.  We
	 ignore the actual relocation; nothing is changed.  */
      {
	asection *regsec
	  = bfd_get_section_by_name (input_section->output_section->owner,
				     MMIX_REG_CONTENTS_SECTION_NAME);
	bfd_vma first_global;

	/* Check that this is an absolute value, or a reference to the
	   register contents section or the register (symbol) section.
	   Absolute numbers can get here as undefined section.  Undefined
	   symbols are signalled elsewhere, so there's no conflict in us
	   accidentally handling it.  */
	if (!bfd_is_abs_section (symsec)
	    && !bfd_is_und_section (symsec)
	    && strcmp (bfd_get_section_name (symsec->owner, symsec),
		       MMIX_REG_CONTENTS_SECTION_NAME) != 0
	    && strcmp (bfd_get_section_name (symsec->owner, symsec),
		       MMIX_REG_SECTION_NAME) != 0)
	{
	  (*_bfd_error_handler)
	    (_("%s: directive LOCAL valid only with a register or absolute value"),
	     bfd_get_filename (input_section->owner));

	  return bfd_reloc_overflow;
	}

      /* If we don't have a register contents section, then $255 is the
	 first global register.  */
      if (regsec == NULL)
	first_global = 255;
      else
	{
	  first_global = bfd_get_section_vma (abfd, regsec) / 8;
	  if (strcmp (bfd_get_section_name (symsec->owner, symsec),
		      MMIX_REG_CONTENTS_SECTION_NAME) == 0)
	    {
	      if ((srel & 7) != 0 || srel < 32*8 || srel > 255*8)
		/* The bfd_reloc_outofrange return value, though
		   intuitively a better value, will not get us an error.  */
		return bfd_reloc_overflow;
	      srel /= 8;
	    }
	}

	if ((bfd_vma) srel >= first_global)
	  {
	    /* FIXME: Better error message.  */
	    (*_bfd_error_handler)
	      (_("%s: LOCAL directive: Register $%ld is not a local register.  First global register is $%ld."),
	       bfd_get_filename (input_section->owner), (long) srel, (long) first_global);

	    return bfd_reloc_overflow;
	  }
      }
      r = bfd_reloc_ok;
      break;

    default:
      r = _bfd_final_link_relocate (howto, input_section->owner, input_section,
				    contents, r_offset,
				    relocation, r_addend);
    }

  return r;
}

/* Return the section that should be marked against GC for a given
   relocation.  */

static asection *
mmix_elf_gc_mark_hook (abfd, info, rel, h, sym)
     bfd *abfd;
     struct bfd_link_info *info ATTRIBUTE_UNUSED;
     Elf_Internal_Rela *rel;
     struct elf_link_hash_entry *h;
     Elf_Internal_Sym *sym;
{
  if (h != NULL)
    {
      switch (ELF64_R_TYPE (rel->r_info))
	{
	case R_MMIX_GNU_VTINHERIT:
	case R_MMIX_GNU_VTENTRY:
	  break;

	default:
	  switch (h->root.type)
	    {
	    case bfd_link_hash_defined:
	    case bfd_link_hash_defweak:
	      return h->root.u.def.section;

	    case bfd_link_hash_common:
	      return h->root.u.c.p->section;

	    default:
	      break;
	    }
	}
    }
  else
    {
      return bfd_section_from_elf_index (abfd, sym->st_shndx);
    }

  return NULL;
}

/* Sort register relocs to come before expanding relocs.  */

static int
mmix_elf_sort_relocs (p1, p2)
     const PTR p1;
     const PTR p2;
{
  const Elf_Internal_Rela *r1 = (const Elf_Internal_Rela *) p1;
  const Elf_Internal_Rela *r2 = (const Elf_Internal_Rela *) p2;
  int r1_is_reg, r2_is_reg;

  /* Sort primarily on r_offset & ~3, so relocs are done to consecutive
     insns.  */
  if ((r1->r_offset & ~(bfd_vma) 3) > (r2->r_offset & ~(bfd_vma) 3))
    return 1;
  else if ((r1->r_offset & ~(bfd_vma) 3) < (r2->r_offset & ~(bfd_vma) 3))
    return -1;

  r1_is_reg
    = (ELF64_R_TYPE (r1->r_info) == R_MMIX_REG_OR_BYTE
       || ELF64_R_TYPE (r1->r_info) == R_MMIX_REG);
  r2_is_reg
    = (ELF64_R_TYPE (r2->r_info) == R_MMIX_REG_OR_BYTE
       || ELF64_R_TYPE (r2->r_info) == R_MMIX_REG);
  if (r1_is_reg != r2_is_reg)
    return r2_is_reg - r1_is_reg;

  /* Neither or both are register relocs.  Then sort on full offset.  */
  if (r1->r_offset > r2->r_offset)
    return 1;
  else if (r1->r_offset < r2->r_offset)
    return -1;
  return 0;
}

/* Look through the relocs for a section during the first phase.  */

static boolean
mmix_elf_check_relocs (abfd, info, sec, relocs)
     bfd *abfd;
     struct bfd_link_info *info;
     asection *sec;
     const Elf_Internal_Rela *relocs;
{
  Elf_Internal_Shdr *symtab_hdr;
  struct elf_link_hash_entry **sym_hashes, **sym_hashes_end;
  const Elf_Internal_Rela *rel;
  const Elf_Internal_Rela *rel_end;

  if (info->relocateable)
    return true;

  symtab_hdr = &elf_tdata (abfd)->symtab_hdr;
  sym_hashes = elf_sym_hashes (abfd);
  sym_hashes_end = sym_hashes + symtab_hdr->sh_size/sizeof(Elf64_External_Sym);
  if (!elf_bad_symtab (abfd))
    sym_hashes_end -= symtab_hdr->sh_info;

  /* First we sort the relocs so that any register relocs come before
     expansion-relocs to the same insn.  FIXME: Not done for mmo.  */
  qsort ((PTR) relocs, sec->reloc_count, sizeof (Elf_Internal_Rela),
	 mmix_elf_sort_relocs);

  rel_end = relocs + sec->reloc_count;
  for (rel = relocs; rel < rel_end; rel++)
    {
      struct elf_link_hash_entry *h;
      unsigned long r_symndx;

      r_symndx = ELF64_R_SYM (rel->r_info);
      if (r_symndx < symtab_hdr->sh_info)
        h = NULL;
      else
        h = sym_hashes[r_symndx - symtab_hdr->sh_info];

      switch (ELF64_R_TYPE (rel->r_info))
        {
        /* This relocation describes the C++ object vtable hierarchy.
           Reconstruct it for later use during GC.  */
        case R_MMIX_GNU_VTINHERIT:
          if (!_bfd_elf64_gc_record_vtinherit (abfd, sec, h, rel->r_offset))
            return false;
          break;

        /* This relocation describes which C++ vtable entries are actually
           used.  Record for later use during GC.  */
        case R_MMIX_GNU_VTENTRY:
          if (!_bfd_elf64_gc_record_vtentry (abfd, sec, h, rel->r_addend))
            return false;
          break;
        }
    }

  return true;
}

/* Change symbols relative to the reg contents section to instead be to
   the register section, and scale them down to correspond to the register
   number.  */

static boolean
mmix_elf_link_output_symbol_hook (abfd, info, name, sym, input_sec)
     bfd *abfd ATTRIBUTE_UNUSED;
     struct bfd_link_info *info ATTRIBUTE_UNUSED;
     const char *name ATTRIBUTE_UNUSED;
     Elf_Internal_Sym *sym;
     asection *input_sec;
{
  if (input_sec != NULL
      && input_sec->name != NULL
      && ELF_ST_TYPE (sym->st_info) != STT_SECTION
      && strcmp (input_sec->name, MMIX_REG_CONTENTS_SECTION_NAME) == 0)
    {
      sym->st_value /= 8;
      sym->st_shndx = SHN_REGISTER;
    }

  return true;
}

/* We fake a register section that holds values that are register numbers.
   Having a SHN_REGISTER and register section translates better to other
   formats (e.g. mmo) than for example a STT_REGISTER attribute.
   This section faking is based on a construct in elf32-mips.c.  */
static asection mmix_elf_reg_section;
static asymbol mmix_elf_reg_section_symbol;
static asymbol *mmix_elf_reg_section_symbol_ptr;

/* Handle the special MIPS section numbers that a symbol may use.
   This is used for both the 32-bit and the 64-bit ABI.  */

void
mmix_elf_symbol_processing (abfd, asym)
     bfd *abfd ATTRIBUTE_UNUSED;
     asymbol *asym;
{
  elf_symbol_type *elfsym;

  elfsym = (elf_symbol_type *) asym;
  switch (elfsym->internal_elf_sym.st_shndx)
    {
    case SHN_REGISTER:
      if (mmix_elf_reg_section.name == NULL)
	{
	  /* Initialize the register section.  */
	  mmix_elf_reg_section.name = MMIX_REG_SECTION_NAME;
	  mmix_elf_reg_section.flags = SEC_NO_FLAGS;
	  mmix_elf_reg_section.output_section = &mmix_elf_reg_section;
	  mmix_elf_reg_section.symbol = &mmix_elf_reg_section_symbol;
	  mmix_elf_reg_section.symbol_ptr_ptr = &mmix_elf_reg_section_symbol_ptr;
	  mmix_elf_reg_section_symbol.name = MMIX_REG_SECTION_NAME;
	  mmix_elf_reg_section_symbol.flags = BSF_SECTION_SYM;
	  mmix_elf_reg_section_symbol.section = &mmix_elf_reg_section;
	  mmix_elf_reg_section_symbol_ptr = &mmix_elf_reg_section_symbol;
	}
      asym->section = &mmix_elf_reg_section;
      break;

    default:
      break;
    }
}

/* Given a BFD section, try to locate the corresponding ELF section
   index.  */

static boolean
mmix_elf_section_from_bfd_section (abfd, sec, retval)
     bfd *                 abfd ATTRIBUTE_UNUSED;
     asection *            sec;
     int *                 retval;
{
  if (strcmp (bfd_get_section_name (abfd, sec), MMIX_REG_SECTION_NAME) == 0)
    *retval = SHN_REGISTER;
  else
    return false;

  return true;
}

/* Hook called by the linker routine which adds symbols from an object
   file.  We must handle the special SHN_REGISTER section number here.

   We also check that we only have *one* each of the section-start
   symbols, since otherwise having two with the same value would cause
   them to be "merged", but with the contents serialized.  */

boolean
mmix_elf_add_symbol_hook (abfd, info, sym, namep, flagsp, secp, valp)
     bfd *abfd;
     struct bfd_link_info *info ATTRIBUTE_UNUSED;
     const Elf_Internal_Sym *sym;
     const char **namep ATTRIBUTE_UNUSED;
     flagword *flagsp ATTRIBUTE_UNUSED;
     asection **secp;
     bfd_vma *valp ATTRIBUTE_UNUSED;
{
  if (sym->st_shndx == SHN_REGISTER)
    *secp = bfd_make_section_old_way (abfd, MMIX_REG_SECTION_NAME);
  else if ((*namep)[0] == '_' && (*namep)[1] == '_' && (*namep)[2] == '.'
	   && strncmp (*namep, MMIX_LOC_SECTION_START_SYMBOL_PREFIX,
		       strlen (MMIX_LOC_SECTION_START_SYMBOL_PREFIX)) == 0)
    {
      /* See if we have another one.  */
      struct elf_link_hash_entry *h
	= (struct elf_link_hash_entry *) bfd_link_hash_lookup (info->hash,
							       *namep,
							       false,
							       false, false);

      if (h != NULL && h->root.type != bfd_link_hash_undefined)
	{
	  /* How do we get the asymbol (or really: the filename) from h?
	     h->root.u.def.section->owner is NULL.  */
	  ((*_bfd_error_handler)
	   (_("%s: Error: multiple definition of `%s'; start of %s is set in a earlier linked file\n"),
	    bfd_get_filename (abfd), *namep,
	    *namep + strlen (MMIX_LOC_SECTION_START_SYMBOL_PREFIX)));
	   bfd_set_error (bfd_error_bad_value);
	   return false;
	}
    }

  return true;
}

/* We consider symbols matching "L.*:[0-9]+" to be local symbols.  */

boolean
mmix_elf_is_local_label_name (abfd, name)
     bfd *abfd;
     const char *name;
{
  const char *colpos;
  int digits;

  /* Also include the default local-label definition.  */
  if (_bfd_elf_is_local_label_name (abfd, name))
    return true;

  if (*name != 'L')
    return false;

  /* If there's no ":", or more than one, it's not a local symbol.  */
  colpos = strchr (name, ':');
  if (colpos == NULL || strchr (colpos + 1, ':') != NULL)
    return false;

  /* Check that there are remaining characters and that they are digits.  */
  if (colpos[1] == 0)
    return false;

  digits = strspn (colpos + 1, "0123456789");
  return digits != 0 && colpos[1 + digits] == 0;
}

/* We get rid of the register section here.  */

boolean
mmix_elf_final_link (abfd, info)
     bfd *abfd;
     struct bfd_link_info *info;
{
  /* We never output a register section, though we create one for
     temporary measures.  Check that nobody entered contents into it.  */
  asection *reg_section;
  asection **secpp;

  reg_section = bfd_get_section_by_name (abfd, MMIX_REG_SECTION_NAME);

  if (reg_section != NULL)
    {
      /* FIXME: Pass error state gracefully.  */
      if (bfd_get_section_flags (abfd, reg_section) & SEC_HAS_CONTENTS)
	_bfd_abort (__FILE__, __LINE__, _("Register section has contents\n"));

      /* Really remove the section.  */
      for (secpp = &abfd->sections;
	   *secpp != reg_section;
	   secpp = &(*secpp)->next)
	;
      bfd_section_list_remove (abfd, secpp);
      --abfd->section_count;
    }

  if (! bfd_elf64_bfd_final_link (abfd, info))
    return false;

  return true;
}

#define ELF_ARCH		bfd_arch_mmix
#define ELF_MACHINE_CODE 	EM_MMIX

/* According to mmix-doc page 36 (paragraph 45), this should be (1LL << 48LL).
   However, that's too much for something somewhere in the linker part of
   BFD; perhaps the start-address has to be a non-zero multiple of this
   number, or larger than this number.  The symptom is that the linker
   complains: "warning: allocated section `.text' not in segment".  We
   settle for 64k; the page-size used in examples is 8k.
   #define ELF_MAXPAGESIZE 0x10000

   Unfortunately, this causes excessive padding in the supposedly small
   for-education programs that are the expected usage (where people would
   inspect output).  We stick to 256 bytes just to have *some* default
   alignment.  */
#define ELF_MAXPAGESIZE 0x100

#define TARGET_BIG_SYM		bfd_elf64_mmix_vec
#define TARGET_BIG_NAME		"elf64-mmix"

#define elf_info_to_howto_rel		NULL
#define elf_info_to_howto		mmix_info_to_howto_rela
#define elf_backend_relocate_section	mmix_elf_relocate_section
#define elf_backend_gc_mark_hook	mmix_elf_gc_mark_hook
#define elf_backend_link_output_symbol_hook \
	mmix_elf_link_output_symbol_hook
#define elf_backend_add_symbol_hook	mmix_elf_add_symbol_hook

#define elf_backend_check_relocs	mmix_elf_check_relocs
#define elf_backend_symbol_processing	mmix_elf_symbol_processing

#define bfd_elf64_bfd_is_local_label_name \
	mmix_elf_is_local_label_name

#define elf_backend_may_use_rel_p	0
#define elf_backend_may_use_rela_p	1
#define elf_backend_default_use_rela_p	1

#define elf_backend_can_gc_sections	1
#define elf_backend_section_from_bfd_section \
	mmix_elf_section_from_bfd_section

#define bfd_elf64_bfd_final_link	mmix_elf_final_link

#include "elf64-target.h"
