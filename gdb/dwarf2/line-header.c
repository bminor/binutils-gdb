/* DWARF 2 debugging format support for GDB.

   Copyright (C) 1994-2020 Free Software Foundation, Inc.

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
#include "dwarf2/line-header.h"
#include "dwarf2/read.h"
#include "complaints.h"
#include "filenames.h"

void
line_header::add_include_dir (const char *include_dir)
{
  if (dwarf_line_debug >= 2)
    {
      size_t new_size;
      if (version >= 5)
        new_size = m_include_dirs.size ();
      else
        new_size = m_include_dirs.size () + 1;
      fprintf_unfiltered (gdb_stdlog, "Adding dir %zu: %s\n",
			  new_size, include_dir);
    }
  m_include_dirs.push_back (include_dir);
}

void
line_header::add_file_name (const char *name,
			    dir_index d_index,
			    unsigned int mod_time,
			    unsigned int length)
{
  if (dwarf_line_debug >= 2)
    {
      size_t new_size;
      if (version >= 5)
        new_size = file_names_size ();
      else
        new_size = file_names_size () + 1;
      fprintf_unfiltered (gdb_stdlog, "Adding file %zu: %s\n",
			  new_size, name);
    }
  m_file_names.emplace_back (name, d_index, mod_time, length);
}

gdb::unique_xmalloc_ptr<char>
line_header::file_file_name (int file)
{
  /* Is the file number a valid index into the line header's file name
     table?  Remember that file numbers start with one, not zero.  */
  if (is_valid_file_index (file))
    {
      const file_entry *fe = file_name_at (file);

      if (!IS_ABSOLUTE_PATH (fe->name))
	{
	  const char *dir = fe->include_dir (this);
	  if (dir != NULL)
	    return gdb::unique_xmalloc_ptr<char> (concat (dir, SLASH_STRING,
							  fe->name,
							  (char *) NULL));
	}
      return make_unique_xstrdup (fe->name);
    }
  else
    {
      /* The compiler produced a bogus file number.  We can at least
         record the macro definitions made in the file, even if we
         won't be able to find the file by name.  */
      char fake_name[80];

      xsnprintf (fake_name, sizeof (fake_name),
		 "<bad macro file number %d>", file);

      complaint (_("bad file number in macro information (%d)"),
                 file);

      return make_unique_xstrdup (fake_name);
    }
}

gdb::unique_xmalloc_ptr<char>
line_header::file_full_name (int file, const char *comp_dir)
{
  /* Is the file number a valid index into the line header's file name
     table?  Remember that file numbers start with one, not zero.  */
  if (is_valid_file_index (file))
    {
      gdb::unique_xmalloc_ptr<char> relative = file_file_name (file);

      if (IS_ABSOLUTE_PATH (relative.get ()) || comp_dir == NULL)
	return relative;
      return gdb::unique_xmalloc_ptr<char> (concat (comp_dir, SLASH_STRING,
						    relative.get (),
						    (char *) NULL));
    }
  else
    return file_file_name (file);
}
