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
mi_info_one_symbol_details (enum search_domain kind,
			    struct symbol *sym, int block)
{
  struct ui_out *uiout = current_uiout;

  ui_out_emit_tuple tuple_emitter (uiout, NULL);
  if (SYMBOL_LINE (sym) != 0)
    uiout->field_unsigned ("line", SYMBOL_LINE (sym));
  uiout->field_string ("name", SYMBOL_PRINT_NAME (sym));

  if (kind == FUNCTIONS_DOMAIN || kind == VARIABLES_DOMAIN)
    {
      string_file tmp_stream;
      type_print (SYMBOL_TYPE (sym), "", &tmp_stream, -1);
      uiout->field_string ("type", tmp_stream.string ());

      std::string str = symbol_to_info_string (sym, block, kind);
      uiout->field_string ("description", str.c_str ());
    }
}

/* This class is used to produce the nested structure of tuples and lists
   required to present the results of the MI_SYMBOL_INFO function.  */
class mi_symbol_info_emitter
{
  /* When printing debug symbols we need to track the last symtab so we can
     spot when we have entered a new one.  */
  const symtab *m_last_symtab;

  /* The ui_out to which output will be sent.  */
  struct ui_out *m_uiout;

  /* The outer container for all the matched symbols.  */
  ui_out_emit_tuple m_outer_symbols;

  /* The order of these optional emitters is critical as they will be
     deleted in reverse order, which is important as these are popped from
     the uiout stack as they are destroyed.  */
  gdb::optional<ui_out_emit_list> m_debug_emitter;
  gdb::optional<ui_out_emit_tuple> m_symtab_emitter;
  gdb::optional<ui_out_emit_list> m_symbols_emitter;
  gdb::optional<ui_out_emit_list> m_nondebug_emitter;

  /* Called when we might want to print our first nondebug symbol in order
     to shutdown any debug symbol printing that might be in progress.  */
  void maybe_finish_debug_output ()
  {
    /* If the debug emitter is in use.  */
    if (m_debug_emitter.has_value ())
      {
	/* Then we should have a symbols list inside a symtab tuple also
	   currently in use.  */
	gdb_assert (m_symbols_emitter.has_value ());
	gdb_assert (m_symtab_emitter.has_value ());

	/* Shut down the symbols list, symtab tuple, and debug list
	   emitters (in that order).  We are now back to the level of the
	   outer_symbols tuple ready to (possibly) start a nondebug list,
	   though that is not done here.  */
	m_symbols_emitter.reset ();
	m_symtab_emitter.reset ();
	m_debug_emitter.reset ();
      }
  }

  /* Return true if the nondebug emitter has been put in place.  */
  bool have_started_nondebug_symbol_output () const
  {
    return m_nondebug_emitter.has_value ();
  }

  /* Called before we print every nondebug symbol.  If this is the first
     nondebug symbol to be printed then it will setup the emitters required
     to print nondebug symbols.  */
  void maybe_start_nondebug_symbol_output ()
  {
    if (!have_started_nondebug_symbol_output ())
      m_nondebug_emitter.emplace (m_uiout, "nondebug");
  }

  /* Actually output one nondebug symbol, puts a tuple emitter in place
     and then outputs the fields for this msymbol.  */
  void output_nondebug_symbol (const struct bound_minimal_symbol &msymbol)
  {
    struct gdbarch *gdbarch = get_objfile_arch (msymbol.objfile);
    ui_out_emit_tuple tuple_emitter (m_uiout, NULL);
    m_uiout->field_core_addr ("address", gdbarch,
			      BMSYMBOL_VALUE_ADDRESS (msymbol));
    m_uiout->field_string ("name", MSYMBOL_PRINT_NAME (msymbol.minsym));
  }

  /* Called before we print every debug symbol.  If this is the first debug
     symbol to be printed then it will setup the top level of emitters
     required to print debug symbols.  */
  void maybe_start_debug_symbol_output ()
  {
    if (!m_debug_emitter.has_value ())
      m_debug_emitter.emplace (m_uiout, "debug");
  }

  /* Called before we print every debug symbol, S is the symtab for the
     symbol to be printed.  If S is different to the last symtab we printed
     for then we close down the emitters for the last symtab, and create
     new emitters for this new symtab.  */
  void setup_emitters_for_symtab (symtab *s)
  {
    if (s != m_last_symtab)
      {
	/* Reset a possible previous symbol list within a symtab.  */
	m_symbols_emitter.reset ();
	m_symtab_emitter.reset ();

	/* Start a new symtab and symbol list within the symtab.  */
	m_symtab_emitter.emplace (m_uiout, nullptr);
	m_uiout->field_string ("filename",
			       symtab_to_filename_for_display (s));
	m_uiout->field_string ("fullname", symtab_to_fullname (s));
	m_symbols_emitter.emplace (m_uiout, "symbols");

	/* Record the current symtab.  */
	m_last_symtab = s;
      }
  }

public:
  /* Constructor.  */
  mi_symbol_info_emitter (struct ui_out *uiout)
    : m_last_symtab (nullptr),
      m_uiout (uiout),
      m_outer_symbols (uiout, "symbols")
  { /* Nothing.  */ }

  /* Output P a symbol found by searching for symbols of type KIND.  */
  void output (const symbol_search &p, enum search_domain kind)
  {
    if (p.msymbol.minsym != NULL)
      {
	/* If this is the first nondebug symbol, and we have previous
	   outputted a debug symbol then we need to close down all of the
	   emitters related to printing debug symbols.  */
	maybe_finish_debug_output ();

	/* If this is the first nondebug symbol then we need to create the
	   emitters related to printing nondebug symbols.  */
	maybe_start_nondebug_symbol_output ();

	/* We are no safe to emit the nondebug symbol.  */
	output_nondebug_symbol (p.msymbol);
      }
    else
      {
	/* All debug symbols should appear in the list before all
	   non-debug symbols.  */
	gdb_assert (!have_started_nondebug_symbol_output ());

	/* If this is the first debug symbol then we need to create the
	   outer level of emitters related to printing debug symbols.  */
	maybe_start_debug_symbol_output ();

	/* Ensure the correct emitters are in place to emit this debug
	   symbol.  */
	setup_emitters_for_symtab (symbol_symtab (p.symbol));

	/* Emit information for this debug symbol.  */
	mi_info_one_symbol_details (kind, p.symbol, p.block);
      }
  }
};

/* This is the guts of the commands '-symbol-info-functions',
   '-symbol-info-variables', and '-symbol-info-types'.  It calls
   search_symbols to find all matches and then prints the matching
   [m]symbols in an MI structured format.  This is similar to
   symtab_symbol_info in symtab.c.  All the arguments are used to
   initialise a SEARCH_SYMBOLS_SPEC, see symtab.h for a description of
   their meaning.  */

static void
mi_symbol_info (enum search_domain kind, const char *regexp,
		const char *t_regexp, bool exclude_minsyms)
{
  /* Must make sure that if we're interrupted, symbols gets freed.  */
  global_symbol_searcher sym_search (kind, regexp, t_regexp, exclude_minsyms);
  std::vector<symbol_search> symbols = sym_search.search ();

  mi_symbol_info_emitter emitter (current_uiout);
  for (const symbol_search &p : symbols)
    {
      QUIT;
      emitter.output (p, kind);
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

  while (1)
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

