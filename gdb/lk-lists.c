/* Iterators for internal data structures of the Linux kernel.

   Copyright (C) 2016 Free Software Foundation, Inc.

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

#include "defs.h"

#include "inferior.h"
#include "lk-lists.h"
#include "lk-low.h"

/* Returns next entry from struct list_head CURR while iterating field
   SNAME->FNAME.  */

CORE_ADDR
lk_list_head_next (CORE_ADDR curr, const char *sname, const char *fname)
{
  CORE_ADDR next, next_prev;

  /* We must always assume that the data we handle is corrupted.  Thus use
     curr->next->prev == curr as sanity check.  */
  next = lk_read_addr (curr + LK_OFFSET (list_head, next));
  next_prev = lk_read_addr (next + LK_OFFSET (list_head, prev));

  if (!curr || curr != next_prev)
    {
      error (_("Memory corruption detected while iterating list_head at "\
	       "0x%s belonging to list %s->%s."),
	     phex (curr, lk_builtin_type_size (unsigned_long)) , sname, fname);
    }

  return next;
}
