/* Parallel for loops

   Copyright (C) 2019-2022 Free Software Foundation, Inc.

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
#include <type_traits>
#include "gdbsupport/thread-pool.h"

namespace gdb
{

namespace detail
{

/* This is a helper class that is used to accumulate results for
   parallel_for.  There is a specialization for 'void', below.  */
template<typename T>
struct par_for_accumulator
{
public:

  explicit par_for_accumulator (size_t n_threads)
    : m_futures (n_threads)
  {
  }

  /* The result type that is accumulated.  */
  typedef std::vector<T> result_type;

  /* Post the Ith task to a background thread, and store a future for
     later.  */
  void post (size_t i, std::function<T ()> task)
  {
    m_futures[i]
      = gdb::thread_pool::g_thread_pool->post_task (std::move (task));
  }

  /* Invoke TASK in the current thread, then compute all the results
     from all background tasks and put them into a result vector,
     which is returned.  */
  result_type finish (gdb::function_view<T ()> task)
  {
    result_type result (m_futures.size () + 1);

    result.back () = task ();

    for (size_t i = 0; i < m_futures.size (); ++i)
      result[i] = m_futures[i].get ();

    return result;
  }

private:
  
  /* A vector of futures coming from the tasks run in the
     background.  */
  std::vector<std::future<T>> m_futures;
};

/* See the generic template.  */
template<>
struct par_for_accumulator<void>
{
public:

  explicit par_for_accumulator (size_t n_threads)
    : m_futures (n_threads)
  {
  }

  /* This specialization does not compute results.  */
  typedef void result_type;

  void post (size_t i, std::function<void ()> task)
  {
    m_futures[i]
      = gdb::thread_pool::g_thread_pool->post_task (std::move (task));
  }

  result_type finish (gdb::function_view<void ()> task)
  {
    task ();

    for (auto &future : m_futures)
      {
	/* Use 'get' and not 'wait', to propagate any exception.  */
	future.get ();
      }
  }

private:

  std::vector<std::future<void>> m_futures;
};

}

/* A very simple "parallel for".  This splits the range of iterators
   into subranges, and then passes each subrange to the callback.  The
   work may or may not be done in separate threads.

   This approach was chosen over having the callback work on single
   items because it makes it simple for the caller to do
   once-per-subrange initialization and destruction.

   The parameter N says how batching ought to be done -- there will be
   at least N elements processed per thread.  Setting N to 0 is not
   allowed.

   If the function returns a non-void type, then a vector of the
   results is returned.  The size of the resulting vector depends on
   the number of threads that were used.  */

template<class RandomIt, class RangeFunction>
typename gdb::detail::par_for_accumulator<
    std::result_of_t<RangeFunction (RandomIt, RandomIt)>
  >::result_type
parallel_for_each (unsigned n, RandomIt first, RandomIt last,
		   RangeFunction callback)
{
  typedef typename std::result_of_t<RangeFunction (RandomIt, RandomIt)>
    result_type;

  size_t n_threads = thread_pool::g_thread_pool->thread_count ();
  size_t n_elements = last - first;
  size_t elts_per_thread = 0;
  if (n_threads > 1)
    {
      /* Require that there should be at least N elements in a
	 thread.  */
      gdb_assert (n > 0);
      if (n_elements / n_threads < n)
	n_threads = std::max (n_elements / n, (size_t) 1);
      elts_per_thread = n_elements / n_threads;
    }

  size_t count = n_threads == 0 ? 0 : n_threads - 1;
  gdb::detail::par_for_accumulator<result_type> results (count);

  for (int i = 0; i < count; ++i)
    {
      RandomIt end = first + elts_per_thread;
      results.post (i, [=] ()
        {
	  return callback (first, end);
	});
      first = end;
    }

  /* Process all the remaining elements in the main thread.  */
  return results.finish ([=] ()
    {
      return callback (first, last);
    });
}

}

#endif /* GDBSUPPORT_PARALLEL_FOR_H */
