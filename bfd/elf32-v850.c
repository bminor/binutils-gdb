/* V850-specific support for 32-bit ELF
   Copyright (C) 1994, 1995 Free Software Foundation, Inc.

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

static reloc_howto_type *bfd_elf32_bfd_reloc_type_lookup
  PARAMS ((bfd *abfd, bfd_reloc_code_real_type code));
static void v850_info_to_howto_rel
  PARAMS ((bfd *, arelent *, Elf32_Internal_Rel *));


/* Try to minimize the amount of space occupied by relocation tables
   on the ROM (not that the ROM won't be swamped by other ELF overhead).  */
#define USE_REL

enum reloc_type
{
  R_V850_NONE = 0,
  R_V850_9_PCREL,
  R_V850_22_PCREL,
  R_V850_HI16_S,
  R_V850_HI16,
  R_V850_LO16,
  R_V850_max
};

static reloc_howto_type elf_v850_howto_table[] =
{
  /* This reloc does nothing.  */
  HOWTO (R_V850_NONE,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_V850_NONE",		/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 false),		/* pcrel_offset */

  /* A PC relative 9 bit branch. */
  HOWTO (R_V850_9_PCREL,	/* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 26,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_V850_9_PCREL",	/* name */
	 false,			/* partial_inplace */
	 0x00ffffff,		/* src_mask */
	 0x00ffffff,		/* dst_mask */
	 true),			/* pcrel_offset */

  /* A PC relative 22 bit branch. */
  HOWTO (R_V850_22_PCREL,	/* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 22,			/* bitsize */
	 true,			/* pc_relative */
	 7,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_V850_22_PCREL",	/* name */
	 false,			/* partial_inplace */
	 0x07ffff80,		/* src_mask */
	 0x07ffff80,		/* dst_mask */
	 true),			/* pcrel_offset */

  /* High 16 bits of symbol value.  */
  HOWTO (R_V850_HI16_S,         /* type */
         0,                     /* rightshift */
         1,                     /* size (0 = byte, 1 = short, 2 = long) */
         16,                    /* bitsize */
         false,                 /* pc_relative */
         16,                    /* bitpos */
         complain_overflow_dont,/* complain_on_overflow */
         bfd_elf_generic_reloc, /* special_function */
         "R_V850_HI16_S",       /* name */
         true,                  /* partial_inplace */
         0xffff,                /* src_mask */
         0xffff,                /* dst_mask */
         false),                /* pcrel_offset */

  /* High 16 bits of symbol value.  */
  HOWTO (R_V850_HI16,           /* type */
         0,                     /* rightshift */
         1,                     /* size (0 = byte, 1 = short, 2 = long) */
         16,                    /* bitsize */
         false,                 /* pc_relative */
         16,                    /* bitpos */
         complain_overflow_dont,/* complain_on_overflow */
         bfd_elf_generic_reloc, /* special_function */
         "R_V850_HI16",         /* name */
         true,                  /* partial_inplace */
         0xffff,                /* src_mask */
         0xffff,                /* dst_mask */
         false),                /* pcrel_offset */

  /* Low 16 bits of symbol value.  */
  HOWTO (R_V850_LO16,           /* type */
         0,                     /* rightshift */
         1,                     /* size (0 = byte, 1 = short, 2 = long) */
         16,                    /* bitsize */
         false,                 /* pc_relative */
         16,                     /* bitpos */
         complain_overflow_dont,/* complain_on_overflow */
         bfd_elf_generic_reloc, /* special_function */
         "R_V850_LO16",         /* name */
         true,                  /* partial_inplace */
         0xffff,                /* src_mask */
         0xffff,                /* dst_mask */
         false),                /* pcrel_offset */
};

/* Map BFD reloc types to V850 ELF reloc types.  */

struct v850_reloc_map
{
  unsigned char bfd_reloc_val;
  unsigned char elf_reloc_val;
};

static const struct v850_reloc_map v850_reloc_map[] =
{
  { BFD_RELOC_NONE, R_V850_NONE, },
  { BFD_RELOC_V850_9_PCREL, R_V850_9_PCREL, },
  { BFD_RELOC_V850_22_PCREL, R_V850_22_PCREL, },
  { BFD_RELOC_HI16_S, R_V850_HI16_S, },
  { BFD_RELOC_HI16, R_V850_HI16, },
  { BFD_RELOC_LO16, R_V850_LO16, },
};

static reloc_howto_type *
bfd_elf32_bfd_reloc_type_lookup (abfd, code)
     bfd *abfd;
     bfd_reloc_code_real_type code;
{
  unsigned int i;

  for (i = 0;
       i < sizeof (v850_reloc_map) / sizeof (struct v850_reloc_map);
       i++)
    {
      if (v850_reloc_map[i].bfd_reloc_val == code)
	return &elf_v850_howto_table[v850_reloc_map[i].elf_reloc_val];
    }

  return NULL;
}

/* Set the howto pointer for an V850 ELF reloc.  */

static void
v850_info_to_howto_rel (abfd, cache_ptr, dst)
     bfd *abfd;
     arelent *cache_ptr;
     Elf32_Internal_Rel *dst;
{
  unsigned int r_type;

  r_type = ELF32_R_TYPE (dst->r_info);
  BFD_ASSERT (r_type < (unsigned int) R_V850_max);
  cache_ptr->howto = &elf_v850_howto_table[r_type];
}

#define TARGET_BIG_SYM		bfd_elf32_v850_vec
#define TARGET_BIG_NAME		"elf32-v850"
#define ELF_ARCH		bfd_arch_v850
#define ELF_MACHINE_CODE	EM_CYGNUS_V850
#define ELF_MAXPAGESIZE		0x1000

#define elf_info_to_howto	0
#define elf_info_to_howto_rel	v850_info_to_howto_rel

#include "elf32-target.h"
