#include "sysdep.h"
#include "bfd.h"
#include "ldsym.h"
#include "ldwarn.h"
#include "ldmisc.h"

/* we keep all the warning symbols in a list, if we ever get a
   warning, we'll search it the hard way. This won't be to bad since
   warnings are infrequent, and never that many (true or false ?).

*/

typedef struct warning_list_struct {
  struct warning_list_struct *next;
  asymbol *sym;
} warning_list_type;


static warning_list_type *warning_list;



/* This is a warning symbol, add the error text to a list we keep, and mark
   the symbol referenced as requiring a warning */


void 
DEFUN(add_warning,(sym),
      asymbol *sym)
{
  CONST    char *name = ((asymbol *)(sym->value))->name;
  warning_list_type *new;

  ldsym_type *lookup = ldsym_get(name);

  lookup->flags |= SYM_WARNING;

  new = (warning_list_type *)ldmalloc(sizeof(warning_list_type));
  new->next = warning_list;
  new->sym  = sym;
  warning_list = new;
}

/* run through the list we kept, and find the warning associated with
   this symbol */
CONST char *
DEFUN(fetch_warning,(sym),
asymbol *sym)
{
  warning_list_type *ptr = warning_list;
  while (ptr != (warning_list_type *)NULL) {
    if (strcmp(((asymbol*)(ptr->sym->value))->name, sym->name) == 0) {
      return ptr->sym->name;
    }
    ptr = ptr->next;
  }
  return "This is a warning without a message !";
}


void 
DEFUN(produce_warnings,(lgs,it),
      ldsym_type *lgs AND
      asymbol *it)
{
  asymbol **ptr;
  ptr  = lgs->srefs_chain;
  while (ptr != (asymbol **)NULL) {
    asymbol *ref = *ptr;
    info("%B: %s\n", ref->the_bfd, fetch_warning(it));
    ptr = (asymbol **)(ref->udata);
  }
}
