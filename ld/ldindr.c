/* ldindr.c
   Handle indirect symbols.

   BFD supplies symbols to be indirected with the BFD_INDIRECT bit
   set. Whenever the linker gets one of these, it calls add_indirect
   with the symbol. We create an entry into the ldsym hash table as if it
   were a normal symbol, but with the SYM_INDIRECT bit set in the
   flags.

   When it comes time to tie up the symbols at a later date, the flag
   will be seen and a call made to do the right thing (tm)

*/



#include "sysdep.h"
#include "bfd.h"
#include "ld.h"
#include "ldsym.h"

extern ld_config_type config;
void 
DEFUN(add_indirect,(ptr),
asymbol **ptr)
{
  if (config.relocateable_output == false) {
    ldsym_type *sp = ldsym_get((*ptr)->name);
    sp->flags |= SYM_INDIRECT;
    sp->sdefs_chain = ptr;
  }
}



void 
DEFUN(do_indirect,(ptr),
ldsym_type *ptr)
{
if (config.relocateable_output == false) {
  /* Dig out the symbol were indirecting to. It's held in the value
     field. 
     */


  CONST char *name = ((asymbol *)(*(ptr->sdefs_chain))->value)->name;

  ldsym_type *new = ldsym_get(name);

  /* We have to make a copy of the sdefs_chain item name, since
     symbols will be clobbered on writing, and we want to write the
     same string twice */


  ptr->sdefs_chain[0][0] = new->sdefs_chain[0][0];
  ptr->sdefs_chain[0][0].name = name;
}
}
