/* This file is tc-m68k.h

   Copyright (C) 1987-1992 Free Software Foundation, Inc.

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
   the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#define TC_M68K 1

#ifdef OBJ_AOUT
#define TARGET_FORMAT "a.out-sunos-big"
#endif

#define COFF_MAGIC MC68MAGIC
#define BFD_ARCH bfd_arch_m68k
#define COFF_FLAGS F_AR32W
#define TC_COUNT_RELOC(x) ((x)->fx_addsy||(x)->fx_subsy)

#define TC_COFF_FIX2RTYPE(fixP) tc_coff_fix2rtype(fixP)
#define TC_COFF_SIZEMACHDEP(frag) tc_coff_sizemachdep(frag)
#ifdef TE_SUN3
/* This variable contains the value to write out at the beginning of
   the a.out file.  The 2<<16 means that this is a 68020 file instead
   of an old-style 68000 file */

#define DEFAULT_MAGIC_NUMBER_FOR_OBJECT_FILE (2<<16|OMAGIC);	/* Magic byte for file header */
#endif /* TE_SUN3 */

#ifndef AOUT_MACHTYPE
#define AOUT_MACHTYPE 0x2
#endif

#define tc_crawl_symbol_chain(a)	{;}	/* not used */
#define tc_headers_hook(a)		{;}	/* not used */
#define tc_aout_pre_write_hook(x)	{;}	/* not used */

#define LISTING_WORD_SIZE 2	/* A word is 2 bytes */
#define LISTING_LHS_WIDTH 2	/* One word on the first line */
#define LISTING_LHS_WIDTH_SECOND 2	/* One word on the second line */
#define LISTING_LHS_CONT_LINES 4/* And 4 lines max */
#define LISTING_HEADER "68K GAS "

/* Copied from write.c */
#define M68K_AIM_KLUDGE(aim, this_state,this_type) \
    if (aim==0 && this_state== 4) { /* hard encoded from tc-m68k.c */ \
					aim=this_type->rlx_forward+1; /* Force relaxation into word mode */ \
				    }

#ifdef M68KCOFF
#define DOT_LABEL_PREFIX
#define REGISTER_PREFIX '%'
#else
#define OPTIONAL_REGISTER_PREFIX '%'
#endif

#ifdef BFD_ASSEMBLER
#define tc_frob_symbol(sym,punt) \
    if (S_GET_SEGMENT (sym) == reg_section) punt = 1
#endif

#define DIFF_EXPR_OK

/* end of tc-m68k.h */
