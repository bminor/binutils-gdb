/* Intel 80386/80486-specific support for 32-bit ELF
   Copyright 1993 Free Software Foundation, Inc.

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

#define USE_REL	1		/* 386 uses REL relocations instead of RELA */

enum reloc_type
  {
    R_386_NONE = 0,
    R_386_32,
    R_386_PC32,
    R_386_GOT32,
    R_386_PLT32,
    R_386_COPY,
    R_386_GLOB_DAT,
    R_386_JUMP_SLOT,
    R_386_RELATIVE,
    R_386_GOTOFF,
    R_386_GOTPC,
    R_386_max
  };

#if 0
static CONST char *CONST reloc_type_names[] =
{
  "R_386_NONE",
  "R_386_32",
  "R_386_PC32",
  "R_386_GOT32",
  "R_386_PLT32",
  "R_386_COPY",
  "R_386_GLOB_DAT",
  "R_386_JUMP_SLOT",
  "R_386_RELATIVE",
  "R_386_GOTOFF",
  "R_386_GOTPC",
};
#endif

static reloc_howto_type elf_howto_table[]=
{
  HOWTO(R_386_NONE,	 0,0, 0,false,0,complain_overflow_bitfield, bfd_elf_generic_reloc,"R_386_NONE",	    true,0x00000000,0x00000000,false),
  HOWTO(R_386_32,	 0,2,32,false,0,complain_overflow_bitfield, bfd_elf_generic_reloc,"R_386_32",	    true,0xffffffff,0xffffffff,false),
  HOWTO(R_386_PC32,	 0,2,32,true, 0,complain_overflow_bitfield, bfd_elf_generic_reloc,"R_386_PC32",	    true,0xffffffff,0xffffffff,true),
  HOWTO(R_386_GOT32,	 0,2,32,false,0,complain_overflow_bitfield, bfd_elf_generic_reloc,"R_386_GOT32",    true,0xffffffff,0xffffffff,false),
  HOWTO(R_386_PLT32,	 0,2,32,false,0,complain_overflow_bitfield, bfd_elf_generic_reloc,"R_386_PLT32",    true,0xffffffff,0xffffffff,false),
  HOWTO(R_386_COPY,      0,2,32,false,0,complain_overflow_bitfield, bfd_elf_generic_reloc,"R_386_COPY",	    true,0xffffffff,0xffffffff,false),
  HOWTO(R_386_GLOB_DAT,  0,2,32,false,0,complain_overflow_bitfield, bfd_elf_generic_reloc,"R_386_GLOB_DAT", true,0xffffffff,0xffffffff,false),
  HOWTO(R_386_JUMP_SLOT, 0,2,32,false,0,complain_overflow_bitfield, bfd_elf_generic_reloc,"R_386_JUMP_SLOT",true,0xffffffff,0xffffffff,false),
  HOWTO(R_386_RELATIVE,  0,2,32,false,0,complain_overflow_bitfield, bfd_elf_generic_reloc,"R_386_RELATIVE", true,0xffffffff,0xffffffff,false),
  HOWTO(R_386_GOTOFF,    0,2,32,false,0,complain_overflow_bitfield, bfd_elf_generic_reloc,"R_386_GOTOFF",   true,0xffffffff,0xffffffff,false),
  HOWTO(R_386_GOTPC,     0,2,32,false,0,complain_overflow_bitfield, bfd_elf_generic_reloc,"R_386_GOTPC",    true,0xffffffff,0xffffffff,false),
};

#ifdef DEBUG_GEN_RELOC
#define TRACE(str) fprintf (stderr, "i386 bfd reloc lookup %d (%s)\n", code, str)
#else
#define TRACE(str)
#endif

static CONST struct reloc_howto_struct *
DEFUN (elf_i386_reloc_type_lookup, (abfd, code),
       bfd *abfd AND
       bfd_reloc_code_real_type code)
{
  switch (code)
    {
    case BFD_RELOC_NONE:
      TRACE ("BFD_RELOC_NONE");
      return &elf_howto_table[ (int)R_386_NONE ];

    case BFD_RELOC_32:
      TRACE ("BFD_RELOC_32");
      return &elf_howto_table[ (int)R_386_32 ];

    case BFD_RELOC_32_PCREL:
      TRACE ("BFD_RELOC_PC32");
      return &elf_howto_table[ (int)R_386_PC32 ];

    case BFD_RELOC_386_GOT32:
      TRACE ("BFD_RELOC_386_GOT32");
      return &elf_howto_table[ (int)R_386_GOT32 ];

    case BFD_RELOC_386_PLT32:
      TRACE ("BFD_RELOC_386_PLT32");
      return &elf_howto_table[ (int)R_386_PLT32 ];

    case BFD_RELOC_386_COPY:
      TRACE ("BFD_RELOC_386_COPY");
      return &elf_howto_table[ (int)R_386_COPY ];

    case BFD_RELOC_386_GLOB_DAT:
      TRACE ("BFD_RELOC_386_GLOB_DAT");
      return &elf_howto_table[ (int)R_386_GLOB_DAT ];

    case BFD_RELOC_386_JUMP_SLOT:
      TRACE ("BFD_RELOC_386_JUMP_SLOT");
      return &elf_howto_table[ (int)R_386_JUMP_SLOT ];

    case BFD_RELOC_386_RELATIVE:
      TRACE ("BFD_RELOC_386_RELATIVE");
      return &elf_howto_table[ (int)R_386_RELATIVE ];

    case BFD_RELOC_386_GOTOFF:
      TRACE ("BFD_RELOC_386_GOTOFF");
      return &elf_howto_table[ (int)R_386_GOTOFF ];

    case BFD_RELOC_386_GOTPC:
      TRACE ("BFD_RELOC_386_GOTPC");
      return &elf_howto_table[ (int)R_386_GOTPC ];

    default:
      break;
    }

  TRACE ("Unknown");
  return 0;
}

static void
DEFUN(elf_i386_info_to_howto, (abfd, cache_ptr, dst),
      bfd		*abfd AND
      arelent		*cache_ptr AND
      Elf32_Internal_Rela *dst)
{
  BFD_ASSERT (ELF32_R_TYPE(dst->r_info) < (unsigned int) R_386_max);

  cache_ptr->howto = &elf_howto_table[ELF32_R_TYPE(dst->r_info)];
}

static void
DEFUN(elf_i386_info_to_howto_rel, (abfd, cache_ptr, dst),
      bfd		*abfd AND
      arelent		*cache_ptr AND
      Elf32_Internal_Rel *dst)
{
  BFD_ASSERT (ELF32_R_TYPE(dst->r_info) < (unsigned int) R_386_max);

  cache_ptr->howto = &elf_howto_table[ELF32_R_TYPE(dst->r_info)];
}

#define TARGET_LITTLE_SYM		bfd_elf32_i386_vec
#define TARGET_LITTLE_NAME		"elf32-i386"
#define ELF_ARCH			bfd_arch_i386
#define ELF_MACHINE_CODE		EM_386
#define elf_info_to_howto		elf_i386_info_to_howto
#define elf_info_to_howto_rel		elf_i386_info_to_howto_rel
#define bfd_elf32_bfd_reloc_type_lookup	elf_i386_reloc_type_lookup
#define ELF_MAXPAGESIZE			0x1000

#include "elf32-target.h"
