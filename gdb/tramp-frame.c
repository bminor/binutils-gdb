/* Signal trampoline unwinder, for GDB the GNU Debugger.

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
#include "tramp-frame.h"
#include "frame-unwind.h"
#include "gdbcore.h"
#include "symtab.h"
#include "objfiles.h"
#include "target.h"
#include "trad-frame.h"

struct frame_data
{
  const struct tramp_frame *tramp_frame;
};

struct tramp_frame_cache
{
  CORE_ADDR func;
  struct trad_frame_cache *trad_cache;
};

static struct trad_frame_cache *
tramp_frame_cache (const struct frame_unwind *self,
		   struct frame_info *next_frame,
		   void **this_cache)
{
  CORE_ADDR pc = frame_pc_unwind (next_frame);
  struct tramp_frame_cache *tramp_cache = (*this_cache);
  if (tramp_cache->trad_cache == NULL)
    {
      tramp_cache->trad_cache = trad_frame_cache_zalloc (next_frame);
      self->unwind_data->tramp_frame->init (self->unwind_data->tramp_frame,
					    next_frame,
					    tramp_cache->trad_cache,
					    tramp_cache->func);
    }
  return tramp_cache->trad_cache;
}

static void
tramp_frame_this_id (const struct frame_unwind *self,
		     struct frame_info *next_frame,
		     void **this_cache,
		     struct frame_id *this_id)
{
  struct trad_frame_cache *trad_cache
    = tramp_frame_cache (self, next_frame, this_cache);
  trad_frame_this_id (trad_cache, next_frame, this_id);
}

static void
tramp_frame_prev_register (const struct frame_unwind *self,
			   struct frame_info *next_frame,
			   void **this_cache,
			   int prev_regnum,
			   int *optimizedp,
			   enum lval_type * lvalp,
			   CORE_ADDR *addrp,
			   int *realnump, void *valuep)
{
  struct trad_frame_cache *trad_cache
    = tramp_frame_cache (self, next_frame, this_cache);
  trad_frame_prev_register (trad_cache, next_frame, prev_regnum, optimizedp,
			    lvalp, addrp, realnump, valuep);
}

static CORE_ADDR
tramp_frame_start (CORE_ADDR pc, const struct tramp_frame *tramp)
{
  int ti;
  /* Search through the trampoline for one that matches the
     instruction sequence around PC.  */
  for (ti = 0; tramp->insn[ti] != 0; ti++)
    {
      CORE_ADDR func = pc - tramp->insn_size * ti;
      int i;
      for (i = 0; 1; i++)
	{
	  bfd_byte buf[sizeof (LONGEST)];
	  CORE_ADDR insn;
	  if (tramp->insn[i] == 0)
	    return func;
	  if (target_read_memory (func + i * tramp->insn_size, buf,
				  tramp->insn_size) != 0)
	    break;
	  insn = extract_unsigned_integer (buf, tramp->insn_size);
	  if (tramp->insn[i] != insn)
	    break;
	}
    }
  /* Trampoline doesn't match.  */
  return 0;
}

static void *
tramp_frame_sniffer (const struct frame_unwind *self,
		     struct frame_info *next_frame)
{
  const struct tramp_frame *tramp = self->unwind_data->tramp_frame;
  CORE_ADDR pc = frame_pc_unwind (next_frame);
  CORE_ADDR func;
  char *name;
  struct tramp_frame_cache *tramp_cache;

  /* If the function has a valid symbol name, it isn't a
     trampoline.  */
  find_pc_partial_function (pc, &name, NULL, NULL);
  if (name != NULL)
    return NULL;
  /* If the function lives in a valid section (even without a starting
     point) it isn't a trampoline.  */
  if (find_pc_section (pc) != NULL)
    return NULL;
  /* Finally, check that the trampoline matches at PC.  */
  func = tramp_frame_start (pc, tramp);
  if (func == 0)
    return NULL;
  tramp_cache = FRAME_OBSTACK_ZALLOC (struct tramp_frame_cache);
  tramp_cache->func = func;
  return tramp_cache;
}

void
tramp_frame_append (struct gdbarch *gdbarch,
		    const struct tramp_frame *tramp_frame)
{
  struct frame_data *data;
  struct frame_unwind *unwinder;

  data = GDBARCH_OBSTACK_ZALLOC (gdbarch, struct frame_data);
  unwinder = GDBARCH_OBSTACK_ZALLOC (gdbarch, struct frame_unwind);

  data->tramp_frame = tramp_frame;
  unwinder->type = SIGTRAMP_FRAME;
  unwinder->unwind_data = data;
  unwinder->sniffer = tramp_frame_sniffer;
  unwinder->this_id = tramp_frame_this_id;
  unwinder->prev_register = tramp_frame_prev_register;

  frame_unwind_append (gdbarch, unwinder);
}
