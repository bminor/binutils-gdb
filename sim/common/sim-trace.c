/* Simulator tracing/debugging support.
   Copyright (C) 1997, 1998 Free Software Foundation, Inc.
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
#include "sim-fpu.h"
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
  /* This table is organized to group related instructions together.  */
  { {"trace", optional_argument, NULL, 't'},
      't', "on|off", "Trace useful things",
      trace_option_handler },
  { {"trace-insn", optional_argument, NULL, OPTION_TRACE_INSN},
      '\0', "on|off", "Perform instruction tracing",
      trace_option_handler },
  { {"trace-decode", optional_argument, NULL, OPTION_TRACE_DECODE},
      '\0', "on|off", "Trace instruction decoding",
      trace_option_handler },
  { {"trace-extract", optional_argument, NULL, OPTION_TRACE_EXTRACT},
      '\0', "on|off", "Trace instruction extraction",
      trace_option_handler },
  { {"trace-linenum", optional_argument, NULL, OPTION_TRACE_LINENUM},
      '\0', "on|off", "Perform line number tracing (implies --trace-insn)",
      trace_option_handler },
  { {"trace-memory", optional_argument, NULL, OPTION_TRACE_MEMORY},
      '\0', "on|off", "Trace memory operations",
      trace_option_handler },
  { {"trace-alu", optional_argument, NULL, OPTION_TRACE_ALU},
      '\0', "on|off", "Trace ALU operations",
      trace_option_handler },
  { {"trace-fpu", optional_argument, NULL, OPTION_TRACE_FPU},
      '\0', "on|off", "Trace FPU operations",
      trace_option_handler },
  { {"trace-branch", optional_argument, NULL, OPTION_TRACE_BRANCH},
      '\0', "on|off", "Trace branching",
      trace_option_handler },
  { {"trace-semantics", optional_argument, NULL, OPTION_TRACE_SEMANTICS},
      '\0', "on|off", "Perform ALU, FPU, MEMORY, and BRANCH tracing",
      trace_option_handler },
  { {"trace-model", optional_argument, NULL, OPTION_TRACE_MODEL},
      '\0', "on|off", "Include model performance data",
      trace_option_handler },
  { {"trace-core", optional_argument, NULL, OPTION_TRACE_CORE},
      '\0', "on|off", "Trace core operations",
      trace_option_handler },
  { {"trace-events", optional_argument, NULL, OPTION_TRACE_EVENTS},
      '\0', "on|off", "Trace events",
      trace_option_handler },
  { {"trace-debug", optional_argument, NULL, OPTION_TRACE_DEBUG},
      '\0', "on|off", "Add information useful for debugging the simulator to the tracing output",
      trace_option_handler },
  { {"trace-file", required_argument, NULL, OPTION_TRACE_FILE},
      '\0', "FILE NAME", "Specify tracing output file",
      trace_option_handler },
  { {NULL, no_argument, NULL, 0}, '\0', NULL, NULL, NULL }
};

/* Set/reset the trace options indicated in MASK.  */

static SIM_RC
set_trace_option_mask (sd, name, mask, arg)
     SIM_DESC sd;
     const char *name;
     int mask;
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

  for (trace_nr = 0; trace_nr < MAX_TRACE_VALUES; ++trace_nr)
    {
      if ((mask & (1 << trace_nr)) == 0)
	continue;

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

  return SIM_RC_OK;
}

/* Set one trace option based on its IDX value.  */

static SIM_RC
set_trace_option (sd, name, idx, arg)
     SIM_DESC sd;
     const char *name;
     int idx;
     const char *arg;
{
  return set_trace_option_mask (sd, name, 1 << idx, arg);
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
	return set_trace_option_mask (sd, "trace", TRACE_USEFUL_MASK, arg);
      break;

    case OPTION_TRACE_INSN :
      if (WITH_TRACE_INSN_P)
	return set_trace_option (sd, "-insn", TRACE_INSN_IDX, arg);
      else
	sim_io_eprintf (sd, "Instruction tracing not compiled in, `--trace-insn' ignored\n");
      break;

    case OPTION_TRACE_DECODE :
      if (WITH_TRACE_DECODE_P)
	return set_trace_option (sd, "-decode", TRACE_DECODE_IDX, arg);
      else
	sim_io_eprintf (sd, "Decode tracing not compiled in, `--trace-decode' ignored\n");
      break;

    case OPTION_TRACE_EXTRACT :
      if (WITH_TRACE_EXTRACT_P)
	return set_trace_option (sd, "-extract", TRACE_EXTRACT_IDX, arg);
      else
	sim_io_eprintf (sd, "Extract tracing not compiled in, `--trace-extract' ignored\n");
      break;

    case OPTION_TRACE_LINENUM :
      if (WITH_TRACE_LINENUM_P && WITH_TRACE_INSN_P)
	{
	  if (set_trace_option (sd, "-linenum", TRACE_LINENUM_IDX, arg) != SIM_RC_OK
	      || set_trace_option (sd, "-linenum", TRACE_INSN_IDX, arg) != SIM_RC_OK)
	    return SIM_RC_FAIL;
	}
      else
	sim_io_eprintf (sd, "Line number or instruction tracing not compiled in, `--trace-linenum' ignored\n");
      break;

    case OPTION_TRACE_MEMORY :
      if (WITH_TRACE_MEMORY_P)
	return set_trace_option (sd, "-memory", TRACE_MEMORY_IDX, arg);
      else
	sim_io_eprintf (sd, "Memory tracing not compiled in, `--trace-memory' ignored\n");
      break;

    case OPTION_TRACE_MODEL :
      if (WITH_TRACE_MODEL_P)
	return set_trace_option (sd, "-model", TRACE_MODEL_IDX, arg);
      else
	sim_io_eprintf (sd, "Model tracing not compiled in, `--trace-model' ignored\n");
      break;

    case OPTION_TRACE_ALU :
      if (WITH_TRACE_ALU_P)
	return set_trace_option (sd, "-alu", TRACE_ALU_IDX, arg);
      else
	sim_io_eprintf (sd, "ALU tracing not compiled in, `--trace-alu' ignored\n");
      break;

    case OPTION_TRACE_CORE :
      if (WITH_TRACE_CORE_P)
	return set_trace_option (sd, "-core", TRACE_CORE_IDX, arg);
      else
	sim_io_eprintf (sd, "CORE tracing not compiled in, `--trace-core' ignored\n");
      break;

    case OPTION_TRACE_EVENTS :
      if (WITH_TRACE_EVENTS_P)
	return set_trace_option (sd, "-events", TRACE_EVENTS_IDX, arg);
      else
	sim_io_eprintf (sd, "EVENTS tracing not compiled in, `--trace-events' ignored\n");
      break;

    case OPTION_TRACE_FPU :
      if (WITH_TRACE_FPU_P)
	return set_trace_option (sd, "-fpu", TRACE_FPU_IDX, arg);
      else
	sim_io_eprintf (sd, "FPU tracing not compiled in, `--trace-fpu' ignored\n");
      break;

    case OPTION_TRACE_BRANCH :
      if (WITH_TRACE_BRANCH_P)
	return set_trace_option (sd, "-branch", TRACE_BRANCH_IDX, arg);
      else
	sim_io_eprintf (sd, "Branch tracing not compiled in, `--trace-branch' ignored\n");
      break;

    case OPTION_TRACE_SEMANTICS :
      if (WITH_TRACE_ALU_P
	  && WITH_TRACE_FPU_P
	  && WITH_TRACE_MEMORY_P
	  && WITH_TRACE_BRANCH_P)
	{
	  if (set_trace_option (sd, "-semantics", TRACE_ALU_IDX, arg) != SIM_RC_OK
	      || set_trace_option (sd, "-semantics", TRACE_FPU_IDX, arg) != SIM_RC_OK
	      || set_trace_option (sd, "-semantics", TRACE_MEMORY_IDX, arg) != SIM_RC_OK
	      || set_trace_option (sd, "-semantics", TRACE_BRANCH_IDX, arg) != SIM_RC_OK)
	    return SIM_RC_FAIL;
	}
      else
	sim_io_eprintf (sd, "Alu, fpu, memory, and/or branch tracing not compiled in, `--trace-semantics' ignored\n");
      break;

    case OPTION_TRACE_DEBUG :
      if (WITH_TRACE_DEBUG_P)
	return set_trace_option (sd, "-debug", TRACE_DEBUG_IDX, arg);
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

typedef enum {
  trace_fmt_invalid,
  trace_fmt_word,
  trace_fmt_fp,
  trace_fmt_fpu,
  trace_fmt_string,
  trace_fmt_instruction_incomplete,
} data_fmt;

/* compute the nr of trace data units consumed by data */
static int
save_data_size (TRACE_DATA *data,
		long size)
{
  return ((size + sizeof (TRACE_INPUT_DATA (data) [0]) - 1)
	  / sizeof (TRACE_INPUT_DATA (data) [0]));
}


/* Archive DATA into the trace buffer */
static void
save_data (SIM_DESC sd,
	   TRACE_DATA *data,
	   data_fmt fmt,
	   long size,
	   void *buf)
{
  int i = TRACE_INPUT_IDX (data);
  if (i == sizeof (TRACE_INPUT_FMT (data)))
    sim_io_error (sd, "trace buffer overflow");
  TRACE_INPUT_FMT (data) [i] = fmt;
  TRACE_INPUT_SIZE (data) [i] = size;
  memcpy (&TRACE_INPUT_DATA (data) [i], buf, size);
  i += save_data_size (data, size);
  TRACE_INPUT_IDX (data) = i;
}

static void
print_data (SIM_DESC sd,
	    sim_cpu *cpu,
	    data_fmt fmt,
	    long size,
	    void *data)
{
  switch (fmt)
    {
    case trace_fmt_instruction_incomplete:
      trace_printf (sd, cpu, " (instruction incomplete)");
      break;
    case trace_fmt_word:
      switch (size)
	{
	case sizeof (unsigned_word):
	  trace_printf (sd, cpu, " 0x%08lx", (long) * (unsigned_word*) data);
	  break;
	default:
	  abort ();
	}
      break;
    case trace_fmt_fp:
      switch (size)
	{
	  /* FIXME: Assumes sizeof float == 4; sizeof double == 8 */
	case 4:
	  trace_printf (sd, cpu, " %8g", * (float*) data);
	  break;
	case 8:
	  trace_printf (sd, cpu, " %8g", * (double*) data);
	  break;
	default:
	  abort ();
	}
    case trace_fmt_fpu:
      /* FIXME: At present sim_fpu data is stored as a double */
      trace_printf (sd, cpu, " %8g", * (double*) data);
      break;
    case trace_fmt_string:
      trace_printf (sd, cpu, " %-8s", (char*) data);
      break;
    default:
      abort ();
    }
}
		  
static const char *
trace_idx_to_str (int trace_idx)
{
  static char num[8];
  switch (trace_idx)
    {
    case TRACE_ALU_IDX:     return "alu:     ";
    case TRACE_INSN_IDX:    return "insn:    ";
    case TRACE_DECODE_IDX:  return "decode:  ";
    case TRACE_EXTRACT_IDX: return "extract: ";
    case TRACE_MEMORY_IDX:  return "memory:  ";
    case TRACE_CORE_IDX:    return "core:    ";
    case TRACE_EVENTS_IDX:  return "events:  ";
    case TRACE_FPU_IDX:     return "fpu:     ";
    case TRACE_BRANCH_IDX:  return "branch:  ";
    default:
      sprintf (num, "?%d?", trace_idx);
      return num;
    }
}

static void
trace_results (SIM_DESC sd,
	       sim_cpu *cpu,
	       int trace_idx,
	       int last_input)
{
  TRACE_DATA *data = CPU_TRACE_DATA (cpu);
  int nr_out;
  int i;

  /* cross check trace_idx against TRACE_IDX (data)? */

  /* prefix */
  trace_printf (sd, cpu, "%s %s",
		trace_idx_to_str (TRACE_IDX (data)),
		TRACE_PREFIX (data));
  TRACE_IDX (data) = 0;

  for (i = 0, nr_out = 0;
       i < TRACE_INPUT_IDX (data);
       i += save_data_size (data, TRACE_INPUT_SIZE (data) [i]), nr_out++)
    {
      if (i == last_input)
	{
	  int pad = (strlen (" 0x") + sizeof (unsigned_word) * 2);
	  int padding = pad * (3 - nr_out);
	  if (padding < 0)
	    padding = 0;
	  padding += strlen (" ::");
	  trace_printf (sd, cpu, "%*s", padding, " ::");
	}
      print_data (sd, cpu,
		  TRACE_INPUT_FMT (data) [i],
		  TRACE_INPUT_SIZE (data) [i],
		  &TRACE_INPUT_DATA (data) [i]);
    }
  trace_printf (sd, cpu, "\n");
}

void
trace_prefix (SIM_DESC sd,
	      sim_cpu *cpu,
	      address_word pc,
	      int line_p,
	      const char *filename,
	      int linenum,
	      const char *fmt,
	      ...)
{
  TRACE_DATA *data = CPU_TRACE_DATA (cpu);
  va_list ap;
  char *prefix = TRACE_PREFIX (data);
  char *chp;

  /* if the previous trace data wasn't flushed, flush it now with a
     note indicating that this occured. */
  if (TRACE_IDX (data) != 0)
    {
      int last_input = TRACE_INPUT_IDX (data);
      save_data (sd, data, trace_fmt_instruction_incomplete, 1, "");
      trace_results (sd, cpu, TRACE_IDX (data), last_input);
    }
  TRACE_IDX (data) = 0;
  TRACE_INPUT_IDX (data) = 0;

  if (!line_p)
    {
      sprintf (prefix, "%s:%-*d 0x%.*lx ",
	       filename,
	       SIZE_LINE_NUMBER, linenum,
	       SIZE_PC, (long)pc);
      chp = strchr (prefix, '\0');
      va_start (ap, fmt);
      vsprintf (chp, fmt, ap);
      va_end (ap);
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

      sprintf (prefix, "0x%.*x %-*.*s ",
	       SIZE_PC, (unsigned) pc,
	       SIZE_LOCATION, SIZE_LOCATION, buf);
      chp = strchr (prefix, '\0');
      va_start (ap, fmt);
      vsprintf (chp, fmt, ap);
      va_end (ap);
    }

  /* pad it out to TRACE_PREFIX_WIDTH.  FIXME: The TRACE_PREFIX_WIDTH
     should be determined at build time using known information about
     the disassembled instructions */
#ifndef TRACE_PREFIX_WIDTH
#define TRACE_PREFIX_WIDTH 48
#endif
  chp = strchr (prefix, '\0');
  if (chp - prefix < TRACE_PREFIX_WIDTH)
    {
      memset (chp, ' ', TRACE_PREFIX_WIDTH - (chp - prefix));
      chp = &prefix [TRACE_PREFIX_WIDTH];
      *chp = '\0';
    }
  strcpy (chp, " -");

  /* check that we've not over flowed the prefix buffer */
  if (strlen (prefix) >= sizeof (TRACE_PREFIX (data)))
    abort ();
}

void
trace_generic (SIM_DESC sd,
	       sim_cpu *cpu,
	       int trace_idx,
	       char *fmt,
	       ...)
{
  va_list ap;
  trace_printf (sd, cpu, "%s %s",
		trace_idx_to_str (trace_idx),
		TRACE_PREFIX (CPU_TRACE_DATA (cpu)));
  va_start (ap, fmt);
  trace_vprintf (sd, cpu, fmt, ap);
  va_end (ap);
  trace_printf (sd, cpu, "\n");
}

void
trace_input0 (SIM_DESC sd,
	      sim_cpu *cpu,
	      int trace_idx)
{
  TRACE_DATA *data = CPU_TRACE_DATA (cpu);
  TRACE_IDX (data) = trace_idx;
}

void
trace_input_word1 (SIM_DESC sd,
		   sim_cpu *cpu,
		   int trace_idx,
		   unsigned_word d0)
{
  TRACE_DATA *data = CPU_TRACE_DATA (cpu);
  TRACE_IDX (data) = trace_idx;
  save_data (sd, data, trace_fmt_word, sizeof (unsigned_word), &d0);
}

void
trace_input_word2 (SIM_DESC sd,
		   sim_cpu *cpu,
		   int trace_idx,
		   unsigned_word d0,
		   unsigned_word d1)
{
  TRACE_DATA *data = CPU_TRACE_DATA (cpu);
  TRACE_IDX (data) = trace_idx;
  save_data (sd, data, trace_fmt_word, sizeof (unsigned_word), &d0);
  save_data (sd, data, trace_fmt_word, sizeof (unsigned_word), &d1);
}

void
trace_input_word3 (SIM_DESC sd,
		   sim_cpu *cpu,
		   int trace_idx,
		   unsigned_word d0,
		   unsigned_word d1,
		   unsigned_word d2)
{
  TRACE_DATA *data = CPU_TRACE_DATA (cpu);
  TRACE_IDX (data) = trace_idx;
  save_data (sd, data, trace_fmt_word, sizeof (unsigned_word), &d0);
  save_data (sd, data, trace_fmt_word, sizeof (unsigned_word), &d1);
  save_data (sd, data, trace_fmt_word, sizeof (unsigned_word), &d2);
}

void
trace_input_fp1 (SIM_DESC sd,
		 sim_cpu *cpu,
		 int trace_idx,
		 fp_word f0)
{
  TRACE_DATA *data = CPU_TRACE_DATA (cpu);
  TRACE_IDX (data) = trace_idx;
  save_data (sd, data, trace_fmt_fp, sizeof (fp_word), &f0);
}

void
trace_input_fp2 (SIM_DESC sd,
		 sim_cpu *cpu,
		 int trace_idx,
		 fp_word f0,
		 fp_word f1)
{
  TRACE_DATA *data = CPU_TRACE_DATA (cpu);
  TRACE_IDX (data) = trace_idx;
  save_data (sd, data, trace_fmt_fp, sizeof (fp_word), &f0);
  save_data (sd, data, trace_fmt_fp, sizeof (fp_word), &f1);
}

void
trace_input_fp3 (SIM_DESC sd,
		 sim_cpu *cpu,
		 int trace_idx,
		 fp_word f0,
		 fp_word f1,
		 fp_word f2)
{
  TRACE_DATA *data = CPU_TRACE_DATA (cpu);
  TRACE_IDX (data) = trace_idx;
  save_data (sd, data, trace_fmt_fp, sizeof (fp_word), &f0);
  save_data (sd, data, trace_fmt_fp, sizeof (fp_word), &f1);
  save_data (sd, data, trace_fmt_fp, sizeof (fp_word), &f2);
}

void
trace_input_fpu1 (SIM_DESC sd,
		  sim_cpu *cpu,
		  int trace_idx,
		  sim_fpu *f0)
{
  double d;
  TRACE_DATA *data = CPU_TRACE_DATA (cpu);
  TRACE_IDX (data) = trace_idx;
  d = sim_fpu_2d (f0);
  save_data (sd, data, trace_fmt_fp, sizeof (double), &d);
}

void
trace_input_fpu2 (SIM_DESC sd,
		  sim_cpu *cpu,
		  int trace_idx,
		  sim_fpu *f0,
		  sim_fpu *f1)
{
  double d;
  TRACE_DATA *data = CPU_TRACE_DATA (cpu);
  TRACE_IDX (data) = trace_idx;
  d = sim_fpu_2d (f0);
  save_data (sd, data, trace_fmt_fp, sizeof (double), &d);
  d = sim_fpu_2d (f1);
  save_data (sd, data, trace_fmt_fp, sizeof (double), &d);
}

void
trace_input_fpu3 (SIM_DESC sd,
		  sim_cpu *cpu,
		  int trace_idx,
		  sim_fpu *f0,
		  sim_fpu *f1,
		  sim_fpu *f2)
{
  double d;
  TRACE_DATA *data = CPU_TRACE_DATA (cpu);
  TRACE_IDX (data) = trace_idx;
  d = sim_fpu_2d (f0);
  save_data (sd, data, trace_fmt_fp, sizeof (double), &d);
  d = sim_fpu_2d (f1);
  save_data (sd, data, trace_fmt_fp, sizeof (double), &d);
  d = sim_fpu_2d (f2);
  save_data (sd, data, trace_fmt_fp, sizeof (double), &d);
}

void
trace_result_word1 (SIM_DESC sd,
		    sim_cpu *cpu,
		    int trace_idx,
		    unsigned_word r0)
{
  TRACE_DATA *data = CPU_TRACE_DATA (cpu);
  int last_input;

  /* Append any results to the end of the inputs */
  last_input = TRACE_INPUT_IDX (data);
  save_data (sd, data, trace_fmt_word, sizeof (unsigned_word), &r0);

  trace_results (sd, cpu, trace_idx, last_input);
}	      

void
trace_result_fp1 (SIM_DESC sd,
		  sim_cpu *cpu,
		  int trace_idx,
		  fp_word f0)
{
  TRACE_DATA *data = CPU_TRACE_DATA (cpu);
  int last_input;

  /* Append any results to the end of the inputs */
  last_input = TRACE_INPUT_IDX (data);
  save_data (sd, data, trace_fmt_fp, sizeof (fp_word), &f0);

  trace_results (sd, cpu, trace_idx, last_input);
}	      

void
trace_result_fpu1 (SIM_DESC sd,
		   sim_cpu *cpu,
		   int trace_idx,
		   sim_fpu *f0)
{
  double d;
  TRACE_DATA *data = CPU_TRACE_DATA (cpu);
  int last_input;

  /* Append any results to the end of the inputs */
  last_input = TRACE_INPUT_IDX (data);
  d = sim_fpu_2d (f0);
  save_data (sd, data, trace_fmt_fp, sizeof (double), &d);

  trace_results (sd, cpu, trace_idx, last_input);
}	      

void
trace_result_string1 (SIM_DESC sd,
		      sim_cpu *cpu,
		      int trace_idx,
		      char *s0)
{
  TRACE_DATA *data = CPU_TRACE_DATA (cpu);
  int last_input;

  /* Append any results to the end of the inputs */
  last_input = TRACE_INPUT_IDX (data);
  save_data (sd, data, trace_fmt_string, strlen (s0) + 1, s0);

  trace_results (sd, cpu, trace_idx, last_input);
}	      

void
trace_result_word1_string1 (SIM_DESC sd,
			    sim_cpu *cpu,
			    int trace_idx,
			    unsigned_word r0,
			    char *s0)
{
  TRACE_DATA *data = CPU_TRACE_DATA (cpu);
  int last_input;

  /* Append any results to the end of the inputs */
  last_input = TRACE_INPUT_IDX (data);
  save_data (sd, data, trace_fmt_word, sizeof (unsigned_word), &r0);
  save_data (sd, data, trace_fmt_string, strlen (s0) + 1, s0);

  trace_results (sd, cpu, trace_idx, last_input);
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
