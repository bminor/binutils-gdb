/* Support for printing Chill types for GDB, the GNU debugger.
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
#include "bfd.h"		/* Binary File Description */
#include "symtab.h"
#include "gdbtypes.h"
#include "expression.h"
#include "value.h"
#include "gdbcore.h"
#include "target.h"
#include "command.h"
#include "gdbcmd.h"
#include "language.h"
#include "demangle.h"
#include "ch-lang.h"

#include <string.h>
#include <errno.h>

void
chill_print_type_base PARAMS ((struct type *, FILE *, int, int));

void
chill_print_type (type, varstring, stream, show, level)
     struct type *type;
     char *varstring;
     FILE *stream;
     int show;
     int level;
{
  struct type *index_type;
  struct type *range_type;
  LONGEST low_bound;
  LONGEST high_bound;

  if (varstring != NULL && *varstring != '\0')
    {
      fputs_filtered (varstring, stream);
      fputs_filtered (" ", stream);
    }
  switch (TYPE_CODE (type))
    {
      case TYPE_CODE_ARRAY:
	range_type = TYPE_FIELD_TYPE (type, 0);
	index_type = TYPE_TARGET_TYPE (range_type);
	low_bound = TYPE_FIELD_BITPOS (range_type, 0);
	high_bound = TYPE_FIELD_BITPOS (range_type, 1);
        fputs_filtered ("array (", stream);
	print_type_scalar (index_type, low_bound, stream);
	fputs_filtered (":", stream);
	print_type_scalar (index_type, high_bound, stream);
	fputs_filtered (") ", stream);
	chill_print_type (TYPE_TARGET_TYPE (type), "", stream, show, level);
	break;
      default:
        chill_print_type_base (type, stream, show, level);
	break;
    }
}

/* Print the name of the type (or the ultimate pointer target,
   function value or array element).

   SHOW nonzero means don't print this type as just its name;
   show its real definition even if it has a name.
   SHOW zero means print just typename or tag if there is one
   SHOW negative means abbreviate structure elements.
   SHOW is decremented for printing of structure elements.

   LEVEL is the depth to indent by.
   We increase it for some recursive calls.  */

void
chill_print_type_base (type, stream, show, level)
     struct type *type;
     FILE *stream;
     int show;
     int level;
{
  QUIT;

  wrap_here ("    ");
  if (type == NULL)
    {
      fputs_filtered ("<type unknown>", stream);
      return;
    }

  /* When SHOW is zero or less, and there is a valid type name, then always
     just print the type name directly from the type. */

  if ((show <= 0) && (TYPE_NAME (type) != NULL))
    {
      fputs_filtered (TYPE_NAME (type), stream);
      return;
    }

  switch (TYPE_CODE (type))
    {
      case TYPE_CODE_ARRAY:
      case TYPE_CODE_PTR:
      case TYPE_CODE_MEMBER:
      case TYPE_CODE_REF:
      case TYPE_CODE_FUNC:
        chill_print_type_base (TYPE_TARGET_TYPE (type), stream, show, level);
	break;

      case TYPE_CODE_VOID:
      case TYPE_CODE_UNDEF:
      case TYPE_CODE_ERROR:
      case TYPE_CODE_RANGE:
      case TYPE_CODE_ENUM:
      case TYPE_CODE_UNION:
      case TYPE_CODE_STRUCT:
      case TYPE_CODE_METHOD:
	error ("missing language support in chill_print_type_base");
	break;

      default:

	/* Handle types not explicitly handled by the other cases,
	   such as fundamental types.  For these, just print whatever
	   the type name is, as recorded in the type itself.  If there
	   is no type name, then complain. */

	if (TYPE_NAME (type) != NULL)
	  {
	    fputs_filtered (TYPE_NAME (type), stream);
	  }
	else
	  {
	    error ("Unrecognized type code (%d) in symbol table.",
		   TYPE_CODE (type));
	  }
	break;
      }
}
