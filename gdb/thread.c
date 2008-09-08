/* Multi-process/thread control for GDB, the GNU debugger.

   Copyright (C) 1986, 1987, 1988, 1993, 1994, 1995, 1996, 1997, 1998, 1999,
   2000, 2001, 2002, 2003, 2004, 2007, 2008 Free Software Foundation, Inc.

   Contributed by Lynx Real-Time Systems, Inc.  Los Gatos, CA.

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
#include "symtab.h"
#include "frame.h"
#include "inferior.h"
#include "environ.h"
#include "value.h"
#include "target.h"
#include "gdbthread.h"
#include "exceptions.h"
#include "command.h"
#include "gdbcmd.h"
#include "regcache.h"
#include "gdb.h"
#include "gdb_string.h"

#include <ctype.h>
#include <sys/types.h>
#include <signal.h>
#include "ui-out.h"
#include "observer.h"
#include "annotate.h"
#include "cli/cli-decode.h"

/* Definition of struct thread_info exported to gdbthread.h */

/* Prototypes for exported functions. */

void _initialize_thread (void);

/* Prototypes for local functions. */

static struct thread_info *thread_list = NULL;
static int highest_thread_num;

static void thread_command (char *tidstr, int from_tty);
static void thread_apply_all_command (char *, int);
static int thread_alive (struct thread_info *);
static void info_threads_command (char *, int);
static void thread_apply_command (char *, int);
static void restore_current_thread (ptid_t);
static void prune_threads (void);

/* Frontend view of the thread state.  Possible extensions: stepping,
   finishing, until(ling),...  */
enum thread_state
{
  THREAD_STOPPED,
  THREAD_RUNNING,
  THREAD_EXITED,
};

static enum thread_state main_thread_state = THREAD_STOPPED;
static int main_thread_executing = 0;

extern struct thread_info*
inferior_thread (void)
{
  struct thread_info *tp = find_thread_pid (inferior_ptid);
  gdb_assert (tp);
  return tp;
}

void
delete_step_resume_breakpoint (struct thread_info *tp)
{
  if (tp && tp->step_resume_breakpoint)
    {
      delete_breakpoint (tp->step_resume_breakpoint);
      tp->step_resume_breakpoint = NULL;
    }
}

static void
clear_thread_inferior_resources (struct thread_info *tp)
{
  /* NOTE: this will take care of any left-over step_resume breakpoints,
     but not any user-specified thread-specific breakpoints.  We can not
     delete the breakpoint straight-off, because the inferior might not
     be stopped at the moment.  */
  if (tp->step_resume_breakpoint)
    {
      tp->step_resume_breakpoint->disposition = disp_del_at_next_stop;
      tp->step_resume_breakpoint = NULL;
    }

  bpstat_clear (&tp->stop_bpstat);

  discard_all_intermediate_continuations_thread (tp);
  discard_all_continuations_thread (tp);
}

static void
free_thread (struct thread_info *tp)
{
  clear_thread_inferior_resources (tp);

  /* FIXME: do I ever need to call the back-end to give it a
     chance at this private data before deleting the thread?  */
  if (tp->private)
    xfree (tp->private);

  xfree (tp);
}

void
init_thread_list (void)
{
  struct thread_info *tp, *tpnext;

  highest_thread_num = 0;
  main_thread_state = THREAD_STOPPED;
  main_thread_executing = 0;

  if (!thread_list)
    return;

  for (tp = thread_list; tp; tp = tpnext)
    {
      tpnext = tp->next;
      free_thread (tp);
    }

  thread_list = NULL;
}

struct thread_info *
add_thread_silent (ptid_t ptid)
{
  struct thread_info *tp;

  tp = find_thread_pid (ptid);
  if (tp)
    /* Found an old thread with the same id.  It has to be dead,
       otherwise we wouldn't be adding a new thread with the same id.
       The OS is reusing this id --- delete it, and recreate a new
       one.  */
    {
      /* In addition to deleting the thread, if this is the current
	 thread, then we need to also get rid of the current infrun
	 context, and take care that delete_thread doesn't really
	 delete the thread if it is inferior_ptid.  Create a new
	 template thread in the list with an invalid ptid, context
	 switch to it, delete the original thread, reset the new
	 thread's ptid, and switch to it.  */

      if (ptid_equal (inferior_ptid, ptid))
	{
	  tp = xmalloc (sizeof (*tp));
	  memset (tp, 0, sizeof (*tp));
	  tp->ptid = minus_one_ptid;
	  tp->num = ++highest_thread_num;
	  tp->next = thread_list;
	  thread_list = tp;
	  context_switch_to (minus_one_ptid);

	  /* Now we can delete it.  */
	  delete_thread (ptid);

	  /* Since the context is already set to this new thread,
	     reset its ptid, and reswitch inferior_ptid to it.  */
	  tp->ptid = ptid;
	  switch_to_thread (ptid);

	  observer_notify_new_thread (tp);

	  /* All done.  */
	  return tp;
	}
      else
	/* Just go ahead and delete it.  */
	delete_thread (ptid);
    }

  tp = (struct thread_info *) xmalloc (sizeof (*tp));
  memset (tp, 0, sizeof (*tp));
  tp->ptid = ptid;
  tp->num = ++highest_thread_num;
  tp->next = thread_list;
  thread_list = tp;

  observer_notify_new_thread (tp);

  return tp;
}

struct thread_info *
add_thread_with_info (ptid_t ptid, struct private_thread_info *private)
{
  struct thread_info *result = add_thread_silent (ptid);

  result->private = private;

  if (print_thread_events)
    printf_unfiltered (_("[New %s]\n"), target_pid_to_str (ptid));

  annotate_new_thread ();
  return result;
}

struct thread_info *
add_thread (ptid_t ptid)
{
  return add_thread_with_info (ptid, NULL);
}

/* Delete thread PTID.  If SILENT, don't notify the observer of this
   exit.  */
static void
delete_thread_1 (ptid_t ptid, int silent)
{
  struct thread_info *tp, *tpprev;

  tpprev = NULL;

  for (tp = thread_list; tp; tpprev = tp, tp = tp->next)
    if (ptid_equal (tp->ptid, ptid))
      break;

  if (!tp)
    return;

  /* If this is the current thread, or there's code out there that
     relies on it existing (refcount > 0) we can't delete yet.  Mark
     it as exited, and notify it.  */
  if (tp->refcount > 0
      || ptid_equal (tp->ptid, inferior_ptid))
    {
      if (tp->state_ != THREAD_EXITED)
	{
	  if (!silent)
	    observer_notify_thread_exit (tp);

	  /* Tag it as exited.  */
	  tp->state_ = THREAD_EXITED;

	  /* Clear breakpoints, etc. associated with this thread.  */
	  clear_thread_inferior_resources (tp);
	}

       /* Will be really deleted some other time.  */
       return;
     }

  if (tpprev)
    tpprev->next = tp->next;
  else
    thread_list = tp->next;

  /* Notify thread exit, but only if we haven't already.  */
  if (!silent && tp->state_ != THREAD_EXITED)
    observer_notify_thread_exit (tp);

  free_thread (tp);
}

/* Delete thread PTID and notify of thread exit.  If this is
   inferior_ptid, don't actually delete it, but tag it as exited and
   do the notification.  If PTID is the user selected thread, clear
   it.  */
void
delete_thread (ptid_t ptid)
{
  delete_thread_1 (ptid, 0 /* not silent */);
}

void
delete_thread_silent (ptid_t ptid)
{
  delete_thread_1 (ptid, 1 /* silent */);
}

struct thread_info *
find_thread_id (int num)
{
  struct thread_info *tp;

  for (tp = thread_list; tp; tp = tp->next)
    if (tp->num == num)
      return tp;

  return NULL;
}

/* Find a thread_info by matching PTID.  */
struct thread_info *
find_thread_pid (ptid_t ptid)
{
  struct thread_info *tp;

  for (tp = thread_list; tp; tp = tp->next)
    if (ptid_equal (tp->ptid, ptid))
      return tp;

  return NULL;
}

/*
 * Thread iterator function.
 *
 * Calls a callback function once for each thread, so long as
 * the callback function returns false.  If the callback function
 * returns true, the iteration will end and the current thread
 * will be returned.  This can be useful for implementing a 
 * search for a thread with arbitrary attributes, or for applying
 * some operation to every thread.
 *
 * FIXME: some of the existing functionality, such as 
 * "Thread apply all", might be rewritten using this functionality.
 */

struct thread_info *
iterate_over_threads (int (*callback) (struct thread_info *, void *),
		      void *data)
{
  struct thread_info *tp, *next;

  for (tp = thread_list; tp; tp = next)
    {
      next = tp->next;
      if ((*callback) (tp, data))
	return tp;
    }

  return NULL;
}

int
thread_count (void)
{
  int result = 0;
  struct thread_info *tp;

  for (tp = thread_list; tp; tp = tp->next)
    ++result;

  return result;  
}

int
valid_thread_id (int num)
{
  struct thread_info *tp;

  for (tp = thread_list; tp; tp = tp->next)
    if (tp->num == num)
      return 1;

  return 0;
}

int
pid_to_thread_id (ptid_t ptid)
{
  struct thread_info *tp;

  for (tp = thread_list; tp; tp = tp->next)
    if (ptid_equal (tp->ptid, ptid))
      return tp->num;

  return 0;
}

ptid_t
thread_id_to_pid (int num)
{
  struct thread_info *thread = find_thread_id (num);
  if (thread)
    return thread->ptid;
  else
    return pid_to_ptid (-1);
}

int
in_thread_list (ptid_t ptid)
{
  struct thread_info *tp;

  for (tp = thread_list; tp; tp = tp->next)
    if (ptid_equal (tp->ptid, ptid))
      return 1;

  return 0;			/* Never heard of 'im */
}

/* Print a list of thread ids currently known, and the total number of
   threads. To be used from within catch_errors. */
static int
do_captured_list_thread_ids (struct ui_out *uiout, void *arg)
{
  struct thread_info *tp;
  int num = 0;
  struct cleanup *cleanup_chain;

  prune_threads ();
  target_find_new_threads ();

  cleanup_chain = make_cleanup_ui_out_tuple_begin_end (uiout, "thread-ids");

  for (tp = thread_list; tp; tp = tp->next)
    {
      if (tp->state_ == THREAD_EXITED)
	continue;
      num++;
      ui_out_field_int (uiout, "thread-id", tp->num);
    }

  do_cleanups (cleanup_chain);
  ui_out_field_int (uiout, "number-of-threads", num);
  return GDB_RC_OK;
}

/* Official gdblib interface function to get a list of thread ids and
   the total number. */
enum gdb_rc
gdb_list_thread_ids (struct ui_out *uiout, char **error_message)
{
  if (catch_exceptions_with_msg (uiout, do_captured_list_thread_ids, NULL,
				 error_message, RETURN_MASK_ALL) < 0)
    return GDB_RC_FAIL;
  return GDB_RC_OK;
}

/* Load infrun state for the thread PID.  */

void
load_infrun_state (ptid_t ptid)
{
  struct thread_info *tp;

  /* If we can't find the thread, then we're debugging a single threaded
     process.  No need to do anything in that case.  */
  tp = find_thread_id (pid_to_thread_id (ptid));
  if (tp == NULL)
    return;
}

/* Save infrun state for the thread PID.  */

void
save_infrun_state (ptid_t ptid)
{
  struct thread_info *tp;

  /* If we can't find the thread, then we're debugging a single-threaded
     process.  Nothing to do in that case.  */
  tp = find_thread_id (pid_to_thread_id (ptid));
  if (tp == NULL)
    return;
}

/* Return true if TP is an active thread. */
static int
thread_alive (struct thread_info *tp)
{
  if (tp->state_ == THREAD_EXITED)
    return 0;
  if (!target_thread_alive (tp->ptid))
    return 0;
  return 1;
}

static void
prune_threads (void)
{
  struct thread_info *tp, *next;

  for (tp = thread_list; tp; tp = next)
    {
      next = tp->next;
      if (!thread_alive (tp))
	delete_thread (tp->ptid);
    }
}

void
thread_change_ptid (ptid_t old_ptid, ptid_t new_ptid)
{
  struct thread_info * tp = find_thread_pid (old_ptid);
  tp->ptid = new_ptid;

  observer_notify_thread_ptid_changed (old_ptid, new_ptid);
}

void
set_running (ptid_t ptid, int running)
{
  struct thread_info *tp;

  if (!thread_list)
    {
      /* This is one of the targets that does not add main
	 thread to the thread list.  Just use a single
	 global flag to indicate that a thread is running.  

	 This problem is unique to ST programs.  For MT programs,
	 the main thread is always present in the thread list.  If it's
	 not, the first call to context_switch will mess up GDB internal
	 state.  */
      if (running
 	  && main_thread_state != THREAD_RUNNING
 	  && !suppress_resume_observer)
	observer_notify_target_resumed (ptid);
      main_thread_state = running ? THREAD_RUNNING : THREAD_STOPPED;
      return;
    }

  /* We try not to notify the observer if no thread has actually changed 
     the running state -- merely to reduce the number of messages to 
     frontend.  Frontend is supposed to handle multiple *running just fine.  */
  if (PIDGET (ptid) == -1)
    {
      int any_started = 0;
      for (tp = thread_list; tp; tp = tp->next)
	{
 	  if (tp->state_ == THREAD_EXITED)
  	    continue;
  	  if (running && tp->state_ == THREAD_STOPPED)
  	    any_started = 1;
 	  tp->state_ = running ? THREAD_RUNNING : THREAD_STOPPED;
	}
      if (any_started && !suppress_resume_observer)
	observer_notify_target_resumed (ptid);      
    }
  else
    {
      int started = 0;
      tp = find_thread_pid (ptid);
      gdb_assert (tp);
      gdb_assert (tp->state_ != THREAD_EXITED);
      if (running && tp->state_ == THREAD_STOPPED)
 	started = 1;
      tp->state_ = running ? THREAD_RUNNING : THREAD_STOPPED;
      if (started && !suppress_resume_observer)
  	observer_notify_target_resumed (ptid);
    }
}

static int
is_thread_state (ptid_t ptid, enum thread_state state)
{
  struct thread_info *tp;

  if (!target_has_execution)
    return 0;

  if (!thread_list)
    return main_thread_state == state;

  tp = find_thread_pid (ptid);
  gdb_assert (tp);
  return tp->state_ == state;
}

int
is_stopped (ptid_t ptid)
{
  /* Without execution, this property is always true.  */
  if (!target_has_execution)
    return 1;

  return is_thread_state (ptid, THREAD_STOPPED);
}

int
is_exited (ptid_t ptid)
{
  /* Without execution, this property is always false.  */
  if (!target_has_execution)
    return 0;

  return is_thread_state (ptid, THREAD_EXITED);
}

int
is_running (ptid_t ptid)
{
   /* Without execution, this property is always false.  */
  if (!target_has_execution)
    return 0;

  return is_thread_state (ptid, THREAD_RUNNING);
}

int
any_running (void)
{
  struct thread_info *tp;

  if (!target_has_execution)
    return 0;

  if (!thread_list)
    return main_thread_state == THREAD_RUNNING;

  for (tp = thread_list; tp; tp = tp->next)
    if (tp->state_ == THREAD_RUNNING)
      return 1;

  return 0;
}

int
is_executing (ptid_t ptid)
{
  struct thread_info *tp;

  if (!target_has_execution)
    return 0;

  if (!thread_list)
    return main_thread_executing;

  tp = find_thread_pid (ptid);
  gdb_assert (tp);
  return tp->executing_;
}

void
set_executing (ptid_t ptid, int executing)
{
  struct thread_info *tp;

  if (!thread_list)
    {
      /* This target does not add the main thread to the thread list.
	 Use a global flag to indicate that the thread is
	 executing.  */
      main_thread_executing = executing;
      return;
    }

  if (PIDGET (ptid) == -1)
    {
      for (tp = thread_list; tp; tp = tp->next)
	tp->executing_ = executing;
    }
  else
    {
      tp = find_thread_pid (ptid);
      gdb_assert (tp);
      tp->executing_ = executing;
    }
}

/* Prints the list of threads and their details on UIOUT.
   This is a version of 'info_thread_command' suitable for
   use from MI.  
   If REQUESTED_THREAD is not -1, it's the GDB id of the thread
   that should be printed.  Otherwise, all threads are
   printed.  */
void
print_thread_info (struct ui_out *uiout, int requested_thread)
{
  struct thread_info *tp;
  ptid_t current_ptid;
  struct cleanup *old_chain;
  char *extra_info;
  int current_thread = -1;

  prune_threads ();
  target_find_new_threads ();
  current_ptid = inferior_ptid;

  /* We'll be switching threads temporarily.  */
  old_chain = make_cleanup_restore_current_thread ();

  make_cleanup_ui_out_list_begin_end (uiout, "threads");
  for (tp = thread_list; tp; tp = tp->next)
    {
      struct cleanup *chain2;

      if (requested_thread != -1 && tp->num != requested_thread)
	continue;

      if (ptid_equal (tp->ptid, current_ptid))
	current_thread = tp->num;

      if (tp->state_ == THREAD_EXITED)
	continue;

      chain2 = make_cleanup_ui_out_tuple_begin_end (uiout, NULL);

      if (ptid_equal (tp->ptid, current_ptid))
	ui_out_text (uiout, "* ");
      else
	ui_out_text (uiout, "  ");

      ui_out_field_int (uiout, "id", tp->num);
      ui_out_text (uiout, " ");
      ui_out_field_string (uiout, "target-id", target_tid_to_str (tp->ptid));

      if (tp->state_ != THREAD_EXITED)
	{
	  extra_info = target_extra_thread_info (tp);
	  if (extra_info)
	    {
	      ui_out_text (uiout, " (");
	      ui_out_field_string (uiout, "details", extra_info);
	      ui_out_text (uiout, ")");
	    }
	  ui_out_text (uiout, "  ");
	}

      if (tp->state_ == THREAD_RUNNING)
	ui_out_text (uiout, "(running)\n");
      else
	{
	  /* The switch below puts us at the top of the stack (leaf
	     frame).  */
	  switch_to_thread (tp->ptid);
	  print_stack_frame (get_selected_frame (NULL),
			     /* For MI output, print frame level.  */
			     ui_out_is_mi_like_p (uiout),
			     LOCATION);
	}

      if (ui_out_is_mi_like_p (uiout))
	{
	  char *state = "stopped";
	  if (tp->state_ == THREAD_EXITED)
	    state = "exited";
	  else if (tp->state_ == THREAD_RUNNING)
	    state = "running";
	  ui_out_field_string (uiout, "state", state);
	}

      do_cleanups (chain2);
    }

  /* Restores the current thread and the frame selected before
     the "info threads" command.  */
  do_cleanups (old_chain);

  if (requested_thread == -1)
    {
      gdb_assert (current_thread != -1
		  || !thread_list);
      if (current_thread != -1 && ui_out_is_mi_like_p (uiout))
	ui_out_field_int (uiout, "current-thread-id", current_thread);

      if (current_thread != -1 && is_exited (current_ptid))
	ui_out_message (uiout, 0, "\n\
The current thread <Thread ID %d> has terminated.  See `help thread'.\n",
			current_thread);
    }
}


/* Print information about currently known threads 

 * Note: this has the drawback that it _really_ switches
 *       threads, which frees the frame cache.  A no-side
 *       effects info-threads command would be nicer.
 */

static void
info_threads_command (char *arg, int from_tty)
{
  print_thread_info (uiout, -1);
}

/* Switch from one thread to another. */

void
switch_to_thread (ptid_t ptid)
{
  if (ptid_equal (ptid, inferior_ptid))
    return;

  inferior_ptid = ptid;
  reinit_frame_cache ();
  registers_changed ();

  /* We don't check for is_stopped, because we're called at times
     while in the TARGET_RUNNING state, e.g., while handling an
     internal event.  */
  if (!is_exited (ptid) && !is_executing (ptid))
    stop_pc = read_pc ();
  else
    stop_pc = ~(CORE_ADDR) 0;
}

static void
restore_current_thread (ptid_t ptid)
{
  if (!ptid_equal (ptid, inferior_ptid))
    {
      if (non_stop)
	context_switch_to (ptid);
      else
	switch_to_thread (ptid);
    }
}

static void
restore_selected_frame (struct frame_id a_frame_id, int frame_level)
{
  struct frame_info *frame = NULL;
  int count;

  gdb_assert (frame_level >= 0);

  /* Restore by level first, check if the frame id is the same as
     expected.  If that fails, try restoring by frame id.  If that
     fails, nothing to do, just warn the user.  */

  count = frame_level;
  frame = find_relative_frame (get_current_frame (), &count);
  if (count == 0
      && frame != NULL
      /* Either the frame ids match, of they're both invalid.  The
	 latter case is not failsafe, but since it's highly unlikely
	 the search by level finds the wrong frame, it's 99.9(9)% of
	 the time (for all practical purposes) safe.  */
      && (frame_id_eq (get_frame_id (frame), a_frame_id)
	  /* Note: could be better to check every frame_id
	     member for equality here.  */
	  || (!frame_id_p (get_frame_id (frame))
	      && !frame_id_p (a_frame_id))))
    {
      /* Cool, all is fine.  */
      select_frame (frame);
      return;
    }

  frame = frame_find_by_id (a_frame_id);
  if (frame != NULL)
    {
      /* Cool, refound it.  */
      select_frame (frame);
      return;
    }

  /* Nothing else to do, the frame layout really changed.  Select the
     innermost stack frame.  */
  select_frame (get_current_frame ());

  /* Warn the user.  */
  if (!ui_out_is_mi_like_p (uiout))
    {
      warning (_("\
Couldn't restore frame #%d in current thread, at reparsed frame #0\n"),
	       frame_level);
      /* For MI, we should probably have a notification about
	 current frame change.  But this error is not very
	 likely, so don't bother for now.  */
      print_stack_frame (get_selected_frame (NULL), 1, SRC_LINE);
    }
}

struct current_thread_cleanup
{
  ptid_t inferior_ptid;
  struct frame_id selected_frame_id;
  int selected_frame_level;
  int was_stopped;
};

static void
do_restore_current_thread_cleanup (void *arg)
{
  struct thread_info *tp;
  struct current_thread_cleanup *old = arg;
  restore_current_thread (old->inferior_ptid);

  /* The running state of the originally selected thread may have
     changed, so we have to recheck it here.  */
  if (old->was_stopped
      && is_stopped (inferior_ptid)
      && target_has_registers
      && target_has_stack
      && target_has_memory)
    restore_selected_frame (old->selected_frame_id,
			    old->selected_frame_level);
}

static void
restore_current_thread_cleanup_dtor (void *arg)
{
  struct current_thread_cleanup *old = arg;
  struct thread_info *tp;
  tp = find_thread_pid (old->inferior_ptid);
  if (tp)
    tp->refcount--;
  xfree (old);
}

struct cleanup *
make_cleanup_restore_current_thread (void)
{
  struct thread_info *tp;
  struct frame_info *frame;
  struct current_thread_cleanup *old;

  old = xmalloc (sizeof (struct current_thread_cleanup));
  old->inferior_ptid = inferior_ptid;
  old->was_stopped = is_stopped (inferior_ptid);
  if (old->was_stopped
      && target_has_registers
      && target_has_stack
      && target_has_memory)
    frame = get_selected_frame (NULL);
  else
    frame = NULL;

  old->selected_frame_id = get_frame_id (frame);
  old->selected_frame_level = frame_relative_level (frame);

  tp = find_thread_pid (inferior_ptid);
  if (tp)
    tp->refcount++;

  return make_cleanup_dtor (do_restore_current_thread_cleanup, old,
			    restore_current_thread_cleanup_dtor);
}

/* Apply a GDB command to a list of threads.  List syntax is a whitespace
   seperated list of numbers, or ranges, or the keyword `all'.  Ranges consist
   of two numbers seperated by a hyphen.  Examples:

   thread apply 1 2 7 4 backtrace       Apply backtrace cmd to threads 1,2,7,4
   thread apply 2-7 9 p foo(1)  Apply p foo(1) cmd to threads 2->7 & 9
   thread apply all p x/i $pc   Apply x/i $pc cmd to all threads
 */

static void
thread_apply_all_command (char *cmd, int from_tty)
{
  struct thread_info *tp;
  struct cleanup *old_chain;
  char *saved_cmd;

  if (cmd == NULL || *cmd == '\000')
    error (_("Please specify a command following the thread ID list"));

  prune_threads ();
  target_find_new_threads ();

  old_chain = make_cleanup_restore_current_thread ();

  /* Save a copy of the command in case it is clobbered by
     execute_command */
  saved_cmd = xstrdup (cmd);
  make_cleanup (xfree, saved_cmd);
  for (tp = thread_list; tp; tp = tp->next)
    if (thread_alive (tp))
      {
	if (non_stop)
	  context_switch_to (tp->ptid);
	else
	  switch_to_thread (tp->ptid);

	printf_filtered (_("\nThread %d (%s):\n"),
			 tp->num, target_tid_to_str (inferior_ptid));
	execute_command (cmd, from_tty);
	strcpy (cmd, saved_cmd);	/* Restore exact command used previously */
      }

  do_cleanups (old_chain);
}

static void
thread_apply_command (char *tidlist, int from_tty)
{
  char *cmd;
  char *p;
  struct cleanup *old_chain;
  char *saved_cmd;

  if (tidlist == NULL || *tidlist == '\000')
    error (_("Please specify a thread ID list"));

  for (cmd = tidlist; *cmd != '\000' && !isalpha (*cmd); cmd++);

  if (*cmd == '\000')
    error (_("Please specify a command following the thread ID list"));

  /* Save a copy of the command in case it is clobbered by
     execute_command */
  saved_cmd = xstrdup (cmd);
  old_chain = make_cleanup (xfree, saved_cmd);
  while (tidlist < cmd)
    {
      struct thread_info *tp;
      int start, end;

      start = strtol (tidlist, &p, 10);
      if (p == tidlist)
	error (_("Error parsing %s"), tidlist);
      tidlist = p;

      while (*tidlist == ' ' || *tidlist == '\t')
	tidlist++;

      if (*tidlist == '-')	/* Got a range of IDs? */
	{
	  tidlist++;		/* Skip the - */
	  end = strtol (tidlist, &p, 10);
	  if (p == tidlist)
	    error (_("Error parsing %s"), tidlist);
	  tidlist = p;

	  while (*tidlist == ' ' || *tidlist == '\t')
	    tidlist++;
	}
      else
	end = start;

      make_cleanup_restore_current_thread ();

      for (; start <= end; start++)
	{
	  tp = find_thread_id (start);

	  if (!tp)
	    warning (_("Unknown thread %d."), start);
	  else if (!thread_alive (tp))
	    warning (_("Thread %d has terminated."), start);
	  else
	    {
	      if (non_stop)
		context_switch_to (tp->ptid);
	      else
		switch_to_thread (tp->ptid);

	      printf_filtered (_("\nThread %d (%s):\n"), tp->num,
			       target_tid_to_str (inferior_ptid));
	      execute_command (cmd, from_tty);

	      /* Restore exact command used previously.  */
	      strcpy (cmd, saved_cmd);
	    }
	}
    }

  do_cleanups (old_chain);
}

/* Switch to the specified thread.  Will dispatch off to thread_apply_command
   if prefix of arg is `apply'.  */

static void
thread_command (char *tidstr, int from_tty)
{
  if (!tidstr)
    {
      if (target_has_stack)
	{
	  if (is_exited (inferior_ptid))
	    printf_filtered (_("[Current thread is %d (%s) (exited)]\n"),
			     pid_to_thread_id (inferior_ptid),
			     target_tid_to_str (inferior_ptid));
	  else
	    printf_filtered (_("[Current thread is %d (%s)]\n"),
			     pid_to_thread_id (inferior_ptid),
			     target_tid_to_str (inferior_ptid));
	}
      else
	error (_("No stack."));
      return;
    }

  annotate_thread_changed ();
  gdb_thread_select (uiout, tidstr, NULL);
}

/* Print notices when new threads are attached and detached.  */
int print_thread_events = 1;
static void
show_print_thread_events (struct ui_file *file, int from_tty,
                          struct cmd_list_element *c, const char *value)
{
  fprintf_filtered (file, _("\
Printing of thread events is %s.\n"),
                    value);
}

static int
do_captured_thread_select (struct ui_out *uiout, void *tidstr)
{
  int num;
  struct thread_info *tp;

  num = value_as_long (parse_and_eval (tidstr));

  tp = find_thread_id (num);

  if (!tp)
    error (_("Thread ID %d not known."), num);

  if (!thread_alive (tp))
    error (_("Thread ID %d has terminated."), num);

  if (non_stop)
    context_switch_to (tp->ptid);
  else
    switch_to_thread (tp->ptid);

  ui_out_text (uiout, "[Switching to thread ");
  ui_out_field_int (uiout, "new-thread-id", pid_to_thread_id (inferior_ptid));
  ui_out_text (uiout, " (");
  ui_out_text (uiout, target_tid_to_str (inferior_ptid));
  ui_out_text (uiout, ")]");

  /* Note that we can't reach this with an exited thread, due to the
     thread_alive check above.  */
  if (tp->state_ == THREAD_RUNNING)
    ui_out_text (uiout, "(running)\n");
  else
    print_stack_frame (get_selected_frame (NULL), 1, SRC_AND_LOC);

  /* Since the current thread may have changed, see if there is any
     exited thread we can now delete.  */
  prune_threads ();

  return GDB_RC_OK;
}

enum gdb_rc
gdb_thread_select (struct ui_out *uiout, char *tidstr, char **error_message)
{
  if (catch_exceptions_with_msg (uiout, do_captured_thread_select, tidstr,
				 error_message, RETURN_MASK_ALL) < 0)
    return GDB_RC_FAIL;
  return GDB_RC_OK;
}

/* Commands with a prefix of `thread'.  */
struct cmd_list_element *thread_cmd_list = NULL;

void
_initialize_thread (void)
{
  static struct cmd_list_element *thread_apply_list = NULL;
  struct cmd_list_element *c;

  c = add_info ("threads", info_threads_command,
		_("IDs of currently known threads."));
  set_cmd_no_selected_thread_ok (c);

  c = add_prefix_cmd ("thread", class_run, thread_command, _("\
Use this command to switch between threads.\n\
The new thread ID must be currently known."),
		      &thread_cmd_list, "thread ", 1, &cmdlist);
  set_cmd_no_selected_thread_ok (c);

  c = add_prefix_cmd ("apply", class_run, thread_apply_command,
		      _("Apply a command to a list of threads."),
		      &thread_apply_list, "thread apply ", 1, &thread_cmd_list);
  set_cmd_no_selected_thread_ok (c);

  c = add_cmd ("all", class_run, thread_apply_all_command,
	       _("Apply a command to all threads."), &thread_apply_list);
  set_cmd_no_selected_thread_ok (c);

  if (!xdb_commands)
    add_com_alias ("t", "thread", class_run, 1);

  add_setshow_boolean_cmd ("thread-events", no_class,
         &print_thread_events, _("\
Set printing of thread events (such as thread start and exit)."), _("\
Show printing of thread events (such as thread start and exit)."), NULL,
         NULL,
         show_print_thread_events,
         &setprintlist, &showprintlist);
}
