/* Simulator watchpoint support.
   Copyright (C) 1997 Free Software Foundation, Inc.
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


#ifndef SIM_WATCH_H
#define SIM_WATCH_H

typedef enum {
  pc_watchpoint,
  clock_watchpoint,
  cycles_watchpoint,
  nr_watchpoint_types,
} watchpoint_type;

typedef enum {
  invalid_watchpoint_action,
  n_interrupt_watchpoint_action,
  interrupt_watchpoint_action,
  break_watchpoint_action,
} watchpoint_action;

typedef struct _sim_watch_point {
  watchpoint_action action;
  long arg;
  sim_event *event;
} sim_watch_point;


typedef struct _sim_watchpoints {

  /* Pointer into the host's data structures specifying the
     address/size of the program-counter */
  /* FIXME: In the future this shall be generalized so that any of the
     N processors M registers can be watched */
  void *pc;
  int sizeof_pc;

  /* Pointer to the handler for interrupt watchpoints */
  /* FIXME: can this be done better? */
  sim_event_handler *interrupt_handler;

  /* suported watchpoints */
  sim_watch_point points[nr_watchpoint_types];

} sim_watchpoints;

/* Watch install handler.  */
MODULE_INSTALL_FN sim_watchpoint_install;

#endif /* SIM_WATCH_H */
