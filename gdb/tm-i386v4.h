/* Macro definitions for GDB on an Intel i386 running SVR4.
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

/* Use the alternate method of determining valid frame chains. */

#define FRAME_CHAIN_VALID_ALTERNATE

/* number of traps that happen between exec'ing the shell 
 * to run an inferior, and when we finally get to 
 * the inferior code.  This is 2 on most implementations.
 */
#define START_INFERIOR_TRAPS_EXPECTED 2

/* Pick up most of what we need from the generic i386 target include file. */

#include "tm-i386v.h"

/* Pick up more stuff from the generic SVR4 host include file. */

#include "tm-sysv4.h"

/* We can't tell how many args there are
   now that the C compiler delays popping them.  */

#undef FRAME_NUM_ARGS
#define FRAME_NUM_ARGS(val,fi) (val = -1)
