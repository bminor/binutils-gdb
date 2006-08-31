/* Async support for GDB, the GNU debugger.
   Copyright 1997, 1998, 1999, 2000, 2001, 2002, 2004, 2006
   Free Software Foundation, Inc.

   Original code from Apple Computer, Inc.
   Modified by Nick Roberts <nickrob@snap.net.nz>

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
#include "top.h"
#include "inferior.h"
#include "target.h"
#include "symfile.h"
#include "symtab.h"
#include "objfiles.h"
#include "gdbcmd.h"
#include "gdbcore.h"
#include "gdbthread.h"
#include "regcache.h"
#include "environ.h"
#include "event-top.h"
#include "inf-loop.h"
#include "gdb_stat.h"
#include "exceptions.h"

#include "bfd.h"

#include <sys/ptrace.h>
#include <sys/signal.h>
#include <setjmp.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <ctype.h>
#include <sys/param.h>
#include <sys/sysctl.h>

#include "async-nat-inferior.h"
/* classic-inferior-support */
//#include "macosx-nat.h"
//#include "macosx-nat-inferior-util.h"

#ifndef EXC_SOFT_SIGNAL
#define EXC_SOFT_SIGNAL 0
#endif

extern int standard_is_async_p (void);
extern int standard_can_async_p (void);

extern bfd *exec_bfd;

gdb_inferior_status *gdb_status = NULL;

int inferior_ptrace_flag = 1;
int inferior_ptrace_on_attach_flag = 1;
int inferior_bind_exception_port_flag = 1;
int inferior_handle_exceptions_flag = 1;
int inferior_handle_all_events_flag = 1;

enum gdb_source_type
{
  NEXT_SOURCE_NONE = 0x0,
  NEXT_SOURCE_EXCEPTION = 0x1,
  NEXT_SOURCE_SIGNAL = 0x2,
  NEXT_SOURCE_CFM = 0x4,
  NEXT_SOURCE_ALL = 0x7
};

struct gdb_pending_event
{
  enum gdb_source_type type;
  unsigned char *buf;
  struct gdb_pending_event *next;
};

struct gdb_pending_event *pending_event_chain, *pending_event_tail;

static enum gdb_source_type gdb_fetch_event (struct
                                                   gdb_inferior_status
                                                   *inferior,
                                                   unsigned char *buf,
                                                   size_t len,
                                                   unsigned int flags,
                                                   int timeout);

static int gdb_service_event (enum gdb_source_type source,
                                 unsigned char *buf,
                                 struct target_waitstatus *status);

static void gdb_handle_signal (gdb_signal_thread_message *msg,
			       struct target_waitstatus *status);

static void gdb_add_to_pending_events (enum gdb_source_type,
                                          unsigned char *buf);

static void gdb_pending_event_handler (void *data);

void gdb_clear_pending_events ();

static void gdb_inferior_reset (gdb_inferior_status *s);

static void gdb_inferior_destroy (gdb_inferior_status *s);

static void
gdb_handle_signal (gdb_signal_thread_message *msg,
		   struct target_waitstatus *status)
{
  //CHECK_FATAL (gdb_status != NULL);

  //CHECK_FATAL (gdb_status->attached_in_ptrace);
  //CHECK_FATAL (!gdb_status->stopped_in_ptrace);
  /* CHECK_FATAL (! gdb_status->stopped_in_softexc); */

  /* if (inferior_debug_flag)
    {
      gdb_signal_thread_debug_status (stderr, msg->status);
      } */

  if (msg->pid != gdb_status->pid)
    {
      warning ("gdb_handle_signal: signal message was for pid %d, "
               "not for inferior process (pid %d)\n",
               msg->pid, gdb_status->pid);
      return;
    }

  if (WIFEXITED (msg->status))
    {
      status->kind = TARGET_WAITKIND_EXITED;
      status->value.integer = WEXITSTATUS (msg->status);
      return;
    }

  if (!WIFSTOPPED (msg->status))
    {
      status->kind = TARGET_WAITKIND_SIGNALLED;
      status->value.sig = target_signal_from_host (WTERMSIG (msg->status));
      return;
    }

  gdb_status->stopped_in_ptrace = 1;

  //prepare_threads_after_stop (gdb_status);

  status->kind = TARGET_WAITKIND_STOPPED;
  status->value.sig = target_signal_from_host (WSTOPSIG (msg->status));
}

static void
gdb_add_to_port_set (struct gdb_inferior_status *inferior,
                        fd_set * fds, int flags)
{
  FD_ZERO (fds);

  if ((flags & NEXT_SOURCE_SIGNAL)
      && inferior->signal_status.receive_fd > 0)
    {
      FD_SET (inferior->signal_status.receive_fd, fds);
    }
}

/* TIMEOUT is either -1, 0, or greater than 0.
   For 0, check if there is anything to read, but don't block.
   For -1, block until there is something to read.
   For >0, block at least the specified number of microseconds, or until there
   is something to read.
   The kernel doesn't give better than ~1HZ (0.01 sec) resolution, so
   don't use this as a high accuracy timer. */

static enum gdb_source_type
gdb_fetch_event (struct gdb_inferior_status *inferior,
                    unsigned char *buf, size_t len,
                    unsigned int flags, int timeout)
{
  fd_set fds;
  int fd, ret;
  struct timeval tv;

  //  CHECK_FATAL (len >= sizeof (gdb_signal_thread_message));

  tv.tv_sec = 0;
  tv.tv_usec = timeout;

  gdb_add_to_port_set (inferior, &fds, flags);

  for (;;)
    {
      if (timeout == -1)
        {
          ret = select (FD_SETSIZE, &fds, NULL, NULL, NULL);
        }
      else
        {
          ret = select (FD_SETSIZE, &fds, NULL, NULL, &tv);
        }
      if ((ret < 0) && (errno == EINTR))
        {
          continue;
        }
      if (ret < 0)
        {
          internal_error (__FILE__, __LINE__, "unable to select: %s",
                          strerror (errno));
        }
      if (ret == 0)
        {
          return NEXT_SOURCE_NONE;
        }
      break;
    }

  fd = inferior->signal_status.receive_fd;
  if (fd > 0 && FD_ISSET (fd, &fds))
    {
      read (fd, buf, sizeof (gdb_signal_thread_message));
      return NEXT_SOURCE_SIGNAL;
    }

  return NEXT_SOURCE_NONE;
}

/* This takes the data from an event and puts it on the tail of the
   "pending event" chain. */

static void
gdb_add_to_pending_events (enum gdb_source_type type,
                              unsigned char *buf)
{
  struct gdb_pending_event *new_event;

  new_event = (struct gdb_pending_event *)
    xmalloc (sizeof (struct gdb_pending_event));

  new_event->type = type;

  if (type == NEXT_SOURCE_SIGNAL)
    {
      gdb_signal_thread_message *mssg;
      mssg = (gdb_signal_thread_message *)
        xmalloc (sizeof (gdb_signal_thread_message));
      memcpy (mssg, buf, sizeof (gdb_signal_thread_message));
      /*inferior_debug (1,
	"gdb_add_to_pending_events: adding a signal event to the pending events.\n"); */
      new_event->buf = (void *) mssg;
    }

  new_event->next = NULL;

  if (pending_event_chain == NULL)
    {
      pending_event_chain = new_event;
      pending_event_tail = new_event;
    }
  else
    {
      pending_event_tail->next = new_event;
      pending_event_tail = new_event;
    }
}

void
gdb_clear_pending_events ()
{
  struct gdb_pending_event *event_ptr = pending_event_chain;

  while (event_ptr != NULL)
    {
      pending_event_chain = event_ptr->next;
      xfree (event_ptr->buf);
      xfree (event_ptr);
      event_ptr = pending_event_chain;
    }
}

/* This extracts the top of the pending event chain and posts a gdb event
   with its content to the gdb event queue.  Returns 0 if there were no
   pending events to be posted, 1 otherwise. */

int
gdb_post_pending_event (void)
{
  struct gdb_pending_event *event;

  if (pending_event_chain == NULL)
    {
      //inferior_debug (1, "gdb_post_pending_event: no events to post\n");
      return 0;
    }
  else
    {
      event = pending_event_chain;
      pending_event_chain = pending_event_chain->next;
      if (pending_event_chain == NULL)
        pending_event_tail = NULL;

      /*inferior_debug (1,
	"gdb_post_pending_event: consuming event off queue\n"); */
      gdb_queue_event (gdb_pending_event_handler, (void *) event, HEAD);

      return 1;
    }
}

static void
gdb_pending_event_handler (void *data)
{
  //inferior_debug (1, "Called in gdb_pending_event_handler\n");
  async_client_callback (INF_REG_EVENT, data);
}

static int
gdb_service_event (enum gdb_source_type source,
                      unsigned char *buf, struct target_waitstatus *status)
{
 if (source == NEXT_SOURCE_SIGNAL)
    {
      // inferior_debug (1, "gdb_service_events: got signal message\n");
      gdb_handle_signal ((gdb_signal_thread_message *) buf, status);
      //      CHECK_FATAL (status->kind != TARGET_WAITKIND_SPURIOUS);
      if (!inferior_handle_all_events_flag)
        {
          return 1;
        }
    }
  else
    {
      error ("got message from unknown source: 0x%08x\n", source);
      return 0;
    }
  return 1;
}

/* This drains the event sources.  The first event found is directly
   handled.  The rest are placed on the pending events queue, to be
   handled the next time that the inferior is "run".

   Returns: The number of events found. */

int
gdb_process_events (struct gdb_inferior_status *inferior,
                       struct target_waitstatus *status,
                       int timeout, int service_first_event)
{
  enum gdb_source_type source;
  unsigned char buf[1024];
  int event_count;

  //  CHECK_FATAL (status->kind == TARGET_WAITKIND_SPURIOUS);

  source = gdb_fetch_event (inferior, buf, sizeof (buf),
                               NEXT_SOURCE_ALL, timeout);
  if (source == NEXT_SOURCE_NONE)
    {
      return 0;
    }

  event_count = 1;

  if (service_first_event)
    {
      if (gdb_service_event (source, buf, status) == 0)
        return 0;
    }
  else
    {
      gdb_add_to_pending_events (source, buf);
    }

  return event_count;
}

ptid_t
gdb_process_pending_event (struct gdb_inferior_status *ns,
                              struct target_waitstatus *status,
                              gdb_client_data client_data)
{
  struct gdb_pending_event *event
    = (struct gdb_pending_event *) client_data;

  //inferior_debug (1, "Processing pending event type: %d\n", event->type);
  gdb_service_event (event->type, (unsigned char *) event->buf, status);

  return pid_to_ptid (gdb_status->pid);
}

void
gdb_create_inferior (struct gdb_inferior_status *inferior,  int pid)
{
  //  CHECK_FATAL (inferior != NULL);

  gdb_inferior_destroy (inferior);
  gdb_inferior_reset (inferior);

  inferior->pid = pid;

  inferior->attached_in_ptrace = 0;
  inferior->stopped_in_ptrace = 0;
  inferior->stopped_in_softexc = 0;

  inferior->suspend_count = 0;
}

void
async_terminal_inferior (void)
{
  terminal_inferior ();

  if (!sync_execution)
    return;
  delete_file_handler (input_fd);
}

void
async_terminal_ours (void)
{
  terminal_ours ();

  if (!sync_execution)
    return;

  add_file_handler (input_fd, stdin_event_handler, 0);
}

static void
gdb_inferior_reset (gdb_inferior_status *s)
{
  s->pid = 0;

  s->attached_in_ptrace = 0;
  s->stopped_in_ptrace = 0;
  s->stopped_in_softexc = 0;

  s->suspend_count = 0;

  gdb_signal_thread_init (&s->signal_status);

}

static void
gdb_inferior_destroy (gdb_inferior_status *s)
{
  gdb_signal_thread_destroy (&s->signal_status);

  s->pid = 0;

  gdb_inferior_reset (s);
}

void
_initialize_gdb_inferior ()
{
  gdb_status = (struct gdb_inferior_status *)
    xmalloc (sizeof (struct gdb_inferior_status));

  gdb_inferior_reset (gdb_status);
}
