/* GNU/Linux native-dependent code common to multiple platforms.
   Copyright (C) 2003 Free Software Foundation, Inc.

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
#include "inferior.h"
#include "target.h"

#include "gdb_wait.h"
#include <sys/ptrace.h>

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

/* Wait extended result codes for the above trace options.  */
#define PTRACE_EVENT_FORK	1
#define PTRACE_EVENT_VFORK	2
#define PTRACE_EVENT_CLONE	3
#define PTRACE_EVENT_EXEC	4

#endif /* PTRACE_EVENT_FORK */

/* We can't always assume that this flag is available, but all systems
   with the ptrace event handlers also have __WALL, so it's safe to use
   here.  */
#ifndef __WALL
#define __WALL          0x40000000 /* Wait for any child.  */
#endif

struct simple_pid_list
{
  int pid;
  struct simple_pid_list *next;
};
struct simple_pid_list *stopped_pids;

/* This variable is a tri-state flag: -1 for unknown, 0 if PTRACE_O_TRACEFORK
   can not be used, 1 if it can.  */

static int linux_supports_tracefork_flag = -1;


/* Trivial list manipulation functions to keep track of a list of
   new stopped processes.  */
static void
add_to_pid_list (struct simple_pid_list **listp, int pid)
{
  struct simple_pid_list *new_pid = xmalloc (sizeof (struct simple_pid_list));
  new_pid->pid = pid;
  new_pid->next = *listp;
  *listp = new_pid;
}

static int
pull_pid_from_list (struct simple_pid_list **listp, int pid)
{
  struct simple_pid_list **p;

  for (p = listp; *p != NULL; p = &(*p)->next)
    if ((*p)->pid == pid)
      {
	struct simple_pid_list *next = (*p)->next;
	xfree (*p);
	*p = next;
	return 1;
      }
  return 0;
}

void
linux_record_stopped_pid (int pid)
{
  add_to_pid_list (&stopped_pids, pid);
}


/* A helper function for linux_test_for_tracefork, called after fork ().  */

static void
linux_tracefork_child (void)
{
  int ret;

  ptrace (PTRACE_TRACEME, 0, 0, 0);
  kill (getpid (), SIGSTOP);
  fork ();
  exit (0);
}

/* Determine if PTRACE_O_TRACEFORK can be used to follow fork events.  We
   create a child process, attach to it, use PTRACE_SETOPTIONS to enable
   fork tracing, and let it fork.  If the process exits, we assume that
   we can't use TRACEFORK; if we get the fork notification, and we can
   extract the new child's PID, then we assume that we can.  */

static void
linux_test_for_tracefork (void)
{
  int child_pid, ret, status;
  long second_pid;

  child_pid = fork ();
  if (child_pid == -1)
    perror_with_name ("linux_test_for_tracefork: fork");

  if (child_pid == 0)
    linux_tracefork_child ();

  ret = waitpid (child_pid, &status, 0);
  if (ret == -1)
    perror_with_name ("linux_test_for_tracefork: waitpid");
  else if (ret != child_pid)
    error ("linux_test_for_tracefork: waitpid: unexpected result %d.", ret);
  if (! WIFSTOPPED (status))
    error ("linux_test_for_tracefork: waitpid: unexpected status %d.", status);

  linux_supports_tracefork_flag = 0;

  ret = ptrace (PTRACE_SETOPTIONS, child_pid, 0, PTRACE_O_TRACEFORK);
  if (ret != 0)
    {
      ptrace (PTRACE_KILL, child_pid, 0, 0);
      waitpid (child_pid, &status, 0);
      return;
    }

  ptrace (PTRACE_CONT, child_pid, 0, 0);
  ret = waitpid (child_pid, &status, 0);
  if (ret == child_pid && WIFSTOPPED (status)
      && status >> 16 == PTRACE_EVENT_FORK)
    {
      second_pid = 0;
      ret = ptrace (PTRACE_GETEVENTMSG, child_pid, 0, &second_pid);
      if (ret == 0 && second_pid != 0)
	{
	  int second_status;

	  linux_supports_tracefork_flag = 1;
	  waitpid (second_pid, &second_status, 0);
	  ptrace (PTRACE_DETACH, second_pid, 0, 0);
	}
    }

  if (WIFSTOPPED (status))
    {
      ptrace (PTRACE_DETACH, child_pid, 0, 0);
      waitpid (child_pid, &status, 0);
    }
}

/* Return non-zero iff we have tracefork functionality available.
   This function also sets linux_supports_tracefork_flag.  */

static int
linux_supports_tracefork (void)
{
  if (linux_supports_tracefork_flag == -1)
    linux_test_for_tracefork ();
  return linux_supports_tracefork_flag;
}


int
child_insert_fork_catchpoint (int pid)
{
  if (linux_supports_tracefork ())
    error ("Fork catchpoints have not been implemented yet.");
  else
    error ("Your system does not support fork catchpoints.");
}

int
child_insert_vfork_catchpoint (int pid)
{
  if (linux_supports_tracefork ())
    error ("Vfork catchpoints have not been implemented yet.");
  else
    error ("Your system does not support vfork catchpoints.");
}

int
child_insert_exec_catchpoint (int pid)
{
  if (linux_supports_tracefork ())
    error ("Exec catchpoints have not been implemented yet.");
  else
    error ("Your system does not support exec catchpoints.");
}


