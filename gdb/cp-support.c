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
