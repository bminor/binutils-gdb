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


static struct elf_backend_data elf_hppa_backend_data = {
  NULL			/* initially, $global$ is undefined */
};

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

/* ELF/PA stab entries */

#ifdef hp800
#undef hp800
#include <a.out.h>	/* we want the non-hp800 definition of 'struct nlist' */
#define hp800
#else
#include <a.out.h>
#endif

/* ELF/PA relocation howto entries */

static bfd_reloc_status_type hppa_elf_reloc();

reloc_howto_type elf_hppa_howto_table[ELF_HOWTO_TABLE_SIZE] = {
	/*  'bitpos' and 'abs' are obsolete */
	/* type			rs sz bsz pcrel  bpos abs    ovrf  sf		   name */
	/* 9.3.4. Address relocation types */
	{ R_HPPA_NONE,		0, 3, 19, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_NONE"},
	{ R_HPPA_32,		0, 3, 32, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_32"	},
	{ R_HPPA_11,		0, 3, 11, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_11"	},
	{ R_HPPA_14,		0, 3, 14, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_14"	},
	{ R_HPPA_17,		0, 3, 17, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_17"	},
	{ R_HPPA_L21,		0, 3, 21, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_L21"	},
	{ R_HPPA_R11,		0, 3, 11, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_R11"	},
	{ R_HPPA_R14,		0, 3, 14, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_R14"	},
	{ R_HPPA_R17,		0, 3, 17, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_R17"	},
	{ R_HPPA_LS21,		0, 3, 21, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_LS21"	},
	{ R_HPPA_RS11,		0, 3, 11, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_RS11"	},
	{ R_HPPA_RS14,		0, 3, 14, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_RS14"	},
	{ R_HPPA_RS17,		0, 3, 17, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_RS17"	},
	{ R_HPPA_LD21,		0, 3, 21, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_LD21"	},
	{ R_HPPA_RD11,		0, 3, 11, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_RD11"	},
	{ R_HPPA_RD14,		0, 3, 14, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_RD14"	},
	{ R_HPPA_RD17,		0, 3, 17, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_RD17"	},
	{ R_HPPA_LR21,		0, 3, 21, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_LR21"	},
	{ R_HPPA_RR14,		0, 3, 14, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_RR14"	},
	{ R_HPPA_RR17,		0, 3, 17, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_RR17"	},
	/* 9.3.5. GOTOFF address relocation types		*/
	{ R_HPPA_GOTOFF_11,	0, 3, 11, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_GOTOFF_11"	},
	{ R_HPPA_GOTOFF_14,	0, 3, 14, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_GOTOFF_14"	},
	{ R_HPPA_GOTOFF_L21,	0, 3, 21, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_GOTOFF_L21"	},
	{ R_HPPA_GOTOFF_R11,	0, 3, 11, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_GOTOFF_R11"	},
	{ R_HPPA_GOTOFF_R14,	0, 3, 14, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_GOTOFF_R14"	},
	{ R_HPPA_GOTOFF_LS21,	0, 3, 21, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_GOTOFF_LS21"	},
	{ R_HPPA_GOTOFF_RS11,	0, 3, 11, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_GOTOFF_RS11"	},
	{ R_HPPA_GOTOFF_RS14,	0, 3, 14, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_GOTOFF_RS14"	},
	{ R_HPPA_GOTOFF_LD21,	0, 3, 21, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_GOTOFF_LD21"	},
	{ R_HPPA_GOTOFF_RD11,	0, 3, 11, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_GOTOFF_RD11"	},
	{ R_HPPA_GOTOFF_RD14,	0, 3, 14, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_GOTOFF_RD14"	},
	{ R_HPPA_GOTOFF_LR21,	0, 3, 21, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_GOTOFF_LR21"	},
	{ R_HPPA_GOTOFF_RR14,	0, 3, 14, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_GOTOFF_RR14"	},
	/* 9.3.6. Absolute call relocation types	*/
	{ R_HPPA_ABS_CALL_11,	0, 3, 11, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_ABS_CALL_11"	},
	{ R_HPPA_ABS_CALL_14,	0, 3, 14, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_ABS_CALL_14"	},
	{ R_HPPA_ABS_CALL_17,	0, 3, 17, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_ABS_CALL_17"	},
	{ R_HPPA_ABS_CALL_L21,	0, 3, 21, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_ABS_CALL_L21"	},
	{ R_HPPA_ABS_CALL_R11,	0, 3, 11, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_ABS_CALL_R11"	},
	{ R_HPPA_ABS_CALL_R14,	0, 3, 14, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_ABS_CALL_R14"	},
	{ R_HPPA_ABS_CALL_R17,	0, 3, 17, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_ABS_CALL_R17"	},
	{ R_HPPA_ABS_CALL_LS21,	0, 3, 21, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_ABS_CALL_LS21"	},
	{ R_HPPA_ABS_CALL_RS11,	0, 3, 11, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_ABS_CALL_RS11"	},
	{ R_HPPA_ABS_CALL_RS14,	0, 3, 14, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_ABS_CALL_RS14"	},
	{ R_HPPA_ABS_CALL_RS17,	0, 3, 17, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_ABS_CALL_RS17"	},
	{ R_HPPA_ABS_CALL_LD21,	0, 3, 21, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_ABS_CALL_LD21"	},
	{ R_HPPA_ABS_CALL_RD11,	0, 3, 11, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_ABS_CALL_RD11"	},
	{ R_HPPA_ABS_CALL_RD14,	0, 3, 14, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_ABS_CALL_RD14"	},
	{ R_HPPA_ABS_CALL_RD17,	0, 3, 17, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_ABS_CALL_RD17"	},
	{ R_HPPA_ABS_CALL_LR21,	0, 3, 21, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_ABS_CALL_LR21"	},
	{ R_HPPA_ABS_CALL_RR14,	0, 3, 14, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_ABS_CALL_RR14"	},
	{ R_HPPA_ABS_CALL_RR17,	0, 3, 17, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_ABS_CALL_RR17"	},
	/* 9.3.7. PC-relative call relocation types	*/
	{ R_HPPA_PCREL_CALL_11,	0, 3, 11, true,  0,   false, true, hppa_elf_reloc, "R_HPPA_PCREL_CALL_11"	},
	{ R_HPPA_PCREL_CALL_14,	0, 3, 14, true,  0,   false, true, hppa_elf_reloc, "R_HPPA_PCREL_CALL_14"	},
	{ R_HPPA_PCREL_CALL_17,	0, 3, 17, true,  0,   false, true, hppa_elf_reloc, "R_HPPA_PCREL_CALL_17"	},
	{ R_HPPA_PCREL_CALL_L21,0, 3, 21, true,  0,   false, true, hppa_elf_reloc, "R_HPPA_PCREL_CALL_L21"	},
	{ R_HPPA_PCREL_CALL_R11,0, 3, 11, true,  0,   false, true, hppa_elf_reloc, "R_HPPA_PCREL_CALL_R11"	},
	{ R_HPPA_PCREL_CALL_R14,0, 3, 14, true,  0,   false, true, hppa_elf_reloc, "R_HPPA_PCREL_CALL_R14"	},
	{ R_HPPA_PCREL_CALL_R17,0, 3, 17, true,  0,   false, true, hppa_elf_reloc, "R_HPPA_PCREL_CALL_R17"	},
	{ R_HPPA_PCREL_CALL_LS21,0,3, 21, true,  0,   false, true, hppa_elf_reloc, "R_HPPA_PCREL_CALL_LS21"	},
	{ R_HPPA_PCREL_CALL_RS11,0,3, 11, true,  0,   false, true, hppa_elf_reloc, "R_HPPA_PCREL_CALL_RS11"	},
	{ R_HPPA_PCREL_CALL_RS14,0,3, 14, true,  0,   false, true, hppa_elf_reloc, "R_HPPA_PCREL_CALL_RS14"	},
	{ R_HPPA_PCREL_CALL_RS17,0,3, 17, true,  0,   false, true, hppa_elf_reloc, "R_HPPA_PCREL_CALL_RS17"	},
	{ R_HPPA_PCREL_CALL_LD21,0,3, 21, true,  0,   false, true, hppa_elf_reloc, "R_HPPA_PCREL_CALL_LD21"	},
	{ R_HPPA_PCREL_CALL_RD11,0,3, 11, true,  0,   false, true, hppa_elf_reloc, "R_HPPA_PCREL_CALL_RD11"	},
	{ R_HPPA_PCREL_CALL_RD14,0,3, 14, true,  0,   false, true, hppa_elf_reloc, "R_HPPA_PCREL_CALL_RD14"	},
	{ R_HPPA_PCREL_CALL_RD17,0,3, 17, true,  0,   false, true, hppa_elf_reloc, "R_HPPA_PCREL_CALL_RD17"	},
	{ R_HPPA_PCREL_CALL_LR21,0,3, 21, true,  0,   false, true, hppa_elf_reloc, "R_HPPA_PCREL_CALL_LR21"	},
	{ R_HPPA_PCREL_CALL_RR14,0,3, 14, true,  0,   false, true, hppa_elf_reloc, "R_HPPA_PCREL_CALL_RR14"	},
	{ R_HPPA_PCREL_CALL_RR17,0,3, 17, true,  0,   false, true, hppa_elf_reloc, "R_HPPA_PCREL_CALL_RR17"	}, /* #69 */

	/* 9.3.8. Plabel relocation types */
	{ R_HPPA_PLABEL_32,	0, 3, 32, true,  0,   false, true, hppa_elf_reloc, "R_HPPA_PLABEL_32"	},
	{ R_HPPA_PLABEL_11,	0, 3, 11, true,  0,   false, true, hppa_elf_reloc, "R_HPPA_PLABEL_11"	},
	{ R_HPPA_PLABEL_14,	0, 3, 14, true,  0,   false, true, hppa_elf_reloc, "R_HPPA_PLABEL_14"	},
	{ R_HPPA_PLABEL_L21,	0, 3, 21, true,  0,   false, true, hppa_elf_reloc, "R_HPPA_PLABEL_L21"	},
	{ R_HPPA_PLABEL_R11,	0, 3, 11, true,  0,   false, true, hppa_elf_reloc, "R_HPPA_PLABEL_R11"	},
	{ R_HPPA_PLABEL_R14,	0, 3, 14, true,  0,   false, true, hppa_elf_reloc, "R_HPPA_PLABEL_R14"	},	/* 75 */

	/* 9.3.9. Data linkage table (DLT) relocation types	*/
	{ R_HPPA_DLT_32,	0, 3, 32, true,  0,   false, true, hppa_elf_reloc, "R_HPPA_DLT_32"	},
	{ R_HPPA_DLT_11,	0, 3, 11, true,  0,   false, true, hppa_elf_reloc, "R_HPPA_DLT_11"	},
	{ R_HPPA_DLT_14,	0, 3, 14, true,  0,   false, true, hppa_elf_reloc, "R_HPPA_DLT_14"	},
	{ R_HPPA_DLT_L21,	0, 3, 21, true,  0,   false, true, hppa_elf_reloc, "R_HPPA_DLT_L21"	},
	{ R_HPPA_DLT_R11,	0, 3, 11, true,  0,   false, true, hppa_elf_reloc, "R_HPPA_DLT_R11"	},
	{ R_HPPA_DLT_R14,	0, 3, 14, true,  0,   false, true, hppa_elf_reloc, "R_HPPA_DLT_R14"	},	/* 81 */

	/* 9.3.10. Relocations for unwinder tables	*/
	{ R_HPPA_UNWIND_ENTRY,	0, 3, 32, true,  0,   false, true, hppa_elf_reloc, "R_HPPA_UNWIND_ENTRY"},
	{ R_HPPA_UNWIND_ENTRIES,0, 3, 32, true,  0,   false, true, hppa_elf_reloc, "R_HPPA_UNWIND_ENTRIES"},	/* 83 */

	{ 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, /* 84-89 */
	{ 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, /* 90-99 */
	{ 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, /* 100-109 */
	{ 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, /* 110-119 */
	{ 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 },		    /* 120-127 */
	
	/*  9.3.11. Relocation types for complex expressions	*/
	{ R_HPPA_PUSH_CONST,	0, 3, 32, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_PUSH_CONST"	},
	{ R_HPPA_PUSH_SYM,	0, 3, 32, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_PUSH_SYM"	},
	{ R_HPPA_PUSH_GOT,	0, 3, 32, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_PUSH_GOT"	},
	{ R_HPPA_PUSH_PC,	0, 3, 32, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_PUSH_PC"	},
	{ R_HPPA_PUSH_PROC,	0, 3, 32, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_PUSH_PROC"	},
	{ R_HPPA_PUSH_PLABEL,	0, 3, 32, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_PUSH_PLABEL"	},
	{ R_HPPA_MAX,		0, 3, 32, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_MAX"	},
	{ R_HPPA_MIN,		0, 3, 32, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_MIN"	},
	{ R_HPPA_ADD,		0, 3, 32, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_ADD"	},
	{ R_HPPA_SUB,		0, 3, 32, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_SUB"	},
	{ R_HPPA_MULT,		0, 3, 32, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_MULT"	},
	{ R_HPPA_DIV,		0, 3, 32, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_DIV"	},
	{ R_HPPA_MOD,		0, 3, 32, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_MOD"	},
	{ R_HPPA_AND,		0, 3, 32, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_AND"	},
	{ R_HPPA_OR,		0, 3, 32, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_OR"	},
	{ R_HPPA_XOR,		0, 3, 32, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_XOR"	},
	{ R_HPPA_NOT,		0, 3, 32, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_NOT"	},
	{ R_HPPA_LSHIFT,	0, 3, 32, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_LSHIFT"	},
	{ R_HPPA_ARITH_RSHIFT,	0, 3, 32, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_ARITH_RSHIFT"	},
	{ R_HPPA_LOGIC_RSHIFT,	0, 3, 32, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_LOGIC_RSHIFT"	},
	{ R_HPPA_EXPR_F,	0, 3, 32, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_L"	},
	{ R_HPPA_EXPR_L,		0, 3, 32, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_EXPR_L"	},
	{ R_HPPA_EXPR_R,		0, 3, 32, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_EXPR_R"	},
	{ R_HPPA_EXPR_LS,		0, 3, 32, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_EXPR_LS"	},
	{ R_HPPA_EXPR_RS,		0, 3, 32, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_EXPR_RS"	},
	{ R_HPPA_EXPR_LD,		0, 3, 32, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_EXPR_LD"	},
	{ R_HPPA_EXPR_RD,		0, 3, 32, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_EXPR_RD"	},
	{ R_HPPA_EXPR_LR,		0, 3, 32, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_EXPR_LR"	},
	{ R_HPPA_EXPR_RR,		0, 3, 32, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_EXPR_RR"	},

	{ R_HPPA_EXPR_32,	0, 3, 32, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_EXPR_32"	},
	{ R_HPPA_EXPR_21,	0, 3, 21, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_EXPR_21"	},
	{ R_HPPA_EXPR_11,	0, 3, 11, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_EXPR_11"	},
	{ R_HPPA_EXPR_14,	0, 3, 14, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_EXPR_14"	},
	{ R_HPPA_EXPR_17,	0, 3, 17, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_EXPR_17"	},
	{ R_HPPA_EXPR_12,	0, 3, 12, false, 0,   false, true, hppa_elf_reloc, "R_HPPA_EXPR_12"	},
	{ R_HPPA_UNIMPLEMENTED, 0, 0,  0, false, 0,   false, false,NULL,	   "R_HPPA_UNIMPLEMENTED"},	/* 163 */
};

static unsigned long
DEFUN(hppa_elf_rebuild_insn, (abfd,insn,value,r_type,r_field, r_format),
      bfd *abfd AND
      unsigned long insn AND
      unsigned long value AND
      unsigned short r_type AND
      unsigned short r_field AND
      unsigned short r_format)
{
	unsigned long const_part; /* part of the instruction that does not change */
	unsigned long rebuilt_part;

	switch ( r_format ) {
	case 11: {
		unsigned w1, w;

		const_part = insn & 0xffffe002;
		dis_assemble_12(value,&w1,&w);
		rebuilt_part = (w1 << 2) | w;
		return const_part | rebuilt_part;
	}

	case 12: {
		unsigned w1, w;

		const_part = insn & 0xffffe002;
		dis_assemble_12(value,&w1,&w);
		rebuilt_part = (w1 << 2) | w;
		return const_part | rebuilt_part;
	}

	case 14:
		const_part = insn & 0xffffc000;
		low_sign_unext(value,14,&rebuilt_part);
		return const_part | rebuilt_part;

	case 17: {
		unsigned w1, w2, w;

		const_part = insn & 0xffe0e002;
		dis_assemble_17(value,&w1,&w2,&w);
		rebuilt_part = (w2 << 2) | (w1 << 16) | w;
		return const_part | rebuilt_part;
	}

	case 21:
		const_part = insn & 0xffe00000;
		dis_assemble_21(value,&rebuilt_part);
		return const_part | rebuilt_part;

	case 32:
		const_part = 0;
		return value;
		
	default:
		fprintf(stderr,"Relocation problem : ");
		fprintf(stderr,"Unrecognized reloc type %d (fmt=%d,fld=%d), in module %s\n",
			r_type, r_format, r_field, abfd->filename); 
	}
	return insn;
}

static unsigned long
DEFUN(hppa_elf_relocate_insn,
      (abfd, input_sect,
       insn, address, symp, sym_value, r_addend,
       r_type, r_format, r_field, pcrel),
      bfd *abfd AND
      asection *input_sect AND
      unsigned long insn AND
      unsigned long address AND
      asymbol *symp AND
      long sym_value AND
      long r_addend AND
      unsigned short r_type AND
      unsigned short r_format AND
      unsigned short r_field AND
      unsigned char pcrel)
{
	unsigned char opcode = get_opcode(insn);
	long constant_value;
	unsigned arg_reloc;

	switch ( opcode ) {
	    case LDO:
	    case LDB:
	    case LDH:
	    case LDW:
	    case LDWM:
	    case STB:
	    case STH:
	    case STW:
	    case STWM:
		constant_value = ELF32_HPPA_R_CONSTANT(r_addend);
		BFD_ASSERT(r_format == 14);

		if ( pcrel )
			sym_value -= address;
		sym_value = hppa_field_adjust(sym_value,constant_value,r_field);
		return hppa_elf_rebuild_insn(abfd,insn,sym_value,r_type,r_format, r_format);

	    case COMICLR:
	    case SUBI:	/* case SUBIO: */
	    case ADDIT:	/* case ADDITO: */
	    case ADDI:	/* case ADDIO: */
		BFD_ASSERT(r_format == 11);

		constant_value = ((insn & 0x1) << 10) | ((insn & 0xffe) >> 1);
		sym_value = hppa_field_adjust(sym_value,constant_value,r_field);
		return hppa_elf_rebuild_insn(abfd,insn,sym_value,r_type,r_field, r_format);

	    case LDIL:
	    case ADDIL:
		BFD_ASSERT(r_format == 21);

		constant_value = assemble_21(insn);
		sym_value = hppa_field_adjust(sym_value,constant_value,r_field);
		return hppa_elf_rebuild_insn(abfd,insn,sym_value,r_type,r_field, r_format);

	    case BL:
	    case BE:
	    case BLE:
		arg_reloc = ELF32_HPPA_R_ARG_RELOC(r_addend);

		BFD_ASSERT(r_format == 17);

		/* XXX computing constant_value is not needed??? */
		constant_value = assemble_17((insn & 0x001f0000) >> 16,
					     (insn & 0x00001ffc) >> 2,
					     insn & 1);
		constant_value = sign_ext(constant_value,17);
		if ( pcrel ) {
			sym_value -=
				address + input_sect->output_offset
					+ input_sect->output_section->vma;
			sym_value = hppa_field_adjust(sym_value,-8,r_field);
		}
		else
			sym_value = hppa_field_adjust(sym_value, constant_value, r_field);

		return hppa_elf_rebuild_insn(abfd,insn,sym_value >> 2,r_type,r_field, r_format);

	    default:
		if ( opcode == 0 && r_format == 32 ) {
			constant_value = insn;
			sym_value = hppa_field_adjust(sym_value,constant_value,r_field);
			return sym_value;			
		}
		else {
			fprintf(stderr,
				"Unrecognized opcode 0x%02x (fmt=%x,field=%x)\n",
				opcode, r_format, r_field);
			return (insn);
		}
	}
}

static void
DEFUN(hppa_elf_relocate_unwind_table,
      (abfd, input_sect,
       data, address, symp, sym_value, r_addend,
       r_type, r_format, r_field, pcrel),
      bfd *abfd AND
      asection *input_sect AND
      PTR data AND
      unsigned long address AND
      asymbol *symp AND
      long sym_value AND
      long r_addend AND
      unsigned short r_type AND
      unsigned short r_format AND
      unsigned short r_field AND
      unsigned char pcrel)
{
	bfd_byte  *hit_data	= address + (bfd_byte *)(data);
	long constant_value;
	long start_offset;
	long end_offset;
	long relocated_value;
	int i;

	BFD_ASSERT( r_format == 32 );
	BFD_ASSERT( r_field == e_fsel );
	switch ( r_type ) {
	    case R_HPPA_UNWIND_ENTRY:
		start_offset = bfd_get_32(abfd, hit_data);
		relocated_value = hppa_field_adjust(sym_value,start_offset,r_field);
		bfd_put_32(abfd, relocated_value ,hit_data);

		hit_data += sizeof(unsigned long);
		end_offset = bfd_get_32(abfd, hit_data);
		relocated_value = hppa_field_adjust(sym_value,end_offset,r_field);
		bfd_put_32(abfd, relocated_value ,hit_data);
		break;

	    case R_HPPA_UNWIND_ENTRIES:
		for ( i = 0; i < r_addend; i++,hit_data += 3*sizeof(unsigned long) ) {
			unsigned int fsize;
			start_offset = bfd_get_32(abfd, hit_data);
			/* Stuff the symbol value into the first word */
			/* of the  unwind descriptor */
			bfd_put_32(abfd, sym_value ,hit_data);

			hit_data += sizeof(unsigned long);
			end_offset = bfd_get_32(abfd, hit_data);
			/* We could also compute the ending offset for */
			/* the 2nd word of the unwind entry by */
			/* retrieving the st_size field of the Elf_Sym */
			/* structure stored with this symbol.  We can */
			/* get it with: 	*/
			/* 	e = (elf_symbol_type *)symp	*/
			/*	fsize = e->internal_elf_sym.st_size */

			fsize = end_offset - start_offset;
			relocated_value = hppa_field_adjust(sym_value,fsize,r_field);
			bfd_put_32(abfd, relocated_value ,hit_data);

			/* If this is not the last unwind entry, */
			/* adjust the symbol value. */
			if ( i+1 < r_addend ) {
				start_offset = bfd_get_32(abfd, hit_data+3*sizeof(unsigned long));
				sym_value += fsize + start_offset - end_offset;
			}
		}
		break;

	    default:
		fprintf(stderr,
			"Unrecognized relocation type 0x%02x (fmt=%x,field=%x)\n",
			r_type, r_format, r_field);
	}
	return;
}

/* Provided the symbol, returns the value reffed */
static  long
get_symbol_value(symbol)       
asymbol *symbol;
{                                             
	long relocation = 0;

	if ( symbol == (asymbol *)NULL )
		relocation = 0;
	else if (symbol->section == &bfd_com_section) {
		relocation = 0;                           
	}
	else {                                      
		relocation = symbol->value +
			symbol->section->output_section->vma +
				symbol->section->output_offset;
	}                                           

	return(relocation);
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
	hppa_elf_gen_reloc_error(base_type,fmt,field)
elf32_hppa_reloc_type base_type;
int fmt;
int field;
{
	fprintf(stderr, "undefined relocation: base=0x%x,fmt=0x%x,field=0x%x\n",
		base_type, fmt, field);
}

unsigned char
hppa_elf_gen_reloc_type(base_type, format, field)
elf32_hppa_reloc_type base_type;
int format;
int field;
{
#define UNDEFINED	hppa_elf_gen_reloc_error(base_type,format,field);

	elf32_hppa_reloc_type final_type = base_type;
	switch ( base_type ) {
	    case R_HPPA:
		switch (format)	{
		    case 11:
			switch ( field ) {
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
				
			    case e_fsel:
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
			switch ( field ) {
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
			switch ( field ) {
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
		    case 21:
			switch ( field ) {
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
			switch ( field ) {
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
		switch (format)	{
		    case 11:
			switch ( field ) {
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
			switch ( field ) {
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
			switch ( field ) {
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
		switch (format)	{
		    case 11:
			switch ( field ) {
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
			switch ( field ) {
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
			switch ( field ) {
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
			switch ( field ) {
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
		switch (format) {
		    case 11:
			switch (field) {
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
			switch (field) {
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
			switch (field) {
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
			switch (field) {
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
		switch (format)	{
		    case 11:
			switch ( field ) {
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
			switch ( field ) {
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
			switch ( field ) {
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
			switch ( field ) {
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
	    default:
		final_type = base_type;
		break;
	}

	return final_type;
}

/* 12.4.4. Derive format from instruction	*/

/*	Given a machine instruction, this function determines its format.	*/
/*	The format can be determined solely from looking at the first six	*/
/*	bits (the major opcode) of the instruction.  Several major opcodes	*/
/*	map to the same format.  Opcodes which do not map to a known format	*/
/*	should probably be reported as an error.				*/

unsigned char
hppa_elf_insn2fmt(type, insn)
elf32_hppa_reloc_type type;
unsigned long insn;
{
	unsigned char fmt = 0;	/* XXX: is this a proper default?	*/
	unsigned char op = get_opcode(insn);

	if ( type == R_HPPA_NONE )
		fmt = 0;
	else {
		switch ( op ) {
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
long global_value	= 0;
long	GOT_value	= 0;	/* XXX:  need to calculate this! For HPUX, GOT == DP */
asymbol *global_symbol = (asymbol *)NULL;

static bfd_reloc_status_type
DEFUN(hppa_elf_reloc,(abfd, reloc_entry, symbol_in, data, input_section, output_bfd),
      bfd *abfd AND
      arelent *reloc_entry AND
      asymbol *symbol_in AND
      PTR data AND
      asection *input_section AND
      bfd *output_bfd)
{
	unsigned long insn;
	long	sym_value	= 0;
	unsigned long unsigned_value;
	long signed_value;

	unsigned long addr	= reloc_entry->address ; /*+ input_section->vma*/
	bfd_byte  *hit_data	= addr + (bfd_byte *)(data);
	unsigned short r_type	= reloc_entry->howto->type & 0xFF;
	unsigned short r_field	= e_fsel;
	boolean r_pcrel		= reloc_entry->howto->pc_relative;

	/* howto->bitsize contains the format (11, 14, 21, etc) information */
	unsigned r_format	= reloc_entry->howto->bitsize;
	long r_addend		= reloc_entry->addend;
	

	if (output_bfd) {
		/* Partial linking - do nothing */
		reloc_entry->address += input_section->output_offset;
		return bfd_reloc_ok;
	}

	if ( symbol_in && symbol_in->section == &bfd_und_section )
		return bfd_reloc_undefined;

	sym_value = get_symbol_value(symbol_in);

	/* compute value of $global$ if it is there. */

	if ( global_symbol == (asymbol *)NULL ) {
		struct elf_backend_data * bed
			= (struct elf_backend_data *)abfd->xvec->backend_data;

		if ( bed && bed->global_sym ) {
			asymbol *gsym = &bed->global_sym->symbol;
			global_value
				= gsym->value
					+ gsym->section->output_section->vma
						+ gsym->section->output_offset;
			GOT_value = global_value;   /* XXX: For HP-UX, GOT==DP */
			global_symbol = gsym;
		}
	}

	/* get the instruction word */
	insn = bfd_get_32(abfd, hit_data);

	/* relocate the value based on the relocation type */

	/* basic_type_1:	relocation is relative to $global$		*/
	/* basic_type_2:	relocation is relative to the current GOT	*/
	/* basic_type_3:	relocation is an absolute call			*/
	/* basic_type_4:	relocation is an PC-relative call		*/
	/* basic_type_5:	relocation is plabel reference			*/
	/* basic_type_6:	relocation is an unwind table relocation	*/
	/* extended_type:	unimplemented					*/
	
	switch ( r_type ) {
	    case R_HPPA_NONE:
		break;
	    case R_HPPA_32:	/*		Symbol + Addend		32	*/
		
		r_field = e_fsel;
		goto do_basic_type_1;
	    case R_HPPA_L21:	/*		L (Symbol, Addend)	21 	*/
		r_field = e_lsel;
		goto do_basic_type_1;
	    case R_HPPA_R11:	/*		R (Symbol, Addend)	11 	*/
		r_field = e_rsel;
		goto do_basic_type_1;
	    case R_HPPA_R14:	/*		R (Symbol, Addend)	14	*/
		r_field = e_rsel;
		goto do_basic_type_1;
	    case R_HPPA_R17:	/*		R (Symbol, Addend)	17 	*/
		r_field = e_rsel;
		goto do_basic_type_1;
	    case R_HPPA_LS21:	/*		LS(Symbol, Addend)	21	*/
		r_field = e_lssel;
		goto do_basic_type_1;
	    case R_HPPA_RS11:	/*		RS(Symbol, Addend)	11	*/
		r_field = e_rssel;
		goto do_basic_type_1;
	    case R_HPPA_RS14:	/*		RS(Symbol, Addend)	14	*/
		r_field = e_rssel;
		goto do_basic_type_1;
	    case R_HPPA_RS17:	/*		RS(Symbol, Addend)	17	*/
		r_field = e_ldsel;
		goto do_basic_type_1;
	    case R_HPPA_LD21:	/*		LD(Symbol, Addend)	21	*/
		r_field = e_ldsel;
		goto do_basic_type_1;
	    case R_HPPA_RD11:	/*		RD(Symbol, Addend)	11	*/
		r_field = e_rdsel;
		goto do_basic_type_1;
	    case R_HPPA_RD14:	/*		RD(Symbol, Addend)	14	*/
		r_field = e_rdsel;
		goto do_basic_type_1;
	    case R_HPPA_RD17:	/*		RD(Symbol, Addend)	17	*/
		r_field = e_rdsel;
		goto do_basic_type_1;
	    case R_HPPA_LR21:	/*		LR(Symbol, Addend)	21	*/
		r_field = e_lrsel;
		goto do_basic_type_1;
	    case R_HPPA_RR14:	/*		RR(Symbol, Addend)	14	*/
		r_field = e_rrsel;
		goto do_basic_type_1;
	    case R_HPPA_RR17:	/*		RR(Symbol, Addend)	17	*/
		r_field = e_rrsel;
		
	    do_basic_type_1:
		insn = hppa_elf_relocate_insn(abfd, input_section, insn, addr,
					      symbol_in, sym_value, r_addend,
					      r_type, r_format, r_field, r_pcrel);
		break;
		
	    case R_HPPA_GOTOFF_11:		/*	Symbol - GOT + Addend	11	*/
		r_field = e_fsel;
		goto do_basic_type_2;
	    case R_HPPA_GOTOFF_14:		/*	Symbol - GOT + Addend	14	*/
		r_field = e_fsel;
		goto do_basic_type_2;
	    case R_HPPA_GOTOFF_L21:		/*	L (Sym - GOT, Addend)	21	*/
		r_field = e_lsel;
		goto do_basic_type_2;
	    case R_HPPA_GOTOFF_R11:		/*	R (Sym - GOT, Addend)	11	*/
		r_field = e_rsel;
		goto do_basic_type_2;
	    case R_HPPA_GOTOFF_R14:		/*	R (Sym - GOT, Addend)	14	*/
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
		insn = hppa_elf_relocate_insn(abfd, input_section, insn, addr,
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
		insn = hppa_elf_relocate_insn(abfd, input_section, insn, addr,
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
	    case R_HPPA_PCREL_CALL_L21:	/*	L (Symbol - PC, Addend)	21 	*/
		r_field = e_lsel;
		goto do_basic_type_4;
	    case R_HPPA_PCREL_CALL_R11:	/*	R (Symbol - PC, Addend)	11 	*/
		r_field = e_rsel;
		goto do_basic_type_4;
	    case R_HPPA_PCREL_CALL_R14:	/*	R (Symbol - PC, Addend)	14	*/
		r_field = e_rsel;
		goto do_basic_type_4;
	    case R_HPPA_PCREL_CALL_R17:	/*	R (Symbol - PC, Addend)	17 	*/
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
	    case R_HPPA_PCREL_CALL_RR17:	/*	RR(Symbol - PC, Addend)	17	*/ /* #69 */
		r_field = e_rrsel;
	    do_basic_type_4:
		insn = hppa_elf_relocate_insn(abfd, input_section, insn, addr,
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
		insn = hppa_elf_relocate_insn(abfd, input_section, insn, addr,
					      symbol_in, sym_value, r_addend,
					      r_type, r_format, r_field, r_pcrel);
		break;
		
	    case R_HPPA_UNWIND_ENTRY:
	    case R_HPPA_UNWIND_ENTRIES:
		hppa_elf_relocate_unwind_table(abfd, input_section, data, addr,
					       symbol_in, sym_value, r_addend,
					       r_type, r_format, r_field, r_pcrel);
		return (bfd_reloc_ok);
		
	    case R_HPPA_PUSH_CONST:		/*	push Addend			-   -	*/
	    case R_HPPA_PUSH_SYM:		/*	push Symbol			-   -	*/
	    case R_HPPA_PUSH_GOT:		/*	push GOT			-   -	*/
	    case R_HPPA_PUSH_PC:		/*	push PC				-   -	*/
	    case R_HPPA_PUSH_PROC:		/*	push Symbol			-   -	*/
	    case R_HPPA_PUSH_PLABEL:	/*	[TBD]				-   -	*/
	    case R_HPPA_MAX:		/*	pop A and B, push max(B,A)	-   -	*/
	    case R_HPPA_MIN:		/*	pop A and B, push min(B,A)	-   -	*/
	    case R_HPPA_ADD:		/*	pop A and B, push B + A		-   -	*/
	    case R_HPPA_SUB:		/*	pop A and B, push B - A		-   -	*/
	    case R_HPPA_MULT:		/*	pop A and B, push B * A		-   -	*/
	    case R_HPPA_DIV:		/*	pop A and B, push B / A		-   -	*/
	    case R_HPPA_MOD:		/*	pop A and B, push B % A		-   -	*/
	    case R_HPPA_AND:		/*	pop A and B, push B & A		-   -	*/
	    case R_HPPA_OR:			/*	pop A and B, push B | A		-   -	*/
	    case R_HPPA_XOR:		/*	pop A and B, push B ^ A		-   -	*/
	    case R_HPPA_NOT:		/*	pop A, push ~A			-   -	*/
	    case R_HPPA_LSHIFT:		/*	pop A, push A << Addend		-   -	*/
	    case R_HPPA_ARITH_RSHIFT:	/*	pop A, push A >> Addend		-   -	*/
	    case R_HPPA_LOGIC_RSHIFT:	/*	pop A, push A >> Addend		-   -	*/
	    case R_HPPA_EXPR_F:		/*	pop A, push A + Addend		L   -	*/
	    case R_HPPA_EXPR_L:		/*	pop A, push L(A,Addend)		L   -	*/
	    case R_HPPA_EXPR_R:		/*	pop A, push R(A,Addend)		R   -	*/
	    case R_HPPA_EXPR_LS:		/*	pop A, push LS(A,Addend)	LS  -	*/
	    case R_HPPA_EXPR_RS:		/*	pop A, push RS(A,Addend)	RS  -	*/
	    case R_HPPA_EXPR_LD:		/*	pop A, push LD(A,Addend)	LD  -	*/
	    case R_HPPA_EXPR_RD:		/*	pop A, push RD(A,Addend)	RD  -	*/
	    case R_HPPA_EXPR_LR:		/*	pop A, push LR(A,Addend)	LR  -	*/
	    case R_HPPA_EXPR_RR:		/*	pop A, push RR(A,Addend)	RR  -	*/
		
	    case R_HPPA_EXPR_32:		/*	pop				-   32	*/
	    case R_HPPA_EXPR_21:		/*	pop				-   21	*/
	    case R_HPPA_EXPR_11:		/*	pop				-   11	*/
	    case R_HPPA_EXPR_14:		/*	pop				-   14	*/
	    case R_HPPA_EXPR_17:		/*	pop				-   17	*/
	    case R_HPPA_EXPR_12:		/*	pop				-   12	*/
		fprintf(stderr, "Relocation problem: ");
		fprintf(stderr,"Unimplemented reloc type %d, in module %s\n",
			r_type,abfd->filename);
		return(bfd_reloc_notsupported);
	    default:
		fprintf(stderr,"Relocation problem : ");
		fprintf(stderr,"Unrecognized reloc type %d, in module %s\n",
			r_type,abfd->filename); 
		return (bfd_reloc_dangerous);
	}

	/* update the instruction word */
	bfd_put_32(abfd, insn ,hit_data);

	return (bfd_reloc_ok);

}

static reloc_howto_type *
elf_hppa_reloc_type_lookup (arch, code)
     bfd_arch_info_type *arch;
     bfd_reloc_code_real_type code;
{
  if (code < R_HPPA_UNIMPLEMENTED)
    return &elf_hppa_howto_table[code];

  return (reloc_howto_type *)0;
}
#define elf_bfd_reloc_type_lookup	elf_hppa_reloc_type_lookup

static void
DEFUN (elf_info_to_howto, (abfd, cache_ptr, dst),
       bfd *abfd AND
       arelent *cache_ptr AND
       Elf_Internal_Rela *dst)
{
  abort ();
}

#define TARGET_BIG_SYM		elf32_hppa_vec
#define TARGET_BIG_NAME		"elf32-hppa"
#define ELF_ARCH		bfd_arch_hppa

#include "elf32-target.h"
