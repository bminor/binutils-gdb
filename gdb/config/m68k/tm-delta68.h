/* Target definitions for delta68.
   Copyright 1993 Free Software Foundation, Inc.

This file is part of GDB.

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

/* Define BPT_VECTOR if it is different than the default.
   This is the vector number used by traps to indicate a breakpoint. */

#define BPT_VECTOR 0x1

#undef CPLUS_MARKER
#define CPLUS_MARKER	'%'
#define GCC_COMPILED_FLAG_SYMBOL "gcc_compiled%"
#define GCC2_COMPILED_FLAG_SYMBOL "gcc2_compiled%"

/* Amount PC must be decremented by after a breakpoint.
   On the Delta, the kernel decrements it for us.  */

#define DECR_PC_AFTER_BREAK 0

/* Not sure what happens if we try to store this register, but
   phdm@info.ucl.ac.be says we need this define.  */
#define CANNOT_STORE_REGISTER(regno)	(regno == FPI_REGNUM)

#include "m68k/tm-m68k.h"
