/* Do various things to symbol tables (other than lookup)), for GDB.
   Copyright (C) 1986, 1987, 1989 Free Software Foundation, Inc.

This file is part of GDB.

GDB is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GDB is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GDB; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */


#include "defs.h"
#include "param.h"
#include "symtab.h"
#include "breakpoint.h"
#include "command.h"

#include <stdio.h>
#include <obstack.h>

static void free_symtab ();


/* Free all the symtabs that are currently installed,
   and all storage associated with them.
   Leaves us in a consistent state with no symtabs installed.  */

void
free_all_symtabs ()
{
  register struct symtab *s, *snext;

  /* All values will be invalid because their types will be!  */

  clear_value_history ();
  clear_displays ();
  clear_internalvars ();
#if defined (CLEAR_SOLIB)
  CLEAR_SOLIB ();
#endif
  set_default_breakpoint (0, 0, 0, 0);

  current_source_symtab = 0;

  for (s = symtab_list; s; s = snext)
    {
      snext = s->next;
      free_symtab (s);
    }
  symtab_list = 0;
  obstack_free (symbol_obstack, 0);
  obstack_init (symbol_obstack);

  if (misc_function_vector)
    free (misc_function_vector);
  misc_function_count = 0;
  misc_function_vector = 0;
  clear_pc_function_cache();
}

/* Free a struct block <- B and all the symbols defined in that block.  */

static void
free_symtab_block (b)
     struct block *b;
{
  register int i, n;
  n = BLOCK_NSYMS (b);
  for (i = 0; i < n; i++)
    {
      free (SYMBOL_NAME (BLOCK_SYM (b, i)));
      free (BLOCK_SYM (b, i));
    }
  free (b);
}

/* Free all the storage associated with the struct symtab <- S.
   Note that some symtabs have contents malloc'ed structure by structure,
   while some have contents that all live inside one big block of memory,
   and some share the contents of another symbol table and so you should
   not free the contents on their behalf (except sometimes the linetable,
   which maybe per symtab even when the rest is not).
   It is s->free_code that says which alternative to use.  */

static void
free_symtab (s)
     register struct symtab *s;
{
  register int i, n;
  register struct blockvector *bv;
  register struct typevector *tv;

  switch (s->free_code)
    {
    case free_nothing:
      /* All the contents are part of a big block of memory
	 and some other symtab is in charge of freeing that block.
	 Therefore, do nothing.  */
      break;

    case free_contents:
      /* Here all the contents were malloc'ed structure by structure
	 and must be freed that way.  */
      /* First free the blocks (and their symbols.  */
      bv = BLOCKVECTOR (s);
      n = BLOCKVECTOR_NBLOCKS (bv);
      for (i = 0; i < n; i++)
	free_symtab_block (BLOCKVECTOR_BLOCK (bv, i));
      /* Free the blockvector itself.  */
      free (bv);
      /* Free the type vector.  */
      tv = TYPEVECTOR (s);
      free (tv);
      /* Also free the linetable.  */
      
    case free_linetable:
      /* Everything will be freed either by our `free_ptr'
	 or by some other symbatb, except for our linetable.
	 Free that now.  */
      free (LINETABLE (s));
      break;
    }

  /* If there is a single block of memory to free, free it.  */
  if (s->free_ptr)
    free (s->free_ptr);

  /* Free source-related stuff */
  if (s->line_charpos)
    free (s->line_charpos);
  if (s->fullname)
    free (s->fullname);
  free (s);
}

/* If a symtab for filename NAME is found, free it along
   with any dependent breakpoints, displays, etc.
   Used when loading new versions of object modules with the "add-file"
   command.
   
   FIXME.  I think this is not the right way to do this.  It needs further
   investigation, though.  -- gnu@cygnus  */

void
free_named_symtab (name)
     char *name;
{
  register struct symtab *s;
  register struct symtab *prev;
  struct blockvector *bv;

#if 0	/* FIXME */
  /* Look for a symtab with the specified name.
     We can't use lookup_symtab () for this, since it 
     might generate a recursive call to psymtab_to_symtab ().  */

  for (s = symtab_list; s; s = s->next)
    {
      if (!strcmp (name, s->filename))
	break;
      prev = s;
    }

  if (s)
    {
      if (s == symtab_list)
	symtab_list = s->next;
      else
	prev->next = s->next;

      /* For now, delete all breakpoints, displays, etc., whether or
	 not they depend on the symtab being freed.  This should be
	 changed so that only those data structures affected are deleted.  */

      /* But don't delete anything if the symtab is empty.
	 This test is necessary due to a bug in "dbxread.c" that
	 causes empty symtabs to be created for N_SO symbols that
	 contain the pathname of the object file.  (This problem
	 has been fixed in GDB 3.9x).  */

      bv = BLOCKLIST (s);
      if (BLOCKLIST_NBLOCKS (bv) > 2
	  || BLOCK_NSYMS (BLOCKVECTOR_BLOCK (bv, 0))
	  || BLOCK_NSYMS (BLOCKVECTOR_BLOCK (bv, 1)))
	{
	  /* Took the following line out because GDB ends up printing it
	     many times when a given module is loaded, because each module
	     contains many symtabs.  */
	  /*
	  printf ("Clearing breakpoints and resetting debugger state.\n");
	  */

	  clear_value_history ();
	  clear_displays ();
	  clear_internalvars ();
	  clear_breakpoints ();
	  set_default_breakpoint (0, 0, 0, 0);
	  current_source_symtab = 0;
	}

      free_symtab (s);
    }
  else
    /* It is still possible that some breakpoints will be affected
       even though no symtab was found, since the file might have
       been compiled without debugging, and hence not be associated
       with a symtab.  In order to handle this correctly, we would need
       to keep a list of text address ranges for undebuggable files.
       For now, we do nothing, since this is a fairly obscure case.  */
    ;
#endif  /* FIXME */
}


static int block_depth ();
static void print_symbol ();

void
print_symtabs (filename)
     char *filename;
{
  FILE *outfile;
  register struct symtab *s;
  register int i, j;
  int len, blen;
  register struct linetable *l;
  struct blockvector *bv;
  register struct block *b;
  int depth;
  struct cleanup *cleanups;
  extern int fclose();

  if (filename == 0)
    error_no_arg ("file to write symbol data in");

  filename = tilde_expand (filename);
  make_cleanup (free, filename);
  
  outfile = fopen (filename, "w");
  if (outfile == 0)
    perror_with_name (filename);

  cleanups = make_cleanup (fclose, outfile);
  immediate_quit++;

  for (s = symtab_list; s; s = s->next)
    {
      /* First print the line table.  */
      fprintf (outfile, "Symtab for file %s\n\n", s->filename);
      fprintf (outfile, "Line table:\n\n");
      l = LINETABLE (s);
      len = l->nitems;
      for (i = 0; i < len; i++)
	fprintf (outfile, " line %d at %x\n", l->item[i].line,
		 l->item[i].pc);
      /* Now print the block info.  */
      fprintf (outfile, "\nBlockvector:\n\n");
      bv = BLOCKVECTOR (s);
      len = BLOCKVECTOR_NBLOCKS (bv);
      for (i = 0; i < len; i++)
	{
	  b = BLOCKVECTOR_BLOCK (bv, i);
	  depth = block_depth (b) * 2;
	  print_spaces (depth, outfile);
	  fprintf (outfile, "block #%03d (object 0x%x) ", i, b);
	  fprintf (outfile, "[0x%x..0x%x]", BLOCK_START (b), BLOCK_END (b));
	  if (BLOCK_SUPERBLOCK (b))
	    fprintf (outfile, " (under 0x%x)", BLOCK_SUPERBLOCK (b));
	  if (BLOCK_FUNCTION (b))
	    fprintf (outfile, " %s", SYMBOL_NAME (BLOCK_FUNCTION (b)));
	  fputc ('\n', outfile);
	  blen = BLOCK_NSYMS (b);
	  for (j = 0; j < blen; j++)
	    {
	      print_symbol (BLOCK_SYM (b, j), depth + 1, outfile);
	    }
	}

      fprintf (outfile, "\n\n");
    }

  immediate_quit--;
  do_cleanups (cleanups);
}

static void
print_symbol (symbol, depth, outfile)
     struct symbol *symbol;
     int depth;
     FILE *outfile;
{
  print_spaces (depth, outfile);
  if (SYMBOL_NAMESPACE (symbol) == LABEL_NAMESPACE)
    {
      fprintf (outfile, "label %s at 0x%x\n", SYMBOL_NAME (symbol),
	       SYMBOL_VALUE_ADDRESS (symbol));
      return;
    }
  if (SYMBOL_NAMESPACE (symbol) == STRUCT_NAMESPACE)
    {
      if (TYPE_NAME (SYMBOL_TYPE (symbol)))
	{
	  type_print_1 (SYMBOL_TYPE (symbol), "", outfile, 1, depth);
	}
      else
	{
	  fprintf (outfile, "%s %s = ",
	       (TYPE_CODE (SYMBOL_TYPE (symbol)) == TYPE_CODE_ENUM
		? "enum"
		: (TYPE_CODE (SYMBOL_TYPE (symbol)) == TYPE_CODE_STRUCT
		   ? "struct" : "union")),
	       SYMBOL_NAME (symbol));
	  type_print_1 (SYMBOL_TYPE (symbol), "", outfile, 1, depth);
	}
      fprintf (outfile, ";\n");
    }
  else
    {
      if (SYMBOL_CLASS (symbol) == LOC_TYPEDEF)
	fprintf (outfile, "typedef ");
      if (SYMBOL_TYPE (symbol))
	{
	  type_print_1 (SYMBOL_TYPE (symbol), SYMBOL_NAME (symbol),
			outfile, 1, depth);
	  fprintf (outfile, "; ");
	}
      else
	fprintf (outfile, "%s ", SYMBOL_NAME (symbol));

      switch (SYMBOL_CLASS (symbol))
	{
	case LOC_CONST:
	  fprintf (outfile, "const %ld (0x%lx),",
		   SYMBOL_VALUE (symbol), SYMBOL_VALUE (symbol));
	  break;

	case LOC_CONST_BYTES:
	  fprintf (outfile, "const %u hex bytes:",
		   TYPE_LENGTH (SYMBOL_TYPE (symbol)));
	  {
	    unsigned i;
	    for (i = 0; i < TYPE_LENGTH (SYMBOL_TYPE (symbol)); i++)
	      fprintf (outfile, " %2x",
			 (unsigned)SYMBOL_VALUE_BYTES (symbol) [i]);
	    fprintf (outfile, ",");
	  }
	  break;

	case LOC_STATIC:
	  fprintf (outfile, "static at 0x%x,", SYMBOL_VALUE_ADDRESS (symbol));
	  break;

	case LOC_REGISTER:
	  fprintf (outfile, "register %ld,", SYMBOL_VALUE (symbol));
	  break;

	case LOC_ARG:
	  fprintf (outfile, "arg at 0x%lx,", SYMBOL_VALUE (symbol));
	  break;

	case LOC_LOCAL_ARG:
	  fprintf (outfile, "arg at offset 0x%x from fp,",
		   SYMBOL_VALUE (symbol));

	case LOC_REF_ARG:
	  fprintf (outfile, "reference arg at 0x%lx,", SYMBOL_VALUE (symbol));
	  break;

	case LOC_REGPARM:
	  fprintf (outfile, "parameter register %ld,", SYMBOL_VALUE (symbol));
	  break;

	case LOC_LOCAL:
	  fprintf (outfile, "local at 0x%lx,", SYMBOL_VALUE (symbol));
	  break;

	case LOC_TYPEDEF:
	  break;

	case LOC_LABEL:
	  fprintf (outfile, "label at 0x%lx", SYMBOL_VALUE_ADDRESS (symbol));
	  break;

	case LOC_BLOCK:
	  fprintf (outfile, "block (object 0x%x) starting at 0x%x,",
		   SYMBOL_BLOCK_VALUE (symbol),
		   BLOCK_START (SYMBOL_BLOCK_VALUE (symbol)));
	  break;

	case LOC_EXTERNAL:
	  fprintf (outfile, "external at 0x%x", SYMBOL_VALUE_ADDRESS (symbol));
	  break;

        default:
	  fprintf (outfile, "botched symbol class %x", SYMBOL_CLASS (symbol));
	  break;
	}
    }
  fprintf (outfile, "\n");
}

/* Return the nexting depth of a block within other blocks in its symtab.  */

static int
block_depth (block)
     struct block *block;
{
  register int i = 0;
  while (block = BLOCK_SUPERBLOCK (block)) i++;
  return i;
}

/*
 * Free all partial_symtab storage.
 */
void
free_all_psymtabs()
{
  obstack_free (psymbol_obstack, 0);
  obstack_init (psymbol_obstack);
  partial_symtab_list = (struct partial_symtab *) 0;
}

void
_initialize_symmisc ()
{
  symtab_list = (struct symtab *) 0;
  partial_symtab_list = (struct partial_symtab *) 0;
  
  add_com ("printsyms", class_obscure, print_symtabs,
	   "Print dump of current symbol definitions to file OUTFILE.");
}

