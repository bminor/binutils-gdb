/* Convert types from GDB to GCC

   Copyright (C) 2014-2017 Free Software Foundation, Inc.

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
#include "compile-cplus.h"
#include "gdb_assert.h"
#include "symtab.h"
#include "source.h"
#include "cp-support.h"
#include "cp-abi.h"
#include "symtab.h"
#include "objfiles.h"
#include "block.h"
#include "gdbcmd.h"
#include "c-lang.h"
#include "compile-c.h" 		/* Included for c_get_range_decl_name
				   et al.  */

#include <algorithm>

using namespace compile;

/* Define to enable debugging for ctor/dtor definitions during
   type conversion.  */

#define DEBUG_XTOR 0

/* A "type" to indicate that convert_type should not attempt to
   cache its resultant type.  This is used, for example, when defining
   namespaces for the oracle.  */

#define DONT_CACHE_TYPE ((gcc_type) 0)

/* Flag to enable internal debugging.  */

static int debug_compile_cplus_types = 0;

/* Flag to enable internal scope switching debugging.  */

static int debug_compile_cplus_scopes = 0;

/* Forward declarations.  */

static gcc_type ccp_convert_func (compile_cplus_instance *instance,
				  struct type *type, int strip_artificial);

/* See description in compile-cplus.h.  */

char *
compile::decl_name (const char *natural)
{
  char *name = NULL;

  if (natural != NULL)
    {
      char *stripped;

      /* !!keiths: FIXME: Write a new parser func to do this?  */
      name = cp_func_name (natural);
      if (name == NULL)
	{
	  stripped = cp_strip_template_parameters (natural);
	  if (stripped != NULL)
	    return stripped;

	  name = xstrdup (natural);
	}
      else
	{
	  stripped = cp_strip_template_parameters (name);
	  if (stripped != NULL)
	    {
	      xfree (name);
	      return stripped;
	    }
	}
    }

  return name;
}

/* See description in compile-cplus.h.  */

int
compile::is_varargs_p (const struct type *type)
{
  /* !!keiths: This doesn't always work, unfortunately.  When we have a
     pure virtual method, TYPE_PROTOTYPED == 0.
     But this *may* be needed for several gdb.compile tests.  Or at least
     indicate other unresolved bugs in this file or elsewhere in gdb.  */
  return TYPE_VARARGS (type) /*|| !TYPE_PROTOTYPED (type)*/;
}

/* Get the access flag for the NUM'th field of TYPE.  */

static enum gcc_cp_symbol_kind
get_field_access_flag (const struct type *type, int num)
{
  if (TYPE_FIELD_PROTECTED (type, num))
    return GCC_CP_ACCESS_PROTECTED;
  else if (TYPE_FIELD_PRIVATE (type, num))
    return GCC_CP_ACCESS_PRIVATE;

  /* GDB assumes everything else is public.  */
  return GCC_CP_ACCESS_PUBLIC;
}

/* Get the access flag for the NUM'th method of TYPE's FNI'th
   fieldlist.  */

enum gcc_cp_symbol_kind
compile::get_method_access_flag (const struct type *type, int fni, int num)
{
  const struct fn_field *methods;

  gdb_assert (TYPE_CODE (type) == TYPE_CODE_STRUCT);

  /* If this type was not declared a class, everything is public.  */
  if (!TYPE_DECLARED_CLASS (type))
    return GCC_CP_ACCESS_PUBLIC;

  /* Otherwise, read accessibility from the fn_field.  */
  methods = TYPE_FN_FIELDLIST1 (type, fni);
  if (TYPE_FN_FIELD_PUBLIC (methods, num))
    return GCC_CP_ACCESS_PUBLIC;
  else if (TYPE_FN_FIELD_PROTECTED (methods, num))
    return GCC_CP_ACCESS_PROTECTED;
  else if (TYPE_FN_FIELD_PRIVATE (methods, num))
    return GCC_CP_ACCESS_PRIVATE;

  gdb_assert_not_reached ("unhandled method access specifier");
}

/* A useful debugging function to output the scope SCOPE
   to stdout.  */

static void __attribute__ ((used))
debug_print_scope (const compile_scope &scope)
{
  int i;

  for (const auto &comp: scope)
    {
      std::string symbol = (comp.bsymbol.symbol != NULL
			    ? SYMBOL_NATURAL_NAME (comp.bsymbol.symbol)
			    : "<none>");

      printf ("\tname = %s, symbol = %s\n", comp.name.c_str (),
	      symbol.c_str ());
    }
}

/* Utility function to convert CODE into a string.  */

static const char *
type_code_to_string (enum type_code code)
{
  const char * const s[] =
    {"BISTRING (deprecated)", "UNDEF (not used)",
      "PTR", "ARRAY", "STRUCT", "UNION", "ENUM",
      "FLAGS", "FUNC", "INT", "FLT", "VOID",
      "SET", "RANGE", "STRING", "ERROR", "METHOD",
      "METHODPTR", "MEMBERPTR", "REF", "CHAR", "BOOL",
      "COMPLEX", "TYPEDEF", "NAMESPACE", "DECFLOAT", "MODULE",
     "INTERNAL_FUNCTION", "XMETHOD"};

  return s[code + 1];
}

/* See description in compile-cplus.h.  */

compile_scope
compile::type_name_to_scope (const char *type_name, const struct block *block)
{
  compile_scope scope;

  if (type_name == NULL)
    {
      /* An anonymous type.  We cannot really do much here.  We simply cannot
	 look up anonymous types easily/at all.  */
      return scope;
    }

  const char *p = type_name;
  std::string lookup_name;

  while (1)
    {
      /* Create a string token of the first component of TYPE_NAME.  */
      int len = cp_find_first_component (p);
      std::string s (p, len);

      /* Advance past the last token.  */
      p += len;

      /* Look up the symbol and decide when to stop.  */
      if (!lookup_name.empty ())
	lookup_name += "::";
      lookup_name += s;

      /* Look up the resulting name.  */
      struct block_symbol bsymbol
	= lookup_symbol (lookup_name.c_str (), block, VAR_DOMAIN, NULL);

      if (bsymbol.symbol != NULL)
	{
	  scope_component comp = {s, bsymbol};

	  scope.push_back (comp);

	  if (TYPE_CODE (SYMBOL_TYPE (bsymbol.symbol)) != TYPE_CODE_NAMESPACE)
	    {
	      /* We're done.  */
	      break;
	    }
	}

      if (*p == ':')
	{
	  ++p;
	  if (*p == ':')
	    ++p;
	  else
	    {
	      /* This shouldn't happen since we are not attempting to
		 loop over user input.  This name is generated by GDB
		 from debug info.  */
	      internal_error (__FILE__, __LINE__,
			      _("malformed TYPE_NAME during parsing"));
	    }
	}
      if (p[0] == '\0')
	break;
    }

  return scope;
}

/* Compare two scope_components for equality.  These are equal if the names
   of the two components' are the same.  */

bool
compile::operator== (const scope_component &lhs, const scope_component &rhs)
{
  return lhs.name == rhs.name;
}

/* Compare two scope_components for inequality.  These are not equal if
   the two components' names are not equal.  */

bool
compile::operator!= (const scope_component &lhs, const scope_component &rhs)
{
  return lhs.name != rhs.name;
}

/* Compare two compile_scopes for equality.  These are equal if they are both
   contain the same number of components and each component is equal.  */

bool
compile::operator== (const compile_scope &lhs, const compile_scope &rhs)
{
  if (lhs.size () != rhs.size ())
    return false;

  for (int i = 0; i < lhs.size (); ++i)
    {
      if (lhs[i] != rhs[i])
	return false;
    }

  return true;
}

/* Compare two compile_scopes for inequality.  These are inequal if they
   contain unequal number of elements or if any of the components are not
   the same.  */

bool
compile::operator!= (const compile_scope &lhs, const compile_scope &rhs)
{
  if (lhs.size () != rhs.size ())
    return true;

  for (int i = 0; i < lhs.size (); ++i)
    {
      if (lhs[i] != rhs[i])
	return true;
    }

  return false;
}

/* See description in compile-cplus.h.  */

void
compile_cplus_instance::enter_scope (compile_scope &new_scope)
{
  bool must_push = m_scopes.empty () || m_scopes.back () != new_scope;

  new_scope.m_pushed = must_push;

  /* Save the new scope.  */
  m_scopes.push_back (new_scope);

  if (must_push)
    {
      if (debug_compile_cplus_scopes)
	printf_unfiltered ("entering new scope %p\n", new_scope);

      /* Push the global namespace. */
      push_namespace ("");

      /* Push all other namespaces.  Note that we do not push the last
	 scope_component -- that's the actual type we are converting.  */
      std::for_each
	(new_scope.begin (), new_scope.end () - 1, [this] (const auto &comp)
	 {
	  gdb_assert (TYPE_CODE (SYMBOL_TYPE (comp.bsymbol.symbol))
		      == TYPE_CODE_NAMESPACE);

	  const char *ns = (comp.name == CP_ANONYMOUS_NAMESPACE_STR ? NULL
			    : comp.name.c_str ());

	  this->push_namespace (ns);
	 });
    }
  else
    {
      if (debug_compile_cplus_scopes)
	{
	  printf_unfiltered
	    ("staying in current scope -- scopes are identical\n");
	}
    }
}

/* See description in compile-cplus.h.  */

void
compile_cplus_instance::leave_scope ()
{
  /* Get the current scope and remove it from the internal list of
     scopes.  */
  compile_scope current = m_scopes.back ();

  m_scopes.pop_back ();

  if (current.m_pushed)
    {
      if (debug_compile_cplus_scopes)
	printf_unfiltered ("leaving scope %p\n", current);

      /* Pop namespaces.  Do not push the last scope_component -- that's
	 the type we are converting, not a namespace.  */
      std::for_each
	(current.begin (),current.end () - 1, [this] (const auto &comp) {
	  gdb_assert (TYPE_CODE (SYMBOL_TYPE (comp.bsymbol.symbol))
		      == TYPE_CODE_NAMESPACE);
	  this->pop_binding_level (comp.name.c_str ());
	});

      /* Pop global namespace.  */
      pop_binding_level ("");
    }
  else
    {
      if (debug_compile_cplus_scopes)
	printf_unfiltered ("identical scopes -- not leaving scope\n");
    }
}

/*See description in compile-cplus.h.  */

compile_scope
compile_cplus_instance::new_scope (const char *type_name, struct type *type)
{
  /* Break the type name into components.  If TYPE was defined in some
     superclass, we do not process TYPE but process the enclosing type
     instead.  */
  compile_scope scope = type_name_to_scope (type_name, block ());

  if (!scope.empty ())
    {
      /* Get the name of the last component, which should be the
	 unqualified name of the type to process.  */
      scope_component &comp = scope.back ();

      if (!types_equal (type, SYMBOL_TYPE (comp.bsymbol.symbol))
	  && (m_scopes.empty ()
	      || (m_scopes.back ().back ().bsymbol.symbol
		  != comp.bsymbol.symbol)))
	{
	  /* The type is defined inside another class(es).  Convert that
	     type instead of defining this type.  */
	  convert_type (SYMBOL_TYPE (comp.bsymbol.symbol));

	  /* If the original type (passed in to us) is defined in a nested
	     class, the previous call will give us that type's gcc_type.
	     Upper layers are expecting to get the original type's
	     gcc_type!  */
	  type_map_t::iterator pos = m_type_map.find (type);

	  gdb_assert (pos != m_type_map.end ());
	  scope.m_nested_type = pos->second;
	  return scope;
	}
    }
  else
    {
      if (TYPE_NAME (type) == NULL)
	{
	  /* Anonymous type  */

	  /* We don't have a qualified name for this to look up, but
	     we need a scope.  We have to assume, then, that it is the same
	     as the current scope, if any.  */
	  if (!m_scopes.empty ())
	    {
	      scope = m_scopes.back ();
	      scope.m_pushed = false;
	    }
	  else
	    scope.push_back (scope_component ());
	}
      else
	{
	  scope_component comp;

	  comp.bsymbol
	    = lookup_symbol (TYPE_NAME (type), block (), VAR_DOMAIN, NULL);

	  char *name = cp_func_name (TYPE_NAME (type));

	  comp.name = name;
	  xfree (name);
	  scope.push_back (comp);
	}
    }

  /* Ensure least one component in the scope.  */
  gdb_assert (scope.size () > 0);
  return scope;
}

/* !!keiths: not RVALUE REFERENCES!  */

gcc_type
compile::convert_reference_base (compile_cplus_instance *instance,
				 gcc_type base)
{
  return instance->build_reference_type (base, GCC_CP_REF_QUAL_LVALUE);
}

/* Convert a reference type to its gcc representation.  */

static gcc_type
ccp_convert_reference (compile_cplus_instance *instance,
		       struct type *type)
{
  gcc_type target = instance->convert_type (TYPE_TARGET_TYPE (type));

  /* !!keiths: GDB does not currently do anything with rvalue references.
     [Except set the type code to TYPE_CODE_ERROR!  */
  return convert_reference_base (instance, target);
}

/* See description in compile-cplus.h.  */

gcc_type
compile::convert_pointer_base (compile_cplus_instance *instance,
			       gcc_type target)
{
  return instance->build_pointer_type (target);
}

/* Convert a pointer type to its gcc representation.  */

static gcc_type
ccp_convert_pointer (compile_cplus_instance *instance,
		     struct type *type)
{
  gcc_type target = instance->convert_type (TYPE_TARGET_TYPE (type));

  return convert_pointer_base (instance, target);
}

/* Convert an array type to its gcc representation.  */

static gcc_type
ccp_convert_array (compile_cplus_instance *instance, struct type *type)
{
  struct type *range = TYPE_INDEX_TYPE (type);
  gcc_type element_type = instance->convert_type (TYPE_TARGET_TYPE (type));

  if (TYPE_LOW_BOUND_KIND (range) != PROP_CONST)
    {
      const char *s = _("array type with non-constant"
			" lower bound is not supported");

      return instance->error (s);
    }

  if (TYPE_LOW_BOUND (range) != 0)
    {
      const char *s = _("cannot convert array type with "
			"non-zero lower bound to C");

      return instance->error (s);
    }

  if (TYPE_HIGH_BOUND_KIND (range) == PROP_LOCEXPR
      || TYPE_HIGH_BOUND_KIND (range) == PROP_LOCLIST)
    {
      gcc_type result;
      char *upper_bound;

      if (TYPE_VECTOR (type))
	{
	  const char *s = _("variably-sized vector type is not supported");

	  return instance->error (s);
	}

      upper_bound = c_get_range_decl_name (&TYPE_RANGE_DATA (range)->high);
      result = instance->build_vla_array_type (element_type, upper_bound);
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
	return instance->build_vector_type (element_type, count);

      return instance->build_array_type (element_type, count);
    }
}

/* Convert a typedef of TYPE.  If not GCC_CP_ACCESS_NONE, NESTED_ACCESS
   will define the accessibility of the typedef definition in its
   containing class.  */

static gcc_type
ccp_convert_typedef (compile_cplus_instance *instance,
		     struct type *type, enum gcc_cp_symbol_kind nested_access)
{
  compile_scope scope = instance->new_scope (TYPE_NAME (type), type);

  if (scope.nested_type () != GCC_TYPE_NONE)
    return scope.nested_type ();

  char *name = NULL;
  struct cleanup *cleanups = make_cleanup (free_current_contents, &name);

  if (TYPE_NAME (type) != NULL)
    name = cp_func_name (TYPE_NAME (type));

  /* Make sure the scope for this type has been pushed.  */
  instance->enter_scope (scope);

  /* Convert the typedef's real type.  */
  gcc_type typedef_type = instance->convert_type (check_typedef (type));

  instance->build_decl ("typedef", name,
			GCC_CP_SYMBOL_TYPEDEF | nested_access,
			typedef_type,
			0, 0,
			/* !!keiths: Wow. More of this!  */
			NULL, 0);

  /* Completed this scope.  */
  instance->leave_scope ();
  do_cleanups (cleanups);
  return typedef_type;
}

/* Convert types defined in TYPE.  */

static void
ccp_convert_type_defns (compile_cplus_instance *instance, struct type *type)
{
  int i;
  enum gcc_cp_symbol_kind accessibility;

  /* Convert typedefs.  */
  for (i = 0; i < TYPE_TYPEDEF_FIELD_COUNT (type); ++i)
    {
      if (TYPE_TYPEDEF_FIELD_PUBLIC (type, i))
	accessibility = GCC_CP_ACCESS_PUBLIC;
      else if (TYPE_TYPEDEF_FIELD_PROTECTED (type, i))
	accessibility = GCC_CP_ACCESS_PROTECTED;
      else if (TYPE_TYPEDEF_FIELD_PRIVATE (type, i))
	accessibility = GCC_CP_ACCESS_PRIVATE;
      else
	gdb_assert_not_reached ("unknown accessibility");
      instance->convert_type (TYPE_TYPEDEF_FIELD_TYPE (type, i), accessibility);
    }

  /* Convert nested types.  */
  for (i = 0; i < TYPE_NESTED_TYPES_COUNT (type); ++i)
    {
      if (TYPE_NESTED_TYPES_FIELD_PUBLIC (type, i))
	accessibility = GCC_CP_ACCESS_PUBLIC;
      else if (TYPE_NESTED_TYPES_FIELD_PROTECTED (type, i))
	accessibility = GCC_CP_ACCESS_PROTECTED;
      else if (TYPE_NESTED_TYPES_FIELD_PRIVATE (type, i))
	accessibility = GCC_CP_ACCESS_PRIVATE;
      else
	gdb_assert_not_reached ("unknown accessibility");
      instance->convert_type (TYPE_NESTED_TYPES_FIELD_TYPE (type, i),
			      accessibility);
    }
}

/* Convert data members defined in TYPE, which should be struct/class/union
   with gcc_type COMP_TYPE.  */

static void
ccp_convert_struct_or_union_members (compile_cplus_instance *instance,
				     struct type *type, gcc_type comp_type)
{
  int i;

  for (i = TYPE_N_BASECLASSES (type); i < TYPE_NFIELDS (type); ++i)
    {
      CORE_ADDR physaddr;
      gcc_type field_type;
      const char *field_name = TYPE_FIELD_NAME (type, i);

      if (TYPE_FIELD_IGNORE (type, i)
	  || TYPE_FIELD_ARTIFICIAL (type, i))
	continue;

      field_type
	= instance->convert_type (TYPE_FIELD_TYPE (type, i));

      if (field_is_static (&TYPE_FIELD (type, i)))
	{
	  switch (TYPE_FIELD_LOC_KIND (type, i))
	    {
	    case FIELD_LOC_KIND_PHYSADDR:
	      {
		physaddr = TYPE_FIELD_STATIC_PHYSADDR (type, i);

		instance->build_decl ("field physaddr", field_name,
				      (GCC_CP_SYMBOL_VARIABLE
				       | get_field_access_flag (type, i)),
				      field_type, NULL, physaddr,
				      NULL, 0);
	      }
	      break;

	    case FIELD_LOC_KIND_PHYSNAME:
	      {
		const char *physname = TYPE_FIELD_STATIC_PHYSNAME (type, i);
		struct block_symbol sym
		  = lookup_symbol (physname, instance->block (), VAR_DOMAIN,
				   NULL);
		const char *filename;
		unsigned int line;

		if (sym.symbol == NULL)
		  {
		    /* We didn't actually find the symbol.  There's little
		       we can do but ignore this member.  */
		    continue;
		  }
		filename = symbol_symtab (sym.symbol)->filename;
		line = SYMBOL_LINE (sym.symbol);
		physaddr = SYMBOL_VALUE_ADDRESS (sym.symbol);
		instance->build_decl ("field physname", field_name,
				      (GCC_CP_SYMBOL_VARIABLE
				       | get_field_access_flag (type, i)),
				      field_type, NULL, physaddr,
				      filename, line);
	      }
	      break;

	    default:
	      gdb_assert_not_reached
		("unexpected static field location kind");
	    }
	}
      else
	{
	  unsigned long bitsize = TYPE_FIELD_BITSIZE (type, i);
	  enum gcc_cp_symbol_kind field_flags = GCC_CP_SYMBOL_FIELD
	    | get_field_access_flag (type, i)
	  /* FIXME:
	    | (field-is-mutable-p (type, i)
	       ? GCC_CP_FLAG_FIELD_MUTABLE
	       : GCC_CP_FLAG_FIELD_NOFLAG)
	     -lxo */
	    ;

	  if (bitsize == 0)
	    bitsize = 8 * TYPE_LENGTH (TYPE_FIELD_TYPE (type, i));

	  /* FIXME: We have to save the returned decl somewhere, so
	     that we can refer to it in expressions, in context for
	     lambdas, etc.  */
	  instance->build_field (field_name, field_type, field_flags,
				 bitsize, TYPE_FIELD_BITPOS (type, i));
	}
    }
}

/* Convert a method type to its gcc representation.  */

static gcc_type
ccp_convert_method (compile_cplus_instance *instance,
		    struct type *parent_type, struct type *method_type)
{
  gcc_type result, func_type, class_type;
  gcc_cp_qualifiers_flags quals;
  gcc_cp_ref_qualifiers_flags rquals;

  /* Get the actual (proto)type of the method, as a function.  */
  func_type = ccp_convert_func (instance, method_type, 1);

  class_type = instance->convert_type (parent_type);
  quals = (enum gcc_cp_qualifiers) 0;
  if (TYPE_CONST (method_type))
    quals |= GCC_CP_QUALIFIER_CONST;
  if (TYPE_VOLATILE (method_type))
    quals |= GCC_CP_QUALIFIER_VOLATILE;
  if (TYPE_RESTRICT (method_type))
    quals |= GCC_CP_QUALIFIER_RESTRICT;
  rquals = GCC_CP_REF_QUAL_NONE;
  /* !!keiths FIXME  */
  result = instance->build_method_type (class_type, func_type, quals, rquals);
  return result;
}

/* Convert a member or method pointer represented by TYPE.  */

static gcc_type
ccp_convert_memberptr (compile_cplus_instance *instance, struct type *type)
{
  struct type *containing_class = TYPE_SELF_TYPE (type);

  if (containing_class == NULL)
    return GCC_TYPE_NONE;

  gcc_type class_type = instance->convert_type (containing_class);
  gcc_type member_type = instance->convert_type (TYPE_TARGET_TYPE (type));

  return instance->build_pointer_to_member_type (class_type, member_type);
}

#define OPHASH1(A) ((uint32_t) A << 16)
#define OPHASH2(A,B) OPHASH1(A) | (uint32_t) B << 8
#define OPHASH3(A,B,C) OPHASH2(A,B) | (uint32_t) C

/* Compute a one, two, or three letter hash for the operator given by
   OP.  Returns the computed hash or zero for (some) conversion operators.  */

static uint32_t
operator_hash (const char *op)
{
  const char *p = op;
  uint32_t hash = 0;
  int len = 0;

  while (p[0] != '\0' && (p[0] != '(' || p[1] == ')'))
    {
      ++len;
      ++p;
    }

  switch (len)
    {
    case 1:
      hash = OPHASH1(op[0]);
      break;
    case 2:
     hash = OPHASH2(op[0], op[1]);
     break;
    case 3:
      /* This will also hash "operator int", but the plug-in does not
	 recognize OPHASH3('i', 'n', 't'), so we still end up in the code
	 to do a conversion operator in the caller.  */
      hash = OPHASH3(op[0], op[1], op[2]);
      break;
    default:
      break;
    }

  return hash;
}

/* Returns non-zero iff TYPE represents a binary method.  */

static int
is_binary_method (const struct type *type)
{
  int i, len;

  for (i = 0, len = 0; i < TYPE_NFIELDS (type); ++i)
    {
      if (!TYPE_FIELD_ARTIFICIAL (type, i))
	++len;
    }

 return len > 1;
}

/* See compile-cplus.h.  */

const char *
compile::maybe_canonicalize_special_function
  (const char *field_name, const struct fn_field *method_field,
   const struct type *method_type, char **outname, bool *ignore)
{
  /* We assume that no method is to be ignored.  */
  *ignore = false;

  /* We only consider ctors and dtors if METHOD_FIELD is non-NULL.  */
  if (method_field != NULL)
    {
      if (method_field->is_constructor)
	{
#if DEBUG_XTOR
	  printf ("*** CONSTRUCTOR %s: ", field_name);
#endif
#if CAUSES_ICE
	  switch (method_field->cdtor_type.ctor_kind)
	    {
	    case complete_object_ctor:
#if DEBUG_XTOR
	      printf ("complete_object_ctor (C1)\n");
#endif
	      return "C1";

	    case base_object_ctor:
#if DEBUG_XTOR
	      printf ("base_object_ctor (C2)\n");
#endif
	      return "C2";

	    case complete_object_allocating_ctor:
	      *ignore = true;
#if DEBUG_XTOR
	      printf ("complete_object_allocating_ctor -- ignored\n");
#endif
	      return field_name; /* C?  */

	    case unified_ctor:
#if DEBUG_XTOR
	      printf ("unified_ctor (C4) -- ignored\n");
#endif
	      *ignore = true;
	      return field_name; /* C4  */

	    case object_ctor_group:
#if DEBUG_XTOR
	      printf ("object_ctor_group -- ignored\n");
#endif
	      *ignore = true;
	      return field_name; /* C?  */

	    case unknown_ctor:
#if DEBUG_XTOR
	      printf ("unknown_ctr -- ignored\n");
#endif
	      *ignore = true;
	      return field_name; /* unknown  */

	    default:
	      gdb_assert_not_reached ("unknown constructor kind");
	    }
#else
#if DEBUG_XTOR
	  printf ("DISABLED -- ignored\n");
#endif
	  *ignore = true;
#endif /* CAUSES_ICE  */
	}
      else  if (method_field->is_destructor)
	{
#if DEBUG_XTOR
	  printf ("*** DESTRUCTOR %s: ", field_name);
#endif
	  switch (method_field->cdtor_type.dtor_kind)
	    {
	    case deleting_dtor:
#if DEBUG_XTOR
	      printf ("deleting_dtor (D0)\n");
#endif
	      return "D0";

	    case complete_object_dtor:
#if DEBUG_XTOR
	      printf ("complete_object_dtor (D1)\n");
#endif
	      return "D1";

	    case base_object_dtor:
#if DEBUG_XTOR
	      printf ("base_object_dtor (D2)\n");
#endif
	      return "D2";

	    case unified_dtor:
#if DEBUG_XTOR
	      printf ("unified_dtor (D4) -- ignored\n");
#endif
	      *ignore = true;
	      return field_name; /* D4  */

	    case object_dtor_group:
#if DEBUG_XTOR
	      printf ("object_dtor_group (D?) -- ignored\n");
#endif
	      *ignore = true;
	      return field_name; /* D?  */

	    case unknown_dtor:
#if DEBUG_XTOR
	      printf ("unknown_dtor -- ignored\n");
#endif
	      *ignore = true;
	      return field_name; /* unknown  */

	    default:
	      gdb_assert_not_reached ("unknown destructor kind");
	    }
	}
    }

  if (!is_operator_name (field_name))
    return field_name;

  /* Skip over "operator".  */
  field_name += sizeof ("operator") - 1;

  if (strncmp (field_name, "new", sizeof ("new") - 1) == 0)
    {
      field_name += 3;
      if (*field_name == '\0')
	return "nw";
      else if (*field_name == '[')
	return "na";
    }
  else if (strncmp (field_name, "delete", sizeof ("delete") - 1) == 0)
    {
      if (*field_name == '\0')
	return "dl";
      else if (*field_name == '[')
	return "da";
    }
  else if (field_name[0] == '\"' && field_name[1] == '\"')
    {
      const char *end;
      size_t len;

      /* Skip over \"\" -- the plug-in doesn't want it.  */
      field_name += 2;

      /* Skip any whitespace that may have been introduced during
	 canonicalization.  */
      field_name = skip_spaces_const (field_name);

      /* Find the opening '(', if any.  */
      end = strchr (field_name, '(');
      if (end == NULL)
	end = field_name + strlen (field_name);

      /* Size of buffer: 'li', 'i', sizeof operator name, '\0'  */
      len = 2 + end - field_name + 1;
      *outname = (char *) xmalloc (len);
      strcpy (*outname, "li");
      strncat (*outname, field_name, end - field_name);
      (*outname)[len-1] = '\0';
      return "li";
    }

  switch (operator_hash (field_name))
    {
    case OPHASH1 ('+'):
      if (is_binary_method (method_type))
	return "pl";
      else
	return "ps";
      break;

    case OPHASH1 ('-'):
      if (is_binary_method (method_type))
	return "mi";
      else
	return "ng";
      break;
    case OPHASH1 ('&'):
      if (is_binary_method (method_type))
	return "an";
      else
	return "ad";
      break;

    case OPHASH1 ('*'):
      if (is_binary_method (method_type))
	return "ml";
      else
	return "de";
      break;

    case OPHASH1 ('~'):
      return "co";
    case OPHASH1 ('/'):
      return "dv";
    case OPHASH1 ('%'):
      return "rm";
    case OPHASH1 ('|'):
      return "or";
    case OPHASH1 ('^'):
      return "eo";
    case OPHASH1 ('='):
      return "aS";
    case OPHASH2 ('+', '='):
      return "pL";
    case OPHASH2 ('-', '='):
      return "mI";
    case OPHASH2 ('*', '='):
      return "mL";
    case OPHASH2 ('/', '='):
      return "dV";
    case OPHASH2 ('%', '='):
      return "rM";
    case OPHASH2 ('&', '='):
      return "aN";
    case OPHASH2 ('|', '='):
      return "oR";
    case OPHASH2 ('^', '='):
      return "eO";
    case OPHASH2 ('<', '<'):
      return "ls";
    case OPHASH2 ('>', '>'):
      return "rs";
    case OPHASH3 ('<', '<', '='):
      return "lS";
    case OPHASH3 ('>', '>', '='):
      return "rS";
    case OPHASH2 ('=', '='):
      return "eq";
    case OPHASH2 ('!', '='):
      return "ne";
    case OPHASH1 ('<'):
      return "lt";
    case OPHASH1 ('>'):
      return "gt";
    case OPHASH2 ('<', '='):
      return "le";
    case OPHASH2 ('>', '='):
      return "ge";
    case OPHASH1 ('!'):
      return "nt";
    case OPHASH2 ('&', '&'):
      return "aa";
    case OPHASH2 ('|', '|'):
      return "oo";
    case OPHASH2 ('+', '+'):
      return "pp";
    case OPHASH2 ('-', '-'):
      return "mm";
    case OPHASH1 (','):
      return "cm";
    case OPHASH3 ('-', '>', '*'):
      return "pm";
    case OPHASH2 ('-', '>'):
      return "pt";
    case OPHASH2 ('(', ')'):
      return "cl";
    case OPHASH2 ('[', ']'):
      return "ix";
    case OPHASH1 ('?'):
      return "qu";

    default:
      /* Conversion operators: Full name is not needed.  */
      return "cv";
    }
}

/* Convert all methods defined in TYPE, which should be a class/struct/union
   with gcc_type CLASS_TYPE.  */

static void
ccp_convert_struct_or_union_methods (compile_cplus_instance *instance,
				     struct type *type, gcc_type class_type)
{
  int i;

  /* First things first: If this class had any template methods, emit them so
     that the compiler knows about them.  */
  instance->emit_function_template_decls ();


  /* Now define the actual methods/template specializations.  */
  for (i = 0; i < TYPE_NFN_FIELDS (type); ++i)
    {
      int j;
      struct fn_field *methods = TYPE_FN_FIELDLIST1 (type, i);
      char *overloaded_name
	= decl_name (TYPE_FN_FIELDLIST_NAME (type, i));
      struct cleanup *outer = make_cleanup (xfree, overloaded_name);

      /* Loop through the fieldlist, adding decls to the compiler's
	 representation of the class.  */
      for (j = 0; j < TYPE_FN_FIELDLIST_LENGTH (type, i); ++j)
	{
	  CORE_ADDR address;
	  gcc_type method_type;
	  struct block_symbol sym;
	  const char *filename;
	  unsigned int line;
	  const char *kind;
	  gcc_cp_symbol_kind_flags sym_kind = GCC_CP_SYMBOL_FUNCTION;
	  const char *name;
	  char *special_name;
	  struct cleanup *back_to;
	  bool ignore;
	  struct template_symbol *tsym = NULL;

	  /* Skip artificial methods.  */
	  if (TYPE_FN_FIELD_ARTIFICIAL (methods, j))
	    continue;

	  special_name = NULL;
	  name = maybe_canonicalize_special_function (overloaded_name,
						      &methods[j],
						      methods[j].type,
						      &special_name,
						      &ignore);
	  if (ignore)
	    continue;

	  back_to = make_cleanup (null_cleanup, NULL);

	  if (special_name != NULL)
	    {
	      make_cleanup (xfree, special_name);
	      name = special_name;
	    }

	  if (name != overloaded_name)
	    {
	      sym_kind |= GCC_CP_FLAG_SPECIAL_FUNCTION;
	    }
	  sym = lookup_symbol (TYPE_FN_FIELD_PHYSNAME (methods, j),
			       instance->block (), VAR_DOMAIN, NULL);

	  if (sym.symbol == NULL)
	    {
	      if (TYPE_FN_FIELD_VIRTUAL_P (methods, j))
		{
		  /* This is beyond hacky, and is really only a workaround for
		     detecting pure virtual methods.  */
		  method_type
		    = ccp_convert_method (instance, type,
					  TYPE_FN_FIELD_TYPE (methods, j));

		  instance->build_decl ("pure virtual method", name,
					(sym_kind
					 | get_method_access_flag (type, i, j)
					 | GCC_CP_FLAG_VIRTUAL_FUNCTION
					 | GCC_CP_FLAG_PURE_VIRTUAL_FUNCTION),
					method_type, NULL, 0,
					NULL /* FIXME: filename  */,
					0 /* FIXME: line number  */);
		  do_cleanups (back_to);
		  continue;
		}

	      /* This can happen if we have a DW_AT_declaration DIE
		 for the method, but no "definition"-type DIE (with
		 DW_AT_specification referencing the decl DIE), i.e.,
		 the compiler has probably optimized the method away.

		 In this case, all we can hope to do is issue a warning
		 to the user letting him know.  If the user has not actually
		 requested using this method, things should still work.  */
	      warning (_("Method %s appears to be optimized out.\n"
			 "All references to this method will be undefined."),
			 TYPE_FN_FIELD_PHYSNAME (methods, j));
	      do_cleanups (back_to);
	      continue;
	    }

	  filename = symbol_symtab (sym.symbol)->filename;
	  line = SYMBOL_LINE (sym.symbol);
	  address = BLOCK_START (SYMBOL_BLOCK_VALUE (sym.symbol));

	  /* Short-circuit for method templates.  */
	  if (SYMBOL_IS_CPLUS_TEMPLATE_FUNCTION (sym.symbol))
	    {
	      struct template_symbol *tsymbol
		= (struct template_symbol *) sym.symbol;

	      instance->build_function_template_specialization (tsymbol,
								address,
								filename, line);
	      do_cleanups (back_to);
	      continue;
	    }

	  if (TYPE_FN_FIELD_STATIC_P (methods, j))
	    {
	      kind = "static method";
	      method_type = ccp_convert_func (instance,
					      TYPE_FN_FIELD_TYPE (methods, j),
					      1);
	    }
	  else
	    {
	      kind = "method";
	      method_type
		= ccp_convert_method (instance, type,
				      TYPE_FN_FIELD_TYPE (methods, j));
	    }

	  if (TYPE_FN_FIELD_VIRTUAL_P (methods, j))
	    sym_kind |= GCC_CP_FLAG_VIRTUAL_FUNCTION;

	  /* FIXME: for cdtors, we must call build_decl with a zero
	     address, if we haven't created the base declaration
	     yet, and then define_cdtor_clone with the address of
	     each clone.  When we leave the address out, GCC uses
	     the address oracle.  -lxo  */
	  if ((sym_kind & GCC_CP_FLAG_SPECIAL_FUNCTION)
	      && (name[0] == 'C' || name[0] == 'D'))
	    {
	      address = 0;
	      /* FIXME: We should declare only one cdtor for each
		 clone set with "C" or "D" as the name, with address
		 zero, then define each address with
		 define_cdtor_clone.  Until this is implemented,
		 declare only one of these, and let the address oracle
		 take care of the addresses.  -lxo */
	      if (name[1] != '2' && name[1] != '4')
		{
		  do_cleanups (back_to);
		  continue;
		}
	    }

	  instance->build_decl (kind, name,
				sym_kind | get_method_access_flag (type, i, j),
				method_type, NULL, address, filename, line);
	  do_cleanups (back_to);
	}

      do_cleanups (outer);
    }
}

/* Convert a struct or union type to its gcc representation.  If this type
   was defined in another type, NESTED_ACCESS should indicate the
   accessibility of this type.  */

static gcc_type
ccp_convert_struct_or_union (compile_cplus_instance *instance,
			     struct type *type,
			     enum gcc_cp_symbol_kind nested_access)
{
  const char *filename = NULL;  /* !!keiths: FIXME  */
  unsigned short line = 0;
  char *name = NULL;
  struct cleanup *back_to = make_cleanup (free_current_contents, &name);

  /* Get the decl name of this type.  */
  if (TYPE_NAME (type) != NULL)
      name = decl_name (TYPE_NAME (type));

  /* First things first: If this type has any templates in it, make sure
     that we collect default arguments and get those types defined BEFORE
     this type is defined.  */
  scan_type_for_function_templates (instance, type);

  /* If this is a new template class, make sure the generic has been seen
     and defined.  */
  instance->maybe_define_new_class_template (type, name);
  instance->emit_class_template_decls ();

  /* Create a new scope for TYPE.  */
  compile_scope scope = instance->new_scope (TYPE_NAME (type), type);

  if (scope.nested_type () != GCC_TYPE_NONE)
    {
      /* The type requested was actually defined inside another type,
	 such as a nested class definition.  Return that type.  */
      return scope.nested_type ();
    }

  /* Push all scopes.  */
  instance->enter_scope (scope);

  /* First we create the resulting type and enter it into our hash
     table.  This lets recursive types work.  */

  gcc_decl resuld; /* FIXME: yeah, it's a terrible pun.  Please make
		      it go once we separate declaration from
		      definition (see below).  -lxo */
  if (TYPE_N_TEMPLATE_ARGUMENTS (type))
    {
      resuld = instance->build_class_template_specialization (type,
							      filename, line);
    }
  else if (TYPE_CODE (type) == TYPE_CODE_STRUCT)
    {
      const char *what = TYPE_DECLARED_CLASS (type) ? "struct" : "class";

      resuld = instance->build_decl (what, name,
				     GCC_CP_SYMBOL_CLASS | nested_access
				     | (TYPE_DECLARED_CLASS (type)
					? GCC_CP_FLAG_CLASS_NOFLAG
					: GCC_CP_FLAG_CLASS_IS_STRUCT),
				     0, NULL, 0, filename, line);
    }
  else
    {
      gdb_assert (TYPE_CODE (type) == TYPE_CODE_UNION);
      resuld = instance->build_decl ("union", name,
				     GCC_CP_SYMBOL_UNION | nested_access,
				     0, NULL, 0, filename, line);
    }

  /* FIXME: we should be able to pop the scope at this point, rather
     than at the end, and we ought to delay the rest of this function
     to the point in which we need the class or union to be a complete
     type, otherwise some well-formed C++ types cannot be represented.
     -lxo */

  gcc_type result;
  if (TYPE_CODE (type) == TYPE_CODE_STRUCT)
    {
      struct gcc_vbase_array bases;
      int num_baseclasses = TYPE_N_BASECLASSES (type);

      memset (&bases, 0, sizeof (bases));

      if (num_baseclasses > 0)
	{
	  bases.elements = XNEWVEC (gcc_type, num_baseclasses);
	  bases.flags = XNEWVEC (enum gcc_cp_symbol_kind, num_baseclasses);
	  bases.n_elements = num_baseclasses;
	  for (int i = 0; i < num_baseclasses; ++i)
	    {
	      struct type *base_type = TYPE_BASECLASS (type, i);

	      bases.flags[i] = GCC_CP_SYMBOL_BASECLASS
		| get_field_access_flag (type, i)
		| (BASETYPE_VIA_VIRTUAL (type, i)
		   ? GCC_CP_FLAG_BASECLASS_VIRTUAL
		   : GCC_CP_FLAG_BASECLASS_NOFLAG);
	      bases.elements[i] = instance->convert_type (base_type);
	    }
	}

      result = instance->start_class_type (name, resuld, &bases,
					   filename, line);
      xfree (bases.flags);
      xfree (bases.elements);
    }
  else
    {
      gdb_assert (TYPE_CODE (type) == TYPE_CODE_UNION);
      result
	= instance->start_class_type (name, resuld, NULL, filename, line);
    }

  instance->insert_type (type, result);

  /* Add definitions.  */
  ccp_convert_type_defns (instance, type);

  /* Add methods.  */
  ccp_convert_struct_or_union_methods (instance, type, result);

  /* Add members.  */
  ccp_convert_struct_or_union_members (instance, type, result);

  /* All finished.  */
  instance->finish_class_type (name, TYPE_LENGTH (type));

  /* Pop all scopes.  */
  instance->leave_scope ();
  do_cleanups (back_to);
  return result;
}

/* Convert an enum type to its gcc representation.  If this type
   was defined in another type, NESTED_ACCESS should indicate the
   accessibility of this type.*/

static gcc_type
ccp_convert_enum (compile_cplus_instance *instance, struct type *type,
		  enum gcc_cp_symbol_kind nested_access)
{
  int i;
  gcc_type int_type;
  const char *filename = NULL;
  unsigned short line = 0;
  /* !!keiths: This does not appear to work. GCC complains about
     being unable to convert enum values from '(MyEnum)0' to 'int'.  */
  int scoped_enum_p = /*TYPE_DECLARED_CLASS (type) ? TRUE :*/ FALSE;

  /* Create a new scope for this type.  */
  compile_scope scope = instance->new_scope (TYPE_NAME (type), type);

  if (scope.nested_type () != GCC_TYPE_NONE)
    {
      /* The type requested was actually defined inside another type,
	 such as a nested class definition.  Return that type.  */
      return scope.nested_type ();
    }

  char *name = NULL;
  struct cleanup *cleanups = make_cleanup (free_current_contents, &name);

  if (TYPE_NAME (type) != NULL)
    name = cp_func_name (TYPE_NAME (type));

  /* Push all scopes.  */
  instance->enter_scope (scope);

  int_type = instance->get_int_type (TYPE_UNSIGNED (type),
				     TYPE_LENGTH (type), NULL);
  gcc_type result
    = instance->start_enum_type (name, int_type,
				 GCC_CP_SYMBOL_ENUM | nested_access
				 | (scoped_enum_p
				    ? GCC_CP_FLAG_ENUM_SCOPED
				    : GCC_CP_FLAG_ENUM_NOFLAG),
				 filename, line);
  for (i = 0; i < TYPE_NFIELDS (type); ++i)
    {
      char *fname = cp_func_name (TYPE_FIELD_NAME (type, i));

      if (TYPE_FIELD_LOC_KIND (type, i) != FIELD_LOC_KIND_ENUMVAL
	  || fname == NULL)
	{
	  xfree (fname);
	  continue;
	}

      instance->build_enum_constant (result, fname,
				     TYPE_FIELD_ENUMVAL (type, i));
      xfree (fname);
    }

  instance->finish_enum_type (result);

  /* Pop all scopes.  */
  instance->leave_scope ();
  do_cleanups (cleanups);
  return result;
}

/* Convert a function type to its gcc representation.  This function does
   not deal with function templates.  */

static gcc_type
ccp_convert_func (compile_cplus_instance *instance, struct type *type,
		  int strip_artificial)
{
  int i, artificials;
  gcc_type result, return_type;
  struct gcc_type_array array;
  int is_varargs = is_varargs_p (type);

  /* This approach means we can't make self-referential function
     types.  Those are impossible in C, though.  */
  return_type = instance->convert_type (TYPE_TARGET_TYPE (type));

  array.n_elements = TYPE_NFIELDS (type);
  array.elements = XNEWVEC (gcc_type, TYPE_NFIELDS (type));
  artificials = 0;
  for (i = 0; i < TYPE_NFIELDS (type); ++i)
    {
      if (strip_artificial && TYPE_FIELD_ARTIFICIAL (type, i))
	{
	  --array.n_elements;
	  ++artificials;
	}
      else
	{
	  array.elements[i - artificials]
	    = instance->convert_type (TYPE_FIELD_TYPE (type, i));
	}
    }

  /* FIXME: add default args, exception specs and, once support is
     added, attributes.  -lxo */

  /* We omit setting the argument types to `void' to be a little flexible
     with some minsyms like printf (compile-cplus.exp has examples).  */
  result = instance->build_function_type (return_type, &array, is_varargs);
  xfree (array.elements);

  return result;
}

/* Convert an integer type to its gcc representation.  */

static gcc_type
ccp_convert_int (compile_cplus_instance *instance, struct type *type)
{
  if (TYPE_NOSIGN (type))
    {
      gdb_assert (TYPE_LENGTH (type) == 1);
      return instance->get_char_type ();
    }

  return instance->get_int_type (TYPE_UNSIGNED (type), TYPE_LENGTH (type),
				 TYPE_NAME (type));
}

/* Convert a floating-point type to its gcc representation.  */

static gcc_type
ccp_convert_float (compile_cplus_instance *instance, struct type *type)
{
  return instance->get_float_type (TYPE_LENGTH (type), TYPE_NAME (type));
}

/* Convert the 'void' type to its gcc representation.  */

static gcc_type
ccp_convert_void (compile_cplus_instance *instance, struct type *type)
{
  return instance->get_void_type ();
}

/* Convert a boolean type to its gcc representation.  */

static gcc_type
ccp_convert_bool (compile_cplus_instance *instance, struct type *type)
{
  return instance->get_bool_type ();
}

/* See description in compile-cplus.h.  */

gcc_type
compile::convert_qualified_base (compile_cplus_instance *instance,
				 gcc_type base,
				 gcc_cp_qualifiers_flags quals)
{
  gcc_type result = base;

  if (quals != 0)
    result = instance->build_qualified_type (base, quals);

  return result;
}

/* Convert a qualified type to its gcc representation.  */

static gcc_type
ccp_convert_qualified (compile_cplus_instance *instance,
		       struct type *type)
{
  struct type *unqual = make_unqualified_type (type);
  gcc_type unqual_converted;
  gcc_cp_qualifiers_flags quals = (enum gcc_cp_qualifiers) 0;
  gcc_type result;

  unqual_converted = instance->convert_type (unqual);

  if (TYPE_CONST (type))
    quals |= GCC_CP_QUALIFIER_CONST;
  if (TYPE_VOLATILE (type))
    quals |= GCC_CP_QUALIFIER_VOLATILE;
  if (TYPE_RESTRICT (type))
    quals |= GCC_CP_QUALIFIER_RESTRICT;

  return convert_qualified_base (instance, unqual_converted, quals);
}

/* Convert a complex type to its gcc representation.  */

static gcc_type
ccp_convert_complex (compile_cplus_instance *instance,
		     struct type *type)
{
  gcc_type base = instance->convert_type (TYPE_TARGET_TYPE (type));

  return instance->build_complex_type (base);
}

/* Convert a namespace of TYPE.  */

static gcc_type
ccp_convert_namespace (compile_cplus_instance *instance,
		       struct type *type)
{
  compile_scope scope = instance->new_scope (TYPE_NAME (type), type);


  char *name = NULL;
  struct cleanup *cleanups = make_cleanup (null_cleanup, NULL);
  if (TYPE_NAME (type) != NULL)
    {
      name = cp_func_name (TYPE_NAME (type));
      make_cleanup (xfree, name);
    }
  else
    name = "";

  /* Push scope.  */
  instance->enter_scope (scope);

  /* Convert this namespace.  */
  instance->push_namespace (name);
  instance->pop_binding_level (name);

  /* Pop scope.  */
  instance->leave_scope ();
  do_cleanups (cleanups);

  /* Namespaces are non-cacheable types.  */
  return DONT_CACHE_TYPE;
}

/* A helper function which knows how to convert most types from their
   gdb representation to the corresponding gcc form.  This examines
   the TYPE and dispatches to the appropriate conversion function.  It
   returns the gcc type.

   If the type was defined in another type, NESTED_ACCESS should indicate the
   accessibility of this type.  */

static gcc_type
convert_type_cplus_basic (compile_cplus_instance *instance,
			  struct type *type,
			  enum gcc_cp_symbol_kind nested_access)
{
  /* Reference types seem to always have a const qualifier, but we
     don't want that to be propagated to the GCC type, because GCC
     doesn't like the reference types themselves to be qualified.  */
  if (TYPE_CODE (type) == TYPE_CODE_REF)
    return ccp_convert_reference (instance, type);

  /* If we are converting a qualified type, first convert the
     unqualified type and then apply the qualifiers.  */
  if ((TYPE_INSTANCE_FLAGS (type) & (TYPE_INSTANCE_FLAG_CONST
				     | TYPE_INSTANCE_FLAG_VOLATILE
				     | TYPE_INSTANCE_FLAG_RESTRICT)) != 0)
    return ccp_convert_qualified (instance, type);

  switch (TYPE_CODE (type))
    {
#if 0
    case TYPE_CODE_REF:
      return ccp_convert_reference (instance, type);
#endif

    case TYPE_CODE_PTR:
      return ccp_convert_pointer (instance, type);

    case TYPE_CODE_ARRAY:
      return ccp_convert_array (instance, type);

    case TYPE_CODE_STRUCT:
    case TYPE_CODE_UNION:
      return ccp_convert_struct_or_union (instance, type, nested_access);

    case TYPE_CODE_ENUM:
      return ccp_convert_enum (instance, type, nested_access);

    case TYPE_CODE_FUNC:
      return ccp_convert_func (instance, type, 0);

    case TYPE_CODE_METHOD:
      return ccp_convert_method (instance, TYPE_SELF_TYPE (type), type);

    case TYPE_CODE_MEMBERPTR:
    case TYPE_CODE_METHODPTR:
      return ccp_convert_memberptr (instance, type);
      break;

    case TYPE_CODE_INT:
      return ccp_convert_int (instance, type);

    case TYPE_CODE_FLT:
      return ccp_convert_float (instance, type);

    case TYPE_CODE_VOID:
      return ccp_convert_void (instance, type);

    case TYPE_CODE_BOOL:
      return ccp_convert_bool (instance, type);

    case TYPE_CODE_COMPLEX:
      return ccp_convert_complex (instance, type);

    case TYPE_CODE_NAMESPACE:
      return ccp_convert_namespace (instance, type);

    case TYPE_CODE_TYPEDEF:
      return ccp_convert_typedef (instance, type, nested_access);
    }

  {
    char *s = xstrprintf (_("unhandled TYPE_CODE_%s"),
			  type_code_to_string (TYPE_CODE (type)));

    return instance->error (s);
    xfree (s);
  }
}

gcc_type
compile_cplus_instance::convert_type (struct type *type,
				      enum gcc_cp_symbol_kind nested_access)
{
  type_map_t::iterator pos = m_type_map.find (type);
  if (pos != m_type_map.end ())
    return pos->second;

  gcc_type result = convert_type_cplus_basic (this, type, nested_access);
  if (result != DONT_CACHE_TYPE)
    insert_type (type, result);
  return result;
}



/* See compile-cplus.h.  */

compile_cplus_instance::compile_cplus_instance (struct gcc_cp_context *gcc_fe)
  : compile_instance (&gcc_fe->base,
		      "-std=gnu++11"),
  m_context (gcc_fe),
  m_function_template_defns (new function_template_defn_map_t ()),
  m_class_template_defns (new class_template_defn_map_t ())
{
  gcc_fe->cp_ops->set_callbacks (gcc_fe, gcc_cplus_convert_symbol,
				 gcc_cplus_symbol_address,
				 gcc_cplus_enter_scope,
				 gcc_cplus_leave_scope,
				 this);
}

/* Plug-in forwards.  */

/* A result printer for plug-in calls that return a boolean result.  */

static void
ccp_output_result (int result)
{
  printf_unfiltered ("%s\n", result ? "true" : "false");
}

/* A result printer for plug-in calls that return a gcc_type or
   gcc_decl.  */

static void
ccp_output_result (gcc_type result)
{
  printf_unfiltered ("%lld\n", result);
}

#define STR(x) #x
#define STRINGIFY(x) STR(x)

#define DECLARE_FORWARD(OP,...)						\
  auto forward = [&] (const char *fmt, ...)				\
  {									\
    if (debug_compile_cplus_types)					\
      {									\
	std::string format (STRINGIFY (OP));				\
									\
	format += " ";							\
	format += fmt;							\
	format += ": ";							\
									\
	va_list args;							\
									\
	va_start (args, fmt);						\
	vprintf_unfiltered (format.c_str (), args);			\
	va_end (args);							\
      }									\
									\
    auto result = m_context->cp_ops->OP (m_context, ##__VA_ARGS__);	\
									\
    if (debug_compile_cplus_types)					\
      ccp_output_result (result);					\
									\
    return result;							\
  };

/* See description in gcc-cp-fe.def.  */

bool
compile_cplus_instance::build_constant (gcc_type type, const char *name,
					unsigned long value,
					const char *filename,
					unsigned int line_number)
{
  DECLARE_FORWARD (build_constant, type, name, value, filename, line_number);

  return forward ("\"%s\"", name);
}

/* See description in gcc-cp-fe.def.  */

gcc_decl
compile_cplus_instance::build_function_template_specialization
  (struct template_symbol *concrete, gcc_address address,
   const char *filename, unsigned int line_number)
{
  function_template_defn *defn
    = find_function_template_defn (concrete);

  /* A generic should already have been defined at this point.  */
  gdb_assert (defn != NULL);

  struct gcc_cp_template_args targs;

  targs.n_elements = concrete->template_arguments->n_arguments;
  targs.kinds = XNEWVEC (char, targs.n_elements);

  struct cleanup *back_to = make_cleanup (xfree, targs.kinds);

  targs.elements = XNEWVEC (gcc_cp_template_arg, targs.n_elements);
  make_cleanup (xfree, targs.elements);
  enumerate_template_arguments (&targs, defn, concrete->template_arguments);

  DECLARE_FORWARD (build_function_template_specialization, defn->decl (),
		   &targs, address, filename, line_number);

  gcc_decl result = forward ("%s", SYMBOL_NATURAL_NAME (&concrete->base));
  do_cleanups (back_to);
  return result;
}

/* See description in gcc-cp-fe.def.  */

gcc_decl
compile_cplus_instance::build_class_template_specialization
  (struct type *concrete, const char *filename, unsigned int line_number)
{
  class_template_defn *defn
    = find_class_template_defn (concrete);

  /* A generic should have already been defined at this point.  */
  gdb_assert (defn != NULL);

  struct gcc_cp_template_args targs;

  targs.n_elements = TYPE_N_TEMPLATE_ARGUMENTS (concrete);
  targs.kinds = XNEWVEC (char, targs.n_elements);

  struct cleanup *back_to = make_cleanup (xfree, targs.kinds);

  targs.elements = XNEWVEC (gcc_cp_template_arg, targs.n_elements);
  make_cleanup (xfree, targs.elements);
  enumerate_template_arguments (&targs, defn,
				TYPE_TEMPLATE_ARGUMENT_INFO (concrete));

  DECLARE_FORWARD (build_class_template_specialization, defn->decl (), &targs,
		   filename, line_number);

  gcc_decl result
    = forward ("%s for template decl %lld\n", TYPE_NAME (concrete),
	       defn->decl ());
  do_cleanups (back_to);
  return result;
}

/* See description in gcc-cp-fe.def.  */

gcc_decl
compile_cplus_instance::build_decl (const char *decl_type,
				    const char *name,
				    enum gcc_cp_symbol_kind sym_kind,
				    gcc_type sym_type,
				    const char *substitution_name,
				    gcc_address address, const char *filename,
				    unsigned int line_number)
{
  DECLARE_FORWARD (build_decl, name, sym_kind, sym_type,
		   substitution_name, address, filename, line_number);

  return forward ("%s %s %d %s", decl_type, name, (int) sym_kind,
		  substitution_name);
}

/* See description in gcc-cp-fe.def.  */

bool
compile_cplus_instance::push_namespace (const char *name)
{
  DECLARE_FORWARD (push_namespace, name);

  return forward ("\"%s\"", name);
}

/* See description in gcc-cp-fe.def.  */

bool
compile_cplus_instance::pop_binding_level (const char *opt_name)
{
  DECLARE_FORWARD (pop_binding_level);

  return forward ("\"%s\"", opt_name);
}

/* See description in gcc-cp-fe.def.  */

gcc_type
compile_cplus_instance::error (const char *message)
{
  DECLARE_FORWARD (error, message);

  return forward ("%s", message);
}

/* See description in gcc-cp-fe.def.  */


gcc_type
compile_cplus_instance::build_reference_type (gcc_type base_type,
					      enum gcc_cp_ref_qualifiers rquals)
{
  DECLARE_FORWARD (build_reference_type, base_type, rquals);

  return forward ("");
}

/* See description in gcc-cp-fe.def.  */

gcc_type
compile_cplus_instance::build_pointer_type (gcc_type base_type)
{
  DECLARE_FORWARD (build_pointer_type, base_type);

  return forward ("");
}

/* See description in gcc-cp-fe.def.  */

gcc_type
compile_cplus_instance::build_vla_array_type (gcc_type element_type,
					      const char *upper_bound_name)
{
  DECLARE_FORWARD (build_vla_array_type, element_type, upper_bound_name);

  return forward ("");
}

/* See description in gcc-cp-fe.def.  */

gcc_type
compile_cplus_instance::build_vector_type (gcc_type element_type,
					   int num_elements)
{
  DECLARE_FORWARD (build_vector_type, element_type, num_elements);

  return forward ("");
}

/* See description in gcc-cp-fe.def.  */

gcc_type
compile_cplus_instance::build_array_type (gcc_type element_type,
					  int num_elements)
{
  DECLARE_FORWARD (build_array_type, element_type, num_elements);

  return forward ("");
}

/* See description in gcc-cp-fe.def.  */

gcc_decl
compile_cplus_instance::build_field (const char *field_name,
				     gcc_type field_type,
				     enum gcc_cp_symbol_kind field_flags,
				     unsigned long bitsize,
				     unsigned long bitpos)
{
  DECLARE_FORWARD (build_field, field_name, field_type, field_flags,
		   bitsize, bitpos);

  return forward ("%s %lld", field_name, field_type);
}

/* See description in gcc-cp-fe.def.  */

gcc_type
compile_cplus_instance::build_method_type (gcc_type class_type,
					   gcc_type func_type,
					   enum gcc_cp_qualifiers quals,
					   enum gcc_cp_ref_qualifiers rquals)
{
  DECLARE_FORWARD (build_method_type, class_type, func_type, quals, rquals);

  return forward ("");
}

/* See description in gcc-cp-fe.def.  */

gcc_type
compile_cplus_instance::start_class_type
  (const char *name, gcc_decl typedecl,
   const struct gcc_vbase_array *base_classes,
   const char *filename, unsigned int line_number)
{
  DECLARE_FORWARD (start_class_type, typedecl, base_classes,
		   filename, line_number);

  return forward ("%s", name);
}

/* See description in gcc-cp-fe.def.  */

bool
compile_cplus_instance::finish_class_type (const char *name,
					   unsigned long size_in_bytes)
{
  DECLARE_FORWARD (finish_class_type, size_in_bytes);

  return forward ("%s (%ld)", name, size_in_bytes);
}

/* See description in gcc-cp-fe.def.  */

gcc_type
compile_cplus_instance::get_int_type (bool is_unsigned,
				      unsigned long size_in_bytes,
				      const char *builtin_name)
{
  DECLARE_FORWARD (get_int_type, is_unsigned, size_in_bytes, builtin_name);

  return forward ("%d %ld %s", is_unsigned, size_in_bytes, builtin_name);
}

/* See description in gcc-cp-fe.def.  */

gcc_type
compile_cplus_instance::start_enum_type (const char *name,
					 gcc_type underlying_int_type,
					 enum gcc_cp_symbol_kind flags,
					 const char *filename,
					 unsigned int line_number)
{
  DECLARE_FORWARD (start_enum_type, name, underlying_int_type,
		   flags, filename, line_number);

  return forward ("%s", name);
}

/* See description in gcc-cp-fe.def.  */

gcc_decl
compile_cplus_instance::build_enum_constant (gcc_type enum_type,
					     const char *name,
					     unsigned long value)
{
  DECLARE_FORWARD (build_enum_constant, enum_type, name, value);

  return forward ("%s = %ld", name, value);
}

/* See description in gcc-cp-fe.def.  */

bool
compile_cplus_instance::finish_enum_type (gcc_type enum_type)
{
  DECLARE_FORWARD (finish_enum_type, enum_type);

  return forward ("");
}

/* See description in gcc-cp-fe.def.  */

gcc_type
compile_cplus_instance::build_function_type
  (gcc_type return_type, const struct gcc_type_array *argument_types,
   bool is_varargs)
{
  DECLARE_FORWARD (build_function_type, return_type, argument_types,
		   is_varargs);

  return forward ("%lld %d", return_type, is_varargs);
}

/* See description in gcc-cp-fe.def.  */

gcc_type
compile_cplus_instance::get_char_type ()
{
  DECLARE_FORWARD (get_char_type);

  return forward ("");
}

/* See description in gcc-cp-fe.def.  */

gcc_type
compile_cplus_instance::get_float_type (unsigned long size_in_bytes,
					const char *builtin_name)
{
  DECLARE_FORWARD (get_float_type, size_in_bytes, builtin_name);

  return forward  ("%ld %s", size_in_bytes, builtin_name);
}

/* See description in gcc-cp-fe.def.  */

gcc_type
compile_cplus_instance::get_void_type ()
{
  DECLARE_FORWARD (get_void_type);

  return forward ("");
}

/* See description in gcc-cp-fe.def.  */

gcc_type
compile_cplus_instance::get_bool_type ()
{
  DECLARE_FORWARD (get_bool_type);

  return forward ("");
}

/* See description in gcc-cp-fe.def.  */

gcc_type
compile_cplus_instance::build_qualified_type (gcc_type unqualified_type,
					      enum gcc_cp_qualifiers qualifiers)
{
  DECLARE_FORWARD (build_qualified_type, unqualified_type, qualifiers);

  return forward ("");
}

/* See description in gcc-cp-fe.def.  */

gcc_type
compile_cplus_instance::build_complex_type (gcc_type element_type)
{
  DECLARE_FORWARD (build_complex_type, element_type);

  return forward ("%lld", element_type);
}

/* See description in gcc-cp-fe.def.  */

gcc_expr
compile_cplus_instance::build_literal_expr (gcc_type type, unsigned long value)
{
  DECLARE_FORWARD (build_literal_expr, type, value);

  return forward ("%lld %ld", type, value);
}

/* See description in gcc-cp-fe.def.  */

gcc_type
compile_cplus_instance::build_type_template_parameter (const char *id,
						       bool pack_p,
						       gcc_type default_type,
						       const char *filename,
						       unsigned int line_number)
{
  DECLARE_FORWARD (build_type_template_parameter, id, pack_p,
		   default_type, filename, line_number);

  return forward ("%s %d %lld %s %d", id, pack_p, default_type,
		  filename, line_number);
}

/* See description in gcc-cp-fe.def.  */

gcc_decl
compile_cplus_instance::build_value_template_parameter
  (gcc_type type, const char *id, gcc_expr default_value,
   const char *filename, unsigned int line_number)
{
  DECLARE_FORWARD (build_value_template_parameter, type, id,
		   default_value, filename, line_number);

  return forward ("%lld %s %lld %s %d", type, id, default_value,
		  filename, line_number);
}

/* See description in gcc-cp-fe.def.  */

bool
compile_cplus_instance::start_template_decl (const char *generic)
{
  DECLARE_FORWARD (start_template_decl);

  return forward ("for generic %s\n", generic);
}

/* See description in gcc-cp-fe.def.  */

gcc_type
compile_cplus_instance::build_pointer_to_member_type (gcc_type class_type,
						      gcc_type member_type)
{
  DECLARE_FORWARD (build_pointer_to_member_type, class_type, member_type);

  return forward ("%lld %lld", class_type, member_type);
}

#undef DECLARE_FORWARD

void _initialize_compile_cplus_types (void);

void
_initialize_compile_cplus_types (void)
{
  add_setshow_boolean_cmd ("compile-cplus-types", no_class,
			     &debug_compile_cplus_types, _("\
Set debugging of C++ compile type conversion."), _("\
Show debugging of C++ compile type conversion."), _("\
When enabled debugging messages are printed during C++ type conversion for\n\
the compile commands."),
			     NULL,
			     NULL,
			     &setdebuglist,
			     &showdebuglist);

  add_setshow_boolean_cmd ("compile-cplus-scopes", no_class,
			     &debug_compile_cplus_scopes, _("\
Set debugging of C++ compile scopes."), _("\
Show debugging of C++ compile scopes."), _("\
When enabled debugging messages are printed about definition scopes during\n\
C++ type conversion for the compile commands."),
			     NULL,
			     NULL,
			     &setdebuglist,
			     &showdebuglist);
}
