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

#include <signal.h>

#ifdef HAVE_STRING_H
#include <string.h>
#else
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#endif

void
engine_init (SIM_DESC sd)
{
  memset (&STATE_CPU (sd, 0)->reg, 0, sizeof STATE_CPU (sd, 0)->reg);
  memset (&STATE_CPU (sd, 0)->cia, 0, sizeof STATE_CPU (sd, 0)->cia);
  CPU_STATE (STATE_CPU (sd, 0)) = sd;
}


/* Mechanisms for stopping/restarting the simulation */

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

  if (sd->halt_ok)
    {
      sim_io_eprintf (sd, "\n");
      engine_halt (sd, cpu, cia, sim_signalled, SIGABRT);
    }
  else
    sim_io_error (sd, " - aborting simulation");
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
	  if (cia.ip == -1)
	    {
	      /* anulled instruction */
	      cia.ip = cia.dp;
	      cia.dp = cia.dp + sizeof (instruction_word);
	    }
	  else
	    {
	      instruction_word insn = IMEM (cia.ip);
	      cia = idecode_issue (sd, insn, cia);
	    }
	}
      while (*keep_running);
      engine_halt (sd, cpu, cia, sim_stopped, SIGINT);
    }
}


#if defined(WITH_TRACE)
/* Tracing support routines */

static char tic80_trace_buffer[1024];
static int  tic80_size_name;

#define SIZE_HEX	8
#define SIZE_DECIMAL	12

/* Initialize tracing by calculating the maximum name size */
static void
tic80_init_trace (void)
{
  int i;
  int len, max_len = 0;

  for (i = 0; i < (int)nr_itable_entries; i++) {
    len = strlen (itable[i].name);
    if (len > max_len)
      max_len = len;
  }

  tic80_size_name = max_len + sizeof(":m") - 1 + sizeof (":s") - 1;
}

/* Trace the result of an ALU operation with 2 integer inputs and an integer output */
char *
tic80_trace_alu3 (int indx,
		  unsigned32 result,
		  unsigned32 input1,
		  unsigned32 input2)
{
  char buf1[SIZE_DECIMAL+10], buf2[SIZE_DECIMAL+10], bufr[SIZE_DECIMAL+10];

  if (!tic80_size_name)
    tic80_init_trace ();

  sprintf (bufr, "(%ld)", (long) (signed32) result);
  sprintf (buf1, "(%ld)", (long) (signed32) input1);
  sprintf (buf2, "(%ld)", (long) (signed32) input2);

  sprintf (tic80_trace_buffer, "%-*s 0x%.*lx %-*s 0x%.*lx %-*s => 0x%.*lx %-*s",
	   tic80_size_name, itable[indx].name,
	   SIZE_HEX, input1, SIZE_DECIMAL, buf1,
	   SIZE_HEX, input2, SIZE_DECIMAL, buf2,
	   SIZE_HEX, result, SIZE_DECIMAL, bufr);

  return tic80_trace_buffer;
}

/* Trace the result of an ALU operation with 1 integer input and an integer output */
char *
tic80_trace_alu2 (int indx,
		  unsigned32 result,
		  unsigned32 input)
{
  char bufi[SIZE_DECIMAL+10], bufr[SIZE_DECIMAL+10];

  if (!tic80_size_name)
    tic80_init_trace ();

  sprintf (bufr, "(%ld)", (long) (signed32) result);
  sprintf (bufi, "(%ld)", (long) (signed32) input);

  sprintf (tic80_trace_buffer, "%-*s 0x%.*lx %-*s %*s => 0x%.*lx %-*s",
	   tic80_size_name, itable[indx].name,
	   SIZE_HEX, input, SIZE_DECIMAL, bufi,
	   SIZE_HEX + SIZE_DECIMAL + 3, "",
	   SIZE_HEX, result, SIZE_DECIMAL, bufr);

  return tic80_trace_buffer;
}

/* Trace the result of a NOP operation */
char *
tic80_trace_nop (int indx)
{
  if (!tic80_size_name)
    tic80_init_trace ();

  sprintf (tic80_trace_buffer, "%s:", itable[indx].name);
  return tic80_trace_buffer;
}

/* Trace the result of a data sink with one input */
char *
tic80_trace_sink1 (int indx, unsigned32 input)
{
  char buf[SIZE_DECIMAL+10];

  if (!tic80_size_name)
    tic80_init_trace ();

  sprintf (buf, "(%ld)", (long) (signed32) input);
  sprintf (tic80_trace_buffer, "%-*s: 0x%.*lx %-*s",
	   tic80_size_name, itable[indx].name,
	   SIZE_HEX, input, SIZE_DECIMAL, buf);

  return tic80_trace_buffer;
}

/* Trace the result of a data sink with two inputs */
char *
tic80_trace_sink2 (int indx, unsigned32 input1, unsigned32 input2)
{
  char buf1[SIZE_DECIMAL+10], buf2[SIZE_DECIMAL+10];

  if (!tic80_size_name)
    tic80_init_trace ();

  sprintf (buf1, "(%ld)", (long) (signed32) input1);
  sprintf (buf2, "(%ld)", (long) (signed32) input2);
  sprintf (tic80_trace_buffer, "%-*s: 0x%.*lx %-*s 0x%.*lx %-*s",
	   tic80_size_name, itable[indx].name,
	   SIZE_HEX, input1, SIZE_DECIMAL, buf1,
	   SIZE_HEX, input2, SIZE_DECIMAL, buf2);

  return tic80_trace_buffer;
}

/* Trace the result of a conditional branch operation */
char *
tic80_trace_cond_br (int indx,
		     int jump_p,
		     unsigned32 cond,
		     unsigned32 target)
{
  char buf[SIZE_DECIMAL+10];

  if (!tic80_size_name)
    tic80_init_trace ();

  sprintf (buf, "(%ld)", (long) (signed32) cond);

  if (jump_p)
    sprintf (tic80_trace_buffer,
	     "%-*s 0x%.*lx %*s 0x%.*lx %-*s => 0x%.*lx",
	     tic80_size_name, itable[indx].name,
	     SIZE_HEX, target, SIZE_DECIMAL, "",
	     SIZE_HEX, cond, SIZE_DECIMAL, buf,
	     SIZE_HEX, target);
  else
    sprintf (tic80_trace_buffer,
	     "%-*s 0x%.*lx %*s 0x%.*lx %-*s => [fallthrough]",
	     tic80_size_name, itable[indx].name,
	     SIZE_HEX, target, SIZE_DECIMAL, "",
	     SIZE_HEX, cond, SIZE_DECIMAL, buf);

  return tic80_trace_buffer;	       
}

/* Trace the result of a unconditional branch operation */
char *
tic80_trace_ucond_br (int indx,
		      unsigned32 target)
{
  if (!tic80_size_name)
    tic80_init_trace ();

  sprintf (tic80_trace_buffer,
	     "%-*s 0x%.*lx %*s => 0x%.*lx",
	     tic80_size_name, itable[indx].name,
	     SIZE_HEX, target, (SIZE_DECIMAL*2) + SIZE_HEX + 4, "",
	     SIZE_HEX, target);

  return tic80_trace_buffer;	       
}

/* Trace the result of a load or store operation with 2 integer addresses
   and an integer output or input */
char *
tic80_trace_ldst (int indx,
		  int st_p,
		  int m_p,
		  int s_p,
		  unsigned32 value,
		  unsigned32 input1,
		  unsigned32 input2)
{
  char buf1[SIZE_DECIMAL+10], buf2[SIZE_DECIMAL+10], bufr[SIZE_DECIMAL+10];
  char name[40];

  if (!tic80_size_name)
    tic80_init_trace ();

  strcpy (name, itable[indx].name);
  if (m_p)
    strcat (name, ":m");

  if (s_p)
    strcat (name, ":s");

  sprintf (bufr, "(%ld)", (long) (signed32) value);
  sprintf (buf1, "(%ld)", (long) (signed32) input1);
  sprintf (buf2, "(%ld)", (long) (signed32) input2);

  sprintf (tic80_trace_buffer, "%-*s 0x%.*lx %-*s 0x%.*lx %-*s %s 0x%.*lx %-*s",
	   tic80_size_name, name,
	   SIZE_HEX, input1, SIZE_DECIMAL, buf1,
	   SIZE_HEX, input2, SIZE_DECIMAL, buf2,
	   (!st_p) ? "=>" : "<=",
	   SIZE_HEX, value, SIZE_DECIMAL, bufr);

  return tic80_trace_buffer;
}
#endif /* WITH_TRACE */
