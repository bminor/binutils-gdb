/* Low-level child interface to ttrace.

   Copyright 2004 Free Software Foundation, Inc.

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

/* The ttrace(2) system call didn't exist before HP-UX 10.30.  Don't
   try to compile this code unless we have it.  */
#ifdef HAVE_TTRACE

#include "command.h"
#include "gdbcore.h"
#include "inferior.h"
#include "observer.h"
#include "target.h"

#include "gdb_assert.h"
#include "gdb_string.h"
#include <sys/ttrace.h>

#include "inf-child.h"
#include "inf-ttrace.h"

/* HACK: Save the ttrace ops returned by inf_ttrace_target.  */
static struct target_ops *ttrace_ops_hack;

/* File descriptors for pipes used as semaphores during initial
   startup of an inferior.  */
static int inf_ttrace_pfd1[2];
static int inf_ttrace_pfd2[2];

static void
do_cleanup_pfds (void *dummy)
{
  close (inf_ttrace_pfd1[0]);
  close (inf_ttrace_pfd1[1]);
  close (inf_ttrace_pfd2[0]);
  close (inf_ttrace_pfd2[1]);
}

static void
inf_ttrace_prepare (void)
{
  if (pipe (inf_ttrace_pfd1) == -1)
    perror_with_name ("pipe");

  if (pipe (inf_ttrace_pfd2) == -1)
    {
      close (inf_ttrace_pfd1[0]);
      close (inf_ttrace_pfd2[0]);
      perror_with_name ("pipe");
    }
}

/* Prepare to be traced.  */

static void
inf_ttrace_me (void)
{
  struct cleanup *old_chain = make_cleanup (do_cleanup_pfds, 0);
  char c;

  /* "Trace me, Dr. Memory!"  */
  if (ttrace (TT_PROC_SETTRC, 0, 0, 0, TT_VERSION, 0) == -1)
    perror_with_name ("ttrace");

  /* Tell our parent that we are ready to be traced.  */
  if (write (inf_ttrace_pfd1[1], &c, sizeof c) != sizeof c)
    perror_with_name ("write");

  /* Wait until our parent has set the initial event mask.  */
  if (read (inf_ttrace_pfd2[0], &c, sizeof c) != sizeof c)
    perror_with_name ("read");

  do_cleanups (old_chain);
}

/* Start tracing PID.  */

static void
inf_ttrace_him (int pid)
{
  struct cleanup *old_chain = make_cleanup (do_cleanup_pfds, 0);
  ttevent_t tte;
  ttstate_t tts;
  char c;

  /* Wait until our child is ready to be traced.  */
  if (read (inf_ttrace_pfd1[0], &c, sizeof c) != sizeof c)
    perror_with_name ("read");

  /* Set the initial event mask.  */
  memset (&tte, 0, sizeof (tte));
  tte.tte_events = TTEVT_EXEC | TTEVT_EXIT;
  tte.tte_opts = TTEO_NOSTRCCHLD;
  if (ttrace (TT_PROC_SET_EVENT_MASK, pid, 0,
	      (uintptr_t)&tte, sizeof tte, 0) == -1)
    perror_with_name ("ttrace");

  /* Tell our child that we have set the initial event mask.  */
  if (write (inf_ttrace_pfd2[1], &c, sizeof c) != sizeof c)
    perror_with_name ("write");

  do_cleanups (old_chain);

  push_target (ttrace_ops_hack);

  /* On some targets, there must be some explicit synchronization
     between the parent and child processes after the debugger forks,
     and before the child execs the debuggee program.  This call
     basically gives permission for the child to exec.  */

  target_acknowledge_created_inferior (pid);

  /* START_INFERIOR_TRAPS_EXPECTED is defined in inferior.h, and will
     be 1 or 2 depending on whether we're starting without or with a
     shell.  */
  startup_inferior (START_INFERIOR_TRAPS_EXPECTED);

  /* On some targets, there must be some explicit actions taken after
     the inferior has been started up.  */
  target_post_startup_inferior (pid_to_ptid (pid));
}

static void
inf_ttrace_create_inferior (char *exec_file, char *allargs, char **env,
			    int from_tty)
{
  fork_inferior (exec_file, allargs, env, inf_ttrace_me, inf_ttrace_him,
		 inf_ttrace_prepare, NULL);

  /* We are at the first instruction we care about.  */
  observer_notify_inferior_created (&current_target, from_tty);

  /* Pedal to the metal...  */
  proceed ((CORE_ADDR) -1, TARGET_SIGNAL_0, 0);
}

static void
inf_ttrace_kill_inferior (void)
{
  pid_t pid = ptid_get_pid (inferior_ptid);

  if (pid == 0)
    return;

  if (ttrace (TT_PROC_EXIT, pid, 0, 0, 0, 0) == -1)
    perror_with_name ("ttrace");
  /* ??? Is it necessary to call ttrace_wait() here?  */
  target_mourn_inferior ();
}

static void
inf_ttrace_mourn_inferior (void)
{
  unpush_target (ttrace_ops_hack);
  generic_mourn_inferior ();
}

static void
inf_ttrace_attach (char *args, int from_tty)
{
  char *exec_file;
  pid_t pid;
  char *dummy;

  if (!args)
    error_no_arg ("process-id to attach");

  dummy = args;
  pid = strtol (args, &dummy, 0);
  if (pid == 0 && args == dummy)
    error ("Illegal process-id: %s\n", args);

  if (pid == getpid ())		/* Trying to masturbate?  */
    error ("I refuse to debug myself!");

  if (from_tty)
    {
      exec_file = (char *) get_exec_file (0);

      if (exec_file)
	printf_unfiltered ("Attaching to program: %s, %s\n", exec_file,
			   target_pid_to_str (pid_to_ptid (pid)));
      else
	printf_unfiltered ("Attaching to %s\n",
			   target_pid_to_str (pid_to_ptid (pid)));

      gdb_flush (gdb_stdout);
    }

  if (ttrace (TT_PROC_ATTACH, pid, 0, TT_KILL_ON_EXIT, TT_VERSION, 0) == -1)
    perror_with_name ("ttrace");
  attach_flag = 1;

  inferior_ptid = pid_to_ptid (pid);
  push_target (ttrace_ops_hack);

  /* Do this first, before anything has had a chance to query the
     inferior's symbol table or similar.  */
  observer_notify_inferior_created (&current_target, from_tty);
}

static void
inf_ttrace_detach (char *args, int from_tty)
{
  int sig = 0;
  pid_t pid = ptid_get_pid (inferior_ptid);

  if (from_tty)
    {
      char *exec_file = get_exec_file (0);
      if (exec_file == 0)
	exec_file = "";
      printf_unfiltered ("Detaching from program: %s, %s\n", exec_file,
			 target_pid_to_str (pid_to_ptid (pid)));
      gdb_flush (gdb_stdout);
    }
  if (args)
    sig = atoi (args);

  /* ??? The HP-UX 11.0 ttrace(2) manual page doesn't mention that we
     can pass a signal number here.  Does this really work?  */
  if (ttrace (TT_PROC_DETACH, pid, 0, 0, sig, 0) == -1)
    perror_with_name ("ttrace");

  inferior_ptid = null_ptid;
  unpush_target (ttrace_ops_hack);
}

static void
inf_ttrace_resume (ptid_t ptid, int step, enum target_signal signal)
{
  pid_t pid = ptid_get_pid (ptid);
  lwpid_t lwpid = ptid_get_lwp (ptid);
  ttreq_t request = step ? TT_LWP_SINGLE : TT_LWP_CONTINUE;
  int sig = target_signal_to_host (signal);

  if (pid == -1)
    {
      pid = ptid_get_pid (inferior_ptid);
      lwpid = ptid_get_lwp (inferior_ptid);
    }

  if (ttrace (request, pid, lwpid, TT_NOPC, sig, 0) == -1)
    perror_with_name ("ttrace");

  if (ptid_equal (ptid, minus_one_ptid))
    {
      /* Let all the other threads run too.  */
      if (ttrace (TT_PROC_CONTINUE, pid, 0, 0, 0, 0) == -1)
	perror_with_name ("ttrace");
    }
}

static ptid_t
inf_ttrace_wait (ptid_t ptid, struct target_waitstatus *ourstatus)
{
  pid_t pid = ptid_get_pid (ptid);
  lwpid_t lwpid = ptid_get_lwp (ptid);
  ttstate_t tts;

  ourstatus->kind = TARGET_WAITKIND_IGNORE;

  if (pid == -1)
    pid = 0;

  gdb_assert (lwpid == 0 || pid != 0);

  do
    {
      set_sigint_trap ();
      set_sigio_trap ();

      if (ttrace_wait (pid, lwpid, TTRACE_WAITOK, &tts, sizeof tts) == -1)
	perror_with_name ("ttrace_wait");

      clear_sigio_trap ();
      clear_sigint_trap ();
    }
  while (tts.tts_event == TTEVT_NONE);

  switch (tts.tts_event)
    {
    case TTEVT_EXEC:
      /* Make it look like a breakpoint.  */
      ourstatus->kind = TARGET_WAITKIND_STOPPED;
      ourstatus->value.sig = TARGET_SIGNAL_TRAP;
      break;
    case TTEVT_EXIT:
      store_waitstatus (ourstatus, tts.tts_u.tts_exit.tts_exitcode);
      break;
    case TTEVT_SIGNAL:
      ourstatus->kind = TARGET_WAITKIND_STOPPED;
      ourstatus->value.sig =
	target_signal_from_host (tts.tts_u.tts_signal.tts_signo);
      break;
    }

  /* Make sure all threads within the process are stopped.  */
  if (ttrace (TT_PROC_STOP, tts.tts_pid, 0, 0, 0, 0) == -1)
    perror_with_name ("ttrace");

  /* HACK: Twiddle INFERIOR_PTID such that the initial thread of a
     process isn't recognized as a new thread.  */
  if (ptid_get_lwp (inferior_ptid) == 0)
    inferior_ptid = ptid_build (tts.tts_pid, tts.tts_lwpid, tts.tts_user_tid);

  return ptid_build (tts.tts_pid, tts.tts_lwpid, tts.tts_user_tid);
}

/* Transfer LEN bytes from ADDR in the inferior's memory into READBUF,
   and transfer LEN bytes from WRITEBUF into the inferior's memory at
   ADDR.  Either READBUF or WRITEBUF may be null, in which case the
   corresponding transfer doesn't happen.  Return the number of bytes
   actually transferred (which may be zero if an error occurs).  */

static LONGEST
inf_ttrace_xfer_memory (CORE_ADDR addr, ULONGEST len,
			void *readbuf, const void *writebuf)
{
  pid_t pid = ptid_get_pid (inferior_ptid);

  /* HP-UX treats text space and data space differently.  GDB however,
     doesn't really know the difference.  Therefore we try both.  Try
     text space before data space though because when we're writing
     into text space the instruction cache might need to be flushed.  */

  if (readbuf
      && ttrace (TT_PROC_RDTEXT, pid, 0, addr, len, (uintptr_t)readbuf) == -1
      && ttrace (TT_PROC_RDDATA, pid, 0, addr, len, (uintptr_t)readbuf) == -1)
    return 0;

  if (writebuf
      && ttrace (TT_PROC_WRTEXT, pid, 0, addr, len, (uintptr_t)writebuf) == -1
      && ttrace (TT_PROC_WRDATA, pid, 0, addr, len, (uintptr_t)writebuf) == -1)
    return 0;

  return len;
}

static LONGEST
inf_ttrace_xfer_partial (struct target_ops *ops, enum target_object object,
			 const char *annex, void *readbuf,
			 const void *writebuf, ULONGEST offset, LONGEST len)
{
  switch (object)
    {
    case TARGET_OBJECT_MEMORY:
      return inf_ttrace_xfer_memory (offset, len, readbuf, writebuf);

    case TARGET_OBJECT_UNWIND_TABLE:
      return -1;

    case TARGET_OBJECT_AUXV:
      return -1;

    case TARGET_OBJECT_WCOOKIE:
      return -1;

    default:
      return -1;
    }
}

/* Print status information about what we're accessing.  */

static void
inf_ttrace_files_info (struct target_ops *ignore)
{
  printf_unfiltered ("\tUsing the running image of %s %s.\n",
		     attach_flag ? "attached" : "child",
		     target_pid_to_str (inferior_ptid));
}


struct target_ops *
inf_ttrace_target (void)
{
  struct target_ops *t = inf_child_target ();

  t->to_create_inferior = inf_ttrace_create_inferior;
  t->to_kill = inf_ttrace_kill_inferior;
  t->to_mourn_inferior = inf_ttrace_mourn_inferior;
  t->to_attach = inf_ttrace_attach;
  t->to_detach = inf_ttrace_detach;
  t->to_resume = inf_ttrace_resume;
  t->to_wait = inf_ttrace_wait;
  t->to_xfer_partial = inf_ttrace_xfer_partial;
  t->to_files_info = inf_ttrace_files_info;

  ttrace_ops_hack = t;
  return t;
}

#endif
