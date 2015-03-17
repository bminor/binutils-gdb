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

#ifndef COMMON_REMOTE_FILEIO_H
#define COMMON_REMOTE_FILEIO_H

#include "gdb/fileio.h"
#include <sys/stat.h>

/* Pack a host-format integer into a byte buffer in big-endian format
   ready for transmission over the remote protocol.  BYTES specifies
   the size of the integer to pack in bytes.  */

static inline void
remote_fileio_to_be (LONGEST num, char *buf, int bytes)
{
  int i;

  for (i = 0; i < bytes; ++i)
    buf[i] = (num >> (8 * (bytes - i - 1))) & 0xff;
}

/* Pack a host-format integer into an fio_uint_t.  */

static inline void
remote_fileio_to_fio_uint (long num, fio_uint_t fnum)
{
  remote_fileio_to_be ((LONGEST) num, (char *) fnum, 4);
}

/* Pack a host-format time_t into an fio_time_t.  */

static inline void
remote_fileio_to_fio_time (time_t num, fio_time_t fnum)
{
  remote_fileio_to_be ((LONGEST) num, (char *) fnum, 4);
}

/* Pack a host-format struct stat into a struct fio_stat.  */

extern void remote_fileio_to_fio_stat (struct stat *st,
				       struct fio_stat *fst);

#endif /* COMMON_REMOTE_FILEIO_H */
