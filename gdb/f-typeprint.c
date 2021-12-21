/* Support for printing Fortran types for GDB, the GNU debugger.

   Copyright (C) 1986-2022 Free Software Foundation, Inc.

   Contributed by Motorola.  Adapted from the C version by Farooq Butt
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
#include "gdbsupport/gdb_obstack.h"
#include "bfd.h"
#include "symtab.h"
#include "gdbtypes.h"
#include "expression.h"
#include "value.h"
#include "gdbcore.h"
#include "target.h"
#include "f-lang.h"
#include "typeprint.h"
#include "cli/cli-style.h"

/* See f-lang.h.  */

void
f_language::print_typedef (struct type *type, struct symbol *new_symbol,
			   struct ui_file *stream) const
{
  type = check_typedef (type);
  print_type (type, "", stream, 0, 0, &type_print_raw_options);
}

/* See f-lang.h.  */

void
f_language::print_type (struct type *type, const char *varstring,
			struct ui_file *stream, int show, int level,
			const struct type_print_options *flags) const
{
  enum type_code code;

  f_type_print_base (type, stream, show, level);
  code = type->code ();
  if ((varstring != NULL && *varstring != '\0')
      /* Need a space if going to print stars or brackets; but not if we
	 will print just a type name.  */
      || ((show > 0
	   || type->name () == 0)
	  && (code == TYPE_CODE_FUNC
	      || code == TYPE_CODE_METHOD
	      || code == TYPE_CODE_ARRAY
	      || ((code == TYPE_CODE_PTR
		   || code == TYPE_CODE_REF)
		  && (TYPE_TARGET_TYPE (type)->code () == TYPE_CODE_FUNC
		      || (TYPE_TARGET_TYPE (type)->code ()
			  == TYPE_CODE_METHOD)
		      || (TYPE_TARGET_TYPE (type)->code ()
			  == TYPE_CODE_ARRAY))))))
    fputs_filtered (" ", stream);
  f_type_print_varspec_prefix (type, stream, show, 0);

  if (varstring != NULL)
    {
      int demangled_args;

      fputs_filtered (varstring, stream);

      /* For demangled function names, we have the arglist as part of the name,
	 so don't print an additional pair of ()'s.  */

      demangled_args = (*varstring != '\0'
			&& varstring[strlen (varstring) - 1] == ')');
      f_type_print_varspec_suffix (type, stream, show, 0, demangled_args, 0, false);
   }
}

/* See f-lang.h.  */

void
f_language::f_type_print_varspec_prefix (struct type *type,
					 struct ui_file *stream,
					 int show, int passed_a_ptr) const
{
  if (type == 0)
    return;

  if (type->name () && show <= 0)
    return;

  QUIT;

  switch (type->code ())
    {
    case TYPE_CODE_PTR:
      f_type_print_varspec_prefix (TYPE_TARGET_TYPE (type), stream, 0, 1);
      break;

    case TYPE_CODE_FUNC:
      f_type_print_varspec_prefix (TYPE_TARGET_TYPE (type), stream, 0, 0);
      if (passed_a_ptr)
	fprintf_filtered (stream, "(");
      break;

    case TYPE_CODE_ARRAY:
      f_type_print_varspec_prefix (TYPE_TARGET_TYPE (type), stream, 0, 0);
      break;

    case TYPE_CODE_UNDEF:
    case TYPE_CODE_STRUCT:
    case TYPE_CODE_UNION:
    case TYPE_CODE_ENUM:
    case TYPE_CODE_INT:
    case TYPE_CODE_FLT:
    case TYPE_CODE_VOID:
    case TYPE_CODE_ERROR:
    case TYPE_CODE_CHAR:
    case TYPE_CODE_BOOL:
    case TYPE_CODE_SET:
    case TYPE_CODE_RANGE:
    case TYPE_CODE_STRING:
    case TYPE_CODE_METHOD:
    case TYPE_CODE_REF:
    case TYPE_CODE_COMPLEX:
    case TYPE_CODE_TYPEDEF:
      /* These types need no prefix.  They are listed here so that
	 gcc -Wall will reveal any types that haven't been handled.  */
      break;
    }
}

/* See f-lang.h.  */

void
f_language::f_type_print_varspec_suffix (struct type *type,
					 struct ui_file *stream,
					 int show, int passed_a_ptr,
					 int demangled_args,
					 int arrayprint_recurse_level,
					 bool print_rank_only) const
{
  /* No static variables are permitted as an error call may occur during
     execution of this function.  */

  if (type == 0)
    return;

  if (type->name () && show <= 0)
    return;

  QUIT;

  switch (type->code ())
    {
    case TYPE_CODE_ARRAY:
      arrayprint_recurse_level++;

      if (arrayprint_recurse_level == 1)
	fprintf_filtered (stream, "(");

      if (type_not_associated (type))
	print_rank_only = true;
      else if (type_not_allocated (type))
	print_rank_only = true;
      else if ((TYPE_ASSOCIATED_PROP (type)
		&& PROP_CONST != TYPE_ASSOCIATED_PROP (type)->kind ())
	       || (TYPE_ALLOCATED_PROP (type)
		   && PROP_CONST != TYPE_ALLOCATED_PROP (type)->kind ())
	       || (TYPE_DATA_LOCATION (type)
		   && PROP_CONST != TYPE_DATA_LOCATION (type)->kind ()))
	{
	  /* This case exist when we ptype a typename which has the dynamic
	     properties but cannot be resolved as there is no object.  */
	  print_rank_only = true;
	}

      if (TYPE_TARGET_TYPE (type)->code () == TYPE_CODE_ARRAY)
	f_type_print_varspec_suffix (TYPE_TARGET_TYPE (type), stream, 0,
				     0, 0, arrayprint_recurse_level,
				     print_rank_only);

      if (print_rank_only)
	fprintf_filtered (stream, ":");
      else
	{
	  LONGEST lower_bound = f77_get_lowerbound (type);
	  if (lower_bound != 1)	/* Not the default.  */
	    fprintf_filtered (stream, "%s:", plongest (lower_bound));

	  /* Make sure that, if we have an assumed size array, we
	       print out a warning and print the upperbound as '*'.  */

	  if (type->bounds ()->high.kind () == PROP_UNDEFINED)
	    fprintf_filtered (stream, "*");
	  else
	    {
	      LONGEST upper_bound = f77_get_upperbound (type);

	      fputs_filtered (plongest (upper_bound), stream);
	    }
	}

      if (TYPE_TARGET_TYPE (type)->code () != TYPE_CODE_ARRAY)
	f_type_print_varspec_suffix (TYPE_TARGET_TYPE (type), stream, 0,
				     0, 0, arrayprint_recurse_level,
				     print_rank_only);

      if (arrayprint_recurse_level == 1)
	fprintf_filtered (stream, ")");
      else
	fprintf_filtered (stream, ",");
      arrayprint_recurse_level--;
      break;

    case TYPE_CODE_PTR:
    case TYPE_CODE_REF:
      f_type_print_varspec_suffix (TYPE_TARGET_TYPE (type), stream, 0, 1, 0,
				   arrayprint_recurse_level, false);
      fprintf_filtered (stream, " )");
      break;

    case TYPE_CODE_FUNC:
      {
	int i, nfields = type->num_fields ();

	f_type_print_varspec_suffix (TYPE_TARGET_TYPE (type), stream, 0,
				     passed_a_ptr, 0,
				     arrayprint_recurse_level, false);
	if (passed_a_ptr)
	  fprintf_filtered (stream, ") ");
	fprintf_filtered (stream, "(");
	if (nfields == 0 && type->is_prototyped ())
	  print_type (builtin_f_type (type->arch ())->builtin_void,
		      "", stream, -1, 0, 0);
	else
	  for (i = 0; i < nfields; i++)
	    {
	      if (i > 0)
		{
		  fputs_filtered (", ", stream);
		  wrap_here ("    ");
		}
	      print_type (type->field (i).type (), "", stream, -1, 0, 0);
	    }
	fprintf_filtered (stream, ")");
      }
      break;

    case TYPE_CODE_UNDEF:
    case TYPE_CODE_STRUCT:
    case TYPE_CODE_UNION:
    case TYPE_CODE_ENUM:
    case TYPE_CODE_INT:
    case TYPE_CODE_FLT:
    case TYPE_CODE_VOID:
    case TYPE_CODE_ERROR:
    case TYPE_CODE_CHAR:
    case TYPE_CODE_BOOL:
    case TYPE_CODE_SET:
    case TYPE_CODE_RANGE:
    case TYPE_CODE_STRING:
    case TYPE_CODE_METHOD:
    case TYPE_CODE_COMPLEX:
    case TYPE_CODE_TYPEDEF:
      /* These types do not need a suffix.  They are listed so that
	 gcc -Wall will report types that may not have been considered.  */
      break;
    }
}

/* See f-lang.h.  */

void
f_language::f_type_print_base (struct type *type, struct ui_file *stream,
			       int show, int level) const
{
  int index;

  QUIT;

  wrap_here ("    ");
  if (type == NULL)
    {
      fputs_styled ("<type unknown>", metadata_style.style (), stream);
      return;
    }

  /* When SHOW is zero or less, and there is a valid type name, then always
     just print the type name directly from the type.  */

  if ((show <= 0) && (type->name () != NULL))
    {
      const char *prefix = "";
      if (type->code () == TYPE_CODE_UNION)
	prefix = "Type, C_Union :: ";
      else if (type->code () == TYPE_CODE_STRUCT)
	prefix = "Type ";
      fprintf_filtered (stream, "%*s%s%s", level, "", prefix, type->name ());
      return;
    }

  if (type->code () != TYPE_CODE_TYPEDEF)
    type = check_typedef (type);

  switch (type->code ())
    {
    case TYPE_CODE_TYPEDEF:
      f_type_print_base (TYPE_TARGET_TYPE (type), stream, 0, level);
      break;

    case TYPE_CODE_ARRAY:
      f_type_print_base (TYPE_TARGET_TYPE (type), stream, show, level);
      break;
    case TYPE_CODE_FUNC:
      if (TYPE_TARGET_TYPE (type) == NULL)
	type_print_unknown_return_type (stream);
      else
	f_type_print_base (TYPE_TARGET_TYPE (type), stream, show, level);
      break;

    case TYPE_CODE_PTR:
      fprintf_filtered (stream, "%*sPTR TO -> ( ", level, "");
      f_type_print_base (TYPE_TARGET_TYPE (type), stream, show, 0);
      break;

    case TYPE_CODE_REF:
      fprintf_filtered (stream, "%*sREF TO -> ( ", level, "");
      f_type_print_base (TYPE_TARGET_TYPE (type), stream, show, 0);
      break;

    case TYPE_CODE_VOID:
      {
	struct type *void_type = builtin_f_type (type->arch ())->builtin_void;
	fprintf_filtered (stream, "%*s%s", level, "", void_type->name ());
      }
      break;

    case TYPE_CODE_UNDEF:
      fprintf_filtered (stream, "%*sstruct <unknown>", level, "");
      break;

    case TYPE_CODE_ERROR:
      fprintf_filtered (stream, "%*s%s", level, "", TYPE_ERROR_NAME (type));
      break;

    case TYPE_CODE_RANGE:
      /* This should not occur.  */
      fprintf_filtered (stream, "%*s<range type>", level, "");
      break;

    case TYPE_CODE_CHAR:
    case TYPE_CODE_INT:
      /* There may be some character types that attempt to come
	 through as TYPE_CODE_INT since dbxstclass.h is so
	 C-oriented, we must change these to "character" from "char".  */

      if (strcmp (type->name (), "char") == 0)
	fprintf_filtered (stream, "%*scharacter", level, "");
      else
	goto default_case;
      break;

    case TYPE_CODE_STRING:
      /* Strings may have dynamic upperbounds (lengths) like arrays.  We
	 check specifically for the PROP_CONST case to indicate that the
	 dynamic type has been resolved.  If we arrive here having been
	 asked to print the type of a value with a dynamic type then the
	 bounds will not have been resolved.  */

      if (type->bounds ()->high.kind () == PROP_CONST)
	{
	  LONGEST upper_bound = f77_get_upperbound (type);

	  fprintf_filtered (stream, "character*%s", pulongest (upper_bound));
	}
      else
	fprintf_filtered (stream, "%*scharacter*(*)", level, "");
      break;

    case TYPE_CODE_STRUCT:
    case TYPE_CODE_UNION:
      if (type->code () == TYPE_CODE_UNION)
	fprintf_filtered (stream, "%*sType, C_Union :: ", level, "");
      else
	fprintf_filtered (stream, "%*sType ", level, "");
      fputs_filtered (type->name (), stream);
      /* According to the definition,
	 we only print structure elements in case show > 0.  */
      if (show > 0)
	{
	  fputs_filtered ("\n", stream);
	  for (index = 0; index < type->num_fields (); index++)
	    {
	      f_type_print_base (type->field (index).type (), stream,
				 show - 1, level + 4);
	      fputs_filtered (" :: ", stream);
	      fputs_styled (type->field (index).name (),
			    variable_name_style.style (), stream);
	      f_type_print_varspec_suffix (type->field (index).type (),
					   stream, show - 1, 0, 0, 0, false);
	      fputs_filtered ("\n", stream);
	    }
	  fprintf_filtered (stream, "%*sEnd Type ", level, "");
	  fputs_filtered (type->name (), stream);
	}
      break;

    case TYPE_CODE_MODULE:
      fprintf_filtered (stream, "%*smodule %s", level, "", type->name ());
      break;

    default_case:
    default:
      /* Handle types not explicitly handled by the other cases,
	 such as fundamental types.  For these, just print whatever
	 the type name is, as recorded in the type itself.  If there
	 is no type name, then complain.  */
      if (type->name () != NULL)
	fprintf_filtered (stream, "%*s%s", level, "", type->name ());
      else
	error (_("Invalid type code (%d) in symbol table."), type->code ());
      break;
    }

  if (TYPE_IS_ALLOCATABLE (type))
    fprintf_filtered (stream, ", allocatable");
}
