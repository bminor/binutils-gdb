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
#include "obstack.h"

static int obj_elf_write_symbol_p PARAMS ((symbolS *sym));

static void obj_elf_line PARAMS ((int));
void obj_elf_version PARAMS ((int));
static void obj_elf_size PARAMS ((int));
static void obj_elf_type PARAMS ((int));
static void obj_elf_ident PARAMS ((int));
static void obj_elf_weak PARAMS ((int));
static void obj_elf_local PARAMS ((int));
static void obj_elf_common PARAMS ((int));
static void obj_elf_data PARAMS ((int));
static void obj_elf_text PARAMS ((int));

const pseudo_typeS obj_pseudo_table[] =
{
  {"comm", obj_elf_common, 0},
  {"ident", obj_elf_ident, 0},
  {"local", obj_elf_local, 0},
  {"previous", obj_elf_previous, 0},
  {"section", obj_elf_section, 0},
  {"size", obj_elf_size, 0},
  {"type", obj_elf_type, 0},
  {"version", obj_elf_version, 0},
  {"weak", obj_elf_weak, 0},

/* These are used for stabs-in-elf configurations.  */
  {"line", obj_elf_line, 0},

  /* These are used for dwarf. */
  {"2byte", cons, 2},
  {"4byte", cons, 4},
  {"8byte", cons, 8},

  /* We need to trap the section changing calls to handle .previous.  */
  {"data", obj_elf_data, 0},
  {"text", obj_elf_text, 0},

  {NULL}			/* end sentinel */
};

#undef NO_RELOC
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

static void
obj_elf_common (ignore)
     int ignore;
{
  char *name;
  char c;
  char *p;
  int temp, size;
  symbolS *symbolP;

  name = input_line_pointer;
  c = get_symbol_end ();
  /* just after name is now '\0' */
  p = input_line_pointer;
  *p = c;
  SKIP_WHITESPACE ();
  if (*input_line_pointer != ',')
    {
      as_bad ("Expected comma after symbol-name");
      ignore_rest_of_line ();
      return;
    }
  input_line_pointer++;		/* skip ',' */
  if ((temp = get_absolute_expression ()) < 0)
    {
      as_bad (".COMMon length (%d.) <0! Ignored.", temp);
      ignore_rest_of_line ();
      return;
    }
  size = temp;
  *p = 0;
  symbolP = symbol_find_or_make (name);
  *p = c;
  if (S_IS_DEFINED (symbolP))
    {
      as_bad ("Ignoring attempt to re-define symbol");
      ignore_rest_of_line ();
      return;
    }
  if (S_GET_VALUE (symbolP) != 0)
    {
      if (S_GET_VALUE (symbolP) != size)
	{
	  as_warn ("Length of .comm \"%s\" is already %ld. Not changed to %d.",
		   S_GET_NAME (symbolP), (long) S_GET_VALUE (symbolP), size);
	}
    }
  know (symbolP->sy_frag == &zero_address_frag);
  if (*input_line_pointer != ',')
    {
      as_bad ("Expected comma after common length");
      ignore_rest_of_line ();
      return;
    }
  input_line_pointer++;
  SKIP_WHITESPACE ();
  if (*input_line_pointer != '"')
    {
      temp = get_absolute_expression ();
      if (temp < 0)
	{
	  temp = 0;
	  as_warn ("Common alignment negative; 0 assumed");
	}
      if (symbolP->local)
	{
	  segT old_sec;
	  int old_subsec;
	  char *pfrag;
	  int align;

	/* allocate_bss: */
	  old_sec = now_seg;
	  old_subsec = now_subseg;
	  align = temp;
	  record_alignment (bss_section, align);
	  subseg_set (bss_section, 0);
	  if (align)
	    frag_align (align, 0);
	  if (S_GET_SEGMENT (symbolP) == bss_section)
	    symbolP->sy_frag->fr_symbol = 0;
	  symbolP->sy_frag = frag_now;
	  pfrag = frag_var (rs_org, 1, 1, (relax_substateT) 0, symbolP, size,
			    (char *) 0);
	  *pfrag = 0;
	  S_SET_SEGMENT (symbolP, bss_section);
	  S_CLEAR_EXTERNAL (symbolP);
	  subseg_set (old_sec, old_subsec);
	}
      else
	{
	allocate_common:
	  S_SET_VALUE (symbolP, (valueT) size);
	  S_SET_EXTERNAL (symbolP);
	  /* should be common, but this is how gas does it for now */
	  S_SET_SEGMENT (symbolP, &bfd_und_section);
	}
    }
  else
    {
      input_line_pointer++;
      /* @@ Some use the dot, some don't.  Can we get some consistency??  */
      if (*input_line_pointer == '.')
	input_line_pointer++;
      /* @@ Some say data, some say bss.  */
      if (strncmp (input_line_pointer, "bss\"", 4)
	  && strncmp (input_line_pointer, "data\"", 5))
	{
	  while (*--input_line_pointer != '"')
	    ;
	  input_line_pointer--;
	  goto bad_common_segment;
	}
      while (*input_line_pointer++ != '"')
	;
      goto allocate_common;
    }
  demand_empty_rest_of_line ();
  return;

  {
  bad_common_segment:
    p = input_line_pointer;
    while (*p && *p != '\n')
      p++;
    c = *p;
    *p = '\0';
    as_bad ("bad .common segment %s", input_line_pointer + 1);
    *p = c;
    input_line_pointer = p;
    ignore_rest_of_line ();
    return;
  }
}

static void 
obj_elf_local (ignore)
     int ignore;
{
  char *name;
  int c;
  symbolS *symbolP;

  do
    {
      name = input_line_pointer;
      c = get_symbol_end ();
      symbolP = symbol_find_or_make (name);
      *input_line_pointer = c;
      SKIP_WHITESPACE ();
      S_CLEAR_EXTERNAL (symbolP);
      symbolP->local = 1;
      if (c == ',')
	{
	  input_line_pointer++;
	  SKIP_WHITESPACE ();
	  if (*input_line_pointer == '\n')
	    c = '\n';
	}
    }
  while (c == ',');
  demand_empty_rest_of_line ();
}

static void 
obj_elf_weak (ignore)
     int ignore;
{
  char *name;
  int c;
  symbolS *symbolP;

  do
    {
      name = input_line_pointer;
      c = get_symbol_end ();
      symbolP = symbol_find_or_make (name);
      *input_line_pointer = c;
      SKIP_WHITESPACE ();
      S_SET_WEAK (symbolP);
      symbolP->local = 1;
      if (c == ',')
	{
	  input_line_pointer++;
	  SKIP_WHITESPACE ();
	  if (*input_line_pointer == '\n')
	    c = '\n';
	}
    }
  while (c == ',');
  demand_empty_rest_of_line ();
}

static segT previous_section;
static int previous_subsection;

void
obj_elf_section (xxx)
     int xxx;
{
  char *string;
  asection *sec;
  int new_sec;

  /* Initialize this with inclusive-or of all flags that can be cleared
     by attributes, but not set by them.  Also include flags that won't
     get set properly in the assembler, but which the user/compiler
     shouldn't be expected to set.  */
  flagword flags = SEC_READONLY | SEC_ALLOC | SEC_RELOC | SEC_LOAD;
  /* Initialize this with the default flags to be used if none are
     specified.  */
  flagword default_flags = 0;

  SKIP_WHITESPACE ();
  /* Get name of section.  */
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
      string = xmalloc ((unsigned long) (p - input_line_pointer + 1));
      strcpy (string, input_line_pointer);
      *p = c;
      input_line_pointer = p;
    }
  if (!strcmp (string, ".rodata")
      || !strcmp (string, ".rodata1"))
    default_flags = SEC_ALLOC | SEC_READONLY | SEC_RELOC | SEC_LOAD;
  else if (!strcmp (string, ".init")
	   || !strcmp (string, ".fini"))
    default_flags = SEC_ALLOC | SEC_READONLY | SEC_RELOC | SEC_CODE | SEC_LOAD;

  SKIP_WHITESPACE ();
  if (*input_line_pointer != ',')
    flags = default_flags;
  while (*input_line_pointer == ',')
    {
      flagword bit;
      unsigned int len;
      int inv;
      char *p, oldp;

      input_line_pointer++;

      /* Under i386-svr4, gcc emits a string here.  I don't know what this
	 string is supposed to signify or how to handle it.  Ignore it for
	 now, unless it becomes a problem.  */
      if (*input_line_pointer == '"')
	{
	  demand_copy_C_string (&xxx);
	  SKIP_WHITESPACE ();
	  continue;
	}

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
      CHECK ("alloc", SEC_ALLOC | SEC_LOAD, 0);
      CHECK ("execinstr", SEC_CODE, 1);
      CHECK ("progbits", SEC_ALLOC | SEC_LOAD, 1);
#undef CHECK

      p = input_line_pointer;
      while (!is_end_of_line[(unsigned char) *p] && *p != 0 && *p != ',')
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

  previous_section = now_seg;
  previous_subsection = now_subseg;

  new_sec = bfd_get_section_by_name (stdoutput, string) == NULL;
  sec = subseg_new (string, 0);
  if (new_sec)
    bfd_set_section_flags (stdoutput, sec, flags);
}

/* Change to the .data section.  */

static void
obj_elf_data (i)
     int i;
{
  previous_section = now_seg;
  previous_subsection = now_subseg;
  s_data (i);
}

/* Change to the .text section.  */

static void
obj_elf_text (i)
     int i;
{
  previous_section = now_seg;
  previous_subsection = now_subseg;
  s_text (i);
}

void
obj_elf_previous (ignore)
     int ignore;
{
  if (previous_section == 0)
    {
      as_bad (".previous without corresponding .section; ignored");
      return;
    }
  subseg_set (previous_section, previous_subsection);
  previous_section = 0;
}

static int
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
 /* FIXME: Just return 0 until is fixed.  */
 return 0;
}

static void
obj_elf_line (ignore)
     int ignore;
{
  /* Assume delimiter is part of expression.  BSD4.2 as fails with
     delightful bug, so we are not being incompatible here. */
  new_logical_line ((char *) NULL, (int) (get_absolute_expression ()));
  demand_empty_rest_of_line ();
}

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
obj_elf_version (ignore)
     int ignore;
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

      /* create the .note section */

      note_secp = subseg_new (".note", 0);
      bfd_set_section_flags (stdoutput,
			     note_secp,
			     SEC_HAS_CONTENTS | SEC_READONLY);

      /* process the version string */

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

      subseg_set (seg, subseg);
    }
  else
    {
      as_bad ("Expected quoted string");
    }
  demand_empty_rest_of_line ();
}

static void
obj_elf_size (ignore)
     int ignore;
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
obj_elf_type (ignore)
     int ignore;
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
  if (*input_line_pointer != '#' && *input_line_pointer != '@')
    {
      as_bad ("expected `#' or `@' after comma in .type directive");
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
obj_elf_ident (ignore)
     int ignore;
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

/* The first entry in a .stabs section is special.  */

void
obj_elf_init_stab_section (seg)
     segT seg;
{
  extern char *logical_input_file, *physical_input_file;
  char *p;
  const char *file;
  unsigned int stroff;

  p = frag_more (12);
  file = logical_input_file;
  if (file == NULL)
    file = physical_input_file;
  if (file == NULL)
    file = "UNKNOWN";
  stroff = get_stab_string_offset (file, segment_name (seg));
  know (stroff == 1);
  md_number_to_chars (p, stroff, 4);
  seg_info (seg)->stabu.p = p;
}

/* Fill in the counts in the first entry in a .stabs section.  */

static void
adjust_stab_sections (abfd, sec, xxx)
     bfd *abfd;
     asection *sec;
     PTR xxx;
{
  char *name;
  asection *strsec;
  char *p;
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

  p = seg_info (sec)->stabu.p;
  assert (p != 0);

  bfd_h_put_16 (abfd, (bfd_vma) nsyms, (bfd_byte *) p + 6);
  bfd_h_put_32 (abfd, (bfd_vma) strsz, (bfd_byte *) p + 8);
}

void 
elf_frob_file ()
{
  bfd_map_over_sections (stdoutput, adjust_stab_sections, (PTR) 0);

#ifdef elf_tc_symbol
  {
    int i;

    for (i = 0; i < stdoutput->symcount; i++)
      elf_tc_symbol (stdoutput, (PTR) (stdoutput->outsymbols[i]),
		     i + 1);
  }
#endif

#ifdef elf_tc_final_processing
  elf_tc_final_processing ();
#endif

  /* Finally, we must make any target-specific sections. */

#ifdef elf_tc_make_sections
  elf_tc_make_sections (stdoutput);
#endif
}
