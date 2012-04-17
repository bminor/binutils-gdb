/* GDB routines for supporting auto-loaded scripts.

   Copyright (C) 2010-2012 Free Software Foundation, Inc.

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
#include "gdb_string.h"
#include "top.h"
#include "exceptions.h"
#include "gdbcmd.h"
#include "objfiles.h"
#include "python.h"
#include "cli/cli-cmds.h"
#include "auto-load.h"

#ifdef HAVE_PYTHON

#include "python-internal.h"

/* NOTE: It's trivial to also support auto-loading normal gdb scripts.
   There has yet to be a need so it's not implemented.  */

/* The suffix of per-objfile scripts to auto-load.
   E.g. When the program loads libfoo.so, look for libfoo-gdb.py.  */
#define GDBPY_AUTO_FILE_NAME "-gdb.py"

/* The section to look for scripts (in file formats that support sections).
   Each entry in this section is a byte of value 1, and then the nul-terminated
   name of the script.  The script name may include a directory.
   The leading byte is to allow upward compatible extensions.  */
#define GDBPY_AUTO_SECTION_NAME ".debug_gdb_scripts"

/* User-settable option to enable/disable auto-loading:
   set auto-load-scripts on|off
   This is true if we should auto-load associated scripts when an objfile
   is opened, false otherwise.
   At the moment, this only affects python scripts, but there's no reason
   one couldn't also have other kinds of auto-loaded scripts, and there's
   no reason to have them each controlled by a separate flag.
   So we elide "python" from the name here and in the option.
   The fact that it lives here is just an implementation detail.  */
static int auto_load_scripts = 1;

/* Load scripts specified in OBJFILE.
   START,END delimit a buffer containing a list of nul-terminated
   file names.
   SOURCE_NAME is used in error messages.

   Scripts are found per normal "source -s" command processing.
   First the script is looked for in $cwd.  If not found there the
   source search path is used.

   The section contains a list of path names of files containing
   python code to load.  Each path is null-terminated.  */

static void
source_section_scripts (struct objfile *objfile, const char *source_name,
			const char *start, const char *end)
{
  const char *p;
  struct auto_load_pspace_info *pspace_info;

  pspace_info = get_auto_load_pspace_data_for_loading (current_program_space);

  for (p = start; p < end; ++p)
    {
      const char *file;
      FILE *stream;
      char *full_path;
      int opened, in_hash_table;
      struct cleanup *back_to;

      if (*p != 1)
	{
	  warning (_("Invalid entry in %s section"), GDBPY_AUTO_SECTION_NAME);
	  /* We could try various heuristics to find the next valid entry,
	     but it's safer to just punt.  */
	  break;
	}
      file = ++p;

      while (p < end && *p != '\0')
	++p;
      if (p == end)
	{
	  char *buf = alloca (p - file + 1);

	  memcpy (buf, file, p - file);
	  buf[p - file] = '\0';
	  warning (_("Non-null-terminated path in %s: %s"),
		   source_name, buf);
	  /* Don't load it.  */
	  break;
	}
      if (p == file)
	{
	  warning (_("Empty path in %s"), source_name);
	  continue;
	}

      opened = find_and_open_script (file, 1 /*search_path*/,
				     &stream, &full_path);

      back_to = make_cleanup (null_cleanup, NULL);
      if (opened)
	{
	  make_cleanup_fclose (stream);
	  make_cleanup (xfree, full_path);
	}

      /* If one script isn't found it's not uncommon for more to not be
	 found either.  We don't want to print an error message for each
	 script, too much noise.  Instead, we print the warning once and tell
	 the user how to find the list of scripts that weren't loaded.

	 IWBN if complaints.c were more general-purpose.  */

      in_hash_table = maybe_add_script (pspace_info, file,
					opened ? full_path : NULL);

      if (! opened)
	{
	  /* We don't throw an error, the program is still debuggable.  */
	  if (script_not_found_warning_print (pspace_info))
	    warning (_("Missing auto-load scripts referenced in section %s\n\
of file %s\n\
Use `info auto-load-scripts [REGEXP]' to list them."),
		     GDBPY_AUTO_SECTION_NAME, objfile->name);
	}
      else
	{
	  /* If this file is not currently loaded, load it.  */
	  if (! in_hash_table)
	    source_python_script_for_objfile (objfile, stream, full_path);
	}

      do_cleanups (back_to);
    }
}

/* Load scripts specified in section SECTION_NAME of OBJFILE.  */

static void
auto_load_section_scripts (struct objfile *objfile, const char *section_name)
{
  bfd *abfd = objfile->obfd;
  asection *scripts_sect;
  bfd_size_type size;
  char *p;
  struct cleanup *cleanups;

  scripts_sect = bfd_get_section_by_name (abfd, section_name);
  if (scripts_sect == NULL)
    return;

  size = bfd_get_section_size (scripts_sect);
  p = xmalloc (size);
  
  cleanups = make_cleanup (xfree, p);

  if (bfd_get_section_contents (abfd, scripts_sect, p, (file_ptr) 0, size))
    source_section_scripts (objfile, section_name, p, p + size);
  else
    warning (_("Couldn't read %s section of %s"),
	     section_name, bfd_get_filename (abfd));

  do_cleanups (cleanups);
}

/* Load any auto-loaded scripts for OBJFILE.  */

void
load_auto_scripts_for_objfile (struct objfile *objfile)
{
  if (auto_load_scripts && gdbpy_global_auto_load)
    {
      auto_load_objfile_script (objfile, GDBPY_AUTO_FILE_NAME);
      auto_load_section_scripts (objfile, GDBPY_AUTO_SECTION_NAME);
    }
}

void
gdbpy_initialize_auto_load (void)
{
  add_setshow_boolean_cmd ("auto-load-scripts", class_support,
			   &auto_load_scripts, _("\
Set the debugger's behaviour regarding auto-loaded scripts."), _("\
Show the debugger's behaviour regarding auto-loaded scripts."), _("\
If enabled, auto-loaded scripts are loaded when the debugger reads\n\
an executable or shared library."),
			   NULL, NULL,
			   &setlist,
			   &showlist);
}

#else /* ! HAVE_PYTHON */

void
load_auto_scripts_for_objfile (struct objfile *objfile)
{
}

#endif /* ! HAVE_PYTHON */
