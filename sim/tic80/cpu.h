/* TIc80 Simulator.
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



struct _sim_cpu {
  unsigned32 reg[32];
  unsigned64 acc[4];
  sim_cia cia;
  sim_cpu_base base;
};

#define GPR(N) ((CPU)->reg[N])
#define ACC(N) ((CPU)->acc[N])

#if defined(WITH_TRACE)
extern char *tic80_trace_alu3	  PARAMS ((int, unsigned32, unsigned32, unsigned32));
extern char *tic80_trace_alu2	  PARAMS ((int, unsigned32, unsigned32));
extern char *tic80_trace_nop	  PARAMS ((int));
extern char *tic80_trace_sink1	  PARAMS ((int, unsigned32));
extern char *tic80_trace_sink2	  PARAMS ((int, unsigned32, unsigned32));
extern char *tic80_trace_cond_br  PARAMS ((int, int, unsigned32, unsigned32));
extern char *tic80_trace_ucond_br PARAMS ((int, unsigned32));
extern char *tic80_trace_ldst	  PARAMS ((int, int, int, int, unsigned32, unsigned32, unsigned32));

#define TRACE_ALU3(indx, result, input1, input2)			\
do {									\
  if (TRACE_ALU_P (CPU)) {						\
    trace_one_insn (SD, CPU, cia.ip, 1, itable[indx].file,		\
		    itable[indx].line_nr, "alu",			\
		    tic80_trace_alu3 (indx, result, input1, input2));	\
  }									\
} while (0)

#define TRACE_ALU2(indx, result, input)					\
do {									\
  if (TRACE_ALU_P (CPU)) {						\
    trace_one_insn (SD, CPU, cia.ip, 1, itable[indx].file,		\
		    itable[indx].line_nr, "alu",			\
		    tic80_trace_alu2 (indx, result, input));		\
  }									\
} while (0)

#define TRACE_NOP(indx)							\
do {									\
  if (TRACE_ALU_P (CPU)) {						\
    trace_one_insn (SD, CPU, cia.ip, 1, itable[indx].file,		\
		    itable[indx].line_nr, "nop",			\
		    tic80_trace_nop (indx));				\
  }									\
} while (0)

#define TRACE_SINK1(indx, input)					\
do {									\
  if (TRACE_ALU_P (CPU)) {						\
    trace_one_insn (SD, CPU, cia.ip, 1, itable[indx].file,		\
		    itable[indx].line_nr, "nop",			\
		    tic80_trace_sink1 (indx, input));			\
  }									\
} while (0)

#define TRACE_SINK2(indx, input1, input2)				\
do {									\
  if (TRACE_ALU_P (CPU)) {						\
    trace_one_insn (SD, CPU, cia.ip, 1, itable[indx].file,		\
		    itable[indx].line_nr, "nop",			\
		    tic80_trace_sink2 (indx, input1, input2));		\
  }									\
} while (0)

#define TRACE_COND_BR(indx, jump_p, cond, target)			\
do {									\
  if (TRACE_BRANCH_P (CPU)) {						\
    trace_one_insn (SD, CPU, cia.ip, 1, itable[indx].file,		\
		    itable[indx].line_nr, "branch",			\
		    tic80_trace_cond_br (indx, jump_p, cond, target));	\
  }									\
} while (0)

#define TRACE_UCOND_BR(indx, target)					\
do {									\
  if (TRACE_ALU_P (CPU)) {						\
    trace_one_insn (SD, CPU, cia.ip, 1, itable[indx].file,		\
		    itable[indx].line_nr, "branch",			\
		    tic80_trace_ucond_br (indx, target));		\
  }									\
} while (0)

#define TRACE_LD(indx, result, m, s, addr1, addr2)			\
do {									\
  if (TRACE_MEMORY_P (CPU)) {						\
    trace_one_insn (SD, CPU, cia.ip, 1, itable[indx].file,		\
		    itable[indx].line_nr, "memory",			\
		    tic80_trace_ldst (indx, 0, m, s, result,		\
				      addr1, addr2));			\
  }									\
} while (0)

#define TRACE_ST(indx, value, m, s, addr1, addr2)			\
do {									\
  if (TRACE_MEMORY_P (CPU)) {						\
    trace_one_insn (SD, CPU, cia.ip, 1, itable[indx].file,		\
		    itable[indx].line_nr, "memory",			\
		    tic80_trace_ldst (indx, 1, m, s, value,		\
				      addr1, addr2));			\
  }									\
} while (0)

#else
#define TRACE_ALU3(indx, result, input1, input2)
#define TRACE_ALU2(indx, result, input)
#define TRACE_NOP(indx)
#define TRACE_SINK1(indx, input)
#define TRACE_SINK2(indx, input1, input2)
#define TRACE_COND_BR(indx, jump_p, cond, target)
#define TRACE_UCOND_BR(indx, target)
#define TRACE_LD(indx, m, s, result, addr1, addr2)
#define TRACE_ST(indx, m, s, value, addr1, addr2)
#endif
