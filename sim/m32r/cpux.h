/* CPU family header for m32rx.

THIS FILE IS MACHINE GENERATED WITH CGEN.

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
  USI h_cr[16];
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
} M32RX_CPU_DATA;

/* Cover fns for register access.  */
USI m32rx_h_pc_get (SIM_CPU *);
void m32rx_h_pc_set (SIM_CPU *, USI);
SI m32rx_h_gr_get (SIM_CPU *, UINT);
void m32rx_h_gr_set (SIM_CPU *, UINT, SI);
USI m32rx_h_cr_get (SIM_CPU *, UINT);
void m32rx_h_cr_set (SIM_CPU *, UINT, USI);
DI m32rx_h_accum_get (SIM_CPU *);
void m32rx_h_accum_set (SIM_CPU *, DI);
DI m32rx_h_accums_get (SIM_CPU *, UINT);
void m32rx_h_accums_set (SIM_CPU *, UINT, DI);
UBI m32rx_h_cond_get (SIM_CPU *);
void m32rx_h_cond_set (SIM_CPU *, UBI);
UBI m32rx_h_sm_get (SIM_CPU *);
void m32rx_h_sm_set (SIM_CPU *, UBI);
UBI m32rx_h_bsm_get (SIM_CPU *);
void m32rx_h_bsm_set (SIM_CPU *, UBI);
UBI m32rx_h_ie_get (SIM_CPU *);
void m32rx_h_ie_set (SIM_CPU *, UBI);
UBI m32rx_h_bie_get (SIM_CPU *);
void m32rx_h_bie_set (SIM_CPU *, UBI);
UBI m32rx_h_bcond_get (SIM_CPU *);
void m32rx_h_bcond_set (SIM_CPU *, UBI);
SI m32rx_h_bpc_get (SIM_CPU *);
void m32rx_h_bpc_set (SIM_CPU *, SI);
UBI m32rx_h_lock_get (SIM_CPU *);
void m32rx_h_lock_set (SIM_CPU *, UBI);

/* These must be hand-written.  */
extern CPUREG_FETCH_FN m32rx_fetch_register;
extern CPUREG_STORE_FN m32rx_store_register;

/* The ARGBUF struct.  */
struct argbuf {
  /* These are the baseclass definitions.  */
  unsigned int length;
  PCADDR addr;
  const IDESC *idesc;
  /* cpu specific data follows */
  insn_t insn;
  union {
    struct { /* e.g. add $dr,$sr */
      UINT f_r1;
      UINT f_r2;
    } fmt_add;
    struct { /* e.g. add3 $dr,$sr,$hash$slo16 */
      UINT f_r1;
      UINT f_r2;
      HI f_simm16;
    } fmt_add3;
    struct { /* e.g. and3 $dr,$sr,$uimm16 */
      UINT f_r1;
      UINT f_r2;
      USI f_uimm16;
    } fmt_and3;
    struct { /* e.g. or3 $dr,$sr,$hash$ulo16 */
      UINT f_r1;
      UINT f_r2;
      UHI f_uimm16;
    } fmt_or3;
    struct { /* e.g. addi $dr,$simm8 */
      UINT f_r1;
      SI f_simm8;
    } fmt_addi;
    struct { /* e.g. addv $dr,$sr */
      UINT f_r1;
      UINT f_r2;
    } fmt_addv;
    struct { /* e.g. addv3 $dr,$sr,$simm16 */
      UINT f_r1;
      UINT f_r2;
      SI f_simm16;
    } fmt_addv3;
    struct { /* e.g. addx $dr,$sr */
      UINT f_r1;
      UINT f_r2;
    } fmt_addx;
    struct { /* e.g. bc.s $disp8 */
      IADDR f_disp8;
    } fmt_bc8;
    struct { /* e.g. bc.l $disp24 */
      IADDR f_disp24;
    } fmt_bc24;
    struct { /* e.g. beq $src1,$src2,$disp16 */
      UINT f_r1;
      UINT f_r2;
      IADDR f_disp16;
    } fmt_beq;
    struct { /* e.g. beqz $src2,$disp16 */
      UINT f_r2;
      IADDR f_disp16;
    } fmt_beqz;
    struct { /* e.g. bl.s $disp8 */
      IADDR f_disp8;
    } fmt_bl8;
    struct { /* e.g. bl.l $disp24 */
      IADDR f_disp24;
    } fmt_bl24;
    struct { /* e.g. bcl.s $disp8 */
      IADDR f_disp8;
    } fmt_bcl8;
    struct { /* e.g. bcl.l $disp24 */
      IADDR f_disp24;
    } fmt_bcl24;
    struct { /* e.g. bra.s $disp8 */
      IADDR f_disp8;
    } fmt_bra8;
    struct { /* e.g. bra.l $disp24 */
      IADDR f_disp24;
    } fmt_bra24;
    struct { /* e.g. cmp $src1,$src2 */
      UINT f_r1;
      UINT f_r2;
    } fmt_cmp;
    struct { /* e.g. cmpi $src2,$simm16 */
      UINT f_r2;
      SI f_simm16;
    } fmt_cmpi;
    struct { /* e.g. cmpz $src2 */
      UINT f_r2;
    } fmt_cmpz;
    struct { /* e.g. div $dr,$sr */
      UINT f_r1;
      UINT f_r2;
    } fmt_div;
    struct { /* e.g. jc $sr */
      UINT f_r2;
    } fmt_jc;
    struct { /* e.g. jl $sr */
      UINT f_r2;
    } fmt_jl;
    struct { /* e.g. jmp $sr */
      UINT f_r2;
    } fmt_jmp;
    struct { /* e.g. ld $dr,@$sr */
      UINT f_r1;
      UINT f_r2;
    } fmt_ld;
    struct { /* e.g. ld $dr,@($slo16,$sr) */
      UINT f_r1;
      UINT f_r2;
      HI f_simm16;
    } fmt_ld_d;
    struct { /* e.g. ldb $dr,@$sr */
      UINT f_r1;
      UINT f_r2;
    } fmt_ldb;
    struct { /* e.g. ldb $dr,@($slo16,$sr) */
      UINT f_r1;
      UINT f_r2;
      HI f_simm16;
    } fmt_ldb_d;
    struct { /* e.g. ldh $dr,@$sr */
      UINT f_r1;
      UINT f_r2;
    } fmt_ldh;
    struct { /* e.g. ldh $dr,@($slo16,$sr) */
      UINT f_r1;
      UINT f_r2;
      HI f_simm16;
    } fmt_ldh_d;
    struct { /* e.g. ld $dr,@$sr+ */
      UINT f_r1;
      UINT f_r2;
    } fmt_ld_plus;
    struct { /* e.g. ld24 $dr,$uimm24 */
      UINT f_r1;
      ADDR f_uimm24;
    } fmt_ld24;
    struct { /* e.g. ldi8 $dr,$simm8 */
      UINT f_r1;
      SI f_simm8;
    } fmt_ldi8;
    struct { /* e.g. ldi16 $dr,$hash$slo16 */
      UINT f_r1;
      HI f_simm16;
    } fmt_ldi16;
    struct { /* e.g. lock $dr,@$sr */
      UINT f_r1;
      UINT f_r2;
    } fmt_lock;
    struct { /* e.g. machi $src1,$src2,$acc */
      UINT f_r1;
      UINT f_acc;
      UINT f_r2;
    } fmt_machi_a;
    struct { /* e.g. mulhi $src1,$src2,$acc */
      UINT f_r1;
      UINT f_acc;
      UINT f_r2;
    } fmt_mulhi_a;
    struct { /* e.g. mv $dr,$sr */
      UINT f_r1;
      UINT f_r2;
    } fmt_mv;
    struct { /* e.g. mvfachi $dr,$accs */
      UINT f_r1;
      UINT f_accs;
    } fmt_mvfachi_a;
    struct { /* e.g. mvfc $dr,$scr */
      UINT f_r1;
      UINT f_r2;
    } fmt_mvfc;
    struct { /* e.g. mvtachi $src1,$accs */
      UINT f_r1;
      UINT f_accs;
    } fmt_mvtachi_a;
    struct { /* e.g. mvtc $sr,$dcr */
      UINT f_r1;
      UINT f_r2;
    } fmt_mvtc;
    struct { /* e.g. nop */
      int empty;
    } fmt_nop;
    struct { /* e.g. rac $accd,$accs,$imm1 */
      UINT f_accd;
      UINT f_accs;
      USI f_imm1;
    } fmt_rac_dsi;
    struct { /* e.g. rte */
      int empty;
    } fmt_rte;
    struct { /* e.g. seth $dr,$hash$hi16 */
      UINT f_r1;
      UHI f_hi16;
    } fmt_seth;
    struct { /* e.g. sll3 $dr,$sr,$simm16 */
      UINT f_r1;
      UINT f_r2;
      SI f_simm16;
    } fmt_sll3;
    struct { /* e.g. slli $dr,$uimm5 */
      UINT f_r1;
      USI f_uimm5;
    } fmt_slli;
    struct { /* e.g. st $src1,@$src2 */
      UINT f_r1;
      UINT f_r2;
    } fmt_st;
    struct { /* e.g. st $src1,@($slo16,$src2) */
      UINT f_r1;
      UINT f_r2;
      HI f_simm16;
    } fmt_st_d;
    struct { /* e.g. stb $src1,@$src2 */
      UINT f_r1;
      UINT f_r2;
    } fmt_stb;
    struct { /* e.g. stb $src1,@($slo16,$src2) */
      UINT f_r1;
      UINT f_r2;
      HI f_simm16;
    } fmt_stb_d;
    struct { /* e.g. sth $src1,@$src2 */
      UINT f_r1;
      UINT f_r2;
    } fmt_sth;
    struct { /* e.g. sth $src1,@($slo16,$src2) */
      UINT f_r1;
      UINT f_r2;
      HI f_simm16;
    } fmt_sth_d;
    struct { /* e.g. st $src1,@+$src2 */
      UINT f_r1;
      UINT f_r2;
    } fmt_st_plus;
    struct { /* e.g. trap $uimm4 */
      USI f_uimm4;
    } fmt_trap;
    struct { /* e.g. unlock $src1,@$src2 */
      UINT f_r1;
      UINT f_r2;
    } fmt_unlock;
    struct { /* e.g. satb $dr,$sr */
      UINT f_r1;
      UINT f_r2;
    } fmt_satb;
    struct { /* e.g. sat $dr,$sr */
      UINT f_r1;
      UINT f_r2;
    } fmt_sat;
    struct { /* e.g. sadd */
      int empty;
    } fmt_sadd;
    struct { /* e.g. macwu1 $src1,$src2 */
      UINT f_r1;
      UINT f_r2;
    } fmt_macwu1;
    struct { /* e.g. msblo $src1,$src2 */
      UINT f_r1;
      UINT f_r2;
    } fmt_msblo;
    struct { /* e.g. mulwu1 $src1,$src2 */
      UINT f_r1;
      UINT f_r2;
    } fmt_mulwu1;
    struct { /* e.g. sc */
      int empty;
    } fmt_sc;
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
    SEMANTIC_FN *sem_full;
#endif
#if ! WITH_SEM_SWITCH_FAST
    SEMANTIC_FN *sem_fast;
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

#define EXTRACT_FMT_ADD_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_ADD_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_ADD3_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  int f_simm16; \
  unsigned int length;
#define EXTRACT_FMT_ADD3_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16); \

#define EXTRACT_FMT_AND3_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  UINT f_uimm16; \
  unsigned int length;
#define EXTRACT_FMT_AND3_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_uimm16 = EXTRACT_UNSIGNED (insn, 32, 16, 16); \

#define EXTRACT_FMT_OR3_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  UINT f_uimm16; \
  unsigned int length;
#define EXTRACT_FMT_OR3_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_uimm16 = EXTRACT_UNSIGNED (insn, 32, 16, 16); \

#define EXTRACT_FMT_ADDI_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  int f_simm8; \
  unsigned int length;
#define EXTRACT_FMT_ADDI_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_simm8 = EXTRACT_SIGNED (insn, 16, 8, 8); \

#define EXTRACT_FMT_ADDV_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_ADDV_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_ADDV3_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  int f_simm16; \
  unsigned int length;
#define EXTRACT_FMT_ADDV3_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16); \

#define EXTRACT_FMT_ADDX_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_ADDX_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_BC8_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  int f_disp8; \
  unsigned int length;
#define EXTRACT_FMT_BC8_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_disp8 = ((((EXTRACT_SIGNED (insn, 16, 8, 8)) << (2))) + (((pc) & (-4)))); \

#define EXTRACT_FMT_BC24_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  int f_disp24; \
  unsigned int length;
#define EXTRACT_FMT_BC24_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_disp24 = ((((EXTRACT_SIGNED (insn, 32, 8, 24)) << (2))) + (pc)); \

#define EXTRACT_FMT_BEQ_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  int f_disp16; \
  unsigned int length;
#define EXTRACT_FMT_BEQ_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_disp16 = ((((EXTRACT_SIGNED (insn, 32, 16, 16)) << (2))) + (pc)); \

#define EXTRACT_FMT_BEQZ_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  int f_disp16; \
  unsigned int length;
#define EXTRACT_FMT_BEQZ_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_disp16 = ((((EXTRACT_SIGNED (insn, 32, 16, 16)) << (2))) + (pc)); \

#define EXTRACT_FMT_BL8_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  int f_disp8; \
  unsigned int length;
#define EXTRACT_FMT_BL8_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_disp8 = ((((EXTRACT_SIGNED (insn, 16, 8, 8)) << (2))) + (((pc) & (-4)))); \

#define EXTRACT_FMT_BL24_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  int f_disp24; \
  unsigned int length;
#define EXTRACT_FMT_BL24_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_disp24 = ((((EXTRACT_SIGNED (insn, 32, 8, 24)) << (2))) + (pc)); \

#define EXTRACT_FMT_BCL8_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  int f_disp8; \
  unsigned int length;
#define EXTRACT_FMT_BCL8_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_disp8 = ((((EXTRACT_SIGNED (insn, 16, 8, 8)) << (2))) + (((pc) & (-4)))); \

#define EXTRACT_FMT_BCL24_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  int f_disp24; \
  unsigned int length;
#define EXTRACT_FMT_BCL24_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_disp24 = ((((EXTRACT_SIGNED (insn, 32, 8, 24)) << (2))) + (pc)); \

#define EXTRACT_FMT_BRA8_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  int f_disp8; \
  unsigned int length;
#define EXTRACT_FMT_BRA8_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_disp8 = ((((EXTRACT_SIGNED (insn, 16, 8, 8)) << (2))) + (((pc) & (-4)))); \

#define EXTRACT_FMT_BRA24_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  int f_disp24; \
  unsigned int length;
#define EXTRACT_FMT_BRA24_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_disp24 = ((((EXTRACT_SIGNED (insn, 32, 8, 24)) << (2))) + (pc)); \

#define EXTRACT_FMT_CMP_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_CMP_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_CMPI_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  int f_simm16; \
  unsigned int length;
#define EXTRACT_FMT_CMPI_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16); \

#define EXTRACT_FMT_CMPZ_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_CMPZ_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_DIV_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  int f_simm16; \
  unsigned int length;
#define EXTRACT_FMT_DIV_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16); \

#define EXTRACT_FMT_JC_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_JC_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_JL_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_JL_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_JMP_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_JMP_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_LD_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_LD_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_LD_D_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  int f_simm16; \
  unsigned int length;
#define EXTRACT_FMT_LD_D_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16); \

#define EXTRACT_FMT_LDB_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_LDB_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_LDB_D_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  int f_simm16; \
  unsigned int length;
#define EXTRACT_FMT_LDB_D_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16); \

#define EXTRACT_FMT_LDH_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_LDH_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_LDH_D_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  int f_simm16; \
  unsigned int length;
#define EXTRACT_FMT_LDH_D_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16); \

#define EXTRACT_FMT_LD_PLUS_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_LD_PLUS_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_LD24_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_uimm24; \
  unsigned int length;
#define EXTRACT_FMT_LD24_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_uimm24 = EXTRACT_UNSIGNED (insn, 32, 8, 24); \

#define EXTRACT_FMT_LDI8_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  int f_simm8; \
  unsigned int length;
#define EXTRACT_FMT_LDI8_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_simm8 = EXTRACT_SIGNED (insn, 16, 8, 8); \

#define EXTRACT_FMT_LDI16_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  int f_simm16; \
  unsigned int length;
#define EXTRACT_FMT_LDI16_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16); \

#define EXTRACT_FMT_LOCK_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_LOCK_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_MACHI_A_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_acc; \
  UINT f_op23; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_MACHI_A_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_acc = EXTRACT_UNSIGNED (insn, 16, 8, 1); \
  f_op23 = EXTRACT_UNSIGNED (insn, 16, 9, 3); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_MULHI_A_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_acc; \
  UINT f_op23; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_MULHI_A_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_acc = EXTRACT_UNSIGNED (insn, 16, 8, 1); \
  f_op23 = EXTRACT_UNSIGNED (insn, 16, 9, 3); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_MV_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_MV_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_MVFACHI_A_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_accs; \
  UINT f_op3; \
  unsigned int length;
#define EXTRACT_FMT_MVFACHI_A_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_accs = EXTRACT_UNSIGNED (insn, 16, 12, 2); \
  f_op3 = EXTRACT_UNSIGNED (insn, 16, 14, 2); \

#define EXTRACT_FMT_MVFC_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_MVFC_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_MVTACHI_A_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_accs; \
  UINT f_op3; \
  unsigned int length;
#define EXTRACT_FMT_MVTACHI_A_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_accs = EXTRACT_UNSIGNED (insn, 16, 12, 2); \
  f_op3 = EXTRACT_UNSIGNED (insn, 16, 14, 2); \

#define EXTRACT_FMT_MVTC_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_MVTC_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_NOP_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_NOP_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_RAC_DSI_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_accd; \
  UINT f_bits67; \
  UINT f_op2; \
  UINT f_accs; \
  UINT f_bit14; \
  UINT f_imm1; \
  unsigned int length;
#define EXTRACT_FMT_RAC_DSI_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_accd = EXTRACT_UNSIGNED (insn, 16, 4, 2); \
  f_bits67 = EXTRACT_UNSIGNED (insn, 16, 6, 2); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_accs = EXTRACT_UNSIGNED (insn, 16, 12, 2); \
  f_bit14 = EXTRACT_UNSIGNED (insn, 16, 14, 1); \
  f_imm1 = ((EXTRACT_UNSIGNED (insn, 16, 15, 1)) + (1)); \

#define EXTRACT_FMT_RTE_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_RTE_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_SETH_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  UINT f_hi16; \
  unsigned int length;
#define EXTRACT_FMT_SETH_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_hi16 = EXTRACT_UNSIGNED (insn, 32, 16, 16); \

#define EXTRACT_FMT_SLL3_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  int f_simm16; \
  unsigned int length;
#define EXTRACT_FMT_SLL3_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16); \

#define EXTRACT_FMT_SLLI_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_shift_op2; \
  UINT f_uimm5; \
  unsigned int length;
#define EXTRACT_FMT_SLLI_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_shift_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 3); \
  f_uimm5 = EXTRACT_UNSIGNED (insn, 16, 11, 5); \

#define EXTRACT_FMT_ST_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_ST_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_ST_D_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  int f_simm16; \
  unsigned int length;
#define EXTRACT_FMT_ST_D_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16); \

#define EXTRACT_FMT_STB_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_STB_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_STB_D_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  int f_simm16; \
  unsigned int length;
#define EXTRACT_FMT_STB_D_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16); \

#define EXTRACT_FMT_STH_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_STH_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_STH_D_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  int f_simm16; \
  unsigned int length;
#define EXTRACT_FMT_STH_D_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16); \

#define EXTRACT_FMT_ST_PLUS_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_ST_PLUS_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_TRAP_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_uimm4; \
  unsigned int length;
#define EXTRACT_FMT_TRAP_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_uimm4 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_UNLOCK_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_UNLOCK_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_SATB_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  UINT f_uimm16; \
  unsigned int length;
#define EXTRACT_FMT_SATB_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_uimm16 = EXTRACT_UNSIGNED (insn, 32, 16, 16); \

#define EXTRACT_FMT_SAT_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  UINT f_uimm16; \
  unsigned int length;
#define EXTRACT_FMT_SAT_CODE \
  length = 4; \
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4); \
  f_uimm16 = EXTRACT_UNSIGNED (insn, 32, 16, 16); \

#define EXTRACT_FMT_SADD_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_SADD_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_MACWU1_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_MACWU1_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_MSBLO_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_MSBLO_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_MULWU1_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_MULWU1_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

#define EXTRACT_FMT_SC_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_r1; \
  UINT f_op2; \
  UINT f_r2; \
  unsigned int length;
#define EXTRACT_FMT_SC_CODE \
  length = 2; \
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4); \
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4); \
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4); \
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4); \

/* Fetched input values of an instruction.  */

struct parexec {
  union {
    struct { /* e.g. add $dr,$sr */
      SI dr;
      SI sr;
    } fmt_add;
    struct { /* e.g. add3 $dr,$sr,$hash$slo16 */
      SI sr;
      HI slo16;
    } fmt_add3;
    struct { /* e.g. and3 $dr,$sr,$uimm16 */
      SI sr;
      USI uimm16;
    } fmt_and3;
    struct { /* e.g. or3 $dr,$sr,$hash$ulo16 */
      SI sr;
      UHI ulo16;
    } fmt_or3;
    struct { /* e.g. addi $dr,$simm8 */
      SI dr;
      SI simm8;
    } fmt_addi;
    struct { /* e.g. addv $dr,$sr */
      SI dr;
      SI sr;
    } fmt_addv;
    struct { /* e.g. addv3 $dr,$sr,$simm16 */
      SI sr;
      SI simm16;
    } fmt_addv3;
    struct { /* e.g. addx $dr,$sr */
      SI dr;
      SI sr;
      UBI condbit;
    } fmt_addx;
    struct { /* e.g. bc.s $disp8 */
      UBI condbit;
      USI disp8;
    } fmt_bc8;
    struct { /* e.g. bc.l $disp24 */
      UBI condbit;
      USI disp24;
    } fmt_bc24;
    struct { /* e.g. beq $src1,$src2,$disp16 */
      SI src1;
      SI src2;
      USI disp16;
    } fmt_beq;
    struct { /* e.g. beqz $src2,$disp16 */
      SI src2;
      USI disp16;
    } fmt_beqz;
    struct { /* e.g. bl.s $disp8 */
      USI pc;
      USI disp8;
    } fmt_bl8;
    struct { /* e.g. bl.l $disp24 */
      USI pc;
      USI disp24;
    } fmt_bl24;
    struct { /* e.g. bcl.s $disp8 */
      UBI condbit;
      USI pc;
      USI disp8;
    } fmt_bcl8;
    struct { /* e.g. bcl.l $disp24 */
      UBI condbit;
      USI pc;
      USI disp24;
    } fmt_bcl24;
    struct { /* e.g. bra.s $disp8 */
      USI disp8;
    } fmt_bra8;
    struct { /* e.g. bra.l $disp24 */
      USI disp24;
    } fmt_bra24;
    struct { /* e.g. cmp $src1,$src2 */
      SI src1;
      SI src2;
    } fmt_cmp;
    struct { /* e.g. cmpi $src2,$simm16 */
      SI src2;
      SI simm16;
    } fmt_cmpi;
    struct { /* e.g. cmpz $src2 */
      SI src2;
    } fmt_cmpz;
    struct { /* e.g. div $dr,$sr */
      SI dr;
      SI sr;
    } fmt_div;
    struct { /* e.g. jc $sr */
      UBI condbit;
      SI sr;
    } fmt_jc;
    struct { /* e.g. jl $sr */
      USI pc;
      SI sr;
    } fmt_jl;
    struct { /* e.g. jmp $sr */
      SI sr;
    } fmt_jmp;
    struct { /* e.g. ld $dr,@$sr */
      SI h_memory_sr;
      USI sr;
    } fmt_ld;
    struct { /* e.g. ld $dr,@($slo16,$sr) */
      SI h_memory_add__VM_sr_slo16;
      SI sr;
      HI slo16;
    } fmt_ld_d;
    struct { /* e.g. ldb $dr,@$sr */
      QI h_memory_sr;
      USI sr;
    } fmt_ldb;
    struct { /* e.g. ldb $dr,@($slo16,$sr) */
      QI h_memory_add__VM_sr_slo16;
      SI sr;
      HI slo16;
    } fmt_ldb_d;
    struct { /* e.g. ldh $dr,@$sr */
      HI h_memory_sr;
      USI sr;
    } fmt_ldh;
    struct { /* e.g. ldh $dr,@($slo16,$sr) */
      HI h_memory_add__VM_sr_slo16;
      SI sr;
      HI slo16;
    } fmt_ldh_d;
    struct { /* e.g. ld $dr,@$sr+ */
      SI h_memory_sr;
      SI sr;
    } fmt_ld_plus;
    struct { /* e.g. ld24 $dr,$uimm24 */
      USI uimm24;
    } fmt_ld24;
    struct { /* e.g. ldi8 $dr,$simm8 */
      SI simm8;
    } fmt_ldi8;
    struct { /* e.g. ldi16 $dr,$hash$slo16 */
      HI slo16;
    } fmt_ldi16;
    struct { /* e.g. lock $dr,@$sr */
      SI h_memory_sr;
      USI sr;
    } fmt_lock;
    struct { /* e.g. machi $src1,$src2,$acc */
      DI acc;
      SI src1;
      SI src2;
    } fmt_machi_a;
    struct { /* e.g. mulhi $src1,$src2,$acc */
      SI src1;
      SI src2;
    } fmt_mulhi_a;
    struct { /* e.g. mv $dr,$sr */
      SI sr;
    } fmt_mv;
    struct { /* e.g. mvfachi $dr,$accs */
      DI accs;
    } fmt_mvfachi_a;
    struct { /* e.g. mvfc $dr,$scr */
      USI scr;
    } fmt_mvfc;
    struct { /* e.g. mvtachi $src1,$accs */
      DI accs;
      SI src1;
    } fmt_mvtachi_a;
    struct { /* e.g. mvtc $sr,$dcr */
      SI sr;
    } fmt_mvtc;
    struct { /* e.g. nop */
      int empty;
    } fmt_nop;
    struct { /* e.g. rac $accd,$accs,$imm1 */
      DI accs;
      USI imm1;
    } fmt_rac_dsi;
    struct { /* e.g. rte */
      UBI h_bsm_0;
      UBI h_bie_0;
      UBI h_bcond_0;
      SI h_bpc_0;
    } fmt_rte;
    struct { /* e.g. seth $dr,$hash$hi16 */
      SI hi16;
    } fmt_seth;
    struct { /* e.g. sll3 $dr,$sr,$simm16 */
      SI sr;
      SI simm16;
    } fmt_sll3;
    struct { /* e.g. slli $dr,$uimm5 */
      SI dr;
      USI uimm5;
    } fmt_slli;
    struct { /* e.g. st $src1,@$src2 */
      USI src2;
      SI src1;
    } fmt_st;
    struct { /* e.g. st $src1,@($slo16,$src2) */
      SI src2;
      HI slo16;
      SI src1;
    } fmt_st_d;
    struct { /* e.g. stb $src1,@$src2 */
      USI src2;
      QI src1;
    } fmt_stb;
    struct { /* e.g. stb $src1,@($slo16,$src2) */
      SI src2;
      HI slo16;
      QI src1;
    } fmt_stb_d;
    struct { /* e.g. sth $src1,@$src2 */
      USI src2;
      HI src1;
    } fmt_sth;
    struct { /* e.g. sth $src1,@($slo16,$src2) */
      SI src2;
      HI slo16;
      HI src1;
    } fmt_sth_d;
    struct { /* e.g. st $src1,@+$src2 */
      SI src2;
      SI src1;
    } fmt_st_plus;
    struct { /* e.g. trap $uimm4 */
      USI h_cr_0;
      SI pc;
      SI uimm4;
    } fmt_trap;
    struct { /* e.g. unlock $src1,@$src2 */
      UBI h_lock_0;
      USI src2;
      SI src1;
    } fmt_unlock;
    struct { /* e.g. satb $dr,$sr */
      SI sr;
    } fmt_satb;
    struct { /* e.g. sat $dr,$sr */
      UBI condbit;
      SI sr;
    } fmt_sat;
    struct { /* e.g. sadd */
      DI h_accums_1;
      DI h_accums_0;
    } fmt_sadd;
    struct { /* e.g. macwu1 $src1,$src2 */
      DI h_accums_1;
      SI src1;
      SI src2;
    } fmt_macwu1;
    struct { /* e.g. msblo $src1,$src2 */
      DI accum;
      SI src1;
      SI src2;
    } fmt_msblo;
    struct { /* e.g. mulwu1 $src1,$src2 */
      SI src1;
      SI src2;
    } fmt_mulwu1;
    struct { /* e.g. sc */
      UBI condbit;
    } fmt_sc;
  } operands;
};

#endif /* CPU_M32RX_H */
