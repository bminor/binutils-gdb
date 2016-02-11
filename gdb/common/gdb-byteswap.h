/* Basic byte-swapping routines, for GDB, the GNU debugger.

   Copyright (C) 2009-2016 Free Software Foundation, Inc.

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

#ifndef GDB_BYTESWAP_H
#define GDB_BYTESWAP_H 1

/* All 'extract' functions return a host-format integer from a
   target-format integer at ADDR which is LEN bytes long.  */

extern LONGEST extract_signed_integer (const gdb_byte *, int,
				       enum bfd_endian);

extern ULONGEST extract_unsigned_integer (const gdb_byte *, int,
					  enum bfd_endian);

/* Sometimes a long long unsigned integer can be extracted as a
   LONGEST value.  This is done so that we can print these values
   better.  If this integer can be converted to a LONGEST, this
   function returns 1 and sets *PVAL.  Otherwise it returns 0.  */

extern int extract_long_unsigned_integer (const gdb_byte *, int,
					  enum bfd_endian, LONGEST *);

/* All 'store' functions accept a host-format integer and store a
   target-format integer at ADDR which is LEN bytes long.  */

extern void store_signed_integer (gdb_byte *, int,
				  enum bfd_endian, LONGEST);

extern void store_unsigned_integer (gdb_byte *, int,
				    enum bfd_endian, ULONGEST);

#endif
