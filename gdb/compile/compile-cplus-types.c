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
#include "gdb_assert.h"
#include "symtab.h"
#include "source.h"
#include "cp-support.h"
#include "cp-abi.h"
#include "symtab.h"
#include "objfiles.h"
#include "block.h"
#include "gdbcmd.h"

#define CONTEXT_DEBUG 0

/* A "type" to indicate that convert_cplus_type should not attempt to
   cache its resultant type.  This is used, for example, when defining
   namespaces for the oracle.  */
#define DONT_CACHE_TYPE ((gcc_type) 0)

/* keiths: Stolen from parser-defs.h.  Seems silly to include all of
   parser-defs.h just for this.  [There is also a name conflict with
   insert_type.]  */

/* A string type.  */
struct stoken
{
  /* Pointer to the beginning of the (not necessarily terminated)
     string.  */
  const char *ptr;

  /* Length of the string.  */
  int length;
};

/* A scope of a type.  Type names are broken into "scopes" or
   "components," a series of unqualified names comprising the
   type name, e.g., "namespace1", "namespace2", "myclass", "method".  */
struct compile_cplus_scope
{
  /* The unqualified name of this scope.  */
  struct stoken name;

  /* The block symbol for this type/scope.  */
  struct block_symbol bsymbol;
};
typedef struct compile_cplus_scope compile_cplus_scope_def;
DEF_VEC_O (compile_cplus_scope_def);

/* A processing context.  */
struct compile_cplus_context
{
  VEC (compile_cplus_scope_def) *scopes;
};
#define CONTEXT_SCOPES(PCTX) ((PCTX)->scopes)
typedef struct compile_cplus_context *compile_cplus_context_def;
DEF_VEC_P (compile_cplus_context_def);

/* A list of contexts we are processing.  A processing context
   is simply a non-namespace scope.  */
static VEC (compile_cplus_context_def) *cplus_processing_contexts = NULL;

/* An object that maps a gdb type to a gcc type.  */
struct type_map_instance
{
  /* The gdb type.  */
  const struct type *type;

  /* The corresponding gcc type handle.  */
  gcc_type gcc_type_handle;
};

/* Flag to enable internal debugging.  */

int debug_compile_cplus_types = 0;

/* Forward declarations.  */

static gcc_type ccp_convert_void (struct compile_cplus_instance *,
				  struct type *);

static gcc_type ccp_convert_func (struct compile_cplus_instance *context,
				  struct type *type, int strip_artificial);

/* Allocate a new C string from the given TOKEN.  */

static char *
copy_stoken (const struct stoken *token)
{
  char *string = (char *) xmalloc (token->length + 1);

  strncpy (string, token->ptr, token->length);
  string[token->length] = '\0';
  return string;
}

/* Hash a type_map_instance.  */

static hashval_t
hash_type_map_instance (const void *p)
{
  const struct type_map_instance *inst = (struct type_map_instance *) p;

  return htab_hash_pointer (inst->type);
}

/* Check two type_map_instance objects for equality.  */

static int
eq_type_map_instance (const void *a, const void *b)
{
  const struct type_map_instance *insta = (struct type_map_instance *) a;
  const struct type_map_instance *instb = (struct type_map_instance *) b;

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
  inst.gcc_type_handle = gcc_type;
  slot = htab_find_slot (context->type_map, &inst, INSERT);

  add = (struct type_map_instance *) *slot;

  /* The type might have already been inserted in order to handle
     recursive types.  */
  gdb_assert (add == NULL || add->gcc_type_handle == gcc_type);

  if (add == NULL)
    {
      add = XNEW (struct type_map_instance);
      *add = inst;
      *slot = add;
    }
}

/* A useful debugging function to output the processing context PCTX
   to stdout.  */

static void __attribute__ ((used))
debug_print_context (const struct compile_cplus_context *pctx)
{
  int i;
  struct compile_cplus_scope *scope;

  if (pctx != NULL)
    {
      for (i = 0;
	   VEC_iterate (compile_cplus_scope_def, CONTEXT_SCOPES (pctx),
			i, scope); ++i)
	{
	  char *name = copy_stoken (&scope->name);
	  const char *symbol;

	  symbol = (scope->bsymbol.symbol != NULL
		    ? SYMBOL_NATURAL_NAME (scope->bsymbol.symbol) : "<none>");
	  printf ("\tname = %s, symbol = %s\n", name, symbol);
	  xfree (name);
	}
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

/* Get the current processing context.  */

static struct compile_cplus_context *
get_current_processing_context (void)
{
  if (VEC_empty (compile_cplus_context_def, cplus_processing_contexts))
    return NULL;

  return VEC_last (compile_cplus_context_def, cplus_processing_contexts);
}

/* Get the main scope to process in the processing context PCTX.  */

static struct compile_cplus_scope *
get_processing_context_scope (struct compile_cplus_context *pctx)
{
  return VEC_last (compile_cplus_scope_def, CONTEXT_SCOPES (pctx));
}

/* !!keiths: Paranoia?  Sadly, we have this information already, but it is
   currently tossed when convert_cplus_type is called from
   convert_one_symbol.  */

static const struct block *
get_current_search_block (void)
{
  const struct block *block;
  enum language save_language;

  /* get_selected_block can change the current language when there
     is no selected frame yet.  */
  /* !!keiths this is probably a bit (over-)defensive, since we can't
     actually compile anything without running the inferior.  */
  save_language = current_language->la_language;
  block = get_selected_block (0);
  set_language (save_language);

  return block;
}

/* Convert TYPENAME into a vector of namespace and top-most/super
   composite scopes.

   For example, for the input "Namespace::classB::classInner", the
   resultant vector will contain the tokens {"Namespace", 9} and
   {"classB", 6}.

   The resulting VEC must be freed, but the individual components should not.
   This function may return NULL if TYPE represents an anonymous type.  */

static VEC (compile_cplus_scope_def) *
ccp_type_name_to_scopes (const char *type_name)
{
  const char *p;
  char *lookup_name;
  int running_length = 0;
  struct cleanup *back_to;
  VEC (compile_cplus_scope_def) *result = NULL;
  const struct block *block = get_current_search_block ();

  if (type_name == NULL)
    {
      /* An anonymous type.  We cannot really do much here.  We simply cannot
	 look up anonymous types easily/at all.  */
      return NULL;
    }

  back_to = make_cleanup (free_current_contents, &lookup_name);
  p = type_name;
  while (1)
    {
      struct stoken s;
      struct block_symbol bsymbol;

      s.ptr = p;
      s.length = cp_find_first_component (p);
      p += s.length;

      /* Look up the symbol and decide when to stop.  */
      if (running_length == 0)
	{
	  lookup_name = copy_stoken (&s);
	  running_length = s.length + 1;
	}
      else
	{
	  running_length += 2 + s.length;
	  lookup_name = (char *) xrealloc (lookup_name, running_length);
	  strcat (lookup_name, "::");
	  strncat (lookup_name, s.ptr, s.length);
	}
      bsymbol = lookup_symbol (lookup_name, block, VAR_DOMAIN, NULL);
      /* !!keiths: I seem to recall some special silliness with typedefs!  */
      if (bsymbol.symbol != NULL)
	{
	  struct compile_cplus_scope c = { s, bsymbol };

	  VEC_safe_push (compile_cplus_scope_def, result, &c);

	  if (TYPE_CODE (SYMBOL_TYPE (bsymbol.symbol))
	      != TYPE_CODE_NAMESPACE)
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

  do_cleanups (back_to);
  return result;
}

/* Does PCTX require a new processing context? Returns 1 if a new context
   should be pushed (by the caller), zero otherwise.  */

int
ccp_need_new_context (const struct compile_cplus_context *pctx)
{
  int i, cur_len, new_len;
  struct compile_cplus_scope *cur_scope, *new_scope;
  struct compile_cplus_context *current
    = get_current_processing_context ();

#if CONTEXT_DEBUG
  printf ("scopes for current context, %p:\n", current);
  debug_print_context (current);
  printf ("comparing to scopes of %p:\n", pctx);
  debug_print_context (pctx);
#endif

  /* PCTX is "the same" as the current scope iff
     1. current is not NULL
     2. CONTEXT_SCOPES of current and PCX are the same length.
     3. scope names are the same.  */
  if (current == NULL)
    {
#if CONTEXT_DEBUG
      printf ("no current context -- need new context\n");
#endif
      return 1;
    }

  cur_len = VEC_length (compile_cplus_scope_def, CONTEXT_SCOPES (current));
  new_len = VEC_length (compile_cplus_scope_def, CONTEXT_SCOPES (pctx));
  if (cur_len != new_len)
    {
#if CONTXET_DEBUG
      printf ("length mismatch: current %d, new %d -- need new context\n",
	      cur_len, new_len);
#endif
      return 1;
    }

  for (i = 0;
       (VEC_iterate (compile_cplus_scope_def,
		     CONTEXT_SCOPES (current), i, cur_scope)
	&& VEC_iterate (compile_cplus_scope_def,
			CONTEXT_SCOPES (pctx), i, new_scope)); ++i)
    {
      if (cur_scope->name.length != new_scope->name.length)
	{
#if CONTEXT_DEBUG
	  printf ("current scope namelen != newscope namelen -- need new context.\n");
#endif
	  return 1;
	}
      else
	{
	  const char *cur_name = cur_scope->name.ptr;
	  const char *new_name = new_scope->name.ptr;

	  if (strncmp (cur_name, new_name, cur_scope->name.length) != 0)
	    {
#if CONTEXT_DEBUG
	      char *a = alloca (cur_scope->name.length + 1);
	      char *b = alloca (cur_scope->name.length + 1);

	      strncpy (a, cur_scope->name.ptr, cur_scope->name.length);
	      strncpy (b, new_scope->name.ptr, new_scope->name.length);
	      printf ("%s != %s -- need new context\n",
		      cur_name, new_name);
#endif
	      return 1;
	    }
	}
    }

#if CONTEXT_DEBUG
  printf ("all scopes identical -- do NOT need new context\n");
#endif
  return 0;
}

/* Push the processing context PCTX into the given compiler CONTEXT.

   The main purpose of this function is to push namespaces, including the
   global namespace, into CONTEXT for type conversions.  */

void
ccp_push_processing_context (struct compile_cplus_instance *context,
			     struct compile_cplus_context *pctx)
{
  /* Push the scope we are processing.  */
  VEC_safe_push (compile_cplus_context_def, cplus_processing_contexts, pctx);

  /* Push the global namespace.  */
  CPOPS ("push_namespace \"\"\n");
  CPCALL (push_namespace, context, "");

  if (CONTEXT_SCOPES (pctx) != NULL)
    {
      int ix;
      struct compile_cplus_scope *scope, *last;

      /* Get the scope we are/will be processing.  */
      last = get_processing_context_scope (pctx);

      /* Push all other namespaces.  */
      for (ix = 0;
	   (VEC_iterate (compile_cplus_scope_def, CONTEXT_SCOPES (pctx),
			 ix, scope)
	    && scope != last);
	   ++ix)
	{
	  char *ns;

	  if (scope->name.length == CP_ANONYMOUS_NAMESPACE_LEN
	      && strncmp (scope->name.ptr, CP_ANONYMOUS_NAMESPACE_STR,
			  CP_ANONYMOUS_NAMESPACE_LEN) == 0)
	    ns = NULL;
	  else
	    ns = copy_stoken (&scope->name);
	  CPOPS ("push_namesapce %s\n", ns);
	  CPCALL (push_namespace, context, ns);
	  xfree (ns);
	}
    }
}

/* Pop the processing context PCTX from CONTEXT.

   This is largely used to pop any namespaces that were required to
   define a type from CONTEXT.  */

void
ccp_pop_processing_context (struct compile_cplus_instance *context,
			    struct compile_cplus_context *pctx)
{
  int ix;
  struct compile_cplus_scope *scope;
  struct compile_cplus_context *ctx;

  /* Pop the context.  */
  ctx = VEC_pop (compile_cplus_context_def, cplus_processing_contexts);
  gdb_assert (ctx == pctx);

  if (CONTEXT_SCOPES (pctx) != NULL)
    {
      struct compile_cplus_scope *last
	= VEC_last (compile_cplus_scope_def, CONTEXT_SCOPES (pctx));

      /* Pop namespaces.  */
      for (ix = 0;
	   (VEC_iterate (compile_cplus_scope_def, CONTEXT_SCOPES (pctx),
			 ix, scope)
	    && scope != last);
	   ++ix)
	{
	  char *ns = copy_stoken (&scope->name);

	  CPOPS ("pop_namespace %s\n", ns);
	  CPCALL (pop_namespace, context);
	  xfree (ns);
	}
    }

  /* Pop global namespace.  */
  CPOPS ("pop_namespace \"\"\n");
  CPCALL (pop_namespace, context);
}

/* Create a new processing context for TYPE with name TYPE_NAME.
   [TYPE_NAME could be TYPE_NAME or SYMBOL_NATURAL_NAME.]

   If TYPE is a nested or local definition, *NESTED_TYPE is set to the
   result and this function returns NULL.

   Otherwise, *NESTED_TYPE is set to GCC_TYPE_NONE and a new processing
   context is returned. [See description of get_type_scopes for more.]
   The result should be freed with delete_processing_context.  */

struct compile_cplus_context *
new_processing_context (struct compile_cplus_instance *context,
			const char *type_name, struct type *type,
			gcc_type *nested_type)
{
  char *name;
  gcc_type result;
  struct cleanup *cleanups;
  struct compile_cplus_context *pctx;
  VEC (compile_cplus_scope_def) *scopes = NULL;

  *nested_type = GCC_TYPE_NONE;
  pctx = XCNEW (struct compile_cplus_context);
  cleanups = make_cleanup (xfree, pctx);

  /* Break the type name into components.  If TYPE was defined in some
     superclass, we do not process TYPE but process the enclosing type
     instead.  */
  CONTEXT_SCOPES (pctx) = ccp_type_name_to_scopes (type_name);
  make_cleanup (VEC_cleanup (compile_cplus_scope_def),
		&CONTEXT_SCOPES (pctx));

  if (CONTEXT_SCOPES (pctx) != NULL)
    {
      struct compile_cplus_scope *scope, *cur_scope = NULL;
      struct compile_cplus_context *cur_context;

      cur_context = get_current_processing_context ();
      if (cur_context != NULL)
	cur_scope = get_processing_context_scope (cur_context);

      /* Get the name of the last component, which should be the
	 unqualified name of the type to process.  */
      scope = get_processing_context_scope (pctx);

      if (!types_equal (type, SYMBOL_TYPE (scope->bsymbol.symbol))
	  && (cur_scope == NULL
	      || cur_scope->bsymbol.symbol != scope->bsymbol.symbol))
	{
	  void **slot;
	  struct type_map_instance inst, *found;

	  /* The type the oracle asked about is defined inside another
	     class(es).  Convert that type instead of defining this type.  */
	  (void) convert_cplus_type (context,
				     SYMBOL_TYPE (scope->bsymbol.symbol));

	  /* If the original type (passed in to us) is defined in a nested
	     class, the previous call will give us that type's gcc_type.
	     Upper layers are expecting to get the original type's
	     gcc_type!  */
	  inst.type = type;
	  slot = htab_find_slot (context->type_map, &inst, NO_INSERT);
	  gdb_assert (*slot != NULL);
	  found = (struct type_map_instance *) *slot;
	  *nested_type = found->gcc_type_handle;
	  do_cleanups (cleanups);
	  return NULL;
	}
    }
  else
    {
      struct compile_cplus_scope scope;

      if (TYPE_NAME (type) == NULL)
	{
	  struct compile_cplus_context *current;

	  /* Anonymous type  */
	  name = NULL;
	  /* We don't have a qualified name for this to look up, but
	     we need a scope.  We have to assume, then, that it is the same
	     as the current scope, if any.  */
	  /* !!keiths: FIXME: This isn't quite right.  */
	  current = get_current_processing_context ();
	  if (current != NULL)
	    {
	      CONTEXT_SCOPES (pctx) = VEC_copy (compile_cplus_scope_def,
						CONTEXT_SCOPES (current));
	    }
	  else
	    {
	      scope.bsymbol.block = NULL;
	      scope.bsymbol.symbol = NULL;
	      scope.name.ptr = NULL;
	      scope.name.length = 0;
	      VEC_safe_push (compile_cplus_scope_def, CONTEXT_SCOPES (pctx),
			     &scope);
	    }
	}
      else
	{
	  scope.bsymbol = lookup_symbol (TYPE_NAME (type),
					 get_current_search_block (),
					 VAR_DOMAIN, NULL);
	  name = cp_func_name (TYPE_NAME (type));
	  make_cleanup (xfree, name);
	  scope.name.ptr = name;
	  scope.name.length = strlen (name);

	  VEC_safe_push (compile_cplus_scope_def, CONTEXT_SCOPES (pctx),
			 &scope);
	}
    }

  discard_cleanups (cleanups);
  return pctx;
}

/* Delete the processing context PCTX. */

void
delete_processing_context (struct compile_cplus_context *pctx)
{
  if (pctx != NULL)
    {
      VEC_free (compile_cplus_scope_def, CONTEXT_SCOPES (pctx));
      xfree (pctx);
    }
}

/* Convert a reference type to its gcc representation.  */

static gcc_type
ccp_convert_reference (struct compile_cplus_instance *context,
		       struct type *type)
{
  gcc_type target = convert_cplus_type (context, TYPE_TARGET_TYPE (type));

  /* !!keiths: GDB does not currently do anything with rvalue references.
     [Except set the type code to TYPE_CODE_ERROR!  */
  CPOPS ("build_reference_type %s\n", TYPE_SAFE_NAME (type));
  return CPCALL (build_reference_type, context, target, GCC_CP_REF_QUAL_LVALUE);
}

/* Convert a pointer type to its gcc representation.  */

static gcc_type
ccp_convert_pointer (struct compile_cplus_instance *context,
		     struct type *type)
{
  gcc_type target = convert_cplus_type (context, TYPE_TARGET_TYPE (type));

  CPOPS ("build_pointer_type %s\n", TYPE_SAFE_NAME (type));
  return CPCALL (build_pointer_type, context, target);
}

/* Convert an array type to its gcc representation.  */

static gcc_type
ccp_convert_array (struct compile_cplus_instance *context, struct type *type)
{
  gcc_type element_type;
  struct type *range = TYPE_INDEX_TYPE (type);

  element_type = convert_cplus_type (context, TYPE_TARGET_TYPE (type));

  if (TYPE_LOW_BOUND_KIND (range) != PROP_CONST)
    {
      const char *s = _("array type with non-constant"
			" lower bound is not supported");
      CPOPS ("error %s\n", s);
      return CPCALL (error, context, s);
    }

  if (TYPE_LOW_BOUND (range) != 0)
    {
      const char *s = _("cannot convert array type with "
			"non-zero lower bound to C");

      CPOPS ("error %s\n", s);
      return CPCALL (error, context, s);
    }

  if (TYPE_HIGH_BOUND_KIND (range) == PROP_LOCEXPR
      || TYPE_HIGH_BOUND_KIND (range) == PROP_LOCLIST)
    {
      gcc_type result;
      char *upper_bound;

      if (TYPE_VECTOR (type))
	{
	  const char *s = _("variably-sized vector type is not supported");

	  CPOPS ("error %s\n", s);
	  return CPCALL (error, context, s);
	}

      upper_bound = c_get_range_decl_name (&TYPE_RANGE_DATA (range)->high);
      CPOPS ("build_vla_array_type\n");
      result = CPCALL (build_vla_array_type, context,
		       element_type, upper_bound);
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
	{
	  CPOPS ("build_vector_type\n");
	  return CPCALL (build_vector_type, context, element_type, count);
	}

      CPOPS ("build_array_type\n");
      return CPCALL (build_array_type, context, element_type, count);
    }
}

/* Convert a typedef of TYPE.  */

static gcc_type
ccp_convert_typedef (struct compile_cplus_instance *context,
		     struct type *type)
{
  int need_new_context;
  gcc_type typedef_type, result;
  char *name;
  struct compile_cplus_scope *scope;
  struct compile_cplus_context *pctx;
  struct cleanup *cleanups;

  cleanups = make_cleanup (null_cleanup, NULL);
  pctx = new_processing_context (context, TYPE_NAME (type), type, &result);
  if (result != GCC_TYPE_NONE)
    {
      gdb_assert (pctx == NULL);
      do_cleanups (cleanups);
      return result;
    }

  if (TYPE_NAME (type) != NULL)
    {
      name = cp_func_name (TYPE_NAME (type));
      make_cleanup (xfree, name);
    }
  else
    name = NULL;

  need_new_context = ccp_need_new_context (pctx);
  if (need_new_context)
    ccp_push_processing_context (context, pctx);

  typedef_type = convert_cplus_type (context, check_typedef (type));

  CPOPS ("new_decl typedef %s, gcc_type = %lld\n", name, typedef_type);
  CPCALL (new_decl, context,
	  name,
	  GCC_CP_SYMBOL_TYPEDEF,
	  typedef_type,
	  0,
	  0,
	  /* !!keiths: Wow. More of this!  */
	  NULL,
	  0);

  if (need_new_context)
    ccp_pop_processing_context (context, pctx);

  delete_processing_context (pctx);
  do_cleanups (cleanups);
  return typedef_type;
}

/* Convert types defined in TYPE.  */

static void
ccp_convert_type_defns (struct compile_cplus_instance *context,
			struct type *type)
{
  int i;

  for (i = 0; i < TYPE_TYPE_DEFN_FIELD_COUNT (type); ++i)
    {
      const char *name = TYPE_TYPE_DEFN_FIELD_NAME (type, i);

      /* !!keiths: Is this "all" I need to do?? */
      (void) convert_cplus_type (context,
				 TYPE_TYPE_DEFN_FIELD_TYPE (type, i));
    }
}

/* Convert data members defined in TYPE, which should be struct/class/union
   with gcc_type COMP_TYPE.  */

static void
ccp_convert_struct_or_union_members (struct compile_cplus_instance *context,
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
	= convert_cplus_type (context, TYPE_FIELD_TYPE (type, i));

      if (field_is_static (&TYPE_FIELD (type, i)))
	{
	  switch (TYPE_FIELD_LOC_KIND (type, i))
	    {
	    case FIELD_LOC_KIND_PHYSADDR:
	      {
		physaddr = TYPE_FIELD_STATIC_PHYSADDR (type, i);

		CPOPS ("new_decl static variable %s at %s\n",
		       field_name, core_addr_to_string (physaddr));
		CPCALL (new_decl, context,
			field_name,
			GCC_CP_SYMBOL_VARIABLE,
			field_type,
			NULL,
			physaddr,
			// FIXME: do we have
			// location info for
			// static data members?
			// -lxo
			NULL,
			0);
	      }
	      break;

	    case FIELD_LOC_KIND_PHYSNAME:
	      {
		const char *physname = TYPE_FIELD_STATIC_PHYSNAME (type, i);
		struct block_symbol sym
		  = lookup_symbol (physname, get_current_search_block (),
				   VAR_DOMAIN, NULL);
		const char *filename = symbol_symtab (sym.symbol)->filename;
		unsigned int line = SYMBOL_LINE (sym.symbol);

		physaddr = SYMBOL_VALUE_ADDRESS (sym.symbol);
		CPOPS ("new_decl static variable from physname %s (%s)\n",
		       field_name, core_addr_to_string (physaddr));
		CPCALL (new_decl, context,
			field_name,
			GCC_CP_SYMBOL_VARIABLE,
			field_type,
			NULL,
			physaddr,
			filename,
			line);
	      }
	      break;

	    default:
	      gdb_assert_not_reached
		("unexpected static field location kind");
	    }
	}
      else
	{
	  /* !!keiths: I am guessing this is insufficient... */
	  unsigned long bitsize = TYPE_FIELD_BITSIZE (type, i);

	  if (bitsize == 0)
	    bitsize = 8 * TYPE_LENGTH (TYPE_FIELD_TYPE (type, i));

	  CPOPS ("new_field %s, gcc_type = %lld\n", field_name, field_type);
	  CPCALL (new_field, context,
		  comp_type,
		  field_name,
		  field_type,
		  bitsize,
		  TYPE_FIELD_BITPOS (type, i));
	}
    }
}

/* Convert a method type to its gcc representation.  */

static gcc_type
ccp_convert_method (struct compile_cplus_instance *context,
		    struct type *parent_type, struct type *method_type)
{
  int i;
  gcc_type result, func_type, class_type;
  gcc_cp_qualifiers_flags quals;
  gcc_cp_ref_qualifiers_flags rquals;

  /* Get the actual (proto)type of the method, as a function.  */
  func_type = ccp_convert_func (context, method_type, 1);

  class_type = convert_cplus_type (context, parent_type);
  quals = (enum gcc_cp_qualifiers) 0; // !!keiths FIXME
  rquals = GCC_CP_REF_QUAL_NONE; // !!keiths FIXME
  CPOPS ("build_method_type\n");
  result = CPCALL (build_method_type, context,
		   class_type, func_type, quals, rquals);
  CPOPS ("\tgcc_type = %lld\n", result);
  return result;
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

  //printf ("hash for %s is %x\n", op, hash);
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

/* See compile-internal.h.  */

#define DEBUG_XTOR 0
const char *
maybe_canonicalize_special_function (const char *field_name,
				     const struct fn_field *method_field,
				     const struct type *method_type,
				     char **outname, int *ignore)
{
  /* We assume that no method is to be ignored.  */
  *ignore = 0;

  /* We only consider ctors and dtors if METHOD_FIELD is non-NULL.  */
  /* !!keiths: Ick.  Maybe we can look it up here instead if it is NULL?  */
  if (method_field != NULL)
    {
      if (method_field->is_constructor)
	{
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
#if DEBUG_XTOR
	      printf ("complete_object_allocating_ctor (C1)\n");
#endif
	      return "C1";
	    case unified_ctor:
#if DEBUG_XTOR
	      printf ("unified_ctor (C4 -- ignore)\n");
#endif
	      *ignore = 1;
	      return field_name; /* C4  */
	    case object_ctor_group:
#if DEBUG_XTOR
	      printf ("object_ctor_group (C? -- ignore)\n");
#endif
	      *ignore = 1;
	      return field_name; /* C1 ??  */

	    default:
	      gdb_assert_not_reached ("unknown constructor kind");
	    }
	}
      else if (method_field->is_destructor)
	{
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
	      printf ("unified_dtor (D4 -- ignore)\n");
#endif
	      *ignore = 1;
	      return field_name; /* D4  */
	    case object_dtor_group:
#if DEBUG_XTOR
	      printf ("object_dtor_group (D? -- ignore)\n");
#endif
	      *ignore = 1;
	      return field_name; /* D1 ??  */

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
      (*outname)[len] = '\0';
      //printf ("GOT LITERAL OPERATOR: %s\n", *outname);
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
ccp_convert_struct_or_union_methods (struct compile_cplus_instance *context,
				     struct type *type, gcc_type class_type)
{
  int i;

  for (i = 0; i < TYPE_NFN_FIELDS (type); ++i)
    {
      int j;
      struct fn_field *methods = TYPE_FN_FIELDLIST1 (type, i);
      const char *overloaded_name = TYPE_FN_FIELDLIST_NAME (type, i);

      /* Loop through the fieldlist, adding decls to the compiler's
	 representation of the class.  */
      for (j = 0; j < TYPE_FN_FIELDLIST_LENGTH (type, i); ++j)
	{
#if 0
	  int qual_flags = 0;
	  /* pmuldoon: Need to detect LVALUE/RVALUE Qualifiers here. */
	  int ref_qual_flags = GCC_CP_REF_QUAL_NONE;
	  gcc_type prototype;

	  /* Skip artificial methods (for now?) */
	  if (! TYPE_FN_FIELD_ARTIFICIAL (methods, j))
	    {
	      struct type *temp_type = TYPE_FN_FIELD_TYPE (methods, j);

	      /* Convert to a function, first. */
	      prototype = convert_cplus_type (context, temp_type);

	      if (TYPE_CONST (temp_type))
		qual_flags |= GCC_CP_QUALIFIER_CONST;
	      if (TYPE_VOLATILE (temp_type))
		qual_flags |= GCC_CP_QUALIFIER_VOLATILE;
	      if (TYPE_RESTRICT (temp_type))
		qual_flags |= GCC_CP_QUALIFIER_RESTRICT;

	      CPOPS ("build_method_type\n");
	      CPCALL (build_method_type, context,
		      class_type, prototype, qual_flags, ref_qual_flags);
	    }
#else
	  CORE_ADDR address;
	  gcc_type method_type;
	  struct block_symbol sym;
	  const char *filename;
	  unsigned int line;
	  const char *kind; //debug
	  gcc_cp_symbol_kind_flags sym_kind = GCC_CP_SYMBOL_FUNCTION;
	  const char *name;
	  char *special_name;
	  struct cleanup *back_to;
	  int ignore;

	  /* Skip artificial methods.  */
	  if (TYPE_FN_FIELD_ARTIFICIAL (methods, j))
	    continue;

	  special_name = NULL;
	  name = maybe_canonicalize_special_function (overloaded_name,
						      &methods[j],
						      methods[j].type,
						      &special_name,
						      &ignore);
	  /* !!keiths: I don't know if this is really necessary...  */
	  if (ignore)
	    {
	      //printf ("ignoring method %s\n", methods[j].physname);
	      continue;
	    }

	  back_to = make_cleanup (null_cleanup, NULL);

	  if (special_name != NULL)
	    {
	      make_cleanup (xfree, special_name);
	      name = special_name;
	    }
	  if (name != overloaded_name)
	    {
	      //printf ("SPECIAL FUNCTION %s\n", name);
	      sym_kind |= GCC_CP_FLAG_SPECIAL_FUNCTION;
	    }
	  sym = lookup_symbol (TYPE_FN_FIELD_PHYSNAME (methods, j),
			       get_current_search_block (), VAR_DOMAIN, NULL);

	  if (sym.symbol == NULL)
	    {
	      if (TYPE_FN_FIELD_VIRTUAL_P (methods, j))
		{
		  /* !!keiths: This is beyond hacky, and is really only a
		     lame workaround for detecting pure virtual methods.  */
		  method_type
		    = ccp_convert_method (context, type,
					  TYPE_FN_FIELD_TYPE (methods, j));
		  CPOPS ("new_decl pure virtual method %s\n", name);
		  CPCALL (new_decl, context,
			  name,
			  (sym_kind
			   | GCC_CP_FLAG_VIRTUAL_FUNCTION
			   | GCC_CP_FLAG_PURE_VIRTUAL_FUNCTION),
			  method_type,
			  NULL,
			  0,
			  NULL,
			  0);
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

	  /* All class methods have TYPE_CODE_METHOD, including static
	     methods.  For the plug-in, though, these use different calls.  */
	  if (TYPE_FN_FIELD_STATIC_P (methods, j))
	    {
	      kind = " static";
	      method_type
		= ccp_convert_func (context,
				    TYPE_FN_FIELD_TYPE (methods, j), 0);
	    }
	  else
	    {
	      kind = "";
	      method_type
		= ccp_convert_method (context, type,
				      TYPE_FN_FIELD_TYPE (methods, j));

	      if (TYPE_FN_FIELD_VIRTUAL_P (methods, j))
		sym_kind |= GCC_CP_FLAG_VIRTUAL_FUNCTION;
	    }

	  /* !!keiths: Is this sufficient?  */
	  address = BLOCK_START (SYMBOL_BLOCK_VALUE (sym.symbol));

	  CPOPS ("new_decl%s method %s at %s\n", kind, name,
		 core_addr_to_string (address));
	  CPCALL (new_decl, context,
		  name,
		  sym_kind,
		  method_type,
		  NULL,
		  address,
		  filename,
		  line);
	  do_cleanups (back_to);
#endif
	}
    }
}

/* Convert a struct or union type to its gcc representation.  */

static gcc_type
ccp_convert_struct_or_union (struct compile_cplus_instance *context,
			     struct type *type)
{
  int i, need_new_context;
  gcc_type result;
  struct compile_cplus_scope *scope;
  struct compile_cplus_context *pctx;
  char *name = NULL;
  const char *filename = NULL;
  unsigned short line = 0;
  struct cleanup *cleanups;

  /* Create an empty cleanup chain.  */
  cleanups = make_cleanup (null_cleanup, NULL);

  /* Create a new processing context for TYPE.  */
  pctx = new_processing_context (context, TYPE_NAME (type), type, &result);
  if (result != GCC_TYPE_NONE)
    {
      /* The type requested was actually defined inside another type,
	 such as a nested class definition.  Return that type.  */
      gdb_assert (pctx == NULL);
      do_cleanups (cleanups);
      return result;
    }

  if (TYPE_NAME (type) != NULL)
    {
      name = cp_func_name (TYPE_NAME (type));
      make_cleanup (xfree, name);
    }
  else
    {
      /* !!keiths: Wow.  */
      if (TYPE_CODE (type) == TYPE_CODE_STRUCT)
	name = "anonymous struct";
      else
	name = "anonymous union";
    }

  /* Push all scopes.  */
  need_new_context = ccp_need_new_context (pctx);
  if (need_new_context)
    {
#if CONTEXT_DEBUG
      printf ("entering new processing scope %p\n", pctx);
#endif
      ccp_push_processing_context (context, pctx);
    }
#if CONTEXT_DEBUG
  else
    printf ("staying in current scope -- scopes are identical\n");
#endif

  /* First we create the resulting type and enter it into our hash
     table.  This lets recursive types work.  */
  if (TYPE_CODE (type) == TYPE_CODE_STRUCT)
    {
      struct gcc_vbase_array bases;
      int num_baseclasses = TYPE_N_BASECLASSES (type);

      memset (&bases, 0, sizeof (bases));

      if (num_baseclasses > 0)
	{
	  bases.elements = XNEWVEC (gcc_type, num_baseclasses);
	  bases.n_elements = num_baseclasses;
	  bases.virtualp = (char *) xcalloc (num_baseclasses, sizeof (char));
	  for (i = 0; i < num_baseclasses; ++i)
	    {
	      struct type *base_type = TYPE_BASECLASS (type, i);

	      if (BASETYPE_VIA_VIRTUAL (type, i))
		bases.virtualp[i] = 1;
	      bases.elements[i] = convert_cplus_type (context, base_type);
	    }
	}

      CPOPS ("start_new_class_type %s\n", name);
      result = CPCALL (start_new_class_type, context,
		       name, &bases, filename, line);
      CPOPS ("\tgcc_type = %lld\n", result);
      xfree (bases.virtualp);
      xfree (bases.elements);
    }
  else
    {
      gdb_assert (TYPE_CODE (type) == TYPE_CODE_UNION);
      CPOPS ("start_new_union_type %s\n", name);
      result = CPCALL (start_new_union_type, context, name, filename, line);
      CPOPS ("\tgcc_type = %lld\n", result);
    }
  insert_type (context, type, result);

  /* Add definitions.  */
  ccp_convert_type_defns (context, type);

  /* Add methods.  */
  ccp_convert_struct_or_union_methods (context, type, result);

  /* Add members.  */
  ccp_convert_struct_or_union_members (context, type, result);

  /* All finished.  */
  CPOPS ("finish_record_or_union %s (%lld)\n", name, result);
  CPCALL (finish_record_or_union, context, result, TYPE_LENGTH (type));

  /* Pop all scopes.  */
  if (need_new_context)
    {
#if CONTEXT_DEBUG
      printf ("leaving processing scope %p\n", pctx);
#endif
      ccp_pop_processing_context (context, pctx);
    }
#if CONTEXT_DEBUG
  else
    printf ("identical contexts -- not leaving context\n");
#endif

  delete_processing_context (pctx);

  do_cleanups (cleanups);
  return result;
}

/* Convert an enum type to its gcc representation.  */

static gcc_type
ccp_convert_enum (struct compile_cplus_instance *context, struct type *type)
{
  int i, need_new_context;
  gcc_type int_type, result;
  struct compile_cplus_scope *scope;
  struct compile_cplus_context *pctx;
  char *name = NULL;
  const char *filename = NULL;
  unsigned short line = 0;
  struct cleanup *cleanups;
  /* !!keiths: This does not appear to work. GCC complains about
     being unable to convert enum values from '(MyEnum)0' to 'int'.  */
  int scoped_enum_p = /*TYPE_DECLARED_CLASS (type) ? TRUE :*/ FALSE;

  /* Create an empty cleanup chain.  */
  cleanups = make_cleanup (null_cleanup, NULL);

  /* Create a new processing context for TYPE.  */
  pctx = new_processing_context (context, TYPE_NAME (type), type, &result);
  if (result != GCC_TYPE_NONE)
    {
      /* The type requested was actually defined inside another type,
	 such as a nested class definition.  Return that type.  */
      gdb_assert (pctx == NULL);
      do_cleanups (cleanups);
      return result;
    }

  if (TYPE_NAME (type) != NULL)
    {
      name = cp_func_name (TYPE_NAME (type));
      make_cleanup (xfree, name);
    }
  else
    {
      /* !!keiths: Wow.  */
      name = "anonymous enum";
    }

  /* Push all scopes.  */
  need_new_context = ccp_need_new_context (pctx);
  if (need_new_context)
    {
#if CONTEXT_DEBUG
      printf ("entering new processing scope %p\n", pctx);
#endif
      ccp_push_processing_context (context, pctx);
    }
#if CONTEXT_DEBUG
  else
    printf ("staying in current scope -- scopes are identical\n");
#endif

  // FIXME: drop any namespaces and enclosing class names, if any. -lxo
  /* !!keiths: Why?  Drop or push, just like convert_struct_or_union?  */

  CPOPS ("int_type %d %d\n", TYPE_UNSIGNED (type), TYPE_LENGTH (type));
  int_type = CPCALL (int_type, context,
		     TYPE_UNSIGNED (type), TYPE_LENGTH (type));
  CPOPS ("\tgcc_type = %lld\n", int_type);

  CPOPS ("start_new_enum_type %s\n", name);
  result = CPCALL (start_new_enum_type, context,
		   name, int_type, scoped_enum_p, filename, line);
  CPOPS ("\tgcc_type = %lld\n", result);

  for (i = 0; i < TYPE_NFIELDS (type); ++i)
    {
      char *fname = cp_func_name (TYPE_FIELD_NAME (type, i));

      if (TYPE_FIELD_LOC_KIND (type, i) != FIELD_LOC_KIND_ENUMVAL
	  || fname == NULL)
	continue;

      CPOPS ("build_add_enum_constant %s = %ld\n", fname,
	     TYPE_FIELD_ENUMVAL (type, i));
      CPCALL (build_add_enum_constant, context,
	      result, fname, TYPE_FIELD_ENUMVAL (type, i));
      xfree (fname);
    }

  CPOPS ("finish_enum_type %s (%lld)\n", name, result);
  CPCALL (finish_enum_type, context, result);

  /* Pop all scopes.  */
  if (need_new_context)
    {
#if CONTEXT_DEBUG
      printf ("leaving processing scope %p\n", pctx);
#endif
      ccp_pop_processing_context (context, pctx);
    }
#if CONTEXT_DEBUG
  else
    printf ("identical contexts -- not leaving context\n");
#endif

  /* Delete the processing context.  */
  delete_processing_context (pctx);

  do_cleanups (cleanups);
  return result;
}

/* Convert a function type to its gcc representation.  */

static gcc_type
ccp_convert_func (struct compile_cplus_instance *context, struct type *type,
		  int strip_artificial)
{
  int i, artificials;
  gcc_type result, return_type;
  struct gcc_type_array array;
  /* !!keiths: This doesn't always work, unfortunately.  When we have a
     pure virtual method, TYPE_PROTOTYPED == 0.
     But this *may* be needed for several gdb.compile tests.  Or at least
     indicate other unresolved bugs in this file or elsewhere in gdb.  */
  int is_varargs = TYPE_VARARGS (type) /*|| !TYPE_PROTOTYPED (type)*/;

  /* This approach means we can't make self-referential function
     types.  Those are impossible in C, though.  */
  return_type = convert_cplus_type (context, TYPE_TARGET_TYPE (type));

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
	    = convert_cplus_type (context, TYPE_FIELD_TYPE (type, i));
	}
    }

  /* We omit setting the argument types to `void' to be a little flexible
     with some minsyms like printf (compile-cplus.exp has examples).  */
  CPOPS ("build_function_type %s\n", TYPE_SAFE_NAME (type));
  result = CPCALL (build_function_type, context,
		   return_type, &array, is_varargs);
  xfree (array.elements);

  return result;
}

/* Convert an integer type to its gcc representation.  */

static gcc_type
ccp_convert_int (struct compile_cplus_instance *context, struct type *type)
{
  gcc_type result;

  CPOPS ("int_type %d %d\n", TYPE_LENGTH (type), TYPE_UNSIGNED (type));
  result = CPCALL (int_type, context, TYPE_UNSIGNED (type), TYPE_LENGTH (type));
  CPOPS ("\tgcc_type = %lld\n", result);
  return result;
}

/* Convert a floating-point type to its gcc representation.  */

static gcc_type
ccp_convert_float (struct compile_cplus_instance *context, struct type *type)
{
  gcc_type result;

  CPOPS ("float_type %s\n", TYPE_SAFE_NAME (type));
  result = CPCALL (float_type, context, TYPE_LENGTH (type));
  CPOPS ("\tgcc_type = %lld\n", result);
  return result;
}

/* Convert the 'void' type to its gcc representation.  */

static gcc_type
ccp_convert_void (struct compile_cplus_instance *context, struct type *type)
{
  gcc_type result;

  CPOPS ("void_type\n");
  result = CPCALL (void_type, context);
  CPOPS ("\tgcc_type = %lld\n", result);
  return result;
}

/* Convert a boolean type to its gcc representation.  */

static gcc_type
ccp_convert_bool (struct compile_cplus_instance *context, struct type *type)
{
  gcc_type result;

  CPOPS ("bool_type %s\n", TYPE_SAFE_NAME (type));
  result = CPCALL (bool_type, context);
  CPOPS ("\tgcc_type = %lld\n", result);
  return result;
}

/* Convert a qualified type to its gcc representation.  */

static gcc_type
ccp_convert_qualified (struct compile_cplus_instance *context,
		       struct type *type)
{
  struct type *unqual = make_unqualified_type (type);
  gcc_type unqual_converted;
  gcc_cp_qualifiers_flags quals = (enum gcc_cp_qualifiers) 0;
  gcc_type result;

  unqual_converted = convert_cplus_type (context, unqual);

  if (TYPE_CONST (type))
    quals |= GCC_CP_QUALIFIER_CONST;
  if (TYPE_VOLATILE (type))
    quals |= GCC_CP_QUALIFIER_VOLATILE;
  if (TYPE_RESTRICT (type))
    quals |= GCC_CP_QUALIFIER_RESTRICT;

  CPOPS ("build_qualified_type %s\n", TYPE_SAFE_NAME (type));
  result = CPCALL (build_qualified_type, context, unqual_converted, quals);
  CPOPS ("\tgcc_type = %lld\n", result);
  return result;
}

/* Convert a complex type to its gcc representation.  */

static gcc_type
ccp_convert_complex (struct compile_cplus_instance *context,
		     struct type *type)
{
  gcc_type result;
  gcc_type base = convert_cplus_type (context, TYPE_TARGET_TYPE (type));

  CPOPS ("build_complex_type %s\n", TYPE_SAFE_NAME (type));
  result = CPCALL (build_complex_type, context, base);
  CPOPS ("\tgcc_type = %lld\n", result);
  return result;
}

/* Convert a namespace of TYPE.  */

static gcc_type
ccp_convert_namespace (struct compile_cplus_instance *context,
		       struct type *type)
{
  int need_new_context;
  gcc_type dummy;
  char *name = NULL;
  struct compile_cplus_scope *scope;
  struct compile_cplus_context *pctx;
  struct cleanup *cleanups;

  /* !!keiths: I don't think we can define namespaces inside other types,
     so the only way to get here is to be defining either a global namespace
     or something defined via namespaces all in the global namespace.  */

  cleanups = make_cleanup (null_cleanup, NULL);
  pctx = new_processing_context (context, TYPE_NAME (type), type, &dummy);

  if (TYPE_NAME (type) != NULL)
    {
      name = cp_func_name (TYPE_NAME (type));
      make_cleanup (xfree, name);
    }

  need_new_context = ccp_need_new_context (pctx);
  if (need_new_context)
    ccp_push_processing_context (context, pctx);

  CPOPS ("push_namesapce %s\n", name);
  CPCALL (push_namespace, context, name);

  CPOPS ("pop_namespace %s\n", name);
  CPCALL (pop_namespace, context);

  if (need_new_context)
    ccp_pop_processing_context (context, pctx);

  delete_processing_context (pctx);
  do_cleanups (cleanups);

  return DONT_CACHE_TYPE;
}

/* A helper function which knows how to convert most types from their
   gdb representation to the corresponding gcc form.  This examines
   the TYPE and dispatches to the appropriate conversion function.  It
   returns the gcc type.  */

static gcc_type
convert_type_cplus_basic (struct compile_cplus_instance *context,
			  struct type *type)
{
  /* Reference types seem to always have a const qualifier, but we
     don't want that to be propagated to the GCC type, because GCC
     doesn't like the reference types themselves to be qualified.  */
  if (TYPE_CODE (type) == TYPE_CODE_REF)
    return ccp_convert_reference (context, type);

  /* If we are converting a qualified type, first convert the
     unqualified type and then apply the qualifiers.  */
  if ((TYPE_INSTANCE_FLAGS (type) & (TYPE_INSTANCE_FLAG_CONST
				     | TYPE_INSTANCE_FLAG_VOLATILE
				     | TYPE_INSTANCE_FLAG_RESTRICT)) != 0)
    return ccp_convert_qualified (context, type);

  switch (TYPE_CODE (type))
    {
#if 0
    case TYPE_CODE_REF:
      return ccp_convert_reference (context, type);
#endif

    case TYPE_CODE_PTR:
      return ccp_convert_pointer (context, type);

    case TYPE_CODE_ARRAY:
      return ccp_convert_array (context, type);

    case TYPE_CODE_STRUCT:
    case TYPE_CODE_UNION:
      return ccp_convert_struct_or_union (context, type);

    case TYPE_CODE_ENUM:
      return ccp_convert_enum (context, type);

    case TYPE_CODE_FUNC:
      return ccp_convert_func (context, type, 0);

#if 0
    case TYPE_CODE_METHOD:
      return ccp_convert_method (context, type);
#endif

    case TYPE_CODE_INT:
      return ccp_convert_int (context, type);

    case TYPE_CODE_FLT:
      return ccp_convert_float (context, type);

    case TYPE_CODE_VOID:
      return ccp_convert_void (context, type);

    case TYPE_CODE_BOOL:
      return ccp_convert_bool (context, type);

    case TYPE_CODE_COMPLEX:
      return ccp_convert_complex (context, type);

    case TYPE_CODE_NAMESPACE:
      return ccp_convert_namespace (context, type);

    case TYPE_CODE_TYPEDEF:
      return ccp_convert_typedef (context, type);
    }

  {
    char *s = xstrprintf (_("unhandled TYPE_CODE_%s"),
			  type_code_to_string (TYPE_CODE (type)));

    CPOPS ("error %s\n", s);
    return CPCALL (error, context, s);
    xfree (s);
  }
}

/* See compile-internal.h.  */

gcc_type
convert_cplus_type (struct compile_cplus_instance *context,
		    struct type *type)
{
  struct type_map_instance inst, *found;
  gcc_type result;

  inst.type = type;
  found = (struct type_map_instance *) htab_find (context->type_map, &inst);
  if (found != NULL)
    return found->gcc_type_handle;

  result = convert_type_cplus_basic (context, type);
  if (result != DONT_CACHE_TYPE)
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
}
