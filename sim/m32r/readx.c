/* Simulator instruction operand reader for m32r.

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

#ifdef DEFINE_LABELS
#undef DEFINE_LABELS

  /* The labels have the case they have because the enum of insn types
     is all uppercase and in the non-stdc case the fmt symbol is built
     into the enum name.

     The order here must match the order in m32rx_decode_vars in decode.c.  */

  static void *labels[] = {
    && case_read_READ_ILLEGAL,
    && case_read_READ_FMT_0_ADD,
    && case_read_READ_FMT_1_ADD3,
    && case_read_READ_FMT_0_ADD,
    && case_read_READ_FMT_2_AND3,
    && case_read_READ_FMT_0_ADD,
    && case_read_READ_FMT_3_OR3,
    && case_read_READ_FMT_0_ADD,
    && case_read_READ_FMT_2_AND3,
    && case_read_READ_FMT_4_ADDI,
    && case_read_READ_FMT_0_ADD,
    && case_read_READ_FMT_5_ADDV3,
    && case_read_READ_FMT_6_ADDX,
    && case_read_READ_FMT_7_BC8,
    && case_read_READ_FMT_8_BC24,
    && case_read_READ_FMT_9_BEQ,
    && case_read_READ_FMT_10_BEQZ,
    && case_read_READ_FMT_10_BEQZ,
    && case_read_READ_FMT_10_BEQZ,
    && case_read_READ_FMT_10_BEQZ,
    && case_read_READ_FMT_10_BEQZ,
    && case_read_READ_FMT_10_BEQZ,
    && case_read_READ_FMT_11_BL8,
    && case_read_READ_FMT_12_BL24,
    && case_read_READ_FMT_13_BCL8,
    && case_read_READ_FMT_14_BCL24,
    && case_read_READ_FMT_7_BC8,
    && case_read_READ_FMT_8_BC24,
    && case_read_READ_FMT_9_BEQ,
    && case_read_READ_FMT_15_BRA8,
    && case_read_READ_FMT_16_BRA24,
    && case_read_READ_FMT_13_BCL8,
    && case_read_READ_FMT_14_BCL24,
    && case_read_READ_FMT_17_CMP,
    && case_read_READ_FMT_18_CMPI,
    && case_read_READ_FMT_17_CMP,
    && case_read_READ_FMT_19_CMPUI,
    && case_read_READ_FMT_17_CMP,
    && case_read_READ_FMT_20_CMPZ,
    && case_read_READ_FMT_21_DIV,
    && case_read_READ_FMT_21_DIV,
    && case_read_READ_FMT_21_DIV,
    && case_read_READ_FMT_21_DIV,
    && case_read_READ_FMT_21_DIV,
    && case_read_READ_FMT_22_JC,
    && case_read_READ_FMT_22_JC,
    && case_read_READ_FMT_23_JL,
    && case_read_READ_FMT_24_JMP,
    && case_read_READ_FMT_25_LD,
    && case_read_READ_FMT_26_LD_D,
    && case_read_READ_FMT_27_LDB,
    && case_read_READ_FMT_28_LDB_D,
    && case_read_READ_FMT_29_LDH,
    && case_read_READ_FMT_30_LDH_D,
    && case_read_READ_FMT_27_LDB,
    && case_read_READ_FMT_28_LDB_D,
    && case_read_READ_FMT_29_LDH,
    && case_read_READ_FMT_30_LDH_D,
    && case_read_READ_FMT_25_LD,
    && case_read_READ_FMT_31_LD24,
    && case_read_READ_FMT_32_LDI8,
    && case_read_READ_FMT_33_LDI16,
    && case_read_READ_FMT_0_ADD,
    && case_read_READ_FMT_34_MACHI_A,
    && case_read_READ_FMT_34_MACHI_A,
    && case_read_READ_FMT_0_ADD,
    && case_read_READ_FMT_35_MULHI_A,
    && case_read_READ_FMT_35_MULHI_A,
    && case_read_READ_FMT_36_MV,
    && case_read_READ_FMT_37_MVFACHI_A,
    && case_read_READ_FMT_37_MVFACHI_A,
    && case_read_READ_FMT_37_MVFACHI_A,
    && case_read_READ_FMT_38_MVFC,
    && case_read_READ_FMT_39_MVTACHI_A,
    && case_read_READ_FMT_39_MVTACHI_A,
    && case_read_READ_FMT_40_MVTC,
    && case_read_READ_FMT_36_MV,
    && case_read_READ_FMT_41_NOP,
    && case_read_READ_FMT_36_MV,
    && case_read_READ_FMT_42_RAC_D,
    && case_read_READ_FMT_43_RAC_DS,
    && case_read_READ_FMT_44_RAC_DSI,
    && case_read_READ_FMT_42_RAC_D,
    && case_read_READ_FMT_43_RAC_DS,
    && case_read_READ_FMT_44_RAC_DSI,
    && case_read_READ_FMT_45_RTE,
    && case_read_READ_FMT_46_SETH,
    && case_read_READ_FMT_0_ADD,
    && case_read_READ_FMT_5_ADDV3,
    && case_read_READ_FMT_47_SLLI,
    && case_read_READ_FMT_0_ADD,
    && case_read_READ_FMT_5_ADDV3,
    && case_read_READ_FMT_47_SLLI,
    && case_read_READ_FMT_0_ADD,
    && case_read_READ_FMT_5_ADDV3,
    && case_read_READ_FMT_47_SLLI,
    && case_read_READ_FMT_17_CMP,
    && case_read_READ_FMT_48_ST_D,
    && case_read_READ_FMT_17_CMP,
    && case_read_READ_FMT_48_ST_D,
    && case_read_READ_FMT_17_CMP,
    && case_read_READ_FMT_48_ST_D,
    && case_read_READ_FMT_17_CMP,
    && case_read_READ_FMT_17_CMP,
    && case_read_READ_FMT_0_ADD,
    && case_read_READ_FMT_0_ADD,
    && case_read_READ_FMT_6_ADDX,
    && case_read_READ_FMT_49_TRAP,
    && case_read_READ_FMT_17_CMP,
    && case_read_READ_FMT_50_SATB,
    && case_read_READ_FMT_50_SATB,
    && case_read_READ_FMT_51_SAT,
    && case_read_READ_FMT_20_CMPZ,
    && case_read_READ_FMT_52_SADD,
    && case_read_READ_FMT_53_MACWU1,
    && case_read_READ_FMT_54_MSBLO,
    && case_read_READ_FMT_17_CMP,
    && case_read_READ_FMT_53_MACWU1,
    && case_read_READ_FMT_55_SC,
    && case_read_READ_FMT_55_SC,
    0
  };
  extern DECODE *m32rx_decode_vars[];
  int i;

  for (i = 0; m32rx_decode_vars[i] != 0; ++i)
    m32rx_decode_vars[i]->read = labels[i];

#endif /* DEFINE_LABELS */

#ifdef DEFINE_SWITCH
#undef DEFINE_SWITCH

{
  SWITCH (read, decode->read)
    {

    CASE (read, READ_ILLEGAL) :
    {
      sim_engine_illegal_insn (current_cpu, NULL_CIA /*FIXME*/);
    }
    BREAK (read);

    CASE (read, READ_FMT_0_ADD) : /* e.g. add $dr,$sr */
    {
#define OPRND(f) par_exec->operands.fmt_0_add.f
  EXTRACT_FMT_0_ADD_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_0_ADD_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (dr) = CPU (h_gr[f_r1]);
      OPRND (sr) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_1_ADD3) : /* e.g. add3 $dr,$sr,#$slo16 */
    {
#define OPRND(f) par_exec->operands.fmt_1_add3.f
  EXTRACT_FMT_1_ADD3_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_1_ADD3_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (slo16) = f_simm16;
      OPRND (sr) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_2_AND3) : /* e.g. and3 $dr,$sr,#$uimm16 */
    {
#define OPRND(f) par_exec->operands.fmt_2_and3.f
  EXTRACT_FMT_2_AND3_VARS /* f-op1 f-r1 f-op2 f-r2 f-uimm16 */
  EXTRACT_FMT_2_AND3_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (sr) = CPU (h_gr[f_r2]);
      OPRND (uimm16) = f_uimm16;
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_3_OR3) : /* e.g. or3 $dr,$sr,#$ulo16 */
    {
#define OPRND(f) par_exec->operands.fmt_3_or3.f
  EXTRACT_FMT_3_OR3_VARS /* f-op1 f-r1 f-op2 f-r2 f-uimm16 */
  EXTRACT_FMT_3_OR3_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (sr) = CPU (h_gr[f_r2]);
      OPRND (ulo16) = f_uimm16;
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_4_ADDI) : /* e.g. addi $dr,#$simm8 */
    {
#define OPRND(f) par_exec->operands.fmt_4_addi.f
  EXTRACT_FMT_4_ADDI_VARS /* f-op1 f-r1 f-simm8 */
  EXTRACT_FMT_4_ADDI_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (dr) = CPU (h_gr[f_r1]);
      OPRND (simm8) = f_simm8;
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_5_ADDV3) : /* e.g. addv3 $dr,$sr,#$simm16 */
    {
#define OPRND(f) par_exec->operands.fmt_5_addv3.f
  EXTRACT_FMT_5_ADDV3_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_5_ADDV3_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (simm16) = f_simm16;
      OPRND (sr) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_6_ADDX) : /* e.g. addx $dr,$sr */
    {
#define OPRND(f) par_exec->operands.fmt_6_addx.f
  EXTRACT_FMT_6_ADDX_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_6_ADDX_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (condbit) = CPU (h_cond);
      OPRND (dr) = CPU (h_gr[f_r1]);
      OPRND (sr) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_7_BC8) : /* e.g. bc $disp8 */
    {
#define OPRND(f) par_exec->operands.fmt_7_bc8.f
  EXTRACT_FMT_7_BC8_VARS /* f-op1 f-r1 f-disp8 */
  EXTRACT_FMT_7_BC8_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (condbit) = CPU (h_cond);
      OPRND (disp8) = f_disp8;
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_8_BC24) : /* e.g. bc $disp24 */
    {
#define OPRND(f) par_exec->operands.fmt_8_bc24.f
  EXTRACT_FMT_8_BC24_VARS /* f-op1 f-r1 f-disp24 */
  EXTRACT_FMT_8_BC24_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (condbit) = CPU (h_cond);
      OPRND (disp24) = f_disp24;
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_9_BEQ) : /* e.g. beq $src1,$src2,$disp16 */
    {
#define OPRND(f) par_exec->operands.fmt_9_beq.f
  EXTRACT_FMT_9_BEQ_VARS /* f-op1 f-r1 f-op2 f-r2 f-disp16 */
  EXTRACT_FMT_9_BEQ_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (disp16) = f_disp16;
      OPRND (src1) = CPU (h_gr[f_r1]);
      OPRND (src2) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_10_BEQZ) : /* e.g. beqz $src2,$disp16 */
    {
#define OPRND(f) par_exec->operands.fmt_10_beqz.f
  EXTRACT_FMT_10_BEQZ_VARS /* f-op1 f-r1 f-op2 f-r2 f-disp16 */
  EXTRACT_FMT_10_BEQZ_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (disp16) = f_disp16;
      OPRND (src2) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_11_BL8) : /* e.g. bl $disp8 */
    {
#define OPRND(f) par_exec->operands.fmt_11_bl8.f
  EXTRACT_FMT_11_BL8_VARS /* f-op1 f-r1 f-disp8 */
  EXTRACT_FMT_11_BL8_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (disp8) = f_disp8;
      OPRND (pc) = CPU (h_pc);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_12_BL24) : /* e.g. bl $disp24 */
    {
#define OPRND(f) par_exec->operands.fmt_12_bl24.f
  EXTRACT_FMT_12_BL24_VARS /* f-op1 f-r1 f-disp24 */
  EXTRACT_FMT_12_BL24_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (disp24) = f_disp24;
      OPRND (pc) = CPU (h_pc);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_13_BCL8) : /* e.g. bcl $disp8 */
    {
#define OPRND(f) par_exec->operands.fmt_13_bcl8.f
  EXTRACT_FMT_13_BCL8_VARS /* f-op1 f-r1 f-disp8 */
  EXTRACT_FMT_13_BCL8_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (condbit) = CPU (h_cond);
      OPRND (disp8) = f_disp8;
      OPRND (pc) = CPU (h_pc);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_14_BCL24) : /* e.g. bcl $disp24 */
    {
#define OPRND(f) par_exec->operands.fmt_14_bcl24.f
  EXTRACT_FMT_14_BCL24_VARS /* f-op1 f-r1 f-disp24 */
  EXTRACT_FMT_14_BCL24_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (condbit) = CPU (h_cond);
      OPRND (disp24) = f_disp24;
      OPRND (pc) = CPU (h_pc);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_15_BRA8) : /* e.g. bra $disp8 */
    {
#define OPRND(f) par_exec->operands.fmt_15_bra8.f
  EXTRACT_FMT_15_BRA8_VARS /* f-op1 f-r1 f-disp8 */
  EXTRACT_FMT_15_BRA8_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (disp8) = f_disp8;
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_16_BRA24) : /* e.g. bra $disp24 */
    {
#define OPRND(f) par_exec->operands.fmt_16_bra24.f
  EXTRACT_FMT_16_BRA24_VARS /* f-op1 f-r1 f-disp24 */
  EXTRACT_FMT_16_BRA24_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (disp24) = f_disp24;
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_17_CMP) : /* e.g. cmp $src1,$src2 */
    {
#define OPRND(f) par_exec->operands.fmt_17_cmp.f
  EXTRACT_FMT_17_CMP_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_17_CMP_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (src1) = CPU (h_gr[f_r1]);
      OPRND (src2) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_18_CMPI) : /* e.g. cmpi $src2,#$simm16 */
    {
#define OPRND(f) par_exec->operands.fmt_18_cmpi.f
  EXTRACT_FMT_18_CMPI_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_18_CMPI_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (simm16) = f_simm16;
      OPRND (src2) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_19_CMPUI) : /* e.g. cmpui $src2,#$uimm16 */
    {
#define OPRND(f) par_exec->operands.fmt_19_cmpui.f
  EXTRACT_FMT_19_CMPUI_VARS /* f-op1 f-r1 f-op2 f-r2 f-uimm16 */
  EXTRACT_FMT_19_CMPUI_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (src2) = CPU (h_gr[f_r2]);
      OPRND (uimm16) = f_uimm16;
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_20_CMPZ) : /* e.g. cmpz $src2 */
    {
#define OPRND(f) par_exec->operands.fmt_20_cmpz.f
  EXTRACT_FMT_20_CMPZ_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_20_CMPZ_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (src2) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_21_DIV) : /* e.g. div $dr,$sr */
    {
#define OPRND(f) par_exec->operands.fmt_21_div.f
  EXTRACT_FMT_21_DIV_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_21_DIV_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (dr) = CPU (h_gr[f_r1]);
      OPRND (sr) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_22_JC) : /* e.g. jc $sr */
    {
#define OPRND(f) par_exec->operands.fmt_22_jc.f
  EXTRACT_FMT_22_JC_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_22_JC_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (condbit) = CPU (h_cond);
      OPRND (sr) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_23_JL) : /* e.g. jl $sr */
    {
#define OPRND(f) par_exec->operands.fmt_23_jl.f
  EXTRACT_FMT_23_JL_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_23_JL_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (pc) = CPU (h_pc);
      OPRND (sr) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_24_JMP) : /* e.g. jmp $sr */
    {
#define OPRND(f) par_exec->operands.fmt_24_jmp.f
  EXTRACT_FMT_24_JMP_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_24_JMP_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (sr) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_25_LD) : /* e.g. ld $dr,@$sr */
    {
#define OPRND(f) par_exec->operands.fmt_25_ld.f
  EXTRACT_FMT_25_LD_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_25_LD_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (h_memory_sr) = GETMEMSI (current_cpu, CPU (h_gr[f_r2]));
      OPRND (sr) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_26_LD_D) : /* e.g. ld $dr,@($slo16,$sr) */
    {
#define OPRND(f) par_exec->operands.fmt_26_ld_d.f
  EXTRACT_FMT_26_LD_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_26_LD_D_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (h_memory_add_WI_sr_slo16) = GETMEMSI (current_cpu, ADDSI (CPU (h_gr[f_r2]), f_simm16));
      OPRND (slo16) = f_simm16;
      OPRND (sr) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_27_LDB) : /* e.g. ldb $dr,@$sr */
    {
#define OPRND(f) par_exec->operands.fmt_27_ldb.f
  EXTRACT_FMT_27_LDB_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_27_LDB_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (h_memory_sr) = GETMEMQI (current_cpu, CPU (h_gr[f_r2]));
      OPRND (sr) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_28_LDB_D) : /* e.g. ldb $dr,@($slo16,$sr) */
    {
#define OPRND(f) par_exec->operands.fmt_28_ldb_d.f
  EXTRACT_FMT_28_LDB_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_28_LDB_D_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (h_memory_add_WI_sr_slo16) = GETMEMQI (current_cpu, ADDSI (CPU (h_gr[f_r2]), f_simm16));
      OPRND (slo16) = f_simm16;
      OPRND (sr) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_29_LDH) : /* e.g. ldh $dr,@$sr */
    {
#define OPRND(f) par_exec->operands.fmt_29_ldh.f
  EXTRACT_FMT_29_LDH_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_29_LDH_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (h_memory_sr) = GETMEMHI (current_cpu, CPU (h_gr[f_r2]));
      OPRND (sr) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_30_LDH_D) : /* e.g. ldh $dr,@($slo16,$sr) */
    {
#define OPRND(f) par_exec->operands.fmt_30_ldh_d.f
  EXTRACT_FMT_30_LDH_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_30_LDH_D_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (h_memory_add_WI_sr_slo16) = GETMEMHI (current_cpu, ADDSI (CPU (h_gr[f_r2]), f_simm16));
      OPRND (slo16) = f_simm16;
      OPRND (sr) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_31_LD24) : /* e.g. ld24 $dr,#$uimm24 */
    {
#define OPRND(f) par_exec->operands.fmt_31_ld24.f
  EXTRACT_FMT_31_LD24_VARS /* f-op1 f-r1 f-uimm24 */
  EXTRACT_FMT_31_LD24_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (uimm24) = f_uimm24;
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_32_LDI8) : /* e.g. ldi $dr,#$simm8 */
    {
#define OPRND(f) par_exec->operands.fmt_32_ldi8.f
  EXTRACT_FMT_32_LDI8_VARS /* f-op1 f-r1 f-simm8 */
  EXTRACT_FMT_32_LDI8_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (simm8) = f_simm8;
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_33_LDI16) : /* e.g. ldi $dr,$slo16 */
    {
#define OPRND(f) par_exec->operands.fmt_33_ldi16.f
  EXTRACT_FMT_33_LDI16_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_33_LDI16_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (slo16) = f_simm16;
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_34_MACHI_A) : /* e.g. machi $src1,$src2,$acc */
    {
#define OPRND(f) par_exec->operands.fmt_34_machi_a.f
  EXTRACT_FMT_34_MACHI_A_VARS /* f-op1 f-r1 f-acc f-op23 f-r2 */
  EXTRACT_FMT_34_MACHI_A_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (acc) = m32rx_h_accums_get (current_cpu, f_acc);
      OPRND (src1) = CPU (h_gr[f_r1]);
      OPRND (src2) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_35_MULHI_A) : /* e.g. mulhi $src1,$src2,$acc */
    {
#define OPRND(f) par_exec->operands.fmt_35_mulhi_a.f
  EXTRACT_FMT_35_MULHI_A_VARS /* f-op1 f-r1 f-acc f-op23 f-r2 */
  EXTRACT_FMT_35_MULHI_A_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (src1) = CPU (h_gr[f_r1]);
      OPRND (src2) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_36_MV) : /* e.g. mv $dr,$sr */
    {
#define OPRND(f) par_exec->operands.fmt_36_mv.f
  EXTRACT_FMT_36_MV_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_36_MV_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (sr) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_37_MVFACHI_A) : /* e.g. mvfachi $dr,$accs */
    {
#define OPRND(f) par_exec->operands.fmt_37_mvfachi_a.f
  EXTRACT_FMT_37_MVFACHI_A_VARS /* f-op1 f-r1 f-op2 f-accs f-op3 */
  EXTRACT_FMT_37_MVFACHI_A_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (accs) = m32rx_h_accums_get (current_cpu, f_accs);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_38_MVFC) : /* e.g. mvfc $dr,$scr */
    {
#define OPRND(f) par_exec->operands.fmt_38_mvfc.f
  EXTRACT_FMT_38_MVFC_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_38_MVFC_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (scr) = m32rx_h_cr_get (current_cpu, f_r2);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_39_MVTACHI_A) : /* e.g. mvtachi $src1,$accs */
    {
#define OPRND(f) par_exec->operands.fmt_39_mvtachi_a.f
  EXTRACT_FMT_39_MVTACHI_A_VARS /* f-op1 f-r1 f-op2 f-accs f-op3 */
  EXTRACT_FMT_39_MVTACHI_A_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (accs) = m32rx_h_accums_get (current_cpu, f_accs);
      OPRND (src1) = CPU (h_gr[f_r1]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_40_MVTC) : /* e.g. mvtc $sr,$dcr */
    {
#define OPRND(f) par_exec->operands.fmt_40_mvtc.f
  EXTRACT_FMT_40_MVTC_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_40_MVTC_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (sr) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_41_NOP) : /* e.g. nop */
    {
#define OPRND(f) par_exec->operands.fmt_41_nop.f
  EXTRACT_FMT_41_NOP_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_41_NOP_CODE

      /* Fetch the input operands for the semantic handler.  */
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_42_RAC_D) : /* e.g. rac $accd */
    {
#define OPRND(f) par_exec->operands.fmt_42_rac_d.f
  EXTRACT_FMT_42_RAC_D_VARS /* f-op1 f-accd f-bits67 f-op2 f-accs f-bit14 f-imm1 */
  EXTRACT_FMT_42_RAC_D_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (accum) = CPU (h_accum);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_43_RAC_DS) : /* e.g. rac $accd,$accs */
    {
#define OPRND(f) par_exec->operands.fmt_43_rac_ds.f
  EXTRACT_FMT_43_RAC_DS_VARS /* f-op1 f-accd f-bits67 f-op2 f-accs f-bit14 f-imm1 */
  EXTRACT_FMT_43_RAC_DS_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (accs) = m32rx_h_accums_get (current_cpu, f_accs);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_44_RAC_DSI) : /* e.g. rac $accd,$accs,#$imm1 */
    {
#define OPRND(f) par_exec->operands.fmt_44_rac_dsi.f
  EXTRACT_FMT_44_RAC_DSI_VARS /* f-op1 f-accd f-bits67 f-op2 f-accs f-bit14 f-imm1 */
  EXTRACT_FMT_44_RAC_DSI_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (accs) = m32rx_h_accums_get (current_cpu, f_accs);
      OPRND (imm1) = f_imm1;
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_45_RTE) : /* e.g. rte */
    {
#define OPRND(f) par_exec->operands.fmt_45_rte.f
  EXTRACT_FMT_45_RTE_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_45_RTE_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (h_bcond_0) = CPU (h_bcond);
      OPRND (h_bie_0) = CPU (h_bie);
      OPRND (h_bpc_0) = CPU (h_bpc);
      OPRND (h_bsm_0) = CPU (h_bsm);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_46_SETH) : /* e.g. seth $dr,#$hi16 */
    {
#define OPRND(f) par_exec->operands.fmt_46_seth.f
  EXTRACT_FMT_46_SETH_VARS /* f-op1 f-r1 f-op2 f-r2 f-hi16 */
  EXTRACT_FMT_46_SETH_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (hi16) = f_hi16;
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_47_SLLI) : /* e.g. slli $dr,#$uimm5 */
    {
#define OPRND(f) par_exec->operands.fmt_47_slli.f
  EXTRACT_FMT_47_SLLI_VARS /* f-op1 f-r1 f-shift-op2 f-uimm5 */
  EXTRACT_FMT_47_SLLI_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (dr) = CPU (h_gr[f_r1]);
      OPRND (uimm5) = f_uimm5;
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_48_ST_D) : /* e.g. st $src1,@($slo16,$src2) */
    {
#define OPRND(f) par_exec->operands.fmt_48_st_d.f
  EXTRACT_FMT_48_ST_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_48_ST_D_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (slo16) = f_simm16;
      OPRND (src1) = CPU (h_gr[f_r1]);
      OPRND (src2) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_49_TRAP) : /* e.g. trap #$uimm4 */
    {
#define OPRND(f) par_exec->operands.fmt_49_trap.f
  EXTRACT_FMT_49_TRAP_VARS /* f-op1 f-r1 f-op2 f-uimm4 */
  EXTRACT_FMT_49_TRAP_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (uimm4) = f_uimm4;
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_50_SATB) : /* e.g. satb $dr,$src2 */
    {
#define OPRND(f) par_exec->operands.fmt_50_satb.f
  EXTRACT_FMT_50_SATB_VARS /* f-op1 f-r1 f-op2 f-r2 f-uimm16 */
  EXTRACT_FMT_50_SATB_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (src2) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_51_SAT) : /* e.g. sat $dr,$src2 */
    {
#define OPRND(f) par_exec->operands.fmt_51_sat.f
  EXTRACT_FMT_51_SAT_VARS /* f-op1 f-r1 f-op2 f-r2 f-uimm16 */
  EXTRACT_FMT_51_SAT_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (condbit) = CPU (h_cond);
      OPRND (src2) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_52_SADD) : /* e.g. sadd */
    {
#define OPRND(f) par_exec->operands.fmt_52_sadd.f
  EXTRACT_FMT_52_SADD_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_52_SADD_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (h_accums_0) = m32rx_h_accums_get (current_cpu, 0);
      OPRND (h_accums_1) = m32rx_h_accums_get (current_cpu, 1);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_53_MACWU1) : /* e.g. macwu1 $src1,$src2 */
    {
#define OPRND(f) par_exec->operands.fmt_53_macwu1.f
  EXTRACT_FMT_53_MACWU1_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_53_MACWU1_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (h_accums_1) = m32rx_h_accums_get (current_cpu, 1);
      OPRND (src1) = CPU (h_gr[f_r1]);
      OPRND (src2) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_54_MSBLO) : /* e.g. msblo $src1,$src2 */
    {
#define OPRND(f) par_exec->operands.fmt_54_msblo.f
  EXTRACT_FMT_54_MSBLO_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_54_MSBLO_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (accum) = CPU (h_accum);
      OPRND (src1) = CPU (h_gr[f_r1]);
      OPRND (src2) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_55_SC) : /* e.g. sc */
    {
#define OPRND(f) par_exec->operands.fmt_55_sc.f
  EXTRACT_FMT_55_SC_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_55_SC_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (condbit) = CPU (h_cond);
#undef OPRND
    }
    BREAK (read);

    }
  ENDSWITCH (read) /* End of read switch.  */
}

#endif /* DEFINE_SWITCH */
