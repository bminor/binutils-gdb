/* Macro definitions for GDB on a Commodore Amiga running SVR4 (amix).
   Copyright (C) 1991, Free Software Foundation, Inc.
   Written by Fred Fish at Cygnus Support (fnf@cygint)

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

/* All Amiga's (so far) running UNIX have come standard with the floating
   point coprocessor. */

#define HAVE_68881

/* Define BPT_VECTOR if it is different than the default.
   This is the vector number used by traps to indicate a breakpoint. */

#define BPT_VECTOR 0x1

/* How much to decrement the PC after a trap.  Depends on kernel. */

#define DECR_PC_AFTER_BREAK 0		/* No decrement required */

/* Address of end of stack space.  Actually one byte past it.
   This value is typically very OS dependent.
   FIXME:  Check to see if SVR4 offers some machine independent way
   of discovering this value and use it if so, and if we need it. */

/* #define STACK_END_ADDR 0xc0800000 */

/* Use the alternate method of determining valid frame chains. */

#define FRAME_CHAIN_VALID_ALTERNATE

#include "tm-sysv4.h"
#include "tm-68k.h"
