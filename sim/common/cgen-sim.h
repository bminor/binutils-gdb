/* Simulator header for Cpu tools GENerated simulators.
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

#ifndef CGEN_SIM_H
#define CGEN_SIM_H

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

/* Forward decls.  Defined in the machine generated files.  */

/* This holds the contents of the extracted insn.
   There are a few common entries (e.g. pc address), and then one big
   union with an entry for each of the instruction formats.  */
typedef struct argbuf ARGBUF;

/* This is one ARGBUF plus whatever else is needed for WITH_SCACHE support.
   At present there is nothing else, but it also provides a level of
   abstraction.  */
typedef struct scache SCACHE;

/* This is a union with one entry for each instruction format.
   Each entry contains all of the non-constant inputs of the instruction.  */
typedef struct parexec PAREXEC;

/* An "Instruction DESCriptor".
   This is the main handle on an instruction for the simulator.  */
typedef struct idesc IDESC;

/* Engine support.
   ??? This is here because it's needed before eng.h (built by genmloop.sh)
   which is needed before cgen-engine.h and cpu.h.
   ??? This depends on a cpu family specific type, PCADDR, but no machine
   generated headers will have been included yet.  sim/common currently
   requires the typedef of sim_cia in sim-main.h between the inclusion of
   sim-basics.h and sim-base.h so this is no different.  */

#if WITH_SCACHE

/* instruction address
   ??? This was intended to be a struct of two elements in the WITH_SCACHE_PBB
   case.  The first element is the PCADDR, the second element is the SCACHE *.
   Haven't found the time yet to make this work, but it is a nicer approach
   than the current br_cache stuff.  */
typedef PCADDR IADDR;
/* current instruction address */
typedef PCADDR CIA;
/* argument to semantic functions */
typedef SCACHE *SEM_ARG;
/* semantic code's version of pc */
#if WITH_SCACHE_PBB
typedef SCACHE *SEM_PC;
#else
typedef PCADDR SEM_PC;
#endif

#else /* ! WITH_SCACHE */

/* instruction address */
typedef PCADDR IADDR;
/* current instruction address */
typedef PCADDR CIA;
/* argument to semantic functions */
typedef ARGBUF *SEM_ARG;
/* semantic code's version of pc */
typedef PCADDR SEM_PC;

#endif /* ! WITH_SCACHE */

/* Additional opcode table support.  */

/* Opcode table for virtual insns (only used by the simulator).  */
extern const struct cgen_insn cgen_virtual_opcode_table[];

/* -ve of indices of virtual insns in cgen_virtual_opcode_table.  */
typedef enum {
  VIRTUAL_INSN_X_INVALID = 0,
  VIRTUAL_INSN_X_BEFORE = -1, VIRTUAL_INSN_X_AFTER = -2,
  VIRTUAL_INSN_X_BEGIN = -3,
  VIRTUAL_INSN_X_CHAIN= -4, VIRTUAL_INSN_X_CTI_CHAIN = -5
} CGEN_INSN_VIRTUAL_TYPE;

/* Return non-zero if OPCODE is a virtual insn.  */
#define CGEN_INSN_VIRTUAL_P(opcode) \
  CGEN_INSN_ATTR ((opcode), CGEN_INSN_VIRTUAL)

/* GNU C's "computed goto" facility is used to speed things up where
   possible.  These macros provide a portable way to use them.
   Nesting of these switch statements is done by providing an extra argument
   that distinguishes them.  `N' can be a number or symbol.
   Variable `labels_##N' must be initialized with the labels of each case.  */

#ifdef __GNUC__
#define SWITCH(N, X) goto *X;
#define CASE(N, X) case_##N##_##X
#define BREAK(N) goto end_switch_##N
#define DEFAULT(N) default_##N
#define ENDSWITCH(N) end_switch_##N:
#else
#define SWITCH(N, X) switch (X)
#define CASE(N, X) case X /* FIXME: old sem-switch had (@arch@_,X) here */
#define BREAK(N) break
#define DEFAULT(N) default
#define ENDSWITCH(N)
#endif

/* Simulator state.  */

/* Records simulator descriptor so utilities like @cpu@_dump_regs can be
   called from gdb.  */
extern SIM_DESC current_state;

/* Simulator state.  */

/* CGEN_STATE contains additional state information not present in
   sim_state_base.  */

typedef struct cgen_state {
  /* FIXME: Moved to sim_state_base.  */
  /* argv, env */
  char **argv;
#define STATE_ARGV(s) ((s) -> cgen_state.argv)
  /* FIXME: Move to sim_state_base.  */
  char **envp;
#define STATE_ENVP(s) ((s) -> cgen_state.envp)

  /* Non-zero if no tracing or profiling is selected.  */
  int run_fast_p;
#define STATE_RUN_FAST_P(sd) ((sd) -> cgen_state.run_fast_p)

  /* Opcode table.  */
  CGEN_OPCODE_DESC opcode_table;
#define STATE_OPCODE_TABLE(sd) ((sd) -> cgen_state.opcode_table)
} CGEN_STATE;

/* Various utilities.  */

/* Called after sim_post_argv_init to do any cgen initialization.  */
extern void cgen_init (SIM_DESC);

/* Return the name of an insn.  */
extern CPU_INSN_NAME_FN cgen_insn_name;

/* Return the maximum number of extra bytes required for a sim_cpu struct.  */
/* ??? Ok, yes, this is less pretty than it should be.  Give me a better
   language [or suggest a better way].  */
extern int cgen_cpu_max_extra_bytes (void);

extern void
sim_disassemble_insn (SIM_CPU *, const CGEN_INSN *,
		      const struct argbuf *, PCADDR, char *);

/* Called to process an invalid instruction.  */
extern void sim_engine_invalid_insn (SIM_CPU *, PCADDR);

#endif /* CGEN_SIM_H */
