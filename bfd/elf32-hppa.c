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

/*
	Written by:

	Center for Software Science
	Department of Computer Science
	University of Utah
*/

#include "elf32-hppa.h"
/*#include "libhppa.h"*/
#define BYTES_IN_WORD 4
#include "aout/aout64.h"

/* ELF/PA relocation howto entries */

static bfd_reloc_status_type hppa_elf_reloc ();

reloc_howto_type elf_hppa_howto_table[ELF_HOWTO_TABLE_SIZE] =
{
/*  'bitpos' and 'abs' are obsolete */
/* type			rs sz bsz pcrel  bpos abs    ovrf  sf		   name */
/* 9.3.4. Address relocation types */
  {R_HPPA_NONE, 0, 3, 19, false, 0, false, true, hppa_elf_reloc, "R_HPPA_NONE"},
  {R_HPPA_32, 0, 3, 32, false, 0, false, true, hppa_elf_reloc, "R_HPPA_32"},
  {R_HPPA_11, 0, 3, 11, false, 0, false, true, hppa_elf_reloc, "R_HPPA_11"},
  {R_HPPA_14, 0, 3, 14, false, 0, false, true, hppa_elf_reloc, "R_HPPA_14"},
  {R_HPPA_17, 0, 3, 17, false, 0, false, true, hppa_elf_reloc, "R_HPPA_17"},
{R_HPPA_L21, 0, 3, 21, false, 0, false, true, hppa_elf_reloc, "R_HPPA_L21"},
{R_HPPA_R11, 0, 3, 11, false, 0, false, true, hppa_elf_reloc, "R_HPPA_R11"},
{R_HPPA_R14, 0, 3, 14, false, 0, false, true, hppa_elf_reloc, "R_HPPA_R14"},
{R_HPPA_R17, 0, 3, 17, false, 0, false, true, hppa_elf_reloc, "R_HPPA_R17"},
  {R_HPPA_LS21, 0, 3, 21, false, 0, false, true, hppa_elf_reloc, "R_HPPA_LS21"},
  {R_HPPA_RS11, 0, 3, 11, false, 0, false, true, hppa_elf_reloc, "R_HPPA_RS11"},
  {R_HPPA_RS14, 0, 3, 14, false, 0, false, true, hppa_elf_reloc, "R_HPPA_RS14"},
  {R_HPPA_RS17, 0, 3, 17, false, 0, false, true, hppa_elf_reloc, "R_HPPA_RS17"},
  {R_HPPA_LD21, 0, 3, 21, false, 0, false, true, hppa_elf_reloc, "R_HPPA_LD21"},
  {R_HPPA_RD11, 0, 3, 11, false, 0, false, true, hppa_elf_reloc, "R_HPPA_RD11"},
  {R_HPPA_RD14, 0, 3, 14, false, 0, false, true, hppa_elf_reloc, "R_HPPA_RD14"},
  {R_HPPA_RD17, 0, 3, 17, false, 0, false, true, hppa_elf_reloc, "R_HPPA_RD17"},
  {R_HPPA_LR21, 0, 3, 21, false, 0, false, true, hppa_elf_reloc, "R_HPPA_LR21"},
  {R_HPPA_RR14, 0, 3, 14, false, 0, false, true, hppa_elf_reloc, "R_HPPA_RR14"},
  {R_HPPA_RR17, 0, 3, 17, false, 0, false, true, hppa_elf_reloc, "R_HPPA_RR17"},
/* 9.3.5. GOTOFF address relocation types		*/
  {R_HPPA_GOTOFF_11, 0, 3, 11, false, 0, false, true, hppa_elf_reloc, "R_HPPA_GOTOFF_11"},
  {R_HPPA_GOTOFF_14, 0, 3, 14, false, 0, false, true, hppa_elf_reloc, "R_HPPA_GOTOFF_14"},
  {R_HPPA_GOTOFF_L21, 0, 3, 21, false, 0, false, true, hppa_elf_reloc, "R_HPPA_GOTOFF_L21"},
  {R_HPPA_GOTOFF_R11, 0, 3, 11, false, 0, false, true, hppa_elf_reloc, "R_HPPA_GOTOFF_R11"},
  {R_HPPA_GOTOFF_R14, 0, 3, 14, false, 0, false, true, hppa_elf_reloc, "R_HPPA_GOTOFF_R14"},
  {R_HPPA_GOTOFF_LS21, 0, 3, 21, false, 0, false, true, hppa_elf_reloc, "R_HPPA_GOTOFF_LS21"},
  {R_HPPA_GOTOFF_RS11, 0, 3, 11, false, 0, false, true, hppa_elf_reloc, "R_HPPA_GOTOFF_RS11"},
  {R_HPPA_GOTOFF_RS14, 0, 3, 14, false, 0, false, true, hppa_elf_reloc, "R_HPPA_GOTOFF_RS14"},
  {R_HPPA_GOTOFF_LD21, 0, 3, 21, false, 0, false, true, hppa_elf_reloc, "R_HPPA_GOTOFF_LD21"},
  {R_HPPA_GOTOFF_RD11, 0, 3, 11, false, 0, false, true, hppa_elf_reloc, "R_HPPA_GOTOFF_RD11"},
  {R_HPPA_GOTOFF_RD14, 0, 3, 14, false, 0, false, true, hppa_elf_reloc, "R_HPPA_GOTOFF_RD14"},
  {R_HPPA_GOTOFF_LR21, 0, 3, 21, false, 0, false, true, hppa_elf_reloc, "R_HPPA_GOTOFF_LR21"},
  {R_HPPA_GOTOFF_RR14, 0, 3, 14, false, 0, false, true, hppa_elf_reloc, "R_HPPA_GOTOFF_RR14"},
/* 9.3.6. Absolute call relocation types	*/
  {R_HPPA_ABS_CALL_11, 0, 3, 11, false, 0, false, true, hppa_elf_reloc, "R_HPPA_ABS_CALL_11"},
  {R_HPPA_ABS_CALL_14, 0, 3, 14, false, 0, false, true, hppa_elf_reloc, "R_HPPA_ABS_CALL_14"},
  {R_HPPA_ABS_CALL_17, 0, 3, 17, false, 0, false, true, hppa_elf_reloc, "R_HPPA_ABS_CALL_17"},
  {R_HPPA_ABS_CALL_L21, 0, 3, 21, false, 0, false, true, hppa_elf_reloc, "R_HPPA_ABS_CALL_L21"},
  {R_HPPA_ABS_CALL_R11, 0, 3, 11, false, 0, false, true, hppa_elf_reloc, "R_HPPA_ABS_CALL_R11"},
  {R_HPPA_ABS_CALL_R14, 0, 3, 14, false, 0, false, true, hppa_elf_reloc, "R_HPPA_ABS_CALL_R14"},
  {R_HPPA_ABS_CALL_R17, 0, 3, 17, false, 0, false, true, hppa_elf_reloc, "R_HPPA_ABS_CALL_R17"},
  {R_HPPA_ABS_CALL_LS21, 0, 3, 21, false, 0, false, true, hppa_elf_reloc, "R_HPPA_ABS_CALL_LS21"},
  {R_HPPA_ABS_CALL_RS11, 0, 3, 11, false, 0, false, true, hppa_elf_reloc, "R_HPPA_ABS_CALL_RS11"},
  {R_HPPA_ABS_CALL_RS14, 0, 3, 14, false, 0, false, true, hppa_elf_reloc, "R_HPPA_ABS_CALL_RS14"},
  {R_HPPA_ABS_CALL_RS17, 0, 3, 17, false, 0, false, true, hppa_elf_reloc, "R_HPPA_ABS_CALL_RS17"},
  {R_HPPA_ABS_CALL_LD21, 0, 3, 21, false, 0, false, true, hppa_elf_reloc, "R_HPPA_ABS_CALL_LD21"},
  {R_HPPA_ABS_CALL_RD11, 0, 3, 11, false, 0, false, true, hppa_elf_reloc, "R_HPPA_ABS_CALL_RD11"},
  {R_HPPA_ABS_CALL_RD14, 0, 3, 14, false, 0, false, true, hppa_elf_reloc, "R_HPPA_ABS_CALL_RD14"},
  {R_HPPA_ABS_CALL_RD17, 0, 3, 17, false, 0, false, true, hppa_elf_reloc, "R_HPPA_ABS_CALL_RD17"},
  {R_HPPA_ABS_CALL_LR21, 0, 3, 21, false, 0, false, true, hppa_elf_reloc, "R_HPPA_ABS_CALL_LR21"},
  {R_HPPA_ABS_CALL_RR14, 0, 3, 14, false, 0, false, true, hppa_elf_reloc, "R_HPPA_ABS_CALL_RR14"},
  {R_HPPA_ABS_CALL_RR17, 0, 3, 17, false, 0, false, true, hppa_elf_reloc, "R_HPPA_ABS_CALL_RR17"},
/* 9.3.7. PC-relative call relocation types	*/
  {R_HPPA_PCREL_CALL_11, 0, 3, 11, true, 0, false, true, hppa_elf_reloc, "R_HPPA_PCREL_CALL_11"},
  {R_HPPA_PCREL_CALL_14, 0, 3, 14, true, 0, false, true, hppa_elf_reloc, "R_HPPA_PCREL_CALL_14"},
  {R_HPPA_PCREL_CALL_17, 0, 3, 17, true, 0, false, true, hppa_elf_reloc, "R_HPPA_PCREL_CALL_17"},
  {R_HPPA_PCREL_CALL_12, 0, 3, 12, true, 0, false, true, hppa_elf_reloc, "R_HPPA_PCREL_CALL_12"},
  {R_HPPA_PCREL_CALL_L21, 0, 3, 21, true, 0, false, true, hppa_elf_reloc, "R_HPPA_PCREL_CALL_L21"},
  {R_HPPA_PCREL_CALL_R11, 0, 3, 11, true, 0, false, true, hppa_elf_reloc, "R_HPPA_PCREL_CALL_R11"},
  {R_HPPA_PCREL_CALL_R14, 0, 3, 14, true, 0, false, true, hppa_elf_reloc, "R_HPPA_PCREL_CALL_R14"},
  {R_HPPA_PCREL_CALL_R17, 0, 3, 17, true, 0, false, true, hppa_elf_reloc, "R_HPPA_PCREL_CALL_R17"},
  {R_HPPA_PCREL_CALL_LS21, 0, 3, 21, true, 0, false, true, hppa_elf_reloc, "R_HPPA_PCREL_CALL_LS21"},
  {R_HPPA_PCREL_CALL_RS11, 0, 3, 11, true, 0, false, true, hppa_elf_reloc, "R_HPPA_PCREL_CALL_RS11"},
  {R_HPPA_PCREL_CALL_RS14, 0, 3, 14, true, 0, false, true, hppa_elf_reloc, "R_HPPA_PCREL_CALL_RS14"},
  {R_HPPA_PCREL_CALL_RS17, 0, 3, 17, true, 0, false, true, hppa_elf_reloc, "R_HPPA_PCREL_CALL_RS17"},
  {R_HPPA_PCREL_CALL_LD21, 0, 3, 21, true, 0, false, true, hppa_elf_reloc, "R_HPPA_PCREL_CALL_LD21"},
  {R_HPPA_PCREL_CALL_RD11, 0, 3, 11, true, 0, false, true, hppa_elf_reloc, "R_HPPA_PCREL_CALL_RD11"},
  {R_HPPA_PCREL_CALL_RD14, 0, 3, 14, true, 0, false, true, hppa_elf_reloc, "R_HPPA_PCREL_CALL_RD14"},
  {R_HPPA_PCREL_CALL_RD17, 0, 3, 17, true, 0, false, true, hppa_elf_reloc, "R_HPPA_PCREL_CALL_RD17"},
  {R_HPPA_PCREL_CALL_LR21, 0, 3, 21, true, 0, false, true, hppa_elf_reloc, "R_HPPA_PCREL_CALL_LR21"},
  {R_HPPA_PCREL_CALL_RR14, 0, 3, 14, true, 0, false, true, hppa_elf_reloc, "R_HPPA_PCREL_CALL_RR14"},
  {R_HPPA_PCREL_CALL_RR17, 0, 3, 17, true, 0, false, true, hppa_elf_reloc, "R_HPPA_PCREL_CALL_RR17"},

/* 9.3.8. Plabel relocation types */
  {R_HPPA_PLABEL_32, 0, 3, 32, true, 0, false, true, hppa_elf_reloc, "R_HPPA_PLABEL_32"},
  {R_HPPA_PLABEL_11, 0, 3, 11, true, 0, false, true, hppa_elf_reloc, "R_HPPA_PLABEL_11"},
  {R_HPPA_PLABEL_14, 0, 3, 14, true, 0, false, true, hppa_elf_reloc, "R_HPPA_PLABEL_14"},
  {R_HPPA_PLABEL_L21, 0, 3, 21, true, 0, false, true, hppa_elf_reloc, "R_HPPA_PLABEL_L21"},
  {R_HPPA_PLABEL_R11, 0, 3, 11, true, 0, false, true, hppa_elf_reloc, "R_HPPA_PLABEL_R11"},
  {R_HPPA_PLABEL_R14, 0, 3, 14, true, 0, false, true, hppa_elf_reloc, "R_HPPA_PLABEL_R14"},

/* 9.3.9. Data linkage table (DLT) relocation types	*/
  {R_HPPA_DLT_32, 0, 3, 32, true, 0, false, true, hppa_elf_reloc, "R_HPPA_DLT_32"},
  {R_HPPA_DLT_11, 0, 3, 11, true, 0, false, true, hppa_elf_reloc, "R_HPPA_DLT_11"},
  {R_HPPA_DLT_14, 0, 3, 14, true, 0, false, true, hppa_elf_reloc, "R_HPPA_DLT_14"},
  {R_HPPA_DLT_L21, 0, 3, 21, true, 0, false, true, hppa_elf_reloc, "R_HPPA_DLT_L21"},
  {R_HPPA_DLT_R11, 0, 3, 11, true, 0, false, true, hppa_elf_reloc, "R_HPPA_DLT_R11"},
  {R_HPPA_DLT_R14, 0, 3, 14, true, 0, false, true, hppa_elf_reloc, "R_HPPA_DLT_R14"},

/* 9.3.10. Relocations for unwinder tables	*/
  {R_HPPA_UNWIND_ENTRY, 0, 3, 32, true, 0, false, true, hppa_elf_reloc, "R_HPPA_UNWIND_ENTRY"},
  {R_HPPA_UNWIND_ENTRIES, 0, 3, 32, true, 0, false, true, hppa_elf_reloc, "R_HPPA_UNWIND_ENTRIES"},

/*  9.3.11. Relocation types for complex expressions	*/
  {R_HPPA_PUSH_CONST, 0, 3, 32, false, 0, false, true, hppa_elf_reloc, "R_HPPA_PUSH_CONST"},
  {R_HPPA_PUSH_PC, 0, 3, 32, false, 0, false, true, hppa_elf_reloc, "R_HPPA_PUSH_PC"},
  {R_HPPA_PUSH_SYM, 0, 3, 32, false, 0, false, true, hppa_elf_reloc, "R_HPPA_PUSH_SYM"},
  {R_HPPA_PUSH_GOTOFF, 0, 3, 32, false, 0, false, true, hppa_elf_reloc, "R_HPPA_PUSH_GOTOFF"},
  {R_HPPA_PUSH_ABS_CALL, 0, 3, 32, false, 0, false, true, hppa_elf_reloc, "R_HPPA_PUSH_ABS_CALL"},
  {R_HPPA_PUSH_PCREL_CALL, 0, 3, 32, false, 0, false, true, hppa_elf_reloc, "R_HPPA_PUSH_PCREL_CALL"},
  {R_HPPA_PUSH_PLABEL, 0, 3, 32, false, 0, false, true, hppa_elf_reloc, "R_HPPA_PUSH_PLABEL"},
{R_HPPA_MAX, 0, 3, 32, false, 0, false, true, hppa_elf_reloc, "R_HPPA_MAX"},
{R_HPPA_MIN, 0, 3, 32, false, 0, false, true, hppa_elf_reloc, "R_HPPA_MIN"},
{R_HPPA_ADD, 0, 3, 32, false, 0, false, true, hppa_elf_reloc, "R_HPPA_ADD"},
{R_HPPA_SUB, 0, 3, 32, false, 0, false, true, hppa_elf_reloc, "R_HPPA_SUB"},
  {R_HPPA_MULT, 0, 3, 32, false, 0, false, true, hppa_elf_reloc, "R_HPPA_MULT"},
{R_HPPA_DIV, 0, 3, 32, false, 0, false, true, hppa_elf_reloc, "R_HPPA_DIV"},
{R_HPPA_MOD, 0, 3, 32, false, 0, false, true, hppa_elf_reloc, "R_HPPA_MOD"},
{R_HPPA_AND, 0, 3, 32, false, 0, false, true, hppa_elf_reloc, "R_HPPA_AND"},
  {R_HPPA_OR, 0, 3, 32, false, 0, false, true, hppa_elf_reloc, "R_HPPA_OR"},
{R_HPPA_XOR, 0, 3, 32, false, 0, false, true, hppa_elf_reloc, "R_HPPA_XOR"},
{R_HPPA_NOT, 0, 3, 32, false, 0, false, true, hppa_elf_reloc, "R_HPPA_NOT"},
  {R_HPPA_LSHIFT, 0, 3, 32, false, 0, false, true, hppa_elf_reloc, "R_HPPA_LSHIFT"},
  {R_HPPA_ARITH_RSHIFT, 0, 3, 32, false, 0, false, true, hppa_elf_reloc, "R_HPPA_ARITH_RSHIFT"},
  {R_HPPA_LOGIC_RSHIFT, 0, 3, 32, false, 0, false, true, hppa_elf_reloc, "R_HPPA_LOGIC_RSHIFT"},
{R_HPPA_EXPR_F, 0, 3, 32, false, 0, false, true, hppa_elf_reloc, "R_HPPA_L"},
  {R_HPPA_EXPR_L, 0, 3, 32, false, 0, false, true, hppa_elf_reloc, "R_HPPA_EXPR_L"},
  {R_HPPA_EXPR_R, 0, 3, 32, false, 0, false, true, hppa_elf_reloc, "R_HPPA_EXPR_R"},
  {R_HPPA_EXPR_LS, 0, 3, 32, false, 0, false, true, hppa_elf_reloc, "R_HPPA_EXPR_LS"},
  {R_HPPA_EXPR_RS, 0, 3, 32, false, 0, false, true, hppa_elf_reloc, "R_HPPA_EXPR_RS"},
  {R_HPPA_EXPR_LD, 0, 3, 32, false, 0, false, true, hppa_elf_reloc, "R_HPPA_EXPR_LD"},
  {R_HPPA_EXPR_RD, 0, 3, 32, false, 0, false, true, hppa_elf_reloc, "R_HPPA_EXPR_RD"},
  {R_HPPA_EXPR_LR, 0, 3, 32, false, 0, false, true, hppa_elf_reloc, "R_HPPA_EXPR_LR"},
  {R_HPPA_EXPR_RR, 0, 3, 32, false, 0, false, true, hppa_elf_reloc, "R_HPPA_EXPR_RR"},

  {R_HPPA_EXPR_32, 0, 3, 32, false, 0, false, true, hppa_elf_reloc, "R_HPPA_EXPR_32"},
  {R_HPPA_EXPR_21, 0, 3, 21, false, 0, false, true, hppa_elf_reloc, "R_HPPA_EXPR_21"},
  {R_HPPA_EXPR_11, 0, 3, 11, false, 0, false, true, hppa_elf_reloc, "R_HPPA_EXPR_11"},
  {R_HPPA_EXPR_14, 0, 3, 14, false, 0, false, true, hppa_elf_reloc, "R_HPPA_EXPR_14"},
  {R_HPPA_EXPR_17, 0, 3, 17, false, 0, false, true, hppa_elf_reloc, "R_HPPA_EXPR_17"},
  {R_HPPA_EXPR_12, 0, 3, 12, false, 0, false, true, hppa_elf_reloc, "R_HPPA_EXPR_12"},
  {R_HPPA_UNIMPLEMENTED, 0, 0, 0, false, 0, false, false, NULL, "R_HPPA_UNIMPLEMENTED"},
};

static symext_chainS *symext_rootP = NULL;
static symext_chainS *symext_lastP = NULL;

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
      constant_value = ELF32_HPPA_R_CONSTANT (r_addend);
      BFD_ASSERT (r_format == 14);

      if (pcrel)
	sym_value -= address;
      sym_value = hppa_field_adjust (sym_value, constant_value, r_field);
      return hppa_elf_rebuild_insn (abfd, insn, sym_value, r_type, r_format, r_format);

    case COMICLR:
    case SUBI:			/* case SUBIO: */
    case ADDIT:		/* case ADDITO: */
    case ADDI:			/* case ADDIO: */
      BFD_ASSERT (r_format == 11);

      constant_value = ((insn & 0x1) << 10) | ((insn & 0xffe) >> 1);
      sym_value = hppa_field_adjust (sym_value, constant_value, r_field);
      return hppa_elf_rebuild_insn (abfd, insn, sym_value, r_type, r_field, r_format);

    case LDIL:
    case ADDIL:
      BFD_ASSERT (r_format == 21);

      constant_value = assemble_21 (insn);
      constant_value += ELF32_HPPA_R_CONSTANT (r_addend);
      sym_value = hppa_field_adjust (sym_value, constant_value, r_field);
      return hppa_elf_rebuild_insn (abfd, insn, sym_value, r_type, r_field, r_format);

    case BL:
    case BE:
    case BLE:
      arg_reloc = ELF32_HPPA_R_ARG_RELOC (r_addend);

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
	  constant_value = insn;
	  constant_value += ELF32_HPPA_R_CONSTANT (r_addend);

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
	  unsigned int fsize;
	  start_offset = bfd_get_32 (abfd, hit_data);
	  /* Stuff the symbol value into the first word */
	  /* of the  unwind descriptor */
	  bfd_put_32 (abfd, sym_value, hit_data);

	  hit_data += sizeof (unsigned long);
	  end_offset = bfd_get_32 (abfd, hit_data);
	  /* We could also compute the ending offset for */
	  /* the 2nd word of the unwind entry by */
	  /* retrieving the st_size field of the Elf_Sym */
	  /* structure stored with this symbol.  We can */
	  /* get it with: 	*/
	  /* 	e = (elf_symbol_type *)symp	*/
	  /*	fsize = e->internal_elf_sym.st_size */

	  fsize = end_offset - start_offset;
	  relocated_value = hppa_field_adjust (sym_value, fsize, r_field);
	  bfd_put_32 (abfd, relocated_value, hit_data);

	  /* If this is not the last unwind entry, */
	  /* adjust the symbol value. */
	  if (i + 1 < r_addend)
	    {
	      start_offset = bfd_get_32 (abfd, hit_data + 3 * sizeof (unsigned long));
	      sym_value += fsize + start_offset - end_offset;
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
#define UNDEFINED	hppa_elf_gen_reloc_error(base_type,format,field);

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
	    case e_lpsel:
	    case e_tsel:
	    case e_ltsel:
	    case e_rtsel:

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
	    case e_lpsel:
	    case e_tsel:
	    case e_ltsel:
	    case e_rtsel:

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

/* 12.4.4. Derive format from instruction	*/

/*	Given a machine instruction, this function determines its format.	*/
/*	The format can be determined solely from looking at the first six	*/
/*	bits (the major opcode) of the instruction.  Several major opcodes	*/
/*	map to the same format.  Opcodes which do not map to a known format	*/
/*	should probably be reported as an error.				*/

unsigned char
hppa_elf_insn2fmt (type, insn)
     elf32_hppa_reloc_type type;
     unsigned long insn;
{
  unsigned char fmt = 0;	/* XXX: is this a proper default?	*/
  unsigned char op = get_opcode (insn);

  if (type == R_HPPA_NONE)
    fmt = 0;
  else
    {
      switch (op)
	{
	case ADDI:
	case ADDIT:
	case SUBI:
	  fmt = 11;
	  break;
	case MOVB:
	case MOVIB:
	case COMBT:
	case COMBF:
	case COMIBT:
	case COMIBF:
	case ADDBT:
	case ADDBF:
	case ADDIBT:
	case ADDIBF:
	case BVB:
	case BB:
	  fmt = 12;
	  break;
	case LDO:
	case LDB:
	case LDH:
	case LDW:
	case LDWM:
	case STB:
	case STH:
	case STW:
	case STWM:
	  fmt = 14;
	  break;
	case BL:
	case BE:
	case BLE:
	  fmt = 17;
	  break;
	case LDIL:
	case ADDIL:
	  fmt = 21;
	  break;
	default:
	  fmt = 32;
	  break;
	}

    }
  return fmt;
}

/* this function is in charge of performing all the HP PA relocations */
static long global_value = 0;
static long GOT_value = 0;	/* XXX:  need to calculate this! For HPUX, GOT == DP */
static asymbol *global_symbol = (asymbol *) NULL;

static bfd_reloc_status_type
DEFUN (hppa_elf_reloc, (abfd, reloc_entry, symbol_in, data, input_section, output_bfd),
       bfd * abfd AND
       arelent * reloc_entry AND
       asymbol * symbol_in AND
       PTR data AND
       asection * input_section AND
       bfd * output_bfd)
{
  unsigned long insn;
  long sym_value = 0;

  unsigned long addr = reloc_entry->address;	/*+ input_section->vma*/
  bfd_byte *hit_data = addr + (bfd_byte *) (data);
  unsigned short r_type = reloc_entry->howto->type & 0xFF;
  unsigned short r_field = e_fsel;
  boolean r_pcrel = reloc_entry->howto->pc_relative;

  /* howto->bitsize contains the format (11, 14, 21, etc) information */
  unsigned r_format = reloc_entry->howto->bitsize;
  long r_addend = reloc_entry->addend;


  if (output_bfd)
    {
      /* Partial linking - do nothing */
      reloc_entry->address += input_section->output_offset;
      return bfd_reloc_ok;
    }

  if (symbol_in && symbol_in->section == &bfd_und_section)
    return bfd_reloc_undefined;

  /* Check for stubs that might be required.  */
  /* symbol_in = hppa_elf_stub_check (abfd, input_section->output_section->owner, reloc_entry); */

  sym_value = get_symbol_value (symbol_in);

  /* compute value of $global$ if it is there. */

  if (global_symbol == (asymbol *) NULL)
    {
      struct elf_backend_data *bed
      = (struct elf_backend_data *) abfd->xvec->backend_data;

      if (bed && bed->global_sym)
	{
	  asymbol *gsym = &bed->global_sym->symbol;
	  global_value
	    = gsym->value
	    + gsym->section->output_section->vma
	    + gsym->section->output_offset;
	  GOT_value = global_value;	/* XXX: For HP-UX, GOT==DP */
	  global_symbol = gsym;
	}
    }

  /* get the instruction word */
  insn = bfd_get_32 (abfd, hit_data);

  /* relocate the value based on the relocation type */

  /* basic_type_1:	relocation is relative to $global$		*/
  /* basic_type_2:	relocation is relative to the current GOT	*/
  /* basic_type_3:	relocation is an absolute call			*/
  /* basic_type_4:	relocation is an PC-relative call		*/
  /* basic_type_5:	relocation is plabel reference			*/
  /* basic_type_6:	relocation is an unwind table relocation	*/
  /* extended_type:	unimplemented					*/

  switch (r_type)
    {
    case R_HPPA_NONE:
      break;
    case R_HPPA_32:		/*		Symbol + Addend		32	*/
      r_field = e_fsel;
      goto do_basic_type_1;
    case R_HPPA_11:		/*		Symbol + Addend    	11	*/
      r_field = e_fsel;
      goto do_basic_type_1;
    case R_HPPA_14:		/*		Symbol + Addend		14	*/
      r_field = e_fsel;
      goto do_basic_type_1;
    case R_HPPA_17:		/*		Symbol + Addend		17	*/
      r_field = e_fsel;
      goto do_basic_type_1;
    case R_HPPA_L21:		/*		L (Symbol, Addend)	21 	*/
      r_field = e_lsel;
      goto do_basic_type_1;
    case R_HPPA_R11:		/*		R (Symbol, Addend)	11 	*/
      r_field = e_rsel;
      goto do_basic_type_1;
    case R_HPPA_R14:		/*		R (Symbol, Addend)	14	*/
      r_field = e_rsel;
      goto do_basic_type_1;
    case R_HPPA_R17:		/*		R (Symbol, Addend)	17 	*/
      r_field = e_rsel;
      goto do_basic_type_1;
    case R_HPPA_LS21:		/*		LS(Symbol, Addend)	21	*/
      r_field = e_lssel;
      goto do_basic_type_1;
    case R_HPPA_RS11:		/*		RS(Symbol, Addend)	11	*/
      r_field = e_rssel;
      goto do_basic_type_1;
    case R_HPPA_RS14:		/*		RS(Symbol, Addend)	14	*/
      r_field = e_rssel;
      goto do_basic_type_1;
    case R_HPPA_RS17:		/*		RS(Symbol, Addend)	17	*/
      r_field = e_ldsel;
      goto do_basic_type_1;
    case R_HPPA_LD21:		/*		LD(Symbol, Addend)	21	*/
      r_field = e_ldsel;
      goto do_basic_type_1;
    case R_HPPA_RD11:		/*		RD(Symbol, Addend)	11	*/
      r_field = e_rdsel;
      goto do_basic_type_1;
    case R_HPPA_RD14:		/*		RD(Symbol, Addend)	14	*/
      r_field = e_rdsel;
      goto do_basic_type_1;
    case R_HPPA_RD17:		/*		RD(Symbol, Addend)	17	*/
      r_field = e_rdsel;
      goto do_basic_type_1;
    case R_HPPA_LR21:		/*		LR(Symbol, Addend)	21	*/
      r_field = e_lrsel;
      goto do_basic_type_1;
    case R_HPPA_RR14:		/*		RR(Symbol, Addend)	14	*/
      r_field = e_rrsel;
      goto do_basic_type_1;
    case R_HPPA_RR17:		/*		RR(Symbol, Addend)	17	*/
      r_field = e_rrsel;

    do_basic_type_1:
      insn = hppa_elf_relocate_insn (abfd, input_section, insn, addr,
				     symbol_in, sym_value, r_addend,
				     r_type, r_format, r_field, r_pcrel);
      break;

    case R_HPPA_GOTOFF_11:	/*	Symbol - GOT + Addend	11	*/
      r_field = e_fsel;
      goto do_basic_type_2;
    case R_HPPA_GOTOFF_14:	/*	Symbol - GOT + Addend	14	*/
      r_field = e_fsel;
      goto do_basic_type_2;
    case R_HPPA_GOTOFF_L21:	/*	L (Sym - GOT, Addend)	21	*/
      r_field = e_lsel;
      goto do_basic_type_2;
    case R_HPPA_GOTOFF_R11:	/*	R (Sym - GOT, Addend)	11	*/
      r_field = e_rsel;
      goto do_basic_type_2;
    case R_HPPA_GOTOFF_R14:	/*	R (Sym - GOT, Addend)	14	*/
      r_field = e_rsel;
      goto do_basic_type_2;
    case R_HPPA_GOTOFF_LS21:	/*	LS(Sym - GOT, Addend)	21	*/
      r_field = e_lssel;
      goto do_basic_type_2;
    case R_HPPA_GOTOFF_RS11:	/*	RS(Sym - GOT, Addend)	11	*/
      r_field = e_rssel;
      goto do_basic_type_2;
    case R_HPPA_GOTOFF_RS14:	/*	RS(Sym - GOT, Addend)	14	*/
      r_field = e_rssel;
      goto do_basic_type_2;
    case R_HPPA_GOTOFF_LD21:	/*	LD(Sym - GOT, Addend)	21	*/
      r_field = e_ldsel;
      goto do_basic_type_2;
    case R_HPPA_GOTOFF_RD11:	/*	RD(Sym - GOT, Addend)	11	*/
      r_field = e_rdsel;
      goto do_basic_type_2;
    case R_HPPA_GOTOFF_RD14:	/*	RD(Sym - GOT, Addend)	14	*/
      r_field = e_rdsel;
      goto do_basic_type_2;
    case R_HPPA_GOTOFF_LR21:	/*	LR(Sym - GOT, Addend)	21	*/
      r_field = e_lrsel;
      goto do_basic_type_2;
    case R_HPPA_GOTOFF_RR14:	/*	RR(Sym - GOT, Addend)	14	*/
      r_field = e_rrsel;
    do_basic_type_2:
      sym_value -= GOT_value;
      insn = hppa_elf_relocate_insn (abfd, input_section, insn, addr,
				     symbol_in, sym_value, r_addend,
				     r_type, r_format, r_field, r_pcrel);
      break;

    case R_HPPA_ABS_CALL_11:	/*	Symbol + Addend		11	*/
      r_field = e_fsel;
      goto do_basic_type_3;
    case R_HPPA_ABS_CALL_14:	/*	Symbol + Addend		14	*/
      r_field = e_fsel;
      goto do_basic_type_3;
    case R_HPPA_ABS_CALL_17:	/*	Symbol + Addend		17	*/
      r_field = e_fsel;
      goto do_basic_type_3;
    case R_HPPA_ABS_CALL_L21:	/*	L (Symbol, Addend)	21 	*/
      r_field = e_lsel;
      goto do_basic_type_3;
    case R_HPPA_ABS_CALL_R11:	/*	R (Symbol, Addend)	11 	*/
      r_field = e_rsel;
      goto do_basic_type_3;
    case R_HPPA_ABS_CALL_R14:	/*	R (Symbol, Addend)	14	*/
      r_field = e_rsel;
      goto do_basic_type_3;
    case R_HPPA_ABS_CALL_R17:	/*	R (Symbol, Addend)	17 	*/
      r_field = e_rsel;
      goto do_basic_type_3;
    case R_HPPA_ABS_CALL_LS21:	/*	LS(Symbol, Addend)	21	*/
      r_field = e_lssel;
      goto do_basic_type_3;
    case R_HPPA_ABS_CALL_RS11:	/*	RS(Symbol, Addend)	11	*/
      r_field = e_lssel;
      goto do_basic_type_3;
    case R_HPPA_ABS_CALL_RS14:	/*	RS(Symbol, Addend)	14	*/
      r_field = e_rssel;
      goto do_basic_type_3;
    case R_HPPA_ABS_CALL_RS17:	/*	RS(Symbol, Addend)	17	*/
      r_field = e_rssel;
      goto do_basic_type_3;
    case R_HPPA_ABS_CALL_LD21:	/*	LD(Symbol, Addend)	21	*/
      r_field = e_ldsel;
      goto do_basic_type_3;
    case R_HPPA_ABS_CALL_RD11:	/*	RD(Symbol, Addend)	11	*/
      r_field = e_rdsel;
      goto do_basic_type_3;
    case R_HPPA_ABS_CALL_RD14:	/*	RD(Symbol, Addend)	14	*/
      r_field = e_rdsel;
      goto do_basic_type_3;
    case R_HPPA_ABS_CALL_RD17:	/*	RD(Symbol, Addend)	17	*/
      r_field = e_rdsel;
      goto do_basic_type_3;
    case R_HPPA_ABS_CALL_LR21:	/*	LR(Symbol, Addend)	21	*/
      r_field = e_lrsel;
      goto do_basic_type_3;
    case R_HPPA_ABS_CALL_RR14:	/*	RR(Symbol, Addend)	14	*/
      r_field = e_rrsel;
      goto do_basic_type_3;
    case R_HPPA_ABS_CALL_RR17:	/*	RR(Symbol, Addend)	17	*/
      r_field = e_rrsel;
    do_basic_type_3:
      insn = hppa_elf_relocate_insn (abfd, input_section, insn, addr,
				     symbol_in, sym_value, r_addend,
				     r_type, r_format, r_field, r_pcrel);
      break;

    case R_HPPA_PCREL_CALL_11:	/*	Symbol - PC + Addend	11	*/
      r_field = e_fsel;
      goto do_basic_type_4;
    case R_HPPA_PCREL_CALL_14:	/*	Symbol - PC + Addend	14	*/
      r_field = e_fsel;
      goto do_basic_type_4;
    case R_HPPA_PCREL_CALL_17:	/*	Symbol - PC + Addend	17	*/
      r_field = e_fsel;
      goto do_basic_type_4;
    case R_HPPA_PCREL_CALL_L21:/*	L (Symbol - PC, Addend)	21 	*/
      r_field = e_lsel;
      goto do_basic_type_4;
    case R_HPPA_PCREL_CALL_R11:/*	R (Symbol - PC, Addend)	11 	*/
      r_field = e_rsel;
      goto do_basic_type_4;
    case R_HPPA_PCREL_CALL_R14:/*	R (Symbol - PC, Addend)	14	*/
      r_field = e_rsel;
      goto do_basic_type_4;
    case R_HPPA_PCREL_CALL_R17:/*	R (Symbol - PC, Addend)	17 	*/
      r_field = e_rsel;
      goto do_basic_type_4;
    case R_HPPA_PCREL_CALL_LS21:	/*	LS(Symbol - PC, Addend)	21	*/
      r_field = e_lssel;
      goto do_basic_type_4;
    case R_HPPA_PCREL_CALL_RS11:	/*	RS(Symbol - PC, Addend)	11	*/
      r_field = e_rssel;
      goto do_basic_type_4;
    case R_HPPA_PCREL_CALL_RS14:	/*	RS(Symbol - PC, Addend)	14	*/
      r_field = e_rssel;
      goto do_basic_type_4;
    case R_HPPA_PCREL_CALL_RS17:	/*	RS(Symbol - PC, Addend)	17	*/
      r_field = e_rssel;
      goto do_basic_type_4;
    case R_HPPA_PCREL_CALL_LD21:	/*	LD(Symbol - PC, Addend)	21	*/
      r_field = e_ldsel;
      goto do_basic_type_4;
    case R_HPPA_PCREL_CALL_RD11:	/*	RD(Symbol - PC, Addend)	11	*/
      r_field = e_rdsel;
      goto do_basic_type_4;
    case R_HPPA_PCREL_CALL_RD14:	/*	RD(Symbol - PC, Addend)	14	*/
      r_field = e_rdsel;
      goto do_basic_type_4;
    case R_HPPA_PCREL_CALL_RD17:	/*	RD(Symbol - PC, Addend)	17	*/
      r_field = e_rdsel;
      goto do_basic_type_4;
    case R_HPPA_PCREL_CALL_LR21:	/*	LR(Symbol - PC, Addend)	21	*/
      r_field = e_lrsel;
      goto do_basic_type_4;
    case R_HPPA_PCREL_CALL_RR14:	/*	RR(Symbol - PC, Addend)	14	*/
      r_field = e_rrsel;
      goto do_basic_type_4;
    case R_HPPA_PCREL_CALL_RR17:	/*	RR(Symbol - PC, Addend)	17	*//* #69 */
      r_field = e_rrsel;
    do_basic_type_4:
      insn = hppa_elf_relocate_insn (abfd, input_section, insn, addr,
				     symbol_in, sym_value, r_addend,
				     r_type, r_format, r_field, r_pcrel);
      break;

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

    case R_HPPA_UNWIND_ENTRY:
    case R_HPPA_UNWIND_ENTRIES:
      hppa_elf_relocate_unwind_table (abfd, input_section, data, addr,
				      symbol_in, sym_value, r_addend,
				      r_type, r_format, r_field, r_pcrel);
      return (bfd_reloc_ok);

    case R_HPPA_PUSH_CONST:	/*	push Addend			-   -	*/
    case R_HPPA_PUSH_PC:	/*	push PC + Addend		-   -	*/
    case R_HPPA_PUSH_SYM:	/*	push Symbol + Addend		-   -	*/
    case R_HPPA_PUSH_GOTOFF:	/*	push Symbol - GOT + Addend	-   -	*/
    case R_HPPA_PUSH_ABS_CALL:	/*	push Symbol + Addend		-   -	*/
    case R_HPPA_PUSH_PCREL_CALL:	/*	push Symbol - PC + Addend	-   -	*/
    case R_HPPA_PUSH_PLABEL:	/*	[TBD]				-   -	*/
    case R_HPPA_MAX:		/*	pop A and B, push max(B,A)	-   -	*/
    case R_HPPA_MIN:		/*	pop A and B, push min(B,A)	-   -	*/
    case R_HPPA_ADD:		/*	pop A and B, push B + A		-   -	*/
    case R_HPPA_SUB:		/*	pop A and B, push B - A		-   -	*/
    case R_HPPA_MULT:		/*	pop A and B, push B * A		-   -	*/
    case R_HPPA_DIV:		/*	pop A and B, push B / A		-   -	*/
    case R_HPPA_MOD:		/*	pop A and B, push B % A		-   -	*/
    case R_HPPA_AND:		/*	pop A and B, push B & A		-   -	*/
    case R_HPPA_OR:		/*	pop A and B, push B | A		-   -	*/
    case R_HPPA_XOR:		/*	pop A and B, push B ^ A		-   -	*/
    case R_HPPA_NOT:		/*	pop A, push ~A			-   -	*/
    case R_HPPA_LSHIFT:	/*	pop A, push A << Addend		-   -	*/
    case R_HPPA_ARITH_RSHIFT:	/*	pop A, push A >> Addend		-   -	*/
    case R_HPPA_LOGIC_RSHIFT:	/*	pop A, push A >> Addend		-   -	*/
    case R_HPPA_EXPR_F:	/*	pop A, push A + Addend		L   -	*/
    case R_HPPA_EXPR_L:	/*	pop A, push L(A,Addend)		L   -	*/
    case R_HPPA_EXPR_R:	/*	pop A, push R(A,Addend)		R   -	*/
    case R_HPPA_EXPR_LS:	/*	pop A, push LS(A,Addend)	LS  -	*/
    case R_HPPA_EXPR_RS:	/*	pop A, push RS(A,Addend)	RS  -	*/
    case R_HPPA_EXPR_LD:	/*	pop A, push LD(A,Addend)	LD  -	*/
    case R_HPPA_EXPR_RD:	/*	pop A, push RD(A,Addend)	RD  -	*/
    case R_HPPA_EXPR_LR:	/*	pop A, push LR(A,Addend)	LR  -	*/
    case R_HPPA_EXPR_RR:	/*	pop A, push RR(A,Addend)	RR  -	*/

    case R_HPPA_EXPR_32:	/*	pop				-   32	*/
    case R_HPPA_EXPR_21:	/*	pop				-   21	*/
    case R_HPPA_EXPR_11:	/*	pop				-   11	*/
    case R_HPPA_EXPR_14:	/*	pop				-   14	*/
    case R_HPPA_EXPR_17:	/*	pop				-   17	*/
    case R_HPPA_EXPR_12:	/*	pop				-   12	*/
      fprintf (stderr, "Relocation problem: ");
      fprintf (stderr, "Unimplemented reloc type %d, in module %s\n",
	       r_type, abfd->filename);
      return (bfd_reloc_notsupported);
    default:
      fprintf (stderr, "Relocation problem : ");
      fprintf (stderr, "Unrecognized reloc type %d, in module %s\n",
	       r_type, abfd->filename);
      return (bfd_reloc_dangerous);
    }

  /* update the instruction word */
  bfd_put_32 (abfd, insn, hit_data);

  return (bfd_reloc_ok);

}

struct elf_reloc_map
  {
    unsigned char bfd_reloc_val;
    unsigned char elf_reloc_val;
  };

static CONST struct elf_reloc_map elf_hppa_reloc_map[] =
{
  {BFD_RELOC_NONE, R_HPPA_NONE,},
  {BFD_RELOC_HPPA_32, R_HPPA_32,},
  {BFD_RELOC_HPPA_11, R_HPPA_11,},
  {BFD_RELOC_HPPA_14, R_HPPA_14,},
  {BFD_RELOC_HPPA_17, R_HPPA_17,},
  {BFD_RELOC_HPPA_L21, R_HPPA_L21,},
  {BFD_RELOC_HPPA_R11, R_HPPA_R11,},
  {BFD_RELOC_HPPA_R14, R_HPPA_R14,},
  {BFD_RELOC_HPPA_R17, R_HPPA_R17,},
  {BFD_RELOC_HPPA_LS21, R_HPPA_LS21,},
  {BFD_RELOC_HPPA_RS11, R_HPPA_RS11,},
  {BFD_RELOC_HPPA_RS14, R_HPPA_RS14,},
  {BFD_RELOC_HPPA_RS17, R_HPPA_RS17,},
  {BFD_RELOC_HPPA_LD21, R_HPPA_LD21,},
  {BFD_RELOC_HPPA_RD11, R_HPPA_RD11,},
  {BFD_RELOC_HPPA_RD14, R_HPPA_RD14,},
  {BFD_RELOC_HPPA_RD17, R_HPPA_RD17,},
  {BFD_RELOC_HPPA_LR21, R_HPPA_LR21,},
  {BFD_RELOC_HPPA_RR14, R_HPPA_RR14,},
  {BFD_RELOC_HPPA_RR17, R_HPPA_RR17,},
  {BFD_RELOC_HPPA_GOTOFF_11, R_HPPA_GOTOFF_11,},
  {BFD_RELOC_HPPA_GOTOFF_14, R_HPPA_GOTOFF_14,},
  {BFD_RELOC_HPPA_GOTOFF_L21, R_HPPA_GOTOFF_L21,},
  {BFD_RELOC_HPPA_GOTOFF_R11, R_HPPA_GOTOFF_R11,},
  {BFD_RELOC_HPPA_GOTOFF_R14, R_HPPA_GOTOFF_R14,},
  {BFD_RELOC_HPPA_GOTOFF_LS21, R_HPPA_GOTOFF_LS21,},
  {BFD_RELOC_HPPA_GOTOFF_RS11, R_HPPA_GOTOFF_RS11,},
  {BFD_RELOC_HPPA_GOTOFF_RS14, R_HPPA_GOTOFF_RS14,},
  {BFD_RELOC_HPPA_GOTOFF_LD21, R_HPPA_GOTOFF_LD21,},
  {BFD_RELOC_HPPA_GOTOFF_RD11, R_HPPA_GOTOFF_RD11,},
  {BFD_RELOC_HPPA_GOTOFF_RD14, R_HPPA_GOTOFF_RD14,},
  {BFD_RELOC_HPPA_GOTOFF_LR21, R_HPPA_GOTOFF_LR21,},
  {BFD_RELOC_HPPA_GOTOFF_RR14, R_HPPA_GOTOFF_RR14,},
  {BFD_RELOC_HPPA_ABS_CALL_11, R_HPPA_ABS_CALL_11,},
  {BFD_RELOC_HPPA_ABS_CALL_14, R_HPPA_ABS_CALL_14,},
  {BFD_RELOC_HPPA_ABS_CALL_17, R_HPPA_ABS_CALL_17,},
  {BFD_RELOC_HPPA_ABS_CALL_L21, R_HPPA_ABS_CALL_L21,},
  {BFD_RELOC_HPPA_ABS_CALL_R11, R_HPPA_ABS_CALL_R11,},
  {BFD_RELOC_HPPA_ABS_CALL_R14, R_HPPA_ABS_CALL_R14,},
  {BFD_RELOC_HPPA_ABS_CALL_R17, R_HPPA_ABS_CALL_R17,},
  {BFD_RELOC_HPPA_ABS_CALL_LS21, R_HPPA_ABS_CALL_LS21,},
  {BFD_RELOC_HPPA_ABS_CALL_RS11, R_HPPA_ABS_CALL_RS11,},
  {BFD_RELOC_HPPA_ABS_CALL_RS14, R_HPPA_ABS_CALL_RS14,},
  {BFD_RELOC_HPPA_ABS_CALL_RS17, R_HPPA_ABS_CALL_RS17,},
  {BFD_RELOC_HPPA_ABS_CALL_LD21, R_HPPA_ABS_CALL_LD21,},
  {BFD_RELOC_HPPA_ABS_CALL_RD11, R_HPPA_ABS_CALL_RD11,},
  {BFD_RELOC_HPPA_ABS_CALL_RD14, R_HPPA_ABS_CALL_RD14,},
  {BFD_RELOC_HPPA_ABS_CALL_RD17, R_HPPA_ABS_CALL_RD17,},
  {BFD_RELOC_HPPA_ABS_CALL_LR21, R_HPPA_ABS_CALL_LR21,},
  {BFD_RELOC_HPPA_ABS_CALL_RR14, R_HPPA_ABS_CALL_RR14,},
  {BFD_RELOC_HPPA_ABS_CALL_RR17, R_HPPA_ABS_CALL_RR17,},
  {BFD_RELOC_HPPA_PCREL_CALL_11, R_HPPA_PCREL_CALL_11,},
  {BFD_RELOC_HPPA_PCREL_CALL_14, R_HPPA_PCREL_CALL_14,},
  {BFD_RELOC_HPPA_PCREL_CALL_17, R_HPPA_PCREL_CALL_17,},
  {BFD_RELOC_HPPA_PCREL_CALL_12, R_HPPA_PCREL_CALL_12,},
  {BFD_RELOC_HPPA_PCREL_CALL_L21, R_HPPA_PCREL_CALL_L21,},
  {BFD_RELOC_HPPA_PCREL_CALL_R11, R_HPPA_PCREL_CALL_R11,},
  {BFD_RELOC_HPPA_PCREL_CALL_R14, R_HPPA_PCREL_CALL_R14,},
  {BFD_RELOC_HPPA_PCREL_CALL_R17, R_HPPA_PCREL_CALL_R17,},
  {BFD_RELOC_HPPA_PCREL_CALL_LS21, R_HPPA_PCREL_CALL_LS21,},
  {BFD_RELOC_HPPA_PCREL_CALL_RS11, R_HPPA_PCREL_CALL_RS11,},
  {BFD_RELOC_HPPA_PCREL_CALL_RS14, R_HPPA_PCREL_CALL_RS14,},
  {BFD_RELOC_HPPA_PCREL_CALL_RS17, R_HPPA_PCREL_CALL_RS17,},
  {BFD_RELOC_HPPA_PCREL_CALL_LD21, R_HPPA_PCREL_CALL_LD21,},
  {BFD_RELOC_HPPA_PCREL_CALL_RD11, R_HPPA_PCREL_CALL_RD11,},
  {BFD_RELOC_HPPA_PCREL_CALL_RD14, R_HPPA_PCREL_CALL_RD14,},
  {BFD_RELOC_HPPA_PCREL_CALL_RD17, R_HPPA_PCREL_CALL_RD17,},
  {BFD_RELOC_HPPA_PCREL_CALL_LR21, R_HPPA_PCREL_CALL_LR21,},
  {BFD_RELOC_HPPA_PCREL_CALL_RR14, R_HPPA_PCREL_CALL_RR14,},
  {BFD_RELOC_HPPA_PCREL_CALL_RR17, R_HPPA_PCREL_CALL_RR17,},
  {BFD_RELOC_HPPA_PLABEL_32, R_HPPA_PLABEL_32,},
  {BFD_RELOC_HPPA_PLABEL_11, R_HPPA_PLABEL_11,},
  {BFD_RELOC_HPPA_PLABEL_14, R_HPPA_PLABEL_14,},
  {BFD_RELOC_HPPA_PLABEL_L21, R_HPPA_PLABEL_L21,},
  {BFD_RELOC_HPPA_PLABEL_R11, R_HPPA_PLABEL_R11,},
  {BFD_RELOC_HPPA_PLABEL_R14, R_HPPA_PLABEL_R14,},
  {BFD_RELOC_HPPA_DLT_32, R_HPPA_DLT_32,},
  {BFD_RELOC_HPPA_DLT_11, R_HPPA_DLT_11,},
  {BFD_RELOC_HPPA_DLT_14, R_HPPA_DLT_14,},
  {BFD_RELOC_HPPA_DLT_L21, R_HPPA_DLT_L21,},
  {BFD_RELOC_HPPA_DLT_R11, R_HPPA_DLT_R11,},
  {BFD_RELOC_HPPA_DLT_R14, R_HPPA_DLT_R14,},
  {BFD_RELOC_HPPA_UNWIND_ENTRY, R_HPPA_UNWIND_ENTRY,},
  {BFD_RELOC_HPPA_UNWIND_ENTRIES, R_HPPA_UNWIND_ENTRIES,},
};

static reloc_howto_type *
elf_hppa_reloc_type_lookup (arch, code)
     bfd_arch_info_type *arch;
     bfd_reloc_code_real_type code;
{
  int i;

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
       elf32_symbol_type * symbolP AND
       int sym_idx)
{
  symext_chainS *symextP;
  unsigned int arg_reloc;

  if (!(symbolP->symbol.flags & BSF_FUNCTION))
    return;

  if (!((symbolP->symbol.flags & BSF_EXPORT) ||
	(symbolP->symbol.flags & BSF_GLOBAL)))
    return;

  arg_reloc = symbolP->tc_data.hppa_arg_reloc;

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

static symext_entryS *symextn_contents = NULL;
static unsigned int symextn_contents_real_size = 0;

void
DEFUN (elf_hppa_tc_make_sections, (abfd, ignored),
       bfd * abfd AND
       PTR ignored)
{
  symext_chainS *symextP;
  symext_entryS *outbound_symexts;
  int size;
  int n;
  int i;
  extern char *stub_section_contents;	/* forward declaration */
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
	elf32_symbol_type *esymP = (elf32_symbol_type *) sym;

	retval = (symext_entryS) esymP->tc_data.hppa_arg_reloc;
	break;
      }
    }
  return retval;
}


typedef struct Elf32_hppa_Stub_description_struct
  {
    bfd *this_bfd;		/* bfd to which this stub */
    /* applies */
    asection *stub_sec;		/* stub section for this bfd */
    unsigned relocs_allocated_cnt;	/* count of relocations for this stub section */
    unsigned real_size;
    unsigned allocated_size;
    int *stub_secp;		/* pointer to the next available location in the buffer */
    char *stub_contents;	/* contents of the stubs for this bfd */
  }

Elf32_hppa_Stub_description;

typedef struct Elf32_hppa_Stub_list_struct
  {
    Elf32_hppa_Stub_description *stub;
    struct Elf32_hppa_Stub_list_struct *next;
  } Elf32_hppa_Stub_list;

static Elf32_hppa_Stub_list *elf_hppa_stub_rootP = NULL;

/* Locate the stub section information for the given bfd. */
static Elf32_hppa_Stub_description *
find_stubs (abfd, stub_sec)
     bfd *abfd;
     asection *stub_sec;
{
  Elf32_hppa_Stub_list *stubP;

  for (stubP = elf_hppa_stub_rootP; stubP; stubP = stubP->next)
    {
      if (stubP->stub->this_bfd == abfd
	  && stubP->stub->stub_sec == stub_sec)
	return stubP->stub;
    }

  return (Elf32_hppa_Stub_description *) NULL;
}

static Elf32_hppa_Stub_description *
new_stub (abfd, stub_sec)
     bfd *abfd;
     asection *stub_sec;
{
  Elf32_hppa_Stub_description *stub = find_stubs (abfd, stub_sec);

  if (stub)
    return stub;

  stub = (Elf32_hppa_Stub_description *) bfd_zalloc (abfd, sizeof (Elf32_hppa_Stub_description));
  stub->this_bfd = abfd;
  stub->stub_sec = stub_sec;
  stub->real_size = 0;
  stub->allocated_size = 0;
  stub->stub_contents = NULL;
  stub->stub_secp = NULL;

  return stub;
}

static void
add_stub (stub)
     Elf32_hppa_Stub_description *stub;
{
  Elf32_hppa_Stub_list *new_entry;

  new_entry = (Elf32_hppa_Stub_list *) bfd_zalloc (stub->this_bfd, sizeof (Elf32_hppa_Stub_list));

  if (new_entry)
    {
      new_entry->stub = stub;

      if (elf_hppa_stub_rootP)
	{
	  new_entry->next = elf_hppa_stub_rootP;
	  elf_hppa_stub_rootP = new_entry;
	}
      else
	{
	  new_entry->next = (Elf32_hppa_Stub_list *) NULL;
	  elf_hppa_stub_rootP = new_entry;
	}
    }
  else
    {
      bfd_error = no_memory;
      bfd_perror ("add_stub");
    }
}

#define ARGUMENTS	0
#define RETURN_VALUE	1

#define	NO_ARG_RELOC	0
#define R_TO_FR		1
#define FR_TO_R		2
#define	ARG_RELOC_ERR	3

#define ARG0	0
#define ARG1	1
#define ARG2	2
#define ARG3	3
#define RETVAL	4

#define AR_NO	0
#define AR_GR	1
#define AR_FR	2
#define AR_FU	3

static CONST char *CONST reloc_type_strings[] =
{
  "NONE", "GR->FR", "FR->GR", "ERROR"
};

static CONST char mismatches[4][4] =
{				/* 	CALLEE NONE	CALLEE GR	CALLEE FR	CALLEE FU	*/
  /* CALLER NONE	*/
 {NO_ARG_RELOC, NO_ARG_RELOC, NO_ARG_RELOC, NO_ARG_RELOC},
 /* CALLER GR	*/
 {NO_ARG_RELOC, NO_ARG_RELOC, R_TO_FR, R_TO_FR},
 /* CALLER FR	*/
 {NO_ARG_RELOC, FR_TO_R, NO_ARG_RELOC, NO_ARG_RELOC},
 /* CALLER FU	*/
 {NO_ARG_RELOC, FR_TO_R, NO_ARG_RELOC, NO_ARG_RELOC},
};

static CONST char retval_mismatches[4][4] =
{				/* 	CALLEE NONE	CALLEE GR	CALLEE FR	CALLEE FU	*/
  /* CALLER NONE	*/
 {NO_ARG_RELOC, NO_ARG_RELOC, NO_ARG_RELOC, NO_ARG_RELOC},
 /* CALLER GR	*/
 {NO_ARG_RELOC, NO_ARG_RELOC, R_TO_FR, R_TO_FR},
 /* CALLER FR	*/
 {NO_ARG_RELOC, FR_TO_R, NO_ARG_RELOC, NO_ARG_RELOC},
 /* CALLER FU	*/
 {NO_ARG_RELOC, FR_TO_R, NO_ARG_RELOC, NO_ARG_RELOC},
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

#include "hppa_stubs.h"

#define NEW_INSTRUCTION(desc,insn)	\
    *((desc)->stub_secp)++ = (insn);	\
    (desc)->real_size += sizeof(int);	\
    bfd_set_section_size((desc)->this_bfd,(desc)->stub_sec,(desc)->real_size);

void
hppa_elf_stub_finish (output_bfd)
     bfd *output_bfd;
{
  extern bfd_error_vector_type bfd_error_vector;
  Elf32_hppa_Stub_list *stub_list = elf_hppa_stub_rootP;
  /* All the stubs have been built.  Finish up building	*/
  /* stub section.  Apply relocations to the section.	*/

  for (; stub_list; stub_list = stub_list->next)
    {
      if (stub_list->stub->real_size)
	{
	  bfd *stub_bfd = stub_list->stub->this_bfd;
	  asection *stub_sec = bfd_get_section_by_name (stub_bfd, ".hppa_linker_stubs");
	  bfd_size_type reloc_size;
	  arelent **reloc_vector;

	  BFD_ASSERT (stub_sec == stub_list->stub->stub_sec);
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
					  stub_list->stub->stub_contents,
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
				    stub_list->stub->stub_contents,
				    0,
				    stub_list->stub->real_size);

	  free (reloc_vector);
	}
    }
}

void
hppa_elf_stub_branch_reloc (stub_desc,	/* the bfd */
			    output_bfd,	/* the output bfd */
			    stub_sym,	/* the stub symbol */
			    offset)	/* the offset within the stub buffer (pre-calculated) */
     Elf32_hppa_Stub_description *stub_desc;
     bfd *output_bfd;
     asymbol *stub_sym;
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

  relent.sym_ptr_ptr[0] = stub_sym;
  relent.howto = bfd_reloc_type_lookup (stub_desc->this_bfd, R_HPPA_ABS_CALL_17);

  /* Save it in the array of relocations for the stub section. */

  memcpy (&stub_desc->stub_sec->relocation[stub_desc->stub_sec->reloc_count++],
	  &relent,
	  sizeof (arelent));
}

asymbol *
hppa_elf_build_arg_reloc_stub (abfd, output_bfd, reloc_entry, stub_types)
     bfd *abfd;
     bfd *output_bfd;
     arelent *reloc_entry;
     int stub_types[5];
{
  asection *stub_sec = bfd_get_section_by_name (abfd, ".hppa_linker_stubs");
  Elf32_hppa_Stub_description *stub_desc = find_stubs (abfd, stub_sec);
  asymbol *stub_sym = NULL;
  asection *output_text_section = bfd_get_section_by_name (output_bfd, ".text");
  int i;
  char stub_sym_name[128];

  if (!stub_sec)
    {
      BFD_ASSERT (stub_desc == NULL);
      stub_sec = bfd_make_section (abfd, ".hppa_linker_stubs");
      bfd_set_section_flags (output_bfd,
			     stub_sec,
			     SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_RELOC | SEC_CODE | SEC_READONLY);
      stub_sec->output_section = output_text_section->output_section;
      stub_sec->output_offset = 0;
      bfd_set_section_alignment (output_bfd, stub_sec, 2);
      stub_desc = new_stub (abfd, stub_sec);
      add_stub (stub_desc);
    }

  /* make sure we have a stub descriptor structure */

  if (!stub_desc)
    {
      stub_desc = new_stub (abfd, stub_sec);
      add_stub (stub_desc);
    }

  /* allocate some space to write the stub */

  if (!stub_desc->stub_contents)
    {
      stub_desc->allocated_size = STUB_BUFFER_INCR;
      stub_desc->stub_contents = (char *) xmalloc (STUB_BUFFER_INCR);
    }
  else if ((stub_desc->allocated_size - stub_desc->real_size) < STUB_MAX_SIZE)
    {
      stub_desc->allocated_size = stub_desc->allocated_size + STUB_BUFFER_INCR;
      stub_desc->stub_contents = (char *) xrealloc (stub_desc->stub_contents,
						    stub_desc->allocated_size);
    }

  stub_desc->stub_secp = (int *) (stub_desc->stub_contents + stub_desc->real_size);

  /* create a symbol to point to this stub */
  stub_sym = bfd_make_empty_symbol (abfd);
  sprintf (stub_sym_name,
	   "_stub_%s_%02d_%02d_%02d_%02d_%02d\000",
	   reloc_entry->sym_ptr_ptr[0]->name,
  stub_types[0], stub_types[1], stub_types[2], stub_types[3], stub_types[4]);
  stub_sym->name = bfd_zalloc (output_bfd, strlen (stub_sym_name) + 1);
  strcpy ((char *) stub_sym->name, stub_sym_name);
  stub_sym->value = (int) stub_desc->stub_secp - (int) stub_desc->stub_contents;
  stub_sym->section = stub_sec;
  stub_sym->flags = BSF_LOCAL | BSF_FUNCTION;

  /* redirect the original relocation from the old symbol (a function) */
  /* to the stub (the stub calls the function).	*/
  /* XXX do we need to change the relocation type? */
  reloc_entry->sym_ptr_ptr = (asymbol **) bfd_zalloc (stub_desc->this_bfd, sizeof (asymbol *));
  reloc_entry->sym_ptr_ptr[0] = stub_sym;

  /* generate the beginning common section for all stubs */

  NEW_INSTRUCTION (stub_desc, ADDI_8_SP);

  /* generate the code to move the arguments around */
  for (i = ARG0; i < ARG3; i++)
    {
      if (stub_types[i] != NO_ARG_RELOC)
	{
	  /* A stub is needed */
	  switch (stub_types[i])
	    {
	    case R_TO_FR:
	      switch (i)
		{
		case ARG0:
		  NEW_INSTRUCTION (stub_desc, LDWS_M8SP_ARG0);
		  NEW_INSTRUCTION (stub_desc, FSTWS_FARG0_M8SP);
		  break;
		case ARG1:
		  NEW_INSTRUCTION (stub_desc, LDWS_M8SP_ARG1);
		  NEW_INSTRUCTION (stub_desc, FSTWS_FARG1_M8SP);
		  break;
		case ARG2:
		  NEW_INSTRUCTION (stub_desc, LDWS_M8SP_ARG1);
		  NEW_INSTRUCTION (stub_desc, FSTWS_FARG1_M8SP);
		  break;
		case ARG3:
		  NEW_INSTRUCTION (stub_desc, LDWS_M8SP_ARG1);
		  NEW_INSTRUCTION (stub_desc, FSTWS_FARG1_M8SP);
		  break;
		}
	      break;

	    case FR_TO_R:
	      switch (i)
		{
		case ARG0:
		  NEW_INSTRUCTION (stub_desc, FLDWS_M8SP_FARG0);
		  NEW_INSTRUCTION (stub_desc, STWS_ARG0_M8SP);
		  break;
		case ARG1:
		  NEW_INSTRUCTION (stub_desc, FLDWS_M8SP_FARG1);
		  NEW_INSTRUCTION (stub_desc, STWS_ARG1_M8SP);
		  break;
		case ARG2:
		  NEW_INSTRUCTION (stub_desc, FLDWS_M8SP_FARG1);
		  NEW_INSTRUCTION (stub_desc, STWS_ARG1_M8SP);
		  break;
		case ARG3:
		  NEW_INSTRUCTION (stub_desc, FLDWS_M8SP_FARG1);
		  NEW_INSTRUCTION (stub_desc, STWS_ARG1_M8SP);
		  break;
		}
	      break;

	    }
	}
    }

  /* generate the branch to the target routine */
  NEW_INSTRUCTION (stub_desc, STW_RP_M8SP);	/* First, save the return address */
  NEW_INSTRUCTION (stub_desc, BL_XXX_RP);	/* set up a branch to the function */

  /* Fix the branch to the function.  We can do this with a relocation. */

  hppa_elf_stub_branch_reloc (stub_desc,
			      output_bfd,	/* the output bfd */
			      stub_sym,	/* the stub symbol */
	   (int) stub_desc->stub_secp - (int) stub_desc->stub_contents - 4);	/* the offset within the stub buffer */

  NEW_INSTRUCTION (stub_desc, NOP);

  /* generate the code to move the return value around */
  i = RETVAL;
  if (stub_types[i] != NO_ARG_RELOC)
    {
      /* A stub is needed */
      switch (stub_types[i])
	{
	case R_TO_FR:
	  NEW_INSTRUCTION (stub_desc, LDWS_M8SP_RET0);
	  NEW_INSTRUCTION (stub_desc, FSTWS_FRET0_M8SP);
	  break;

	case FR_TO_R:
	  NEW_INSTRUCTION (stub_desc, FLDWS_M8SP_FRET0);
	  NEW_INSTRUCTION (stub_desc, STWS_RET0_M8SP);
	  break;
	}
    }

  /* generate the ending common section for all stubs */

  NEW_INSTRUCTION (stub_desc, LDW_M8SP_RP);	/* restore return address */
  NEW_INSTRUCTION (stub_desc, SUBI_8_SP);

  /* XXX: can we assume this is a save return? */
  NEW_INSTRUCTION (stub_desc, BV_N_0RP);

  return stub_sym;
}

int
hppa_elf_arg_reloc_needed_p (abfd, reloc_entry, stub_types)
     bfd *abfd;
     arelent *reloc_entry;
     int stub_types[5];
{
  int i;
  /* If the symbol is still undefined, there is	*/
  /* no way to know if a stub is required.	*/

  if (reloc_entry->sym_ptr_ptr[0] && reloc_entry->sym_ptr_ptr[0]->section != &bfd_und_section)
    {
      symext_entryS caller_ar = (symext_entryS) ELF32_HPPA_R_ARG_RELOC (reloc_entry->addend);
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
	      caller_loc[ARG0] = AR_FU;
	      caller_loc[ARG1] = AR_NO;
	    }
	  if (caller_loc[ARG2] == AR_FU || caller_loc[ARG3] == AR_FU)
	    {
	      caller_loc[ARG2] = AR_FU;
	      caller_loc[ARG3] = AR_NO;
	    }
	  if (callee_loc[ARG0] == AR_FU || callee_loc[ARG1] == AR_FU)
	    {
	      callee_loc[ARG0] = AR_FU;
	      callee_loc[ARG1] = AR_NO;
	    }
	  if (callee_loc[ARG2] == AR_FU || callee_loc[ARG3] == AR_FU)
	    {
	      callee_loc[ARG2] = AR_FU;
	      callee_loc[ARG3] = AR_NO;
	    }

	  stub_types[ARG0] = type_of_mismatch (caller_loc[ARG0], callee_loc[ARG0], ARGUMENTS);
	  stub_types[ARG1] = type_of_mismatch (caller_loc[ARG1], callee_loc[ARG1], ARGUMENTS);
	  stub_types[ARG2] = type_of_mismatch (caller_loc[ARG2], callee_loc[ARG2], ARGUMENTS);
	  stub_types[ARG3] = type_of_mismatch (caller_loc[ARG3], callee_loc[ARG3], ARGUMENTS);
	  stub_types[RETVAL] = type_of_mismatch (caller_loc[RETVAL], callee_loc[RETVAL], RETURN_VALUE);

	  /* XXX for now, just report a */
	  /* warning */

	  /* But, when we start building stubs, here are the steps involved: */
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
	  /*	branch to be R_HPPA_ABS_CALL to the stub routine. */


	  if (stub_types[0]
	      || stub_types[1]
	      || stub_types[2]
	      || stub_types[3]
	      || stub_types[4])
	    {
#ifdef DETECT_STUBS
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
hppa_elf_stub_check (abfd, output_bfd, reloc_entry)
     bfd *abfd;
     bfd *output_bfd;
     arelent *reloc_entry;
{
  int stub_types[5];

  switch (reloc_entry->howto->type)
    {
    case R_HPPA_ABS_CALL_11:	/*	Symbol + Addend		11	*/
    case R_HPPA_ABS_CALL_14:	/*	Symbol + Addend		14	*/
    case R_HPPA_ABS_CALL_17:	/*	Symbol + Addend		17	*/
    case R_HPPA_ABS_CALL_L21:	/*	L (Symbol, Addend)	21 	*/
    case R_HPPA_ABS_CALL_R11:	/*	R (Symbol, Addend)	11 	*/
    case R_HPPA_ABS_CALL_R14:	/*	R (Symbol, Addend)	14	*/
    case R_HPPA_ABS_CALL_R17:	/*	R (Symbol, Addend)	17 	*/
    case R_HPPA_ABS_CALL_LS21:	/*	LS(Symbol, Addend)	21	*/
    case R_HPPA_ABS_CALL_RS11:	/*	RS(Symbol, Addend)	11	*/
    case R_HPPA_ABS_CALL_RS14:	/*	RS(Symbol, Addend)	14	*/
    case R_HPPA_ABS_CALL_RS17:	/*	RS(Symbol, Addend)	17	*/
    case R_HPPA_ABS_CALL_LD21:	/*	LD(Symbol, Addend)	21	*/
    case R_HPPA_ABS_CALL_RD11:	/*	RD(Symbol, Addend)	11	*/
    case R_HPPA_ABS_CALL_RD14:	/*	RD(Symbol, Addend)	14	*/
    case R_HPPA_ABS_CALL_RD17:	/*	RD(Symbol, Addend)	17	*/
    case R_HPPA_ABS_CALL_LR21:	/*	LR(Symbol, Addend)	21	*/
    case R_HPPA_ABS_CALL_RR14:	/*	RR(Symbol, Addend)	14	*/
    case R_HPPA_ABS_CALL_RR17:	/*	RR(Symbol, Addend)	17	*/

    case R_HPPA_PCREL_CALL_11:	/*	Symbol - PC + Addend	11	*/
    case R_HPPA_PCREL_CALL_14:	/*	Symbol - PC + Addend	14	*/
    case R_HPPA_PCREL_CALL_17:	/*	Symbol - PC + Addend	17	*/
    case R_HPPA_PCREL_CALL_12:	/*	Symbol - PC + Addend	12	*/
    case R_HPPA_PCREL_CALL_L21:/*	L (Symbol - PC, Addend)	21 	*/
    case R_HPPA_PCREL_CALL_R11:/*	R (Symbol - PC, Addend)	11 	*/
    case R_HPPA_PCREL_CALL_R14:/*	R (Symbol - PC, Addend)	14	*/
    case R_HPPA_PCREL_CALL_R17:/*	R (Symbol - PC, Addend)	17 	*/
    case R_HPPA_PCREL_CALL_LS21:	/*	LS(Symbol - PC, Addend)	21	*/
    case R_HPPA_PCREL_CALL_RS11:	/*	RS(Symbol - PC, Addend)	11	*/
    case R_HPPA_PCREL_CALL_RS14:	/*	RS(Symbol - PC, Addend)	14	*/
    case R_HPPA_PCREL_CALL_RS17:	/*	RS(Symbol - PC, Addend)	17	*/
    case R_HPPA_PCREL_CALL_LD21:	/*	LD(Symbol - PC, Addend)	21	*/
    case R_HPPA_PCREL_CALL_RD11:	/*	RD(Symbol - PC, Addend)	11	*/
    case R_HPPA_PCREL_CALL_RD14:	/*	RD(Symbol - PC, Addend)	14	*/
    case R_HPPA_PCREL_CALL_RD17:	/*	RD(Symbol - PC, Addend)	17	*/
    case R_HPPA_PCREL_CALL_LR21:	/*	LR(Symbol - PC, Addend)	21	*/
    case R_HPPA_PCREL_CALL_RR14:	/*	RR(Symbol - PC, Addend)	14	*/
    case R_HPPA_PCREL_CALL_RR17:	/*	RR(Symbol - PC, Addend)	17	*/
      if (hppa_elf_arg_reloc_needed_p (abfd, reloc_entry, stub_types))
	{
	  /* generate a stub */
	  return hppa_elf_build_arg_reloc_stub (abfd, output_bfd, reloc_entry, stub_types);
	}
      break;

    default:
      break;

    }
  return reloc_entry->sym_ptr_ptr[0];
}

#define STUB_SYM_BUFFER_INC	5

asymbol *
hppa_look_for_stubs_in_section (abfd, output_bfd, asec, syms, new_sym_cnt)
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

  /* Relocations are in different places depending on whether this is	*/
  /* an output section or an input section.  Also, the relocations are	*/
  /* in different forms.  Sigh.	*/
  /* Luckily, we have bfd_canonicalize_reloc() to straighten this out for us. */

  /* if ( asec->orelocation || asec->relocation ) { */
  if (asec->reloc_count > 0)
    {
      arelent **reloc_vector = (arelent **) alloca (asec->reloc_count * (sizeof (arelent *) + 1));

      bfd_canonicalize_reloc (abfd, asec, reloc_vector, syms);
      for (i = 0; i < asec->reloc_count; i++)
	{
#if 0
	  arelent *rle;

	  if ( asec->orelocation )
	    rle = asec->orelocation[i];
	  else
	    rle = asec->relocation+i;
#endif

	  arelent *rle = reloc_vector[i];

	  switch (rle->howto->type)
	    {
	    case R_HPPA_ABS_CALL_11:	/*	Symbol + Addend		11	*/
	    case R_HPPA_ABS_CALL_14:	/*	Symbol + Addend		14	*/
	    case R_HPPA_ABS_CALL_17:	/*	Symbol + Addend		17	*/
	    case R_HPPA_ABS_CALL_L21:	/*	L (Symbol, Addend)	21 	*/
	    case R_HPPA_ABS_CALL_R11:	/*	R (Symbol, Addend)	11 	*/
	    case R_HPPA_ABS_CALL_R14:	/*	R (Symbol, Addend)	14	*/
	    case R_HPPA_ABS_CALL_R17:	/*	R (Symbol, Addend)	17 	*/
	    case R_HPPA_ABS_CALL_LS21:	/*	LS(Symbol, Addend)	21	*/
	    case R_HPPA_ABS_CALL_RS11:	/*	RS(Symbol, Addend)	11	*/
	    case R_HPPA_ABS_CALL_RS14:	/*	RS(Symbol, Addend)	14	*/
	    case R_HPPA_ABS_CALL_RS17:	/*	RS(Symbol, Addend)	17	*/
	    case R_HPPA_ABS_CALL_LD21:	/*	LD(Symbol, Addend)	21	*/
	    case R_HPPA_ABS_CALL_RD11:	/*	RD(Symbol, Addend)	11	*/
	    case R_HPPA_ABS_CALL_RD14:	/*	RD(Symbol, Addend)	14	*/
	    case R_HPPA_ABS_CALL_RD17:	/*	RD(Symbol, Addend)	17	*/
	    case R_HPPA_ABS_CALL_LR21:	/*	LR(Symbol, Addend)	21	*/
	    case R_HPPA_ABS_CALL_RR14:	/*	RR(Symbol, Addend)	14	*/
	    case R_HPPA_ABS_CALL_RR17:	/*	RR(Symbol, Addend)	17	*/

	    case R_HPPA_PCREL_CALL_11:	/*	Symbol - PC + Addend	11	*/
	    case R_HPPA_PCREL_CALL_14:	/*	Symbol - PC + Addend	14	*/
	    case R_HPPA_PCREL_CALL_17:	/*	Symbol - PC + Addend	17	*/
	    case R_HPPA_PCREL_CALL_12:	/*	Symbol - PC + Addend	12	*/
	    case R_HPPA_PCREL_CALL_L21:	/*	L (Symbol - PC, Addend)	21 	*/
	    case R_HPPA_PCREL_CALL_R11:	/*	R (Symbol - PC, Addend)	11 	*/
	    case R_HPPA_PCREL_CALL_R14:	/*	R (Symbol - PC, Addend)	14	*/
	    case R_HPPA_PCREL_CALL_R17:	/*	R (Symbol - PC, Addend)	17 	*/
	    case R_HPPA_PCREL_CALL_LS21:	/*	LS(Symbol - PC, Addend)	21	*/
	    case R_HPPA_PCREL_CALL_RS11:	/*	RS(Symbol - PC, Addend)	11	*/
	    case R_HPPA_PCREL_CALL_RS14:	/*	RS(Symbol - PC, Addend)	14	*/
	    case R_HPPA_PCREL_CALL_RS17:	/*	RS(Symbol - PC, Addend)	17	*/
	    case R_HPPA_PCREL_CALL_LD21:	/*	LD(Symbol - PC, Addend)	21	*/
	    case R_HPPA_PCREL_CALL_RD11:	/*	RD(Symbol - PC, Addend)	11	*/
	    case R_HPPA_PCREL_CALL_RD14:	/*	RD(Symbol - PC, Addend)	14	*/
	    case R_HPPA_PCREL_CALL_RD17:	/*	RD(Symbol - PC, Addend)	17	*/
	    case R_HPPA_PCREL_CALL_LR21:	/*	LR(Symbol - PC, Addend)	21	*/
	    case R_HPPA_PCREL_CALL_RR14:	/*	RR(Symbol - PC, Addend)	14	*/
	    case R_HPPA_PCREL_CALL_RR17:	/*	RR(Symbol - PC, Addend)	17	*/
	      if (hppa_elf_arg_reloc_needed_p (abfd, rle, stub_types))
		{
		  /* generate a stub */
		  /* keep track of the new symbol */
		  if (new_cnt == new_max)
		    {
		      new_max += STUB_SYM_BUFFER_INC;
		      new_syms = (asymbol *) realloc (new_syms, new_max * sizeof (asymbol));
		    }
		  new_syms[new_cnt++] = *(hppa_elf_build_arg_reloc_stub (abfd, output_bfd, rle, stub_types));
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

int
hppa_look_for_stubs (abfd, output_bfd)
     bfd *abfd;
     bfd *output_bfd;
{
  /* bfd_map_over_sections(abfd,hppa_look_for_stubs_in_section,(PTR)output_bfd,NULL); */
}

boolean
DEFUN (hppa_elf_get_section_contents, (abfd, section, location, offset, count),
       bfd * abfd AND
       sec_ptr section AND
       PTR location AND
       file_ptr offset AND
       bfd_size_type count)
{
  /* if this is the linker stub section, then we have the	*/
  /* section contents in memory rather than on disk.	*/
  if (strcmp (section->name, ".hppa_linker_stubs") == 0)
    {
      Elf32_hppa_Stub_description *stub_desc = find_stubs (abfd, section);

      if (count == 0)
	return true;
      if ((bfd_size_type) (offset + count) > section->_raw_size)
	return (false);		/* on error */
      if ((bfd_size_type) (offset + count) > stub_desc->real_size)
	return (false);		/* on error */

      memcpy (location, stub_desc->stub_contents + offset, count);
      return (true);
    }
  /* if this is the symbol extension section, then we have the	*/
  /* section contents in memory rather than on disk.	*/
  else if (strcmp (section->name, ".hppa_symextn") == 0)
    {
      if (count == 0)
	return true;
      if ((bfd_size_type) (offset + count) > section->_raw_size)
	return (false);		/* on error */
      if ((bfd_size_type) (offset + count) > symextn_contents_real_size)
	return (false);		/* on error */

      memcpy (location, symextn_contents + offset, count);
      return (true);
    }
  else
    return bfd_generic_get_section_contents (abfd, section, location, offset, count);
}

static void
DEFUN (elf_info_to_howto, (abfd, cache_ptr, dst),
       bfd * abfd AND
       arelent * cache_ptr AND
       Elf32_Internal_Rela * dst)
{
  abort ();
}

#define TARGET_BIG_SYM		bfd_elf32_hppa_vec
#define TARGET_BIG_NAME		"elf32-hppa"
#define ELF_ARCH		bfd_arch_hppa

#include "elf32-target.h"
