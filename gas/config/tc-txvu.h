/* tc-txvu.h -- Header file for tc-txvu.c.
   Copyright (C) 1997 Free Software Foundation, Inc.

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

#define TC_TXVU

#ifndef BFD_ASSEMBLER
/* leading space so will compile with cc */
 #error TXVU support requires BFD_ASSEMBLER
#endif

#define LISTING_HEADER "TXVU GAS "

/* The target BFD architecture.  */
#define TARGET_ARCH bfd_arch_txvu

#define TARGET_FORMAT "elf32-txvu"

#define TARGET_BYTES_BIG_ENDIAN 0

/* call md_pcrel_from_section, not md_pcrel_from */
#define MD_PCREL_FROM_SECTION(FIXP, SEC) md_pcrel_from_section(FIXP, SEC)   

/* Permit temporary numeric labels.  */
#define LOCAL_LABELS_FB 1

#define DIFF_EXPR_OK		/* .-foo gets turned into PC relative relocs */

/* We don't need to handle .word strangely.  */
#define WORKING_DOT_WORD

#define md_convert_frag(b,s,f)		{as_fatal ("txvu convert_frag\n");}
#define md_estimate_size_before_relax(f,s) \
			(as_fatal("estimate_size_before_relax called"),1)

#define MD_APPLY_FIX3

#define TC_HANDLES_FX_DONE
