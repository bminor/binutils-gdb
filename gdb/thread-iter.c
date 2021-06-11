/* Thread iterators and ranges for GDB, the GNU debugger.

   Copyright (C) 2018-2021 Free Software Foundation, Inc.

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
#include "gdbthread.h"
#include "inferior.h"

/* See thread-iter.h.  */

all_threads_iterator::all_threads_iterator (begin_t)
{
  /* Advance M_INF/M_THR to the first thread's position.  */
  for (m_inf = inferior_list; m_inf != NULL; m_inf = m_inf->next)
    {
      auto thr_iter = m_inf->thread_list.begin ();
      if (thr_iter != m_inf->thread_list.end ())
	{
	  m_thr = &*thr_iter;
	  return;
	}
    }
  m_thr = nullptr;
}

/* See thread-iter.h.  */

void
all_threads_iterator::advance ()
{
  intrusive_list<thread_info>::iterator thr_iter (m_thr);

  /* The loop below is written in the natural way as-if we'd always
     start at the beginning of the inferior list.  This fast forwards
     the algorithm to the actual current position.  */
  goto start;

  for (; m_inf != NULL; m_inf = m_inf->next)
    {
      thr_iter = m_inf->thread_list.begin ();
      while (thr_iter != m_inf->thread_list.end ())
	{
	  m_thr = &*thr_iter;
	  return;
	start:
	  ++thr_iter;
	}
    }

  m_thr = nullptr;
}

/* See thread-iter.h.  */

bool
all_matching_threads_iterator::m_inf_matches ()
{
  return ((m_filter_target == nullptr
	   || m_filter_target == m_inf->process_target ())
	  && (m_filter_ptid == minus_one_ptid
	      || m_filter_ptid.pid () == m_inf->pid));
}

/* See thread-iter.h.  */

all_matching_threads_iterator::all_matching_threads_iterator
  (process_stratum_target *filter_target, ptid_t filter_ptid)
    : m_filter_target (filter_target),
      m_filter_ptid (filter_ptid)
{
  gdb_assert ((filter_target == nullptr && filter_ptid == minus_one_ptid)
	      || filter_target->stratum () == process_stratum);

  for (m_inf = inferior_list; m_inf != NULL; m_inf = m_inf->next)
    if (m_inf_matches ())
      for (auto thr_iter = m_inf->thread_list.begin ();
	   thr_iter != m_inf->thread_list.end ();
	   ++thr_iter)
	if (thr_iter->ptid.matches (m_filter_ptid))
	  {
	    m_thr = &*thr_iter;
	    return;
	  }

  m_thr = nullptr;
}

/* See thread-iter.h.  */

void
all_matching_threads_iterator::advance ()
{
  intrusive_list<thread_info>::iterator thr_iter (m_thr);

  /* The loop below is written in the natural way as-if we'd always
     start at the beginning of the inferior list.  This fast forwards
     the algorithm to the actual current position.  */
  goto start;

  for (; m_inf != NULL; m_inf = m_inf->next)
    if (m_inf_matches ())
      {
	thr_iter = m_inf->thread_list.begin ();
	while (thr_iter != m_inf->thread_list.end ())
	  {
	    if (thr_iter->ptid.matches (m_filter_ptid))
	      {
		m_thr = &*thr_iter;
		return;
	      }
	  start:
	    ++thr_iter;
	  }
      }

  m_thr = nullptr;
}
