/* Do various things to symbol tables (other than lookup), for GDB.
   Copyright 1986, 1987, 1989, 1991, 1992 Free Software Foundation, Inc.

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


#include "defs.h"
#include "symtab.h"
#include "gdbtypes.h"
#include "bfd.h"
#include "symfile.h"
#include "breakpoint.h"
#include "command.h"
#include "obstack.h"

#include <string.h>

#ifndef DEV_TTY
#define DEV_TTY "/dev/tty"
#endif

/* Prototypes for local functions */

static PTR 
dump_symtab PARAMS ((struct objfile *, struct symtab *, PTR, PTR, PTR));

static PTR 
dump_psymtab PARAMS ((struct objfile *, struct partial_symtab *, PTR, PTR,
		      PTR));

static PTR 
dump_msymbols PARAMS ((struct objfile *, PTR, PTR, PTR));

static PTR 
dump_objfile PARAMS ((struct objfile *, PTR, PTR, PTR));

static void
printobjfiles_command PARAMS ((void));

static int
block_depth PARAMS ((struct block *));

static void
print_partial_symbol PARAMS ((struct partial_symbol *, int, char *, FILE *));

static void
printpsyms_command PARAMS ((char *, int));

static void
print_symbol PARAMS ((struct symbol *, int, FILE *));

static void
printsyms_command PARAMS ((char *, int));

static void
free_symtab_block PARAMS ((struct objfile *, struct block *));

static void
printmsyms_command PARAMS ((char *, int));

static void
dump_symtab_lines PARAMS ((struct symtab *));

static void
dump_symtabs PARAMS ((struct symtab *));

static void
dump_last_symtab PARAMS ((struct symtab *));

static void
dump_blockvector PARAMS ((struct blockvector *));

static void
dump_block PARAMS ((struct block *));

static char *
dump_addrclass PARAMS ((int));

static char *
dump_namespace PARAMS ((int));

static void
dump_symbol PARAMS ((struct symbol *));

static void
dump_type PARAMS ((struct type *));

static void
dump_linetable PARAMS ((struct linetable *));

static void
dump_strtbl PARAMS ((void));


/* Free a struct block <- B and all the symbols defined in that block.  */

static void
free_symtab_block (objfile, b)
     struct objfile *objfile;
     struct block *b;
{
  register int i, n;
  n = BLOCK_NSYMS (b);
  for (i = 0; i < n; i++)
    {
      mfree (objfile -> md, SYMBOL_NAME (BLOCK_SYM (b, i)));
      mfree (objfile -> md, BLOCK_SYM (b, i));
    }
  mfree (objfile -> md, b);
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
	free_symtab_block (s -> objfile, BLOCKVECTOR_BLOCK (bv, i));
      /* Free the blockvector itself.  */
      mfree (s -> objfile -> md, bv);
      /* Also free the linetable.  */
      
    case free_linetable:
      /* Everything will be freed either by our `free_ptr'
	 or by some other symtab, except for our linetable.
	 Free that now.  */
      if (LINETABLE (s))
	mfree (s -> objfile -> md, LINETABLE (s));
      break;
    }

  /* If there is a single block of memory to free, free it.  */
  if (s -> free_ptr != NULL)
    mfree (s -> objfile -> md, s -> free_ptr);

  /* Free source-related stuff */
  if (s -> line_charpos != NULL)
    mfree (s -> objfile -> md, s -> line_charpos);
  if (s -> fullname != NULL)
    mfree (s -> objfile -> md, s -> fullname);
  mfree (s -> objfile -> md, s);
}

static PTR 
dump_objfile (objfile, arg1, arg2, arg3)
     struct objfile *objfile;
     PTR arg1;
     PTR arg2;
     PTR arg3;
{
  struct symtab *symtab;
  struct partial_symtab *psymtab;

  printf_filtered ("\nObject file %s:  ", objfile -> name);
  printf_filtered ("Objfile at %x, bfd at %x, %d minsyms\n\n",
		   objfile, objfile -> obfd, objfile->minimal_symbol_count);

  if (objfile -> psymtabs)
    {
      printf_filtered ("Psymtabs:\n");
      for (psymtab = objfile -> psymtabs;
	   psymtab != NULL;
	   psymtab = psymtab -> next)
	{
	  printf_filtered ("%s at %x, ", psymtab -> filename, psymtab);
	  if (psymtab -> objfile != objfile)
	    {
	      printf_filtered ("NOT ON CHAIN!  ");
	    }
	  wrap_here ("  ");
	}
      printf_filtered ("\n\n");
    }

  if (objfile -> symtabs)
    {
      printf_filtered ("Symtabs:\n");
      for (symtab = objfile -> symtabs;
	   symtab != NULL;
	   symtab = symtab->next)
	{
	  printf_filtered ("%s at %x, ", symtab -> filename, symtab);
	  if (symtab -> objfile != objfile)
	    {
	      printf_filtered ("NOT ON CHAIN!  ");
	    }
	  wrap_here ("  ");
	}
      printf_filtered ("\n\n");
    }
  return (NULL);
}

static PTR 
dump_msymbols (objfile, arg1, arg2, arg3)
     struct objfile *objfile;
     PTR arg1;
     PTR arg2;
     PTR arg3;
{
  FILE *outfile = (FILE *) arg1;
  char *symname = (char *) arg2;
  struct minimal_symbol *msymbol;
  int index;
  char ms_type;
  
  /* Print minimal symbols from this objfile unless a specific linkage
     unit file was requested, in which case we only print the symbols
     for that one. */
  if (symname == NULL || (strcmp (symname, objfile -> name) == 0))
    {
      fprintf_filtered (outfile, "\nObject file %s:\n\n", objfile -> name);
      for (index = 0, msymbol = objfile -> msymbols;
	   msymbol -> name != NULL; msymbol++, index++)
	{
	  switch (msymbol -> type)
	    {
	      case mst_unknown:
		ms_type = 'u';
		break;
	      case mst_text:
		ms_type = 't';
		break;
	      case mst_data:
		ms_type = 'd';
		break;
	      case mst_bss:
		ms_type = 'b';
		break;
	      case mst_abs:
		ms_type = 'a';
		break;
	      default:
		ms_type = '?';
		break;
	    }
	  fprintf_filtered (outfile, "[%2d] %c %#10x %s\n", index, ms_type,
			    msymbol -> address, msymbol -> name);
	}
      if (objfile -> minimal_symbol_count != index)
	{
	  warning ("internal error:  minimal symbol count %d != %d",
		   objfile -> minimal_symbol_count, index);
	}
      fprintf_filtered (outfile, "\n");
    }
  return (NULL);
}

static PTR 
dump_psymtab (objfile, psymtab, arg1, arg2, arg3)
     struct objfile *objfile;
     struct partial_symtab *psymtab;
     PTR arg1;
     PTR arg2;
     PTR arg3;
{
  FILE *outfile = (FILE *) arg1;
  char *symname = (char *) arg2;

  /* If source file name is specified, reject all but that one.  */
  if (symname == NULL || (strcmp (symname, psymtab -> filename) == 0))
    {
      fprintf_filtered (outfile, "\nPartial symtab for source file %s ",
			psymtab -> filename);
      fprintf_filtered (outfile, "(object 0x%x)\n\n", psymtab);
      fprintf (outfile, "  Read from object file %s (0x%x)\n",
	       objfile -> name, objfile);
      
      if (psymtab -> readin)
	{
	  fprintf_filtered (outfile,
		    "  Full symtab was read (at 0x%x by function at 0x%x)\n",
			    psymtab -> symtab, psymtab -> read_symtab);
	}
      fprintf_filtered (outfile, "  Relocate symbols by 0x%x\n",
			psymtab -> addr);
      fprintf_filtered (outfile, "  Symbols cover text addresses 0x%x-0x%x\n",
			psymtab -> textlow, psymtab -> texthigh);
      fprintf_filtered (outfile, "  Depends on %d other partial symtabs.\n",
			psymtab -> number_of_dependencies);
      if (psymtab -> n_global_syms > 0)
	{
	  print_partial_symbol (objfile -> global_psymbols.list
				+ psymtab -> globals_offset,
				psymtab -> n_global_syms, "Global", outfile);
	}
      if (psymtab -> n_static_syms > 0)
	{
	  print_partial_symbol (objfile -> static_psymbols.list
				+ psymtab -> statics_offset,
				psymtab -> n_static_syms, "Static", outfile);
	}
      fprintf_filtered (outfile, "\n");
    }
  return (NULL);
}

static PTR 
dump_symtab (objfile, symtab, arg1, arg2, arg3)
     struct objfile *objfile;
     struct symtab *symtab;
     PTR arg1;
     PTR arg2;
     PTR arg3;
{
  FILE *outfile = (FILE *) arg1;
  char *symname = (char *) arg2;
  register int i, j;
  int len, blen;
  register struct linetable *l;
  struct blockvector *bv;
  register struct block *b;
  int depth;

  if (symname == NULL || (strcmp (symname, symtab -> filename) == 0))
    {
      fprintf (outfile, "\nSymtab for file %s\n", symtab->filename);
      fprintf (outfile, "Read from object file %s (%x)\n", objfile->name,
	       objfile);
      
      /* First print the line table.  */
      l = LINETABLE (symtab);
      if (l) {
	fprintf (outfile, "\nLine table:\n\n");
	len = l->nitems;
	for (i = 0; i < len; i++)
	  fprintf (outfile, " line %d at %x\n", l->item[i].line,
		   l->item[i].pc);
      }
      /* Now print the block info.  */
      fprintf (outfile, "\nBlockvector:\n\n");
      bv = BLOCKVECTOR (symtab);
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
      fprintf (outfile, "\n");
    }
  return (NULL);
}

static void
printsyms_command (args, from_tty)
     char *args;
     int from_tty;
{
  char **argv;
  FILE *outfile;
  struct cleanup *cleanups;
  char *symname = NULL;
  char *filename = DEV_TTY;

  dont_repeat ();

  if (args == NULL)
    {
      error ("printsyms takes an output file name and optional symbol file name");
    }
  else if ((argv = buildargv (args)) == NULL)
    {
      nomem (0);
    }
  cleanups = make_cleanup (freeargv, (char *) argv);

  if (argv[0] != NULL)
    {
      filename = argv[0];
      /* If a second arg is supplied, it is a source file name to match on */
      if (argv[1] != NULL)
	{
	  symname = argv[1];
	}
    }

  filename = tilde_expand (filename);
  make_cleanup (free, filename);
  
  outfile = fopen (filename, "w");
  if (outfile == 0)
    perror_with_name (filename);
  make_cleanup (fclose, (char *) outfile);

  immediate_quit++;
  iterate_over_symtabs (dump_symtab, (PTR) outfile, (PTR) symname, (PTR) NULL);
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

static void
printpsyms_command (args, from_tty)
     char *args;
     int from_tty;
{
  char **argv;
  FILE *outfile;
  struct cleanup *cleanups;
  char *symname = NULL;
  char *filename = DEV_TTY;

  dont_repeat ();

  if (args == NULL)
    {
      error ("printpsyms takes an output file name and optional symbol file name");
    }
  else if ((argv = buildargv (args)) == NULL)
    {
      nomem (0);
    }
  cleanups = make_cleanup (freeargv, (char *) argv);

  if (argv[0] != NULL)
    {
      filename = argv[0];
      /* If a second arg is supplied, it is a source file name to match on */
      if (argv[1] != NULL)
	{
	  symname = argv[1];
	}
    }

  filename = tilde_expand (filename);
  make_cleanup (free, filename);
  
  outfile = fopen (filename, "w");
  if (outfile == 0)
    perror_with_name (filename);
  make_cleanup (fclose, outfile);

  immediate_quit++;
  iterate_over_psymtabs (dump_psymtab, (PTR) outfile, (PTR) symname,
			 (PTR) NULL);
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

static void
printmsyms_command (args, from_tty)
     char *args;
     int from_tty;
{
  char **argv;
  FILE *outfile;
  struct cleanup *cleanups;
  char *filename = DEV_TTY;
  char *symname = NULL;

  dont_repeat ();

  if (args == NULL)
    {
      error ("printmsyms takes an output file name and optional symbol file name");
    }
  else if ((argv = buildargv (args)) == NULL)
    {
      nomem (0);
    }
  cleanups = make_cleanup (freeargv, argv);

  if (argv[0] != NULL)
    {
      filename = argv[0];
      /* If a second arg is supplied, it is a source file name to match on */
      if (argv[1] != NULL)
	{
	  symname = argv[1];
	}
    }

  filename = tilde_expand (filename);
  make_cleanup (free, filename);
  
  outfile = fopen (filename, "w");
  if (outfile == 0)
    perror_with_name (filename);
  make_cleanup (fclose, outfile);

  immediate_quit++;
  iterate_over_objfiles (dump_msymbols, (PTR) outfile, (PTR) symname,
			 (PTR) NULL);
  immediate_quit--;
  fprintf_filtered (outfile, "\n\n");
  do_cleanups (cleanups);
}

static void
printobjfiles_command ()
{
  dont_repeat ();

  immediate_quit++;
  iterate_over_objfiles (dump_objfile, (PTR) NULL, (PTR) NULL, (PTR) NULL);
  immediate_quit--;
}


/* Debugging functions from xcoffread.c originally.   Some are redundant
   and should probably disappear.  */

static void
dump_strtbl ()
{
#if 0		/* We don't know its length FIXME */
  int ii;
  printf ("===STRING TABLE DUMP...\n\n");
  for ( ii=0; ii < strtbl_len; ++ii )
    printf ("%c", isprint (*(strtbl+ii)) ? *(strtbl+ii) : ' ');
  printf ("\n");
#endif
}

static void
dump_linetable (ltb)
     struct linetable *ltb;
{
  int ii;
  for (ii=0; ii < ltb->nitems; ++ii)
    printf ("line: %d, addr: 0x%x\n", ltb->item[ii].line, ltb->item[ii].pc);
}

static void
dump_type (typeP)
     struct type *typeP;
{
  printf ("0x%x: name: %s\n", typeP, typeP->name ? typeP->name : "(nil)");
}

static void
dump_symbol (pp)
     struct symbol *pp;
{
  printf (" sym: %s\t%s,\t%s\ttype: 0x%x, val: 0x%x end: 0x%x\n", 
      pp->name, dump_namespace (pp->namespace),
      dump_addrclass (pp->class), pp->type,
      SYMBOL_CLASS(pp) == LOC_BLOCK ? BLOCK_START(SYMBOL_BLOCK_VALUE(pp))
      : pp->value.value,
      SYMBOL_CLASS(pp) == LOC_BLOCK ? BLOCK_END(SYMBOL_BLOCK_VALUE(pp)) : 0);
}

static char *
dump_namespace (ns)
int ns;
{
  static char *ns_name [] = { 
    "UNDEF_NS", "VAR_NS", "STRUCT_NS", "LABEL_NS"};

  switch (ns) {
  case UNDEF_NAMESPACE:
  case VAR_NAMESPACE:
  case STRUCT_NAMESPACE:
  case LABEL_NAMESPACE:
    return ns_name[ns];
  }
 
  return "***ERROR***";
}

static char *
dump_addrclass (ac)
int ac;						/* address class */
{
  static char *ac_name [] = {
    "LOC_UNDEF",
    "LOC_CONST",
    "LOC_STATIC",
    "LOC_REGISTER",
    "LOC_ARG",
    "LOC_REF_ARG",
    "LOC_REGPARM",
    "LOC_LOCAL",
    "LOC_TYPEDEF",
    "LOC_LABEL",
    "LOC_BLOCK",
    "LOC_CONST_BYTES",
    "LOC_LOCAL_ARG",
  };
  switch (ac) {
  case LOC_UNDEF:
  case LOC_CONST:
  case LOC_STATIC:
  case LOC_REGISTER:
  case LOC_ARG:
  case LOC_REF_ARG:
  case LOC_REGPARM:
  case LOC_LOCAL:
  case LOC_TYPEDEF:
  case LOC_LABEL:
  case LOC_BLOCK:
  case LOC_CONST_BYTES:
  case LOC_LOCAL_ARG:
    return ac_name [ac];
  }
  return "***ERROR***";
}

static void
dump_block (pp)
     struct block *pp;
{
  int ii;
  printf ("BLOCK..: start: 0x%x, end: 0x%x\n", pp->startaddr, pp->endaddr);
  for (ii=0; ii < pp->nsyms; ++ii)
    dump_symbol (pp->sym[ii]);
}

static void
dump_blockvector (pp)
     struct blockvector *pp;
{
  int ii;
  for (ii=0; ii < pp->nblocks; ++ii)
    dump_block (pp->block [ii]);
}

static void
dump_last_symtab (pp)
     struct symtab *pp;
{
  for ( ; pp; pp = pp->next) {
    if ( pp->next == 0 ) {
      printf ("SYMTAB NAME: %s\n", pp->filename);
      dump_blockvector (pp->blockvector);
    }
  }
}

static void
dump_symtabs (pp)
     struct symtab *pp;
{
  for ( ; pp; pp = pp->next) {
    printf ("SYMTAB NAME: %s\n", pp->filename ? pp->filename : "(nil)");
/*    if (pp->linetable)
      dump_linetable (pp->linetable); */
    dump_blockvector (pp->blockvector);
  }
}

static void
dump_symtab_lines (pp)
     struct symtab *pp;
{
  for ( ; pp; pp = pp->next) {
    printf ("SYMTAB NAME: %s\n", pp->filename ? pp->filename : "(nil)");
    if (pp->linetable)
      dump_linetable (pp->linetable);
    /* dump_blockvector (pp->blockvector); */
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


/* Increase the space allocated for LISTP. */

void
extend_psymbol_list (listp, objfile)
     register struct psymbol_allocation_list *listp;
     struct objfile *objfile;
{
  int new_size;
  if (listp->size == 0)
    {
      new_size = 255;
      listp->list = (struct partial_symbol *)
	xmmalloc (objfile -> md, new_size * sizeof (struct partial_symbol));
    }
  else
    {
      new_size = listp->size * 2;
      listp->list = (struct partial_symbol *)
	xmrealloc (objfile -> md, (char *) listp->list,
		   new_size * sizeof (struct partial_symbol));
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
    extend_psymbol_list (listp, objfile);
  psym = listp -> next++;
  SYMBOL_NAME (psym) = (char *) obstack_alloc (&objfile->psymbol_obstack,
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
    extend_psymbol_list (listp, objfile);
  psym = listp -> next++;
  SYMBOL_NAME (psym) = (char *) obstack_alloc (&objfile->psymbol_obstack,
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
  add_com ("printmsyms", class_obscure, printmsyms_command,
	   "Print dump of current minimal symbol definitions to file OUTFILE.\n\
If a SOURCE file is specified, dump only that file's symbols.");
  add_com ("printpsyms", class_obscure, printpsyms_command,
	  "Print dump of current partial symbol definitions to file OUTFILE.\n\
If a SOURCE file is specified, dump only that file's partial symbols.");
  add_com ("printsyms", class_obscure, printsyms_command,
	   "Print dump of current symbol definitions to file OUTFILE.\n\
If a SOURCE file is specified, dump only that file's symbols.");
  add_com ("printobjfiles", class_obscure, printobjfiles_command,
	   "Print dump of current object file definitions.");
}

