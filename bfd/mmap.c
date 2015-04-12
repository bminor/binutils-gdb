/* BFD library -- mmap of file descriptors.
   Copyright (C) 2016 Free Software Foundation, Inc.
   This file is part of BFD, the Binary File Descriptor library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#include "sysdep.h"

#ifdef HAVE_MMAP
#include "bfd.h"
#include "libbfd.h"
#include "libiberty.h"
#include "bfd_stdint.h"

#include <sys/mman.h>

static file_ptr
mmap_btell (struct bfd *abfd)
{
  return abfd->where;
}

/* Copied and modified from gold_fallocate in gold.  */

static int
mmap_fallocate (int fd, file_ptr offset, file_ptr len)
{
#ifdef HAVE_FALLOCATE
  if (fallocate (fd, 0, offset, len) == 0)
    return 0;
#endif
#ifdef HAVE_POSIX_FALLOCATE
    return posix_fallocate (fd, offset, len);
#endif
  if (ftruncate (fd, offset + len) < 0)
    return errno;
  return 0;
}

static int
mmap_resize (bfd *abfd, file_ptr size)
{
  if (mmap_fallocate (abfd->mmap_fd, 0, size) != 0)
    {
syscall_error:
      bfd_set_error (bfd_error_system_call);
      abfd->mmap_size = 0;
      return -1;
    }
  if (abfd->mmap_size != 0)
    {
#ifdef HAVE_MREMAP
      abfd->iostream = mremap (abfd->iostream, abfd->mmap_size,
			       size, MREMAP_MAYMOVE);
      if (abfd->iostream == MAP_FAILED)
	goto syscall_error;
      else
	goto success;
#else
      if (munmap (abfd->iostream, abfd->mmap_size) != 0)
	goto syscall_error;
#endif
    }
  abfd->iostream = mmap (NULL, size, PROT_READ | PROT_WRITE,
			 MAP_SHARED, abfd->mmap_fd, 0);
  if (abfd->iostream == MAP_FAILED)
    goto syscall_error;

success:
  abfd->mmap_size = size;
  return 0;
}

static int
mmap_bseek (bfd *abfd, file_ptr position, int direction)
{
  file_ptr nwhere;

  if (direction == SEEK_SET)
    nwhere = position;
  else
    nwhere = abfd->where + position;

  if (nwhere < 0)
    {
      abfd->where = 0;
      errno = EINVAL;
      return -1;
    }
  else if (nwhere >= abfd->mmap_size && mmap_resize (abfd, nwhere) != 0)
    return -1;

  return 0;
}

static file_ptr
mmap_bread (struct bfd *abfd, void *buf, file_ptr size)
{
  memcpy (buf, abfd->iostream + abfd->where, size);
  return size;
}

static file_ptr
mmap_bwrite (bfd *abfd, const void *ptr, file_ptr size)
{
  file_ptr filesize = abfd->where + size;

  if (filesize > abfd->mmap_size && mmap_resize (abfd, filesize) != 0)
    {
       bfd_set_error (bfd_error_system_call);
       return 0;
    }

  memcpy (abfd->iostream + abfd->where, ptr, size);
  return size;
}

static int
mmap_bclose (struct bfd *abfd)
{
  int status = munmap (abfd->iostream, abfd->mmap_size);
  if (status == 0)
    status = close (abfd->mmap_fd);
  if (status != 0)
    bfd_set_error (bfd_error_system_call);

  abfd->iostream = MAP_FAILED;
  abfd->mmap_size = 0;
  abfd->mmap_fd = -1;

  return status;
}

static int
mmap_bflush (struct bfd *abfd ATTRIBUTE_UNUSED)
{
#ifdef HAVE_MSYNC
  int status = msync (abfd->iostream, abfd->mmap_size, MS_SYNC);
  if (status != 0)
    bfd_set_error (bfd_error_system_call);
  return status;
#else
  return 0;
#endif
}

static int
mmap_bstat (struct bfd *abfd ATTRIBUTE_UNUSED, struct stat *sb)
{
  int status;
  status = fstat (abfd->mmap_fd, sb);
  if (status < 0)
    bfd_set_error (bfd_error_system_call);
  return status;
}

static void *
mmap_bmmap (struct bfd *abfd ATTRIBUTE_UNUSED,
	    void *addr ATTRIBUTE_UNUSED,
	    bfd_size_type len ATTRIBUTE_UNUSED,
	    int prot ATTRIBUTE_UNUSED,
	    int flags ATTRIBUTE_UNUSED,
	    file_ptr offset ATTRIBUTE_UNUSED,
	    void **map_addr ATTRIBUTE_UNUSED,
	    bfd_size_type *map_len ATTRIBUTE_UNUSED)
{
  /* Unsupported.  */
  abort ();
  return 0;
}

static const struct bfd_iovec mmap_iovec =
{
  &mmap_bread, &mmap_bwrite, &mmap_btell, &mmap_bseek,
  &mmap_bclose, &mmap_bflush, &mmap_bstat, &mmap_bmmap
};
#endif

/*
INTERNAL_FUNCTION
	bfd_mmap_init

SYNOPSIS
	bfd_boolean bfd_mmap_init (bfd *abfd);

DESCRIPTION
	Use mmap on BFD.  Fallback to bfd_cache_init.
*/

bfd_boolean
bfd_mmap_init (bfd *abfd)
{
#ifdef HAVE_MMAP
  /* Only suport write before writing starts.  */
  if (abfd->direction == write_direction
      && !abfd->output_has_begun)
    {
      if (abfd->mmap_size != 0 || abfd->iostream == NULL)
	abort ();

      abfd->mmap_fd = dup (fileno (abfd->iostream));
      if (abfd->mmap_fd < 0)
	{
	  bfd_set_error (bfd_error_system_call);
	  return FALSE;
	}

      abfd->iovec = &mmap_iovec;
      return TRUE;
    }
#endif

  return bfd_cache_init (abfd);
}
