/* Low level interface to ptrace, for the remote server for GDB.
   Copyright (C) 1995, 1996, 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005,
   2006, 2007, 2008 Free Software Foundation, Inc.

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

#include "server.h"
#include "linux-low.h"

#include <sys/wait.h>
#include <stdio.h>
#include <sys/param.h>
#include <sys/ptrace.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/syscall.h>
#include <sched.h>

#ifndef PTRACE_GETSIGINFO
# define PTRACE_GETSIGINFO 0x4202
# define PTRACE_SETSIGINFO 0x4203
#endif

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
   in some contexts.  */
#ifndef __WALL
#define __WALL          0x40000000 /* Wait for any child.  */
#endif

#ifdef __UCLIBC__
#if !(defined(__UCLIBC_HAS_MMU__) || defined(__ARCH_HAS_MMU__))
#define HAS_NOMMU
#endif
#endif

/* ``all_processes'' is keyed by the "overall process ID", which
   GNU/Linux calls tgid, "thread group ID".

   ``all_threads'' is keyed by the LWP ID, which we use as the GDB
   protocol representation of the thread ID.

   ``all_lwps'' is keyed by the LWP ID - on Linux the LWP IDs are
   global (presently).  */

struct inferior_list all_lwps;

/* A list of all unknown lwps which receive stop signals.  Some other
   lwp will presumably claim each of these as forked children
   momentarily.  */

struct inferior_list stopped_pids;

/* FIXME this is a bit of a hack, and could be removed.  */
int stopping_threads;

/* FIXME make into a target method?  */
int using_threads = 1;

static int must_set_ptrace_flags;

/* This flag is true iff we've just created or attached to our first
   inferior but it has not stopped yet.  As soon as it does, we need
   to call the low target's arch_setup callback.  Doing this only on
   the first inferior avoids reinializing the architecture on every
   inferior, and avoids messing with the register caches of the
   already running inferiors.  NOTE: this assumes all inferiors under
   control of gdbserver have the same architecture.  */
static int new_inferior;

static void linux_resume_one_lwp (struct inferior_list_entry *entry,
				  int step, int signal, siginfo_t *info);
static void linux_resume (struct thread_resume *resume_info, size_t n);
static void stop_all_lwps (void);
static int linux_wait_for_event (ptid_t ptid, int *wstat, int options);
static int check_removed_breakpoint (struct lwp_info *event_child);
static void *add_lwp (ptid_t ptid);

struct pending_signals
{
  int signal;
  siginfo_t info;
  struct pending_signals *prev;
};

#define PTRACE_ARG3_TYPE long
#define PTRACE_XFER_TYPE long

#ifdef HAVE_LINUX_REGSETS
static char *disabled_regsets;
static int num_regsets;
#endif

#define pid_of(proc) ptid_get_pid ((proc)->head.id)
#define lwpid_of(proc) ptid_get_lwp ((proc)->head.id)

#define is_lwpid(ptid) (ptid_get_lwp (ptid) != 0)

/* FIXME: Delete eventually.  */
#define inferior_pid (pid_of (get_thread_lwp (current_inferior)))
#define inferior_lwpid (lwpid_of (get_thread_lwp (current_inferior)))

/* The read/write ends of the pipe registered as waitable file in the
   event loop.  */
static int linux_event_pipe[2] = { -1, -1 };

/* True if we're currently in async mode.  */
#define target_is_async_p() (linux_event_pipe[0] != -1)

static int my_waitpid (int pid, int *status, int flags);
static void send_sigstop (struct inferior_list_entry *entry);
static void wait_for_sigstop (struct inferior_list_entry *entry);
static int cancel_breakpoints_callback (struct inferior_list_entry *entry,
					void *data);

/* Add a process to the common process list, and set its private
   data.  */

static struct process_info *
linux_add_process (int pid, int attached)
{
  struct process_info *proc;

  proc = add_process (pid, attached);
  proc->private = calloc (1, sizeof (*proc->private));

  return proc;
}

/* Read the tgid of an lwp from `/proc/PID/status'.  */

static int
tgid_of_pid (int pid)
{
  FILE *status_file;
  char buf[100];
  int tgid = 0;

  snprintf (buf, sizeof (buf), "/proc/%d/status", pid);
  status_file = fopen (buf, "r");
  if (status_file != NULL)
    {
      while (fgets (buf, sizeof (buf), status_file))
	{
	  if (strncmp (buf, "Tgid:", 5) == 0)
	    {
	      tgid = strtol (buf + 5, NULL, 0);
	      break;
	    }
	}
      fclose (status_file);
    }

  return tgid;
}

static char *
linux_pid_to_exec_file (int pid)
{
  static char path[MAXPATHLEN];
  char procexe[MAXPATHLEN];

  sprintf (procexe, "/proc/%d/exe", pid);
  if (readlink (procexe, path, MAXPATHLEN) > 0)
    return path;
  else
    return NULL;
}

static int linux_supports_tracefork_flag;

static void
linux_enable_event_reporting (int pid)
{
  int options;

  options = PTRACE_O_TRACEFORK | PTRACE_O_TRACEVFORK
    | PTRACE_O_TRACEEXEC | PTRACE_O_TRACECLONE;

#if 0
  /* TODO: We need to model this in the remote protocol.  */
  if (linux_supports_tracevforkdone (pid))
    options |= PTRACE_O_TRACEVFORKDONE;
#endif

  /* Do not enable PTRACE_O_TRACEEXIT until GDB is more prepared to support
     read-only process state.  */
  ptrace (PTRACE_SETOPTIONS, pid, 0, options);
}

/* Handle a GNU/Linux extended wait response.  If we see a clone
   event, we need to add the new LWP to our list (and not report the
   trap to higher layers).  This function returns non-zero if the
   event should be ignored and we should wait again.  */

static int
handle_extended_wait (struct lwp_info *event_child, int wstat)
{
  int event = wstat >> 16;
  struct lwp_info *new_lwp;
  struct target_waitstatus *ourstatus = &event_child->waitstatus;

  if (event == PTRACE_EVENT_FORK
      || event == PTRACE_EVENT_VFORK
      || event == PTRACE_EVENT_CLONE)
    {
      ptid_t ptid;
      unsigned long new_pid;
      int ret, status = W_STOPCODE (SIGSTOP);

      ptrace (PTRACE_GETEVENTMSG, lwpid_of (event_child), 0, &new_pid);

      /* If we haven't already seen the new PID stop, wait for it now.  */
      if (! pull_pid_from_list (&stopped_pids, new_pid))
	{
	  /* The new child has a pending SIGSTOP.  We can't affect it until it
	     hits the SIGSTOP, but we're already attached.  */

	  ret = my_waitpid (new_pid, &status, __WALL);

	  if (ret == -1)
	    perror_with_name ("waiting for new child");
	  else if (ret != new_pid)
	    warning ("wait returned unexpected PID %d", ret);
	  else if (!WIFSTOPPED (status))
	    warning ("wait returned unexpected status 0x%x", status);
	}

      ourstatus->value.related_pid = ptid_build (new_pid, new_pid, 0);

      linux_enable_event_reporting (new_pid);

      if (event == PTRACE_EVENT_FORK
	  || event == PTRACE_EVENT_VFORK)
	{
	  int is_fork = (event == PTRACE_EVENT_FORK);

	  if (is_fork)
	    ourstatus->kind = TARGET_WAITKIND_FORKED;
	  else
	    ourstatus->kind = TARGET_WAITKIND_VFORKED;

	  if (debug_threads)
	    {
	      if (is_fork)
		fprintf (stderr, "PTRACE_EVENT_FORK pid=(%ld), newpid=(%ld)",
			 lwpid_of (event_child), new_pid);
	      else
		fprintf (stderr, "PTRACE_EVENT_VFORK pid=(%ld), newpid=(%ld)",
			 lwpid_of (event_child), new_pid);
	    }

	  linux_add_process (new_pid, 1);
	  ptid = ptid_build (new_pid, new_pid, 0);
	  new_lwp = (struct lwp_info *) add_lwp (ptid);
	  new_lwp->stopped = 1;
	  add_thread (ptid, new_lwp);

	  event_child->stopped = 1;
	  return 0;
	}
      else
	{
	  ptid = ptid_build (pid_of (event_child), new_pid, 0);
	  new_lwp = (struct lwp_info *) add_lwp (ptid);
	  add_thread (ptid, new_lwp);
#if 0
	  new_thread_notify (thread_id_to_gdb_id (new_lwp->lwpid));
#endif

	  /* Normally we will get the pending SIGSTOP.  But in some
	     cases we might get another signal delivered to the group
	     first.  If we do, be sure not to lose it.  */
	  if (WSTOPSIG (status) == SIGSTOP)
	    {
	      if (stopping_threads)
		new_lwp->stopped = 1;
	      else
		ptrace (PTRACE_CONT, new_pid, 0, 0);
	    }
	  else
	    {
	      new_lwp->stop_expected = 1;
	      if (stopping_threads)
		{
		  new_lwp->stopped = 1;
		  new_lwp->status_pending_p = 1;
		  new_lwp->status_pending = status;
		}
	      else
		/* Pass the signal on.  This is what GDB does - except
		   shouldn't we really report it instead?  */
		ptrace (PTRACE_CONT, new_pid, 0, WSTOPSIG (status));
	    }

	  /* Always resume the current thread.  If we are stopping
	     threads, it will have a pending SIGSTOP; we may as well
	     collect it now.  */
	  linux_resume_one_lwp (&event_child->head,
				event_child->stepping, 0, NULL);
	  return 1;
	}
    }
  else if (event == PTRACE_EVENT_EXEC)
    {
      fprintf (stderr, "got PTRACE_EVENT_EXEC\n");

      ourstatus->kind = TARGET_WAITKIND_EXECD;
      ourstatus->value.execd_pathname
	= strdup (linux_pid_to_exec_file (lwpid_of (event_child)));
      return 0;
    }

  error ("unknown ptrace event %d", event);
}

/* This function should only be called if the lwp got a SIGTRAP.
   The SIGTRAP could mean several things.

   On i386, where decr_pc_after_break is non-zero:
   If we were single-stepping this lwp using PTRACE_SINGLESTEP,
   we will get only the one SIGTRAP (even if the instruction we
   stepped over was a breakpoint).  The value of $eip will be the
   next instruction.
   If we continue the lwp using PTRACE_CONT, we will get a
   SIGTRAP when we hit a breakpoint.  The value of $eip will be
   the instruction after the breakpoint (i.e. needs to be
   decremented).  If we report the SIGTRAP to GDB, we must also
   report the undecremented PC.  If we cancel the SIGTRAP, we
   must resume at the decremented PC.

   (Presumably, not yet tested) On a non-decr_pc_after_break machine
   with hardware or kernel single-step:
   If we single-step over a breakpoint instruction, our PC will
   point at the following instruction.  If we continue and hit a
   breakpoint instruction, our PC will point at the breakpoint
   instruction.  */

static CORE_ADDR
get_stop_pc (void)
{
  CORE_ADDR stop_pc = (*the_low_target.get_pc) ();

  if (get_thread_lwp (current_inferior)->stepping)
    return stop_pc;
  else
    return stop_pc - the_low_target.decr_pc_after_break;
}

static void *
add_lwp (ptid_t ptid)
{
  struct lwp_info *lwp;

  lwp = (struct lwp_info *) malloc (sizeof (*lwp));
  memset (lwp, 0, sizeof (*lwp));

  lwp->head.id = ptid;

  lwp->waitstatus.kind = TARGET_WAITKIND_IGNORE;

  add_inferior_to_list (&all_lwps, &lwp->head);

  return lwp;
}

/* Start an inferior process and returns its pid.
   ALLARGS is a vector of program-name and args. */

static int
linux_create_inferior (char *program, char **allargs)
{
  void *new_lwp;
  int pid;
  ptid_t ptid;

#if defined(__UCLIBC__) && defined(HAS_NOMMU)
  pid = vfork ();
#else
  pid = fork ();
#endif
  if (pid < 0)
    perror_with_name ("fork");

  if (pid == 0)
    {
      ptrace (PTRACE_TRACEME, 0, 0, 0);

      signal (__SIGRTMIN + 1, SIG_DFL);

      setpgid (0, 0);

      execv (program, allargs);
      if (errno == ENOENT)
	execvp (program, allargs);

      fprintf (stderr, "Cannot exec %s: %s.\n", program,
	       strerror (errno));
      fflush (stderr);
      _exit (0177);
    }

  /* If this the first process?  If so, then set the arch.  */
  if (all_processes.head == NULL)
    new_inferior = 1;

  linux_add_process (pid, 0);

  ptid = ptid_build (pid, pid, 0);
  new_lwp = add_lwp (ptid);
  add_thread (ptid, new_lwp);
  must_set_ptrace_flags = 1;

  return pid;
}

/* Attach to an inferior process.  */

void
linux_attach_lwp (unsigned long pid, int initial)
{
  ptid_t ptid;
  struct lwp_info *new_lwp;

  if (ptrace (PTRACE_ATTACH, pid, 0, 0) != 0)
    {
      if (!initial)
	{
	  /* If we fail to attach to an LWP, just warn.  */
	  fprintf (stderr, "Cannot attach to lwp %ld: %s (%d)\n", pid,
		   strerror (errno), errno);
	  fflush (stderr);
	  return;
	}
      else
	{
	  new_inferior = 0;

	  /* If we fail to attach to a process, report an error.  */
	  error ("Cannot attach to lwp %ld: %s (%d)\n", pid,
		 strerror (errno), errno);
	}
    }

  linux_enable_event_reporting (pid);

  if (initial)
    /* NOTE/FIXME: This lwp might have not been the tgid.  */
    ptid = ptid_build (pid, pid, 0);
  else
    ptid = ptid_build (inferior_pid, pid, 0);

  new_lwp = (struct lwp_info *) add_lwp (ptid);
  add_thread (ptid, new_lwp);

#if 0
  new_thread_notify (thread_id_to_gdb_id (new_lwp->lwpid));
#endif

  /* The next time we wait for this LWP we'll see a SIGSTOP as PTRACE_ATTACH
     brings it to a halt.  We should ignore that SIGSTOP and resume the lwp
     (unless this is the first process, in which case the flag will be cleared
     in linux_attach).

     On the other hand, if we are currently trying to stop all threads, we
     should treat the new thread as if we had sent it a SIGSTOP.  This works
     because we are guaranteed that add_lwp added us to the end of the
     list, and so the new thread has not yet reached wait_for_sigstop (but
     will).  */
  if (! stopping_threads)
    new_lwp->stop_expected = 1;
}

int
linux_attach (unsigned long pid)
{
  struct lwp_info *lwp;
  int tgid = tgid_of_pid (pid);

  /* Maybe we could be smarter about this.  */
  if (tgid != 0 && tgid != pid)
    warning ("%s is a cloned process",
	     target_pid_to_str (pid_to_ptid (pid)));

  linux_attach_lwp (pid, 1);

  /* If this the first process?  If so, then set the arch.  */
  if (all_processes.head == NULL)
    new_inferior = 1;

  linux_add_process (pid, 1);

  /* Don't ignore the initial SIGSTOP if we just attached to this process.
     It will be collected by wait shortly.  */
  lwp = (struct lwp_info *) find_inferior_id (&all_lwps,
					      ptid_build (pid, pid, 0));
  lwp->stop_expected = 0;

  return 0;
}

struct counter
{
  int pid;
  int count;
};

static int
check_if_last_thread_of_pid (struct inferior_list_entry *entry, void *args)
{
  struct counter *counter = args;

  if (ptid_get_pid (entry->id) == counter->pid)
    {
      counter->count++;

      if (counter->count > 1)
	/* OK, we know it's not the last, we can stop counting.  */
	return 1;
    }

  return 0;
}

static int
is_last_thread_of_process (struct thread_info *thread)
{
  ptid_t ptid = ((struct inferior_list_entry *)thread)->id;
  int pid = ptid_get_pid (ptid);
  struct counter counter = { pid , 0 };

  return (find_inferior (&all_threads,
			 check_if_last_thread_of_pid, &counter) == NULL);
}

/* Kill the inferior lwp.  */

static int
linux_kill_one_lwp (struct inferior_list_entry *entry, void *args)
{
  struct thread_info *thread = (struct thread_info *) entry;
  struct lwp_info *lwp = get_thread_lwp (thread);
  int wstat;
  int pid = * (int *) args;

  if (ptid_get_pid (entry->id) != pid)
    return 0;

  /* We avoid killing the first thread here, because of a Linux kernel (at
     least 2.6.0-test7 through 2.6.8-rc4) bug; if we kill the parent before
     the children get a chance to be reaped, it will remain a zombie
     forever.  */

  if (pid_of (lwp) == lwpid_of (lwp))
    {
      if (debug_threads)
	fprintf (stderr, "lkop: is last of process %s\n",
		 target_pid_to_str (entry->id));
      return 0;
    }

  /* If we're killing a running inferior, make sure it is stopped
     first, as PTRACE_KILL will not work otherwise.  */
  if (!lwp->stopped)
    send_sigstop (&lwp->head);

  do
    {
      ptrace (PTRACE_KILL, lwpid_of (lwp), 0, 0);

      /* Make sure it died.  The loop is most likely unnecessary.  */
      pid = my_waitpid (lwpid_of (lwp), &wstat, __WALL);
    } while (pid == lwpid_of (lwp));

  remove_inferior (&all_lwps, &lwp->head);
  remove_thread (thread);

  return 0;
}

static int
linux_kill (int pid)
{
  struct process_info *process;
  struct lwp_info *lwp;
  struct thread_info *thread;
  int wstat;
  int lwpid;

  process =
    (struct process_info *) find_inferior_id (&all_processes, pid_to_ptid (pid));

  if (process == NULL)
    return -1;

  find_inferior (&all_threads, linux_kill_one_lwp, &pid);

  /* See the comment in linux_kill_one_lwp.  We did not kill the first
     thread in the list, so do so now.  */
  lwp = find_lwp_pid (pid_to_ptid (pid));
  thread = get_lwp_thread (lwp);

  if (debug_threads)
    fprintf (stderr, "lk_1: killing lwp %s, for pid: %d\n",
	     target_pid_to_str (lwp->head.id), pid);

  /* If we're killing a running inferior, make sure it is stopped
     first, as PTRACE_KILL will not work otherwise.  */
  if (!lwp->stopped)
    send_sigstop (&lwp->head);

  do
    {
      ptrace (PTRACE_KILL, lwpid_of (lwp), 0, 0);

      /* Make sure it died.  The loop is most likely unnecessary.  */
      lwpid = my_waitpid (lwpid_of (lwp), &wstat, __WALL);
    } while (lwpid == lwpid_of (lwp));

  remove_inferior (&all_lwps, &lwp->head);
  remove_thread (thread);
  remove_inferior (&all_processes, &process->head);

  return 0;
}

static int
linux_detach_one_lwp (struct inferior_list_entry *entry, void *args)
{
  struct thread_info *thread = (struct thread_info *) entry;
  struct lwp_info *lp = get_thread_lwp (thread);
  int pid = * (int *) args;

  if (ptid_get_pid (entry->id) != pid)
    return 0;

  /* If we're detaching from a running inferior, make sure it is
     stopped first, as PTRACE_DETACH will not work otherwise.  */
  if (!lp->stopped)
    {
      send_sigstop (&lp->head);
      wait_for_sigstop (&lp->head);
    }

  /* Make sure the LWP isn't stopped at a breakpoint that's
     no longer there.  */
  check_removed_breakpoint (lp);

  /* If this lwp is stopped but is expecting a SIGSTOP, then make
     sure we take care of that now.  This isn't absolutely guaranteed
     to collect the SIGSTOP, but is fairly likely to.  */
  if (lp->stop_expected)
    {
      int wstat;
      /* Clear stop_expected, so that the SIGSTOP will be reported.  */
      lp->stop_expected = 0;
      if (lp->stopped)
	linux_resume_one_lwp (&lp->head, 0, 0, NULL);
      linux_wait_for_event (lp->head.id, &wstat, __WALL);
    }

  /* Flush any pending changes to the lwp's registers.  */
  regcache_invalidate_one ((struct inferior_list_entry *)
			   get_lwp_thread (lp));

  /* Finally, let it resume.  */
  ptrace (PTRACE_DETACH, lwpid_of (lp), 0, 0);

  /* Get rid of it.  */
  remove_inferior (&all_lwps, &lp->head);
  remove_thread (thread);

  return 0;
}

static int
any_thread_of (struct inferior_list_entry *entry, void *args)
{
  int *pid_p = args;

  if (ptid_get_pid (entry->id) == *pid_p)
    return 1;

  return 0;
}

static int
linux_detach (int pid)
{
  struct process_info *process;

  process =
    (struct process_info *) find_inferior_id (&all_processes,
					      pid_to_ptid (pid));

  if (process == NULL)
    return -1;

  current_inferior =
    (struct thread_info *) find_inferior (&all_threads, any_thread_of, &pid);

  delete_all_breakpoints ();
  find_inferior (&all_threads, linux_detach_one_lwp, &pid);

  remove_inferior (&all_processes, &process->head);
  return 0;
}

static void
linux_join (int pid)
{
  int status, ret;

  struct process_info *process;
  process = (struct process_info *) find_inferior_id (&all_processes,
						      pid_to_ptid (pid));

  if (!process)
    return; /* weird */

  /* If we are attached, then we can exit.  Otherwise, we need to hang
     around doing nothing, until the child is gone.  */
  if (process->attached)
    return;

  do {
    ret = my_waitpid (pid, &status, 0);
    if (WIFEXITED (status) || WIFSIGNALED (status))
      break;
  } while (ret != -1 || errno != ECHILD);
}

/* Return nonzero if the given thread is still alive.  */
static int
linux_thread_alive (ptid_t ptid)
{
  struct lwp_info *lwp = find_lwp_pid (ptid);

  /* We assume we always know if a thread exits.  If a whole process
     exited but we still haven't been able to report it to GDB, we'll
     hold on to the last lwp of the dead process.  */
  if (lwp != NULL)
    return !lwp->dead;
  else
    return 0;
}

/* Return nonzero if this lwp stopped at a breakpoint which
   no longer appears to be inserted.  Also adjust the PC
   appropriately to resume where the breakpoint used to be.  */
static int
check_removed_breakpoint (struct lwp_info *event_child)
{
  CORE_ADDR stop_pc;
  struct thread_info *saved_inferior;

  if (event_child->pending_is_breakpoint == 0)
    return 0;

  if (debug_threads)
    fprintf (stderr, "Checking for breakpoint in lwp %ld.\n",
	     lwpid_of (event_child));

  saved_inferior = current_inferior;
  current_inferior = get_lwp_thread (event_child);

  stop_pc = get_stop_pc ();

  /* If the PC has changed since we stopped, then we shouldn't do
     anything.  This happens if, for instance, GDB handled the
     decr_pc_after_break subtraction itself.  */
  if (stop_pc != event_child->pending_stop_pc)
    {
      if (debug_threads)
	fprintf (stderr, "Ignoring, PC was changed.  Old PC was 0x%08llx\n",
		 event_child->pending_stop_pc);

      event_child->pending_is_breakpoint = 0;
      current_inferior = saved_inferior;
      return 0;
    }

  /* If the breakpoint is still there, we will report hitting it.  */
  if ((*the_low_target.breakpoint_at) (stop_pc))
    {
      if (debug_threads)
	fprintf (stderr, "Ignoring, breakpoint is still present.\n");
      current_inferior = saved_inferior;
      return 0;
    }

  if (debug_threads)
    fprintf (stderr, "Removed breakpoint.\n");

  /* For decr_pc_after_break targets, here is where we perform the
     decrement.  We go immediately from this function to resuming,
     and can not safely call get_stop_pc () again.  */
  if (the_low_target.set_pc != NULL)
    (*the_low_target.set_pc) (stop_pc);

  /* We consumed the pending SIGTRAP.  */
  event_child->pending_is_breakpoint = 0;
  event_child->status_pending_p = 0;
  event_child->status_pending = 0;

  current_inferior = saved_inferior;
  return 1;
}

/* Return 1 if this lwp has an interesting status pending.  This function
   may silently resume an inferior lwp.  */
static int
status_pending_p (struct inferior_list_entry *entry, void *arg)
{
  struct lwp_info *lwp = (struct lwp_info *) entry;
  ptid_t ptid = * (ptid_t *) arg;

  /* Check if we're only interested in events from a specific process
     or its lwps.  */
  if (!ptid_equal (minus_one_ptid, ptid)
      && ptid_get_pid (ptid) != ptid_get_pid (lwp->head.id))
    return 0;

  if (lwp->status_pending_p && !lwp->suspended)
    if (check_removed_breakpoint (lwp))
      {
	/* This thread was stopped at a breakpoint, and the breakpoint
	   is now gone.  We were told to continue (or step...) all threads,
	   so GDB isn't trying to single-step past this breakpoint.
	   So instead of reporting the old SIGTRAP, pretend we got to
	   the breakpoint just after it was removed instead of just
	   before; resume the lwp.  */
	linux_resume_one_lwp (&lwp->head, 0, 0, NULL);
	return 0;
      }

  return (lwp->status_pending_p && !lwp->suspended);
}

static int
same_lwp (struct inferior_list_entry *entry, void *data)
{
  ptid_t ptid = *(ptid_t *) data;
  int lwp;

  if (ptid_get_lwp (ptid) != 0)
    lwp = ptid_get_lwp (ptid);
  else
    lwp = ptid_get_pid (ptid);

  if (ptid_get_lwp (entry->id) == lwp)
    return 1;

  return 0;
}

struct lwp_info *
find_lwp_pid (ptid_t ptid)
{
  return (struct lwp_info*) find_inferior (&all_lwps, same_lwp, &ptid);
}

/* Wrapper function for waitpid which handles EINTR, and emulates
   __WALL for systems where that is not available.  */

static int
my_waitpid (int pid, int *status, int flags)
{
  int ret, out_errno;

  if (debug_threads)
    fprintf (stderr, "my_waitpid (%d, 0x%x)\n", pid, flags);

  if (flags & __WALL)
    {
      sigset_t mask;
      sigset_t org_mask;
      sigset_t suspend_mask;

      int org_flags = flags;
      int wnohang = (flags & WNOHANG) != 0;
      flags &= ~__WALL;
      flags |= WNOHANG;
      flags &= ~__WCLONE;

      sigemptyset (&mask);
      sigaddset (&mask, SIGCHLD);

      sigprocmask (SIG_BLOCK, &mask, &org_mask);

      /* Any signal unblocks the sigsuspend below.  */
      sigemptyset (&suspend_mask);

      while (1)
	{
	  do
	    {
	      errno = 0;
	      ret = waitpid (pid, status, flags);
	    }
	  while (ret == -1 && errno == EINTR);
	  out_errno = errno;

	  if (ret == -1)
	    {
	      if (out_errno != ECHILD)
		break;

	      /* We know there's no child of this flavour, recurse
		 with the right type.  */
	      org_flags &= ~__WALL;
	      org_flags |= (flags & __WCLONE) ? 0 : __WCLONE;
	      ret = my_waitpid (pid, status, org_flags);
	      out_errno = errno;
	      break;
	    }
	  else if (ret > 0)
	    break;

	  if (flags & __WCLONE)
	    {
	      if (wnohang)
		break;
	      else
		{
		  if (debug_threads)
		    fprintf (stderr, "blocking\n");

		  /* Block waiting for any signal.  Ideally, we'd wait for
		     SIGCHLD and LinuxThreads' signals, but this avoids
		     knowing which signals those are.  */
		  sigsuspend (&suspend_mask);
		}
	    }

	  flags ^= __WCLONE;
	}

      sigprocmask (SIG_SETMASK, &org_mask, NULL);
    }
  else
    {
      do
	ret = waitpid (pid, status, flags);
      while (ret == -1 && errno == EINTR);
      out_errno = errno;
    }

  if (debug_threads)
    fprintf (stderr, "my_waitpid (%d, 0x%x): status(%x), %d\n",
	     pid, flags, status ? *status : -1, ret);

  errno = out_errno;
  return ret;
}

static struct lwp_info *
linux_wait_for_lwp (ptid_t ptid, int *wstatp, int options)
{
  int ret;
  int to_wait_for = -1;
  struct lwp_info *child = NULL;

  if (debug_threads)
    fprintf (stderr, "linux_wait_for_lwp: %s\n", target_pid_to_str (ptid));

  if (ptid_equal (ptid, minus_one_ptid))
    to_wait_for = -1;			/* any child */
  else if (is_lwpid (ptid))
    to_wait_for = ptid_get_lwp (ptid);	/* this lwp only */
  else
    to_wait_for = -1;			/* a specific tgid, but this
					   is not possible.  We wait
					   for everything, and cache
					   what we don't need now.  */

  options |= __WALL;

retry:

  ret = my_waitpid (to_wait_for, wstatp, options);
  if (ret == 0 || (ret == -1 && errno == ECHILD && (options & WNOHANG)))
    return NULL;
  else if (ret == -1)
    perror_with_name ("waitpid");

  if (debug_threads
      && (!WIFSTOPPED (*wstatp)
	  || (WSTOPSIG (*wstatp) != 32
	      && WSTOPSIG (*wstatp) != 33)))
    fprintf (stderr, "Got an event from %d (%x)\n", ret, *wstatp);

  child = find_lwp_pid (pid_to_ptid (ret));

  /* If we didn't find a lwp, one of two things presumably happened:
     - A lwp we started and then detached from has exited.  Ignore it.
     - A lwp we are controlling has forked and the new child's stop
     was reported to us by the kernel.  Save its PID.  */
  if (child == NULL && WIFSTOPPED (*wstatp))
    {
      add_pid_to_list (&stopped_pids, ret);
      if (options & WNOHANG)
	return NULL;
      goto retry;
    }
  else if (child == NULL)
    {
      if (options & WNOHANG)
	return NULL;
      goto retry;
    }

  child->stopped = 1;
  child->pending_is_breakpoint = 0;

  child->last_status = *wstatp;

  /* Architecture-specific setup after inferior is running.
     This needs to happen after we have attached to the inferior
     and it is stopped for the first time, but before we access
     any inferior registers.  */
  if (new_inferior)
    {
      the_low_target.arch_setup ();
#ifdef HAVE_LINUX_REGSETS
      memset (disabled_regsets, 0, num_regsets);
#endif
      new_inferior = 0;
    }

  if (debug_threads
      && WIFSTOPPED (*wstatp))
    {
      current_inferior = (struct thread_info *)
	find_inferior_id (&all_threads, child->head.id);
      /* For testing only; i386_stop_pc prints out a diagnostic.  */
      if (the_low_target.get_pc != NULL)
	get_stop_pc ();
    }

  if (ptid_is_pid (ptid))
    {
      if (pid_of (child) != ptid_get_pid (ptid))
	{
	  if (debug_threads)
	    fprintf (stderr, "LWP %ld got an event %06x, leaving pending.\n",
		     lwpid_of (child), *wstatp);
	  child->status_pending_p = 1;
	  child->status_pending = *wstatp;
	  if ((options & WNOHANG) == 0)
	    goto retry;

	  child = NULL;
	}
    }

  return child;
}

static int
resume_stopped_lwps (struct inferior_list_entry *entry, void *arg)
{
  ptid_t ptid = * (ptid_t *) arg;
  struct lwp_info *lwp = (struct lwp_info *) entry;

  if ((ptid_equal (ptid, minus_one_ptid)
       || (ptid_is_pid (ptid)
	   && ptid_get_pid (ptid) == pid_of (lwp)))
      && lwp->stopped
      && !lwp->suspended)
    {
      if (debug_threads)
	fprintf (stderr, "resuming stopped LWP %ld\n", lwpid_of (lwp));
      linux_resume_one_lwp (&lwp->head, 0, 0, NULL);
    }

  return 0;
}


static int
linux_wait_for_event (ptid_t ptid, int *wstat, int options)
{
  CORE_ADDR stop_pc;
  struct lwp_info *event_child = NULL;
  int bp_status;
  struct lwp_info *requested_child = NULL;

  /* Check for a lwp with a pending status.  */
  /* It is possible that the user changed the pending task's registers since
     it stopped.  We correctly handle the change of PC if we hit a breakpoint
     (in check_removed_breakpoint); signals should be reported anyway.  */

  if (ptid_equal (ptid, minus_one_ptid)
      || ptid_equal (pid_to_ptid (ptid_get_pid (ptid)), ptid))
    {
      event_child = (struct lwp_info *)
	find_inferior (&all_lwps, status_pending_p, &ptid);
      if (debug_threads && event_child)
	fprintf (stderr, "Got a pending child %ld\n", lwpid_of (event_child));

      /* If we just handled a pending status, and the event was not
	 interesting to report to GDB, we will reach here again.  This
	 time, we may find that there are no more interesting pending
	 statuses to handle, but, the previous iteration left all
	 threads stopped.  Resume them now.

	 Don't do this in non-stop mode, as that would resume threads
	 silently behind GDB's back --- e.g., just after starting a
	 new inferior, we could get here due to a spurious
	 target_wait(..., TARGET_WNOHANG) call.  In that case, we
	 don't want to resume all stopped threads.  In all-stop, there
	 should be no wnohang calls, so we always want to resume
	 threads.  */
      if (!non_stop && !event_child)
	find_inferior (&all_lwps, resume_stopped_lwps, &ptid);
    }
  else
    {
      requested_child = find_lwp_pid (ptid);
      if (requested_child->status_pending_p
	  && !check_removed_breakpoint (requested_child))
	event_child = requested_child;
    }

  if (event_child != NULL)
    {
      if (debug_threads)
	fprintf (stderr, "Got an event from pending child %ld (%04x)\n",
		 ptid_get_lwp (event_child->head.id), event_child->status_pending);
      *wstat = event_child->status_pending;
      event_child->status_pending_p = 0;
      event_child->status_pending = 0;
    }

  /* If no lwp of interested had a pending wait status, wait for
     one.  */
  if (!event_child)
    event_child = linux_wait_for_lwp (ptid, wstat, options);

    {
      if ((options & WNOHANG) && event_child == NULL)
	return -1;

      if (event_child == NULL)
	error ("event from unknown child");

      current_inferior = get_lwp_thread (event_child);

      /* Check for thread exit.  */
      if (! WIFSTOPPED (*wstat))
	{
	  if (debug_threads)
	    fprintf (stderr, "LWP %ld exiting\n",
		     ptid_get_lwp (event_child->head.id));

	  /* If the last thread is exiting, just return.  */
	  if (is_last_thread_of_process (current_inferior))
	    {
	      if (debug_threads)
		fprintf (stderr, "LWP %ld is last lwp of process\n",
			 ptid_get_lwp (event_child->head.id));
	      return 0;
	    }
#if 0
	  dead_thread_notify (thread_id_to_gdb_id (event_child->lwpid));
#endif

	  remove_inferior (&all_lwps, &event_child->head);
	  free (event_child);
	  remove_thread (current_inferior);

	  if (!non_stop)
	    {
	      current_inferior = (struct thread_info *) all_threads.head;
	      if (debug_threads)
		fprintf (stderr, "Current inferior is now %ld\n",
			 lwpid_of (get_thread_lwp (current_inferior)));
	    }
	  else
	    {
	      current_inferior = NULL;
	      if (debug_threads)
		fprintf (stderr, "Current inferior is now <NULL>\n");
	    }

	  /* If we were waiting for this particular child to do something...
	     well, it did something.  */
	  if (requested_child != NULL)
	    return 0;

	  /* Wait for a more interesting event.  */
	  return -1;
	}

      if (WIFSTOPPED (*wstat)
	  && WSTOPSIG (*wstat) == SIGSTOP
	  && event_child->stop_expected)
	{
	  if (debug_threads)
	    fprintf (stderr, "Expected stop.\n");
	  event_child->stop_expected = 0;
	  linux_resume_one_lwp (&event_child->head,
				event_child->stepping, 0, NULL);
	  return -1;
	}

      if (WIFSTOPPED (*wstat) && WSTOPSIG (*wstat) == SIGTRAP
	  && *wstat >> 16 != 0)
	{
	  if (handle_extended_wait (event_child, *wstat))
	    return -1;
	}

      /* If GDB is not interested in this signal, don't stop other
	 threads, and don't report it to GDB.  Just resume the
	 inferior right away.  We do this for threading-related
	 signals as well as any that GDB specifically requested we
	 ignore.  But never ignore SIGSTOP if we sent it ourselves,
	 and do not ignore signals when stepping - they may require
	 special handling to skip the signal handler.  */
      /* FIXME drow/2002-06-09: Get signal numbers from the inferior's
	 thread library?  */
      if (WIFSTOPPED (*wstat)
	  && !event_child->stepping
	  && (
#ifdef USE_THREAD_DB
	      (current_process ()->private->thread_db_active
	       && (WSTOPSIG (*wstat) == __SIGRTMIN
		   || WSTOPSIG (*wstat) == __SIGRTMIN + 1))
	      ||
#endif
	      (pass_signals[target_signal_from_host (WSTOPSIG (*wstat))]
	       && (WSTOPSIG (*wstat) != SIGSTOP || !stopping_threads))))
	{
	  siginfo_t info, *info_p;

	  if (debug_threads)
	    fprintf (stderr, "Ignored signal %d for LWP %ld.\n",
		     WSTOPSIG (*wstat), ptid_get_lwp (event_child->head.id));

	  if (ptrace (PTRACE_GETSIGINFO, ptid_get_lwp (event_child->head.id), 0, &info) == 0)
	    info_p = &info;
	  else
	    info_p = NULL;

	  linux_resume_one_lwp (&event_child->head,
				event_child->stepping,
				WSTOPSIG (*wstat), info_p);
	  return -1;
	}

      /* If this event was not handled above, and is not a SIGTRAP, report
	 it.  */
      if (!WIFSTOPPED (*wstat) || WSTOPSIG (*wstat) != SIGTRAP)
	return 0;

      /* If this target does not support breakpoints, we simply report the
	 SIGTRAP; it's of no concern to us.  */
      if (the_low_target.get_pc == NULL)
	return 0;

      stop_pc = get_stop_pc ();

      /* bp_reinsert will only be set if we were single-stepping.
	 Notice that we will resume the lwp after hitting
	 a gdbserver breakpoint; single-stepping to/over one
	 is not supported (yet).  */
      if (event_child->bp_reinsert != 0)
	{
	  if (debug_threads)
	    fprintf (stderr, "Reinserted breakpoint.\n");
	  reinsert_breakpoint (event_child->bp_reinsert);
	  event_child->bp_reinsert = 0;

	  /* Clear the single-stepping flag and SIGTRAP as we resume.  */
	  linux_resume_one_lwp (&event_child->head, 0, 0, NULL);
	  return -1;
	}

      bp_status = check_breakpoints (stop_pc);

      if (bp_status != 0)
	{
	  if (debug_threads)
	    fprintf (stderr, "Hit a gdbserver breakpoint.\n");

	  /* We hit one of our own breakpoints.  We mark it as a pending
	     breakpoint, so that check_removed_breakpoint () will do the PC
	     adjustment for us at the appropriate time.  */
	  event_child->pending_is_breakpoint = 1;
	  event_child->pending_stop_pc = stop_pc;

	  /* We may need to put the breakpoint back.  We continue in the event
	     loop instead of simply replacing the breakpoint right away,
	     in order to not lose signals sent to the thread that hit the
	     breakpoint.  Unfortunately this increases the window where another
	     thread could sneak past the removed breakpoint.  For the current
	     use of server-side breakpoints (thread creation) this is
	     acceptable; but it needs to be considered before this breakpoint
	     mechanism can be used in more general ways.  For some breakpoints
	     it may be necessary to stop all other threads, but that should
	     be avoided where possible.

	     If breakpoint_reinsert_addr is NULL, that means that we can
	     use PTRACE_SINGLESTEP on this platform.  Uninsert the breakpoint,
	     mark it for reinsertion, and single-step.

	     Otherwise, call the target function to figure out where we need
	     our temporary breakpoint, create it, and continue executing this
	     lwp.  */

	  /* NOTE: we're lifting breakpoints in non-stop mode.  This
	     is currently only used for thread event breakpoints, so
	     it isn't that bad as long as we have PTRACE_EVENT_CLONE
	     events.  */
	  if (bp_status == 2)
	    /* No need to reinsert.  */
	    linux_resume_one_lwp (&event_child->head, 0, 0, NULL);
	  else if (the_low_target.breakpoint_reinsert_addr == NULL)
	    {
	      event_child->bp_reinsert = stop_pc;
	      uninsert_breakpoint (stop_pc);
	      linux_resume_one_lwp (&event_child->head, 1, 0, NULL);
	    }
	  else
	    {
	      reinsert_breakpoint_by_bp
		(stop_pc, (*the_low_target.breakpoint_reinsert_addr) ());
	      linux_resume_one_lwp (&event_child->head, 0, 0, NULL);
	    }

	  return -1;
	}

      if (debug_threads)
	fprintf (stderr, "Hit a non-gdbserver breakpoint.\n");

      /* If we were single-stepping, we definitely want to report the
	 SIGTRAP.  The single-step operation has completed, so also
         clear the stepping flag; in general this does not matter,
	 because the SIGTRAP will be reported to the client, which
	 will give us a new action for this thread, but clear it for
	 consistency anyway.  It's safe to clear the stepping flag
         because the only consumer of get_stop_pc () after this point
	 is check_removed_breakpoint, and pending_is_breakpoint is not
	 set.  It might be wiser to use a step_completed flag instead.  */
      if (event_child->stepping)
	{
	  event_child->stepping = 0;
	  return 0;
	}

      /* A SIGTRAP that we can't explain.  It may have been a breakpoint.
	 Check if it is a breakpoint, and if so mark the lwp information
	 accordingly.  This will handle both the necessary fiddling with the
	 PC on decr_pc_after_break targets and suppressing extra threads
	 hitting a breakpoint if two hit it at once and then GDB removes it
	 after the first is reported.  Arguably it would be better to report
	 multiple threads hitting breakpoints simultaneously, but the current
	 remote protocol does not allow this.  */
      if ((*the_low_target.breakpoint_at) (stop_pc))
	{
	  event_child->pending_is_breakpoint = 1;
	  event_child->pending_stop_pc = stop_pc;
	}

      return 0;
    }

  /* NOTREACHED */
  return 0;
}

/* Wait for process, returns status.  */

static ptid_t
linux_wait_1 (ptid_t ptid, struct target_waitstatus *ourstatus, int target_options)
{
  int w;
  struct thread_info *thread = NULL;
  struct lwp_info *lwp = NULL;
  int options;

  /* Translate generic target options into linux options.  */
  options = __WALL;
  if (target_options & TARGET_WNOHANG)
    options |= WNOHANG;

  ourstatus->kind = TARGET_WAITKIND_IGNORE;

  /* If we were only supposed to resume one thread, only wait for
     that thread - if it's still alive.  If it died, however - which
     can happen if we're coming from the thread death case below -
     then we need to make sure we restart the other threads.  We could
     pick a thread at random or restart all; restarting all is less
     arbitrary.  */
  /* TODO: Check if the incoming PTID is requesting an event for an
     lwp of another process?  */
  /* Don't do this in non-stop.  */
  if (!non_stop
      && !ptid_equal (cont_thread, null_ptid)
      && !ptid_equal (cont_thread, minus_one_ptid))
    {
      thread = (struct thread_info *) find_inferior_id (&all_threads,
							cont_thread);

      /* No stepping, no signal - unless one is pending already, of course.  */
      if (thread == NULL)
	{
	  struct thread_resume resume_info;
	  resume_info.thread = minus_one_ptid;
	  resume_info.kind = rk_continue;
	  resume_info.sig = 0;
	  linux_resume (&resume_info, 1);
	}
      else
	ptid = cont_thread;
    }

  if (linux_wait_for_event (ptid, &w, options) != 0)
    return minus_one_ptid;

  if (!non_stop)
    stop_all_lwps ();

  if (must_set_ptrace_flags)
    {
      linux_enable_event_reporting (inferior_lwpid);
      must_set_ptrace_flags = 0;
    }

  /* If we are waiting for a particular child, and it exited,
     linux_wait_for_event will return its exit status.  Similarly if
     the last child exited.  If this is not the last child, however,
     do not report it as exited until there is a 'thread exited' response
     available in the remote protocol.  Instead, just wait for another event.
     This should be safe, because if the thread crashed we will already
     have reported the termination signal to GDB; that should stop any
     in-progress stepping operations, etc.

     Report the exit status of the last thread to exit.  This matches
     LinuxThreads' behavior.  */

  lwp = get_thread_lwp (current_inferior);

  /* Now that we've selected our final event LWP, cancel any
     breakpoints in other LWPs that have hit a GDB breakpoint.  See
     the comment in cancel_breakpoints_callback to find out why.  */
  if (!non_stop)
    find_inferior (&all_lwps, cancel_breakpoints_callback, lwp);

  if (is_last_thread_of_process (current_inferior))
    {
      if (WIFEXITED (w) || WIFSIGNALED (w))
	{
	  struct process_info *process;
	  int pid;

	  pid = pid_of (lwp);

	  process = (struct process_info *)
	    find_inferior_id (&all_processes, pid_to_ptid (pid));

	  remove_inferior (&all_lwps, &lwp->head);
	  remove_thread (current_inferior);
	  remove_inferior (&all_processes, &process->head);

	  current_inferior = NULL;

	  if (WIFEXITED (w))
	    {
	      ourstatus->kind = TARGET_WAITKIND_EXITED;
	      ourstatus->value.integer = WEXITSTATUS (w);

	      if (debug_threads)
		fprintf (stderr, "\nChild exited with retcode = %x \n", WEXITSTATUS (w));

	      return pid_to_ptid (pid);
	    }
	  else
	    {
	      ourstatus->kind = TARGET_WAITKIND_SIGNALLED;
	      ourstatus->value.sig = target_signal_from_host (WTERMSIG (w));

	      if (debug_threads)
		fprintf (stderr, "\nChild terminated with signal = %x \n", WTERMSIG (w));

	      return pid_to_ptid (pid);
	    }
	}
    }
  else
    {
      if (!WIFSTOPPED (w))
	return lwp->head.id;
    }

  if (lwp->waitstatus.kind != TARGET_WAITKIND_IGNORE)
    {
      *ourstatus = lwp->waitstatus;
      lwp->waitstatus.kind = TARGET_WAITKIND_IGNORE;
    }
  else
    {
      ourstatus->kind = TARGET_WAITKIND_STOPPED;
      ourstatus->value.sig = target_signal_from_host (WSTOPSIG (w));
    }

  if (debug_threads)
    fprintf (stderr, "linux_wait ret = %s, %d, %d\n",
	     target_pid_to_str (lwp->head.id),
	     ourstatus->kind,
	     ourstatus->value.sig);

  return lwp->head.id;
}

/* Get rid of any pending event in the pipe.  */
static void
async_file_flush (void)
{
  int ret;
  char buf;

  do
    ret = read (linux_event_pipe[0], &buf, 1);
  while (ret == 0 || (ret == -1 && errno == EINTR));
}

/* Put something in the pipe, so the event loop wakes up.  */
static void
async_file_mark (void)
{
  int ret;

  async_file_flush ();

  do
    ret = write (linux_event_pipe[1], "+", 1);
  while (ret == 0 || (ret == -1 && errno == EINTR));
}

/* */
static ptid_t
linux_wait (ptid_t ptid, struct target_waitstatus *ourstatus, int target_options)
{
  ptid_t event_ptid;

  if (debug_threads)
    fprintf (stderr, "linux_wait: [%s]\n", target_pid_to_str (ptid));

  /* Flush the async file first.  We can't do it below, because that
     would be racy.  A SIGCHLD may arrive any time after we get out of
     my_waitpid, which marks the async file.  */
  if (target_is_async_p ())
    async_file_flush ();

  do
    {
      event_ptid = linux_wait_1 (ptid, ourstatus, target_options);
    } while ((target_options & TARGET_WNOHANG) == 0
	     && ourstatus->kind == TARGET_WAITKIND_IGNORE);

  /* If we requested any event, and something came out, assume there
     may be more.  If we requested a specific lwp or process, also
     assume there may be more.  */
  if (target_is_async_p ())
    {
      if (!ptid_equal (ptid, minus_one_ptid)
	  || ourstatus->kind != TARGET_WAITKIND_IGNORE)
	async_file_mark ();
    }

  return event_ptid;
}

/* Send a signal to an LWP.  For LinuxThreads, kill is enough; however, if
   thread groups are in use, we need to use tkill.  */

static int
kill_lwp (unsigned long lwpid, int signo)
{
  static int tkill_failed;

  errno = 0;

#ifdef SYS_tkill
  if (!tkill_failed)
    {
      int ret = syscall (SYS_tkill, lwpid, signo);
      if (errno != ENOSYS)
        return ret;
      errno = 0;
      tkill_failed = 1;
    }
#endif

  return kill (lwpid, signo);
}

static void
send_sigstop (struct inferior_list_entry *entry)
{
  int lwpid;
  struct lwp_info *lwp;

  lwp = (struct lwp_info *) entry;

  if (lwp->stopped)
    return;

  lwpid = lwpid_of (lwp);

  /* If we already have a pending stop signal for this lwp, don't
     send another.  */
  if (lwp->stop_expected)
    {
      if (debug_threads)
	fprintf (stderr, "Have pending sigstop for lwp %d\n",
		 lwpid);

      /* We clear the stop_expected flag so that wait_for_sigstop
	 will receive the SIGSTOP event (instead of silently resuming and
	 waiting again).  It'll be reset below.  */
      lwp->stop_expected = 0;
      return;
    }

  if (debug_threads)
    fprintf (stderr, "Sending sigstop to lwp %d\n", lwpid);

  kill_lwp (lwpid, SIGSTOP);
}

static void
wait_for_sigstop (struct inferior_list_entry *entry)
{
  struct lwp_info *lwp = (struct lwp_info *) entry;
  struct thread_info *saved_inferior;
  int wstat;
  ptid_t saved_tid;
  ptid_t ptid;

  if (lwp->stopped)
    return;

  saved_inferior = current_inferior;
  saved_tid = ((struct inferior_list_entry *) saved_inferior)->id;

  ptid = lwp->head.id;

  stopping_threads = 1;
  linux_wait_for_lwp (ptid, &wstat, __WALL);
  stopping_threads = 0;

  /* If we stopped with a non-SIGSTOP signal, save it for later
     and record the pending SIGSTOP.  If the lwp exited, just
     return.  */
  if (WIFSTOPPED (wstat)
      && WSTOPSIG (wstat) != SIGSTOP)
    {
      if (debug_threads)
	fprintf (stderr, "LWP %ld stopped with non-sigstop status %06x\n",
		 lwpid_of (lwp), wstat);
      lwp->status_pending_p = 1;
      lwp->status_pending = wstat;
      lwp->stop_expected = 1;
    }
  else if (!WIFSTOPPED (wstat))
    {
      if (debug_threads)
	fprintf (stderr, "Process %ld exited while stopping LWPs\n",
		 ptid_get_lwp (lwp->head.id));

      /* Leave this status pending for the next time we're able to
	 report it.  In the mean time, we'll report this lwp as
	 dead to GDB, so GDB doesn't try to read registers and
	 memory from it.  */
      lwp->dead = 1;
      lwp->status_pending_p = 1;
      lwp->status_pending = wstat;
      lwp->stopped = 1; /* prevent trying to stop it again.  */
      lwp->stop_expected = 0; /* No stop is expected, the lwp is
				 dead.  */
    }

  if (non_stop)
    /* We can't change the current inferior behind GDB's back,
       otherwise, a subsequent command may apply to the wrong process.
       Actually, we have to take care of this in all-stop +
       multi-process too.  FIXME.  */
    current_inferior = saved_inferior;
  else
    {
      if (linux_thread_alive (saved_tid))
	current_inferior = saved_inferior;
      else
	{
	  if (debug_threads)
	    fprintf (stderr, "Previously current thread died.\n");

	  /* Set a valid thread as current.  */
	  set_desired_inferior (0);
	}
    }
}

static void
stop_all_lwps (void)
{
  for_each_inferior (&all_lwps, send_sigstop);
  for_each_inferior (&all_lwps, wait_for_sigstop);
}

/* Resume execution of the inferior lwp.
   If STEP is nonzero, single-step it.
   If SIGNAL is nonzero, give it that signal.  */

static void
linux_resume_one_lwp (struct inferior_list_entry *entry,
		      int step, int signal, siginfo_t *info)
{
  struct lwp_info *lwp = (struct lwp_info *) entry;
  struct thread_info *saved_inferior;

  if (lwp->stopped == 0)
    return;

  /* If we have pending signals or status, and a new signal, enqueue the
     signal.  Also enqueue the signal if we are waiting to reinsert a
     breakpoint; it will be picked up again below.  */
  if (signal != 0
      && (lwp->status_pending_p || lwp->pending_signals != NULL
	  || lwp->bp_reinsert != 0))
    {
      struct pending_signals *p_sig;
      p_sig = malloc (sizeof (*p_sig));
      p_sig->prev = lwp->pending_signals;
      p_sig->signal = signal;
      if (info == NULL)
	memset (&p_sig->info, 0, sizeof (siginfo_t));
      else
	memcpy (&p_sig->info, info, sizeof (siginfo_t));
      lwp->pending_signals = p_sig;
    }

  if (lwp->status_pending_p && !check_removed_breakpoint (lwp))
    return;

  saved_inferior = current_inferior;
  current_inferior = get_lwp_thread (lwp);

  if (debug_threads)
    fprintf (stderr, "Resuming lwp %ld (%s, signal %d, stop %s)\n", inferior_lwpid,
	     step ? "step" : "continue", signal,
	     lwp->stop_expected ? "expected" : "not expected");

  /* This bit needs some thinking about.  If we get a signal that
     we must report while a single-step reinsert is still pending,
     we often end up resuming the thread.  It might be better to
     (ew) allow a stack of pending events; then we could be sure that
     the reinsert happened right away and not lose any signals.

     Making this stack would also shrink the window in which breakpoints are
     uninserted (see comment in linux_wait_for_lwp) but not enough for
     complete correctness, so it won't solve that problem.  It may be
     worthwhile just to solve this one, however.  */
  if (lwp->bp_reinsert != 0)
    {
      if (debug_threads)
	fprintf (stderr, "  pending reinsert at %08lx", (long)lwp->bp_reinsert);
      if (step == 0)
	fprintf (stderr, "BAD - reinserting but not stepping.\n");
      step = 1;

      /* Postpone any pending signal.  It was enqueued above.  */
      signal = 0;
    }

  check_removed_breakpoint (lwp);

  if (debug_threads && the_low_target.get_pc != NULL)
    {
      fprintf (stderr, "  ");
      (*the_low_target.get_pc) ();
    }

  /* If we have pending signals, consume one unless we are trying to reinsert
     a breakpoint.  */
  if (lwp->pending_signals != NULL && lwp->bp_reinsert == 0)
    {
      struct pending_signals **p_sig;

      p_sig = &lwp->pending_signals;
      while ((*p_sig)->prev != NULL)
	p_sig = &(*p_sig)->prev;

      signal = (*p_sig)->signal;
      if ((*p_sig)->info.si_signo != 0)
	ptrace (PTRACE_SETSIGINFO, ptid_get_lwp (lwp->head.id), 0, &(*p_sig)->info);

      free (*p_sig);
      *p_sig = NULL;
    }

  regcache_invalidate_one ((struct inferior_list_entry *)
			   get_lwp_thread (lwp));
  errno = 0;
  lwp->stopped = 0;
  lwp->stepping = step;
  ptrace (step ? PTRACE_SINGLESTEP : PTRACE_CONT, lwpid_of (lwp), 0, signal);

  current_inferior = saved_inferior;
  if (errno)
    {
      /* ESRCH from ptrace either means that the thread was already
	 running (an error) or that it is gone (a race condition).  If
	 it's gone, we will get a notification the next time we wait,
	 so we can ignore the error.  We could differentiate these
	 two, but it's tricky without waiting; the thread still exists
	 as a zombie, so sending it signal 0 would succeed.  So just
	 ignore ESRCH.  */
      if (errno == ESRCH)
	return;

      perror_with_name ("ptrace");
    }
}

struct resume_info
{
  struct thread_resume *request;
  size_t n;
};

/* This function is called once per thread.  We look up the thread in
   RESUME_INFO, and mark the thread with a pointer to the appropriate
   resume request.

   This algorithm is O(threads * resume elements), but resume elements
   is small (and will remain small at least until GDB supports thread
   suspension).  */
static int
linux_set_resume_request (struct inferior_list_entry *entry, void *arg)
{
  struct lwp_info *lwp;
  struct thread_info *thread;
  int ndx;
  struct resume_info *resume_info;

  thread = (struct thread_info *) entry;
  lwp = get_thread_lwp (thread);
  resume_info = arg;

  for (ndx = 0; ndx < resume_info->n; ndx++)
    {
      ptid_t ptid = resume_info->request[ndx].thread;
      if (ptid_equal (ptid, minus_one_ptid)
	  || ptid_equal (ptid, entry->id)
	  || (ptid_is_pid (ptid)
	      && (ptid_get_pid (ptid) == pid_of (lwp)))
	  || (ptid_get_lwp (ptid) == -1
	      && (ptid_get_pid (ptid) == pid_of (lwp))))
	{
	  lwp->resume = &resume_info->request[ndx];
	  return 0;
	}
    }

  /* Don't touch.  */
  lwp->resume = NULL;

  return 0;
}

/* Set *FLAG_P if this lwp has an interesting status pending.  */
static int
resume_status_pending_p (struct inferior_list_entry *entry, void *flag_p)
{
  struct lwp_info *lwp = (struct lwp_info *) entry;

  /* LWPs which will not be resumed are not interesting, because
     we might not wait for them next time through linux_wait.  */
  if (lwp->resume == NULL)
    return 0;

  if (lwp->dead)
    {
      /* If this thread is known to be dead, then the whole process it
	 belonged to has exited.  Report it.  */

      /* TODO: See if the dead flag is really needed, or the
	 pending_status is enough.  */
      if (!lwp->status_pending_p)
	warning ("dead thread without a pending status?");

      * (int *) flag_p = 1;
      return 0;
    }

  /* If this thread has a removed breakpoint, we won't have any
     events to report later, so check now.  check_removed_breakpoint
     may clear status_pending_p.  We avoid calling check_removed_breakpoint
     for any thread that we are not otherwise going to resume - this
     lets us preserve stopped status when two threads hit a breakpoint.
     GDB removes the breakpoint to single-step a particular thread
     past it, then re-inserts it and resumes all threads.  We want
     to report the second thread without resuming it in the interim.  */
  if (lwp->status_pending_p && !lwp->suspended)
    check_removed_breakpoint (lwp);

  if (lwp->status_pending_p)
    * (int *) flag_p = 1;

  return 0;
}

static int
cancel_breakpoint (struct lwp_info *lwp)
{
  /* Arrange for a breakpoint to be hit again later.  We don't keep
     the SIGTRAP status and don't forward the SIGTRAP signal to the
     thread.  Eventually we will resume this thread, and this
     breakpoint will trap again.

     If we do not do this, then we run the risk that the user will
     delete or disable the breakpoint, but the thread will have
     already tripped on it.  */

  if (lwp->stopped
      && !lwp->dead
      && !lwp->stepping
      && ((lwp->status_pending_p
	   && WIFSTOPPED (lwp->status_pending)
	   && WSTOPSIG (lwp->status_pending) == SIGTRAP)
	  || (!lwp->status_pending_p
	      && WIFSTOPPED (lwp->last_status)
	      && WSTOPSIG (lwp->last_status) == SIGTRAP)))
    {
      CORE_ADDR stop_pc;
      struct thread_info *saved_inferior;

      /* If there's a breakpoint here, adjust the PC, so the
	 breakpoint is hit again when the thread is resumed.  */

      if (debug_threads)
	fprintf (stderr, "Checking for breakpoint in lwp %ld.\n",
		 lwpid_of (lwp));

      saved_inferior = current_inferior;
      current_inferior = get_lwp_thread (lwp);

      stop_pc = (*the_low_target.get_pc) ();
      stop_pc -= the_low_target.decr_pc_after_break;

      /* If there's a breakpoint there, back off the PC.  */
      if (breakpoint_at (stop_pc)
	  || (*the_low_target.breakpoint_at) (stop_pc))
	{
	  if (the_low_target.set_pc != NULL)
	    {
	      if (debug_threads)
		fprintf (stderr, "CB: breakpoint present, backing off PC.\n");
	      (*the_low_target.set_pc) (stop_pc);
	    }
	  else
	    {
	      if (debug_threads)
		fprintf (stderr, "CB: breakpoint present ignoring SIGTRAP.\n");
	    }

	  lwp->pending_is_breakpoint = 0;
	  lwp->status_pending_p = 0;
	  lwp->status_pending = 0;
	  lwp->last_status = W_STOPCODE (0);

	  current_inferior = saved_inferior;
	  return 1;
	}

      current_inferior = saved_inferior;
    }

  return 0;
}

static int
cancel_breakpoints_callback (struct inferior_list_entry *entry, void *data)
{
  struct lwp_info *lp = (struct lwp_info *) entry;
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

  cancel_breakpoint (lp);
  return 0;
}

/* This function is called once per thread.  We check the thread's resume
   request, which will tell us whether to resume, step, or leave the thread
   stopped; and what signal, if any, it should be sent.

   For threads which we aren't explicitly told otherwise, we preserve
   the stepping flag; this is used for stepping over gdbserver-placed
   breakpoints.

   If pending_flags was set in any thread, we queue any needed
   signals, since we won't actually resume.  We already have a pending
   event to report, so we don't need to preserve any step requests;
   they should be re-issued if necessary.  */

static int
linux_resume_one_thread (struct inferior_list_entry *entry, void *arg)
{
  struct lwp_info *lwp;
  struct thread_info *thread;
  int step;
  int pending_flag = * (int *) arg;

  thread = (struct thread_info *) entry;
  lwp = get_thread_lwp (thread);

  if (lwp->resume == NULL)
    return 0;

  if (lwp->resume->kind == rk_stop)
    {
      if (debug_threads)
	fprintf (stderr, "suspending thread %s\n",
		 target_pid_to_str (entry->id));

      /* Do this before waiting for sigstop, as the thread may
	 die.  */
      /* TODO: how do we report that happening to GDB?  GDB and the
	 user can get very confused if a thread exits without GDB
	 being aware of it.  E.g, user tries to interrupts thread, but
	 GDB doesn't reply back, The simplest seem to be to implement
	 a thread exit stop reply.  Note that returning a generic
	 error is not an option if we're handling a vCont.  The reply
	 has already been sent at this point, and, the same vCont can
	 hold multiple resumptions -- to which would the error
	 apply?  */
      lwp->resume = NULL;

      /* To simplify things, we're waiting for the lwp to stop here,
	 and pushing the stop reply to gdbserver's common code.  An
	 alternative, would be to defer to linux_wait to do the wait,
	 and notice that an lwp with the "suspended" flag but
	 "!stopped", should leave any status pending, and report a
	 sig0 stop status.  */

      /* Do we have to distinguish internal stops from external stops,
	 due to the support for gdbserver breakpoints?  If so, then
	 thread_info needs to gain a "running" property.  */
      if (!lwp->stopped)
	{
	  ptid_t ptid = entry->id;

	  if (debug_threads)
	    fprintf (stderr, "running -> suspending %s\n",
		     target_pid_to_str (entry->id));

	  lwp->suspended = 1;
	  send_sigstop (&lwp->head);
	  wait_for_sigstop (&lwp->head);
	  if (!lwp->dead)
	    {
	      struct target_waitstatus status;
	      status.kind = TARGET_WAITKIND_STOPPED;

	      /* Cancel internal breakpoints, otherwise, the user will
		 see a possibly invalid PC on decr_pc_after_break
		 archs, because GDB can't tell there's a breakpoint
		 there.  As long as we do it, might as well cancel GDB
		 breakpoints too, although GDB will also adjust the PC
		 if we don't in that case.  */

	      /* Cancel breakpoints, but leave out finished steps and
		 watchpoint hits.  */
	      if (cancel_breakpoint (lwp))
		/* The LWP hit a breakpoint while we tried to stop it,
		   and we backed off the PC.  Report a SIG0.  */
		status.value.sig = 0;
	      else if (lwp->stopped
		       && lwp->stepping
		       && ((!lwp->status_pending_p
			    && WIFSTOPPED (lwp->last_status)
			    && WSTOPSIG (lwp->last_status) == SIGTRAP)
			   || (lwp->status_pending_p
			       && WIFSTOPPED (lwp->status_pending)
			       && WSTOPSIG (lwp->status_pending) == SIGTRAP)))
		{
		  /* The LWP finished a hardware single-step; report
		     the SIGTRAP to GDB.  */
		  lwp->pending_is_breakpoint = 0;
		  lwp->status_pending_p = 0;
		  lwp->status_pending = 0;
		  lwp->last_status = W_STOPCODE (SIGTRAP);
		  /* Report the finished single-step.  When using
		     displaced stepping, GDB needs this to be able to
		     fixup the PC.  */
		  status.value.sig = TARGET_SIGNAL_TRAP;
		}
	      else
		/* Leave other signals pending.  */
		status.value.sig = 0;

	      /* Pass the stop reply back to GDB.  */
	      push_event (ptid, &status);
	    }
	  else
	    ;
	}
      else
	{
	  if (debug_threads)
	    {
	      if (lwp->suspended)
		fprintf (stderr, "already stopped/suspended %s\n",
			 target_pid_to_str (entry->id));
	      else
		fprintf (stderr, "already stopped/not suspended %s\n",
			 target_pid_to_str (entry->id));
	    }

	  /* Make sure we leave the LWP suspended, so we don't try to
	     resume it without GDB telling us to.  FIXME: The LWP may
	     have been stopped in an internal event that was not meant
	     to be notified back to GDB (e.g., gdbserver breakpoint),
	     so we should be reporting a stop event in that case
	     too.  */
	  lwp->suspended = 1;
	}
      return 0;
    }
  else
    lwp->suspended = 0;

  /* If this thread which is about to be resumed has a pending status,
     then don't resume any threads - we can just report the pending
     status.  Make sure to queue any signals that would otherwise be
     sent.  In all-stop mode, we do this decision based on if *any*
     thread has a pending status.  */
  if (non_stop)
    resume_status_pending_p (&lwp->head, &pending_flag);

  if (!pending_flag)
    {
      if (debug_threads)
	fprintf (stderr, "resuming thread %s\n",
		 target_pid_to_str (entry->id));

      if (ptid_equal (lwp->resume->thread, minus_one_ptid))
	step = lwp->stepping || (lwp->resume->kind == rk_step);
      else
	step = (lwp->resume->kind == rk_step);

      linux_resume_one_lwp (&lwp->head, step, lwp->resume->sig, NULL);
    }
  else
    {
      if (debug_threads)
	fprintf (stderr, "leaving thread %s stopped\n",
		 target_pid_to_str (entry->id));

      /* If we have a new signal, enqueue the signal.  */
      if (lwp->resume->sig != 0)
	{
	  struct pending_signals *p_sig;
	  p_sig = malloc (sizeof (*p_sig));
	  p_sig->prev = lwp->pending_signals;
	  p_sig->signal = lwp->resume->sig;
	  memset (&p_sig->info, 0, sizeof (siginfo_t));

	  /* If this is the same signal we were previously stopped by,
	     make sure to queue its siginfo.  We can ignore the return
	     value of ptrace; if it fails, we'll skip
	     PTRACE_SETSIGINFO.  */
	  if (WIFSTOPPED (lwp->last_status)
	      && WSTOPSIG (lwp->last_status) == lwp->resume->sig)
	    ptrace (PTRACE_GETSIGINFO, ptid_get_lwp (lwp->head.id), 0, &p_sig->info);

	  lwp->pending_signals = p_sig;
	}
    }

  lwp->resume = NULL;
  return 0;
}

static void
linux_resume (struct thread_resume *resume_info, size_t n)
{
  int pending_flag;
  struct resume_info info = { resume_info, n };

  if (debug_threads)
    {
      int idx;
      fprintf (stderr, "linux_resume\n");
      for (idx = 0; idx < n; idx++)
	{
	  fprintf (stderr, " thread(%s), kind(%d), sig(%d)\n",
		   target_pid_to_str (resume_info[idx].thread),
		   resume_info[idx].kind,
		   resume_info[idx].sig);
	  idx++;
	}
    }

  find_inferior (&all_threads, linux_set_resume_request, &info);

  /* If there is a thread which would otherwise be resumed, which
     has a pending status, then don't resume any threads - we can just
     report the pending status.  Make sure to queue any signals
     that would otherwise be sent.  */
  /* This is only used in all-stop mode however.  In non-stop mode,
     decide to leave pending based on the thread we're resuming itself
     having a pending status already or not.  */
  pending_flag = 0;
  if (!non_stop)
    find_inferior (&all_lwps, resume_status_pending_p, &pending_flag);

  if (debug_threads)
    {
      if (pending_flag)
	fprintf (stderr, "Not resuming, pending status\n");
      else
	fprintf (stderr, "Resuming, no pending status\n");
    }

  find_inferior (&all_threads, linux_resume_one_thread, &pending_flag);
}

#ifdef HAVE_LINUX_USRREGS

int
register_addr (int regnum)
{
  int addr;

  if (regnum < 0 || regnum >= the_low_target.num_regs)
    error ("Invalid register number %d.", regnum);

  addr = the_low_target.regmap[regnum];

  return addr;
}

/* Fetch one register.  */
static void
fetch_register (int regno)
{
  CORE_ADDR regaddr;
  int i, size;
  char *buf;

  if (regno >= the_low_target.num_regs)
    return;
  if ((*the_low_target.cannot_fetch_register) (regno))
    return;

  regaddr = register_addr (regno);
  if (regaddr == -1)
    return;
  size = (register_size (regno) + sizeof (PTRACE_XFER_TYPE) - 1)
         & - sizeof (PTRACE_XFER_TYPE);
  buf = alloca (size);
  for (i = 0; i < size; i += sizeof (PTRACE_XFER_TYPE))
    {
      errno = 0;
      *(PTRACE_XFER_TYPE *) (buf + i) =
	ptrace (PTRACE_PEEKUSER, inferior_lwpid, (PTRACE_ARG3_TYPE) regaddr, 0);
      regaddr += sizeof (PTRACE_XFER_TYPE);
      if (errno != 0)
	{
	  /* Warning, not error, in case we are attached; sometimes the
	     kernel doesn't let us at the registers.  */
	  char *err = strerror (errno);
	  char *msg = alloca (strlen (err) + 128);
	  sprintf (msg, "reading register %d: %s", regno, err);
	  error (msg);
	  goto error_exit;
	}
    }

  if (the_low_target.supply_ptrace_register)
    the_low_target.supply_ptrace_register (regno, buf);
  else
    supply_register (regno, buf);

error_exit:;
}

/* Fetch all registers, or just one, from the child lwp.  */
static void
usr_fetch_inferior_registers (int regno)
{
  if (regno == -1 || regno == 0)
    for (regno = 0; regno < the_low_target.num_regs; regno++)
      fetch_register (regno);
  else
    fetch_register (regno);
}

/* Store our register values back into the inferior.
   If REGNO is -1, do this for all registers.
   Otherwise, REGNO specifies which register (so we can save time).  */
static void
usr_store_inferior_registers (int regno)
{
  CORE_ADDR regaddr;
  int i, size;
  char *buf;

  if (regno >= 0)
    {
      if (regno >= the_low_target.num_regs)
	return;

      if ((*the_low_target.cannot_store_register) (regno) == 1)
	return;

      regaddr = register_addr (regno);
      if (regaddr == -1)
	return;
      errno = 0;
      size = (register_size (regno) + sizeof (PTRACE_XFER_TYPE) - 1)
	     & - sizeof (PTRACE_XFER_TYPE);
      buf = alloca (size);
      memset (buf, 0, size);

      if (the_low_target.collect_ptrace_register)
	the_low_target.collect_ptrace_register (regno, buf);
      else
	collect_register (regno, buf);

      for (i = 0; i < size; i += sizeof (PTRACE_XFER_TYPE))
	{
	  errno = 0;
	  ptrace (PTRACE_POKEUSER, inferior_lwpid, (PTRACE_ARG3_TYPE) regaddr,
		  *(PTRACE_XFER_TYPE *) (buf + i));
	  if (errno != 0)
	    {
	      /* At this point, ESRCH should mean the lwp is already gone,
		 in which case we simply ignore attempts to change its registers.
		 See also the related comment in linux_resume_one_lwp.  */
	      if (errno == ESRCH)
		return;

	      if ((*the_low_target.cannot_store_register) (regno) == 0)
		{
		  char *err = strerror (errno);
		  char *msg = alloca (strlen (err) + 128);
		  sprintf (msg, "writing register %d: %s",
			   regno, err);
		  error (msg);
		  return;
		}
	    }
	  regaddr += sizeof (PTRACE_XFER_TYPE);
	}
    }
  else
    for (regno = 0; regno < the_low_target.num_regs; regno++)
      usr_store_inferior_registers (regno);
}
#endif /* HAVE_LINUX_USRREGS */



#ifdef HAVE_LINUX_REGSETS

static int
regsets_fetch_inferior_registers ()
{
  struct regset_info *regset;
  int saw_general_regs = 0;

  regset = target_regsets;

  while (regset->size >= 0)
    {
      void *buf;
      int res;

      if (regset->size == 0 || disabled_regsets[regset - target_regsets])
	{
	  regset ++;
	  continue;
	}

      buf = malloc (regset->size);
      res = ptrace (regset->get_request, inferior_lwpid, 0, buf);
      if (res < 0)
	{
	  if (errno == EIO)
	    {
	      /* If we get EIO on a regset, do not try it again for
		 this lwp.  */
	      disabled_regsets[regset - target_regsets] = 1;
	      continue;
	    }
	  else
	    {
	      char s[256];
	      sprintf (s, "ptrace(regsets_fetch_inferior_registers) PID=%ld",
		       inferior_lwpid);
	      perror (s);
	    }
	}
      else if (regset->type == GENERAL_REGS)
	saw_general_regs = 1;
      regset->store_function (buf);
      regset ++;
    }
  if (saw_general_regs)
    return 0;
  else
    return 1;
}

static int
regsets_store_inferior_registers ()
{
  struct regset_info *regset;
  int saw_general_regs = 0;

  regset = target_regsets;

  while (regset->size >= 0)
    {
      void *buf;
      int res;

      if (regset->size == 0 || disabled_regsets[regset - target_regsets])
	{
	  regset ++;
	  continue;
	}

      buf = malloc (regset->size);

      /* First fill the buffer with the current register set contents,
	 in case there are any items in the kernel's regset that are
	 not in gdbserver's regcache.  */
      res = ptrace (regset->get_request, inferior_lwpid, 0, buf);

      if (res == 0)
	{
	  /* Then overlay our cached registers on that.  */
	  regset->fill_function (buf);

	  /* Only now do we write the register set.  */
	  res = ptrace (regset->set_request, inferior_lwpid, 0, buf);
	}

      if (res < 0)
	{
	  if (errno == EIO)
	    {
	      /* If we get EIO on a regset, do not try it again for
		 this lwp.  */
	      disabled_regsets[regset - target_regsets] = 1;
	      continue;
	    }
	  else if (errno == ESRCH)
	    {
	      /* At this point, ESRCH should mean the lwp is already gone,
		 in which case we simply ignore attempts to change its registers.
		 See also the related comment in linux_resume_one_lwp.  */
	      free (buf);
	      return 0;
	    }
	  else
	    {
	      perror ("Warning: ptrace(regsets_store_inferior_registers)");
	    }
	}
      else if (regset->type == GENERAL_REGS)
	saw_general_regs = 1;
      regset ++;
      free (buf);
    }
  if (saw_general_regs)
    return 0;
  else
    return 1;
  return 0;
}

#endif /* HAVE_LINUX_REGSETS */


void
linux_fetch_registers (int regno)
{
#ifdef HAVE_LINUX_REGSETS
  if (regsets_fetch_inferior_registers () == 0)
    return;
#endif
#ifdef HAVE_LINUX_USRREGS
  usr_fetch_inferior_registers (regno);
#endif
}

void
linux_store_registers (int regno)
{
#ifdef HAVE_LINUX_REGSETS
  if (regsets_store_inferior_registers () == 0)
    return;
#endif
#ifdef HAVE_LINUX_USRREGS
  usr_store_inferior_registers (regno);
#endif
}


/* Copy LEN bytes from inferior's memory starting at MEMADDR
   to debugger memory starting at MYADDR.  */

static int
linux_read_memory (CORE_ADDR memaddr, unsigned char *myaddr, int len)
{
  register int i;
  /* Round starting address down to longword boundary.  */
  register CORE_ADDR addr = memaddr & -(CORE_ADDR) sizeof (PTRACE_XFER_TYPE);
  /* Round ending address up; get number of longwords that makes.  */
  register int count
    = (((memaddr + len) - addr) + sizeof (PTRACE_XFER_TYPE) - 1)
      / sizeof (PTRACE_XFER_TYPE);
  /* Allocate buffer of that many longwords.  */
  register PTRACE_XFER_TYPE *buffer
    = (PTRACE_XFER_TYPE *) alloca (count * sizeof (PTRACE_XFER_TYPE));
  int fd;
  char filename[64];

  /* Try using /proc.  Don't bother for one word.  */
  if (len >= 3 * sizeof (long))
    {
      /* We could keep this file open and cache it - possibly one per
	 thread.  That requires some juggling, but is even faster.  */
      sprintf (filename, "/proc/%ld/mem", inferior_lwpid);
      fd = open (filename, O_RDONLY | O_LARGEFILE);
      if (fd == -1)
	goto no_proc;

      /* If pread64 is available, use it.  It's faster if the kernel
	 supports it (only one syscall), and it's 64-bit safe even on
	 32-bit platforms (for instance, SPARC debugging a SPARC64
	 application).  */
#ifdef HAVE_PREAD64
      if (pread64 (fd, myaddr, len, memaddr) != len)
#else
      if (lseek (fd, memaddr, SEEK_SET) == -1 || read (fd, memaddr, len) != len)
#endif
	{
	  close (fd);
	  goto no_proc;
	}

      close (fd);
      return 0;
    }

 no_proc:
  /* Read all the longwords */
  for (i = 0; i < count; i++, addr += sizeof (PTRACE_XFER_TYPE))
    {
      errno = 0;
      buffer[i] = ptrace (PTRACE_PEEKTEXT, inferior_lwpid, (PTRACE_ARG3_TYPE) addr, 0);
      if (errno)
	return errno;
    }

  /* Copy appropriate bytes out of the buffer.  */
  memcpy (myaddr, (char *) buffer + (memaddr & (sizeof (PTRACE_XFER_TYPE) - 1)), len);

  return 0;
}

/* Copy LEN bytes of data from debugger memory at MYADDR
   to inferior's memory at MEMADDR.
   On failure (cannot write the inferior)
   returns the value of errno.  */

static int
linux_write_memory (CORE_ADDR memaddr, const unsigned char *myaddr, int len)
{
  register int i;
  /* Round starting address down to longword boundary.  */
  register CORE_ADDR addr = memaddr & -(CORE_ADDR) sizeof (PTRACE_XFER_TYPE);
  /* Round ending address up; get number of longwords that makes.  */
  register int count
  = (((memaddr + len) - addr) + sizeof (PTRACE_XFER_TYPE) - 1) / sizeof (PTRACE_XFER_TYPE);
  /* Allocate buffer of that many longwords.  */
  register PTRACE_XFER_TYPE *buffer = (PTRACE_XFER_TYPE *) alloca (count * sizeof (PTRACE_XFER_TYPE));

  if (debug_threads)
    {
      fprintf (stderr, "Writing %02x to %08lx\n", (unsigned)myaddr[0], (long)memaddr);
    }

  /* Fill start and end extra bytes of buffer with existing memory data.  */

  buffer[0] = ptrace (PTRACE_PEEKTEXT, inferior_lwpid,
		      (PTRACE_ARG3_TYPE) addr, 0);

  if (count > 1)
    {
      buffer[count - 1]
	= ptrace (PTRACE_PEEKTEXT, inferior_lwpid,
		  (PTRACE_ARG3_TYPE) (addr + (count - 1)
				      * sizeof (PTRACE_XFER_TYPE)),
		  0);
    }

  /* Copy data to be written over corresponding part of buffer */

  memcpy ((char *) buffer + (memaddr & (sizeof (PTRACE_XFER_TYPE) - 1)), myaddr, len);

  /* Write the entire buffer.  */

  for (i = 0; i < count; i++, addr += sizeof (PTRACE_XFER_TYPE))
    {
      errno = 0;
      ptrace (PTRACE_POKETEXT, inferior_lwpid, (PTRACE_ARG3_TYPE) addr, buffer[i]);
      if (errno)
	return errno;
    }

  return 0;
}

/* Helper functions for linux_test_for_tracefork, called via clone ().  */

static int
linux_tracefork_grandchild (void *arg)
{
  _exit (0);
}

#define STACK_SIZE 4096

static int
linux_tracefork_child (void *arg)
{
  ptrace (PTRACE_TRACEME, 0, 0, 0);
  kill (getpid (), SIGSTOP);
#ifdef __ia64__
  __clone2 (linux_tracefork_grandchild, arg, STACK_SIZE,
	    CLONE_VM | SIGCHLD, NULL);
#else
  clone (linux_tracefork_grandchild, arg + STACK_SIZE,
	 CLONE_VM | SIGCHLD, NULL);
#endif
  _exit (0);
}

/* Determine if PTRACE_O_TRACEFORK can be used to follow fork events.  Make
   sure that we can enable the option, and that it had the desired
   effect.  */

static void
linux_test_for_tracefork (void)
{
  int child_pid, ret, status;
  long second_pid;
  char *stack = malloc (STACK_SIZE * 4);

  linux_supports_tracefork_flag = 0;

  /* Use CLONE_VM instead of fork, to support uClinux (no MMU).  */
#ifdef __ia64__
  child_pid = __clone2 (linux_tracefork_child, stack, STACK_SIZE,
			CLONE_VM | SIGCHLD, stack + STACK_SIZE * 2);
#else
  child_pid = clone (linux_tracefork_child, stack + STACK_SIZE,
		     CLONE_VM | SIGCHLD, stack + STACK_SIZE * 2);
#endif
  if (child_pid == -1)
    perror_with_name ("clone");

  ret = my_waitpid (child_pid, &status, 0);
  if (ret == -1)
    perror_with_name ("waitpid");
  else if (ret != child_pid)
    error ("linux_test_for_tracefork: waitpid: unexpected result %d.", ret);
  if (! WIFSTOPPED (status))
    error ("linux_test_for_tracefork: waitpid: unexpected status %d.", status);

  ret = ptrace (PTRACE_SETOPTIONS, child_pid, 0, PTRACE_O_TRACEFORK);
  if (ret != 0)
    {
      ret = ptrace (PTRACE_KILL, child_pid, 0, 0);
      if (ret != 0)
	{
	  warning ("linux_test_for_tracefork: failed to kill child");
	  return;
	}

      ret = my_waitpid (child_pid, &status, 0);
      if (ret != child_pid)
	warning ("linux_test_for_tracefork: failed to wait for killed child");
      else if (!WIFSIGNALED (status))
	warning ("linux_test_for_tracefork: unexpected wait status 0x%x from "
		 "killed child", status);

      return;
    }

  ret = ptrace (PTRACE_CONT, child_pid, 0, 0);
  if (ret != 0)
    warning ("linux_test_for_tracefork: failed to resume child");

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
	    warning ("linux_test_for_tracefork: failed to kill second child");
	  my_waitpid (second_pid, &status, 0);
	}
    }
  else
    warning ("linux_test_for_tracefork: unexpected result from waitpid "
	     "(%d, status 0x%x)", ret, status);

  do
    {
      ret = ptrace (PTRACE_KILL, child_pid, 0, 0);
      if (ret != 0)
	warning ("linux_test_for_tracefork: failed to kill child");
      my_waitpid (child_pid, &status, 0);
    }
  while (WIFSTOPPED (status));

  free (stack);
}


static void
linux_look_up_symbols (void)
{
#ifdef USE_THREAD_DB
  /* It would probably be cleaner now to move this whole function to
     thread-db.c.  */

  struct process_info *proc = current_process ();
  if (proc->private->thread_db_active)
    return;

  proc->private->thread_db_active
    = thread_db_init (!linux_supports_tracefork_flag);
#endif
}

static void
linux_request_interrupt (void)
{
  extern unsigned long signal_pid;

  if (!ptid_equal (cont_thread, null_ptid)
      && !ptid_equal (cont_thread, minus_one_ptid))
    {
      struct lwp_info *lwp;
      int lwpid;

      lwp = get_thread_lwp (current_inferior);
      lwpid = ptid_get_lwp (lwp->head.id);
      kill_lwp (lwpid, SIGINT);
    }
  else
    kill_lwp (signal_pid, SIGINT);
}

/* Copy LEN bytes from inferior's auxiliary vector starting at OFFSET
   to debugger memory starting at MYADDR.  */

static int
linux_read_auxv (CORE_ADDR offset, unsigned char *myaddr, unsigned int len)
{
  char filename[PATH_MAX];
  int fd, n;

  snprintf (filename, sizeof filename, "/proc/%ld/auxv", inferior_lwpid);

  fd = open (filename, O_RDONLY);
  if (fd < 0)
    return -1;

  if (offset != (CORE_ADDR) 0
      && lseek (fd, (off_t) offset, SEEK_SET) != (off_t) offset)
    n = -1;
  else
    n = read (fd, myaddr, len);

  close (fd);

  return n;
}

/* These watchpoint related wrapper functions simply pass on the function call
   if the target has registered a corresponding function.  */

static int
linux_insert_watchpoint (char type, CORE_ADDR addr, int len)
{
  if (the_low_target.insert_watchpoint != NULL)
    return the_low_target.insert_watchpoint (type, addr, len);
  else
    /* Unsupported (see target.h).  */
    return 1;
}

static int
linux_remove_watchpoint (char type, CORE_ADDR addr, int len)
{
  if (the_low_target.remove_watchpoint != NULL)
    return the_low_target.remove_watchpoint (type, addr, len);
  else
    /* Unsupported (see target.h).  */
    return 1;
}

static int
linux_stopped_by_watchpoint (void)
{
  if (the_low_target.stopped_by_watchpoint != NULL)
    return the_low_target.stopped_by_watchpoint ();
  else
    return 0;
}

static CORE_ADDR
linux_stopped_data_address (void)
{
  if (the_low_target.stopped_data_address != NULL)
    return the_low_target.stopped_data_address ();
  else
    return 0;
}

#if defined(__UCLIBC__) && defined(HAS_NOMMU)
#if defined(__mcoldfire__)
/* These should really be defined in the kernel's ptrace.h header.  */
#define PT_TEXT_ADDR 49*4
#define PT_DATA_ADDR 50*4
#define PT_TEXT_END_ADDR  51*4
#endif

#if defined(__arm__)
#ifndef PTRACE_GETPROCMAP
#define PTRACE_GETPROCMAP 27
#endif
#endif

/* Under uClinux, programs are loaded at non-zero offsets, which we need
   to tell gdb about.  */

static int
linux_read_offsets (CORE_ADDR *text_p, CORE_ADDR *data_p)
{
#if defined(PT_TEXT_ADDR) && defined(PT_DATA_ADDR) && defined(PT_TEXT_END_ADDR)
  unsigned long text, text_end, data;
  int pid = pid_of (get_thread_lwp (current_inferior));

  errno = 0;

  text = ptrace (PTRACE_PEEKUSER, pid, (long)PT_TEXT_ADDR, 0);
  text_end = ptrace (PTRACE_PEEKUSER, pid, (long)PT_TEXT_END_ADDR, 0);
  data = ptrace (PTRACE_PEEKUSER, pid, (long)PT_DATA_ADDR, 0);

  if (errno == 0)
    {
      /* Both text and data offsets produced at compile-time (and so
         used by gdb) are relative to the beginning of the program,
         with the data segment immediately following the text segment.
         However, the actual runtime layout in memory may put the data
         somewhere else, so when we send gdb a data base-address, we
         use the real data base address and subtract the compile-time
         data base-address from it (which is just the length of the
         text segment).  BSS immediately follows data in both
         cases.  */
      *text_p = text;
      *data_p = data - (text_end - text);
      
      return 1;
    }
#endif
#if defined(PTRACE_GETPROCMAP)
  unsigned long data[6];
  int pid = pid_of (get_thread_lwp (current_inferior));

  if (ptrace (PTRACE_GETPROCMAP, pid, NULL, data) == 0)
    {
      /* GETPROCMAP returns text_start, text_end, data_start.  See above.  */
      *text_p = data[0];
      *data_p = data[2] - (data[1] - data[0]);
      return 1;
    }
#endif
 return 0;
}
#endif

/* SIGCHLD handler that serves two purposes: In non-stop/async mode,
   so we notice when any child changes state; as the handler for the
   sigsuspend in my_waitpid.  */

static void
sigchld_handler (int signo)
{
  int old_errno = errno;

  if (debug_threads)
    /* fprintf is not an async-signal-safe function, so call write
       directly.  */
    write (2, "sigchld_handler\n",
	   strlen ("sigchld_handler\n"));

  if (target_is_async_p ())
    async_file_mark (); /* trigger a linux_wait */

  errno = old_errno;
}

static int
linux_async (int enable)
{
  int previous = (linux_event_pipe[0] != -1);

  if (previous != enable)
    {
      sigset_t mask;
      sigemptyset (&mask);
      sigaddset (&mask, SIGCHLD);

      sigprocmask (SIG_BLOCK, &mask, NULL);

      if (enable)
	{
	  if (pipe (linux_event_pipe) == -1)
	    internal_error ("creating event pipe failed.");

	  fcntl (linux_event_pipe[0], F_SETFL, O_NONBLOCK);
	  fcntl (linux_event_pipe[1], F_SETFL, O_NONBLOCK);

	  /* Register the event loop handler.  */
	  add_file_handler (linux_event_pipe[0],
			    handle_target_event, NULL);

	  /* Always trigger a linux_wait.  */
	  async_file_mark ();
	}
      else
	{
	  delete_file_handler (linux_event_pipe[0]);

	  close (linux_event_pipe[0]);
	  close (linux_event_pipe[1]);
	  linux_event_pipe[0] = -1;
	  linux_event_pipe[1] = -1;
	}

      sigprocmask (SIG_UNBLOCK, &mask, NULL);
    }

  return previous;
}

static int
linux_start_non_stop (int nonstop)
{
  /* Register or unregister from event-loop accordingly.  */
  linux_async (nonstop);
  return 0;
}

static struct target_ops linux_target_ops = {
  linux_create_inferior,
  linux_attach,
  linux_kill,
  linux_detach,
  linux_join,
  linux_thread_alive,
  linux_resume,
  linux_wait,
  linux_fetch_registers,
  linux_store_registers,
  linux_read_memory,
  linux_write_memory,
  linux_look_up_symbols,
  linux_request_interrupt,
  linux_read_auxv,
  linux_insert_watchpoint,
  linux_remove_watchpoint,
  linux_stopped_by_watchpoint,
  linux_stopped_data_address,
#if defined(__UCLIBC__) && defined(HAS_NOMMU)
  linux_read_offsets,
#else
  NULL,
#endif
#ifdef USE_THREAD_DB
  thread_db_get_tls_address,
#else
  NULL,
#endif
  NULL,
  hostio_last_error_from_errno,
  linux_pid_to_exec_file,
  linux_async,
  linux_start_non_stop,
};

static void
linux_init_signals ()
{
  /* FIXME drow/2002-06-09: As above, we should check with LinuxThreads
     to find what the cancel signal actually is.  */
  signal (__SIGRTMIN+1, SIG_IGN);
}

void
initialize_low (void)
{
  struct sigaction sigchld_action;

  set_target_ops (&linux_target_ops);
  set_breakpoint_data (the_low_target.breakpoint,
		       the_low_target.breakpoint_len);
  linux_init_signals ();
  linux_test_for_tracefork ();
#ifdef HAVE_LINUX_REGSETS
  for (num_regsets = 0; target_regsets[num_regsets].size >= 0; num_regsets++)
    ;
  disabled_regsets = malloc (num_regsets);
#endif

  sigchld_action.sa_handler = sigchld_handler;
  sigemptyset (&sigchld_action.sa_mask);
  sigchld_action.sa_flags = SA_RESTART;
  sigaction (SIGCHLD, &sigchld_action, NULL);
}
