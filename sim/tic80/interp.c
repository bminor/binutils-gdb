/*  This file is part of the GDB simulators.

    Copyright (C) 1997, Free Software Foundation
    Condtributed by Cyngnus Solutions.

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

#include "idecode.h"
#include "itable.h"

#ifdef HAVE_STRING_H
#include <string.h>
#else
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#endif


#if 0

void
engine_error (SIM_DESC sd,
	      sim_cpu *cpu,
	      instruction_address cia,
	      const char *fmt,
	      ...)
{
  va_list ap;
  va_start (ap, fmt);
  sim_io_evprintf (sd, fmt, ap);
  va_end (ap);

  sim_halt (sd, cpu, NULL, cia, sim_stopped, SIGABRT);
}

void
engine_halt (SIM_DESC sd,
	     sim_cpu *cpu,
	     instruction_address cia,
	     enum sim_stop reason,
	     int siggnal)
{
  if (!sd->halt_ok)
    sim_io_error (sd, "engine_halt - bad longjmp");
  sd->reason = reason;
  sd->siggnal = siggnal;
  sd->halt_ok = 0;
  sd->restart_ok = 0;
  if (cpu != NULL)
    cpu->cia = cia;
  longjmp (sd->path_to_halt, 1);
}

void
engine_restart (SIM_DESC sd,
		sim_cpu *cpu,
		instruction_address cia)
{
  if (!sd->restart_ok)
    sim_io_error (sd, "engine_restart - bad longjmp");
  sd->restart_ok = 0;
  cpu->cia = cia;
  longjmp(sd->path_to_restart, 1);
}


void
engine_run_until_stop (SIM_DESC sd,
		       volatile int *keep_running)
{
  if (!setjmp (sd->path_to_halt))
    {
      instruction_address cia;
      sim_cpu *cpu = STATE_CPU (sd, 0);
      sd->halt_ok = 1;
      setjmp (sd->path_to_restart);
      sd->restart_ok = 1;
      cia = cpu->cia;
      do
	{
	  instruction_word insn = IMEM (cia);
	  cia = idecode_issue (sd, insn, cia);
	}
      while (*keep_running);
      engine_halt (sd, cpu, cia, sim_stopped, SIM_SIGINT);
    }
}


void
engine_step (SIM_DESC sd)
{
  if (!setjmp (sd->path_to_halt))
    {
      instruction_address cia;
      instruction_word insn;
      sim_cpu *cpu = STATE_CPU (sd, 0);
      sd->halt_ok = 1;
      setjmp (sd->path_to_restart);
      sd->restart_ok = 1;
      cia = cpu->cia;
      insn = IMEM (cia);
      cia = idecode_issue (sd, insn, cia);
      engine_halt (sd, cpu, cia, sim_stopped, SIM_SIGTRAP);
    }
}

#endif
