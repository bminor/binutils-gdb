/* debuginfod utilities for GDB.
   Copyright (C) 2020 Free Software Foundation, Inc.

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
#include <errno.h>
#include "cli/cli-style.h"
#include "gdbsupport/scoped_fd.h"
#include "debuginfod-support.h"

#ifndef HAVE_LIBDEBUGINFOD
scoped_fd
debuginfod_source_query (const unsigned char *build_id,
			 int build_id_len,
			 const char *srcpath,
			 gdb::unique_xmalloc_ptr<char> *destname)
{
  return scoped_fd (-ENOSYS);
}

scoped_fd
debuginfod_debuginfo_query (const unsigned char *build_id,
			    int build_id_len,
			    const char *filename,
			    gdb::unique_xmalloc_ptr<char> *destname)
{
  return scoped_fd (-ENOSYS);
}
#else
#include <elfutils/debuginfod.h>

/* TODO: Use debuginfod API extensions instead of these globals.  */
static std::string desc;
static std::string fname;
static bool has_printed;

static int
progressfn (debuginfod_client *c, long cur, long total)
{
  if (check_quit_flag ())
    {
      printf_filtered ("Cancelling download of %s %ps...\n",
		       desc.c_str (),
		       styled_string (file_name_style.style (), fname.c_str ()));
      return 1;
    }

  if (!has_printed && total != 0)
    {
      /* Print this message only once.  */
      has_printed = true;
      printf_filtered ("Downloading %s %ps...\n",
		       desc.c_str (),
		       styled_string (file_name_style.style (), fname.c_str ()));
    }

  return 0;
}

static debuginfod_client *
debuginfod_init ()
{
  debuginfod_client *c = debuginfod_begin ();

  if (c != nullptr)
    debuginfod_set_progressfn (c, progressfn);

  return c;
}

/* See debuginfod-support.h  */

scoped_fd
debuginfod_source_query (const unsigned char *build_id,
			 int build_id_len,
			 const char *srcpath,
			 gdb::unique_xmalloc_ptr<char> *destname)
{
  if (getenv (DEBUGINFOD_URLS_ENV_VAR) == NULL)
    return scoped_fd (-ENOSYS);

  debuginfod_client *c = debuginfod_init ();

  if (c == nullptr)
    return scoped_fd (-ENOMEM);

  desc = std::string ("source file");
  fname = std::string (srcpath);
  has_printed = false;

  scoped_fd fd (debuginfod_find_source (c,
					build_id,
					build_id_len,
					srcpath,
					nullptr));

  /* TODO: Add 'set debug debuginfod' command to control when error messages are shown.  */
  if (fd.get () < 0 && fd.get () != -ENOENT)
    printf_filtered (_("Download failed: %s.  Continuing without source file %ps.\n"),
		     safe_strerror (-fd.get ()),
		     styled_string (file_name_style.style (),  srcpath));
  else
    destname->reset (xstrdup (srcpath));

  debuginfod_end (c);
  return fd;
}

/* See debuginfod-support.h  */

scoped_fd
debuginfod_debuginfo_query (const unsigned char *build_id,
			    int build_id_len,
			    const char *filename,
			    gdb::unique_xmalloc_ptr<char> *destname)
{
  if (getenv (DEBUGINFOD_URLS_ENV_VAR) == NULL)
    return scoped_fd (-ENOSYS);

  debuginfod_client *c = debuginfod_init ();

  if (c == nullptr)
    return scoped_fd (-ENOMEM);

  desc = std::string ("separate debug info for");
  fname = std::string (filename);
  has_printed = false;
  char *dname = nullptr;

  scoped_fd fd (debuginfod_find_debuginfo (c, build_id, build_id_len, &dname));

  if (fd.get () < 0 && fd.get () != -ENOENT)
    printf_filtered (_("Download failed: %s.  Continuing without debug info for %ps.\n"),
		     safe_strerror (-fd.get ()),
		     styled_string (file_name_style.style (),  filename));

  destname->reset (dname);
  debuginfod_end (c);
  return fd;
}
#endif
