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

#if 0

/* FIXME: carlton/2002-10-09: cp_locate_arguments and
   cp_find_last_component are broken: they don't deal with operator<<
   and the like.  Oops.  I decided to fix this by starting from the
   front, leading to cp_find_first_component (which may or may not be
   correct...); if I don't find other uses for cp_locate_arguments and
   cp_find_last_component soon, I'll delete them.  */

/* Given a name that may or may not be the demangled name of a
   function or method, this returns a pointer to the last character
   that isn't part of the arguments, if there are any.  I.e. given a
   pointer to "foo", it returns a pointer to the trailing '\0'; given
   a pointer to "C::foo() const", it returns a pointer to the '('.  */

const char *
cp_locate_arguments (const char *name)
{
/* NOTE: carlton/2002-10-08: we search from the front rather than from
   the back to avoid getting screwed up by the 'const' on the end.
   ARGSOK says when a parenthesis should be the start of the arguments
   to a function: otherwise, we get screwed up by 'A::(anonymous
   namespace)::foo' and its ilk.  */

  const char *current = name;
  const char *end = name + strlen(name);
  /* The nesting depth.  */
  int depth = 0;
  /* Might a parenthesis be the start of arguments?  */
  int argsok = 0;

  while (current < end)
    {
      switch (*current)
	{
	case '(':
	  if (depth == 0 && argsok)
	    return current;
	  ++depth;
	  argsok = 0;
	  break;
	case '<':
	  ++depth;
	  argsok = 0;
	  break;
	case ')':
	case '>':
	  --depth;
	  argsok = 1;
	  break;
	case ':':
	  argsok = 0;
	  break;
	default:
	  argsok = 1;
	  break;
	}
      ++current;
    }

  return current;
}

/* Given pointers FIRST and LAST such that [FIRST,LAST) is a substring
   of a demangled name, not including the arguments (i.e. call
   cp_locate_arguments first!), this returns a pointer to the
   beginning of the end of the last component.  For example, given
   either "A::B::C::foo" or "foo", it returns a pointer to 'f'.  */

/* FIXME: carlton/2002-10-08: This overlaps dreadfully with
   find_last_component.  Sigh.  */

const char *
cp_find_last_component (const char *first, const char *last)
{
  const char *current;
  int depth;

  for (current = last - 1, depth = 0;
       current >= first;
       --current)
    {
      switch (*current)
	{
	case '(':
	case '<':
	  --depth;
	  break;
	case ')':
	case '>':
	  ++depth;
	  break;
	case ':':
	  if (depth == 0)
	    return current + 1;
	  break;
	default:
	  break;
	}
    }

  return first;
}

#endif /* 0 */

/* This allocates a new using_data structure initialized to contain
   OUTER and INNER, and puts it at the beginning of the linked list
   given by NEXT.  It returns the resulting struct using_data_node.
   All memory is allocated using OBSTACK.  */
struct using_data_node *
cp_add_using (const char *outer, const char *inner,
	      struct using_data_node *next,
	      struct obstack *obstack)
{
  struct using_data *current
    = obstack_alloc (obstack, sizeof (struct using_data));
  struct using_data_node *retval
    = obstack_alloc (obstack, sizeof (struct using_data_node));

  current->outer = outer;
  current->inner = inner;
  retval->current = current;
  retval->next = next;

  return retval;
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
