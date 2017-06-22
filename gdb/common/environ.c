/* environ.c -- library for manipulating environments for GNU.

   Copyright (C) 1986-2017 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include "common-defs.h"
#include "environ.h"
#include <algorithm>
#include <utility>

/* See common/environ.h.  */

gdb_environ &
gdb_environ::operator= (gdb_environ &&e)
{
  /* Are we self-moving?  */
  if (&e == this)
    return *this;

  m_environ_vector = std::move (e.m_environ_vector);
  e.m_environ_vector.clear ();
  e.m_environ_vector.push_back (NULL);
  return *this;
}

/* See common/environ.h.  */

gdb_environ gdb_environ::from_host_environ ()
{
  extern char **environ;
  gdb_environ e;

  if (environ == NULL)
    return e;

  for (int i = 0; environ[i] != NULL; ++i)
    {
      /* Make sure we add the element before the last (NULL).  */
      e.m_environ_vector.insert (e.m_environ_vector.end () - 1,
				 xstrdup (environ[i]));
    }

  return e;
}

/* See common/environ.h.  */

void
gdb_environ::clear ()
{
  for (char *v : m_environ_vector)
    xfree (v);
  m_environ_vector.clear ();
  /* Always add the NULL element.  */
  m_environ_vector.push_back (NULL);
}

/* Helper function to check if STRING contains an environment variable
   assignment of VAR, i.e., if STRING starts with 'VAR='.  Return true
   if it contains, false otherwise.  */

static bool
match_var_in_string (char *string, const char *var, size_t var_len)
{
  if (strncmp (string, var, var_len) == 0 && string[var_len] == '=')
    return true;

  return false;
}

/* See common/environ.h.  */

const char *
gdb_environ::get (const char *var) const
{
  size_t len = strlen (var);

  for (char *el : m_environ_vector)
    if (el != NULL && match_var_in_string (el, var, len))
      return &el[len + 1];

  return NULL;
}

/* See common/environ.h.  */

void
gdb_environ::set (const char *var, const char *value)
{
  /* We have to unset the variable in the vector if it exists.  */
  unset (var);

  /* Insert the element before the last one, which is always NULL.  */
  m_environ_vector.insert (m_environ_vector.end () - 1,
			   concat (var, "=", value, NULL));
}

/* See common/environ.h.  */

void
gdb_environ::unset (const char *var)
{
  size_t len = strlen (var);

  /* We iterate until '.end () - 1' because the last element is
     always NULL.  */
  for (std::vector<char *>::iterator el = m_environ_vector.begin ();
       el != m_environ_vector.end () - 1;
       ++el)
    if (match_var_in_string (*el, var, len))
      {
	xfree (*el);
	m_environ_vector.erase (el);
	break;
      }
}

/* See common/environ.h.  */

char **
gdb_environ::envp () const
{
  return const_cast<char **> (&m_environ_vector[0]);
}
