/* Definitions for BFD wrappers used by GDB.

   Copyright (C) 2011
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

#ifndef GDB_BFD_H
#define GDB_BFD_H

/* Make a copy ABFD's filename using bfd_alloc, and reassign it to the
   BFD.  This ensures that the BFD's filename has the same lifetime as
   the BFD itself.  */

void gdb_bfd_stash_filename (struct bfd *abfd);

/* Acquire a new reference to ABFD.  Returns ABFD for convenience.
   It is fine for ABFD to be NULL; in this case the function does
   nothing and returns NULL.  */

struct bfd *gdb_bfd_ref (struct bfd *abfd);

/* Release a reference to ABFD.  If this is the last reference, ABFD
   will be freed.  If ABFD is NULL, this function does nothing.  */

void gdb_bfd_unref (struct bfd *abfd);

#endif /* GDB_BFD_H */
