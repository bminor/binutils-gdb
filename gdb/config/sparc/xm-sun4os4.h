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

#include "sparc/xm-sparc.h"
#define FPU

/* SunOS 4.x has memory mapped files.  */

#define HAVE_MMAP

/* If you expect to use the mmalloc package to obtain mapped symbol files,
   for now you have to specify some parameters that determine how gdb places
   the mappings in it's address space.  See the comments in map_to_address()
   for details.  This is expected to only be a short term solution.  Yes it
   is a kludge.
   FIXME:  Make this more automatic. */

#define MMAP_BASE_ADDRESS	0xE0000000	/* First mapping here */
#define MMAP_INCREMENT		0x01000000	/* Increment to next mapping */

/* /usr/include/malloc.h is included by vx-share/xdr_ld, and might
   declare these using char * not void *.  The following should work with
   acc, gcc, or /bin/cc.  */

#define MALLOC_INCOMPATIBLE
#include <malloc.h>

/* acc for SunOS4 comes with string.h and memory.h headers which we
   pick up somewhere (where?) and which use char *, not void *.  The
   following should work with acc, gcc, or /bin/cc, at least with
   SunOS 4.1.1.  */

#define MEM_FNS_DECLARED
#include <memory.h>

/* SunOS 4.x uses nonstandard "char *" as type of third argument to ptrace() */

#define PTRACE_ARG3_TYPE char*

/* Using termios is required to save and restore ICRNL and ONLCR
   separately.  */

/* At least SunOS 4.1.1 has termios.  I'm not sure about 4.0.3.  */
#define HAVE_TERMIOS
