/* Generic simulator halt/restart.
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

#include "sim-main.h"

#include <stdio.h>
#include <signal.h>


/* Generic halt */

void
sim_engine_halt (SIM_DESC sd,
		 sim_cpu *last_cpu,
		 sim_cpu *next_cpu, /* NULL - use default */
		 sim_cia cia,
		 enum sim_stop reason,
		 int sigrc)
{
  sim_engine *engine = STATE_ENGINE (sd);
  if (engine->jmpbuf != NULL)
    {
      jmp_buf *halt_buf = engine->jmpbuf;
      engine->last_cpu = last_cpu;
      engine->next_cpu = next_cpu;
      engine->reason = reason;
      engine->sigrc = sigrc;
      SIM_ENGINE_HALT_HOOK (sd, last_cpu, cia);
      longjmp(*halt_buf, 1);
    }
  else
    sim_io_error (sd, "sim_halt - bad long jump");
}


/* Generic restart */

void
sim_engine_restart (SIM_DESC sd,
		    sim_cpu *last_cpu,
		    sim_cpu *next_cpu,
		    sim_cia cia)
{
  sim_engine *engine = STATE_ENGINE (sd);
  if (engine->jmpbuf != NULL)
    {
      jmp_buf *halt_buf = engine->jmpbuf;
      engine->last_cpu = last_cpu;
      engine->next_cpu = next_cpu;
      SIM_ENGINE_RESTART_HOOK (sd, last_cpu, cia);
      longjmp(*halt_buf, 0);
    }
  else
    sim_io_error (sd, "sim_restart - bad long jump");
}


/* Generic error code */

void
sim_engine_abort (SIM_DESC sd,
		  sim_cpu *cpu,
		  sim_cia cia,
		  const char *fmt,
		  ...)
{
  if (sd == NULL)
    {
      va_list ap;
      va_start(ap, fmt);
      vfprintf (stderr, fmt, ap);
      va_end(ap);
      fprintf (stderr, "\nQuit\n");
      abort ();
    }
  else if (STATE_ENGINE (sd)->jmpbuf == NULL)
    {
      va_list ap;
      va_start(ap, fmt);
      sim_io_evprintf (sd, fmt, ap);
      va_end(ap);
      sim_io_eprintf (sd, "\n");
      sim_io_error (sd, "Quit Simulator");
    }
  else
    {
      va_list ap;
      va_start(ap, fmt);
      sim_io_evprintf (sd, fmt, ap);
      va_end(ap);
      sim_io_eprintf (sd, "\n");
      sim_engine_halt (sd, cpu, NULL, cia, sim_stopped, SIGABRT);
    }
}


/* Generic next/last cpu */

int
sim_engine_last_cpu_nr (SIM_DESC sd)
{
  sim_engine *engine = STATE_ENGINE (sd);
  if (engine->last_cpu != NULL)
    return engine->last_cpu - STATE_CPU (sd, 0);
  else
    return MAX_NR_PROCESSORS;
}

int
sim_engine_next_cpu_nr (SIM_DESC sd)
{
  sim_engine *engine = STATE_ENGINE (sd);
  if (engine->next_cpu != NULL)
    return engine->next_cpu - STATE_CPU (sd, 0);
  else
    return sim_engine_last_cpu_nr (sd) + 1;
}
