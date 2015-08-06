/* Convert types from GDB to GCC

   Copyright (C) 2014-2015 Free Software Foundation, Inc.

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
#include "gdbtypes.h"
#include "compile-internal.h"
#include "gdb_assert.h"
#include "symtab.h"
#include "source.h"

/* An object that maps a gdb type to a gcc type.  */

struct type_map_instance
{
  /* The gdb type.  */

  struct type *type;

  /* The corresponding gcc type handle.  */

  gcc_type gcc_type;
};

/* Hash a type_map_instance.  */

static hashval_t
hash_type_map_instance (const void *p)
{
  const struct type_map_instance *inst = p;

  return htab_hash_pointer (inst->type);
}

/* Check two type_map_instance objects for equality.  */

static int
eq_type_map_instance (const void *a, const void *b)
{
  const struct type_map_instance *insta = a;
  const struct type_map_instance *instb = b;

  return insta->type == instb->type;
}



/* Insert an entry into the type map associated with CONTEXT that maps
   from the gdb type TYPE to the gcc type GCC_TYPE.  It is ok for a
   given type to be inserted more than once, provided that the exact
   same association is made each time.  This simplifies how type
   caching works elsewhere in this file -- see how struct type caching
   is handled.  */

static void
insert_type (struct compile_cplus_instance *context, struct type *type,
	     gcc_type gcc_type)
{
  struct type_map_instance inst, *add;
  void **slot;

  inst.type = type;
  inst.gcc_type = gcc_type;
  slot = htab_find_slot (context->type_map, &inst, INSERT);

  add = *slot;
  /* The type might have already been inserted in order to handle
     recursive types.  */
  gdb_assert (add == NULL || add->gcc_type == gcc_type);

  if (add == NULL)
    {
      add = XNEW (struct type_map_instance);
      *add = inst;
      *slot = add;
    }
}

/* Convert a pointer type to its gcc representation.  */

static gcc_type
convert_pointer (struct compile_cplus_instance *context, struct type *type)
{
  gcc_type target = convert_cplus_type (context, TYPE_TARGET_TYPE (type));

  return CP_CTX (context)->cp_ops->build_pointer_type (CP_CTX (context),
						      target);
}

/* Convert an array type to its gcc representation.  */

static gcc_type
convert_array (struct compile_cplus_instance *context, struct type *type)
{
  gcc_type element_type;
  struct type *range = TYPE_INDEX_TYPE (type);

  element_type = convert_cplus_type (context, TYPE_TARGET_TYPE (type));

  if (TYPE_LOW_BOUND_KIND (range) != PROP_CONST)
    return CP_CTX (context)->cp_ops->error (CP_CTX (context),
					    _("array type with non-constant"
					      " lower bound is not supported"));
  if (TYPE_LOW_BOUND (range) != 0)
    return CP_CTX (context)->cp_ops->error (CP_CTX (context),
					    _("cannot convert array type with "
					      "non-zero lower bound to C"));

  if (TYPE_HIGH_BOUND_KIND (range) == PROP_LOCEXPR
      || TYPE_HIGH_BOUND_KIND (range) == PROP_LOCLIST)
    {
      gcc_type result;
      char *upper_bound;

      if (TYPE_VECTOR (type))
	return CP_CTX (context)->cp_ops->error (CP_CTX (context),
						_("variably-sized vector type"
						  " is not supported"));

      upper_bound = c_get_range_decl_name (&TYPE_RANGE_DATA (range)->high);
      result = CP_CTX (context)->cp_ops->build_vla_array_type (CP_CTX (context),
							       element_type,
							       upper_bound);
      xfree (upper_bound);
      return result;
    }
  else
    {
      LONGEST low_bound, high_bound, count;

      if (get_array_bounds (type, &low_bound, &high_bound) == 0)
	count = -1;
      else
	{
	  gdb_assert (low_bound == 0); /* Ensured above.  */
	  count = high_bound + 1;
	}

      if (TYPE_VECTOR (type))
	return CP_CTX (context)->cp_ops->build_vector_type (CP_CTX (context),
							    element_type,
							    count);
      return CP_CTX (context)->cp_ops->build_array_type (CP_CTX (context),
							 element_type, count);
    }
}


static
void find_line_and_src_from_type (struct type *type, unsigned short *line,
				  const char **source)
{
  /* pmuldoon: There's got to be a better way of finding the file and
     source location of a type in source other than finding the symbol
     (searching the symbol table for the string name) from the full
     symbol table.  */
  struct symtab *s;
  struct symbol_search *symbols = NULL;
  char *exact;
  const char *name = TYPE_NAME (type);

  /* Room for NAME and ^ and $ and trailing null.  This is needed for
     exact matches to type name.  */
  exact = xcalloc (strlen (name) + 3, sizeof (char));

  /* Exact matches only.  */
  snprintf (exact, strlen (name) + 3, "^%s$", name);
  search_symbols (exact, TYPES_DOMAIN, 0, NULL, &symbols);
  xfree (exact);
  /* pmuldoon: For now we only cope if one symbol for TYPE is
     found. What to do if class = = minimal symbol?  */
  gdb_assert (symbols && symbols->symbol && symbols->next == NULL);

  s = symbol_symtab (symbols->symbol);
  *source = symtab_to_filename_for_display (s);
  *line = symbols->symbol->line;
  free_search_symbols (symbols);
}

/* Convert a struct or union type to its gcc representation.  */

static gcc_type
convert_struct_or_union (struct compile_cplus_instance *context, struct type *type)
{
  int i;
  gcc_type result;

  // FIXME: drop any namespaces and enclosing class names, if any. -lxo
  const char *name = TYPE_NAME (type);
  const char *filename = NULL;
  unsigned short line = 0;

  find_line_and_src_from_type (type, &line, &filename);

  CP_CTX (context)->cp_ops->push_namespace (CP_CTX (context), "");
  // FIXME: push (and, after the call, pop) any other namespaces, if
  // any, and drop the above when defining a nested or local class.
  // drop any namespace and class names from before the symbol name,
  // and any function signatures from after it.  -lxo

  /* First we create the resulting type and enter it into our hash
     table.  This lets recursive types work.  */
  if (TYPE_CODE (type) == TYPE_CODE_STRUCT)
    {
      struct gcc_vbase_array bases;
      int num_base_classes = TYPE_N_BASECLASSES (type);

      if (num_base_classes > 0 )
	{

	  bases.elements = XNEWVEC (gcc_type, num_base_classes);
	  bases.n_elements = num_base_classes;
	  bases.virtualp = xcalloc (num_base_classes, sizeof (char));
	  for (i = 0; i < num_base_classes; i++)
	    {
	      /* pmuldoon: Not sure what to populate in this base
		 class array.  */
	      struct type *base_type = TYPE_BASECLASS (type, i);
	      const char *base_filename = NULL;
	      unsigned short base_line = 0;

	      find_line_and_src_from_type (base_type, &base_line,
					   &base_filename);
	      if (BASETYPE_VIA_VIRTUAL (type, i))
		bases.virtualp[i] = '1';
	      bases.elements[i] = convert_cplus_type (context, base_type);
	    }
	}

      // FIXME: build base classes array.  -lxo
      result = CP_CTX (context)->cp_ops->start_new_class_type
	(CP_CTX (context), name, &bases, filename, line);
    }
  else
    {
      gdb_assert (TYPE_CODE (type) == TYPE_CODE_UNION);
      result = CP_CTX (context)->cp_ops->start_new_union_type
	(CP_CTX (context), name, filename, line);
    }
  insert_type (context, type, result);

  for (i = 0; i < TYPE_NFIELDS (type); ++i)
    {
      gcc_type field_type;
      unsigned long bitsize = TYPE_FIELD_BITSIZE (type, i);

      field_type = convert_cplus_type (context, TYPE_FIELD_TYPE (type, i));
      if (bitsize == 0)
	bitsize = 8 * TYPE_LENGTH (TYPE_FIELD_TYPE (type, i));

      if (field_is_static (&TYPE_FIELD (type, i)))
	CP_CTX (context)->cp_ops->new_decl (CP_CTX (context),
					    TYPE_FIELD_NAME (type, i),
					    GCC_CP_SYMBOL_VARIABLE,
					    field_type, NULL,
					    // FIXME: compute the
					    // static var address.
					    // -lxo
					    0,
					    // FIXME: do we have
					    // location info for
					    // static data members?
					    // -lxo
					    filename, line);
      else
	CP_CTX (context)->cp_ops->new_field (CP_CTX (context), result,
					     TYPE_FIELD_NAME (type, i),
					     field_type,
					     bitsize,
					     TYPE_FIELD_BITPOS (type, i));
    }

  // FIXME: define member functions, typedefs and classes.  -lxo

  CP_CTX (context)->cp_ops->finish_record_or_union (CP_CTX (context), result,
						    TYPE_LENGTH (type));
  CP_CTX (context)->cp_ops->pop_namespace (CP_CTX (context));
  return result;
}

/* Convert an enum type to its gcc representation.  */

static gcc_type
convert_enum (struct compile_cplus_instance *context, struct type *type)
{
  gcc_type int_type, result;
  int i;
  struct gcc_cp_context *ctx = CP_CTX (context);

  // FIXME: drop any namespaces and enclosing class names, if any. -lxo
  const char *name = type->main_type->name;

  // FIXME: how do we get these? -lxo
  _Bool scoped_enum_p = FALSE;
  const char *filename = NULL;
  unsigned short line = 0;

  int_type = ctx->cp_ops->int_type (ctx,
				   TYPE_UNSIGNED (type),
				   TYPE_LENGTH (type));

  CP_CTX (context)->cp_ops->push_namespace (CP_CTX (context), "");
  // FIXME: push (and, after the call, pop) any other namespaces, if
  // any, and drop the above when defining a nested or local class.
  // drop any namespace and class names from before the symbol name,
  // and any function signatures from after it.  -lxo

  result = ctx->cp_ops->start_new_enum_type (ctx, name, int_type,
					     scoped_enum_p, filename, line);
  for (i = 0; i < TYPE_NFIELDS (type); ++i)
    {
      ctx->cp_ops->build_add_enum_constant (ctx,
					   result,
					   TYPE_FIELD_NAME (type, i),
					   TYPE_FIELD_ENUMVAL (type, i));
    }

  ctx->cp_ops->finish_enum_type (ctx, result);

  CP_CTX (context)->cp_ops->pop_namespace (CP_CTX (context));

  return result;
}

/* Convert a function type to its gcc representation.  */

static gcc_type
convert_func (struct compile_cplus_instance *context, struct type *type)
{
  int i;
  gcc_type result, return_type;
  struct gcc_type_array array;
  int is_varargs = TYPE_VARARGS (type) || !TYPE_PROTOTYPED (type);

  /* This approach means we can't make self-referential function
     types.  Those are impossible in C, though.  */
  return_type = convert_cplus_type (context, TYPE_TARGET_TYPE (type));

  array.n_elements = TYPE_NFIELDS (type);
  array.elements = XNEWVEC (gcc_type, TYPE_NFIELDS (type));
  for (i = 0; i < TYPE_NFIELDS (type); ++i)
    array.elements[i] = convert_cplus_type (context, TYPE_FIELD_TYPE (type, i));

  result = CP_CTX (context)->cp_ops->build_function_type (CP_CTX (context),
							  return_type,
							  &array, is_varargs);
  xfree (array.elements);

  return result;
}

/* Convert an integer type to its gcc representation.  */

static gcc_type
convert_int (struct compile_cplus_instance *context, struct type *type)
{
  return CP_CTX (context)->cp_ops->int_type (CP_CTX (context),
					     TYPE_UNSIGNED (type),
					     TYPE_LENGTH (type));
}

/* Convert a floating-point type to its gcc representation.  */

static gcc_type
convert_float (struct compile_cplus_instance *context, struct type *type)
{
  return CP_CTX (context)->cp_ops->float_type (CP_CTX (context),
					       TYPE_LENGTH (type));
}

/* Convert the 'void' type to its gcc representation.  */

static gcc_type
convert_void (struct compile_cplus_instance *context, struct type *type)
{
  return CP_CTX (context)->cp_ops->void_type (CP_CTX (context));
}

/* Convert a boolean type to its gcc representation.  */

static gcc_type
convert_bool (struct compile_cplus_instance *context, struct type *type)
{
  return CP_CTX (context)->cp_ops->bool_type (CP_CTX (context));
}

/* Convert a qualified type to its gcc representation.  */

static gcc_type
convert_qualified (struct compile_cplus_instance *context, struct type *type)
{
  struct type *unqual = make_unqualified_type (type);
  gcc_type unqual_converted;
  int quals = 0;

  unqual_converted = convert_cplus_type (context, unqual);

  if (TYPE_CONST (type))
    quals |= GCC_QUALIFIER_CONST;
  if (TYPE_VOLATILE (type))
    quals |= GCC_QUALIFIER_VOLATILE;
  if (TYPE_RESTRICT (type))
    quals |= GCC_QUALIFIER_RESTRICT;

  return CP_CTX (context)->cp_ops->build_qualified_type (CP_CTX (context),
							 unqual_converted,
							 quals);
}

/* Convert a complex type to its gcc representation.  */

static gcc_type
convert_complex (struct compile_cplus_instance *context, struct type *type)
{
  gcc_type base = convert_cplus_type (context, TYPE_TARGET_TYPE (type));

  return CP_CTX (context)->cp_ops->build_complex_type (CP_CTX (context), base);
}

/* A helper function which knows how to convert most types from their
   gdb representation to the corresponding gcc form.  This examines
   the TYPE and dispatches to the appropriate conversion function.  It
   returns the gcc type.  */

static gcc_type
convert_type_cplus_basic (struct compile_cplus_instance *context, struct type *type)
{
  /* If we are converting a qualified type, first convert the
     unqualified type and then apply the qualifiers.  */
  if ((TYPE_INSTANCE_FLAGS (type) & (TYPE_INSTANCE_FLAG_CONST
				     | TYPE_INSTANCE_FLAG_VOLATILE
				     | TYPE_INSTANCE_FLAG_RESTRICT)) != 0)
    return convert_qualified (context, type);

  switch (TYPE_CODE (type))
    {
    case TYPE_CODE_PTR:
      return convert_pointer (context, type);

    case TYPE_CODE_ARRAY:
      return convert_array (context, type);

    case TYPE_CODE_STRUCT:
    case TYPE_CODE_UNION:
      return convert_struct_or_union (context, type);

    case TYPE_CODE_ENUM:
      return convert_enum (context, type);

    case TYPE_CODE_FUNC:
      return convert_func (context, type);

    case TYPE_CODE_INT:
      return convert_int (context, type);

    case TYPE_CODE_FLT:
      return convert_float (context, type);

    case TYPE_CODE_VOID:
      return convert_void (context, type);

    case TYPE_CODE_BOOL:
      return convert_bool (context, type);

    case TYPE_CODE_COMPLEX:
      return convert_complex (context, type);
    }

  return CP_CTX (context)->cp_ops->error (CP_CTX (context),
					  _("cannot convert gdb type "
					    "to gcc type"));
}

/* See compile-internal.h.  */

gcc_type
convert_cplus_type (struct compile_cplus_instance *context, struct type *type)
{
  struct type_map_instance inst, *found;
  gcc_type result;

  /* We don't ever have to deal with typedefs in this code, because
     those are only needed as symbols by the C compiler.  */
  CHECK_TYPEDEF (type);

  inst.type = type;
  found = htab_find (context->type_map, &inst);
  if (found != NULL)
    return found->gcc_type;

  result = convert_type_cplus_basic (context, type);
  insert_type (context, type, result);
  return result;
}



/* Delete the compiler instance C.  */

static void
delete_instance (struct compile_instance *c)
{
  struct compile_cplus_instance *context = (struct compile_cplus_instance *) c;

  context->base.fe->ops->destroy (context->base.fe);
  htab_delete (context->type_map);
  if (context->symbol_err_map != NULL)
    htab_delete (context->symbol_err_map);
  xfree (context);
}

/* See compile-internal.h.  */

struct compile_instance *
new_cplus_compile_instance (struct gcc_cp_context *fe)
{
  struct compile_cplus_instance *result = XCNEW (struct compile_cplus_instance);

  result->base.fe = &fe->base;
  result->base.destroy = delete_instance;
  result->base.gcc_target_options = ("-std=gnu++11"
				     " -fno-access-control"
				     /* Otherwise the .o file may need
					"_Unwind_Resume" and
					"__gcc_personality_v0".  */
				     " -fno-exceptions");

  result->type_map = htab_create_alloc (10, hash_type_map_instance,
					eq_type_map_instance,
					xfree, xcalloc, xfree);

  fe->cp_ops->set_callbacks (fe, gcc_cplus_convert_symbol,
			     gcc_cplus_symbol_address, result);

  return &result->base;
}
