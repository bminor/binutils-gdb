/* MIPS ELF support for BFD.
   Copyright (C) 1993 Free Software Foundation, Inc.

   By Ian Lance Taylor, Cygnus Support, <ian@cygnus.com>, from
   information in the System V Application Binary Interface, MIPS
   Processor Supplement.

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

/* This file holds definitions specific to the MIPS ELF ABI.  Note
   that most of this is not actually implemented by BFD.  */

/* Processor specific flags for the ELF header e_flags field.  */

/* At least one .noreorder directive appears in the source.  */
#define EF_MIPS_NOREORDER	0x00000001

/* File contains position independent code.  */
#define EF_MIPS_PIC		0x00000002

/* Code in file uses the standard calling sequence for calling
   position independent code.  */
#define EF_MIPS_CPIC		0x00000004

/* Four bit MIPS architecture field.  */
#define EF_MIPS_ARCH		0xf0000000

/* Processor specific section indices.  These sections do not actually
   exist.  Symbols with a st_shndx field corresponding to one of these
   values have a special meaning.  */

/* Defined and allocated common symbol.  Value is virtual address.  If
   relocated, alignment must be preserved.  */
#define SHN_MIPS_ACOMMON	0xff00

/* Small common symbol.  */
#define SHN_MIPS_SCOMMON	0xff03

/* Small undefined symbol.  */
#define SHN_MIPS_SUNDEFINED	0xff04

/* Processor specific section types.  */

/* Section contains the set of dynamic shared objects used when
   statically linking.  */
#define SHT_MIPS_LIBLIST	0x70000000

/* Section contains list of symbols whose definitions conflict with
   symbols defined in shared objects.  */
#define SHT_MIPS_CONFLICT	0x70000002

/* Section contains the global pointer table.  */
#define SHT_MIPS_GPTAB		0x70000003

/* Section contains microcode information.  The exact format is
   unspecified.  */
#define SHT_MIPS_UCODE		0x70000004

/* Section contains some sort of debugging information.  The exact
   format is unspecified.  It's probably ECOFF symbols.  */
#define SHT_MIPS_DEBUG		0x70000005

/* Section contains register usage information.  */
#define SHT_MIPS_REGINFO	0x70000006

/* A section of type SHT_MIPS_LIBLIST contains an array of the
   following structure.  The sh_link field is the section index of the
   string table.  The sh_info field is the number of entries in the
   section.  */
typedef struct
{
  /* String table index for name of shared object.  */
  Elf32_Word l_name;
  /* Time stamp.  */
  Elf32_Word l_time_stamp;
  /* Checksum of symbol names and common sizes.  */
  Elf32_Word l_checksum;
  /* String table index for version.  */
  Elf32_Word l_version;
  /* Flags.  */
  Elf32_Word l_flags;
} Elf32_Lib;

/* The l_flags field of an Elf32_Lib structure may contain the
   following flags.  */

/* Require an exact match at runtime.  */
#define LL_EXACT_MATCH		0x00000001

/* Ignore version incompatibilities at runtime.  */
#define LL_IGNORE_INT_VER	0x00000002

/* A section of type SHT_MIPS_CONFLICT is an array of indices into the
   .dynsym section.  Each element has the following type.  */
typedef Elf32_Addr Elf32_Conflict;

/* A section of type SHT_MIPS_GPTAB contains information about how
   much GP space would be required for different -G arguments.  This
   information is only used so that the linker can provide informative
   suggestions as to the best -G value to use.  The sh_info field is
   the index of the section for which this information applies.  The
   contents of the section are an array of the following union.  The
   first element uses the gt_header field.  The remaining elements use
   the gt_entry field.  */
typedef union
{
  struct
    {
      /* -G value actually used for this object file.  */
      Elf32_Word gt_current_g_value;
      /* Unused.  */
      Elf32_Word gt_unused;
    } gt_header;
  struct
    {
      /* If this -G argument has been used...  */
      Elf32_Word gt_g_value;
      /* ...this many GP section bytes would be required.  */
      Elf32_Word gt_bytes;
    } gt_entry;
} Elf32_gptab;

/* A section of type SHT_MIPS_REGINFO contains the following
   structure.  */
typedef struct
{
  /* Mask of general purpose registers used.  */
  Elf32_Word ri_gprmask;
  /* Mask of co-processor registers used.  */
  Elf32_Word ri_cprmask[4];
  /* GP register value for this object file.  */
  Elf32_SWord ri_gp_value;
} Elf_RegInfo;

/* Processor specific section flags.  */

/* This section must be in the global data area.  */
define SHF_MIPS_GPREL		0x10000000
