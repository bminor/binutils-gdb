/* Minimal symbol table definitions for GDB.

   Copyright (C) 2011 Free Software Foundation, Inc.

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

#ifndef MINSYMS_H
#define MINSYMS_H

/* Functions for dealing with the minimal symbol table, really a misc
   address<->symbol mapping for things we don't have debug symbols for.  */

void prim_record_minimal_symbol (const char *, CORE_ADDR,
				 enum minimal_symbol_type,
				 struct objfile *);

struct minimal_symbol *prim_record_minimal_symbol_full
    (const char *,
     int, int, CORE_ADDR,
     enum minimal_symbol_type,
     int section,
     asection *bfd_section,
     struct objfile *);

struct minimal_symbol *prim_record_minimal_symbol_and_info
    (const char *,
     CORE_ADDR,
     enum minimal_symbol_type,
     int section,
     asection *bfd_section,
     struct objfile *);

unsigned int msymbol_hash_iw (const char *);

unsigned int msymbol_hash (const char *);

/* Compute the next hash value from previous HASH and the character C.  This
   is only a GDB in-memory computed value with no external files compatibility
   requirements.  */

#define SYMBOL_HASH_NEXT(hash, c)			\
  ((hash) * 67 + tolower ((unsigned char) (c)) - 113)

struct objfile *msymbol_objfile (struct minimal_symbol *sym);

struct minimal_symbol *lookup_minimal_symbol (const char *,
					      const char *,
					      struct objfile *);

struct minimal_symbol *lookup_minimal_symbol_text (const char *,
						   struct objfile *);

struct minimal_symbol *lookup_minimal_symbol_solib_trampoline
    (const char *,
     struct objfile *);

struct minimal_symbol *lookup_minimal_symbol_by_pc_name
(CORE_ADDR, const char *, struct objfile *);

struct minimal_symbol *lookup_minimal_symbol_by_pc (CORE_ADDR);

struct minimal_symbol *lookup_minimal_symbol_and_objfile (const char *,
							  struct objfile **);

struct minimal_symbol *lookup_minimal_symbol_by_pc_section
    (CORE_ADDR,
     struct obj_section *);

struct minimal_symbol *lookup_solib_trampoline_symbol_by_pc (CORE_ADDR);

void init_minimal_symbol_collection (void);

struct cleanup *make_cleanup_discard_minimal_symbols (void);

void install_minimal_symbols (struct objfile *);

/* Sort all the minimal symbols in OBJFILE.  */

void msymbols_sort (struct objfile *objfile);

/* Create the terminating entry of OBJFILE's minimal symbol table.
   If OBJFILE->msymbols is zero, allocate a single entry from
   OBJFILE->objfile_obstack; otherwise, just initialize
   OBJFILE->msymbols[OBJFILE->minimal_symbol_count].  */

void terminate_minimal_symbol_table (struct objfile *objfile);

void iterate_over_minimal_symbols (struct objfile *objf,
				   const char *name,
				   void (*callback) (struct minimal_symbol *,
						     void *),
				   void *user_data);

#endif /* MINSYMS_H */
