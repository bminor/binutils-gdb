/* Copyright (C) 1991 Free Software Foundation, Inc.
   
This file is part of GLD, the Gnu Linker.

GLD is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GLD is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GLD; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/*
 * By steve chamberlain
 * steve@cygnus.com
 */

#include "bfd.h"
#include "sysdep.h" 
#include "ld.h"
#include "ldexp.h"
#include "ldlang.h"
#include "ldsym.h"
#include "ldmisc.h"
#include "ldgram.h"

/* exported list of statements needed to handle constructors */
lang_statement_list_type constructor_list;



typedef struct constructor_list 
{
   CONST char *name;
    struct constructor_list *next;
}  constructor_list_type;
   
static constructor_list_type *constructor_name_list;

static void
add_constructor_name (name)
     CONST char *name;
{
    register constructor_list_type *ptr = constructor_name_list;
    for (; ptr != (constructor_list_type *)NULL; ptr = ptr->next) {
	if (strcmp (ptr->name, name) == 0) 
	    return;
    }

    /* There isn't an entry, so add one */
    ptr = (constructor_list_type *) ldmalloc (sizeof(constructor_list_type));
    ptr->next = constructor_name_list;
    ptr->name = name;
    constructor_name_list = ptr;
}

void
ldlang_add_constructor (name)
     ldsym_type *name;
{
  if (name->flags & SYM_CONSTRUCTOR) return;
  add_constructor_name (name->name);
  name->flags |= SYM_CONSTRUCTOR;
}


/* this function looks through the sections attached to the supplied
   bfd to see if any of them are magical constructor sections. If so
   their names are remembered and added to the list of constructors */

void
ldlang_check_for_constructors (entry)
     struct lang_input_statement_struct *entry;
{
    asection *section;

    for (section = entry->the_bfd->sections;
	 section != (asection *)NULL;
	 section = section->next) 
    {
	if (section->flags & SEC_CONSTRUCTOR) 
	    add_constructor_name (section->name);
    }
}


/* run through the symbol table, find all the symbols which are
   constructors and for each one, create statements to do something
   like..

   for something like "__CTOR_LIST__, foo" in the assembler

   __CTOR_LIST__ = . ;
   LONG(__CTOR_LIST_END - . / 4 - 2)
   *(foo)
   __CTOR_LIST_END= .

   Put these statements onto a special list.

*/


void
find_constructors ()
{
    lang_statement_list_type *old = stat_ptr;
    constructor_list_type *p = constructor_name_list;
    stat_ptr = & constructor_list;
    lang_list_init(stat_ptr);
    while (p != (constructor_list_type *)NULL) 
    {
	/* Have we already done this one ? */
	CONST char *name = p->name;
	ldsym_type *lookup = ldsym_get_soft(name);

	/* If ld is invoked from collect, then the constructor list
	   will already have been defined, so don't do it again. */

	if (lookup->sdefs_chain == (asymbol **)NULL) 
	    {
		size_t len = strlen(name);
		char *end = ldmalloc(len+3);
		strcpy(end, name);
		strcat(end,"$e");

		lang_add_assignment
		    ( exp_assop('=',name, exp_nameop(NAME,".")));

		lang_add_data
		    (LONG, exp_binop('-',
				     exp_binop ( '/',
						exp_binop ( '-',
							   exp_nameop(NAME, end),
							   exp_nameop(NAME,".")),
						exp_intop(4)),

				     exp_intop(2)));

				      
		lang_add_wild(name, (char *)NULL);
		lang_add_data(LONG, exp_intop(0));
		lang_add_assignment
		    (exp_assop('=', end, exp_nameop(NAME,".")));
	    }
	p = p->next;		    
    }
    stat_ptr = old;
}

