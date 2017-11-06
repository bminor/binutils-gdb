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

