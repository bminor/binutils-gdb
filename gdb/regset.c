/* Regset support functions, for GDB.

   Copyright 2004 Free Software Foundation, Inc.

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

#include "defs.h"
#include "regset.h"
#include "gdb_assert.h"


struct regset *
regset_xmalloc (const void *descr,
                supply_regset_ftype *supply_regset,
                collect_regset_ftype *collect_regset)
{
  struct regset *r = (struct regset *) xmalloc (sizeof (*r));

  r->descr = descr;
  r->supply_regset = supply_regset;
  r->collect_regset = collect_regset;

  return r;
}
