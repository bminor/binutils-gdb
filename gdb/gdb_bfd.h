/* Definitions for BFD wrappers used by GDB.

   Copyright (C) 2011, 2012
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

/* Open a read-only (FOPEN_RB) BFD given arguments like bfd_fopen.
   Returns NULL on error.  On success, returns a new reference to the
   BFD, which must be freed with gdb_bfd_unref.  BFDs returned by this
   call are shared among all callers opening the same file.  If FD is
   not -1, then after this call it is owned by BFD.  */

struct bfd *gdb_bfd_open (const char *name, const char *target, int fd);

/* Acquire a new reference to ABFD.  Returns ABFD for convenience.
   It is fine for ABFD to be NULL; in this case the function does
   nothing and returns NULL.  */

struct bfd *gdb_bfd_ref (struct bfd *abfd);

/* Release a reference to ABFD.  If this is the last reference, ABFD
   will be freed.  If ABFD is NULL, this function does nothing.  */

void gdb_bfd_unref (struct bfd *abfd);

/* Try to read or map the contents of the section SECT.  If
   successful, the section data is returned and *SIZE is set to the
   size of the section data; this may not be the same as the size
   according to bfd_get_section_size if the section was compressed.
   The returned section data is associated with the BFD and will be
   destroyed when the BFD is destroyed.  There is no other way to free
   it; for temporary uses of section data, see
   bfd_malloc_and_get_section.  SECT may not have relocations.  This
   function will throw on error.  */

const gdb_byte *gdb_bfd_map_section (asection *section, bfd_size_type *size);

#endif /* GDB_BFD_H */
