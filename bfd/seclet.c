/* This module is part of BFD */


/* The intention is that one day, all the code which uses sections
   will change and use seclets instead - maybe seglet would have been
   a better name..

   Anyway, a seclet contains enough info to be able to describe an
   area of output memory in one go.

   The only description so far catered for is that of the
   <<bfd_indirect_seclet>>, which is a select which points to a
   <<section>> and the <<asymbols>> associated with the section, so
   that relocation can be done when needed.

   One day there will be more types - they will at least migrate from
   the linker's data structures - also there could be extra stuff,
   like a bss seclet, which descibes a lump of memory as containing
   zeros compactly, without the horrible SEC_* flag cruft.


*/

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"
#include "seclet.h"
#include "coff/internal.h"
bfd_seclet_type *
DEFUN(bfd_new_seclet,(abfd, section),
      bfd *abfd AND
      asection *section)
{
  bfd_seclet_type *n = (bfd_seclet_type *)bfd_alloc(abfd, sizeof(bfd_seclet_type));
  if (section->seclets_tail != (bfd_seclet_type *)NULL) {
      section->seclets_tail->next = n;
    }
  else
  {
    section->seclets_head = n;
  }
  section->seclets_tail = n;

  return n;
  
}




#define MAX_ERRORS_IN_A_ROW 10
extern bfd_error_vector_type bfd_error_vector;
bfd_vma 
DEFUN(get_value,(reloc, seclet),
      arelent  *reloc AND
      bfd_seclet_type *seclet)
{
  bfd_vma value;
  if (reloc->sym_ptr_ptr) 
  {
    asymbol *symbol = *(reloc->sym_ptr_ptr);

  
    /* A symbol holds a pointer to a section, and an offset from the
       base of the section.  To relocate, we find where the section will
       live in the output and add that in */

    if (symbol->section == (asection *)NULL) 
    {
      /* Ouch, this is an undefined symbol.. */
      bfd_error_vector.undefined_symbol(reloc, seclet);
      value = symbol->value;
    }
    else 
    {
      value = symbol->value +
       symbol->section->output_offset +
	symbol->section->output_section->vma;
    }  
  }
  
  else 
  {
    value = 0;  
  }
  
  /* Add the value contained in the relocation */
  value += (short)((reloc->addend) & 0xffff);
  
  return value;
  

}

static char *
DEFUN(foo_bfd_get_relocated_section_contents,(seclet),
      bfd_seclet_type *seclet)

{
  asymbol **symbols = 0;
  extern bfd *output_bfd;
  bfd *abfd;    

  /* Get enough memory to hold the stuff */
  bfd *input_bfd = seclet->u.indirect.section->owner;
  asection *input_section = seclet->u.indirect.section;

  char *data = malloc(input_section->_raw_size);
  char *dst = data;
  char *prev_dst = data;
  unsigned int gap = 0;

  bfd_size_type reloc_size = bfd_get_reloc_upper_bound(input_bfd,
						       input_section);
  arelent **reloc_vector = (arelent **)ldmalloc(reloc_size);
  abfd = output_bfd;
  
  /* read in the section */
  bfd_get_section_contents(input_bfd,
			   input_section,
			   data,
			   0,
			   input_section->_raw_size);
  
  
  if (bfd_canonicalize_reloc(input_bfd, 
			     input_section,
			     reloc_vector,
			     seclet->u.indirect.symbols) )
  {
    arelent **parent = reloc_vector;
    arelent *reloc ;
    


    unsigned int dst_address = 0;
    unsigned int src_address = 0;
    unsigned int run;
    unsigned int idx;
    
    /* Find how long a run we can do */
    while (dst_address < seclet->size) 
    {
      
      reloc = *parent;
      if (reloc) 
      {
	/* Note that the relaxing didn't tie up the addresses in the
	   relocation, so we use the original address to work out the
	   run of non-relocated data */
	run = reloc->address - src_address;
	parent++;
	
      }
      else 
      {
	run = seclet->size - dst_address;
      }
      /* Copy the bytes */
      for (idx = 0; idx < run; idx++)
      {
	data[dst_address++] = data[src_address++];
      }
    
      /* Now do the relocation */
    
      if (reloc) 
      {
	switch (reloc->howto->type) 
	{
	case R_JMP2:
	  /* Speciial relaxed type */
	{
	  bfd_vma dot = seclet->offset + dst_address + seclet->u.indirect.section->output_section->vma;	  
	  int   gap = get_value(reloc,seclet)-dot-1;
	  if ((gap & ~0xff  ) != 0 &&((gap & 0xff00)!= 0xff00)) abort();

	  bfd_put_8(abfd,gap,   data+dst_address);

	  switch (data[dst_address-1]) 
	  {
	    
	  case 0x5e:
	    /* jsr -> bsr */
	    bfd_put_8(abfd, 0x55, data+dst_address-1);
	    break;
	  case 0x5a:	 
	    /* jmp ->bra */
	    bfd_put_8(abfd, 0x40, data+dst_address-1);
	    break;
	  
	  default:
	    abort();
	  
	  }
	
	

	     
	  dst_address++;
	  src_address+=3;
  
	  break;
	}

	     
	case R_MOVB2:
	  /* Special relaxed type, there will be a gap between where we
	     get stuff from and where we put stuff to now 
	     
	     for a mov.b @aa:16 -> mov.b @aa:8
	     opcode 0x6a 0x0y offset
	     ->     0x2y off
	     */
	  if (data[dst_address-1] != 0x6a)
	   abort();
	  switch (data[dst_address] & 0xf0) 
	  {
	  case 0x00:
	    /* Src is memory */
	    data[dst_address-1] = (data[src_address] & 0xf) | 0x20;
	    break;
	  case 0x80:
	    /* Src is reg */
	    data[dst_address-1] = (data[src_address] & 0xf) | 0x30;
	    break;
	  default:
	    abort();
	  }
	
	  /* the offset must fit ! after all, what was all the relaxing
	     about ? */

	  bfd_put_8(abfd, get_value(reloc, seclet), data + dst_address);

	  /* Note the magic - src goes up by two bytes, but dst by only
	     one */
	  dst_address+=1;
	  src_address+=3;
	
	  break;
	  /* PCrel 8 bits */
	case R_PCRBYTE:	  
	{
	  bfd_vma dot = seclet->offset + dst_address + seclet->u.indirect.section->output_section->vma;	  
	  int   gap = get_value(reloc,seclet)-dot;
	  if (gap > 127 || gap < -128) 
	  {
	    bfd_error_vector.reloc_value_truncated(reloc, seclet);
	  }
	  
	  bfd_put_8(abfd,gap,   data+dst_address);
	  dst_address++;
	  src_address++;
  
	  break;
	}

	case R_RELBYTE:
	{
	  unsigned  int gap =get_value(reloc,seclet);
	  if (gap > 256)
	  {
	    bfd_error_vector.reloc_value_truncated(reloc, seclet);
	  }
	  
	  bfd_put_8(abfd, gap, data+dst_address);
	  dst_address+=1;
	  src_address+=1;


	}
	  break; 
	case R_JMP1:
	  /* A relword which would have like to have been a pcrel */
	case R_MOVB1:	
	  /* A relword which would like to have been modified but
	     didn't make it */
	case R_RELWORD:
	  bfd_put_16(abfd, get_value(reloc,seclet), data+dst_address);
	  dst_address+=2;
	  src_address+=2;
	  break;
	
	default:
	  abort();
	}
      }    
    }
  }
  free((char *)reloc_vector);
  return data;
  
}

void
DEFUN(rel,(abfd, seclet, output_section),
      bfd *abfd AND
      bfd_seclet_type *seclet AND
      asection *output_section)
{
  bfd_byte *data;
  if (output_section->flags & SEC_HAS_CONTENTS )
  {
    
  data = bfd_get_relocated_section_contents(abfd, seclet);

  if(bfd_set_section_contents(abfd,
			      output_section,
			      data,
			      seclet->offset,
			      seclet->size) == false)
  {
    abort();
  }
  
}

    

  

}

void
DEFUN(seclet_dump_seclet,(abfd, seclet, section),
      bfd *abfd AND
      bfd_seclet_type *seclet AND
      asection *section)
{
  switch (seclet->type) 
  {
    
  case bfd_indirect_seclet:
    /* The contents of this section come from another one somewhere
       else */
    rel(abfd, seclet, section);
    
    
    break;
    
  default:
    abort();
  }
  


}

void
DEFUN(seclet_dump,(abfd),
      bfd *abfd)
{
  /* Write all the seclets on the bfd out, relocate etc according to the
     rules */

  asection *o = abfd->sections;
  while (o != (asection *)NULL) 
  {
    bfd_seclet_type *p = o->seclets_head;
    while (p != (bfd_seclet_type *)NULL) 
    {
      seclet_dump_seclet(abfd, p, o);
      p = p ->next;
    }
 
    o = o->next;
  }

}
