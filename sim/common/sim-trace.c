/* Simulator tracing/debugging support.
   Copyright (C) 1997 Free Software Foundation, Inc.
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
#include "bfd.h"

#include "sim-assert.h"

#ifdef HAVE_STRING_H
#include <string.h>
#else
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#endif

#ifndef SIZE_PHASE
#define SIZE_PHASE 8
#endif

#ifndef SIZE_LOCATION
#define SIZE_LOCATION 20
#endif

#ifndef SIZE_PC
#define SIZE_PC 6
#endif

#ifndef SIZE_LINE_NUMBER
#define SIZE_LINE_NUMBER 4
#endif

static MODULE_UNINSTALL_FN trace_uninstall;

static DECLARE_OPTION_HANDLER (trace_option_handler);

enum {
  OPTION_TRACE_INSN	= OPTION_START,
  OPTION_TRACE_DECODE,
  OPTION_TRACE_EXTRACT,
  OPTION_TRACE_LINENUM,
  OPTION_TRACE_MEMORY,
  OPTION_TRACE_MODEL,
  OPTION_TRACE_ALU,
  OPTION_TRACE_CORE,
  OPTION_TRACE_EVENTS,
  OPTION_TRACE_FPU,
  OPTION_TRACE_BRANCH,
  OPTION_TRACE_SEMANTICS,
  OPTION_TRACE_DEBUG,
  OPTION_TRACE_FILE
};

static const OPTION trace_options[] =
{
  { {"trace", optional_argument, NULL, 't'},
      't', "on|off", "Perform tracing",
      trace_option_handler },
  { {"trace-insn", optional_argument, NULL, OPTION_TRACE_INSN},
      '\0', "on|off", "Perform instruction tracing",
      trace_option_handler },
  { {"trace-decode", optional_argument, NULL, OPTION_TRACE_DECODE},
      '\0', "on|off", "Perform instruction decoding tracing",
      trace_option_handler },
  { {"trace-extract", optional_argument, NULL, OPTION_TRACE_EXTRACT},
      '\0', "on|off", "Perform instruction extraction tracing",
      trace_option_handler },
  { {"trace-linenum", optional_argument, NULL, OPTION_TRACE_LINENUM},
      '\0', "on|off", "Perform line number tracing (implies --trace-insn)",
      trace_option_handler },
  { {"trace-memory", optional_argument, NULL, OPTION_TRACE_MEMORY},
      '\0', "on|off", "Perform memory tracing",
      trace_option_handler },
  { {"trace-model", optional_argument, NULL, OPTION_TRACE_MODEL},
      '\0', "on|off", "Perform model tracing",
      trace_option_handler },
  { {"trace-alu", optional_argument, NULL, OPTION_TRACE_ALU},
      '\0', "on|off", "Perform ALU tracing",
      trace_option_handler },
  { {"trace-core", optional_argument, NULL, OPTION_TRACE_CORE},
      '\0', "on|off", "Perform CORE tracing",
      trace_option_handler },
  { {"trace-events", optional_argument, NULL, OPTION_TRACE_EVENTS},
      '\0', "on|off", "Perform EVENTS tracing",
      trace_option_handler },
  { {"trace-fpu", optional_argument, NULL, OPTION_TRACE_FPU},
      '\0', "on|off", "Perform FPU tracing",
      trace_option_handler },
  { {"trace-branch", optional_argument, NULL, OPTION_TRACE_BRANCH},
      '\0', "on|off", "Perform branch tracing",
      trace_option_handler },
  { {"trace-semantics", optional_argument, NULL, OPTION_TRACE_SEMANTICS},
      '\0', "on|off", "Perform ALU, FPU, MEMORY, and BRANCH tracing",
      trace_option_handler },
  { {"trace-debug", optional_argument, NULL, OPTION_TRACE_DEBUG},
      '\0', "on|off", "Add information useful for debugging the simulator to the tracing output",
      trace_option_handler },
  { {"trace-file", required_argument, NULL, OPTION_TRACE_FILE},
      '\0', "FILE NAME", "Specify tracing output file",
      trace_option_handler },
  { {NULL, no_argument, NULL, 0}, '\0', NULL, NULL, NULL }
};


/* Set FIRST_TRACE .. LAST_TRACE according to arg.  At least
   FIRST_TRACE is always set */

static SIM_RC
set_trace_options (sd, name, first_trace, last_trace, arg)
     SIM_DESC sd;
     const char *name;
     int first_trace;
     int last_trace;
     const char *arg;
{
  int trace_nr;
  int cpu_nr;
  int trace_val = 1;

  if (arg != NULL)
    {
      if (strcmp (arg, "yes") == 0
	  || strcmp (arg, "on") == 0
	  || strcmp (arg, "1") == 0)
	trace_val = 1;
      else if (strcmp (arg, "no") == 0
	       || strcmp (arg, "off") == 0
	       || strcmp (arg, "0") == 0)
	trace_val = 0;
      else
	{
	  sim_io_eprintf (sd, "Argument `%s' for `--trace%s' invalid, one of `on', `off', `yes', `no' expected\n", arg, name);
	  return SIM_RC_FAIL;
	}
    }

  trace_nr = first_trace;
  do
    {
      /* Set non-cpu specific values.  */
      switch (trace_nr)
	{
	case TRACE_EVENTS_IDX:
	  STATE_EVENTS (sd)->trace = trace_val;
	  break;
	case TRACE_DEBUG_IDX:
	  STATE_TRACE_FLAGS (sd)[trace_nr] = trace_val;
	  break;
	}

      /* Set cpu values.  */
      for (cpu_nr = 0; cpu_nr < MAX_NR_PROCESSORS; cpu_nr++)
	{
	  CPU_TRACE_FLAGS (STATE_CPU (sd, cpu_nr))[trace_nr] = trace_val;
	}
    }
  while (++trace_nr < last_trace);

  return SIM_RC_OK;
}


static SIM_RC
trace_option_handler (sd, opt, arg, is_command)
     SIM_DESC sd;
     int opt;
     char *arg;
     int is_command;
{
  int n;

  switch (opt)
    {
    case 't' :
      if (! WITH_TRACE)
	sim_io_eprintf (sd, "Tracing not compiled in, `-t' ignored\n");
      else
	return set_trace_options (sd, "trace", 0, MAX_TRACE_VALUES, arg);
      break;

    case OPTION_TRACE_INSN :
      if (WITH_TRACE_INSN_P)
	return set_trace_options (sd, "-insn", TRACE_INSN_IDX, -1, arg);
      else
	sim_io_eprintf (sd, "Instruction tracing not compiled in, `--trace-insn' ignored\n");
      break;

    case OPTION_TRACE_DECODE :
      if (WITH_TRACE_DECODE_P)
	return set_trace_options (sd, "-decode", TRACE_DECODE_IDX, -1, arg);
      else
	sim_io_eprintf (sd, "Decode tracing not compiled in, `--trace-decode' ignored\n");
      break;

    case OPTION_TRACE_EXTRACT :
      if (WITH_TRACE_EXTRACT_P)
	return set_trace_options (sd, "-extract", TRACE_EXTRACT_IDX, -1, arg);
      else
	sim_io_eprintf (sd, "Extract tracing not compiled in, `--trace-extract' ignored\n");
      break;

    case OPTION_TRACE_LINENUM :
      if (WITH_TRACE_LINENUM_P && WITH_TRACE_INSN_P)
	{
	  if (set_trace_options (sd, "-linenum", TRACE_LINENUM_IDX, -1, arg) != SIM_RC_OK
	      || set_trace_options (sd, "-linenum", TRACE_INSN_IDX, -1, arg) != SIM_RC_OK)
	    return SIM_RC_FAIL;
	}
      else
	sim_io_eprintf (sd, "Line number or instruction tracing not compiled in, `--trace-linenum' ignored\n");
      break;

    case OPTION_TRACE_MEMORY :
      if (WITH_TRACE_MEMORY_P)
	return set_trace_options (sd, "-memory", TRACE_MEMORY_IDX, -1, arg);
      else
	sim_io_eprintf (sd, "Memory tracing not compiled in, `--trace-memory' ignored\n");
      break;

    case OPTION_TRACE_MODEL :
      if (WITH_TRACE_MODEL_P)
	return set_trace_options (sd, "-model", TRACE_MODEL_IDX, -1, arg);
      else
	sim_io_eprintf (sd, "Model tracing not compiled in, `--trace-model' ignored\n");
      break;

    case OPTION_TRACE_ALU :
      if (WITH_TRACE_ALU_P)
	return set_trace_options (sd, "-alu", TRACE_ALU_IDX, -1, arg);
      else
	sim_io_eprintf (sd, "ALU tracing not compiled in, `--trace-alu' ignored\n");
      break;

    case OPTION_TRACE_CORE :
      if (WITH_TRACE_CORE_P)
	return set_trace_options (sd, "-core", TRACE_CORE_IDX, -1, arg);
      else
	sim_io_eprintf (sd, "CORE tracing not compiled in, `--trace-core' ignored\n");
      break;

    case OPTION_TRACE_EVENTS :
      if (WITH_TRACE_EVENTS_P)
	return set_trace_options (sd, "-events", TRACE_EVENTS_IDX, -1, arg);
      else
	sim_io_eprintf (sd, "EVENTS tracing not compiled in, `--trace-events' ignored\n");
      break;

    case OPTION_TRACE_FPU :
      if (WITH_TRACE_FPU_P)
	return set_trace_options (sd, "-fpu", TRACE_FPU_IDX, -1, arg);
      else
	sim_io_eprintf (sd, "FPU tracing not compiled in, `--trace-fpu' ignored\n");
      break;

    case OPTION_TRACE_BRANCH :
      if (WITH_TRACE_BRANCH_P)
	return set_trace_options (sd, "-branch", TRACE_BRANCH_IDX, -1, arg);
      else
	sim_io_eprintf (sd, "Branch tracing not compiled in, `--trace-branch' ignored\n");
      break;

    case OPTION_TRACE_SEMANTICS :
      if (WITH_TRACE_ALU_P
	  && WITH_TRACE_FPU_P
	  && WITH_TRACE_MEMORY_P
	  && WITH_TRACE_BRANCH_P)
	{
	  if (set_trace_options (sd, "-semantics", TRACE_ALU_IDX, -1, arg) != SIM_RC_OK
	      || set_trace_options (sd, "-semantics", TRACE_FPU_IDX, -1, arg) != SIM_RC_OK
	      || set_trace_options (sd, "-semantics", TRACE_MEMORY_IDX, -1, arg) != SIM_RC_OK
	      || set_trace_options (sd, "-semantics", TRACE_BRANCH_IDX, -1, arg) != SIM_RC_OK)
	    return SIM_RC_FAIL;
	}
      else
	sim_io_eprintf (sd, "Alu, fpu, memory, and/or branch tracing not compiled in, `--trace-semantics' ignored\n");
      break;

    case OPTION_TRACE_DEBUG :
      if (WITH_TRACE_DEBUG_P)
	return set_trace_options (sd, "-debug", TRACE_DEBUG_IDX, -1, arg);
      else
	sim_io_eprintf (sd, "Tracing debug support not compiled in, `--trace-debug' ignored\n");
      break;

    case OPTION_TRACE_FILE :
      if (! WITH_TRACE)
	sim_io_eprintf (sd, "Tracing not compiled in, `--trace-file' ignored\n");
      else
	{
	  FILE *f = fopen (arg, "w");

	  if (f == NULL)
	    {
	      sim_io_eprintf (sd, "Unable to open trace output file `%s'\n", arg);
	      return SIM_RC_FAIL;
	    }
	  for (n = 0; n < MAX_NR_PROCESSORS; ++n)
	    TRACE_FILE (CPU_TRACE_DATA (STATE_CPU (sd, n))) = f;
	  TRACE_FILE (STATE_TRACE_DATA (sd)) = f;
	}
      break;
    }

  return SIM_RC_OK;
}

/* Install tracing support.  */

SIM_RC
trace_install (SIM_DESC sd)
{
  int i;

  SIM_ASSERT (STATE_MAGIC (sd) == SIM_MAGIC_NUMBER);

  sim_add_option_table (sd, trace_options);
  memset (STATE_TRACE_DATA (sd), 0, sizeof (* STATE_TRACE_DATA (sd)));
  for (i = 0; i < MAX_NR_PROCESSORS; ++i)
    memset (CPU_TRACE_DATA (STATE_CPU (sd, i)), 0,
	    sizeof (* CPU_TRACE_DATA (STATE_CPU (sd, i))));
  sim_module_add_uninstall_fn (sd, trace_uninstall);
  return SIM_RC_OK;
}

static void
trace_uninstall (SIM_DESC sd)
{
  int i,j;
  FILE *sfile = TRACE_FILE (STATE_TRACE_DATA (sd));

  if (sfile != NULL)
    fclose (sfile);

  for (i = 0; i < MAX_NR_PROCESSORS; ++i)
    {
      FILE *cfile = TRACE_FILE (CPU_TRACE_DATA (STATE_CPU (sd, i)));
      if (cfile != NULL && cfile != sfile)
	{
	  /* If output from different cpus is going to the same file,
	     avoid closing the file twice.  */
	  for (j = 0; j < i; ++j)
	    if (TRACE_FILE (CPU_TRACE_DATA (STATE_CPU (sd, j))) == cfile)
	      break;
	  if (i == j)
	    fclose (cfile);
	}
    }
}

void
trace_one_insn (SIM_DESC sd, sim_cpu *cpu, address_word pc,
		int line_p, const char *filename, int linenum,
		const char *phase_wo_colon, const char *fmt,
		...)
{
  va_list ap;
  char phase[SIZE_PHASE+2];

  strncpy (phase, phase_wo_colon, SIZE_PHASE);
  strcat (phase, ":");

  if (!line_p)
    {
      trace_printf (sd, cpu, "%-*s %s:%-*d 0x%.*lx ",
		    SIZE_PHASE+1, phase,
		    filename,
		    SIZE_LINE_NUMBER, linenum,
		    SIZE_PC, (long)pc);
      va_start (ap, fmt);
      trace_vprintf (sd, cpu, fmt, ap);
      va_end (ap);
      trace_printf (sd, cpu, "\n");
    }
  else
    {
      char buf[256];

      buf[0] = 0;
      if (STATE_TEXT_SECTION (CPU_STATE (cpu))
	  && pc >= STATE_TEXT_START (CPU_STATE (cpu))
	  && pc < STATE_TEXT_END (CPU_STATE (cpu)))
	{
	  const char *pc_filename = (const char *)0;
	  const char *pc_function = (const char *)0;
	  unsigned int pc_linenum = 0;

	  if (bfd_find_nearest_line (STATE_PROG_BFD (CPU_STATE (cpu)),
				     STATE_TEXT_SECTION (CPU_STATE (cpu)),
				     (struct symbol_cache_entry **) 0,
				     pc - STATE_TEXT_START (CPU_STATE (cpu)),
				     &pc_filename, &pc_function, &pc_linenum))
	    {
	      char *p = buf;
	      if (pc_linenum)
		{
		  sprintf (p, "#%-*d ", SIZE_LINE_NUMBER, pc_linenum);
		  p += strlen (p);
		}
	      else
		{
		  sprintf (p, "%-*s ", SIZE_LINE_NUMBER+1, "---");
		  p += SIZE_LINE_NUMBER+2;
		}

	      if (pc_function)
		{
		  sprintf (p, "%s ", pc_function);
		  p += strlen (p);
		}
	      else if (pc_filename)
		{
		  char *q = (char *) strrchr (pc_filename, '/');
		  sprintf (p, "%s ", (q) ? q+1 : pc_filename);
		  p += strlen (p);
		}

	      if (*p == ' ')
		*p = '\0';
	    }
	}

      trace_printf (sd, cpu, "%-*s 0x%.*x %-*.*s ",
		    SIZE_PHASE+1, phase,
		    SIZE_PC, (unsigned) pc,
		    SIZE_LOCATION, SIZE_LOCATION, buf);
      va_start (ap, fmt);
      trace_vprintf (sd, cpu, fmt, ap);
      va_end (ap);
      trace_printf (sd, cpu, "\n");
    }
}

void
trace_vprintf (SIM_DESC sd, sim_cpu *cpu, const char *fmt, va_list ap)
{
  if (cpu != NULL)
    {
      if (TRACE_FILE (CPU_TRACE_DATA (cpu)) != NULL)
	vfprintf (TRACE_FILE (CPU_TRACE_DATA (cpu)), fmt, ap);
      else
	sim_io_evprintf (sd, fmt, ap);
    }
  else
    {
      if (TRACE_FILE (STATE_TRACE_DATA (sd)) != NULL)
	vfprintf (TRACE_FILE (STATE_TRACE_DATA (sd)), fmt, ap);
      else
	sim_io_evprintf (sd, fmt, ap);
    }
}

void
trace_printf VPARAMS ((SIM_DESC sd, sim_cpu *cpu, const char *fmt, ...))
{
#if !defined __STDC__ && !defined ALMOST_STDC
  SIM_DESC sd;
  sim_cpu *cpu;
  const char *fmt;
#endif
  va_list ap;

  VA_START (ap, fmt);
#if !defined __STDC__ && !defined ALMOST_STDC
  sd = va_arg (ap, SIM_DESC);
  cpu = va_arg (ap, sim_cpu *);
  fmt = va_arg (ap, const char *);
#endif

  trace_vprintf (sd, cpu, fmt, ap);

  va_end (ap);
}

void
debug_printf VPARAMS ((sim_cpu *cpu, const char *fmt, ...))
{
#if !defined __STDC__ && !defined ALMOST_STDC
  sim_cpu *cpu;
  const char *fmt;
#endif
  va_list ap;

  VA_START (ap, fmt);
#if !defined __STDC__ && !defined ALMOST_STDC
  cpu = va_arg (ap, sim_cpu *);
  fmt = va_arg (ap, const char *);
#endif

  if (CPU_DEBUG_FILE (cpu) == NULL)
    (* STATE_CALLBACK (CPU_STATE (cpu))->evprintf_filtered)
      (STATE_CALLBACK (CPU_STATE (cpu)), fmt, ap);
  else
    vfprintf (CPU_DEBUG_FILE (cpu), fmt, ap);

  va_end (ap);
}
