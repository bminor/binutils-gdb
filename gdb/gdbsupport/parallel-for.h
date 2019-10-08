/* Parallel for loops

   Copyright (C) 2019 Free Software Foundation, Inc.

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

#ifndef GDBSUPPORT_PARALLEL_FOR_H
#define GDBSUPPORT_PARALLEL_FOR_H

#include <algorithm>
#if CXX_STD_THREAD
#include <condition_variable>
#include <system_error>
#include <thread>
#endif

#include "gdbsupport/block-signals.h"
#include "gdbsupport/thread_pool.h"

namespace gdb
{

/* True if threading should be enabled.  */

extern int max_threads;

extern thread_pool parallel_for_pool;

/* A very simple "parallel for".  This splits the range of iterators
   into subranges, and then passes each subrange to the callback.  The
   work may or may not be done in separate threads.

   This approach was chosen over having the callback work on single
   items because it makes it simple for the caller to do
   once-per-subrange initialization and destruction.  */

template<class RandomIt, class RangeFunction>
void
parallel_for_each (RandomIt first, RandomIt last, RangeFunction callback)
{
#if CXX_STD_THREAD
  int n_threads = std::thread::hardware_concurrency ();
  /* So we can use a local array below.  */
  const int local_max = 16;
  /* Be sure to handle the "unlimited" case.  */
  if (max_threads >= 0 && n_threads > max_threads)
    n_threads = max_threads;
  if (n_threads > local_max)
    n_threads = local_max;
  if (!parallel_for_pool.started ())
  {
    /* Ensure that signals used by gdb are blocked in the new
       threads.  */
    block_signals blocker;
    parallel_for_pool.start (n_threads);
  }

  std::future<void> futures[n_threads];

  size_t n_elements = last - first;
  if (n_threads > 1 && 2 * n_threads <= n_elements)
    {
      size_t elts_per_thread = n_elements / n_threads;
      for (int i = 0; i < n_threads; ++i)
	{
	  RandomIt end = first + elts_per_thread;
	  futures[i] = parallel_for_pool.post_task ([&, first, end] () {
						     callback (first, end);
						     });
	  first = end;
	}
    }
  else
    n_threads = 0;
#endif /* CXX_STD_THREAD */

  /* Process all the remaining elements in the main thread.  */
  callback (first, last);
#ifdef CXX_STD_THREAD
  for (size_t i = 0; i < n_threads; ++i)
    futures[i].wait ();
#endif /* CXX_STD_THREAD */
}

}

#endif /* GDBSUPPORT_PARALLEL_FOR_H */
