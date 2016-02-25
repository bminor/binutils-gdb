/* Convert symbols from GDB to GCC

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
#include "compile-internal.h"
#include "gdb_assert.h"
#include "symtab.h"
#include "parser-defs.h"
#include "block.h"
#include "objfiles.h"
#include "compile.h"
#include "value.h"
#include "exceptions.h"
#include "gdbtypes.h"
#include "dwarf2loc.h"
#include "cp-support.h"


#define DEBUG 0
#define DEBUG_ORACLE 0

/* Object of this type are stored in the compiler's symbol_err_map.  */

struct symbol_error
{
  /* The symbol.  */

  const struct symbol *sym;

  /* The error message to emit.  This is malloc'd and owned by the
     hash table.  */

  char *message;
};

/* Hash function for struct symbol_error.  */

static hashval_t
hash_symbol_error (const void *a)
{
  const struct symbol_error *se = (struct symbol_error *) a;

  return htab_hash_pointer (se->sym);
}

/* Equality function for struct symbol_error.  */

static int
eq_symbol_error (const void *a, const void *b)
{
  const struct symbol_error *sea = (struct symbol_error *) a;
  const struct symbol_error *seb = (struct symbol_error *) b;

  return sea->sym == seb->sym;
}

/* Deletion function for struct symbol_error.  */

static void
del_symbol_error (void *a)
{
  struct symbol_error *se = (struct symbol_error *) a;

  xfree (se->message);
  xfree (se);
}

/* Associate SYMBOL with some error text.  */

static void
insert_symbol_error (htab_t hash, const struct symbol *sym, const char *text)
{
  struct symbol_error e;
  void **slot;

  e.sym = sym;
  slot = htab_find_slot (hash, &e, INSERT);
  if (*slot == NULL)
    {
      struct symbol_error *e = XNEW (struct symbol_error);

      e->sym = sym;
      e->message = xstrdup (text);
      *slot = e;
    }
}

/* Emit the error message corresponding to SYM, if one exists, and
   arrange for it not to be emitted again.  */

static void
error_symbol_once (struct compile_cplus_instance *context,
		   const struct symbol *sym)
{
  struct symbol_error search;
  struct symbol_error *err;
  char *message;

  if (context->symbol_err_map == NULL)
    return;

  search.sym = sym;
  err = (struct symbol_error *) htab_find (context->symbol_err_map, &search);
  if (err == NULL || err->message == NULL)
    return;

  message = err->message;
  err->message = NULL;
  make_cleanup (xfree, message);
  error (_("%s"), message);
}



/* Compute the name of the pointer representing a local symbol's
   address.  */

static char *
symbol_substitution_name (struct symbol *sym)
{
  return concat ("__", SYMBOL_NATURAL_NAME (sym), "_ptr", (char *) NULL);
}

/* Convert a given symbol, SYM, to the compiler's representation.
   CONTEXT is the compiler instance.  IS_GLOBAL is true if the
   symbol came from the global scope.  IS_LOCAL is true if the symbol
   came from a local scope.  (Note that the two are not strictly
   inverses because the symbol might have come from the static
   scope.)  */

static void
convert_one_symbol (struct compile_cplus_instance *context,
		    struct block_symbol sym,
		    int is_global,
		    int is_local)
{
  gcc_type sym_type;
  const char *filename = symbol_symtab (sym.symbol)->filename;
  unsigned short line = SYMBOL_LINE (sym.symbol);

  error_symbol_once (context, sym.symbol);

  if (SYMBOL_CLASS (sym.symbol) == LOC_LABEL)
    sym_type = 0;
  else
    sym_type = convert_cplus_type (context, SYMBOL_TYPE (sym.symbol));

  if (SYMBOL_DOMAIN (sym.symbol) == STRUCT_DOMAIN)
    {
      /* Nothing to do.  */
    }
  else
    {
      gcc_decl decl;
      /* Squash compiler warning.  */
      gcc_cp_symbol_kind_flags kind = GCC_CP_FLAG_BASE;
      CORE_ADDR addr = 0;
      char *symbol_name = NULL, *name = NULL;

      switch (SYMBOL_CLASS (sym.symbol))
	{
	case LOC_TYPEDEF:
	  if (TYPE_CODE (SYMBOL_TYPE (sym.symbol)) == TYPE_CODE_TYPEDEF)
	    kind = GCC_CP_SYMBOL_TYPEDEF;
	  else  if (TYPE_CODE (SYMBOL_TYPE (sym.symbol)) == TYPE_CODE_NAMESPACE)
	    return;
	  break;

	case LOC_LABEL:
	  kind = GCC_CP_SYMBOL_LABEL;
	  addr = SYMBOL_VALUE_ADDRESS (sym.symbol);
	  break;

	case LOC_BLOCK:
	  {
	    int ignore;
	    char *special_name;
	    const char *func_name;
	    struct fn_field *field;

	    kind = GCC_CP_SYMBOL_FUNCTION;
	    addr = BLOCK_START (SYMBOL_BLOCK_VALUE (sym.symbol));
	    if (is_global && TYPE_GNU_IFUNC (SYMBOL_TYPE (sym.symbol)))
	      addr = gnu_ifunc_resolve_addr (target_gdbarch (), addr);

	    special_name = NULL;
	    field = cp_find_method_field (sym.symbol, 1);
	    func_name = maybe_canonicalize_special_function
	      (SYMBOL_LINKAGE_NAME (sym.symbol), NULL,
	       SYMBOL_TYPE (sym.symbol), &special_name, &ignore);
	    if (special_name != NULL)
	      {
		kind |= GCC_CP_FLAG_SPECIAL_FUNCTION;
		name = special_name;
	      }
	    else if (func_name != SYMBOL_NATURAL_NAME (sym.symbol))
	      {
		kind |= GCC_CP_FLAG_SPECIAL_FUNCTION;
		name = xstrdup (func_name);
	      }
	    else if (ignore)
	      {
		/* !!keiths: I don't think we can get here, can we?  */
		gdb_assert_not_reached ("told to ignore method!");
	      }
	  }
	  break;

	case LOC_CONST:
	  if (TYPE_CODE (SYMBOL_TYPE (sym.symbol)) == TYPE_CODE_ENUM)
	    {
	      /* Already handled by convert_enum.  */
	      return;
	    }
	  CPOPS ("build_constant %s\n", SYMBOL_NATURAL_NAME (sym.symbol));
	  CPCALL (build_constant, context,
		  sym_type, SYMBOL_NATURAL_NAME (sym.symbol),
		  SYMBOL_VALUE (sym.symbol),
		  filename, line);
	  return;

	case LOC_CONST_BYTES:
	  error (_("Unsupported LOC_CONST_BYTES for symbol \"%s\"."),
		 SYMBOL_PRINT_NAME (sym.symbol));

	case LOC_UNDEF:
	  internal_error (__FILE__, __LINE__, _("LOC_UNDEF found for \"%s\"."),
			  SYMBOL_PRINT_NAME (sym.symbol));

	case LOC_COMMON_BLOCK:
	  error (_("Fortran common block is unsupported for compilation "
		   "evaluaton of symbol \"%s\"."),
		 SYMBOL_PRINT_NAME (sym.symbol));

	case LOC_OPTIMIZED_OUT:
	  error (_("Symbol \"%s\" cannot be used for compilation evaluation "
		   "as it is optimized out."),
		 SYMBOL_PRINT_NAME (sym.symbol));

	case LOC_COMPUTED:
	  if (is_local)
	    goto substitution;
	  /* Probably TLS here.  */
	  warning (_("Symbol \"%s\" is thread-local and currently can only "
		     "be referenced from the current thread in "
		     "compiled code."),
		   SYMBOL_PRINT_NAME (sym.symbol));
	  /* FALLTHROUGH */
	case LOC_UNRESOLVED:
	  /* 'symbol_name' cannot be used here as that one is used only for
	     local variables from compile_dwarf_expr_to_c.
	     Global variables can be accessed by GCC only by their address, not
	     by their name.  */
	  {
	    struct value *val;
	    struct frame_info *frame = NULL;

	    if (symbol_read_needs_frame (sym.symbol))
	      {
		frame = get_selected_frame (NULL);
		if (frame == NULL)
		  error (_("Symbol \"%s\" cannot be used because "
			   "there is no selected frame"),
			 SYMBOL_PRINT_NAME (sym.symbol));
	      }

	    val = read_var_value (sym.symbol, sym.block, frame);
	    if (VALUE_LVAL (val) != lval_memory)
	      error (_("Symbol \"%s\" cannot be used for compilation "
		       "evaluation as its address has not been found."),
		     SYMBOL_PRINT_NAME (sym.symbol));

	    kind = GCC_CP_SYMBOL_VARIABLE;
	    addr = value_address (val);
	  }
	  break;


	case LOC_REGISTER:
	case LOC_ARG:
	case LOC_REF_ARG:
	case LOC_REGPARM_ADDR:
	case LOC_LOCAL:
	substitution:
	  kind = GCC_CP_SYMBOL_VARIABLE;
	  symbol_name = symbol_substitution_name (sym.symbol);
	  break;

	case LOC_STATIC:
	  kind = GCC_CP_SYMBOL_VARIABLE;
	  addr = SYMBOL_VALUE_ADDRESS (sym.symbol);
	  break;

	case LOC_FINAL_VALUE:
	default:
	  gdb_assert_not_reached ("Unreachable case in convert_one_symbol.");

	}

      /* Don't emit local variable decls for a raw expression.  */
      if (context->base.scope != COMPILE_I_RAW_SCOPE
	  || symbol_name == NULL)
	{
	  int need_new_context = 0;
	  struct compile_cplus_context *pctx = NULL;

	  /* For non-local symbols, create/push a new processing context
	     so that the symbol is properly scoped to the plug-in.  */
	  if (!is_local)
	    {
	      gcc_type dummy;

	      pctx = new_processing_context (context,
					     SYMBOL_NATURAL_NAME (sym.symbol),
					     SYMBOL_TYPE (sym.symbol),
					     &dummy);
	      if (dummy != GCC_TYPE_NONE)
		{
		  /* We found a symbol for this type that was defined inside
		     some other symbol, e.g., a class tyepdef defined.
		     Don't return anything in that case because that really
		     confuses users.  */
		  xfree (symbol_name);
		  xfree (name);
		  delete_processing_context (pctx);
		  return;
		}

	      need_new_context = ccp_need_new_context (pctx);
	      if (need_new_context)
		ccp_push_processing_context (context, pctx);
	    }

	  /* Get the `raw' name of the symbol.  */
	  if (name == NULL && SYMBOL_NATURAL_NAME (sym.symbol) != NULL)
	    {
	      name
		= cp_func_name (SYMBOL_NATURAL_NAME (sym.symbol));
	    }

	  /* Define the decl.  */
	  CPOPS ("new_decl %s %d %s\n", name, (int) kind, symbol_name);
	  decl = CPCALL (new_decl, context,
			 name, kind, sym_type, symbol_name, addr,
			 filename, line);

	  /* Pop any processing context.  */
	  if (need_new_context)
	    ccp_pop_processing_context (context, pctx);

	  delete_processing_context (pctx);
	}

      xfree (symbol_name);
      xfree (name);
    }
}

/* Convert a full symbol to its gcc form.  CONTEXT is the compiler to
   use, IDENTIFIER is the name of the symbol, SYM is the symbol
   itself, and DOMAIN is the domain which was searched.  */

static void
convert_symbol_sym (struct compile_cplus_instance *context,
		    const char *identifier, struct block_symbol sym,
		    domain_enum domain)
{
  const struct block *static_block;
  int is_local_symbol;

  /* If we found a symbol and it is not in the  static or global
     scope, then we should first convert any static or global scope
     symbol of the same name.  This lets this unusual case work:

     int x; // Global.
     int func(void)
     {
     int x;
     // At this spot, evaluate "extern int x; x"
     }
  */

  static_block = block_static_block (sym.block);
  /* STATIC_BLOCK is NULL if FOUND_BLOCK is the global block.  */
  is_local_symbol = (sym.block != static_block && static_block != NULL);
  if (is_local_symbol)
    {
      struct block_symbol global_sym;

      global_sym = lookup_symbol (identifier, NULL, domain, NULL);
      /* If the outer symbol is in the static block, we ignore it, as
	 it cannot be referenced.  */
      if (global_sym.symbol != NULL
	  && global_sym.block != block_static_block (global_sym.block))
	{
	  if (compile_debug)
	    fprintf_unfiltered (gdb_stdout,
				"gcc_convert_symbol \"%s\": global symbol\n",
				identifier);
	  convert_one_symbol (context, global_sym, 1, 0);
	}
    }

  if (compile_debug)
    fprintf_unfiltered (gdb_stdout,
			"gcc_convert_symbol \"%s\": local symbol\n",
			identifier);
  convert_one_symbol (context, sym, 0, is_local_symbol);
}

/* Convert a minimal symbol to its gcc form.  CONTEXT is the compiler
   to use and BMSYM is the minimal symbol to convert.  */

static void
convert_symbol_bmsym (struct compile_cplus_instance *context,
		      struct bound_minimal_symbol bmsym)
{
  struct minimal_symbol *msym = bmsym.minsym;
  struct objfile *objfile = bmsym.objfile;
  struct type *type;
  gcc_cp_symbol_kind_flags kind;
  gcc_type sym_type;
  gcc_decl decl;
  CORE_ADDR addr;

  addr = MSYMBOL_VALUE_ADDRESS (objfile, msym);

  /* Conversion copied from write_exp_msymbol.  */
  switch (MSYMBOL_TYPE (msym))
    {
    case mst_text:
    case mst_file_text:
    case mst_solib_trampoline:
      type = objfile_type (objfile)->nodebug_text_symbol;
      kind = GCC_CP_SYMBOL_FUNCTION;
      break;

    case mst_text_gnu_ifunc:
      /* nodebug_text_gnu_ifunc_symbol would cause:
	 function return type cannot be function  */
      type = objfile_type (objfile)->nodebug_text_symbol;
      kind = GCC_CP_SYMBOL_FUNCTION;
      addr = gnu_ifunc_resolve_addr (target_gdbarch (), addr);
      break;

    case mst_data:
    case mst_file_data:
    case mst_bss:
    case mst_file_bss:
      type = objfile_type (objfile)->nodebug_data_symbol;
      kind = GCC_CP_SYMBOL_VARIABLE;
      break;

    case mst_slot_got_plt:
      type = objfile_type (objfile)->nodebug_got_plt_symbol;
      kind = GCC_CP_SYMBOL_FUNCTION;
      break;

    default:
      type = objfile_type (objfile)->nodebug_unknown_symbol;
      kind = GCC_CP_SYMBOL_VARIABLE;
      break;
    }

  sym_type = convert_cplus_type (context, type);
  CPOPS ("push_namespace \"\"\n");
  CPCALL (push_namespace, context, "");
  // FIXME: push (and, after the call, pop) any other namespaces, if
  // any, and drop the above when defining a class member.  drop any
  // namespace and class names from before the symbol name, and any
  // function signatures from after it.  -lxo
  /* !!keiths: I don't see how we could do this.  We have NO debug
     information for the symbol.  While we have access to the demangled
     name, we still don't know what A::B::C::D::E::F means without debug
     info, no?  */
  CPOPS ("new_decl minsym %s %d\n", MSYMBOL_NATURAL_NAME (msym), (int) kind);
  decl = CPCALL (new_decl, context,
		 MSYMBOL_NATURAL_NAME (msym), kind, sym_type,
		 NULL, addr, NULL, 0);
  CPOPS ("pop_namespace \"\"\n");
  CPCALL (pop_namespace, context);
}

/* Do a regular expression search of the symbol table for any symbol
   named NAME in the given DOMAIN.  Warning: This is INCREDIBLY slow.  */

static int
regexp_search_symbols (struct compile_cplus_instance *context,
		      const char *name, domain_enum domain)
{
  char *regexp;
  enum search_domain search_domain;
  struct symbol_search *symbols, *p;
  struct cleanup *cleanup;
  int found = 0;

  switch (domain)
    {
    case STRUCT_DOMAIN:
      search_domain = TYPES_DOMAIN;
      break;
    case VAR_DOMAIN:
      /* !!keiths: Don't run this for VAR_DOMAIN symbols?  */
      return 0;
      search_domain = VARIABLES_DOMAIN;
      break;
    default:
      /* This will cause search_symbols to assert.  */
      search_domain = ALL_DOMAIN;
      break;
    }

  symbols = NULL;
  cleanup = make_cleanup_free_search_symbols (&symbols);

  regexp = xstrprintf ("\\(\\(.*::\\)\\|^\\)%s$", name);
  make_cleanup (xfree, regexp);
  search_symbols (regexp, search_domain, 0, NULL, &symbols);

  for (p = symbols; p != NULL; p = p->next)
    {
      if (p->symbol != NULL)
	{
	  struct block_symbol sym;

	  sym.symbol = p->symbol;
	  sym.block = SYMBOL_BLOCK_VALUE (p->symbol);
	  convert_symbol_sym (context, name, sym, domain);
	  found = 1;
	}
      /* !!keiths: Ignore minsyms?  */
    }

  do_cleanups (cleanup);
  return found;
}

/* See compile-internal.h.  */

void
gcc_cplus_convert_symbol (void *datum,
			  struct gcc_cp_context *gcc_context,
			  enum gcc_cp_oracle_request request,
			  const char *identifier)
{
  struct compile_cplus_instance *context
    = (struct compile_cplus_instance *) datum;
  domain_enum domain;
  int found = 0;
  struct search_multiple_result search_result;
  struct cleanup *cleanups;
  enum search_domain search_domain;

  switch (request)
    {
    case GCC_CP_ORACLE_SYMBOL:
      domain = VAR_DOMAIN;
      break;
    case GCC_CP_ORACLE_TAG:
      domain = STRUCT_DOMAIN;
      break;
    case GCC_CP_ORACLE_LABEL:
      domain = LABEL_DOMAIN;
      break;
    default:
      gdb_assert_not_reached ("Unrecognized oracle request.");
    }

  /* We can't allow exceptions to escape out of this callback.  Safest
     is to simply emit a gcc error.  */
#if DEBUG_ORACLE
  printf ("got oracle request for %s in domain %s\n", identifier,
	  domain_name (domain));
#endif

  memset (&search_result, 0, sizeof (search_result));
  cleanups = make_cleanup (search_multiple_result_cleanup, &search_result);
  TRY
    {
      int ix;

      /* !!keiths: Symbol lookup is out of control.  Here's the current
	 process, screaming for a custom symbol table search:

	 1. If looking up a symbol in VAR_DOMAIN (basically anything but
	 a type), use linespec.c's (new) multi-symbol search.  This will
	 allow overloads of functions (not methods) to be converted.

	 2. If a symbol is not found, do a "standard" lookup.  This will
	 find variables in the current scope.

	 3. If a symbol is still not found, try a regexp search.  This
	 allows namespace-y stuff to work (cp-simple-ns.exp). This is currently
	 only used for STRUCT_DOMAIN lookups.

	 4. Finally, if all else fails, fall back to minsyms.  */

      if (domain == VAR_DOMAIN)
	{
	  search_result = search_symbols_multiple (identifier,
						   current_language,
						   domain, NULL, NULL);
	  if (!VEC_empty (block_symbol_d, search_result.symbols))
	    {
	      struct block_symbol *elt;

	      for (ix = 0;
		   VEC_iterate (block_symbol_d, search_result.symbols, ix, elt);
		   ++ix)
		{
		  convert_symbol_sym (context, identifier, *elt, domain);
		}
	      found = 1;
	    }
	}

      if (!found)
	{
	  struct block_symbol sym;

	  sym = lookup_symbol (identifier, context->base.block, domain, NULL);
	  if (sym.symbol != NULL)
	    {
	      convert_symbol_sym (context, identifier, sym, domain);
	      found = 1;
	    }
	}

      if (!found)
	{
	  /* Try a regexp search of the program's symbols.  */
	  found = regexp_search_symbols (context, identifier, domain);

	  /* One last attempt: fall back to minsyms.  */
	  if (!found && !VEC_empty (bound_minimal_symbol_d,
				    search_result.minimal_symbols))
	    {
	      struct bound_minimal_symbol *elt;

	      for (ix = 0;
		   VEC_iterate (bound_minimal_symbol_d,
				search_result.minimal_symbols, ix, elt);
		   ++ix)
		{
		  convert_symbol_bmsym (context, *elt);
		}
	      found = 1;
	    }
	}
    }
  CATCH (e, RETURN_MASK_ALL)
    {
      CPOPS ("error: %s\n", e.message);
      CPCALL (error, context, e.message);
    }
  END_CATCH

  if (compile_debug && !found)
    fprintf_unfiltered (gdb_stdout,
			"gcc_convert_symbol \"%s\": lookup_symbol failed\n",
			identifier);

#if DEBUG_ORACLE
  if (found)
    printf ("found type for %s!\n", identifier);
  else
    printf ("did not find type for %s\n", identifier);
#endif

  do_cleanups (cleanups);
  return;
}

/* See compile-internal.h.  */

gcc_address
gcc_cplus_symbol_address (void *datum, struct gcc_cp_context *gcc_context,
			  const char *identifier)
{
  struct compile_cplus_instance *context
    = (struct compile_cplus_instance *) datum;
  gcc_address result = 0;
  int found = 0;

#if DEBUG_ORACLE
  printf ("got oracle request for address of %s\n", identifier);
#endif

  /* We can't allow exceptions to escape out of this callback.  Safest
     is to simply emit a gcc error.  */
  TRY
    {
      struct symbol *sym;

      /* We only need global functions here.  */
      sym = lookup_symbol (identifier, NULL, VAR_DOMAIN, NULL).symbol;
      if (sym != NULL && SYMBOL_CLASS (sym) == LOC_BLOCK)
	{
	  if (compile_debug)
	    fprintf_unfiltered (gdb_stdout,
				"gcc_symbol_address \"%s\": full symbol\n",
				identifier);
	  result = BLOCK_START (SYMBOL_BLOCK_VALUE (sym));
	  if (TYPE_GNU_IFUNC (SYMBOL_TYPE (sym)))
	    result = gnu_ifunc_resolve_addr (target_gdbarch (), result);
	  found = 1;
	}
      else
	{
	  struct bound_minimal_symbol msym;

	  msym = lookup_bound_minimal_symbol (identifier);
	  if (msym.minsym != NULL)
	    {
	      if (compile_debug)
		fprintf_unfiltered (gdb_stdout,
				    "gcc_symbol_address \"%s\": minimal "
				    "symbol\n",
				    identifier);
	      result = BMSYMBOL_VALUE_ADDRESS (msym);
	      if (MSYMBOL_TYPE (msym.minsym) == mst_text_gnu_ifunc)
		result = gnu_ifunc_resolve_addr (target_gdbarch (), result);
	      found = 1;
	    }
	}
    }

  CATCH (e, RETURN_MASK_ERROR)
    {
      CPOPS ("error: %s\n", e.message);
      CPCALL (error, context, e.message);
    }
  END_CATCH

  if (compile_debug && !found)
    fprintf_unfiltered (gdb_stdout,
			"gcc_symbol_address \"%s\": failed\n",
			identifier);

#if DEBUG_ORACLE
  if (found)
    printf ("found address for %s!\n", identifier);
  else
    printf ("did not find address for %s\n", identifier);
#endif

  return result;
}



/* A hash function for symbol names.  */

static hashval_t
hash_symname (const void *a)
{
  const struct symbol *sym = (struct symbol *) a;

  return htab_hash_string (SYMBOL_NATURAL_NAME (sym));
}

/* A comparison function for hash tables that just looks at symbol
   names.  */

static int
eq_symname (const void *a, const void *b)
{
  const struct symbol *syma = (struct symbol *) a;
  const struct symbol *symb = (struct symbol *) b;

  return strcmp (SYMBOL_NATURAL_NAME (syma), SYMBOL_NATURAL_NAME (symb)) == 0;
}

/* If a symbol with the same name as SYM is already in HASHTAB, return
   1.  Otherwise, add SYM to HASHTAB and return 0.  */

static int
symbol_seen (htab_t hashtab, struct symbol *sym)
{
  void **slot;

  slot = htab_find_slot (hashtab, sym, INSERT);
  if (*slot != NULL)
    return 1;

  *slot = sym;
  return 0;
}

/* Generate C code to compute the length of a VLA.  */

static void
generate_vla_size (struct compile_cplus_instance *compiler,
		   struct ui_file *stream,
		   struct gdbarch *gdbarch,
		   unsigned char *registers_used,
		   CORE_ADDR pc,
		   struct type *type,
		   struct symbol *sym)
{
  type = check_typedef (type);

  if (TYPE_CODE (type) == TYPE_CODE_REF)
    type = check_typedef (TYPE_TARGET_TYPE (type));

  switch (TYPE_CODE (type))
    {
    case TYPE_CODE_RANGE:
      {
	if (TYPE_HIGH_BOUND_KIND (type) == PROP_LOCEXPR
	    || TYPE_HIGH_BOUND_KIND (type) == PROP_LOCLIST)
	  {
	    const struct dynamic_prop *prop = &TYPE_RANGE_DATA (type)->high;
	    char *name = c_get_range_decl_name (prop);
	    struct cleanup *cleanup = make_cleanup (xfree, name);

	    dwarf2_compile_property_to_c (stream, name,
					  gdbarch, registers_used,
					  prop, pc, sym);
	    do_cleanups (cleanup);
	  }
      }
      break;

    case TYPE_CODE_ARRAY:
      generate_vla_size (compiler, stream, gdbarch, registers_used, pc,
			 TYPE_INDEX_TYPE (type), sym);
      generate_vla_size (compiler, stream, gdbarch, registers_used, pc,
			 TYPE_TARGET_TYPE (type), sym);
      break;

    case TYPE_CODE_UNION:
    case TYPE_CODE_STRUCT:
      {
	int i;

	for (i = 0; i < TYPE_NFIELDS (type); ++i)
	  if (!field_is_static (&TYPE_FIELD (type, i)))
	    generate_vla_size (compiler, stream, gdbarch, registers_used, pc,
			       TYPE_FIELD_TYPE (type, i), sym);
      }
      break;
    }
}

/* Generate C code to compute the address of SYM.  */

static void
generate_cplus_for_for_one_variable (struct compile_cplus_instance *compiler,
				 struct ui_file *stream,
				 struct gdbarch *gdbarch,
				 unsigned char *registers_used,
				 CORE_ADDR pc,
				 struct symbol *sym)
{

  TRY
    {
      if (is_dynamic_type (SYMBOL_TYPE (sym)))
	{
	  struct ui_file *size_file = mem_fileopen ();
	  struct cleanup *cleanup = make_cleanup_ui_file_delete (size_file);

	  generate_vla_size (compiler, size_file, gdbarch, registers_used, pc,
			     SYMBOL_TYPE (sym), sym);
	  ui_file_put (size_file, ui_file_write_for_put, stream);

	  do_cleanups (cleanup);
	}

      if (SYMBOL_COMPUTED_OPS (sym) != NULL)
	{
	  char *generated_name = symbol_substitution_name (sym);
	  struct cleanup *cleanup = make_cleanup (xfree, generated_name);
	  /* We need to emit to a temporary buffer in case an error
	     occurs in the middle.  */
	  struct ui_file *local_file = mem_fileopen ();

	  make_cleanup_ui_file_delete (local_file);
	  SYMBOL_COMPUTED_OPS (sym)->generate_c_location (sym, local_file,
							  gdbarch,
							  registers_used,
							  pc, generated_name);
	  ui_file_put (local_file, ui_file_write_for_put, stream);

	  do_cleanups (cleanup);
	}
      else
	{
	  switch (SYMBOL_CLASS (sym))
	    {
	    case LOC_REGISTER:
	    case LOC_ARG:
	    case LOC_REF_ARG:
	    case LOC_REGPARM_ADDR:
	    case LOC_LOCAL:
	      error (_("Local symbol unhandled when generating C code."));

	    case LOC_COMPUTED:
	      gdb_assert_not_reached (_("LOC_COMPUTED variable "
					"missing a method."));

	    default:
	      /* Nothing to do for all other cases, as they don't represent
		 local variables.  */
	      break;
	    }
	}
    }

  CATCH (e, RETURN_MASK_ERROR)
    {
      if (compiler->symbol_err_map == NULL)
	compiler->symbol_err_map = htab_create_alloc (10,
						      hash_symbol_error,
						      eq_symbol_error,
						      del_symbol_error,
						      xcalloc,
						      xfree);
      insert_symbol_error (compiler->symbol_err_map, sym, e.message);
    }
  END_CATCH
}

/* See compile-internal.h.  */

unsigned char *
generate_cplus_for_variable_locations (struct compile_cplus_instance *compiler,
				       struct ui_file *stream,
				       struct gdbarch *gdbarch,
				       const struct block *block,
				       CORE_ADDR pc)
{
  struct cleanup *cleanup, *outer;
  htab_t symhash;
  const struct block *static_block = block_static_block (block);
  unsigned char *registers_used;

  /* If we're already in the static or global block, there is nothing
     to write.  */
  if (static_block == NULL || block == static_block)
    return NULL;

  registers_used = XCNEWVEC (unsigned char, gdbarch_num_regs (gdbarch));
  outer = make_cleanup (xfree, registers_used);

  /* Ensure that a given name is only entered once.  This reflects the
     reality of shadowing.  */
  symhash = htab_create_alloc (1, hash_symname, eq_symname, NULL,
			       xcalloc, xfree);
  cleanup = make_cleanup_htab_delete (symhash);

  while (1)
    {
      struct symbol *sym;
      struct block_iterator iter;

      /* Iterate over symbols in this block, generating code to
	 compute the location of each local variable.  */
      for (sym = block_iterator_first (block, &iter);
	   sym != NULL;
	   sym = block_iterator_next (&iter))
	{
	  if (!symbol_seen (symhash, sym))
	    generate_cplus_for_for_one_variable (compiler, stream, gdbarch,
						 registers_used, pc, sym);
	}

      /* If we just finished the outermost block of a function, we're
	 done.  */
      if (BLOCK_FUNCTION (block) != NULL)
	break;
      block = BLOCK_SUPERBLOCK (block);
    }

  do_cleanups (cleanup);
  discard_cleanups (outer);
  return registers_used;
}
