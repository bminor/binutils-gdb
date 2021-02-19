/* rename.c -- rename a file, preserving symlinks.
   Copyright (C) 1999-2021 Free Software Foundation, Inc.

   This file is part of GNU Binutils.

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
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA
   02110-1301, USA.  */

#include "sysdep.h"
#include "bfd.h"
#include "bucomm.h"

#ifdef HAVE_GOOD_UTIME_H
#include <utime.h>
#else /* ! HAVE_GOOD_UTIME_H */
#ifdef HAVE_UTIMES
#include <sys/time.h>
#endif /* HAVE_UTIMES */
#endif /* ! HAVE_GOOD_UTIME_H */

#if ! defined (_WIN32) || defined (__CYGWIN32__)
static int simple_copy (const char *, const char *);

/* The number of bytes to copy at once.  */
#define COPY_BUF 8192

/* Copy file FROM to file TO, performing no translations.
   Return 0 if ok, -1 if error.  */

static int
simple_copy (const char *from, const char *to)
{
  int fromfd, tofd, nread;
  int saved;
  char buf[COPY_BUF];

  fromfd = open (from, O_RDONLY | O_BINARY);
  if (fromfd < 0)
    return -1;
#ifdef O_CREAT
  tofd = open (to, O_CREAT | O_WRONLY | O_TRUNC | O_BINARY, 0777);
#else
  tofd = creat (to, 0777);
#endif
  if (tofd < 0)
    {
      saved = errno;
      close (fromfd);
      errno = saved;
      return -1;
    }
  while ((nread = read (fromfd, buf, sizeof buf)) > 0)
    {
      if (write (tofd, buf, nread) != nread)
	{
	  saved = errno;
	  close (fromfd);
	  close (tofd);
	  errno = saved;
	  return -1;
	}
    }
  saved = errno;
  close (fromfd);
  close (tofd);
  if (nread < 0)
    {
      errno = saved;
      return -1;
    }
  return 0;
}
#endif /* __CYGWIN32__ or not _WIN32 */

/* Set the times of the file DESTINATION to be the same as those in
   STATBUF.  */

void
set_times (const char *destination, const struct stat *statbuf)
{
  int result;

  {
#ifdef HAVE_GOOD_UTIME_H
    struct utimbuf tb;

    tb.actime = statbuf->st_atime;
    tb.modtime = statbuf->st_mtime;
    result = utime (destination, &tb);
#else /* ! HAVE_GOOD_UTIME_H */
#ifndef HAVE_UTIMES
    long tb[2];

    tb[0] = statbuf->st_atime;
    tb[1] = statbuf->st_mtime;
    result = utime (destination, tb);
#else /* HAVE_UTIMES */
    struct timeval tv[2];

    tv[0].tv_sec = statbuf->st_atime;
    tv[0].tv_usec = 0;
    tv[1].tv_sec = statbuf->st_mtime;
    tv[1].tv_usec = 0;
    result = utimes (destination, tv);
#endif /* HAVE_UTIMES */
#endif /* ! HAVE_GOOD_UTIME_H */
  }

  if (result != 0)
    non_fatal (_("%s: cannot set time: %s"), destination, strerror (errno));
}

/* Rename FROM to TO, copying if TO exists.  TARGET_STAT has the file status
   that, if non-NULL, is used to fix up timestamps after rename.  Return 0 if
   ok, -1 if error.  */

int
smart_rename (const char *from, const char *to,
	      struct stat *target_stat ATTRIBUTE_UNUSED)
{
  int ret = 0;
  struct stat to_stat;
  bfd_boolean exists;

  exists = lstat (to, &to_stat) == 0;

#if defined (_WIN32) && !defined (__CYGWIN32__)
  /* Win32, unlike unix, will not erase `to' in `rename(from, to)' but
     fail instead.  Also, chown is not present.  */

  if (exists)
    remove (to);

  ret = rename (from, to);
  if (ret != 0)
    {
      /* We have to clean up here.  */
      non_fatal (_("unable to rename '%s'; reason: %s"), to, strerror (errno));
      unlink (from);
    }
#else
  /* Avoid a full copy and use rename if TO does not exist.  */
  if (!exists)
    {
      if ((ret = rename (from, to)) != 0)
	{
	  /* We have to clean up here.  */
	  non_fatal (_("unable to rename '%s'; reason: %s"), to, strerror (errno));
	  unlink (from);
	}
    }
  else
    {
      ret = simple_copy (from, to);
      if (ret != 0)
	non_fatal (_("unable to copy file '%s'; reason: %s"), to, strerror (errno));

      if (target_stat != NULL)
	set_times (to, target_stat);
      unlink (from);
    }
#endif /* _WIN32 && !__CYGWIN32__ */

  return ret;
}
