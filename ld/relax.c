

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
    }
    break;
  case lang_data_statement_enum:
    {
abort();
      
#if 0
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
			       
			       
#endif

    }
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


    
static void
DEFUN(perform_slip,(s, slip, input_section, value),
      asymbol **s AND
      unsigned int slip AND
      asection *input_section AND
      bfd_vma value)
{
  
  /* Find all symbols past this point, and make them know
     what's happened */
  while (*s) 
  {
    asymbol *p = *s;
    if (p->section == input_section) 
    {
      /* This was pointing into this section, so mangle it */
      if (p->value > value)
      {
	p->value -=2;
      }
    }
    s++;
	
  }    
}
static int 
DEFUN(movb1,(input_section, symbols, r, shrink),
      asection *input_section AND
      asymbol **symbols AND
      arelent *r AND
      unsigned int shrink) 
{


  bfd_vma value = get_value(r, input_section);
	
  if (value >= 0xff00)
  { 

    /* Change the reloc type from 16bit, possible 8 to 8bit
       possible 16 */
    r->howto = r->howto + 1;	  
    /* The place to relc moves back by one */
    r->address -=1;
	  
    /* This will be two bytes smaller in the long run */
    shrink +=2 ;
    perform_slip(symbols, 2, input_section, r->address - shrink +1);

	  
  }      
  return shrink;      
}

static int 
DEFUN(jmp1,(input_section, symbols, r, shrink),
      asection *input_section AND
      asymbol **symbols AND
      arelent *r AND
      unsigned int shrink) 
{

  
  bfd_vma value = get_value(r, 0);
	
  bfd_vma dot = input_section->output_section->vma +
   input_section->output_offset + r->address;	
  bfd_vma gap;
  
    /* See if the address we're looking at within 127 bytes of where
       we are, if so then we can use a small branch rather than the
       jump we were going to */

  gap = value - (dot - shrink);
  

  if (-120 < (long)gap && (long)gap < 120 )
  { 

    /* Change the reloc type from 16bit, possible 8 to 8bit
       possible 16 */
    r->howto = r->howto + 1;	  
    /* The place to relc moves back by one */
    r->address -=1;
	  
    /* This will be two bytes smaller in the long run */
    shrink +=2 ;
    perform_slip(symbols, 2, input_section, r->address-shrink +1);

	  
  }      
  return shrink;      
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

  
  /* Get enough memory to hold the stuff */
  bfd *input_bfd = i->owner;
  asection *input_section = i;
  int shrink = 0 ;
  int new = 0;
  
  bfd_size_type reloc_size = bfd_get_reloc_upper_bound(input_bfd,
						       input_section);
  arelent **reloc_vector = (arelent **)ldmalloc(reloc_size);

  /* Get the relocs and think about them */
  if (bfd_canonicalize_reloc(input_bfd, 
			     input_section,
			     reloc_vector,
			     is->ifile->asymbols) )
  {
    arelent **parent;
    asymbol **symbols =   is->ifile->asymbols; 
    for (parent = reloc_vector; *parent; parent++) 
    {
      arelent *r = *parent;
      switch (r->howto->type) {
	case R_MOVB2:
	case R_JMP2:
	  
	  shrink+=2;
	  break;
	  
	case R_MOVB1:
	  shrink = movb1(input_section, symbols, r, shrink);
	  new = 1;
	  
	  break;
	case R_JMP1:
	  shrink = jmp1(input_section, symbols, r, shrink);
	  new = 1;
	  
	  break;
	}
    }

  }
  input_section->_cooked_size -= shrink;  
  free((char *)reloc_vector);
  return new;
  
}

