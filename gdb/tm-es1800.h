/* Parameters for execution on ES-1800 emulator for 68000.
   The code was originally written by Johan Holmberg TT/SJ Ericsson Telecom
   AB and later modified by Johan Henriksson TT/SJ. It was adapted to GDB 4.0
   by Jan Norden TX/DK.
   Copyright (C) 1986, 1987, 1989, 1991, 1992 Free Software Foundation, Inc.

This file is part of GDB.

GDB is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GDB is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GDB; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#define	GDBINIT_FILENAME	".esgdbinit"

#define	DEFAULT_PROMPT		"(esgdb) "

/* Kludge... */
#include "tm-sun3.h"

#undef SAVED_PC_AFTER_CALL

#define SAVED_PC_AFTER_CALL(frame) \
        read_memory_integer (read_register (SP_REGNUM), 4)
