/* Common Linux native ptrace code for AArch64 MTE.

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

#include "gdbsupport/common-defs.h"
#include "gdbsupport/byte-vector.h"

#include "arch/aarch64.h"
#include "arch/aarch64-linux.h"
#include "nat/aarch64-linux.h"
#include "nat/aarch64-mte-linux-ptrace.h"

#include "linux-ptrace.h"
#include <sys/uio.h>

/* Helper to prepare a vector of tags to be passed on to the kernel.  The
   main purpose of this function is to optimize the number of calls to
   ptrace if we're writing too many tags at once, like a pattern fill
   request.

   Return a vector of tags of up to MAX_SIZE size, containing the tags that
   must be passed on to the kernel.  GRANULES is the number of tag granules to
   be modified.  */

static gdb::byte_vector
prepare_tag_vector (size_t granules, const gdb::byte_vector &tags,
		    size_t max_size)
{
  gdb::byte_vector t (0);

  if (granules == 0)
    return t;

  gdb_assert (tags.size () > 0 && max_size > 0);

  if (granules > TAGS_MAX_SIZE)
    t.resize (TAGS_MAX_SIZE);
  else
    t.resize (granules);

  size_t tag_count = tags.size ();

  for (size_t i = 0; i < t.size (); i++)
    t[i] = tags[i % tag_count];

  return t;
}

/* See nat/aarch64-mte-linux-ptrace.h */

int
aarch64_mte_fetch_memtags (int tid, CORE_ADDR address, size_t len,
			   gdb::byte_vector &tags)
{
  size_t ntags = get_tag_granules (address, len, MTE_GRANULE_SIZE);
  gdb_byte tagbuf[ntags];

  struct iovec iovec;
  iovec.iov_base = tagbuf;
  iovec.iov_len = ntags;

  /* Attempt to read ntags allocation tags from the kernel.  */
  if (ptrace (PTRACE_PEEKMTETAGS, tid, address, &iovec) < 0)
    {
      if (errno == EIO)
	warning ("PTRACE_PEEKMTETAGS not supported");
      else if (errno == EFAULT)
	warning ("Couldn't not fetch allocation tags");

      tags.clear ();
      return 1;
    }

  /* Make sure the kernel returned at least one tag.  */
  if (iovec.iov_len <= 0)
    {
      warning (_("Kernel returned no tags."));
      return 1;
    }

  /* In case the kernel has returned less tags than we requested, adjust
     the number of tags and warn.  */
  if (iovec.iov_len != ntags)
    {
      warning (_("Kernel returned less tags than requested."));
      ntags = iovec.iov_len;
    }

  tags.resize (ntags);
  memcpy (tags.data (), tagbuf, ntags);

  return 0;
}

/* See nat/aarch64-mte-linux-ptrace.h */

int
aarch64_mte_store_memtags (int tid, CORE_ADDR address, size_t len,
			   const gdb::byte_vector &tags)
{
  if (tags.size () == 0)
    return 1;

  /* Get the number of tags we need to write.  */
  size_t ntags = get_tag_granules (address, len, MTE_GRANULE_SIZE);

  gdb::byte_vector t = prepare_tag_vector (ntags, tags, TAGS_MAX_SIZE);

  /* Write all the tags, TAGS_MAX_SIZE blocks at a time.  */
  while (t.size () != 0)
    {
      struct iovec iovec;
      iovec.iov_base = t.data ();
      iovec.iov_len = t.size ();

      /* Request the kernel to update NTAGS allocation tags.  */
      if (ptrace (PTRACE_POKEMTETAGS, tid, address, &iovec) < 0)
	{
	  if (errno == EIO)
	    warning ("PTRACE_PEEKMTETAGS not supported");
	  else if (errno == EFAULT)
	    warning ("Couldn't not store allocation tags");

	  return -1;
	}

      /* Update the tag block.  */
      address += t.size () * MTE_GRANULE_SIZE;
      ntags -= t.size ();
      t = prepare_tag_vector (ntags, tags, TAGS_MAX_SIZE);
    }

  return 0;
}
