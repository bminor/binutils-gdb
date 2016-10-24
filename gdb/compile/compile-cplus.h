/* Header file for GDB compile C++ language support.
   Copyright (C) 2016 Free Software Foundation, Inc.

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

#ifndef GDB_COMPILE_CPLUS_H
#define GDB_COMPILE_CPLUS_H

#include "gcc-cp-interface.h"
#include "common/enum-flags.h"
#include "compile-cplus-templates.h"

struct type;
struct block;
struct compile_cplus_context;

// enum-flags wrapper
DEF_ENUM_FLAGS_TYPE (enum gcc_cp_qualifiers, gcc_cp_qualifiers_flags);
DEF_ENUM_FLAGS_TYPE (enum gcc_cp_ref_qualifiers, gcc_cp_ref_qualifiers_flags);
DEF_ENUM_FLAGS_TYPE (enum gcc_cp_symbol_kind, gcc_cp_symbol_kind_flags);

namespace compile
{
  /* A subclass of compile_instance that is specific to the C++ front
     end.  */

  class compile_cplus_instance
    : public compile_instance
  {
  public:

    explicit compile_cplus_instance (struct gcc_cp_context *gcc_fe);
    ~compile_cplus_instance ();

    /* If SYM is a template symbol whose generic we have not yet declared,
       add it to INSTANCE's list of template definitions and scan for default
       values.  */

    void
    maybe_define_new_function_template (const struct symbol *sym,
					struct type *parent_type,
					int f_idx, int m_idx);

    /* If TYPE (with declaration name DECL_NAME) represents a concrete instance
       of a new class template, note the new template definition.  */

    void
    maybe_define_new_class_template (struct type *type, const char *decl_name);

    /* Find the generic template definition for TSYM or NULL if none was
       found.  */

    compile::function_template_defn *
      find_function_template_defn (struct template_symbol *tsym);

    /* Search for an existing class template definition based on TYPE.
       Returns NULL if no template based on TYPE is known.  */

    compile::class_template_defn *
      find_class_template_defn (struct type *type);

    // Emit any new function template definitions to the compiler plug-in.

    void emit_function_template_decls ();

    // Emit any new class template definitions to the compiler plug-in.

    void emit_class_template_decls ();

    /* Convert a gdb type, TYPE, to a GCC type.

       If this type was defined in another type, NESTED_ACCESS should indicate
       the accessibility of this type (or GCC_CP_ACCESS_NONE if not a nested
       type).

       The new GCC type is returned.  */

    gcc_type convert_type (struct type *type,
			   enum gcc_cp_symbol_kind nested_access);

    struct compile_cplus_context *
    new_context (const char *type_name, struct type *type,
		 gcc_type *nested_type);
    void push_context (struct compile_cplus_context *);
    bool need_new_context (const struct compile_cplus_context *);
    void pop_context (struct compile_cplus_context *);
    void delete_context (struct compile_cplus_context *);

    // !!keiths: YUCK!
    // Plug-in forwards

    bool build_constant (gcc_type type, const char *name, unsigned long value,
			 const char *filename, unsigned int line_number);

    gcc_decl specialize_function_template (struct template_symbol *concrete,
					   gcc_address address,
					   const char *filename,
					   unsigned int line_number);

    gcc_decl specialize_class_template (struct type *concrete,
					const char *filename,
					unsigned int line_number);

    // DECL_DESC for debugging only
    gcc_decl new_decl (const char *decl_desc, const char *name,
		       enum gcc_cp_symbol_kind sym_kind,
		       gcc_type sym_type, const char *substitution_name,
		       gcc_address address,
		       const char *filename, unsigned int line_number);

    bool push_namespace (const char *name);

    // NAME is for debugging only
    bool pop_namespace (const char *name);

    gcc_type error (const char *message);

    gcc_type build_reference_type (gcc_type base_type,
				   enum gcc_cp_ref_qualifiers rquals);

    gcc_type build_pointer_type (gcc_type base_type);

    gcc_type build_vla_array_type (gcc_type element_type,
				   const char *upper_bound_name);

    gcc_type build_vector_type (gcc_type element_type, int num_elements);

    gcc_type build_array_type (gcc_type element_type, int num_elements);

    gcc_decl new_field (const char *field_name, gcc_type field_type,
			enum gcc_cp_symbol_kind field_flags,
			unsigned long bitsize, unsigned long bitpos);

    gcc_type build_method_type (gcc_type class_type, gcc_type func_type,
				enum gcc_cp_qualifiers quals,
				enum gcc_cp_ref_qualifiers rquals);

    // NAME only for debugging
    gcc_type start_class_definition (const char *name, gcc_decl typedecl,
				     const struct gcc_vbase_array *base_classes,
				     const char *filename,
				     unsigned int line_number);

    // NAME for debugging
    bool finish_record_or_union (const char *name, unsigned long size_in_bytes);

    gcc_type int_type (bool is_unsigned, unsigned long size_in_bytes,
		       const char *builtin_name);

    gcc_type start_new_enum_type (const char *name, gcc_type underlying_int_type,
				  enum gcc_cp_symbol_kind flags,
				  const char *filename, unsigned int line_number);

    gcc_decl build_add_enum_constant (gcc_type enum_type, const char *name,
				      unsigned long value);

    bool finish_enum_type (gcc_type enum_type);

    gcc_type build_function_type (gcc_type return_type,
				  const struct gcc_type_array *argument_types,
				  bool is_varargs);

    gcc_type char_type ();

    gcc_type float_type (unsigned long size_in_bytes, const char *builtin_name);

    gcc_type void_type ();

    gcc_type bool_type ();

    gcc_type build_qualified_type (gcc_type unqualified_type,
				   enum gcc_cp_qualifiers qualifiers);

    gcc_type build_complex_type (gcc_type element_type);

    gcc_expr literal_expr (gcc_type type, unsigned long value);

    gcc_type new_template_typename_parm (const char *id, bool pack_p,
					 gcc_type default_type,
					 const char *filename,
					 unsigned int line_number);

    gcc_decl new_template_value_parm (gcc_type type, const char *id,
				      gcc_expr default_value,
				      const char *filename,
				      unsigned int line_number);

    // GENERIC only for debugging
    bool start_new_template_decl (const char *generic);

  private:

    // Enumerate the template arguments fo template DEFN into DEST.

    void
      enumerate_template_arguments
      (struct gcc_cp_template_args *dest,
       const compile::template_defn *defn,
       const struct template_argument_info *arg_info);

    // The C++ compile plug-in context.
    struct gcc_cp_context *m_context;

    // A cache of function template definitions.
    compile::function_template_defn_map_t *
      m_function_template_defns;

    // A cache of class template definitions.
    compile::class_template_defn_map_t *
      m_class_template_defns;
  };

  /* Return the declaration name of the natural name NATURAL.
     This returns a name with no function arguments or template parameters.
     The result must be freed by the caller.  */

  char *decl_name (const char *natural);

  // Add the qualifiers given by QUALS to BASE.

  gcc_type convert_qualified_base (compile_cplus_instance *instance,
				   gcc_type base,
				   gcc_cp_qualifiers_flags quals);

  // Convert TARGET into a pointer type in the given compiler INSTANCE.

  gcc_type convert_pointer_base (compile_cplus_instance *instance,
				 gcc_type target);

  // Convert BASE into a reference type in the given compile INSTANCE.

  gcc_type
  convert_reference_base (compile_cplus_instance *instance,
			  gcc_type base);

  /* Returns non-zero if the given TYPE represents a varargs function,
     zero otherwise.  */

  int is_varargs_p (const struct type *type);

  /* Get the access flag for the NUM'th method of TYPE's FNI'th
     fieldlist.  */

  enum gcc_cp_symbol_kind
  get_method_access_flag (const struct type *type, int fni, int num);

/* Maybe canonicalize FIELD_NAME with function field METHOD_FIELD (may
   be NULL for non-constructors) and METHOD_TYPE (may not be NULL).

   If the field is not represented by one of the plug-in's "special functions,"
   (operators, ctors, dtors), return FIELD_NAME.

   Otherwise return the unique plug-in identifier for the function.

   If memory was allocated for the name (required by some function types),
   it *OUTNAME will be set and should be used over the return value.  It
   must subsequently be freed by the caller.

   If the given method should be ignored (not defined to the plug-in),
   IGNORE will be true.  */

  extern const char *
  maybe_canonicalize_special_function (const char *field_name,
				       const struct fn_field *method_field,
				       const struct type *method_type,
				       char **outname,
				       bool *ignore);
};

/* A callback suitable for use as the GCC C++ symbol oracle.  */

extern gcc_cp_oracle_function gcc_cplus_convert_symbol;

/* A callback suitable for use as the GCC C++ address oracle.  */

extern gcc_cp_symbol_address_function gcc_cplus_symbol_address;

/* Callbacks suitable for use as the GCC C++ enter/leave scope requests.  */

extern gcc_cp_enter_leave_user_expr_scope_function gcc_cplus_enter_scope;
extern gcc_cp_enter_leave_user_expr_scope_function gcc_cplus_leave_scope;

#endif // GDB_COMPILE_CPLUS_H
