/* Header file for GDB compile C++ language support.
   Copyright (C) 2016, 2017 Free Software Foundation, Inc.

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

#include <string>
#include <memory>

struct type;
struct block;

/* enum-flags wrapper  */
DEF_ENUM_FLAGS_TYPE (enum gcc_cp_qualifiers, gcc_cp_qualifiers_flags);
DEF_ENUM_FLAGS_TYPE (enum gcc_cp_ref_qualifiers, gcc_cp_ref_qualifiers_flags);
DEF_ENUM_FLAGS_TYPE (enum gcc_cp_symbol_kind, gcc_cp_symbol_kind_flags);

namespace compile
{
  class compile_cplus_instance;

  /* A single component of a type's scope.  Type names are broken into
     "components," a series of unqualified names comprising the type name,
     e.g., "namespace1", "namespace2", "myclass".  */

  struct scope_component
  {
    /* The unqualified name of this scope.  */
    std::string name;

    /* The block symbol for this type/scope.  */
    struct block_symbol bsymbol;
  };

  /* Comparison operators for scope_components.  */

  bool operator== (const scope_component &lhs, const scope_component &rhs);
  bool operator!= (const scope_component &lhs, const scope_component &rhs);


  /* A single compiler scope used to define a type.

     A compile_scope is a list of scope_components, where all leading
     scope_components are namespaces, followed by a single non-namespace
     type component (the actual type we are converting).  */

  class compile_scope : private std::vector<scope_component>
  {
  public:

    using std::vector<scope_component>::push_back;
    using std::vector<scope_component>::pop_back;
    using std::vector<scope_component>::back;
    using std::vector<scope_component>::empty;
    using std::vector<scope_component>::size;
    using std::vector<scope_component>::begin;
    using std::vector<scope_component>::end;
    using std::vector<scope_component>::operator[];

    compile_scope ()
      : m_nested_type (GCC_TYPE_NONE), m_pushed (false)
    {
    }

    /* Return the gcc_type of the type if it is a nested definition.
       Returns GCC_TYPE_NONE if this type was not nested.  */

    gcc_type nested_type ()
    {
      return m_nested_type;
    }

  private:
    /* compile_cplus_instance is a friend class so that it can set the
       following private members when compile_scopes are created.  */

    friend compile_cplus_instance;

    /* If the type was actually a nested type, this will hold that nested
       type after the scope is pushed.  */
    gcc_type m_nested_type;

    /* If true, this scope was pushed to the compiler and all namespaces
       must be popped when leaving the scope.  */
    bool m_pushed;
  };

  /* Comparison operators for compile_scopes.  */

  bool operator== (const compile_scope &lhs, const compile_scope &rhs);
  bool operator!= (const compile_scope &lhs, const compile_scope &rhs);

  /* A subclass of compile_instance that is specific to the C++ front
     end.  */

  class compile_cplus_instance
    : public compile_instance
  {
  public:

    explicit compile_cplus_instance (struct gcc_cp_context *gcc_fe);

    /* If SYM is a template symbol whose generic we have not yet declared,
       add it to INSTANCE's list of template definitions and scan for default
       values.  */

    void maybe_define_new_function_template (const struct symbol *sym,
					     struct type *parent_type,
					     int f_idx, int m_idx);

    /* If TYPE (with declaration name DECL_NAME) represents a concrete instance
       of a new class template, note the new template definition.  */

    void maybe_define_new_class_template (struct type *type,
					  const char *decl_name);

    /* Find the generic template definition for TSYM or NULL if none was
       found.  */

    function_template_defn *find_function_template_defn
      (struct template_symbol *tsym);

    /* Search for an existing class template definition based on TYPE.
       Returns NULL if no template based on TYPE is known.  */

    class_template_defn *find_class_template_defn (struct type *type);

    /* Emit any new function template definitions to the compiler
       plug-in.  */

    void emit_function_template_decls ();

    /* Emit any new class template definitions to the compiler
       plug-in.  */

    void emit_class_template_decls ();

    /* Convert a gdb type, TYPE, to a GCC type.

       If this type was defined in another type, NESTED_ACCESS should indicate
       the accessibility of this type (or GCC_CP_ACCESS_NONE if not a nested
       type).  GCC_CP_ACCESS_NONE is the default nested access.

       The new GCC type is returned.  */

    gcc_type convert_type
      (struct type *type,
       enum gcc_cp_symbol_kind nested_access = GCC_CP_ACCESS_NONE);

    /* Factory method to create a new scope based on TYPE with name TYPE_NAME.
       [TYPE_NAME could be TYPE_NAME or SYMBOL_NATURAL_NAME.]

       If TYPE is a nested or local definition, nested_type () will return
       the gcc_type of the conversion.

       Otherwise, nested_type () is GCC_TYPE_NONE.  */

    compile_scope new_scope (const char *type_name, struct type *type);

    /* Enter the given NEW_SCOPE.  */

    void enter_scope (compile_scope &scope);

    /* Leave the current scope.  */

    void leave_scope ();

    /* !!keiths: YUCK!
       Plug-in forwards  */

    gcc_type get_bool_type ();

    gcc_decl build_enum_constant (gcc_type enum_type, const char *name,
				  unsigned long value);

    gcc_type build_array_type (gcc_type element_type, int num_elements);

    bool build_constant (gcc_type type, const char *name, unsigned long value,
			 const char *filename, unsigned int line_number);

    gcc_type build_complex_type (gcc_type element_type);

    gcc_type build_function_type (gcc_type return_type,
				  const struct gcc_type_array *argument_types,
				  bool is_varargs);

    gcc_type build_method_type (gcc_type class_type, gcc_type func_type,
				enum gcc_cp_qualifiers quals,
				enum gcc_cp_ref_qualifiers rquals);

    gcc_type build_qualified_type (gcc_type unqualified_type,
				   enum gcc_cp_qualifiers qualifiers);

    gcc_type build_pointer_to_member_type (gcc_type class_type,
					   gcc_type member_type);

    gcc_type build_pointer_type (gcc_type base_type);

    gcc_type build_reference_type (gcc_type base_type,
				   enum gcc_cp_ref_qualifiers rquals);

    gcc_type build_vla_array_type (gcc_type element_type,
				   const char *upper_bound_name);

    gcc_type build_vector_type (gcc_type element_type, int num_elements);

    gcc_type get_char_type ();

    gcc_type error (const char *message);

    bool finish_enum_type (gcc_type enum_type);

    /* NAME for debugging  */
    bool finish_class_type (const char *name, unsigned long size_in_bytes);

    gcc_type get_float_type (unsigned long size_in_bytes, const char *builtin_name);

    gcc_type get_int_type (bool is_unsigned, unsigned long size_in_bytes,
		       const char *builtin_name);

    gcc_expr build_literal_expr (gcc_type type, unsigned long value);

    /* DECL_DESC for debugging only  */
    gcc_decl build_decl (const char *decl_desc, const char *name,
			 enum gcc_cp_symbol_kind sym_kind,
			 gcc_type sym_type, const char *substitution_name,
			 gcc_address address,
			 const char *filename, unsigned int line_number);

    gcc_decl build_field (const char *field_name, gcc_type field_type,
			  enum gcc_cp_symbol_kind field_flags,
			  unsigned long bitsize, unsigned long bitpos);

    gcc_type build_type_template_parameter (const char *id, bool pack_p,
					    gcc_type default_type,
					    const char *filename,
					    unsigned int line_number);

    gcc_decl build_value_template_parameter (gcc_type type, const char *id,
					     gcc_expr default_value,
					     const char *filename,
					     unsigned int line_number);

    /* NAME is for debugging only  */
    bool pop_binding_level (const char *name);

    bool push_namespace (const char *name);

    gcc_decl build_class_template_specialization (struct type *concrete,
						  const char *filename,
						  unsigned int line_number);

    gcc_decl build_function_template_specialization (struct template_symbol *concrete,
						     gcc_address address,
						     const char *filename,
						     unsigned int line_number);

    /* NAME only for debugging  */
    gcc_type start_class_type (const char *name, gcc_decl typedecl,
			       const struct gcc_vbase_array *base_classes,
			       const char *filename,
			       unsigned int line_number);

    gcc_type start_enum_type (const char *name,
			      gcc_type underlying_int_type,
			      enum gcc_cp_symbol_kind flags,
			      const char *filename,
			      unsigned int line_number);

    /* GENERIC only for debugging  */
    bool start_template_decl (const char *generic);

    gcc_type get_void_type ();


  private:

    /* Enumerate the template arguments of template DEFN into DEST.  */

    void enumerate_template_arguments
      (struct gcc_cp_template_args *dest, const template_defn *defn,
       const struct template_argument_info *arg_info);

    /* The C++ compile plug-in context.  */
    struct gcc_cp_context *m_context;

    /* A cache of function template definitions.  */
    std::unique_ptr<function_template_defn_map_t> m_function_template_defns;

    /* A cache of class template definitions.  */
    std::unique_ptr<class_template_defn_map_t> m_class_template_defns;

    /* A list of scopes we are processing.  */
    std::vector<compile_scope> m_scopes;
  };

  /* Return the declaration name of the natural name NATURAL.
     This returns a name with no function arguments or template parameters.
     The result must be freed by the caller.  */

  char *decl_name (const char *natural);

  /* Add the qualifiers given by QUALS to BASE.  */

  gcc_type convert_qualified_base (compile_cplus_instance *instance,
				   gcc_type base,
				   gcc_cp_qualifiers_flags quals);

  /* Convert TARGET into a pointer type in the given compiler
     INSTANCE.  */

  gcc_type convert_pointer_base (compile_cplus_instance *instance,
				 gcc_type target);

  /* Convert BASE into a reference type in the given compile
     INSTANCE.  */

  gcc_type convert_reference_base (compile_cplus_instance *instance,
				   gcc_type base);

  /* Returns non-zero if the given TYPE represents a varargs function,
     zero otherwise.  */

  int is_varargs_p (const struct type *type);

  /* Get the access flag for the NUM'th method of TYPE's FNI'th
     fieldlist.  */

  enum gcc_cp_symbol_kind get_method_access_flag (const struct type *type,
						  int fni, int num);

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

  const char *maybe_canonicalize_special_function
    (const char *field_name, const struct fn_field *method_field,
     const struct type *method_type, char **outname, bool *ignore);
};

/* A callback suitable for use as the GCC C++ symbol oracle.  */

extern gcc_cp_oracle_function gcc_cplus_convert_symbol;

/* A callback suitable for use as the GCC C++ address oracle.  */

extern gcc_cp_symbol_address_function gcc_cplus_symbol_address;

/* Callbacks suitable for use as the GCC C++ enter/leave scope requests.  */

extern gcc_cp_enter_leave_user_expr_scope_function gcc_cplus_enter_scope;
extern gcc_cp_enter_leave_user_expr_scope_function gcc_cplus_leave_scope;

#endif /* GDB_COMPILE_CPLUS_H  */
