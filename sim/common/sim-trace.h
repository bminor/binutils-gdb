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

extern void trace_printf PARAMS ((struct _sim_cpu *, const char *, ...));

/* Debug support.  */

/* Return non-zero if debugging of IDX for CPU is enabled.  */
#define DEBUG_P(cpu, idx) \
((WITH_DEBUG & (1 << (idx))) != 0 \
 && CPU_DEBUG_FLAGS (cpu)[idx] != 0)

/* Non-zero if "--debug-insn" specified.  */
#define DEBUG_INSN_P(cpu) DEBUG_P (cpu, DEBUG_INSN_IDX)

extern void debug_printf PARAMS ((struct _sim_cpu *, const char *, ...));

#endif /* SIM_TRACE_H */
