/* Tracing support for CGEN-based simulators.
   Copyright (C) 1996, 1997, 1998 Free Software Foundation, Inc.
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
#include "bfd.h"
#include "cpu-opc.h"

#ifndef SIZE_INSTRUCTION
#define SIZE_INSTRUCTION 16
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

#ifndef SIZE_CYCLE_COUNT
#define SIZE_CYCLE_COUNT 2
#endif

#ifndef SIZE_TOTAL_CYCLE_COUNT
#define SIZE_TOTAL_CYCLE_COUNT 9
#endif

#ifndef SIZE_TRACE_BUF
#define SIZE_TRACE_BUF 256
#endif

/* Text is queued in TRACE_BUF because we want to output the insn's cycle
   count first but that isn't known until after the insn has executed.
   This also handles the queueing of trace results, TRACE_RESULT may be
   called multiple times for one insn.  */
static char trace_buf[SIZE_TRACE_BUF];
/* If NULL, output to stdout directly.  */
static char *bufptr;

/* Non-zero if this is the first insn in a set of parallel insns.  */
static int first_insn_p;

/* For communication between trace_insn and trace_result.  */
static int printed_result_p;

/* Insn and its extracted fields.
   Set by trace_insn, used by trace_insn_fini.
   ??? Move to SIM_CPU to support heterogeneous multi-cpu case.  */
static const struct cgen_insn *current_insn;
static CGEN_FIELDS insn_fields;
static const struct argbuf *current_abuf;

void
trace_insn_init (SIM_CPU *cpu, int first_p)
{
  bufptr = trace_buf;
  *bufptr = 0;
  first_insn_p = first_p;

  /* Set to NULL so trace_insn_fini can know if trace_insn was called.  */
  current_insn = NULL;
  current_abuf = NULL;
}

void
trace_insn_fini (SIM_CPU *cpu, const struct argbuf *abuf, int last_p)
{
  SIM_DESC sd = CPU_STATE (cpu);

  /* Was insn traced?  It might not be if trace ranges are in effect.  */
  if (current_insn == NULL)
    return;

  /* The first thing printed is current and total cycle counts.  */

  if (PROFILE_MODEL_P (cpu)
      && ARGBUF_PROFILE_P (current_abuf))
    {
      unsigned long total = PROFILE_MODEL_TOTAL_CYCLES (CPU_PROFILE_DATA (cpu));
      unsigned long this_insn = PROFILE_MODEL_CUR_INSN_CYCLES (CPU_PROFILE_DATA (cpu));

      if (last_p)
	{
	  trace_printf (sd, cpu, "%-*ld %-*ld ",
			SIZE_CYCLE_COUNT, this_insn,
			SIZE_TOTAL_CYCLE_COUNT, total);
	}
      else
	{
	  trace_printf (sd, cpu, "%-*ld %-*s ",
			SIZE_CYCLE_COUNT, this_insn,
			SIZE_TOTAL_CYCLE_COUNT, "---");
	}
    }

  /* Print the disassembled insn.  */

  trace_printf (sd, cpu, "%s", TRACE_PREFIX (CPU_TRACE_DATA (cpu)));

#if 0
  /* Print insn results.  */
  {
    const CGEN_OPERAND_INSTANCE *opinst = CGEN_INSN_OPERANDS (current_insn);

    if (opinst)
      {
	int i;
	int indices[MAX_OPERAND_INSTANCES];

	/* Fetch the operands used by the insn.  */
	/* FIXME: Add fn ptr to CGEN_OPCODE_DESC.  */
	CGEN_SYM (get_insn_operands) (STATE_OPCODE_TABLE (sd), current_insn,
				      0, CGEN_FIELDS_BITSIZE (&insn_fields),
				      indices);

	for (i = 0;
	     CGEN_OPERAND_INSTANCE_TYPE (opinst) != CGEN_OPERAND_INSTANCE_END;
	     ++i, ++opinst)
	  {
	    if (CGEN_OPERAND_INSTANCE_TYPE (opinst) == CGEN_OPERAND_INSTANCE_OUTPUT)
	      trace_result (cpu, current_insn, opinst, indices[i]);
	  }
      }
  }
#endif

  /* Print anything else requested.  */

  if (*trace_buf)
    trace_printf (sd, cpu, " %s\n", trace_buf);
  else
    trace_printf (sd, cpu, "\n");
}

void
trace_insn (SIM_CPU *cpu, const struct cgen_insn *opcode,
	    const struct argbuf *abuf, PCADDR pc)
{
  char disasm_buf[50];

  printed_result_p = 0;
  current_insn = opcode;
  current_abuf = abuf;

  if (CGEN_INSN_VIRTUAL_P (opcode))
    {
      trace_prefix (CPU_STATE (cpu), cpu, NULL_CIA, pc, 0,
		    NULL, 0, CGEN_INSN_NAME (opcode));
      return;
    }

  sim_disassemble_insn (cpu, opcode, abuf, pc, disasm_buf, &insn_fields);
  trace_prefix (CPU_STATE (cpu), cpu, NULL_CIA, pc, TRACE_LINENUM_P (cpu),
		NULL, 0,
		"%s%-*s",
		first_insn_p ? " " : "|",
		SIZE_INSTRUCTION, disasm_buf);
}

void
trace_extract (SIM_CPU *cpu, PCADDR pc, char *name, ...)
{
  va_list args;
  int printed_one_p = 0;
  char *fmt;

  va_start (args, name);

  trace_printf (CPU_STATE (cpu), cpu, "Extract: 0x%.*lx: %s ",
		SIZE_PC, pc, name);

  do {
    int type,ival;

    fmt = va_arg (args, char *);

    if (fmt)
      {
	if (printed_one_p)
	  trace_printf (CPU_STATE (cpu), cpu, ", ");
	printed_one_p = 1;
	type = va_arg (args, int);
	switch (type)
	  {
	  case 'x' :
	    ival = va_arg (args, int);
	    trace_printf (CPU_STATE (cpu), cpu, fmt, ival);
	    break;
	  default :
	    abort ();
	  }
      }
  } while (fmt);

  va_end (args);
  trace_printf (CPU_STATE (cpu), cpu, "\n");
}

void
trace_result (SIM_CPU *cpu, char *name, int type, ...)
{
  va_list args;

  va_start (args, type);
  if (printed_result_p)
    cgen_trace_printf (cpu, ", ");

  switch (type)
    {
    case 'x' :
    default :
      cgen_trace_printf (cpu, "%s <- 0x%x", name, va_arg (args, int));
      break;
    case 'D' :
      {
	DI di;
	/* this is separated from previous line for sunos cc */
	di = va_arg (args, DI);
	cgen_trace_printf (cpu, "%s <- 0x%x%08x", name,
			   GETHIDI(di), GETLODI (di));
	break;
      }
    }

  printed_result_p = 1;
  va_end (args);
}

/* Print trace output to BUFPTR if active, otherwise print normally.
   This is only for tracing semantic code.  */

void
cgen_trace_printf (SIM_CPU *cpu, char *fmt, ...)
{
  va_list args;

  va_start (args, fmt);

  if (bufptr == NULL)
    {
      if (TRACE_FILE (CPU_TRACE_DATA (cpu)) == NULL)
	(* STATE_CALLBACK (CPU_STATE (cpu))->evprintf_filtered)
	  (STATE_CALLBACK (CPU_STATE (cpu)), fmt, args);
      else
	vfprintf (TRACE_FILE (CPU_TRACE_DATA (cpu)), fmt, args);
    }
  else
    {
      vsprintf (bufptr, fmt, args);
      bufptr += strlen (bufptr);
      /* ??? Need version of SIM_ASSERT that is always enabled.  */
      if (bufptr - trace_buf > SIZE_TRACE_BUF)
	abort ();
    }

  va_end (args);
}
