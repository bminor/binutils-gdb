/*  This file is part of the program psim.

    Copyright (C) 1994-1995, Andrew Cagney <cagney@highland.com.au>

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


#include <signal.h> /* FIXME - should be machine dependant version */
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "basics.h"
#include "psim.h"

#include "../../gdb/defs.h"

#include "devices.h"

#include "../../gdb/remote-sim.h"


/* Structures used by the simulator, for gdb just have static structures */

static psim *simulator;
static int nr_cpus;
static char *register_names[] = REGISTER_NAMES;
static int print_info = 0;

void
sim_open (char *args)
{
  int i;

  /* trace the call */
  TRACE(trace_gdb, ("sim_open(args=%s) called\n", args ? args : "(null)"));

  if (args) {
    char *buf = (char *)alloca (strlen (args) + 1);
    char *p;
    strcpy (buf, args);

    p = strtok (args, " \t");
    while (p != (char *)0) {
      if (*p != '-')
	error ("Argument is not an option '%s'", p);

      else {
	/* check arguments -- note, main.c also contains argument processing
	   code for the standalone emulator.  */
	while (*++p != '\0') {
	  switch (*p) {
	  default:
	    error ("Usage: target sim [ -a -p -c -C -s -i -I -t ]\n");
	    break;
	  case 'a':
	    for (i = 0; i < nr_trace; i++)
	      trace[i] = 1;
	    break;
	  case 'p':
	    trace[trace_cpu] = trace[trace_semantics] = 1;
	    break;
	  case 'c':
	    trace[trace_core] = 1;
	    break;
	  case 'C':
	    trace[trace_console_device] = 1;
	    break;
	  case 's':
	    trace[trace_create_stack] = 1;
	    break;
	  case 'i':
	    trace[trace_icu_device] = 1;
	    break;
	  case 'I':
	    print_info = 1;
	    break;
	  case 't':
	    trace[trace_device_tree] = 1;
	    break;
	  }
	}
      }

      p = strtok ((char *)0, " \t");
    }
  }

  /* do something */
  TRACE(trace_tbd, ("sim_open() - TBD - should parse the arguments\n"));
  TRACE(trace_tbd, ("sim_open() - TBD - can not create simulator here as do not have description of it\n"));
}


void
sim_close (int quitting)
{
  TRACE(trace_gdb, ("sim_close(quitting=%d) called\n", quitting));
  if (print_info)
    psim_print_info (simulator, 1);

  /* nothing to do */
}


int
sim_load (char *prog, int from_tty)
{
  TRACE(trace_gdb, ("sim_load(prog=%s, from_tty=%d) called\n",
		    prog, from_tty));

  /* sanity check */
  if (prog == NULL) {
    error ("sim_load() - TBD - read stan shebs e-mail about how to find the program name?\n");
    return -1;
  }
  TRACE(trace_tbd, ("sim_load() - TBD - parse that prog stripping things like quotes\n"));

  /* create the simulator */
  TRACE(trace_gdb, ("sim_load() - first time, create the simulator\n"));
  nr_cpus = (WITH_SMP ? WITH_SMP : 1);
  simulator = psim_create(prog, nr_cpus);

  /* bring in all the data section */
  psim_load(simulator);

  return 0;
}


void
sim_kill (void)
{
  TRACE(trace_gdb, ("sim_kill(void) called\n"));
  /* do nothing, nothing to do */
}


int
sim_read (SIM_ADDR mem, unsigned char *buf, int length)
{
  return psim_read_memory(simulator, nr_cpus, buf, mem, length,
			  raw_transfer);
}


int
sim_write (SIM_ADDR mem, unsigned char *buf, int length)
{
  return psim_write_memory(simulator, nr_cpus, buf, mem, length,
			   raw_transfer, 1/*violate_ro*/);
}


void
sim_fetch_register (int regno, unsigned char *buf)
{
  if (simulator == NULL) {
    return;
  }

  psim_read_register(simulator, nr_cpus, buf, register_names[regno],
		     raw_transfer);
}


void
sim_store_register (int regno, unsigned char *buf)
{
  if (simulator == NULL)
    return;

  psim_write_register(simulator, nr_cpus, buf, register_names[regno],
		      raw_transfer);
}


void
sim_info (int verbose)
{
  TRACE(trace_gdb, ("sim_info(verbose=%d) called\n", verbose));
  psim_print_info (simulator, verbose);
}


void
sim_create_inferior (SIM_ADDR start_address, char **argv, char **envp)
{
  unsigned_word entry_point = start_address;

  TRACE(trace_gdb, ("sim_create_inferior(start_address=0x%x, ...)\n",
		    start_address));

  psim_load(simulator);
  psim_stack(simulator, argv, envp);

  psim_write_register(simulator, -1 /* all start at same PC */,
		      &entry_point, "pc", cooked_transfer);
}


static volatile int sim_should_run;

void
sim_stop_reason (enum sim_stop *reason, int *sigrc)
{
  psim_status status = psim_get_status(simulator);

  switch (CURRENT_ENVIRONMENT) {

  case VIRTUAL_ENVIRONMENT:
    switch (status.reason) {
    case was_continuing:
      *reason = sim_stopped;
      *sigrc = SIGTRAP;
      if (sim_should_run) {
	error("sim_stop_reason() unknown reason for halt\n");
      }
      break;
    case was_trap:
      *reason = sim_stopped;
      *sigrc = SIGTRAP;
      break;
    case was_exited:
      *reason = sim_exited;
      *sigrc = 0;
      break;
    case was_signalled:
      *reason = sim_signalled;
      *sigrc = status.signal;
      break;
    }
    break;

  case OPERATING_ENVIRONMENT:
    *reason = sim_stopped;
    *sigrc = SIGTRAP;
    break;

  default:
    error("sim_stop_reason() - unknown environment\n");
  
  }
}



/* Run (or resume) the program.  */
static void
sim_ctrl_c()
{
  sim_should_run = 0;
}

void
sim_resume (int step, int siggnal)
{
  void (*prev) ();
  unsigned_word program_counter;

  prev = signal(SIGINT, sim_ctrl_c);
  sim_should_run = 1;

  if (step)
    psim_step(simulator);
  else
    psim_run_until_stop(simulator, &sim_should_run);

  signal(SIGINT, prev);
}

void
sim_do_command(char *cmd)
{
  TRACE(trace_gdb, ("sim_do_commands(cmd=%s) called\n", cmd));
}

/****/

void *
zalloc(long size)
{
  void *memory = (void*)xmalloc(size);
  if (memory == NULL)
    error("xmalloc failed\n");
  bzero(memory, size);
  return memory;
}

void zfree(void *data)
{
  mfree(NULL, data);
}
