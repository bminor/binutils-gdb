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

/* Base class for displaced stepping closures (the arch-specific data).  */

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
    failed_before = 0;
    step_thread = nullptr;
    step_gdbarch = nullptr;
    step_closure.reset ();
    step_original = 0;
    step_copy = 0;
    step_saved_copy.clear ();
  }

  /* True if preparing a displaced step ever failed.  If so, we won't
     try displaced stepping for this inferior again.  */
  int failed_before;

  /* If this is not nullptr, this is the thread carrying out a
     displaced single-step in process PID.  This thread's state will
     require fixing up once it has completed its step.  */
  thread_info *step_thread;

  /* The architecture the thread had when we stepped it.  */
  gdbarch *step_gdbarch;

  /* The closure provided gdbarch_displaced_step_copy_insn, to be used
     for post-step cleanup.  */
  displaced_step_copy_insn_closure_up step_closure;

  /* The address of the original instruction, and the copy we
     made.  */
  CORE_ADDR step_original, step_copy;

  /* Saved contents of copy area.  */
  gdb::byte_vector step_saved_copy;
};

#endif /* DISPLACED_STEPPING_H */
