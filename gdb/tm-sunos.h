/* Copyright (C) 1990 Free Software Foundation, Inc.

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

/* This is for SunOS version 4, not for earlier versions.  */

#define CLEAR_SOLIB clear_solib
#define SOLIB_ADD(filename, from_tty) solib_add (filename, from_tty)
#define SOLIB_XFER_MEMORY(memaddr, myaddr, len, write) solib_xfer_memory (memaddr, myaddr, len, write)

/* If we can't set a breakpoint, and it's in a shared library, just
   disable it.  */
#define DISABLE_UNSETTABLE_BREAK(addr) solib_address(addr)
extern int solib_address ();			/* solib.c */
extern int solib_xfer_memory ();
extern void solib_add ();
extern void clear_solib ();
