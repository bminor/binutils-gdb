/* Self tests for scoped_mmap for GDB, the GNU debugger.

   Copyright (C) 2018 Free Software Foundation, Inc.

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

#include "common/scoped_mmap.h"
#include "config.h"

#if defined(HAVE_SYS_MMAN_H) && defined(HAVE_UNISTD_H)

#include "selftest.h"

#include <unistd.h>

namespace selftests {
namespace scoped_mmap {

/* Test that the file is unmapped.  */
static void
test_destroy ()
{
  void *mem;

  errno = 0;
  {
    ::scoped_mmap smmap (nullptr, sysconf (_SC_PAGESIZE), PROT_WRITE,
			 MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);

    mem = smmap.get ();
    SELF_CHECK (mem != nullptr);
  }

  SELF_CHECK (msync (mem, sysconf (_SC_PAGESIZE), 0) == -1 && errno == ENOMEM);
}

/* Test that the memory can be released.  */
static void
test_release ()
{
  void *mem;

  errno = 0;
  {
    ::scoped_mmap smmap (nullptr, sysconf (_SC_PAGESIZE), PROT_WRITE,
			 MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);

    mem = smmap.release ();
    SELF_CHECK (mem != nullptr);
  }

  SELF_CHECK (msync (mem, sysconf (_SC_PAGESIZE), 0) == 0 || errno != ENOMEM);

  munmap (mem, sysconf (_SC_PAGESIZE));
}

/* Run selftests.  */
static void
run_tests ()
{
  test_destroy ();
  test_release ();
}

} /* namespace scoped_mmap */
} /* namespace selftests */

#endif /* !defined(HAVE_SYS_MMAN_H) || !defined(HAVE_UNISTD_H) */

void
_initialize_scoped_mmap_selftests ()
{
#if defined(HAVE_SYS_MMAN_H) && defined(HAVE_UNISTD_H)
  selftests::register_test ("scoped_mmap",
			    selftests::scoped_mmap::run_tests);
#endif
}
