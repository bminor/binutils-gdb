/* Low-level DWARF 2 reading code

   Copyright (C) 1994-2020 Free Software Foundation, Inc.

   Adapted by Gary Funck (gary@intrepid.com), Intrepid Technology,
   Inc.  with support from Florida State University (under contract
   with the Ada Joint Program Office), and Silicon Graphics, Inc.
   Initial contribution by Brent Benson, Harris Computer Systems, Inc.,
   based on Fred Fish's (Cygnus Support) implementation of DWARF 1
   support.

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

#ifndef GDB_DWARF2_LEB_H
#define GDB_DWARF2_LEB_H

/* Read dwarf information from a buffer.  */

static inline unsigned int
read_1_byte (bfd *abfd, const gdb_byte *buf)
{
  return bfd_get_8 (abfd, buf);
}

static inline int
read_1_signed_byte (bfd *abfd, const gdb_byte *buf)
{
  return bfd_get_signed_8 (abfd, buf);
}

static inline unsigned int
read_2_bytes (bfd *abfd, const gdb_byte *buf)
{
  return bfd_get_16 (abfd, buf);
}

static inline int
read_2_signed_bytes (bfd *abfd, const gdb_byte *buf)
{
  return bfd_get_signed_16 (abfd, buf);
}

/* Read the next three bytes (little-endian order) as an unsigned integer.  */
static inline unsigned int
read_3_bytes (bfd *abfd, const gdb_byte *buf)
{
  unsigned int result = 0;
  for (int i = 0; i < 3; ++i)
    {
      unsigned char byte = bfd_get_8 (abfd, buf);
      buf++;
      result |= ((unsigned int) byte << (i * 8));
    }
  return result;
}

static inline unsigned int
read_4_bytes (bfd *abfd, const gdb_byte *buf)
{
  return bfd_get_32 (abfd, buf);
}

static inline int
read_4_signed_bytes (bfd *abfd, const gdb_byte *buf)
{
  return bfd_get_signed_32 (abfd, buf);
}

static inline ULONGEST
read_8_bytes (bfd *abfd, const gdb_byte *buf)
{
  return bfd_get_64 (abfd, buf);
}

extern LONGEST read_signed_leb128 (bfd *, const gdb_byte *, unsigned int *);

extern ULONGEST read_unsigned_leb128 (bfd *, const gdb_byte *, unsigned int *);

#endif /* GDB_DWARF2_LEB_H */
