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


/* Notes:

   When scheduling an event, the a delta of zero/one refers to the
   timeline as follows:

   epoch   0|1              1|2              2|3              3|
   **queue**|--insn--|*queue*|--insn--|*queue*|--insn--|*queue*|
     |   ^               ^        |       ^                ^
     `- +0 ------------ +1 --..   `----- +0 ------------- +1 --..

   When the queue is initialized, the time is set to zero with a
   number of initialization events scheduled.  Consequently, as also
   illustrated above, the event queue should be processed before the
   first instruction.  That instruction being executed during tick 1.

   The event queue is processed using:

        if (sim_events_tick (sd)) {
	  sim_events_process (sd);
	}

   */
   

typedef void sim_event_handler(void *data);

typedef struct _sim_event sim_event;
struct _sim_event {
  void *data;
  sim_event_handler *handler;
  signed64 time_of_event;  
  sim_event *next;
};

typedef struct _sim_events sim_events;
struct _sim_events {
  int processing;
  sim_event *queue;
  sim_event *volatile held;
  sim_event *volatile *volatile held_end;
  signed64 time_of_event;
  int time_from_event;
  void *path_to_halt_or_restart;
  int trace;
};



/* Initialization */

INLINE_SIM_EVENTS\
(void) sim_events_init
(SIM_DESC sd);


/* Set Tracing Level */

INLINE_SIM_EVENTS\
(void) sim_events_set_trace
(SIM_DESC sd,
 int level);


/* Schedule an event DELTA_TIME ticks into the future */

INLINE_SIM_EVENTS\
(sim_event *) sim_events_schedule
(SIM_DESC sd,
 signed64 delta_time,
 sim_event_handler *handler,
 void *data);

INLINE_SIM_EVENTS\
(sim_event *) sim_events_schedule_after_signal
(SIM_DESC sd,
 signed64 delta_time,
 sim_event_handler *handler,
 void *data);


/* Schedule an event WALLCLOCK milli-seconds from the start of the
   simulation.  The exact interpretation of wallclock is host
   dependant. */

INLINE_SIM_EVENTS\
(void) sim_events_wallclock_schedule
(SIM_DESC sd,
 signed64 wallclock_ms_time,
 sim_event_handler *handler,
 void *data);


#if 0
/* Schedule an event when the value at ADDR lies between LB..UB */

typedef enum {
  /* value host byte ordered */
  watch_host_1,
  watch_host_2,
  watch_host_4,
  watch_host_8,
  /* value target byte ordered */
  watch_targ_1,
  watch_targ_2,
  watch_targ_4,
  watch_targ_8,
  /* value big-endian */
  watch_bend_1,
  watch_bend_2,
  watch_bend_4,
  watch_bend_8,
  /* value little-endian */
  watch_lend_1,
  watch_lend_2,
  watch_lend_4,
  watch_lend_8,
} sim_watchpoint;

INLINE_SIM_EVENTS\
(void) sim_events_watchpoint_schedule
(SIM_DESC sd,
 sim_watchpoint type,
 void *addr,
 unsigned64 lb,
 unsigned64 ub,
 sim_event_handler *handler,
 void *data);
#endif


#if 0
/* Schedule an event when the value in CORE lies between LB..UB */

INLINE_SIM_EVENTS\
(void) sim_events_watchcore_schedule
(SIM_DESC sd,
 sim_watchpoint type,
 address_word addr,
 sim_core_maps map,
 unsigned64 lb,
 unsigned64 ub,
 sim_event_handler *handler,
 void *data);
#endif


/* Deschedule the specified event */

INLINE_SIM_EVENTS\
(void) sim_events_deschedule
(SIM_DESC sd,
 sim_event *event_to_remove);



/* progress time.  Broken into two parts so that if something is
   pending, the caller has a chance to save any cached state */

INLINE_SIM_EVENTS\
(int) sim_events_tick
(SIM_DESC sd);

INLINE_SIM_EVENTS\
(void) sim_events_process
(SIM_DESC sd);


/* local concept of time */

INLINE_SIM_EVENTS\
(signed64) sim_events_time
(SIM_DESC sd);


#endif
