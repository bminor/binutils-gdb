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

/* This file is meant to be included by sim-basics.h.  */

#ifndef SIM_TRACE_H
#define SIM_TRACE_H

#ifndef __attribute__
#if !defined(__GNUC__) || (__GNUC__ < 2) || (__GNUC__ == 2 && __GNU_MINOR__ < 7)
#define __attribute__(attr)
#endif
#endif

/* Maximum number of traceable entities.  */
#ifndef MAX_TRACE_VALUES
#define MAX_TRACE_VALUES 12
#endif

/* Standard traceable entities.  */
#define TRACE_INSN_IDX 0
#define TRACE_DECODE_IDX 1
#define TRACE_EXTRACT_IDX 2
#define TRACE_LINENUM_IDX 3
#define TRACE_MEMORY_IDX 4
#define TRACE_MODEL_IDX 5
#define TRACE_ALU_IDX 6
#define TRACE_CORE_IDX 7
#define TRACE_EVENTS_IDX 8
#define TRACE_FPU_IDX 9
#define TRACE_NEXT_IDX 16 /* simulator specific trace bits begin here */

/* Masks so WITH_TRACE can have symbolic values.  */
#define TRACE_insn 1
#define TRACE_decode 2
#define TRACE_extract 4
#define TRACE_linenum 8
#define TRACE_memory 16
#define TRACE_model 32
#define TRACE_alu 64
#define TRACE_core 128
#define TRACE_events 256
#define TRACE_fpu 512

/* Preprocessor macros to simplify tests of WITH_TRACE.  */
#define WITH_TRACE_INSN_P (WITH_TRACE & TRACE_insn)
#define WITH_TRACE_DECODE_P (WITH_TRACE & TRACE_decode)
#define WITH_TRACE_EXTRACT_P (WITH_TRACE & TRACE_extract)
#define WITH_TRACE_LINENUM_P (WITH_TRACE & TRACE_linenum)
#define WITH_TRACE_MEMORY_P (WITH_TRACE & TRACE_memory)
#define WITH_TRACE_MODEL_P (WITH_TRACE & TRACE_model)
#define WITH_TRACE_ALU_P (WITH_TRACE & TRACE_alu)
#define WITH_TRACE_CORE_P (WITH_TRACE & TRACE_core)
#define WITH_TRACE_EVENTS_P (WITH_TRACE & TRACE_events)
#define WITH_TRACE_FPU_P (WITH_TRACE & TRACE_fpu)

/* Tracing install handler.  */
MODULE_INSTALL_FN trace_install;

/* Struct containing all trace data.  */

typedef struct {
  /* Boolean array of specified tracing flags.  */
  /* ??? It's not clear that using an array vs a bit mask is faster.
     Consider the case where one wants to test whether any of several bits
     are set.  */
  char trace_flags[MAX_TRACE_VALUES];
#define TRACE_FLAGS(t) ((t)->trace_flags)

  /* Tracing output goes to this or stderr if NULL.
     We can't store `stderr' here as stderr goes through a callback.  */
  FILE *trace_file;
#define TRACE_FILE(t) ((t)->trace_file)
} TRACE_DATA;

/* Usage macros.  */

#define CPU_TRACE_FLAGS(cpu) TRACE_FLAGS (CPU_TRACE_DATA (cpu))

/* forward reference */
struct _sim_cpu;

/* Tracing support.  */

/* Return non-zero if tracing of IDX is enabled for CPU.  */
#define TRACE_P(cpu,idx) \
((WITH_TRACE & (1 << (idx))) != 0 \
 && CPU_TRACE_FLAGS (cpu)[idx] != 0)

/* Non-zero if "--trace-insn" specified for CPU.  */
#define TRACE_INSN_P(cpu) TRACE_P (cpu, TRACE_INSN_IDX)
/* Non-zero if "--trace-decode" specified for CPU.  */
#define TRACE_DECODE_P(cpu) TRACE_P (cpu, TRACE_DECODE_IDX)
/* Non-zero if "--trace-fpu" specified for CPU. */
#define TRACE_FPU_P(cpu) TRACE_P (cpu, TRACE_FPU_IDX)

extern void trace_printf PARAMS ((SIM_DESC, sim_cpu *, const char *, ...))
     __attribute__((format (printf, 3, 4)));

/* Debug support.
   This is included here because there isn't enough of it to justify
   a sim-debug.h.  */

/* Return non-zero if debugging of IDX for CPU is enabled.  */
#define DEBUG_P(cpu, idx) \
((WITH_DEBUG & (1 << (idx))) != 0 \
 && CPU_DEBUG_FLAGS (cpu)[idx] != 0)

/* Non-zero if "--debug-insn" specified.  */
#define DEBUG_INSN_P(cpu) DEBUG_P (cpu, DEBUG_INSN_IDX)

extern void debug_printf PARAMS ((struct _sim_cpu *, const char *, ...))
     __attribute__((format (printf, 2, 3)));

#endif /* SIM_TRACE_H */
