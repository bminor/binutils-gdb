/* Multi-threaded debugging support for Linux (LWP layer).
   Copyright 2000 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include "defs.h"

#include "gdb_assert.h"
#include <errno.h>
#include <signal.h>
#include <sys/ptrace.h>
#include "gdb_wait.h"

#include "gdbthread.h"
#include "inferior.h"
#include "target.h"

#define DEBUG 1

#if DEBUG
extern const char *strsignal (int sig);
#endif

/* On Linux there are no real LWP's.  The closest thing to LWP's are
   processes sharing the same VM space.  A multi-threaded process is
   basically a group of such processes.  However, such a grouping is
   almost entirely a user-space issue; the kernel doesn't enforce such
   a grouping at all (this might change in the future).  In general,
   we'll rely on the threads library (i.e. the LinuxThreads library)
   to provide such a grouping.

   It is perfectly well possible to write a multi-threaded application
   without the assistance of a threads library, by using the clone
   system call directly.  This module should be able to give some
   rudimentary support for debugging such applications if developers
   specify the CLONE_PTRACE flag in the clone system call, and are
   using Linux 2.4 or above.

   Note that there are some peculiarities in Linux that affect this
   code:

   - In general one should specify the __WCLONE flag to waitpid in
     order to make it report events for any of the cloned processes.
     However, if a cloned process has exited the exit status is only
     reported if the __WCLONE flag is absent.

   - When a traced, cloned process exits and is waited for by the
     debugger, the kernel reassigns it to the origional parent and
     keeps it around as a "zombie".  Somehow, the LinuxThreads library
     doesn't notice this, which leads to the "zombie problem": When
     debugged a multi-threaded process that spawns a lot of threads
     will run out of processes, even if the threads exit, because the
     "zombies" stay around.  */

/* Structure describing a LWP.  */
struct lwp_info
{
  /* The process id of the LWP.  This is a combination of the LWP id
     and overall process id.  */
  int pid;

  /* Non-zero if we sent this LWP a SIGSTOP (but the LWP didn't report
     it back yet).  */
  int signalled;

  /* Non-zero if this LWP is stopped.  */
  int stopped;

  /* If non-zero, a pending wait status.  */
  int status;

  /* Non-zero if we were stepping this LWP.  */
  int step;

  /* Next LWP in list.  */
  struct lwp_info *next;
};

/* List of known LWPs.  */
static struct lwp_info *lwp_list;

/* Number of LWPs in the list.  */
static int num_lwps;

/* Non-zero if we're running in "threaded" mode.  */
static int threaded;


#ifndef TIDGET
#define TIDGET(PID)		(((PID) & 0x7fffffff) >> 16)
#define PIDGET(PID)		(((PID) & 0xffff))
#define MERGEPID(PID, TID)	(((PID) & 0xffff) | ((TID) << 16))
#endif

#define THREAD_FLAG		0x80000000
#define is_lwp(pid)		(((pid) & THREAD_FLAG) == 0 && TIDGET (pid))
#define GET_LWP(pid)		TIDGET (pid)
#define GET_PID(pid)		PIDGET (pid)
#define BUILD_LWP(tid, pid)	MERGEPID (pid, tid)

#define is_cloned(pid)	(GET_LWP (pid) != GET_PID (pid))

/* If the last reported event was a SIGTRAP, this variable is set to
   the process id of the LWP/thread that got it.  */
int trap_pid;


/* This module's target-specific operations.  */
static struct target_ops lin_lwp_ops;

/* The standard child operations.  */
extern struct target_ops child_ops;

/* Signal mask for use with sigsuspend in lin_lwp_wait, initialized in
   _initialize_lin_lwp.  */
static sigset_t suspend_mask;


/* Prototypes for local functions.  */
static void lin_lwp_mourn_inferior (void);


/* Initialize the list of LWPs.  */

static void
init_lwp_list (void)
{
  struct lwp_info *lp, *lpnext;

  for (lp = lwp_list; lp; lp = lpnext)
    {
      lpnext = lp->next;
      free (lp);
    }

  lwp_list = NULL;
  num_lwps = 0;
  threaded = 0;
}

/* Add the LWP specified by PID to the list.  If this causes the
   number of LWPs to become larger than one, go into "threaded" mode.
   Return a pointer to the structure describing the new LWP.  */

static struct lwp_info *
add_lwp (int pid)
{
  struct lwp_info *lp;

  gdb_assert (is_lwp (pid));

  lp = (struct lwp_info *) xmalloc (sizeof (struct lwp_info));

  memset (lp, 0, sizeof (struct lwp_info));

  lp->pid = pid;

  lp->next = lwp_list;
  lwp_list = lp;
  if (++num_lwps > 1)
    threaded = 1;

  return lp;
}

/* Remove the LWP specified by PID from the list.  */

static void
delete_lwp (int pid)
{
  struct lwp_info *lp, *lpprev;

  lpprev = NULL;

  for (lp = lwp_list; lp; lpprev = lp, lp = lp->next)
    if (lp->pid == pid)
      break;

  if (!lp)
    return;

  /* We don't go back to "non-threaded" mode if the number of threads
     becomes less than two.  */
  num_lwps--;

  if (lpprev)
    lpprev->next = lp->next;
  else
    lwp_list = lp->next;

  free (lp);
}

/* Return a pointer to the structure describing the LWP corresponding
   to PID.  If no corresponding LWP could be found, return NULL.  */

static struct lwp_info *
find_lwp_pid (int pid)
{
  struct lwp_info *lp;

  if (is_lwp (pid))
    pid = GET_LWP (pid);

  for (lp = lwp_list; lp; lp = lp->next)
    if (pid == GET_LWP (lp->pid))
      return lp;

  return NULL;
}

/* Call CALLBACK with its second argument set to DATA for every LWP in
   the list.  If CALLBACK returns 1 for a particular LWP, return a
   pointer to the structure describing that LWP immediately.
   Otherwise return NULL.  */

struct lwp_info *
iterate_over_lwps (int (*callback) (struct lwp_info *, void *), void *data)
{
  struct lwp_info *lp;

  for (lp = lwp_list; lp; lp = lp->next)
    if ((*callback) (lp, data))
      return lp;

  return NULL;
}


/* Helper functions.  */

static void
restore_inferior_pid (void *arg)
{
  int *saved_pid_ptr = arg;
  inferior_pid = *saved_pid_ptr;
  free (arg);
}

static struct cleanup *
save_inferior_pid (void)
{
  int *saved_pid_ptr;

  saved_pid_ptr = xmalloc (sizeof (int));
  *saved_pid_ptr = inferior_pid;
  return make_cleanup (restore_inferior_pid, saved_pid_ptr);
}


/* Implementation of the PREPARE_TO_PROCEED hook for the Linux LWP layer.  */

int
lin_lwp_prepare_to_proceed (void)
{
  if (trap_pid && inferior_pid != trap_pid)
    {
      /* Switched over from TRAP_PID.  */
      CORE_ADDR stop_pc = read_pc ();
      CORE_ADDR trap_pc;

      /* Avoid switching where it wouldn't do any good, i.e. if both
         threads are at the same breakpoint.  */
      trap_pc = read_pc_pid (trap_pid);
      if (trap_pc != stop_pc && breakpoint_here_p (trap_pc))
	{
	  /* User hasn't deleted the breakpoint.  Return non-zero, and
             switch back to TRAP_PID.  */
	  inferior_pid = trap_pid;

	  /* FIXME: Is this stuff really necessary?  */
	  flush_cached_frames ();
	  registers_changed ();

	  return 1;
	}
    }

  return 0;
}


#if 0
static void
lin_lwp_open (char *args, int from_tty)
{
  push_target (&lin_lwp_ops);
}
#endif

/* Attach to the LWP specified by PID.  If VERBOSE is non-zero, print
   a message telling the user that a new LWP has been added to the
   process.  */

void
lin_lwp_attach_lwp (int pid, int verbose)
{
  struct lwp_info *lp;

  gdb_assert (is_lwp (pid));

  if (verbose)
    printf_filtered ("[New %s]\n", target_pid_to_str (pid));

  if (ptrace (PTRACE_ATTACH, GET_LWP (pid), 0, 0) < 0)
    error ("Can't attach %s: %s", target_pid_to_str (pid), strerror (errno));

  lp = add_lwp (pid);
  lp->signalled = 1;
}

static void
lin_lwp_attach (char *args, int from_tty)
{
  /* FIXME: We should probably accept a list of process id's, and
     attach all of them.  */
  error("Not implemented yet");
}

static void
lin_lwp_detach (char *args, int from_tty)
{
  /* FIXME: Provide implementation when we implement lin_lwp_attach.  */
  error ("Not implemented yet");
}


struct private_thread_info
{
  int lwpid;
};

/* Return non-zero if TP corresponds to the LWP specified by DATA
   (which is assumed to be a pointer to a `struct lwp_info'.  */

static int
find_lwp_callback (struct thread_info *tp, void *data)
{
  struct lwp_info *lp = data;

  if (tp->private->lwpid == GET_LWP (lp->pid))
    return 1;

  return 0;
}

/* Resume LP.  */

static int
resume_callback (struct lwp_info *lp, void *data)
{
  if (lp->stopped && lp->status == 0)
    {
      struct thread_info *tp;

#if 1
      /* FIXME: kettenis/2000-08-26: This should really be handled
         properly by core GDB.  */

      tp = find_thread_pid (lp->pid);
      if (tp == NULL)
	tp = iterate_over_threads (find_lwp_callback, lp);
      gdb_assert (tp);

      /* If we were previously stepping the thread, and now continue
         the thread we must invalidate the stepping range.  However,
         if there is a step_resume breakpoint for this thread, we must
         preserve the stepping range to make it possible to continue
         stepping once we hit it.  */
      if (tp->step_range_end && tp->step_resume_breakpoint == NULL)
	{
	  gdb_assert (lp->step);
	  tp->step_range_start = tp->step_range_end = 0;
	}
#endif

      child_resume (GET_LWP (lp->pid), 0, TARGET_SIGNAL_0);
      lp->stopped = 0;
      lp->step = 0;
    }

  return 0;
}

static void
lin_lwp_resume (int pid, int step, enum target_signal signo)
{
  struct lwp_info *lp;
  int resume_all;

  /* Apparently the interpretation of PID is dependent on STEP: If
     STEP is non-zero, a specific PID means `step only this process
     id'.  But if STEP is zero, then PID means `continue *all*
     processes, but give the signal only to this one'.  */
  resume_all = (pid == -1) || !step;

  /* If PID is -1, it's the current inferior that should be
     handled special.  */
  if (pid == -1)
    pid = inferior_pid;

  lp = find_lwp_pid (pid);
  if (lp)
    {
      pid = GET_LWP (lp->pid);

      /* Mark LWP as not stopped to prevent it from being continued by
	 resume_callback.  */
      lp->stopped = 0;

      /* Remember if we're stepping.  */
      lp->step = step;

      /* If we have a pending wait status for this thread, there is no
         point in resuming the process.  */
      if (lp->status)
	{
	  /* FIXME: What should we do if we are supposed to continue
             this thread with a signal?  */
	  gdb_assert (signo == TARGET_SIGNAL_0);
	  return;
	}
    }

  if (resume_all)
    iterate_over_lwps (resume_callback, NULL);

  child_resume (pid, step, signo);
}


/* Send a SIGSTOP to LP.  */

static int
stop_callback (struct lwp_info *lp, void *data)
{
  if (! lp->stopped && ! lp->signalled)
    {
      int ret;

      ret = kill (GET_LWP (lp->pid), SIGSTOP);
      gdb_assert (ret == 0);

      lp->signalled = 1;
      gdb_assert (lp->status == 0);
    }

  return 0;
}

/* Wait until LP is stopped.  */

static int
stop_wait_callback (struct lwp_info *lp, void *data)
{
  if (! lp->stopped && lp->signalled)
    {
      pid_t pid;
      int status;

      gdb_assert (lp->status == 0);

      pid = waitpid (GET_LWP (lp->pid), &status,
		     is_cloned (lp->pid) ? __WCLONE : 0);
      if (pid == -1 && errno == ECHILD)
	/* OK, the proccess has disappeared.  We'll catch the actual
           exit event in lin_lwp_wait.  */
	return 0;

      gdb_assert (pid == GET_LWP (lp->pid));

      if (WIFEXITED (status) || WIFSIGNALED (status))
	{
	  gdb_assert (num_lwps > 1);
	  gdb_assert (! is_cloned (lp->pid));

	  gdb_assert (in_thread_list (lp->pid));
	  if (lp->pid != inferior_pid)
	    delete_thread (lp->pid);
	  printf_unfiltered ("[%s exited]\n",
			     target_pid_to_str (lp->pid));

	  delete_lwp (lp->pid);
	  return 0;
	}

      gdb_assert (WIFSTOPPED (status));
      lp->stopped = 1;

      if (WSTOPSIG (status) != SIGSTOP)
	{
	  if (WSTOPSIG (status) == SIGTRAP
	      && breakpoint_inserted_here_p (read_pc_pid (pid)
					     - DECR_PC_AFTER_BREAK))
	    {
	      /* If a LWP other than the LWP that we're reporting an
                 event for has hit a GDB breakpoint (as opposed to
                 some random trap signal), then just arrange for it to
                 hit it again later.  We don't keep the SIGTRAP status
                 and don't forward the SIGTRAP signal to the LWP.  We
                 will handle the current event, eventually we will
                 resume all LWPs, and this one will get its breakpoint
                 trap again.

		 If we do not do this, then we run the risk that the
		 user will delete or disable the breakpoint, but the
		 thread will have already tripped on it.  */
#if DEBUG
	      printf ("Tripped breakpoint at %lx in LWP %d"
		      " while waiting for SIGSTOP.\n",
		      (long) read_pc_pid (lp->pid), pid);
#endif
	      /* Set the PC to before the trap.  */
	      if (DECR_PC_AFTER_BREAK)
		write_pc_pid (read_pc_pid (pid) - DECR_PC_AFTER_BREAK, pid);
	    }
	  else
	    {
#if DEBUG
	      printf ("Received %s in LWP %d while waiting for SIGSTOP.\n",
		      strsignal (WSTOPSIG (status)), pid);
#endif
	      /* The thread was stopped with a signal other than
		 SIGSTOP, and didn't accidentiliy trip a breakpoint.
		 Record the wait status.  */
	      lp->status = status;
	    }
	}
      else
	{
	  /* We caught the SIGSTOP that we intended to catch, so
             there's no SIGSTOP pending.  */
	  lp->signalled = 0;
	}
    }

  return 0;
}

/* Return non-zero if LP has a wait status pending.  */

static int
status_callback (struct lwp_info *lp, void *data)
{
  return (lp->status != 0);
}

/* Return non-zero if LP isn't stopped.  */

static int
running_callback (struct lwp_info *lp, void *data)
{
  return (lp->stopped == 0);
}

static int
lin_lwp_wait (int pid, struct target_waitstatus *ourstatus)
{
  struct lwp_info *lp = NULL;
  int options = 0;
  int status = 0;

 retry:

  /* First check if there is a LWP with a wait status pending.  */
  if (pid == -1)
    {
      /* Any LWP will do.  */
      lp = iterate_over_lwps (status_callback, NULL);
      if (lp)
	{
#if DEBUG
	  printf ("Using pending wait status for LWP %d.\n",
		  GET_LWP (lp->pid));
#endif
	  status = lp->status;
	  lp->status = 0;
	}

      /* But if we don't fine one, we'll have to wait, and check both
         cloned and uncloned processes.  We start with the cloned
         processes.  */
      options = __WCLONE | WNOHANG;
    }
  else if (is_lwp (pid))
    {
#if DEBUG
      printf ("Waiting for specific LWP %d.\n", GET_LWP (pid));
#endif
      /* We have a specific LWP to check.  */
      lp = find_lwp_pid (GET_LWP (pid));
      gdb_assert (lp);
      status = lp->status;
      lp->status = 0;
#if DEBUG
      if (status)
	  printf ("Using pending wait status for LWP %d.\n",
		  GET_LWP (lp->pid));
#endif

      /* If we have to wait, take into account whether PID is a cloned
         process or not.  And we have to convert it to something that
         the layer beneath us can understand.  */
      options = is_cloned (lp->pid) ? __WCLONE : 0;
      pid = GET_LWP (pid);
    }

  if (status && lp->signalled)
    {
      /* A pending SIGSTOP may interfere with the normal stream of
	 events.  In a typical case where interference is a problem,
	 we have a SIGSTOP signal pending for LWP A while
	 single-stepping it, encounter an event in LWP B, and take the
	 pending SIGSTOP while trying to stop LWP A.  After processing
	 the event in LWP B, LWP A is continued, and we'll never see
	 the SIGTRAP associated with the last time we were
	 single-stepping LWP A.  */

      /* Resume the thread.  It should halt immediately returning the
	 pending SIGSTOP.  */
      child_resume (GET_LWP (lp->pid), lp->step, TARGET_SIGNAL_0);
      lp->stopped = 0;

      /* This should catch the pending SIGSTOP.  */
      stop_wait_callback (lp, NULL);
    }

  set_sigint_trap ();	/* Causes SIGINT to be passed on to the
			   attached process. */
  set_sigio_trap ();

  while (status == 0)
    {
      pid_t lwpid;

      lwpid = waitpid (pid, &status, options);
      if (lwpid > 0)
	{
	  gdb_assert (pid == -1 || lwpid == pid);

	  lp = find_lwp_pid (lwpid);
	  if (! lp)
	    {
	      lp = add_lwp (BUILD_LWP (lwpid, inferior_pid));
	      if (threaded)
		{
		  gdb_assert (WIFSTOPPED (status) && WSTOPSIG (status) == SIGSTOP);
		  lp->signalled = 1;

		  if (! in_thread_list (inferior_pid))
		    {
		      inferior_pid = BUILD_LWP (inferior_pid, inferior_pid);
		      add_thread (inferior_pid);
		    }

		  add_thread (lp->pid);
		  printf_unfiltered ("[New %s]\n",
				     target_pid_to_str (lp->pid));
		}
	    }

	  /* Make sure we don't report a TARGET_WAITKIND_EXITED or
             TARGET_WAITKIND_SIGNALLED event if there are still LWP's
             left in the process.  */
	  if ((WIFEXITED (status) || WIFSIGNALED (status)) && num_lwps > 1)
	    {
	      if (in_thread_list (lp->pid))
		{
		  /* Core GDB cannot deal with us deeting the current
                     thread.  */
		  if (lp->pid != inferior_pid)
		    delete_thread (lp->pid);
		  printf_unfiltered ("[%s exited]\n",
				     target_pid_to_str (lp->pid));
		}
#if DEBUG
	      printf ("%s exited.\n", target_pid_to_str (lp->pid));
#endif
	      delete_lwp (lp->pid);

	      /* Make sure there is at least one thread running.  */
	      gdb_assert (iterate_over_lwps (running_callback, NULL));

	      /* Discard the event.  */
	      status = 0;
	      continue;
	    }

	  /* Make sure we don't report a SIGSTOP that we sent
             ourselves in an attempt to stop an LWP.  */
	  if (lp->signalled && WIFSTOPPED (status)
	      && WSTOPSIG (status) == SIGSTOP)
	    {
#if DEBUG
	      printf ("Delayed SIGSTOP caught for %s.\n",
		      target_pid_to_str (lp->pid));
#endif
	      /* This is a delayed SIGSTOP.  */
	      lp->signalled = 0;

	      child_resume (GET_LWP (lp->pid), lp->step, TARGET_SIGNAL_0);
	      lp->stopped = 0;

	      /* Discard the event.  */
	      status = 0;
	      continue;
	    }

	  break;
	}

      if (pid == -1)
	{
	  /* Alternate between checking cloned and uncloned processes.  */
	  options ^= __WCLONE;

	  /* And suspend every time we have checked both.  */
	  if (options & __WCLONE)
	    sigsuspend (&suspend_mask);
	}

      /* We shouldn't end up here unless we want to try again.  */
      gdb_assert (status == 0);
    }

  clear_sigio_trap ();
  clear_sigint_trap ();

  gdb_assert (lp);

  /* Don't report signals that GDB isn't interested in, such as
     signals that are neither printed nor stopped upon.  Stopping all
     threads can be a bit time-consuming so if we want decent
     performance with heavily multi-threaded programs, especially when
     they're using a high frequency timer, we'd better avoid it if we
     can.  */

  if (WIFSTOPPED (status))
    {
      int signo = target_signal_from_host (WSTOPSIG (status));

      if (signal_stop_state (signo) == 0
	  && signal_print_state (signo) == 0
	  && signal_pass_state (signo) == 1)
	{
	  child_resume (GET_LWP (lp->pid), lp->step, signo);
	  lp->stopped = 0;
	  status = 0;
	  goto retry;
	}
    }

  /* This LWP is stopped now.  */
  lp->stopped = 1;

  /* Now stop all other LWP's ...  */
  iterate_over_lwps (stop_callback, NULL);

  /* ... and wait until all of them have reported back that they're no
     longer running.  */
  iterate_over_lwps (stop_wait_callback, NULL);

  /* If we're not running in "threaded" mode, we'll report the bare
     process id.  */

  if (WIFSTOPPED (status) && WSTOPSIG (status) == SIGTRAP)
    trap_pid = (threaded ? lp->pid : GET_LWP (lp->pid));
  else
    trap_pid = 0;

  store_waitstatus (ourstatus, status);
  return (threaded ? lp->pid : GET_LWP (lp->pid));
}

static int
kill_callback (struct lwp_info *lp, void *data)
{
  ptrace (PTRACE_KILL, GET_LWP (lp->pid), 0, 0);
  return 0;
}

static int
kill_wait_callback (struct lwp_info *lp, void *data)
{
  pid_t pid;

  /* We must make sure that there are no pending events (delayed
     SIGSTOPs, pending SIGTRAPs, etc.) to make sure the current
     program doesn't interfere with any following debugging session.  */

  /* For cloned processes we must check both with __WCLONE and
     without, since the exit status of a cloned process isn't reported
     with __WCLONE.  */
  if (is_cloned (lp->pid))
    {
      do
	{
	  pid = waitpid (GET_LWP (lp->pid), NULL, __WCLONE);
	}
      while (pid == GET_LWP (lp->pid));

      gdb_assert (pid == -1 && errno == ECHILD);
    }

  do
    {
      pid = waitpid (GET_LWP (lp->pid), NULL, 0);
    }
  while (pid == GET_LWP (lp->pid));

  gdb_assert (pid == -1 && errno == ECHILD);
  return 0;
}

static void
lin_lwp_kill (void)
{
  /* Kill all LWP's ...  */
  iterate_over_lwps (kill_callback, NULL);

  /* ... and wait until we've flushed all events.  */
  iterate_over_lwps (kill_wait_callback, NULL);

  target_mourn_inferior ();
}

static void
lin_lwp_create_inferior (char *exec_file, char *allargs, char **env)
{
  struct target_ops *target_beneath;

  init_lwp_list ();

#if 0
  target_beneath = find_target_beneath (&lin_lwp_ops);
#else
  target_beneath = &child_ops;
#endif
  target_beneath->to_create_inferior (exec_file, allargs, env);
}

static void  
lin_lwp_mourn_inferior (void)
{
  struct target_ops *target_beneath;

  init_lwp_list ();

  trap_pid = 0;

#if 0
  target_beneath = find_target_beneath (&lin_lwp_ops);
#else
  target_beneath = &child_ops;
#endif
  target_beneath->to_mourn_inferior ();
}

static void
lin_lwp_fetch_registers (int regno)
{
  struct cleanup *old_chain = save_inferior_pid ();

  if (is_lwp (inferior_pid))
    inferior_pid = GET_LWP (inferior_pid);

  fetch_inferior_registers (regno);

  do_cleanups (old_chain);
}

static void
lin_lwp_store_registers (int regno)
{
  struct cleanup *old_chain = save_inferior_pid ();

  if (is_lwp (inferior_pid))
    inferior_pid = GET_LWP (inferior_pid);

  store_inferior_registers (regno);

  do_cleanups (old_chain);
}

static int
lin_lwp_xfer_memory (CORE_ADDR memaddr, char *myaddr, int len, int write,
		     struct target_ops *target)
{
  struct cleanup *old_chain = save_inferior_pid ();
  int xfer;

  if (is_lwp (inferior_pid))
    inferior_pid = GET_LWP (inferior_pid);

  xfer = child_xfer_memory (memaddr, myaddr, len, write, target);

  do_cleanups (old_chain);
  return xfer;
}

static int
lin_lwp_thread_alive (int pid)
{
  gdb_assert (is_lwp (pid));

  errno = 0;
  ptrace (PTRACE_PEEKUSER, GET_LWP (pid), 0, 0);
  if (errno)
    return 0;

  return 1;
}

static char *
lin_lwp_pid_to_str (int pid)
{
  static char buf[64];

  if (is_lwp (pid))
    {
      snprintf (buf, sizeof (buf), "LWP %d", GET_LWP (pid));
      return buf;
    }

  return normal_pid_to_str (pid);
}

static void
init_lin_lwp_ops (void)
{
#if 0
  lin_lwp_ops.to_open = lin_lwp_open;
#endif
  lin_lwp_ops.to_shortname = "lwp-layer";
  lin_lwp_ops.to_longname = "lwp-layer";
  lin_lwp_ops.to_doc = "Low level threads support (LWP layer)";
  lin_lwp_ops.to_attach = lin_lwp_attach;
  lin_lwp_ops.to_detach = lin_lwp_detach;
  lin_lwp_ops.to_resume = lin_lwp_resume;
  lin_lwp_ops.to_wait = lin_lwp_wait;
  lin_lwp_ops.to_fetch_registers = lin_lwp_fetch_registers;
  lin_lwp_ops.to_store_registers = lin_lwp_store_registers;
  lin_lwp_ops.to_xfer_memory = lin_lwp_xfer_memory;
  lin_lwp_ops.to_kill = lin_lwp_kill;
  lin_lwp_ops.to_create_inferior = lin_lwp_create_inferior;
  lin_lwp_ops.to_mourn_inferior = lin_lwp_mourn_inferior;
  lin_lwp_ops.to_thread_alive = lin_lwp_thread_alive;
  lin_lwp_ops.to_pid_to_str = lin_lwp_pid_to_str;
  lin_lwp_ops.to_stratum = thread_stratum;
  lin_lwp_ops.to_has_thread_control = tc_schedlock;
  lin_lwp_ops.to_magic = OPS_MAGIC;
}

static void
sigchld_handler (int signo)
{
  /* Do nothing.  The only reason for this handler is that it allows
     us to use sigsuspend in lin_lwp_wait above to wait for the
     arrival of a SIGCHLD.  */
}

void
_initialize_lin_lwp (void)
{
  struct sigaction action;
  sigset_t mask;

  extern void thread_db_init (struct target_ops *);

  init_lin_lwp_ops ();
  add_target (&lin_lwp_ops);
  thread_db_init (&lin_lwp_ops);

  action.sa_handler = sigchld_handler;
  sigemptyset (&action.sa_mask);
  action.sa_flags = 0;
  sigaction (SIGCHLD, &action, NULL);

  /* We block SIGCHLD throughout this code ...  */
  sigemptyset (&mask);
  sigaddset (&mask, SIGCHLD);
  sigprocmask (SIG_BLOCK, &mask, &suspend_mask);

  /* ... except during a sigsuspend.  */
  sigdelset (&suspend_mask, SIGCHLD);
}


/* FIXME: kettenis/2000-08-26: The stuff on this page is specific to
   the LinuxThreads library and therefore doesn't really belong here.  */

/* Read variable NAME in the target and return its value if found.
   Otherwise return zero.  It is assumed that the type of the variable
   is `int'.  */

static int
get_signo (const char *name)
{
  struct minimal_symbol *ms;
  int signo;

  ms = lookup_minimal_symbol (name, NULL, NULL);
  if (ms == NULL)
    return 0;

  if (target_read_memory (SYMBOL_VALUE_ADDRESS (ms), (char *) &signo,
			  sizeof (signo)) != 0)
    return 0;

  return signo;
}

/* Return the set of signals used by the threads library in *SET.  */

void
lin_thread_get_thread_signals (sigset_t *set)
{
  int restart;
  int cancel;

  sigemptyset (set);

  restart = get_signo ("__pthread_sig_restart");
  if (restart == 0)
    return;

  cancel = get_signo ("__pthread_sig_cancel");
  if (cancel == 0)
    return;

  sigaddset (set, restart);
  sigaddset (set, cancel);
}
