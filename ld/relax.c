

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

static void
DEFUN(build_it,(statement),
      lang_statement_union_type *statement)
{
  switch (statement->header.type) {
  case lang_fill_statement_enum: 
    {
#if 0
      bfd_byte play_area[SHORT_SIZE];
      unsigned int i;
      bfd_putshort(output_bfd, statement->fill_statement.fill, play_area);
      /* Write out all entire shorts */
      for (i = 0;
	   i < statement->fill_statement.size - SHORT_SIZE + 1;
	   i+= SHORT_SIZE)
	{
	  bfd_set_section_contents(output_bfd,
				   statement->fill_statement.output_section,
				   play_area,
				   statement->data_statement.output_offset +i,
				   SHORT_SIZE);

	}

      /* Now write any remaining byte */
      if (i < statement->fill_statement.size) 
	{
	  bfd_set_section_contents(output_bfd,
				   statement->fill_statement.output_section,
				   play_area,
				   statement->data_statement.output_offset +i,
				   1);

	}
#endif
      abort();
    }
    break;
  case lang_data_statement_enum:
#if 0
    {

      bfd_vma value = statement->data_statement.value;
      bfd_byte play_area[LONG_SIZE];
      unsigned int size = 0;
      switch (statement->data_statement.type) {
      case LONG:
	bfd_put_32(output_bfd, value,  play_area);
	size = LONG_SIZE;
	break;
      case SHORT:
	bfd_put_16(output_bfd, value,  play_area);
	size = SHORT_SIZE;
	break;
      case BYTE:
	bfd_put_8(output_bfd, value,  play_area);
	size = BYTE_SIZE;
	break;
      }
      
      bfd_set_section_contents(output_bfd,
			       statement->data_statement.output_section,
			       play_area,
			       statement->data_statement.output_vma,
			       size);
			       
			       

    }
#endif
    break;
  case lang_input_section_enum:
    {
      /* Create a new seclet in the output section with this
	 attached */

      asection *i  = statement->input_section.section;

      asection *output_section = i->output_section;
	
      bfd_seclet_type *seclet  = bfd_new_seclet(output_section->owner,output_section);
	
      seclet->type = bfd_indirect_seclet;
      seclet->u.indirect.section = i;
      seclet->u.indirect.symbols = statement->input_section.ifile->asymbols;
      seclet->size = bfd_get_section_size_before_reloc(i);
      seclet->offset = i->output_offset;
      seclet->next = 0;
	
    }
    break;

  default:
    /* All the other ones fall through */
    ;

  }



}


void 
DEFUN(write_relaxnorel,(output_bfd),
      bfd *output_bfd)
{
/* Tie up all the statements to generate an output bfd structure which
   bfd can mull over */


  lang_for_each_statement(build_it);

  seclet_dump(output_bfd);

}


    


/* See if we can change the size of this section by shrinking the
   relocations in it. If this happens, then we'll have to renumber the
   symbols in it, and shift around the data too.
 */
boolean
DEFUN(relax_section,(this_ptr),
      lang_statement_union_type **this_ptr)
{

  lang_input_section_type *is = &((*this_ptr)->input_section);
  asection *i = is->section;

  return bfd_relax_section(i->owner, i, is->ifile->asymbols);
  
}

