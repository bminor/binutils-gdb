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


#ifndef _SIM_EVENTS_H_
#define _SIM_EVENTS_H_


typedef void event_handler(void *data);

typedef struct _event_entry event_entry;
struct _event_entry {
  void *data;
  event_handler *handler;
  signed64 time_of_event;  
  event_entry *next;
};

typedef struct _event_queue event_queue;
struct _event_queue {
  int processing;
  event_entry *queue;
  event_entry *volatile held;
  event_entry *volatile *volatile held_end;
  signed64 time_of_event;
  signed64 time_from_event;
  int trace;
};

typedef struct event_entry *event_entry_tag;


/* Initialization */

INLINE_SIM_EVENTS\
(void) event_queue_init
(engine *system);


/* Tracing level */

INLINE_SIM_EVENTS\
(void) event_queue_trace
(engine *system,
 int level);


/* (de)Schedule things to happen in the future. */

INLINE_SIM_EVENTS\
(event_entry_tag) event_queue_schedule
(engine *system,
 signed64 delta_time,
 event_handler *handler,
 void *data);

INLINE_SIM_EVENTS\
(event_entry_tag) event_queue_schedule_after_signal
(engine *system,
 signed64 delta_time,
 event_handler *handler,
 void *data);

INLINE_SIM_EVENTS\
(void) event_queue_deschedule
(engine *system,
 event_entry_tag event_to_remove);


/* progress time.  Broken into two parts so that if something is
   pending, the caller has a chance to save any cached state */

INLINE_SIM_EVENTS\
(int) event_queue_tick
(engine *system);

INLINE_SIM_EVENTS\
(void) event_queue_process
(engine *system);


/* local concept of time */

INLINE_SIM_EVENTS\
(signed64) event_queue_time
(engine *system);


#endif
