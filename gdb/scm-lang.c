/* Scheme/Guile language support routines for GDB, the GNU debugger.
   Copyright 1995 Free Software Foundation, Inc.

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include "defs.h"
#include "symtab.h"
#include "gdbtypes.h"
#include "expression.h"
#include "parser-defs.h"
#include "language.h"
#include "value.h"
#include "c-lang.h"
#include "scm-lang.h"
#include "scm-tags.h"

extern struct type ** const (c_builtin_types[]);
extern value_ptr value_allocate_space_in_inferior PARAMS ((int));
extern value_ptr find_function_in_inferior PARAMS ((char*));

static void scm_lreadr ();

struct type *SCM_TYPE = NULL;

static void
scm_read_token (c, weird)
     int c;
     int weird;
{
  while (1)
    {
      c = *lexptr++;
      switch (c)
	{
	case '[':
	case ']':
	case '(':
	case ')':
	case '\"':
	case ';':
	case ' ':  case '\t':  case '\r':  case '\f':
	case '\n':
	  if (weird)
	    goto default_case;
	case '\0':  /* End of line */
	eof_case:
	  --lexptr;
	  return;
	case '\\':
	  if (!weird)
	    goto default_case;
	  else
	    {
	      c = *lexptr++;
	      if (c == '\0')
		goto eof_case;
	      else
		goto default_case;
	    }
	case '}':
	  if (!weird)
	    goto default_case;

	  c = *lexptr++;
	  if (c == '#')
	    return;
	  else
	    {
	      --lexptr;
	      c = '}';
	      goto default_case;
	    }

	default:
	default_case:
	  ;
	}
    }
}

static int 
scm_skip_ws ()
{
  register int c;
  while (1)
    switch ((c = *lexptr++))
      {
      case '\0':
      goteof:
	return c;
      case ';':
      lp:
	switch ((c = *lexptr++))
	  {
	  case '\0':
	    goto goteof;
	  default:
	    goto lp;
	  case '\n':
	    break;
	  }
      case ' ':  case '\t':  case '\r':  case '\f':  case '\n':
	break;
      default:
	return c;
      }
}

static void
scm_lreadparen ()
{
  for (;;)
    {
      int c = scm_skip_ws ();
      if (')' == c || ']' == c)
	return;
      --lexptr;
      if (c == '\0')
	error ("missing close paren");
      scm_lreadr ();
    }
}

static void
scm_lreadr ()
{
  int c, j;
 tryagain:
  c = *lexptr++;
  switch (c)
    {
    case '\0':
      lexptr--;
      return;
    case '[':
    case '(':
      scm_lreadparen ();
      return;
    case ']':
    case ')':
      error ("unexpected #\\%c", c);
      goto tryagain;
    case '\'':
    case '`':
      scm_lreadr ();
      return;
    case ',':
      c = *lexptr++;
      if ('@' != c)
	lexptr--;
      scm_lreadr ();
      return;
    case '#':
      c = *lexptr++;
      switch (c)
	{
	case '[':
	case '(':
	  scm_lreadparen ();
	  return;
	case 't':  case 'T':
	case 'f':  case 'F':
	  return;
	case 'b':  case 'B':
	case 'o':  case 'O':
	case 'd':  case 'D':
	case 'x':  case 'X':
	case 'i':  case 'I':
	case 'e':  case 'E':
	  lexptr--;
	  c = '#';
	  goto num;
	case '*': /* bitvector */
	  scm_read_token (c, 0);
	  return;
	case '{':
	  scm_read_token (c, 1);
	  return;
	case '\\': /* character */
	  c = *lexptr++;
	  scm_read_token (c, 0);
	  return;
	case '|':
	  j = 1;		/* here j is the comment nesting depth */
	lp:
	  c = *lexptr++;
	lpc:
	  switch (c)
	    {
	    case '\0':
	      error ("unbalanced comment");
	    default:
	      goto lp;
	    case '|':
	      if ('#' != (c = *lexptr++))
		goto lpc;
	      if (--j)
		goto lp;
	      break;
	    case '#':
	      if ('|' != (c = *lexptr++))
		goto lpc;
	      ++j;
	      goto lp;
	    }
	  goto tryagain;
	case '.':
	default:
	callshrp:
	  scm_lreadr ();
	  return;
	}
    case '\"':
      while ('\"' != (c = *lexptr++))
	{
	  if (c == '\\')
	    switch (c = *lexptr++)
	      {
	      case '\0':
		error ("non-terminated string literal");
	      case '\n':
		continue;
	      case '0':
	      case 'f':
	      case 'n':
	      case 'r':
	      case 't':
	      case 'a':
	      case 'v':
		break;
	      }
	}
      return;
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
    case '.':
    case '-':
    case '+':
    num:
      scm_read_token (c, 0);
      return;
    case ':':
      scm_read_token ('-', 0);
      return;
    default:
      scm_read_token (c, 0);
    tok:
      return;
    }
}

int
scm_parse ()
{
  char* start;
  struct stoken str;
  while (*lexptr == ' ')
    lexptr++;
  start = lexptr;
  scm_lreadr ();
  str.length = lexptr - start;
  str.ptr = start;
  write_exp_elt_opcode (OP_EXPRSTRING);
  write_exp_string (str);
  write_exp_elt_opcode (OP_EXPRSTRING);
  return 0;
}

void
scm_printchar (c, stream)
     int c;
     GDB_FILE *stream;
{
  fprintf_filtered (stream, "#\\%c", c);
}

static void
scm_printstr (stream, string, length, force_ellipses)
     GDB_FILE *stream;
     char *string;
     unsigned int length;
     int force_ellipses;
{
  fprintf_filtered (stream, "\"%s\"", string);
}

int
is_scmvalue_type (type)
     struct type *type;
{
  if (TYPE_CODE (type) == TYPE_CODE_INT
      && TYPE_NAME (type) && strcmp (TYPE_NAME (type), "SCM") == 0)
    {
      SCM_TYPE = type;
      return 1;
    }
  return 0;
}

/* Get the INDEX'th SCM value, assuming SVALUE is the address
   of the 0'th one.  */

LONGEST
scm_get_field (svalue, index)
     LONGEST svalue;
     int index;
{
  value_ptr val;
  char buffer[20];
  if (SCM_TYPE == NULL)
    error ("internal error - no SCM type");
  read_memory (SCM2PTR (svalue) + index * TYPE_LENGTH (SCM_TYPE),
	       buffer, TYPE_LENGTH (SCM_TYPE));
  return unpack_long (SCM_TYPE, buffer);
}

static value_ptr
evaluate_subexp_scm (expect_type, exp, pos, noside)
     struct type *expect_type;
     register struct expression *exp;
     register int *pos;
     enum noside noside;
{
  enum exp_opcode op = exp->elts[*pos].opcode;
  value_ptr func, addr;
  int len, pc;  char *str;
  switch (op)
    {
    case OP_EXPRSTRING:
      pc = (*pos)++;
      len = longest_to_int (exp->elts[pc + 1].longconst);
      (*pos) += 3 + BYTES_TO_EXP_ELEM (len + 1);
      if (noside == EVAL_SKIP)
	goto nosideret;
      str = &exp->elts[ + 2].string;
      addr = value_allocate_space_in_inferior (len);
      write_memory (value_as_long (addr), str, len);
      func = find_function_in_inferior ("scm_evstr");
      return call_function_by_hand (func, 1, &addr);
    default: ;
    }
  return evaluate_subexp_standard (expect_type, exp, pos, noside);
 nosideret:
  return value_from_longest (builtin_type_long, (LONGEST) 1);
}

const struct language_defn scm_language_defn = {
  "scheme",			/* Language name */
  language_scm,
  c_builtin_types,
  range_check_off,
  type_check_off,
  scm_parse,
  c_error,
  evaluate_subexp_scm,
  scm_printchar,			/* Print a character constant */
  scm_printstr,			/* Function to print string constant */
  NULL,	/* Create fundamental type in this language */
  c_print_type,			/* Print a type using appropriate syntax */
  scm_val_print,		/* Print a value using appropriate syntax */
  scm_value_print,		/* Print a top-level value */
  {"",     "",    "",  ""},	/* Binary format info */
  {"#o%lo",  "#o",   "o", ""},	/* Octal format info */
  {"%ld",   "",    "d", ""},	/* Decimal format info */
  {"#x%lX", "#X",  "X", ""},	/* Hex format info */
  NULL,				/* expression operators for printing */
  1,				/* c-style arrays */
  0,				/* String lower bound */
  &builtin_type_char,		/* Type of string elements */ 
  LANG_MAGIC
};

void
_initialize_scheme_language ()
{
  add_language (&scm_language_defn);
}
