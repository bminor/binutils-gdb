/*  This file is part of the program psim.

    Copyright (C) 1994-1997, Andrew Cagney <cagney@highland.com.au>

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
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 
    */


#ifndef _SIM_EVENTS_C_
#define _SIM_EVENTS_C_

#include "sim-main.h"
#include "sim-assert.h"

#include <signal.h>


/* The event queue maintains a single absolute time using two
   variables.
   
   TIME_OF_EVENT: this holds the time at which the next event is ment
   to occure.  If no next event it will hold the time of the last
   event.

   TIME_FROM_EVENT: The current distance from TIME_OF_EVENT.  If an
   event is pending, this will be positive.  If no future event is
   pending this will be negative.  This variable is decremented once
   for each iteration of a clock cycle.

   Initially, the clock is started at time one (0) with TIME_OF_EVENT
   == 0 and TIME_FROM_EVENT == 0.

   Clearly there is a bug in that this code assumes that the absolute
   time counter will never become greater than 2^62.

   To avoid the need to use 64bit arithmetic, the event queue always
   contains at least one event scheduled every 16 000 ticks.  This
   limits the time from event counter to values less than
   16 000. */


#if !defined (SIM_EVENTS_POLL_RATE)
#define SIM_EVENTS_POLL_RATE 0x4000
#endif


#define _ETRACE sd

#undef ETRACE
#define ETRACE(ARGS) \
do \
  { \
    if (WITH_TRACE) \
      { \
        if (sd->events.trace) \
          { \
            const char *file; \
            SIM_FILTER_PATH(file, __FILE__); \
            sim_io_printf (sd, "%s:%d: ", file, __LINE__); \
            sim_io_printf  ARGS; \
          } \
      } \
  } \
while (0)


STATIC_INLINE_SIM_EVENTS\
(void)
sim_events_poll (void *data)
{
  /* just re-schedule in 1000 million ticks time */
  SIM_DESC sd = data;
  sim_events_schedule(sd, SIM_EVENTS_POLL_RATE, sim_events_poll, sd);
  sim_io_poll_quit (sd);
}


INLINE_SIM_EVENTS\
(void)
sim_events_init(SIM_DESC sd)
{
  sim_events *events = &sd->events;
  sim_event *event;

  /* drain the interrupt queue */
  {
#if defined(HAVE_SIGPROCMASK) && defined(SIG_SETMASK)
    sigset_t old_mask;
    sigset_t new_mask;
    sigfillset(&new_mask);
    /*-LOCK-*/ sigprocmask(SIG_SETMASK, &new_mask, &old_mask);
#endif
    event = events->held;
    while (event != NULL) {
      sim_event *dead = event;
      event = event->next;
      zfree(dead);
    }
    events->held = NULL;
    events->held_end = &events->held;
#if defined(HAVE_SIGPROCMASK) && defined(SIG_SETMASK)
    /*-UNLOCK-*/ sigprocmask(SIG_SETMASK, &old_mask, NULL);
#endif
  }

  /* drain the normal queue */
  event = events->queue;
  while (event != NULL) {
    sim_event *dead = event;
    event = event->next;
    zfree(dead);
  }
  events->queue = NULL;
    
  /* wind time back to zero */
  events->processing = 0;
  events->time_of_event = 0;
  events->time_from_event = 0;

  /* schedule our initial counter event */
  sim_events_schedule(sd, 0, sim_events_poll, sd);

  /* from now on, except when the large-int event is being processed
     the event queue is non empty */
  SIM_ASSERT(events->queue != NULL);
}

INLINE_SIM_EVENTS\
(signed64)
sim_events_time(SIM_DESC sd)
{
  sim_events *events = &sd->events;
  return events->time_of_event - events->time_from_event;
}

STATIC_INLINE_SIM_EVENTS\
(void)
update_time_from_event(SIM_DESC sd)
{
  sim_events *events = &sd->events;
  signed64 current_time = sim_events_time(sd);
  if (events->queue != NULL) {
    events->time_from_event = (events->queue->time_of_event - current_time);
    events->time_of_event = events->queue->time_of_event;
  }
  else {
    events->time_of_event = current_time - 1;
    events->time_from_event = -1;
  }
  SIM_ASSERT(current_time == sim_events_time (sd));
  SIM_ASSERT((events->time_from_event >= 0) == (events->queue != NULL));
}

STATIC_INLINE_SIM_EVENTS\
(void)
insert_sim_event(SIM_DESC sd,
		 sim_event *new_event,
		 signed64 delta)
{
  sim_events *events = &sd->events;
  sim_event *curr;
  sim_event **prev;
  signed64 time_of_event;

  if (delta < 0)
    engine_error (sd, "what is past is past!\n");

  /* compute when the event should occure */
  time_of_event = sim_events_time(sd) + delta;

  /* find the queue insertion point - things are time ordered */
  prev = &events->queue;
  curr = events->queue;
  while (curr != NULL && time_of_event >= curr->time_of_event) {
    SIM_ASSERT(curr->next == NULL
	   || curr->time_of_event <= curr->next->time_of_event);
    prev = &curr->next;
    curr = curr->next;
  }
  SIM_ASSERT(curr == NULL || time_of_event < curr->time_of_event);

  /* insert it */
  new_event->next = curr;
  *prev = new_event;
  new_event->time_of_event = time_of_event;

  /* adjust the time until the first event */
  update_time_from_event(sd);
}

INLINE_SIM_EVENTS\
(sim_event *)
sim_events_schedule(SIM_DESC sd,
		    signed64 delta_time,
		    sim_event_handler *handler,
		    void *data)
{
  sim_event *new_event = ZALLOC(sim_event);
  new_event->data = data;
  new_event->handler = handler;
  insert_sim_event(sd, new_event, delta_time);
  ETRACE((_ETRACE,
	  "event scheduled at %ld - tag 0x%lx - time %ld, handler 0x%lx, data 0x%lx\n",
	  (long)sim_events_time(sd),
	  (long)new_event,
	  (long)new_event->time_of_event,
	  (long)new_event->handler,
	  (long)new_event->data));
  return new_event;
}


INLINE_SIM_EVENTS\
(sim_event *)
sim_events_schedule_after_signal(SIM_DESC sd,
				 signed64 delta_time,
				 sim_event_handler *handler,
				 void *data)
{
  sim_events *events = &sd->events;
  sim_event *new_event = ZALLOC(sim_event);

  new_event->data = data;
  new_event->handler = handler;
  new_event->time_of_event = delta_time; /* work it out later */
  new_event->next = NULL;

  {
#if defined(HAVE_SIGPROCMASK) && defined(SIG_SETMASK)
    sigset_t old_mask;
    sigset_t new_mask;
    sigfillset(&new_mask);
    /*-LOCK-*/ sigprocmask(SIG_SETMASK, &new_mask, &old_mask);
#endif
    if (events->held == NULL) {
      events->held = new_event;
    }
    else {
      *events->held_end = new_event;
    }
    events->held_end = &new_event->next;
#if defined(HAVE_SIGPROCMASK) && defined(SIG_SETMASK)
    /*-UNLOCK-*/ sigprocmask(SIG_SETMASK, &old_mask, NULL);
#endif
  }

  ETRACE((_ETRACE,
	  "event scheduled at %ld - tag 0x%lx - time %ld, handler 0x%lx, data 0x%lx\n",
	  (long)sim_events_time(sd),
	  (long)new_event,
	  (long)new_event->time_of_event,
	  (long)new_event->handler,
	  (long)new_event->data));

  return new_event;
}


INLINE_SIM_EVENTS\
(void)
sim_events_deschedule(SIM_DESC sd,
		      sim_event *event_to_remove)
{
  sim_events *events = &sd->events;
  sim_event *to_remove = (sim_event*)event_to_remove;
  SIM_ASSERT((events->time_from_event >= 0) == (events->queue != NULL));
  if (event_to_remove != NULL) {
    sim_event *current;
    sim_event **ptr_to_current;
    for (ptr_to_current = &events->queue, current = *ptr_to_current;
	 current != NULL && current != to_remove;
	 ptr_to_current = &current->next, current = *ptr_to_current);
    if (current == to_remove) {
      *ptr_to_current = current->next;
      ETRACE((_ETRACE,
	      "event descheduled at %ld - tag 0x%lx - time %ld, handler 0x%lx, data 0x%lx\n",
	      (long)sim_events_time(sd),
	      (long)event_to_remove,
	      (long)current->time_of_event,
	      (long)current->handler,
	      (long)current->data));
      zfree(current);
      update_time_from_event(sd);
    }
    else {
      ETRACE((_ETRACE,
	      "event descheduled at %ld - tag 0x%lx - not found\n",
	      (long)sim_events_time(sd),
	      (long)event_to_remove));
    }
  }
  SIM_ASSERT((events->time_from_event >= 0) == (events->queue != NULL));
}




INLINE_SIM_EVENTS\
(int)
sim_events_tick(SIM_DESC sd)
{
  sim_events *events = &sd->events;

  /* we should only be here when the previous tick has been fully
     processed */
  SIM_ASSERT(!events->processing && events->queue != NULL);

  /* Advance the time but *only* if there is nothing to process */
  if (events->time_from_event == 0)
    return 1;
  else if (events->held != NULL)
    return 1;
  else {
    events->time_from_event -= 1;
    return 0;
  }
}



INLINE_SIM_EVENTS\
(void)
sim_events_process(SIM_DESC sd)
{
  sim_events *events = &sd->events;
  signed64 event_time = sim_events_time(sd);

  /* something to do */
  SIM_ASSERT(events->time_from_event == 0 || events->held != NULL);
  SIM_ASSERT(events->queue != NULL);

  /* move any events that were queued by any signal handlers onto the
     real event queue.  */
  if (events->held != NULL) {
    sim_event *held_events;
    sim_event *curr_event;

#if defined(HAVE_SIGPROCMASK) && defined(SIG_SETMASK)
    /*-LOCK-*/
    sigset_t old_mask;
    sigset_t new_mask;
    sigfillset(&new_mask);
    sigprocmask(SIG_SETMASK, &new_mask, &old_mask);
#endif

    held_events = events->held;
    events->held = NULL;
    events->held_end = &events->held;

#if defined(HAVE_SIGPROCMASK) && defined(SIG_SETMASK)
    /*-UNLOCK-*/
    sigprocmask(SIG_SETMASK, &old_mask, NULL);
#endif

    do {
      curr_event = held_events;
      held_events = curr_event->next;
      insert_sim_event(sd, curr_event, curr_event->time_of_event);
    } while (held_events != NULL);
  }

  /* consume all events for this or earlier times.  Be careful to
     allow a new event to appear under our feet */
  events->processing = 1;
  while (events->queue->time_of_event <= event_time) {
    sim_event *to_do = events->queue;
    sim_event_handler *handler = to_do->handler;
    void *data = to_do->data;
    events->queue = to_do->next;
    ETRACE((_ETRACE,
	    "event issued at %ld - tag 0x%lx - handler 0x%lx, data 0x%lx\n",
	    (long)event_time,
	    (long)to_do,
	    (long)handler,
	    (long)data));
    zfree (to_do);
    handler (data);
  }
  events->processing = 0;

  /* re-caculate time for new events - advance the time */
  update_time_from_event(sd);
  SIM_ASSERT(events->time_from_event > 0 && events->queue != NULL);
  events->time_from_event -= 1;
}

#endif
