/* This file is tc-sh.h

   Copyright (C) 1993 Free Software Foundation, Inc.

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


#define TC_SH

/* This macro translates between an internal fix and an coff reloc type */
#define TC_COFF_FIX2RTYPE(fixP) tc_coff_fix2rtype(fixP)

#define BFD_ARCH bfd_arch_sh
#define COFF_MAGIC 0x0500
/* Write out relocs which haven't been done by gas */
#define TC_COUNT_RELOC(x) (((x)->fx_addsy))
#define IGNORE_NONSTANDARD_ESCAPES

#define TC_RELOC_MANGLE(a,b,c) tc_reloc_mangle(a,b,c)

#define tc_coff_symbol_emit_hook(a) ; /* not used */

#define DO_NOT_STRIP 0
#define DO_STRIP 0
#define LISTING_HEADER "Hitachi Super-H GAS "
#define NEED_FX_R_TYPE 1
#define RELOC_32 1234
#define COFF_FLAGS 1

#define TC_COFF_SIZEMACHDEP(frag) tc_coff_sizemachdep(frag)

/* end of tc-sh.h */
