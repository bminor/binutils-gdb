/* Generic serial interface functions.

   Copyright 1992, 1993, 1994, 1995, 1996, 1998, 1999, 2000, 2001,
   2003, 2004, 2005 Free Software Foundation, Inc.

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
#include "serial.h"
#include "ser-unix.h"
#include "event-loop.h"

static timer_handler_func push_event;
static handler_func fd_event;

/* Event handling for ASYNC serial code.

   At any time the SERIAL device either: has an empty FIFO and is
   waiting on a FD event; or has a non-empty FIFO/error condition and
   is constantly scheduling timer events.

   ASYNC only stops pestering its client when it is de-async'ed or it
   is told to go away. */

/* Value of scb->async_state: */
enum {
  /* >= 0 (TIMER_SCHEDULED) */
  /* The ID of the currently scheduled timer event. This state is
     rarely encountered.  Timer events are one-off so as soon as the
     event is delivered the state is shanged to NOTHING_SCHEDULED. */
  FD_SCHEDULED = -1,
  /* The fd_event() handler is scheduled.  It is called when ever the
     file descriptor becomes ready. */
  NOTHING_SCHEDULED = -2
  /* Either no task is scheduled (just going into ASYNC mode) or a
     timer event has just gone off and the current state has been
     forced into nothing scheduled. */
};

/* Identify and schedule the next ASYNC task based on scb->async_state
   and scb->buf* (the input FIFO).  A state machine is used to avoid
   the need to make redundant calls into the event-loop - the next
   scheduled task is only changed when needed. */

void
reschedule (struct serial *scb)
{
  if (serial_is_async_p (scb))
    {
      int next_state;
      switch (scb->async_state)
	{
	case FD_SCHEDULED:
	  if (scb->bufcnt == 0)
	    next_state = FD_SCHEDULED;
	  else
	    {
	      delete_file_handler (scb->fd);
	      next_state = create_timer (0, push_event, scb);
	    }
	  break;
	case NOTHING_SCHEDULED:
	  if (scb->bufcnt == 0)
	    {
	      add_file_handler (scb->fd, fd_event, scb);
	      next_state = FD_SCHEDULED;
	    }
	  else
	    {
	      next_state = create_timer (0, push_event, scb);
	    }
	  break;
	default: /* TIMER SCHEDULED */
	  if (scb->bufcnt == 0)
	    {
	      delete_timer (scb->async_state);
	      add_file_handler (scb->fd, fd_event, scb);
	      next_state = FD_SCHEDULED;
	    }
	  else
	    next_state = scb->async_state;
	  break;
	}
      if (serial_debug_p (scb))
	{
	  switch (next_state)
	    {
	    case FD_SCHEDULED:
	      if (scb->async_state != FD_SCHEDULED)
		fprintf_unfiltered (gdb_stdlog, "[fd%d->fd-scheduled]\n",
				    scb->fd);
	      break;
	    default: /* TIMER SCHEDULED */
	      if (scb->async_state == FD_SCHEDULED)
		fprintf_unfiltered (gdb_stdlog, "[fd%d->timer-scheduled]\n",
				    scb->fd);
	      break;
	    }
	}
      scb->async_state = next_state;
    }
}

/* FD_EVENT: This is scheduled when the input FIFO is empty (and there
   is no pending error).  As soon as data arrives, it is read into the
   input FIFO and the client notified.  The client should then drain
   the FIFO using readchar().  If the FIFO isn't immediatly emptied,
   push_event() is used to nag the client until it is. */

static void
fd_event (int error, void *context)
{
  struct serial *scb = context;
  if (error != 0)
    {
      scb->bufcnt = SERIAL_ERROR;
    }
  else if (scb->bufcnt == 0)
    {
      /* Prime the input FIFO.  The readchar() function is used to
         pull characters out of the buffer.  See also
         generic_readchar(). */
      int nr;
      do
	{
	  nr = read (scb->fd, scb->buf, BUFSIZ);
	}
      while (nr == -1 && errno == EINTR);
      if (nr == 0)
	{
	  scb->bufcnt = SERIAL_EOF;
	}
      else if (nr > 0)
	{
	  scb->bufcnt = nr;
	  scb->bufp = scb->buf;
	}
      else
	{
	  scb->bufcnt = SERIAL_ERROR;
	}
    }
  scb->async_handler (scb, scb->async_context);
  reschedule (scb);
}

/* PUSH_EVENT: The input FIFO is non-empty (or there is a pending
   error).  Nag the client until all the data has been read.  In the
   case of errors, the client will need to close or de-async the
   device before naging stops. */

static void
push_event (void *context)
{
  struct serial *scb = context;
  scb->async_state = NOTHING_SCHEDULED; /* Timers are one-off */
  scb->async_handler (scb, scb->async_context);
  /* re-schedule */
  reschedule (scb);
}

int
ser_base_write (struct serial *scb, const char *str, int len)
{
  int cc;

  while (len > 0)
    {
      cc = write (scb->fd, str, len);

      if (cc < 0)
	return 1;
      len -= cc;
      str += cc;
    }
  return 0;
}

int
ser_base_flush_output (struct serial *scb)
{
  return 0;
}

int
ser_base_flush_input (struct serial *scb)
{
  if (scb->bufcnt >= 0)
    {
      scb->bufcnt = 0;
      scb->bufp = scb->buf;
      return 0;
    }
  else
    return SERIAL_ERROR;
}

int
ser_base_send_break (struct serial *scb)
{
  return 0;
}

int
ser_base_drain_output (struct serial *scb)
{
  return 0;
}

void
ser_base_raw (struct serial *scb)
{
  return;			/* Always in raw mode */
}

serial_ttystate
ser_base_get_tty_state (struct serial *scb)
{
  /* allocate a dummy */
  return (serial_ttystate) XMALLOC (int);
}

int
ser_base_set_tty_state (struct serial *scb, serial_ttystate ttystate)
{
  return 0;
}

int
ser_base_noflush_set_tty_state (struct serial *scb,
				serial_ttystate new_ttystate,
				serial_ttystate old_ttystate)
{
  return 0;
}

void
ser_base_print_tty_state (struct serial *scb, 
			  serial_ttystate ttystate,
			  struct ui_file *stream)
{
  /* Nothing to print.  */
  return;
}

int
ser_base_setbaudrate (struct serial *scb, int rate)
{
  return 0;			/* Never fails! */
}

int
ser_base_setstopbits (struct serial *scb, int num)
{
  return 0;			/* Never fails! */
}

/* Put the SERIAL device into/out-of ASYNC mode.  */

void
ser_base_async (struct serial *scb,
		int async_p)
{
  if (async_p)
    {
      /* Force a re-schedule. */
      scb->async_state = NOTHING_SCHEDULED;
      if (serial_debug_p (scb))
	fprintf_unfiltered (gdb_stdlog, "[fd%d->asynchronous]\n",
			    scb->fd);
      reschedule (scb);
    }
  else
    {
      if (serial_debug_p (scb))
	fprintf_unfiltered (gdb_stdlog, "[fd%d->synchronous]\n",
			    scb->fd);
      /* De-schedule whatever tasks are currently scheduled. */
      switch (scb->async_state)
	{
	case FD_SCHEDULED:
	  delete_file_handler (scb->fd);
	  break;
	case NOTHING_SCHEDULED:
	  break;
	default: /* TIMER SCHEDULED */
	  delete_timer (scb->async_state);
	  break;
	}
    }
}
