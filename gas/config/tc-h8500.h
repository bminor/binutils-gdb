/* This file is tc-h8500.h

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


#define TC_H8500
/* This macro translates between an internal fix and an coff reloc type */
#define TC_COFF_FIX2RTYPE(fixP) tc_coff_fix2rtype(fixP)

#define BFD_ARCH bfd_arch_h8500
#define COFF_MAGIC 0x8500
#define TC_COUNT_RELOC(x) ((x)->fx_addsy||(x)->fx_subsy)
#define IGNORE_NONSTANDARD_ESCAPES

#define TC_RELOC_MANGLE(a,b,c) tc_reloc_mangle(a,b,c)

#define DO_NOT_STRIP 0
#define DO_STRIP 0
#define LISTING_HEADER "Hitachi H8/500 GAS "
#define NEED_FX_R_TYPE 1
#define RELOC_32 1234
#define COFF_FLAGS 1

#define TC_START_LABEL(ch, ptr)  (ch == ':' && start_label(ptr))
#define TC_COFF_SIZEMACHDEP(frag) tc_coff_sizemachdep(frag)

/* end of tc-h8500.h */
