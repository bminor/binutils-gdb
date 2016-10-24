/* Convert types from GDB to GCC

   Copyright (C) 2014-2016 Free Software Foundation, Inc.

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
#include "compile-c.h"

/* Convert a pointer type to its gcc representation.  */

static gcc_type
convert_pointer (struct compile_c_instance *context, struct type *type)
{
  gcc_type target = context->convert_type (TYPE_TARGET_TYPE (type));

  return context->build_pointer_type (target);
}

/* Convert an array type to its gcc representation.  */

static gcc_type
convert_array (struct compile_c_instance *context, struct type *type)
{
  gcc_type element_type;
  struct type *range = TYPE_INDEX_TYPE (type);

  element_type = context->convert_type (TYPE_TARGET_TYPE (type));

  if (TYPE_LOW_BOUND_KIND (range) != PROP_CONST)
    return context->error (_("array type with non-constant"
			     " lower bound is not supported"));
  if (TYPE_LOW_BOUND (range) != 0)
    return context->error (_("cannot convert array type with "
			     "non-zero lower bound to C"));

  if (TYPE_HIGH_BOUND_KIND (range) == PROP_LOCEXPR
      || TYPE_HIGH_BOUND_KIND (range) == PROP_LOCLIST)
    {
      gcc_type result;
      char *upper_bound;

      if (TYPE_VECTOR (type))
	return context->error (_("variably-sized vector type"
				 " is not supported"));

      upper_bound = c_get_range_decl_name (&TYPE_RANGE_DATA (range)->high);
      result = context->build_vla_array_type (element_type, upper_bound);
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
	return context->build_vector_type (element_type, count);
      return context->build_array_type (element_type, count);
    }
}

/* Convert a struct or union type to its gcc representation.  */

static gcc_type
convert_struct_or_union (struct compile_c_instance *context, struct type *type)
{
  int i;
  gcc_type result;

  /* First we create the resulting type and enter it into our hash
     table.  This lets recursive types work.  */
  if (TYPE_CODE (type) == TYPE_CODE_STRUCT)
    result = context->build_record_type ();
  else
    {
      gdb_assert (TYPE_CODE (type) == TYPE_CODE_UNION);
      result = context->build_union_type ();
    }
  context->insert_type (type, result);

  for (i = 0; i < TYPE_NFIELDS (type); ++i)
    {
      gcc_type field_type;
      unsigned long bitsize = TYPE_FIELD_BITSIZE (type, i);

      field_type = context->convert_type (TYPE_FIELD_TYPE (type, i));
      if (bitsize == 0)
	bitsize = 8 * TYPE_LENGTH (TYPE_FIELD_TYPE (type, i));
      context->build_add_field (result, TYPE_FIELD_NAME (type, i),
				field_type, bitsize,
				TYPE_FIELD_BITPOS (type, i));
    }

  context->finish_record_or_union (result, TYPE_LENGTH (type));
  return result;
}

/* Convert an enum type to its gcc representation.  */

static gcc_type
convert_enum (struct compile_c_instance *context, struct type *type)
{
  gcc_type int_type, result;
  int i;

  if (context->c_version () >= GCC_C_FE_VERSION_1)
    int_type = context->int_type (TYPE_UNSIGNED (type), TYPE_LENGTH (type),
				  NULL);
  else
    int_type = context->int_type (TYPE_UNSIGNED (type), TYPE_LENGTH (type));

  result = context->build_enum_type (int_type);
  for (i = 0; i < TYPE_NFIELDS (type); ++i)
    {
      context->build_add_enum_constant (result, TYPE_FIELD_NAME (type, i),
					TYPE_FIELD_ENUMVAL (type, i));
    }

  context->finish_enum_type (result);

  return result;
}

/* Convert a function type to its gcc representation.  */

static gcc_type
convert_func (struct compile_c_instance *context, struct type *type)
{
  int i;
  gcc_type result, return_type;
  struct gcc_type_array array;
  int is_varargs = TYPE_VARARGS (type) || !TYPE_PROTOTYPED (type);

  /* This approach means we can't make self-referential function
     types.  Those are impossible in C, though.  */
  return_type = context->convert_type (TYPE_TARGET_TYPE (type));

  array.n_elements = TYPE_NFIELDS (type);
  array.elements = XNEWVEC (gcc_type, TYPE_NFIELDS (type));
  for (i = 0; i < TYPE_NFIELDS (type); ++i)
    array.elements[i] = context->convert_type (TYPE_FIELD_TYPE (type, i));

  result = context->build_function_type (return_type, &array, is_varargs);
  xfree (array.elements);

  return result;
}

/* Convert an integer type to its gcc representation.  */

static gcc_type
convert_int (struct compile_c_instance *context, struct type *type)
{
  if (context->c_version () >= GCC_C_FE_VERSION_1)
    {
      if (TYPE_NOSIGN (type))
	{
	  gdb_assert (TYPE_LENGTH (type) == 1);
	  return context->char_type ();
	}
      return context->int_type (TYPE_UNSIGNED (type), TYPE_LENGTH (type),
				TYPE_NAME (type));
    }
  else
    return context->int_type (TYPE_UNSIGNED (type), TYPE_LENGTH (type));
}

/* Convert a floating-point type to its gcc representation.  */

static gcc_type
convert_float (struct compile_c_instance *context, struct type *type)
{
  if (context->c_version () >= GCC_C_FE_VERSION_1)
    return context->float_type (TYPE_LENGTH (type), TYPE_NAME (type));
  else
    return context->float_type (TYPE_LENGTH (type));
}

/* Convert the 'void' type to its gcc representation.  */

static gcc_type
convert_void (struct compile_c_instance *context, struct type *type)
{
  return context->void_type ();
}

/* Convert a boolean type to its gcc representation.  */

static gcc_type
convert_bool (struct compile_c_instance *context, struct type *type)
{
  return context->bool_type ();
}

/* Convert a qualified type to its gcc representation.  */

static gcc_type
convert_qualified (struct compile_c_instance *context, struct type *type)
{
  struct type *unqual = make_unqualified_type (type);
  gcc_type unqual_converted;
  gcc_qualifiers_flags quals = 0;

  unqual_converted = context->convert_type (unqual);

  if (TYPE_CONST (type))
    quals |= GCC_QUALIFIER_CONST;
  if (TYPE_VOLATILE (type))
    quals |= GCC_QUALIFIER_VOLATILE;
  if (TYPE_RESTRICT (type))
    quals |= GCC_QUALIFIER_RESTRICT;

  return context->build_qualified_type (unqual_converted, quals);
}

/* Convert a complex type to its gcc representation.  */

static gcc_type
convert_complex (struct compile_c_instance *context, struct type *type)
{
  gcc_type base = context->convert_type (TYPE_TARGET_TYPE (type));

  return context->build_complex_type (base);
}

/* A helper function which knows how to convert most types from their
   gdb representation to the corresponding gcc form.  This examines
   the TYPE and dispatches to the appropriate conversion function.  It
   returns the gcc type.  */

static gcc_type
convert_type_basic (struct compile_c_instance *context, struct type *type)
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

  return context->error (_("cannot convert gdb type to gcc type"));
}

gcc_type
compile_c_instance::convert_type (struct type *type)
{
  /* We don't ever have to deal with typedefs in this code, because
     those are only needed as symbols by the C compiler.  */
  type = check_typedef (type);

  type_map_t::iterator pos = m_type_map.find (type);
  if (pos != m_type_map.end ())
    return pos->second;

  gcc_type result = convert_type_basic (this, type);
  insert_type (type, result);
  return result;
}

#define FORWARD(OP,...) (m_context->c_ops->OP (m_context, ##__VA_ARGS__))

unsigned int
compile_c_instance::c_version () const
{
  return m_context->c_ops->c_version;
}

bool
compile_c_instance::tagbind (const char *name, gcc_type tagged_type,
			     const char *filename, unsigned int line_number)
{
  return FORWARD (tagbind, name, tagged_type, filename, line_number);
}

bool
compile_c_instance::build_constant (gcc_type type, const char *name,
				    unsigned long value, const char *filename,
				    unsigned int line_number)
{
  return FORWARD (build_constant, type, name, value, filename, line_number);
}

gcc_decl
compile_c_instance::build_decl (const char *name,
				enum gcc_c_symbol_kind sym_kind,
				gcc_type sym_type,
				const char *substitution_name,
				gcc_address address, const char *filename,
				unsigned int line_number)
{
  return FORWARD (build_decl, name, sym_kind, sym_type, substitution_name,
		  address, filename, line_number);
}

bool
compile_c_instance::bind (gcc_decl decl, bool is_global)
{
  return FORWARD (bind, decl, is_global);
}

gcc_type
compile_c_instance::error (const char *message)
{
  return FORWARD (error, message);
}

gcc_type
compile_c_instance::build_pointer_type (gcc_type base_type)
{
  return FORWARD (build_pointer_type, base_type);
}

gcc_type
compile_c_instance::build_vla_array_type (gcc_type element_type,
					  const char *upper_bound_name)
{
  return FORWARD (build_vla_array_type, element_type, upper_bound_name);
}

gcc_type
compile_c_instance::build_vector_type (gcc_type element_type, int num_elements)
{
  return FORWARD (build_vector_type, element_type, num_elements);
}

gcc_type
compile_c_instance::build_array_type (gcc_type element_type, int num_elements)
{
  return FORWARD (build_array_type, element_type, num_elements);
}

gcc_type
compile_c_instance::build_record_type ()
{
  return FORWARD (build_record_type);
}

gcc_type
compile_c_instance::build_union_type ()
{
  return FORWARD (build_union_type);
}

bool
compile_c_instance::build_add_field (gcc_type record_or_union_type,
				     const char *field_name,
				     gcc_type field_type, unsigned long bitsize,
				     unsigned long bitpos)
{
  return FORWARD (build_add_field, record_or_union_type, field_name,
		  field_type, bitsize, bitpos);
}

bool
compile_c_instance::finish_record_or_union (gcc_type record_or_union_type,
					    unsigned long size_in_bytes)
{
  return FORWARD (finish_record_or_union, record_or_union_type, size_in_bytes);
}

gcc_type
compile_c_instance::int_type (bool is_unsigned, unsigned long size_in_bytes,
			      const char *builtin_name)
{
  return FORWARD (int_type, is_unsigned, size_in_bytes, builtin_name);
}

gcc_type
compile_c_instance::int_type (bool is_unsigned, unsigned long size_in_bytes)
{
  return FORWARD (int_type_v0, is_unsigned, size_in_bytes);
}

gcc_type
compile_c_instance::build_enum_type (gcc_type underlying_int_type)
{
  return FORWARD (build_enum_type, underlying_int_type);
}

bool
compile_c_instance::build_add_enum_constant (gcc_type enum_type,
					     const char *name,
					     unsigned long value)
{
  return FORWARD (build_add_enum_constant, enum_type, name, value);
}

bool
compile_c_instance::finish_enum_type (gcc_type enum_type)
{
  return FORWARD (finish_enum_type, enum_type);
}

gcc_type
compile_c_instance::build_function_type (gcc_type return_value,
					 const struct gcc_type_array *argument_types,
					 bool is_varargs)
{
  return FORWARD (build_function_type, return_value, argument_types,
		  is_varargs);
}

gcc_type
compile_c_instance::char_type ()
{
  return FORWARD (char_type);
}

gcc_type
compile_c_instance::float_type (unsigned long size_in_bytes,
				const char *builtin_name)
{
  return FORWARD (float_type, size_in_bytes, builtin_name);
}

gcc_type
compile_c_instance::float_type (unsigned long size_in_bytes)
{
  return FORWARD (float_type_v0, size_in_bytes);
}

gcc_type
compile_c_instance::void_type ()
{
  return FORWARD (void_type);
}

gcc_type
compile_c_instance::bool_type ()
{
  return FORWARD (bool_type);
}

gcc_type
compile_c_instance::build_qualified_type (gcc_type unqualified_type,
					  enum gcc_qualifiers qualifiers)
{
  return FORWARD (build_qualified_type, unqualified_type, qualifiers);
}

gcc_type
compile_c_instance::build_complex_type (gcc_type element_type)
{
  return FORWARD (build_complex_type, element_type);
}

#undef FORWARD
