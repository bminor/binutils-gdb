/* CPU family header for fr30bf.

THIS FILE IS MACHINE GENERATED WITH CGEN.

Copyright (C) 1996, 1997, 1998, 1999 Free Software Foundation, Inc.

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

#ifndef CPU_FR30BF_H
#define CPU_FR30BF_H

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
  /* general registers */
  SI h_gr[16];
#define GET_H_GR(a1) CPU (h_gr)[a1]
#define SET_H_GR(a1, x) (CPU (h_gr)[a1] = (x))
  /* coprocessor registers */
  SI h_cr[16];
#define GET_H_CR(a1) CPU (h_cr)[a1]
#define SET_H_CR(a1, x) (CPU (h_cr)[a1] = (x))
  /* dedicated registers */
  SI h_dr[6];
/* GET_H_DR macro user-written */
/* SET_H_DR macro user-written */
  /* program status */
  USI h_ps;
/* GET_H_PS macro user-written */
/* SET_H_PS macro user-written */
  /* General Register 13 explicitely required */
  SI h_r13;
#define GET_H_R13() CPU (h_r13)
#define SET_H_R13(x) (CPU (h_r13) = (x))
  /* General Register 14 explicitely required */
  SI h_r14;
#define GET_H_R14() CPU (h_r14)
#define SET_H_R14(x) (CPU (h_r14) = (x))
  /* General Register 15 explicitely required */
  SI h_r15;
#define GET_H_R15() CPU (h_r15)
#define SET_H_R15(x) (CPU (h_r15) = (x))
  /* negative         bit */
  BI h_nbit;
#define GET_H_NBIT() CPU (h_nbit)
#define SET_H_NBIT(x) (CPU (h_nbit) = (x))
  /* zero             bit */
  BI h_zbit;
#define GET_H_ZBIT() CPU (h_zbit)
#define SET_H_ZBIT(x) (CPU (h_zbit) = (x))
  /* overflow         bit */
  BI h_vbit;
#define GET_H_VBIT() CPU (h_vbit)
#define SET_H_VBIT(x) (CPU (h_vbit) = (x))
  /* carry            bit */
  BI h_cbit;
#define GET_H_CBIT() CPU (h_cbit)
#define SET_H_CBIT(x) (CPU (h_cbit) = (x))
  /* interrupt enable bit */
  BI h_ibit;
#define GET_H_IBIT() CPU (h_ibit)
#define SET_H_IBIT(x) (CPU (h_ibit) = (x))
  /* stack            bit */
  BI h_sbit;
/* GET_H_SBIT macro user-written */
/* SET_H_SBIT macro user-written */
  /* trace trap       bit */
  BI h_tbit;
#define GET_H_TBIT() CPU (h_tbit)
#define SET_H_TBIT(x) (CPU (h_tbit) = (x))
  /* division 0       bit */
  BI h_d0bit;
#define GET_H_D0BIT() CPU (h_d0bit)
#define SET_H_D0BIT(x) (CPU (h_d0bit) = (x))
  /* division 1       bit */
  BI h_d1bit;
#define GET_H_D1BIT() CPU (h_d1bit)
#define SET_H_D1BIT(x) (CPU (h_d1bit) = (x))
  /* condition code   bits */
  UQI h_ccr;
/* GET_H_CCR macro user-written */
/* SET_H_CCR macro user-written */
  /* system condition bits */
  UQI h_scr;
/* GET_H_SCR macro user-written */
/* SET_H_SCR macro user-written */
  /* interrupt level  mask */
  UQI h_ilm;
/* GET_H_ILM macro user-written */
/* SET_H_ILM macro user-written */
  } hardware;
#define CPU_CGEN_HW(cpu) (& (cpu)->cpu_data.hardware)
} FR30BF_CPU_DATA;

/* Cover fns for register access.  */
USI fr30bf_h_pc_get (SIM_CPU *);
void fr30bf_h_pc_set (SIM_CPU *, USI);
SI fr30bf_h_gr_get (SIM_CPU *, UINT);
void fr30bf_h_gr_set (SIM_CPU *, UINT, SI);
SI fr30bf_h_cr_get (SIM_CPU *, UINT);
void fr30bf_h_cr_set (SIM_CPU *, UINT, SI);
SI fr30bf_h_dr_get (SIM_CPU *, UINT);
void fr30bf_h_dr_set (SIM_CPU *, UINT, SI);
USI fr30bf_h_ps_get (SIM_CPU *);
void fr30bf_h_ps_set (SIM_CPU *, USI);
SI fr30bf_h_r13_get (SIM_CPU *);
void fr30bf_h_r13_set (SIM_CPU *, SI);
SI fr30bf_h_r14_get (SIM_CPU *);
void fr30bf_h_r14_set (SIM_CPU *, SI);
SI fr30bf_h_r15_get (SIM_CPU *);
void fr30bf_h_r15_set (SIM_CPU *, SI);
BI fr30bf_h_nbit_get (SIM_CPU *);
void fr30bf_h_nbit_set (SIM_CPU *, BI);
BI fr30bf_h_zbit_get (SIM_CPU *);
void fr30bf_h_zbit_set (SIM_CPU *, BI);
BI fr30bf_h_vbit_get (SIM_CPU *);
void fr30bf_h_vbit_set (SIM_CPU *, BI);
BI fr30bf_h_cbit_get (SIM_CPU *);
void fr30bf_h_cbit_set (SIM_CPU *, BI);
BI fr30bf_h_ibit_get (SIM_CPU *);
void fr30bf_h_ibit_set (SIM_CPU *, BI);
BI fr30bf_h_sbit_get (SIM_CPU *);
void fr30bf_h_sbit_set (SIM_CPU *, BI);
BI fr30bf_h_tbit_get (SIM_CPU *);
void fr30bf_h_tbit_set (SIM_CPU *, BI);
BI fr30bf_h_d0bit_get (SIM_CPU *);
void fr30bf_h_d0bit_set (SIM_CPU *, BI);
BI fr30bf_h_d1bit_get (SIM_CPU *);
void fr30bf_h_d1bit_set (SIM_CPU *, BI);
UQI fr30bf_h_ccr_get (SIM_CPU *);
void fr30bf_h_ccr_set (SIM_CPU *, UQI);
UQI fr30bf_h_scr_get (SIM_CPU *);
void fr30bf_h_scr_set (SIM_CPU *, UQI);
UQI fr30bf_h_ilm_get (SIM_CPU *);
void fr30bf_h_ilm_set (SIM_CPU *, UQI);

/* These must be hand-written.  */
extern CPUREG_FETCH_FN fr30bf_fetch_register;
extern CPUREG_STORE_FN fr30bf_store_register;

typedef struct {
  UINT load_regs;
  UINT load_regs_pending;
} MODEL_FR30_1_DATA;

union sem_fields {
    struct { /* empty sformat for unspecified field list */
      int empty;
    } fmt_empty;
    struct { /* e.g. add $Rj,$Ri */
      SI * i_Ri;
      SI * i_Rj;
      unsigned char in_Ri;
      unsigned char in_Rj;
      unsigned char out_Ri;
    } fmt_add;
    struct { /* e.g. add $u4,$Ri */
      UINT f_u4;
      SI * i_Ri;
      unsigned char in_Ri;
      unsigned char out_Ri;
    } fmt_addi;
    struct { /* e.g. add2 $m4,$Ri */
      SI f_m4;
      SI * i_Ri;
      unsigned char in_Ri;
      unsigned char out_Ri;
    } fmt_add2;
    struct { /* e.g. addc $Rj,$Ri */
      SI * i_Ri;
      SI * i_Rj;
      unsigned char in_Ri;
      unsigned char in_Rj;
      unsigned char out_Ri;
    } fmt_addc;
    struct { /* e.g. addn $Rj,$Ri */
      SI * i_Ri;
      SI * i_Rj;
      unsigned char in_Ri;
      unsigned char in_Rj;
      unsigned char out_Ri;
    } fmt_addn;
    struct { /* e.g. addn $u4,$Ri */
      UINT f_u4;
      SI * i_Ri;
      unsigned char in_Ri;
      unsigned char out_Ri;
    } fmt_addni;
    struct { /* e.g. addn2 $m4,$Ri */
      SI f_m4;
      SI * i_Ri;
      unsigned char in_Ri;
      unsigned char out_Ri;
    } fmt_addn2;
    struct { /* e.g. cmp $Rj,$Ri */
      SI * i_Ri;
      SI * i_Rj;
      unsigned char in_Ri;
      unsigned char in_Rj;
    } fmt_cmp;
    struct { /* e.g. cmp $u4,$Ri */
      UINT f_u4;
      SI * i_Ri;
      unsigned char in_Ri;
    } fmt_cmpi;
    struct { /* e.g. cmp2 $m4,$Ri */
      SI f_m4;
      SI * i_Ri;
      unsigned char in_Ri;
    } fmt_cmp2;
    struct { /* e.g. and $Rj,$Ri */
      SI * i_Ri;
      SI * i_Rj;
      unsigned char in_Ri;
      unsigned char in_Rj;
      unsigned char out_Ri;
    } fmt_and;
    struct { /* e.g. and $Rj,@$Ri */
      SI * i_Ri;
      SI * i_Rj;
      unsigned char in_Ri;
      unsigned char in_Rj;
    } fmt_andm;
    struct { /* e.g. andh $Rj,@$Ri */
      SI * i_Ri;
      SI * i_Rj;
      unsigned char in_Ri;
      unsigned char in_Rj;
    } fmt_andh;
    struct { /* e.g. andb $Rj,@$Ri */
      SI * i_Ri;
      SI * i_Rj;
      unsigned char in_Ri;
      unsigned char in_Rj;
    } fmt_andb;
    struct { /* e.g. bandl $u4,@$Ri */
      UINT f_u4;
      SI * i_Ri;
      unsigned char in_Ri;
    } fmt_bandl;
    struct { /* e.g. btstl $u4,@$Ri */
      UINT f_u4;
      SI * i_Ri;
      unsigned char in_Ri;
    } fmt_btstl;
    struct { /* e.g. mul $Rj,$Ri */
      SI * i_Ri;
      SI * i_Rj;
      unsigned char in_Ri;
      unsigned char in_Rj;
    } fmt_mul;
    struct { /* e.g. mulu $Rj,$Ri */
      SI * i_Ri;
      SI * i_Rj;
      unsigned char in_Ri;
      unsigned char in_Rj;
    } fmt_mulu;
    struct { /* e.g. mulh $Rj,$Ri */
      SI * i_Ri;
      SI * i_Rj;
      unsigned char in_Ri;
      unsigned char in_Rj;
    } fmt_mulh;
    struct { /* e.g. div0s $Ri */
      SI * i_Ri;
      unsigned char in_Ri;
    } fmt_div0s;
    struct { /* e.g. div0u $Ri */
      int empty;
    } fmt_div0u;
    struct { /* e.g. div1 $Ri */
      SI * i_Ri;
      unsigned char in_Ri;
    } fmt_div1;
    struct { /* e.g. div2 $Ri */
      SI * i_Ri;
      unsigned char in_Ri;
    } fmt_div2;
    struct { /* e.g. div3 */
      int empty;
    } fmt_div3;
    struct { /* e.g. div4s */
      int empty;
    } fmt_div4s;
    struct { /* e.g. lsl $Rj,$Ri */
      SI * i_Ri;
      SI * i_Rj;
      unsigned char in_Ri;
      unsigned char in_Rj;
      unsigned char out_Ri;
    } fmt_lsl;
    struct { /* e.g. lsl $u4,$Ri */
      UINT f_u4;
      SI * i_Ri;
      unsigned char in_Ri;
      unsigned char out_Ri;
    } fmt_lsli;
    struct { /* e.g. ldi:8 $i8,$Ri */
      UINT f_i8;
      SI * i_Ri;
      unsigned char out_Ri;
    } fmt_ldi8;
    struct { /* e.g. ldi:20 $i20,$Ri */
      UINT f_i20;
      SI * i_Ri;
      unsigned char out_Ri;
    } fmt_ldi20;
    struct { /* e.g. ldi:32 $i32,$Ri */
      UINT f_i32;
      SI * i_Ri;
      unsigned char out_Ri;
    } fmt_ldi32;
    struct { /* e.g. ld @$Rj,$Ri */
      SI * i_Rj;
      SI * i_Ri;
      unsigned char in_Rj;
      unsigned char out_Ri;
    } fmt_ld;
    struct { /* e.g. lduh @$Rj,$Ri */
      SI * i_Rj;
      SI * i_Ri;
      unsigned char in_Rj;
      unsigned char out_Ri;
    } fmt_lduh;
    struct { /* e.g. ldub @$Rj,$Ri */
      SI * i_Rj;
      SI * i_Ri;
      unsigned char in_Rj;
      unsigned char out_Ri;
    } fmt_ldub;
    struct { /* e.g. ld @($R13,$Rj),$Ri */
      SI * i_Rj;
      SI * i_Ri;
      unsigned char in_Rj;
      unsigned char in_h_gr_13;
      unsigned char out_Ri;
    } fmt_ldr13;
    struct { /* e.g. lduh @($R13,$Rj),$Ri */
      SI * i_Rj;
      SI * i_Ri;
      unsigned char in_Rj;
      unsigned char in_h_gr_13;
      unsigned char out_Ri;
    } fmt_ldr13uh;
    struct { /* e.g. ldub @($R13,$Rj),$Ri */
      SI * i_Rj;
      SI * i_Ri;
      unsigned char in_Rj;
      unsigned char in_h_gr_13;
      unsigned char out_Ri;
    } fmt_ldr13ub;
    struct { /* e.g. ld @($R14,$disp10),$Ri */
      SI f_disp10;
      SI * i_Ri;
      unsigned char in_h_gr_14;
      unsigned char out_Ri;
    } fmt_ldr14;
    struct { /* e.g. lduh @($R14,$disp9),$Ri */
      SI f_disp9;
      SI * i_Ri;
      unsigned char in_h_gr_14;
      unsigned char out_Ri;
    } fmt_ldr14uh;
    struct { /* e.g. ldub @($R14,$disp8),$Ri */
      INT f_disp8;
      SI * i_Ri;
      unsigned char in_h_gr_14;
      unsigned char out_Ri;
    } fmt_ldr14ub;
    struct { /* e.g. ld @($R15,$udisp6),$Ri */
      USI f_udisp6;
      SI * i_Ri;
      unsigned char in_h_gr_15;
      unsigned char out_Ri;
    } fmt_ldr15;
    struct { /* e.g. ld @$R15+,$Ri */
      UINT f_Ri;
      SI * i_Ri;
      unsigned char in_h_gr_15;
      unsigned char out_Ri;
      unsigned char out_h_gr_15;
    } fmt_ldr15gr;
    struct { /* e.g. ld @$R15+,$Rs2 */
      UINT f_Rs2;
      unsigned char in_h_gr_15;
      unsigned char out_h_gr_15;
    } fmt_ldr15dr;
    struct { /* e.g. ld @$R15+,$ps */
      int empty;
      unsigned char in_h_gr_15;
      unsigned char out_h_gr_15;
    } fmt_ldr15ps;
    struct { /* e.g. st $Ri,@$Rj */
      SI * i_Ri;
      SI * i_Rj;
      unsigned char in_Ri;
      unsigned char in_Rj;
    } fmt_st;
    struct { /* e.g. sth $Ri,@$Rj */
      SI * i_Ri;
      SI * i_Rj;
      unsigned char in_Ri;
      unsigned char in_Rj;
    } fmt_sth;
    struct { /* e.g. stb $Ri,@$Rj */
      SI * i_Ri;
      SI * i_Rj;
      unsigned char in_Ri;
      unsigned char in_Rj;
    } fmt_stb;
    struct { /* e.g. st $Ri,@($R13,$Rj) */
      SI * i_Ri;
      SI * i_Rj;
      unsigned char in_Ri;
      unsigned char in_Rj;
      unsigned char in_h_gr_13;
    } fmt_str13;
    struct { /* e.g. sth $Ri,@($R13,$Rj) */
      SI * i_Ri;
      SI * i_Rj;
      unsigned char in_Ri;
      unsigned char in_Rj;
      unsigned char in_h_gr_13;
    } fmt_str13h;
    struct { /* e.g. stb $Ri,@($R13,$Rj) */
      SI * i_Ri;
      SI * i_Rj;
      unsigned char in_Ri;
      unsigned char in_Rj;
      unsigned char in_h_gr_13;
    } fmt_str13b;
    struct { /* e.g. st $Ri,@($R14,$disp10) */
      SI f_disp10;
      SI * i_Ri;
      unsigned char in_Ri;
      unsigned char in_h_gr_14;
    } fmt_str14;
    struct { /* e.g. sth $Ri,@($R14,$disp9) */
      SI f_disp9;
      SI * i_Ri;
      unsigned char in_Ri;
      unsigned char in_h_gr_14;
    } fmt_str14h;
    struct { /* e.g. stb $Ri,@($R14,$disp8) */
      INT f_disp8;
      SI * i_Ri;
      unsigned char in_Ri;
      unsigned char in_h_gr_14;
    } fmt_str14b;
    struct { /* e.g. st $Ri,@($R15,$udisp6) */
      USI f_udisp6;
      SI * i_Ri;
      unsigned char in_Ri;
      unsigned char in_h_gr_15;
    } fmt_str15;
    struct { /* e.g. st $Ri,@-$R15 */
      SI * i_Ri;
      unsigned char in_Ri;
      unsigned char in_h_gr_15;
      unsigned char out_h_gr_15;
    } fmt_str15gr;
    struct { /* e.g. st $Rs2,@-$R15 */
      UINT f_Rs2;
      unsigned char in_h_gr_15;
      unsigned char out_h_gr_15;
    } fmt_str15dr;
    struct { /* e.g. st $ps,@-$R15 */
      int empty;
      unsigned char in_h_gr_15;
      unsigned char out_h_gr_15;
    } fmt_str15ps;
    struct { /* e.g. mov $Rj,$Ri */
      SI * i_Rj;
      SI * i_Ri;
      unsigned char in_Rj;
      unsigned char out_Ri;
    } fmt_mov;
    struct { /* e.g. mov $Rs1,$Ri */
      UINT f_Rs1;
      SI * i_Ri;
      unsigned char out_Ri;
    } fmt_movdr;
    struct { /* e.g. mov $ps,$Ri */
      SI * i_Ri;
      unsigned char out_Ri;
    } fmt_movps;
    struct { /* e.g. mov $Ri,$Rs1 */
      UINT f_Rs1;
      SI * i_Ri;
      unsigned char in_Ri;
    } fmt_mov2dr;
    struct { /* e.g. mov $Ri,$ps */
      SI * i_Ri;
      unsigned char in_Ri;
    } fmt_mov2ps;
    struct { /* e.g. bno:d $label9 */
      int empty;
    } fmt_bnod;
    struct { /* e.g. dmov $R13,@$dir10 */
      USI f_dir10;
      unsigned char in_h_gr_13;
    } fmt_dmovr13;
    struct { /* e.g. dmovh $R13,@$dir9 */
      USI f_dir9;
      unsigned char in_h_gr_13;
    } fmt_dmovr13h;
    struct { /* e.g. dmovb $R13,@$dir8 */
      UINT f_dir8;
      unsigned char in_h_gr_13;
    } fmt_dmovr13b;
    struct { /* e.g. dmov @$R13+,@$dir10 */
      USI f_dir10;
      unsigned char in_h_gr_13;
      unsigned char out_h_gr_13;
    } fmt_dmovr13pi;
    struct { /* e.g. dmovh @$R13+,@$dir9 */
      USI f_dir9;
      unsigned char in_h_gr_13;
      unsigned char out_h_gr_13;
    } fmt_dmovr13pih;
    struct { /* e.g. dmovb @$R13+,@$dir8 */
      UINT f_dir8;
      unsigned char in_h_gr_13;
      unsigned char out_h_gr_13;
    } fmt_dmovr13pib;
    struct { /* e.g. dmov @$R15+,@$dir10 */
      USI f_dir10;
      unsigned char in_h_gr_15;
      unsigned char out_h_gr_15;
    } fmt_dmovr15pi;
    struct { /* e.g. dmov @$dir10,$R13 */
      USI f_dir10;
      unsigned char out_h_gr_13;
    } fmt_dmov2r13;
    struct { /* e.g. dmovh @$dir9,$R13 */
      USI f_dir9;
      unsigned char out_h_gr_13;
    } fmt_dmov2r13h;
    struct { /* e.g. dmovb @$dir8,$R13 */
      UINT f_dir8;
      unsigned char out_h_gr_13;
    } fmt_dmov2r13b;
    struct { /* e.g. dmov @$dir10,@$R13+ */
      USI f_dir10;
      unsigned char in_h_gr_13;
      unsigned char out_h_gr_13;
    } fmt_dmov2r13pi;
    struct { /* e.g. dmovh @$dir9,@$R13+ */
      USI f_dir9;
      unsigned char in_h_gr_13;
      unsigned char out_h_gr_13;
    } fmt_dmov2r13pih;
    struct { /* e.g. dmovb @$dir8,@$R13+ */
      UINT f_dir8;
      unsigned char in_h_gr_13;
      unsigned char out_h_gr_13;
    } fmt_dmov2r13pib;
    struct { /* e.g. dmov @$dir10,@-$R15 */
      USI f_dir10;
      unsigned char in_h_gr_15;
      unsigned char out_h_gr_15;
    } fmt_dmov2r15pd;
    struct { /* e.g. ldres @$Ri+,$u4 */
      SI * i_Ri;
      unsigned char in_Ri;
      unsigned char out_Ri;
    } fmt_ldres;
    struct { /* e.g. copop $u4c,$ccc,$CRj,$CRi */
      int empty;
    } fmt_copop;
    struct { /* e.g. copld $u4c,$ccc,$Rjc,$CRi */
      int empty;
    } fmt_copld;
    struct { /* e.g. copst $u4c,$ccc,$CRj,$Ric */
      int empty;
    } fmt_copst;
    struct { /* e.g. nop */
      int empty;
    } fmt_nop;
    struct { /* e.g. andccr $u8 */
      UINT f_u8;
    } fmt_andccr;
    struct { /* e.g. stilm $u8 */
      UINT f_u8;
    } fmt_stilm;
    struct { /* e.g. addsp $s10 */
      SI f_s10;
      unsigned char in_h_gr_15;
      unsigned char out_h_gr_15;
    } fmt_addsp;
    struct { /* e.g. extsb $Ri */
      SI * i_Ri;
      unsigned char in_Ri;
      unsigned char out_Ri;
    } fmt_extsb;
    struct { /* e.g. extub $Ri */
      SI * i_Ri;
      unsigned char in_Ri;
      unsigned char out_Ri;
    } fmt_extub;
    struct { /* e.g. extsh $Ri */
      SI * i_Ri;
      unsigned char in_Ri;
      unsigned char out_Ri;
    } fmt_extsh;
    struct { /* e.g. extuh $Ri */
      SI * i_Ri;
      unsigned char in_Ri;
      unsigned char out_Ri;
    } fmt_extuh;
    struct { /* e.g. ldm0 ($reglist_low_ld) */
      UINT f_reglist_low_ld;
      unsigned char in_h_gr_15;
      unsigned char out_h_gr_0;
      unsigned char out_h_gr_1;
      unsigned char out_h_gr_15;
      unsigned char out_h_gr_2;
      unsigned char out_h_gr_3;
      unsigned char out_h_gr_4;
      unsigned char out_h_gr_5;
      unsigned char out_h_gr_6;
      unsigned char out_h_gr_7;
    } fmt_ldm0;
    struct { /* e.g. ldm1 ($reglist_hi_ld) */
      UINT f_reglist_hi_ld;
      unsigned char in_h_gr_15;
      unsigned char out_h_gr_10;
      unsigned char out_h_gr_11;
      unsigned char out_h_gr_12;
      unsigned char out_h_gr_13;
      unsigned char out_h_gr_14;
      unsigned char out_h_gr_15;
      unsigned char out_h_gr_8;
      unsigned char out_h_gr_9;
    } fmt_ldm1;
    struct { /* e.g. stm0 ($reglist_low_st) */
      UINT f_reglist_low_st;
      unsigned char in_h_gr_0;
      unsigned char in_h_gr_1;
      unsigned char in_h_gr_15;
      unsigned char in_h_gr_2;
      unsigned char in_h_gr_3;
      unsigned char in_h_gr_4;
      unsigned char in_h_gr_5;
      unsigned char in_h_gr_6;
      unsigned char in_h_gr_7;
      unsigned char out_h_gr_15;
    } fmt_stm0;
    struct { /* e.g. stm1 ($reglist_hi_st) */
      UINT f_reglist_hi_st;
      unsigned char in_h_gr_10;
      unsigned char in_h_gr_11;
      unsigned char in_h_gr_12;
      unsigned char in_h_gr_13;
      unsigned char in_h_gr_14;
      unsigned char in_h_gr_15;
      unsigned char in_h_gr_8;
      unsigned char in_h_gr_9;
      unsigned char out_h_gr_15;
    } fmt_stm1;
    struct { /* e.g. enter $u10 */
      USI f_u10;
      unsigned char in_h_gr_14;
      unsigned char in_h_gr_15;
      unsigned char out_h_gr_14;
      unsigned char out_h_gr_15;
    } fmt_enter;
    struct { /* e.g. leave */
      int empty;
      unsigned char in_h_gr_14;
      unsigned char in_h_gr_15;
      unsigned char out_h_gr_14;
      unsigned char out_h_gr_15;
    } fmt_leave;
    struct { /* e.g. xchb @$Rj,$Ri */
      SI * i_Ri;
      SI * i_Rj;
      unsigned char in_Ri;
      unsigned char in_Rj;
      unsigned char out_Ri;
    } fmt_xchb;
  /* cti insns, kept separately so addr_cache is in fixed place */
  struct {
    union {
    struct { /* e.g. jmp @$Ri */
      SI * i_Ri;
      unsigned char in_Ri;
    } fmt_jmp;
    struct { /* e.g. call @$Ri */
      SI * i_Ri;
      unsigned char in_Ri;
    } fmt_callr;
    struct { /* e.g. call $label12 */
      IADDR i_label12;
    } fmt_call;
    struct { /* e.g. ret */
      int empty;
    } fmt_ret;
    struct { /* e.g. int $u8 */
      UINT f_u8;
    } fmt_int;
    struct { /* e.g. inte */
      int empty;
    } fmt_inte;
    struct { /* e.g. reti */
      int empty;
    } fmt_reti;
    struct { /* e.g. bra:d $label9 */
      IADDR i_label9;
    } fmt_brad;
    struct { /* e.g. beq:d $label9 */
      IADDR i_label9;
    } fmt_beqd;
    struct { /* e.g. bc:d $label9 */
      IADDR i_label9;
    } fmt_bcd;
    struct { /* e.g. bn:d $label9 */
      IADDR i_label9;
    } fmt_bnd;
    struct { /* e.g. bv:d $label9 */
      IADDR i_label9;
    } fmt_bvd;
    struct { /* e.g. blt:d $label9 */
      IADDR i_label9;
    } fmt_bltd;
    struct { /* e.g. ble:d $label9 */
      IADDR i_label9;
    } fmt_bled;
    struct { /* e.g. bls:d $label9 */
      IADDR i_label9;
    } fmt_blsd;
    } fields;
#if WITH_SCACHE_PBB
    SEM_PC addr_cache;
#endif
  } cti;
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
  /* Instruction fields.  */ \
  unsigned int length;
#define EXTRACT_IFMT_EMPTY_CODE \
  length = 0; \

#define EXTRACT_IFMT_ADD_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_op2; \
  UINT f_Rj; \
  UINT f_Ri; \
  unsigned int length;
#define EXTRACT_IFMT_ADD_CODE \
  length = 2; \
  f_op1 = EXTRACT_UINT (insn, 16, 0, 4); \
  f_op2 = EXTRACT_UINT (insn, 16, 4, 4); \
  f_Rj = EXTRACT_UINT (insn, 16, 8, 4); \
  f_Ri = EXTRACT_UINT (insn, 16, 12, 4); \

#define EXTRACT_IFMT_ADDI_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_op2; \
  UINT f_u4; \
  UINT f_Ri; \
  unsigned int length;
#define EXTRACT_IFMT_ADDI_CODE \
  length = 2; \
  f_op1 = EXTRACT_UINT (insn, 16, 0, 4); \
  f_op2 = EXTRACT_UINT (insn, 16, 4, 4); \
  f_u4 = EXTRACT_UINT (insn, 16, 8, 4); \
  f_Ri = EXTRACT_UINT (insn, 16, 12, 4); \

#define EXTRACT_IFMT_ADD2_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_op2; \
  SI f_m4; \
  UINT f_Ri; \
  unsigned int length;
#define EXTRACT_IFMT_ADD2_CODE \
  length = 2; \
  f_op1 = EXTRACT_UINT (insn, 16, 0, 4); \
  f_op2 = EXTRACT_UINT (insn, 16, 4, 4); \
  f_m4 = ((EXTRACT_UINT (insn, 16, 8, 4)) | (((-1) << (4)))); \
  f_Ri = EXTRACT_UINT (insn, 16, 12, 4); \

#define EXTRACT_IFMT_DIV0S_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_op2; \
  UINT f_op3; \
  UINT f_Ri; \
  unsigned int length;
#define EXTRACT_IFMT_DIV0S_CODE \
  length = 2; \
  f_op1 = EXTRACT_UINT (insn, 16, 0, 4); \
  f_op2 = EXTRACT_UINT (insn, 16, 4, 4); \
  f_op3 = EXTRACT_UINT (insn, 16, 8, 4); \
  f_Ri = EXTRACT_UINT (insn, 16, 12, 4); \

#define EXTRACT_IFMT_DIV3_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_op2; \
  UINT f_op3; \
  UINT f_op4; \
  unsigned int length;
#define EXTRACT_IFMT_DIV3_CODE \
  length = 2; \
  f_op1 = EXTRACT_UINT (insn, 16, 0, 4); \
  f_op2 = EXTRACT_UINT (insn, 16, 4, 4); \
  f_op3 = EXTRACT_UINT (insn, 16, 8, 4); \
  f_op4 = EXTRACT_UINT (insn, 16, 12, 4); \

#define EXTRACT_IFMT_LDI8_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_i8; \
  UINT f_Ri; \
  unsigned int length;
#define EXTRACT_IFMT_LDI8_CODE \
  length = 2; \
  f_op1 = EXTRACT_UINT (insn, 16, 0, 4); \
  f_i8 = EXTRACT_UINT (insn, 16, 4, 8); \
  f_Ri = EXTRACT_UINT (insn, 16, 12, 4); \

#define EXTRACT_IFMT_LDI20_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_i20; \
  UINT f_i20_4; \
  UINT f_i20_16; \
  UINT f_op2; \
  UINT f_Ri; \
  /* Contents of trailing part of insn.  */ \
  UINT word_1; \
  unsigned int length;
#define EXTRACT_IFMT_LDI20_CODE \
  length = 4; \
  word_1 = GETIMEMUHI (current_cpu, pc + 2); \
  f_op1 = EXTRACT_UINT (insn, 16, 0, 4); \
  f_i20_4 = EXTRACT_UINT (insn, 16, 8, 4); \
  f_i20_16 = (0|(EXTRACT_UINT (word_1, 16, 0, 16) << 0)); \
do {\
  f_i20 = ((((f_i20_4) << (16))) | (f_i20_16));\
} while (0);\
  f_op2 = EXTRACT_UINT (insn, 16, 4, 4); \
  f_Ri = EXTRACT_UINT (insn, 16, 12, 4); \

#define EXTRACT_IFMT_LDI32_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_i32; \
  UINT f_op2; \
  UINT f_op3; \
  UINT f_Ri; \
  /* Contents of trailing part of insn.  */ \
  UINT word_1; \
  unsigned int length;
#define EXTRACT_IFMT_LDI32_CODE \
  length = 6; \
  word_1 = GETIMEMUSI (current_cpu, pc + 2); \
  f_op1 = EXTRACT_UINT (insn, 16, 0, 4); \
  f_i32 = (0|(EXTRACT_UINT (word_1, 32, 0, 32) << 0)); \
  f_op2 = EXTRACT_UINT (insn, 16, 4, 4); \
  f_op3 = EXTRACT_UINT (insn, 16, 8, 4); \
  f_Ri = EXTRACT_UINT (insn, 16, 12, 4); \

#define EXTRACT_IFMT_LDR14_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  SI f_disp10; \
  UINT f_Ri; \
  unsigned int length;
#define EXTRACT_IFMT_LDR14_CODE \
  length = 2; \
  f_op1 = EXTRACT_UINT (insn, 16, 0, 4); \
  f_disp10 = ((EXTRACT_INT (insn, 16, 4, 8)) << (2)); \
  f_Ri = EXTRACT_UINT (insn, 16, 12, 4); \

#define EXTRACT_IFMT_LDR14UH_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  SI f_disp9; \
  UINT f_Ri; \
  unsigned int length;
#define EXTRACT_IFMT_LDR14UH_CODE \
  length = 2; \
  f_op1 = EXTRACT_UINT (insn, 16, 0, 4); \
  f_disp9 = ((EXTRACT_INT (insn, 16, 4, 8)) << (1)); \
  f_Ri = EXTRACT_UINT (insn, 16, 12, 4); \

#define EXTRACT_IFMT_LDR14UB_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  INT f_disp8; \
  UINT f_Ri; \
  unsigned int length;
#define EXTRACT_IFMT_LDR14UB_CODE \
  length = 2; \
  f_op1 = EXTRACT_UINT (insn, 16, 0, 4); \
  f_disp8 = EXTRACT_INT (insn, 16, 4, 8); \
  f_Ri = EXTRACT_UINT (insn, 16, 12, 4); \

#define EXTRACT_IFMT_LDR15_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_op2; \
  USI f_udisp6; \
  UINT f_Ri; \
  unsigned int length;
#define EXTRACT_IFMT_LDR15_CODE \
  length = 2; \
  f_op1 = EXTRACT_UINT (insn, 16, 0, 4); \
  f_op2 = EXTRACT_UINT (insn, 16, 4, 4); \
  f_udisp6 = ((EXTRACT_UINT (insn, 16, 8, 4)) << (2)); \
  f_Ri = EXTRACT_UINT (insn, 16, 12, 4); \

#define EXTRACT_IFMT_LDR15DR_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_op2; \
  UINT f_op3; \
  UINT f_Rs2; \
  unsigned int length;
#define EXTRACT_IFMT_LDR15DR_CODE \
  length = 2; \
  f_op1 = EXTRACT_UINT (insn, 16, 0, 4); \
  f_op2 = EXTRACT_UINT (insn, 16, 4, 4); \
  f_op3 = EXTRACT_UINT (insn, 16, 8, 4); \
  f_Rs2 = EXTRACT_UINT (insn, 16, 12, 4); \

#define EXTRACT_IFMT_MOVDR_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_op2; \
  UINT f_Rs1; \
  UINT f_Ri; \
  unsigned int length;
#define EXTRACT_IFMT_MOVDR_CODE \
  length = 2; \
  f_op1 = EXTRACT_UINT (insn, 16, 0, 4); \
  f_op2 = EXTRACT_UINT (insn, 16, 4, 4); \
  f_Rs1 = EXTRACT_UINT (insn, 16, 8, 4); \
  f_Ri = EXTRACT_UINT (insn, 16, 12, 4); \

#define EXTRACT_IFMT_CALL_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_op5; \
  SI f_rel12; \
  unsigned int length;
#define EXTRACT_IFMT_CALL_CODE \
  length = 2; \
  f_op1 = EXTRACT_UINT (insn, 16, 0, 4); \
  f_op5 = EXTRACT_UINT (insn, 16, 4, 1); \
  f_rel12 = ((((EXTRACT_INT (insn, 16, 5, 11)) << (1))) + (((pc) + (2)))); \

#define EXTRACT_IFMT_INT_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_op2; \
  UINT f_u8; \
  unsigned int length;
#define EXTRACT_IFMT_INT_CODE \
  length = 2; \
  f_op1 = EXTRACT_UINT (insn, 16, 0, 4); \
  f_op2 = EXTRACT_UINT (insn, 16, 4, 4); \
  f_u8 = EXTRACT_UINT (insn, 16, 8, 8); \

#define EXTRACT_IFMT_BRAD_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_cc; \
  SI f_rel9; \
  unsigned int length;
#define EXTRACT_IFMT_BRAD_CODE \
  length = 2; \
  f_op1 = EXTRACT_UINT (insn, 16, 0, 4); \
  f_cc = EXTRACT_UINT (insn, 16, 4, 4); \
  f_rel9 = ((((EXTRACT_INT (insn, 16, 8, 8)) << (1))) + (((pc) + (2)))); \

#define EXTRACT_IFMT_DMOVR13_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_op2; \
  USI f_dir10; \
  unsigned int length;
#define EXTRACT_IFMT_DMOVR13_CODE \
  length = 2; \
  f_op1 = EXTRACT_UINT (insn, 16, 0, 4); \
  f_op2 = EXTRACT_UINT (insn, 16, 4, 4); \
  f_dir10 = ((EXTRACT_UINT (insn, 16, 8, 8)) << (2)); \

#define EXTRACT_IFMT_DMOVR13H_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_op2; \
  USI f_dir9; \
  unsigned int length;
#define EXTRACT_IFMT_DMOVR13H_CODE \
  length = 2; \
  f_op1 = EXTRACT_UINT (insn, 16, 0, 4); \
  f_op2 = EXTRACT_UINT (insn, 16, 4, 4); \
  f_dir9 = ((EXTRACT_UINT (insn, 16, 8, 8)) << (1)); \

#define EXTRACT_IFMT_DMOVR13B_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_op2; \
  UINT f_dir8; \
  unsigned int length;
#define EXTRACT_IFMT_DMOVR13B_CODE \
  length = 2; \
  f_op1 = EXTRACT_UINT (insn, 16, 0, 4); \
  f_op2 = EXTRACT_UINT (insn, 16, 4, 4); \
  f_dir8 = EXTRACT_UINT (insn, 16, 8, 8); \

#define EXTRACT_IFMT_COPOP_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_ccc; \
  UINT f_op2; \
  UINT f_op3; \
  UINT f_CRj; \
  UINT f_u4c; \
  UINT f_CRi; \
  /* Contents of trailing part of insn.  */ \
  UINT word_1; \
  unsigned int length;
#define EXTRACT_IFMT_COPOP_CODE \
  length = 4; \
  word_1 = GETIMEMUHI (current_cpu, pc + 2); \
  f_op1 = EXTRACT_UINT (insn, 16, 0, 4); \
  f_ccc = (0|(EXTRACT_UINT (word_1, 16, 0, 8) << 0)); \
  f_op2 = EXTRACT_UINT (insn, 16, 4, 4); \
  f_op3 = EXTRACT_UINT (insn, 16, 8, 4); \
  f_CRj = (0|(EXTRACT_UINT (word_1, 16, 8, 4) << 0)); \
  f_u4c = EXTRACT_UINT (insn, 16, 12, 4); \
  f_CRi = (0|(EXTRACT_UINT (word_1, 16, 12, 16) << 0)); \

#define EXTRACT_IFMT_COPLD_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_ccc; \
  UINT f_op2; \
  UINT f_op3; \
  UINT f_Rjc; \
  UINT f_u4c; \
  UINT f_CRi; \
  /* Contents of trailing part of insn.  */ \
  UINT word_1; \
  unsigned int length;
#define EXTRACT_IFMT_COPLD_CODE \
  length = 4; \
  word_1 = GETIMEMUHI (current_cpu, pc + 2); \
  f_op1 = EXTRACT_UINT (insn, 16, 0, 4); \
  f_ccc = (0|(EXTRACT_UINT (word_1, 16, 0, 8) << 0)); \
  f_op2 = EXTRACT_UINT (insn, 16, 4, 4); \
  f_op3 = EXTRACT_UINT (insn, 16, 8, 4); \
  f_Rjc = (0|(EXTRACT_UINT (word_1, 16, 8, 4) << 0)); \
  f_u4c = EXTRACT_UINT (insn, 16, 12, 4); \
  f_CRi = (0|(EXTRACT_UINT (word_1, 16, 12, 16) << 0)); \

#define EXTRACT_IFMT_COPST_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_ccc; \
  UINT f_op2; \
  UINT f_op3; \
  UINT f_CRj; \
  UINT f_u4c; \
  UINT f_Ric; \
  /* Contents of trailing part of insn.  */ \
  UINT word_1; \
  unsigned int length;
#define EXTRACT_IFMT_COPST_CODE \
  length = 4; \
  word_1 = GETIMEMUHI (current_cpu, pc + 2); \
  f_op1 = EXTRACT_UINT (insn, 16, 0, 4); \
  f_ccc = (0|(EXTRACT_UINT (word_1, 16, 0, 8) << 0)); \
  f_op2 = EXTRACT_UINT (insn, 16, 4, 4); \
  f_op3 = EXTRACT_UINT (insn, 16, 8, 4); \
  f_CRj = (0|(EXTRACT_UINT (word_1, 16, 8, 4) << 0)); \
  f_u4c = EXTRACT_UINT (insn, 16, 12, 4); \
  f_Ric = (0|(EXTRACT_UINT (word_1, 16, 12, 16) << 0)); \

#define EXTRACT_IFMT_ADDSP_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_op2; \
  SI f_s10; \
  unsigned int length;
#define EXTRACT_IFMT_ADDSP_CODE \
  length = 2; \
  f_op1 = EXTRACT_UINT (insn, 16, 0, 4); \
  f_op2 = EXTRACT_UINT (insn, 16, 4, 4); \
  f_s10 = ((EXTRACT_INT (insn, 16, 8, 8)) << (2)); \

#define EXTRACT_IFMT_LDM0_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_op2; \
  UINT f_reglist_low_ld; \
  unsigned int length;
#define EXTRACT_IFMT_LDM0_CODE \
  length = 2; \
  f_op1 = EXTRACT_UINT (insn, 16, 0, 4); \
  f_op2 = EXTRACT_UINT (insn, 16, 4, 4); \
  f_reglist_low_ld = EXTRACT_UINT (insn, 16, 8, 8); \

#define EXTRACT_IFMT_LDM1_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_op2; \
  UINT f_reglist_hi_ld; \
  unsigned int length;
#define EXTRACT_IFMT_LDM1_CODE \
  length = 2; \
  f_op1 = EXTRACT_UINT (insn, 16, 0, 4); \
  f_op2 = EXTRACT_UINT (insn, 16, 4, 4); \
  f_reglist_hi_ld = EXTRACT_UINT (insn, 16, 8, 8); \

#define EXTRACT_IFMT_STM0_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_op2; \
  UINT f_reglist_low_st; \
  unsigned int length;
#define EXTRACT_IFMT_STM0_CODE \
  length = 2; \
  f_op1 = EXTRACT_UINT (insn, 16, 0, 4); \
  f_op2 = EXTRACT_UINT (insn, 16, 4, 4); \
  f_reglist_low_st = EXTRACT_UINT (insn, 16, 8, 8); \

#define EXTRACT_IFMT_STM1_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_op2; \
  UINT f_reglist_hi_st; \
  unsigned int length;
#define EXTRACT_IFMT_STM1_CODE \
  length = 2; \
  f_op1 = EXTRACT_UINT (insn, 16, 0, 4); \
  f_op2 = EXTRACT_UINT (insn, 16, 4, 4); \
  f_reglist_hi_st = EXTRACT_UINT (insn, 16, 8, 8); \

#define EXTRACT_IFMT_ENTER_VARS \
  /* Instruction fields.  */ \
  UINT f_op1; \
  UINT f_op2; \
  USI f_u10; \
  unsigned int length;
#define EXTRACT_IFMT_ENTER_CODE \
  length = 2; \
  f_op1 = EXTRACT_UINT (insn, 16, 0, 4); \
  f_op2 = EXTRACT_UINT (insn, 16, 4, 4); \
  f_u10 = ((EXTRACT_UINT (insn, 16, 8, 8)) << (2)); \

/* Collection of various things for the trace handler to use.  */

typedef struct trace_record {
  IADDR pc;
  /* FIXME:wip */
} TRACE_RECORD;

#endif /* CPU_FR30BF_H */
