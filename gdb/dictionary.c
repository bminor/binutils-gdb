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
   operations that dictionaries provide.

   To add a new dictionary implementation <impl>, what you should do
   is:

   * Add a new element DICT_<IMPL> to dict_type.
   
   * Create a new structure dictionary_<impl>.  If your new
   implementation is a variant of an existing one, make sure that
   their structs have the same initial data members.  Define accessor
   macros for your new data members.

   * Implement all the functions in dict_vtbl as static functions,
   whose name is the same as the corresponding member of dict_vtbl
   plus _<impl>.  You don't have to do this for those members where
   you can reuse existing generic functions
   (e.g. add_symbol_nonexpandable, free_obstack) or in the case where
   your new implementation is a variant of an existing implementation
   and where the variant doesn't affect the member function in
   question.

   * Define a static const struct dict_vtbl dict_<impl>_vtbl.

   * Define a function dict_create_<impl> to create these
   gizmos.  Add its declaration to dictionary.h.

   To add a new operation <op> on all existing implementations, what
   you should do is:

   * Add a new member <op> to struct dict_vtbl.

   * If there is useful generic behavior <op>, define a static
   function <op>_something_informative that implements that behavior.
   (E.g. add_symbol_nonexpandable, free_obstack.)

   * For every implementation <impl> that should have its own specific
   behavior for <op>, define a static function <op>_<impl>
   implementing it.

   * Modify all existing dict_vtbl_<impl>'s to include the appropriate
   member.

   * Define a function dict_<op> that looks up <op> in the dict_vtbl
   and calls the appropriate function.  Add a declaration for
   dict_<op> to dictionary.h.
   
*/

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
			    const char *name);
  /* Iterator functions.  */
  struct symbol *(*iterator_first) (const struct dictionary *dict,
				    struct dict_iterator *iterator);
  struct symbol *(*iterator_next) (struct dict_iterator *iterator);
  void (*add_symbol) (struct dictionary *dict, struct symbol *sym);
  struct symbol *(*iter_name_first) (const struct dictionary *dict,
				     const char *name,
				     struct dict_iterator *iterator);
  struct symbol *(*iter_name_next) (const char *name,
				    struct dict_iterator *iterator);
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

struct dictionary_linear
{
  int nsyms;
  struct symbol **syms;
};

/* In this implementation, symbols are stored in an array that grows
   as necessary.  Note: the entries are ordered so that its initial
   segment matches dictionary_linear.  */

struct dictionary_linear_expandable
{
  /* How many symbols we currently have.  */
  int nsyms;
  struct symbol **syms;
  /* How many symbols we can store before needing to reallocate.  */
  int capacity;
};

/* And now, the star of our show.  */

struct dictionary
{
  const struct dict_vtbl *vtbl;
  union
  {
    struct dictionary_hashed hashed;
    struct dictionary_linear linear;
    struct dictionary_linear_expandable linear_expandable;
  }
  data;
};

/* Accessor macros.  */

#define DICT_VTBL(d)			(d)->vtbl

#define DICT_HASHED_NBUCKETS(d)		(d)->data.hashed.nbuckets
#define DICT_HASHED_BUCKETS(d)		(d)->data.hashed.buckets
#define DICT_HASHED_BUCKET(d,i)		DICT_HASHED_BUCKETS (d) [i]

/* These can be used for DICT_LINEAR_EXPANDABLEs, too.  */

#define DICT_LINEAR_NSYMS(d)		(d)->data.linear.nsyms
#define DICT_LINEAR_SYMS(d)		(d)->data.linear.syms
#define DICT_LINEAR_SYM(d,i)		DICT_LINEAR_SYMS (d) [i]

#define DICT_LINEAR_EXPANDABLE_CAPACITY(d) \
		(d)->data.linear_expandable.capacity

/* The initial size of a DICT_LINEAR_EXPANDABLE dictionary.  */

#define DICT_LINEAR_EXPANDABLE_INITIAL_CAPACITY 10

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

/* Declarations of functions for vtbls.  */

/* Functions that might work across a range of dictionary types.  */

static void add_symbol_nonexpandable (struct dictionary *dict,
				      struct symbol *sym);

static void free_obstack (struct dictionary *dict);

/* Functions for DICT_HASHED dictionaries.  */

static struct symbol *lookup_hashed (const struct dictionary *dict,
				     const char *name);

static struct symbol *iterator_first_hashed (const struct dictionary *dict,
					     struct dict_iterator *iterator);

static struct symbol *iterator_next_hashed (struct dict_iterator *iterator);

static struct symbol *iter_name_first_hashed (const struct dictionary *dict,
					      const char *name,
					      struct dict_iterator *iterator);

static struct symbol *iter_name_next_hashed (const char *name,
					     struct dict_iterator *iterator);

/* Functions for DICT_LINEAR and DICT_LINEAR_EXPANDABLE
   dictionaries.  */

static struct symbol *lookup_linear (const struct dictionary *dict,
				     const char *name);

static struct symbol *iterator_first_linear (const struct dictionary *dict,
					     struct dict_iterator *iterator);

static struct symbol *iterator_next_linear (struct dict_iterator *iterator);

static struct symbol *iter_name_first_linear (const struct dictionary *dict,
					      const char *name,
					      struct dict_iterator *iterator);

static struct symbol *iter_name_next_linear (const char *name,
					     struct dict_iterator *iterator);


/* Functions only for DICT_LINEAR_EXPANDABLE.  */

static void free_linear_expandable (struct dictionary *dict);

static void add_symbol_linear_expandable (struct dictionary *dict,
					  struct symbol *sym);

/* Various vtbls that we'll actually use.  */

static const struct dict_vtbl dict_hashed_vtbl =
  {
    DICT_HASHED, free_obstack, lookup_hashed, iterator_first_hashed,
    iterator_next_hashed, add_symbol_nonexpandable, iter_name_first_hashed,
    iter_name_next_hashed,
  };

static const struct dict_vtbl dict_linear_vtbl =
  {
    DICT_LINEAR, free_obstack, lookup_linear, iterator_first_linear,
    iterator_next_linear, add_symbol_nonexpandable, iter_name_first_linear,
    iter_name_next_linear,
  };

static const struct dict_vtbl dict_linear_expandable_vtbl =
  {
    DICT_LINEAR_EXPANDABLE, free_linear_expandable, lookup_linear,
    iterator_first_linear, iterator_next_linear,
    add_symbol_linear_expandable, iter_name_first_linear,
    iter_name_next_linear,
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
	  hash_index = msymbol_hash_iw (SYMBOL_BEST_NAME (sym)) % nbuckets;
	  sym->hash_next = buckets[hash_index];
	  buckets[hash_index] = sym;
	}
    }

  return retval;
}

/* Create a dictionary implemented via a fixed-size array.  All memory
   it uses is allocated on OBSTACK; the environment is initialized
   from the SYMBOL_LIST.  The symbols are ordered in the same order
   that they're found in SYMBOL_LIST.  */

struct dictionary *
dict_create_linear (struct obstack *obstack,
		    const struct pending *symbol_list)
{
  struct dictionary *retval;
  int nsyms, i, j;
  struct symbol **syms;
  const struct pending *list_counter;

  retval = obstack_alloc (obstack, sizeof (struct dictionary));
  DICT_VTBL (retval) = &dict_linear_vtbl;

  /* Calculate the number of symbols, and allocate space for them.  */
  for (nsyms = 0, list_counter = symbol_list;
       list_counter != NULL;
       nsyms += list_counter->nsyms, list_counter = list_counter->next)
    {
      /* EMPTY */ ;
    }
  DICT_LINEAR_NSYMS (retval) = nsyms;
  syms = obstack_alloc (obstack, nsyms * sizeof (struct symbol *));
  DICT_LINEAR_SYMS (retval) = syms;

  /* Now fill in the symbols.  Start filling in from the back, so as
     to preserve the original order of the symbols.  */
  for (list_counter = symbol_list, j = nsyms - 1;
       list_counter != NULL;
       list_counter = list_counter->next)
    {
      for (i = list_counter->nsyms - 1;
	   i >= 0;
	   --i, --j)
	{
	  syms[j] = list_counter->symbol[i];
	}
    }

  return retval;
}

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
{
  struct dictionary *retval;

  retval = xmalloc (sizeof (struct dictionary));
  DICT_VTBL (retval) = &dict_linear_expandable_vtbl;
  DICT_LINEAR_NSYMS (retval) = 0;
  DICT_LINEAR_EXPANDABLE_CAPACITY (retval)
    = DICT_LINEAR_EXPANDABLE_INITIAL_CAPACITY;
  DICT_LINEAR_SYMS (retval)
    = xmalloc (DICT_LINEAR_EXPANDABLE_CAPACITY (retval)
	       * sizeof (struct symbol *));

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
	     const char *name)
{
  return (DICT_VTBL (dict))->lookup (dict, name);
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

int
dict_empty (struct dictionary *dict)
{
  struct dict_iterator iter;

  return (dict_iterator_first (dict, &iter) == NULL);
}

/* Add SYM to DICT.  DICT had better be expandable.  */

void
dict_add_symbol (struct dictionary *dict, struct symbol *sym)
{
  (DICT_VTBL (dict))->add_symbol (dict, sym);
}

struct symbol *
dict_iter_name_first (const struct dictionary *dict,
		      const char *name,
		      struct dict_iterator *iterator)
{
  return (DICT_VTBL (dict))->iter_name_first (dict, name, iterator);
}

struct symbol *
dict_iter_name_next (const char *name, struct dict_iterator *iterator)
{
  return (DICT_VTBL (DICT_ITERATOR_DICT (iterator)))
    ->iter_name_next (name, iterator);
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
	       const char *name)
{
  unsigned int hash_index
    = msymbol_hash_iw (name) % DICT_HASHED_NBUCKETS (dict);
  struct symbol *sym;
  
  for (sym = DICT_HASHED_BUCKET (dict, hash_index);
       sym;
       sym = sym->hash_next)
    {
      /* Warning: the order of arguments to strcmp_iw matters!  */
      if (strcmp_iw (SYMBOL_BEST_NAME (sym), name) == 0)
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

  /* FIXME: carlton/2002-09-23: Should I gdb_assert that
     DICT_ITERATOR_CURRENT (iterator) != NULL?  (Ditto for
     iter_name_next_hashed.)  */
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

static struct symbol *
iter_name_first_hashed (const struct dictionary *dict,
			const char *name,
			struct dict_iterator *iterator)
{
  DICT_ITERATOR_DICT (iterator) = dict;
  DICT_ITERATOR_CURRENT (iterator) = lookup_hashed (dict, name);

  return DICT_ITERATOR_CURRENT (iterator);
}

static struct symbol *
iter_name_next_hashed (const char *name, struct dict_iterator *iterator)
{
  struct symbol *next;

  for (next = DICT_ITERATOR_CURRENT (iterator)->hash_next;
       next; next = next->hash_next)
    {
      if (strcmp_iw (SYMBOL_BEST_NAME (next), name) == 0)
	break;
    }

  DICT_ITERATOR_CURRENT (iterator) = next;

  return next;
}

/* Functions for DICT_LINEAR and DICT_LINEAR_EXPANDABLE.  */

static struct symbol *
lookup_linear (const struct dictionary *dict, const char *name)
{
  /* NOTE: carlton/2002-09-26: I don't expect this to get called much,
     so let's just use iter_name_first_linear.  */

  struct dict_iterator iter;

  return iter_name_first_linear (dict, name, &iter);
}

static struct symbol *
iterator_first_linear (const struct dictionary *dict,
		       struct dict_iterator *iterator)
{
  DICT_ITERATOR_DICT (iterator) = dict;
  DICT_ITERATOR_INDEX (iterator) = 0;
  return DICT_LINEAR_NSYMS (dict) ? DICT_LINEAR_SYM (dict, 0) : NULL;
}

static struct symbol *
iterator_next_linear (struct dict_iterator *iterator)
{
  const struct dictionary *dict = DICT_ITERATOR_DICT (iterator);

  if (++DICT_ITERATOR_INDEX (iterator) >= DICT_LINEAR_NSYMS (dict))
    return NULL;
  else
    return DICT_LINEAR_SYM (dict, DICT_ITERATOR_INDEX (iterator));
}

static struct symbol *
iter_name_first_linear (const struct dictionary *dict,
			const char *name,
			struct dict_iterator *iterator)
{
  DICT_ITERATOR_DICT (iterator) = dict;
  DICT_ITERATOR_INDEX (iterator) = -1;

  return iter_name_next_linear (name, iterator);
}

static struct symbol *
iter_name_next_linear (const char *name, struct dict_iterator *iterator)
{
  const struct dictionary *dict = DICT_ITERATOR_DICT (iterator);
  int i, nsyms = DICT_LINEAR_NSYMS (dict);
  struct symbol *sym, *retval = NULL;

  for (i = DICT_ITERATOR_INDEX (iterator) + 1; i < nsyms; ++i)
    {
      sym = DICT_LINEAR_SYM (dict, i);
      if (strcmp_iw (SYMBOL_BEST_NAME (sym), name) == 0)
	{
	  retval = sym;
	  break;
	}
    }

  DICT_ITERATOR_INDEX (iterator) = i;
  
  return retval;
}

/* Functions only for DICT_LINEAR_EXPANDABLE.  */

static void
free_linear_expandable (struct dictionary *dict)
{
  xfree (DICT_LINEAR_SYMS (dict));
  xfree (dict);
}


static void
add_symbol_linear_expandable (struct dictionary *dict,
			      struct symbol *sym)
{
  int nsyms = ++DICT_LINEAR_NSYMS (dict);

  /* Do we have enough room?  If not, grow it.  */
  if (nsyms > DICT_LINEAR_EXPANDABLE_CAPACITY (dict)) {
    DICT_LINEAR_EXPANDABLE_CAPACITY (dict) *= 2;
    DICT_LINEAR_SYMS (dict)
      = xrealloc (DICT_LINEAR_SYMS (dict),
		  DICT_LINEAR_EXPANDABLE_CAPACITY (dict)
		  * sizeof (struct symbol *));
  }

  DICT_LINEAR_SYM (dict, nsyms - 1) = sym;
}
