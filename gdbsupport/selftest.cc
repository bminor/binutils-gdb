/* GDB self-testing.
   Copyright (C) 2016-2022 Free Software Foundation, Inc.

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

#include "common-defs.h"
#include "common-exceptions.h"
#include "common-debug.h"
#include "selftest.h"
#include <map>
#include <functional>

namespace selftests
{
/* All the tests that have been registered.  Using an std::map allows keeping
   the order of tests stable and easily looking up whether a test name
   exists.  */

static std::map<std::string, std::function<void(void)>> tests;

/* See selftest.h.  */

void
register_test (const std::string &name,
	       std::function<void(void)> function)
{
  /* Check that no test with this name already exist.  */
  gdb_assert (tests.find (name) == tests.end ());

  tests[name] = function;
}

/* See selftest.h.  */

static bool run_verbose_ = false;

/* See selftest.h.  */

bool
run_verbose ()
{
  return run_verbose_;
}

/* See selftest.h.  */

void
run_tests (gdb::array_view<const char *const> filters, bool verbose)
{
  int ran = 0, failed = 0;
  run_verbose_ = verbose;

  for (const auto &pair : tests)
    {
      const std::string &name = pair.first;
      const auto &test = pair.second;
      bool run = false;

      if (filters.empty ())
	run = true;
      else
	{
	  for (const char *filter : filters)
	    {
	      if (name.find (filter) != std::string::npos)
		run = true;
	    }
	}

      if (!run)
	continue;

      try
	{
	  debug_printf (_("Running selftest %s.\n"), name.c_str ());
	  ++ran;
	  test ();
	}
      catch (const gdb_exception_error &ex)
	{
	  ++failed;
	  debug_printf ("Self test failed: %s\n", ex.what ());
	}

      reset ();
    }

  debug_printf (_("Ran %d unit tests, %d failed\n"),
		ran, failed);
}

/* See selftest.h.  */

void for_each_selftest (for_each_selftest_ftype func)
{
  for (const auto &pair : tests)
    func (pair.first);
}

} // namespace selftests
