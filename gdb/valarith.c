/* Perform arithmetic and other operations on values, for GDB.
   Copyright (C) 1986 Free Software Foundation, Inc.

GDB is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY.  No author or distributor accepts responsibility to anyone
for the consequences of using it or for whether it serves any
particular purpose or works at all, unless he says so in writing.
Refer to the GDB General Public License for full details.

Everyone is granted permission to copy, modify and redistribute GDB,
but only under the conditions described in the GDB General Public
License.  A copy of this license is supposed to have been given to you
along with GDB so you can know your rights and responsibilities.  It
should be in a file named COPYING.  Among other things, the copyright
notice and this notice must be preserved on all copies.

In other words, go ahead and share GDB, but don't try to stop
anyone else from sharing it farther.  Help stamp out software hoarding!
*/

#include "defs.h"
#include "initialize.h"
#include "param.h"
#include "symtab.h"
#include "value.h"
#include "expression.h"

START_FILE

value
value_add (arg1, arg2)
	value arg1, arg2;
{
  register value val, valint, valptr;
  register int len;

  COERCE_ARRAY (arg1);
  COERCE_ARRAY (arg2);

  if ((TYPE_CODE (VALUE_TYPE (arg1)) == TYPE_CODE_PTR
       || TYPE_CODE (VALUE_TYPE (arg2)) == TYPE_CODE_PTR)
      &&
      (TYPE_CODE (VALUE_TYPE (arg1)) == TYPE_CODE_INT
       || TYPE_CODE (VALUE_TYPE (arg2)) == TYPE_CODE_INT))
    /* Exactly one argument is a pointer, and one is an integer.  */
    {
      if (TYPE_CODE (VALUE_TYPE (arg1)) == TYPE_CODE_PTR)
	{
	  valptr = arg1;
	  valint = arg2;
	}
      else
	{
	  valptr = arg2;
	  valint = arg1;
	}
      len = TYPE_LENGTH (TYPE_TARGET_TYPE (VALUE_TYPE (valptr)));
      if (len == 0) len = 1;	/* For (void *) */
      val = value_from_long (builtin_type_long,
			     value_as_long (valptr)
			     + (len * value_as_long (valint)));
      VALUE_TYPE (val) = VALUE_TYPE (valptr);
      return val;
    }

  return value_binop (arg1, arg2, BINOP_ADD);
}

value
value_sub (arg1, arg2)
	value arg1, arg2;
{
  register value val;

  COERCE_ARRAY (arg1);
  COERCE_ARRAY (arg2);

  if (TYPE_CODE (VALUE_TYPE (arg1)) == TYPE_CODE_PTR
      && 
      TYPE_CODE (VALUE_TYPE (arg2)) == TYPE_CODE_INT)
    {
      val = value_from_long (builtin_type_long,
			     value_as_long (arg1)
			     - TYPE_LENGTH (TYPE_TARGET_TYPE (VALUE_TYPE (arg1))) * value_as_long (arg2));
      VALUE_TYPE (val) = VALUE_TYPE (arg1);
      return val;
    }

  if (TYPE_CODE (VALUE_TYPE (arg1)) == TYPE_CODE_PTR
      && 
      VALUE_TYPE (arg1) == VALUE_TYPE (arg2))
    {
      val = value_from_long (builtin_type_long,
			     (value_as_long (arg1) - value_as_long (arg2))
			     / TYPE_LENGTH (TYPE_TARGET_TYPE (VALUE_TYPE (arg1))));
      return val;
    }

  return value_binop (arg1, arg2, BINOP_SUB);
}

/* Return the value of ARRAY[IDX].  */

value
value_subscript (array, idx)
     value array, idx;
{
  return value_ind (value_add (array, idx));
}

/* Perform a binary operation on two integers or two floats.
   Does not support addition and subtraction on pointers;
   use value_add or value_sub if you want to handle those possibilities.  */

value
value_binop (arg1, arg2, op)
     value arg1, arg2;
     int op;
{
  register value val;

  COERCE_ENUM (arg1);
  COERCE_ENUM (arg2);

  if ((TYPE_CODE (VALUE_TYPE (arg1)) != TYPE_CODE_FLT
       &&
       TYPE_CODE (VALUE_TYPE (arg1)) != TYPE_CODE_INT)
      ||
      (TYPE_CODE (VALUE_TYPE (arg2)) != TYPE_CODE_FLT
       &&
       TYPE_CODE (VALUE_TYPE (arg2)) != TYPE_CODE_INT))
    error ("Argument to arithmetic operation not a number.");

  if (TYPE_CODE (VALUE_TYPE (arg1)) == TYPE_CODE_FLT
      ||
      TYPE_CODE (VALUE_TYPE (arg2)) == TYPE_CODE_FLT)
    {
      double v1, v2, v;
      v1 = value_as_double (arg1);
      v2 = value_as_double (arg2);
      switch (op)
	{
	case BINOP_ADD:
	  v = v1 + v2;
	  break;

	case BINOP_SUB:
	  v = v1 - v2;
	  break;

	case BINOP_MUL:
	  v = v1 * v2;
	  break;

	case BINOP_DIV:
	  v = v1 / v2;
	  break;

	default:
	  error ("Integer-only operation on floating point number.");
	}

      val = allocate_value (builtin_type_double);
      *(double *) VALUE_CONTENTS (val) = v;
    }
  else
    {
      long v1, v2, v;
      v1 = value_as_long (arg1);
      v2 = value_as_long (arg2);

      switch (op)
	{
	case BINOP_ADD:
	  v = v1 + v2;
	  break;

	case BINOP_SUB:
	  v = v1 - v2;
	  break;

	case BINOP_MUL:
	  v = v1 * v2;
	  break;

	case BINOP_DIV:
	  v = v1 / v2;
	  break;

	case BINOP_REM:
	  v = v1 % v2;
	  break;

	case BINOP_LSH:
	  v = v1 << v2;
	  break;

	case BINOP_RSH:
	  v = v1 >> v2;
	  break;

	case BINOP_LOGAND:
	  v = v1 & v2;
	  break;

	case BINOP_LOGIOR:
	  v = v1 | v2;
	  break;

	case BINOP_LOGXOR:
	  v = v1 ^ v2;
	  break;

	case BINOP_AND:
	  v = v1 && v2;
	  break;

	case BINOP_OR:
	  v = v1 || v2;
	  break;

	default:
	  error ("Invalid binary operation on numbers.");
	}

      val = allocate_value (builtin_type_long);
      *(long *) VALUE_CONTENTS (val) = v;
    }

  return val;
}

/* Simulate the C operator ! -- return 1 if ARG1 contains zeros.  */

int
value_zerop (arg1)
     value arg1;
{
  register int len;
  register char *p;

  COERCE_ARRAY (arg1);

  len = TYPE_LENGTH (VALUE_TYPE (arg1));
  p = VALUE_CONTENTS (arg1);

  while (--len >= 0)
    {
      if (*p++)
	break;
    }

  return len < 0;
}

/* Simulate the C operator == by returning a 1
   iff ARG1 and ARG2 have equal contents.  */

int
value_equal (arg1, arg2)
     register value arg1, arg2;

{
  register int len;
  register char *p1, *p2;
  enum type_code code1;
  enum type_code code2;

  COERCE_ARRAY (arg1);
  COERCE_ARRAY (arg2);

  code1 = TYPE_CODE (VALUE_TYPE (arg1));
  code2 = TYPE_CODE (VALUE_TYPE (arg2));

  if (code1 == TYPE_CODE_INT && code2 == TYPE_CODE_INT)
    return value_as_long (arg1) == value_as_long (arg2);
  else if ((code1 == TYPE_CODE_FLT || code1 == TYPE_CODE_INT)
	   && (code2 == TYPE_CODE_FLT || code2 == TYPE_CODE_INT))
    return value_as_double (arg1) == value_as_double (arg2);
  else if ((code1 == TYPE_CODE_PTR && code2 == TYPE_CODE_INT)
	   || (code2 == TYPE_CODE_PTR && code1 == TYPE_CODE_INT))
    return value_as_long (arg1) == value_as_long (arg2);
  else if (code1 == code2
	   && ((len = TYPE_LENGTH (VALUE_TYPE (arg1)))
	       == TYPE_LENGTH (VALUE_TYPE (arg2))))
    {
      p1 = VALUE_CONTENTS (arg1);
      p2 = VALUE_CONTENTS (arg2);
      while (--len >= 0)
	{
	  if (*p1++ != *p2++)
	    break;
	}
      return len < 0;
    }
  else
    error ("Invalid type combination in equality test.");
}

/* Simulate the C operator < by returning 1
   iff ARG1's contents are less than ARG2's.  */

int
value_less (arg1, arg2)
     register value arg1, arg2;
{
  register enum type_code code1;
  register enum type_code code2;

  COERCE_ARRAY (arg1);
  COERCE_ARRAY (arg2);

  code1 = TYPE_CODE (VALUE_TYPE (arg1));
  code2 = TYPE_CODE (VALUE_TYPE (arg2));

  if (code1 == TYPE_CODE_INT && code2 == TYPE_CODE_INT)
    return value_as_long (arg1) < value_as_long (arg2);
  else if ((code1 == TYPE_CODE_FLT || code1 == TYPE_CODE_INT)
	   && (code2 == TYPE_CODE_FLT || code2 == TYPE_CODE_INT))
    return value_as_double (arg1) < value_as_double (arg2);
  else if ((code1 == TYPE_CODE_PTR || code1 == TYPE_CODE_INT)
	   && (code2 == TYPE_CODE_PTR || code2 == TYPE_CODE_INT))
    return value_as_long (arg1) < value_as_long (arg2);
  else
    error ("Invalid type combination in ordering comparison.");
}

/* The unary operators - and ~.  Both free the argument ARG1.  */

value
value_neg (arg1)
     register value arg1;
{
  register struct type *type;

  COERCE_ENUM (arg1);

  type = VALUE_TYPE (arg1);

  if (TYPE_CODE (type) == TYPE_CODE_FLT)
    return value_from_double (type, - value_as_double (arg1));
  else if (TYPE_CODE (type) == TYPE_CODE_INT)
    return value_from_long (type, - value_as_long (arg1));
  else
    error ("Argument to negate operation not a number.");
}

value
value_lognot (arg1)
     register value arg1;
{
  COERCE_ENUM (arg1);

  if (TYPE_CODE (VALUE_TYPE (arg1)) != TYPE_CODE_INT)
    error ("Argument to complement operation not an integer.");

  return value_from_long (VALUE_TYPE (arg1), ~ value_as_long (arg1));
}

static
initialize ()
{
}

END_FILE
