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

#include "defs.h"
#include "dictionary.h"
#include "gdb_obstack.h"
#include "symtab.h"
#include "buildsym.h"
#include "gdb_assert.h"

/* This file implements dictionaries, which are tables that associate
   symbols to names.  They are represented by an opaque type 'struct
   dictionary'.  That type has various internal implementations, which
   you can choose between depending on what properties you need
   (e.g. fast lookup, order-preserving, expandable).

   Each dictionary starts with a 'virtual function table' that
   contains the functions that actually implement the various
   operations that dictionaries provide.  */

/* NOTE: carlton/2002-09-20: Originally, I'd had each dictionary start
   with a dict_type member, and had implemented the various functions
   via big switch statements.  But that led to some really large
   functions, and might cause problems in the future (e.g. if I were
   to provide two different allocators for a single type that either
   allocate using obstacks or using xfree(), then we'd have to expand
   the number of dict_types to get dict_free() to work), so I'm going
   with dict_vtbl instead.  I left the dict_type in the dict_vtbl,
   but it's never used: it's just there to make debugging a bit
   easier.  */

/* An enum representing the various implementations of dictionaries.
   Used only for debugging.  */

enum dict_type
  {
    /* Symbols are stored in a (fixed-size) hash table.  */
    DICT_HASHED,
    /* Symbols are stored in a (fixed-size) array.  */
    DICT_LINEAR,
    /* Symbols are stored in an expandable array.  */
    DICT_LINEAR_EXPANDABLE,
    /* Symbols are stored in a fixed-size block.  */
    DICT_BLOCK,
    /* Symbols are stored in an expandable block.  */
    DICT_BLOCK_EXPANDABLE,
  };

/* The virtual function table.  */

struct dict_vtbl
{
  /* The type of the dictionary.  This is only here to make debugging
     a bit easier; it's not actually used.  */
  enum dict_type type;
  /* The function to free a dictionary.  */
  void (*free)(struct dictionary *);
};

/* Now comes the structs used to store the data for different
   implementations.  If two implementations have data in common, put
   the common data at the top of their structs, ordered in the same
   way.  */

struct dictionary_block
{
  struct block *block;
};

struct dictionary_block_expandable
{
  struct block *block;
  unsigned int maxsyms;
};

/* And now, the star of our show.  */

struct dictionary
{
  const struct dict_vtbl *vtbl;
  union
  {
    struct dictionary_block block;
    struct dictionary_block_expandable block_expandable;
  }
  data;
};

/* Accessor macros.  */

#define DICT_VTBL(d)			(d)->vtbl

/* This can be used for block_expandables, too.  */

#define DICT_BLOCK_BLOCK(d)		(d)->data.block.block

#define DICT_BLOCK_EXPANDABLE_MAXSYMS(d) (d)->data.block_expandable.maxsyms


#define DICT_BLOCK_EXPANDABLE_INITIAL_MAXSYMS 10

/* Declarations of functions for vtbls.  */

static void dict_free_block(struct dictionary *dict);

/* Various vtbls that we'll actually use.  */

const struct dict_vtbl dict_block_vtbl =
  {
    DICT_BLOCK, dict_free_block,
  };

const struct dict_vtbl dict_block_expandable_vtbl =
  {
    DICT_BLOCK_EXPANDABLE, dict_free_block,
  };

/* The creation functions.  */

/* Allocate a dictionary in which symbol lookup is implemented via
   BLOCK.  Needs to be freed by dict_free; I won't worry about that,
   however, since this will go away soon.  */
struct dictionary *
dict_create_block (struct block *block)
{
  struct dictionary *retval = xmalloc (sizeof (struct dictionary));

  DICT_VTBL (retval) = &dict_block_vtbl;
  DICT_BLOCK_BLOCK (retval) = block;

  return retval;
}

struct dictionary *
dict_create_block_expandable (struct block *block)
{
  struct dictionary *retval = xmalloc (sizeof (struct dictionary));

  DICT_VTBL (retval) = &dict_block_expandable_vtbl;
  DICT_BLOCK_BLOCK (retval) = block;
  /* We'll resize the block the first time we add a symbol to it.  */
  DICT_BLOCK_EXPANDABLE_MAXSYMS (retval) = 0;

  return retval;
}

/* The functions providing the dictionary interface.  */

void
dict_free (struct dictionary *dict)
{
  (DICT_VTBL (dict))->free (dict);
}

/* The functions implementing the dictionary interface.  */

/* First, for DICT_BLOCK and DICT_BLOCK_EXPANDABLE.  */

static void
dict_free_block (struct dictionary *dict)
{
  xfree (dict);
}

/* A special-case function for DICT_BLOCK_EXPANDABLE.  */

/* FIXME: carlton/2002-09-20: But some callers use xmmalloc!!!
   Crap.  */

struct block *
dict_add_symbol_block (struct dictionary *dict, struct symbol *sym)
{
  gdb_assert ((DICT_VTBL (dict))->type == DICT_BLOCK_EXPANDABLE);

  struct block *block = DICT_BLOCK_BLOCK (dict);

  if (++BLOCK_NSYMS (block) > DICT_BLOCK_EXPANDABLE_MAXSYMS (dict))
    {
      if (DICT_BLOCK_EXPANDABLE_MAXSYMS (dict))
	DICT_BLOCK_EXPANDABLE_MAXSYMS (dict) *= 2;
      else
	DICT_BLOCK_EXPANDABLE_MAXSYMS (dict)
	  = DICT_BLOCK_EXPANDABLE_INITIAL_MAXSYMS;

      block = xrealloc (block,
			sizeof (struct block)
			+ ((DICT_BLOCK_EXPANDABLE_MAXSYMS (dict) -1)
			   * sizeof (struct symbol)));
      DICT_BLOCK_BLOCK (dict) = block;
    }

  BLOCK_SYM (block, BLOCK_NSYMS (block) - 1) = sym;

  return block;
}
