/* Native support for i386 running Solaris 2.
   Copyright 1998, 1999, 2000, 2007, 2008, 2009, 2010
   Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#ifdef NEW_PROC_API	/* Solaris 6 and above can do HW watchpoints */

/* Solaris x86 2.6 and 2.7 targets have a kernel bug when stepping
   over an instruction that causes a page fault without triggering
   a hardware watchpoint. The kernel properly notices that it shouldn't
   stop, because the hardware watchpoint is not triggered, but it forgets
   the step request and continues the program normally.
   Work around the problem by removing hardware watchpoints if a step is
   requested, GDB will check for a hardware watchpoint trigger after the
   step anyway.  */
#define CANNOT_STEP_HW_WATCHPOINTS

#endif /* NEW_PROC_API */
