/*  This file is part of the program psim.

    Copyright (C) 1994-1997, Andrew Cagney <cagney@highland.com.au>
    Copyright (C) 1997, Free Software Foundation

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


#ifndef _SIM_MAIN_H_
#define _SIM_MAIN_H_

#include "sim-basics.h"

#include <signal.h>

/* These are generated files.  */
#include "itable.h"
#include "idecode.h"
#include "idecode.h"

typedef instruction_address sim_cia;
static const sim_cia null_cia = {0}; /* Dummy */
#define NULL_CIA null_cia

#include "sim-base.h"

#include "alu.h"
#include "cpu.h"


struct sim_state {

  /* escape route for inner functions */
  int halt_ok;
  jmp_buf path_to_halt;
  int restart_ok;
  jmp_buf path_to_restart;

  /* status from last halt */
  enum sim_stop reason;
  int siggnal;

  /* the processors proper */
  sim_cpu cpu;
#define STATE_CPU(sd, n) (&(sd)->cpu)

  /* The base class.  */
  sim_state_base base;

};

/* (re) initialize the simulator */

extern void engine_init
(SIM_DESC sd);


/* Mechanisms for stopping/restarting the simulation.

   A non NULL CPU argument designates the processor that is initiating
   the halt.  After the simulation has stopped that processor should
   be marked as the last one active */

extern void engine_error
(SIM_DESC sd,
 sim_cpu *cpu,
 instruction_address cia,
 const char *fmt,
 ...);

extern void engine_halt
(SIM_DESC sd,
 sim_cpu *cpu,
 instruction_address cia,
 enum sim_stop reason,
 int siggnal);

extern void engine_restart
(SIM_DESC sd,
 sim_cpu *cpu,
 instruction_address cia);

/* SIMULATE INSTRUCTIONS, various different ways of achieving the same
   thing (others later) */

extern void engine_run_until_stop
(SIM_DESC sd,
 volatile int *keep_running);

extern void engine_step
(SIM_DESC sd);

#endif
