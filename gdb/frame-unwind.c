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

static struct gdbarch_data *frame_unwind_data;

struct frame_unwind_table
{
  frame_unwind_p_ftype **p;
  int middle;
  int nr;
};

/* Append a predicate to the end of the table.  */
static void
append_predicate (struct frame_unwind_table *table, frame_unwind_p_ftype *p)
{
  table->p = xrealloc (table->p, ((table->nr + 1)
				  * sizeof (frame_unwind_p_ftype *)));
  table->p[table->nr] = p;
  table->nr++;
}

static void *
frame_unwind_init (struct gdbarch *gdbarch)
{
  struct frame_unwind_table *table = XCALLOC (1, struct frame_unwind_table);
  append_predicate (table, dummy_frame_p);
  return table;
}

static void
frame_unwind_free (struct gdbarch *gdbarch, void *data)
{
  struct frame_unwind_table *table =
    gdbarch_data (gdbarch, frame_unwind_data);
  xfree (table->p);
  xfree (table);
}

void
frame_unwind_append_predicate (struct gdbarch *gdbarch,
			       frame_unwind_p_ftype *p)
{
  struct frame_unwind_table *table =
    gdbarch_data (gdbarch, frame_unwind_data);
  if (table == NULL)
    {
      /* ULGH, called during architecture initialization.  Patch
         things up.  */
      table = frame_unwind_init (gdbarch);
      set_gdbarch_data (gdbarch, frame_unwind_data, table);
    }
  append_predicate (table, p);
}

const struct frame_unwind *
frame_unwind_find_by_pc (struct gdbarch *gdbarch, CORE_ADDR pc)
{
  int i;
  struct frame_unwind_table *table =
    gdbarch_data (gdbarch, frame_unwind_data);
  /* Seriously old code.  Don't even try to use this new mechanism.  */
  if (!DEPRECATED_USE_GENERIC_DUMMY_FRAMES)
    return trad_frame_unwind;
  for (i = 0; i < table->nr; i++)
    {
      const struct frame_unwind *desc = table->p[i] (pc);
      if (desc != NULL)
	return desc;
    }
  return trad_frame_unwind;
}

void
_initialize_frame_unwind (void)
{
  frame_unwind_data = register_gdbarch_data (frame_unwind_init,
					     frame_unwind_free);
}
