/* Macro definitions for GDB on an Intel i386 running SCO Open Server 5.
   Copyright 1998, 2002 Free Software Foundation, Inc.
   Written by J. Kean Johnston (jkj@sco.com).

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#ifndef TM_I386SCO5_H
#define TM_I386SCO5_H

/* Pick up most of what we need from the i386 SVR4 target include file.  */
#include "i386/tm-i386v4.h"

/* SCO is unlike other SVR3 targets in that it has SVR4 style shared
   libs, with a slight twist.  We expect 3 traps (2 for the exec and
   one for the dynamic loader).  After the third trap we insert the
   shared library breakpoints, then wait for the 4th trap.  */

#undef START_INFERIOR_TRAPS_EXPECTED
#define START_INFERIOR_TRAPS_EXPECTED 3

#endif /* ifndef TM_I386SCO5_H */
