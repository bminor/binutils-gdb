/* Main simulator entry points for the SPARC.
   Copyright (C) 1999 Cygnus Solutions.  */

#include "sim-main.h"
#include <signal.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include "libiberty.h"
#include "bfd.h"
#include "sim-core.h"
#include "dev32.h"

/* FIXME: Do we *need* to pass state to the semantic routines?  */
SIM_DESC current_state;

static MODULE_SUSPEND_FN regwin_suspend;

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
  char c;
  int i;
  SIM_DESC sd = sim_state_alloc (kind, callback);

  /* The cpu data is kept in a separately allocated chunk of memory.  */
  if (sim_cpu_alloc_all (sd, 1, cgen_cpu_max_extra_bytes ()) != SIM_RC_OK)
    {
      free_state (sd);
      return 0;
    }

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

  /* Make the default --environment=operating to be compatible with erc32.  */
  STATE_ENVIRONMENT (sd) = OPERATING_ENVIRONMENT;

  /* getopt will print the error message so we just have to exit if this fails.
     FIXME: Hmmm...  in the case of gdb we need getopt to call
     print_filtered.  */
  if (sim_parse_args (sd, argv) != SIM_RC_OK)
    {
      free_state (sd);
      return 0;
    }

  /* check for/establish the a reference program image */
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

  /* Initialize various cgen things not done by common framework.  */
  cgen_init (sd);

  /* Open a copy of the opcode table.  */
  STATE_OPCODE_TABLE (sd) = sparc_cgen_opcode_open (STATE_ARCHITECTURE (sd)->mach,
						    CGEN_ENDIAN_BIG);
  sparc_cgen_init_dis (STATE_OPCODE_TABLE (sd));

  /* Allocate a handler for the control registers and other devices
     if no memory for that range has been allocated by the user.
     All are allocated in one chunk to keep things from being
     unnecessarily complicated.  */
  if (sim_core_read_buffer (sd, NULL, read_map, &c, ERC32_DEVICE_ADDR, 1) == 0)
    sim_core_attach (sd, NULL,
		     0 /*level*/,
		     access_read_write,
		     0 /*space ???*/,
		     ERC32_DEVICE_ADDR, ERC32_DEVICE_LEN /*nr_bytes*/,
		     0 /*modulo*/,
		     &sparc_devices,
		     NULL /*buffer*/);

  /* Allocate core managed memory if none specified by user.  */
  /* Use address 4 here in case the user wanted address 0 unmapped.  */
  if (sim_core_read_buffer (sd, NULL, read_map, &c, 4, 1) == 0)
    sim_do_commandf (sd, "memory region 0,0x%x", SPARC_DEFAULT_MEM_SIZE);
  /* FIXME: magic number */
  if (sim_core_read_buffer (sd, NULL, read_map, &c, 0x2000000, 1) == 0)
    sim_do_commandf (sd, "memory region 0x%x,0x%x", 0x2000000, 0x200000);

#ifdef HAVE_SPARC32
  if (! ARCH64_P (sd))
    {
      sparc32_alloc_regwins (STATE_CPU (sd, 0), NWINDOWS);
    }
#endif
#ifdef HAVE_SPARC64
  if (ARCH64_P (sd))
    {
      sparc64_alloc_regwins (STATE_CPU (sd, 0), NWINDOWS);
    }
#endif

  sim_module_add_suspend_fn (sd, regwin_suspend);

  /* Perform a cold-reset of the cpu(s).  */

  for (i = 0; i < MAX_NR_PROCESSORS; ++i)
    sparc32_cold_reset (STATE_CPU (sd, i), 0);

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
#ifdef HAVE_SPARC32
  if (! ARCH64_P (sd))
    sparc32_free_regwins (STATE_CPU (sd, 0));
#endif
#ifdef HAVE_SPARC64
  if (ARCH64_P (sd))
    sparc64_free_regwins (STATE_CPU (sd, 0));
#endif
  sim_module_uninstall (sd);
}

SIM_RC
sim_create_inferior (sd, abfd, argv, envp)
     SIM_DESC sd;
     struct _bfd *abfd;
     char **argv;
     char **envp;
{
  SIM_ADDR addr;

  if (abfd)
    addr = bfd_get_start_address (abfd);
  else
    addr = 0;

#if 0
  STATE_ARGV (sd) = sim_copy_argv (argv);
  STATE_ENVP (sd) = sim_copy_argv (envp);
#endif

#ifdef HAVE_SPARC32
  if (! ARCH64_P (sd))
    {
      sparc32_cold_reset (STATE_CPU (sd, 0), 1);
      sparc32_init_pc (STATE_CPU (sd, 0), addr, addr + 4);
    }
#endif
#ifdef HAVE_SPARC64
  if (ARCH64_P (sd))
    {
      sparc64_cold_reset (STATE_CPU (sd, 0), 1);
      sparc64_init_pc (STATE_CPU (sd, 0), addr, addr + 4);
    }
#endif

  return SIM_RC_OK;
}

/* Flush the register windows when we suspend so we can walk the stack
   in gdb.
   ??? Extremely inefficient in case where gdb is running a program
   by stepping it.
   ??? Also interferes with debugging of --environment=operating.  */

static SIM_RC
regwin_suspend (SIM_DESC sd)
{
  int c;

  for (c = 0; c < MAX_NR_PROCESSORS; ++c)
    {
      SIM_CPU *cpu = STATE_CPU (sd, c);

#ifdef HAVE_SPARC32
      if (ARCH32_P (cpu))
	sparc32_flush_regwins (cpu, sim_pc_get (cpu), 1 /* no errors */);
#endif
#ifdef HAVE_SPARC64
      if (ARCH64_P (cpu))
	sparc64_flush_regwins (cpu, sim_pc_get (cpu), 1 /* no errors */);
#endif
    }

  return SIM_RC_OK;
}

void
sim_do_command (sd, cmd)
     SIM_DESC sd;
     char *cmd;
{ 
  if (sim_args_command (sd, cmd) != SIM_RC_OK)
    sim_io_eprintf (sd, "Unknown command `%s'\n", cmd);
}
