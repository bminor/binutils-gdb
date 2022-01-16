/* DWARF 2 abbreviations

   Copyright (C) 1994-2022 Free Software Foundation, Inc.

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
#include "dwarf2/read.h"
#include "dwarf2/abbrev.h"
#include "dwarf2/leb.h"
#include "bfd.h"

/* Hash function for an abbrev.  */

static hashval_t
hash_abbrev (const void *item)
{
  const struct abbrev_info *info = (const struct abbrev_info *) item;
  /* Warning: if you change this next line, you must also update the
     other code in this class using the _with_hash functions.  */
  return info->number;
}

/* Comparison function for abbrevs.  */

static int
eq_abbrev (const void *lhs, const void *rhs)
{
  const struct abbrev_info *l_info = (const struct abbrev_info *) lhs;
  const struct abbrev_info *r_info = (const struct abbrev_info *) rhs;
  return l_info->number == r_info->number;
}

/* Abbreviation tables.

   In DWARF version 2, the description of the debugging information is
   stored in a separate .debug_abbrev section.  Before we read any
   dies from a section we read in all abbreviations and install them
   in a hash table.  */

abbrev_table::abbrev_table (sect_offset off)
  : sect_off (off),
    m_abbrevs (htab_create_alloc (20, hash_abbrev, eq_abbrev,
				  nullptr, xcalloc, xfree))
{
}

/* Add an abbreviation to the table.  */

void
abbrev_table::add_abbrev (struct abbrev_info *abbrev)
{
  void **slot = htab_find_slot_with_hash (m_abbrevs.get (), abbrev,
					  abbrev->number, INSERT);
  *slot = abbrev;
}

/* Read in an abbrev table.  */

abbrev_table_up
abbrev_table::read (struct dwarf2_section_info *section,
		    sect_offset sect_off)
{
  bfd *abfd = section->get_bfd_owner ();
  const gdb_byte *abbrev_ptr;
  struct abbrev_info *cur_abbrev;

  abbrev_table_up abbrev_table (new struct abbrev_table (sect_off));
  struct obstack *obstack = &abbrev_table->m_abbrev_obstack;

  /* Caller must ensure this.  */
  gdb_assert (section->readin);
  abbrev_ptr = section->buffer + to_underlying (sect_off);

  while (true)
    {
      unsigned int bytes_read;
      /* Loop until we reach an abbrev number of 0.  */
      unsigned int abbrev_number = read_unsigned_leb128 (abfd, abbrev_ptr,
							 &bytes_read);
      if (abbrev_number == 0)
	break;
      abbrev_ptr += bytes_read;

      /* Start without any attrs.  */
      obstack_blank (obstack, offsetof (abbrev_info, attrs));
      cur_abbrev = (struct abbrev_info *) obstack_base (obstack);

      /* Read in abbrev header.  */
      cur_abbrev->number = abbrev_number;
      cur_abbrev->tag
	= (enum dwarf_tag) read_unsigned_leb128 (abfd, abbrev_ptr,
						 &bytes_read);
      abbrev_ptr += bytes_read;
      cur_abbrev->has_children = read_1_byte (abfd, abbrev_ptr);
      abbrev_ptr += 1;

      /* Now read in declarations.  */
      int num_attrs = 0;
      for (;;)
	{
	  struct attr_abbrev cur_attr;

	  cur_attr.name
	    = (enum dwarf_attribute) read_unsigned_leb128 (abfd, abbrev_ptr,
							   &bytes_read);
	  abbrev_ptr += bytes_read;
	  cur_attr.form
	    = (enum dwarf_form) read_unsigned_leb128 (abfd, abbrev_ptr,
						      &bytes_read);
	  abbrev_ptr += bytes_read;
	  if (cur_attr.form == DW_FORM_implicit_const)
	    {
	      cur_attr.implicit_const = read_signed_leb128 (abfd, abbrev_ptr,
							    &bytes_read);
	      abbrev_ptr += bytes_read;
	    }
	  else
	    cur_attr.implicit_const = -1;

	  if (cur_attr.name == 0)
	    break;

	  ++num_attrs;
	  obstack_grow (obstack, &cur_attr, sizeof (cur_attr));
	}

      cur_abbrev = (struct abbrev_info *) obstack_finish (obstack);
      cur_abbrev->num_attrs = num_attrs;
      abbrev_table->add_abbrev (cur_abbrev);
    }

  return abbrev_table;
}
