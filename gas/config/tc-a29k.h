/* tc-a29k.h -- Assemble for the AMD 29000.
   Copyright (C) 1989, 1990, 1991 Free Software Foundation, Inc.

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

/* $Id$ */

#define TC_A29K

#define tc_headers_hook(a)		; /* not used */
#define tc_headers_hook(a)		; /* not used */
#define tc_crawl_symbol_chain(a)	; /* not used */
#define tc_coff_symbol_emit_hook(a)	; /* not used */

#define AOUT_MACHTYPE 101

/* end of tc-a29k.h */
