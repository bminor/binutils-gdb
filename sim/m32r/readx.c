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
    && case_read_READ_FMT_5_ADDV,
    && case_read_READ_FMT_6_ADDV3,
    && case_read_READ_FMT_7_ADDX,
    && case_read_READ_FMT_8_BC8,
    && case_read_READ_FMT_9_BC24,
    && case_read_READ_FMT_10_BEQ,
    && case_read_READ_FMT_11_BEQZ,
    && case_read_READ_FMT_11_BEQZ,
    && case_read_READ_FMT_11_BEQZ,
    && case_read_READ_FMT_11_BEQZ,
    && case_read_READ_FMT_11_BEQZ,
    && case_read_READ_FMT_11_BEQZ,
    && case_read_READ_FMT_12_BL8,
    && case_read_READ_FMT_13_BL24,
    && case_read_READ_FMT_14_BCL8,
    && case_read_READ_FMT_15_BCL24,
    && case_read_READ_FMT_8_BC8,
    && case_read_READ_FMT_9_BC24,
    && case_read_READ_FMT_10_BEQ,
    && case_read_READ_FMT_16_BRA8,
    && case_read_READ_FMT_17_BRA24,
    && case_read_READ_FMT_14_BCL8,
    && case_read_READ_FMT_15_BCL24,
    && case_read_READ_FMT_18_CMP,
    && case_read_READ_FMT_19_CMPI,
    && case_read_READ_FMT_18_CMP,
    && case_read_READ_FMT_20_CMPUI,
    && case_read_READ_FMT_18_CMP,
    && case_read_READ_FMT_21_CMPZ,
    && case_read_READ_FMT_22_DIV,
    && case_read_READ_FMT_22_DIV,
    && case_read_READ_FMT_22_DIV,
    && case_read_READ_FMT_22_DIV,
    && case_read_READ_FMT_22_DIV,
    && case_read_READ_FMT_23_JC,
    && case_read_READ_FMT_23_JC,
    && case_read_READ_FMT_24_JL,
    && case_read_READ_FMT_25_JMP,
    && case_read_READ_FMT_26_LD,
    && case_read_READ_FMT_27_LD_D,
    && case_read_READ_FMT_28_LDB,
    && case_read_READ_FMT_29_LDB_D,
    && case_read_READ_FMT_30_LDH,
    && case_read_READ_FMT_31_LDH_D,
    && case_read_READ_FMT_28_LDB,
    && case_read_READ_FMT_29_LDB_D,
    && case_read_READ_FMT_30_LDH,
    && case_read_READ_FMT_31_LDH_D,
    && case_read_READ_FMT_32_LD_PLUS,
    && case_read_READ_FMT_33_LD24,
    && case_read_READ_FMT_34_LDI8,
    && case_read_READ_FMT_35_LDI16,
    && case_read_READ_FMT_36_LOCK,
    && case_read_READ_FMT_37_MACHI_A,
    && case_read_READ_FMT_37_MACHI_A,
    && case_read_READ_FMT_0_ADD,
    && case_read_READ_FMT_38_MULHI_A,
    && case_read_READ_FMT_38_MULHI_A,
    && case_read_READ_FMT_39_MV,
    && case_read_READ_FMT_40_MVFACHI_A,
    && case_read_READ_FMT_40_MVFACHI_A,
    && case_read_READ_FMT_40_MVFACHI_A,
    && case_read_READ_FMT_41_MVFC,
    && case_read_READ_FMT_42_MVTACHI_A,
    && case_read_READ_FMT_42_MVTACHI_A,
    && case_read_READ_FMT_43_MVTC,
    && case_read_READ_FMT_39_MV,
    && case_read_READ_FMT_44_NOP,
    && case_read_READ_FMT_39_MV,
    && case_read_READ_FMT_45_RAC_D,
    && case_read_READ_FMT_46_RAC_DS,
    && case_read_READ_FMT_47_RAC_DSI,
    && case_read_READ_FMT_45_RAC_D,
    && case_read_READ_FMT_46_RAC_DS,
    && case_read_READ_FMT_47_RAC_DSI,
    && case_read_READ_FMT_48_RTE,
    && case_read_READ_FMT_49_SETH,
    && case_read_READ_FMT_0_ADD,
    && case_read_READ_FMT_50_SLL3,
    && case_read_READ_FMT_51_SLLI,
    && case_read_READ_FMT_0_ADD,
    && case_read_READ_FMT_50_SLL3,
    && case_read_READ_FMT_51_SLLI,
    && case_read_READ_FMT_0_ADD,
    && case_read_READ_FMT_50_SLL3,
    && case_read_READ_FMT_51_SLLI,
    && case_read_READ_FMT_52_ST,
    && case_read_READ_FMT_53_ST_D,
    && case_read_READ_FMT_54_STB,
    && case_read_READ_FMT_55_STB_D,
    && case_read_READ_FMT_56_STH,
    && case_read_READ_FMT_57_STH_D,
    && case_read_READ_FMT_58_ST_PLUS,
    && case_read_READ_FMT_58_ST_PLUS,
    && case_read_READ_FMT_0_ADD,
    && case_read_READ_FMT_5_ADDV,
    && case_read_READ_FMT_7_ADDX,
    && case_read_READ_FMT_59_TRAP,
    && case_read_READ_FMT_60_UNLOCK,
    && case_read_READ_FMT_61_SATB,
    && case_read_READ_FMT_61_SATB,
    && case_read_READ_FMT_62_SAT,
    && case_read_READ_FMT_21_CMPZ,
    && case_read_READ_FMT_63_SADD,
    && case_read_READ_FMT_64_MACWU1,
    && case_read_READ_FMT_65_MSBLO,
    && case_read_READ_FMT_66_MULWU1,
    && case_read_READ_FMT_64_MACWU1,
    && case_read_READ_FMT_67_SC,
    && case_read_READ_FMT_67_SC,
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

    CASE (read, READ_FMT_5_ADDV) : /* e.g. addv $dr,$sr */
    {
#define OPRND(f) par_exec->operands.fmt_5_addv.f
  EXTRACT_FMT_5_ADDV_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_5_ADDV_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (dr) = CPU (h_gr[f_r1]);
      OPRND (sr) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_6_ADDV3) : /* e.g. addv3 $dr,$sr,#$simm16 */
    {
#define OPRND(f) par_exec->operands.fmt_6_addv3.f
  EXTRACT_FMT_6_ADDV3_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_6_ADDV3_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (simm16) = f_simm16;
      OPRND (sr) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_7_ADDX) : /* e.g. addx $dr,$sr */
    {
#define OPRND(f) par_exec->operands.fmt_7_addx.f
  EXTRACT_FMT_7_ADDX_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_7_ADDX_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (condbit) = CPU (h_cond);
      OPRND (dr) = CPU (h_gr[f_r1]);
      OPRND (sr) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_8_BC8) : /* e.g. bc $disp8 */
    {
#define OPRND(f) par_exec->operands.fmt_8_bc8.f
  EXTRACT_FMT_8_BC8_VARS /* f-op1 f-r1 f-disp8 */
  EXTRACT_FMT_8_BC8_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (condbit) = CPU (h_cond);
      RECORD_IADDR (OPRND (disp8), (pc & -4L) + f_disp8);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_9_BC24) : /* e.g. bc $disp24 */
    {
#define OPRND(f) par_exec->operands.fmt_9_bc24.f
  EXTRACT_FMT_9_BC24_VARS /* f-op1 f-r1 f-disp24 */
  EXTRACT_FMT_9_BC24_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (condbit) = CPU (h_cond);
      OPRND (disp24) = pc + f_disp24;
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_10_BEQ) : /* e.g. beq $src1,$src2,$disp16 */
    {
#define OPRND(f) par_exec->operands.fmt_10_beq.f
  EXTRACT_FMT_10_BEQ_VARS /* f-op1 f-r1 f-op2 f-r2 f-disp16 */
  EXTRACT_FMT_10_BEQ_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (disp16) = pc + f_disp16;
      OPRND (src1) = CPU (h_gr[f_r1]);
      OPRND (src2) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_11_BEQZ) : /* e.g. beqz $src2,$disp16 */
    {
#define OPRND(f) par_exec->operands.fmt_11_beqz.f
  EXTRACT_FMT_11_BEQZ_VARS /* f-op1 f-r1 f-op2 f-r2 f-disp16 */
  EXTRACT_FMT_11_BEQZ_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (disp16) = pc + f_disp16;
      OPRND (src2) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_12_BL8) : /* e.g. bl $disp8 */
    {
#define OPRND(f) par_exec->operands.fmt_12_bl8.f
  EXTRACT_FMT_12_BL8_VARS /* f-op1 f-r1 f-disp8 */
  EXTRACT_FMT_12_BL8_CODE

      /* Fetch the input operands for the semantic handler.  */
      RECORD_IADDR (OPRND (disp8), (pc & -4L) + f_disp8);
      OPRND (pc) = CPU (h_pc);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_13_BL24) : /* e.g. bl $disp24 */
    {
#define OPRND(f) par_exec->operands.fmt_13_bl24.f
  EXTRACT_FMT_13_BL24_VARS /* f-op1 f-r1 f-disp24 */
  EXTRACT_FMT_13_BL24_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (disp24) = pc + f_disp24;
      OPRND (pc) = CPU (h_pc);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_14_BCL8) : /* e.g. bcl $disp8 */
    {
#define OPRND(f) par_exec->operands.fmt_14_bcl8.f
  EXTRACT_FMT_14_BCL8_VARS /* f-op1 f-r1 f-disp8 */
  EXTRACT_FMT_14_BCL8_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (condbit) = CPU (h_cond);
      RECORD_IADDR (OPRND (disp8), (pc & -4L) + f_disp8);
      OPRND (pc) = CPU (h_pc);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_15_BCL24) : /* e.g. bcl $disp24 */
    {
#define OPRND(f) par_exec->operands.fmt_15_bcl24.f
  EXTRACT_FMT_15_BCL24_VARS /* f-op1 f-r1 f-disp24 */
  EXTRACT_FMT_15_BCL24_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (condbit) = CPU (h_cond);
      OPRND (disp24) = pc + f_disp24;
      OPRND (pc) = CPU (h_pc);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_16_BRA8) : /* e.g. bra $disp8 */
    {
#define OPRND(f) par_exec->operands.fmt_16_bra8.f
  EXTRACT_FMT_16_BRA8_VARS /* f-op1 f-r1 f-disp8 */
  EXTRACT_FMT_16_BRA8_CODE

      /* Fetch the input operands for the semantic handler.  */
      RECORD_IADDR (OPRND (disp8), (pc & -4L) + f_disp8);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_17_BRA24) : /* e.g. bra $disp24 */
    {
#define OPRND(f) par_exec->operands.fmt_17_bra24.f
  EXTRACT_FMT_17_BRA24_VARS /* f-op1 f-r1 f-disp24 */
  EXTRACT_FMT_17_BRA24_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (disp24) = pc + f_disp24;
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_18_CMP) : /* e.g. cmp $src1,$src2 */
    {
#define OPRND(f) par_exec->operands.fmt_18_cmp.f
  EXTRACT_FMT_18_CMP_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_18_CMP_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (src1) = CPU (h_gr[f_r1]);
      OPRND (src2) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_19_CMPI) : /* e.g. cmpi $src2,#$simm16 */
    {
#define OPRND(f) par_exec->operands.fmt_19_cmpi.f
  EXTRACT_FMT_19_CMPI_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_19_CMPI_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (simm16) = f_simm16;
      OPRND (src2) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_20_CMPUI) : /* e.g. cmpui $src2,#$uimm16 */
    {
#define OPRND(f) par_exec->operands.fmt_20_cmpui.f
  EXTRACT_FMT_20_CMPUI_VARS /* f-op1 f-r1 f-op2 f-r2 f-uimm16 */
  EXTRACT_FMT_20_CMPUI_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (src2) = CPU (h_gr[f_r2]);
      OPRND (uimm16) = f_uimm16;
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_21_CMPZ) : /* e.g. cmpz $src2 */
    {
#define OPRND(f) par_exec->operands.fmt_21_cmpz.f
  EXTRACT_FMT_21_CMPZ_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_21_CMPZ_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (src2) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_22_DIV) : /* e.g. div $dr,$sr */
    {
#define OPRND(f) par_exec->operands.fmt_22_div.f
  EXTRACT_FMT_22_DIV_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_22_DIV_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (dr) = CPU (h_gr[f_r1]);
      OPRND (sr) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_23_JC) : /* e.g. jc $sr */
    {
#define OPRND(f) par_exec->operands.fmt_23_jc.f
  EXTRACT_FMT_23_JC_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_23_JC_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (condbit) = CPU (h_cond);
      OPRND (sr) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_24_JL) : /* e.g. jl $sr */
    {
#define OPRND(f) par_exec->operands.fmt_24_jl.f
  EXTRACT_FMT_24_JL_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_24_JL_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (pc) = CPU (h_pc);
      OPRND (sr) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_25_JMP) : /* e.g. jmp $sr */
    {
#define OPRND(f) par_exec->operands.fmt_25_jmp.f
  EXTRACT_FMT_25_JMP_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_25_JMP_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (sr) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_26_LD) : /* e.g. ld $dr,@$sr */
    {
#define OPRND(f) par_exec->operands.fmt_26_ld.f
  EXTRACT_FMT_26_LD_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_26_LD_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (h_memory_sr) = GETMEMSI (current_cpu, CPU (h_gr[f_r2]));
      OPRND (sr) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_27_LD_D) : /* e.g. ld $dr,@($slo16,$sr) */
    {
#define OPRND(f) par_exec->operands.fmt_27_ld_d.f
  EXTRACT_FMT_27_LD_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_27_LD_D_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (h_memory_add_WI_sr_slo16) = GETMEMSI (current_cpu, ADDSI (CPU (h_gr[f_r2]), f_simm16));
      OPRND (slo16) = f_simm16;
      OPRND (sr) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_28_LDB) : /* e.g. ldb $dr,@$sr */
    {
#define OPRND(f) par_exec->operands.fmt_28_ldb.f
  EXTRACT_FMT_28_LDB_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_28_LDB_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (h_memory_sr) = GETMEMQI (current_cpu, CPU (h_gr[f_r2]));
      OPRND (sr) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_29_LDB_D) : /* e.g. ldb $dr,@($slo16,$sr) */
    {
#define OPRND(f) par_exec->operands.fmt_29_ldb_d.f
  EXTRACT_FMT_29_LDB_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_29_LDB_D_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (h_memory_add_WI_sr_slo16) = GETMEMQI (current_cpu, ADDSI (CPU (h_gr[f_r2]), f_simm16));
      OPRND (slo16) = f_simm16;
      OPRND (sr) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_30_LDH) : /* e.g. ldh $dr,@$sr */
    {
#define OPRND(f) par_exec->operands.fmt_30_ldh.f
  EXTRACT_FMT_30_LDH_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_30_LDH_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (h_memory_sr) = GETMEMHI (current_cpu, CPU (h_gr[f_r2]));
      OPRND (sr) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_31_LDH_D) : /* e.g. ldh $dr,@($slo16,$sr) */
    {
#define OPRND(f) par_exec->operands.fmt_31_ldh_d.f
  EXTRACT_FMT_31_LDH_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_31_LDH_D_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (h_memory_add_WI_sr_slo16) = GETMEMHI (current_cpu, ADDSI (CPU (h_gr[f_r2]), f_simm16));
      OPRND (slo16) = f_simm16;
      OPRND (sr) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_32_LD_PLUS) : /* e.g. ld $dr,@$sr+ */
    {
#define OPRND(f) par_exec->operands.fmt_32_ld_plus.f
  EXTRACT_FMT_32_LD_PLUS_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_32_LD_PLUS_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (h_memory_sr) = GETMEMSI (current_cpu, CPU (h_gr[f_r2]));
      OPRND (sr) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_33_LD24) : /* e.g. ld24 $dr,#$uimm24 */
    {
#define OPRND(f) par_exec->operands.fmt_33_ld24.f
  EXTRACT_FMT_33_LD24_VARS /* f-op1 f-r1 f-uimm24 */
  EXTRACT_FMT_33_LD24_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (uimm24) = f_uimm24;
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_34_LDI8) : /* e.g. ldi $dr,#$simm8 */
    {
#define OPRND(f) par_exec->operands.fmt_34_ldi8.f
  EXTRACT_FMT_34_LDI8_VARS /* f-op1 f-r1 f-simm8 */
  EXTRACT_FMT_34_LDI8_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (simm8) = f_simm8;
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_35_LDI16) : /* e.g. ldi $dr,$slo16 */
    {
#define OPRND(f) par_exec->operands.fmt_35_ldi16.f
  EXTRACT_FMT_35_LDI16_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_35_LDI16_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (slo16) = f_simm16;
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_36_LOCK) : /* e.g. lock $dr,@$sr */
    {
#define OPRND(f) par_exec->operands.fmt_36_lock.f
  EXTRACT_FMT_36_LOCK_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_36_LOCK_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (dr) = CPU (h_gr[f_r1]);
      OPRND (sr) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_37_MACHI_A) : /* e.g. machi $src1,$src2,$acc */
    {
#define OPRND(f) par_exec->operands.fmt_37_machi_a.f
  EXTRACT_FMT_37_MACHI_A_VARS /* f-op1 f-r1 f-acc f-op23 f-r2 */
  EXTRACT_FMT_37_MACHI_A_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (acc) = m32rx_h_accums_get (current_cpu, f_acc);
      OPRND (src1) = CPU (h_gr[f_r1]);
      OPRND (src2) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_38_MULHI_A) : /* e.g. mulhi $src1,$src2,$acc */
    {
#define OPRND(f) par_exec->operands.fmt_38_mulhi_a.f
  EXTRACT_FMT_38_MULHI_A_VARS /* f-op1 f-r1 f-acc f-op23 f-r2 */
  EXTRACT_FMT_38_MULHI_A_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (src1) = CPU (h_gr[f_r1]);
      OPRND (src2) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_39_MV) : /* e.g. mv $dr,$sr */
    {
#define OPRND(f) par_exec->operands.fmt_39_mv.f
  EXTRACT_FMT_39_MV_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_39_MV_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (sr) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_40_MVFACHI_A) : /* e.g. mvfachi $dr,$accs */
    {
#define OPRND(f) par_exec->operands.fmt_40_mvfachi_a.f
  EXTRACT_FMT_40_MVFACHI_A_VARS /* f-op1 f-r1 f-op2 f-accs f-op3 */
  EXTRACT_FMT_40_MVFACHI_A_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (accs) = m32rx_h_accums_get (current_cpu, f_accs);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_41_MVFC) : /* e.g. mvfc $dr,$scr */
    {
#define OPRND(f) par_exec->operands.fmt_41_mvfc.f
  EXTRACT_FMT_41_MVFC_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_41_MVFC_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (scr) = m32rx_h_cr_get (current_cpu, f_r2);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_42_MVTACHI_A) : /* e.g. mvtachi $src1,$accs */
    {
#define OPRND(f) par_exec->operands.fmt_42_mvtachi_a.f
  EXTRACT_FMT_42_MVTACHI_A_VARS /* f-op1 f-r1 f-op2 f-accs f-op3 */
  EXTRACT_FMT_42_MVTACHI_A_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (accs) = m32rx_h_accums_get (current_cpu, f_accs);
      OPRND (src1) = CPU (h_gr[f_r1]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_43_MVTC) : /* e.g. mvtc $sr,$dcr */
    {
#define OPRND(f) par_exec->operands.fmt_43_mvtc.f
  EXTRACT_FMT_43_MVTC_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_43_MVTC_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (sr) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_44_NOP) : /* e.g. nop */
    {
#define OPRND(f) par_exec->operands.fmt_44_nop.f
  EXTRACT_FMT_44_NOP_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_44_NOP_CODE

      /* Fetch the input operands for the semantic handler.  */
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_45_RAC_D) : /* e.g. rac $accd */
    {
#define OPRND(f) par_exec->operands.fmt_45_rac_d.f
  EXTRACT_FMT_45_RAC_D_VARS /* f-op1 f-accd f-bits67 f-op2 f-accs f-bit14 f-imm1 */
  EXTRACT_FMT_45_RAC_D_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (accum) = CPU (h_accum);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_46_RAC_DS) : /* e.g. rac $accd,$accs */
    {
#define OPRND(f) par_exec->operands.fmt_46_rac_ds.f
  EXTRACT_FMT_46_RAC_DS_VARS /* f-op1 f-accd f-bits67 f-op2 f-accs f-bit14 f-imm1 */
  EXTRACT_FMT_46_RAC_DS_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (accs) = m32rx_h_accums_get (current_cpu, f_accs);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_47_RAC_DSI) : /* e.g. rac $accd,$accs,#$imm1 */
    {
#define OPRND(f) par_exec->operands.fmt_47_rac_dsi.f
  EXTRACT_FMT_47_RAC_DSI_VARS /* f-op1 f-accd f-bits67 f-op2 f-accs f-bit14 f-imm1 */
  EXTRACT_FMT_47_RAC_DSI_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (accs) = m32rx_h_accums_get (current_cpu, f_accs);
      OPRND (imm1) = f_imm1;
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_48_RTE) : /* e.g. rte */
    {
#define OPRND(f) par_exec->operands.fmt_48_rte.f
  EXTRACT_FMT_48_RTE_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_48_RTE_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (h_bcond_0) = CPU (h_bcond);
      OPRND (h_bie_0) = CPU (h_bie);
      OPRND (h_bpc_0) = CPU (h_bpc);
      OPRND (h_bsm_0) = CPU (h_bsm);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_49_SETH) : /* e.g. seth $dr,#$hi16 */
    {
#define OPRND(f) par_exec->operands.fmt_49_seth.f
  EXTRACT_FMT_49_SETH_VARS /* f-op1 f-r1 f-op2 f-r2 f-hi16 */
  EXTRACT_FMT_49_SETH_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (hi16) = f_hi16;
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_50_SLL3) : /* e.g. sll3 $dr,$sr,#$simm16 */
    {
#define OPRND(f) par_exec->operands.fmt_50_sll3.f
  EXTRACT_FMT_50_SLL3_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_50_SLL3_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (simm16) = f_simm16;
      OPRND (sr) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_51_SLLI) : /* e.g. slli $dr,#$uimm5 */
    {
#define OPRND(f) par_exec->operands.fmt_51_slli.f
  EXTRACT_FMT_51_SLLI_VARS /* f-op1 f-r1 f-shift-op2 f-uimm5 */
  EXTRACT_FMT_51_SLLI_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (dr) = CPU (h_gr[f_r1]);
      OPRND (uimm5) = f_uimm5;
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_52_ST) : /* e.g. st $src1,@$src2 */
    {
#define OPRND(f) par_exec->operands.fmt_52_st.f
  EXTRACT_FMT_52_ST_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_52_ST_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (src1) = CPU (h_gr[f_r1]);
      OPRND (src2) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_53_ST_D) : /* e.g. st $src1,@($slo16,$src2) */
    {
#define OPRND(f) par_exec->operands.fmt_53_st_d.f
  EXTRACT_FMT_53_ST_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_53_ST_D_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (slo16) = f_simm16;
      OPRND (src1) = CPU (h_gr[f_r1]);
      OPRND (src2) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_54_STB) : /* e.g. stb $src1,@$src2 */
    {
#define OPRND(f) par_exec->operands.fmt_54_stb.f
  EXTRACT_FMT_54_STB_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_54_STB_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (src1) = CPU (h_gr[f_r1]);
      OPRND (src2) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_55_STB_D) : /* e.g. stb $src1,@($slo16,$src2) */
    {
#define OPRND(f) par_exec->operands.fmt_55_stb_d.f
  EXTRACT_FMT_55_STB_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_55_STB_D_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (slo16) = f_simm16;
      OPRND (src1) = CPU (h_gr[f_r1]);
      OPRND (src2) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_56_STH) : /* e.g. sth $src1,@$src2 */
    {
#define OPRND(f) par_exec->operands.fmt_56_sth.f
  EXTRACT_FMT_56_STH_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_56_STH_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (src1) = CPU (h_gr[f_r1]);
      OPRND (src2) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_57_STH_D) : /* e.g. sth $src1,@($slo16,$src2) */
    {
#define OPRND(f) par_exec->operands.fmt_57_sth_d.f
  EXTRACT_FMT_57_STH_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_57_STH_D_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (slo16) = f_simm16;
      OPRND (src1) = CPU (h_gr[f_r1]);
      OPRND (src2) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_58_ST_PLUS) : /* e.g. st $src1,@+$src2 */
    {
#define OPRND(f) par_exec->operands.fmt_58_st_plus.f
  EXTRACT_FMT_58_ST_PLUS_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_58_ST_PLUS_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (src1) = CPU (h_gr[f_r1]);
      OPRND (src2) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_59_TRAP) : /* e.g. trap #$uimm4 */
    {
#define OPRND(f) par_exec->operands.fmt_59_trap.f
  EXTRACT_FMT_59_TRAP_VARS /* f-op1 f-r1 f-op2 f-uimm4 */
  EXTRACT_FMT_59_TRAP_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (uimm4) = f_uimm4;
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_60_UNLOCK) : /* e.g. unlock $src1,@$src2 */
    {
#define OPRND(f) par_exec->operands.fmt_60_unlock.f
  EXTRACT_FMT_60_UNLOCK_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_60_UNLOCK_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (src1) = CPU (h_gr[f_r1]);
      OPRND (src2) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_61_SATB) : /* e.g. satb $dr,$sr */
    {
#define OPRND(f) par_exec->operands.fmt_61_satb.f
  EXTRACT_FMT_61_SATB_VARS /* f-op1 f-r1 f-op2 f-r2 f-uimm16 */
  EXTRACT_FMT_61_SATB_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (sr) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_62_SAT) : /* e.g. sat $dr,$sr */
    {
#define OPRND(f) par_exec->operands.fmt_62_sat.f
  EXTRACT_FMT_62_SAT_VARS /* f-op1 f-r1 f-op2 f-r2 f-uimm16 */
  EXTRACT_FMT_62_SAT_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (condbit) = CPU (h_cond);
      OPRND (sr) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_63_SADD) : /* e.g. sadd */
    {
#define OPRND(f) par_exec->operands.fmt_63_sadd.f
  EXTRACT_FMT_63_SADD_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_63_SADD_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (h_accums_0) = m32rx_h_accums_get (current_cpu, 0);
      OPRND (h_accums_1) = m32rx_h_accums_get (current_cpu, 1);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_64_MACWU1) : /* e.g. macwu1 $src1,$src2 */
    {
#define OPRND(f) par_exec->operands.fmt_64_macwu1.f
  EXTRACT_FMT_64_MACWU1_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_64_MACWU1_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (h_accums_1) = m32rx_h_accums_get (current_cpu, 1);
      OPRND (src1) = CPU (h_gr[f_r1]);
      OPRND (src2) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_65_MSBLO) : /* e.g. msblo $src1,$src2 */
    {
#define OPRND(f) par_exec->operands.fmt_65_msblo.f
  EXTRACT_FMT_65_MSBLO_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_65_MSBLO_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (accum) = CPU (h_accum);
      OPRND (src1) = CPU (h_gr[f_r1]);
      OPRND (src2) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_66_MULWU1) : /* e.g. mulwu1 $src1,$src2 */
    {
#define OPRND(f) par_exec->operands.fmt_66_mulwu1.f
  EXTRACT_FMT_66_MULWU1_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_66_MULWU1_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (src1) = CPU (h_gr[f_r1]);
      OPRND (src2) = CPU (h_gr[f_r2]);
#undef OPRND
    }
    BREAK (read);

    CASE (read, READ_FMT_67_SC) : /* e.g. sc */
    {
#define OPRND(f) par_exec->operands.fmt_67_sc.f
  EXTRACT_FMT_67_SC_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_67_SC_CODE

      /* Fetch the input operands for the semantic handler.  */
      OPRND (condbit) = CPU (h_cond);
#undef OPRND
    }
    BREAK (read);

    }
  ENDSWITCH (read) /* End of read switch.  */
}

#endif /* DEFINE_SWITCH */
