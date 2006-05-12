/* Async support for GDB, the GNU debugger.
   Copyright 1997, 1998, 1999, 2000, 2001, 2002
   Free Software Foundation, Inc.

   Contributed by Apple Computer, Inc.

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
#include "gdbcmd.h"
#include "event-loop.h"
#include "inferior.h"

#include "async-nat-sigthread.h"
//#include "macosx-nat-inferior.h"
//#include "macosx-nat-mutils.h"

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <sys/select.h>

static FILE *sigthread_stderr_re = NULL;
static int sigthread_debugflag = 0;
static void* gdb_signal_thread (void *arg);

/* A re-entrant version for use by the signal handling thread */

void
sigthread_debug_re (const char *fmt, ...)
{
  va_list ap;
  if (sigthread_debugflag)
    {
      va_start (ap, fmt);
      fprintf (sigthread_stderr_re, "[%d sigthread]: ", getpid ());
      vfprintf (sigthread_stderr_re, fmt, ap);
      va_end (ap);
      fflush (sigthread_stderr_re);
    }
}

void
gdb_signal_thread_init (gdb_signal_thread_status *s)
{
  s->transmit_fd = -1;
  s->receive_fd = -1;

  s->inferior_pid = -1;
  s->signal_thread = 0;
}

void
gdb_signal_thread_create (gdb_signal_thread_status *s, int pid)
{
  int fd[2];
  int ret;

  ret = pipe (fd);
  //  CHECK_FATAL (ret == 0);

  s->transmit_fd = fd[1];
  s->receive_fd = fd[0];

  s->inferior_pid = pid;

  s->signal_thread =
    gdb_thread_fork (gdb_signal_thread, s);
}

void
gdb_signal_thread_destroy (gdb_signal_thread_status *s)
{
  if (s->signal_thread != 0)
    {
      gdb_thread_kill (s->signal_thread);
    }

  if (s->receive_fd > 0)
    {
      delete_file_handler (s->receive_fd);
      close (s->receive_fd);
    }
  if (s->transmit_fd > 0)
    close (s->transmit_fd);

  gdb_signal_thread_init (s);
}

void
gdb_signal_thread_debug (FILE *f, gdb_signal_thread_status *s)
{
  fprintf (f, "                [SIGNAL THREAD]\n");
}

void
gdb_signal_thread_debug_status (FILE *f, WAITSTATUS status)
{
  if (WIFEXITED (status))
    {
      fprintf (f, "process exited with status %d\n", WEXITSTATUS (status));
    }
  else if (WIFSIGNALED (status))
    {
      fprintf (f, "process terminated with signal %d (%s)\n",
               WTERMSIG (status),
               target_signal_to_string (WTERMSIG (status)));
    }
  else if (WIFSTOPPED (status))
    {
      fprintf (f, "process stopped with signal %d (%s)\n", WSTOPSIG (status),
               target_signal_to_string (WSTOPSIG (status)));
    }
  else
    {
      fprintf (f, "unknown status value %d\n", status);
    }
}

static void*
gdb_signal_thread (void *arg)
{
  gdb_signal_thread_status *s = (gdb_signal_thread_status *) arg;
  //  CHECK_FATAL (s != NULL);

  for (;;)
    {

      gdb_signal_thread_message msg;
      WAITSTATUS status = 0;
      pid_t pid = 0;

      pthread_testcancel ();

      sigthread_debug_re
        ("gdb_signal_thread: waiting for events for pid %d\n",
         s->inferior_pid);

      pid = waitpid (s->inferior_pid, &status, 0);

      sigthread_debug_re
        ("gdb_signal_thread: received event for pid %d\n",
         s->inferior_pid);

      if ((pid < 0) && (errno == ECHILD))
        {
          sigthread_debug_re
            ("gdb_signal_thread: no children present; waiting for parent\n");
          for (;;)
            {
              pthread_testcancel ();
              sched_yield ();
            }
        }

      if ((pid < 0) && (errno == EINTR))
        {
          sigthread_debug_re
            ("gdb_signal_thread: wait interrupted; continuing\n");
          continue;
        }

      if (pid < 0)
        {
          fprintf (sigthread_stderr_re,
                   "gdb_signal_thread: unexpected error: %s\n",
                   strerror (errno));
          abort ();
        }

      if (sigthread_debugflag)
        {
          sigthread_debug_re ("gdb_signal_thread: received event for pid %d: ", pid);
          gdb_signal_thread_debug_status (sigthread_stderr_re, status);
        }

      if (pid != s->inferior_pid)
        {
          fprintf (sigthread_stderr_re,
                   "gdb_signal_thread: event was for unexpected pid (got %d, was expecting %d)\n",
                   pid, s->inferior_pid);
          abort ();
        }

      msg.pid = pid;
      msg.status = status;
      write (s->transmit_fd, &msg, sizeof (msg));
    }
}

void
gdb_pthread_kill (pthread_t pthread)
{
  int ret;

  ret = pthread_cancel (pthread);
  if (ret != 0)
    {
      warning ("Unable to cancel thread: %s (%d)", strerror (errno), errno);
    }
}

pthread_t
gdb_pthread_fork (pthread_fn_t function, void *arg)
{
  int result;
  pthread_t pthread = 0;
  pthread_attr_t attr;

  result = pthread_attr_init (&attr);
  if (result != 0)
    {
      error ("Unable to initialize thread attributes: %s (%d)",
             strerror (errno), errno);
    }

  result = pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_JOINABLE);
  if (result != 0)
    {
      error ("Unable to initialize thread attributes: %s (%d)",
             strerror (errno), errno);
    }

  result = pthread_create (&pthread, &attr, function, arg);
  if (result != 0)
    {
      error ("Unable to create thread: %s (%d)", strerror (errno), errno);
    }

  result = pthread_attr_destroy (&attr);
  if (result != 0)
    {
      warning ("Unable to deallocate thread attributes: %s (%d)",
               strerror (errno), errno);
    }

  return pthread;
}

void
_initialize_gdb_nat_sigthread ()
{
  sigthread_stderr_re = fdopen (fileno (stderr), "w+");

  add_setshow_boolean_cmd ("signals", no_class,
			   &sigthread_debugflag, _("\
Set if printing signal thread debugging statements."), _("\
Show if printing signal thread debugging statements."), NULL,
			   NULL, NULL,
			   &setdebuglist, &showdebuglist);
}
