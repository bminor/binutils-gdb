/* ldindr.c
   Handle indirect symbols.

   BFD supplies symbols to be indirected with the BFD_INDIRECT bit
   set. Whenever the linker gets one of these, it calls add_indirect
   with the symbol. We look up the symbol which this one dereferneces,
   and stop if they are the same. If they are not the same, copy all
   the information from the current to the dereffed symbol. Set the
   indirect bit in the flag. From now on the ldsym_get stuff will
   perform the indirection for us, at no charge.
*/



#include "sysdep.h"
#include "bfd.h"
#include "ld.h"
#include "ldsym.h"
#include "ldmisc.h"



static asymbol **
DEFUN(move_it,(a_list, b_list),
asymbol **a_list AND
asymbol **b_list)
{
  asymbol **head = a_list;
  asymbol **cursor = head;

  if (a_list == 0) return b_list;
  if (b_list == 0) return a_list;

  while (1) {
    asymbol *ptr = cursor[0];
    asymbol **next = (asymbol **)(ptr->udata);
    if (next == 0) {
      ptr->udata = (PTR) b_list;
      return head;
    }
    cursor = next;
  }
}

void 
DEFUN(add_indirect,(ptr),
asymbol **ptr)
{
  ldsym_type *lgs = ldsym_get((*ptr)->name);
  ldsym_type *new = ldsym_get(((asymbol *)((*ptr)->value))->name);

  /* If the mapping has already been done, stop now */
  if (lgs == new) return;
  lgs->flags |= SYM_INDIRECT;

  new->scoms_chain = move_it(new->scoms_chain, lgs->scoms_chain);
  lgs->scoms_chain = 0;
  new->srefs_chain = move_it(new->srefs_chain, lgs->srefs_chain);
  lgs->srefs_chain = 0;
  new->sdefs_chain = move_it(new->sdefs_chain, lgs->sdefs_chain);
  lgs->sdefs_chain = 0;

  lgs->sdefs_chain = (asymbol **)new;
}



