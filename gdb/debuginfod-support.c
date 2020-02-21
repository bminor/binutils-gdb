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

#include <errno.h>
#include "defs.h"
#include "gdbsupport/scoped_fd.h"
#include "debuginfod-support.h"

#ifndef HAVE_LIBDEBUGINFOD
scoped_fd
debuginfod_source_query (const unsigned char *build_id __attribute__((unused)),
                         int build_id_len __attribute__((unused)),
                         const char *srcpath __attribute__((unused)),
                         gdb::unique_xmalloc_ptr<char> *filename __attribute__((unused)))
{
  return scoped_fd (-ENOSYS);
}

scoped_fd
debuginfod_debuginfo_query (const unsigned char *build_id __attribute__((unused)),
                            int build_id_len __attribute__((unused)),
                            gdb::unique_xmalloc_ptr<char> *filename __attribute__((unused)))
{
  return scoped_fd (-ENOSYS);
}
#else
#include <elfutils/debuginfod.h>

static int
progressfn (debuginfod_client *c, long cur, long total)
{
  if (check_quit_flag ())
    {
      printf_unfiltered ("Cancelling download...\n");
      return 1;
    }

  printf_unfiltered ("Downloading... %.0f%% (%ld/%ld)%s",
                     (cur * 100.0f) / total,
                     cur, total,
                     (cur == total) ? "\n" : "\r");
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
  debuginfod_client *c = debuginfod_init ();

  if (c == nullptr)
    return scoped_fd (-ENOMEM);

  char *dname = nullptr;

  printf_unfiltered ("Attempting to download source file %s\n", srcpath);
  scoped_fd fd (debuginfod_find_source (c,
                                        build_id,
                                        build_id_len,
                                        srcpath,
                                        &dname));

  if (fd.get () < 0)
    printf_unfiltered ("Download unsuccessful. Continuing without source file %s.\n",
                       srcpath);
  else
    printf_unfiltered ("Download successful.\n");


  destname->reset (dname);
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
  debuginfod_client *c = debuginfod_init ();

  if (c == nullptr)
    return scoped_fd (-ENOMEM);

  char *dname = nullptr;

  printf_filtered ("Attempting to download debug info for %s\n", filename);
  scoped_fd fd (debuginfod_find_debuginfo (c, build_id, build_id_len, &dname));

  if (fd.get () < 0)
    printf_unfiltered ("Download unsuccessful. Continuing without debug info for %s.\n",
                       filename);
  else
    printf_unfiltered ("Download successful.\n");

  debuginfod_end (c);
  destname->reset (dname);

  return fd;
}
#endif
