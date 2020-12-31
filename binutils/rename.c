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

#ifndef S_ISLNK
#ifdef S_IFLNK
#define S_ISLNK(m) (((m) & S_IFMT) == S_IFLNK)
#else
#define S_ISLNK(m) 0
#define lstat stat
#endif
#endif

#if !defined (_WIN32) || defined (__CYGWIN32__)
/* Try to preserve the permission bits and ownership of an existing file when
   rename overwrites it.  FD is the file being renamed and TARGET_STAT has the
   status of the file that was overwritten.  */
static void
try_preserve_permissions (int fd, struct stat *target_stat)
{
  struct stat from_stat;
  int ret = 0;

  if (fstat (fd, &from_stat) != 0)
    return;

  int from_mode = from_stat.st_mode & 0777;
  int to_mode = target_stat->st_mode & 0777;

  /* Fix up permissions before we potentially lose ownership with fchown.
     Clear the setxid bits because in case the fchown below fails then we don't
     want to end up with a sxid file owned by the invoking user.  If the user
     hasn't changed or if fchown succeeded, we add back the sxid bits at the
     end.  */
  if (from_mode != to_mode)
    fchmod (fd, to_mode);

  /* Fix up ownership, this will clear the setxid bits.  */
  if (from_stat.st_uid != target_stat->st_uid
      || from_stat.st_gid != target_stat->st_gid)
    ret = fchown (fd, target_stat->st_uid, target_stat->st_gid);

  /* Fix up the sxid bits if either the fchown wasn't needed or it
     succeeded.  */
  if (ret == 0)
    fchmod (fd, target_stat->st_mode & 07777);
}
#endif

/* Rename FROM to TO, copying if TO is either a link or is not a regular file.
   FD is an open file descriptor pointing to FROM that we can use to safely fix
   up permissions of the file after renaming.  TARGET_STAT has the file status
   that is used to fix up permissions and timestamps after rename.  Return 0 if
   ok, -1 if error and FD is closed before returning.  */

int
smart_rename (const char *from, const char *to, int fd ATTRIBUTE_UNUSED,
	      struct stat *target_stat ATTRIBUTE_UNUSED,
	      int preserve_dates ATTRIBUTE_UNUSED)
{
  int ret = 0;
  bfd_boolean exists = target_stat != NULL;

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
  /* Avoid a full copy and use rename if we can fix up permissions of the
     file after renaming, i.e.:

     - TO is not a symbolic link
     - TO is a regular file with only one hard link
     - We have permission to write to TO
     - FD is available to safely fix up permissions to be the same as the file
       we overwrote with the rename.

     Note though that the actual file on disk that TARGET_STAT describes may
     have changed and we're only trying to preserve the status we know about.
     At no point do we try to interact with the new file changes, so there can
     only be two outcomes, i.e. either the external file change survives
     without knowledge of our change (if it happens after the rename syscall)
     or our rename and permissions fixup survive without any knowledge of the
     external change.  */
  if (! exists
      || (fd >= 0
	  && !S_ISLNK (target_stat->st_mode)
	  && S_ISREG (target_stat->st_mode)
	  && (target_stat->st_mode & S_IWUSR)
	  && target_stat->st_nlink == 1)
      )
    {
      ret = rename (from, to);
      if (ret == 0)
	{
	  if (exists)
	    try_preserve_permissions (fd, target_stat);
	}
      else
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

      if (preserve_dates)
	set_times (to, target_stat);
      unlink (from);
    }
  if (fd >= 0)
    close (fd);
#endif /* _WIN32 && !__CYGWIN32__ */

  return ret;
}
