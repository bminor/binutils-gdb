/* Traditional frame unwind support, for GDB the GNU Debugger.

   Copyright 2003, 2004 Free Software Foundation, Inc.

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
#include "frame.h"
#include "frame-unwind.h"
#include "trad-frame.h"
#include "regcache.h"
#include "frame-base.h"
#include "gdb_assert.h"

/* A traditional frame is unwound by analysing the function prologue
   and using the information gathered to track registers.  For
   non-optimized frames, the technique is reliable (just need to check
   for all potential instruction sequences).  */

struct trad_frame_cache *
trad_frame_cache_zalloc (struct frame_info *next_frame)
{
  struct gdbarch *gdbarch = get_frame_arch (next_frame);
  const int numregs = NUM_REGS + NUM_PSEUDO_REGS;
  struct trad_frame_cache *trad_cache;
  int regnum;

  trad_cache = FRAME_OBSTACK_ZALLOC (struct trad_frame_cache);
  trad_cache->saved_regs = FRAME_OBSTACK_CALLOC (numregs, struct trad_frame_saved_reg);
  for (regnum = 0; regnum < numregs; regnum++)
    {
      trad_cache->saved_regs[regnum].realreg = regnum;
      trad_cache->saved_regs[regnum].addr = -1;
    }      
  return trad_cache;
}

enum { REG_VALUE = -1, REG_UNKNOWN = -2 };

int
trad_frame_value_p (struct trad_frame_cache *this_cache, int regnum)
{
  return (this_cache->saved_regs[regnum].realreg == REG_VALUE);
}

int
trad_frame_addr_p (struct trad_frame_cache *this_cache, int regnum)
{
  return (this_cache->saved_regs[regnum].realreg >= 0
	  && this_cache->saved_regs[regnum].addr != -1);
}

int
trad_frame_realreg_p (struct trad_frame_cache *this_cache,
		      int regnum)
{
  return (this_cache->saved_regs[regnum].realreg >= 0
	  && this_cache->saved_regs[regnum].addr == -1);
}

void
trad_frame_set_reg_value (struct trad_frame_cache *this_cache,
			  int regnum, LONGEST val)
{
  /* Make the REALREG invalid, indicating that the ADDR contains the
     register's value.  */
  this_cache->saved_regs[regnum].realreg = REG_VALUE;
  this_cache->saved_regs[regnum].addr = val;
}

void
trad_frame_set_addr (struct trad_frame_cache *this_cache,
		     int regnum, CORE_ADDR addr)
{
  /* Make the REALREG invalid, indicating that the ADDR contains the
     register's value.  */
  this_cache->saved_regs[regnum].realreg = regnum;
  this_cache->saved_regs[regnum].addr = addr;
}

void
trad_frame_set_unknown (struct trad_frame_cache *this_cache,
			int regnum)
{
  /* Make the REALREG invalid, indicating that the value is not known.  */
  this_cache->saved_regs[regnum].realreg = REG_UNKNOWN;
  this_cache->saved_regs[regnum].addr = -1;
}

struct frame_data
{
  const struct frame_unwind *frame_unwind;
  const struct frame_base *frame_base;
  const struct trad_frame *trad_frame;
};

void
trad_frame_prev_register (struct trad_frame_cache *trad_cache,
			  struct frame_info *next_frame,
			  int regnum, int *optimizedp,
			  enum lval_type *lvalp, CORE_ADDR *addrp,
			  int *realregp, void *bufferp)
{
  struct gdbarch *gdbarch = get_frame_arch (next_frame);

  gdb_assert (trad_cache != NULL);
  if (trad_frame_addr_p (trad_cache, regnum))
    {
      /* The register was saved in memory.  */
      *optimizedp = 0;
      *lvalp = lval_memory;
      *addrp = trad_cache->saved_regs[regnum].addr;
      *realregp = -1;
      if (bufferp != NULL)
	{
	  /* Read the value in from memory.  */
	  get_frame_memory (next_frame, trad_cache->saved_regs[regnum].addr, bufferp,
			    register_size (gdbarch, regnum));
	}
    }
  else if (trad_frame_realreg_p (trad_cache, regnum))
    {
      /* Ask the next frame to return the value of the register.  */
      frame_register_unwind (next_frame, trad_cache->saved_regs[regnum].realreg,
			     optimizedp, lvalp, addrp, realregp, bufferp);
    }
  else if (trad_frame_value_p (trad_cache, regnum))
    {
      /* The register's value is available.  */
      *optimizedp = 0;
      *lvalp = not_lval;
      *addrp = 0;
      *realregp = -1;
      if (bufferp != NULL)
	store_unsigned_integer (bufferp, register_size (gdbarch, regnum),
				trad_cache->saved_regs[regnum].addr);
    }
  else
    {
      error ("Register %s not available",
	     gdbarch_register_name (gdbarch, regnum));
    }
}

void
trad_frame_this_id (struct trad_frame_cache *trad_cache,
		    struct frame_info *next_frame,
		    struct frame_id *this_id)
{
  gdb_assert (trad_cache != NULL);
  (*this_id) = trad_cache->this_id;
}

static void *
trad_frame_sniffer (const struct frame_unwind *self,
		    struct frame_info *next_frame)
{
  const struct trad_frame *trad_frame = self->unwind_data->trad_frame;
  gdb_assert (trad_frame->sniffer != NULL);
  return trad_frame->sniffer (trad_frame, next_frame);
}

static CORE_ADDR
trad_frame_base (const struct frame_base *self,
		 struct frame_info *next_frame,
		 void **this_cache)
{
  struct trad_frame_cache *trad_cache = (*this_cache);

  gdb_assert (trad_cache != NULL);
  return trad_cache->this_base;
}

static const struct frame_base *
trad_frame_base_sniffer (const struct frame_base_sniffer *self,
			 struct frame_info *next_frame)
{
  const struct trad_frame *trad_frame = self->sniffer_data->trad_frame;
  gdb_assert (trad_frame->sniffer != NULL);
  if (trad_frame->sniffer (trad_frame, next_frame))
    return self->sniffer_data->frame_base;
  else
    return NULL;
}

#if 0
void
trad_frame_append (struct gdbarch *gdbarch,
		   const struct trad_frame *trad_frame)
{
  struct frame_data *data;
  struct frame_unwind *unwind;
  struct frame_base *base;
  struct frame_base_sniffer *base_sniffer;

  data = GDBARCH_OBSTACK_ZALLOC (gdbarch, struct frame_data);
  unwind = GDBARCH_OBSTACK_ZALLOC (gdbarch, struct frame_unwind);

  base = GDBARCH_OBSTACK_ZALLOC (gdbarch, struct frame_base);
  base_sniffer = GDBARCH_OBSTACK_ZALLOC (gdbarch, struct frame_base_sniffer);

  data->frame_unwind = unwind;
  data->frame_base = base; 
  data->trad_frame = trad_frame;

  unwind->type = trad_frame->type;
  unwind->this_id = trad_frame_this_id;
  unwind->prev_register = trad_frame_prev_register;
  unwind->unwind_data = data;
  unwind->sniffer = trad_frame_sniffer;

  frame_unwind_append (gdbarch, unwind);

  base->base_data = data;
  base->unwind = unwind;
  base->this_base = trad_frame_base;
  base->this_locals = trad_frame_base;
  base->this_args = trad_frame_base;

  base_sniffer->sniffer = trad_frame_base_sniffer;
  base_sniffer->sniffer_data = data;

  frame_base_sniffer_append (gdbarch, base_sniffer);
}
#endif
