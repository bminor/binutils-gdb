/* CPU family header for m32r.

This file is machine generated with CGEN.

Copyright (C) 1996, 1997, 1998 Free Software Foundation, Inc.

This file is part of the GNU Simulators.

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
59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

*/

#ifndef CPU_M32R_H
#define CPU_M32R_H

/* Maximum number of instructions that are fetched at a time.
   This is for LIW type instructions sets (e.g. m32r).  */
#define MAX_LIW_INSNS 2

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
  /* general registers */
  SI h_gr[16];
#define GET_H_GR(a1) CPU (h_gr)[a1]
#define SET_H_GR(a1, x) (CPU (h_gr)[a1] = (x))
  /* control registers */
  USI h_cr[7];
#define GET_H_CR(a1) CPU (h_cr)[a1]
#define SET_H_CR(a1, x) (CPU (h_cr)[a1] = (x))
  /* accumulator */
  DI h_accum;
#define GET_H_ACCUM() CPU (h_accum)
#define SET_H_ACCUM(x) (CPU (h_accum) = (x))
/* start-sanitize-m32rx */
  /* accumulators */
  DI h_accums[2];
/* end-sanitize-m32rx */
#define GET_H_ACCUMS(a1) CPU (h_accums)[a1]
#define SET_H_ACCUMS(a1, x) (CPU (h_accums)[a1] = (x))
/* start-sanitize-m32rx */
  /* abort flag */
  UBI h_abort;
/* end-sanitize-m32rx */
#define GET_H_ABORT() CPU (h_abort)
#define SET_H_ABORT(x) (CPU (h_abort) = (x))
  /* condition bit */
  UBI h_cond;
#define GET_H_COND() CPU (h_cond)
#define SET_H_COND(x) (CPU (h_cond) = (x))
  /* sm */
  UBI h_sm;
#define GET_H_SM() CPU (h_sm)
#define SET_H_SM(x) (CPU (h_sm) = (x))
  /* bsm */
  UBI h_bsm;
#define GET_H_BSM() CPU (h_bsm)
#define SET_H_BSM(x) (CPU (h_bsm) = (x))
  /* ie */
  UBI h_ie;
#define GET_H_IE() CPU (h_ie)
#define SET_H_IE(x) (CPU (h_ie) = (x))
  /* bie */
  UBI h_bie;
#define GET_H_BIE() CPU (h_bie)
#define SET_H_BIE(x) (CPU (h_bie) = (x))
  /* bcond */
  UBI h_bcond;
#define GET_H_BCOND() CPU (h_bcond)
#define SET_H_BCOND(x) (CPU (h_bcond) = (x))
  /* bpc */
  SI h_bpc;
#define GET_H_BPC() CPU (h_bpc)
#define SET_H_BPC(x) (CPU (h_bpc) = (x))
  /* lock */
  UBI h_lock;
#define GET_H_LOCK() CPU (h_lock)
#define SET_H_LOCK(x) (CPU (h_lock) = (x))
  } hardware;
#define CPU_CGEN_HW(cpu) (& (cpu)->cpu_data.hardware)
  /* CPU profiling state information.  */
  struct {
  /* general registers */
  unsigned long h_gr;
  } profile;
#define CPU_CGEN_PROFILE(cpu) (& (cpu)->cpu_data.profile)
} M32R_CPU_DATA;

USI m32r_h_pc_get (SIM_CPU *);
void m32r_h_pc_set (SIM_CPU *, USI);
SI m32r_h_gr_get (SIM_CPU *, UINT);
void m32r_h_gr_set (SIM_CPU *, UINT, SI);
USI m32r_h_cr_get (SIM_CPU *, UINT);
void m32r_h_cr_set (SIM_CPU *, UINT, USI);
DI m32r_h_accum_get (SIM_CPU *);
void m32r_h_accum_set (SIM_CPU *, DI);
DI m32r_h_accums_get (SIM_CPU *, UINT);
void m32r_h_accums_set (SIM_CPU *, UINT, DI);
UBI m32r_h_abort_get (SIM_CPU *);
void m32r_h_abort_set (SIM_CPU *, UBI);
UBI m32r_h_cond_get (SIM_CPU *);
void m32r_h_cond_set (SIM_CPU *, UBI);
UBI m32r_h_sm_get (SIM_CPU *);
void m32r_h_sm_set (SIM_CPU *, UBI);
UBI m32r_h_bsm_get (SIM_CPU *);
void m32r_h_bsm_set (SIM_CPU *, UBI);
UBI m32r_h_ie_get (SIM_CPU *);
void m32r_h_ie_set (SIM_CPU *, UBI);
UBI m32r_h_bie_get (SIM_CPU *);
void m32r_h_bie_set (SIM_CPU *, UBI);
UBI m32r_h_bcond_get (SIM_CPU *);
void m32r_h_bcond_set (SIM_CPU *, UBI);
SI m32r_h_bpc_get (SIM_CPU *);
void m32r_h_bpc_set (SIM_CPU *, SI);
UBI m32r_h_lock_get (SIM_CPU *);
void m32r_h_lock_set (SIM_CPU *, UBI);
extern DECODE *m32r_decode (SIM_CPU *, PCADDR, insn_t);

/* The ARGBUF struct.  */
struct argbuf {
  /* These are the baseclass definitions.  */
  unsigned int length;
  PCADDR addr;
  const struct cgen_insn *opcode;
#if ! defined (SCACHE_P)
  insn_t insn;
#endif
  /* cpu specific data follows */
  union {
    struct { /* e.g. add $dr,$sr */
      SI * f_r1;
      SI * f_r2;
    } fmt_0_add;
    struct { /* e.g. add3 $dr,$sr,#$slo16 */
      SI * f_r1;
      SI * f_r2;
      HI f_simm16;
    } fmt_1_add3;
    struct { /* e.g. and3 $dr,$sr,#$uimm16 */
      SI * f_r1;
      SI * f_r2;
      USI f_uimm16;
    } fmt_2_and3;
    struct { /* e.g. or3 $dr,$sr,#$ulo16 */
      SI * f_r1;
      SI * f_r2;
      UHI f_uimm16;
    } fmt_3_or3;
    struct { /* e.g. addi $dr,#$simm8 */
      SI * f_r1;
      SI f_simm8;
    } fmt_4_addi;
    struct { /* e.g. addv $dr,$sr */
      SI * f_r1;
      SI * f_r2;
    } fmt_5_addv;
    struct { /* e.g. addv3 $dr,$sr,#$simm16 */
      SI * f_r1;
      SI * f_r2;
      SI f_simm16;
    } fmt_6_addv3;
    struct { /* e.g. addx $dr,$sr */
      SI * f_r1;
      SI * f_r2;
    } fmt_7_addx;
    struct { /* e.g. bc $disp8 */
      IADDR f_disp8;
    } fmt_8_bc8;
    struct { /* e.g. bc $disp24 */
      IADDR f_disp24;
    } fmt_9_bc24;
    struct { /* e.g. beq $src1,$src2,$disp16 */
      SI * f_r1;
      SI * f_r2;
      IADDR f_disp16;
    } fmt_10_beq;
    struct { /* e.g. beqz $src2,$disp16 */
      SI * f_r2;
      IADDR f_disp16;
    } fmt_11_beqz;
    struct { /* e.g. bl $disp8 */
      IADDR f_disp8;
    } fmt_12_bl8;
    struct { /* e.g. bl $disp24 */
      IADDR f_disp24;
    } fmt_13_bl24;
    struct { /* e.g. bra $disp8 */
      IADDR f_disp8;
    } fmt_14_bra8;
    struct { /* e.g. bra $disp24 */
      IADDR f_disp24;
    } fmt_15_bra24;
    struct { /* e.g. cmp $src1,$src2 */
      SI * f_r1;
      SI * f_r2;
    } fmt_16_cmp;
    struct { /* e.g. cmpi $src2,#$simm16 */
      SI * f_r2;
      SI f_simm16;
    } fmt_17_cmpi;
    struct { /* e.g. cmpui $src2,#$uimm16 */
      SI * f_r2;
      USI f_uimm16;
    } fmt_18_cmpui;
    struct { /* e.g. div $dr,$sr */
      SI * f_r1;
      SI * f_r2;
    } fmt_19_div;
    struct { /* e.g. jl $sr */
      SI * f_r2;
    } fmt_20_jl;
    struct { /* e.g. jmp $sr */
      SI * f_r2;
    } fmt_21_jmp;
    struct { /* e.g. ld $dr,@$sr */
      SI * f_r1;
      SI * f_r2;
    } fmt_22_ld;
    struct { /* e.g. ld $dr,@($slo16,$sr) */
      SI * f_r1;
      SI * f_r2;
      HI f_simm16;
    } fmt_23_ld_d;
    struct { /* e.g. ldb $dr,@$sr */
      SI * f_r1;
      SI * f_r2;
    } fmt_24_ldb;
    struct { /* e.g. ldb $dr,@($slo16,$sr) */
      SI * f_r1;
      SI * f_r2;
      HI f_simm16;
    } fmt_25_ldb_d;
    struct { /* e.g. ldh $dr,@$sr */
      SI * f_r1;
      SI * f_r2;
    } fmt_26_ldh;
    struct { /* e.g. ldh $dr,@($slo16,$sr) */
      SI * f_r1;
      SI * f_r2;
      HI f_simm16;
    } fmt_27_ldh_d;
    struct { /* e.g. ld $dr,@$sr+ */
      SI * f_r1;
      SI * f_r2;
    } fmt_28_ld_plus;
    struct { /* e.g. ld24 $dr,#$uimm24 */
      SI * f_r1;
      ADDR f_uimm24;
    } fmt_29_ld24;
    struct { /* e.g. ldi $dr,#$simm8 */
      SI * f_r1;
      SI f_simm8;
    } fmt_30_ldi8;
    struct { /* e.g. ldi $dr,$slo16 */
      SI * f_r1;
      HI f_simm16;
    } fmt_31_ldi16;
    struct { /* e.g. lock $dr,@$sr */
      SI * f_r1;
      SI * f_r2;
    } fmt_32_lock;
    struct { /* e.g. machi $src1,$src2 */
      SI * f_r1;
      SI * f_r2;
    } fmt_33_machi;
    struct { /* e.g. mulhi $src1,$src2 */
      SI * f_r1;
      SI * f_r2;
    } fmt_34_mulhi;
    struct { /* e.g. mv $dr,$sr */
      SI * f_r1;
      SI * f_r2;
    } fmt_35_mv;
    struct { /* e.g. mvfachi $dr */
      SI * f_r1;
    } fmt_36_mvfachi;
    struct { /* e.g. mvfc $dr,$scr */
      SI * f_r1;
      UINT f_r2;
    } fmt_37_mvfc;
    struct { /* e.g. mvtachi $src1 */
      SI * f_r1;
    } fmt_38_mvtachi;
    struct { /* e.g. mvtc $sr,$dcr */
      UINT f_r1;
      SI * f_r2;
    } fmt_39_mvtc;
    struct { /* e.g. nop */
      int empty;
    } fmt_40_nop;
    struct { /* e.g. rac */
      int empty;
    } fmt_41_rac;
    struct { /* e.g. rte */
      int empty;
    } fmt_42_rte;
    struct { /* e.g. seth $dr,#$hi16 */
      SI * f_r1;
      UHI f_hi16;
    } fmt_43_seth;
    struct { /* e.g. sll3 $dr,$sr,#$simm16 */
      SI * f_r1;
      SI * f_r2;
      SI f_simm16;
    } fmt_44_sll3;
    struct { /* e.g. slli $dr,#$uimm5 */
      SI * f_r1;
      USI f_uimm5;
    } fmt_45_slli;
    struct { /* e.g. st $src1,@$src2 */
      SI * f_r1;
      SI * f_r2;
    } fmt_46_st;
    struct { /* e.g. st $src1,@($slo16,$src2) */
      SI * f_r1;
      SI * f_r2;
      HI f_simm16;
    } fmt_47_st_d;
    struct { /* e.g. stb $src1,@$src2 */
      SI * f_r1;
      SI * f_r2;
    } fmt_48_stb;
    struct { /* e.g. stb $src1,@($slo16,$src2) */
      SI * f_r1;
      SI * f_r2;
      HI f_simm16;
    } fmt_49_stb_d;
    struct { /* e.g. sth $src1,@$src2 */
      SI * f_r1;
      SI * f_r2;
    } fmt_50_sth;
    struct { /* e.g. sth $src1,@($slo16,$src2) */
      SI * f_r1;
      SI * f_r2;
      HI f_simm16;
    } fmt_51_sth_d;
    struct { /* e.g. st $src1,@+$src2 */
      SI * f_r1;
      SI * f_r2;
    } fmt_52_st_plus;
    struct { /* e.g. trap #$uimm4 */
      USI f_uimm4;
    } fmt_53_trap;
    struct { /* e.g. unlock $src1,@$src2 */
      SI * f_r1;
      SI * f_r2;
    } fmt_54_unlock;
  } fields;
#if 1 || WITH_PROFILE_MODEL_P /*FIXME:wip*/
  unsigned long h_gr_get;
  unsigned long h_gr_set;
#endif
};

/* A cached insn.
   This is currently also used in the non-scache case.  In this situation we
   assume the cache size is 1, and do a few things a little differently.  */
/* FIXME: non-scache version to be redone.  */

struct scache {
  IADDR next;
  union {
#if ! WITH_SEM_SWITCH_FULL
    SEMANTIC_FN *sem_fn;
#endif
#if ! WITH_SEM_SWITCH_FAST
    SEMANTIC_FN *sem_fast_fn;
#endif
#if WITH_SEM_SWITCH_FULL || WITH_SEM_SWITCH_FAST
#ifdef __GNUC__
    void *sem_case;
#else
    int sem_case;
#endif
#endif
  } semantic;
  struct argbuf argbuf;
};

/* Macros to simplify extraction, reading and semantic code.
   These define and assign the local vars that contain the insn's fields.  */

#define EXTRACT_FMT_0_ADD_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_0_ADD_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_1_ADD3_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  int f_simm16; \
  unsigned int length;
#define EXTRACT_FMT_1_ADD3_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16); \

#define EXTRACT_FMT_2_AND3_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  UINT f_uimm16; \
  unsigned int length;
#define EXTRACT_FMT_2_AND3_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_uimm16 = EXTRACT_UNSIGNED (insn, 32, 16, 16); \

#define EXTRACT_FMT_3_OR3_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  UINT f_uimm16; \
  unsigned int length;
#define EXTRACT_FMT_3_OR3_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_uimm16 = EXTRACT_UNSIGNED (insn, 32, 16, 16); \

#define EXTRACT_FMT_4_ADDI_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  int f_simm8; \
  unsigned int length;
#define EXTRACT_FMT_4_ADDI_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_simm8 = EXTRACT_SIGNED (insn, 16, 8, 8); \

#define EXTRACT_FMT_5_ADDV_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_5_ADDV_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_6_ADDV3_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  int f_simm16; \
  unsigned int length;
#define EXTRACT_FMT_6_ADDV3_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16); \

#define EXTRACT_FMT_7_ADDX_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_7_ADDX_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_8_BC8_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  int f_disp8; \
  unsigned int length;
#define EXTRACT_FMT_8_BC8_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_disp8 = ((EXTRACT_SIGNED (insn, 16, 8, 8)) << (2)); \

#define EXTRACT_FMT_9_BC24_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  int f_disp24; \
  unsigned int length;
#define EXTRACT_FMT_9_BC24_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_disp24 = ((EXTRACT_SIGNED (insn, 32, 8, 24)) << (2)); \

#define EXTRACT_FMT_10_BEQ_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  int f_disp16; \
  unsigned int length;
#define EXTRACT_FMT_10_BEQ_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_disp16 = ((EXTRACT_SIGNED (insn, 32, 16, 16)) << (2)); \

#define EXTRACT_FMT_11_BEQZ_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  int f_disp16; \
  unsigned int length;
#define EXTRACT_FMT_11_BEQZ_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_disp16 = ((EXTRACT_SIGNED (insn, 32, 16, 16)) << (2)); \

#define EXTRACT_FMT_12_BL8_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  int f_disp8; \
  unsigned int length;
#define EXTRACT_FMT_12_BL8_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_disp8 = ((EXTRACT_SIGNED (insn, 16, 8, 8)) << (2)); \

#define EXTRACT_FMT_13_BL24_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  int f_disp24; \
  unsigned int length;
#define EXTRACT_FMT_13_BL24_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_disp24 = ((EXTRACT_SIGNED (insn, 32, 8, 24)) << (2)); \

#define EXTRACT_FMT_14_BRA8_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  int f_disp8; \
  unsigned int length;
#define EXTRACT_FMT_14_BRA8_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_disp8 = ((EXTRACT_SIGNED (insn, 16, 8, 8)) << (2)); \

#define EXTRACT_FMT_15_BRA24_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  int f_disp24; \
  unsigned int length;
#define EXTRACT_FMT_15_BRA24_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_disp24 = ((EXTRACT_SIGNED (insn, 32, 8, 24)) << (2)); \

#define EXTRACT_FMT_16_CMP_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_16_CMP_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_17_CMPI_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  int f_simm16; \
  unsigned int length;
#define EXTRACT_FMT_17_CMPI_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16); \

#define EXTRACT_FMT_18_CMPUI_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  UINT f_uimm16; \
  unsigned int length;
#define EXTRACT_FMT_18_CMPUI_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_uimm16 = EXTRACT_UNSIGNED (insn, 32, 16, 16); \

#define EXTRACT_FMT_19_DIV_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  int f_simm16; \
  unsigned int length;
#define EXTRACT_FMT_19_DIV_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16); \

#define EXTRACT_FMT_20_JL_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_20_JL_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_21_JMP_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_21_JMP_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_22_LD_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_22_LD_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_23_LD_D_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  int f_simm16; \
  unsigned int length;
#define EXTRACT_FMT_23_LD_D_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16); \

#define EXTRACT_FMT_24_LDB_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_24_LDB_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_25_LDB_D_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  int f_simm16; \
  unsigned int length;
#define EXTRACT_FMT_25_LDB_D_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16); \

#define EXTRACT_FMT_26_LDH_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_26_LDH_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_27_LDH_D_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  int f_simm16; \
  unsigned int length;
#define EXTRACT_FMT_27_LDH_D_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16); \

#define EXTRACT_FMT_28_LD_PLUS_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_28_LD_PLUS_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_29_LD24_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_uimm24; \
  unsigned int length;
#define EXTRACT_FMT_29_LD24_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_uimm24 = EXTRACT_UNSIGNED (insn, 32, 8, 24); \

#define EXTRACT_FMT_30_LDI8_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  int f_simm8; \
  unsigned int length;
#define EXTRACT_FMT_30_LDI8_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_simm8 = EXTRACT_SIGNED (insn, 16, 8, 8); \

#define EXTRACT_FMT_31_LDI16_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  int f_simm16; \
  unsigned int length;
#define EXTRACT_FMT_31_LDI16_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16); \

#define EXTRACT_FMT_32_LOCK_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_32_LOCK_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_33_MACHI_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_33_MACHI_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_34_MULHI_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_34_MULHI_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_35_MV_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_35_MV_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_36_MVFACHI_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_36_MVFACHI_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_37_MVFC_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_37_MVFC_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_38_MVTACHI_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_38_MVTACHI_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_39_MVTC_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_39_MVTC_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_40_NOP_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_40_NOP_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_41_RAC_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_41_RAC_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_42_RTE_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_42_RTE_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_43_SETH_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  UINT f_hi16; \
  unsigned int length;
#define EXTRACT_FMT_43_SETH_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_hi16 = EXTRACT_UNSIGNED (insn, 32, 16, 16); \

#define EXTRACT_FMT_44_SLL3_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  int f_simm16; \
  unsigned int length;
#define EXTRACT_FMT_44_SLL3_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16); \

#define EXTRACT_FMT_45_SLLI_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_shift_op2; \
  UINT f_uimm5; \
  unsigned int length;
#define EXTRACT_FMT_45_SLLI_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_shift_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 3); \
  f_uimm5 = EXTRACT_UNSIGNED (insn, 16, 11, 5); \

#define EXTRACT_FMT_46_ST_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_46_ST_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_47_ST_D_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  int f_simm16; \
  unsigned int length;
#define EXTRACT_FMT_47_ST_D_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16); \

#define EXTRACT_FMT_48_STB_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_48_STB_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_49_STB_D_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  int f_simm16; \
  unsigned int length;
#define EXTRACT_FMT_49_STB_D_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16); \

#define EXTRACT_FMT_50_STH_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_50_STH_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_51_STH_D_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  int f_simm16; \
  unsigned int length;
#define EXTRACT_FMT_51_STH_D_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16); \

#define EXTRACT_FMT_52_ST_PLUS_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_52_ST_PLUS_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_53_TRAP_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_uimm4; \
  unsigned int length;
#define EXTRACT_FMT_53_TRAP_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_uimm4 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_54_UNLOCK_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_54_UNLOCK_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#endif /* CPU_M32R_H */
