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
#include "compile-cplus.h"
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
#include "gdbcmd.h"
#include "compile-c.h" // !!keiths FIXME for c_get_range_decl_name



using namespace compile;

// See description in compile-internal.h.

int debug_compile_oracle = 0;

/* Convert a given symbol, SYM, to the compiler's representation.
   CONTEXT is the compiler instance.  IS_GLOBAL is true if the
   symbol came from the global scope.  IS_LOCAL is true if the symbol
   came from a local scope.  (Note that the two are not strictly
   inverses because the symbol might have come from the static
   scope.)  */

static void
convert_one_symbol (compile_cplus_instance *instance,
		    struct block_symbol sym, bool is_global, bool is_local)
{
  /* Squash compiler warning.  */
  gcc_type sym_type = 0;
  const char *filename = symbol_symtab (sym.symbol)->filename;
  unsigned short line = SYMBOL_LINE (sym.symbol);

  instance->error_symbol_once (sym.symbol);

  if (SYMBOL_CLASS (sym.symbol) == LOC_LABEL)
    sym_type = 0;
  else if (!SYMBOL_IS_CPLUS_TEMPLATE_FUNCTION (sym.symbol))
    sym_type = instance->convert_type (SYMBOL_TYPE (sym.symbol));

  if (SYMBOL_DOMAIN (sym.symbol) == STRUCT_DOMAIN)
    {
      /* Nothing to do.  */
    }
  else
    {
      /* Squash compiler warning.  */
      gcc_cp_symbol_kind_flags kind = GCC_CP_FLAG_BASE;
      CORE_ADDR addr = 0;
      std::string name;
      char *symbol_name = NULL;

      // Add a null cleanup for templates.  !!keiths: remove!
      struct cleanup *back_to
	= make_cleanup (free_current_contents, &symbol_name);

      switch (SYMBOL_CLASS (sym.symbol))
	{
	case LOC_TYPEDEF:
	  if (TYPE_CODE (SYMBOL_TYPE (sym.symbol)) == TYPE_CODE_TYPEDEF)
	    kind = GCC_CP_SYMBOL_TYPEDEF;
	  else  if (TYPE_CODE (SYMBOL_TYPE (sym.symbol)) == TYPE_CODE_NAMESPACE)
	    {
	      do_cleanups (back_to);
	      return;
	    }
	  break;

	case LOC_LABEL:
	  kind = GCC_CP_SYMBOL_LABEL;
	  addr = SYMBOL_VALUE_ADDRESS (sym.symbol);
	  break;

	case LOC_BLOCK:
	  {
	    bool ignore;
	    char *special_name;
	    const char *func_name;

	    kind = GCC_CP_SYMBOL_FUNCTION;
	    addr = BLOCK_START (SYMBOL_BLOCK_VALUE (sym.symbol));
	    if (is_global && TYPE_GNU_IFUNC (SYMBOL_TYPE (sym.symbol)))
	      addr = gnu_ifunc_resolve_addr (target_gdbarch (), addr);

	    special_name = NULL;
	    //cp_find_method_field (sym.symbol, 1);
	    func_name = maybe_canonicalize_special_function
	      (SYMBOL_LINKAGE_NAME (sym.symbol), NULL,
	       SYMBOL_TYPE (sym.symbol), &special_name, &ignore);
	    if (special_name != NULL)
	      {
		kind |= GCC_CP_FLAG_SPECIAL_FUNCTION;
		name = special_name;
		xfree (special_name);
	      }
	    else if (func_name != SYMBOL_NATURAL_NAME (sym.symbol))
	      {
		kind |= GCC_CP_FLAG_SPECIAL_FUNCTION;
		name = func_name;
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
	      do_cleanups (back_to);
	      return;
	    }
	  instance->build_constant (sym_type, SYMBOL_NATURAL_NAME (sym.symbol),
				    SYMBOL_VALUE (sym.symbol), filename, line);
	  do_cleanups (back_to);
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
	  symbol_name = c_symbol_substitution_name (sym.symbol);
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
      if (instance->scope () != COMPILE_I_RAW_SCOPE
	  || symbol_name == NULL)
	{
	  compile_scope scope;

	  /* For non-local symbols, create/push a new scope so that the
	     symbol is properly scoped to the plug-in.  */
	  if (!is_local)
	    {
	      scope
		= instance->new_scope (SYMBOL_NATURAL_NAME (sym.symbol),
				       SYMBOL_TYPE (sym.symbol));
	      if (scope.nested_type () != GCC_TYPE_NONE)
		{
		  /* We found a symbol for this type that was defined inside
		     some other symbol, e.g., a class tyepdef defined.
		     Don't return anything in that case because that really
		     confuses users.  */
		  do_cleanups (back_to);
		  return;
		}

	      instance->enter_scope (scope);
	    }

	  /* Get the `raw' name of the symbol.  */
	  if (name.empty () && SYMBOL_NATURAL_NAME (sym.symbol) != NULL)
	    {
	      char *str = cp_func_name (SYMBOL_NATURAL_NAME (sym.symbol));

	      name = str;
	      xfree (str);
	    }

	  /* Define the decl.  */
	  if (SYMBOL_IS_CPLUS_TEMPLATE_FUNCTION (sym.symbol))
	    {
	      struct template_symbol *tsymbol
		= (struct template_symbol *) sym.symbol;

	      instance->specialize_function_template (tsymbol, addr,
						      filename, line);
	    }
	  else
	    {
	      instance->new_decl ("variable", name.c_str (), kind, sym_type,
				  symbol_name, addr, filename, line);
	    }

	  // Pop scope for non-local symbols.
	  if (!is_local)
	    instance->leave_scope ();
	}

      /* Free any allocated memory.  */
      do_cleanups (back_to);
    }
}

/* Convert a full symbol to its gcc form.  CONTEXT is the compiler to
   use, IDENTIFIER is the name of the symbol, SYM is the symbol
   itself, and DOMAIN is the domain which was searched.  */

static void
convert_symbol_sym (compile_cplus_instance *instance,
		    const char *identifier, struct block_symbol sym,
		    domain_enum domain)
{
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

  const struct block *static_block = block_static_block (sym.block);
  /* STATIC_BLOCK is NULL if FOUND_BLOCK is the global block.  */
  bool is_local_symbol = (sym.block != static_block && static_block != NULL);
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
	  convert_one_symbol (instance, global_sym, true, false);
	}
    }

  if (compile_debug)
    fprintf_unfiltered (gdb_stdout,
			"gcc_convert_symbol \"%s\": local symbol\n",
			identifier);
  convert_one_symbol (instance, sym, false, is_local_symbol);
}

/* Convert a minimal symbol to its gcc form.  CONTEXT is the compiler
   to use and BMSYM is the minimal symbol to convert.  */

static void
convert_symbol_bmsym (compile_cplus_instance *instance,
		      struct bound_minimal_symbol bmsym)
{
  struct minimal_symbol *msym = bmsym.minsym;
  struct objfile *objfile = bmsym.objfile;
  struct type *type;
  gcc_cp_symbol_kind_flags kind;
  gcc_type sym_type;
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

  sym_type = instance->convert_type (type);
  instance->push_namespace ("");
  // FIXME: push (and, after the call, pop) any other namespaces, if
  // any, and drop the above when defining a class member.  drop any
  // namespace and class names from before the symbol name, and any
  // function signatures from after it.  -lxo
  /* !!keiths: I don't see how we could do this.  We have NO debug
     information for the symbol.  While we have access to the demangled
     name, we still don't know what A::B::C::D::E::F means without debug
     info, no?  */
  instance->new_decl ("minsym", MSYMBOL_NATURAL_NAME (msym), kind, sym_type,
		      NULL, addr, NULL, 0);
  instance->pop_namespace ("");
}

/* Do a regular expression search of the symbol table for any symbol
   named NAME in the given DOMAIN.  Warning: This is INCREDIBLY slow.  */

static int
regexp_search_symbols (compile_cplus_instance *instance,
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
      /* !!keiths: We really don't want to search functions.  The search
	 will return all kinds of stuff that we don't really want, such as
	 every operator+ defined in every class.  */
      return 0;
      /* !!keiths; fscked up.  We need to search through functions
	 when GCC_CP_ORACLE_SYMBOL (= VAR_DOMAIN).  */
      /* !!keiths; I hope we don't have to search even more domains!  */
      search_domain = FUNCTIONS_DOMAIN;
      break;
    default:
      /* This will cause search_symbols to assert.  */
      search_domain = ALL_DOMAIN;
      break;
    }

  symbols = NULL;
  cleanup = make_cleanup_free_search_symbols (&symbols);

  regexp = xstrprintf ("\\(\\(::\\)\\|^\\)%s\\($\\|<\\)", name);
  make_cleanup (xfree, regexp);
  search_symbols (regexp, search_domain, 0, NULL, &symbols);

  for (p = symbols; p != NULL; p = p->next)
    {
      if (p->symbol != NULL)
	{
	  struct block_symbol sym;

	  sym.symbol = p->symbol;
	  sym.block = SYMBOL_BLOCK_VALUE (p->symbol);
	  convert_symbol_sym (instance, name, sym, domain);
	  found = 1;
	}
      /* !!keiths: Ignore minsyms?  */
    }

  do_cleanups (cleanup);
  return found;
}

/* See compile-cplus.h.  */

void
gcc_cplus_convert_symbol (void *datum,
			  struct gcc_cp_context *gcc_context,
			  enum gcc_cp_oracle_request request,
			  const char *identifier)
{
  compile_cplus_instance *instance
    = (compile_cplus_instance *) datum;
  domain_enum domain;
  int found = 0;
  struct search_multiple_result search_result;
  struct cleanup *cleanups;
  /* !!keiths create htab for template definitions */

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
  if (debug_compile_oracle)
    {
      printf ("got oracle request for \"%s\" in domain %s\n", identifier,
	      domain_name (domain));
    }

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

	      /* Define any template generics from the found symbols.  */
	      define_templates (instance, search_result.symbols);

	      /* Convert each found symbol.  */
	      for (ix = 0;
		   VEC_iterate (block_symbol_d, search_result.symbols, ix, elt);
		   ++ix)
		{
		  convert_symbol_sym (instance, identifier, *elt, domain);
		}
	      found = 1;
	    }
	}

      if (!found)
	{
	  struct block_symbol sym;

	  sym = lookup_symbol (identifier, instance->block (), domain, NULL);
	  if (sym.symbol != NULL)
	    {
	      convert_symbol_sym (instance, identifier, sym, domain);
	      found = 1;
	    }
	}

      if (!found)
	{
	  /* Try a regexp search of the program's symbols.  */
	  found = regexp_search_symbols (instance, identifier, domain);

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
		  convert_symbol_bmsym (instance, *elt);
		}
	      found = 1;
	    }
	}
    }
  CATCH (e, RETURN_MASK_ALL)
    {
      instance->error (e.message);
    }
  END_CATCH

  if (compile_debug && !found)
    fprintf_unfiltered (gdb_stdout,
			"gcc_convert_symbol \"%s\": lookup_symbol failed\n",
			identifier);

  if (debug_compile_oracle)
    {
      if (found)
	printf_unfiltered ("found type for %s!\n", identifier);
      else
	printf_unfiltered ("did not find type for %s\n", identifier);
    }

  do_cleanups (cleanups);
  return;
}

/* See compile-cplus.h.  */

gcc_address
gcc_cplus_symbol_address (void *datum, struct gcc_cp_context *gcc_context,
			  const char *identifier)
{
  compile_cplus_instance *instance
    = (compile_cplus_instance *) datum;
  gcc_address result = 0;
  int found = 0;

  if (debug_compile_oracle)
    printf_unfiltered ("got oracle request for address of %s\n", identifier);

  /* We can't allow exceptions to escape out of this callback.  Safest
     is to simply emit a gcc error.  */
  TRY
    {
      struct symbol *sym;

      /* FIXME: We used to only need global functions here, but we may
	 now be asked for other symbols.  IDENTIFIER is a mangled
	 name.  -lxo */
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
      instance->error (e.message);
    }
  END_CATCH

  if (compile_debug && !found)
    fprintf_unfiltered (gdb_stdout,
			"gcc_symbol_address \"%s\": failed\n",
			identifier);

  if (debug_compile_oracle)
    {
      if (found)
	printf_unfiltered ("found address for %s!\n", identifier);
      else
	printf_unfiltered ("did not find address for %s\n", identifier);
    }

  return result;
}



void _initialize_compile_cplus_symbols (void);

void
_initialize_compile_cplus_symbols (void)
{
  add_setshow_boolean_cmd ("compile-oracle", no_class,
			   &debug_compile_oracle, _("\
Set debugging of compiler plug-in oracle requests."), _("\
Show debugging of compiler plug-in oracle requests."), _("\
When enabled debugging messages are printed for compiler plug-in\n\
oracle requests."),
			   NULL,
			   NULL,
			   &setdebuglist,
			   &showdebuglist);
}
