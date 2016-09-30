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
#include "compile-cplus-templates.hh"

struct type;

// enum-flags wrapper
DEF_ENUM_FLAGS_TYPE (enum gcc_cp_qualifiers, gcc_cp_qualifiers_flags);
DEF_ENUM_FLAGS_TYPE (enum gcc_cp_ref_qualifiers, gcc_cp_ref_qualifiers_flags);
DEF_ENUM_FLAGS_TYPE (enum gcc_cp_symbol_kind, gcc_cp_symbol_kind_flags);

/* Flag to enable internal debugging for C++ type-conversion.  */

extern int debug_compile_cplus_types;

/* A subclass of compile_instance that is specific to the C++ front
   end.  */
struct compile_cplus_instance
{
  /* Base class.  Note that the base class vtable actually points to a
     gcc_c_fe_vtable.  */

  struct compile_instance base;

  /* Map from gdb types to gcc types.  */

  htab_t type_map;

  /* Map from gdb symbols to gcc error messages to emit.  */

  htab_t symbol_err_map;

  // !!keiths: If I moved these into another class, I could probably
  // avoid having to #include compile-cplus-templates.hh everywhere...
  // A cache of function template definitions.
  compile::cplus::templates::function_template_defn_map *function_template_defns;
  
  // A cache of class template definitions.
  compile::cplus::templates::class_template_defn_map *class_template_defns;
};

/* A helper macro that takes a compile_cplus_instance and returns its
   corresponding gcc_cp_context.  */

 #define CP_CTX(I) ((struct gcc_cp_context *) ((I)->base.fe))

namespace compile::cplus
{
  /* Return the declaration name of the natural name NATURAL.
     This returns a name with no function arguments or template parameters.
     The result must be freed by the caller.  */

  char *decl_name (const char *natural);

  // Get the search block to use for symbol searches.

  const struct block *get_current_search_block (void);

  // Add the qualifiers given by QUALS to BASE.

  gcc_type convert_qualified_base (struct compile_cplus_instance *instance,
				   gcc_type base,
				   gcc_cp_qualifiers_flags quals);

  // Convert TARGET into a pointer type in the given compiler INSTANCE.

  gcc_type convert_pointer_base (struct compile_cplus_instance *instance,
				 gcc_type target);

  // Convert BASE into a reference type in the given compile INSTANCE.

  gcc_type
  convert_reference_base (struct compile_cplus_instance *instance,
			  gcc_type base);

  /* Returns non-zero if the given TYPE represents a varargs function,
     zero otherwise.  */

  int is_varargs_p (const struct type *type);

  /* Get the access flag for the NUM'th method of TYPE's FNI'th
     fieldlist.  */

  enum gcc_cp_symbol_kind
  get_method_access_flag (const struct type *type, int fni, int num);
};

/* Convert a gdb type, TYPE, to a GCC type.  INSTANCE is used to do the
   actual conversion.  If this type was defined in another type,
   NESTED_ACCESS should indicate the accessibility of this type
   (or GCC_CP_ACCESS_NONE if not a nested type).

   The new GCC type is returned.  */

extern gcc_type convert_cplus_type (struct compile_cplus_instance *instance,
				    struct type *type,
				    enum gcc_cp_symbol_kind nested_access);

/* A callback suitable for use as the GCC C++ symbol oracle.  */

extern gcc_cp_oracle_function gcc_cplus_convert_symbol;

/* A callback suitable for use as the GCC C++ address oracle.  */

extern gcc_cp_symbol_address_function gcc_cplus_symbol_address;

/* Callbacks suitable for use as the GCC C++ enter/leave scope requests.  */

extern gcc_cp_enter_leave_user_expr_scope_function gcc_cplus_enter_scope;
extern gcc_cp_enter_leave_user_expr_scope_function gcc_cplus_leave_scope;

/* Instantiate a GDB object holding state for the GCC context FE.  The
   new object is returned.  */

extern struct compile_instance *new_cplus_compile_instance
  (struct gcc_cp_context *fe);

/* Emit code to compute the address for all the local variables in
   scope at PC in BLOCK.  Returns a malloc'd vector, indexed by gdb
   register number, where each element indicates if the corresponding
   register is needed to compute a local variable.  */

extern unsigned char *generate_cplus_for_variable_locations
  (struct compile_cplus_instance *compiler,
   struct ui_file *stream,
   struct gdbarch *gdbarch,
   const struct block *block,
   CORE_ADDR pc);

/* Get the GCC mode attribute value for a given type size.  */

extern const char *cplus_get_mode_for_size (int size);

/* Given a dynamic property, return an xmallocd name that is used to
   represent its size.  The result must be freed by the caller.  The
   contents of the resulting string will be the same each time for
   each call with the same argument.  */

struct dynamic_prop;
extern char *cplus_get_range_decl_name (const struct dynamic_prop *prop);

/* A macro to help with calling the plug-in.  */
#define CPCALL(OP,CONTEXT,...)  \
  (CP_CTX ((CONTEXT))->cp_ops->OP (CP_CTX ((CONTEXT)), ##__VA_ARGS__))

struct compile_cplus_context;
extern void ccp_push_processing_context (struct compile_cplus_instance *,
					 struct compile_cplus_context *);
extern int ccp_need_new_context (const struct compile_cplus_context *);

extern void ccp_pop_processing_context (struct compile_cplus_instance *,
					struct compile_cplus_context *);
extern struct compile_cplus_context *
  new_processing_context (struct compile_cplus_instance *,
			  const char *, struct type *, gcc_type *);

extern void delete_processing_context (struct compile_cplus_context *);

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

#endif // GDB_COMPILE_CPLUS_H
