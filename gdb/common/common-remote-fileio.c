/* Remote File-I/O communications

   Copyright (C) 2003-2015 Free Software Foundation, Inc.

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
#include "common-remote-fileio.h"
#include <sys/stat.h>

/* See common-remote-fileio.h.  */

int
remote_fileio_to_fio_error (int error)
{
  switch (error)
    {
      case EPERM:
        return FILEIO_EPERM;
      case ENOENT:
        return FILEIO_ENOENT;
      case EINTR:
        return FILEIO_EINTR;
      case EIO:
        return FILEIO_EIO;
      case EBADF:
        return FILEIO_EBADF;
      case EACCES:
        return FILEIO_EACCES;
      case EFAULT:
        return FILEIO_EFAULT;
      case EBUSY:
        return FILEIO_EBUSY;
      case EEXIST:
        return FILEIO_EEXIST;
      case ENODEV:
        return FILEIO_ENODEV;
      case ENOTDIR:
        return FILEIO_ENOTDIR;
      case EISDIR:
        return FILEIO_EISDIR;
      case EINVAL:
        return FILEIO_EINVAL;
      case ENFILE:
        return FILEIO_ENFILE;
      case EMFILE:
        return FILEIO_EMFILE;
      case EFBIG:
        return FILEIO_EFBIG;
      case ENOSPC:
        return FILEIO_ENOSPC;
      case ESPIPE:
        return FILEIO_ESPIPE;
      case EROFS:
        return FILEIO_EROFS;
      case ENOSYS:
        return FILEIO_ENOSYS;
      case ENAMETOOLONG:
        return FILEIO_ENAMETOOLONG;
    }
  return FILEIO_EUNKNOWN;
}

/* Convert a host-format mode_t into a bitmask of File-I/O flags.  */

static LONGEST
remote_fileio_mode_to_target (mode_t mode)
{
  mode_t tmode = 0;

  if (S_ISREG (mode))
    tmode |= FILEIO_S_IFREG;
  if (S_ISDIR (mode))
    tmode |= FILEIO_S_IFDIR;
  if (S_ISCHR (mode))
    tmode |= FILEIO_S_IFCHR;
  if (mode & S_IRUSR)
    tmode |= FILEIO_S_IRUSR;
  if (mode & S_IWUSR)
    tmode |= FILEIO_S_IWUSR;
  if (mode & S_IXUSR)
    tmode |= FILEIO_S_IXUSR;
#ifdef S_IRGRP
  if (mode & S_IRGRP)
    tmode |= FILEIO_S_IRGRP;
#endif
#ifdef S_IWRGRP
  if (mode & S_IWGRP)
    tmode |= FILEIO_S_IWGRP;
#endif
#ifdef S_IXGRP
  if (mode & S_IXGRP)
    tmode |= FILEIO_S_IXGRP;
#endif
  if (mode & S_IROTH)
    tmode |= FILEIO_S_IROTH;
#ifdef S_IWOTH
  if (mode & S_IWOTH)
    tmode |= FILEIO_S_IWOTH;
#endif
#ifdef S_IXOTH
  if (mode & S_IXOTH)
    tmode |= FILEIO_S_IXOTH;
#endif
  return tmode;
}

/* Pack a host-format mode_t into an fio_mode_t.  */

static void
remote_fileio_to_fio_mode (mode_t num, fio_mode_t fnum)
{
  remote_fileio_to_be (remote_fileio_mode_to_target (num),
		       (char *) fnum, 4);
}

/* Pack a host-format integer into an fio_ulong_t.  */

static void
remote_fileio_to_fio_ulong (LONGEST num, fio_ulong_t fnum)
{
  remote_fileio_to_be (num, (char *) fnum, 8);
}

/* See common-remote-fileio.h.  */

void
remote_fileio_to_fio_stat (struct stat *st, struct fio_stat *fst)
{
  LONGEST blksize;

  remote_fileio_to_fio_uint ((long) st->st_dev, fst->fst_dev);
  remote_fileio_to_fio_uint ((long) st->st_ino, fst->fst_ino);
  remote_fileio_to_fio_mode (st->st_mode, fst->fst_mode);
  remote_fileio_to_fio_uint ((long) st->st_nlink, fst->fst_nlink);
  remote_fileio_to_fio_uint ((long) st->st_uid, fst->fst_uid);
  remote_fileio_to_fio_uint ((long) st->st_gid, fst->fst_gid);
  remote_fileio_to_fio_uint ((long) st->st_rdev, fst->fst_rdev);
  remote_fileio_to_fio_ulong ((LONGEST) st->st_size, fst->fst_size);
#ifdef HAVE_STRUCT_STAT_ST_BLKSIZE
  blksize = st->st_blksize;
#else
  blksize = 512;
#endif
  remote_fileio_to_fio_ulong (blksize, fst->fst_blksize);
#if HAVE_STRUCT_STAT_ST_BLOCKS
  remote_fileio_to_fio_ulong ((LONGEST) st->st_blocks, fst->fst_blocks);
#else
  /* FIXME: This is correct for DJGPP, but other systems that don't
     have st_blocks, if any, might prefer 512 instead of st_blksize.
     (eliz, 30-12-2003)  */
  remote_fileio_to_fio_ulong (((LONGEST) st->st_size + blksize - 1)
			      / blksize,
			      fst->fst_blocks);
#endif
  remote_fileio_to_fio_time (st->st_atime, fst->fst_atime);
  remote_fileio_to_fio_time (st->st_mtime, fst->fst_mtime);
  remote_fileio_to_fio_time (st->st_ctime, fst->fst_ctime);
}
