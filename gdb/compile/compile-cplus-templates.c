/* Template support for compile.

   Copyright (C) 2016, 2017 Free Software Foundation, Inc.

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
#include "compile-internal.h"
#include "compile-cplus.h"
#include "cp-support.h"
#include "demangle.h"
#include "typeprint.h"
#include "c-lang.h"
#include "gdbcmd.h"

#include <algorithm>

using namespace compile;

/* Modifiers for abstract template parameters when used in template function
   declarations, including CV and ref qualifiers and pointer and reference
   type modifiers, e.g., const T*.  */

enum template_parameter_type_modifier
{
  /*/ The abstract parameter type is not qualified at all.  */
  PARAMETER_NONE,

  /* The abstract parameter type was declared `const', const T.  */
  PARAMETER_CONST,

  /* The abstract parameter type was declared `volatile', volatile T.  */
  PARAMETER_VOLATILE,

  /* The abstract parameter type was declared `restrict', restrict T.  */
  PARAMETER_RESTRICT,

  /* The abstract parameter type was declared as a pointer, T*.  */
  PARAMETER_POINTER,

  /* The abstract parameter type was declared as a reference, T&.  */
  PARAMETER_LVALUE_REFERENCE,

  /* The abstract parameter type was declared as rvalue reference,
     T&&.  */
  PARAMETER_RVALUE_REFERENCE
};
typedef enum template_parameter_type_modifier template_parameter_modifier;

/* Forward declarations.  */

static void print_template_parameter_list
  (const struct template_argument_info *arg_info, struct ui_file *stream);

static void print_template_type (const struct demangle_component *comp,
				 const struct template_symbol *tsymbol,
				 struct ui_file *stream);

static void print_conversion_node (const struct demangle_component *comp,
				   const struct template_symbol *tsymbol,
				   struct ui_file *stream);

static void print_function_template_arglist
  (const struct demangle_component *comp,
   const struct template_symbol *tsymbol, struct ui_file *stream);

/* See description in compile-cplus-templates.h.  */

function_template_defn::function_template_defn
  (std::string generic, std::unique_ptr<demangle_parse_info> info,
   const struct template_symbol *tsymbol, struct type *parent_type,
   int fidx, int midx)
  : template_defn (compile::decl_name (tsymbol->search_name), generic,
		   tsymbol->template_arguments->n_arguments),
    m_tsymbol (tsymbol), m_parent_type (parent_type),
    m_fidx (fidx), m_midx (midx),
    m_demangle_info (std::move (info))
{
}

/* Return a string representing the template declaration for TSYMBOL.
   All template symbols deriving from the same source declaration should
   yield the same string representation.

   This string representation is of the generic form
   RETURN_TYPE QUALIFIED_NAME <parameter list>(argument list), with
   generic template parameters instead of any instanced type.

   For example, both "void foo<int> (int)" and "void foo<A> (A)" will
   return "T foo<typename T>(T)".  */

static std::string
function_template_decl (const struct template_symbol *tsymbol,
			const struct demangle_parse_info *info)
{
  gdb_assert (info != NULL);

  string_file stream;
  struct demangle_component *ret_comp = info->tree;

  if (ret_comp != NULL)
    {
      if (ret_comp->type == DEMANGLE_COMPONENT_TYPED_NAME)
	ret_comp = d_right (ret_comp);

      /* Print out the return type to the stream (if there is one).  */
      if (d_left (ret_comp) != NULL)
	{
	  if (tsymbol->template_return_index == -1)
	    {
	      struct type *return_type
		= TYPE_TARGET_TYPE (SYMBOL_TYPE (&tsymbol->base));

	      c_print_type (return_type, "", &stream, -1, 0,
			    &type_print_raw_options);
	    }
	  else
	    print_template_type (d_left (ret_comp), tsymbol, &stream);
	  stream.putc (' ');
	}

      /* Print the name of the template.  */
      if (tsymbol->conversion_operator_index != -1)
	print_conversion_node (info->tree, tsymbol, &stream);
      else
	{
	  stream.puts (tsymbol->search_name);
	  if (tsymbol->search_name[strlen (tsymbol->search_name) - 1] == '<')
	    stream.putc (' ');
	}

      /* Print out template (generic) arguments.  */
      stream.putc ('<');
      print_template_parameter_list (tsymbol->template_arguments, &stream);
      stream.putc ('>');

      /* Print out function arguments.  */
      stream.putc ('(');
      print_function_template_arglist (ret_comp, tsymbol, &stream);
      stream.putc (')');
    }

  return std::move (stream.string ());
}

/* Compute the generic used by the given function template
   definition.  */

static std::string
compute_function_template_generic (struct template_symbol *tsymbol,
				   const demangle_parse_info *info)
{
  gdb_assert (info->tree != NULL);

  /* Ensure template arguments have been decoded.  */
  cp_decode_template_type_indices (tsymbol, info);

  /* Output the template generic.  */
  return function_template_decl (tsymbol, info);
}

/* See description in compile-cplus.h.  */

void
compile_cplus_instance::maybe_define_new_function_template
  (const struct symbol *sym, struct type *parent_type, int f_idx,
   int m_idx)

{
  if (sym != NULL && SYMBOL_IS_CPLUS_TEMPLATE_FUNCTION (sym))
    {
      struct template_symbol *tsym = (struct template_symbol *) sym;

      if (tsym->linkage_name == NULL)
	return;

      std::unique_ptr<demangle_parse_info> info
	= cp_mangled_name_to_comp (tsym->linkage_name, DMGL_ANSI | DMGL_PARAMS);

      std::string generic
	= compute_function_template_generic (tsym, info.get ());
      function_template_defn_map_t::iterator pos
	= m_function_template_defns->find (generic);

      function_template_defn *defn;

      if (pos == m_function_template_defns->end ())
	{
	  /* Create the new template definition and insert it into
	     the cache.  */
	  defn = new function_template_defn (generic, std::move (info), tsym,
					     parent_type, f_idx, m_idx);
	  m_function_template_defns->insert (std::make_pair (generic, defn));
	}
      else
	{
	  /* Or use the existing definition.  */
	  defn = pos->second.get ();
	}

      /* Loop over the template arguments, noting any default values.  */
      for (unsigned int i = 0; i < tsym->template_arguments->n_arguments; ++i)
	{
	  if (defn->default_argument (i) == NULL
	      && tsym->template_arguments->default_arguments[i] != NULL)
	    {
	      struct symbol *def
		= tsym->template_arguments->default_arguments[i];
	      defn->set_default_argument (i, def);

	      /* We don't want to define them here because it could start
		 emitting template definitions before we're even done
		 collecting the default values.  [Easy to demonstrate if the
		 default value is a class.]  */
	    }
	}
    }
}

/* See description in compile-cplus-templates.h.  */

void
compile::define_templates (compile_cplus_instance *instance,
			   VEC (block_symbol_d) *symbols)
{
  int i;
  struct block_symbol *elt;

  /* We need to do this in two passes.  On the first pass, we collect
     the list of "unique" template definitions we need (using the template
     hashing function) and we collect the list of default values for the
     template (which can only be done after we have a list of all templates).
     On the second pass, we iterate over the list of templates we need to
     define, enumerating those definitions (with default values) to the
     compiler plug-in.  */

  for (i = 0; VEC_iterate (block_symbol_d, symbols, i, elt); ++i)
    instance->maybe_define_new_function_template (elt->symbol, NULL, -1, -1);

  /* From here on out, we MUST have all types declared or defined,
     otherwise GCC will give us "definition of TYPE in template parameter
     list."  */
  /* Create any new template definitions we encountered.  */
  instance->emit_function_template_decls ();
  instance->emit_class_template_decls ();
}

/* See description in compile-cplus-templates.h.  */

function_template_defn *
compile_cplus_instance::find_function_template_defn
  (struct template_symbol *tsym)
{
  if (tsym->linkage_name == NULL)
    return NULL;

  std::unique_ptr<demangle_parse_info> info
    = cp_mangled_name_to_comp (tsym->linkage_name, DMGL_ANSI | DMGL_PARAMS);

  std::string generic = compute_function_template_generic (tsym, info.get ());
  function_template_defn_map_t::iterator pos
    = m_function_template_defns->find (generic);

  if (pos != m_function_template_defns->end ())
    return pos->second.get ();

  return NULL;
}

/* Compute the generic used by the given function template
   definition.  */

static std::string
compute_class_template_generic (std::string name, struct type *type)
{
  string_file stream;

  /* Format: class|struct|union namespaces::NAME<parameters>  */
  if (TYPE_CODE (type) == TYPE_CODE_STRUCT)
    {
      if (TYPE_DECLARED_CLASS (type))
	stream.puts ("class ");
      else
	stream.puts ("struct ");
    }
  else
    {
      gdb_assert (TYPE_CODE (type) == TYPE_CODE_UNION);
      stream.puts ("union ");
    }

  /* Print all namespaces.  Note that we do not push the last
     scope_component -- that's the actual type we are defining.  */

  compile::compile_scope scope = type_name_to_scope (TYPE_NAME (type), NULL);
  std::for_each (scope.begin (), scope.end () - 1, [&] (const auto &comp)
     {
       gdb_assert (TYPE_CODE (SYMBOL_TYPE (comp.bsymbol.symbol))
		   == TYPE_CODE_NAMESPACE);

       if (comp.name != CP_ANONYMOUS_NAMESPACE_STR)
	 stream.printf ("%s::", comp.name.c_str ());
     });

  stream.printf ("%s<", name.c_str ());
  print_template_parameter_list (TYPE_TEMPLATE_ARGUMENT_INFO (type), &stream);
  stream.putc ('>');

  return std::move (stream.string ());
}

/* See description in compile-cplus-templates.h.  */

class_template_defn *
compile_cplus_instance::find_class_template_defn (struct type *type)
{
  /* There are no template definitions associated with anonymous types or
     types without template arguments.  */
  if (TYPE_NAME (type) == NULL || TYPE_TEMPLATE_ARGUMENT_INFO (type) == NULL)
    return NULL;

  char *name = decl_name (TYPE_NAME (type));
  struct cleanup *back_to = make_cleanup (xfree, name);

  std::string generic (compute_class_template_generic (name, type));
  class_template_defn_map_t::iterator pos
    = m_class_template_defns->find (generic);
  if (pos != m_class_template_defns->end ())
    {
      /* A template generic for this was already defined.  */
      do_cleanups (back_to);
      return pos->second.get ();
    }

  /* No generic for this template was found.  */
  do_cleanups (back_to);
  return NULL;
}

/* A class providing printing for a single parameter type modifier.  */

class one_template_type_modifier_printer
{
public:
  /* Construct a new printer which outputs to STREAM.  */
  explicit one_template_type_modifier_printer (struct ui_file *stream)
    : m_stream (stream)
  {
  }

  /* Unary function to output the modifier.  */
  void operator() (template_parameter_modifier modifier)
  {
    switch (modifier)
      {
      case PARAMETER_NONE:
	break;

      case PARAMETER_CONST:
	fputs_unfiltered (" const", m_stream);
	break;

      case PARAMETER_VOLATILE:
	fputs_unfiltered (" volatile", m_stream);
	break;

      case PARAMETER_RESTRICT:
	fputs_unfiltered (" restrict", m_stream);
	break;

      case PARAMETER_POINTER:
	fputs_unfiltered ("*", m_stream);
	break;

      case PARAMETER_LVALUE_REFERENCE:
	fputc_unfiltered ('&', m_stream);
	break;

      case PARAMETER_RVALUE_REFERENCE:
	fputs_unfiltered ("&&", m_stream);

      default:
	gdb_assert_not_reached ("unknown template parameter modifier");
      }
  }

private:
  /* The stream to which to print the modifier.  */
  struct ui_file *m_stream;
};

/* Print the type modifiers MODIFIERS to STREAM.  */

static void
print_template_type_modifiers
  (const std::vector<template_parameter_modifier> &modifiers,
   struct ui_file *stream)
{
  one_template_type_modifier_printer printer (stream);

  for (auto &item : modifiers)
    printer (item);
}

/* Get the abstract template type described by COMP, returning any
   type modifiers in MODIFIERS.  */

static const struct demangle_component *
get_template_type (const struct demangle_component *comp,
		   std::vector <template_parameter_modifier> &modifiers)
{
  bool done = 0;

  /* This is probably a little too simplistic...  */
  while (!done)
    {
      switch (comp->type)
	{
	case DEMANGLE_COMPONENT_POINTER:
	  modifiers.insert (modifiers.begin (), PARAMETER_POINTER);
	  comp = d_left (comp);
	  break;

	case DEMANGLE_COMPONENT_REFERENCE:
	  modifiers.insert (modifiers.begin (), PARAMETER_LVALUE_REFERENCE);
	  comp = d_left (comp);
	  break;

	case DEMANGLE_COMPONENT_CONST:
	  modifiers.insert (modifiers.begin (), PARAMETER_CONST);
	  comp = d_left (comp);
	  break;

	case DEMANGLE_COMPONENT_RESTRICT:
	  modifiers.insert (modifiers.begin (), PARAMETER_RESTRICT);
	  comp = d_left (comp);
	  break;

	case DEMANGLE_COMPONENT_VOLATILE:
	  modifiers.insert (modifiers.begin (), PARAMETER_VOLATILE);
	  comp = d_left (comp);
	  break;

	case DEMANGLE_COMPONENT_TEMPLATE_PARAM:
	default:
	  done = true;
	  break;
	}
    }

  return comp;
}

/* Print the generic parameter type given by COMP from the template symbol
   TSYMBOL to STREAM.  This function prints the generic template parameter
   type, not the instanced type, e.g., "const T&".  */

static void
print_template_type (const struct demangle_component *comp,
		     const struct template_symbol *tsymbol,
		     struct ui_file *stream)
{
  /* Get the template parameter and modifiers.  */
  std::vector<template_parameter_modifier> modifiers;
  comp = get_template_type (comp, modifiers);

  /* This had better be a template parameter!  */
  gdb_assert (comp->type == DEMANGLE_COMPONENT_TEMPLATE_PARAM);

  /* Using the parameter's index, get the parameter's symbol and print it
     with modifiers.  */
  long idx = comp->u.s_number.number;
  struct symbol *sym = tsymbol->template_arguments->arguments[idx];

  fputs_unfiltered (SYMBOL_NATURAL_NAME (sym), stream);
  print_template_type_modifiers (modifiers, stream);
}

/* Print the template parameter list of a type/symbol to STREAM.  */

static void
print_template_parameter_list (const struct template_argument_info *arg_info,
			       struct ui_file *stream)
{
  for (int i = 0; i < arg_info->n_arguments; ++i)
    {
      if (i != 0)
	fputs_unfiltered (", ", stream);

      switch (arg_info->argument_kinds[i])
	{
	case type_parameter:
	  fprintf_unfiltered (stream, "typename %s",
			      SYMBOL_NATURAL_NAME (arg_info->arguments[i]));
	  break;

	case value_parameter:
	  c_print_type (SYMBOL_TYPE (arg_info->arguments[i]), "", stream, -1, 0,
			&type_print_raw_options);
	  fprintf_unfiltered (stream, " %s",
			      SYMBOL_NATURAL_NAME (arg_info->arguments[i]));
	  break;

	case template_parameter:
	  break;

	case variadic_parameter:
	  break;

	default:
	  gdb_assert_not_reached ("unexpected template parameter kind");
	}
    }
}

/* Print out the generic template function argument list of the template
   symbol TSYMBOL to STREAM.  COMP represents the FUNCTION_TYPE of the
   demangle tree for TSYMBOL.  */

static void
print_function_template_arglist (const struct demangle_component *comp,
				 const struct template_symbol *tsymbol,
				 struct ui_file *stream)
{
  int i, artificials;
  struct type *ttype = SYMBOL_TYPE (&tsymbol->base);

  for (i = 0, artificials = 0; i < TYPE_NFIELDS (ttype); ++i)
    {
      int tidx;

      if (TYPE_FIELD_ARTIFICIAL (ttype, i))
	{
	  ++artificials;
	  continue;
	}

      if ((i - artificials) > 0)
	fputs_unfiltered (", ", stream);

      tidx = tsymbol->template_argument_indices[i - artificials];
      if (tidx == -1)
	{
	  /* A concrete type was used to define this argument.  */
	  c_print_type (TYPE_FIELD_TYPE (ttype, i), "", stream, -1, 0,
			&type_print_raw_options);
	  continue;
	}

      /* The type of this argument was specified by a template parameter,
	 possibly with added CV and ref qualifiers.  */

      /* Get the next ARGLIST node and print it.  */
      comp = d_right (comp);
      gdb_assert (comp != NULL);
      gdb_assert (comp->type == DEMANGLE_COMPONENT_ARGLIST);
      print_template_type (d_left (comp), tsymbol, stream);
    }
}

/* Print the conversion operator in COMP for the template symbol TSYMBOL
   to STREAM.  */

static void
print_conversion_node (const struct demangle_component *comp,
		       const struct template_symbol *tsymbol,
		       struct ui_file *stream)
{
  while (1)
    {
      switch (comp->type)
	{
	case DEMANGLE_COMPONENT_TYPED_NAME:
	case DEMANGLE_COMPONENT_TEMPLATE:
	  comp = d_left (comp);
	  break;

	case DEMANGLE_COMPONENT_QUAL_NAME:
	  {
	    /* Print out the qualified name.  */
	    struct cleanup *back_to;
	    char *ret = cp_comp_to_string (d_left (comp), 10);

	    back_to = make_cleanup (xfree, ret);
	    fprintf_unfiltered (stream, "%s::", ret);
	    do_cleanups (back_to);

	    /* Follow the rest of the name.  */
	    comp = d_right (comp);
	  }
	  break;

	case DEMANGLE_COMPONENT_CONVERSION:
	  fputs_unfiltered ("operator ", stream);
	  print_template_type (d_left (comp), tsymbol, stream);
	  return;

	default:
	  return;
	}
    }
}

/* See description in compile-cplus-templates.h.  */

void
compile_cplus_instance::maybe_define_new_class_template
  (struct type *type, const char *decl_name)
{
  if (TYPE_N_TEMPLATE_ARGUMENTS (type) == 0 || decl_name == NULL)
    return;

  std::string generic (compute_class_template_generic (decl_name, type));
  class_template_defn_map_t::iterator pos
    = m_class_template_defns->find (generic);

  class_template_defn *defn = NULL;

  if (pos == m_class_template_defns->end ())
    {
      /* Insert the new template definition into the cache.  */
      defn = new class_template_defn (decl_name, generic, type);
      m_class_template_defns->insert (std::make_pair (generic, defn));
    }
  else
    {
      /* If there is an existing definition, use that definition.  */
      defn = pos->second.get ();
    }

  /* Loop over the template arguments, noting any default values.  */
  for (unsigned int i = 0; i < TYPE_N_TEMPLATE_ARGUMENTS (type); ++i)
    {
      if (defn->default_argument (i) == NULL
	  && TYPE_TEMPLATE_DEFAULT_ARGUMENT (type, i) != NULL)
	{
	  defn->set_default_argument (i,
				      TYPE_TEMPLATE_DEFAULT_ARGUMENT (type, i));

	  /* We don't want to define them here because it could start
	     emitting template definitions before we're even done
	     collecting the default values.  [Easy to demonstrate if the
	     default value is a class.]  */
	}
    }
}

/* See description in compile-cplus-templates.h.  */

void
compile::scan_type_for_function_templates (compile_cplus_instance *instance,
					   struct type *type)
{
  for (int i = 0; i < TYPE_NFN_FIELDS (type); ++i)
    {
      struct fn_field *methods = TYPE_FN_FIELDLIST1 (type, i);

      for (int j = 0; j < TYPE_FN_FIELDLIST_LENGTH (type, i); ++j)
	{
	  struct block_symbol sym
	    = lookup_symbol (TYPE_FN_FIELD_PHYSNAME (methods, j),
			     instance->block (), VAR_DOMAIN, NULL);

	  instance->maybe_define_new_function_template (sym.symbol, type, i, j);
	}
    }
}

/* Helper function to define and return the `value' of TYPE of the template
   parameter ARG in compile INSTANCE.  */

static gcc_expr
get_template_argument_value (compile_cplus_instance *instance,
			     gcc_type type, struct symbol *arg)
{
  gcc_expr value = 0;

  switch (SYMBOL_CLASS (arg))
    {
      /* !!keiths: More (incomplete) fun.  */
    case LOC_CONST:
      value = instance->build_literal_expr (type, SYMBOL_VALUE (arg));
      break;

    case LOC_COMPUTED:
      {
	struct value *val;
	struct frame_info *frame = NULL;

	/* !!keiths: I don't think this can happen, but I've been
	   wrong before.  */
	if (symbol_read_needs_frame (arg))
	  {
	    frame = get_selected_frame (NULL);
	    gdb_assert (frame != NULL);
	  }
	val = read_var_value (arg, instance->block (), frame);

	/* !!keiths: This is a hack, but I don't want to write
	   yet another linkage name translation function.  At least
	   not just yet.  */
	value = instance->build_literal_expr (type, value_address (val));
      }
      break;

    default:
      gdb_assert_not_reached
	("unhandled template value argument symbol class");
    }

  return value;
}

/* Enumerate the template parameters of the generic form of the template
   definition DEFN into DEST.  */

static void
define_template_parameters_generic
  (compile_cplus_instance *instance, template_defn *defn,
   const struct template_argument_info *arg_info,
   const char *filename, int line)
{
  for (int i = 0; i < arg_info->n_arguments; ++i)
    {
      const char *id = SYMBOL_NATURAL_NAME (arg_info->arguments[i]);

      switch (arg_info->argument_kinds[i])
	{
	case type_parameter:
	  {
	    /* GDB doesn't support variadic templates yet.  */
	    int is_pack = 0;
	    gcc_type default_type = 0;

	    if (defn->default_argument (i) != NULL)
	      {
		struct type *type = SYMBOL_TYPE (defn->default_argument (i));

		/* This type must previously have been converted,
		   or GCC will error with "definition of TYPE inside
		   template parameter list."  */
		default_type = instance->convert_type (type);
	      }

	    gcc_type abstract_type
	      = instance->build_type_template_parameter (id, is_pack,
			default_type, filename, line);
	    defn->set_parameter_abstract_type (i, abstract_type);
	  }
	  break;

	case value_parameter:
	  {
	    gcc_expr default_value = 0;
	    struct type *ptype = SYMBOL_TYPE (arg_info->arguments[i]);

	    /* Get the argument's type.  This type must also have been
	     previously defined (or declared) to prevent errors.  */
	    gcc_type abstract_type = instance->convert_type (ptype);
	    defn->set_parameter_abstract_type (i, abstract_type);

	    if (defn->default_argument (i) != NULL)
	      {
		default_value
		  = get_template_argument_value (instance, abstract_type,
						 defn->default_argument (i));
	      }

	    instance->build_value_template_parameter (abstract_type, id,
						      default_value,
						      filename, line);
	  }
	  break;

	case template_parameter:
	  /* GDB doesn't support template-template parameters.  */
	  break;

	case variadic_parameter:
	  /* GDB doesn't support variadic templates.  */
	  break;

	default:
	  gdb_assert_not_reached ("unexpected template parameter kind");
	}
    }
}

/* Populate the `kinds' member of DEST from ARG_INFO.  */

static void
enumerate_template_parameter_kinds
  (compile_cplus_instance *instance, struct gcc_cp_template_args *dest,
   const struct template_argument_info *arg_info)
{
  for (int i = 0; i < arg_info->n_arguments; ++i)
    {
      switch (arg_info->argument_kinds[i])
	{
	case type_parameter:
	  dest->kinds[i] = GCC_CP_TPARG_CLASS;
	  break;
	case value_parameter:
	  dest->kinds[i] = GCC_CP_TPARG_VALUE;
	  break;
	case template_parameter:
	  dest->kinds[i] = GCC_CP_TPARG_TEMPL;
	  break;
	case variadic_parameter:
	  dest->kinds[i] = GCC_CP_TPARG_PACK;
	  break;
	default:
	  gdb_assert_not_reached ("unexpected template parameter kind");
	}
    }
}

/* See description in compile-cplus-templates.h.  */

void
compile_cplus_instance::enumerate_template_arguments
  (struct gcc_cp_template_args *dest, const template_defn *defn,
   const struct template_argument_info *arg_info)
{
  /* Fill in the parameter kinds.  */
  enumerate_template_parameter_kinds (this, dest, arg_info);

  /* Loop over the arguments, converting parameter types, values, etc
     into DEST.  */
  for (int i = 0; i < arg_info->n_arguments; ++i)
    {
      switch (arg_info->argument_kinds[i])
	{
	case type_parameter:
	  {
	    gcc_type type
	      = convert_type (SYMBOL_TYPE (arg_info->arguments[i]));

	    dest->elements[i].type = type;
	  }
	  break;

	case value_parameter:
	  {
	    gcc_type type = defn->parameter_abstract_type (i);

	    dest->elements[i].value
	      = get_template_argument_value (this, type,
					     arg_info->arguments[i]);
	  }
	  break;

	case template_parameter:
	  break;

	case variadic_parameter:
	  break;

	default:
	  gdb_assert_not_reached ("unexpected template parameter kind");
	}
    }
}

/* Define the type for all default template parameters for the template
   arguments given by ARGUMENTS.  */

static void
define_default_template_parameter_types
  (compile_cplus_instance *instance, template_defn *defn,
   const struct template_argument_info *arg_info)
{
  for (int i = 0; i < arg_info->n_arguments; ++i)
    {
      if (defn->default_argument (i) != NULL)
	{
	  switch (arg_info->argument_kinds[i])
	    {
	    case type_parameter:
	    case value_parameter:
	      instance->convert_type (SYMBOL_TYPE (defn->default_argument (i)));
	      break;

	    case template_parameter:
	    case variadic_parameter:
	    default:
	      gdb_assert (_("unexpected template parameter kind"));
	    }
	}
    }
}

/* A class to add type modifiers to a given compiler type.  */

class template_parameter_type_modifier_adder
{
public:
  template_parameter_type_modifier_adder
    (compile_cplus_instance *instance, gcc_type the_type)
      : m_instance (instance), m_flags (0), m_type (the_type)
  {
  }

  void operator() (template_parameter_modifier modifier)
  {
    switch (modifier)
      {
      case PARAMETER_NONE:
	break;

      case PARAMETER_CONST:
	m_flags |= GCC_CP_QUALIFIER_CONST;
	break;

      case PARAMETER_VOLATILE:
	m_flags |= GCC_CP_QUALIFIER_VOLATILE;
	break;

      case PARAMETER_RESTRICT:
	m_flags |= GCC_CP_QUALIFIER_RESTRICT;
	break;

      case PARAMETER_POINTER:
	m_type = convert_qualified_base (m_instance, m_type, m_flags);
	m_type = convert_pointer_base (m_instance, m_type);
	m_flags = (enum gcc_cp_qualifiers) 0;
	break;

      case PARAMETER_LVALUE_REFERENCE:
	m_type = convert_qualified_base (m_instance, m_type, m_flags);
	m_type = convert_pointer_base (m_instance, m_type);
	m_flags = (enum gcc_cp_qualifiers) 0;
	break;

      case PARAMETER_RVALUE_REFERENCE:
	m_type = convert_qualified_base (m_instance, m_type, m_flags);
	m_type = convert_reference_base (m_instance, m_type);
	m_flags = (enum gcc_cp_qualifiers) 0;
	break;

      default:
	gdb_assert_not_reached ("unknown template parameter modifier");
      }
  }

  /* Returns the modified type.  */

  gcc_type type () const
  {
    return m_type;
  }

private:
  /* The compiler instance into which to define the new type(s).  */
  compile_cplus_instance *m_instance;

  /* The qualifier flags.  */
  gcc_cp_qualifiers_flags m_flags;

  /* The type we are modifying.  */
  gcc_type m_type;
};

/* Add the modifiers given by MODIFIERS to TYPE.  */

static gcc_type
add_template_type_modifiers
  (compile_cplus_instance *instance, gcc_type type,
   const std::vector<template_parameter_modifier> &modifiers)
{
  template_parameter_type_modifier_adder adder (instance, type);

  for (auto &item : modifiers)
    adder (item);
  return adder.type ();
}

/* Add the type modifiers described in COMP to BASE_TYPE.  */

static gcc_type
add_type_modifiers (compile_cplus_instance *instance,
		    gcc_type base_type,
		    const struct demangle_component *comp)
{
  std::vector<template_parameter_modifier> modifiers;

  get_template_type (comp, modifiers);

  gcc_type result
    = add_template_type_modifiers (instance, base_type, modifiers);

  return result;
}

/* A struct to define (to the plug-in) and fill-in the
   function template definition based on the template instance in SLOT.
   CALL_DATA should be the compiler instance to use.  */

class function_template_definer
{
 public:

  function_template_definer (compile_cplus_instance *instance)
    : m_instance (instance)
  {
  }

  void operator() (function_template_defn *defn)
  {
    if (defn->defined ())
      {
	/* This template has already been defined.  Keep looking for more
	   undefined templates.  */
	return;
      }

    /* Ensure this is one-time operation.  */
    defn->set_defined (true);

    struct fn_field *method_field;
    struct type *method_type;
    const struct template_symbol *tsym = defn->template_symbol ();
    if (defn->parent_type () != NULL
	&& defn->fidx () != -1 && defn->midx () != -1)
      {
	struct fn_field *methods
	  = TYPE_FN_FIELDLIST1 (defn->parent_type (), defn->fidx ());

	method_field = &methods[defn->midx ()];
	method_type = method_field->type;
      }
    else
      {
	method_field = NULL;
	method_type = SYMBOL_TYPE (&tsym->base);
      }

    bool ignore;
    char *special_name = NULL;
    const char *id = defn->decl_name ();
    gdb_assert (!strchr (id, ':'));
    const char *name = maybe_canonicalize_special_function (id,
							    method_field,
							    method_type,
							    &special_name,
							    &ignore);

    /* Ignore any "ignore" -- we need the template defined even if
       this specific instance shouldn't emit a template.  */
    struct cleanup *back_to = make_cleanup (null_cleanup, NULL);

    if (special_name != NULL)
      {
	make_cleanup (xfree, special_name);
	name = special_name;
      }

    gcc_cp_symbol_kind_flags sym_kind = GCC_CP_SYMBOL_FUNCTION;

    if (name != id)
      sym_kind |= GCC_CP_FLAG_SPECIAL_FUNCTION;

    /* Define any default value types.  */
    define_default_template_parameter_types (m_instance, defn,
					     tsym->template_arguments);

    /* Assess the processing context.  */
    gcc_type result;
    compile_scope scope
      = m_instance->new_scope (SYMBOL_NATURAL_NAME (&tsym->base),
			       SYMBOL_TYPE (&tsym->base));

    if (scope.nested_type () != GCC_TYPE_NONE)
      {
	do_cleanups (back_to);
	/* new_scope returned the type of the actual template instance from
	   which we're constructing the template definition.  It is already
	   defined.  */
	return;
      }

    /* Start the new template declaration.  */
    m_instance->enter_scope (scope);
    m_instance->start_template_decl (defn->generic ().c_str ());

    /* Get the parameters' generic kinds and types.  */
    define_template_parameters_generic (m_instance, defn,
					tsym->template_arguments,
					symbol_symtab (&tsym->base)->filename,
					SYMBOL_LINE (&tsym->base));

    /* Find the function node describing this template function.  */
    gdb_assert (defn->demangle_info ()->tree->type
		== DEMANGLE_COMPONENT_TYPED_NAME);
    struct demangle_component *comp = d_right (defn->demangle_info ()->tree);

    gdb_assert (comp->type == DEMANGLE_COMPONENT_FUNCTION_TYPE);

    /* The return type is either a concrete type (TYPE_TARGET_TYPE)
       or a template parameter.  */
    gcc_type return_type;

    if (tsym->template_return_index != -1)
      {
	gcc_type param_type
	  = defn->parameter_abstract_type (tsym->template_return_index);

	return_type
	  = add_type_modifiers (m_instance, param_type, d_left (comp));
      }
    else if (tsym->conversion_operator_index != -1)
      {
	bool done = false;
	gcc_type param_type
	  = defn->parameter_abstract_type (tsym->conversion_operator_index);

	/* Conversion operators do not have a return type or arguments,
	   so we need to use the CONVERSION node in the left/name sub-tree
	   of the demangle tree.  */

	comp = d_left (defn->demangle_info ()->tree);
	while (!done)
	  {
	    switch (comp->type)
	      {
	      case DEMANGLE_COMPONENT_TEMPLATE:
		comp = d_left (comp);
		break;

	      case DEMANGLE_COMPONENT_QUAL_NAME:
		comp = d_right (comp);
		break;

	      case DEMANGLE_COMPONENT_CONVERSION:
	      default:
		done = true;
		break;
	      }
	  }

	/* We had better have found a CONVERSION node if
	   tsym->conversion_operator_index was set!  */
	gdb_assert (comp->type == DEMANGLE_COMPONENT_CONVERSION);
	return_type = add_type_modifiers (m_instance, param_type,
					  d_left (comp));
      }
    else
      {
	struct type *temp = TYPE_TARGET_TYPE (SYMBOL_TYPE (&tsym->base));

	return_type = m_instance->convert_type (temp);
      }

    /* Get the parameters' definitions, and put them into ARRAY.  */
    struct type *templ_type = SYMBOL_TYPE (&tsym->base);
    int is_varargs = is_varargs_p (templ_type);
    struct gcc_type_array array;

    array.n_elements = TYPE_NFIELDS (templ_type);
    array.elements = XNEWVEC (gcc_type, TYPE_NFIELDS (templ_type));
    make_cleanup (xfree, array.elements);

    int artificials = 0;

    /* d_right (info->tree) is FUNCTION_TYPE (assert above).  */
    comp = d_right (d_right (defn->demangle_info ()->tree));
    gdb_assert (comp != NULL && comp->type == DEMANGLE_COMPONENT_ARGLIST);

    for (int i = 0; i < TYPE_NFIELDS (templ_type); ++i)
      {
	if (TYPE_FIELD_ARTIFICIAL (templ_type, i))
	  {
	    --array.n_elements;
	    ++artificials;
	  }
	else
	  {
	    int tidx = tsym->template_argument_indices[i - artificials];
	    struct type *arg_type = TYPE_FIELD_TYPE (templ_type, i);

	    if (tidx == -1)
	      {
		/* The parameter's type is a concrete type.  */
		array.elements[i - artificials]
		  = m_instance->convert_type (arg_type);
	      }
	    else
	      {
		/* The parameter's type is a template parameter.  */
		gcc_type result = defn->parameter_abstract_type (tidx);

		array.elements[i - artificials]
		  = add_type_modifiers (m_instance, result, d_left (comp));
	      }

	    /* Move to the next ARGLIST node.  */
	    comp = d_right (comp);
	  }
      }

    gcc_type func_type = m_instance->build_function_type (return_type, &array,
							  is_varargs);

    /* If we have a method, create its type and set additional symbol flags
       for the compiler.  */
    if (defn->parent_type () != NULL
	&& defn->fidx () != -1 && defn->midx () != -1)
      {
	gcc_type class_type;
	struct fn_field *methods
	  = TYPE_FN_FIELDLIST1 (defn->parent_type (), defn->fidx ());

	/* Get the defining class's type.  This should already be in the
	   cache.  */
	class_type = m_instance->convert_type (defn->parent_type ());

	/* Add any virtuality flags.  */
	if (TYPE_FN_FIELD_VIRTUAL_P (methods, defn->midx ()))
	  {
	    sym_kind |= GCC_CP_FLAG_VIRTUAL_FUNCTION;

	    /* Unfortunate to have to do a symbol lookup, but this is the only
	       way to know if we have a pure virtual method.  */
	    struct block_symbol sym
	      = lookup_symbol (TYPE_FN_FIELD_PHYSNAME (methods, defn->midx ()),
			       m_instance->block (), VAR_DOMAIN, NULL);
	    if (sym.symbol == NULL)
	      {
		/* !!keiths: The pure virtual hack.  See
		   ccp_convert_struct_or_union_methods for more.  */
		sym_kind |= GCC_CP_FLAG_PURE_VIRTUAL_FUNCTION;
	      }
	  }

	/* Add access flags.  */
	sym_kind |= get_method_access_flag (defn->parent_type (),
					    defn->fidx (), defn->midx ());

	/* Create the method type.  */
	if (!TYPE_FN_FIELD_STATIC_P (methods, defn->midx ()))
	  {
	    gcc_cp_qualifiers_flags quals;
	    gcc_cp_ref_qualifiers_flags rquals;

	    quals = (enum gcc_cp_qualifiers) 0; /* !!keiths FIXME  */
	    rquals = GCC_CP_REF_QUAL_NONE; /* !!keiths FIXME  */
	    func_type
	      = m_instance->build_method_type (class_type, func_type, quals,
					       rquals);
	  }
      }

    /* Finally, define the new generic template declaration.  */
    gcc_decl decl
      = m_instance->build_decl ("function template", name, sym_kind,
				func_type, 0, 0,
				symbol_symtab (&(tsym->base))->filename,
				SYMBOL_LINE (&(tsym->base)));
    defn->set_decl (decl);

    m_instance->leave_scope ();
    do_cleanups (back_to);
  }

 private:

  /* The compiler instance to use.  */
  compile_cplus_instance *m_instance;
};

/* See description in compile-cplus-templates.h.  */

void
compile_cplus_instance::emit_function_template_decls ()
{
  function_template_definer definer (this);

  for (auto &item : *m_function_template_defns)
    definer (item.second.get ());
}

/* A class to define and fill-in class template definitions.  */

class class_template_definer
{
 public:

  class_template_definer (compile_cplus_instance *instance)
    : m_instance (instance)
  {
  }

  void operator() (class_template_defn *defn)
  {
    if (defn->defined ())
      {
	/* This template has already been defined.  Keep looking for more
	   undefined templates.  */
	return;
      }

    /* Make sure this is only done once!  */
    defn->set_defined (true);

    /* Define any default value types.  */
    const struct template_argument_info *arg_info
      = TYPE_TEMPLATE_ARGUMENT_INFO (defn->type ());

    define_default_template_parameter_types (m_instance, defn, arg_info);

    /* Create/push new scope.  */
    compile_scope scope
      = m_instance->new_scope (TYPE_NAME (defn->type ()), defn->type ());

    if (scope.nested_type () != GCC_TYPE_NONE)
      {
	/* new_processing_context returned the type of the actual template
	   instance from which we're constructing the template definition.
	   It is already defined.  */
	return;
      }
    m_instance->enter_scope (scope);

    /* Start a new template list for this template.  */
    m_instance->start_template_decl (defn->generic ().c_str ());

    /* Get the parameters' generic kinds and types.  */
    define_template_parameters_generic (m_instance, defn, arg_info,
					/* filename */ NULL, /*
					   !!keiths FIXME  */
					/* line */ 0
					/* !!keiths FIXME  */
					);


    /* Define the new generic template declaration.  */
    if (TYPE_CODE (defn->type ()) == TYPE_CODE_STRUCT)
      {
	gcc_decl decl
	  = m_instance->build_decl ("class template", defn->decl_name (),
				    GCC_CP_SYMBOL_CLASS /* | nested_access? */
				    | (TYPE_DECLARED_CLASS (defn->type ())
				       ? GCC_CP_FLAG_CLASS_NOFLAG
				       : GCC_CP_FLAG_CLASS_IS_STRUCT),
				  0, NULL, 0, /*filename*/ NULL, /*line*/ 0);

	defn->set_decl (decl);
       }
    else
      {
	gdb_assert (TYPE_CODE (defn->type ()) == TYPE_CODE_UNION);
	gcc_decl decl
	  = m_instance->build_decl ("union template", defn->decl_name (),
				    GCC_CP_SYMBOL_UNION /* | nested_access? */,
				    0, NULL, 0, /*fileanme*/NULL, /*line*/0);

	defn->set_decl (decl);
      }

    m_instance->leave_scope ();
  }

 private:

  /* The compiler instance to use.  */
  compile_cplus_instance *m_instance;
};

/* See description in compile-cplus-templates.h.  */

void
compile_cplus_instance::emit_class_template_decls ()
{
  class_template_definer definer (this);

  for (auto &item : *m_class_template_defns)
    definer (item.second.get ());
}

/* A command to test function_template_decl.  */

static void
print_template_defn_command (char *arg, int from_tty)
{

  char *demangled_name
    = gdb_demangle (arg, DMGL_ANSI | DMGL_PARAMS | DMGL_RET_DROP);

  if (demangled_name == NULL)
    {
      fprintf_filtered (gdb_stderr, _("could not demangle \"%s\"\n"), arg);
      return;
    }

  struct cleanup *back_to = make_cleanup (xfree, demangled_name);
  struct block_symbol symbol
    = lookup_symbol (demangled_name, NULL, VAR_DOMAIN, NULL);

  if (symbol.symbol == NULL)
    {
      fprintf_filtered (gdb_stderr, _("could not find symbol for \"%s\"\n"),
			arg);
      do_cleanups (back_to);
      return;
    }

  if (!SYMBOL_IS_CPLUS_TEMPLATE_FUNCTION (symbol.symbol))
    {
      fprintf_filtered (gdb_stderr, _("symbol \"%s\" does not represent a"
				      " template function\n"), arg);
      do_cleanups (back_to);
      return;
    }

  struct template_symbol *tsymbol = (struct template_symbol *) symbol.symbol;

  cp_decode_template_type_indices (tsymbol, NULL);

  std::unique_ptr<demangle_parse_info> info
    = cp_mangled_name_to_comp (arg, DMGL_ANSI | DMGL_PARAMS);
  std::string str = function_template_decl (tsymbol, info.get ());

  fprintf_filtered (gdb_stdout, "%s\n", str.c_str ());
  do_cleanups (back_to);
}

void _initialize_compile_cplus_templates ();

void
_initialize_compile_cplus_templates ()
{
  add_cmd ("tdef", class_maintenance, print_template_defn_command,
	   _("Print the template generic for the given linkage name."),
	   &maint_cplus_cmd_list);
}
