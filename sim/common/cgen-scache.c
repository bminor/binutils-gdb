/* Simulator cache routines for CGEN simulators (and maybe others).
   Copyright (C) 1996, 1997 Free Software Foundation, Inc.
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

#define SCACHE_P
#define SCACHE_DEFINE_INLINE

#include "sim-main.h"
#include "libiberty.h"
#include "cgen-scache.h"
#include "sim-options.h"
#include "sim-io.h"

/* Unused address.  */
#define UNUSED_ADDR 0xffffffff

static MODULE_INIT_FN scache_init;
static MODULE_UNINSTALL_FN scache_uninstall;

static DECLARE_OPTION_HANDLER (scache_option_handler);

#define OPTION_PROFILE_SCACHE	(OPTION_START + 0)

static const OPTION scache_options[] = {
  { {"scache-size", optional_argument, NULL, 'c'},
      'c', "[SIZE]", "Specify size of simulator execution cache",
      scache_option_handler },
  { {"profile-scache", no_argument, NULL, OPTION_PROFILE_SCACHE},
      '\0', NULL, "Perform simulator execution cache profiling",
      scache_option_handler },
  { {NULL, no_argument, NULL, 0}, '\0', NULL, NULL, NULL }
};

static SIM_RC
scache_option_handler (sd, opt, arg)
     SIM_DESC sd;
     int opt;
     char *arg;
{
  int n;

  switch (opt)
    {
    case 'c' :
      if (WITH_SCACHE)
	{
	  if (arg != NULL)
	    {
	      int n = strtol (arg, NULL, 0);
	      /* The m32r port assumes a cache size of at least 2 so it
		 can decode both 16 bit insns.  */
	      if (n < 2)
		{
		  sim_io_eprintf (sd, "invalid scache size `%d'", n);
		  return SIM_RC_FAIL;
		}
	      /* Ensure it's a multiple of 2.  */
	      if ((n & (n - 1)) != 0)
		{
		  sim_io_eprintf (sd, "scache size `%d' not a multiple of 2\n", n);
		  {
		    /* round up to nearest multiple of 2 */
		    int i;
		    for (i = 1; i < n; i <<= 1)
		      continue;
		    n = i;
		    
		  }
		  sim_io_eprintf (sd, "rounding scache size up to %d\n", n);
		}
	      STATE_SCACHE_SIZE (sd) = n;
	    }
	  else
	    STATE_SCACHE_SIZE (sd) = SCACHE_DEFAULT_CACHE_SIZE;
	}
      else
	sim_io_eprintf (sd, "Simulator execution cache not enabled, `--scache-size' ignored\n");
      break;

    case OPTION_PROFILE_SCACHE :
      if (WITH_SCACHE && WITH_PROFILE_SCACHE_P)
	for (n = 0; n < MAX_NR_PROCESSORS; ++n)
	  CPU_PROFILE_FLAGS (STATE_CPU (sd, n))[PROFILE_SCACHE_IDX] = 1;
      else
	sim_io_eprintf (sd, "Simulator cache profiling not compiled in, `--profile-scache' ignored\n");
      break;

    }

  return SIM_RC_OK;
}

SIM_RC
scache_install (SIM_DESC sd)
{
  sim_add_option_table (sd, scache_options);
  sim_module_add_init_fn (sd, scache_init);
  sim_module_add_uninstall_fn (sd, scache_uninstall);

  /* This is the default, it may be overridden on the command line.  */
  STATE_SCACHE_SIZE (sd) = WITH_SCACHE;

  return SIM_RC_OK;
}

static SIM_RC
scache_init (SIM_DESC sd)
{
  int c;

  for (c = 0; c < MAX_NR_PROCESSORS; ++c)
    {
      SIM_CPU *cpu = STATE_CPU (sd, c);

      CPU_SCACHE_SIZE (cpu) = STATE_SCACHE_SIZE (sd);
      CPU_SCACHE_CACHE (cpu) = (SCACHE *)
	xmalloc (CPU_SCACHE_SIZE (cpu) * sizeof (SCACHE));
    }

  scache_flush (sd);

  return SIM_RC_OK;
}

static void
scache_uninstall (SIM_DESC sd)
{
  int c;

  for (c = 0; c < MAX_NR_PROCESSORS; ++c)
    {
      SIM_CPU *cpu = STATE_CPU (sd, c);

      if (CPU_SCACHE_CACHE (cpu) != NULL)
	free (CPU_SCACHE_CACHE (cpu));
    }
}

void
scache_flush (SIM_DESC sd)
{
  int i,c;
  SCACHE *sc;

  for (c = 0; c < MAX_NR_PROCESSORS; ++c)
    {
      SIM_CPU *cpu = STATE_CPU (sd, c);

      /* Technically, this may not be necessary, but it helps debugging.  */
      memset (CPU_SCACHE_CACHE (cpu), 0,
	      CPU_SCACHE_SIZE (cpu) * sizeof (SCACHE));

      for (i = 0, sc = CPU_SCACHE_CACHE (cpu); i < CPU_SCACHE_SIZE (cpu);
	   ++i, ++sc)
	{
	  sc->argbuf.addr = UNUSED_ADDR;
	}
    }
}

void
scache_print_profile (SIM_DESC sd, int verbose)
{
  /* FIXME: Need to add smp support.  */
  SIM_CPU *cpu = STATE_CPU (sd, 0);
  unsigned long hits = CPU_SCACHE_HITS (cpu);
  unsigned long misses = CPU_SCACHE_MISSES (cpu);

  sim_io_printf (sd, "Simulator Cache Statistics\n\n");

  sim_io_printf (sd, "Cache size: %d\n", CPU_SCACHE_SIZE (cpu));
  sim_io_printf (sd, "Hits:       %d\n", hits);
  sim_io_printf (sd, "Misses:     %d\n", misses);
  if (hits + misses != 0)
    sim_io_printf (sd, "Hit rate:   %.2f%%\n",
		   ((double) hits / ((double) hits + (double) misses)) * 100);
  sim_io_printf (sd, "\n");
}
