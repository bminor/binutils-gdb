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
#include "sim-assert.h"


static void free_state (SIM_DESC);

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

  sim_cpu_alloc_all (sd, 1, 0);

  if (sim_pre_argv_init (sd, argv[0]) != SIM_RC_OK)
    {
      free_state (sd);
      return 0;
    }

  /* Allocate core managed memory */
  sim_do_commandf (sd, "memory region 0,0x%lx", M32R_DEFAULT_MEM_SIZE);

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

  /* Store in a global so things like sparc32_dump_regs can be invoked
     from the gdb command line.  */
  current_state = sd;
  sim_gx_read_block_list();
  return sd;
}

void
sim_close (sd, quitting)
     SIM_DESC sd;
     int quitting;
{
  sim_gx_write_block_list();
  sim_module_uninstall (sd);
}


USI
m32r_h_pc_get (SIM_CPU *current_cpu)
{
  return current_cpu->regs.h_pc;
}

/* Set a value for h-pc.  */

void
m32r_h_pc_set (SIM_CPU *current_cpu, USI newval)
{
  current_cpu->regs.h_pc = newval;
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

  CPU_PC_STORE (current_cpu) = m32r_h_pc_set;
  CPU_PC_FETCH (current_cpu) = m32r_h_pc_get;
  CPU_REG_STORE (current_cpu) = NULL;
  CPU_REG_FETCH (current_cpu) = NULL;

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
  sim_io_error(sd, "cannot sim_stop\n");
}

/* This isn't part of the official interface.
   This is just a good place to put this for now.  */

void
sim_sync_stop (SIM_DESC sd, SIM_CPU *cpu, PCADDR pc, enum sim_stop reason, int sigrc)
{
  sim_io_error(sd, "cannot sim_stop\n");
}

void
sim_resume (sd, step, siggnal)
     SIM_DESC sd;
     int step, siggnal;
{
  sim_engine *engine = STATE_ENGINE (sd);
  jmp_buf buf;
  int jmpval;

  sim_module_resume (sd);
  ASSERT(step == 0); /* XXX */

  engine->jmpbuf = &buf;
  jmpval = setjmp (buf);
  if(jmpval == 0)
    sim_engine_run(sd, 0, 1, siggnal);
  engine->jmpbuf = NULL;

  sim_module_suspend (sd);
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

/* Utility fns to access registers, without knowing the current mach.  */

SI
h_gr_get (SIM_CPU *current_cpu, UINT regno)
{
  SIM_DESC sd = CURRENT_STATE;
  sim_io_error(sd, "cannot h_gr_get\n");
}

void
h_gr_set (SIM_CPU *current_cpu, UINT regno, SI newval)
{
  SIM_DESC sd = CURRENT_STATE;
  sim_io_error(sd, "cannot h_gr_set\n");
}
