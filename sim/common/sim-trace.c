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
  OPTION_TRACE_FILE
};

static const OPTION trace_options[] =
{
  { {"trace", no_argument, NULL, 't'},
      't', NULL, "Perform tracing",
      trace_option_handler },
  { {"trace-insn", no_argument, NULL, OPTION_TRACE_INSN},
      '\0', NULL, "Perform instruction tracing",
      trace_option_handler },
  { {"trace-decode", no_argument, NULL, OPTION_TRACE_DECODE},
      '\0', NULL, "Perform instruction decoding tracing",
      trace_option_handler },
  { {"trace-extract", no_argument, NULL, OPTION_TRACE_EXTRACT},
      '\0', NULL, "Perform instruction extraction tracing",
      trace_option_handler },
  { {"trace-linenum", no_argument, NULL, OPTION_TRACE_LINENUM},
      '\0', NULL, "Perform line number tracing (implies --trace-insn)",
      trace_option_handler },
  { {"trace-memory", no_argument, NULL, OPTION_TRACE_MEMORY},
      '\0', NULL, "Perform memory tracing",
      trace_option_handler },
  { {"trace-model", no_argument, NULL, OPTION_TRACE_MODEL},
      '\0', NULL, "Perform model tracing",
      trace_option_handler },
  { {"trace-alu", no_argument, NULL, OPTION_TRACE_ALU},
      '\0', NULL, "Perform ALU tracing",
      trace_option_handler },
  { {"trace-core", no_argument, NULL, OPTION_TRACE_CORE},
      '\0', NULL, "Perform CORE tracing",
      trace_option_handler },
  { {"trace-events", no_argument, NULL, OPTION_TRACE_EVENTS},
      '\0', NULL, "Perform EVENTS tracing",
      trace_option_handler },
  { {"trace-fpu", no_argument, NULL, OPTION_TRACE_FPU},
      '\0', NULL, "Perform FPU tracing",
      trace_option_handler },
  { {"trace-branch", no_argument, NULL, OPTION_TRACE_BRANCH},
      '\0', NULL, "Perform branch tracing",
      trace_option_handler },
  { {"trace-semantics", no_argument, NULL, OPTION_TRACE_SEMANTICS},
      '\0', NULL, "Perform ALU, FPU, and MEMORY tracing",
      trace_option_handler },
  { {"trace-file", required_argument, NULL, OPTION_TRACE_FILE},
      '\0', "FILE NAME", "Specify tracing output file",
      trace_option_handler },
  { {NULL, no_argument, NULL, 0}, '\0', NULL, NULL, NULL }
};

static SIM_RC
trace_option_handler (sd, opt, arg)
     SIM_DESC sd;
     int opt;
     char *arg;
{
  int i,n;

  switch (opt)
    {
    case 't' :
      if (! WITH_TRACE)
	sim_io_eprintf (sd, "Tracing not compiled in, `-t' ignored\n");
      else
	{
	  for (n = 0; n < MAX_NR_PROCESSORS; ++n)
	    for (i = 0; i < MAX_TRACE_VALUES; ++i)
	      CPU_TRACE_FLAGS (STATE_CPU (sd, n))[i] = 1;
	  STATE_CORE(sd)->trace = 1;
	  STATE_EVENTS(sd)->trace = 1;
	}
      break;

    case OPTION_TRACE_INSN :
      if (WITH_TRACE_INSN_P)
	for (n = 0; n < MAX_NR_PROCESSORS; ++n)
	  CPU_TRACE_FLAGS (STATE_CPU (sd, n))[TRACE_INSN_IDX] = 1;
      else
	sim_io_eprintf (sd, "Instruction tracing not compiled in, `--trace-insn' ignored\n");
      break;

    case OPTION_TRACE_DECODE :
      if (WITH_TRACE_DECODE_P)
	for (n = 0; n < MAX_NR_PROCESSORS; ++n)
	  CPU_TRACE_FLAGS (STATE_CPU (sd, n))[TRACE_DECODE_IDX] = 1;
      else
	sim_io_eprintf (sd, "Decode tracing not compiled in, `--trace-decode' ignored\n");
      break;

    case OPTION_TRACE_EXTRACT :
      if (WITH_TRACE_EXTRACT_P)
	for (n = 0; n < MAX_NR_PROCESSORS; ++n)
	  CPU_TRACE_FLAGS (STATE_CPU (sd, n))[TRACE_EXTRACT_IDX] = 1;
      else
	sim_io_eprintf (sd, "Extract tracing not compiled in, `--trace-extract' ignored\n");
      break;

    case OPTION_TRACE_LINENUM :
      if (WITH_TRACE_LINENUM_P && WITH_TRACE_INSN_P)
	for (n = 0; n < MAX_NR_PROCESSORS; ++n)
	  {
	    CPU_TRACE_FLAGS (STATE_CPU (sd, n))[TRACE_LINENUM_IDX] = 1;
	    CPU_TRACE_FLAGS (STATE_CPU (sd, n))[TRACE_INSN_IDX] = 1;
	  }
      else
	sim_io_eprintf (sd, "Line number or instruction tracing not compiled in, `--trace-linenum' ignored\n");
      break;

    case OPTION_TRACE_MEMORY :
      if (WITH_TRACE_MEMORY_P)
	for (n = 0; n < MAX_NR_PROCESSORS; ++n)
	  CPU_TRACE_FLAGS (STATE_CPU (sd, n))[TRACE_MEMORY_IDX] = 1;
      else
	sim_io_eprintf (sd, "Memory tracing not compiled in, `--trace-memory' ignored\n");
      break;

    case OPTION_TRACE_MODEL :
      if (WITH_TRACE_MODEL_P)
	for (n = 0; n < MAX_NR_PROCESSORS; ++n)
	  CPU_TRACE_FLAGS (STATE_CPU (sd, n))[TRACE_MODEL_IDX] = 1;
      else
	sim_io_eprintf (sd, "Model tracing not compiled in, `--trace-model' ignored\n");
      break;

    case OPTION_TRACE_ALU :
      if (WITH_TRACE_ALU_P)
	for (n = 0; n < MAX_NR_PROCESSORS; ++n)
	  CPU_TRACE_FLAGS (STATE_CPU (sd, n))[TRACE_ALU_IDX] = 1;
      else
	sim_io_eprintf (sd, "ALU tracing not compiled in, `--trace-alu' ignored\n");
      break;

    case OPTION_TRACE_CORE :
      if (WITH_TRACE_CORE_P)
	{
	  for (n = 0; n < MAX_NR_PROCESSORS; ++n)
	    CPU_TRACE_FLAGS (STATE_CPU (sd, n))[TRACE_CORE_IDX] = 1;
	  STATE_CORE(sd)->trace = 1;
	}
      else
	sim_io_eprintf (sd, "CORE tracing not compiled in, `--trace-core' ignored\n");
      break;

    case OPTION_TRACE_EVENTS :
      if (WITH_TRACE_EVENTS_P)
	{
	  for (n = 0; n < MAX_NR_PROCESSORS; ++n)
	    CPU_TRACE_FLAGS (STATE_CPU (sd, n))[TRACE_EVENTS_IDX] = 1;
	  STATE_EVENTS(sd)->trace = 1;
	}
      else
	sim_io_eprintf (sd, "EVENTS tracing not compiled in, `--trace-events' ignored\n");
      break;

    case OPTION_TRACE_FPU :
      if (WITH_TRACE_FPU_P)
	for (n = 0; n < MAX_NR_PROCESSORS; ++n)
	  CPU_TRACE_FLAGS (STATE_CPU (sd, n))[TRACE_FPU_IDX] = 1;
      else
	sim_io_eprintf (sd, "FPU tracing not compiled in, `--trace-fpu' ignored\n");
      break;

    case OPTION_TRACE_BRANCH :
      if (WITH_TRACE_BRANCH_P)
	for (n = 0; n < MAX_NR_PROCESSORS; ++n)
	  CPU_TRACE_FLAGS (STATE_CPU (sd, n))[TRACE_FPU_IDX] = 1;
      else
	sim_io_eprintf (sd, "Branch tracing not compiled in, `--trace-branch' ignored\n");
      break;

    case OPTION_TRACE_SEMANTICS :
      if (WITH_TRACE_ALU_P && WITH_TRACE_FPU_P && WITH_TRACE_MEMORY_P)
	for (n = 0; n < MAX_NR_PROCESSORS; ++n)
	  {
	    CPU_TRACE_FLAGS (STATE_CPU (sd, n))[TRACE_ALU_IDX] = 1;
	    CPU_TRACE_FLAGS (STATE_CPU (sd, n))[TRACE_FPU_IDX] = 1;
	    CPU_TRACE_FLAGS (STATE_CPU (sd, n))[TRACE_MEMORY_IDX] = 1;
	    CPU_TRACE_FLAGS (STATE_CPU (sd, n))[TRACE_BRANCH_IDX] = 1;
	  }
      else
	sim_io_eprintf (sd, "Alu, fpu, and/or memory tracing not compiled in, `--trace-semantics' ignored\n");
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

  sim_add_option_table (sd, trace_options);
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

  for (i = 0; i < MAX_NR_PROCESSORS; ++i)
    {
      TRACE_DATA *data = CPU_TRACE_DATA (STATE_CPU (sd, i));
      if (TRACE_FILE (data) != NULL)
	{
	  /* If output from different cpus is going to the same file,
	     avoid closing the file twice.  */
	  for (j = 0; j < i; ++j)
	    if (TRACE_FILE (CPU_TRACE_DATA (STATE_CPU (sd, j)))
		== TRACE_FILE (data))
	      break;
	  if (i == j)
	    fclose (TRACE_FILE (data));
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
  if (cpu != NULL && TRACE_FILE (CPU_TRACE_DATA (cpu)) != NULL)
    vfprintf (TRACE_FILE (CPU_TRACE_DATA (cpu)), fmt, ap);
  else
    sim_io_evprintf (sd, fmt, ap);
}

void
trace_printf VPARAMS ((SIM_DESC sd, sim_cpu *cpu, const char *fmt, ...))
{
#ifndef __STDC__
  SIM_DESC sd;
  sim_cpu *cpu;
  const char *fmt;
#endif
  va_list ap;

  VA_START (ap, fmt);
#ifndef __STDC__
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
#ifndef __STDC__
  sim_cpu *cpu;
  const char *fmt;
#endif
  va_list ap;

  VA_START (ap, fmt);
#ifndef __STDC__
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
