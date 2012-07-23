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
#ifdef HAVE_ZLIB_H
#include <zlib.h>
#endif
#ifdef HAVE_MMAP
#include <sys/mman.h>
#ifndef MAP_FAILED
#define MAP_FAILED ((void *) -1)
#endif
#endif

/* An object of this type is stored in the section's user data when
   mapping a section.  */

struct gdb_bfd_section_data
{
  /* Size of the data.  */
  bfd_size_type size;
  /* If the data was mmapped, this is the length of the map.  */
  bfd_size_type map_len;
  /* The data.  If NULL, the section data has not been read.  */
  void *data;
  /* If the data was mmapped, this is the map address.  */
  void *map_addr;
};

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
      gdb_bfd_ref (abfd);
      return abfd;
    }

  abfd = bfd_fopen (name, target, FOPEN_RB, fd);
  if (abfd == NULL)
    return NULL;

  slot = htab_find_slot_with_hash (gdb_bfd_cache, &search, hash, INSERT);
  gdb_assert (!*slot);
  *slot = abfd;

  gdb_bfd_stash_filename (abfd);
  gdb_bfd_ref (abfd);
  return abfd;
}

/* A helper function that releases any section data attached to the
   BFD.  */

static void
free_one_bfd_section (bfd *abfd, asection *sectp, void *ignore)
{
  struct gdb_bfd_section_data *sect = bfd_get_section_userdata (abfd, sectp);

  if (sect != NULL && sect->data != NULL)
    {
#ifdef HAVE_MMAP
      if (sect->map_addr != NULL)
	{
	  int res;

	  res = munmap (sect->map_addr, sect->map_len);
	  gdb_assert (res == 0);
	}
      else
#endif
	xfree (sect->data);
    }
}

/* Close ABFD, and warn if that fails.  */

static int
gdb_bfd_close_or_warn (struct bfd *abfd)
{
  int ret;
  char *name = bfd_get_filename (abfd);

  bfd_map_over_sections (abfd, free_one_bfd_section, NULL);

  ret = bfd_close (abfd);

  if (!ret)
    warning (_("cannot close \"%s\": %s"),
	     name, bfd_errmsg (bfd_get_error ()));

  return ret;
}

/* Add reference to ABFD.  Returns ABFD.  */

void
gdb_bfd_ref (struct bfd *abfd)
{
  struct gdb_bfd_data *gdata;

  if (abfd == NULL)
    return;

  gdata = bfd_usrdata (abfd);

  if (gdata != NULL)
    {
      gdata->refc += 1;
      return;
    }

  gdata = bfd_zalloc (abfd, sizeof (struct gdb_bfd_data));
  gdata->refc = 1;
  gdata->mtime = bfd_get_mtime (abfd);
  bfd_usrdata (abfd) = gdata;
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

/* A helper function that returns the section data descriptor
   associated with SECTION.  If no such descriptor exists, a new one
   is allocated and cleared.  */

static struct gdb_bfd_section_data *
get_section_descriptor (asection *section)
{
  struct gdb_bfd_section_data *result;

  result = bfd_get_section_userdata (section->owner, section);

  if (result == NULL)
    {
      result = bfd_zalloc (section->owner, sizeof (*result));
      bfd_set_section_userdata (section->owner, section, result);
    }

  return result;
}

/* Decompress a section that was compressed using zlib.  Store the
   decompressed buffer, and its size, in DESCRIPTOR.  */

static void
zlib_decompress_section (asection *sectp,
			 struct gdb_bfd_section_data *descriptor)
{
  bfd *abfd = sectp->owner;
#ifndef HAVE_ZLIB_H
  error (_("Support for zlib-compressed data (from '%s', section '%s') "
           "is disabled in this copy of GDB"),
         bfd_get_filename (abfd),
	 bfd_get_section_name (sectp));
#else
  bfd_size_type compressed_size = bfd_get_section_size (sectp);
  gdb_byte *compressed_buffer = xmalloc (compressed_size);
  struct cleanup *cleanup = make_cleanup (xfree, compressed_buffer);
  struct cleanup *inner_cleanup;
  bfd_size_type uncompressed_size;
  gdb_byte *uncompressed_buffer;
  z_stream strm;
  int rc;
  int header_size = 12;
  struct dwarf2_per_bfd_section *section_data;

  if (bfd_seek (abfd, sectp->filepos, SEEK_SET) != 0
      || bfd_bread (compressed_buffer,
		    compressed_size, abfd) != compressed_size)
    error (_("can't read data from '%s', section '%s'"),
           bfd_get_filename (abfd),
	   bfd_get_section_name (abfd, sectp));

  /* Read the zlib header.  In this case, it should be "ZLIB" followed
     by the uncompressed section size, 8 bytes in big-endian order.  */
  if (compressed_size < header_size
      || strncmp (compressed_buffer, "ZLIB", 4) != 0)
    error (_("corrupt ZLIB header from '%s', section '%s'"),
           bfd_get_filename (abfd),
	   bfd_get_section_name (abfd, sectp));
  uncompressed_size = compressed_buffer[4]; uncompressed_size <<= 8;
  uncompressed_size += compressed_buffer[5]; uncompressed_size <<= 8;
  uncompressed_size += compressed_buffer[6]; uncompressed_size <<= 8;
  uncompressed_size += compressed_buffer[7]; uncompressed_size <<= 8;
  uncompressed_size += compressed_buffer[8]; uncompressed_size <<= 8;
  uncompressed_size += compressed_buffer[9]; uncompressed_size <<= 8;
  uncompressed_size += compressed_buffer[10]; uncompressed_size <<= 8;
  uncompressed_size += compressed_buffer[11];

  /* It is possible the section consists of several compressed
     buffers concatenated together, so we uncompress in a loop.  */
  strm.zalloc = NULL;
  strm.zfree = NULL;
  strm.opaque = NULL;
  strm.avail_in = compressed_size - header_size;
  strm.next_in = (Bytef*) compressed_buffer + header_size;
  strm.avail_out = uncompressed_size;
  uncompressed_buffer = xmalloc (uncompressed_size);
  inner_cleanup = make_cleanup (xfree, uncompressed_buffer);
  rc = inflateInit (&strm);
  while (strm.avail_in > 0)
    {
      if (rc != Z_OK)
        error (_("setting up uncompression in '%s', section '%s': %d"),
               bfd_get_filename (abfd),
	       bfd_get_section_name (abfd, sectp),
	       rc);
      strm.next_out = ((Bytef*) uncompressed_buffer
                       + (uncompressed_size - strm.avail_out));
      rc = inflate (&strm, Z_FINISH);
      if (rc != Z_STREAM_END)
        error (_("zlib error uncompressing from '%s', section '%s': %d"),
               bfd_get_filename (abfd),
	       bfd_get_section_name (abfd, sectp),
	       rc);
      rc = inflateReset (&strm);
    }
  rc = inflateEnd (&strm);
  if (rc != Z_OK
      || strm.avail_out != 0)
    error (_("concluding uncompression in '%s', section '%s': %d"),
           bfd_get_filename (abfd),
	   bfd_get_section_name (abfd, sectp),
	   rc);

  discard_cleanups (inner_cleanup);
  do_cleanups (cleanup);

  /* Attach the data to the BFD section.  */
  descriptor->data = uncompressed_buffer;
  descriptor->size = uncompressed_size;
#endif
}

/* See gdb_bfd.h.  */

const gdb_byte *
gdb_bfd_map_section (asection *sectp, bfd_size_type *size)
{
  bfd *abfd;
  gdb_byte *buf, *retbuf;
  unsigned char header[4];
  struct gdb_bfd_section_data *descriptor;

  gdb_assert ((sectp->flags & SEC_RELOC) == 0);
  gdb_assert (size != NULL);

  abfd = sectp->owner;

  descriptor = get_section_descriptor (sectp);

  /* If the data was already read for this BFD, just reuse it.  */
  if (descriptor->data != NULL)
    goto done;

  /* Check if the file has a 4-byte header indicating compression.  */
  if (bfd_get_section_size (sectp) > sizeof (header)
      && bfd_seek (abfd, sectp->filepos, SEEK_SET) == 0
      && bfd_bread (header, sizeof (header), abfd) == sizeof (header))
    {
      /* Upon decompression, update the buffer and its size.  */
      if (strncmp (header, "ZLIB", sizeof (header)) == 0)
        {
          zlib_decompress_section (sectp, descriptor);
	  goto done;
        }
    }

#ifdef HAVE_MMAP
  {
    /* The page size, used when mmapping.  */
    static int pagesize;

    if (pagesize == 0)
      pagesize = getpagesize ();

    /* Only try to mmap sections which are large enough: we don't want
       to waste space due to fragmentation.  */

    if (bfd_get_section_size (sectp) > 4 * pagesize)
      {
	descriptor->size = bfd_get_section_size (sectp);
	descriptor->data = bfd_mmap (abfd, 0, descriptor->size, PROT_READ,
				     MAP_PRIVATE, sectp->filepos,
				     &descriptor->map_addr,
				     &descriptor->map_len);

	if ((caddr_t)descriptor->data != MAP_FAILED)
	  {
#if HAVE_POSIX_MADVISE
	    posix_madvise (descriptor->map_addr, descriptor->map_len,
			   POSIX_MADV_WILLNEED);
#endif
	    goto done;
	  }

	/* On failure, clear out the section data and try again.  */
	memset (descriptor, 0, sizeof (*descriptor));
      }
  }
#endif /* HAVE_MMAP */

  /* If we get here, we are a normal, not-compressed section.  */

  descriptor->size = bfd_get_section_size (sectp);
  descriptor->data = xmalloc (descriptor->size);

  if (bfd_seek (abfd, sectp->filepos, SEEK_SET) != 0
      || bfd_bread (descriptor->data, bfd_get_section_size (sectp),
		    abfd) != bfd_get_section_size (sectp))
    {
      xfree (descriptor->data);
      descriptor->data = NULL;
      error (_("Can't read data for section '%s'"),
	     bfd_get_filename (abfd));
    }

 done:
  gdb_assert (descriptor->data != NULL);
  *size = descriptor->size;
  return descriptor->data;
}
