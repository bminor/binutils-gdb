/* Main simulator entry points for the M32R.
   Copyright (C) 1996, 1997, 1998 Free Software Foundation, Inc.
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
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include "sim-options.h"
#include "libiberty.h"
#include "bfd.h"
#include "targ-vals.h"

static void free_state (SIM_DESC);
static void print_m32r_misc_cpu (SIM_CPU *cpu, int verbose);

/* Records simulator descriptor so utilities like m32r_dump_regs can be
   called from gdb.  */
SIM_DESC current_state;

/* Cover function of sim_state_free to free the cpu buffers as well.  */

static void
free_state (SIM_DESC sd)
{
  if (STATE_MODULES (sd) != NULL)
    sim_module_uninstall (sd);
  sim_cpu_free_all (sd);
  sim_state_free (sd);
}

/* Create an instance of the simulator.  */

SIM_DESC
sim_open (kind, callback, abfd, argv)
     SIM_OPEN_KIND kind;
     host_callback *callback;
     struct _bfd *abfd;
     char **argv;
{
  SIM_DESC sd = sim_state_alloc (kind, callback);

  /* The cpu data is kept in a separately allocated chunk of memory.  */
  if (sim_cpu_alloc_all (sd, 1, cgen_cpu_max_extra_bytes ()) != SIM_RC_OK)
    {
      free_state (sd);
      return 0;
    }

#if 0 /* FIXME: pc is in mach-specific struct */
  /* FIXME: watchpoints code shouldn't need this */
  {
    SIM_CPU *current_cpu = STATE_CPU (sd, 0);
    STATE_WATCHPOINTS (sd)->pc = &(PC);
    STATE_WATCHPOINTS (sd)->sizeof_pc = sizeof (PC);
  }
#endif

  if (sim_pre_argv_init (sd, argv[0]) != SIM_RC_OK)
    {
      free_state (sd);
      return 0;
    }

#if 0 /* FIXME: 'twould be nice if we could do this */
  /* These options override any module options.
     Obviously ambiguity should be avoided, however the caller may wish to
     augment the meaning of an option.  */
  if (extra_options != NULL)
    sim_add_option_table (sd, extra_options);
#endif

  /* Allocate core managed memory */
  sim_do_commandf (sd, "memory region 0,0x%lx", M32R_DEFAULT_MEM_SIZE);

  /* Allocate a handler for the MSPR register.  */
  sim_core_attach (sd, NULL,
		   0 /*level*/,
		   access_read_write,
		   0 /*space ???*/,
		   M32R_DEVICE_ADDR, M32R_DEVICE_LEN /*nr_bytes*/,
		   0 /*modulo*/,
		   &m32r_devices,
		   NULL /*buffer*/);

  /* getopt will print the error message so we just have to exit if this fails.
     FIXME: Hmmm...  in the case of gdb we need getopt to call
     print_filtered.  */
  if (sim_parse_args (sd, argv) != SIM_RC_OK)
    {
      free_state (sd);
      return 0;
    }

  /* check for/establish the reference program image */
  if (sim_analyze_program (sd,
			   (STATE_PROG_ARGV (sd) != NULL
			    ? *STATE_PROG_ARGV (sd)
			    : NULL),
			   abfd) != SIM_RC_OK)
    {
      free_state (sd);
      return 0;
    }

  /* If both cpu model and state architecture are set, ensure they're
     compatible.  If only one is set, set the other.  If neither are set,
     use the default model.  STATE_ARCHITECTURE is the bfd_arch_info data
     for the selected "mach" (bfd terminology).  */
  {
    SIM_CPU *cpu = STATE_CPU (sd, 0);

    if (! STATE_ARCHITECTURE (sd)
	/* Only check cpu 0.  STATE_ARCHITECTURE is for that one only.  */
	&& ! CPU_MACH (cpu))
      {
	/* Set the default model.  */
	const MODEL *model = sim_model_lookup (WITH_DEFAULT_MODEL);
	sim_model_set (sd, NULL, model);
      }
    if (STATE_ARCHITECTURE (sd)
	&& CPU_MACH (cpu))
      {
	if (strcmp (STATE_ARCHITECTURE (sd)->printable_name,
		    MACH_NAME (CPU_MACH (cpu))) != 0)
	  {
	    sim_io_eprintf (sd, "invalid model `%s' for `%s'\n",
			    MODEL_NAME (CPU_MODEL (cpu)),
			    STATE_ARCHITECTURE (sd)->printable_name);
	    free_state (sd);
	    return 0;
	  }
      }
    else if (STATE_ARCHITECTURE (sd))
      {
	/* Use the default model for the selected machine.
	   The default model is the first one in the list.  */
	const MACH *mach = sim_mach_lookup (STATE_ARCHITECTURE (sd)->printable_name);
	sim_model_set (sd, NULL, MACH_MODELS (mach));
      }
    else
      {
        STATE_ARCHITECTURE (sd) = bfd_scan_arch (MACH_NAME (CPU_MACH (cpu)));
      }
  }

  /* Establish any remaining configuration options.  */
  if (sim_config (sd) != SIM_RC_OK)
    {
      free_state (sd);
      return 0;
    }

  if (sim_post_argv_init (sd) != SIM_RC_OK)
    {
      free_state (sd);
      return 0;
    }

  /* Initialize various cgen things not done by common framework.  */
  cgen_init (sd);

  {
    int c;

    for (c = 0; c < MAX_NR_PROCESSORS; ++c)
      {
	/* Only needed for profiling, but the structure member is small.  */
	memset (& CPU_M32R_MISC_PROFILE (STATE_CPU (sd, c)), 0,
		sizeof (CPU_M32R_MISC_PROFILE (STATE_CPU (sd, c))));
	/* Hook in callback for reporting these stats */
	PROFILE_INFO_CPU_CALLBACK (CPU_PROFILE_DATA (STATE_CPU (sd, c)))
	  = print_m32r_misc_cpu;
      }
  }

  /* Store in a global so things like sparc32_dump_regs can be invoked
     from the gdb command line.  */
  current_state = sd;

  return sd;
}

void
sim_close (sd, quitting)
     SIM_DESC sd;
     int quitting;
{
  sim_module_uninstall (sd);
}

SIM_RC
sim_create_inferior (sd, abfd, argv, envp)
     SIM_DESC sd;
     struct _bfd *abfd;
     char **argv;
     char **envp;
{
  SIM_CPU *current_cpu = STATE_CPU (sd, 0);
  SIM_ADDR addr;

  if (abfd != NULL)
    addr = bfd_get_start_address (abfd);
  else
    addr = 0;
  sim_pc_set (current_cpu, addr);

#if 0
  STATE_ARGV (sd) = sim_copy_argv (argv);
  STATE_ENVP (sd) = sim_copy_argv (envp);
#endif

  return SIM_RC_OK;
}

int
sim_stop (SIM_DESC sd)
{
  switch (STATE_ARCHITECTURE (sd)->mach)
    {
    case bfd_mach_m32r :
      return m32r_engine_stop (sd, NULL, NULL_CIA, sim_stopped, SIM_SIGINT);
/* start-sanitize-m32rx */
#ifdef HAVE_CPU_M32RX
    case bfd_mach_m32rx :
      return m32rx_engine_stop (sd, NULL, NULL_CIA, sim_stopped, SIM_SIGINT);
#endif
/* end-sanitize-m32rx */
    default :
      abort ();
    }
}

/* This isn't part of the official interface.
   This is just a good place to put this for now.  */

void
sim_sync_stop (SIM_DESC sd, SIM_CPU *cpu, PCADDR pc, enum sim_stop reason, int sigrc)
{
  switch (STATE_ARCHITECTURE (sd)->mach)
    {
    case bfd_mach_m32r :
      (void) m32r_engine_stop (sd, cpu, pc, reason, sigrc);
      break;
/* start-sanitize-m32rx */
#ifdef HAVE_CPU_M32RX
    case bfd_mach_m32rx :
      (void) m32rx_engine_stop (sd, cpu, pc, reason, sigrc);
      break;
#endif
/* end-sanitize-m32rx */
    default :
      abort ();
    }
}

void
sim_resume (sd, step, siggnal)
     SIM_DESC sd;
     int step, siggnal;
{
  sim_module_resume (sd);

  switch (STATE_ARCHITECTURE (sd)->mach)
    {
    case bfd_mach_m32r :
      m32r_engine_run (sd, step, siggnal);
      break;
/* start-sanitize-m32rx */
#ifdef HAVE_CPU_M32RX
    case bfd_mach_m32rx :
      m32rx_engine_run (sd, step, siggnal);
      break;
#endif
/* end-sanitize-m32rx */
    default :
      abort ();
    }

  sim_module_suspend (sd);
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
				     CPU_M32R_MISC_PROFILE (cpu).fillnop_count));
    }
}

/* The contents of BUF are in target byte order.  */

int
sim_fetch_register (sd, rn, buf, length)
     SIM_DESC sd;
     int rn;
     unsigned char *buf;
     int length;
{
  SIM_CPU *cpu = STATE_CPU (sd, 0);

  return (* CPU_REG_FETCH (cpu)) (cpu, rn, buf, length);
}
 
/* The contents of BUF are in target byte order.  */

int
sim_store_register (sd, rn, buf, length)
     SIM_DESC sd;
     int rn;
     unsigned char *buf;
     int length;
{
  SIM_CPU *cpu = STATE_CPU (sd, 0);

  return (* CPU_REG_STORE (cpu)) (cpu, rn, buf, length);
}

void
sim_do_command (sd, cmd)
     SIM_DESC sd;
     char *cmd;
{ 
  if (sim_args_command (sd, cmd) != SIM_RC_OK)
    sim_io_eprintf (sd, "Unknown command `%s'\n", cmd);
}

/* The semantic code invokes this for illegal (unrecognized) instructions.  */

void
sim_engine_illegal_insn (current_cpu, pc)
     SIM_CPU *current_cpu;
     PCADDR pc;
{
  sim_engine_halt (CPU_STATE (current_cpu), current_cpu, NULL, pc,
		   sim_stopped, SIM_SIGILL);
}

/* Utility fns to access registers, without knowing the current mach.  */

SI
h_gr_get (SIM_CPU *current_cpu, UINT regno)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
    case bfd_mach_m32r :
      return m32r_h_gr_get (current_cpu, regno);
/* start-sanitize-m32rx */
#ifdef HAVE_CPU_M32RX
    case bfd_mach_m32rx :
      return m32rx_h_gr_get (current_cpu, regno);
#endif
/* end-sanitize-m32rx */
    default :
      abort ();
    }
}

void
h_gr_set (SIM_CPU *current_cpu, UINT regno, SI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
    case bfd_mach_m32r :
      m32r_h_gr_set (current_cpu, regno, newval);
      break;
/* start-sanitize-m32rx */
#ifdef HAVE_CPU_M32RX
    case bfd_mach_m32rx :
      m32rx_h_gr_set (current_cpu, regno, newval);
      break;
#endif
/* end-sanitize-m32rx */
    default :
      abort ();
    }
}

/* Read/write functions for system call interface.  */

static int
syscall_read_mem (host_callback *cb, struct cb_syscall *sc,
		  unsigned long taddr, char *buf, int bytes)
{
  SIM_DESC sd = (SIM_DESC) sc->p1;
  SIM_CPU *cpu = (SIM_CPU *) sc->p2;

  return sim_core_read_buffer (sd, cpu, read_map, buf, taddr, bytes);
}

static int
syscall_write_mem (host_callback *cb, struct cb_syscall *sc,
		   unsigned long taddr, const char *buf, int bytes)
{
  SIM_DESC sd = (SIM_DESC) sc->p1;
  SIM_CPU *cpu = (SIM_CPU *) sc->p2;

  return sim_core_write_buffer (sd, cpu, write_map, buf, taddr, bytes);
}

/* Trap support.
   The result is the pc address to continue at.  */

USI
do_trap (SIM_CPU *current_cpu, int num)
{
  SIM_DESC sd = CPU_STATE (current_cpu);
  host_callback *cb = STATE_CALLBACK (sd);

#ifdef SIM_HAVE_BREAKPOINTS
  /* Check for breakpoints "owned" by the simulator first, regardless
     of --environment.  */
  if (num == 1)
    {
      /* First try sim-break.c.  If it's a breakpoint the simulator "owns"
	 it doesn't return.  Otherwise it returns and let's us try.  */
      sim_handle_breakpoint (sd, current_cpu, sim_pc_get (current_cpu));
      /* Fall through.  */
    }
#endif

  if (STATE_ENVIRONMENT (sd) == OPERATING_ENVIRONMENT)
    {
      /* The new pc is the trap vector entry.
	 We assume there's a branch there to some handler.  */
      USI new_pc = num * 4;
      return new_pc;
    }

  switch (num)
    {
    case 0 :
      /* Trap 0 is used for system calls.  */
      {
	CB_SYSCALL s;

	CB_SYSCALL_INIT (&s);
	s.func = h_gr_get (current_cpu, 0);
	s.arg1 = h_gr_get (current_cpu, 1);
	s.arg2 = h_gr_get (current_cpu, 2);
	s.arg3 = h_gr_get (current_cpu, 3);

	if (s.func == TARGET_SYS_exit)
	  {
	    sim_engine_halt (sd, current_cpu, NULL, sim_pc_get (current_cpu),
			     sim_exited, s.arg1);
	  }

	s.p1 = (PTR) sd;
	s.p2 = (PTR) current_cpu;
	s.read_mem = syscall_read_mem;
	s.write_mem = syscall_write_mem;
	cb_syscall (STATE_CALLBACK (sd), &s);
	h_gr_set (current_cpu, 2, s.errcode);
	h_gr_set (current_cpu, 0, s.result);
	h_gr_set (current_cpu, 1, s.result2);
	break;
      }

    case 1:	/* breakpoint trap */
      sim_engine_halt (sd, current_cpu, NULL, NULL_CIA,
		       sim_stopped, SIM_SIGTRAP);
      break;

    default :
      {
	USI new_pc = num * 4;
	return new_pc;
      }
    }

  /* Fake an "rte" insn.  */
  return (sim_pc_get (current_cpu) & -4) + 4;
}
