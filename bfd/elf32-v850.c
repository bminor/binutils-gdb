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
static bfd_reloc_status_type bfd_elf32_v850_reloc
  PARAMS ((bfd *, arelent *, asymbol *, PTR, asection *, bfd *, char **));



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
  R_V850_32,
  R_V850_16,
  R_V850_8,
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
	 bfd_elf32_v850_reloc,	/* special_function */
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
	 bfd_elf32_v850_reloc,	/* special_function */
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
         0,                    /* bitpos */
         complain_overflow_dont,/* complain_on_overflow */
         bfd_elf32_v850_reloc, /* special_function */
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
         0,                    /* bitpos */
         complain_overflow_dont,/* complain_on_overflow */
         bfd_elf32_v850_reloc, /* special_function */
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
         0,                     /* bitpos */
         complain_overflow_dont,/* complain_on_overflow */
         bfd_elf_generic_reloc, /* special_function */
         "R_V850_LO16",         /* name */
         true,                  /* partial_inplace */
         0xffff,                /* src_mask */
         0xffff,                /* dst_mask */
         false),                /* pcrel_offset */

  /* Simple 32bit reloc.  */
  HOWTO (R_V850_32,           /* type */
         0,                     /* rightshift */
         2,                     /* size (0 = byte, 1 = short, 2 = long) */
         32,                    /* bitsize */
         false,                 /* pc_relative */
         0,                     /* bitpos */
         complain_overflow_dont,/* complain_on_overflow */
         bfd_elf_generic_reloc, /* special_function */
         "R_V850_32",         /* name */
         true,                  /* partial_inplace */
         0xffffffff,                /* src_mask */
         0xffffffff,                /* dst_mask */
         false),                /* pcrel_offset */

  /* Simple 16bit reloc.  */
  HOWTO (R_V850_16,           /* type */
         0,                     /* rightshift */
         1,                     /* size (0 = byte, 1 = short, 2 = long) */
         16,                    /* bitsize */
         false,                 /* pc_relative */
         0,                     /* bitpos */
         complain_overflow_dont,/* complain_on_overflow */
         bfd_elf_generic_reloc, /* special_function */
         "R_V850_16",         /* name */
         true,                  /* partial_inplace */
         0xffff,                /* src_mask */
         0xffff,                /* dst_mask */
         false),                /* pcrel_offset */

  /* Simple 8bit reloc.  */
  HOWTO (R_V850_8,           /* type */
         0,                     /* rightshift */
         0,                     /* size (0 = byte, 1 = short, 2 = long) */
         8,                    /* bitsize */
         false,                 /* pc_relative */
         0,                     /* bitpos */
         complain_overflow_dont,/* complain_on_overflow */
         bfd_elf_generic_reloc, /* special_function */
         "R_V850_8",         /* name */
         true,                  /* partial_inplace */
         0xff,                /* src_mask */
         0xff,                /* dst_mask */
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
  { BFD_RELOC_32, R_V850_32, },
  { BFD_RELOC_16, R_V850_16, },
  { BFD_RELOC_8, R_V850_8, },
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

static bfd_reloc_status_type
bfd_elf32_v850_reloc (abfd, reloc, symbol, data, isection, obfd, err)
     bfd *abfd;
     arelent *reloc;
     asymbol *symbol;
     PTR data;
     asection *isection;
     bfd *obfd;
     char **err;
{
  if (obfd != (bfd *) NULL
      && (symbol->flags & BSF_SECTION_SYM) == 0
      && (! reloc->howto->partial_inplace
	  || reloc->addend == 0))
    {
      reloc->address += isection->output_offset;
      return bfd_reloc_ok;
    }
  else if (obfd != NULL)
    {
      return bfd_reloc_continue;
    }

  /* We handle final linking of some relocs ourselves.  */
    {
      long relocation, insn;

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

      if (reloc->howto->type == R_V850_22_PCREL)
	{
	  if (relocation > 0x1ffff || relocation < -0x200000)
	    return bfd_reloc_overflow;

	  if ((relocation % 2) != 0)
	    return bfd_reloc_dangerous;

	  insn = bfd_get_32 (abfd, (bfd_byte *) data + reloc->address);
	  insn |= (((relocation & 0xfffe) << 16)
		   | ((relocation & 0x3f0000) >> 16));
	  bfd_put_32 (abfd, insn, (bfd_byte *)data + reloc->address);
	  return bfd_reloc_ok;
	}
      else if (reloc->howto->type == R_V850_9_PCREL)
	{
	  if (relocation > 0xff || relocation < -0x100)
	    return bfd_reloc_overflow;

	  if ((relocation % 2) != 0)
	    return bfd_reloc_dangerous;

	  insn = bfd_get_16 (abfd, (bfd_byte *) data + reloc->address);
	  insn |= ((relocation & 0x1f0) << 7) | ((relocation & 0x0e) << 3);
	  bfd_put_16 (abfd, insn, (bfd_byte *)data + reloc->address);
	  return bfd_reloc_ok;
	}
      else if (reloc->howto->type == R_V850_HI16_S)
	{
	  relocation += bfd_get_16 (abfd, (bfd_byte *) data + reloc->address);
	  relocation = (relocation >> 16) + ((relocation & 0x8000) != 0);
	  bfd_put_16 (abfd, relocation, (bfd_byte *)data + reloc->address);
	  return bfd_reloc_ok;
	}
      else if (reloc->howto->type == R_V850_HI16)
	{
	  relocation += bfd_get_16 (abfd, (bfd_byte *) data + reloc->address);
	  relocation = (relocation >> 16);
	  bfd_put_16 (abfd, relocation, (bfd_byte *)data + reloc->address);
	  return bfd_reloc_ok;
	}
    }

  return bfd_reloc_continue;
}

static boolean bfd_elf32_v850_is_local_label PARAMS ((bfd *, asymbol *));

/*ARGSUSED*/
static boolean
bfd_elf32_v850_is_local_label (abfd, symbol)
     bfd *abfd;
     asymbol *symbol;
{
  return ((symbol->name[0] == '.' && (symbol->name[1] == 'L' || symbol->name[1] == '.'))
	  || (symbol->name[0] == '_' && symbol->name[1] == '.' && symbol->name[2] == 'L'
	      && symbol->name[3] == '_'));
}

#define bfd_elf32_bfd_is_local_label	bfd_elf32_v850_is_local_label

#define TARGET_LITTLE_SYM		bfd_elf32_v850_vec
#define TARGET_LITTLE_NAME		"elf32-v850"
#define ELF_ARCH		bfd_arch_v850
#define ELF_MACHINE_CODE	EM_CYGNUS_V850
#define ELF_MAXPAGESIZE		0x1000

#define elf_info_to_howto	0
#define elf_info_to_howto_rel	v850_info_to_howto_rel

#define elf_symbol_leading_char '_'

#include "elf32-target.h"
