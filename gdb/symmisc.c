/* Do various things to symbol tables (other than lookup), for GDB.
   Copyright 1986, 1987, 1989, 1991 Free Software Foundation, Inc.

This file is part of GDB.

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


#include <stdio.h>
#include "defs.h"
#include "symtab.h"
#include "bfd.h"
#include "symfile.h"
#include "breakpoint.h"
#include "command.h"
#include "obstack.h"

#include <string.h>

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

void
free_symtab (s)
     register struct symtab *s;
{
  register int i, n;
  register struct blockvector *bv;

  switch (s->free_code)
    {
    case free_nothing:
      /* All the contents are part of a big block of memory (an obstack),
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
      /* Also free the linetable.  */
      
    case free_linetable:
      /* Everything will be freed either by our `free_ptr'
	 or by some other symtab, except for our linetable.
	 Free that now.  */
      if (LINETABLE (s))
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

static int block_depth ();
static void print_symbol ();
static void print_partial_symbol ();

void
printsyms_command (filename)
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
  char *symname;

  if (filename == 0)
    error_no_arg ("file to write symbol data in");

  /* If a second arg is supplied, it is a source file name to match on */
  symname = strchr (filename, ' ');
  if (symname)
    *symname++ = '\0';

  filename = tilde_expand (filename);
  make_cleanup (free, filename);
  
  outfile = fopen (filename, "w");
  if (outfile == 0)
    perror_with_name (filename);

  cleanups = make_cleanup (fclose, outfile);
  immediate_quit++;

  for (s = symtab_list; s; s = s->next)
    {
      /* If source file name is specified, reject all but that one.  */
      if (symname)
        if (0 != strncmp (symname, s->filename, strlen (symname)))
	  continue;

      fprintf (outfile, "Symtab for file %s\n", s->filename);
      fprintf (outfile, "Read from object file %s (%x)\n", s->objfile->name,
			s->objfile);

      /* First print the line table.  */
      l = LINETABLE (s);
      if (l) {
	fprintf (outfile, "\nLine table:\n\n");
	len = l->nitems;
	for (i = 0; i < len; i++)
	  fprintf (outfile, " line %d at %x\n", l->item[i].line,
		   l->item[i].pc);
      }
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

        default:
	  fprintf (outfile, "botched symbol class %x", SYMBOL_CLASS (symbol));
	  break;
	}
    }
  fprintf (outfile, "\n");
}

void
printpsyms_command (filename)
     char *filename;
{
  FILE *outfile;
  struct partial_symtab *p;
  struct cleanup *cleanups;
  extern int fclose();
  char *symname;

  if (filename == 0)
    error_no_arg ("file to write partial symbol data in");

  /* If a second arg is supplied, it is a source file name to match on */
  symname = strchr (filename, ' ');
  if (symname)
    *symname++ = '\0';

  filename = tilde_expand (filename);
  make_cleanup (free, filename);
  
  outfile = fopen (filename, "w");
  if (outfile == 0)
    perror_with_name (filename);

  cleanups = make_cleanup (fclose, outfile);
  immediate_quit++;

  for (p = partial_symtab_list; p; p = p->next)
    {
      /* If source file name is specified, reject all but that one.  */
      if (symname)
        if (0 != strncmp (symname, p->filename, strlen (symname)))
	  continue;

      fprintf_filtered (outfile, "Partial symtab for source file %s ",
	       p->filename);
      fprintf_filtered (outfile, "(object 0x%x)\n\n", p);
      fprintf (outfile, "  Read from object file %s (0x%x)\n", p->objfile->name,
			p->objfile);

      if (p->readin)
	fprintf_filtered (outfile, "  Full symtab was read (at 0x%x by function at 0x%x)\n",
			  p->symtab, p->read_symtab);
      fprintf_filtered (outfile, "  Relocate symbols by 0x%x\n", p->addr);
      fprintf_filtered (outfile, "  Symbols cover text addresses 0x%x-0x%x\n",
			p->textlow, p->texthigh);
      fprintf_filtered (outfile, "  Depends on %d other partial symtabs.\n",
			p->number_of_dependencies);
      if (p->n_global_syms > 0)
	print_partial_symbol (global_psymbols.list + p->globals_offset,
			      p->n_global_syms, "Global", outfile);
      if (p->n_static_syms > 0)
	print_partial_symbol (static_psymbols.list + p->statics_offset,
			      p->n_static_syms, "Static", outfile);
      fprintf_filtered (outfile, "\n\n");
    }

  immediate_quit--;
  do_cleanups (cleanups);
}

static void
print_partial_symbol (p, count, what, outfile)
struct partial_symbol *p;
int count;
char *what;
FILE *outfile;
{

  fprintf_filtered (outfile, "  %s partial symbols:\n", what);
  while (count-- > 0)
    {
      fprintf_filtered (outfile, "    `%s', ", SYMBOL_NAME(p));
      switch (SYMBOL_NAMESPACE (p))
	{
	case UNDEF_NAMESPACE:
	  fputs_filtered ("undefined namespace, ", outfile);
	  break;
	case VAR_NAMESPACE:
	  /* This is the usual thing -- don't print it */
	  break;
	case STRUCT_NAMESPACE:
	  fputs_filtered ("struct namespace, ", outfile);
	  break;
	case LABEL_NAMESPACE:
	  fputs_filtered ("label namespace, ", outfile);
	  break;
	default:
	  fputs_filtered ("<invalid namespace>, ", outfile);
	  break;
	}
      switch (SYMBOL_CLASS (p))
	{
	case LOC_UNDEF:
	  fputs_filtered ("undefined", outfile);
	  break;
	case LOC_CONST:
	  fputs_filtered ("constant int", outfile);
	  break;
	case LOC_STATIC:
	  fputs_filtered ("static", outfile);
	  break;
	case LOC_REGISTER:
	  fputs_filtered ("register", outfile);
	  break;
	case LOC_ARG:
	  fputs_filtered ("pass by value", outfile);
	  break;
	case LOC_REF_ARG:
	  fputs_filtered ("pass by reference", outfile);
	  break;
	case LOC_REGPARM:
	  fputs_filtered ("register parameter", outfile);
	  break;
	case LOC_LOCAL:
	  fputs_filtered ("stack parameter", outfile);
	  break;
	case LOC_TYPEDEF:
	  fputs_filtered ("type", outfile);
	  break;
	case LOC_LABEL:
	  fputs_filtered ("label", outfile);
	  break;
	case LOC_BLOCK:
	  fputs_filtered ("function", outfile);
	  break;
	case LOC_CONST_BYTES:
	  fputs_filtered ("constant bytes", outfile);
	  break;
	case LOC_LOCAL_ARG:
	  fputs_filtered ("shuffled arg", outfile);
	  break;
	default:
	  fputs_filtered ("<invalid location>", outfile);
	  break;
	}
      fputs_filtered (", ", outfile);
      fprintf_filtered (outfile, "0x%x\n", SYMBOL_VALUE (p));
      p++;
    }
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

static void
printobjfiles_command ()
{
  struct objfile *objfile;
  struct symtab *symtab;
  struct partial_symtab *psymtab;
  int first;

  for (objfile = object_files;  objfile;  objfile = objfile->next) {
    printf_filtered ("\nObject file %s:  ", objfile->name);
    printf_filtered ("Objfile at %x, bfd at %x\n\n", objfile, objfile->obfd);

    if (objfile->psymtabs) {
      printf_filtered ("Psymtabs:\n");
      for (psymtab = objfile->psymtabs;
	   psymtab;
	   psymtab = psymtab->objfile_chain) {
        printf_filtered ("%s at %x, ", psymtab->filename, psymtab);
	if (psymtab->objfile != objfile)
	  printf_filtered ("NOT ON CHAIN!  ");
        wrap_here ("  ");
      }
      printf_filtered ("\n\n");
    }

    if (objfile->symtabs) {
      printf_filtered ("Symtabs:\n");
      for (symtab = objfile->symtabs;
	   symtab;
	   symtab = symtab->objfile_chain) {
        printf_filtered ("%s at %x, ", symtab->filename, symtab);
	if (symtab->objfile != objfile)
	  printf_filtered ("NOT ON CHAIN!  ");
        wrap_here ("  ");
      }
      printf_filtered ("\n\n");
    }
  }

  /* Now check for psymtabs that aren't owned by an objfile.  */

  first = 1;
  for (psymtab = partial_symtab_list; psymtab; psymtab = psymtab->next) {
    for (objfile = object_files; objfile; objfile = objfile->next) {
      if (psymtab->objfile == objfile)
        goto next;
    }
    if (first)
      printf_filtered ("Psymtabs that aren't owned by any objfile:\n");
    first = 0;
    printf_filtered ("  %s at %x, psymtab->objfile %x\n", psymtab->filename,
		     psymtab, psymtab->objfile);
  next: ;
  }

  /* Now check for symtabs that aren't owned by an objfile.  */

  first = 1;
  for (symtab = symtab_list; symtab; symtab = symtab->next) {
    for (objfile = object_files; objfile; objfile = objfile->next) {
      if (symtab->objfile == objfile)
        goto next2;
    }
    if (first)
      printf_filtered ("Symtabs that aren't owned by any objfile:\n");
    first = 0;
    printf_filtered ("  %s at %x, symtab->objfile %x\n", symtab->filename,
		     symtab, symtab->objfile);
  next2: ;
  }
}

struct cplus_struct_type cplus_struct_default;

void
allocate_cplus_struct_type (type)
     struct type *type;
{
  if (!HAVE_CPLUS_STRUCT (type))
    {
      int nfields = TYPE_NFIELDS (type);
      TYPE_CPLUS_SPECIFIC (type) = (struct cplus_struct_type *)
	obstack_alloc (symbol_obstack, sizeof (struct cplus_struct_type));
      *(TYPE_CPLUS_SPECIFIC(type)) = cplus_struct_default;
    }
}

/* Increase the space allocated for LISTP. */

void
extend_psymbol_list(listp)
     register struct psymbol_allocation_list *listp;
{
  int new_size;
  if (listp->size == 0)
    {
      new_size = 255;
      listp->list = (struct partial_symbol *)
	xmalloc (new_size * sizeof (struct partial_symbol));
    }
  else
    {
      new_size = listp->size * 2;
      listp->list = (struct partial_symbol *)
	xrealloc (listp->list, new_size * sizeof (struct partial_symbol));
    }
  /* Next assumes we only went one over.  Should be good if
     program works correctly */
  listp->next = listp->list + listp->size;
  listp->size = new_size;
}

#ifdef DEBUG

/* The work performed by this function is normally done by the macro
   ADD_PSYMBOL_TO_LIST defined in symfile.h.  When debugging gdb, this
   function makes things easier. */

void
add_psymbol_to_list (name, namelength, namespace, class, listp, psymval)
     char *name;
     int namelength;
     enum namespace namespace;
     enum address_class class;
     struct psymbol_allocation_list *listp;
     unsigned long psymval;
{
  register struct partial_symbol *psym;

  if (listp -> next >= listp -> list + listp -> size)
    extend_psymbol_list (listp);
  psym = listp -> next++;
  SYMBOL_NAME (psym) = (char *) obstack_alloc (psymbol_obstack,
					       namelength + 1);
  memcpy (SYMBOL_NAME (psym), name, namelength);
  SYMBOL_NAME (psym)[namelength] = '\0';
  SYMBOL_NAMESPACE (psym) = namespace;
  SYMBOL_CLASS (psym) = class;
  SYMBOL_VALUE (psym) = psymval;
}

/* The work performed by this function is normally done by the macro
   ADD_PSYMBOL_ADDR_TO_LIST defined in symfile.h.  When debugging gdb, this
   function makes things easier. */

void
add_psymbol_addr_to_list (name, namelength, namespace, class, listp, psymval)
     char *name;
     int namelength;
     enum namespace namespace;
     enum address_class class;
     struct psymbol_allocation_list *listp;
     CORE_ADDR psymval;
{
  register struct partial_symbol *psym;

  if (listp -> next >= listp -> list + listp -> size)
    extend_psymbol_list (listp);
  psym = listp -> next++;
  SYMBOL_NAME (psym) = (char *) obstack_alloc (psymbol_obstack,
					       namelength + 1);
  memcpy (SYMBOL_NAME (psym), name, namelength);
  SYMBOL_NAME (psym)[namelength] = '\0';
  SYMBOL_NAMESPACE (psym) = namespace;
  SYMBOL_CLASS (psym) = class;
  SYMBOL_VALUE_ADDRESS (psym) = psymval;
}

#endif /* DEBUG */

void
_initialize_symmisc ()
{
  symtab_list = (struct symtab *) 0;
  partial_symtab_list = (struct partial_symtab *) 0;
  
  add_com ("printsyms", class_obscure, printsyms_command,
	   "Print dump of current symbol definitions to file OUTFILE.\n\
If a SOURCE file is specified, dump only that file's symbols.");
  add_com ("printpsyms", class_obscure, printpsyms_command,
	  "Print dump of current partial symbol definitions to file OUTFILE.\n\
If a SOURCE file is specified, dump only that file's partial symbols.");
  add_com ("printobjfiles", class_obscure, printobjfiles_command,
	   "Print dump of current object file definitions.");
}

