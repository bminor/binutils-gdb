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

/* FIXME: carlton/2002-09-24: because of namespace_enum, you have to
   include symtab.h before including this file.  But I'm leaving in
   opaque declarations as if that weren't true.  (Probably the
   'namespace' argument to dict_lookup should eventually be moved back
   into lookup_block_symbol, I think.)  */

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

/* Create a dictionary implemented via a fixed-size hashtable.  All
   memory it uses is allocated on OBSTACK; the environment is
   initialized from SYMBOL_LIST.  */

extern struct dictionary *dict_create_hashed (struct obstack *obstack,
					      const struct pending
					      *symbol_list);

/* Create a dictionary implemented via a fixed-size array.  All memory
   it uses is allocated on OBSTACK; the environment is initialized
   from the SYMBOL_LIST.  The symbols are ordered in the same order
   that they're found in SYMBOL_LIST.  */

extern struct dictionary *dict_create_linear (struct obstack *obstack,
					      const struct pending
					      *symbol_list);

/* Create a dictionary implemented via an array that grows as
   necessary.  The dictionary is initially empty; to add symbols to
   it, call dict_add_symbol().  Call dict_free() when you're done with
   it.  */

/* FIXME: carlton/2002-09-11: This environment type exists only to
   make mdebugread.c and jv-lang.c happy.  The former should be
   converted over to the buildsym.c mechanisms (or made obsolete, I
   suggest in an excess of optimism); the latter should probably be
   rethought.  */

extern struct dictionary *dict_create_linear_expandable (void);

#if 0

/* Create a DICT_BLOCK dictionary pointing BLOCK.  */

extern struct dictionary *
dict_create_block (struct block *block);

/* Create a DICT_BLOCK_EXPANDABLE dictionary pointing at BLOCK.  */

extern struct dictionary *
dict_create_block_expandable (struct block *block);

#endif


/* The functions providing the interface to dictionaries.  */

/* Search DICT for symbol NAME in NAMESPACE.

   If MANGLED_NAME is non-NULL, verify that any symbol we find has this
   particular mangled name.
*/

extern struct symbol *dict_lookup (const struct dictionary *dict,
				   const char *name,
				   const char *mangled_name,
				   const namespace_enum namespace);

/* Free the memory used by a dictionary that's not on an obstack.  (If
   any.)  */

extern void dict_free (struct dictionary *dict);

/* Add a symbol to an expandable dictionary.  */

extern void dict_add_symbol (struct dictionary *dict, struct symbol *sym);

/* Is the dictionary empty?  */

extern int dict_empty (struct dictionary *dict);

#if 0

/* Special case.  */

extern struct block *dict_add_symbol_block (struct dictionary *dict,
					    struct symbol *sym);

#endif

/* A type containing data that is used when iterating over all symbols
   in a dictionary.  */

/* NOTE: carlton/2002-09-11: I originally wanted to make this opaque,
   but that led to complications.  Fortunately, it turned out that all
   implementations of dictionaries currently need to keep track of the
   same types of data (though how they interpret that data varies
   depending on the implementation), so it's really not so bad after
   all.  But code outside of dictionary.c should never examine the
   innards of a dict_iterator.  */

struct dict_iterator
{
  /* The dictionary that this iterator is associated to.  */
  const struct dictionary *dict;
  /* The next two members are data that is used in a way that depends
     on DICT's implementation type.  */
  int index;
  struct symbol *current;
};

/* Initialize ITERATOR to point at the first symbol in DICT, and
   return that first symbol, or NULL if DICT is empty.  */

extern struct symbol *dict_iterator_first (const struct dictionary *dict,
					   struct dict_iterator *iterator);

/* Advance ITERATOR, and return the next symbol, or NULL if there are
   no more symbols.  Don't call this if you've previously received
   NULL from dict_iterator_first or dict_iterator_next on this
   iteration.  */

extern struct symbol *dict_iterator_next (struct dict_iterator *iterator);

/* Macro to loop through all symbols in a dictionary DICT, in no
   particular order.  ITER is a struct dict_iterator (NOTE: __not__ a
   struct dict_iterator *), and SYM points to the current symbol.

   It's implemented as a single loop, so you can terminate the loop
   early by a break if you desire.  */

#define ALL_DICT_SYMBOLS(dict, iter, sym)			\
	for ((sym) = dict_iterator_first ((dict), &(iter));	\
	     (sym);						\
	     (sym) = dict_iterator_next (&(iter)))

/* For backwards compatibility, I suppose.  */

#define ALL_BLOCK_SYMBOLS(block, iter, sym)			\
	ALL_DICT_SYMBOLS (BLOCK_DICT (block), iter, sym)
