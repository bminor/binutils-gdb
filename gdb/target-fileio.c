/* Target File-I/O communications

   Copyright (C) 2003, 2005, 2006, 2007, 2008 Free Software Foundation, Inc.

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
#include "gdb_string.h"
#include "gdbcmd.h"
#include "gdb/fileio.h"
#include "gdb_wait.h"
#include "gdb_stat.h"
#include "exceptions.h"
#include "target-fileio.h"

#include <fcntl.h>
#include <sys/time.h>
#ifdef __CYGWIN__
#include <sys/cygwin.h>		/* For cygwin_conv_to_full_posix_path.  */
#endif

static struct {
  int *fd_map;
  int fd_map_size;
} target_fio_data;

#define FIO_FD_INVALID		-1
#define FIO_FD_CONSOLE_IN	-2
#define FIO_FD_CONSOLE_OUT	-3

static int target_fio_system_call_allowed = 0;

static int
target_fileio_init_fd_map (void)
{
  int i;

  if (!target_fio_data.fd_map)
    {
      target_fio_data.fd_map = (int *) xmalloc (10 * sizeof (int));
      target_fio_data.fd_map_size = 10;
      target_fio_data.fd_map[0] = FIO_FD_CONSOLE_IN;
      target_fio_data.fd_map[1] = FIO_FD_CONSOLE_OUT;
      target_fio_data.fd_map[2] = FIO_FD_CONSOLE_OUT;
      for (i = 3; i < 10; ++i)
        target_fio_data.fd_map[i] = FIO_FD_INVALID;
    }
  return 3;
}

static int
target_fileio_resize_fd_map (void)
{
  int i = target_fio_data.fd_map_size;

  if (!target_fio_data.fd_map)
    return target_fileio_init_fd_map ();
  target_fio_data.fd_map_size += 10;
  target_fio_data.fd_map =
    (int *) xrealloc (target_fio_data.fd_map,
		      target_fio_data.fd_map_size * sizeof (int));
  for (; i < target_fio_data.fd_map_size; i++)
    target_fio_data.fd_map[i] = FIO_FD_INVALID;
  return target_fio_data.fd_map_size - 10;
}

static int
target_fileio_next_free_fd (void)
{
  int i;

  for (i = 0; i < target_fio_data.fd_map_size; ++i)
    if (target_fio_data.fd_map[i] == FIO_FD_INVALID)
      return i;
  return target_fileio_resize_fd_map ();
}

static int
target_fileio_fd_to_targetfd (int fd)
{
  int target_fd =target_fileio_next_free_fd ();
  target_fio_data.fd_map[target_fd] = fd;
  return target_fd;
}

static int
target_fileio_map_fd (int target_fd)
{
 target_fileio_init_fd_map ();
  if (target_fd < 0 || target_fd >= target_fio_data.fd_map_size)
    return FIO_FD_INVALID;
  return target_fio_data.fd_map[target_fd];
}

static void
target_fileio_close_target_fd (int target_fd)
{
 target_fileio_init_fd_map ();
  if (target_fd >= 0 && target_fd < target_fio_data.fd_map_size)
    target_fio_data.fd_map[target_fd] = FIO_FD_INVALID;
}

static int
target_fileio_oflags_to_host (long flags)
{
  int hflags = 0;

  if (flags & FILEIO_O_CREAT)
    hflags |= O_CREAT;
  if (flags & FILEIO_O_EXCL)
    hflags |= O_EXCL;
  if (flags & FILEIO_O_TRUNC)
    hflags |= O_TRUNC;
  if (flags & FILEIO_O_APPEND)
    hflags |= O_APPEND;
  if (flags & FILEIO_O_RDONLY)
    hflags |= O_RDONLY;
  if (flags & FILEIO_O_WRONLY)
    hflags |= O_WRONLY;
  if (flags & FILEIO_O_RDWR)
    hflags |= O_RDWR;
/* On systems supporting binary and text mode, always open files in
   binary mode. */
#ifdef O_BINARY
  hflags |= O_BINARY;
#endif
  return hflags;
}

static mode_t
target_fileio_mode_to_host (long mode, int open_call)
{
  mode_t hmode = 0;

  if (!open_call)
    {
      if (mode & FILEIO_S_IFREG)
	hmode |= S_IFREG;
      if (mode & FILEIO_S_IFDIR)
	hmode |= S_IFDIR;
      if (mode & FILEIO_S_IFCHR)
	hmode |= S_IFCHR;
    }
  if (mode & FILEIO_S_IRUSR)
    hmode |= S_IRUSR;
  if (mode & FILEIO_S_IWUSR)
    hmode |= S_IWUSR;
  if (mode & FILEIO_S_IXUSR)
    hmode |= S_IXUSR;
#ifdef S_IRGRP
  if (mode & FILEIO_S_IRGRP)
    hmode |= S_IRGRP;
#endif
#ifdef S_IWGRP
  if (mode & FILEIO_S_IWGRP)
    hmode |= S_IWGRP;
#endif
#ifdef S_IXGRP
  if (mode & FILEIO_S_IXGRP)
    hmode |= S_IXGRP;
#endif
  if (mode & FILEIO_S_IROTH)
    hmode |= S_IROTH;
#ifdef S_IWOTH
  if (mode & FILEIO_S_IWOTH)
    hmode |= S_IWOTH;
#endif
#ifdef S_IXOTH
  if (mode & FILEIO_S_IXOTH)
    hmode |= S_IXOTH;
#endif
  return hmode;
}

static LONGEST
target_fileio_mode_to_target (mode_t mode)
{
  mode_t tmode = 0;

  if (S_ISREG(mode))
    tmode |= FILEIO_S_IFREG;
  if (S_ISDIR(mode))
    tmode |= FILEIO_S_IFDIR;
  if (S_ISCHR(mode))
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

static int
target_fileio_errno_to_target (int error)
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

static int
target_fileio_seek_flag_to_host (long num, int *flag)
{
  if (!flag)
    return 0;
  switch (num)
    {
      case FILEIO_SEEK_SET:
        *flag = SEEK_SET;
	break;
      case FILEIO_SEEK_CUR:
        *flag =  SEEK_CUR;
	break;
      case FILEIO_SEEK_END:
        *flag =  SEEK_END;
	break;
      default:
        return -1;
    }
  return 0;
}

static int
target_fileio_extract_long (char **buf, LONGEST *retlong)
{
  char *c;
  int sign = 1;

  if (!buf || !*buf || !**buf || !retlong)
    return -1;
  c = strchr (*buf, ',');
  if (c)
    *c++ = '\0';
  else
    c = strchr (*buf, '\0');
  while (strchr ("+-", **buf))
    {
      if (**buf == '-')
	sign = -sign;
      ++*buf;
    }
  for (*retlong = 0; **buf; ++*buf)
    {
      *retlong <<= 4;
      if (**buf >= '0' && **buf <= '9')
        *retlong += **buf - '0';
      else if (**buf >= 'a' && **buf <= 'f')
        *retlong += **buf - 'a' + 10;
      else if (**buf >= 'A' && **buf <= 'F')
        *retlong += **buf - 'A' + 10;
      else
        return -1;
    }
  *retlong *= sign;
  *buf = c;
  return 0;
}

static int
target_fileio_extract_int (char **buf, long *retint)
{
  int ret;
  LONGEST retlong;

  if (!retint)
    return -1;
  ret =target_fileio_extract_long (buf, &retlong);
  if (!ret)
    *retint = (long) retlong;
  return ret;
}

static int
target_fileio_extract_ptr_w_len (char **buf, CORE_ADDR *ptrval, int *length)
{
  char *c;
  LONGEST retlong;

  if (!buf || !*buf || !**buf || !ptrval || !length)
    return -1;
  c = strchr (*buf, '/');
  if (!c)
    return -1;
  *c++ = '\0';
  if (target_fileio_extract_long (buf, &retlong))
    return -1;
  *ptrval = (CORE_ADDR) retlong;
  *buf = c;
  if (target_fileio_extract_long (buf, &retlong))
    return -1;
  *length = (int) retlong;
  return 0;
}

/* Convert to big endian */
static void
target_fileio_to_be (LONGEST num, char *buf, int bytes)
{
  int i;

  for (i = 0; i < bytes; ++i)
    buf[i] = (num >> (8 * (bytes - i - 1))) & 0xff;
}

static void
target_fileio_to_fio_uint (long num, fio_uint_t fnum)
{
 target_fileio_to_be ((LONGEST) num, (char *) fnum, 4);
}

static void
target_fileio_to_fio_mode (mode_t num, fio_mode_t fnum)
{
 target_fileio_to_be (target_fileio_mode_to_target(num), (char *) fnum, 4);
}

static void
target_fileio_to_fio_time (time_t num, fio_time_t fnum)
{
 target_fileio_to_be ((LONGEST) num, (char *) fnum, 4);
}

static void
target_fileio_to_fio_long (LONGEST num, fio_long_t fnum)
{
 target_fileio_to_be (num, (char *) fnum, 8);
}

static void
target_fileio_to_fio_ulong (LONGEST num, fio_ulong_t fnum)
{
 target_fileio_to_be (num, (char *) fnum, 8);
}

static void
target_fileio_to_fio_stat (struct stat *st, struct fio_stat *fst)
{
  LONGEST blksize;

  /* `st_dev' is set in the calling function */
 target_fileio_to_fio_uint ((long) st->st_ino, fst->fst_ino);
 target_fileio_to_fio_mode (st->st_mode, fst->fst_mode);
 target_fileio_to_fio_uint ((long) st->st_nlink, fst->fst_nlink);
 target_fileio_to_fio_uint ((long) st->st_uid, fst->fst_uid);
 target_fileio_to_fio_uint ((long) st->st_gid, fst->fst_gid);
 target_fileio_to_fio_uint ((long) st->st_rdev, fst->fst_rdev);
 target_fileio_to_fio_ulong ((LONGEST) st->st_size, fst->fst_size);
#ifdef HAVE_STRUCT_STAT_ST_BLKSIZE
  blksize = st->st_blksize;
#else
  blksize = 512;
#endif
 target_fileio_to_fio_ulong (blksize, fst->fst_blksize);
#if HAVE_STRUCT_STAT_ST_BLOCKS
 target_fileio_to_fio_ulong ((LONGEST) st->st_blocks, fst->fst_blocks);
#else
  /* FIXME: This is correct for DJGPP, but other systems that don't
     have st_blocks, if any, might prefer 512 instead of st_blksize.
     (eliz, 30-12-2003)  */
 target_fileio_to_fio_ulong (((LONGEST) st->st_size + blksize - 1)
			      / blksize,
			      fst->fst_blocks);
#endif
 target_fileio_to_fio_time (st->st_atime, fst->fst_atime);
 target_fileio_to_fio_time (st->st_mtime, fst->fst_mtime);
 target_fileio_to_fio_time (st->st_ctime, fst->fst_ctime);
}

static void
target_fileio_to_fio_timeval (struct timeval *tv, struct fio_timeval *ftv)
{
 target_fileio_to_fio_time (tv->tv_sec, ftv->ftv_sec);
 target_fileio_to_fio_long (tv->tv_usec, ftv->ftv_usec);
}

int target_fio_no_longjmp = 0;


static void
target_fileio_func_open (char *buf, struct file_io_operations *operations)
{
  CORE_ADDR ptrval;
  int length, retlength;
  long num;
  int flags, fd;
  mode_t mode;
  char *pathname;
  struct stat st;

  /* 1. Parameter: Ptr to pathname / length incl. trailing zero */
  if (target_fileio_extract_ptr_w_len (&buf, &ptrval, &length))
    {
      operations->ioerror ();
      return;
    }
  /* 2. Parameter: open flags */
  if (target_fileio_extract_int (&buf, &num))
    {
      operations->ioerror ();
      return;
    }
  flags =target_fileio_oflags_to_host (num);
  /* 3. Parameter: open mode */
  if (target_fileio_extract_int (&buf, &num))
    {
      operations->ioerror ();
      return;
    }
  mode =target_fileio_mode_to_host (num, 1);

  /* Read pathname */
  pathname = alloca (length);
  retlength = operations->read_bytes (ptrval, (gdb_byte *) pathname, length);
  if (retlength != length)
    {
      operations->ioerror ();
      return;
    }

  /* Check if pathname exists and is not a regular file or directory.  If so,
     return an appropriate error code.  Same for trying to open directories
     for writing. */
  if (!stat (pathname, &st))
    {
      if (!S_ISREG (st.st_mode) && !S_ISDIR (st.st_mode))
	{
	  operations->reply (-1, FILEIO_ENODEV);
	  return;
	}
      if (S_ISDIR (st.st_mode)
	  && ((flags & O_WRONLY) == O_WRONLY || (flags & O_RDWR) == O_RDWR))
	{
	  operations->reply (-1, FILEIO_EISDIR);
	  return;
	}
    }

  target_fio_no_longjmp = 1;
  fd = open (pathname, flags, mode);
  if (fd < 0)
    {
      operations->return_errno (-1);
      return;
    }

  fd =target_fileio_fd_to_targetfd (fd);
  operations->return_success (fd);
}

static void
target_fileio_func_close (char *buf, struct file_io_operations *operations)
{
  long num;
  int fd;

  /* Parameter: file descriptor */
  if (target_fileio_extract_int (&buf, &num))
    {
      operations->ioerror ();
      return;
    }
  fd =target_fileio_map_fd ((int) num);
  if (fd == FIO_FD_INVALID)
    {
      operations->badfd ();
      return;
    }

  target_fio_no_longjmp = 1;
  if (fd != FIO_FD_CONSOLE_IN && fd != FIO_FD_CONSOLE_OUT && close (fd))
    operations->return_errno (-1);
 target_fileio_close_target_fd ((int) num);
  operations->return_success (0);
}

static void
target_fileio_func_read (char *buf, struct file_io_operations *operations)
{
  long target_fd, num;
  LONGEST lnum;
  CORE_ADDR ptrval;
  int fd, ret, retlength;
  gdb_byte *buffer;
  size_t length;
  off_t old_offset, new_offset;

  /* 1. Parameter: file descriptor */
  if (target_fileio_extract_int (&buf, &target_fd))
    {
      operations->ioerror ();
      return;
    }
  fd =target_fileio_map_fd ((int) target_fd);
  if (fd == FIO_FD_INVALID)
    {
      operations->badfd ();
      return;
    }
  /* 2. Parameter: buffer pointer */
  if (target_fileio_extract_long (&buf, &lnum))
    {
      operations->ioerror ();
      return;
    }
  ptrval = (CORE_ADDR) lnum;
  /* 3. Parameter: buffer length */
  if (target_fileio_extract_int (&buf, &num))
    {
      operations->ioerror ();
      return;
    }
  length = (size_t) num;

  switch (fd)
    {
      case FIO_FD_CONSOLE_OUT:
	operations->badfd ();
	return;
      case FIO_FD_CONSOLE_IN:
	{
	  static char *remaining_buf = NULL;
	  static int remaining_length = 0;

	  buffer = (gdb_byte *) xmalloc (32768);
	  if (remaining_buf)
	    {
	      target_fio_no_longjmp = 1;
	      if (remaining_length > length)
		{
		  memcpy (buffer, remaining_buf, length);
		  memmove (remaining_buf, remaining_buf + length,
			   remaining_length - length);
		  remaining_length -= length;
		  ret = length;
		}
	      else
		{
		  memcpy (buffer, remaining_buf, remaining_length);
		  xfree (remaining_buf);
		  remaining_buf = NULL;
		  ret = remaining_length;
		}
	    }
	  else
	    {
	      ret = ui_file_read (gdb_stdtargin, (char *) buffer, 32767);
	      target_fio_no_longjmp = 1;
	      if (ret > 0 && (size_t)ret > length)
		{
		  remaining_buf = (char *) xmalloc (ret - length);
		  remaining_length = ret - length;
		  memcpy (remaining_buf, buffer + length, remaining_length);
		  ret = length;
		}
	    }
	}
	break;
      default:
	buffer = (gdb_byte *) xmalloc (length);
	/* POSIX defines EINTR behaviour of read in a weird way.  It's allowed
	   for read() to return -1 even if "some" bytes have been read.  It
	   has been corrected in SUSv2 but that doesn't help us much...
	   Therefore a complete solution must check how many bytes have been
	   read on EINTR to return a more reliable value to the target */
	old_offset = lseek (fd, 0, SEEK_CUR);
	target_fio_no_longjmp = 1;
	ret = read (fd, buffer, length);
	if (ret < 0 && errno == EINTR)
	  {
	    new_offset = lseek (fd, 0, SEEK_CUR);
	    /* If some data has been read, return the number of bytes read.
	       The Ctrl-C flag is set in target_fileio_reply() anyway */
	    if (old_offset != new_offset)
	      ret = new_offset - old_offset;
	  }
	break;
    }

  if (ret > 0)
    {
      retlength = operations->write_bytes (ptrval, buffer, ret);
      if (retlength != ret)
	ret = -1; /* errno has been set to EIO in remote_write_bytes() */
    }

  if (ret < 0)
    operations->return_errno (-1);
  else
    operations->return_success (ret);

  xfree (buffer);
}

static void
target_fileio_func_write (char *buf, struct file_io_operations *operations)
{
  long target_fd, num;
  LONGEST lnum;
  CORE_ADDR ptrval;
  int fd, ret, retlength;
  gdb_byte *buffer;
  size_t length;

  /* 1. Parameter: file descriptor */
  if (target_fileio_extract_int (&buf, &target_fd))
    {
      operations->ioerror ();
      return;
    }
  fd =target_fileio_map_fd ((int) target_fd);
  if (fd == FIO_FD_INVALID)
    {
      operations->badfd ();
      return;
    }
  /* 2. Parameter: buffer pointer */
  if (target_fileio_extract_long (&buf, &lnum))
    {
      operations->ioerror ();
      return;
    }
  ptrval = (CORE_ADDR) lnum;
  /* 3. Parameter: buffer length */
  if (target_fileio_extract_int (&buf, &num))
    {
      operations->ioerror ();
      return;
    }
  length = (size_t) num;
    
  buffer = (gdb_byte *) xmalloc (length);
  retlength = operations->read_bytes (ptrval, buffer, length);
  if (retlength != length)
    {
      xfree (buffer);
      operations->ioerror ();
      return;
    }

  target_fio_no_longjmp = 1;
  switch (fd)
    {
      case FIO_FD_CONSOLE_IN:
	operations->badfd ();
	xfree (buffer);
	return;
      case FIO_FD_CONSOLE_OUT:
	ui_file_write (target_fd == 1 ? gdb_stdtarg : gdb_stdtargerr,
		       (char *) buffer, length);
	gdb_flush (target_fd == 1 ? gdb_stdtarg : gdb_stdtargerr);
	ret = length;
	break;
      default:
	ret = write (fd, buffer, length);
	if (ret < 0 && errno == EACCES)
	  errno = EBADF; /* Cygwin returns EACCESS when writing to a R/O file.*/
	break;
    }

  if (ret < 0)
    operations->return_errno (-1);
  else
    operations->return_success (ret);

  xfree (buffer);
}

static void
target_fileio_func_lseek (char *buf, struct file_io_operations *operations)
{
  long num;
  LONGEST lnum;
  int fd, flag;
  off_t offset, ret;

  /* 1. Parameter: file descriptor */
  if (target_fileio_extract_int (&buf, &num))
    {
      operations->ioerror ();
      return;
    }
  fd =target_fileio_map_fd ((int) num);
  if (fd == FIO_FD_INVALID)
    {
      operations->badfd ();
      return;
    }
  else if (fd == FIO_FD_CONSOLE_IN || fd == FIO_FD_CONSOLE_OUT)
    {
      operations->reply (-1, FILEIO_ESPIPE);
      return;
    }

  /* 2. Parameter: offset */
  if (target_fileio_extract_long (&buf, &lnum))
    {
      operations->ioerror ();
      return;
    }
  offset = (off_t) lnum;
  /* 3. Parameter: flag */
  if (target_fileio_extract_int (&buf, &num))
    {
      operations->ioerror ();
      return;
    }
  if (target_fileio_seek_flag_to_host (num, &flag))
    {
      operations->reply (-1, FILEIO_EINVAL);
      return;
    }
  
  target_fio_no_longjmp = 1;
  ret = lseek (fd, offset, flag);

  if (ret == (off_t) -1)
    operations->return_errno (-1);
  else
    operations->return_success (ret);
}

static void
target_fileio_func_rename (char *buf, struct file_io_operations *operations)
{
  CORE_ADDR old_ptr, new_ptr;
  int old_len, new_len, retlength;
  char *oldpath, *newpath;
  int ret, of, nf;
  struct stat ost, nst;

  /* 1. Parameter: Ptr to oldpath / length incl. trailing zero */
  if (target_fileio_extract_ptr_w_len (&buf, &old_ptr, &old_len))
    {
      operations->ioerror ();
      return;
    }
  
  /* 2. Parameter: Ptr to newpath / length incl. trailing zero */
  if (target_fileio_extract_ptr_w_len (&buf, &new_ptr, &new_len))
    {
      operations->ioerror ();
      return;
    }
  
  /* Request oldpath */
  oldpath = alloca (old_len);
  retlength = operations->read_bytes (old_ptr, (gdb_byte *) oldpath, old_len);
  if (retlength != old_len)
    {
      operations->ioerror ();
      return;
    }
  
  /* Request newpath */
  newpath = alloca (new_len);
  retlength = operations->read_bytes (new_ptr, (gdb_byte *) newpath, new_len);
  if (retlength != new_len)
    {
      operations->ioerror ();
      return;
    }
  
  /* Only operate on regular files and directories */
  of = stat (oldpath, &ost);
  nf = stat (newpath, &nst);
  if ((!of && !S_ISREG (ost.st_mode) && !S_ISDIR (ost.st_mode))
      || (!nf && !S_ISREG (nst.st_mode) && !S_ISDIR (nst.st_mode)))
    {
      operations->reply (-1, FILEIO_EACCES);
      return;
    }

  target_fio_no_longjmp = 1;
  ret = rename (oldpath, newpath);

  if (ret == -1)
    {
      /* Special case: newpath is a non-empty directory.  Some systems
         return ENOTEMPTY, some return EEXIST.  We coerce that to be
	 always EEXIST. */
      if (errno == ENOTEMPTY)
        errno = EEXIST;
#ifdef __CYGWIN__
      /* Workaround some Cygwin problems with correct errnos. */
      if (errno == EACCES)
        {
	  if (!of && !nf && S_ISDIR (nst.st_mode))
	    {
	      if (S_ISREG (ost.st_mode))
		errno = EISDIR;
	      else
		{
		  char oldfullpath[PATH_MAX + 1];
		  char newfullpath[PATH_MAX + 1];
		  int len;

		  cygwin_conv_to_full_posix_path (oldpath, oldfullpath);
		  cygwin_conv_to_full_posix_path (newpath, newfullpath);
		  len = strlen (oldfullpath);
		  if (newfullpath[len] == '/'
		      && !strncmp (oldfullpath, newfullpath, len))
		    errno = EINVAL;
		  else
		    errno = EEXIST;
		}
	    }
	}
#endif

      operations->return_errno (-1);
    }
  else
    operations->return_success (ret);
}

static void
target_fileio_func_unlink (char *buf, struct file_io_operations *operations)
{
  CORE_ADDR ptrval;
  int length, retlength;
  char *pathname;
  int ret;
  struct stat st;

  /* Parameter: Ptr to pathname / length incl. trailing zero */
  if (target_fileio_extract_ptr_w_len (&buf, &ptrval, &length))
    {
      operations->ioerror ();
      return;
    }
  /* Request pathname */
  pathname = alloca (length);
  retlength = operations->read_bytes (ptrval, (gdb_byte *) pathname, length);
  if (retlength != length)
    {
      operations->ioerror ();
      return;
    }

  /* Only operate on regular files (and directories, which allows to return
     the correct return code) */
  if (!stat (pathname, &st) && !S_ISREG (st.st_mode) && !S_ISDIR (st.st_mode))
    {
      operations->reply (-1, FILEIO_ENODEV);
      return;
    }

  target_fio_no_longjmp = 1;
  ret = unlink (pathname);

  if (ret == -1)
    operations->return_errno (-1);
  else
    operations->return_success (ret);
}

static void
target_fileio_func_stat (char *buf, struct file_io_operations *operations)
{
  CORE_ADDR statptr, nameptr;
  int ret, namelength, retlength;
  char *pathname;
  LONGEST lnum;
  struct stat st;
  struct fio_stat fst;

  /* 1. Parameter: Ptr to pathname / length incl. trailing zero */
  if (target_fileio_extract_ptr_w_len (&buf, &nameptr, &namelength))
    {
      operations->ioerror ();
      return;
    }

  /* 2. Parameter: Ptr to struct stat */
  if (target_fileio_extract_long (&buf, &lnum))
    {
      operations->ioerror ();
      return;
    }
  statptr = (CORE_ADDR) lnum;
  
  /* Request pathname */
  pathname = alloca (namelength);
  retlength = operations->read_bytes (nameptr, (gdb_byte *) pathname, namelength);
  if (retlength != namelength)
    {
      operations->ioerror ();
      return;
    }

  target_fio_no_longjmp = 1;
  ret = stat (pathname, &st);

  if (ret == -1)
    {
      operations->return_errno (-1);
      return;
    }
  /* Only operate on regular files and directories */
  if (!ret && !S_ISREG (st.st_mode) && !S_ISDIR (st.st_mode))
    {
      operations->reply (-1, FILEIO_EACCES);
      return;
    }
  if (statptr)
    {
     target_fileio_to_fio_stat (&st, &fst);
     target_fileio_to_fio_uint (0, fst.fst_dev);
      
      retlength = operations->write_bytes (statptr,
					   (gdb_byte *) &fst, sizeof fst);
      if (retlength != sizeof fst)
	{
	  operations->return_errno (-1);
	  return;
	}
    }
  operations->return_success (ret);
}

static void
target_fileio_func_fstat (char *buf, struct file_io_operations *operations)
{
  CORE_ADDR ptrval;
  int fd, ret, retlength;
  long target_fd;
  LONGEST lnum;
  struct stat st;
  struct fio_stat fst;
  struct timeval tv;

  /* 1. Parameter: file descriptor */
  if (target_fileio_extract_int (&buf, &target_fd))
    {
      operations->ioerror ();
      return;
    }
  fd =target_fileio_map_fd ((int) target_fd);
  if (fd == FIO_FD_INVALID)
    {
      operations->badfd ();
      return;
    }
  /* 2. Parameter: Ptr to struct stat */
  if (target_fileio_extract_long (&buf, &lnum))
    {
      operations->ioerror ();
      return;
    }
  ptrval = (CORE_ADDR) lnum;

  // ARC bug fix 25/11/2008
  memset(&st,  0, sizeof(st));
  memset(&fst, 0, sizeof(fst));

  target_fio_no_longjmp = 1;
  if (fd == FIO_FD_CONSOLE_IN || fd == FIO_FD_CONSOLE_OUT)
    {
     target_fileio_to_fio_uint (1, fst.fst_dev);
      st.st_mode = S_IFCHR | (fd == FIO_FD_CONSOLE_IN ? S_IRUSR : S_IWUSR);
      st.st_nlink = 1;
#ifdef HAVE_GETUID
      st.st_uid = getuid ();
#else
      st.st_uid = 0;
#endif
#ifdef HAVE_GETGID
      st.st_gid = getgid ();
#else
      st.st_gid = 0;
#endif
      st.st_rdev = 0;
      st.st_size = 0;
#ifdef HAVE_STRUCT_STAT_ST_BLKSIZE
      st.st_blksize = 512;
#endif
#if HAVE_STRUCT_STAT_ST_BLOCKS
      st.st_blocks = 0;
#endif

      /* N.B. st.st_ino is not set! */

      if (!gettimeofday (&tv, NULL))
	st.st_atime = st.st_mtime = st.st_ctime = tv.tv_sec;
      else
        st.st_atime = st.st_mtime = st.st_ctime = (time_t) 0;
      ret = 0;
    }
  else {
    ret = fstat (fd, &st);
    target_fileio_to_fio_uint (st.st_dev, fst.fst_dev);  // ARC bug fix 10/11/2008  gdb bug: 9655
  }

  if (ret == -1)
    {
      operations->return_errno (-1);
      return;
    }
  if (ptrval)
    {
     target_fileio_to_fio_stat (&st, &fst);

      retlength = operations->write_bytes (ptrval, (gdb_byte *) &fst, sizeof fst);
      if (retlength != sizeof fst)
	{
	  operations->return_errno (-1);
	  return;
	}
    }
  operations->return_success (ret);
}

static void
target_fileio_func_gettimeofday (char *buf, struct file_io_operations *operations)
{
  LONGEST lnum;
  CORE_ADDR ptrval;
  int ret, retlength;
  struct timeval tv;
  struct fio_timeval ftv;

  /* 1. Parameter: struct timeval pointer */
  if (target_fileio_extract_long (&buf, &lnum))
    {
      operations->ioerror ();
      return;
    }
  ptrval = (CORE_ADDR) lnum;
  /* 2. Parameter: some pointer value... */
  if (target_fileio_extract_long (&buf, &lnum))
    {
      operations->ioerror ();
      return;
    }
  /* ...which has to be NULL */
  if (lnum)
    {
      operations->reply (-1, FILEIO_EINVAL);
      return;
    }

  target_fio_no_longjmp = 1;
  ret = gettimeofday (&tv, NULL);

  if (ret == -1)
    {
      operations->return_errno (-1);
      return;
    }

  if (ptrval)
    {
     target_fileio_to_fio_timeval (&tv, &ftv);

      retlength = operations->write_bytes (ptrval, (gdb_byte *) &ftv, sizeof ftv);
      if (retlength != sizeof ftv)
	{
	  operations->return_errno (-1);
	  return;
	}
    }
  operations->return_success (ret);
}

static void
target_fileio_func_isatty (char *buf, struct file_io_operations *operations)
{
  long target_fd;
  int fd;

  /* Parameter: file descriptor */
  if (target_fileio_extract_int (&buf, &target_fd))
    {
      operations->ioerror ();
      return;
    }
  target_fio_no_longjmp = 1;
  fd =target_fileio_map_fd ((int) target_fd);
  operations->return_success (fd == FIO_FD_CONSOLE_IN ||
  				fd == FIO_FD_CONSOLE_OUT ? 1 : 0);
}

static void
target_fileio_func_system (char *buf, struct file_io_operations *operations)
{
  CORE_ADDR ptrval;
  int ret, length, retlength;
  char *cmdline = NULL;

  /* Parameter: Ptr to commandline / length incl. trailing zero */
  if (target_fileio_extract_ptr_w_len (&buf, &ptrval, &length))
    {
      operations->ioerror ();
      return;
    }

  if (length)
    {
      /* Request commandline */
      cmdline = alloca (length);
      retlength = operations->read_bytes (ptrval, (gdb_byte *) cmdline, length);
      if (retlength != length)
	{
	  operations->ioerror ();
	  return;
	}
    }
  
  /* Check if system(3) has been explicitely allowed using the
     `set remote system-call-allowed 1' command.  If length is 0,
     indicating a NULL parameter to the system call, return zero to
     indicate a shell is not available.  Otherwise fail with EPERM.  */
  if (!target_fio_system_call_allowed)
    {
      if (!length)
	operations->return_success (0);
      else
	operations->reply (-1, FILEIO_EPERM);
      return;
    }

  target_fio_no_longjmp = 1;
  ret = system (cmdline);

  if (!length)
    operations->return_success (ret);
  else if (ret == -1)
    operations->return_errno (-1);
  else
    operations->return_success (WEXITSTATUS (ret));
}

static struct {
  char *name;
  void (*func)(char *, struct file_io_operations *);
} target_fio_func_map[] = {
  { "open",         target_fileio_func_open         },
  { "close",        target_fileio_func_close        },
  { "read",         target_fileio_func_read         },
  { "write",        target_fileio_func_write        },
  { "lseek",        target_fileio_func_lseek        },
  { "rename",       target_fileio_func_rename       },
  { "unlink",       target_fileio_func_unlink       },
  { "stat",         target_fileio_func_stat         },
  { "fstat",        target_fileio_func_fstat        },
  { "gettimeofday", target_fileio_func_gettimeofday },
  { "isatty",       target_fileio_func_isatty       },
  { "system",       target_fileio_func_system       },
  { NULL, NULL }
};

struct request_args {
    char                     *buf;
    struct file_io_operations *operations;
};

static int
do_target_fileio_request (struct ui_out *uiout, void *args)
{
  struct request_args* request = (struct request_args*) args;
  char *buf = request->buf;
  char *c;
  int idx;

  request->operations->set_ctrl_c_signal_handler();

  c = strchr (++buf, ',');
  if (c)
    *c++ = '\0';
  else
    c = strchr (buf, '\0');
  for (idx = 0; target_fio_func_map[idx].name; ++idx)
    if (!strcmp (target_fio_func_map[idx].name, buf))
      break;
  if (!target_fio_func_map[idx].name)	/* ERROR: No such function. */
    return RETURN_ERROR;
  target_fio_func_map[idx].func (c, request->operations);
  return 0;
}

/* Close any open descriptors, and reinitialize the file mapping.  */

void
target_fileio_reset (void)
{
  int ix;

  for (ix = 0; ix != target_fio_data.fd_map_size; ix++)
    {
      int fd = target_fio_data.fd_map[ix];

      if (fd >= 0)
	close (fd);
    }
  if (target_fio_data.fd_map)
    {
      free (target_fio_data.fd_map);
      target_fio_data.fd_map = NULL;
      target_fio_data.fd_map_size = 0;
    }
}


void
target_fileio_request (char *buf, struct file_io_operations *operations)
{
  struct request_args args = {buf, operations};
  int ex;

  target_fio_no_longjmp = 0;

  ex = catch_exceptions (uiout, do_target_fileio_request, (void *)&args,
			 RETURN_MASK_ALL);
  switch (ex)
    {
      case RETURN_ERROR:
	operations->reply (-1, FILEIO_ENOSYS);
        break;
      case RETURN_QUIT:
        operations->reply (-1, FILEIO_EINTR);
	break;
      default:
        break;
    }
}

static void
set_system_call_allowed (char *args, int from_tty)
{
  if (args)
    {
      char *arg_end;
      int val = strtoul (args, &arg_end, 10);
      if (*args && *arg_end == '\0')
        {
	  target_fio_system_call_allowed = !!val;
	  return;
	}
    }
  error (_("Illegal argument for \"set remote system-call-allowed\" command"));
}

static void
show_system_call_allowed (char *args, int from_tty)
{
  if (args)
    error (_("Garbage after \"show remote system-call-allowed\" command: `%s'"), args);
  printf_unfiltered ("Calling host system(3) call from target is %sallowed\n",
		     target_fio_system_call_allowed ? "" : "not ");
}

void
initialize_target_fileio (struct cmd_list_element *set_cmdlist,
                          struct cmd_list_element *show_cmdlist)
{
  add_cmd ("system-call-allowed", no_class,
           set_system_call_allowed,
           _("Set if the host system(3) call is allowed for the target."),
           &set_cmdlist);
  add_cmd ("system-call-allowed", no_class,
           show_system_call_allowed,
           _("Show if the host system(3) call is allowed for the target."),
           &show_cmdlist);
}
