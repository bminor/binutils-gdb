/* tc-dvp.h -- Header file for tc-dvp.c.
   Copyright (C) 1997, 1998 Free Software Foundation, Inc.

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to
   the Free Software Foundation, 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA. */

#define TC_DVP

#ifndef BFD_ASSEMBLER
/* leading space so will compile with cc */
 #error DVP support requires BFD_ASSEMBLER
#endif

#define LISTING_HEADER "DVP GAS "

/* The target BFD architecture.  */
#define TARGET_ARCH bfd_arch_mips

#define TARGET_FORMAT "elf32-littlemips"

#define TARGET_BYTES_BIG_ENDIAN 0

/* call md_pcrel_from_section, not md_pcrel_from */
#define MD_PCREL_FROM_SECTION(FIXP, SEC) md_pcrel_from_section(FIXP, SEC)   

/* Permit temporary numeric labels.  */
#define LOCAL_LABELS_FB 1

#define DIFF_EXPR_OK		/* .-foo gets turned into PC relative relocs */

/* We don't need to handle .word strangely.  */
#define WORKING_DOT_WORD

/* Handle mpg/direct alignment requirements with relaxation.  */
extern long dvp_relax_frag PARAMS ((fragS *, long));
#define md_relax_frag(fragP,stretch) dvp_relax_frag ((fragP), (stretch))

#define MD_APPLY_FIX3

/* Ensure insns at labels have their mach type properly recorded.  */
int force_mach_label PARAMS ((void));
#define TC_START_LABEL(ch, ptr)	(ch == ':' && force_mach_label ())

#define TC_HANDLES_FX_DONE

/* Record user specified val, for cases where we can't compute the actual
   value until the end of assembly.  */
#define TC_FIX_TYPE \
struct { \
  int type; /* gif_type, or vif type */ \
  int nregs; /* for gif insns only */ \
  short wl; short cl; /* for unpack only */ \
  int user_value; \
}
/* Code to initialize it.  */
#define TC_INIT_FIX_DATA(fixP) \
do { memset (&fixP->tc_fix_data, 0, sizeof (fixP->tc_fix_data)); } while (0)

/* Called after parsing a file.  */
extern void dvp_parse_done PARAMS ((void));
#define md_after_pass_hook() dvp_after_pass_hook ()

/* Called for each label.  */
extern void dvp_frob_label PARAMS ((struct symbol *));
#define tc_frob_label(sym) dvp_frob_label (sym)

/* Called just before writing the file out.  */
extern void dvp_frob_file PARAMS ((void));
#define tc_frob_file() dvp_frob_file ()

/* Default section names. */
#define TEXT_SECTION_NAME	".vutext"
#define DATA_SECTION_NAME	".vudata"
#define BSS_SECTION_NAME	".vubss"

#define ELF_TC_SPECIAL_SECTIONS \
  { ".vubss",	SHT_NOBITS,	SHF_ALLOC + SHF_WRITE		}, \
  { ".vudata",	SHT_PROGBITS,	SHF_ALLOC + SHF_WRITE		}, \
  { ".vutext",	SHT_PROGBITS,	SHF_ALLOC + SHF_EXECINSTR	},
