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
#if HAVE_LIBDEBUGINFOD
#include <elfutils/debuginfod.h>
#endif

#if HAVE_LIBDEBUGINFOD
static int
progressfn (debuginfod_client *c,
             long a __attribute__ ((unused)),
             long b __attribute__ ((unused)))
{
  return check_quit_flag ();
}

static debuginfod_client *
debuginfod_init ()
{
  debuginfod_client *c = debuginfod_begin ();

  if (c != nullptr)
    debuginfod_set_progressfn (c, progressfn);

  return c;
}
#endif

/* See debuginfod-support.h  */

int
debuginfod_source_query (const unsigned char *build_id,
                         int build_id_len,
                         const char *srcpath,
                         char **filename)
{
#if HAVE_LIBDEBUGINFOD
  debuginfod_client *c = debuginfod_init ();

  if (c == nullptr)
    return -ENOMEM;

  int fd = debuginfod_find_source (c,
                                   build_id,
                                   build_id_len,
                                   srcpath,
                                   filename);
  debuginfod_end (c);

  return fd;
#else
  return -ENOSYS;
#endif
}

/* See debuginfod-support.h  */

int
debuginfod_debuginfo_query (const unsigned char *build_id,
                            int build_id_len,
                            char **filename)
{
#if HAVE_LIBDEBUGINFOD
  debuginfod_client *c = debuginfod_init ();

  if (c == nullptr)
    return -ENOMEM;

  int fd = debuginfod_find_debuginfo (c, build_id, build_id_len, filename);
  debuginfod_end (c);

  return fd;
#else
  return -ENOSYS;
#endif
}
