/* Routines for name->symbol lookups in GDB.
   
   Copyright 2002 Free Software Foundation, Inc.

   Contributed by David Carlton <carlton@bactrian.org>.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or (at
   your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* An opaque type for dictionaries; only dictionary.c should know
   about its innards.  */

struct dictionary;

/* Other types needed for declarations.  */

struct symbol;
struct obstack;
struct pending;
struct block;


/* The creation functions for various implementations of
   dictionaries.  */

/* Create a DICT_BLOCK dictionary pointing at the given block.  */

extern struct dictionary *
dict_create_block (struct block *block);

/* Create a DICT_BLOCK_EXPANDABLE dictionary pointing at the given block.  */
extern struct dictionary *
dict_create_block_expandable (struct block *block);


/* The functions providing the interface to dictionaries.  */

/* Free the memory used by a dictionary that's not on an obstack.  (If
   any.)  */
extern void dict_free (struct dictionary *dict);

/* Special case.  */

extern struct block *dict_add_symbol_block (struct dictionary *dict,
					    struct symbol *sym);
