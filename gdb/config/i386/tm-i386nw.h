/* Macro definitions for i386 running NetWare.
   Copyright 1993, 1994 Free Software Foundation, Inc.

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

/* Override number of expected traps from sysv. */
#define START_INFERIOR_TRAPS_EXPECTED 2

/* FIXME:  tm-i386v.h seems to be used in a lot of target config files as a
   generic i386 target config file.  The parts that are truely generic should
   be extracted out and placed in a tm-i386.h file that all the others,
   including tm-i386v.h, includes as needed. */
#include "i386/tm-i386v.h"

/* Stop backtracing when we wander into main.  */

#define FRAME_CHAIN_VALID_ALTERNATE


/* Offsets (in target ints) into jmp_buf.  Not defined in any system header
   file, so we have to step through setjmp/longjmp with a debugger and figure
   them out.  */

#define JB_ELEMENT_SIZE 4	/* jmp_buf[] is array of ints */

#define JB_PC	6		/* Setjmp()'s return PC saved here */

/* Figure out where the longjmp will land.  Slurp the args out of the stack.
   We expect the first arg to be a pointer to the jmp_buf structure from which
   we extract the pc (JB_PC) that we will land at.  The pc is copied into ADDR.
   This routine returns true on success */

extern int
get_longjmp_target PARAMS ((CORE_ADDR *));

#define GET_LONGJMP_TARGET(ADDR) get_longjmp_target(ADDR)
