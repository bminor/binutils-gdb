/*  This file is part of the program psim.

    Copyright (C) 1994-1996, Andrew Cagney <cagney@highland.com.au>
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


#include <signal.h> /* FIXME - should be machine dependant version */
#include <stdarg.h>
#include <ctype.h>

#include "bfd.h"
#include "sim-main.h"


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


#define SIM_ADDR unsigned

/* Structures used by the simulator, for gdb just have static structures */

struct sim_state simulation = { 0 };


SIM_DESC
sim_open (SIM_OPEN_KIND kind, char **argv)
{
  /*FIXME - later make these configurable */
  static void *eit_ram;
  static int sizeof_eit_ram = 0x1000;

  STATE_OPEN_KIND (&simulation) = kind;

  /* establish the simulator configuration */
  sim_config (&simulation,
	      LITTLE_ENDIAN/*d30v always big endian*/);

  if (sim_pre_argv_init (&simulation, argv[0]) != SIM_RC_OK)
    return 0;

  /* getopt will print the error message so we just have to exit if this fails.
     FIXME: Hmmm...  in the case of gdb we need getopt to call
     print_filtered.  */
  if (sim_parse_args (&simulation, argv) != SIM_RC_OK)
    return 0;

  engine_init(&simulation);

  /* external memory */
  sim_core_attach(&simulation,
		  attach_raw_memory,
		  access_read_write_exec,
		  0, 0x2000000, 0x100000, NULL, NULL);

 /* FIXME: for now */
  return (SIM_DESC) &simulation;
}


/* NOTE: sim_size is going away */
void sim_size (int i);
void
sim_size (int i)
{
  sim_io_error (NULL, "unexpected call to sim_size()");
}


void
sim_close (SIM_DESC sd, int quitting)
{
}


SIM_RC
sim_load (SIM_DESC sd, char *prog, bfd *abfd, int from_tty)
{
  extern bfd *sim_load_file (); /* ??? Don't know where this should live.  */
  bfd *prog_bfd;

  prog_bfd = sim_load_file (sd, STATE_MY_NAME (sd),
			    STATE_CALLBACK (sd),
			    prog,
			    /* pass NULL for abfd, we always open our own */
			    NULL,
			    STATE_OPEN_KIND (sd) == SIM_OPEN_DEBUG);
  if (prog_bfd == NULL)
    return SIM_RC_FAIL;
  sim_analyze_program (sd, prog_bfd);
  return SIM_RC_OK;
}


void
sim_kill (SIM_DESC sd)
{
}


int
sim_read (SIM_DESC sd, SIM_ADDR mem, unsigned char *buf, int length)
{
  sim_io_error (sd, "sim_read");
  return 0;
}


int
sim_write (SIM_DESC sd, SIM_ADDR mem, unsigned char *buf, int length)
{
  return sim_core_write_buffer (sd, sim_core_write_map,
				buf, mem, length);
}


void
sim_fetch_register (SIM_DESC sd, int regno, unsigned char *buf)
{
  sim_io_error (sd, "sim_fetch_register");
}


void
sim_store_register (SIM_DESC sd, int regno, unsigned char *buf)
{
  sim_io_error (sd, "sim_info");
}


void
sim_info (SIM_DESC sd, int verbose)
{
}


SIM_RC
sim_create_inferior (SIM_DESC sd,
		     char **argv,
		     char **envp)
{
  STATE_CPU (sd, 0)->cia.ip = STATE_START_ADDR(sd);
  STATE_CPU (sd, 0)->cia.dp = (STATE_START_ADDR(sd)
			       + sizeof (instruction_word));
  return SIM_RC_OK;
}


volatile int keep_running = 1;

void
sim_stop_reason (SIM_DESC sd, enum sim_stop *reason, int *sigrc)
{
  *reason = simulation.reason;
  *sigrc = simulation.siggnal;
  keep_running = 1; /* ready for next run */
}


int
sim_stop (SIM_DESC sd)
{
  keep_running = 0;
  return 1;
}

void
sim_resume (SIM_DESC sd, int step, int siggnal)
{
  /* keep_running = 1 - in sim_stop_reason */
  if (step)
    keep_running = 0;
  engine_run_until_stop(sd, &keep_running);
}

void
sim_do_command (SIM_DESC sd, char *cmd)
{
  sim_io_error (sd, "sim_do_command - unimplemented");
}


void
sim_set_callbacks (SIM_DESC sd, host_callback *callback)
{
  STATE_CALLBACK (&simulation) = callback;
}
