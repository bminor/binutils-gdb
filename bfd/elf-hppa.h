/* Common code for PA ELF implementations.
   Copyright (C) 1999 Free Software Foundation, Inc.

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

#define ELF_HOWTO_TABLE_SIZE       R_PARISC_UNIMPLEMENTED + 1

/* This file is included by multiple PA ELF BFD backends with different
   sizes.

   Most of the routines are written to be size independent, but sometimes
   external constraints require 32 or 64 bit specific code.  We remap
   the definitions/functions as necessary here.  */
#if ARCH_SIZE == 64
#define ELF_R_TYPE(X)   ELF64_R_TYPE(X)
#define ELF_R_SYM(X)   ELF64_R_SYM(X)
#define _bfd_elf_hppa_gen_reloc_type _bfd_elf64_hppa_gen_reloc_type
#define elf_hppa_relocate_section elf64_hppa_relocate_section
#define bfd_elf_bfd_final_link bfd_elf64_bfd_final_link
#define elf_hppa_final_link elf64_hppa_final_link
#endif
#if ARCH_SIZE == 32
#define ELF_R_TYPE(X)   ELF32_R_TYPE(X)
#define ELF_R_SYM(X)   ELF32_R_SYM(X)
#define _bfd_elf_hppa_gen_reloc_type _bfd_elf32_hppa_gen_reloc_type
#define elf_hppa_relocate_section elf32_hppa_relocate_section
#define bfd_elf_bfd_final_link bfd_elf32_bfd_final_link
#define elf_hppa_final_link elf32_hppa_final_link
#endif

static boolean
elf_hppa_relocate_section
  PARAMS ((bfd *, struct bfd_link_info *, bfd *, asection *,
           bfd_byte *, Elf_Internal_Rela *, Elf_Internal_Sym *, asection **));

static bfd_reloc_status_type elf_hppa_final_link_relocate
  PARAMS ((reloc_howto_type *, bfd *, bfd *, asection *,
           bfd_byte *, bfd_vma, bfd_vma, bfd_vma, struct bfd_link_info *,
           asection *, const char *, int));

static unsigned long elf_hppa_relocate_insn
  PARAMS ((bfd *, asection *, unsigned long, unsigned long, long,
           long, unsigned long, unsigned long, unsigned long));

static boolean elf_hppa_add_symbol_hook
  PARAMS ((bfd *, struct bfd_link_info *, const Elf_Internal_Sym *,
	   const char **, flagword *, asection **, bfd_vma *));

static boolean elf_hppa_final_link
  PARAMS ((bfd *, struct bfd_link_info *));

/* ELF/PA relocation howto entries.  */

static reloc_howto_type elf_hppa_howto_table[ELF_HOWTO_TABLE_SIZE] =
{
  {R_PARISC_NONE, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_NONE", false, 0, 0, false},

  /* The values in DIR32 are to placate the check in
     _bfd_stab_section_find_nearest_line.  */
  {R_PARISC_DIR32, 0, 2, 32, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_DIR32", false, 0, 0xffffffff, false},
  {R_PARISC_DIR21L, 0, 0, 21, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_DIR21L", false, 0, 0, false},
  {R_PARISC_DIR17R, 0, 0, 17, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_DIR17R", false, 0, 0, false},
  {R_PARISC_DIR17F, 0, 0, 17, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_DIR17F", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_DIR14R, 0, 0, 14, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_DIR14R", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_PCREL32, 0, 0, 32, true, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_PCREL32", false, 0, 0, false},

  {R_PARISC_PCREL21L, 0, 0, 21, true, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_PCREL21L", false, 0, 0, false},
  {R_PARISC_PCREL17R, 0, 0, 17, true, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_PCREL17R", false, 0, 0, false},
  {R_PARISC_PCREL17F, 0, 0, 17, true, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_PCREL17F", false, 0, 0, false},
  {R_PARISC_PCREL17C, 0, 0, 17, true, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_PCREL17C", false, 0, 0, false},
  {R_PARISC_PCREL14R, 0, 0, 14, true, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_PCREL14R", false, 0, 0, false},
  {R_PARISC_PCREL14F, 0, 0, 14, true, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_PCREL14F", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_DPREL21L, 0, 0, 21, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_DPREL21L", false, 0, 0, false},
  {R_PARISC_DPREL14WR, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_DPREL14WR", false, 0, 0, false},

  {R_PARISC_DPREL14DR, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_DPREL14DR", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_DPREL14R, 0, 0, 14, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_DPREL14R", false, 0, 0, false},
  {R_PARISC_DPREL14F, 0, 0, 14, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_DPREL14F", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_DLTREL21L, 0, 0, 21, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_DLTREL21L", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},

  {R_PARISC_DLTREL14R, 0, 0, 14, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_DLTREL14R", false, 0, 0, false},
  {R_PARISC_DLTREL14F, 0, 0, 14, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_DLTREL14F", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_DLTIND21L, 0, 0, 21, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_DLTIND21L", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_DLTIND14R, 0, 0, 14, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_DLTIND14R", false, 0, 0, false},
  {R_PARISC_DLTIND14F, 0, 0, 14, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_DLTIND14F", false, 0, 0, false},

  {R_PARISC_SETBASE, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_SETBASE", false, 0, 0, false},
  {R_PARISC_SECREL32, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_SECREL32", false, 0, 0, false},
  {R_PARISC_BASEREL21L, 0, 0, 21, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_BASEREL21L", false, 0, 0, false},
  {R_PARISC_BASEREL17R, 0, 0, 17, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_BASEREL17R", false, 0, 0, false},
  {R_PARISC_BASEREL17F, 0, 0, 17, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_BASEREL17F", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_BASEREL14R, 0, 0, 14, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_BASEREL14R", false, 0, 0, false},
  {R_PARISC_BASEREL14F, 0, 0, 14, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_BASEREL14F", false, 0, 0, false},
  {R_PARISC_SEGBASE, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_SEGBASE", false, 0, 0, false},
  {R_PARISC_SEGREL32, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_SEGREL32", false, 0, 0, false},

  {R_PARISC_PLTOFF21L, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_PLTOFF21L", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_PLTOFF14R, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_PLTOFF14R", false, 0, 0, false},
  {R_PARISC_PLTOFF14F, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_PLTOFF14F", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_LTOFF_FPTR32, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_LTOFF_FPTR32", false, 0, 0, false},
  {R_PARISC_LTOFF_FPTR21L, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_LTOFF_FPTR21L", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},

  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_LTOFF_FPTR14R, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_LTOFF_FPTR14R", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_FPTR64, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_FPTR64", false, 0, 0, false},
  {R_PARISC_PLABEL32, 0, 0, 32, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_PLABEL32", false, 0, 0, false},
  {R_PARISC_PLABEL21L, 0, 0, 21, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_PLABEL21L", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},

  {R_PARISC_PLABEL14R, 0, 0, 14, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_PLABEL14R", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_PCREL64, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_PCREL64", false, 0, 0, false},
  {R_PARISC_PCREL22C, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_PCREL22C", false, 0, 0, false},
  {R_PARISC_PCREL22F, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_PCREL22F", false, 0, 0, false},
  {R_PARISC_PCREL14WR, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_PCREL14WR", false, 0, 0, false},
  {R_PARISC_PCREL14DR, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_PCREL14DR", false, 0, 0, false},
  {R_PARISC_PCREL16F, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_PCREL16F", false, 0, 0, false},
  {R_PARISC_PCREL16WF, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_PCREL16WF", false, 0, 0, false},
  {R_PARISC_PCREL16DF, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_PCREL16DF", false, 0, 0, false},

  {R_PARISC_DIR64, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_DIR64", false, 0, 0, false},
  {R_PARISC_DIR64WR, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_DIR64WR", false, 0, 0, false},
  {R_PARISC_DIR64DR, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_DIR64DR", false, 0, 0, false},
  {R_PARISC_DIR14WR, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_DIR14WR", false, 0, 0, false},
  {R_PARISC_DIR14DR, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_DIR14DR", false, 0, 0, false},
  {R_PARISC_DIR16F, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_DIR16F", false, 0, 0, false},
  {R_PARISC_DIR16WF, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_DIR16WF", false, 0, 0, false},
  {R_PARISC_DIR16DF, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_DIR16DF", false, 0, 0, false},
  {R_PARISC_GPREL64, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_GPREL64", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},

  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_DLTREL14WR, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_DLTREL14WR", false, 0, 0, false},
  {R_PARISC_DLTREL14DR, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_DLTREL14DR", false, 0, 0, false},
  {R_PARISC_GPREL16F, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_GPREL16F", false, 0, 0, false},
  {R_PARISC_GPREL16WF, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_GPREL16WF", false, 0, 0, false},
  {R_PARISC_GPREL16DF, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_GPREL16DF", false, 0, 0, false},
  {R_PARISC_LTOFF64, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_LTOFF64", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_DLTIND14WR, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_DLTIND14WR", false, 0, 0, false},

  {R_PARISC_DLTIND14DR, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_DLTIND14DR", false, 0, 0, false},
  {R_PARISC_LTOFF16F, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_LTOFF16F", false, 0, 0, false},
  {R_PARISC_LTOFF16DF, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_LTOFF16DF", false, 0, 0, false},
  {R_PARISC_SECREL64, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_SECREL64", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_BASEREL14WR, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_BSEREL14WR", false, 0, 0, false},
  {R_PARISC_BASEREL14DR, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_BASEREL14DR", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},

  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_SEGREL64, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_SEGREL64", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_PLTOFF14WR, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_PLTOFF14WR", false, 0, 0, false},
  {R_PARISC_PLTOFF14DR, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_PLTOFF14DR", false, 0, 0, false},
  {R_PARISC_PLTOFF16F, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_PLTOFF16F", false, 0, 0, false},
  {R_PARISC_PLTOFF16WF, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_PLTOFF16WF", false, 0, 0, false},
  {R_PARISC_PLTOFF16DF, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_PLTOFF16DF", false, 0, 0, false},

  {R_PARISC_LTOFF_FPTR64, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_LTOFF_FPTR14WR, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_LTOFF_FPTR14WR", false, 0, 0, false},
  {R_PARISC_LTOFF_FPTR14DR, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_LTOFF_FPTR14DR", false, 0, 0, false},
  {R_PARISC_LTOFF_FPTR16F, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_LTOFF_FPTR16F", false, 0, 0, false},
  {R_PARISC_LTOFF_FPTR16WF, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_LTOFF_FPTR16WF", false, 0, 0, false},
  {R_PARISC_LTOFF_FPTR16DF, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_COPY, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_COPY", false, 0, 0, false},
  {R_PARISC_IPLT, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_IPLT", false, 0, 0, false},

  {R_PARISC_EPLT, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_EPLT", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},

  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},

  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_TPREL32, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_TPREL32", false, 0, 0, false},
  {R_PARISC_TPREL21L, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_TPREL21L", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_TPREL14R, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_TPREL14R", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},

  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_LTOFF_TP21L, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_LTOFF_TP21L", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_LTOFF_TP14R, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_LTOFF_TP14F, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_LTOFF_TP14F", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},

  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},

  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},

  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},

  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},

  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_TPREL64, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_TPREL64", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_TPREL14WR, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_TPREL14WR", false, 0, 0, false},

  {R_PARISC_TPREL14DR, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_TPREL14DR", false, 0, 0, false},
  {R_PARISC_TPREL16F, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_TPREL16F", false, 0, 0, false},
  {R_PARISC_TPREL16WF, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_TPREL16WF", false, 0, 0, false},
  {R_PARISC_TPREL16DF, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_TPREL16DF", false, 0, 0, false},
  {R_PARISC_LTOFF_TP64, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_LTOFF_TP64", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_UNIMPLEMENTED", false, 0, 0, false},
  {R_PARISC_LTOFF_TP14WR, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_LTOFF_TP14WR", false, 0, 0, false},
  {R_PARISC_LTOFF_TP14DR, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_LTOFF_TP14DR", false, 0, 0, false},
  {R_PARISC_LTOFF_TP16F, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_LTOFF_TP16F", false, 0, 0, false},

  {R_PARISC_LTOFF_TP16WF, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_LTOFF_TP16WF", false, 0, 0, false},
  {R_PARISC_LTOFF_TP16DF, 0, 0, 0, false, 0, complain_overflow_bitfield, bfd_elf_generic_reloc, "R_PARISC_LTOFF_TP16DF", false, 0, 0, false},
};

#define OFFSET_14R_FROM_21L 4
#define OFFSET_14F_FROM_21L 5

/* Return one (or more) BFD relocations which implement the base
   relocation with modifications based on format and field.  */

elf_hppa_reloc_type **
_bfd_elf_hppa_gen_reloc_type (abfd, base_type, format, field, ignore, sym)
     bfd *abfd;
     elf_hppa_reloc_type base_type;
     int format;
     int field;
     int ignore ATTRIBUTE_UNUSED;
     asymbol *sym ATTRIBUTE_UNUSED;
{
  elf_hppa_reloc_type *finaltype;
  elf_hppa_reloc_type **final_types;

  /* Allocate slots for the BFD relocation.  */
  final_types = ((elf_hppa_reloc_type **)
		 bfd_alloc (abfd, sizeof (elf_hppa_reloc_type *) * 2));
  if (final_types == NULL)
    return NULL;

  /* Allocate space for the relocation itself.  */
  finaltype = ((elf_hppa_reloc_type *)
	       bfd_alloc (abfd, sizeof (elf_hppa_reloc_type)));
  if (finaltype == NULL)
    return NULL;

  /* Some reasonable defaults.  */
  final_types[0] = finaltype;
  final_types[1] = NULL;

#define final_type finaltype[0]

  final_type = base_type;

  /* Just a tangle of nested switch statements to deal with the braindamage
     that a different field selector means a completely different relocation
     for PA ELF.  */
  switch (base_type)
    {
    /* We have been using generic relocation types.  However, that may not
       really make sense.  Anyway, we need to support both R_PARISC_DIR64
       and R_PARISC_DIR32 here.  */
    case R_PARISC_DIR32:
    case R_PARISC_DIR64:
    case R_HPPA_ABS_CALL:
      switch (format)
	{
	case 14:
	  switch (field)
	    {
	    case e_rsel:
	    case e_rrsel:
	      final_type = R_PARISC_DIR14R;
	      break;
	    case e_rtsel:
	      final_type = R_PARISC_DLTIND14R;
	      break;
	    case e_rtpsel:
	      final_type = R_PARISC_LTOFF_FPTR14DR;
	      break;
	    case e_tsel:
	      final_type = R_PARISC_DLTIND14F;
	      break;
	    case e_rpsel:
	      final_type = R_PARISC_PLABEL14R;
	      break;
	    default:
	      return NULL;
	    }
	  break;

	case 17:
	  switch (field)
	    {
	    case e_fsel:
	      final_type = R_PARISC_DIR17F;
	      break;
	    case e_rsel:
	    case e_rrsel:
	      final_type = R_PARISC_DIR17R;
	      break;
	    default:
	      return NULL;
	    }
	  break;

	case 21:
	  switch (field)
	    {
	    case e_lsel:
	    case e_lrsel:
	      final_type = R_PARISC_DIR21L;
	      break;
	    case e_ltsel:
	      final_type = R_PARISC_DLTIND21L;
	      break;
	    case e_ltpsel:
	      final_type = R_PARISC_LTOFF_FPTR21L;
	      break;
	    case e_lpsel:
	      final_type = R_PARISC_PLABEL21L;
	      break;
	    default:
	      return NULL;
	    }
	  break;

	case 32:
	  switch (field)
	    {
	    case e_fsel:
	      final_type = R_PARISC_DIR32;
	      /* When in 64bit mode, a 32bit relocation is supposed to
		 be a section relative relocation.  Dwarf2 (for example)
		 uses 32bit section relative relocations.  */
	      if (bfd_get_arch_info (abfd)->bits_per_address != 32)
	        final_type = R_PARISC_SECREL32;
	      break;
	    case e_psel:
	      final_type = R_PARISC_PLABEL32;
	      break;
	    default:
	      return NULL;
	    }
	  break;

	case 64:
	  switch (field)
	    {
	    case e_fsel:
	      final_type = R_PARISC_DIR64;
	      break;
	    case e_psel:
	      final_type = R_PARISC_FPTR64;
	      break;
	    default:
	      return NULL;
	    }
	  break;

	default:
	  return NULL;
	}
      break;


    case R_HPPA_GOTOFF:
      switch (format)
	{
	case 14:
	  switch (field)
	    {
	    case e_rsel:
	    case e_rrsel:
	      final_type = base_type + OFFSET_14R_FROM_21L;
	      break;
	    case e_fsel:
	      final_type = base_type + OFFSET_14F_FROM_21L;
	      break;
	    default:
	      return NULL;
	    }
	  break;

	case 21:
	  switch (field)
	    {
	    case e_lrsel:
	    case e_lsel:
	      final_type = base_type;
	      break;
	    default:
	      return NULL;
	    }
	  break;

	default:
	  return NULL;
	}
      break;


    case R_HPPA_PCREL_CALL:
      switch (format)
	{
	case 14:
	  switch (field)
	    {
	    case e_rsel:
	    case e_rrsel:
	      final_type = R_PARISC_PCREL14R;
	      break;
	    case e_fsel:
	      final_type = R_PARISC_PCREL14F;
	      break;
	    default:
	      return NULL;
	    }
	  break;

	case 17:
	  switch (field)
	    {
	    case e_rsel:
	    case e_rrsel:
	      final_type = R_PARISC_PCREL17R;
	      break;
	    case e_fsel:
	      final_type = R_PARISC_PCREL17F;
	      break;
	    default:
	      return NULL;
	    }
	  break;

	case 22:
	  switch (field)
	    {
	    case e_fsel:
	      final_type = R_PARISC_PCREL22F;
	      break;
	    default:
	      return NULL;
	    }
	  break;

	case 21:
	  switch (field)
	    {
	    case e_lsel:
	    case e_lrsel:
	      final_type = R_PARISC_PCREL21L;
	      break;
	    default:
	      return NULL;
	    }
	  break;

	default:
	  return NULL;
	}
      break;

    case R_PARISC_SEGREL32:
    case R_PARISC_SEGBASE:
      /* The defaults are fine for these cases.  */
      break;

    default:
      return NULL;
    }

  return final_types;
}

/* Translate from an elf into field into a howto relocation pointer.  */

static void
elf_hppa_info_to_howto (abfd, bfd_reloc, elf_reloc)
     bfd *abfd ATTRIBUTE_UNUSED;
     arelent *bfd_reloc;
     Elf_Internal_Rela *elf_reloc;
{
  BFD_ASSERT (ELF_R_TYPE(elf_reloc->r_info)
	      < (unsigned int) R_PARISC_UNIMPLEMENTED);
  bfd_reloc->howto = &elf_hppa_howto_table[ELF_R_TYPE (elf_reloc->r_info)];
}

/* Translate from an elf into field into a howto relocation pointer.  */

static void
elf_hppa_info_to_howto_rel (abfd, bfd_reloc, elf_reloc)
     bfd *abfd ATTRIBUTE_UNUSED;
     arelent *bfd_reloc;
     Elf_Internal_Rel *elf_reloc;
{
  BFD_ASSERT (ELF_R_TYPE(elf_reloc->r_info)
	      < (unsigned int) R_PARISC_UNIMPLEMENTED);
  bfd_reloc->howto = &elf_hppa_howto_table[ELF_R_TYPE (elf_reloc->r_info)];
}

/* Return the address of the howto table entry to perform the CODE
   relocation for an ARCH machine.  */

static reloc_howto_type *
elf_hppa_reloc_type_lookup (abfd, code)
     bfd *abfd ATTRIBUTE_UNUSED;
     bfd_reloc_code_real_type code;
{
  if ((int) code < (int) R_PARISC_UNIMPLEMENTED)
    {
      BFD_ASSERT ((int) elf_hppa_howto_table[(int) code].type == (int) code);
      return &elf_hppa_howto_table[(int) code];
    }
  return NULL;
}

static void
elf_hppa_final_write_processing (abfd, linker)
     bfd *abfd;
     boolean linker ATTRIBUTE_UNUSED;
{
  int mach = bfd_get_mach (abfd);

  elf_elfheader (abfd)->e_flags &= ~(EF_PARISC_ARCH | EF_PARISC_TRAPNIL
				     | EF_PARISC_EXT | EF_PARISC_LSB
				     | EF_PARISC_WIDE | EF_PARISC_NO_KABP
				     | EF_PARISC_LAZYSWAP);

  if (mach == 10)
    elf_elfheader (abfd)->e_flags |= EFA_PARISC_1_0;
  else if (mach == 11)
    elf_elfheader (abfd)->e_flags |= EFA_PARISC_1_1;
  else if (mach == 20)
    elf_elfheader (abfd)->e_flags |= EFA_PARISC_2_0;
  else if (mach == 25)
    elf_elfheader (abfd)->e_flags |= EF_PARISC_WIDE | EFA_PARISC_2_0;
}

/* Return true if SYM represents a local label symbol.  */

static boolean
elf_hppa_is_local_label_name (abfd, name)
     bfd *abfd ATTRIBUTE_UNUSED;
     const char *name;
{
  return (name[0] == 'L' && name[1] == '$');
}

/* Set the correct type for an ELF section.  We do this by the
   section name, which is a hack, but ought to work.  */

static boolean
elf_hppa_fake_sections (abfd, hdr, sec)
     bfd *abfd;
     Elf64_Internal_Shdr *hdr;
     asection *sec;
{
  register const char *name;

  name = bfd_get_section_name (abfd, sec);

  if (strcmp (name, ".PARISC.unwind") == 0)
    {
      int indx;
      asection *sec;
      hdr->sh_type = SHT_LOPROC + 1;
      /* ?!? How are unwinds supposed to work for symbols in arbitrary
	 sections?  Or what if we have multiple .text sections in a single
	 .o file?  HP really messed up on this one. 

	 Ugh.  We can not use elf_section_data (sec)->this_idx at this
	 point because it is not initialized yet.

	 So we (gasp) recompute it here.  Hopefully nobody ever changes the
	 way sections are numbered in elf.c!  */
      for (sec = abfd->sections, indx = 1; sec; sec = sec->next, indx++)
	{
	  if (sec->name && strcmp (sec->name, ".text") == 0)
	    {
	      hdr->sh_info = indx;
	      break;
	    }
	}
      
      /* I have no idea if this is really necessary or what it means.  */
      hdr->sh_entsize = 4;
    }
  return true;
}

/* Hook called by the linker routine which adds symbols from an object
   file.  HP's libraries define symbols with HP specific section
   indices, which we have to handle.  */

static boolean
elf_hppa_add_symbol_hook (abfd, info, sym, namep, flagsp, secp, valp)
     bfd *abfd;
     struct bfd_link_info *info ATTRIBUTE_UNUSED;
     const Elf_Internal_Sym *sym;
     const char **namep ATTRIBUTE_UNUSED;
     flagword *flagsp ATTRIBUTE_UNUSED;
     asection **secp;
     bfd_vma *valp;
{
  int index = sym->st_shndx;
  
  switch (index)
    {
    case SHN_PARISC_ANSI_COMMON:
      *secp = bfd_make_section_old_way (abfd, ".PARISC.ansi.common");
      (*secp)->flags |= SEC_IS_COMMON;
      *valp = sym->st_size;
      break;
      
    case SHN_PARISC_HUGE_COMMON:
      *secp = bfd_make_section_old_way (abfd, ".PARISC.huge.common");
      (*secp)->flags |= SEC_IS_COMMON;
      *valp = sym->st_size;
      break;
    }

  return true;
}

/* Called after we have seen all the input files/sections, but before
   final symbol resolution and section placement has been determined.

   We use this hook to (possibly) provide a value for __gp, then we
   fall back to the generic ELF final link routine.  */

static boolean
elf_hppa_final_link (abfd, info)
     bfd *abfd;
     struct bfd_link_info *info;
{
  /* Make sure we've got ourselves a suitable __gp value.  */
  if (!info->relocateable)
    {
      bfd_vma min_short_vma = (bfd_vma) -1, max_short_vma = 0;
      struct elf_link_hash_entry *gp;
      bfd_vma gp_val = 0;
      asection *os;

      /* Find the min and max vma of all short sections.  */
      for (os = abfd->sections; os ; os = os->next)
	{
	  bfd_vma lo, hi;

	  if ((os->flags & SEC_ALLOC) == 0)
	    continue;

	  lo = os->vma;
	  hi = os->vma + os->_raw_size;
	  if (hi < lo)
	    hi = (bfd_vma) -1;

	  /* This would be cleaner if we marked sections with an attribute
	     indicating they are short sections.  */
	  if (strcmp (os->name, ".sbss") == 0
	      || strcmp (os->name, ".sdata") == 0)
	    {
	      if (min_short_vma > lo)
		min_short_vma = lo;
	      if (max_short_vma < hi)
		max_short_vma = hi;
	    }
	}

      /* See if the user wants to force a value.  */
      gp = elf_link_hash_lookup (elf_hash_table (info), "__gp", false,
				 false, false);

      if (gp
	  && (gp->root.type == bfd_link_hash_defined
	      || gp->root.type == bfd_link_hash_defweak))
	{
	  asection *gp_sec = gp->root.u.def.section;
	  gp_val = (gp->root.u.def.value
		    + gp_sec->output_section->vma
		    + gp_sec->output_offset);
	}
      else if (max_short_vma != 0)
	{
	  /* Pick a sensible value.  */
	  gp_val = min_short_vma;

	  /* If we don't cover all the short data, adjust.  */
	  if (max_short_vma - gp_val >= 0x2000)
	    gp_val = min_short_vma + 0x2000;

	  /* If we're addressing stuff past the end, adjust back.  */
	  if (gp_val > max_short_vma)
	    gp_val = max_short_vma - 0x2000 + 8;

	  /* If there was no __gp symbol, create one.  */
	  if (!gp)
	    gp = elf_link_hash_lookup (elf_hash_table (info), "__gp", true,
				       true, false);

	  /* We now know the value for the global pointer, figure out which
	     section to shove it into and mark it as defined in the hash
	     table.  */
	  for (os = abfd->sections; os ; os = os->next)
	    {
	      bfd_vma low = os->output_offset + os->output_section->vma;
	      bfd_vma high = low + os->_raw_size;

	      if (gp_val >= low && gp_val <= high)
		{
		  gp->root.type = bfd_link_hash_defined;
		  gp->root.u.def.section = os;
		  gp->root.u.def.value = gp_val - low;
		  break;
		}
	    }
	}

      /* Validate whether all short sections are within
	 range of the chosen GP.  */

      if (max_short_vma != 0)
	{
	  if (max_short_vma - min_short_vma >= 0x4000)
	    {
	      (*_bfd_error_handler)
		(_("%s: short data segment overflowed (0x%lx >= 0x4000)"),
		 bfd_get_filename (abfd),
		 (unsigned long)(max_short_vma - min_short_vma));
	      return false;
	    }
	  else if ((gp_val > min_short_vma
		    && gp_val - min_short_vma > 0x2000)
		   || (gp_val < max_short_vma
		       && max_short_vma - gp_val >= 0x2000))
	    {
	      (*_bfd_error_handler)
		(_("%s: __gp does not cover short data segment"),
		 bfd_get_filename (abfd));
	      return false;
	    }
	}

      _bfd_set_gp_value (abfd, gp_val);
    }

  /* Invoke the regular ELF backend linker to do all the work.  */
  return bfd_elf_bfd_final_link (abfd, info);
}

/* Relocate an HPPA ELF section.  */

static boolean
elf_hppa_relocate_section (output_bfd, info, input_bfd, input_section,
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
  Elf_Internal_Rela *rel;
  Elf_Internal_Rela *relend;

  symtab_hdr = &elf_tdata (input_bfd)->symtab_hdr;

  rel = relocs;
  relend = relocs + input_section->reloc_count;
  for (; rel < relend; rel++)
    {
      int r_type;
      reloc_howto_type *howto;
      unsigned long r_symndx;
      struct elf_link_hash_entry *h;
      Elf_Internal_Sym *sym;
      asection *sym_sec;
      bfd_vma relocation;
      bfd_reloc_status_type r;
      const char *sym_name;

      r_type = ELF_R_TYPE (rel->r_info);
      if (r_type < 0 || r_type >= (int) R_PARISC_UNIMPLEMENTED)
	{
	  bfd_set_error (bfd_error_bad_value);
	  return false;
	}
      howto = elf_hppa_howto_table + r_type;

      r_symndx = ELF_R_SYM (rel->r_info);

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
		  sym_sec = local_sections[r_symndx];
		  rel->r_addend += sym_sec->output_offset;
		}
	    }

	  continue;
	}

      /* This is a final link.  */
      h = NULL;
      sym = NULL;
      sym_sec = NULL;
      if (r_symndx < symtab_hdr->sh_info)
	{
	  sym = local_syms + r_symndx;
	  sym_sec = local_sections[r_symndx];
	  relocation = ((ELF_ST_TYPE (sym->st_info) == STT_SECTION
			   ? 0 : sym->st_value)
			 + sym_sec->output_offset
			 + sym_sec->output_section->vma);
	}
      else
	{
	  long indx;

	  indx = r_symndx - symtab_hdr->sh_info;
	  h = elf_sym_hashes (input_bfd)[indx];
	  while (h->root.type == bfd_link_hash_indirect
		 || h->root.type == bfd_link_hash_warning)
	    h = (struct elf_link_hash_entry *) h->root.u.i.link;
	  if (h->root.type == bfd_link_hash_defined
	      || h->root.type == bfd_link_hash_defweak)
	    {
	      sym_sec = h->root.u.def.section;
	      relocation = (h->root.u.def.value
			    + sym_sec->output_offset
			    + sym_sec->output_section->vma);
	    }
	  else if (h->root.type == bfd_link_hash_undefweak)
	    relocation = 0;
	  else
	    {
	      if (!((*info->callbacks->undefined_symbol)
		    (info, h->root.root.string, input_bfd,
		     input_section, rel->r_offset)))
		return false;
	      break;
	    }
	}

      if (h != NULL)
	sym_name = h->root.root.string;
      else
	{
	  sym_name = bfd_elf_string_from_elf_section (input_bfd,
						      symtab_hdr->sh_link,
						      sym->st_name);
	  if (sym_name == NULL)
	    return false;
	  if (*sym_name == '\0')
	    sym_name = bfd_section_name (input_bfd, sym_sec);
	}

      r = elf_hppa_final_link_relocate (howto, input_bfd, output_bfd,
					input_section, contents,
					rel->r_offset, relocation,
					rel->r_addend, info, sym_sec,
					sym_name, h == NULL);

      if (r != bfd_reloc_ok)
	{
	  switch (r)
	    {
	    default:
	      abort ();
	    case bfd_reloc_overflow:
	      {
		if (!((*info->callbacks->reloc_overflow)
		      (info, sym_name, howto->name, (bfd_vma) 0,
			input_bfd, input_section, rel->r_offset)))
		  return false;
	      }
	      break;
	    }
	}
    }
  return true;
}


/* Actually perform a relocation as part of a final link.  */

static bfd_reloc_status_type
elf_hppa_final_link_relocate (howto, input_bfd, output_bfd,
			      input_section, contents, offset, value,
			      addend, info, sym_sec, sym_name, is_local)
     reloc_howto_type *howto;
     bfd *input_bfd;
     bfd *output_bfd ATTRIBUTE_UNUSED;
     asection *input_section;
     bfd_byte *contents;
     bfd_vma offset;
     bfd_vma value;
     bfd_vma addend;
     struct bfd_link_info *info;
     asection *sym_sec;
     const char *sym_name;
     int is_local;
{
  unsigned long insn;
  unsigned long r_type = howto->type;
  unsigned long r_format = howto->bitsize;
  unsigned long r_field = e_fsel;
  bfd_byte *hit_data = contents + offset;
  boolean r_pcrel = howto->pc_relative;

  insn = bfd_get_32 (input_bfd, hit_data);

/* For reference here a quick summary of the relocations found in the
   HPUX 11.00 PA64 .o and .a files, but not yet implemented.  This is mostly
   a guide to help prioritize what relocation support is worked on first.
   The list will be deleted eventually.

   27210 R_PARISC_SEGREL32
   8458 R_PARISC_DLTREL14R
   8284 R_PARISC_DLTIND21L
   8218 R_PARISC_DLTIND14DR
   6675 R_PARISC_FPTR64
   6561 R_PARISC_DLTREL21L
   3974 R_PARISC_DIR64
   3715 R_PARISC_DLTREL14DR
   1584 R_PARISC_LTOFF_FPTR14DR
   1565 R_PARISC_LTOFF_FPTR21L
   1120 R_PARISC_PCREL64
   1096 R_PARISC_LTOFF_TP14DR
   982 R_PARISC_LTOFF_TP21L
   791 R_PARISC_GPREL64
   772 R_PARISC_PLTOFF14DR
   386 R_PARISC_PLTOFF21L
   77 R_PARISC_DLTREL14WR
   6 R_PARISC_LTOFF64
   5 R_PARISC_SEGREL64
   1 R_PARISC_DLTIND14R
   1 R_PARISC_PCREL21L
   1 R_PARISC_PCREL14R */

  switch (r_type)
    {
    case R_PARISC_NONE:
      break;

    case R_PARISC_PCREL22F:
    case R_PARISC_PCREL17F:
      {
	bfd_vma location;
	r_field = e_fsel;

	/* Find out where we are and where we're going.  */
	location = (offset +
		    input_section->output_offset +
		    input_section->output_section->vma);

	insn = elf_hppa_relocate_insn (input_bfd, input_section, insn,
				       offset, value, addend, r_format,
				       r_field, r_pcrel);
	break;
      }

    /* Something we don't know how to handle.  */
    default:
      /* ?!? This is temporary as we flesh out basic linker support, once
	 the basic support is functional we will return the not_supported
	 error conditional appropriately.  */
#if 0
      return bfd_reloc_not_supported;
#else
      return bfd_reloc_ok;
#endif
    }

  /* Update the instruction word.  */
  bfd_put_32 (input_bfd, insn, hit_data);
  return (bfd_reloc_ok);
}

/* Relocate the given INSN given the various input parameters.  */

static unsigned long
elf_hppa_relocate_insn (abfd, input_sect, insn, address, sym_value,
			r_addend, r_format, r_field, pcrel)
     bfd *abfd;
     asection *input_sect;
     unsigned long insn;
     unsigned long address;
     long sym_value;
     long r_addend;
     unsigned long r_format;
     unsigned long r_field;
     unsigned long pcrel;
{
  unsigned char opcode = get_opcode (insn);
  long constant_value;

  switch (opcode)
    {
    /* This is any 17 or 22bit PC-relative branch.  In PA2.0 syntax it
       corresponds to the "B" instruction.  */
    case BL:
      /* Turn SYM_VALUE into a proper PC relative address.  */
      sym_value -= (address + input_sect->output_offset
		    + input_sect->output_section->vma);

      /* Adjust for any field selectors.  */
      sym_value = hppa_field_adjust (sym_value, -8, r_field);

      /* All PC relative branches are implicitly shifted by 2 places.  */
      sym_value >>= 2;

      /* Now determine if this is a 17 or 22 bit branch and take
	 appropriate action.  */
      if (((insn >> 13) & 0x7) == 0x4
	  || ((insn >> 13) & 0x7) == 0x5)
	{
	  unsigned int w3, w2, w1, w;

	  /* These are 22 bit branches.  Mask off bits we do not care
	     about.  */
	  sym_value &= 0x3fffff;

	  /* Now extract the W1, W2, W3 and W fields from the value.  */
	  dis_assemble_22 (sym_value, &w3, &w1, &w2, &w);

	  /* Mask out bits for the value in the instruction.  */
	  insn &= 0xfc00e002;

	  /* Insert the bits for the W1, W2 and W fields into the
	     instruction.  */
	  insn |= (w3 << 21) | (w2 << 2) | (w1 << 16) | w;
	  return insn;
	}
      else
	{
	  unsigned int w2, w1, w;
	  /* These are 17 bit branches.  Mask off bits we do not care
	     about.  */
	  sym_value &= 0x1ffff;

	  /* Now extract the W1, W2 and W fields from the value.  */
	  dis_assemble_17 (sym_value, &w1, &w2, &w);

	  /* Mask out bits for the value in the instruction.  */
	  insn &= 0xffe0e002;

	  /* Insert the bits for the W1, W2 and W fields into the
	     instruction.  */
	  insn |= (w2 << 2) | (w1 << 16) | w;
	  return insn;
	}

    /* This corresponds to any 17 bit absolute branch.  */
    case BE:
    case BLE:
      {
	unsigned int w2, w1, w;

	/* Adjust for any field selectors.  */
	sym_value = hppa_field_adjust (sym_value, 0, r_field);

	/* All absolute branches are implicitly shifted by 2 places.  */
	sym_value >>= 2;

	/* These are 17 bit branches.  Mask off bits we do not care
	   about.  */
	sym_value &= 0x1ffff;

	/* Now extract the W1, W2 and W fields from the value.  */
	dis_assemble_17 (sym_value, &w1, &w2, &w);

	/* Mask out bits for the value in the instruction.  */
	insn &= 0xffe0e002;

	/* Insert the bits for the W1, W2 and W fields into the
	   instruction.  */
	insn |= (w2 << 2) | (w1 << 16) | w;
	return insn;
      }

    default:
      return insn;
    }
}
