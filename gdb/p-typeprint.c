/* Support for printing Pascal types for GDB, the GNU debugger.
   Copyright (C) 2000-2022 Free Software Foundation, Inc.

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

/* This file is derived from p-typeprint.c */

#include "defs.h"
#include "gdbsupport/gdb_obstack.h"
#include "bfd.h"		/* Binary File Description */
#include "symtab.h"
#include "gdbtypes.h"
#include "expression.h"
#include "value.h"
#include "gdbcore.h"
#include "target.h"
#include "language.h"
#include "p-lang.h"
#include "typeprint.h"
#include "gdb-demangle.h"
#include <ctype.h>
#include "cli/cli-style.h"

/* See language.h.  */

void
pascal_language::print_type (struct type *type, const char *varstring,
			     struct ui_file *stream, int show, int level,
			     const struct type_print_options *flags) const
{
  enum type_code code;
  int demangled_args;

  code = type->code ();

  if (show > 0)
    type = check_typedef (type);

  if ((code == TYPE_CODE_FUNC
       || code == TYPE_CODE_METHOD))
    {
      type_print_varspec_prefix (type, stream, show, 0, flags);
    }
  /* first the name */
  fputs_filtered (varstring, stream);

  if ((varstring != NULL && *varstring != '\0')
      && !(code == TYPE_CODE_FUNC
	   || code == TYPE_CODE_METHOD))
    {
      fputs_filtered (" : ", stream);
    }

  if (!(code == TYPE_CODE_FUNC
	|| code == TYPE_CODE_METHOD))
    {
      type_print_varspec_prefix (type, stream, show, 0, flags);
    }

  type_print_base (type, stream, show, level, flags);
  /* For demangled function names, we have the arglist as part of the name,
     so don't print an additional pair of ()'s.  */

  demangled_args = varstring ? strchr (varstring, '(') != NULL : 0;
  type_print_varspec_suffix (type, stream, show, 0, demangled_args,
				    flags);

}

/* See language.h.  */

void
pascal_language::print_typedef (struct type *type, struct symbol *new_symbol,
				struct ui_file *stream) const
{
  type = check_typedef (type);
  fprintf_filtered (stream, "type ");
  fprintf_filtered (stream, "%s = ", new_symbol->print_name ());
  type_print (type, "", stream, 0);
  fprintf_filtered (stream, ";");
}

/* See p-lang.h.  */

void
pascal_language::type_print_derivation_info (struct ui_file *stream,
					     struct type *type) const
{
  const char *name;
  int i;

  for (i = 0; i < TYPE_N_BASECLASSES (type); i++)
    {
      fputs_filtered (i == 0 ? ": " : ", ", stream);
      fprintf_filtered (stream, "%s%s ",
			BASETYPE_VIA_PUBLIC (type, i) ? "public" : "private",
			BASETYPE_VIA_VIRTUAL (type, i) ? " virtual" : "");
      name = TYPE_BASECLASS (type, i)->name ();
      fprintf_filtered (stream, "%s", name ? name : "(null)");
    }
  if (i > 0)
    {
      fputs_filtered (" ", stream);
    }
}

/* See p-lang.h.  */

void
pascal_language::type_print_method_args (const char *physname,
					 const char *methodname,
					 struct ui_file *stream) const
{
  int is_constructor = (startswith (physname, "__ct__"));
  int is_destructor = (startswith (physname, "__dt__"));

  if (is_constructor || is_destructor)
    {
      physname += 6;
    }

  fputs_filtered (methodname, stream);

  if (physname && (*physname != 0))
    {
      fputs_filtered (" (", stream);
      /* We must demangle this.  */
      while (isdigit (physname[0]))
	{
	  int len = 0;
	  int i, j;
	  char *argname;

	  while (isdigit (physname[len]))
	    {
	      len++;
	    }
	  i = strtol (physname, &argname, 0);
	  physname += len;

	  for (j = 0; j < i; ++j)
	    fputc_filtered (physname[j], stream);

	  physname += i;
	  if (physname[0] != 0)
	    {
	      fputs_filtered (", ", stream);
	    }
	}
      fputs_filtered (")", stream);
    }
}

/* See p-lang.h.  */

void
pascal_language::type_print_varspec_prefix (struct type *type,
					    struct ui_file *stream,
					    int show, int passed_a_ptr,
					    const struct type_print_options *flags) const
{
  if (type == 0)
    return;

  if (type->name () && show <= 0)
    return;

  QUIT;

  switch (type->code ())
    {
    case TYPE_CODE_PTR:
      fprintf_filtered (stream, "^");
      type_print_varspec_prefix (TYPE_TARGET_TYPE (type), stream, 0, 1,
					flags);
      break;			/* Pointer should be handled normally
				   in pascal.  */

    case TYPE_CODE_METHOD:
      if (passed_a_ptr)
	fprintf_filtered (stream, "(");
      if (TYPE_TARGET_TYPE (type) != NULL
	  && TYPE_TARGET_TYPE (type)->code () != TYPE_CODE_VOID)
	{
	  fprintf_filtered (stream, "function  ");
	}
      else
	{
	  fprintf_filtered (stream, "procedure ");
	}

      if (passed_a_ptr)
	{
	  fprintf_filtered (stream, " ");
	  type_print_base (TYPE_SELF_TYPE (type),
				  stream, 0, passed_a_ptr, flags);
	  fprintf_filtered (stream, "::");
	}
      break;

    case TYPE_CODE_REF:
      type_print_varspec_prefix (TYPE_TARGET_TYPE (type), stream, 0, 1,
				 flags);
      fprintf_filtered (stream, "&");
      break;

    case TYPE_CODE_FUNC:
      if (passed_a_ptr)
	fprintf_filtered (stream, "(");

      if (TYPE_TARGET_TYPE (type) != NULL
	  && TYPE_TARGET_TYPE (type)->code () != TYPE_CODE_VOID)
	{
	  fprintf_filtered (stream, "function  ");
	}
      else
	{
	  fprintf_filtered (stream, "procedure ");
	}

      break;

    case TYPE_CODE_ARRAY:
      if (passed_a_ptr)
	fprintf_filtered (stream, "(");
      fprintf_filtered (stream, "array ");
      if (TYPE_LENGTH (TYPE_TARGET_TYPE (type)) > 0
	  && type->bounds ()->high.kind () != PROP_UNDEFINED)
	fprintf_filtered (stream, "[%s..%s] ",
			  plongest (type->bounds ()->low.const_val ()),
			  plongest (type->bounds ()->high.const_val ()));
      fprintf_filtered (stream, "of ");
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
    case TYPE_CODE_COMPLEX:
    case TYPE_CODE_TYPEDEF:
    case TYPE_CODE_FIXED_POINT:
      /* These types need no prefix.  They are listed here so that
	 gcc -Wall will reveal any types that haven't been handled.  */
      break;
    default:
      gdb_assert_not_reached ("unexpected type");
      break;
    }
}

/* See p-lang.h.  */

void
pascal_language::print_func_args (struct type *type, struct ui_file *stream,
				  const struct type_print_options *flags) const
{
  int i, len = type->num_fields ();

  if (len)
    {
      fprintf_filtered (stream, "(");
    }
  for (i = 0; i < len; i++)
    {
      if (i > 0)
	{
	  fputs_filtered (", ", stream);
	  wrap_here ("    ");
	}
      /*  Can we find if it is a var parameter ??
	  if ( TYPE_FIELD(type, i) == )
	  {
	    fprintf_filtered (stream, "var ");
	  } */
      print_type (type->field (i).type (), ""	/* TYPE_FIELD_NAME
						   seems invalid!  */
			 ,stream, -1, 0, flags);
    }
  if (len)
    {
      fprintf_filtered (stream, ")");
    }
}

/* See p-lang.h.  */

void
pascal_language::type_print_func_varspec_suffix  (struct type *type,
						  struct ui_file *stream,
						  int show, int passed_a_ptr,
						  int demangled_args,
						  const struct type_print_options *flags) const
{
  if (TYPE_TARGET_TYPE (type) == NULL
      || TYPE_TARGET_TYPE (type)->code () != TYPE_CODE_VOID)
    {
      fprintf_filtered (stream, " : ");
      type_print_varspec_prefix (TYPE_TARGET_TYPE (type),
					stream, 0, 0, flags);

      if (TYPE_TARGET_TYPE (type) == NULL)
	type_print_unknown_return_type (stream);
      else
	type_print_base (TYPE_TARGET_TYPE (type), stream, show, 0,
				flags);

      type_print_varspec_suffix (TYPE_TARGET_TYPE (type), stream, 0,
				 passed_a_ptr, 0, flags);
    }
}

/* See p-lang.h.  */

void
pascal_language::type_print_varspec_suffix (struct type *type,
					    struct ui_file *stream,
					    int show, int passed_a_ptr,
					    int demangled_args,
					    const struct type_print_options *flags) const
{
  if (type == 0)
    return;

  if (type->name () && show <= 0)
    return;

  QUIT;

  switch (type->code ())
    {
    case TYPE_CODE_ARRAY:
      if (passed_a_ptr)
	fprintf_filtered (stream, ")");
      break;

    case TYPE_CODE_METHOD:
      if (passed_a_ptr)
	fprintf_filtered (stream, ")");
      type_print_method_args ("", "", stream);
      type_print_func_varspec_suffix (type, stream, show,
					     passed_a_ptr, 0, flags);
      break;

    case TYPE_CODE_PTR:
    case TYPE_CODE_REF:
      type_print_varspec_suffix (TYPE_TARGET_TYPE (type),
				 stream, 0, 1, 0, flags);
      break;

    case TYPE_CODE_FUNC:
      if (passed_a_ptr)
	fprintf_filtered (stream, ")");
      if (!demangled_args)
	print_func_args (type, stream, flags);
      type_print_func_varspec_suffix (type, stream, show,
					     passed_a_ptr, 0, flags);
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
    case TYPE_CODE_COMPLEX:
    case TYPE_CODE_TYPEDEF:
    case TYPE_CODE_FIXED_POINT:
      /* These types do not need a suffix.  They are listed so that
	 gcc -Wall will report types that may not have been considered.  */
      break;
    default:
      gdb_assert_not_reached ("unexpected type");
      break;
    }
}

/* See p-lang.h.  */

void
pascal_language::type_print_base (struct type *type, struct ui_file *stream, int show,
				  int level, const struct type_print_options *flags) const
{
  int i;
  int len;
  LONGEST lastval;
  enum
    {
      s_none, s_public, s_private, s_protected
    }
  section_type;

  QUIT;
  wrap_here ("    ");
  if (type == NULL)
    {
      fputs_styled ("<type unknown>", metadata_style.style (), stream);
      return;
    }

  /* void pointer */
  if ((type->code () == TYPE_CODE_PTR)
      && (TYPE_TARGET_TYPE (type)->code () == TYPE_CODE_VOID))
    {
      fputs_filtered (type->name () ? type->name () : "pointer",
		      stream);
      return;
    }
  /* When SHOW is zero or less, and there is a valid type name, then always
     just print the type name directly from the type.  */

  if (show <= 0
      && type->name () != NULL)
    {
      fputs_filtered (type->name (), stream);
      return;
    }

  type = check_typedef (type);

  switch (type->code ())
    {
    case TYPE_CODE_TYPEDEF:
    case TYPE_CODE_PTR:
    case TYPE_CODE_REF:
      type_print_base (TYPE_TARGET_TYPE (type), stream, show, level,
		       flags);
      break;

    case TYPE_CODE_ARRAY:
      print_type (TYPE_TARGET_TYPE (type), NULL, stream, 0, 0, flags);
      break;

    case TYPE_CODE_FUNC:
    case TYPE_CODE_METHOD:
      break;
    case TYPE_CODE_STRUCT:
      if (type->name () != NULL)
	{
	  fputs_filtered (type->name (), stream);
	  fputs_filtered (" = ", stream);
	}
      if (HAVE_CPLUS_STRUCT (type))
	{
	  fprintf_filtered (stream, "class ");
	}
      else
	{
	  fprintf_filtered (stream, "record ");
	}
      goto struct_union;

    case TYPE_CODE_UNION:
      if (type->name () != NULL)
	{
	  fputs_filtered (type->name (), stream);
	  fputs_filtered (" = ", stream);
	}
      fprintf_filtered (stream, "case <?> of ");

    struct_union:
      wrap_here ("    ");
      if (show < 0)
	{
	  /* If we just printed a tag name, no need to print anything else.  */
	  if (type->name () == NULL)
	    fprintf_filtered (stream, "{...}");
	}
      else if (show > 0 || type->name () == NULL)
	{
	  type_print_derivation_info (stream, type);

	  fprintf_filtered (stream, "\n");
	  if ((type->num_fields () == 0) && (TYPE_NFN_FIELDS (type) == 0))
	    {
	      if (type->is_stub ())
		fprintf_filtered (stream, "%*s<incomplete type>\n",
				  level + 4, "");
	      else
		fprintf_filtered (stream, "%*s<no data fields>\n",
				  level + 4, "");
	    }

	  /* Start off with no specific section type, so we can print
	     one for the first field we find, and use that section type
	     thereafter until we find another type.  */

	  section_type = s_none;

	  /* If there is a base class for this type,
	     do not print the field that it occupies.  */

	  len = type->num_fields ();
	  for (i = TYPE_N_BASECLASSES (type); i < len; i++)
	    {
	      QUIT;
	      /* Don't print out virtual function table.  */
	      if ((startswith (type->field (i).name (), "_vptr"))
		  && is_cplus_marker ((type->field (i).name ())[5]))
		continue;

	      /* If this is a pascal object or class we can print the
		 various section labels.  */

	      if (HAVE_CPLUS_STRUCT (type))
		{
		  if (TYPE_FIELD_PROTECTED (type, i))
		    {
		      if (section_type != s_protected)
			{
			  section_type = s_protected;
			  fprintf_filtered (stream, "%*sprotected\n",
					    level + 2, "");
			}
		    }
		  else if (TYPE_FIELD_PRIVATE (type, i))
		    {
		      if (section_type != s_private)
			{
			  section_type = s_private;
			  fprintf_filtered (stream, "%*sprivate\n",
					    level + 2, "");
			}
		    }
		  else
		    {
		      if (section_type != s_public)
			{
			  section_type = s_public;
			  fprintf_filtered (stream, "%*spublic\n",
					    level + 2, "");
			}
		    }
		}

	      print_spaces_filtered (level + 4, stream);
	      if (field_is_static (&type->field (i)))
		fprintf_filtered (stream, "static ");
	      print_type (type->field (i).type (),
				 type->field (i).name (),
				 stream, show - 1, level + 4, flags);
	      if (!field_is_static (&type->field (i))
		  && TYPE_FIELD_PACKED (type, i))
		{
		  /* It is a bitfield.  This code does not attempt
		     to look at the bitpos and reconstruct filler,
		     unnamed fields.  This would lead to misleading
		     results if the compiler does not put out fields
		     for such things (I don't know what it does).  */
		  fprintf_filtered (stream, " : %d",
				    TYPE_FIELD_BITSIZE (type, i));
		}
	      fprintf_filtered (stream, ";\n");
	    }

	  /* If there are both fields and methods, put a space between.  */
	  len = TYPE_NFN_FIELDS (type);
	  if (len && section_type != s_none)
	    fprintf_filtered (stream, "\n");

	  /* Object pascal: print out the methods.  */

	  for (i = 0; i < len; i++)
	    {
	      struct fn_field *f = TYPE_FN_FIELDLIST1 (type, i);
	      int j, len2 = TYPE_FN_FIELDLIST_LENGTH (type, i);
	      const char *method_name = TYPE_FN_FIELDLIST_NAME (type, i);

	      /* this is GNU C++ specific
		 how can we know constructor/destructor?
		 It might work for GNU pascal.  */
	      for (j = 0; j < len2; j++)
		{
		  const char *physname = TYPE_FN_FIELD_PHYSNAME (f, j);

		  int is_constructor = (startswith (physname, "__ct__"));
		  int is_destructor = (startswith (physname, "__dt__"));

		  QUIT;
		  if (TYPE_FN_FIELD_PROTECTED (f, j))
		    {
		      if (section_type != s_protected)
			{
			  section_type = s_protected;
			  fprintf_filtered (stream, "%*sprotected\n",
					    level + 2, "");
			}
		    }
		  else if (TYPE_FN_FIELD_PRIVATE (f, j))
		    {
		      if (section_type != s_private)
			{
			  section_type = s_private;
			  fprintf_filtered (stream, "%*sprivate\n",
					    level + 2, "");
			}
		    }
		  else
		    {
		      if (section_type != s_public)
			{
			  section_type = s_public;
			  fprintf_filtered (stream, "%*spublic\n",
					    level + 2, "");
			}
		    }

		  print_spaces_filtered (level + 4, stream);
		  if (TYPE_FN_FIELD_STATIC_P (f, j))
		    fprintf_filtered (stream, "static ");
		  if (TYPE_TARGET_TYPE (TYPE_FN_FIELD_TYPE (f, j)) == 0)
		    {
		      /* Keep GDB from crashing here.  */
		      fprintf_filtered (stream, "<undefined type> %s;\n",
					TYPE_FN_FIELD_PHYSNAME (f, j));
		      break;
		    }

		  if (is_constructor)
		    {
		      fprintf_filtered (stream, "constructor ");
		    }
		  else if (is_destructor)
		    {
		      fprintf_filtered (stream, "destructor  ");
		    }
		  else if (TYPE_TARGET_TYPE (TYPE_FN_FIELD_TYPE (f, j)) != 0
			   && TYPE_TARGET_TYPE (TYPE_FN_FIELD_TYPE(f, j))->code () != TYPE_CODE_VOID)
		    {
		      fprintf_filtered (stream, "function  ");
		    }
		  else
		    {
		      fprintf_filtered (stream, "procedure ");
		    }
		  /* This does not work, no idea why !!  */

		  type_print_method_args (physname, method_name, stream);

		  if (TYPE_TARGET_TYPE (TYPE_FN_FIELD_TYPE (f, j)) != 0
		      && TYPE_TARGET_TYPE (TYPE_FN_FIELD_TYPE(f, j))->code () != TYPE_CODE_VOID)
		    {
		      fputs_filtered (" : ", stream);
		      type_print (TYPE_TARGET_TYPE (TYPE_FN_FIELD_TYPE (f, j)),
				  "", stream, -1);
		    }
		  if (TYPE_FN_FIELD_VIRTUAL_P (f, j))
		    fprintf_filtered (stream, "; virtual");

		  fprintf_filtered (stream, ";\n");
		}
	    }
	  fprintf_filtered (stream, "%*send", level, "");
	}
      break;

    case TYPE_CODE_ENUM:
      if (type->name () != NULL)
	{
	  fputs_filtered (type->name (), stream);
	  if (show > 0)
	    fputs_filtered (" ", stream);
	}
      /* enum is just defined by
	 type enume_name = (enum_member1,enum_member2,...)  */
      fprintf_filtered (stream, " = ");
      wrap_here ("    ");
      if (show < 0)
	{
	  /* If we just printed a tag name, no need to print anything else.  */
	  if (type->name () == NULL)
	    fprintf_filtered (stream, "(...)");
	}
      else if (show > 0 || type->name () == NULL)
	{
	  fprintf_filtered (stream, "(");
	  len = type->num_fields ();
	  lastval = 0;
	  for (i = 0; i < len; i++)
	    {
	      QUIT;
	      if (i)
		fprintf_filtered (stream, ", ");
	      wrap_here ("    ");
	      fputs_filtered (type->field (i).name (), stream);
	      if (lastval != type->field (i).loc_enumval ())
		{
		  fprintf_filtered (stream,
				    " := %s",
				    plongest (type->field (i).loc_enumval ()));
		  lastval = type->field (i).loc_enumval ();
		}
	      lastval++;
	    }
	  fprintf_filtered (stream, ")");
	}
      break;

    case TYPE_CODE_VOID:
      fprintf_filtered (stream, "void");
      break;

    case TYPE_CODE_UNDEF:
      fprintf_filtered (stream, "record <unknown>");
      break;

    case TYPE_CODE_ERROR:
      fprintf_filtered (stream, "%s", TYPE_ERROR_NAME (type));
      break;

      /* this probably does not work for enums.  */
    case TYPE_CODE_RANGE:
      {
	struct type *target = TYPE_TARGET_TYPE (type);

	print_type_scalar (target, type->bounds ()->low.const_val (), stream);
	fputs_filtered ("..", stream);
	print_type_scalar (target, type->bounds ()->high.const_val (), stream);
      }
      break;

    case TYPE_CODE_SET:
      fputs_filtered ("set of ", stream);
      print_type (type->index_type (), "", stream,
			 show - 1, level, flags);
      break;

    case TYPE_CODE_STRING:
      fputs_filtered ("String", stream);
      break;

    default:
      /* Handle types not explicitly handled by the other cases,
	 such as fundamental types.  For these, just print whatever
	 the type name is, as recorded in the type itself.  If there
	 is no type name, then complain.  */
      if (type->name () != NULL)
	{
	  fputs_filtered (type->name (), stream);
	}
      else
	{
	  /* At least for dump_symtab, it is important that this not be
	     an error ().  */
	  fprintf_styled (stream, metadata_style.style (),
			  "<invalid unnamed pascal type code %d>",
			  type->code ());
	}
      break;
    }
}
