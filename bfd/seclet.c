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
