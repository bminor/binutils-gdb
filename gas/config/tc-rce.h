/* This file is tc-rce.h

   Copyright (C) 1993-1994 Free Software Foundation, Inc.

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
   the Free Software Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#ifndef	TC_RCE
#define TC_RCE 1

#ifdef BFD_ASSEMBLER

#define TARGET_ARCH	bfd_arch_rce
#define TARGET_BYTES_BIG_ENDIAN 1
#ifdef OBJ_AOUT
#define TARGET_FORMAT	"a.out-rce"
#endif

#else /* ! BFD_ASSEMBLER */

#define BFD_ARCH	bfd_arch_rce
#define NO_RELOC	0
#define RELOC_32	1234

#endif /* ! BFD_ASSEMBLER */

#define AOUT_MACHTYPE	155

/* Don't write out relocs for pcrel stuff*/
#define TC_COUNT_RELOC(x) (((x)->fx_addsy||(x)->fx_subsy)&&(x)->fx_r_type < 22)
#define IGNORE_NONSTANDARD_ESCAPES

#define TC_RELOC_MANGLE(s,a,b,c) tc_reloc_mangle(a,b,c)

#define DO_NOT_STRIP 0
#define DO_STRIP 0
#define LISTING_HEADER "RCE Experimental GAS "
#define NEED_FX_R_TYPE 1
#define TC_COFF_SIZEMACHDEP(frag) tc_coff_sizemachdep(frag)

#define md_operand(x)

extern const struct relax_type md_relax_table[];
#define TC_GENERIC_RELAX_TABLE md_relax_table

#endif
