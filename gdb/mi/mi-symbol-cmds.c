/* MI Command Set - symbol commands.
   Copyright (C) 2003-2019 Free Software Foundation, Inc.

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
#include "mi-cmds.h"
#include "symtab.h"
#include "objfiles.h"
#include "ui-out.h"
#include "source.h"
#include "mi-getopt.h"

/* Print the list of all pc addresses and lines of code for the
   provided (full or base) source file name.  The entries are sorted
   in ascending PC order.  */

void
mi_cmd_symbol_list_lines (const char *command, char **argv, int argc)
{
  struct gdbarch *gdbarch;
  char *filename;
  struct symtab *s;
  int i;
  struct ui_out *uiout = current_uiout;

  if (argc != 1)
    error (_("-symbol-list-lines: Usage: SOURCE_FILENAME"));

  filename = argv[0];
  s = lookup_symtab (filename);

  if (s == NULL)
    error (_("-symbol-list-lines: Unknown source file name."));

  /* Now, dump the associated line table.  The pc addresses are
     already sorted by increasing values in the symbol table, so no
     need to perform any other sorting.  */

  gdbarch = get_objfile_arch (SYMTAB_OBJFILE (s));

  ui_out_emit_list list_emitter (uiout, "lines");
  if (SYMTAB_LINETABLE (s) != NULL && SYMTAB_LINETABLE (s)->nitems > 0)
    for (i = 0; i < SYMTAB_LINETABLE (s)->nitems; i++)
    {
      ui_out_emit_tuple tuple_emitter (uiout, NULL);
      uiout->field_core_addr ("pc", gdbarch, SYMTAB_LINETABLE (s)->item[i].pc);
      uiout->field_signed ("line", SYMTAB_LINETABLE (s)->item[i].line);
    }
}

/* Used by the -symbol-info-* and -symbol-info-module-* commands to print
   information about the symbol SYM in a block of index BLOCK (either
   GLOBAL_BLOCK or STATIC_BLOCK).  KIND is the kind of symbol we searched
   for in order to find SYM, which impact which fields are displayed in the
   results.  */

static void
output_debug_symbol (ui_out *uiout, enum search_domain kind,
		     struct symbol *sym, int block)
{
  ui_out_emit_tuple tuple_emitter (uiout, NULL);

  if (SYMBOL_LINE (sym) != 0)
    uiout->field_unsigned ("line", SYMBOL_LINE (sym));
  uiout->field_string ("name", sym->print_name ());

  if (kind == FUNCTIONS_DOMAIN || kind == VARIABLES_DOMAIN)
    {
      string_file tmp_stream;
      type_print (SYMBOL_TYPE (sym), "", &tmp_stream, -1);
      uiout->field_string ("type", tmp_stream.string ());

      std::string str = symbol_to_info_string (sym, block, kind);
      uiout->field_string ("description", str);
    }
}

/* Actually output one nondebug symbol, puts a tuple emitter in place
   and then outputs the fields for this msymbol.  */

static void
output_nondebug_symbol (ui_out *uiout,
			const struct bound_minimal_symbol &msymbol)
{
  struct gdbarch *gdbarch = get_objfile_arch (msymbol.objfile);
  ui_out_emit_tuple tuple_emitter (uiout, NULL);

  uiout->field_core_addr ("address", gdbarch,
			  BMSYMBOL_VALUE_ADDRESS (msymbol));
  uiout->field_string ("name", msymbol.minsym->print_name ());
}

/* This is the guts of the commands '-symbol-info-functions',
   '-symbol-info-variables', and '-symbol-info-types'.  It searches for
   symbols matching KING, NAME_REGEXP, TYPE_REGEXP, and EXCLUDE_MINSYMS,
   and then prints the matching [m]symbols in an MI structured format.  */

static void
mi_symbol_info (enum search_domain kind, const char *name_regexp,
		const char *type_regexp, bool exclude_minsyms)
{
  global_symbol_searcher sym_search (kind, name_regexp);
  sym_search.set_symbol_type_regexp (type_regexp);
  sym_search.set_exclude_minsyms (exclude_minsyms);
  std::vector<symbol_search> symbols = sym_search.search ();
  ui_out *uiout = current_uiout;
  int i = 0;

  ui_out_emit_tuple outer_symbols_emitter (uiout, "symbols");

  /* Debug symbols are placed first. */
  if (i < symbols.size () && symbols[i].msymbol.minsym == nullptr)
    {
      ui_out_emit_list debug_symbols_list_emitter (uiout, "debug");

      /* As long as we have debug symbols...  */
      while (i < symbols.size () && symbols[i].msymbol.minsym == nullptr)
	{
	  symtab *symtab = symbol_symtab (symbols[i].symbol);
	  ui_out_emit_tuple symtab_tuple_emitter (uiout, nullptr);

	  uiout->field_string ("filename",
			       symtab_to_filename_for_display (symtab));
	  uiout->field_string ("fullname", symtab_to_fullname (symtab));

	  ui_out_emit_list symbols_list_emitter (uiout, "symbols");

	  /* As long as we have debug symbols from this symtab...  */
	  for (; (i < symbols.size ()
		  && symbols[i].msymbol.minsym == nullptr
		  && symbol_symtab (symbols[i].symbol) == symtab);
	       ++i)
	    {
	      symbol_search &s = symbols[i];

	      output_debug_symbol (uiout, kind, s.symbol, s.block);
	    }
	}
    }

  /* Non-debug symbols are placed after.  */
  if (i < symbols.size ())
    {
      ui_out_emit_list nondebug_symbols_list_emitter (uiout, "nondebug");

      /* As long as we have nondebug symbols...  */
      for (; i < symbols.size (); i++)
	{
	  gdb_assert (symbols[i].msymbol.minsym != nullptr);
	  output_nondebug_symbol (uiout, symbols[i].msymbol);
	}
    }
}

/* Helper for mi_cmd_symbol_info_{functions,variables} - depending on KIND.
   Processes command line options from ARGV and ARGC.  */

static void
mi_info_functions_or_variables (enum search_domain kind, char **argv, int argc)
{
  const char *regexp = nullptr;
  const char *t_regexp = nullptr;
  bool exclude_minsyms = true;

  enum opt
    {
     INCLUDE_NONDEBUG_OPT, TYPE_REGEXP_OPT, NAME_REGEXP_OPT
    };
  static const struct mi_opt opts[] =
  {
    {"-include-nondebug" , INCLUDE_NONDEBUG_OPT, 0},
    {"-type", TYPE_REGEXP_OPT, 1},
    {"-name", NAME_REGEXP_OPT, 1},
    { 0, 0, 0 }
  };

  int oind = 0;
  char *oarg = nullptr;

  while (1)
    {
      const char *cmd_string
	= ((kind == FUNCTIONS_DOMAIN)
	   ? "-symbol-info-functions" : "-symbol-info-variables");
      int opt = mi_getopt (cmd_string, argc, argv, opts, &oind, &oarg);
      if (opt < 0)
	break;
      switch ((enum opt) opt)
	{
	case INCLUDE_NONDEBUG_OPT:
	  exclude_minsyms = false;
	  break;
	case TYPE_REGEXP_OPT:
	  t_regexp = oarg;
	  break;
	case NAME_REGEXP_OPT:
	  regexp = oarg;
	  break;
	}
    }

  mi_symbol_info (kind, regexp, t_regexp, exclude_minsyms);
}

/* Implement -symbol-info-functions command.  */

void
mi_cmd_symbol_info_functions (const char *command, char **argv, int argc)
{
  mi_info_functions_or_variables (FUNCTIONS_DOMAIN, argv, argc);
}

/* Implement -symbol-inf-modules command.  */

void
mi_cmd_symbol_info_modules (const char *command, char **argv, int argc)
{
  const char *regexp = nullptr;

  enum opt
    {
     NAME_REGEXP_OPT
    };
  static const struct mi_opt opts[] =
  {
    {"-name", NAME_REGEXP_OPT, 1},
    { 0, 0, 0 }
  };

  int oind = 0;
  char *oarg = nullptr;

  while (1)
    {
      int opt = mi_getopt ("-symbol-info-modules", argc, argv, opts,
			   &oind, &oarg);
      if (opt < 0)
	break;
      switch ((enum opt) opt)
	{
	case NAME_REGEXP_OPT:
	  regexp = oarg;
	  break;
	}
    }

  mi_symbol_info (MODULES_DOMAIN, regexp, nullptr, true);
}

/* Implement -symbol-info-types command.  */

void
mi_cmd_symbol_info_types (const char *command, char **argv, int argc)
{
  const char *regexp = nullptr;

  enum opt
    {
     NAME_REGEXP_OPT
    };
  static const struct mi_opt opts[] =
  {
    {"-name", NAME_REGEXP_OPT, 1},
    { 0, 0, 0 }
  };

  int oind = 0;
  char *oarg = nullptr;

  while (true)
    {
      int opt = mi_getopt ("-symbol-info-types", argc, argv, opts,
			   &oind, &oarg);
      if (opt < 0)
	break;
      switch ((enum opt) opt)
	{
	case NAME_REGEXP_OPT:
	  regexp = oarg;
	  break;
	}
    }

  mi_symbol_info (TYPES_DOMAIN, regexp, nullptr, true);
}

/* Implement -symbol-info-variables command.  */

void
mi_cmd_symbol_info_variables (const char *command, char **argv, int argc)
{
  mi_info_functions_or_variables (VARIABLES_DOMAIN, argv, argc);
}
