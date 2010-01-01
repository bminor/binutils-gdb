/* Copyright (C) 2008, 2009, 2010 Free Software Foundation, Inc.

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
#include "windows-tdep.h"
#include "gdb_obstack.h"
#include "xml-support.h"

void
windows_xfer_shared_library (const char* so_name, CORE_ADDR load_addr,
			     struct gdbarch *gdbarch, struct obstack *obstack)
{
  char *p;
  obstack_grow_str (obstack, "<library name=\"");
  p = xml_escape_text (so_name);
  obstack_grow_str (obstack, p);
  xfree (p);
  obstack_grow_str (obstack, "\"><segment address=\"");
  /* The symbols in a dll are offset by 0x1000, which is the the
     offset from 0 of the first byte in an image - because of the file
     header and the section alignment. */
  obstack_grow_str (obstack, paddress (gdbarch, load_addr + 0x1000));
  obstack_grow_str (obstack, "\"/></library>");
}
