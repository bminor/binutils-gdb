/* PowerPC64-specific support for 64-bit ELF.
   Copyright 1999, 2000, 2001 Free Software Foundation, Inc.
   Written by Linus Nordberg, Swox AB <info@swox.com>,
   based on elf32-ppc.c by Ian Lance Taylor.

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

/* This file is based on the 64-bit PowerPC ELF ABI.  It is also based
   on the file elf32-ppc.c.  */

#include "bfd.h"
#include "sysdep.h"
#include "bfdlink.h"
#include "libbfd.h"
#include "elf-bfd.h"
#include "elf/ppc.h"

#define USE_RELA		/* we want RELA relocations, not REL.  */


static void ppc_howto_init
  PARAMS ((void));
static reloc_howto_type *ppc64_elf_reloc_type_lookup
  PARAMS ((bfd *abfd, bfd_reloc_code_real_type code));
static void ppc64_elf_info_to_howto
  PARAMS ((bfd *abfd, arelent *cache_ptr, Elf64_Internal_Rela *dst));
static bfd_reloc_status_type ppc64_elf_addr16_ha_reloc
  PARAMS ((bfd *, arelent *, asymbol *, PTR, asection *, bfd *, char **));
static boolean ppc64_elf_set_private_flags
  PARAMS ((bfd *, flagword));
static boolean ppc64_elf_copy_private_bfd_data
  PARAMS ((bfd *, bfd *));
static boolean ppc64_elf_merge_private_bfd_data
  PARAMS ((bfd *, bfd *));
static boolean ppc64_elf_section_from_shdr
  PARAMS ((bfd *, Elf64_Internal_Shdr *, char *));
static boolean ppc64_elf_create_dynamic_sections
  PARAMS ((bfd *, struct bfd_link_info *));
static boolean ppc64_elf_check_relocs
  PARAMS ((bfd *, struct bfd_link_info *, asection *,
	   const Elf_Internal_Rela *));
static asection * ppc64_elf_gc_mark_hook
  PARAMS ((bfd *abfd, struct bfd_link_info *info, Elf_Internal_Rela *rel,
	   struct elf_link_hash_entry *h, Elf_Internal_Sym *sym));
static boolean ppc64_elf_gc_sweep_hook
  PARAMS ((bfd *abfd, struct bfd_link_info *info, asection *sec,
	   const Elf_Internal_Rela *relocs));
static boolean ppc64_elf_adjust_dynamic_symbol
  PARAMS ((struct bfd_link_info *, struct elf_link_hash_entry *));
static boolean ppc_adjust_dynindx
  PARAMS ((struct elf_link_hash_entry *, PTR));
static boolean ppc64_elf_size_dynamic_sections
  PARAMS ((bfd *, struct bfd_link_info *));
static boolean ppc64_elf_final_link
  PARAMS ((bfd *, struct bfd_link_info *));
static boolean ppc64_elf_fake_sections
  PARAMS ((bfd *, Elf64_Internal_Shdr *, asection *));
static boolean ppc64_elf_relocate_section
  PARAMS ((bfd *, struct bfd_link_info *info, bfd *, asection *, bfd_byte *,
	   Elf_Internal_Rela *relocs, Elf_Internal_Sym *local_syms,
	   asection **));
static boolean ppc64_elf_finish_dynamic_symbol
  PARAMS ((bfd *, struct bfd_link_info *, struct elf_link_hash_entry *,
	   Elf_Internal_Sym *));
static boolean ppc64_elf_finish_dynamic_sections
  PARAMS ((bfd *, struct bfd_link_info *));


/* Branch prediction bit for branch taken relocs.  */
#define BRANCH_PREDICT_BIT 0x200000

/* Mask to set RA in memory instructions.  */
#define RA_REGISTER_MASK 0x001f0000

/* Value to shift register by to insert RA.  */
#define RA_REGISTER_SHIFT 16

/* The name of the dynamic interpreter.  This is put in the .interp
   section.  */
#define ELF_DYNAMIC_INTERPRETER "/usr/lib/ld.so.1"

/* The size in bytes of an entry in the procedure linkage table.  */
#define PLT_ENTRY_SIZE 24

/* The initial size of the plt reserved for the dynamic linker.  */
#define PLT_INITIAL_ENTRY_SIZE 96

/* TOC base pointers offset from start of TOC.  */
#define TOC_BASE_OFF (0x8000)

/* Global linkage stub.  The first instruction has its offset patched.  */
/* FIXME: Need traceback table?  */
static const unsigned int ppc64_elf_glink_code[] = {
  0xe9820000,			/* ld    %r12,0(%r2) */
  0xf8410028,			/* std   %r2,40(%r1) */
  0xe80c0000,			/* ld    %r0,0(%r12) */
  0xe84c0008,			/* ld    %r2,8(%r12) */
  0x7c0903a6,			/* mtctr %r0         */
  0x4e800420			/* bctr              */
};

#define PPC64_ELF_GLINK_SIZE \
  ((sizeof ppc64_elf_glink_code / sizeof ppc64_elf_glink_code[0]) * 4)

/* Relocation HOWTO's.  */
static reloc_howto_type *ppc64_elf_howto_table[(int) R_PPC_max];

static reloc_howto_type ppc64_elf_howto_raw[] = {
  /* This reloc does nothing.  */
  HOWTO (R_PPC64_NONE,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_NONE",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 false),		/* pcrel_offset */

  /* A standard 32 bit relocation.  */
  HOWTO (R_PPC64_ADDR32,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_ADDR32",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffffffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* An absolute 26 bit branch; the lower two bits must be zero.
     FIXME: we don't check that, we just clear them.  */
  HOWTO (R_PPC64_ADDR24,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 26,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_ADDR24",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0x3fffffc,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* A standard 16 bit relocation.  */
  HOWTO (R_PPC64_ADDR16,	/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_ADDR16",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* A 16 bit relocation without overflow.  */
  HOWTO (R_PPC64_ADDR16_LO,	/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_ADDR16_LO",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* Bits 16-31 of an address.  */
  HOWTO (R_PPC64_ADDR16_HI,	/* type */
	 16,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_ADDR16_HI",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* Bits 16-31 of an address, plus 1 if the contents of the low 16
     bits, treated as a signed number, is negative.  */
  HOWTO (R_PPC64_ADDR16_HA,	/* type */
	 16,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 ppc64_elf_addr16_ha_reloc, /* special_function */
	 "R_PPC64_ADDR16_HA",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* An absolute 16 bit branch; the lower two bits must be zero.
     FIXME: we don't check that, we just clear them.  */
  HOWTO (R_PPC64_ADDR14,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_ADDR14",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xfffc,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* An absolute 16 bit branch, for which bit 10 should be set to
     indicate that the branch is expected to be taken.  The lower two
     bits must be zero.  */
  HOWTO (R_PPC64_ADDR14_BRTAKEN, /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_ADDR14_BRTAKEN",/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xfffc,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* An absolute 16 bit branch, for which bit 10 should be set to
     indicate that the branch is not expected to be taken.  The lower
     two bits must be zero.  */
  HOWTO (R_PPC64_ADDR14_BRNTAKEN, /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_ADDR14_BRNTAKEN",/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xfffc,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* A relative 26 bit branch; the lower two bits must be zero.  */
  HOWTO (R_PPC64_REL24,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 26,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_REL24",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0x3fffffc,		/* dst_mask */
	 true),			/* pcrel_offset */

  /* A relative 16 bit branch; the lower two bits must be zero.  */
  HOWTO (R_PPC64_REL14,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_REL14",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xfffc,		/* dst_mask */
	 true),			/* pcrel_offset */

  /* A relative 16 bit branch.  Bit 10 should be set to indicate that
     the branch is expected to be taken.  The lower two bits must be
     zero.  */
  HOWTO (R_PPC64_REL14_BRTAKEN,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_REL14_BRTAKEN", /* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xfffc,		/* dst_mask */
	 true),			/* pcrel_offset */

  /* A relative 16 bit branch.  Bit 10 should be set to indicate that
     the branch is not expected to be taken.  The lower two bits must
     be zero.  */
  HOWTO (R_PPC64_REL14_BRNTAKEN, /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_REL14_BRNTAKEN",/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xfffc,		/* dst_mask */
	 true),			/* pcrel_offset */

  /* Like R_PPC64_ADDR16, but referring to the GOT table entry for the
     symbol.  */
  HOWTO (R_PPC64_GOT16,		/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_GOT16",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* Like R_PPC64_ADDR16_LO, but referring to the GOT table entry for
     the symbol.  */
  HOWTO (R_PPC64_GOT16_LO,	/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_GOT16_LO",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* Like R_PPC64_ADDR16_HI, but referring to the GOT table entry for
     the symbol.  */
  HOWTO (R_PPC64_GOT16_HI,	/* type */
	 16,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_GOT16_HI",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* Like R_PPC64_ADDR16_HA, but referring to the GOT table entry for
     the symbol.  */
  HOWTO (R_PPC64_GOT16_HA,	/* type */
	 16,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 ppc64_elf_addr16_ha_reloc, /* special_function */
	 "R_PPC64_GOT16_HA",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* This is used only by the dynamic linker.  The symbol should exist
     both in the object being run and in some shared library.  The
     dynamic linker copies the data addressed by the symbol from the
     shared library into the object, because the object being
     run has to have the data at some particular address.  */
  HOWTO (R_PPC64_COPY,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	 /* special_function */
	 "R_PPC64_COPY",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 false),		/* pcrel_offset */

  /* Like R_PPC64_ADDR64, but used when setting global offset table
     entries.  */
  HOWTO (R_PPC64_GLOB_DAT,	/* type */
	 0,			/* rightshift */
	 4,			/* size (0=byte, 1=short, 2=long, 4=64 bits) */
	 64,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	 /* special_function */
	 "R_PPC64_GLOB_DAT",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffffffffffffffff,	/* dst_mask */
	 false),		/* pcrel_offset */

  /* Created by the link editor.  Marks a procedure linkage table
     entry for a symbol.  */
  HOWTO (R_PPC64_JMP_SLOT,	/* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 0,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_JMP_SLOT",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 false),		/* pcrel_offset */

  /* Used only by the dynamic linker.  When the object is run, this
     doubleword64 is set to the load address of the object, plus the
     addend.  */
  HOWTO (R_PPC64_RELATIVE,	/* type */
	 0,			/* rightshift */
	 4,			/* size (0=byte, 1=short, 2=long, 4=64 bits) */
	 64,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_RELATIVE",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffffffffffffffff,	/* dst_mask */
	 false),		/* pcrel_offset */

  /* Like R_PPC64_ADDR32, but may be unaligned.  */
  HOWTO (R_PPC64_UADDR32,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_UADDR32",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffffffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* Like R_PPC64_ADDR16, but may be unaligned.  */
  HOWTO (R_PPC64_UADDR16,	/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_UADDR16",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* 32-bit PC relative.  */
  HOWTO (R_PPC64_REL32,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 /* FIXME: Verify.  Was complain_overflow_bitfield. */
	 complain_overflow_signed, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_REL32",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffffffff,		/* dst_mask */
	 true),			/* pcrel_offset */

  /* 32-bit relocation to the symbol's procedure linkage table.
     FIXME: R_PPC64_PLT32 not supported.  */
  HOWTO (R_PPC64_PLT32,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_PLT32",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 false),		/* pcrel_offset */

  /* 32-bit PC relative relocation to the symbol's procedure linkage table.
     FIXME: R_PPC64_PLTREL32 not supported.  */
  HOWTO (R_PPC64_PLTREL32,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_PLTREL32",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 true),			/* pcrel_offset */

  /* Like R_PPC64_ADDR16_LO, but referring to the PLT table entry for
     the symbol.  */
  HOWTO (R_PPC64_PLT16_LO,	/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_PLT16_LO",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* Like R_PPC64_ADDR16_HI, but referring to the PLT table entry for
     the symbol.  */
  HOWTO (R_PPC64_PLT16_HI,	/* type */
	 16,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_PLT16_HI",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* Like R_PPC64_ADDR16_HA, but referring to the PLT table entry for
     the symbol.  */
  HOWTO (R_PPC64_PLT16_HA,	/* type */
	 16,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 ppc64_elf_addr16_ha_reloc, /* special_function */
	 "R_PPC64_PLT16_HA",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* 32-bit section relative relocation.  */
  /* FIXME: Verify R_PPC64_SECTOFF.  Seems strange with size=2 and
     dst_mask=0.  */
  HOWTO (R_PPC64_SECTOFF,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_SECTOFF",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 true),			/* pcrel_offset */

  /* 16-bit lower half section relative relocation.  */
  HOWTO (R_PPC64_SECTOFF_LO,	/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_SECTOFF_LO",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* 16-bit upper half section relative relocation.  */
  HOWTO (R_PPC64_SECTOFF_HI,	/* type */
	 16,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_SECTOFF_HI",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* 16-bit upper half adjusted section relative relocation.  */
  HOWTO (R_PPC64_SECTOFF_HA,	/* type */
	 16,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 ppc64_elf_addr16_ha_reloc, /* special_function */
	 "R_PPC64_SECTOFF_HA",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* Like R_PPC64_REL24 without touching the two least significant
     bits.  */
  /* FIXME: Verify R_PPC64_ADDR30.  */
  HOWTO (R_PPC64_ADDR30,	/* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 30,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc, /* special_function */
	 "R_PPC64_ADDR30",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xfffffffc,		/* dst_mask */
	 true),			/* pcrel_offset */

  /* Relocs in the 64-bit PowerPC ELF ABI, not in the 32-bit ABI.  */

  /* A standard 64-bit relocation.  */
  HOWTO (R_PPC64_ADDR64,	/* type */
	 0,			/* rightshift */
	 4,			/* size (0=byte, 1=short, 2=long, 4=64 bits) */
	 64,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_ADDR64",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffffffffffffffff,	/* dst_mask */
	 false),		/* pcrel_offset */

  /* The bits 32-47 of an address.  */
  HOWTO (R_PPC64_ADDR16_HIGHER,	/* type */
	 32,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_ADDR16_HIGHER", /* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* The bits 32-47 of an address, plus 1 if the contents of the low
     16 bits, treated as a signed number, is negative.  */
  HOWTO (R_PPC64_ADDR16_HIGHERA, /* type */
	 32,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 ppc64_elf_addr16_ha_reloc, /* special_function */
	 "R_PPC64_ADDR16_HIGHERA", /* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* The bits 48-63 of an address.  */
  HOWTO (R_PPC64_ADDR16_HIGHEST,/* type */
	 48,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_ADDR16_HIGHEST", /* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* The bits 48-63 of an address, plus 1 if the contents of the low
     16 bits, treated as a signed number, is negative.  */
  HOWTO (R_PPC64_ADDR16_HIGHESTA,/* type */
	 48,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 ppc64_elf_addr16_ha_reloc, /* special_function */
	 "R_PPC64_ADDR16_HIGHESTA", /* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* Like ADDR64, but may be unaligned.  */
  HOWTO (R_PPC64_UADDR64,	/* type */
	 0,			/* rightshift */
	 4,			/* size (0=byte, 1=short, 2=long, 4=64 bits) */
	 64,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_UADDR64",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffffffffffffffff,	/* dst_mask */
	 false),		/* pcrel_offset */

  /* 64-bit relative relocation.  */
  HOWTO (R_PPC64_REL64,		/* type */
	 0,			/* rightshift */
	 4,			/* size (0=byte, 1=short, 2=long, 4=64 bits) */
	 64,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_REL64",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffffffffffffffff,	/* dst_mask */
	 true),			/* pcrel_offset */

  /* 64-bit relocation to the symbol's procedure linkage table. */
  /* FIXME: R_PPC64_PLT64 not supported.  */
  HOWTO (R_PPC64_PLT64,		/* type */
	 0,			/* rightshift */
	 4,			/* size (0=byte, 1=short, 2=long, 4=64 bits) */
	 64,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_PLT64",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 false),		/* pcrel_offset */

  /* 64-bit PC relative relocation to the symbol's procedure linkage
     table.  */
  /* FIXME: R_PPC64_PLTREL64 not supported.  */
  HOWTO (R_PPC64_PLTREL64,	/* type */
	 0,			/* rightshift */
	 4,			/* size (0=byte, 1=short, 2=long, 4=64 bits) */
	 64,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_PLTREL64",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 true),			/* pcrel_offset */

  /* 16 bit TOC-relative relocation.  */

  /* R_PPC64_TOC16	  47	   half16*	S + A - .TOC.  */
  HOWTO (R_PPC64_TOC16,		/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_TOC16",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* 16 bit TOC-relative relocation without overflow.  */

  /* R_PPC64_TOC16_LO	  48	   half16	 #lo (S + A - .TOC.)  */
  HOWTO (R_PPC64_TOC16_LO,	/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_TOC16_LO",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* 16 bit TOC-relative relocation, high 16 bits.  */

  /* R_PPC64_TOC16_HI	  49	   half16	 #hi (S + A - .TOC.)  */
  HOWTO (R_PPC64_TOC16_HI,	/* type */
	 16,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_TOC16_HI",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* 16 bit TOC-relative relocation, high 16 bits, plus 1 if the
     contents of the low 16 bits, treated as a signed number, is
     negative.  */

  /* R_PPC64_TOC16_HA	  50	   half16	 #ha (S + A - .TOC.)  */
  HOWTO (R_PPC64_TOC16_HA,	/* type */
	 16,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 ppc64_elf_addr16_ha_reloc, /* special_function */
	 "R_PPC64_TOC16_HA",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* 64-bit relocation; insert value of TOC base (.TOC.).  */

  /* R_PPC64_TOC		  51	   doubleword64	 .TOC.  */
  HOWTO (R_PPC64_TOC,		/* type */
	 0,			/* rightshift */
	 4,			/* size (0=byte, 1=short, 2=long, 4=64 bits) */
	 64,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_TOC",		/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffffffffffffffff,	/* dst_mask */
	 false),		/* pcrel_offset */

  /* Like R_PPC64_GOT16, but also informs the link editor that the
     value to relocate may (!) refer to a PLT entry which the link
     editor (a) may replace with the symbol value.  If the link editor
     is unable to fully resolve the symbol, it may (b) create a PLT
     entry and store the address to the new PLT entry in the GOT.
     This permits lazy resolution of function symbols at run time.
     The link editor may also skip all of this and just (c) emit a
     R_PPC64_GLOB_DAT to tie the symbol to the GOT entry.  */
  /* FIXME: R_PPC64_PLTGOT16 not implemented.  */
    HOWTO (R_PPC64_PLTGOT16,	/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_PLTGOT16",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* Like R_PPC64_PLTGOT16, but without overflow.  */
  /* FIXME: R_PPC64_PLTGOT16_LO not implemented.  */
  HOWTO (R_PPC64_PLTGOT16_LO,	/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_PLTGOT16_LO",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* Like R_PPC64_PLT_GOT16, but using bits 16-31 of the address.  */
  /* FIXME: R_PPC64_PLTGOT16_HI not implemented.  */
  HOWTO (R_PPC64_PLTGOT16_HI,	/* type */
	 16,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_PLTGOT16_HI",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* Like R_PPC64_PLT_GOT16, but using bits 16-31 of the address, plus
     1 if the contents of the low 16 bits, treated as a signed number,
     is negative.  */
  /* FIXME: R_PPC64_PLTGOT16_HA not implemented.  */
  HOWTO (R_PPC64_PLTGOT16_HA,	/* type */
	 16,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 ppc64_elf_addr16_ha_reloc, /* special_function */
	 "R_PPC64_PLTGOT16_HA",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* Like R_PPC64_ADDR16, but for instructions with a DS field.  */
  HOWTO (R_PPC64_ADDR16_DS,	/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_ADDR16_DS",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xfffc,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* Like R_PPC64_ADDR16_LO, but for instructions with a DS field.  */
  HOWTO (R_PPC64_ADDR16_LO_DS,	/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_ADDR16_LO_DS",/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xfffc,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* Like R_PPC64_GOT16, but for instructions with a DS field.  */
  HOWTO (R_PPC64_GOT16_DS,	/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_GOT16_DS",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xfffc,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* Like R_PPC64_GOT16_LO, but for instructions with a DS field.  */
  HOWTO (R_PPC64_GOT16_LO_DS,	/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_GOT16_LO_DS",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xfffc,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* Like R_PPC64_PLT16_LO, but for instructions with a DS field.  */
  HOWTO (R_PPC64_PLT16_LO_DS,	/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_PLT16_LO_DS",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xfffc,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* Like R_PPC64_SECTOFF, but for instructions with a DS field.  */
  /* FIXME: Verify R_PPC64_SECTOFF.  Seems strange with size=2 and
     dst_mask=0.  */
  HOWTO (R_PPC64_SECTOFF_DS,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_SECTOFF_DS",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 true),			/* pcrel_offset */

  /* Like R_PPC64_SECTOFF_LO, but for instructions with a DS field.  */
  HOWTO (R_PPC64_SECTOFF_LO_DS, /* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_SECTOFF_LO_DS",/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xfffc,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* Like R_PPC64_TOC16, but for instructions with a DS field.  */
  HOWTO (R_PPC64_TOC16_DS,	/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_TOC16_DS",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xfffc,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* Like R_PPC64_TOC16_LO, but for instructions with a DS field.  */
  HOWTO (R_PPC64_TOC16_LO_DS,	/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_TOC16_LO_DS",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xfffc,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* Like R_PPC64_PLTGOT16, but for instructions with a DS field.  */
  /* FIXME: R_PPC64_PLTGOT16_DS not implemented.  */
    HOWTO (R_PPC64_PLTGOT16_DS,	/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_PLTGOT16_DS",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xfffc,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* Like R_PPC64_PLTGOT16_LO, but for instructions with a DS field.  */
  /* FIXME: R_PPC64_PLTGOT16_LO not implemented.  */
  HOWTO (R_PPC64_PLTGOT16_LO_DS,/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PPC64_PLTGOT16_LO_DS",/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xfffc,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* GNU extension to record C++ vtable hierarchy.  */
  HOWTO (R_PPC64_GNU_VTINHERIT,	/* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 0,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 NULL,			/* special_function */
	 "R_PPC64_GNU_VTINHERIT", /* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 false),		/* pcrel_offset */

  /* GNU extension to record C++ vtable member usage.  */
  HOWTO (R_PPC64_GNU_VTENTRY,	/* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 0,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 NULL,			/* special_function */
	 "R_PPC64_GNU_VTENTRY",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 false),		/* pcrel_offset */
};


/* Initialize the ppc64_elf_howto_table, so that linear accesses can
   be done.  */

static void
ppc_howto_init ()
{
  unsigned int i, type;

  for (i = 0;
       i < sizeof (ppc64_elf_howto_raw) / sizeof (ppc64_elf_howto_raw[0]);
       i++)
    {
      type = ppc64_elf_howto_raw[i].type;
      BFD_ASSERT (type < (sizeof (ppc64_elf_howto_table)
			  / sizeof (ppc64_elf_howto_table[0])));
      ppc64_elf_howto_table[type] = &ppc64_elf_howto_raw[i];
    }
}

static reloc_howto_type *
ppc64_elf_reloc_type_lookup (abfd, code)
     bfd *abfd ATTRIBUTE_UNUSED;
     bfd_reloc_code_real_type code;
{
  enum elf_ppc_reloc_type ppc_reloc = R_PPC_NONE;

  if (!ppc64_elf_howto_table[R_PPC64_ADDR32])
    /* Initialize howto table if needed.  */
    ppc_howto_init ();

  switch ((int) code)
    {
    default:
      return (reloc_howto_type *) NULL;

    case BFD_RELOC_NONE:		 ppc_reloc = R_PPC64_NONE;
      break;
    case BFD_RELOC_32:			 ppc_reloc = R_PPC64_ADDR32;
      break;
    case BFD_RELOC_PPC_BA26:		 ppc_reloc = R_PPC64_ADDR24;
      break;
    case BFD_RELOC_16:			 ppc_reloc = R_PPC64_ADDR16;
      break;
    case BFD_RELOC_LO16:		 ppc_reloc = R_PPC64_ADDR16_LO;
      break;
    case BFD_RELOC_HI16:		 ppc_reloc = R_PPC64_ADDR16_HI;
      break;
    case BFD_RELOC_HI16_S:		 ppc_reloc = R_PPC64_ADDR16_HA;
      break;
    case BFD_RELOC_PPC_BA16:		 ppc_reloc = R_PPC64_ADDR14;
      break;
    case BFD_RELOC_PPC_BA16_BRTAKEN:	 ppc_reloc = R_PPC64_ADDR14_BRTAKEN;
      break;
    case BFD_RELOC_PPC_BA16_BRNTAKEN:	 ppc_reloc = R_PPC64_ADDR14_BRNTAKEN;
      break;
    case BFD_RELOC_PPC_B26:		 ppc_reloc = R_PPC64_REL24;
      break;
    case BFD_RELOC_PPC_B16:		 ppc_reloc = R_PPC64_REL14;
      break;
    case BFD_RELOC_PPC_B16_BRTAKEN:	 ppc_reloc = R_PPC64_REL14_BRTAKEN;
      break;
    case BFD_RELOC_PPC_B16_BRNTAKEN:	 ppc_reloc = R_PPC64_REL14_BRNTAKEN;
      break;
    case BFD_RELOC_16_GOTOFF:		 ppc_reloc = R_PPC64_GOT16;
      break;
    case BFD_RELOC_LO16_GOTOFF:		 ppc_reloc = R_PPC64_GOT16_LO;
      break;
    case BFD_RELOC_HI16_GOTOFF:		 ppc_reloc = R_PPC64_GOT16_HI;
      break;
    case BFD_RELOC_HI16_S_GOTOFF:	 ppc_reloc = R_PPC64_GOT16_HA;
      break;
    case BFD_RELOC_PPC_COPY:		 ppc_reloc = R_PPC64_COPY;
      break;
    case BFD_RELOC_PPC_GLOB_DAT:	 ppc_reloc = R_PPC64_GLOB_DAT;
      break;
    case BFD_RELOC_32_PCREL:		 ppc_reloc = R_PPC64_REL32;
      break;
    case BFD_RELOC_32_PLTOFF:		 ppc_reloc = R_PPC64_PLT32;
      break;
    case BFD_RELOC_32_PLT_PCREL:	 ppc_reloc = R_PPC64_PLTREL32;
      break;
    case BFD_RELOC_LO16_PLTOFF:		 ppc_reloc = R_PPC64_PLT16_LO;
      break;
    case BFD_RELOC_HI16_PLTOFF:		 ppc_reloc = R_PPC64_PLT16_HI;
      break;
    case BFD_RELOC_HI16_S_PLTOFF:	 ppc_reloc = R_PPC64_PLT16_HA;
      break;
    case BFD_RELOC_32_BASEREL:		 ppc_reloc = R_PPC64_SECTOFF;
      break;
    case BFD_RELOC_LO16_BASEREL:	 ppc_reloc = R_PPC64_SECTOFF_LO;
      break;
    case BFD_RELOC_HI16_BASEREL:	 ppc_reloc = R_PPC64_SECTOFF_HI;
      break;
    case BFD_RELOC_HI16_S_BASEREL:	 ppc_reloc = R_PPC64_SECTOFF_HA;
      break;
      /* FIXME: Is CTOR 32 or 64 bits?  Fix md_apply_fix3 in
	 gas/config/tc-ppc.c too.  */
    case BFD_RELOC_CTOR:		 ppc_reloc = R_PPC64_ADDR32;
      break;
    case BFD_RELOC_64:			 ppc_reloc = R_PPC64_ADDR64;
      break;
    case BFD_RELOC_PPC64_HIGHER:	 ppc_reloc = R_PPC64_ADDR16_HIGHER;
      break;
    case BFD_RELOC_PPC64_HIGHER_S:	 ppc_reloc = R_PPC64_ADDR16_HIGHERA;
      break;
    case BFD_RELOC_PPC64_HIGHEST:	 ppc_reloc = R_PPC64_ADDR16_HIGHEST;
      break;
    case BFD_RELOC_PPC64_HIGHEST_S:	 ppc_reloc = R_PPC64_ADDR16_HIGHESTA;
      break;
    case BFD_RELOC_64_PCREL:		 ppc_reloc = R_PPC64_REL64;
      break;
    case BFD_RELOC_64_PLTOFF:		 ppc_reloc = R_PPC64_PLT64;
      break;
    case BFD_RELOC_64_PLT_PCREL:	 ppc_reloc = R_PPC64_PLTREL64;
      break;
    case BFD_RELOC_PPC_TOC16:		 ppc_reloc = R_PPC64_TOC16;
      break;
    case BFD_RELOC_PPC64_TOC16_LO:	 ppc_reloc = R_PPC64_TOC16_LO;
      break;
    case BFD_RELOC_PPC64_TOC16_HI:	 ppc_reloc = R_PPC64_TOC16_HI;
      break;
    case BFD_RELOC_PPC64_TOC16_HA:	 ppc_reloc = R_PPC64_TOC16_HA;
      break;
    case BFD_RELOC_PPC64_TOC:		 ppc_reloc = R_PPC64_TOC;
      break;
    case BFD_RELOC_PPC64_PLTGOT16:	 ppc_reloc = R_PPC64_PLTGOT16;
      break;
    case BFD_RELOC_PPC64_PLTGOT16_LO:	 ppc_reloc = R_PPC64_PLTGOT16_LO;
      break;
    case BFD_RELOC_PPC64_PLTGOT16_HI:	 ppc_reloc = R_PPC64_PLTGOT16_HI;
      break;
    case BFD_RELOC_PPC64_PLTGOT16_HA:	 ppc_reloc = R_PPC64_PLTGOT16_HA;
      break;
    case BFD_RELOC_PPC64_ADDR16_DS:      ppc_reloc = R_PPC64_ADDR16_DS;
      break;
    case BFD_RELOC_PPC64_ADDR16_LO_DS:   ppc_reloc = R_PPC64_ADDR16_LO_DS;
      break;
    case BFD_RELOC_PPC64_GOT16_DS:       ppc_reloc = R_PPC64_GOT16_DS;
      break;
    case BFD_RELOC_PPC64_GOT16_LO_DS:    ppc_reloc = R_PPC64_GOT16_LO_DS;
      break;
    case BFD_RELOC_PPC64_PLT16_LO_DS:    ppc_reloc = R_PPC64_PLT16_LO_DS;
      break;
    case BFD_RELOC_PPC64_SECTOFF_DS:     ppc_reloc = R_PPC64_SECTOFF_DS;
      break;
    case BFD_RELOC_PPC64_SECTOFF_LO_DS:  ppc_reloc = R_PPC64_SECTOFF_LO_DS;
      break;
    case BFD_RELOC_PPC64_TOC16_DS:       ppc_reloc = R_PPC64_TOC16_DS;
      break;
    case BFD_RELOC_PPC64_TOC16_LO_DS:    ppc_reloc = R_PPC64_TOC16_LO_DS;
      break;
    case BFD_RELOC_PPC64_PLTGOT16_DS:    ppc_reloc = R_PPC64_PLTGOT16_DS;
      break;
    case BFD_RELOC_PPC64_PLTGOT16_LO_DS: ppc_reloc = R_PPC64_PLTGOT16_LO_DS;
      break;
    case BFD_RELOC_VTABLE_INHERIT:	 ppc_reloc = R_PPC64_GNU_VTINHERIT;
      break;
    case BFD_RELOC_VTABLE_ENTRY:	 ppc_reloc = R_PPC64_GNU_VTENTRY;
      break;
    }

  return ppc64_elf_howto_table[(int) ppc_reloc];
};

/* Set the howto pointer for a PowerPC ELF reloc.  */

static void
ppc64_elf_info_to_howto (abfd, cache_ptr, dst)
     bfd *abfd ATTRIBUTE_UNUSED;
     arelent *cache_ptr;
     Elf64_Internal_Rela *dst;
{
  if (!ppc64_elf_howto_table[R_PPC64_ADDR32])
    /* Initialize howto table if needed.  */
    ppc_howto_init ();

  BFD_ASSERT (ELF64_R_TYPE (dst->r_info) < (unsigned int) R_PPC_max);
  cache_ptr->howto = ppc64_elf_howto_table[ELF64_R_TYPE (dst->r_info)];
}

/* Handle the R_PPC_ADDR16_HA reloc.  */

static bfd_reloc_status_type
ppc64_elf_addr16_ha_reloc (abfd, reloc_entry, symbol, data, input_section,
			   output_bfd, error_message)
     bfd *abfd ATTRIBUTE_UNUSED;
     arelent *reloc_entry;
     asymbol *symbol;
     PTR data ATTRIBUTE_UNUSED;
     asection *input_section;
     bfd *output_bfd;
     char **error_message ATTRIBUTE_UNUSED;
{
  bfd_vma relocation;

  if (output_bfd != NULL)
    {
      reloc_entry->address += input_section->output_offset;
      return bfd_reloc_ok;
    }

  if (reloc_entry->address > input_section->_cooked_size)
    return bfd_reloc_outofrange;

  if (bfd_is_com_section (symbol->section))
    relocation = 0;
  else
    relocation = symbol->value;

  relocation += symbol->section->output_section->vma;
  relocation += symbol->section->output_offset;
  relocation += reloc_entry->addend;

  reloc_entry->addend += (relocation & 0x8000) << 1;

  return bfd_reloc_continue;
}

/* Function to set whether a module needs the -mrelocatable bit set.  */

static boolean
ppc64_elf_set_private_flags (abfd, flags)
     bfd *abfd;
     flagword flags;
{
  BFD_ASSERT (!elf_flags_init (abfd)
	      || elf_elfheader (abfd)->e_flags == flags);

  elf_elfheader (abfd)->e_flags = flags;
  elf_flags_init (abfd) = true;
  return true;
}

/* Copy backend specific data from one object module to another.  */
static boolean
ppc64_elf_copy_private_bfd_data (ibfd, obfd)
     bfd *ibfd;
     bfd *obfd;
{
  if (bfd_get_flavour (ibfd) != bfd_target_elf_flavour
      || bfd_get_flavour (obfd) != bfd_target_elf_flavour)
    return true;

  BFD_ASSERT (!elf_flags_init (obfd)
	      || elf_elfheader (obfd)->e_flags == elf_elfheader (ibfd)->e_flags);

  elf_elfheader (obfd)->e_flags = elf_elfheader (ibfd)->e_flags;
  elf_flags_init (obfd) = true;
  return true;
}

/* Merge backend specific data from an object file to the output
   object file when linking.  */
static boolean
ppc64_elf_merge_private_bfd_data (ibfd, obfd)
     bfd *ibfd;
     bfd *obfd;
{
  flagword old_flags;
  flagword new_flags;
  boolean error;

  /* Check if we have the same endianess.  */
  if (ibfd->xvec->byteorder != obfd->xvec->byteorder
      && obfd->xvec->byteorder != BFD_ENDIAN_UNKNOWN)
    {
      const char *msg;

      if (bfd_big_endian (ibfd))
	msg = _("%s: compiled for a big endian system and target is little endian");
      else
	msg = _("%s: compiled for a little endian system and target is big endian");

      (*_bfd_error_handler) (msg, bfd_get_filename (ibfd));

      bfd_set_error (bfd_error_wrong_format);
      return false;
    }

  if (bfd_get_flavour (ibfd) != bfd_target_elf_flavour
      || bfd_get_flavour (obfd) != bfd_target_elf_flavour)
    return true;

  new_flags = elf_elfheader (ibfd)->e_flags;
  old_flags = elf_elfheader (obfd)->e_flags;
  if (!elf_flags_init (obfd))
    {
      /* First call, no flags set.  */
      elf_flags_init (obfd) = true;
      elf_elfheader (obfd)->e_flags = new_flags;
    }

  else if (new_flags == old_flags)
    /* Compatible flags are ok.  */
    ;

  else
    {
      /* Incompatible flags.  Warn about -mrelocatable mismatch.
	 Allow -mrelocatable-lib to be linked with either.  */
      error = false;
      if ((new_flags & EF_PPC_RELOCATABLE) != 0
	  && (old_flags & (EF_PPC_RELOCATABLE | EF_PPC_RELOCATABLE_LIB)) == 0)
	{
	  error = true;
	  (*_bfd_error_handler)
	    (_("%s: compiled with -mrelocatable and linked with modules compiled normally"),
	     bfd_get_filename (ibfd));
	}
      else if ((new_flags & (EF_PPC_RELOCATABLE | EF_PPC_RELOCATABLE_LIB)) == 0
	       && (old_flags & EF_PPC_RELOCATABLE) != 0)
	{
	  error = true;
	  (*_bfd_error_handler)
	    (_("%s: compiled normally and linked with modules compiled with -mrelocatable"),
	     bfd_get_filename (ibfd));
	}

      /* The output is -mrelocatable-lib iff both the input files are.  */
      if (! (new_flags & EF_PPC_RELOCATABLE_LIB))
	elf_elfheader (obfd)->e_flags &= ~EF_PPC_RELOCATABLE_LIB;

      /* The output is -mrelocatable iff it can't be -mrelocatable-lib,
         but each input file is either -mrelocatable or -mrelocatable-lib.  */
      if (! (elf_elfheader (obfd)->e_flags & EF_PPC_RELOCATABLE_LIB)
	  && (new_flags & (EF_PPC_RELOCATABLE_LIB | EF_PPC_RELOCATABLE))
	  && (old_flags & (EF_PPC_RELOCATABLE_LIB | EF_PPC_RELOCATABLE)))
	elf_elfheader (obfd)->e_flags |= EF_PPC_RELOCATABLE;

      /* Do not warn about eabi vs. V.4 mismatch, just or in the bit
	 if any module uses it.  */
      elf_elfheader (obfd)->e_flags |= (new_flags & EF_PPC_EMB);

      new_flags &= ~(EF_PPC_RELOCATABLE | EF_PPC_RELOCATABLE_LIB | EF_PPC_EMB);
      old_flags &= ~(EF_PPC_RELOCATABLE | EF_PPC_RELOCATABLE_LIB | EF_PPC_EMB);

      /* Warn about any other mismatches.  */
      if (new_flags != old_flags)
	{
	  error = true;
	  (*_bfd_error_handler)
	    (_("%s: uses different e_flags (0x%lx) fields than previous modules (0x%lx)"),
	     bfd_get_filename (ibfd), (long) new_flags, (long) old_flags);
	}

      if (error)
	{
	  bfd_set_error (bfd_error_bad_value);
	  return false;
	}
    }

  return true;
}

/* Handle a PowerPC specific section when reading an object file.  This
   is called when elfcode.h finds a section with an unknown type.  */

static boolean
ppc64_elf_section_from_shdr (abfd, hdr, name)
     bfd *abfd;
     Elf64_Internal_Shdr *hdr;
     char *name;
{
  asection *newsect;
  flagword flags;

  if (! _bfd_elf_make_section_from_shdr (abfd, hdr, name))
    return false;

  newsect = hdr->bfd_section;
  flags = bfd_get_section_flags (abfd, newsect);
  if (hdr->sh_flags & SHF_EXCLUDE)
    flags |= SEC_EXCLUDE;

  if (hdr->sh_type == SHT_ORDERED)
    flags |= SEC_SORT_ENTRIES;

  bfd_set_section_flags (abfd, newsect, flags);
  return true;
}

/* The following routines are specific to the ELF linker, while
   routines above are used generally.  They appear in this file more
   or less in the order in which they are called from external
   routines.  eg. ppc64_elf_check_relocs is called early in the link
   process, ppc64_elf_finish_dynamic_sections is one of the last
   functions called.  */

/* Create the .glink section as well as the ordinary dynamic
   sections.  */

static boolean
ppc64_elf_create_dynamic_sections (abfd, info)
     bfd *abfd;
     struct bfd_link_info *info;
{
  asection *s;
  flagword flags;

  if (!_bfd_elf_create_dynamic_sections (abfd, info))
    return false;

  /* Our .plt just contains pointers, no code.  */
  s = bfd_get_section_by_name (abfd, ".plt");
  if (s == NULL)
    return false;
  flags = bfd_get_section_flags (abfd, s);
  flags &= ~SEC_CODE;
  if (! bfd_set_section_flags (abfd, s, flags))
    return false;

  /* Create .glink for global linkage functions.  */
  flags = (SEC_ALLOC | SEC_LOAD | SEC_HAS_CONTENTS | SEC_IN_MEMORY
	   | SEC_LINKER_CREATED);
  s = bfd_make_section (abfd, ".glink");
  if (s == NULL
      || ! bfd_set_section_flags (abfd, s, flags)
      || ! bfd_set_section_alignment (abfd, s, 3))
    return false;

  return true;
}

/* Look through the relocs for a section during the first phase, and
   allocate space in the global offset table or procedure linkage
   table.  */

static boolean
ppc64_elf_check_relocs (abfd, info, sec, relocs)
     bfd *abfd;
     struct bfd_link_info *info;
     asection *sec;
     const Elf_Internal_Rela *relocs;
{
  bfd *dynobj;
  Elf_Internal_Shdr *symtab_hdr;
  struct elf_link_hash_entry **sym_hashes, **sym_hashes_end;
  const Elf_Internal_Rela *rel;
  const Elf_Internal_Rela *rel_end;
  bfd_signed_vma *local_got_refcounts;
  asection *sreloc;
  asection *sgot = NULL;
  asection *srelgot = NULL;

  if (info->relocateable)
    return true;

#ifdef DEBUG
  fprintf (stderr, "ppc64_elf_check_relocs called for section %s in %s\n",
	   bfd_get_section_name (abfd, sec),
	   bfd_get_filename (abfd));
#endif

  dynobj = elf_hash_table (info)->dynobj;
  symtab_hdr = &elf_tdata (abfd)->symtab_hdr;
  local_got_refcounts = elf_local_got_refcounts (abfd);

  sym_hashes = elf_sym_hashes (abfd);
  sym_hashes_end = (sym_hashes
		    + symtab_hdr->sh_size / sizeof (Elf64_External_Sym));
  if (!elf_bad_symtab (abfd))
    sym_hashes_end -= symtab_hdr->sh_info;

  sreloc = NULL;

  rel_end = relocs + sec->reloc_count;
  for (rel = relocs; rel < rel_end; rel++)
    {
      unsigned long r_symndx;
      struct elf_link_hash_entry *h;

      r_symndx = ELF64_R_SYM (rel->r_info);
      if (r_symndx < symtab_hdr->sh_info)
	h = NULL;
      else
	h = sym_hashes[r_symndx - symtab_hdr->sh_info];

      switch (ELF64_R_TYPE (rel->r_info))
	{
	  /* GOT16 relocations */
	case R_PPC64_GOT16:
	case R_PPC64_GOT16_LO:
	case R_PPC64_GOT16_HI:
	case R_PPC64_GOT16_HA:
	case R_PPC64_GOT16_DS:
	case R_PPC64_GOT16_LO_DS:
	  /* This symbol requires a global offset table entry.  */

	  if (sgot == NULL)
	    {
	      if (dynobj == NULL)
		elf_hash_table (info)->dynobj = dynobj = abfd;
	      if (! _bfd_elf_create_got_section (dynobj, info))
		return false;
	      sgot = bfd_get_section_by_name (dynobj, ".got");
	      BFD_ASSERT (sgot != NULL);
	    }

	  if (srelgot == NULL
	      && (h != NULL || info->shared))
	    {
	      srelgot = bfd_get_section_by_name (dynobj, ".rela.got");
	      if (srelgot == NULL)
		{
		  srelgot = bfd_make_section (dynobj, ".rela.got");
		  if (srelgot == NULL
		      || ! bfd_set_section_flags (dynobj, srelgot,
						  (SEC_ALLOC
						   | SEC_LOAD
						   | SEC_HAS_CONTENTS
						   | SEC_IN_MEMORY
						   | SEC_LINKER_CREATED
						   | SEC_READONLY))
		      || ! bfd_set_section_alignment (dynobj, srelgot, 2))
		    return false;
		}
	    }

	  if (h != NULL)
	    {
	      if (h->got.refcount == -1)
		{
		  /* Make sure this symbol is output as a dynamic symbol.  */
		  if (h->dynindx == -1)
		    if (!bfd_elf64_link_record_dynamic_symbol (info, h))
		      return false;

		  /* Allocate space in the .got.  */
		  sgot->_raw_size += 8;
		  /* Allocate relocation space.  */
		  srelgot->_raw_size += sizeof (Elf64_External_Rela);

		  h->got.refcount = 1;
		}
	      else
		h->got.refcount++;
	    }
	  else
	    {
	      /* This is a global offset table entry for a local symbol.  */
	      if (local_got_refcounts == NULL)
		{
		  bfd_size_type size;

		  size = symtab_hdr->sh_info;
		  size *= sizeof (bfd_signed_vma);
		  local_got_refcounts = ((bfd_signed_vma *)
					 bfd_alloc (abfd, size));
		  if (local_got_refcounts == NULL)
		    return false;
		  elf_local_got_refcounts (abfd) = local_got_refcounts;
		  memset (local_got_refcounts, -1, (size_t) size);
		}
	      if (local_got_refcounts[r_symndx] == -1)
		{
		  sgot->_raw_size += 8;

		  /* If we are generating a shared object, we need to
                     output a R_PPC64_RELATIVE reloc so that the
                     dynamic linker can adjust this GOT entry.  */
		  if (info->shared)
		    srelgot->_raw_size += sizeof (Elf64_External_Rela);

		  local_got_refcounts[r_symndx] = 1;
		}
	      else
		local_got_refcounts[r_symndx]++;
	    }
	  break;

	case R_PPC64_PLT64:
	case R_PPC64_PLT32:
	case R_PPC64_PLT16_LO:
	case R_PPC64_PLT16_HI:
	case R_PPC64_PLT16_HA:
	case R_PPC64_PLTGOT16_DS:
	case R_PPC64_PLTGOT16_LO_DS:
#ifdef DEBUG
	  fprintf (stderr, "Reloc requires a PLT entry\n");
#endif
	  /* This symbol requires a procedure linkage table entry.  We
             actually build the entry in adjust_dynamic_symbol,
             because this might be a case of linking PIC code without
             linking in any dynamic objects, in which case we don't
             need to generate a procedure linkage table after all.  */

	  if (h == NULL)
	    {
	      /* It does not make sense to have a procedure linkage
                 table entry for a local symbol.  */
	      bfd_set_error (bfd_error_bad_value);
	      return false;
	    }

	  /* Make sure this symbol is output as a dynamic symbol.  */
	  if (h->dynindx == -1)
	    {
	      if (! bfd_elf64_link_record_dynamic_symbol (info, h))
		return false;
	    }
	  if (h->plt.refcount == -1)
	    {
	      h->elf_link_hash_flags |= ELF_LINK_HASH_NEEDS_PLT;
	      h->plt.refcount = 1;
	    }
	  else
	    h->plt.refcount++;
	  break;

	  /* The following relocations don't need to propagate the
	     relocation if linking a shared object since they are
	     section relative.  */
	case R_PPC64_SECTOFF:
	case R_PPC64_SECTOFF_LO:
	case R_PPC64_SECTOFF_HI:
	case R_PPC64_SECTOFF_HA:
	case R_PPC64_SECTOFF_DS:
	case R_PPC64_SECTOFF_LO_DS:
	case R_PPC64_TOC16:
	case R_PPC64_TOC16_LO:
	case R_PPC64_TOC16_HI:
	case R_PPC64_TOC16_HA:
	case R_PPC64_TOC16_DS:
	case R_PPC64_TOC16_LO_DS:
	  break;

	  /* This relocation describes the C++ object vtable hierarchy.
	     Reconstruct it for later use during GC.  */
	case R_PPC64_GNU_VTINHERIT:
	  if (!_bfd_elf64_gc_record_vtinherit (abfd, sec, h, rel->r_offset))
	    return false;
	  break;

	  /* This relocation describes which C++ vtable entries are actually
	     used.  Record for later use during GC.  */
	case R_PPC64_GNU_VTENTRY:
	  if (!_bfd_elf64_gc_record_vtentry (abfd, sec, h, rel->r_addend))
	    return false;
	  break;

	  /* When creating a shared object, we must copy these
	     relocs into the output file.  We create a reloc
	     section in dynobj and make room for the reloc.  */
	case R_PPC64_REL64:
	case R_PPC64_REL32:
	case R_PPC64_REL24:
	case R_PPC64_REL14:
	case R_PPC64_REL14_BRTAKEN:
	case R_PPC64_REL14_BRNTAKEN:
	  if (h == NULL)
	    break;
	  /* fall through */

	case R_PPC64_TOC:
	  /* fall through */

	default:
	  if (info->shared)
	    {
#ifdef DEBUG
	      fprintf (stderr, "ppc64_elf_check_relocs need to create relocation for %s\n",
		       (h && h->root.root.string
			? h->root.root.string
			: "<unknown>"));
#endif
	      if (sreloc == NULL)
		{
		  const char *name;

		  name = (bfd_elf_string_from_elf_section
			  (abfd,
			   elf_elfheader (abfd)->e_shstrndx,
			   elf_section_data (sec)->rel_hdr.sh_name));
		  if (name == NULL)
		    return false;

		  BFD_ASSERT (strncmp (name, ".rela", 5) == 0
			      && strcmp (bfd_get_section_name (abfd, sec),
					 name + 5) == 0);

		  sreloc = bfd_get_section_by_name (dynobj, name);
		  if (sreloc == NULL)
		    {
		      flagword flags;

		      sreloc = bfd_make_section (dynobj, name);
		      flags = (SEC_HAS_CONTENTS | SEC_READONLY
			       | SEC_IN_MEMORY | SEC_LINKER_CREATED);
		      if ((sec->flags & SEC_ALLOC) != 0)
			flags |= SEC_ALLOC | SEC_LOAD;
		      if (sreloc == NULL
			  || ! bfd_set_section_flags (dynobj, sreloc, flags)
			  || ! bfd_set_section_alignment (dynobj, sreloc, 2))
			return false;
		    }
		}

	      sreloc->_raw_size += sizeof (Elf64_External_Rela);

	      /* FIXME: We should here do what the m68k and i386
		 backends do: if the reloc is pc-relative, record it
		 in case it turns out that the reloc is unnecessary
		 because the symbol is forced local by versioning or
		 we are linking with -Bdynamic.  Fortunately this
		 case is not frequent.  */
	    }

	  break;
	}
    }

  return true;
}

/* Return the section that should be marked against GC for a given
   relocation.  */

static asection *
ppc64_elf_gc_mark_hook (abfd, info, rel, h, sym)
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
	case R_PPC64_GNU_VTINHERIT:
	case R_PPC64_GNU_VTENTRY:
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
      if (! (elf_bad_symtab (abfd)
	     && ELF_ST_BIND (sym->st_info) != STB_LOCAL)
	  && ! ((sym->st_shndx <= 0 || sym->st_shndx >= SHN_LORESERVE)
		&& sym->st_shndx != SHN_COMMON))
	{
	  return bfd_section_from_elf_index (abfd, sym->st_shndx);
	}
    }

  return NULL;
}

/* Update the got entry reference counts for the section being removed.  */

static boolean
ppc64_elf_gc_sweep_hook (abfd, info, sec, relocs)
     bfd *abfd;
     struct bfd_link_info *info ATTRIBUTE_UNUSED;
     asection *sec;
     const Elf_Internal_Rela *relocs;
{
  Elf_Internal_Shdr *symtab_hdr;
  struct elf_link_hash_entry **sym_hashes;
  bfd_signed_vma *local_got_refcounts;
  const Elf_Internal_Rela *rel, *relend;
  unsigned long r_symndx;
  struct elf_link_hash_entry *h;

  symtab_hdr = &elf_tdata (abfd)->symtab_hdr;
  sym_hashes = elf_sym_hashes (abfd);
  local_got_refcounts = elf_local_got_refcounts (abfd);

  relend = relocs + sec->reloc_count;
  for (rel = relocs; rel < relend; rel++)
    switch (ELF64_R_TYPE (rel->r_info))
      {
      case R_PPC64_GOT16:
      case R_PPC64_GOT16_LO:
      case R_PPC64_GOT16_HI:
      case R_PPC64_GOT16_HA:
	r_symndx = ELF64_R_SYM (rel->r_info);
	if (r_symndx >= symtab_hdr->sh_info)
	  {
	    h = sym_hashes[r_symndx - symtab_hdr->sh_info];
	    if (h->got.refcount > 0)
	      h->got.refcount--;
	  }
	else
	  {
	    if (local_got_refcounts[r_symndx] > 0)
	      local_got_refcounts[r_symndx]--;
	  }
        break;

      case R_PPC64_PLT32:
      case R_PPC64_PLT16_LO:
      case R_PPC64_PLT16_HI:
      case R_PPC64_PLT16_HA:
	r_symndx = ELF64_R_SYM (rel->r_info);
	if (r_symndx >= symtab_hdr->sh_info)
	  {
	    h = sym_hashes[r_symndx - symtab_hdr->sh_info];
	    if (h->plt.refcount > 0)
	      h->plt.refcount--;
	  }
	break;

      default:
	break;
      }

  return true;
}

/* Adjust a symbol defined by a dynamic object and referenced by a
   regular object.  The current definition is in some section of the
   dynamic object, but we're not including those sections.  We have to
   change the definition to something the rest of the link can
   understand.  */

static boolean
ppc64_elf_adjust_dynamic_symbol (info, h)
     struct bfd_link_info *info;
     struct elf_link_hash_entry *h;
{
  bfd *dynobj = elf_hash_table (info)->dynobj;
  asection *s;
  unsigned int power_of_two;
  bfd_vma plt_offset;

#ifdef DEBUG
  fprintf (stderr, "ppc64_elf_adjust_dynamic_symbol called for %s\n",
	   h->root.root.string);
#endif

  /* Make sure we know what is going on here.  */
  BFD_ASSERT (dynobj != NULL
	      && ((h->elf_link_hash_flags & ELF_LINK_HASH_NEEDS_PLT)
		  || h->weakdef != NULL
		  || ((h->elf_link_hash_flags
		       & ELF_LINK_HASH_DEF_DYNAMIC) != 0
		      && (h->elf_link_hash_flags
			  & ELF_LINK_HASH_REF_REGULAR) != 0
		      && (h->elf_link_hash_flags
			  & ELF_LINK_HASH_DEF_REGULAR) == 0)));

  /* If this is a function, alter the symbol to refer to global
     linkage code, which will be generated by ppc64_elf_relocate_section.
     Also, allocate a slot in the PLT.  */
  if (h->type == STT_FUNC
      || (h->elf_link_hash_flags & ELF_LINK_HASH_NEEDS_PLT) != 0)
    {
      asection *splt, *sglink;

      if (! elf_hash_table (info)->dynamic_sections_created
	  || ((!info->shared || info->symbolic || h->dynindx == -1)
	      && (h->elf_link_hash_flags
		  & ELF_LINK_HASH_DEF_REGULAR) != 0))
	{
	  /* A PLT entry is not required/allowed when:

	     1. We are not using ld.so; because then the PLT entry
	     can't be set up, so we can't use one.

	     2. We know for certain that a symbol is defined in
	     this object, because this object is the application,
	     is linked with -Bsymbolic, or because the symbol is local.

	     We used to end up here for GC if
	     (info->shared && h->plt.refcount <= 0)
	     We can't use plt.refcount here since
	     ppc64_elf_check_relocs can't detect all needs for
	     PLT.  FIXME: Rewrite GC.  */

	  h->plt.offset = (bfd_vma) -1;
	  h->elf_link_hash_flags &= ~ELF_LINK_HASH_NEEDS_PLT;
	  return true;
	}

      splt = bfd_get_section_by_name (dynobj, ".plt");
      sglink = bfd_get_section_by_name (dynobj, ".glink");
      BFD_ASSERT (splt != NULL && sglink != NULL);

      /* If it's a function entry point (the name starts with a dot)
         (FIXME: Could it ever be anything else?), find its
         corresponding function descriptor symbol and make sure it's
         output as a dynamic symbol.  Create it as undefined if
         necessary.  ppc64_elf_finish_dynamic_symbol will look it up
         again and create a JMP_SLOT reloc for it.  FIXME: The second
         lookup could be avoided by adding a new flag to
         elf_link_hash_entry->elf_link_hash_flags.  */

      if (h->root.root.string[0] == '.')
	{
	  struct elf_link_hash_entry *fdh;

	  /* FIXME: Follow bfd_link_hash_indirect?  */
	  fdh = elf_link_hash_lookup (elf_hash_table (info),
				      h->root.root.string + 1,
				      false, false, false);

	  if (fdh == (struct elf_link_hash_entry *) NULL)
	    {
	      asymbol *newsym;

	      /* Create it as undefined.  */
	      newsym = bfd_make_empty_symbol (dynobj);
	      BFD_ASSERT (newsym != NULL);
	      /* FIXME: Need own copy of name? */
	      newsym->name = h->root.root.string + 1;
	      newsym->section = bfd_und_section_ptr;
	      newsym->value = 0;
	      newsym->flags = BSF_DYNAMIC | BSF_OBJECT;

	      if ( ! (_bfd_generic_link_add_one_symbol
		      (info, dynobj, newsym->name, newsym->flags,
		       newsym->section, newsym->value,
		       NULL, /* string */
		       true, /* copy */
		       false, /*collect*/
		       (struct bfd_link_hash_entry **) &fdh)))
		{
		  /* FIXME: Print error message.  */
		  return false;
		}
	      fdh->root.type = bfd_link_hash_undefined;
#ifdef DEBUG
	      fprintf (stderr, "\tcreated %s", newsym->name);
#endif
	    }
#ifdef DEBUG
	  else
	    fprintf (stderr, "\tfound %s", fdh->root.root.string);
	  fprintf (stderr, "\n");
#endif

	  BFD_ASSERT (fdh != NULL);

	  /* Make sure it's output as a dynamic symbol.  */
	  if (fdh->dynindx == -1)
	    {
	      if (! bfd_elf64_link_record_dynamic_symbol (info, fdh))
		return false;
	    }
	  BFD_ASSERT (fdh->dynindx != -1);
	}

      /* If this is the first .plt entry, make room for the special
	 first entry.  */
      if (splt->_raw_size == 0)
	splt->_raw_size = PLT_INITIAL_ENTRY_SIZE;

      plt_offset = splt->_raw_size;

      /* FIXME: What about function pointer comparisons in 64-bit PPC?
	 We can't use the same trick as 32-bit PPC, since we set the
	 symbol to the linkage function. This will make function entry
	 point comparison work as expected within one object, but not
	 across object boundaries.  From the ABI:

 In this ABI, the address of a function is actually the address of a
 function descriptor.  A reference to a function, other than a function
 call, will normally load the address of the function descriptor from
 the global offset table.  The dynamic linker will ensure that for a
 given function, the same address is used for all references to the
 function from any global offset table.  Thus, function address
 comparisons will work as expected.

 When making a call to the function, the code may refer to the
 procedure linkage table entry, in order to permit lazy symbol
 resolution at run time.  In order to support correct function address
 comparisons, the compiler should be careful to only generate
 references to the procedure linkage table entry for function calls.
 For any other use of a function, the compiler should use the real
 address.

	 I don't see how this could ever work when passing a pointer to
	 a function across an object boundary.  The compiler has no
	 way of knowing how to find the function descriptor for a
	 function whose entrypoint is taken as an argument.

	 Or should the compiler arrange so that the function descriptor
	 address is passed and make the callee dig out the entry point
	 from that?  */
#if 0
      /* If this symbol is not defined in a regular file, and we are
	 not generating a shared library, then set the symbol to this
	 location in the .plt.  This is required to make function
	 pointers compare as equal between the normal executable and
	 the shared library.  */
      if (! info->shared
	  && (h->elf_link_hash_flags & ELF_LINK_HASH_DEF_REGULAR) == 0)
	{
	  h->root.u.def.section = splt;
	  h->root.u.def.value = plt_offset;
	}
#endif

      /* FIXME: How should a linkage symbol be defined?  It shouldn't
         be global, since it can't be used by any other object than
         this one.  Set ELF_LINK_FORCED_LOCAL?  */
      h->root.type = bfd_link_hash_defined;
      h->root.u.def.section = sglink;
      h->root.u.def.value = sglink->_raw_size;
      h->elf_link_hash_flags |= ELF_LINK_HASH_NEEDS_PLT;
      h->plt.offset = plt_offset;

      /* Make room for global linkage code in .glink.  */
      sglink->_raw_size += PPC64_ELF_GLINK_SIZE;

      /* Make room for this entry in PLT.  */
      splt->_raw_size += PLT_ENTRY_SIZE;

      /* We also need to make an entry in the .rela.plt section.  */
      s = bfd_get_section_by_name (dynobj, ".rela.plt");
      BFD_ASSERT (s != NULL);
      s->_raw_size += sizeof (Elf64_External_Rela);

      return true;
    }

  /* If this is a weak symbol, and there is a real definition, the
     processor independent code will have arranged for us to see the
     real definition first, and we can just use the same value.  */
  if (h->weakdef != NULL)
    {
      BFD_ASSERT (h->weakdef->root.type == bfd_link_hash_defined
		  || h->weakdef->root.type == bfd_link_hash_defweak);
      h->root.u.def.section = h->weakdef->root.u.def.section;
      h->root.u.def.value = h->weakdef->root.u.def.value;
      return true;
    }

  /* This is a reference to a symbol defined by a dynamic object which
     is not a function.  */

  /* If we are creating a shared library, we must presume that the
     only references to the symbol are via the global offset table.
     For such cases we need not do anything here; the relocations will
     be handled correctly by relocate_section.  */
  if (info->shared)
    return true;

  /* We must allocate the symbol in our .dynbss section, which will
     become part of the .bss section of the executable.  There will be
     an entry for this symbol in the .dynsym section.  The dynamic
     object will contain position independent code, so all references
     from the dynamic object to this symbol will go through the global
     offset table.  The dynamic linker will use the .dynsym entry to
     determine the address it must put in the global offset table, so
     both the dynamic object and the regular object will refer to the
     same memory location for the variable.  */
  s = bfd_get_section_by_name (dynobj, ".dynbss");
  BFD_ASSERT (s != NULL);

  /* We must generate a R_PPC_COPY reloc to tell the dynamic linker to
     copy the initial value out of the dynamic object and into the
     runtime process image.  We need to remember the offset into the
     .rela.bss section we are going to use.  */
  if ((h->root.u.def.section->flags & SEC_ALLOC) != 0)
    {
      asection *srel;

      srel = bfd_get_section_by_name (dynobj, ".rela.bss");
      BFD_ASSERT (srel != NULL);
      srel->_raw_size += sizeof (Elf64_External_Rela);
      h->elf_link_hash_flags |= ELF_LINK_HASH_NEEDS_COPY;
    }

  /* We need to figure out the alignment required for this symbol.  I
     have no idea how ELF linkers handle this.  */
  power_of_two = bfd_log2 (h->size);
  if (power_of_two > 4)
    power_of_two = 4;

  /* Apply the required alignment.  */
  s->_raw_size = BFD_ALIGN (s->_raw_size,
			    (bfd_size_type) (1 << power_of_two));
  if (power_of_two > bfd_get_section_alignment (dynobj, s))
    {
      if (! bfd_set_section_alignment (dynobj, s, power_of_two))
	return false;
    }

  /* Define the symbol as being at this point in the section.  */
  h->root.u.def.section = s;
  h->root.u.def.value = s->_raw_size;

  /* Increment the section size to make room for the symbol.  */
  s->_raw_size += h->size;

  return true;
}

/* Increment the index of a dynamic symbol by a given amount.  Called
   via elf_link_hash_traverse.  */

static boolean
ppc_adjust_dynindx (h, cparg)
     struct elf_link_hash_entry *h;
     PTR cparg;
{
  int *cp = (int *) cparg;

#ifdef DEBUG
  fprintf (stderr,
	   "ppc_adjust_dynindx: h->dynindx = %ld, *cp = %d (%s)\n",
	   h->dynindx, *cp,
	   h->root.root.string ? h->root.root.string : "<unknown>");
#endif

  if (h->dynindx != -1)
    h->dynindx += *cp;

  return true;
}

/* Set the sizes of the dynamic sections.  */

static boolean
ppc64_elf_size_dynamic_sections (output_bfd, info)
     bfd *output_bfd;
     struct bfd_link_info *info;
{
  bfd *dynobj;
  asection *s;
  boolean plt;
  boolean relocs;
  boolean reltext;

#ifdef DEBUG
  fprintf (stderr, "ppc64_elf_size_dynamic_sections called\n");
#endif

  dynobj = elf_hash_table (info)->dynobj;
  BFD_ASSERT (dynobj != NULL);

  if (elf_hash_table (info)->dynamic_sections_created)
    {
      /* Set the contents of the .interp section to the interpreter.  */
      if (! info->shared)
	{
	  s = bfd_get_section_by_name (dynobj, ".interp");
	  BFD_ASSERT (s != NULL);
	  s->_raw_size = sizeof ELF_DYNAMIC_INTERPRETER;
	  s->contents = (unsigned char *) ELF_DYNAMIC_INTERPRETER;
	}
    }
  else
    {
      /* We may have created entries in the .rela.got section.
	 However, if we are not creating the dynamic sections, we will
	 not actually use these entries.  Reset the size of .rela.got
	 which will cause it to get stripped from the output file
	 below.  */
      s = bfd_get_section_by_name (dynobj, ".rela.got");
      if (s != NULL)
	s->_raw_size = 0;
    }

  /* The check_relocs and adjust_dynamic_symbol entry points have
     determined the sizes of the various dynamic sections.  Allocate
     memory for them.  */
  plt = false;
  relocs = false;
  reltext = false;
  for (s = dynobj->sections; s != NULL; s = s->next)
    {
      const char *name;
      boolean strip;

      if ((s->flags & SEC_LINKER_CREATED) == 0)
	continue;

      /* It's OK to base decisions on the section name, because none
	 of the dynobj section names depend upon the input files.  */
      name = bfd_get_section_name (dynobj, s);

      strip = false;

      if (strcmp (name, ".plt") == 0)
	{
	  if (s->_raw_size == 0)
	    {
	      /* Strip this section if we don't need it; see the
                 comment below.  */
	      strip = true;
	    }
	  else
	    {
	      /* Remember whether there is a PLT.  */
	      plt = true;
	    }
	}
      else if (strncmp (name, ".rela", 5) == 0)
	{
	  if (s->_raw_size == 0)
	    {
	      /* If we don't need this section, strip it from the
		 output file.  This is mostly to handle .rela.bss and
		 .rela.plt.  We must create both sections in
		 create_dynamic_sections, because they must be created
		 before the linker maps input sections to output
		 sections.  The linker does that before
		 adjust_dynamic_symbol is called, and it is that
		 function which decides whether anything needs to go
		 into these sections.  */
	      strip = true;
	    }
	  else
	    {
	      asection *target;
	      const char *outname;

	      /* Remember whether there are any relocation sections. */
	      relocs = true;

	      /* If this relocation section applies to a read only
		 section, then we probably need a DT_TEXTREL entry.  */
	      outname = bfd_get_section_name (output_bfd,
					      s->output_section);
	      target = bfd_get_section_by_name (output_bfd, outname + 5);
	      if (target != NULL
		  && (target->flags & SEC_READONLY) != 0
		  && (target->flags & SEC_ALLOC) != 0)
		reltext = true;

	      /* We use the reloc_count field as a counter if we need
		 to copy relocs into the output file.  */
	      s->reloc_count = 0;
	    }
	}
      else if (strcmp (name, ".got") != 0
	       && strcmp (name, ".toc") != 0
	       && strcmp (name, ".glink") != 0)
	{
	  /* It's not one of our sections, so don't allocate space.  */
	  continue;
	}

      if (strip)
	{
	  _bfd_strip_section_from_output (info, s);
	  continue;
	}

#ifdef DEBUG
      fprintf (stderr, "\tallocating %lu bytes for section %s\n",
	       (unsigned long) s->_raw_size, name);
#endif

      /* Allocate memory for the section contents.  */
      s->contents = (bfd_byte *) bfd_zalloc (dynobj, s->_raw_size);
      if (s->contents == NULL && s->_raw_size != 0)
	return false;
    }

  if (elf_hash_table (info)->dynamic_sections_created)
    {
      /* Add some entries to the .dynamic section.  We fill in the
	 values later, in ppc64_elf_finish_dynamic_sections, but we
	 must add the entries now so that we get the correct size for
	 the .dynamic section.  The DT_DEBUG entry is filled in by the
	 dynamic linker and used by the debugger.  */
#define add_dynamic_entry(TAG, VAL) \
  bfd_elf64_add_dynamic_entry (info, (bfd_vma) (TAG), (bfd_vma) (VAL))

      if (!info->shared)
	{
	  if (!add_dynamic_entry (DT_DEBUG, 0))
	    return false;
	}

      if (plt)
	{
	  if (!add_dynamic_entry (DT_PLTGOT, 0)
	      || !add_dynamic_entry (DT_PLTRELSZ, 0)
	      || !add_dynamic_entry (DT_PLTREL, DT_RELA)
	      || !add_dynamic_entry (DT_JMPREL, 0))
	    return false;
	}

      if (relocs)
	{
	  if (!add_dynamic_entry (DT_RELA, 0)
	      || !add_dynamic_entry (DT_RELASZ, 0)
	      || !add_dynamic_entry (DT_RELAENT, sizeof (Elf64_External_Rela)))
	    return false;
	}

      if (reltext)
	{
	  if (!add_dynamic_entry (DT_TEXTREL, 0))
	    return false;
	}
    }
#undef add_dynamic_entry

  /* If we are generating a shared library, we generate a section
     symbol for each output section.  These are local symbols, which
     means that they must come first in the dynamic symbol table.
     That means we must increment the dynamic symbol index of every
     other dynamic symbol.

     FIXME: We assume that there will never be relocations to
     locations in linker-created sections that do not have
     externally-visible names. Instead, we should work out precisely
     which sections relocations are targetted at.  */
  if (info->shared)
    {
      int c;

      for (c = 0, s = output_bfd->sections; s != NULL; s = s->next)
	{
	  if ((s->flags & SEC_LINKER_CREATED) != 0
	      || (s->flags & SEC_ALLOC) == 0)
	    {
	      elf_section_data (s)->dynindx = -1;
	      continue;
	    }

	  /* These symbols will have no names, so we don't need to
	     fiddle with dynstr_index.  */

	  elf_section_data (s)->dynindx = c + 1;

	  c++;
	}

      elf_link_hash_traverse (elf_hash_table (info),
			      ppc_adjust_dynindx,
			      (PTR) &c);
      elf_hash_table (info)->dynsymcount += c;
    }

  return true;
}

/* Called after we have seen all the input files/sections, but before
   final symbol resolution and section placement has been determined.

   We use this hook to provide a value for TOCstart, which we store in
   the output bfd elf_gp, then we call the generic ELF final link
   routine.  */

static boolean
ppc64_elf_final_link (abfd, info)
     bfd *abfd;
     struct bfd_link_info *info;
{
  if (! info->relocateable)
    {
      asection *s;
      bfd_vma TOCstart;

      /* The TOC consists of sections .got, .toc, .tocbss, .plt in this
	 order.  The TOC starts where the first of these sections starts.  */
      s = bfd_get_section_by_name (abfd, ".got");
      if (s == NULL)
	s = bfd_get_section_by_name (abfd, ".toc");
      if (s == NULL)
	s = bfd_get_section_by_name (abfd, ".tocbss");
      if (s == NULL)
	s = bfd_get_section_by_name (abfd, ".plt");
      if (s == NULL)
	{
	  /* This may happen for
	     o  references to TOC base (SYM@toc / TOC[tc0]) without a
	     .toc directive
	     o  bad linker script
	     o --gc-sections and empty TOC sections

	     FIXME: Warn user?  */

	  /* Look for a likely section.  We probably won't even be
	     using TOCstart.  */
	  for (s = abfd->sections; s != NULL; s = s->next)
	    if ((s->flags & (SEC_ALLOC | SEC_SMALL_DATA | SEC_READONLY))
		== (SEC_ALLOC | SEC_SMALL_DATA))
	      break;
	  if (s == NULL)
	    for (s = abfd->sections; s != NULL; s = s->next)
	      if ((s->flags & (SEC_ALLOC | SEC_SMALL_DATA))
		  == (SEC_ALLOC | SEC_SMALL_DATA))
		break;
	  if (s == NULL)
	    for (s = abfd->sections; s != NULL; s = s->next)
	      if ((s->flags & (SEC_ALLOC | SEC_READONLY)) == SEC_ALLOC)
		break;
	  if (s == NULL)
	    for (s = abfd->sections; s != NULL; s = s->next)
	      if ((s->flags & SEC_ALLOC) == SEC_ALLOC)
		break;
	}

      TOCstart = 0;
      if (s != NULL)
	TOCstart = s->output_section->vma + s->output_offset;

      elf_gp (abfd) = TOCstart;
    }

  /* Invoke the regular gc ELF backend linker to do all the work.  */
  return _bfd_elf64_gc_common_final_link (abfd, info);
}

/* Set up any other section flags and such that may be necessary.  */

static boolean
ppc64_elf_fake_sections (abfd, shdr, asect)
     bfd *abfd ATTRIBUTE_UNUSED;
     Elf64_Internal_Shdr *shdr;
     asection *asect;
{
  if ((asect->flags & SEC_EXCLUDE) != 0)
    shdr->sh_flags |= SHF_EXCLUDE;

  if ((asect->flags & SEC_SORT_ENTRIES) != 0)
    shdr->sh_type = SHT_ORDERED;

  return true;
}

/* The RELOCATE_SECTION function is called by the ELF backend linker
   to handle the relocations for a section.

   The relocs are always passed as Rela structures; if the section
   actually uses Rel structures, the r_addend field will always be
   zero.

   This function is responsible for adjust the section contents as
   necessary, and (if using Rela relocs and generating a
   relocateable output file) adjusting the reloc addend as
   necessary.

   This function does not have to worry about setting the reloc
   address or the reloc symbol index.

   LOCAL_SYMS is a pointer to the swapped in local symbols.

   LOCAL_SECTIONS is an array giving the section in the input file
   corresponding to the st_shndx field of each local symbol.

   The global hash table entry for the global symbols can be found
   via elf_sym_hashes (input_bfd).

   When generating relocateable output, this function must handle
   STB_LOCAL/STT_SECTION symbols specially.  The output symbol is
   going to be the section symbol corresponding to the output
   section, which means that the addend must be adjusted
   accordingly.  */

static boolean
ppc64_elf_relocate_section (output_bfd, info, input_bfd, input_section,
			    contents, relocs, local_syms, local_sections)
     bfd *output_bfd;
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
  bfd *dynobj;
  Elf_Internal_Rela *rel;
  Elf_Internal_Rela *relend;
  asection *sreloc;
  asection *splt;
  asection *sgot;
  bfd_vma *local_got_offsets;
  bfd_vma TOCstart;
  boolean ret = true;

#ifdef DEBUG
  fprintf (stderr, "ppc64_elf_relocate_section called for %s section %s, %ld relocations%s\n",
	   bfd_get_filename (input_bfd),
	   bfd_section_name (input_bfd, input_section),
	   (long) input_section->reloc_count,
	   (info->relocateable) ? " (relocatable)" : "");
#endif

  if (!ppc64_elf_howto_table[R_PPC64_ADDR32])
    /* Initialize howto table if needed.  */
    ppc_howto_init ();

  local_got_offsets = elf_local_got_offsets (input_bfd);

  sreloc = NULL;
  splt = NULL;
  sgot = NULL;
  dynobj = elf_hash_table (info)->dynobj;
  if (dynobj != NULL)
    {
      splt = bfd_get_section_by_name (dynobj, ".plt");
      sgot = bfd_get_section_by_name (dynobj, ".got");
    }
  TOCstart = elf_gp (output_bfd);

  symtab_hdr = &elf_tdata (input_bfd)->symtab_hdr;
  sym_hashes = elf_sym_hashes (input_bfd);
  rel = relocs;
  relend = relocs + input_section->reloc_count;
  for (; rel < relend; rel++)
    {
      enum elf_ppc_reloc_type r_type;
      bfd_vma offset;
      bfd_vma addend;
      bfd_reloc_status_type r;
      Elf_Internal_Sym *sym;
      asection *sec;
      struct elf_link_hash_entry *h;
      const char *sym_name;
      reloc_howto_type *howto;
      unsigned long r_symndx;
      bfd_vma relocation;
      long insn;

      r_type = (enum elf_ppc_reloc_type) ELF64_R_TYPE (rel->r_info);
      offset = rel->r_offset;
      addend = rel->r_addend;
      r = bfd_reloc_other;
      sym = (Elf_Internal_Sym *) 0;
      sec = (asection *) 0;
      h = (struct elf_link_hash_entry *) 0;
      sym_name = (const char *) 0;

      /* Unknown relocation handling.  */
      if ((unsigned) r_type >= (unsigned) R_PPC_max
	  || !ppc64_elf_howto_table[(int) r_type])
	{
	  (*_bfd_error_handler) (_("%s: unknown relocation type %d"),
				 bfd_get_filename (input_bfd),
				 (int) r_type);
	  bfd_set_error (bfd_error_bad_value);
	  ret = false;
	  continue;
	}

      howto = ppc64_elf_howto_table[(int) r_type];
      r_symndx = ELF64_R_SYM (rel->r_info);

      if (info->relocateable)
	{
	  /* This is a relocatable link.  We don't have to change
	     anything, unless the reloc is against a section symbol,
	     in which case we have to adjust according to where the
	     section symbol winds up in the output section.  */
	  if (r_symndx < symtab_hdr->sh_info)
	    {
	      sym = local_syms + r_symndx;
	      if ((unsigned) ELF_ST_TYPE (sym->st_info) == STT_SECTION)
		{
		  sec = local_sections[r_symndx];
		  addend = rel->r_addend += sec->output_offset + sym->st_value;
		}
	    }

#ifdef DEBUG
	  fprintf (stderr, "\ttype = %s (%d), symbol index = %ld, offset = %ld, addend = %ld\n",
		   howto->name,
		   (int) r_type,
		   r_symndx,
		   (long) offset,
		   (long) addend);
#endif
	  continue;
	}

      /* This is a final link.  */

      /* Set `relocation'.  */
      if (r_type == R_PPC64_TOC)
	{
	  /* Relocation value is TOC base.  Symbol is ignored.  */
	  relocation = TOCstart + TOC_BASE_OFF;
	}
      else if (r_symndx < symtab_hdr->sh_info)
	{
	  /* It's a local symbol.  */
	  sym = local_syms + r_symndx;
	  sec = local_sections[r_symndx];
	  sym_name = "<local symbol>";

	  relocation = (sec->output_section->vma
			+ sec->output_offset
			+ sym->st_value);
	}
      else
	{
	  /* It's a global symbol.  */
	  h = sym_hashes[r_symndx - symtab_hdr->sh_info];
	  while (h->root.type == bfd_link_hash_indirect
		 || h->root.type == bfd_link_hash_warning)
	    h = (struct elf_link_hash_entry *) h->root.u.i.link;
	  sym_name = h->root.root.string;
	  if (h->root.type == bfd_link_hash_defined
	      || h->root.type == bfd_link_hash_defweak)
	    {
	      sec = h->root.u.def.section;
	      if ((r_type == R_PPC64_PLT32
		   && h->plt.offset != (bfd_vma) -1)
		  || ((r_type == R_PPC64_GOT16
		       || r_type == R_PPC64_GOT16_LO
		       || r_type == R_PPC64_GOT16_HI
		       || r_type == R_PPC64_GOT16_HA
		       || r_type == R_PPC64_GOT16_DS
		       || r_type == R_PPC64_GOT16_LO_DS)
		      && elf_hash_table (info)->dynamic_sections_created
		      && (! info->shared
			  || (! info->symbolic && h->dynindx != -1)
			  || (h->elf_link_hash_flags
			      & ELF_LINK_HASH_DEF_REGULAR) == 0))
		  || (info->shared
		      && ((! info->symbolic && h->dynindx != -1)
			  || (h->elf_link_hash_flags
			      & ELF_LINK_HASH_DEF_REGULAR) == 0)
		      && (input_section->flags & SEC_ALLOC) != 0
		      && (r_type == R_PPC64_ADDR64
			  || r_type == R_PPC64_ADDR32
			  || r_type == R_PPC64_ADDR24
			  || r_type == R_PPC64_ADDR16
			  || r_type == R_PPC64_ADDR16_LO
			  || r_type == R_PPC64_ADDR16_DS
			  || r_type == R_PPC64_ADDR16_LO_DS
			  || r_type == R_PPC64_ADDR16_HI
			  || r_type == R_PPC64_ADDR16_HA
			  || r_type == R_PPC64_ADDR16_HIGHER
			  || r_type == R_PPC64_ADDR16_HIGHERA
			  || r_type == R_PPC64_ADDR16_HIGHEST
			  || r_type == R_PPC64_ADDR16_HIGHESTA
			  || r_type == R_PPC64_ADDR14
			  || r_type == R_PPC64_ADDR14_BRTAKEN
			  || r_type == R_PPC64_ADDR14_BRNTAKEN
			  || r_type == R_PPC64_COPY
			  || r_type == R_PPC64_GLOB_DAT
			  || r_type == R_PPC64_JMP_SLOT
			  || r_type == R_PPC64_UADDR64
			  || r_type == R_PPC64_UADDR32
			  || r_type == R_PPC64_UADDR16)))
		{
		  /* In these cases, we don't need the relocation
                     value.  We check specially because in some
                     obscure cases sec->output_section will be NULL.  */
		  relocation = 0;
		}
	      else
		relocation = (h->root.u.def.value
			      + sec->output_section->vma
			      + sec->output_offset);
	    }
	  else if (h->root.type == bfd_link_hash_undefweak)
	    relocation = 0;
	  else if (info->shared
	       && (!info->symbolic || info->allow_shlib_undefined)
	       && !info->no_undefined)
	    relocation = 0;
	  else
	    {
	      if (! ((*info->callbacks->undefined_symbol)
		     (info, h->root.root.string, input_bfd, input_section,
		      rel->r_offset, (!info->shared
				      || info->no_undefined
				      || ELF_ST_VISIBILITY (h->other)))))
		return false;
	      relocation = 0;
	    }
	}

      switch ((int) r_type)
	{
	case (int) R_PPC64_ADDR16_DS:
	case (int) R_PPC64_ADDR16_LO_DS:
	case (int) R_PPC64_GOT16_DS:
	case (int) R_PPC64_GOT16_LO_DS:
	case (int) R_PPC64_PLT16_LO_DS:
	case (int) R_PPC64_SECTOFF_DS:
	case (int) R_PPC64_SECTOFF_LO_DS:
	case (int) R_PPC64_TOC16_DS:
	case (int) R_PPC64_TOC16_LO_DS:
	case (int) R_PPC64_PLTGOT16_DS:
	case (int) R_PPC64_PLTGOT16_LO_DS:
	  if (((relocation + addend) & 3) != 0)
	    {
	      (*_bfd_error_handler) (_("%s: error: relocation %s not a multiple of 4"),
				     bfd_get_filename (input_bfd),
				     howto->name);
	      bfd_set_error (bfd_error_bad_value);
	      ret = false;
	      continue;
	    }
	}

      /* Set `addend'.  */
      switch ((int) r_type)
	{
	default:
	  (*_bfd_error_handler) (_("%s: unknown relocation type %d for symbol %s"),
				 bfd_get_filename (input_bfd),
				 (int) r_type, sym_name);

	  bfd_set_error (bfd_error_bad_value);
	  ret = false;
	  continue;

	  /* Relocations that may need to be propagated if this is a shared
	     object.  */
	case (int) R_PPC64_REL24:
	case (int) R_PPC64_ADDR24:
	  /* An ADDR24 or REL24 branching to a linkage function may be
	     followed by a nop that we have to replace with an ld in
	     order to restore the TOC base pointer.  Only calls to
	     shared objects need to alter the TOC base.  These are
	     recognized by their need for a PLT entry.  */
	  if (h != NULL
	      && (h->elf_link_hash_flags & ELF_LINK_HASH_NEEDS_PLT) != 0
	      /* Make sure that there really is an instruction after
                 the branch that we can decode.  */
	      && (rel->r_offset - input_section->vma + 8
		  <= input_section->_cooked_size))
	    {
	      bfd_byte *pnext;

	      pnext = contents + (rel->r_offset - input_section->vma) + 4;
	      insn = bfd_get_32 (input_bfd, pnext);

	      if (insn == 0x60000000	 /* nop (ori  r0,r0,0) */
		  || insn == 0x4def7b82	 /* cror 15,15,15 */
		  || insn == 0x4ffffb82) /* cror 31,31,31 */
		{
		  bfd_put_32 (input_bfd,
			      (bfd_vma) 0xe8410028, /* ld r2,40(r1) */
			      pnext);
#ifdef DEBUG
		  fprintf (stderr, "ppc64_elf_relocate_section: " \
			   "patched nop after call to %s\n", sym_name);
#endif
		}
	    }
	  /* fall through */

	case (int) R_PPC64_REL64:
	case (int) R_PPC64_REL32:
	case (int) R_PPC64_REL14:
	  /* If these relocations are not to a named symbol, they can be
	     handled right here, no need to bother the dynamic linker.  */
	  if (h == NULL && r_type != R_PPC64_ADDR24)
	    break;
	  /* fall through */

	  /* Relocations that always need to be propagated if this is a shared
	     object.  */
	case (int) R_PPC64_NONE:
	case (int) R_PPC64_ADDR64:
	case (int) R_PPC64_ADDR32:
	case (int) R_PPC64_ADDR16:
	case (int) R_PPC64_ADDR16_LO:
	case (int) R_PPC64_ADDR16_DS:
	case (int) R_PPC64_ADDR16_LO_DS:
	case (int) R_PPC64_ADDR16_HI:
	case (int) R_PPC64_ADDR16_HA:
	case (int) R_PPC64_ADDR16_HIGHER:
	case (int) R_PPC64_ADDR16_HIGHERA:
	case (int) R_PPC64_ADDR16_HIGHEST:
	case (int) R_PPC64_ADDR16_HIGHESTA:
	case (int) R_PPC64_ADDR14:
	case (int) R_PPC64_UADDR64:
	case (int) R_PPC64_UADDR32:
	case (int) R_PPC64_UADDR16:
	case (int) R_PPC64_TOC:
	  if (info->shared)
	    {
	      Elf_Internal_Rela outrel;
	      boolean skip;

#ifdef DEBUG
	      fprintf (stderr, "ppc64_elf_relocate_section need to create relocation for %s\n",
		       (h && h->root.root.string
			? h->root.root.string
			: "<unknown>"));
#endif

	      /* When generating a shared object, these relocations
                 are copied into the output file to be resolved at run
                 time.  */

	      if (sreloc == NULL)
		{
		  const char *name;

		  name = (bfd_elf_string_from_elf_section
			  (input_bfd,
			   elf_elfheader (input_bfd)->e_shstrndx,
			   elf_section_data (input_section)->rel_hdr.sh_name));
		  if (name == NULL)
		    return false;

		  BFD_ASSERT (strncmp (name, ".rela", 5) == 0
			      && strcmp (bfd_get_section_name (input_bfd,
							       input_section),
					 name + 5) == 0);

		  sreloc = bfd_get_section_by_name (dynobj, name);
		  BFD_ASSERT (sreloc != NULL);
		}

	      skip = false;

	      if (elf_section_data (input_section)->stab_info == NULL)
		outrel.r_offset = rel->r_offset;
	      else
		{
		  bfd_vma off;

		  off = (_bfd_stab_section_offset
			 (output_bfd, &elf_hash_table (info)->stab_info,
			  input_section,
			  &elf_section_data (input_section)->stab_info,
			  rel->r_offset));
		  if (off == (bfd_vma) -1)
		    skip = true;
		  outrel.r_offset = off;
		}

	      outrel.r_offset += (input_section->output_section->vma
				  + input_section->output_offset);

	      if (skip)
		memset (&outrel, 0, sizeof outrel);
	      /* h->dynindx may be -1 if this symbol was marked to
                 become local.  */
	      else if (h != NULL
		       && ((! info->symbolic && h->dynindx != -1)
			   || (h->elf_link_hash_flags
			       & ELF_LINK_HASH_DEF_REGULAR) == 0))
		{
		  BFD_ASSERT (h->dynindx != -1);
		  outrel.r_info = ELF64_R_INFO (h->dynindx, r_type);
		  outrel.r_addend = rel->r_addend;
		}
	      else
		{
		  if (r_type == R_PPC64_ADDR64)
		    {
		      outrel.r_info = ELF64_R_INFO (0, R_PPC64_RELATIVE);
		      outrel.r_addend = relocation + rel->r_addend;
		    }
		  else
		    {
		      long indx;

		      if (h == NULL)
			sec = local_sections[r_symndx];
		      else
			{
			  BFD_ASSERT (h->root.type == bfd_link_hash_defined
				      || (h->root.type
					  == bfd_link_hash_defweak));
			  sec = h->root.u.def.section;
			}

		      if (r_type == R_PPC64_TOC
			  || (sec != NULL && bfd_is_abs_section (sec)))
			{
			  indx = 0;
			}
		      else if (sec == NULL || sec->owner == NULL)
			{
			  bfd_set_error (bfd_error_bad_value);
			  return false;
			}
		      else
			{
			  asection *osec;

			  osec = sec->output_section;
			  indx = elf_section_data (osec)->dynindx;
			  BFD_ASSERT (indx > 0);
#ifdef DEBUG
			  if (indx <= 0)
			    {
			      printf ("indx=%ld section=%s flags=%08x name=%s\n",
				      indx, osec->name, osec->flags,
				      h->root.root.string);
			    }
#endif
			}

		      outrel.r_info = ELF64_R_INFO (indx, r_type);
		      outrel.r_addend = relocation + rel->r_addend;
		    }
		}

	      bfd_elf64_swap_reloca_out (output_bfd, &outrel,
					 (((Elf64_External_Rela *)
					   sreloc->contents)
					  + sreloc->reloc_count));
	      ++sreloc->reloc_count;

	      /* This reloc will be computed at runtime, so there's no
                 need to do anything now, unless this is a RELATIVE
                 reloc in an unallocated section.  */
	      if (skip
		  || (input_section->flags & SEC_ALLOC) != 0
		  || ELF64_R_TYPE (outrel.r_info) != R_PPC64_RELATIVE)
		continue;
	    }

	  /* Arithmetic adjust relocations that aren't going into a
	     shared object.  */
	  if ((r_type == R_PPC64_ADDR16_HA
	       || r_type == R_PPC64_ADDR16_HIGHERA
	       || r_type == R_PPC64_ADDR16_HIGHESTA)
	      /* It's just possible that this symbol is a weak symbol
		 that's not actually defined anywhere. In that case,
		 'sec' would be NULL, and we should leave the symbol
		 alone (it will be set to zero elsewhere in the link).  */
	      && sec != NULL)
	    {
	      addend += ((relocation + addend) & 0x8000) << 1;
	    }
	  break;

	  /* Branch taken prediction relocations.  */
	case (int) R_PPC64_ADDR14_BRTAKEN:
	case (int) R_PPC64_REL14_BRTAKEN:
	  insn = bfd_get_32 (output_bfd, contents + offset);
	  if ((relocation - offset) & 0x8000)
	    insn &= ~BRANCH_PREDICT_BIT;
	  else
	    insn |= BRANCH_PREDICT_BIT;
	  bfd_put_32 (output_bfd, (bfd_vma) insn, contents + offset);
	  break;

	  /* Branch not taken predicition relocations.  */
	case (int) R_PPC64_ADDR14_BRNTAKEN:
	case (int) R_PPC64_REL14_BRNTAKEN:
	  insn = bfd_get_32 (output_bfd, contents + offset);
	  if ((relocation - offset) & 0x8000)
	    insn |= BRANCH_PREDICT_BIT;
	  else
	    insn &= ~BRANCH_PREDICT_BIT;
	  bfd_put_32 (output_bfd, (bfd_vma) insn, contents + offset);
	  break;


	  /* GOT16 relocations.  Like an ADDR16 using the symbol's
	     address in the GOT as relocation value instead of the
	     symbols value itself.  Also, create a GOT entry for the
	     symbol and put the symbol value there.  */
	  /* FIXME: If the symbol is not in the .got section
	     (typically in the .toc section) we fail (h->got.offset is
	     0).  */
	  /* NOTE: Sets `relocation'.  */
	case (int) R_PPC64_GOT16:
	case (int) R_PPC64_GOT16_LO:
	case (int) R_PPC64_GOT16_HI:
	case (int) R_PPC64_GOT16_HA:
	case (int) R_PPC64_GOT16_DS:
	case (int) R_PPC64_GOT16_LO_DS:
	  {
	    /* Relocation is to the entry for this symbol in the global
	       offset table.  */
	    bfd_vma off;
	    BFD_ASSERT (sgot != NULL);

	    if (h != NULL)
	      {
		off = h->got.offset;
		BFD_ASSERT (off != (bfd_vma) -1);

		if (! elf_hash_table (info)->dynamic_sections_created
		    || (info->shared
			&& (info->symbolic || h->dynindx == -1)
			&& (h->elf_link_hash_flags
			    & ELF_LINK_HASH_DEF_REGULAR)))
		  {
		    /* This is actually a static link, or it is a
		       -Bsymbolic link and the symbol is defined
		       locally.  We must initialize this entry in the
		       global offset table.  Since the offset must
		       always be a multiple of 4, we use the least
		       significant bit to record whether we have
		       initialized it already.

		       When doing a dynamic link, we create a .rela.got
		       relocation entry to initialize the value.  This
		       is done in the finish_dynamic_symbol routine.  */
		    if ((off & 1) != 0)
		      off &= ~1;
		    else
		      {
			bfd_put_64 (output_bfd, relocation,
				    sgot->contents + off);
			h->got.offset |= 1;
		      }
		  }
	      }
	    else
	      {
		BFD_ASSERT (local_got_offsets != NULL
			    && local_got_offsets[r_symndx] != (bfd_vma) -1);

		off = local_got_offsets[r_symndx];

		/* The offset must always be a multiple of 4.  We use
		   the least significant bit to record whether we have
		   already processed this entry.  */
		if ((off & 1) != 0)
		  off &= ~1;
		else
		  {
		    bfd_put_64 (output_bfd, relocation, sgot->contents + off);

		    if (info->shared)
		      {
			asection *srelgot;
			Elf_Internal_Rela outrel;

			/* We need to generate a R_PPC64_RELATIVE reloc
			   for the dynamic linker.  */
			srelgot = bfd_get_section_by_name (dynobj, ".rela.got");
			BFD_ASSERT (srelgot != NULL);

			outrel.r_offset = (sgot->output_section->vma
					   + sgot->output_offset
					   + off);
			outrel.r_info = ELF64_R_INFO (0, R_PPC64_RELATIVE);
			outrel.r_addend = relocation;
			bfd_elf64_swap_reloca_out (output_bfd, &outrel,
						   (((Elf64_External_Rela *)
						     srelgot->contents)
						    + srelgot->reloc_count));
			++srelgot->reloc_count;
		      }

		    local_got_offsets[r_symndx] |= 1;
		  }
	      }

	    relocation = sgot->output_offset + off;
	    /* TOC base (r2) is TOC start plus 0x8000.  */
	    addend -= TOC_BASE_OFF;
	    break;
	  }

	  /* TOC16 relocs.  We want the offset relative to the TOC base,
	     which is the address of the start of the TOC plus 0x8000.
	     The TOC consists of sections .got, .toc, .tocbss, and .plt,
	     in this order.  */

	case (int) R_PPC64_TOC16:
	case (int) R_PPC64_TOC16_LO:
	case (int) R_PPC64_TOC16_HI:
	case (int) R_PPC64_TOC16_DS:
	case (int) R_PPC64_TOC16_LO_DS:
	  BFD_ASSERT (sec != (asection *) 0);
	  /* Only .got, .toc and *UND* symbols are allowed.  */
	  BFD_ASSERT (bfd_is_und_section (sec)
		      || strcmp (bfd_get_section_name (abfd, sec),
				 ".got") == 0
		      || strcmp (bfd_get_section_name (abfd, sec),
				 ".toc") == 0);

	  addend -= TOCstart + TOC_BASE_OFF;
	  break;

	case (int) R_PPC64_TOC16_HA:
	  BFD_ASSERT (sec != (asection *) 0);
	  BFD_ASSERT (bfd_is_und_section (sec)
		      || strcmp (bfd_get_section_name (abfd, sec),
				 ".got") == 0
		      || strcmp (bfd_get_section_name (abfd, sec),
				 ".toc") == 0);

	  addend -= TOCstart + TOC_BASE_OFF;
	  /* Add 1 to 16-bit word 16:31 if sign bit in 0:15 is set.  */
	  addend += ((relocation + addend) & 0x8000) << 1;
	  break;

	  /* Relocate against the beginning of the section.  */
	case (int) R_PPC64_SECTOFF:
	case (int) R_PPC64_SECTOFF_LO:
	case (int) R_PPC64_SECTOFF_HI:
	case (int) R_PPC64_SECTOFF_DS:
	case (int) R_PPC64_SECTOFF_LO_DS:
	  BFD_ASSERT (sec != (asection *) 0);
	  addend -= sec->output_section->vma;
	  break;

	case (int) R_PPC64_SECTOFF_HA:
	  BFD_ASSERT (sec != (asection *) 0);
	  addend -= sec->output_section->vma;
	  addend += ((relocation + addend) & 0x8000) << 1;
	  break;

	case (int) R_PPC64_COPY:
	case (int) R_PPC64_GLOB_DAT:
	case (int) R_PPC64_JMP_SLOT:
	case (int) R_PPC64_RELATIVE:
	case (int) R_PPC64_PLT32:
	case (int) R_PPC64_PLTREL32:
	case (int) R_PPC64_PLT16_LO:
	case (int) R_PPC64_PLT16_LO_DS:
	case (int) R_PPC64_PLT16_HI:
	case (int) R_PPC64_PLT16_HA:
	case (int) R_PPC64_PLT64:
	case (int) R_PPC64_PLTREL64:
	case (int) R_PPC64_PLTGOT16:
	case (int) R_PPC64_PLTGOT16_LO:
	case (int) R_PPC64_PLTGOT16_DS:
	case (int) R_PPC64_PLTGOT16_LO_DS:
	case (int) R_PPC64_PLTGOT16_HI:
	case (int) R_PPC64_PLTGOT16_HA:
	  (*_bfd_error_handler) (_("%s: Relocation %s is not yet supported for symbol %s."),
				 bfd_get_filename (input_bfd),
				 ppc64_elf_howto_table[(int) r_type]->name,
				 sym_name);

	  bfd_set_error (bfd_error_invalid_operation);
	  ret = false;
	  continue;

	case (int) R_PPC_GNU_VTINHERIT:
	case (int) R_PPC_GNU_VTENTRY:
	  /* These are no-ops in the end.  */
	  continue;
	}

#ifdef DEBUG
      fprintf (stderr, "  type = %s (%d), name = %s, sym index = %ld, offset = %ld, addend = %ld\n",
	       howto->name,
	       (int) r_type,
	       sym_name,
	       r_symndx,
	       (long) offset,
	       (long) addend);
#endif

      r = _bfd_final_link_relocate (howto,
				    input_bfd,
				    input_section,
				    contents,
				    offset,
				    relocation,
				    addend);

      if (r == bfd_reloc_ok)
	;
      else if (r == bfd_reloc_overflow)
	{
	  const char *name;

	  if (h != NULL)
	    {
	      if (h->root.type == bfd_link_hash_undefweak
		  && howto->pc_relative)
		{
		  /* Assume this is a call protected by other code that
		     detects the symbol is undefined.  If this is the case,
		     we can safely ignore the overflow.  If not, the
		     program is hosed anyway, and a little warning isn't
		     going to help.  */

		  continue;
		}

	      name = h->root.root.string;
	    }
	  else
	    {
	      name = bfd_elf_string_from_elf_section (input_bfd,
						      symtab_hdr->sh_link,
						      sym->st_name);
	      if (name == NULL)
		continue;
	      if (*name == '\0')
		name = bfd_section_name (input_bfd, sec);
	    }

	  if (! (*info->callbacks->reloc_overflow) (info,
						    name,
						    howto->name,
						    (bfd_vma) 0,
						    input_bfd,
						    input_section,
						    offset))
	    return false;
	}
      else
	ret = false;
    }

#ifdef DEBUG
  fprintf (stderr, "\n");
#endif

  return ret;
}

/* Finish up dynamic symbol handling.  We set the contents of various
   dynamic sections here.  */

static boolean
ppc64_elf_finish_dynamic_symbol (output_bfd, info, h, sym)
     bfd *output_bfd;
     struct bfd_link_info *info;
     struct elf_link_hash_entry *h;
     Elf_Internal_Sym *sym;
{
  bfd *dynobj;

#ifdef DEBUG
  fprintf (stderr, "ppc64_elf_finish_dynamic_symbol called for %s",
	   h->root.root.string);
#endif

  dynobj = elf_hash_table (info)->dynobj;
  BFD_ASSERT (dynobj != NULL);

  if (h->plt.offset != (bfd_vma) -1)
    {
      asection *splt, *srela, *sglink;
      Elf_Internal_Rela rela;
      bfd_vma reloc_index, plt_offset;
      struct elf_link_hash_entry *funcdesc_h;

#ifdef DEBUG
      fprintf (stderr, ", plt_offset = 0x%lx", (unsigned long) h->plt.offset);
#endif

      /* This symbol has an entry in the procedure linkage table.  Set
         it up.  */

      BFD_ASSERT (h->dynindx != -1);

      splt = bfd_get_section_by_name (dynobj, ".plt");
      srela = bfd_get_section_by_name (dynobj, ".rela.plt");
      BFD_ASSERT (splt != NULL && srela != NULL);

      /* We don't need to fill in the .plt.  The dynamic linker will
	 fill it in.  */

      /* We may need to create a linkage function. */
      sglink = bfd_get_section_by_name (dynobj, ".glink");
      if (sglink != NULL && h->root.u.def.section == sglink)
	{
	  bfd_byte *p;
	  bfd_vma pltoff;
	  const unsigned int *stub;

	  /* Where to write it.  */
	  p = h->root.u.def.section->contents + h->root.u.def.value;

	  /* The function descriptor is in the PLT.  */
	  pltoff = splt->output_section->vma
	    + splt->output_section->output_offset
	    + h->plt.offset
	    - elf_gp (output_bfd) - TOC_BASE_OFF;

	  if (pltoff + 0x8000 > 0xffff)
	    {
	      (*_bfd_error_handler)
		(_("linkage table overflow against `%s'"),
		 h->root.root.string);
	    }

	  /* Write it out.  */
	  stub = ppc64_elf_glink_code;
	  bfd_put_32 (output_bfd, *stub | (pltoff & 0xfffc), p);
	  while (p += 4, ++stub < (ppc64_elf_glink_code
				   + (sizeof (ppc64_elf_glink_code)
				      / sizeof (*ppc64_elf_glink_code))));
	    {
	      bfd_put_32 (output_bfd, (bfd_vma) *stub, p);
	    }
#ifdef DEBUG
	  fprintf (stderr, ", linkage function");
#endif
	}

      /* Create a JMP_SLOT reloc to inform the dynamic linker to fix
         the PLT entry.  */

      /* Save the PLT offset, since `h' may be changed below.  */
      plt_offset = h->plt.offset;

      /* If it's a function entry point, find its corresponding
	 function descriptor.  ppc64_elf_adjust_dynamic_symbol have
	 already set it up for us.  */

      /* FIXME: Is it safe to assume that this symbol is a function?  */
      BFD_ASSERT (h->root.root.string[0] == '.');

      /* FIXME: Follow bfd_link_hash_indirect?  */
      funcdesc_h = elf_link_hash_lookup (elf_hash_table (info),
					 h->root.root.string + 1,
					 false, false, false);
      BFD_ASSERT (funcdesc_h != NULL);
      BFD_ASSERT (funcdesc_h->dynindx != -1);

      /* FIXME: Rename the symbol for the function entry point to
         `linkage_for_FUNC'?  */

      rela.r_offset = (splt->output_section->vma
		       + splt->output_offset
		       + plt_offset);
      rela.r_info = ELF64_R_INFO (funcdesc_h->dynindx, R_PPC64_JMP_SLOT);
      rela.r_addend = 0;

      reloc_index = (plt_offset - PLT_INITIAL_ENTRY_SIZE) / PLT_ENTRY_SIZE;
      bfd_elf64_swap_reloca_out (output_bfd, &rela,
				 ((Elf64_External_Rela *) srela->contents
				  + reloc_index));
#ifdef DEBUG
      fprintf (stderr, ", JMP_SLOT for %s", funcdesc_h->root.root.string);
#endif

#if 0
      /* FIXME: Really do this?  */
      if ((h->elf_link_hash_flags & ELF_LINK_HASH_DEF_REGULAR) == 0)
	{
	  /* Mark the symbol as undefined, rather than as defined in
	     the .plt section.  Leave the value alone.  */
	  sym->st_shndx = SHN_UNDEF;
	}
#endif
    }

  if (h->got.offset != (bfd_vma) -1)
    {
      asection *sgot;
      asection *srela;
      Elf_Internal_Rela rela;

#ifdef DEBUG
      fprintf (stderr, ", got.offset = %08lx", (unsigned long) h->got.offset);
#endif

      /* This symbol has an entry in the global offset table.  Set it
         up.  */

      sgot = bfd_get_section_by_name (dynobj, ".got");
      srela = bfd_get_section_by_name (dynobj, ".rela.got");
      BFD_ASSERT (sgot != NULL && srela != NULL);

      rela.r_offset = (sgot->output_section->vma
		       + sgot->output_offset
		       + (h->got.offset &~ (bfd_vma) 1));

      /* If this is a -Bsymbolic link, and the symbol is defined
	 locally, we just want to emit a RELATIVE reloc.  The entry in
	 the global offset table will already have been initialized in
	 the relocate_section function.  */
      if (info->shared
	  && (info->symbolic || h->dynindx == -1)
	  && (h->elf_link_hash_flags & ELF_LINK_HASH_DEF_REGULAR))
	{
	  rela.r_info = ELF64_R_INFO (0, R_PPC64_RELATIVE);
	  rela.r_addend = (h->root.u.def.value
			   + h->root.u.def.section->output_section->vma
			   + h->root.u.def.section->output_offset);
	}
      else
	{
	  BFD_ASSERT ((h->got.offset & 1) == 0);
	  bfd_put_64 (output_bfd, (bfd_vma) 0, sgot->contents + h->got.offset);
	  rela.r_info = ELF64_R_INFO (h->dynindx, R_PPC64_GLOB_DAT);
	  rela.r_addend = 0;
	}

      bfd_elf64_swap_reloca_out (output_bfd, &rela,
				 ((Elf64_External_Rela *) srela->contents
				  + srela->reloc_count));
      ++srela->reloc_count;
    }

  if ((h->elf_link_hash_flags & ELF_LINK_HASH_NEEDS_COPY) != 0)
    {
      asection *s;
      Elf_Internal_Rela rela;

      /* This symbols needs a copy reloc.  Set it up.  */

#ifdef DEBUG
      fprintf (stderr, ", copy");
#endif

      BFD_ASSERT (h->dynindx != -1);

      s = bfd_get_section_by_name (h->root.u.def.section->owner,
				   ".rela.bss");
      BFD_ASSERT (s != NULL);

      rela.r_offset = (h->root.u.def.value
		       + h->root.u.def.section->output_section->vma
		       + h->root.u.def.section->output_offset);
      rela.r_info = ELF64_R_INFO (h->dynindx, R_PPC64_COPY);
      rela.r_addend = 0;
      bfd_elf64_swap_reloca_out (output_bfd, &rela,
				 ((Elf64_External_Rela *) s->contents
				  + s->reloc_count));
      ++s->reloc_count;
    }

#ifdef DEBUG
  fprintf (stderr, "\n");
#endif

  /* Mark some specially defined symbols as absolute.  */
  if (strcmp (h->root.root.string, "_DYNAMIC") == 0)
    sym->st_shndx = SHN_ABS;

  return true;
}

/* Finish up the dynamic sections.  */

static boolean
ppc64_elf_finish_dynamic_sections (output_bfd, info)
     bfd *output_bfd;
     struct bfd_link_info *info;
{
  asection *sdyn;
  bfd *dynobj = elf_hash_table (info)->dynobj;

#ifdef DEBUG
  fprintf (stderr, "ppc64_elf_finish_dynamic_sections called\n");
#endif

  sdyn = bfd_get_section_by_name (dynobj, ".dynamic");

  if (elf_hash_table (info)->dynamic_sections_created)
    {
      asection *splt;
      Elf64_External_Dyn *dyncon, *dynconend;

      splt = bfd_get_section_by_name (dynobj, ".plt");
      BFD_ASSERT (splt != NULL && sdyn != NULL);

      dyncon = (Elf64_External_Dyn *) sdyn->contents;
      dynconend = (Elf64_External_Dyn *) (sdyn->contents + sdyn->_raw_size);
      for (; dyncon < dynconend; dyncon++)
	{
	  Elf_Internal_Dyn dyn;
	  const char *name;
	  boolean size;

	  bfd_elf64_swap_dyn_in (dynobj, dyncon, &dyn);

	  switch (dyn.d_tag)
	    {
	    case DT_PLTGOT:   name = ".plt";	  size = false; break;
	    case DT_PLTRELSZ: name = ".rela.plt"; size = true;  break;
	    case DT_JMPREL:   name = ".rela.plt"; size = false; break;
	    default:	      name = NULL;	  size = false; break;
	    }

	  if (name != NULL)
	    {
	      asection *s;

	      s = bfd_get_section_by_name (output_bfd, name);
	      if (s == NULL)
		dyn.d_un.d_val = 0;
	      else
		{
		  if (! size)
		    dyn.d_un.d_ptr = s->vma;
		  else
		    {
		      if (s->_cooked_size != 0)
			dyn.d_un.d_val = s->_cooked_size;
		      else
			dyn.d_un.d_val = s->_raw_size;
		    }
		}
	      bfd_elf64_swap_dyn_out (output_bfd, &dyn, dyncon);
	    }
	}
    }

  if (info->shared)
    {
      asection *sdynsym;
      asection *s;
      Elf_Internal_Sym sym;
      int maxdindx = 0;

      /* Set up the section symbols for the output sections.  */

      sdynsym = bfd_get_section_by_name (dynobj, ".dynsym");
      BFD_ASSERT (sdynsym != NULL);

      sym.st_size = 0;
      sym.st_name = 0;
      sym.st_info = ELF_ST_INFO (STB_LOCAL, STT_SECTION);
      sym.st_other = 0;

      for (s = output_bfd->sections; s != NULL; s = s->next)
	{
	  int indx, dindx;

	  sym.st_value = s->vma;

	  indx = elf_section_data (s)->this_idx;
	  dindx = elf_section_data (s)->dynindx;
	  if (dindx != -1)
	    {
	      BFD_ASSERT (indx > 0);
	      BFD_ASSERT (dindx > 0);

	      if (dindx > maxdindx)
		maxdindx = dindx;

	      sym.st_shndx = indx;

	      bfd_elf64_swap_symbol_out (output_bfd, &sym,
					 (PTR) (((Elf64_External_Sym *)
						 sdynsym->contents)
						+ dindx));
	    }
	}

      /* Set the sh_info field of the output .dynsym section to the
         index of the first global symbol.  */
      elf_section_data (sdynsym->output_section)->this_hdr.sh_info =
	maxdindx + 1;
    }

  return true;
}

#define TARGET_LITTLE_SYM	bfd_elf64_powerpcle_vec
#define TARGET_LITTLE_NAME	"elf64-powerpcle"
#define TARGET_BIG_SYM		bfd_elf64_powerpc_vec
#define TARGET_BIG_NAME		"elf64-powerpc"
#define ELF_ARCH		bfd_arch_powerpc
#define ELF_MACHINE_CODE	EM_PPC64
#define ELF_MAXPAGESIZE		0x10000
#define elf_info_to_howto	ppc64_elf_info_to_howto

#ifdef  EM_CYGNUS_POWERPC
#define ELF_MACHINE_ALT1	EM_CYGNUS_POWERPC
#endif

#ifdef EM_PPC_OLD
#define ELF_MACHINE_ALT2	EM_PPC_OLD
#endif

#define elf_backend_want_got_sym 0
#define elf_backend_want_plt_sym 0
#define elf_backend_plt_alignment 3
#define elf_backend_plt_not_loaded 1
#define elf_backend_got_symbol_offset 0
#define elf_backend_got_header_size 0
#define elf_backend_can_gc_sections 1

#define elf_backend_plt_header_size PLT_INITIAL_ENTRY_SIZE

#define bfd_elf64_bfd_copy_private_bfd_data	ppc64_elf_copy_private_bfd_data
#define bfd_elf64_bfd_merge_private_bfd_data	ppc64_elf_merge_private_bfd_data
#define bfd_elf64_bfd_reloc_type_lookup		ppc64_elf_reloc_type_lookup
#define bfd_elf64_bfd_set_private_flags		ppc64_elf_set_private_flags
#define bfd_elf64_bfd_final_link		ppc64_elf_final_link

#define elf_backend_gc_mark_hook		ppc64_elf_gc_mark_hook
#define elf_backend_gc_sweep_hook		ppc64_elf_gc_sweep_hook
#define elf_backend_section_from_shdr		ppc64_elf_section_from_shdr
#define elf_backend_relocate_section		ppc64_elf_relocate_section
#define elf_backend_create_dynamic_sections	ppc64_elf_create_dynamic_sections
#define elf_backend_check_relocs		ppc64_elf_check_relocs
#define elf_backend_adjust_dynamic_symbol	ppc64_elf_adjust_dynamic_symbol
#define elf_backend_size_dynamic_sections	ppc64_elf_size_dynamic_sections
#define elf_backend_finish_dynamic_symbol	ppc64_elf_finish_dynamic_symbol
#define elf_backend_finish_dynamic_sections	ppc64_elf_finish_dynamic_sections
#define elf_backend_fake_sections		ppc64_elf_fake_sections

#include "elf64-target.h"
