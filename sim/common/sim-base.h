/* Simulator pseudo baseclass.
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

/* This file is meant to be included by sim-basics.h.  */

#ifndef SIM_BASE_H
#define SIM_BASE_H

/* Global pointer to current state while sim_resume is running.
   On a machine with lots of registers, it might be possible to reserve
   one of them for current_state.  However on a machine with few registers
   current_state can't permanently live in one and indirecting through it
   will be slower [in which case one can have sim_resume set globals from
   current_state for faster access].
   If CURRENT_STATE_REG is defined, it means current_state is living in
   a global register.  */

#ifdef CURRENT_STATE_REG
/* FIXME: wip */
#else
extern struct sim_state *current_state;
#endif

/* Simulator state pseudo base-class.
   Each simulator is required to have a sim-main.h file that includes
   sim-basics.h and defines struct sim_state to be:

   struct sim_state {
     struct sim_state_base base;
     ... simulator specific members ...
   };
   */

struct sim_state_base {
  /* Marker for those wanting to do sanity checks.  */
  int magic;
#define SIM_MAGIC_NUMBER 0x4242
#define STATE_MAGIC(sd) ((sd)->base.magic)

  /* Simulator's argv[0].  */
  const char *my_name;
#define STATE_MY_NAME(sd) ((sd)->base.my_name)

  /* Who opened the simulator.  */
  SIM_OPEN_KIND open_kind;
#define STATE_OPEN_KIND(sd) ((sd)->base.open_kind)

  /* The host callbacks.  */
  struct host_callback_struct *callback;
#define STATE_CALLBACK(sd) ((sd)->base.callback)

#if 0 /* FIXME: Not ready yet.  */
  /* Stuff defined in sim-config.h.  */
  struct sim_config config;
#define STATE_CONFIG(sd) ((sd)->base.config)
#endif
};

/* Functions for allocating/freeing a sim_state.  */
SIM_DESC sim_state_alloc PARAMS ((void));
void sim_state_free PARAMS ((SIM_DESC));

#endif /* SIM_BASE_H */
