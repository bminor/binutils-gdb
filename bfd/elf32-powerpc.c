/* PowerPC-specific support for 32-bit ELF
   Copyright 1994 Free Software Foundation, Inc.
   Written by Ian Lance Taylor, Cygnus Support.

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

/* I wrote this file without the benefit of a PowerPC ELF ABI.  Thus,
   it probably does not correspond to whatever people finally settle
   on.  Ian Taylor.  */

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"
#include "libelf.h"

static bfd_reloc_status_type powerpc_elf_toc16_reloc PARAMS ((bfd *abfd,
							      arelent *reloc,
							      asymbol *symbol,
							      PTR data,
							      asection *sec,
							      bfd *output_bfd,
							      char **error));
static bfd_reloc_status_type powerpc_elf_b26_reloc PARAMS ((bfd *abfd,
							    arelent *reloc,
							    asymbol *symbol,
							    PTR data,
							    asection *sec,
							    bfd *output_bfd,
							    char **error));
static const struct reloc_howto_struct *bfd_elf32_bfd_reloc_type_lookup
  PARAMS ((bfd *abfd, bfd_reloc_code_real_type code));
static void powerpc_info_to_howto_rel
  PARAMS ((bfd *abfd, arelent *cache_ptr, Elf32_Internal_Rel *dst));

#define USE_REL

enum reloc_type
{
  R_POWERPC_NONE = 0,
  R_POWERPC_32,
  R_POWERPC_B26,
  R_POWERPC_BA26,
  R_POWERPC_TOC16,
  R_POWERPC_max
};

static reloc_howto_type elf_powerpc_howto_table[] =
{
  /* This relocation does not do anything.  */
  HOWTO (R_POWERPC_NONE,        /* type */                                 
	 0,	                /* rightshift */                           
	 2,	                /* size (0 = byte, 1 = short, 2 = long) */ 
	 32,	                /* bitsize */                   
	 false,	                /* pc_relative */                          
	 0,	                /* bitpos */                               
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc, /* special_function */                     
	 "R_POWERPC_NONE",      /* name */                                 
	 true,	                /* partial_inplace */                      
	 0,		        /* src_mask */                             
	 0,            		/* dst_mask */                             
	 false),                /* pcrel_offset */

  /* Standard 32 bit relocation.  */
  HOWTO (R_POWERPC_32,          /* type */                                 
	 0,	                /* rightshift */                           
	 2,	                /* size (0 = byte, 1 = short, 2 = long) */ 
	 32,	                /* bitsize */                   
	 false,	                /* pc_relative */                          
	 0,	                /* bitpos */                               
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc, /* special_function */                     
	 "R_POWERPC_32",        /* name */                                 
	 true,	                /* partial_inplace */                      
	 0xffffffff,            /* src_mask */                             
	 0xffffffff,            /* dst_mask */                             
	 false),                /* pcrel_offset */

  /* 26 bit relative branch.  */
  HOWTO (R_POWERPC_B26,         /* type */                                 
	 0,	                /* rightshift */                           
	 2,	                /* size (0 = byte, 1 = short, 2 = long) */ 
	 26,	                /* bitsize */                   
	 true,	                /* pc_relative */                          
	 0,	                /* bitpos */                               
	 complain_overflow_signed, /* complain_on_overflow */
	 powerpc_elf_b26_reloc, /* special_function */                     
	 "R_POWERPC_B26",       /* name */                                 
	 true,	                /* partial_inplace */                      
	 0x3fffffc,	        /* src_mask */                             
	 0x3fffffc,        	/* dst_mask */                             
	 false),                /* pcrel_offset */

  /* 26 bit absolute branch.  */
  HOWTO (R_POWERPC_BA26,        /* type */                                 
	 0,	                /* rightshift */                           
	 2,	                /* size (0 = byte, 1 = short, 2 = long) */ 
	 26,	                /* bitsize */                   
	 false,	                /* pc_relative */                          
	 0,	                /* bitpos */                               
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc, /* special_function */                     
	 "R_POWERPC_BA26",      /* name */                                 
	 true,	                /* partial_inplace */                      
	 0x3fffffc,	        /* src_mask */                             
	 0x3fffffc,        	/* dst_mask */                             
	 false),                /* pcrel_offset */
  
  /* 16 bit reference to TOC section.  */
  HOWTO (R_POWERPC_TOC16,       /* type */                                 
	 0,	                /* rightshift */                           
	 1,	                /* size (0 = byte, 1 = short, 2 = long) */ 
	 16,	                /* bitsize */                   
	 false,	                /* pc_relative */                          
	 0,	                /* bitpos */                               
	 complain_overflow_signed, /* complain_on_overflow */
	 powerpc_elf_toc16_reloc, /* special_function */                     
	 "R_POWERPC_TOC16",     /* name */                                 
	 true,	                /* partial_inplace */                      
	 0xffff,	        /* src_mask */                             
	 0xffff,        	/* dst_mask */                             
	 false),                /* pcrel_offset */
};

/* Handle the TOC16 reloc.  We want to use the offset within the .toc
   section, not the actual VMA.  */

static bfd_reloc_status_type
powerpc_elf_toc16_reloc (abfd,
			 reloc_entry,
			 symbol,
			 data,
			 input_section,
			 output_bfd,
			 error_message)
     bfd *abfd;
     arelent *reloc_entry;
     asymbol *symbol;
     PTR data;
     asection *input_section;
     bfd *output_bfd;
     char **error_message;
{
  asection *sec;

  /* Simply adjust the reloc addend by the negative of the VMA of the
     .toc section.  */
  sec = bfd_get_section (*reloc_entry->sym_ptr_ptr);
  if (sec == &bfd_und_section)
    return bfd_reloc_continue;
  BFD_ASSERT (strcmp (bfd_get_section_name (abfd, sec), ".toc") == 0);
  reloc_entry->addend -= sec->output_section->vma;
  return bfd_reloc_continue;
}

/* Handle a branch.  If the next instruction is the special
   instruction "cror 31,31,31", and this branch is to a stub routine
   we have created, we must change the cror instruction into an
   instruction which reloads the TOC register.  We can detect a stub
   routine because it is in a BFD named "linker stubs".  FIXME: What a
   hack.  */

static bfd_reloc_status_type
powerpc_elf_b26_reloc (abfd,
		       reloc_entry,
		       symbol,
		       data,
		       input_section,
		       output_bfd,
		       error_message)
     bfd *abfd;
     arelent *reloc_entry;
     asymbol *symbol;
     PTR data;
     asection *input_section;
     bfd *output_bfd;
     char **error_message;
{
  if (bfd_get_section (symbol) != &bfd_und_section
      && (strcmp (bfd_get_section (symbol)->owner->filename, "linker stubs")
	  == 0))
    {
      bfd_vma val;

      /* This symbol is a stub created by the linker.  */
      val = bfd_get_32 (abfd, (bfd_byte *) data + reloc_entry->address + 4);
      if (val == 0x4ffffb82)			/* cror 31,31,31 */
	bfd_put_32 (abfd, (bfd_vma) 0x80410014,	/* l r2,20(r1) */
		    (bfd_byte *) data + reloc_entry->address + 4);
    }

  if (output_bfd != (bfd *) NULL
      && bfd_get_section (symbol) == &bfd_und_section)
    {
      bfd_size_type address;

      /* If we are generating relocateable output, and the symbol is
	 undefined, we just want to adjust the reloc by the amount the
	 section moved.  */
      address = reloc_entry->address;
      reloc_entry->address += input_section->output_offset;
      return _bfd_relocate_contents (reloc_entry->howto, abfd,
				     - input_section->output_offset,
				     (bfd_byte *) data + address);
    }

  /* Otherwise, let bfd_perform_relocation handle it.  */
  return bfd_reloc_continue;
}

/* Map BFD reloc types to PowerPC ELF reloc types.  */

struct powerpc_reloc_map
{
  unsigned char bfd_reloc_val;
  unsigned char elf_reloc_val;
};

static const struct powerpc_reloc_map powerpc_reloc_map[] =
{
  { BFD_RELOC_NONE, R_POWERPC_NONE, },
  { BFD_RELOC_32, R_POWERPC_32 },
  { BFD_RELOC_CTOR, R_POWERPC_32 },
  { BFD_RELOC_PPC_B26, R_POWERPC_B26 },
  { BFD_RELOC_PPC_BA26, R_POWERPC_BA26 },
  { BFD_RELOC_PPC_TOC16, R_POWERPC_TOC16 }
};

static const struct reloc_howto_struct *
bfd_elf32_bfd_reloc_type_lookup (abfd, code)
     bfd *abfd;
     bfd_reloc_code_real_type code;
{
  int i;

  for (i = 0;
       i < sizeof (powerpc_reloc_map) / sizeof (struct powerpc_reloc_map);
       i++)
    {
      if (powerpc_reloc_map[i].bfd_reloc_val == code)
	return &elf_powerpc_howto_table[powerpc_reloc_map[i].elf_reloc_val];
    }

  return NULL;
}

/* Set the howto pointer for a PowerPC ELF reloc.  */

static void
powerpc_info_to_howto_rel (abfd, cache_ptr, dst)
     bfd *abfd;
     arelent *cache_ptr;
     Elf32_Internal_Rel *dst;
{
  BFD_ASSERT (ELF32_R_TYPE (dst->r_info) < (unsigned int) R_POWERPC_max);
  cache_ptr->howto = &elf_powerpc_howto_table[ELF32_R_TYPE(dst->r_info)];
}

#define TARGET_BIG_SYM		bfd_elf32_powerpc_vec
#define TARGET_BIG_NAME		"elf32-powerpc"
#define ELF_ARCH		bfd_arch_powerpc
#define ELF_MACHINE_CODE	EM_CYGNUS_POWERPC
#define ELF_MAXPAGESIZE		0x10000
#define elf_info_to_howto	0
#define elf_info_to_howto_rel	powerpc_info_to_howto_rel

#include "elf32-target.h"
