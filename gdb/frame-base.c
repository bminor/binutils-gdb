/* Definitions for frame address handler, for GDB, the GNU debugger.

   Copyright 2003 Free Software Foundation, Inc.

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
#include "frame-base.h"
#include "frame.h"
#include "gdb_obstack.h"

/* A default frame base implementations.  If it wasn't for the old
   DEPRECATED_FRAME_LOCALS_ADDRESS and DEPRECATED_FRAME_ARGS_ADDRESS,
   these could be combined into a single function.  All architectures
   really need to override this.  */

static CORE_ADDR
default_frame_base_address (const struct frame_base *self,
			    struct frame_info *next_frame, void **this_cache)
{
  struct frame_info *this_frame = get_prev_frame (next_frame);
  return get_frame_base (this_frame); /* sigh! */
}

static CORE_ADDR
default_frame_locals_address (const struct frame_base *self,
			      struct frame_info *next_frame, void **this_cache)
{
  if (DEPRECATED_FRAME_LOCALS_ADDRESS_P ())
    {
      /* This is bad.  The computation of per-frame locals address
	 should use a per-frame frame-base.  */
      struct frame_info *this_frame = get_prev_frame (next_frame);
      return DEPRECATED_FRAME_LOCALS_ADDRESS (this_frame);
    }
  return default_frame_base_address (self, next_frame, this_cache);
}

static CORE_ADDR
default_frame_args_address (const struct frame_base *self,
			    struct frame_info *next_frame, void **this_cache)
{
  if (DEPRECATED_FRAME_ARGS_ADDRESS_P ())
    {
      struct frame_info *this_frame = get_prev_frame (next_frame);
      return DEPRECATED_FRAME_ARGS_ADDRESS (this_frame);
    }
  return default_frame_base_address (self, next_frame, this_cache);
}

static const struct frame_base default_frame_base = {
  NULL, /* No parent.  */
  default_frame_base_address,
  default_frame_locals_address,
  default_frame_args_address
};

static struct gdbarch_data *frame_base_data;

struct frame_base_table_entry
{
  const struct frame_base_sniffer *sniffer;
  struct frame_base_table_entry *next;
};

struct frame_base_table
{
  struct frame_base_table_entry *first;
  const struct frame_base *default_base;
};

static void *
frame_base_init (struct obstack *obstack)
{
  return OBSTACK_ZALLOC (obstack, struct frame_base_table);
}

/* Append a predicate to the end of the table.  */
static void
append_predicate (struct gdbarch *gdbarch,
		  const struct frame_base_sniffer *sniffer)
{
  struct frame_base_table *table = gdbarch_data (gdbarch, frame_base_data);
  struct frame_base_table_entry **entry;

  for (entry = &table->first; (*entry) != NULL; entry = &(*entry)->next);
  (*entry) = GDBARCH_OBSTACK_ZALLOC (gdbarch, struct frame_base_table_entry);
  (*entry)->sniffer = sniffer;
}

void
frame_base_append_sniffer (struct gdbarch *gdbarch,
			   frame_base_sniffer_ftype *sniffer)
{
  struct frame_base_sniffer *base_sniffer;

  base_sniffer = GDBARCH_OBSTACK_ZALLOC (gdbarch, struct frame_base_sniffer);
  base_sniffer->sniffer = sniffer;
  append_predicate (gdbarch, base_sniffer);
}

void
frame_base_sniffer_append (struct gdbarch *gdbarch,
			   const struct frame_base_sniffer *sniffer)
{
  append_predicate (gdbarch, sniffer);
}

void
frame_base_set_default (struct gdbarch *gdbarch,
			const struct frame_base *default_base)
{
  struct frame_base_table *table = gdbarch_data (gdbarch, frame_base_data);
  table->default_base = default_base;
}

const struct frame_base *
frame_base_find_by_frame (struct frame_info *next_frame)
{
  struct gdbarch *gdbarch = get_frame_arch (next_frame);
  struct frame_base_table *table = gdbarch_data (gdbarch, frame_base_data);
  struct frame_base_table_entry *entry;

  for (entry = table->first; entry != NULL; entry = entry->next)
    {
      const struct frame_base *desc;

      desc = entry->sniffer->sniffer (entry->sniffer, next_frame);
      if (desc != NULL)
	return desc;
    }
  return table->default_base;
}

extern initialize_file_ftype _initialize_frame_base; /* -Wmissing-prototypes */

void
_initialize_frame_base (void)
{
  frame_base_data = gdbarch_data_register_pre_init (frame_base_init);
}
