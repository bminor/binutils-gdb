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
      return section_table_xfer_memory_partial (readbuf, writebuf, offset, len,
						ops->to_sections,
						ops->to_sections_end);
    default:
      return -1;
    }
}

static void
target_bfd_xclose (struct target_ops *t, int quitting)
{
  bfd_close (t->to_data);
  xfree (t->to_sections);
  xfree (t);
}

struct target_ops *
target_bfd_reopen (struct bfd *bfd)
{
  struct target_ops *t = XZALLOC (struct target_ops);
  t->to_shortname = "bfd";
  t->to_longname = _("BFD backed target");
  t->to_doc = _("You should never see this");
  t->to_xfer_partial = target_bfd_xfer_partial;
  t->to_xclose = target_bfd_xclose;
  t->to_data = bfd;

  build_section_table (bfd,
		       &t->to_sections,
		       &t->to_sections_end);
  return t;
}
