/* Definitions for frame unwinder, for GDB, the GNU debugger.

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
#include "frame.h"
#include "frame-unwind.h"
#include "gdb_assert.h"
#include "dummy-frame.h"
#include "gdb_obstack.h"

static struct gdbarch_data *frame_unwind_data;

struct frame_unwind_table_entry
{
  const struct frame_unwind_sniffer *sniffer;
  struct frame_unwind_table_entry *next;
};

struct frame_unwind_table
{
  struct frame_unwind_table_entry *first;
};

static void *
frame_unwind_init (struct obstack *obstack)
{
  struct frame_unwind_table *table;
  struct frame_unwind_sniffer *dummy_sniffer;

  dummy_sniffer = OBSTACK_ZALLOC (obstack, struct frame_unwind_sniffer);
  dummy_sniffer->sniffer = dummy_frame_sniffer;

  table = OBSTACK_ZALLOC (obstack, struct frame_unwind_table);
  table->first = OBSTACK_ZALLOC (obstack, struct frame_unwind_table_entry);
  table->first->sniffer = dummy_sniffer;

  return table;
}

/* Append a predicate to the end of the table.  */

void
frame_unwind_sniffer_append (struct gdbarch *gdbarch,
			     const struct frame_unwind_sniffer *sniffer)
{
  struct frame_unwind_table *table = gdbarch_data (gdbarch, frame_unwind_data);
  struct frame_unwind_table_entry **entry;
  for (entry = &table->first; (*entry) != NULL; entry = &(*entry)->next);
  (*entry) = GDBARCH_OBSTACK_ZALLOC (gdbarch, struct frame_unwind_table_entry);
  (*entry)->sniffer = sniffer;
}

void
frame_unwind_append_sniffer (struct gdbarch *gdbarch,
			     frame_unwind_sniffer_ftype *sniffer)
{
  struct frame_unwind_sniffer *unwind_sniffer;

  unwind_sniffer = GDBARCH_OBSTACK_ZALLOC (gdbarch, struct frame_unwind_sniffer);
  unwind_sniffer->sniffer = sniffer;
  frame_unwind_sniffer_append (gdbarch, unwind_sniffer);
}

const struct frame_unwind *
frame_unwind_find_by_frame (struct frame_info *next_frame)
{
  int i;
  struct gdbarch *gdbarch = get_frame_arch (next_frame);
  struct frame_unwind_table *table = gdbarch_data (gdbarch, frame_unwind_data);
  struct frame_unwind_table_entry *entry;
  if (!DEPRECATED_USE_GENERIC_DUMMY_FRAMES && legacy_frame_p (gdbarch))
    /* Seriously old code.  Don't even try to use this new mechanism.
       (Note: The variable USE_GENERIC_DUMMY_FRAMES is deprecated, not
       the dummy frame mechanism.  All architectures should be using
       generic dummy frames).  */
    return legacy_saved_regs_unwind;
  for (entry = table->first; entry != NULL; entry = entry->next)
    {
      const struct frame_unwind *desc;
      gdb_assert (entry->sniffer->sniffer != NULL);
      desc = entry->sniffer->sniffer (entry->sniffer, next_frame);
      if (desc != NULL)
	return desc;
    }
  return legacy_saved_regs_unwind;
}

extern initialize_file_ftype _initialize_frame_unwind; /* -Wmissing-prototypes */

void
_initialize_frame_unwind (void)
{
  frame_unwind_data = register_gdbarch_data (frame_unwind_init, NULL);
}
