/* Macro definitions for i386 running under BSD Unix.
   Copyright 1986, 1987, 1989, 1991, 1992, 1993 Free Software Foundation, Inc.

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

#ifndef TM_I386LYNX_H
#define TM_I386LYNX_H

/* Override number of expected traps from sysv. */
#define START_INFERIOR_TRAPS_EXPECTED 2

/* Most definitions from sysv could be used. */
#include "i386/tm-i386v.h"

#undef SAVED_PC_AFTER_CALL

#define SAVED_PC_AFTER_CALL i386lynx_saved_pc_after_call
CORE_ADDR i386lynx_saved_pc_after_call ();

/* Lynx needs a special definition of this so that we can
   print out the pid and thread number seperatly.  */

#undef target_pid_to_str

#define target_pid_to_str(PID) \
	i386lynx_pid_to_str (PID)
extern char *i386lynx_pid_to_str PARAMS ((int pid));

#endif /* TM_I386LYNX_H */
