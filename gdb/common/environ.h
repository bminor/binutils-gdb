/* Header for environment manipulation library.
   Copyright (C) 1989-2017 Free Software Foundation, Inc.

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

#if !defined (ENVIRON_H)
#define ENVIRON_H 1

#include <vector>

/* Class that represents the environment variables as seen by the
   inferior.  */

class gdb_environ
{
public:
  /* Regular constructor and destructor.  */
  gdb_environ ()
  {
    /* Make sure that the vector contains at least a NULL element.
       If/when we add more variables to it, NULL will always be the
       last element.  */
    m_environ_vector.push_back (NULL);
  }

  ~gdb_environ ()
  {
    clear ();
  }

  /* Move constructor.  */
  gdb_environ (gdb_environ &&e)
    : m_environ_vector (std::move (e.m_environ_vector))
  {
    /* Make sure that the moved-from vector is left at a valid
       state (only one NULL element).  */
    e.m_environ_vector.clear ();
    e.m_environ_vector.push_back (NULL);
  }

  /* Move assignment.  */
  gdb_environ &operator= (gdb_environ &&e);

  /* Create a gdb_environ object using the host's environment
     variables.  */
  static gdb_environ from_host_environ ();

  /* Clear the environment variables stored in the object.  */
  void clear ();

  /* Return the value in the environment for the variable VAR.  The
     returned pointer is only valid as long as the gdb_environ object
     is not modified.  */
  const char *get (const char *var) const;

  /* Store VAR=VALUE in the environment.  */
  void set (const char *var, const char *value);

  /* Unset VAR in environment.  */
  void unset (const char *var);

  /* Return the environment vector represented as a 'char **'.  */
  char **envp () const;

private:
  /* A vector containing the environment variables.  */
  std::vector<char *> m_environ_vector;
};

#endif /* defined (ENVIRON_H) */
