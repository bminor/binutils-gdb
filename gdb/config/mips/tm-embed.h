/* Copyright 1993, 1997, 1998, 1999, 2000 Free Software Foundation, Inc.

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

#include "mips/tm-mips.h"

/* We need to remove watchpoints when stepping, else we hit them again! */

/* FIXME: cagney/2003-08-29: The macros HAVE_STEPPABLE_WATCHPOINT,
   HAVE_NONSTEPPABLE_WATCHPOINT, and HAVE_CONTINUABLE_WATCHPOINT need
   to all be folded into the target vector.  Since they are being used
   as guards for STOPPED_BY_WATCHPOINT, why not have
   STOPPED_BY_WATCHPOINT return the type of watchpoint that the code
   is sitting on?  */

#define HAVE_NONSTEPPABLE_WATCHPOINT 1
