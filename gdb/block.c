/* Block support for the GNU debugger, GDB.

   Copyright 2002 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include "defs.h"
#include "block.h"
#include "gdb_obstack.h"
#include "cp-support.h"

static void block_initialize_namespace (struct block *block,
					struct obstack *obstack);

/* Return the symbol for the function which contains a specified
   lexical block, described by a struct block BL.  */

struct symbol *
block_function (struct block *bl)
{
  while (BLOCK_FUNCTION (bl) == 0 && BLOCK_SUPERBLOCK (bl) != 0)
    bl = BLOCK_SUPERBLOCK (bl);

  return BLOCK_FUNCTION (bl);
}

/* Return Nonzero if block A is lexically nested within block B,
   or if A and B have the same pc range.
   Return zero otherwise. */

int
contained_in (struct block *a, struct block *b)
{
  if (!a || !b)
    return 0;
  return BLOCK_START (a) >= BLOCK_START (b) && BLOCK_END (a) <= BLOCK_END (b);
}

/* Now come some functions designed to deal with C++ namespace issues.
   The accessors are safe to use even in the non-C++ case.  */

/* This returns the using directives associated to BLOCK (but _not_
   its parents), if any.  */

struct using_direct_node *
block_using (const struct block *block)
{
  if (BLOCK_NAMESPACE (block) == NULL)
    return NULL;
  else
    return BLOCK_NAMESPACE (block)->using;
}

/* This returns the using directives associated to BLOCK and its
   parents, if any.  The resulting structure must be freed by calling
   cp_free_usings on it.  */

struct using_direct_node *
block_all_usings (const struct block *block)
{
  struct using_direct_node *using = NULL;

  while (block != NULL)
    {
      using = cp_copy_usings (block_using (block), using);
      block = BLOCK_SUPERBLOCK (block);
    }

  return using;
}


/* Set block_using (BLOCK) to USING; if needed, allocate memory via
   OBSTACK.  */

void
block_set_using (struct block *block, struct using_direct_node *using,
		 struct obstack *obstack)
{
  block_initialize_namespace (block, obstack);

  BLOCK_NAMESPACE (block)->using = using;
}

/* This returns the namespace that BLOCK is enclosed in, or "" if it
   isn't enclosed in a namespace at all.  This travels the chain of
   superblocks looking for a scope, if necessary.  */

const char *
block_scope (const struct block *block)
{
  for (; block != NULL; block = BLOCK_SUPERBLOCK (block))
    {
      if (BLOCK_NAMESPACE (block) != NULL
	  && BLOCK_NAMESPACE (block)->scope != NULL)
	return BLOCK_NAMESPACE (block)->scope;
    }

  return "";
}

/* Set block_scope (BLOCK) to SCOPE; if needed, allocate memory via
   OBSTACK.  (It won't make a copy of SCOPE, however, so that already
   has to be allocated correctly.)  */

void
block_set_scope (struct block *block, const char *scope,
		 struct obstack *obstack)
{
  block_initialize_namespace (block, obstack);

  BLOCK_NAMESPACE (block)->scope = scope;
}

/* If BLOCK_NAMESPACE (block) is NULL, allocate it via OBSTACK and
   ititialize its members to zero.  */

static void
block_initialize_namespace (struct block *block, struct obstack *obstack)
{
  if (BLOCK_NAMESPACE (block) == NULL)
    {
      BLOCK_NAMESPACE (block)
	= obstack_alloc (obstack, sizeof (struct namespace_info));
      BLOCK_NAMESPACE (block)->using = NULL;
    }
}

/* Return the static block associated to BLOCK.  Return NULL if block
   is NULL or if block is a global block.  */

const struct block *
block_static_block (const struct block *block)
{
  if (block == NULL || BLOCK_SUPERBLOCK (block) == NULL)
    return NULL;

  while (BLOCK_SUPERBLOCK (BLOCK_SUPERBLOCK (block)) != NULL)
    block = BLOCK_SUPERBLOCK (block);

  return block;
}
