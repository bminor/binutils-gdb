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
#include "gdb_obstack.h"
#include "symtab.h"
#include "buildsym.h"
#include "gdb_assert.h"
#include "dictionary.h"

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
  void (*free) (struct dictionary *dict);
  /* The symbol lookup function.  */
  struct symbol *(*lookup) (const struct dictionary *dict,
			    const char *name,
			    const char *mangled_name,
			    const namespace_enum namespace);
  /* Iterator functions.  */
  struct symbol *(*iterator_first) (const struct dictionary *dict,
				    struct dict_iterator *iterator);
  struct symbol *(*iterator_next) (struct dict_iterator *iterator);
  void (*add_symbol) (struct dictionary *dict, struct symbol *sym);
};

/* Now comes the structs used to store the data for different
   implementations.  If two implementations have data in common, put
   the common data at the top of their structs, ordered in the same
   way.  */

struct dictionary_hashed
{
  int nbuckets;
  struct symbol **buckets;
};

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
    struct dictionary_hashed hashed;
    struct dictionary_block block;
    struct dictionary_block_expandable block_expandable;
  }
  data;
};

/* Accessor macros.  */

#define DICT_VTBL(d)			(d)->vtbl

#define DICT_HASHED_NBUCKETS(d)		(d)->data.hashed.nbuckets
#define DICT_HASHED_BUCKETS(d)		(d)->data.hashed.buckets
#define DICT_HASHED_BUCKET(d,i)		DICT_HASHED_BUCKETS (d) [i]

/* This can be used for block_expandables, too.  */

#define DICT_BLOCK_BLOCK(d)		(d)->data.block.block

#define DICT_BLOCK_EXPANDABLE_MAXSYMS(d) (d)->data.block_expandable.maxsyms

/* The initial size of a DICT_HASHED dictionary.  */

#define DICT_HASHED_INITIAL_MAXSYMS 10
#define DICT_BLOCK_EXPANDABLE_INITIAL_MAXSYMS DICT_HASHED_INITIAL_MAXSYMS

/* This calculates the number of buckets we'll use in a hashtable,
   given the number of symbols that it will contain.  */

#define DICT_HASHTABLE_SIZE(n)	((n)/5 + 1)

/* Accessor macros for dict_iterators; they're here rather than
   dictionary.h because code elsewhere should treat dict_iterators as
   opaque.  */

/* The dictionary that the iterator is associated to.  */
#define DICT_ITERATOR_DICT(iter)		(iter)->dict
/* For linear dictionaries, the index of the last symbol returned; for
   hashed dictionaries, the bucket of the last symbol returned.  */
#define DICT_ITERATOR_INDEX(iter)		(iter)->index
/* For hashed dictionaries, this points to the last symbol returned;
   otherwise, this is unused.  */
#define DICT_ITERATOR_CURRENT(iter)		(iter)->current

/* Functions to handle some of the common code in dict_iterator_first and
   dict_iterator_next.  */

static struct symbol *iterator_hashed_advance (struct dict_iterator *iter);

static struct symbol *iterator_block_hashed_advance (struct dict_iterator
						     *iter);

/* Declarations of functions for vtbls.  */

/* Functions that might work across a range of dictionary types.  */

static void add_symbol_nonexpandable (struct dictionary *dict,
				      struct symbol *sym);

static void free_obstack (struct dictionary *dict);

/* Functions for DICT_HASHED dictionaries.  */

static struct symbol *lookup_hashed (const struct dictionary *dict,
				     const char *name,
				     const char *mangled_name,
				     const namespace_enum namespace);

static struct symbol *iterator_first_hashed (const struct dictionary *dict,
					     struct dict_iterator *iterator);

static struct symbol *iterator_next_hashed (struct dict_iterator *iterator);

/* Functions for blocks.  */

static struct symbol *lookup_block (const struct dictionary *dict,
				    const char *name,
				    const char *mangled_name,
				    const namespace_enum namespace);


static void free_block(struct dictionary *dict);

static struct symbol *iterator_first_block (const struct dictionary *dict,
					    struct dict_iterator *iterator);

static struct symbol *iterator_next_block (struct dict_iterator *iterator);

/* Various vtbls that we'll actually use.  */

const struct dict_vtbl dict_hashed_vtbl =
  {
    DICT_HASHED, free_obstack, lookup_hashed, iterator_first_hashed,
    iterator_next_hashed, add_symbol_nonexpandable,
  };

const struct dict_vtbl dict_block_vtbl =
  {
    DICT_BLOCK, free_block, lookup_block, iterator_first_block,
    iterator_next_block, add_symbol_nonexpandable,
  };

const struct dict_vtbl dict_block_expandable_vtbl =
  {
    DICT_BLOCK_EXPANDABLE, free_block, lookup_block, iterator_first_block,
    iterator_next_block, add_symbol_nonexpandable,
  };

/* The creation functions.  */

/* Create a dictionary implemented via a fixed-size hashtable.  All
   memory it uses is allocated on OBSTACK; the environment is
   initialized from SYMBOL_LIST.  */

struct dictionary *
dict_create_hashed (struct obstack *obstack,
		    const struct pending *symbol_list)
{
  struct dictionary *retval;
  int nsyms, nbuckets, i;
  struct symbol **buckets;
  const struct pending *list_counter;

  retval = obstack_alloc (obstack, sizeof (struct dictionary));
  DICT_VTBL (retval) = &dict_hashed_vtbl;

  /* Calculate the number of symbols, and allocate space for them.  */
  for (nsyms = 0, list_counter = symbol_list;
       list_counter != NULL;
       nsyms += list_counter->nsyms, list_counter = list_counter->next)
    {
      /* EMPTY */ ;
    }
  nbuckets = DICT_HASHTABLE_SIZE (nsyms);
  DICT_HASHED_NBUCKETS (retval) = nbuckets;
  buckets = obstack_alloc (obstack, nbuckets * sizeof (struct symbol *));
  memset (buckets, 0, nbuckets * sizeof (struct symbol *));
  DICT_HASHED_BUCKETS (retval) = buckets;

  /* Now fill the buckets.  */
  for (list_counter = symbol_list;
       list_counter != NULL;
       list_counter = list_counter->next)
    {
      for (i = list_counter->nsyms - 1; i >= 0; --i)
	{
	  struct symbol *sym = list_counter->symbol[i];
	  unsigned int hash_index;
	  const char *name = SYMBOL_DEMANGLED_NAME (sym);
	  if (name == NULL)
	    name = SYMBOL_NAME (sym);
	  hash_index = msymbol_hash_iw (name) % nbuckets;
	  sym->hash_next = buckets[hash_index];
	  buckets[hash_index] = sym;
	}
    }

  return retval;
}

#if 0
/* Create a dictionary implemented via a fixed-size array.  All memory
   it uses is allocated on OBSTACK; the environment is initialized
   from the SYMBOL_LIST.  The symbols are ordered in the same order
   that they're found in SYMBOL_LIST.  */

struct dictionary *
dict_create_linear (struct obstack *obstack,
		    const struct pending *symbol_list)

/* Create a dictionary implemented via an array that grows as
   necessary.  The dictionary is initially empty; to add symbols to
   it, call dict_add_symbol().  Call dict_free() when you're done with
   it.  */

/* FIXME: carlton/2002-09-11: This environment type exists only to
   make mdebugread.c and jv-lang.c happy.  The former should be
   converted over to the buildsym.c mechanisms (or made obsolete, I
   suggest in an excess of optimism); the latter should perhaps be
   rethought.  */

struct dictionary *
dict_create_linear_expandable (void)
#endif

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

/* Free the memory used by a dictionary that's not on an obstack.  (If
   any.)  */

void
dict_free (struct dictionary *dict)
{
  (DICT_VTBL (dict))->free (dict);
}

/* Search DICT for symbol NAME in NAMESPACE.

   If MANGLED_NAME is non-NULL, verify that any symbol we find has this
   particular mangled name.
*/

/* FIXME: carlton/2002-09-23: namespace_enum is a typedef in
   symtab.h...  */

struct symbol *
dict_lookup (const struct dictionary *dict,
	     const char *name,
	     const char *mangled_name,
	     const namespace_enum namespace)
{
  return (DICT_VTBL (dict))->lookup (dict, name, mangled_name, namespace);
}

/* Initialize ITERATOR to point at the first symbol in DICT, and
   return that first symbol, or NULL if DICT is empty.  */

struct symbol *
dict_iterator_first (const struct dictionary *dict,
		     struct dict_iterator *iterator)
{
  return (DICT_VTBL (dict))->iterator_first (dict, iterator);
}

/* Advance ITERATOR, and return the next symbol, or NULL if there are
   no more symbols.  */

struct symbol *
dict_iterator_next (struct dict_iterator *iterator)
{
  return (DICT_VTBL (DICT_ITERATOR_DICT (iterator)))
    ->iterator_next (iterator);
}

/* Test to see if DICT is empty.  I could put this in the vtable, but
   it's easy enough to do generically and doesn't get called a
   lot.  */

extern int
dict_empty (struct dictionary *dict)
{
  struct dict_iterator iter;

  return (dict_iterator_first (dict, &iter) == NULL);
}

/* Add SYM to DICT.  DICT had better be expandable.  */

extern void
dict_add_symbol (struct dictionary *dict, struct symbol *sym)
{
  (DICT_VTBL (dict))->add_symbol (dict, sym);
}


/* The functions implementing the dictionary interface.  */

/* Generic functions, where appropriate.  */

static void
free_obstack (struct dictionary *dict)
{
  /* Do nothing!  */
}

static void
add_symbol_nonexpandable (struct dictionary *dict, struct symbol *sym)
{
  internal_error (__FILE__, __LINE__,
		  "dict_add_symbol: non-expandable dictionary");
}

/* Functions for DICT_HASHED.  */

static struct symbol *
lookup_hashed (const struct dictionary *dict,
	       const char *name,
	       const char *mangled_name,
	       const namespace_enum namespace)
{
  unsigned int hash_index
    = msymbol_hash_iw (name) % DICT_HASHED_NBUCKETS (dict);
  struct symbol *sym;
  
  for (sym = DICT_HASHED_BUCKET (dict, hash_index);
       sym;
       sym = sym->hash_next)
    {
      if (SYMBOL_NAMESPACE (sym) == namespace
	  && (mangled_name
	      ? strcmp (SYMBOL_NAME (sym), mangled_name) == 0
	      : SYMBOL_MATCHES_NAME (sym, name)))
	return sym;
    }
  
  return NULL;
}

static struct symbol *
iterator_first_hashed (const struct dictionary *dict,
		       struct dict_iterator *iterator)
{
  DICT_ITERATOR_DICT (iterator) = dict;
  DICT_ITERATOR_INDEX (iterator) = -1;
  return iterator_hashed_advance (iterator);
}

static struct symbol *
iterator_next_hashed (struct dict_iterator *iterator)
{
  const struct dictionary *dict = DICT_ITERATOR_DICT (iterator);
  struct symbol *next;

  /* FIXME: carlton/2002-09-23: Should I assert that
     DICT_ITERATOR_CURRENT (iterator) != NULL?  */
  next = DICT_ITERATOR_CURRENT (iterator)->hash_next;
  
  if (next == NULL)
    return iterator_hashed_advance (iterator);
  else
    {
      DICT_ITERATOR_CURRENT (iterator) = next;
      return next;
    }
}

static struct symbol *
iterator_hashed_advance (struct dict_iterator *iterator)
{
  const struct dictionary *dict = DICT_ITERATOR_DICT (iterator);
  int nbuckets = DICT_HASHED_NBUCKETS (dict);
  int i;

  for (i = DICT_ITERATOR_INDEX (iterator) + 1; i < nbuckets; ++i)
    {
      struct symbol *sym = DICT_HASHED_BUCKET (dict, i);
      
      if (sym != NULL)
	{
	  DICT_ITERATOR_INDEX (iterator) = i;
	  DICT_ITERATOR_CURRENT (iterator) = sym;
	  return sym;
	}
    }

  return NULL;
}

/* Functions for DICT_BLOCK and DICT_BLOCK_EXPANDABLE.  */

static void
free_block (struct dictionary *dict)
{
  xfree (dict);
}

static struct symbol *
lookup_block (const struct dictionary *dict,
	      const char *name,
	      const char *mangled_name,
	      const namespace_enum namespace)
{
  struct block *block = DICT_BLOCK_BLOCK (dict);
  register int bot, top;
  register struct symbol *sym;
  register struct symbol *sym_found = NULL;

  if (BLOCK_HASHTABLE (block))
    {
      unsigned int hash_index;
      hash_index = msymbol_hash_iw (name);
      hash_index = hash_index % BLOCK_BUCKETS (block);
      for (sym = BLOCK_BUCKET (block, hash_index); sym; sym = sym->hash_next)
	{
	  if (SYMBOL_NAMESPACE (sym) == namespace 
	      && (mangled_name
		  ? strcmp (SYMBOL_NAME (sym), mangled_name) == 0
		  : SYMBOL_MATCHES_NAME (sym, name)))
	    return sym;
	}
      return NULL;
    }
  else
    {
      /* Note that parameter symbols do not always show up last in the
	 list.  This loop makes sure to take anything else other than
	 parameter symbols first; it only uses parameter symbols as a
	 last resort.  Note that this only takes up extra computation
	 time on a match.  */
      top = BLOCK_NSYMS (block);
      bot = 0;
      while (bot < top)
	{
	  sym = BLOCK_SYM (block, bot);
	  /* If there is more than one symbol with the right name and
	     namespace, we return the first one; I believe it is now
	     impossible for us to encounter two symbols with the same
	     name and namespace here, because blocks containing
	     argument symbols are no longer sorted.  The exception is
	     for C++, where multiple functions (cloned constructors /
	     destructors, in particular) can have the same demangled
	     name.  So if we have a particular mangled name to match,
	     try to do so.  */
	  if (SYMBOL_NAMESPACE (sym) == namespace
	      && (mangled_name
		  ? strcmp (SYMBOL_NAME (sym), mangled_name) == 0
		  : SYMBOL_MATCHES_NAME (sym, name)))
	    {
#if 0
	      /* FIXME: carlton/2002-09-11: According to
		 <http://sources.redhat.com/ml/gdb/2002-03/msg00232.html>,
		 the SYMBOL_ALIASES stuff is unused, and it makes
		 the code messier, so I'm #if'ing it out here.  */

	      /* If SYM has aliases, then use any alias that is active
	         at the current PC.  If no alias is active at the current
	         PC, then use the main symbol.

	         ?!? Is checking the current pc correct?  Is this routine
	         ever called to look up a symbol from another context?

		 FIXME: No, it's not correct.  If someone sets a
		 conditional breakpoint at an address, then the
		 breakpoint's `struct expression' should refer to the
		 `struct symbol' appropriate for the breakpoint's
		 address, which may not be the PC.

		 Even if it were never called from another context,
		 it's totally bizarre for lookup_symbol's behavior to
		 depend on the value of the inferior's current PC.  We
		 should pass in the appropriate PC as well as the
		 block.  The interface to lookup_symbol should change
		 to require the caller to provide a PC.  */

	      if (SYMBOL_ALIASES (sym))
		sym = find_active_alias (sym, read_pc ());
#endif /* 0 */

	      sym_found = sym;
	      if (SYMBOL_CLASS (sym) != LOC_ARG &&
		  SYMBOL_CLASS (sym) != LOC_LOCAL_ARG &&
		  SYMBOL_CLASS (sym) != LOC_REF_ARG &&
		  SYMBOL_CLASS (sym) != LOC_REGPARM &&
		  SYMBOL_CLASS (sym) != LOC_REGPARM_ADDR &&
		  SYMBOL_CLASS (sym) != LOC_BASEREG_ARG)
		{
		  break;
		}
	    }
	  bot++;
	}
      return (sym_found);		/* Will be NULL if not found. */
    }  
}

static struct symbol *
iterator_first_block (const struct dictionary *dict,
		      struct dict_iterator *iterator)
{
  struct block *block = DICT_BLOCK_BLOCK (dict);

  DICT_ITERATOR_DICT (iterator) = dict;
	
  if (BLOCK_HASHTABLE (block))
    {
      DICT_ITERATOR_INDEX (iterator) = -1;
      return iterator_block_hashed_advance (iterator);
    }
  else
    {
      DICT_ITERATOR_INDEX (iterator) = 0;
      return BLOCK_NSYMS (block) ? BLOCK_SYM (block, 0) : NULL;
    }
}

static struct symbol *
iterator_next_block (struct dict_iterator *iterator)
{
  const struct dictionary *dict = DICT_ITERATOR_DICT (iterator);
  struct block *block = DICT_BLOCK_BLOCK (dict);
	
  if (BLOCK_HASHTABLE (block))
    {
      struct symbol *next = DICT_ITERATOR_CURRENT (iterator)->hash_next;
	    
      if (next == NULL)
	return iterator_block_hashed_advance (iterator);
      else
	{
	  DICT_ITERATOR_CURRENT (iterator) = next;
	  return next;
	}
    }
  else
    {
      if (++(DICT_ITERATOR_INDEX (iterator)) >= BLOCK_NSYMS (block))
	return NULL;
      else
	return BLOCK_SYM (block, DICT_ITERATOR_INDEX (iterator));
    }
}

/* A helper function for iterator_first_block and
   iterator_first_block_next_block.  Search for the next nonempty
   bucket; update iterator accordingly, and return it.  */

static struct symbol *
iterator_block_hashed_advance (struct dict_iterator *iterator)
{
  struct block *block =
    DICT_BLOCK_BLOCK (DICT_ITERATOR_DICT (iterator));
  int nbuckets = BLOCK_BUCKETS (block);
  int i;

  for (i = DICT_ITERATOR_INDEX (iterator) + 1; i < nbuckets; ++i)
    {
      struct symbol *sym = BLOCK_BUCKET (block, i);
      
      if (sym != NULL)
	{
	  DICT_ITERATOR_INDEX (iterator) = i;
	  DICT_ITERATOR_CURRENT (iterator) = sym;
	  return sym;
	}
    }

  return NULL;
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
