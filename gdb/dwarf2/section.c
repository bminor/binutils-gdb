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

#include "defs.h"
#include "dwarf2/section.h"
#include "gdb_bfd.h"
#include "objfiles.h"

struct dwarf2_section_info *
get_containing_section (const struct dwarf2_section_info *section)
{
  gdb_assert (section->is_virtual);
  return section->s.containing_section;
}

struct bfd *
get_section_bfd_owner (const struct dwarf2_section_info *section)
{
  if (section->is_virtual)
    {
      section = get_containing_section (section);
      gdb_assert (!section->is_virtual);
    }
  return section->s.section->owner;
}

asection *
get_section_bfd_section (const struct dwarf2_section_info *section)
{
  if (section->is_virtual)
    {
      section = get_containing_section (section);
      gdb_assert (!section->is_virtual);
    }
  return section->s.section;
}

const char *
get_section_name (const struct dwarf2_section_info *section)
{
  asection *sectp = get_section_bfd_section (section);

  gdb_assert (sectp != NULL);
  return bfd_section_name (sectp);
}

const char *
get_section_file_name (const struct dwarf2_section_info *section)
{
  bfd *abfd = get_section_bfd_owner (section);

  return bfd_get_filename (abfd);
}

int
get_section_id (const struct dwarf2_section_info *section)
{
  asection *sectp = get_section_bfd_section (section);

  if (sectp == NULL)
    return 0;
  return sectp->id;
}

int
get_section_flags (const struct dwarf2_section_info *section)
{
  asection *sectp = get_section_bfd_section (section);

  gdb_assert (sectp != NULL);
  return bfd_section_flags (sectp);
}

int
dwarf2_section_empty_p (const struct dwarf2_section_info *section)
{
  if (section->is_virtual)
    return section->size == 0;
  return section->s.section == NULL || section->size == 0;
}

void
dwarf2_read_section (struct objfile *objfile, dwarf2_section_info *info)
{
  asection *sectp;
  bfd *abfd;
  gdb_byte *buf, *retbuf;

  if (info->readin)
    return;
  info->buffer = NULL;
  info->readin = true;

  if (dwarf2_section_empty_p (info))
    return;

  sectp = get_section_bfd_section (info);

  /* If this is a virtual section we need to read in the real one first.  */
  if (info->is_virtual)
    {
      struct dwarf2_section_info *containing_section =
	get_containing_section (info);

      gdb_assert (sectp != NULL);
      if ((sectp->flags & SEC_RELOC) != 0)
	{
	  error (_("Dwarf Error: DWP format V2 with relocations is not"
		   " supported in section %s [in module %s]"),
		 get_section_name (info), get_section_file_name (info));
	}
      dwarf2_read_section (objfile, containing_section);
      /* Other code should have already caught virtual sections that don't
	 fit.  */
      gdb_assert (info->virtual_offset + info->size
		  <= containing_section->size);
      /* If the real section is empty or there was a problem reading the
	 section we shouldn't get here.  */
      gdb_assert (containing_section->buffer != NULL);
      info->buffer = containing_section->buffer + info->virtual_offset;
      return;
    }

  /* If the section has relocations, we must read it ourselves.
     Otherwise we attach it to the BFD.  */
  if ((sectp->flags & SEC_RELOC) == 0)
    {
      info->buffer = gdb_bfd_map_section (sectp, &info->size);
      return;
    }

  buf = (gdb_byte *) obstack_alloc (&objfile->objfile_obstack, info->size);
  info->buffer = buf;

  /* When debugging .o files, we may need to apply relocations; see
     http://sourceware.org/ml/gdb-patches/2002-04/msg00136.html .
     We never compress sections in .o files, so we only need to
     try this when the section is not compressed.  */
  retbuf = symfile_relocate_debug_section (objfile, sectp, buf);
  if (retbuf != NULL)
    {
      info->buffer = retbuf;
      return;
    }

  abfd = get_section_bfd_owner (info);
  gdb_assert (abfd != NULL);

  if (bfd_seek (abfd, sectp->filepos, SEEK_SET) != 0
      || bfd_bread (buf, info->size, abfd) != info->size)
    {
      error (_("Dwarf Error: Can't read DWARF data"
	       " in section %s [in module %s]"),
	     bfd_section_name (sectp), bfd_get_filename (abfd));
    }
}
