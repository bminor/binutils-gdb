/* Header file for GDB compile C-language support.
   Copyright (C) 2014-2016 Free Software Foundation, Inc.

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

#ifndef GDB_COMPILE_C_H
#define GDB_COMPILE_C_H

#include "common/enum-flags.h"

/* enum-flags wrapper.  */
DEF_ENUM_FLAGS_TYPE (enum gcc_qualifiers, gcc_qualifiers_flags);

/* A callback suitable for use as the GCC C symbol oracle.  */

extern gcc_c_oracle_function gcc_convert_symbol;

/* A callback suitable for use as the GCC C address oracle.  */

extern gcc_c_symbol_address_function gcc_symbol_address;

/* A subclass of compile_instance that is specific to the C front
   end.  */

class compile_c_instance
  : public compile_instance
{
 public:

  compile_c_instance (struct gcc_base_context *base, const char *options)
    : compile_instance (base, options)
    {
    }

  explicit compile_c_instance (struct gcc_c_context *gcc_c)
    : compile_instance (&gcc_c->base,
			"-std=gnu11"
			/* Otherwise the .o file may need
			   "_Unwind_Resume" and
			   "__gcc_personality_v0".  */
			" -fno-exceptions"
			" -Wno-implicit-function-declaration"),
      m_context (gcc_c)
  {
    m_context->c_ops->set_callbacks (m_context, gcc_convert_symbol,
				     gcc_symbol_address, this);
  }

  ~compile_c_instance ()
    {
      m_gcc_fe->ops->destroy (m_gcc_fe);
    }

  /* Convert a gdb type, TYPE, to a GCC type.

     The new GCC type is returned.  */

  gcc_type convert_type (struct type *type);

  // Plug-in forwards

  unsigned int c_version () const;

  bool tagbind (const char *name, gcc_type tagged_type, const char *filename,
		unsigned int line_number);

  bool build_constant (gcc_type type, const char *name, unsigned long value,
		       const char *filename, unsigned int line_number);

  gcc_decl build_decl (const char *name, enum gcc_c_symbol_kind sym_kind,
		       gcc_type sym_type, const char *substitution_name,
		       gcc_address address, const char *filename,
		       unsigned int line_number);

  bool bind (gcc_decl decl, bool is_global);

  gcc_type error (const char *message);

  gcc_type build_pointer_type (gcc_type base_type);

  gcc_type build_vla_array_type (gcc_type element_type,
				 const char *upper_bound_name);

  gcc_type build_vector_type (gcc_type element_type, int num_elements);

  gcc_type build_array_type (gcc_type element_type, int num_elements);

  gcc_type build_record_type ();

  gcc_type build_union_type ();

  bool build_add_field (gcc_type record_or_union_type, const char *field_name,
			gcc_type field_type, unsigned long bitsize,
			unsigned long bitpos);

  bool finish_record_or_union (gcc_type record_or_union_type,
			       unsigned long size_in_bytes);

  gcc_type int_type (bool is_unsigned, unsigned long size_in_bytes,
		     const char *builtin_name);

  gcc_type int_type (bool is_unsigned, unsigned long size_in_bytes);

  gcc_type build_enum_type (gcc_type underlying_int_type);

  bool build_add_enum_constant (gcc_type enum_type, const char *name,
				unsigned long value);

  bool finish_enum_type (gcc_type enum_type);

  gcc_type build_function_type (gcc_type return_value,
				const struct gcc_type_array *argument_types,
				bool is_varargs);

  gcc_type char_type ();

  gcc_type float_type (unsigned long size_in_bytes, const char *builtin_name);

  gcc_type float_type (unsigned long size_in_bytes);

  gcc_type void_type ();

  gcc_type bool_type ();

  gcc_type build_qualified_type (gcc_type unqualified_type,
				 enum gcc_qualifiers qualifiers);

  gcc_type build_complex_type (gcc_type element_type);

private:

  // The GCC C context.
  struct gcc_c_context *m_context;
};

/* Emit code to compute the address for all the local variables in
   scope at PC in BLOCK.  Returns a malloc'd vector, indexed by gdb
   register number, where each element indicates if the corresponding
   register is needed to compute a local variable.  */

extern unsigned char *generate_c_for_variable_locations
     (struct compile_instance *compiler,
      struct ui_file *stream,
      struct gdbarch *gdbarch,
      const struct block *block,
      CORE_ADDR pc);

/* Get the GCC mode attribute value for a given type size.  */

extern const char *c_get_mode_for_size (int size);

/* Given a dynamic property, return an xmallocd name that is used to
   represent its size.  The result must be freed by the caller.  The
   contents of the resulting string will be the same each time for
   each call with the same argument.  */

struct dynamic_prop;
extern char *c_get_range_decl_name (const struct dynamic_prop *prop);

/* Compute the name of the pointer representing a local symbol's
   address.  */

extern char *c_symbol_substitution_name (struct symbol *sym);

#endif // GDB_COMPILE_C_H
