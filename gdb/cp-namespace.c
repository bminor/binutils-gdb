/* Helper routines for C++ support in GDB.
   Copyright 2003 Free Software Foundation, Inc.

   Contributed by David Carlton and by Kealia, Inc.

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

/* When set, the file that we're processing seems to have debugging
   info for C++ namespaces, so cp-namespace.c shouldn't try to guess
   namespace info itself.  */

unsigned char processing_has_namespace_info;

/* If processing_has_namespace_info is nonzero, this string should
   contain the name of the current namespace.  The string is
   temporary; copy it if you need it.  */

/* FIXME: carlton/2003-06-12: This isn't entirely reliable: currently,
   we get mislead by DW_AT_specification.  */

const char *processing_current_namespace;

/* List of using directives that are active in the current file.  */

static struct using_direct *using_list;

static struct using_direct *cp_add_using (const char *name,
					  unsigned int inner_len,
					  unsigned int outer_len,
					  struct using_direct *next);

static struct using_direct *cp_copy_usings (struct using_direct *using,
					    struct obstack *obstack);

static struct symbol *lookup_namespace_scope (const char *name,
					      const char *linkage_name,
					      const struct block *block,
					      const domain_enum domain,
					      struct symtab **symtab,
					      const char *scope,
					      int scope_len);

static struct symbol *lookup_symbol_file (const char *name,
					  const char *linkage_name,
					  const struct block *block,
					  const domain_enum domain,
					  struct symtab **symtab,
					  int anonymous_namespace);

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
#if 0
      /* FIXME: carlton/2003-06-12: As mentioned above,
	 'processing_has_namespace_info' currently isn't entirely
	 reliable, so let's always use demangled names to get this
	 information for now.  */

      if (processing_has_namespace_info)
	{
	  block_set_scope
	    (block, obsavestring (processing_current_namespace,
				  strlen (processing_current_namespace),
				  obstack),
	     obstack);
	}
      else
#endif
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

/* The C++-specific version of name lookup for static and global
   names.  This makes sure that names get looked for in all namespaces
   that are in scope.  NAME is the natural name of the symbol that
   we're looking for, LINKAGE_NAME (which is optional) is its linkage
   name, BLOCK is the block that we're searching within, DOMAIN says
   what kind of symbols we're looking for, and if SYMTAB is non-NULL,
   we should store the symtab where we found the symbol in it.  */

struct symbol *
cp_lookup_symbol_nonlocal (const char *name,
			   const char *linkage_name,
			   const struct block *block,
			   const domain_enum domain,
			   struct symtab **symtab)
{
  return lookup_namespace_scope (name, linkage_name, block, domain,
				 symtab, block_scope (block), 0);
}

/* Lookup NAME at namespace scope (or, in C terms, in static and
   global variables).  SCOPE is the namespace that the current
   function is defined within; only consider namespaces whose length
   is at least SCOPE_LEN.  Other arguments are as in
   cp_lookup_symbol_nonlocal.

   For example, if we're within a function A::B::f and looking for a
   symbol f, this will get called with NAME = "f", SCOPE = "A::B", and
   SCOPE_LEN = 0.  It then calls itself with NAME and SCOPE the same,
   but with SCOPE_LEN = 1.  And then it calls itself with NAME and
   SCOPE the same, but with SCOPE_LEN = 4.  This third call looks for
   "A::B::x"; if it doesn't find it, then the second call looks for
   "A::x", and if that call fails, then the first call looks for
   "x".  */

static struct symbol *
lookup_namespace_scope (const char *name,
			const char *linkage_name,
			const struct block *block,
			const domain_enum domain,
			struct symtab **symtab,
			const char *scope,
			int scope_len)
{
  char *namespace;

  if (scope[scope_len] != '\0')
    {
      /* Recursively search for names in child namespaces first.  */

      struct symbol *sym;
      int new_scope_len = scope_len;

      /* If the current scope is followed by "::", skip past that.  */
      if (new_scope_len != 0)
	{
	  gdb_assert (scope[new_scope_len] == ':');
	  new_scope_len += 2;
	}
      new_scope_len += cp_find_first_component (scope + new_scope_len);
      sym = lookup_namespace_scope (name, linkage_name, block,
				    domain, symtab,
				    scope, new_scope_len);
      if (sym != NULL)
	return sym;
    }

  /* Okay, we didn't find a match in our children, so look for the
     name in the current namespace.  */

  namespace = alloca (scope_len + 1);
  strncpy (namespace, scope, scope_len);
  namespace[scope_len] = '\0';
  return cp_lookup_symbol_namespace (namespace, name, linkage_name,
				     block, domain, symtab);
}

/* Look up NAME in the C++ namespace NAMESPACE, applying the using
   directives that are active in BLOCK.  Other arguments are as in
   cp_lookup_symbol_nonlocal.  */

struct symbol *
cp_lookup_symbol_namespace (const char *namespace,
			    const char *name,
			    const char *linkage_name,
			    const struct block *block,
			    const domain_enum domain,
			    struct symtab **symtab)
{
  const struct using_direct *current;
  struct symbol *sym;

  /* First, go through the using directives.  If any of them add new
     names to the namespace we're searching in, see if we can find a
     match by applying them.  */

  for (current = block_using (block);
       current != NULL;
       current = current->next)
    {
      if (strcmp (namespace, current->outer) == 0)
	{
	  sym = cp_lookup_symbol_namespace (current->inner,
					    name,
					    linkage_name,
					    block,
					    domain,
					    symtab);
	  if (sym != NULL)
	    return sym;
	}
    }

  /* We didn't find anything by applying any of the using directives
     that are still applicable; so let's see if we've got a match
     using the current namespace.  */
  
  if (namespace[0] == '\0')
    {
      return lookup_symbol_file (name, linkage_name, block,
				 domain, symtab, 0);
    }
  else
    {
      char *concatenated_name
	= alloca (strlen (namespace) + 2 + strlen (name) + 1);
      strcpy (concatenated_name, namespace);
      strcat (concatenated_name, "::");
      strcat (concatenated_name, name);
      sym = lookup_symbol_file (concatenated_name, linkage_name,
				block, domain, symtab,
				cp_is_anonymous (namespace));
      return sym;
    }
}

/* Look up NAME in BLOCK's static block and in global blocks.  If
   ANONYMOUS_NAMESPACE is nonzero, the symbol in question is located
   within an anonymous namespace.  Other arguments are as in
   cp_lookup_symbol_nonlocal.  */

static struct symbol *
lookup_symbol_file (const char *name,
		    const char *linkage_name,
		    const struct block *block,
		    const domain_enum domain,
		    struct symtab **symtab,
		    int anonymous_namespace)
{
  struct symbol *sym = NULL;

  sym = lookup_symbol_static (name, linkage_name, block, domain, symtab);
  if (sym != NULL)
    return sym;

  if (anonymous_namespace)
    {
      /* Symbols defined in anonymous namespaces have external linkage
	 but should be treated as local to a single file nonetheless.
	 So we only search the current file's global block.  */

      const struct block *global_block = block_global_block (block);
      
      if (global_block != NULL)
	return lookup_symbol_aux_block (name, linkage_name, global_block,
					domain, symtab);
      else
	return NULL;
    }
  else
    {
      return lookup_symbol_global (name, linkage_name, domain, symtab);
    }
}
