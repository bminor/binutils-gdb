/* BFD back-end for HP PA-RISC ELF files.
   Copyright (C) 1990-1991 Free Software Foundation, Inc.

   Written by

	Center for Software Science
	Department of Computer Science
	University of Utah

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
#include "obstack.h"
#include "libelf.h"

/* ELF32/HPPA relocation support

	This file contains ELF32/HPPA relocation support as specified
	in the Stratus FTX/Golf Object File Format (SED-1762) dated
	November 19, 1992.
*/

#include "elf32-hppa.h"
#include "libhppa.h"
#include "aout/aout64.h"
#include "hppa_stubs.h"

/* ELF/PA relocation howto entries */

static bfd_reloc_status_type hppa_elf_reloc ();

static reloc_howto_type elf_hppa_howto_table[ELF_HOWTO_TABLE_SIZE] =
{
/*  'bitpos' and 'abs' are obsolete */
/* type			rs sz bsz pcrel  bpos abs    ovrf  sf		   name */
/* 9.3.4. Address relocation types */
  {R_HPPA_NONE, 0, 3, 19, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_NONE"},
  {R_HPPA_32, 0, 3, 32, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_32"},
  {R_HPPA_11, 0, 3, 11, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_11"},
  {R_HPPA_14, 0, 3, 14, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_14"},
  {R_HPPA_17, 0, 3, 17, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_17"},
{R_HPPA_L21, 0, 3, 21, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_L21"},
{R_HPPA_R11, 0, 3, 11, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_R11"},
{R_HPPA_R14, 0, 3, 14, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_R14"},
{R_HPPA_R17, 0, 3, 17, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_R17"},
  {R_HPPA_LS21, 0, 3, 21, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_LS21"},
  {R_HPPA_RS11, 0, 3, 11, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_RS11"},
  {R_HPPA_RS14, 0, 3, 14, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_RS14"},
  {R_HPPA_RS17, 0, 3, 17, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_RS17"},
  {R_HPPA_LD21, 0, 3, 21, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_LD21"},
  {R_HPPA_RD11, 0, 3, 11, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_RD11"},
  {R_HPPA_RD14, 0, 3, 14, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_RD14"},
  {R_HPPA_RD17, 0, 3, 17, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_RD17"},
  {R_HPPA_LR21, 0, 3, 21, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_LR21"},
  {R_HPPA_RR14, 0, 3, 14, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_RR14"},
  {R_HPPA_RR17, 0, 3, 17, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_RR17"},
/* 9.3.5. GOTOFF address relocation types		*/
  {R_HPPA_GOTOFF_11, 0, 3, 11, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_GOTOFF_11"},
  {R_HPPA_GOTOFF_14, 0, 3, 14, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_GOTOFF_14"},
  {R_HPPA_GOTOFF_L21, 0, 3, 21, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_GOTOFF_L21"},
  {R_HPPA_GOTOFF_R11, 0, 3, 11, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_GOTOFF_R11"},
  {R_HPPA_GOTOFF_R14, 0, 3, 14, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_GOTOFF_R14"},
  {R_HPPA_GOTOFF_LS21, 0, 3, 21, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_GOTOFF_LS21"},
  {R_HPPA_GOTOFF_RS11, 0, 3, 11, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_GOTOFF_RS11"},
  {R_HPPA_GOTOFF_RS14, 0, 3, 14, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_GOTOFF_RS14"},
  {R_HPPA_GOTOFF_LD21, 0, 3, 21, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_GOTOFF_LD21"},
  {R_HPPA_GOTOFF_RD11, 0, 3, 11, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_GOTOFF_RD11"},
  {R_HPPA_GOTOFF_RD14, 0, 3, 14, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_GOTOFF_RD14"},
  {R_HPPA_GOTOFF_LR21, 0, 3, 21, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_GOTOFF_LR21"},
  {R_HPPA_GOTOFF_RR14, 0, 3, 14, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_GOTOFF_RR14"},
/* 9.3.6. Absolute call relocation types	*/
  {R_HPPA_ABS_CALL_11, 0, 3, 11, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_ABS_CALL_11"},
  {R_HPPA_ABS_CALL_14, 0, 3, 14, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_ABS_CALL_14"},
  {R_HPPA_ABS_CALL_17, 0, 3, 17, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_ABS_CALL_17"},
  {R_HPPA_ABS_CALL_L21, 0, 3, 21, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_ABS_CALL_L21"},
  {R_HPPA_ABS_CALL_R11, 0, 3, 11, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_ABS_CALL_R11"},
  {R_HPPA_ABS_CALL_R14, 0, 3, 14, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_ABS_CALL_R14"},
  {R_HPPA_ABS_CALL_R17, 0, 3, 17, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_ABS_CALL_R17"},
  {R_HPPA_ABS_CALL_LS21, 0, 3, 21, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_ABS_CALL_LS21"},
  {R_HPPA_ABS_CALL_RS11, 0, 3, 11, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_ABS_CALL_RS11"},
  {R_HPPA_ABS_CALL_RS14, 0, 3, 14, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_ABS_CALL_RS14"},
  {R_HPPA_ABS_CALL_RS17, 0, 3, 17, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_ABS_CALL_RS17"},
  {R_HPPA_ABS_CALL_LD21, 0, 3, 21, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_ABS_CALL_LD21"},
  {R_HPPA_ABS_CALL_RD11, 0, 3, 11, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_ABS_CALL_RD11"},
  {R_HPPA_ABS_CALL_RD14, 0, 3, 14, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_ABS_CALL_RD14"},
  {R_HPPA_ABS_CALL_RD17, 0, 3, 17, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_ABS_CALL_RD17"},
  {R_HPPA_ABS_CALL_LR21, 0, 3, 21, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_ABS_CALL_LR21"},
  {R_HPPA_ABS_CALL_RR14, 0, 3, 14, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_ABS_CALL_RR14"},
  {R_HPPA_ABS_CALL_RR17, 0, 3, 17, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_ABS_CALL_RR17"},
/* 9.3.7. PC-relative call relocation types	*/
  {R_HPPA_PCREL_CALL_11, 0, 3, 11, true, 0, complain_overflow_signed, hppa_elf_reloc, "R_HPPA_PCREL_CALL_11"},
  {R_HPPA_PCREL_CALL_14, 0, 3, 14, true, 0, complain_overflow_signed, hppa_elf_reloc, "R_HPPA_PCREL_CALL_14"},
  {R_HPPA_PCREL_CALL_17, 0, 3, 17, true, 0, complain_overflow_signed, hppa_elf_reloc, "R_HPPA_PCREL_CALL_17"},
  {R_HPPA_PCREL_CALL_12, 0, 3, 12, true, 0, complain_overflow_signed, hppa_elf_reloc, "R_HPPA_PCREL_CALL_12"},
  {R_HPPA_PCREL_CALL_L21, 0, 3, 21, true, 0, complain_overflow_signed, hppa_elf_reloc, "R_HPPA_PCREL_CALL_L21"},
  {R_HPPA_PCREL_CALL_R11, 0, 3, 11, true, 0, complain_overflow_signed, hppa_elf_reloc, "R_HPPA_PCREL_CALL_R11"},
  {R_HPPA_PCREL_CALL_R14, 0, 3, 14, true, 0, complain_overflow_signed, hppa_elf_reloc, "R_HPPA_PCREL_CALL_R14"},
  {R_HPPA_PCREL_CALL_R17, 0, 3, 17, true, 0, complain_overflow_signed, hppa_elf_reloc, "R_HPPA_PCREL_CALL_R17"},
  {R_HPPA_PCREL_CALL_LS21, 0, 3, 21, true, 0, complain_overflow_signed, hppa_elf_reloc, "R_HPPA_PCREL_CALL_LS21"},
  {R_HPPA_PCREL_CALL_RS11, 0, 3, 11, true, 0, complain_overflow_signed, hppa_elf_reloc, "R_HPPA_PCREL_CALL_RS11"},
  {R_HPPA_PCREL_CALL_RS14, 0, 3, 14, true, 0, complain_overflow_signed, hppa_elf_reloc, "R_HPPA_PCREL_CALL_RS14"},
  {R_HPPA_PCREL_CALL_RS17, 0, 3, 17, true, 0, complain_overflow_signed, hppa_elf_reloc, "R_HPPA_PCREL_CALL_RS17"},
  {R_HPPA_PCREL_CALL_LD21, 0, 3, 21, true, 0, complain_overflow_signed, hppa_elf_reloc, "R_HPPA_PCREL_CALL_LD21"},
  {R_HPPA_PCREL_CALL_RD11, 0, 3, 11, true, 0, complain_overflow_signed, hppa_elf_reloc, "R_HPPA_PCREL_CALL_RD11"},
  {R_HPPA_PCREL_CALL_RD14, 0, 3, 14, true, 0, complain_overflow_signed, hppa_elf_reloc, "R_HPPA_PCREL_CALL_RD14"},
  {R_HPPA_PCREL_CALL_RD17, 0, 3, 17, true, 0, complain_overflow_signed, hppa_elf_reloc, "R_HPPA_PCREL_CALL_RD17"},
  {R_HPPA_PCREL_CALL_LR21, 0, 3, 21, true, 0, complain_overflow_signed, hppa_elf_reloc, "R_HPPA_PCREL_CALL_LR21"},
  {R_HPPA_PCREL_CALL_RR14, 0, 3, 14, true, 0, complain_overflow_signed, hppa_elf_reloc, "R_HPPA_PCREL_CALL_RR14"},
  {R_HPPA_PCREL_CALL_RR17, 0, 3, 17, true, 0, complain_overflow_signed, hppa_elf_reloc, "R_HPPA_PCREL_CALL_RR17"},

/* 9.3.8. Plabel relocation types */
  {R_HPPA_PLABEL_32, 0, 3, 32, false, 0, complain_overflow_signed, hppa_elf_reloc, "R_HPPA_PLABEL_32"},
  {R_HPPA_PLABEL_11, 0, 3, 11, false, 0, complain_overflow_signed, hppa_elf_reloc, "R_HPPA_PLABEL_11"},
  {R_HPPA_PLABEL_14, 0, 3, 14, false, 0, complain_overflow_signed, hppa_elf_reloc, "R_HPPA_PLABEL_14"},
  {R_HPPA_PLABEL_L21, 0, 3, 21, false, 0, complain_overflow_signed, hppa_elf_reloc, "R_HPPA_PLABEL_L21"},
  {R_HPPA_PLABEL_R11, 0, 3, 11, false, 0, complain_overflow_signed, hppa_elf_reloc, "R_HPPA_PLABEL_R11"},
  {R_HPPA_PLABEL_R14, 0, 3, 14, false, 0, complain_overflow_signed, hppa_elf_reloc, "R_HPPA_PLABEL_R14"},

/* 9.3.9. Data linkage table (DLT) relocation types	*/
  {R_HPPA_DLT_32, 0, 3, 32, false, 0, complain_overflow_signed, hppa_elf_reloc, "R_HPPA_DLT_32"},
  {R_HPPA_DLT_11, 0, 3, 11, false, 0, complain_overflow_signed, hppa_elf_reloc, "R_HPPA_DLT_11"},
  {R_HPPA_DLT_14, 0, 3, 14, false, 0, complain_overflow_signed, hppa_elf_reloc, "R_HPPA_DLT_14"},
  {R_HPPA_DLT_L21, 0, 3, 21, false, 0, complain_overflow_signed, hppa_elf_reloc, "R_HPPA_DLT_L21"},
  {R_HPPA_DLT_R11, 0, 3, 11, false, 0, complain_overflow_signed, hppa_elf_reloc, "R_HPPA_DLT_R11"},
  {R_HPPA_DLT_R14, 0, 3, 14, false, 0, complain_overflow_signed, hppa_elf_reloc, "R_HPPA_DLT_R14"},

/* 9.3.10. Relocations for unwinder tables	*/
  {R_HPPA_UNWIND_ENTRY, 0, 3, 32, true, 0, complain_overflow_signed, hppa_elf_reloc, "R_HPPA_UNWIND_ENTRY"},
  {R_HPPA_UNWIND_ENTRIES, 0, 3, 32, true, 0, complain_overflow_signed, hppa_elf_reloc, "R_HPPA_UNWIND_ENTRIES"},

/*  9.3.11. Relocation types for complex expressions	*/
  {R_HPPA_PUSH_CONST, 0, 3, 32, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_PUSH_CONST"},
  {R_HPPA_PUSH_PC, 0, 3, 32, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_PUSH_PC"},
  {R_HPPA_PUSH_SYM, 0, 3, 32, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_PUSH_SYM"},
  {R_HPPA_PUSH_GOTOFF, 0, 3, 32, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_PUSH_GOTOFF"},
  {R_HPPA_PUSH_ABS_CALL, 0, 3, 32, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_PUSH_ABS_CALL"},
  {R_HPPA_PUSH_PCREL_CALL, 0, 3, 32, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_PUSH_PCREL_CALL"},
  {R_HPPA_PUSH_PLABEL, 0, 3, 32, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_PUSH_PLABEL"},
{R_HPPA_MAX, 0, 3, 32, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_MAX"},
{R_HPPA_MIN, 0, 3, 32, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_MIN"},
{R_HPPA_ADD, 0, 3, 32, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_ADD"},
{R_HPPA_SUB, 0, 3, 32, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_SUB"},
  {R_HPPA_MULT, 0, 3, 32, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_MULT"},
{R_HPPA_DIV, 0, 3, 32, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_DIV"},
{R_HPPA_MOD, 0, 3, 32, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_MOD"},
{R_HPPA_AND, 0, 3, 32, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_AND"},
  {R_HPPA_OR, 0, 3, 32, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_OR"},
{R_HPPA_XOR, 0, 3, 32, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_XOR"},
{R_HPPA_NOT, 0, 3, 32, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_NOT"},
  {R_HPPA_LSHIFT, 0, 3, 32, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_LSHIFT"},
  {R_HPPA_ARITH_RSHIFT, 0, 3, 32, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_ARITH_RSHIFT"},
  {R_HPPA_LOGIC_RSHIFT, 0, 3, 32, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_LOGIC_RSHIFT"},
{R_HPPA_EXPR_F, 0, 3, 32, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_L"},
  {R_HPPA_EXPR_L, 0, 3, 32, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_EXPR_L"},
  {R_HPPA_EXPR_R, 0, 3, 32, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_EXPR_R"},
  {R_HPPA_EXPR_LS, 0, 3, 32, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_EXPR_LS"},
  {R_HPPA_EXPR_RS, 0, 3, 32, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_EXPR_RS"},
  {R_HPPA_EXPR_LD, 0, 3, 32, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_EXPR_LD"},
  {R_HPPA_EXPR_RD, 0, 3, 32, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_EXPR_RD"},
  {R_HPPA_EXPR_LR, 0, 3, 32, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_EXPR_LR"},
  {R_HPPA_EXPR_RR, 0, 3, 32, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_EXPR_RR"},

  {R_HPPA_EXPR_32, 0, 3, 32, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_EXPR_32"},
  {R_HPPA_EXPR_21, 0, 3, 21, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_EXPR_21"},
  {R_HPPA_EXPR_11, 0, 3, 11, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_EXPR_11"},
  {R_HPPA_EXPR_14, 0, 3, 14, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_EXPR_14"},
  {R_HPPA_EXPR_17, 0, 3, 17, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_EXPR_17"},
  {R_HPPA_EXPR_12, 0, 3, 12, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_EXPR_12"},
  {R_HPPA_STUB_CALL_17, 0, 3, 17, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_HPPA_STUB_CALL_17"},
  {R_HPPA_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_HPPA_UNIMPLEMENTED"},
};

static symext_chainS *symext_rootP;
static symext_chainS *symext_lastP;
static boolean symext_chain_built;

static unsigned long
DEFUN (hppa_elf_rebuild_insn, (abfd, insn, value, r_type, r_field, r_format),
       bfd * abfd AND
       unsigned long insn AND
       unsigned long value AND
       unsigned short r_type AND
       unsigned short r_field AND
       unsigned short r_format)
{
  unsigned long const_part;	/* part of the instruction that does not change */
  unsigned long rebuilt_part;

  switch (r_format)
    {
    case 11:
      {
	unsigned w1, w;

	const_part = insn & 0xffffe002;
	dis_assemble_12 (value, &w1, &w);
	rebuilt_part = (w1 << 2) | w;
	return const_part | rebuilt_part;
      }

    case 12:
      {
	unsigned w1, w;

	const_part = insn & 0xffffe002;
	dis_assemble_12 (value, &w1, &w);
	rebuilt_part = (w1 << 2) | w;
	return const_part | rebuilt_part;
      }

    case 14:
      const_part = insn & 0xffffc000;
      low_sign_unext (value, 14, &rebuilt_part);
      return const_part | rebuilt_part;

    case 17:
      {
	unsigned w1, w2, w;

	const_part = insn & 0xffe0e002;
	dis_assemble_17 (value, &w1, &w2, &w);
	rebuilt_part = (w2 << 2) | (w1 << 16) | w;
	return const_part | rebuilt_part;
      }

    case 21:
      const_part = insn & 0xffe00000;
      dis_assemble_21 (value, &rebuilt_part);
      return const_part | rebuilt_part;

    case 32:
      const_part = 0;
      return value;

    default:
      fprintf (stderr, "Relocation problem : ");
      fprintf (stderr,
	       "Unrecognized reloc type %d (fmt=%d,fld=%d), in module %s\n",
	       r_type, r_format, r_field, abfd->filename);
    }
  return insn;
}

static unsigned long
DEFUN (hppa_elf_relocate_insn,
       (abfd, input_sect,
	insn, address, symp, sym_value, r_addend,
	r_type, r_format, r_field, pcrel),
       bfd * abfd AND
       asection * input_sect AND
       unsigned long insn AND
       unsigned long address AND
       asymbol * symp AND
       long sym_value AND
       long r_addend AND
       unsigned short r_type AND
       unsigned short r_format AND
       unsigned short r_field AND
       unsigned char pcrel)
{
  unsigned char opcode = get_opcode (insn);
  long constant_value;
  unsigned arg_reloc;

  switch (opcode)
    {
    case LDO:
    case LDB:
    case LDH:
    case LDW:
    case LDWM:
    case STB:
    case STH:
    case STW:
    case STWM:
      constant_value = HPPA_R_CONSTANT (r_addend);
      BFD_ASSERT (r_format == 14);

      if (pcrel)
	sym_value -= address;
      sym_value = hppa_field_adjust (sym_value, constant_value, r_field);
      return hppa_elf_rebuild_insn (abfd, insn, sym_value, r_type, r_field, r_format);

    case COMICLR:
    case SUBI:			/* case SUBIO: */
    case ADDIT:		/* case ADDITO: */
    case ADDI:			/* case ADDIO: */
      BFD_ASSERT (r_format == 11);

      constant_value = HPPA_R_CONSTANT(r_addend);
      sym_value = hppa_field_adjust (sym_value, constant_value, r_field);
      return hppa_elf_rebuild_insn (abfd, insn, sym_value, r_type, r_field, r_format);

    case LDIL:
    case ADDIL:
      BFD_ASSERT (r_format == 21);

      constant_value = HPPA_R_CONSTANT (r_addend);
      sym_value = hppa_field_adjust (sym_value, constant_value, r_field);
      return hppa_elf_rebuild_insn (abfd, insn, sym_value, r_type, r_field, r_format);

    case BL:
    case BE:
    case BLE:
      arg_reloc = HPPA_R_ARG_RELOC (r_addend);

      BFD_ASSERT (r_format == 17);

      /* XXX computing constant_value is not needed??? */
      constant_value = assemble_17 ((insn & 0x001f0000) >> 16,
				    (insn & 0x00001ffc) >> 2,
				    insn & 1);
      /* @@ Assumes only 32 bits.  */
      constant_value = (constant_value << 15) >> 15;
      if (pcrel)
	{
	  sym_value -=
	    address + input_sect->output_offset
	    + input_sect->output_section->vma;
	  sym_value = hppa_field_adjust (sym_value, -8, r_field);
	}
      else
	sym_value = hppa_field_adjust (sym_value, constant_value, r_field);

      return hppa_elf_rebuild_insn (abfd, insn, sym_value >> 2, r_type, r_field, r_format);

    default:
      if (opcode == 0)
	{
	  BFD_ASSERT (r_format == 32);
	  constant_value = HPPA_R_CONSTANT (r_addend);

	  return hppa_field_adjust (sym_value, constant_value, r_field);
	}
      else
	{
	  fprintf (stderr,
		   "Unrecognized opcode 0x%02x (fmt=%x,field=%x)\n",
		   opcode, r_format, r_field);
	  return insn;
	}
    }
}

static void
DEFUN (hppa_elf_relocate_unwind_table,
       (abfd, input_sect,
	data, address, symp, sym_value, r_addend,
	r_type, r_format, r_field, pcrel),
       bfd * abfd AND
       asection * input_sect AND
       PTR data AND
       unsigned long address AND
       asymbol * symp AND
       long sym_value AND
       long r_addend AND
       unsigned short r_type AND
       unsigned short r_format AND
       unsigned short r_field AND
       unsigned char pcrel)
{
  bfd_byte *hit_data = address + (bfd_byte *) (data);
  long start_offset;
  long end_offset;
  long relocated_value;
  int i;

  BFD_ASSERT (r_format == 32);
  BFD_ASSERT (r_field == e_fsel);
  switch (r_type)
    {
    case R_HPPA_UNWIND_ENTRY:
      start_offset = bfd_get_32 (abfd, hit_data);
      relocated_value = hppa_field_adjust (sym_value, start_offset, r_field);
      bfd_put_32 (abfd, relocated_value, hit_data);

      hit_data += sizeof (unsigned long);
      end_offset = bfd_get_32 (abfd, hit_data);
      relocated_value = hppa_field_adjust (sym_value, end_offset, r_field);
      bfd_put_32 (abfd, relocated_value, hit_data);
      break;

    case R_HPPA_UNWIND_ENTRIES:
      for (i = 0; i < r_addend; i++, hit_data += 3 * sizeof (unsigned long))
	{
	  unsigned int adjustment;
	  start_offset = bfd_get_32 (abfd, hit_data);
	  /* Stuff the symbol value into the first word */
	  /* of the  unwind descriptor */
	  bfd_put_32 (abfd, sym_value, hit_data);
	  adjustment = sym_value - start_offset;

	  hit_data += sizeof (unsigned long);
	  end_offset = adjustment + bfd_get_32 (abfd, hit_data);
	  bfd_put_32 (abfd, end_offset, hit_data);

	  /* If this is not the last unwind entry, */
	  /* adjust the symbol value. */
	  if (i + 1 < r_addend)
	    {
	      start_offset = bfd_get_32 (abfd, hit_data + 3 * sizeof (unsigned long));
              sym_value = start_offset + adjustment;
	    }
	}
      break;

    default:
      fprintf (stderr,
	       "Unrecognized relocation type 0x%02x (fmt=%x,field=%x)\n",
	       r_type, r_format, r_field);
    }
}

/* Provided the symbol, returns the value reffed */
static long
get_symbol_value (symbol)
     asymbol *symbol;
{
  long relocation = 0;

  if (symbol == (asymbol *) NULL)
    relocation = 0;
  else if (symbol->section == &bfd_com_section)
    {
      relocation = 0;
    }
  else
    {
      relocation = symbol->value +
	symbol->section->output_section->vma +
	symbol->section->output_offset;
    }

  return (relocation);
}

/* This function provides a pretty straight-forward mapping between a */
/* base relocation type, format and field into the relocation type */
/* that will be emitted in an object file.  The only wrinkle in the */
/* mapping is that when the T, TR, TL, P, PR, or PL expression */
/* prefixes are involved, the type gets promoted to a *_GOTOFF_* */
/* relocation (in the case of T, TR, and TL) or a PLABEL relocation */
/* (in the case of P, PR, and PL).	*/

/* NOTE: XXX the T, TR, TL, P, PR, and PL expression prefixes are not */
/* handled yet. */

static void
hppa_elf_gen_reloc_error (base_type, fmt, field)
     elf32_hppa_reloc_type base_type;
     int fmt;
     int field;
{
  fprintf (stderr, "undefined relocation: base=0x%x,fmt=0x%x,field=0x%x\n",
	   base_type, fmt, field);
}

elf32_hppa_reloc_type **
hppa_elf_gen_reloc_type (abfd, base_type, format, field)
     bfd *abfd;
     elf32_hppa_reloc_type base_type;
     int format;
     int field;
{
#define UNDEFINED	hppa_elf_gen_reloc_error(base_type,format,field)

  elf32_hppa_reloc_type *finaltype;
  elf32_hppa_reloc_type **final_types;
  int i;

  final_types = (elf32_hppa_reloc_type **) bfd_alloc_by_size_t (abfd, sizeof (elf32_hppa_reloc_type *) * 2);
  BFD_ASSERT (final_types != 0);

  finaltype = (elf32_hppa_reloc_type *) bfd_alloc_by_size_t (abfd, sizeof (elf32_hppa_reloc_type));
  BFD_ASSERT (finaltype != 0);

  final_types[0] = finaltype;
  final_types[1] = NULL;

#define final_type finaltype[0]

  final_type = base_type;

  switch (base_type)
    {
    case R_HPPA:
      switch (format)
	{
	case 11:
	  switch (field)
	    {
	    case e_fsel:
	      final_type = R_HPPA_11;
	      break;
	    case e_rsel:
	      final_type = R_HPPA_R11;
	      break;
	    case e_rssel:
	      final_type = R_HPPA_RS11;
	      break;
	    case e_rdsel:
	      final_type = R_HPPA_RD11;
	      break;

	    case e_psel:
	      final_type = R_HPPA_PLABEL_11;
	      break;
	    case e_rpsel:
	      final_type = R_HPPA_PLABEL_R11;
	      break;
	    case e_tsel:
	      final_type = R_HPPA_DLT_11;
	      break;
	    case e_rtsel:
	      final_type = R_HPPA_DLT_R11;
	      break;

	    case e_lpsel:
	    case e_ltsel:
	    case e_lsel:
	    case e_lrsel:
	    case e_lssel:
	    case e_rrsel:
	    default:
	      UNDEFINED;
	      final_type = base_type;
	      break;
	    }
	  break;
	case 12:
	  UNDEFINED;
	  break;
	case 14:
	  switch (field)
	    {
	    case e_rsel:
	      final_type = R_HPPA_R14;
	      break;
	    case e_rssel:
	      final_type = R_HPPA_RS14;
	      break;
	    case e_rdsel:
	      final_type = R_HPPA_RD14;
	      break;
	    case e_rrsel:
	      final_type = R_HPPA_RR14;
	      break;

	    case e_psel:
	      final_type = R_HPPA_PLABEL_14;
	      break;
	    case e_rpsel:
	      final_type = R_HPPA_PLABEL_R14;
	      break;
	    case e_tsel:
	      final_type = R_HPPA_DLT_14;
	      break;
	    case e_rtsel:
	      final_type = R_HPPA_DLT_R14;
	      break;

	    case e_lpsel:
	    case e_ltsel:

	    case e_fsel:
	    case e_lsel:
	    case e_lssel:
	    case e_ldsel:
	    case e_lrsel:
	    default:
	      UNDEFINED;
	      final_type = base_type;
	      break;
	    }
	  break;
	case 17:
	  switch (field)
	    {
	    case e_fsel:
	      final_type = R_HPPA_17;
	      break;
	    case e_rsel:
	      final_type = R_HPPA_R17;
	      break;
	    case e_rssel:
	      final_type = R_HPPA_RS17;
	      break;
	    case e_rdsel:
	      final_type = R_HPPA_RD17;
	      break;
	    case e_rrsel:
	      final_type = R_HPPA_RR17;
	      break;
	    case e_lsel:
	    case e_lssel:
	    case e_ldsel:
	    case e_lrsel:
	    default:
	      UNDEFINED;
	      final_type = base_type;
	      break;
	    }
	  break;
	case 21:
	  switch (field)
	    {
	    case e_lsel:
	      final_type = R_HPPA_L21;
	      break;
	    case e_lssel:
	      final_type = R_HPPA_LS21;
	      break;
	    case e_ldsel:
	      final_type = R_HPPA_LD21;
	      break;
	    case e_lrsel:
	      final_type = R_HPPA_LR21;
	      break;
	    case e_lpsel:
	      final_type = R_HPPA_PLABEL_L21;
	      break;
	    case e_ltsel:
	      final_type = R_HPPA_PLABEL_L21;
	      break;
	    case e_rsel:
	    case e_rssel:
	    case e_rdsel:
	    case e_rrsel:
	    case e_fsel:
	    default:
	      UNDEFINED;
	      final_type = base_type;
	      break;
	    }
	  break;
	case 32:
	  switch (field)
	    {
	    case e_fsel:
	      final_type = R_HPPA_32;
	      break;
	    case e_psel:
	      final_type = R_HPPA_PLABEL_32;
	      break;
	    case e_tsel:
	      final_type == R_HPPA_DLT_32;
	      break;
	    default:
	      UNDEFINED;
	      final_type = base_type;
	      break;
	    }
	  break;
	default:
	  UNDEFINED;
	  final_type = base_type;
	  break;
	}
      break;
    case R_HPPA_GOTOFF:
      switch (format)
	{
	case 11:
	  switch (field)
	    {
	    case e_rsel:
	      final_type = R_HPPA_GOTOFF_R11;
	      break;
	    case e_rssel:
	      final_type = R_HPPA_GOTOFF_RS11;
	      break;
	    case e_rdsel:
	      final_type = R_HPPA_GOTOFF_RD11;
	      break;
	    case e_fsel:
	      final_type = R_HPPA_GOTOFF_11;
	      break;
	    case e_lsel:
	    case e_lrsel:
	    case e_lssel:
	    case e_rrsel:
	    default:
	      UNDEFINED;
	      final_type = base_type;
	      break;
	    }
	  break;
	case 12:
	  UNDEFINED;
	  final_type = base_type;
	  break;
	case 14:
	  switch (field)
	    {
	    case e_rsel:
	      final_type = R_HPPA_GOTOFF_R14;
	      break;
	    case e_rssel:
	      final_type = R_HPPA_GOTOFF_RS14;
	      break;
	    case e_rdsel:
	      final_type = R_HPPA_GOTOFF_RD14;
	      break;
	    case e_rrsel:
	      final_type = R_HPPA_GOTOFF_RR14;
	      break;
	    case e_fsel:
	      final_type = R_HPPA_GOTOFF_14;
	      break;
	    case e_lsel:
	    case e_lssel:
	    case e_ldsel:
	    case e_lrsel:
	    default:
	      UNDEFINED;
	      final_type = base_type;
	      break;
	    }
	  break;
	case 17:
	  UNDEFINED;
	  final_type = base_type;
	  break;
	case 21:
	  switch (field)
	    {
	    case e_lsel:
	      final_type = R_HPPA_GOTOFF_L21;
	      break;
	    case e_lssel:
	      final_type = R_HPPA_GOTOFF_LS21;
	      break;
	    case e_ldsel:
	      final_type = R_HPPA_GOTOFF_LD21;
	      break;
	    case e_lrsel:
	      final_type = R_HPPA_GOTOFF_LR21;
	      break;
	    case e_rsel:
	    case e_rssel:
	    case e_rdsel:
	    case e_rrsel:
	    case e_fsel:
	    default:
	      UNDEFINED;
	      final_type = base_type;
	      break;
	    }
	  break;
	case 32:
	  UNDEFINED;
	  final_type = base_type;
	  break;
	default:
	  UNDEFINED;
	  final_type = base_type;
	  break;
	}
      break;
    case R_HPPA_PCREL_CALL:
      switch (format)
	{
	case 11:
	  switch (field)
	    {
	    case e_rsel:
	      final_type = R_HPPA_PCREL_CALL_R11;
	      break;
	    case e_rssel:
	      final_type = R_HPPA_PCREL_CALL_RS11;
	      break;
	    case e_rdsel:
	      final_type = R_HPPA_PCREL_CALL_RD11;
	      break;
	    case e_fsel:
	      final_type = R_HPPA_PCREL_CALL_11;
	      break;
	    case e_lsel:
	    case e_lrsel:
	    case e_lssel:
	    case e_rrsel:
	    default:
	      UNDEFINED;
	      final_type = base_type;
	      break;
	    }
	  break;
	case 12:
	  UNDEFINED;
	  final_type = base_type;
	  break;
	case 14:
	  switch (field)
	    {
	    case e_rsel:
	      final_type = R_HPPA_PCREL_CALL_R14;
	      break;
	    case e_rssel:
	      final_type = R_HPPA_PCREL_CALL_RS14;
	      break;
	    case e_rdsel:
	      final_type = R_HPPA_PCREL_CALL_RD14;
	      break;
	    case e_rrsel:
	      final_type = R_HPPA_PCREL_CALL_RR14;
	      break;
	    case e_fsel:
	      final_type = R_HPPA_PCREL_CALL_14;
	      break;
	    case e_lsel:
	    case e_lssel:
	    case e_ldsel:
	    case e_lrsel:
	    default:
	      UNDEFINED;
	      final_type = base_type;
	      break;
	    }
	  break;
	case 17:
	  switch (field)
	    {
	    case e_rsel:
	      final_type = R_HPPA_PCREL_CALL_R17;
	      break;
	    case e_rssel:
	      final_type = R_HPPA_PCREL_CALL_RS17;
	      break;
	    case e_rdsel:
	      final_type = R_HPPA_PCREL_CALL_RD17;
	      break;
	    case e_rrsel:
	      final_type = R_HPPA_PCREL_CALL_RR17;
	      break;
	    case e_fsel:
	      final_type = R_HPPA_PCREL_CALL_17;
	      break;
	    case e_lsel:
	    case e_lssel:
	    case e_ldsel:
	    case e_lrsel:
	    default:
	      UNDEFINED;
	      final_type = base_type;
	      break;
	    }
	  break;
	case 21:
	  switch (field)
	    {
	    case e_lsel:
	      final_type = R_HPPA_PCREL_CALL_L21;
	      break;
	    case e_lssel:
	      final_type = R_HPPA_PCREL_CALL_LS21;
	      break;
	    case e_ldsel:
	      final_type = R_HPPA_PCREL_CALL_LD21;
	      break;
	    case e_lrsel:
	      final_type = R_HPPA_PCREL_CALL_LR21;
	      break;
	    case e_rsel:
	    case e_rssel:
	    case e_rdsel:
	    case e_rrsel:
	    case e_fsel:
	    default:
	      UNDEFINED;
	      final_type = base_type;
	      break;
	    }
	  break;
	case 32:
	  UNDEFINED;
	  final_type = base_type;
	  break;
	default:
	  UNDEFINED;
	  final_type = base_type;
	  break;
	}
      break;
    case R_HPPA_PLABEL:
      switch (format)
	{
	case 11:
	  switch (field)
	    {
	    case e_fsel:
	      final_type = R_HPPA_PLABEL_11;
	      break;
	    case e_rsel:
	      final_type = R_HPPA_PLABEL_R11;
	      break;
	    default:
	      UNDEFINED;
	      final_type = base_type;
	      break;
	    }
	  break;
	case 14:
	  switch (field)
	    {
	    case e_fsel:
	      final_type = R_HPPA_PLABEL_14;
	      break;
	    case e_rsel:
	      final_type = R_HPPA_PLABEL_R14;
	      break;
	    default:
	      UNDEFINED;
	      final_type = base_type;
	      break;
	    }
	  break;
	case 21:
	  switch (field)
	    {
	    case e_lsel:
	      final_type = R_HPPA_PLABEL_L21;
	      break;
	    default:
	      UNDEFINED;
	      final_type = base_type;
	      break;
	    }
	  break;
	case 32:
	  switch (field)
	    {
	    case e_fsel:
	      final_type = R_HPPA_PLABEL_32;
	      break;
	    default:
	      UNDEFINED;
	      final_type = base_type;
	      break;
	    }
	  break;
	default:
	  UNDEFINED;
	  final_type = base_type;
	  break;
	}
    case R_HPPA_ABS_CALL:
      switch (format)
	{
	case 11:
	  switch (field)
	    {
	    case e_rsel:
	      final_type = R_HPPA_ABS_CALL_R11;
	      break;
	    case e_rssel:
	      final_type = R_HPPA_ABS_CALL_RS11;
	      break;
	    case e_rdsel:
	      final_type = R_HPPA_ABS_CALL_RD11;
	      break;
	    case e_fsel:
	      final_type = R_HPPA_ABS_CALL_11;
	      break;
	    case e_lsel:
	    case e_lrsel:
	    case e_lssel:
	    case e_rrsel:
	    default:
	      UNDEFINED;
	      final_type = base_type;
	      break;
	    }
	  break;
	case 12:
	  UNDEFINED;
	  final_type = base_type;
	  break;
	case 14:
	  switch (field)
	    {
	    case e_rsel:
	      final_type = R_HPPA_ABS_CALL_R14;
	      break;
	    case e_rssel:
	      final_type = R_HPPA_ABS_CALL_RS14;
	      break;
	    case e_rdsel:
	      final_type = R_HPPA_ABS_CALL_RD14;
	      break;
	    case e_rrsel:
	      final_type = R_HPPA_ABS_CALL_RR14;
	      break;
	    case e_fsel:
	      final_type = R_HPPA_ABS_CALL_14;
	      break;
	    case e_lsel:
	    case e_lssel:
	    case e_ldsel:
	    case e_lrsel:
	    default:
	      UNDEFINED;
	      final_type = base_type;
	      break;
	    }
	  break;
	case 17:
	  switch (field)
	    {
	    case e_rsel:
	      final_type = R_HPPA_ABS_CALL_R17;
	      break;
	    case e_rssel:
	      final_type = R_HPPA_ABS_CALL_RS17;
	      break;
	    case e_rdsel:
	      final_type = R_HPPA_ABS_CALL_RD17;
	      break;
	    case e_rrsel:
	      final_type = R_HPPA_ABS_CALL_RR17;
	      break;
	    case e_fsel:
	      final_type = R_HPPA_ABS_CALL_17;
	      break;
	    case e_lsel:
	    case e_lssel:
	    case e_ldsel:
	    case e_lrsel:
	    default:
	      UNDEFINED;
	      final_type = base_type;
	      break;
	    }
	  break;
	case 21:
	  switch (field)
	    {
	    case e_lsel:
	      final_type = R_HPPA_ABS_CALL_L21;
	      break;
	    case e_lssel:
	      final_type = R_HPPA_ABS_CALL_LS21;
	      break;
	    case e_ldsel:
	      final_type = R_HPPA_ABS_CALL_LD21;
	      break;
	    case e_lrsel:
	      final_type = R_HPPA_ABS_CALL_LR21;
	      break;
	    case e_rsel:
	    case e_rssel:
	    case e_rdsel:
	    case e_rrsel:
	    case e_fsel:
	    default:
	      UNDEFINED;
	      final_type = base_type;
	      break;
	    }
	  break;
	case 32:
	  UNDEFINED;
	  final_type = base_type;
	  break;
	default:
	  UNDEFINED;
	  final_type = base_type;
	  break;
	}
      break;
    case R_HPPA_UNWIND:
      final_type = R_HPPA_UNWIND_ENTRY;
      break;
    case R_HPPA_COMPLEX:
    case R_HPPA_COMPLEX_PCREL_CALL:
    case R_HPPA_COMPLEX_ABS_CALL:
      final_types = (elf32_hppa_reloc_type **) bfd_alloc_by_size_t (abfd, sizeof (elf32_hppa_reloc_type *) * 6);
      BFD_ASSERT (final_types != 0);

      finaltype = (elf32_hppa_reloc_type *) bfd_alloc_by_size_t (abfd, sizeof (elf32_hppa_reloc_type) * 5);
      BFD_ASSERT (finaltype != 0);

      for (i = 0; i < 5; i++)
	final_types[i] = &finaltype[i];

      final_types[5] = NULL;

      finaltype[0] = R_HPPA_PUSH_SYM;

      if (base_type == R_HPPA_COMPLEX)
	finaltype[1] = R_HPPA_PUSH_SYM;
      else if (base_type == R_HPPA_COMPLEX_PCREL_CALL)
	finaltype[1] = R_HPPA_PUSH_PCREL_CALL;
      else			/* base_type == R_HPPA_COMPLEX_ABS_CALL */
	finaltype[1] = R_HPPA_PUSH_ABS_CALL;

      finaltype[2] = R_HPPA_SUB;

      switch (field)
	{
	case e_fsel:
	  finaltype[3] = R_HPPA_EXPR_F;
	  break;
	case e_lsel:
	  finaltype[3] = R_HPPA_EXPR_L;
	  break;
	case e_rsel:
	  finaltype[3] = R_HPPA_EXPR_R;
	  break;
	case e_lssel:
	  finaltype[3] = R_HPPA_EXPR_LS;
	  break;
	case e_rssel:
	  finaltype[3] = R_HPPA_EXPR_RS;
	  break;
	case e_ldsel:
	  finaltype[3] = R_HPPA_EXPR_LD;
	  break;
	case e_rdsel:
	  finaltype[3] = R_HPPA_EXPR_RD;
	  break;
	case e_lrsel:
	  finaltype[3] = R_HPPA_EXPR_LR;
	  break;
	case e_rrsel:
	  finaltype[3] = R_HPPA_EXPR_RR;
	  break;
	}

      switch (format)
	{
	case 11:
	  finaltype[4] = R_HPPA_EXPR_11;
	  break;
	case 12:
	  finaltype[4] = R_HPPA_EXPR_12;
	  break;
	case 14:
	  finaltype[4] = R_HPPA_EXPR_14;
	  break;
	case 17:
	  finaltype[4] = R_HPPA_EXPR_17;
	  break;
	case 21:
	  finaltype[4] = R_HPPA_EXPR_21;
	  break;
	case 32:
	  finaltype[4] = R_HPPA_EXPR_32;
	  break;
	}

      break;

    default:
      final_type = base_type;
      break;
    }

  return final_types;
}

#undef final_type


/* this function is in charge of performing all the HP PA relocations */
static long global_value;
static long GOT_value;	/* XXX:  need to calculate this! For HPUX, GOT == DP */
static asymbol *global_symbol;
static int global_sym_defined;

static bfd_reloc_status_type
hppa_elf_reloc (abfd, reloc_entry, symbol_in, data, input_section, output_bfd)
     bfd *abfd;
     arelent *reloc_entry;
     asymbol *symbol_in;
     PTR data;
     asection *input_section;
     bfd *output_bfd;
{
  unsigned long insn;
  long sym_value = 0;
  unsigned long addr = reloc_entry->address;
  bfd_byte *hit_data = addr + (bfd_byte *) (data);
  unsigned short r_type = reloc_entry->howto->type & 0xFF;
  unsigned short r_field = e_fsel;
  boolean r_pcrel = reloc_entry->howto->pc_relative;
  unsigned r_format = reloc_entry->howto->bitsize;
  long r_addend = reloc_entry->addend;

  if (output_bfd)
    {
      /* Partial linking - do nothing */
      reloc_entry->address += input_section->output_offset;
      return bfd_reloc_ok;
    }

  /* If performing final link and the symbol we're relocating against
     is undefined, then return an error.  */ 
  if (symbol_in && symbol_in->section == &bfd_und_section)
    return bfd_reloc_undefined;

  sym_value = get_symbol_value (symbol_in);

  /* Compute the value of $global$.  */
  if (!global_sym_defined)
    {
      if (global_symbol)
	{
	  global_value = (global_symbol->value
			  + global_symbol->section->output_section->vma
			  + global_symbol->section->output_offset);
	  GOT_value = global_value;
	  global_sym_defined++;
	}
    }

  /* Get the instruction word.  */
  insn = bfd_get_32 (abfd, hit_data);

  /* Relocate the value based on one of the basic relocation types

   basic_type_1:	relocation is relative to $global$
   basic_type_2:	relocation is relative to the current GOT
   basic_type_3:	relocation is an absolute call
   basic_type_4:	relocation is an PC-relative call
   basic_type_5:	relocation is plabel reference
   basic_type_6:	relocation is an unwind table relocation
   extended_type:	unimplemented  */

  switch (r_type)
    {
    case R_HPPA_NONE:
      break;

    /* Handle all the basic type 1 relocations.  */
    case R_HPPA_32:
      r_field = e_fsel;
      goto do_basic_type_1;
    case R_HPPA_11:
      r_field = e_fsel;
      goto do_basic_type_1;
    case R_HPPA_14:
      r_field = e_fsel;
      goto do_basic_type_1;
    case R_HPPA_17:
      r_field = e_fsel;
      goto do_basic_type_1;
    case R_HPPA_L21:
      r_field = e_lsel;
      goto do_basic_type_1;
    case R_HPPA_R11:
      r_field = e_rsel;
      goto do_basic_type_1;
    case R_HPPA_R14:
      r_field = e_rsel;
      goto do_basic_type_1;
    case R_HPPA_R17:
      r_field = e_rsel;
      goto do_basic_type_1;
    case R_HPPA_LS21:
      r_field = e_lssel;
      goto do_basic_type_1;
    case R_HPPA_RS11:
      r_field = e_rssel;
      goto do_basic_type_1;
    case R_HPPA_RS14:
      r_field = e_rssel;
      goto do_basic_type_1;
    case R_HPPA_RS17:
      r_field = e_ldsel;
      goto do_basic_type_1;
    case R_HPPA_LD21:
      r_field = e_ldsel;
      goto do_basic_type_1;
    case R_HPPA_RD11:
      r_field = e_rdsel;
      goto do_basic_type_1;
    case R_HPPA_RD14:
      r_field = e_rdsel;
      goto do_basic_type_1;
    case R_HPPA_RD17:
      r_field = e_rdsel;
      goto do_basic_type_1;
    case R_HPPA_LR21:
      r_field = e_lrsel;
      goto do_basic_type_1;
    case R_HPPA_RR14:
      r_field = e_rrsel;
      goto do_basic_type_1;
    case R_HPPA_RR17:
      r_field = e_rrsel;

    do_basic_type_1:
      insn = hppa_elf_relocate_insn (abfd, input_section, insn, addr,
				     symbol_in, sym_value, r_addend,
				     r_type, r_format, r_field, r_pcrel);
      break;

    /* Handle all the basic type 2 relocations.  */
    case R_HPPA_GOTOFF_11:
      r_field = e_fsel;
      goto do_basic_type_2;
    case R_HPPA_GOTOFF_14:
      r_field = e_fsel;
      goto do_basic_type_2;
    case R_HPPA_GOTOFF_L21:
      r_field = e_lsel;
      goto do_basic_type_2;
    case R_HPPA_GOTOFF_R11:
      r_field = e_rsel;
      goto do_basic_type_2;
    case R_HPPA_GOTOFF_R14:
      r_field = e_rsel;
      goto do_basic_type_2;
    case R_HPPA_GOTOFF_LS21:
      r_field = e_lssel;
      goto do_basic_type_2;
    case R_HPPA_GOTOFF_RS11:
      r_field = e_rssel;
      goto do_basic_type_2;
    case R_HPPA_GOTOFF_RS14:
      r_field = e_rssel;
      goto do_basic_type_2;
    case R_HPPA_GOTOFF_LD21:
      r_field = e_ldsel;
      goto do_basic_type_2;
    case R_HPPA_GOTOFF_RD11:
      r_field = e_rdsel;
      goto do_basic_type_2;
    case R_HPPA_GOTOFF_RD14:
      r_field = e_rdsel;
      goto do_basic_type_2;
    case R_HPPA_GOTOFF_LR21:
      r_field = e_lrsel;
      goto do_basic_type_2;
    case R_HPPA_GOTOFF_RR14:
      r_field = e_rrsel;

    do_basic_type_2:
      sym_value -= GOT_value;
      insn = hppa_elf_relocate_insn (abfd, input_section, insn, addr,
				     symbol_in, sym_value, r_addend,
				     r_type, r_format, r_field, r_pcrel);
      break;

    /* Handle all the basic type 3 relocations.  */
    case R_HPPA_ABS_CALL_11:
      r_field = e_fsel;
      goto do_basic_type_3;
    case R_HPPA_ABS_CALL_14:
      r_field = e_fsel;
      goto do_basic_type_3;
    case R_HPPA_ABS_CALL_17:
      r_field = e_fsel;
      goto do_basic_type_3;
    case R_HPPA_ABS_CALL_L21:
      r_field = e_lsel;
      goto do_basic_type_3;
    case R_HPPA_ABS_CALL_R11:
      r_field = e_rsel;
      goto do_basic_type_3;
    case R_HPPA_ABS_CALL_R14:
      r_field = e_rsel;
      goto do_basic_type_3;
    case R_HPPA_ABS_CALL_R17:
      r_field = e_rsel;
      goto do_basic_type_3;
    case R_HPPA_ABS_CALL_LS21:
      r_field = e_lssel;
      goto do_basic_type_3;
    case R_HPPA_ABS_CALL_RS11:
      r_field = e_lssel;
      goto do_basic_type_3;
    case R_HPPA_ABS_CALL_RS14:
      r_field = e_rssel;
      goto do_basic_type_3;
    case R_HPPA_ABS_CALL_RS17:
      r_field = e_rssel;
      goto do_basic_type_3;
    case R_HPPA_ABS_CALL_LD21:
      r_field = e_ldsel;
      goto do_basic_type_3;
    case R_HPPA_ABS_CALL_RD11:
      r_field = e_rdsel;
      goto do_basic_type_3;
    case R_HPPA_ABS_CALL_RD14:
      r_field = e_rdsel;
      goto do_basic_type_3;
    case R_HPPA_ABS_CALL_RD17:
      r_field = e_rdsel;
      goto do_basic_type_3;
    case R_HPPA_ABS_CALL_LR21:
      r_field = e_lrsel;
      goto do_basic_type_3;
    case R_HPPA_ABS_CALL_RR14:
      r_field = e_rrsel;
      goto do_basic_type_3;
    case R_HPPA_ABS_CALL_RR17:
      r_field = e_rrsel;

    do_basic_type_3:
      insn = hppa_elf_relocate_insn (abfd, input_section, insn, addr,
				     symbol_in, sym_value, r_addend,
				     r_type, r_format, r_field, r_pcrel);
      break;

    /* Handle all the basic type 4 relocations.  */  
    case R_HPPA_PCREL_CALL_11:
      r_field = e_fsel;
      goto do_basic_type_4;
    case R_HPPA_PCREL_CALL_14:
      r_field = e_fsel;
      goto do_basic_type_4;
    case R_HPPA_PCREL_CALL_17:
      r_field = e_fsel;
      goto do_basic_type_4;
    case R_HPPA_PCREL_CALL_L21:
      r_field = e_lsel;
      goto do_basic_type_4;
    case R_HPPA_PCREL_CALL_R11:
      r_field = e_rsel;
      goto do_basic_type_4;
    case R_HPPA_PCREL_CALL_R14:
      r_field = e_rsel;
      goto do_basic_type_4;
    case R_HPPA_PCREL_CALL_R17:
      r_field = e_rsel;
      goto do_basic_type_4;
    case R_HPPA_PCREL_CALL_LS21:
      r_field = e_lssel;
      goto do_basic_type_4;
    case R_HPPA_PCREL_CALL_RS11:
      r_field = e_rssel;
      goto do_basic_type_4;
    case R_HPPA_PCREL_CALL_RS14:
      r_field = e_rssel;
      goto do_basic_type_4;
    case R_HPPA_PCREL_CALL_RS17:
      r_field = e_rssel;
      goto do_basic_type_4;
    case R_HPPA_PCREL_CALL_LD21:
      r_field = e_ldsel;
      goto do_basic_type_4;
    case R_HPPA_PCREL_CALL_RD11:
      r_field = e_rdsel;
      goto do_basic_type_4;
    case R_HPPA_PCREL_CALL_RD14:
      r_field = e_rdsel;
      goto do_basic_type_4;
    case R_HPPA_PCREL_CALL_RD17:
      r_field = e_rdsel;
      goto do_basic_type_4;
    case R_HPPA_PCREL_CALL_LR21:
      r_field = e_lrsel;
      goto do_basic_type_4;
    case R_HPPA_PCREL_CALL_RR14:
      r_field = e_rrsel;
      goto do_basic_type_4;
    case R_HPPA_PCREL_CALL_RR17:
      r_field = e_rrsel;

    do_basic_type_4:
      insn = hppa_elf_relocate_insn (abfd, input_section, insn, addr,
				     symbol_in, sym_value, r_addend,
				     r_type, r_format, r_field, r_pcrel);
      break;

    /* Handle all the basic type 5 relocations.  */  
    case R_HPPA_PLABEL_32:
    case R_HPPA_PLABEL_11:
    case R_HPPA_PLABEL_14:
      r_field = e_fsel;
      goto do_basic_type_5;
    case R_HPPA_PLABEL_L21:
      r_field = e_lsel;
      goto do_basic_type_5;
    case R_HPPA_PLABEL_R11:
    case R_HPPA_PLABEL_R14:
      r_field = e_rsel;
    do_basic_type_5:
      insn = hppa_elf_relocate_insn (abfd, input_section, insn, addr,
				     symbol_in, sym_value, r_addend,
				     r_type, r_format, r_field, r_pcrel);
      break;

    /* Handle all basic type 6 relocations.  */
    case R_HPPA_UNWIND_ENTRY:
    case R_HPPA_UNWIND_ENTRIES:
      hppa_elf_relocate_unwind_table (abfd, input_section, data, addr,
				      symbol_in, sym_value, r_addend,
				      r_type, r_format, r_field, r_pcrel);
      return bfd_reloc_ok;

    /* Handle the stack operations and similar braindamage.  */
    case R_HPPA_PUSH_CONST:
    case R_HPPA_PUSH_PC:
    case R_HPPA_PUSH_SYM:
    case R_HPPA_PUSH_GOTOFF:
    case R_HPPA_PUSH_ABS_CALL:
    case R_HPPA_PUSH_PCREL_CALL:
    case R_HPPA_PUSH_PLABEL:
    case R_HPPA_MAX:
    case R_HPPA_MIN:
    case R_HPPA_ADD:
    case R_HPPA_SUB:
    case R_HPPA_MULT:
    case R_HPPA_DIV:
    case R_HPPA_MOD:
    case R_HPPA_AND:
    case R_HPPA_OR:
    case R_HPPA_XOR:
    case R_HPPA_NOT:
    case R_HPPA_LSHIFT:
    case R_HPPA_ARITH_RSHIFT:
    case R_HPPA_LOGIC_RSHIFT:
    case R_HPPA_EXPR_F:
    case R_HPPA_EXPR_L:
    case R_HPPA_EXPR_R:
    case R_HPPA_EXPR_LS:
    case R_HPPA_EXPR_RS:
    case R_HPPA_EXPR_LD:
    case R_HPPA_EXPR_RD:
    case R_HPPA_EXPR_LR:
    case R_HPPA_EXPR_RR:
    case R_HPPA_EXPR_32:
    case R_HPPA_EXPR_21:
    case R_HPPA_EXPR_11:
    case R_HPPA_EXPR_14:
    case R_HPPA_EXPR_17:
    case R_HPPA_EXPR_12:
      fprintf (stderr, "Relocation problem: ");
      fprintf (stderr, "Unimplemented reloc type %d, in module %s\n",
	       r_type, abfd->filename);
      return bfd_reloc_notsupported;

    /* This is a linker internal relocation.  */
    case R_HPPA_STUB_CALL_17:
      /* This relocation is for a branch to a long branch stub.
	 Change instruction to a BLE,N.  It may also be necessary
	 to change interchange the branch and its delay slot.
	 The original instruction stream is

	    bl <foo>,r		; call foo using register r as
				; the return pointer
	    XXX			; delay slot instruction

	 The new instruction stream will be:

	    XXX			; delay slot instruction
	    ble <foo_stub>	; call the long call stub for foo
				; using r31 as the return pointer

	 This braindamage is necessary because the compiler may put
	 an instruction which uses %r31 in the delay slot of the original
	 call.  By changing the call instruction from a "bl" to a "ble"
	 %r31 gets clobbered before the delay slot executes.

	 We do not interchange the branch and delay slot if the delay
	 slot was already nullified, or if the instruction in the delay
	 slot modifies the return pointer to avoid an unconditional
	 jump after the call returns (GCC optimization).  */
	 
      if (insn & 2)
        {
	  insn = BLE_N_XXX_0_0;
	  bfd_put_32 (abfd, insn, hit_data);
	  r_type = R_HPPA_ABS_CALL_17;
	  r_pcrel = 0;
	  insn = hppa_elf_relocate_insn (abfd, input_section, insn,
					 addr, symbol_in, sym_value,
					 r_addend, r_type, r_format,
					 r_field, r_pcrel);
        }
      else
	{
	  unsigned long old_delay_slot_insn = bfd_get_32 (abfd, hit_data + 4);
	  unsigned rtn_reg = (insn & 0x03e00000) >> 21;

	  if (get_opcode (old_delay_slot_insn) == LDO)
	    {
	      unsigned ldo_src_reg = (old_delay_slot_insn & 0x03e00000) >> 21;
	      unsigned ldo_target_reg = (old_delay_slot_insn & 0x001f0000) >> 16;

	      /* If the target of the LDO is the same as the return
		 register then there is no reordering.  We can leave the
		 instuction as a non-nullified BLE in this case.  */
	      if (ldo_target_reg == rtn_reg)
		{
		  unsigned long new_delay_slot_insn = old_delay_slot_insn;

		  BFD_ASSERT(ldo_src_reg == ldo_target_reg);
		  new_delay_slot_insn &= 0xfc00ffff;
		  new_delay_slot_insn |= ((31 << 21) | (31 << 16));
		  bfd_put_32 (abfd, new_delay_slot_insn, hit_data + 4);
		  insn = BLE_XXX_0_0;
		  r_type = R_HPPA_ABS_CALL_17;
		  r_pcrel = 0;
		  insn = hppa_elf_relocate_insn (abfd, input_section, insn,
						 addr, symbol_in, sym_value,
						 r_addend, r_type, r_format,
						 r_field, r_pcrel);
		  bfd_put_32 (abfd, insn, hit_data);
		  return bfd_reloc_ok;
		}
              else if (rtn_reg == 31)
                {
                  /* The return register is r31, so this is a millicode
		     call.  Do not perform any instruction reordering.  */
	          insn = BLE_XXX_0_0;
	          r_type = R_HPPA_ABS_CALL_17;
	          r_pcrel = 0;
	          insn = hppa_elf_relocate_insn (abfd, input_section, insn,
					         addr, symbol_in, sym_value,
					         r_addend, r_type, r_format,
					         r_field, r_pcrel);
	          bfd_put_32 (abfd, insn, hit_data);
	          return bfd_reloc_ok;
                }
	      else
		{
		  /* Check to see if the delay slot instruction has a
		     relocation.  If so, we need to change the address
		     field of it, because the instruction it relocates
		     is going to be moved.  */
		  arelent * next_reloc_entry = reloc_entry+1;

		  if (next_reloc_entry->address == reloc_entry->address + 4)
		    next_reloc_entry->address -= 4;

		  insn = old_delay_slot_insn;
		  bfd_put_32 (abfd, insn, hit_data);
		  insn = BLE_N_XXX_0_0;
		  bfd_put_32 (abfd, insn, hit_data + 4);
		  r_type = R_HPPA_ABS_CALL_17;
		  r_pcrel = 0;
		  insn = hppa_elf_relocate_insn (abfd, input_section, insn,
						 addr + 4, symbol_in, 
						 sym_value, r_addend, r_type,
						 r_format, r_field, r_pcrel);
		  bfd_put_32 (abfd, insn, hit_data + 4);
		  return bfd_reloc_ok;
		}
	    }
          else if (rtn_reg == 31)
            {
              /* The return register is r31, so this is a millicode call.
		 Perform no instruction reordering in this case.  */
	      insn = BLE_XXX_0_0;
	      r_type = R_HPPA_ABS_CALL_17;
	      r_pcrel = 0;
	      insn = hppa_elf_relocate_insn (abfd, input_section, insn,
					     addr, symbol_in, sym_value,
					     r_addend, r_type, r_format,
					     r_field, r_pcrel);
	      bfd_put_32 (abfd, insn, hit_data);
	      return bfd_reloc_ok;
            }
	  else
	    {
	      /* Check to see if the delay slot instruction has a
		 relocation.  If so, we need to change its address
		 field  because the instruction it relocates is going
		 to be moved.  */
	      arelent * next_reloc_entry = reloc_entry+1;

	      if (next_reloc_entry->address == reloc_entry->address + 4)
		next_reloc_entry->address -= 4;

	      insn = old_delay_slot_insn;
	      bfd_put_32 (abfd, insn, hit_data);
	      insn = BLE_N_XXX_0_0;
	      bfd_put_32 (abfd, insn, hit_data + 4);
	      r_type = R_HPPA_ABS_CALL_17;
	      r_pcrel = 0;
	      insn = hppa_elf_relocate_insn (abfd, input_section, insn,
					     addr + 4, symbol_in, sym_value,
					     r_addend, r_type, r_format,
					     r_field, r_pcrel);
	      bfd_put_32 (abfd, insn, hit_data + 4);
	      return bfd_reloc_ok;
	    }
	}
      break;
      
    default:
      fprintf (stderr, "Relocation problem : ");
      fprintf (stderr, "Unrecognized reloc type %d, in module %s\n",
	       r_type, abfd->filename);
      return bfd_reloc_dangerous;
    }

  /* Update the instruction word.  */
  bfd_put_32 (abfd, insn, hit_data);
  return (bfd_reloc_ok);
}

static const reloc_howto_type *
elf_hppa_reloc_type_lookup (arch, code)
     bfd_arch_info_type *arch;
     bfd_reloc_code_real_type code;
{
  if ((int) code < (int) R_HPPA_UNIMPLEMENTED)
    {
      BFD_ASSERT ((int) elf_hppa_howto_table[(int) code].type == (int) code);
      return &elf_hppa_howto_table[(int) code];
    }

  return (reloc_howto_type *) 0;
}

#define bfd_elf32_bfd_reloc_type_lookup	elf_hppa_reloc_type_lookup


void
DEFUN (elf_hppa_tc_symbol, (abfd, symbolP, sym_idx),
       bfd * abfd AND
       elf_symbol_type * symbolP AND
       int sym_idx)
{
  symext_chainS *symextP;
  unsigned int arg_reloc;

  /* Only functions can have argument relocations.  */
  if (!(symbolP->symbol.flags & BSF_FUNCTION))
    return;

  arg_reloc = symbolP->tc_data.hppa_arg_reloc;

  /* If there are no argument relocation bits, then no relocation is
     necessary.  Do not add this to the symextn section.  */
  if (arg_reloc == 0)
    return;

  symextP = (symext_chainS *) bfd_alloc (abfd, sizeof (symext_chainS) * 2);

  symextP[0].entry = ELF32_HPPA_SX_WORD (HPPA_SXT_SYMNDX, sym_idx);
  symextP[0].next = &symextP[1];

  symextP[1].entry = ELF32_HPPA_SX_WORD (HPPA_SXT_ARG_RELOC, arg_reloc);
  symextP[1].next = NULL;

  if (symext_rootP == NULL)
    {
      symext_rootP = &symextP[0];
      symext_lastP = &symextP[1];
    }
  else
    {
      symext_lastP->next = &symextP[0];
      symext_lastP = &symextP[1];
    }
}

/* Accessor function for the list of symbol extension records. */
symext_chainS *elf32_hppa_get_symextn_chain()
{
  return symext_rootP;
}

static symext_entryS *symextn_contents;
static unsigned int symextn_contents_real_size;

void
DEFUN (elf_hppa_tc_make_sections, (abfd, ignored),
       bfd * abfd AND
       PTR ignored)
{
  symext_chainS *symextP;
  int size;
  int n;
  int i;
  void hppa_elf_stub_finish ();	/* forward declaration */
  asection *symextn_sec;

  hppa_elf_stub_finish (abfd);

  if (symext_rootP == NULL)
    return;

  for (n = 0, symextP = symext_rootP; symextP; symextP = symextP->next, ++n)
    ;

  size = sizeof (symext_entryS) * n;
  symextn_sec = bfd_get_section_by_name (abfd, SYMEXTN_SECTION_NAME);
  if (symextn_sec == (asection *) 0)
    {
      symextn_sec = bfd_make_section (abfd, SYMEXTN_SECTION_NAME);
      bfd_set_section_flags (abfd,
			     symextn_sec,
	 SEC_HAS_CONTENTS | SEC_LOAD | SEC_ALLOC | SEC_CODE | SEC_READONLY);
      symextn_sec->output_section = symextn_sec;
      symextn_sec->output_offset = 0;
      bfd_set_section_alignment (abfd, symextn_sec, 2);
    }
  symextn_contents = (symext_entryS *) bfd_alloc (abfd, size);

  for (i = 0, symextP = symext_rootP; symextP; symextP = symextP->next, ++i)
    symextn_contents[i] = symextP->entry;
  symextn_contents_real_size = size;
  bfd_set_section_size (abfd, symextn_sec, symextn_contents_real_size);

  return;
}

/* Support for HP PA-RISC stub generation.

   Written by

	Center for Software Science
	Department of Computer Science
	University of Utah

    */

/*
    HP-PA calling conventions state:

    1. an argument relocation stub is required whenever the callee and
    caller argument relocation bits do not match exactly.  The exception
    to this rule is if either the caller or callee argument relocation
    bit are 00 (do not relocate).

    2. The linker can optionally add a symbol record for the stub so that
    the stub can be reused.  The symbol record will be the same as the
    original export symbol record, except that the relocation bits will
    reflect the input of the stub, the type would be STUB and the symbol
    value will be the location of the relocation stub.

    Other notes:

    Stubs can be inserted *before* the section of the caller.  The stubs
    can be treated as calls to code that manipulates the arguments.

 */

typedef enum
  {
    HPPA_STUB_ILLEGAL,
    HPPA_STUB_ARG_RELOC,
    HPPA_STUB_LONG_BRANCH
  } hppa_stub_type;

symext_entryS
elf32_hppa_get_sym_extn (abfd, sym, type)
     bfd *abfd;
     asymbol *sym;
     int type;
{
  /* This function finds the symbol extension record of the */
  /* specified type for the specified symbol.  It returns the */
  /* value of the symbol extension record.	*/
  symext_entryS retval;

  switch (type)
    {
    case HPPA_SXT_NULL:
      retval = (symext_entryS) 0;
      break;
    case HPPA_SXT_SYMNDX:
      retval = (symext_entryS) 0;	/* XXX: need to fix this */
      break;
    case HPPA_SXT_ARG_RELOC:
      {
	elf_symbol_type *esymP = (elf_symbol_type *) sym;

	retval = (symext_entryS) esymP->tc_data.hppa_arg_reloc;
	break;
      }
    /* This should never happen.  */
    default:
      abort();
    }
  return retval;
}

typedef struct elf32_hppa_stub_name_list_struct
{
  /* name of this stub  */
  asymbol *sym;
  /* stub description for this stub  */
  struct elf32_hppa_stub_description_struct *stub_desc;
  /* pointer into stub contents  */
  int *stub_secp;
  /* size of this stub  */
  unsigned size;
  /* next stub name entry  */
  struct elf32_hppa_stub_name_list_struct *next;
} elf32_hppa_stub_name_list;

typedef struct elf32_hppa_stub_description_struct
  {
    struct elf32_hppa_stub_description_struct *next;
    bfd *this_bfd;		/* bfd to which this stub applies */
    asection *stub_sec;		/* stub section for this bfd */
    unsigned relocs_allocated_cnt; /* count of relocations for this stub section */
    unsigned real_size;
    unsigned allocated_size;
    int *stub_secp;		/* pointer to the next available location in the buffer */
    char *stub_contents;	/* contents of the stubs for this bfd */
    elf32_hppa_stub_name_list *stub_listP;
  }
elf32_hppa_stub_description;

static elf32_hppa_stub_description *elf_hppa_stub_rootP;

/* Locate the stub section information for the given bfd. */
static elf32_hppa_stub_description *
find_stubs (abfd, stub_sec)
     bfd *abfd;
     asection *stub_sec;
{
  elf32_hppa_stub_description *stubP;

  for (stubP = elf_hppa_stub_rootP; stubP; stubP = stubP->next)
    {
      if (stubP->this_bfd == abfd
	  && stubP->stub_sec == stub_sec)
	return stubP;
    }

  return (elf32_hppa_stub_description *) NULL;
}

static elf32_hppa_stub_description *
new_stub (abfd, stub_sec)
     bfd *abfd;
     asection *stub_sec;
{
  elf32_hppa_stub_description *stub = find_stubs (abfd, stub_sec);

  if (stub)
    return stub;

  stub = (elf32_hppa_stub_description *) bfd_zalloc (abfd, sizeof (elf32_hppa_stub_description));
  if (stub)
    {
      stub->this_bfd = abfd;
      stub->stub_sec = stub_sec;
      stub->real_size = 0;
      stub->allocated_size = 0;
      stub->stub_contents = NULL;
      stub->stub_secp = NULL;

      stub->next = elf_hppa_stub_rootP;
      elf_hppa_stub_rootP = stub;
    }
  else
    {
      bfd_error = no_memory;
      bfd_perror ("new_stub");
    }

  return stub;
}

/* Locate the stub by the given name.  */
static elf32_hppa_stub_name_list *
find_stub_by_name (abfd, stub_sec, name)
     bfd *abfd;
     asection *stub_sec;
     char *name;
{
  elf32_hppa_stub_description *stub = find_stubs (abfd, stub_sec);

  if (stub)
    {
      elf32_hppa_stub_name_list *name_listP;

      for (name_listP = stub->stub_listP; name_listP; name_listP = name_listP->next)
	{
	  if (!strcmp (name_listP->sym->name, name))
	    return name_listP;
	}
    }

  return 0;
}

/* Locate the stub by the given name.  */
static elf32_hppa_stub_name_list *
add_stub_by_name(abfd, stub_sec, sym)
     bfd *abfd;
     asection *stub_sec;
     asymbol *sym;
{
  elf32_hppa_stub_description *stub = find_stubs (abfd, stub_sec);
  elf32_hppa_stub_name_list *stub_entry;

  if (!stub)
    stub = new_stub(abfd, stub_sec);

  if (stub)
    {
      stub_entry = (elf32_hppa_stub_name_list *)
	bfd_zalloc (abfd, sizeof (elf32_hppa_stub_name_list));

      if (stub_entry)
	{
	  stub_entry->size = 0;
	  stub_entry->sym = sym;
	  stub_entry->stub_desc = stub;
	  /* First byte of this stub is the pointer to
	     the next available location in the stub buffer.  */
	  stub_entry->stub_secp = stub->stub_secp;
	  if (stub->stub_listP)
	    stub_entry->next = stub->stub_listP;
	  else
	    stub_entry->next = NULL;
	  stub->stub_listP = stub_entry;
	  return stub_entry;
	}
      else
	{
	  bfd_error = no_memory;
	  bfd_perror("add_stub_by_name");
	}
    }

  return (elf32_hppa_stub_name_list *)NULL;
}

#define ARGUMENTS	0
#define RETURN_VALUE	1

#define	NO_ARG_RELOC	0
#define R_TO_FR		1
#define R01_TO_FR	2
#define R23_TO_FR	3
#define FR_TO_R		4
#define FR_TO_R01	5
#define FR_TO_R23	6
#define	ARG_RELOC_ERR	7

#define ARG0	0
#define ARG1	1
#define ARG2	2
#define ARG3	3
#define RETVAL	4

#define AR_NO	0
#define AR_GR	1
#define AR_FR	2
#define AR_FU	3
/* FP register in arg0/arg1.  This value can only appear in the arg0 location. */
#define AR_DBL01	4
/* FP register in arg2/arg3.  This value can only appear in the arg2 location. */
#define AR_DBL23	5

#define AR_WARN(type,loc) \
  fprintf(stderr,"WARNING:  Illegal argument relocation: %s for %s\n", \
	  reloc_type_strings[type],reloc_loc_strings[loc])

static CONST char *CONST reloc_type_strings[] =
{
  "NONE", "GR->FR", "GR0,GR1->FR1", "GR2,GR3->FR3", "FR->GR", "FR->GR0,GR1", "FR->GR2,GR3", "ERROR"
};

static CONST char *CONST reloc_loc_strings[] =
{
  "ARG0", "ARG1", "ARG2", "ARG3", "RETVAL"
};

static CONST char mismatches[6][6] =
{				/* 	CALLEE NONE	CALLEE GR	CALLEE FR	CALLEE FU	CALLEE DBL01	CALLEE DBL23	*/
  /* CALLER NONE	*/
 {NO_ARG_RELOC, NO_ARG_RELOC, NO_ARG_RELOC, ARG_RELOC_ERR, NO_ARG_RELOC, NO_ARG_RELOC},
 /* CALLER GR	*/
 {NO_ARG_RELOC, NO_ARG_RELOC, R_TO_FR, ARG_RELOC_ERR, R01_TO_FR, ARG_RELOC_ERR},
 /* CALLER FR	*/
 {NO_ARG_RELOC, FR_TO_R, NO_ARG_RELOC, ARG_RELOC_ERR, ARG_RELOC_ERR},
 /* CALLER FU	*/
 {ARG_RELOC_ERR, ARG_RELOC_ERR, ARG_RELOC_ERR, ARG_RELOC_ERR, ARG_RELOC_ERR, ARG_RELOC_ERR},
 /* CALLER DBL01	*/
 {NO_ARG_RELOC, FR_TO_R01, NO_ARG_RELOC, ARG_RELOC_ERR, NO_ARG_RELOC, ARG_RELOC_ERR},
 /* CALLER DBL23	*/
 {NO_ARG_RELOC, FR_TO_R23, NO_ARG_RELOC, ARG_RELOC_ERR, ARG_RELOC_ERR, NO_ARG_RELOC},
};

static CONST char retval_mismatches[6][6] =
{	/* 	CALLEE NONE	CALLEE GR	CALLEE FR	CALLEE FU	CALLEE DBL01	CALLEE DBL23	*/
  /* CALLER NONE	*/
 {NO_ARG_RELOC, NO_ARG_RELOC, NO_ARG_RELOC, ARG_RELOC_ERR, NO_ARG_RELOC, NO_ARG_RELOC},
 /* CALLER GR	*/
 {NO_ARG_RELOC, NO_ARG_RELOC, FR_TO_R, ARG_RELOC_ERR, FR_TO_R01, ARG_RELOC_ERR},
 /* CALLER FR	*/
 {NO_ARG_RELOC, R_TO_FR, NO_ARG_RELOC, ARG_RELOC_ERR, ARG_RELOC_ERR, ARG_RELOC_ERR},
 /* CALLER FU	*/
 {ARG_RELOC_ERR, ARG_RELOC_ERR, ARG_RELOC_ERR, ARG_RELOC_ERR, ARG_RELOC_ERR, ARG_RELOC_ERR},
 /* CALLER DBL01	*/
 {NO_ARG_RELOC, R01_TO_FR, NO_ARG_RELOC, ARG_RELOC_ERR, NO_ARG_RELOC, ARG_RELOC_ERR},
 /* CALLER DBL23	*/
 {NO_ARG_RELOC, R23_TO_FR, NO_ARG_RELOC, ARG_RELOC_ERR, ARG_RELOC_ERR, NO_ARG_RELOC},
};

static int
type_of_mismatch (caller_bits, callee_bits, type)
     int caller_bits;
     int callee_bits;
     int type;
{
  switch (type)
    {
    case ARGUMENTS:
      return mismatches[caller_bits][callee_bits];
    case RETURN_VALUE:
      return retval_mismatches[caller_bits][callee_bits];
    }

  return 0;
}

#define EXTRACT_ARBITS(ar,which)	((ar) >> (8-(which*2))) & 3

#define NEW_INSTRUCTION(entry,insn)	\
{ \
  *((entry)->stub_desc->stub_secp)++ = (insn);	\
  (entry)->stub_desc->real_size += sizeof(int);	\
  (entry)->size += sizeof(int);	\
  bfd_set_section_size((entry)->stub_desc->this_bfd,	\
		       (entry)->stub_desc->stub_sec,	\
		       (entry)->stub_desc->real_size);	\
}

#define CURRENT_STUB_OFFSET(entry)	\
  ((int)(entry)->stub_desc->stub_secp \
   - (int)(entry)->stub_desc->stub_contents - 4)

static boolean stubs_finished = false;

void
hppa_elf_stub_finish (output_bfd)
     bfd *output_bfd;
{
  extern bfd_error_vector_type bfd_error_vector;
  elf32_hppa_stub_description *stub_list = elf_hppa_stub_rootP;
  /* All the stubs have been built.  Finish up building	*/
  /* stub section.  Apply relocations to the section.	*/

  if ( stubs_finished )
    return;

  for (; stub_list; stub_list = stub_list->next)
    {
      if (stub_list->real_size)
	{
	  bfd *stub_bfd = stub_list->this_bfd;
	  asection *stub_sec = bfd_get_section_by_name (stub_bfd, ".hppa_linker_stubs");
	  bfd_size_type reloc_size;
	  arelent **reloc_vector;

	  BFD_ASSERT (stub_sec == stub_list->stub_sec);
	  reloc_size = bfd_get_reloc_upper_bound (stub_bfd, stub_sec);
	  reloc_vector = (arelent **) alloca (reloc_size);

	  BFD_ASSERT (stub_sec);

	  /* We are not relaxing the section, so just copy the size info */
	  stub_sec->_cooked_size = stub_sec->_raw_size;
	  stub_sec->reloc_done = true;


	  if (bfd_canonicalize_reloc (stub_bfd,
				      stub_sec,
				      reloc_vector,
				      output_bfd->outsymbols))
	    {
	      arelent **parent;
	      for (parent = reloc_vector; *parent != (arelent *) NULL;
		   parent++)
		{
		  bfd_reloc_status_type r =
		  bfd_perform_relocation (stub_bfd,
					  *parent,
					  stub_list->stub_contents,
					  stub_sec, 0);


		  if (r != bfd_reloc_ok)
		    {
		      switch (r)
			{
			case bfd_reloc_undefined:
			  bfd_error_vector.undefined_symbol (*parent, NULL);
			  break;
			case bfd_reloc_dangerous:
			  bfd_error_vector.reloc_dangerous (*parent, NULL);
			  break;
			case bfd_reloc_outofrange:
			case bfd_reloc_overflow:
			  bfd_error_vector.reloc_value_truncated (*parent, NULL);
			  break;
			default:
			  abort ();
			  break;
			}
		    }
		}
	    }

	  bfd_set_section_contents (output_bfd,
				    stub_sec,
				    stub_list->stub_contents,
				    0,
				    stub_list->real_size);

	  free (reloc_vector);
	}
    }
  stubs_finished = true;
}

void
hppa_elf_stub_branch_reloc (stub_desc,	/* the bfd */
			    output_bfd,	/* the output bfd */
			    target_sym,	/* the target symbol */
			    offset)	/* the offset within the stub buffer (pre-calculated) */
     elf32_hppa_stub_description *stub_desc;
     bfd *output_bfd;
     asymbol *target_sym;
     int offset;
{
  /* Allocate a new relocation entry. */
  arelent relent;
  int size;

  if (stub_desc->relocs_allocated_cnt == stub_desc->stub_sec->reloc_count)
    {
      if (stub_desc->stub_sec->relocation == NULL)
	{
	  stub_desc->relocs_allocated_cnt = STUB_RELOC_INCR;
	  size = sizeof (arelent) * stub_desc->relocs_allocated_cnt;
	  stub_desc->stub_sec->relocation = (arelent *) zalloc (size);
	}
      else
	{
	  stub_desc->relocs_allocated_cnt += STUB_RELOC_INCR;
	  size = sizeof (arelent) * stub_desc->relocs_allocated_cnt;
	  stub_desc->stub_sec->relocation = (arelent *) realloc (stub_desc->stub_sec->relocation,
								 size);
	}
    }

  /* Fill in the details. */
  relent.address = offset;
  relent.addend = 0;
  relent.sym_ptr_ptr = (asymbol **) bfd_zalloc (stub_desc->this_bfd, sizeof (asymbol *));
  BFD_ASSERT (relent.sym_ptr_ptr);

  relent.sym_ptr_ptr[0] = target_sym;
  relent.howto = bfd_reloc_type_lookup (stub_desc->this_bfd, R_HPPA_PCREL_CALL_17);

  /* Save it in the array of relocations for the stub section. */

  memcpy (&stub_desc->stub_sec->relocation[stub_desc->stub_sec->reloc_count++],
	  &relent,
	  sizeof (arelent));
}

void
hppa_elf_stub_reloc (stub_desc,	/* the bfd */
		     output_bfd,	/* the output bfd */
		     target_sym,	/* the target symbol */
		     offset,	/* the offset within the stub buffer (pre-calculated) */
		     type)
elf32_hppa_stub_description *stub_desc;
bfd *output_bfd;
asymbol *target_sym;
int offset;
elf32_hppa_reloc_type type;
{
  /* Allocate a new relocation entry. */
  arelent relent;
  int size;
  Elf_Internal_Shdr *rela_hdr;

  if (stub_desc->relocs_allocated_cnt == stub_desc->stub_sec->reloc_count)
    {
      if (stub_desc->stub_sec->relocation == NULL)
	{
	  stub_desc->relocs_allocated_cnt = STUB_RELOC_INCR;
	  size = sizeof (arelent) * stub_desc->relocs_allocated_cnt;
	  stub_desc->stub_sec->relocation = (arelent *) zalloc (size);
	}
      else
	{
	  stub_desc->relocs_allocated_cnt += STUB_RELOC_INCR;
	  size = sizeof (arelent) * stub_desc->relocs_allocated_cnt;
	  stub_desc->stub_sec->relocation = (arelent *) realloc (stub_desc->stub_sec->relocation,
								 size);
	}
    }

  rela_hdr = &elf_section_data(stub_desc->stub_sec)->rel_hdr;
  rela_hdr->sh_size += sizeof(Elf32_External_Rela);

  /* Fill in the details. */
  relent.address = offset;
  relent.addend = 0;
  relent.sym_ptr_ptr = (asymbol **) bfd_zalloc (stub_desc->this_bfd, sizeof (asymbol *));
  BFD_ASSERT (relent.sym_ptr_ptr);

  relent.sym_ptr_ptr[0] = target_sym;
  relent.howto = bfd_reloc_type_lookup (stub_desc->this_bfd, type);

  /* Save it in the array of relocations for the stub section. */

  memcpy (&stub_desc->stub_sec->relocation[stub_desc->stub_sec->reloc_count++],
	  &relent,
	  sizeof (arelent));
}

asymbol *
hppa_elf_build_arg_reloc_stub (abfd, output_bfd, reloc_entry,
			       stub_types, rtn_adjust, data)
     bfd *abfd;
     bfd *output_bfd;
     arelent *reloc_entry;
     int stub_types[5];
     int rtn_adjust;
     unsigned *data;
{
  asection *stub_sec = bfd_get_section_by_name (abfd, ".hppa_linker_stubs");
  elf32_hppa_stub_description *stub_desc = find_stubs (abfd, stub_sec);
  asymbol *stub_sym = NULL;
  asymbol *target_sym = reloc_entry->sym_ptr_ptr[0];
  asection *output_text_section = bfd_get_section_by_name (output_bfd, ".text");
  int i;
  char stub_sym_name[128];
  elf32_hppa_stub_name_list *stub_entry;
  unsigned insn = data[0];

  /* Perform some additional checks on whether we should really do the
     return adjustment.  For example, if the instruction is nullified
     or if the delay slot contains an instruction that modifies the return
     pointer, then the branch instructions should not be rearranged
     (rtn_adjust is false).  */
  if (insn & 2 || insn == 0)
    rtn_adjust = false;
  else
    {
      unsigned delay_insn = data[1];

      if (get_opcode (delay_insn) == LDO
	  && (((insn & 0x03e00000) >> 21) == ((delay_insn & 0x001f0000) >> 16)))
	rtn_adjust = false;
    }

  /* See if the proper stub entry has already been made.  */
  if (!stub_sec)
    {
      BFD_ASSERT (stub_desc == NULL);
      stub_sec = bfd_make_section (abfd, ".hppa_linker_stubs");
      bfd_set_section_flags (abfd,
			     stub_sec,
			     SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD
			     | SEC_RELOC | SEC_CODE | SEC_READONLY);
      stub_sec->output_section = output_text_section->output_section;
      stub_sec->output_offset = 0;
      bfd_set_section_alignment (abfd, stub_sec, 2);
      stub_desc = new_stub (abfd, stub_sec);
    }

  /* Make the stub if we did not find one already.  */
  if (!stub_desc)
    stub_desc = new_stub (abfd, stub_sec);

  /* Allocate space to write the stub.
     FIXME.  Why using realloc?!?  */
  if (!stub_desc->stub_contents)
    {
      stub_desc->allocated_size = STUB_BUFFER_INCR;
      stub_desc->stub_contents = (char *) bfd_xmalloc (STUB_BUFFER_INCR);
    }
  else if ((stub_desc->allocated_size - stub_desc->real_size) < STUB_MAX_SIZE)
    {
      stub_desc->allocated_size = stub_desc->allocated_size + STUB_BUFFER_INCR;
      stub_desc->stub_contents = (char *) realloc (stub_desc->stub_contents,
						   stub_desc->allocated_size);
    }

  stub_desc->stub_secp
    = (int *) (stub_desc->stub_contents + stub_desc->real_size);

  sprintf (stub_sym_name,
	   "_stub_%s_%02d_%02d_%02d_%02d_%02d_%s",
	   reloc_entry->sym_ptr_ptr[0]->name,
	   stub_types[0], stub_types[1], stub_types[2],
	   stub_types[3], stub_types[4],
	   rtn_adjust ? "RA" : "");
  stub_entry = find_stub_by_name (abfd, stub_sec, stub_sym_name);

  if (stub_entry)
    {
      stub_sym = stub_entry->sym;
      /* Redirect the original relocation from the old symbol (a function)
	 to the stub (the stub calls the function).  Should we need to
	 change the relocation type?  */
      reloc_entry->sym_ptr_ptr = (asymbol **) bfd_zalloc (stub_desc->this_bfd,
							  sizeof (asymbol *));
      reloc_entry->sym_ptr_ptr[0] = stub_sym;
      if (reloc_entry->howto->type != R_HPPA_PLABEL_32
	  && (get_opcode(insn) == BLE
	      || get_opcode (insn) == BE
	      || get_opcode (insn) == BL))
	reloc_entry->howto = bfd_reloc_type_lookup (abfd, R_HPPA_STUB_CALL_17);
    }
  else
    {
      /* Create a new symbol to point to this stub.  */
      stub_sym = bfd_make_empty_symbol (abfd);
      stub_sym->name = bfd_zalloc (abfd, strlen (stub_sym_name) + 1);
      strcpy ((char *) stub_sym->name, stub_sym_name);
      stub_sym->value
	= (int) stub_desc->stub_secp - (int) stub_desc->stub_contents;
      stub_sym->section = stub_sec;
      stub_sym->flags = BSF_LOCAL | BSF_FUNCTION;
      stub_entry = add_stub_by_name (abfd, stub_sec, stub_sym);

      /* Redirect the original relocation from the old symbol (a function)
	 to the stub (the stub calls the function).  Change the type of
	 relocation to be the internal use only stub R_HPPA_STUB_CALL_17.  */
      reloc_entry->sym_ptr_ptr = (asymbol **) bfd_zalloc (stub_desc->this_bfd,
							  sizeof (asymbol *));
      reloc_entry->sym_ptr_ptr[0] = stub_sym;
      if (reloc_entry->howto->type != R_HPPA_PLABEL_32
	  && (get_opcode (insn) == BLE
	      || get_opcode (insn) == BE
	      || get_opcode (insn) == BL))
	reloc_entry->howto = bfd_reloc_type_lookup (abfd, R_HPPA_STUB_CALL_17);

      /* Generate common code for all stubs.  */

      NEW_INSTRUCTION (stub_entry, LDSID_31_1);
      NEW_INSTRUCTION (stub_entry, MTSP_1_SR0);
      NEW_INSTRUCTION (stub_entry, ADDI_8_SP);

      /* Generate code to move the arguments around.  */
      for (i = ARG0; i < ARG3; i++)
	{
	  if (stub_types[i] != NO_ARG_RELOC)
	    {
	      switch (stub_types[i])
		{
		case R_TO_FR:
		  switch (i)
		    {
		    case ARG0:
		      NEW_INSTRUCTION (stub_entry, STWS_ARG0_M8SP);
		      NEW_INSTRUCTION (stub_entry, FLDWS_M8SP_FARG0);
		      break;
		    case ARG1:
		      NEW_INSTRUCTION (stub_entry, STWS_ARG1_M8SP);
		      NEW_INSTRUCTION (stub_entry, FLDWS_M8SP_FARG1);
		      break;
		    case ARG2:
		      NEW_INSTRUCTION (stub_entry, STWS_ARG2_M8SP);
		      NEW_INSTRUCTION (stub_entry, FLDWS_M8SP_FARG2);
		      break;
		    case ARG3:
		      NEW_INSTRUCTION (stub_entry, STWS_ARG3_M8SP);
		      NEW_INSTRUCTION (stub_entry, FLDWS_M8SP_FARG3);
		      break;
		    }
		  continue;
		  
		case R01_TO_FR:
		  switch (i)
		    {
		    case ARG0:
		      NEW_INSTRUCTION (stub_entry, STWS_ARG0_M4SP);
		      NEW_INSTRUCTION (stub_entry, STWS_ARG1_M8SP);
		      NEW_INSTRUCTION (stub_entry, FLDDS_M8SP_FARG1);
		      break;
		    default:
		      AR_WARN (stub_types[i],i);
		      break;
		    }
		  continue;
		  
		case R23_TO_FR:
		  switch (i)
		    {
		    case ARG2:
		      NEW_INSTRUCTION (stub_entry, STWS_ARG2_M4SP);
		      NEW_INSTRUCTION (stub_entry, STWS_ARG3_M8SP);
		      NEW_INSTRUCTION (stub_entry, FLDDS_M8SP_FARG3);
		      break;
		    default:
		      AR_WARN (stub_types[i],i);
		      break;
		    }
		  continue;
		  
		case FR_TO_R:
		  switch (i)
		    {
		    case ARG0:
		      NEW_INSTRUCTION (stub_entry, FSTWS_FARG0_M8SP);
		      NEW_INSTRUCTION (stub_entry, LDWS_M4SP_ARG0);
		      break;
		    case ARG1:
		      NEW_INSTRUCTION (stub_entry, FSTWS_FARG1_M8SP);
		      NEW_INSTRUCTION (stub_entry, LDWS_M4SP_ARG1);
		      break;
		    case ARG2:
		      NEW_INSTRUCTION (stub_entry, FSTWS_FARG2_M8SP);
		      NEW_INSTRUCTION (stub_entry, LDWS_M4SP_ARG2);
		      break;
		    case ARG3:
		      NEW_INSTRUCTION (stub_entry, FSTWS_FARG3_M8SP);
		      NEW_INSTRUCTION (stub_entry, LDWS_M4SP_ARG3);
		      break;
		    }
		  continue;
		  
		case FR_TO_R01:
		  switch (i)
		    {
		    case ARG0:
		      NEW_INSTRUCTION (stub_entry, FSTDS_FARG1_M8SP);
		      NEW_INSTRUCTION (stub_entry, LDWS_M4SP_ARG0);
		      NEW_INSTRUCTION (stub_entry, LDWS_M8SP_ARG1);
		      break;
		    default:
		      AR_WARN (stub_types[i],i);
		      break;
		    }
		  continue;
		  
		case FR_TO_R23:
		  switch (i)
		    {
		    case ARG2:
		      NEW_INSTRUCTION (stub_entry, FSTDS_FARG3_M8SP);
		      NEW_INSTRUCTION (stub_entry, LDWS_M4SP_ARG2);
		      NEW_INSTRUCTION (stub_entry, LDWS_M8SP_ARG3);
		      break;
		    default:
		      AR_WARN (stub_types[i],i);
		      break;
		    }
		  continue;
		  
		}
	    }
	}

      NEW_INSTRUCTION (stub_entry, ADDI_M8_SP_SP);

      /* Adjust the return address if necessary.  */
      if (rtn_adjust)
	{
	  NEW_INSTRUCTION (stub_entry, ADDI_M4_31_RP);
	}

      /* Save the return address.  */
      NEW_INSTRUCTION (stub_entry, STW_RP_M8SP);

      /* Long branch to the target function.  */
      NEW_INSTRUCTION (stub_entry, LDIL_XXX_31);
      hppa_elf_stub_reloc (stub_entry->stub_desc,
			   abfd,
			   target_sym,
			   CURRENT_STUB_OFFSET (stub_entry),
			   R_HPPA_L21);
      NEW_INSTRUCTION (stub_entry, BLE_XXX_0_31);
      hppa_elf_stub_reloc (stub_entry->stub_desc,
			   abfd,
			   target_sym,
			   CURRENT_STUB_OFFSET (stub_entry),
			   R_HPPA_ABS_CALL_R17);
      NEW_INSTRUCTION (stub_entry, COPY_31_2);
      

      /* Restore the return address.  */
      NEW_INSTRUCTION (stub_entry, LDW_M8SP_RP);
      
      /* Generate the code to move the return value around.  */
      i = RETVAL;
      if (stub_types[i] != NO_ARG_RELOC)
	{
	  switch (stub_types[i])
	    {
	    case R_TO_FR:
	      NEW_INSTRUCTION (stub_entry, STWS_RET0_M8SP);
	      NEW_INSTRUCTION (stub_entry, FLDWS_M8SP_FRET0);
	      break;
	      
	    case FR_TO_R:
	      NEW_INSTRUCTION (stub_entry, FSTWS_FRET0_M8SP);
	      NEW_INSTRUCTION (stub_entry, LDWS_M4SP_RET0);
	      break;
	    }
	}
      NEW_INSTRUCTION (stub_entry, BV_N_0_RP);
    }

  return stub_sym;
}

int
hppa_elf_arg_reloc_needed_p (abfd, reloc_entry, stub_types, caller_ar)
     bfd *abfd;
     arelent *reloc_entry;
     int stub_types[5];
     symext_entryS caller_ar;
{
  /* If the symbol is still undefined, there is	*/
  /* no way to know if a stub is required.	*/

  if (reloc_entry->sym_ptr_ptr[0] && reloc_entry->sym_ptr_ptr[0]->section != &bfd_und_section)
    {
      symext_entryS callee_ar = elf32_hppa_get_sym_extn (abfd,
						reloc_entry->sym_ptr_ptr[0],
							 HPPA_SXT_ARG_RELOC);

      /* Now, determine if a stub is */
      /* required.  A stub is required if they the callee and caller	*/
      /* argument relocation bits are both nonzero and not equal.	*/

      if (caller_ar && callee_ar)
	{
	  /* Both are non-zero, we need to do further checking. */
	  /* First, check if there is a return value relocation to be done */
	  int caller_loc[5];
	  int callee_loc[5];

	  callee_loc[RETVAL] = EXTRACT_ARBITS (callee_ar, RETVAL);
	  caller_loc[RETVAL] = EXTRACT_ARBITS (caller_ar, RETVAL);
	  callee_loc[ARG0] = EXTRACT_ARBITS (callee_ar, ARG0);
	  caller_loc[ARG0] = EXTRACT_ARBITS (caller_ar, ARG0);
	  callee_loc[ARG1] = EXTRACT_ARBITS (callee_ar, ARG1);
	  caller_loc[ARG1] = EXTRACT_ARBITS (caller_ar, ARG1);
	  callee_loc[ARG2] = EXTRACT_ARBITS (callee_ar, ARG2);
	  caller_loc[ARG2] = EXTRACT_ARBITS (caller_ar, ARG2);
	  callee_loc[ARG3] = EXTRACT_ARBITS (callee_ar, ARG3);
	  caller_loc[ARG3] = EXTRACT_ARBITS (caller_ar, ARG3);

	  /* Check some special combinations.  For */
	  /* example, if FU appears in ARG1 or ARG3, we */
	  /* can move it to ARG0 or ARG2, respectively. */

	  if (caller_loc[ARG0] == AR_FU || caller_loc[ARG1] == AR_FU)
	    {
	      caller_loc[ARG0] = AR_DBL01;
	      caller_loc[ARG1] = AR_NO;
	    }
	  if (caller_loc[ARG2] == AR_FU || caller_loc[ARG3] == AR_FU)
	    {
	      caller_loc[ARG2] = AR_DBL23;
	      caller_loc[ARG3] = AR_NO;
	    }
	  if (callee_loc[ARG0] == AR_FU || callee_loc[ARG1] == AR_FU)
	    {
	      callee_loc[ARG0] = AR_DBL01;
	      callee_loc[ARG1] = AR_NO;
	    }
	  if (callee_loc[ARG2] == AR_FU || callee_loc[ARG3] == AR_FU)
	    {
	      callee_loc[ARG2] = AR_DBL23;
	      callee_loc[ARG3] = AR_NO;
	    }

	  stub_types[ARG0] = type_of_mismatch (caller_loc[ARG0], callee_loc[ARG0], ARGUMENTS);
	  stub_types[ARG1] = type_of_mismatch (caller_loc[ARG1], callee_loc[ARG1], ARGUMENTS);
	  stub_types[ARG2] = type_of_mismatch (caller_loc[ARG2], callee_loc[ARG2], ARGUMENTS);
	  stub_types[ARG3] = type_of_mismatch (caller_loc[ARG3], callee_loc[ARG3], ARGUMENTS);
	  stub_types[RETVAL] = type_of_mismatch (caller_loc[RETVAL], callee_loc[RETVAL], RETURN_VALUE);

	  /* Steps involved in building stubs: */
	  /* 1. Determine what argument registers need to relocated.  This */
	  /* 	step is already done here. */
	  /* 2. Build the appropriate stub in the .hppa_linker_stubs section. */
	  /* 	This section should never appear in an object file.  It is */
	  /* 	only used internally.  The output_section of the */
	  /* 	.hppa_linker_stubs section is the .text section of the */
	  /* 	executable.	*/
	  /* 3. Build a symbol that is used (internally only) as the entry */
	  /*	point of the stub. */
	  /* 4. Change the instruction of the original branch into a branch to */
	  /* 	the stub routine. */
	  /* 5. Build a relocation entry for the instruction of the original */
	  /*	branch to be R_HPPA_PCREL_CALL to the stub routine. */


	  if (stub_types[0]
	      || stub_types[1]
	      || stub_types[2]
	      || stub_types[3]
	      || stub_types[4])
	    {
#ifdef DETECT_STUBS
	      int i;

	      fprintf (stderr, "Stub needed for %s @ %s+0x%x: callee/caller ar=0x%x/0x%x ",
		       reloc_entry->sym_ptr_ptr[0]->name,
		       abfd->filename, reloc_entry->address,
		       callee_ar, caller_ar);
	      for (i = ARG0; i < RETVAL; i++)
		{
		  if (stub_types[i] != NO_ARG_RELOC)
		    {
		      fprintf (stderr, "%s%d: %s ",
			       i == RETVAL ? "ret" : "arg",
			       i == RETVAL ? 0 : i,
			       reloc_type_strings[stub_types[i]]);
		    }
		}
	      fprintf (stderr, "\n");
#endif
	      return 1;
	    }

	}
    }
  return 0;
}

asymbol *
hppa_elf_build_long_branch_stub (abfd, output_bfd, reloc_entry, symbol, data)
     bfd *abfd;
     bfd *output_bfd;
     arelent *reloc_entry;
     asymbol *symbol;
     unsigned *data;
{
  asection *stub_sec = bfd_get_section_by_name (abfd, ".hppa_linker_stubs");
  elf32_hppa_stub_description *stub_desc = find_stubs (abfd, stub_sec);
  asymbol *stub_sym = NULL;
  asymbol *target_sym = reloc_entry->sym_ptr_ptr[0];
  asection *output_text_section = bfd_get_section_by_name (output_bfd, ".text");
  char stub_sym_name[128];
  int milli = false;
  int dyncall = false;
  elf32_hppa_stub_name_list *stub_entry;
  int rtn_adjust = true;
  int rtn_reg;
  unsigned insn;

  /* Create the stub section if it does not already exist.  */
  if (!stub_sec)
    {
      BFD_ASSERT (stub_desc == NULL);
      stub_sec = bfd_make_section (abfd, ".hppa_linker_stubs");
      bfd_set_section_flags (abfd,
			     stub_sec,
			     SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD
			     | SEC_RELOC | SEC_CODE | SEC_READONLY);
      stub_sec->output_section = output_text_section->output_section;
      stub_sec->output_offset = 0;

      /* Set up the ELF section header for this new section.  This
	 is basically the same processing as elf_make_sections().  
	 elf_make_sections is static and therefore not accessable
	 here.  */
      {
	Elf_Internal_Shdr *this_hdr;
	this_hdr = &elf_section_data (stub_sec)->this_hdr;
	
	/* Set the sizes of this section.  The contents have already
	   been set up ?!?  */
	this_hdr->sh_addr = stub_sec->vma;
	this_hdr->sh_size = stub_sec->_raw_size;
	
	/* Set appropriate flags for sections with relocations.  */
	if (stub_sec->flags & SEC_RELOC)
	  {
	    Elf_Internal_Shdr *rela_hdr;
	    int use_rela_p = get_elf_backend_data (abfd)->use_rela_p;
	    
	    rela_hdr = &elf_section_data (stub_sec)->rel_hdr;
	    
	    if (use_rela_p)
	      {
		rela_hdr->sh_type = SHT_RELA;
		rela_hdr->sh_entsize = sizeof (Elf32_External_Rela);
	      }
	    else
	      {
		rela_hdr->sh_type = SHT_REL;
		rela_hdr->sh_entsize = sizeof (Elf32_External_Rel);
	      }
	    rela_hdr->sh_flags = 0;
	    rela_hdr->sh_addr = 0;
	    rela_hdr->sh_offset = 0;
	    rela_hdr->sh_addralign = 0;
	    rela_hdr->size = 0;
	  }

	if (stub_sec->flags & SEC_ALLOC)
	  {
	    this_hdr->sh_flags |= SHF_ALLOC;
	    /* FIXME.  If SEC_LOAD is true should we do something with
	       with sh_type?  */
	  }

	if (!(stub_sec->flags & SEC_READONLY))
	  this_hdr->sh_flags |= SHF_WRITE;
	
	if (stub_sec->flags & SEC_CODE)
	  this_hdr->sh_flags |= SHF_EXECINSTR;
      }
      
      bfd_set_section_alignment (abfd, stub_sec, 2);
      stub_desc = new_stub (abfd, stub_sec);
    }
  
  if (!stub_desc)
    stub_desc = new_stub (abfd, stub_sec);
  
  /* Allocate memory to contain the stub.  FIXME.  Why isn't this using
     the BFD memory allocation routines?  */
  if (!stub_desc->stub_contents)
    {
      stub_desc->allocated_size = STUB_BUFFER_INCR;
      stub_desc->stub_contents = (char *) malloc (STUB_BUFFER_INCR);
    }
  else if ((stub_desc->allocated_size - stub_desc->real_size) < STUB_MAX_SIZE)
    {
      stub_desc->allocated_size = stub_desc->allocated_size + STUB_BUFFER_INCR;
      stub_desc->stub_contents = (char *) realloc (stub_desc->stub_contents,
						   stub_desc->allocated_size);
    }
  
  stub_desc->stub_secp
    = (int *) (stub_desc->stub_contents + stub_desc->real_size);
  
  /* Is this a millicode call?  If so, the return address
     comes in on r31 rather than r2 (rp) so a slightly
     different code sequence is needed.  */
  
  insn = data[0];
  rtn_reg = (insn & 0x03e00000) >> 21;
  if (rtn_reg == 31)
    milli = true;
  
  if (strcmp (symbol->name, "$$dyncall") == 0)
    dyncall = true;

  /* If we are creating a call from a stub to another stub, then
     never do the instruction reordering.  We can tell if we are
     going to be calling one stub from another by the fact that
     the symbol name has '_stub_' (arg. reloc. stub) or '_lb_stub_'
     prepended to the name.  Alternatively, the section of the
     symbol will be '.hppa_linker_stubs'.  */

  if ((strncmp (symbol->name, "_stub_", 6) == 0)
      || (strncmp (symbol->name, "_lb_stub_", 9) == 0))
    {
      BFD_ASSERT (strcmp (symbol->section->name, ".hppa_linker_stubs") == 0);
      rtn_adjust = false;
    }
  
  /* Check to see if we modify the return pointer
     in the delay slot of the branch.  */
  {
    unsigned delay_insn = data[1];
    
    /* If we nullify the delay slot, or if the delay slot contains an
       instruction that modifies the return pointer, then no additional
       modification of the return pointer is necessary.  */
    if (insn & 2 || insn == 0)
      rtn_adjust = false;
    else
      {
	if (get_opcode (delay_insn) == LDO
	    && (((delay_insn & 0x001f0000) >> 16) == rtn_reg))
	  rtn_adjust = false;
        if (milli)
          rtn_adjust = false;
      }
  }
  
  sprintf (stub_sym_name,
	   "_lb_stub_%s_%s", reloc_entry->sym_ptr_ptr[0]->name,
	   rtn_adjust ? "RA" : "");
  stub_entry = find_stub_by_name(abfd, stub_sec, stub_sym_name);

  /* If a copy of this stub already exists re-use it.  */
  if (stub_entry)
    {
      stub_sym = stub_entry->sym;

      /* Change symbol associated with the original relocation to point
	 to the stub.

	 FIXME.  Is there a need to change the relocation type too?  */
      reloc_entry->sym_ptr_ptr = (asymbol **) bfd_zalloc (stub_desc->this_bfd,
							  sizeof (asymbol *));
      reloc_entry->sym_ptr_ptr[0] = stub_sym;
      reloc_entry->howto = bfd_reloc_type_lookup (abfd, R_HPPA_STUB_CALL_17);
    }
  else
    {
      /* We will need to allocate a new stub.  */
      stub_sym = bfd_make_empty_symbol (abfd);
      stub_sym->name = bfd_zalloc (abfd, strlen (stub_sym_name) + 1);
      strcpy ((char *) stub_sym->name, stub_sym_name);
      stub_sym->value
	= (int) stub_desc->stub_secp - (int) stub_desc->stub_contents;
      stub_sym->section = stub_sec;
      stub_sym->flags = BSF_LOCAL | BSF_FUNCTION;
      stub_entry = add_stub_by_name (abfd, stub_sec, stub_sym);
      
      /* Change symbol associated with the original relocation to point
	 to the stub.

	 FIXME.  Is there a need to change the relocation type too?  */
      reloc_entry->sym_ptr_ptr = (asymbol **) bfd_zalloc (stub_desc->this_bfd,
							  sizeof (asymbol *));
      reloc_entry->sym_ptr_ptr[0] = stub_sym;
      reloc_entry->howto = bfd_reloc_type_lookup (abfd, R_HPPA_STUB_CALL_17);
      
      /* Build the stub.  */
      
      /* 1. initialization for the call. */
      NEW_INSTRUCTION (stub_entry, LDSID_31_1);
      NEW_INSTRUCTION (stub_entry, MTSP_1_SR0);
      
      if (!dyncall)
	{
	  if (!milli)
	    {
	      if (rtn_adjust)
		{
		  NEW_INSTRUCTION (stub_entry, ADDI_M4_31_RP);
		}
	      else
		{
		  NEW_INSTRUCTION (stub_entry, COPY_31_2);
		}
	    }
	  else
	    {
	      if (rtn_adjust)
		{
		  NEW_INSTRUCTION (stub_entry, ADDI_M4_31_1);
		}
	      else
		{
		  NEW_INSTRUCTION (stub_entry, COPY_31_1);
		}
	    }
	  
	  NEW_INSTRUCTION (stub_entry, LDIL_XXX_31);
	  hppa_elf_stub_reloc (stub_desc,
			       abfd,
			       target_sym,
			       CURRENT_STUB_OFFSET (stub_entry),
			       R_HPPA_L21);
	  
	  /* 2. Make the call. */
	  if (!milli)
	    {
	      NEW_INSTRUCTION (stub_entry, BE_XXX_0_31);
	      hppa_elf_stub_reloc (stub_desc,
				   abfd,
				   target_sym,
				   CURRENT_STUB_OFFSET (stub_entry),
				   R_HPPA_ABS_CALL_R17);
	      NEW_INSTRUCTION (stub_entry, COPY_2_31);
	    }
	  else
	    {
	      NEW_INSTRUCTION (stub_entry, BE_XXX_0_31);
	      hppa_elf_stub_reloc (stub_desc,
				   abfd,
				   target_sym,
				   CURRENT_STUB_OFFSET (stub_entry),
				   R_HPPA_ABS_CALL_R17);
	      NEW_INSTRUCTION (stub_entry, COPY_1_31);
	    }
	}
      else
	{
	  /* 3. Branch back to the original location.
	     (For non-millicode calls, this is accomplished with the
	     COPY_31_2 instruction.  For millicode calls, the return
	     location is already in r2.)  */
	  if (rtn_adjust)
	    {
	      NEW_INSTRUCTION (stub_entry, ADDI_M4_31_RP);
	    }
	  NEW_INSTRUCTION (stub_entry, LDIL_XXX_31);
	  hppa_elf_stub_reloc (stub_desc,
			       abfd,
			       target_sym,
			       CURRENT_STUB_OFFSET (stub_entry),
			       R_HPPA_L21);
	  
	  NEW_INSTRUCTION (stub_entry, BE_XXX_0_31);
	  hppa_elf_stub_reloc (stub_desc,
			       abfd,
			       target_sym,
			       CURRENT_STUB_OFFSET (stub_entry),
			       R_HPPA_ABS_CALL_R17);
	  NEW_INSTRUCTION (stub_entry, COPY_2_31);
	}
    }
  return stub_sym;
}

int
hppa_elf_long_branch_needed_p (abfd, asec, reloc_entry, symbol, insn)
     bfd *abfd;
     asection *asec;
     arelent *reloc_entry;
     asymbol *symbol;
     unsigned insn;
{
  long sym_value = get_symbol_value(symbol);
  int fmt = reloc_entry->howto->bitsize;
  unsigned char op = get_opcode(insn);
  unsigned raddr;

#define too_far(val,num_bits)	((int)(val) > (1<<(num_bits))-1) || ((int)(val) < (-1<<(num_bits)))

  switch (op)
    {
    case BL:
      raddr =
	reloc_entry->address + asec->output_offset + asec->output_section->vma;
      if ( too_far(sym_value - raddr,fmt+1) )
	{
#ifdef DETECT_STUBS
	  fprintf(stderr,"long_branch needed on BL insn: abfd=%s,sym=%s,distance=0x%x\n",abfd->filename,symbol->name,sym_value - reloc_entry->address);
#endif
	  return 1;
	}
      break;
    }  
  return 0;
}

#define STUB_SYM_BUFFER_INC	5

asymbol *
hppa_look_for_stubs_in_section (stub_bfd, abfd, output_bfd, asec,
				syms, new_sym_cnt)
     bfd *stub_bfd;
     bfd *abfd;
     bfd *output_bfd;
     asection *asec;
     asymbol **syms;
     int *new_sym_cnt;
{
  int i;
  int stub_types[5];
  asymbol *new_syms = (asymbol *) NULL;
  int new_cnt = 0;
  int new_max = 0;

  /* Relocations are in different places depending on whether this is
     an output section or an input section.  Also, the relocations are
     in different forms.  Sigh.  Luckily, we have
     bfd_canonicalize_reloc() to straighten this out for us . */

  if (asec->reloc_count > 0)
    {
      arelent **reloc_vector
	= (arelent **) alloca (asec->reloc_count * (sizeof (arelent *) + 1));

      bfd_canonicalize_reloc (abfd, asec, reloc_vector, syms);
      for (i = 0; i < asec->reloc_count; i++)
	{
	  arelent *rle = reloc_vector[i];

	  switch (rle->howto->type)
	    {
	    case R_HPPA_ABS_CALL_11:
	    case R_HPPA_ABS_CALL_14:
	    case R_HPPA_ABS_CALL_17:
	    case R_HPPA_ABS_CALL_L21:
	    case R_HPPA_ABS_CALL_R11:
	    case R_HPPA_ABS_CALL_R14:
	    case R_HPPA_ABS_CALL_R17:
	    case R_HPPA_ABS_CALL_LS21:
	    case R_HPPA_ABS_CALL_RS11:
	    case R_HPPA_ABS_CALL_RS14:
	    case R_HPPA_ABS_CALL_RS17:
	    case R_HPPA_ABS_CALL_LD21:
	    case R_HPPA_ABS_CALL_RD11:
	    case R_HPPA_ABS_CALL_RD14:
	    case R_HPPA_ABS_CALL_RD17:
	    case R_HPPA_ABS_CALL_LR21:
	    case R_HPPA_ABS_CALL_RR14:
	    case R_HPPA_ABS_CALL_RR17:
	    case R_HPPA_PCREL_CALL_11:
	    case R_HPPA_PCREL_CALL_14:
	    case R_HPPA_PCREL_CALL_17:
	    case R_HPPA_PCREL_CALL_12:
	    case R_HPPA_PCREL_CALL_L21:
	    case R_HPPA_PCREL_CALL_R11:
	    case R_HPPA_PCREL_CALL_R14:
	    case R_HPPA_PCREL_CALL_R17:
	    case R_HPPA_PCREL_CALL_LS21:
	    case R_HPPA_PCREL_CALL_RS11:
	    case R_HPPA_PCREL_CALL_RS14:
	    case R_HPPA_PCREL_CALL_RS17:
	    case R_HPPA_PCREL_CALL_LD21:
	    case R_HPPA_PCREL_CALL_RD11:
	    case R_HPPA_PCREL_CALL_RD14:
	    case R_HPPA_PCREL_CALL_RD17:
	    case R_HPPA_PCREL_CALL_LR21:
	    case R_HPPA_PCREL_CALL_RR14:
	    case R_HPPA_PCREL_CALL_RR17:
	      {
		symext_entryS caller_ar
		  = (symext_entryS) HPPA_R_ARG_RELOC (rle->addend);
		unsigned insn[2];

		bfd_get_section_contents (abfd, asec, insn, rle->address,
					  sizeof(insn));
		if (hppa_elf_arg_reloc_needed_p (abfd, rle, stub_types,
						 caller_ar))
		  {
		    /* Generate a stub and keep track of the new symbol.  */
		    asymbol *r;

		    if (new_cnt == new_max)
		      {
			new_max += STUB_SYM_BUFFER_INC;
			new_syms = (asymbol *)
			  realloc (new_syms, new_max * sizeof (asymbol));
		      }

		    /* The rtn_adjust argument is true here because we
		       know that we have a branch and (with a few exceptions
		       detailed under the relocation code for relocation type
		       R_HPPA_STUB_CALL_17) it will be possible to perform
		       the code reorientation.  */
		    r = hppa_elf_build_arg_reloc_stub (stub_bfd, output_bfd,
						       rle, stub_types,
						       true, insn);
		    new_syms[new_cnt++] = *r;
		  }

		/* We need to retrieve the section contents to check for
		   long branch stubs.  */
		if (hppa_elf_long_branch_needed_p (abfd, asec, rle,
						   rle->sym_ptr_ptr[0],
						   insn[0]))
		  {
		    /* Generate a stub and keep track of the new symbol.  */
		    asymbol *r;

		    if (new_cnt == new_max)
		      {
			new_max += STUB_SYM_BUFFER_INC;
			new_syms = (asymbol *)
			  realloc (new_syms, (new_max * sizeof (asymbol)));
		      }
		    r = hppa_elf_build_long_branch_stub (stub_bfd, output_bfd,
							 rle,
							 rle->sym_ptr_ptr[0],
							 insn);
		    new_syms[new_cnt++] = *r;
		  }
	      }
	      break;

	    case R_HPPA_PLABEL_32:
	    case R_HPPA_PLABEL_11:
	    case R_HPPA_PLABEL_14:
	    case R_HPPA_PLABEL_L21:
	    case R_HPPA_PLABEL_R11:
	    case R_HPPA_PLABEL_R14:
	      {
		/* On a plabel relocation, assume the arguments of the
		   caller are set up in general registers.
		   NOTE:  0x155 = ARGW0=CR,ARGW1=GR,ARGW2=GR,RETVAL=GR */
		symext_entryS caller_ar = (symext_entryS) 0x155;
		unsigned insn[2];

		bfd_get_section_contents (abfd, asec, insn, rle->address,
					  sizeof(insn));

		if (hppa_elf_arg_reloc_needed_p (abfd, rle, stub_types,
						 caller_ar))
		  {
		    /* Generate a plabel stub and keep track of the
		       new symbol.  */
		    asymbol *r;
		    int rtn_adjust;

		    if (new_cnt == new_max)
		      {
			new_max += STUB_SYM_BUFFER_INC;
			new_syms = (asymbol *) realloc (new_syms, new_max
							* sizeof (asymbol));
		      }

		    /* Determine whether a return adjustment
		       (see the relocation code for relocation type 
		       R_HPPA_STUB_CALL_17) is possible.  Basically,
		       determine whether we are looking at a branch or not.  */
							      
		    if (rle->howto->type == R_HPPA_PLABEL_32)
		      rtn_adjust = false;
		    else
		      {
			switch (get_opcode(insn[0]))
			  {
			  case BLE:
			  case BE:
			    rtn_adjust = true;
			    break;
			  default:
			    rtn_adjust = false;
			  }
		      }
		    r = hppa_elf_build_arg_reloc_stub (stub_bfd, output_bfd,
						       rle, stub_types,
						       rtn_adjust, insn);
		    new_syms[new_cnt++] = *r;
		  }
	      }
	      break;

	    default:
	      break;

	    }
	}
    }
  *new_sym_cnt = new_cnt;
  return new_syms;
}


char *linker_stubs = NULL;
int linker_stubs_size = 0;
int linker_stubs_max_size = 0;
#define STUB_ALLOC_INCR	100

boolean
DEFUN (hppa_elf_set_section_contents, (abfd, section, location, offset, count),
       bfd * abfd AND
       sec_ptr section AND
       PTR location AND
       file_ptr offset AND
       bfd_size_type count)
{
  if ( strcmp(section->name, ".hppa_linker_stubs") == 0 )
    {
      if ( linker_stubs_max_size < offset + count )
	{
	  linker_stubs_max_size = offset + count + STUB_ALLOC_INCR;
	  linker_stubs = (char *)realloc(linker_stubs, linker_stubs_max_size);
	}

      if ( offset + count > linker_stubs_size )
	linker_stubs_size = offset + count;

      memcpy(linker_stubs + offset,location,count);
      return (true);
    }
  else
    return bfd_elf32_set_section_contents (abfd, section, location,
					   offset, count);
}

/* Get the contents of the given section.
   
   This is special for PA ELF because some sections (such as linker stubs)
   may reside in memory rather than on disk, or in the case of the symbol
   extension section, the contents may need to be generated from other
   information contained in the BFD.  */

boolean
hppa_elf_get_section_contents (abfd, section, location, offset, count)
     bfd *abfd;
     sec_ptr section;
     PTR location;
     file_ptr offset;
     bfd_size_type count;
{
  /* If this is the linker stub section, then its contents are contained
     in memory rather than on disk.  FIXME.  Is that always right?  What
     about the case where a final executable is read in and a user tries
     to get the contents of this section?  In that case the contents would
     be on disk like everything else.  */
  if (strcmp (section->name, ".hppa_linker_stubs") == 0)
    {
      elf32_hppa_stub_description *stub_desc = find_stubs (abfd, section);
      
      if (count == 0)
	return true;
      
      /* Sanity check our arguments.  */
      if ((bfd_size_type) (offset + count) > section->_raw_size
	  || (bfd_size_type) (offset + count) > stub_desc->real_size)
	return (false);
      
      memcpy (location, stub_desc->stub_contents + offset, count);
      return (true);
    }

  /* The symbol extension section also needs special handling.  Its
     contents might be on the disk, in memory, or still need to
     be generated. */
  else if (strcmp (section->name, ".hppa_symextn") == 0)
    {
      /* If there are no output sections, then read the contents of the
	 symbol extension section from disk.  */
      if (section->output_section == NULL
	  && abfd->direction == read_direction)
	{
	  return bfd_generic_get_section_contents (abfd, section, location,
						   offset, count);
	}
      
      /* If this is the first time through, and there are output sections,
	 then build the symbol extension section based on other information
	 contained in the BFD.  */
      else if (! symext_chain_built)
	{
	  int i;
	  int *symtab_map =
	    (int *) elf_sym_extra(section->output_section->owner);
	  
	  for (i = 0; i < section->output_section->owner->symcount; i++ )
	    {
	      elf_hppa_tc_symbol(section->output_section->owner,
				 ((elf_symbol_type *)
				  section->output_section->owner->outsymbols[i]),
				 symtab_map[i]);
	    }
	  symext_chain_built++;
	  elf_hppa_tc_make_sections (section->output_section->owner, NULL);
	}

      /* At this point we know that the symbol extension section has been
	 built.  We just need to copy it into the user's buffer.  */
      if (count == 0)
	return true;
      
      /* Sanity check our arguments.  */
      if ((bfd_size_type) (offset + count) > section->_raw_size
	  || (bfd_size_type) (offset + count) > symextn_contents_real_size)
	return (false);
      
      memcpy (location,
	      ((char *)symextn_contents + section->output_offset + offset),
	      count);
      return (true);
    }
  else
    return bfd_generic_get_section_contents (abfd, section, location,
					     offset, count);
}

static void
DEFUN (elf_info_to_howto, (abfd, cache_ptr, dst),
       bfd * abfd AND
       arelent * cache_ptr AND
       Elf32_Internal_Rela * dst)
{
  BFD_ASSERT (ELF32_R_TYPE(dst->r_info) < (unsigned int) R_HPPA_UNIMPLEMENTED);
  cache_ptr->howto = &elf_hppa_howto_table[ELF32_R_TYPE(dst->r_info)];
}

static void
DEFUN (elf32_hppa_backend_symbol_processing, (abfd, sym),
       bfd * abfd AND
       asymbol * sym)
{
  /* Is this a definition of $global$?  If so, keep it because it will be
    needed if any relocations are performed.  */

  if (!strcmp (sym->name, "$global$")
      && sym->section != &bfd_und_section)
    {
      global_symbol = sym;
    }
}

#define elf_backend_symbol_processing	elf32_hppa_backend_symbol_processing

struct elf32_hppa_symextn_map_struct
{
  int old_index;
  bfd *bfd;
  asymbol *sym;
  int new_index;
};

static struct elf32_hppa_symextn_map_struct *elf32_hppa_symextn_map;
static int elf32_hppa_symextn_map_size;

static boolean
DEFUN (elf32_hppa_backend_symbol_table_processing, (abfd, esyms,symcnt),
       bfd 		* abfd AND
       elf_symbol_type	*esyms AND
       int		symcnt)
{
  Elf32_Internal_Shdr *symextn_hdr = bfd_elf_find_section (abfd, SYMEXTN_SECTION_NAME);
  int i;
  int current_sym_idx = 0;

  /* If the symbol extension section does not exist, all the symbol */
  /* all the symbol extension information is assumed to be zero.	*/

  if ( symextn_hdr == NULL )
    {
      for ( i = 0; i < symcnt; i++ )
	{
	  esyms[i].tc_data.hppa_arg_reloc = 0;
	}
      return (true);
    }

  /* allocate a buffer of the appropriate size for the symextn section */

  symextn_hdr->contents = bfd_zalloc(abfd,symextn_hdr->sh_size);
  symextn_hdr->size = symextn_hdr->sh_size;
	
  /* read in the symextn section */

  if (bfd_seek (abfd, symextn_hdr->sh_offset, SEEK_SET) == -1)
    {
      bfd_error = system_call_error;
      return (false);
    }
  if (bfd_read ((PTR) symextn_hdr->contents, 1, symextn_hdr->size, abfd) 
      != symextn_hdr->size)
    {
      free ((PTR)symextn_hdr->contents);
      bfd_error = system_call_error;
      return (false);
    }	

  /* parse the entries, updating the symtab entries as we go */

  for ( i = 0; i < symextn_hdr->size / sizeof(symext_entryS); i++ )
    {
      symext_entryS *seP = ((symext_entryS *)symextn_hdr->contents) + i;
      int se_value = ELF32_HPPA_SX_VAL(*seP);
      int se_type = ELF32_HPPA_SX_TYPE(*seP);

      switch ( se_type )
	{
	case HPPA_SXT_NULL:
	  break;

	case HPPA_SXT_SYMNDX:
	  if ( se_value >= symcnt )
	    {
	      bfd_error = bad_value;
	      bfd_perror("elf32_hppa_backend_symbol_table_processing -- symbol index");
	      return (false);
	    }
	  current_sym_idx = se_value - 1;
	  break;

	case HPPA_SXT_ARG_RELOC:
	  esyms[current_sym_idx].tc_data.hppa_arg_reloc = se_value;
	  break;

	default:
	  bfd_error = bad_value;
	  bfd_perror("elf32_hppa_backend_symbol_table_processing");
	  return (false);
	}
    }
  return (true);
}

#define elf_backend_symbol_table_processing	elf32_hppa_backend_symbol_table_processing

static boolean
DEFUN (elf32_hppa_backend_section_processing, (abfd, secthdr),
       bfd 		* abfd AND
       Elf32_Internal_Shdr *secthdr)
{
  int i,j,k;

  if ( secthdr->sh_type == SHT_HPPA_SYMEXTN )
    {
      for ( i = 0; i < secthdr->size / sizeof(symext_entryS); i++ )
	{
	  symext_entryS *seP = ((symext_entryS *)secthdr->contents) + i;
	  int se_value = ELF32_HPPA_SX_VAL(*seP);
	  int se_type = ELF32_HPPA_SX_TYPE(*seP);
	  
	  switch ( se_type )
	    {
	    case HPPA_SXT_NULL:
	      break;
	      
	    case HPPA_SXT_SYMNDX:
	      for ( j = 0; j < abfd->symcount; j++ )
		{
		  /* locate the map entry for this symbol, if there is one. */
		  /* modify the symbol extension section symbol index entry */
		  /* to reflect the new symbol table index */
		  
		  for ( k = 0; k < elf32_hppa_symextn_map_size; k++ )
		    {
		      if ( elf32_hppa_symextn_map[k].old_index == se_value
			  && elf32_hppa_symextn_map[k].bfd == abfd->outsymbols[j]->the_bfd
			  && elf32_hppa_symextn_map[k].sym == abfd->outsymbols[j] )
			{
			  bfd_put_32(abfd,
				     ELF32_HPPA_SX_WORD (HPPA_SXT_SYMNDX, j),
				     (char *)seP);
			}
		    }
		}
	      break;
	      
	    case HPPA_SXT_ARG_RELOC:
	      break;
	      
	    default:
	      bfd_error = bad_value;
	      bfd_perror("elf32_hppa_backend_section_processing");
	      return (false);
	    }
	}
    }
  return true;
}

#define elf_backend_section_processing	elf32_hppa_backend_section_processing

static boolean
DEFUN (elf32_hppa_backend_section_from_shdr, (abfd, hdr, name),
       bfd 		* abfd AND
       Elf32_Internal_Shdr *hdr AND
       char		* name)
{
  asection *newsect;

  if ( hdr->sh_type == SHT_HPPA_SYMEXTN )
    {
      BFD_ASSERT ( strcmp(name,".hppa_symextn") == 0 );

      /* Bits that get saved. This one is real. */
      if (!hdr->rawdata)
	{
	  newsect = bfd_make_section (abfd, name);
	  if (newsect != NULL)
	    {
	      newsect->vma = hdr->sh_addr;
	      newsect->_raw_size = hdr->sh_size;
	      newsect->filepos = hdr->sh_offset;	/* so we can read back the bits */
	      newsect->flags |= SEC_HAS_CONTENTS;
	      newsect->alignment_power = hdr->sh_addralign;

	      if (hdr->sh_flags & SHF_ALLOC)
		{
		  newsect->flags |= SEC_ALLOC;
		  newsect->flags |= SEC_LOAD;
		}

	      if (!(hdr->sh_flags & SHF_WRITE))
		newsect->flags |= SEC_READONLY;

	      if (hdr->sh_flags & SHF_EXECINSTR)
		newsect->flags |= SEC_CODE;	/* FIXME: may only contain SOME code */
	      else
		newsect->flags |= SEC_DATA;

	      hdr->rawdata = (void *) newsect;
	    }
	}
      return true;
    }
  return false;
}

#define elf_backend_section_from_shdr	elf32_hppa_backend_section_from_shdr

static boolean
DEFUN (elf32_hppa_backend_fake_sections, (abfd, secthdr, asect),
       bfd 		* abfd AND
       Elf_Internal_Shdr *secthdr AND
       asection		*asect)
{

  if ( strcmp(asect->name, ".hppa_symextn") == 0 )
    {
      secthdr->sh_type = SHT_HPPA_SYMEXTN;
      secthdr->sh_flags = 0;
      secthdr->sh_info = elf_section_data(asect)->rel_hdr.sh_link;
      secthdr->sh_link = elf_onesymtab(abfd);
      return true;
    }

  if (!strcmp (asect->name, ".hppa_unwind"))
    {
      secthdr->sh_type = SHT_PROGBITS;
      /* Unwind descriptors are not part of the program memory image.  */
      secthdr->sh_flags = 0;
      secthdr->sh_info = 0;
      secthdr->sh_link = 0;
      secthdr->sh_entsize = 16;
      return true;
    }

  /* @@ Should this be CPU specific??  KR */
  if (!strcmp (asect->name, ".stabstr"))
    {
      secthdr->sh_type = SHT_STRTAB;
      secthdr->sh_flags = 0;
      secthdr->sh_info = 0;
      secthdr->sh_link = 0;
      secthdr->sh_entsize = 0;
      return true;
    }

  return false;
}

#define elf_backend_fake_sections	elf32_hppa_backend_fake_sections

static boolean
DEFUN (elf32_hppa_backend_section_from_bfd_section, (abfd, hdr, asect, retval),
       bfd 			*abfd AND
       Elf32_Internal_Shdr	*hdr AND
       asection			*asect AND
       int			*retval)
{

  if ( hdr->sh_type == SHT_HPPA_SYMEXTN )
    {
      if (hdr->rawdata)
	{
	  if (((struct sec *) (hdr->rawdata)) == asect)
	    {
	      BFD_ASSERT( strcmp(asect->name, ".hppa_symextn") == 0 );
	      return true;
	    }
	}
    }
  else if ( hdr->sh_type == SHT_STRTAB )
    {
      if (hdr->rawdata)
	{
	  if (((struct sec *) (hdr->rawdata)) == asect)
	    {
	      BFD_ASSERT ( strcmp (asect->name, ".stabstr") == 0);
	      return true;
	    }
	}
    }

  return false;
}

#define elf_backend_section_from_bfd_section	elf32_hppa_backend_section_from_bfd_section

#define bfd_generic_get_section_contents	hppa_elf_get_section_contents
#define bfd_elf32_set_section_contents		hppa_elf_set_section_contents

#define TARGET_BIG_SYM		bfd_elf32_hppa_vec
#define TARGET_BIG_NAME		"elf32-hppa"
#define ELF_ARCH		bfd_arch_hppa
#define ELF_MACHINE_CODE	EM_HPPA
#define ELF_MAXPAGESIZE		0x1000

#include "elf32-target.h"
