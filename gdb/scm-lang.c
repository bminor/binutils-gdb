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
#include "c-lang.h"
#include "value.h"

extern struct type ** const (c_builtin_types[]);
extern value_ptr value_allocate_space_in_inferior PARAMS ((int));
extern value_ptr find_function_in_inferior PARAMS ((char*));

static void scm_lreadr ();

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

static void
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
is_object_type (type)
     struct type *type;
{
  /* FIXME - this should test for the SCM type, but we can't do that ! */
  return TYPE_CODE (type) == TYPE_CODE_INT
    && TYPE_NAME (type)
#if 1
    && strcmp (TYPE_NAME (type), "SCM") == 0;
#else
    && TYPE_LENGTH (type) == TYPE_LENGTH (builtin_type_long)
    && strcmp (TYPE_NAME (type), "long int") == 0;
#endif
}

/* Prints the SCM value VALUE by invoking the inferior, if appropraite.
   Returns >= 0 on succes;  retunr -1 if the inferior cannot/should not
   print VALUE. */

int
scm_inferior_print (value, stream, format, deref_ref, recurse, pretty)
     LONGEST value;
     GDB_FILE *stream;
     int format;
     int deref_ref;
     int recurse;
     enum val_prettyprint pretty;
{
  return -1;
}

#define SCM_ITAG8_DATA(X)	((X)>>8)
#define SCM_ICHR(x)	((unsigned char)SCM_ITAG8_DATA(x))
#define SCM_ICHRP(x)    (SCM_ITAG8(x) == scm_tc8_char)
#define scm_tc8_char 0xf4
#define SCM_IFLAGP(n)            ((0x87 & (int)(n))==4)
#define SCM_ISYMNUM(n)           ((int)((n)>>9))
#define SCM_ISYMCHARS(n)         (scm_isymnames[SCM_ISYMNUM(n)])
#define SCM_ILOCP(n)             ((0xff & (int)(n))==0xfc)
#define SCM_ITAG8(X)             ((int)(X) & 0xff)

/* {Names of immediate symbols}
 * This table must agree with the declarations in scm.h: {Immediate Symbols}.*/

static char *scm_isymnames[] =
{
  /* This table must agree with the declarations */
  "#@and",
  "#@begin",
  "#@case",
  "#@cond",
  "#@do",
  "#@if",
  "#@lambda",
  "#@let",
  "#@let*",
  "#@letrec",
  "#@or",
  "#@quote",
  "#@set!",
  "#@define",
#if 0
  "#@literal-variable-ref",
  "#@literal-variable-set!",
#endif
  "#@apply",
  "#@call-with-current-continuation",

 /* user visible ISYMS */
 /* other keywords */
 /* Flags */

  "#f",
  "#t",
  "#<undefined>",
  "#<eof>",
  "()",
  "#<unspecified>"
};

int
scm_val_print (type, valaddr, address, stream, format, deref_ref, recurse,
	     pretty)
     struct type *type;
     char *valaddr;
     CORE_ADDR address;
     GDB_FILE *stream;
     int format;
     int deref_ref;
     int recurse;
     enum val_prettyprint pretty;
{
  if (is_object_type (type))
    {
      LONGEST svalue = unpack_long (type, valaddr);
      if (scm_inferior_print (svalue, stream, format,
			      deref_ref, recurse, pretty) >= 0)
	{
	}
      else
	{
	  switch (7 & svalue)
	    {
	    case 2:
	    case 6:
	      print_longest (stream, format ? format : 'd', 1, svalue >> 2);
	      break;
	    case 4:
	      if (SCM_ICHRP (svalue))
		{
		  svalue = SCM_ICHR (svalue);
		  scm_printchar (svalue, stream);
		  break;
		}
	      else if (SCM_IFLAGP (svalue)
	       && (SCM_ISYMNUM (svalue)
		   < (sizeof scm_isymnames / sizeof (char *))))
		{
		  fputs_filtered (SCM_ISYMCHARS (svalue), stream);
		  break;
		}
	      else if (SCM_ILOCP (svalue))
		{
#if 0
		  fputs_filtered ("#@", stream);
		  scm_intprint ((long) IFRAME (exp), 10, port);
		  scm_putc (ICDRP (exp) ? '-' : '+', port);
		  scm_intprint ((long) IDIST (exp), 10, port);
		  break;
#endif
		}
	    default:
	      fprintf_filtered (stream, "#<%lX>", svalue);
	    }
	}
      gdb_flush (stream);
      return (0);
    }
  else
    {
      return c_val_print (type, valaddr, address, stream, format,
			  deref_ref, recurse, pretty);
    }
}

int
scm_value_print (val, stream, format, pretty)
     value_ptr val;
     GDB_FILE *stream;
     int format;
     enum val_prettyprint pretty;
{
  return (val_print (VALUE_TYPE (val), VALUE_CONTENTS (val),
		     VALUE_ADDRESS (val), stream, format, 1, 0, pretty));
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
