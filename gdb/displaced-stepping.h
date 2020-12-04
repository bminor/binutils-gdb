/* Displaced stepping related things.

   Copyright (C) 2020 Free Software Foundation, Inc.

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

#ifndef DISPLACED_STEPPING_H
#define DISPLACED_STEPPING_H

#include "gdbsupport/byte-vector.h"

struct gdbarch;
struct thread_info;

/* True if we are debugging displaced stepping.  */

extern bool debug_displaced;

/* Print a "displaced" debug statement.  */

#define displaced_debug_printf(fmt, ...) \
  do \
    { \
      if (debug_displaced) \
	debug_prefixed_printf ("displaced", __func__, fmt, ##__VA_ARGS__); \
    } \
  while (0)

enum displaced_step_prepare_status
{
  /* A displaced stepping buffer was successfully allocated and prepared.  */
  DISPLACED_STEP_PREPARE_STATUS_OK,

  /* This particular instruction can't be displaced stepped, GDB should fall
     back on in-line stepping.  */
  DISPLACED_STEP_PREPARE_STATUS_CANT,

  /* Not enough resources are available at this time, try again later.  */
  DISPLACED_STEP_PREPARE_STATUS_UNAVAILABLE,
};

enum displaced_step_finish_status
{
  /* Either the instruction was stepped and fixed up, or the specified thread
     wasn't executing a displaced step (in which case there's nothing to
     finish). */
  DISPLACED_STEP_FINISH_STATUS_OK,

  /* The thread started a displaced step, but didn't complete it.  */
  DISPLACED_STEP_FINISH_STATUS_NOT_EXECUTED,
};

/* Data returned by a gdbarch displaced_step_copy_insn method, to be passed to
   the matching displaced_step_fixup method.  */

struct displaced_step_copy_insn_closure
{
  virtual ~displaced_step_copy_insn_closure () = 0;
};

using displaced_step_copy_insn_closure_up
  = std::unique_ptr<displaced_step_copy_insn_closure>;

/* A simple displaced step closure that contains only a byte buffer.  */

struct buf_displaced_step_copy_insn_closure : displaced_step_copy_insn_closure
{
  buf_displaced_step_copy_insn_closure (int buf_size)
  : buf (buf_size)
  {}

  /* The content of this buffer is up to the user of the class, but typically
     original instruction bytes, used during fixup to determine what needs to
     be fixed up.  */
  gdb::byte_vector buf;
};

/* Per-inferior displaced stepping state.  */

struct displaced_step_inferior_state
{
  displaced_step_inferior_state ()
  {
    reset ();
  }

  /* Put this object back in its original state.  */
  void reset ()
  {
    failed_before = false;
    in_progress_count = 0;
    unavailable = false;
  }

  /* True if preparing a displaced step ever failed.  If so, we won't
     try displaced stepping for this inferior again.  */
  bool failed_before;

  /* Number of displaced steps in progress for this inferior.  */
  unsigned int in_progress_count;

  /* If true, this tells GDB that it's not worth asking the gdbarch displaced
     stepping implementation to prepare a displaced step, because it would
     return UNAVAILABLE.  This is set and reset by the gdbarch in the
     displaced_step_prepare and displaced_step_finish methods.  */
  bool unavailable;
};

/* Per-thread displaced stepping state.  */

struct displaced_step_thread_state
{
  /* Return true if this thread is currently executing a displaced step.  */
  bool in_progress () const
  {
    return m_original_gdbarch != nullptr;
  }

  /* Return the gdbarch of the thread prior to the step.  */
  gdbarch *get_original_gdbarch () const
  {
    return m_original_gdbarch;
  }

  /* Mark this thread as currently executing a displaced step.

     ORIGINAL_GDBARCH is the current gdbarch of the thread (before the step
     is executed).  */
  void set (gdbarch *original_gdbarch)
  {
    m_original_gdbarch = original_gdbarch;
  }

  /* Mark this thread as no longer executing a displaced step.  */
  void reset ()
  {
    m_original_gdbarch = nullptr;
  }

private:
  gdbarch *m_original_gdbarch = nullptr;
};

/* Manage access to a single displaced stepping buffer.  */

struct displaced_step_buffer
{
  explicit displaced_step_buffer (CORE_ADDR buffer_addr)
    : m_addr (buffer_addr)
  {}

  displaced_step_prepare_status prepare (thread_info *thread,
					 CORE_ADDR &displaced_pc);

  displaced_step_finish_status finish (gdbarch *arch, thread_info *thread,
				       gdb_signal sig);

  const displaced_step_copy_insn_closure *
    copy_insn_closure_by_addr (CORE_ADDR addr);

  void restore_in_ptid (ptid_t ptid);

private:
  /* Original PC of the instruction being displaced-stepped in this buffer.  */
  CORE_ADDR m_original_pc = 0;

  /* Address of the buffer.  */
  const CORE_ADDR m_addr;

  /* If set, the thread currently using the buffer.  */
  thread_info *m_current_thread = nullptr;

  /* Saved contents of copy area.  */
  gdb::byte_vector m_saved_copy;

  /* The closure provided gdbarch_displaced_step_copy_insn, to be used
     for post-step cleanup.  */
  displaced_step_copy_insn_closure_up m_copy_insn_closure;
};

#endif /* DISPLACED_STEPPING_H */
