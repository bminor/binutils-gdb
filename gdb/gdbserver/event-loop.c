/* Event loop machinery for the remote server for GDB.
   Copyright (C) 1999, 2000, 2001, 2002, 2005, 2006, 2007, 2008
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
   along with this program.  If not, see <http://www.gnu.org/licenses/>. */

/* Based on src/gdb/event-loop.c.  */

#include "server.h"

#ifdef HAVE_POLL
#if defined (HAVE_POLL_H)
#include <poll.h>
#elif defined (HAVE_SYS_POLL_H)
#include <sys/poll.h>
#endif
#endif

#include <sys/types.h>
#include <string.h>
#include <sys/time.h>

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

typedef struct gdb_event gdb_event;
typedef void (event_handler_func) (int);

/* Tell create_file_handler what events we are interested in.
   This is used by the select version of the event loop. */

#define GDB_READABLE	(1<<1)
#define GDB_WRITABLE	(1<<2)
#define GDB_EXCEPTION	(1<<3)

/* Event for the GDB event system.  Events are queued by calling
   async_queue_event and serviced later on by gdb_do_one_event. An
   event can be, for instance, a file descriptor becoming ready to be
   read. Servicing an event simply means that the procedure PROC will
   be called.  We have 2 queues, one for file handlers that we listen
   to in the event loop, and one for the file handlers+events that are
   ready. The procedure PROC associated with each event is always the
   same (handle_file_event).  Its duty is to invoke the handler
   associated with the file descriptor whose state change generated
   the event, plus doing other cleanups and such. */

struct gdb_event
  {
    event_handler_func *proc;	/* Procedure to call to service this event. */
    int fd;			/* File descriptor that is ready. */
    struct gdb_event *next_event;	/* Next in list of events or NULL. */
  };

/* Information about each file descriptor we register with the event
   loop. */

typedef struct file_handler
  {
    int fd;			/* File descriptor. */
    int mask;			/* Events we want to monitor: POLLIN, etc. */
    int ready_mask;		/* Events that have been seen since
				   the last time. */
    handler_func *proc;		/* Procedure to call when fd is ready. */
    gdb_client_data client_data;	/* Argument to pass to proc. */
    int error;			/* Was an error detected on this fd? */
    struct file_handler *next_file;	/* Next registered file descriptor. */
  }
file_handler;

/* Event queue:
   - the first event in the queue is the head of the queue.
   It will be the next to be serviced.
   - the last event in the queue

   Events can be inserted at the front of the queue or at the end of
   the queue.  Events will be extracted from the queue for processing
   starting from the head.  Therefore, events inserted at the head of
   the queue will be processed in a last in first out fashion, while
   those inserted at the tail of the queue will be processed in a first
   in first out manner.  All the fields are NULL if the queue is
   empty. */

static struct
  {
    gdb_event *first_event;	/* First pending event */
    gdb_event *last_event;	/* Last pending event */
  }
event_queue;

/* Gdb_notifier is just a list of file descriptors gdb is interested
   in.  These are the input file descriptor, and the target file
   descriptor. We have two flavors of the notifier, one for platforms
   that have the POLL function, the other for those that don't, and
   only support SELECT. Each of the elements in the gdb_notifier list
   is basically a description of what kind of events gdb is interested
   in, for each fd.  */

#ifdef USE_WIN32API
#include <windows.h>
#include <io.h>
#endif

static struct
  {
    /* Ptr to head of file handler list. */
    file_handler *first_file_handler;

#ifdef HAVE_POLL
    /* Ptr to array of pollfd structures. */
    struct pollfd *poll_fds;
#endif

    /* Masks to be used in the next call to select.
       Bits are set in response to calls to create_file_handler. */
    fd_set check_masks[3];

    /* What file descriptors were found ready by select. */
    fd_set ready_masks[3];

    /* Number of file descriptors to monitor. (for poll) */
    /* Number of valid bits (highest fd value + 1). (for select) */
    int num_fds;
  }
gdb_notifier;

static void create_file_handler (int fd, int mask, handler_func * proc, gdb_client_data client_data);
static void handle_file_event (int event_file_desc);
static int gdb_wait_for_event (void);
static void async_queue_event (gdb_event * event_ptr);
static gdb_event *create_file_event (int fd);
static int process_event (void);


/* Insert an event object into the gdb event queue

   EVENT_PTR points to the event to be inserted into the queue.  The
   caller must allocate memory for the event. It is freed after the
   event has ben handled.  Events in the queue will be processed head
   to tail, therefore, events will be processed first in first out. */
static void
async_queue_event (gdb_event *event_ptr)
{
  /* The event will become the new last_event. */

  event_ptr->next_event = NULL;
  if (event_queue.first_event == NULL)
    event_queue.first_event = event_ptr;
  else
    event_queue.last_event->next_event = event_ptr;
  event_queue.last_event = event_ptr;
}

/* Create a file event, to be enqueued in the event queue for
   processing. The procedure associated to this event is always
   handle_file_event, which will in turn invoke the one that was
   associated to FD when it was registered with the event loop. */
static gdb_event *
create_file_event (int fd)
{
  gdb_event *file_event_ptr;

  file_event_ptr = malloc (sizeof (gdb_event));
  file_event_ptr->proc = handle_file_event;
  file_event_ptr->fd = fd;
  return (file_event_ptr);
}

/* Process one event.  If an event was processed, 1 is returned
   otherwise 0 is returned.  Scan the queue from head to tail,
   processing therefore the high priority events first, by invoking
   the associated event handler procedure.  */
static int
process_event (void)
{
  gdb_event *event_ptr, *prev_ptr;
  event_handler_func *proc;
  int fd;

  /* Look in the event queue to find an event that is ready
     to be processed. */

  for (event_ptr = event_queue.first_event; event_ptr != NULL;
       event_ptr = event_ptr->next_event)
    {
      /* Call the handler for the event. */

      proc = event_ptr->proc;
      fd = event_ptr->fd;

      /* Let's get rid of the event from the event queue.  We need to
         do this now because while processing the event, the proc
         function could end up calling 'error' and therefore jump out
         to the caller of this function, gdb_do_one_event. In that
         case, we would have on the event queue an event wich has been
         processed, but not deleted. */

      if (event_queue.first_event == event_ptr)
	{
	  event_queue.first_event = event_ptr->next_event;
	  if (event_ptr->next_event == NULL)
	    event_queue.last_event = NULL;
	}
      else
	{
	  prev_ptr = event_queue.first_event;
	  while (prev_ptr->next_event != event_ptr)
	    prev_ptr = prev_ptr->next_event;

	  prev_ptr->next_event = event_ptr->next_event;
	  if (event_ptr->next_event == NULL)
	    event_queue.last_event = prev_ptr;
	}
      free (event_ptr);

      /* Now call the procedure associated with the event. */
      (*proc) (fd);
      return 1;
    }

  /* this is the case if there are no event on the event queue. */
  return 0;
}

/* Process one high level event.  If nothing is ready at this time,
   wait for something to happen (via gdb_wait_for_event), then process
   it.  Returns >0 if something was done otherwise returns <0 (this
   can happen if there are no event sources to wait for).  */

int
gdb_do_one_event (void)
{
  /* Any events already waiting in the queue?  */
  if (process_event ())
    return 1;

  /* Wait for a new event.  If gdb_wait_for_event returns -1, we
     should get out because this means that there are no event sources
     left. This will make the event loop stop, and the application
     exit.  */

  if (gdb_wait_for_event () < 0)
    return -1;

  /* Handle any new events occurred while waiting.  */
  if (process_event ())
    return 1;

  /* If gdb_wait_for_event has returned 1, it means that one
     event has been handled. We break out of the loop. */
  return 1;
}

/* Start up the event loop. This is the entry point to the event loop
   from the command loop. */

void
start_event_loop (void)
{
  /* Loop until there is nothing to do. This is the entry point to the
     event loop engine. gdb_do_one_event, called via catch_errors()
     will process one event for each invocation.  It blocks waiting
     for an event and then processes it.  >0 when an event is
     processed, 0 when catch_errors() caught an error and <0 when
     there are no longer any event sources registered.  */

  while (1)
    {
      int gdb_result;

      gdb_result = gdb_do_one_event ();
      if (gdb_result < 0)
	break;
    }

  /* We are done with the event loop. There are no more event sources
     to listen to.  So we exit gdbserver.  */
  return;
}


/* Wrapper function for create_file_handler, so that the caller
   doesn't have to know implementation details about the use of poll
   vs. select.  */
void
add_file_handler (int fd, handler_func * proc, gdb_client_data client_data)
{
#ifdef HAVE_POLL
  create_file_handler (fd, POLLIN, proc, client_data);
#else
  create_file_handler (fd, GDB_READABLE | GDB_EXCEPTION, proc, client_data);
#endif
}

/* Add a file handler/descriptor to the list of descriptors we are
   interested in.
   FD is the file descriptor for the file/stream to be listened to.
   For the poll case, MASK is a combination (OR) of
   POLLIN, POLLRDNORM, POLLRDBAND, POLLPRI, POLLOUT, POLLWRNORM,
   POLLWRBAND: these are the events we are interested in. If any of them
   occurs, proc should be called.
   For the select case, MASK is a combination of READABLE, WRITABLE, EXCEPTION.
   PROC is the procedure that will be called when an event occurs for
   FD.  CLIENT_DATA is the argument to pass to PROC. */
static void
create_file_handler (int fd, int mask, handler_func * proc, gdb_client_data client_data)
{
  file_handler *file_ptr;

  /* Do we already have a file handler for this file? (We may be
     changing its associated procedure). */
  for (file_ptr = gdb_notifier.first_file_handler;
       file_ptr != NULL;
       file_ptr = file_ptr->next_file)
    if (file_ptr->fd == fd)
      break;

  /* It is a new file descriptor.  Add it to the list.  Otherwise,
     just change the data associated with it. */
  if (file_ptr == NULL)
    {
      file_ptr = malloc (sizeof (*file_ptr));
      file_ptr->fd = fd;
      file_ptr->ready_mask = 0;
      file_ptr->next_file = gdb_notifier.first_file_handler;
      gdb_notifier.first_file_handler = file_ptr;

#ifdef HAVE_POLL
      gdb_notifier.num_fds++;
      if (gdb_notifier.poll_fds)
	gdb_notifier.poll_fds =
	  realloc (gdb_notifier.poll_fds,
		   (gdb_notifier.num_fds
		    * sizeof (struct pollfd)));
      else
	gdb_notifier.poll_fds =
	  malloc (sizeof (struct pollfd));
      (gdb_notifier.poll_fds + gdb_notifier.num_fds - 1)->fd = fd;
      (gdb_notifier.poll_fds + gdb_notifier.num_fds - 1)->events = mask;
      (gdb_notifier.poll_fds + gdb_notifier.num_fds - 1)->revents = 0;
#else
      if (mask & GDB_READABLE)
	FD_SET (fd, &gdb_notifier.check_masks[0]);
      else
	FD_CLR (fd, &gdb_notifier.check_masks[0]);

      if (mask & GDB_WRITABLE)
	FD_SET (fd, &gdb_notifier.check_masks[1]);
      else
	FD_CLR (fd, &gdb_notifier.check_masks[1]);

      if (mask & GDB_EXCEPTION)
	FD_SET (fd, &gdb_notifier.check_masks[2]);
      else
	FD_CLR (fd, &gdb_notifier.check_masks[2]);

      if (gdb_notifier.num_fds <= fd)
	gdb_notifier.num_fds = fd + 1;
#endif
    }

  file_ptr->proc = proc;
  file_ptr->client_data = client_data;
  file_ptr->mask = mask;
}

/* Remove the file descriptor FD from the list of monitored fd's:
   i.e. we don't care anymore about events on the FD. */
void
delete_file_handler (int fd)
{
  file_handler *file_ptr, *prev_ptr = NULL;
  int i;
#ifdef HAVE_POLL
  int j;
  struct pollfd *new_poll_fds;
#endif

  /* Find the entry for the given file. */

  for (file_ptr = gdb_notifier.first_file_handler; file_ptr != NULL;
       file_ptr = file_ptr->next_file)
    {
      if (file_ptr->fd == fd)
	break;
    }

  if (file_ptr == NULL)
    return;

#ifdef HAVE_POLL
  /* Create a new poll_fds array by copying every fd's information but the
     one we want to get rid of. */

  new_poll_fds = malloc ((gdb_notifier.num_fds - 1) * sizeof (struct pollfd));

  for (i = 0, j = 0; i < gdb_notifier.num_fds; i++)
    {
      if ((gdb_notifier.poll_fds + i)->fd != fd)
	{
	  (new_poll_fds + j)->fd = (gdb_notifier.poll_fds + i)->fd;
	  (new_poll_fds + j)->events = (gdb_notifier.poll_fds + i)->events;
	  (new_poll_fds + j)->revents = (gdb_notifier.poll_fds + i)->revents;
	  j++;
	}
    }
  free (gdb_notifier.poll_fds);
  gdb_notifier.poll_fds = new_poll_fds;
  gdb_notifier.num_fds--;
#else
  if (file_ptr->mask & GDB_READABLE)
    FD_CLR (fd, &gdb_notifier.check_masks[0]);
  if (file_ptr->mask & GDB_WRITABLE)
    FD_CLR (fd, &gdb_notifier.check_masks[1]);
  if (file_ptr->mask & GDB_EXCEPTION)
    FD_CLR (fd, &gdb_notifier.check_masks[2]);

  /* Find current max fd. */

  if ((fd + 1) == gdb_notifier.num_fds)
    {
      gdb_notifier.num_fds--;
      for (i = gdb_notifier.num_fds; i; i--)
	{
	  if (FD_ISSET (i - 1, &gdb_notifier.check_masks[0])
	      || FD_ISSET (i - 1, &gdb_notifier.check_masks[1])
	      || FD_ISSET (i - 1, &gdb_notifier.check_masks[2]))
	    break;
	}
      gdb_notifier.num_fds = i;
    }
#endif /* HAVE_POLL */

  /* Deactivate the file descriptor, by clearing its mask,
     so that it will not fire again. */

  file_ptr->mask = 0;

  /* Get rid of the file handler in the file handler list. */
  if (file_ptr == gdb_notifier.first_file_handler)
    gdb_notifier.first_file_handler = file_ptr->next_file;
  else
    {
      for (prev_ptr = gdb_notifier.first_file_handler;
	   prev_ptr->next_file != file_ptr;
	   prev_ptr = prev_ptr->next_file)
	;
      prev_ptr->next_file = file_ptr->next_file;
    }
  free (file_ptr);
}

/* Handle the given event by calling the procedure associated to the
   corresponding file handler.  Called by process_event indirectly,
   through event_ptr->proc.  EVENT_FILE_DESC is file descriptor of the
   event in the front of the event queue. */
static void
handle_file_event (int event_file_desc)
{
  file_handler *file_ptr;
  int mask;
#ifdef HAVE_POLL
  int error_mask;
  int error_mask_returned;
#endif

  /* Search the file handler list to find one that matches the fd in
     the event. */
  for (file_ptr = gdb_notifier.first_file_handler; file_ptr != NULL;
       file_ptr = file_ptr->next_file)
    {
      if (file_ptr->fd == event_file_desc)
	{
	  /* With poll, the ready_mask could have any of three events
	     set to 1: POLLHUP, POLLERR, POLLNVAL. These events cannot
	     be used in the requested event mask (events), but they
	     can be returned in the return mask (revents). We need to
	     check for those event too, and add them to the mask which
	     will be passed to the handler. */

	  /* See if the desired events (mask) match the received
	     events (ready_mask). */

#ifdef HAVE_POLL
	  error_mask = POLLHUP | POLLERR | POLLNVAL;
	  mask = (file_ptr->ready_mask & file_ptr->mask) |
	    (file_ptr->ready_mask & error_mask);
	  error_mask_returned = mask & error_mask;

	  if (error_mask_returned != 0)
	    {
	      /* Work in progress.  We may need to tell somebody what
		 kind of error we had. */
	      if (error_mask_returned & POLLHUP)
		printf ("Hangup detected on fd %d\n", file_ptr->fd);
	      if (error_mask_returned & POLLERR)
		printf ("Error detected on fd %d\n", file_ptr->fd);
	      if (error_mask_returned & POLLNVAL)
		printf ("Invalid or non-`poll'able fd %d\n", file_ptr->fd);
	      file_ptr->error = 1;
	    }
	  else
	    file_ptr->error = 0;
#else
	  if (file_ptr->ready_mask & GDB_EXCEPTION)
	    {
	      fprintf (stderr, "Exception condition detected on fd %d\n",
		       file_ptr->fd);
	      file_ptr->error = 1;
	    }
	  else
	    file_ptr->error = 0;
	  mask = file_ptr->ready_mask & file_ptr->mask;
#endif /* HAVE_POLL */

	  /* Clear the received events for next time around. */
	  file_ptr->ready_mask = 0;

	  /* If there was a match, then call the handler. */
	  if (mask != 0)
	    (*file_ptr->proc) (file_ptr->error, file_ptr->client_data);
	  break;
	}
    }
}

/* Called by gdb_do_one_event to wait for new events on the
   monitored file descriptors. Queue file events as they are
   detected by the poll.
   If there are no events, this function will block in the
   call to poll.
   Return -1 if there are no files descriptors to monitor,
   otherwise return 0. */
static int
gdb_wait_for_event (void)
{
  file_handler *file_ptr;
  gdb_event *file_event_ptr;
  int num_found = 0;

  /* Make sure all output is done before getting another event. */
  fflush (stdout);
  fflush (stderr);

  if (gdb_notifier.num_fds == 0)
    return -1;

#ifdef HAVE_POLL
  num_found =
    poll (gdb_notifier.poll_fds,
	  (unsigned long) gdb_notifier.num_fds, -1);

  /* Don't print anything if we get out of poll because of a
     signal. */
  if (num_found == -1 && errno != EINTR)
    perror_with_name (("poll"));
#else
  gdb_notifier.ready_masks[0] = gdb_notifier.check_masks[0];
  gdb_notifier.ready_masks[1] = gdb_notifier.check_masks[1];
  gdb_notifier.ready_masks[2] = gdb_notifier.check_masks[2];
  num_found = select (gdb_notifier.num_fds,
		      &gdb_notifier.ready_masks[0],
		      &gdb_notifier.ready_masks[1],
		      &gdb_notifier.ready_masks[2],
		      NULL);

  /* Clear the masks after an error from select. */
  if (num_found == -1)
    {
      FD_ZERO (&gdb_notifier.ready_masks[0]);
      FD_ZERO (&gdb_notifier.ready_masks[1]);
      FD_ZERO (&gdb_notifier.ready_masks[2]);
#ifdef EINTR
      /* Dont print anything is we got a signal, let gdb handle it. */
      if (errno != EINTR)
	perror_with_name (("select"));
#endif
    }
#endif /* HAVE_POLL */

  /* Enqueue all detected file events. */

#ifdef HAVE_POLL
    {
      int i;
      for (i = 0; (i < gdb_notifier.num_fds) && (num_found > 0); i++)
	{
	  if ((gdb_notifier.poll_fds + i)->revents)
	    num_found--;
	  else
	    continue;

	  for (file_ptr = gdb_notifier.first_file_handler;
	       file_ptr != NULL;
	       file_ptr = file_ptr->next_file)
	    {
	      if (file_ptr->fd == (gdb_notifier.poll_fds + i)->fd)
		break;
	    }

	  if (file_ptr)
	    {
	      /* Enqueue an event only if this is still a new event for
	         this fd. */
	      if (file_ptr->ready_mask == 0)
		{
		  file_event_ptr = create_file_event (file_ptr->fd);
		  async_queue_event (file_event_ptr);
		}
	      file_ptr->ready_mask = (gdb_notifier.poll_fds + i)->revents;
	    }
	}
    }
#else
    {
      for (file_ptr = gdb_notifier.first_file_handler;
	   (file_ptr != NULL) && (num_found > 0);
	   file_ptr = file_ptr->next_file)
	{
	  int mask = 0;

	  if (FD_ISSET (file_ptr->fd, &gdb_notifier.ready_masks[0]))
	    mask |= GDB_READABLE;
	  if (FD_ISSET (file_ptr->fd, &gdb_notifier.ready_masks[1]))
	    mask |= GDB_WRITABLE;
	  if (FD_ISSET (file_ptr->fd, &gdb_notifier.ready_masks[2]))
	    mask |= GDB_EXCEPTION;

	  if (!mask)
	    continue;
	  else
	    num_found--;

	  /* Enqueue an event only if this is still a new event for
	     this fd. */

	  if (file_ptr->ready_mask == 0)
	    {
	      file_event_ptr = create_file_event (file_ptr->fd);
	      async_queue_event (file_event_ptr);
	    }
	  file_ptr->ready_mask = mask;
	}
    }
#endif /* HAVE_POLL */
  return 0;
}
