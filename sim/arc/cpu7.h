/* CPU family header for arc700f.

THIS FILE IS MACHINE GENERATED WITH CGEN.

Copyright 1996-2007 Free Software Foundation, Inc.

This file is part of the GNU simulators.

   This file is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   It is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.

*/

#ifndef CPU_ARC700F_H
#define CPU_ARC700F_H

/* Maximum number of instructions that are fetched at a time.
   This is for LIW type instructions sets (e.g. m32r).  */
#define MAX_LIW_INSNS 1

/* Maximum number of instructions that can be executed in parallel.  */
#define MAX_PARALLEL_INSNS 1

/* CPU state information.  */
typedef struct {
  /* Hardware elements.  */
  struct {
  /* loop inhibit bit */
  BI h_lbit;
#define GET_H_LBIT() CPU (h_lbit)
#define SET_H_LBIT(x) (CPU (h_lbit) = (x))
  /* zerobit */
  BI h_zbit;
#define GET_H_ZBIT() CPU (h_zbit)
#define SET_H_ZBIT(x) (CPU (h_zbit) = (x))
  /* negative bit */
  BI h_nbit;
#define GET_H_NBIT() CPU (h_nbit)
#define SET_H_NBIT(x) (CPU (h_nbit) = (x))
  /* carry bit */
  BI h_cbit;
#define GET_H_CBIT() CPU (h_cbit)
#define SET_H_CBIT(x) (CPU (h_cbit) = (x))
  /* overflow bit */
  BI h_vbit;
#define GET_H_VBIT() CPU (h_vbit)
#define SET_H_VBIT(x) (CPU (h_vbit) = (x))
  /* user mode bit */
  BI h_ubit;
#define GET_H_UBIT() CPU (h_ubit)
#define SET_H_UBIT(x) (CPU (h_ubit) = (x))
  /* interupt 1 enable bit */
  BI h_e1;
#define GET_H_E1() CPU (h_e1)
#define SET_H_E1(x) (CPU (h_e1) = (x))
  /* interupt 2 enable bit */
  BI h_e2;
#define GET_H_E2() CPU (h_e2)
#define SET_H_E2(x) (CPU (h_e2) = (x))
  /* channel 1 saturate */
  BI h_s1bit;
#define GET_H_S1BIT() CPU (h_s1bit)
#define SET_H_S1BIT(x) (CPU (h_s1bit) = (x))
  /* channel 2 saturate */
  BI h_s2bit;
#define GET_H_S2BIT() CPU (h_s2bit)
#define SET_H_S2BIT(x) (CPU (h_s2bit) = (x))
  /* core registers */
  SI h_cr[64];
#define GET_H_CR(index) (index == 61) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (CPU (h_cr[index]))
#define SET_H_CR(index, x) \
do { \
  switch ((index))\
  {\
  case 62 : {\
((void) 0); /*nop*/\
}\
    break;\
  case 61 :   case 63 : {\
cgen_rtx_error (current_cpu, "invalid insn");\
}\
    break;\
  default : {\
CPU (h_cr[(index)]) = (x);\
}\
    break;\
  }\
;} while (0)
  /* auxiliary registers */
  SI h_auxr[64];
#define GET_H_AUXR(index) (index == 0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (index == 6) ? (ADDSI (CPU (h_pc), 4)) : (index == 10) ? (ORSI (SLLSI (ZEXTBISI (CPU (h_lbit)), 12), ORSI (SLLSI (ZEXTBISI (CPU (h_zbit)), 11), ORSI (SLLSI (ZEXTBISI (CPU (h_nbit)), 10), ORSI (SLLSI (ZEXTBISI (CPU (h_cbit)), 9), ORSI (SLLSI (ZEXTBISI (CPU (h_vbit)), 8), ORSI (SLLSI (ZEXTBISI (CPU (h_e1)), 1), SLLSI (ZEXTBISI (CPU (h_e2)), 2)))))))) : (index == 33) ? (ADDSI (CPU_INSN_COUNT (current_cpu), SUBSI (CPU (h_auxr[((UINT) 35)]), CPU (h_timer_expire[((UINT) 0)])))) : (index == 65) ? (ORSI (SLLSI (ZEXTBISI (CPU (h_s1bit)), 9), SLLSI (ZEXTBISI (CPU (h_s2bit)), 4))) : (CPU (h_auxr[index]))
#define SET_H_AUXR(index, x) \
do { \
  switch ((index))\
  {\
  case 0 : {\
cgen_rtx_error (current_cpu, "invalid insn");\
}\
    break;\
  case 3 : {\
{\
CPU (h_auxr[((UINT) 3)]) = (x);\
scache_flush_cpu (current_cpu);\
}\
}\
    break;\
  case 4 :   case 5 :   case 6 :   case 10 :   case 1027 :   case 1040 :   case 1041 :   case 1046 : {\
((void) 0); /*nop*/\
}\
    break;\
  case 33 :   case 34 :   case 35 : {\
{\
CPU (h_auxr[(index)]) = (x);\
CPU (h_timer_expire[((UINT) 0)]) = ADDSI (CPU_INSN_COUNT (current_cpu), SUBSI (CPU (h_auxr[((UINT) 35)]), CPU (h_auxr[((UINT) 33)])));\
}\
}\
    break;\
  case 65 : {\
if (ANDSI ((x), 2)) {\
CPU (h_s1bit) = 0;\
CPU (h_s2bit) = 0;\
}\
}\
    break;\
  default : {\
CPU (h_auxr[(index)]) = (x);\
}\
    break;\
  }\
;} while (0)
  /* used internally in simulator to speed up timer expiration check */
  SI h_timer_expire[1];
#define GET_H_TIMER_EXPIRE(a1) CPU (h_timer_expire)[a1]
#define SET_H_TIMER_EXPIRE(a1, x) (CPU (h_timer_expire)[a1] = (x))
  /* offset to profile counters */
  SI h_prof_offset[1];
#define GET_H_PROF_OFFSET(a1) CPU (h_prof_offset)[a1]
#define SET_H_PROF_OFFSET(a1, x) (CPU (h_prof_offset)[a1] = (x))
  /* program counter */
  USI h_pc;
#define GET_H_PC() CPU (h_pc)
#define SET_H_PC(x) (CPU (h_pc) = (x))
  } hardware;
#define CPU_CGEN_HW(cpu) (& (cpu)->cpu_data.hardware)
} ARC700F_CPU_DATA;

/* Virtual regs.  */

#define GET_H_QCONDB(index) (index == COND_AL) ? (1) : (index == COND_EQ) ? (CPU (h_zbit)) : (index == COND_NE) ? (NOTBI (CPU (h_zbit))) : (index == COND_PL) ? (NOTBI (CPU (h_nbit))) : (index == COND_MI) ? (CPU (h_nbit)) : (index == COND_CS) ? (CPU (h_cbit)) : (index == COND_CC) ? (NOTBI (CPU (h_cbit))) : (index == COND_VS) ? (CPU (h_vbit)) : (index == COND_VC) ? (NOTBI (CPU (h_vbit))) : (index == COND_GT) ? (ANDBI (NOTBI (CPU (h_zbit)), EQBI (CPU (h_nbit), CPU (h_vbit)))) : (index == COND_GE) ? (EQBI (CPU (h_nbit), CPU (h_vbit))) : (index == COND_LT) ? (NEBI (CPU (h_nbit), CPU (h_vbit))) : (index == COND_LE) ? (ORBI (CPU (h_zbit), NEBI (CPU (h_nbit), CPU (h_vbit)))) : (index == COND_HI) ? (ANDBI (NOTBI (CPU (h_cbit)), NOTBI (CPU (h_zbit)))) : (index == COND_LS) ? (ORBI (CPU (h_cbit), CPU (h_zbit))) : (index == COND_PNZ) ? (ANDBI (NOTBI (CPU (h_nbit)), NOTBI (CPU (h_zbit)))) : (((cgen_rtx_error (current_cpu, "invalid insn"), 0), 1))
#define SET_H_QCONDB(index, x) \
do { \
((void) 0); /*nop*/\
;} while (0)
#define GET_H_QCONDJ(index) (index == COND_AL) ? (1) : (index == COND_EQ) ? (CPU (h_zbit)) : (index == COND_NE) ? (NOTBI (CPU (h_zbit))) : (index == COND_PL) ? (NOTBI (CPU (h_nbit))) : (index == COND_MI) ? (CPU (h_nbit)) : (index == COND_CS) ? (CPU (h_cbit)) : (index == COND_CC) ? (NOTBI (CPU (h_cbit))) : (index == COND_VS) ? (CPU (h_vbit)) : (index == COND_VC) ? (NOTBI (CPU (h_vbit))) : (index == COND_GT) ? (ANDBI (NOTBI (CPU (h_zbit)), EQBI (CPU (h_nbit), CPU (h_vbit)))) : (index == COND_GE) ? (EQBI (CPU (h_nbit), CPU (h_vbit))) : (index == COND_LT) ? (NEBI (CPU (h_nbit), CPU (h_vbit))) : (index == COND_LE) ? (ORBI (CPU (h_zbit), NEBI (CPU (h_nbit), CPU (h_vbit)))) : (index == COND_HI) ? (ANDBI (NOTBI (CPU (h_cbit)), NOTBI (CPU (h_zbit)))) : (index == COND_LS) ? (ORBI (CPU (h_cbit), CPU (h_zbit))) : (index == COND_PNZ) ? (ANDBI (NOTBI (CPU (h_nbit)), NOTBI (CPU (h_zbit)))) : (((cgen_rtx_error (current_cpu, "invalid insn"), 0), 1))
#define SET_H_QCONDJ(index, x) \
do { \
((void) 0); /*nop*/\
;} while (0)
#define GET_H_QCONDI(index) (index == COND_AL) ? (1) : (index == COND_EQ) ? (CPU (h_zbit)) : (index == COND_NE) ? (NOTBI (CPU (h_zbit))) : (index == COND_PL) ? (NOTBI (CPU (h_nbit))) : (index == COND_MI) ? (CPU (h_nbit)) : (index == COND_CS) ? (CPU (h_cbit)) : (index == COND_CC) ? (NOTBI (CPU (h_cbit))) : (index == COND_VS) ? (CPU (h_vbit)) : (index == COND_VC) ? (NOTBI (CPU (h_vbit))) : (index == COND_GT) ? (ANDBI (NOTBI (CPU (h_zbit)), EQBI (CPU (h_nbit), CPU (h_vbit)))) : (index == COND_GE) ? (EQBI (CPU (h_nbit), CPU (h_vbit))) : (index == COND_LT) ? (NEBI (CPU (h_nbit), CPU (h_vbit))) : (index == COND_LE) ? (ORBI (CPU (h_zbit), NEBI (CPU (h_nbit), CPU (h_vbit)))) : (index == COND_HI) ? (ANDBI (NOTBI (CPU (h_cbit)), NOTBI (CPU (h_zbit)))) : (index == COND_LS) ? (ORBI (CPU (h_cbit), CPU (h_zbit))) : (index == COND_PNZ) ? (ANDBI (NOTBI (CPU (h_nbit)), NOTBI (CPU (h_zbit)))) : (((cgen_rtx_error (current_cpu, "invalid insn"), 0), 1))
#define SET_H_QCONDI(index, x) \
do { \
((void) 0); /*nop*/\
;} while (0)
#define GET_H_I2COND(index) (index == 0) ? (1) : (index == 1) ? (CPU (h_zbit)) : (index == 2) ? (NOTBI (CPU (h_zbit))) : ((cgen_rtx_error (current_cpu, "unreachable - put in because of parser error"), 0))
#define SET_H_I2COND(index, x) \
do { \
((void) 0); /*nop*/\
;} while (0)
#define GET_H_I3COND(index) (index == COND3_CS) ? (CPU (h_cbit)) : (index == COND3_CC) ? (NOTBI (CPU (h_cbit))) : (index == COND3_GT) ? (ANDBI (NOTBI (CPU (h_zbit)), EQBI (CPU (h_nbit), CPU (h_vbit)))) : (index == COND3_GE) ? (EQBI (CPU (h_nbit), CPU (h_vbit))) : (index == COND3_LT) ? (NEBI (CPU (h_nbit), CPU (h_vbit))) : (index == COND3_LE) ? (ORBI (CPU (h_zbit), NEBI (CPU (h_nbit), CPU (h_vbit)))) : (index == COND3_HI) ? (ANDBI (NOTBI (CPU (h_cbit)), NOTBI (CPU (h_zbit)))) : (index == COND3_LS) ? (ORBI (CPU (h_cbit), CPU (h_zbit))) : ((cgen_rtx_error (current_cpu, "unreachable - put in because of parser error"), 0))
#define SET_H_I3COND(index, x) \
do { \
((void) 0); /*nop*/\
;} while (0)
#define GET_H_CR16(index) (index == 0 || index == 1 || index == 2 || index == 3) ? (CPU (h_cr[index])) : (CPU (h_cr[((index) + (8))]))
#define SET_H_CR16(index, x) \
do { \
  switch ((index))\
  {\
  case 0 :   case 1 :   case 2 :   case 3 : {\
CPU (h_cr[(index)]) = (x);\
}\
    break;\
  default : {\
CPU (h_cr[(((index)) + (8))]) = (x);\
}\
    break;\
  }\
;} while (0)
#define GET_H_R0(index) CPU (h_cr[((UINT) 0)])
#define SET_H_R0(index, x) \
do { \
CPU (h_cr[((UINT) 0)]) = (x);\
;} while (0)
#define GET_H_GP(index) CPU (h_cr[((UINT) 26)])
#define SET_H_GP(index, x) \
do { \
CPU (h_cr[((UINT) 26)]) = (x);\
;} while (0)
#define GET_H_SP(index) CPU (h_cr[((UINT) 28)])
#define SET_H_SP(index, x) \
do { \
CPU (h_cr[((UINT) 28)]) = (x);\
;} while (0)
#define GET_H_PCL(index) CPU (h_cr[((UINT) 63)])
#define SET_H_PCL(index, x) \
do { \
(cgen_rtx_error (current_cpu, "invalid insn"), 0);} while (0)
#define GET_H_NOILINK(index) CPU (h_cr[index])
#define SET_H_NOILINK(index, x) \
do { \
CPU (h_cr[(index)]) = (x);\
;} while (0)
#define GET_H_ILINKX(index) CPU (h_cr[index])
#define SET_H_ILINKX(index, x) \
do { \
CPU (h_cr[(index)]) = (x);\
;} while (0)
#define GET_H_R31(index) CPU (h_cr[((UINT) 31)])
#define SET_H_R31(index, x) \
do { \
CPU (h_cr[((UINT) 31)]) = (x);\
;} while (0)
#define GET_H_STATUS32(index) GET_H_AUXR (((UINT) 10))
#define SET_H_STATUS32(index, x) \
do { \
{\
CPU (h_lbit) = ANDSI (SRLSI ((x), 12), 1);\
CPU (h_zbit) = ANDSI (SRLSI ((x), 11), 1);\
CPU (h_nbit) = ANDSI (SRLSI ((x), 10), 1);\
CPU (h_cbit) = ANDSI (SRLSI ((x), 9), 1);\
CPU (h_vbit) = ANDSI (SRLSI ((x), 8), 1);\
CPU (h_e1) = ANDSI (SRLSI ((x), 1), 1);\
CPU (h_e2) = ANDSI (SRLSI ((x), 2), 1);\
}\
;} while (0)

/* Cover fns for register access.  */
BI arc700f_h_lbit_get (SIM_CPU *);
void arc700f_h_lbit_set (SIM_CPU *, BI);
BI arc700f_h_zbit_get (SIM_CPU *);
void arc700f_h_zbit_set (SIM_CPU *, BI);
BI arc700f_h_nbit_get (SIM_CPU *);
void arc700f_h_nbit_set (SIM_CPU *, BI);
BI arc700f_h_cbit_get (SIM_CPU *);
void arc700f_h_cbit_set (SIM_CPU *, BI);
BI arc700f_h_vbit_get (SIM_CPU *);
void arc700f_h_vbit_set (SIM_CPU *, BI);
BI arc700f_h_ubit_get (SIM_CPU *);
void arc700f_h_ubit_set (SIM_CPU *, BI);
BI arc700f_h_e1_get (SIM_CPU *);
void arc700f_h_e1_set (SIM_CPU *, BI);
BI arc700f_h_e2_get (SIM_CPU *);
void arc700f_h_e2_set (SIM_CPU *, BI);
BI arc700f_h_s1bit_get (SIM_CPU *);
void arc700f_h_s1bit_set (SIM_CPU *, BI);
BI arc700f_h_s2bit_get (SIM_CPU *);
void arc700f_h_s2bit_set (SIM_CPU *, BI);
BI arc700f_h_Qcondb_get (SIM_CPU *, UINT);
void arc700f_h_Qcondb_set (SIM_CPU *, UINT, BI);
BI arc700f_h_Qcondj_get (SIM_CPU *, UINT);
void arc700f_h_Qcondj_set (SIM_CPU *, UINT, BI);
BI arc700f_h_Qcondi_get (SIM_CPU *, UINT);
void arc700f_h_Qcondi_set (SIM_CPU *, UINT, BI);
BI arc700f_h_i2cond_get (SIM_CPU *, UINT);
void arc700f_h_i2cond_set (SIM_CPU *, UINT, BI);
BI arc700f_h_i3cond_get (SIM_CPU *, UINT);
void arc700f_h_i3cond_set (SIM_CPU *, UINT, BI);
SI arc700f_h_cr_get (SIM_CPU *, UINT);
void arc700f_h_cr_set (SIM_CPU *, UINT, SI);
SI arc700f_h_cr16_get (SIM_CPU *, UINT);
void arc700f_h_cr16_set (SIM_CPU *, UINT, SI);
SI arc700f_h_r0_get (SIM_CPU *, UINT);
void arc700f_h_r0_set (SIM_CPU *, UINT, SI);
SI arc700f_h_gp_get (SIM_CPU *, UINT);
void arc700f_h_gp_set (SIM_CPU *, UINT, SI);
SI arc700f_h_sp_get (SIM_CPU *, UINT);
void arc700f_h_sp_set (SIM_CPU *, UINT, SI);
SI arc700f_h_pcl_get (SIM_CPU *, UINT);
void arc700f_h_pcl_set (SIM_CPU *, UINT, SI);
SI arc700f_h_noilink_get (SIM_CPU *, UINT);
void arc700f_h_noilink_set (SIM_CPU *, UINT, SI);
SI arc700f_h_ilinkx_get (SIM_CPU *, UINT);
void arc700f_h_ilinkx_set (SIM_CPU *, UINT, SI);
SI arc700f_h_r31_get (SIM_CPU *, UINT);
void arc700f_h_r31_set (SIM_CPU *, UINT, SI);
SI arc700f_h_auxr_get (SIM_CPU *, UINT);
void arc700f_h_auxr_set (SIM_CPU *, UINT, SI);
SI arc700f_h_status32_get (SIM_CPU *, UINT);
void arc700f_h_status32_set (SIM_CPU *, UINT, SI);
SI arc700f_h_timer_expire_get (SIM_CPU *, UINT);
void arc700f_h_timer_expire_set (SIM_CPU *, UINT, SI);
SI arc700f_h_prof_offset_get (SIM_CPU *, UINT);
void arc700f_h_prof_offset_set (SIM_CPU *, UINT, SI);
USI arc700f_h_pc_get (SIM_CPU *);
void arc700f_h_pc_set (SIM_CPU *, USI);

/* These must be hand-written.  */
extern CPUREG_FETCH_FN arc700f_fetch_register;
extern CPUREG_STORE_FN arc700f_store_register;

typedef struct {
  int empty;
} MODEL_ARC700_DATA;

/* Instruction argument buffer.  */

union sem_fields {
  struct { /* no operands */
    int empty;
  } fmt_empty;
  struct { /*  */
    UINT f_trapnum;
  } sfmt_trap_s;
  struct { /*  */
    SI f_s9x2;
  } sfmt_ldw_s_gprel;
  struct { /*  */
    INT f_s9x1;
  } sfmt_ldb_s_gprel;
  struct { /*  */
    SI f_s9x4;
  } sfmt_ld_s_gprel;
  struct { /*  */
    IADDR i_label25a;
  } sfmt_bl;
  struct { /*  */
    IADDR i_label13a;
  } sfmt_bl_s;
  struct { /*  */
    IADDR i_label25;
  } sfmt_b_l;
  struct { /*  */
    INT f_s12x2;
    UINT f_op_B;
  } sfmt_lp_L_s12_;
  struct { /*  */
    UINT f_op__b;
    UINT f_u8;
  } sfmt_mov_s_r_u7;
  struct { /*  */
    UINT f_op__b;
    UINT f_u7;
  } sfmt_add_s_r_u7;
  struct { /*  */
    UINT f_op__b;
    UINT f_op_h;
  } sfmt_add_s_mcah;
  struct { /*  */
    SI f_u8x4;
    UINT f_op__b;
  } sfmt_ld_s_pcrel;
  struct { /*  */
    IADDR i_label21a;
    UINT f_cond_Q;
  } sfmt_blcc;
  struct { /*  */
    IADDR i_label21;
    UINT f_cond_Q;
  } sfmt_bcc_l;
  struct { /*  */
    IADDR i_label7;
    UINT f_cond_i3;
  } sfmt_bcc_s;
  struct { /*  */
    IADDR i_label10;
    UINT f_cond_i2;
  } sfmt_b_s;
  struct { /*  */
    SI f_u6x2;
    UINT f_cond_Q;
    UINT f_op_B;
  } sfmt_lpcc_ccu6;
  struct { /*  */
    UINT f_op__b;
    UINT f_op__c;
    UINT f_u3;
  } sfmt_add_s_cbu3;
  struct { /*  */
    INT f_s12;
    UINT f_F;
    UINT f_op_B;
  } sfmt_add_L_s12__RA_;
  struct { /*  */
    INT f_s9;
    UINT f_op_B;
    UINT f_op_C;
  } sfmt_st_abs;
  struct { /*  */
    SI f_u5x2;
    UINT f_op__b;
    UINT f_op__c;
  } sfmt_ldw_s_abu;
  struct { /*  */
    UINT f_op__b;
    UINT f_op__c;
    UINT f_u5;
  } sfmt_ldb_s_abu;
  struct { /*  */
    SI f_u5x4;
    UINT f_op__b;
    UINT f_op__c;
  } sfmt_ld_s_abu;
  struct { /*  */
    UINT f_op__a;
    UINT f_op__b;
    UINT f_op__c;
  } sfmt_ld_s_abc;
  struct { /*  */
    INT f_s9;
    UINT f_op_A;
    UINT f_op_B;
  } sfmt_ld_abs;
  struct { /*  */
    IADDR i_label8;
    UINT f_brscond;
    UINT f_op__b;
  } sfmt_brcc_s;
  struct { /*  */
    UINT f_F;
    UINT f_op_A;
    UINT f_op_B;
    UINT f_op_C;
  } sfmt_add_L_r_r__RA__RC;
  struct { /*  */
    UINT f_F;
    UINT f_op_A;
    UINT f_op_B;
    UINT f_u6;
  } sfmt_add_L_u6__RA_;
  struct { /*  */
    UINT f_F;
    UINT f_cond_Q;
    UINT f_op_B;
    UINT f_u6;
  } sfmt_add_ccu6__RA_;
  struct { /*  */
    IADDR i_label9;
    UINT f_brcond;
    UINT f_op_B;
    UINT f_u6;
  } sfmt_brcc_U6;
  struct { /*  */
    IADDR i_label9;
    UINT f_brcond;
    UINT f_op_B;
    UINT f_op_C;
  } sfmt_brcc_RC;
  struct { /*  */
    UINT f_F;
    UINT f_cond_Q;
    UINT f_op_B;
    UINT f_op_C;
    UINT f_op_Cj;
  } sfmt_j_cc___RC_noilink_;
#if WITH_SCACHE_PBB
  /* Writeback handler.  */
  struct {
    /* Pointer to argbuf entry for insn whose results need writing back.  */
    const struct argbuf *abuf;
  } write;
  /* x-before handler */
  struct {
    /*const SCACHE *insns[MAX_PARALLEL_INSNS];*/
    int first_p;
  } before;
  /* x-after handler */
  struct {
    int empty;
  } after;
  /* This entry is used to terminate each pbb.  */
  struct {
    /* Number of insns in pbb.  */
    int insn_count;
    /* Next pbb to execute.  */
    SCACHE *next;
    SCACHE *branch_target;
  } chain;
#endif
};

/* The ARGBUF struct.  */
struct argbuf {
  /* These are the baseclass definitions.  */
  IADDR addr;
  const IDESC *idesc;
  char trace_p;
  char profile_p;
  /* ??? Temporary hack for skip insns.  */
  char skip_count;
  char unused;
  /* cpu specific data follows */
  union sem semantic;
  int written;
  union sem_fields fields;
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
  unsigned int length;
#define EXTRACT_IFMT_EMPTY_CODE \
  length = 0; \

#define EXTRACT_IFMT_B_S_VARS \
  UINT f_opm; \
  UINT f_cond_i2; \
  SI f_rel10; \
  UINT f_dummy; \
  unsigned int length;
#define EXTRACT_IFMT_B_S_CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_cond_i2 = EXTRACT_MSB0_UINT (insn, 32, 5, 2); \
  f_rel10 = ((((EXTRACT_MSB0_INT (insn, 32, 7, 9)) << (1))) + (((pc) & (-4)))); \
  f_dummy = EXTRACT_MSB0_UINT (insn, 32, 16, 16); \

#define EXTRACT_IFMT_BCC_S_VARS \
  UINT f_opm; \
  UINT f_cond_i2; \
  UINT f_cond_i3; \
  SI f_rel7; \
  UINT f_dummy; \
  unsigned int length;
#define EXTRACT_IFMT_BCC_S_CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_cond_i2 = EXTRACT_MSB0_UINT (insn, 32, 5, 2); \
  f_cond_i3 = EXTRACT_MSB0_UINT (insn, 32, 7, 3); \
  f_rel7 = ((((EXTRACT_MSB0_INT (insn, 32, 10, 6)) << (1))) + (((pc) & (-4)))); \
  f_dummy = EXTRACT_MSB0_UINT (insn, 32, 16, 16); \

#define EXTRACT_IFMT_BRCC_S_VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_brscond; \
  SI f_rel8; \
  UINT f_dummy; \
  unsigned int length;
#define EXTRACT_IFMT_BRCC_S_CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_brscond = EXTRACT_MSB0_UINT (insn, 32, 8, 1); \
  f_rel8 = ((((EXTRACT_MSB0_INT (insn, 32, 9, 7)) << (1))) + (((pc) & (-4)))); \
  f_dummy = EXTRACT_MSB0_UINT (insn, 32, 16, 16); \

#define EXTRACT_IFMT_BCC_L_VARS \
  UINT f_opm; \
  UINT f_d21l; \
  INT f_d21h; \
  INT f_rel21; \
  UINT f_buf; \
  UINT f_delay_N; \
  UINT f_cond_Q; \
  unsigned int length;
#define EXTRACT_IFMT_BCC_L_CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_d21l = EXTRACT_MSB0_UINT (insn, 32, 5, 10); \
  f_d21h = EXTRACT_MSB0_INT (insn, 32, 16, 10); \
{\
  f_rel21 = ((((((f_d21l) << (1))) | (((f_d21h) << (11))))) + (((pc) & (-4))));\
}\
  f_buf = EXTRACT_MSB0_UINT (insn, 32, 15, 1); \
  f_delay_N = EXTRACT_MSB0_UINT (insn, 32, 26, 1); \
  f_cond_Q = EXTRACT_MSB0_UINT (insn, 32, 27, 5); \

#define EXTRACT_IFMT_B_L_VARS \
  UINT f_opm; \
  UINT f_d21l; \
  UINT f_d25m; \
  INT f_d25h; \
  INT f_rel25; \
  UINT f_buf; \
  UINT f_delay_N; \
  UINT f_res27; \
  unsigned int length;
#define EXTRACT_IFMT_B_L_CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_d21l = EXTRACT_MSB0_UINT (insn, 32, 5, 10); \
  f_d25m = EXTRACT_MSB0_UINT (insn, 32, 16, 10); \
  f_d25h = EXTRACT_MSB0_INT (insn, 32, 28, 4); \
{\
  f_rel25 = ((((((((f_d21l) << (1))) | (((f_d25m) << (11))))) | (((f_d25h) << (21))))) + (((pc) & (-4))));\
}\
  f_buf = EXTRACT_MSB0_UINT (insn, 32, 15, 1); \
  f_delay_N = EXTRACT_MSB0_UINT (insn, 32, 26, 1); \
  f_res27 = EXTRACT_MSB0_UINT (insn, 32, 27, 1); \

#define EXTRACT_IFMT_BRCC_RC_VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_B_5_3; \
  UINT f_op_B; \
  UINT f_d9l; \
  INT f_d9h; \
  INT f_rel9; \
  UINT f_buf; \
  UINT f_op_C; \
  UINT f_delay_N; \
  UINT f_br; \
  UINT f_brcond; \
  unsigned int length;
#define EXTRACT_IFMT_BRCC_RC_CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_B_5_3 = EXTRACT_MSB0_UINT (insn, 32, 17, 3); \
{\
  f_op_B = ((f_op__b) | (((f_B_5_3) << (3))));\
}\
  f_d9l = EXTRACT_MSB0_UINT (insn, 32, 8, 7); \
  f_d9h = EXTRACT_MSB0_INT (insn, 32, 16, 1); \
{\
  f_rel9 = ((((((f_d9l) << (1))) | (((f_d9h) << (8))))) + (((pc) & (-4))));\
}\
  f_buf = EXTRACT_MSB0_UINT (insn, 32, 15, 1); \
  f_op_C = EXTRACT_MSB0_UINT (insn, 32, 20, 6); \
  f_delay_N = EXTRACT_MSB0_UINT (insn, 32, 26, 1); \
  f_br = EXTRACT_MSB0_UINT (insn, 32, 27, 1); \
  f_brcond = EXTRACT_MSB0_UINT (insn, 32, 28, 4); \

#define EXTRACT_IFMT_BRCC_U6_VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_B_5_3; \
  UINT f_op_B; \
  UINT f_d9l; \
  INT f_d9h; \
  INT f_rel9; \
  UINT f_buf; \
  UINT f_u6; \
  UINT f_delay_N; \
  UINT f_br; \
  UINT f_brcond; \
  unsigned int length;
#define EXTRACT_IFMT_BRCC_U6_CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_B_5_3 = EXTRACT_MSB0_UINT (insn, 32, 17, 3); \
{\
  f_op_B = ((f_op__b) | (((f_B_5_3) << (3))));\
}\
  f_d9l = EXTRACT_MSB0_UINT (insn, 32, 8, 7); \
  f_d9h = EXTRACT_MSB0_INT (insn, 32, 16, 1); \
{\
  f_rel9 = ((((((f_d9l) << (1))) | (((f_d9h) << (8))))) + (((pc) & (-4))));\
}\
  f_buf = EXTRACT_MSB0_UINT (insn, 32, 15, 1); \
  f_u6 = EXTRACT_MSB0_UINT (insn, 32, 20, 6); \
  f_delay_N = EXTRACT_MSB0_UINT (insn, 32, 26, 1); \
  f_br = EXTRACT_MSB0_UINT (insn, 32, 27, 1); \
  f_brcond = EXTRACT_MSB0_UINT (insn, 32, 28, 4); \

#define EXTRACT_IFMT_BL_S_VARS \
  UINT f_opm; \
  SI f_rel13bl; \
  UINT f_dummy; \
  unsigned int length;
#define EXTRACT_IFMT_BL_S_CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_rel13bl = ((((EXTRACT_MSB0_INT (insn, 32, 5, 11)) << (2))) + (((pc) & (-4)))); \
  f_dummy = EXTRACT_MSB0_UINT (insn, 32, 16, 16); \

#define EXTRACT_IFMT_BLCC_VARS \
  UINT f_opm; \
  UINT f_d21bl; \
  INT f_d21h; \
  INT f_rel21bl; \
  UINT f_bluf; \
  UINT f_buf; \
  UINT f_delay_N; \
  UINT f_cond_Q; \
  unsigned int length;
#define EXTRACT_IFMT_BLCC_CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_d21bl = EXTRACT_MSB0_UINT (insn, 32, 5, 9); \
  f_d21h = EXTRACT_MSB0_INT (insn, 32, 16, 10); \
{\
  f_rel21bl = ((((((f_d21bl) << (2))) | (((f_d21h) << (11))))) + (((pc) & (-4))));\
}\
  f_bluf = EXTRACT_MSB0_UINT (insn, 32, 14, 1); \
  f_buf = EXTRACT_MSB0_UINT (insn, 32, 15, 1); \
  f_delay_N = EXTRACT_MSB0_UINT (insn, 32, 26, 1); \
  f_cond_Q = EXTRACT_MSB0_UINT (insn, 32, 27, 5); \

#define EXTRACT_IFMT_BL_VARS \
  UINT f_opm; \
  UINT f_d21bl; \
  UINT f_d25m; \
  INT f_d25h; \
  INT f_rel25bl; \
  UINT f_bluf; \
  UINT f_buf; \
  UINT f_delay_N; \
  UINT f_res27; \
  unsigned int length;
#define EXTRACT_IFMT_BL_CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_d21bl = EXTRACT_MSB0_UINT (insn, 32, 5, 9); \
  f_d25m = EXTRACT_MSB0_UINT (insn, 32, 16, 10); \
  f_d25h = EXTRACT_MSB0_INT (insn, 32, 28, 4); \
{\
  f_rel25bl = ((((((((f_d21bl) << (2))) | (((f_d25m) << (11))))) | (((f_d25h) << (21))))) + (((pc) & (-4))));\
}\
  f_bluf = EXTRACT_MSB0_UINT (insn, 32, 14, 1); \
  f_buf = EXTRACT_MSB0_UINT (insn, 32, 15, 1); \
  f_delay_N = EXTRACT_MSB0_UINT (insn, 32, 26, 1); \
  f_res27 = EXTRACT_MSB0_UINT (insn, 32, 27, 1); \

#define EXTRACT_IFMT_LD_ABS_VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_B_5_3; \
  UINT f_op_B; \
  UINT f_u8; \
  INT f_d9h; \
  INT f_s9; \
  UINT f_LDODi; \
  UINT f_ldoaa; \
  UINT f_ldozzx; \
  UINT f_op_A; \
  unsigned int length;
#define EXTRACT_IFMT_LD_ABS_CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_B_5_3 = EXTRACT_MSB0_UINT (insn, 32, 17, 3); \
{\
  f_op_B = ((f_op__b) | (((f_B_5_3) << (3))));\
}\
  f_u8 = EXTRACT_MSB0_UINT (insn, 32, 8, 8); \
  f_d9h = EXTRACT_MSB0_INT (insn, 32, 16, 1); \
{\
  f_s9 = ((f_u8) | (((f_d9h) << (8))));\
}\
  f_LDODi = EXTRACT_MSB0_UINT (insn, 32, 20, 1); \
  f_ldoaa = EXTRACT_MSB0_UINT (insn, 32, 21, 2); \
  f_ldozzx = EXTRACT_MSB0_UINT (insn, 32, 23, 3); \
  f_op_A = EXTRACT_MSB0_UINT (insn, 32, 26, 6); \

#define EXTRACT_IFMT_LD_ABC_VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_B_5_3; \
  UINT f_op_B; \
  UINT f_ldraa; \
  UINT f_ldr6zzx; \
  UINT f_LDRDi; \
  UINT f_op_C; \
  UINT f_op_A; \
  unsigned int length;
#define EXTRACT_IFMT_LD_ABC_CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_B_5_3 = EXTRACT_MSB0_UINT (insn, 32, 17, 3); \
{\
  f_op_B = ((f_op__b) | (((f_B_5_3) << (3))));\
}\
  f_ldraa = EXTRACT_MSB0_UINT (insn, 32, 8, 2); \
  f_ldr6zzx = EXTRACT_MSB0_UINT (insn, 32, 10, 6); \
  f_LDRDi = EXTRACT_MSB0_UINT (insn, 32, 16, 1); \
  f_op_C = EXTRACT_MSB0_UINT (insn, 32, 20, 6); \
  f_op_A = EXTRACT_MSB0_UINT (insn, 32, 26, 6); \

#define EXTRACT_IFMT_LD_S_ABC_VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_op__c; \
  UINT f_i16_43; \
  UINT f_op__a; \
  UINT f_dummy; \
  unsigned int length;
#define EXTRACT_IFMT_LD_S_ABC_CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_op__c = EXTRACT_MSB0_UINT (insn, 32, 8, 3); \
  f_i16_43 = EXTRACT_MSB0_UINT (insn, 32, 11, 2); \
  f_op__a = EXTRACT_MSB0_UINT (insn, 32, 13, 3); \
  f_dummy = EXTRACT_MSB0_UINT (insn, 32, 16, 16); \

#define EXTRACT_IFMT_LD_S_ABU_VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_op__c; \
  SI f_u5x4; \
  UINT f_dummy; \
  unsigned int length;
#define EXTRACT_IFMT_LD_S_ABU_CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_op__c = EXTRACT_MSB0_UINT (insn, 32, 8, 3); \
  f_u5x4 = ((EXTRACT_MSB0_UINT (insn, 32, 11, 5)) << (2)); \
  f_dummy = EXTRACT_MSB0_UINT (insn, 32, 16, 16); \

#define EXTRACT_IFMT_LD_S_ABSP_VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_op__c; \
  SI f_u5x4; \
  UINT f_dummy; \
  unsigned int length;
#define EXTRACT_IFMT_LD_S_ABSP_CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_op__c = EXTRACT_MSB0_UINT (insn, 32, 8, 3); \
  f_u5x4 = ((EXTRACT_MSB0_UINT (insn, 32, 11, 5)) << (2)); \
  f_dummy = EXTRACT_MSB0_UINT (insn, 32, 16, 16); \

#define EXTRACT_IFMT_LD_S_GPREL_VARS \
  UINT f_opm; \
  UINT f_i16_gp_type; \
  SI f_s9x4; \
  UINT f_dummy; \
  unsigned int length;
#define EXTRACT_IFMT_LD_S_GPREL_CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_i16_gp_type = EXTRACT_MSB0_UINT (insn, 32, 5, 2); \
  f_s9x4 = ((EXTRACT_MSB0_INT (insn, 32, 7, 9)) << (2)); \
  f_dummy = EXTRACT_MSB0_UINT (insn, 32, 16, 16); \

#define EXTRACT_IFMT_LD_S_PCREL_VARS \
  UINT f_opm; \
  UINT f_op__b; \
  SI f_u8x4; \
  UINT f_dummy; \
  unsigned int length;
#define EXTRACT_IFMT_LD_S_PCREL_CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_u8x4 = ((EXTRACT_MSB0_UINT (insn, 32, 8, 8)) << (2)); \
  f_dummy = EXTRACT_MSB0_UINT (insn, 32, 16, 16); \

#define EXTRACT_IFMT_LDB_S_ABU_VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_op__c; \
  UINT f_u5; \
  UINT f_dummy; \
  unsigned int length;
#define EXTRACT_IFMT_LDB_S_ABU_CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_op__c = EXTRACT_MSB0_UINT (insn, 32, 8, 3); \
  f_u5 = EXTRACT_MSB0_UINT (insn, 32, 11, 5); \
  f_dummy = EXTRACT_MSB0_UINT (insn, 32, 16, 16); \

#define EXTRACT_IFMT_LDB_S_GPREL_VARS \
  UINT f_opm; \
  UINT f_i16_gp_type; \
  INT f_s9x1; \
  UINT f_dummy; \
  unsigned int length;
#define EXTRACT_IFMT_LDB_S_GPREL_CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_i16_gp_type = EXTRACT_MSB0_UINT (insn, 32, 5, 2); \
  f_s9x1 = EXTRACT_MSB0_INT (insn, 32, 7, 9); \
  f_dummy = EXTRACT_MSB0_UINT (insn, 32, 16, 16); \

#define EXTRACT_IFMT_LDW_S_ABU_VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_op__c; \
  SI f_u5x2; \
  UINT f_dummy; \
  unsigned int length;
#define EXTRACT_IFMT_LDW_S_ABU_CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_op__c = EXTRACT_MSB0_UINT (insn, 32, 8, 3); \
  f_u5x2 = ((EXTRACT_MSB0_UINT (insn, 32, 11, 5)) << (1)); \
  f_dummy = EXTRACT_MSB0_UINT (insn, 32, 16, 16); \

#define EXTRACT_IFMT_LDW_S_GPREL_VARS \
  UINT f_opm; \
  UINT f_i16_gp_type; \
  SI f_s9x2; \
  UINT f_dummy; \
  unsigned int length;
#define EXTRACT_IFMT_LDW_S_GPREL_CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_i16_gp_type = EXTRACT_MSB0_UINT (insn, 32, 5, 2); \
  f_s9x2 = ((EXTRACT_MSB0_INT (insn, 32, 7, 9)) << (1)); \
  f_dummy = EXTRACT_MSB0_UINT (insn, 32, 16, 16); \

#define EXTRACT_IFMT_ST_ABS_VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_B_5_3; \
  UINT f_op_B; \
  UINT f_u8; \
  INT f_d9h; \
  INT f_s9; \
  UINT f_LDODi; \
  UINT f_op_C; \
  UINT f_stoaa; \
  UINT f_stozzr; \
  unsigned int length;
#define EXTRACT_IFMT_ST_ABS_CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_B_5_3 = EXTRACT_MSB0_UINT (insn, 32, 17, 3); \
{\
  f_op_B = ((f_op__b) | (((f_B_5_3) << (3))));\
}\
  f_u8 = EXTRACT_MSB0_UINT (insn, 32, 8, 8); \
  f_d9h = EXTRACT_MSB0_INT (insn, 32, 16, 1); \
{\
  f_s9 = ((f_u8) | (((f_d9h) << (8))));\
}\
  f_LDODi = EXTRACT_MSB0_UINT (insn, 32, 20, 1); \
  f_op_C = EXTRACT_MSB0_UINT (insn, 32, 20, 6); \
  f_stoaa = EXTRACT_MSB0_UINT (insn, 32, 27, 2); \
  f_stozzr = EXTRACT_MSB0_UINT (insn, 32, 29, 3); \

#define EXTRACT_IFMT_ADD_L_S12__RA__VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_B_5_3; \
  UINT f_op_B; \
  UINT f_go_type; \
  UINT f_go_op; \
  UINT f_F; \
  UINT f_u6; \
  INT f_s12h; \
  INT f_s12; \
  unsigned int length;
#define EXTRACT_IFMT_ADD_L_S12__RA__CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_B_5_3 = EXTRACT_MSB0_UINT (insn, 32, 17, 3); \
{\
  f_op_B = ((f_op__b) | (((f_B_5_3) << (3))));\
}\
  f_go_type = EXTRACT_MSB0_UINT (insn, 32, 8, 2); \
  f_go_op = EXTRACT_MSB0_UINT (insn, 32, 10, 6); \
  f_F = EXTRACT_MSB0_UINT (insn, 32, 16, 1); \
  f_u6 = EXTRACT_MSB0_UINT (insn, 32, 20, 6); \
  f_s12h = EXTRACT_MSB0_INT (insn, 32, 26, 6); \
{\
  f_s12 = ((f_u6) | (((f_s12h) << (6))));\
}\

#define EXTRACT_IFMT_ADD_CCU6__RA__VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_B_5_3; \
  UINT f_op_B; \
  UINT f_go_type; \
  UINT f_go_op; \
  UINT f_F; \
  UINT f_u6; \
  UINT f_go_cc_type; \
  UINT f_cond_Q; \
  unsigned int length;
#define EXTRACT_IFMT_ADD_CCU6__RA__CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_B_5_3 = EXTRACT_MSB0_UINT (insn, 32, 17, 3); \
{\
  f_op_B = ((f_op__b) | (((f_B_5_3) << (3))));\
}\
  f_go_type = EXTRACT_MSB0_UINT (insn, 32, 8, 2); \
  f_go_op = EXTRACT_MSB0_UINT (insn, 32, 10, 6); \
  f_F = EXTRACT_MSB0_UINT (insn, 32, 16, 1); \
  f_u6 = EXTRACT_MSB0_UINT (insn, 32, 20, 6); \
  f_go_cc_type = EXTRACT_MSB0_UINT (insn, 32, 26, 1); \
  f_cond_Q = EXTRACT_MSB0_UINT (insn, 32, 27, 5); \

#define EXTRACT_IFMT_ADD_L_U6__RA__VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_B_5_3; \
  UINT f_op_B; \
  UINT f_go_type; \
  UINT f_go_op; \
  UINT f_F; \
  UINT f_u6; \
  UINT f_op_A; \
  unsigned int length;
#define EXTRACT_IFMT_ADD_L_U6__RA__CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_B_5_3 = EXTRACT_MSB0_UINT (insn, 32, 17, 3); \
{\
  f_op_B = ((f_op__b) | (((f_B_5_3) << (3))));\
}\
  f_go_type = EXTRACT_MSB0_UINT (insn, 32, 8, 2); \
  f_go_op = EXTRACT_MSB0_UINT (insn, 32, 10, 6); \
  f_F = EXTRACT_MSB0_UINT (insn, 32, 16, 1); \
  f_u6 = EXTRACT_MSB0_UINT (insn, 32, 20, 6); \
  f_op_A = EXTRACT_MSB0_UINT (insn, 32, 26, 6); \

#define EXTRACT_IFMT_ADD_L_R_R__RA__RC_VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_B_5_3; \
  UINT f_op_B; \
  UINT f_go_type; \
  UINT f_go_op; \
  UINT f_F; \
  UINT f_op_C; \
  UINT f_op_A; \
  unsigned int length;
#define EXTRACT_IFMT_ADD_L_R_R__RA__RC_CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_B_5_3 = EXTRACT_MSB0_UINT (insn, 32, 17, 3); \
{\
  f_op_B = ((f_op__b) | (((f_B_5_3) << (3))));\
}\
  f_go_type = EXTRACT_MSB0_UINT (insn, 32, 8, 2); \
  f_go_op = EXTRACT_MSB0_UINT (insn, 32, 10, 6); \
  f_F = EXTRACT_MSB0_UINT (insn, 32, 16, 1); \
  f_op_C = EXTRACT_MSB0_UINT (insn, 32, 20, 6); \
  f_op_A = EXTRACT_MSB0_UINT (insn, 32, 26, 6); \

#define EXTRACT_IFMT_ADD_CC__RA__RC_VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_B_5_3; \
  UINT f_op_B; \
  UINT f_go_type; \
  UINT f_go_op; \
  UINT f_F; \
  UINT f_op_C; \
  UINT f_go_cc_type; \
  UINT f_cond_Q; \
  unsigned int length;
#define EXTRACT_IFMT_ADD_CC__RA__RC_CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_B_5_3 = EXTRACT_MSB0_UINT (insn, 32, 17, 3); \
{\
  f_op_B = ((f_op__b) | (((f_B_5_3) << (3))));\
}\
  f_go_type = EXTRACT_MSB0_UINT (insn, 32, 8, 2); \
  f_go_op = EXTRACT_MSB0_UINT (insn, 32, 10, 6); \
  f_F = EXTRACT_MSB0_UINT (insn, 32, 16, 1); \
  f_op_C = EXTRACT_MSB0_UINT (insn, 32, 20, 6); \
  f_go_cc_type = EXTRACT_MSB0_UINT (insn, 32, 26, 1); \
  f_cond_Q = EXTRACT_MSB0_UINT (insn, 32, 27, 5); \

#define EXTRACT_IFMT_ADD_S_CBU3_VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_op__c; \
  UINT f_i16_43; \
  UINT f_u3; \
  UINT f_dummy; \
  unsigned int length;
#define EXTRACT_IFMT_ADD_S_CBU3_CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_op__c = EXTRACT_MSB0_UINT (insn, 32, 8, 3); \
  f_i16_43 = EXTRACT_MSB0_UINT (insn, 32, 11, 2); \
  f_u3 = EXTRACT_MSB0_UINT (insn, 32, 13, 3); \
  f_dummy = EXTRACT_MSB0_UINT (insn, 32, 16, 16); \

#define EXTRACT_IFMT_ADD_S_MCAH_VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_h_2_0; \
  UINT f_h_5_3; \
  UINT f_op_h; \
  UINT f_i16_43; \
  UINT f_dummy; \
  unsigned int length;
#define EXTRACT_IFMT_ADD_S_MCAH_CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_h_2_0 = EXTRACT_MSB0_UINT (insn, 32, 8, 3); \
  f_h_5_3 = EXTRACT_MSB0_UINT (insn, 32, 13, 3); \
{\
  f_op_h = ((f_h_2_0) | (((f_h_5_3) << (3))));\
}\
  f_i16_43 = EXTRACT_MSB0_UINT (insn, 32, 11, 2); \
  f_dummy = EXTRACT_MSB0_UINT (insn, 32, 16, 16); \

#define EXTRACT_IFMT_ADD_S_ASSPSP_VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_op__c; \
  SI f_u5x4; \
  UINT f_dummy; \
  unsigned int length;
#define EXTRACT_IFMT_ADD_S_ASSPSP_CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_op__c = EXTRACT_MSB0_UINT (insn, 32, 8, 3); \
  f_u5x4 = ((EXTRACT_MSB0_UINT (insn, 32, 11, 5)) << (2)); \
  f_dummy = EXTRACT_MSB0_UINT (insn, 32, 16, 16); \

#define EXTRACT_IFMT_ADD_S_GP_VARS \
  UINT f_opm; \
  UINT f_i16_gp_type; \
  SI f_s9x4; \
  UINT f_dummy; \
  unsigned int length;
#define EXTRACT_IFMT_ADD_S_GP_CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_i16_gp_type = EXTRACT_MSB0_UINT (insn, 32, 5, 2); \
  f_s9x4 = ((EXTRACT_MSB0_INT (insn, 32, 7, 9)) << (2)); \
  f_dummy = EXTRACT_MSB0_UINT (insn, 32, 16, 16); \

#define EXTRACT_IFMT_ADD_S_R_U7_VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_i16addcmpu7_type; \
  UINT f_u7; \
  UINT f_dummy; \
  unsigned int length;
#define EXTRACT_IFMT_ADD_S_R_U7_CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_i16addcmpu7_type = EXTRACT_MSB0_UINT (insn, 32, 8, 1); \
  f_u7 = EXTRACT_MSB0_UINT (insn, 32, 9, 7); \
  f_dummy = EXTRACT_MSB0_UINT (insn, 32, 16, 16); \

#define EXTRACT_IFMT_I16_GO_SUB_S_GO_VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_op__c; \
  UINT f_i16_go; \
  UINT f_dummy; \
  unsigned int length;
#define EXTRACT_IFMT_I16_GO_SUB_S_GO_CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_op__c = EXTRACT_MSB0_UINT (insn, 32, 8, 3); \
  f_i16_go = EXTRACT_MSB0_UINT (insn, 32, 11, 5); \
  f_dummy = EXTRACT_MSB0_UINT (insn, 32, 16, 16); \

#define EXTRACT_IFMT_SUB_S_GO_SUB_NE_VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_op__c; \
  UINT f_i16_go; \
  UINT f_dummy; \
  unsigned int length;
#define EXTRACT_IFMT_SUB_S_GO_SUB_NE_CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_op__c = EXTRACT_MSB0_UINT (insn, 32, 8, 3); \
  f_i16_go = EXTRACT_MSB0_UINT (insn, 32, 11, 5); \
  f_dummy = EXTRACT_MSB0_UINT (insn, 32, 16, 16); \

#define EXTRACT_IFMT_SUB_S_SSB_VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_op__c; \
  UINT f_u5; \
  UINT f_dummy; \
  unsigned int length;
#define EXTRACT_IFMT_SUB_S_SSB_CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_op__c = EXTRACT_MSB0_UINT (insn, 32, 8, 3); \
  f_u5 = EXTRACT_MSB0_UINT (insn, 32, 11, 5); \
  f_dummy = EXTRACT_MSB0_UINT (insn, 32, 16, 16); \

#define EXTRACT_IFMT_MOV_L_U6__VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_B_5_3; \
  UINT f_op_B; \
  UINT f_go_type; \
  UINT f_go_op; \
  UINT f_F; \
  UINT f_u6; \
  UINT f_op_A; \
  unsigned int length;
#define EXTRACT_IFMT_MOV_L_U6__CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_B_5_3 = EXTRACT_MSB0_UINT (insn, 32, 17, 3); \
{\
  f_op_B = ((f_op__b) | (((f_B_5_3) << (3))));\
}\
  f_go_type = EXTRACT_MSB0_UINT (insn, 32, 8, 2); \
  f_go_op = EXTRACT_MSB0_UINT (insn, 32, 10, 6); \
  f_F = EXTRACT_MSB0_UINT (insn, 32, 16, 1); \
  f_u6 = EXTRACT_MSB0_UINT (insn, 32, 20, 6); \
  f_op_A = EXTRACT_MSB0_UINT (insn, 32, 26, 6); \

#define EXTRACT_IFMT_MOV_L_R_R__RC_VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_B_5_3; \
  UINT f_op_B; \
  UINT f_go_type; \
  UINT f_go_op; \
  UINT f_F; \
  UINT f_op_C; \
  UINT f_op_A; \
  unsigned int length;
#define EXTRACT_IFMT_MOV_L_R_R__RC_CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_B_5_3 = EXTRACT_MSB0_UINT (insn, 32, 17, 3); \
{\
  f_op_B = ((f_op__b) | (((f_B_5_3) << (3))));\
}\
  f_go_type = EXTRACT_MSB0_UINT (insn, 32, 8, 2); \
  f_go_op = EXTRACT_MSB0_UINT (insn, 32, 10, 6); \
  f_F = EXTRACT_MSB0_UINT (insn, 32, 16, 1); \
  f_op_C = EXTRACT_MSB0_UINT (insn, 32, 20, 6); \
  f_op_A = EXTRACT_MSB0_UINT (insn, 32, 26, 6); \

#define EXTRACT_IFMT_MOV_S_R_U7_VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_u8; \
  UINT f_dummy; \
  unsigned int length;
#define EXTRACT_IFMT_MOV_S_R_U7_CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_u8 = EXTRACT_MSB0_UINT (insn, 32, 8, 8); \
  f_dummy = EXTRACT_MSB0_UINT (insn, 32, 16, 16); \

#define EXTRACT_IFMT_TST_L_S12__VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_B_5_3; \
  UINT f_op_B; \
  UINT f_go_type; \
  UINT f_go_op; \
  UINT f_F; \
  UINT f_u6; \
  INT f_s12h; \
  INT f_s12; \
  unsigned int length;
#define EXTRACT_IFMT_TST_L_S12__CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_B_5_3 = EXTRACT_MSB0_UINT (insn, 32, 17, 3); \
{\
  f_op_B = ((f_op__b) | (((f_B_5_3) << (3))));\
}\
  f_go_type = EXTRACT_MSB0_UINT (insn, 32, 8, 2); \
  f_go_op = EXTRACT_MSB0_UINT (insn, 32, 10, 6); \
  f_F = EXTRACT_MSB0_UINT (insn, 32, 16, 1); \
  f_u6 = EXTRACT_MSB0_UINT (insn, 32, 20, 6); \
  f_s12h = EXTRACT_MSB0_INT (insn, 32, 26, 6); \
{\
  f_s12 = ((f_u6) | (((f_s12h) << (6))));\
}\

#define EXTRACT_IFMT_TST_CCU6__VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_B_5_3; \
  UINT f_op_B; \
  UINT f_go_type; \
  UINT f_go_op; \
  UINT f_F; \
  UINT f_u6; \
  UINT f_go_cc_type; \
  UINT f_cond_Q; \
  unsigned int length;
#define EXTRACT_IFMT_TST_CCU6__CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_B_5_3 = EXTRACT_MSB0_UINT (insn, 32, 17, 3); \
{\
  f_op_B = ((f_op__b) | (((f_B_5_3) << (3))));\
}\
  f_go_type = EXTRACT_MSB0_UINT (insn, 32, 8, 2); \
  f_go_op = EXTRACT_MSB0_UINT (insn, 32, 10, 6); \
  f_F = EXTRACT_MSB0_UINT (insn, 32, 16, 1); \
  f_u6 = EXTRACT_MSB0_UINT (insn, 32, 20, 6); \
  f_go_cc_type = EXTRACT_MSB0_UINT (insn, 32, 26, 1); \
  f_cond_Q = EXTRACT_MSB0_UINT (insn, 32, 27, 5); \

#define EXTRACT_IFMT_TST_L_U6__VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_B_5_3; \
  UINT f_op_B; \
  UINT f_go_type; \
  UINT f_go_op; \
  UINT f_F; \
  UINT f_u6; \
  UINT f_op_A; \
  unsigned int length;
#define EXTRACT_IFMT_TST_L_U6__CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_B_5_3 = EXTRACT_MSB0_UINT (insn, 32, 17, 3); \
{\
  f_op_B = ((f_op__b) | (((f_B_5_3) << (3))));\
}\
  f_go_type = EXTRACT_MSB0_UINT (insn, 32, 8, 2); \
  f_go_op = EXTRACT_MSB0_UINT (insn, 32, 10, 6); \
  f_F = EXTRACT_MSB0_UINT (insn, 32, 16, 1); \
  f_u6 = EXTRACT_MSB0_UINT (insn, 32, 20, 6); \
  f_op_A = EXTRACT_MSB0_UINT (insn, 32, 26, 6); \

#define EXTRACT_IFMT_TST_L_R_R__RC_VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_B_5_3; \
  UINT f_op_B; \
  UINT f_go_type; \
  UINT f_go_op; \
  UINT f_F; \
  UINT f_op_C; \
  UINT f_op_A; \
  unsigned int length;
#define EXTRACT_IFMT_TST_L_R_R__RC_CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_B_5_3 = EXTRACT_MSB0_UINT (insn, 32, 17, 3); \
{\
  f_op_B = ((f_op__b) | (((f_B_5_3) << (3))));\
}\
  f_go_type = EXTRACT_MSB0_UINT (insn, 32, 8, 2); \
  f_go_op = EXTRACT_MSB0_UINT (insn, 32, 10, 6); \
  f_F = EXTRACT_MSB0_UINT (insn, 32, 16, 1); \
  f_op_C = EXTRACT_MSB0_UINT (insn, 32, 20, 6); \
  f_op_A = EXTRACT_MSB0_UINT (insn, 32, 26, 6); \

#define EXTRACT_IFMT_TST_CC__RC_VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_B_5_3; \
  UINT f_op_B; \
  UINT f_go_type; \
  UINT f_go_op; \
  UINT f_F; \
  UINT f_op_C; \
  UINT f_go_cc_type; \
  UINT f_cond_Q; \
  unsigned int length;
#define EXTRACT_IFMT_TST_CC__RC_CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_B_5_3 = EXTRACT_MSB0_UINT (insn, 32, 17, 3); \
{\
  f_op_B = ((f_op__b) | (((f_B_5_3) << (3))));\
}\
  f_go_type = EXTRACT_MSB0_UINT (insn, 32, 8, 2); \
  f_go_op = EXTRACT_MSB0_UINT (insn, 32, 10, 6); \
  f_F = EXTRACT_MSB0_UINT (insn, 32, 16, 1); \
  f_op_C = EXTRACT_MSB0_UINT (insn, 32, 20, 6); \
  f_go_cc_type = EXTRACT_MSB0_UINT (insn, 32, 26, 1); \
  f_cond_Q = EXTRACT_MSB0_UINT (insn, 32, 27, 5); \

#define EXTRACT_IFMT_J_L_R_R___RC_NOILINK__VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_B_5_3; \
  UINT f_op_B; \
  UINT f_go_type; \
  UINT f_go_op; \
  UINT f_F; \
  UINT f_op_Cj; \
  UINT f_op_A; \
  unsigned int length;
#define EXTRACT_IFMT_J_L_R_R___RC_NOILINK__CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_B_5_3 = EXTRACT_MSB0_UINT (insn, 32, 17, 3); \
{\
  f_op_B = ((f_op__b) | (((f_B_5_3) << (3))));\
}\
  f_go_type = EXTRACT_MSB0_UINT (insn, 32, 8, 2); \
  f_go_op = EXTRACT_MSB0_UINT (insn, 32, 10, 6); \
  f_F = EXTRACT_MSB0_UINT (insn, 32, 16, 1); \
  f_op_Cj = EXTRACT_MSB0_UINT (insn, 32, 20, 6); \
  f_op_A = EXTRACT_MSB0_UINT (insn, 32, 26, 6); \

#define EXTRACT_IFMT_J_CC___RC_NOILINK__VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_B_5_3; \
  UINT f_op_B; \
  UINT f_go_type; \
  UINT f_go_op; \
  UINT f_F; \
  UINT f_op_Cj; \
  UINT f_go_cc_type; \
  UINT f_cond_Q; \
  unsigned int length;
#define EXTRACT_IFMT_J_CC___RC_NOILINK__CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_B_5_3 = EXTRACT_MSB0_UINT (insn, 32, 17, 3); \
{\
  f_op_B = ((f_op__b) | (((f_B_5_3) << (3))));\
}\
  f_go_type = EXTRACT_MSB0_UINT (insn, 32, 8, 2); \
  f_go_op = EXTRACT_MSB0_UINT (insn, 32, 10, 6); \
  f_F = EXTRACT_MSB0_UINT (insn, 32, 16, 1); \
  f_op_Cj = EXTRACT_MSB0_UINT (insn, 32, 20, 6); \
  f_go_cc_type = EXTRACT_MSB0_UINT (insn, 32, 26, 1); \
  f_cond_Q = EXTRACT_MSB0_UINT (insn, 32, 27, 5); \

#define EXTRACT_IFMT_J_L_R_R___RC_ILINK__VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_B_5_3; \
  UINT f_op_B; \
  UINT f_go_type; \
  UINT f_go_op; \
  UINT f_F; \
  UINT f_op_Cj; \
  UINT f_op_A; \
  unsigned int length;
#define EXTRACT_IFMT_J_L_R_R___RC_ILINK__CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_B_5_3 = EXTRACT_MSB0_UINT (insn, 32, 17, 3); \
{\
  f_op_B = ((f_op__b) | (((f_B_5_3) << (3))));\
}\
  f_go_type = EXTRACT_MSB0_UINT (insn, 32, 8, 2); \
  f_go_op = EXTRACT_MSB0_UINT (insn, 32, 10, 6); \
  f_F = EXTRACT_MSB0_UINT (insn, 32, 16, 1); \
  f_op_Cj = EXTRACT_MSB0_UINT (insn, 32, 20, 6); \
  f_op_A = EXTRACT_MSB0_UINT (insn, 32, 26, 6); \

#define EXTRACT_IFMT_J_CC___RC_ILINK__VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_B_5_3; \
  UINT f_op_B; \
  UINT f_go_type; \
  UINT f_go_op; \
  UINT f_F; \
  UINT f_op_Cj; \
  UINT f_go_cc_type; \
  UINT f_cond_Q; \
  unsigned int length;
#define EXTRACT_IFMT_J_CC___RC_ILINK__CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_B_5_3 = EXTRACT_MSB0_UINT (insn, 32, 17, 3); \
{\
  f_op_B = ((f_op__b) | (((f_B_5_3) << (3))));\
}\
  f_go_type = EXTRACT_MSB0_UINT (insn, 32, 8, 2); \
  f_go_op = EXTRACT_MSB0_UINT (insn, 32, 10, 6); \
  f_F = EXTRACT_MSB0_UINT (insn, 32, 16, 1); \
  f_op_Cj = EXTRACT_MSB0_UINT (insn, 32, 20, 6); \
  f_go_cc_type = EXTRACT_MSB0_UINT (insn, 32, 26, 1); \
  f_cond_Q = EXTRACT_MSB0_UINT (insn, 32, 27, 5); \

#define EXTRACT_IFMT_J_L_S12__VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_B_5_3; \
  UINT f_op_B; \
  UINT f_go_type; \
  UINT f_go_op; \
  UINT f_F; \
  UINT f_u6; \
  INT f_s12h; \
  INT f_s12; \
  unsigned int length;
#define EXTRACT_IFMT_J_L_S12__CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_B_5_3 = EXTRACT_MSB0_UINT (insn, 32, 17, 3); \
{\
  f_op_B = ((f_op__b) | (((f_B_5_3) << (3))));\
}\
  f_go_type = EXTRACT_MSB0_UINT (insn, 32, 8, 2); \
  f_go_op = EXTRACT_MSB0_UINT (insn, 32, 10, 6); \
  f_F = EXTRACT_MSB0_UINT (insn, 32, 16, 1); \
  f_u6 = EXTRACT_MSB0_UINT (insn, 32, 20, 6); \
  f_s12h = EXTRACT_MSB0_INT (insn, 32, 26, 6); \
{\
  f_s12 = ((f_u6) | (((f_s12h) << (6))));\
}\

#define EXTRACT_IFMT_J_CCU6__VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_B_5_3; \
  UINT f_op_B; \
  UINT f_go_type; \
  UINT f_go_op; \
  UINT f_F; \
  UINT f_u6; \
  UINT f_go_cc_type; \
  UINT f_cond_Q; \
  unsigned int length;
#define EXTRACT_IFMT_J_CCU6__CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_B_5_3 = EXTRACT_MSB0_UINT (insn, 32, 17, 3); \
{\
  f_op_B = ((f_op__b) | (((f_B_5_3) << (3))));\
}\
  f_go_type = EXTRACT_MSB0_UINT (insn, 32, 8, 2); \
  f_go_op = EXTRACT_MSB0_UINT (insn, 32, 10, 6); \
  f_F = EXTRACT_MSB0_UINT (insn, 32, 16, 1); \
  f_u6 = EXTRACT_MSB0_UINT (insn, 32, 20, 6); \
  f_go_cc_type = EXTRACT_MSB0_UINT (insn, 32, 26, 1); \
  f_cond_Q = EXTRACT_MSB0_UINT (insn, 32, 27, 5); \

#define EXTRACT_IFMT_J_L_U6__VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_B_5_3; \
  UINT f_op_B; \
  UINT f_go_type; \
  UINT f_go_op; \
  UINT f_F; \
  UINT f_u6; \
  UINT f_op_A; \
  unsigned int length;
#define EXTRACT_IFMT_J_L_U6__CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_B_5_3 = EXTRACT_MSB0_UINT (insn, 32, 17, 3); \
{\
  f_op_B = ((f_op__b) | (((f_B_5_3) << (3))));\
}\
  f_go_type = EXTRACT_MSB0_UINT (insn, 32, 8, 2); \
  f_go_op = EXTRACT_MSB0_UINT (insn, 32, 10, 6); \
  f_F = EXTRACT_MSB0_UINT (insn, 32, 16, 1); \
  f_u6 = EXTRACT_MSB0_UINT (insn, 32, 20, 6); \
  f_op_A = EXTRACT_MSB0_UINT (insn, 32, 26, 6); \

#define EXTRACT_IFMT_J_S__S_VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_op__c; \
  UINT f_i16_go; \
  UINT f_dummy; \
  unsigned int length;
#define EXTRACT_IFMT_J_S__S_CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_op__c = EXTRACT_MSB0_UINT (insn, 32, 8, 3); \
  f_i16_go = EXTRACT_MSB0_UINT (insn, 32, 11, 5); \
  f_dummy = EXTRACT_MSB0_UINT (insn, 32, 16, 16); \

#define EXTRACT_IFMT_J_L_R_R_D___RC__VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_B_5_3; \
  UINT f_op_B; \
  UINT f_go_type; \
  UINT f_go_op; \
  UINT f_F; \
  UINT f_op_C; \
  UINT f_op_A; \
  unsigned int length;
#define EXTRACT_IFMT_J_L_R_R_D___RC__CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_B_5_3 = EXTRACT_MSB0_UINT (insn, 32, 17, 3); \
{\
  f_op_B = ((f_op__b) | (((f_B_5_3) << (3))));\
}\
  f_go_type = EXTRACT_MSB0_UINT (insn, 32, 8, 2); \
  f_go_op = EXTRACT_MSB0_UINT (insn, 32, 10, 6); \
  f_F = EXTRACT_MSB0_UINT (insn, 32, 16, 1); \
  f_op_C = EXTRACT_MSB0_UINT (insn, 32, 20, 6); \
  f_op_A = EXTRACT_MSB0_UINT (insn, 32, 26, 6); \

#define EXTRACT_IFMT_J_CC_D___RC__VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_B_5_3; \
  UINT f_op_B; \
  UINT f_go_type; \
  UINT f_go_op; \
  UINT f_F; \
  UINT f_op_C; \
  UINT f_go_cc_type; \
  UINT f_cond_Q; \
  unsigned int length;
#define EXTRACT_IFMT_J_CC_D___RC__CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_B_5_3 = EXTRACT_MSB0_UINT (insn, 32, 17, 3); \
{\
  f_op_B = ((f_op__b) | (((f_B_5_3) << (3))));\
}\
  f_go_type = EXTRACT_MSB0_UINT (insn, 32, 8, 2); \
  f_go_op = EXTRACT_MSB0_UINT (insn, 32, 10, 6); \
  f_F = EXTRACT_MSB0_UINT (insn, 32, 16, 1); \
  f_op_C = EXTRACT_MSB0_UINT (insn, 32, 20, 6); \
  f_go_cc_type = EXTRACT_MSB0_UINT (insn, 32, 26, 1); \
  f_cond_Q = EXTRACT_MSB0_UINT (insn, 32, 27, 5); \

#define EXTRACT_IFMT_LP_L_S12__VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_B_5_3; \
  UINT f_op_B; \
  UINT f_go_type; \
  UINT f_go_op; \
  UINT f_F; \
  UINT f_u6; \
  INT f_s12h; \
  INT f_s12x2; \
  unsigned int length;
#define EXTRACT_IFMT_LP_L_S12__CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_B_5_3 = EXTRACT_MSB0_UINT (insn, 32, 17, 3); \
{\
  f_op_B = ((f_op__b) | (((f_B_5_3) << (3))));\
}\
  f_go_type = EXTRACT_MSB0_UINT (insn, 32, 8, 2); \
  f_go_op = EXTRACT_MSB0_UINT (insn, 32, 10, 6); \
  f_F = EXTRACT_MSB0_UINT (insn, 32, 16, 1); \
  f_u6 = EXTRACT_MSB0_UINT (insn, 32, 20, 6); \
  f_s12h = EXTRACT_MSB0_INT (insn, 32, 26, 6); \
{\
  f_s12x2 = ((((f_u6) << (1))) | (((f_s12h) << (7))));\
}\

#define EXTRACT_IFMT_LPCC_CCU6_VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_B_5_3; \
  UINT f_op_B; \
  UINT f_go_type; \
  UINT f_go_op; \
  UINT f_F; \
  SI f_u6x2; \
  UINT f_go_cc_type; \
  UINT f_cond_Q; \
  unsigned int length;
#define EXTRACT_IFMT_LPCC_CCU6_CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_B_5_3 = EXTRACT_MSB0_UINT (insn, 32, 17, 3); \
{\
  f_op_B = ((f_op__b) | (((f_B_5_3) << (3))));\
}\
  f_go_type = EXTRACT_MSB0_UINT (insn, 32, 8, 2); \
  f_go_op = EXTRACT_MSB0_UINT (insn, 32, 10, 6); \
  f_F = EXTRACT_MSB0_UINT (insn, 32, 16, 1); \
  f_u6x2 = ((EXTRACT_MSB0_UINT (insn, 32, 20, 6)) << (1)); \
  f_go_cc_type = EXTRACT_MSB0_UINT (insn, 32, 26, 1); \
  f_cond_Q = EXTRACT_MSB0_UINT (insn, 32, 27, 5); \

#define EXTRACT_IFMT_LR_L_R_R___RC__VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_B_5_3; \
  UINT f_op_B; \
  UINT f_go_type; \
  UINT f_go_op; \
  UINT f_F; \
  UINT f_op_C; \
  UINT f_op_A; \
  unsigned int length;
#define EXTRACT_IFMT_LR_L_R_R___RC__CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_B_5_3 = EXTRACT_MSB0_UINT (insn, 32, 17, 3); \
{\
  f_op_B = ((f_op__b) | (((f_B_5_3) << (3))));\
}\
  f_go_type = EXTRACT_MSB0_UINT (insn, 32, 8, 2); \
  f_go_op = EXTRACT_MSB0_UINT (insn, 32, 10, 6); \
  f_F = EXTRACT_MSB0_UINT (insn, 32, 16, 1); \
  f_op_C = EXTRACT_MSB0_UINT (insn, 32, 20, 6); \
  f_op_A = EXTRACT_MSB0_UINT (insn, 32, 26, 6); \

#define EXTRACT_IFMT_LR_L_S12__VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_B_5_3; \
  UINT f_op_B; \
  UINT f_go_type; \
  UINT f_go_op; \
  UINT f_F; \
  UINT f_u6; \
  INT f_s12h; \
  INT f_s12; \
  unsigned int length;
#define EXTRACT_IFMT_LR_L_S12__CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_B_5_3 = EXTRACT_MSB0_UINT (insn, 32, 17, 3); \
{\
  f_op_B = ((f_op__b) | (((f_B_5_3) << (3))));\
}\
  f_go_type = EXTRACT_MSB0_UINT (insn, 32, 8, 2); \
  f_go_op = EXTRACT_MSB0_UINT (insn, 32, 10, 6); \
  f_F = EXTRACT_MSB0_UINT (insn, 32, 16, 1); \
  f_u6 = EXTRACT_MSB0_UINT (insn, 32, 20, 6); \
  f_s12h = EXTRACT_MSB0_INT (insn, 32, 26, 6); \
{\
  f_s12 = ((f_u6) | (((f_s12h) << (6))));\
}\

#define EXTRACT_IFMT_LR_L_U6__VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_B_5_3; \
  UINT f_op_B; \
  UINT f_go_type; \
  UINT f_go_op; \
  UINT f_F; \
  UINT f_u6; \
  UINT f_op_A; \
  unsigned int length;
#define EXTRACT_IFMT_LR_L_U6__CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_B_5_3 = EXTRACT_MSB0_UINT (insn, 32, 17, 3); \
{\
  f_op_B = ((f_op__b) | (((f_B_5_3) << (3))));\
}\
  f_go_type = EXTRACT_MSB0_UINT (insn, 32, 8, 2); \
  f_go_op = EXTRACT_MSB0_UINT (insn, 32, 10, 6); \
  f_F = EXTRACT_MSB0_UINT (insn, 32, 16, 1); \
  f_u6 = EXTRACT_MSB0_UINT (insn, 32, 20, 6); \
  f_op_A = EXTRACT_MSB0_UINT (insn, 32, 26, 6); \

#define EXTRACT_IFMT_ASL_L_R_R__RC_VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_B_5_3; \
  UINT f_op_B; \
  UINT f_go_type; \
  UINT f_go_op; \
  UINT f_F; \
  UINT f_op_C; \
  UINT f_op_A; \
  unsigned int length;
#define EXTRACT_IFMT_ASL_L_R_R__RC_CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_B_5_3 = EXTRACT_MSB0_UINT (insn, 32, 17, 3); \
{\
  f_op_B = ((f_op__b) | (((f_B_5_3) << (3))));\
}\
  f_go_type = EXTRACT_MSB0_UINT (insn, 32, 8, 2); \
  f_go_op = EXTRACT_MSB0_UINT (insn, 32, 10, 6); \
  f_F = EXTRACT_MSB0_UINT (insn, 32, 16, 1); \
  f_op_C = EXTRACT_MSB0_UINT (insn, 32, 20, 6); \
  f_op_A = EXTRACT_MSB0_UINT (insn, 32, 26, 6); \

#define EXTRACT_IFMT_ASL_L_U6__VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_B_5_3; \
  UINT f_op_B; \
  UINT f_go_type; \
  UINT f_go_op; \
  UINT f_F; \
  UINT f_u6; \
  UINT f_op_A; \
  unsigned int length;
#define EXTRACT_IFMT_ASL_L_U6__CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_B_5_3 = EXTRACT_MSB0_UINT (insn, 32, 17, 3); \
{\
  f_op_B = ((f_op__b) | (((f_B_5_3) << (3))));\
}\
  f_go_type = EXTRACT_MSB0_UINT (insn, 32, 8, 2); \
  f_go_op = EXTRACT_MSB0_UINT (insn, 32, 10, 6); \
  f_F = EXTRACT_MSB0_UINT (insn, 32, 16, 1); \
  f_u6 = EXTRACT_MSB0_UINT (insn, 32, 20, 6); \
  f_op_A = EXTRACT_MSB0_UINT (insn, 32, 26, 6); \

#define EXTRACT_IFMT_EX_L_R_R__RC_VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_B_5_3; \
  UINT f_op_B; \
  UINT f_go_type; \
  UINT f_go_op; \
  UINT f_F; \
  UINT f_op_C; \
  UINT f_op_A; \
  unsigned int length;
#define EXTRACT_IFMT_EX_L_R_R__RC_CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_B_5_3 = EXTRACT_MSB0_UINT (insn, 32, 17, 3); \
{\
  f_op_B = ((f_op__b) | (((f_B_5_3) << (3))));\
}\
  f_go_type = EXTRACT_MSB0_UINT (insn, 32, 8, 2); \
  f_go_op = EXTRACT_MSB0_UINT (insn, 32, 10, 6); \
  f_F = EXTRACT_MSB0_UINT (insn, 32, 16, 1); \
  f_op_C = EXTRACT_MSB0_UINT (insn, 32, 20, 6); \
  f_op_A = EXTRACT_MSB0_UINT (insn, 32, 26, 6); \

#define EXTRACT_IFMT_EX_L_U6__VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_B_5_3; \
  UINT f_op_B; \
  UINT f_go_type; \
  UINT f_go_op; \
  UINT f_F; \
  UINT f_u6; \
  UINT f_op_A; \
  unsigned int length;
#define EXTRACT_IFMT_EX_L_U6__CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_B_5_3 = EXTRACT_MSB0_UINT (insn, 32, 17, 3); \
{\
  f_op_B = ((f_op__b) | (((f_B_5_3) << (3))));\
}\
  f_go_type = EXTRACT_MSB0_UINT (insn, 32, 8, 2); \
  f_go_op = EXTRACT_MSB0_UINT (insn, 32, 10, 6); \
  f_F = EXTRACT_MSB0_UINT (insn, 32, 16, 1); \
  f_u6 = EXTRACT_MSB0_UINT (insn, 32, 20, 6); \
  f_op_A = EXTRACT_MSB0_UINT (insn, 32, 26, 6); \

#define EXTRACT_IFMT_SWI_VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_go_type; \
  UINT f_go_op; \
  UINT f_F; \
  UINT f_B_5_3; \
  UINT f_op_C; \
  UINT f_op_A; \
  unsigned int length;
#define EXTRACT_IFMT_SWI_CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_go_type = EXTRACT_MSB0_UINT (insn, 32, 8, 2); \
  f_go_op = EXTRACT_MSB0_UINT (insn, 32, 10, 6); \
  f_F = EXTRACT_MSB0_UINT (insn, 32, 16, 1); \
  f_B_5_3 = EXTRACT_MSB0_UINT (insn, 32, 17, 3); \
  f_op_C = EXTRACT_MSB0_UINT (insn, 32, 20, 6); \
  f_op_A = EXTRACT_MSB0_UINT (insn, 32, 26, 6); \

#define EXTRACT_IFMT_TRAP_S_VARS \
  UINT f_opm; \
  UINT f_trapnum; \
  UINT f_i16_go; \
  UINT f_dummy; \
  unsigned int length;
#define EXTRACT_IFMT_TRAP_S_CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_trapnum = EXTRACT_MSB0_UINT (insn, 32, 5, 6); \
  f_i16_go = EXTRACT_MSB0_UINT (insn, 32, 11, 5); \
  f_dummy = EXTRACT_MSB0_UINT (insn, 32, 16, 16); \

#define EXTRACT_IFMT_BRK_S_VARS \
  UINT f_opm; \
  UINT f_trapnum; \
  UINT f_i16_go; \
  UINT f_dummy; \
  unsigned int length;
#define EXTRACT_IFMT_BRK_S_CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_trapnum = EXTRACT_MSB0_UINT (insn, 32, 5, 6); \
  f_i16_go = EXTRACT_MSB0_UINT (insn, 32, 11, 5); \
  f_dummy = EXTRACT_MSB0_UINT (insn, 32, 16, 16); \

#define EXTRACT_IFMT_DIVAW_CCU6__RA__VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_B_5_3; \
  UINT f_op_B; \
  UINT f_go_type; \
  UINT f_go_op; \
  UINT f_F; \
  UINT f_u6; \
  UINT f_go_cc_type; \
  UINT f_cond_Q; \
  unsigned int length;
#define EXTRACT_IFMT_DIVAW_CCU6__RA__CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_B_5_3 = EXTRACT_MSB0_UINT (insn, 32, 17, 3); \
{\
  f_op_B = ((f_op__b) | (((f_B_5_3) << (3))));\
}\
  f_go_type = EXTRACT_MSB0_UINT (insn, 32, 8, 2); \
  f_go_op = EXTRACT_MSB0_UINT (insn, 32, 10, 6); \
  f_F = EXTRACT_MSB0_UINT (insn, 32, 16, 1); \
  f_u6 = EXTRACT_MSB0_UINT (insn, 32, 20, 6); \
  f_go_cc_type = EXTRACT_MSB0_UINT (insn, 32, 26, 1); \
  f_cond_Q = EXTRACT_MSB0_UINT (insn, 32, 27, 5); \

#define EXTRACT_IFMT_DIVAW_L_U6__RA__VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_B_5_3; \
  UINT f_op_B; \
  UINT f_go_type; \
  UINT f_go_op; \
  UINT f_F; \
  UINT f_u6; \
  UINT f_op_A; \
  unsigned int length;
#define EXTRACT_IFMT_DIVAW_L_U6__RA__CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_B_5_3 = EXTRACT_MSB0_UINT (insn, 32, 17, 3); \
{\
  f_op_B = ((f_op__b) | (((f_B_5_3) << (3))));\
}\
  f_go_type = EXTRACT_MSB0_UINT (insn, 32, 8, 2); \
  f_go_op = EXTRACT_MSB0_UINT (insn, 32, 10, 6); \
  f_F = EXTRACT_MSB0_UINT (insn, 32, 16, 1); \
  f_u6 = EXTRACT_MSB0_UINT (insn, 32, 20, 6); \
  f_op_A = EXTRACT_MSB0_UINT (insn, 32, 26, 6); \

#define EXTRACT_IFMT_DIVAW_L_R_R__RA__RC_VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_B_5_3; \
  UINT f_op_B; \
  UINT f_go_type; \
  UINT f_go_op; \
  UINT f_F; \
  UINT f_op_C; \
  UINT f_op_A; \
  unsigned int length;
#define EXTRACT_IFMT_DIVAW_L_R_R__RA__RC_CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_B_5_3 = EXTRACT_MSB0_UINT (insn, 32, 17, 3); \
{\
  f_op_B = ((f_op__b) | (((f_B_5_3) << (3))));\
}\
  f_go_type = EXTRACT_MSB0_UINT (insn, 32, 8, 2); \
  f_go_op = EXTRACT_MSB0_UINT (insn, 32, 10, 6); \
  f_F = EXTRACT_MSB0_UINT (insn, 32, 16, 1); \
  f_op_C = EXTRACT_MSB0_UINT (insn, 32, 20, 6); \
  f_op_A = EXTRACT_MSB0_UINT (insn, 32, 26, 6); \

#define EXTRACT_IFMT_DIVAW_CC__RA__RC_VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_B_5_3; \
  UINT f_op_B; \
  UINT f_go_type; \
  UINT f_go_op; \
  UINT f_F; \
  UINT f_op_C; \
  UINT f_go_cc_type; \
  UINT f_cond_Q; \
  unsigned int length;
#define EXTRACT_IFMT_DIVAW_CC__RA__RC_CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_B_5_3 = EXTRACT_MSB0_UINT (insn, 32, 17, 3); \
{\
  f_op_B = ((f_op__b) | (((f_B_5_3) << (3))));\
}\
  f_go_type = EXTRACT_MSB0_UINT (insn, 32, 8, 2); \
  f_go_op = EXTRACT_MSB0_UINT (insn, 32, 10, 6); \
  f_F = EXTRACT_MSB0_UINT (insn, 32, 16, 1); \
  f_op_C = EXTRACT_MSB0_UINT (insn, 32, 20, 6); \
  f_go_cc_type = EXTRACT_MSB0_UINT (insn, 32, 26, 1); \
  f_cond_Q = EXTRACT_MSB0_UINT (insn, 32, 27, 5); \

#define EXTRACT_IFMT_POP_S_B_VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_op__c; \
  UINT f_u5; \
  UINT f_dummy; \
  unsigned int length;
#define EXTRACT_IFMT_POP_S_B_CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_op__c = EXTRACT_MSB0_UINT (insn, 32, 8, 3); \
  f_u5 = EXTRACT_MSB0_UINT (insn, 32, 11, 5); \
  f_dummy = EXTRACT_MSB0_UINT (insn, 32, 16, 16); \

#define EXTRACT_IFMT_POP_S_BLINK_VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_op__c; \
  UINT f_u5; \
  UINT f_dummy; \
  unsigned int length;
#define EXTRACT_IFMT_POP_S_BLINK_CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_op__c = EXTRACT_MSB0_UINT (insn, 32, 8, 3); \
  f_u5 = EXTRACT_MSB0_UINT (insn, 32, 11, 5); \
  f_dummy = EXTRACT_MSB0_UINT (insn, 32, 16, 16); \

#define EXTRACT_IFMT_CURRENT_LOOP_END_VARS \
  UINT f_opm; \
  UINT f_op__b; \
  UINT f_B_5_3; \
  UINT f_op_B; \
  UINT f_go_type; \
  UINT f_go_op; \
  UINT f_F; \
  UINT f_op_C; \
  UINT f_op_A; \
  unsigned int length;
#define EXTRACT_IFMT_CURRENT_LOOP_END_CODE \
  length = 4; \
  f_opm = EXTRACT_MSB0_UINT (insn, 32, 0, 5); \
  f_op__b = EXTRACT_MSB0_UINT (insn, 32, 5, 3); \
  f_B_5_3 = EXTRACT_MSB0_UINT (insn, 32, 17, 3); \
{\
  f_op_B = ((f_op__b) | (((f_B_5_3) << (3))));\
}\
  f_go_type = EXTRACT_MSB0_UINT (insn, 32, 8, 2); \
  f_go_op = EXTRACT_MSB0_UINT (insn, 32, 10, 6); \
  f_F = EXTRACT_MSB0_UINT (insn, 32, 16, 1); \
  f_op_C = EXTRACT_MSB0_UINT (insn, 32, 20, 6); \
  f_op_A = EXTRACT_MSB0_UINT (insn, 32, 26, 6); \

/* Collection of various things for the trace handler to use.  */

typedef struct trace_record {
  IADDR pc;
  /* FIXME:wip */
} TRACE_RECORD;

#endif /* CPU_ARC700F_H */
