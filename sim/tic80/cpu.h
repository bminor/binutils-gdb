/* TIc80 Simulator.
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



/* TI C80 control registers */

typedef enum {
  EPC_CR,
  EIP_CR,
  CONFIG_CR,
  INTPEN_CR,
  IE_CR,
  FPST_CR,
  PPERROR_CR,
  PKTREQ_CR,
  TCOUNT_CR,
  TSCALE_CR,
  FLTOP_CR,
  FLTADR_CR,
  FLTTAG_CR,
  FLTDLT_CR,
  FLTDTH_CR,
  FLT005_CR,
  FLT006_CR,
  FLT007_CR,
  FLT008_CR,
  FLT009_CR,
  FLT010_CR,
  FLT011_CR,
  FLT012_CR,
  FLT013_CR,
  FLT014_CR,
  FLT015_CR,
  SYSSTK_CR,
  SYSTMP_CR,
  MPC_CR,
  MIP_CR,
  ECOMCNTL_CR,
  ANASTAT_CR,
  BRK1_CR,
  BRK2_CR,
  ITAG0_CR,
  ITAG1_CR,
  ITAG2_CR,
  ITAG3_CR,
  ITAG4_CR,
  ITAG5_CR,
  ITAG6_CR,
  ITAG7_CR,
  ITAG8_CR,
  ITAG9_CR,
  ITAG10_CR,
  ITAG11_CR,
  ITAG12_CR,
  ITAG13_CR,
  ITAG14_CR,
  ITAG15_CR,
  ILRU_CR,
  DTAG0_CR,
  DTAG1_CR,
  DTAG2_CR,
  DTAG3_CR,
  DTAG4_CR,
  DTAG5_CR,
  DTAG6_CR,
  DTAG7_CR,
  DTAG8_CR,
  DTAG9_CR,
  DTAG10_CR,
  DTAG11_CR,
  DTAG12_CR,
  DTAG13_CR,
  DTAG14_CR,
  DTAG15_CR,
  DLRU_CR,
  IN0P_CR,
  IN1P_CR,
  OUTP_CR,
  SCRATCH_CR,
  nr_tic80_control_regs,
} tic80_control_regs;

/* extern int tic80_cr2index (tic80_control_regs reg); */

/* Map an instruction CR index onto the corresponding internal cr enum
   or SCRATCH_CR if the index is invalid */

extern tic80_control_regs tic80_index2cr (int index);


/* TIc80 interrupt register bits */

enum {
  IE_CR_PE = BIT32(31),
  IE_CR_X4 = BIT32(30),
  IE_CR_X3 = BIT32(29),
  IE_CR_BP = BIT32(28),
  IE_CR_PB = BIT32(27),
  IE_CR_PC = BIT32(26),
  IE_CR_MI = BIT32(25),
  /**/
  IE_CR_P3 = BIT32(19),
  IE_CR_P2 = BIT32(18),
  IE_CR_P1 = BIT32(17),
  IE_CR_P0 = BIT32(16),
  IE_CR_IO = BIT32(15),
  IE_CR_MF = BIT32(14),
  /**/
  IE_CR_X2 = BIT32(12),
  IE_CR_X1 = BIT32(11),
  IE_CR_TI = BIT32(10),
  IE_CR_F1 = BIT32(9),
  IE_CR_F0 = BIT32(8),
  IE_CR_FX = BIT32(7),
  IE_CR_FU = BIT32(6),
  IE_CR_FO = BIT32(5),
  /**/
  IE_CR_FZ = BIT32(3),
  IE_CR_FI = BIT32(2),
  /**/
  IE_CR_IE = BIT32(0),
};




struct _sim_cpu {
  unsigned32 reg[32];
  unsigned64 acc[4];
  unsigned32 cr[nr_tic80_control_regs];
  int is_user_mode; /* hidden mode latch */
  sim_cia cia;
  sim_cpu_base base;
};

#define CIA_GET(CPU) ((CPU)->cia)
#define CIA_SET(CPU,VAL) ((CPU)->cia = (VAL))

#define GPR(N) ((CPU)->reg[N])
#define GPR_CLEAR(N) (GPR((N)) = 0)
#define ACC(N) ((CPU)->acc[N])
#define CR(N) ((CPU)->cr[tic80_index2cr ((N))])



#if defined(WITH_TRACE)
extern char *tic80_trace_alu3	  PARAMS ((int, unsigned32, unsigned32, unsigned32));
extern char *tic80_trace_cmp	  PARAMS ((int, unsigned32, unsigned32, unsigned32));
extern char *tic80_trace_alu2	  PARAMS ((int, unsigned32, unsigned32));
extern char *tic80_trace_shift	  PARAMS ((int, unsigned32, unsigned32, int, int, int, int, int));
extern void tic80_trace_fpu3	  PARAMS ((SIM_DESC, sim_cpu *, sim_cia, int, sim_fpu *, sim_fpu *, sim_fpu *));
extern void tic80_trace_fpu2	  PARAMS ((SIM_DESC, sim_cpu *, sim_cia, int, sim_fpu *, sim_fpu *));
extern void tic80_trace_fpu1	  PARAMS ((SIM_DESC, sim_cpu *, sim_cia, int, sim_fpu *));
extern void tic80_trace_fpu2i	  PARAMS ((SIM_DESC, sim_cpu *, sim_cia, int, unsigned32, sim_fpu *, sim_fpu *));
extern void tic80_trace_fpu2cmp	  PARAMS ((SIM_DESC, sim_cpu *, sim_cia, int, unsigned32, sim_fpu *, sim_fpu *));
extern char *tic80_trace_nop	  PARAMS ((int));
extern char *tic80_trace_sink1	  PARAMS ((int, unsigned32));
extern char *tic80_trace_sink2	  PARAMS ((int, unsigned32, unsigned32));
extern char *tic80_trace_sink3	  PARAMS ((int, unsigned32, unsigned32, unsigned32));
extern char *tic80_trace_cond_br  PARAMS ((int, int, unsigned32, unsigned32, int, int));
extern char *tic80_trace_ucond_br PARAMS ((int, unsigned32));
extern void tic80_trace_ldst	  PARAMS ((SIM_DESC, sim_cpu *, sim_cia, int, int, int, int, unsigned32, unsigned32, unsigned32));

#define TRACE_ALU3(indx, result, input1, input2)			\
do {									\
  if (TRACE_ALU_P (CPU)) {						\
    trace_one_insn (SD, CPU, cia.ip, 1, itable[indx].file,		\
		    itable[indx].line_nr, "alu",			\
		    tic80_trace_alu3 (indx, result, input1, input2));	\
  }									\
} while (0)

#define TRACE_CMP(indx, result, input1, input2)				\
do {									\
  if (TRACE_ALU_P (CPU)) {						\
    trace_one_insn (SD, CPU, cia.ip, 1, itable[indx].file,		\
		    itable[indx].line_nr, "alu",			\
		    tic80_trace_cmp (indx, result, input1, input2));	\
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

#define TRACE_SHIFT(indx, result, input, i, n, merge, endmask, rotate)	\
do {									\
  if (TRACE_ALU_P (CPU)) {						\
    trace_one_insn (SD, CPU, cia.ip, 1, itable[indx].file,		\
		    itable[indx].line_nr, "shift",			\
		    tic80_trace_shift (indx, result, input, i, n,	\
				       merge, endmask, rotate));	\
  }									\
} while (0)

#define TRACE_FPU3(result, input1, input2)				\
do {									\
  if (TRACE_FPU_P (CPU)) {						\
    tic80_trace_fpu3 (SD, CPU, cia, MY_INDEX, 				\
		      &result, &input1, &input2);			\
  }									\
} while (0)

#define TRACE_FPU2(result, input)					\
do {									\
  if (TRACE_FPU_P (CPU)) {						\
    tic80_trace_fpu2 (SD, CPU, cia, MY_INDEX, 				\
		      &result, &input);					\
  }									\
} while (0)

#define TRACE_FPU1(result)						\
do {									\
  if (TRACE_FPU_P (CPU)) {						\
    tic80_trace_fpu1 (SD, CPU, cia, MY_INDEX, 				\
		      &result);						\
  }									\
} while (0)

#define TRACE_FPU2I(result, input1, input2)				\
do {									\
  if (TRACE_FPU_P (CPU)) {						\
    tic80_trace_fpu2i (SD, CPU, cia, MY_INDEX, 				\
		       result, &input1, &input2);			\
  }									\
} while (0)

#define TRACE_FPU2CMP(result, input1, input2)				\
do {									\
  if (TRACE_FPU_P (CPU)) {						\
    tic80_trace_fpu2cmp (SD, CPU, cia, MY_INDEX,			\
			 result, &input1, &input2);			\
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

#define TRACE_SINK3(indx, input1, input2, input3)			\
do {									\
  if (TRACE_ALU_P (CPU)) {						\
    trace_one_insn (SD, CPU, cia.ip, 1, itable[indx].file,		\
		    itable[indx].line_nr, "nop",			\
		    tic80_trace_sink3 (indx, input1, input2, input3));	\
  }									\
} while (0)

#define TRACE_COND_BR(indx, jump_p, cond, target, size, code)		\
do {									\
  if (TRACE_BRANCH_P (CPU)) {						\
    trace_one_insn (SD, CPU, cia.ip, 1, itable[indx].file,		\
		    itable[indx].line_nr, "branch",			\
		    tic80_trace_cond_br (indx, jump_p, cond, target,	\
					 size, code));			\
  }									\
} while (0)

#define TRACE_UCOND_BR(indx, target)					\
do {									\
  if (TRACE_BRANCH_P (CPU)) {						\
    trace_one_insn (SD, CPU, cia.ip, 1, itable[indx].file,		\
		    itable[indx].line_nr, "branch",			\
		    tic80_trace_ucond_br (indx, target));		\
  }									\
} while (0)

#define TRACE_LD(result, m, s, addr1, addr2)				\
do {									\
  if (TRACE_MEMORY_P (CPU)) {						\
    tic80_trace_ldst (SD, CPU, cia, MY_INDEX, 				\
 		      0, m, s, result, addr1, addr2);			\
  }									\
} while (0)

#define TRACE_ST(value, m, s, addr1, addr2)				\
do {									\
  if (TRACE_MEMORY_P (CPU)) {						\
    tic80_trace_ldst (SD, CPU, cia, MY_INDEX, 				\
		      1, m, s, value, addr1, addr2);			\
  }									\
} while (0)

#else
#define TRACE_ALU3(indx, result, input1, input2)
#define TRACE_ALU2(indx, result, input)
#define TRACE_FPU3(result, input1, input2)
#define TRACE_FPU2(result, input)
#define TRACE_FPU1(result)
#define TRACE_FPU2I(result, input1, input2)
#define TRACE_NOP(indx)
#define TRACE_SINK1(indx, input)
#define TRACE_SINK2(indx, input1, input2)
#define TRACE_SINK3(indx, input1, input2, input3)
#define TRACE_COND_BR(indx, jump_p, cond, target, size, code)
#define TRACE_UCOND_BR(indx, target)
#define TRACE_LD(m, s, result, addr1, addr2)
#define TRACE_ST(m, s, value, addr1, addr2)
#endif
