/* Main simulator entry points for the M32R.
   Copyright (C) 1996, 1997 Free Software Foundation, Inc.
   Contributed by Cygnus Support.

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
#include <signal.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include "libiberty.h"
#include "bfd.h"
#include "sim-core.h"
#include "cpu-sim.h"

/* Global state until sim_open starts creating and returning it
   [and the other simulator i/f fns take it as an argument].  */
struct sim_state sim_global_state;

/* FIXME: Do we *need* to pass state to the semantic routines?  */
STATE current_state;

/* Create an instance of the simulator.  */

SIM_DESC
sim_open (kind, callback, abfd, argv)
     SIM_OPEN_KIND kind;
     host_callback *callback;
     struct _bfd *abfd;
     char **argv;
{
  int i;
  SIM_DESC sd = &sim_global_state;

  /* FIXME: until we alloc one, use the global.  */
  memset (sd, 0, sizeof (sim_global_state));
  STATE_OPEN_KIND (sd) = kind;
  STATE_CALLBACK (sd) = callback;

  if (sim_pre_argv_init (sd, argv[0]) != SIM_RC_OK)
    return 0;

#if 0 /* FIXME: 'twould be nice if we could do this */
  /* These options override any module options.
     Obviously ambiguity should be avoided, however the caller may wish to
     augment the meaning of an option.  */
  if (extra_options != NULL)
    sim_add_option_table (sd, extra_options);
#endif

  /* getopt will print the error message so we just have to exit if this fails.
     FIXME: Hmmm...  in the case of gdb we need getopt to call
     print_filtered.  */
  if (sim_parse_args (sd, argv) != SIM_RC_OK)
    {
      sim_module_uninstall (sd);
      return 0;
    }

  if (sim_post_argv_init (sd) != SIM_RC_OK)
    {
      sim_module_uninstall (sd);
      return 0;
    }

  /* Initialize various cgen things not done by common framework.  */
  cgen_init (sd);

  /* FIXME:wip */
  sim_core_attach (sd, NULL, attach_raw_memory, access_read_write_exec,
		   0, 0, M32R_DEFAULT_MEM_SIZE, NULL, NULL);

  /* Only needed for profiling, but the structure member is small.  */
  for (i = 0; i < MAX_NR_PROCESSORS; ++i)
    memset (& CPU_M32R_PROFILE (STATE_CPU (sd, i)), 0,
	    sizeof (CPU_M32R_PROFILE (STATE_CPU (sd, i))));

  return &sim_global_state;
}

void
sim_close (sd, quitting)
     SIM_DESC sd;
     int quitting;
{
  sim_module_uninstall (sd);
}

SIM_RC
sim_load (sd, prog, abfd, from_tty)
     SIM_DESC sd;
     char *prog;
     bfd *abfd;
     int from_tty;
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
  STATE_CPU_CPU (sd, 0)->pc = STATE_START_ADDR (sd);
  return SIM_RC_OK;
} 

SIM_RC
sim_create_inferior (sd, argv, envp)
     SIM_DESC sd;
     char **argv;
     char **envp;
{
#if 0
  STATE_ARGV (sd) = sim_copy_argv (argv);
  STATE_ENVP (sd) = sim_copy_argv (envp);
#endif
  return SIM_RC_OK;
}

void
sim_kill (sd)
     SIM_DESC sd;
{
  /* nothing to do */
}

int
sim_stop (SIM_DESC sd)
{
  return engine_stop (sd);
}

void
sim_resume (sd, step, siggnal)
     SIM_DESC sd;
     int step, siggnal;
{
  engine_run (sd, step, siggnal);
}

void
sim_stop_reason (sd, reason, sigrc)
     SIM_DESC sd;
     enum sim_stop *reason;
     int *sigrc;
{
  sim_cpu *cpu = STATE_CPU (sd, 0);

  /* Map sim_state to sim_stop.  */
  switch (CPU_EXEC_STATE (cpu))
    {
    case EXEC_STATE_EXITED :
      *reason = sim_exited;
      *sigrc = CPU_HALT_SIGRC (cpu);
      break;
    case EXEC_STATE_STOPPED :
      *reason = sim_stopped;
      *sigrc = sim_signal_to_host (CPU_HALT_SIGRC (cpu));
      break;
    case EXEC_STATE_SIGNALLED :
      *reason = sim_signalled;
      *sigrc = sim_signal_to_host (CPU_HALT_SIGRC (cpu));
      break;
    }
}

/* PROFILE_CPU_CALLBACK */

static void
print_m32r_misc_cpu (SIM_CPU *cpu, int verbose)
{
  SIM_DESC sd = CPU_STATE (cpu);
  char buf[20];

  if (CPU_PROFILE_FLAGS (cpu) [PROFILE_INSN_IDX])
    {
      sim_io_printf (sd, "Miscellaneous Statistics\n\n");
      sim_io_printf (sd, "  %-*s %s\n\n",
		     PROFILE_LABEL_WIDTH, "Fill nops:",
		     sim_add_commas (buf, sizeof (buf),
				     CPU_M32R_PROFILE (cpu).fillnop_count));
    }
}

void
sim_info (sd, verbose)
     SIM_DESC sd;
     int verbose;
{
  profile_print (sd, STATE_VERBOSE_P (sd), NULL, print_m32r_misc_cpu);
}

/* The contents of BUF are in target byte order.  */

void
sim_fetch_register (sd, rn, buf)
     SIM_DESC sd;
     int rn;
     unsigned char *buf;
{
  if (rn < 16)
    SETTWI (buf, STATE_CPU_CPU (sd, 0)->h_gr[rn]);
  else if (rn < 21)
    SETTWI (buf, STATE_CPU_CPU (sd, 0)->h_cr[rn - 16]);
  else switch (rn) {
    case PC_REGNUM:
      SETTWI (buf, STATE_CPU_CPU (sd, 0)->pc);
      break;
    case ACCL_REGNUM:
      SETTWI (buf, GETLODI (STATE_CPU_CPU (sd, 0)->h_accum));
      break;
    case ACCH_REGNUM:
      SETTWI (buf, GETHIDI (STATE_CPU_CPU (sd, 0)->h_accum));
      break;
#if 0
    case 23: *reg = STATE_CPU_CPU (sd, 0)->h_cond;		break;
    case 24: *reg = STATE_CPU_CPU (sd, 0)->h_sm;		break;
    case 25: *reg = STATE_CPU_CPU (sd, 0)->h_bsm;		break;
    case 26: *reg = STATE_CPU_CPU (sd, 0)->h_ie;		break;
    case 27: *reg = STATE_CPU_CPU (sd, 0)->h_bie;		break;
    case 28: *reg = STATE_CPU_CPU (sd, 0)->h_bcarry;		break; /* rename: bc */
    case 29: memcpy (buf, &STATE_CPU_CPU (sd, 0)->h_bpc, sizeof(WI));	break; /* duplicate */
#endif
    default: abort ();
  }
}
 
/* The contents of BUF are in target byte order.  */

void
sim_store_register (sd, rn, buf)
     SIM_DESC sd;
     int rn;
     unsigned char *buf;
{
  if (rn < 16)
    STATE_CPU_CPU (sd, 0)->h_gr[rn] = GETTWI (buf);
  else if (rn < 21)
    STATE_CPU_CPU (sd, 0)->h_cr[rn - 16] = GETTWI (buf);
  else switch (rn) {
    case PC_REGNUM:
      STATE_CPU_CPU (sd, 0)->pc = GETTWI (buf);
      break;
    case ACCL_REGNUM:
      SETLODI (STATE_CPU_CPU (sd, 0)->h_accum, GETTWI (buf));
      break;
    case ACCH_REGNUM:
      SETHIDI (STATE_CPU_CPU (sd, 0)->h_accum, GETTWI (buf));
      break;
#if 0
    case 23: STATE_CPU_CPU (sd, 0)->h_cond   = *reg;			break;
    case 24: STATE_CPU_CPU (sd, 0)->h_sm     = *reg;			break;
    case 25: STATE_CPU_CPU (sd, 0)->h_bsm    = *reg;			break;
    case 26: STATE_CPU_CPU (sd, 0)->h_ie     = *reg;			break;
    case 27: STATE_CPU_CPU (sd, 0)->h_bie    = *reg;			break;
    case 28: STATE_CPU_CPU (sd, 0)->h_bcarry = *reg;			break; /* rename: bc */
    case 29: memcpy (&STATE_CPU_CPU (sd, 0)->h_bpc, buf, sizeof(DI));	break; /* duplicate */
#endif
  }
}

int
sim_read (sd, addr, buf, len)
     SIM_DESC sd;
     SIM_ADDR addr;
     unsigned char *buf;
     int len;
{
#if 1
  return sim_core_read_buffer (sd, NULL, sim_core_read_map,
				buf, addr, len);
#else
  return (*STATE_MEM_READ (sd)) (sd, addr, buf, len);
#endif
} 

int
sim_write (sd, addr, buf, len)
     SIM_DESC sd;
     SIM_ADDR addr;
     unsigned char *buf;
     int len;
{
#if 1
  return sim_core_write_buffer (sd, NULL, sim_core_write_map,
				buf, addr, len);
#else
  return (*STATE_MEM_WRITE (sd)) (sd, addr, buf, len);
#endif
}

void
sim_do_command (sd, cmd)
     SIM_DESC sd;
     char *cmd;
{ 
  sim_io_error (sd, "sim_do_command - unimplemented");
}
