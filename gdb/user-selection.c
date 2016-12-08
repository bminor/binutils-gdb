#include "defs.h"
#include "user-selection.h"
#include "inferior.h"
#include "gdbthread.h"
#include "observer.h"
#include "gdbcmd.h"

/* The user-visible selection.  */
static user_selection main_user_selection;

/* Knob for user-selection related debug traces.  */
static int debug_user_selection = 0;

/* See user-selection.h.  */

user_selection *
global_user_selection ()
{
  return &main_user_selection;
}

/* See user-selection.h.  */

void
init_global_user_selection ()
{
  /* Fetch the initial inferior, which should have been added by now.  The
     initial inferior is selected on startup.  */
  struct inferior *inf = find_inferior_id (1);

  gdb_assert (inf != nullptr);

  global_user_selection ()->select_inferior (inf, false);
}

/* See user-selection.h.  */

bool
user_selection::select_inferior (struct inferior *inf, bool notify)
{
  const char *debug_prefix = "user_selection::select_thread";

  /* There is always a selected inferior.  */
  gdb_assert (inf != nullptr);

  if (debug_user_selection)
    printf_unfiltered ("%s: num=%d\n", debug_prefix, inf->num);

  /* No-op if this is already the currently selected inferior.  */
  if (inf == m_inferior)
    {
      if (debug_user_selection)
	printf_unfiltered ("%s: already selected inferior", debug_prefix);

      return false;
    }

  /* When we change inferior, thread and frame will change as well.  */
  user_selected_what what = USER_SELECTED_INFERIOR | USER_SELECTED_THREAD | USER_SELECTED_FRAME;

  /* INF becomes selected.  */
  m_inferior = inf;

  /* Clear the thread and frame fields.  */
  if (m_thread != nullptr)
    {
      m_thread->put ();
      m_thread = nullptr;
    }

  m_frame_id = null_frame_id;
  m_frame_level = INVALID_FRAME_LEVEL;

  if (m_inferior->pid != 0)
    {
      /* This inferior is executing, so it should have threads.  Select the first
         one.  */
      m_thread = iterate_over_threads (
	[inf] (struct thread_info *thread, void *) -> int
	  {
	    return inf->pid == ptid_get_pid (thread->ptid);
	  }
      );

      /* We expect this inferior to have at least one thread.  If we didn't
         find it, we have a problem.  */
      gdb_assert (m_thread != nullptr);

      /* Acquire a strong reference, so the thread_info object doesn't get freed
         while it's selected.  */
      m_thread->get ();
    }

  sanity_check ();

  if (notify)
    observer_notify_global_user_selection_changed (this, what);

  return true;
}

/* See user-selection.h.  */

bool
user_selection::select_thread (struct thread_info *thread, bool notify)
{
  const char *debug_prefix = "user_selection::select_thread";

  /* When changing thread, the frame will necessarily change as well.  */
  user_selected_what what = USER_SELECTED_THREAD | USER_SELECTED_FRAME;

  if (debug_user_selection)
    printf_unfiltered ("%s: num=%d, ptid=%s",
		       debug_prefix, thread->global_num,
		       target_pid_to_str (thread->ptid));

  /* No-op if this is already the currently selected thread.  */
  if (thread == m_thread)
    {
      if (debug_user_selection)
	printf_unfiltered ("%s: already selected thread", debug_prefix);

      return false;
    }

  /* Clear the frame fields.  */
  m_frame_id = null_frame_id;
  m_frame_level = INVALID_FRAME_LEVEL;

  /* Release the reference.  */
  if (m_thread != nullptr)
    m_thread->put ();

  m_thread = thread;

  if (m_thread != nullptr)
    {
      /* Acquire a strong reference, so the thread_info object doesn't get freed
         while it's selected.  */
      m_thread->get ();

      /* The inferior of the thread becomes the newly selected inferior, if it's
         not already.  */
      if (m_inferior != thread->inf)
	{
	  m_inferior = thread->inf;

	  what |= USER_SELECTED_INFERIOR;
	}
    }

  sanity_check ();

  if (notify)
    observer_notify_global_user_selection_changed (this, what);

  return true;
}

bool
user_selection::select_frame (struct frame_info *frame, bool notify)
{
  const char *debug_prefix = "user_selection::select_frame";

  /* No-op if this is already the selected frame.  */
  if (frame_id_eq (m_frame_id, get_frame_id (frame))
      && m_frame_level == frame_relative_level (frame))
    return false;

  m_frame_id = get_frame_id (frame);
  m_frame_level = frame_relative_level (frame);

  if (debug_user_selection)
    {
      string_file buf;

      fprint_frame_id (&buf, m_frame_id);
      printf_unfiltered ("%s: Selected frame #%d %s\n", debug_prefix,
			 m_frame_level, buf.c_str ());
    }

  sanity_check ();

  if (notify)
    observer_notify_global_user_selection_changed (this, USER_SELECTED_FRAME);

  return true;
}

/* Do some basic checks to verify that the selection is coherent.  */

void
user_selection::sanity_check () const
{
  /* We always have a current inferior.  */
  gdb_assert (m_inferior != nullptr);

  /* The selected thread must match the selected inferior.  */
  if (m_thread != nullptr)
    gdb_assert (m_thread->inf == m_inferior);

  /* Can't have a current frame without a current thread.  */
  if (m_frame_level != INVALID_FRAME_LEVEL)
   gdb_assert (m_thread != nullptr);
}

/* Apply US to the core of gdb.  This makes the internally selected inferior,
   thread and frame reflect the selection in US.  */

static void
apply_user_selection (user_selection *us)
{
  /* Select inferior.  */
  set_current_inferior (us->inferior ());
  set_current_program_space (us->inferior ()->pspace);

  /* Select thread.  */
  if (us->thread () != nullptr)
    switch_to_thread (us->thread ()->ptid);
  else
    switch_to_thread (null_ptid);

  /* Select frame.  */
  if (us->has_valid_frame ())
    {
      struct frame_info *fi = us->frame ();

      select_frame (fi);
    }
}

/* Try to make the current (as in: where the program is currently stopped) frame
   the selected one.  */

void
user_selection::try_select_current_frame ()
{
  /* This function should only be called when we don't have a selected frame
     yet.  */
  gdb_assert (!has_valid_frame ());

  /* We need to select the relevant inferior/thread internally in order for
     get_current_frame to work.  */
  apply_user_selection (this);

  TRY
    {
      struct frame_info *fi = get_current_frame ();

      m_frame_id = get_frame_id (fi);
      m_frame_level = frame_relative_level (fi);
    }
  CATCH (exception, RETURN_MASK_ALL)
    {
      /* We can't determine the current frame, too bad.  */
    }
  END_CATCH
}

/* See user-selection.h.  */

void
apply_global_user_selection ()
{
  apply_user_selection (global_user_selection ());
}

/* Callback for the new_thread observer.  */

static void
global_user_selection_on_new_thread (struct thread_info *tp)
{
  user_selection *us = global_user_selection ();

  /* If a new thread is created while:

       1. We don't have a currently selected thread,
       2. The inferior of the new thread is the currently selected inferior,

     then we silently make that new thread the selected one.  It covers the case
     of automatically selecting the initial thread when starting an
     inferior.  */

  if (us->thread () == nullptr && tp->inf == us->inferior ())
    us->select_thread (tp, false);
}

/* Callback for the on_exited observer.  */

static void
global_user_selection_on_exited (struct inferior *inferior)
{
  user_selection *us = global_user_selection ();

  /* When an inferior exits and it's the current inferior, it means we have one
     of its thread currently selected.  De-select it.  */

  if (inferior == us->inferior ())
    us->select_thread (NULL, false);
}

/* Callback for the on_target_resumed observer.  */

static void
global_user_selection_on_target_resumed (ptid_t ptid)
{
  user_selection *us = global_user_selection ();

  /* If the selected thread has been resumed, our frame isn't valid anymore.  */
  if (us->thread () != nullptr && ptid_match (us->thread ()->ptid, ptid))
    us->select_frame (NULL, false);
}

/* Implementation of the "maintenance print user-selection" command.  */

static void
maint_print_user_selection (char *cmd, int from_tty)
{
  user_selection *us = global_user_selection ();

  struct inferior *inf = us->inferior ();
  struct thread_info *tp = us->thread ();
  struct frame_id frame_id = us->raw_frame_id ();
  int frame_level = us->raw_frame_level ();

  /* Print inferior.  */
  fprintf_filtered(gdb_stdout, "inferior %p (num=%d)\n", inf, inf->num);

  /* Print thread.  */
  if (tp != nullptr)
    fprintf_filtered (gdb_stdout,
		      "thread %p (gnum=%d, per-inf-num=%d, inf=%p)\n", tp,
		      tp->global_num, tp->per_inf_num, tp->inf);
  else
    fprintf_filtered(gdb_stdout, "thread null\n");

  /* Print frame.  */
  fprint_frame_id (gdb_stdout, frame_id);
  fprintf_filtered (gdb_stdout, ", level=%d\n", frame_level);
}

/* Initialize observer callbacks and commands.  */

void
_initialize_user_selection ()
{
  observer_attach_new_thread (global_user_selection_on_new_thread);
  observer_attach_inferior_exit (global_user_selection_on_exited);
  observer_attach_target_resumed (global_user_selection_on_target_resumed);

  add_setshow_boolean_cmd ("user-selection", class_maintenance,
			   &debug_user_selection, "blah", "blah", "blah", NULL,
			   NULL, &setdebuglist, &showdebuglist);

  add_cmd ("user-selection", class_maintenance, maint_print_user_selection,
	   "foo", &maintenanceprintlist);
}
