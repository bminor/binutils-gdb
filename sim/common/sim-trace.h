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

/* Tracing support.  */

#if WITH_SMP

/* Return non-zero if tracing of IDX is enabled for CPU.  */
#define TRACE_P(cpu,idx) \
((WITH_TRACE & (1 << (idx))) != 0 \
 && CPU_TRACE_FLAGS (cpu)[idx] != 0)

/* Non-zero if "--trace-insn" specified for CPU.  */
#define TRACE_INSN_P(cpu) TRACE_P (cpu, TRACE_INSN_IDX)

#define TRACE_PRINTF(cpu, idx, args) \
do { \
  if ((WITH_TRACE & (1 << (idx))) != 0 \
      && CPU_TRACE_FLAGS (cpu)[idx] != 0) \
    trace_printf args; \
} while (0)

#else

/* Fetch current tracing flags.  */
#define CURRENT_TRACE_FLAGS CPU_TRACE_FLAGS (& CURRENT_STATE->cpu)

/* Return non-zero if tracing of IDX is enabled.  */
#define TRACE_P(idx) \
((WITH_TRACE & (1 << (idx))) != 0 \
 && CURRENT_TRACE_FLAGS[idx] != 0)

/* Non-zero if "--trace-insn" specified.  */
#define TRACE_INSN_P TRACE_P (TRACE_INSN_IDX)

#define TRACE_PRINTF(idx, args) \
do { \
  if ((WITH_TRACE & (1 << (idx))) != 0 \
      && CURRENT_TRACE_FLAGS[idx] != 0) \
    trace_printf args; \
} while (0)

#endif /* WITH_SMP */

extern void trace_printf PARAMS ((char *, ...));

/* Debug support.  */

#if WITH_SMP

/* Return non-zero if debugging of IDX for CPU is enabled.  */
#define DEBUG_P(cpu, idx) \
((WITH_DEBUG & (1 << (idx))) != 0 \
 && CPU_DEBUG_FLAGS (cpu)[idx] != 0)

/* Non-zero if "--debug-insn" specified.  */
#define DEBUG_INSN_P(cpu) DEBUG_P (cpu, DEBUG_INSN_IDX)

#define DEBUG_PRINTF(cpu, idx, args) \
do { \
  if ((WITH_DEBUG & (1 << (idx))) != 0 \
      && CPU_DEBUG_FLAGS (cpu)[idx] != 0) \
    debug_printf args; \
} while (0)

#else

/* Fetch current debugging flags.  */
#define CURRENT_DEBUG_FLAGS CPU_DEBUG_FLAGS (& CURRENT_STATE->cpu)

/* Return non-zero if debugging of IDX is enabled.  */
#define DEBUG_P(idx) \
((WITH_DEBUG & (1 << (idx))) != 0 \
 && CURRENT_DEBUG_FLAGS[idx] != 0)

/* Non-zero if "--debug-insn" specified.  */
#define DEBUG_INSN_P DEBUG_P (DEBUG_INSN_IDX)

#define DEBUG_PRINTF(idx, args) \
do { \
  if ((WITH_DEBUG & (1 << (idx))) != 0 \
      && CURRENT_DEBUG_FLAGS[idx] != 0) \
    debug_printf args; \
} while (0)

#endif /* WITH_SMP */

extern void debug_printf PARAMS ((char *, ...));

#endif /* SIM_TRACE_H */
