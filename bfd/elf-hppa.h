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

#if ARCH_SIZE == 64
#define ELF_R_TYPE(X)   ELF64_R_TYPE(X)
#define _bfd_elf_hppa_gen_reloc_type _bfd_elf64_hppa_gen_reloc_type
#endif
#if ARCH_SIZE == 32
#define ELF_R_TYPE(X)   ELF32_R_TYPE(X)
#define _bfd_elf_hppa_gen_reloc_type _bfd_elf32_hppa_gen_reloc_type
#endif

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

