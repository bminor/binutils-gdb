/* "Quick" symbol functions

   Copyright (C) 2021 Free Software Foundation, Inc.

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

#ifndef GDB_QUICK_SYMBOL_H
#define GDB_QUICK_SYMBOL_H

/* Comparison function for symbol look ups.  */

typedef int (symbol_compare_ftype) (const char *string1,
				    const char *string2);

/* Callback for quick_symbol_functions->map_symbol_filenames.  */

typedef void (symbol_filename_ftype) (const char *filename,
				      const char *fullname, void *data);

/* Callback for quick_symbol_functions->expand_symtabs_matching
   to match a file name.  */

typedef bool (expand_symtabs_file_matcher_ftype) (const char *filename,
						  bool basenames);

/* Callback for quick_symbol_functions->expand_symtabs_matching
   to match a symbol name.  */

typedef bool (expand_symtabs_symbol_matcher_ftype) (const char *name);

/* Callback for quick_symbol_functions->expand_symtabs_matching
   to be called after a symtab has been expanded.  */

typedef void (expand_symtabs_exp_notify_ftype) (compunit_symtab *symtab);

/* The "quick" symbol functions exist so that symbol readers can
   avoiding an initial read of all the symbols.  For example, symbol
   readers might choose to use the "partial symbol table" utilities,
   which is one implementation of the quick symbol functions.

   The quick symbol functions are generally opaque: the underlying
   representation is hidden from the caller.

   In general, these functions should only look at whatever special
   index the symbol reader creates -- looking through the symbol
   tables themselves is handled by generic code.  If a function is
   defined as returning a "symbol table", this means that the function
   should only return a newly-created symbol table; it should not
   examine pre-existing ones.

   The exact list of functions here was determined in an ad hoc way
   based on gdb's history.  */

struct quick_symbol_functions
{
  virtual ~quick_symbol_functions ()
  {
  }

  /* Return true if this objfile has any "partial" symbols
     available.  */
  virtual bool has_symbols (struct objfile *objfile) = 0;

  /* Return the symbol table for the "last" file appearing in
     OBJFILE.  */
  virtual struct symtab *find_last_source_symtab (struct objfile *objfile) = 0;

  /* Forget all cached full file names for OBJFILE.  */
  virtual void forget_cached_source_info (struct objfile *objfile) = 0;

  /* Expand and iterate over each "partial" symbol table in OBJFILE
     where the source file is named NAME.

     If NAME is not absolute, a match after a '/' in the symbol table's
     file name will also work, REAL_PATH is NULL then.  If NAME is
     absolute then REAL_PATH is non-NULL absolute file name as resolved
     via gdb_realpath from NAME.

     If a match is found, the "partial" symbol table is expanded.
     Then, this calls iterate_over_some_symtabs (or equivalent) over
     all newly-created symbol tables, passing CALLBACK to it.
     The result of this call is returned.  */
  virtual bool map_symtabs_matching_filename
    (struct objfile *objfile, const char *name, const char *real_path,
     gdb::function_view<bool (symtab *)> callback) = 0;

  /* Check to see if the symbol is defined in a "partial" symbol table
     of OBJFILE.  BLOCK_INDEX should be either GLOBAL_BLOCK or STATIC_BLOCK,
     depending on whether we want to search global symbols or static
     symbols.  NAME is the name of the symbol to look for.  DOMAIN
     indicates what sort of symbol to search for.

     Returns the newly-expanded compunit in which the symbol is
     defined, or NULL if no such symbol table exists.  If OBJFILE
     contains !TYPE_OPAQUE symbol prefer its compunit.  If it contains
     only TYPE_OPAQUE symbol(s), return at least that compunit.  */
  virtual struct compunit_symtab *lookup_symbol (struct objfile *objfile,
						 block_enum block_index,
						 const char *name,
						 domain_enum domain) = 0;

  /* Check to see if the global symbol is defined in a "partial" symbol table
     of OBJFILE. NAME is the name of the symbol to look for.  DOMAIN
     indicates what sort of symbol to search for.

     If found, sets *symbol_found_p to true and returns the symbol language.
     defined, or NULL if no such symbol table exists.  */
  virtual enum language lookup_global_symbol_language
       (struct objfile *objfile,
	const char *name,
	domain_enum domain,
	bool *symbol_found_p) = 0;

  /* Print statistics about any indices loaded for OBJFILE.  The
     statistics should be printed to gdb_stdout.  This is used for
     "maint print statistics".  Statistics are printed in two
     sections.  PRINT_BCACHE is false when printing the first section
     of general statistics, and true when printing bcache statistics.  */
  virtual void print_stats (struct objfile *objfile, bool print_bcache) = 0;

  /* Dump any indices loaded for OBJFILE.  The dump should go to
     gdb_stdout.  This is used for "maint print objfiles".  */
  virtual void dump (struct objfile *objfile) = 0;

  /* Find all the symbols in OBJFILE named FUNC_NAME, and ensure that
     the corresponding symbol tables are loaded.  */
  virtual void expand_symtabs_for_function (struct objfile *objfile,
					    const char *func_name) = 0;

  /* Read all symbol tables associated with OBJFILE.  */
  virtual void expand_all_symtabs (struct objfile *objfile) = 0;

  /* Read all symbol tables associated with OBJFILE which have
     symtab_to_fullname equal to FULLNAME.
     This is for the purposes of examining code only, e.g., expand_line_sal.
     The routine may ignore debug info that is known to not be useful with
     code, e.g., DW_TAG_type_unit for dwarf debug info.  */
  virtual void expand_symtabs_with_fullname (struct objfile *objfile,
					     const char *fullname) = 0;

  /* Find global or static symbols in all tables that are in DOMAIN
     and for which MATCH (symbol name, NAME) == 0, passing each to 
     CALLBACK, reading in partial symbol tables as needed.  Look
     through global symbols if GLOBAL and otherwise static symbols.
     Passes NAME and NAMESPACE to CALLBACK with each symbol
     found.  After each block is processed, passes NULL to CALLBACK.
     MATCH must be weaker than strcmp_iw_ordered in the sense that
     strcmp_iw_ordered(x,y) == 0 --> MATCH(x,y) == 0.  ORDERED_COMPARE,
     if non-null, must be an ordering relation compatible with
     strcmp_iw_ordered in the sense that
	    strcmp_iw_ordered(x,y) == 0 --> ORDERED_COMPARE(x,y) == 0
     and 
	    strcmp_iw_ordered(x,y) <= 0 --> ORDERED_COMPARE(x,y) <= 0
     (allowing strcmp_iw_ordered(x,y) < 0 while ORDERED_COMPARE(x, y) == 0).
     CALLBACK returns true to indicate that the scan should continue, or
     false to indicate that the scan should be terminated.  */

  virtual void map_matching_symbols
    (struct objfile *,
     const lookup_name_info &lookup_name,
     domain_enum domain,
     int global,
     gdb::function_view<symbol_found_callback_ftype> callback,
     symbol_compare_ftype *ordered_compare) = 0;

  /* Expand all symbol tables in OBJFILE matching some criteria.

     FILE_MATCHER is called for each file in OBJFILE.  The file name
     is passed to it.  If the matcher returns false, the file is
     skipped.  If FILE_MATCHER is NULL the file is not skipped.  If
     BASENAMES is true the matcher should consider only file base
     names (the passed file name is already only the lbasename'd
     part).

     If the file is not skipped, and SYMBOL_MATCHER and LOOKUP_NAME are NULL,
     the symbol table is expanded.

     Otherwise, individual symbols are considered.

     If KIND does not match, the symbol is skipped.

     If the symbol name does not match LOOKUP_NAME, the symbol is skipped.

     If SYMBOL_MATCHER returns false, then the symbol is skipped.

     Otherwise, the symbol's symbol table is expanded.  */
  virtual void expand_symtabs_matching
    (struct objfile *objfile,
     gdb::function_view<expand_symtabs_file_matcher_ftype> file_matcher,
     const lookup_name_info *lookup_name,
     gdb::function_view<expand_symtabs_symbol_matcher_ftype> symbol_matcher,
     gdb::function_view<expand_symtabs_exp_notify_ftype> expansion_notify,
     enum search_domain kind) = 0;

  /* Return the comp unit from OBJFILE that contains PC and
     SECTION.  Return NULL if there is no such compunit.  This
     should return the compunit that contains a symbol whose
     address exactly matches PC, or, if there is no exact match, the
     compunit that contains a symbol whose address is closest to
     PC.  */
  virtual struct compunit_symtab *find_pc_sect_compunit_symtab
    (struct objfile *objfile, struct bound_minimal_symbol msymbol,
     CORE_ADDR pc, struct obj_section *section, int warn_if_readin) = 0;

  /* Return the comp unit from OBJFILE that contains a symbol at
     ADDRESS.  Return NULL if there is no such comp unit.  Unlike
     find_pc_sect_compunit_symtab, any sort of symbol (not just text
     symbols) can be considered, and only exact address matches are
     considered.  */
  virtual struct compunit_symtab *find_compunit_symtab_by_address
    (struct objfile *objfile, CORE_ADDR address) = 0;

  /* Call a callback for every file defined in OBJFILE whose symtab is
     not already read in.  FUN is the callback.  It is passed the file's
     FILENAME, the file's FULLNAME (if need_fullname is non-zero), and
     the DATA passed to this function.  */
  virtual void map_symbol_filenames (struct objfile *objfile,
				     symbol_filename_ftype *fun, void *data,
				     int need_fullname) = 0;

  /* This is called when the objfile is relocated.  It can be used to
     clean up any internal caches.  */
  virtual void relocated ()
  {
    /* Do nothing.  */
  }

  /* Return true if this class can lazily read the symbols.  This may
     only return true if there are in fact symbols to be read, because
     this is used in the implementation of 'has_partial_symbols'.  */
  virtual bool can_lazily_read_symbols ()
  {
    return false;
  }

  /* Read the partial symbols for OBJFILE.  This will only ever be
     called if can_lazily_read_symbols returns true.  */
  virtual void read_partial_symbols (struct objfile *objfile)
  {
  }
};

typedef std::unique_ptr<quick_symbol_functions> quick_symbol_functions_up;

#endif /* GDB_QUICK_SYMBOL_H */
