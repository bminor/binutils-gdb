/* MIPS ELF support for BFD.
   Copyright (C) 1993, 1994, 1995, 1996, 1998 Free Software Foundation, Inc.

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

/* This file holds definitions specific to the MIPS ELF ABI.  Note
   that most of this is not actually implemented by BFD.  */

#ifndef _ELF_MIPS_H
#define _ELF_MIPS_H

/* Relocation types.  */
#define R_MIPS_NONE	0
#define R_MIPS_16	1
#define R_MIPS_32	2
#define R_MIPS_REL32	3
#define R_MIPS_26	4
#define R_MIPS_HI16	5
#define R_MIPS_LO16	6
#define R_MIPS_GPREL16	7
#define R_MIPS_LITERAL	8
#define R_MIPS_GOT16	9
#define R_MIPS_PC16	10
#define R_MIPS_CALL16	11
#define R_MIPS_GPREL32	12

/* Processor specific flags for the ELF header e_flags field.  */

/* At least one .noreorder directive appears in the source.  */
#define EF_MIPS_NOREORDER	0x00000001

/* File contains position independent code.  */
#define EF_MIPS_PIC		0x00000002

/* Code in file uses the standard calling sequence for calling
   position independent code.  */
#define EF_MIPS_CPIC		0x00000004

/* Code in file uses new ABI (-n32 on Irix 6).  */
#define EF_MIPS_ABI2		0x00000020

/* Four bit MIPS architecture field.  */
#define EF_MIPS_ARCH		0xf0000000

/* -mips1 code.  */
#define E_MIPS_ARCH_1		0x00000000

/* -mips2 code.  */
#define E_MIPS_ARCH_2		0x10000000

/* -mips3 code.  */
#define E_MIPS_ARCH_3		0x20000000

/* -mips4 code.  */
#define E_MIPS_ARCH_4		0x30000000

/* Machine variant if we know it.  This field was invented at Cygnus,
   but it is hoped that other vendors will adopt it.  If some standard
   is developed, this code should be changed to follow it. */

#define EF_MIPS_MACH		0x00FF0000

/* Cygnus is choosing values between 80 and 9F;
   00 - 7F should be left for a future standard;
   the rest are open. */

#define E_MIPS_MACH_3900	0x00810000

#define E_MIPS_MACH_4010	0x00820000
#define E_MIPS_MACH_4100	0x00830000
/* start-sanitize-vr4320 */
#define E_MIPS_MACH_4320	0x00840000
/* end-sanitize-vr4320 */
#define E_MIPS_MACH_4650	0x00850000
/* start-sanitize-tx49 */
#define E_MIPS_MACH_4900	0x00860000
/* end-sanitize-tx49 */

/* start-sanitize-vr5400 */
#define E_MIPS_MACH_5400	0x00910000
/* end-sanitize-vr5400 */
/* start-sanitize-r5900 */
#define E_MIPS_MACH_5900	0x00920000
/* end-sanitize-r5900 */

/* Processor specific section indices.  These sections do not actually
   exist.  Symbols with a st_shndx field corresponding to one of these
   values have a special meaning.  */

/* Defined and allocated common symbol.  Value is virtual address.  If
   relocated, alignment must be preserved.  */
#define SHN_MIPS_ACOMMON	0xff00

/* Defined and allocated text symbol.  Value is virtual address.
   Occur in the dynamic symbol table of Alpha OSF/1 and Irix 5 executables.  */
#define SHN_MIPS_TEXT		0xff01

/* Defined and allocated data symbol.  Value is virtual address.
   Occur in the dynamic symbol table of Alpha OSF/1 and Irix 5 executables.  */
#define SHN_MIPS_DATA		0xff02

/* Small common symbol.  */
#define SHN_MIPS_SCOMMON	0xff03

/* Small undefined symbol.  */
#define SHN_MIPS_SUNDEFINED	0xff04

/* Processor specific section types.  */

/* Section contains the set of dynamic shared objects used when
   statically linking.  */
#define SHT_MIPS_LIBLIST	0x70000000

/* I'm not sure what this is, but it's used on Irix 5.  */
#define SHT_MIPS_MSYM		0x70000001

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

/* ??? */
#define SHT_MIPS_PACKAGE	0x70000007

/* ??? */
#define SHT_MIPS_PACKSYM	0x70000008

/* ??? */
#define SHT_MIPS_RELD		0x70000009

/* Section contains interface information.  */
#define SHT_MIPS_IFACE		0x7000000b

/* Section contains description of contents of another section.  */
#define SHT_MIPS_CONTENT	0x7000000c

/* Section contains miscellaneous options.  */
#define SHT_MIPS_OPTIONS	0x7000000d

/* ??? */
#define SHT_MIPS_SHDR		0x70000010

/* ??? */
#define SHT_MIPS_FDESC		0x70000011

/* ??? */
#define SHT_MIPS_EXTSYM		0x70000012

/* ??? */
#define SHT_MIPS_DENSE		0x70000013

/* ??? */
#define SHT_MIPS_PDESC		0x70000014

/* ??? */
#define SHT_MIPS_LOCSYM		0x70000015

/* ??? */
#define SHT_MIPS_AUXSYM		0x70000016

/* ??? */
#define SHT_MIPS_OPTSYM		0x70000017

/* ??? */
#define SHT_MIPS_LOCSTR		0x70000018

/* ??? */
#define SHT_MIPS_LINE		0x70000019

/* ??? */
#define SHT_MIPS_RFDESC		0x7000001a

/* ??? */
#define SHT_MIPS_DELTASYM	0x7000001b

/* ??? */
#define SHT_MIPS_DELTAINST	0x7000001c

/* ??? */
#define SHT_MIPS_DELTACLASS	0x7000001d

/* DWARF debugging section.  */
#define SHT_MIPS_DWARF		0x7000001e

/* ??? */
#define SHT_MIPS_DELTADECL	0x7000001f

/* List of libraries the binary depends on.  Includes a time stamp, version
   number.  */
#define SHT_MIPS_SYMBOL_LIB	0x70000020

/* Events section.  */
#define SHT_MIPS_EVENTS		0x70000021

/* ??? */
#define SHT_MIPS_TRANSLATE	0x70000022

/* ??? */
#define SHT_MIPS_PIXIE		0x70000023

/* ??? */
#define SHT_MIPS_XLATE		0x70000024

/* ??? */
#define SHT_MIPS_XLATE_DEBUG	0x70000025

/* ??? */
#define SHT_MIPS_WHIRL		0x70000026

/* ??? */
#define SHT_MIPS_EH_REGION	0x70000027

/* ??? */
#define SHT_MIPS_XLATE_OLD	0x70000028

/* ??? */
#define SHT_MIPS_PDR_EXCEPTION	0x70000029

/* start-sanitize-sky */
/* The VU overlay table.  */
#define SHT_DVP_OVERLAY_TABLE		0x7ffff420
#define SHNAME_DVP_OVERLAY_TABLE	".DVP.ovlytab"
#define SHNAME_DVP_OVERLAY_STRTAB	".DVP.ovlystrtab"
/* A VU overlay.  */
#define SHT_DVP_OVERLAY			0x7ffff421
/* Prefix of VU overlay sections.  */
#define SHNAME_DVP_OVERLAY_PREFIX	".DVP.overlay."
/* end-sanitize-sky */

/* A section of type SHT_MIPS_LIBLIST contains an array of the
   following structure.  The sh_link field is the section index of the
   string table.  The sh_info field is the number of entries in the
   section.  */
typedef struct
{
  /* String table index for name of shared object.  */
  unsigned long l_name;
  /* Time stamp.  */
  unsigned long l_time_stamp;
  /* Checksum of symbol names and common sizes.  */
  unsigned long l_checksum;
  /* String table index for version.  */
  unsigned long l_version;
  /* Flags.  */
  unsigned long l_flags;
} Elf32_Lib;

/* The external version of Elf32_Lib.  */
typedef struct
{
  unsigned char l_name[4];
  unsigned char l_time_stamp[4];
  unsigned char l_checksum[4];
  unsigned char l_version[4];
  unsigned char l_flags[4];
} Elf32_External_Lib;

/* The l_flags field of an Elf32_Lib structure may contain the
   following flags.  */

/* Require an exact match at runtime.  */
#define LL_EXACT_MATCH		0x00000001

/* Ignore version incompatibilities at runtime.  */
#define LL_IGNORE_INT_VER	0x00000002

/* A section of type SHT_MIPS_CONFLICT is an array of indices into the
   .dynsym section.  Each element has the following type.  */
typedef unsigned long Elf32_Conflict;

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
      unsigned long gt_current_g_value;
      /* Unused.  */
      unsigned long gt_unused;
    } gt_header;
  struct
    {
      /* If this -G argument has been used...  */
      unsigned long gt_g_value;
      /* ...this many GP section bytes would be required.  */
      unsigned long gt_bytes;
    } gt_entry;
} Elf32_gptab;

/* The external version of Elf32_gptab.  */

typedef union
{
  struct
    {
      unsigned char gt_current_g_value[4];
      unsigned char gt_unused[4];
    } gt_header;
  struct
    {
      unsigned char gt_g_value[4];
      unsigned char gt_bytes[4];
    } gt_entry;
} Elf32_External_gptab;

/* A section of type SHT_MIPS_REGINFO contains the following
   structure.  */
typedef struct
{
  /* Mask of general purpose registers used.  */
  unsigned long ri_gprmask;
  /* Mask of co-processor registers used.  */
  unsigned long ri_cprmask[4];
  /* GP register value for this object file.  */
  long ri_gp_value;
} Elf32_RegInfo;

/* The external version of the Elf_RegInfo structure.  */
typedef struct
{
  unsigned char ri_gprmask[4];
  unsigned char ri_cprmask[4][4];
  unsigned char ri_gp_value[4];
} Elf32_External_RegInfo;

/* MIPS ELF .reginfo swapping routines.  */
extern void bfd_mips_elf32_swap_reginfo_in
  PARAMS ((bfd *, const Elf32_External_RegInfo *, Elf32_RegInfo *));
extern void bfd_mips_elf32_swap_reginfo_out
  PARAMS ((bfd *, const Elf32_RegInfo *, Elf32_External_RegInfo *));

/* Processor specific section flags.  */

/* This section must be in the global data area.  */
#define SHF_MIPS_GPREL		0x10000000

/* This section should be merged.  */
#define SHF_MIPS_MERGE		0x20000000

/* This section contains 32 bit addresses.  */
#define SHF_MIPS_ADDR32		0x40000000

/* This section contains 64 bit addresses.  */
#define SHF_MIPS_ADDR64		0x80000000

/* This section may not be stripped.  */
#define SHF_MIPS_NOSTRIP	0x08000000

/* This section is local to threads.  */
#define SHF_MIPS_LOCAL		0x04000000

/* Linker should generate implicit weak names for this section.  */
#define SHF_MIPS_NAMES		0x02000000

/* Processor specific program header types.  */

/* Register usage information.  Identifies one .reginfo section.  */
#define PT_MIPS_REGINFO		0x70000000

/* Runtime procedure table.  */
#define PT_MIPS_RTPROC		0x70000001

/* Options (for what ???).  */
#define PT_MIPS_OPTIONS		0x70000002

/* Processor specific dynamic array tags.  */

/* 32 bit version number for runtime linker interface.  */
#define DT_MIPS_RLD_VERSION	0x70000001

/* Time stamp.  */
#define DT_MIPS_TIME_STAMP	0x70000002

/* Checksum of external strings and common sizes.  */
#define DT_MIPS_ICHECKSUM	0x70000003

/* Index of version string in string table.  */
#define DT_MIPS_IVERSION	0x70000004

/* 32 bits of flags.  */
#define DT_MIPS_FLAGS		0x70000005

/* Base address of the segment.  */
#define DT_MIPS_BASE_ADDRESS	0x70000006

/* ??? */
#define DT_MIPS_MSYM		0x70000007

/* Address of .conflict section.  */
#define DT_MIPS_CONFLICT	0x70000008

/* Address of .liblist section.  */
#define DT_MIPS_LIBLIST		0x70000009

/* Number of local global offset table entries.  */
#define DT_MIPS_LOCAL_GOTNO	0x7000000a

/* Number of entries in the .conflict section.  */
#define DT_MIPS_CONFLICTNO	0x7000000b

/* Number of entries in the .liblist section.  */
#define DT_MIPS_LIBLISTNO	0x70000010

/* Number of entries in the .dynsym section.  */
#define DT_MIPS_SYMTABNO	0x70000011

/* Index of first external dynamic symbol not referenced locally.  */
#define DT_MIPS_UNREFEXTNO	0x70000012

/* Index of first dynamic symbol in global offset table.  */
#define DT_MIPS_GOTSYM		0x70000013

/* Number of page table entries in global offset table.  */
#define DT_MIPS_HIPAGENO	0x70000014

/* Address of run time loader map, used for debugging.  */
#define DT_MIPS_RLD_MAP		0x70000016

/* Delta C++ class definition.  */
#define DT_MIPS_DELTA_CLASS	0x70000017

/* Number of entries in DT_MIPS_DELTA_CLASS.  */
#define DT_MIPS_DELTA_CLASS_NO	0x70000018

/* Delta C++ class instances.  */
#define DT_MIPS_DELTA_INSTANCE	0x70000019

/* Number of entries in DT_MIPS_DELTA_INSTANCE.  */
#define DT_MIPS_DELTA_INSTANCE_NO	0x7000001a

/* Delta relocations.  */
#define DT_MIPS_DELTA_RELOC	0x7000001b

/* Number of entries in DT_MIPS_DELTA_RELOC.  */
#define DT_MIPS_DELTA_RELOC_NO	0x7000001c

/* Delta symbols that Delta relocations refer to.  */
#define DT_MIPS_DELTA_SYM	0x7000001d

/* Number of entries in DT_MIPS_DELTA_SYM.  */
#define DT_MIPS_DELTA_SYM_NO	0x7000001e

/* Delta symbols that hold class declarations.  */
#define DT_MIPS_DELTA_CLASSSYM	0x70000020

/* Number of entries in DT_MIPS_DELTA_CLASSSYM.  */
#define DT_MIPS_DELTA_CLASSSYM_NO	0x70000021

/* Flags indicating information about C++ flavor.  */
#define DT_MIPS_CXX_FLAGS	0x70000022

/* Pixie information (???).  */
#define DT_MIPS_PIXIE_INIT	0x70000023

/* ??? */
#define DT_MIPS_SYMBOL_LIB	0x70000024

/* ??? */
#define DT_MIPS_LOCALPAGE_GOTIDX	0x70000025

/* ??? */
#define DT_MIPS_LOCAL_GOTIDX	0x70000026

/* ??? */
#define DT_MIPS_HIDDEN_GOTIDX	0x70000027

/* ??? */
#define DT_MIPS_PROTECTED_GOTIDX	0x70000028

/* Address of `.MIPS.options'.  */
#define DT_MIPS_OPTIONS		0x70000029

/* Address of `.interface'.  */
#define DT_MIPS_INTERFACE	0x7000002a

/* ??? */
#define DT_MIPS_DYNSTR_ALIGN	0x7000002b

/* Size of the .interface section.  */
#define DT_MIPS_INTERFACE_SIZE	0x7000002c

/* Size of rld_text_resolve function stored in the GOT.  */
#define DT_MIPS_RLD_TEXT_RESOLVE_ADDR	0x7000002d

/* Default suffix of DSO to be added by rld on dlopen() calls.  */
#define DT_MIPS_PERF_SUFFIX	0x7000002e

/* Size of compact relocation section (O32).  */
#define DT_MIPS_COMPACT_SIZE	0x7000002f

/* GP value for auxiliary GOTs.  */
#define DT_MIPS_GP_VALUE	0x70000030

/* Address of auxiliary .dynamic.  */
#define DT_MIPS_AUX_DYNAMIC	0x70000031

/* Flags which may appear in a DT_MIPS_FLAGS entry.  */

/* No flags.  */
#define RHF_NONE		0x00000000

/* Uses shortcut pointers.  */
#define RHF_QUICKSTART		0x00000001

/* Hash size is not a power of two.  */
#define RHF_NOTPOT		0x00000002

/* Ignore LD_LIBRARY_PATH.  */
#define RHS_NO_LIBRARY_REPLACEMENT \
				0x00000004

/* Special values for the st_other field in the symbol table.  These
   are used in an Irix 5 dynamic symbol table.  */

#define STO_DEFAULT		0x00
#define STO_INTERNAL		0x01
#define STO_HIDDEN		0x02
#define STO_PROTECTED		0x03

/* start-sanitize-sky */
/* These values are used for the dvp.  */
#define STO_DVP_DMA		0xe8
#define STO_DVP_VIF		0xe9
#define STO_DVP_GIF		0xea
#define STO_DVP_VU		0xeb
/* Reserve a couple in case we need them.  */
#define STO_DVP_RES1		0xec
#define STO_DVP_RES2		0xed
#define STO_DVP_P(sto) ((sto) >= STO_DVP_DMA && (sto) <= STO_DVP_RES2)

/* end-sanitize-sky */
/* This value is used for a mips16 .text symbol.  */
#define STO_MIPS16		0xf0

/* The 64-bit MIPS ELF ABI uses an unusual reloc format.  Each
   relocation entry specifies up to three actual relocations, all at
   the same address.  The first relocation which required a symbol
   uses the symbol in the r_sym field.  The second relocation which
   requires a symbol uses the symbol in the r_ssym field.  If all
   three relocations require a symbol, the third one uses a zero
   value.  */

/* An entry in a 64 bit SHT_REL section.  */

typedef struct
{
  /* Address of relocation.  */
  unsigned char r_offset[8];
  /* Symbol index.  */
  unsigned char r_sym[4];
  /* Special symbol.  */
  unsigned char r_ssym[1];
  /* Third relocation.  */
  unsigned char r_type3[1];
  /* Second relocation.  */
  unsigned char r_type2[1];
  /* First relocation.  */
  unsigned char r_type[1];
} Elf64_Mips_External_Rel;

typedef struct
{
  /* Address of relocation.  */
  bfd_vma r_offset;
  /* Symbol index.  */
  unsigned long r_sym;
  /* Special symbol.  */
  unsigned char r_ssym;
  /* Third relocation.  */
  unsigned char r_type3;
  /* Second relocation.  */
  unsigned char r_type2;
  /* First relocation.  */
  unsigned char r_type;
} Elf64_Mips_Internal_Rel;

/* An entry in a 64 bit SHT_RELA section.  */

typedef struct
{
  /* Address of relocation.  */
  unsigned char r_offset[8];
  /* Symbol index.  */
  unsigned char r_sym[4];
  /* Special symbol.  */
  unsigned char r_ssym[1];
  /* Third relocation.  */
  unsigned char r_type3[1];
  /* Second relocation.  */
  unsigned char r_type2[1];
  /* First relocation.  */
  unsigned char r_type[1];
  /* Addend.  */
  unsigned char r_addend[8];
} Elf64_Mips_External_Rela;

typedef struct
{
  /* Address of relocation.  */
  bfd_vma r_offset;
  /* Symbol index.  */
  unsigned long r_sym;
  /* Special symbol.  */
  unsigned char r_ssym;
  /* Third relocation.  */
  unsigned char r_type3;
  /* Second relocation.  */
  unsigned char r_type2;
  /* First relocation.  */
  unsigned char r_type;
  /* Addend.  */
  bfd_signed_vma r_addend;
} Elf64_Mips_Internal_Rela;

/* Values found in the r_ssym field of a relocation entry.  */

/* No relocation.  */
#define RSS_UNDEF	0

/* Value of GP.  */
#define RSS_GP		1

/* Value of GP in object being relocated.  */
#define RSS_GP0		2

/* Address of location being relocated.  */
#define RSS_LOC		3

/* A SHT_MIPS_OPTIONS section contains a series of options, each of
   which starts with this header.  */

typedef struct
{
  /* Type of option.  */
  unsigned char kind[1];
  /* Size of option descriptor, including header.  */
  unsigned char size[1];
  /* Section index of affected section, or 0 for global option.  */
  unsigned char section[2];
  /* Information specific to this kind of option.  */
  unsigned char info[4];
} Elf_External_Options;

typedef struct
{
  /* Type of option.  */
  unsigned char kind;
  /* Size of option descriptor, including header.  */
  unsigned char size;
  /* Section index of affected section, or 0 for global option.  */
  unsigned short section;
  /* Information specific to this kind of option.  */
  unsigned long info;
} Elf_Internal_Options;

/* MIPS ELF option header swapping routines.  */
extern void bfd_mips_elf_swap_options_in
  PARAMS ((bfd *, const Elf_External_Options *, Elf_Internal_Options *));
extern void bfd_mips_elf_swap_options_out
  PARAMS ((bfd *, const Elf_Internal_Options *, Elf_External_Options *));

/* Values which may appear in the kind field of an Elf_Options
   structure.  */

/* Undefined.  */
#define ODK_NULL	0

/* Register usage and GP value.  */
#define ODK_REGINFO	1

/* Exception processing information.  */
#define ODK_EXCEPTIONS	2

/* Section padding information.  */
#define ODK_PAD		3

/* Hardware workarounds performed.  */
#define ODK_HWPATCH	4

/* Fill value used by the linker.  */
#define ODK_FILL	5

/* Reserved space for desktop tools.  */
#define ODK_TAGS	6

/* Hardware workarounds, AND bits when merging.  */
#define ODK_HWAND	7

/* Hardware workarounds, OR bits when merging.  */
#define ODK_HWOR	8

/* GP group to use for text/data sections.  */
#define ODK_GP_GROUP	9

/* ID information.  */
#define ODK_IDENT	10

/* In the 32 bit ABI, an ODK_REGINFO option is just a Elf32_RegInfo
   structure.  In the 64 bit ABI, it is the following structure.  The
   info field of the options header is not used.  */

typedef struct
{
  /* Mask of general purpose registers used.  */
  unsigned char ri_gprmask[4];
  /* Padding.  */
  unsigned char ri_pad[4];
  /* Mask of co-processor registers used.  */
  unsigned char ri_cprmask[4][4];
  /* GP register value for this object file.  */
  unsigned char ri_gp_value[8];
} Elf64_External_RegInfo;

typedef struct
{
  /* Mask of general purpose registers used.  */
  unsigned long ri_gprmask;
  /* Padding.  */
  unsigned long ri_pad;
  /* Mask of co-processor registers used.  */
  unsigned long ri_cprmask[4];
  /* GP register value for this object file.  */
  bfd_vma ri_gp_value;
} Elf64_Internal_RegInfo;

/* MIPS ELF reginfo swapping routines.  */
extern void bfd_mips_elf64_swap_reginfo_in
  PARAMS ((bfd *, const Elf64_External_RegInfo *, Elf64_Internal_RegInfo *));
extern void bfd_mips_elf64_swap_reginfo_out
  PARAMS ((bfd *, const Elf64_Internal_RegInfo *, Elf64_External_RegInfo *));

/* Masks for the info work of an ODK_EXCEPTIONS descriptor.  */
#define OEX_FPU_MIN	0x1f	/* FPEs which must be enabled.  */
#define OEX_FPU_MAX	0x1f00	/* FPEs which may be enabled.  */
#define OEX_PAGE0	0x10000	/* Page zero must be mapped.  */
#define OEX_SMM		0x20000	/* Force sequential memory mode.  */
#define OEX_FPDBUG	0x40000	/* Force floating-point debug mode.  */
#define OEX_DISMISS	0x80000	/* Dismiss invalid address faults.  */

/* Masks of the FP exceptions for OEX_FPU_MIN and OEX_FPU_MAX.  */
#define OEX_FPU_INVAL	0x10	/* Invalid operation exception.  */
#define OEX_FPU_DIV0	0x08	/* Division by zero exception.  */
#define OEX_FPU_OFLO	0x04	/* Overflow exception.  */
#define OEX_FPU_UFLO	0x02	/* Underflow exception.  */
#define OEX_FPU_INEX	0x01	/* Inexact exception.  */

/* Masks for the info word of an ODK_PAD descriptor.  */
#define OPAD_PREFIX	0x01
#define OPAD_POSTFIX	0x02
#define OPAD_SYMBOL	0x04

/* Masks for the info word of an ODK_HWPATCH descriptor.  */
#define OHW_R4KEOP	0x01	/* R4000 end-of-page patch.  */
#define OHW_R8KPFETCH	0x02	/* May need R8000 prefetch patch.  */
#define OHW_R5KEOP	0x04	/* R5000 end-of-page patch.  */
#define OHW_R5KCVTL	0x08	/* R5000 cvt.[ds].l bug (clean == 1).  */

/* Masks for the info word of an ODK_IDENT/ODK_GP_GROUP descriptor.  */
#define OGP_GROUP	0x0000ffff	/* GP group number.  */
#define OGP_SELF	0xffff0000	/* Self-contained GP groups.  */

/* Masks for the info word of an ODK_HWAND/ODK_HWOR descriptor.  */
#define OHWA0_R4KEOP_CHECKED	0x00000001
#define OHWA0_R4KEOP_CLEAN	0x00000002

/* start-sanitize-sky */
/* The vu overlay table is an array of this.  */

typedef struct
{
  /* `name' is offset into overlay string table section.  */
  char name[4];
  char lma[4];
  char vma[4];
} Elf32_Dvp_External_Overlay;

typedef struct
{
  bfd_vma name;
  bfd_vma lma;
  bfd_vma vma;
} Elf32_Dvp_Internal_Overlay;

/* overlay swapping routines.  */
extern void bfd_dvp_elf32_swap_overlay_in
  PARAMS ((bfd *, const Elf32_Dvp_External_Overlay *,
	   Elf32_Dvp_Internal_Overlay *));
extern void bfd_dvp_elf32_swap_overlay_out
  PARAMS ((bfd *, const Elf32_Dvp_Internal_Overlay *,
	   Elf32_Dvp_External_Overlay *));

/* end-sanitize-sky */

#endif /* _ELF_MIPS_H */
