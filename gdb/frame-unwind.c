/* Definitions for frame unwinder, for GDB, the GNU debugger.

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
#include "gdb_assert.h"
#include "dummy-frame.h"
#include "gdb_obstack.h"

static struct gdbarch_data *frame_unwind_data;

struct frame_unwind_table_entry
{
  frame_unwind_sniffer_ftype *sniffer;
  const struct frame_unwind *unwinder;
  struct frame_unwind_table_entry *next;
};

struct frame_unwind_table
{
  struct frame_unwind_table_entry *head;
  struct frame_unwind_table_entry **tail;
};

static void *
frame_unwind_init (struct obstack *obstack)
{
  struct frame_unwind_table *table
    = OBSTACK_ZALLOC (obstack, struct frame_unwind_table);
  table->head = OBSTACK_ZALLOC (obstack, struct frame_unwind_table_entry);
  table->head->sniffer = dummy_frame_sniffer;
  table->tail = &table->head->next;
  return table;
}

void
frame_unwind_append_sniffer (struct gdbarch *gdbarch,
			     frame_unwind_sniffer_ftype *sniffer)
{
  struct frame_unwind_table *table = gdbarch_data (gdbarch, frame_unwind_data);
  (*table->tail) = GDBARCH_OBSTACK_ZALLOC (gdbarch, struct frame_unwind_table_entry);
  (*table->tail)->sniffer = sniffer;
  table->tail = &((*table->tail)->next);
}

void
frame_unwind_register_unwinder (struct gdbarch *gdbarch,
				const struct frame_unwind *unwinder)
{
  struct frame_unwind_table *table = gdbarch_data (gdbarch, frame_unwind_data);
  (*table->tail) = GDBARCH_OBSTACK_ZALLOC (gdbarch,
					   struct frame_unwind_table_entry);
  (*table->tail)->unwinder = unwinder;
  table->tail = &((*table->tail)->next);
}

const struct frame_unwind *
frame_unwind_find_by_frame (struct frame_info *next_frame, void **this_cache)
{
  int i;
  struct gdbarch *gdbarch = get_frame_arch (next_frame);
  struct frame_unwind_table *table = gdbarch_data (gdbarch, frame_unwind_data);
  struct frame_unwind_table_entry *entry;
  for (entry = table->head; entry != NULL; entry = entry->next)
    {
      if (entry->sniffer != NULL)
	{
	  const struct frame_unwind *desc = NULL;
	  desc = entry->sniffer (next_frame);
	  if (desc != NULL)
	    return desc;
	}
      if (entry->unwinder != NULL)
	{
	  if (entry->unwinder->sniffer (entry->unwinder, next_frame,
					this_cache))
	    return entry->unwinder;
	}
    }
  return legacy_saved_regs_unwind;
}

extern initialize_file_ftype _initialize_frame_unwind; /* -Wmissing-prototypes */

void
_initialize_frame_unwind (void)
{
  frame_unwind_data = gdbarch_data_register_pre_init (frame_unwind_init);
}
