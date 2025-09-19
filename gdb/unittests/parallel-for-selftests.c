/* Self tests for parallel_for_each

   Copyright (C) 2021-2025 Free Software Foundation, Inc.

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

using foreach_callback_t = gdb::function_view<void (iterator_range<int *> range)>;
using do_foreach_t = gdb::function_view<void (int *first, int *last,
					     foreach_callback_t)>;

/* Run one parallel-for-each test on the range [1, UPPER_BOUND) using the
   parallel-for-each implementation DO_FOREACH.  */

static void
test_one (do_foreach_t do_foreach, int upper_bound)
{
  std::vector<int> input;

  for (int i = 0; i < upper_bound; ++i)
    input.emplace_back (i);

  std::vector<int> output;
  std::mutex mtx;

  /* The (unfortunate) reason why we don't use std::vector<int>::iterator as
     the parallel-for-each iterator type is that std::atomic won't work with
     that type when building with -D_GLIBCXX_DEBUG.  */
  do_foreach (input.data (),  input.data () + input.size (),
	      [&] (iterator_range<int *> range)
		{
		  /* We shouldn't receive empty ranges.  */
		  SELF_CHECK (!range.empty ());

		  std::lock_guard lock (mtx);

		  for (int i : range)
		    output.emplace_back (i * 2);
		});

  /* Verify that each item was processed exactly once.  */
  SELF_CHECK (output.size () == upper_bound);
  std::sort (output.begin (), output.end ());

  for (int i = 0; i < output.size (); ++i)
    SELF_CHECK (output[i] == i * 2);
}

/* Run all tests on the parallel-for-each implementation DO_FOREACH.  */

static void
test_one_function (int n_threads, do_foreach_t do_foreach)
{
  save_restore_n_threads saver;
  gdb::thread_pool::g_thread_pool->set_thread_count (n_threads);

  /* Test with a few arbitrary number of items.  */
  test_one (do_foreach, 0);
  test_one (do_foreach, 1);
  test_one (do_foreach, 1000);
}

static void
test_parallel_for_each ()
{
  struct test_worker
  {
    /* DUMMY is there to test passing multiple arguments to the worker
       constructor.  */
    test_worker (foreach_callback_t callback, int dummy)
      : m_callback (callback)
    {
    }

    void operator() (iterator_range<int *> range)
    {
      return m_callback (range);
    }

  private:
    foreach_callback_t m_callback;
  };

  const std::vector<do_foreach_t> for_each_functions
    {
      /* Test gdb::parallel_for_each.  */
      [] (int *start, int *end, foreach_callback_t callback)
      { gdb::parallel_for_each<1, int *, test_worker> (start, end, callback, 0); },

      /* Test gdb::parallel_for_each_async.  */
      [] (int *start, int *end, foreach_callback_t callback)
      {
	bool done_flag = false;
	std::condition_variable cv;
	std::mutex mtx;

	gdb::parallel_for_each_async<1, int *, test_worker> (start, end,
	  [&mtx, &done_flag, &cv] ()
	    {
	      std::lock_guard<std::mutex> lock (mtx);
	      done_flag = true;
	      cv.notify_one();
	    },  callback, 0);

	/* Wait for the async parallel-for to complete.  */
	std::unique_lock<std::mutex> lock (mtx);
	cv.wait (lock, [&done_flag] () { return done_flag; });
      },

      /* Test gdb::sequential_for_each.  */
      [] (int *start, int *end, foreach_callback_t callback)
      { gdb::sequential_for_each<int *, test_worker> (start, end, callback, 0); },
    };

  int default_thread_count = gdb::thread_pool::g_thread_pool->thread_count ();

  for (int n_threads : { 0, 1, 3, default_thread_count })
    for (const auto &for_each_function : for_each_functions)
      test_one_function (n_threads, for_each_function);
}

} /* namespace parallel_for */
} /* namespace selftests */

#endif /* CXX_STD_THREAD */

INIT_GDB_FILE (parallel_for_selftests)
{
#ifdef CXX_STD_THREAD
  selftests::register_test ("parallel_for",
			    selftests::parallel_for::test_parallel_for_each);
#endif /* CXX_STD_THREAD */
}
