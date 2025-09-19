/* Synchronized work queue.

   Copyright (C) 2025 Free Software Foundation, Inc.

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

#ifndef GDBSUPPORT_WORK_QUEUE_H
#define GDBSUPPORT_WORK_QUEUE_H

#include "gdbsupport/iterator-range.h"

namespace gdb
{

/* Implementation of a thread-safe work queue.

   The work items are specified by two iterators of type RandomIt.

   BATCH_SIZE is the number of work items to pop in a batch.  */

template<typename RandomIt, std::size_t batch_size>
class work_queue
{
public:
  /* The work items are specified by the range `[first, last[`.  */
  work_queue (const RandomIt first, const RandomIt last) noexcept
    : m_next (first),
      m_last (last)
  {
    gdb_assert (first <= last);
  }

  DISABLE_COPY_AND_ASSIGN (work_queue);

  /* Pop a batch of work items.

     The return value is an iterator range delimiting the work items.  */
  iterator_range<RandomIt> pop_batch () noexcept
  {
    for (;;)
      {
	/* Grab a snapshot of M_NEXT.  */
	auto next = m_next.load ();
	gdb_assert (next <= m_last);

	/* The number of items remaining in the queue.  */
	const auto n_remaining = static_cast<std::size_t> (m_last - next);

	/* Are we done?  */
	if (n_remaining == 0)
	  return { m_last, m_last };

	/* The batch size is proportional to the number of items remaining in
	   the queue.  We do this to try to stike a balance, avoiding
	   synchronization overhead when there are many items to process at the
	   start, and avoiding workload imbalance when there are few items to
	   process at the end.  */
	const auto this_batch_size = std::min (batch_size, n_remaining);

	/* The range of items in this batch.  */
	const auto this_batch_first = next;
	const auto this_batch_last = next + this_batch_size;

	/* Update M_NEXT.  If the current value of M_NEXT doesn't match NEXT, it
	   means another thread updated it concurrently, restart.  */
	if (!m_next.compare_exchange_weak (next, this_batch_last))
	  continue;

	return { this_batch_first, this_batch_last };
      }
  }

private:
  /* The next work item to hand out.  */
  std::atomic<RandomIt> m_next;

  /* The end of the work item range.  */
  RandomIt m_last;
};

} /* namespace gdb */

#endif /* GDBSUPPORT_WORK_QUEUE_H */
