/* Target-struct-independent code to start (run) and stop an inferior
   process.

   Copyright (C) 1986, 1987, 1988, 1989, 1990, 1991, 1992, 1993, 1994, 1995,
   1996, 1997, 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007,
   2008 Free Software Foundation, Inc.

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
#include "gdb_string.h"
#include <ctype.h>
#include "symtab.h"
#include "frame.h"
#include "inferior.h"
#include "exceptions.h"
#include "breakpoint.h"
#include "gdb_wait.h"
#include "gdbcore.h"
#include "gdbcmd.h"
#include "cli/cli-script.h"
#include "target.h"
#include "gdbthread.h"
#include "annotate.h"
#include "symfile.h"
#include "top.h"
#include <signal.h>
#include "inf-loop.h"
#include "regcache.h"
#include "value.h"
#include "observer.h"
#include "language.h"
#include "solib.h"
#include "main.h"

#include "gdb_assert.h"
#include "mi/mi-common.h"
#include "event-top.h"

/* Prototypes for local functions */

static void signals_info (char *, int);

static void handle_command (char *, int);

static void sig_print_info (enum target_signal);

static void sig_print_header (void);

static void resume_cleanups (void *);

static int hook_stop_stub (void *);

static int restore_selected_frame (void *);

static void build_infrun (void);

static int follow_fork (void);

static void set_schedlock_func (char *args, int from_tty,
				struct cmd_list_element *c);

struct thread_stepping_state;

static int currently_stepping (struct thread_stepping_state *tss);

static void xdb_handle_command (char *args, int from_tty);

static int prepare_to_proceed (int);

void _initialize_infrun (void);

/* When set, stop the 'step' command if we enter a function which has
   no line number information.  The normal behavior is that we step
   over such function.  */
int step_stop_if_no_debug = 0;
static void
show_step_stop_if_no_debug (struct ui_file *file, int from_tty,
			    struct cmd_list_element *c, const char *value)
{
  fprintf_filtered (file, _("Mode of the step operation is %s.\n"), value);
}

/* In asynchronous mode, but simulating synchronous execution. */

int sync_execution = 0;

/* wait_for_inferior and normal_stop use this to notify the user
   when the inferior stopped in a different thread than it had been
   running in.  */

static ptid_t previous_inferior_ptid;

int debug_displaced = 0;
static void
show_debug_displaced (struct ui_file *file, int from_tty,
		      struct cmd_list_element *c, const char *value)
{
  fprintf_filtered (file, _("Displace stepping debugging is %s.\n"), value);
}

static int debug_infrun = 0;
static void
show_debug_infrun (struct ui_file *file, int from_tty,
		   struct cmd_list_element *c, const char *value)
{
  fprintf_filtered (file, _("Inferior debugging is %s.\n"), value);
}

/* If the program uses ELF-style shared libraries, then calls to
   functions in shared libraries go through stubs, which live in a
   table called the PLT (Procedure Linkage Table).  The first time the
   function is called, the stub sends control to the dynamic linker,
   which looks up the function's real address, patches the stub so
   that future calls will go directly to the function, and then passes
   control to the function.

   If we are stepping at the source level, we don't want to see any of
   this --- we just want to skip over the stub and the dynamic linker.
   The simple approach is to single-step until control leaves the
   dynamic linker.

   However, on some systems (e.g., Red Hat's 5.2 distribution) the
   dynamic linker calls functions in the shared C library, so you
   can't tell from the PC alone whether the dynamic linker is still
   running.  In this case, we use a step-resume breakpoint to get us
   past the dynamic linker, as if we were using "next" to step over a
   function call.

   IN_SOLIB_DYNSYM_RESOLVE_CODE says whether we're in the dynamic
   linker code or not.  Normally, this means we single-step.  However,
   if SKIP_SOLIB_RESOLVER then returns non-zero, then its value is an
   address where we can place a step-resume breakpoint to get past the
   linker's symbol resolution function.

   IN_SOLIB_DYNSYM_RESOLVE_CODE can generally be implemented in a
   pretty portable way, by comparing the PC against the address ranges
   of the dynamic linker's sections.

   SKIP_SOLIB_RESOLVER is generally going to be system-specific, since
   it depends on internal details of the dynamic linker.  It's usually
   not too hard to figure out where to put a breakpoint, but it
   certainly isn't portable.  SKIP_SOLIB_RESOLVER should do plenty of
   sanity checking.  If it can't figure things out, returning zero and
   getting the (possibly confusing) stepping behavior is better than
   signalling an error, which will obscure the change in the
   inferior's state.  */

/* This function returns TRUE if pc is the address of an instruction
   that lies within the dynamic linker (such as the event hook, or the
   dld itself).

   This function must be used only when a dynamic linker event has
   been caught, and the inferior is being stepped out of the hook, or
   undefined results are guaranteed.  */

#ifndef SOLIB_IN_DYNAMIC_LINKER
#define SOLIB_IN_DYNAMIC_LINKER(pid,pc) 0
#endif


/* Convert the #defines into values.  This is temporary until wfi control
   flow is completely sorted out.  */

#ifndef CANNOT_STEP_HW_WATCHPOINTS
#define CANNOT_STEP_HW_WATCHPOINTS 0
#else
#undef  CANNOT_STEP_HW_WATCHPOINTS
#define CANNOT_STEP_HW_WATCHPOINTS 1
#endif

/* Tables of how to react to signals; the user sets them.  */

static unsigned char *signal_stop;
static unsigned char *signal_print;
static unsigned char *signal_program;

#define SET_SIGS(nsigs,sigs,flags) \
  do { \
    int signum = (nsigs); \
    while (signum-- > 0) \
      if ((sigs)[signum]) \
	(flags)[signum] = 1; \
  } while (0)

#define UNSET_SIGS(nsigs,sigs,flags) \
  do { \
    int signum = (nsigs); \
    while (signum-- > 0) \
      if ((sigs)[signum]) \
	(flags)[signum] = 0; \
  } while (0)

/* Value to pass to target_resume() to cause all threads to resume */

#define RESUME_ALL (pid_to_ptid (-1))

/* Command list pointer for the "stop" placeholder.  */

static struct cmd_list_element *stop_command;

/* Function inferior was in as of last step command.  */

static struct symbol *step_start_function;

/* Nonzero if we are presently stepping over a breakpoint.

   If we hit a breakpoint or watchpoint, and then continue,
   we need to single step the current thread with breakpoints
   disabled, to avoid hitting the same breakpoint or
   watchpoint again.  And we should step just a single
   thread and keep other threads stopped, so that 
   other threads don't miss breakpoints while they are removed.

   So, this variable simultaneously means that we need to single
   step the current thread, keep other threads stopped, and that
   breakpoints should be removed while we step.

   This variable is set either:
   - in proceed, when we resume inferior on user's explicit request
   - in keep_going, if handle_inferior_event decides we need to
   step over breakpoint.  

   The variable is cleared in clear_proceed_status, called every
   time before we call proceed.  The proceed calls wait_for_inferior,
   which calls handle_inferior_event in a loop, and until 
   wait_for_inferior exits, this variable is changed only by keep_going.  */

static int stepping_over_breakpoint;

/* Nonzero if we want to give control to the user when we're notified
   of shared library events by the dynamic linker.  */
static int stop_on_solib_events;
static void
show_stop_on_solib_events (struct ui_file *file, int from_tty,
			   struct cmd_list_element *c, const char *value)
{
  fprintf_filtered (file, _("Stopping for shared library events is %s.\n"),
		    value);
}

/* Nonzero means expecting a trace trap
   and should stop the inferior and return silently when it happens.  */

int stop_after_trap;

/* Nonzero means expecting a trap and caller will handle it themselves.
   It is used after attach, due to attaching to a process;
   when running in the shell before the child program has been exec'd;
   and when running some kinds of remote stuff (FIXME?).  */

enum stop_kind stop_soon;

/* Nonzero if proceed is being used for a "finish" command or a similar
   situation when stop_registers should be saved.  */

int proceed_to_finish;

/* Save register contents here when about to pop a stack dummy frame,
   if-and-only-if proceed_to_finish is set.
   Thus this contains the return value from the called function (assuming
   values are returned in a register).  */

struct regcache *stop_registers;

/* Nonzero after stop if current stack frame should be printed.  */

static int stop_print_frame;

/* Step-resume or longjmp-resume breakpoint.  */
static struct breakpoint *step_resume_breakpoint = NULL;

/* This is a cached copy of the pid/waitstatus of the last event
   returned by target_wait()/deprecated_target_wait_hook().  This
   information is returned by get_last_target_status().  */
static ptid_t target_last_wait_ptid;
static struct target_waitstatus target_last_waitstatus;

/* Context-switchable data.  */
struct thread_stepping_state
{
  /* Should we step over breakpoint next time keep_going
     is called?  */
  int stepping_over_breakpoint;
  int current_line;
  struct symtab *current_symtab;
  int step_after_step_resume_breakpoint;
  int stepping_through_solib_after_catch;
  bpstat stepping_through_solib_catchpoints;
};

struct thread_stepping_state gtss;
struct thread_stepping_state *tss = &gtss;

static void context_switch (ptid_t ptid);

void init_thread_stepping_state (struct thread_stepping_state *tss);

void init_infwait_state (void);

/* This is used to remember when a fork, vfork or exec event
   was caught by a catchpoint, and thus the event is to be
   followed at the next resume of the inferior, and not
   immediately. */
static struct
{
  enum target_waitkind kind;
  struct
  {
    ptid_t parent_pid;
    ptid_t child_pid;
  }
  fork_event;
  char *execd_pathname;
}
pending_follow;

static const char follow_fork_mode_child[] = "child";
static const char follow_fork_mode_parent[] = "parent";

static const char *follow_fork_mode_kind_names[] = {
  follow_fork_mode_child,
  follow_fork_mode_parent,
  NULL
};

static const char *follow_fork_mode_string = follow_fork_mode_parent;
static void
show_follow_fork_mode_string (struct ui_file *file, int from_tty,
			      struct cmd_list_element *c, const char *value)
{
  fprintf_filtered (file, _("\
Debugger response to a program call of fork or vfork is \"%s\".\n"),
		    value);
}


static int
follow_fork (void)
{
  int follow_child = (follow_fork_mode_string == follow_fork_mode_child);

  return target_follow_fork (follow_child);
}

void
follow_inferior_reset_breakpoints (void)
{
  /* Was there a step_resume breakpoint?  (There was if the user
     did a "next" at the fork() call.)  If so, explicitly reset its
     thread number.

     step_resumes are a form of bp that are made to be per-thread.
     Since we created the step_resume bp when the parent process
     was being debugged, and now are switching to the child process,
     from the breakpoint package's viewpoint, that's a switch of
     "threads".  We must update the bp's notion of which thread
     it is for, or it'll be ignored when it triggers.  */

  if (step_resume_breakpoint)
    breakpoint_re_set_thread (step_resume_breakpoint);

  /* Reinsert all breakpoints in the child.  The user may have set
     breakpoints after catching the fork, in which case those
     were never set in the child, but only in the parent.  This makes
     sure the inserted breakpoints match the breakpoint list.  */

  breakpoint_re_set ();
  insert_breakpoints ();
}

/* EXECD_PATHNAME is assumed to be non-NULL. */

static void
follow_exec (ptid_t pid, char *execd_pathname)
{
  ptid_t saved_pid = pid;
  struct target_ops *tgt;

  /* This is an exec event that we actually wish to pay attention to.
     Refresh our symbol table to the newly exec'd program, remove any
     momentary bp's, etc.

     If there are breakpoints, they aren't really inserted now,
     since the exec() transformed our inferior into a fresh set
     of instructions.

     We want to preserve symbolic breakpoints on the list, since
     we have hopes that they can be reset after the new a.out's
     symbol table is read.

     However, any "raw" breakpoints must be removed from the list
     (e.g., the solib bp's), since their address is probably invalid
     now.

     And, we DON'T want to call delete_breakpoints() here, since
     that may write the bp's "shadow contents" (the instruction
     value that was overwritten witha TRAP instruction).  Since
     we now have a new a.out, those shadow contents aren't valid. */
  update_breakpoints_after_exec ();

  /* If there was one, it's gone now.  We cannot truly step-to-next
     statement through an exec(). */
  step_resume_breakpoint = NULL;
  step_range_start = 0;
  step_range_end = 0;

  /* What is this a.out's name? */
  printf_unfiltered (_("Executing new program: %s\n"), execd_pathname);

  /* We've followed the inferior through an exec.  Therefore, the
     inferior has essentially been killed & reborn. */

  gdb_flush (gdb_stdout);
  generic_mourn_inferior ();
  /* Because mourn_inferior resets inferior_ptid. */
  inferior_ptid = saved_pid;

  if (gdb_sysroot && *gdb_sysroot)
    {
      char *name = alloca (strlen (gdb_sysroot)
			    + strlen (execd_pathname)
			    + 1);
      strcpy (name, gdb_sysroot);
      strcat (name, execd_pathname);
      execd_pathname = name;
    }

  /* That a.out is now the one to use. */
  exec_file_attach (execd_pathname, 0);

  /* Reset the shared library package.  This ensures that we get a
     shlib event when the child reaches "_start", at which point the
     dld will have had a chance to initialize the child.  */
  /* Also, loading a symbol file below may trigger symbol lookups, and
     we don't want those to be satisfied by the libraries of the
     previous incarnation of this process.  */
  no_shared_libraries (NULL, 0);

  /* Load the main file's symbols.  */
  symbol_file_add_main (execd_pathname, 0);

#ifdef SOLIB_CREATE_INFERIOR_HOOK
  SOLIB_CREATE_INFERIOR_HOOK (PIDGET (inferior_ptid));
#else
  solib_create_inferior_hook ();
#endif

  /* Reinsert all breakpoints.  (Those which were symbolic have
     been reset to the proper address in the new a.out, thanks
     to symbol_file_command...) */
  insert_breakpoints ();

  /* The next resume of this inferior should bring it to the shlib
     startup breakpoints.  (If the user had also set bp's on
     "main" from the old (parent) process, then they'll auto-
     matically get reset there in the new process.) */
}

/* Non-zero if we just simulating a single-step.  This is needed
   because we cannot remove the breakpoints in the inferior process
   until after the `wait' in `wait_for_inferior'.  */
static int singlestep_breakpoints_inserted_p = 0;

/* The thread we inserted single-step breakpoints for.  */
static ptid_t singlestep_ptid;

/* PC when we started this single-step.  */
static CORE_ADDR singlestep_pc;

/* If another thread hit the singlestep breakpoint, we save the original
   thread here so that we can resume single-stepping it later.  */
static ptid_t saved_singlestep_ptid;
static int stepping_past_singlestep_breakpoint;

/* If not equal to null_ptid, this means that after stepping over breakpoint
   is finished, we need to switch to deferred_step_ptid, and step it.

   The use case is when one thread has hit a breakpoint, and then the user 
   has switched to another thread and issued 'step'. We need to step over
   breakpoint in the thread which hit the breakpoint, but then continue
   stepping the thread user has selected.  */
static ptid_t deferred_step_ptid;

/* Displaced stepping.  */

/* In non-stop debugging mode, we must take special care to manage
   breakpoints properly; in particular, the traditional strategy for
   stepping a thread past a breakpoint it has hit is unsuitable.
   'Displaced stepping' is a tactic for stepping one thread past a
   breakpoint it has hit while ensuring that other threads running
   concurrently will hit the breakpoint as they should.

   The traditional way to step a thread T off a breakpoint in a
   multi-threaded program in all-stop mode is as follows:

   a0) Initially, all threads are stopped, and breakpoints are not
       inserted.
   a1) We single-step T, leaving breakpoints uninserted.
   a2) We insert breakpoints, and resume all threads.

   In non-stop debugging, however, this strategy is unsuitable: we
   don't want to have to stop all threads in the system in order to
   continue or step T past a breakpoint.  Instead, we use displaced
   stepping:

   n0) Initially, T is stopped, other threads are running, and
       breakpoints are inserted.
   n1) We copy the instruction "under" the breakpoint to a separate
       location, outside the main code stream, making any adjustments
       to the instruction, register, and memory state as directed by
       T's architecture.
   n2) We single-step T over the instruction at its new location.
   n3) We adjust the resulting register and memory state as directed
       by T's architecture.  This includes resetting T's PC to point
       back into the main instruction stream.
   n4) We resume T.

   This approach depends on the following gdbarch methods:

   - gdbarch_max_insn_length and gdbarch_displaced_step_location
     indicate where to copy the instruction, and how much space must
     be reserved there.  We use these in step n1.

   - gdbarch_displaced_step_copy_insn copies a instruction to a new
     address, and makes any necessary adjustments to the instruction,
     register contents, and memory.  We use this in step n1.

   - gdbarch_displaced_step_fixup adjusts registers and memory after
     we have successfuly single-stepped the instruction, to yield the
     same effect the instruction would have had if we had executed it
     at its original address.  We use this in step n3.

   - gdbarch_displaced_step_free_closure provides cleanup.

   The gdbarch_displaced_step_copy_insn and
   gdbarch_displaced_step_fixup functions must be written so that
   copying an instruction with gdbarch_displaced_step_copy_insn,
   single-stepping across the copied instruction, and then applying
   gdbarch_displaced_insn_fixup should have the same effects on the
   thread's memory and registers as stepping the instruction in place
   would have.  Exactly which responsibilities fall to the copy and
   which fall to the fixup is up to the author of those functions.

   See the comments in gdbarch.sh for details.

   Note that displaced stepping and software single-step cannot
   currently be used in combination, although with some care I think
   they could be made to.  Software single-step works by placing
   breakpoints on all possible subsequent instructions; if the
   displaced instruction is a PC-relative jump, those breakpoints
   could fall in very strange places --- on pages that aren't
   executable, or at addresses that are not proper instruction
   boundaries.  (We do generally let other threads run while we wait
   to hit the software single-step breakpoint, and they might
   encounter such a corrupted instruction.)  One way to work around
   this would be to have gdbarch_displaced_step_copy_insn fully
   simulate the effect of PC-relative instructions (and return NULL)
   on architectures that use software single-stepping.

   In non-stop mode, we can have independent and simultaneous step
   requests, so more than one thread may need to simultaneously step
   over a breakpoint.  The current implementation assumes there is
   only one scratch space per process.  In this case, we have to
   serialize access to the scratch space.  If thread A wants to step
   over a breakpoint, but we are currently waiting for some other
   thread to complete a displaced step, we leave thread A stopped and
   place it in the displaced_step_request_queue.  Whenever a displaced
   step finishes, we pick the next thread in the queue and start a new
   displaced step operation on it.  See displaced_step_prepare and
   displaced_step_fixup for details.  */

/* If this is not null_ptid, this is the thread carrying out a
   displaced single-step.  This thread's state will require fixing up
   once it has completed its step.  */
static ptid_t displaced_step_ptid;

struct displaced_step_request
{
  ptid_t ptid;
  struct displaced_step_request *next;
};

/* A queue of pending displaced stepping requests.  */
struct displaced_step_request *displaced_step_request_queue;

/* The architecture the thread had when we stepped it.  */
static struct gdbarch *displaced_step_gdbarch;

/* The closure provided gdbarch_displaced_step_copy_insn, to be used
   for post-step cleanup.  */
static struct displaced_step_closure *displaced_step_closure;

/* The address of the original instruction, and the copy we made.  */
static CORE_ADDR displaced_step_original, displaced_step_copy;

/* Saved contents of copy area.  */
static gdb_byte *displaced_step_saved_copy;

/* When this is non-zero, we are allowed to use displaced stepping, if
   the architecture supports it.  When this is zero, we use
   traditional the hold-and-step approach.  */
int can_use_displaced_stepping = 1;
static void
show_can_use_displaced_stepping (struct ui_file *file, int from_tty,
				 struct cmd_list_element *c,
				 const char *value)
{
  fprintf_filtered (file, _("\
Debugger's willingness to use displaced stepping to step over "
"breakpoints is %s.\n"), value);
}

/* Return non-zero if displaced stepping is enabled, and can be used
   with GDBARCH.  */
static int
use_displaced_stepping (struct gdbarch *gdbarch)
{
  return (can_use_displaced_stepping
	  && gdbarch_displaced_step_copy_insn_p (gdbarch));
}

/* Clean out any stray displaced stepping state.  */
static void
displaced_step_clear (void)
{
  /* Indicate that there is no cleanup pending.  */
  displaced_step_ptid = null_ptid;

  if (displaced_step_closure)
    {
      gdbarch_displaced_step_free_closure (displaced_step_gdbarch,
                                           displaced_step_closure);
      displaced_step_closure = NULL;
    }
}

static void
cleanup_displaced_step_closure (void *ptr)
{
  struct displaced_step_closure *closure = ptr;

  gdbarch_displaced_step_free_closure (current_gdbarch, closure);
}

/* Dump LEN bytes at BUF in hex to FILE, followed by a newline.  */
void
displaced_step_dump_bytes (struct ui_file *file,
                           const gdb_byte *buf,
                           size_t len)
{
  int i;

  for (i = 0; i < len; i++)
    fprintf_unfiltered (file, "%02x ", buf[i]);
  fputs_unfiltered ("\n", file);
}

/* Prepare to single-step, using displaced stepping.

   Note that we cannot use displaced stepping when we have a signal to
   deliver.  If we have a signal to deliver and an instruction to step
   over, then after the step, there will be no indication from the
   target whether the thread entered a signal handler or ignored the
   signal and stepped over the instruction successfully --- both cases
   result in a simple SIGTRAP.  In the first case we mustn't do a
   fixup, and in the second case we must --- but we can't tell which.
   Comments in the code for 'random signals' in handle_inferior_event
   explain how we handle this case instead.

   Returns 1 if preparing was successful -- this thread is going to be
   stepped now; or 0 if displaced stepping this thread got queued.  */
static int
displaced_step_prepare (ptid_t ptid)
{
  struct cleanup *old_cleanups;
  struct regcache *regcache = get_thread_regcache (ptid);
  struct gdbarch *gdbarch = get_regcache_arch (regcache);
  CORE_ADDR original, copy;
  ULONGEST len;
  struct displaced_step_closure *closure;

  /* We should never reach this function if the architecture does not
     support displaced stepping.  */
  gdb_assert (gdbarch_displaced_step_copy_insn_p (gdbarch));

  /* For the first cut, we're displaced stepping one thread at a
     time.  */

  if (!ptid_equal (displaced_step_ptid, null_ptid))
    {
      /* Already waiting for a displaced step to finish.  Defer this
	 request and place in queue.  */
      struct displaced_step_request *req, *new_req;

      if (debug_displaced)
	fprintf_unfiltered (gdb_stdlog,
			    "displaced: defering step of %s\n",
			    target_pid_to_str (ptid));

      new_req = xmalloc (sizeof (*new_req));
      new_req->ptid = ptid;
      new_req->next = NULL;

      if (displaced_step_request_queue)
	{
	  for (req = displaced_step_request_queue;
	       req && req->next;
	       req = req->next)
	    ;
	  req->next = new_req;
	}
      else
	displaced_step_request_queue = new_req;

      return 0;
    }
  else
    {
      if (debug_displaced)
	fprintf_unfiltered (gdb_stdlog,
			    "displaced: stepping %s now\n",
			    target_pid_to_str (ptid));
    }

  displaced_step_clear ();

  original = regcache_read_pc (regcache);

  copy = gdbarch_displaced_step_location (gdbarch);
  len = gdbarch_max_insn_length (gdbarch);

  /* Save the original contents of the copy area.  */
  displaced_step_saved_copy = xmalloc (len);
  old_cleanups = make_cleanup (free_current_contents,
                               &displaced_step_saved_copy);
  read_memory (copy, displaced_step_saved_copy, len);
  if (debug_displaced)
    {
      fprintf_unfiltered (gdb_stdlog, "displaced: saved 0x%s: ",
			  paddr_nz (copy));
      displaced_step_dump_bytes (gdb_stdlog, displaced_step_saved_copy, len);
    };

  closure = gdbarch_displaced_step_copy_insn (gdbarch,
                                              original, copy, regcache);

  /* We don't support the fully-simulated case at present.  */
  gdb_assert (closure);

  make_cleanup (cleanup_displaced_step_closure, closure);

  /* Resume execution at the copy.  */
  regcache_write_pc (regcache, copy);

  discard_cleanups (old_cleanups);

  if (debug_displaced)
    fprintf_unfiltered (gdb_stdlog, "displaced: displaced pc to 0x%s\n",
                        paddr_nz (copy));

  /* Save the information we need to fix things up if the step
     succeeds.  */
  displaced_step_ptid = ptid;
  displaced_step_gdbarch = gdbarch;
  displaced_step_closure = closure;
  displaced_step_original = original;
  displaced_step_copy = copy;
  return 1;
}

static void
displaced_step_clear_cleanup (void *ignore)
{
  displaced_step_clear ();
}

static void
write_memory_ptid (ptid_t ptid, CORE_ADDR memaddr, const gdb_byte *myaddr, int len)
{
  struct cleanup *ptid_cleanup = save_inferior_ptid ();
  inferior_ptid = ptid;
  write_memory (memaddr, myaddr, len);
  do_cleanups (ptid_cleanup);
}

static void
displaced_step_fixup (ptid_t event_ptid, enum target_signal signal)
{
  struct cleanup *old_cleanups;

  /* Was this event for the pid we displaced?  */
  if (ptid_equal (displaced_step_ptid, null_ptid)
      || ! ptid_equal (displaced_step_ptid, event_ptid))
    return;

  old_cleanups = make_cleanup (displaced_step_clear_cleanup, 0);

  /* Restore the contents of the copy area.  */
  {
    ULONGEST len = gdbarch_max_insn_length (displaced_step_gdbarch);
    write_memory_ptid (displaced_step_ptid, displaced_step_copy,
		       displaced_step_saved_copy, len);
    if (debug_displaced)
      fprintf_unfiltered (gdb_stdlog, "displaced: restored 0x%s\n",
                          paddr_nz (displaced_step_copy));
  }

  /* Did the instruction complete successfully?  */
  if (signal == TARGET_SIGNAL_TRAP)
    {
      /* Fix up the resulting state.  */
      gdbarch_displaced_step_fixup (displaced_step_gdbarch,
                                    displaced_step_closure,
                                    displaced_step_original,
                                    displaced_step_copy,
                                    get_thread_regcache (displaced_step_ptid));
    }
  else
    {
      /* Since the instruction didn't complete, all we can do is
         relocate the PC.  */
      struct regcache *regcache = get_thread_regcache (event_ptid);
      CORE_ADDR pc = regcache_read_pc (regcache);
      pc = displaced_step_original + (pc - displaced_step_copy);
      regcache_write_pc (regcache, pc);
    }

  do_cleanups (old_cleanups);

  /* Are there any pending displaced stepping requests?  If so, run
     one now.  */
  if (displaced_step_request_queue)
    {
      struct displaced_step_request *head;
      ptid_t ptid;

      head = displaced_step_request_queue;
      ptid = head->ptid;
      displaced_step_request_queue = head->next;
      xfree (head);

      if (debug_displaced)
	fprintf_unfiltered (gdb_stdlog,
			    "displaced: stepping queued %s now\n",
			    target_pid_to_str (ptid));


      displaced_step_ptid = null_ptid;
      displaced_step_prepare (ptid);
      target_resume (ptid, 1, TARGET_SIGNAL_0);
    }
}


/* Resuming.  */

/* Things to clean up if we QUIT out of resume ().  */
static void
resume_cleanups (void *ignore)
{
  normal_stop ();
}

static const char schedlock_off[] = "off";
static const char schedlock_on[] = "on";
static const char schedlock_step[] = "step";
static const char *scheduler_enums[] = {
  schedlock_off,
  schedlock_on,
  schedlock_step,
  NULL
};
static const char *scheduler_mode = schedlock_off;
static void
show_scheduler_mode (struct ui_file *file, int from_tty,
		     struct cmd_list_element *c, const char *value)
{
  fprintf_filtered (file, _("\
Mode for locking scheduler during execution is \"%s\".\n"),
		    value);
}

static void
set_schedlock_func (char *args, int from_tty, struct cmd_list_element *c)
{
  if (!target_can_lock_scheduler)
    {
      scheduler_mode = schedlock_off;
      error (_("Target '%s' cannot support this command."), target_shortname);
    }
}


/* Resume the inferior, but allow a QUIT.  This is useful if the user
   wants to interrupt some lengthy single-stepping operation
   (for child processes, the SIGINT goes to the inferior, and so
   we get a SIGINT random_signal, but for remote debugging and perhaps
   other targets, that's not true).

   STEP nonzero if we should step (zero to continue instead).
   SIG is the signal to give the inferior (zero for none).  */
void
resume (int step, enum target_signal sig)
{
  int should_resume = 1;
  struct cleanup *old_cleanups = make_cleanup (resume_cleanups, 0);
  struct regcache *regcache = get_current_regcache ();
  struct gdbarch *gdbarch = get_regcache_arch (regcache);
  CORE_ADDR pc = regcache_read_pc (regcache);
  QUIT;

  if (debug_infrun)
    fprintf_unfiltered (gdb_stdlog,
                        "infrun: resume (step=%d, signal=%d), "
                        "stepping_over_breakpoint=%d\n",
			step, sig, stepping_over_breakpoint);

  /* Some targets (e.g. Solaris x86) have a kernel bug when stepping
     over an instruction that causes a page fault without triggering
     a hardware watchpoint. The kernel properly notices that it shouldn't
     stop, because the hardware watchpoint is not triggered, but it forgets
     the step request and continues the program normally.
     Work around the problem by removing hardware watchpoints if a step is
     requested, GDB will check for a hardware watchpoint trigger after the
     step anyway.  */
  if (CANNOT_STEP_HW_WATCHPOINTS && step)
    remove_hw_watchpoints ();


  /* Normally, by the time we reach `resume', the breakpoints are either
     removed or inserted, as appropriate.  The exception is if we're sitting
     at a permanent breakpoint; we need to step over it, but permanent
     breakpoints can't be removed.  So we have to test for it here.  */
  if (breakpoint_here_p (pc) == permanent_breakpoint_here)
    {
      if (gdbarch_skip_permanent_breakpoint_p (gdbarch))
	gdbarch_skip_permanent_breakpoint (gdbarch, regcache);
      else
	error (_("\
The program is stopped at a permanent breakpoint, but GDB does not know\n\
how to step past a permanent breakpoint on this architecture.  Try using\n\
a command like `return' or `jump' to continue execution."));
    }

  /* If enabled, step over breakpoints by executing a copy of the
     instruction at a different address.

     We can't use displaced stepping when we have a signal to deliver;
     the comments for displaced_step_prepare explain why.  The
     comments in the handle_inferior event for dealing with 'random
     signals' explain what we do instead.  */
  if (use_displaced_stepping (gdbarch)
      && stepping_over_breakpoint
      && sig == TARGET_SIGNAL_0)
    {
      if (!displaced_step_prepare (inferior_ptid))
	{
	  /* Got placed in displaced stepping queue.  Will be resumed
	     later when all the currently queued displaced stepping
	     requests finish.  */
	  discard_cleanups (old_cleanups);
	  return;
	}
    }

  if (step && gdbarch_software_single_step_p (gdbarch))
    {
      /* Do it the hard way, w/temp breakpoints */
      if (gdbarch_software_single_step (gdbarch, get_current_frame ()))
        {
          /* ...and don't ask hardware to do it.  */
          step = 0;
          /* and do not pull these breakpoints until after a `wait' in
          `wait_for_inferior' */
          singlestep_breakpoints_inserted_p = 1;
          singlestep_ptid = inferior_ptid;
          singlestep_pc = pc;
        }
    }

  /* If there were any forks/vforks/execs that were caught and are
     now to be followed, then do so.  */
  switch (pending_follow.kind)
    {
    case TARGET_WAITKIND_FORKED:
    case TARGET_WAITKIND_VFORKED:
      pending_follow.kind = TARGET_WAITKIND_SPURIOUS;
      if (follow_fork ())
	should_resume = 0;
      break;

    case TARGET_WAITKIND_EXECD:
      /* follow_exec is called as soon as the exec event is seen. */
      pending_follow.kind = TARGET_WAITKIND_SPURIOUS;
      break;

    default:
      break;
    }

  /* Install inferior's terminal modes.  */
  target_terminal_inferior ();

  if (should_resume)
    {
      ptid_t resume_ptid;

      resume_ptid = RESUME_ALL;	/* Default */

      /* If STEP is set, it's a request to use hardware stepping
	 facilities.  But in that case, we should never
	 use singlestep breakpoint.  */
      gdb_assert (!(singlestep_breakpoints_inserted_p && step));

      if (singlestep_breakpoints_inserted_p
	  && stepping_past_singlestep_breakpoint)
	{
	  /* The situation here is as follows.  In thread T1 we wanted to
	     single-step.  Lacking hardware single-stepping we've
	     set breakpoint at the PC of the next instruction -- call it
	     P.  After resuming, we've hit that breakpoint in thread T2.
	     Now we've removed original breakpoint, inserted breakpoint
	     at P+1, and try to step to advance T2 past breakpoint.
	     We need to step only T2, as if T1 is allowed to freely run,
	     it can run past P, and if other threads are allowed to run,
	     they can hit breakpoint at P+1, and nested hits of single-step
	     breakpoints is not something we'd want -- that's complicated
	     to support, and has no value.  */
	  resume_ptid = inferior_ptid;
	}

      if ((step || singlestep_breakpoints_inserted_p)
	  && stepping_over_breakpoint)
	{
	  /* We're allowing a thread to run past a breakpoint it has
	     hit, by single-stepping the thread with the breakpoint
	     removed.  In which case, we need to single-step only this
	     thread, and keep others stopped, as they can miss this
	     breakpoint if allowed to run.

	     The current code actually removes all breakpoints when
	     doing this, not just the one being stepped over, so if we
	     let other threads run, we can actually miss any
	     breakpoint, not just the one at PC.  */
	  resume_ptid = inferior_ptid;
	}

      if (non_stop)
	{
	  /* With non-stop mode on, threads are always handled
	     individually.  */
	  resume_ptid = inferior_ptid;
	}
      else if ((scheduler_mode == schedlock_on)
	       || (scheduler_mode == schedlock_step
		   && (step || singlestep_breakpoints_inserted_p)))
	{
	  /* User-settable 'scheduler' mode requires solo thread resume. */
	  resume_ptid = inferior_ptid;
	}

      if (gdbarch_cannot_step_breakpoint (gdbarch))
	{
	  /* Most targets can step a breakpoint instruction, thus
	     executing it normally.  But if this one cannot, just
	     continue and we will hit it anyway.  */
	  if (step && breakpoint_inserted_here_p (pc))
	    step = 0;
	}

      if (debug_displaced
          && use_displaced_stepping (gdbarch)
          && stepping_over_breakpoint)
        {
	  struct regcache *resume_regcache = get_thread_regcache (resume_ptid);
          CORE_ADDR actual_pc = regcache_read_pc (resume_regcache);
          gdb_byte buf[4];

          fprintf_unfiltered (gdb_stdlog, "displaced: run 0x%s: ",
                              paddr_nz (actual_pc));
          read_memory (actual_pc, buf, sizeof (buf));
          displaced_step_dump_bytes (gdb_stdlog, buf, sizeof (buf));
        }

      target_resume (resume_ptid, step, sig);
    }

  discard_cleanups (old_cleanups);
}

/* Proceeding.  */

/* Clear out all variables saying what to do when inferior is continued.
   First do this, then set the ones you want, then call `proceed'.  */

void
clear_proceed_status (void)
{
  stepping_over_breakpoint = 0;
  step_range_start = 0;
  step_range_end = 0;
  step_frame_id = null_frame_id;
  step_over_calls = STEP_OVER_UNDEBUGGABLE;
  stop_after_trap = 0;
  stop_soon = NO_STOP_QUIETLY;
  proceed_to_finish = 0;
  breakpoint_proceeded = 1;	/* We're about to proceed... */

  if (stop_registers)
    {
      regcache_xfree (stop_registers);
      stop_registers = NULL;
    }

  /* Discard any remaining commands or status from previous stop.  */
  bpstat_clear (&stop_bpstat);
}

/* This should be suitable for any targets that support threads. */

static int
prepare_to_proceed (int step)
{
  ptid_t wait_ptid;
  struct target_waitstatus wait_status;

  /* Get the last target status returned by target_wait().  */
  get_last_target_status (&wait_ptid, &wait_status);

  /* Make sure we were stopped at a breakpoint.  */
  if (wait_status.kind != TARGET_WAITKIND_STOPPED
      || wait_status.value.sig != TARGET_SIGNAL_TRAP)
    {
      return 0;
    }

  /* Switched over from WAIT_PID.  */
  if (!ptid_equal (wait_ptid, minus_one_ptid)
      && !ptid_equal (inferior_ptid, wait_ptid))
    {
      struct regcache *regcache = get_thread_regcache (wait_ptid);

      if (breakpoint_here_p (regcache_read_pc (regcache)))
	{
	  /* If stepping, remember current thread to switch back to.  */
	  if (step)
	    deferred_step_ptid = inferior_ptid;

	  /* Switch back to WAIT_PID thread.  */
	  switch_to_thread (wait_ptid);

	  /* We return 1 to indicate that there is a breakpoint here,
	     so we need to step over it before continuing to avoid
	     hitting it straight away. */
	  return 1;
	}
    }

  return 0;
}

/* Record the pc of the program the last time it stopped.  This is
   just used internally by wait_for_inferior, but need to be preserved
   over calls to it and cleared when the inferior is started.  */
static CORE_ADDR prev_pc;

/* Basic routine for continuing the program in various fashions.

   ADDR is the address to resume at, or -1 for resume where stopped.
   SIGGNAL is the signal to give it, or 0 for none,
   or -1 for act according to how it stopped.
   STEP is nonzero if should trap after one instruction.
   -1 means return after that and print nothing.
   You should probably set various step_... variables
   before calling here, if you are stepping.

   You should call clear_proceed_status before calling proceed.  */

void
proceed (CORE_ADDR addr, enum target_signal siggnal, int step)
{
  struct regcache *regcache = get_current_regcache ();
  struct gdbarch *gdbarch = get_regcache_arch (regcache);
  CORE_ADDR pc = regcache_read_pc (regcache);
  int oneproc = 0;

  if (step > 0)
    step_start_function = find_pc_function (pc);
  if (step < 0)
    stop_after_trap = 1;

  if (addr == (CORE_ADDR) -1)
    {
      if (pc == stop_pc && breakpoint_here_p (pc))
	/* There is a breakpoint at the address we will resume at,
	   step one instruction before inserting breakpoints so that
	   we do not stop right away (and report a second hit at this
	   breakpoint).  */
	oneproc = 1;
      else if (gdbarch_single_step_through_delay_p (gdbarch)
	       && gdbarch_single_step_through_delay (gdbarch,
						     get_current_frame ()))
	/* We stepped onto an instruction that needs to be stepped
	   again before re-inserting the breakpoint, do so.  */
	oneproc = 1;
    }
  else
    {
      regcache_write_pc (regcache, addr);
    }

  if (debug_infrun)
    fprintf_unfiltered (gdb_stdlog,
			"infrun: proceed (addr=0x%s, signal=%d, step=%d)\n",
			paddr_nz (addr), siggnal, step);

  if (non_stop)
    /* In non-stop, each thread is handled individually.  The context
       must already be set to the right thread here.  */
    ;
  else
    {
      /* In a multi-threaded task we may select another thread and
	 then continue or step.

	 But if the old thread was stopped at a breakpoint, it will
	 immediately cause another breakpoint stop without any
	 execution (i.e. it will report a breakpoint hit incorrectly).
	 So we must step over it first.

	 prepare_to_proceed checks the current thread against the
	 thread that reported the most recent event.  If a step-over
	 is required it returns TRUE and sets the current thread to
	 the old thread. */
      if (prepare_to_proceed (step))
	oneproc = 1;
    }

  if (oneproc)
    {
      stepping_over_breakpoint = 1;
      /* If displaced stepping is enabled, we can step over the
	 breakpoint without hitting it, so leave all breakpoints
	 inserted.  Otherwise we need to disable all breakpoints, step
	 one instruction, and then re-add them when that step is
	 finished.  */
      if (!use_displaced_stepping (gdbarch))
	remove_breakpoints ();
    }

  /* We can insert breakpoints if we're not trying to step over one,
     or if we are stepping over one but we're using displaced stepping
     to do so.  */
  if (! stepping_over_breakpoint || use_displaced_stepping (gdbarch))
    insert_breakpoints ();

  if (siggnal != TARGET_SIGNAL_DEFAULT)
    stop_signal = siggnal;
  /* If this signal should not be seen by program,
     give it zero.  Used for debugging signals.  */
  else if (!signal_program[stop_signal])
    stop_signal = TARGET_SIGNAL_0;

  annotate_starting ();

  /* Make sure that output from GDB appears before output from the
     inferior.  */
  gdb_flush (gdb_stdout);

  /* Refresh prev_pc value just prior to resuming.  This used to be
     done in stop_stepping, however, setting prev_pc there did not handle
     scenarios such as inferior function calls or returning from
     a function via the return command.  In those cases, the prev_pc
     value was not set properly for subsequent commands.  The prev_pc value 
     is used to initialize the starting line number in the ecs.  With an 
     invalid value, the gdb next command ends up stopping at the position
     represented by the next line table entry past our start position.
     On platforms that generate one line table entry per line, this
     is not a problem.  However, on the ia64, the compiler generates
     extraneous line table entries that do not increase the line number.
     When we issue the gdb next command on the ia64 after an inferior call
     or a return command, we often end up a few instructions forward, still 
     within the original line we started.

     An attempt was made to have init_execution_control_state () refresh
     the prev_pc value before calculating the line number.  This approach
     did not work because on platforms that use ptrace, the pc register
     cannot be read unless the inferior is stopped.  At that point, we
     are not guaranteed the inferior is stopped and so the regcache_read_pc ()
     call can fail.  Setting the prev_pc value here ensures the value is 
     updated correctly when the inferior is stopped.  */
  prev_pc = regcache_read_pc (get_current_regcache ());

  /* Fill in with reasonable starting values.  */
  init_thread_stepping_state (tss);

  /* We'll update this if & when we switch to a new thread. */
  previous_inferior_ptid = inferior_ptid;

  /* Reset to normal state.  */
  init_infwait_state ();

  /* Resume inferior.  */
  resume (oneproc || step || bpstat_should_step (), stop_signal);

  /* Wait for it to stop (if not standalone)
     and in any case decode why it stopped, and act accordingly.  */
  /* Do this only if we are not using the event loop, or if the target
     does not support asynchronous execution. */
  if (!target_can_async_p ())
    {
      wait_for_inferior (0);
      normal_stop ();
    }
}


/* Start remote-debugging of a machine over a serial link.  */

void
start_remote (int from_tty)
{
  init_wait_for_inferior ();
  stop_soon = STOP_QUIETLY_REMOTE;
  stepping_over_breakpoint = 0;

  /* Always go on waiting for the target, regardless of the mode. */
  /* FIXME: cagney/1999-09-23: At present it isn't possible to
     indicate to wait_for_inferior that a target should timeout if
     nothing is returned (instead of just blocking).  Because of this,
     targets expecting an immediate response need to, internally, set
     things up so that the target_wait() is forced to eventually
     timeout. */
  /* FIXME: cagney/1999-09-24: It isn't possible for target_open() to
     differentiate to its caller what the state of the target is after
     the initial open has been performed.  Here we're assuming that
     the target has stopped.  It should be possible to eventually have
     target_open() return to the caller an indication that the target
     is currently running and GDB state should be set to the same as
     for an async run. */
  wait_for_inferior (0);

  /* Now that the inferior has stopped, do any bookkeeping like
     loading shared libraries.  We want to do this before normal_stop,
     so that the displayed frame is up to date.  */
  post_create_inferior (&current_target, from_tty);

  normal_stop ();
}

/* Initialize static vars when a new inferior begins.  */

void
init_wait_for_inferior (void)
{
  /* These are meaningless until the first time through wait_for_inferior.  */
  prev_pc = 0;

  breakpoint_init_inferior (inf_starting);

  /* Don't confuse first call to proceed(). */
  stop_signal = TARGET_SIGNAL_0;

  /* The first resume is not following a fork/vfork/exec. */
  pending_follow.kind = TARGET_WAITKIND_SPURIOUS;	/* I.e., none. */

  clear_proceed_status ();

  stepping_past_singlestep_breakpoint = 0;
  deferred_step_ptid = null_ptid;

  target_last_wait_ptid = minus_one_ptid;

  init_thread_stepping_state (tss);
  previous_inferior_ptid = null_ptid;
  init_infwait_state ();

  displaced_step_clear ();
}


/* This enum encodes possible reasons for doing a target_wait, so that
   wfi can call target_wait in one place.  (Ultimately the call will be
   moved out of the infinite loop entirely.) */

enum infwait_states
{
  infwait_normal_state,
  infwait_thread_hop_state,
  infwait_step_watch_state,
  infwait_nonstep_watch_state
};

/* Why did the inferior stop? Used to print the appropriate messages
   to the interface from within handle_inferior_event(). */
enum inferior_stop_reason
{
  /* Step, next, nexti, stepi finished. */
  END_STEPPING_RANGE,
  /* Inferior terminated by signal. */
  SIGNAL_EXITED,
  /* Inferior exited. */
  EXITED,
  /* Inferior received signal, and user asked to be notified. */
  SIGNAL_RECEIVED
};

/* The PTID we'll do a target_wait on.*/
ptid_t waiton_ptid;

/* Current inferior wait state.  */
enum infwait_states infwait_state;

/* Data to be passed around while handling an event.  This data is
   discarded between events.  */
struct execution_control_state
{
  ptid_t ptid;
  struct target_waitstatus ws;
  int random_signal;
  CORE_ADDR stop_func_start;
  CORE_ADDR stop_func_end;
  char *stop_func_name;
  int new_thread_event;
  int wait_some_more;
};

void init_execution_control_state (struct execution_control_state *ecs);

void handle_inferior_event (struct execution_control_state *ecs);

static void step_into_function (struct execution_control_state *ecs);
static void insert_step_resume_breakpoint_at_frame (struct frame_info *step_frame);
static void insert_step_resume_breakpoint_at_caller (struct frame_info *);
static void insert_step_resume_breakpoint_at_sal (struct symtab_and_line sr_sal,
						  struct frame_id sr_id);
static void insert_longjmp_resume_breakpoint (CORE_ADDR);

static void stop_stepping (struct execution_control_state *ecs);
static void prepare_to_wait (struct execution_control_state *ecs);
static void keep_going (struct execution_control_state *ecs);
static void print_stop_reason (enum inferior_stop_reason stop_reason,
			       int stop_info);

/* Wait for control to return from inferior to debugger.

   If TREAT_EXEC_AS_SIGTRAP is non-zero, then handle EXEC signals
   as if they were SIGTRAP signals.  This can be useful during
   the startup sequence on some targets such as HP/UX, where
   we receive an EXEC event instead of the expected SIGTRAP.

   If inferior gets a signal, we may decide to start it up again
   instead of returning.  That is why there is a loop in this function.
   When this function actually returns it means the inferior
   should be left stopped and GDB should read more commands.  */

void
wait_for_inferior (int treat_exec_as_sigtrap)
{
  struct cleanup *old_cleanups;
  struct execution_control_state ecss;
  struct execution_control_state *ecs;

  if (debug_infrun)
    fprintf_unfiltered
      (gdb_stdlog, "infrun: wait_for_inferior (treat_exec_as_sigtrap=%d)\n",
       treat_exec_as_sigtrap);

  old_cleanups = make_cleanup (delete_step_resume_breakpoint,
			       &step_resume_breakpoint);

  ecs = &ecss;
  memset (ecs, 0, sizeof (*ecs));

  overlay_cache_invalid = 1;

  /* We have to invalidate the registers BEFORE calling target_wait
     because they can be loaded from the target while in target_wait.
     This makes remote debugging a bit more efficient for those
     targets that provide critical registers as part of their normal
     status mechanism. */

  registers_changed ();

  while (1)
    {
      if (deprecated_target_wait_hook)
	ecs->ptid = deprecated_target_wait_hook (waiton_ptid, &ecs->ws);
      else
	ecs->ptid = target_wait (waiton_ptid, &ecs->ws);

      if (treat_exec_as_sigtrap && ecs->ws.kind == TARGET_WAITKIND_EXECD)
        {
          xfree (ecs->ws.value.execd_pathname);
          ecs->ws.kind = TARGET_WAITKIND_STOPPED;
          ecs->ws.value.sig = TARGET_SIGNAL_TRAP;
        }

      /* Now figure out what to do with the result of the result.  */
      handle_inferior_event (ecs);

      if (!ecs->wait_some_more)
	break;
    }
  do_cleanups (old_cleanups);
}

/* Asynchronous version of wait_for_inferior. It is called by the
   event loop whenever a change of state is detected on the file
   descriptor corresponding to the target. It can be called more than
   once to complete a single execution command. In such cases we need
   to keep the state in a global variable ECSS. If it is the last time
   that this function is called for a single execution command, then
   report to the user that the inferior has stopped, and do the
   necessary cleanups. */

void
fetch_inferior_event (void *client_data)
{
  struct execution_control_state ecss;
  struct execution_control_state *ecs = &ecss;
  struct cleanup *old_chain = make_cleanup (null_cleanup, NULL);
  int was_sync = sync_execution;

  memset (ecs, 0, sizeof (*ecs));

  overlay_cache_invalid = 1;

  if (non_stop)
    /* In non-stop mode, the user/frontend should not notice a thread
       switch due to internal events.  Make sure we reverse to the
       user selected thread and frame after handling the event and
       running any breakpoint commands.  */
    make_cleanup_restore_current_thread ();

  /* We have to invalidate the registers BEFORE calling target_wait
     because they can be loaded from the target while in target_wait.
     This makes remote debugging a bit more efficient for those
     targets that provide critical registers as part of their normal
     status mechanism. */

  registers_changed ();

  if (deprecated_target_wait_hook)
    ecs->ptid =
      deprecated_target_wait_hook (waiton_ptid, &ecs->ws);
  else
    ecs->ptid = target_wait (waiton_ptid, &ecs->ws);

  if (non_stop
      && ecs->ws.kind != TARGET_WAITKIND_IGNORE
      && ecs->ws.kind != TARGET_WAITKIND_EXITED
      && ecs->ws.kind != TARGET_WAITKIND_SIGNALLED)
    /* In non-stop mode, each thread is handled individually.  Switch
       early, so the global state is set correctly for this
       thread.  */
    context_switch (ecs->ptid);

  /* Now figure out what to do with the result of the result.  */
  handle_inferior_event (ecs);

  if (!ecs->wait_some_more)
    {
      delete_step_resume_breakpoint (&step_resume_breakpoint);

      normal_stop ();
      if (step_multi && stop_step)
	inferior_event_handler (INF_EXEC_CONTINUE, NULL);
      else
	inferior_event_handler (INF_EXEC_COMPLETE, NULL);
    }

  /* Revert thread and frame.  */
  do_cleanups (old_chain);

  /* If the inferior was in sync execution mode, and now isn't,
     restore the prompt.  */
  if (was_sync && !sync_execution)
    display_gdb_prompt (0);
}

/* Prepare an execution control state for looping through a
   wait_for_inferior-type loop.  */

void
init_execution_control_state (struct execution_control_state *ecs)
{
  ecs->random_signal = 0;
}

/* Clear context switchable stepping state.  */

void
init_thread_stepping_state (struct thread_stepping_state *tss)
{
  struct symtab_and_line sal;

  tss->stepping_over_breakpoint = 0;
  tss->step_after_step_resume_breakpoint = 0;
  tss->stepping_through_solib_after_catch = 0;
  tss->stepping_through_solib_catchpoints = NULL;

  sal = find_pc_line (prev_pc, 0);
  tss->current_line = sal.line;
  tss->current_symtab = sal.symtab;
}

/* Return the cached copy of the last pid/waitstatus returned by
   target_wait()/deprecated_target_wait_hook().  The data is actually
   cached by handle_inferior_event(), which gets called immediately
   after target_wait()/deprecated_target_wait_hook().  */

void
get_last_target_status (ptid_t *ptidp, struct target_waitstatus *status)
{
  *ptidp = target_last_wait_ptid;
  *status = target_last_waitstatus;
}

void
nullify_last_target_wait_ptid (void)
{
  target_last_wait_ptid = minus_one_ptid;
}

/* Switch thread contexts, maintaining "infrun state". */

static void
context_switch (ptid_t ptid)
{
  /* Caution: it may happen that the new thread (or the old one!)
     is not in the thread list.  In this case we must not attempt
     to "switch context", or we run the risk that our context may
     be lost.  This may happen as a result of the target module
     mishandling thread creation.  */

  if (debug_infrun)
    {
      fprintf_unfiltered (gdb_stdlog, "infrun: Switching context from %s ",
			  target_pid_to_str (inferior_ptid));
      fprintf_unfiltered (gdb_stdlog, "to %s\n",
			  target_pid_to_str (ptid));
    }

  if (in_thread_list (inferior_ptid) && in_thread_list (ptid))
    {				/* Perform infrun state context switch: */
      /* Save infrun state for the old thread.  */
      save_infrun_state (inferior_ptid, prev_pc,
			 stepping_over_breakpoint, step_resume_breakpoint,
			 step_range_start,
			 step_range_end, &step_frame_id,
			 tss->stepping_over_breakpoint,
			 tss->stepping_through_solib_after_catch,
			 tss->stepping_through_solib_catchpoints,
			 tss->current_line, tss->current_symtab,
			 cmd_continuation, intermediate_continuation,
			 proceed_to_finish,
			 step_over_calls,
			 stop_step,
			 step_multi,
			 stop_signal,
			 stop_bpstat);

      /* Load infrun state for the new thread.  */
      load_infrun_state (ptid, &prev_pc,
			 &stepping_over_breakpoint, &step_resume_breakpoint,
			 &step_range_start,
			 &step_range_end, &step_frame_id,
			 &tss->stepping_over_breakpoint,
			 &tss->stepping_through_solib_after_catch,
			 &tss->stepping_through_solib_catchpoints,
			 &tss->current_line, &tss->current_symtab,
			 &cmd_continuation, &intermediate_continuation,
			 &proceed_to_finish,
			 &step_over_calls,
			 &stop_step,
			 &step_multi,
			 &stop_signal,
			 &stop_bpstat);
    }

  switch_to_thread (ptid);
}

/* Context switch to thread PTID.  */
ptid_t
context_switch_to (ptid_t ptid)
{
  ptid_t current_ptid = inferior_ptid;

  /* Context switch to the new thread.	*/
  if (!ptid_equal (ptid, inferior_ptid))
    {
      context_switch (ptid);
    }
  return current_ptid;
}

static void
adjust_pc_after_break (struct execution_control_state *ecs)
{
  struct regcache *regcache = get_thread_regcache (ecs->ptid);
  struct gdbarch *gdbarch = get_regcache_arch (regcache);
  CORE_ADDR breakpoint_pc;

  /* If this target does not decrement the PC after breakpoints, then
     we have nothing to do.  */
  if (gdbarch_decr_pc_after_break (gdbarch) == 0)
    return;

  /* If we've hit a breakpoint, we'll normally be stopped with SIGTRAP.  If
     we aren't, just return.

     We assume that waitkinds other than TARGET_WAITKIND_STOPPED are not
     affected by gdbarch_decr_pc_after_break.  Other waitkinds which are
     implemented by software breakpoints should be handled through the normal
     breakpoint layer.

     NOTE drow/2004-01-31: On some targets, breakpoints may generate
     different signals (SIGILL or SIGEMT for instance), but it is less
     clear where the PC is pointing afterwards.  It may not match
     gdbarch_decr_pc_after_break.  I don't know any specific target that
     generates these signals at breakpoints (the code has been in GDB since at
     least 1992) so I can not guess how to handle them here.

     In earlier versions of GDB, a target with 
     gdbarch_have_nonsteppable_watchpoint would have the PC after hitting a
     watchpoint affected by gdbarch_decr_pc_after_break.  I haven't found any
     target with both of these set in GDB history, and it seems unlikely to be
     correct, so gdbarch_have_nonsteppable_watchpoint is not checked here.  */

  if (ecs->ws.kind != TARGET_WAITKIND_STOPPED)
    return;

  if (ecs->ws.value.sig != TARGET_SIGNAL_TRAP)
    return;

  /* Find the location where (if we've hit a breakpoint) the
     breakpoint would be.  */
  breakpoint_pc = regcache_read_pc (regcache)
		  - gdbarch_decr_pc_after_break (gdbarch);

  /* Check whether there actually is a software breakpoint inserted
     at that location.  */
  if (software_breakpoint_inserted_here_p (breakpoint_pc))
    {
      /* When using hardware single-step, a SIGTRAP is reported for both
	 a completed single-step and a software breakpoint.  Need to
	 differentiate between the two, as the latter needs adjusting
	 but the former does not.

	 The SIGTRAP can be due to a completed hardware single-step only if 
	  - we didn't insert software single-step breakpoints
	  - the thread to be examined is still the current thread
	  - this thread is currently being stepped

	 If any of these events did not occur, we must have stopped due
	 to hitting a software breakpoint, and have to back up to the
	 breakpoint address.

	 As a special case, we could have hardware single-stepped a
	 software breakpoint.  In this case (prev_pc == breakpoint_pc),
	 we also need to back up to the breakpoint address.  */

      if (singlestep_breakpoints_inserted_p
	  || !ptid_equal (ecs->ptid, inferior_ptid)
	  || !currently_stepping (tss)
	  || prev_pc == breakpoint_pc)
	regcache_write_pc (regcache, breakpoint_pc);
    }
}

void
init_infwait_state (void)
{
  waiton_ptid = pid_to_ptid (-1);
  infwait_state = infwait_normal_state;
}

void
error_is_running (void)
{
  error (_("\
Cannot execute this command while the selected thread is running."));
}

void
ensure_not_running (void)
{
  if (is_running (inferior_ptid))
    error_is_running ();
}

/* Given an execution control state that has been freshly filled in
   by an event from the inferior, figure out what it means and take
   appropriate action.  */

void
handle_inferior_event (struct execution_control_state *ecs)
{
  int sw_single_step_trap_p = 0;
  int stopped_by_watchpoint;
  int stepped_after_stopped_by_watchpoint = 0;
  struct symtab_and_line stop_pc_sal;

  breakpoint_retire_moribund ();

  /* Cache the last pid/waitstatus. */
  target_last_wait_ptid = ecs->ptid;
  target_last_waitstatus = ecs->ws;

  /* Always clear state belonging to the previous time we stopped.  */
  stop_stack_dummy = 0;

  adjust_pc_after_break (ecs);

  reinit_frame_cache ();

  /* If it's a new process, add it to the thread database */

  ecs->new_thread_event = (!ptid_equal (ecs->ptid, inferior_ptid)
			   && !ptid_equal (ecs->ptid, minus_one_ptid)
			   && !in_thread_list (ecs->ptid));

  if (ecs->ws.kind != TARGET_WAITKIND_EXITED
      && ecs->ws.kind != TARGET_WAITKIND_SIGNALLED && ecs->new_thread_event)
    add_thread (ecs->ptid);

  if (ecs->ws.kind != TARGET_WAITKIND_IGNORE)
    {
      /* Mark the non-executing threads accordingly.  */
      if (!non_stop
 	  || ecs->ws.kind == TARGET_WAITKIND_EXITED
 	  || ecs->ws.kind == TARGET_WAITKIND_SIGNALLED)
 	set_executing (pid_to_ptid (-1), 0);
      else
 	set_executing (ecs->ptid, 0);
    }

  switch (infwait_state)
    {
    case infwait_thread_hop_state:
      if (debug_infrun)
        fprintf_unfiltered (gdb_stdlog, "infrun: infwait_thread_hop_state\n");
      /* Cancel the waiton_ptid. */
      waiton_ptid = pid_to_ptid (-1);
      break;

    case infwait_normal_state:
      if (debug_infrun)
        fprintf_unfiltered (gdb_stdlog, "infrun: infwait_normal_state\n");
      break;

    case infwait_step_watch_state:
      if (debug_infrun)
        fprintf_unfiltered (gdb_stdlog,
			    "infrun: infwait_step_watch_state\n");

      stepped_after_stopped_by_watchpoint = 1;
      break;

    case infwait_nonstep_watch_state:
      if (debug_infrun)
        fprintf_unfiltered (gdb_stdlog,
			    "infrun: infwait_nonstep_watch_state\n");
      insert_breakpoints ();

      /* FIXME-maybe: is this cleaner than setting a flag?  Does it
         handle things like signals arriving and other things happening
         in combination correctly?  */
      stepped_after_stopped_by_watchpoint = 1;
      break;

    default:
      internal_error (__FILE__, __LINE__, _("bad switch"));
    }
  infwait_state = infwait_normal_state;

  switch (ecs->ws.kind)
    {
    case TARGET_WAITKIND_LOADED:
      if (debug_infrun)
        fprintf_unfiltered (gdb_stdlog, "infrun: TARGET_WAITKIND_LOADED\n");
      /* Ignore gracefully during startup of the inferior, as it might
         be the shell which has just loaded some objects, otherwise
         add the symbols for the newly loaded objects.  Also ignore at
         the beginning of an attach or remote session; we will query
         the full list of libraries once the connection is
         established.  */
      if (stop_soon == NO_STOP_QUIETLY)
	{
	  /* Check for any newly added shared libraries if we're
	     supposed to be adding them automatically.  Switch
	     terminal for any messages produced by
	     breakpoint_re_set.  */
	  target_terminal_ours_for_output ();
	  /* NOTE: cagney/2003-11-25: Make certain that the target
	     stack's section table is kept up-to-date.  Architectures,
	     (e.g., PPC64), use the section table to perform
	     operations such as address => section name and hence
	     require the table to contain all sections (including
	     those found in shared libraries).  */
	  /* NOTE: cagney/2003-11-25: Pass current_target and not
	     exec_ops to SOLIB_ADD.  This is because current GDB is
	     only tooled to propagate section_table changes out from
	     the "current_target" (see target_resize_to_sections), and
	     not up from the exec stratum.  This, of course, isn't
	     right.  "infrun.c" should only interact with the
	     exec/process stratum, instead relying on the target stack
	     to propagate relevant changes (stop, section table
	     changed, ...) up to other layers.  */
#ifdef SOLIB_ADD
	  SOLIB_ADD (NULL, 0, &current_target, auto_solib_add);
#else
	  solib_add (NULL, 0, &current_target, auto_solib_add);
#endif
	  target_terminal_inferior ();

	  /* If requested, stop when the dynamic linker notifies
	     gdb of events.  This allows the user to get control
	     and place breakpoints in initializer routines for
	     dynamically loaded objects (among other things).  */
	  if (stop_on_solib_events)
	    {
	      stop_stepping (ecs);
	      return;
	    }

	  /* NOTE drow/2007-05-11: This might be a good place to check
	     for "catch load".  */
	}

      /* If we are skipping through a shell, or through shared library
	 loading that we aren't interested in, resume the program.  If
	 we're running the program normally, also resume.  But stop if
	 we're attaching or setting up a remote connection.  */
      if (stop_soon == STOP_QUIETLY || stop_soon == NO_STOP_QUIETLY)
	{
	  /* Loading of shared libraries might have changed breakpoint
	     addresses.  Make sure new breakpoints are inserted.  */
	  if (stop_soon == NO_STOP_QUIETLY
	      && !breakpoints_always_inserted_mode ())
	    insert_breakpoints ();
	  resume (0, TARGET_SIGNAL_0);
	  prepare_to_wait (ecs);
	  return;
	}

      break;

    case TARGET_WAITKIND_SPURIOUS:
      if (debug_infrun)
        fprintf_unfiltered (gdb_stdlog, "infrun: TARGET_WAITKIND_SPURIOUS\n");
      resume (0, TARGET_SIGNAL_0);
      prepare_to_wait (ecs);
      return;

    case TARGET_WAITKIND_EXITED:
      if (debug_infrun)
        fprintf_unfiltered (gdb_stdlog, "infrun: TARGET_WAITKIND_EXITED\n");
      target_terminal_ours ();	/* Must do this before mourn anyway */
      print_stop_reason (EXITED, ecs->ws.value.integer);

      /* Record the exit code in the convenience variable $_exitcode, so
         that the user can inspect this again later.  */
      set_internalvar (lookup_internalvar ("_exitcode"),
		       value_from_longest (builtin_type_int,
					   (LONGEST) ecs->ws.value.integer));
      gdb_flush (gdb_stdout);
      target_mourn_inferior ();
      singlestep_breakpoints_inserted_p = 0;
      stop_print_frame = 0;
      stop_stepping (ecs);
      return;

    case TARGET_WAITKIND_SIGNALLED:
      if (debug_infrun)
        fprintf_unfiltered (gdb_stdlog, "infrun: TARGET_WAITKIND_SIGNALLED\n");
      stop_print_frame = 0;
      stop_signal = ecs->ws.value.sig;
      target_terminal_ours ();	/* Must do this before mourn anyway */

      /* Note: By definition of TARGET_WAITKIND_SIGNALLED, we shouldn't
         reach here unless the inferior is dead.  However, for years
         target_kill() was called here, which hints that fatal signals aren't
         really fatal on some systems.  If that's true, then some changes
         may be needed. */
      target_mourn_inferior ();

      print_stop_reason (SIGNAL_EXITED, stop_signal);
      singlestep_breakpoints_inserted_p = 0;
      stop_stepping (ecs);
      return;

      /* The following are the only cases in which we keep going;
         the above cases end in a continue or goto. */
    case TARGET_WAITKIND_FORKED:
    case TARGET_WAITKIND_VFORKED:
      if (debug_infrun)
        fprintf_unfiltered (gdb_stdlog, "infrun: TARGET_WAITKIND_FORKED\n");
      stop_signal = TARGET_SIGNAL_TRAP;
      pending_follow.kind = ecs->ws.kind;

      pending_follow.fork_event.parent_pid = ecs->ptid;
      pending_follow.fork_event.child_pid = ecs->ws.value.related_pid;

      if (!ptid_equal (ecs->ptid, inferior_ptid))
	{
	  context_switch (ecs->ptid);
	  reinit_frame_cache ();
	}

      stop_pc = read_pc ();

      stop_bpstat = bpstat_stop_status (stop_pc, ecs->ptid);

      ecs->random_signal = !bpstat_explains_signal (stop_bpstat);

      /* If no catchpoint triggered for this, then keep going.  */
      if (ecs->random_signal)
	{
	  stop_signal = TARGET_SIGNAL_0;
	  keep_going (ecs);
	  return;
	}
      goto process_event_stop_test;

    case TARGET_WAITKIND_EXECD:
      if (debug_infrun)
        fprintf_unfiltered (gdb_stdlog, "infrun: TARGET_WAITKIND_EXECD\n");
      stop_signal = TARGET_SIGNAL_TRAP;

      pending_follow.execd_pathname =
	savestring (ecs->ws.value.execd_pathname,
		    strlen (ecs->ws.value.execd_pathname));

      /* This causes the eventpoints and symbol table to be reset.  Must
         do this now, before trying to determine whether to stop. */
      follow_exec (inferior_ptid, pending_follow.execd_pathname);
      xfree (pending_follow.execd_pathname);

      stop_pc = regcache_read_pc (get_thread_regcache (ecs->ptid));

      {
	/* The breakpoints module may need to touch the inferior's
	   memory.  Switch to the (stopped) event ptid
	   momentarily.  */
	ptid_t saved_inferior_ptid = inferior_ptid;
	inferior_ptid = ecs->ptid;

	stop_bpstat = bpstat_stop_status (stop_pc, ecs->ptid);

	ecs->random_signal = !bpstat_explains_signal (stop_bpstat);
	inferior_ptid = saved_inferior_ptid;
      }

      if (!ptid_equal (ecs->ptid, inferior_ptid))
	{
	  context_switch (ecs->ptid);
	  reinit_frame_cache ();
	}

      /* If no catchpoint triggered for this, then keep going.  */
      if (ecs->random_signal)
	{
	  stop_signal = TARGET_SIGNAL_0;
	  keep_going (ecs);
	  return;
	}
      goto process_event_stop_test;

      /* Be careful not to try to gather much state about a thread
         that's in a syscall.  It's frequently a losing proposition.  */
    case TARGET_WAITKIND_SYSCALL_ENTRY:
      if (debug_infrun)
        fprintf_unfiltered (gdb_stdlog, "infrun: TARGET_WAITKIND_SYSCALL_ENTRY\n");
      resume (0, TARGET_SIGNAL_0);
      prepare_to_wait (ecs);
      return;

      /* Before examining the threads further, step this thread to
         get it entirely out of the syscall.  (We get notice of the
         event when the thread is just on the verge of exiting a
         syscall.  Stepping one instruction seems to get it back
         into user code.)  */
    case TARGET_WAITKIND_SYSCALL_RETURN:
      if (debug_infrun)
        fprintf_unfiltered (gdb_stdlog, "infrun: TARGET_WAITKIND_SYSCALL_RETURN\n");
      target_resume (ecs->ptid, 1, TARGET_SIGNAL_0);
      prepare_to_wait (ecs);
      return;

    case TARGET_WAITKIND_STOPPED:
      if (debug_infrun)
        fprintf_unfiltered (gdb_stdlog, "infrun: TARGET_WAITKIND_STOPPED\n");
      stop_signal = ecs->ws.value.sig;
      break;

      /* We had an event in the inferior, but we are not interested
         in handling it at this level. The lower layers have already
         done what needs to be done, if anything.

         One of the possible circumstances for this is when the
         inferior produces output for the console. The inferior has
         not stopped, and we are ignoring the event.  Another possible
         circumstance is any event which the lower level knows will be
         reported multiple times without an intervening resume.  */
    case TARGET_WAITKIND_IGNORE:
      if (debug_infrun)
        fprintf_unfiltered (gdb_stdlog, "infrun: TARGET_WAITKIND_IGNORE\n");
      prepare_to_wait (ecs);
      return;
    }

  if (ecs->new_thread_event)
    {
      if (non_stop)
	/* Non-stop assumes that the target handles adding new threads
	   to the thread list.  */
	internal_error (__FILE__, __LINE__, "\
targets should add new threads to the thread list themselves in non-stop mode.");

      /* We may want to consider not doing a resume here in order to
	 give the user a chance to play with the new thread.  It might
	 be good to make that a user-settable option.  */

      /* At this point, all threads are stopped (happens automatically
	 in either the OS or the native code).  Therefore we need to
	 continue all threads in order to make progress.  */

      target_resume (RESUME_ALL, 0, TARGET_SIGNAL_0);
      prepare_to_wait (ecs);
      return;
    }

  /* Do we need to clean up the state of a thread that has completed a
     displaced single-step?  (Doing so usually affects the PC, so do
     it here, before we set stop_pc.)  */
  displaced_step_fixup (ecs->ptid, stop_signal);

  stop_pc = regcache_read_pc (get_thread_regcache (ecs->ptid));

  if (debug_infrun)
    {
      fprintf_unfiltered (gdb_stdlog, "infrun: stop_pc = 0x%s\n",
                          paddr_nz (stop_pc));
      if (STOPPED_BY_WATCHPOINT (&ecs->ws))
	{
          CORE_ADDR addr;
	  fprintf_unfiltered (gdb_stdlog, "infrun: stopped by watchpoint\n");

          if (target_stopped_data_address (&current_target, &addr))
            fprintf_unfiltered (gdb_stdlog,
                                "infrun: stopped data address = 0x%s\n",
                                paddr_nz (addr));
          else
            fprintf_unfiltered (gdb_stdlog,
                                "infrun: (no data address available)\n");
	}
    }

  if (stepping_past_singlestep_breakpoint)
    {
      gdb_assert (singlestep_breakpoints_inserted_p);
      gdb_assert (ptid_equal (singlestep_ptid, ecs->ptid));
      gdb_assert (!ptid_equal (singlestep_ptid, saved_singlestep_ptid));

      stepping_past_singlestep_breakpoint = 0;

      /* We've either finished single-stepping past the single-step
         breakpoint, or stopped for some other reason.  It would be nice if
         we could tell, but we can't reliably.  */
      if (stop_signal == TARGET_SIGNAL_TRAP)
	{
	  if (debug_infrun)
	    fprintf_unfiltered (gdb_stdlog, "infrun: stepping_past_singlestep_breakpoint\n");
	  /* Pull the single step breakpoints out of the target.  */
	  remove_single_step_breakpoints ();
	  singlestep_breakpoints_inserted_p = 0;

	  ecs->random_signal = 0;

	  context_switch (saved_singlestep_ptid);
	  if (deprecated_context_hook)
	    deprecated_context_hook (pid_to_thread_id (ecs->ptid));

	  resume (1, TARGET_SIGNAL_0);
	  prepare_to_wait (ecs);
	  return;
	}
    }

  stepping_past_singlestep_breakpoint = 0;

  if (!ptid_equal (deferred_step_ptid, null_ptid))
    {
      /* In non-stop mode, there's never a deferred_step_ptid set.  */
      gdb_assert (!non_stop);

      /* If we stopped for some other reason than single-stepping, ignore
	 the fact that we were supposed to switch back.  */
      if (stop_signal == TARGET_SIGNAL_TRAP)
	{
	  if (debug_infrun)
	    fprintf_unfiltered (gdb_stdlog,
				"infrun: handling deferred step\n");

	  /* Pull the single step breakpoints out of the target.  */
	  if (singlestep_breakpoints_inserted_p)
	    {
	      remove_single_step_breakpoints ();
	      singlestep_breakpoints_inserted_p = 0;
	    }

	  /* Note: We do not call context_switch at this point, as the
	     context is already set up for stepping the original thread.  */
	  switch_to_thread (deferred_step_ptid);
	  deferred_step_ptid = null_ptid;
	  /* Suppress spurious "Switching to ..." message.  */
	  previous_inferior_ptid = inferior_ptid;

	  resume (1, TARGET_SIGNAL_0);
	  prepare_to_wait (ecs);
	  return;
	}

      deferred_step_ptid = null_ptid;
    }

  /* See if a thread hit a thread-specific breakpoint that was meant for
     another thread.  If so, then step that thread past the breakpoint,
     and continue it.  */

  if (stop_signal == TARGET_SIGNAL_TRAP)
    {
      int thread_hop_needed = 0;

      /* Check if a regular breakpoint has been hit before checking
         for a potential single step breakpoint. Otherwise, GDB will
         not see this breakpoint hit when stepping onto breakpoints.  */
      if (regular_breakpoint_inserted_here_p (stop_pc))
	{
	  ecs->random_signal = 0;
	  if (!breakpoint_thread_match (stop_pc, ecs->ptid))
	    thread_hop_needed = 1;
	}
      else if (singlestep_breakpoints_inserted_p)
	{
	  /* We have not context switched yet, so this should be true
	     no matter which thread hit the singlestep breakpoint.  */
	  gdb_assert (ptid_equal (inferior_ptid, singlestep_ptid));
	  if (debug_infrun)
	    fprintf_unfiltered (gdb_stdlog, "infrun: software single step "
				"trap for %s\n",
				target_pid_to_str (ecs->ptid));

	  ecs->random_signal = 0;
	  /* The call to in_thread_list is necessary because PTIDs sometimes
	     change when we go from single-threaded to multi-threaded.  If
	     the singlestep_ptid is still in the list, assume that it is
	     really different from ecs->ptid.  */
	  if (!ptid_equal (singlestep_ptid, ecs->ptid)
	      && in_thread_list (singlestep_ptid))
	    {
	      /* If the PC of the thread we were trying to single-step
		 has changed, discard this event (which we were going
		 to ignore anyway), and pretend we saw that thread
		 trap.  This prevents us continuously moving the
		 single-step breakpoint forward, one instruction at a
		 time.  If the PC has changed, then the thread we were
		 trying to single-step has trapped or been signalled,
		 but the event has not been reported to GDB yet.

		 There might be some cases where this loses signal
		 information, if a signal has arrived at exactly the
		 same time that the PC changed, but this is the best
		 we can do with the information available.  Perhaps we
		 should arrange to report all events for all threads
		 when they stop, or to re-poll the remote looking for
		 this particular thread (i.e. temporarily enable
		 schedlock).  */

	     CORE_ADDR new_singlestep_pc
	       = regcache_read_pc (get_thread_regcache (singlestep_ptid));

	     if (new_singlestep_pc != singlestep_pc)
	       {
		 if (debug_infrun)
		   fprintf_unfiltered (gdb_stdlog, "infrun: unexpected thread,"
				       " but expected thread advanced also\n");

		 /* The current context still belongs to
		    singlestep_ptid.  Don't swap here, since that's
		    the context we want to use.  Just fudge our
		    state and continue.  */
                 ecs->ptid = singlestep_ptid;
                 stop_pc = new_singlestep_pc;
               }
             else
	       {
		 if (debug_infrun)
		   fprintf_unfiltered (gdb_stdlog,
				       "infrun: unexpected thread\n");

		 thread_hop_needed = 1;
		 stepping_past_singlestep_breakpoint = 1;
		 saved_singlestep_ptid = singlestep_ptid;
	       }
	    }
	}

      if (thread_hop_needed)
	{
	  int remove_status = 0;

	  if (debug_infrun)
	    fprintf_unfiltered (gdb_stdlog, "infrun: thread_hop_needed\n");

	  /* Saw a breakpoint, but it was hit by the wrong thread.
	     Just continue. */

	  if (singlestep_breakpoints_inserted_p)
	    {
	      /* Pull the single step breakpoints out of the target. */
	      remove_single_step_breakpoints ();
	      singlestep_breakpoints_inserted_p = 0;
	    }

	  /* If the arch can displace step, don't remove the
	     breakpoints.  */
	  if (!use_displaced_stepping (current_gdbarch))
	    remove_status = remove_breakpoints ();

	  /* Did we fail to remove breakpoints?  If so, try
	     to set the PC past the bp.  (There's at least
	     one situation in which we can fail to remove
	     the bp's: On HP-UX's that use ttrace, we can't
	     change the address space of a vforking child
	     process until the child exits (well, okay, not
	     then either :-) or execs. */
	  if (remove_status != 0)
	    error (_("Cannot step over breakpoint hit in wrong thread"));
	  else
	    {			/* Single step */
	      if (!ptid_equal (inferior_ptid, ecs->ptid))
		context_switch (ecs->ptid);

	      if (!non_stop)
		{
		  /* Only need to require the next event from this
		     thread in all-stop mode.  */
		  waiton_ptid = ecs->ptid;
		  infwait_state = infwait_thread_hop_state;
		}

	      tss->stepping_over_breakpoint = 1;
	      keep_going (ecs);
	      registers_changed ();
	      return;
	    }
	}
      else if (singlestep_breakpoints_inserted_p)
	{
	  sw_single_step_trap_p = 1;
	  ecs->random_signal = 0;
	}
    }
  else
    ecs->random_signal = 1;

  /* See if something interesting happened to the non-current thread.  If
     so, then switch to that thread.  */
  if (!ptid_equal (ecs->ptid, inferior_ptid))
    {
      if (debug_infrun)
	fprintf_unfiltered (gdb_stdlog, "infrun: context switch\n");

      context_switch (ecs->ptid);

      if (deprecated_context_hook)
	deprecated_context_hook (pid_to_thread_id (ecs->ptid));
    }

  if (singlestep_breakpoints_inserted_p)
    {
      /* Pull the single step breakpoints out of the target. */
      remove_single_step_breakpoints ();
      singlestep_breakpoints_inserted_p = 0;
    }

  if (stepped_after_stopped_by_watchpoint)
    stopped_by_watchpoint = 0;
  else
    stopped_by_watchpoint = watchpoints_triggered (&ecs->ws);

  /* If necessary, step over this watchpoint.  We'll be back to display
     it in a moment.  */
  if (stopped_by_watchpoint
      && (HAVE_STEPPABLE_WATCHPOINT
	  || gdbarch_have_nonsteppable_watchpoint (current_gdbarch)))
    {
      /* At this point, we are stopped at an instruction which has
         attempted to write to a piece of memory under control of
         a watchpoint.  The instruction hasn't actually executed
         yet.  If we were to evaluate the watchpoint expression
         now, we would get the old value, and therefore no change
         would seem to have occurred.

         In order to make watchpoints work `right', we really need
         to complete the memory write, and then evaluate the
         watchpoint expression.  We do this by single-stepping the
	 target.

	 It may not be necessary to disable the watchpoint to stop over
	 it.  For example, the PA can (with some kernel cooperation)
	 single step over a watchpoint without disabling the watchpoint.

	 It is far more common to need to disable a watchpoint to step
	 the inferior over it.  If we have non-steppable watchpoints,
	 we must disable the current watchpoint; it's simplest to
	 disable all watchpoints and breakpoints.  */
	 
      if (!HAVE_STEPPABLE_WATCHPOINT)
	remove_breakpoints ();
      registers_changed ();
      target_resume (ecs->ptid, 1, TARGET_SIGNAL_0);	/* Single step */
      waiton_ptid = ecs->ptid;
      if (HAVE_STEPPABLE_WATCHPOINT)
	infwait_state = infwait_step_watch_state;
      else
	infwait_state = infwait_nonstep_watch_state;
      prepare_to_wait (ecs);
      return;
    }

  ecs->stop_func_start = 0;
  ecs->stop_func_end = 0;
  ecs->stop_func_name = 0;
  /* Don't care about return value; stop_func_start and stop_func_name
     will both be 0 if it doesn't work.  */
  find_pc_partial_function (stop_pc, &ecs->stop_func_name,
			    &ecs->stop_func_start, &ecs->stop_func_end);
  ecs->stop_func_start
    += gdbarch_deprecated_function_start_offset (current_gdbarch);
  tss->stepping_over_breakpoint = 0;
  bpstat_clear (&stop_bpstat);
  stop_step = 0;
  stop_print_frame = 1;
  ecs->random_signal = 0;
  stopped_by_random_signal = 0;

  if (stop_signal == TARGET_SIGNAL_TRAP
      && stepping_over_breakpoint
      && gdbarch_single_step_through_delay_p (current_gdbarch)
      && currently_stepping (tss))
    {
      /* We're trying to step off a breakpoint.  Turns out that we're
	 also on an instruction that needs to be stepped multiple
	 times before it's been fully executing. E.g., architectures
	 with a delay slot.  It needs to be stepped twice, once for
	 the instruction and once for the delay slot.  */
      int step_through_delay
	= gdbarch_single_step_through_delay (current_gdbarch,
					     get_current_frame ());
      if (debug_infrun && step_through_delay)
	fprintf_unfiltered (gdb_stdlog, "infrun: step through delay\n");
      if (step_range_end == 0 && step_through_delay)
	{
	  /* The user issued a continue when stopped at a breakpoint.
	     Set up for another trap and get out of here.  */
         tss->stepping_over_breakpoint = 1;
         keep_going (ecs);
         return;
	}
      else if (step_through_delay)
	{
	  /* The user issued a step when stopped at a breakpoint.
	     Maybe we should stop, maybe we should not - the delay
	     slot *might* correspond to a line of source.  In any
	     case, don't decide that here, just set 
	     ecs->stepping_over_breakpoint, making sure we 
	     single-step again before breakpoints are re-inserted.  */
	  tss->stepping_over_breakpoint = 1;
	}
    }

  /* Look at the cause of the stop, and decide what to do.
     The alternatives are:
     1) stop_stepping and return; to really stop and return to the debugger,
     2) keep_going and return to start up again
     (set tss->stepping_over_breakpoint to 1 to single step once)
     3) set ecs->random_signal to 1, and the decision between 1 and 2
     will be made according to the signal handling tables.  */

  /* First, distinguish signals caused by the debugger from signals
     that have to do with the program's own actions.  Note that
     breakpoint insns may cause SIGTRAP or SIGILL or SIGEMT, depending
     on the operating system version.  Here we detect when a SIGILL or
     SIGEMT is really a breakpoint and change it to SIGTRAP.  We do
     something similar for SIGSEGV, since a SIGSEGV will be generated
     when we're trying to execute a breakpoint instruction on a
     non-executable stack.  This happens for call dummy breakpoints
     for architectures like SPARC that place call dummies on the
     stack.

     If we're doing a displaced step past a breakpoint, then the
     breakpoint is always inserted at the original instruction;
     non-standard signals can't be explained by the breakpoint.  */
  if (stop_signal == TARGET_SIGNAL_TRAP
      || (! stepping_over_breakpoint
          && breakpoint_inserted_here_p (stop_pc)
	  && (stop_signal == TARGET_SIGNAL_ILL
	      || stop_signal == TARGET_SIGNAL_SEGV
	      || stop_signal == TARGET_SIGNAL_EMT))
      || stop_soon == STOP_QUIETLY || stop_soon == STOP_QUIETLY_NO_SIGSTOP
      || stop_soon == STOP_QUIETLY_REMOTE)
    {
      if (stop_signal == TARGET_SIGNAL_TRAP && stop_after_trap)
	{
          if (debug_infrun)
	    fprintf_unfiltered (gdb_stdlog, "infrun: stopped\n");
	  stop_print_frame = 0;
	  stop_stepping (ecs);
	  return;
	}

      /* This is originated from start_remote(), start_inferior() and
         shared libraries hook functions.  */
      if (stop_soon == STOP_QUIETLY || stop_soon == STOP_QUIETLY_REMOTE)
	{
          if (debug_infrun)
	    fprintf_unfiltered (gdb_stdlog, "infrun: quietly stopped\n");
	  stop_stepping (ecs);
	  return;
	}

      /* This originates from attach_command().  We need to overwrite
	 the stop_signal here, because some kernels don't ignore a
	 SIGSTOP in a subsequent ptrace(PTRACE_CONT,SIGSTOP) call.
	 See more comments in inferior.h.  On the other hand, if we
	 get a non-SIGSTOP, report it to the user - assume the backend
	 will handle the SIGSTOP if it should show up later.

	 Also consider that the attach is complete when we see a
	 SIGTRAP.  Some systems (e.g. Windows), and stubs supporting
	 target extended-remote report it instead of a SIGSTOP
	 (e.g. gdbserver).  We already rely on SIGTRAP being our
	 signal, so this is no exception.  */
      if (stop_soon == STOP_QUIETLY_NO_SIGSTOP
	  && (stop_signal == TARGET_SIGNAL_STOP
	      || stop_signal == TARGET_SIGNAL_TRAP))
	{
	  stop_stepping (ecs);
	  stop_signal = TARGET_SIGNAL_0;
	  return;
	}

      /* See if there is a breakpoint at the current PC.  */
      stop_bpstat = bpstat_stop_status (stop_pc, ecs->ptid);
      
      /* Following in case break condition called a
	 function.  */
      stop_print_frame = 1;

      /* NOTE: cagney/2003-03-29: These two checks for a random signal
         at one stage in the past included checks for an inferior
         function call's call dummy's return breakpoint.  The original
         comment, that went with the test, read:

         ``End of a stack dummy.  Some systems (e.g. Sony news) give
         another signal besides SIGTRAP, so check here as well as
         above.''

         If someone ever tries to get get call dummys on a
         non-executable stack to work (where the target would stop
         with something like a SIGSEGV), then those tests might need
         to be re-instated.  Given, however, that the tests were only
         enabled when momentary breakpoints were not being used, I
         suspect that it won't be the case.

         NOTE: kettenis/2004-02-05: Indeed such checks don't seem to
         be necessary for call dummies on a non-executable stack on
         SPARC.  */

      if (stop_signal == TARGET_SIGNAL_TRAP)
	ecs->random_signal
	  = !(bpstat_explains_signal (stop_bpstat)
	      || stepping_over_breakpoint
	      || (step_range_end && step_resume_breakpoint == NULL));
      else
	{
	  ecs->random_signal = !bpstat_explains_signal (stop_bpstat);
	  if (!ecs->random_signal)
	    stop_signal = TARGET_SIGNAL_TRAP;
	}
    }

  /* When we reach this point, we've pretty much decided
     that the reason for stopping must've been a random
     (unexpected) signal. */

  else
    ecs->random_signal = 1;

process_event_stop_test:
  /* For the program's own signals, act according to
     the signal handling tables.  */

  if (ecs->random_signal)
    {
      /* Signal not for debugging purposes.  */
      int printed = 0;

      if (debug_infrun)
	 fprintf_unfiltered (gdb_stdlog, "infrun: random signal %d\n", stop_signal);

      stopped_by_random_signal = 1;

      if (signal_print[stop_signal])
	{
	  printed = 1;
	  target_terminal_ours_for_output ();
	  print_stop_reason (SIGNAL_RECEIVED, stop_signal);
	}
      if (signal_stop_state (stop_signal))
	{
	  stop_stepping (ecs);
	  return;
	}
      /* If not going to stop, give terminal back
         if we took it away.  */
      else if (printed)
	target_terminal_inferior ();

      /* Clear the signal if it should not be passed.  */
      if (signal_program[stop_signal] == 0)
	stop_signal = TARGET_SIGNAL_0;

      if (prev_pc == read_pc ()
	  && stepping_over_breakpoint
	  && step_resume_breakpoint == NULL)
	{
	  /* We were just starting a new sequence, attempting to
	     single-step off of a breakpoint and expecting a SIGTRAP.
	     Instead this signal arrives.  This signal will take us out
	     of the stepping range so GDB needs to remember to, when
	     the signal handler returns, resume stepping off that
	     breakpoint.  */
	  /* To simplify things, "continue" is forced to use the same
	     code paths as single-step - set a breakpoint at the
	     signal return address and then, once hit, step off that
	     breakpoint.  */
          if (debug_infrun)
            fprintf_unfiltered (gdb_stdlog,
                                "infrun: signal arrived while stepping over "
                                "breakpoint\n");

	  insert_step_resume_breakpoint_at_frame (get_current_frame ());
	  tss->step_after_step_resume_breakpoint = 1;
	  keep_going (ecs);
	  return;
	}

      if (step_range_end != 0
	  && stop_signal != TARGET_SIGNAL_0
	  && stop_pc >= step_range_start && stop_pc < step_range_end
	  && frame_id_eq (get_frame_id (get_current_frame ()),
			  step_frame_id)
	  && step_resume_breakpoint == NULL)
	{
	  /* The inferior is about to take a signal that will take it
	     out of the single step range.  Set a breakpoint at the
	     current PC (which is presumably where the signal handler
	     will eventually return) and then allow the inferior to
	     run free.

	     Note that this is only needed for a signal delivered
	     while in the single-step range.  Nested signals aren't a
	     problem as they eventually all return.  */
          if (debug_infrun)
            fprintf_unfiltered (gdb_stdlog,
                                "infrun: signal may take us out of "
                                "single-step range\n");

	  insert_step_resume_breakpoint_at_frame (get_current_frame ());
	  keep_going (ecs);
	  return;
	}

      /* Note: step_resume_breakpoint may be non-NULL.  This occures
	 when either there's a nested signal, or when there's a
	 pending signal enabled just as the signal handler returns
	 (leaving the inferior at the step-resume-breakpoint without
	 actually executing it).  Either way continue until the
	 breakpoint is really hit.  */
      keep_going (ecs);
      return;
    }

  /* Handle cases caused by hitting a breakpoint.  */
  {
    CORE_ADDR jmp_buf_pc;
    struct bpstat_what what;

    what = bpstat_what (stop_bpstat);

    if (what.call_dummy)
      {
	stop_stack_dummy = 1;
      }

    switch (what.main_action)
      {
      case BPSTAT_WHAT_SET_LONGJMP_RESUME:
	/* If we hit the breakpoint at longjmp while stepping, we
	   install a momentary breakpoint at the target of the
	   jmp_buf.  */

	if (debug_infrun)
	  fprintf_unfiltered (gdb_stdlog,
			      "infrun: BPSTAT_WHAT_SET_LONGJMP_RESUME\n");

	tss->stepping_over_breakpoint = 1;

	if (!gdbarch_get_longjmp_target_p (current_gdbarch)
	    || !gdbarch_get_longjmp_target (current_gdbarch,
					    get_current_frame (), &jmp_buf_pc))
	  {
	    if (debug_infrun)
	      fprintf_unfiltered (gdb_stdlog, "\
infrun: BPSTAT_WHAT_SET_LONGJMP_RESUME (!gdbarch_get_longjmp_target)\n");
	    keep_going (ecs);
	    return;
	  }

	/* We're going to replace the current step-resume breakpoint
	   with a longjmp-resume breakpoint.  */
	if (step_resume_breakpoint != NULL)
	  delete_step_resume_breakpoint (&step_resume_breakpoint);

	/* Insert a breakpoint at resume address.  */
	insert_longjmp_resume_breakpoint (jmp_buf_pc);

	keep_going (ecs);
	return;

      case BPSTAT_WHAT_CLEAR_LONGJMP_RESUME:
        if (debug_infrun)
	  fprintf_unfiltered (gdb_stdlog,
			      "infrun: BPSTAT_WHAT_CLEAR_LONGJMP_RESUME\n");

	gdb_assert (step_resume_breakpoint != NULL);
	delete_step_resume_breakpoint (&step_resume_breakpoint);

	stop_step = 1;
	print_stop_reason (END_STEPPING_RANGE, 0);
	stop_stepping (ecs);
	return;

      case BPSTAT_WHAT_SINGLE:
        if (debug_infrun)
	  fprintf_unfiltered (gdb_stdlog, "infrun: BPSTAT_WHAT_SINGLE\n");
	tss->stepping_over_breakpoint = 1;
	/* Still need to check other stuff, at least the case
	   where we are stepping and step out of the right range.  */
	break;

      case BPSTAT_WHAT_STOP_NOISY:
        if (debug_infrun)
	  fprintf_unfiltered (gdb_stdlog, "infrun: BPSTAT_WHAT_STOP_NOISY\n");
	stop_print_frame = 1;

	/* We are about to nuke the step_resume_breakpointt via the
	   cleanup chain, so no need to worry about it here.  */

	stop_stepping (ecs);
	return;

      case BPSTAT_WHAT_STOP_SILENT:
        if (debug_infrun)
	  fprintf_unfiltered (gdb_stdlog, "infrun: BPSTAT_WHAT_STOP_SILENT\n");
	stop_print_frame = 0;

	/* We are about to nuke the step_resume_breakpoin via the
	   cleanup chain, so no need to worry about it here.  */

	stop_stepping (ecs);
	return;

      case BPSTAT_WHAT_STEP_RESUME:
	/* This proably demands a more elegant solution, but, yeah
	   right...

	   This function's use of the simple variable
	   step_resume_breakpoint doesn't seem to accomodate
	   simultaneously active step-resume bp's, although the
	   breakpoint list certainly can.

	   If we reach here and step_resume_breakpoint is already
	   NULL, then apparently we have multiple active
	   step-resume bp's.  We'll just delete the breakpoint we
	   stopped at, and carry on.  

	   Correction: what the code currently does is delete a
	   step-resume bp, but it makes no effort to ensure that
	   the one deleted is the one currently stopped at.  MVS  */

        if (debug_infrun)
	  fprintf_unfiltered (gdb_stdlog, "infrun: BPSTAT_WHAT_STEP_RESUME\n");

	if (step_resume_breakpoint == NULL)
	  {
	    step_resume_breakpoint =
	      bpstat_find_step_resume_breakpoint (stop_bpstat);
	  }
	delete_step_resume_breakpoint (&step_resume_breakpoint);
	if (tss->step_after_step_resume_breakpoint)
	  {
	    /* Back when the step-resume breakpoint was inserted, we
	       were trying to single-step off a breakpoint.  Go back
	       to doing that.  */
	    tss->step_after_step_resume_breakpoint = 0;
	    tss->stepping_over_breakpoint = 1;
	    keep_going (ecs);
	    return;
	  }
	break;

      case BPSTAT_WHAT_CHECK_SHLIBS:
      case BPSTAT_WHAT_CHECK_SHLIBS_RESUME_FROM_HOOK:
	{
          if (debug_infrun)
	    fprintf_unfiltered (gdb_stdlog, "infrun: BPSTAT_WHAT_CHECK_SHLIBS\n");

	  /* Check for any newly added shared libraries if we're
	     supposed to be adding them automatically.  Switch
	     terminal for any messages produced by
	     breakpoint_re_set.  */
	  target_terminal_ours_for_output ();
	  /* NOTE: cagney/2003-11-25: Make certain that the target
	     stack's section table is kept up-to-date.  Architectures,
	     (e.g., PPC64), use the section table to perform
	     operations such as address => section name and hence
	     require the table to contain all sections (including
	     those found in shared libraries).  */
	  /* NOTE: cagney/2003-11-25: Pass current_target and not
	     exec_ops to SOLIB_ADD.  This is because current GDB is
	     only tooled to propagate section_table changes out from
	     the "current_target" (see target_resize_to_sections), and
	     not up from the exec stratum.  This, of course, isn't
	     right.  "infrun.c" should only interact with the
	     exec/process stratum, instead relying on the target stack
	     to propagate relevant changes (stop, section table
	     changed, ...) up to other layers.  */
#ifdef SOLIB_ADD
	  SOLIB_ADD (NULL, 0, &current_target, auto_solib_add);
#else
	  solib_add (NULL, 0, &current_target, auto_solib_add);
#endif
	  target_terminal_inferior ();

	  /* If requested, stop when the dynamic linker notifies
	     gdb of events.  This allows the user to get control
	     and place breakpoints in initializer routines for
	     dynamically loaded objects (among other things).  */
	  if (stop_on_solib_events || stop_stack_dummy)
	    {
	      stop_stepping (ecs);
	      return;
	    }

	  /* If we stopped due to an explicit catchpoint, then the
	     (see above) call to SOLIB_ADD pulled in any symbols
	     from a newly-loaded library, if appropriate.

	     We do want the inferior to stop, but not where it is
	     now, which is in the dynamic linker callback.  Rather,
	     we would like it stop in the user's program, just after
	     the call that caused this catchpoint to trigger.  That
	     gives the user a more useful vantage from which to
	     examine their program's state. */
	  else if (what.main_action
		   == BPSTAT_WHAT_CHECK_SHLIBS_RESUME_FROM_HOOK)
	    {
	      /* ??rehrauer: If I could figure out how to get the
	         right return PC from here, we could just set a temp
	         breakpoint and resume.  I'm not sure we can without
	         cracking open the dld's shared libraries and sniffing
	         their unwind tables and text/data ranges, and that's
	         not a terribly portable notion.

	         Until that time, we must step the inferior out of the
	         dld callback, and also out of the dld itself (and any
	         code or stubs in libdld.sl, such as "shl_load" and
	         friends) until we reach non-dld code.  At that point,
	         we can stop stepping. */
	      bpstat_get_triggered_catchpoints (stop_bpstat,
						&tss->
						stepping_through_solib_catchpoints);
	      tss->stepping_through_solib_after_catch = 1;

	      /* Be sure to lift all breakpoints, so the inferior does
	         actually step past this point... */
	      tss->stepping_over_breakpoint = 1;
	      break;
	    }
	  else
	    {
	      /* We want to step over this breakpoint, then keep going.  */
	      tss->stepping_over_breakpoint = 1;
	      break;
	    }
	}
	break;

      case BPSTAT_WHAT_LAST:
	/* Not a real code, but listed here to shut up gcc -Wall.  */

      case BPSTAT_WHAT_KEEP_CHECKING:
	break;
      }
  }

  /* We come here if we hit a breakpoint but should not
     stop for it.  Possibly we also were stepping
     and should stop for that.  So fall through and
     test for stepping.  But, if not stepping,
     do not stop.  */

  /* Are we stepping to get the inferior out of the dynamic linker's
     hook (and possibly the dld itself) after catching a shlib
     event?  */
  if (tss->stepping_through_solib_after_catch)
    {
#if defined(SOLIB_ADD)
      /* Have we reached our destination?  If not, keep going. */
      if (SOLIB_IN_DYNAMIC_LINKER (PIDGET (ecs->ptid), stop_pc))
	{
          if (debug_infrun)
	    fprintf_unfiltered (gdb_stdlog, "infrun: stepping in dynamic linker\n");
	  tss->stepping_over_breakpoint = 1;
	  keep_going (ecs);
	  return;
	}
#endif
      if (debug_infrun)
	 fprintf_unfiltered (gdb_stdlog, "infrun: step past dynamic linker\n");
      /* Else, stop and report the catchpoint(s) whose triggering
         caused us to begin stepping. */
      tss->stepping_through_solib_after_catch = 0;
      bpstat_clear (&stop_bpstat);
      stop_bpstat = bpstat_copy (tss->stepping_through_solib_catchpoints);
      bpstat_clear (&tss->stepping_through_solib_catchpoints);
      stop_print_frame = 1;
      stop_stepping (ecs);
      return;
    }

  if (step_resume_breakpoint)
    {
      if (debug_infrun)
	 fprintf_unfiltered (gdb_stdlog,
			     "infrun: step-resume breakpoint is inserted\n");

      /* Having a step-resume breakpoint overrides anything
         else having to do with stepping commands until
         that breakpoint is reached.  */
      keep_going (ecs);
      return;
    }

  if (step_range_end == 0)
    {
      if (debug_infrun)
	 fprintf_unfiltered (gdb_stdlog, "infrun: no stepping, continue\n");
      /* Likewise if we aren't even stepping.  */
      keep_going (ecs);
      return;
    }

  /* If stepping through a line, keep going if still within it.

     Note that step_range_end is the address of the first instruction
     beyond the step range, and NOT the address of the last instruction
     within it! */
  if (stop_pc >= step_range_start && stop_pc < step_range_end)
    {
      if (debug_infrun)
	 fprintf_unfiltered (gdb_stdlog, "infrun: stepping inside range [0x%s-0x%s]\n",
			    paddr_nz (step_range_start),
			    paddr_nz (step_range_end));
      keep_going (ecs);
      return;
    }

  /* We stepped out of the stepping range.  */

  /* If we are stepping at the source level and entered the runtime
     loader dynamic symbol resolution code, we keep on single stepping
     until we exit the run time loader code and reach the callee's
     address.  */
  if (step_over_calls == STEP_OVER_UNDEBUGGABLE
#ifdef IN_SOLIB_DYNSYM_RESOLVE_CODE
      && IN_SOLIB_DYNSYM_RESOLVE_CODE (stop_pc)
#else
      && in_solib_dynsym_resolve_code (stop_pc)
#endif
      )
    {
      CORE_ADDR pc_after_resolver =
	gdbarch_skip_solib_resolver (current_gdbarch, stop_pc);

      if (debug_infrun)
	 fprintf_unfiltered (gdb_stdlog, "infrun: stepped into dynsym resolve code\n");

      if (pc_after_resolver)
	{
	  /* Set up a step-resume breakpoint at the address
	     indicated by SKIP_SOLIB_RESOLVER.  */
	  struct symtab_and_line sr_sal;
	  init_sal (&sr_sal);
	  sr_sal.pc = pc_after_resolver;

	  insert_step_resume_breakpoint_at_sal (sr_sal, null_frame_id);
	}

      keep_going (ecs);
      return;
    }

  if (step_range_end != 1
      && (step_over_calls == STEP_OVER_UNDEBUGGABLE
	  || step_over_calls == STEP_OVER_ALL)
      && get_frame_type (get_current_frame ()) == SIGTRAMP_FRAME)
    {
      if (debug_infrun)
	 fprintf_unfiltered (gdb_stdlog, "infrun: stepped into signal trampoline\n");
      /* The inferior, while doing a "step" or "next", has ended up in
         a signal trampoline (either by a signal being delivered or by
         the signal handler returning).  Just single-step until the
         inferior leaves the trampoline (either by calling the handler
         or returning).  */
      keep_going (ecs);
      return;
    }

  /* Check for subroutine calls.  The check for the current frame
     equalling the step ID is not necessary - the check of the
     previous frame's ID is sufficient - but it is a common case and
     cheaper than checking the previous frame's ID.

     NOTE: frame_id_eq will never report two invalid frame IDs as
     being equal, so to get into this block, both the current and
     previous frame must have valid frame IDs.  */
  if (!frame_id_eq (get_frame_id (get_current_frame ()), step_frame_id)
      && frame_id_eq (frame_unwind_caller_id (get_current_frame ()),
		      step_frame_id))
    {
      CORE_ADDR real_stop_pc;

      if (debug_infrun)
	 fprintf_unfiltered (gdb_stdlog, "infrun: stepped into subroutine\n");

      if ((step_over_calls == STEP_OVER_NONE)
	  || ((step_range_end == 1)
	      && in_prologue (prev_pc, ecs->stop_func_start)))
	{
	  /* I presume that step_over_calls is only 0 when we're
	     supposed to be stepping at the assembly language level
	     ("stepi").  Just stop.  */
	  /* Also, maybe we just did a "nexti" inside a prolog, so we
	     thought it was a subroutine call but it was not.  Stop as
	     well.  FENN */
	  stop_step = 1;
	  print_stop_reason (END_STEPPING_RANGE, 0);
	  stop_stepping (ecs);
	  return;
	}

      if (step_over_calls == STEP_OVER_ALL)
	{
	  /* We're doing a "next", set a breakpoint at callee's return
	     address (the address at which the caller will
	     resume).  */
	  insert_step_resume_breakpoint_at_caller (get_current_frame ());
	  keep_going (ecs);
	  return;
	}

      /* If we are in a function call trampoline (a stub between the
         calling routine and the real function), locate the real
         function.  That's what tells us (a) whether we want to step
         into it at all, and (b) what prologue we want to run to the
         end of, if we do step into it.  */
      real_stop_pc = skip_language_trampoline (get_current_frame (), stop_pc);
      if (real_stop_pc == 0)
	real_stop_pc = gdbarch_skip_trampoline_code
			 (current_gdbarch, get_current_frame (), stop_pc);
      if (real_stop_pc != 0)
	ecs->stop_func_start = real_stop_pc;

      if (
#ifdef IN_SOLIB_DYNSYM_RESOLVE_CODE
	  IN_SOLIB_DYNSYM_RESOLVE_CODE (ecs->stop_func_start)
#else
	  in_solib_dynsym_resolve_code (ecs->stop_func_start)
#endif
)
	{
	  struct symtab_and_line sr_sal;
	  init_sal (&sr_sal);
	  sr_sal.pc = ecs->stop_func_start;

	  insert_step_resume_breakpoint_at_sal (sr_sal, null_frame_id);
	  keep_going (ecs);
	  return;
	}

      /* If we have line number information for the function we are
         thinking of stepping into, step into it.

         If there are several symtabs at that PC (e.g. with include
         files), just want to know whether *any* of them have line
         numbers.  find_pc_line handles this.  */
      {
	struct symtab_and_line tmp_sal;

	tmp_sal = find_pc_line (ecs->stop_func_start, 0);
	if (tmp_sal.line != 0)
	  {
	    step_into_function (ecs);
	    return;
	  }
      }

      /* If we have no line number and the step-stop-if-no-debug is
         set, we stop the step so that the user has a chance to switch
         in assembly mode.  */
      if (step_over_calls == STEP_OVER_UNDEBUGGABLE && step_stop_if_no_debug)
	{
	  stop_step = 1;
	  print_stop_reason (END_STEPPING_RANGE, 0);
	  stop_stepping (ecs);
	  return;
	}

      /* Set a breakpoint at callee's return address (the address at
         which the caller will resume).  */
      insert_step_resume_breakpoint_at_caller (get_current_frame ());
      keep_going (ecs);
      return;
    }

  /* If we're in the return path from a shared library trampoline,
     we want to proceed through the trampoline when stepping.  */
  if (gdbarch_in_solib_return_trampoline (current_gdbarch,
					  stop_pc, ecs->stop_func_name))
    {
      /* Determine where this trampoline returns.  */
      CORE_ADDR real_stop_pc;
      real_stop_pc = gdbarch_skip_trampoline_code
		       (current_gdbarch, get_current_frame (), stop_pc);

      if (debug_infrun)
	 fprintf_unfiltered (gdb_stdlog, "infrun: stepped into solib return tramp\n");

      /* Only proceed through if we know where it's going.  */
      if (real_stop_pc)
	{
	  /* And put the step-breakpoint there and go until there. */
	  struct symtab_and_line sr_sal;

	  init_sal (&sr_sal);	/* initialize to zeroes */
	  sr_sal.pc = real_stop_pc;
	  sr_sal.section = find_pc_overlay (sr_sal.pc);

	  /* Do not specify what the fp should be when we stop since
	     on some machines the prologue is where the new fp value
	     is established.  */
	  insert_step_resume_breakpoint_at_sal (sr_sal, null_frame_id);

	  /* Restart without fiddling with the step ranges or
	     other state.  */
	  keep_going (ecs);
	  return;
	}
    }

  stop_pc_sal = find_pc_line (stop_pc, 0);

  /* NOTE: tausq/2004-05-24: This if block used to be done before all
     the trampoline processing logic, however, there are some trampolines 
     that have no names, so we should do trampoline handling first.  */
  if (step_over_calls == STEP_OVER_UNDEBUGGABLE
      && ecs->stop_func_name == NULL
      && stop_pc_sal.line == 0)
    {
      if (debug_infrun)
	 fprintf_unfiltered (gdb_stdlog, "infrun: stepped into undebuggable function\n");

      /* The inferior just stepped into, or returned to, an
         undebuggable function (where there is no debugging information
         and no line number corresponding to the address where the
         inferior stopped).  Since we want to skip this kind of code,
         we keep going until the inferior returns from this
         function - unless the user has asked us not to (via
         set step-mode) or we no longer know how to get back
         to the call site.  */
      if (step_stop_if_no_debug
	  || !frame_id_p (frame_unwind_caller_id (get_current_frame ())))
	{
	  /* If we have no line number and the step-stop-if-no-debug
	     is set, we stop the step so that the user has a chance to
	     switch in assembly mode.  */
	  stop_step = 1;
	  print_stop_reason (END_STEPPING_RANGE, 0);
	  stop_stepping (ecs);
	  return;
	}
      else
	{
	  /* Set a breakpoint at callee's return address (the address
	     at which the caller will resume).  */
	  insert_step_resume_breakpoint_at_caller (get_current_frame ());
	  keep_going (ecs);
	  return;
	}
    }

  if (step_range_end == 1)
    {
      /* It is stepi or nexti.  We always want to stop stepping after
         one instruction.  */
      if (debug_infrun)
	 fprintf_unfiltered (gdb_stdlog, "infrun: stepi/nexti\n");
      stop_step = 1;
      print_stop_reason (END_STEPPING_RANGE, 0);
      stop_stepping (ecs);
      return;
    }

  if (stop_pc_sal.line == 0)
    {
      /* We have no line number information.  That means to stop
         stepping (does this always happen right after one instruction,
         when we do "s" in a function with no line numbers,
         or can this happen as a result of a return or longjmp?).  */
      if (debug_infrun)
	 fprintf_unfiltered (gdb_stdlog, "infrun: no line number info\n");
      stop_step = 1;
      print_stop_reason (END_STEPPING_RANGE, 0);
      stop_stepping (ecs);
      return;
    }

  if ((stop_pc == stop_pc_sal.pc)
      && (tss->current_line != stop_pc_sal.line
	  || tss->current_symtab != stop_pc_sal.symtab))
    {
      /* We are at the start of a different line.  So stop.  Note that
         we don't stop if we step into the middle of a different line.
         That is said to make things like for (;;) statements work
         better.  */
      if (debug_infrun)
	 fprintf_unfiltered (gdb_stdlog, "infrun: stepped to a different line\n");
      stop_step = 1;
      print_stop_reason (END_STEPPING_RANGE, 0);
      stop_stepping (ecs);
      return;
    }

  /* We aren't done stepping.

     Optimize by setting the stepping range to the line.
     (We might not be in the original line, but if we entered a
     new line in mid-statement, we continue stepping.  This makes
     things like for(;;) statements work better.)  */

  step_range_start = stop_pc_sal.pc;
  step_range_end = stop_pc_sal.end;
  step_frame_id = get_frame_id (get_current_frame ());
  tss->current_line = stop_pc_sal.line;
  tss->current_symtab = stop_pc_sal.symtab;

  /* In the case where we just stepped out of a function into the
     middle of a line of the caller, continue stepping, but
     step_frame_id must be modified to current frame */
#if 0
  /* NOTE: cagney/2003-10-16: I think this frame ID inner test is too
     generous.  It will trigger on things like a step into a frameless
     stackless leaf function.  I think the logic should instead look
     at the unwound frame ID has that should give a more robust
     indication of what happened.  */
  if (step - ID == current - ID)
    still stepping in same function;
  else if (step - ID == unwind (current - ID))
    stepped into a function;
  else
    stepped out of a function;
  /* Of course this assumes that the frame ID unwind code is robust
     and we're willing to introduce frame unwind logic into this
     function.  Fortunately, those days are nearly upon us.  */
#endif
  {
    struct frame_info *frame = get_current_frame ();
    struct frame_id current_frame = get_frame_id (frame);
    if (!(frame_id_inner (get_frame_arch (frame), current_frame,
			  step_frame_id)))
      step_frame_id = current_frame;
  }

  if (debug_infrun)
     fprintf_unfiltered (gdb_stdlog, "infrun: keep going\n");
  keep_going (ecs);
}

/* Are we in the middle of stepping?  */

static int
currently_stepping (struct thread_stepping_state *tss)
{
  return (((step_range_end && step_resume_breakpoint == NULL)
	   || stepping_over_breakpoint)
	  || tss->stepping_through_solib_after_catch
	  || bpstat_should_step ());
}

/* Subroutine call with source code we should not step over.  Do step
   to the first line of code in it.  */

static void
step_into_function (struct execution_control_state *ecs)
{
  struct symtab *s;
  struct symtab_and_line stop_func_sal, sr_sal;

  s = find_pc_symtab (stop_pc);
  if (s && s->language != language_asm)
    ecs->stop_func_start = gdbarch_skip_prologue
			     (current_gdbarch, ecs->stop_func_start);

  stop_func_sal = find_pc_line (ecs->stop_func_start, 0);
  /* Use the step_resume_break to step until the end of the prologue,
     even if that involves jumps (as it seems to on the vax under
     4.2).  */
  /* If the prologue ends in the middle of a source line, continue to
     the end of that source line (if it is still within the function).
     Otherwise, just go to end of prologue.  */
  if (stop_func_sal.end
      && stop_func_sal.pc != ecs->stop_func_start
      && stop_func_sal.end < ecs->stop_func_end)
    ecs->stop_func_start = stop_func_sal.end;

  /* Architectures which require breakpoint adjustment might not be able
     to place a breakpoint at the computed address.  If so, the test
     ``ecs->stop_func_start == stop_pc'' will never succeed.  Adjust
     ecs->stop_func_start to an address at which a breakpoint may be
     legitimately placed.

     Note:  kevinb/2004-01-19:  On FR-V, if this adjustment is not
     made, GDB will enter an infinite loop when stepping through
     optimized code consisting of VLIW instructions which contain
     subinstructions corresponding to different source lines.  On
     FR-V, it's not permitted to place a breakpoint on any but the
     first subinstruction of a VLIW instruction.  When a breakpoint is
     set, GDB will adjust the breakpoint address to the beginning of
     the VLIW instruction.  Thus, we need to make the corresponding
     adjustment here when computing the stop address.  */

  if (gdbarch_adjust_breakpoint_address_p (current_gdbarch))
    {
      ecs->stop_func_start
	= gdbarch_adjust_breakpoint_address (current_gdbarch,
					     ecs->stop_func_start);
    }

  if (ecs->stop_func_start == stop_pc)
    {
      /* We are already there: stop now.  */
      stop_step = 1;
      print_stop_reason (END_STEPPING_RANGE, 0);
      stop_stepping (ecs);
      return;
    }
  else
    {
      /* Put the step-breakpoint there and go until there.  */
      init_sal (&sr_sal);	/* initialize to zeroes */
      sr_sal.pc = ecs->stop_func_start;
      sr_sal.section = find_pc_overlay (ecs->stop_func_start);

      /* Do not specify what the fp should be when we stop since on
         some machines the prologue is where the new fp value is
         established.  */
      insert_step_resume_breakpoint_at_sal (sr_sal, null_frame_id);

      /* And make sure stepping stops right away then.  */
      step_range_end = step_range_start;
    }
  keep_going (ecs);
}

/* Insert a "step-resume breakpoint" at SR_SAL with frame ID SR_ID.
   This is used to both functions and to skip over code.  */

static void
insert_step_resume_breakpoint_at_sal (struct symtab_and_line sr_sal,
				      struct frame_id sr_id)
{
  /* There should never be more than one step-resume or longjmp-resume
     breakpoint per thread, so we should never be setting a new
     step_resume_breakpoint when one is already active.  */
  gdb_assert (step_resume_breakpoint == NULL);

  if (debug_infrun)
    fprintf_unfiltered (gdb_stdlog,
			"infrun: inserting step-resume breakpoint at 0x%s\n",
			paddr_nz (sr_sal.pc));

  step_resume_breakpoint = set_momentary_breakpoint (sr_sal, sr_id,
						     bp_step_resume);
}

/* Insert a "step-resume breakpoint" at RETURN_FRAME.pc.  This is used
   to skip a potential signal handler.

   This is called with the interrupted function's frame.  The signal
   handler, when it returns, will resume the interrupted function at
   RETURN_FRAME.pc.  */

static void
insert_step_resume_breakpoint_at_frame (struct frame_info *return_frame)
{
  struct symtab_and_line sr_sal;

  gdb_assert (return_frame != NULL);
  init_sal (&sr_sal);		/* initialize to zeros */

  sr_sal.pc = gdbarch_addr_bits_remove
		(current_gdbarch, get_frame_pc (return_frame));
  sr_sal.section = find_pc_overlay (sr_sal.pc);

  insert_step_resume_breakpoint_at_sal (sr_sal, get_frame_id (return_frame));
}

/* Similar to insert_step_resume_breakpoint_at_frame, except
   but a breakpoint at the previous frame's PC.  This is used to
   skip a function after stepping into it (for "next" or if the called
   function has no debugging information).

   The current function has almost always been reached by single
   stepping a call or return instruction.  NEXT_FRAME belongs to the
   current function, and the breakpoint will be set at the caller's
   resume address.

   This is a separate function rather than reusing
   insert_step_resume_breakpoint_at_frame in order to avoid
   get_prev_frame, which may stop prematurely (see the implementation
   of frame_unwind_caller_id for an example).  */

static void
insert_step_resume_breakpoint_at_caller (struct frame_info *next_frame)
{
  struct symtab_and_line sr_sal;

  /* We shouldn't have gotten here if we don't know where the call site
     is.  */
  gdb_assert (frame_id_p (frame_unwind_caller_id (next_frame)));

  init_sal (&sr_sal);		/* initialize to zeros */

  sr_sal.pc = gdbarch_addr_bits_remove
		(current_gdbarch, frame_unwind_caller_pc (next_frame));
  sr_sal.section = find_pc_overlay (sr_sal.pc);

  insert_step_resume_breakpoint_at_sal (sr_sal,
					frame_unwind_caller_id (next_frame));
}

/* Insert a "longjmp-resume" breakpoint at PC.  This is used to set a
   new breakpoint at the target of a jmp_buf.  The handling of
   longjmp-resume uses the same mechanisms used for handling
   "step-resume" breakpoints.  */

static void
insert_longjmp_resume_breakpoint (CORE_ADDR pc)
{
  /* There should never be more than one step-resume or longjmp-resume
     breakpoint per thread, so we should never be setting a new
     longjmp_resume_breakpoint when one is already active.  */
  gdb_assert (step_resume_breakpoint == NULL);

  if (debug_infrun)
    fprintf_unfiltered (gdb_stdlog,
			"infrun: inserting longjmp-resume breakpoint at 0x%s\n",
			paddr_nz (pc));

  step_resume_breakpoint =
    set_momentary_breakpoint_at_pc (pc, bp_longjmp_resume);
}

static void
stop_stepping (struct execution_control_state *ecs)
{
  if (debug_infrun)
    fprintf_unfiltered (gdb_stdlog, "infrun: stop_stepping\n");

  /* Let callers know we don't want to wait for the inferior anymore.  */
  ecs->wait_some_more = 0;
}

/* This function handles various cases where we need to continue
   waiting for the inferior.  */
/* (Used to be the keep_going: label in the old wait_for_inferior) */

static void
keep_going (struct execution_control_state *ecs)
{
  /* Save the pc before execution, to compare with pc after stop.  */
  prev_pc = read_pc ();		/* Might have been DECR_AFTER_BREAK */

  /* If we did not do break;, it means we should keep running the
     inferior and not return to debugger.  */

  if (stepping_over_breakpoint && stop_signal != TARGET_SIGNAL_TRAP)
    {
      /* We took a signal (which we are supposed to pass through to
         the inferior, else we'd have done a break above) and we
         haven't yet gotten our trap.  Simply continue.  */
      resume (currently_stepping (tss), stop_signal);
    }
  else
    {
      /* Either the trap was not expected, but we are continuing
         anyway (the user asked that this signal be passed to the
         child)
         -- or --
         The signal was SIGTRAP, e.g. it was our signal, but we
         decided we should resume from it.

         We're going to run this baby now!  

	 Note that insert_breakpoints won't try to re-insert
	 already inserted breakpoints.  Therefore, we don't
	 care if breakpoints were already inserted, or not.  */
      
      if (tss->stepping_over_breakpoint)
	{
	  if (! use_displaced_stepping (current_gdbarch))
	    /* Since we can't do a displaced step, we have to remove
	       the breakpoint while we step it.  To keep things
	       simple, we remove them all.  */
	    remove_breakpoints ();
	}
      else
	{
	  struct gdb_exception e;
	  /* Stop stepping when inserting breakpoints
	     has failed.  */
	  TRY_CATCH (e, RETURN_MASK_ERROR)
	    {
	      insert_breakpoints ();
	    }
	  if (e.reason < 0)
	    {
	      stop_stepping (ecs);
	      return;
	    }
	}

      stepping_over_breakpoint = tss->stepping_over_breakpoint;

      /* Do not deliver SIGNAL_TRAP (except when the user explicitly
         specifies that such a signal should be delivered to the
         target program).

         Typically, this would occure when a user is debugging a
         target monitor on a simulator: the target monitor sets a
         breakpoint; the simulator encounters this break-point and
         halts the simulation handing control to GDB; GDB, noteing
         that the break-point isn't valid, returns control back to the
         simulator; the simulator then delivers the hardware
         equivalent of a SIGNAL_TRAP to the program being debugged. */

      if (stop_signal == TARGET_SIGNAL_TRAP && !signal_program[stop_signal])
	stop_signal = TARGET_SIGNAL_0;


      resume (currently_stepping (tss), stop_signal);
    }

  prepare_to_wait (ecs);
}

/* This function normally comes after a resume, before
   handle_inferior_event exits.  It takes care of any last bits of
   housekeeping, and sets the all-important wait_some_more flag.  */

static void
prepare_to_wait (struct execution_control_state *ecs)
{
  if (debug_infrun)
    fprintf_unfiltered (gdb_stdlog, "infrun: prepare_to_wait\n");
  if (infwait_state == infwait_normal_state)
    {
      overlay_cache_invalid = 1;

      /* We have to invalidate the registers BEFORE calling
         target_wait because they can be loaded from the target while
         in target_wait.  This makes remote debugging a bit more
         efficient for those targets that provide critical registers
         as part of their normal status mechanism. */

      registers_changed ();
      waiton_ptid = pid_to_ptid (-1);
    }
  /* This is the old end of the while loop.  Let everybody know we
     want to wait for the inferior some more and get called again
     soon.  */
  ecs->wait_some_more = 1;
}

/* Print why the inferior has stopped. We always print something when
   the inferior exits, or receives a signal. The rest of the cases are
   dealt with later on in normal_stop() and print_it_typical().  Ideally
   there should be a call to this function from handle_inferior_event()
   each time stop_stepping() is called.*/
static void
print_stop_reason (enum inferior_stop_reason stop_reason, int stop_info)
{
  switch (stop_reason)
    {
    case END_STEPPING_RANGE:
      /* We are done with a step/next/si/ni command. */
      /* For now print nothing. */
      /* Print a message only if not in the middle of doing a "step n"
         operation for n > 1 */
      if (!step_multi || !stop_step)
	if (ui_out_is_mi_like_p (uiout))
	  ui_out_field_string
	    (uiout, "reason",
	     async_reason_lookup (EXEC_ASYNC_END_STEPPING_RANGE));
      break;
    case SIGNAL_EXITED:
      /* The inferior was terminated by a signal. */
      annotate_signalled ();
      if (ui_out_is_mi_like_p (uiout))
	ui_out_field_string
	  (uiout, "reason",
	   async_reason_lookup (EXEC_ASYNC_EXITED_SIGNALLED));
      ui_out_text (uiout, "\nProgram terminated with signal ");
      annotate_signal_name ();
      ui_out_field_string (uiout, "signal-name",
			   target_signal_to_name (stop_info));
      annotate_signal_name_end ();
      ui_out_text (uiout, ", ");
      annotate_signal_string ();
      ui_out_field_string (uiout, "signal-meaning",
			   target_signal_to_string (stop_info));
      annotate_signal_string_end ();
      ui_out_text (uiout, ".\n");
      ui_out_text (uiout, "The program no longer exists.\n");
      break;
    case EXITED:
      /* The inferior program is finished. */
      annotate_exited (stop_info);
      if (stop_info)
	{
	  if (ui_out_is_mi_like_p (uiout))
	    ui_out_field_string (uiout, "reason", 
				 async_reason_lookup (EXEC_ASYNC_EXITED));
	  ui_out_text (uiout, "\nProgram exited with code ");
	  ui_out_field_fmt (uiout, "exit-code", "0%o",
			    (unsigned int) stop_info);
	  ui_out_text (uiout, ".\n");
	}
      else
	{
	  if (ui_out_is_mi_like_p (uiout))
	    ui_out_field_string
	      (uiout, "reason",
	       async_reason_lookup (EXEC_ASYNC_EXITED_NORMALLY));
	  ui_out_text (uiout, "\nProgram exited normally.\n");
	}
      /* Support the --return-child-result option.  */
      return_child_result_value = stop_info;
      break;
    case SIGNAL_RECEIVED:
      /* Signal received. The signal table tells us to print about
         it. */
      annotate_signal ();
      ui_out_text (uiout, "\nProgram received signal ");
      annotate_signal_name ();
      if (ui_out_is_mi_like_p (uiout))
	ui_out_field_string
	  (uiout, "reason", async_reason_lookup (EXEC_ASYNC_SIGNAL_RECEIVED));
      ui_out_field_string (uiout, "signal-name",
			   target_signal_to_name (stop_info));
      annotate_signal_name_end ();
      ui_out_text (uiout, ", ");
      annotate_signal_string ();
      ui_out_field_string (uiout, "signal-meaning",
			   target_signal_to_string (stop_info));
      annotate_signal_string_end ();
      ui_out_text (uiout, ".\n");
      break;
    default:
      internal_error (__FILE__, __LINE__,
		      _("print_stop_reason: unrecognized enum value"));
      break;
    }
}


/* Here to return control to GDB when the inferior stops for real.
   Print appropriate messages, remove breakpoints, give terminal our modes.

   STOP_PRINT_FRAME nonzero means print the executing frame
   (pc, function, args, file, line number and line text).
   BREAKPOINTS_FAILED nonzero means stop was due to error
   attempting to insert breakpoints.  */

void
normal_stop (void)
{
  struct target_waitstatus last;
  ptid_t last_ptid;

  get_last_target_status (&last_ptid, &last);

  /* In non-stop mode, we don't want GDB to switch threads behind the
     user's back, to avoid races where the user is typing a command to
     apply to thread x, but GDB switches to thread y before the user
     finishes entering the command.  */

  /* As with the notification of thread events, we want to delay
     notifying the user that we've switched thread context until
     the inferior actually stops.

     There's no point in saying anything if the inferior has exited.
     Note that SIGNALLED here means "exited with a signal", not
     "received a signal".  */
  if (!non_stop
      && !ptid_equal (previous_inferior_ptid, inferior_ptid)
      && target_has_execution
      && last.kind != TARGET_WAITKIND_SIGNALLED
      && last.kind != TARGET_WAITKIND_EXITED)
    {
      target_terminal_ours_for_output ();
      printf_filtered (_("[Switching to %s]\n"),
		       target_pid_to_str (inferior_ptid));
      annotate_thread_changed ();
      previous_inferior_ptid = inferior_ptid;
    }

  /* NOTE drow/2004-01-17: Is this still necessary?  */
  /* Make sure that the current_frame's pc is correct.  This
     is a correction for setting up the frame info before doing
     gdbarch_decr_pc_after_break */
  if (target_has_execution)
    /* FIXME: cagney/2002-12-06: Has the PC changed?  Thanks to
       gdbarch_decr_pc_after_break, the program counter can change.  Ask the
       frame code to check for this and sort out any resultant mess.
       gdbarch_decr_pc_after_break needs to just go away.  */
    deprecated_update_frame_pc_hack (get_current_frame (), read_pc ());

  if (!breakpoints_always_inserted_mode () && target_has_execution)
    {
      if (remove_breakpoints ())
	{
	  target_terminal_ours_for_output ();
	  printf_filtered (_("\
Cannot remove breakpoints because program is no longer writable.\n\
It might be running in another process.\n\
Further execution is probably impossible.\n"));
	}
    }

  /* If an auto-display called a function and that got a signal,
     delete that auto-display to avoid an infinite recursion.  */

  if (stopped_by_random_signal)
    disable_current_display ();

  /* Don't print a message if in the middle of doing a "step n"
     operation for n > 1 */
  if (step_multi && stop_step)
    goto done;

  target_terminal_ours ();

  /* Set the current source location.  This will also happen if we
     display the frame below, but the current SAL will be incorrect
     during a user hook-stop function.  */
  if (target_has_stack && !stop_stack_dummy)
    set_current_sal_from_frame (get_current_frame (), 1);

  /* Look up the hook_stop and run it (CLI internally handles problem
     of stop_command's pre-hook not existing).  */
  if (stop_command)
    catch_errors (hook_stop_stub, stop_command,
		  "Error while running hook_stop:\n", RETURN_MASK_ALL);

  if (!target_has_stack)
    {

      goto done;
    }

  /* Select innermost stack frame - i.e., current frame is frame 0,
     and current location is based on that.
     Don't do this on return from a stack dummy routine,
     or if the program has exited. */

  if (!stop_stack_dummy)
    {
      select_frame (get_current_frame ());

      /* Print current location without a level number, if
         we have changed functions or hit a breakpoint.
         Print source line if we have one.
         bpstat_print() contains the logic deciding in detail
         what to print, based on the event(s) that just occurred. */

      /* If --batch-silent is enabled then there's no need to print the current
	 source location, and to try risks causing an error message about
	 missing source files.  */
      if (stop_print_frame && !batch_silent)
	{
	  int bpstat_ret;
	  int source_flag;
	  int do_frame_printing = 1;

	  bpstat_ret = bpstat_print (stop_bpstat);
	  switch (bpstat_ret)
	    {
	    case PRINT_UNKNOWN:
	      /* If we had hit a shared library event breakpoint,
		 bpstat_print would print out this message.  If we hit
		 an OS-level shared library event, do the same
		 thing.  */
	      if (last.kind == TARGET_WAITKIND_LOADED)
		{
		  printf_filtered (_("Stopped due to shared library event\n"));
		  source_flag = SRC_LINE;	/* something bogus */
		  do_frame_printing = 0;
		  break;
		}

	      /* FIXME: cagney/2002-12-01: Given that a frame ID does
	         (or should) carry around the function and does (or
	         should) use that when doing a frame comparison.  */
	      if (stop_step
		  && frame_id_eq (step_frame_id,
				  get_frame_id (get_current_frame ()))
		  && step_start_function == find_pc_function (stop_pc))
		source_flag = SRC_LINE;	/* finished step, just print source line */
	      else
		source_flag = SRC_AND_LOC;	/* print location and source line */
	      break;
	    case PRINT_SRC_AND_LOC:
	      source_flag = SRC_AND_LOC;	/* print location and source line */
	      break;
	    case PRINT_SRC_ONLY:
	      source_flag = SRC_LINE;
	      break;
	    case PRINT_NOTHING:
	      source_flag = SRC_LINE;	/* something bogus */
	      do_frame_printing = 0;
	      break;
	    default:
	      internal_error (__FILE__, __LINE__, _("Unknown value."));
	    }

	  if (ui_out_is_mi_like_p (uiout))
	    {

	      ui_out_field_int (uiout, "thread-id",
				pid_to_thread_id (inferior_ptid));
	      if (non_stop)
		{
		  struct cleanup *back_to = make_cleanup_ui_out_list_begin_end 
		    (uiout, "stopped-threads");
		  ui_out_field_int (uiout, NULL,
				    pid_to_thread_id (inferior_ptid));		  		  
		  do_cleanups (back_to);
		}
	      else
		ui_out_field_string (uiout, "stopped-threads", "all");
	    }
	  /* The behavior of this routine with respect to the source
	     flag is:
	     SRC_LINE: Print only source line
	     LOCATION: Print only location
	     SRC_AND_LOC: Print location and source line */
	  if (do_frame_printing)
	    print_stack_frame (get_selected_frame (NULL), 0, source_flag);

	  /* Display the auto-display expressions.  */
	  do_displays ();
	}
    }

  /* Save the function value return registers, if we care.
     We might be about to restore their previous contents.  */
  if (proceed_to_finish)
    {
      /* This should not be necessary.  */
      if (stop_registers)
	regcache_xfree (stop_registers);

      /* NB: The copy goes through to the target picking up the value of
	 all the registers.  */
      stop_registers = regcache_dup (get_current_regcache ());
    }

  if (stop_stack_dummy)
    {
      /* Pop the empty frame that contains the stack dummy.  POP_FRAME
         ends with a setting of the current frame, so we can use that
         next. */
      frame_pop (get_current_frame ());
      /* Set stop_pc to what it was before we called the function.
         Can't rely on restore_inferior_status because that only gets
         called if we don't stop in the called function.  */
      stop_pc = read_pc ();
      select_frame (get_current_frame ());
    }

done:
  annotate_stopped ();
  if (!suppress_stop_observer && !step_multi)
    observer_notify_normal_stop (stop_bpstat);
  /* Delete the breakpoint we stopped at, if it wants to be deleted.
     Delete any breakpoint that is to be deleted at the next stop.  */
  breakpoint_auto_delete (stop_bpstat);

  if (target_has_execution
      && last.kind != TARGET_WAITKIND_SIGNALLED
      && last.kind != TARGET_WAITKIND_EXITED)
    {
      if (!non_stop)
	set_running (pid_to_ptid (-1), 0);
      else
	set_running (inferior_ptid, 0);
    }
}

static int
hook_stop_stub (void *cmd)
{
  execute_cmd_pre_hook ((struct cmd_list_element *) cmd);
  return (0);
}

int
signal_stop_state (int signo)
{
  /* Always stop on signals if we're just gaining control of the
     program.  */
  return signal_stop[signo] || stop_soon != NO_STOP_QUIETLY;
}

int
signal_print_state (int signo)
{
  return signal_print[signo];
}

int
signal_pass_state (int signo)
{
  return signal_program[signo];
}

int
signal_stop_update (int signo, int state)
{
  int ret = signal_stop[signo];
  signal_stop[signo] = state;
  return ret;
}

int
signal_print_update (int signo, int state)
{
  int ret = signal_print[signo];
  signal_print[signo] = state;
  return ret;
}

int
signal_pass_update (int signo, int state)
{
  int ret = signal_program[signo];
  signal_program[signo] = state;
  return ret;
}

static void
sig_print_header (void)
{
  printf_filtered (_("\
Signal        Stop\tPrint\tPass to program\tDescription\n"));
}

static void
sig_print_info (enum target_signal oursig)
{
  char *name = target_signal_to_name (oursig);
  int name_padding = 13 - strlen (name);

  if (name_padding <= 0)
    name_padding = 0;

  printf_filtered ("%s", name);
  printf_filtered ("%*.*s ", name_padding, name_padding, "                 ");
  printf_filtered ("%s\t", signal_stop[oursig] ? "Yes" : "No");
  printf_filtered ("%s\t", signal_print[oursig] ? "Yes" : "No");
  printf_filtered ("%s\t\t", signal_program[oursig] ? "Yes" : "No");
  printf_filtered ("%s\n", target_signal_to_string (oursig));
}

/* Specify how various signals in the inferior should be handled.  */

static void
handle_command (char *args, int from_tty)
{
  char **argv;
  int digits, wordlen;
  int sigfirst, signum, siglast;
  enum target_signal oursig;
  int allsigs;
  int nsigs;
  unsigned char *sigs;
  struct cleanup *old_chain;

  if (args == NULL)
    {
      error_no_arg (_("signal to handle"));
    }

  /* Allocate and zero an array of flags for which signals to handle. */

  nsigs = (int) TARGET_SIGNAL_LAST;
  sigs = (unsigned char *) alloca (nsigs);
  memset (sigs, 0, nsigs);

  /* Break the command line up into args. */

  argv = buildargv (args);
  if (argv == NULL)
    {
      nomem (0);
    }
  old_chain = make_cleanup_freeargv (argv);

  /* Walk through the args, looking for signal oursigs, signal names, and
     actions.  Signal numbers and signal names may be interspersed with
     actions, with the actions being performed for all signals cumulatively
     specified.  Signal ranges can be specified as <LOW>-<HIGH>. */

  while (*argv != NULL)
    {
      wordlen = strlen (*argv);
      for (digits = 0; isdigit ((*argv)[digits]); digits++)
	{;
	}
      allsigs = 0;
      sigfirst = siglast = -1;

      if (wordlen >= 1 && !strncmp (*argv, "all", wordlen))
	{
	  /* Apply action to all signals except those used by the
	     debugger.  Silently skip those. */
	  allsigs = 1;
	  sigfirst = 0;
	  siglast = nsigs - 1;
	}
      else if (wordlen >= 1 && !strncmp (*argv, "stop", wordlen))
	{
	  SET_SIGS (nsigs, sigs, signal_stop);
	  SET_SIGS (nsigs, sigs, signal_print);
	}
      else if (wordlen >= 1 && !strncmp (*argv, "ignore", wordlen))
	{
	  UNSET_SIGS (nsigs, sigs, signal_program);
	}
      else if (wordlen >= 2 && !strncmp (*argv, "print", wordlen))
	{
	  SET_SIGS (nsigs, sigs, signal_print);
	}
      else if (wordlen >= 2 && !strncmp (*argv, "pass", wordlen))
	{
	  SET_SIGS (nsigs, sigs, signal_program);
	}
      else if (wordlen >= 3 && !strncmp (*argv, "nostop", wordlen))
	{
	  UNSET_SIGS (nsigs, sigs, signal_stop);
	}
      else if (wordlen >= 3 && !strncmp (*argv, "noignore", wordlen))
	{
	  SET_SIGS (nsigs, sigs, signal_program);
	}
      else if (wordlen >= 4 && !strncmp (*argv, "noprint", wordlen))
	{
	  UNSET_SIGS (nsigs, sigs, signal_print);
	  UNSET_SIGS (nsigs, sigs, signal_stop);
	}
      else if (wordlen >= 4 && !strncmp (*argv, "nopass", wordlen))
	{
	  UNSET_SIGS (nsigs, sigs, signal_program);
	}
      else if (digits > 0)
	{
	  /* It is numeric.  The numeric signal refers to our own
	     internal signal numbering from target.h, not to host/target
	     signal  number.  This is a feature; users really should be
	     using symbolic names anyway, and the common ones like
	     SIGHUP, SIGINT, SIGALRM, etc. will work right anyway.  */

	  sigfirst = siglast = (int)
	    target_signal_from_command (atoi (*argv));
	  if ((*argv)[digits] == '-')
	    {
	      siglast = (int)
		target_signal_from_command (atoi ((*argv) + digits + 1));
	    }
	  if (sigfirst > siglast)
	    {
	      /* Bet he didn't figure we'd think of this case... */
	      signum = sigfirst;
	      sigfirst = siglast;
	      siglast = signum;
	    }
	}
      else
	{
	  oursig = target_signal_from_name (*argv);
	  if (oursig != TARGET_SIGNAL_UNKNOWN)
	    {
	      sigfirst = siglast = (int) oursig;
	    }
	  else
	    {
	      /* Not a number and not a recognized flag word => complain.  */
	      error (_("Unrecognized or ambiguous flag word: \"%s\"."), *argv);
	    }
	}

      /* If any signal numbers or symbol names were found, set flags for
         which signals to apply actions to. */

      for (signum = sigfirst; signum >= 0 && signum <= siglast; signum++)
	{
	  switch ((enum target_signal) signum)
	    {
	    case TARGET_SIGNAL_TRAP:
	    case TARGET_SIGNAL_INT:
	      if (!allsigs && !sigs[signum])
		{
		  if (query ("%s is used by the debugger.\n\
Are you sure you want to change it? ", target_signal_to_name ((enum target_signal) signum)))
		    {
		      sigs[signum] = 1;
		    }
		  else
		    {
		      printf_unfiltered (_("Not confirmed, unchanged.\n"));
		      gdb_flush (gdb_stdout);
		    }
		}
	      break;
	    case TARGET_SIGNAL_0:
	    case TARGET_SIGNAL_DEFAULT:
	    case TARGET_SIGNAL_UNKNOWN:
	      /* Make sure that "all" doesn't print these.  */
	      break;
	    default:
	      sigs[signum] = 1;
	      break;
	    }
	}

      argv++;
    }

  target_notice_signals (inferior_ptid);

  if (from_tty)
    {
      /* Show the results.  */
      sig_print_header ();
      for (signum = 0; signum < nsigs; signum++)
	{
	  if (sigs[signum])
	    {
	      sig_print_info (signum);
	    }
	}
    }

  do_cleanups (old_chain);
}

static void
xdb_handle_command (char *args, int from_tty)
{
  char **argv;
  struct cleanup *old_chain;

  /* Break the command line up into args. */

  argv = buildargv (args);
  if (argv == NULL)
    {
      nomem (0);
    }
  old_chain = make_cleanup_freeargv (argv);
  if (argv[1] != (char *) NULL)
    {
      char *argBuf;
      int bufLen;

      bufLen = strlen (argv[0]) + 20;
      argBuf = (char *) xmalloc (bufLen);
      if (argBuf)
	{
	  int validFlag = 1;
	  enum target_signal oursig;

	  oursig = target_signal_from_name (argv[0]);
	  memset (argBuf, 0, bufLen);
	  if (strcmp (argv[1], "Q") == 0)
	    sprintf (argBuf, "%s %s", argv[0], "noprint");
	  else
	    {
	      if (strcmp (argv[1], "s") == 0)
		{
		  if (!signal_stop[oursig])
		    sprintf (argBuf, "%s %s", argv[0], "stop");
		  else
		    sprintf (argBuf, "%s %s", argv[0], "nostop");
		}
	      else if (strcmp (argv[1], "i") == 0)
		{
		  if (!signal_program[oursig])
		    sprintf (argBuf, "%s %s", argv[0], "pass");
		  else
		    sprintf (argBuf, "%s %s", argv[0], "nopass");
		}
	      else if (strcmp (argv[1], "r") == 0)
		{
		  if (!signal_print[oursig])
		    sprintf (argBuf, "%s %s", argv[0], "print");
		  else
		    sprintf (argBuf, "%s %s", argv[0], "noprint");
		}
	      else
		validFlag = 0;
	    }
	  if (validFlag)
	    handle_command (argBuf, from_tty);
	  else
	    printf_filtered (_("Invalid signal handling flag.\n"));
	  if (argBuf)
	    xfree (argBuf);
	}
    }
  do_cleanups (old_chain);
}

/* Print current contents of the tables set by the handle command.
   It is possible we should just be printing signals actually used
   by the current target (but for things to work right when switching
   targets, all signals should be in the signal tables).  */

static void
signals_info (char *signum_exp, int from_tty)
{
  enum target_signal oursig;
  sig_print_header ();

  if (signum_exp)
    {
      /* First see if this is a symbol name.  */
      oursig = target_signal_from_name (signum_exp);
      if (oursig == TARGET_SIGNAL_UNKNOWN)
	{
	  /* No, try numeric.  */
	  oursig =
	    target_signal_from_command (parse_and_eval_long (signum_exp));
	}
      sig_print_info (oursig);
      return;
    }

  printf_filtered ("\n");
  /* These ugly casts brought to you by the native VAX compiler.  */
  for (oursig = TARGET_SIGNAL_FIRST;
       (int) oursig < (int) TARGET_SIGNAL_LAST;
       oursig = (enum target_signal) ((int) oursig + 1))
    {
      QUIT;

      if (oursig != TARGET_SIGNAL_UNKNOWN
	  && oursig != TARGET_SIGNAL_DEFAULT && oursig != TARGET_SIGNAL_0)
	sig_print_info (oursig);
    }

  printf_filtered (_("\nUse the \"handle\" command to change these tables.\n"));
}

struct inferior_status
{
  enum target_signal stop_signal;
  CORE_ADDR stop_pc;
  bpstat stop_bpstat;
  int stop_step;
  int stop_stack_dummy;
  int stopped_by_random_signal;
  int stepping_over_breakpoint;
  CORE_ADDR step_range_start;
  CORE_ADDR step_range_end;
  struct frame_id step_frame_id;
  enum step_over_calls_kind step_over_calls;
  CORE_ADDR step_resume_break_address;
  int stop_after_trap;
  int stop_soon;

  /* These are here because if call_function_by_hand has written some
     registers and then decides to call error(), we better not have changed
     any registers.  */
  struct regcache *registers;

  /* A frame unique identifier.  */
  struct frame_id selected_frame_id;

  int breakpoint_proceeded;
  int restore_stack_info;
  int proceed_to_finish;
};

void
write_inferior_status_register (struct inferior_status *inf_status, int regno,
				LONGEST val)
{
  int size = register_size (current_gdbarch, regno);
  void *buf = alloca (size);
  store_signed_integer (buf, size, val);
  regcache_raw_write (inf_status->registers, regno, buf);
}

/* Save all of the information associated with the inferior<==>gdb
   connection.  INF_STATUS is a pointer to a "struct inferior_status"
   (defined in inferior.h).  */

struct inferior_status *
save_inferior_status (int restore_stack_info)
{
  struct inferior_status *inf_status = XMALLOC (struct inferior_status);

  inf_status->stop_signal = stop_signal;
  inf_status->stop_pc = stop_pc;
  inf_status->stop_step = stop_step;
  inf_status->stop_stack_dummy = stop_stack_dummy;
  inf_status->stopped_by_random_signal = stopped_by_random_signal;
  inf_status->stepping_over_breakpoint = stepping_over_breakpoint;
  inf_status->step_range_start = step_range_start;
  inf_status->step_range_end = step_range_end;
  inf_status->step_frame_id = step_frame_id;
  inf_status->step_over_calls = step_over_calls;
  inf_status->stop_after_trap = stop_after_trap;
  inf_status->stop_soon = stop_soon;
  /* Save original bpstat chain here; replace it with copy of chain.
     If caller's caller is walking the chain, they'll be happier if we
     hand them back the original chain when restore_inferior_status is
     called.  */
  inf_status->stop_bpstat = stop_bpstat;
  stop_bpstat = bpstat_copy (stop_bpstat);
  inf_status->breakpoint_proceeded = breakpoint_proceeded;
  inf_status->restore_stack_info = restore_stack_info;
  inf_status->proceed_to_finish = proceed_to_finish;

  inf_status->registers = regcache_dup (get_current_regcache ());

  inf_status->selected_frame_id = get_frame_id (get_selected_frame (NULL));
  return inf_status;
}

static int
restore_selected_frame (void *args)
{
  struct frame_id *fid = (struct frame_id *) args;
  struct frame_info *frame;

  frame = frame_find_by_id (*fid);

  /* If inf_status->selected_frame_id is NULL, there was no previously
     selected frame.  */
  if (frame == NULL)
    {
      warning (_("Unable to restore previously selected frame."));
      return 0;
    }

  select_frame (frame);

  return (1);
}

void
restore_inferior_status (struct inferior_status *inf_status)
{
  stop_signal = inf_status->stop_signal;
  stop_pc = inf_status->stop_pc;
  stop_step = inf_status->stop_step;
  stop_stack_dummy = inf_status->stop_stack_dummy;
  stopped_by_random_signal = inf_status->stopped_by_random_signal;
  stepping_over_breakpoint = inf_status->stepping_over_breakpoint;
  step_range_start = inf_status->step_range_start;
  step_range_end = inf_status->step_range_end;
  step_frame_id = inf_status->step_frame_id;
  step_over_calls = inf_status->step_over_calls;
  stop_after_trap = inf_status->stop_after_trap;
  stop_soon = inf_status->stop_soon;
  bpstat_clear (&stop_bpstat);
  stop_bpstat = inf_status->stop_bpstat;
  breakpoint_proceeded = inf_status->breakpoint_proceeded;
  proceed_to_finish = inf_status->proceed_to_finish;

  /* The inferior can be gone if the user types "print exit(0)"
     (and perhaps other times).  */
  if (target_has_execution)
    /* NB: The register write goes through to the target.  */
    regcache_cpy (get_current_regcache (), inf_status->registers);
  regcache_xfree (inf_status->registers);

  /* FIXME: If we are being called after stopping in a function which
     is called from gdb, we should not be trying to restore the
     selected frame; it just prints a spurious error message (The
     message is useful, however, in detecting bugs in gdb (like if gdb
     clobbers the stack)).  In fact, should we be restoring the
     inferior status at all in that case?  .  */

  if (target_has_stack && inf_status->restore_stack_info)
    {
      /* The point of catch_errors is that if the stack is clobbered,
         walking the stack might encounter a garbage pointer and
         error() trying to dereference it.  */
      if (catch_errors
	  (restore_selected_frame, &inf_status->selected_frame_id,
	   "Unable to restore previously selected frame:\n",
	   RETURN_MASK_ERROR) == 0)
	/* Error in restoring the selected frame.  Select the innermost
	   frame.  */
	select_frame (get_current_frame ());

    }

  xfree (inf_status);
}

static void
do_restore_inferior_status_cleanup (void *sts)
{
  restore_inferior_status (sts);
}

struct cleanup *
make_cleanup_restore_inferior_status (struct inferior_status *inf_status)
{
  return make_cleanup (do_restore_inferior_status_cleanup, inf_status);
}

void
discard_inferior_status (struct inferior_status *inf_status)
{
  /* See save_inferior_status for info on stop_bpstat. */
  bpstat_clear (&inf_status->stop_bpstat);
  regcache_xfree (inf_status->registers);
  xfree (inf_status);
}

int
inferior_has_forked (ptid_t pid, ptid_t *child_pid)
{
  struct target_waitstatus last;
  ptid_t last_ptid;

  get_last_target_status (&last_ptid, &last);

  if (last.kind != TARGET_WAITKIND_FORKED)
    return 0;

  if (!ptid_equal (last_ptid, pid))
    return 0;

  *child_pid = last.value.related_pid;
  return 1;
}

int
inferior_has_vforked (ptid_t pid, ptid_t *child_pid)
{
  struct target_waitstatus last;
  ptid_t last_ptid;

  get_last_target_status (&last_ptid, &last);

  if (last.kind != TARGET_WAITKIND_VFORKED)
    return 0;

  if (!ptid_equal (last_ptid, pid))
    return 0;

  *child_pid = last.value.related_pid;
  return 1;
}

int
inferior_has_execd (ptid_t pid, char **execd_pathname)
{
  struct target_waitstatus last;
  ptid_t last_ptid;

  get_last_target_status (&last_ptid, &last);

  if (last.kind != TARGET_WAITKIND_EXECD)
    return 0;

  if (!ptid_equal (last_ptid, pid))
    return 0;

  *execd_pathname = xstrdup (last.value.execd_pathname);
  return 1;
}

/* Oft used ptids */
ptid_t null_ptid;
ptid_t minus_one_ptid;

/* Create a ptid given the necessary PID, LWP, and TID components.  */

ptid_t
ptid_build (int pid, long lwp, long tid)
{
  ptid_t ptid;

  ptid.pid = pid;
  ptid.lwp = lwp;
  ptid.tid = tid;
  return ptid;
}

/* Create a ptid from just a pid.  */

ptid_t
pid_to_ptid (int pid)
{
  return ptid_build (pid, 0, 0);
}

/* Fetch the pid (process id) component from a ptid.  */

int
ptid_get_pid (ptid_t ptid)
{
  return ptid.pid;
}

/* Fetch the lwp (lightweight process) component from a ptid.  */

long
ptid_get_lwp (ptid_t ptid)
{
  return ptid.lwp;
}

/* Fetch the tid (thread id) component from a ptid.  */

long
ptid_get_tid (ptid_t ptid)
{
  return ptid.tid;
}

/* ptid_equal() is used to test equality of two ptids.  */

int
ptid_equal (ptid_t ptid1, ptid_t ptid2)
{
  return (ptid1.pid == ptid2.pid && ptid1.lwp == ptid2.lwp
	  && ptid1.tid == ptid2.tid);
}

/* restore_inferior_ptid() will be used by the cleanup machinery
   to restore the inferior_ptid value saved in a call to
   save_inferior_ptid().  */

static void
restore_inferior_ptid (void *arg)
{
  ptid_t *saved_ptid_ptr = arg;
  inferior_ptid = *saved_ptid_ptr;
  xfree (arg);
}

/* Save the value of inferior_ptid so that it may be restored by a
   later call to do_cleanups().  Returns the struct cleanup pointer
   needed for later doing the cleanup.  */

struct cleanup *
save_inferior_ptid (void)
{
  ptid_t *saved_ptid_ptr;

  saved_ptid_ptr = xmalloc (sizeof (ptid_t));
  *saved_ptid_ptr = inferior_ptid;
  return make_cleanup (restore_inferior_ptid, saved_ptid_ptr);
}


int non_stop = 0;
static int non_stop_1 = 0;

static void
set_non_stop (char *args, int from_tty,
	      struct cmd_list_element *c)
{
  if (target_has_execution)
    {
      non_stop_1 = non_stop;
      error (_("Cannot change this setting while the inferior is running."));
    }

  non_stop = non_stop_1;
}

static void
show_non_stop (struct ui_file *file, int from_tty,
	       struct cmd_list_element *c, const char *value)
{
  fprintf_filtered (file,
		    _("Controlling the inferior in non-stop mode is %s.\n"),
		    value);
}


void
_initialize_infrun (void)
{
  int i;
  int numsigs;
  struct cmd_list_element *c;

  add_info ("signals", signals_info, _("\
What debugger does when program gets various signals.\n\
Specify a signal as argument to print info on that signal only."));
  add_info_alias ("handle", "signals", 0);

  add_com ("handle", class_run, handle_command, _("\
Specify how to handle a signal.\n\
Args are signals and actions to apply to those signals.\n\
Symbolic signals (e.g. SIGSEGV) are recommended but numeric signals\n\
from 1-15 are allowed for compatibility with old versions of GDB.\n\
Numeric ranges may be specified with the form LOW-HIGH (e.g. 1-5).\n\
The special arg \"all\" is recognized to mean all signals except those\n\
used by the debugger, typically SIGTRAP and SIGINT.\n\
Recognized actions include \"stop\", \"nostop\", \"print\", \"noprint\",\n\
\"pass\", \"nopass\", \"ignore\", or \"noignore\".\n\
Stop means reenter debugger if this signal happens (implies print).\n\
Print means print a message if this signal happens.\n\
Pass means let program see this signal; otherwise program doesn't know.\n\
Ignore is a synonym for nopass and noignore is a synonym for pass.\n\
Pass and Stop may be combined."));
  if (xdb_commands)
    {
      add_com ("lz", class_info, signals_info, _("\
What debugger does when program gets various signals.\n\
Specify a signal as argument to print info on that signal only."));
      add_com ("z", class_run, xdb_handle_command, _("\
Specify how to handle a signal.\n\
Args are signals and actions to apply to those signals.\n\
Symbolic signals (e.g. SIGSEGV) are recommended but numeric signals\n\
from 1-15 are allowed for compatibility with old versions of GDB.\n\
Numeric ranges may be specified with the form LOW-HIGH (e.g. 1-5).\n\
The special arg \"all\" is recognized to mean all signals except those\n\
used by the debugger, typically SIGTRAP and SIGINT.\n\
Recognized actions include \"s\" (toggles between stop and nostop), \n\
\"r\" (toggles between print and noprint), \"i\" (toggles between pass and \
nopass), \"Q\" (noprint)\n\
Stop means reenter debugger if this signal happens (implies print).\n\
Print means print a message if this signal happens.\n\
Pass means let program see this signal; otherwise program doesn't know.\n\
Ignore is a synonym for nopass and noignore is a synonym for pass.\n\
Pass and Stop may be combined."));
    }

  if (!dbx_commands)
    stop_command = add_cmd ("stop", class_obscure,
			    not_just_help_class_command, _("\
There is no `stop' command, but you can set a hook on `stop'.\n\
This allows you to set a list of commands to be run each time execution\n\
of the program stops."), &cmdlist);

  add_setshow_zinteger_cmd ("infrun", class_maintenance, &debug_infrun, _("\
Set inferior debugging."), _("\
Show inferior debugging."), _("\
When non-zero, inferior specific debugging is enabled."),
			    NULL,
			    show_debug_infrun,
			    &setdebuglist, &showdebuglist);

  add_setshow_boolean_cmd ("displaced", class_maintenance, &debug_displaced, _("\
Set displaced stepping debugging."), _("\
Show displaced stepping debugging."), _("\
When non-zero, displaced stepping specific debugging is enabled."),
			    NULL,
			    show_debug_displaced,
			    &setdebuglist, &showdebuglist);

  add_setshow_boolean_cmd ("non-stop", no_class,
			   &non_stop_1, _("\
Set whether gdb controls the inferior in non-stop mode."), _("\
Show whether gdb controls the inferior in non-stop mode."), _("\
When debugging a multi-threaded program and this setting is\n\
off (the default, also called all-stop mode), when one thread stops\n\
(for a breakpoint, watchpoint, exception, or similar events), GDB stops\n\
all other threads in the program while you interact with the thread of\n\
interest.  When you continue or step a thread, you can allow the other\n\
threads to run, or have them remain stopped, but while you inspect any\n\
thread's state, all threads stop.\n\
\n\
In non-stop mode, when one thread stops, other threads can continue\n\
to run freely.  You'll be able to step each thread independently,\n\
leave it stopped or free to run as needed."),
			   set_non_stop,
			   show_non_stop,
			   &setlist,
			   &showlist);

  numsigs = (int) TARGET_SIGNAL_LAST;
  signal_stop = (unsigned char *) xmalloc (sizeof (signal_stop[0]) * numsigs);
  signal_print = (unsigned char *)
    xmalloc (sizeof (signal_print[0]) * numsigs);
  signal_program = (unsigned char *)
    xmalloc (sizeof (signal_program[0]) * numsigs);
  for (i = 0; i < numsigs; i++)
    {
      signal_stop[i] = 1;
      signal_print[i] = 1;
      signal_program[i] = 1;
    }

  /* Signals caused by debugger's own actions
     should not be given to the program afterwards.  */
  signal_program[TARGET_SIGNAL_TRAP] = 0;
  signal_program[TARGET_SIGNAL_INT] = 0;

  /* Signals that are not errors should not normally enter the debugger.  */
  signal_stop[TARGET_SIGNAL_ALRM] = 0;
  signal_print[TARGET_SIGNAL_ALRM] = 0;
  signal_stop[TARGET_SIGNAL_VTALRM] = 0;
  signal_print[TARGET_SIGNAL_VTALRM] = 0;
  signal_stop[TARGET_SIGNAL_PROF] = 0;
  signal_print[TARGET_SIGNAL_PROF] = 0;
  signal_stop[TARGET_SIGNAL_CHLD] = 0;
  signal_print[TARGET_SIGNAL_CHLD] = 0;
  signal_stop[TARGET_SIGNAL_IO] = 0;
  signal_print[TARGET_SIGNAL_IO] = 0;
  signal_stop[TARGET_SIGNAL_POLL] = 0;
  signal_print[TARGET_SIGNAL_POLL] = 0;
  signal_stop[TARGET_SIGNAL_URG] = 0;
  signal_print[TARGET_SIGNAL_URG] = 0;
  signal_stop[TARGET_SIGNAL_WINCH] = 0;
  signal_print[TARGET_SIGNAL_WINCH] = 0;

  /* These signals are used internally by user-level thread
     implementations.  (See signal(5) on Solaris.)  Like the above
     signals, a healthy program receives and handles them as part of
     its normal operation.  */
  signal_stop[TARGET_SIGNAL_LWP] = 0;
  signal_print[TARGET_SIGNAL_LWP] = 0;
  signal_stop[TARGET_SIGNAL_WAITING] = 0;
  signal_print[TARGET_SIGNAL_WAITING] = 0;
  signal_stop[TARGET_SIGNAL_CANCEL] = 0;
  signal_print[TARGET_SIGNAL_CANCEL] = 0;

  add_setshow_zinteger_cmd ("stop-on-solib-events", class_support,
			    &stop_on_solib_events, _("\
Set stopping for shared library events."), _("\
Show stopping for shared library events."), _("\
If nonzero, gdb will give control to the user when the dynamic linker\n\
notifies gdb of shared library events.  The most common event of interest\n\
to the user would be loading/unloading of a new library."),
			    NULL,
			    show_stop_on_solib_events,
			    &setlist, &showlist);

  add_setshow_enum_cmd ("follow-fork-mode", class_run,
			follow_fork_mode_kind_names,
			&follow_fork_mode_string, _("\
Set debugger response to a program call of fork or vfork."), _("\
Show debugger response to a program call of fork or vfork."), _("\
A fork or vfork creates a new process.  follow-fork-mode can be:\n\
  parent  - the original process is debugged after a fork\n\
  child   - the new process is debugged after a fork\n\
The unfollowed process will continue to run.\n\
By default, the debugger will follow the parent process."),
			NULL,
			show_follow_fork_mode_string,
			&setlist, &showlist);

  add_setshow_enum_cmd ("scheduler-locking", class_run, 
			scheduler_enums, &scheduler_mode, _("\
Set mode for locking scheduler during execution."), _("\
Show mode for locking scheduler during execution."), _("\
off  == no locking (threads may preempt at any time)\n\
on   == full locking (no thread except the current thread may run)\n\
step == scheduler locked during every single-step operation.\n\
	In this mode, no other thread may run during a step command.\n\
	Other threads may run while stepping over a function call ('next')."), 
			set_schedlock_func,	/* traps on target vector */
			show_scheduler_mode,
			&setlist, &showlist);

  add_setshow_boolean_cmd ("step-mode", class_run, &step_stop_if_no_debug, _("\
Set mode of the step operation."), _("\
Show mode of the step operation."), _("\
When set, doing a step over a function without debug line information\n\
will stop at the first instruction of that function. Otherwise, the\n\
function is skipped and the step command stops at a different source line."),
			   NULL,
			   show_step_stop_if_no_debug,
			   &setlist, &showlist);

  add_setshow_boolean_cmd ("can-use-displaced-stepping", class_maintenance,
			   &can_use_displaced_stepping, _("\
Set debugger's willingness to use displaced stepping."), _("\
Show debugger's willingness to use displaced stepping."), _("\
If zero, gdb will not use displaced stepping to step over\n\
breakpoints, even if such is supported by the target."),
			   NULL,
			   show_can_use_displaced_stepping,
			   &maintenance_set_cmdlist,
			   &maintenance_show_cmdlist);

  /* ptid initializations */
  null_ptid = ptid_build (0, 0, 0);
  minus_one_ptid = ptid_build (-1, 0, 0);
  inferior_ptid = null_ptid;
  target_last_wait_ptid = minus_one_ptid;
  displaced_step_ptid = null_ptid;
}
