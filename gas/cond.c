/* cond.c - conditional assembly pseudo-ops, and .include
   Copyright (C) 1990, 1991 Free Software Foundation, Inc.

This file is part of GAS, the GNU Assembler.

GAS is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GAS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GAS; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* static const char rcsid[] = "$Id$"; */

#include "as.h"

#include "obstack.h"

void s_ifdef(arg)
int arg;
{
/*	register char c; */
	register char *name;	/* points to name of symbol */
	register struct symbol *	symbolP; /* Points to symbol */
	
	SKIP_WHITESPACE();		/* Leading whitespace is part of operand. */
	name = input_line_pointer;
	if (!is_name_beginner(*name)) {
		as_bad("invalid identifier for .ifdef");
		obstack_1grow (&cond_obstack, 0);
	} else {
		get_symbol_end();
		++input_line_pointer;
		symbolP = symbol_find(name);
		
		/* ??? Should we try to optimize such that if we hit a .endif
		   before a .else, we don't need to push state?  */
		obstack_1grow(&cond_obstack, (symbolP != 0) ^ arg);
	}
} /* s_ifdef() */

/* This is allocated to grow and shrink as .ifdef/.endif pairs
   are scanned.  When the top element is nonzero, it means
   we should accept input.  Otherwise, we should ignore input.  */
struct obstack cond_obstack;

void s_if(arg)
int arg;
{
	expressionS operand;
	
	SKIP_WHITESPACE();		/* Leading whitespace is part of operand. */
	expr(0, &operand);
	
	if (operand.X_add_symbol != NULL
	    || operand.X_subtract_symbol != NULL)
	    as_bad("non-constant expression in .if statement");
	
	/* If the above error is signaled, this will dispatch
	   using an undefined result.  No big deal.  */
	obstack_1grow(&cond_obstack, (operand.X_add_number != 0) ^ arg);
} /* s_if() */

void s_endif(arg)
int arg;
{
	char *base = obstack_base(&cond_obstack);
	char *ptr = obstack_next_free(&cond_obstack);
	
	if (ptr-1 == base) {
		as_bad("unbalanced .endif");
	} else {
		obstack_free(&cond_obstack, ptr-1);
		cond_obstack.object_base = base;
	}
} /* s_endif() */

void s_else(arg)
int arg;
{
	char *ptr = obstack_next_free(&cond_obstack);
	if (ptr-1 == obstack_base(&cond_obstack)) {
		as_bad(".else without matching .if");
	} else {
		ptr[-1] = !ptr[-1];
	}
} /* s_else() */

void s_ifeqs(arg)
int arg;
{
	as_bad("ifeqs not implemented.");
} /* s_ifeqs() */

void s_end(arg)
int arg;
{
	;
} /* s_end() */

int ignore_input() {
  char *ptr = obstack_next_free (&cond_obstack);

  /* We cannot ignore certain pseudo ops.  */
  if (input_line_pointer[-1] == '.')
    {
      if (input_line_pointer[0] == 'i'
	  && (!strncmp (input_line_pointer, "if", 2)
	      || !strncmp (input_line_pointer, "ifdef", 5)
	      || !strncmp (input_line_pointer, "ifndef", 6)))
	return 0;
      if (input_line_pointer[0] == 'e'
	  && (!strncmp (input_line_pointer, "else", 4)
	      || !strncmp (input_line_pointer, "endif", 5)))
	return 0;
    }

  return (ptr[-1] == 0);
} /* ignore_input() */

/* end of cond.c */
