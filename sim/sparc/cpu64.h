/* CPU family header for sparc64.

THIS FILE IS MACHINE GENERATED WITH CGEN.

Copyright (C) 1999 Cygnus Solutions, Inc.

This file is part of the Cygnus Simulators.


*/

#ifndef CPU_SPARC64_H
#define CPU_SPARC64_H

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
  /* version */
  UDI h_ver;
#define GET_H_VER() CPU (h_ver)
#define SET_H_VER(x) (CPU (h_ver) = (x))
  /* processor state */
  UDI h_pstate;
#define GET_H_PSTATE() CPU (h_pstate)
#define SET_H_PSTATE(x) (CPU (h_pstate) = (x))
  /* trap base address */
  UDI h_tba;
#define GET_H_TBA() CPU (h_tba)
#define SET_H_TBA(x) (CPU (h_tba) = (x))
  /* trap type */
  UDI h_tt;
#define GET_H_TT() CPU (h_tt)
#define SET_H_TT(x) (CPU (h_tt) = (x))
  /* trap pc */
  UDI h_tpc;
#define GET_H_TPC() CPU (h_tpc)
#define SET_H_TPC(x) (CPU (h_tpc) = (x))
  /* trap npc */
  UDI h_tnpc;
#define GET_H_TNPC() CPU (h_tnpc)
#define SET_H_TNPC(x) (CPU (h_tnpc) = (x))
  /* trap state */
  UDI h_tstate;
#define GET_H_TSTATE() CPU (h_tstate)
#define SET_H_TSTATE(x) (CPU (h_tstate) = (x))
  /* trap level */
  UQI h_tl;
#define GET_H_TL() CPU (h_tl)
#define SET_H_TL(x) (CPU (h_tl) = (x))
  /* address space identifier */
  UQI h_asi;
#define GET_H_ASI() CPU (h_asi)
#define SET_H_ASI(x) (CPU (h_asi) = (x))
  /* tick counter */
  UDI h_tick;
#define GET_H_TICK() CPU (h_tick)
#define SET_H_TICK(x) (CPU (h_tick) = (x))
  /* savable window registers */
  UDI h_cansave;
#define GET_H_CANSAVE() CPU (h_cansave)
#define SET_H_CANSAVE(x) (CPU (h_cansave) = (x))
  /* restorable window registers */
  UDI h_canrestore;
#define GET_H_CANRESTORE() CPU (h_canrestore)
#define SET_H_CANRESTORE(x) (CPU (h_canrestore) = (x))
  /* other window registers */
  UDI h_otherwin;
#define GET_H_OTHERWIN() CPU (h_otherwin)
#define SET_H_OTHERWIN(x) (CPU (h_otherwin) = (x))
  /* clean window registers */
  UDI h_cleanwin;
#define GET_H_CLEANWIN() CPU (h_cleanwin)
#define SET_H_CLEANWIN(x) (CPU (h_cleanwin) = (x))
  /* window state */
  UDI h_wstate;
#define GET_H_WSTATE() CPU (h_wstate)
#define SET_H_WSTATE(x) (CPU (h_wstate) = (x))
  /*  */
  UQI h_fcc0;
#define GET_H_FCC0() CPU (h_fcc0)
#define SET_H_FCC0(x) (CPU (h_fcc0) = (x))
  /*  */
  UQI h_fcc1;
#define GET_H_FCC1() CPU (h_fcc1)
#define SET_H_FCC1(x) (CPU (h_fcc1) = (x))
  /*  */
  UQI h_fcc2;
#define GET_H_FCC2() CPU (h_fcc2)
#define SET_H_FCC2(x) (CPU (h_fcc2) = (x))
  /*  */
  UQI h_fcc3;
#define GET_H_FCC3() CPU (h_fcc3)
#define SET_H_FCC3(x) (CPU (h_fcc3) = (x))
  /* fsr rounding direction */
  UQI h_fsr_rd;
#define GET_H_FSR_RD() CPU (h_fsr_rd)
#define SET_H_FSR_RD(x) (CPU (h_fsr_rd) = (x))
  /* fsr trap enable mask */
  UQI h_fsr_tem;
#define GET_H_FSR_TEM() CPU (h_fsr_tem)
#define SET_H_FSR_TEM(x) (CPU (h_fsr_tem) = (x))
  /* fsr nonstandard fp */
  BI h_fsr_ns;
#define GET_H_FSR_NS() CPU (h_fsr_ns)
#define SET_H_FSR_NS(x) (CPU (h_fsr_ns) = (x))
  /* fsr version */
  UQI h_fsr_ver;
#define GET_H_FSR_VER() CPU (h_fsr_ver)
#define SET_H_FSR_VER(x) (CPU (h_fsr_ver) = (x))
  /* fsr fp trap type */
  UQI h_fsr_ftt;
#define GET_H_FSR_FTT() CPU (h_fsr_ftt)
#define SET_H_FSR_FTT(x) (CPU (h_fsr_ftt) = (x))
  /* fsr queue not empty */
  BI h_fsr_qne;
#define GET_H_FSR_QNE() CPU (h_fsr_qne)
#define SET_H_FSR_QNE(x) (CPU (h_fsr_qne) = (x))
  /* fsr accrued exception */
  UQI h_fsr_aexc;
#define GET_H_FSR_AEXC() CPU (h_fsr_aexc)
#define SET_H_FSR_AEXC(x) (CPU (h_fsr_aexc) = (x))
  /* fsr current exception */
  UQI h_fsr_cexc;
#define GET_H_FSR_CEXC() CPU (h_fsr_cexc)
#define SET_H_FSR_CEXC(x) (CPU (h_fsr_cexc) = (x))
  /* fpsr enable fp */
  BI h_fpsr_fef;
#define GET_H_FPSR_FEF() CPU (h_fpsr_fef)
#define SET_H_FPSR_FEF(x) (CPU (h_fpsr_fef) = (x))
  /* fpsr dirty upper */
  BI h_fpsr_du;
#define GET_H_FPSR_DU() CPU (h_fpsr_du)
#define SET_H_FPSR_DU(x) (CPU (h_fpsr_du) = (x))
  /* fpsr dirty lower */
  BI h_fpsr_dl;
#define GET_H_FPSR_DL() CPU (h_fpsr_dl)
#define SET_H_FPSR_DL(x) (CPU (h_fpsr_dl) = (x))
/* GET_H_FPSR macro user-written */
/* SET_H_FPSR macro user-written */
  } hardware;
#define CPU_CGEN_HW(cpu) (& (cpu)->cpu_data.hardware)
} SPARC64_CPU_DATA;

/* Cover fns for register access.  */
USI sparc64_h_pc_get (SIM_CPU *);
void sparc64_h_pc_set (SIM_CPU *, USI);
SI sparc64_h_npc_get (SIM_CPU *);
void sparc64_h_npc_set (SIM_CPU *, SI);
SI sparc64_h_gr_get (SIM_CPU *, UINT);
void sparc64_h_gr_set (SIM_CPU *, UINT, SI);
BI sparc64_h_icc_c_get (SIM_CPU *);
void sparc64_h_icc_c_set (SIM_CPU *, BI);
BI sparc64_h_icc_n_get (SIM_CPU *);
void sparc64_h_icc_n_set (SIM_CPU *, BI);
BI sparc64_h_icc_v_get (SIM_CPU *);
void sparc64_h_icc_v_set (SIM_CPU *, BI);
BI sparc64_h_icc_z_get (SIM_CPU *);
void sparc64_h_icc_z_set (SIM_CPU *, BI);
BI sparc64_h_xcc_c_get (SIM_CPU *);
void sparc64_h_xcc_c_set (SIM_CPU *, BI);
BI sparc64_h_xcc_n_get (SIM_CPU *);
void sparc64_h_xcc_n_set (SIM_CPU *, BI);
BI sparc64_h_xcc_v_get (SIM_CPU *);
void sparc64_h_xcc_v_set (SIM_CPU *, BI);
BI sparc64_h_xcc_z_get (SIM_CPU *);
void sparc64_h_xcc_z_set (SIM_CPU *, BI);
SI sparc64_h_y_get (SIM_CPU *);
void sparc64_h_y_set (SIM_CPU *, SI);
SI sparc64_h_asr_get (SIM_CPU *, UINT);
void sparc64_h_asr_set (SIM_CPU *, UINT, SI);
BI sparc64_h_annul_p_get (SIM_CPU *);
void sparc64_h_annul_p_set (SIM_CPU *, BI);
SF sparc64_h_fr_get (SIM_CPU *, UINT);
void sparc64_h_fr_set (SIM_CPU *, UINT, SF);
UDI sparc64_h_ver_get (SIM_CPU *);
void sparc64_h_ver_set (SIM_CPU *, UDI);
UDI sparc64_h_pstate_get (SIM_CPU *);
void sparc64_h_pstate_set (SIM_CPU *, UDI);
UDI sparc64_h_tba_get (SIM_CPU *);
void sparc64_h_tba_set (SIM_CPU *, UDI);
UDI sparc64_h_tt_get (SIM_CPU *);
void sparc64_h_tt_set (SIM_CPU *, UDI);
UDI sparc64_h_tpc_get (SIM_CPU *);
void sparc64_h_tpc_set (SIM_CPU *, UDI);
UDI sparc64_h_tnpc_get (SIM_CPU *);
void sparc64_h_tnpc_set (SIM_CPU *, UDI);
UDI sparc64_h_tstate_get (SIM_CPU *);
void sparc64_h_tstate_set (SIM_CPU *, UDI);
UQI sparc64_h_tl_get (SIM_CPU *);
void sparc64_h_tl_set (SIM_CPU *, UQI);
UQI sparc64_h_asi_get (SIM_CPU *);
void sparc64_h_asi_set (SIM_CPU *, UQI);
UDI sparc64_h_tick_get (SIM_CPU *);
void sparc64_h_tick_set (SIM_CPU *, UDI);
UDI sparc64_h_cansave_get (SIM_CPU *);
void sparc64_h_cansave_set (SIM_CPU *, UDI);
UDI sparc64_h_canrestore_get (SIM_CPU *);
void sparc64_h_canrestore_set (SIM_CPU *, UDI);
UDI sparc64_h_otherwin_get (SIM_CPU *);
void sparc64_h_otherwin_set (SIM_CPU *, UDI);
UDI sparc64_h_cleanwin_get (SIM_CPU *);
void sparc64_h_cleanwin_set (SIM_CPU *, UDI);
UDI sparc64_h_wstate_get (SIM_CPU *);
void sparc64_h_wstate_set (SIM_CPU *, UDI);
UQI sparc64_h_fcc0_get (SIM_CPU *);
void sparc64_h_fcc0_set (SIM_CPU *, UQI);
UQI sparc64_h_fcc1_get (SIM_CPU *);
void sparc64_h_fcc1_set (SIM_CPU *, UQI);
UQI sparc64_h_fcc2_get (SIM_CPU *);
void sparc64_h_fcc2_set (SIM_CPU *, UQI);
UQI sparc64_h_fcc3_get (SIM_CPU *);
void sparc64_h_fcc3_set (SIM_CPU *, UQI);
UQI sparc64_h_fsr_rd_get (SIM_CPU *);
void sparc64_h_fsr_rd_set (SIM_CPU *, UQI);
UQI sparc64_h_fsr_tem_get (SIM_CPU *);
void sparc64_h_fsr_tem_set (SIM_CPU *, UQI);
BI sparc64_h_fsr_ns_get (SIM_CPU *);
void sparc64_h_fsr_ns_set (SIM_CPU *, BI);
UQI sparc64_h_fsr_ver_get (SIM_CPU *);
void sparc64_h_fsr_ver_set (SIM_CPU *, UQI);
UQI sparc64_h_fsr_ftt_get (SIM_CPU *);
void sparc64_h_fsr_ftt_set (SIM_CPU *, UQI);
BI sparc64_h_fsr_qne_get (SIM_CPU *);
void sparc64_h_fsr_qne_set (SIM_CPU *, BI);
UQI sparc64_h_fsr_aexc_get (SIM_CPU *);
void sparc64_h_fsr_aexc_set (SIM_CPU *, UQI);
UQI sparc64_h_fsr_cexc_get (SIM_CPU *);
void sparc64_h_fsr_cexc_set (SIM_CPU *, UQI);
BI sparc64_h_fpsr_fef_get (SIM_CPU *);
void sparc64_h_fpsr_fef_set (SIM_CPU *, BI);
BI sparc64_h_fpsr_du_get (SIM_CPU *);
void sparc64_h_fpsr_du_set (SIM_CPU *, BI);
BI sparc64_h_fpsr_dl_get (SIM_CPU *);
void sparc64_h_fpsr_dl_set (SIM_CPU *, BI);
UQI sparc64_h_fpsr_get (SIM_CPU *);
void sparc64_h_fpsr_set (SIM_CPU *, UQI);

/* These must be hand-written.  */
extern CPUREG_FETCH_FN sparc64_fetch_register;
extern CPUREG_STORE_FN sparc64_store_register;

typedef struct {
  int empty;
} MODEL_SPARC64_DEF_DATA;

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

#define EXTRACT_IFMT_BEQZ_VARS \
  /* Instruction fields.  */ \
  INT f_disp16; \
  UINT f_disp16_hi; \
  UINT f_disp16_lo; \
  UINT f_rs1; \
  UINT f_p; \
  UINT f_op2; \
  UINT f_fmt2_rcond; \
  INT f_bpr_res28_1; \
  UINT f_a; \
  UINT f_op; \
  unsigned int length;
#define EXTRACT_IFMT_BEQZ_CODE \
  length = 4; \
  f_disp16_hi = EXTRACT_UINT (insn, 32, 10, 2); \
  f_disp16_lo = EXTRACT_UINT (insn, 32, 18, 14); \
do {\
  f_disp16 = ((((f_disp16_hi) << (14))) | (f_disp16_low));\
} while (0);\
  f_rs1 = EXTRACT_UINT (insn, 32, 18, 5); \
  f_p = EXTRACT_UINT (insn, 32, 19, 1); \
  f_op2 = EXTRACT_UINT (insn, 32, 24, 3); \
  f_fmt2_rcond = EXTRACT_UINT (insn, 32, 27, 3); \
  f_bpr_res28_1 = EXTRACT_INT (insn, 32, 28, 1); \
  f_a = EXTRACT_UINT (insn, 32, 29, 1); \
  f_op = EXTRACT_UINT (insn, 32, 31, 2); \

#define EXTRACT_IFMT_BPCC_BA_VARS \
  /* Instruction fields.  */ \
  INT f_disp19; \
  UINT f_p; \
  UINT f_fmt2_cc0; \
  UINT f_fmt2_cc1; \
  UINT f_op2; \
  UINT f_fmt2_cond; \
  UINT f_a; \
  UINT f_op; \
  unsigned int length;
#define EXTRACT_IFMT_BPCC_BA_CODE \
  length = 4; \
  f_disp19 = EXTRACT_INT (insn, 32, 13, 19); \
  f_p = EXTRACT_UINT (insn, 32, 19, 1); \
  f_fmt2_cc0 = EXTRACT_UINT (insn, 32, 20, 1); \
  f_fmt2_cc1 = EXTRACT_UINT (insn, 32, 21, 1); \
  f_op2 = EXTRACT_UINT (insn, 32, 24, 3); \
  f_fmt2_cond = EXTRACT_UINT (insn, 32, 28, 4); \
  f_a = EXTRACT_UINT (insn, 32, 29, 1); \
  f_op = EXTRACT_UINT (insn, 32, 31, 2); \

#define EXTRACT_IFMT_DONE_VARS \
  /* Instruction fields.  */ \
  INT f_res_18_19; \
  UINT f_op3; \
  UINT f_fcn; \
  UINT f_op; \
  unsigned int length;
#define EXTRACT_IFMT_DONE_CODE \
  length = 4; \
  f_res_18_19 = EXTRACT_INT (insn, 32, 18, 19); \
  f_op3 = EXTRACT_UINT (insn, 32, 24, 6); \
  f_fcn = EXTRACT_UINT (insn, 32, 29, 5); \
  f_op = EXTRACT_UINT (insn, 32, 31, 2); \

#define EXTRACT_IFMT_FLUSH_VARS \
  /* Instruction fields.  */ \
  UINT f_rs2; \
  INT f_res_asi; \
  UINT f_i; \
  UINT f_rs1; \
  UINT f_op3; \
  UINT f_rd; \
  UINT f_op; \
  unsigned int length;
#define EXTRACT_IFMT_FLUSH_CODE \
  length = 4; \
  f_rs2 = EXTRACT_UINT (insn, 32, 4, 5); \
  f_res_asi = EXTRACT_INT (insn, 32, 12, 8); \
  f_i = EXTRACT_UINT (insn, 32, 13, 1); \
  f_rs1 = EXTRACT_UINT (insn, 32, 18, 5); \
  f_op3 = EXTRACT_UINT (insn, 32, 24, 6); \
  f_rd = EXTRACT_UINT (insn, 32, 29, 5); \
  f_op = EXTRACT_UINT (insn, 32, 31, 2); \

#define EXTRACT_IFMT_FLUSH_IMM_VARS \
  /* Instruction fields.  */ \
  INT f_simm13; \
  UINT f_i; \
  UINT f_rs1; \
  UINT f_op3; \
  UINT f_rd; \
  UINT f_op; \
  unsigned int length;
#define EXTRACT_IFMT_FLUSH_IMM_CODE \
  length = 4; \
  f_simm13 = EXTRACT_INT (insn, 32, 12, 13); \
  f_i = EXTRACT_UINT (insn, 32, 13, 1); \
  f_rs1 = EXTRACT_UINT (insn, 32, 18, 5); \
  f_op3 = EXTRACT_UINT (insn, 32, 24, 6); \
  f_rd = EXTRACT_UINT (insn, 32, 29, 5); \
  f_op = EXTRACT_UINT (insn, 32, 31, 2); \

#define EXTRACT_IFMT_FLUSHW_VARS \
  /* Instruction fields.  */ \
  INT f_simm13; \
  UINT f_i; \
  UINT f_rs1; \
  UINT f_op3; \
  UINT f_rd; \
  UINT f_op; \
  unsigned int length;
#define EXTRACT_IFMT_FLUSHW_CODE \
  length = 4; \
  f_simm13 = EXTRACT_INT (insn, 32, 12, 13); \
  f_i = EXTRACT_UINT (insn, 32, 13, 1); \
  f_rs1 = EXTRACT_UINT (insn, 32, 18, 5); \
  f_op3 = EXTRACT_UINT (insn, 32, 24, 6); \
  f_rd = EXTRACT_UINT (insn, 32, 29, 5); \
  f_op = EXTRACT_UINT (insn, 32, 31, 2); \

#define EXTRACT_IFMT_IMPDEP1_VARS \
  /* Instruction fields.  */ \
  INT f_impdep19; \
  UINT f_op3; \
  INT f_impdep5; \
  UINT f_op; \
  unsigned int length;
#define EXTRACT_IFMT_IMPDEP1_CODE \
  length = 4; \
  f_impdep19 = EXTRACT_INT (insn, 32, 18, 19); \
  f_op3 = EXTRACT_UINT (insn, 32, 24, 6); \
  f_impdep5 = EXTRACT_INT (insn, 32, 29, 5); \
  f_op = EXTRACT_UINT (insn, 32, 31, 2); \

#define EXTRACT_IFMT_MEMBAR_VARS \
  /* Instruction fields.  */ \
  UINT f_membarmask; \
  INT f_membar_res12_6; \
  UINT f_i; \
  UINT f_rs1; \
  UINT f_op3; \
  UINT f_rd; \
  UINT f_op; \
  unsigned int length;
#define EXTRACT_IFMT_MEMBAR_CODE \
  length = 4; \
  f_membarmask = EXTRACT_UINT (insn, 32, 6, 7); \
  f_membar_res12_6 = EXTRACT_INT (insn, 32, 12, 6); \
  f_i = EXTRACT_UINT (insn, 32, 13, 1); \
  f_rs1 = EXTRACT_UINT (insn, 32, 18, 5); \
  f_op3 = EXTRACT_UINT (insn, 32, 24, 6); \
  f_rd = EXTRACT_UINT (insn, 32, 29, 5); \
  f_op = EXTRACT_UINT (insn, 32, 31, 2); \

#define EXTRACT_IFMT_MOVA_ICC_ICC_VARS \
  /* Instruction fields.  */ \
  UINT f_rs2; \
  INT f_fmt4_res10_6; \
  UINT f_fmt4_cc1_0; \
  UINT f_i; \
  UINT f_fmt4_cc2; \
  UINT f_op3; \
  UINT f_fmt2_cond; \
  UINT f_rd; \
  UINT f_op; \
  unsigned int length;
#define EXTRACT_IFMT_MOVA_ICC_ICC_CODE \
  length = 4; \
  f_rs2 = EXTRACT_UINT (insn, 32, 4, 5); \
  f_fmt4_res10_6 = EXTRACT_INT (insn, 32, 10, 6); \
  f_fmt4_cc1_0 = EXTRACT_UINT (insn, 32, 12, 2); \
  f_i = EXTRACT_UINT (insn, 32, 13, 1); \
  f_fmt4_cc2 = EXTRACT_UINT (insn, 32, 18, 1); \
  f_op3 = EXTRACT_UINT (insn, 32, 24, 6); \
  f_fmt2_cond = EXTRACT_UINT (insn, 32, 28, 4); \
  f_rd = EXTRACT_UINT (insn, 32, 29, 5); \
  f_op = EXTRACT_UINT (insn, 32, 31, 2); \

#define EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS \
  /* Instruction fields.  */ \
  INT f_simm11; \
  UINT f_fmt4_cc1_0; \
  UINT f_i; \
  UINT f_fmt4_cc2; \
  UINT f_op3; \
  UINT f_fmt2_cond; \
  UINT f_rd; \
  UINT f_op; \
  unsigned int length;
#define EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE \
  length = 4; \
  f_simm11 = EXTRACT_INT (insn, 32, 10, 11); \
  f_fmt4_cc1_0 = EXTRACT_UINT (insn, 32, 12, 2); \
  f_i = EXTRACT_UINT (insn, 32, 13, 1); \
  f_fmt4_cc2 = EXTRACT_UINT (insn, 32, 18, 1); \
  f_op3 = EXTRACT_UINT (insn, 32, 24, 6); \
  f_fmt2_cond = EXTRACT_UINT (insn, 32, 28, 4); \
  f_rd = EXTRACT_UINT (insn, 32, 29, 5); \
  f_op = EXTRACT_UINT (insn, 32, 31, 2); \

#define EXTRACT_IFMT_LDSB_REG_REG_VARS \
  /* Instruction fields.  */ \
  UINT f_rs2; \
  INT f_res_asi; \
  UINT f_i; \
  UINT f_rs1; \
  UINT f_op3; \
  UINT f_rd; \
  UINT f_op; \
  unsigned int length;
#define EXTRACT_IFMT_LDSB_REG_REG_CODE \
  length = 4; \
  f_rs2 = EXTRACT_UINT (insn, 32, 4, 5); \
  f_res_asi = EXTRACT_INT (insn, 32, 12, 8); \
  f_i = EXTRACT_UINT (insn, 32, 13, 1); \
  f_rs1 = EXTRACT_UINT (insn, 32, 18, 5); \
  f_op3 = EXTRACT_UINT (insn, 32, 24, 6); \
  f_rd = EXTRACT_UINT (insn, 32, 29, 5); \
  f_op = EXTRACT_UINT (insn, 32, 31, 2); \

#define EXTRACT_IFMT_LDSB_REG_IMM_VARS \
  /* Instruction fields.  */ \
  INT f_simm13; \
  UINT f_i; \
  UINT f_rs1; \
  UINT f_op3; \
  UINT f_rd; \
  UINT f_op; \
  unsigned int length;
#define EXTRACT_IFMT_LDSB_REG_IMM_CODE \
  length = 4; \
  f_simm13 = EXTRACT_INT (insn, 32, 12, 13); \
  f_i = EXTRACT_UINT (insn, 32, 13, 1); \
  f_rs1 = EXTRACT_UINT (insn, 32, 18, 5); \
  f_op3 = EXTRACT_UINT (insn, 32, 24, 6); \
  f_rd = EXTRACT_UINT (insn, 32, 29, 5); \
  f_op = EXTRACT_UINT (insn, 32, 31, 2); \

#define EXTRACT_IFMT_LDSB_REG_REG_ASI_VARS \
  /* Instruction fields.  */ \
  UINT f_rs2; \
  UINT f_asi; \
  UINT f_i; \
  UINT f_rs1; \
  UINT f_op3; \
  UINT f_rd; \
  UINT f_op; \
  unsigned int length;
#define EXTRACT_IFMT_LDSB_REG_REG_ASI_CODE \
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

#endif /* CPU_SPARC64_H */
