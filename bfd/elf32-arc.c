/* ARC-specific support for 32-bit ELF
   Copyright (C) 1994 Free Software Foundation, Inc.
   Contributed by Doug Evans (dje@cygnus.com).

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
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"
#include "libelf.h"

static bfd_reloc_status_type arc_elf_unsupported_reloc
  PARAMS ((bfd *, arelent *, asymbol *, PTR, asection *, bfd *, char **));
static const struct reloc_howto_struct *bfd_elf32_bfd_reloc_type_lookup
  PARAMS ((bfd *abfd, bfd_reloc_code_real_type code));
static void arc_info_to_howto
  PARAMS ((bfd *abfd, arelent *cache_ptr, Elf32_Internal_Rela *dst));

enum reloc_type
{
  R_ARC_NONE = 0,
  R_ARC_ADDR32,
  R_ARC_B22_PCREL,
  R_ARC_max
};

static reloc_howto_type elf_arc_howto_table[] =
{
  /* This reloc does nothing.  */
  HOWTO (R_ARC_NONE,		/* type */                                 
	 0,			/* rightshift */                           
	 2,	                /* size (0 = byte, 1 = short, 2 = long) */ 
	 32,	                /* bitsize */                   
	 false,	                /* pc_relative */                          
	 0,	                /* bitpos */                               
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc, /* special_function */                     
	 "R_ARC_NONE",          /* name */                                 
	 false,	                /* partial_inplace */                      
	 0,		        /* src_mask */                             
	 0,            		/* dst_mask */                             
	 false),                /* pcrel_offset */

  /* A standard 32 bit relocation.  */
  HOWTO (R_ARC_ADDR32,          /* type */                                 
	 0,	                /* rightshift */                           
	 2,	                /* size (0 = byte, 1 = short, 2 = long) */ 
	 32,	                /* bitsize */                   
	 false,	                /* pc_relative */                          
	 0,	                /* bitpos */                               
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc, /* special_function */                     
	 "R_ARC_ADDR32",	/* name */
	 false,	                /* partial_inplace */                      
	 0,		        /* src_mask */                             
	 0xffffffff,   		/* dst_mask */                             
	 false),                /* pcrel_offset */

  /* A relative 22 bit branch; the lower two bits must be zero.  */
  HOWTO (R_ARC_B22_PCREL,       /* type */                                 
	 2,	                /* rightshift */                           
	 2,	                /* size (0 = byte, 1 = short, 2 = long) */ 
	 22,	                /* bitsize */                   
	 true,	                /* pc_relative */                          
	 7,	                /* bitpos */                               
	 complain_overflow_signed, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */                     
	 "R_ARC_B22_PCREL",	/* name */
	 false,	                /* partial_inplace */                      
	 0,		        /* src_mask */                             
	 0x07ffff80,   		/* dst_mask */                             
	 true),                 /* pcrel_offset */

};

/* Don't pretend we can deal with unsupported relocs.  */

static bfd_reloc_status_type
arc_elf_unsupported_reloc (abfd, reloc_entry, symbol, data, input_section,
			   output_bfd, error_message)
     bfd *abfd;
     arelent *reloc_entry;
     asymbol *symbol;
     PTR data;
     asection *input_section;
     bfd *output_bfd;
     char **error_message;
{
  abort ();
}

/* Map BFD reloc types to PowerPC ELF reloc types.  */

struct arc_reloc_map
{
  unsigned char bfd_reloc_val;
  unsigned char elf_reloc_val;
};

static const struct arc_reloc_map arc_reloc_map[] =
{
  { BFD_RELOC_NONE, R_ARC_NONE, },
  { BFD_RELOC_32, R_ARC_ADDR32 },
  { BFD_RELOC_CTOR, R_ARC_ADDR32 },
  { BFD_RELOC_ARC_B22_PCREL, R_ARC_B22_PCREL },
};

static const struct reloc_howto_struct *
bfd_elf32_bfd_reloc_type_lookup (abfd, code)
     bfd *abfd;
     bfd_reloc_code_real_type code;
{
  int i;

  for (i = 0;
       i < sizeof (arc_reloc_map) / sizeof (struct arc_reloc_map);
       i++)
    {
      if (arc_reloc_map[i].bfd_reloc_val == code)
	return &elf_arc_howto_table[arc_reloc_map[i].elf_reloc_val];
    }

  return NULL;
}

/* Set the howto pointer for an ARC ELF reloc.  */

static void
arc_info_to_howto (abfd, cache_ptr, dst)
     bfd *abfd;
     arelent *cache_ptr;
     Elf32_Internal_Rela *dst;
{
  BFD_ASSERT (ELF32_R_TYPE (dst->r_info) < (unsigned int) R_ARC_max);
  cache_ptr->howto = &elf_arc_howto_table[ELF32_R_TYPE (dst->r_info)];
}

#define TARGET_BIG_SYM		bfd_elf32_arc_vec
#define TARGET_BIG_NAME		"elf32-arc"
#define ELF_ARCH		bfd_arch_arc
#define ELF_MACHINE_CODE	EM_CYGNUS_ARC
#define ELF_MAXPAGESIZE		0x10000
#define elf_info_to_howto	arc_info_to_howto

#include "elf32-target.h"
