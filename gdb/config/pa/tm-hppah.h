/* Parameters for execution on an HP PA-RISC machine, running HPUX, for GDB.
   Copyright 1991, 1992 Free Software Foundation, Inc. 

   Contributed by the Center for Software Science at the
   University of Utah (pa-gdb-bugs@cs.utah.edu).

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

/* Mostly it's common to all HPPA's.  */
#include "pa/tm-hppa.h"

/* Saved PC's are different, since there is millicode.  */
extern CORE_ADDR millicode_start, millicode_end;

/* We need to figure out where the text region is so that we use the
   appropriate ptrace operator to manipulate text.  Simply reading/writing
   user space will crap out HPUX.  */

#define NEED_TEXT_START_END
