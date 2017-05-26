/* GDB self-testing.
   Copyright (C) 2016-2017 Free Software Foundation, Inc.

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
#ifdef GDBSERVER
#define QUIT do {} while (0)
#else
#include "defs.h" /* for QUIT */
#endif
#include "common-exceptions.h"
#include "common-debug.h"
#include "selftest.h"
#include <vector>

/* All the tests that have been registered.  */

static std::vector<self_test_function *> tests;

/* See selftest.h.  */

void
register_self_test (self_test_function *function)
{
  tests.push_back (function);
}

/* See selftest.h.  */

void
run_self_tests (void)
{
  int failed = 0;

  for (int i = 0; i < tests.size (); ++i)
    {
      QUIT;

      TRY
	{
	  tests[i] ();
	}
      CATCH (ex, RETURN_MASK_ERROR)
	{
	  ++failed;
#ifndef GDBSERVER
	  exception_fprintf (gdb_stderr, ex, _("Self test failed: "));
#endif
	}
      END_CATCH

#ifndef GDBSERVER
      /* Clear GDB internal state.  */
      registers_changed ();
      reinit_frame_cache ();
#endif
    }

  debug_printf ("Ran %lu unit tests, %d failed\n",
		(long) tests.size (), failed);
}
