/* Default profiling support.
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

#include "sim-main.h"
#include "sim-io.h"
#include "sim-options.h"

#define COMMAS(n) sim_add_commas (comma_buf, sizeof (comma_buf), (n))

static MODULE_UNINSTALL_FN profile_uninstall;

static void print_bar (SIM_DESC, unsigned int, unsigned int, unsigned int);

static DECLARE_OPTION_HANDLER (profile_option_handler);

#define OPTION_PROFILE_INSN	(OPTION_START + 0)
#define OPTION_PROFILE_MEMORY	(OPTION_START + 1)
#define OPTION_PROFILE_MODEL	(OPTION_START + 2)
#define OPTION_PROFILE_FILE	(OPTION_START + 3)
#define OPTION_PROFILE_RANGE	(OPTION_START + 4)

static const OPTION profile_options[] = {
  { {"profile", no_argument, NULL, 'p'},
      'p', NULL, "Perform profiling",
      profile_option_handler },
  { {"profile-insn", no_argument, NULL, OPTION_PROFILE_INSN},
      '\0', NULL, "Perform instruction profiling",
      profile_option_handler },
  { {"profile-memory", no_argument, NULL, OPTION_PROFILE_MEMORY},
      '\0', NULL, "Perform memory profiling",
      profile_option_handler },
  { {"profile-model", no_argument, NULL, OPTION_PROFILE_MODEL},
      '\0', NULL, "Perform model profiling",
      profile_option_handler },
  { {"profile-file", required_argument, NULL, OPTION_PROFILE_FILE},
      '\0', "FILE NAME", "Specify profile output file",
      profile_option_handler },
  { {"profile-pc-frequency", required_argument, NULL, 'F'},
      'F', "PC PROFILE FREQUENCY", "Turn on PC profiling at specified frequency",
      profile_option_handler },
  { {"profile-pc-size", required_argument, NULL, 'S'},
      'S', "PC PROFILE SIZE", "Specify PC profiling size",
      profile_option_handler },
#if 0 /*FIXME:wip*/
  { {"profile-range", required_argument, NULL, OPTION_PROFILE_RANGE},
      0, NULL, "Specify range of addresses to profile",
      profile_option_handler },
#endif
  { {NULL, no_argument, NULL, 0}, '\0', NULL, NULL, NULL }
};

static SIM_RC
profile_option_handler (SIM_DESC sd, int opt, char *arg)
{
  int i,n;

  switch (opt)
    {
    case 'p' :
      if (! WITH_PROFILE)
	sim_io_eprintf (sd, "Profiling not compiled in, -p option ignored\n");
      else
	{
	  for (n = 0; n < MAX_NR_PROCESSORS; ++n)
	    for (i = 0; i < MAX_PROFILE_VALUES; ++i)
	      CPU_PROFILE_FLAGS (STATE_CPU (sd, n))[i] = 1;
	}
      break;

    case OPTION_PROFILE_INSN :
#if WITH_PROFILE_INSN_P
      for (n = 0; n < MAX_NR_PROCESSORS; ++n)
	CPU_PROFILE_FLAGS (STATE_CPU (sd, n))[PROFILE_INSN_IDX] = 1;
#else
      sim_io_eprintf (sd, "Instruction profiling not compiled in, `--profile-insn' ignored\n");
#endif
      break;

    case OPTION_PROFILE_MEMORY :
#if WITH_PROFILE_MEMORY_P
      for (n = 0; n < MAX_NR_PROCESSORS; ++n)
	CPU_PROFILE_FLAGS (STATE_CPU (sd, n))[PROFILE_MEMORY_IDX] = 1;
#else
      sim_io_eprintf (sd, "Memory profiling not compiled in, `--profile-memory' ignored\n");
#endif
      break;

    case OPTION_PROFILE_MODEL :
#if WITH_PROFILE_MODEL_P
      for (n = 0; n < MAX_NR_PROCESSORS; ++n)
	CPU_PROFILE_FLAGS (STATE_CPU (sd, n))[PROFILE_MODEL_IDX] = 1;
#else
      sim_io_eprintf (sd, "Model profiling not compiled in, `--profile-model' ignored\n");
#endif
      break;

    case OPTION_PROFILE_FILE :
      /* FIXME: Might want this to apply to pc profiling only,
	 or have two profile file options.  */
      if (! WITH_PROFILE)
	sim_io_eprintf (sd, "Profiling not compiled in, `--profile-file' ignored\n");
      else
	{
	  FILE *f = fopen (arg, "w");

	  if (f == NULL)
	    {
	      sim_io_eprintf (sd, "Unable to open profile output file `%s'\n", arg);
	      return SIM_RC_FAIL;
	    }
	  for (n = 0; n < MAX_NR_PROCESSORS; ++n)
	    PROFILE_FILE (CPU_PROFILE_DATA (STATE_CPU (sd, n))) = f;
	}
      break;

    case 'F' :
#if WITH_PROFILE_PC_P
      /* FIXME: Validate arg.  */
      i = atoi (arg);
      for (n = 0; n < MAX_NR_PROCESSORS; ++n)
	PROFILE_PC_FREQ (CPU_PROFILE_DATA (STATE_CPU (sd, n))) = i;
#else
      sim_io_eprintf (sd, "PC profiling not compiled in, `--profile-pc-frequency' ignored\n");
#endif
      break;

    case 'S' :
#if WITH_PROFILE_PC_P
      /* FIXME: Validate arg.  */
      i = atoi (arg);
      for (n = 0; n < MAX_NR_PROCESSORS; ++n)
	PROFILE_PC_SIZE (CPU_PROFILE_DATA (STATE_CPU (sd, n))) = i;
#else
      sim_io_eprintf (sd, "PC profiling not compiled in, `--profile-pc-size' ignored\n");
#endif
      break;

#if 0 /* FIXME:wip */
    case OPTION_PROFILE_RANGE :
      break;
#endif
    }

  return SIM_RC_OK;
}

/* Install profiling support in the simulator.  */

SIM_RC
profile_install (SIM_DESC sd)
{
  int i;

  sim_add_option_table (sd, profile_options);
  for (i = 0; i < MAX_NR_PROCESSORS; ++i)
    memset (CPU_PROFILE_DATA (STATE_CPU (sd, i)), 0,
	    sizeof (* CPU_PROFILE_DATA (STATE_CPU (sd, i))));
  sim_module_add_uninstall_fn (sd, profile_uninstall);
  return SIM_RC_OK;
}

static void
profile_uninstall (SIM_DESC sd)
{
  int i;

  for (i = 0; i < MAX_NR_PROCESSORS; ++i)
    {
      PROFILE_DATA *data = CPU_PROFILE_DATA (STATE_CPU (sd, i));
      if (PROFILE_FILE (data) != NULL)
	fclose (PROFILE_FILE (data));
    }
}

/* Summary printing support.  */

#if WITH_PROFILE_INSN_P

static void
profile_print_insn (sim_cpu *cpu, int verbose)
{
  unsigned int i, n, total, max_val, max_name_len;
  SIM_DESC sd = CPU_STATE (cpu);
  PROFILE_DATA *data = CPU_PROFILE_DATA (cpu);
  char comma_buf[20];

  sim_io_printf (sd, "Instruction Statistics\n\n");

  /* First pass over data computes various things.  */
  max_val = total = max_name_len = 0;
  for (i = 1; i < MAX_INSNS; ++i)
    {
      total += PROFILE_INSN_COUNT (data) [i];
      if (PROFILE_INSN_COUNT (data) [i] > max_val)
	max_val = PROFILE_INSN_COUNT (data) [i];
      n = strlen (INSN_NAME (i));
      if (n > max_name_len)
	max_name_len = n;
    }

  sim_io_printf (sd, "  Total: %s insns\n", COMMAS (total));

  if (verbose && max_val != 0)
    {
      /* Now we can print the histogram.  */
      sim_io_printf (sd, "\n");
      for (i = 1; i < MAX_INSNS; ++i)
	{
	  if (PROFILE_INSN_COUNT (data) [i] != 0)
	    {
	      sim_io_printf (sd, "   %*s: %*s: ",
			     max_name_len, INSN_NAME (i),
			     max_val < 10000 ? 5 : 10,
			     COMMAS (PROFILE_INSN_COUNT (data) [i]));
	      print_bar (sd, PROFILE_HISTOGRAM_WIDTH,
			 PROFILE_INSN_COUNT (data) [i],
			 max_val);
	      sim_io_printf (sd, "\n");
	    }
	}
    }

  sim_io_printf (sd, "\n");
}

#endif

#if WITH_PROFILE_MEMORY_P

static void
profile_print_memory (sim_cpu *cpu, int verbose)
{
  unsigned int i, n;
  unsigned int total_read, total_write;
  unsigned int max_val, max_name_len;
  /* FIXME: Need to add smp support.  */
  SIM_DESC sd = CPU_STATE (cpu);
  PROFILE_DATA *data = CPU_PROFILE_DATA (cpu);
  char comma_buf[20];

  sim_io_printf (sd, "Memory Access Statistics\n\n");

  /* First pass over data computes various things.  */
  max_val = total_read = total_write = max_name_len = 0;
  for (i = 0; i < MAX_MODES; ++i)
    {
      total_read += PROFILE_READ_COUNT (data) [i];
      total_write += PROFILE_WRITE_COUNT (data) [i];
      if (PROFILE_READ_COUNT (data) [i] > max_val)
	max_val = PROFILE_READ_COUNT (data) [i];
      if (PROFILE_WRITE_COUNT (data) [i] > max_val)
	max_val = PROFILE_WRITE_COUNT (data) [i];
      n = strlen (MODE_NAME (i));
      if (n > max_name_len)
	max_name_len = n;
    }

  /* One could use PROFILE_LABEL_WIDTH here.  I chose not to.  */
  sim_io_printf (sd, "  Total read:  %s accesses\n",
		 COMMAS (total_read));
  sim_io_printf (sd, "  Total write: %s accesses\n",
		 COMMAS (total_write));

  if (verbose && max_val != 0)
    {
      /* FIXME: Need to separate instruction fetches from data fetches
	 as the former swamps the latter.  */
      /* Now we can print the histogram.  */
      sim_io_printf (sd, "\n");
      for (i = 0; i < MAX_MODES; ++i)
	{
	  if (PROFILE_READ_COUNT (data) [i] != 0)
	    {
	      sim_io_printf (sd, "   %*s read:  %*s: ",
			     max_name_len, MODE_NAME (i),
			     max_val < 10000 ? 5 : 10,
			     COMMAS (PROFILE_READ_COUNT (data) [i]));
	      print_bar (sd, PROFILE_HISTOGRAM_WIDTH,
			 PROFILE_READ_COUNT (data) [i],
			 max_val);
	      sim_io_printf (sd, "\n");
	    }
	  if (PROFILE_WRITE_COUNT (data) [i] != 0)
	    {
	      sim_io_printf (sd, "   %*s write: %*s: ",
			     max_name_len, MODE_NAME (i),
			     max_val < 10000 ? 5 : 10,
			     COMMAS (PROFILE_WRITE_COUNT (data) [i]));
	      print_bar (sd, PROFILE_HISTOGRAM_WIDTH,
			 PROFILE_WRITE_COUNT (data) [i],
			 max_val);
	      sim_io_printf (sd, "\n");
	    }
	}
    }

  sim_io_printf (sd, "\n");
}

#endif

#if WITH_PROFILE_MODEL_P

static void
profile_print_model (sim_cpu *cpu, int verbose)
{
  SIM_DESC sd = CPU_STATE (cpu);
  PROFILE_DATA *data = CPU_PROFILE_DATA (cpu);
  unsigned long cti_stalls = PROFILE_MODEL_CTI_STALL_COUNT (data);
  unsigned long load_stalls = PROFILE_MODEL_LOAD_STALL_COUNT (data);
  unsigned long total = PROFILE_MODEL_CYCLE_COUNT (data)
    + cti_stalls + load_stalls;
  char comma_buf[20];

  sim_io_printf (sd, "Model %s Timing Information\n\n",
		 MODEL_NAME (STATE_MODEL (sd)));
  sim_io_printf (sd, "  %-*s %s\n",
		 PROFILE_LABEL_WIDTH, "Taken branches:",
		 COMMAS (PROFILE_MODEL_TAKEN_COUNT (data)));
  sim_io_printf (sd, "  %-*s %s\n",
		 PROFILE_LABEL_WIDTH, "Untaken branches:",
		 COMMAS (PROFILE_MODEL_UNTAKEN_COUNT (data)));
  sim_io_printf (sd, "  %-*s %s\n",
		 PROFILE_LABEL_WIDTH, "Cycles stalled due to branches:",
		 COMMAS (cti_stalls));
  sim_io_printf (sd, "  %-*s %s\n",
		 PROFILE_LABEL_WIDTH, "Cycles stalled due to loads:",
		 COMMAS (load_stalls));
  sim_io_printf (sd, "  %-*s %s\n",
		 PROFILE_LABEL_WIDTH, "Total cycles (*approximate*):",
		 COMMAS (total));
  sim_io_printf (sd, "\n");
}

#endif

static void
print_bar (SIM_DESC sd, unsigned int width,
	   unsigned int val, unsigned int max_val)
{
  unsigned int i, count;

  count = ((double) val / (double) max_val) * (double) width;

  for (i = 0; i < count; ++i)
    sim_io_printf (sd, "*");
}

/* Print the simulator's execution speed for CPU.  */

static void
profile_print_speed (sim_cpu *cpu)
{
  SIM_DESC sd = CPU_STATE (cpu);
  PROFILE_DATA *data = CPU_PROFILE_DATA (cpu);
  unsigned long milliseconds = PROFILE_EXEC_TIME (data);
  unsigned long total = PROFILE_TOTAL_INSN_COUNT (data);
  char comma_buf[20];

  sim_io_printf (sd, "Simulator Execution Speed\n\n");

  if (total != 0)
    sim_io_printf (sd, "  Total instructions:   %s\n", COMMAS (total));

  if (milliseconds < 1000)
    sim_io_printf (sd, "  Total Execution Time: < 1 second\n\n");
  else
    {
      /* The printing of the time rounded to 2 decimal places makes the speed
	 calculation seem incorrect [even though it is correct].  So round
	 MILLISECONDS first. This can marginally affect the result, but it's
	 better that the user not perceive there's a math error.  */
      double secs = (double) milliseconds / 1000;
      secs = ((double) (unsigned long) (secs * 100 + .5)) / 100;
      sim_io_printf (sd, "  Total Execution Time: %.2f seconds\n", secs);
      /* Don't confuse things with data that isn't useful.
	 If we ran for less than 2 seconds, only use the data if we
	 executed more than 100,000 insns.  */
      if (secs >= 2 || total >= 100000)
	sim_io_printf (sd, "  Simulator Speed:      %s insns/second\n\n",
		       COMMAS ((unsigned long) ((double) total / secs)));
    }
}

/* Top level function to print all summary profile information.
   It is [currently] intended that all such data is printed by this function.
   I'd rather keep it all in one place for now.  To that end, MISC_CPU and
   MISC are callbacks used to print any miscellaneous data.

   One might want to add a user option that allows printing by type or by cpu
   (i.e. print all insn data for each cpu first, or print data cpu by cpu).
   This may be a case of featuritis so it's currently left out.

   Note that results are indented two spaces to distinguish them from
   section titles.  */

void
profile_print (SIM_DESC sd, int verbose,
	       PROFILE_CALLBACK *misc, PROFILE_CPU_CALLBACK *misc_cpu)
{
  int i,c;
  int print_title_p = 0;

  /* Only print the title if some data has been collected.  */
  /* FIXME: If the number of processors can be selected on the command line,
     then MAX_NR_PROCESSORS will need to take an argument of `sd'.  */

  for (c = 0; c < MAX_NR_PROCESSORS; ++c)
    {
      sim_cpu *cpu = STATE_CPU (sd, c);
      PROFILE_DATA *data = CPU_PROFILE_DATA (cpu);

      for (i = 0; i < MAX_PROFILE_VALUES; ++i)
	if (PROFILE_FLAGS (data) [i])
	  print_title_p = 1;
      /* One could break out early if print_title_p is set.  */
    }
  if (print_title_p)
    sim_io_printf (sd, "Summary profiling results:\n\n");

  /* Loop, cpu by cpu, printing results.  */

  for (c = 0; c < MAX_NR_PROCESSORS; ++c)
    {
      sim_cpu *cpu = STATE_CPU (sd, c);
      PROFILE_DATA *data = CPU_PROFILE_DATA (cpu);

      if (MAX_NR_PROCESSORS > 1)
	sim_io_printf (sd, "CPU %d\n\n", c);

#if WITH_PROFILE_INSN_P
      if (PROFILE_FLAGS (data) [PROFILE_INSN_IDX])
	profile_print_insn (cpu, verbose);
#endif

#if WITH_PROFILE_MEMORY_P
      if (PROFILE_FLAGS (data) [PROFILE_MEMORY_IDX])
	profile_print_memory (cpu, verbose);
#endif

#if WITH_PROFILE_MODEL_P
      if (PROFILE_FLAGS (data) [PROFILE_MODEL_IDX])
	profile_print_model (cpu, verbose);
#endif

#if WITH_PROFILE_SCACHE_P && WITH_SCACHE
      if (PROFILE_FLAGS (data) [PROFILE_SCACHE_IDX])
	scache_print_profile (cpu, verbose);
#endif

      /* Print cpu-specific data before the execution speed.  */
      if (misc_cpu != NULL)
	(*misc_cpu) (cpu, verbose);

      /* Always try to print execution time and speed.  */
      if (verbose
	  || PROFILE_FLAGS (data) [PROFILE_INSN_IDX])
	profile_print_speed (cpu);
    }

  /* Finally print non-cpu specific miscellaneous data.  */

  if (misc != NULL)
    (*misc) (sd, verbose);
}
