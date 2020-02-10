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
#include "debuginfod-support.h"

#ifndef HAVE_LIBDEBUGINFOD
int
debuginfod_source_query (const unsigned char *build_id __attribute__((unused)),
                         int build_id_len __attribute__((unused)),
                         const char *srcpath __attribute__((unused)),
                         gdb::unique_xmalloc_ptr<char> *filename __attribute__((unused)))
{
  return -ENOSYS;
}

int
debuginfod_debuginfo_query (const unsigned char *build_id __attribute__((unused)),
                            int build_id_len __attribute__((unused)),
                            gdb::unique_xmalloc_ptr<char> *filename __attribute__((unused)))
{
  return -ENOSYS;
}
#else
#include <elfutils/debuginfod.h>

static int
progressfn (debuginfod_client *c,
             long a __attribute__((unused)),
             long b __attribute__((unused)))
{
  int quit_flag = check_quit_flag ();

  /* Avoid swallowing quit_flag's current value.  */
  if (quit_flag)
    set_quit_flag ();

  return quit_flag;
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

int
debuginfod_source_query (const unsigned char *build_id,
                         int build_id_len,
                         const char *srcpath,
                         gdb::unique_xmalloc_ptr<char> *filename)
{
  debuginfod_client *c = debuginfod_init ();

  if (c == nullptr)
    return -ENOMEM;

  char *fname = NULL;
  int fd = debuginfod_find_source (c,
                                   build_id,
                                   build_id_len,
                                   srcpath,
                                   &fname);
  debuginfod_end (c);
  filename->reset (fname);

  return fd;
}

/* See debuginfod-support.h  */

int
debuginfod_debuginfo_query (const unsigned char *build_id,
                            int build_id_len,
                            gdb::unique_xmalloc_ptr<char> *filename)
{
  debuginfod_client *c = debuginfod_init ();

  if (c == nullptr)
    return -ENOMEM;

  char *fname = NULL;
  int fd = debuginfod_find_debuginfo (c, build_id, build_id_len, &fname);
  debuginfod_end (c);
  filename->reset (fname);

  return fd;
}
#endif
