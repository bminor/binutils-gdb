/* ELF object file format
   Copyright (C) 1992, 1993 Free Software Foundation, Inc.

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2,
   or (at your option) any later version.

   GAS is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
   the GNU General Public License for more details.

   You should have received a copy of the GNU General Public
   License along with GAS; see the file COPYING.  If not, write
   to the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA. */

#include "as.h"
#include "subsegs.h"
#include "aout/stab_gnu.h"
#include "obstack.h"

static void obj_elf_stab PARAMS ((int what));
static void obj_elf_xstab PARAMS ((int what));
static void obj_elf_line PARAMS ((void));
void obj_elf_desc PARAMS ((void));
void obj_elf_version PARAMS ((void));
static void obj_elf_size PARAMS ((void));
static void obj_elf_type PARAMS ((void));
static void obj_elf_ident PARAMS ((void));

const pseudo_typeS obj_pseudo_table[] =
{
  {"ident", obj_elf_ident, 0},
  {"previous", obj_elf_previous, 0},
  {"section", obj_elf_section, 0},
  {"size", obj_elf_size, 0},
  {"type", obj_elf_type, 0},
  {"version", obj_elf_version, 0},

/* These are used for stabs-in-elf configurations.  */
  {"desc", obj_elf_desc, 0},
  {"line", obj_elf_line, 0},
  {"stabd", obj_elf_stab, 'd'},
  {"stabn", obj_elf_stab, 'n'},
  {"stabs", obj_elf_stab, 's'},
/* This is used on Solaris 2.x on SPARC, but not supported yet.  */
  {"xstabs", obj_elf_xstab, 's'},

  /* These are used for dwarf. */
  {"2byte", cons, 2},
  {"4byte", cons, 4},

  {NULL}			/* end sentinel */
};

#include "aout/aout64.h"

void
elf_file_symbol (s)
     char *s;
{
  symbolS *sym;

  sym = symbol_new (s, absolute_section, (valueT) 0, (struct frag *) 0);
  sym->sy_frag = &zero_address_frag;
  sym->bsym->flags |= BSF_FILE;

  if (symbol_rootP != sym)
    {
      symbol_remove (sym, &symbol_rootP, &symbol_lastP);
      symbol_insert (sym, symbol_rootP, &symbol_rootP, &symbol_lastP);
#ifdef DEBUG
      verify_symbol_chain (symbol_rootP, symbol_lastP);
#endif
    }
}

static segT previous_section;
static int previous_subsection;

void
obj_elf_section (xxx)
     int xxx;
{
  char *string;
  asection *sec;

  /* Initialize this with inclusive-or of all flags that can be cleared
     by attributes, but not set by them.  Also include flags that won't
     get set properly in the assembler, but which the user/compiler
     shouldn't be expected to set.  */
  flagword flags = SEC_READONLY | SEC_ALLOC | SEC_RELOC;
  /* Initialize this with the default flags to be used if none are
     specified.  */
  flagword default_flags = 0;

  SKIP_WHITESPACE ();
  if (*input_line_pointer == '"')
    string = demand_copy_C_string (&xxx);
  else
    {
      char *p = input_line_pointer;
      char c;
      while (0 == strchr ("\n\t,; ", *p))
	p++;
      c = *p;
      *p = 0;
      string = xmalloc (p - input_line_pointer + 1);
      strcpy (string, input_line_pointer);
      *p = c;
      input_line_pointer = p;
    }
  if (!strcmp (string, ".rodata"))
    default_flags = SEC_ALLOC | SEC_READONLY | SEC_RELOC;
  else if (!strcmp (string, ".init")
	   || !strcmp (string, ".fini"))
    default_flags = SEC_ALLOC | SEC_READONLY | SEC_RELOC | SEC_CODE;
  SKIP_WHITESPACE ();
  if (*input_line_pointer != ',')
    flags = default_flags;
  while (*input_line_pointer == ',')
    {
      flagword bit;
      int len, inv;
      char *p, oldp;

      input_line_pointer++;
      if (*input_line_pointer != '#' && *input_line_pointer != '@')
	{
	  as_bad ("unrecognized syntax in .section command");
	  ignore_rest_of_line ();
	  break;
	}
      input_line_pointer++;

#define CHECK(X,BIT,NEG)	\
      if (!strncmp(X,input_line_pointer,len = sizeof(X) - 1)) { \
	bit = BIT; inv = NEG; goto match; }

      CHECK ("write", SEC_READONLY, 1);
      CHECK ("alloc", SEC_ALLOC, 0);
      CHECK ("execinstr", SEC_CODE, 1);
#undef CHECK

      p = input_line_pointer;
      while (!is_end_of_line[*p] && *p != 0 && *p != ',')
	p++;
      *p = 0;
      oldp = *p;
      as_bad ("unrecognized section attribute `%s' ignored",
	      input_line_pointer);
      *p = oldp;
      continue;

    match:
      if (inv)
	flags &= ~bit;
      else
	flags |= bit;
      input_line_pointer += len;
    }
  demand_empty_rest_of_line ();

  /* If the C string wasn't valid, `string' could be null.  */
  if (!string)
    return;

  sec = bfd_get_section_by_name (stdoutput, string);
  if (sec == 0)
    {
      sec = subseg_new (string, 0);
      bfd_set_section_flags (stdoutput, sec, flags);
      sec->output_section = sec;
    }
  previous_section = now_seg;
  previous_subsection = now_subseg;
  subseg_set (sec, 0);
}

void
obj_elf_previous ()
{
  if (previous_section == 0)
    {
      as_bad (".previous without corresponding .section; ignored");
      return;
    }
  subseg_set (previous_section, previous_subsection);
  previous_section = 0;
}

int
obj_elf_write_symbol_p (sym)
     symbolS *sym;
{
  /* If this is a local symbol, are there any relocations for which
     need this symbol? */

  /* To find this out, we examine all relocations in all bfd sections
     that have relocations.  If there is one that references this
     symbol, we need to keep this symbol.  In this case, we return a
     true status.  In all other cases, we return a false status. */

  if (S_IS_LOCAL (sym))
    {
      asymbol *bsym = sym->bsym;
      bfd *abfd = bsym->the_bfd;
      asection *bsec;

      for (bsec = abfd->sections; bsec; bsec = bsec->next)
	{
	  struct reloc_cache_entry **rlocs = bsec->orelocation;
	  int rcnt = bsec->reloc_count;

	  if (rlocs)
	    {
	      int i;

	      for (i = 0; i < rcnt; i++)
		if (rlocs[i]->sym_ptr_ptr
		    && rlocs[i]->sym_ptr_ptr[0] == bsym)
		  return 1;
	    }
	  else
	    {
	      /* No relocations for this section.  Check the seg_info
	         structure to see if there are any fixups for this
	         section. */
	      segment_info_type *seginfo = seg_info (bsec);
	      fixS *fixp;

	      for (fixp = seginfo->fix_root; fixp; fixp = fixp->fx_next)
		if ((fixp->fx_addsy && fixp->fx_addsy->bsym == bsym)
		    || (fixp->fx_subsy && fixp->fx_subsy->bsym == bsym))
		  return 1;
	    }
	}
    }
  return 0;
}

int
obj_elf_write_symbol (sym)
     symbolS *sym;
{
  return /* obj_elf_write_symbol_p (sym) || */ !S_IS_LOCAL (sym);
}

int
obj_elf_frob_symbol (sym, punt)
     symbolS *sym;
     int *punt;
{
#if 0 /* ?? The return value is ignored.  Only the value of *punt is
	 relevant.  */
  return obj_elf_write_symbol_p (sym);
#endif
}

static void
obj_elf_line ()
{
  /* Assume delimiter is part of expression.  BSD4.2 as fails with
     delightful bug, so we are not being incompatible here. */
  new_logical_line ((char *) NULL, (int) (get_absolute_expression ()));
  demand_empty_rest_of_line ();
}

/*
 *			stab()
 *
 * Handle .stabX directives, which used to be open-coded.
 * So much creeping featurism overloaded the semantics that we decided
 * to put all .stabX thinking in one place. Here.
 *
 * We try to make any .stabX directive legal. Other people's AS will often
 * do assembly-time consistency checks: eg assigning meaning to n_type bits
 * and "protecting" you from setting them to certain values. (They also zero
 * certain bits before emitting symbols. Tut tut.)
 *
 * If an expression is not absolute we either gripe or use the relocation
 * information. Other people's assemblers silently forget information they
 * don't need and invent information they need that you didn't supply.
 *
 * .stabX directives always make a symbol table entry. It may be junk if
 * the rest of your .stabX directive is malformed.
 */

/*
 *                      elf_stab_symbol_string()
 *
 * Build a string dictionary entry for a .stabX symbol.
 * The symbol is added to the .stabstr section.
 *
 */

static unsigned int
elf_stab_symbol_string (string, secname)
     char *string, *secname;
{
  asection *save_seg;
  asection *seg;
  subsegT save_subseg;
  unsigned int length;
  unsigned int old_gdb_string_index;
  char *clengthP;
  int i;
  char c;
  /* @@FIXME -- there should be no static data here!
     This also has the effect of making all stab string tables large enough
     to contain all the contents written to any of them.  This only matters
     with the Solaris native compiler for the moment, but it should be fixed
     anyways.  */
  static unsigned int gdb_string_index = 0;

  old_gdb_string_index = 0;
  length = strlen (string);
  clengthP = (char *) &length;
  if (length > 0)
    {				/* Ordinary case. */
      save_seg = now_seg;
      save_subseg = now_subseg;

      /* Create the stab sections, if they are not already created. */
      {
	char *newsecname = xmalloc (strlen (secname) + 4);
	strcpy (newsecname, secname);
	strcat (newsecname, "str");
	seg = bfd_get_section_by_name (stdoutput, newsecname);
	if (seg == 0)
	  {
	    seg = bfd_make_section_old_way (stdoutput, newsecname);
	    bfd_set_section_flags (stdoutput, seg, SEC_READONLY | SEC_ALLOC);
	  }
/*	free (newsecname);*/
      }
      subseg_new ((char *) seg->name, save_subseg);
      old_gdb_string_index = gdb_string_index;
      i = 0;
      while ((c = *string++))
	{
	  i++;
	  gdb_string_index++;
	  FRAG_APPEND_1_CHAR (c);
	}
      {
	FRAG_APPEND_1_CHAR ((char) 0);
	i++;
	gdb_string_index++;
      }
      while (i % 4 != 0)
	{
	  FRAG_APPEND_1_CHAR ((char) 0);
	  i++;
	  gdb_string_index++;
	}
      subseg_new ((char *) save_seg->name, save_subseg);
    }

  return old_gdb_string_index;
}

static void
DEFUN (elf_stab_symbol, (symbolP, stab_type),
       symbolS *symbolP AND
       int stab_type)
{
  char *toP;

  toP = frag_more (8);
  /* the string index portion of the stab */
  md_number_to_chars (toP, (valueT) symbolP->sy_name_offset, 4);
  md_number_to_chars (toP + 4, (valueT) S_GET_TYPE (symbolP), 1);
  md_number_to_chars (toP + 5, (valueT) S_GET_OTHER (symbolP), 1);
  md_number_to_chars (toP + 6, (valueT) S_GET_DESC (symbolP), 2);
  /* The n_value field doesn't get written here, it gets done below.  It
     may be an expression needing relocating.  */
}

static void 
obj_elf_stab_generic (what, secname)
     int what;
     char *secname;
{
  extern int listing;

  symbolS *symbolP = 0;
  char *string;
  int saved_type = 0;
  int length;
  int goof = 0;
  int seg_is_new = 0;
  long longint;
  asection *saved_seg = now_seg;
  asection *seg;
  subsegT subseg = now_subseg;

#if 1
  /* This function doesn't work yet.

     Actually, this function is okay, but some finalizations are
     needed before writing the object file; that's not done yet, and
     the Solaris linker chokes without it.

     In any case, this should effectively disable it for now.  */
  if (what == 's')
    demand_copy_C_string (&length);
  s_ignore (69);
  return;
#endif

  seg = bfd_get_section_by_name (stdoutput, secname);
  if (seg == 0)
    {
      seg = subseg_new (secname, 0);
      bfd_set_section_flags (stdoutput, seg,
			     SEC_READONLY | SEC_ALLOC | SEC_RELOC);
      subseg_set (saved_seg, subseg);
      seg_is_new = 1;
    }

  /*
   * Enter with input_line_pointer pointing past .stabX and any following
   * whitespace.
   */
  if (what == 's')
    {
      string = demand_copy_C_string (&length);
      SKIP_WHITESPACE ();
      if (*input_line_pointer == ',')
	input_line_pointer++;
      else
	{
	  as_bad ("I need a comma after symbol's name");
	  goof = 1;
	}
    }
  else
    string = "";

  /*
   * Input_line_pointer->after ','.  String->symbol name.
   */
  if (!goof)
    {
      symbolP = symbol_new (string, &bfd_und_section, (valueT) 0, (struct frag *) 0);

      /* enter the string in the .stab string table (section .stabstr) */
      symbolP->sy_name_offset = elf_stab_symbol_string (string, secname);

      switch (what)
	{
	case 'd':
	  S_SET_NAME (symbolP, NULL);	/* .stabd feature. */
	  S_SET_VALUE (symbolP,
		       (valueT) ((char*) obstack_next_free (&frags) - frag_now->fr_literal));
	  S_SET_SEGMENT (symbolP, now_seg);
	  symbolP->sy_frag = frag_now;
	  break;

	case 'n':
	  symbolP->sy_frag = &zero_address_frag;
	  break;

	case 's':
	  symbolP->sy_frag = &zero_address_frag;
	  break;

	default:
	  BAD_CASE (what);
	  break;
	}

      if (get_absolute_expression_and_terminator (&longint) == ',')
	{
	  saved_type = longint;
	  S_SET_TYPE (symbolP, saved_type);
	}
      else
	{
	  as_bad ("I want a comma after the n_type expression");
	  goof = 1;
	  input_line_pointer--;	/* Backup over a non-',' char. */
	}
    }

  if (!goof)
    {
      if (get_absolute_expression_and_terminator (&longint) == ',')
	S_SET_OTHER (symbolP, longint);
      else
	{
	  as_bad ("I want a comma after the n_other expression");
	  goof = 1;
	  input_line_pointer--;	/* Backup over a non-',' char. */
	}
    }

  if (!goof)
    {
      S_SET_DESC (symbolP, get_absolute_expression ());
      if (what == 's' || what == 'n')
	{
	  if (*input_line_pointer != ',')
	    {
	      as_bad ("I want a comma after the n_desc expression");
	      goof = 1;
	    }
	  else
	    {
	      input_line_pointer++;
	    }
	}
    }

  if (goof)
    {
      ignore_rest_of_line ();
      return;
    }

  subseg_new ((char *) seg->name, subseg);

  if (seg_is_new)
    /* allocate and discard -- filled in later */
    (void) frag_more (12);

  /* Emit the stab symbol. */
  elf_stab_symbol (symbolP, what);

  if (what == 's' || what == 'n')
    {
      cons (4);
      input_line_pointer--;
    }
  else
    {
      char *p = frag_more (4);
      md_number_to_chars (p, 0, 0);
    }

  subseg_new ((char *) saved_seg->name, subseg);

  if ((what == 's' || what == 'n')
      && symbolP->sy_value.X_op == O_constant)
    {
      /* symbol is not needed in the regular symbol table */
      symbol_remove (symbolP, &symbol_rootP, &symbol_lastP);
    }

  if (what == 's' && S_GET_TYPE (symbolP) == N_SO)
    {
      fragS *fragp = seg_info (seg)->frchainP->frch_root;
      while (fragp
	     && fragp->fr_address + fragp->fr_fix < 12)
	fragp = fragp->fr_next;
      assert (fragp != 0);
      assert (fragp->fr_type == rs_fill);
      assert (fragp->fr_address == 0 && fragp->fr_fix >= 12);
      md_number_to_chars (fragp->fr_literal, (valueT) symbolP->sy_name_offset,
			  4);
    }

  if (listing)
    switch (S_GET_TYPE (symbolP))
      {
      case N_SLINE:
	listing_source_line (S_GET_DESC (symbolP));
	break;
      case N_SO:
      case N_SOL:
	listing_source_file (string);
	break;
      }

  demand_empty_rest_of_line ();
}

static void
obj_elf_stab (what)
     int what;
{
  obj_elf_stab_generic (what, ".stab");
}

static void
obj_elf_xstab (what)
     int what;
{
  int length;
  char *secname;

  secname = demand_copy_C_string (&length);
  SKIP_WHITESPACE ();
  if (*input_line_pointer == ',')
    input_line_pointer++;
  else
    {
      as_bad ("comma missing in .xstabs");
      ignore_rest_of_line ();
      return;
    }
  obj_elf_stab_generic (what, secname);
}

void 
obj_elf_desc ()
{
  char *name;
  char c;
  char *p;
  symbolS *symbolP;
  int temp;

  /* Frob invented at RMS' request. Set the n_desc of a symbol.  */
  name = input_line_pointer;
  c = get_symbol_end ();
  p = input_line_pointer;
  *p = c;
  SKIP_WHITESPACE ();
  if (*input_line_pointer != ',')
    {
      *p = 0;
      as_bad ("Expected comma after name \"%s\"", name);
      *p = c;
      ignore_rest_of_line ();
    }
  else
    {
      input_line_pointer++;
      temp = get_absolute_expression ();
      *p = 0;
      symbolP = symbol_find_or_make (name);
      *p = c;
      S_SET_DESC (symbolP, temp);
    }
  demand_empty_rest_of_line ();
}				/* obj_elf_desc() */

void 
obj_read_begin_hook ()
{
}

void 
obj_symbol_new_hook (symbolP)
     symbolS *symbolP;
{
#if 0 /* BFD already takes care of this */
  elf32_symbol_type *esym = (elf32_symbol_type *) symbolP;

  /* There is an Elf_Internal_Sym and an Elf_External_Sym.  For now,
     just zero them out.  */

  bzero ((char *) &esym->internal_elf_sym, sizeof (esym->internal_elf_sym));
  bzero ((char *) &esym->native_elf_sym, sizeof (esym->native_elf_sym));
  bzero ((char *) &esym->tc_data, sizeof (esym->tc_data));
#endif
}

void 
obj_elf_version ()
{
  char *name;
  unsigned int c;
  char ch;
  char *p;
  asection *seg = now_seg;
  subsegT subseg = now_subseg;
  Elf_Internal_Note i_note;
  Elf_External_Note e_note;
  asection *note_secp = (asection *) NULL;
  int i, len;

  SKIP_WHITESPACE ();
  if (*input_line_pointer == '\"')
    {
      ++input_line_pointer;	/* -> 1st char of string. */
      name = input_line_pointer;

      while (is_a_char (c = next_char_of_string ()))
	;
      c = *input_line_pointer;
      *input_line_pointer = '\0';
      *(input_line_pointer - 1) = '\0';
      *input_line_pointer = c;

      /* create the .note section if this is the first version string */

      note_secp = bfd_get_section_by_name (stdoutput, ".note");
      if (note_secp == (asection *) NULL)
	{
	  note_secp = bfd_make_section_old_way (stdoutput, ".note");
	  bfd_set_section_flags (stdoutput,
				 note_secp,
				 SEC_LOAD | SEC_ALLOC | SEC_HAS_CONTENTS);
	}

      /* process the version string */

      subseg_new ((char *) note_secp->name, 0);
      len = strlen (name);

      i_note.namesz = ((len + 1) + 3) & ~3; /* round this to word boundary */
      i_note.descsz = 0;	/* no description */
      i_note.type = NT_VERSION;
      p = frag_more (sizeof (e_note.namesz));
      md_number_to_chars (p, (valueT) i_note.namesz, 4);
      p = frag_more (sizeof (e_note.descsz));
      md_number_to_chars (p, (valueT) i_note.descsz, 4);
      p = frag_more (sizeof (e_note.type));
      md_number_to_chars (p, (valueT) i_note.type, 4);

      for (i = 0; i < len; i++)
	{
	  ch = *(name + i);
	  {
	    FRAG_APPEND_1_CHAR (ch);
	  }
	}
      frag_align (2, 0);

      subseg_new ((char *) seg->name, subseg);
    }
  else
    {
      as_bad ("Expected quoted string");
    }
  demand_empty_rest_of_line ();
}

static void
obj_elf_size ()
{
  char *name = input_line_pointer;
  char c = get_symbol_end ();
  char *p;
  expressionS exp;
  symbolS *sym;

  p = input_line_pointer;
  *p = c;
  SKIP_WHITESPACE ();
  if (*input_line_pointer != ',')
    {
      *p = 0;
      as_bad ("expected comma after name `%s' in .size directive", name);
      *p = c;
      ignore_rest_of_line ();
      return;
    }
  input_line_pointer++;
  expression (&exp);
  if (exp.X_op == O_absent)
    {
      as_bad ("missing expression in .size directive");
      exp.X_op = O_constant;
      exp.X_add_number = 0;
    }
  *p = 0;
  sym = symbol_find_or_make (name);
  *p = c;
  if (exp.X_op == O_constant)
    S_SET_SIZE (sym, exp.X_add_number);
  else
    {
#if 0
      static int warned;
      if (!warned)
	{
	  as_tsktsk (".size expressions not yet supported, ignored");
	  warned++;
	}
#endif
    }
  demand_empty_rest_of_line ();
}

static void
obj_elf_type ()
{
  char *name = input_line_pointer;
  char c = get_symbol_end ();
  char *p;
  int type = 0;
  symbolS *sym;

  p = input_line_pointer;
  *p = c;
  SKIP_WHITESPACE ();
  if (*input_line_pointer != ',')
    {
      as_bad ("expected comma after name in .type directive");
    egress:
      ignore_rest_of_line ();
      return;
    }
  input_line_pointer++;
  SKIP_WHITESPACE ();
  if (*input_line_pointer != '#')
    {
      as_bad ("expected `#' after comma in .type directive");
      goto egress;
    }
  input_line_pointer++;
  if (!strncmp ("function", input_line_pointer, sizeof ("function") - 1))
    {
      type = BSF_FUNCTION;
      input_line_pointer += sizeof ("function") - 1;
    }
  else if (!strncmp ("object", input_line_pointer, sizeof ("object") - 1))
    {
      input_line_pointer += sizeof ("object") - 1;
    }
  else
    {
      as_bad ("unrecognized symbol type, ignored");
      goto egress;
    }
  demand_empty_rest_of_line ();
  *p = 0;
  sym = symbol_find_or_make (name);
  sym->bsym->flags |= type;
}

static void
obj_elf_ident ()
{
  static segT comment_section;
  segT old_section = now_seg;
  int old_subsection = now_subseg;

  if (!comment_section)
    {
      char *p;
      comment_section = subseg_new (".comment", 0);
      bfd_set_section_flags (stdoutput, comment_section, SEC_HAS_CONTENTS);
      p = frag_more (1);
      *p = 0;
    }
  else
    subseg_set (comment_section, 0);
  stringer (1);
  subseg_set (old_section, old_subsection);
}

static void
adjust_stab_sections (abfd, sec, xxx)
     bfd *abfd;
     asection *sec;
     PTR xxx;
{
  char *name;
  asection *strsec;
  fragS *fragp;
  int strsz, nsyms;

  if (strncmp (".stab", sec->name, 5))
    return;
  if (!strcmp ("str", sec->name + strlen (sec->name) - 3))
    return;

  name = (char *) alloca (strlen (sec->name) + 4);
  strcpy (name, sec->name);
  strcat (name, "str");
  strsec = bfd_get_section_by_name (abfd, name);
  if (strsec)
    strsz = bfd_section_size (abfd, strsec);
  else
    strsz = 0;
  nsyms = bfd_section_size (abfd, sec) / 12 - 1;

  fragp = seg_info (sec)->frchainP->frch_root;
  while (fragp
	 && fragp->fr_address + fragp->fr_fix < 12)
    fragp = fragp->fr_next;
  assert (fragp != 0);
  assert (fragp->fr_type == rs_fill);
  assert (fragp->fr_address == 0 && fragp->fr_fix >= 12);

  bfd_h_put_16 (abfd, nsyms, fragp->fr_literal + 6);
  bfd_h_put_32 (abfd, strsz, fragp->fr_literal + 8);
}

void 
elf_frob_file ()
{
  bfd_map_over_sections (stdoutput, adjust_stab_sections, (PTR) 0);

#ifdef elf_tc_symbol
  {
    int i;

    for (i = 0; i < stdoutput->symcount; i++)
      elf_tc_symbol (stdoutput, (elf_symbol_type *) (stdoutput->outsymbols[i]),
		     i + 1);
  }
#endif

#ifdef elf_tc_final_processing
  elf_tc_final_processing_hook ();
#endif

  /* Finally, we must make any target-specific sections. */

#ifdef elf_tc_make_sections
  elf_tc_make_sections (stdoutput, NULL);
#endif
}
