/* Macro definitions for running gdb on a Sun 4 running sunos 4.
   Copyright (C) 1989, Free Software Foundation, Inc.

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

#include "xm-sparc.h"
#define FPU

/* Large alloca's fail because the attempt to increase the stack limit in
   main() fails because shared libraries are allocated just below the initial
   stack limit.  The SunOS kernel will not allow the stack to grow into
   the area occupied by the shared libraries.  Sun knows about this bug
   but has no obvious fix for it.  */
#define BROKEN_LARGE_ALLOCA

#define HAVE_STRSTR
