/* Floating point routines for GDB, the GNU debugger.

   Copyright (C) 2017 Free Software Foundation, Inc.

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
#include "dfp.h"
#include "doublest.h"
#include "gdbtypes.h"
#include "floatformat.h"
#include "target-float.h"


/* Helper routines operating on binary floating-point data.  */

#include <math.h>

/* Convert the byte-stream ADDR, interpreted as floating-point format FMT,
   to an integer value (rounding towards zero).  */
static LONGEST
floatformat_to_longest (const struct floatformat *fmt, const gdb_byte *addr)
{
  DOUBLEST d;
  floatformat_to_doublest (fmt, addr, &d);
  return (LONGEST) d;
}

/* Convert signed integer VAL to a target floating-number of format FMT
   and store it as byte-stream ADDR.  */
static void
floatformat_from_longest (const struct floatformat *fmt, gdb_byte *addr,
			  LONGEST val)
{
  DOUBLEST d = (DOUBLEST) val;
  floatformat_from_doublest (fmt, &d, addr);
}

/* Convert unsigned integer VAL to a target floating-number of format FMT
   and store it as byte-stream ADDR.  */
static void
floatformat_from_ulongest (const struct floatformat *fmt, gdb_byte *addr,
			   ULONGEST val)
{
  DOUBLEST d = (DOUBLEST) val;
  floatformat_from_doublest (fmt, &d, addr);
}

/* Convert a floating-point number of format FROM_FMT from the target
   byte-stream FROM to a floating-point number of format TO_FMT, and
   store it to the target byte-stream TO.  */
static void
floatformat_convert (const gdb_byte *from, const struct floatformat *from_fmt,
		     gdb_byte *to, const struct floatformat *to_fmt)
{
  if (from_fmt == to_fmt)
    {
      /* The floating-point formats match, so we simply copy the data.  */
      memcpy (to, from, floatformat_totalsize_bytes (to_fmt));
    }
  else
    {
      /* The floating-point formats don't match.  The best we can do
	 (apart from simulating the target FPU) is converting to the
	 widest floating-point type supported by the host, and then
	 again to the desired type.  */
      DOUBLEST d;

      floatformat_to_doublest (from_fmt, from, &d);
      floatformat_from_doublest (to_fmt, &d, to);
    }
}

/* Perform the binary operation indicated by OPCODE, using as operands the
   target byte streams X and Y, interpreted as floating-point numbers of
   formats FMT_X and FMT_Y, respectively.  Convert the result to format
   FMT_RES and store it into the byte-stream RES.  */
static void
floatformat_binop (enum exp_opcode op,
		   const struct floatformat *fmt_x, const gdb_byte *x,
		   const struct floatformat *fmt_y, const gdb_byte *y,
		   const struct floatformat *fmt_result, gdb_byte *result)
{
  DOUBLEST v1, v2, v = 0;

  floatformat_to_doublest (fmt_x, x, &v1);
  floatformat_to_doublest (fmt_y, y, &v2);

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

      case BINOP_EXP:
	errno = 0;
	v = pow (v1, v2);
	if (errno)
	  error (_("Cannot perform exponentiation: %s"),
		 safe_strerror (errno));
	break;

      case BINOP_MIN:
	v = v1 < v2 ? v1 : v2;
	break;

      case BINOP_MAX:
	v = v1 > v2 ? v1 : v2;
	break;

      default:
	error (_("Integer-only operation on floating point number."));
	break;
    }

  floatformat_from_doublest (fmt_result, &v, result);
}

/* Compare the two target byte streams X and Y, interpreted as floating-point
   numbers of formats FMT_X and FMT_Y, respectively.  Return zero if X and Y
   are equal, -1 if X is less than Y, and 1 otherwise.  */
static int
floatformat_compare (const struct floatformat *fmt_x, const gdb_byte *x,
		     const struct floatformat *fmt_y, const gdb_byte *y)
{
  DOUBLEST v1, v2;

  floatformat_to_doublest (fmt_x, x, &v1);
  floatformat_to_doublest (fmt_y, y, &v2);

  if (v1 == v2)
    return 0;
  if (v1 < v2)
    return -1;
  return 1;
}


/* Typed floating-point routines.  These routines operate on floating-point
   values in target format, represented by a byte buffer interpreted as a
   "struct type", which may be either a binary or decimal floating-point
   type (TYPE_CODE_FLT or TYPE_CODE_DECFLOAT).  */

/* Return whether the byte-stream ADDR holds a valid value of
   floating-point type TYPE.  */
bool
target_float_is_valid (const gdb_byte *addr, const struct type *type)
{
  if (TYPE_CODE (type) == TYPE_CODE_FLT)
    return floatformat_is_valid (floatformat_from_type (type), addr);

  if (TYPE_CODE (type) == TYPE_CODE_DECFLOAT)
    return true;

  gdb_assert_not_reached ("unexpected type code");
}

/* Return whether the byte-stream ADDR, interpreted as floating-point
   type TYPE, is numerically equal to zero (of either sign).  */
bool
target_float_is_zero (const gdb_byte *addr, const struct type *type)
{
  if (TYPE_CODE (type) == TYPE_CODE_FLT)
    return (floatformat_classify (floatformat_from_type (type), addr)
	    == float_zero);

  if (TYPE_CODE (type) == TYPE_CODE_DECFLOAT)
    return decimal_is_zero (addr, TYPE_LENGTH (type),
			    gdbarch_byte_order (get_type_arch (type)));

  gdb_assert_not_reached ("unexpected type code");
}

/* Convert the byte-stream ADDR, interpreted as floating-point type TYPE,
   to a string, optionally using the print format FORMAT.  */
std::string
target_float_to_string (const gdb_byte *addr, const struct type *type,
			const char *format)
{
  if (TYPE_CODE (type) == TYPE_CODE_FLT)
    return floatformat_to_string (floatformat_from_type (type), addr, format);

  if (TYPE_CODE (type) == TYPE_CODE_DECFLOAT)
    return decimal_to_string (addr, TYPE_LENGTH (type),
			      gdbarch_byte_order (get_type_arch (type)),
			      format);

  gdb_assert_not_reached ("unexpected type code");
}

/* Parse string STRING into a target floating-number of type TYPE and
   store it as byte-stream ADDR.  Return whether parsing succeeded.  */
bool
target_float_from_string (gdb_byte *addr, const struct type *type,
			  const std::string &string)
{
  /* Ensure possible padding bytes in the target buffer are zeroed out.  */
  memset (addr, 0, TYPE_LENGTH (type));

  if (TYPE_CODE (type) == TYPE_CODE_FLT)
    return floatformat_from_string (floatformat_from_type (type), addr,
				    string);

  if (TYPE_CODE (type) == TYPE_CODE_DECFLOAT)
    return decimal_from_string (addr, TYPE_LENGTH (type),
				gdbarch_byte_order (get_type_arch (type)),
				string);

  gdb_assert_not_reached ("unexpected type code");
}

/* Convert the byte-stream ADDR, interpreted as floating-point type TYPE,
   to an integer value (rounding towards zero).  */
LONGEST
target_float_to_longest (const gdb_byte *addr, const struct type *type)
{
  if (TYPE_CODE (type) == TYPE_CODE_FLT)
    return floatformat_to_longest (floatformat_from_type (type), addr);

  if (TYPE_CODE (type) == TYPE_CODE_DECFLOAT)
    return decimal_to_longest (addr, TYPE_LENGTH (type),
			       gdbarch_byte_order (get_type_arch (type)));

  gdb_assert_not_reached ("unexpected type code");
}

/* Convert signed integer VAL to a target floating-number of type TYPE
   and store it as byte-stream ADDR.  */
void
target_float_from_longest (gdb_byte *addr, const struct type *type,
			   LONGEST val)
{
  /* Ensure possible padding bytes in the target buffer are zeroed out.  */
  memset (addr, 0, TYPE_LENGTH (type));

  if (TYPE_CODE (type) == TYPE_CODE_FLT)
    {
      floatformat_from_longest (floatformat_from_type (type), addr, val);
      return;
    }

  if (TYPE_CODE (type) == TYPE_CODE_DECFLOAT)
    {
      decimal_from_longest (val, addr, TYPE_LENGTH (type),
			    gdbarch_byte_order (get_type_arch (type)));
      return;
    }

  gdb_assert_not_reached ("unexpected type code");
}

/* Convert unsigned integer VAL to a target floating-number of type TYPE
   and store it as byte-stream ADDR.  */
void
target_float_from_ulongest (gdb_byte *addr, const struct type *type,
			    ULONGEST val)
{
  /* Ensure possible padding bytes in the target buffer are zeroed out.  */
  memset (addr, 0, TYPE_LENGTH (type));

  if (TYPE_CODE (type) == TYPE_CODE_FLT)
    {
      floatformat_from_ulongest (floatformat_from_type (type), addr, val);
      return;
    }

  if (TYPE_CODE (type) == TYPE_CODE_DECFLOAT)
    {
      decimal_from_ulongest (val, addr, TYPE_LENGTH (type),
			     gdbarch_byte_order (get_type_arch (type)));
      return;
    }

  gdb_assert_not_reached ("unexpected type code");
}

/* Convert a floating-point number of type FROM_TYPE from the target
   byte-stream FROM to a floating-point number of type TO_TYPE, and
   store it to the target byte-stream TO.  */
void
target_float_convert (const gdb_byte *from, const struct type *from_type,
		      gdb_byte *to, const struct type *to_type)
{
  /* Ensure possible padding bytes in the target buffer are zeroed out.  */
  memset (to, 0, TYPE_LENGTH (to_type));

  /* Use direct conversion routines if we have them.  */

  if (TYPE_CODE (from_type) == TYPE_CODE_FLT
      && TYPE_CODE (to_type) == TYPE_CODE_FLT)
    {
      floatformat_convert (from, floatformat_from_type (from_type),
			   to, floatformat_from_type (to_type));
      return;
    }

  if (TYPE_CODE (from_type) == TYPE_CODE_DECFLOAT
      && TYPE_CODE (to_type) == TYPE_CODE_DECFLOAT)
    {
      decimal_convert (from, TYPE_LENGTH (from_type),
		       gdbarch_byte_order (get_type_arch (from_type)),
		       to, TYPE_LENGTH (to_type),
		       gdbarch_byte_order (get_type_arch (to_type)));
      return;
    }

  /* We cannot directly convert between binary and decimal floating-point
     types, so go via an intermediary string.  */

  if ((TYPE_CODE (from_type) == TYPE_CODE_FLT
       && TYPE_CODE (to_type) == TYPE_CODE_DECFLOAT)
      || (TYPE_CODE (from_type) == TYPE_CODE_DECFLOAT
	  && TYPE_CODE (to_type) == TYPE_CODE_FLT))
    {
      std::string str = target_float_to_string (from, from_type);
      target_float_from_string (to, to_type, str);
      return;
    }

  gdb_assert_not_reached ("unexpected type code");
}

/* Perform the binary operation indicated by OPCODE, using as operands the
   target byte streams X and Y, interpreted as floating-point numbers of
   types TYPE_X and TYPE_Y, respectively.  Convert the result to type
   TYPE_RES and store it into the byte-stream RES.

   The three types must either be all binary floating-point types, or else
   all decimal floating-point types.  Binary and decimal floating-point
   types cannot be mixed within a single operation.  */
void
target_float_binop (enum exp_opcode opcode,
		    const gdb_byte *x, const struct type *type_x,
		    const gdb_byte *y, const struct type *type_y,
		    gdb_byte *res, const struct type *type_res)
{
  /* Ensure possible padding bytes in the target buffer are zeroed out.  */
  memset (res, 0, TYPE_LENGTH (type_res));

  if (TYPE_CODE (type_res) == TYPE_CODE_FLT)
    {
      gdb_assert (TYPE_CODE (type_x) == TYPE_CODE_FLT);
      gdb_assert (TYPE_CODE (type_y) == TYPE_CODE_FLT);
      return floatformat_binop (opcode,
				floatformat_from_type (type_x), x,
				floatformat_from_type (type_y), y,
				floatformat_from_type (type_res), res);
    }

  if (TYPE_CODE (type_res) == TYPE_CODE_DECFLOAT)
    {
      gdb_assert (TYPE_CODE (type_x) == TYPE_CODE_DECFLOAT);
      gdb_assert (TYPE_CODE (type_y) == TYPE_CODE_DECFLOAT);
      return decimal_binop (opcode,
			    x, TYPE_LENGTH (type_x),
			    gdbarch_byte_order (get_type_arch (type_x)),
			    y, TYPE_LENGTH (type_y),
			    gdbarch_byte_order (get_type_arch (type_y)),
			    res, TYPE_LENGTH (type_res),
			    gdbarch_byte_order (get_type_arch (type_res)));
    }

  gdb_assert_not_reached ("unexpected type code");
}

/* Compare the two target byte streams X and Y, interpreted as floating-point
   numbers of types TYPE_X and TYPE_Y, respectively.  Return zero if X and Y
   are equal, -1 if X is less than Y, and 1 otherwise.

   The two types must either both be binary floating-point types, or else
   both be decimal floating-point types.  Binary and decimal floating-point
   types cannot compared directly against each other.  */
int
target_float_compare (const gdb_byte *x, const struct type *type_x,
		      const gdb_byte *y, const struct type *type_y)
{
  if (TYPE_CODE (type_x) == TYPE_CODE_FLT)
    {
      gdb_assert (TYPE_CODE (type_y) == TYPE_CODE_FLT);
      return floatformat_compare (floatformat_from_type (type_x), x,
				  floatformat_from_type (type_y), y);
    }

  if (TYPE_CODE (type_x) == TYPE_CODE_DECFLOAT)
    {
      gdb_assert (TYPE_CODE (type_y) == TYPE_CODE_DECFLOAT);
      return decimal_compare (x, TYPE_LENGTH (type_x),
			      gdbarch_byte_order (get_type_arch (type_x)),
			      y, TYPE_LENGTH (type_y),
			      gdbarch_byte_order (get_type_arch (type_y)));
    }

  gdb_assert_not_reached ("unexpected type code");
}

