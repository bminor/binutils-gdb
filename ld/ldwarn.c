/*
   Copyright (C) 1991 Free Software Foundation, Inc.
   Written by Steve Chamberlain of Cygnus Support.

This file is part of GLD, the GNU linker.

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

#include "bfd.h"
#include "sysdep.h"
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
add_warning (sym)
     asymbol *sym;
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
fetch_warning (sym)
     asymbol *sym;
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
produce_warnings (lgs, it)
     ldsym_type *lgs;
     asymbol *it;
{
  asymbol **ptr;
  ptr  = lgs->srefs_chain;
  while (ptr != (asymbol **)NULL) {
    asymbol *ref = *ptr;
    info_msg ("%B: %s\n", bfd_asymbol_bfd(ref), fetch_warning(it));
    ptr = (asymbol **)(ref->udata);
  }
}
