/* Macro definitions for GDB hosted on a Stratus machine.
   Copyright (C) 1992, Free Software Foundation, Inc.
   Contributed by Peggy Fieland (pfieland@stratus.com)

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

/* Pick up most of what we need from the generic i860 host include file. */

#include "xm-i860.h"

/* Pick up more stuff from the generic SVR4 host include file. */

#include "xm-svr4.h"
#define NOVOLVOID 1 /* compiler doesn't like volatile void declaration on functions*/
#define NO_JOB_CONTROL
