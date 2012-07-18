/* Definitions for BFD wrappers used by GDB.

   Copyright (C) 2011
   Free Software Foundation, Inc.

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
#include "gdb_bfd.h"
#include "gdb_assert.h"
#include "gdb_string.h"

/* See gdb_bfd.h.  */

void
gdb_bfd_stash_filename (struct bfd *abfd)
{
  char *name = bfd_get_filename (abfd);
  char *data;

  data = bfd_alloc (abfd, strlen (name) + 1);
  strcpy (data, name);

  /* Unwarranted chumminess with BFD.  */
  abfd->filename = data;
}

/* Close ABFD, and warn if that fails.  */

static int
gdb_bfd_close_or_warn (struct bfd *abfd)
{
  int ret;
  char *name = bfd_get_filename (abfd);

  ret = bfd_close (abfd);

  if (!ret)
    warning (_("cannot close \"%s\": %s"),
	     name, bfd_errmsg (bfd_get_error ()));

  return ret;
}

/* Add reference to ABFD.  Returns ABFD.  */

struct bfd *
gdb_bfd_ref (struct bfd *abfd)
{
  int *p_refcount;

  if (abfd == NULL)
    return NULL;

  p_refcount = bfd_usrdata (abfd);

  if (p_refcount != NULL)
    {
      *p_refcount += 1;
      return abfd;
    }

  p_refcount = xmalloc (sizeof (*p_refcount));
  *p_refcount = 1;
  bfd_usrdata (abfd) = p_refcount;

  return abfd;
}

/* Unreference and possibly close ABFD.  */

void
gdb_bfd_unref (struct bfd *abfd)
{
  int *p_refcount;
  char *name;

  if (abfd == NULL)
    return;

  p_refcount = bfd_usrdata (abfd);
  gdb_assert (*p_refcount >= 1);

  *p_refcount -= 1;
  if (*p_refcount > 0)
    return;

  xfree (p_refcount);
  bfd_usrdata (abfd) = NULL;  /* Paranoia.  */

  name = bfd_get_filename (abfd);
  gdb_bfd_close_or_warn (abfd);
}
