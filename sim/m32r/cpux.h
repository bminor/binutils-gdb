/* CPU family header for m32rx.

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

#ifndef CPU_M32RX_H
#define CPU_M32RX_H

/* Maximum number of instructions that are fetched at a time.
   This is for LIW type instructions sets (e.g. m32r).  */
#define MAX_LIW_INSNS 2

/* Maximum number of instructions that can be executed in parallel.  */
#define MAX_PARALLEL_INSNS 2

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
  SI h_cr[7];
#define GET_H_CR(a1) CPU (h_cr)[a1]
#define SET_H_CR(a1, x) (CPU (h_cr)[a1] = (x))
  /* accumulator */
  DI h_accum;
#define GET_H_ACCUM() CPU (h_accum)
#define SET_H_ACCUM(x) (CPU (h_accum) = (x))
  /* accumulators */
  DI h_accums[2];
#define GET_H_ACCUMS(a1) CPU (h_accums)[a1]
#define SET_H_ACCUMS(a1, x) (CPU (h_accums)[a1] = (x))
  /* abort flag */
  UBI h_abort;
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
  } hardware;
#define CPU_CGEN_HW(cpu) (& (cpu)->cpu_data.hardware)
  /* CPU profiling state information.  */
  struct {
  /* general registers */
  unsigned long h_gr;
  } profile;
#define CPU_CGEN_PROFILE(cpu) (& (cpu)->cpu_data.profile)
} M32RX_CPU_DATA;

/* FIXME: length parm to decode() is currently unneeded.  */
extern DECODE *m32rx_decode (SIM_CPU *, insn_t /*, int*/);

/* The ARGBUF struct.  */
struct argbuf {
  /* These are the baseclass definitions.  */
  unsigned int length;
  PCADDR addr;
  const struct cgen_insn *opcode;
  /* unsigned long insn; - no longer needed */
  /* cpu specific data follows */
  union {
    struct { /* e.g. add $dr,$sr */
      UINT f_r1;
      UINT f_r2;
    } fmt_0_add;
    struct { /* e.g. add3 $dr,$sr,#$slo16 */
      UINT f_r1;
      UINT f_r2;
      HI f_simm16;
    } fmt_1_add3;
    struct { /* e.g. and3 $dr,$sr,#$uimm16 */
      UINT f_r1;
      UINT f_r2;
      USI f_uimm16;
    } fmt_2_and3;
    struct { /* e.g. or3 $dr,$sr,#$ulo16 */
      UINT f_r1;
      UINT f_r2;
      UHI f_uimm16;
    } fmt_3_or3;
    struct { /* e.g. addi $dr,#$simm8 */
      UINT f_r1;
      SI f_simm8;
    } fmt_4_addi;
    struct { /* e.g. addv3 $dr,$sr,#$simm16 */
      UINT f_r1;
      UINT f_r2;
      SI f_simm16;
    } fmt_5_addv3;
    struct { /* e.g. addx $dr,$sr */
      UINT f_r1;
      UINT f_r2;
    } fmt_6_addx;
    struct { /* e.g. bc $disp8 */
      IADDR f_disp8;
    } fmt_7_bc8;
    struct { /* e.g. bc $disp24 */
      IADDR f_disp24;
    } fmt_8_bc24;
    struct { /* e.g. beq $src1,$src2,$disp16 */
      UINT f_r1;
      UINT f_r2;
      IADDR f_disp16;
    } fmt_9_beq;
    struct { /* e.g. beqz $src2,$disp16 */
      UINT f_r2;
      IADDR f_disp16;
    } fmt_10_beqz;
    struct { /* e.g. bl $disp8 */
      IADDR f_disp8;
    } fmt_11_bl8;
    struct { /* e.g. bl $disp24 */
      IADDR f_disp24;
    } fmt_12_bl24;
    struct { /* e.g. bcl $disp8 */
      IADDR f_disp8;
    } fmt_13_bcl8;
    struct { /* e.g. bcl $disp24 */
      IADDR f_disp24;
    } fmt_14_bcl24;
    struct { /* e.g. bra $disp8 */
      IADDR f_disp8;
    } fmt_15_bra8;
    struct { /* e.g. bra $disp24 */
      IADDR f_disp24;
    } fmt_16_bra24;
    struct { /* e.g. cmp $src1,$src2 */
      UINT f_r1;
      UINT f_r2;
    } fmt_17_cmp;
    struct { /* e.g. cmpi $src2,#$simm16 */
      UINT f_r2;
      SI f_simm16;
    } fmt_18_cmpi;
    struct { /* e.g. cmpui $src2,#$uimm16 */
      UINT f_r2;
      USI f_uimm16;
    } fmt_19_cmpui;
    struct { /* e.g. cmpz $src2 */
      UINT f_r2;
    } fmt_20_cmpz;
    struct { /* e.g. div $dr,$sr */
      UINT f_r1;
      UINT f_r2;
    } fmt_21_div;
    struct { /* e.g. jc $sr */
      UINT f_r2;
    } fmt_22_jc;
    struct { /* e.g. jl $sr */
      UINT f_r2;
    } fmt_23_jl;
    struct { /* e.g. jmp $sr */
      UINT f_r2;
    } fmt_24_jmp;
    struct { /* e.g. ld $dr,@$sr */
      UINT f_r1;
      UINT f_r2;
    } fmt_25_ld;
    struct { /* e.g. ld $dr,@($slo16,$sr) */
      UINT f_r1;
      UINT f_r2;
      HI f_simm16;
    } fmt_26_ld_d;
    struct { /* e.g. ldb $dr,@$sr */
      UINT f_r1;
      UINT f_r2;
    } fmt_27_ldb;
    struct { /* e.g. ldb $dr,@($slo16,$sr) */
      UINT f_r1;
      UINT f_r2;
      HI f_simm16;
    } fmt_28_ldb_d;
    struct { /* e.g. ldh $dr,@$sr */
      UINT f_r1;
      UINT f_r2;
    } fmt_29_ldh;
    struct { /* e.g. ldh $dr,@($slo16,$sr) */
      UINT f_r1;
      UINT f_r2;
      HI f_simm16;
    } fmt_30_ldh_d;
    struct { /* e.g. ld24 $dr,#$uimm24 */
      UINT f_r1;
      ADDR f_uimm24;
    } fmt_31_ld24;
    struct { /* e.g. ldi $dr,#$simm8 */
      UINT f_r1;
      SI f_simm8;
    } fmt_32_ldi8;
    struct { /* e.g. ldi $dr,$slo16 */
      UINT f_r1;
      HI f_simm16;
    } fmt_33_ldi16;
    struct { /* e.g. machi $src1,$src2 */
      UINT f_r1;
      UINT f_r2;
    } fmt_34_machi;
    struct { /* e.g. machi $src1,$src2,$acc */
      UINT f_r1;
      UINT f_acc;
      UINT f_r2;
    } fmt_35_machi_a;
    struct { /* e.g. mulhi $src1,$src2,$acc */
      UINT f_r1;
      UINT f_acc;
      UINT f_r2;
    } fmt_36_mulhi_a;
    struct { /* e.g. mv $dr,$sr */
      UINT f_r1;
      UINT f_r2;
    } fmt_37_mv;
    struct { /* e.g. mvfachi $dr */
      UINT f_r1;
    } fmt_38_mvfachi;
    struct { /* e.g. mvfachi $dr,$accs */
      UINT f_r1;
      UINT f_accs;
    } fmt_39_mvfachi_a;
    struct { /* e.g. mvfc $dr,$scr */
      UINT f_r1;
      UINT f_r2;
    } fmt_40_mvfc;
    struct { /* e.g. mvtachi $src1 */
      UINT f_r1;
    } fmt_41_mvtachi;
    struct { /* e.g. mvtachi $src1,$accs */
      UINT f_r1;
      UINT f_accs;
    } fmt_42_mvtachi_a;
    struct { /* e.g. mvtc $sr,$dcr */
      UINT f_r1;
      UINT f_r2;
    } fmt_43_mvtc;
    struct { /* e.g. nop */
      int empty;
    } fmt_44_nop;
    struct { /* e.g. rac */
      int empty;
    } fmt_45_rac;
    struct { /* e.g. rac $accs */
      UINT f_accs;
    } fmt_46_rac_a;
    struct { /* e.g. seth $dr,$hi16 */
      UINT f_r1;
      UHI f_hi16;
    } fmt_47_seth;
    struct { /* e.g. slli $dr,#$uimm5 */
      UINT f_r1;
      USI f_uimm5;
    } fmt_48_slli;
    struct { /* e.g. st $src1,@($slo16,$src2) */
      UINT f_r1;
      UINT f_r2;
      HI f_simm16;
    } fmt_49_st_d;
    struct { /* e.g. trap #$uimm4 */
      USI f_uimm4;
    } fmt_50_trap;
    struct { /* e.g. satb $dr,$src2 */
      UINT f_r1;
      UINT f_r2;
    } fmt_51_satb;
    struct { /* e.g. pcmpbz $src2 */
      UINT f_r2;
    } fmt_52_pcmpbz;
    struct { /* e.g. sadd */
      int empty;
    } fmt_53_sadd;
    struct { /* e.g. macwu1 $src1,$src2 */
      UINT f_r1;
      UINT f_r2;
    } fmt_54_macwu1;
    struct { /* e.g. sc */
      int empty;
    } fmt_55_sc;
  } fields;
#if 1 || WITH_PROFILE_MODEL_P /*FIXME:wip*/
  unsigned long h_gr_get;
  unsigned long h_gr_set;
#endif
};

/* A cached insn.
   This is also used in the non-scache case.  In this situation we assume
   the cache size is 1, and do a few things a little differently.  */

struct scache {
  IADDR next;
  union {
#if ! WITH_SEM_SWITCH_FULL
    SEMANTIC_FN *sem_fn;
#endif
#if ! WITH_SEM_SWITCH_FAST
#if WITH_SCACHE
    SEMANTIC_CACHE_FN *sem_fast_fn;
#else
    SEMANTIC_FN *sem_fast_fn;
#endif
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

#define EXTRACT_FMT_5_ADDV3_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  int f_simm16; \
  unsigned int length;
#define EXTRACT_FMT_5_ADDV3_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16); \

#define EXTRACT_FMT_6_ADDX_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_6_ADDX_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_7_BC8_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  int f_disp8; \
  unsigned int length;
#define EXTRACT_FMT_7_BC8_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_disp8 = EXTRACT_SIGNED (insn, 16, 8, 8) << 2; \

#define EXTRACT_FMT_8_BC24_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  int f_disp24; \
  unsigned int length;
#define EXTRACT_FMT_8_BC24_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_disp24 = EXTRACT_SIGNED (insn, 32, 8, 24) << 2; \

#define EXTRACT_FMT_9_BEQ_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  int f_disp16; \
  unsigned int length;
#define EXTRACT_FMT_9_BEQ_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_disp16 = EXTRACT_SIGNED (insn, 32, 16, 16) << 2; \

#define EXTRACT_FMT_10_BEQZ_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  int f_disp16; \
  unsigned int length;
#define EXTRACT_FMT_10_BEQZ_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_disp16 = EXTRACT_SIGNED (insn, 32, 16, 16) << 2; \

#define EXTRACT_FMT_11_BL8_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  int f_disp8; \
  unsigned int length;
#define EXTRACT_FMT_11_BL8_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_disp8 = EXTRACT_SIGNED (insn, 16, 8, 8) << 2; \

#define EXTRACT_FMT_12_BL24_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  int f_disp24; \
  unsigned int length;
#define EXTRACT_FMT_12_BL24_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_disp24 = EXTRACT_SIGNED (insn, 32, 8, 24) << 2; \

#define EXTRACT_FMT_13_BCL8_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  int f_disp8; \
  unsigned int length;
#define EXTRACT_FMT_13_BCL8_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_disp8 = EXTRACT_SIGNED (insn, 16, 8, 8) << 2; \

#define EXTRACT_FMT_14_BCL24_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  int f_disp24; \
  unsigned int length;
#define EXTRACT_FMT_14_BCL24_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_disp24 = EXTRACT_SIGNED (insn, 32, 8, 24) << 2; \

#define EXTRACT_FMT_15_BRA8_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  int f_disp8; \
  unsigned int length;
#define EXTRACT_FMT_15_BRA8_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_disp8 = EXTRACT_SIGNED (insn, 16, 8, 8) << 2; \

#define EXTRACT_FMT_16_BRA24_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  int f_disp24; \
  unsigned int length;
#define EXTRACT_FMT_16_BRA24_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_disp24 = EXTRACT_SIGNED (insn, 32, 8, 24) << 2; \

#define EXTRACT_FMT_17_CMP_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_17_CMP_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_18_CMPI_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  int f_simm16; \
  unsigned int length;
#define EXTRACT_FMT_18_CMPI_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16); \

#define EXTRACT_FMT_19_CMPUI_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  UINT f_uimm16; \
  unsigned int length;
#define EXTRACT_FMT_19_CMPUI_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_uimm16 = EXTRACT_UNSIGNED (insn, 32, 16, 16); \

#define EXTRACT_FMT_20_CMPZ_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_20_CMPZ_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_21_DIV_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  int f_simm16; \
  unsigned int length;
#define EXTRACT_FMT_21_DIV_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16); \

#define EXTRACT_FMT_22_JC_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_22_JC_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_23_JL_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_23_JL_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_24_JMP_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_24_JMP_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_25_LD_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_25_LD_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_26_LD_D_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  int f_simm16; \
  unsigned int length;
#define EXTRACT_FMT_26_LD_D_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16); \

#define EXTRACT_FMT_27_LDB_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_27_LDB_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_28_LDB_D_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  int f_simm16; \
  unsigned int length;
#define EXTRACT_FMT_28_LDB_D_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16); \

#define EXTRACT_FMT_29_LDH_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_29_LDH_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_30_LDH_D_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  int f_simm16; \
  unsigned int length;
#define EXTRACT_FMT_30_LDH_D_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16); \

#define EXTRACT_FMT_31_LD24_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_uimm24; \
  unsigned int length;
#define EXTRACT_FMT_31_LD24_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_uimm24 = EXTRACT_UNSIGNED (insn, 32, 8, 24); \

#define EXTRACT_FMT_32_LDI8_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  int f_simm8; \
  unsigned int length;
#define EXTRACT_FMT_32_LDI8_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_simm8 = EXTRACT_SIGNED (insn, 16, 8, 8); \

#define EXTRACT_FMT_33_LDI16_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  int f_simm16; \
  unsigned int length;
#define EXTRACT_FMT_33_LDI16_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16); \

#define EXTRACT_FMT_34_MACHI_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_34_MACHI_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_35_MACHI_A_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_acc; \
  UINT f_op23; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_35_MACHI_A_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_acc = EXTRACT_UNSIGNED (insn, 16, 8, 1); \
  f_op23 = EXTRACT_UNSIGNED (insn, 16, 9, 3); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_36_MULHI_A_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_acc; \
  UINT f_op23; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_36_MULHI_A_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_acc = EXTRACT_UNSIGNED (insn, 16, 8, 1); \
  f_op23 = EXTRACT_UNSIGNED (insn, 16, 9, 3); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_37_MV_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_37_MV_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_38_MVFACHI_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_38_MVFACHI_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_39_MVFACHI_A_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_accs; \
  UINT f_op3; \
  unsigned int length;
#define EXTRACT_FMT_39_MVFACHI_A_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_accs = EXTRACT_UNSIGNED (insn, 16, 12, 2); \
  f_op3 = EXTRACT_UNSIGNED (insn, 16, 14, 2); \

#define EXTRACT_FMT_40_MVFC_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_40_MVFC_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_41_MVTACHI_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_41_MVTACHI_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_42_MVTACHI_A_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_accs; \
  UINT f_op3; \
  unsigned int length;
#define EXTRACT_FMT_42_MVTACHI_A_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_accs = EXTRACT_UNSIGNED (insn, 16, 12, 2); \
  f_op3 = EXTRACT_UNSIGNED (insn, 16, 14, 2); \

#define EXTRACT_FMT_43_MVTC_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_43_MVTC_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_44_NOP_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_44_NOP_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_45_RAC_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_45_RAC_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_46_RAC_A_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_accs; \
  UINT f_op3; \
  unsigned int length;
#define EXTRACT_FMT_46_RAC_A_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_accs = EXTRACT_UNSIGNED (insn, 16, 12, 2); \
  f_op3 = EXTRACT_UNSIGNED (insn, 16, 14, 2); \

#define EXTRACT_FMT_47_SETH_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  UINT f_hi16; \
  unsigned int length;
#define EXTRACT_FMT_47_SETH_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_hi16 = EXTRACT_UNSIGNED (insn, 32, 16, 16); \

#define EXTRACT_FMT_48_SLLI_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_shift_op2; \
  UINT f_uimm5; \
  unsigned int length;
#define EXTRACT_FMT_48_SLLI_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_shift_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 3); \
  f_uimm5 = EXTRACT_UNSIGNED (insn, 16, 11, 5); \

#define EXTRACT_FMT_49_ST_D_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  int f_simm16; \
  unsigned int length;
#define EXTRACT_FMT_49_ST_D_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16); \

#define EXTRACT_FMT_50_TRAP_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_uimm4; \
  unsigned int length;
#define EXTRACT_FMT_50_TRAP_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_uimm4 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_51_SATB_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  UINT f_uimm16; \
  unsigned int length;
#define EXTRACT_FMT_51_SATB_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_uimm16 = EXTRACT_UNSIGNED (insn, 32, 16, 16); \

#define EXTRACT_FMT_52_PCMPBZ_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_52_PCMPBZ_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_53_SADD_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_53_SADD_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_54_MACWU1_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_54_MACWU1_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_55_SC_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_55_SC_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

/* Fetched input values of an instruction.  */

struct parallel_exec {
  union {
    struct { /* e.g. add $dr,$sr */
      SI dr;
      SI sr;
    } fmt_0_add;
    struct { /* e.g. add3 $dr,$sr,#$slo16 */
      HI slo16;
      SI sr;
    } fmt_1_add3;
    struct { /* e.g. and3 $dr,$sr,#$uimm16 */
      SI sr;
      USI uimm16;
    } fmt_2_and3;
    struct { /* e.g. or3 $dr,$sr,#$ulo16 */
      SI sr;
      UHI ulo16;
    } fmt_3_or3;
    struct { /* e.g. addi $dr,#$simm8 */
      SI dr;
      SI simm8;
    } fmt_4_addi;
    struct { /* e.g. addv3 $dr,$sr,#$simm16 */
      SI simm16;
      SI sr;
    } fmt_5_addv3;
    struct { /* e.g. addx $dr,$sr */
      UBI condbit;
      SI dr;
      SI sr;
    } fmt_6_addx;
    struct { /* e.g. bc $disp8 */
      UBI condbit;
      IADDR disp8;
    } fmt_7_bc8;
    struct { /* e.g. bc $disp24 */
      UBI condbit;
      IADDR disp24;
    } fmt_8_bc24;
    struct { /* e.g. beq $src1,$src2,$disp16 */
      IADDR disp16;
      SI src1;
      SI src2;
    } fmt_9_beq;
    struct { /* e.g. beqz $src2,$disp16 */
      IADDR disp16;
      SI src2;
    } fmt_10_beqz;
    struct { /* e.g. bl $disp8 */
      IADDR disp8;
      USI pc;
    } fmt_11_bl8;
    struct { /* e.g. bl $disp24 */
      IADDR disp24;
      USI pc;
    } fmt_12_bl24;
    struct { /* e.g. bcl $disp8 */
      UBI condbit;
      IADDR disp8;
      USI pc;
    } fmt_13_bcl8;
    struct { /* e.g. bcl $disp24 */
      UBI condbit;
      IADDR disp24;
      USI pc;
    } fmt_14_bcl24;
    struct { /* e.g. bra $disp8 */
      IADDR disp8;
    } fmt_15_bra8;
    struct { /* e.g. bra $disp24 */
      IADDR disp24;
    } fmt_16_bra24;
    struct { /* e.g. cmp $src1,$src2 */
      SI src1;
      SI src2;
    } fmt_17_cmp;
    struct { /* e.g. cmpi $src2,#$simm16 */
      SI simm16;
      SI src2;
    } fmt_18_cmpi;
    struct { /* e.g. cmpui $src2,#$uimm16 */
      SI src2;
      USI uimm16;
    } fmt_19_cmpui;
    struct { /* e.g. cmpz $src2 */
      SI src2;
    } fmt_20_cmpz;
    struct { /* e.g. div $dr,$sr */
      SI dr;
      SI sr;
    } fmt_21_div;
    struct { /* e.g. jc $sr */
      UBI condbit;
      SI sr;
    } fmt_22_jc;
    struct { /* e.g. jl $sr */
      USI pc;
      SI sr;
    } fmt_23_jl;
    struct { /* e.g. jmp $sr */
      SI sr;
    } fmt_24_jmp;
    struct { /* e.g. ld $dr,@$sr */
      UQI h_memory;
      SI sr;
    } fmt_25_ld;
    struct { /* e.g. ld $dr,@($slo16,$sr) */
      UQI h_memory;
      HI slo16;
      SI sr;
    } fmt_26_ld_d;
    struct { /* e.g. ldb $dr,@$sr */
      UQI h_memory;
      SI sr;
    } fmt_27_ldb;
    struct { /* e.g. ldb $dr,@($slo16,$sr) */
      UQI h_memory;
      HI slo16;
      SI sr;
    } fmt_28_ldb_d;
    struct { /* e.g. ldh $dr,@$sr */
      UQI h_memory;
      SI sr;
    } fmt_29_ldh;
    struct { /* e.g. ldh $dr,@($slo16,$sr) */
      UQI h_memory;
      HI slo16;
      SI sr;
    } fmt_30_ldh_d;
    struct { /* e.g. ld24 $dr,#$uimm24 */
      ADDR uimm24;
    } fmt_31_ld24;
    struct { /* e.g. ldi $dr,#$simm8 */
      SI simm8;
    } fmt_32_ldi8;
    struct { /* e.g. ldi $dr,$slo16 */
      HI slo16;
    } fmt_33_ldi16;
    struct { /* e.g. machi $src1,$src2 */
      DI accum;
      SI src1;
      SI src2;
    } fmt_34_machi;
    struct { /* e.g. machi $src1,$src2,$acc */
      DI acc;
      SI src1;
      SI src2;
    } fmt_35_machi_a;
    struct { /* e.g. mulhi $src1,$src2,$acc */
      SI src1;
      SI src2;
    } fmt_36_mulhi_a;
    struct { /* e.g. mv $dr,$sr */
      SI sr;
    } fmt_37_mv;
    struct { /* e.g. mvfachi $dr */
      DI accum;
    } fmt_38_mvfachi;
    struct { /* e.g. mvfachi $dr,$accs */
      DI accs;
    } fmt_39_mvfachi_a;
    struct { /* e.g. mvfc $dr,$scr */
      SI scr;
    } fmt_40_mvfc;
    struct { /* e.g. mvtachi $src1 */
      DI accum;
      SI src1;
    } fmt_41_mvtachi;
    struct { /* e.g. mvtachi $src1,$accs */
      DI accs;
      SI src1;
    } fmt_42_mvtachi_a;
    struct { /* e.g. mvtc $sr,$dcr */
      SI sr;
    } fmt_43_mvtc;
    struct { /* e.g. nop */
      int empty;
    } fmt_44_nop;
    struct { /* e.g. rac */
      DI accum;
    } fmt_45_rac;
    struct { /* e.g. rac $accs */
      DI accs;
    } fmt_46_rac_a;
    struct { /* e.g. seth $dr,$hi16 */
      UHI hi16;
    } fmt_47_seth;
    struct { /* e.g. slli $dr,#$uimm5 */
      SI dr;
      USI uimm5;
    } fmt_48_slli;
    struct { /* e.g. st $src1,@($slo16,$src2) */
      HI slo16;
      SI src1;
      SI src2;
    } fmt_49_st_d;
    struct { /* e.g. trap #$uimm4 */
      USI uimm4;
    } fmt_50_trap;
    struct { /* e.g. satb $dr,$src2 */
      int empty;
    } fmt_51_satb;
    struct { /* e.g. pcmpbz $src2 */
      int empty;
    } fmt_52_pcmpbz;
    struct { /* e.g. sadd */
      DI h_accums;
      DI h_accums;
    } fmt_53_sadd;
    struct { /* e.g. macwu1 $src1,$src2 */
      DI h_accums;
      SI src1;
      SI src2;
    } fmt_54_macwu1;
    struct { /* e.g. sc */
      UBI condbit;
    } fmt_55_sc;
  } operands;
};

#endif /* CPU_M32RX_H */
