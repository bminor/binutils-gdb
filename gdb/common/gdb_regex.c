/* Shared utility routines for GDB to interact with agent.

   Copyright (C) 2009-2015 Free Software Foundation, Inc.

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

#include "common-defs.h"
#include "gdb_regex.h"

/* A cleanup function that calls regfree.  */

static void
do_regfree_cleanup (void *r)
{
  regfree (r);
}

/* Create a new cleanup that frees the compiled regular expression R.  */

struct cleanup *
make_regfree_cleanup (regex_t *r)
{
  return make_cleanup (do_regfree_cleanup, r);
}

/* Return an xmalloc'd error message resulting from a regular
   expression compilation failure.  */

char *
get_regcomp_error (int code, regex_t *rx)
{
  size_t length = regerror (code, rx, NULL, 0);
  char *result = xmalloc (length);

  regerror (code, rx, result, length);
  return result;
}

/* Compile a regexp and throw an exception on error.  This returns a
   cleanup to free the resulting pattern on success.  RX must not be
   NULL.  */

struct cleanup *
compile_rx_or_error (regex_t *pattern, const char *rx, const char *message)
{
  int code;

  gdb_assert (rx != NULL);

  code = regcomp (pattern, rx, REG_NOSUB);
  if (code != 0)
    {
      char *err = get_regcomp_error (code, pattern);

      make_cleanup (xfree, err);
      error (("%s: %s"), message, err);
    }

  return make_regfree_cleanup (pattern);
}
