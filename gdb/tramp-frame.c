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
#include "trad-frame.h"
#include "frame-unwind.h"
#include "gdbcore.h"
#include "symtab.h"
#include "objfiles.h"
#include "target.h"

struct trad_frame_data
{
  const struct tramp_frame *tramp_frame;
};

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

static void
tramp_frame_init (const struct trad_frame *self,
		  struct frame_info *next_frame,
		  struct trad_frame_cache *this_cache)
{
  CORE_ADDR pc = frame_pc_unwind (next_frame);
  const struct tramp_frame *tramp = self->trad_data->tramp_frame;
  tramp->init (tramp, next_frame, this_cache, tramp_frame_start (pc, tramp));
}

static int
tramp_frame_sniffer (const struct trad_frame *self,
		     struct frame_info *next_frame)
{
  const struct tramp_frame *tramp = self->trad_data->tramp_frame;
  CORE_ADDR pc = frame_pc_unwind (next_frame);
  char *name;
  /* If the function has a valid symbol name, it isn't a
     trampoline.  */
  find_pc_partial_function (pc, &name, NULL, NULL);
  if (name != NULL)
    return NULL;
  /* If the function lives in a valid section (even without a starting
     point) it isn't a trampoline.  */
  if (find_pc_section (pc) != NULL)
    return NULL;
  /* The problem here is that this code, and tramp_frame_cache, both
     end up doing a search to find the function start :-(.  */
  return (tramp_frame_start (pc, tramp) != 0);
}

void
tramp_frame_append (struct gdbarch *gdbarch,
		    const struct tramp_frame *tramp)
{
  struct trad_frame_data *trad_data;
  struct trad_frame *trad;

  trad_data = GDBARCH_OBSTACK_ZALLOC (gdbarch, struct trad_frame_data);
  trad = GDBARCH_OBSTACK_ZALLOC (gdbarch, struct trad_frame);

  trad_data->tramp_frame = tramp;
  trad->type = SIGTRAMP_FRAME;
  trad->trad_data = trad_data;
  trad->sniffer = tramp_frame_sniffer;
  trad->init = tramp_frame_init;
  trad_frame_append (gdbarch, trad);
}
