/* GDB self-test for each gdbarch.
   Copyright (C) 2017-2022 Free Software Foundation, Inc.

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
#include <functional>

#if GDB_SELF_TEST
#include "gdbsupport/selftest.h"
#include "selftest-arch.h"
#include "arch-utils.h"

namespace selftests {

static bool skip_arch (const char *arch)
{
  if (strcmp ("fr300", arch) == 0)
    {
      /* PR 20946 */
      return true;
    }

  if (strcmp ("powerpc:EC603e", arch) == 0
      || strcmp ("powerpc:e500mc", arch) == 0
      || strcmp ("powerpc:e500mc64", arch) == 0
      || strcmp ("powerpc:titan", arch) == 0
      || strcmp ("powerpc:vle", arch) == 0
      || strcmp ("powerpc:e5500", arch) == 0
      || strcmp ("powerpc:e6500", arch) == 0)
    {
      /* PR 19797 */
      return true;
    }

  return false;
}

/* Register a kind of selftest that calls the test function once for each
   gdbarch known to GDB.  */

void
register_test_foreach_arch (const std::string &name,
			    self_test_foreach_arch_function *function)
{
  std::vector<const char *> arches = gdbarch_printable_names ();
  for (const char *arch : arches)
    {
      if (skip_arch (arch))
	continue;

      auto test_fn
	= ([=] ()
	   {
	     struct gdbarch_info info;
	     info.bfd_arch_info = bfd_scan_arch (arch);
	     struct gdbarch *gdbarch = gdbarch_find_by_info (info);
	     SELF_CHECK (gdbarch != NULL);
	     function (gdbarch);
	     reset ();
	   });

      std::string test_name
	= name + std::string ("::") + std::string (arch);
      register_test (test_name, test_fn);
    }
}

void
reset ()
{
  /* Clear GDB internal state.  */
  registers_changed ();
  reinit_frame_cache ();
}
} // namespace selftests
#endif /* GDB_SELF_TEST */
