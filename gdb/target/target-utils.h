/* Utility target functions for GDB, and GDBserver.

   Copyright (C) 2015 Free Software Foundation, Inc.

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

#ifndef TARGET_TARGET_UTILS_H
#define TARGET_TARGET_UTILS_H

typedef int (read_alloc_pread_ftype) (int handle, gdb_byte *read_buf, int len,
				      ULONGEST offset, int *target_errno);
extern LONGEST read_alloc (gdb_byte **buf_p, int handle,
			   read_alloc_pread_ftype *pread_func, int padding);

struct inferior;
typedef LONGEST (read_stralloc_func_ftype) (struct inferior *inf,
					    const char *filename,
					    gdb_byte **buf_p, int padding);
extern char *read_stralloc (struct inferior *inf, const char *filename,
			    read_stralloc_func_ftype *func);

#endif /* TARGET_TARGET_UTILS_H */
