/* GNU/Linux native-dependent code common to multiple platforms.

   Copyright (C) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008
   Free Software Foundation, Inc.

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
#include "inferior.h"
#include "target.h"
#include "gdb_string.h"
#include "gdb_wait.h"
#include "gdb_assert.h"
#ifdef HAVE_TKILL_SYSCALL
#include <unistd.h>
#include <sys/syscall.h>
#endif
#include <sys/ptrace.h>
#include "linux-nat.h"
#include "linux-fork.h"
#include "gdbthread.h"
#include "gdbcmd.h"
#include "regcache.h"
#include "regset.h"
#include "inf-ptrace.h"
#include "auxv.h"
#include <sys/param.h>		/* for MAXPATHLEN */
#include <sys/procfs.h>		/* for elf_gregset etc. */
#include "elf-bfd.h"		/* for elfcore_write_* */
#include "gregset.h"		/* for gregset */
#include "gdbcore.h"		/* for get_exec_file */
#include <ctype.h>		/* for isdigit */
#include "gdbthread.h"		/* for struct thread_info etc. */
#include "gdb_stat.h"		/* for struct stat */
#include <fcntl.h>		/* for O_RDONLY */
#include "inf-loop.h"
#include "event-loop.h"
#include "event-top.h"

/* This comment documents high-level logic of this file. 

Waiting for events in sync mode
===============================

When waiting for an event in a specific thread, we just use waitpid, passing
the specific pid, and not passing WNOHANG.

When waiting for an event in all threads, waitpid is not quite good. Prior to
version 2.4, Linux can either wait for event in main thread, or in secondary
threads. (2.4 has the __WALL flag).  So, if we use blocking waitpid, we might
miss an event.  The solution is to use non-blocking waitpid, together with
sigsuspend.  First, we use non-blocking waitpid to get an event in the main 
process, if any. Second, we use non-blocking waitpid with the __WCLONED
flag to check for events in cloned processes.  If nothing is found, we use
sigsuspend to wait for SIGCHLD.  When SIGCHLD arrives, it means something
happened to a child process -- and SIGCHLD will be delivered both for events
in main debugged process and in cloned processes.  As soon as we know there's
an event, we get back to calling nonblocking waitpid with and without __WCLONED.

Note that SIGCHLD should be blocked between waitpid and sigsuspend calls,
so that we don't miss a signal. If SIGCHLD arrives in between, when it's
blocked, the signal becomes pending and sigsuspend immediately
notices it and returns.

Waiting for events in async mode
================================

In async mode, GDB should always be ready to handle both user input and target
events, so neither blocking waitpid nor sigsuspend are viable
options. Instead, we should notify the GDB main event loop whenever there's
unprocessed event from the target.  The only way to notify this event loop is
to make it wait on input from a pipe, and write something to the pipe whenever
there's event. Obviously, if we fail to notify the event loop if there's
target event, it's bad.  If we notify the event loop when there's no event
from target, linux-nat.c will detect that there's no event, actually, and
report event of type TARGET_WAITKIND_IGNORE, but it will waste time and
better avoided.

The main design point is that every time GDB is outside linux-nat.c, we have a
SIGCHLD handler installed that is called when something happens to the target
and notifies the GDB event loop. Also, the event is extracted from the target
using waitpid and stored for future use.  Whenever GDB core decides to handle
the event, and calls into linux-nat.c, we disable SIGCHLD and process things
as in sync mode, except that before waitpid call we check if there are any
previously read events.

It could happen that during event processing, we'll try to get more events
than there are events in the local queue, which will result to waitpid call.
Those waitpid calls, while blocking, are guarantied to always have
something for waitpid to return.  E.g., stopping a thread with SIGSTOP, and
waiting for the lwp to stop.

The event loop is notified about new events using a pipe. SIGCHLD handler does
waitpid and writes the results in to a pipe. GDB event loop has the other end
of the pipe among the sources. When event loop starts to process the event
and calls a function in linux-nat.c, all events from the pipe are transferred
into a local queue and SIGCHLD is blocked. Further processing goes as in sync
mode. Before we return from linux_nat_wait, we transfer all unprocessed events
from local queue back to the pipe, so that when we get back to event loop,
event loop will notice there's something more to do.

SIGCHLD is blocked when we're inside target_wait, so that should we actually
want to wait for some more events, SIGCHLD handler does not steal them from
us. Technically, it would be possible to add new events to the local queue but
it's about the same amount of work as blocking SIGCHLD.

This moving of events from pipe into local queue and back into pipe when we
enter/leave linux-nat.c is somewhat ugly. Unfortunately, GDB event loop is
home-grown and incapable to wait on any queue.

Use of signals
==============

We stop threads by sending a SIGSTOP.  The use of SIGSTOP instead of another
signal is not entirely significant; we just need for a signal to be delivered,
so that we can intercept it.  SIGSTOP's advantage is that it can not be
blocked.  A disadvantage is that it is not a real-time signal, so it can only
be queued once; we do not keep track of other sources of SIGSTOP.

Two other signals that can't be blocked are SIGCONT and SIGKILL.  But we can't
use them, because they have special behavior when the signal is generated -
not when it is delivered.  SIGCONT resumes the entire thread group and SIGKILL
kills the entire thread group.

A delivered SIGSTOP would stop the entire thread group, not just the thread we
tkill'd.  But we never let the SIGSTOP be delivered; we always intercept and 
cancel it (by PTRACE_CONT without passing SIGSTOP).

We could use a real-time signal instead.  This would solve those problems; we
could use PTRACE_GETSIGINFO to locate the specific stop signals sent by GDB.
But we would still have to have some support for SIGSTOP, since PTRACE_ATTACH
generates it, and there are races with trying to find a signal that is not
blocked.  */

#ifndef O_LARGEFILE
#define O_LARGEFILE 0
#endif

/* If the system headers did not provide the constants, hard-code the normal
   values.  */
#ifndef PTRACE_EVENT_FORK

#define PTRACE_SETOPTIONS	0x4200
#define PTRACE_GETEVENTMSG	0x4201

/* options set using PTRACE_SETOPTIONS */
#define PTRACE_O_TRACESYSGOOD	0x00000001
#define PTRACE_O_TRACEFORK	0x00000002
#define PTRACE_O_TRACEVFORK	0x00000004
#define PTRACE_O_TRACECLONE	0x00000008
#define PTRACE_O_TRACEEXEC	0x00000010
#define PTRACE_O_TRACEVFORKDONE	0x00000020
#define PTRACE_O_TRACEEXIT	0x00000040

/* Wait extended result codes for the above trace options.  */
#define PTRACE_EVENT_FORK	1
#define PTRACE_EVENT_VFORK	2
#define PTRACE_EVENT_CLONE	3
#define PTRACE_EVENT_EXEC	4
#define PTRACE_EVENT_VFORK_DONE	5
#define PTRACE_EVENT_EXIT	6

#endif /* PTRACE_EVENT_FORK */

/* We can't always assume that this flag is available, but all systems
   with the ptrace event handlers also have __WALL, so it's safe to use
   here.  */
#ifndef __WALL
#define __WALL          0x40000000 /* Wait for any child.  */
#endif

#ifndef PTRACE_GETSIGINFO
#define PTRACE_GETSIGINFO    0x4202
#endif

/* The single-threaded native GNU/Linux target_ops.  We save a pointer for
   the use of the multi-threaded target.  */
static struct target_ops *linux_ops;
static struct target_ops linux_ops_saved;

/* The method to call, if any, when a new thread is attached.  */
static void (*linux_nat_new_thread) (ptid_t);

/* The saved to_xfer_partial method, inherited from inf-ptrace.c.
   Called by our to_xfer_partial.  */
static LONGEST (*super_xfer_partial) (struct target_ops *, 
				      enum target_object,
				      const char *, gdb_byte *, 
				      const gdb_byte *,
				      ULONGEST, LONGEST);

static int debug_linux_nat;
static void
show_debug_linux_nat (struct ui_file *file, int from_tty,
		      struct cmd_list_element *c, const char *value)
{
  fprintf_filtered (file, _("Debugging of GNU/Linux lwp module is %s.\n"),
		    value);
}

static int debug_linux_nat_async = 0;
static void
show_debug_linux_nat_async (struct ui_file *file, int from_tty,
			    struct cmd_list_element *c, const char *value)
{
  fprintf_filtered (file, _("Debugging of GNU/Linux async lwp module is %s.\n"),
		    value);
}

static int linux_parent_pid;

struct simple_pid_list
{
  int pid;
  int status;
  struct simple_pid_list *next;
};
struct simple_pid_list *stopped_pids;

/* This variable is a tri-state flag: -1 for unknown, 0 if PTRACE_O_TRACEFORK
   can not be used, 1 if it can.  */

static int linux_supports_tracefork_flag = -1;

/* If we have PTRACE_O_TRACEFORK, this flag indicates whether we also have
   PTRACE_O_TRACEVFORKDONE.  */

static int linux_supports_tracevforkdone_flag = -1;

/* Async mode support */

/* True if async mode is currently on.  */
static int linux_nat_async_enabled;

/* Zero if the async mode, although enabled, is masked, which means
   linux_nat_wait should behave as if async mode was off.  */
static int linux_nat_async_mask_value = 1;

/* The read/write ends of the pipe registered as waitable file in the
   event loop.  */
static int linux_nat_event_pipe[2] = { -1, -1 };

/* Number of queued events in the pipe.  */
static volatile int linux_nat_num_queued_events;

/* The possible SIGCHLD handling states.  */

enum sigchld_state
{
  /* SIGCHLD disabled, with action set to sigchld_handler, for the
     sigsuspend in linux_nat_wait.  */
  sigchld_sync,
  /* SIGCHLD enabled, with action set to async_sigchld_handler.  */
  sigchld_async,
  /* Set SIGCHLD to default action.  Used while creating an
     inferior.  */
  sigchld_default
};

/* The current SIGCHLD handling state.  */
static enum sigchld_state linux_nat_async_events_state;

static enum sigchld_state linux_nat_async_events (enum sigchld_state enable);
static void pipe_to_local_event_queue (void);
static void local_event_queue_to_pipe (void);
static void linux_nat_event_pipe_push (int pid, int status, int options);
static int linux_nat_event_pipe_pop (int* ptr_status, int* ptr_options);
static void linux_nat_set_async_mode (int on);
static void linux_nat_async (void (*callback)
			     (enum inferior_event_type event_type, void *context),
			     void *context);
static int linux_nat_async_mask (int mask);
static int kill_lwp (int lwpid, int signo);

/* Captures the result of a successful waitpid call, along with the
   options used in that call.  */
struct waitpid_result
{
  int pid;
  int status;
  int options;
  struct waitpid_result *next;
};

/* A singly-linked list of the results of the waitpid calls performed
   in the async SIGCHLD handler.  */
static struct waitpid_result *waitpid_queue = NULL;

static int
queued_waitpid (int pid, int *status, int flags)
{
  struct waitpid_result *msg = waitpid_queue, *prev = NULL;

  if (debug_linux_nat_async)
    fprintf_unfiltered (gdb_stdlog,
			"\
QWPID: linux_nat_async_events_state(%d), linux_nat_num_queued_events(%d)\n",
			linux_nat_async_events_state,
			linux_nat_num_queued_events);

  if (flags & __WALL)
    {
      for (; msg; prev = msg, msg = msg->next)
	if (pid == -1 || pid == msg->pid)
	  break;
    }
  else if (flags & __WCLONE)
    {
      for (; msg; prev = msg, msg = msg->next)
	if (msg->options & __WCLONE
	    && (pid == -1 || pid == msg->pid))
	  break;
    }
  else
    {
      for (; msg; prev = msg, msg = msg->next)
	if ((msg->options & __WCLONE) == 0
	    && (pid == -1 || pid == msg->pid))
	  break;
    }

  if (msg)
    {
      int pid;

      if (prev)
	prev->next = msg->next;
      else
	waitpid_queue = msg->next;

      msg->next = NULL;
      if (status)
	*status = msg->status;
      pid = msg->pid;

      if (debug_linux_nat_async)
	fprintf_unfiltered (gdb_stdlog, "QWPID: pid(%d), status(%x)\n",
			    pid, msg->status);
      xfree (msg);

      return pid;
    }

  if (debug_linux_nat_async)
    fprintf_unfiltered (gdb_stdlog, "QWPID: miss\n");

  if (status)
    *status = 0;
  return -1;
}

static void
push_waitpid (int pid, int status, int options)
{
  struct waitpid_result *event, *new_event;

  new_event = xmalloc (sizeof (*new_event));
  new_event->pid = pid;
  new_event->status = status;
  new_event->options = options;
  new_event->next = NULL;

  if (waitpid_queue)
    {
      for (event = waitpid_queue;
	   event && event->next;
	   event = event->next)
	;

      event->next = new_event;
    }
  else
    waitpid_queue = new_event;
}

/* Drain all queued events of PID.  If PID is -1, the effect is of
   draining all events.  */
static void
drain_queued_events (int pid)
{
  while (queued_waitpid (pid, NULL, __WALL) != -1)
    ;
}


/* Trivial list manipulation functions to keep track of a list of
   new stopped processes.  */
static void
add_to_pid_list (struct simple_pid_list **listp, int pid, int status)
{
  struct simple_pid_list *new_pid = xmalloc (sizeof (struct simple_pid_list));
  new_pid->pid = pid;
  new_pid->status = status;
  new_pid->next = *listp;
  *listp = new_pid;
}

static int
pull_pid_from_list (struct simple_pid_list **listp, int pid, int *status)
{
  struct simple_pid_list **p;

  for (p = listp; *p != NULL; p = &(*p)->next)
    if ((*p)->pid == pid)
      {
	struct simple_pid_list *next = (*p)->next;
	*status = (*p)->status;
	xfree (*p);
	*p = next;
	return 1;
      }
  return 0;
}

static void
linux_record_stopped_pid (int pid, int status)
{
  add_to_pid_list (&stopped_pids, pid, status);
}


/* A helper function for linux_test_for_tracefork, called after fork ().  */

static void
linux_tracefork_child (void)
{
  int ret;

  ptrace (PTRACE_TRACEME, 0, 0, 0);
  kill (getpid (), SIGSTOP);
  fork ();
  _exit (0);
}

/* Wrapper function for waitpid which handles EINTR, and checks for
   locally queued events.  */

static int
my_waitpid (int pid, int *status, int flags)
{
  int ret;

  /* There should be no concurrent calls to waitpid.  */
  gdb_assert (linux_nat_async_events_state == sigchld_sync);

  ret = queued_waitpid (pid, status, flags);
  if (ret != -1)
    return ret;

  do
    {
      ret = waitpid (pid, status, flags);
    }
  while (ret == -1 && errno == EINTR);

  return ret;
}

/* Determine if PTRACE_O_TRACEFORK can be used to follow fork events.

   First, we try to enable fork tracing on ORIGINAL_PID.  If this fails,
   we know that the feature is not available.  This may change the tracing
   options for ORIGINAL_PID, but we'll be setting them shortly anyway.

   However, if it succeeds, we don't know for sure that the feature is
   available; old versions of PTRACE_SETOPTIONS ignored unknown options.  We
   create a child process, attach to it, use PTRACE_SETOPTIONS to enable
   fork tracing, and let it fork.  If the process exits, we assume that we
   can't use TRACEFORK; if we get the fork notification, and we can extract
   the new child's PID, then we assume that we can.  */

static void
linux_test_for_tracefork (int original_pid)
{
  int child_pid, ret, status;
  long second_pid;

  linux_supports_tracefork_flag = 0;
  linux_supports_tracevforkdone_flag = 0;

  ret = ptrace (PTRACE_SETOPTIONS, original_pid, 0, PTRACE_O_TRACEFORK);
  if (ret != 0)
    return;

  child_pid = fork ();
  if (child_pid == -1)
    perror_with_name (("fork"));

  if (child_pid == 0)
    linux_tracefork_child ();

  ret = my_waitpid (child_pid, &status, 0);
  if (ret == -1)
    perror_with_name (("waitpid"));
  else if (ret != child_pid)
    error (_("linux_test_for_tracefork: waitpid: unexpected result %d."), ret);
  if (! WIFSTOPPED (status))
    error (_("linux_test_for_tracefork: waitpid: unexpected status %d."), status);

  ret = ptrace (PTRACE_SETOPTIONS, child_pid, 0, PTRACE_O_TRACEFORK);
  if (ret != 0)
    {
      ret = ptrace (PTRACE_KILL, child_pid, 0, 0);
      if (ret != 0)
	{
	  warning (_("linux_test_for_tracefork: failed to kill child"));
	  return;
	}

      ret = my_waitpid (child_pid, &status, 0);
      if (ret != child_pid)
	warning (_("linux_test_for_tracefork: failed to wait for killed child"));
      else if (!WIFSIGNALED (status))
	warning (_("linux_test_for_tracefork: unexpected wait status 0x%x from "
		 "killed child"), status);

      return;
    }

  /* Check whether PTRACE_O_TRACEVFORKDONE is available.  */
  ret = ptrace (PTRACE_SETOPTIONS, child_pid, 0,
		PTRACE_O_TRACEFORK | PTRACE_O_TRACEVFORKDONE);
  linux_supports_tracevforkdone_flag = (ret == 0);

  ret = ptrace (PTRACE_CONT, child_pid, 0, 0);
  if (ret != 0)
    warning (_("linux_test_for_tracefork: failed to resume child"));

  ret = my_waitpid (child_pid, &status, 0);

  if (ret == child_pid && WIFSTOPPED (status)
      && status >> 16 == PTRACE_EVENT_FORK)
    {
      second_pid = 0;
      ret = ptrace (PTRACE_GETEVENTMSG, child_pid, 0, &second_pid);
      if (ret == 0 && second_pid != 0)
	{
	  int second_status;

	  linux_supports_tracefork_flag = 1;
	  my_waitpid (second_pid, &second_status, 0);
	  ret = ptrace (PTRACE_KILL, second_pid, 0, 0);
	  if (ret != 0)
	    warning (_("linux_test_for_tracefork: failed to kill second child"));
	  my_waitpid (second_pid, &status, 0);
	}
    }
  else
    warning (_("linux_test_for_tracefork: unexpected result from waitpid "
	     "(%d, status 0x%x)"), ret, status);

  ret = ptrace (PTRACE_KILL, child_pid, 0, 0);
  if (ret != 0)
    warning (_("linux_test_for_tracefork: failed to kill child"));
  my_waitpid (child_pid, &status, 0);
}

/* Return non-zero iff we have tracefork functionality available.
   This function also sets linux_supports_tracefork_flag.  */

static int
linux_supports_tracefork (int pid)
{
  if (linux_supports_tracefork_flag == -1)
    linux_test_for_tracefork (pid);
  return linux_supports_tracefork_flag;
}

static int
linux_supports_tracevforkdone (int pid)
{
  if (linux_supports_tracefork_flag == -1)
    linux_test_for_tracefork (pid);
  return linux_supports_tracevforkdone_flag;
}


void
linux_enable_event_reporting (ptid_t ptid)
{
  int pid = ptid_get_lwp (ptid);
  int options;

  if (pid == 0)
    pid = ptid_get_pid (ptid);

  if (! linux_supports_tracefork (pid))
    return;

  options = PTRACE_O_TRACEFORK | PTRACE_O_TRACEVFORK | PTRACE_O_TRACEEXEC
    | PTRACE_O_TRACECLONE;
  if (linux_supports_tracevforkdone (pid))
    options |= PTRACE_O_TRACEVFORKDONE;

  /* Do not enable PTRACE_O_TRACEEXIT until GDB is more prepared to support
     read-only process state.  */

  ptrace (PTRACE_SETOPTIONS, pid, 0, options);
}

static void
linux_child_post_attach (int pid)
{
  linux_enable_event_reporting (pid_to_ptid (pid));
  check_for_thread_db ();
}

static void
linux_child_post_startup_inferior (ptid_t ptid)
{
  linux_enable_event_reporting (ptid);
  check_for_thread_db ();
}

static int
linux_child_follow_fork (struct target_ops *ops, int follow_child)
{
  ptid_t last_ptid;
  struct target_waitstatus last_status;
  int has_vforked;
  int parent_pid, child_pid;

  if (target_can_async_p ())
    target_async (NULL, 0);

  get_last_target_status (&last_ptid, &last_status);
  has_vforked = (last_status.kind == TARGET_WAITKIND_VFORKED);
  parent_pid = ptid_get_lwp (last_ptid);
  if (parent_pid == 0)
    parent_pid = ptid_get_pid (last_ptid);
  child_pid = last_status.value.related_pid;

  if (! follow_child)
    {
      /* We're already attached to the parent, by default. */

      /* Before detaching from the child, remove all breakpoints from
         it.  (This won't actually modify the breakpoint list, but will
         physically remove the breakpoints from the child.) */
      /* If we vforked this will remove the breakpoints from the parent
	 also, but they'll be reinserted below.  */
      detach_breakpoints (child_pid);

      /* Detach new forked process?  */
      if (detach_fork)
	{
	  if (info_verbose || debug_linux_nat)
	    {
	      target_terminal_ours ();
	      fprintf_filtered (gdb_stdlog,
				"Detaching after fork from child process %d.\n",
				child_pid);
	    }

	  ptrace (PTRACE_DETACH, child_pid, 0, 0);
	}
      else
	{
	  struct fork_info *fp;
	  /* Retain child fork in ptrace (stopped) state.  */
	  fp = find_fork_pid (child_pid);
	  if (!fp)
	    fp = add_fork (child_pid);
	  fork_save_infrun_state (fp, 0);
	}

      if (has_vforked)
	{
	  gdb_assert (linux_supports_tracefork_flag >= 0);
	  if (linux_supports_tracevforkdone (0))
	    {
	      int status;

	      ptrace (PTRACE_CONT, parent_pid, 0, 0);
	      my_waitpid (parent_pid, &status, __WALL);
	      if ((status >> 16) != PTRACE_EVENT_VFORK_DONE)
		warning (_("Unexpected waitpid result %06x when waiting for "
			 "vfork-done"), status);
	    }
	  else
	    {
	      /* We can't insert breakpoints until the child has
		 finished with the shared memory region.  We need to
		 wait until that happens.  Ideal would be to just
		 call:
		 - ptrace (PTRACE_SYSCALL, parent_pid, 0, 0);
		 - waitpid (parent_pid, &status, __WALL);
		 However, most architectures can't handle a syscall
		 being traced on the way out if it wasn't traced on
		 the way in.

		 We might also think to loop, continuing the child
		 until it exits or gets a SIGTRAP.  One problem is
		 that the child might call ptrace with PTRACE_TRACEME.

		 There's no simple and reliable way to figure out when
		 the vforked child will be done with its copy of the
		 shared memory.  We could step it out of the syscall,
		 two instructions, let it go, and then single-step the
		 parent once.  When we have hardware single-step, this
		 would work; with software single-step it could still
		 be made to work but we'd have to be able to insert
		 single-step breakpoints in the child, and we'd have
		 to insert -just- the single-step breakpoint in the
		 parent.  Very awkward.

		 In the end, the best we can do is to make sure it
		 runs for a little while.  Hopefully it will be out of
		 range of any breakpoints we reinsert.  Usually this
		 is only the single-step breakpoint at vfork's return
		 point.  */

	      usleep (10000);
	    }

	  /* Since we vforked, breakpoints were removed in the parent
	     too.  Put them back.  */
	  reattach_breakpoints (parent_pid);
	}
    }
  else
    {
      char child_pid_spelling[40];

      /* Needed to keep the breakpoint lists in sync.  */
      if (! has_vforked)
	detach_breakpoints (child_pid);

      /* Before detaching from the parent, remove all breakpoints from it. */
      remove_breakpoints ();

      if (info_verbose || debug_linux_nat)
	{
	  target_terminal_ours ();
	  fprintf_filtered (gdb_stdlog,
			    "Attaching after fork to child process %d.\n",
			    child_pid);
	}

      /* If we're vforking, we may want to hold on to the parent until
	 the child exits or execs.  At exec time we can remove the old
	 breakpoints from the parent and detach it; at exit time we
	 could do the same (or even, sneakily, resume debugging it - the
	 child's exec has failed, or something similar).

	 This doesn't clean up "properly", because we can't call
	 target_detach, but that's OK; if the current target is "child",
	 then it doesn't need any further cleanups, and lin_lwp will
	 generally not encounter vfork (vfork is defined to fork
	 in libpthread.so).

	 The holding part is very easy if we have VFORKDONE events;
	 but keeping track of both processes is beyond GDB at the
	 moment.  So we don't expose the parent to the rest of GDB.
	 Instead we quietly hold onto it until such time as we can
	 safely resume it.  */

      if (has_vforked)
	linux_parent_pid = parent_pid;
      else if (!detach_fork)
	{
	  struct fork_info *fp;
	  /* Retain parent fork in ptrace (stopped) state.  */
	  fp = find_fork_pid (parent_pid);
	  if (!fp)
	    fp = add_fork (parent_pid);
	  fork_save_infrun_state (fp, 0);
	}
      else
	target_detach (NULL, 0);

      inferior_ptid = ptid_build (child_pid, child_pid, 0);

      /* Reinstall ourselves, since we might have been removed in
	 target_detach (which does other necessary cleanup).  */

      push_target (ops);
      linux_nat_switch_fork (inferior_ptid);
      check_for_thread_db ();

      /* Reset breakpoints in the child as appropriate.  */
      follow_inferior_reset_breakpoints ();
    }

  if (target_can_async_p ())
    target_async (inferior_event_handler, 0);

  return 0;
}


static void
linux_child_insert_fork_catchpoint (int pid)
{
  if (! linux_supports_tracefork (pid))
    error (_("Your system does not support fork catchpoints."));
}

static void
linux_child_insert_vfork_catchpoint (int pid)
{
  if (!linux_supports_tracefork (pid))
    error (_("Your system does not support vfork catchpoints."));
}

static void
linux_child_insert_exec_catchpoint (int pid)
{
  if (!linux_supports_tracefork (pid))
    error (_("Your system does not support exec catchpoints."));
}

/* On GNU/Linux there are no real LWP's.  The closest thing to LWP's
   are processes sharing the same VM space.  A multi-threaded process
   is basically a group of such processes.  However, such a grouping
   is almost entirely a user-space issue; the kernel doesn't enforce
   such a grouping at all (this might change in the future).  In
   general, we'll rely on the threads library (i.e. the GNU/Linux
   Threads library) to provide such a grouping.

   It is perfectly well possible to write a multi-threaded application
   without the assistance of a threads library, by using the clone
   system call directly.  This module should be able to give some
   rudimentary support for debugging such applications if developers
   specify the CLONE_PTRACE flag in the clone system call, and are
   using the Linux kernel 2.4 or above.

   Note that there are some peculiarities in GNU/Linux that affect
   this code:

   - In general one should specify the __WCLONE flag to waitpid in
     order to make it report events for any of the cloned processes
     (and leave it out for the initial process).  However, if a cloned
     process has exited the exit status is only reported if the
     __WCLONE flag is absent.  Linux kernel 2.4 has a __WALL flag, but
     we cannot use it since GDB must work on older systems too.

   - When a traced, cloned process exits and is waited for by the
     debugger, the kernel reassigns it to the original parent and
     keeps it around as a "zombie".  Somehow, the GNU/Linux Threads
     library doesn't notice this, which leads to the "zombie problem":
     When debugged a multi-threaded process that spawns a lot of
     threads will run out of processes, even if the threads exit,
     because the "zombies" stay around.  */

/* List of known LWPs.  */
struct lwp_info *lwp_list;

/* Number of LWPs in the list.  */
static int num_lwps;


/* Original signal mask.  */
static sigset_t normal_mask;

/* Signal mask for use with sigsuspend in linux_nat_wait, initialized in
   _initialize_linux_nat.  */
static sigset_t suspend_mask;

/* SIGCHLD action for synchronous mode.  */
struct sigaction sync_sigchld_action;

/* SIGCHLD action for asynchronous mode.  */
static struct sigaction async_sigchld_action;

/* SIGCHLD default action, to pass to new inferiors.  */
static struct sigaction sigchld_default_action;


/* Prototypes for local functions.  */
static int stop_wait_callback (struct lwp_info *lp, void *data);
static int linux_nat_thread_alive (ptid_t ptid);
static char *linux_child_pid_to_exec_file (int pid);
static int cancel_breakpoint (struct lwp_info *lp);


/* Convert wait status STATUS to a string.  Used for printing debug
   messages only.  */

static char *
status_to_str (int status)
{
  static char buf[64];

  if (WIFSTOPPED (status))
    snprintf (buf, sizeof (buf), "%s (stopped)",
	      strsignal (WSTOPSIG (status)));
  else if (WIFSIGNALED (status))
    snprintf (buf, sizeof (buf), "%s (terminated)",
	      strsignal (WSTOPSIG (status)));
  else
    snprintf (buf, sizeof (buf), "%d (exited)", WEXITSTATUS (status));

  return buf;
}

/* Initialize the list of LWPs.  Note that this module, contrary to
   what GDB's generic threads layer does for its thread list,
   re-initializes the LWP lists whenever we mourn or detach (which
   doesn't involve mourning) the inferior.  */

static void
init_lwp_list (void)
{
  struct lwp_info *lp, *lpnext;

  for (lp = lwp_list; lp; lp = lpnext)
    {
      lpnext = lp->next;
      xfree (lp);
    }

  lwp_list = NULL;
  num_lwps = 0;
}

/* Add the LWP specified by PID to the list.  Return a pointer to the
   structure describing the new LWP.  The LWP should already be stopped
   (with an exception for the very first LWP).  */

static struct lwp_info *
add_lwp (ptid_t ptid)
{
  struct lwp_info *lp;

  gdb_assert (is_lwp (ptid));

  lp = (struct lwp_info *) xmalloc (sizeof (struct lwp_info));

  memset (lp, 0, sizeof (struct lwp_info));

  lp->waitstatus.kind = TARGET_WAITKIND_IGNORE;

  lp->ptid = ptid;

  lp->next = lwp_list;
  lwp_list = lp;
  ++num_lwps;

  if (num_lwps > 1 && linux_nat_new_thread != NULL)
    linux_nat_new_thread (ptid);

  return lp;
}

/* Remove the LWP specified by PID from the list.  */

static void
delete_lwp (ptid_t ptid)
{
  struct lwp_info *lp, *lpprev;

  lpprev = NULL;

  for (lp = lwp_list; lp; lpprev = lp, lp = lp->next)
    if (ptid_equal (lp->ptid, ptid))
      break;

  if (!lp)
    return;

  num_lwps--;

  if (lpprev)
    lpprev->next = lp->next;
  else
    lwp_list = lp->next;

  xfree (lp);
}

/* Return a pointer to the structure describing the LWP corresponding
   to PID.  If no corresponding LWP could be found, return NULL.  */

static struct lwp_info *
find_lwp_pid (ptid_t ptid)
{
  struct lwp_info *lp;
  int lwp;

  if (is_lwp (ptid))
    lwp = GET_LWP (ptid);
  else
    lwp = GET_PID (ptid);

  for (lp = lwp_list; lp; lp = lp->next)
    if (lwp == GET_LWP (lp->ptid))
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
  struct lwp_info *lp, *lpnext;

  for (lp = lwp_list; lp; lp = lpnext)
    {
      lpnext = lp->next;
      if ((*callback) (lp, data))
	return lp;
    }

  return NULL;
}

/* Update our internal state when changing from one fork (checkpoint,
   et cetera) to another indicated by NEW_PTID.  We can only switch
   single-threaded applications, so we only create one new LWP, and
   the previous list is discarded.  */

void
linux_nat_switch_fork (ptid_t new_ptid)
{
  struct lwp_info *lp;

  init_thread_list ();
  init_lwp_list ();
  lp = add_lwp (new_ptid);
  add_thread_silent (new_ptid);
  lp->stopped = 1;
}

/* Record a PTID for later deletion.  */

struct saved_ptids
{
  ptid_t ptid;
  struct saved_ptids *next;
};
static struct saved_ptids *threads_to_delete;

static void
record_dead_thread (ptid_t ptid)
{
  struct saved_ptids *p = xmalloc (sizeof (struct saved_ptids));
  p->ptid = ptid;
  p->next = threads_to_delete;
  threads_to_delete = p;
}

/* Delete any dead threads which are not the current thread.  */

static void
prune_lwps (void)
{
  struct saved_ptids **p = &threads_to_delete;

  while (*p)
    if (! ptid_equal ((*p)->ptid, inferior_ptid))
      {
	struct saved_ptids *tmp = *p;
	delete_thread (tmp->ptid);
	*p = tmp->next;
	xfree (tmp);
      }
    else
      p = &(*p)->next;
}

/* Handle the exit of a single thread LP.  */

static void
exit_lwp (struct lwp_info *lp)
{
  struct thread_info *th = find_thread_pid (lp->ptid);

  if (th)
    {
      if (print_thread_events)
	printf_unfiltered (_("[%s exited]\n"), target_pid_to_str (lp->ptid));

      /* Core GDB cannot deal with us deleting the current thread.  */
      if (!ptid_equal (lp->ptid, inferior_ptid))
	delete_thread (lp->ptid);
      else
	record_dead_thread (lp->ptid);
    }

  delete_lwp (lp->ptid);
}

/* Detect `T (stopped)' in `/proc/PID/status'.
   Other states including `T (tracing stop)' are reported as false.  */

static int
pid_is_stopped (pid_t pid)
{
  FILE *status_file;
  char buf[100];
  int retval = 0;

  snprintf (buf, sizeof (buf), "/proc/%d/status", (int) pid);
  status_file = fopen (buf, "r");
  if (status_file != NULL)
    {
      int have_state = 0;

      while (fgets (buf, sizeof (buf), status_file))
	{
	  if (strncmp (buf, "State:", 6) == 0)
	    {
	      have_state = 1;
	      break;
	    }
	}
      if (have_state && strstr (buf, "T (stopped)") != NULL)
	retval = 1;
      fclose (status_file);
    }
  return retval;
}

/* Wait for the LWP specified by LP, which we have just attached to.
   Returns a wait status for that LWP, to cache.  */

static int
linux_nat_post_attach_wait (ptid_t ptid, int first, int *cloned,
			    int *signalled)
{
  pid_t new_pid, pid = GET_LWP (ptid);
  int status;

  if (pid_is_stopped (pid))
    {
      if (debug_linux_nat)
	fprintf_unfiltered (gdb_stdlog,
			    "LNPAW: Attaching to a stopped process\n");

      /* The process is definitely stopped.  It is in a job control
	 stop, unless the kernel predates the TASK_STOPPED /
	 TASK_TRACED distinction, in which case it might be in a
	 ptrace stop.  Make sure it is in a ptrace stop; from there we
	 can kill it, signal it, et cetera.

         First make sure there is a pending SIGSTOP.  Since we are
	 already attached, the process can not transition from stopped
	 to running without a PTRACE_CONT; so we know this signal will
	 go into the queue.  The SIGSTOP generated by PTRACE_ATTACH is
	 probably already in the queue (unless this kernel is old
	 enough to use TASK_STOPPED for ptrace stops); but since SIGSTOP
	 is not an RT signal, it can only be queued once.  */
      kill_lwp (pid, SIGSTOP);

      /* Finally, resume the stopped process.  This will deliver the SIGSTOP
	 (or a higher priority signal, just like normal PTRACE_ATTACH).  */
      ptrace (PTRACE_CONT, pid, 0, 0);
    }

  /* Make sure the initial process is stopped.  The user-level threads
     layer might want to poke around in the inferior, and that won't
     work if things haven't stabilized yet.  */
  new_pid = my_waitpid (pid, &status, 0);
  if (new_pid == -1 && errno == ECHILD)
    {
      if (first)
	warning (_("%s is a cloned process"), target_pid_to_str (ptid));

      /* Try again with __WCLONE to check cloned processes.  */
      new_pid = my_waitpid (pid, &status, __WCLONE);
      *cloned = 1;
    }

  gdb_assert (pid == new_pid && WIFSTOPPED (status));

  if (WSTOPSIG (status) != SIGSTOP)
    {
      *signalled = 1;
      if (debug_linux_nat)
	fprintf_unfiltered (gdb_stdlog,
			    "LNPAW: Received %s after attaching\n",
			    status_to_str (status));
    }

  return status;
}

/* Attach to the LWP specified by PID.  Return 0 if successful or -1
   if the new LWP could not be attached.  */

int
lin_lwp_attach_lwp (ptid_t ptid)
{
  struct lwp_info *lp;
  enum sigchld_state async_events_original_state;

  gdb_assert (is_lwp (ptid));

  async_events_original_state = linux_nat_async_events (sigchld_sync);

  lp = find_lwp_pid (ptid);

  /* We assume that we're already attached to any LWP that has an id
     equal to the overall process id, and to any LWP that is already
     in our list of LWPs.  If we're not seeing exit events from threads
     and we've had PID wraparound since we last tried to stop all threads,
     this assumption might be wrong; fortunately, this is very unlikely
     to happen.  */
  if (GET_LWP (ptid) != GET_PID (ptid) && lp == NULL)
    {
      int status, cloned = 0, signalled = 0;

      if (ptrace (PTRACE_ATTACH, GET_LWP (ptid), 0, 0) < 0)
	{
	  /* If we fail to attach to the thread, issue a warning,
	     but continue.  One way this can happen is if thread
	     creation is interrupted; as of Linux kernel 2.6.19, a
	     bug may place threads in the thread list and then fail
	     to create them.  */
	  warning (_("Can't attach %s: %s"), target_pid_to_str (ptid),
		   safe_strerror (errno));
	  return -1;
	}

      if (debug_linux_nat)
	fprintf_unfiltered (gdb_stdlog,
			    "LLAL: PTRACE_ATTACH %s, 0, 0 (OK)\n",
			    target_pid_to_str (ptid));

      status = linux_nat_post_attach_wait (ptid, 0, &cloned, &signalled);
      lp = add_lwp (ptid);
      lp->stopped = 1;
      lp->cloned = cloned;
      lp->signalled = signalled;
      if (WSTOPSIG (status) != SIGSTOP)
	{
	  lp->resumed = 1;
	  lp->status = status;
	}

      target_post_attach (GET_LWP (lp->ptid));

      if (debug_linux_nat)
	{
	  fprintf_unfiltered (gdb_stdlog,
			      "LLAL: waitpid %s received %s\n",
			      target_pid_to_str (ptid),
			      status_to_str (status));
	}
    }
  else
    {
      /* We assume that the LWP representing the original process is
         already stopped.  Mark it as stopped in the data structure
         that the GNU/linux ptrace layer uses to keep track of
         threads.  Note that this won't have already been done since
         the main thread will have, we assume, been stopped by an
         attach from a different layer.  */
      if (lp == NULL)
	lp = add_lwp (ptid);
      lp->stopped = 1;
    }

  linux_nat_async_events (async_events_original_state);
  return 0;
}

static void
linux_nat_create_inferior (char *exec_file, char *allargs, char **env,
			   int from_tty)
{
  int saved_async = 0;

  /* The fork_child mechanism is synchronous and calls target_wait, so
     we have to mask the async mode.  */

  if (target_can_async_p ())
    /* Mask async mode.  Creating a child requires a loop calling
       wait_for_inferior currently.  */
    saved_async = linux_nat_async_mask (0);
  else
    {
      /* Restore the original signal mask.  */
      sigprocmask (SIG_SETMASK, &normal_mask, NULL);
      /* Make sure we don't block SIGCHLD during a sigsuspend.  */
      suspend_mask = normal_mask;
      sigdelset (&suspend_mask, SIGCHLD);
    }

  /* Set SIGCHLD to the default action, until after execing the child,
     since the inferior inherits the superior's signal mask.  It will
     be blocked again in linux_nat_wait, which is only reached after
     the inferior execing.  */
  linux_nat_async_events (sigchld_default);

  linux_ops->to_create_inferior (exec_file, allargs, env, from_tty);

  if (saved_async)
    linux_nat_async_mask (saved_async);
}

static void
linux_nat_attach (char *args, int from_tty)
{
  struct lwp_info *lp;
  int status;

  /* FIXME: We should probably accept a list of process id's, and
     attach all of them.  */
  linux_ops->to_attach (args, from_tty);

  if (!target_can_async_p ())
    {
      /* Restore the original signal mask.  */
      sigprocmask (SIG_SETMASK, &normal_mask, NULL);
      /* Make sure we don't block SIGCHLD during a sigsuspend.  */
      suspend_mask = normal_mask;
      sigdelset (&suspend_mask, SIGCHLD);
    }

  /* Add the initial process as the first LWP to the list.  */
  inferior_ptid = BUILD_LWP (GET_PID (inferior_ptid), GET_PID (inferior_ptid));
  lp = add_lwp (inferior_ptid);

  status = linux_nat_post_attach_wait (lp->ptid, 1, &lp->cloned,
				       &lp->signalled);
  lp->stopped = 1;

  /* If this process is not using thread_db, then we still don't
     detect any other threads, but add at least this one.  */
  add_thread_silent (lp->ptid);

  /* Save the wait status to report later.  */
  lp->resumed = 1;
  if (debug_linux_nat)
    fprintf_unfiltered (gdb_stdlog,
			"LNA: waitpid %ld, saving status %s\n",
			(long) GET_PID (lp->ptid), status_to_str (status));

  if (!target_can_async_p ())
    lp->status = status;
  else
    {
      /* We already waited for this LWP, so put the wait result on the
	 pipe.  The event loop will wake up and gets us to handling
	 this event.  */
      linux_nat_event_pipe_push (GET_PID (lp->ptid), status,
				 lp->cloned ? __WCLONE : 0);
      /* Register in the event loop.  */
      target_async (inferior_event_handler, 0);
    }
}

/* Get pending status of LP.  */
static int
get_pending_status (struct lwp_info *lp, int *status)
{
  struct target_waitstatus last;
  ptid_t last_ptid;

  get_last_target_status (&last_ptid, &last);

  /* If this lwp is the ptid that GDB is processing an event from, the
     signal will be in stop_signal.  Otherwise, in all-stop + sync
     mode, we may cache pending events in lp->status while trying to
     stop all threads (see stop_wait_callback).  In async mode, the
     events are always cached in waitpid_queue.  */

  *status = 0;
  if (GET_LWP (lp->ptid) == GET_LWP (last_ptid))
    {
      if (stop_signal != TARGET_SIGNAL_0
	  && signal_pass_state (stop_signal))
	*status = W_STOPCODE (target_signal_to_host (stop_signal));
    }
  else if (target_can_async_p ())
    queued_waitpid (GET_LWP (lp->ptid), status, __WALL);
  else
    *status = lp->status;

  return 0;
}

static int
detach_callback (struct lwp_info *lp, void *data)
{
  gdb_assert (lp->status == 0 || WIFSTOPPED (lp->status));

  if (debug_linux_nat && lp->status)
    fprintf_unfiltered (gdb_stdlog, "DC:  Pending %s for %s on detach.\n",
			strsignal (WSTOPSIG (lp->status)),
			target_pid_to_str (lp->ptid));

  /* If there is a pending SIGSTOP, get rid of it.  */
  if (lp->signalled)
    {
      if (debug_linux_nat)
	fprintf_unfiltered (gdb_stdlog,
			    "DC: Sending SIGCONT to %s\n",
			    target_pid_to_str (lp->ptid));

      kill_lwp (GET_LWP (lp->ptid), SIGCONT);
      lp->signalled = 0;
    }

  /* We don't actually detach from the LWP that has an id equal to the
     overall process id just yet.  */
  if (GET_LWP (lp->ptid) != GET_PID (lp->ptid))
    {
      int status = 0;

      /* Pass on any pending signal for this LWP.  */
      get_pending_status (lp, &status);

      errno = 0;
      if (ptrace (PTRACE_DETACH, GET_LWP (lp->ptid), 0,
		  WSTOPSIG (status)) < 0)
	error (_("Can't detach %s: %s"), target_pid_to_str (lp->ptid),
	       safe_strerror (errno));

      if (debug_linux_nat)
	fprintf_unfiltered (gdb_stdlog,
			    "PTRACE_DETACH (%s, %s, 0) (OK)\n",
			    target_pid_to_str (lp->ptid),
			    strsignal (WSTOPSIG (lp->status)));

      delete_lwp (lp->ptid);
    }

  return 0;
}

static void
linux_nat_detach (char *args, int from_tty)
{
  int pid;
  int status;
  enum target_signal sig;

  if (target_can_async_p ())
    linux_nat_async (NULL, 0);

  iterate_over_lwps (detach_callback, NULL);

  /* Only the initial process should be left right now.  */
  gdb_assert (num_lwps == 1);

  /* Pass on any pending signal for the last LWP.  */
  if ((args == NULL || *args == '\0')
      && get_pending_status (lwp_list, &status) != -1
      && WIFSTOPPED (status))
    {
      /* Put the signal number in ARGS so that inf_ptrace_detach will
	 pass it along with PTRACE_DETACH.  */
      args = alloca (8);
      sprintf (args, "%d", (int) WSTOPSIG (status));
      fprintf_unfiltered (gdb_stdlog,
			  "LND: Sending signal %s to %s\n",
			  args,
			  target_pid_to_str (lwp_list->ptid));
    }

  /* Destroy LWP info; it's no longer valid.  */
  init_lwp_list ();

  pid = GET_PID (inferior_ptid);
  inferior_ptid = pid_to_ptid (pid);
  linux_ops->to_detach (args, from_tty);

  if (target_can_async_p ())
    drain_queued_events (pid);
}

/* Resume LP.  */

static int
resume_callback (struct lwp_info *lp, void *data)
{
  if (lp->stopped && lp->status == 0)
    {
      linux_ops->to_resume (pid_to_ptid (GET_LWP (lp->ptid)),
			    0, TARGET_SIGNAL_0);
      if (debug_linux_nat)
	fprintf_unfiltered (gdb_stdlog,
			    "RC:  PTRACE_CONT %s, 0, 0 (resume sibling)\n",
			    target_pid_to_str (lp->ptid));
      lp->stopped = 0;
      lp->step = 0;
      memset (&lp->siginfo, 0, sizeof (lp->siginfo));
    }

  return 0;
}

static int
resume_clear_callback (struct lwp_info *lp, void *data)
{
  lp->resumed = 0;
  return 0;
}

static int
resume_set_callback (struct lwp_info *lp, void *data)
{
  lp->resumed = 1;
  return 0;
}

static void
linux_nat_resume (ptid_t ptid, int step, enum target_signal signo)
{
  struct lwp_info *lp;
  int resume_all;

  if (debug_linux_nat)
    fprintf_unfiltered (gdb_stdlog,
			"LLR: Preparing to %s %s, %s, inferior_ptid %s\n",
			step ? "step" : "resume",
			target_pid_to_str (ptid),
			signo ? strsignal (signo) : "0",
			target_pid_to_str (inferior_ptid));

  prune_lwps ();

  if (target_can_async_p ())
    /* Block events while we're here.  */
    linux_nat_async_events (sigchld_sync);

  /* A specific PTID means `step only this process id'.  */
  resume_all = (PIDGET (ptid) == -1);

  if (resume_all)
    iterate_over_lwps (resume_set_callback, NULL);
  else
    iterate_over_lwps (resume_clear_callback, NULL);

  /* If PID is -1, it's the current inferior that should be
     handled specially.  */
  if (PIDGET (ptid) == -1)
    ptid = inferior_ptid;

  lp = find_lwp_pid (ptid);
  gdb_assert (lp != NULL);

  ptid = pid_to_ptid (GET_LWP (lp->ptid));

  /* Remember if we're stepping.  */
  lp->step = step;

  /* Mark this LWP as resumed.  */
  lp->resumed = 1;

  /* If we have a pending wait status for this thread, there is no
     point in resuming the process.  But first make sure that
     linux_nat_wait won't preemptively handle the event - we
     should never take this short-circuit if we are going to
     leave LP running, since we have skipped resuming all the
     other threads.  This bit of code needs to be synchronized
     with linux_nat_wait.  */

  /* In async mode, we never have pending wait status.  */
  if (target_can_async_p () && lp->status)
    internal_error (__FILE__, __LINE__, "Pending status in async mode");

  if (lp->status && WIFSTOPPED (lp->status))
    {
      int saved_signo = target_signal_from_host (WSTOPSIG (lp->status));

      if (signal_stop_state (saved_signo) == 0
	  && signal_print_state (saved_signo) == 0
	  && signal_pass_state (saved_signo) == 1)
	{
	  if (debug_linux_nat)
	    fprintf_unfiltered (gdb_stdlog,
				"LLR: Not short circuiting for ignored "
				"status 0x%x\n", lp->status);

	  /* FIXME: What should we do if we are supposed to continue
	     this thread with a signal?  */
	  gdb_assert (signo == TARGET_SIGNAL_0);
	  signo = saved_signo;
	  lp->status = 0;
	}
    }

  if (lp->status)
    {
      /* FIXME: What should we do if we are supposed to continue
	 this thread with a signal?  */
      gdb_assert (signo == TARGET_SIGNAL_0);

      if (debug_linux_nat)
	fprintf_unfiltered (gdb_stdlog,
			    "LLR: Short circuiting for status 0x%x\n",
			    lp->status);

      return;
    }

  /* Mark LWP as not stopped to prevent it from being continued by
     resume_callback.  */
  lp->stopped = 0;

  if (resume_all)
    iterate_over_lwps (resume_callback, NULL);

  linux_ops->to_resume (ptid, step, signo);
  memset (&lp->siginfo, 0, sizeof (lp->siginfo));

  if (debug_linux_nat)
    fprintf_unfiltered (gdb_stdlog,
			"LLR: %s %s, %s (resume event thread)\n",
			step ? "PTRACE_SINGLESTEP" : "PTRACE_CONT",
			target_pid_to_str (ptid),
			signo ? strsignal (signo) : "0");

  if (target_can_async_p ())
    {
      target_executing = 1;
      target_async (inferior_event_handler, 0);
    }
}

/* Issue kill to specified lwp.  */

static int tkill_failed;

static int
kill_lwp (int lwpid, int signo)
{
  errno = 0;

/* Use tkill, if possible, in case we are using nptl threads.  If tkill
   fails, then we are not using nptl threads and we should be using kill.  */

#ifdef HAVE_TKILL_SYSCALL
  if (!tkill_failed)
    {
      int ret = syscall (__NR_tkill, lwpid, signo);
      if (errno != ENOSYS)
	return ret;
      errno = 0;
      tkill_failed = 1;
    }
#endif

  return kill (lwpid, signo);
}

/* Handle a GNU/Linux extended wait response.  If we see a clone
   event, we need to add the new LWP to our list (and not report the
   trap to higher layers).  This function returns non-zero if the
   event should be ignored and we should wait again.  If STOPPING is
   true, the new LWP remains stopped, otherwise it is continued.  */

static int
linux_handle_extended_wait (struct lwp_info *lp, int status,
			    int stopping)
{
  int pid = GET_LWP (lp->ptid);
  struct target_waitstatus *ourstatus = &lp->waitstatus;
  struct lwp_info *new_lp = NULL;
  int event = status >> 16;

  if (event == PTRACE_EVENT_FORK || event == PTRACE_EVENT_VFORK
      || event == PTRACE_EVENT_CLONE)
    {
      unsigned long new_pid;
      int ret;

      ptrace (PTRACE_GETEVENTMSG, pid, 0, &new_pid);

      /* If we haven't already seen the new PID stop, wait for it now.  */
      if (! pull_pid_from_list (&stopped_pids, new_pid, &status))
	{
	  /* The new child has a pending SIGSTOP.  We can't affect it until it
	     hits the SIGSTOP, but we're already attached.  */
	  ret = my_waitpid (new_pid, &status,
			    (event == PTRACE_EVENT_CLONE) ? __WCLONE : 0);
	  if (ret == -1)
	    perror_with_name (_("waiting for new child"));
	  else if (ret != new_pid)
	    internal_error (__FILE__, __LINE__,
			    _("wait returned unexpected PID %d"), ret);
	  else if (!WIFSTOPPED (status))
	    internal_error (__FILE__, __LINE__,
			    _("wait returned unexpected status 0x%x"), status);
	}

      ourstatus->value.related_pid = new_pid;

      if (event == PTRACE_EVENT_FORK)
	ourstatus->kind = TARGET_WAITKIND_FORKED;
      else if (event == PTRACE_EVENT_VFORK)
	ourstatus->kind = TARGET_WAITKIND_VFORKED;
      else
	{
	  ourstatus->kind = TARGET_WAITKIND_IGNORE;
	  new_lp = add_lwp (BUILD_LWP (new_pid, GET_PID (inferior_ptid)));
	  new_lp->cloned = 1;

	  if (WSTOPSIG (status) != SIGSTOP)
	    {
	      /* This can happen if someone starts sending signals to
		 the new thread before it gets a chance to run, which
		 have a lower number than SIGSTOP (e.g. SIGUSR1).
		 This is an unlikely case, and harder to handle for
		 fork / vfork than for clone, so we do not try - but
		 we handle it for clone events here.  We'll send
		 the other signal on to the thread below.  */

	      new_lp->signalled = 1;
	    }
	  else
	    status = 0;

	  if (stopping)
	    new_lp->stopped = 1;
	  else
	    {
	      new_lp->resumed = 1;
	      ptrace (PTRACE_CONT, lp->waitstatus.value.related_pid, 0,
		      status ? WSTOPSIG (status) : 0);
	    }

	  if (debug_linux_nat)
	    fprintf_unfiltered (gdb_stdlog,
				"LHEW: Got clone event from LWP %ld, resuming\n",
				GET_LWP (lp->ptid));
	  ptrace (PTRACE_CONT, GET_LWP (lp->ptid), 0, 0);

	  return 1;
	}

      return 0;
    }

  if (event == PTRACE_EVENT_EXEC)
    {
      ourstatus->kind = TARGET_WAITKIND_EXECD;
      ourstatus->value.execd_pathname
	= xstrdup (linux_child_pid_to_exec_file (pid));

      if (linux_parent_pid)
	{
	  detach_breakpoints (linux_parent_pid);
	  ptrace (PTRACE_DETACH, linux_parent_pid, 0, 0);

	  linux_parent_pid = 0;
	}

      return 0;
    }

  internal_error (__FILE__, __LINE__,
		  _("unknown ptrace event %d"), event);
}

/* Wait for LP to stop.  Returns the wait status, or 0 if the LWP has
   exited.  */

static int
wait_lwp (struct lwp_info *lp)
{
  pid_t pid;
  int status;
  int thread_dead = 0;

  gdb_assert (!lp->stopped);
  gdb_assert (lp->status == 0);

  pid = my_waitpid (GET_LWP (lp->ptid), &status, 0);
  if (pid == -1 && errno == ECHILD)
    {
      pid = my_waitpid (GET_LWP (lp->ptid), &status, __WCLONE);
      if (pid == -1 && errno == ECHILD)
	{
	  /* The thread has previously exited.  We need to delete it
	     now because, for some vendor 2.4 kernels with NPTL
	     support backported, there won't be an exit event unless
	     it is the main thread.  2.6 kernels will report an exit
	     event for each thread that exits, as expected.  */
	  thread_dead = 1;
	  if (debug_linux_nat)
	    fprintf_unfiltered (gdb_stdlog, "WL: %s vanished.\n",
				target_pid_to_str (lp->ptid));
	}
    }

  if (!thread_dead)
    {
      gdb_assert (pid == GET_LWP (lp->ptid));

      if (debug_linux_nat)
	{
	  fprintf_unfiltered (gdb_stdlog,
			      "WL: waitpid %s received %s\n",
			      target_pid_to_str (lp->ptid),
			      status_to_str (status));
	}
    }

  /* Check if the thread has exited.  */
  if (WIFEXITED (status) || WIFSIGNALED (status))
    {
      thread_dead = 1;
      if (debug_linux_nat)
	fprintf_unfiltered (gdb_stdlog, "WL: %s exited.\n",
			    target_pid_to_str (lp->ptid));
    }

  if (thread_dead)
    {
      exit_lwp (lp);
      return 0;
    }

  gdb_assert (WIFSTOPPED (status));

  /* Handle GNU/Linux's extended waitstatus for trace events.  */
  if (WIFSTOPPED (status) && WSTOPSIG (status) == SIGTRAP && status >> 16 != 0)
    {
      if (debug_linux_nat)
	fprintf_unfiltered (gdb_stdlog,
			    "WL: Handling extended status 0x%06x\n",
			    status);
      if (linux_handle_extended_wait (lp, status, 1))
	return wait_lwp (lp);
    }

  return status;
}

/* Save the most recent siginfo for LP.  This is currently only called
   for SIGTRAP; some ports use the si_addr field for
   target_stopped_data_address.  In the future, it may also be used to
   restore the siginfo of requeued signals.  */

static void
save_siginfo (struct lwp_info *lp)
{
  errno = 0;
  ptrace (PTRACE_GETSIGINFO, GET_LWP (lp->ptid),
	  (PTRACE_TYPE_ARG3) 0, &lp->siginfo);

  if (errno != 0)
    memset (&lp->siginfo, 0, sizeof (lp->siginfo));
}

/* Send a SIGSTOP to LP.  */

static int
stop_callback (struct lwp_info *lp, void *data)
{
  if (!lp->stopped && !lp->signalled)
    {
      int ret;

      if (debug_linux_nat)
	{
	  fprintf_unfiltered (gdb_stdlog,
			      "SC:  kill %s **<SIGSTOP>**\n",
			      target_pid_to_str (lp->ptid));
	}
      errno = 0;
      ret = kill_lwp (GET_LWP (lp->ptid), SIGSTOP);
      if (debug_linux_nat)
	{
	  fprintf_unfiltered (gdb_stdlog,
			      "SC:  lwp kill %d %s\n",
			      ret,
			      errno ? safe_strerror (errno) : "ERRNO-OK");
	}

      lp->signalled = 1;
      gdb_assert (lp->status == 0);
    }

  return 0;
}

/* Wait until LP is stopped.  If DATA is non-null it is interpreted as
   a pointer to a set of signals to be flushed immediately.  */

static int
stop_wait_callback (struct lwp_info *lp, void *data)
{
  sigset_t *flush_mask = data;

  if (!lp->stopped)
    {
      int status;

      status = wait_lwp (lp);
      if (status == 0)
	return 0;

      /* Ignore any signals in FLUSH_MASK.  */
      if (flush_mask && sigismember (flush_mask, WSTOPSIG (status)))
	{
	  if (!lp->signalled)
	    {
	      lp->stopped = 1;
	      return 0;
	    }

	  errno = 0;
	  ptrace (PTRACE_CONT, GET_LWP (lp->ptid), 0, 0);
	  if (debug_linux_nat)
	    fprintf_unfiltered (gdb_stdlog,
				"PTRACE_CONT %s, 0, 0 (%s)\n",
				target_pid_to_str (lp->ptid),
				errno ? safe_strerror (errno) : "OK");

	  return stop_wait_callback (lp, flush_mask);
	}

      if (WSTOPSIG (status) != SIGSTOP)
	{
	  if (WSTOPSIG (status) == SIGTRAP)
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

	      /* Save the trap's siginfo in case we need it later.  */
	      save_siginfo (lp);

	      /* Now resume this LWP and get the SIGSTOP event. */
	      errno = 0;
	      ptrace (PTRACE_CONT, GET_LWP (lp->ptid), 0, 0);
	      if (debug_linux_nat)
		{
		  fprintf_unfiltered (gdb_stdlog,
				      "PTRACE_CONT %s, 0, 0 (%s)\n",
				      target_pid_to_str (lp->ptid),
				      errno ? safe_strerror (errno) : "OK");

		  fprintf_unfiltered (gdb_stdlog,
				      "SWC: Candidate SIGTRAP event in %s\n",
				      target_pid_to_str (lp->ptid));
		}
	      /* Hold this event/waitstatus while we check to see if
		 there are any more (we still want to get that SIGSTOP). */
	      stop_wait_callback (lp, data);

	      if (target_can_async_p ())
		{
		  /* Don't leave a pending wait status in async mode.
		     Retrigger the breakpoint.  */
		  if (!cancel_breakpoint (lp))
		    {
		      /* There was no gdb breakpoint set at pc.  Put
			 the event back in the queue.  */
		      if (debug_linux_nat)
			fprintf_unfiltered (gdb_stdlog,
					    "SWC: kill %s, %s\n",
					    target_pid_to_str (lp->ptid),
					    status_to_str ((int) status));
		      kill_lwp (GET_LWP (lp->ptid), WSTOPSIG (status));
		    }
		}
	      else
		{
		  /* Hold the SIGTRAP for handling by
		     linux_nat_wait. */
		  /* If there's another event, throw it back into the
		     queue. */
		  if (lp->status)
		    {
		      if (debug_linux_nat)
			fprintf_unfiltered (gdb_stdlog,
					    "SWC: kill %s, %s\n",
					    target_pid_to_str (lp->ptid),
					    status_to_str ((int) status));
		      kill_lwp (GET_LWP (lp->ptid), WSTOPSIG (lp->status));
		    }
		  /* Save the sigtrap event. */
		  lp->status = status;
		}
	      return 0;
	    }
	  else
	    {
	      /* The thread was stopped with a signal other than
	         SIGSTOP, and didn't accidentally trip a breakpoint. */

	      if (debug_linux_nat)
		{
		  fprintf_unfiltered (gdb_stdlog,
				      "SWC: Pending event %s in %s\n",
				      status_to_str ((int) status),
				      target_pid_to_str (lp->ptid));
		}
	      /* Now resume this LWP and get the SIGSTOP event. */
	      errno = 0;
	      ptrace (PTRACE_CONT, GET_LWP (lp->ptid), 0, 0);
	      if (debug_linux_nat)
		fprintf_unfiltered (gdb_stdlog,
				    "SWC: PTRACE_CONT %s, 0, 0 (%s)\n",
				    target_pid_to_str (lp->ptid),
				    errno ? safe_strerror (errno) : "OK");

	      /* Hold this event/waitstatus while we check to see if
	         there are any more (we still want to get that SIGSTOP). */
	      stop_wait_callback (lp, data);

	      /* If the lp->status field is still empty, use it to
		 hold this event.  If not, then this event must be
		 returned to the event queue of the LWP.  */
	      if (lp->status || target_can_async_p ())
		{
		  if (debug_linux_nat)
		    {
		      fprintf_unfiltered (gdb_stdlog,
					  "SWC: kill %s, %s\n",
					  target_pid_to_str (lp->ptid),
					  status_to_str ((int) status));
		    }
		  kill_lwp (GET_LWP (lp->ptid), WSTOPSIG (status));
		}
	      else
		lp->status = status;
	      return 0;
	    }
	}
      else
	{
	  /* We caught the SIGSTOP that we intended to catch, so
	     there's no SIGSTOP pending.  */
	  lp->stopped = 1;
	  lp->signalled = 0;
	}
    }

  return 0;
}

/* Check whether PID has any pending signals in FLUSH_MASK.  If so set
   the appropriate bits in PENDING, and return 1 - otherwise return 0.  */

static int
linux_nat_has_pending (int pid, sigset_t *pending, sigset_t *flush_mask)
{
  sigset_t blocked, ignored;
  int i;

  linux_proc_pending_signals (pid, pending, &blocked, &ignored);

  if (!flush_mask)
    return 0;

  for (i = 1; i < NSIG; i++)
    if (sigismember (pending, i))
      if (!sigismember (flush_mask, i)
	  || sigismember (&blocked, i)
	  || sigismember (&ignored, i))
	sigdelset (pending, i);

  if (sigisemptyset (pending))
    return 0;

  return 1;
}

/* DATA is interpreted as a mask of signals to flush.  If LP has
   signals pending, and they are all in the flush mask, then arrange
   to flush them.  LP should be stopped, as should all other threads
   it might share a signal queue with.  */

static int
flush_callback (struct lwp_info *lp, void *data)
{
  sigset_t *flush_mask = data;
  sigset_t pending, intersection, blocked, ignored;
  int pid, status;

  /* Normally, when an LWP exits, it is removed from the LWP list.  The
     last LWP isn't removed till later, however.  So if there is only
     one LWP on the list, make sure it's alive.  */
  if (lwp_list == lp && lp->next == NULL)
    if (!linux_nat_thread_alive (lp->ptid))
      return 0;

  /* Just because the LWP is stopped doesn't mean that new signals
     can't arrive from outside, so this function must be careful of
     race conditions.  However, because all threads are stopped, we
     can assume that the pending mask will not shrink unless we resume
     the LWP, and that it will then get another signal.  We can't
     control which one, however.  */

  if (lp->status)
    {
      if (debug_linux_nat)
	printf_unfiltered (_("FC: LP has pending status %06x\n"), lp->status);
      if (WIFSTOPPED (lp->status) && sigismember (flush_mask, WSTOPSIG (lp->status)))
	lp->status = 0;
    }

  /* While there is a pending signal we would like to flush, continue
     the inferior and collect another signal.  But if there's already
     a saved status that we don't want to flush, we can't resume the
     inferior - if it stopped for some other reason we wouldn't have
     anywhere to save the new status.  In that case, we must leave the
     signal unflushed (and possibly generate an extra SIGINT stop).
     That's much less bad than losing a signal.  */
  while (lp->status == 0
	 && linux_nat_has_pending (GET_LWP (lp->ptid), &pending, flush_mask))
    {
      int ret;
      
      errno = 0;
      ret = ptrace (PTRACE_CONT, GET_LWP (lp->ptid), 0, 0);
      if (debug_linux_nat)
	fprintf_unfiltered (gdb_stderr,
			    "FC: Sent PTRACE_CONT, ret %d %d\n", ret, errno);

      lp->stopped = 0;
      stop_wait_callback (lp, flush_mask);
      if (debug_linux_nat)
	fprintf_unfiltered (gdb_stderr,
			    "FC: Wait finished; saved status is %d\n",
			    lp->status);
    }

  return 0;
}

/* Return non-zero if LP has a wait status pending.  */

static int
status_callback (struct lwp_info *lp, void *data)
{
  /* Only report a pending wait status if we pretend that this has
     indeed been resumed.  */
  return (lp->status != 0 && lp->resumed);
}

/* Return non-zero if LP isn't stopped.  */

static int
running_callback (struct lwp_info *lp, void *data)
{
  return (lp->stopped == 0 || (lp->status != 0 && lp->resumed));
}

/* Count the LWP's that have had events.  */

static int
count_events_callback (struct lwp_info *lp, void *data)
{
  int *count = data;

  gdb_assert (count != NULL);

  /* Count only LWPs that have a SIGTRAP event pending.  */
  if (lp->status != 0
      && WIFSTOPPED (lp->status) && WSTOPSIG (lp->status) == SIGTRAP)
    (*count)++;

  return 0;
}

/* Select the LWP (if any) that is currently being single-stepped.  */

static int
select_singlestep_lwp_callback (struct lwp_info *lp, void *data)
{
  if (lp->step && lp->status != 0)
    return 1;
  else
    return 0;
}

/* Select the Nth LWP that has had a SIGTRAP event.  */

static int
select_event_lwp_callback (struct lwp_info *lp, void *data)
{
  int *selector = data;

  gdb_assert (selector != NULL);

  /* Select only LWPs that have a SIGTRAP event pending. */
  if (lp->status != 0
      && WIFSTOPPED (lp->status) && WSTOPSIG (lp->status) == SIGTRAP)
    if ((*selector)-- == 0)
      return 1;

  return 0;
}

static int
cancel_breakpoint (struct lwp_info *lp)
{
  /* Arrange for a breakpoint to be hit again later.  We don't keep
     the SIGTRAP status and don't forward the SIGTRAP signal to the
     LWP.  We will handle the current event, eventually we will resume
     this LWP, and this breakpoint will trap again.

     If we do not do this, then we run the risk that the user will
     delete or disable the breakpoint, but the LWP will have already
     tripped on it.  */

  struct regcache *regcache = get_thread_regcache (lp->ptid);
  struct gdbarch *gdbarch = get_regcache_arch (regcache);
  CORE_ADDR pc;

  pc = regcache_read_pc (regcache) - gdbarch_decr_pc_after_break (gdbarch);
  if (breakpoint_inserted_here_p (pc))
    {
      if (debug_linux_nat)
	fprintf_unfiltered (gdb_stdlog,
			    "CB: Push back breakpoint for %s\n",
			    target_pid_to_str (lp->ptid));

      /* Back up the PC if necessary.  */
      if (gdbarch_decr_pc_after_break (gdbarch))
	regcache_write_pc (regcache, pc);

      return 1;
    }
  return 0;
}

static int
cancel_breakpoints_callback (struct lwp_info *lp, void *data)
{
  struct lwp_info *event_lp = data;

  /* Leave the LWP that has been elected to receive a SIGTRAP alone.  */
  if (lp == event_lp)
    return 0;

  /* If a LWP other than the LWP that we're reporting an event for has
     hit a GDB breakpoint (as opposed to some random trap signal),
     then just arrange for it to hit it again later.  We don't keep
     the SIGTRAP status and don't forward the SIGTRAP signal to the
     LWP.  We will handle the current event, eventually we will resume
     all LWPs, and this one will get its breakpoint trap again.

     If we do not do this, then we run the risk that the user will
     delete or disable the breakpoint, but the LWP will have already
     tripped on it.  */

  if (lp->status != 0
      && WIFSTOPPED (lp->status) && WSTOPSIG (lp->status) == SIGTRAP
      && cancel_breakpoint (lp))
    /* Throw away the SIGTRAP.  */
    lp->status = 0;

  return 0;
}

/* Select one LWP out of those that have events pending.  */

static void
select_event_lwp (struct lwp_info **orig_lp, int *status)
{
  int num_events = 0;
  int random_selector;
  struct lwp_info *event_lp;

  /* Record the wait status for the original LWP.  */
  (*orig_lp)->status = *status;

  /* Give preference to any LWP that is being single-stepped.  */
  event_lp = iterate_over_lwps (select_singlestep_lwp_callback, NULL);
  if (event_lp != NULL)
    {
      if (debug_linux_nat)
	fprintf_unfiltered (gdb_stdlog,
			    "SEL: Select single-step %s\n",
			    target_pid_to_str (event_lp->ptid));
    }
  else
    {
      /* No single-stepping LWP.  Select one at random, out of those
         which have had SIGTRAP events.  */

      /* First see how many SIGTRAP events we have.  */
      iterate_over_lwps (count_events_callback, &num_events);

      /* Now randomly pick a LWP out of those that have had a SIGTRAP.  */
      random_selector = (int)
	((num_events * (double) rand ()) / (RAND_MAX + 1.0));

      if (debug_linux_nat && num_events > 1)
	fprintf_unfiltered (gdb_stdlog,
			    "SEL: Found %d SIGTRAP events, selecting #%d\n",
			    num_events, random_selector);

      event_lp = iterate_over_lwps (select_event_lwp_callback,
				    &random_selector);
    }

  if (event_lp != NULL)
    {
      /* Switch the event LWP.  */
      *orig_lp = event_lp;
      *status = event_lp->status;
    }

  /* Flush the wait status for the event LWP.  */
  (*orig_lp)->status = 0;
}

/* Return non-zero if LP has been resumed.  */

static int
resumed_callback (struct lwp_info *lp, void *data)
{
  return lp->resumed;
}

/* Stop an active thread, verify it still exists, then resume it.  */

static int
stop_and_resume_callback (struct lwp_info *lp, void *data)
{
  struct lwp_info *ptr;

  if (!lp->stopped && !lp->signalled)
    {
      stop_callback (lp, NULL);
      stop_wait_callback (lp, NULL);
      /* Resume if the lwp still exists.  */
      for (ptr = lwp_list; ptr; ptr = ptr->next)
	if (lp == ptr)
	  {
	    resume_callback (lp, NULL);
	    resume_set_callback (lp, NULL);
	  }
    }
  return 0;
}

/* Check if we should go on and pass this event to common code.
   Return the affected lwp if we are, or NULL otherwise.  */
static struct lwp_info *
linux_nat_filter_event (int lwpid, int status, int options)
{
  struct lwp_info *lp;

  lp = find_lwp_pid (pid_to_ptid (lwpid));

  /* Check for stop events reported by a process we didn't already
     know about - anything not already in our LWP list.

     If we're expecting to receive stopped processes after
     fork, vfork, and clone events, then we'll just add the
     new one to our list and go back to waiting for the event
     to be reported - the stopped process might be returned
     from waitpid before or after the event is.  */
  if (WIFSTOPPED (status) && !lp)
    {
      linux_record_stopped_pid (lwpid, status);
      return NULL;
    }

  /* Make sure we don't report an event for the exit of an LWP not in
     our list, i.e.  not part of the current process.  This can happen
     if we detach from a program we original forked and then it
     exits.  */
  if (!WIFSTOPPED (status) && !lp)
    return NULL;

  /* NOTE drow/2003-06-17: This code seems to be meant for debugging
     CLONE_PTRACE processes which do not use the thread library -
     otherwise we wouldn't find the new LWP this way.  That doesn't
     currently work, and the following code is currently unreachable
     due to the two blocks above.  If it's fixed some day, this code
     should be broken out into a function so that we can also pick up
     LWPs from the new interface.  */
  if (!lp)
    {
      lp = add_lwp (BUILD_LWP (lwpid, GET_PID (inferior_ptid)));
      if (options & __WCLONE)
	lp->cloned = 1;

      gdb_assert (WIFSTOPPED (status)
		  && WSTOPSIG (status) == SIGSTOP);
      lp->signalled = 1;

      if (!in_thread_list (inferior_ptid))
	{
	  inferior_ptid = BUILD_LWP (GET_PID (inferior_ptid),
				     GET_PID (inferior_ptid));
	  add_thread (inferior_ptid);
	}

      add_thread (lp->ptid);
    }

  /* Save the trap's siginfo in case we need it later.  */
  if (WIFSTOPPED (status) && WSTOPSIG (status) == SIGTRAP)
    save_siginfo (lp);

  /* Handle GNU/Linux's extended waitstatus for trace events.  */
  if (WIFSTOPPED (status) && WSTOPSIG (status) == SIGTRAP && status >> 16 != 0)
    {
      if (debug_linux_nat)
	fprintf_unfiltered (gdb_stdlog,
			    "LLW: Handling extended status 0x%06x\n",
			    status);
      if (linux_handle_extended_wait (lp, status, 0))
	return NULL;
    }

  /* Check if the thread has exited.  */
  if ((WIFEXITED (status) || WIFSIGNALED (status)) && num_lwps > 1)
    {
      /* If this is the main thread, we must stop all threads and
	 verify if they are still alive.  This is because in the nptl
	 thread model, there is no signal issued for exiting LWPs
	 other than the main thread.  We only get the main thread exit
	 signal once all child threads have already exited.  If we
	 stop all the threads and use the stop_wait_callback to check
	 if they have exited we can determine whether this signal
	 should be ignored or whether it means the end of the debugged
	 application, regardless of which threading model is being
	 used.  */
      if (GET_PID (lp->ptid) == GET_LWP (lp->ptid))
	{
	  lp->stopped = 1;
	  iterate_over_lwps (stop_and_resume_callback, NULL);
	}

      if (debug_linux_nat)
	fprintf_unfiltered (gdb_stdlog,
			    "LLW: %s exited.\n",
			    target_pid_to_str (lp->ptid));

      exit_lwp (lp);

      /* If there is at least one more LWP, then the exit signal was
	 not the end of the debugged application and should be
	 ignored.  */
      if (num_lwps > 0)
	{
	  /* Make sure there is at least one thread running.  */
	  gdb_assert (iterate_over_lwps (running_callback, NULL));

	  /* Discard the event.  */
	  return NULL;
	}
    }

  /* Check if the current LWP has previously exited.  In the nptl
     thread model, LWPs other than the main thread do not issue
     signals when they exit so we must check whenever the thread has
     stopped.  A similar check is made in stop_wait_callback().  */
  if (num_lwps > 1 && !linux_nat_thread_alive (lp->ptid))
    {
      if (debug_linux_nat)
	fprintf_unfiltered (gdb_stdlog,
			    "LLW: %s exited.\n",
			    target_pid_to_str (lp->ptid));

      exit_lwp (lp);

      /* Make sure there is at least one thread running.  */
      gdb_assert (iterate_over_lwps (running_callback, NULL));

      /* Discard the event.  */
      return NULL;
    }

  /* Make sure we don't report a SIGSTOP that we sent ourselves in
     an attempt to stop an LWP.  */
  if (lp->signalled
      && WIFSTOPPED (status) && WSTOPSIG (status) == SIGSTOP)
    {
      if (debug_linux_nat)
	fprintf_unfiltered (gdb_stdlog,
			    "LLW: Delayed SIGSTOP caught for %s.\n",
			    target_pid_to_str (lp->ptid));

      /* This is a delayed SIGSTOP.  */
      lp->signalled = 0;

      registers_changed ();

      linux_ops->to_resume (pid_to_ptid (GET_LWP (lp->ptid)),
			    lp->step, TARGET_SIGNAL_0);
      if (debug_linux_nat)
	fprintf_unfiltered (gdb_stdlog,
			    "LLW: %s %s, 0, 0 (discard SIGSTOP)\n",
			    lp->step ?
			    "PTRACE_SINGLESTEP" : "PTRACE_CONT",
			    target_pid_to_str (lp->ptid));

      lp->stopped = 0;
      gdb_assert (lp->resumed);

      /* Discard the event.  */
      return NULL;
    }

  /* An interesting event.  */
  gdb_assert (lp);
  return lp;
}

/* Get the events stored in the pipe into the local queue, so they are
   accessible to queued_waitpid.  We need to do this, since it is not
   always the case that the event at the head of the pipe is the event
   we want.  */

static void
pipe_to_local_event_queue (void)
{
  if (debug_linux_nat_async)
    fprintf_unfiltered (gdb_stdlog,
			"PTLEQ: linux_nat_num_queued_events(%d)\n",
			linux_nat_num_queued_events);
  while (linux_nat_num_queued_events)
    {
      int lwpid, status, options;
      lwpid = linux_nat_event_pipe_pop (&status, &options);
      gdb_assert (lwpid > 0);
      push_waitpid (lwpid, status, options);
    }
}

/* Get the unprocessed events stored in the local queue back into the
   pipe, so the event loop realizes there's something else to
   process.  */

static void
local_event_queue_to_pipe (void)
{
  struct waitpid_result *w = waitpid_queue;
  while (w)
    {
      struct waitpid_result *next = w->next;
      linux_nat_event_pipe_push (w->pid,
				 w->status,
				 w->options);
      xfree (w);
      w = next;
    }
  waitpid_queue = NULL;

  if (debug_linux_nat_async)
    fprintf_unfiltered (gdb_stdlog,
			"LEQTP: linux_nat_num_queued_events(%d)\n",
			linux_nat_num_queued_events);
}

static ptid_t
linux_nat_wait (ptid_t ptid, struct target_waitstatus *ourstatus)
{
  struct lwp_info *lp = NULL;
  int options = 0;
  int status = 0;
  pid_t pid = PIDGET (ptid);
  sigset_t flush_mask;

  if (debug_linux_nat_async)
    fprintf_unfiltered (gdb_stdlog, "LLW: enter\n");

  /* The first time we get here after starting a new inferior, we may
     not have added it to the LWP list yet - this is the earliest
     moment at which we know its PID.  */
  if (num_lwps == 0)
    {
      gdb_assert (!is_lwp (inferior_ptid));

      inferior_ptid = BUILD_LWP (GET_PID (inferior_ptid),
				 GET_PID (inferior_ptid));
      lp = add_lwp (inferior_ptid);
      lp->resumed = 1;
      /* Add the main thread to GDB's thread list.  */
      add_thread_silent (lp->ptid);
    }

  sigemptyset (&flush_mask);

  /* Block events while we're here.  */
  linux_nat_async_events (sigchld_sync);

retry:

  /* Make sure there is at least one LWP that has been resumed.  */
  gdb_assert (iterate_over_lwps (resumed_callback, NULL));

  /* First check if there is a LWP with a wait status pending.  */
  if (pid == -1)
    {
      /* Any LWP that's been resumed will do.  */
      lp = iterate_over_lwps (status_callback, NULL);
      if (lp)
	{
	  if (target_can_async_p ())
	    internal_error (__FILE__, __LINE__,
			    "Found an LWP with a pending status in async mode.");

	  status = lp->status;
	  lp->status = 0;

	  if (debug_linux_nat && status)
	    fprintf_unfiltered (gdb_stdlog,
				"LLW: Using pending wait status %s for %s.\n",
				status_to_str (status),
				target_pid_to_str (lp->ptid));
	}

      /* But if we don't find one, we'll have to wait, and check both
         cloned and uncloned processes.  We start with the cloned
         processes.  */
      options = __WCLONE | WNOHANG;
    }
  else if (is_lwp (ptid))
    {
      if (debug_linux_nat)
	fprintf_unfiltered (gdb_stdlog,
			    "LLW: Waiting for specific LWP %s.\n",
			    target_pid_to_str (ptid));

      /* We have a specific LWP to check.  */
      lp = find_lwp_pid (ptid);
      gdb_assert (lp);
      status = lp->status;
      lp->status = 0;

      if (debug_linux_nat && status)
	fprintf_unfiltered (gdb_stdlog,
			    "LLW: Using pending wait status %s for %s.\n",
			    status_to_str (status),
			    target_pid_to_str (lp->ptid));

      /* If we have to wait, take into account whether PID is a cloned
         process or not.  And we have to convert it to something that
         the layer beneath us can understand.  */
      options = lp->cloned ? __WCLONE : 0;
      pid = GET_LWP (ptid);
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
      registers_changed ();
      linux_ops->to_resume (pid_to_ptid (GET_LWP (lp->ptid)),
			    lp->step, TARGET_SIGNAL_0);
      if (debug_linux_nat)
	fprintf_unfiltered (gdb_stdlog,
			    "LLW: %s %s, 0, 0 (expect SIGSTOP)\n",
			    lp->step ? "PTRACE_SINGLESTEP" : "PTRACE_CONT",
			    target_pid_to_str (lp->ptid));
      lp->stopped = 0;
      gdb_assert (lp->resumed);

      /* This should catch the pending SIGSTOP.  */
      stop_wait_callback (lp, NULL);
    }

  if (!target_can_async_p ())
    {
      /* Causes SIGINT to be passed on to the attached process.  */
      set_sigint_trap ();
      set_sigio_trap ();
    }

  while (status == 0)
    {
      pid_t lwpid;

      if (target_can_async_p ())
	/* In async mode, don't ever block.  Only look at the locally
	   queued events.  */
	lwpid = queued_waitpid (pid, &status, options);
      else
	lwpid = my_waitpid (pid, &status, options);

      if (lwpid > 0)
	{
	  gdb_assert (pid == -1 || lwpid == pid);

	  if (debug_linux_nat)
	    {
	      fprintf_unfiltered (gdb_stdlog,
				  "LLW: waitpid %ld received %s\n",
				  (long) lwpid, status_to_str (status));
	    }

	  lp = linux_nat_filter_event (lwpid, status, options);
	  if (!lp)
	    {
	      /* A discarded event.  */
	      status = 0;
	      continue;
	    }

	  break;
	}

      if (pid == -1)
	{
	  /* Alternate between checking cloned and uncloned processes.  */
	  options ^= __WCLONE;

	  /* And every time we have checked both:
	     In async mode, return to event loop;
	     In sync mode, suspend waiting for a SIGCHLD signal.  */
	  if (options & __WCLONE)
	    {
	      if (target_can_async_p ())
		{
		  /* No interesting event.  */
		  ourstatus->kind = TARGET_WAITKIND_IGNORE;

		  /* Get ready for the next event.  */
		  target_async (inferior_event_handler, 0);

		  if (debug_linux_nat_async)
		    fprintf_unfiltered (gdb_stdlog, "LLW: exit (ignore)\n");

		  return minus_one_ptid;
		}

	      sigsuspend (&suspend_mask);
	    }
	}

      /* We shouldn't end up here unless we want to try again.  */
      gdb_assert (status == 0);
    }

  if (!target_can_async_p ())
    {
      clear_sigio_trap ();
      clear_sigint_trap ();
    }

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

      /* If we get a signal while single-stepping, we may need special
	 care, e.g. to skip the signal handler.  Defer to common code.  */
      if (!lp->step
	  && signal_stop_state (signo) == 0
	  && signal_print_state (signo) == 0
	  && signal_pass_state (signo) == 1)
	{
	  /* FIMXE: kettenis/2001-06-06: Should we resume all threads
	     here?  It is not clear we should.  GDB may not expect
	     other threads to run.  On the other hand, not resuming
	     newly attached threads may cause an unwanted delay in
	     getting them running.  */
	  registers_changed ();
	  linux_ops->to_resume (pid_to_ptid (GET_LWP (lp->ptid)),
				lp->step, signo);
	  if (debug_linux_nat)
	    fprintf_unfiltered (gdb_stdlog,
				"LLW: %s %s, %s (preempt 'handle')\n",
				lp->step ?
				"PTRACE_SINGLESTEP" : "PTRACE_CONT",
				target_pid_to_str (lp->ptid),
				signo ? strsignal (signo) : "0");
	  lp->stopped = 0;
	  status = 0;
	  goto retry;
	}

      if (signo == TARGET_SIGNAL_INT && signal_pass_state (signo) == 0)
	{
	  /* If ^C/BREAK is typed at the tty/console, SIGINT gets
	     forwarded to the entire process group, that is, all LWP's
	     will receive it.  Since we only want to report it once,
	     we try to flush it from all LWPs except this one.  */
	  sigaddset (&flush_mask, SIGINT);
	}
    }

  /* This LWP is stopped now.  */
  lp->stopped = 1;

  if (debug_linux_nat)
    fprintf_unfiltered (gdb_stdlog, "LLW: Candidate event %s in %s.\n",
			status_to_str (status), target_pid_to_str (lp->ptid));

  /* Now stop all other LWP's ...  */
  iterate_over_lwps (stop_callback, NULL);

  /* ... and wait until all of them have reported back that they're no
     longer running.  */
  iterate_over_lwps (stop_wait_callback, &flush_mask);
  iterate_over_lwps (flush_callback, &flush_mask);

  /* If we're not waiting for a specific LWP, choose an event LWP from
     among those that have had events.  Giving equal priority to all
     LWPs that have had events helps prevent starvation.  */
  if (pid == -1)
    select_event_lwp (&lp, &status);

  /* Now that we've selected our final event LWP, cancel any
     breakpoints in other LWPs that have hit a GDB breakpoint.  See
     the comment in cancel_breakpoints_callback to find out why.  */
  iterate_over_lwps (cancel_breakpoints_callback, lp);

  if (WIFSTOPPED (status) && WSTOPSIG (status) == SIGTRAP)
    {
      if (debug_linux_nat)
	fprintf_unfiltered (gdb_stdlog,
			    "LLW: trap ptid is %s.\n",
			    target_pid_to_str (lp->ptid));
    }

  if (lp->waitstatus.kind != TARGET_WAITKIND_IGNORE)
    {
      *ourstatus = lp->waitstatus;
      lp->waitstatus.kind = TARGET_WAITKIND_IGNORE;
    }
  else
    store_waitstatus (ourstatus, status);

  /* Get ready for the next event.  */
  if (target_can_async_p ())
    target_async (inferior_event_handler, 0);

  if (debug_linux_nat_async)
    fprintf_unfiltered (gdb_stdlog, "LLW: exit\n");

  return lp->ptid;
}

static int
kill_callback (struct lwp_info *lp, void *data)
{
  errno = 0;
  ptrace (PTRACE_KILL, GET_LWP (lp->ptid), 0, 0);
  if (debug_linux_nat)
    fprintf_unfiltered (gdb_stdlog,
			"KC:  PTRACE_KILL %s, 0, 0 (%s)\n",
			target_pid_to_str (lp->ptid),
			errno ? safe_strerror (errno) : "OK");

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
  if (lp->cloned)
    {
      do
	{
	  pid = my_waitpid (GET_LWP (lp->ptid), NULL, __WCLONE);
	  if (pid != (pid_t) -1)
	    {
	      if (debug_linux_nat)
		fprintf_unfiltered (gdb_stdlog,
				    "KWC: wait %s received unknown.\n",
				    target_pid_to_str (lp->ptid));
	      /* The Linux kernel sometimes fails to kill a thread
		 completely after PTRACE_KILL; that goes from the stop
		 point in do_fork out to the one in
		 get_signal_to_deliever and waits again.  So kill it
		 again.  */
	      kill_callback (lp, NULL);
	    }
	}
      while (pid == GET_LWP (lp->ptid));

      gdb_assert (pid == -1 && errno == ECHILD);
    }

  do
    {
      pid = my_waitpid (GET_LWP (lp->ptid), NULL, 0);
      if (pid != (pid_t) -1)
	{
	  if (debug_linux_nat)
	    fprintf_unfiltered (gdb_stdlog,
				"KWC: wait %s received unk.\n",
				target_pid_to_str (lp->ptid));
	  /* See the call to kill_callback above.  */
	  kill_callback (lp, NULL);
	}
    }
  while (pid == GET_LWP (lp->ptid));

  gdb_assert (pid == -1 && errno == ECHILD);
  return 0;
}

static void
linux_nat_kill (void)
{
  struct target_waitstatus last;
  ptid_t last_ptid;
  int status;

  if (target_can_async_p ())
    target_async (NULL, 0);

  /* If we're stopped while forking and we haven't followed yet,
     kill the other task.  We need to do this first because the
     parent will be sleeping if this is a vfork.  */

  get_last_target_status (&last_ptid, &last);

  if (last.kind == TARGET_WAITKIND_FORKED
      || last.kind == TARGET_WAITKIND_VFORKED)
    {
      ptrace (PT_KILL, last.value.related_pid, 0, 0);
      wait (&status);
    }

  if (forks_exist_p ())
    {
      linux_fork_killall ();
      drain_queued_events (-1);
    }
  else
    {
      /* Kill all LWP's ...  */
      iterate_over_lwps (kill_callback, NULL);

      /* ... and wait until we've flushed all events.  */
      iterate_over_lwps (kill_wait_callback, NULL);
    }

  target_mourn_inferior ();
}

static void
linux_nat_mourn_inferior (void)
{
  /* Destroy LWP info; it's no longer valid.  */
  init_lwp_list ();

  if (! forks_exist_p ())
    {
      /* Normal case, no other forks available.  */
      if (target_can_async_p ())
	linux_nat_async (NULL, 0);
      linux_ops->to_mourn_inferior ();
    }
  else
    /* Multi-fork case.  The current inferior_ptid has exited, but
       there are other viable forks to debug.  Delete the exiting
       one and context-switch to the first available.  */
    linux_fork_mourn_inferior ();
}

static LONGEST
linux_nat_xfer_partial (struct target_ops *ops, enum target_object object,
			const char *annex, gdb_byte *readbuf,
			const gdb_byte *writebuf,
			ULONGEST offset, LONGEST len)
{
  struct cleanup *old_chain = save_inferior_ptid ();
  LONGEST xfer;

  if (is_lwp (inferior_ptid))
    inferior_ptid = pid_to_ptid (GET_LWP (inferior_ptid));

  xfer = linux_ops->to_xfer_partial (ops, object, annex, readbuf, writebuf,
				     offset, len);

  do_cleanups (old_chain);
  return xfer;
}

static int
linux_nat_thread_alive (ptid_t ptid)
{
  gdb_assert (is_lwp (ptid));

  errno = 0;
  ptrace (PTRACE_PEEKUSER, GET_LWP (ptid), 0, 0);
  if (debug_linux_nat)
    fprintf_unfiltered (gdb_stdlog,
			"LLTA: PTRACE_PEEKUSER %s, 0, 0 (%s)\n",
			target_pid_to_str (ptid),
			errno ? safe_strerror (errno) : "OK");

  /* Not every Linux kernel implements PTRACE_PEEKUSER.  But we can
     handle that case gracefully since ptrace will first do a lookup
     for the process based upon the passed-in pid.  If that fails we
     will get either -ESRCH or -EPERM, otherwise the child exists and
     is alive.  */
  if (errno == ESRCH || errno == EPERM)
    return 0;

  return 1;
}

static char *
linux_nat_pid_to_str (ptid_t ptid)
{
  static char buf[64];

  if (is_lwp (ptid)
      && ((lwp_list && lwp_list->next)
	  || GET_PID (ptid) != GET_LWP (ptid)))
    {
      snprintf (buf, sizeof (buf), "LWP %ld", GET_LWP (ptid));
      return buf;
    }

  return normal_pid_to_str (ptid);
}

static void
sigchld_handler (int signo)
{
  if (linux_nat_async_enabled
      && linux_nat_async_events_state != sigchld_sync
      && signo == SIGCHLD)
    /* It is *always* a bug to hit this.  */
    internal_error (__FILE__, __LINE__,
		    "sigchld_handler called when async events are enabled");

  /* Do nothing.  The only reason for this handler is that it allows
     us to use sigsuspend in linux_nat_wait above to wait for the
     arrival of a SIGCHLD.  */
}

/* Accepts an integer PID; Returns a string representing a file that
   can be opened to get the symbols for the child process.  */

static char *
linux_child_pid_to_exec_file (int pid)
{
  char *name1, *name2;

  name1 = xmalloc (MAXPATHLEN);
  name2 = xmalloc (MAXPATHLEN);
  make_cleanup (xfree, name1);
  make_cleanup (xfree, name2);
  memset (name2, 0, MAXPATHLEN);

  sprintf (name1, "/proc/%d/exe", pid);
  if (readlink (name1, name2, MAXPATHLEN) > 0)
    return name2;
  else
    return name1;
}

/* Service function for corefiles and info proc.  */

static int
read_mapping (FILE *mapfile,
	      long long *addr,
	      long long *endaddr,
	      char *permissions,
	      long long *offset,
	      char *device, long long *inode, char *filename)
{
  int ret = fscanf (mapfile, "%llx-%llx %s %llx %s %llx",
		    addr, endaddr, permissions, offset, device, inode);

  filename[0] = '\0';
  if (ret > 0 && ret != EOF)
    {
      /* Eat everything up to EOL for the filename.  This will prevent
         weird filenames (such as one with embedded whitespace) from
         confusing this code.  It also makes this code more robust in
         respect to annotations the kernel may add after the filename.

         Note the filename is used for informational purposes
         only.  */
      ret += fscanf (mapfile, "%[^\n]\n", filename);
    }

  return (ret != 0 && ret != EOF);
}

/* Fills the "to_find_memory_regions" target vector.  Lists the memory
   regions in the inferior for a corefile.  */

static int
linux_nat_find_memory_regions (int (*func) (CORE_ADDR,
					    unsigned long,
					    int, int, int, void *), void *obfd)
{
  long long pid = PIDGET (inferior_ptid);
  char mapsfilename[MAXPATHLEN];
  FILE *mapsfile;
  long long addr, endaddr, size, offset, inode;
  char permissions[8], device[8], filename[MAXPATHLEN];
  int read, write, exec;
  int ret;

  /* Compose the filename for the /proc memory map, and open it.  */
  sprintf (mapsfilename, "/proc/%lld/maps", pid);
  if ((mapsfile = fopen (mapsfilename, "r")) == NULL)
    error (_("Could not open %s."), mapsfilename);

  if (info_verbose)
    fprintf_filtered (gdb_stdout,
		      "Reading memory regions from %s\n", mapsfilename);

  /* Now iterate until end-of-file.  */
  while (read_mapping (mapsfile, &addr, &endaddr, &permissions[0],
		       &offset, &device[0], &inode, &filename[0]))
    {
      size = endaddr - addr;

      /* Get the segment's permissions.  */
      read = (strchr (permissions, 'r') != 0);
      write = (strchr (permissions, 'w') != 0);
      exec = (strchr (permissions, 'x') != 0);

      if (info_verbose)
	{
	  fprintf_filtered (gdb_stdout,
			    "Save segment, %lld bytes at 0x%s (%c%c%c)",
			    size, paddr_nz (addr),
			    read ? 'r' : ' ',
			    write ? 'w' : ' ', exec ? 'x' : ' ');
	  if (filename[0])
	    fprintf_filtered (gdb_stdout, " for %s", filename);
	  fprintf_filtered (gdb_stdout, "\n");
	}

      /* Invoke the callback function to create the corefile
	 segment.  */
      func (addr, size, read, write, exec, obfd);
    }
  fclose (mapsfile);
  return 0;
}

/* Records the thread's register state for the corefile note
   section.  */

static char *
linux_nat_do_thread_registers (bfd *obfd, ptid_t ptid,
			       char *note_data, int *note_size)
{
  gdb_gregset_t gregs;
  gdb_fpregset_t fpregs;
  unsigned long lwp = ptid_get_lwp (ptid);
  struct regcache *regcache = get_thread_regcache (ptid);
  struct gdbarch *gdbarch = get_regcache_arch (regcache);
  const struct regset *regset;
  int core_regset_p;
  struct cleanup *old_chain;
  struct core_regset_section *sect_list;
  char *gdb_regset;

  old_chain = save_inferior_ptid ();
  inferior_ptid = ptid;
  target_fetch_registers (regcache, -1);
  do_cleanups (old_chain);

  core_regset_p = gdbarch_regset_from_core_section_p (gdbarch);
  sect_list = gdbarch_core_regset_sections (gdbarch);

  if (core_regset_p
      && (regset = gdbarch_regset_from_core_section (gdbarch, ".reg",
						     sizeof (gregs))) != NULL
      && regset->collect_regset != NULL)
    regset->collect_regset (regset, regcache, -1,
			    &gregs, sizeof (gregs));
  else
    fill_gregset (regcache, &gregs, -1);

  note_data = (char *) elfcore_write_prstatus (obfd,
					       note_data,
					       note_size,
					       lwp,
					       stop_signal, &gregs);

  /* The loop below uses the new struct core_regset_section, which stores
     the supported section names and sizes for the core file.  Note that
     note PRSTATUS needs to be treated specially.  But the other notes are
     structurally the same, so they can benefit from the new struct.  */
  if (core_regset_p && sect_list != NULL)
    while (sect_list->sect_name != NULL)
      {
	/* .reg was already handled above.  */
	if (strcmp (sect_list->sect_name, ".reg") == 0)
	  {
	    sect_list++;
	    continue;
	  }
	regset = gdbarch_regset_from_core_section (gdbarch,
						   sect_list->sect_name,
						   sect_list->size);
	gdb_assert (regset && regset->collect_regset);
	gdb_regset = xmalloc (sect_list->size);
	regset->collect_regset (regset, regcache, -1,
				gdb_regset, sect_list->size);
	note_data = (char *) elfcore_write_register_note (obfd,
							  note_data,
							  note_size,
							  sect_list->sect_name,
							  gdb_regset,
							  sect_list->size);
	xfree (gdb_regset);
	sect_list++;
      }

  /* For architectures that does not have the struct core_regset_section
     implemented, we use the old method.  When all the architectures have
     the new support, the code below should be deleted.  */
  else
    {
      if (core_regset_p
          && (regset = gdbarch_regset_from_core_section (gdbarch, ".reg2",
							 sizeof (fpregs))) != NULL
	  && regset->collect_regset != NULL)
	regset->collect_regset (regset, regcache, -1,
				&fpregs, sizeof (fpregs));
      else
	fill_fpregset (regcache, &fpregs, -1);

      note_data = (char *) elfcore_write_prfpreg (obfd,
						  note_data,
						  note_size,
						  &fpregs, sizeof (fpregs));
    }

  return note_data;
}

struct linux_nat_corefile_thread_data
{
  bfd *obfd;
  char *note_data;
  int *note_size;
  int num_notes;
};

/* Called by gdbthread.c once per thread.  Records the thread's
   register state for the corefile note section.  */

static int
linux_nat_corefile_thread_callback (struct lwp_info *ti, void *data)
{
  struct linux_nat_corefile_thread_data *args = data;

  args->note_data = linux_nat_do_thread_registers (args->obfd,
						   ti->ptid,
						   args->note_data,
						   args->note_size);
  args->num_notes++;

  return 0;
}

/* Records the register state for the corefile note section.  */

static char *
linux_nat_do_registers (bfd *obfd, ptid_t ptid,
			char *note_data, int *note_size)
{
  return linux_nat_do_thread_registers (obfd,
					ptid_build (ptid_get_pid (inferior_ptid),
						    ptid_get_pid (inferior_ptid),
						    0),
					note_data, note_size);
}

/* Fills the "to_make_corefile_note" target vector.  Builds the note
   section for a corefile, and returns it in a malloc buffer.  */

static char *
linux_nat_make_corefile_notes (bfd *obfd, int *note_size)
{
  struct linux_nat_corefile_thread_data thread_args;
  struct cleanup *old_chain;
  /* The variable size must be >= sizeof (prpsinfo_t.pr_fname).  */
  char fname[16] = { '\0' };
  /* The variable size must be >= sizeof (prpsinfo_t.pr_psargs).  */
  char psargs[80] = { '\0' };
  char *note_data = NULL;
  ptid_t current_ptid = inferior_ptid;
  gdb_byte *auxv;
  int auxv_len;

  if (get_exec_file (0))
    {
      strncpy (fname, strrchr (get_exec_file (0), '/') + 1, sizeof (fname));
      strncpy (psargs, get_exec_file (0), sizeof (psargs));
      if (get_inferior_args ())
	{
	  char *string_end;
	  char *psargs_end = psargs + sizeof (psargs);

	  /* linux_elfcore_write_prpsinfo () handles zero unterminated
	     strings fine.  */
	  string_end = memchr (psargs, 0, sizeof (psargs));
	  if (string_end != NULL)
	    {
	      *string_end++ = ' ';
	      strncpy (string_end, get_inferior_args (),
		       psargs_end - string_end);
	    }
	}
      note_data = (char *) elfcore_write_prpsinfo (obfd,
						   note_data,
						   note_size, fname, psargs);
    }

  /* Dump information for threads.  */
  thread_args.obfd = obfd;
  thread_args.note_data = note_data;
  thread_args.note_size = note_size;
  thread_args.num_notes = 0;
  iterate_over_lwps (linux_nat_corefile_thread_callback, &thread_args);
  if (thread_args.num_notes == 0)
    {
      /* iterate_over_threads didn't come up with any threads; just
         use inferior_ptid.  */
      note_data = linux_nat_do_registers (obfd, inferior_ptid,
					  note_data, note_size);
    }
  else
    {
      note_data = thread_args.note_data;
    }

  auxv_len = target_read_alloc (&current_target, TARGET_OBJECT_AUXV,
				NULL, &auxv);
  if (auxv_len > 0)
    {
      note_data = elfcore_write_note (obfd, note_data, note_size,
				      "CORE", NT_AUXV, auxv, auxv_len);
      xfree (auxv);
    }

  make_cleanup (xfree, note_data);
  return note_data;
}

/* Implement the "info proc" command.  */

static void
linux_nat_info_proc_cmd (char *args, int from_tty)
{
  long long pid = PIDGET (inferior_ptid);
  FILE *procfile;
  char **argv = NULL;
  char buffer[MAXPATHLEN];
  char fname1[MAXPATHLEN], fname2[MAXPATHLEN];
  int cmdline_f = 1;
  int cwd_f = 1;
  int exe_f = 1;
  int mappings_f = 0;
  int environ_f = 0;
  int status_f = 0;
  int stat_f = 0;
  int all = 0;
  struct stat dummy;

  if (args)
    {
      /* Break up 'args' into an argv array.  */
      if ((argv = buildargv (args)) == NULL)
	nomem (0);
      else
	make_cleanup_freeargv (argv);
    }
  while (argv != NULL && *argv != NULL)
    {
      if (isdigit (argv[0][0]))
	{
	  pid = strtoul (argv[0], NULL, 10);
	}
      else if (strncmp (argv[0], "mappings", strlen (argv[0])) == 0)
	{
	  mappings_f = 1;
	}
      else if (strcmp (argv[0], "status") == 0)
	{
	  status_f = 1;
	}
      else if (strcmp (argv[0], "stat") == 0)
	{
	  stat_f = 1;
	}
      else if (strcmp (argv[0], "cmd") == 0)
	{
	  cmdline_f = 1;
	}
      else if (strncmp (argv[0], "exe", strlen (argv[0])) == 0)
	{
	  exe_f = 1;
	}
      else if (strcmp (argv[0], "cwd") == 0)
	{
	  cwd_f = 1;
	}
      else if (strncmp (argv[0], "all", strlen (argv[0])) == 0)
	{
	  all = 1;
	}
      else
	{
	  /* [...] (future options here) */
	}
      argv++;
    }
  if (pid == 0)
    error (_("No current process: you must name one."));

  sprintf (fname1, "/proc/%lld", pid);
  if (stat (fname1, &dummy) != 0)
    error (_("No /proc directory: '%s'"), fname1);

  printf_filtered (_("process %lld\n"), pid);
  if (cmdline_f || all)
    {
      sprintf (fname1, "/proc/%lld/cmdline", pid);
      if ((procfile = fopen (fname1, "r")) != NULL)
	{
	  fgets (buffer, sizeof (buffer), procfile);
	  printf_filtered ("cmdline = '%s'\n", buffer);
	  fclose (procfile);
	}
      else
	warning (_("unable to open /proc file '%s'"), fname1);
    }
  if (cwd_f || all)
    {
      sprintf (fname1, "/proc/%lld/cwd", pid);
      memset (fname2, 0, sizeof (fname2));
      if (readlink (fname1, fname2, sizeof (fname2)) > 0)
	printf_filtered ("cwd = '%s'\n", fname2);
      else
	warning (_("unable to read link '%s'"), fname1);
    }
  if (exe_f || all)
    {
      sprintf (fname1, "/proc/%lld/exe", pid);
      memset (fname2, 0, sizeof (fname2));
      if (readlink (fname1, fname2, sizeof (fname2)) > 0)
	printf_filtered ("exe = '%s'\n", fname2);
      else
	warning (_("unable to read link '%s'"), fname1);
    }
  if (mappings_f || all)
    {
      sprintf (fname1, "/proc/%lld/maps", pid);
      if ((procfile = fopen (fname1, "r")) != NULL)
	{
	  long long addr, endaddr, size, offset, inode;
	  char permissions[8], device[8], filename[MAXPATHLEN];

	  printf_filtered (_("Mapped address spaces:\n\n"));
	  if (gdbarch_addr_bit (current_gdbarch) == 32)
	    {
	      printf_filtered ("\t%10s %10s %10s %10s %7s\n",
			   "Start Addr",
			   "  End Addr",
			   "      Size", "    Offset", "objfile");
            }
	  else
            {
	      printf_filtered ("  %18s %18s %10s %10s %7s\n",
			   "Start Addr",
			   "  End Addr",
			   "      Size", "    Offset", "objfile");
	    }

	  while (read_mapping (procfile, &addr, &endaddr, &permissions[0],
			       &offset, &device[0], &inode, &filename[0]))
	    {
	      size = endaddr - addr;

	      /* FIXME: carlton/2003-08-27: Maybe the printf_filtered
		 calls here (and possibly above) should be abstracted
		 out into their own functions?  Andrew suggests using
		 a generic local_address_string instead to print out
		 the addresses; that makes sense to me, too.  */

	      if (gdbarch_addr_bit (current_gdbarch) == 32)
	        {
	          printf_filtered ("\t%#10lx %#10lx %#10x %#10x %7s\n",
			       (unsigned long) addr,	/* FIXME: pr_addr */
			       (unsigned long) endaddr,
			       (int) size,
			       (unsigned int) offset,
			       filename[0] ? filename : "");
		}
	      else
	        {
	          printf_filtered ("  %#18lx %#18lx %#10x %#10x %7s\n",
			       (unsigned long) addr,	/* FIXME: pr_addr */
			       (unsigned long) endaddr,
			       (int) size,
			       (unsigned int) offset,
			       filename[0] ? filename : "");
	        }
	    }

	  fclose (procfile);
	}
      else
	warning (_("unable to open /proc file '%s'"), fname1);
    }
  if (status_f || all)
    {
      sprintf (fname1, "/proc/%lld/status", pid);
      if ((procfile = fopen (fname1, "r")) != NULL)
	{
	  while (fgets (buffer, sizeof (buffer), procfile) != NULL)
	    puts_filtered (buffer);
	  fclose (procfile);
	}
      else
	warning (_("unable to open /proc file '%s'"), fname1);
    }
  if (stat_f || all)
    {
      sprintf (fname1, "/proc/%lld/stat", pid);
      if ((procfile = fopen (fname1, "r")) != NULL)
	{
	  int itmp;
	  char ctmp;
	  long ltmp;

	  if (fscanf (procfile, "%d ", &itmp) > 0)
	    printf_filtered (_("Process: %d\n"), itmp);
	  if (fscanf (procfile, "(%[^)]) ", &buffer[0]) > 0)
	    printf_filtered (_("Exec file: %s\n"), buffer);
	  if (fscanf (procfile, "%c ", &ctmp) > 0)
	    printf_filtered (_("State: %c\n"), ctmp);
	  if (fscanf (procfile, "%d ", &itmp) > 0)
	    printf_filtered (_("Parent process: %d\n"), itmp);
	  if (fscanf (procfile, "%d ", &itmp) > 0)
	    printf_filtered (_("Process group: %d\n"), itmp);
	  if (fscanf (procfile, "%d ", &itmp) > 0)
	    printf_filtered (_("Session id: %d\n"), itmp);
	  if (fscanf (procfile, "%d ", &itmp) > 0)
	    printf_filtered (_("TTY: %d\n"), itmp);
	  if (fscanf (procfile, "%d ", &itmp) > 0)
	    printf_filtered (_("TTY owner process group: %d\n"), itmp);
	  if (fscanf (procfile, "%lu ", &ltmp) > 0)
	    printf_filtered (_("Flags: 0x%lx\n"), ltmp);
	  if (fscanf (procfile, "%lu ", &ltmp) > 0)
	    printf_filtered (_("Minor faults (no memory page): %lu\n"),
			     (unsigned long) ltmp);
	  if (fscanf (procfile, "%lu ", &ltmp) > 0)
	    printf_filtered (_("Minor faults, children: %lu\n"),
			     (unsigned long) ltmp);
	  if (fscanf (procfile, "%lu ", &ltmp) > 0)
	    printf_filtered (_("Major faults (memory page faults): %lu\n"),
			     (unsigned long) ltmp);
	  if (fscanf (procfile, "%lu ", &ltmp) > 0)
	    printf_filtered (_("Major faults, children: %lu\n"),
			     (unsigned long) ltmp);
	  if (fscanf (procfile, "%ld ", &ltmp) > 0)
	    printf_filtered (_("utime: %ld\n"), ltmp);
	  if (fscanf (procfile, "%ld ", &ltmp) > 0)
	    printf_filtered (_("stime: %ld\n"), ltmp);
	  if (fscanf (procfile, "%ld ", &ltmp) > 0)
	    printf_filtered (_("utime, children: %ld\n"), ltmp);
	  if (fscanf (procfile, "%ld ", &ltmp) > 0)
	    printf_filtered (_("stime, children: %ld\n"), ltmp);
	  if (fscanf (procfile, "%ld ", &ltmp) > 0)
	    printf_filtered (_("jiffies remaining in current time slice: %ld\n"),
			     ltmp);
	  if (fscanf (procfile, "%ld ", &ltmp) > 0)
	    printf_filtered (_("'nice' value: %ld\n"), ltmp);
	  if (fscanf (procfile, "%lu ", &ltmp) > 0)
	    printf_filtered (_("jiffies until next timeout: %lu\n"),
			     (unsigned long) ltmp);
	  if (fscanf (procfile, "%lu ", &ltmp) > 0)
	    printf_filtered (_("jiffies until next SIGALRM: %lu\n"),
			     (unsigned long) ltmp);
	  if (fscanf (procfile, "%ld ", &ltmp) > 0)
	    printf_filtered (_("start time (jiffies since system boot): %ld\n"),
			     ltmp);
	  if (fscanf (procfile, "%lu ", &ltmp) > 0)
	    printf_filtered (_("Virtual memory size: %lu\n"),
			     (unsigned long) ltmp);
	  if (fscanf (procfile, "%lu ", &ltmp) > 0)
	    printf_filtered (_("Resident set size: %lu\n"), (unsigned long) ltmp);
	  if (fscanf (procfile, "%lu ", &ltmp) > 0)
	    printf_filtered (_("rlim: %lu\n"), (unsigned long) ltmp);
	  if (fscanf (procfile, "%lu ", &ltmp) > 0)
	    printf_filtered (_("Start of text: 0x%lx\n"), ltmp);
	  if (fscanf (procfile, "%lu ", &ltmp) > 0)
	    printf_filtered (_("End of text: 0x%lx\n"), ltmp);
	  if (fscanf (procfile, "%lu ", &ltmp) > 0)
	    printf_filtered (_("Start of stack: 0x%lx\n"), ltmp);
#if 0				/* Don't know how architecture-dependent the rest is...
				   Anyway the signal bitmap info is available from "status".  */
	  if (fscanf (procfile, "%lu ", &ltmp) > 0)	/* FIXME arch? */
	    printf_filtered (_("Kernel stack pointer: 0x%lx\n"), ltmp);
	  if (fscanf (procfile, "%lu ", &ltmp) > 0)	/* FIXME arch? */
	    printf_filtered (_("Kernel instr pointer: 0x%lx\n"), ltmp);
	  if (fscanf (procfile, "%ld ", &ltmp) > 0)
	    printf_filtered (_("Pending signals bitmap: 0x%lx\n"), ltmp);
	  if (fscanf (procfile, "%ld ", &ltmp) > 0)
	    printf_filtered (_("Blocked signals bitmap: 0x%lx\n"), ltmp);
	  if (fscanf (procfile, "%ld ", &ltmp) > 0)
	    printf_filtered (_("Ignored signals bitmap: 0x%lx\n"), ltmp);
	  if (fscanf (procfile, "%ld ", &ltmp) > 0)
	    printf_filtered (_("Catched signals bitmap: 0x%lx\n"), ltmp);
	  if (fscanf (procfile, "%lu ", &ltmp) > 0)	/* FIXME arch? */
	    printf_filtered (_("wchan (system call): 0x%lx\n"), ltmp);
#endif
	  fclose (procfile);
	}
      else
	warning (_("unable to open /proc file '%s'"), fname1);
    }
}

/* Implement the to_xfer_partial interface for memory reads using the /proc
   filesystem.  Because we can use a single read() call for /proc, this
   can be much more efficient than banging away at PTRACE_PEEKTEXT,
   but it doesn't support writes.  */

static LONGEST
linux_proc_xfer_partial (struct target_ops *ops, enum target_object object,
			 const char *annex, gdb_byte *readbuf,
			 const gdb_byte *writebuf,
			 ULONGEST offset, LONGEST len)
{
  LONGEST ret;
  int fd;
  char filename[64];

  if (object != TARGET_OBJECT_MEMORY || !readbuf)
    return 0;

  /* Don't bother for one word.  */
  if (len < 3 * sizeof (long))
    return 0;

  /* We could keep this file open and cache it - possibly one per
     thread.  That requires some juggling, but is even faster.  */
  sprintf (filename, "/proc/%d/mem", PIDGET (inferior_ptid));
  fd = open (filename, O_RDONLY | O_LARGEFILE);
  if (fd == -1)
    return 0;

  /* If pread64 is available, use it.  It's faster if the kernel
     supports it (only one syscall), and it's 64-bit safe even on
     32-bit platforms (for instance, SPARC debugging a SPARC64
     application).  */
#ifdef HAVE_PREAD64
  if (pread64 (fd, readbuf, len, offset) != len)
#else
  if (lseek (fd, offset, SEEK_SET) == -1 || read (fd, readbuf, len) != len)
#endif
    ret = 0;
  else
    ret = len;

  close (fd);
  return ret;
}

/* Parse LINE as a signal set and add its set bits to SIGS.  */

static void
add_line_to_sigset (const char *line, sigset_t *sigs)
{
  int len = strlen (line) - 1;
  const char *p;
  int signum;

  if (line[len] != '\n')
    error (_("Could not parse signal set: %s"), line);

  p = line;
  signum = len * 4;
  while (len-- > 0)
    {
      int digit;

      if (*p >= '0' && *p <= '9')
	digit = *p - '0';
      else if (*p >= 'a' && *p <= 'f')
	digit = *p - 'a' + 10;
      else
	error (_("Could not parse signal set: %s"), line);

      signum -= 4;

      if (digit & 1)
	sigaddset (sigs, signum + 1);
      if (digit & 2)
	sigaddset (sigs, signum + 2);
      if (digit & 4)
	sigaddset (sigs, signum + 3);
      if (digit & 8)
	sigaddset (sigs, signum + 4);

      p++;
    }
}

/* Find process PID's pending signals from /proc/pid/status and set
   SIGS to match.  */

void
linux_proc_pending_signals (int pid, sigset_t *pending, sigset_t *blocked, sigset_t *ignored)
{
  FILE *procfile;
  char buffer[MAXPATHLEN], fname[MAXPATHLEN];
  int signum;

  sigemptyset (pending);
  sigemptyset (blocked);
  sigemptyset (ignored);
  sprintf (fname, "/proc/%d/status", pid);
  procfile = fopen (fname, "r");
  if (procfile == NULL)
    error (_("Could not open %s"), fname);

  while (fgets (buffer, MAXPATHLEN, procfile) != NULL)
    {
      /* Normal queued signals are on the SigPnd line in the status
	 file.  However, 2.6 kernels also have a "shared" pending
	 queue for delivering signals to a thread group, so check for
	 a ShdPnd line also.

	 Unfortunately some Red Hat kernels include the shared pending
	 queue but not the ShdPnd status field.  */

      if (strncmp (buffer, "SigPnd:\t", 8) == 0)
	add_line_to_sigset (buffer + 8, pending);
      else if (strncmp (buffer, "ShdPnd:\t", 8) == 0)
	add_line_to_sigset (buffer + 8, pending);
      else if (strncmp (buffer, "SigBlk:\t", 8) == 0)
	add_line_to_sigset (buffer + 8, blocked);
      else if (strncmp (buffer, "SigIgn:\t", 8) == 0)
	add_line_to_sigset (buffer + 8, ignored);
    }

  fclose (procfile);
}

static LONGEST
linux_xfer_partial (struct target_ops *ops, enum target_object object,
                    const char *annex, gdb_byte *readbuf,
		    const gdb_byte *writebuf, ULONGEST offset, LONGEST len)
{
  LONGEST xfer;

  if (object == TARGET_OBJECT_AUXV)
    return procfs_xfer_auxv (ops, object, annex, readbuf, writebuf,
			     offset, len);

  xfer = linux_proc_xfer_partial (ops, object, annex, readbuf, writebuf,
				  offset, len);
  if (xfer != 0)
    return xfer;

  return super_xfer_partial (ops, object, annex, readbuf, writebuf,
			     offset, len);
}

/* Create a prototype generic GNU/Linux target.  The client can override
   it with local methods.  */

static void
linux_target_install_ops (struct target_ops *t)
{
  t->to_insert_fork_catchpoint = linux_child_insert_fork_catchpoint;
  t->to_insert_vfork_catchpoint = linux_child_insert_vfork_catchpoint;
  t->to_insert_exec_catchpoint = linux_child_insert_exec_catchpoint;
  t->to_pid_to_exec_file = linux_child_pid_to_exec_file;
  t->to_post_startup_inferior = linux_child_post_startup_inferior;
  t->to_post_attach = linux_child_post_attach;
  t->to_follow_fork = linux_child_follow_fork;
  t->to_find_memory_regions = linux_nat_find_memory_regions;
  t->to_make_corefile_notes = linux_nat_make_corefile_notes;

  super_xfer_partial = t->to_xfer_partial;
  t->to_xfer_partial = linux_xfer_partial;
}

struct target_ops *
linux_target (void)
{
  struct target_ops *t;

  t = inf_ptrace_target ();
  linux_target_install_ops (t);

  return t;
}

struct target_ops *
linux_trad_target (CORE_ADDR (*register_u_offset)(struct gdbarch *, int, int))
{
  struct target_ops *t;

  t = inf_ptrace_trad_target (register_u_offset);
  linux_target_install_ops (t);

  return t;
}

/* Controls if async mode is permitted.  */
static int linux_async_permitted = 0;

/* The set command writes to this variable.  If the inferior is
   executing, linux_nat_async_permitted is *not* updated.  */
static int linux_async_permitted_1 = 0;

static void
set_maintenance_linux_async_permitted (char *args, int from_tty,
			       struct cmd_list_element *c)
{
  if (target_has_execution)
    {
      linux_async_permitted_1 = linux_async_permitted;
      error (_("Cannot change this setting while the inferior is running."));
    }

  linux_async_permitted = linux_async_permitted_1;
  linux_nat_set_async_mode (linux_async_permitted);
}

static void
show_maintenance_linux_async_permitted (struct ui_file *file, int from_tty,
			    struct cmd_list_element *c, const char *value)
{
  fprintf_filtered (file, _("\
Controlling the GNU/Linux inferior in asynchronous mode is %s.\n"),
		    value);
}

/* target_is_async_p implementation.  */

static int
linux_nat_is_async_p (void)
{
  /* NOTE: palves 2008-03-21: We're only async when the user requests
     it explicitly with the "maintenance set linux-async" command.
     Someday, linux will always be async.  */
  if (!linux_async_permitted)
    return 0;

  return 1;
}

/* target_can_async_p implementation.  */

static int
linux_nat_can_async_p (void)
{
  /* NOTE: palves 2008-03-21: We're only async when the user requests
     it explicitly with the "maintenance set linux-async" command.
     Someday, linux will always be async.  */
  if (!linux_async_permitted)
    return 0;

  /* See target.h/target_async_mask.  */
  return linux_nat_async_mask_value;
}

/* target_async_mask implementation.  */

static int
linux_nat_async_mask (int mask)
{
  int current_state;
  current_state = linux_nat_async_mask_value;

  if (current_state != mask)
    {
      if (mask == 0)
	{
	  linux_nat_async (NULL, 0);
	  linux_nat_async_mask_value = mask;
	}
      else
	{
	  linux_nat_async_mask_value = mask;
	  linux_nat_async (inferior_event_handler, 0);
	}
    }

  return current_state;
}

/* Pop an event from the event pipe.  */

static int
linux_nat_event_pipe_pop (int* ptr_status, int* ptr_options)
{
  struct waitpid_result event = {0};
  int ret;

  do
    {
      ret = read (linux_nat_event_pipe[0], &event, sizeof (event));
    }
  while (ret == -1 && errno == EINTR);

  gdb_assert (ret == sizeof (event));

  *ptr_status = event.status;
  *ptr_options = event.options;

  linux_nat_num_queued_events--;

  return event.pid;
}

/* Push an event into the event pipe.  */

static void
linux_nat_event_pipe_push (int pid, int status, int options)
{
  int ret;
  struct waitpid_result event = {0};
  event.pid = pid;
  event.status = status;
  event.options = options;

  do
    {
      ret = write (linux_nat_event_pipe[1], &event, sizeof (event));
      gdb_assert ((ret == -1 && errno == EINTR) || ret == sizeof (event));
    } while (ret == -1 && errno == EINTR);

  linux_nat_num_queued_events++;
}

static void
get_pending_events (void)
{
  int status, options, pid;

  if (!linux_nat_async_enabled
      || linux_nat_async_events_state != sigchld_async)
    internal_error (__FILE__, __LINE__,
		    "get_pending_events called with async masked");

  while (1)
    {
      status = 0;
      options = __WCLONE | WNOHANG;

      do
	{
	  pid = waitpid (-1, &status, options);
	}
      while (pid == -1 && errno == EINTR);

      if (pid <= 0)
	{
	  options = WNOHANG;
	  do
	    {
	      pid = waitpid (-1, &status, options);
	    }
	  while (pid == -1 && errno == EINTR);
	}

      if (pid <= 0)
	/* No more children reporting events.  */
	break;

      if (debug_linux_nat_async)
	fprintf_unfiltered (gdb_stdlog, "\
get_pending_events: pid(%d), status(%x), options (%x)\n",
			    pid, status, options);

      linux_nat_event_pipe_push (pid, status, options);
    }

  if (debug_linux_nat_async)
    fprintf_unfiltered (gdb_stdlog, "\
get_pending_events: linux_nat_num_queued_events(%d)\n",
			linux_nat_num_queued_events);
}

/* SIGCHLD handler for async mode.  */

static void
async_sigchld_handler (int signo)
{
  if (debug_linux_nat_async)
    fprintf_unfiltered (gdb_stdlog, "async_sigchld_handler\n");

  get_pending_events ();
}

/* Set SIGCHLD handling state to STATE.  Returns previous state.  */

static enum sigchld_state
linux_nat_async_events (enum sigchld_state state)
{
  enum sigchld_state current_state = linux_nat_async_events_state;

  if (debug_linux_nat_async)
    fprintf_unfiltered (gdb_stdlog,
			"LNAE: state(%d): linux_nat_async_events_state(%d), "
			"linux_nat_num_queued_events(%d)\n",
			state, linux_nat_async_events_state,
			linux_nat_num_queued_events);

  if (current_state != state)
    {
      sigset_t mask;
      sigemptyset (&mask);
      sigaddset (&mask, SIGCHLD);

      /* Always block before changing state.  */
      sigprocmask (SIG_BLOCK, &mask, NULL);

      /* Set new state.  */
      linux_nat_async_events_state = state;

      switch (state)
	{
	case sigchld_sync:
	  {
	    /* Block target events.  */
	    sigprocmask (SIG_BLOCK, &mask, NULL);
	    sigaction (SIGCHLD, &sync_sigchld_action, NULL);
	    /* Get events out of queue, and make them available to
	       queued_waitpid / my_waitpid.  */
	    pipe_to_local_event_queue ();
	  }
	  break;
	case sigchld_async:
	  {
	    /* Unblock target events for async mode.  */

	    sigprocmask (SIG_BLOCK, &mask, NULL);

	    /* Put events we already waited on, in the pipe first, so
	       events are FIFO.  */
	    local_event_queue_to_pipe ();
	    /* While in masked async, we may have not collected all
	       the pending events.  Get them out now.  */
	    get_pending_events ();

	    /* Let'em come.   */
	    sigaction (SIGCHLD, &async_sigchld_action, NULL);
	    sigprocmask (SIG_UNBLOCK, &mask, NULL);
	  }
	  break;
	case sigchld_default:
	  {
	    /* SIGCHLD default mode.  */
	    sigaction (SIGCHLD, &sigchld_default_action, NULL);

	    /* Get events out of queue, and make them available to
	       queued_waitpid / my_waitpid.  */
	    pipe_to_local_event_queue ();

	    /* Unblock SIGCHLD.  */
	    sigprocmask (SIG_UNBLOCK, &mask, NULL);
	  }
	  break;
	}
    }

  return current_state;
}

static int async_terminal_is_ours = 1;

/* target_terminal_inferior implementation.  */

static void
linux_nat_terminal_inferior (void)
{
  if (!target_is_async_p ())
    {
      /* Async mode is disabled.  */
      terminal_inferior ();
      return;
    }

  /* GDB should never give the terminal to the inferior, if the
     inferior is running in the background (run&, continue&, etc.).
     This check can be removed when the common code is fixed.  */
  if (!sync_execution)
    return;

  terminal_inferior ();

  if (!async_terminal_is_ours)
    return;

  delete_file_handler (input_fd);
  async_terminal_is_ours = 0;
  set_sigint_trap ();
}

/* target_terminal_ours implementation.  */

void
linux_nat_terminal_ours (void)
{
  if (!target_is_async_p ())
    {
      /* Async mode is disabled.  */
      terminal_ours ();
      return;
    }

  /* GDB should never give the terminal to the inferior if the
     inferior is running in the background (run&, continue&, etc.),
     but claiming it sure should.  */
  terminal_ours ();

  if (!sync_execution)
    return;

  if (async_terminal_is_ours)
    return;

  clear_sigint_trap ();
  add_file_handler (input_fd, stdin_event_handler, 0);
  async_terminal_is_ours = 1;
}

static void (*async_client_callback) (enum inferior_event_type event_type,
				      void *context);
static void *async_client_context;

static void
linux_nat_async_file_handler (int error, gdb_client_data client_data)
{
  async_client_callback (INF_REG_EVENT, async_client_context);
}

/* target_async implementation.  */

static void
linux_nat_async (void (*callback) (enum inferior_event_type event_type,
				   void *context), void *context)
{
  if (linux_nat_async_mask_value == 0 || !linux_nat_async_enabled)
    internal_error (__FILE__, __LINE__,
		    "Calling target_async when async is masked");

  if (callback != NULL)
    {
      async_client_callback = callback;
      async_client_context = context;
      add_file_handler (linux_nat_event_pipe[0],
			linux_nat_async_file_handler, NULL);

      linux_nat_async_events (sigchld_async);
    }
  else
    {
      async_client_callback = callback;
      async_client_context = context;

      linux_nat_async_events (sigchld_sync);
      delete_file_handler (linux_nat_event_pipe[0]);
    }
  return;
}

/* Enable/Disable async mode.  */

static void
linux_nat_set_async_mode (int on)
{
  if (linux_nat_async_enabled != on)
    {
      if (on)
	{
	  gdb_assert (waitpid_queue == NULL);
	  if (pipe (linux_nat_event_pipe) == -1)
	    internal_error (__FILE__, __LINE__,
			    "creating event pipe failed.");
	  fcntl (linux_nat_event_pipe[0], F_SETFL, O_NONBLOCK);
	  fcntl (linux_nat_event_pipe[1], F_SETFL, O_NONBLOCK);
	}
      else
	{
	  drain_queued_events (-1);
	  linux_nat_num_queued_events = 0;
	  close (linux_nat_event_pipe[0]);
	  close (linux_nat_event_pipe[1]);
	  linux_nat_event_pipe[0] = linux_nat_event_pipe[1] = -1;

	}
    }
  linux_nat_async_enabled = on;
}

void
linux_nat_add_target (struct target_ops *t)
{
  /* Save the provided single-threaded target.  We save this in a separate
     variable because another target we've inherited from (e.g. inf-ptrace)
     may have saved a pointer to T; we want to use it for the final
     process stratum target.  */
  linux_ops_saved = *t;
  linux_ops = &linux_ops_saved;

  /* Override some methods for multithreading.  */
  t->to_create_inferior = linux_nat_create_inferior;
  t->to_attach = linux_nat_attach;
  t->to_detach = linux_nat_detach;
  t->to_resume = linux_nat_resume;
  t->to_wait = linux_nat_wait;
  t->to_xfer_partial = linux_nat_xfer_partial;
  t->to_kill = linux_nat_kill;
  t->to_mourn_inferior = linux_nat_mourn_inferior;
  t->to_thread_alive = linux_nat_thread_alive;
  t->to_pid_to_str = linux_nat_pid_to_str;
  t->to_has_thread_control = tc_schedlock;

  t->to_can_async_p = linux_nat_can_async_p;
  t->to_is_async_p = linux_nat_is_async_p;
  t->to_async = linux_nat_async;
  t->to_async_mask = linux_nat_async_mask;
  t->to_terminal_inferior = linux_nat_terminal_inferior;
  t->to_terminal_ours = linux_nat_terminal_ours;

  /* We don't change the stratum; this target will sit at
     process_stratum and thread_db will set at thread_stratum.  This
     is a little strange, since this is a multi-threaded-capable
     target, but we want to be on the stack below thread_db, and we
     also want to be used for single-threaded processes.  */

  add_target (t);

  /* TODO: Eliminate this and have libthread_db use
     find_target_beneath.  */
  thread_db_init (t);
}

/* Register a method to call whenever a new thread is attached.  */
void
linux_nat_set_new_thread (struct target_ops *t, void (*new_thread) (ptid_t))
{
  /* Save the pointer.  We only support a single registered instance
     of the GNU/Linux native target, so we do not need to map this to
     T.  */
  linux_nat_new_thread = new_thread;
}

/* Return the saved siginfo associated with PTID.  */
struct siginfo *
linux_nat_get_siginfo (ptid_t ptid)
{
  struct lwp_info *lp = find_lwp_pid (ptid);

  gdb_assert (lp != NULL);

  return &lp->siginfo;
}

void
_initialize_linux_nat (void)
{
  sigset_t mask;

  add_info ("proc", linux_nat_info_proc_cmd, _("\
Show /proc process information about any running process.\n\
Specify any process id, or use the program being debugged by default.\n\
Specify any of the following keywords for detailed info:\n\
  mappings -- list of mapped memory regions.\n\
  stat     -- list a bunch of random process info.\n\
  status   -- list a different bunch of random process info.\n\
  all      -- list all available /proc info."));

  add_setshow_zinteger_cmd ("lin-lwp", class_maintenance,
			    &debug_linux_nat, _("\
Set debugging of GNU/Linux lwp module."), _("\
Show debugging of GNU/Linux lwp module."), _("\
Enables printf debugging output."),
			    NULL,
			    show_debug_linux_nat,
			    &setdebuglist, &showdebuglist);

  add_setshow_zinteger_cmd ("lin-lwp-async", class_maintenance,
			    &debug_linux_nat_async, _("\
Set debugging of GNU/Linux async lwp module."), _("\
Show debugging of GNU/Linux async lwp module."), _("\
Enables printf debugging output."),
			    NULL,
			    show_debug_linux_nat_async,
			    &setdebuglist, &showdebuglist);

  add_setshow_boolean_cmd ("linux-async", class_maintenance,
			   &linux_async_permitted_1, _("\
Set whether gdb controls the GNU/Linux inferior in asynchronous mode."), _("\
Show whether gdb controls the GNU/Linux inferior in asynchronous mode."), _("\
Tells gdb whether to control the GNU/Linux inferior in asynchronous mode."),
			   set_maintenance_linux_async_permitted,
			   show_maintenance_linux_async_permitted,
			   &maintenance_set_cmdlist,
			   &maintenance_show_cmdlist);

  /* Get the default SIGCHLD action.  Used while forking an inferior
     (see linux_nat_create_inferior/linux_nat_async_events).  */
  sigaction (SIGCHLD, NULL, &sigchld_default_action);

  /* Block SIGCHLD by default.  Doing this early prevents it getting
     unblocked if an exception is thrown due to an error while the
     inferior is starting (sigsetjmp/siglongjmp).  */
  sigemptyset (&mask);
  sigaddset (&mask, SIGCHLD);
  sigprocmask (SIG_BLOCK, &mask, NULL);

  /* Save this mask as the default.  */
  sigprocmask (SIG_SETMASK, NULL, &normal_mask);

  /* The synchronous SIGCHLD handler.  */
  sync_sigchld_action.sa_handler = sigchld_handler;
  sigemptyset (&sync_sigchld_action.sa_mask);
  sync_sigchld_action.sa_flags = SA_RESTART;

  /* Make it the default.  */
  sigaction (SIGCHLD, &sync_sigchld_action, NULL);

  /* Make sure we don't block SIGCHLD during a sigsuspend.  */
  sigprocmask (SIG_SETMASK, NULL, &suspend_mask);
  sigdelset (&suspend_mask, SIGCHLD);

  /* SIGCHLD handler for async mode.  */
  async_sigchld_action.sa_handler = async_sigchld_handler;
  sigemptyset (&async_sigchld_action.sa_mask);
  async_sigchld_action.sa_flags = SA_RESTART;

  /* Install the default mode.  */
  linux_nat_set_async_mode (linux_async_permitted);
}


/* FIXME: kettenis/2000-08-26: The stuff on this page is specific to
   the GNU/Linux Threads library and therefore doesn't really belong
   here.  */

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

  if (target_read_memory (SYMBOL_VALUE_ADDRESS (ms), (gdb_byte *) &signo,
			  sizeof (signo)) != 0)
    return 0;

  return signo;
}

/* Return the set of signals used by the threads library in *SET.  */

void
lin_thread_get_thread_signals (sigset_t *set)
{
  struct sigaction action;
  int restart, cancel;
  sigset_t blocked_mask;

  sigemptyset (&blocked_mask);
  sigemptyset (set);

  restart = get_signo ("__pthread_sig_restart");
  cancel = get_signo ("__pthread_sig_cancel");

  /* LinuxThreads normally uses the first two RT signals, but in some legacy
     cases may use SIGUSR1/SIGUSR2.  NPTL always uses RT signals, but does
     not provide any way for the debugger to query the signal numbers -
     fortunately they don't change!  */

  if (restart == 0)
    restart = __SIGRTMIN;

  if (cancel == 0)
    cancel = __SIGRTMIN + 1;

  sigaddset (set, restart);
  sigaddset (set, cancel);

  /* The GNU/Linux Threads library makes terminating threads send a
     special "cancel" signal instead of SIGCHLD.  Make sure we catch
     those (to prevent them from terminating GDB itself, which is
     likely to be their default action) and treat them the same way as
     SIGCHLD.  */

  action.sa_handler = sigchld_handler;
  sigemptyset (&action.sa_mask);
  action.sa_flags = SA_RESTART;
  sigaction (cancel, &action, NULL);

  /* We block the "cancel" signal throughout this code ...  */
  sigaddset (&blocked_mask, cancel);
  sigprocmask (SIG_BLOCK, &blocked_mask, NULL);

  /* ... except during a sigsuspend.  */
  sigdelset (&suspend_mask, cancel);
}
