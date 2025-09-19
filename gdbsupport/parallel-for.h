/* Parallel for loops

   Copyright (C) 2019-2025 Free Software Foundation, Inc.

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
#include <atomic>
#include <tuple>
#include "gdbsupport/iterator-range.h"
#include "gdbsupport/thread-pool.h"
#include "gdbsupport/work-queue.h"

namespace gdb
{

/* If enabled, print debug info about the inner workings of the parallel for
   each functions.  */
constexpr bool parallel_for_each_debug = false;

/* A "parallel-for" implementation using a shared work queue.  Work items get
   popped in batches of size up to BATCH_SIZE from the queue and handed out to
   worker threads.

   Each worker thread instantiates an object of type Worker, forwarding ARGS to
   its constructor.  The Worker object can be used to keep some per-worker
   thread state.

   Worker threads call Worker::operator() repeatedly until the queue is
   empty.

   This function is synchronous, meaning that it blocks and returns once the
   processing is complete.  */

template<std::size_t batch_size, class RandomIt, class Worker,
	 class... WorkerArgs>
void
parallel_for_each (const RandomIt first, const RandomIt last,
		   WorkerArgs &&...worker_args)
{
  gdb_assert (first <= last);

  if (parallel_for_each_debug)
    {
      debug_printf ("Parallel for: n elements: %zu\n",
		    static_cast<std::size_t> (last - first));
      debug_printf ("Parallel for: batch size: %zu\n", batch_size);
    }

  std::vector<gdb::future<void>> results;
  work_queue<RandomIt, batch_size> queue (first, last);

  /* The worker thread task.

     We need to capture args as a tuple, because it's not possible to capture
     the parameter pack directly in C++17.  Once we migrate to C++20, the
     capture can be simplified to:

       ... args = std::forward<Args>(args)

     and `args` can be used as-is in the lambda.  */
  auto args_tuple
    = std::forward_as_tuple (std::forward<WorkerArgs> (worker_args)...);
  auto task = [&queue, first, &args_tuple] ()
    {
      /* Instantiate the user-defined worker.  */
      auto worker = std::make_from_tuple<Worker> (args_tuple);

      for (;;)
	{
	  const auto batch = queue.pop_batch ();

	  if (batch.empty ())
	    break;

	  if (parallel_for_each_debug)
	    debug_printf ("Processing %zu items, range [%zu, %zu[\n",
			  batch.size (),
			  batch.begin () - first,
			  batch.end () - first);

	  worker (batch);
	}
    };

  /* Start N_WORKER_THREADS tasks.  */
  const size_t n_worker_threads
    = std::max<size_t> (thread_pool::g_thread_pool->thread_count (), 1);

  for (int i = 0; i < n_worker_threads; ++i)
    results.push_back (gdb::thread_pool::g_thread_pool->post_task (task));

  /* Wait for all of them to be finished.  */
  for (auto &fut : results)
    fut.get ();
}

/* A sequential drop-in replacement of parallel_for_each.  This can be useful
   when debugging multi-threading behavior, and you want to limit
   multi-threading in a fine-grained way.  */

template<class RandomIt, class Worker, class... WorkerArgs>
void
sequential_for_each (RandomIt first, RandomIt last, WorkerArgs &&...worker_args)
{
  if (first == last)
    return;

  Worker (std::forward<WorkerArgs> (worker_args)...) ({ first, last });
}

namespace detail
{

/* Type to hold the state shared between threads of
   gdb::parallel_for_each_async.  */

template<std::size_t min_batch_size, typename RandomIt, typename... WorkerArgs>
struct pfea_state
{
  pfea_state (RandomIt first, RandomIt last, std::function<void ()> &&done,
	      WorkerArgs &&...worker_args)
    : first (first),
      last (last),
      worker_args_tuple (std::forward_as_tuple
			 (std::forward<WorkerArgs> (worker_args)...)),
      queue (first, last),
      m_done (std::move (done))
  {}

  DISABLE_COPY_AND_ASSIGN (pfea_state);

  /* This gets called by the last worker thread that drops its reference on
     the shared state, thus when the processing is complete.  */
  ~pfea_state ()
  {
    if (m_done)
      m_done ();
  }

  /* The interval to process.  */
  const RandomIt first, last;

  /* Tuple of arguments to pass when constructing the user's worker object.

     Use std::decay_t to avoid storing references to the caller's local
     variables.  If we didn't use it and the caller passed an lvalue `foo *`,
     we would store it as a reference to `foo *`, thus storing a reference to
     the caller's local variable.

     The downside is that it's not possible to pass arguments by reference,
     callers need to pass pointers or std::reference_wrappers.  */
  std::tuple<std::decay_t<WorkerArgs>...> worker_args_tuple;

  /* Work queue that worker threads pull work items from.  */
  work_queue<RandomIt, min_batch_size> queue;

private:
  /* Callable called when the parallel-for is done.  */
  std::function<void ()> m_done;
};

} /* namespace detail */

/* A "parallel-for" implementation using a shared work queue.  Work items get
   popped in batches from the queue and handed out to worker threads.

   Batch sizes are proportional to the number of remaining items in the queue,
   but always greater or equal to MIN_BATCH_SIZE.

   The DONE callback is invoked when processing is done.

   Each worker thread instantiates an object of type Worker, forwarding ARGS to
   its constructor.  The Worker object can be used to keep some per-worker
   thread state.  This version does not support passing references as arguments
   to the worker.  Use std::reference_wrapper or pointers instead.

   Worker threads call Worker::operator() repeatedly until the queue is
   empty.

   This function is asynchronous.  An arbitrary worker thread will call the DONE
   callback when processing is done.  */

template<std::size_t min_batch_size, class RandomIt, class Worker,
	 class... WorkerArgs>
void
parallel_for_each_async (const RandomIt first, const RandomIt last,
			 std::function<void ()> &&done,
			 WorkerArgs &&...worker_args)
{
  gdb_assert (first <= last);

  if (parallel_for_each_debug)
    {
      debug_printf ("Parallel for: n elements: %zu\n",
		    static_cast<std::size_t> (last - first));
      debug_printf ("Parallel for: min batch size: %zu\n", min_batch_size);
    }

  const size_t n_worker_threads
    = std::max<size_t> (thread_pool::g_thread_pool->thread_count (), 1);

  /* The state shared between all worker threads.  All worker threads get a
     reference on the shared pointer through the lambda below.  The last worker
     thread to drop its reference will cause this object to be destroyed, which
     will call the DONE callback.  */
  using state_t = detail::pfea_state<min_batch_size, RandomIt, WorkerArgs...>;
  auto state
    = std::make_shared<state_t> (first, last, std::move (done),
				 std::forward<WorkerArgs> (worker_args)...);

  /* The worker thread task.  */
  auto task = [state] ()
    {
      /* Instantiate the user-defined worker.  */
      auto worker = std::make_from_tuple<Worker> (state->worker_args_tuple);

      for (;;)
	{
	  const auto batch = state->queue.pop_batch ();

	  if (batch.empty ())
	    break;

	  if (parallel_for_each_debug)
	    debug_printf ("Processing %zu items, range [%zu, %zu[\n",
			  batch.size (),
			  batch.begin () - state->first,
			  batch.end () - state->first);

	  worker (batch);
	}
    };

  /* Start N_WORKER_THREADS tasks.  */
  for (int i = 0; i < n_worker_threads; ++i)
    gdb::thread_pool::g_thread_pool->post_task (task);
}

}

#endif /* GDBSUPPORT_PARALLEL_FOR_H */
