/* Simulator header for Cpu tools GENerated simulators.
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

#ifndef CGEN_SIM_H
#define CGEN_SIM_H

#define PC (STATE_CPU_CPU (current_state, 0)->pc)

/* Execution state.  */
enum exec_state {
  EXEC_STATE_RUNNING, EXEC_STATE_EXITED,
  EXEC_STATE_STOPPED, EXEC_STATE_SIGNALLED
};

/* Signals we use.  */
enum sim_signal_type {
  SIM_SIGNONE,
  SIM_SIGILL,    /* illegal insn */
  SIM_SIGTRAP,
  SIM_SIGALIGN,  /* misaligned memory access */
  SIM_SIGACCESS, /* tried to read/write memory that's not readable/writable */
  SIM_SIGXCPU    /* cpu limit exceeded */
};

void engine_halt PARAMS ((struct _sim_cpu *, enum exec_state, int));
void engine_signal PARAMS ((struct _sim_cpu *, enum sim_signal_type));

/* Instruction field support macros.  */

#define EXTRACT_SIGNED(val, total, start, length) \
(((((val) >> ((total) - ((start) + (length)))) & ((1 << (length)) - 1)) \
  ^ (1 << ((length) - 1))) \
 - (1 << ((length) - 1)))

#define EXTRACT_UNSIGNED(val, total, start, length) \
(((val) >> ((total) - ((start) + (length)))) & ((1 << (length)) - 1))

/* Compute number of longs required to hold N bits.  */
#define HOST_LONGS_FOR_BITS(n) \
  (((n) + sizeof (long) * 8 - 1) / sizeof (long) * 8)

/* Decode,extract,semantics.  */

typedef void (EXTRACT_FN) PARAMS ((SIM_CPU *, PCADDR, insn_t, struct argbuf *));
/*typedef CIA (SEMANTIC_FN) PARAMS ((SEM_ARG));*/
typedef PCADDR (SEMANTIC_FN) PARAMS ((SIM_CPU *, struct argbuf *));
#if 0 /* wip */
typedef void (EXTRACT_CACHE_FN) PARAMS ((SIM_CPU *, PCADDR, insn_t, struct argbuf *));
#endif
typedef PCADDR (SEMANTIC_CACHE_FN) PARAMS ((SIM_CPU *, struct scache *));

typedef struct {
  /* Using cgen_insn_type requires <cpu>-opc.h.  */
  int /*enum cgen_insn_type*/ insn_type;
  const struct cgen_insn *opcode;
  /* FIXME: Perhaps rename these to normal/fast versions to associate them
     with the normal/fast args to genmloop.sh.  */
  EXTRACT_FN *extract;
  SEMANTIC_FN *semantic;
#if 0 /* wip */
  EXTRACT_CACHE_FN *extract_fast;
#endif
  SEMANTIC_CACHE_FN *semantic_fast;
#if defined (USE_SEM_SWITCH) && defined (__GNUC__)
  void *semantic_lab;
#endif
} DECODE;

/* FIXME: length parm to decode() is currently unneeded.  */
extern DECODE *decode PARAMS ((insn_t /*, int*/));

/* Simulator state.  */

#if WITH_SCACHE
#include "cgen-scache.h"
#endif

/* ??? Do we *need* to pass state to the semantic routines?  */
extern SIM_DESC current_state;

/* FIXME: Until sim_open creates one.  */
extern struct sim_state sim_global_state;

/* Simulator state.  */

/* Main state struct.
   CGEN_STATE contains addition state information not present in
   sim_state_base.  */

typedef struct cgen_state {
  /* argv, env */
  char **argv;
#define STATE_ARGV(s) ((s)->cgen_state.argv)
  char **envp;
#define STATE_ENVP(s) ((s)->cgen_state.envp)
} CGEN_STATE;

/* Additional per-cpu data.  */

typedef struct {
  /* Simulator's execution cache.  */
#if WITH_SCACHE
  CPU_SCACHE scache;
#endif /* WITH_SCACHE */

  enum exec_state exec_state;
#define CPU_EXEC_STATE(cpu) ((cpu)->cgen_cpu.exec_state)

  int halt_sigrc;
#define CPU_HALT_SIGRC(cpu) ((cpu)->cgen_cpu.halt_sigrc)

  jmp_buf halt_jmp_buf;
#define CPU_HALT_JMP_BUF(cpu) ((cpu)->cgen_cpu.halt_jmp_buf)

  CPU_DATA cpu;
#define CPU_CPU(c) (& (c)->cgen_cpu.cpu)
  CPU_PROFILE profile_state;
#define CPU_PROFILE_STATE(cpu) (& (cpu)->cgen_cpu.profile_state)
} CGEN_CPU;

/* Various utilities.  */

int engine_stop (SIM_DESC);
void engine_run (SIM_DESC, int, int);
void engine_resume (SIM_DESC, int, int);
void engine_halt (SIM_CPU *, enum exec_state, int);
void engine_signal (SIM_CPU *, enum sim_signal_type);

int sim_signal_to_host (int);

void
sim_disassemble_insn (const struct cgen_insn *, const struct argbuf *,
		      PCADDR, char *);

#endif /* CGEN_SIM_H */
