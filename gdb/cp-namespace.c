/* Helper routines for C++ support in GDB.
   Copyright 2003 Free Software Foundation, Inc.

   Contributed by David Carlton.

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
#include "cp-support.h"
#include "gdb_obstack.h"
#include "symtab.h"
#include "symfile.h"
#include "gdb_assert.h"
#include "block.h"
#include "objfiles.h"
#include "gdbtypes.h"
#include "dictionary.h"
#include "gdbcmd.h"

/* When set, the file that we're processing seems to have debugging
   info for C++ namespaces, so cp-namespace.c shouldn't try to guess
   namespace info itself.  */

unsigned char processing_has_namespace_info;

/* If processing_has_namespace_info is nonzero, this string should
   contain the name of the current namespace.  The string is
   temporary; copy it if you need it.  */

const char *processing_current_prefix;

/* List of using directives that are active in the current file.  */

static struct using_direct *using_list;

static struct using_direct *cp_add_using (const char *name,
					  unsigned int inner_len,
					  unsigned int outer_len,
					  struct using_direct *next);

static struct using_direct *cp_copy_usings (struct using_direct *using,
					    struct obstack *obstack);

/* This block exists only to store symbols associated to namespaces.
   Normally, try to avoid accessing it directly: instead, use
   get_namespace_block if you can.  Similarly with
   possible_namespace_block and namespace_objfile.  */

static struct block *namespace_block = NULL;

static struct block *possible_namespace_block = NULL;

static struct objfile *namespace_objfile = NULL;

static void initialize_namespace_blocks (void);

static struct block *get_namespace_block (void);

static struct block *get_possible_namespace_block (void);

static struct objfile *get_namespace_objfile (void);

static void free_namespace_blocks (struct symtab *symtab);

static int check_one_possible_namespace_symbol (const char *name,
						int len);

static int check_possible_namespace_symbols_loop (const char *name,
						  int len);

static void maintenance_print_namespace (char *args, int from_tty);

/* Set up support for dealing with C++ namespace info in the current
   symtab.  */

void cp_initialize_namespace ()
{
  processing_has_namespace_info = 0;
  using_list = NULL;
}

/* Add all the using directives we've gathered to the current symtab.
   STATIC_BLOCK should be the symtab's static block; OBSTACK is used
   for allocation.  */

void
cp_finalize_namespace (struct block *static_block,
		       struct obstack *obstack)
{
  if (using_list != NULL)
    {
      block_set_using (static_block,
		       cp_copy_usings (using_list, obstack),
		       obstack);
      using_list = NULL;
    }
}

/* Check to see if SYMBOL refers to an object contained within an
   anonymous namespace; if so, add an appropriate using directive.  */

/* Optimize away strlen ("(anonymous namespace)").  */

#define ANONYMOUS_NAMESPACE_LEN 21

void
cp_scan_for_anonymous_namespaces (const struct symbol *symbol)
{
  if (!processing_has_namespace_info
      && SYMBOL_CPLUS_DEMANGLED_NAME (symbol) != NULL)
    {
      const char *name = SYMBOL_CPLUS_DEMANGLED_NAME (symbol);
      unsigned int previous_component;
      unsigned int next_component;
      const char *len;

      /* Start with a quick-and-dirty check for mention of "(anonymous
	 namespace)".  */

      if (!cp_is_anonymous (name))
	return;

      previous_component = 0;
      next_component = cp_find_first_component (name + previous_component);

      while (name[next_component] == ':')
	{
	  if ((next_component - previous_component) == ANONYMOUS_NAMESPACE_LEN
	      && strncmp (name + previous_component,
			  "(anonymous namespace)",
			  ANONYMOUS_NAMESPACE_LEN) == 0)
	    {
	      /* We've found a component of the name that's an
		 anonymous namespace.  So add symbols in it to the
		 namespace given by the previous component if there is
		 one, or to the global namespace if there isn't.  */
	      cp_add_using_directive (name,
				      previous_component == 0
				      ? 0 : previous_component - 2,
				      next_component);
	    }
	  /* The "+ 2" is for the "::".  */
	  previous_component = next_component + 2;
	  next_component = (previous_component
			    + cp_find_first_component (name
						       + previous_component));
	}
    }
}

/* Add a using directive to using_list.  NAME is the start of a string
   that should contain the namespaces we want to add as initial
   substrings, OUTER_LENGTH is the end of the outer namespace, and
   INNER_LENGTH is the end of the inner namespace.  If the using
   directive in question has already been added, don't add it
   twice.  */

void
cp_add_using_directive (const char *name, unsigned int outer_length,
			unsigned int inner_length)
{
  struct using_direct *current;
  struct using_direct *new;

  /* Has it already been added?  */

  for (current = using_list; current != NULL; current = current->next)
    {
      if ((strncmp (current->inner, name, inner_length) == 0)
	  && (strlen (current->inner) == inner_length)
	  && (strlen (current->outer) == outer_length))
	return;
    }

  using_list = cp_add_using (name, inner_length, outer_length,
			     using_list);
}

/* Record the namespace that the function defined by SYMBOL was
   defined in, if necessary.  BLOCK is the associated block; use
   OBSTACK for allocation.  */

void
cp_set_block_scope (const struct symbol *symbol,
		    struct block *block,
		    struct obstack *obstack)
{
  /* Make sure that the name was originally mangled: if not, there
     certainly isn't any namespace information to worry about!  */

  if (SYMBOL_CPLUS_DEMANGLED_NAME (symbol) != NULL)
    {
      if (processing_has_namespace_info)
	{
	  block_set_scope
	    (block, obsavestring (processing_current_prefix,
				  strlen (processing_current_prefix),
				  obstack),
	     obstack);
	}
      else
	{
	  /* Try to figure out the appropriate namespace from the
	     demangled name.  */

	  /* FIXME: carlton/2003-04-15: If the function in question is
	     a method of a class, the name will actually include the
	     name of the class as well.  This should be harmless, but
	     is a little unfortunate.  */

	  const char *name = SYMBOL_CPLUS_DEMANGLED_NAME (symbol);
	  unsigned int prefix_len = cp_entire_prefix_len (name);

	  block_set_scope (block,
			   obsavestring (name, prefix_len, obstack),
			   obstack);
	}
    }
}

/* Test whether or not NAMESPACE looks like it mentions an anonymous
   namespace; return nonzero if so.  */

int
cp_is_anonymous (const char *namespace)
{
  return (strstr (namespace, "(anonymous namespace)")
	  != NULL);
}

/* Create a new struct using direct whose inner namespace is the
   initial substring of NAME of leng INNER_LEN and whose outer
   namespace is the initial substring of NAME of length OUTER_LENGTH.
   Set its next member in the linked list to NEXT; allocate all memory
   using xmalloc.  It copies the strings, so NAME can be a temporary
   string.  */

static struct using_direct *
cp_add_using (const char *name,
	      unsigned int inner_len,
	      unsigned int outer_len,
	      struct using_direct *next)
{
  struct using_direct *retval;

  gdb_assert (outer_len < inner_len);

  retval = xmalloc (sizeof (struct using_direct));
  retval->inner = savestring (name, inner_len);
  retval->outer = savestring (name, outer_len);
  retval->next = next;

  return retval;
}

/* Make a copy of the using directives in the list pointed to by
   USING, using OBSTACK to allocate memory.  Free all memory pointed
   to by USING via xfree.  */

static struct using_direct *
cp_copy_usings (struct using_direct *using,
		struct obstack *obstack)
{
  if (using == NULL)
    {
      return NULL;
    }
  else
    {
      struct using_direct *retval
	= obstack_alloc (obstack, sizeof (struct using_direct));
      retval->inner = obsavestring (using->inner, strlen (using->inner),
				    obstack);
      retval->outer = obsavestring (using->outer, strlen (using->outer),
				    obstack);
      retval->next = cp_copy_usings (using->next, obstack);

      xfree (using->inner);
      xfree (using->outer);
      xfree (using);

      return retval;
    }
}

/* Allocate everything necessary for namespace_block and
   possible_namespace_block.  */

static void
initialize_namespace_blocks (void)
{
  struct objfile *objfile = get_namespace_objfile ();
  struct symtab *namespace_symtab;
  struct blockvector *bv;
  struct block *bl;

  namespace_symtab = allocate_symtab ("<C++-namespaces>", objfile);
  namespace_symtab->language = language_cplus;
  namespace_symtab->free_code = free_nothing;
  namespace_symtab->dirname = NULL;

  /* 2 = 3 blocks (global = namespace_block, static = NULL,
     possible_namespace_block) - 1 block that's always part of struct
     blockvector.  */
  bv = obstack_alloc (&objfile->symbol_obstack,
		      sizeof (struct blockvector)
		      + 2 * sizeof (struct block *));
  BLOCKVECTOR_NBLOCKS (bv) = 3;
  BLOCKVECTOR (namespace_symtab) = bv;
  
  /* Allocate dummy STATIC_BLOCK. */
  bl = allocate_block (&objfile->symbol_obstack);
  BLOCK_DICT (bl) = dict_create_linear (&objfile->symbol_obstack,
					NULL);
  BLOCKVECTOR_BLOCK (bv, STATIC_BLOCK) = bl;

  /* Allocate GLOBAL_BLOCK, which is namespace_block.  */
  bl = allocate_block (&objfile->symbol_obstack);
  BLOCK_DICT (bl) = dict_create_hashed_expandable ();
  BLOCKVECTOR_BLOCK (bv, GLOBAL_BLOCK) = bl;
  namespace_block = bl;

  /* Allocate possible_namespace_block; we put it where the first
     local block will live, though I don't think there's any need to
     pretend that it's actually a local block (e.g. by setting
     BLOCK_SUPERBLOCK appropriately).  */
  bl = allocate_block (&objfile->symbol_obstack);
  BLOCK_DICT (bl) = dict_create_hashed_expandable ();
  BLOCKVECTOR_BLOCK (bv, 2) = bl;
  possible_namespace_block = bl;

  namespace_symtab->free_func = free_namespace_blocks;
}

/* Locate namespace_block, allocating it if necessary.  */

static struct block *
get_namespace_block (void)
{
  if (namespace_block == NULL)
    initialize_namespace_blocks ();

  return namespace_block;
}

/* Locate possible_namespace_block, allocating it if necessary.  */

static struct block *
get_possible_namespace_block (void)
{
  if (namespace_block == NULL)
    initialize_namespace_blocks ();

  return possible_namespace_block;
}

/* Free the dictionary associated to the namespace block.  */

static void
free_namespace_blocks (struct symtab *symtab)
{
  gdb_assert (namespace_block != NULL);
  dict_free (BLOCK_DICT (namespace_block));
  namespace_block = NULL;
  dict_free (BLOCK_DICT (possible_namespace_block));
  possible_namespace_block = NULL;
  namespace_objfile = NULL;
}

/* Locate the namespace objfile, allocating it if necessary.  */

static struct objfile *
get_namespace_objfile (void)
{
  if (namespace_objfile == NULL)
    {
      namespace_objfile = allocate_objfile (NULL, 0);
      namespace_objfile->name = "<C++-namespaces>";
    }

  return namespace_objfile;
}

/* Check to see if there's already a namespace symbol corresponding to
   the initial substring of NAME whose length is LEN; if there isn't
   one, allocate one and add it to the namespace symtab.  Return the
   symbol in question.  */

struct symbol *
cp_check_namespace_symbol (const char *name, int len)
{
  struct objfile *objfile = get_namespace_objfile ();
  char *name_copy = obsavestring (name, len, &objfile->symbol_obstack);
  struct symbol *sym = cp_lookup_namespace_symbol (name_copy);

  if (sym == NULL)
    {
      struct type *type = init_type (TYPE_CODE_NAMESPACE, 0, 0,
				     name_copy, objfile);
      TYPE_TAG_NAME (type) = TYPE_NAME (type);

      sym = obstack_alloc (&objfile->symbol_obstack, sizeof (struct symbol));
      memset (sym, 0, sizeof (struct symbol));
      SYMBOL_LANGUAGE (sym) = language_cplus;
      DEPRECATED_SYMBOL_NAME (sym) = name_copy;
      SYMBOL_CLASS (sym) = LOC_TYPEDEF;
      SYMBOL_TYPE (sym) = type;
      SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;

      dict_add_symbol (BLOCK_DICT (get_namespace_block ()), sym);
    }
  else
    {
      obstack_free (&objfile->symbol_obstack, name_copy);
    }

  return sym;
}

/* Look for a symbol in namespace_block named NAME.  */

struct symbol *
cp_lookup_namespace_symbol (const char *name)
{
  return lookup_block_symbol (get_namespace_block (), name, NULL,
			      VAR_NAMESPACE);
}

/* The next few functions deal with "possible namespace symbols".
   These are symbols that claim to be associated to namespaces,
   whereas in fact we don't know if the object of that name is a
   namespace or a class.  So don't trust them until you've searched
   through all the global symbols to see if there's a class of that
   name or not.  */

/* FIXME: carlton/2002-12-18: This concept is a hack.  But it seems to
   be the easiest way to deal with our desire for namespace symbols,
   given the commonness of compilers that don't generate debugging
   info for them.  Once such compilers are more common, we should
   delete all the possible namespace stuff.  */

/* Check to see if there's already a possible namespace symbol whose
   name is the initial substring of NAME of length LEN.  If not,
   create one and return 0; otherwise, return 1.  */

static int
check_one_possible_namespace_symbol (const char *name, int len)
{
  struct objfile *objfile = get_namespace_objfile ();
  char *name_copy = obsavestring (name, len, &objfile->symbol_obstack);
  const struct block *block = get_possible_namespace_block ();
  struct symbol *sym = lookup_block_symbol (block, name_copy,
					    NULL, VAR_NAMESPACE);

  if (sym == NULL)
    {
      struct type *type = init_type (TYPE_CODE_NAMESPACE, 0, 0,
				     name_copy, objfile);
      TYPE_TAG_NAME (type) = TYPE_NAME (type);

      sym = obstack_alloc (&objfile->symbol_obstack, sizeof (struct symbol));
      memset (sym, 0, sizeof (struct symbol));
      SYMBOL_LANGUAGE (sym) = language_cplus;
      DEPRECATED_SYMBOL_NAME (sym) = name_copy;
      SYMBOL_CLASS (sym) = LOC_TYPEDEF;
      SYMBOL_TYPE (sym) = type;
      SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;

      dict_add_symbol (BLOCK_DICT (block), sym);
      return 0;
    }
  else
    {
      obstack_free (&objfile->symbol_obstack, name_copy);
      return 1;
    }
}

/* This is a helper loop for cp_check_possible_namespace_symbols; it
   ensures that there are namespace symbols for all namespaces that
   are initial substrings of NAME of length LEN.  It returns 1 if a
   previous loop had already created the shortest such symbol and 0
   otherwise.  */

static int
check_possible_namespace_symbols_loop (const char *name, int len)
{
  if (name[len] == ':')
    {
      int done;
      int next_len = len + 2;

      next_len += cp_find_first_component (name + next_len);
      done = check_possible_namespace_symbols_loop (name, next_len);

      if (!done)
	{
	  done = check_one_possible_namespace_symbol (name, len);
	}

      return done;
    }
  else
    return 0;
}

/* Ensure that there are symbols in possible_namespace_block for all
   initial substrings of NAME that look like namespaces or
   classes.  */

void
cp_check_possible_namespace_symbols (const char *name)
{
  check_possible_namespace_symbols_loop (name,
					 cp_find_first_component (name));
}

/* Look for a symbol in possible_namespace_block named NAME.  */

struct symbol *
cp_lookup_possible_namespace_symbol (const char *name)
{
  return lookup_block_symbol (get_possible_namespace_block (),
			      name, NULL, VAR_NAMESPACE);
}

static void
maintenance_cplus_namespace (char *args, int from_tty)
{
  const struct block *namespace_block = get_namespace_block ();
  const struct block *possible_namespace_block
    = get_possible_namespace_block ();
  struct dict_iterator iter;
  struct symbol *sym;

  printf_unfiltered ("Definite namespaces:\n");
  ALL_BLOCK_SYMBOLS (namespace_block, iter, sym)
    {
      printf_unfiltered ("%s\n", SYMBOL_PRINT_NAME (sym));
    }
  printf_unfiltered ("Possible namespaces:\n");
  ALL_BLOCK_SYMBOLS (possible_namespace_block, iter, sym)
    {
      printf_unfiltered ("%s\n", SYMBOL_PRINT_NAME (sym));
    }
}

void
_initialize_cp_namespace (void)
{
  add_cmd ("namespace", class_maintenance, maintenance_cplus_namespace,
	   "Print the list of current known C++ namespaces.",
	   &maint_cplus_cmd_list);
}
