/* Event loop machinery for GDB, the GNU debugger.
   Copyright 1999 Free Software Foundation, Inc.
   Written by Elena Zannoni <ezannoni@cygnus.com> of Cygnus Solutions.

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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. */

#include "event-loop.h"
#include <readline/readline.h>
#include <setjmp.h>
#include "top.h"

/* For config.h which may define HAVE_POLL */
#include "defs.h"

#ifdef HAVE_POLL
#include <sys/poll.h>
#endif
#include <errno.h>

/* Event queue:  
   - the first event in the queue is the head of the queue. 
   It will be the next to be serviced.
   - the last event in the queue 

   Events can be inserted at the front of the queue or at the end of
   the queue.  Events will be extracted from the queue for processing
   starting from the head.  Therefore, events inserted at the head of
   the queue will be processed in a last in first out fashoin, while
   those inserted at the tail of the queue will be processed in a first
   in first out manner.  All the fields are NULL if the queue is
   empty. */

static struct
  {
    gdb_event *first_event;	/* First pending event */
    gdb_event *last_event;	/* Last pending event */
  }
event_queue;

/* Gdb_notifier is just a list of file descriptors gdb is interested in.
   These are the input file descriptor, and the target file
   descriptor. We have two flavors of the notifier, one for platforms
   that have the POLL function, the other for those that don't, and
   only support SELECT. Each of the elements in the gdb_notifier list is
   basically a description of what kind of events gdb is interested
   in, for each fd. */

/* As of 4/30/99 only the input file descriptor is registered with the
   event loop. */

#ifdef HAVE_POLL
/* Poll based implementation of the notifier. */

static struct
  {
    /* Ptr to head of file handler list. */
    file_handler *first_file_handler;

    /* Ptr to array of pollfd structures. */
    struct pollfd *poll_fds;

    /* Number of file descriptors to monitor. */
    int num_fds;

  }
gdb_notifier;

#else /* ! HAVE_POLL */

/* Select based implementation of the notifier. */

static struct
  {
    /* Ptr to head of file handler list. */
    file_handler *first_file_handler;

    /* Masks to be used in the next call to select.
       Bits are set in response to calls to create_file_handler. */
    fd_mask check_masks[3 * MASK_SIZE];

    /* What file descriptors were found ready by select. */
    fd_mask ready_masks[3 * MASK_SIZE];

    /* Number of valid bits (highest fd value + 1). */
    int num_fds;

  }
gdb_notifier;

#endif /* HAVE_POLL */

/* All the async_signal_handlers gdb is interested in are kept onto
   this list. */
static struct
  {
    /* Pointer to first in handler list. */
    async_signal_handler *first_handler;     
    
    /* Pointer to last in handler list. */
    async_signal_handler *last_handler;	     
  }
sighandler_list;

/* Is any of the handlers ready?  Check this variable using
   check_async_ready. This is used by process_event, to determine
   whether or not to invoke the invoke_async_signal_handler
   function. */
static int async_handler_ready = 0;

static void invoke_async_signal_handler PARAMS ((void));
static int gdb_wait_for_event PARAMS ((void));
static int check_async_ready PARAMS ((void));
extern display_gdb_prompt PARAMS ((char *));


/* Insert an event object into the gdb event queue at 
   the specified position.
   POSITION can be head or tail, with values TAIL, HEAD.
   EVENT_PTR points to the event to be inserted into the queue.
   The caller must allocate memory for the event. It is freed
   after the event has ben handled.
   Events in the queue will be processed head to tail, therefore,
   events inserted at the head of the queue will be processed
   as last in first out. Event appended at the tail of the queue
   will be processed first in first out. */
static void
async_queue_event (event_ptr, position)
     gdb_event *event_ptr;
     queue_position position;
{
  if (position == TAIL)
    {
      /* The event will become the new last_event. */

      event_ptr->next_event = NULL;
      if (event_queue.first_event == NULL)
	event_queue.first_event = event_ptr;
      else
	event_queue.last_event->next_event = event_ptr;
      event_queue.last_event = event_ptr;
    }
  else if (position == HEAD)
    {
      /* The event becomes the new first_event. */

      event_ptr->next_event = event_queue.first_event;
      if (event_queue.first_event == NULL)
	event_queue.last_event = event_ptr;
      event_queue.first_event = event_ptr;
    }
}

/* Process one event.
   The event can be the next one to be serviced in the event queue,
   or an asynchronous event handler can be invoked in response to
   the reception of a signal.
   If an event was processed (either way), 1 is returned otherwise
   0 is returned.   
   Scan the queue from head to tail, processing therefore the high
   priority events first, by invoking the associated event handler
   procedure. */
static int
process_event ()
{
  gdb_event *event_ptr, *prev_ptr;
  event_handler_func *proc;
  int fd;

  /* First let's see if there are any asynchronous event handlers that
     are ready. These would be the result of invoking any of the
     signal handlers. */

  if (check_async_ready ())
    {
      invoke_async_signal_handler ();
      return 1;
    }

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
      free ((char *) event_ptr);

      /* Now call the procedure associted with the event. */
      (*proc) (fd);
      return 1;
    }

  /* this is the case if there are no event on the event queue. */
  return 0;
}

/* Process one high level event.  If nothing is ready at this time,
   wait for something to happen (via gdb_wait_for_event), then process
   it.  Returns 1 if something was done otherwise returns 0 (this can
   happen if there are no event sources to wait for). */
int
gdb_do_one_event ()
{
  int result = 0;

  while (1)
    {
      if (!SET_TOP_LEVEL ())
	{
	  /* Any events already waiting in the queue? */
	  if (process_event ())
	    {
	      result = 1;
	      break;
	    }

	  /* Wait for a new event.  If gdb_wait_for_event returns -1,
	     we should get out because this means that there are no
	     event sources left. This will make the event loop stop,
	     and the application exit. */

	  result = gdb_wait_for_event ();
	  if (result < 0)
	    {
	      result = 0;
	      break;
	    }

	  /* Handle any new events occurred while waiting. */
	  if (process_event ())
	    {
	      result = 1;
	      break;
	    }

	  /* If gdb_wait_for_event has returned 1, it means that one
	     event has been handled. We break out of the loop. */
	  if (result)
	    break;
	}			/* end of if !set_top_level */
      else
	{
	  display_gdb_prompt (0);
	  /* Maybe better to set a flag to be checked somewhere as to
	     whether display the prompt or not. */
	}
    }
  return result;
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
void
create_file_handler (fd, mask, proc, client_data)
     int fd;
     int mask;
     file_handler_func *proc;
     gdb_client_data client_data;
{
  file_handler *file_ptr;

#ifndef HAVE_POLL
  int index, bit;
#endif

  /* Do we already have a file handler for this file? (We may be
     changing its associated procedure). */
  for (file_ptr = gdb_notifier.first_file_handler; file_ptr != NULL;
       file_ptr = file_ptr->next_file)
    {
      if (file_ptr->fd == fd)
	break;
    }

  /* It is a new file descriptor. */
  if (file_ptr == NULL)
    {
      file_ptr = (file_handler *) xmalloc (sizeof (file_handler));
      file_ptr->fd = fd;
      file_ptr->ready_mask = 0;
      file_ptr->next_file = gdb_notifier.first_file_handler;
      gdb_notifier.first_file_handler = file_ptr;
    }
  file_ptr->proc = proc;
  file_ptr->client_data = client_data;
  file_ptr->mask = mask;

#ifdef HAVE_POLL

  gdb_notifier.num_fds++;
  gdb_notifier.poll_fds =
    (struct pollfd *) realloc (gdb_notifier.poll_fds,
			   (gdb_notifier.num_fds) * sizeof (struct pollfd));
  (gdb_notifier.poll_fds + gdb_notifier.num_fds - 1)->fd = fd;
  (gdb_notifier.poll_fds + gdb_notifier.num_fds - 1)->events = mask;
  (gdb_notifier.poll_fds + gdb_notifier.num_fds - 1)->revents = 0;

#else /* ! HAVE_POLL */

  index = fd / (NBBY * sizeof (fd_mask));
  bit = 1 << (fd % (NBBY * sizeof (fd_mask)));

  if (mask & GDB_READABLE)
    gdb_notifier.check_masks[index] |= bit;
  else
    gdb_notifier.check_masks[index] &= ~bit;

  if (mask & GDB_WRITABLE)
    (gdb_notifier.check_masks + MASK_SIZE)[index] |= bit;
  else
    (gdb_notifier.check_masks + MASK_SIZE)[index] &= ~bit;

  if (mask & GDB_EXCEPTION)
    (gdb_notifier.check_masks + 2 * (MASK_SIZE))[index] |= bit;
  else
    (gdb_notifier.check_masks + 2 * (MASK_SIZE))[index] &= ~bit;

  if (gdb_notifier.num_fds <= fd)
    gdb_notifier.num_fds = fd + 1;

#endif /* HAVE_POLL */
}

/* Remove the file descriptor FD from the list of monitored fd's: 
   i.e. we don't care anymore about events on the FD. */
void
delete_file_handler (fd)
     int fd;
{
  file_handler *file_ptr, *prev_ptr = NULL;
  int i, j;
  struct pollfd *new_poll_fds;
#ifndef HAVE_POLL
  int index, bit;
  unsigned long flags;
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

  /* Deactivate the file descriptor, by clearing its mask, 
     so that it will not fire again. */

  file_ptr->mask = 0;

#ifdef HAVE_POLL
  /* Create a new poll_fds array by copying every fd's information but the
     one we want to get rid of. */

  new_poll_fds =
    (struct pollfd *) xmalloc ((gdb_notifier.num_fds - 1) * sizeof (struct pollfd));

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

#else /* ! HAVE_POLL */

  index = fd / (NBBY * sizeof (fd_mask));
  bit = 1 << (fd % (NBBY * sizeof (fd_mask)));

  if (file_ptr->mask & GDB_READABLE)
    gdb_notifier.check_masks[index] &= ~bit;
  if (file_ptr->mask & GDB_WRITABLE)
    (gdb_notifier.check_masks + MASK_SIZE)[index] &= ~bit;
  if (file_ptr->mask & GDB_EXCEPTION)
    (gdb_notifier.check_masks + 2 * (MASK_SIZE))[index] &= ~bit;

  /* Find current max fd. */

  if ((fd + 1) == gdb_notifier.num_fds)
    {
      for (gdb_notifier.num_fds = 0; index >= 0; index--)
	{
	  flags = gdb_notifier.check_masks[index]
	    | (gdb_notifier.check_masks + MASK_SIZE)[index]
	    | (gdb_notifier.check_masks + 2 * (MASK_SIZE))[index];
	  if (flags)
	    {
	      for (i = (NBBY * sizeof (fd_mask)); i > 0; i--)
		{
		  if (flags & (((unsigned long) 1) << (i - 1)))
		    break;
		}
	      gdb_notifier.num_fds = index * (NBBY * sizeof (fd_mask)) + i;
	      break;
	    }
	}
    }
#endif /* HAVE_POLL */

  /* Get rid of the file handler in the file handler list. */
  if (file_ptr == gdb_notifier.first_file_handler)
    gdb_notifier.first_file_handler = file_ptr->next_file;
  else
    {
      for (prev_ptr = gdb_notifier.first_file_handler;
	   prev_ptr->next_file == file_ptr;
	   prev_ptr = prev_ptr->next_file)
	;
      prev_ptr->next_file = file_ptr->next_file;
    }
  free ((char *) file_ptr);
}

/* Handle the given event by calling the procedure associated to the
   corresponding file handler.  Called by process_event indirectly,
   through event_ptr->proc.  EVENT_FILE_DESC is file descriptor of the
   event in the front of the event queue. */
static void
handle_file_event (event_file_desc)
     int event_file_desc;
{
  file_handler *file_ptr;
  int mask, error_mask;

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

#else /* ! HAVE_POLL */
	  mask = file_ptr->ready_mask & file_ptr->mask;
#endif /* HAVE_POLL */

	  /* Clear the received events for next time around. */
	  file_ptr->ready_mask = 0;

	  /* If there was a match, then call the handler. */
	  if (mask != 0)
	    (*file_ptr->proc) (file_ptr->client_data, mask);
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
gdb_wait_for_event ()
{
  file_handler *file_ptr;
  gdb_event *file_event_ptr;
  int num_found, i;

#ifndef HAVE_POLL
  int mask, bit, index;
#endif

  if (gdb_notifier.num_fds == 0)
    return -1;

#ifdef HAVE_POLL
  num_found =
    poll (gdb_notifier.poll_fds, (unsigned long) gdb_notifier.num_fds, -1);

#else /* ! HAVE_POLL */
  memcpy (gdb_notifier.ready_masks,
	  gdb_notifier.check_masks,
	  3 * MASK_SIZE * sizeof (fd_mask));
  num_found = select (gdb_notifier.num_fds,
		      (SELECT_MASK *) & gdb_notifier.ready_masks[0],
		      (SELECT_MASK *) & gdb_notifier.ready_masks[MASK_SIZE],
		  (SELECT_MASK *) & gdb_notifier.ready_masks[2 * MASK_SIZE],
		      NULL);

  /* Clear the masks after an error from select. */
  if (num_found == -1)
    memset (gdb_notifier.ready_masks,
	    0, 3 * MASK_SIZE * sizeof (fd_mask));

#endif /* HAVE_POLL */

  /* Enqueue all detected file events. */

#ifdef HAVE_POLL

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
	      file_event_ptr =
		(gdb_event *) xmalloc (sizeof (gdb_event));
	      file_event_ptr->proc = handle_file_event;
	      file_event_ptr->fd = file_ptr->fd;
	      async_queue_event (file_event_ptr, TAIL);
	    }
	}

      file_ptr->ready_mask = (gdb_notifier.poll_fds + i)->revents;
    }

#else /* ! HAVE_POLL */
  for (file_ptr = gdb_notifier.first_file_handler;
       (file_ptr != NULL) && (num_found > 0);
       file_ptr = file_ptr->next_file)
    {
      index = file_ptr->fd / (NBBY * sizeof (fd_mask));
      bit = 1 << (file_ptr->fd % (NBBY * sizeof (fd_mask)));
      mask = 0;

      if (gdb_notifier.ready_masks[index] & bit)
	mask |= GDB_READABLE;
      if ((gdb_notifier.ready_masks + MASK_SIZE)[index] & bit)
	mask |= GDB_WRITABLE;
      if ((gdb_notifier.ready_masks + 2 * (MASK_SIZE))[index] & bit)
	mask |= GDB_EXCEPTION;

      if (!mask)
	continue;
      else
	num_found--;

      /* Enqueue an event only if this is still a new event for
         this fd. */

      if (file_ptr->ready_mask == 0)
	{
	  file_event_ptr =
	    (gdb_event *) xmalloc (sizeof (gdb_event));
	  file_event_ptr->proc = handle_file_event;
	  file_event_ptr->fd = file_ptr->fd;
	  async_queue_event (file_event_ptr, TAIL);
	}
      file_ptr->ready_mask = mask;
    }
#endif /* HAVE_POLL */

  return 0;
}


/* Create an asynchronous handler, allocating memory for it. 
   Return a pointer to the newly created handler.
   This pointer will be used to invoke the handler by 
   invoke_async_signal_handler.
   PROC is the function to call with CLIENT_DATA argument 
   whenever the handler is invoked. */
async_signal_handler *
create_async_signal_handler (proc, client_data)
     async_handler_func *proc;
     gdb_client_data client_data;
{
  async_signal_handler *async_handler_ptr;

  async_handler_ptr =
    (async_signal_handler *) xmalloc (sizeof (async_signal_handler));
  async_handler_ptr->ready = 0;
  async_handler_ptr->next_handler = NULL;
  async_handler_ptr->proc = proc;
  async_handler_ptr->client_data = client_data;
  if (sighandler_list.first_handler == NULL)
    sighandler_list.first_handler = async_handler_ptr;
  else
    sighandler_list.last_handler->next_handler = async_handler_ptr;
  sighandler_list.last_handler = async_handler_ptr;
  return async_handler_ptr;
}

/* Mark the handler (ASYNC_HANDLER_PTR) as ready. This information will
   be used when the handlers are invoked, after we have waited for
   some event.  The caller of this function is the interrupt handler
   associated with a signal. */
void
mark_async_signal_handler (async_handler_ptr)
     async_signal_handler *async_handler_ptr;
{
  ((async_signal_handler *) async_handler_ptr)->ready = 1;
  async_handler_ready = 1;
}

/* Call all the handlers that are ready. */
static void
invoke_async_signal_handler ()
{
  async_signal_handler *async_handler_ptr;

  if (async_handler_ready == 0)
    return;
  async_handler_ready = 0;

  /* Invoke ready handlers. */

  while (1)
    {
      for (async_handler_ptr = sighandler_list.first_handler; 
	   async_handler_ptr != NULL;
	   async_handler_ptr = async_handler_ptr->next_handler)
	{
	  if (async_handler_ptr->ready)
	    break;
	}
      if (async_handler_ptr == NULL)
	break;
      async_handler_ptr->ready = 0;
      (*async_handler_ptr->proc) (async_handler_ptr->client_data);
    }

  return;
}

/* Delete an asynchronous handler (ASYNC_HANDLER_PTR). 
   Free the space allocated for it.  */
void
delete_async_signal_handler (async_handler_ptr)
     async_signal_handler *async_handler_ptr;
{
  async_signal_handler *prev_ptr;

  if (sighandler_list.first_handler == async_handler_ptr)
    {
      sighandler_list.first_handler = async_handler_ptr->next_handler;
      if (sighandler_list.first_handler == NULL)
	sighandler_list.last_handler = NULL;
    }
  else
    {
      prev_ptr = sighandler_list.first_handler;
      while (prev_ptr->next_handler != async_handler_ptr)
	prev_ptr = prev_ptr->next_handler;
      prev_ptr->next_handler = async_handler_ptr->next_handler;
      if (sighandler_list.last_handler == async_handler_ptr)
	sighandler_list.last_handler = prev_ptr;
    }
  free ((char *) async_handler_ptr);
}

/* Is it necessary to call invoke_async_signal_handler? */
static int
check_async_ready ()
{
  return async_handler_ready;
}
