/* Hardware event manager.
   Copyright (C) 1998 Free Software Foundation, Inc.
   Contributed by Cygnus Support.

This file is part of GDB, the GNU debugger.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */


#include "sim-main.h"
#include "hw-base.h"

#include "sim-events.h"


/* The hw-events object is implemented using sim-events */

struct hw_event {
  void *data;
  struct hw *me;
  hw_event_callback *callback;
  sim_event *real;
  struct hw_event_data *entry;
};

struct hw_event_data {
  struct hw_event event;
  struct hw_event_data *next;
  struct hw_event_data **prev;
};

void
create_hw_event_data (struct hw *me)
{
  /* NOP */
}

void
delete_hw_event_data (struct hw *me)
{
  if (me->events_of_hw != NULL)
    hw_abort (me, "stray events");
}


static void
delete_hw_event (struct hw *me,
		 struct hw_event **event)
{
  struct hw_event_data *entry = (*event)->entry;
  *(entry->prev) = entry->next;
  entry->next->prev = entry->prev;
  (*event) = NULL;
}


static void
create_hw_event (struct hw *me,
		 struct hw_event **event)
{
  struct hw_event_data *entry = HW_ZALLOC (me, struct hw_event_data);
  entry->next = me->events_of_hw;
  entry->prev = &me->events_of_hw;
  me->events_of_hw->prev = &entry->next;
  me->events_of_hw = entry;
  (*event) = &entry->event;
}



/* Pass the H/W event onto the real callback */

static void
bounce_hw_event (SIM_DESC sd,
		 void *data)
{
  /* save the data */
  struct hw_event *event = (struct hw_event*)data;
  struct hw *me = event->me;
  void *event_data = event->data;
  hw_event_callback *callback = event->callback;
  hw_free (me, data);
  event = NULL;
  callback (me, event_data);
}



/* Map onto the event functions */

struct hw_event *
hw_event_queue_schedule (struct hw *me,
			 signed64 delta_time,
			 hw_event_callback *callback,
			 void *data)
{
  struct hw_event *event;
  create_hw_event (me, &event);
  /* fill it in */
  event->data = data;
  event->callback = callback;
  event->me = me;
  event->real = sim_events_schedule (hw_system (me),
				     delta_time,
				     bounce_hw_event,
				     event);
  return event;
}


void
hw_event_queue_deschedule (struct hw *me,
			   struct hw_event *event_to_remove)
{
  /* remove it from the event queue */
  sim_events_deschedule (hw_system (me),
			 event_to_remove->real);
  delete_hw_event (me, &event_to_remove);
}


signed64
hw_event_queue_time (struct hw *me)
{
  return sim_events_time (hw_system (me));
}


