/* M32R-specific support for 32-bit ELF.
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

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"
#include "elf-bfd.h"
#include "elf/m32r.h"

static bfd_reloc_status_type m32r_elf_10_pcrel_reloc
  PARAMS ((bfd *, arelent *, asymbol *, PTR, asection *, bfd *, char **));
static bfd_reloc_status_type m32r_elf_hi16_slo_reloc
  PARAMS ((bfd *, arelent *, asymbol *, PTR, asection *, bfd *, char **));
static reloc_howto_type *bfd_elf32_bfd_reloc_type_lookup
  PARAMS ((bfd *abfd, bfd_reloc_code_real_type code));
static void m32r_info_to_howto_rel
  PARAMS ((bfd *, arelent *, Elf32_Internal_Rel *));

/* Use REL instead of RELA to save space.  */
#define USE_REL

static reloc_howto_type elf_m32r_howto_table[] =
{
  /* This reloc does nothing.  */
  HOWTO (R_M32R_NONE,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_M32R_NONE",		/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 false),		/* pcrel_offset */

  /* A 16 bit absolute relocation.  */
  HOWTO (R_M32R_16,		/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_M32R_16",		/* name */
	 false,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* A 32 bit absolute relocation.  */
  HOWTO (R_M32R_32,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_M32R_32",		/* name */
	 false,			/* partial_inplace */
	 0xffffffff,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* A 24 bit address.  */
  HOWTO (R_M32R_24,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 24,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_unsigned, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_M32R_24",		/* name */
	 false,			/* partial_inplace */
	 0xffffff,		/* src_mask */
	 0xffffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* An PC Relative 10-bit relocation, shifted by 2.
     This reloc is complicated because relocations are relative to pc & -4.
     i.e. branches in the right insn slot use the address of the left insn
     slot for pc.  */
  HOWTO (R_M32R_10_PCREL,	/* type */
	 2,	                /* rightshift */
	 1,	                /* size (0 = byte, 1 = short, 2 = long) */
	 10,	                /* bitsize */
	 true,	                /* pc_relative */
	 0,	                /* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 m32r_elf_10_pcrel_reloc, /* special_function */
	 "R_M32R_10_PCREL",	/* name */
	 false,	                /* partial_inplace */
	 0xff,		        /* src_mask */
	 0xff,   		/* dst_mask */
	 true),			/* pcrel_offset */

  /* A relative 18 bit relocation, right shifted by 2.  */
  HOWTO (R_M32R_18_PCREL,	/* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 18,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_M32R_18_PCREL",	/* name */
	 false,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 true),			/* pcrel_offset */

  /* A relative 26 bit relocation, right shifted by 2.  */
  HOWTO (R_M32R_26_PCREL,	/* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 26,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_M32R_26_PCREL",	/* name */
	 false,			/* partial_inplace */
	 0xffffff,		/* src_mask */
	 0xffffff,		/* dst_mask */
	 true),			/* pcrel_offset */

  /* High 16 bits of address when lower 16 is or'd in.  */
  HOWTO (R_M32R_HI16_ULO,	/* type */
	 16,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_M32R_HI16_ULO",	/* name */
	 false,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* High 16 bits of address when lower 16 is added in.  */
  HOWTO (R_M32R_HI16_SLO,	/* type */
	 16,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 m32r_elf_hi16_slo_reloc, /* special_function */
	 "R_M32R_HI16_SLO",	/* name */
	 false,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* Lower 16 bits of address.  */
  HOWTO (R_M32R_LO16,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_M32R_LO16",		/* name */
	 false,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 false),		/* pcrel_offset */
};

/* Handle the R_M32R_10_PCREL reloc.  */

static bfd_reloc_status_type
m32r_elf_10_pcrel_reloc (abfd, reloc_entry, symbol, data,
			 input_section, output_bfd, error_message)
     bfd *abfd;
     arelent *reloc_entry;
     asymbol *symbol;
     PTR data;
     asection *input_section;
     bfd *output_bfd;
     char **error_message;
{
  bfd_vma relocation;
  bfd_vma x;
  reloc_howto_type *howto;

  /* This part if from bfd_elf_generic_reloc.  */
  if (output_bfd != (bfd *) NULL
      && (symbol->flags & BSF_SECTION_SYM) == 0
      && (! reloc_entry->howto->partial_inplace
	  || reloc_entry->addend == 0))
    {
      reloc_entry->address += input_section->output_offset;
      return bfd_reloc_ok;
    }

  if (output_bfd != NULL)
    {
      /* FIXME: See bfd_perform_relocation.  Is this right?  */
      return bfd_reloc_continue;
    }

  /* The rest is copied from bfd_perform_relocation, modified to suit us.  */

  if (reloc_entry->address > input_section->_cooked_size)
    return bfd_reloc_outofrange;

  relocation = (symbol->value
		+ symbol->section->output_section->vma
		+ symbol->section->output_offset);
  relocation += reloc_entry->addend;
  relocation -=	(input_section->output_section->vma
		 + input_section->output_offset);
  relocation -= (reloc_entry->address & -4L);

  howto = reloc_entry->howto;
  x = bfd_get_16 (abfd, (char *) data + reloc_entry->address);
  relocation >>= howto->rightshift;
  relocation <<= howto->bitpos;
  x = (x & ~howto->dst_mask) | (((x & howto->src_mask) + relocation) & howto->dst_mask);
  bfd_put_16 (abfd, x, (char *) data + reloc_entry->address);

  if ((bfd_signed_vma) relocation < - 0x80
      || (bfd_signed_vma) relocation > 0x7f)
    return bfd_reloc_overflow;
  else
    return bfd_reloc_ok;
}

/* Handle the R_M32R_HI16_SLO reloc.
   This reloc is used in load/store with displacement instructions.
   The lower 16 bits are sign extended when added to the high 16 bytes
   so if the lower 16 bits are negative (bit 15 == 1) then we must add one
   to the high 16 bytes (which will get subtracted off when the low 16 bits
   are added.  */

static bfd_reloc_status_type
m32r_elf_hi16_slo_reloc (abfd, reloc_entry, symbol, data,
			 input_section, output_bfd, error_message)
     bfd *abfd;
     arelent *reloc_entry;
     asymbol *symbol;
     PTR data;
     asection *input_section;
     bfd *output_bfd;
     char **error_message;
{
  bfd_vma relocation;
  unsigned long x;
  int bit15;
  reloc_howto_type *howto;

  /* This part if from bfd_elf_generic_reloc.  */
  if (output_bfd != (bfd *) NULL
      && (symbol->flags & BSF_SECTION_SYM) == 0
      && (! reloc_entry->howto->partial_inplace
	  || reloc_entry->addend == 0))
    {
      reloc_entry->address += input_section->output_offset;
      return bfd_reloc_ok;
    }

  if (output_bfd != NULL)
    {
      /* FIXME: See bfd_perform_relocation.  Is this right?  */
      return bfd_reloc_continue;
    }

  /* The rest is copied from bfd_perform_relocation, modified to suit us.  */

  if (reloc_entry->address > input_section->_cooked_size)
    return bfd_reloc_outofrange;

  relocation = (symbol->value
		+ symbol->section->output_section->vma
		+ symbol->section->output_offset);
  relocation += reloc_entry->addend;

  howto = reloc_entry->howto;
  x = bfd_get_32 (abfd, (char *) data + reloc_entry->address);
  bit15 = relocation & 0x8000;
  relocation >>= howto->rightshift;
  if (bit15)
    relocation += 1;
  relocation <<= howto->bitpos;
  x = (x & ~howto->dst_mask) | (((x & howto->src_mask) + relocation) & howto->dst_mask);
  bfd_put_32 (abfd, x, (char *) data + reloc_entry->address);

  return bfd_reloc_ok;
}

/* Map BFD reloc types to M32R ELF reloc types.  */

struct m32r_reloc_map
{
  unsigned char bfd_reloc_val;
  unsigned char elf_reloc_val;
};

static const struct m32r_reloc_map m32r_reloc_map[] =
{
  { BFD_RELOC_NONE, R_M32R_NONE },
  { BFD_RELOC_16, R_M32R_16 },
  { BFD_RELOC_32, R_M32R_32 },
  { BFD_RELOC_M32R_24, R_M32R_24 },
  { BFD_RELOC_M32R_10_PCREL, R_M32R_10_PCREL },
  { BFD_RELOC_M32R_18_PCREL, R_M32R_18_PCREL },
  { BFD_RELOC_M32R_26_PCREL, R_M32R_26_PCREL },
  { BFD_RELOC_M32R_HI16_ULO, R_M32R_HI16_ULO },
  { BFD_RELOC_M32R_HI16_SLO, R_M32R_HI16_SLO },
  { BFD_RELOC_M32R_LO16, R_M32R_LO16 },
};

static reloc_howto_type *
bfd_elf32_bfd_reloc_type_lookup (abfd, code)
     bfd *abfd;
     bfd_reloc_code_real_type code;
{
  unsigned int i;

  for (i = 0;
       i < sizeof (m32r_reloc_map) / sizeof (struct m32r_reloc_map);
       i++)
    {
      if (m32r_reloc_map[i].bfd_reloc_val == code)
	return &elf_m32r_howto_table[m32r_reloc_map[i].elf_reloc_val];
    }

  return NULL;
}

/* Set the howto pointer for an M32R ELF reloc.  */

static void
m32r_info_to_howto_rel (abfd, cache_ptr, dst)
     bfd *abfd;
     arelent *cache_ptr;
     Elf32_Internal_Rel *dst;
{
  unsigned int r_type;

  r_type = ELF32_R_TYPE (dst->r_info);
  BFD_ASSERT (r_type < (unsigned int) R_M32R_max);
  cache_ptr->howto = &elf_m32r_howto_table[r_type];
}

#define ELF_ARCH		bfd_arch_m32r
#define ELF_MACHINE_CODE	EM_CYGNUS_M32R
#define ELF_MAXPAGESIZE		0x1000

#define TARGET_BIG_SYM          bfd_elf32_m32r_vec
#define TARGET_BIG_NAME		"elf32-m32r"

#define elf_info_to_howto	0
#define elf_info_to_howto_rel	m32r_info_to_howto_rel
#define elf_backend_object_p	0
#define elf_backend_final_write_processing	0

#include "elf32-target.h"
