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
#include <ctype.h>

#include "basics.h"
#include "psim.h"

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#else
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#endif

#include "../../gdb/defs.h"

#include "devices.h"

#include "../../gdb/remote-sim.h"
#include "../../gdb/callback.h"


/* Structures used by the simulator, for gdb just have static structures */

static psim *simulator;
static char *register_names[] = REGISTER_NAMES;
static int print_info = 0;

void
sim_open (char *args)
{
  int i;

  /* trace the call */
  TRACE(trace_gdb, ("sim_open(args=%s) called\n", args ? args : "(null)"));

  if (args) {
    char **argv = buildargv(args);
    int argp = 0;
    int argc;
    for (argc = 0; argv[argc]; argc++);

    while (argp < argc) {
      if (*argv[argp] != '-')
	error ("Argument is not an option '%s'", argv[argp]);

      else {
	/* check arguments -- note, main.c also contains argument processing
	   code for the standalone emulator.  */
	char *p = argv[argp] + 1;
	while (*p != '\0') {
	  switch (*p) {
	  default:
	    printf_filtered("Usage:\n\ttarget sim [ -t <trace-option> ]\n");
	    trace_usage();
	    error ("");
	    break;
	  case 't':
	    argp += 1;
	    if (argv[argp] == NULL)
	      error("Missing <trace> option for -t\n");
	    trace_option(argv[argp]); /* better fail if NULL */
	    break;
	  case 'I':
	    print_info = 1;
	    break;
	  }
	}
      }
      argp += 1;
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
  char **argv;
  TRACE(trace_gdb, ("sim_load(prog=%s, from_tty=%d) called\n",
		    prog, from_tty));
  ASSERT(prog != NULL);

  /* parse the arguments, assume that the file is argument 0 */
  argv = buildargv(prog);
  ASSERT(argv != NULL && argv[0] != NULL);

  /* create the simulator */
  TRACE(trace_gdb, ("sim_load() - first time, create the simulator\n"));
  simulator = psim_create(argv[0]);

  /* bring in all the data section */
  psim_init(simulator);

  /* release the arguments */
  freeargv(argv);

  /* `I did it my way' */
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
  int result = psim_read_memory(simulator, MAX_NR_PROCESSORS,
				buf, mem, length);
  TRACE(trace_gdb, ("sim_read(mem=0x%x, buf=0x%x, length=%d) = %d\n",
		    mem, buf, length, result));
  return result;
}


int
sim_write (SIM_ADDR mem, unsigned char *buf, int length)
{
  int result = psim_write_memory(simulator, MAX_NR_PROCESSORS,
				 buf, mem, length,
				 1/*violate_ro*/);
  TRACE(trace_gdb, ("sim_write(mem=0x%x, buf=0x%x, length=%d) = %d\n",
		    mem, buf, length, result));
  return result;
}


void
sim_fetch_register (int regno, unsigned char *buf)
{
  if (simulator == NULL) {
    return;
  }
  TRACE(trace_gdb, ("sim_fetch_register(regno=%d(%s), buf=0x%x)\n",
		    regno, register_names[regno], buf));
  psim_read_register(simulator, MAX_NR_PROCESSORS,
		     buf, register_names[regno],
		     raw_transfer);
}


void
sim_store_register (int regno, unsigned char *buf)
{
  if (simulator == NULL)
    return;
  TRACE(trace_gdb, ("sim_store_register(regno=%d(%s), buf=0x%x)\n",
		    regno, register_names[regno], buf));
  psim_write_register(simulator, MAX_NR_PROCESSORS,
		      buf, register_names[regno],
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

  psim_init(simulator);
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

  case USER_ENVIRONMENT:
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

  TRACE(trace_gdb, ("sim_stop_reason(reason=0x%x(%d), sigrc=0x%x(%d))\n",
		    reason, *reason, sigrc, *sigrc));
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

  TRACE(trace_gdb, ("sim_resume(step=%d, siggnal=%d)\n",
		    step, siggnal));

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

void
sim_set_callbacks (host_callback *callback)
{
  TRACE(trace_gdb, ("sim_set_callbacks called\n"));
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
