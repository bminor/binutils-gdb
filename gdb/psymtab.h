/* Public partial symbol table definitions.

   Copyright (C) 2009-2019 Free Software Foundation, Inc.

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

#ifndef PSYMTAB_H
#define PSYMTAB_H

#include "symfile.h"
#include "common/next-iterator.h"

/* A bcache for partial symbols.  */

struct psymbol_bcache;

extern struct psymbol_bcache *psymbol_bcache_init (void);
extern void psymbol_bcache_free (struct psymbol_bcache *);
extern struct bcache *psymbol_bcache_get_bcache (struct psymbol_bcache *);

extern const struct quick_symbol_functions psym_functions;

extern const struct quick_symbol_functions dwarf2_gdb_index_functions;
extern const struct quick_symbol_functions dwarf2_debug_names_functions;

/* A range adapter that makes it possible to iterate over all
   psymtabs in one objfile.  */

class objfile_psymtabs : public next_adapter<struct partial_symtab>
{
public:

  explicit objfile_psymtabs (struct objfile *objfile)
    : next_adapter<struct partial_symtab> (objfile->psymtabs)
  {
  }
};

/* Ensure that the partial symbols for OBJFILE have been loaded.  If
   VERBOSE is non-zero, then this will print a message when symbols
   are loaded.  This function returns a range adapter suitable for
   iterating over the psymtabs of OBJFILE.  */

extern objfile_psymtabs require_partial_symbols (struct objfile *objfile,
						 int verbose);

#endif /* PSYMTAB_H */
