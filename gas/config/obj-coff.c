/* coff object file format
   Copyright (C) 1989, 1990, 1991, 1992, 1993, 1994
   Free Software Foundation, Inc.

   This file is part of GAS.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to
   the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "as.h"
#include "obstack.h"
#include "subsegs.h"

static symbolS *tag_find_or_make PARAMS ((char *name));
static symbolS *tag_find PARAMS ((char *name));
static void tag_init PARAMS ((void));
static void tag_insert PARAMS ((const char *name, symbolS * symbolP));
const char *s_get_name PARAMS ((symbolS * s));

static void obj_coff_def PARAMS ((int));
static void obj_coff_dim PARAMS ((int));
static void obj_coff_endef PARAMS ((int));
static void obj_coff_line PARAMS ((int));
static void obj_coff_ln PARAMS ((int));
static void obj_coff_scl PARAMS ((int));
static void obj_coff_size PARAMS ((int));
static void obj_coff_tag PARAMS ((int));
static void obj_coff_type PARAMS ((int));
static void obj_coff_val PARAMS ((int));

static void SA_SET_SYM_TAGNDX PARAMS ((symbolS *, symbolS *));

static struct hash_control *tag_hash;
static symbolS *def_symbol_in_progress;

const pseudo_typeS obj_pseudo_table[] =
{
  {"def", obj_coff_def, 0},
  {"dim", obj_coff_dim, 0},
  {"endef", obj_coff_endef, 0},
  {"line", obj_coff_line, 0},
  {"ln", obj_coff_ln, 0},
  {"appline", obj_coff_ln, 1},
  {"scl", obj_coff_scl, 0},
  {"size", obj_coff_size, 0},
  {"tag", obj_coff_tag, 0},
  {"type", obj_coff_type, 0},
  {"val", obj_coff_val, 0},
  { "section", obj_coff_section, 0 },
  {"ident", s_ignore, 0},	/* we don't yet handle this. */
  {"optim", s_ignore, 0},	/* For sun386i cc (?) */
  /* other stuff */
  {"ABORT", s_abort, 0},

  {NULL}			/* end sentinel */
};				/* obj_pseudo_table */

/* stack stuff */
typedef struct
  {
    unsigned long chunk_size;
    unsigned long element_size;
    unsigned long size;
    char *data;
    unsigned long pointer;
  }
stack;

static stack *
stack_init (chunk_size, element_size)
     unsigned long chunk_size;
     unsigned long element_size;
{
  stack *st;

  st = (stack *) malloc (sizeof (stack));
  if (!st)
    return 0;
  st->data = malloc (chunk_size);
  if (!st->data)
    {
      free (st);
      return 0;
    }
  st->pointer = 0;
  st->size = chunk_size;
  st->chunk_size = chunk_size;
  st->element_size = element_size;
  return st;
}

static void
stack_delete (st)
     stack *st;
{
  free (st->data);
  free (st);
}

static char *
stack_push (st, element)
     stack *st;
     char *element;
{
  if (st->pointer + st->element_size >= st->size)
    {
      st->size += st->chunk_size;
      if ((st->data = xrealloc (st->data, st->size)) == (char *) 0)
	return (char *) 0;
    }
  memcpy (st->data + st->pointer, element, st->element_size);
  st->pointer += st->element_size;
  return st->data + st->pointer;
}

static char *
stack_pop (st)
     stack *st;
{
  if (st->pointer < st->element_size)
    {
      st->pointer = 0;
      return (char *) 0;
    }
  st->pointer -= st->element_size;
  return st->data + st->pointer;
}

/*
 * Maintain a list of the tagnames of the structres.
 */

static void
tag_init ()
{
  tag_hash = hash_new ();
}

static void
tag_insert (name, symbolP)
     const char *name;
     symbolS *symbolP;
{
  const char *error_string;

  if ((error_string = hash_jam (tag_hash, name, (char *) symbolP)))
    {
      as_fatal ("Inserting \"%s\" into structure table failed: %s",
		name, error_string);
    }
}

static symbolS *
tag_find_or_make (name)
     char *name;
{
  symbolS *symbolP;

  if ((symbolP = tag_find (name)) == NULL)
    {
      symbolP = symbol_new (name, undefined_section,
			    0, &zero_address_frag);

      tag_insert (S_GET_NAME (symbolP), symbolP);
      symbol_table_insert (symbolP);
    }				/* not found */

  return symbolP;
}

static symbolS *
tag_find (name)
     char *name;
{
#ifdef STRIP_UNDERSCORE
  if (*name == '_')
    name++;
#endif /* STRIP_UNDERSCORE */
  return (symbolS *) hash_find (tag_hash, name);
}

struct line_no {
  struct line_no *next;
  fragS *frag;
  alent l;
};

#define GET_FILENAME_STRING(X) \
((char*)(&((X)->sy_symbol.ost_auxent->x_file.x_n.x_offset))[1])

/* @@ Ick.  */
static segT
fetch_coff_debug_section ()
{
  static segT debug_section;
  if (!debug_section)
    {
      CONST asymbol *s;
      s = bfd_make_debug_symbol (stdoutput, (char *) 0, 0);
      assert (s != 0);
      debug_section = s->section;
    }
  return debug_section;
}

void
SA_SET_SYM_ENDNDX (sym, val)
     symbolS *sym;
     symbolS *val;
{
  combined_entry_type *entry, *p;

  entry = &coffsymbol (sym->bsym)->native[1];
  p = coffsymbol (val->bsym)->native;
  entry->u.auxent.x_sym.x_fcnary.x_fcn.x_endndx.p = p;
  entry->fix_end = 1;
}

static void
SA_SET_SYM_TAGNDX (sym, val)
     symbolS *sym;
     symbolS *val;
{
  combined_entry_type *entry, *p;

  entry = &coffsymbol (sym->bsym)->native[1];
  p = coffsymbol (val->bsym)->native;
  entry->u.auxent.x_sym.x_tagndx.p = p;
  entry->fix_tag = 1;
}

static int
S_GET_DATA_TYPE (sym)
     symbolS *sym;
{
  return coffsymbol (sym->bsym)->native->u.syment.n_type;
}

int
S_SET_DATA_TYPE (sym, val)
     symbolS *sym;
     int val;
{
  coffsymbol (sym->bsym)->native->u.syment.n_type = val;
  return val;
}

int
S_GET_STORAGE_CLASS (sym)
     symbolS *sym;
{
  return coffsymbol (sym->bsym)->native->u.syment.n_sclass;
}

int
S_SET_STORAGE_CLASS (sym, val)
     symbolS *sym;
     int val;
{
  coffsymbol (sym->bsym)->native->u.syment.n_sclass = val;
  return val;
}

/* Merge a debug symbol containing debug information into a normal symbol. */

void
c_symbol_merge (debug, normal)
     symbolS *debug;
     symbolS *normal;
{
  S_SET_DATA_TYPE (normal, S_GET_DATA_TYPE (debug));
  S_SET_STORAGE_CLASS (normal, S_GET_STORAGE_CLASS (debug));

  if (S_GET_NUMBER_AUXILIARY (debug) > S_GET_NUMBER_AUXILIARY (normal))
    /* take the most we have */
    S_SET_NUMBER_AUXILIARY (normal, S_GET_NUMBER_AUXILIARY (debug));

  if (S_GET_NUMBER_AUXILIARY (debug) > 0)
    {
      /* Move all the auxiliary information.  */
      /* @@ How many fields do we want to preserve?  Would it make more
	 sense to pick and choose those we want to copy?  Should look
	 into this further....  [raeburn:19920512.2209EST]  */
      alent *linenos;
      linenos = coffsymbol (normal->bsym)->lineno;
      memcpy ((char *) &coffsymbol (normal->bsym)->native,
	      (char *) &coffsymbol (debug->bsym)->native,
	      S_GET_NUMBER_AUXILIARY(debug) * AUXESZ);
      coffsymbol (normal->bsym)->lineno = linenos;
    }

  /* Move the debug flags. */
  SF_SET_DEBUG_FIELD (normal, SF_GET_DEBUG_FIELD (debug));
}

static symbolS *previous_file_symbol;
void
c_dot_file_symbol (filename)
     char *filename;
{
  symbolS *symbolP;

  symbolP = symbol_new (filename, &bfd_abs_section, 0,
			&zero_address_frag);

  S_SET_STORAGE_CLASS (symbolP, C_FILE);
  S_SET_NUMBER_AUXILIARY (symbolP, 1);

  symbolP->bsym->flags = BSF_DEBUGGING;

#ifndef NO_LISTING
  {
    extern int listing;
    if (listing)
      {
	listing_source_file (filename);
      }
  }
#endif

  S_SET_VALUE (symbolP, (long) previous_file_symbol);

  previous_file_symbol = symbolP;

  /* Make sure that the symbol is first on the symbol chain */
  if (symbol_rootP != symbolP)
    {
      if (symbolP == symbol_lastP)
	{
	  symbol_lastP = symbol_lastP->sy_previous;
	}			/* if it was the last thing on the list */

      symbol_remove (symbolP, &symbol_rootP, &symbol_lastP);
      symbol_insert (symbolP, symbol_rootP, &symbol_rootP, &symbol_lastP);
      symbol_rootP = symbolP;
    }				/* if not first on the list */
}

/*
 * Build a 'section static' symbol.
 */

char *
c_section_symbol (name, value, length, nreloc, nlnno)
     char *name;
     long value;
     long length;
     unsigned short nreloc;
     unsigned short nlnno;
{
  symbolS *symbolP;

  symbolP = symbol_new (name,
			(name[1] == 't'
			 ? text_section
			 : name[1] == 'd'
			 ? data_section
			 : bss_section),
			value,
			&zero_address_frag);

  S_SET_STORAGE_CLASS (symbolP, C_STAT);
  S_SET_NUMBER_AUXILIARY (symbolP, 1);

  SA_SET_SCN_SCNLEN (symbolP, length);
  SA_SET_SCN_NRELOC (symbolP, nreloc);
  SA_SET_SCN_NLINNO (symbolP, nlnno);

  SF_SET_STATICS (symbolP);

  return (char *) symbolP;
}

/* Line number handling */

int coff_line_base;

/* Symbol of last function, which we should hang line#s off of.  */
static symbolS *line_fsym;

#define in_function()		(line_fsym != 0)
#define clear_function()	(line_fsym = 0)
#define set_function(F)		(line_fsym = (F), coff_add_linesym (F))


void
obj_symbol_new_hook (symbolP)
     symbolS *symbolP;
{
  char underscore = 0;		/* Symbol has leading _ */

  {
    long sz = (OBJ_COFF_MAX_AUXENTRIES + 1) * sizeof (combined_entry_type);
    char *s = (char *) bfd_alloc_by_size_t (stdoutput, sz);
    memset (s, 0, sz);
    coffsymbol (symbolP->bsym)->native = (combined_entry_type *) s;
  }
  S_SET_DATA_TYPE (symbolP, T_NULL);
  S_SET_STORAGE_CLASS (symbolP, 0);
  S_SET_NUMBER_AUXILIARY (symbolP, 0);

  if (S_IS_STRING (symbolP))
    SF_SET_STRING (symbolP);
  if (!underscore && S_IS_LOCAL (symbolP))
    SF_SET_LOCAL (symbolP);
}


/*
 * Handle .ln directives.
 */

static symbolS *current_lineno_sym;
static struct line_no *line_nos;

static void
add_lineno (frag, offset, num)
     fragS *frag;
     int offset;
     int num;
{
  struct line_no *new_line = (struct line_no *) bfd_alloc_by_size_t (stdoutput,
								     sizeof (struct line_no));
  if (!current_lineno_sym)
    {
      abort ();
    }
  new_line->next = line_nos;
  new_line->frag = frag;
  new_line->l.line_number = num;
  new_line->l.u.offset = offset;
  line_nos = new_line;
}

void
coff_add_linesym (sym)
     symbolS *sym;
{
  if (line_nos)
    {
      add_lineno (0, 0, 0);
      coffsymbol (current_lineno_sym->bsym)->lineno = (alent *) line_nos;
      line_nos = 0;
    }
  current_lineno_sym = sym;
}

static void
obj_coff_ln (appline)
     int appline;
{
  int l;

  if (! appline && def_symbol_in_progress != NULL)
    {
      as_warn (".ln pseudo-op inside .def/.endef: ignored.");
      demand_empty_rest_of_line ();
      return;
    }

  l = get_absolute_expression ();
  if (!appline)
    {
      add_lineno (frag_now, frag_now_fix (), l);
    }

#ifndef NO_LISTING
  {
    extern int listing;

    if (listing)
      {
	if (! appline)
	  l += coff_line_base - 1;
	listing_source_line (l);
      }
  }
#endif

  demand_empty_rest_of_line ();
}

/*
 *			def()
 *
 * Handle .def directives.
 *
 * One might ask : why can't we symbol_new if the symbol does not
 * already exist and fill it with debug information.  Because of
 * the C_EFCN special symbol. It would clobber the value of the
 * function symbol before we have a chance to notice that it is
 * a C_EFCN. And a second reason is that the code is more clear this
 * way. (at least I think it is :-).
 *
 */

#define SKIP_SEMI_COLON()	while (*input_line_pointer++ != ';')
#define SKIP_WHITESPACES()	while (*input_line_pointer == ' ' || \
				       *input_line_pointer == '\t') \
    input_line_pointer++;

static void
obj_coff_def (what)
     int what;
{
  char name_end;		/* Char after the end of name */
  char *symbol_name;		/* Name of the debug symbol */
  char *symbol_name_copy;	/* Temporary copy of the name */
  unsigned int symbol_name_length;

  if (def_symbol_in_progress != NULL)
    {
      as_warn (".def pseudo-op used inside of .def/.endef: ignored.");
      demand_empty_rest_of_line ();
      return;
    }				/* if not inside .def/.endef */

  SKIP_WHITESPACES ();

  symbol_name = input_line_pointer;
#ifdef STRIP_UNDERSCORE
  if (symbol_name[0] == '_' && symbol_name[1] != 0)
    symbol_name++;
#endif /* STRIP_UNDERSCORE */

  name_end = get_symbol_end ();
  symbol_name_length = strlen (symbol_name);
  symbol_name_copy = xmalloc (symbol_name_length + 1);
  strcpy (symbol_name_copy, symbol_name);

  /* Initialize the new symbol */
  def_symbol_in_progress = symbol_make (symbol_name_copy);
  def_symbol_in_progress->sy_frag = &zero_address_frag;
  S_SET_VALUE (def_symbol_in_progress, 0);

  if (S_IS_STRING (def_symbol_in_progress))
    SF_SET_STRING (def_symbol_in_progress);

  *input_line_pointer = name_end;

  demand_empty_rest_of_line ();
}

unsigned int dim_index;

static void
obj_coff_endef (ignore)
     int ignore;
{
  symbolS *symbolP;
  /* DIM BUG FIX sac@cygnus.com */
  dim_index = 0;
  if (def_symbol_in_progress == NULL)
    {
      as_warn (".endef pseudo-op used outside of .def/.endef: ignored.");
      demand_empty_rest_of_line ();
      return;
    }				/* if not inside .def/.endef */

  /* Set the section number according to storage class. */
  switch (S_GET_STORAGE_CLASS (def_symbol_in_progress))
    {
    case C_STRTAG:
    case C_ENTAG:
    case C_UNTAG:
      SF_SET_TAG (def_symbol_in_progress);
      /* intentional fallthrough */
    case C_FILE:
    case C_TPDEF:
      SF_SET_DEBUG (def_symbol_in_progress);
      S_SET_SEGMENT (def_symbol_in_progress, fetch_coff_debug_section ());
      break;

    case C_EFCN:
      SF_SET_LOCAL (def_symbol_in_progress);	/* Do not emit this symbol. */
      /* intentional fallthrough */
    case C_BLOCK:
      SF_SET_PROCESS (def_symbol_in_progress);	/* Will need processing before writing */
      /* intentional fallthrough */
    case C_FCN:
      {
	CONST char *name;
	S_SET_SEGMENT (def_symbol_in_progress, text_section);

	name = bfd_asymbol_name (def_symbol_in_progress->bsym);
	if (name[1] == 'b' && name[2] == 'f')
	  {
	    if (! in_function ())
	      as_warn ("`%s' symbol without preceding function", name);
/*	    SA_SET_SYM_LNNO (def_symbol_in_progress, 12345);*/
	    /* Will need relocating */
	    SF_SET_PROCESS (def_symbol_in_progress);
	    clear_function ();
	  }
      }
      break;

#ifdef C_AUTOARG
    case C_AUTOARG:
#endif /* C_AUTOARG */
    case C_AUTO:
    case C_REG:
    case C_MOS:
    case C_MOE:
    case C_MOU:
    case C_ARG:
    case C_REGPARM:
    case C_FIELD:
    case C_EOS:
      SF_SET_DEBUG (def_symbol_in_progress);
      S_SET_SEGMENT (def_symbol_in_progress, absolute_section);
      break;

    case C_EXT:
    case C_STAT:
    case C_LABEL:
      /* Valid but set somewhere else (s_comm, s_lcomm, colon) */
      break;

    case C_USTATIC:
    case C_EXTDEF:
    case C_ULABEL:
      as_warn ("unexpected storage class %d",
	       S_GET_STORAGE_CLASS (def_symbol_in_progress));
      break;
    }				/* switch on storage class */

  /* Now that we have built a debug symbol, try to find if we should
     merge with an existing symbol or not.  If a symbol is C_EFCN or
     SEG_ABSOLUTE or untagged SEG_DEBUG it never merges. */

  /* Two cases for functions.  Either debug followed by definition or
     definition followed by debug.  For definition first, we will
     merge the debug symbol into the definition.  For debug first, the
     lineno entry MUST point to the definition function or else it
     will point off into space when obj_crawl_symbol_chain() merges
     the debug symbol into the real symbol.  Therefor, let's presume
     the debug symbol is a real function reference. */

  /* FIXME-SOON If for some reason the definition label/symbol is
     never seen, this will probably leave an undefined symbol at link
     time. */

  if (S_GET_STORAGE_CLASS (def_symbol_in_progress) == C_EFCN
      || (!strcmp (bfd_get_section_name (stdoutput,
					 S_GET_SEGMENT (def_symbol_in_progress)),
		   "*DEBUG*")
	  && !SF_GET_TAG (def_symbol_in_progress))
      || S_GET_SEGMENT (def_symbol_in_progress) == absolute_section
      || (symbolP = symbol_find_base (S_GET_NAME (def_symbol_in_progress), DO_NOT_STRIP)) == NULL)
    {
      if (def_symbol_in_progress != symbol_lastP)
	symbol_append (def_symbol_in_progress, symbol_lastP, &symbol_rootP,
		       &symbol_lastP);
    }
  else
    {
      /* This symbol already exists, merge the newly created symbol
	 into the old one.  This is not mandatory. The linker can
	 handle duplicate symbols correctly. But I guess that it save
	 a *lot* of space if the assembly file defines a lot of
	 symbols. [loic] */

      /* The debug entry (def_symbol_in_progress) is merged into the
	 previous definition. */

      c_symbol_merge (def_symbol_in_progress, symbolP);
      /* FIXME-SOON Should *def_symbol_in_progress be free'd? xoxorich. */
      def_symbol_in_progress = symbolP;

      if (SF_GET_FUNCTION (def_symbol_in_progress)
	  || SF_GET_TAG (def_symbol_in_progress))
	{
	  /* For functions, and tags, the symbol *must* be where the
	     debug symbol appears.  Move the existing symbol to the
	     current place. */
	  /* If it already is at the end of the symbol list, do nothing */
	  if (def_symbol_in_progress != symbol_lastP)
	    {
	      symbol_remove (def_symbol_in_progress, &symbol_rootP, &symbol_lastP);
	      symbol_append (def_symbol_in_progress, symbol_lastP, &symbol_rootP, &symbol_lastP);
	    }
	}
    }

  if (SF_GET_TAG (def_symbol_in_progress)
      && symbol_find_base (S_GET_NAME (def_symbol_in_progress), DO_NOT_STRIP) == NULL)
    {
      tag_insert (S_GET_NAME (def_symbol_in_progress), def_symbol_in_progress);
    }

  if (SF_GET_FUNCTION (def_symbol_in_progress))
    {
      know (sizeof (def_symbol_in_progress) <= sizeof (long));
      set_function (def_symbol_in_progress);
      SF_SET_PROCESS (def_symbol_in_progress);

      if (symbolP == NULL)
	{
	  /* That is, if this is the first time we've seen the
	     function... */
	  symbol_table_insert (def_symbol_in_progress);
	} /* definition follows debug */
    } /* Create the line number entry pointing to the function being defined */

  def_symbol_in_progress = NULL;
  demand_empty_rest_of_line ();
}

static void
obj_coff_dim (ignore)
     int ignore;
{
  int dim_index;

  if (def_symbol_in_progress == NULL)
    {
      as_warn (".dim pseudo-op used outside of .def/.endef: ignored.");
      demand_empty_rest_of_line ();
      return;
    }				/* if not inside .def/.endef */

  S_SET_NUMBER_AUXILIARY (def_symbol_in_progress, 1);

  for (dim_index = 0; dim_index < DIMNUM; dim_index++)
    {
      SKIP_WHITESPACES ();
      SA_SET_SYM_DIMEN (def_symbol_in_progress, dim_index,
			get_absolute_expression ());

      switch (*input_line_pointer)
	{
	case ',':
	  input_line_pointer++;
	  break;

	default:
	  as_warn ("badly formed .dim directive ignored");
	  /* intentional fallthrough */
	case '\n':
	case ';':
	  dim_index = DIMNUM;
	  break;
	}
    }

  demand_empty_rest_of_line ();
}

static void
obj_coff_line (ignore)
     int ignore;
{
  int this_base;

  if (def_symbol_in_progress == NULL)
    {
      /* Probably stabs-style line?  */
      obj_coff_ln (0);
      return;
    }

  this_base = get_absolute_expression ();
  if (this_base > coff_line_base)
    coff_line_base = this_base;

  S_SET_NUMBER_AUXILIARY (def_symbol_in_progress, 1);
  SA_SET_SYM_LNNO (def_symbol_in_progress, coff_line_base);

  demand_empty_rest_of_line ();
}

static void
obj_coff_size (ignore)
     int ignore;
{
  if (def_symbol_in_progress == NULL)
    {
      as_warn (".size pseudo-op used outside of .def/.endef ignored.");
      demand_empty_rest_of_line ();
      return;
    }				/* if not inside .def/.endef */

  S_SET_NUMBER_AUXILIARY (def_symbol_in_progress, 1);
  SA_SET_SYM_SIZE (def_symbol_in_progress, get_absolute_expression ());
  demand_empty_rest_of_line ();
}

static void
obj_coff_scl (ignore)
     int ignore;
{
  if (def_symbol_in_progress == NULL)
    {
      as_warn (".scl pseudo-op used outside of .def/.endef ignored.");
      demand_empty_rest_of_line ();
      return;
    }				/* if not inside .def/.endef */

  S_SET_STORAGE_CLASS (def_symbol_in_progress, get_absolute_expression ());
  demand_empty_rest_of_line ();
}

static void
obj_coff_tag (ignore)
     int ignore;
{
  char *symbol_name;
  char name_end;

  if (def_symbol_in_progress == NULL)
    {
      as_warn (".tag pseudo-op used outside of .def/.endef ignored.");
      demand_empty_rest_of_line ();
      return;
    }

  S_SET_NUMBER_AUXILIARY (def_symbol_in_progress, 1);
  symbol_name = input_line_pointer;
  name_end = get_symbol_end ();

  /* Assume that the symbol referred to by .tag is always defined.
     This was a bad assumption.  I've added find_or_make. xoxorich. */
  SA_SET_SYM_TAGNDX (def_symbol_in_progress,
		     tag_find_or_make (symbol_name));
  if (SA_GET_SYM_TAGNDX (def_symbol_in_progress) == 0L)
    {
      as_warn ("tag not found for .tag %s", symbol_name);
    }				/* not defined */

  SF_SET_TAGGED (def_symbol_in_progress);
  *input_line_pointer = name_end;

  demand_empty_rest_of_line ();
}

static void
obj_coff_type (ignore)
     int ignore;
{
  if (def_symbol_in_progress == NULL)
    {
      as_warn (".type pseudo-op used outside of .def/.endef ignored.");
      demand_empty_rest_of_line ();
      return;
    }				/* if not inside .def/.endef */

  S_SET_DATA_TYPE (def_symbol_in_progress, get_absolute_expression ());

  if (ISFCN (S_GET_DATA_TYPE (def_symbol_in_progress)) &&
      S_GET_STORAGE_CLASS (def_symbol_in_progress) != C_TPDEF)
    {
      SF_SET_FUNCTION (def_symbol_in_progress);
    }				/* is a function */

  demand_empty_rest_of_line ();
}

static void
obj_coff_val (ignore)
     int ignore;
{
  if (def_symbol_in_progress == NULL)
    {
      as_warn (".val pseudo-op used outside of .def/.endef ignored.");
      demand_empty_rest_of_line ();
      return;
    }				/* if not inside .def/.endef */

  if (is_name_beginner (*input_line_pointer))
    {
      char *symbol_name = input_line_pointer;
      char name_end = get_symbol_end ();

      if (!strcmp (symbol_name, "."))
	{
	  def_symbol_in_progress->sy_frag = frag_now;
	  S_SET_VALUE (def_symbol_in_progress, (valueT) frag_now_fix ());
	  /* If the .val is != from the .def (e.g. statics) */
	}
      else if (strcmp (S_GET_NAME (def_symbol_in_progress), symbol_name))
	{
	  def_symbol_in_progress->sy_value.X_op = O_symbol;
	  def_symbol_in_progress->sy_value.X_add_symbol =
	    symbol_find_or_make (symbol_name);
	  def_symbol_in_progress->sy_value.X_op_symbol = NULL;
	  def_symbol_in_progress->sy_value.X_add_number = 0;

	  /* If the segment is undefined when the forward reference is
	     resolved, then copy the segment id from the forward
	     symbol.  */
	  SF_SET_GET_SEGMENT (def_symbol_in_progress);
	}
      /* Otherwise, it is the name of a non debug symbol and its value will be calculated later. */
      *input_line_pointer = name_end;
    }
  else
    {
      S_SET_VALUE (def_symbol_in_progress, get_absolute_expression ());
    }				/* if symbol based */

  demand_empty_rest_of_line ();
}

void
obj_read_begin_hook ()
{
  /* These had better be the same.  Usually 18 bytes. */
#ifndef BFD_HEADERS
  know (sizeof (SYMENT) == sizeof (AUXENT));
  know (SYMESZ == AUXESZ);
#endif
  tag_init ();
}


symbolS *coff_last_function;

void
coff_frob_symbol (symp, punt)
     symbolS *symp;
     int *punt;
{
  static symbolS *last_tagP;
  static stack *block_stack;
  static symbolS *set_end;

  if (symp == &abs_symbol)
    {
      *punt = 1;
      return;
    }

  if (current_lineno_sym)
    coff_add_linesym ((symbolS *) 0);

  if (!block_stack)
    block_stack = stack_init (512, sizeof (symbolS*));

  if (!S_IS_DEFINED (symp) && S_GET_STORAGE_CLASS (symp) != C_STAT)
    S_SET_STORAGE_CLASS (symp, C_EXT);

  if (!SF_GET_DEBUG (symp))
    {
      symbolS *real;
      if (!SF_GET_LOCAL (symp)
	  && (real = symbol_find_base (S_GET_NAME (symp), DO_NOT_STRIP))
	  && real != symp)
	{
	  c_symbol_merge (symp, real);
	  *punt = 1;
	}
      if (!S_IS_DEFINED (symp) && !SF_GET_LOCAL (symp))
	{
	  assert (S_GET_VALUE (symp) == 0);
	  S_SET_EXTERNAL (symp);
	}
      else if (S_GET_STORAGE_CLASS (symp) == C_NULL)
	{
	  if (S_GET_SEGMENT (symp) == text_section)
	    S_SET_STORAGE_CLASS (symp, C_LABEL);
	  else
	    S_SET_STORAGE_CLASS (symp, C_STAT);
	}
      if (SF_GET_PROCESS (symp))
	{
	  if (S_GET_STORAGE_CLASS (symp) == C_BLOCK)
	    {
	      if (!strcmp (S_GET_NAME (symp), ".bb"))
		stack_push (block_stack, (char *) &symp);
	      else
		{
		  symbolS *begin;
		  begin = *(symbolS **) stack_pop (block_stack);
		  if (begin == 0)
		    as_warn ("mismatched .eb");
		  else
		    set_end = begin;
		}
	    }
	  if (coff_last_function == 0 && SF_GET_FUNCTION (symp))
	    {
	      union internal_auxent *auxp;
	      coff_last_function = symp;
	      if (S_GET_NUMBER_AUXILIARY (symp) < 1)
		S_SET_NUMBER_AUXILIARY (symp, 1);
	      auxp = &coffsymbol (symp->bsym)->native[1].u.auxent;
	      memset (auxp->x_sym.x_fcnary.x_ary.x_dimen, 0,
		      sizeof (auxp->x_sym.x_fcnary.x_ary.x_dimen));
	    }
	  if (S_GET_STORAGE_CLASS (symp) == C_EFCN)
	    {
	      if (coff_last_function == 0)
		as_fatal ("C_EFCN symbol out of scope");
	      SA_SET_SYM_FSIZE (coff_last_function,
				(long) (S_GET_VALUE (symp)
					- S_GET_VALUE (coff_last_function)));
	      set_end = coff_last_function;
	      coff_last_function = 0;
	    }
	}
      else if (SF_GET_TAG (symp))
	last_tagP = symp;
      else if (S_GET_STORAGE_CLASS (symp) == C_EOS)
	set_end = last_tagP;
      else if (S_GET_STORAGE_CLASS (symp) == C_FILE)
	{
	  if (S_GET_VALUE (symp))
	    {
	      S_SET_VALUE ((symbolS *) S_GET_VALUE (symp), 0xdeadbeef);
	      S_SET_VALUE (symp, 0);
	    }
	}
      if (S_IS_EXTERNAL (symp))
	S_SET_STORAGE_CLASS (symp, C_EXT);
      else if (SF_GET_LOCAL (symp))
	*punt = 1;
      /* more ... */
    }

  if (set_end != (symbolS *) NULL
      && ! *punt)
    {
      SA_SET_SYM_ENDNDX (set_end, symp);
      set_end = NULL;
    }

  if (coffsymbol (symp->bsym)->lineno)
    {
      int i, n;
      struct line_no *lptr;
      alent *l;

      lptr = (struct line_no *) coffsymbol (symp->bsym)->lineno;
      for (i = 0; lptr; lptr = lptr->next)
	i++;
      n = i + 1;
      lptr = (struct line_no *) coffsymbol (symp->bsym)->lineno;
      l = (alent *) bfd_alloc_by_size_t (stdoutput, n * sizeof (alent));
      coffsymbol (symp->bsym)->lineno = l;
      for (i = n - 1; i > 0; i--)
	{
	  if (lptr->frag)
	    lptr->l.u.offset += lptr->frag->fr_address;
	  l[i] = lptr->l;
	  lptr = lptr->next;
	}
    }
}

/*
 * implement the .section pseudo op:
 *	.section name {, "flags"}
 *                ^         ^
 *                |         +--- optional flags: 'b' for bss
 *                |                              'i' for info
 *                +-- section name               'l' for lib
 *                                               'n' for noload
 *                                               'o' for over
 *                                               'w' for data
 *						 'd' (apparently m88k for data)
 *                                               'x' for text
 * But if the argument is not a quoted string, treat it as a
 * subsegment number.
 */

void
obj_coff_section (ignore)
     int ignore;
{
  /* Strip out the section name */
  char *section_name;
  char c;
  char *name;
  unsigned int exp;
  flagword flags;
  asection *sec;

  section_name = input_line_pointer;
  c = get_symbol_end ();

  name = xmalloc (input_line_pointer - section_name + 1);
  strcpy (name, section_name);

  *input_line_pointer = c;

  SKIP_WHITESPACE ();

  exp = 0;
  flags = SEC_NO_FLAGS;

  if (*input_line_pointer == ',')
    {
      ++input_line_pointer;
      SKIP_WHITESPACE ();
      if (*input_line_pointer != '"')
	exp = get_absolute_expression ();
      else
	{
	  ++input_line_pointer;
	  while (*input_line_pointer != '"'
		 && ! is_end_of_line[(unsigned char) *input_line_pointer])
	    {
	      switch (*input_line_pointer)
		{
		case 'b': flags |= SEC_ALLOC; flags &=~ SEC_LOAD; break;
		case 'n': flags &=~ SEC_LOAD; break;
		case 'd':
		case 'w': flags &=~ SEC_READONLY; break;
		case 'x': flags |= SEC_CODE; break;

		case 'i': /* STYP_INFO */
		case 'l': /* STYP_LIB */
		case 'o': /* STYP_OVER */
		  as_warn ("unsupported section attribute '%c'",
			   *input_line_pointer);
		  break;

		default:
		  as_warn("unknown section attribute '%c'",
			  *input_line_pointer);
		  break;
		}
	      ++input_line_pointer;
	    }
	  if (*input_line_pointer == '"')
	    ++input_line_pointer;
	}
    }

  sec = subseg_new (name, (subsegT) exp);

  if (flags != SEC_NO_FLAGS)
    {
      if (! bfd_set_section_flags (stdoutput, sec, flags))
	as_warn ("error setting flags for \"%s\": %s",
		 bfd_section_name (stdoutput, sec),
		 bfd_errmsg (bfd_get_error ()));
    }
}

void
coff_frob_file ()
{
  if (symbol_rootP == NULL
      || S_GET_STORAGE_CLASS (symbol_rootP) != C_FILE)
    {
      assert (previous_file_symbol == 0);
      c_dot_file_symbol ("fake");
    }
}

void
coff_frob_section (sec)
     segT sec;
{
  segT strsec;
  char *strname, *p;
  fragS *fragp;
  bfd_vma size, n_entries, mask;

  /* The COFF back end in BFD requires that all section sizes be
     rounded up to multiples of the corresponding section alignments.
     Seems kinda silly to me, but that's the way it is.  */
  size = bfd_get_section_size_before_reloc (sec);
  assert (sec->alignment_power >= stdoutput->xvec->align_power_min);
  mask = ((bfd_vma) 1 << (bfd_vma) sec->alignment_power) - 1;
  if (size & mask)
    bfd_set_section_size (stdoutput, sec, (size + mask) & ~mask);

  /* @@ these should be in a "stabs.h" file, or maybe as.h */
#ifndef STAB_SECTION_NAME
#define STAB_SECTION_NAME ".stab"
#endif
#ifndef STAB_STRING_SECTION_NAME
#define STAB_STRING_SECTION_NAME ".stabstr"
#endif
  if (strcmp (STAB_STRING_SECTION_NAME, sec->name))
    return;

  strsec = sec;
  sec = subseg_get (STAB_SECTION_NAME, 0);
  /* size is already rounded up, since other section will be listed first */
  size = bfd_get_section_size_before_reloc (strsec);

  n_entries = bfd_get_section_size_before_reloc (sec) / 12 - 1;

  /* Find first non-empty frag.  It should be large enough.  */
  fragp = seg_info (sec)->frchainP->frch_root;
  while (fragp && fragp->fr_fix == 0)
    fragp = fragp->fr_next;
  assert (fragp != 0 && fragp->fr_fix >= 12);

  /* Store the values.  */
  p = fragp->fr_literal;
  bfd_h_put_16 (stdoutput, n_entries, (bfd_byte *) p + 6);
  bfd_h_put_32 (stdoutput, size, (bfd_byte *) p + 8);
}

void
obj_coff_init_stab_section (seg)
     segT seg;
{
  char *file;
  char *p;
  char *stabstr_name;
  unsigned int stroff;

  /* Make space for this first symbol. */
  p = frag_more (12);
  /* Zero it out. */
  memset (p, 0, 12);
  as_where (&file, (unsigned int *) NULL);
  stabstr_name = (char *) alloca (strlen (seg->name) + 4);
  strcpy (stabstr_name, seg->name);
  strcat (stabstr_name, "str");
  stroff = get_stab_string_offset (file, stabstr_name);
  know (stroff == 1);
  md_number_to_chars (p, stroff, 4);
}

#ifdef DEBUG
/* for debugging */
const char *
s_get_name (s)
     symbolS *s;
{
  return ((s == NULL) ? "(NULL)" : S_GET_NAME (s));
}

void
symbol_dump ()
{
  symbolS *symbolP;

  for (symbolP = symbol_rootP; symbolP; symbolP = symbol_next (symbolP))
    {
      printf("0x%lx: \"%s\" type = %ld, class = %d, segment = %d\n",
	     (unsigned long) symbolP,
	     S_GET_NAME(symbolP),
	     (long) S_GET_DATA_TYPE(symbolP),
	     S_GET_STORAGE_CLASS(symbolP),
	     (int) S_GET_SEGMENT(symbolP));
    }
}

#endif /* DEBUG */

/* end of obj-coff.c */
