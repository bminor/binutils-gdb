/* Code dealing with register stack frames, for GDB, the GNU debugger.

   Copyright 1986, 1987, 1988, 1989, 1990, 1991, 1992, 1993, 1994,
   1995, 1996, 1997, 1998, 1999, 2000, 2001, 2002 Free Software
   Foundation, Inc.

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
#include "regcache.h"
#include "sentinel-frame.h"
#include "inferior.h"
#include "frame-unwind.h"

struct frame_unwind_cache
{
  struct regcache *regcache;
};

void *
sentinel_frame_cache (struct regcache *regcache)
{
  struct frame_unwind_cache *cache = 
    FRAME_OBSTACK_ZALLOC (struct frame_unwind_cache);
  cache->regcache = regcache;
  return cache;
}

/* Here the register value is taken direct from the register cache.  */

void
sentinel_frame_register_unwind (struct frame_info *next_frame,
				void **this_cache,
				int prev_regnum, int *optimized,
				enum lval_type *lvalp, CORE_ADDR *addrp,
				int *realnum, void *bufferp)
{
  /* Hey don't let on but, for the sentinel frame, next_frame->next ==
     next_frame.  Fortunatly, that local knowledge isn't needed,
     instead THIS_CACHE contains all the information needed to find
     the frame's thread's REGCACHE and that REGCACHE is then accessed
     directly.  */
  struct frame_unwind_cache *cache = *this_cache;

  /* Describe the register's location.  A reg-frame maps all registers
     onto the corresponding hardware register.  */
  *optimized = 0;
  *lvalp = lval_register;
  *addrp = REGISTER_BYTE (prev_regnum);
  *realnum = prev_regnum;

  /* If needed, find and return the value of the register.  */
  if (bufferp != NULL)
    {
      /* Return the actual value.  */
      /* Use the regcache_cooked_read() method so that it, on the fly,
         constructs either a raw or pseudo register from the raw
         register cache.  */
      regcache_cooked_read (cache->regcache, prev_regnum, bufferp);
    }
}

CORE_ADDR
sentinel_frame_pc_unwind (struct frame_info *next_frame,
			  void **this_cache)
{
  /* FIXME: cagney/2003-01-08: This should be using a per-architecture
     method that doesn't suffer from DECR_PC_AFTER_BREAK problems.
     Such a method would take unwind_cache, regcache and stop reason
     parameters.  */
  return read_pc ();
}

void
sentinel_frame_id_unwind (struct frame_info *next_frame,
			  void **this_cache,
			  struct frame_id *this_id)
{
  internal_error (__FILE__, __LINE__, "sentinel_frame_id_unwind called");
}

static void
sentinel_frame_pop (struct frame_info *next_frame,
		    void **this_cache,
		    struct regcache *regcache)
{
  internal_error (__FILE__, __LINE__, "Function sentinal_frame_pop called");
}

const struct frame_unwind sentinel_frame_unwinder =
{
  sentinel_frame_pop,
  sentinel_frame_pc_unwind,
  sentinel_frame_id_unwind,
  sentinel_frame_register_unwind
};

const struct frame_unwind *const sentinel_frame_unwind = &sentinel_frame_unwinder;
