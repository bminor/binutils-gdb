/* CPU family header for sparc32.

THIS FILE IS MACHINE GENERATED WITH CGEN.

Copyright (C) 1999 Cygnus Solutions, Inc.

This file is part of the Cygnus Simulators.


*/

#ifndef CPU_SPARC32_H
#define CPU_SPARC32_H

/* Maximum number of instructions that are fetched at a time.
   This is for LIW type instructions sets (e.g. m32r).  */
#define MAX_LIW_INSNS 1

/* Maximum number of instructions that can be executed in parallel.  */
#define MAX_PARALLEL_INSNS 1

/* CPU state information.  */
typedef struct {
  /* Hardware elements.  */
  struct {
  /* program counter */
  USI h_pc;
#define GET_H_PC() CPU (h_pc)
#define SET_H_PC(x) (CPU (h_pc) = (x))
  /* next pc */
  SI h_npc;
#define GET_H_NPC() CPU (h_npc)
#define SET_H_NPC(x) (CPU (h_npc) = (x))
/* GET_H_GR macro user-written */
/* SET_H_GR macro user-written */
  /* icc carry bit */
  BI h_icc_c;
#define GET_H_ICC_C() CPU (h_icc_c)
#define SET_H_ICC_C(x) (CPU (h_icc_c) = (x))
  /* icc negative bit */
  BI h_icc_n;
#define GET_H_ICC_N() CPU (h_icc_n)
#define SET_H_ICC_N(x) (CPU (h_icc_n) = (x))
  /* icc overflow bit */
  BI h_icc_v;
#define GET_H_ICC_V() CPU (h_icc_v)
#define SET_H_ICC_V(x) (CPU (h_icc_v) = (x))
  /* icc zero bit */
  BI h_icc_z;
#define GET_H_ICC_Z() CPU (h_icc_z)
#define SET_H_ICC_Z(x) (CPU (h_icc_z) = (x))
  /* xcc carry bit */
  BI h_xcc_c;
#define GET_H_XCC_C() CPU (h_xcc_c)
#define SET_H_XCC_C(x) (CPU (h_xcc_c) = (x))
  /* xcc negative bit */
  BI h_xcc_n;
#define GET_H_XCC_N() CPU (h_xcc_n)
#define SET_H_XCC_N(x) (CPU (h_xcc_n) = (x))
  /* xcc overflow bit */
  BI h_xcc_v;
#define GET_H_XCC_V() CPU (h_xcc_v)
#define SET_H_XCC_V(x) (CPU (h_xcc_v) = (x))
  /* xcc zero bit */
  BI h_xcc_z;
#define GET_H_XCC_Z() CPU (h_xcc_z)
#define SET_H_XCC_Z(x) (CPU (h_xcc_z) = (x))
/* GET_H_Y macro user-written */
/* SET_H_Y macro user-written */
  /* ancilliary state registers */
  SI h_asr[32];
#define GET_H_ASR(a1) CPU (h_asr)[a1]
#define SET_H_ASR(a1, x) (CPU (h_asr)[a1] = (x))
  /* annul next insn? - assists execution */
  BI h_annul_p;
#define GET_H_ANNUL_P() CPU (h_annul_p)
#define SET_H_ANNUL_P(x) (CPU (h_annul_p) = (x))
  /* floating point regs */
  SF h_fr[32];
#define GET_H_FR(a1) CPU (h_fr)[a1]
#define SET_H_FR(a1, x) (CPU (h_fr)[a1] = (x))
  /* psr register */
  USI h_psr;
/* GET_H_PSR macro user-written */
/* SET_H_PSR macro user-written */
  /* supervisor bit */
  BI h_s;
#define GET_H_S() CPU (h_s)
#define SET_H_S(x) (CPU (h_s) = (x))
  /* previous supervisor bit */
  BI h_ps;
#define GET_H_PS() CPU (h_ps)
#define SET_H_PS(x) (CPU (h_ps) = (x))
  /* processor interrupt level */
  UQI h_pil;
#define GET_H_PIL() CPU (h_pil)
#define SET_H_PIL(x) (CPU (h_pil) = (x))
  /* enable traps bit */
  BI h_et;
#define GET_H_ET() CPU (h_et)
#define SET_H_ET(x) (CPU (h_et) = (x))
  /* tbr register */
  SI h_tbr;
/* GET_H_TBR macro user-written */
/* SET_H_TBR macro user-written */
  /* current window pointer */
  UQI h_cwp;
/* GET_H_CWP macro user-written */
/* SET_H_CWP macro user-written */
  /* window invalid mask */
  USI h_wim;
/* GET_H_WIM macro user-written */
/* SET_H_WIM macro user-written */
  /* alternate global indicator */
  QI h_ag;
#define GET_H_AG() CPU (h_ag)
#define SET_H_AG(x) (CPU (h_ag) = (x))
  /* enable coprocessor bit */
  BI h_ec;
#define GET_H_EC() CPU (h_ec)
#define SET_H_EC(x) (CPU (h_ec) = (x))
  /* enable fpu bit */
  BI h_ef;
#define GET_H_EF() CPU (h_ef)
#define SET_H_EF(x) (CPU (h_ef) = (x))
  /* floating point status register */
  USI h_fsr;
#define GET_H_FSR() CPU (h_fsr)
#define SET_H_FSR(x) (CPU (h_fsr) = (x))
  } hardware;
#define CPU_CGEN_HW(cpu) (& (cpu)->cpu_data.hardware)
} SPARC32_CPU_DATA;

/* Cover fns for register access.  */
USI sparc32_h_pc_get (SIM_CPU *);
void sparc32_h_pc_set (SIM_CPU *, USI);
SI sparc32_h_npc_get (SIM_CPU *);
void sparc32_h_npc_set (SIM_CPU *, SI);
SI sparc32_h_gr_get (SIM_CPU *, UINT);
void sparc32_h_gr_set (SIM_CPU *, UINT, SI);
BI sparc32_h_icc_c_get (SIM_CPU *);
void sparc32_h_icc_c_set (SIM_CPU *, BI);
BI sparc32_h_icc_n_get (SIM_CPU *);
void sparc32_h_icc_n_set (SIM_CPU *, BI);
BI sparc32_h_icc_v_get (SIM_CPU *);
void sparc32_h_icc_v_set (SIM_CPU *, BI);
BI sparc32_h_icc_z_get (SIM_CPU *);
void sparc32_h_icc_z_set (SIM_CPU *, BI);
BI sparc32_h_xcc_c_get (SIM_CPU *);
void sparc32_h_xcc_c_set (SIM_CPU *, BI);
BI sparc32_h_xcc_n_get (SIM_CPU *);
void sparc32_h_xcc_n_set (SIM_CPU *, BI);
BI sparc32_h_xcc_v_get (SIM_CPU *);
void sparc32_h_xcc_v_set (SIM_CPU *, BI);
BI sparc32_h_xcc_z_get (SIM_CPU *);
void sparc32_h_xcc_z_set (SIM_CPU *, BI);
SI sparc32_h_y_get (SIM_CPU *);
void sparc32_h_y_set (SIM_CPU *, SI);
SI sparc32_h_asr_get (SIM_CPU *, UINT);
void sparc32_h_asr_set (SIM_CPU *, UINT, SI);
BI sparc32_h_annul_p_get (SIM_CPU *);
void sparc32_h_annul_p_set (SIM_CPU *, BI);
SF sparc32_h_fr_get (SIM_CPU *, UINT);
void sparc32_h_fr_set (SIM_CPU *, UINT, SF);
USI sparc32_h_psr_get (SIM_CPU *);
void sparc32_h_psr_set (SIM_CPU *, USI);
BI sparc32_h_s_get (SIM_CPU *);
void sparc32_h_s_set (SIM_CPU *, BI);
BI sparc32_h_ps_get (SIM_CPU *);
void sparc32_h_ps_set (SIM_CPU *, BI);
UQI sparc32_h_pil_get (SIM_CPU *);
void sparc32_h_pil_set (SIM_CPU *, UQI);
BI sparc32_h_et_get (SIM_CPU *);
void sparc32_h_et_set (SIM_CPU *, BI);
SI sparc32_h_tbr_get (SIM_CPU *);
void sparc32_h_tbr_set (SIM_CPU *, SI);
UQI sparc32_h_cwp_get (SIM_CPU *);
void sparc32_h_cwp_set (SIM_CPU *, UQI);
USI sparc32_h_wim_get (SIM_CPU *);
void sparc32_h_wim_set (SIM_CPU *, USI);
QI sparc32_h_ag_get (SIM_CPU *);
void sparc32_h_ag_set (SIM_CPU *, QI);
BI sparc32_h_ec_get (SIM_CPU *);
void sparc32_h_ec_set (SIM_CPU *, BI);
BI sparc32_h_ef_get (SIM_CPU *);
void sparc32_h_ef_set (SIM_CPU *, BI);
USI sparc32_h_fsr_get (SIM_CPU *);
void sparc32_h_fsr_set (SIM_CPU *, USI);

/* These must be hand-written.  */
extern CPUREG_FETCH_FN sparc32_fetch_register;
extern CPUREG_STORE_FN sparc32_store_register;

typedef struct {
  int empty;
} MODEL_SPARC32_DEF_DATA;

/* The ARGBUF struct.  */
struct argbuf {
  /* These are the baseclass definitions.  */
  IADDR addr;
  const IDESC *idesc;
  char trace_p;
  char profile_p;
  /* cpu specific data follows */
  CGEN_INSN_INT insn;
  int written;
};

/* A cached insn.

   ??? SCACHE used to contain more than just argbuf.  We could delete the
   type entirely and always just use ARGBUF, but for future concerns and as
   a level of abstraction it is left in.  */

struct scache {
  struct argbuf argbuf;
};

/* Macros to simplify extraction, reading and semantic code.
   These define and assign the local vars that contain the insn's fields.  */

#define EXTRACT_IFMT_EMPTY_VARS \
  /* Instruction fields.  */ \
  unsigned int length;
#define EXTRACT_IFMT_EMPTY_CODE \
  length = 0; \

#define EXTRACT_IFMT_RD_ASR_VARS \
  /* Instruction fields.  */ \
  INT f_simm13; \
  UINT f_i; \
  UINT f_rs1; \
  UINT f_op3; \
  UINT f_rd; \
  UINT f_op; \
  unsigned int length;
#define EXTRACT_IFMT_RD_ASR_CODE \
  length = 4; \
  f_simm13 = EXTRACT_INT (insn, 32, 12, 13); \
  f_i = EXTRACT_UINT (insn, 32, 13, 1); \
  f_rs1 = EXTRACT_UINT (insn, 32, 18, 5); \
  f_op3 = EXTRACT_UINT (insn, 32, 24, 6); \
  f_rd = EXTRACT_UINT (insn, 32, 29, 5); \
  f_op = EXTRACT_UINT (insn, 32, 31, 2); \

#define EXTRACT_IFMT_WR_ASR_VARS \
  /* Instruction fields.  */ \
  UINT f_rs2; \
  INT f_res_asi; \
  UINT f_i; \
  UINT f_rs1; \
  UINT f_op3; \
  UINT f_rd; \
  UINT f_op; \
  unsigned int length;
#define EXTRACT_IFMT_WR_ASR_CODE \
  length = 4; \
  f_rs2 = EXTRACT_UINT (insn, 32, 4, 5); \
  f_res_asi = EXTRACT_INT (insn, 32, 12, 8); \
  f_i = EXTRACT_UINT (insn, 32, 13, 1); \
  f_rs1 = EXTRACT_UINT (insn, 32, 18, 5); \
  f_op3 = EXTRACT_UINT (insn, 32, 24, 6); \
  f_rd = EXTRACT_UINT (insn, 32, 29, 5); \
  f_op = EXTRACT_UINT (insn, 32, 31, 2); \

#define EXTRACT_IFMT_WR_ASR_IMM_VARS \
  /* Instruction fields.  */ \
  INT f_simm13; \
  UINT f_i; \
  UINT f_rs1; \
  UINT f_op3; \
  UINT f_rd; \
  UINT f_op; \
  unsigned int length;
#define EXTRACT_IFMT_WR_ASR_IMM_CODE \
  length = 4; \
  f_simm13 = EXTRACT_INT (insn, 32, 12, 13); \
  f_i = EXTRACT_UINT (insn, 32, 13, 1); \
  f_rs1 = EXTRACT_UINT (insn, 32, 18, 5); \
  f_op3 = EXTRACT_UINT (insn, 32, 24, 6); \
  f_rd = EXTRACT_UINT (insn, 32, 29, 5); \
  f_op = EXTRACT_UINT (insn, 32, 31, 2); \

#define EXTRACT_IFMT_RD_PSR_VARS \
  /* Instruction fields.  */ \
  INT f_simm13; \
  UINT f_i; \
  UINT f_rs1; \
  UINT f_op3; \
  UINT f_rd; \
  UINT f_op; \
  unsigned int length;
#define EXTRACT_IFMT_RD_PSR_CODE \
  length = 4; \
  f_simm13 = EXTRACT_INT (insn, 32, 12, 13); \
  f_i = EXTRACT_UINT (insn, 32, 13, 1); \
  f_rs1 = EXTRACT_UINT (insn, 32, 18, 5); \
  f_op3 = EXTRACT_UINT (insn, 32, 24, 6); \
  f_rd = EXTRACT_UINT (insn, 32, 29, 5); \
  f_op = EXTRACT_UINT (insn, 32, 31, 2); \

#define EXTRACT_IFMT_WR_PSR_VARS \
  /* Instruction fields.  */ \
  UINT f_rs2; \
  INT f_res_asi; \
  UINT f_i; \
  UINT f_rs1; \
  UINT f_op3; \
  UINT f_rd; \
  UINT f_op; \
  unsigned int length;
#define EXTRACT_IFMT_WR_PSR_CODE \
  length = 4; \
  f_rs2 = EXTRACT_UINT (insn, 32, 4, 5); \
  f_res_asi = EXTRACT_INT (insn, 32, 12, 8); \
  f_i = EXTRACT_UINT (insn, 32, 13, 1); \
  f_rs1 = EXTRACT_UINT (insn, 32, 18, 5); \
  f_op3 = EXTRACT_UINT (insn, 32, 24, 6); \
  f_rd = EXTRACT_UINT (insn, 32, 29, 5); \
  f_op = EXTRACT_UINT (insn, 32, 31, 2); \

#define EXTRACT_IFMT_WR_PSR_IMM_VARS \
  /* Instruction fields.  */ \
  INT f_simm13; \
  UINT f_i; \
  UINT f_rs1; \
  UINT f_op3; \
  UINT f_rd; \
  UINT f_op; \
  unsigned int length;
#define EXTRACT_IFMT_WR_PSR_IMM_CODE \
  length = 4; \
  f_simm13 = EXTRACT_INT (insn, 32, 12, 13); \
  f_i = EXTRACT_UINT (insn, 32, 13, 1); \
  f_rs1 = EXTRACT_UINT (insn, 32, 18, 5); \
  f_op3 = EXTRACT_UINT (insn, 32, 24, 6); \
  f_rd = EXTRACT_UINT (insn, 32, 29, 5); \
  f_op = EXTRACT_UINT (insn, 32, 31, 2); \

#define EXTRACT_IFMT_LDSTUB_REG_REG_VARS \
  /* Instruction fields.  */ \
  UINT f_rs2; \
  INT f_res_asi; \
  UINT f_i; \
  UINT f_rs1; \
  UINT f_op3; \
  UINT f_rd; \
  UINT f_op; \
  unsigned int length;
#define EXTRACT_IFMT_LDSTUB_REG_REG_CODE \
  length = 4; \
  f_rs2 = EXTRACT_UINT (insn, 32, 4, 5); \
  f_res_asi = EXTRACT_INT (insn, 32, 12, 8); \
  f_i = EXTRACT_UINT (insn, 32, 13, 1); \
  f_rs1 = EXTRACT_UINT (insn, 32, 18, 5); \
  f_op3 = EXTRACT_UINT (insn, 32, 24, 6); \
  f_rd = EXTRACT_UINT (insn, 32, 29, 5); \
  f_op = EXTRACT_UINT (insn, 32, 31, 2); \

#define EXTRACT_IFMT_LDSTUB_REG_IMM_VARS \
  /* Instruction fields.  */ \
  INT f_simm13; \
  UINT f_i; \
  UINT f_rs1; \
  UINT f_op3; \
  UINT f_rd; \
  UINT f_op; \
  unsigned int length;
#define EXTRACT_IFMT_LDSTUB_REG_IMM_CODE \
  length = 4; \
  f_simm13 = EXTRACT_INT (insn, 32, 12, 13); \
  f_i = EXTRACT_UINT (insn, 32, 13, 1); \
  f_rs1 = EXTRACT_UINT (insn, 32, 18, 5); \
  f_op3 = EXTRACT_UINT (insn, 32, 24, 6); \
  f_rd = EXTRACT_UINT (insn, 32, 29, 5); \
  f_op = EXTRACT_UINT (insn, 32, 31, 2); \

#define EXTRACT_IFMT_LDSTUB_REG_REG_ASI_VARS \
  /* Instruction fields.  */ \
  UINT f_rs2; \
  UINT f_asi; \
  UINT f_i; \
  UINT f_rs1; \
  UINT f_op3; \
  UINT f_rd; \
  UINT f_op; \
  unsigned int length;
#define EXTRACT_IFMT_LDSTUB_REG_REG_ASI_CODE \
  length = 4; \
  f_rs2 = EXTRACT_UINT (insn, 32, 4, 5); \
  f_asi = EXTRACT_UINT (insn, 32, 12, 8); \
  f_i = EXTRACT_UINT (insn, 32, 13, 1); \
  f_rs1 = EXTRACT_UINT (insn, 32, 18, 5); \
  f_op3 = EXTRACT_UINT (insn, 32, 24, 6); \
  f_rd = EXTRACT_UINT (insn, 32, 29, 5); \
  f_op = EXTRACT_UINT (insn, 32, 31, 2); \

#define EXTRACT_IFMT_LDD_REG_REG_VARS \
  /* Instruction fields.  */ \
  UINT f_rs2; \
  INT f_res_asi; \
  UINT f_i; \
  UINT f_rs1; \
  UINT f_op3; \
  UINT f_rd; \
  UINT f_op; \
  unsigned int length;
#define EXTRACT_IFMT_LDD_REG_REG_CODE \
  length = 4; \
  f_rs2 = EXTRACT_UINT (insn, 32, 4, 5); \
  f_res_asi = EXTRACT_INT (insn, 32, 12, 8); \
  f_i = EXTRACT_UINT (insn, 32, 13, 1); \
  f_rs1 = EXTRACT_UINT (insn, 32, 18, 5); \
  f_op3 = EXTRACT_UINT (insn, 32, 24, 6); \
  f_rd = EXTRACT_UINT (insn, 32, 29, 5); \
  f_op = EXTRACT_UINT (insn, 32, 31, 2); \

#define EXTRACT_IFMT_LDD_REG_IMM_VARS \
  /* Instruction fields.  */ \
  INT f_simm13; \
  UINT f_i; \
  UINT f_rs1; \
  UINT f_op3; \
  UINT f_rd; \
  UINT f_op; \
  unsigned int length;
#define EXTRACT_IFMT_LDD_REG_IMM_CODE \
  length = 4; \
  f_simm13 = EXTRACT_INT (insn, 32, 12, 13); \
  f_i = EXTRACT_UINT (insn, 32, 13, 1); \
  f_rs1 = EXTRACT_UINT (insn, 32, 18, 5); \
  f_op3 = EXTRACT_UINT (insn, 32, 24, 6); \
  f_rd = EXTRACT_UINT (insn, 32, 29, 5); \
  f_op = EXTRACT_UINT (insn, 32, 31, 2); \

#define EXTRACT_IFMT_LDD_REG_REG_ASI_VARS \
  /* Instruction fields.  */ \
  UINT f_rs2; \
  UINT f_asi; \
  UINT f_i; \
  UINT f_rs1; \
  UINT f_op3; \
  UINT f_rd; \
  UINT f_op; \
  unsigned int length;
#define EXTRACT_IFMT_LDD_REG_REG_ASI_CODE \
  length = 4; \
  f_rs2 = EXTRACT_UINT (insn, 32, 4, 5); \
  f_asi = EXTRACT_UINT (insn, 32, 12, 8); \
  f_i = EXTRACT_UINT (insn, 32, 13, 1); \
  f_rs1 = EXTRACT_UINT (insn, 32, 18, 5); \
  f_op3 = EXTRACT_UINT (insn, 32, 24, 6); \
  f_rd = EXTRACT_UINT (insn, 32, 29, 5); \
  f_op = EXTRACT_UINT (insn, 32, 31, 2); \

#define EXTRACT_IFMT_FP_LD_REG_REG_VARS \
  /* Instruction fields.  */ \
  UINT f_rs2; \
  INT f_res_asi; \
  UINT f_i; \
  UINT f_rs1; \
  UINT f_op3; \
  UINT f_rd; \
  UINT f_op; \
  unsigned int length;
#define EXTRACT_IFMT_FP_LD_REG_REG_CODE \
  length = 4; \
  f_rs2 = EXTRACT_UINT (insn, 32, 4, 5); \
  f_res_asi = EXTRACT_INT (insn, 32, 12, 8); \
  f_i = EXTRACT_UINT (insn, 32, 13, 1); \
  f_rs1 = EXTRACT_UINT (insn, 32, 18, 5); \
  f_op3 = EXTRACT_UINT (insn, 32, 24, 6); \
  f_rd = EXTRACT_UINT (insn, 32, 29, 5); \
  f_op = EXTRACT_UINT (insn, 32, 31, 2); \

#define EXTRACT_IFMT_FP_LD_REG_IMM_VARS \
  /* Instruction fields.  */ \
  INT f_simm13; \
  UINT f_i; \
  UINT f_rs1; \
  UINT f_op3; \
  UINT f_rd; \
  UINT f_op; \
  unsigned int length;
#define EXTRACT_IFMT_FP_LD_REG_IMM_CODE \
  length = 4; \
  f_simm13 = EXTRACT_INT (insn, 32, 12, 13); \
  f_i = EXTRACT_UINT (insn, 32, 13, 1); \
  f_rs1 = EXTRACT_UINT (insn, 32, 18, 5); \
  f_op3 = EXTRACT_UINT (insn, 32, 24, 6); \
  f_rd = EXTRACT_UINT (insn, 32, 29, 5); \
  f_op = EXTRACT_UINT (insn, 32, 31, 2); \

#define EXTRACT_IFMT_FP_LD_REG_REG_ASI_VARS \
  /* Instruction fields.  */ \
  UINT f_rs2; \
  UINT f_asi; \
  UINT f_i; \
  UINT f_rs1; \
  UINT f_op3; \
  UINT f_rd; \
  UINT f_op; \
  unsigned int length;
#define EXTRACT_IFMT_FP_LD_REG_REG_ASI_CODE \
  length = 4; \
  f_rs2 = EXTRACT_UINT (insn, 32, 4, 5); \
  f_asi = EXTRACT_UINT (insn, 32, 12, 8); \
  f_i = EXTRACT_UINT (insn, 32, 13, 1); \
  f_rs1 = EXTRACT_UINT (insn, 32, 18, 5); \
  f_op3 = EXTRACT_UINT (insn, 32, 24, 6); \
  f_rd = EXTRACT_UINT (insn, 32, 29, 5); \
  f_op = EXTRACT_UINT (insn, 32, 31, 2); \

#define EXTRACT_IFMT_SETHI_VARS \
  /* Instruction fields.  */ \
  INT f_hi22; \
  UINT f_op2; \
  UINT f_rd; \
  UINT f_op; \
  unsigned int length;
#define EXTRACT_IFMT_SETHI_CODE \
  length = 4; \
  f_hi22 = EXTRACT_INT (insn, 32, 21, 22); \
  f_op2 = EXTRACT_UINT (insn, 32, 24, 3); \
  f_rd = EXTRACT_UINT (insn, 32, 29, 5); \
  f_op = EXTRACT_UINT (insn, 32, 31, 2); \

#define EXTRACT_IFMT_UNIMP_VARS \
  /* Instruction fields.  */ \
  INT f_imm22; \
  UINT f_op2; \
  UINT f_rd_res; \
  UINT f_op; \
  unsigned int length;
#define EXTRACT_IFMT_UNIMP_CODE \
  length = 4; \
  f_imm22 = EXTRACT_INT (insn, 32, 21, 22); \
  f_op2 = EXTRACT_UINT (insn, 32, 24, 3); \
  f_rd_res = EXTRACT_UINT (insn, 32, 29, 5); \
  f_op = EXTRACT_UINT (insn, 32, 31, 2); \

#define EXTRACT_IFMT_CALL_VARS \
  /* Instruction fields.  */ \
  SI f_disp30; \
  UINT f_op; \
  unsigned int length;
#define EXTRACT_IFMT_CALL_CODE \
  length = 4; \
  f_disp30 = ((((EXTRACT_INT (insn, 32, 29, 30)) << (2))) + (pc)); \
  f_op = EXTRACT_UINT (insn, 32, 31, 2); \

#define EXTRACT_IFMT_BA_VARS \
  /* Instruction fields.  */ \
  SI f_disp22; \
  UINT f_op2; \
  UINT f_fmt2_cond; \
  UINT f_a; \
  UINT f_op; \
  unsigned int length;
#define EXTRACT_IFMT_BA_CODE \
  length = 4; \
  f_disp22 = ((((EXTRACT_INT (insn, 32, 21, 22)) << (2))) + (pc)); \
  f_op2 = EXTRACT_UINT (insn, 32, 24, 3); \
  f_fmt2_cond = EXTRACT_UINT (insn, 32, 28, 4); \
  f_a = EXTRACT_UINT (insn, 32, 29, 1); \
  f_op = EXTRACT_UINT (insn, 32, 31, 2); \

#define EXTRACT_IFMT_TA_VARS \
  /* Instruction fields.  */ \
  UINT f_rs2; \
  INT f_res_asi; \
  UINT f_i; \
  UINT f_rs1; \
  UINT f_op3; \
  UINT f_fmt2_cond; \
  UINT f_a; \
  UINT f_op; \
  unsigned int length;
#define EXTRACT_IFMT_TA_CODE \
  length = 4; \
  f_rs2 = EXTRACT_UINT (insn, 32, 4, 5); \
  f_res_asi = EXTRACT_INT (insn, 32, 12, 8); \
  f_i = EXTRACT_UINT (insn, 32, 13, 1); \
  f_rs1 = EXTRACT_UINT (insn, 32, 18, 5); \
  f_op3 = EXTRACT_UINT (insn, 32, 24, 6); \
  f_fmt2_cond = EXTRACT_UINT (insn, 32, 28, 4); \
  f_a = EXTRACT_UINT (insn, 32, 29, 1); \
  f_op = EXTRACT_UINT (insn, 32, 31, 2); \

#define EXTRACT_IFMT_TA_IMM_VARS \
  /* Instruction fields.  */ \
  INT f_simm13; \
  UINT f_i; \
  UINT f_rs1; \
  UINT f_op3; \
  UINT f_fmt2_cond; \
  UINT f_a; \
  UINT f_op; \
  unsigned int length;
#define EXTRACT_IFMT_TA_IMM_CODE \
  length = 4; \
  f_simm13 = EXTRACT_INT (insn, 32, 12, 13); \
  f_i = EXTRACT_UINT (insn, 32, 13, 1); \
  f_rs1 = EXTRACT_UINT (insn, 32, 18, 5); \
  f_op3 = EXTRACT_UINT (insn, 32, 24, 6); \
  f_fmt2_cond = EXTRACT_UINT (insn, 32, 28, 4); \
  f_a = EXTRACT_UINT (insn, 32, 29, 1); \
  f_op = EXTRACT_UINT (insn, 32, 31, 2); \

/* Collection of various things for the trace handler to use.  */

typedef struct trace_record {
  IADDR pc;
  /* FIXME:wip */
} TRACE_RECORD;

#endif /* CPU_SPARC32_H */
