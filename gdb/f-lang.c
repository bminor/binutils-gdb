/* Fortran language support routines for GDB, the GNU debugger.

   Copyright (C) 1993-2020 Free Software Foundation, Inc.

   Contributed by Motorola.  Adapted from the C parser by Farooq Butt
   (fmbutt@engage.sps.mot.com).

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
#include "varobj.h"
#include "gdbcore.h"
#include "f-lang.h"
#include "valprint.h"
#include "value.h"
#include "cp-support.h"
#include "charset.h"
#include "c-lang.h"
#include "target-float.h"
#include "gdbarch.h"

#include <math.h>

/* Local functions */

/* Return the encoding that should be used for the character type
   TYPE.  */

const char *
f_language::get_encoding (struct type *type)
{
  const char *encoding;

  switch (TYPE_LENGTH (type))
    {
    case 1:
      encoding = target_charset (get_type_arch (type));
      break;
    case 4:
      if (type_byte_order (type) == BFD_ENDIAN_BIG)
	encoding = "UTF-32BE";
      else
	encoding = "UTF-32LE";
      break;

    default:
      error (_("unrecognized character type"));
    }

  return encoding;
}



/* Table of operators and their precedences for printing expressions.  */

const struct op_print f_language::op_print_tab[] =
{
  {"+", BINOP_ADD, PREC_ADD, 0},
  {"+", UNOP_PLUS, PREC_PREFIX, 0},
  {"-", BINOP_SUB, PREC_ADD, 0},
  {"-", UNOP_NEG, PREC_PREFIX, 0},
  {"*", BINOP_MUL, PREC_MUL, 0},
  {"/", BINOP_DIV, PREC_MUL, 0},
  {"DIV", BINOP_INTDIV, PREC_MUL, 0},
  {"MOD", BINOP_REM, PREC_MUL, 0},
  {"=", BINOP_ASSIGN, PREC_ASSIGN, 1},
  {".OR.", BINOP_LOGICAL_OR, PREC_LOGICAL_OR, 0},
  {".AND.", BINOP_LOGICAL_AND, PREC_LOGICAL_AND, 0},
  {".NOT.", UNOP_LOGICAL_NOT, PREC_PREFIX, 0},
  {".EQ.", BINOP_EQUAL, PREC_EQUAL, 0},
  {".NE.", BINOP_NOTEQUAL, PREC_EQUAL, 0},
  {".LE.", BINOP_LEQ, PREC_ORDER, 0},
  {".GE.", BINOP_GEQ, PREC_ORDER, 0},
  {".GT.", BINOP_GTR, PREC_ORDER, 0},
  {".LT.", BINOP_LESS, PREC_ORDER, 0},
  {"**", UNOP_IND, PREC_PREFIX, 0},
  {"@", BINOP_REPEAT, PREC_REPEAT, 0},
  {NULL, OP_NULL, PREC_REPEAT, 0}
};

enum f_primitive_types {
  f_primitive_type_character,
  f_primitive_type_logical,
  f_primitive_type_logical_s1,
  f_primitive_type_logical_s2,
  f_primitive_type_logical_s8,
  f_primitive_type_integer,
  f_primitive_type_integer_s2,
  f_primitive_type_real,
  f_primitive_type_real_s8,
  f_primitive_type_real_s16,
  f_primitive_type_complex_s8,
  f_primitive_type_complex_s16,
  f_primitive_type_void,
  nr_f_primitive_types
};

/* Called from fortran_value_subarray to take a slice of an array or a
   string.  ARRAY is the array or string to be accessed.  EXP, POS, and
   NOSIDE are as for evaluate_subexp_standard.  Return a value that is a
   slice of the array.  */

static struct value *
value_f90_subarray (struct value *array,
		    struct expression *exp, int *pos, enum noside noside)
{
  int pc = (*pos) + 1;
  LONGEST low_bound, high_bound, stride;
  struct type *range = check_typedef (value_type (array)->index_type ());
  enum range_flag range_flag
    = (enum range_flag) longest_to_int (exp->elts[pc].longconst);

  *pos += 3;

  if (range_flag & RANGE_LOW_BOUND_DEFAULT)
    low_bound = range->bounds ()->low.const_val ();
  else
    low_bound = value_as_long (evaluate_subexp (nullptr, exp, pos, noside));

  if (range_flag & RANGE_HIGH_BOUND_DEFAULT)
    high_bound = range->bounds ()->high.const_val ();
  else
    high_bound = value_as_long (evaluate_subexp (nullptr, exp, pos, noside));

  if (range_flag & RANGE_HAS_STRIDE)
    stride = value_as_long (evaluate_subexp (nullptr, exp, pos, noside));
  else
    stride = 1;

  if (stride != 1)
    error (_("Fortran array strides are not currently supported"));

  return value_slice (array, low_bound, high_bound - low_bound + 1);
}

/* Helper for skipping all the arguments in an undetermined argument list.
   This function was designed for use in the OP_F77_UNDETERMINED_ARGLIST
   case of evaluate_subexp_standard as multiple, but not all, code paths
   require a generic skip.  */

static void
skip_undetermined_arglist (int nargs, struct expression *exp, int *pos,
			   enum noside noside)
{
  for (int i = 0; i < nargs; ++i)
    evaluate_subexp (nullptr, exp, pos, noside);
}

/* Return the number of dimensions for a Fortran array or string.  */

int
calc_f77_array_dims (struct type *array_type)
{
  int ndimen = 1;
  struct type *tmp_type;

  if ((array_type->code () == TYPE_CODE_STRING))
    return 1;

  if ((array_type->code () != TYPE_CODE_ARRAY))
    error (_("Can't get dimensions for a non-array type"));

  tmp_type = array_type;

  while ((tmp_type = TYPE_TARGET_TYPE (tmp_type)))
    {
      if (tmp_type->code () == TYPE_CODE_ARRAY)
	++ndimen;
    }
  return ndimen;
}

/* Called from evaluate_subexp_standard to perform array indexing, and
   sub-range extraction, for Fortran.  As well as arrays this function
   also handles strings as they can be treated like arrays of characters.
   ARRAY is the array or string being accessed.  EXP, POS, and NOSIDE are
   as for evaluate_subexp_standard, and NARGS is the number of arguments
   in this access (e.g. 'array (1,2,3)' would be NARGS 3).  */

static struct value *
fortran_value_subarray (struct value *array, struct expression *exp,
			int *pos, int nargs, enum noside noside)
{
  if (exp->elts[*pos].opcode == OP_RANGE)
    return value_f90_subarray (array, exp, pos, noside);

  if (noside == EVAL_SKIP)
    {
      skip_undetermined_arglist (nargs, exp, pos, noside);
      /* Return the dummy value with the correct type.  */
      return array;
    }

  LONGEST subscript_array[MAX_FORTRAN_DIMS];
  int ndimensions = 1;
  struct type *type = check_typedef (value_type (array));

  if (nargs > MAX_FORTRAN_DIMS)
    error (_("Too many subscripts for F77 (%d Max)"), MAX_FORTRAN_DIMS);

  ndimensions = calc_f77_array_dims (type);

  if (nargs != ndimensions)
    error (_("Wrong number of subscripts"));

  gdb_assert (nargs > 0);

  /* Now that we know we have a legal array subscript expression let us
     actually find out where this element exists in the array.  */

  /* Take array indices left to right.  */
  for (int i = 0; i < nargs; i++)
    {
      /* Evaluate each subscript; it must be a legal integer in F77.  */
      value *arg2 = evaluate_subexp_with_coercion (exp, pos, noside);

      /* Fill in the subscript array.  */
      subscript_array[i] = value_as_long (arg2);
    }

  /* Internal type of array is arranged right to left.  */
  for (int i = nargs; i > 0; i--)
    {
      struct type *array_type = check_typedef (value_type (array));
      LONGEST index = subscript_array[i - 1];

      array = value_subscripted_rvalue (array, index,
					f77_get_lowerbound (array_type));
    }

  return array;
}

/* Special expression evaluation cases for Fortran.  */

static struct value *
evaluate_subexp_f (struct type *expect_type, struct expression *exp,
		   int *pos, enum noside noside)
{
  struct value *arg1 = NULL, *arg2 = NULL;
  enum exp_opcode op;
  int pc;
  struct type *type;

  pc = *pos;
  *pos += 1;
  op = exp->elts[pc].opcode;

  switch (op)
    {
    default:
      *pos -= 1;
      return evaluate_subexp_standard (expect_type, exp, pos, noside);

    case UNOP_ABS:
      arg1 = evaluate_subexp (nullptr, exp, pos, noside);
      if (noside == EVAL_SKIP)
	return eval_skip_value (exp);
      type = value_type (arg1);
      switch (type->code ())
	{
	case TYPE_CODE_FLT:
	  {
	    double d
	      = fabs (target_float_to_host_double (value_contents (arg1),
						   value_type (arg1)));
	    return value_from_host_double (type, d);
	  }
	case TYPE_CODE_INT:
	  {
	    LONGEST l = value_as_long (arg1);
	    l = llabs (l);
	    return value_from_longest (type, l);
	  }
	}
      error (_("ABS of type %s not supported"), TYPE_SAFE_NAME (type));

    case BINOP_MOD:
      arg1 = evaluate_subexp (nullptr, exp, pos, noside);
      arg2 = evaluate_subexp (value_type (arg1), exp, pos, noside);
      if (noside == EVAL_SKIP)
	return eval_skip_value (exp);
      type = value_type (arg1);
      if (type->code () != value_type (arg2)->code ())
	error (_("non-matching types for parameters to MOD ()"));
      switch (type->code ())
	{
	case TYPE_CODE_FLT:
	  {
	    double d1
	      = target_float_to_host_double (value_contents (arg1),
					     value_type (arg1));
	    double d2
	      = target_float_to_host_double (value_contents (arg2),
					     value_type (arg2));
	    double d3 = fmod (d1, d2);
	    return value_from_host_double (type, d3);
	  }
	case TYPE_CODE_INT:
	  {
	    LONGEST v1 = value_as_long (arg1);
	    LONGEST v2 = value_as_long (arg2);
	    if (v2 == 0)
	      error (_("calling MOD (N, 0) is undefined"));
	    LONGEST v3 = v1 - (v1 / v2) * v2;
	    return value_from_longest (value_type (arg1), v3);
	  }
	}
      error (_("MOD of type %s not supported"), TYPE_SAFE_NAME (type));

    case UNOP_FORTRAN_CEILING:
      {
	arg1 = evaluate_subexp (nullptr, exp, pos, noside);
	if (noside == EVAL_SKIP)
	  return eval_skip_value (exp);
	type = value_type (arg1);
	if (type->code () != TYPE_CODE_FLT)
	  error (_("argument to CEILING must be of type float"));
	double val
	  = target_float_to_host_double (value_contents (arg1),
					 value_type (arg1));
	val = ceil (val);
	return value_from_host_double (type, val);
      }

    case UNOP_FORTRAN_FLOOR:
      {
	arg1 = evaluate_subexp (nullptr, exp, pos, noside);
	if (noside == EVAL_SKIP)
	  return eval_skip_value (exp);
	type = value_type (arg1);
	if (type->code () != TYPE_CODE_FLT)
	  error (_("argument to FLOOR must be of type float"));
	double val
	  = target_float_to_host_double (value_contents (arg1),
					 value_type (arg1));
	val = floor (val);
	return value_from_host_double (type, val);
      }

    case BINOP_FORTRAN_MODULO:
      {
	arg1 = evaluate_subexp (nullptr, exp, pos, noside);
	arg2 = evaluate_subexp (value_type (arg1), exp, pos, noside);
	if (noside == EVAL_SKIP)
	  return eval_skip_value (exp);
	type = value_type (arg1);
	if (type->code () != value_type (arg2)->code ())
	  error (_("non-matching types for parameters to MODULO ()"));
	/* MODULO(A, P) = A - FLOOR (A / P) * P */
	switch (type->code ())
	  {
	  case TYPE_CODE_INT:
	    {
	      LONGEST a = value_as_long (arg1);
	      LONGEST p = value_as_long (arg2);
	      LONGEST result = a - (a / p) * p;
	      if (result != 0 && (a < 0) != (p < 0))
		result += p;
	      return value_from_longest (value_type (arg1), result);
	    }
	  case TYPE_CODE_FLT:
	    {
	      double a
		= target_float_to_host_double (value_contents (arg1),
					       value_type (arg1));
	      double p
		= target_float_to_host_double (value_contents (arg2),
					       value_type (arg2));
	      double result = fmod (a, p);
	      if (result != 0 && (a < 0.0) != (p < 0.0))
		result += p;
	      return value_from_host_double (type, result);
	    }
	  }
	error (_("MODULO of type %s not supported"), TYPE_SAFE_NAME (type));
      }

    case BINOP_FORTRAN_CMPLX:
      arg1 = evaluate_subexp (nullptr, exp, pos, noside);
      arg2 = evaluate_subexp (value_type (arg1), exp, pos, noside);
      if (noside == EVAL_SKIP)
	return eval_skip_value (exp);
      type = builtin_f_type(exp->gdbarch)->builtin_complex_s16;
      return value_literal_complex (arg1, arg2, type);

    case UNOP_FORTRAN_KIND:
      arg1 = evaluate_subexp (NULL, exp, pos, EVAL_AVOID_SIDE_EFFECTS);
      type = value_type (arg1);

      switch (type->code ())
	{
	  case TYPE_CODE_STRUCT:
	  case TYPE_CODE_UNION:
	  case TYPE_CODE_MODULE:
	  case TYPE_CODE_FUNC:
	    error (_("argument to kind must be an intrinsic type"));
	}

      if (!TYPE_TARGET_TYPE (type))
	return value_from_longest (builtin_type (exp->gdbarch)->builtin_int,
				   TYPE_LENGTH (type));
      return value_from_longest (builtin_type (exp->gdbarch)->builtin_int,
				 TYPE_LENGTH (TYPE_TARGET_TYPE (type)));


    case OP_F77_UNDETERMINED_ARGLIST:
      /* Remember that in F77, functions, substring ops and array subscript
	 operations cannot be disambiguated at parse time.  We have made
	 all array subscript operations, substring operations as well as
	 function calls come here and we now have to discover what the heck
	 this thing actually was.  If it is a function, we process just as
	 if we got an OP_FUNCALL.  */
      int nargs = longest_to_int (exp->elts[pc + 1].longconst);
      (*pos) += 2;

      /* First determine the type code we are dealing with.  */
      arg1 = evaluate_subexp (nullptr, exp, pos, noside);
      type = check_typedef (value_type (arg1));
      enum type_code code = type->code ();

      if (code == TYPE_CODE_PTR)
	{
	  /* Fortran always passes variable to subroutines as pointer.
	     So we need to look into its target type to see if it is
	     array, string or function.  If it is, we need to switch
	     to the target value the original one points to.  */
	  struct type *target_type = check_typedef (TYPE_TARGET_TYPE (type));

	  if (target_type->code () == TYPE_CODE_ARRAY
	      || target_type->code () == TYPE_CODE_STRING
	      || target_type->code () == TYPE_CODE_FUNC)
	    {
	      arg1 = value_ind (arg1);
	      type = check_typedef (value_type (arg1));
	      code = type->code ();
	    }
	}

      switch (code)
	{
	case TYPE_CODE_ARRAY:
	case TYPE_CODE_STRING:
	  return fortran_value_subarray (arg1, exp, pos, nargs, noside);

	case TYPE_CODE_PTR:
	case TYPE_CODE_FUNC:
	case TYPE_CODE_INTERNAL_FUNCTION:
	  {
	    /* It's a function call.  Allocate arg vector, including
	    space for the function to be called in argvec[0] and a
	    termination NULL.  */
	    struct value **argvec = (struct value **)
	      alloca (sizeof (struct value *) * (nargs + 2));
	    argvec[0] = arg1;
	    int tem = 1;
	    for (; tem <= nargs; tem++)
	      {
		argvec[tem] = evaluate_subexp_with_coercion (exp, pos, noside);
		/* Arguments in Fortran are passed by address.  Coerce the
		   arguments here rather than in value_arg_coerce as
		   otherwise the call to malloc to place the non-lvalue
		   parameters in target memory is hit by this Fortran
		   specific logic.  This results in malloc being called
		   with a pointer to an integer followed by an attempt to
		   malloc the arguments to malloc in target memory.
		   Infinite recursion ensues.  */
		if (code == TYPE_CODE_PTR || code == TYPE_CODE_FUNC)
		  {
		    bool is_artificial
		      = TYPE_FIELD_ARTIFICIAL (value_type (arg1), tem - 1);
		    argvec[tem] = fortran_argument_convert (argvec[tem],
							    is_artificial);
		  }
	      }
	    argvec[tem] = 0;	/* signal end of arglist */
	    if (noside == EVAL_SKIP)
	      return eval_skip_value (exp);
	    return evaluate_subexp_do_call (exp, noside, nargs, argvec, NULL,
					    expect_type);
	  }

	default:
	  error (_("Cannot perform substring on this type"));
	}
    }

  /* Should be unreachable.  */
  return nullptr;
}

/* Special expression lengths for Fortran.  */

static void
operator_length_f (const struct expression *exp, int pc, int *oplenp,
		   int *argsp)
{
  int oplen = 1;
  int args = 0;

  switch (exp->elts[pc - 1].opcode)
    {
    default:
      operator_length_standard (exp, pc, oplenp, argsp);
      return;

    case UNOP_FORTRAN_KIND:
    case UNOP_FORTRAN_FLOOR:
    case UNOP_FORTRAN_CEILING:
      oplen = 1;
      args = 1;
      break;

    case BINOP_FORTRAN_CMPLX:
    case BINOP_FORTRAN_MODULO:
      oplen = 1;
      args = 2;
      break;

    case OP_F77_UNDETERMINED_ARGLIST:
      oplen = 3;
      args = 1 + longest_to_int (exp->elts[pc - 2].longconst);
      break;
    }

  *oplenp = oplen;
  *argsp = args;
}

/* Helper for PRINT_SUBEXP_F.  Arguments are as for PRINT_SUBEXP_F, except
   the extra argument NAME which is the text that should be printed as the
   name of this operation.  */

static void
print_unop_subexp_f (struct expression *exp, int *pos,
		     struct ui_file *stream, enum precedence prec,
		     const char *name)
{
  (*pos)++;
  fprintf_filtered (stream, "%s(", name);
  print_subexp (exp, pos, stream, PREC_SUFFIX);
  fputs_filtered (")", stream);
}

/* Helper for PRINT_SUBEXP_F.  Arguments are as for PRINT_SUBEXP_F, except
   the extra argument NAME which is the text that should be printed as the
   name of this operation.  */

static void
print_binop_subexp_f (struct expression *exp, int *pos,
		      struct ui_file *stream, enum precedence prec,
		      const char *name)
{
  (*pos)++;
  fprintf_filtered (stream, "%s(", name);
  print_subexp (exp, pos, stream, PREC_SUFFIX);
  fputs_filtered (",", stream);
  print_subexp (exp, pos, stream, PREC_SUFFIX);
  fputs_filtered (")", stream);
}

/* Special expression printing for Fortran.  */

static void
print_subexp_f (struct expression *exp, int *pos,
		struct ui_file *stream, enum precedence prec)
{
  int pc = *pos;
  enum exp_opcode op = exp->elts[pc].opcode;

  switch (op)
    {
    default:
      print_subexp_standard (exp, pos, stream, prec);
      return;

    case UNOP_FORTRAN_KIND:
      print_unop_subexp_f (exp, pos, stream, prec, "KIND");
      return;

    case UNOP_FORTRAN_FLOOR:
      print_unop_subexp_f (exp, pos, stream, prec, "FLOOR");
      return;

    case UNOP_FORTRAN_CEILING:
      print_unop_subexp_f (exp, pos, stream, prec, "CEILING");
      return;

    case BINOP_FORTRAN_CMPLX:
      print_binop_subexp_f (exp, pos, stream, prec, "CMPLX");
      return;

    case BINOP_FORTRAN_MODULO:
      print_binop_subexp_f (exp, pos, stream, prec, "MODULO");
      return;

    case OP_F77_UNDETERMINED_ARGLIST:
      print_subexp_funcall (exp, pos, stream);
      return;
    }
}

/* Special expression names for Fortran.  */

static const char *
op_name_f (enum exp_opcode opcode)
{
  switch (opcode)
    {
    default:
      return op_name_standard (opcode);

#define OP(name)	\
    case name:		\
      return #name ;
#include "fortran-operator.def"
#undef OP
    }
}

/* Special expression dumping for Fortran.  */

static int
dump_subexp_body_f (struct expression *exp,
		    struct ui_file *stream, int elt)
{
  int opcode = exp->elts[elt].opcode;
  int oplen, nargs, i;

  switch (opcode)
    {
    default:
      return dump_subexp_body_standard (exp, stream, elt);

    case UNOP_FORTRAN_KIND:
    case UNOP_FORTRAN_FLOOR:
    case UNOP_FORTRAN_CEILING:
    case BINOP_FORTRAN_CMPLX:
    case BINOP_FORTRAN_MODULO:
      operator_length_f (exp, (elt + 1), &oplen, &nargs);
      break;

    case OP_F77_UNDETERMINED_ARGLIST:
      return dump_subexp_body_funcall (exp, stream, elt);
    }

  elt += oplen;
  for (i = 0; i < nargs; i += 1)
    elt = dump_subexp (exp, stream, elt);

  return elt;
}

/* Special expression checking for Fortran.  */

static int
operator_check_f (struct expression *exp, int pos,
		  int (*objfile_func) (struct objfile *objfile,
				       void *data),
		  void *data)
{
  const union exp_element *const elts = exp->elts;

  switch (elts[pos].opcode)
    {
    case UNOP_FORTRAN_KIND:
    case UNOP_FORTRAN_FLOOR:
    case UNOP_FORTRAN_CEILING:
    case BINOP_FORTRAN_CMPLX:
    case BINOP_FORTRAN_MODULO:
      /* Any references to objfiles are held in the arguments to this
	 expression, not within the expression itself, so no additional
	 checking is required here, the outer expression iteration code
	 will take care of checking each argument.  */
      break;

    default:
      return operator_check_standard (exp, pos, objfile_func, data);
    }

  return 0;
}

/* Expression processing for Fortran.  */
const struct exp_descriptor f_language::exp_descriptor_tab =
{
  print_subexp_f,
  operator_length_f,
  operator_check_f,
  op_name_f,
  dump_subexp_body_f,
  evaluate_subexp_f
};

/* See language.h.  */

void
f_language::language_arch_info (struct gdbarch *gdbarch,
				struct language_arch_info *lai) const
{
  const struct builtin_f_type *builtin = builtin_f_type (gdbarch);

  lai->string_char_type = builtin->builtin_character;
  lai->primitive_type_vector
    = GDBARCH_OBSTACK_CALLOC (gdbarch, nr_f_primitive_types + 1,
			      struct type *);

  lai->primitive_type_vector [f_primitive_type_character]
    = builtin->builtin_character;
  lai->primitive_type_vector [f_primitive_type_logical]
    = builtin->builtin_logical;
  lai->primitive_type_vector [f_primitive_type_logical_s1]
    = builtin->builtin_logical_s1;
  lai->primitive_type_vector [f_primitive_type_logical_s2]
    = builtin->builtin_logical_s2;
  lai->primitive_type_vector [f_primitive_type_logical_s8]
    = builtin->builtin_logical_s8;
  lai->primitive_type_vector [f_primitive_type_real]
    = builtin->builtin_real;
  lai->primitive_type_vector [f_primitive_type_real_s8]
    = builtin->builtin_real_s8;
  lai->primitive_type_vector [f_primitive_type_real_s16]
    = builtin->builtin_real_s16;
  lai->primitive_type_vector [f_primitive_type_complex_s8]
    = builtin->builtin_complex_s8;
  lai->primitive_type_vector [f_primitive_type_complex_s16]
    = builtin->builtin_complex_s16;
  lai->primitive_type_vector [f_primitive_type_void]
    = builtin->builtin_void;

  lai->bool_type_symbol = "logical";
  lai->bool_type_default = builtin->builtin_logical_s2;
}

/* See language.h.  */

unsigned int
f_language::search_name_hash (const char *name) const
{
  return cp_search_name_hash (name);
}

/* See language.h.  */

struct block_symbol
f_language::lookup_symbol_nonlocal (const char *name,
				    const struct block *block,
				    const domain_enum domain) const
{
  return cp_lookup_symbol_nonlocal (this, name, block, domain);
}

/* See language.h.  */

symbol_name_matcher_ftype *
f_language::get_symbol_name_matcher_inner
	(const lookup_name_info &lookup_name) const
{
  return cp_get_symbol_name_matcher (lookup_name);
}

/* Single instance of the Fortran language class.  */

static f_language f_language_defn;

static void *
build_fortran_types (struct gdbarch *gdbarch)
{
  struct builtin_f_type *builtin_f_type
    = GDBARCH_OBSTACK_ZALLOC (gdbarch, struct builtin_f_type);

  builtin_f_type->builtin_void
    = arch_type (gdbarch, TYPE_CODE_VOID, TARGET_CHAR_BIT, "void");

  builtin_f_type->builtin_character
    = arch_type (gdbarch, TYPE_CODE_CHAR, TARGET_CHAR_BIT, "character");

  builtin_f_type->builtin_logical_s1
    = arch_boolean_type (gdbarch, TARGET_CHAR_BIT, 1, "logical*1");

  builtin_f_type->builtin_integer_s2
    = arch_integer_type (gdbarch, gdbarch_short_bit (gdbarch), 0,
			 "integer*2");

  builtin_f_type->builtin_integer_s8
    = arch_integer_type (gdbarch, gdbarch_long_long_bit (gdbarch), 0,
			 "integer*8");

  builtin_f_type->builtin_logical_s2
    = arch_boolean_type (gdbarch, gdbarch_short_bit (gdbarch), 1,
			 "logical*2");

  builtin_f_type->builtin_logical_s8
    = arch_boolean_type (gdbarch, gdbarch_long_long_bit (gdbarch), 1,
			 "logical*8");

  builtin_f_type->builtin_integer
    = arch_integer_type (gdbarch, gdbarch_int_bit (gdbarch), 0,
			 "integer");

  builtin_f_type->builtin_logical
    = arch_boolean_type (gdbarch, gdbarch_int_bit (gdbarch), 1,
			 "logical*4");

  builtin_f_type->builtin_real
    = arch_float_type (gdbarch, gdbarch_float_bit (gdbarch),
		       "real", gdbarch_float_format (gdbarch));
  builtin_f_type->builtin_real_s8
    = arch_float_type (gdbarch, gdbarch_double_bit (gdbarch),
		       "real*8", gdbarch_double_format (gdbarch));
  auto fmt = gdbarch_floatformat_for_type (gdbarch, "real(kind=16)", 128);
  if (fmt != nullptr)
    builtin_f_type->builtin_real_s16
      = arch_float_type (gdbarch, 128, "real*16", fmt);
  else if (gdbarch_long_double_bit (gdbarch) == 128)
    builtin_f_type->builtin_real_s16
      = arch_float_type (gdbarch, gdbarch_long_double_bit (gdbarch),
			 "real*16", gdbarch_long_double_format (gdbarch));
  else
    builtin_f_type->builtin_real_s16
      = arch_type (gdbarch, TYPE_CODE_ERROR, 128, "real*16");

  builtin_f_type->builtin_complex_s8
    = init_complex_type ("complex*8", builtin_f_type->builtin_real);
  builtin_f_type->builtin_complex_s16
    = init_complex_type ("complex*16", builtin_f_type->builtin_real_s8);

  if (builtin_f_type->builtin_real_s16->code () == TYPE_CODE_ERROR)
    builtin_f_type->builtin_complex_s32
      = arch_type (gdbarch, TYPE_CODE_ERROR, 256, "complex*32");
  else
    builtin_f_type->builtin_complex_s32
      = init_complex_type ("complex*32", builtin_f_type->builtin_real_s16);

  return builtin_f_type;
}

static struct gdbarch_data *f_type_data;

const struct builtin_f_type *
builtin_f_type (struct gdbarch *gdbarch)
{
  return (const struct builtin_f_type *) gdbarch_data (gdbarch, f_type_data);
}

void _initialize_f_language ();
void
_initialize_f_language ()
{
  f_type_data = gdbarch_data_register_post_init (build_fortran_types);
}

/* See f-lang.h.  */

struct value *
fortran_argument_convert (struct value *value, bool is_artificial)
{
  if (!is_artificial)
    {
      /* If the value is not in the inferior e.g. registers values,
	 convenience variables and user input.  */
      if (VALUE_LVAL (value) != lval_memory)
	{
	  struct type *type = value_type (value);
	  const int length = TYPE_LENGTH (type);
	  const CORE_ADDR addr
	    = value_as_long (value_allocate_space_in_inferior (length));
	  write_memory (addr, value_contents (value), length);
	  struct value *val
	    = value_from_contents_and_address (type, value_contents (value),
					       addr);
	  return value_addr (val);
	}
      else
	return value_addr (value); /* Program variables, e.g. arrays.  */
    }
    return value;
}

/* See f-lang.h.  */

struct type *
fortran_preserve_arg_pointer (struct value *arg, struct type *type)
{
  if (value_type (arg)->code () == TYPE_CODE_PTR)
    return value_type (arg);
  return type;
}
