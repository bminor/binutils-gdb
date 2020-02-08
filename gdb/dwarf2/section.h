/* DWARF 2 low-level section code

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

#ifndef GDB_DWARF2_SECTION_H
#define GDB_DWARF2_SECTION_H

/* A descriptor for dwarf sections.

   S.ASECTION, SIZE are typically initialized when the objfile is first
   scanned.  BUFFER, READIN are filled in later when the section is read.
   If the section contained compressed data then SIZE is updated to record
   the uncompressed size of the section.

   DWP file format V2 introduces a wrinkle that is easiest to handle by
   creating the concept of virtual sections contained within a real section.
   In DWP V2 the sections of the input DWO files are concatenated together
   into one section, but section offsets are kept relative to the original
   input section.
   If this is a virtual dwp-v2 section, S.CONTAINING_SECTION is a backlink to
   the real section this "virtual" section is contained in, and BUFFER,SIZE
   describe the virtual section.  */

struct dwarf2_section_info
{
  union
  {
    /* If this is a real section, the bfd section.  */
    asection *section;
    /* If this is a virtual section, pointer to the containing ("real")
       section.  */
    struct dwarf2_section_info *containing_section;
  } s;
  /* Pointer to section data, only valid if readin.  */
  const gdb_byte *buffer;
  /* The size of the section, real or virtual.  */
  bfd_size_type size;
  /* If this is a virtual section, the offset in the real section.
     Only valid if is_virtual.  */
  bfd_size_type virtual_offset;
  /* True if we have tried to read this section.  */
  bool readin;
  /* True if this is a virtual section, False otherwise.
     This specifies which of s.section and s.containing_section to use.  */
  bool is_virtual;
};

/* Read the contents of the section INFO.
   OBJFILE is the main object file, but not necessarily the file where
   the section comes from.  E.g., for DWO files the bfd of INFO is the bfd
   of the DWO file.
   If the section is compressed, uncompress it before returning.  */

extern void dwarf2_read_section (struct objfile *objfile,
				 dwarf2_section_info *info);

extern const char *get_section_name (const struct dwarf2_section_info *);

extern const char *get_section_file_name (const struct dwarf2_section_info *);

/* Return the containing section of virtual section SECTION.  */

extern struct dwarf2_section_info *get_containing_section
  (const struct dwarf2_section_info *section);

/* Return the bfd owner of SECTION.  */

extern struct bfd *get_section_bfd_owner
  (const struct dwarf2_section_info *section);

/* Return the bfd section of SECTION.
   Returns NULL if the section is not present.  */

extern asection *get_section_bfd_section
  (const struct dwarf2_section_info *section);

/* Return the name of SECTION.  */

extern const char *get_section_name
  (const struct dwarf2_section_info *section);

/* Return the name of the file SECTION is in.  */

extern const char *get_section_file_name
  (const struct dwarf2_section_info *section);

/* Return the id of SECTION.
   Returns 0 if SECTION doesn't exist.  */

extern int get_section_id (const struct dwarf2_section_info *section);

/* Return the flags of SECTION.
   SECTION (or containing section if this is a virtual section) must exist.  */

extern int get_section_flags (const struct dwarf2_section_info *section);

extern int dwarf2_section_empty_p (const struct dwarf2_section_info *section);

#endif /* GDB_DWARF2_SECTION_H */
