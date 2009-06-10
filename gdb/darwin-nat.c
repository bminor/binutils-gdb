/* Darwin support for GDB, the GNU debugger.
   Copyright (C) 2008, 2009 Free Software Foundation, Inc.

   Contributed by AdaCore.

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
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "defs.h"
#include "top.h"
#include "inferior.h"
#include "target.h"
#include "symfile.h"
#include "symtab.h"
#include "objfiles.h"
#include "gdb.h"
#include "gdbcmd.h"
#include "gdbcore.h"
#include "gdbthread.h"
#include "regcache.h"
#include "event-top.h"
#include "inf-loop.h"
#include "gdb_stat.h"
#include "exceptions.h"
#include "inf-child.h"
#include "value.h"
#include "arch-utils.h"
#include "bfd.h"

#include <sys/ptrace.h>
#include <sys/signal.h>
#include <machine/setjmp.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <ctype.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#include <sys/proc.h>

#include <mach/mach_error.h>
#include <mach/mach_vm.h>
#include <mach/mach_init.h>
#include <mach/vm_map.h>
#include <mach/task.h>
#include <mach/mach_port.h>
#include <mach/thread_act.h>
#include <mach/port.h>

#include "darwin-nat.h"

/* Quick overview.
   Darwin kernel is Mach + BSD derived kernel.  Note that they share the
   same memory space and are linked together (ie there is no micro-kernel).

   Although ptrace(2) is available on Darwin, it is not complete.  We have
   to use Mach calls to read and write memory and to modify registers.  We
   also use Mach to get inferior faults.  As we cannot use select(2) or
   signals with Mach port (the Mach communication channel), signals are
   reported to gdb as an exception.  Furthermore we detect death of the
   inferior through a Mach notification message.  This way we only wait
   on Mach ports.

   Some Mach documentation is available for Apple xnu source package or
   from the web.  */


#define PTRACE(CMD, PID, ADDR, SIG) \
 darwin_ptrace(#CMD, CMD, (PID), (ADDR), (SIG))

extern boolean_t exc_server (mach_msg_header_t *in, mach_msg_header_t *out);

static void darwin_stop (ptid_t);

static void darwin_mourn_inferior (struct target_ops *ops);

static int darwin_lookup_task (char *args, task_t * ptask, int *ppid);

static void darwin_kill_inferior (struct target_ops *ops);

static void darwin_ptrace_me (void);

static void darwin_ptrace_him (int pid);

static void darwin_create_inferior (struct target_ops *ops, char *exec_file,
				    char *allargs, char **env, int from_tty);

static void darwin_files_info (struct target_ops *ops);

/* Current inferior.  */
darwin_inferior *darwin_inf = NULL;

/* Target operations for Darwin.  */
static struct target_ops *darwin_ops;

/* Task identifier of gdb.  */
static task_t gdb_task;

/* A copy of mach_host_self ().  */
mach_port_t darwin_host_self;

/* Exception port.  */
mach_port_t darwin_ex_port;

/* Notification port.  */
mach_port_t darwin_not_port;

/* Port set.  */
mach_port_t darwin_port_set;

/* Page size. */
static vm_size_t mach_page_size;

/* If Set, catch all mach exceptions (before they are converted to signals
   by the kernel).  */
static int enable_mach_exceptions;

#define PAGE_TRUNC(x) ((x) & ~(mach_page_size - 1))
#define PAGE_ROUND(x) PAGE_TRUNC((x) + mach_page_size - 1)

/* Buffer containing received message and to be sent message.  */
static union
{
  mach_msg_header_t hdr;
  char data[1024];
} msgin, msgout;

/* Current message state.
   If the kernel has sent a message it expects a reply and the inferior
   can't be killed before.  */
static enum msg_state { NO_MESSAGE, GOT_MESSAGE, REPLY_SENT } msg_state;

/* Unmarshalled received message.  */
static struct exc_msg
{
  /* Receive port.  */
  mach_port_t port;

  /* Thread and task taking the exception.  */
  mach_port_t thread_port;
  mach_port_t task_port;

  /* Type of the exception.  */
  exception_type_t ex_type;

  /* Machine dependent details.  */
  mach_msg_type_number_t data_count;
  integer_t ex_data[4];
} exc_msg;


/* This controls output of inferior debugging.
   1 = basic exception handling
   2 = task management
   3 = thread management
   4 = pending_event_handler
   6 = most chatty level.  */

static int darwin_debug_flag = 0;

static void
inferior_debug (int level, const char *fmt, ...)
{
  va_list ap;

  if (darwin_debug_flag < level)
    return;

  va_start (ap, fmt);
  printf_unfiltered (_("[%d inferior]: "), getpid ());
  vprintf_unfiltered (fmt, ap);
  va_end (ap);
}

void
mach_check_error (kern_return_t ret, const char *file,
                  unsigned int line, const char *func)
{
  if (ret == KERN_SUCCESS)
    return;
  if (func == NULL)
    func = _("[UNKNOWN]");
  
  error (_("error on line %u of \"%s\" in function \"%s\": %s (0x%lx)\n"),
         line, file, func, mach_error_string (ret), (unsigned long) ret);
}

static const char *
unparse_exception_type (unsigned int i)
{
  static char unknown_exception_buf[32];

  switch (i)
    {
    case EXC_BAD_ACCESS:
      return "EXC_BAD_ACCESS";
    case EXC_BAD_INSTRUCTION:
      return "EXC_BAD_INSTRUCTION";
    case EXC_ARITHMETIC:
      return "EXC_ARITHMETIC";
    case EXC_EMULATION:
      return "EXC_EMULATION";
    case EXC_SOFTWARE:
      return "EXC_SOFTWARE";
    case EXC_BREAKPOINT:
      return "EXC_BREAKPOINT";
    case EXC_SYSCALL:
      return "EXC_SYSCALL";
    case EXC_MACH_SYSCALL:
      return "EXC_MACH_SYSCALL";
    case EXC_RPC_ALERT:
      return "EXC_RPC_ALERT";
    case EXC_CRASH:
      return "EXC_CRASH";
    default:
      snprintf (unknown_exception_buf, 32, _("unknown (%d)"), i);
      return unknown_exception_buf;
    }
}

static int
darwin_ptrace (const char *name,
	       int request, int pid, PTRACE_TYPE_ARG3 arg3, int arg4)
{
  int ret;

  ret = ptrace (request, pid, (caddr_t) arg3, arg4);

  inferior_debug (2, _("ptrace (%s, %d, 0x%x, %d): %d (%s)\n"),
                  name, pid, arg3, arg4, ret,
                  (ret != 0) ? safe_strerror (errno) : _("no error"));
  return ret;
}

static int
cmp_thread_t (const void *l, const void *r)
{
  thread_t lt = *(const thread_t *)l;
  thread_t lr = *(const thread_t *)r;
  return (int)(lr - lt);
}

static void
darwin_check_new_threads (darwin_inferior *inf)
{
  kern_return_t kret;
  unsigned int i;
  thread_array_t thread_list;
  unsigned int new_nbr;
  unsigned int old_nbr;
  unsigned int new_ix, old_ix;
  VEC (thread_t) *thread_vec;

  /* Get list of threads.  */
  kret = task_threads (inf->task, &thread_list, &new_nbr);
  MACH_CHECK_ERROR (kret);
  if (kret != KERN_SUCCESS)
    return;

  if (new_nbr > 1)
    qsort (thread_list, new_nbr, sizeof (thread_t), cmp_thread_t);

  thread_vec = VEC_alloc (thread_t, new_nbr);

  if (inf->threads)
    old_nbr = VEC_length (thread_t, inf->threads);
  else
    old_nbr = 0;

  for (new_ix = 0, old_ix = 0; new_ix < new_nbr || old_ix < old_nbr;)
    {
      thread_t new_id = (new_ix < new_nbr) ?
	thread_list[new_ix] : THREAD_NULL;
      thread_t old_id = (old_ix < old_nbr) ?
	VEC_index (thread_t, inf->threads, old_ix) : THREAD_NULL;

      if (old_id == new_id)
	{
	  /* Thread still exist.  */
	  VEC_safe_push (thread_t, thread_vec, old_id);
	  new_ix++;
	  old_ix++;

	  kret = mach_port_deallocate (gdb_task, old_id);
	  MACH_CHECK_ERROR (kret);
	  continue;
	}
      if (new_id < old_id || old_ix == old_nbr)
	{
	  /* A thread was created.  */
	  struct thread_info *tp;

	  tp = add_thread (ptid_build (inf->pid, 0, new_id));
	  VEC_safe_push (thread_t, thread_vec, new_id);
	  new_ix++;
	  continue;
	}
      if (new_id > old_id || new_ix == new_nbr)
	{
	  /* A thread was removed.  */
	  delete_thread (ptid_build (inf->pid, 0, old_id));
	  kret = mach_port_deallocate (gdb_task, old_id);
	  MACH_CHECK_ERROR (kret);
	  old_ix++;
	}
    }

  if (inf->threads)
    VEC_free (thread_t, inf->threads);
  inf->threads = thread_vec;

  kret = vm_deallocate (gdb_task, (vm_address_t) thread_list,
			new_nbr * sizeof (int));
  MACH_CHECK_ERROR (kret);
}

static void
darwin_stop (ptid_t t)
{
  int ret;

  ret = kill (ptid_get_pid (inferior_ptid), SIGINT);
}

static void
darwin_resume (struct target_ops *ops,
	       ptid_t ptid, int step, enum target_signal signal)
{
  struct target_waitstatus status;
  int pid;
  thread_t thread;
  kern_return_t kret;
  int res;

  /* minus_one_ptid is RESUME_ALL.  */
  if (ptid_equal (ptid, minus_one_ptid))
    ptid = inferior_ptid;

  pid = ptid_get_pid (ptid);
  thread = ptid_get_tid (ptid);

  inferior_debug
    (2, _("darwin_resume: state=%d, thread=0x%x, step=%d signal=%d\n"),
     msg_state, thread, step, signal);

  switch (msg_state)
    {
    case GOT_MESSAGE:
      switch (exc_msg.ex_type)
	{
	case EXC_SOFTWARE:
	  if (exc_msg.ex_data[0] == EXC_SOFT_SIGNAL)
	    {
	      int nsignal = target_signal_to_host (signal);
	      res = PTRACE (PT_THUPDATE, pid,
			    (void *)(uintptr_t)exc_msg.thread_port, nsignal);
	      if (res < 0)
		printf_unfiltered (_("ptrace THUP: res=%d\n"), res);
	    }
	  break;

	default:
	  break;
	}
	  
      if (thread != 0)
	{
	  inferior_debug (2, _("darwin_set_sstep (thread=%x, enable=%d)\n"),
			  thread, step);
	  darwin_set_sstep (thread, step);
	}

      kret = mach_msg (&msgout.hdr, MACH_SEND_MSG | MACH_SEND_INTERRUPT,
		       msgout.hdr.msgh_size, 0,
		       MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE,
		       MACH_PORT_NULL);
      if (kret != 0)
	printf_unfiltered (_("mach_msg (reply) ret=%d\n"), kret);

      msg_state = REPLY_SENT;
      break;

    case NO_MESSAGE:
      if (step)
	res = PTRACE (PT_STEP, pid, (caddr_t)1, 0);
      else
	res = PTRACE (PT_CONTINUE, pid, (caddr_t)1, 0);
      break;

    default:
      gdb_assert (0);
    }
}

kern_return_t
catch_exception_raise_state
  (mach_port_t port,
   exception_type_t exception_type, mach_exception_data_t exception_data,
   mach_msg_type_number_t data_count, thread_state_flavor_t * state_flavor,
   thread_state_t in_state, mach_msg_type_number_t in_state_count,
   thread_state_t out_state, mach_msg_type_number_t out_state_count)
{
  return KERN_FAILURE;
}

kern_return_t
catch_exception_raise_state_identity
  (mach_port_t port, mach_port_t thread_port, mach_port_t task_port,
   exception_type_t exception_type, mach_exception_data_t exception_data,
   mach_msg_type_number_t data_count, thread_state_flavor_t * state_flavor,
   thread_state_t in_state, mach_msg_type_number_t in_state_count,
   thread_state_t out_state, mach_msg_type_number_t out_state_count)
{
  kern_return_t kret;

  kret = mach_port_deallocate (mach_task_self (), task_port);
  MACH_CHECK_ERROR (kret);
  kret = mach_port_deallocate (mach_task_self (), thread_port);
  MACH_CHECK_ERROR (kret);

  return KERN_FAILURE;
}

kern_return_t
catch_exception_raise (mach_port_t port,
		       mach_port_t thread_port,
		       mach_port_t task_port,
		       exception_type_t exception_type,
		       exception_data_t exception_data,
		       mach_msg_type_number_t data_count)
{
  kern_return_t kret;
  int i;
  int res;

  /* We got new rights to the task.  Get rid of it.  */
  kret = mach_port_deallocate (mach_task_self (), task_port);
  MACH_CHECK_ERROR (kret);

  inferior_debug
    (7, _("catch_exception_raise: exception_type=%d, data_count=%d\n"),
     exception_type, data_count);
  if (darwin_debug_flag > 7)
    {
      for (i = 0; i < data_count; i++)
	printf_unfiltered (" %08x", exception_data[i]);
      printf_unfiltered ("\n");
    }

  /* Save the message.
     FIXME: this should be in a per-thread variable.  */
  exc_msg.port = port;
  exc_msg.thread_port = thread_port;
  exc_msg.task_port = task_port;
  exc_msg.ex_type = exception_type;
  exc_msg.data_count = data_count;
  for (i = 0; i < data_count && i < 4; i++)
    exc_msg.ex_data[i] = exception_data[i];

  return KERN_SUCCESS;
}

static ptid_t
darwin_wait (struct target_ops *ops,
	     ptid_t ptid, struct target_waitstatus *status, int options)
{
  kern_return_t kret;
  mach_msg_header_t *hdr = &msgin.hdr;
  pid_t pid = ptid_get_pid (inferior_ptid); /* FIXME.  */

  gdb_assert (msg_state != GOT_MESSAGE);

  inferior_debug (6, _("darwin_wait: waiting for a message\n"));

  /* Wait for a message.  */
  kret = mach_msg (&msgin.hdr, MACH_RCV_MSG | MACH_RCV_INTERRUPT, 0,
		   sizeof (msgin.data), darwin_port_set, 0, MACH_PORT_NULL);

  if (kret == MACH_RCV_INTERRUPTED)
    {
      status->kind = TARGET_WAITKIND_IGNORE;
      return minus_one_ptid;
    }

  if (kret != MACH_MSG_SUCCESS)
    {
      inferior_debug (1, _("mach_msg: ret=%x\n"), kret);
      status->kind = TARGET_WAITKIND_SPURIOUS;
      return minus_one_ptid;
    }

  /* Debug: display message.  */
  if (darwin_debug_flag > 10)
   {
     const unsigned long *buf = (unsigned long *) hdr;
     unsigned int i;
     
     printf_unfiltered (_(" bits: 0x%x"), hdr->msgh_bits);
     printf_unfiltered (_(", size: 0x%x"), hdr->msgh_size);
     printf_unfiltered (_(", remote-port: 0x%x"), hdr->msgh_remote_port);
     printf_unfiltered (_(", local-port: 0x%x"), hdr->msgh_local_port);
     printf_unfiltered (_(", reserved: 0x%x"), hdr->msgh_reserved);
     printf_unfiltered (_(", id: 0x%x\n"), hdr->msgh_id);

     if (darwin_debug_flag > 11)
	{
	  printf_unfiltered (_("  data:"));
	  for (i = 0; i < hdr->msgh_size; i++)
	    printf_unfiltered (" %08lx", buf[i]);
	  printf_unfiltered (_("\n"));
	}
   }

  /* Exception message.  */
  if (hdr->msgh_local_port == darwin_ex_port)
    {
      /* Handle it via the exception server.  */
      if (!exc_server (&msgin.hdr, &msgout.hdr))
	{
	  printf_unfiltered (_("exc_server: unknown message (id=%x)\n"),
			     hdr->msgh_id);
	  status->kind = TARGET_WAITKIND_SPURIOUS;
	  return minus_one_ptid;
	}

      status->kind = TARGET_WAITKIND_STOPPED;

      inferior_debug (2, _("darwin_wait: thread=%x, got %s\n"),
		      exc_msg.thread_port, 
		      unparse_exception_type (exc_msg.ex_type));
  
      switch (exc_msg.ex_type)
	{
	case EXC_BAD_ACCESS:
	  status->value.sig = TARGET_EXC_BAD_ACCESS;
	  break;
	case EXC_BAD_INSTRUCTION:
	  status->value.sig = TARGET_EXC_BAD_INSTRUCTION;
	  break;
	case EXC_ARITHMETIC:
	  status->value.sig = TARGET_EXC_ARITHMETIC;
	  break;
	case EXC_EMULATION:
	  status->value.sig = TARGET_EXC_EMULATION;
	  break;
	case EXC_SOFTWARE:
	  if (exc_msg.ex_data[0] == EXC_SOFT_SIGNAL)
	    {
	      status->value.sig = target_signal_from_host (exc_msg.ex_data[1]);
	      inferior_debug (2, _("  (signal %d: %s)\n"),
			      exc_msg.ex_data[1],
			      target_signal_to_name (status->value.sig));
	    }
	  else
            status->value.sig = TARGET_EXC_SOFTWARE;
	  break;
	case EXC_BREAKPOINT:
	  /* Many internal GDB routines expect breakpoints to be reported
	     as TARGET_SIGNAL_TRAP, and will report TARGET_EXC_BREAKPOINT
	     as a spurious signal. */
	  status->value.sig = TARGET_SIGNAL_TRAP;
	  break;
	default:
	  status->value.sig = TARGET_SIGNAL_UNKNOWN;
	  break;
	}

      msg_state = GOT_MESSAGE;

      return ptid_build (pid, 0, exc_msg.thread_port);
    }
  else if (hdr->msgh_local_port == darwin_not_port)
    {
      pid_t res;
      int wstatus;

      /* FIXME: translate task port to pid.  */
      res = wait4 (pid, &wstatus, 0, NULL);
      if (res != pid)
	{
	  printf_unfiltered (_("wait4: res=%x\n"), res);
	  wstatus = 0;
	}
      status->kind = TARGET_WAITKIND_EXITED;
      status->value.integer = WEXITSTATUS (wstatus);

      inferior_debug (2, _("darwin_wait: pid=%d exit, status=%x\n"),
		      pid, wstatus);

      msg_state = NO_MESSAGE;

      return ptid;
    }
  else
    {
      printf_unfiltered (_("Bad local-port: %x\n"), hdr->msgh_local_port);
      status->kind = TARGET_WAITKIND_SPURIOUS;
      return minus_one_ptid;
    }
}

static void
darwin_mourn_inferior (struct target_ops *ops)
{
  struct inferior *inf = current_inferior ();
  kern_return_t kret;
  mach_port_t prev;
  int i;

  unpush_target (darwin_ops);

  /* Deallocate threads.  */
  if (darwin_inf->threads)
    {
      int k;
      thread_t t;
      for (k = 0; VEC_iterate (thread_t, darwin_inf->threads, k, t); k++)
	{
	  kret = mach_port_deallocate (gdb_task, t);
	  MACH_CHECK_ERROR (kret);
	}
      VEC_free (thread_t, darwin_inf->threads);
      darwin_inf->threads = NULL;
    }

  kret = mach_port_request_notification (gdb_task, darwin_inf->task,
					 MACH_NOTIFY_DEAD_NAME, 0,
					 darwin_inf->prev_not_port,
					 MACH_MSG_TYPE_MAKE_SEND_ONCE,
					 &prev);
  /* This can fail if the task is dead.  */
  if (kret == KERN_SUCCESS)
    {
      kret = mach_port_deallocate (gdb_task, prev);
      MACH_CHECK_ERROR (kret);
    }

  /* Deallocate saved exception ports.  */
  for (i = 0; i < darwin_inf->exception_info.count; i++)
    {
      kret = mach_port_deallocate
	(gdb_task, darwin_inf->exception_info.ports[i]);
      MACH_CHECK_ERROR (kret);
    }
  darwin_inf->exception_info.count = 0;

  kret = mach_port_deallocate (gdb_task, darwin_inf->task);
  MACH_CHECK_ERROR (kret);

  darwin_inf->task = 0;
  darwin_inf->pid = 0;

  generic_mourn_inferior ();
}

static void
darwin_stop_inferior (struct target_ops *ops, darwin_inferior *inf)
{
  struct target_waitstatus wstatus;
  ptid_t ptid;
  kern_return_t kret;
  int status;
  int res;

  gdb_assert (inf != NULL);

  kret = task_suspend (inf->task);
  MACH_CHECK_ERROR (kret);

  if (msg_state == GOT_MESSAGE)
    darwin_resume (darwin_ops, inferior_ptid, 0, 0);

  res = kill (inf->pid, SIGSTOP);
  if (res != 0)
    warning (_("cannot kill: %s\n"), safe_strerror (errno));

  ptid = darwin_wait (ops, inferior_ptid, &wstatus, 0);
  gdb_assert (wstatus.kind = TARGET_WAITKIND_STOPPED);
}

static void
darwin_kill_inferior (struct target_ops *ops)
{
  struct target_waitstatus wstatus;
  ptid_t ptid;
  kern_return_t kret;
  int status;
  int res;

  gdb_assert (darwin_inf != NULL);

  if (ptid_equal (inferior_ptid, null_ptid))
    return;

  darwin_stop_inferior (ops, darwin_inf);

  res = PTRACE (PT_KILL, darwin_inf->pid, 0, 0);
  gdb_assert (res == 0);

  if (msg_state == GOT_MESSAGE)
    {
      exc_msg.ex_type = 0;
      darwin_resume (ops, inferior_ptid, 0, 0);
    }

  kret = task_resume (darwin_inf->task);
  MACH_CHECK_ERROR (kret);

  ptid = darwin_wait (ops, inferior_ptid, &wstatus, 0);

  /* This double wait seems required...  */
  res = waitpid (darwin_inf->pid, &status, 0);
  gdb_assert (res == darwin_inf->pid);

  msg_state = NO_MESSAGE;

  target_mourn_inferior ();
}

/* The child must synchronize with gdb: gdb must set the exception port
   before the child call PTRACE_SIGEXC.  We use a pipe to achieve this.
   FIXME: is there a lighter way ?  */
static int ptrace_fds[2];

static void
darwin_ptrace_me (void)
{
  int res;
  char c;

  /* Close write end point.  */
  close (ptrace_fds[1]);

  /* Wait until gdb is ready.  */
  res = read (ptrace_fds[0], &c, 1);
  gdb_assert (res == 0);
  close (ptrace_fds[0]);

  /* Get rid of privileges.  */
  setegid (getgid ());

  /* Set TRACEME.  */
  PTRACE (PT_TRACE_ME, 0, 0, 0);

  /* Redirect signals to exception port.  */
  PTRACE (PT_SIGEXC, 0, 0, 0);
}

/* Dummy function to be sure fork_inferior uses fork(2) and not vfork(2).  */
static void
darwin_pre_ptrace (void)
{
  if (pipe (ptrace_fds) != 0)
    {
      ptrace_fds[0] = -1;
      ptrace_fds[1] = -1;
      error (_("unable to create a pipe: %s"), safe_strerror (errno));
    }
}

static kern_return_t
darwin_save_exception_ports (darwin_inferior *inf)
{
  kern_return_t kret;

  inf->exception_info.count =
    sizeof (inf->exception_info.ports) / sizeof (inf->exception_info.ports[0]);

  kret = task_get_exception_ports
    (inf->task, EXC_MASK_ALL, inf->exception_info.masks,
     &inf->exception_info.count, inf->exception_info.ports,
     inf->exception_info.behaviors, inf->exception_info.flavors);
  return kret;
}

static kern_return_t
darwin_restore_exception_ports (darwin_inferior *inf)
{
  int i;
  kern_return_t kret;

  for (i = 0; i < inf->exception_info.count; i++)
    {
      kret = task_set_exception_ports
        (inf->task, inf->exception_info.masks[i], inf->exception_info.ports[i],
	 inf->exception_info.behaviors[i], inf->exception_info.flavors[i]);
      if (kret != KERN_SUCCESS)
        return kret;
    }

  return KERN_SUCCESS;
}

static void
darwin_attach_pid (int pid)
{
  task_t itask;
  kern_return_t kret;
  mach_port_t prev_port;
  int traps_expected;
  exception_mask_t mask;

  kret = task_for_pid (gdb_task, pid, &itask);
  if (kret != KERN_SUCCESS)
    {
      int status;
      struct inferior *inf = current_inferior ();

      if (!inf->attach_flag)
	{
	  kill (pid, 9);
	  waitpid (pid, &status, 0);
	}

      error (_("Unable to find Mach task port for process-id %d: %s (0x%lx).\n"
	       " (please check gdb is setgid procmod)"),
             pid, mach_error_string (kret), (unsigned long) kret);
    }

  inferior_debug (2, _("inferior task: 0x%08x, pid: %d\n"), itask, pid);

  if (darwin_ex_port == MACH_PORT_NULL)
    {
      /* Create a port to get exceptions.  */
      kret = mach_port_allocate (gdb_task, MACH_PORT_RIGHT_RECEIVE,
				 &darwin_ex_port);
      gdb_assert (kret == KERN_SUCCESS);

      kret = mach_port_insert_right (gdb_task, darwin_ex_port, darwin_ex_port,
				     MACH_MSG_TYPE_MAKE_SEND);
      gdb_assert (kret == KERN_SUCCESS);

      /* Create a port set and put ex_port in it.  */
      kret = mach_port_allocate (gdb_task, MACH_PORT_RIGHT_PORT_SET,
				 &darwin_port_set);
      gdb_assert (kret == KERN_SUCCESS);

      kret = mach_port_move_member (gdb_task, darwin_ex_port, darwin_port_set);
      gdb_assert (kret == KERN_SUCCESS);

      /* Create a port to be notified when the child task terminates.  */
      kret = mach_port_allocate (gdb_task, MACH_PORT_RIGHT_RECEIVE,
				 &darwin_not_port);
      gdb_assert (kret == KERN_SUCCESS);

      kret = mach_port_insert_right (gdb_task, darwin_not_port, darwin_not_port,
				     MACH_MSG_TYPE_MAKE_SEND);
      gdb_assert (kret == KERN_SUCCESS);

      kret = mach_port_move_member (gdb_task, darwin_not_port, darwin_port_set);
      gdb_assert (kret == KERN_SUCCESS);
    }

  kret = mach_port_request_notification (gdb_task, itask,
					 MACH_NOTIFY_DEAD_NAME, 0,
					 darwin_not_port,
					 MACH_MSG_TYPE_MAKE_SEND_ONCE,
					 &darwin_inf->prev_not_port);
  gdb_assert (kret == KERN_SUCCESS);

  darwin_inf->task = itask;
  darwin_inf->pid = pid;

  kret = darwin_save_exception_ports (darwin_inf);
  gdb_assert (kret == KERN_SUCCESS);

  /* Set exception port.  */
  if (enable_mach_exceptions)
    mask = EXC_MASK_ALL;
  else
    mask = EXC_MASK_SOFTWARE;
  kret = task_set_exception_ports
    (itask, mask, darwin_ex_port, EXCEPTION_DEFAULT, THREAD_STATE_NONE);
  gdb_assert (kret == KERN_SUCCESS);

  push_target (darwin_ops);
}

static void
darwin_init_thread_list (darwin_inferior *inf)
{
  thread_t thread;

  darwin_check_new_threads (inf);

  gdb_assert (inf->threads && VEC_length (thread_t, inf->threads) > 0);
  thread = VEC_index (thread_t, inf->threads, 0);
  inferior_ptid = ptid_build (inf->pid, 0, thread);
}

static void
darwin_ptrace_him (int pid)
{
  task_t itask;
  kern_return_t kret;
  mach_port_t prev_port;
  int traps_expected;

  darwin_attach_pid (pid);

  /* Let's the child run.  */
  close (ptrace_fds[0]);
  close (ptrace_fds[1]);

  /* fork_inferior automatically add a thread - but it uses a wrong tid.  */
  delete_thread_silent (inferior_ptid);
  darwin_init_thread_list (darwin_inf);
  
  startup_inferior (START_INFERIOR_TRAPS_EXPECTED);
}

static void
darwin_create_inferior (struct target_ops *ops, char *exec_file,
			char *allargs, char **env, int from_tty)
{
  /* Do the hard work.  */
  fork_inferior (exec_file, allargs, env, darwin_ptrace_me, darwin_ptrace_him,
		 darwin_pre_ptrace, NULL);
  
  /* Return now in case of error.  */
  if (ptid_equal (inferior_ptid, null_ptid))
    return;
}


/* Attach to process PID, then initialize for debugging it
   and wait for the trace-trap that results from attaching.  */
static void
darwin_attach (struct target_ops *ops, char *args, int from_tty)
{
  pid_t pid;
  pid_t pid2;
  int wstatus;
  int res;
  struct inferior *inf;

  gdb_assert (msg_state == NO_MESSAGE);

  if (!args)
    error_no_arg (_("process-id to attach"));

  pid = atoi (args);

  if (pid == getpid ())		/* Trying to masturbate? */
    error (_("I refuse to debug myself!"));

  if (from_tty)
    printf_unfiltered (_("Attaching to pid %d\n"), pid);

  res = PTRACE (PT_ATTACHEXC, pid, 0, 0);
  if (res != 0)
    error (_("Unable to attach to process-id %d: %s (%d)"),
	   pid, safe_strerror (errno), errno);

  inf = add_inferior (pid);
  inf->attach_flag = 1;

  darwin_attach_pid (pid);

  pid2 = wait4 (pid, &wstatus, WUNTRACED, NULL);
  gdb_assert (pid2 == pid);
  inferior_debug (1, _("darwin_attach: wait4 pid=%d, status=0x%x\n"),
		  pid2, wstatus);

  
  darwin_init_thread_list (darwin_inf);

  darwin_check_osabi (darwin_inf, ptid_get_tid (inferior_ptid));

  /* Looks strange, but the kernel doesn't stop the process...
     (Bug in Leopard ?)
     Do it manually.  */
  /* FIXME: doesn't look to work with multi-threads!!  */
  kill (pid, SIGSTOP);
}

/* Take a program previously attached to and detaches it.
   The program resumes execution and will no longer stop
   on signals, etc.  We'd better not have left any breakpoints
   in the program or it'll die when it hits one.  For this
   to work, it may be necessary for the process to have been
   previously attached.  It *might* work if the program was
   started via fork.  */
static void
darwin_detach (struct target_ops *ops, char *args, int from_tty)
{
  kern_return_t kret;
  int res;

  if (from_tty)
    {
      char *exec_file = get_exec_file (0);
      if (exec_file == 0)
	exec_file = "";
      printf_unfiltered (_("Detaching from program: %s, %d\n"), exec_file,
			 ptid_get_pid (inferior_ptid));
      gdb_flush (gdb_stdout);
    }

  darwin_stop_inferior (ops, darwin_inf);

  kret = darwin_restore_exception_ports (darwin_inf);
  MACH_CHECK_ERROR (kret);

  if (msg_state == GOT_MESSAGE)
    {
      exc_msg.ex_type = 0;
      darwin_resume (ops, inferior_ptid, 0, 0);
    }

  kret = task_resume (darwin_inf->task);
  gdb_assert (kret == KERN_SUCCESS);

  res = PTRACE (PT_DETACH, darwin_inf->pid, 0, 0);
  if (res != 0)
    printf_unfiltered (_("Unable to detach from process-id %d: %s (%d)"),
		       darwin_inf->pid, safe_strerror (errno), errno);

  msg_state = NO_MESSAGE;

  darwin_mourn_inferior (ops);
}

static void
darwin_files_info (struct target_ops *ops)
{
  gdb_assert (darwin_inf != NULL);
}

static char *
darwin_pid_to_str (struct target_ops *ops, ptid_t ptid)
{
  static char buf[128];

  snprintf (buf, sizeof (buf),
	    _("process %d gdb-thread 0x%lx"),
	    (unsigned) ptid_get_pid (ptid),
	    (unsigned long) ptid_get_tid (ptid));
  return buf;
}

static int
darwin_thread_alive (struct target_ops *ops, ptid_t ptid)
{
  return 1;
}

/* If RDADDR is not NULL, read inferior task's LEN bytes from ADDR and
   copy it to RDADDR in gdb's address space.
   If WRADDR is not NULL, write gdb's LEN bytes from WRADDR and copy it
   to ADDR in inferior task's address space.
   Return 0 on failure; number of bytes read / writen  otherwise.  */
static int
darwin_read_write_inferior (task_t task, CORE_ADDR addr,
			    char *rdaddr, const char *wraddr, int length)
{
  kern_return_t err;
  mach_vm_address_t offset = addr & (mach_page_size - 1);
  mach_vm_address_t low_address = (mach_vm_address_t) (addr - offset);
  mach_vm_size_t aligned_length = (mach_vm_size_t) PAGE_ROUND (offset + length);
  pointer_t copied;
  int copy_count;
  mach_vm_size_t remaining_length;
  mach_vm_address_t region_address;
  mach_vm_size_t region_length;

  inferior_debug (8, _("darwin_read_write_inferior(%s, len=%d)\n"),
		  core_addr_to_string (addr), length);
  
  /* Get memory from inferior with page aligned addresses */
  err = mach_vm_read (task, low_address, aligned_length,
		      &copied, &copy_count);
  if (err != KERN_SUCCESS)
    {
      warning (_("darwin_read_write_inferior: vm_read failed: %s"), 
	       mach_error_string (err));
      return 0;
    }

  if (rdaddr != NULL)
    memcpy (rdaddr, (char *)copied + offset, length);

  if (wraddr == NULL)
    goto out;

  memcpy ((char *)copied + offset, wraddr, length);

  /* Do writes atomically.
     First check for holes and unwritable memory.  */
  for (region_address = low_address, remaining_length = aligned_length;
       region_address < low_address + aligned_length;
       region_address += region_length, remaining_length -= region_length)
    {
      vm_region_basic_info_data_64_t info;
      mach_port_t object_name;
      mach_vm_address_t old_address = region_address;
      mach_msg_type_number_t count;
      
      region_length = remaining_length;
      count = VM_REGION_BASIC_INFO_COUNT_64;
      err = mach_vm_region (task, &region_address, &region_length,
			    VM_REGION_BASIC_INFO_64,
			    (vm_region_info_t) &info, &count, &object_name);
      
      if (err != KERN_SUCCESS)
	{
	  warning (_("darwin_write_inferior: mach_vm_region failed: %s"), 
		   mach_error_string (err));
	  goto out;
	}

      /* Check for holes in memory */
      if (region_address > old_address)
	{
	  warning (_("No memory at %s (vs %s+0x%x). Nothing written"),
		   core_addr_to_string (old_address),
		   core_addr_to_string (region_address),
		   (unsigned)region_length);
	  length = 0;
	  goto out;
	}

      if (!(info.max_protection & VM_PROT_WRITE))
	{
	  warning (_("Memory at address %s is unwritable. Nothing written"),
		   core_addr_to_string (old_address));
	  length = 0;
	  goto out;
	}

      if (!(info.protection & VM_PROT_WRITE))
	{
	  err = mach_vm_protect (task, old_address, region_length,
				 FALSE, info.protection | VM_PROT_WRITE);
	  if (err != KERN_SUCCESS)
	    {
	      warning
		(_("darwin_read_write_inferior: mach_vm_protect failed: %s"), 
		 mach_error_string (err));
	      length = 0;
	      goto out;
	    }
	}
    }

  err = mach_vm_write (task, low_address, copied, aligned_length);

  if (err != KERN_SUCCESS)
    {
      warning (_("darwin_read_write_inferior: mach_vm_write failed: %s"),
	       mach_error_string (err));
      length = 0;
    }
out:
  mach_vm_deallocate (mach_task_self (), copied, copy_count);
  return length;
}


/* Return 0 on failure, number of bytes handled otherwise.  TARGET
   is ignored. */
static int
darwin_xfer_memory (CORE_ADDR memaddr, gdb_byte *myaddr, int len, int write,
		    struct mem_attrib *attrib, struct target_ops *target)
{
  task_t task = darwin_inf->task;

  if (task == MACH_PORT_NULL)
    return 0;

  inferior_debug (8, _("darwin_xfer_memory(%s, %d, %c)\n"),
		  core_addr_to_string (memaddr), len, write ? 'w' : 'r');

  if (write)
    return darwin_read_write_inferior (task, memaddr, NULL, myaddr, len);
  else
    return darwin_read_write_inferior (task, memaddr, myaddr, NULL, len);
}

static LONGEST
darwin_xfer_partial (struct target_ops *ops,
		     enum target_object object, const char *annex,
		     gdb_byte *readbuf, const gdb_byte *writebuf,
		     ULONGEST offset, LONGEST len)
{
  inferior_debug (8, _("darwin_xfer_partial(%s, %d, rbuf=%s, wbuf=%s)\n"),
		  core_addr_to_string (offset), (int)len, 
		  host_address_to_string (readbuf),
		  host_address_to_string (writebuf));

  if (object != TARGET_OBJECT_MEMORY)
    return -1;

  return darwin_read_write_inferior (darwin_inf->task, offset,
				     readbuf, writebuf, len);
}

static void
set_enable_mach_exceptions (char *args, int from_tty,
			    struct cmd_list_element *c)
{
  if (darwin_inf && darwin_inf->task != TASK_NULL)
    {
      exception_mask_t mask;
      kern_return_t kret;

      if (enable_mach_exceptions)
	mask = EXC_MASK_ALL;
      else
	{
	  darwin_restore_exception_ports (darwin_inf);
	  mask = EXC_MASK_SOFTWARE;
	}
      kret = task_set_exception_ports (darwin_inf->task, mask, darwin_ex_port,
				       EXCEPTION_DEFAULT, THREAD_STATE_NONE);
      MACH_CHECK_ERROR (kret);
    }
}

void
_initialize_darwin_inferior ()
{
  kern_return_t kret;

  gdb_assert (darwin_inf == NULL);
  
  gdb_task = mach_task_self ();
  darwin_host_self = mach_host_self ();

  /* Read page size.  */
  kret = host_page_size (darwin_host_self, &mach_page_size);
  if (kret != KERN_SUCCESS)
    {
      mach_page_size = 0x1000;
      MACH_CHECK_ERROR (kret);
    }

  darwin_inf = (struct darwin_inferior *)
    xmalloc (sizeof (struct darwin_inferior));

  memset (darwin_inf, 0, sizeof (*darwin_inf));

  darwin_ops = inf_child_target ();

  darwin_ops->to_shortname = "darwin-child";
  darwin_ops->to_longname = _("Darwin child process");
  darwin_ops->to_doc =
    _("Darwin child process (started by the \"run\" command).");
  darwin_ops->to_create_inferior = darwin_create_inferior;
  darwin_ops->to_attach = darwin_attach;
  darwin_ops->to_attach_no_wait = 0;
  darwin_ops->to_detach = darwin_detach;
  darwin_ops->to_files_info = darwin_files_info;
  darwin_ops->to_wait = darwin_wait;
  darwin_ops->to_mourn_inferior = darwin_mourn_inferior;
  darwin_ops->to_kill = darwin_kill_inferior;
  darwin_ops->to_stop = darwin_stop;
  darwin_ops->to_resume = darwin_resume;
  darwin_ops->to_thread_alive = darwin_thread_alive;
  darwin_ops->to_pid_to_str = darwin_pid_to_str;
  darwin_ops->to_load = NULL;
  darwin_ops->deprecated_xfer_memory = darwin_xfer_memory;
  darwin_ops->to_xfer_partial = darwin_xfer_partial;
  darwin_ops->to_has_thread_control = tc_schedlock /*| tc_switch */;

  darwin_complete_target (darwin_ops);

  add_target (darwin_ops);

  inferior_debug (2, _("GDB task: 0x%lx, pid: %d\n"), mach_task_self (),
                  getpid ());

  add_setshow_zinteger_cmd ("darwin", class_obscure,
			    &darwin_debug_flag, _("\
Set if printing inferior communication debugging statements."), _("\
Show if printing inferior communication debugging statements."), NULL,
			    NULL, NULL,
			    &setdebuglist, &showdebuglist);

  add_setshow_boolean_cmd ("mach-exceptions", class_support,
			   &enable_mach_exceptions, _("\
Set if mach exceptions are caught."), _("\
Show if mach exceptions are caught."), _("\
When this mode is on, all low level exceptions are reported before being\n\
reported by the kernel."),
			   &set_enable_mach_exceptions, NULL,
			   &setlist, &showlist);
}
