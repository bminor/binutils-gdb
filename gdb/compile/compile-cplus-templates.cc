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

#include "defs.h"
#include "compile-internal.h"
#include "compile-cplus.hh"
#include "cp-support.h"
#include "demangle.h"
#include "typeprint.h"
#include "c-lang.h"
#include "gdbcmd.h"

#include <algorithm>

/* Modifiers for abstract template parameters when used in template function
   declarations, including CV and ref qualifiers and pointer and reference
   type modifiers, e.g., const T*.  */

enum template_parameter_type_modifier
{
  /* The abstract parameter type is not qualified at all.  */
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

  /* The abstract parameter type was declared as rvalue reference, T&&.  */
  PARAMETER_RVALUE_REFERENCE
};
typedef enum template_parameter_type_modifier template_parameter_modifier;

// Forward declarations

static void print_template_parameter_list
  (struct ui_file *stream, const struct template_argument_info *arg_info);

static void print_template_type (const struct demangle_component *comp,
				 const struct template_symbol *tsymbol,
				 struct ui_file *stream);

static void print_conversion_node (const struct demangle_component *comp,
				   const struct template_symbol *tsymbol,
				   struct ui_file *stream);

static void print_function_template_arglist
  (const struct demangle_component *comp,
   const struct template_symbol *tsymbol, struct ui_file *stream);

// See description in compile-cplus-templates.hh.

void
compile::cplus::templates::function_template_defn::
destroy (compile::cplus::templates::function_template_map_item p)
{
  delete p.second;
}

// See description in compile-cplus-templates.hh.

compile::cplus::templates::
function_template_defn::
function_template_defn (std::string generic, parsed_demangle_info info,
			const struct template_symbol *tsymbol,
			struct type *parent_type, int fidx, int midx)
  : template_defn (compile::cplus::decl_name (tsymbol->search_name),
		   generic, tsymbol->template_arguments->n_arguments),
    m_tsymbol (tsymbol), m_parent_type (parent_type),
    m_fidx (fidx), m_midx (midx),
    m_demangle_info (info)
{
}

// See description in compile-cplus-templates.hh.

void
compile::cplus::templates::class_template_defn::
destroy (compile::cplus::templates::class_template_map_item p)
{
  delete p.second;
}

/* Return a string representing the template declaration for TSYMBOL.
   All template symbols deriving from the same source declaration should
   yield the same string representation.

   This string representation is of the generic form
   RETURN_TYPE QUALIFIED_NAME <parameter list>(argument list), with
   generic template parameters instead of any instanced type.

   For example, both "void foo<int> (int)" and "void foo<A> (A)" will
   return "T foo<typename T>(T)".

   The return result should be freed.  */

static char *
function_template_decl (const struct template_symbol *tsymbol,
			const struct demangle_parse_info *info)
{
  struct demangle_component *ret_comp;
  struct ui_file *stream;
  long length;
  struct cleanup *back_to;
  struct block_symbol symbol;
  char *str = NULL;

  gdb_assert (info != NULL);

  stream  = mem_fileopen ();
  back_to = make_cleanup_ui_file_delete (stream);

  ret_comp = info->tree;
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

	      c_print_type (return_type, "", stream, -1, 0,
			    &type_print_raw_options);
	    }
	  else
	    print_template_type (d_left (ret_comp), tsymbol, stream);
	  fputc_unfiltered (' ', stream);
	}

      /* Print the name of the template.  */
      if (tsymbol->conversion_operator_index != -1)
	print_conversion_node (info->tree, tsymbol, stream);
      else
	{
	  fputs_unfiltered (tsymbol->search_name, stream);
	  if (tsymbol->search_name[strlen (tsymbol->search_name) - 1]
	      == '<')
	    fputc_unfiltered (' ', stream);
	}

      /* Print out template (generic) arguments.  */
      fputc_unfiltered ('<', stream);
      print_template_parameter_list (stream, tsymbol->template_arguments);
      fputc_unfiltered ('>', stream);

      /* Print out function arguments.  */
      fputc_unfiltered ('(', stream);
      print_function_template_arglist (ret_comp, tsymbol, stream);
      fputc_unfiltered (')', stream);
    }

  str = ui_file_xstrdup (stream, &length);
  do_cleanups (back_to);
  return str;
}

/* Compute the generic used by the given function template definition.  */

static std::string
compute_function_template_generic (struct template_symbol *tsymbol,
				   const parsed_demangle_info &info)
{
  gdb_assert (info.tree () != NULL);

  /* Make sure template arguments have been decoded.  */
  cp_decode_template_type_indices (tsymbol, info.info ());

  /* Output the template generic.  */
  char *generic_c = function_template_decl (tsymbol, info.info ());
  std::string generic (generic_c);
  xfree (generic_c);
#if 0
  printf ("generic for function \"%s\" is \"%s\"\n", defn->decl_name (),
	  defn->generic ().c_str ());
#endif
  return generic;
}

/* If SYM is a template symbol whose generic we have not yet declared,
   add it to INSTANCE's list of template definitions and scan for default
   values.  */

static void
maybe_define_new_function_template (struct compile_cplus_instance *instance,
				    const struct symbol *sym,
				    struct type *parent_type,
				    int f_idx, int m_idx)
{
  if (sym != NULL && SYMBOL_IS_CPLUS_TEMPLATE_FUNCTION (sym))
    {
      struct template_symbol *tsym = (struct template_symbol *) sym;

      parsed_demangle_info info (tsym->linkage_name, DMGL_ANSI | DMGL_PARAMS);
      std::string generic (compute_function_template_generic (tsym, info));
      compile::cplus::templates::function_template_defn_map::iterator pos
	= instance->function_template_defns->find (generic);
      compile::cplus::templates::function_template_defn *defn;     
      if (pos == instance->function_template_defns->end ())
	{
	  // New template definition -- insert it into the cache.
	  // !!keiths: This invokes the copy ctor for parsed_demangle_info
	  // Is there a way to not do that and still maintain RAII??
	  defn
	    = new compile::cplus::templates::function_template_defn (generic,
								    info, tsym,
								    parent_type,
								    f_idx,
								    m_idx);
	  instance->function_template_defns->insert (std::make_pair (generic,
								     defn));
	}
      else
	{
	  // Use the existing definition.
	  defn = pos->second;
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

// See description in compile-cplus-templates.hh.

void
compile::cplus::templates::
define_templates (struct compile_cplus_instance *instance,
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
    maybe_define_new_function_template (instance, elt->symbol, NULL, -1, -1);

  /* !!keiths: From here on out, we MUST have all types declared or defined,
     otherwise GCC will give us "definition of TYPE in template parameter
     list."  */
  /* Create any new template definitions we encountered.  */
  compile::cplus::templates::emit_function_template_decls (instance);
  compile::cplus::templates::emit_class_template_decls (instance);
}

// See description in compile-cplus-templates.hh.

compile::cplus::templates::function_template_defn *
compile::cplus::templates::
find_function_template_defn (struct compile_cplus_instance *instance,
			     struct template_symbol *tsym)
{
  /* Some times the template has no no linkage name from the compiler.
     There's not much we can do in this case.  */
  if (tsym->linkage_name == NULL)
    return NULL;

  parsed_demangle_info info (tsym->linkage_name, DMGL_ANSI | DMGL_PARAMS);
  std::string generic (compute_function_template_generic (tsym, info));
  compile::cplus::templates::function_template_defn_map::iterator pos
    = instance->function_template_defns->find (generic);
  if (pos != instance->function_template_defns->end ())
    {
      /* A template generic for this was already defined.  */
      return pos->second;
    }

  /* No generic for this template was found.  */
  return NULL;
}

/* Compute the generic used by the given function template definition.  */

static std::string
compute_class_template_generic (std::string name, struct type *type)
{
  long length;

  struct ui_file *stream = mem_fileopen ();
  struct cleanup *back_to = make_cleanup_ui_file_delete (stream);

  /* class|struct|union NAME<parameters>  */
  if (TYPE_CODE (type) == TYPE_CODE_STRUCT)
    {
      if (TYPE_DECLARED_CLASS (type))
	fputs_unfiltered ("class ", stream);
      else
	fputs_unfiltered ("struct ", stream);
    }
  else
    {
      gdb_assert (TYPE_CODE (type) == TYPE_CODE_UNION);
      fputs_unfiltered ("union ", stream);
    }

  fputs_unfiltered (name.c_str (), stream);
  fputc_unfiltered ('<', stream);
  print_template_parameter_list (stream, TYPE_TEMPLATE_ARGUMENT_INFO (type));
  fputc_unfiltered ('>', stream);

  char *generic = ui_file_xstrdup (stream, &length);
  make_cleanup (xfree, generic);
  std::string string_generic (generic);
  do_cleanups (back_to);
#if 0
  printf ("generic for class \"%s\" is \"%s\"\n", name.c_str (), generic);
#endif
  return string_generic;
}

// See description in compile-cplus-templates.hh.

compile::cplus::templates::class_template_defn *
compile::cplus::templates::
find_class_template_defn (struct compile_cplus_instance *instance,
			  struct type *type)
{
  /* There are no template definitions associated with anonymous types or
     types without template arguments.  */
  if (TYPE_NAME (type) == NULL || TYPE_TEMPLATE_ARGUMENT_INFO (type) == NULL)
    return NULL;

  char *name = compile::cplus::decl_name (TYPE_NAME (type));
  struct cleanup *back_to = make_cleanup (xfree, name);

  std::string generic (compute_class_template_generic (name, type));
  compile::cplus::templates::class_template_defn_map::iterator pos
    = instance->class_template_defns->find (generic);
  if (pos != instance->class_template_defns->end ())
    {
      /* A template generic for this was already defined.  */
      do_cleanups (back_to);
      return pos->second;
    }

  /* No generic for this template was found.  */
  do_cleanups (back_to);
  return NULL;
}

// A class providing printing for a single parameter type modifier.

class one_template_type_modifier_printer
{
public:
  // Construct a new printer which outputs to STREAM.
  explicit one_template_type_modifier_printer (struct ui_file *stream)
    : m_stream (stream)
  {
  }

  // Unary function to output the modifier.
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
  // The stream to which to print the modifier.
  struct ui_file *m_stream;
};

// Print the type modifiers MODIFIERS to STREAM.

static void
print_template_type_modifiers
  (const std::vector<template_parameter_modifier> &modifiers,
   struct ui_file *stream)
{
  one_template_type_modifier_printer printer (stream);

  std::for_each (modifiers.begin (), modifiers.end (), printer);
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
print_template_parameter_list (struct ui_file *stream,
			       const struct template_argument_info *arg_info)
{
  int i;

  for (i = 0; i < arg_info->n_arguments; ++i)
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
  int done = 0;
  int quals = 0;
  int i, artificials;
  struct demangle_component *arg;
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

// See description in compile-cplus-templates.hh.

void
compile::cplus::templates::
maybe_define_new_class_template (struct compile_cplus_instance *instance,
				 struct type *type, const char *decl_name)
{
  if (TYPE_N_TEMPLATE_ARGUMENTS (type) == 0)
    return;

  std::string generic (compute_class_template_generic (decl_name, type));
  compile::cplus::templates::class_template_defn_map::iterator pos
    = instance->class_template_defns->find (generic);

  compile::cplus::templates::class_template_defn *defn = NULL;
  if (pos == instance->class_template_defns->end ())
    {
      // New template definition -- insert it into the cache.
      defn
	= new compile::cplus::templates::class_template_defn (decl_name,
							     generic, type);
      instance->class_template_defns->insert (std::make_pair (generic, defn));
    }
  else
    {
      /* Existing definition.  Use the existing definition.  */
      defn = pos->second;
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

// See description in compile-cplus-templates.hh.

void
compile::cplus::templates::
scan_type_for_function_templates (struct compile_cplus_instance *instance,
				  struct type *type)
{
  int i;

  for (i = 0; i < TYPE_NFN_FIELDS (type); ++i)
    {
      int j;
      struct fn_field *methods = TYPE_FN_FIELDLIST1 (type, i);

      for (j = 0; j < TYPE_FN_FIELDLIST_LENGTH (type, i); ++j)
	{
	  const struct block *block
	    = compile::cplus::get_current_search_block ();
	  struct block_symbol sym
	    = lookup_symbol (TYPE_FN_FIELD_PHYSNAME (methods, j),
			     block, VAR_DOMAIN, NULL);

	  maybe_define_new_function_template (instance, sym.symbol,
					      type, i, j);
	}
    }
}

/* Helper function to define and return the `value' of TYPE of the template
   parameter ARG in compile INSTANCE.  */

static gcc_expr
get_template_argument_value (struct compile_cplus_instance *instance,
			     gcc_type type, struct symbol *arg)
{
  gcc_expr value = 0;

  switch (SYMBOL_CLASS (arg))
    {
      /* !!keiths: More (incomplete) fun.  */
    case LOC_CONST:
      if (debug_compile_cplus_types)
	{
	  printf_unfiltered ("literal_expr %lld %ld\n",
			     type, SYMBOL_VALUE (arg));
	}
	value = CPCALL (literal_expr, instance, type, SYMBOL_VALUE (arg));
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
	val = read_var_value (arg, instance->base.block, frame);

	/* !!keiths: This is a hack, but I don't want to write
	   yet another linkage name translation function.  At least
	   not just yet.  */
	value = CPCALL (literal_expr, instance, type, value_address (val));
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
(struct compile_cplus_instance *instance,
 compile::cplus::templates::template_defn *defn,
   const struct template_argument_info *arg_info,
   const char *filename, int line)
{
  int i;

  for (i = 0; i < arg_info->n_arguments; ++i)
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
		default_type = convert_cplus_type (instance, type,
						   GCC_CP_ACCESS_NONE);
	      }

	    if (debug_compile_cplus_types)
	      {
		printf_unfiltered
		  ("new_template_typename_parm %s %d %lld %s %d\n",
		   id, is_pack, default_type, filename, line);
	      }

	    /* !!keiths: This would be the base generic parameter...
	       Shouldn't this carry any qualifiers with it?  Can we
	       actually figure out those qualifiers, e.g,
	    const T& foo<A> () vs T foo<const A&> ().  */
	    gcc_type abstract_type
	      = CPCALL (new_template_typename_parm, instance, id, is_pack,
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
	    gcc_type abstract_type
	      = convert_cplus_type (instance, ptype, GCC_CP_ACCESS_NONE);
	    defn->set_parameter_abstract_type (i, abstract_type);

	    if (defn->default_argument (i) != NULL)
	      {
		default_value
		  = get_template_argument_value (instance, abstract_type,
						 defn->default_argument (i));
	      }

	    if (debug_compile_cplus_types)
	      {
		printf_unfiltered
		  ("new_template_value_parm %lld %s %lld %s %d\n",
		   abstract_type, id, default_value, filename, line);
	      }

	    CPCALL (new_template_value_parm, instance, abstract_type, id,
			default_value, filename, line);
	  }
	  break;

	case template_parameter:
	  // GDB doesn't support template-template parameters.
	  break;

	case variadic_parameter:
	  // GDB doesn't support variadic templates.
	  break;

	default:
	  gdb_assert_not_reached ("unexpected template parameter kind");
	}
    }
}

/* Fill in the `kinds'  member of DEST from ARG_INFO.  */

static void
enumerate_template_parameter_kinds (struct compile_cplus_instance *instance,
				    struct gcc_cp_template_args *dest,
				    const struct template_argument_info *arg_info)
{
  int i;

  for (i = 0; i < arg_info->n_arguments; ++i)
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

// See description in compile-cplus-templates.hh.

void
compile::cplus::templates::
enumerate_template_arguments (struct compile_cplus_instance *instance,
			      struct gcc_cp_template_args *dest,
			      const compile::cplus::templates::template_defn *defn,
			      const struct template_argument_info *arg_info)
{
  int i;

  /* Fill in the parameter kinds.  */
  enumerate_template_parameter_kinds (instance, dest, arg_info);

  /* Loop over the arguments, converting parameter types, values, etc
     into DEST.  */
  for (i = 0; i < arg_info->n_arguments; ++i)
    {
      switch (arg_info->argument_kinds[i])
	{
	case type_parameter:
	  {
	    gcc_type type
	      = convert_cplus_type (instance,
				    SYMBOL_TYPE (arg_info->arguments[i]),
				    GCC_CP_ACCESS_NONE);

	    dest->elements[i].type = type;
	  }
	  break;

	case value_parameter:
	  {
	    gcc_type type = defn->parameter_abstract_type (i);

	    dest->elements[i].value
	      = get_template_argument_value (instance, type,
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
  (struct compile_cplus_instance *instance,
   compile::cplus::templates::template_defn *defn,
   const struct template_argument_info *arg_info)
{
  int i;

  for (i = 0; i < arg_info->n_arguments; ++i)
    {
      if (defn->default_argument (i) != NULL)
	{
	  switch (arg_info->argument_kinds[i])
	    {
	    case type_parameter:
	    case value_parameter:
	      convert_cplus_type (instance,
				  SYMBOL_TYPE (defn->default_argument (i)),
				  GCC_CP_ACCESS_NONE);
	      break;

	    case template_parameter:
	    case variadic_parameter:
	    default:
	      gdb_assert (_("unexpected template parameter kind"));
	    }
	}
    }
}

// A class to add type modifiers to a given compiler type.

class template_parameter_type_modifier_adder
{
public:
  template_parameter_type_modifier_adder
    (struct compile_cplus_instance *instance, gcc_type the_type)
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
	m_type = compile::cplus::convert_qualified_base (m_instance,
							 m_type, m_flags);
	m_type = compile::cplus::convert_pointer_base (m_instance, m_type);
	m_flags = (enum gcc_cp_qualifiers) 0;
	break;

      case PARAMETER_LVALUE_REFERENCE:
	m_type = compile::cplus::convert_qualified_base (m_instance,
							 m_type, m_flags);
	m_type = compile::cplus::convert_pointer_base (m_instance, m_type);
	m_flags = (enum gcc_cp_qualifiers) 0;
	break;

      case PARAMETER_RVALUE_REFERENCE:
	m_type = compile::cplus::convert_qualified_base (m_instance,
							 m_type, m_flags);
	m_type = compile::cplus::convert_reference_base (m_instance, m_type);
	m_flags = (enum gcc_cp_qualifiers) 0;
	break;

      default:
	gdb_assert_not_reached ("unknown template parameter modifier");
      }
  }

  // Returns the modified type.

  gcc_type
  type () const
  {
    return m_type;
  }

private:
  // The compiler instance into which to elide the new type(s).
  struct compile_cplus_instance *m_instance;

  // The qualifier flags.
  gcc_cp_qualifiers_flags m_flags;

  // The type we are modifying.
  gcc_type m_type;
};

/* Add the modifiers given by MODIFIERS to TYPE.  */

static gcc_type
add_template_type_modifiers
  (struct compile_cplus_instance *instance, gcc_type type,
   const std::vector<template_parameter_modifier> &modifiers)
{
  template_parameter_type_modifier_adder adder (instance, type);

  adder = std::for_each (modifiers.begin (), modifiers.end (), adder);
  return adder.type ();
}

/* Add the type modifiers described in COMP to BASE_TYPE.  */

static gcc_type
add_type_modifiers (struct compile_cplus_instance *instance,
		    gcc_type base_type,
		    const struct demangle_component *comp)
{
  gcc_type result;
  std::vector<template_parameter_modifier> modifiers;

  get_template_type (comp, modifiers);
  result = add_template_type_modifiers (instance, base_type, modifiers);
  return result;
}

/* A hashtable callback to define (to the plug-in) and fill-in the
   function template definition based on the template instance in SLOT.
   CALL_DATA should be the compiler instance to use.  */

/* !!keiths: This is heinously long!  */

static void
define_function_template (struct compile_cplus_instance *instance,
			  compile::cplus::templates::function_template_defn *defn)
{
  if (defn->defined ())
    {
      /* This template has already been defined.  Keep looking for more
	 undefined templates.  */
      return;
    }

  // Make sure we only do this once.
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

  // Define any default value types.
  define_default_template_parameter_types (instance, defn,
					   tsym->template_arguments);

  // Assess the processing context.
  gcc_type result;
  struct compile_cplus_context *pctx
    = new_processing_context (instance, SYMBOL_NATURAL_NAME (&tsym->base),
			      SYMBOL_TYPE (&tsym->base), &result);
  if (result != GCC_TYPE_NONE)
    {
      gdb_assert (pctx == NULL);
      do_cleanups (back_to);
      /* new_processing_context returned the type of the actual template
	 instance from which we're constructing the template definition.
	 It is already defined.  */
      return;
    }

  /* If we need a new context, push it.  */
  int need_new_context = ccp_need_new_context (pctx);
  if (need_new_context)
    ccp_push_processing_context (instance, pctx);

  if (debug_compile_cplus_types)
    {
      printf_unfiltered ("start_new_template_decl for function generic %s\n",
			 defn->generic ().c_str ());
    }
  CPCALL (start_new_template_decl, instance);

  // Get the parameters' generic kinds and types.
  define_template_parameters_generic (instance, defn,
				      tsym->template_arguments,
				      symbol_symtab (&tsym->base)->filename,
				      SYMBOL_LINE (&tsym->base));

  /* Find the function node describing this template function.  */
  gdb_assert (defn->demangle_info ()->tree ()->type
	      == DEMANGLE_COMPONENT_TYPED_NAME);
  struct demangle_component *comp = d_right (defn->demangle_info ()->tree ());
  gdb_assert (comp->type == DEMANGLE_COMPONENT_FUNCTION_TYPE);

  /* The return type is either a concrete type (TYPE_TARGET_TYPE)
     or a template parameter.  */
  gcc_type return_type;
  if (tsym->template_return_index != -1)
    {
      gcc_type param_type
	= defn->parameter_abstract_type (tsym->template_return_index);

      return_type
	= add_type_modifiers (instance, param_type, d_left (comp));
    }
  else if (tsym->conversion_operator_index != -1)
    {
      bool done = false;
      gcc_type param_type
	= defn->parameter_abstract_type (tsym->conversion_operator_index);

      /* Conversion operators do not have a return type or arguments,
	 so we need to use the CONVERSION node in the left/name sub-tree
	 of the demangle tree.  */

      comp = d_left (defn->demangle_info ()->tree ());
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
      return_type = add_type_modifiers (instance, param_type, d_left (comp));
    }
  else
    {
      return_type
	= convert_cplus_type (instance,
			      TYPE_TARGET_TYPE (SYMBOL_TYPE (&tsym->base)),
			      GCC_CP_ACCESS_NONE);
    }

  /* Get the parameters' definitions, and put them into ARRAY.  */
  struct type *templ_type = SYMBOL_TYPE (&tsym->base);
  int is_varargs = compile::cplus::is_varargs_p (templ_type);
  struct gcc_type_array array;
  array.n_elements = TYPE_NFIELDS (templ_type);
  array.elements = XNEWVEC (gcc_type, TYPE_NFIELDS (templ_type));
  make_cleanup (xfree, array.elements);
  int artificials = 0;

  /* d_right (info->tree) is FUNCTION_TYPE (assert above).  */
  comp = d_right (d_right (defn->demangle_info ()->tree ()));
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
	      // The parameter's type is a concrete type.
	      array.elements[i - artificials]
		= convert_cplus_type (instance, arg_type, GCC_CP_ACCESS_NONE);
	    }
	  else
	    {
	      // The parameter's type is a template parameter.
	      result = defn->parameter_abstract_type (tidx);

	      array.elements[i - artificials]
		= add_type_modifiers (instance, result, d_left (comp));
	    }

	  /* Move to the next ARGLIST node.  */
	  comp = d_right (comp);
	}
    }

  if (debug_compile_cplus_types)
    {
      printf_unfiltered ("build_function_type for template %s %lld %d\n",
			 defn->generic ().c_str (), return_type, is_varargs);
    }
  gcc_type func_type = CPCALL (build_function_type, instance, return_type,
			       &array, is_varargs);

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
      class_type = convert_cplus_type (instance, defn->parent_type (),
				       GCC_CP_ACCESS_NONE);

      /* Add any virtuality flags.  */
      if (TYPE_FN_FIELD_VIRTUAL_P (methods, defn->midx ()))
	{
	  sym_kind |= GCC_CP_FLAG_VIRTUAL_FUNCTION;

	  /* Unfortunate to have to do a symbol lookup, but this is the only
	     way to know if we have a pure virtual method.  */
	  const struct block *block
	    = compile::cplus::get_current_search_block ();
	  struct block_symbol sym
	    = lookup_symbol (TYPE_FN_FIELD_PHYSNAME (methods, defn->midx ()),
			     block, VAR_DOMAIN, NULL);
	  if (sym.symbol == NULL)
	    {
	      /* !!keiths: The pure virtual hack.  See
		 ccp_convert_struct_or_union_methods for more.  */
	      sym_kind |= GCC_CP_FLAG_PURE_VIRTUAL_FUNCTION;
	    }
	}

      /* Add access flags.  */
      sym_kind |= compile::cplus::get_method_access_flag (defn->parent_type (),
							 defn->fidx (),
							 defn->midx ());

      /* Create the method type.  */
      if (!TYPE_FN_FIELD_STATIC_P (methods, defn->midx ()))
	{
	  gcc_cp_qualifiers_flags quals;
	  gcc_cp_ref_qualifiers_flags rquals;

	  quals = (enum gcc_cp_qualifiers) 0; // !!keiths FIXME
	  rquals = GCC_CP_REF_QUAL_NONE; // !!keiths FIXME
	  if (debug_compile_cplus_types)
	    {
	      printf_unfiltered ("build_method_type for template %s\n",
				 defn->generic ().c_str ());
	    }
	  func_type = CPCALL (build_method_type, instance, class_type,
			      func_type, quals, rquals);
	}
    }

  /* Finally, define the new generic template declaration.  */
  if (debug_compile_cplus_types)
    printf_unfiltered ("new_decl function template defn %s\n", name);
  defn->set_decl (CPCALL (new_decl, instance,
			  name,
			  sym_kind,
			  func_type,
			  0,
			  0,
			  symbol_symtab (&(tsym->base))->filename,
			  SYMBOL_LINE (&(tsym->base))));

  /* Pop the processing context, if we pushed one, and then delete it.  */
  if (need_new_context)
    ccp_pop_processing_context (instance, pctx);
  delete_processing_context (pctx);
  do_cleanups (back_to);
}

// See description in compile-cplus-templates.hh.

void
compile::cplus::templates::
emit_function_template_decls (struct compile_cplus_instance *instance)
{
  compile::cplus::templates::function_template_defn_map::iterator pos;

  for (pos = instance->function_template_defns->begin ();
       pos != instance->function_template_defns->end (); ++pos)
    define_function_template (instance, pos->second);
}

/* Define and fill-in the class template definition based on the
   template DEFN.  */

static void
define_class_template (struct compile_cplus_instance *instance,
		       compile::cplus::templates::class_template_defn *defn)
{
  if (defn->defined ())
    {
      /* This template has already been defined.  Keep looking for more
	 undefined templates.  */
      return;
    }

  // Make sure this is only done once!
  defn->set_defined (true);

  // Define any default value types.
  const struct template_argument_info *arg_info
    = TYPE_TEMPLATE_ARGUMENT_INFO (defn->type ());
  define_default_template_parameter_types (instance, defn, arg_info);

  // Asses the processing context.
  gcc_type result;
  struct compile_cplus_context *pctx
    = new_processing_context (instance, defn->decl_name (), defn->type (),
			      &result);
  if (result != GCC_TYPE_NONE)
    {
      gdb_assert (pctx == NULL);
      /* new_processing_context returned the type of the actual template
	 instance from which we're constructing the template definition.
	 It is already defined.  */
      return;
    }

  // If we need a new context, push it.
  int need_new_context = ccp_need_new_context (pctx);
  if (need_new_context)
    ccp_push_processing_context (instance, pctx);

  // Now start a new template list for this template.
  if (debug_compile_cplus_types)
    {
      printf_unfiltered ("start_new_template_decl for class generic %s\n",
			 defn->generic ().c_str ());
    }
  CPCALL (start_new_template_decl, instance);

  // Get the parameters' generic kinds and types.
  define_template_parameters_generic (instance, defn, arg_info,
				      /* filename */ NULL, // !!keiths FIXME
				      /* line */ 0); // !!keiths FIXME

  /* Define the new generic template declaration.  */
  if (TYPE_CODE (defn->type ()) == TYPE_CODE_STRUCT)
    {
      if (debug_compile_cplus_types)
	{
	  printf_unfiltered ("new_decl for class template defn %s\n",
			     defn->decl_name ());
	}
      defn->set_decl (CPCALL (new_decl, instance,
			      defn->decl_name (),
			      GCC_CP_SYMBOL_CLASS /* | nested_access? */
			      | (TYPE_DECLARED_CLASS (defn->type ())
				 ? GCC_CP_FLAG_CLASS_NOFLAG
				 : GCC_CP_FLAG_CLASS_IS_STRUCT),
			      0, NULL, 0, /*filename*/ NULL, /*line*/ 0));
    }
  else
    {
      gdb_assert (TYPE_CODE (defn->type ()) == TYPE_CODE_UNION);
      if (debug_compile_cplus_types)
	{
	  printf_unfiltered ("new_decl for union template defn %s\n",
			     defn->decl_name ());
	}
      defn->set_decl (CPCALL (new_decl, instance,
			      defn->decl_name (),
			      GCC_CP_SYMBOL_UNION /* | nested_access? */,
			      0, NULL, 0, /*fileanme*/NULL, /*line*/0));
    }
  if (debug_compile_cplus_types)
    printf_unfiltered ("\tgcc_decl = %lld\n", defn->decl ());

  if (need_new_context)
    ccp_pop_processing_context (instance, pctx);
  delete_processing_context (pctx);
}

// See description in compile-cplus-templates.hh.

void
compile::cplus::templates::
emit_class_template_decls (struct compile_cplus_instance *instance)
{
  compile::cplus::templates::class_template_defn_map::iterator pos;

  for (pos = instance->class_template_defns->begin ();
       pos != instance->class_template_defns->end (); ++pos)
    define_class_template (instance, pos->second);
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
  parsed_demangle_info info (arg, DMGL_ANSI | DMGL_PARAMS);
  char *str = function_template_decl (tsymbol, info.info ());
  make_cleanup (xfree, str);
  fprintf_filtered (gdb_stdout, "%s\n", str);
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
