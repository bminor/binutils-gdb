/*  This file is part of the program psim.

    Copyright (C) 1994-1998, Andrew Cagney <cagney@highland.com.au>

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


#include "sim-main.h"

#include "hw-device.h"
#include "hw-properties.h"

#if HAVE_STDLIB_H
#include <stdlib.h>
#endif


/* Address methods */

const hw_unit *
hw_unit_address (struct hw *me)
{
  return &me->unit_address_of_hw;
}



/* IOCTL: */

int
hw_ioctl (struct hw *me,
	  sim_cpu *processor,
	  sim_cia cia,
	  hw_ioctl_request request,
	  ...)
{
  int status;
  va_list ap;
  va_start(ap, request);
  status = me->to_ioctl (me, processor, cia, request, ap);
  va_end(ap);
  return status;
}
      
/* I/O */

void volatile
hw_abort (struct hw *me,
	  const char *fmt,
	  ...)
{
  SIM_DESC sd;
  const char *name;
  va_list ap;
  va_start(ap, fmt);
  /* find a system to abort through */
  if (me == NULL || hw_system (me) == NULL)
    sd = NULL;
  else
    sd = hw_system (me);
  /* find an identity */
  if (me != NULL && hw_path (me) != NULL && hw_path (me) [0] != '\0')
    name = hw_path (me);
  else if (me != NULL && hw_name (me) != NULL && hw_name (me)[0] != '\0')
    name = hw_name (me);
  else if (me != NULL && hw_family (me) != NULL && hw_family (me)[0] != '\0')
    name = hw_family (me);
  else
    name = "device";
  /* report the problem */
  sim_io_eprintf (sd, "%s: ", name);
  sim_io_evprintf (sd, fmt, ap);
  sim_io_error (sd, "%s", "");
}


/* The event queue abstraction (for devices) */


struct _hw_event {
  void *data;
  struct hw *me;
  hw_event_handler *handler;
  sim_event *real;
};

/* Pass the H/W event onto the real handler */

static void
bounce_hw_event (SIM_DESC sd,
		 void *data)
{
  hw_event event = * (hw_event*) data;
  zfree (data);
  event.handler (event.me, event.data);
}


/* Map onto the event functions */

hw_event *
hw_event_queue_schedule (struct hw *me,
			 signed64 delta_time,
			 hw_event_handler *handler,
			 void *data)
{
  hw_event *event = ZALLOC (hw_event);
  event->data = data;
  event->handler = handler;
  event->me = me;
  event->real = sim_events_schedule (hw_system (me),
				     delta_time,
				     bounce_hw_event,
				     event);
  return event;
}

void
hw_event_queue_deschedule (struct hw *me,
			   hw_event *event_to_remove)
{
  sim_events_deschedule (hw_system (me),
			 event_to_remove->real);
  zfree (event_to_remove);
}

signed64
hw_event_queue_time (struct hw *me)
{
  return sim_events_time (hw_system (me));
}


/* Mechanism for associating allocated memory regions to a device.
   When a device is deleted any remaining memory regions are also
   reclaimed.

   FIXME: Perhaphs this can be generalized, perhaphs it should not
   be. */

struct hw_alloc_data {
  void *alloc;
  int zalloc_p;
  struct hw_alloc_data *next;
};

extern void *
hw_zalloc (struct hw *me, unsigned long size)
{
  struct hw_alloc_data *memory = ZALLOC (struct hw_alloc_data);
  memory->alloc = zalloc (size);
  memory->zalloc_p = 1;
  memory->next = me->alloc_of_hw;
  me->alloc_of_hw = memory;
  return memory->alloc;
}

extern void *
hw_malloc (struct hw *me, unsigned long size)
{
  struct hw_alloc_data *memory = ZALLOC (struct hw_alloc_data);
  memory->alloc = zalloc (size);
  memory->zalloc_p = 0;
  memory->next = me->alloc_of_hw;
  me->alloc_of_hw = memory;
  return memory->alloc;
}

extern void
hw_free (struct hw *me,
	 void *alloc)
{
  struct hw_alloc_data **memory;
  for (memory = &me->alloc_of_hw;
       *memory != NULL;
       memory = &(*memory)->next)
    {
      if ((*memory)->alloc == alloc)
	{
	  struct hw_alloc_data *die = (*memory);
	  (*memory) = die->next;
	  if (die->zalloc_p)
	    zfree (die->alloc);
	  else
	    free (die->alloc);
	  zfree (die);
	  return;
	}
    }
  hw_abort (me, "free of memory not belonging to a device");
}

extern void
hw_free_all (struct hw *me)
{
  while (me->alloc_of_hw != NULL)
    {
      hw_free (me, me->alloc_of_hw->alloc);
    }
}
