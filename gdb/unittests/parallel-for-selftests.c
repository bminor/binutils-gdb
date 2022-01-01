/* Self tests for parallel_for_each

   Copyright (C) 2021-2022 Free Software Foundation, Inc.

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
#include "gdbsupport/selftest.h"
#include "gdbsupport/parallel-for.h"

#if CXX_STD_THREAD

#include "gdbsupport/thread-pool.h"

namespace selftests {
namespace parallel_for {

struct save_restore_n_threads
{
  save_restore_n_threads ()
    : n_threads (gdb::thread_pool::g_thread_pool->thread_count ())
  {
  }

  ~save_restore_n_threads ()
  {
    gdb::thread_pool::g_thread_pool->set_thread_count (n_threads);
  }

  int n_threads;
};

static void
test (int n_threads)
{
  save_restore_n_threads saver;
  gdb::thread_pool::g_thread_pool->set_thread_count (n_threads);

#define NUMBER 10000

  std::atomic<int> counter (0);
  gdb::parallel_for_each (0, NUMBER,
			  [&] (int start, int end)
			  {
			    counter += end - start;
			  });

  SELF_CHECK (counter == NUMBER);

#undef NUMBER
}

static void
test_n_threads ()
{
  test (0);
  test (1);
  test (3);
}

}
}

#endif /* CXX_STD_THREAD */

void _initialize_parallel_for_selftests ();
void
_initialize_parallel_for_selftests ()
{
#ifdef CXX_STD_THREAD
  selftests::register_test ("parallel_for",
			    selftests::parallel_for::test_n_threads);
#endif /* CXX_STD_THREAD */
}
