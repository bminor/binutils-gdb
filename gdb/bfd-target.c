/* Very simple "bfd" target, for GDB, the GNU debugger.

   Copyright (C) 2003, 2005, 2007, 2008, 2009 Free Software Foundation, Inc.

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
#include "target.h"
#include "bfd-target.h"
#include "exec.h"

static LONGEST
target_bfd_xfer_partial (struct target_ops *ops,
			 enum target_object object,
			 const char *annex, gdb_byte *readbuf,
			 const gdb_byte *writebuf,
			 ULONGEST offset, LONGEST len)
{
  switch (object)
    {
    case TARGET_OBJECT_MEMORY:
      {
	struct target_section_table *table = ops->to_data;
	return section_table_xfer_memory_partial (readbuf, writebuf, offset, len,
						  table->sections,
						  table->sections_end,
						  NULL);
      }
    default:
      return -1;
    }
}

static struct target_section_table *
target_bfd_get_section_table (struct target_ops *ops)
{
  return ops->to_data;
}

static void
target_bfd_xclose (struct target_ops *t, int quitting)
{
  struct target_section_table *table = t->to_data;

  /* If the target sections table is empty, the bfd had already been
     closed.  */
  if (table->sections != table->sections_end)
    bfd_close (table->sections->bfd);
  xfree (table->sections);
  xfree (table);
  xfree (t);
}

struct target_ops *
target_bfd_reopen (struct bfd *bfd)
{
  struct target_ops *t;
  struct target_section_table *table;

  table = XZALLOC (struct target_section_table);
  build_section_table (bfd, &table->sections, &table->sections_end);

  /* No use keeping the bfd open if there are no target sections we
     care about.  This way, we avoid keeping the bfd pointer stored
     somewhere so that target_bfd_xclose could use it.  */
  if (table->sections == table->sections_end)
    bfd_close (bfd);

  t = XZALLOC (struct target_ops);
  t->to_shortname = "bfd";
  t->to_longname = _("BFD backed target");
  t->to_doc = _("You should never see this");
  t->to_get_section_table = target_bfd_get_section_table;
  t->to_xfer_partial = target_bfd_xfer_partial;
  t->to_xclose = target_bfd_xclose;
  t->to_data = table;

  return t;
}
