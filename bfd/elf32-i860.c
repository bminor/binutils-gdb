/* Intel i860 specific support for 32-bit ELF.
   Copyright 1993, 2000 Free Software Foundation, Inc.

   Full i860 support contributed by Jason Eckhardt <jle@cygnus.com>.

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

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"
#include "elf-bfd.h"
#include "elf/i860.h"


/* Prototypes.  */
static reloc_howto_type *lookup_howto
  PARAMS ((unsigned int));

static reloc_howto_type *elf32_i860_reloc_type_lookup
  PARAMS ((bfd *abfd, bfd_reloc_code_real_type code));

static void elf32_i860_info_to_howto_rela 
  PARAMS ((bfd *, arelent *, Elf32_Internal_Rela *));


/* This howto table is preliminary.  */
static reloc_howto_type elf32_i860_howto_table [] =
{
  /* This relocation does nothing.  */
  HOWTO (R_860_NONE,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_860_NONE",		/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 false),		/* pcrel_offset */

  /* A 32-bit absolute relocation.  */
  HOWTO (R_860_32,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_860_32",		/* name */
	 false,			/* partial_inplace */
	 0xffffffff,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  HOWTO (R_860_COPY,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_860_COPY",		/* name */
	 true,			/* partial_inplace */
	 0xffffffff,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  HOWTO (R_860_GLOB_DAT,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_860_GLOB_DAT",	/* name */
	 true,			/* partial_inplace */
	 0xffffffff,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  HOWTO (R_860_JUMP_SLOT,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_860_JUMP_SLOT",	/* name */
	 true,			/* partial_inplace */
	 0xffffffff,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  HOWTO (R_860_RELATIVE,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_860_RELATIVE",	/* name */
	 true,			/* partial_inplace */
	 0xffffffff,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* A 26-bit PC-relative relocation.  */
  HOWTO (R_860_PC26,	        /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 26,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_860_PC26",		/* name */
	 false,			/* partial_inplace */
	 0x3ffffff,		/* src_mask */
	 0x3ffffff,		/* dst_mask */
	 true),		        /* pcrel_offset */

  HOWTO (R_860_PLT26,	        /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 26,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_860_PLT26",		/* name */
	 true,			/* partial_inplace */
	 0xffffffff,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 true),		        /* pcrel_offset */

  /* A 16-bit PC-relative relocation.  */
  HOWTO (R_860_PC16,	        /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_860_PC16",		/* name */
	 false,			/* partial_inplace */
	 0x1f07ff,		/* src_mask */
	 0x1f07ff,		/* dst_mask */
	 true),		        /* pcrel_offset */

  HOWTO (R_860_LOW0,	        /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_860_LOW0",		/* name */
	 false,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 false),	        /* pcrel_offset */

  HOWTO (R_860_SPLIT0,	        /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_860_SPLIT0",	/* name */
	 false,			/* partial_inplace */
	 0x1f07ff,		/* src_mask */
	 0x1f07ff,		/* dst_mask */
	 false),	        /* pcrel_offset */

  HOWTO (R_860_LOW1,	        /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_860_LOW1",		/* name */
	 false,			/* partial_inplace */
	 0xfffe,		/* src_mask */
	 0xfffe,		/* dst_mask */
	 false),	        /* pcrel_offset */

  HOWTO (R_860_SPLIT1,	        /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_860_SPLIT1",	/* name */
	 false,			/* partial_inplace */
	 0x1f07fe,		/* src_mask */
	 0x1f07fe,		/* dst_mask */
	 false),	        /* pcrel_offset */

  HOWTO (R_860_LOW2,	        /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_860_LOW2",		/* name */
	 false,			/* partial_inplace */
	 0xfffc,		/* src_mask */
	 0xfffc,		/* dst_mask */
	 false),	        /* pcrel_offset */

  HOWTO (R_860_SPLIT2,	        /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_860_SPLIT2",	/* name */
	 false,			/* partial_inplace */
	 0x1f07fc,		/* src_mask */
	 0x1f07fc,		/* dst_mask */
	 false),	        /* pcrel_offset */

  HOWTO (R_860_LOW3,	        /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_860_LOW3",		/* name */
	 false,			/* partial_inplace */
	 0xfff8,		/* src_mask */
	 0xfff8,		/* dst_mask */
	 false),	        /* pcrel_offset */

  HOWTO (R_860_LOGOT0,	        /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_860_LOGOT0",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff,		/* dst_mask */
	 true),		        /* pcrel_offset */

  HOWTO (R_860_SPGOT0,	        /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_860_SPGOT0",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff,		/* dst_mask */
	 true),		        /* pcrel_offset */

  HOWTO (R_860_LOGOT1,	        /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_860_LOGOT1",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff,		/* dst_mask */
	 true),		        /* pcrel_offset */

  HOWTO (R_860_SPGOT1,	        /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_860_SPGOT1",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff,		/* dst_mask */
	 true),		        /* pcrel_offset */

  HOWTO (R_860_LOGOTOFF0,        /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_860_LOGOTOFF0",	/* name */
	 true,			/* partial_inplace */
	 0xffffffff,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 false),	        /* pcrel_offset */

  HOWTO (R_860_SPGOTOFF0,        /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_860_SPGOTOFF0",	/* name */
	 true,			/* partial_inplace */
	 0xffffffff,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 false),	        /* pcrel_offset */

  HOWTO (R_860_LOGOTOFF1,        /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_860_LOGOTOFF1",	/* name */
	 true,			/* partial_inplace */
	 0xffffffff,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 false),	        /* pcrel_offset */

  HOWTO (R_860_SPGOTOFF1,       /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_860_SPGOTOFF1",	/* name */
	 true,			/* partial_inplace */
	 0xffffffff,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 false),	        /* pcrel_offset */

  HOWTO (R_860_LOGOTOFF2,        /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_860_LOGOTOFF2",	/* name */
	 true,			/* partial_inplace */
	 0xffffffff,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 false),	        /* pcrel_offset */

  HOWTO (R_860_LOGOTOFF3,        /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_860_LOGOTOFF3",	/* name */
	 true,			/* partial_inplace */
	 0xffffffff,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 false),	        /* pcrel_offset */

  HOWTO (R_860_LOPC,	        /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_860_LOPC",		/* name */
	 false,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 true),		        /* pcrel_offset */

  HOWTO (R_860_HIGHADJ,	        /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_860_HIGHADJ",	/* name */
	 false,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 false),	        /* pcrel_offset */

  HOWTO (R_860_HAGOT,	        /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_860_HAGOT",		/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff,		/* dst_mask */
	 true),		        /* pcrel_offset */

  HOWTO (R_860_HAGOTOFF,        /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_860_HAGOTOFF",	/* name */
	 true,			/* partial_inplace */
	 0xffffffff,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 false),	        /* pcrel_offset */

  HOWTO (R_860_HAPC,	        /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_860_HAPC",		/* name */
	 false,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 true),		        /* pcrel_offset */

  HOWTO (R_860_HIGH,	        /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_860_HIGH",		/* name */
	 false,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 false),	        /* pcrel_offset */

  HOWTO (R_860_HIGOT,	        /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_860_HIGOT",		/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff,		/* dst_mask */
	 true),		        /* pcrel_offset */

  HOWTO (R_860_HIGOTOFF,        /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_860_HIGOTOFF",	/* name */
	 true,			/* partial_inplace */
	 0xffffffff,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 false),	        /* pcrel_offset */
};


static unsigned char elf_code_to_howto_index[R_860_max + 1];

static reloc_howto_type *
lookup_howto (rtype)
     unsigned int rtype;
{
  static int initialized = 0;
  int i;
  int howto_tbl_size = (int) (sizeof (elf32_i860_howto_table)
			/ sizeof (elf32_i860_howto_table[0]));

  if (! initialized)
    {
      initialized = 1;
      memset (elf_code_to_howto_index, 0xff,
	      sizeof (elf_code_to_howto_index));
      for (i = 0; i < howto_tbl_size; i++)
        elf_code_to_howto_index[elf32_i860_howto_table[i].type] = i;
    }

  BFD_ASSERT (rtype <= R_860_max);
  i = elf_code_to_howto_index[rtype];
  if (i >= howto_tbl_size)
    return 0;
  return elf32_i860_howto_table + i;
}


/* Given a BFD reloc, return the matching HOWTO structure.  */
static reloc_howto_type *
elf32_i860_reloc_type_lookup (abfd, code)
     bfd * abfd ATTRIBUTE_UNUSED;
     bfd_reloc_code_real_type code;
{
  unsigned int rtype;

  switch (code)
    {
    case BFD_RELOC_NONE:
      rtype = R_860_NONE;
      break;
    case BFD_RELOC_32:
      rtype = R_860_32;
      break;
    case BFD_RELOC_860_COPY:
      rtype = R_860_COPY;
      break;
    case BFD_RELOC_860_GLOB_DAT:
      rtype = R_860_GLOB_DAT; 
      break;
    case BFD_RELOC_860_JUMP_SLOT:
      rtype = R_860_JUMP_SLOT;
      break;
    case BFD_RELOC_860_RELATIVE:
      rtype = R_860_RELATIVE;
      break;
    case BFD_RELOC_860_PC26:
      rtype = R_860_PC26;
      break;
    case BFD_RELOC_860_PLT26:
      rtype = R_860_PLT26;
      break;
    case BFD_RELOC_860_PC16:
      rtype = R_860_PC16;
      break;
    case BFD_RELOC_860_LOW0:
      rtype = R_860_LOW0;
      break;
    case BFD_RELOC_860_SPLIT0:
      rtype = R_860_SPLIT0;
      break;
    case BFD_RELOC_860_LOW1:
      rtype = R_860_LOW1;
      break;
    case BFD_RELOC_860_SPLIT1:
      rtype = R_860_SPLIT1;
      break;
    case BFD_RELOC_860_LOW2:
      rtype = R_860_LOW2;
      break;
    case BFD_RELOC_860_SPLIT2:
      rtype = R_860_SPLIT2;
      break;
    case BFD_RELOC_860_LOW3:
      rtype = R_860_LOW3;
      break;
    case BFD_RELOC_860_LOGOT0:
      rtype = R_860_LOGOT0;
      break;
    case BFD_RELOC_860_SPGOT0:
      rtype = R_860_SPGOT0;
      break;
    case BFD_RELOC_860_LOGOT1:
      rtype = R_860_LOGOT1;
      break;
    case BFD_RELOC_860_SPGOT1:
      rtype = R_860_SPGOT1;
      break;
    case BFD_RELOC_860_LOGOTOFF0:
      rtype = R_860_LOGOTOFF0;
      break;
    case BFD_RELOC_860_SPGOTOFF0:
      rtype = R_860_SPGOTOFF0;
      break;
    case BFD_RELOC_860_LOGOTOFF1:
      rtype = R_860_LOGOTOFF1;
      break;
    case BFD_RELOC_860_SPGOTOFF1:
      rtype = R_860_SPGOTOFF1;
      break;
    case BFD_RELOC_860_LOGOTOFF2:
      rtype = R_860_LOGOTOFF2;
      break;
    case BFD_RELOC_860_LOGOTOFF3:
      rtype = R_860_LOGOTOFF3;
      break;
    case BFD_RELOC_860_LOPC:
      rtype = R_860_LOPC;
      break;
    case BFD_RELOC_860_HIGHADJ:
      rtype = R_860_HIGHADJ;
      break;
    case BFD_RELOC_860_HAGOT:
      rtype = R_860_HAGOT;
      break;
    case BFD_RELOC_860_HAGOTOFF:
      rtype = R_860_HAGOTOFF;
      break;
    case BFD_RELOC_860_HAPC:
      rtype = R_860_HAPC;
      break;
    case BFD_RELOC_860_HIGH:
      rtype = R_860_HIGH;
      break;
    case BFD_RELOC_860_HIGOT:
      rtype = R_860_HIGOT;
      break;
    case BFD_RELOC_860_HIGOTOFF:
      rtype = R_860_HIGOTOFF;
      break;
    default:
      rtype = 0;
      break;
    }
  return lookup_howto (rtype);
}


/* Given a ELF reloc, return the matching HOWTO structure.  */
static void
elf32_i860_info_to_howto_rela (abfd, bfd_reloc, elf_reloc)
     bfd *abfd ATTRIBUTE_UNUSED;
     arelent *bfd_reloc;
     Elf64_Internal_Rela *elf_reloc;
{
  bfd_reloc->howto = lookup_howto (ELF32_R_TYPE (elf_reloc->r_info));
}



#define TARGET_BIG_SYM		bfd_elf32_i860_vec
#define TARGET_BIG_NAME		"elf32-i860"
#define TARGET_LITTLE_SYM	bfd_elf32_i860_little_vec
#define TARGET_LITTLE_NAME	"elf32-i860-little"
#define ELF_ARCH		bfd_arch_i860
#define ELF_MACHINE_CODE	EM_860
#define ELF_MAXPAGESIZE		4096


#define elf_info_to_howto			elf32_i860_info_to_howto_rela
#define bfd_elf32_bfd_reloc_type_lookup		elf32_i860_reloc_type_lookup

#include "elf32-target.h"
