/* Definitions to make GDB run on a mips box under 4.3bsd.

   Copyright 1986, 1987, 1989, 1991, 1992, 1993, 1994, 1995, 1996,
   1997, 1998, 1999, 2000, 2002, 2003 Free Software Foundation, Inc.

   Contributed by Per Bothner (bothner@cs.wisc.edu) at U.Wisconsin
   and by Alessandro Forin (af@cs.cmu.edu) at CMU..

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

#ifndef TM_MIPS_H
#define TM_MIPS_H 1

#include "regcache.h"

struct frame_info;
struct symbol;
struct type;
struct value;

/* Special symbol found in blocks associated with routines.  We can
   hang mdebug_extra_func_info's off of this.  */

#define MDEBUG_EFI_SYMBOL_NAME "__GDB_EFI_INFO__"

#endif /* TM_MIPS_H */
