/* V850-specific support for 32-bit ELF
   Copyright (C) 1996, 1997 Free Software Foundation, Inc.

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



/* XXX FIXME: This code is littered with 32bit int, 16bit short, 8bit char
   dependencies.  As is the gas & simulator code or the v850.  */


#include "bfd.h"
#include "sysdep.h"
#include "bfdlink.h"
#include "libbfd.h"
#include "elf-bfd.h"
#include "elf/v850.h"

static reloc_howto_type *v850_elf_reloc_type_lookup
  PARAMS ((bfd *abfd, bfd_reloc_code_real_type code));
static void v850_elf_info_to_howto_rel
  PARAMS ((bfd *, arelent *, Elf32_Internal_Rel *));
static bfd_reloc_status_type v850_elf_reloc
  PARAMS ((bfd *, arelent *, asymbol *, PTR, asection *, bfd *, char **));
static boolean v850_elf_is_local_label_name PARAMS ((bfd *, const char *));
static boolean v850_elf_relocate_section PARAMS((bfd *,
						 struct bfd_link_info *,
						 bfd *,
						 asection *,
						 bfd_byte *,
						 Elf_Internal_Rela *,
						 Elf_Internal_Sym *,
						 asection **));
/* Try to minimize the amount of space occupied by relocation tables
   on the ROM (not that the ROM won't be swamped by other ELF overhead).  */
#define USE_REL

/* Note: It is REQUIRED that the 'type' value of each entry in this array
   match the index of the entry in the array.  */
static reloc_howto_type v850_elf_howto_table[] =
{
  /* This reloc does nothing.  */
  HOWTO (R_V850_NONE,			/* type */
	 0,				/* rightshift */
	 2,				/* size (0 = byte, 1 = short, 2 = long) */
	 32,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_bitfield,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_V850_NONE",			/* name */
	 false,				/* partial_inplace */
	 0,				/* src_mask */
	 0,				/* dst_mask */
	 false),			/* pcrel_offset */

  /* A PC relative 9 bit branch. */
  HOWTO (R_V850_9_PCREL,		/* type */
	 2,				/* rightshift */
	 2,				/* size (0 = byte, 1 = short, 2 = long) */
	 26,				/* bitsize */
	 true,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_bitfield,	/* complain_on_overflow */
	 v850_elf_reloc,		/* special_function */
	 "R_V850_9_PCREL",		/* name */
	 false,				/* partial_inplace */
	 0x00ffffff,			/* src_mask */
	 0x00ffffff,			/* dst_mask */
	 true),				/* pcrel_offset */

  /* A PC relative 22 bit branch. */
  HOWTO (R_V850_22_PCREL,		/* type */
	 2,				/* rightshift */
	 2,				/* size (0 = byte, 1 = short, 2 = long) */
	 22,				/* bitsize */
	 true,				/* pc_relative */
	 7,				/* bitpos */
	 complain_overflow_signed,	/* complain_on_overflow */
	 v850_elf_reloc,		/* special_function */
	 "R_V850_22_PCREL",		/* name */
	 false,				/* partial_inplace */
	 0x07ffff80,			/* src_mask */
	 0x07ffff80,			/* dst_mask */
	 true),				/* pcrel_offset */

  /* High 16 bits of symbol value.  */
  HOWTO (R_V850_HI16_S,			/* type */
	 0,				/* rightshift */
	 1,				/* size (0 = byte, 1 = short, 2 = long) */
	 16,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 v850_elf_reloc,		/* special_function */
	 "R_V850_HI16_S",		/* name */
	 true,				/* partial_inplace */
	 0xffff,			/* src_mask */
	 0xffff,			/* dst_mask */
	 false),			/* pcrel_offset */

  /* High 16 bits of symbol value.  */
  HOWTO (R_V850_HI16,			/* type */
	 0,				/* rightshift */
	 1,				/* size (0 = byte, 1 = short, 2 = long) */
	 16,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 v850_elf_reloc,		/* special_function */
	 "R_V850_HI16",			/* name */
	 true,				/* partial_inplace */
	 0xffff,			/* src_mask */
	 0xffff,			/* dst_mask */
	 false),			/* pcrel_offset */

  /* Low 16 bits of symbol value.  */
  HOWTO (R_V850_LO16,			/* type */
	 0,				/* rightshift */
	 1,				/* size (0 = byte, 1 = short, 2 = long) */
	 16,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 v850_elf_reloc,		/* special_function */
	 "R_V850_LO16",			/* name */
	 true,				/* partial_inplace */
	 0xffff,			/* src_mask */
	 0xffff,			/* dst_mask */
	 false),			/* pcrel_offset */

  /* Simple 32bit reloc.  */
  HOWTO (R_V850_32,			/* type */
	 0,				/* rightshift */
	 2,				/* size (0 = byte, 1 = short, 2 = long) */
	 32,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_V850_32",			/* name */
	 true,				/* partial_inplace */
	 0xffffffff,			/* src_mask */
	 0xffffffff,			/* dst_mask */
	 false),			/* pcrel_offset */

  /* Simple 16bit reloc.  */
  HOWTO (R_V850_16,			/* type */
	 0,				/* rightshift */
	 1,				/* size (0 = byte, 1 = short, 2 = long) */
	 16,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_V850_16",			/* name */
	 true,				/* partial_inplace */
	 0xffff,			/* src_mask */
	 0xffff,			/* dst_mask */
	 false),			/* pcrel_offset */

  /* Simple 8bit reloc.	 */
  HOWTO (R_V850_8,			/* type */
	 0,				/* rightshift */
	 0,				/* size (0 = byte, 1 = short, 2 = long) */
	 8,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,		/* special_function */
	 "R_V850_8",			/* name */
	 true,				/* partial_inplace */
	 0xff,				/* src_mask */
	 0xff,				/* dst_mask */
	 false),			/* pcrel_offset */

  /* 16 bit offset from the short data area pointer.  */
  HOWTO (R_V850_SDA_16_16_OFFSET,	/* type */
	 0,				/* rightshift */
	 1,				/* size (0 = byte, 1 = short, 2 = long) */
	 16,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 v850_elf_reloc,		/* special_function */
	 "R_V850_SDA_16_16_OFFSET",	/* name */
	 false,				/* partial_inplace */
	 0xffff,			/* src_mask */
	 0xffff,			/* dst_mask */
	 false),			/* pcrel_offset */

  /* 15 bit offset from the short data area pointer.  */
  HOWTO (R_V850_SDA_15_16_OFFSET,	/* type */
	 1,				/* rightshift */
	 1,				/* size (0 = byte, 1 = short, 2 = long) */
	 16,				/* bitsize */
	 false,				/* pc_relative */
	 1,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 v850_elf_reloc,		/* special_function */
	 "R_V850_SDA_15_16_OFFSET",	/* name */
	 false,				/* partial_inplace */
	 0xfffe,			/* src_mask */
	 0xfffe,			/* dst_mask */
	 false),			/* pcrel_offset */

  /* 16 bit offset from the zero data area pointer.  */
  HOWTO (R_V850_ZDA_16_16_OFFSET,	/* type */
	 0,				/* rightshift */
	 1,				/* size (0 = byte, 1 = short, 2 = long) */
	 16,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 v850_elf_reloc,		/* special_function */
	 "R_V850_ZDA_16_16_OFFSET",	/* name */
	 false,				/* partial_inplace */
	 0xffff,			/* src_mask */
	 0xffff,			/* dst_mask */
	 false),			/* pcrel_offset */

  /* 15 bit offset from the zero data area pointer.  */
  HOWTO (R_V850_ZDA_15_16_OFFSET,	/* type */
	 1,				/* rightshift */
	 1,				/* size (0 = byte, 1 = short, 2 = long) */
	 16,				/* bitsize */
	 false,				/* pc_relative */
	 1,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 v850_elf_reloc,		/* special_function */
	 "R_V850_ZDA_15_16_OFFSET",	/* name */
	 false,				/* partial_inplace */
	 0xfffe,			/* src_mask */
	 0xfffe,			/* dst_mask */
	 false),			/* pcrel_offset */

  /* 6 bit offset from the tiny data area pointer.  */
  HOWTO (R_V850_TDA_6_8_OFFSET,		/* type */
	 2,				/* rightshift */
	 1,				/* size (0 = byte, 1 = short, 2 = long) */
	 8,				/* bitsize */
	 false,				/* pc_relative */
	 1,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 v850_elf_reloc,		/* special_function */
	 "R_V850_TDA_6_8_OFFSET",	/* name */
	 false,				/* partial_inplace */
	 0x7e,				/* src_mask */
	 0x7e,				/* dst_mask */
	 false),			/* pcrel_offset */

  /* 8 bit offset from the tiny data area pointer.  */
  HOWTO (R_V850_TDA_7_8_OFFSET,		/* type */
	 1,				/* rightshift */
	 1,				/* size (0 = byte, 1 = short, 2 = long) */
	 8,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 v850_elf_reloc,		/* special_function */
	 "R_V850_TDA_7_8_OFFSET",	/* name */
	 false,				/* partial_inplace */
	 0x7f,				/* src_mask */
	 0x7f,				/* dst_mask */
	 false),			/* pcrel_offset */
  
  /* 7 bit offset from the tiny data area pointer.  */
  HOWTO (R_V850_TDA_7_7_OFFSET,		/* type */
	 0,				/* rightshift */
	 1,				/* size (0 = byte, 1 = short, 2 = long) */
	 7,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 v850_elf_reloc,		/* special_function */
	 "R_V850_TDA_7_7_OFFSET",	/* name */
	 false,				/* partial_inplace */
	 0x7f,				/* src_mask */
	 0x7f,				/* dst_mask */
	 false),			/* pcrel_offset */

  /* 16 bit offset from the tiny data area pointer!  */
  HOWTO (R_V850_TDA_16_16_OFFSET,	/* type */
	 0,				/* rightshift */
	 1,				/* size (0 = byte, 1 = short, 2 = long) */
	 16,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 v850_elf_reloc,		/* special_function */
	 "R_V850_TDA_16_16_OFFSET",	/* name */
	 false,				/* partial_inplace */
	 0xffff,			/* src_mask */
	 0xfff,				/* dst_mask */
	 false),			/* pcrel_offset */

/* start-sanitize-v850e */
  
  /* 5 bit offset from the tiny data area pointer.  */
  HOWTO (R_V850_TDA_4_5_OFFSET,		/* type */
	 1,				/* rightshift */
	 1,				/* size (0 = byte, 1 = short, 2 = long) */
	 5,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 v850_elf_reloc,		/* special_function */
	 "R_V850_TDA_4_5_OFFSET",	/* name */
	 false,				/* partial_inplace */
	 0x0f,				/* src_mask */
	 0x0f,				/* dst_mask */
	 false),			/* pcrel_offset */

  /* 4 bit offset from the tiny data area pointer.  */
  HOWTO (R_V850_TDA_4_4_OFFSET,		/* type */
	 0,				/* rightshift */
	 1,				/* size (0 = byte, 1 = short, 2 = long) */
	 4,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 v850_elf_reloc,		/* special_function */
	 "R_V850_TDA_4_4_OFFSET",	/* name */
	 false,				/* partial_inplace */
	 0x0f,				/* src_mask */
	 0x0f,				/* dst_mask */
	 false),			/* pcrel_offset */

  /* 16 bit offset from the short data area pointer.  */
  HOWTO (R_V850_SDA_16_16_SPLIT_OFFSET,	/* type */
	 0,				/* rightshift */
	 2,				/* size (0 = byte, 1 = short, 2 = long) */
	 16,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 v850_elf_reloc,		/* special_function */
	 "R_V850_SDA_16_16_SPLIT_OFFSET",/* name */
	 false,				/* partial_inplace */
	 0xfffe0020,			/* src_mask */
	 0xfffe0020,			/* dst_mask */
	 false),			/* pcrel_offset */

  /* 16 bit offset from the zero data area pointer.  */
  HOWTO (R_V850_ZDA_16_16_SPLIT_OFFSET,	/* type */
	 0,				/* rightshift */
	 2,				/* size (0 = byte, 1 = short, 2 = long) */
	 16,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 v850_elf_reloc,		/* special_function */
	 "R_V850_ZDA_16_16_SPLIT_OFFSET",/* name */
	 false,				/* partial_inplace */
	 0xfffe0020,			/* src_mask */
	 0xfffe0020,			/* dst_mask */
	 false),			/* pcrel_offset */

  /* 6 bit offset from the call table base pointer.  */
  HOWTO (R_V850_CALLT_6_7_OFFSET,	/* type */
	 0,				/* rightshift */
	 1,				/* size (0 = byte, 1 = short, 2 = long) */
	 7,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 v850_elf_reloc,		/* special_function */
	 "R_V850_CALLT_6_7_OFFSET",	/* name */
	 false,				/* partial_inplace */
	 0x3f,				/* src_mask */
	 0x3f,				/* dst_mask */
	 false),			/* pcrel_offset */

  /* 16 bit offset from the call table base pointer.  */
  HOWTO (R_V850_CALLT_16_16_OFFSET,	/* type */
	 0,				/* rightshift */
	 1,				/* size (0 = byte, 1 = short, 2 = long) */
	 16,				/* bitsize */
	 false,				/* pc_relative */
	 0,				/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 v850_elf_reloc,		/* special_function */
	 "R_V850_CALLT_16_16_OFFSET",	/* name */
	 false,				/* partial_inplace */
	 0xffff,			/* src_mask */
	 0xffff,			/* dst_mask */
	 false),			/* pcrel_offset */

/* end-sanitize-v850e */
};

/* Map BFD reloc types to V850 ELF reloc types.  */

struct v850_elf_reloc_map
{
  unsigned char bfd_reloc_val;
  unsigned char elf_reloc_val;
};

static const struct v850_elf_reloc_map v850_elf_reloc_map[] =
{
  { BFD_RELOC_NONE,		R_V850_NONE },
  { BFD_RELOC_V850_9_PCREL,	R_V850_9_PCREL },
  { BFD_RELOC_V850_22_PCREL,	R_V850_22_PCREL },
  { BFD_RELOC_HI16_S,		R_V850_HI16_S },
  { BFD_RELOC_HI16,		R_V850_HI16 },
  { BFD_RELOC_LO16,		R_V850_LO16 },
  { BFD_RELOC_32,		R_V850_32 },
  { BFD_RELOC_16,		R_V850_16 },
  { BFD_RELOC_8,		R_V850_8 },
  { BFD_RELOC_V850_SDA_16_16_OFFSET, R_V850_SDA_16_16_OFFSET },
  { BFD_RELOC_V850_SDA_15_16_OFFSET, R_V850_SDA_15_16_OFFSET },
  { BFD_RELOC_V850_ZDA_16_16_OFFSET, R_V850_ZDA_16_16_OFFSET },
  { BFD_RELOC_V850_ZDA_15_16_OFFSET, R_V850_ZDA_15_16_OFFSET },
  { BFD_RELOC_V850_TDA_6_8_OFFSET,   R_V850_TDA_6_8_OFFSET   },
  { BFD_RELOC_V850_TDA_7_8_OFFSET,   R_V850_TDA_7_8_OFFSET   },
  { BFD_RELOC_V850_TDA_7_7_OFFSET,   R_V850_TDA_7_7_OFFSET   },
  { BFD_RELOC_V850_TDA_16_16_OFFSET, R_V850_TDA_16_16_OFFSET },
/* start-sanitize-v850e */
  { BFD_RELOC_V850_TDA_4_5_OFFSET,         R_V850_TDA_4_5_OFFSET         },
  { BFD_RELOC_V850_TDA_4_4_OFFSET,         R_V850_TDA_4_4_OFFSET         },
  { BFD_RELOC_V850_SDA_16_16_SPLIT_OFFSET, R_V850_SDA_16_16_SPLIT_OFFSET },
  { BFD_RELOC_V850_ZDA_16_16_SPLIT_OFFSET, R_V850_ZDA_16_16_SPLIT_OFFSET },
  { BFD_RELOC_V850_CALLT_6_7_OFFSET,       R_V850_CALLT_6_7_OFFSET       },
  { BFD_RELOC_V850_CALLT_16_16_OFFSET,     R_V850_CALLT_16_16_OFFSET     },
/* end-sanitize-v850e */
};


/* Map a bfd relocation into the appropriate howto structure */
static reloc_howto_type *
v850_elf_reloc_type_lookup (abfd, code)
     bfd *                     abfd;
     bfd_reloc_code_real_type  code;
{
  unsigned int i;

  for (i = 0;
       i < sizeof (v850_elf_reloc_map) / sizeof (struct v850_elf_reloc_map);
       i++)
    {
      if (v850_elf_reloc_map[i].bfd_reloc_val == code)
	{
	  BFD_ASSERT (v850_elf_howto_table[v850_elf_reloc_map[i].elf_reloc_val].type == v850_elf_reloc_map[i].elf_reloc_val);
	  
	  return & v850_elf_howto_table[v850_elf_reloc_map[i].elf_reloc_val];
	}
    }

  return NULL;
}


/* Set the howto pointer for an V850 ELF reloc.  */
static void
v850_elf_info_to_howto_rel (abfd, cache_ptr, dst)
     bfd *                 abfd;
     arelent *             cache_ptr;
     Elf32_Internal_Rel *  dst;
{
  unsigned int r_type;

  r_type = ELF32_R_TYPE (dst->r_info);
  BFD_ASSERT (r_type < (unsigned int) R_V850_max);
  cache_ptr->howto = &v850_elf_howto_table[r_type];
}


/* Look through the relocs for a section during the first phase, and
   allocate space in the global offset table or procedure linkage
   table.  */

static boolean
v850_elf_check_relocs (abfd, info, sec, relocs)
     bfd *                      abfd;
     struct bfd_link_info *     info;
     asection *                 sec;
     const Elf_Internal_Rela *  relocs;
{
  boolean ret = true;
  bfd *dynobj;
  Elf_Internal_Shdr *symtab_hdr;
  struct elf_link_hash_entry **sym_hashes;
  const Elf_Internal_Rela *rel;
  const Elf_Internal_Rela *rel_end;
  asection *sreloc;
  enum reloc_type r_type;
  int other = 0;
  const char *common = (const char *)0;

  if (info->relocateable)
    return true;

#ifdef DEBUG
  fprintf (stderr, "v850_elf_check_relocs called for section %s in %s\n",
	   bfd_get_section_name (abfd, sec),
	   bfd_get_filename (abfd));
#endif

  dynobj = elf_hash_table (info)->dynobj;
  symtab_hdr = &elf_tdata (abfd)->symtab_hdr;
  sym_hashes = elf_sym_hashes (abfd);
  sreloc = NULL;

  rel_end = relocs + sec->reloc_count;
  for (rel = relocs; rel < rel_end; rel++)
    {
      unsigned long r_symndx;
      struct elf_link_hash_entry *h;

      r_symndx = ELF32_R_SYM (rel->r_info);
      if (r_symndx < symtab_hdr->sh_info)
	h = NULL;
      else
	h = sym_hashes[r_symndx - symtab_hdr->sh_info];

      r_type = (enum reloc_type) ELF32_R_TYPE (rel->r_info);
      switch (r_type)
	{
	default:
	case R_V850_NONE:
	case R_V850_9_PCREL:
	case R_V850_22_PCREL:
	case R_V850_HI16_S:
	case R_V850_HI16:
	case R_V850_LO16:
	case R_V850_32:
	case R_V850_16:
	case R_V850_8:
/* start-sanitize-v850e */
	case R_V850_CALLT_6_7_OFFSET:
	case R_V850_CALLT_16_16_OFFSET:
/* end-sanitize-v850e */
	  break;

/* start-sanitize-v850e */
	case R_V850_SDA_16_16_SPLIT_OFFSET:
/* end-sanitize-v850e */
	case R_V850_SDA_16_16_OFFSET:
	case R_V850_SDA_15_16_OFFSET:
	  other = V850_OTHER_SDA;
	  common = ".scommon";
	  goto small_data_common;
	  
/* start-sanitize-v850e */
	case R_V850_ZDA_16_16_SPLIT_OFFSET:
/* end-sanitize-v850e */
	case R_V850_ZDA_16_16_OFFSET:
	case R_V850_ZDA_15_16_OFFSET:
	  other = V850_OTHER_ZDA;
	  common = ".zcommon";
	  goto small_data_common;
	  
/* start-sanitize-v850e */
	case R_V850_TDA_4_5_OFFSET:
	case R_V850_TDA_4_4_OFFSET:
/* end-sanitize-v850e */	  
	case R_V850_TDA_6_8_OFFSET:
	case R_V850_TDA_7_8_OFFSET:
	case R_V850_TDA_7_7_OFFSET:
	case R_V850_TDA_16_16_OFFSET:
	  other = V850_OTHER_TDA;
	  common = ".tcommon";
	  /* fall through */

#define V850_OTHER_MASK (V850_OTHER_TDA | V850_OTHER_SDA | V850_OTHER_ZDA)

	small_data_common:
	  if (h)
	    {
	      h->other |= other;	/* flag which type of relocation was used */
	      if ((h->other & V850_OTHER_MASK) != (other & V850_OTHER_MASK)
		  && (h->other & V850_OTHER_ERROR) == 0)
		{
		  const char *msg;

		  switch (h->other & V850_OTHER_MASK)
		    {
		    default:
		      msg = "Variable cannot occupy in multiple small data regions";
		      break;
		    case V850_OTHER_SDA | V850_OTHER_ZDA | V850_OTHER_TDA:
		      msg = "Variable can only be in one of the small, zero, and tiny data regions";
		      break;
		    case V850_OTHER_SDA | V850_OTHER_ZDA:
		      msg = "Variable cannot be in both small and zero data regions simultaneously";
		      break;
		    case V850_OTHER_SDA | V850_OTHER_TDA:
		      msg = "Variable cannot be in both small and tiny data regions simultaneously";
		      break;
		    case V850_OTHER_ZDA | V850_OTHER_TDA:
		      msg = "Variable cannot be in both zero and tiny data regions simultaneously";
		      break;
		    }

		  (*info->callbacks->warning) (info, msg, h->root.root.string,
					       abfd, h->root.u.def.section, 0);

		  bfd_set_error (bfd_error_bad_value);
		  h->other |= V850_OTHER_ERROR;
		  ret = false;
		}
	    }

	  if (h && h->root.type == bfd_link_hash_common
	      && h->root.u.c.p
	      && !strcmp (bfd_get_section_name (abfd, h->root.u.c.p->section), "COMMON"))
	    {
	      asection *section = h->root.u.c.p->section = bfd_make_section_old_way (abfd, common);
	      section->flags |= SEC_IS_COMMON;
	    }

#ifdef DEBUG
	  fprintf (stderr, "v850_elf_check_relocs, found %s relocation for %s%s\n",
		   v850_elf_howto_table[ (int)r_type ].name,
		   (h && h->root.root.string) ? h->root.root.string : "<unknown>",
		   (h->root.type == bfd_link_hash_common) ? ", symbol is common" : "");
#endif
	  break;
	}
    }

  return ret;
}


/* Insert the addend into the instruction.  */
static bfd_reloc_status_type
v850_elf_reloc (abfd, reloc, symbol, data, isection, obfd, err)
     bfd *       abfd;
     arelent *   reloc;
     asymbol *   symbol;
     PTR         data;
     asection *  isection;
     bfd *       obfd;
     char **     err;
{
  long relocation;
  long insn;

  
  /* If there is an output BFD,
     and the symbol is not a section name (which is only defined at final link time),
     and either we are not putting the addend into the instruction
         or the addend is zero, so there is nothing to add into the instruction
     then just fixup the address and return.  */
  if (obfd != (bfd *) NULL
      && (symbol->flags & BSF_SECTION_SYM) == 0
      && (! reloc->howto->partial_inplace
	  || reloc->addend == 0))
    {
      reloc->address += isection->output_offset;
      return bfd_reloc_ok;
    }
#if 0  
  else if (obfd != NULL)
    {
      return bfd_reloc_continue;
    }
#endif
  
  /* Catch relocs involving undefined symbols.  */
  if (bfd_is_und_section (symbol->section)
      && (symbol->flags & BSF_WEAK) == 0
      && obfd == NULL)
    return bfd_reloc_undefined;

  /* We handle final linking of some relocs ourselves.  */

  /* Is the address of the relocation really within the section?  */
  if (reloc->address > isection->_cooked_size)
    return bfd_reloc_outofrange;
  
  /* Work out which section the relocation is targetted at and the
     initial relocation command value.  */
  
  /* Get symbol value.  (Common symbols are special.)  */
  if (bfd_is_com_section (symbol->section))
    relocation = 0;
  else
    relocation = symbol->value;
  
  /* Convert input-section-relative symbol value to absolute + addend.  */
  relocation += symbol->section->output_section->vma;
  relocation += symbol->section->output_offset;
  relocation += reloc->addend;
  
  if (reloc->howto->pc_relative == true)
    {
      /* Here the variable relocation holds the final address of the
	 symbol we are relocating against, plus any addend.  */
      relocation -= isection->output_section->vma + isection->output_offset;
      
      /* Deal with pcrel_offset */
      relocation -= reloc->address;
    }
  
  /* I've got no clue... */
  reloc->addend = 0;	
  
  switch (reloc->howto->type)
    {
    default:
      /* fprintf (stderr, "reloc type %d not SUPPORTED\n", reloc->howto->type ); */
      return bfd_reloc_notsupported;
      
    case R_V850_22_PCREL:
      if (relocation > 0x1ffff || relocation < -0x200000)
	return bfd_reloc_overflow;
      
      if ((relocation % 2) != 0)
	return bfd_reloc_dangerous;
      
      insn = bfd_get_32 (abfd, (bfd_byte *) data + reloc->address);
      insn &= ~0xfffe003f;
      insn |= (((relocation & 0xfffe) << 16)
	       | ((relocation & 0x3f0000) >> 16));
      bfd_put_32 (abfd, insn, (bfd_byte *)data + reloc->address);
      return bfd_reloc_ok;
      
    case R_V850_9_PCREL:
      if (relocation > 0xff || relocation < -0x100)
	return bfd_reloc_overflow;
      
      if ((relocation % 2) != 0)
	return bfd_reloc_dangerous;
      
      insn = bfd_get_16 (abfd, (bfd_byte *) data + reloc->address);
      insn &= ~ 0xf870;
      insn |= ((relocation & 0x1f0) << 7) | ((relocation & 0x0e) << 3);
      bfd_put_16 (abfd, insn, (bfd_byte *)data + reloc->address);
      return bfd_reloc_ok;
      
    case R_V850_HI16_S:
      relocation += bfd_get_16 (abfd, (bfd_byte *) data + reloc->address);
      relocation = (relocation >> 16) + ((relocation & 0x8000) != 0);
      bfd_put_16 (abfd, relocation, (bfd_byte *)data + reloc->address);
      return bfd_reloc_ok;
      
    case R_V850_HI16:
      relocation += bfd_get_16 (abfd, (bfd_byte *) data + reloc->address);
      relocation = (relocation >> 16);
      bfd_put_16 (abfd, relocation, (bfd_byte *)data + reloc->address);
      return bfd_reloc_ok;
      
    case R_V850_LO16:
      relocation += (short)bfd_get_16 (abfd, (bfd_byte *) data + reloc->address);
      /* Do not complain if value has top bit set, as this has been anticipated.  */
      bfd_put_16 (abfd, relocation, (bfd_byte *)data + reloc->address);
      return bfd_reloc_ok;

    case R_V850_16:
      relocation += (short)bfd_get_16 (abfd, (bfd_byte *) data + reloc->address);
      /* drop through */
      
    case R_V850_SDA_16_16_OFFSET:
    case R_V850_ZDA_16_16_OFFSET:
    case R_V850_TDA_16_16_OFFSET:
      if ((long)relocation > 0x7fff || (long)relocation < -0x8000)
	return bfd_reloc_overflow;
      bfd_put_16 (abfd, relocation, (bfd_byte *)data + reloc->address);
      return bfd_reloc_ok;
      
    case R_V850_SDA_15_16_OFFSET:
    case R_V850_ZDA_15_16_OFFSET:
      if ((long)relocation > 0x7ffe || (long)relocation < -0x8000)
	return bfd_reloc_overflow;
      
      if (relocation & 1)
	return bfd_reloc_dangerous;
      
      insn  = bfd_get_16 (abfd, (bfd_byte *) data + reloc->address);
      insn &= 1;
      insn |= (relocation >> 1) & ~1;
      
      bfd_put_16 (abfd, insn, (bfd_byte *)data + reloc->address);
      return bfd_reloc_ok;
      
    case R_V850_TDA_6_8_OFFSET:
      if ((long) relocation > 0xfc || (long) relocation < 0)
	return bfd_reloc_overflow;
      
      if (relocation & 3)
	return bfd_reloc_dangerous;
      
      insn  = bfd_get_16 (abfd, (bfd_byte *) data + reloc->address);
      insn &= 0xff81;
      insn |= (relocation >> 1);
      
      bfd_put_16 (abfd, insn, (bfd_byte *)data + reloc->address);
      return bfd_reloc_ok;
      
    case R_V850_TDA_7_8_OFFSET:
      if ((long) relocation > 0xfe || (long) relocation < 0)
	return bfd_reloc_overflow;
      
      if (relocation & 1)
	return bfd_reloc_dangerous;
      
      insn  = bfd_get_16 (abfd, (bfd_byte *) data + reloc->address);
      insn &= 0xff80;
      insn |= (relocation >> 1);
      
      bfd_put_16 (abfd, insn, (bfd_byte *)data + reloc->address);
      return bfd_reloc_ok;
      
    case R_V850_TDA_7_7_OFFSET:
      if ((long) relocation > 0x7f || (long) relocation < 0)
	return bfd_reloc_overflow;
      
      insn  = bfd_get_16 (abfd, (bfd_byte *) data + reloc->address);
      insn &= 0xff80;
      insn |= relocation;
      
      bfd_put_16 (abfd, insn, (bfd_byte *)data + reloc->address);
      return bfd_reloc_ok;
      
/* start-sanitize-v850e */
    case R_V850_TDA_4_5_OFFSET:
      if ((long) relocation > 0x1e || (long) relocation < 0)
	return bfd_reloc_overflow;
      
      if (relocation & 1)
	return bfd_reloc_dangerous;
      
      insn  = bfd_get_16 (abfd, (bfd_byte *) data + reloc->address);
      insn &= 0xfff0;
      insn |= (relocation >> 1);
      
      bfd_put_16 (abfd, insn, (bfd_byte *)data + reloc->address);
      return bfd_reloc_ok;
      
    case R_V850_TDA_4_4_OFFSET:
      if ((long) relocation > 0xf || (long) relocation < 0)
	return bfd_reloc_overflow;
      
      insn  = bfd_get_16 (abfd, (bfd_byte *) data + reloc->address);
      insn &= 0xfff0;
      insn |= relocation;
      
      bfd_put_16 (abfd, insn, (bfd_byte *)data + reloc->address);
      return bfd_reloc_ok;
      
    case R_V850_ZDA_16_16_SPLIT_OFFSET:
    case R_V850_SDA_16_16_SPLIT_OFFSET:
      if ((long) relocation > 0xffff || (long) relocation < 0)
	return bfd_reloc_overflow;
      
      insn  = bfd_get_32 (abfd, (bfd_byte *) data + reloc->address);
      
      insn &= 0x0001ffdf;
      insn |= (relocation & 1) << 5;
      insn |= (relocation & ~1) << 16;
      
      bfd_put_32 (abfd, insn, (bfd_byte *)data + reloc->address);
      return bfd_reloc_ok;
      
    case R_V850_CALLT_6_7_OFFSET:
      if ((long) relocation > 0x7e || (long) relocation < 0)
	return bfd_reloc_overflow;
      
      if (relocation & 1)
	return bfd_reloc_dangerous;
      
      insn  = bfd_get_16 (abfd, (bfd_byte *) data + reloc->address);
      insn &= 0xff80;
      insn |= (relocation >> 1);
      
      bfd_put_16 (abfd, insn, (bfd_byte *)data + reloc->address);
      return bfd_reloc_ok;
      
    case R_V850_CALLT_16_16_OFFSET:
      bfd_put_16 (abfd, relocation, (bfd_byte *)data + reloc->address);
      return bfd_reloc_ok;
      
/* end-sanitize-v850e */
    }

  return bfd_reloc_continue;
}


/*ARGSUSED*/
static boolean
v850_elf_is_local_label_name (abfd, name)
     bfd *         abfd;
     const char *  name;
{
  return ((name[0] == '.' && (name[1] == 'L' || name[1] == '.'))
	  || (name[0] == '_' && name[1] == '.' && name[2] == 'L'
	      && name[3] == '_'));
}


/* Perform a relocation as part of a final link.  */
static bfd_reloc_status_type
v850_elf_final_link_relocate (howto, input_bfd, output_bfd,
				    input_section, contents, offset, value,
				    addend, info, sym_sec, is_local)
     reloc_howto_type *      howto;
     bfd *                   input_bfd;
     bfd *                   output_bfd;
     asection *              input_section;
     bfd_byte *              contents;
     bfd_vma                 offset;
     bfd_vma                 value;
     bfd_vma                 addend;
     struct bfd_link_info *  info;
     asection *              sym_sec;
     int                     is_local;
{
  unsigned long  insn;
  unsigned long  r_type   = howto->type;
  bfd_byte *     hit_data = contents + offset;

  switch (r_type)
    {
    case R_V850_9_PCREL:
      value -= (input_section->output_section->vma
		+ input_section->output_offset);
      value -= offset;

      if ((long)value > 0xff || (long)value < -0x100)
	return bfd_reloc_overflow;

      if ((value % 2) != 0)
	return bfd_reloc_dangerous;

      insn = bfd_get_16 (input_bfd, hit_data);
      insn &= 0x078f;
      insn |= ((value & 0x1f0) << 7) | ((value & 0x0e) << 3);
      bfd_put_16 (input_bfd, insn, hit_data);
      return bfd_reloc_ok;
    
    case R_V850_22_PCREL:
      value -= (input_section->output_section->vma
		+ input_section->output_offset);
      value -= offset;
    
      if ((long)value > 0x1ffff || (long)value < -0x200000)
	return bfd_reloc_overflow;

      if ((value % 2) != 0)
	return bfd_reloc_dangerous;

      insn = bfd_get_32 (input_bfd, hit_data);
      insn &= 0x1ffc0;
      insn |= (((value & 0xfffe) << 16) | ((value & 0x3f0000) >> 16));
      bfd_put_32 (input_bfd, insn, hit_data);
      return bfd_reloc_ok;
      
    case R_V850_HI16_S:
      value += (short)bfd_get_16 (input_bfd, hit_data);
      value = (value >> 16) + ((value & 0x8000) != 0);

      if ((long)value > 0x7fff || (long)value < -0x8000)
	return bfd_reloc_overflow;

      bfd_put_16 (input_bfd, value, hit_data);
      return bfd_reloc_ok;

    case R_V850_HI16:
      value += (short)bfd_get_16 (input_bfd, hit_data);
      value >>= 16;

      if ((long)value > 0x7fff || (long)value < -0x8000)
	return bfd_reloc_overflow;

      bfd_put_16 (input_bfd, value, hit_data);
      return bfd_reloc_ok;

    case R_V850_LO16:
      value += (short)bfd_get_16 (input_bfd, hit_data);
      value &= 0xffff;

      bfd_put_16 (input_bfd, value, hit_data);
      return bfd_reloc_ok;

    case R_V850_16:
      value += (short) bfd_get_16 (input_bfd, hit_data);

      if ((long) value > 0x7fff || (long) value < -0x8000)
	return bfd_reloc_overflow;

      bfd_put_16 (input_bfd, value, hit_data);
      return bfd_reloc_ok;

    case R_V850_ZDA_16_16_OFFSET:
      if (sym_sec == NULL)
	return bfd_reloc_undefined;
	  
      value -= sym_sec->output_section->vma;
      value += (short) bfd_get_16 (input_bfd, hit_data);

      if ((long) value > 0x7fff || (long) value < -0x8000)
	return bfd_reloc_overflow;

      bfd_put_16 (input_bfd, value, hit_data);
      return bfd_reloc_ok;

    case R_V850_ZDA_15_16_OFFSET:	
      if (sym_sec == NULL)
	return bfd_reloc_undefined;
	  
      insn = bfd_get_16 (input_bfd, hit_data);

      value -= sym_sec->output_section->vma;
      value += ((insn & 0xfffe) << 1);
      
      if ((long) value > 0x7ffe || (long) value < -0x8000)
	return bfd_reloc_overflow;

      value &= ~1;
      value |= (insn & 1);
      
      bfd_put_16 (input_bfd, value, hit_data);
      return bfd_reloc_ok;

    case R_V850_32:
      value += bfd_get_32 (input_bfd, hit_data);
      bfd_put_32 (input_bfd, value, hit_data);
      return bfd_reloc_ok;

    case R_V850_8:
      value += (char)bfd_get_8 (input_bfd, hit_data);

      if ((long)value > 0x7f || (long)value < -0x80)
	return bfd_reloc_overflow;

      bfd_put_8 (input_bfd, value, hit_data);
      return bfd_reloc_ok;

    case R_V850_SDA_16_16_OFFSET:
      if (sym_sec == NULL)
	return bfd_reloc_undefined;
	  
      {
	unsigned long                gp;
	struct bfd_link_hash_entry * h;

	/* Get the value of __gp.  */
	h = bfd_link_hash_lookup (info->hash, "__gp", false, false, true);
	if (h == (struct bfd_link_hash_entry *) NULL
	    || h->type != bfd_link_hash_defined)
	  return bfd_reloc_other;

	gp = (h->u.def.value
	      + h->u.def.section->output_section->vma
	      + h->u.def.section->output_offset);

	value -= sym_sec->output_section->vma;
	value -= (gp - sym_sec->output_section->vma);
	value += (short) bfd_get_16 (input_bfd, hit_data);

	if ((long)value > 0x7fff || (long)value < -0x8000)
	  return bfd_reloc_overflow;

	bfd_put_16 (input_bfd, value, hit_data);
	return bfd_reloc_ok;
      }

    case R_V850_SDA_15_16_OFFSET:
      if (sym_sec == NULL)
	return bfd_reloc_undefined;
	  
      {
	unsigned long                gp;
	struct bfd_link_hash_entry * h;

	/* Get the value of __gp.  */
	h = bfd_link_hash_lookup (info->hash, "__gp", false, false, true);
	if (h == (struct bfd_link_hash_entry *) NULL
	    || h->type != bfd_link_hash_defined)
	  return bfd_reloc_other;

	gp = (h->u.def.value
	      + h->u.def.section->output_section->vma
	      + h->u.def.section->output_offset);
	
	value -= sym_sec->output_section->vma;
	value -= (gp - sym_sec->output_section->vma);

	insn = bfd_get_16 (input_bfd, hit_data);
	
	value += ((insn & 0xfffe) << 1);
	
	if ((long)value > 0x7ffe || (long)value < -0x8000)
	  return bfd_reloc_overflow;

	value &= ~1;
	value |= (insn & 1);
	
	bfd_put_16 (input_bfd, value, hit_data);
	return bfd_reloc_ok;
      }

    case R_V850_TDA_6_8_OFFSET:
      {
	unsigned long                ep;
	struct bfd_link_hash_entry * h;
	
	insn = bfd_get_16 (input_bfd, hit_data);

	/* Get the value of __ep.  */
	h = bfd_link_hash_lookup (info->hash, "__ep", false, false, true);
	if (h == (struct bfd_link_hash_entry *) NULL
	    || h->type != bfd_link_hash_defined)
	  return bfd_reloc_continue;  /* Actually this indicates that __ep could not be found. */

	ep = (h->u.def.value
	      + h->u.def.section->output_section->vma
	      + h->u.def.section->output_offset);

	value -= ep;
	value += ((insn & 0x7e) << 2);
	
	if ((long) value > 0xfc || (long) value < 0)
	  return bfd_reloc_overflow;
	
	if ((value % 2) != 0)
	  return bfd_reloc_dangerous;
	
	insn &= 0xff81;
	insn |= (value >> 1);

	bfd_put_16 (input_bfd, insn, hit_data);
	return bfd_reloc_ok;
      }
    
    case R_V850_TDA_7_8_OFFSET:
      {
	unsigned long                ep;
	struct bfd_link_hash_entry * h;
	
	insn = bfd_get_16 (input_bfd, hit_data);

	/* Get the value of __ep.  */
	h = bfd_link_hash_lookup (info->hash, "__ep", false, false, true);
	if (h == (struct bfd_link_hash_entry *) NULL
	    || h->type != bfd_link_hash_defined)
	  return bfd_reloc_continue;    /* Actually this indicates that __ep could not be found. */

	ep = (h->u.def.value
	      + h->u.def.section->output_section->vma
	      + h->u.def.section->output_offset);
	
	value -= ep;
	value += ((insn & 0x7f) << 1);
	
	if ((long) value > 0xfe || (long) value < 0)
	  return bfd_reloc_overflow;
	
	insn &= 0xff80;
	insn |= (value >> 1);
	
	bfd_put_16 (input_bfd, insn, hit_data);
	return bfd_reloc_ok;
      }
    
    case R_V850_TDA_7_7_OFFSET:
      {
	unsigned long                ep;
	struct bfd_link_hash_entry * h;
	
	insn = bfd_get_16 (input_bfd, hit_data);

	/* Get the value of __ep.  */
	h = bfd_link_hash_lookup (info->hash, "__ep", false, false, true);
	if (h == (struct bfd_link_hash_entry *) NULL
	    || h->type != bfd_link_hash_defined)
	  return bfd_reloc_continue;  /* Actually this indicates that __ep could not be found. */

	ep = (h->u.def.value
	      + h->u.def.section->output_section->vma
	      + h->u.def.section->output_offset);
	value -= ep;
	
	value += insn & 0x7f;
	
	if ((long) value > 0x7f || (long) value < 0)
	  return bfd_reloc_overflow;
	
	insn &= 0xff80;
	insn |= value;
	bfd_put_16 (input_bfd, insn, hit_data);
	return bfd_reloc_ok;
      }
    
    case R_V850_TDA_16_16_OFFSET:
      {
	unsigned long                ep;
	struct bfd_link_hash_entry * h;

	/* Get the value of __ep.  */
	h = bfd_link_hash_lookup (info->hash, "__ep", false, false, true);
	if (h == (struct bfd_link_hash_entry *) NULL
	    || h->type != bfd_link_hash_defined)
	  return bfd_reloc_other;

	ep = (h->u.def.value
	      + h->u.def.section->output_section->vma
	      + h->u.def.section->output_offset);
	value -= ep;
	
	value += (short) bfd_get_16 (input_bfd, hit_data);

	if ((long)value > 0x7fff || (long)value < -0x8000)
	  return bfd_reloc_overflow;

	bfd_put_16 (input_bfd, value, hit_data);
	return bfd_reloc_ok;
      }

/* start-sanitize-v850e */
    case R_V850_TDA_4_5_OFFSET:
      {
	unsigned long                ep;
	struct bfd_link_hash_entry * h;
	
	/* Get the value of __ep.  */
	h = bfd_link_hash_lookup (info->hash, "__ep", false, false, true);
	if (h == (struct bfd_link_hash_entry *) NULL
	    || h->type != bfd_link_hash_defined)
	  return bfd_reloc_continue;  /* Actually this indicates that __ep could not be found. */

	ep = (h->u.def.value
	      + h->u.def.section->output_section->vma
	      + h->u.def.section->output_offset);
	value -= ep;
	
	insn = bfd_get_16 (input_bfd, hit_data);

	value += ((insn & 0xf) << 1);
	
	if ((long) value > 0x1e || (long) value < 0)
	  return bfd_reloc_overflow;
	
	insn &= 0xfff0;
	insn |= (value >> 1);
	bfd_put_16 (input_bfd, insn, hit_data);
	return bfd_reloc_ok;
      }
    
    case R_V850_TDA_4_4_OFFSET:
      {
	unsigned long                ep;
	struct bfd_link_hash_entry * h;
	
	/* Get the value of __ep.  */
	h = bfd_link_hash_lookup (info->hash, "__ep", false, false, true);
	if (h == (struct bfd_link_hash_entry *) NULL
	    || h->type != bfd_link_hash_defined)
	  return bfd_reloc_continue;  /* Actually this indicates that __ep could not be found. */

	ep = (h->u.def.value
	      + h->u.def.section->output_section->vma
	      + h->u.def.section->output_offset);
	value -= ep;
	
	insn = bfd_get_16 (input_bfd, hit_data);

	value += insn & 0xf;
	
	if ((long) value > 0xf || (long) value < 0)
	  return bfd_reloc_overflow;
	
	insn &= 0xfff0;
	insn |= value;
	bfd_put_16 (input_bfd, insn, hit_data);
	return bfd_reloc_ok;
      }
    
    case R_V850_SDA_16_16_SPLIT_OFFSET:
      if (sym_sec == NULL)
	return bfd_reloc_undefined;
	  
      {
	unsigned long                gp;
	struct bfd_link_hash_entry * h;

	/* Get the value of __gp.  */
	h = bfd_link_hash_lookup (info->hash, "__gp", false, false, true);
	if (h == (struct bfd_link_hash_entry *) NULL
	    || h->type != bfd_link_hash_defined)
	  return bfd_reloc_other;

	gp = (h->u.def.value
	      + h->u.def.section->output_section->vma
	      + h->u.def.section->output_offset);
	
	value -= sym_sec->output_section->vma;
	value -= (gp - sym_sec->output_section->vma);

	insn = bfd_get_32 (input_bfd, hit_data);
	
	value += ((insn & 0xfffe0000) >> 16);
	value += ((insn & 0x20) >> 5);
	  
	if ((long)value > 0x7fff || (long)value < -0x8000)
	  return bfd_reloc_overflow;

	insn &= 0x0001ffdf;
	insn |= (value & 1) << 5;
	insn |= (value & ~1) << 16;
	
	bfd_put_32 (input_bfd, insn, hit_data);
	return bfd_reloc_ok;
      }

    case R_V850_ZDA_16_16_SPLIT_OFFSET:
      if (sym_sec == NULL)
	return bfd_reloc_undefined;
	  
      insn = bfd_get_32 (input_bfd, hit_data);
	
      value -= sym_sec->output_section->vma;
      value += ((insn & 0xfffe0000) >> 16);
      value += ((insn & 0x20) >> 5);
	  
      if ((long)value > 0x7fff || (long)value < -0x8000)
	return bfd_reloc_overflow;
      
      insn &= 0x0001ffdf;
      insn |= (value & 1) << 5;
      insn |= (value & ~1) << 16;
      
      bfd_put_32 (input_bfd, insn, hit_data);
      return bfd_reloc_ok;

    case R_V850_CALLT_6_7_OFFSET:
      {
	unsigned long                ctbp;
	struct bfd_link_hash_entry * h;
	
	/* Get the value of __ctbp.  */
	h = bfd_link_hash_lookup (info->hash, "__ctbp", false, false, true);
	if (h == (struct bfd_link_hash_entry *) NULL
	    || h->type != bfd_link_hash_defined)
	  return bfd_reloc_continue;  /* Actually this indicates that __ctbp could not be found. */

	ctbp = (h->u.def.value
	      + h->u.def.section->output_section->vma
	      + h->u.def.section->output_offset);
	value -= ctbp;
	
	insn = bfd_get_16 (input_bfd, hit_data);

	value += ((insn & 0x3f) << 1);
	
	if ((long) value > 0x7e || (long) value < 0)
	  return bfd_reloc_overflow;
	
	insn &= 0xff80;
	insn |= (value >> 1);
	bfd_put_16 (input_bfd, insn, hit_data);
	return bfd_reloc_ok;
      }
    
    case R_V850_CALLT_16_16_OFFSET:
      if (sym_sec == NULL)
	return bfd_reloc_undefined;
	  
      {
	unsigned long                ctbp;
	struct bfd_link_hash_entry * h;

	/* Get the value of __ctbp.  */
	h = bfd_link_hash_lookup (info->hash, "__ctbp", false, false, true);
	if (h == (struct bfd_link_hash_entry *) NULL
	    || h->type != bfd_link_hash_defined)
	  return bfd_reloc_other;

	ctbp = (h->u.def.value
	      + h->u.def.section->output_section->vma
	      + h->u.def.section->output_offset);

	value -= sym_sec->output_section->vma;
	value -= (ctbp - sym_sec->output_section->vma);
	value += (short) bfd_get_16 (input_bfd, hit_data);

	if ((long) value > 0xffff || (long) value < 0)
	  return bfd_reloc_overflow;

	bfd_put_16 (input_bfd, value, hit_data);
	return bfd_reloc_ok;
      }

/* end-sanitize-v850e */
    
    case R_V850_NONE:
      return bfd_reloc_ok;

    default:
      return bfd_reloc_notsupported;
    }
}


/* Relocate an V850 ELF section.  */
static boolean
v850_elf_relocate_section (output_bfd, info, input_bfd, input_section,
			   contents, relocs, local_syms, local_sections)
     bfd *                  output_bfd;
     struct bfd_link_info * info;
     bfd *                  input_bfd;
     asection *             input_section;
     bfd_byte *             contents;
     Elf_Internal_Rela *    relocs;
     Elf_Internal_Sym *     local_syms;
     asection **            local_sections;
{
  Elf_Internal_Shdr *           symtab_hdr;
  struct elf_link_hash_entry ** sym_hashes;
  Elf_Internal_Rela *           rel;
  Elf_Internal_Rela *           relend;

  symtab_hdr = & elf_tdata (input_bfd)->symtab_hdr;
  sym_hashes = elf_sym_hashes (input_bfd);

  rel    = relocs;
  relend = relocs + input_section->reloc_count;
  for (; rel < relend; rel++)
    {
      int                          r_type;
      reloc_howto_type *           howto;
      unsigned long                r_symndx;
      Elf_Internal_Sym *           sym;
      asection *                   sec;
      struct elf_link_hash_entry * h;
      bfd_vma                      relocation;
      bfd_reloc_status_type        r;

      r_symndx = ELF32_R_SYM (rel->r_info);
      r_type   = ELF32_R_TYPE (rel->r_info);
      howto    = v850_elf_howto_table + r_type;

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
		  sec = local_sections[r_symndx];
		  rel->r_addend += sec->output_offset + sym->st_value;
		}
	    }

	  continue;
	}

      /* This is a final link.  */
      h = NULL;
      sym = NULL;
      sec = NULL;
      if (r_symndx < symtab_hdr->sh_info)
	{
	  sym = local_syms + r_symndx;
	  sec = local_sections[r_symndx];
	  relocation = (sec->output_section->vma
			+ sec->output_offset
			+ sym->st_value);
#if 0
	  {
	    char * name;
	    name = bfd_elf_string_from_elf_section (input_bfd, symtab_hdr->sh_link, sym->st_name);
	    name = (name == NULL) ? "<none>" : name;
fprintf (stderr, "local: sec: %s, sym: %s (%d), value: %x + %x + %x addend %x rel %x\n", sec->name, name, sym->st_name,
	 sec->output_section->vma, sec->output_offset, sym->st_value, rel->r_addend, rel);
	  }
#endif
	}
      else
	{
	  h = sym_hashes[r_symndx - symtab_hdr->sh_info];
	  
	  while (h->root.type == bfd_link_hash_indirect
		 || h->root.type == bfd_link_hash_warning)
	    h = (struct elf_link_hash_entry *) h->root.u.i.link;
	  
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
	  else
	    {
	      if (! ((*info->callbacks->undefined_symbol)
		     (info, h->root.root.string, input_bfd,
		      input_section, rel->r_offset)))
		return false;
	      relocation = 0;
	    }
	}

      /* FIXME: We should use the addend, but the COFF relocations
         don't.  */
      r = v850_elf_final_link_relocate (howto, input_bfd, output_bfd,
					input_section,
					contents, rel->r_offset,
					relocation, rel->r_addend,
					info, sec, h == NULL);

      if (r != bfd_reloc_ok)
	{
	  const char * name;
	  const char * msg = (const char *)0;

	  if (h != NULL)
	    name = h->root.root.string;
	  else
	    {
	      name = (bfd_elf_string_from_elf_section
		      (input_bfd, symtab_hdr->sh_link, sym->st_name));
	      if (name == NULL || *name == '\0')
		name = bfd_section_name (input_bfd, sec);
	    }

	  switch (r)
	    {
	    case bfd_reloc_overflow:
	      if (! ((*info->callbacks->reloc_overflow)
		     (info, name, howto->name, (bfd_vma) 0,
		      input_bfd, input_section, rel->r_offset)))
		return false;
	      break;

	    case bfd_reloc_undefined:
	      fprintf (stderr, "undef2 %s\n", name );
	      if (! ((*info->callbacks->undefined_symbol)
		     (info, name, input_bfd, input_section,
		      rel->r_offset)))
		return false;
	      break;

	    case bfd_reloc_outofrange:
	      msg = "internal error: out of range error";
	      goto common_error;

	    case bfd_reloc_notsupported:
	      msg = "internal error: unsupported relocation error";
	      goto common_error;

	    case bfd_reloc_dangerous:
	      msg = "internal error: dangerous relocation";
	      goto common_error;

	    case bfd_reloc_other:
	      msg = "could not locate special linker symbol __gp";
	      goto common_error;

	    case bfd_reloc_continue:
	      msg = "could not locate special linker symbol __ep";
	      goto common_error;

	    default:
	      msg = "internal error: unknown error";
	      /* fall through */

	    common_error:
	      if (!((*info->callbacks->warning)
		    (info, msg, name, input_bfd, input_section,
		     rel->r_offset)))
		return false;
	      break;
	    }
	}
    }

  return true;
}

/* Set the right machine number.  */
static boolean
v850_elf_object_p (abfd)
     bfd *abfd;
{
  switch (elf_elfheader (abfd)->e_flags & EF_V850_ARCH)
    {
    default:
    case E_V850_ARCH:   (void) bfd_default_set_arch_mach (abfd, bfd_arch_v850, 0); break;
/* start-sanitize-v850e */
    case E_V850E_ARCH:  (void) bfd_default_set_arch_mach (abfd, bfd_arch_v850, bfd_mach_v850e); break;
/* end-sanitize-v850e */
/* start-sanitize-v850eq */
    case E_V850EQ_ARCH: (void) bfd_default_set_arch_mach (abfd, bfd_arch_v850, bfd_mach_v850eq); break;
/* end-sanitize-v850eq */
    }
}

/* Store the machine number in the flags field.  */
void
v850_elf_final_write_processing (abfd, linker)
     bfd *   abfd;
     boolean linker;
{
  unsigned long val;

  switch (bfd_get_mach (abfd))
    {
    default:
    case 0: val = E_V850_ARCH; break;
/* start-sanitize-v850e */
    case bfd_mach_v850e: val = E_V850E_ARCH; break;
/* end-sanitize-v850e */
/* start-sanitize-v850eq */
    case bfd_mach_v850eq: val = E_V850EQ_ARCH;  break;
/* end-sanitize-v850eq */
    }

  elf_elfheader (abfd)->e_flags &=~ EF_V850_ARCH;
  elf_elfheader (abfd)->e_flags |= val;
}

/* Function to keep V850 specific file flags. */
boolean
v850_elf_set_private_flags (abfd, flags)
     bfd *    abfd;
     flagword flags;
{
  BFD_ASSERT (!elf_flags_init (abfd)
	      || elf_elfheader (abfd)->e_flags == flags);

  elf_elfheader (abfd)->e_flags = flags;
  elf_flags_init (abfd) = true;
  return true;
}

/* Copy backend specific data from one object module to another */
boolean
v850_elf_copy_private_bfd_data (ibfd, obfd)
     bfd * ibfd;
     bfd * obfd;
{
  if (   bfd_get_flavour (ibfd) != bfd_target_elf_flavour
      || bfd_get_flavour (obfd) != bfd_target_elf_flavour)
    return true;

  BFD_ASSERT (!elf_flags_init (obfd)
	      || (elf_elfheader (obfd)->e_flags
		  == elf_elfheader (ibfd)->e_flags));

  elf_gp (obfd) = elf_gp (ibfd);
  elf_elfheader (obfd)->e_flags = elf_elfheader (ibfd)->e_flags;
  elf_flags_init (obfd) = true;
  return true;
}

/* Merge backend specific data from an object file to the output
   object file when linking.  */
boolean
v850_elf_merge_private_bfd_data (ibfd, obfd)
     bfd * ibfd;
     bfd * obfd;
{
  flagword old_flags;
  flagword new_flags;

  if (   bfd_get_flavour (ibfd) != bfd_target_elf_flavour
      || bfd_get_flavour (obfd) != bfd_target_elf_flavour)
    return true;

  new_flags = elf_elfheader (ibfd)->e_flags;
  old_flags = elf_elfheader (obfd)->e_flags;

  if (! elf_flags_init (obfd))
    {
      elf_flags_init (obfd) = true;
      elf_elfheader (obfd)->e_flags = new_flags;

      if (bfd_get_arch (obfd) == bfd_get_arch (ibfd)
	  && bfd_get_arch_info (obfd)->the_default)
	{
	  return bfd_set_arch_mach (obfd, bfd_get_arch (ibfd), bfd_get_mach (ibfd));
	}

      return true;
    }

  /* Check flag compatibility.  */

  if (new_flags == old_flags)
    return true;

  if ((new_flags & EF_V850_ARCH) != (old_flags & EF_V850_ARCH))
    {
      _bfd_error_handler ("%s: Architecture mismatch with previous modules",
	     bfd_get_filename (ibfd));
      bfd_set_error (bfd_error_bad_value);
      return false;
    }

  return true;
}
/* Display the flags field */

static boolean
v850_elf_print_private_bfd_data (abfd, ptr)
     bfd *   abfd;
     PTR     ptr;
{
  FILE * file = (FILE *) ptr;
  
  BFD_ASSERT (abfd != NULL && ptr != NULL)
  
  fprintf (file, "private flags = %x", elf_elfheader (abfd)->e_flags);
  
  switch (elf_elfheader (abfd)->e_flags & EF_V850_ARCH)
    {
    default:
    case E_V850_ARCH: fprintf (file, ": v850 architecture"); break;
/* start-sanitize-v850e */
    case E_V850E_ARCH: fprintf (file, ": v850e architecture"); break;
/* end-sanitize-v850e */
/* start-sanitize-v850eq */
    case E_V850EQ_ARCH: fprintf (file, ": v850eq architecture"); break;
/* end-sanitize-v850eq */
    }
  
  fputc ('\n', file);
  
  return true;
}

#define TARGET_LITTLE_SYM			bfd_elf32_v850_vec
#define TARGET_LITTLE_NAME			"elf32-v850"
#define ELF_ARCH				bfd_arch_v850
#define ELF_MACHINE_CODE			EM_CYGNUS_V850
#define ELF_MAXPAGESIZE				0x1000
	
#define elf_info_to_howto			0
#define elf_info_to_howto_rel			v850_elf_info_to_howto_rel
#define elf_backend_check_relocs		v850_elf_check_relocs
#define elf_backend_relocate_section    	v850_elf_relocate_section
#define elf_backend_object_p			v850_elf_object_p
#define elf_backend_final_write_processing 	v850_elf_final_write_processing
#define bfd_elf32_bfd_is_local_label_name	v850_elf_is_local_label_name
#define bfd_elf32_bfd_reloc_type_lookup		v850_elf_reloc_type_lookup
#define bfd_elf32_bfd_copy_private_bfd_data 	v850_elf_copy_private_bfd_data
#define bfd_elf32_bfd_merge_private_bfd_data 	v850_elf_merge_private_bfd_data
#define bfd_elf32_bfd_set_private_flags		v850_elf_set_private_flags
#define bfd_elf32_bfd_print_private_bfd_data	v850_elf_print_private_bfd_data

#define elf_symbol_leading_char			'_'

#include "elf32-target.h"
