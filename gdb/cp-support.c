/* Helper routines for C++ support in GDB.
   Copyright 2002 Free Software Foundation, Inc.

   Contributed by MontaVista Software.

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
#include "gdb_string.h"
#include "demangle.h"
#include "gdb_obstack.h"
#include "gdb_assert.h"
#include "symtab.h"
#include "symfile.h"
#include "block.h"
#include "objfiles.h"
#include "gdbtypes.h"
#include "dictionary.h"
#include "gdbcmd.h"

static const char *find_last_component (const char *name);

/* This block exists only to store symbols associated to namespaces.
   Normally, try to avoid accessing it directly: instead, use
   get_namespace_block if you can.  Similarly with
   namespace_objfile.  */

static struct block *namespace_block = NULL;

static struct objfile *namespace_objfile = NULL;

static struct block *get_namespace_block (void);

static struct objfile *get_namespace_objfile (void);

static void free_namespace_block (struct symtab *symtab);

static void maintenance_print_namespace (char *args, int from_tty);

/* Here are some random pieces of trivia to keep in mind while trying
   to take apart demangled names:

   - Names can contain function arguments or templates, so the process
     has to be, to some extent recursive: maybe keep track of your
     depth based on encountering <> and ().

   - Parentheses don't just have to happen at the end of a name: they
     can occur even if the name in question isn't a function, because
     a template argument might be a type that's a function.

   - Conversely, even if you're trying to deal with a function, its
     demangled name might not end with ')': it could be a const (or
     volatile, I suppose) class method, in which case it ends with
     "const".

   - Parentheses are also used in anonymous namespaces: a variable
     'foo' in an anonymous namespace gets demangled as "(anonymous
     namespace)::foo".

   - And operator names can contain parentheses or angle brackets.
     Fortunately, I _think_ that operator names can only occur in a
     fairly restrictive set of locations (in particular, they have be
     at depth 0, don't they?).  */

/* NOTE: carlton/2002-10-25: Daniel Jacobowitz came up with an example
   where operator names don't occur at depth 0.  Sigh.  (It involved a
   template argument that was a pointer: I hadn't realized that was
   possible.)  Handling such edge cases does not seem like a
   high-priority problem to me.  */

/* FIXME: carlton/2002-10-09: Do all the functions here handle all the
   above considerations correctly?  */

/* Find the last component of the demangled C++ name NAME.  NAME
   must be a method name including arguments, in order to correctly
   locate the last component.

   This function return a pointer to the first colon before the
   last component, or NULL if the name had only one component.  */

static const char *
find_last_component (const char *name)
{
  const char *p;
  int depth;

  /* Functions can have local classes, so we need to find the
     beginning of the last argument list, not the end of the first
     one.  */
  p = name + strlen (name) - 1;
  while (p > name && *p != ')')
    p--;

  if (p == name)
    return NULL;

  /* P now points at the `)' at the end of the argument list.  Walk
     back to the beginning.  */
  p--;
  depth = 1;
  while (p > name && depth > 0)
    {
      if (*p == '<' || *p == '(')
	depth--;
      else if (*p == '>' || *p == ')')
	depth++;
      p--;
    }

  if (p == name)
    return NULL;

  while (p > name && *p != ':')
    p--;

  if (p == name || p == name + 1 || p[-1] != ':')
    return NULL;

  return p - 1;
}

/* Return the name of the class containing method PHYSNAME.  */

char *
class_name_from_physname (const char *physname)
{
  char *ret = NULL;
  const char *end;
  int depth = 0;
  char *demangled_name = cplus_demangle (physname, DMGL_ANSI);

  if (demangled_name == NULL)
    return NULL;

  end = find_last_component (demangled_name);
  if (end != NULL)
    {
      ret = xmalloc (end - demangled_name + 1);
      memcpy (ret, demangled_name, end - demangled_name);
      ret[end - demangled_name] = '\0';
    }

  xfree (demangled_name);
  return ret;
}

/* Return the name of the method whose linkage name is PHYSNAME.  */

char *
method_name_from_physname (const char *physname)
{
  char *ret = NULL;
  const char *end;
  int depth = 0;
  char *demangled_name = cplus_demangle (physname, DMGL_ANSI);

  if (demangled_name == NULL)
    return NULL;

  end = find_last_component (demangled_name);
  if (end != NULL)
    {
      char *args;
      int len;

      /* Skip "::".  */
      end = end + 2;

      /* Find the argument list, if any.  */
      args = strchr (end, '(');
      if (args == NULL)
	len = strlen (end + 2);
      else
	{
	  args --;
	  while (*args == ' ')
	    args --;
	  len = args - end + 1;
	}
      ret = xmalloc (len + 1);
      memcpy (ret, end, len);
      ret[len] = 0;
    }

  xfree (demangled_name);
  return ret;
}

/* This allocates a new using_direct structure initialized to contain
   NAME, OUTER_LENGTH, and INNER_LENGTH, and puts it at the beginning
   of the linked list given by NEXT.  It returns the resulting struct
   using_direct_node.  All memory is allocated using OBSTACK.  */

struct using_direct_node *
cp_add_using_obstack (const char *name,
		      unsigned short outer_length,
		      unsigned short inner_length,
		      struct using_direct_node *next,
		      struct obstack *obstack)
{
  struct using_direct *current
    = obstack_alloc (obstack, sizeof (struct using_direct));
  struct using_direct_node *retval
    = obstack_alloc (obstack, sizeof (struct using_direct_node));

  gdb_assert (outer_length < inner_length);

  current->name = name;
  current->outer_length = outer_length;
  current->inner_length = inner_length;
  retval->current = current;
  retval->next = next;

  return retval;
}

/* Same as cp_add_using, except that it uses xmalloc instead of
   obstacks.  */

struct using_direct_node *
cp_add_using_xmalloc (const char *name,
		      unsigned short outer_length,
		      unsigned short inner_length,
		      struct using_direct_node *next)
{
  struct using_direct *current = xmalloc (sizeof (struct using_direct));
  struct using_direct_node *retval
    = xmalloc (sizeof (struct using_direct_node));

  gdb_assert (outer_length < inner_length);

  current->name = name;
  current->outer_length = outer_length;
  current->inner_length = inner_length;
  retval->current = current;
  retval->next = next;

  return retval;
}

/* This copies the using_direct_nodes in TOCOPY, using xmalloc, and
   sticks them onto a list ending in TAIL.  (It doesn't copy the
   using_directs, just the using_direct_nodes.)  */

struct using_direct_node *
cp_copy_usings (struct using_direct_node *tocopy,
		struct using_direct_node *tail)
{
  struct using_direct_node *new_node;
  
  if (tocopy == NULL)
    return tail;

  new_node = xmalloc (sizeof (struct using_direct_node));
  new_node->current = tocopy->current;
  new_node->next = cp_copy_usings (tocopy->next, tail);

  return new_node;
}

/* This xfree's all the using_direct_nodes in USING (but not their
   using_directs!)  */
void
cp_free_usings (struct using_direct_node *using)
{
  struct using_direct_node *next;

  if (using != NULL)
    {
      for (next = using->next; next;
	   using = next, next = next->next)
	xfree (using);
      
      xfree (using);
    }
}


/* This returns the first component of NAME, which should be the
   demangled name of a C++ variable/function/method/etc.
   Specifically, it returns a pointer to the first colon forming the
   boundary of the first component: so, given 'A::foo' or 'A::B::foo'
   it returns a pointer to the first :, and given 'foo', it returns a
   pointer to the trailing '\0'.  */

/* Well, that's what it should do when called externally, but to make
   the recursion easier, it also stops if it reaches an unexpected ')'
   or '>'.  */

/* Let's optimize away calls to strlen("operator").  */

#define LENGTH_OF_OPERATOR 8

const char *
cp_find_first_component (const char *name)
{
  /* Names like 'operator<<' screw up the recursion, so let's
     special-case them.  I _hope_ they can only occur at the start of
     a component.  */

  if (strncmp (name, "operator", LENGTH_OF_OPERATOR) == 0)
    {
      name += LENGTH_OF_OPERATOR;
      switch (*name)
	{
	case '<':
	  if (name[1] == '<')
	    name += 2;
	  else
	    name += 1;
	  break;
	case '>':
	case '-':
	  if (name[1] == '>')
	    name +=2;
	  else
	    name += 1;
	  break;
	case '(':
	  name += 2;
	  break;
	default:
	  name += 1;
	  break;
	}
    }

  for (;; ++name)
    {
      switch (*name)
	{
	case '<':
	  /* Template; eat it up.  The calls to cp_first_component
	     should only return (I hope!) when they reach the '>'
	     terminating the component or a '::' between two
	     components.  (Hence the '+ 2'.)  */
	  for (name = cp_find_first_component (name + 1);
	       *name != '>';
	       name = cp_find_first_component (name + 2))
	    gdb_assert (*name == ':');
	  break;
	case '(':
	  /* Similar comment as to '<'.  */
	  for (name = cp_find_first_component (name + 1);
	       *name != ')';
	       name = cp_find_first_component (name + 2))
	    gdb_assert (*name == ':');
	  break;
	case '>':
	case ')':
	case '\0':
	case ':':
	  return name;
	default:
	  break;
	}
    }
}

/* Locate the namespace_block, allocating it if necessary.  */

static struct block *
get_namespace_block (void)
{
  if (namespace_block == NULL)
    {
      struct objfile *objfile = get_namespace_objfile ();
      struct symtab *namespace_symtab;
      struct blockvector *bv;
      struct block *bl;

      namespace_symtab = allocate_symtab ("<C++-namespaces>", objfile);
      namespace_symtab->language = language_cplus;
      namespace_symtab->free_code = free_nothing;
      namespace_symtab->dirname = NULL;

      bv = obstack_alloc (&objfile->symbol_obstack,
			  sizeof (struct blockvector));
      BLOCKVECTOR_NBLOCKS (bv) = 1;
      BLOCKVECTOR (namespace_symtab) = bv;
      
      /* Allocate dummy STATIC_BLOCK. */
      bl = obstack_alloc (&objfile->symbol_obstack, sizeof (struct block));
      BLOCK_START (bl) = 0;
      BLOCK_END (bl) = 0;
      BLOCK_FUNCTION (bl) = NULL;
      BLOCK_SUPERBLOCK (bl) = NULL;
      BLOCK_DICT (bl) = dict_create_linear (&objfile->symbol_obstack,
					    NULL);
      BLOCK_NAMESPACE (bl) = NULL;
      BLOCK_GCC_COMPILED (bl) = 0;
      BLOCKVECTOR_BLOCK (bv, STATIC_BLOCK) = bl;

      /* Allocate GLOBAL_BLOCK.  */
      bl = (struct block *)
	obstack_alloc (&objfile->symbol_obstack, sizeof (struct block));
      *bl = *BLOCKVECTOR_BLOCK (bv, STATIC_BLOCK);
      BLOCK_DICT (bl) = dict_create_linear_expandable ();
      BLOCKVECTOR_BLOCK (bv, GLOBAL_BLOCK) = bl;
      namespace_symtab->free_func = free_namespace_block;

      namespace_block = bl;
    }

  return namespace_block;
}

/* Free the dictionary associated to the namespace block.  */

static void
free_namespace_block (struct symtab *symtab)
{
  gdb_assert (namespace_block != NULL);
  dict_free (BLOCK_DICT (namespace_block));
  namespace_block = NULL;
  namespace_objfile = NULL;
}

/* Locate the namespace objfile, allocating it if necessary.  */

static struct objfile *
get_namespace_objfile (void)
{
  if (namespace_objfile == NULL)
    {
      namespace_objfile = allocate_objfile (NULL, 0);
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
  const struct block *block = get_namespace_block ();
  struct symbol *sym = lookup_block_symbol (block, name_copy,
					    NULL, VAR_NAMESPACE);

  if (sym == NULL)
    {
      struct type *type = alloc_type (objfile);
      INIT_CPLUS_SPECIFIC (type);
      TYPE_TAG_NAME (type) = obsavestring (name, len, &objfile->type_obstack);
      TYPE_CODE (type) = TYPE_CODE_NAMESPACE;
      TYPE_LENGTH (type) = 0;
      
      sym = obstack_alloc (&objfile->symbol_obstack, sizeof (struct symbol));
      memset (sym, 0, sizeof (struct symbol));
      SYMBOL_LANGUAGE (sym) = language_cplus;
      SYMBOL_NAME (sym) = name_copy;
      SYMBOL_CLASS (sym) = LOC_TYPEDEF;
      SYMBOL_TYPE (sym) = type;
      SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;

      dict_add_symbol (BLOCK_DICT (block), sym);
    }
  else
    {
      obstack_free (&objfile->symbol_obstack, name_copy);
    }

  return sym;
}

static void
maintenance_print_namespace (char *args, int from_tty)
{
  const struct block *block = get_namespace_block ();
  struct dict_iterator iter;
  struct symbol *sym;

  ALL_BLOCK_SYMBOLS (block, iter, sym)
    {
      printf_unfiltered ("%s\n", SYMBOL_BEST_NAME (sym));
    }
}

void
_initialize_cp_support (void)
{
  add_cmd ("namespace", class_maintenance, maintenance_print_namespace,
	   "Print the list of current known C++ namespaces.",
	   &maintenanceprintlist);
}
