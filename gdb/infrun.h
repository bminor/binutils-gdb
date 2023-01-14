/* Copyright (C) 1986-2021 Free Software Foundation, Inc.

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

#ifndef INFRUN_H
#define INFRUN_H 1

#include "symtab.h"
#include "gdbsupport/byte-vector.h"

struct target_waitstatus;
struct frame_info;
struct address_space;
struct return_value_info;
struct process_stratum_target;
struct thread_info;

/* True if we are debugging run control.  */
extern bool debug_infrun;

/* Print an "infrun" debug statement.  */

#define infrun_debug_printf(fmt, ...) \
  debug_prefixed_printf_cond (debug_infrun, "infrun",fmt, ##__VA_ARGS__)

/* Print "infrun" start/end debug statements.  */

#define INFRUN_SCOPED_DEBUG_START_END(msg) \
  scoped_debug_start_end (debug_infrun, "infrun", msg)

/* Print "infrun" enter/exit debug statements.  */

#define INFRUN_SCOPED_DEBUG_ENTER_EXIT \
  scoped_debug_enter_exit (debug_infrun, "infrun")

/* Nonzero if we want to give control to the user when we're notified
   of shared library events by the dynamic linker.  */
extern int stop_on_solib_events;

/* True if execution commands resume all threads of all processes by
   default; otherwise, resume only threads of the current inferior
   process.  */
extern bool sched_multi;

/* When set, stop the 'step' command if we enter a function which has
   no line number information.  The normal behavior is that we step
   over such function.  */
extern bool step_stop_if_no_debug;

/* If set, the inferior should be controlled in non-stop mode.  In
   this mode, each thread is controlled independently.  Execution
   commands apply only to the selected thread by default, and stop
   events stop only the thread that had the event -- the other threads
   are kept running freely.  */
extern bool non_stop;

/* When set (default), the target should attempt to disable the
   operating system's address space randomization feature when
   starting an inferior.  */
extern bool disable_randomization;

/* Returns a unique identifier for the current stop.  This can be used
   to tell whether a command has proceeded the inferior past the
   current location.  */
extern ULONGEST get_stop_id (void);

/* Reverse execution.  */
enum exec_direction_kind
  {
    EXEC_FORWARD,
    EXEC_REVERSE
  };

/* The current execution direction.  */
extern enum exec_direction_kind execution_direction;

extern void start_remote (int from_tty);

/* Clear out all variables saying what to do when inferior is
   continued or stepped.  First do this, then set the ones you want,
   then call `proceed'.  STEP indicates whether we're preparing for a
   step/stepi command.  */
extern void clear_proceed_status (int step);

extern void proceed (CORE_ADDR, enum gdb_signal);

/* Return a ptid representing the set of threads that we will proceed,
   in the perspective of the user/frontend.  We may actually resume
   fewer threads at first, e.g., if a thread is stopped at a
   breakpoint that needs stepping-off, but that should not be visible
   to the user/frontend, and neither should the frontend/user be
   allowed to proceed any of the threads that happen to be stopped for
   internal run control handling, if a previous command wanted them
   resumed.  */
extern ptid_t user_visible_resume_ptid (int step);

/* Return the process_stratum target that we will proceed, in the
   perspective of the user/frontend.  If RESUME_PTID is
   MINUS_ONE_PTID, then we'll resume all threads of all targets, so
   the function returns NULL.  Otherwise, we'll be resuming a process
   or thread of the current process, so we return the current
   inferior's process stratum target.  */
extern process_stratum_target *user_visible_resume_target (ptid_t resume_ptid);

/* Return control to GDB when the inferior stops for real.  Print
   appropriate messages, remove breakpoints, give terminal our modes,
   and run the stop hook.  Returns true if the stop hook proceeded the
   target, false otherwise.  */
extern int normal_stop (void);

/* Return the cached copy of the last target/ptid/waitstatus returned
   by target_wait()/deprecated_target_wait_hook().  The data is
   actually cached by handle_inferior_event(), which gets called
   immediately after target_wait()/deprecated_target_wait_hook().  */
extern void get_last_target_status (process_stratum_target **target,
				    ptid_t *ptid,
				    struct target_waitstatus *status);

/* Set the cached copy of the last target/ptid/waitstatus.  */
extern void set_last_target_status (process_stratum_target *target, ptid_t ptid,
				    struct target_waitstatus status);

/* Clear the cached copy of the last ptid/waitstatus returned by
   target_wait().  */
extern void nullify_last_target_wait_ptid ();

/* Stop all threads.  Only returns after everything is halted.  */
extern void stop_all_threads (void);

extern void prepare_for_detach (void);

extern void fetch_inferior_event ();

extern void init_wait_for_inferior (void);

extern void insert_step_resume_breakpoint_at_sal (struct gdbarch *,
						  struct symtab_and_line ,
						  struct frame_id);

/* Returns true if we're trying to step past the instruction at
   ADDRESS in ASPACE.  */
extern int stepping_past_instruction_at (struct address_space *aspace,
					 CORE_ADDR address);

/* Returns true if thread whose thread number is THREAD is stepping
   over a breakpoint.  */
extern int thread_is_stepping_over_breakpoint (int thread);

/* Returns true if we're trying to step past an instruction that
   triggers a non-steppable watchpoint.  */
extern int stepping_past_nonsteppable_watchpoint (void);

/* Record in TP the frame and location we're currently stepping through.  */
extern void set_step_info (thread_info *tp,
			   struct frame_info *frame,
			   struct symtab_and_line sal);

/* Several print_*_reason helper functions to print why the inferior
   has stopped to the passed in UIOUT.  */

/* Signal received, print why the inferior has stopped.  */
extern void print_signal_received_reason (struct ui_out *uiout,
					  enum gdb_signal siggnal);

/* Print why the inferior has stopped.  We are done with a
   step/next/si/ni command, print why the inferior has stopped.  */
extern void print_end_stepping_range_reason (struct ui_out *uiout);

/* The inferior was terminated by a signal, print why it stopped.  */
extern void print_signal_exited_reason (struct ui_out *uiout,
					enum gdb_signal siggnal);

/* The inferior program is finished, print why it stopped.  */
extern void print_exited_reason (struct ui_out *uiout, int exitstatus);

/* Reverse execution: target ran out of history info, print why the
   inferior has stopped.  */
extern void print_no_history_reason (struct ui_out *uiout);

/* Print the result of a function at the end of a 'finish' command.
   RV points at an object representing the captured return value/type
   and its position in the value history.  */

extern void print_return_value (struct ui_out *uiout,
				struct return_value_info *rv);

/* Print current location without a level number, if we have changed
   functions or hit a breakpoint.  Print source line if we have one.
   If the execution command captured a return value, print it.  If
   DISPLAYS is false, do not call 'do_displays'.  */

extern void print_stop_event (struct ui_out *uiout, bool displays = true);

/* Pretty print the results of target_wait, for debugging purposes.  */

extern void print_target_wait_results (ptid_t waiton_ptid, ptid_t result_ptid,
				       const struct target_waitstatus *ws);

extern int signal_stop_state (int);

extern int signal_print_state (int);

extern int signal_pass_state (int);

extern int signal_stop_update (int, int);

extern int signal_print_update (int, int);

extern int signal_pass_update (int, int);

extern void update_signals_program_target (void);

/* Clear the convenience variables associated with the exit of the
   inferior.  Currently, those variables are $_exitcode and
   $_exitsignal.  */
extern void clear_exit_convenience_vars (void);

/* Dump LEN bytes at BUF in hex to a string and return it.  */
extern std::string displaced_step_dump_bytes (const gdb_byte *buf, size_t len);

extern void update_observer_mode (void);

extern void signal_catch_update (const unsigned int *);

/* In some circumstances we allow a command to specify a numeric
   signal.  The idea is to keep these circumstances limited so that
   users (and scripts) develop portable habits.  For comparison,
   POSIX.2 `kill' requires that 1,2,3,6,9,14, and 15 work (and using a
   numeric signal at all is obsolescent.  We are slightly more lenient
   and allow 1-15 which should match host signal numbers on most
   systems.  Use of symbolic signal names is strongly encouraged.  */
enum gdb_signal gdb_signal_from_command (int num);

/* Enables/disables infrun's async event source in the event loop.  */
extern void infrun_async (int enable);

/* Call infrun's event handler the next time through the event
   loop.  */
extern void mark_infrun_async_event_handler (void);

/* The global chain of threads that need to do a step-over operation
   to get past e.g., a breakpoint.  */
extern struct thread_info *global_thread_step_over_chain_head;

/* Remove breakpoints if possible (usually that means, if everything
   is stopped).  On failure, print a message.  */
extern void maybe_remove_breakpoints (void);

/* If a UI was in sync execution mode, and now isn't, restore its
   prompt (a synchronous execution command has finished, and we're
   ready for input).  */
extern void all_uis_check_sync_execution_done (void);

/* If a UI was in sync execution mode, and hasn't displayed the prompt
   yet, re-disable its prompt (a synchronous execution command was
   started or re-started).  */
extern void all_uis_on_sync_execution_starting (void);

#endif /* INFRUN_H */
