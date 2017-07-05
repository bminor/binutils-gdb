/* Self tests for gdb_environ for GDB, the GNU debugger.

   Copyright (C) 2017 Free Software Foundation, Inc.

   This file is part of GDB.

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

#include "defs.h"
#include "selftest.h"
#include "common/environ.h"
#include "common/diagnostics.h"

namespace selftests {
namespace gdb_environ_tests {

static void
run_tests ()
{
  /* Set a test environment variable.  This will be unset at the end
     of this function.  */
  if (setenv ("GDB_SELFTEST_ENVIRON", "1", 1) != 0)
    error (_("Could not set environment variable for testing."));

  gdb_environ env;

  /* When the vector is initialized, there should always be one NULL
     element in it.  */
  SELF_CHECK (env.envp ()[0] == NULL);

  /* Make sure that there is no other element.  */
  SELF_CHECK (env.get ("PWD") == NULL);

  /* Check if unset followed by a set in an empty vector works.  */
  env.set ("PWD", "test");
  SELF_CHECK (strcmp (env.get ("PWD"), "test") == 0);
  /* The second element must be NULL.  */
  SELF_CHECK (env.envp ()[1] == NULL);
  env.unset ("PWD");
  SELF_CHECK (env.envp ()[0] == NULL);

  /* Initialize the environment vector using the host's environ.  */
  env = gdb_environ::from_host_environ ();

  /* Our test environment variable should be present at the
     vector.  */
  SELF_CHECK (strcmp (env.get ("GDB_SELFTEST_ENVIRON"), "1") == 0);

  /* Set our test variable to another value.  */
  env.set ("GDB_SELFTEST_ENVIRON", "test");
  SELF_CHECK (strcmp (env.get ("GDB_SELFTEST_ENVIRON"), "test") == 0);

  /* And unset our test variable.  The variable still exists in the
     host's environment, but doesn't exist in our vector.  */
  env.unset ("GDB_SELFTEST_ENVIRON");
  SELF_CHECK (env.get ("GDB_SELFTEST_ENVIRON") == NULL);

  /* Re-set the test variable.  */
  env.set ("GDB_SELFTEST_ENVIRON", "1");
  SELF_CHECK (strcmp (env.get ("GDB_SELFTEST_ENVIRON"), "1") == 0);

  /* When we clear our environ vector, there should be only one
     element on it (NULL), and we shouldn't be able to get our test
     variable.  */
  env.clear ();
  SELF_CHECK (env.envp ()[0] == NULL);
  SELF_CHECK (env.get ("GDB_SELFTEST_ENVIRON") == NULL);

  /* Reinitialize our environ vector using the host environ.  We
     should be able to see one (and only one) instance of the test
     variable.  */
  env = gdb_environ::from_host_environ ();
  char **envp = env.envp ();
  int num_found = 0;

  for (size_t i = 0; envp[i] != NULL; ++i)
    if (strcmp (envp[i], "GDB_SELFTEST_ENVIRON=1") == 0)
      ++num_found;
  SELF_CHECK (num_found == 1);

  /* Get rid of our test variable.  */
  unsetenv ("GDB_SELFTEST_ENVIRON");

  /* Test the case when we set a variable A, then set a variable B,
     then unset A, and make sure that we cannot find A in the environ
     vector, but can still find B.  */
  env.set ("GDB_SELFTEST_ENVIRON_1", "aaa");
  SELF_CHECK (strcmp (env.get ("GDB_SELFTEST_ENVIRON_1"), "aaa") == 0);

  env.set ("GDB_SELFTEST_ENVIRON_2", "bbb");
  SELF_CHECK (strcmp (env.get ("GDB_SELFTEST_ENVIRON_2"), "bbb") == 0);

  env.unset ("GDB_SELFTEST_ENVIRON_1");
  SELF_CHECK (env.get ("GDB_SELFTEST_ENVIRON_1") == NULL);
  SELF_CHECK (strcmp (env.get ("GDB_SELFTEST_ENVIRON_2"), "bbb") == 0);

  env.clear ();

  /* Test that after a std::move the moved-from object is left at a
     valid state (i.e., its only element is NULL).  */
  env.set ("A", "1");
  SELF_CHECK (strcmp (env.get ("A"), "1") == 0);
  gdb_environ env2;
  env2 = std::move (env);
  SELF_CHECK (env.envp ()[0] == NULL);
  SELF_CHECK (strcmp (env2.get ("A"), "1") == 0);
  SELF_CHECK (env2.envp ()[1] == NULL);
  env.set ("B", "2");
  SELF_CHECK (strcmp (env.get ("B"), "2") == 0);
  SELF_CHECK (env.envp ()[1] == NULL);

  /* Test that the move constructor leaves everything at a valid
     state.  */
  env.clear ();
  env.set ("A", "1");
  SELF_CHECK (strcmp (env.get ("A"), "1") == 0);
  gdb_environ env3 = std::move (env);
  SELF_CHECK (env.envp ()[0] == NULL);
  SELF_CHECK (strcmp (env3.get ("A"), "1") == 0);
  SELF_CHECK (env3.envp ()[1] == NULL);
  env.set ("B", "2");
  SELF_CHECK (strcmp (env.get ("B"), "2") == 0);
  SELF_CHECK (env.envp ()[1] == NULL);

  /* Test self-move.  */
  env.clear ();
  env.set ("A", "1");
  SELF_CHECK (strcmp (env.get ("A"), "1") == 0);

  /* Some compilers warn about moving to self, but that's precisely what we want
     to test here, so turn this warning off.  */
  DIAGNOSTIC_PUSH
  DIAGNOSTIC_IGNORE_SELF_MOVE
  env = std::move (env);
  DIAGNOSTIC_POP

  SELF_CHECK (strcmp (env.get ("A"), "1") == 0);
  SELF_CHECK (strcmp (env.envp ()[0], "A=1") == 0);
  SELF_CHECK (env.envp ()[1] == NULL);
}
} /* namespace gdb_environ */
} /* namespace selftests */

void
_initialize_environ_selftests ()
{
  register_self_test (selftests::gdb_environ_tests::run_tests);
}
