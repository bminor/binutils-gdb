/* hash.c -- hash table routines for BFD
   Copyright 1993 Free Software Foundation, Inc.
   Written by Steve Chamberlain <sac@cygnus.com>

This file is part of GLD, the Gnu Linker.

GLD is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GLD is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GLD; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"
#include "obstack.h"

/* Obstack allocation and deallocation routines.  */
#define obstack_chunk_alloc bfd_xmalloc_by_size_t
#define obstack_chunk_free free

/* The default number of entries to use when creating a hash table.  */
#define DEFAULT_SIZE (4051)

/* Create a new hash table, given a number of entries.  */

boolean
bfd_hash_table_init_n (table, newfunc, size)
     struct bfd_hash_table *table;
     struct bfd_hash_entry *(*newfunc) PARAMS ((struct bfd_hash_entry *,
						struct bfd_hash_table *,
						const char *));
     unsigned int size;
{
  unsigned int alloc;

  alloc = size * sizeof (struct bfd_hash_entry *);
  obstack_begin (&table->memory, alloc);
  table->table = ((struct bfd_hash_entry **)
		  obstack_alloc (&table->memory, alloc));
  memset ((PTR) table->table, 0, alloc);
  table->size = size;
  table->newfunc = newfunc;
  return true;
}

/* Create a new hash table with the default number of entries.  */

boolean
bfd_hash_table_init (table, newfunc)
     struct bfd_hash_table *table;
     struct bfd_hash_entry *(*newfunc) PARAMS ((struct bfd_hash_entry *,
						struct bfd_hash_table *,
						const char *));
{
  return bfd_hash_table_init_n (table, newfunc, DEFAULT_SIZE);
}

/* Free a hash table.  */

void
bfd_hash_table_free (table)
     struct bfd_hash_table *table;
{
  obstack_free (&table->memory, (PTR) NULL);
}

/* Look up a string in a hash table.  */

struct bfd_hash_entry *
bfd_hash_lookup (table, string, create, copy)
     struct bfd_hash_table *table;
     const char *string;
     boolean create;
     boolean copy;
{
  register const unsigned char *s;
  register unsigned long hash;
  register unsigned int c;
  struct bfd_hash_entry *hashp;
  unsigned int len;
  unsigned int index;
  
  hash = 0;
  len = 0;
  s = (const unsigned char *) string;
  while ((c = *s++) != '\0')
    {
      hash += c + (c << 17);
      hash ^= hash >> 2;
      ++len;
    }
  hash += len + (len << 17);
  hash ^= hash >> 2;

  index = hash % table->size;
  for (hashp = table->table[index];
       hashp != (struct bfd_hash_entry *) NULL;
       hashp = hashp->next)
    {
      if (hashp->hash == hash
	  && strcmp (hashp->string, string) == 0)
	return hashp;
    }

  if (! create)
    return (struct bfd_hash_entry *) NULL;

  hashp = (*table->newfunc) ((struct bfd_hash_entry *) NULL, table, string);
  if (hashp == (struct bfd_hash_entry *) NULL)
    return (struct bfd_hash_entry *) NULL;
  if (copy)
    {
      char *new;

      new = (char *) obstack_alloc (&table->memory, len + 1);
      strcpy (new, string);
      string = new;
    }
  hashp->string = string;
  hashp->hash = hash;
  hashp->next = table->table[index];
  table->table[index] = hashp;

  return hashp;
}

/* Base method for creating a new hash table entry.  */

/*ARGSUSED*/
struct bfd_hash_entry *
bfd_hash_newfunc (entry, table, string)
     struct bfd_hash_entry *entry;
     struct bfd_hash_table *table;
     const char *string;
{
  if (entry == (struct bfd_hash_entry *) NULL)
    entry = ((struct bfd_hash_entry *)
	     bfd_hash_allocate (table, sizeof (struct bfd_hash_entry)));
  return entry;
}

/* Allocate space in a hash table.  */

PTR
bfd_hash_allocate (table, size)
     struct bfd_hash_table *table;
     size_t size;
{
  return obstack_alloc (&table->memory, size);
}

/* Traverse a hash table.  */

void
bfd_hash_traverse (table, func, info)
     struct bfd_hash_table *table;
     boolean (*func) PARAMS ((struct bfd_hash_entry *, PTR));
     PTR info;
{
  unsigned int i;

  for (i = 0; i < table->size; i++)
    {
      struct bfd_hash_entry *p;

      for (p = table->table[i]; p != NULL; p = p->next)
	{
	  if (! (*func) (p, info))
	    return;
	}
    }
}
