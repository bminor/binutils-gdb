/* for separate threads within the inferior process, for GDB.
   Copyright (C) 1986, 1987, 1988 Free Software Foundation, Inc.

GDB is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY.  No author or distributor accepts responsibility to anyone
for the consequences of using it or for whether it serves any
particular purpose or works at all, unless he says so in writing.
Refer to the GDB General Public License for full details.

Everyone is granted permission to copy, modify and redistribute GDB,
but only under the conditions described in the GDB General Public
License.  A copy of this license is supposed to have been given to you
along with GDB so you can know your rights and responsibilities.  It
should be in a file named COPYING.  Among other things, the copyright
notice and this notice must be preserved on all copies.

In other words, go ahead and share GDB, but don't try to stop
anyone else from sharing it farther.  Help stamp out software hoarding!
*/

#include "defs.h"
#include "symtab.h"
#include "frame.h"
#include "inferior.h"
#include "environ.h"
#include "value.h"
#include "target.h"
#include "thread.h"

#include <sys/types.h>
#include <signal.h>

/*#include "lynxos-core.h"*/

struct thread_info
{
  struct thread_info *next;
  int pid;			/* Actual process id */
  int num;			/* Convenient handle */
};

static struct thread_info *thread_list = NULL;
static int highest_thread_num;

static void thread_info PARAMS ((void));

static void thread_command PARAMS ((char * tidstr, int from_tty));

static void prune_threads PARAMS ((void));

static void thread_switch PARAMS ((int pid));

void
init_thread_list ()
{
  struct thread_info *tp, *tpnext;

  if (!thread_list)
    return;

  for (tp = thread_list; tp; tp = tpnext)
    {
      tpnext = tp->next;
      free (tp);
    }

  thread_list = NULL;
  highest_thread_num = 0;
}

void
add_thread (pid)
     int pid;
{
  struct thread_info *tp;

  tp = xmalloc (sizeof (struct thread_info));

  tp->pid = pid;
  tp->num = ++highest_thread_num;
  tp->next = thread_list;
  thread_list = tp;
}

static struct thread_info *
find_thread_id (num)
    int num;
{
  struct thread_info *tp;

  for (tp = thread_list; tp; tp = tp->next)
    if (tp->num == num)
      return tp;

  return NULL;
}

int
in_thread_list (pid)
    int pid;
{
  struct thread_info *tp;

  for (tp = thread_list; tp; tp = tp->next)
    if (tp->pid == pid)
      return 1;

  return 0;			/* Never heard of 'im */
}

#if 0
void
bfd_get_core_threads (abfd)
    bfd *abfd;
{
    int i;

    inferior_pid = BUILDPID (inferior_pid, core_thread (abfd)->pid);
    for (i = 0; i < core_pss (abfd).threadcnt; i++)
      add_thread (core_thread (abfd)[i].pid);
}
#endif

static void
prune_threads ()
{
  struct thread_info *tp, *tpprev;

  tpprev = 0;

  for (tp = thread_list; tp; tp = tp->next)
    if (tp->pid == -1)
      {
 	if (tpprev)
	  tpprev->next = tp->next;
 	else
	  thread_list = NULL;

	free (tp);
      }
    else
      tpprev = tp;
}

/* Print information about currently known threads */

static void
info_threads_command (arg, from_tty)
     char *arg;
     int from_tty;
{
  struct thread_info *tp;
  int current_pid = inferior_pid;

  for (tp = thread_list; tp; tp = tp->next)
    {
      if (target_has_execution
	  && kill (tp->pid, 0) == -1)
 	{
	  tp->pid == -1;	/* Mark it as dead */
	  continue;
 	}

      if (tp->pid == current_pid)
	printf_filtered ("* ");
      else
	printf_filtered ("  ");

      printf_filtered ("%d %s  ", tp->num, target_pid_to_str (tp->pid));

      thread_switch (tp->pid);
      print_stack_frame (selected_frame, -1, 0);
    }

  thread_switch (current_pid);
  prune_threads ();
}

/* Switch from one thread to another. */

void
thread_switch (pid)
     int pid;
{
  if (pid == inferior_pid)
    return;

  inferior_pid = pid;
  pc_changed = 0;
  flush_cached_frames ();
  registers_changed ();
  stop_pc = read_pc();
  set_current_frame (create_new_frame (read_fp (), stop_pc));
  stop_frame_address = FRAME_FP (get_current_frame ());
  select_frame (get_current_frame (), 0);
}

static void
thread_command (tidstr, from_tty)
     char *tidstr;
     int from_tty;
{
  int num;
  struct thread_info *tp;

  if (!tidstr)
    error ("Please specify a thread ID.  Use the \"info threads\" command to\n\
see the IDs of currently known threads.");


  num = atoi (tidstr);

  tp = find_thread_id (num);

  if (!tp)
    error ("Thread ID %d not known.  Use the \"info threads\" command to\n\
see the IDs of currently known threads.", num);

  thread_switch (tp->pid);

  printf_filtered ("[Switching to %s]\n", target_pid_to_str (inferior_pid));
  print_stack_frame (selected_frame, selected_frame_level, 1);
}

void
_initialize_thread ()
{
  add_info ("threads", info_threads_command,
	    "IDs of currently known threads.");
  add_com ("thread", class_info, thread_command,
	   "Use this command to switch between threads.\n\
The new thread ID must be currently known.");
}
