/* Self tests for scoped_fd for GDB, the GNU debugger.

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

#include "common/scoped_fd.h"
#include "config.h"

#ifdef HAVE_UNISTD_H

#include "selftest.h"

namespace selftests {
namespace scoped_fd {

/* Test that the file descriptor is closed.  */
static void
test_destroy ()
{
  char filename[] = "scoped_fd-selftest-XXXXXX";
  int fd = mkstemp (filename);
  SELF_CHECK (fd >= 0);

  unlink (filename);
  errno = 0;
  {
    ::scoped_fd sfd (fd);

    SELF_CHECK (sfd.get () == fd);
  }

  SELF_CHECK (close (fd) == -1 && errno == EBADF);
}

/* Test that the file descriptor can be released.  */
static void
test_release ()
{
  char filename[] = "scoped_fd-selftest-XXXXXX";
  int fd = mkstemp (filename);
  SELF_CHECK (fd >= 0);

  unlink (filename);
  errno = 0;
  {
    ::scoped_fd sfd (fd);

    SELF_CHECK (sfd.release () == fd);
  }

  SELF_CHECK (close (fd) == 0 || errno != EBADF);
}

/* Run selftests.  */
static void
run_tests ()
{
  test_destroy ();
  test_release ();
}

} /* namespace scoped_fd */
} /* namespace selftests */

#endif /* HAVE_UNISTD_H */

void
_initialize_scoped_fd_selftests ()
{
#ifdef HAVE_UNISTD_H
  selftests::register_test ("scoped_fd",
			    selftests::scoped_fd::run_tests);
#endif
}
