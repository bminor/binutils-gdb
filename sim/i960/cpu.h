/* CPU family header for i960base.

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

#ifndef CPU_I960BASE_H
#define CPU_I960BASE_H

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
  SI h_gr[32];
#define GET_H_GR(a1) CPU (h_gr)[a1]
#define SET_H_GR(a1, x) (CPU (h_gr)[a1] = (x))
  /* condition code */
  SI h_cc;
#define GET_H_CC() CPU (h_cc)
#define SET_H_CC(x) (CPU (h_cc) = (x))
  } hardware;
#define CPU_CGEN_HW(cpu) (& (cpu)->cpu_data.hardware)
} I960BASE_CPU_DATA;

/* Cover fns for register access.  */
USI i960base_h_pc_get (SIM_CPU *);
void i960base_h_pc_set (SIM_CPU *, USI);
SI i960base_h_gr_get (SIM_CPU *, UINT);
void i960base_h_gr_set (SIM_CPU *, UINT, SI);
SI i960base_h_cc_get (SIM_CPU *);
void i960base_h_cc_set (SIM_CPU *, SI);

/* These must be hand-written.  */
extern CPUREG_FETCH_FN i960base_fetch_register;
extern CPUREG_STORE_FN i960base_store_register;

typedef struct {
  int empty;
} MODEL_I960KA_DATA;

typedef struct {
  int empty;
} MODEL_I960CA_DATA;

union sem_fields {
    struct { /* empty sformat for unspecified field list */
      int empty;
    } fmt_empty;
    struct { /* e.g. mulo $src1, $src2, $dst */
      SI * i_src1;
      SI * i_src2;
      SI * i_dst;
      unsigned char in_src1;
      unsigned char in_src2;
      unsigned char out_dst;
    } fmt_mulo;
    struct { /* e.g. mulo $lit1, $src2, $dst */
      UINT f_src1;
      SI * i_src2;
      SI * i_dst;
      unsigned char in_src2;
      unsigned char out_dst;
    } fmt_mulo1;
    struct { /* e.g. mulo $src1, $lit2, $dst */
      UINT f_src2;
      SI * i_src1;
      SI * i_dst;
      unsigned char in_src1;
      unsigned char out_dst;
    } fmt_mulo2;
    struct { /* e.g. mulo $lit1, $lit2, $dst */
      UINT f_src1;
      UINT f_src2;
      SI * i_dst;
      unsigned char out_dst;
    } fmt_mulo3;
    struct { /* e.g. notbit $src1, $src2, $dst */
      SI * i_src1;
      SI * i_src2;
      SI * i_dst;
      unsigned char in_src1;
      unsigned char in_src2;
      unsigned char out_dst;
    } fmt_notbit;
    struct { /* e.g. notbit $lit1, $src2, $dst */
      UINT f_src1;
      SI * i_src2;
      SI * i_dst;
      unsigned char in_src2;
      unsigned char out_dst;
    } fmt_notbit1;
    struct { /* e.g. notbit $src1, $lit2, $dst */
      UINT f_src2;
      SI * i_src1;
      SI * i_dst;
      unsigned char in_src1;
      unsigned char out_dst;
    } fmt_notbit2;
    struct { /* e.g. notbit $lit1, $lit2, $dst */
      UINT f_src1;
      UINT f_src2;
      SI * i_dst;
      unsigned char out_dst;
    } fmt_notbit3;
    struct { /* e.g. not $src1, $src2, $dst */
      SI * i_src1;
      SI * i_dst;
      unsigned char in_src1;
      unsigned char out_dst;
    } fmt_not;
    struct { /* e.g. not $lit1, $src2, $dst */
      UINT f_src1;
      SI * i_dst;
      unsigned char out_dst;
    } fmt_not1;
    struct { /* e.g. not $src1, $lit2, $dst */
      SI * i_src1;
      SI * i_dst;
      unsigned char in_src1;
      unsigned char out_dst;
    } fmt_not2;
    struct { /* e.g. not $lit1, $lit2, $dst */
      UINT f_src1;
      SI * i_dst;
      unsigned char out_dst;
    } fmt_not3;
    struct { /* e.g. shlo $src1, $src2, $dst */
      SI * i_src1;
      SI * i_src2;
      SI * i_dst;
      unsigned char in_src1;
      unsigned char in_src2;
      unsigned char out_dst;
    } fmt_shlo;
    struct { /* e.g. shlo $lit1, $src2, $dst */
      UINT f_src1;
      SI * i_src2;
      SI * i_dst;
      unsigned char in_src2;
      unsigned char out_dst;
    } fmt_shlo1;
    struct { /* e.g. shlo $src1, $lit2, $dst */
      UINT f_src2;
      SI * i_src1;
      SI * i_dst;
      unsigned char in_src1;
      unsigned char out_dst;
    } fmt_shlo2;
    struct { /* e.g. shlo $lit1, $lit2, $dst */
      UINT f_src1;
      UINT f_src2;
      SI * i_dst;
      unsigned char out_dst;
    } fmt_shlo3;
    struct { /* e.g. emul $src1, $src2, $dst */
      UINT f_srcdst;
      SI * i_src1;
      SI * i_src2;
      SI * i_dst;
      unsigned char in_src1;
      unsigned char in_src2;
      unsigned char out_dst;
      unsigned char out_h_gr_add__VM_index_of_dst_1;
    } fmt_emul;
    struct { /* e.g. emul $lit1, $src2, $dst */
      UINT f_srcdst;
      UINT f_src1;
      SI * i_src2;
      SI * i_dst;
      unsigned char in_src2;
      unsigned char out_dst;
      unsigned char out_h_gr_add__VM_index_of_dst_1;
    } fmt_emul1;
    struct { /* e.g. emul $src1, $lit2, $dst */
      UINT f_srcdst;
      UINT f_src2;
      SI * i_src1;
      SI * i_dst;
      unsigned char in_src1;
      unsigned char out_dst;
      unsigned char out_h_gr_add__VM_index_of_dst_1;
    } fmt_emul2;
    struct { /* e.g. emul $lit1, $lit2, $dst */
      UINT f_srcdst;
      UINT f_src1;
      UINT f_src2;
      SI * i_dst;
      unsigned char out_dst;
      unsigned char out_h_gr_add__VM_index_of_dst_1;
    } fmt_emul3;
    struct { /* e.g. movl $src1, $dst */
      UINT f_src1;
      UINT f_srcdst;
      SI * i_src1;
      SI * i_dst;
      unsigned char in_h_gr_add__VM_index_of_src1_1;
      unsigned char in_src1;
      unsigned char out_dst;
      unsigned char out_h_gr_add__VM_index_of_dst_1;
    } fmt_movl;
    struct { /* e.g. movl $lit1, $dst */
      UINT f_srcdst;
      UINT f_src1;
      SI * i_dst;
      unsigned char out_dst;
      unsigned char out_h_gr_add__VM_index_of_dst_1;
    } fmt_movl1;
    struct { /* e.g. movt $src1, $dst */
      UINT f_src1;
      UINT f_srcdst;
      SI * i_src1;
      SI * i_dst;
      unsigned char in_h_gr_add__VM_index_of_src1_1;
      unsigned char in_h_gr_add__VM_index_of_src1_2;
      unsigned char in_src1;
      unsigned char out_dst;
      unsigned char out_h_gr_add__VM_index_of_dst_1;
      unsigned char out_h_gr_add__VM_index_of_dst_2;
    } fmt_movt;
    struct { /* e.g. movt $lit1, $dst */
      UINT f_srcdst;
      UINT f_src1;
      SI * i_dst;
      unsigned char out_dst;
      unsigned char out_h_gr_add__VM_index_of_dst_1;
      unsigned char out_h_gr_add__VM_index_of_dst_2;
    } fmt_movt1;
    struct { /* e.g. movq $src1, $dst */
      UINT f_src1;
      UINT f_srcdst;
      SI * i_src1;
      SI * i_dst;
      unsigned char in_h_gr_add__VM_index_of_src1_1;
      unsigned char in_h_gr_add__VM_index_of_src1_2;
      unsigned char in_h_gr_add__VM_index_of_src1_3;
      unsigned char in_src1;
      unsigned char out_dst;
      unsigned char out_h_gr_add__VM_index_of_dst_1;
      unsigned char out_h_gr_add__VM_index_of_dst_2;
      unsigned char out_h_gr_add__VM_index_of_dst_3;
    } fmt_movq;
    struct { /* e.g. movq $lit1, $dst */
      UINT f_srcdst;
      UINT f_src1;
      SI * i_dst;
      unsigned char out_dst;
      unsigned char out_h_gr_add__VM_index_of_dst_1;
      unsigned char out_h_gr_add__VM_index_of_dst_2;
      unsigned char out_h_gr_add__VM_index_of_dst_3;
    } fmt_movq1;
    struct { /* e.g. modpc $src1, $src2, $dst */
      SI * i_src2;
      SI * i_dst;
      unsigned char in_src2;
      unsigned char out_dst;
    } fmt_modpc;
    struct { /* e.g. lda $offset, $dst */
      UINT f_offset;
      SI * i_dst;
      unsigned char out_dst;
    } fmt_lda_offset;
    struct { /* e.g. lda $offset($abase), $dst */
      UINT f_offset;
      SI * i_abase;
      SI * i_dst;
      unsigned char in_abase;
      unsigned char out_dst;
    } fmt_lda_indirect_offset;
    struct { /* e.g. lda ($abase), $dst */
      SI * i_abase;
      SI * i_dst;
      unsigned char in_abase;
      unsigned char out_dst;
    } fmt_lda_indirect;
    struct { /* e.g. lda ($abase)[$index*S$scale], $dst */
      UINT f_scale;
      SI * i_abase;
      SI * i_index;
      SI * i_dst;
      unsigned char in_abase;
      unsigned char in_index;
      unsigned char out_dst;
    } fmt_lda_indirect_index;
    struct { /* e.g. lda $optdisp, $dst */
      UINT f_optdisp;
      SI * i_dst;
      unsigned char out_dst;
    } fmt_lda_disp;
    struct { /* e.g. lda $optdisp($abase), $dst */
      UINT f_optdisp;
      SI * i_abase;
      SI * i_dst;
      unsigned char in_abase;
      unsigned char out_dst;
    } fmt_lda_indirect_disp;
    struct { /* e.g. lda $optdisp[$index*S$scale], $dst */
      UINT f_optdisp;
      UINT f_scale;
      SI * i_index;
      SI * i_dst;
      unsigned char in_index;
      unsigned char out_dst;
    } fmt_lda_index_disp;
    struct { /* e.g. lda $optdisp($abase)[$index*S$scale], $dst */
      UINT f_optdisp;
      UINT f_scale;
      SI * i_abase;
      SI * i_index;
      SI * i_dst;
      unsigned char in_abase;
      unsigned char in_index;
      unsigned char out_dst;
    } fmt_lda_indirect_index_disp;
    struct { /* e.g. ld $offset, $dst */
      UINT f_offset;
      SI * i_dst;
      unsigned char out_dst;
    } fmt_ld_offset;
    struct { /* e.g. ld $offset($abase), $dst */
      UINT f_offset;
      SI * i_abase;
      SI * i_dst;
      unsigned char in_abase;
      unsigned char out_dst;
    } fmt_ld_indirect_offset;
    struct { /* e.g. ld ($abase), $dst */
      SI * i_abase;
      SI * i_dst;
      unsigned char in_abase;
      unsigned char out_dst;
    } fmt_ld_indirect;
    struct { /* e.g. ld ($abase)[$index*S$scale], $dst */
      UINT f_scale;
      SI * i_abase;
      SI * i_index;
      SI * i_dst;
      unsigned char in_abase;
      unsigned char in_index;
      unsigned char out_dst;
    } fmt_ld_indirect_index;
    struct { /* e.g. ld $optdisp, $dst */
      UINT f_optdisp;
      SI * i_dst;
      unsigned char out_dst;
    } fmt_ld_disp;
    struct { /* e.g. ld $optdisp($abase), $dst */
      UINT f_optdisp;
      SI * i_abase;
      SI * i_dst;
      unsigned char in_abase;
      unsigned char out_dst;
    } fmt_ld_indirect_disp;
    struct { /* e.g. ld $optdisp[$index*S$scale], $dst */
      UINT f_optdisp;
      UINT f_scale;
      SI * i_index;
      SI * i_dst;
      unsigned char in_index;
      unsigned char out_dst;
    } fmt_ld_index_disp;
    struct { /* e.g. ld $optdisp($abase)[$index*S$scale], $dst */
      UINT f_optdisp;
      UINT f_scale;
      SI * i_abase;
      SI * i_index;
      SI * i_dst;
      unsigned char in_abase;
      unsigned char in_index;
      unsigned char out_dst;
    } fmt_ld_indirect_index_disp;
    struct { /* e.g. ldob $offset, $dst */
      UINT f_offset;
      SI * i_dst;
      unsigned char out_dst;
    } fmt_ldob_offset;
    struct { /* e.g. ldob $offset($abase), $dst */
      UINT f_offset;
      SI * i_abase;
      SI * i_dst;
      unsigned char in_abase;
      unsigned char out_dst;
    } fmt_ldob_indirect_offset;
    struct { /* e.g. ldob ($abase), $dst */
      SI * i_abase;
      SI * i_dst;
      unsigned char in_abase;
      unsigned char out_dst;
    } fmt_ldob_indirect;
    struct { /* e.g. ldob ($abase)[$index*S$scale], $dst */
      UINT f_scale;
      SI * i_abase;
      SI * i_index;
      SI * i_dst;
      unsigned char in_abase;
      unsigned char in_index;
      unsigned char out_dst;
    } fmt_ldob_indirect_index;
    struct { /* e.g. ldob $optdisp, $dst */
      UINT f_optdisp;
      SI * i_dst;
      unsigned char out_dst;
    } fmt_ldob_disp;
    struct { /* e.g. ldob $optdisp($abase), $dst */
      UINT f_optdisp;
      SI * i_abase;
      SI * i_dst;
      unsigned char in_abase;
      unsigned char out_dst;
    } fmt_ldob_indirect_disp;
    struct { /* e.g. ldob $optdisp[$index*S$scale], $dst */
      UINT f_optdisp;
      UINT f_scale;
      SI * i_index;
      SI * i_dst;
      unsigned char in_index;
      unsigned char out_dst;
    } fmt_ldob_index_disp;
    struct { /* e.g. ldob $optdisp($abase)[$index*S$scale], $dst */
      UINT f_optdisp;
      UINT f_scale;
      SI * i_abase;
      SI * i_index;
      SI * i_dst;
      unsigned char in_abase;
      unsigned char in_index;
      unsigned char out_dst;
    } fmt_ldob_indirect_index_disp;
    struct { /* e.g. ldos $offset, $dst */
      UINT f_offset;
      SI * i_dst;
      unsigned char out_dst;
    } fmt_ldos_offset;
    struct { /* e.g. ldos $offset($abase), $dst */
      UINT f_offset;
      SI * i_abase;
      SI * i_dst;
      unsigned char in_abase;
      unsigned char out_dst;
    } fmt_ldos_indirect_offset;
    struct { /* e.g. ldos ($abase), $dst */
      SI * i_abase;
      SI * i_dst;
      unsigned char in_abase;
      unsigned char out_dst;
    } fmt_ldos_indirect;
    struct { /* e.g. ldos ($abase)[$index*S$scale], $dst */
      UINT f_scale;
      SI * i_abase;
      SI * i_index;
      SI * i_dst;
      unsigned char in_abase;
      unsigned char in_index;
      unsigned char out_dst;
    } fmt_ldos_indirect_index;
    struct { /* e.g. ldos $optdisp, $dst */
      UINT f_optdisp;
      SI * i_dst;
      unsigned char out_dst;
    } fmt_ldos_disp;
    struct { /* e.g. ldos $optdisp($abase), $dst */
      UINT f_optdisp;
      SI * i_abase;
      SI * i_dst;
      unsigned char in_abase;
      unsigned char out_dst;
    } fmt_ldos_indirect_disp;
    struct { /* e.g. ldos $optdisp[$index*S$scale], $dst */
      UINT f_optdisp;
      UINT f_scale;
      SI * i_index;
      SI * i_dst;
      unsigned char in_index;
      unsigned char out_dst;
    } fmt_ldos_index_disp;
    struct { /* e.g. ldos $optdisp($abase)[$index*S$scale], $dst */
      UINT f_optdisp;
      UINT f_scale;
      SI * i_abase;
      SI * i_index;
      SI * i_dst;
      unsigned char in_abase;
      unsigned char in_index;
      unsigned char out_dst;
    } fmt_ldos_indirect_index_disp;
    struct { /* e.g. ldib $offset, $dst */
      UINT f_offset;
      SI * i_dst;
      unsigned char out_dst;
    } fmt_ldib_offset;
    struct { /* e.g. ldib $offset($abase), $dst */
      UINT f_offset;
      SI * i_abase;
      SI * i_dst;
      unsigned char in_abase;
      unsigned char out_dst;
    } fmt_ldib_indirect_offset;
    struct { /* e.g. ldib ($abase), $dst */
      SI * i_abase;
      SI * i_dst;
      unsigned char in_abase;
      unsigned char out_dst;
    } fmt_ldib_indirect;
    struct { /* e.g. ldib ($abase)[$index*S$scale], $dst */
      UINT f_scale;
      SI * i_abase;
      SI * i_index;
      SI * i_dst;
      unsigned char in_abase;
      unsigned char in_index;
      unsigned char out_dst;
    } fmt_ldib_indirect_index;
    struct { /* e.g. ldib $optdisp, $dst */
      UINT f_optdisp;
      SI * i_dst;
      unsigned char out_dst;
    } fmt_ldib_disp;
    struct { /* e.g. ldib $optdisp($abase), $dst */
      UINT f_optdisp;
      SI * i_abase;
      SI * i_dst;
      unsigned char in_abase;
      unsigned char out_dst;
    } fmt_ldib_indirect_disp;
    struct { /* e.g. ldib $optdisp[$index*S$scale], $dst */
      UINT f_optdisp;
      UINT f_scale;
      SI * i_index;
      SI * i_dst;
      unsigned char in_index;
      unsigned char out_dst;
    } fmt_ldib_index_disp;
    struct { /* e.g. ldib $optdisp($abase)[$index*S$scale], $dst */
      UINT f_optdisp;
      UINT f_scale;
      SI * i_abase;
      SI * i_index;
      SI * i_dst;
      unsigned char in_abase;
      unsigned char in_index;
      unsigned char out_dst;
    } fmt_ldib_indirect_index_disp;
    struct { /* e.g. ldis $offset, $dst */
      UINT f_offset;
      SI * i_dst;
      unsigned char out_dst;
    } fmt_ldis_offset;
    struct { /* e.g. ldis $offset($abase), $dst */
      UINT f_offset;
      SI * i_abase;
      SI * i_dst;
      unsigned char in_abase;
      unsigned char out_dst;
    } fmt_ldis_indirect_offset;
    struct { /* e.g. ldis ($abase), $dst */
      SI * i_abase;
      SI * i_dst;
      unsigned char in_abase;
      unsigned char out_dst;
    } fmt_ldis_indirect;
    struct { /* e.g. ldis ($abase)[$index*S$scale], $dst */
      UINT f_scale;
      SI * i_abase;
      SI * i_index;
      SI * i_dst;
      unsigned char in_abase;
      unsigned char in_index;
      unsigned char out_dst;
    } fmt_ldis_indirect_index;
    struct { /* e.g. ldis $optdisp, $dst */
      UINT f_optdisp;
      SI * i_dst;
      unsigned char out_dst;
    } fmt_ldis_disp;
    struct { /* e.g. ldis $optdisp($abase), $dst */
      UINT f_optdisp;
      SI * i_abase;
      SI * i_dst;
      unsigned char in_abase;
      unsigned char out_dst;
    } fmt_ldis_indirect_disp;
    struct { /* e.g. ldis $optdisp[$index*S$scale], $dst */
      UINT f_optdisp;
      UINT f_scale;
      SI * i_index;
      SI * i_dst;
      unsigned char in_index;
      unsigned char out_dst;
    } fmt_ldis_index_disp;
    struct { /* e.g. ldis $optdisp($abase)[$index*S$scale], $dst */
      UINT f_optdisp;
      UINT f_scale;
      SI * i_abase;
      SI * i_index;
      SI * i_dst;
      unsigned char in_abase;
      unsigned char in_index;
      unsigned char out_dst;
    } fmt_ldis_indirect_index_disp;
    struct { /* e.g. ldl $offset, $dst */
      UINT f_srcdst;
      UINT f_offset;
      SI * i_dst;
      unsigned char out_dst;
      unsigned char out_h_gr_add__VM_index_of_dst_1;
    } fmt_ldl_offset;
    struct { /* e.g. ldl $offset($abase), $dst */
      UINT f_srcdst;
      UINT f_offset;
      SI * i_abase;
      SI * i_dst;
      unsigned char in_abase;
      unsigned char out_dst;
      unsigned char out_h_gr_add__VM_index_of_dst_1;
    } fmt_ldl_indirect_offset;
    struct { /* e.g. ldl ($abase), $dst */
      UINT f_srcdst;
      SI * i_abase;
      SI * i_dst;
      unsigned char in_abase;
      unsigned char out_dst;
      unsigned char out_h_gr_add__VM_index_of_dst_1;
    } fmt_ldl_indirect;
    struct { /* e.g. ldl ($abase)[$index*S$scale], $dst */
      UINT f_srcdst;
      UINT f_scale;
      SI * i_abase;
      SI * i_index;
      SI * i_dst;
      unsigned char in_abase;
      unsigned char in_index;
      unsigned char out_dst;
      unsigned char out_h_gr_add__VM_index_of_dst_1;
    } fmt_ldl_indirect_index;
    struct { /* e.g. ldl $optdisp, $dst */
      UINT f_srcdst;
      UINT f_optdisp;
      SI * i_dst;
      unsigned char out_dst;
      unsigned char out_h_gr_add__VM_index_of_dst_1;
    } fmt_ldl_disp;
    struct { /* e.g. ldl $optdisp($abase), $dst */
      UINT f_srcdst;
      UINT f_optdisp;
      SI * i_abase;
      SI * i_dst;
      unsigned char in_abase;
      unsigned char out_dst;
      unsigned char out_h_gr_add__VM_index_of_dst_1;
    } fmt_ldl_indirect_disp;
    struct { /* e.g. ldl $optdisp[$index*S$scale], $dst */
      UINT f_srcdst;
      UINT f_optdisp;
      UINT f_scale;
      SI * i_index;
      SI * i_dst;
      unsigned char in_index;
      unsigned char out_dst;
      unsigned char out_h_gr_add__VM_index_of_dst_1;
    } fmt_ldl_index_disp;
    struct { /* e.g. ldl $optdisp($abase)[$index*S$scale], $dst */
      UINT f_srcdst;
      UINT f_optdisp;
      UINT f_scale;
      SI * i_abase;
      SI * i_index;
      SI * i_dst;
      unsigned char in_abase;
      unsigned char in_index;
      unsigned char out_dst;
      unsigned char out_h_gr_add__VM_index_of_dst_1;
    } fmt_ldl_indirect_index_disp;
    struct { /* e.g. ldt $offset, $dst */
      UINT f_srcdst;
      UINT f_offset;
      SI * i_dst;
      unsigned char out_dst;
      unsigned char out_h_gr_add__VM_index_of_dst_1;
      unsigned char out_h_gr_add__VM_index_of_dst_2;
    } fmt_ldt_offset;
    struct { /* e.g. ldt $offset($abase), $dst */
      UINT f_srcdst;
      UINT f_offset;
      SI * i_abase;
      SI * i_dst;
      unsigned char in_abase;
      unsigned char out_dst;
      unsigned char out_h_gr_add__VM_index_of_dst_1;
      unsigned char out_h_gr_add__VM_index_of_dst_2;
    } fmt_ldt_indirect_offset;
    struct { /* e.g. ldt ($abase), $dst */
      UINT f_srcdst;
      SI * i_abase;
      SI * i_dst;
      unsigned char in_abase;
      unsigned char out_dst;
      unsigned char out_h_gr_add__VM_index_of_dst_1;
      unsigned char out_h_gr_add__VM_index_of_dst_2;
    } fmt_ldt_indirect;
    struct { /* e.g. ldt ($abase)[$index*S$scale], $dst */
      UINT f_srcdst;
      UINT f_scale;
      SI * i_abase;
      SI * i_index;
      SI * i_dst;
      unsigned char in_abase;
      unsigned char in_index;
      unsigned char out_dst;
      unsigned char out_h_gr_add__VM_index_of_dst_1;
      unsigned char out_h_gr_add__VM_index_of_dst_2;
    } fmt_ldt_indirect_index;
    struct { /* e.g. ldt $optdisp, $dst */
      UINT f_srcdst;
      UINT f_optdisp;
      SI * i_dst;
      unsigned char out_dst;
      unsigned char out_h_gr_add__VM_index_of_dst_1;
      unsigned char out_h_gr_add__VM_index_of_dst_2;
    } fmt_ldt_disp;
    struct { /* e.g. ldt $optdisp($abase), $dst */
      UINT f_srcdst;
      UINT f_optdisp;
      SI * i_abase;
      SI * i_dst;
      unsigned char in_abase;
      unsigned char out_dst;
      unsigned char out_h_gr_add__VM_index_of_dst_1;
      unsigned char out_h_gr_add__VM_index_of_dst_2;
    } fmt_ldt_indirect_disp;
    struct { /* e.g. ldt $optdisp[$index*S$scale], $dst */
      UINT f_srcdst;
      UINT f_optdisp;
      UINT f_scale;
      SI * i_index;
      SI * i_dst;
      unsigned char in_index;
      unsigned char out_dst;
      unsigned char out_h_gr_add__VM_index_of_dst_1;
      unsigned char out_h_gr_add__VM_index_of_dst_2;
    } fmt_ldt_index_disp;
    struct { /* e.g. ldt $optdisp($abase)[$index*S$scale], $dst */
      UINT f_srcdst;
      UINT f_optdisp;
      UINT f_scale;
      SI * i_abase;
      SI * i_index;
      SI * i_dst;
      unsigned char in_abase;
      unsigned char in_index;
      unsigned char out_dst;
      unsigned char out_h_gr_add__VM_index_of_dst_1;
      unsigned char out_h_gr_add__VM_index_of_dst_2;
    } fmt_ldt_indirect_index_disp;
    struct { /* e.g. ldq $offset, $dst */
      UINT f_srcdst;
      UINT f_offset;
      SI * i_dst;
      unsigned char out_dst;
      unsigned char out_h_gr_add__VM_index_of_dst_1;
      unsigned char out_h_gr_add__VM_index_of_dst_2;
      unsigned char out_h_gr_add__VM_index_of_dst_3;
    } fmt_ldq_offset;
    struct { /* e.g. ldq $offset($abase), $dst */
      UINT f_srcdst;
      UINT f_offset;
      SI * i_abase;
      SI * i_dst;
      unsigned char in_abase;
      unsigned char out_dst;
      unsigned char out_h_gr_add__VM_index_of_dst_1;
      unsigned char out_h_gr_add__VM_index_of_dst_2;
      unsigned char out_h_gr_add__VM_index_of_dst_3;
    } fmt_ldq_indirect_offset;
    struct { /* e.g. ldq ($abase), $dst */
      UINT f_srcdst;
      SI * i_abase;
      SI * i_dst;
      unsigned char in_abase;
      unsigned char out_dst;
      unsigned char out_h_gr_add__VM_index_of_dst_1;
      unsigned char out_h_gr_add__VM_index_of_dst_2;
      unsigned char out_h_gr_add__VM_index_of_dst_3;
    } fmt_ldq_indirect;
    struct { /* e.g. ldq ($abase)[$index*S$scale], $dst */
      UINT f_srcdst;
      UINT f_scale;
      SI * i_abase;
      SI * i_index;
      SI * i_dst;
      unsigned char in_abase;
      unsigned char in_index;
      unsigned char out_dst;
      unsigned char out_h_gr_add__VM_index_of_dst_1;
      unsigned char out_h_gr_add__VM_index_of_dst_2;
      unsigned char out_h_gr_add__VM_index_of_dst_3;
    } fmt_ldq_indirect_index;
    struct { /* e.g. ldq $optdisp, $dst */
      UINT f_srcdst;
      UINT f_optdisp;
      SI * i_dst;
      unsigned char out_dst;
      unsigned char out_h_gr_add__VM_index_of_dst_1;
      unsigned char out_h_gr_add__VM_index_of_dst_2;
      unsigned char out_h_gr_add__VM_index_of_dst_3;
    } fmt_ldq_disp;
    struct { /* e.g. ldq $optdisp($abase), $dst */
      UINT f_srcdst;
      UINT f_optdisp;
      SI * i_abase;
      SI * i_dst;
      unsigned char in_abase;
      unsigned char out_dst;
      unsigned char out_h_gr_add__VM_index_of_dst_1;
      unsigned char out_h_gr_add__VM_index_of_dst_2;
      unsigned char out_h_gr_add__VM_index_of_dst_3;
    } fmt_ldq_indirect_disp;
    struct { /* e.g. ldq $optdisp[$index*S$scale], $dst */
      UINT f_srcdst;
      UINT f_optdisp;
      UINT f_scale;
      SI * i_index;
      SI * i_dst;
      unsigned char in_index;
      unsigned char out_dst;
      unsigned char out_h_gr_add__VM_index_of_dst_1;
      unsigned char out_h_gr_add__VM_index_of_dst_2;
      unsigned char out_h_gr_add__VM_index_of_dst_3;
    } fmt_ldq_index_disp;
    struct { /* e.g. ldq $optdisp($abase)[$index*S$scale], $dst */
      UINT f_srcdst;
      UINT f_optdisp;
      UINT f_scale;
      SI * i_abase;
      SI * i_index;
      SI * i_dst;
      unsigned char in_abase;
      unsigned char in_index;
      unsigned char out_dst;
      unsigned char out_h_gr_add__VM_index_of_dst_1;
      unsigned char out_h_gr_add__VM_index_of_dst_2;
      unsigned char out_h_gr_add__VM_index_of_dst_3;
    } fmt_ldq_indirect_index_disp;
    struct { /* e.g. st $st_src, $offset */
      UINT f_offset;
      SI * i_st_src;
      unsigned char in_st_src;
    } fmt_st_offset;
    struct { /* e.g. st $st_src, $offset($abase) */
      UINT f_offset;
      SI * i_abase;
      SI * i_st_src;
      unsigned char in_abase;
      unsigned char in_st_src;
    } fmt_st_indirect_offset;
    struct { /* e.g. st $st_src, ($abase) */
      SI * i_abase;
      SI * i_st_src;
      unsigned char in_abase;
      unsigned char in_st_src;
    } fmt_st_indirect;
    struct { /* e.g. st $st_src, ($abase)[$index*S$scale] */
      UINT f_scale;
      SI * i_abase;
      SI * i_index;
      SI * i_st_src;
      unsigned char in_abase;
      unsigned char in_index;
      unsigned char in_st_src;
    } fmt_st_indirect_index;
    struct { /* e.g. st $st_src, $optdisp */
      UINT f_optdisp;
      SI * i_st_src;
      unsigned char in_st_src;
    } fmt_st_disp;
    struct { /* e.g. st $st_src, $optdisp($abase) */
      UINT f_optdisp;
      SI * i_abase;
      SI * i_st_src;
      unsigned char in_abase;
      unsigned char in_st_src;
    } fmt_st_indirect_disp;
    struct { /* e.g. st $st_src, $optdisp[$index*S$scale */
      UINT f_optdisp;
      UINT f_scale;
      SI * i_index;
      SI * i_st_src;
      unsigned char in_index;
      unsigned char in_st_src;
    } fmt_st_index_disp;
    struct { /* e.g. st $st_src, $optdisp($abase)[$index*S$scale] */
      UINT f_optdisp;
      UINT f_scale;
      SI * i_abase;
      SI * i_index;
      SI * i_st_src;
      unsigned char in_abase;
      unsigned char in_index;
      unsigned char in_st_src;
    } fmt_st_indirect_index_disp;
    struct { /* e.g. stob $st_src, $offset */
      UINT f_offset;
      SI * i_st_src;
      unsigned char in_st_src;
    } fmt_stob_offset;
    struct { /* e.g. stob $st_src, $offset($abase) */
      UINT f_offset;
      SI * i_abase;
      SI * i_st_src;
      unsigned char in_abase;
      unsigned char in_st_src;
    } fmt_stob_indirect_offset;
    struct { /* e.g. stob $st_src, ($abase) */
      SI * i_abase;
      SI * i_st_src;
      unsigned char in_abase;
      unsigned char in_st_src;
    } fmt_stob_indirect;
    struct { /* e.g. stob $st_src, ($abase)[$index*S$scale] */
      UINT f_scale;
      SI * i_abase;
      SI * i_index;
      SI * i_st_src;
      unsigned char in_abase;
      unsigned char in_index;
      unsigned char in_st_src;
    } fmt_stob_indirect_index;
    struct { /* e.g. stob $st_src, $optdisp */
      UINT f_optdisp;
      SI * i_st_src;
      unsigned char in_st_src;
    } fmt_stob_disp;
    struct { /* e.g. stob $st_src, $optdisp($abase) */
      UINT f_optdisp;
      SI * i_abase;
      SI * i_st_src;
      unsigned char in_abase;
      unsigned char in_st_src;
    } fmt_stob_indirect_disp;
    struct { /* e.g. stob $st_src, $optdisp[$index*S$scale */
      UINT f_optdisp;
      UINT f_scale;
      SI * i_index;
      SI * i_st_src;
      unsigned char in_index;
      unsigned char in_st_src;
    } fmt_stob_index_disp;
    struct { /* e.g. stob $st_src, $optdisp($abase)[$index*S$scale] */
      UINT f_optdisp;
      UINT f_scale;
      SI * i_abase;
      SI * i_index;
      SI * i_st_src;
      unsigned char in_abase;
      unsigned char in_index;
      unsigned char in_st_src;
    } fmt_stob_indirect_index_disp;
    struct { /* e.g. stos $st_src, $offset */
      UINT f_offset;
      SI * i_st_src;
      unsigned char in_st_src;
    } fmt_stos_offset;
    struct { /* e.g. stos $st_src, $offset($abase) */
      UINT f_offset;
      SI * i_abase;
      SI * i_st_src;
      unsigned char in_abase;
      unsigned char in_st_src;
    } fmt_stos_indirect_offset;
    struct { /* e.g. stos $st_src, ($abase) */
      SI * i_abase;
      SI * i_st_src;
      unsigned char in_abase;
      unsigned char in_st_src;
    } fmt_stos_indirect;
    struct { /* e.g. stos $st_src, ($abase)[$index*S$scale] */
      UINT f_scale;
      SI * i_abase;
      SI * i_index;
      SI * i_st_src;
      unsigned char in_abase;
      unsigned char in_index;
      unsigned char in_st_src;
    } fmt_stos_indirect_index;
    struct { /* e.g. stos $st_src, $optdisp */
      UINT f_optdisp;
      SI * i_st_src;
      unsigned char in_st_src;
    } fmt_stos_disp;
    struct { /* e.g. stos $st_src, $optdisp($abase) */
      UINT f_optdisp;
      SI * i_abase;
      SI * i_st_src;
      unsigned char in_abase;
      unsigned char in_st_src;
    } fmt_stos_indirect_disp;
    struct { /* e.g. stos $st_src, $optdisp[$index*S$scale */
      UINT f_optdisp;
      UINT f_scale;
      SI * i_index;
      SI * i_st_src;
      unsigned char in_index;
      unsigned char in_st_src;
    } fmt_stos_index_disp;
    struct { /* e.g. stos $st_src, $optdisp($abase)[$index*S$scale] */
      UINT f_optdisp;
      UINT f_scale;
      SI * i_abase;
      SI * i_index;
      SI * i_st_src;
      unsigned char in_abase;
      unsigned char in_index;
      unsigned char in_st_src;
    } fmt_stos_indirect_index_disp;
    struct { /* e.g. stl $st_src, $offset */
      UINT f_srcdst;
      UINT f_offset;
      SI * i_st_src;
      unsigned char in_h_gr_add__VM_index_of_st_src_1;
      unsigned char in_st_src;
    } fmt_stl_offset;
    struct { /* e.g. stl $st_src, $offset($abase) */
      UINT f_srcdst;
      UINT f_offset;
      SI * i_abase;
      SI * i_st_src;
      unsigned char in_abase;
      unsigned char in_h_gr_add__VM_index_of_st_src_1;
      unsigned char in_st_src;
    } fmt_stl_indirect_offset;
    struct { /* e.g. stl $st_src, ($abase) */
      UINT f_srcdst;
      SI * i_abase;
      SI * i_st_src;
      unsigned char in_abase;
      unsigned char in_h_gr_add__VM_index_of_st_src_1;
      unsigned char in_st_src;
    } fmt_stl_indirect;
    struct { /* e.g. stl $st_src, ($abase)[$index*S$scale] */
      UINT f_srcdst;
      UINT f_scale;
      SI * i_abase;
      SI * i_index;
      SI * i_st_src;
      unsigned char in_abase;
      unsigned char in_h_gr_add__VM_index_of_st_src_1;
      unsigned char in_index;
      unsigned char in_st_src;
    } fmt_stl_indirect_index;
    struct { /* e.g. stl $st_src, $optdisp */
      UINT f_srcdst;
      UINT f_optdisp;
      SI * i_st_src;
      unsigned char in_h_gr_add__VM_index_of_st_src_1;
      unsigned char in_st_src;
    } fmt_stl_disp;
    struct { /* e.g. stl $st_src, $optdisp($abase) */
      UINT f_srcdst;
      UINT f_optdisp;
      SI * i_abase;
      SI * i_st_src;
      unsigned char in_abase;
      unsigned char in_h_gr_add__VM_index_of_st_src_1;
      unsigned char in_st_src;
    } fmt_stl_indirect_disp;
    struct { /* e.g. stl $st_src, $optdisp[$index*S$scale */
      UINT f_srcdst;
      UINT f_optdisp;
      UINT f_scale;
      SI * i_index;
      SI * i_st_src;
      unsigned char in_h_gr_add__VM_index_of_st_src_1;
      unsigned char in_index;
      unsigned char in_st_src;
    } fmt_stl_index_disp;
    struct { /* e.g. stl $st_src, $optdisp($abase)[$index*S$scale] */
      UINT f_srcdst;
      UINT f_optdisp;
      UINT f_scale;
      SI * i_abase;
      SI * i_index;
      SI * i_st_src;
      unsigned char in_abase;
      unsigned char in_h_gr_add__VM_index_of_st_src_1;
      unsigned char in_index;
      unsigned char in_st_src;
    } fmt_stl_indirect_index_disp;
    struct { /* e.g. stt $st_src, $offset */
      UINT f_srcdst;
      UINT f_offset;
      SI * i_st_src;
      unsigned char in_h_gr_add__VM_index_of_st_src_1;
      unsigned char in_h_gr_add__VM_index_of_st_src_2;
      unsigned char in_st_src;
    } fmt_stt_offset;
    struct { /* e.g. stt $st_src, $offset($abase) */
      UINT f_srcdst;
      UINT f_offset;
      SI * i_abase;
      SI * i_st_src;
      unsigned char in_abase;
      unsigned char in_h_gr_add__VM_index_of_st_src_1;
      unsigned char in_h_gr_add__VM_index_of_st_src_2;
      unsigned char in_st_src;
    } fmt_stt_indirect_offset;
    struct { /* e.g. stt $st_src, ($abase) */
      UINT f_srcdst;
      SI * i_abase;
      SI * i_st_src;
      unsigned char in_abase;
      unsigned char in_h_gr_add__VM_index_of_st_src_1;
      unsigned char in_h_gr_add__VM_index_of_st_src_2;
      unsigned char in_st_src;
    } fmt_stt_indirect;
    struct { /* e.g. stt $st_src, ($abase)[$index*S$scale] */
      UINT f_srcdst;
      UINT f_scale;
      SI * i_abase;
      SI * i_index;
      SI * i_st_src;
      unsigned char in_abase;
      unsigned char in_h_gr_add__VM_index_of_st_src_1;
      unsigned char in_h_gr_add__VM_index_of_st_src_2;
      unsigned char in_index;
      unsigned char in_st_src;
    } fmt_stt_indirect_index;
    struct { /* e.g. stt $st_src, $optdisp */
      UINT f_srcdst;
      UINT f_optdisp;
      SI * i_st_src;
      unsigned char in_h_gr_add__VM_index_of_st_src_1;
      unsigned char in_h_gr_add__VM_index_of_st_src_2;
      unsigned char in_st_src;
    } fmt_stt_disp;
    struct { /* e.g. stt $st_src, $optdisp($abase) */
      UINT f_srcdst;
      UINT f_optdisp;
      SI * i_abase;
      SI * i_st_src;
      unsigned char in_abase;
      unsigned char in_h_gr_add__VM_index_of_st_src_1;
      unsigned char in_h_gr_add__VM_index_of_st_src_2;
      unsigned char in_st_src;
    } fmt_stt_indirect_disp;
    struct { /* e.g. stt $st_src, $optdisp[$index*S$scale */
      UINT f_srcdst;
      UINT f_optdisp;
      UINT f_scale;
      SI * i_index;
      SI * i_st_src;
      unsigned char in_h_gr_add__VM_index_of_st_src_1;
      unsigned char in_h_gr_add__VM_index_of_st_src_2;
      unsigned char in_index;
      unsigned char in_st_src;
    } fmt_stt_index_disp;
    struct { /* e.g. stt $st_src, $optdisp($abase)[$index*S$scale] */
      UINT f_srcdst;
      UINT f_optdisp;
      UINT f_scale;
      SI * i_abase;
      SI * i_index;
      SI * i_st_src;
      unsigned char in_abase;
      unsigned char in_h_gr_add__VM_index_of_st_src_1;
      unsigned char in_h_gr_add__VM_index_of_st_src_2;
      unsigned char in_index;
      unsigned char in_st_src;
    } fmt_stt_indirect_index_disp;
    struct { /* e.g. stq $st_src, $offset */
      UINT f_srcdst;
      UINT f_offset;
      SI * i_st_src;
      unsigned char in_h_gr_add__VM_index_of_st_src_1;
      unsigned char in_h_gr_add__VM_index_of_st_src_2;
      unsigned char in_h_gr_add__VM_index_of_st_src_3;
      unsigned char in_st_src;
    } fmt_stq_offset;
    struct { /* e.g. stq $st_src, $offset($abase) */
      UINT f_srcdst;
      UINT f_offset;
      SI * i_abase;
      SI * i_st_src;
      unsigned char in_abase;
      unsigned char in_h_gr_add__VM_index_of_st_src_1;
      unsigned char in_h_gr_add__VM_index_of_st_src_2;
      unsigned char in_h_gr_add__VM_index_of_st_src_3;
      unsigned char in_st_src;
    } fmt_stq_indirect_offset;
    struct { /* e.g. stq $st_src, ($abase) */
      UINT f_srcdst;
      SI * i_abase;
      SI * i_st_src;
      unsigned char in_abase;
      unsigned char in_h_gr_add__VM_index_of_st_src_1;
      unsigned char in_h_gr_add__VM_index_of_st_src_2;
      unsigned char in_h_gr_add__VM_index_of_st_src_3;
      unsigned char in_st_src;
    } fmt_stq_indirect;
    struct { /* e.g. stq $st_src, ($abase)[$index*S$scale] */
      UINT f_srcdst;
      UINT f_scale;
      SI * i_abase;
      SI * i_index;
      SI * i_st_src;
      unsigned char in_abase;
      unsigned char in_h_gr_add__VM_index_of_st_src_1;
      unsigned char in_h_gr_add__VM_index_of_st_src_2;
      unsigned char in_h_gr_add__VM_index_of_st_src_3;
      unsigned char in_index;
      unsigned char in_st_src;
    } fmt_stq_indirect_index;
    struct { /* e.g. stq $st_src, $optdisp */
      UINT f_srcdst;
      UINT f_optdisp;
      SI * i_st_src;
      unsigned char in_h_gr_add__VM_index_of_st_src_1;
      unsigned char in_h_gr_add__VM_index_of_st_src_2;
      unsigned char in_h_gr_add__VM_index_of_st_src_3;
      unsigned char in_st_src;
    } fmt_stq_disp;
    struct { /* e.g. stq $st_src, $optdisp($abase) */
      UINT f_srcdst;
      UINT f_optdisp;
      SI * i_abase;
      SI * i_st_src;
      unsigned char in_abase;
      unsigned char in_h_gr_add__VM_index_of_st_src_1;
      unsigned char in_h_gr_add__VM_index_of_st_src_2;
      unsigned char in_h_gr_add__VM_index_of_st_src_3;
      unsigned char in_st_src;
    } fmt_stq_indirect_disp;
    struct { /* e.g. stq $st_src, $optdisp[$index*S$scale */
      UINT f_srcdst;
      UINT f_optdisp;
      UINT f_scale;
      SI * i_index;
      SI * i_st_src;
      unsigned char in_h_gr_add__VM_index_of_st_src_1;
      unsigned char in_h_gr_add__VM_index_of_st_src_2;
      unsigned char in_h_gr_add__VM_index_of_st_src_3;
      unsigned char in_index;
      unsigned char in_st_src;
    } fmt_stq_index_disp;
    struct { /* e.g. stq $st_src, $optdisp($abase)[$index*S$scale] */
      UINT f_srcdst;
      UINT f_optdisp;
      UINT f_scale;
      SI * i_abase;
      SI * i_index;
      SI * i_st_src;
      unsigned char in_abase;
      unsigned char in_h_gr_add__VM_index_of_st_src_1;
      unsigned char in_h_gr_add__VM_index_of_st_src_2;
      unsigned char in_h_gr_add__VM_index_of_st_src_3;
      unsigned char in_index;
      unsigned char in_st_src;
    } fmt_stq_indirect_index_disp;
    struct { /* e.g. cmpi $src1, $src2 */
      SI * i_src1;
      SI * i_src2;
      unsigned char in_src1;
      unsigned char in_src2;
    } fmt_cmpi;
    struct { /* e.g. cmpi $lit1, $src2 */
      UINT f_src1;
      SI * i_src2;
      unsigned char in_src2;
    } fmt_cmpi1;
    struct { /* e.g. cmpi $src1, $lit2 */
      UINT f_src2;
      SI * i_src1;
      unsigned char in_src1;
    } fmt_cmpi2;
    struct { /* e.g. cmpi $lit1, $lit2 */
      UINT f_src1;
      UINT f_src2;
    } fmt_cmpi3;
    struct { /* e.g. cmpo $src1, $src2 */
      SI * i_src1;
      SI * i_src2;
      unsigned char in_src1;
      unsigned char in_src2;
    } fmt_cmpo;
    struct { /* e.g. cmpo $lit1, $src2 */
      UINT f_src1;
      SI * i_src2;
      unsigned char in_src2;
    } fmt_cmpo1;
    struct { /* e.g. cmpo $src1, $lit2 */
      UINT f_src2;
      SI * i_src1;
      unsigned char in_src1;
    } fmt_cmpo2;
    struct { /* e.g. cmpo $lit1, $lit2 */
      UINT f_src1;
      UINT f_src2;
    } fmt_cmpo3;
    struct { /* e.g. testno $br_src1 */
      SI * i_br_src1;
      unsigned char out_br_src1;
    } fmt_testno_reg;
    struct { /* e.g. flushreg */
      int empty;
    } fmt_flushreg;
  /* cti insns, kept separately so addr_cache is in fixed place */
  struct {
    union {
    struct { /* e.g. cmpobe $br_src1, $br_src2, $br_disp */
      IADDR i_br_disp;
      SI * i_br_src1;
      SI * i_br_src2;
      unsigned char in_br_src1;
      unsigned char in_br_src2;
    } fmt_cmpobe_reg;
    struct { /* e.g. cmpobe $br_lit1, $br_src2, $br_disp */
      UINT f_br_src1;
      IADDR i_br_disp;
      SI * i_br_src2;
      unsigned char in_br_src2;
    } fmt_cmpobe_lit;
    struct { /* e.g. cmpobl $br_src1, $br_src2, $br_disp */
      IADDR i_br_disp;
      SI * i_br_src1;
      SI * i_br_src2;
      unsigned char in_br_src1;
      unsigned char in_br_src2;
    } fmt_cmpobl_reg;
    struct { /* e.g. cmpobl $br_lit1, $br_src2, $br_disp */
      UINT f_br_src1;
      IADDR i_br_disp;
      SI * i_br_src2;
      unsigned char in_br_src2;
    } fmt_cmpobl_lit;
    struct { /* e.g. bbc $br_src1, $br_src2, $br_disp */
      IADDR i_br_disp;
      SI * i_br_src1;
      SI * i_br_src2;
      unsigned char in_br_src1;
      unsigned char in_br_src2;
    } fmt_bbc_reg;
    struct { /* e.g. bbc $br_lit1, $br_src2, $br_disp */
      UINT f_br_src1;
      IADDR i_br_disp;
      SI * i_br_src2;
      unsigned char in_br_src2;
    } fmt_bbc_lit;
    struct { /* e.g. bno $ctrl_disp */
      IADDR i_ctrl_disp;
    } fmt_bno;
    struct { /* e.g. b $ctrl_disp */
      IADDR i_ctrl_disp;
    } fmt_b;
    struct { /* e.g. bx $offset($abase) */
      UINT f_offset;
      SI * i_abase;
      unsigned char in_abase;
    } fmt_bx_indirect_offset;
    struct { /* e.g. bx ($abase) */
      SI * i_abase;
      unsigned char in_abase;
    } fmt_bx_indirect;
    struct { /* e.g. bx ($abase)[$index*S$scale] */
      UINT f_scale;
      SI * i_abase;
      SI * i_index;
      unsigned char in_abase;
      unsigned char in_index;
    } fmt_bx_indirect_index;
    struct { /* e.g. bx $optdisp */
      UINT f_optdisp;
    } fmt_bx_disp;
    struct { /* e.g. bx $optdisp($abase) */
      UINT f_optdisp;
      SI * i_abase;
      unsigned char in_abase;
    } fmt_bx_indirect_disp;
    struct { /* e.g. callx $optdisp */
      UINT f_optdisp;
      unsigned char in_h_gr_0;
      unsigned char in_h_gr_1;
      unsigned char in_h_gr_10;
      unsigned char in_h_gr_11;
      unsigned char in_h_gr_12;
      unsigned char in_h_gr_13;
      unsigned char in_h_gr_14;
      unsigned char in_h_gr_15;
      unsigned char in_h_gr_2;
      unsigned char in_h_gr_3;
      unsigned char in_h_gr_31;
      unsigned char in_h_gr_4;
      unsigned char in_h_gr_5;
      unsigned char in_h_gr_6;
      unsigned char in_h_gr_7;
      unsigned char in_h_gr_8;
      unsigned char in_h_gr_9;
      unsigned char out_h_gr_0;
      unsigned char out_h_gr_1;
      unsigned char out_h_gr_10;
      unsigned char out_h_gr_11;
      unsigned char out_h_gr_12;
      unsigned char out_h_gr_13;
      unsigned char out_h_gr_14;
      unsigned char out_h_gr_15;
      unsigned char out_h_gr_2;
      unsigned char out_h_gr_3;
      unsigned char out_h_gr_31;
      unsigned char out_h_gr_4;
      unsigned char out_h_gr_5;
      unsigned char out_h_gr_6;
      unsigned char out_h_gr_7;
      unsigned char out_h_gr_8;
      unsigned char out_h_gr_9;
    } fmt_callx_disp;
    struct { /* e.g. callx ($abase) */
      SI * i_abase;
      unsigned char in_abase;
      unsigned char in_h_gr_0;
      unsigned char in_h_gr_1;
      unsigned char in_h_gr_10;
      unsigned char in_h_gr_11;
      unsigned char in_h_gr_12;
      unsigned char in_h_gr_13;
      unsigned char in_h_gr_14;
      unsigned char in_h_gr_15;
      unsigned char in_h_gr_2;
      unsigned char in_h_gr_3;
      unsigned char in_h_gr_31;
      unsigned char in_h_gr_4;
      unsigned char in_h_gr_5;
      unsigned char in_h_gr_6;
      unsigned char in_h_gr_7;
      unsigned char in_h_gr_8;
      unsigned char in_h_gr_9;
      unsigned char out_h_gr_0;
      unsigned char out_h_gr_1;
      unsigned char out_h_gr_10;
      unsigned char out_h_gr_11;
      unsigned char out_h_gr_12;
      unsigned char out_h_gr_13;
      unsigned char out_h_gr_14;
      unsigned char out_h_gr_15;
      unsigned char out_h_gr_2;
      unsigned char out_h_gr_3;
      unsigned char out_h_gr_31;
      unsigned char out_h_gr_4;
      unsigned char out_h_gr_5;
      unsigned char out_h_gr_6;
      unsigned char out_h_gr_7;
      unsigned char out_h_gr_8;
      unsigned char out_h_gr_9;
    } fmt_callx_indirect;
    struct { /* e.g. callx $offset($abase) */
      UINT f_offset;
      SI * i_abase;
      unsigned char in_abase;
      unsigned char in_h_gr_0;
      unsigned char in_h_gr_1;
      unsigned char in_h_gr_10;
      unsigned char in_h_gr_11;
      unsigned char in_h_gr_12;
      unsigned char in_h_gr_13;
      unsigned char in_h_gr_14;
      unsigned char in_h_gr_15;
      unsigned char in_h_gr_2;
      unsigned char in_h_gr_3;
      unsigned char in_h_gr_31;
      unsigned char in_h_gr_4;
      unsigned char in_h_gr_5;
      unsigned char in_h_gr_6;
      unsigned char in_h_gr_7;
      unsigned char in_h_gr_8;
      unsigned char in_h_gr_9;
      unsigned char out_h_gr_0;
      unsigned char out_h_gr_1;
      unsigned char out_h_gr_10;
      unsigned char out_h_gr_11;
      unsigned char out_h_gr_12;
      unsigned char out_h_gr_13;
      unsigned char out_h_gr_14;
      unsigned char out_h_gr_15;
      unsigned char out_h_gr_2;
      unsigned char out_h_gr_3;
      unsigned char out_h_gr_31;
      unsigned char out_h_gr_4;
      unsigned char out_h_gr_5;
      unsigned char out_h_gr_6;
      unsigned char out_h_gr_7;
      unsigned char out_h_gr_8;
      unsigned char out_h_gr_9;
    } fmt_callx_indirect_offset;
    struct { /* e.g. ret */
      int empty;
      unsigned char in_h_gr_0;
      unsigned char in_h_gr_2;
      unsigned char in_h_gr_31;
      unsigned char out_h_gr_0;
      unsigned char out_h_gr_1;
      unsigned char out_h_gr_10;
      unsigned char out_h_gr_11;
      unsigned char out_h_gr_12;
      unsigned char out_h_gr_13;
      unsigned char out_h_gr_14;
      unsigned char out_h_gr_15;
      unsigned char out_h_gr_2;
      unsigned char out_h_gr_3;
      unsigned char out_h_gr_31;
      unsigned char out_h_gr_4;
      unsigned char out_h_gr_5;
      unsigned char out_h_gr_6;
      unsigned char out_h_gr_7;
      unsigned char out_h_gr_8;
      unsigned char out_h_gr_9;
    } fmt_ret;
    struct { /* e.g. calls $src1 */
      SI * i_src1;
      unsigned char in_src1;
    } fmt_calls;
    struct { /* e.g. fmark */
      int empty;
    } fmt_fmark;
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

#define EXTRACT_IFMT_MULO_VARS \
  /* Instruction fields.  */ \
  UINT f_opcode; \
  UINT f_srcdst; \
  UINT f_src2; \
  UINT f_m3; \
  UINT f_m2; \
  UINT f_m1; \
  UINT f_opcode2; \
  UINT f_zero; \
  UINT f_src1; \
  unsigned int length;
#define EXTRACT_IFMT_MULO_CODE \
  length = 4; \
  f_opcode = EXTRACT_UINT (insn, 32, 0, 8); \
  f_srcdst = EXTRACT_UINT (insn, 32, 8, 5); \
  f_src2 = EXTRACT_UINT (insn, 32, 13, 5); \
  f_m3 = EXTRACT_UINT (insn, 32, 18, 1); \
  f_m2 = EXTRACT_UINT (insn, 32, 19, 1); \
  f_m1 = EXTRACT_UINT (insn, 32, 20, 1); \
  f_opcode2 = EXTRACT_UINT (insn, 32, 21, 4); \
  f_zero = EXTRACT_UINT (insn, 32, 25, 2); \
  f_src1 = EXTRACT_UINT (insn, 32, 27, 5); \

#define EXTRACT_IFMT_MULO1_VARS \
  /* Instruction fields.  */ \
  UINT f_opcode; \
  UINT f_srcdst; \
  UINT f_src2; \
  UINT f_m3; \
  UINT f_m2; \
  UINT f_m1; \
  UINT f_opcode2; \
  UINT f_zero; \
  UINT f_src1; \
  unsigned int length;
#define EXTRACT_IFMT_MULO1_CODE \
  length = 4; \
  f_opcode = EXTRACT_UINT (insn, 32, 0, 8); \
  f_srcdst = EXTRACT_UINT (insn, 32, 8, 5); \
  f_src2 = EXTRACT_UINT (insn, 32, 13, 5); \
  f_m3 = EXTRACT_UINT (insn, 32, 18, 1); \
  f_m2 = EXTRACT_UINT (insn, 32, 19, 1); \
  f_m1 = EXTRACT_UINT (insn, 32, 20, 1); \
  f_opcode2 = EXTRACT_UINT (insn, 32, 21, 4); \
  f_zero = EXTRACT_UINT (insn, 32, 25, 2); \
  f_src1 = EXTRACT_UINT (insn, 32, 27, 5); \

#define EXTRACT_IFMT_MULO2_VARS \
  /* Instruction fields.  */ \
  UINT f_opcode; \
  UINT f_srcdst; \
  UINT f_src2; \
  UINT f_m3; \
  UINT f_m2; \
  UINT f_m1; \
  UINT f_opcode2; \
  UINT f_zero; \
  UINT f_src1; \
  unsigned int length;
#define EXTRACT_IFMT_MULO2_CODE \
  length = 4; \
  f_opcode = EXTRACT_UINT (insn, 32, 0, 8); \
  f_srcdst = EXTRACT_UINT (insn, 32, 8, 5); \
  f_src2 = EXTRACT_UINT (insn, 32, 13, 5); \
  f_m3 = EXTRACT_UINT (insn, 32, 18, 1); \
  f_m2 = EXTRACT_UINT (insn, 32, 19, 1); \
  f_m1 = EXTRACT_UINT (insn, 32, 20, 1); \
  f_opcode2 = EXTRACT_UINT (insn, 32, 21, 4); \
  f_zero = EXTRACT_UINT (insn, 32, 25, 2); \
  f_src1 = EXTRACT_UINT (insn, 32, 27, 5); \

#define EXTRACT_IFMT_MULO3_VARS \
  /* Instruction fields.  */ \
  UINT f_opcode; \
  UINT f_srcdst; \
  UINT f_src2; \
  UINT f_m3; \
  UINT f_m2; \
  UINT f_m1; \
  UINT f_opcode2; \
  UINT f_zero; \
  UINT f_src1; \
  unsigned int length;
#define EXTRACT_IFMT_MULO3_CODE \
  length = 4; \
  f_opcode = EXTRACT_UINT (insn, 32, 0, 8); \
  f_srcdst = EXTRACT_UINT (insn, 32, 8, 5); \
  f_src2 = EXTRACT_UINT (insn, 32, 13, 5); \
  f_m3 = EXTRACT_UINT (insn, 32, 18, 1); \
  f_m2 = EXTRACT_UINT (insn, 32, 19, 1); \
  f_m1 = EXTRACT_UINT (insn, 32, 20, 1); \
  f_opcode2 = EXTRACT_UINT (insn, 32, 21, 4); \
  f_zero = EXTRACT_UINT (insn, 32, 25, 2); \
  f_src1 = EXTRACT_UINT (insn, 32, 27, 5); \

#define EXTRACT_IFMT_LDA_OFFSET_VARS \
  /* Instruction fields.  */ \
  UINT f_opcode; \
  UINT f_srcdst; \
  UINT f_abase; \
  UINT f_modea; \
  UINT f_zeroa; \
  UINT f_offset; \
  unsigned int length;
#define EXTRACT_IFMT_LDA_OFFSET_CODE \
  length = 4; \
  f_opcode = EXTRACT_UINT (insn, 32, 0, 8); \
  f_srcdst = EXTRACT_UINT (insn, 32, 8, 5); \
  f_abase = EXTRACT_UINT (insn, 32, 13, 5); \
  f_modea = EXTRACT_UINT (insn, 32, 18, 1); \
  f_zeroa = EXTRACT_UINT (insn, 32, 19, 1); \
  f_offset = EXTRACT_UINT (insn, 32, 20, 12); \

#define EXTRACT_IFMT_LDA_INDIRECT_VARS \
  /* Instruction fields.  */ \
  UINT f_opcode; \
  UINT f_srcdst; \
  UINT f_abase; \
  UINT f_modeb; \
  UINT f_scale; \
  UINT f_zerob; \
  UINT f_index; \
  unsigned int length;
#define EXTRACT_IFMT_LDA_INDIRECT_CODE \
  length = 4; \
  f_opcode = EXTRACT_UINT (insn, 32, 0, 8); \
  f_srcdst = EXTRACT_UINT (insn, 32, 8, 5); \
  f_abase = EXTRACT_UINT (insn, 32, 13, 5); \
  f_modeb = EXTRACT_UINT (insn, 32, 18, 4); \
  f_scale = EXTRACT_UINT (insn, 32, 22, 3); \
  f_zerob = EXTRACT_UINT (insn, 32, 25, 2); \
  f_index = EXTRACT_UINT (insn, 32, 27, 5); \

#define EXTRACT_IFMT_LDA_DISP_VARS \
  /* Instruction fields.  */ \
  UINT f_opcode; \
  UINT f_optdisp; \
  UINT f_srcdst; \
  UINT f_abase; \
  UINT f_modeb; \
  UINT f_scale; \
  UINT f_zerob; \
  UINT f_index; \
  /* Contents of trailing part of insn.  */ \
  UINT word_1; \
  unsigned int length;
#define EXTRACT_IFMT_LDA_DISP_CODE \
  length = 8; \
  word_1 = GETIMEMUSI (current_cpu, pc + 4); \
  f_opcode = EXTRACT_UINT (insn, 32, 0, 8); \
  f_optdisp = (0|(EXTRACT_UINT (word_1, 32, 0, 32) << 0)); \
  f_srcdst = EXTRACT_UINT (insn, 32, 8, 5); \
  f_abase = EXTRACT_UINT (insn, 32, 13, 5); \
  f_modeb = EXTRACT_UINT (insn, 32, 18, 4); \
  f_scale = EXTRACT_UINT (insn, 32, 22, 3); \
  f_zerob = EXTRACT_UINT (insn, 32, 25, 2); \
  f_index = EXTRACT_UINT (insn, 32, 27, 5); \

#define EXTRACT_IFMT_ST_OFFSET_VARS \
  /* Instruction fields.  */ \
  UINT f_opcode; \
  UINT f_srcdst; \
  UINT f_abase; \
  UINT f_modea; \
  UINT f_zeroa; \
  UINT f_offset; \
  unsigned int length;
#define EXTRACT_IFMT_ST_OFFSET_CODE \
  length = 4; \
  f_opcode = EXTRACT_UINT (insn, 32, 0, 8); \
  f_srcdst = EXTRACT_UINT (insn, 32, 8, 5); \
  f_abase = EXTRACT_UINT (insn, 32, 13, 5); \
  f_modea = EXTRACT_UINT (insn, 32, 18, 1); \
  f_zeroa = EXTRACT_UINT (insn, 32, 19, 1); \
  f_offset = EXTRACT_UINT (insn, 32, 20, 12); \

#define EXTRACT_IFMT_ST_INDIRECT_VARS \
  /* Instruction fields.  */ \
  UINT f_opcode; \
  UINT f_srcdst; \
  UINT f_abase; \
  UINT f_modeb; \
  UINT f_scale; \
  UINT f_zerob; \
  UINT f_index; \
  unsigned int length;
#define EXTRACT_IFMT_ST_INDIRECT_CODE \
  length = 4; \
  f_opcode = EXTRACT_UINT (insn, 32, 0, 8); \
  f_srcdst = EXTRACT_UINT (insn, 32, 8, 5); \
  f_abase = EXTRACT_UINT (insn, 32, 13, 5); \
  f_modeb = EXTRACT_UINT (insn, 32, 18, 4); \
  f_scale = EXTRACT_UINT (insn, 32, 22, 3); \
  f_zerob = EXTRACT_UINT (insn, 32, 25, 2); \
  f_index = EXTRACT_UINT (insn, 32, 27, 5); \

#define EXTRACT_IFMT_ST_DISP_VARS \
  /* Instruction fields.  */ \
  UINT f_opcode; \
  UINT f_optdisp; \
  UINT f_srcdst; \
  UINT f_abase; \
  UINT f_modeb; \
  UINT f_scale; \
  UINT f_zerob; \
  UINT f_index; \
  /* Contents of trailing part of insn.  */ \
  UINT word_1; \
  unsigned int length;
#define EXTRACT_IFMT_ST_DISP_CODE \
  length = 8; \
  word_1 = GETIMEMUSI (current_cpu, pc + 4); \
  f_opcode = EXTRACT_UINT (insn, 32, 0, 8); \
  f_optdisp = (0|(EXTRACT_UINT (word_1, 32, 0, 32) << 0)); \
  f_srcdst = EXTRACT_UINT (insn, 32, 8, 5); \
  f_abase = EXTRACT_UINT (insn, 32, 13, 5); \
  f_modeb = EXTRACT_UINT (insn, 32, 18, 4); \
  f_scale = EXTRACT_UINT (insn, 32, 22, 3); \
  f_zerob = EXTRACT_UINT (insn, 32, 25, 2); \
  f_index = EXTRACT_UINT (insn, 32, 27, 5); \

#define EXTRACT_IFMT_CMPOBE_REG_VARS \
  /* Instruction fields.  */ \
  UINT f_opcode; \
  UINT f_br_src1; \
  UINT f_br_src2; \
  UINT f_br_m1; \
  SI f_br_disp; \
  UINT f_br_zero; \
  unsigned int length;
#define EXTRACT_IFMT_CMPOBE_REG_CODE \
  length = 4; \
  f_opcode = EXTRACT_UINT (insn, 32, 0, 8); \
  f_br_src1 = EXTRACT_UINT (insn, 32, 8, 5); \
  f_br_src2 = EXTRACT_UINT (insn, 32, 13, 5); \
  f_br_m1 = EXTRACT_UINT (insn, 32, 18, 1); \
  f_br_disp = ((((EXTRACT_INT (insn, 32, 19, 11)) << (2))) + (pc)); \
  f_br_zero = EXTRACT_UINT (insn, 32, 30, 2); \

#define EXTRACT_IFMT_CMPOBE_LIT_VARS \
  /* Instruction fields.  */ \
  UINT f_opcode; \
  UINT f_br_src1; \
  UINT f_br_src2; \
  UINT f_br_m1; \
  SI f_br_disp; \
  UINT f_br_zero; \
  unsigned int length;
#define EXTRACT_IFMT_CMPOBE_LIT_CODE \
  length = 4; \
  f_opcode = EXTRACT_UINT (insn, 32, 0, 8); \
  f_br_src1 = EXTRACT_UINT (insn, 32, 8, 5); \
  f_br_src2 = EXTRACT_UINT (insn, 32, 13, 5); \
  f_br_m1 = EXTRACT_UINT (insn, 32, 18, 1); \
  f_br_disp = ((((EXTRACT_INT (insn, 32, 19, 11)) << (2))) + (pc)); \
  f_br_zero = EXTRACT_UINT (insn, 32, 30, 2); \

#define EXTRACT_IFMT_BNO_VARS \
  /* Instruction fields.  */ \
  UINT f_opcode; \
  SI f_ctrl_disp; \
  UINT f_ctrl_zero; \
  unsigned int length;
#define EXTRACT_IFMT_BNO_CODE \
  length = 4; \
  f_opcode = EXTRACT_UINT (insn, 32, 0, 8); \
  f_ctrl_disp = ((((EXTRACT_INT (insn, 32, 8, 22)) << (2))) + (pc)); \
  f_ctrl_zero = EXTRACT_UINT (insn, 32, 30, 2); \

/* Collection of various things for the trace handler to use.  */

typedef struct trace_record {
  IADDR pc;
  /* FIXME:wip */
} TRACE_RECORD;

#endif /* CPU_I960BASE_H */
