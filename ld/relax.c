/* Copyright (C) 1992, 1993 Free Software Foundation, Inc.

This file is part of GLD, the Gnu Linker.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/*

new age linking


Tie together all the interseting blocks

*/


#include "bfd.h"
#include "../bfd/seclet.h"
#include "coff/internal.h"
#include "sysdep.h"

#include "ldlang.h"
#include "ld.h"
#include "ldwrite.h"
#include "ldmisc.h"
#include "ldsym.h"
#include "ldgram.h"
#include "relax.h"
static void
build_it (statement)
     lang_statement_union_type * statement;
{
  switch (statement->header.type)
    {
#if 0
      {

	bfd_byte play_area[SHORT_SIZE];
	unsigned int i;
	bfd_putshort (output_bfd, statement->fill_statement.fill, play_area);
	/* Write out all entire shorts */
	for (i = 0;
	     i < statement->fill_statement.size - SHORT_SIZE + 1;
	     i += SHORT_SIZE)
	  {
	    bfd_set_section_contents (output_bfd,
				   statement->fill_statement.output_section,
				      play_area,
				statement->data_statement.output_offset + i,
				      SHORT_SIZE);

	  }

	/* Now write any remaining byte */
	if (i < statement->fill_statement.size)
	  {
	    bfd_set_section_contents (output_bfd,
				   statement->fill_statement.output_section,
				      play_area,
				statement->data_statement.output_offset + i,
				      1);

	  }

	abort ();
      }
      break;
#endif
    case lang_data_statement_enum:

      {

	bfd_vma value = statement->data_statement.value;
	bfd_byte play_area[LONG_SIZE];
	unsigned int size = 0;
	asection *output_section = statement->data_statement.output_section;
	switch (statement->data_statement.type)
	  {
	  case LONG:
	    bfd_put_32 (output_section->owner, value, play_area);
	    size = LONG_SIZE;
	    break;
	  case SHORT:
	    bfd_put_16 (output_section->owner, value, play_area);
	    size = SHORT_SIZE;
	    break;
	  case BYTE:
	    bfd_put_8 (output_section->owner, value, play_area);
	    size = BYTE_SIZE;
	    break;
	  }

	bfd_set_section_contents (output_section->owner,
				  statement->data_statement.output_section,
				  play_area,
				  statement->data_statement.output_vma,
				  size);



      }

      break;
    case lang_input_section_enum:
      {
	/* Create a new seclet in the output section with this
	attached */
	if (statement->input_section.ifile->just_syms_flag == false)
	  {
	    asection *i = statement->input_section.section;

	    asection *output_section = i->output_section;

	    bfd_seclet_type *seclet = bfd_new_seclet (output_section->owner, output_section);

	    if (i->flags & SEC_NEVER_LOAD)
	      {
		/* We've got a never load section inside one which is going
	      to be output, we'll change it into a fill seclet */
		seclet->type = bfd_fill_seclet;
		seclet->u.fill.value = 0;
	      }
	    else
	      {
		seclet->type = bfd_indirect_seclet;
		seclet->u.indirect.section = i;
		seclet->u.indirect.symbols
		  = statement->input_section.ifile->asymbols;
	      }
	    seclet->size = i->_cooked_size;
	    seclet->offset = i->output_offset;
	    seclet->next = 0;
	  }

      }
      break;
    case lang_padding_statement_enum:
      /* Make a new seclet with the right filler */
      {
	/* Create a new seclet in the output section with this
	attached */

	bfd_seclet_type *seclet =
	bfd_new_seclet (statement->padding_statement.output_section->owner,
			statement->padding_statement.output_section);

	seclet->type = bfd_fill_seclet;
	seclet->size = statement->padding_statement.size;
	seclet->offset = statement->padding_statement.output_offset;
	seclet->u.fill.value = statement->padding_statement.fill;
	seclet->next = 0;
      }
      break;



      break;
    default:
      /* All the other ones fall through */
      ;

    }



}


void
write_relax (output_bfd, data, relocateable)
     bfd * output_bfd;
     PTR data;
     boolean relocateable;
{
  /* Tie up all the statements to generate an output bfd structure which
     bfd can mull over */
  lang_for_each_statement (build_it);

  if (bfd_seclet_link (output_bfd, data, relocateable) == false)
    einfo ("%F%P: %B: %E\n", output_bfd);
}

/* See if we can change the size of this section by shrinking the
   relocations in it. If this happens, then we'll have to renumber the
   symbols in it, and shift around the data too.
 */
boolean
relax_section (this_ptr)
     lang_statement_union_type ** this_ptr;
{
  extern lang_input_statement_type *script_file;
  lang_input_section_type *is = &((*this_ptr)->input_section);
  asection *i = is->section;
  if (!(i->owner->flags & BFD_IS_RELAXABLE))
    {
      if (i->owner != script_file->the_bfd)
	einfo ("%B: not assembled with -linkrelax\n", i->owner);
    }

  return bfd_relax_section (i->owner, i, is->ifile->asymbols);

}
