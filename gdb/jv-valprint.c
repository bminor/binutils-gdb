/* Support for printing Java values for GDB, the GNU debugger.
   Copyright 1997 Free Software Foundation, Inc.

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
#include "value.h"
#include "demangle.h"
#include "valprint.h"
#include "language.h"
#include "jv-lang.h"
#include "c-lang.h"

int
java_value_print (val, stream, format, pretty)
     value_ptr val;
     GDB_FILE *stream;
     int format;
     enum val_prettyprint pretty;
{
  struct type *type = VALUE_TYPE (val);
  CORE_ADDR address = VALUE_ADDRESS (val) + VALUE_OFFSET (val);
  if (is_object_type (type))
    {
      CORE_ADDR obj_addr = unpack_pointer (type, VALUE_CONTENTS (val));
      if (obj_addr != 0)
	{
	  value_ptr obj_val
	    = value_at (TYPE_TARGET_TYPE (type), obj_addr, NULL);
	  type = type_from_class (java_class_from_object (obj_val));
	  type = lookup_pointer_type (type);
	}
    }
  if (TYPE_CODE (type) == TYPE_CODE_PTR && ! value_logical_not (val))
    {
      type_print (TYPE_TARGET_TYPE (type), "", stream, -1);
    }

  if (TYPE_CODE (type) == TYPE_CODE_STRUCT && TYPE_TAG_NAME (type) != NULL
      && TYPE_TAG_NAME (type)[0] == '[')
    {
      value_ptr len = value_at (java_int_type, address + JAVA_OBJECT_SIZE, 0);
      long length = value_as_long (len);
      fprintf_filtered (stream, "{");
      fprintf_filtered (stream, "length = %ld", length);
      fprintf_filtered (stream, "}");
      return 0;
    }

  return (val_print (type, VALUE_CONTENTS (val), address,
		     stream, format, 1, 0, pretty));
}

int
java_val_print (type, valaddr, address, stream, format, deref_ref, recurse,
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
  register unsigned int i = 0;		/* Number of characters printed */
  struct type *elttype;
  CORE_ADDR addr;

  CHECK_TYPEDEF (type);
  switch (TYPE_CODE (type))
    {
    case TYPE_CODE_PTR:
      if (format && format != 's')
	{
	  print_scalar_formatted (valaddr, type, format, 0, stream);
	  break;
	}
#if 0
      if (vtblprint && cp_is_vtbl_ptr_type(type))
	{
          /* Print the unmangled name if desired.  */
	  /* Print vtable entry - we only get here if we ARE using
	     -fvtable_thunks.  (Otherwise, look under TYPE_CODE_STRUCT.) */
	  print_address_demangle(extract_address (valaddr, TYPE_LENGTH (type)),
				 stream, demangle);
	  break;
	}
#endif
      addr = unpack_pointer (type, valaddr);
      if (addr == 0)
	{
	  fputs_filtered ("null", stream);
	  return i;
	}
      elttype = check_typedef (TYPE_TARGET_TYPE (type));
	{
	print_unpacked_pointer:
          elttype = check_typedef (TYPE_TARGET_TYPE (type));

	  if (TYPE_CODE (elttype) == TYPE_CODE_FUNC)
	    {
	      /* Try to print what function it points to.  */
	      print_address_demangle (addr, stream, demangle);
	      /* Return value is irrelevant except for string pointers.  */
	      return (0);
	    }

	  if (addressprint && format != 's')
	    {
	      fputs_filtered ("@", stream);
	      print_longest (stream, 'x', 0, (ULONGEST) addr);
	    }
	  return i;
	}
      break;
    default:
      return c_val_print (type, valaddr, address, stream, format,
			  deref_ref, recurse, pretty);
    }
}
