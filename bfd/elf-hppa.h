/* Common code for PA ELF implementations.
   Copyright (C) 1999
   Free Software Foundation, Inc.

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
  {R_PARISC_NONE, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_NONE"},

  /* The values in DIR32 are to placate the check in
     _bfd_stab_section_find_nearest_line.  */
  {R_PARISC_DIR32, 0, 2, 32, false, 0, complain_overflow_bitfield, 0, "R_PARISC_DIR32", false, 0, 0xffffffff, false},
  {R_PARISC_DIR21L, 0, 0, 21, false, 0, complain_overflow_bitfield, 0, "R_PARISC_DIR21L"},
  {R_PARISC_DIR17R, 0, 0, 17, false, 0, complain_overflow_bitfield, 0, "R_PARISC_DIR17R"},
  {R_PARISC_DIR17F, 0, 0, 17, false, 0, complain_overflow_bitfield, 0, "R_PARISC_DIR17F"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_DIR14R, 0, 0, 14, false, 0, complain_overflow_bitfield, 0, "R_PARISC_DIR14R"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_PCREL32, 0, 0, 32, true, 0, complain_overflow_bitfield, 0, "R_PARISC_PCREL32"},

  {R_PARISC_PCREL21L, 0, 0, 21, true, 0, complain_overflow_bitfield, 0, "R_PARISC_PCREL21L"},
  {R_PARISC_PCREL17R, 0, 0, 17, true, 0, complain_overflow_bitfield, 0, "R_PARISC_PCREL17R"},
  {R_PARISC_PCREL17F, 0, 0, 17, true, 0, complain_overflow_bitfield, 0, "R_PARISC_PCREL17F"},
  {R_PARISC_PCREL17C, 0, 0, 17, true, 0, complain_overflow_bitfield, 0, "R_PARISC_PCREL17C"},
  {R_PARISC_PCREL14R, 0, 0, 14, true, 0, complain_overflow_bitfield, 0, "R_PARISC_PCREL14R"},
  {R_PARISC_PCREL14F, 0, 0, 14, true, 0, complain_overflow_bitfield, 0, "R_PARISC_PCREL14F"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_DPREL21L, 0, 0, 21, false, 0, complain_overflow_bitfield, 0, "R_PARISC_DPREL21L"},
  {R_PARISC_DPREL14WR, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_DPREL14WR"},

  {R_PARISC_DPREL14DR, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_DPREL14DR"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_DPREL14R, 0, 0, 14, false, 0, complain_overflow_bitfield, 0, "R_PARISC_DPREL14R"},
  {R_PARISC_DPREL14F, 0, 0, 14, false, 0, complain_overflow_bitfield, 0, "R_PARISC_DPREL14F"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_DLTREL21L, 0, 0, 21, false, 0, complain_overflow_bitfield, 0, "R_PARISC_DLTREL21L"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},

  {R_PARISC_DLTREL14R, 0, 0, 14, false, 0, complain_overflow_bitfield, 0, "R_PARISC_DLTREL14R"},
  {R_PARISC_DLTREL14F, 0, 0, 14, false, 0, complain_overflow_bitfield, 0, "R_PARISC_DLTREL14F"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_DLTIND21L, 0, 0, 21, false, 0, complain_overflow_bitfield, 0, "R_PARISC_DLTIND21L"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_DLTIND14R, 0, 0, 14, false, 0, complain_overflow_bitfield, 0, "R_PARISC_DLTIND14R"},
  {R_PARISC_DLTIND14F, 0, 0, 14, false, 0, complain_overflow_bitfield, 0, "R_PARISC_DLTIND14F"},

  {R_PARISC_SETBASE, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_SETBASE"},
  {R_PARISC_SECREL32, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_SECREL32"},
  {R_PARISC_BASEREL21L, 0, 0, 21, false, 0, complain_overflow_bitfield, 0, "R_PARISC_BASEREL21L"},
  {R_PARISC_BASEREL17R, 0, 0, 17, false, 0, complain_overflow_bitfield, 0, "R_PARISC_BASEREL17R"},
  {R_PARISC_BASEREL17F, 0, 0, 17, false, 0, complain_overflow_bitfield, 0, "R_PARISC_BASEREL17F"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_BASEREL14R, 0, 0, 14, false, 0, complain_overflow_bitfield, 0, "R_PARISC_BASEREL14R"},
  {R_PARISC_BASEREL14F, 0, 0, 14, false, 0, complain_overflow_bitfield, 0, "R_PARISC_BASEREL14F"},
  {R_PARISC_SEGBASE, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_SEGBASE"},
  {R_PARISC_SEGREL32, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_SEGREL32"},

  {R_PARISC_PLTOFF21L, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_PLTOFF21L"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_PLTOFF14R, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_PLTOFF14R"},
  {R_PARISC_PLTOFF14F, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_PLTOFF14F"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_LTOFF_FPTR32, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_LTOFF_FPTR32"},
  {R_PARISC_LTOFF_FPTR21L, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_LTOFF_FPTR21L"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},

  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_LTOFF_FPTR14R, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_LTOFF_FPTR14R"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_FPTR64, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_FPTR64"},
  {R_PARISC_PLABEL32, 0, 0, 32, false, 0, complain_overflow_bitfield, 0, "R_PARISC_PLABEL32"},
  {R_PARISC_PLABEL21L, 0, 0, 21, false, 0, complain_overflow_bitfield, 0, "R_PARISC_PLABEL21L"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},

  {R_PARISC_PLABEL14R, 0, 0, 14, false, 0, complain_overflow_bitfield, 0, "R_PARISC_PLABEL14R"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_PCREL64, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_PCREL64"},
  {R_PARISC_PCREL22C, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_PCREL22C"},
  {R_PARISC_PCREL22F, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_PCREL22F"},
  {R_PARISC_PCREL14WR, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_PCREL14WR"},
  {R_PARISC_PCREL14DR, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_PCREL14DR"},
  {R_PARISC_PCREL16F, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_PCREL16F"},
  {R_PARISC_PCREL16WF, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_PCREL16WF"},
  {R_PARISC_PCREL16DF, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_PCREL16DF"},

  {R_PARISC_DIR64, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_DIR64"},
  {R_PARISC_DIR64WR, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_DIR64WR"},
  {R_PARISC_DIR64DR, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_DIR64DR"},
  {R_PARISC_DIR14WR, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_DIR14WR"},
  {R_PARISC_DIR14DR, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_DIR14DR"},
  {R_PARISC_DIR16F, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_DIR16F"},
  {R_PARISC_DIR16WF, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_DIR16WF"},
  {R_PARISC_DIR16DF, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_DIR16DF"},
  {R_PARISC_GPREL64, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_GPREL64"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},

  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_DLTREL14WR, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_DLTREL14WR"},
  {R_PARISC_DLTREL14DR, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_DLTREL14DR"},
  {R_PARISC_GPREL16F, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_GPREL16F"},
  {R_PARISC_GPREL16WF, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_GPREL16WF"},
  {R_PARISC_GPREL16DF, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_GPREL16DF"},
  {R_PARISC_LTOFF64, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_LTOFF64"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_DLTIND14WR, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_DLTIND14WR"},

  {R_PARISC_DLTIND14DR, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_DLTIND14DR"},
  {R_PARISC_LTOFF16F, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_LTOFF16F"},
  {R_PARISC_LTOFF16DF, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_LTOFF16DF"},
  {R_PARISC_SECREL64, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_SECREL64"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_BASEREL14WR, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_BSEREL14WR"},
  {R_PARISC_BASEREL14DR, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_BASEREL14DR"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},

  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_SEGREL64, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_SEGREL64"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_PLTOFF14WR, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_PLTOFF14WR"},
  {R_PARISC_PLTOFF14DR, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_PLTOFF14DR"},
  {R_PARISC_PLTOFF16F, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_PLTOFF16F"},
  {R_PARISC_PLTOFF16WF, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_PLTOFF16WF"},
  {R_PARISC_PLTOFF16DF, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_PLTOFF16DF"},

  {R_PARISC_LTOFF_FPTR64, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_LTOFF_FPTR14WR, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_LTOFF_FPTR14WR"},
  {R_PARISC_LTOFF_FPTR14DR, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_LTOFF_FPTR14DR"},
  {R_PARISC_LTOFF_FPTR16F, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_LTOFF_FPTR16F"},
  {R_PARISC_LTOFF_FPTR16WF, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_LTOFF_FPTR16WF"},
  {R_PARISC_LTOFF_FPTR16DF, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_COPY, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_COPY"},
  {R_PARISC_IPLT, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_IPLT"},

  {R_PARISC_EPLT, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_EPLT"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},

  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},

  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_TPREL32, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_TPREL32"},
  {R_PARISC_TPREL21L, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_TPREL21L"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_TPREL14R, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_TPREL14R"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_UNIMPLEMENTED"},

  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_LTOFF_TP21L, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_LTOFF_TP21L"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_LTOFF_TP14R, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_LTOFF_TP14F, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_LTOFF_TP14F"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},

  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},

  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},

  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},

  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},

  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_TPREL64, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_TPREL64"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_TPREL14WR, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_TPREL14WR"},

  {R_PARISC_TPREL14DR, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_TPREL14DR"},
  {R_PARISC_TPREL16F, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_TPREL16F"},
  {R_PARISC_TPREL16WF, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_TPREL16WF"},
  {R_PARISC_TPREL16DF, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_TPREL16DF"},
  {R_PARISC_LTOFF_TP64, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_LTOFF_TP64"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_LTOFF_TP14WR, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_LTOFF_TP14WR"},
  {R_PARISC_LTOFF_TP14DR, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_LTOFF_TP14DR"},
  {R_PARISC_LTOFF_TP16F, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_LTOFF_TP16F"},

  {R_PARISC_LTOFF_TP16WF, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_LTOFF_TP16WF"},
  {R_PARISC_LTOFF_TP16DF, 0, 0, 0, false, 0, complain_overflow_bitfield, 0, "R_PARISC_LTOFF_TP16DF"},
};

/* Return one (or more) BFD relocations which implement the base
   relocation with modifications based on format and field.  */

elf_hppa_reloc_type **
_bfd_elf_hppa_gen_reloc_type (abfd, base_type, format, field, ignore, sym)
     bfd *abfd;
     elf_hppa_reloc_type base_type;
     int format;
     int field;
     int ignore;
     asymbol *sym;
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
    case R_HPPA:
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
	      final_type = R_PARISC_DLTREL14R;
	      break;
	    case e_tsel:
	      final_type = R_PARISC_DLTREL14F;
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
	      final_type = R_PARISC_DLTREL21L;
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
	      break;
	    case e_psel:
	      final_type = R_PARISC_PLABEL32;
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
	      final_type = R_PARISC_DPREL14R;
	      break;
	    case e_fsel:
	      final_type = R_PARISC_DPREL14F;
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
	      final_type = R_PARISC_DPREL21L;
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

    default:
      return NULL;
    }

  return final_types;
}

/* Translate from an elf into field into a howto relocation pointer.  */

static void
elf_hppa_info_to_howto (abfd, bfd_reloc, elf_reloc)
     bfd *abfd;
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
     bfd *abfd;
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
     bfd *abfd;
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
     boolean linker;
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
    {
      elf_elfheader (abfd)->e_flags |= EFA_PARISC_2_0;
#if ARCH_SIZE == 64
      elf_elfheader (abfd)->e_flags |= EF_PARISC_WIDE;
#endif
    }

}
