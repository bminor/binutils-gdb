/* ho-rs6000.h  Rs6000 host-specific header file.
   Copyright (C) 1987, 1991, 1992 Free Software Foundation, Inc.

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

/* M_RS6000 is used in aout_gnu.h as an enumerator.
   Nothing tests for the macro being defined, so don't bother defining it.  */
/* #define M_RS6000 1 */

/* The assert.h macros assume that cpp DTRT when substituting for
   a macro argument inside a string, including requoting.  The non-STDC
   cpp on the rs6k botches the requoting.  (Many non-STDC cpps do, but
   the assert.h here relies on it working.)  */
#ifndef __STDC__
#define BROKEN_ASSERT
#endif

/* The RS/6000 native compiler miscompiles an expression in tc-m68k.c.  */
#ifndef __STDC__
#define IBM_COMPILER_SUX
#endif

extern void free ();

/* end of ho-rs6000.h */
