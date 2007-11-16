/* Modula 2 language support routines for GDB, the GNU debugger.

   Copyright (C) 1992, 1993, 1994, 1995, 1996, 1998, 2000, 2002, 2003, 2004,
   2005, 2007 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include "defs.h"
#include "symtab.h"
#include "gdbtypes.h"
#include "expression.h"
#include "parser-defs.h"
#include "language.h"
#include "m2-lang.h"
#include "c-lang.h"
#include "valprint.h"

extern void _initialize_m2_language (void);
static struct type *m2_create_fundamental_type (struct objfile *, int);
static void m2_printchar (int, struct ui_file *);
static void m2_emit_char (int, struct ui_file *, int);

/* Print the character C on STREAM as part of the contents of a literal
   string whose delimiter is QUOTER.  Note that that format for printing
   characters and strings is language specific.
   FIXME:  This is a copy of the same function from c-exp.y.  It should
   be replaced with a true Modula version.  */

static void
m2_emit_char (int c, struct ui_file *stream, int quoter)
{

  c &= 0xFF;			/* Avoid sign bit follies */

  if (PRINT_LITERAL_FORM (c))
    {
      if (c == '\\' || c == quoter)
	{
	  fputs_filtered ("\\", stream);
	}
      fprintf_filtered (stream, "%c", c);
    }
  else
    {
      switch (c)
	{
	case '\n':
	  fputs_filtered ("\\n", stream);
	  break;
	case '\b':
	  fputs_filtered ("\\b", stream);
	  break;
	case '\t':
	  fputs_filtered ("\\t", stream);
	  break;
	case '\f':
	  fputs_filtered ("\\f", stream);
	  break;
	case '\r':
	  fputs_filtered ("\\r", stream);
	  break;
	case '\033':
	  fputs_filtered ("\\e", stream);
	  break;
	case '\007':
	  fputs_filtered ("\\a", stream);
	  break;
	default:
	  fprintf_filtered (stream, "\\%.3o", (unsigned int) c);
	  break;
	}
    }
}

/* FIXME:  This is a copy of the same function from c-exp.y.  It should
   be replaced with a true Modula version.  */

static void
m2_printchar (int c, struct ui_file *stream)
{
  fputs_filtered ("'", stream);
  LA_EMIT_CHAR (c, stream, '\'');
  fputs_filtered ("'", stream);
}

/* Print the character string STRING, printing at most LENGTH characters.
   Printing stops early if the number hits print_max; repeat counts
   are printed as appropriate.  Print ellipses at the end if we
   had to stop before printing LENGTH characters, or if FORCE_ELLIPSES.
   FIXME:  This is a copy of the same function from c-exp.y.  It should
   be replaced with a true Modula version.  */

static void
m2_printstr (struct ui_file *stream, const gdb_byte *string,
	     unsigned int length, int width, int force_ellipses)
{
  unsigned int i;
  unsigned int things_printed = 0;
  int in_quotes = 0;
  int need_comma = 0;

  if (length == 0)
    {
      fputs_filtered ("\"\"", gdb_stdout);
      return;
    }

  for (i = 0; i < length && things_printed < print_max; ++i)
    {
      /* Position of the character we are examining
         to see whether it is repeated.  */
      unsigned int rep1;
      /* Number of repetitions we have detected so far.  */
      unsigned int reps;

      QUIT;

      if (need_comma)
	{
	  fputs_filtered (", ", stream);
	  need_comma = 0;
	}

      rep1 = i + 1;
      reps = 1;
      while (rep1 < length && string[rep1] == string[i])
	{
	  ++rep1;
	  ++reps;
	}

      if (reps > repeat_count_threshold)
	{
	  if (in_quotes)
	    {
	      if (inspect_it)
		fputs_filtered ("\\\", ", stream);
	      else
		fputs_filtered ("\", ", stream);
	      in_quotes = 0;
	    }
	  m2_printchar (string[i], stream);
	  fprintf_filtered (stream, " <repeats %u times>", reps);
	  i = rep1 - 1;
	  things_printed += repeat_count_threshold;
	  need_comma = 1;
	}
      else
	{
	  if (!in_quotes)
	    {
	      if (inspect_it)
		fputs_filtered ("\\\"", stream);
	      else
		fputs_filtered ("\"", stream);
	      in_quotes = 1;
	    }
	  LA_EMIT_CHAR (string[i], stream, '"');
	  ++things_printed;
	}
    }

  /* Terminate the quotes if necessary.  */
  if (in_quotes)
    {
      if (inspect_it)
	fputs_filtered ("\\\"", stream);
      else
	fputs_filtered ("\"", stream);
    }

  if (force_ellipses || i < length)
    fputs_filtered ("...", stream);
}

static struct value *
evaluate_subexp_modula2 (struct type *expect_type, struct expression *exp,
			 int *pos, enum noside noside)
{
  enum exp_opcode op = exp->elts[*pos].opcode;
  struct value *arg1;
  struct value *arg2;
  struct type *type;
  switch (op)
    {
    case UNOP_HIGH:
      (*pos)++;
      arg1 = evaluate_subexp_with_coercion (exp, pos, noside);

      if (noside == EVAL_SKIP || noside == EVAL_AVOID_SIDE_EFFECTS)
	return arg1;
      else
	{
	  arg1 = coerce_ref (arg1);
	  type = check_typedef (value_type (arg1));

	  if (m2_is_unbounded_array (type))
	    {
	      struct value *temp = arg1;
	      type = TYPE_FIELD_TYPE (type, 1);
	      /* i18n: Do not translate the "_m2_high" part!  */
	      arg1 = value_struct_elt (&temp, NULL, "_m2_high", NULL,
				       _("unbounded structure "
					 "missing _m2_high field"));
	  
	      if (value_type (arg1) != type)
		arg1 = value_cast (type, arg1);
	    }
	}
      return arg1;

    case BINOP_SUBSCRIPT:
      (*pos)++;
      arg1 = evaluate_subexp_with_coercion (exp, pos, noside);
      arg2 = evaluate_subexp_with_coercion (exp, pos, noside);
      if (noside == EVAL_SKIP)
	goto nosideret;
      /* If the user attempts to subscript something that is not an
         array or pointer type (like a plain int variable for example),
         then report this as an error.  */

      arg1 = coerce_ref (arg1);
      type = check_typedef (value_type (arg1));

      if (m2_is_unbounded_array (type))
	{
	  struct value *temp = arg1;
	  type = TYPE_FIELD_TYPE (type, 0);
	  if (type == NULL || (TYPE_CODE (type) != TYPE_CODE_PTR)) {
	    warning (_("internal error: unbounded array structure is unknown"));
	    return evaluate_subexp_standard (expect_type, exp, pos, noside);
	  }
	  /* i18n: Do not translate the "_m2_contents" part!  */
	  arg1 = value_struct_elt (&temp, NULL, "_m2_contents", NULL,
				   _("unbounded structure "
				     "missing _m2_contents field"));
	  
	  if (value_type (arg1) != type)
	    arg1 = value_cast (type, arg1);

	  type = check_typedef (value_type (arg1));
	  return value_ind (value_add (arg1, arg2));
	}
      else
	if (TYPE_CODE (type) != TYPE_CODE_ARRAY)
	  {
	    if (TYPE_NAME (type))
	      error (_("cannot subscript something of type `%s'"),
		     TYPE_NAME (type));
	    else
	      error (_("cannot subscript requested type"));
	  }

      if (noside == EVAL_AVOID_SIDE_EFFECTS)
	return value_zero (TYPE_TARGET_TYPE (type), VALUE_LVAL (arg1));
      else
	return value_subscript (arg1, arg2);

    default:
      return evaluate_subexp_standard (expect_type, exp, pos, noside);
    }

 nosideret:
  return value_from_longest (builtin_type_long, (LONGEST) 1);
}

/* FIXME:  This is a copy of c_create_fundamental_type(), before
   all the non-C types were stripped from it.  Needs to be fixed
   by an experienced Modula programmer.  */

static struct type *
m2_create_fundamental_type (struct objfile *objfile, int typeid)
{
  struct type *type = NULL;

  switch (typeid)
    {
    default:
      /* FIXME:  For now, if we are asked to produce a type not in this
         language, create the equivalent of a C integer type with the
         name "<?type?>".  When all the dust settles from the type
         reconstruction work, this should probably become an error.  */
      type = init_type (TYPE_CODE_INT,
			gdbarch_int_bit (current_gdbarch) / TARGET_CHAR_BIT,
			0, "<?type?>", objfile);
      warning (_("internal error: no Modula fundamental type %d"), typeid);
      break;
    case FT_VOID:
      type = init_type (TYPE_CODE_VOID,
			TARGET_CHAR_BIT / TARGET_CHAR_BIT,
			0, "void", objfile);
      break;
    case FT_BOOLEAN:
      type = init_type (TYPE_CODE_BOOL,
			TARGET_CHAR_BIT / TARGET_CHAR_BIT,
			TYPE_FLAG_UNSIGNED, "boolean", objfile);
      break;
    case FT_STRING:
      type = init_type (TYPE_CODE_STRING,
			TARGET_CHAR_BIT / TARGET_CHAR_BIT,
			0, "string", objfile);
      break;
    case FT_CHAR:
      type = init_type (TYPE_CODE_INT,
			TARGET_CHAR_BIT / TARGET_CHAR_BIT,
			0, "char", objfile);
      break;
    case FT_SIGNED_CHAR:
      type = init_type (TYPE_CODE_INT,
			TARGET_CHAR_BIT / TARGET_CHAR_BIT,
			0, "signed char", objfile);
      break;
    case FT_UNSIGNED_CHAR:
      type = init_type (TYPE_CODE_INT,
			TARGET_CHAR_BIT / TARGET_CHAR_BIT,
			TYPE_FLAG_UNSIGNED, "unsigned char", objfile);
      break;
    case FT_SHORT:
      type = init_type (TYPE_CODE_INT,
			gdbarch_short_bit (current_gdbarch) / TARGET_CHAR_BIT,
			0, "short", objfile);
      break;
    case FT_SIGNED_SHORT:
      type = init_type (TYPE_CODE_INT,
			gdbarch_short_bit (current_gdbarch) / TARGET_CHAR_BIT,
			0, "short", objfile);	/* FIXME-fnf */
      break;
    case FT_UNSIGNED_SHORT:
      type = init_type (TYPE_CODE_INT,
			gdbarch_short_bit (current_gdbarch) / TARGET_CHAR_BIT,
			TYPE_FLAG_UNSIGNED, "unsigned short", objfile);
      break;
    case FT_INTEGER:
      type = init_type (TYPE_CODE_INT,
			gdbarch_int_bit (current_gdbarch) / TARGET_CHAR_BIT,
			0, "int", objfile);
      break;
    case FT_SIGNED_INTEGER:
      type = init_type (TYPE_CODE_INT,
			gdbarch_int_bit (current_gdbarch) / TARGET_CHAR_BIT,
			0, "int", objfile);	/* FIXME -fnf */
      break;
    case FT_UNSIGNED_INTEGER:
      type = init_type (TYPE_CODE_INT,
			gdbarch_int_bit (current_gdbarch) / TARGET_CHAR_BIT,
			TYPE_FLAG_UNSIGNED, "unsigned int", objfile);
      break;
    case FT_FIXED_DECIMAL:
      type = init_type (TYPE_CODE_INT,
			gdbarch_int_bit (current_gdbarch) / TARGET_CHAR_BIT,
			0, "fixed decimal", objfile);
      break;
    case FT_LONG:
      type = init_type (TYPE_CODE_INT,
			gdbarch_long_bit (current_gdbarch) / TARGET_CHAR_BIT,
			0, "long", objfile);
      break;
    case FT_SIGNED_LONG:
      type = init_type (TYPE_CODE_INT,
			gdbarch_long_bit (current_gdbarch) / TARGET_CHAR_BIT,
			0, "long", objfile);	/* FIXME -fnf */
      break;
    case FT_UNSIGNED_LONG:
      type = init_type (TYPE_CODE_INT,
			gdbarch_long_bit (current_gdbarch) / TARGET_CHAR_BIT,
			TYPE_FLAG_UNSIGNED, "unsigned long", objfile);
      break;
    case FT_LONG_LONG:
      type = init_type (TYPE_CODE_INT,
			gdbarch_long_long_bit (current_gdbarch)
			  / TARGET_CHAR_BIT,
			0, "long long", objfile);
      break;
    case FT_SIGNED_LONG_LONG:
      type = init_type (TYPE_CODE_INT,
			gdbarch_long_long_bit (current_gdbarch)
			  / TARGET_CHAR_BIT,
			0, "signed long long", objfile);
      break;
    case FT_UNSIGNED_LONG_LONG:
      type = init_type (TYPE_CODE_INT,
			gdbarch_long_long_bit (current_gdbarch)
			  / TARGET_CHAR_BIT,
			TYPE_FLAG_UNSIGNED, "unsigned long long", objfile);
      break;
    case FT_FLOAT:
      type = init_type (TYPE_CODE_FLT,
			gdbarch_float_bit (current_gdbarch) / TARGET_CHAR_BIT,
			0, "float", objfile);
      break;
    case FT_DBL_PREC_FLOAT:
      type = init_type (TYPE_CODE_FLT,
			gdbarch_double_bit (current_gdbarch) / TARGET_CHAR_BIT,
			0, "double", objfile);
      break;
    case FT_FLOAT_DECIMAL:
      type = init_type (TYPE_CODE_FLT,
			gdbarch_double_bit (current_gdbarch) / TARGET_CHAR_BIT,
			0, "floating decimal", objfile);
      break;
    case FT_EXT_PREC_FLOAT:
      type = init_type (TYPE_CODE_FLT,
			gdbarch_long_double_bit (current_gdbarch)
			  / TARGET_CHAR_BIT,
			0, "long double", objfile);
      break;
    case FT_COMPLEX:
      type = init_type (TYPE_CODE_COMPLEX,
			2 * gdbarch_float_bit (current_gdbarch)
			  / TARGET_CHAR_BIT,
			0, "complex", objfile);
      TYPE_TARGET_TYPE (type)
	= m2_create_fundamental_type (objfile, FT_FLOAT);
      break;
    case FT_DBL_PREC_COMPLEX:
      type = init_type (TYPE_CODE_COMPLEX,
			2 * gdbarch_double_bit (current_gdbarch)
			  / TARGET_CHAR_BIT,
			0, "double complex", objfile);
      TYPE_TARGET_TYPE (type)
	= m2_create_fundamental_type (objfile, FT_DBL_PREC_FLOAT);
      break;
    case FT_EXT_PREC_COMPLEX:
      type = init_type (TYPE_CODE_COMPLEX,
			2 * gdbarch_long_double_bit (current_gdbarch)
			  / TARGET_CHAR_BIT,
			0, "long double complex", objfile);
      TYPE_TARGET_TYPE (type)
	= m2_create_fundamental_type (objfile, FT_EXT_PREC_FLOAT);
      break;
    }
  return (type);
}


/* Table of operators and their precedences for printing expressions.  */

static const struct op_print m2_op_print_tab[] =
{
  {"+", BINOP_ADD, PREC_ADD, 0},
  {"+", UNOP_PLUS, PREC_PREFIX, 0},
  {"-", BINOP_SUB, PREC_ADD, 0},
  {"-", UNOP_NEG, PREC_PREFIX, 0},
  {"*", BINOP_MUL, PREC_MUL, 0},
  {"/", BINOP_DIV, PREC_MUL, 0},
  {"DIV", BINOP_INTDIV, PREC_MUL, 0},
  {"MOD", BINOP_REM, PREC_MUL, 0},
  {":=", BINOP_ASSIGN, PREC_ASSIGN, 1},
  {"OR", BINOP_LOGICAL_OR, PREC_LOGICAL_OR, 0},
  {"AND", BINOP_LOGICAL_AND, PREC_LOGICAL_AND, 0},
  {"NOT", UNOP_LOGICAL_NOT, PREC_PREFIX, 0},
  {"=", BINOP_EQUAL, PREC_EQUAL, 0},
  {"<>", BINOP_NOTEQUAL, PREC_EQUAL, 0},
  {"<=", BINOP_LEQ, PREC_ORDER, 0},
  {">=", BINOP_GEQ, PREC_ORDER, 0},
  {">", BINOP_GTR, PREC_ORDER, 0},
  {"<", BINOP_LESS, PREC_ORDER, 0},
  {"^", UNOP_IND, PREC_PREFIX, 0},
  {"@", BINOP_REPEAT, PREC_REPEAT, 0},
  {"CAP", UNOP_CAP, PREC_BUILTIN_FUNCTION, 0},
  {"CHR", UNOP_CHR, PREC_BUILTIN_FUNCTION, 0},
  {"ORD", UNOP_ORD, PREC_BUILTIN_FUNCTION, 0},
  {"FLOAT", UNOP_FLOAT, PREC_BUILTIN_FUNCTION, 0},
  {"HIGH", UNOP_HIGH, PREC_BUILTIN_FUNCTION, 0},
  {"MAX", UNOP_MAX, PREC_BUILTIN_FUNCTION, 0},
  {"MIN", UNOP_MIN, PREC_BUILTIN_FUNCTION, 0},
  {"ODD", UNOP_ODD, PREC_BUILTIN_FUNCTION, 0},
  {"TRUNC", UNOP_TRUNC, PREC_BUILTIN_FUNCTION, 0},
  {NULL, 0, 0, 0}
};

/* The built-in types of Modula-2.  */

enum m2_primitive_types {
  m2_primitive_type_char,
  m2_primitive_type_int,
  m2_primitive_type_card,
  m2_primitive_type_real,
  m2_primitive_type_bool,
  nr_m2_primitive_types
};

static void
m2_language_arch_info (struct gdbarch *gdbarch,
		       struct language_arch_info *lai)
{
  const struct builtin_m2_type *builtin = builtin_m2_type (gdbarch);

  lai->string_char_type = builtin->builtin_char;
  lai->primitive_type_vector
    = GDBARCH_OBSTACK_CALLOC (gdbarch, nr_m2_primitive_types + 1,
                              struct type *);

  lai->primitive_type_vector [m2_primitive_type_char]
    = builtin->builtin_char;
  lai->primitive_type_vector [m2_primitive_type_int]
    = builtin->builtin_int;
  lai->primitive_type_vector [m2_primitive_type_card]
    = builtin->builtin_card;
  lai->primitive_type_vector [m2_primitive_type_real]
    = builtin->builtin_real;
  lai->primitive_type_vector [m2_primitive_type_bool]
    = builtin->builtin_bool;
}

const struct exp_descriptor exp_descriptor_modula2 = 
{
  print_subexp_standard,
  operator_length_standard,
  op_name_standard,
  dump_subexp_body_standard,
  evaluate_subexp_modula2
};

const struct language_defn m2_language_defn =
{
  "modula-2",
  language_m2,
  range_check_on,
  type_check_on,
  case_sensitive_on,
  array_row_major,
  &exp_descriptor_modula2,
  m2_parse,			/* parser */
  m2_error,			/* parser error function */
  null_post_parser,
  m2_printchar,			/* Print character constant */
  m2_printstr,			/* function to print string constant */
  m2_emit_char,			/* Function to print a single character */
  m2_create_fundamental_type,	/* Create fundamental type in this language */
  m2_print_type,		/* Print a type using appropriate syntax */
  m2_val_print,			/* Print a value using appropriate syntax */
  c_value_print,		/* Print a top-level value */
  NULL,				/* Language specific skip_trampoline */
  value_of_this,		/* value_of_this */
  basic_lookup_symbol_nonlocal,	/* lookup_symbol_nonlocal */
  basic_lookup_transparent_type,/* lookup_transparent_type */
  NULL,				/* Language specific symbol demangler */
  NULL,				/* Language specific class_name_from_physname */
  m2_op_print_tab,		/* expression operators for printing */
  0,				/* arrays are first-class (not c-style) */
  0,				/* String lower bound */
  default_word_break_characters,
  m2_language_arch_info,
  default_print_array_index,
  default_pass_by_reference,
  LANG_MAGIC
};

static void *
build_m2_types (struct gdbarch *gdbarch)
{
  struct builtin_m2_type *builtin_m2_type
    = GDBARCH_OBSTACK_ZALLOC (gdbarch, struct builtin_m2_type);

  /* Modula-2 "pervasive" types.  NOTE:  these can be redefined!!! */
  builtin_m2_type->builtin_int =
    init_type (TYPE_CODE_INT,
	       gdbarch_int_bit (gdbarch) / TARGET_CHAR_BIT,
	       0, "INTEGER", (struct objfile *) NULL);
  builtin_m2_type->builtin_card =
    init_type (TYPE_CODE_INT, 
	       gdbarch_int_bit (gdbarch) / TARGET_CHAR_BIT,
	       TYPE_FLAG_UNSIGNED,
	       "CARDINAL", (struct objfile *) NULL);
  builtin_m2_type->builtin_real =
    init_type (TYPE_CODE_FLT,
	       gdbarch_float_bit (gdbarch) / TARGET_CHAR_BIT,
	       0,
	       "REAL", (struct objfile *) NULL);
  builtin_m2_type->builtin_char =
    init_type (TYPE_CODE_CHAR, TARGET_CHAR_BIT / TARGET_CHAR_BIT,
	       TYPE_FLAG_UNSIGNED,
	       "CHAR", (struct objfile *) NULL);
  builtin_m2_type->builtin_bool =
    init_type (TYPE_CODE_BOOL, 
	       gdbarch_int_bit (gdbarch) / TARGET_CHAR_BIT,
	       TYPE_FLAG_UNSIGNED,
	       "BOOLEAN", (struct objfile *) NULL);

  return builtin_m2_type;
}

static struct gdbarch_data *m2_type_data;

const struct builtin_m2_type *
builtin_m2_type (struct gdbarch *gdbarch)
{
  return gdbarch_data (gdbarch, m2_type_data);
}


/* Initialization for Modula-2 */

void
_initialize_m2_language (void)
{
  m2_type_data = gdbarch_data_register_post_init (build_m2_types);

  add_language (&m2_language_defn);
}
