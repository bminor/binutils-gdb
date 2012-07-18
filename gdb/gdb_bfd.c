/* Definitions for BFD wrappers used by GDB.

   Copyright (C) 2011, 2012
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
#include "hashtab.h"

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

/* An object of this type is stored in each BFD's user data.  */

struct gdb_bfd_data
{
  /* The reference count.  */
  int refc;

  /* The mtime of the BFD at the point the cache entry was made.  */
  time_t mtime;
};

/* A hash table storing all the BFDs maintained in the cache.  */

static htab_t gdb_bfd_cache;

/* The type of an object being looked up in gdb_bfd_cache.  We use
   htab's capability of storing one kind of object (BFD in this case)
   and using a different sort of object for searching.  */

struct gdb_bfd_cache_search
{
  /* The filename.  */
  const char *filename;
  /* The mtime.  */
  time_t mtime;
};

/* A hash function for BFDs.  */

static hashval_t
hash_bfd (const void *b)
{
  const bfd *abfd = b;

  /* It is simplest to just hash the filename.  */
  return htab_hash_string (bfd_get_filename (abfd));
}

/* An equality function for BFDs.  Note that this expects the caller
   to search using struct gdb_bfd_cache_search only, not BFDs.  */

static int
eq_bfd (const void *a, const void *b)
{
  const bfd *abfd = a;
  const struct gdb_bfd_cache_search *s = b;
  struct gdb_bfd_data *gdata = bfd_usrdata (abfd);

  return (gdata->mtime == s->mtime
	  && strcmp (bfd_get_filename (abfd), s->filename) == 0);
}

/* See gdb_bfd.h.  */

struct bfd *
gdb_bfd_open (const char *name, const char *target, int fd)
{
  hashval_t hash;
  void **slot;
  bfd *abfd;
  struct gdb_bfd_cache_search search;
  struct stat st;

  if (gdb_bfd_cache == NULL)
    gdb_bfd_cache = htab_create_alloc (1, hash_bfd, eq_bfd, NULL,
				       xcalloc, xfree);

  if (fd == -1)
    {
      fd = open (name, O_RDONLY | O_BINARY);
      if (fd == -1)
	{
	  bfd_set_error (bfd_error_system_call);
	  return NULL;
	}
    }

  search.filename = name;
  if (fstat (fd, &st) < 0)
    {
      /* Weird situation here.  */
      search.mtime = 0;
    }
  else
    search.mtime = st.st_mtime;

  /* Note that this must compute the same result as hash_bfd.  */
  hash = htab_hash_string (name);
  /* Note that we cannot use htab_find_slot_with_hash here, because
     opening the BFD may fail; and this would violate hashtab
     invariants.  */
  abfd = htab_find_with_hash (gdb_bfd_cache, &search, hash);
  if (abfd != NULL)
    {
      close (fd);
      return gdb_bfd_ref (abfd);
    }

  abfd = bfd_fopen (name, target, FOPEN_RB, fd);
  if (abfd == NULL)
    return NULL;

  slot = htab_find_slot_with_hash (gdb_bfd_cache, &search, hash, INSERT);
  gdb_assert (!*slot);
  *slot = abfd;

  gdb_bfd_stash_filename (abfd);
  return gdb_bfd_ref (abfd);
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
  struct gdb_bfd_data *gdata;

  if (abfd == NULL)
    return NULL;

  gdata = bfd_usrdata (abfd);

  if (gdata != NULL)
    {
      gdata->refc += 1;
      return abfd;
    }

  gdata = bfd_zalloc (abfd, sizeof (struct gdb_bfd_data));
  gdata->refc = 1;
  gdata->mtime = bfd_get_mtime (abfd);
  bfd_usrdata (abfd) = gdata;

  return abfd;
}

/* Unreference and possibly close ABFD.  */

void
gdb_bfd_unref (struct bfd *abfd)
{
  struct gdb_bfd_data *gdata;
  struct gdb_bfd_cache_search search;

  if (abfd == NULL)
    return;

  gdata = bfd_usrdata (abfd);
  gdb_assert (gdata->refc >= 1);

  gdata->refc -= 1;
  if (gdata->refc > 0)
    return;

  search.filename = bfd_get_filename (abfd);

  if (gdb_bfd_cache && search.filename)
    {
      hashval_t hash = htab_hash_string (search.filename);
      void **slot;

      search.mtime = gdata->mtime;
      slot = htab_find_slot_with_hash (gdb_bfd_cache, &search, hash,
				       NO_INSERT);

      if (slot && *slot)
	htab_clear_slot (gdb_bfd_cache, slot);
    }

  bfd_usrdata (abfd) = NULL;  /* Paranoia.  */

  gdb_bfd_close_or_warn (abfd);
}
