/* Tracing support for CGEN-based simulators.
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

/* Text is queued in TRACE_BUF because we want to output the insn's cycle
   count first but that isn't known until after the insn has executed.  */
static char trace_buf[1024];
/* If NULL, output to stdout directly.  */
static char *bufptr;

/* For computing an instruction's cycle count.
   FIXME: Need to move into cpu struct for smp case.  */
static unsigned long last_cycle_count;

void
trace_insn_init (SIM_CPU *cpu)
{
  bufptr = trace_buf;
  *bufptr = 0;
}

void
trace_insn_fini (SIM_CPU *cpu)
{
  if (CPU_PROFILE_FLAGS (cpu) [PROFILE_MODEL_IDX])
    {
      unsigned long total = PROFILE_TOTAL_CYCLE_COUNT (CPU_PROFILE_DATA (cpu));
      trace_printf (CPU_STATE (cpu), cpu, "%-*ld %-*ld ",
		    SIZE_CYCLE_COUNT, total - last_cycle_count,
		    SIZE_TOTAL_CYCLE_COUNT, total);
      last_cycle_count = total;
    }

  trace_printf (CPU_STATE (cpu), cpu, "%s\n", trace_buf);
}

/* For communication between trace_insn and trace_result.  */
static int printed_result_p;

void
trace_insn (SIM_CPU *cpu, const struct cgen_insn *opcode,
	    const struct argbuf *abuf, PCADDR pc)
{
  const char *filename;
  const char *functionname;
  unsigned int linenumber;
  char *p, buf[256], disasm_buf[50];

  if (! TRACE_P (cpu, TRACE_LINENUM_IDX))
    {
      cgen_trace_printf (cpu, "0x%.*x %-*s ",
			 SIZE_PC, (unsigned) pc,
			 SIZE_INSTRUCTION,
			 CGEN_INSN_MNEMONIC (opcode));
      return;
    }

  buf[0] = 0;
  
  if (STATE_TEXT_SECTION (CPU_STATE (cpu))
      && pc >= STATE_TEXT_START (CPU_STATE (cpu))
      && pc < STATE_TEXT_END (CPU_STATE (cpu)))
    {
      filename = (const char *) 0;
      functionname = (const char *) 0;
      linenumber = 0;
      if (bfd_find_nearest_line (STATE_PROG_BFD (CPU_STATE (cpu)),
				 STATE_TEXT_SECTION (CPU_STATE (cpu)),
				 (struct symbol_cache_entry **) 0,
				 pc - STATE_TEXT_START (CPU_STATE (cpu)),
				 &filename, &functionname, &linenumber))
	{
	  p = buf;
	  if (linenumber)
	    {
	      sprintf (p, "#%-*d ", SIZE_LINE_NUMBER, linenumber);
	      p += strlen (p);
	    }
	  else
	    {
	      sprintf (p, "%-*s ", SIZE_LINE_NUMBER+1, "---");
	      p += SIZE_LINE_NUMBER+2;
	    }

	  if (functionname)
	    {
	      sprintf (p, "%s ", functionname);
	      p += strlen (p);
	    }
	  else if (filename)
	    {
	      char *q = (char *) strrchr (filename, '/');
	      sprintf (p, "%s ", (q) ? q+1 : filename);
	      p += strlen (p);
	    }

	  if (*p == ' ')
	    *p = '\0';
	}
    }

  sim_disassemble_insn (cpu, opcode, abuf, pc, disasm_buf);

  cgen_trace_printf (cpu, "0x%.*x %-*.*s %-*s ",
		     SIZE_PC, (unsigned) pc,
		     SIZE_LOCATION, SIZE_LOCATION, buf,
		     SIZE_INSTRUCTION,
#if 0
		     CGEN_INSN_SYNTAX (opcode)->mnemonic
#else
		     disasm_buf
#endif
		);

  printed_result_p = 0;
}

void
trace_extract (SIM_CPU *cpu, PCADDR pc, char *name, ...)
{
  va_list args;
  int printed_one_p = 0;
  char *fmt;

  va_start (args, name);

  trace_printf (CPU_STATE (cpu), cpu, "Extract: 0x%.*x: %s ", SIZE_PC, pc, name);

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
    }

  va_end (args);
}
