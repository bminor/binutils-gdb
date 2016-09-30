/* Template support for compile.

   Copyright (C) 2016 Free Software Foundation, Inc.

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

#ifndef COMPILE_CPLUS_TEMPLATES_H
#define COMPILE_CPLUS_TEMPLATES_H
#include "gdbtypes.h"
#include "cp-support.h"

#include <string>
#include <vector>
#include <unordered_map>

struct symbol;
struct gcc_cp_template_args;
struct template_symbol;
struct compile_cplus_instance;

namespace compile::cplus::templates
{
  class function_template_defn;
  class class_template_defn;

  // Types used for tracking template definitions.
  typedef std::pair<std::string, function_template_defn *>
    function_template_map_item;
  typedef std::unordered_map<std::string, function_template_defn *>
    function_template_defn_map;
  typedef std::pair<std::string, class_template_defn *>
    class_template_map_item;
  typedef std::unordered_map<std::string, class_template_defn *>
    class_template_defn_map;

  // A base class holding data common to all template definitions.

  class template_defn
  {
  public:

    // Return the declaration name of this definition.

    const char *
    decl_name () const
    {
      return m_decl_name.c_str ();
    }

    // Return the compiler plug-in's decl for this definition.

    gcc_decl
    decl () const
    {
      return m_decl;
    }

    // Set the compiler plug-in's decl for this definition.

    void
    set_decl (gcc_decl decl)
    {
      m_decl = decl;
    }

    // Return the generic string for this template definition.

    const std::string
    generic (void) const
    {
      return m_generic;
    }

    // Return the compiler plug-in's abstract type for the IDX'th
    // template parameter.

    gcc_type
    parameter_abstract_type (unsigned int idx) const
    {
      return m_abstract_types[idx];
    }

    // Set the IDX'th template parameter's abstract type.

    void
    set_parameter_abstract_type (unsigned int idx, gcc_type type)
    {
      m_abstract_types[idx] = type;
    }

    // Has this template already been elided to the compiler plug-in?

    bool
    defined (void) const
    {
      return m_defined;
    }

    // Mark this definition as defined in the compiler plug-in.

    void
    set_defined (bool val)
    {
      m_defined = val;
    }

    // Return the ARG_NUM'th template parameter's default value
    // or NULL if none was set (or known).

    struct symbol *
    default_argument (unsigned int arg_num) const
    {
      return m_default_arguments[arg_num];
    }

    // Record the value of the ARG_NUM'th template parameter.

    void
    set_default_argument (unsigned int arg_num, struct symbol *value)
    {
      m_default_arguments[arg_num] = value;
    }

  protected:

    // Protected constructor so that no one instantiates this
    // type directly.
    //
    // DECL_NAME is the declaration name of this template, i.e., it's
    // name with no template parameters.  GENERIC is the computed generic
    // template definition.  N_PARAMETERS specifies how many template
    // parameters this template has.

    template_defn (std::string decl_name, std::string generic,
		   unsigned int n_parameters)
      : m_decl_name (decl_name), m_generic (generic),
	m_abstract_types (n_parameters), m_decl (0),
	m_default_arguments (n_parameters), m_defined (false)
    {
    }

  private:

    // The declaration name of the template, excluding any parameters.
    std::string m_decl_name;

    // A string representation of the generic template definition.
    std::string m_generic;

    // The abstract template parameter types.
    std::vector<gcc_type> m_abstract_types;

    // The decl associated with this template definition.
    gcc_decl m_decl;

    // A list of default values for the parameters of this template.
    std::vector<struct symbol *> m_default_arguments;

    /* Has this template already been defined?  This is a necessary evil
       since we have to traverse over all hash table entries.  */
    bool m_defined;
  };

  // A function template definition.

  class function_template_defn
    : public template_defn
  {
  public:

    // A unary function to delete map items.

    static void destroy (function_template_map_item p);

    // Construct a new function template definition with the generic
    // string representation GENERIC and demangle INFO, based on the
    // concrete instance given by template symbol TSYMBOL.
    //
    // If this definition is a method template, PARENT_TYPE is the type
    // of the closing class and FIDX and MIDX are the fieldlist and
    // method indices, respectively, which describe this method.
    //
    // If this definition is not a method template, PARENT_TYPE is NULL
    // and FIDX/MIDX are both -1.

    function_template_defn (std::string generic, parsed_demangle_info info,
			    const struct template_symbol *tsymbol,
			    struct type *parent_type, int fidx, int midx);

    // Return the demangle information for this template.

    const parsed_demangle_info *
    demangle_info (void) const
    {
      return &m_demangle_info;
    }

    // Return the concrete instance used to define this template.

    const struct template_symbol *
    template_symbol (void) const
    {
      return m_tsymbol;
    }

    // For method templates, return the type of the enclosing parent type,
    // or NULL for non-method templates.

    struct type *
    parent_type (void) const
    {
      return m_parent_type;
    }

    // For method templates, return the field list index in PARENT_TYPE
    // which describes this method.  Return -1 otherwise.
    int
    fidx (void) const
    {
      return m_fidx;
    }

    // For method templates, return the index of this method into the
    // field list (given by fidx()).  Return -1 otherwise.

    int
    midx (void) const
    {
      return m_midx;
    }

  private:

    // The template symbol used to create this template definition.
    // NOTE: Any given template_defn could be associated with any number
    // of template instances in the program.
    //
    // This field is not const since we will be lazily computing template
    // parameter indices for the function's argument and return types.

    const struct template_symbol *m_tsymbol;

    // The parent type or NULL if this does not represent a method.

    struct type *m_parent_type;

    // The fieldlist and method indices for the method or -1 if this template
    // definition does not represent a method.

    int m_fidx;
    int m_midx;

    // Demangle tree for the template defining this generic.
    //  Must be freed with cp_demangled_name_parse_free.

    parsed_demangle_info m_demangle_info;
  };

  // A class template definition.

  class class_template_defn
    : public template_defn
  {
  public:

    // A unary function to delete map items.

    static void destroy (class_template_map_item p);

    // Construct a new class template definition with the generic
    // string representation GENERIC based on the concrete instance
    // TYPE.

    class_template_defn (std::string decl_name, std::string generic,
			 struct type *type)
      : template_defn (decl_name, generic, TYPE_N_TEMPLATE_ARGUMENTS (type)),
	m_type (type)
    {
    }

    // Return concrete instance that this template definition was based on.

    struct type *
    type (void) const
    {
      return m_type;
    }

  private:

    // The type used to create this template definition.
    // NOTE: Any given template_defn could be associated with any number
    // of template instances in the program.
    struct type *m_type;
  };

  // Loop over SYMBOLS, defining any generic template definitions for
  // any template symbols in the list.

  void define_templates (struct compile_cplus_instance *instance,
			 VEC (block_symbol_d) *symbols);

  // Enumerate the template arguments fo template DEFN into DEST.

  void enumerate_template_arguments (struct compile_cplus_instance *instance,
				     struct gcc_cp_template_args *dest,
				     const template_defn *defn,
				     const struct template_argument_info *arg_info);

  // Emit any new function template definitions to the compiler plug-in
  // INSTANCE.

  void emit_function_template_decls (struct compile_cplus_instance *instance);

  // Scan TYPE for any new function templates.
  // Does not actually emit definitions for any new templates until
  // emit_function_template_decls is called.

  void scan_type_for_function_templates (struct compile_cplus_instance *instance,
					 struct type *type);

  // If TYPE (with declaration name NAME) represents a concrete instance
  // of a new template, note the new template definition in INSTANCE

  void maybe_define_new_class_template (struct compile_cplus_instance *instance,
					struct type *type, const char *name);

  // Emit any new class template definitions to the compiler plug-in
  // INSTANCE.

  void emit_class_template_decls (struct compile_cplus_instance *instance);

  // Search for an existing class template definition based on TYPE.
  // Returns NULL if no template based on TYPE is known.

  class_template_defn *
  find_class_template_defn (struct compile_cplus_instance *instance,
			    struct type *type);

  // Find the generic template definition for TSYM or NULL if none was
  // found.

  function_template_defn *
  find_function_template_defn (struct compile_cplus_instance *instance,
			       struct template_symbol *tsym);
};
#endif // COMPILE_CPLUS_TEMPLATES_H
