/* Support for printing Chill values for GDB, the GNU debugger.
   Copyright 1986, 1988, 1989, 1991 Free Software Foundation, Inc.

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
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "defs.h"
#include "obstack.h"
#include "symtab.h"
#include "gdbtypes.h"
#include "valprint.h"
#include "expression.h"
#include "language.h"


/* Print data of type TYPE located at VALADDR (within GDB), which came from
   the inferior at address ADDRESS, onto stdio stream STREAM according to
   FORMAT (a letter or 0 for natural format).  The data at VALADDR is in
   target byte order.

   If the data are a string pointer, returns the number of string characters
   printed.

   If DEREF_REF is nonzero, then dereference references, otherwise just print
   them like pointers.

   The PRETTY parameter controls prettyprinting.  */

int
chill_val_print (type, valaddr, address, stream, format, deref_ref, recurse,
		 pretty)
     struct type *type;
     char *valaddr;
     CORE_ADDR address;
     FILE *stream;
     int format;
     int deref_ref;
     int recurse;
     enum val_prettyprint pretty;
{
  LONGEST val;
  unsigned int i;
  struct type *elttype;
  unsigned eltlen;
  CORE_ADDR addr;

  switch (TYPE_CODE (type))
    {
    case TYPE_CODE_ARRAY:
      if (TYPE_LENGTH (type) > 0 && TYPE_LENGTH (TYPE_TARGET_TYPE (type)) > 0)
	{
	  if (prettyprint_arrays)
	    {
	      print_spaces_filtered (2 + 2 * recurse, stream);
	    }
	  fprintf_filtered (stream, "[");
	  val_print_array_elements (type, valaddr, address, stream, format,
				    deref_ref, recurse, pretty, 0);
	  fprintf_filtered (stream, "]");
	}
      else
	{
	  error ("unimplemented in chill_val_print; unspecified array length");
	}
      break;

    case TYPE_CODE_INT:
      format = format ? format : output_format;
      if (format)
	{
	  print_scalar_formatted (valaddr, type, format, 0, stream);
	}
      else
	{
	  val_print_type_code_int (type, valaddr, stream);
	}
      break;

    case TYPE_CODE_CHAR:
      format = format ? format : output_format;
      if (format)
	{
	  print_scalar_formatted (valaddr, type, format, 0, stream);
	}
      else
	{
	  LA_PRINT_CHAR ((unsigned char) unpack_long (type, valaddr),
			 stream);
	}
      break;

    case TYPE_CODE_FLT:
      if (format)
	{
	  print_scalar_formatted (valaddr, type, format, 0, stream);
	}
      else
	{
	  print_floating (valaddr, type, stream);
	}
      break;

    case TYPE_CODE_BOOL:
      format = format ? format : output_format;
      if (format)
	{
	  print_scalar_formatted (valaddr, type, format, 0, stream);
	}
      else
	{
	  val = unpack_long (builtin_type_chill_bool, valaddr);
	  fprintf_filtered (stream, val ? "TRUE" : "FALSE");
	}
      break;

    case TYPE_CODE_UNDEF:
      /* This happens (without TYPE_FLAG_STUB set) on systems which don't use
	 dbx xrefs (NO_DBX_XREFS in gcc) if a file has a "struct foo *bar"
	 and no complete type for struct foo in that file.  */
      fprintf_filtered (stream, "<incomplete type>");
      break;

    case TYPE_CODE_PTR:
      if (format && format != 's')
	{
	  print_scalar_formatted (valaddr, type, format, 0, stream);
	  break;
	}
      addr = unpack_pointer (type, valaddr);
      elttype = TYPE_TARGET_TYPE (type);
      
      if (TYPE_CODE (elttype) == TYPE_CODE_FUNC)
	{
	  /* Try to print what function it points to.  */
	  print_address_demangle (addr, stream, demangle);
	  /* Return value is irrelevant except for string pointers.  */
	  return (0);
	}
      if (addressprint && format != 's')
	{
	  fprintf_filtered (stream, "0x%x", addr);
	}
      
      /* For a pointer to char or unsigned char, also print the string
	 pointed to, unless pointer is null.  */
      i = 0;		/* Number of characters printed.  */
      if (TYPE_LENGTH (elttype) == 1
	  && TYPE_CODE (elttype) == TYPE_CODE_CHAR
	  && (format == 0 || format == 's')
	  && addr != 0
	  && /* If print_max is UINT_MAX, the alloca below will fail.
		In that case don't try to print the string.  */
	  print_max < UINT_MAX)
	  {
	    i = val_print_string (addr, stream);
	  }
      /* Return number of characters printed, plus one for the
	 terminating null if we have "reached the end".  */
      return (i + (print_max && i != print_max));
      break;

    case TYPE_CODE_STRING:
      if (format && format != 's')
	{
	  print_scalar_formatted (valaddr, type, format, 0, stream);
	  break;
	}
      addr = unpack_pointer (lookup_pointer_type (builtin_type_char), valaddr);
      if (addressprint && format != 's')
	{
	  fprintf_filtered (stream, "0x%x", addr);
	}
      if (addr != 0)
	{
	  i = val_print_string (addr, TYPE_LENGTH (type), stream);
	}
      /* Return number of characters printed, plus one for the terminating
	 null if we have "reached the end".  */
      return (i + (print_max && i != print_max));
      break;

    case TYPE_CODE_MEMBER:
    case TYPE_CODE_REF:
    case TYPE_CODE_UNION:
    case TYPE_CODE_STRUCT:
    case TYPE_CODE_ENUM:
    case TYPE_CODE_FUNC:
    case TYPE_CODE_VOID:
    case TYPE_CODE_ERROR:
    case TYPE_CODE_RANGE:
      error ("Unimplemented chill_val_print support for type %d",
	     TYPE_CODE (type));
      break;

    default:
      error ("Invalid Chill type code %d in symbol table.", TYPE_CODE (type));
    }
  fflush (stream);
  return (0);
}
