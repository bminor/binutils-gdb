/* tc-d10v.h -- Header file for tc-d10v.c.
   Copyright (C) 1996 Free Software Foundation, Inc.
   Written by Martin Hunt, Cygnus Support.

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
   the Free Software Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. */

#define TC_D10V

#ifndef BFD_ASSEMBLER
 #error D10V support requires BFD_ASSEMBLER
#endif

/* The target BFD architecture.  */
#define TARGET_ARCH bfd_arch_d10v

#define TARGET_FORMAT "elf32-d10v"

#define MD_APPLY_FIX3
#define md_operand(x)

/* call md_pcrel_from_section, not md_pcrel_from */
#define MD_PCREL_FROM_SECTION(FIXP, SEC) md_pcrel_from_section(FIXP, SEC)   

/* Permit temporary numeric labels.  */
#define LOCAL_LABELS_FB 1

#define LOCAL_LABEL(name) (name[0] == '.' \
			   && (name[1] == 'L' || name[1] == '.'))
#define FAKE_LABEL_NAME ".L0\001"
#define DIFF_EXPR_OK		/* .-foo gets turned into PC relative relocs */

/* We don't need to handle .word strangely.  */
#define WORKING_DOT_WORD


#define md_number_to_chars           number_to_chars_bigendian

#define md_after_pass_hook	     md_after_pass

extern int start_label PARAMS ((void));
#define TC_START_LABEL(ch, ptr)  (ch == ':' && start_label())


