/* Generic simulator halt/resume.
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

#ifndef SIM_ENGINE_H
#define SIM_ENGINE_H


typedef struct _sim_engine sim_engine;
struct _sim_engine
{
  void *jmpbuf;
  sim_cpu *last_cpu;
  sim_cpu *next_cpu;
  enum sim_stop reason;
  sim_event *stepper;
  int sigrc;
};



/* Halt the simulator *now* */

extern void sim_engine_halt
(SIM_DESC sd,
 sim_cpu *last_cpu,
 sim_cpu *next_cpu, /* NULL -> succ (last_cpu) or event-mgr */
 sim_cia cia,
 enum sim_stop reason,
 int sigrc);

/* Halt hook - allow target specific operation when halting a
   simulator */

#if !defined (SIM_ENGINE_HALT_HOOK)
#define SIM_ENGINE_HALT_HOOK(SD, LAST_CPU, CIA) if ((LAST_CPU) != NULL) (LAST_CPU)->cia = cia
#endif



/* restart the simulator *now* */

extern void sim_engine_restart
(SIM_DESC sd,
 sim_cpu *last_cpu,
 sim_cpu *next_cpu,
 sim_cia cia);

/* Restart hook - allow target specific operation when restarting a
   simulator */

#if !defined (SIM_ENGINE_RESTART_HOOK)
#define SIM_ENGINE_RESTART_HOOK(SD, LAST_CPU, CIA) SIM_ENGINE_HALT_HOOK(SD, LAST_CPU, CIA)
#endif




/* Abort the simulator *now*.

   This function is NULL safe.  It can be called when either of SD or
   CIA are NULL.

   This function is setjmp/longjmp safe.  It can be called when of
   the sim_engine setjmp/longjmp buffer has not been established.

   Simulators that are using components such as sim-core but are not
   yet using this sim-engine module should link in file sim-abort.o
   which implements a non setjmp/longjmp version of
   sim_engine_abort. */

extern void sim_engine_abort
(SIM_DESC sd,
 sim_cpu *cpu,
 sim_cia cia,
 const char *fmt,
 ...);

/* No abort hook - when possible this function exits using the
   engine_halt function (and SIM_ENGINE_HALT_HOOK). */




/* Called by the generic sim_resume to run the simulation within the
   above safty net.

   An example implementation of sim_engine_run can be found in the
   file sim-run.c */

extern void sim_engine_run
(SIM_DESC sd,
 int next_cpu_nr,
 int siggnal); /* most simulators ignore siggnal */



/* Determine the state of next/last cpu when the simulator was last
   halted - a value >= nr-cpus indicates that the event-queue was
   next/last. */

extern int sim_engine_next_cpu_nr (SIM_DESC sd);
extern int sim_engine_last_cpu_nr (SIM_DESC sd);


#endif
