/* Simulator instruction semantics for arc600f.

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

#ifdef DEFINE_LABELS

  /* The labels have the case they have because the enum of insn types
     is all uppercase and in the non-stdc case the insn symbol is built
     into the enum name.  */

  static struct {
    int index;
    void *label;
  } labels[] = {
    { ARC600F_INSN_X_INVALID, && case_sem_INSN_X_INVALID },
    { ARC600F_INSN_X_AFTER, && case_sem_INSN_X_AFTER },
    { ARC600F_INSN_X_BEFORE, && case_sem_INSN_X_BEFORE },
    { ARC600F_INSN_X_CTI_CHAIN, && case_sem_INSN_X_CTI_CHAIN },
    { ARC600F_INSN_X_CHAIN, && case_sem_INSN_X_CHAIN },
    { ARC600F_INSN_X_BEGIN, && case_sem_INSN_X_BEGIN },
    { ARC600F_INSN_B_S, && case_sem_INSN_B_S },
    { ARC600F_INSN_BCC_S, && case_sem_INSN_BCC_S },
    { ARC600F_INSN_BRCC_S, && case_sem_INSN_BRCC_S },
    { ARC600F_INSN_BCC_L, && case_sem_INSN_BCC_L },
    { ARC600F_INSN_BCC_L_D, && case_sem_INSN_BCC_L_D },
    { ARC600F_INSN_B_L, && case_sem_INSN_B_L },
    { ARC600F_INSN_B_L_D, && case_sem_INSN_B_L_D },
    { ARC600F_INSN_BRCC_RC, && case_sem_INSN_BRCC_RC },
    { ARC600F_INSN_BRCC_RC_D, && case_sem_INSN_BRCC_RC_D },
    { ARC600F_INSN_BRCC_U6, && case_sem_INSN_BRCC_U6 },
    { ARC600F_INSN_BRCC_U6_D, && case_sem_INSN_BRCC_U6_D },
    { ARC600F_INSN_BL_S, && case_sem_INSN_BL_S },
    { ARC600F_INSN_BLCC, && case_sem_INSN_BLCC },
    { ARC600F_INSN_BLCC_D, && case_sem_INSN_BLCC_D },
    { ARC600F_INSN_BL, && case_sem_INSN_BL },
    { ARC600F_INSN_BL_D, && case_sem_INSN_BL_D },
    { ARC600F_INSN_LD_ABS, && case_sem_INSN_LD_ABS },
    { ARC600F_INSN_LD__AW_ABS, && case_sem_INSN_LD__AW_ABS },
    { ARC600F_INSN_LD_AB_ABS, && case_sem_INSN_LD_AB_ABS },
    { ARC600F_INSN_LD_AS_ABS, && case_sem_INSN_LD_AS_ABS },
    { ARC600F_INSN_LD_ABC, && case_sem_INSN_LD_ABC },
    { ARC600F_INSN_LD__AW_ABC, && case_sem_INSN_LD__AW_ABC },
    { ARC600F_INSN_LD_AB_ABC, && case_sem_INSN_LD_AB_ABC },
    { ARC600F_INSN_LD_AS_ABC, && case_sem_INSN_LD_AS_ABC },
    { ARC600F_INSN_LD_S_ABC, && case_sem_INSN_LD_S_ABC },
    { ARC600F_INSN_LD_S_ABU, && case_sem_INSN_LD_S_ABU },
    { ARC600F_INSN_LD_S_ABSP, && case_sem_INSN_LD_S_ABSP },
    { ARC600F_INSN_LD_S_GPREL, && case_sem_INSN_LD_S_GPREL },
    { ARC600F_INSN_LD_S_PCREL, && case_sem_INSN_LD_S_PCREL },
    { ARC600F_INSN_LDB_ABS, && case_sem_INSN_LDB_ABS },
    { ARC600F_INSN_LDB__AW_ABS, && case_sem_INSN_LDB__AW_ABS },
    { ARC600F_INSN_LDB_AB_ABS, && case_sem_INSN_LDB_AB_ABS },
    { ARC600F_INSN_LDB_AS_ABS, && case_sem_INSN_LDB_AS_ABS },
    { ARC600F_INSN_LDB_ABC, && case_sem_INSN_LDB_ABC },
    { ARC600F_INSN_LDB__AW_ABC, && case_sem_INSN_LDB__AW_ABC },
    { ARC600F_INSN_LDB_AB_ABC, && case_sem_INSN_LDB_AB_ABC },
    { ARC600F_INSN_LDB_AS_ABC, && case_sem_INSN_LDB_AS_ABC },
    { ARC600F_INSN_LDB_S_ABC, && case_sem_INSN_LDB_S_ABC },
    { ARC600F_INSN_LDB_S_ABU, && case_sem_INSN_LDB_S_ABU },
    { ARC600F_INSN_LDB_S_ABSP, && case_sem_INSN_LDB_S_ABSP },
    { ARC600F_INSN_LDB_S_GPREL, && case_sem_INSN_LDB_S_GPREL },
    { ARC600F_INSN_LDB_X_ABS, && case_sem_INSN_LDB_X_ABS },
    { ARC600F_INSN_LDB__AW_X_ABS, && case_sem_INSN_LDB__AW_X_ABS },
    { ARC600F_INSN_LDB_AB_X_ABS, && case_sem_INSN_LDB_AB_X_ABS },
    { ARC600F_INSN_LDB_AS_X_ABS, && case_sem_INSN_LDB_AS_X_ABS },
    { ARC600F_INSN_LDB_X_ABC, && case_sem_INSN_LDB_X_ABC },
    { ARC600F_INSN_LDB__AW_X_ABC, && case_sem_INSN_LDB__AW_X_ABC },
    { ARC600F_INSN_LDB_AB_X_ABC, && case_sem_INSN_LDB_AB_X_ABC },
    { ARC600F_INSN_LDB_AS_X_ABC, && case_sem_INSN_LDB_AS_X_ABC },
    { ARC600F_INSN_LDW_ABS, && case_sem_INSN_LDW_ABS },
    { ARC600F_INSN_LDW__AW_ABS, && case_sem_INSN_LDW__AW_ABS },
    { ARC600F_INSN_LDW_AB_ABS, && case_sem_INSN_LDW_AB_ABS },
    { ARC600F_INSN_LDW_AS_ABS, && case_sem_INSN_LDW_AS_ABS },
    { ARC600F_INSN_LDW_ABC, && case_sem_INSN_LDW_ABC },
    { ARC600F_INSN_LDW__AW_ABC, && case_sem_INSN_LDW__AW_ABC },
    { ARC600F_INSN_LDW_AB_ABC, && case_sem_INSN_LDW_AB_ABC },
    { ARC600F_INSN_LDW_AS_ABC, && case_sem_INSN_LDW_AS_ABC },
    { ARC600F_INSN_LDW_S_ABC, && case_sem_INSN_LDW_S_ABC },
    { ARC600F_INSN_LDW_S_ABU, && case_sem_INSN_LDW_S_ABU },
    { ARC600F_INSN_LDW_S_GPREL, && case_sem_INSN_LDW_S_GPREL },
    { ARC600F_INSN_LDW_X_ABS, && case_sem_INSN_LDW_X_ABS },
    { ARC600F_INSN_LDW__AW_X_ABS, && case_sem_INSN_LDW__AW_X_ABS },
    { ARC600F_INSN_LDW_AB_X_ABS, && case_sem_INSN_LDW_AB_X_ABS },
    { ARC600F_INSN_LDW_AS_X_ABS, && case_sem_INSN_LDW_AS_X_ABS },
    { ARC600F_INSN_LDW_X_ABC, && case_sem_INSN_LDW_X_ABC },
    { ARC600F_INSN_LDW__AW_X_ABC, && case_sem_INSN_LDW__AW_X_ABC },
    { ARC600F_INSN_LDW_AB_X_ABC, && case_sem_INSN_LDW_AB_X_ABC },
    { ARC600F_INSN_LDW_AS_X_ABC, && case_sem_INSN_LDW_AS_X_ABC },
    { ARC600F_INSN_LDW_S_X_ABU, && case_sem_INSN_LDW_S_X_ABU },
    { ARC600F_INSN_ST_ABS, && case_sem_INSN_ST_ABS },
    { ARC600F_INSN_ST__AW_ABS, && case_sem_INSN_ST__AW_ABS },
    { ARC600F_INSN_ST_AB_ABS, && case_sem_INSN_ST_AB_ABS },
    { ARC600F_INSN_ST_AS_ABS, && case_sem_INSN_ST_AS_ABS },
    { ARC600F_INSN_ST_S_ABU, && case_sem_INSN_ST_S_ABU },
    { ARC600F_INSN_ST_S_ABSP, && case_sem_INSN_ST_S_ABSP },
    { ARC600F_INSN_STB_ABS, && case_sem_INSN_STB_ABS },
    { ARC600F_INSN_STB__AW_ABS, && case_sem_INSN_STB__AW_ABS },
    { ARC600F_INSN_STB_AB_ABS, && case_sem_INSN_STB_AB_ABS },
    { ARC600F_INSN_STB_AS_ABS, && case_sem_INSN_STB_AS_ABS },
    { ARC600F_INSN_STB_S_ABU, && case_sem_INSN_STB_S_ABU },
    { ARC600F_INSN_STB_S_ABSP, && case_sem_INSN_STB_S_ABSP },
    { ARC600F_INSN_STW_ABS, && case_sem_INSN_STW_ABS },
    { ARC600F_INSN_STW__AW_ABS, && case_sem_INSN_STW__AW_ABS },
    { ARC600F_INSN_STW_AB_ABS, && case_sem_INSN_STW_AB_ABS },
    { ARC600F_INSN_STW_AS_ABS, && case_sem_INSN_STW_AS_ABS },
    { ARC600F_INSN_STW_S_ABU, && case_sem_INSN_STW_S_ABU },
    { ARC600F_INSN_ADD_L_S12__RA_, && case_sem_INSN_ADD_L_S12__RA_ },
    { ARC600F_INSN_ADD_CCU6__RA_, && case_sem_INSN_ADD_CCU6__RA_ },
    { ARC600F_INSN_ADD_L_U6__RA_, && case_sem_INSN_ADD_L_U6__RA_ },
    { ARC600F_INSN_ADD_L_R_R__RA__RC, && case_sem_INSN_ADD_L_R_R__RA__RC },
    { ARC600F_INSN_ADD_CC__RA__RC, && case_sem_INSN_ADD_CC__RA__RC },
    { ARC600F_INSN_ADD_S_ABC, && case_sem_INSN_ADD_S_ABC },
    { ARC600F_INSN_ADD_S_CBU3, && case_sem_INSN_ADD_S_CBU3 },
    { ARC600F_INSN_ADD_S_MCAH, && case_sem_INSN_ADD_S_MCAH },
    { ARC600F_INSN_ADD_S_ABSP, && case_sem_INSN_ADD_S_ABSP },
    { ARC600F_INSN_ADD_S_ASSPSP, && case_sem_INSN_ADD_S_ASSPSP },
    { ARC600F_INSN_ADD_S_GP, && case_sem_INSN_ADD_S_GP },
    { ARC600F_INSN_ADD_S_R_U7, && case_sem_INSN_ADD_S_R_U7 },
    { ARC600F_INSN_ADC_L_S12__RA_, && case_sem_INSN_ADC_L_S12__RA_ },
    { ARC600F_INSN_ADC_CCU6__RA_, && case_sem_INSN_ADC_CCU6__RA_ },
    { ARC600F_INSN_ADC_L_U6__RA_, && case_sem_INSN_ADC_L_U6__RA_ },
    { ARC600F_INSN_ADC_L_R_R__RA__RC, && case_sem_INSN_ADC_L_R_R__RA__RC },
    { ARC600F_INSN_ADC_CC__RA__RC, && case_sem_INSN_ADC_CC__RA__RC },
    { ARC600F_INSN_SUB_L_S12__RA_, && case_sem_INSN_SUB_L_S12__RA_ },
    { ARC600F_INSN_SUB_CCU6__RA_, && case_sem_INSN_SUB_CCU6__RA_ },
    { ARC600F_INSN_SUB_L_U6__RA_, && case_sem_INSN_SUB_L_U6__RA_ },
    { ARC600F_INSN_SUB_L_R_R__RA__RC, && case_sem_INSN_SUB_L_R_R__RA__RC },
    { ARC600F_INSN_SUB_CC__RA__RC, && case_sem_INSN_SUB_CC__RA__RC },
    { ARC600F_INSN_SUB_S_CBU3, && case_sem_INSN_SUB_S_CBU3 },
    { ARC600F_INSN_I16_GO_SUB_S_GO, && case_sem_INSN_I16_GO_SUB_S_GO },
    { ARC600F_INSN_SUB_S_GO_SUB_NE, && case_sem_INSN_SUB_S_GO_SUB_NE },
    { ARC600F_INSN_SUB_S_SSB, && case_sem_INSN_SUB_S_SSB },
    { ARC600F_INSN_SUB_S_ASSPSP, && case_sem_INSN_SUB_S_ASSPSP },
    { ARC600F_INSN_SBC_L_S12__RA_, && case_sem_INSN_SBC_L_S12__RA_ },
    { ARC600F_INSN_SBC_CCU6__RA_, && case_sem_INSN_SBC_CCU6__RA_ },
    { ARC600F_INSN_SBC_L_U6__RA_, && case_sem_INSN_SBC_L_U6__RA_ },
    { ARC600F_INSN_SBC_L_R_R__RA__RC, && case_sem_INSN_SBC_L_R_R__RA__RC },
    { ARC600F_INSN_SBC_CC__RA__RC, && case_sem_INSN_SBC_CC__RA__RC },
    { ARC600F_INSN_AND_L_S12__RA_, && case_sem_INSN_AND_L_S12__RA_ },
    { ARC600F_INSN_AND_CCU6__RA_, && case_sem_INSN_AND_CCU6__RA_ },
    { ARC600F_INSN_AND_L_U6__RA_, && case_sem_INSN_AND_L_U6__RA_ },
    { ARC600F_INSN_AND_L_R_R__RA__RC, && case_sem_INSN_AND_L_R_R__RA__RC },
    { ARC600F_INSN_AND_CC__RA__RC, && case_sem_INSN_AND_CC__RA__RC },
    { ARC600F_INSN_I16_GO_AND_S_GO, && case_sem_INSN_I16_GO_AND_S_GO },
    { ARC600F_INSN_OR_L_S12__RA_, && case_sem_INSN_OR_L_S12__RA_ },
    { ARC600F_INSN_OR_CCU6__RA_, && case_sem_INSN_OR_CCU6__RA_ },
    { ARC600F_INSN_OR_L_U6__RA_, && case_sem_INSN_OR_L_U6__RA_ },
    { ARC600F_INSN_OR_L_R_R__RA__RC, && case_sem_INSN_OR_L_R_R__RA__RC },
    { ARC600F_INSN_OR_CC__RA__RC, && case_sem_INSN_OR_CC__RA__RC },
    { ARC600F_INSN_I16_GO_OR_S_GO, && case_sem_INSN_I16_GO_OR_S_GO },
    { ARC600F_INSN_BIC_L_S12__RA_, && case_sem_INSN_BIC_L_S12__RA_ },
    { ARC600F_INSN_BIC_CCU6__RA_, && case_sem_INSN_BIC_CCU6__RA_ },
    { ARC600F_INSN_BIC_L_U6__RA_, && case_sem_INSN_BIC_L_U6__RA_ },
    { ARC600F_INSN_BIC_L_R_R__RA__RC, && case_sem_INSN_BIC_L_R_R__RA__RC },
    { ARC600F_INSN_BIC_CC__RA__RC, && case_sem_INSN_BIC_CC__RA__RC },
    { ARC600F_INSN_I16_GO_BIC_S_GO, && case_sem_INSN_I16_GO_BIC_S_GO },
    { ARC600F_INSN_XOR_L_S12__RA_, && case_sem_INSN_XOR_L_S12__RA_ },
    { ARC600F_INSN_XOR_CCU6__RA_, && case_sem_INSN_XOR_CCU6__RA_ },
    { ARC600F_INSN_XOR_L_U6__RA_, && case_sem_INSN_XOR_L_U6__RA_ },
    { ARC600F_INSN_XOR_L_R_R__RA__RC, && case_sem_INSN_XOR_L_R_R__RA__RC },
    { ARC600F_INSN_XOR_CC__RA__RC, && case_sem_INSN_XOR_CC__RA__RC },
    { ARC600F_INSN_I16_GO_XOR_S_GO, && case_sem_INSN_I16_GO_XOR_S_GO },
    { ARC600F_INSN_MAX_L_S12__RA_, && case_sem_INSN_MAX_L_S12__RA_ },
    { ARC600F_INSN_MAX_CCU6__RA_, && case_sem_INSN_MAX_CCU6__RA_ },
    { ARC600F_INSN_MAX_L_U6__RA_, && case_sem_INSN_MAX_L_U6__RA_ },
    { ARC600F_INSN_MAX_L_R_R__RA__RC, && case_sem_INSN_MAX_L_R_R__RA__RC },
    { ARC600F_INSN_MAX_CC__RA__RC, && case_sem_INSN_MAX_CC__RA__RC },
    { ARC600F_INSN_MIN_L_S12__RA_, && case_sem_INSN_MIN_L_S12__RA_ },
    { ARC600F_INSN_MIN_CCU6__RA_, && case_sem_INSN_MIN_CCU6__RA_ },
    { ARC600F_INSN_MIN_L_U6__RA_, && case_sem_INSN_MIN_L_U6__RA_ },
    { ARC600F_INSN_MIN_L_R_R__RA__RC, && case_sem_INSN_MIN_L_R_R__RA__RC },
    { ARC600F_INSN_MIN_CC__RA__RC, && case_sem_INSN_MIN_CC__RA__RC },
    { ARC600F_INSN_MOV_L_S12_, && case_sem_INSN_MOV_L_S12_ },
    { ARC600F_INSN_MOV_CCU6_, && case_sem_INSN_MOV_CCU6_ },
    { ARC600F_INSN_MOV_L_U6_, && case_sem_INSN_MOV_L_U6_ },
    { ARC600F_INSN_MOV_L_R_R__RC, && case_sem_INSN_MOV_L_R_R__RC },
    { ARC600F_INSN_MOV_CC__RC, && case_sem_INSN_MOV_CC__RC },
    { ARC600F_INSN_MOV_S_MCAH, && case_sem_INSN_MOV_S_MCAH },
    { ARC600F_INSN_MOV_S_MCAHB, && case_sem_INSN_MOV_S_MCAHB },
    { ARC600F_INSN_MOV_S_R_U7, && case_sem_INSN_MOV_S_R_U7 },
    { ARC600F_INSN_TST_L_S12_, && case_sem_INSN_TST_L_S12_ },
    { ARC600F_INSN_TST_CCU6_, && case_sem_INSN_TST_CCU6_ },
    { ARC600F_INSN_TST_L_U6_, && case_sem_INSN_TST_L_U6_ },
    { ARC600F_INSN_TST_L_R_R__RC, && case_sem_INSN_TST_L_R_R__RC },
    { ARC600F_INSN_TST_CC__RC, && case_sem_INSN_TST_CC__RC },
    { ARC600F_INSN_TST_S_GO, && case_sem_INSN_TST_S_GO },
    { ARC600F_INSN_CMP_L_S12_, && case_sem_INSN_CMP_L_S12_ },
    { ARC600F_INSN_CMP_CCU6_, && case_sem_INSN_CMP_CCU6_ },
    { ARC600F_INSN_CMP_L_U6_, && case_sem_INSN_CMP_L_U6_ },
    { ARC600F_INSN_CMP_L_R_R__RC, && case_sem_INSN_CMP_L_R_R__RC },
    { ARC600F_INSN_CMP_CC__RC, && case_sem_INSN_CMP_CC__RC },
    { ARC600F_INSN_CMP_S_MCAH, && case_sem_INSN_CMP_S_MCAH },
    { ARC600F_INSN_CMP_S_R_U7, && case_sem_INSN_CMP_S_R_U7 },
    { ARC600F_INSN_RCMP_L_S12_, && case_sem_INSN_RCMP_L_S12_ },
    { ARC600F_INSN_RCMP_CCU6_, && case_sem_INSN_RCMP_CCU6_ },
    { ARC600F_INSN_RCMP_L_U6_, && case_sem_INSN_RCMP_L_U6_ },
    { ARC600F_INSN_RCMP_L_R_R__RC, && case_sem_INSN_RCMP_L_R_R__RC },
    { ARC600F_INSN_RCMP_CC__RC, && case_sem_INSN_RCMP_CC__RC },
    { ARC600F_INSN_RSUB_L_S12__RA_, && case_sem_INSN_RSUB_L_S12__RA_ },
    { ARC600F_INSN_RSUB_CCU6__RA_, && case_sem_INSN_RSUB_CCU6__RA_ },
    { ARC600F_INSN_RSUB_L_U6__RA_, && case_sem_INSN_RSUB_L_U6__RA_ },
    { ARC600F_INSN_RSUB_L_R_R__RA__RC, && case_sem_INSN_RSUB_L_R_R__RA__RC },
    { ARC600F_INSN_RSUB_CC__RA__RC, && case_sem_INSN_RSUB_CC__RA__RC },
    { ARC600F_INSN_BSET_L_S12__RA_, && case_sem_INSN_BSET_L_S12__RA_ },
    { ARC600F_INSN_BSET_CCU6__RA_, && case_sem_INSN_BSET_CCU6__RA_ },
    { ARC600F_INSN_BSET_L_U6__RA_, && case_sem_INSN_BSET_L_U6__RA_ },
    { ARC600F_INSN_BSET_L_R_R__RA__RC, && case_sem_INSN_BSET_L_R_R__RA__RC },
    { ARC600F_INSN_BSET_CC__RA__RC, && case_sem_INSN_BSET_CC__RA__RC },
    { ARC600F_INSN_BSET_S_SSB, && case_sem_INSN_BSET_S_SSB },
    { ARC600F_INSN_BCLR_L_S12__RA_, && case_sem_INSN_BCLR_L_S12__RA_ },
    { ARC600F_INSN_BCLR_CCU6__RA_, && case_sem_INSN_BCLR_CCU6__RA_ },
    { ARC600F_INSN_BCLR_L_U6__RA_, && case_sem_INSN_BCLR_L_U6__RA_ },
    { ARC600F_INSN_BCLR_L_R_R__RA__RC, && case_sem_INSN_BCLR_L_R_R__RA__RC },
    { ARC600F_INSN_BCLR_CC__RA__RC, && case_sem_INSN_BCLR_CC__RA__RC },
    { ARC600F_INSN_BCLR_S_SSB, && case_sem_INSN_BCLR_S_SSB },
    { ARC600F_INSN_BTST_L_S12_, && case_sem_INSN_BTST_L_S12_ },
    { ARC600F_INSN_BTST_CCU6_, && case_sem_INSN_BTST_CCU6_ },
    { ARC600F_INSN_BTST_L_U6_, && case_sem_INSN_BTST_L_U6_ },
    { ARC600F_INSN_BTST_L_R_R__RC, && case_sem_INSN_BTST_L_R_R__RC },
    { ARC600F_INSN_BTST_CC__RC, && case_sem_INSN_BTST_CC__RC },
    { ARC600F_INSN_BTST_S_SSB, && case_sem_INSN_BTST_S_SSB },
    { ARC600F_INSN_BXOR_L_S12__RA_, && case_sem_INSN_BXOR_L_S12__RA_ },
    { ARC600F_INSN_BXOR_CCU6__RA_, && case_sem_INSN_BXOR_CCU6__RA_ },
    { ARC600F_INSN_BXOR_L_U6__RA_, && case_sem_INSN_BXOR_L_U6__RA_ },
    { ARC600F_INSN_BXOR_L_R_R__RA__RC, && case_sem_INSN_BXOR_L_R_R__RA__RC },
    { ARC600F_INSN_BXOR_CC__RA__RC, && case_sem_INSN_BXOR_CC__RA__RC },
    { ARC600F_INSN_BMSK_L_S12__RA_, && case_sem_INSN_BMSK_L_S12__RA_ },
    { ARC600F_INSN_BMSK_CCU6__RA_, && case_sem_INSN_BMSK_CCU6__RA_ },
    { ARC600F_INSN_BMSK_L_U6__RA_, && case_sem_INSN_BMSK_L_U6__RA_ },
    { ARC600F_INSN_BMSK_L_R_R__RA__RC, && case_sem_INSN_BMSK_L_R_R__RA__RC },
    { ARC600F_INSN_BMSK_CC__RA__RC, && case_sem_INSN_BMSK_CC__RA__RC },
    { ARC600F_INSN_BMSK_S_SSB, && case_sem_INSN_BMSK_S_SSB },
    { ARC600F_INSN_ADD1_L_S12__RA_, && case_sem_INSN_ADD1_L_S12__RA_ },
    { ARC600F_INSN_ADD1_CCU6__RA_, && case_sem_INSN_ADD1_CCU6__RA_ },
    { ARC600F_INSN_ADD1_L_U6__RA_, && case_sem_INSN_ADD1_L_U6__RA_ },
    { ARC600F_INSN_ADD1_L_R_R__RA__RC, && case_sem_INSN_ADD1_L_R_R__RA__RC },
    { ARC600F_INSN_ADD1_CC__RA__RC, && case_sem_INSN_ADD1_CC__RA__RC },
    { ARC600F_INSN_I16_GO_ADD1_S_GO, && case_sem_INSN_I16_GO_ADD1_S_GO },
    { ARC600F_INSN_ADD2_L_S12__RA_, && case_sem_INSN_ADD2_L_S12__RA_ },
    { ARC600F_INSN_ADD2_CCU6__RA_, && case_sem_INSN_ADD2_CCU6__RA_ },
    { ARC600F_INSN_ADD2_L_U6__RA_, && case_sem_INSN_ADD2_L_U6__RA_ },
    { ARC600F_INSN_ADD2_L_R_R__RA__RC, && case_sem_INSN_ADD2_L_R_R__RA__RC },
    { ARC600F_INSN_ADD2_CC__RA__RC, && case_sem_INSN_ADD2_CC__RA__RC },
    { ARC600F_INSN_I16_GO_ADD2_S_GO, && case_sem_INSN_I16_GO_ADD2_S_GO },
    { ARC600F_INSN_ADD3_L_S12__RA_, && case_sem_INSN_ADD3_L_S12__RA_ },
    { ARC600F_INSN_ADD3_CCU6__RA_, && case_sem_INSN_ADD3_CCU6__RA_ },
    { ARC600F_INSN_ADD3_L_U6__RA_, && case_sem_INSN_ADD3_L_U6__RA_ },
    { ARC600F_INSN_ADD3_L_R_R__RA__RC, && case_sem_INSN_ADD3_L_R_R__RA__RC },
    { ARC600F_INSN_ADD3_CC__RA__RC, && case_sem_INSN_ADD3_CC__RA__RC },
    { ARC600F_INSN_I16_GO_ADD3_S_GO, && case_sem_INSN_I16_GO_ADD3_S_GO },
    { ARC600F_INSN_SUB1_L_S12__RA_, && case_sem_INSN_SUB1_L_S12__RA_ },
    { ARC600F_INSN_SUB1_CCU6__RA_, && case_sem_INSN_SUB1_CCU6__RA_ },
    { ARC600F_INSN_SUB1_L_U6__RA_, && case_sem_INSN_SUB1_L_U6__RA_ },
    { ARC600F_INSN_SUB1_L_R_R__RA__RC, && case_sem_INSN_SUB1_L_R_R__RA__RC },
    { ARC600F_INSN_SUB1_CC__RA__RC, && case_sem_INSN_SUB1_CC__RA__RC },
    { ARC600F_INSN_SUB2_L_S12__RA_, && case_sem_INSN_SUB2_L_S12__RA_ },
    { ARC600F_INSN_SUB2_CCU6__RA_, && case_sem_INSN_SUB2_CCU6__RA_ },
    { ARC600F_INSN_SUB2_L_U6__RA_, && case_sem_INSN_SUB2_L_U6__RA_ },
    { ARC600F_INSN_SUB2_L_R_R__RA__RC, && case_sem_INSN_SUB2_L_R_R__RA__RC },
    { ARC600F_INSN_SUB2_CC__RA__RC, && case_sem_INSN_SUB2_CC__RA__RC },
    { ARC600F_INSN_SUB3_L_S12__RA_, && case_sem_INSN_SUB3_L_S12__RA_ },
    { ARC600F_INSN_SUB3_CCU6__RA_, && case_sem_INSN_SUB3_CCU6__RA_ },
    { ARC600F_INSN_SUB3_L_U6__RA_, && case_sem_INSN_SUB3_L_U6__RA_ },
    { ARC600F_INSN_SUB3_L_R_R__RA__RC, && case_sem_INSN_SUB3_L_R_R__RA__RC },
    { ARC600F_INSN_SUB3_CC__RA__RC, && case_sem_INSN_SUB3_CC__RA__RC },
    { ARC600F_INSN_MPY_L_S12__RA_, && case_sem_INSN_MPY_L_S12__RA_ },
    { ARC600F_INSN_MPY_CCU6__RA_, && case_sem_INSN_MPY_CCU6__RA_ },
    { ARC600F_INSN_MPY_L_U6__RA_, && case_sem_INSN_MPY_L_U6__RA_ },
    { ARC600F_INSN_MPY_L_R_R__RA__RC, && case_sem_INSN_MPY_L_R_R__RA__RC },
    { ARC600F_INSN_MPY_CC__RA__RC, && case_sem_INSN_MPY_CC__RA__RC },
    { ARC600F_INSN_MPYH_L_S12__RA_, && case_sem_INSN_MPYH_L_S12__RA_ },
    { ARC600F_INSN_MPYH_CCU6__RA_, && case_sem_INSN_MPYH_CCU6__RA_ },
    { ARC600F_INSN_MPYH_L_U6__RA_, && case_sem_INSN_MPYH_L_U6__RA_ },
    { ARC600F_INSN_MPYH_L_R_R__RA__RC, && case_sem_INSN_MPYH_L_R_R__RA__RC },
    { ARC600F_INSN_MPYH_CC__RA__RC, && case_sem_INSN_MPYH_CC__RA__RC },
    { ARC600F_INSN_MPYHU_L_S12__RA_, && case_sem_INSN_MPYHU_L_S12__RA_ },
    { ARC600F_INSN_MPYHU_CCU6__RA_, && case_sem_INSN_MPYHU_CCU6__RA_ },
    { ARC600F_INSN_MPYHU_L_U6__RA_, && case_sem_INSN_MPYHU_L_U6__RA_ },
    { ARC600F_INSN_MPYHU_L_R_R__RA__RC, && case_sem_INSN_MPYHU_L_R_R__RA__RC },
    { ARC600F_INSN_MPYHU_CC__RA__RC, && case_sem_INSN_MPYHU_CC__RA__RC },
    { ARC600F_INSN_MPYU_L_S12__RA_, && case_sem_INSN_MPYU_L_S12__RA_ },
    { ARC600F_INSN_MPYU_CCU6__RA_, && case_sem_INSN_MPYU_CCU6__RA_ },
    { ARC600F_INSN_MPYU_L_U6__RA_, && case_sem_INSN_MPYU_L_U6__RA_ },
    { ARC600F_INSN_MPYU_L_R_R__RA__RC, && case_sem_INSN_MPYU_L_R_R__RA__RC },
    { ARC600F_INSN_MPYU_CC__RA__RC, && case_sem_INSN_MPYU_CC__RA__RC },
    { ARC600F_INSN_J_L_R_R___RC_NOILINK_, && case_sem_INSN_J_L_R_R___RC_NOILINK_ },
    { ARC600F_INSN_J_CC___RC_NOILINK_, && case_sem_INSN_J_CC___RC_NOILINK_ },
    { ARC600F_INSN_J_L_R_R___RC_ILINK_, && case_sem_INSN_J_L_R_R___RC_ILINK_ },
    { ARC600F_INSN_J_CC___RC_ILINK_, && case_sem_INSN_J_CC___RC_ILINK_ },
    { ARC600F_INSN_J_L_S12_, && case_sem_INSN_J_L_S12_ },
    { ARC600F_INSN_J_CCU6_, && case_sem_INSN_J_CCU6_ },
    { ARC600F_INSN_J_L_U6_, && case_sem_INSN_J_L_U6_ },
    { ARC600F_INSN_J_S, && case_sem_INSN_J_S },
    { ARC600F_INSN_J_S__S, && case_sem_INSN_J_S__S },
    { ARC600F_INSN_J_SEQ__S, && case_sem_INSN_J_SEQ__S },
    { ARC600F_INSN_J_SNE__S, && case_sem_INSN_J_SNE__S },
    { ARC600F_INSN_J_L_S12_D_, && case_sem_INSN_J_L_S12_D_ },
    { ARC600F_INSN_J_CCU6_D_, && case_sem_INSN_J_CCU6_D_ },
    { ARC600F_INSN_J_L_U6_D_, && case_sem_INSN_J_L_U6_D_ },
    { ARC600F_INSN_J_L_R_R_D___RC_, && case_sem_INSN_J_L_R_R_D___RC_ },
    { ARC600F_INSN_J_CC_D___RC_, && case_sem_INSN_J_CC_D___RC_ },
    { ARC600F_INSN_J_S_D, && case_sem_INSN_J_S_D },
    { ARC600F_INSN_J_S__S_D, && case_sem_INSN_J_S__S_D },
    { ARC600F_INSN_JL_L_S12_, && case_sem_INSN_JL_L_S12_ },
    { ARC600F_INSN_JL_CCU6_, && case_sem_INSN_JL_CCU6_ },
    { ARC600F_INSN_JL_L_U6_, && case_sem_INSN_JL_L_U6_ },
    { ARC600F_INSN_JL_S, && case_sem_INSN_JL_S },
    { ARC600F_INSN_JL_L_R_R___RC_NOILINK_, && case_sem_INSN_JL_L_R_R___RC_NOILINK_ },
    { ARC600F_INSN_JL_CC___RC_NOILINK_, && case_sem_INSN_JL_CC___RC_NOILINK_ },
    { ARC600F_INSN_JL_L_S12_D_, && case_sem_INSN_JL_L_S12_D_ },
    { ARC600F_INSN_JL_CCU6_D_, && case_sem_INSN_JL_CCU6_D_ },
    { ARC600F_INSN_JL_L_U6_D_, && case_sem_INSN_JL_L_U6_D_ },
    { ARC600F_INSN_JL_L_R_R_D___RC_, && case_sem_INSN_JL_L_R_R_D___RC_ },
    { ARC600F_INSN_JL_CC_D___RC_, && case_sem_INSN_JL_CC_D___RC_ },
    { ARC600F_INSN_JL_S_D, && case_sem_INSN_JL_S_D },
    { ARC600F_INSN_LP_L_S12_, && case_sem_INSN_LP_L_S12_ },
    { ARC600F_INSN_LPCC_CCU6, && case_sem_INSN_LPCC_CCU6 },
    { ARC600F_INSN_FLAG_L_S12_, && case_sem_INSN_FLAG_L_S12_ },
    { ARC600F_INSN_FLAG_CCU6_, && case_sem_INSN_FLAG_CCU6_ },
    { ARC600F_INSN_FLAG_L_U6_, && case_sem_INSN_FLAG_L_U6_ },
    { ARC600F_INSN_FLAG_L_R_R__RC, && case_sem_INSN_FLAG_L_R_R__RC },
    { ARC600F_INSN_FLAG_CC__RC, && case_sem_INSN_FLAG_CC__RC },
    { ARC600F_INSN_LR_L_R_R___RC_, && case_sem_INSN_LR_L_R_R___RC_ },
    { ARC600F_INSN_LR_L_S12_, && case_sem_INSN_LR_L_S12_ },
    { ARC600F_INSN_LR_L_U6_, && case_sem_INSN_LR_L_U6_ },
    { ARC600F_INSN_SR_L_R_R___RC_, && case_sem_INSN_SR_L_R_R___RC_ },
    { ARC600F_INSN_SR_L_S12_, && case_sem_INSN_SR_L_S12_ },
    { ARC600F_INSN_SR_L_U6_, && case_sem_INSN_SR_L_U6_ },
    { ARC600F_INSN_ASL_L_R_R__RC, && case_sem_INSN_ASL_L_R_R__RC },
    { ARC600F_INSN_ASL_L_U6_, && case_sem_INSN_ASL_L_U6_ },
    { ARC600F_INSN_I16_GO_ASL_S_GO, && case_sem_INSN_I16_GO_ASL_S_GO },
    { ARC600F_INSN_ASR_L_R_R__RC, && case_sem_INSN_ASR_L_R_R__RC },
    { ARC600F_INSN_ASR_L_U6_, && case_sem_INSN_ASR_L_U6_ },
    { ARC600F_INSN_I16_GO_ASR_S_GO, && case_sem_INSN_I16_GO_ASR_S_GO },
    { ARC600F_INSN_LSR_L_R_R__RC, && case_sem_INSN_LSR_L_R_R__RC },
    { ARC600F_INSN_LSR_L_U6_, && case_sem_INSN_LSR_L_U6_ },
    { ARC600F_INSN_I16_GO_LSR_S_GO, && case_sem_INSN_I16_GO_LSR_S_GO },
    { ARC600F_INSN_ROR_L_R_R__RC, && case_sem_INSN_ROR_L_R_R__RC },
    { ARC600F_INSN_ROR_L_U6_, && case_sem_INSN_ROR_L_U6_ },
    { ARC600F_INSN_RRC_L_R_R__RC, && case_sem_INSN_RRC_L_R_R__RC },
    { ARC600F_INSN_RRC_L_U6_, && case_sem_INSN_RRC_L_U6_ },
    { ARC600F_INSN_SEXB_L_R_R__RC, && case_sem_INSN_SEXB_L_R_R__RC },
    { ARC600F_INSN_SEXB_L_U6_, && case_sem_INSN_SEXB_L_U6_ },
    { ARC600F_INSN_I16_GO_SEXB_S_GO, && case_sem_INSN_I16_GO_SEXB_S_GO },
    { ARC600F_INSN_SEXW_L_R_R__RC, && case_sem_INSN_SEXW_L_R_R__RC },
    { ARC600F_INSN_SEXW_L_U6_, && case_sem_INSN_SEXW_L_U6_ },
    { ARC600F_INSN_I16_GO_SEXW_S_GO, && case_sem_INSN_I16_GO_SEXW_S_GO },
    { ARC600F_INSN_EXTB_L_R_R__RC, && case_sem_INSN_EXTB_L_R_R__RC },
    { ARC600F_INSN_EXTB_L_U6_, && case_sem_INSN_EXTB_L_U6_ },
    { ARC600F_INSN_I16_GO_EXTB_S_GO, && case_sem_INSN_I16_GO_EXTB_S_GO },
    { ARC600F_INSN_EXTW_L_R_R__RC, && case_sem_INSN_EXTW_L_R_R__RC },
    { ARC600F_INSN_EXTW_L_U6_, && case_sem_INSN_EXTW_L_U6_ },
    { ARC600F_INSN_I16_GO_EXTW_S_GO, && case_sem_INSN_I16_GO_EXTW_S_GO },
    { ARC600F_INSN_ABS_L_R_R__RC, && case_sem_INSN_ABS_L_R_R__RC },
    { ARC600F_INSN_ABS_L_U6_, && case_sem_INSN_ABS_L_U6_ },
    { ARC600F_INSN_I16_GO_ABS_S_GO, && case_sem_INSN_I16_GO_ABS_S_GO },
    { ARC600F_INSN_NOT_L_R_R__RC, && case_sem_INSN_NOT_L_R_R__RC },
    { ARC600F_INSN_NOT_L_U6_, && case_sem_INSN_NOT_L_U6_ },
    { ARC600F_INSN_I16_GO_NOT_S_GO, && case_sem_INSN_I16_GO_NOT_S_GO },
    { ARC600F_INSN_RLC_L_R_R__RC, && case_sem_INSN_RLC_L_R_R__RC },
    { ARC600F_INSN_RLC_L_U6_, && case_sem_INSN_RLC_L_U6_ },
    { ARC600F_INSN_I16_GO_NEG_S_GO, && case_sem_INSN_I16_GO_NEG_S_GO },
    { ARC600F_INSN_SWI, && case_sem_INSN_SWI },
    { ARC600F_INSN_TRAP_S, && case_sem_INSN_TRAP_S },
    { ARC600F_INSN_BRK, && case_sem_INSN_BRK },
    { ARC600F_INSN_BRK_S, && case_sem_INSN_BRK_S },
    { ARC600F_INSN_ASL_L_S12__RA_, && case_sem_INSN_ASL_L_S12__RA_ },
    { ARC600F_INSN_ASL_CCU6__RA_, && case_sem_INSN_ASL_CCU6__RA_ },
    { ARC600F_INSN_ASL_L_U6__RA_, && case_sem_INSN_ASL_L_U6__RA_ },
    { ARC600F_INSN_ASL_L_R_R__RA__RC, && case_sem_INSN_ASL_L_R_R__RA__RC },
    { ARC600F_INSN_ASL_CC__RA__RC, && case_sem_INSN_ASL_CC__RA__RC },
    { ARC600F_INSN_ASL_S_CBU3, && case_sem_INSN_ASL_S_CBU3 },
    { ARC600F_INSN_ASL_S_SSB, && case_sem_INSN_ASL_S_SSB },
    { ARC600F_INSN_I16_GO_ASLM_S_GO, && case_sem_INSN_I16_GO_ASLM_S_GO },
    { ARC600F_INSN_LSR_L_S12__RA_, && case_sem_INSN_LSR_L_S12__RA_ },
    { ARC600F_INSN_LSR_CCU6__RA_, && case_sem_INSN_LSR_CCU6__RA_ },
    { ARC600F_INSN_LSR_L_U6__RA_, && case_sem_INSN_LSR_L_U6__RA_ },
    { ARC600F_INSN_LSR_L_R_R__RA__RC, && case_sem_INSN_LSR_L_R_R__RA__RC },
    { ARC600F_INSN_LSR_CC__RA__RC, && case_sem_INSN_LSR_CC__RA__RC },
    { ARC600F_INSN_LSR_S_SSB, && case_sem_INSN_LSR_S_SSB },
    { ARC600F_INSN_I16_GO_LSRM_S_GO, && case_sem_INSN_I16_GO_LSRM_S_GO },
    { ARC600F_INSN_ASR_L_S12__RA_, && case_sem_INSN_ASR_L_S12__RA_ },
    { ARC600F_INSN_ASR_CCU6__RA_, && case_sem_INSN_ASR_CCU6__RA_ },
    { ARC600F_INSN_ASR_L_U6__RA_, && case_sem_INSN_ASR_L_U6__RA_ },
    { ARC600F_INSN_ASR_L_R_R__RA__RC, && case_sem_INSN_ASR_L_R_R__RA__RC },
    { ARC600F_INSN_ASR_CC__RA__RC, && case_sem_INSN_ASR_CC__RA__RC },
    { ARC600F_INSN_ASR_S_CBU3, && case_sem_INSN_ASR_S_CBU3 },
    { ARC600F_INSN_ASR_S_SSB, && case_sem_INSN_ASR_S_SSB },
    { ARC600F_INSN_I16_GO_ASRM_S_GO, && case_sem_INSN_I16_GO_ASRM_S_GO },
    { ARC600F_INSN_ROR_L_S12__RA_, && case_sem_INSN_ROR_L_S12__RA_ },
    { ARC600F_INSN_ROR_CCU6__RA_, && case_sem_INSN_ROR_CCU6__RA_ },
    { ARC600F_INSN_ROR_L_U6__RA_, && case_sem_INSN_ROR_L_U6__RA_ },
    { ARC600F_INSN_ROR_L_R_R__RA__RC, && case_sem_INSN_ROR_L_R_R__RA__RC },
    { ARC600F_INSN_ROR_CC__RA__RC, && case_sem_INSN_ROR_CC__RA__RC },
    { ARC600F_INSN_MUL64_L_S12_, && case_sem_INSN_MUL64_L_S12_ },
    { ARC600F_INSN_MUL64_CCU6_, && case_sem_INSN_MUL64_CCU6_ },
    { ARC600F_INSN_MUL64_L_U6_, && case_sem_INSN_MUL64_L_U6_ },
    { ARC600F_INSN_MUL64_L_R_R__RC, && case_sem_INSN_MUL64_L_R_R__RC },
    { ARC600F_INSN_MUL64_CC__RC, && case_sem_INSN_MUL64_CC__RC },
    { ARC600F_INSN_MUL64_S_GO, && case_sem_INSN_MUL64_S_GO },
    { ARC600F_INSN_MULU64_L_S12_, && case_sem_INSN_MULU64_L_S12_ },
    { ARC600F_INSN_MULU64_CCU6_, && case_sem_INSN_MULU64_CCU6_ },
    { ARC600F_INSN_MULU64_L_U6_, && case_sem_INSN_MULU64_L_U6_ },
    { ARC600F_INSN_MULU64_L_R_R__RC, && case_sem_INSN_MULU64_L_R_R__RC },
    { ARC600F_INSN_MULU64_CC__RC, && case_sem_INSN_MULU64_CC__RC },
    { ARC600F_INSN_ADDS_L_S12__RA_, && case_sem_INSN_ADDS_L_S12__RA_ },
    { ARC600F_INSN_ADDS_CCU6__RA_, && case_sem_INSN_ADDS_CCU6__RA_ },
    { ARC600F_INSN_ADDS_L_U6__RA_, && case_sem_INSN_ADDS_L_U6__RA_ },
    { ARC600F_INSN_ADDS_L_R_R__RA__RC, && case_sem_INSN_ADDS_L_R_R__RA__RC },
    { ARC600F_INSN_ADDS_CC__RA__RC, && case_sem_INSN_ADDS_CC__RA__RC },
    { ARC600F_INSN_SUBS_L_S12__RA_, && case_sem_INSN_SUBS_L_S12__RA_ },
    { ARC600F_INSN_SUBS_CCU6__RA_, && case_sem_INSN_SUBS_CCU6__RA_ },
    { ARC600F_INSN_SUBS_L_U6__RA_, && case_sem_INSN_SUBS_L_U6__RA_ },
    { ARC600F_INSN_SUBS_L_R_R__RA__RC, && case_sem_INSN_SUBS_L_R_R__RA__RC },
    { ARC600F_INSN_SUBS_CC__RA__RC, && case_sem_INSN_SUBS_CC__RA__RC },
    { ARC600F_INSN_DIVAW_L_S12__RA_, && case_sem_INSN_DIVAW_L_S12__RA_ },
    { ARC600F_INSN_DIVAW_CCU6__RA_, && case_sem_INSN_DIVAW_CCU6__RA_ },
    { ARC600F_INSN_DIVAW_L_U6__RA_, && case_sem_INSN_DIVAW_L_U6__RA_ },
    { ARC600F_INSN_DIVAW_L_R_R__RA__RC, && case_sem_INSN_DIVAW_L_R_R__RA__RC },
    { ARC600F_INSN_DIVAW_CC__RA__RC, && case_sem_INSN_DIVAW_CC__RA__RC },
    { ARC600F_INSN_ASLS_L_S12__RA_, && case_sem_INSN_ASLS_L_S12__RA_ },
    { ARC600F_INSN_ASLS_CCU6__RA_, && case_sem_INSN_ASLS_CCU6__RA_ },
    { ARC600F_INSN_ASLS_L_U6__RA_, && case_sem_INSN_ASLS_L_U6__RA_ },
    { ARC600F_INSN_ASLS_L_R_R__RA__RC, && case_sem_INSN_ASLS_L_R_R__RA__RC },
    { ARC600F_INSN_ASLS_CC__RA__RC, && case_sem_INSN_ASLS_CC__RA__RC },
    { ARC600F_INSN_ASRS_L_S12__RA_, && case_sem_INSN_ASRS_L_S12__RA_ },
    { ARC600F_INSN_ASRS_CCU6__RA_, && case_sem_INSN_ASRS_CCU6__RA_ },
    { ARC600F_INSN_ASRS_L_U6__RA_, && case_sem_INSN_ASRS_L_U6__RA_ },
    { ARC600F_INSN_ASRS_L_R_R__RA__RC, && case_sem_INSN_ASRS_L_R_R__RA__RC },
    { ARC600F_INSN_ASRS_CC__RA__RC, && case_sem_INSN_ASRS_CC__RA__RC },
    { ARC600F_INSN_ADDSDW_L_S12__RA_, && case_sem_INSN_ADDSDW_L_S12__RA_ },
    { ARC600F_INSN_ADDSDW_CCU6__RA_, && case_sem_INSN_ADDSDW_CCU6__RA_ },
    { ARC600F_INSN_ADDSDW_L_U6__RA_, && case_sem_INSN_ADDSDW_L_U6__RA_ },
    { ARC600F_INSN_ADDSDW_L_R_R__RA__RC, && case_sem_INSN_ADDSDW_L_R_R__RA__RC },
    { ARC600F_INSN_ADDSDW_CC__RA__RC, && case_sem_INSN_ADDSDW_CC__RA__RC },
    { ARC600F_INSN_SUBSDW_L_S12__RA_, && case_sem_INSN_SUBSDW_L_S12__RA_ },
    { ARC600F_INSN_SUBSDW_CCU6__RA_, && case_sem_INSN_SUBSDW_CCU6__RA_ },
    { ARC600F_INSN_SUBSDW_L_U6__RA_, && case_sem_INSN_SUBSDW_L_U6__RA_ },
    { ARC600F_INSN_SUBSDW_L_R_R__RA__RC, && case_sem_INSN_SUBSDW_L_R_R__RA__RC },
    { ARC600F_INSN_SUBSDW_CC__RA__RC, && case_sem_INSN_SUBSDW_CC__RA__RC },
    { ARC600F_INSN_SWAP_L_R_R__RC, && case_sem_INSN_SWAP_L_R_R__RC },
    { ARC600F_INSN_SWAP_L_U6_, && case_sem_INSN_SWAP_L_U6_ },
    { ARC600F_INSN_NORM_L_R_R__RC, && case_sem_INSN_NORM_L_R_R__RC },
    { ARC600F_INSN_NORM_L_U6_, && case_sem_INSN_NORM_L_U6_ },
    { ARC600F_INSN_RND16_L_R_R__RC, && case_sem_INSN_RND16_L_R_R__RC },
    { ARC600F_INSN_RND16_L_U6_, && case_sem_INSN_RND16_L_U6_ },
    { ARC600F_INSN_ABSSW_L_R_R__RC, && case_sem_INSN_ABSSW_L_R_R__RC },
    { ARC600F_INSN_ABSSW_L_U6_, && case_sem_INSN_ABSSW_L_U6_ },
    { ARC600F_INSN_ABSS_L_R_R__RC, && case_sem_INSN_ABSS_L_R_R__RC },
    { ARC600F_INSN_ABSS_L_U6_, && case_sem_INSN_ABSS_L_U6_ },
    { ARC600F_INSN_NEGSW_L_R_R__RC, && case_sem_INSN_NEGSW_L_R_R__RC },
    { ARC600F_INSN_NEGSW_L_U6_, && case_sem_INSN_NEGSW_L_U6_ },
    { ARC600F_INSN_NEGS_L_R_R__RC, && case_sem_INSN_NEGS_L_R_R__RC },
    { ARC600F_INSN_NEGS_L_U6_, && case_sem_INSN_NEGS_L_U6_ },
    { ARC600F_INSN_NORMW_L_R_R__RC, && case_sem_INSN_NORMW_L_R_R__RC },
    { ARC600F_INSN_NORMW_L_U6_, && case_sem_INSN_NORMW_L_U6_ },
    { ARC600F_INSN_NOP_S, && case_sem_INSN_NOP_S },
    { ARC600F_INSN_UNIMP_S, && case_sem_INSN_UNIMP_S },
    { ARC600F_INSN_POP_S_B, && case_sem_INSN_POP_S_B },
    { ARC600F_INSN_POP_S_BLINK, && case_sem_INSN_POP_S_BLINK },
    { ARC600F_INSN_PUSH_S_B, && case_sem_INSN_PUSH_S_B },
    { ARC600F_INSN_PUSH_S_BLINK, && case_sem_INSN_PUSH_S_BLINK },
    { ARC600F_INSN_MULLW_L_S12__RA_, && case_sem_INSN_MULLW_L_S12__RA_ },
    { ARC600F_INSN_MULLW_CCU6__RA_, && case_sem_INSN_MULLW_CCU6__RA_ },
    { ARC600F_INSN_MULLW_L_U6__RA_, && case_sem_INSN_MULLW_L_U6__RA_ },
    { ARC600F_INSN_MULLW_L_R_R__RA__RC, && case_sem_INSN_MULLW_L_R_R__RA__RC },
    { ARC600F_INSN_MULLW_CC__RA__RC, && case_sem_INSN_MULLW_CC__RA__RC },
    { ARC600F_INSN_MACLW_L_S12__RA_, && case_sem_INSN_MACLW_L_S12__RA_ },
    { ARC600F_INSN_MACLW_CCU6__RA_, && case_sem_INSN_MACLW_CCU6__RA_ },
    { ARC600F_INSN_MACLW_L_U6__RA_, && case_sem_INSN_MACLW_L_U6__RA_ },
    { ARC600F_INSN_MACLW_L_R_R__RA__RC, && case_sem_INSN_MACLW_L_R_R__RA__RC },
    { ARC600F_INSN_MACLW_CC__RA__RC, && case_sem_INSN_MACLW_CC__RA__RC },
    { ARC600F_INSN_MACHLW_L_S12__RA_, && case_sem_INSN_MACHLW_L_S12__RA_ },
    { ARC600F_INSN_MACHLW_CCU6__RA_, && case_sem_INSN_MACHLW_CCU6__RA_ },
    { ARC600F_INSN_MACHLW_L_U6__RA_, && case_sem_INSN_MACHLW_L_U6__RA_ },
    { ARC600F_INSN_MACHLW_L_R_R__RA__RC, && case_sem_INSN_MACHLW_L_R_R__RA__RC },
    { ARC600F_INSN_MACHLW_CC__RA__RC, && case_sem_INSN_MACHLW_CC__RA__RC },
    { ARC600F_INSN_MULULW_L_S12__RA_, && case_sem_INSN_MULULW_L_S12__RA_ },
    { ARC600F_INSN_MULULW_CCU6__RA_, && case_sem_INSN_MULULW_CCU6__RA_ },
    { ARC600F_INSN_MULULW_L_U6__RA_, && case_sem_INSN_MULULW_L_U6__RA_ },
    { ARC600F_INSN_MULULW_L_R_R__RA__RC, && case_sem_INSN_MULULW_L_R_R__RA__RC },
    { ARC600F_INSN_MULULW_CC__RA__RC, && case_sem_INSN_MULULW_CC__RA__RC },
    { ARC600F_INSN_MACHULW_L_S12__RA_, && case_sem_INSN_MACHULW_L_S12__RA_ },
    { ARC600F_INSN_MACHULW_CCU6__RA_, && case_sem_INSN_MACHULW_CCU6__RA_ },
    { ARC600F_INSN_MACHULW_L_U6__RA_, && case_sem_INSN_MACHULW_L_U6__RA_ },
    { ARC600F_INSN_MACHULW_L_R_R__RA__RC, && case_sem_INSN_MACHULW_L_R_R__RA__RC },
    { ARC600F_INSN_MACHULW_CC__RA__RC, && case_sem_INSN_MACHULW_CC__RA__RC },
    { ARC600F_INSN_CURRENT_LOOP_END, && case_sem_INSN_CURRENT_LOOP_END },
    { ARC600F_INSN_CURRENT_LOOP_END_AFTER_BRANCH, && case_sem_INSN_CURRENT_LOOP_END_AFTER_BRANCH },
    { ARC600F_INSN_ARC600_CURRENT_LOOP_END_AFTER_BRANCH, && case_sem_INSN_ARC600_CURRENT_LOOP_END_AFTER_BRANCH },
    { 0, 0 }
  };
  int i;

  for (i = 0; labels[i].label != 0; ++i)
    {
#if FAST_P
      CPU_IDESC (current_cpu) [labels[i].index].sem_fast_lab = labels[i].label;
#else
      CPU_IDESC (current_cpu) [labels[i].index].sem_full_lab = labels[i].label;
#endif
    }

#undef DEFINE_LABELS
#endif /* DEFINE_LABELS */

#ifdef DEFINE_SWITCH

/* If hyper-fast [well not unnecessarily slow] execution is selected, turn
   off frills like tracing and profiling.  */
/* FIXME: A better way would be to have TRACE_RESULT check for something
   that can cause it to be optimized out.  Another way would be to emit
   special handlers into the instruction "stream".  */

#if FAST_P
#undef TRACE_RESULT
#define TRACE_RESULT(cpu, abuf, name, type, val)
#endif

#undef GET_ATTR
#if defined (__STDC__) || defined (ALMOST_STDC) || defined (HAVE_STRINGIZE)
#define GET_ATTR(cpu, num, attr) CGEN_ATTR_VALUE (NULL, abuf->idesc->attrs, CGEN_INSN_##attr)
#else
#define GET_ATTR(cpu, num, attr) CGEN_ATTR_VALUE (NULL, abuf->idesc->attrs, CGEN_INSN_/**/attr)
#endif

{

#if WITH_SCACHE_PBB

/* Branch to next handler without going around main loop.  */
#define NEXT(vpc) goto * SEM_ARGBUF (vpc) -> semantic.sem_case
SWITCH (sem, SEM_ARGBUF (vpc) -> semantic.sem_case)

#else /* ! WITH_SCACHE_PBB */

#define NEXT(vpc) BREAK (sem)
#ifdef __GNUC__
#if FAST_P
  SWITCH (sem, SEM_ARGBUF (sc) -> idesc->sem_fast_lab)
#else
  SWITCH (sem, SEM_ARGBUF (sc) -> idesc->sem_full_lab)
#endif
#else
  SWITCH (sem, SEM_ARGBUF (sc) -> idesc->num)
#endif

#endif /* ! WITH_SCACHE_PBB */

    {

  CASE (sem, INSN_X_INVALID) : /* --invalid-- */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.fmt_empty.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 0);

  {
    /* Update the recorded pc in the cpu state struct.
       Only necessary for WITH_SCACHE case, but to avoid the
       conditional compilation ....  */
    SET_H_PC (pc);
    /* Virtual insns have zero size.  Overwrite vpc with address of next insn
       using the default-insn-bitsize spec.  When executing insns in parallel
       we may want to queue the fault and continue execution.  */
    vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
    vpc = sim_engine_invalid_insn (current_cpu, pc, vpc);
  }

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_X_AFTER) : /* --after-- */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.fmt_empty.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 0);

  {
#if WITH_SCACHE_PBB_ARC600F
    arc600f_pbb_after (current_cpu, sem_arg);
#endif
  }

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_X_BEFORE) : /* --before-- */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.fmt_empty.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 0);

  {
#if WITH_SCACHE_PBB_ARC600F
    arc600f_pbb_before (current_cpu, sem_arg);
#endif
  }

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_X_CTI_CHAIN) : /* --cti-chain-- */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.fmt_empty.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 0);

  {
#if WITH_SCACHE_PBB_ARC600F
#ifdef DEFINE_SWITCH
    vpc = arc600f_pbb_cti_chain (current_cpu, sem_arg,
			       pbb_br_type, pbb_br_npc);
    BREAK (sem);
#else
    /* FIXME: Allow provision of explicit ifmt spec in insn spec.  */
    vpc = arc600f_pbb_cti_chain (current_cpu, sem_arg,
			       CPU_PBB_BR_TYPE (current_cpu),
			       CPU_PBB_BR_NPC (current_cpu));
#endif
#endif
  }

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_X_CHAIN) : /* --chain-- */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.fmt_empty.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 0);

  {
#if WITH_SCACHE_PBB_ARC600F
    vpc = arc600f_pbb_chain (current_cpu, sem_arg);
#ifdef DEFINE_SWITCH
    BREAK (sem);
#endif
#endif
  }

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_X_BEGIN) : /* --begin-- */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.fmt_empty.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 0);

  {
#if WITH_SCACHE_PBB_ARC600F
#if defined DEFINE_SWITCH || defined FAST_P
    /* In the switch case FAST_P is a constant, allowing several optimizations
       in any called inline functions.  */
    vpc = arc600f_pbb_begin (current_cpu, FAST_P);
#else
#if 0 /* cgen engine can't handle dynamic fast/full switching yet.  */
    vpc = arc600f_pbb_begin (current_cpu, STATE_RUN_FAST_P (CPU_STATE (current_cpu)));
#else
    vpc = arc600f_pbb_begin (current_cpu, 0);
#endif
#endif
#endif
  }

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_B_S) : /* b$i2cond $label10 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_b_s.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (GET_H_I2COND (FLD (f_cond_i2))) {
if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
  {
    USI opval = FLD (i_label10);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
 else if (0) {
  {
    USI opval = FLD (i_label10);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
  {
    USI opval = FLD (i_label10);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BCC_S) : /* b$i3cond$_S $label7 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_bcc_s.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (GET_H_I3COND (FLD (f_cond_i3))) {
if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
  {
    USI opval = FLD (i_label7);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
 else if (0) {
  {
    USI opval = FLD (i_label7);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
  {
    USI opval = FLD (i_label7);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BRCC_S) : /* br$RccS$_S $R_b,0,$label8 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_brcc_s.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if ((FLD (f_brscond) == 0) ? (EQSI (GET_H_CR16 (FLD (f_op__b)), 0)) : (FLD (f_brscond) == 1) ? (NESI (GET_H_CR16 (FLD (f_op__b)), 0)) : ((cgen_rtx_error (current_cpu, "unreachable - put in because of parser error"), 0))) {
if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
  {
    USI opval = FLD (i_label8);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
 else if (0) {
  {
    USI opval = FLD (i_label8);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
  {
    USI opval = FLD (i_label8);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BCC_L) : /* b$Qcondb$_L $label21 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_bcc_l.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
((void) 0); /*nop*/
if (GET_H_QCONDB (FLD (f_cond_Q))) {
if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
  {
    USI opval = FLD (i_label21);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
 else if (0) {
  {
    USI opval = FLD (i_label21);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
  {
    USI opval = FLD (i_label21);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BCC_L_D) : /* b$Qcondb$_L.d $label21 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_bcc_l.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
((void) 0); /*nop*/
if (GET_H_QCONDB (FLD (f_cond_Q))) {
if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
{
  {
    USI opval = FLD (i_label21);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
 else if (1) {
{
  {
    USI opval = FLD (i_label21);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
{
  {
    USI opval = FLD (i_label21);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_B_L) : /* b$uncondb$_L $label25 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_b_l.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
  {
    USI opval = FLD (i_label25);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
 else if (0) {
  {
    USI opval = FLD (i_label25);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
  {
    USI opval = FLD (i_label25);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_B_L_D) : /* b$uncondb$_L.d $label25 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_b_l.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
{
  {
    USI opval = FLD (i_label25);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
 else if (1) {
{
  {
    USI opval = FLD (i_label25);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
{
  {
    USI opval = FLD (i_label25);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BRCC_RC) : /* b$Rcc $RB,$RC,$label9 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_brcc_RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 21);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
{
  SI tmp_condition;
  SI tmp_B;
  SI tmp_C;
  tmp_condition = FLD (f_brcond);
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_B = GET_H_CR (FLD (f_op_B));
  tmp_C = GET_H_CR (FLD (f_op_C));
if ((tmp_condition == CONDBR_REQ) ? (EQSI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RNE) ? (NESI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RLT) ? (LTSI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RGE) ? (GESI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RLO) ? (LTUSI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RHS) ? (GEUSI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_BIT0) ? (EQSI (ANDSI (tmp_B, SLLSI (1, tmp_C)), 0)) : (tmp_condition == CONDBR_BIT1) ? (NESI (ANDSI (tmp_B, SLLSI (1, tmp_C)), 0)) : ((cgen_rtx_error (current_cpu, "unreachable - put in because of parser error"), 0))) {
  {
    USI opval = FLD (i_label9);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 22);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}
 else if (0) {
{
  SI tmp_condition;
  SI tmp_B;
  SI tmp_C;
  tmp_condition = FLD (f_brcond);
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_B = GET_H_CR (FLD (f_op_B));
  tmp_C = GET_H_CR (FLD (f_op_C));
if ((tmp_condition == CONDBR_REQ) ? (EQSI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RNE) ? (NESI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RLT) ? (LTSI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RGE) ? (GESI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RLO) ? (LTUSI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RHS) ? (GEUSI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_BIT0) ? (EQSI (ANDSI (tmp_B, SLLSI (1, tmp_C)), 0)) : (tmp_condition == CONDBR_BIT1) ? (NESI (ANDSI (tmp_B, SLLSI (1, tmp_C)), 0)) : ((cgen_rtx_error (current_cpu, "unreachable - put in because of parser error"), 0))) {
  {
    USI opval = FLD (i_label9);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 22);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 20);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 22);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
{
  SI tmp_condition;
  SI tmp_B;
  SI tmp_C;
  tmp_condition = FLD (f_brcond);
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_B = GET_H_CR (FLD (f_op_B));
  tmp_C = GET_H_CR (FLD (f_op_C));
if ((tmp_condition == CONDBR_REQ) ? (EQSI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RNE) ? (NESI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RLT) ? (LTSI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RGE) ? (GESI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RLO) ? (LTUSI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RHS) ? (GEUSI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_BIT0) ? (EQSI (ANDSI (tmp_B, SLLSI (1, tmp_C)), 0)) : (tmp_condition == CONDBR_BIT1) ? (NESI (ANDSI (tmp_B, SLLSI (1, tmp_C)), 0)) : ((cgen_rtx_error (current_cpu, "unreachable - put in because of parser error"), 0))) {
  {
    USI opval = FLD (i_label9);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 22);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BRCC_RC_D) : /* b$Rcc.d $RB,$RC,$label9 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_brcc_RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 21);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
{
  SI tmp_condition;
  SI tmp_B;
  SI tmp_C;
  tmp_condition = FLD (f_brcond);
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_B = GET_H_CR (FLD (f_op_B));
  tmp_C = GET_H_CR (FLD (f_op_C));
if ((tmp_condition == CONDBR_REQ) ? (EQSI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RNE) ? (NESI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RLT) ? (LTSI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RGE) ? (GESI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RLO) ? (LTUSI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RHS) ? (GEUSI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_BIT0) ? (EQSI (ANDSI (tmp_B, SLLSI (1, tmp_C)), 0)) : (tmp_condition == CONDBR_BIT1) ? (NESI (ANDSI (tmp_B, SLLSI (1, tmp_C)), 0)) : ((cgen_rtx_error (current_cpu, "unreachable - put in because of parser error"), 0))) {
{
  {
    USI opval = FLD (i_label9);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 22);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}
}
 else if (1) {
{
  SI tmp_condition;
  SI tmp_B;
  SI tmp_C;
  tmp_condition = FLD (f_brcond);
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_B = GET_H_CR (FLD (f_op_B));
  tmp_C = GET_H_CR (FLD (f_op_C));
if ((tmp_condition == CONDBR_REQ) ? (EQSI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RNE) ? (NESI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RLT) ? (LTSI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RGE) ? (GESI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RLO) ? (LTUSI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RHS) ? (GEUSI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_BIT0) ? (EQSI (ANDSI (tmp_B, SLLSI (1, tmp_C)), 0)) : (tmp_condition == CONDBR_BIT1) ? (NESI (ANDSI (tmp_B, SLLSI (1, tmp_C)), 0)) : ((cgen_rtx_error (current_cpu, "unreachable - put in because of parser error"), 0))) {
{
  {
    USI opval = FLD (i_label9);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 22);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 20);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 22);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
{
  SI tmp_condition;
  SI tmp_B;
  SI tmp_C;
  tmp_condition = FLD (f_brcond);
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_B = GET_H_CR (FLD (f_op_B));
  tmp_C = GET_H_CR (FLD (f_op_C));
if ((tmp_condition == CONDBR_REQ) ? (EQSI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RNE) ? (NESI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RLT) ? (LTSI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RGE) ? (GESI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RLO) ? (LTUSI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RHS) ? (GEUSI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_BIT0) ? (EQSI (ANDSI (tmp_B, SLLSI (1, tmp_C)), 0)) : (tmp_condition == CONDBR_BIT1) ? (NESI (ANDSI (tmp_B, SLLSI (1, tmp_C)), 0)) : ((cgen_rtx_error (current_cpu, "unreachable - put in because of parser error"), 0))) {
{
  {
    USI opval = FLD (i_label9);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 22);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BRCC_U6) : /* b$Rcc $RB,$U6,$label9 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_brcc_U6.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
{
  SI tmp_condition;
  SI tmp_B;
  SI tmp_C;
  tmp_condition = FLD (f_brcond);
((void) 0); /*nop*/
  tmp_B = GET_H_CR (FLD (f_op_B));
  tmp_C = FLD (f_u6);
if ((tmp_condition == CONDBR_REQ) ? (EQSI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RNE) ? (NESI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RLT) ? (LTSI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RGE) ? (GESI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RLO) ? (LTUSI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RHS) ? (GEUSI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_BIT0) ? (EQSI (ANDSI (tmp_B, SLLSI (1, tmp_C)), 0)) : (tmp_condition == CONDBR_BIT1) ? (NESI (ANDSI (tmp_B, SLLSI (1, tmp_C)), 0)) : ((cgen_rtx_error (current_cpu, "unreachable - put in because of parser error"), 0))) {
  {
    USI opval = FLD (i_label9);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}
 else if (0) {
{
  SI tmp_condition;
  SI tmp_B;
  SI tmp_C;
  tmp_condition = FLD (f_brcond);
((void) 0); /*nop*/
  tmp_B = GET_H_CR (FLD (f_op_B));
  tmp_C = FLD (f_u6);
if ((tmp_condition == CONDBR_REQ) ? (EQSI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RNE) ? (NESI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RLT) ? (LTSI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RGE) ? (GESI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RLO) ? (LTUSI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RHS) ? (GEUSI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_BIT0) ? (EQSI (ANDSI (tmp_B, SLLSI (1, tmp_C)), 0)) : (tmp_condition == CONDBR_BIT1) ? (NESI (ANDSI (tmp_B, SLLSI (1, tmp_C)), 0)) : ((cgen_rtx_error (current_cpu, "unreachable - put in because of parser error"), 0))) {
  {
    USI opval = FLD (i_label9);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
{
  SI tmp_condition;
  SI tmp_B;
  SI tmp_C;
  tmp_condition = FLD (f_brcond);
((void) 0); /*nop*/
  tmp_B = GET_H_CR (FLD (f_op_B));
  tmp_C = FLD (f_u6);
if ((tmp_condition == CONDBR_REQ) ? (EQSI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RNE) ? (NESI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RLT) ? (LTSI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RGE) ? (GESI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RLO) ? (LTUSI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RHS) ? (GEUSI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_BIT0) ? (EQSI (ANDSI (tmp_B, SLLSI (1, tmp_C)), 0)) : (tmp_condition == CONDBR_BIT1) ? (NESI (ANDSI (tmp_B, SLLSI (1, tmp_C)), 0)) : ((cgen_rtx_error (current_cpu, "unreachable - put in because of parser error"), 0))) {
  {
    USI opval = FLD (i_label9);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BRCC_U6_D) : /* b$Rcc.d $RB,$U6,$label9 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_brcc_U6.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
{
  SI tmp_condition;
  SI tmp_B;
  SI tmp_C;
  tmp_condition = FLD (f_brcond);
((void) 0); /*nop*/
  tmp_B = GET_H_CR (FLD (f_op_B));
  tmp_C = FLD (f_u6);
if ((tmp_condition == CONDBR_REQ) ? (EQSI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RNE) ? (NESI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RLT) ? (LTSI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RGE) ? (GESI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RLO) ? (LTUSI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RHS) ? (GEUSI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_BIT0) ? (EQSI (ANDSI (tmp_B, SLLSI (1, tmp_C)), 0)) : (tmp_condition == CONDBR_BIT1) ? (NESI (ANDSI (tmp_B, SLLSI (1, tmp_C)), 0)) : ((cgen_rtx_error (current_cpu, "unreachable - put in because of parser error"), 0))) {
{
  {
    USI opval = FLD (i_label9);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}
}
 else if (1) {
{
  SI tmp_condition;
  SI tmp_B;
  SI tmp_C;
  tmp_condition = FLD (f_brcond);
((void) 0); /*nop*/
  tmp_B = GET_H_CR (FLD (f_op_B));
  tmp_C = FLD (f_u6);
if ((tmp_condition == CONDBR_REQ) ? (EQSI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RNE) ? (NESI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RLT) ? (LTSI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RGE) ? (GESI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RLO) ? (LTUSI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RHS) ? (GEUSI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_BIT0) ? (EQSI (ANDSI (tmp_B, SLLSI (1, tmp_C)), 0)) : (tmp_condition == CONDBR_BIT1) ? (NESI (ANDSI (tmp_B, SLLSI (1, tmp_C)), 0)) : ((cgen_rtx_error (current_cpu, "unreachable - put in because of parser error"), 0))) {
{
  {
    USI opval = FLD (i_label9);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
{
  SI tmp_condition;
  SI tmp_B;
  SI tmp_C;
  tmp_condition = FLD (f_brcond);
((void) 0); /*nop*/
  tmp_B = GET_H_CR (FLD (f_op_B));
  tmp_C = FLD (f_u6);
if ((tmp_condition == CONDBR_REQ) ? (EQSI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RNE) ? (NESI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RLT) ? (LTSI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RGE) ? (GESI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RLO) ? (LTUSI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_RHS) ? (GEUSI (tmp_B, tmp_C)) : (tmp_condition == CONDBR_BIT0) ? (EQSI (ANDSI (tmp_B, SLLSI (1, tmp_C)), 0)) : (tmp_condition == CONDBR_BIT1) ? (NESI (ANDSI (tmp_B, SLLSI (1, tmp_C)), 0)) : ((cgen_rtx_error (current_cpu, "unreachable - put in because of parser error"), 0))) {
{
  {
    USI opval = FLD (i_label9);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BL_S) : /* bl$uncondj$_S $label13a */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_bl_s.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
{
  {
    SI opval = ADDSI (pc, 2);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    USI opval = FLD (i_label13a);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
 else if (0) {
{
  {
    SI opval = ADDSI (pc, 2);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    USI opval = FLD (i_label13a);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
{
  {
    SI opval = ADDSI (pc, 2);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    USI opval = FLD (i_label13a);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BLCC) : /* bl$Qcondj$_L $label21 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_blcc.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
((void) 0); /*nop*/
if (GET_H_QCONDB (FLD (f_cond_Q))) {
if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
{
  {
    SI opval = ADDSI (pc, 4);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    USI opval = FLD (i_label21a);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
 else if (0) {
{
  {
    SI opval = ADDSI (pc, 4);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    USI opval = FLD (i_label21a);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
{
  {
    SI opval = ADDSI (pc, 4);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    USI opval = FLD (i_label21a);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BLCC_D) : /* bl$Qcondj$_L.d $label21 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_blcc.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
((void) 0); /*nop*/
if (GET_H_QCONDB (FLD (f_cond_Q))) {
if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
{
  {
    SI opval = ADDSI (pc, 8);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
{
  {
    USI opval = FLD (i_label21a);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}
 else if (1) {
{
  {
    SI opval = ADDSI (pc, 8);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
{
  {
    USI opval = FLD (i_label21a);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
{
  {
    SI opval = ADDSI (pc, 8);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
{
  {
    USI opval = FLD (i_label21a);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BL) : /* bl$uncondj$_L $label25a */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_bl.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
{
  {
    SI opval = ADDSI (pc, 4);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    USI opval = FLD (i_label25a);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
 else if (0) {
{
  {
    SI opval = ADDSI (pc, 4);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    USI opval = FLD (i_label25a);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
{
  {
    SI opval = ADDSI (pc, 4);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    USI opval = FLD (i_label25a);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BL_D) : /* bl$uncondj$_L.d $label25a */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_bl.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
{
  HI tmp_nword;
  tmp_nword = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
if (ANDHI (ANDHI (tmp_nword, SRAHI (tmp_nword, 1)), 40960)) {
  {
    SI opval = ADDSI (pc, 6);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
} else {
  {
    SI opval = ADDSI (pc, 8);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
{
  {
    USI opval = FLD (i_label25a);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}
 else if (1) {
{
  HI tmp_nword;
  tmp_nword = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
if (ANDHI (ANDHI (tmp_nword, SRAHI (tmp_nword, 1)), 40960)) {
  {
    SI opval = ADDSI (pc, 6);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
} else {
  {
    SI opval = ADDSI (pc, 8);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
{
  {
    USI opval = FLD (i_label25a);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
{
  HI tmp_nword;
  tmp_nword = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
if (ANDHI (ANDHI (tmp_nword, SRAHI (tmp_nword, 1)), 40960)) {
  {
    SI opval = ADDSI (pc, 6);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
} else {
  {
    SI opval = ADDSI (pc, 8);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
{
  {
    USI opval = FLD (i_label25a);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LD_ABS) : /* ld$LDODi $RA,[$RB,$s9] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ld_abs.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR (FLD (f_op_B)), FLD (f_s9));
  {
    SI opval = GETMEMSI (current_cpu, pc, tmp_eaddr);
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LD__AW_ABS) : /* ld$_AW$LDODi $RA,[$RB,$s9] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ld_abs.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR (FLD (f_op_B)), FLD (f_s9));
  {
    SI opval = tmp_eaddr;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, tmp_eaddr);
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LD_AB_ABS) : /* ld.ab$LDODi $RA,[$RB,$s9] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ld_abs.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_sum;
  SI tmp_eaddr;
  tmp_sum = ADDSI (GET_H_CR (FLD (f_op_B)), FLD (f_s9));
  tmp_eaddr = GET_H_CR (FLD (f_op_B));
  {
    SI opval = GETMEMSI (current_cpu, pc, tmp_eaddr);
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = tmp_sum;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LD_AS_ABS) : /* ld.as$LDODi $RA,[$RB,$s9] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ld_abs.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR (FLD (f_op_B)), SLLSI (FLD (f_s9), 2));
  {
    SI opval = GETMEMSI (current_cpu, pc, tmp_eaddr);
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LD_ABC) : /* ld$LDRDi $RA,[$RB,$RC] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)));
  {
    SI opval = GETMEMSI (current_cpu, pc, tmp_eaddr);
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LD__AW_ABC) : /* ld$_AW$LDRDi $RA,[$RB,$RC] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)));
  {
    SI opval = tmp_eaddr;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, tmp_eaddr);
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LD_AB_ABC) : /* ld.ab$LDRDi $RA,[$RB,$RC] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_sum;
  SI tmp_eaddr;
  tmp_sum = ADDSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)));
  tmp_eaddr = GET_H_CR (FLD (f_op_B));
  {
    SI opval = GETMEMSI (current_cpu, pc, tmp_eaddr);
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = tmp_sum;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LD_AS_ABC) : /* ld.as$LDRDi $RA,[$RB,$RC] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR (FLD (f_op_B)), SLLSI (GET_H_CR (FLD (f_op_C)), 2));
  {
    SI opval = GETMEMSI (current_cpu, pc, tmp_eaddr);
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LD_S_ABC) : /* ld$_S $R_a,[$R_b,$R_c] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ld_s_abc.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR16 (FLD (f_op__b)), GET_H_CR16 (FLD (f_op__c)));
  {
    SI opval = GETMEMSI (current_cpu, pc, tmp_eaddr);
    SET_H_CR16 (FLD (f_op__a), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LD_S_ABU) : /* ld$_S $R_c,[$R_b,$sc_u5_] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ld_s_abu.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR16 (FLD (f_op__b)), FLD (f_u5x4));
  {
    SI opval = GETMEMSI (current_cpu, pc, tmp_eaddr);
    SET_H_CR16 (FLD (f_op__c), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LD_S_ABSP) : /* ld$_S $R_b,[$SP,$u5x4] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ld_s_abu.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_SP (), FLD (f_u5x4));
  {
    SI opval = GETMEMSI (current_cpu, pc, tmp_eaddr);
    SET_H_CR16 (FLD (f_op__b), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LD_S_GPREL) : /* ld$_S $R_b,[$GP,$sc_s9_] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ld_s_gprel.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_GP (), FLD (f_s9x4));
  {
    SI opval = GETMEMSI (current_cpu, pc, tmp_eaddr);
    SET_H_R0 (, opval);
    TRACE_RESULT (current_cpu, abuf, "r0", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LD_S_PCREL) : /* ld$_S $R_b,[$PCL,$u8x4] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ld_s_pcrel.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (ANDSI (pc, -4), FLD (f_u8x4));
  {
    SI opval = GETMEMSI (current_cpu, pc, tmp_eaddr);
    SET_H_CR16 (FLD (f_op__b), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LDB_ABS) : /* ldb$LDODi $RA,[$RB,$s9] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ld_abs.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR (FLD (f_op_B)), FLD (f_s9));
  {
    SI opval = ZEXTQISI (GETMEMQI (current_cpu, pc, tmp_eaddr));
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LDB__AW_ABS) : /* ldb$_AW$LDODi $RA,[$RB,$s9] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ld_abs.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR (FLD (f_op_B)), FLD (f_s9));
  {
    SI opval = tmp_eaddr;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = ZEXTQISI (GETMEMQI (current_cpu, pc, tmp_eaddr));
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LDB_AB_ABS) : /* ldb.ab$LDODi $RA,[$RB,$s9] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ld_abs.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_sum;
  SI tmp_eaddr;
  tmp_sum = ADDSI (GET_H_CR (FLD (f_op_B)), FLD (f_s9));
  tmp_eaddr = GET_H_CR (FLD (f_op_B));
  {
    SI opval = ZEXTQISI (GETMEMQI (current_cpu, pc, tmp_eaddr));
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = tmp_sum;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LDB_AS_ABS) : /* ldb.as$LDODi $RA,[$RB,$s9] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ld_abs.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR (FLD (f_op_B)), (cgen_rtx_error (current_cpu, "invalid insn"), 0));
  {
    SI opval = ZEXTQISI (GETMEMQI (current_cpu, pc, tmp_eaddr));
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LDB_ABC) : /* ldb$LDRDi $RA,[$RB,$RC] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)));
  {
    SI opval = ZEXTQISI (GETMEMQI (current_cpu, pc, tmp_eaddr));
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LDB__AW_ABC) : /* ldb$_AW$LDRDi $RA,[$RB,$RC] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)));
  {
    SI opval = tmp_eaddr;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = ZEXTQISI (GETMEMQI (current_cpu, pc, tmp_eaddr));
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LDB_AB_ABC) : /* ldb.ab$LDRDi $RA,[$RB,$RC] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_sum;
  SI tmp_eaddr;
  tmp_sum = ADDSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)));
  tmp_eaddr = GET_H_CR (FLD (f_op_B));
  {
    SI opval = ZEXTQISI (GETMEMQI (current_cpu, pc, tmp_eaddr));
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = tmp_sum;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LDB_AS_ABC) : /* ldb.as$LDRDi $RA,[$RB,$RC] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR (FLD (f_op_B)), (cgen_rtx_error (current_cpu, "invalid insn"), 0));
  {
    SI opval = ZEXTQISI (GETMEMQI (current_cpu, pc, tmp_eaddr));
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LDB_S_ABC) : /* ldb$_S $R_a,[$R_b,$R_c] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ld_s_abc.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR16 (FLD (f_op__b)), GET_H_CR16 (FLD (f_op__c)));
  {
    SI opval = ZEXTQISI (GETMEMQI (current_cpu, pc, tmp_eaddr));
    SET_H_CR16 (FLD (f_op__a), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LDB_S_ABU) : /* ldb$_S $R_c,[$R_b,$sc_u5b] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ldb_s_abu.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR16 (FLD (f_op__b)), FLD (f_u5));
  {
    SI opval = ZEXTQISI (GETMEMQI (current_cpu, pc, tmp_eaddr));
    SET_H_CR16 (FLD (f_op__c), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LDB_S_ABSP) : /* ldb$_S $R_b,[$SP,$u5x4] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ld_s_abu.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_SP (), FLD (f_u5x4));
  {
    SI opval = ZEXTQISI (GETMEMQI (current_cpu, pc, tmp_eaddr));
    SET_H_CR16 (FLD (f_op__b), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LDB_S_GPREL) : /* ldb$_S $R_b,[$GP,$sc_s9b] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ldb_s_gprel.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_GP (), FLD (f_s9x1));
  {
    SI opval = ZEXTQISI (GETMEMQI (current_cpu, pc, tmp_eaddr));
    SET_H_R0 (, opval);
    TRACE_RESULT (current_cpu, abuf, "r0", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LDB_X_ABS) : /* ldb.x$LDODi $RA,[$RB,$s9] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ld_abs.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR (FLD (f_op_B)), FLD (f_s9));
  {
    SI opval = EXTQISI (GETMEMQI (current_cpu, pc, tmp_eaddr));
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LDB__AW_X_ABS) : /* ldb$_AW.x$LDODi $RA,[$RB,$s9] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ld_abs.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR (FLD (f_op_B)), FLD (f_s9));
  {
    SI opval = tmp_eaddr;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = EXTQISI (GETMEMQI (current_cpu, pc, tmp_eaddr));
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LDB_AB_X_ABS) : /* ldb.ab.x$LDODi $RA,[$RB,$s9] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ld_abs.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_sum;
  SI tmp_eaddr;
  tmp_sum = ADDSI (GET_H_CR (FLD (f_op_B)), FLD (f_s9));
  tmp_eaddr = GET_H_CR (FLD (f_op_B));
  {
    SI opval = EXTQISI (GETMEMQI (current_cpu, pc, tmp_eaddr));
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = tmp_sum;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LDB_AS_X_ABS) : /* ldb.as.x$LDODi $RA,[$RB,$s9] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ld_abs.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR (FLD (f_op_B)), (cgen_rtx_error (current_cpu, "invalid insn"), 0));
  {
    SI opval = EXTQISI (GETMEMQI (current_cpu, pc, tmp_eaddr));
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LDB_X_ABC) : /* ldb.x$LDRDi $RA,[$RB,$RC] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)));
  {
    SI opval = EXTQISI (GETMEMQI (current_cpu, pc, tmp_eaddr));
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LDB__AW_X_ABC) : /* ldb$_AW.x$LDRDi $RA,[$RB,$RC] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)));
  {
    SI opval = tmp_eaddr;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = EXTQISI (GETMEMQI (current_cpu, pc, tmp_eaddr));
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LDB_AB_X_ABC) : /* ldb.ab.x$LDRDi $RA,[$RB,$RC] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_sum;
  SI tmp_eaddr;
  tmp_sum = ADDSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)));
  tmp_eaddr = GET_H_CR (FLD (f_op_B));
  {
    SI opval = EXTQISI (GETMEMQI (current_cpu, pc, tmp_eaddr));
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = tmp_sum;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LDB_AS_X_ABC) : /* ldb.as.x$LDRDi $RA,[$RB,$RC] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR (FLD (f_op_B)), (cgen_rtx_error (current_cpu, "invalid insn"), 0));
  {
    SI opval = EXTQISI (GETMEMQI (current_cpu, pc, tmp_eaddr));
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LDW_ABS) : /* ldw$LDODi $RA,[$RB,$s9] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ld_abs.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR (FLD (f_op_B)), FLD (f_s9));
  {
    SI opval = ZEXTHISI (GETMEMHI (current_cpu, pc, tmp_eaddr));
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LDW__AW_ABS) : /* ldw$_AW$LDODi $RA,[$RB,$s9] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ld_abs.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR (FLD (f_op_B)), FLD (f_s9));
  {
    SI opval = tmp_eaddr;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = ZEXTHISI (GETMEMHI (current_cpu, pc, tmp_eaddr));
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LDW_AB_ABS) : /* ldw.ab$LDODi $RA,[$RB,$s9] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ld_abs.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_sum;
  SI tmp_eaddr;
  tmp_sum = ADDSI (GET_H_CR (FLD (f_op_B)), FLD (f_s9));
  tmp_eaddr = GET_H_CR (FLD (f_op_B));
  {
    SI opval = ZEXTHISI (GETMEMHI (current_cpu, pc, tmp_eaddr));
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = tmp_sum;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LDW_AS_ABS) : /* ldw.as$LDODi $RA,[$RB,$s9] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ld_abs.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR (FLD (f_op_B)), SLLSI (FLD (f_s9), 1));
  {
    SI opval = ZEXTHISI (GETMEMHI (current_cpu, pc, tmp_eaddr));
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LDW_ABC) : /* ldw$LDRDi $RA,[$RB,$RC] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)));
  {
    SI opval = ZEXTHISI (GETMEMHI (current_cpu, pc, tmp_eaddr));
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LDW__AW_ABC) : /* ldw$_AW$LDRDi $RA,[$RB,$RC] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)));
  {
    SI opval = tmp_eaddr;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = ZEXTHISI (GETMEMHI (current_cpu, pc, tmp_eaddr));
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LDW_AB_ABC) : /* ldw.ab$LDRDi $RA,[$RB,$RC] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_sum;
  SI tmp_eaddr;
  tmp_sum = ADDSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)));
  tmp_eaddr = GET_H_CR (FLD (f_op_B));
  {
    SI opval = ZEXTHISI (GETMEMHI (current_cpu, pc, tmp_eaddr));
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = tmp_sum;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LDW_AS_ABC) : /* ldw.as$LDRDi $RA,[$RB,$RC] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR (FLD (f_op_B)), SLLSI (GET_H_CR (FLD (f_op_C)), 1));
  {
    SI opval = ZEXTHISI (GETMEMHI (current_cpu, pc, tmp_eaddr));
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LDW_S_ABC) : /* ldw$_S $R_a,[$R_b,$R_c] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ld_s_abc.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR16 (FLD (f_op__b)), GET_H_CR16 (FLD (f_op__c)));
  {
    SI opval = ZEXTHISI (GETMEMHI (current_cpu, pc, tmp_eaddr));
    SET_H_CR16 (FLD (f_op__a), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LDW_S_ABU) : /* ldw$_S $R_c,[$R_b,$sc_u5w] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ldw_s_abu.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR16 (FLD (f_op__b)), FLD (f_u5x2));
  {
    SI opval = ZEXTHISI (GETMEMHI (current_cpu, pc, tmp_eaddr));
    SET_H_CR16 (FLD (f_op__c), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LDW_S_GPREL) : /* ldw$_S $R_b,[$GP,$sc_s9w] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ldw_s_gprel.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_GP (), FLD (f_s9x2));
  {
    SI opval = ZEXTHISI (GETMEMHI (current_cpu, pc, tmp_eaddr));
    SET_H_R0 (, opval);
    TRACE_RESULT (current_cpu, abuf, "r0", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LDW_X_ABS) : /* ldw.x$LDODi $RA,[$RB,$s9] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ld_abs.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR (FLD (f_op_B)), FLD (f_s9));
  {
    SI opval = EXTHISI (GETMEMHI (current_cpu, pc, tmp_eaddr));
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LDW__AW_X_ABS) : /* ldw$_AW.x$LDODi $RA,[$RB,$s9] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ld_abs.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR (FLD (f_op_B)), FLD (f_s9));
  {
    SI opval = tmp_eaddr;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = EXTHISI (GETMEMHI (current_cpu, pc, tmp_eaddr));
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LDW_AB_X_ABS) : /* ldw.ab.x$LDODi $RA,[$RB,$s9] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ld_abs.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_sum;
  SI tmp_eaddr;
  tmp_sum = ADDSI (GET_H_CR (FLD (f_op_B)), FLD (f_s9));
  tmp_eaddr = GET_H_CR (FLD (f_op_B));
  {
    SI opval = EXTHISI (GETMEMHI (current_cpu, pc, tmp_eaddr));
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = tmp_sum;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LDW_AS_X_ABS) : /* ldw.as.x$LDODi $RA,[$RB,$s9] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ld_abs.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR (FLD (f_op_B)), SLLSI (FLD (f_s9), 1));
  {
    SI opval = EXTHISI (GETMEMHI (current_cpu, pc, tmp_eaddr));
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LDW_X_ABC) : /* ldw.x$LDRDi $RA,[$RB,$RC] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)));
  {
    SI opval = EXTHISI (GETMEMHI (current_cpu, pc, tmp_eaddr));
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LDW__AW_X_ABC) : /* ldw$_AW.x$LDRDi $RA,[$RB,$RC] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)));
  {
    SI opval = tmp_eaddr;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = EXTHISI (GETMEMHI (current_cpu, pc, tmp_eaddr));
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LDW_AB_X_ABC) : /* ldw.ab.x$LDRDi $RA,[$RB,$RC] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_sum;
  SI tmp_eaddr;
  tmp_sum = ADDSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)));
  tmp_eaddr = GET_H_CR (FLD (f_op_B));
  {
    SI opval = EXTHISI (GETMEMHI (current_cpu, pc, tmp_eaddr));
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = tmp_sum;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LDW_AS_X_ABC) : /* ldw.as.x$LDRDi $RA,[$RB,$RC] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR (FLD (f_op_B)), SLLSI (GET_H_CR (FLD (f_op_C)), 1));
  {
    SI opval = EXTHISI (GETMEMHI (current_cpu, pc, tmp_eaddr));
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LDW_S_X_ABU) : /* ldw$_S.x $R_c,[$R_b,$sc_u5w] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ldw_s_abu.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR16 (FLD (f_op__b)), FLD (f_u5x2));
  {
    SI opval = EXTHISI (GETMEMHI (current_cpu, pc, tmp_eaddr));
    SET_H_CR16 (FLD (f_op__c), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ST_ABS) : /* st$STODi $RC,[$RB,$s9] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_st_abs.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR (FLD (f_op_B)), FLD (f_s9));
  {
    SI opval = GET_H_CR (FLD (f_op_C));
    SETMEMSI (current_cpu, pc, tmp_eaddr, opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ST__AW_ABS) : /* st$_AW$STODi $RC,[$RB,$s9] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_st_abs.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR (FLD (f_op_B)), FLD (f_s9));
  {
    SI opval = tmp_eaddr;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_CR (FLD (f_op_C));
    SETMEMSI (current_cpu, pc, tmp_eaddr, opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ST_AB_ABS) : /* st.ab$STODi $RC,[$RB,$s9] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_st_abs.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_sum;
  SI tmp_eaddr;
  tmp_sum = ADDSI (GET_H_CR (FLD (f_op_B)), FLD (f_s9));
  tmp_eaddr = GET_H_CR (FLD (f_op_B));
  {
    SI opval = GET_H_CR (FLD (f_op_C));
    SETMEMSI (current_cpu, pc, tmp_eaddr, opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = tmp_sum;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ST_AS_ABS) : /* st.as$STODi $RC,[$RB,$s9] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_st_abs.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR (FLD (f_op_B)), SLLSI (FLD (f_s9), 2));
  {
    SI opval = GET_H_CR (FLD (f_op_C));
    SETMEMSI (current_cpu, pc, tmp_eaddr, opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ST_S_ABU) : /* st$_S $R_c,[$R_b,$sc_u5_] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ld_s_abu.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR16 (FLD (f_op__b)), FLD (f_u5x4));
  {
    SI opval = GET_H_CR16 (FLD (f_op__c));
    SETMEMSI (current_cpu, pc, tmp_eaddr, opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ST_S_ABSP) : /* st$_S $R_b,[$SP,$u5x4] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ld_s_abu.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_SP (), FLD (f_u5x4));
  {
    SI opval = GET_H_CR16 (FLD (f_op__b));
    SETMEMSI (current_cpu, pc, tmp_eaddr, opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_STB_ABS) : /* stb$STODi $RC,[$RB,$s9] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_st_abs.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR (FLD (f_op_B)), FLD (f_s9));
  {
    QI opval = GET_H_CR (FLD (f_op_C));
    SETMEMQI (current_cpu, pc, tmp_eaddr, opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_STB__AW_ABS) : /* stb$_AW$STODi $RC,[$RB,$s9] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_st_abs.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR (FLD (f_op_B)), FLD (f_s9));
  {
    SI opval = tmp_eaddr;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    QI opval = GET_H_CR (FLD (f_op_C));
    SETMEMQI (current_cpu, pc, tmp_eaddr, opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_STB_AB_ABS) : /* stb.ab$STODi $RC,[$RB,$s9] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_st_abs.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_sum;
  SI tmp_eaddr;
  tmp_sum = ADDSI (GET_H_CR (FLD (f_op_B)), FLD (f_s9));
  tmp_eaddr = GET_H_CR (FLD (f_op_B));
  {
    QI opval = GET_H_CR (FLD (f_op_C));
    SETMEMQI (current_cpu, pc, tmp_eaddr, opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = tmp_sum;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_STB_AS_ABS) : /* stb.as$STODi $RC,[$RB,$s9] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_st_abs.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR (FLD (f_op_B)), (cgen_rtx_error (current_cpu, "invalid insn"), 0));
  {
    QI opval = GET_H_CR (FLD (f_op_C));
    SETMEMQI (current_cpu, pc, tmp_eaddr, opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_STB_S_ABU) : /* stb$_S $R_c,[$R_b,$sc_u5b] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ldb_s_abu.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR16 (FLD (f_op__b)), FLD (f_u5));
  {
    QI opval = GET_H_CR16 (FLD (f_op__c));
    SETMEMQI (current_cpu, pc, tmp_eaddr, opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_STB_S_ABSP) : /* stb$_S $R_b,[$SP,$u5x4] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ld_s_abu.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_SP (), FLD (f_u5x4));
  {
    QI opval = GET_H_CR16 (FLD (f_op__b));
    SETMEMQI (current_cpu, pc, tmp_eaddr, opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_STW_ABS) : /* stw$STODi $RC,[$RB,$s9] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_st_abs.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR (FLD (f_op_B)), FLD (f_s9));
  {
    HI opval = GET_H_CR (FLD (f_op_C));
    SETMEMHI (current_cpu, pc, tmp_eaddr, opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_STW__AW_ABS) : /* stw$_AW$STODi $RC,[$RB,$s9] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_st_abs.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR (FLD (f_op_B)), FLD (f_s9));
  {
    SI opval = tmp_eaddr;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    HI opval = GET_H_CR (FLD (f_op_C));
    SETMEMHI (current_cpu, pc, tmp_eaddr, opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_STW_AB_ABS) : /* stw.ab$STODi $RC,[$RB,$s9] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_st_abs.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_sum;
  SI tmp_eaddr;
  tmp_sum = ADDSI (GET_H_CR (FLD (f_op_B)), FLD (f_s9));
  tmp_eaddr = GET_H_CR (FLD (f_op_B));
  {
    HI opval = GET_H_CR (FLD (f_op_C));
    SETMEMHI (current_cpu, pc, tmp_eaddr, opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = tmp_sum;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_STW_AS_ABS) : /* stw.as$STODi $RC,[$RB,$s9] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_st_abs.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR (FLD (f_op_B)), SLLSI (FLD (f_s9), 1));
  {
    HI opval = GET_H_CR (FLD (f_op_C));
    SETMEMHI (current_cpu, pc, tmp_eaddr, opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_STW_S_ABU) : /* stw$_S $R_c,[$R_b,$sc_u5w] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ldw_s_abu.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_eaddr;
  tmp_eaddr = ADDSI (GET_H_CR16 (FLD (f_op__b)), FLD (f_u5x2));
  {
    HI opval = GET_H_CR16 (FLD (f_op__c));
    SETMEMHI (current_cpu, pc, tmp_eaddr, opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ADD_L_S12__RA_) : /* add$_L$F $RB,$RB,$s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ADDSI (GET_H_CR (FLD (f_op_B)), FLD (f_s12));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ADDOFSI (GET_H_CR (FLD (f_op_B)), FLD (f_s12), 0);
    CPU (h_vbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = ADDCFSI (GET_H_CR (FLD (f_op_B)), FLD (f_s12), 0);
    CPU (h_cbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ADD_CCU6__RA_) : /* add$Qcondi$F $RB,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = ADDSI (GET_H_CR (FLD (f_op_B)), FLD (f_u6));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ADDOFSI (GET_H_CR (FLD (f_op_B)), FLD (f_u6), 0);
    CPU (h_vbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = ADDCFSI (GET_H_CR (FLD (f_op_B)), FLD (f_u6), 0);
    CPU (h_cbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ADD_L_U6__RA_) : /* add$_L$F $RA,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ADDSI (GET_H_CR (FLD (f_op_B)), FLD (f_u6));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ADDOFSI (GET_H_CR (FLD (f_op_B)), FLD (f_u6), 0);
    CPU (h_vbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = ADDCFSI (GET_H_CR (FLD (f_op_B)), FLD (f_u6), 0);
    CPU (h_cbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ADD_L_R_R__RA__RC) : /* add$_L$F $RA,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ADDSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ADDOFSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)), 0);
    CPU (h_vbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = ADDCFSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)), 0);
    CPU (h_cbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ADD_CC__RA__RC) : /* add$Qcondi$F $RB,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = ADDSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ADDOFSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)), 0);
    CPU (h_vbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = ADDCFSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)), 0);
    CPU (h_cbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ADD_S_ABC) : /* add$_S $R_a,$R_b,$R_c */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ld_s_abc.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
((void) 0); /*nop*/
  tmp_B = GET_H_CR16 (FLD (f_op__b));
  tmp_C = GET_H_CR16 (FLD (f_op__c));
  {
    SI opval = ADDSI (tmp_B, tmp_C);
    SET_H_CR16 (FLD (f_op__a), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ADD_S_CBU3) : /* add$_S $R_c,$R_b,$u3 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
((void) 0); /*nop*/
  tmp_B = GET_H_CR16 (FLD (f_op__b));
  tmp_C = FLD (f_u3);
  {
    SI opval = ADDSI (tmp_B, tmp_C);
    SET_H_CR16 (FLD (f_op__c), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ADD_S_MCAH) : /* add$_S $R_b,$R_b,$Rh */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_s_mcah.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
{
if (EQSI (FLD (f_op_h), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 2));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (2, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_h), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_B = GET_H_CR16 (FLD (f_op__b));
  tmp_C = GET_H_CR (FLD (f_op_h));
  {
    SI opval = ADDSI (tmp_B, tmp_C);
    SET_H_CR16 (FLD (f_op__b), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ADD_S_ABSP) : /* add$_S $R_b,$SP,$u5x4 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ld_s_abu.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
((void) 0); /*nop*/
  tmp_B = GET_H_SP ();
  tmp_C = FLD (f_u5x4);
  {
    SI opval = ADDSI (tmp_B, tmp_C);
    SET_H_CR16 (FLD (f_op__b), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ADD_S_ASSPSP) : /* add$_S $SP,$SP,$u5x4 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ld_s_abu.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
((void) 0); /*nop*/
  tmp_B = GET_H_SP ();
  tmp_C = FLD (f_u5x4);
  {
    SI opval = ADDSI (tmp_B, tmp_C);
    SET_H_SP (, opval);
    TRACE_RESULT (current_cpu, abuf, "sp", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ADD_S_GP) : /* add$_S $R0,$GP,$s9x4 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ld_s_gprel.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
((void) 0); /*nop*/
  tmp_B = GET_H_GP ();
  tmp_C = FLD (f_s9x4);
  {
    SI opval = ADDSI (tmp_B, tmp_C);
    SET_H_R0 (, opval);
    TRACE_RESULT (current_cpu, abuf, "r0", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ADD_S_R_U7) : /* add$_S $R_b,$R_b,$u7 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_s_r_u7.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
((void) 0); /*nop*/
  tmp_B = GET_H_CR16 (FLD (f_op__b));
  tmp_C = FLD (f_u7);
  {
    SI opval = ADDSI (tmp_B, tmp_C);
    SET_H_CR16 (FLD (f_op__b), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ADC_L_S12__RA_) : /* adc$_L$F $RB,$RB,$s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ADDCSI (GET_H_CR (FLD (f_op_B)), FLD (f_s12), CPU (h_cbit));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ADDOFSI (GET_H_CR (FLD (f_op_B)), FLD (f_s12), CPU (h_cbit));
    CPU (h_vbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = ADDCFSI (GET_H_CR (FLD (f_op_B)), FLD (f_s12), CPU (h_cbit));
    CPU (h_cbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ADC_CCU6__RA_) : /* adc$Qcondi$F $RB,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = ADDCSI (GET_H_CR (FLD (f_op_B)), FLD (f_u6), CPU (h_cbit));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ADDOFSI (GET_H_CR (FLD (f_op_B)), FLD (f_u6), CPU (h_cbit));
    CPU (h_vbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = ADDCFSI (GET_H_CR (FLD (f_op_B)), FLD (f_u6), CPU (h_cbit));
    CPU (h_cbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ADC_L_U6__RA_) : /* adc$_L$F $RA,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ADDCSI (GET_H_CR (FLD (f_op_B)), FLD (f_u6), CPU (h_cbit));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ADDOFSI (GET_H_CR (FLD (f_op_B)), FLD (f_u6), CPU (h_cbit));
    CPU (h_vbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = ADDCFSI (GET_H_CR (FLD (f_op_B)), FLD (f_u6), CPU (h_cbit));
    CPU (h_cbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ADC_L_R_R__RA__RC) : /* adc$_L$F $RA,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ADDCSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)), CPU (h_cbit));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ADDOFSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)), CPU (h_cbit));
    CPU (h_vbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = ADDCFSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)), CPU (h_cbit));
    CPU (h_cbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ADC_CC__RA__RC) : /* adc$Qcondi$F $RB,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = ADDCSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)), CPU (h_cbit));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ADDOFSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)), CPU (h_cbit));
    CPU (h_vbit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = ADDCFSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)), CPU (h_cbit));
    CPU (h_cbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SUB_L_S12__RA_) : /* sub$_L$F $RB,$RB,$s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = SUBSI (GET_H_CR (FLD (f_op_B)), FLD (f_s12));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = SUBOFSI (GET_H_CR (FLD (f_op_B)), FLD (f_s12), 0);
    CPU (h_vbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = SUBCFSI (GET_H_CR (FLD (f_op_B)), FLD (f_s12), 0);
    CPU (h_cbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SUB_CCU6__RA_) : /* sub$Qcondi$F $RB,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = SUBSI (GET_H_CR (FLD (f_op_B)), FLD (f_u6));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = SUBOFSI (GET_H_CR (FLD (f_op_B)), FLD (f_u6), 0);
    CPU (h_vbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = SUBCFSI (GET_H_CR (FLD (f_op_B)), FLD (f_u6), 0);
    CPU (h_cbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SUB_L_U6__RA_) : /* sub$_L$F $RA,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = SUBSI (GET_H_CR (FLD (f_op_B)), FLD (f_u6));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = SUBOFSI (GET_H_CR (FLD (f_op_B)), FLD (f_u6), 0);
    CPU (h_vbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = SUBCFSI (GET_H_CR (FLD (f_op_B)), FLD (f_u6), 0);
    CPU (h_cbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SUB_L_R_R__RA__RC) : /* sub$_L$F $RA,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = SUBSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = SUBOFSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)), 0);
    CPU (h_vbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = SUBCFSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)), 0);
    CPU (h_cbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SUB_CC__RA__RC) : /* sub$Qcondi$F $RB,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = SUBSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = SUBOFSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)), 0);
    CPU (h_vbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = SUBCFSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)), 0);
    CPU (h_cbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SUB_S_CBU3) : /* sub$_S $R_c,$R_b,$u3 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
((void) 0); /*nop*/
  tmp_B = GET_H_CR16 (FLD (f_op__b));
  tmp_C = FLD (f_u3);
  {
    SI opval = SUBSI (tmp_B, tmp_C);
    SET_H_CR16 (FLD (f_op__c), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_I16_GO_SUB_S_GO) : /* sub$_S $R_b,$R_b,$R_c */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
((void) 0); /*nop*/
  tmp_B = GET_H_CR16 (FLD (f_op__b));
  tmp_C = GET_H_CR16 (FLD (f_op__c));
  {
    SI opval = SUBSI (tmp_B, tmp_C);
    SET_H_CR16 (FLD (f_op__b), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SUB_S_GO_SUB_NE) : /* sub$_S $NE$R_b,$R_b,$R_b */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_mov_s_r_u7.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (EQBI (CPU (h_zbit), 0)) {
  {
    SI opval = 0;
    SET_H_CR16 (FLD (f_op__b), opval);
    written |= (1 << 1);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SUB_S_SSB) : /* sub$_S $R_b,$R_b,$u5 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ldb_s_abu.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
((void) 0); /*nop*/
  tmp_B = GET_H_CR16 (FLD (f_op__b));
  tmp_C = FLD (f_u5);
  {
    SI opval = SUBSI (tmp_B, tmp_C);
    SET_H_CR16 (FLD (f_op__b), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SUB_S_ASSPSP) : /* sub$_S $SP,$SP,$u5x4 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ld_s_abu.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
((void) 0); /*nop*/
  tmp_B = GET_H_SP ();
  tmp_C = FLD (f_u5x4);
  {
    SI opval = SUBSI (tmp_B, tmp_C);
    SET_H_SP (, opval);
    TRACE_RESULT (current_cpu, abuf, "sp", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SBC_L_S12__RA_) : /* sbc$_L$F $RB,$RB,$s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = SUBCSI (GET_H_CR (FLD (f_op_B)), FLD (f_s12), CPU (h_cbit));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = SUBOFSI (GET_H_CR (FLD (f_op_B)), FLD (f_s12), CPU (h_cbit));
    CPU (h_vbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = SUBCFSI (GET_H_CR (FLD (f_op_B)), FLD (f_s12), CPU (h_cbit));
    CPU (h_cbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SBC_CCU6__RA_) : /* sbc$Qcondi$F $RB,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = SUBCSI (GET_H_CR (FLD (f_op_B)), FLD (f_u6), CPU (h_cbit));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = SUBOFSI (GET_H_CR (FLD (f_op_B)), FLD (f_u6), CPU (h_cbit));
    CPU (h_vbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = SUBCFSI (GET_H_CR (FLD (f_op_B)), FLD (f_u6), CPU (h_cbit));
    CPU (h_cbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SBC_L_U6__RA_) : /* sbc$_L$F $RA,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = SUBCSI (GET_H_CR (FLD (f_op_B)), FLD (f_u6), CPU (h_cbit));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = SUBOFSI (GET_H_CR (FLD (f_op_B)), FLD (f_u6), CPU (h_cbit));
    CPU (h_vbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = SUBCFSI (GET_H_CR (FLD (f_op_B)), FLD (f_u6), CPU (h_cbit));
    CPU (h_cbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SBC_L_R_R__RA__RC) : /* sbc$_L$F $RA,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = SUBCSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)), CPU (h_cbit));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = SUBOFSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)), CPU (h_cbit));
    CPU (h_vbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = SUBCFSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)), CPU (h_cbit));
    CPU (h_cbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SBC_CC__RA__RC) : /* sbc$Qcondi$F $RB,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = SUBCSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)), CPU (h_cbit));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = SUBOFSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)), CPU (h_cbit));
    CPU (h_vbit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = SUBCFSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)), CPU (h_cbit));
    CPU (h_cbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_AND_L_S12__RA_) : /* and$_L$F $RB,$RB,$s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ANDSI (GET_H_CR (FLD (f_op_B)), FLD (f_s12));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_AND_CCU6__RA_) : /* and$Qcondi$F $RB,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = ANDSI (GET_H_CR (FLD (f_op_B)), FLD (f_u6));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_AND_L_U6__RA_) : /* and$_L$F $RA,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ANDSI (GET_H_CR (FLD (f_op_B)), FLD (f_u6));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_AND_L_R_R__RA__RC) : /* and$_L$F $RA,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ANDSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_AND_CC__RA__RC) : /* and$Qcondi$F $RB,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = ANDSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_I16_GO_AND_S_GO) : /* and$_S $R_b,$R_b,$R_c */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
((void) 0); /*nop*/
  tmp_B = GET_H_CR16 (FLD (f_op__b));
  tmp_C = GET_H_CR16 (FLD (f_op__c));
  {
    SI opval = ANDSI (tmp_B, tmp_C);
    SET_H_CR16 (FLD (f_op__b), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_OR_L_S12__RA_) : /* or$_L$F $RB,$RB,$s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ORSI (GET_H_CR (FLD (f_op_B)), FLD (f_s12));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_OR_CCU6__RA_) : /* or$Qcondi$F $RB,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = ORSI (GET_H_CR (FLD (f_op_B)), FLD (f_u6));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_OR_L_U6__RA_) : /* or$_L$F $RA,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ORSI (GET_H_CR (FLD (f_op_B)), FLD (f_u6));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_OR_L_R_R__RA__RC) : /* or$_L$F $RA,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ORSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_OR_CC__RA__RC) : /* or$Qcondi$F $RB,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = ORSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_I16_GO_OR_S_GO) : /* or$_S $R_b,$R_b,$R_c */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
((void) 0); /*nop*/
  tmp_B = GET_H_CR16 (FLD (f_op__b));
  tmp_C = GET_H_CR16 (FLD (f_op__c));
  {
    SI opval = ORSI (tmp_B, tmp_C);
    SET_H_CR16 (FLD (f_op__b), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BIC_L_S12__RA_) : /* bic$_L$F $RB,$RB,$s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ANDSI (GET_H_CR (FLD (f_op_B)), INVSI (FLD (f_s12)));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BIC_CCU6__RA_) : /* bic$Qcondi$F $RB,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = ANDSI (GET_H_CR (FLD (f_op_B)), INVSI (FLD (f_u6)));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BIC_L_U6__RA_) : /* bic$_L$F $RA,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ANDSI (GET_H_CR (FLD (f_op_B)), INVSI (FLD (f_u6)));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BIC_L_R_R__RA__RC) : /* bic$_L$F $RA,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ANDSI (GET_H_CR (FLD (f_op_B)), INVSI (GET_H_CR (FLD (f_op_C))));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BIC_CC__RA__RC) : /* bic$Qcondi$F $RB,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = ANDSI (GET_H_CR (FLD (f_op_B)), INVSI (GET_H_CR (FLD (f_op_C))));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_I16_GO_BIC_S_GO) : /* bic$_S $R_b,$R_b,$R_c */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
((void) 0); /*nop*/
  tmp_B = GET_H_CR16 (FLD (f_op__b));
  tmp_C = GET_H_CR16 (FLD (f_op__c));
  {
    SI opval = ANDSI (tmp_B, INVSI (tmp_C));
    SET_H_CR16 (FLD (f_op__b), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_XOR_L_S12__RA_) : /* xor$_L$F $RB,$RB,$s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = XORSI (GET_H_CR (FLD (f_op_B)), FLD (f_s12));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_XOR_CCU6__RA_) : /* xor$Qcondi$F $RB,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = XORSI (GET_H_CR (FLD (f_op_B)), FLD (f_u6));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_XOR_L_U6__RA_) : /* xor$_L$F $RA,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = XORSI (GET_H_CR (FLD (f_op_B)), FLD (f_u6));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_XOR_L_R_R__RA__RC) : /* xor$_L$F $RA,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = XORSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_XOR_CC__RA__RC) : /* xor$Qcondi$F $RB,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = XORSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_I16_GO_XOR_S_GO) : /* xor$_S $R_b,$R_b,$R_c */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
((void) 0); /*nop*/
  tmp_B = GET_H_CR16 (FLD (f_op__b));
  tmp_C = GET_H_CR16 (FLD (f_op__c));
  {
    SI opval = XORSI (tmp_B, tmp_C);
    SET_H_CR16 (FLD (f_op__b), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MAX_L_S12__RA_) : /* max$_L$F $RB,$RB,$s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (GTSI (GET_H_CR (FLD (f_op_B)), FLD (f_s12))) ? (GET_H_CR (FLD (f_op_B))) : (FLD (f_s12));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = GESI (FLD (f_s12), GET_H_CR (FLD (f_op_B)));
    CPU (h_cbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
  {
    BI opval = SUBOFSI (GET_H_CR (FLD (f_op_B)), FLD (f_s12), 0);
    CPU (h_vbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MAX_CCU6__RA_) : /* max$Qcondi$F $RB,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = (GTSI (GET_H_CR (FLD (f_op_B)), FLD (f_u6))) ? (GET_H_CR (FLD (f_op_B))) : (FLD (f_u6));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = GESI (FLD (f_u6), GET_H_CR (FLD (f_op_B)));
    CPU (h_cbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
  {
    BI opval = SUBOFSI (GET_H_CR (FLD (f_op_B)), FLD (f_u6), 0);
    CPU (h_vbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MAX_L_U6__RA_) : /* max$_L$F $RA,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (GTSI (GET_H_CR (FLD (f_op_B)), FLD (f_u6))) ? (GET_H_CR (FLD (f_op_B))) : (FLD (f_u6));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = GESI (FLD (f_u6), GET_H_CR (FLD (f_op_B)));
    CPU (h_cbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
  {
    BI opval = SUBOFSI (GET_H_CR (FLD (f_op_B)), FLD (f_u6), 0);
    CPU (h_vbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MAX_L_R_R__RA__RC) : /* max$_L$F $RA,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (GTSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)))) ? (GET_H_CR (FLD (f_op_B))) : (GET_H_CR (FLD (f_op_C)));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = GESI (GET_H_CR (FLD (f_op_C)), GET_H_CR (FLD (f_op_B)));
    CPU (h_cbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
  {
    BI opval = SUBOFSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)), 0);
    CPU (h_vbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MAX_CC__RA__RC) : /* max$Qcondi$F $RB,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = (GTSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)))) ? (GET_H_CR (FLD (f_op_B))) : (GET_H_CR (FLD (f_op_C)));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = GESI (GET_H_CR (FLD (f_op_C)), GET_H_CR (FLD (f_op_B)));
    CPU (h_cbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
  {
    BI opval = SUBOFSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)), 0);
    CPU (h_vbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MIN_L_S12__RA_) : /* min$_L$F $RB,$RB,$s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (LTSI (GET_H_CR (FLD (f_op_B)), FLD (f_s12))) ? (GET_H_CR (FLD (f_op_B))) : (FLD (f_s12));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = LESI (FLD (f_s12), GET_H_CR (FLD (f_op_B)));
    CPU (h_cbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
  {
    BI opval = SUBOFSI (GET_H_CR (FLD (f_op_B)), FLD (f_s12), 0);
    CPU (h_vbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MIN_CCU6__RA_) : /* min$Qcondi$F $RB,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = (LTSI (GET_H_CR (FLD (f_op_B)), FLD (f_u6))) ? (GET_H_CR (FLD (f_op_B))) : (FLD (f_u6));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = LESI (FLD (f_u6), GET_H_CR (FLD (f_op_B)));
    CPU (h_cbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
  {
    BI opval = SUBOFSI (GET_H_CR (FLD (f_op_B)), FLD (f_u6), 0);
    CPU (h_vbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MIN_L_U6__RA_) : /* min$_L$F $RA,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (LTSI (GET_H_CR (FLD (f_op_B)), FLD (f_u6))) ? (GET_H_CR (FLD (f_op_B))) : (FLD (f_u6));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = LESI (FLD (f_u6), GET_H_CR (FLD (f_op_B)));
    CPU (h_cbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
  {
    BI opval = SUBOFSI (GET_H_CR (FLD (f_op_B)), FLD (f_u6), 0);
    CPU (h_vbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MIN_L_R_R__RA__RC) : /* min$_L$F $RA,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (LTSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)))) ? (GET_H_CR (FLD (f_op_B))) : (GET_H_CR (FLD (f_op_C)));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = LESI (GET_H_CR (FLD (f_op_C)), GET_H_CR (FLD (f_op_B)));
    CPU (h_cbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
  {
    BI opval = SUBOFSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)), 0);
    CPU (h_vbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MIN_CC__RA__RC) : /* min$Qcondi$F $RB,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = (LTSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)))) ? (GET_H_CR (FLD (f_op_B))) : (GET_H_CR (FLD (f_op_C)));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = LESI (GET_H_CR (FLD (f_op_C)), GET_H_CR (FLD (f_op_B)));
    CPU (h_cbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
  {
    BI opval = SUBOFSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C)), 0);
    CPU (h_vbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MOV_L_S12_) : /* mov$_L$F $RB,$s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
((void) 0); /*nop*/
  tmp_result = FLD (f_s12);
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
if (GTSI (FLD (f_F), 0)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 4);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MOV_CCU6_) : /* mov$Qcondi$F $RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
((void) 0); /*nop*/
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
((void) 0); /*nop*/
  tmp_result = FLD (f_u6);
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
if (GTSI (FLD (f_F), 0)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 4);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 5);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MOV_L_U6_) : /* mov$_L$F $RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
((void) 0); /*nop*/
  tmp_result = FLD (f_u6);
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
if (GTSI (FLD (f_F), 0)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 4);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MOV_L_R_R__RC) : /* mov$_L$F $RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
{
if (EQSI (FLD (f_op_C), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_C), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = GET_H_CR (FLD (f_op_C));
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
if (GTSI (FLD (f_F), 0)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MOV_CC__RC) : /* mov$Qcondi$F $RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_C), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_C), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
((void) 0); /*nop*/
  tmp_result = GET_H_CR (FLD (f_op_C));
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
if (GTSI (FLD (f_F), 0)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MOV_S_MCAH) : /* mov$_S $R_b,$Rh */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_s_mcah.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
{
if (EQSI (FLD (f_op_h), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 2));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (2, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_h), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = GET_H_CR (FLD (f_op_h));
  {
    SI opval = tmp_result;
    SET_H_CR16 (FLD (f_op__b), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
if (GTSI (-1, 0)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 6);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MOV_S_MCAHB) : /* mov$_S $Rh,$R_b */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_s_mcah.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
((void) 0); /*nop*/
  tmp_result = GET_H_CR16 (FLD (f_op__b));
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_h), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
if (GTSI (-1, 0)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MOV_S_R_U7) : /* mov$_S $R_b,$u7 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_mov_s_r_u7.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
((void) 0); /*nop*/
  tmp_result = FLD (f_u8);
  {
    SI opval = tmp_result;
    SET_H_CR16 (FLD (f_op__b), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
if (GTSI (-1, 0)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_TST_L_S12_) : /* tst$_L$F1 $RB,$s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_B = GET_H_CR (FLD (f_op_B));
  tmp_C = FLD (f_s12);
  tmp_result = ANDSI (tmp_B, tmp_C);
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_TST_CCU6_) : /* tst$Qcondi$F1 $RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
((void) 0); /*nop*/
  tmp_B = GET_H_CR (FLD (f_op_B));
  tmp_C = FLD (f_u6);
  tmp_result = ANDSI (tmp_B, tmp_C);
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_TST_L_U6_) : /* tst$_L$F1 $RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_B = GET_H_CR (FLD (f_op_B));
  tmp_C = FLD (f_u6);
  tmp_result = ANDSI (tmp_B, tmp_C);
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_TST_L_R_R__RC) : /* tst$_L$F1 $RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_st_abs.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_B = GET_H_CR (FLD (f_op_B));
  tmp_C = GET_H_CR (FLD (f_op_C));
  tmp_result = ANDSI (tmp_B, tmp_C);
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_TST_CC__RC) : /* tst$Qcondi$F1 $RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
((void) 0); /*nop*/
  tmp_B = GET_H_CR (FLD (f_op_B));
  tmp_C = GET_H_CR (FLD (f_op_C));
  tmp_result = ANDSI (tmp_B, tmp_C);
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_TST_S_GO) : /* tst$_S $R_b,$R_c */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
((void) 0); /*nop*/
  tmp_B = GET_H_CR16 (FLD (f_op__b));
  tmp_C = GET_H_CR16 (FLD (f_op__c));
  tmp_result = ANDSI (tmp_B, tmp_C);
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_CMP_L_S12_) : /* cmp$_L$F1 $RB,$s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  DI tmp_tmp;
  DI tmp_B;
  DI tmp_C;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_B = EXTSIDI (GET_H_CR (FLD (f_op_B)));
  tmp_C = EXTSIDI (FLD (f_s12));
  tmp_tmp = SUBDI (tmp_B, tmp_C);
  tmp_result = SUBWORDDISI (tmp_tmp, 1);
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = NEBI (LTSI (tmp_result, 0), LTDI (tmp_tmp, 0));
    CPU (h_vbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  tmp_B = ZEXTSIDI (GET_H_CR (FLD (f_op_B)));
  tmp_C = ZEXTSIDI (FLD (f_s12));
  tmp_tmp = SUBDI (tmp_B, tmp_C);
  {
    BI opval = LTDI (tmp_tmp, 0);
    CPU (h_cbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_CMP_CCU6_) : /* cmp$Qcondi$F1 $RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  DI tmp_tmp;
  DI tmp_B;
  DI tmp_C;
((void) 0); /*nop*/
  tmp_B = EXTSIDI (GET_H_CR (FLD (f_op_B)));
  tmp_C = EXTSIDI (FLD (f_u6));
  tmp_tmp = SUBDI (tmp_B, tmp_C);
  tmp_result = SUBWORDDISI (tmp_tmp, 1);
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = NEBI (LTSI (tmp_result, 0), LTDI (tmp_tmp, 0));
    CPU (h_vbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  tmp_B = ZEXTSIDI (GET_H_CR (FLD (f_op_B)));
  tmp_C = ZEXTSIDI (FLD (f_u6));
  tmp_tmp = SUBDI (tmp_B, tmp_C);
  {
    BI opval = LTDI (tmp_tmp, 0);
    CPU (h_cbit) = opval;
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_CMP_L_U6_) : /* cmp$_L$F1 $RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  DI tmp_tmp;
  DI tmp_B;
  DI tmp_C;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_B = EXTSIDI (GET_H_CR (FLD (f_op_B)));
  tmp_C = EXTSIDI (FLD (f_u6));
  tmp_tmp = SUBDI (tmp_B, tmp_C);
  tmp_result = SUBWORDDISI (tmp_tmp, 1);
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = NEBI (LTSI (tmp_result, 0), LTDI (tmp_tmp, 0));
    CPU (h_vbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  tmp_B = ZEXTSIDI (GET_H_CR (FLD (f_op_B)));
  tmp_C = ZEXTSIDI (FLD (f_u6));
  tmp_tmp = SUBDI (tmp_B, tmp_C);
  {
    BI opval = LTDI (tmp_tmp, 0);
    CPU (h_cbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_CMP_L_R_R__RC) : /* cmp$_L$F1 $RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_st_abs.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  DI tmp_tmp;
  DI tmp_B;
  DI tmp_C;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_B = EXTSIDI (GET_H_CR (FLD (f_op_B)));
  tmp_C = EXTSIDI (GET_H_CR (FLD (f_op_C)));
  tmp_tmp = SUBDI (tmp_B, tmp_C);
  tmp_result = SUBWORDDISI (tmp_tmp, 1);
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = NEBI (LTSI (tmp_result, 0), LTDI (tmp_tmp, 0));
    CPU (h_vbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  tmp_B = ZEXTSIDI (GET_H_CR (FLD (f_op_B)));
  tmp_C = ZEXTSIDI (GET_H_CR (FLD (f_op_C)));
  tmp_tmp = SUBDI (tmp_B, tmp_C);
  {
    BI opval = LTDI (tmp_tmp, 0);
    CPU (h_cbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_CMP_CC__RC) : /* cmp$Qcondi$F1 $RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  DI tmp_tmp;
  DI tmp_B;
  DI tmp_C;
((void) 0); /*nop*/
  tmp_B = EXTSIDI (GET_H_CR (FLD (f_op_B)));
  tmp_C = EXTSIDI (GET_H_CR (FLD (f_op_C)));
  tmp_tmp = SUBDI (tmp_B, tmp_C);
  tmp_result = SUBWORDDISI (tmp_tmp, 1);
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = NEBI (LTSI (tmp_result, 0), LTDI (tmp_tmp, 0));
    CPU (h_vbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  tmp_B = ZEXTSIDI (GET_H_CR (FLD (f_op_B)));
  tmp_C = ZEXTSIDI (GET_H_CR (FLD (f_op_C)));
  tmp_tmp = SUBDI (tmp_B, tmp_C);
  {
    BI opval = LTDI (tmp_tmp, 0);
    CPU (h_cbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_CMP_S_MCAH) : /* cmp$_S $R_b,$Rh */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_s_mcah.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  DI tmp_tmp;
  DI tmp_B;
  DI tmp_C;
{
if (EQSI (FLD (f_op_h), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 2));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (2, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_h), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_B = EXTSIDI (GET_H_CR16 (FLD (f_op__b)));
  tmp_C = EXTSIDI (GET_H_CR (FLD (f_op_h)));
  tmp_tmp = SUBDI (tmp_B, tmp_C);
  tmp_result = SUBWORDDISI (tmp_tmp, 1);
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = NEBI (LTSI (tmp_result, 0), LTDI (tmp_tmp, 0));
    CPU (h_vbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  tmp_B = ZEXTSIDI (GET_H_CR16 (FLD (f_op__b)));
  tmp_C = ZEXTSIDI (GET_H_CR (FLD (f_op_h)));
  tmp_tmp = SUBDI (tmp_B, tmp_C);
  {
    BI opval = LTDI (tmp_tmp, 0);
    CPU (h_cbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_CMP_S_R_U7) : /* cmp$_S $R_b,$u7 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_s_r_u7.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  DI tmp_tmp;
  DI tmp_B;
  DI tmp_C;
((void) 0); /*nop*/
  tmp_B = EXTSIDI (GET_H_CR16 (FLD (f_op__b)));
  tmp_C = EXTSIDI (FLD (f_u7));
  tmp_tmp = SUBDI (tmp_B, tmp_C);
  tmp_result = SUBWORDDISI (tmp_tmp, 1);
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = NEBI (LTSI (tmp_result, 0), LTDI (tmp_tmp, 0));
    CPU (h_vbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  tmp_B = ZEXTSIDI (GET_H_CR16 (FLD (f_op__b)));
  tmp_C = ZEXTSIDI (FLD (f_u7));
  tmp_tmp = SUBDI (tmp_B, tmp_C);
  {
    BI opval = LTDI (tmp_tmp, 0);
    CPU (h_cbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_RCMP_L_S12_) : /* rcmp$_L$F1 $RB,$s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  DI tmp_tmp;
  DI tmp_B;
  DI tmp_C;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_B = EXTSIDI (GET_H_CR (FLD (f_op_B)));
  tmp_C = EXTSIDI (FLD (f_s12));
  tmp_tmp = SUBDI (tmp_C, tmp_B);
  tmp_result = SUBWORDDISI (tmp_tmp, 1);
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = NEBI (LTSI (tmp_result, 0), LTDI (tmp_tmp, 0));
    CPU (h_vbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  tmp_B = ZEXTSIDI (GET_H_CR (FLD (f_op_B)));
  tmp_C = ZEXTSIDI (FLD (f_s12));
  tmp_tmp = SUBDI (tmp_C, tmp_B);
  {
    BI opval = LTDI (tmp_tmp, 0);
    CPU (h_cbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_RCMP_CCU6_) : /* rcmp$Qcondi$F1 $RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  DI tmp_tmp;
  DI tmp_B;
  DI tmp_C;
((void) 0); /*nop*/
  tmp_B = EXTSIDI (GET_H_CR (FLD (f_op_B)));
  tmp_C = EXTSIDI (FLD (f_u6));
  tmp_tmp = SUBDI (tmp_C, tmp_B);
  tmp_result = SUBWORDDISI (tmp_tmp, 1);
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = NEBI (LTSI (tmp_result, 0), LTDI (tmp_tmp, 0));
    CPU (h_vbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  tmp_B = ZEXTSIDI (GET_H_CR (FLD (f_op_B)));
  tmp_C = ZEXTSIDI (FLD (f_u6));
  tmp_tmp = SUBDI (tmp_C, tmp_B);
  {
    BI opval = LTDI (tmp_tmp, 0);
    CPU (h_cbit) = opval;
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_RCMP_L_U6_) : /* rcmp$_L$F1 $RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  DI tmp_tmp;
  DI tmp_B;
  DI tmp_C;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_B = EXTSIDI (GET_H_CR (FLD (f_op_B)));
  tmp_C = EXTSIDI (FLD (f_u6));
  tmp_tmp = SUBDI (tmp_C, tmp_B);
  tmp_result = SUBWORDDISI (tmp_tmp, 1);
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = NEBI (LTSI (tmp_result, 0), LTDI (tmp_tmp, 0));
    CPU (h_vbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  tmp_B = ZEXTSIDI (GET_H_CR (FLD (f_op_B)));
  tmp_C = ZEXTSIDI (FLD (f_u6));
  tmp_tmp = SUBDI (tmp_C, tmp_B);
  {
    BI opval = LTDI (tmp_tmp, 0);
    CPU (h_cbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_RCMP_L_R_R__RC) : /* rcmp$_L$F1 $RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_st_abs.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  DI tmp_tmp;
  DI tmp_B;
  DI tmp_C;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_B = EXTSIDI (GET_H_CR (FLD (f_op_B)));
  tmp_C = EXTSIDI (GET_H_CR (FLD (f_op_C)));
  tmp_tmp = SUBDI (tmp_C, tmp_B);
  tmp_result = SUBWORDDISI (tmp_tmp, 1);
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = NEBI (LTSI (tmp_result, 0), LTDI (tmp_tmp, 0));
    CPU (h_vbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  tmp_B = ZEXTSIDI (GET_H_CR (FLD (f_op_B)));
  tmp_C = ZEXTSIDI (GET_H_CR (FLD (f_op_C)));
  tmp_tmp = SUBDI (tmp_C, tmp_B);
  {
    BI opval = LTDI (tmp_tmp, 0);
    CPU (h_cbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_RCMP_CC__RC) : /* rcmp$Qcondi$F1 $RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  DI tmp_tmp;
  DI tmp_B;
  DI tmp_C;
((void) 0); /*nop*/
  tmp_B = EXTSIDI (GET_H_CR (FLD (f_op_B)));
  tmp_C = EXTSIDI (GET_H_CR (FLD (f_op_C)));
  tmp_tmp = SUBDI (tmp_C, tmp_B);
  tmp_result = SUBWORDDISI (tmp_tmp, 1);
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = NEBI (LTSI (tmp_result, 0), LTDI (tmp_tmp, 0));
    CPU (h_vbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  tmp_B = ZEXTSIDI (GET_H_CR (FLD (f_op_B)));
  tmp_C = ZEXTSIDI (GET_H_CR (FLD (f_op_C)));
  tmp_tmp = SUBDI (tmp_C, tmp_B);
  {
    BI opval = LTDI (tmp_tmp, 0);
    CPU (h_cbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_RSUB_L_S12__RA_) : /* rsub$_L$F $RB,$RB,$s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = SUBSI (FLD (f_s12), GET_H_CR (FLD (f_op_B)));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = SUBOFSI (FLD (f_s12), GET_H_CR (FLD (f_op_B)), 0);
    CPU (h_vbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = SUBCFSI (FLD (f_s12), GET_H_CR (FLD (f_op_B)), 0);
    CPU (h_cbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_RSUB_CCU6__RA_) : /* rsub$Qcondi$F $RB,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = SUBSI (FLD (f_u6), GET_H_CR (FLD (f_op_B)));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = SUBOFSI (FLD (f_u6), GET_H_CR (FLD (f_op_B)), 0);
    CPU (h_vbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = SUBCFSI (FLD (f_u6), GET_H_CR (FLD (f_op_B)), 0);
    CPU (h_cbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_RSUB_L_U6__RA_) : /* rsub$_L$F $RA,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = SUBSI (FLD (f_u6), GET_H_CR (FLD (f_op_B)));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = SUBOFSI (FLD (f_u6), GET_H_CR (FLD (f_op_B)), 0);
    CPU (h_vbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = SUBCFSI (FLD (f_u6), GET_H_CR (FLD (f_op_B)), 0);
    CPU (h_cbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_RSUB_L_R_R__RA__RC) : /* rsub$_L$F $RA,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = SUBSI (GET_H_CR (FLD (f_op_C)), GET_H_CR (FLD (f_op_B)));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = SUBOFSI (GET_H_CR (FLD (f_op_C)), GET_H_CR (FLD (f_op_B)), 0);
    CPU (h_vbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = SUBCFSI (GET_H_CR (FLD (f_op_C)), GET_H_CR (FLD (f_op_B)), 0);
    CPU (h_cbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_RSUB_CC__RA__RC) : /* rsub$Qcondi$F $RB,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = SUBSI (GET_H_CR (FLD (f_op_C)), GET_H_CR (FLD (f_op_B)));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = SUBOFSI (GET_H_CR (FLD (f_op_C)), GET_H_CR (FLD (f_op_B)), 0);
    CPU (h_vbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = SUBCFSI (GET_H_CR (FLD (f_op_C)), GET_H_CR (FLD (f_op_B)), 0);
    CPU (h_cbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BSET_L_S12__RA_) : /* bset$_L$F $RB,$RB,$s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ORSI (GET_H_CR (FLD (f_op_B)), SLLSI (1, ANDSI (FLD (f_s12), 31)));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BSET_CCU6__RA_) : /* bset$Qcondi$F $RB,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = ORSI (GET_H_CR (FLD (f_op_B)), SLLSI (1, ANDSI (FLD (f_u6), 31)));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BSET_L_U6__RA_) : /* bset$_L$F $RA,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ORSI (GET_H_CR (FLD (f_op_B)), SLLSI (1, ANDSI (FLD (f_u6), 31)));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BSET_L_R_R__RA__RC) : /* bset$_L$F $RA,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ORSI (GET_H_CR (FLD (f_op_B)), SLLSI (1, ANDSI (GET_H_CR (FLD (f_op_C)), 31)));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BSET_CC__RA__RC) : /* bset$Qcondi$F $RB,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = ORSI (GET_H_CR (FLD (f_op_B)), SLLSI (1, ANDSI (GET_H_CR (FLD (f_op_C)), 31)));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BSET_S_SSB) : /* bset$_S $R_b,$R_b,$u5 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ldb_s_abu.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
((void) 0); /*nop*/
  tmp_B = GET_H_CR16 (FLD (f_op__b));
  tmp_C = FLD (f_u5);
  {
    SI opval = ORSI (tmp_B, SLLSI (1, ANDSI (tmp_C, 31)));
    SET_H_CR16 (FLD (f_op__b), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BCLR_L_S12__RA_) : /* bclr$_L$F $RB,$RB,$s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ANDSI (GET_H_CR (FLD (f_op_B)), INVSI (SLLSI (1, ANDSI (FLD (f_s12), 31))));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BCLR_CCU6__RA_) : /* bclr$Qcondi$F $RB,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = ANDSI (GET_H_CR (FLD (f_op_B)), INVSI (SLLSI (1, ANDSI (FLD (f_u6), 31))));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BCLR_L_U6__RA_) : /* bclr$_L$F $RA,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ANDSI (GET_H_CR (FLD (f_op_B)), INVSI (SLLSI (1, ANDSI (FLD (f_u6), 31))));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BCLR_L_R_R__RA__RC) : /* bclr$_L$F $RA,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ANDSI (GET_H_CR (FLD (f_op_B)), INVSI (SLLSI (1, ANDSI (GET_H_CR (FLD (f_op_C)), 31))));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BCLR_CC__RA__RC) : /* bclr$Qcondi$F $RB,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = ANDSI (GET_H_CR (FLD (f_op_B)), INVSI (SLLSI (1, ANDSI (GET_H_CR (FLD (f_op_C)), 31))));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BCLR_S_SSB) : /* bclr$_S $R_b,$R_b,$u5 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ldb_s_abu.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
((void) 0); /*nop*/
  tmp_B = GET_H_CR16 (FLD (f_op__b));
  tmp_C = FLD (f_u5);
  {
    SI opval = ANDSI (tmp_B, INVSI (SLLSI (1, ANDSI (tmp_C, 31))));
    SET_H_CR16 (FLD (f_op__b), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BTST_L_S12_) : /* btst$_L$F1 $RB,$s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_B = GET_H_CR (FLD (f_op_B));
  tmp_C = FLD (f_s12);
  tmp_result = ANDSI (tmp_B, SLLSI (1, ANDSI (tmp_C, 31)));
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BTST_CCU6_) : /* btst$Qcondi$F1 $RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
((void) 0); /*nop*/
  tmp_B = GET_H_CR (FLD (f_op_B));
  tmp_C = FLD (f_u6);
  tmp_result = ANDSI (tmp_B, SLLSI (1, ANDSI (tmp_C, 31)));
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BTST_L_U6_) : /* btst$_L$F1 $RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_B = GET_H_CR (FLD (f_op_B));
  tmp_C = FLD (f_u6);
  tmp_result = ANDSI (tmp_B, SLLSI (1, ANDSI (tmp_C, 31)));
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BTST_L_R_R__RC) : /* btst$_L$F1 $RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_st_abs.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_B = GET_H_CR (FLD (f_op_B));
  tmp_C = GET_H_CR (FLD (f_op_C));
  tmp_result = ANDSI (tmp_B, SLLSI (1, ANDSI (tmp_C, 31)));
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BTST_CC__RC) : /* btst$Qcondi$F1 $RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
((void) 0); /*nop*/
  tmp_B = GET_H_CR (FLD (f_op_B));
  tmp_C = GET_H_CR (FLD (f_op_C));
  tmp_result = ANDSI (tmp_B, SLLSI (1, ANDSI (tmp_C, 31)));
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BTST_S_SSB) : /* btst$_S $R_b,$u5 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ldb_s_abu.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
((void) 0); /*nop*/
  tmp_B = GET_H_CR16 (FLD (f_op__b));
  tmp_C = FLD (f_u5);
  tmp_result = ANDSI (tmp_B, SLLSI (1, ANDSI (tmp_C, 31)));
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BXOR_L_S12__RA_) : /* bxor$_L$F $RB,$RB,$s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = XORSI (GET_H_CR (FLD (f_op_B)), SLLSI (1, ANDSI (FLD (f_s12), 31)));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BXOR_CCU6__RA_) : /* bxor$Qcondi$F $RB,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = XORSI (GET_H_CR (FLD (f_op_B)), SLLSI (1, ANDSI (FLD (f_u6), 31)));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BXOR_L_U6__RA_) : /* bxor$_L$F $RA,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = XORSI (GET_H_CR (FLD (f_op_B)), SLLSI (1, ANDSI (FLD (f_u6), 31)));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BXOR_L_R_R__RA__RC) : /* bxor$_L$F $RA,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = XORSI (GET_H_CR (FLD (f_op_B)), SLLSI (1, ANDSI (GET_H_CR (FLD (f_op_C)), 31)));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BXOR_CC__RA__RC) : /* bxor$Qcondi$F $RB,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = XORSI (GET_H_CR (FLD (f_op_B)), SLLSI (1, ANDSI (GET_H_CR (FLD (f_op_C)), 31)));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BMSK_L_S12__RA_) : /* bmsk$_L$F $RB,$RB,$s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ANDSI (GET_H_CR (FLD (f_op_B)), SUBSI (SLLSI (SLLSI (1, ANDSI (FLD (f_s12), 31)), 1), 1));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BMSK_CCU6__RA_) : /* bmsk$Qcondi$F $RB,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = ANDSI (GET_H_CR (FLD (f_op_B)), SUBSI (SLLSI (SLLSI (1, ANDSI (FLD (f_u6), 31)), 1), 1));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BMSK_L_U6__RA_) : /* bmsk$_L$F $RA,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ANDSI (GET_H_CR (FLD (f_op_B)), SUBSI (SLLSI (SLLSI (1, ANDSI (FLD (f_u6), 31)), 1), 1));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BMSK_L_R_R__RA__RC) : /* bmsk$_L$F $RA,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ANDSI (GET_H_CR (FLD (f_op_B)), SUBSI (SLLSI (SLLSI (1, ANDSI (GET_H_CR (FLD (f_op_C)), 31)), 1), 1));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BMSK_CC__RA__RC) : /* bmsk$Qcondi$F $RB,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = ANDSI (GET_H_CR (FLD (f_op_B)), SUBSI (SLLSI (SLLSI (1, ANDSI (GET_H_CR (FLD (f_op_C)), 31)), 1), 1));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BMSK_S_SSB) : /* bmsk$_S $R_b,$R_b,$u5 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ldb_s_abu.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
((void) 0); /*nop*/
  tmp_B = GET_H_CR16 (FLD (f_op__b));
  tmp_C = FLD (f_u5);
  {
    SI opval = ANDSI (tmp_B, SUBSI (SLLSI (SLLSI (1, ANDSI (tmp_C, 31)), 1), 1));
    SET_H_CR16 (FLD (f_op__b), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ADD1_L_S12__RA_) : /* add1$_L$F $RB,$RB,$s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ADDSI (GET_H_CR (FLD (f_op_B)), SLLSI (FLD (f_s12), 1));
if (FLD (f_F)) {
{
  SI tmp_sC;
  tmp_sC = SLLSI (FLD (f_s12), 1);
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ADDOFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_vbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = ADDCFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_cbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ADD1_CCU6__RA_) : /* add1$Qcondi$F $RB,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = ADDSI (GET_H_CR (FLD (f_op_B)), SLLSI (FLD (f_u6), 1));
if (FLD (f_F)) {
{
  SI tmp_sC;
  tmp_sC = SLLSI (FLD (f_u6), 1);
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ADDOFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_vbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = ADDCFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_cbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ADD1_L_U6__RA_) : /* add1$_L$F $RA,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ADDSI (GET_H_CR (FLD (f_op_B)), SLLSI (FLD (f_u6), 1));
if (FLD (f_F)) {
{
  SI tmp_sC;
  tmp_sC = SLLSI (FLD (f_u6), 1);
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ADDOFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_vbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = ADDCFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_cbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ADD1_L_R_R__RA__RC) : /* add1$_L$F $RA,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ADDSI (GET_H_CR (FLD (f_op_B)), SLLSI (GET_H_CR (FLD (f_op_C)), 1));
if (FLD (f_F)) {
{
  SI tmp_sC;
  tmp_sC = SLLSI (GET_H_CR (FLD (f_op_C)), 1);
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ADDOFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_vbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = ADDCFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_cbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ADD1_CC__RA__RC) : /* add1$Qcondi$F $RB,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = ADDSI (GET_H_CR (FLD (f_op_B)), SLLSI (GET_H_CR (FLD (f_op_C)), 1));
if (FLD (f_F)) {
{
  SI tmp_sC;
  tmp_sC = SLLSI (GET_H_CR (FLD (f_op_C)), 1);
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ADDOFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_vbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = ADDCFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_cbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_I16_GO_ADD1_S_GO) : /* add1$_S $R_b,$R_b,$R_c */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
((void) 0); /*nop*/
  tmp_B = GET_H_CR16 (FLD (f_op__b));
  tmp_C = GET_H_CR16 (FLD (f_op__c));
  {
    SI opval = ADDSI (tmp_B, SLLSI (tmp_C, 1));
    SET_H_CR16 (FLD (f_op__b), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ADD2_L_S12__RA_) : /* add2$_L$F $RB,$RB,$s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ADDSI (GET_H_CR (FLD (f_op_B)), SLLSI (FLD (f_s12), 2));
if (FLD (f_F)) {
{
  SI tmp_sC;
  tmp_sC = SLLSI (FLD (f_s12), 2);
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ADDOFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_vbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = ADDCFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_cbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ADD2_CCU6__RA_) : /* add2$Qcondi$F $RB,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = ADDSI (GET_H_CR (FLD (f_op_B)), SLLSI (FLD (f_u6), 2));
if (FLD (f_F)) {
{
  SI tmp_sC;
  tmp_sC = SLLSI (FLD (f_u6), 2);
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ADDOFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_vbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = ADDCFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_cbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ADD2_L_U6__RA_) : /* add2$_L$F $RA,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ADDSI (GET_H_CR (FLD (f_op_B)), SLLSI (FLD (f_u6), 2));
if (FLD (f_F)) {
{
  SI tmp_sC;
  tmp_sC = SLLSI (FLD (f_u6), 2);
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ADDOFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_vbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = ADDCFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_cbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ADD2_L_R_R__RA__RC) : /* add2$_L$F $RA,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ADDSI (GET_H_CR (FLD (f_op_B)), SLLSI (GET_H_CR (FLD (f_op_C)), 2));
if (FLD (f_F)) {
{
  SI tmp_sC;
  tmp_sC = SLLSI (GET_H_CR (FLD (f_op_C)), 2);
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ADDOFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_vbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = ADDCFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_cbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ADD2_CC__RA__RC) : /* add2$Qcondi$F $RB,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = ADDSI (GET_H_CR (FLD (f_op_B)), SLLSI (GET_H_CR (FLD (f_op_C)), 2));
if (FLD (f_F)) {
{
  SI tmp_sC;
  tmp_sC = SLLSI (GET_H_CR (FLD (f_op_C)), 2);
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ADDOFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_vbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = ADDCFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_cbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_I16_GO_ADD2_S_GO) : /* add2$_S $R_b,$R_b,$R_c */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
((void) 0); /*nop*/
  tmp_B = GET_H_CR16 (FLD (f_op__b));
  tmp_C = GET_H_CR16 (FLD (f_op__c));
  {
    SI opval = ADDSI (tmp_B, SLLSI (tmp_C, 2));
    SET_H_CR16 (FLD (f_op__b), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ADD3_L_S12__RA_) : /* add3$_L$F $RB,$RB,$s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ADDSI (GET_H_CR (FLD (f_op_B)), SLLSI (FLD (f_s12), 3));
if (FLD (f_F)) {
{
  SI tmp_sC;
  tmp_sC = SLLSI (FLD (f_s12), 3);
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ADDOFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_vbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = ADDCFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_cbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ADD3_CCU6__RA_) : /* add3$Qcondi$F $RB,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = ADDSI (GET_H_CR (FLD (f_op_B)), SLLSI (FLD (f_u6), 3));
if (FLD (f_F)) {
{
  SI tmp_sC;
  tmp_sC = SLLSI (FLD (f_u6), 3);
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ADDOFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_vbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = ADDCFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_cbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ADD3_L_U6__RA_) : /* add3$_L$F $RA,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ADDSI (GET_H_CR (FLD (f_op_B)), SLLSI (FLD (f_u6), 3));
if (FLD (f_F)) {
{
  SI tmp_sC;
  tmp_sC = SLLSI (FLD (f_u6), 3);
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ADDOFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_vbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = ADDCFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_cbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ADD3_L_R_R__RA__RC) : /* add3$_L$F $RA,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ADDSI (GET_H_CR (FLD (f_op_B)), SLLSI (GET_H_CR (FLD (f_op_C)), 3));
if (FLD (f_F)) {
{
  SI tmp_sC;
  tmp_sC = SLLSI (GET_H_CR (FLD (f_op_C)), 3);
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ADDOFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_vbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = ADDCFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_cbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ADD3_CC__RA__RC) : /* add3$Qcondi$F $RB,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = ADDSI (GET_H_CR (FLD (f_op_B)), SLLSI (GET_H_CR (FLD (f_op_C)), 3));
if (FLD (f_F)) {
{
  SI tmp_sC;
  tmp_sC = SLLSI (GET_H_CR (FLD (f_op_C)), 3);
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ADDOFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_vbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = ADDCFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_cbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_I16_GO_ADD3_S_GO) : /* add3$_S $R_b,$R_b,$R_c */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
((void) 0); /*nop*/
  tmp_B = GET_H_CR16 (FLD (f_op__b));
  tmp_C = GET_H_CR16 (FLD (f_op__c));
  {
    SI opval = ADDSI (tmp_B, SLLSI (tmp_C, 3));
    SET_H_CR16 (FLD (f_op__b), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SUB1_L_S12__RA_) : /* sub1$_L$F $RB,$RB,$s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = SUBSI (GET_H_CR (FLD (f_op_B)), SLLSI (FLD (f_s12), 1));
if (FLD (f_F)) {
{
  SI tmp_sC;
  tmp_sC = SLLSI (FLD (f_s12), 1);
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = SUBOFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_vbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = SUBCFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_cbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SUB1_CCU6__RA_) : /* sub1$Qcondi$F $RB,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = SUBSI (GET_H_CR (FLD (f_op_B)), SLLSI (FLD (f_u6), 1));
if (FLD (f_F)) {
{
  SI tmp_sC;
  tmp_sC = SLLSI (FLD (f_u6), 1);
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = SUBOFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_vbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = SUBCFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_cbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SUB1_L_U6__RA_) : /* sub1$_L$F $RA,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = SUBSI (GET_H_CR (FLD (f_op_B)), SLLSI (FLD (f_u6), 1));
if (FLD (f_F)) {
{
  SI tmp_sC;
  tmp_sC = SLLSI (FLD (f_u6), 1);
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = SUBOFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_vbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = SUBCFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_cbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SUB1_L_R_R__RA__RC) : /* sub1$_L$F $RA,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = SUBSI (GET_H_CR (FLD (f_op_B)), SLLSI (GET_H_CR (FLD (f_op_C)), 1));
if (FLD (f_F)) {
{
  SI tmp_sC;
  tmp_sC = SLLSI (GET_H_CR (FLD (f_op_C)), 1);
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = SUBOFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_vbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = SUBCFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_cbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SUB1_CC__RA__RC) : /* sub1$Qcondi$F $RB,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = SUBSI (GET_H_CR (FLD (f_op_B)), SLLSI (GET_H_CR (FLD (f_op_C)), 1));
if (FLD (f_F)) {
{
  SI tmp_sC;
  tmp_sC = SLLSI (GET_H_CR (FLD (f_op_C)), 1);
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = SUBOFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_vbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = SUBCFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_cbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SUB2_L_S12__RA_) : /* sub2$_L$F $RB,$RB,$s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = SUBSI (GET_H_CR (FLD (f_op_B)), SLLSI (FLD (f_s12), 2));
if (FLD (f_F)) {
{
  SI tmp_sC;
  tmp_sC = SLLSI (FLD (f_s12), 2);
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = SUBOFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_vbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = SUBCFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_cbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SUB2_CCU6__RA_) : /* sub2$Qcondi$F $RB,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = SUBSI (GET_H_CR (FLD (f_op_B)), SLLSI (FLD (f_u6), 2));
if (FLD (f_F)) {
{
  SI tmp_sC;
  tmp_sC = SLLSI (FLD (f_u6), 2);
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = SUBOFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_vbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = SUBCFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_cbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SUB2_L_U6__RA_) : /* sub2$_L$F $RA,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = SUBSI (GET_H_CR (FLD (f_op_B)), SLLSI (FLD (f_u6), 2));
if (FLD (f_F)) {
{
  SI tmp_sC;
  tmp_sC = SLLSI (FLD (f_u6), 2);
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = SUBOFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_vbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = SUBCFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_cbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SUB2_L_R_R__RA__RC) : /* sub2$_L$F $RA,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = SUBSI (GET_H_CR (FLD (f_op_B)), SLLSI (GET_H_CR (FLD (f_op_C)), 2));
if (FLD (f_F)) {
{
  SI tmp_sC;
  tmp_sC = SLLSI (GET_H_CR (FLD (f_op_C)), 2);
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = SUBOFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_vbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = SUBCFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_cbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SUB2_CC__RA__RC) : /* sub2$Qcondi$F $RB,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = SUBSI (GET_H_CR (FLD (f_op_B)), SLLSI (GET_H_CR (FLD (f_op_C)), 2));
if (FLD (f_F)) {
{
  SI tmp_sC;
  tmp_sC = SLLSI (GET_H_CR (FLD (f_op_C)), 2);
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = SUBOFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_vbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = SUBCFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_cbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SUB3_L_S12__RA_) : /* sub3$_L$F $RB,$RB,$s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = SUBSI (GET_H_CR (FLD (f_op_B)), SLLSI (FLD (f_s12), 3));
if (FLD (f_F)) {
{
  SI tmp_sC;
  tmp_sC = SLLSI (FLD (f_s12), 3);
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = SUBOFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_vbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = SUBCFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_cbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SUB3_CCU6__RA_) : /* sub3$Qcondi$F $RB,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = SUBSI (GET_H_CR (FLD (f_op_B)), SLLSI (FLD (f_u6), 3));
if (FLD (f_F)) {
{
  SI tmp_sC;
  tmp_sC = SLLSI (FLD (f_u6), 3);
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = SUBOFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_vbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = SUBCFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_cbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SUB3_L_U6__RA_) : /* sub3$_L$F $RA,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = SUBSI (GET_H_CR (FLD (f_op_B)), SLLSI (FLD (f_u6), 3));
if (FLD (f_F)) {
{
  SI tmp_sC;
  tmp_sC = SLLSI (FLD (f_u6), 3);
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = SUBOFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_vbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = SUBCFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_cbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SUB3_L_R_R__RA__RC) : /* sub3$_L$F $RA,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = SUBSI (GET_H_CR (FLD (f_op_B)), SLLSI (GET_H_CR (FLD (f_op_C)), 3));
if (FLD (f_F)) {
{
  SI tmp_sC;
  tmp_sC = SLLSI (GET_H_CR (FLD (f_op_C)), 3);
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = SUBOFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_vbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = SUBCFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_cbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SUB3_CC__RA__RC) : /* sub3$Qcondi$F $RB,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = SUBSI (GET_H_CR (FLD (f_op_B)), SLLSI (GET_H_CR (FLD (f_op_C)), 3));
if (FLD (f_F)) {
{
  SI tmp_sC;
  tmp_sC = SLLSI (GET_H_CR (FLD (f_op_C)), 3);
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = SUBOFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_vbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = SUBCFSI (GET_H_CR (FLD (f_op_B)), tmp_sC, 0);
    CPU (h_cbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MPY_L_S12__RA_) : /* mpy$_L$F $RB,$RB,$s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (MULSI (GET_H_CR (FLD (f_op_B)), FLD (f_s12)));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = NEDI (EXTSIDI (tmp_result), MULDI (EXTSIDI (GET_H_CR (FLD (f_op_B))), EXTSIDI (FLD (f_s12))));
    CPU (h_vbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MPY_CCU6__RA_) : /* mpy$Qcondi$F $RB,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (MULSI (GET_H_CR (FLD (f_op_B)), FLD (f_u6)));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = NEDI (EXTSIDI (tmp_result), MULDI (EXTSIDI (GET_H_CR (FLD (f_op_B))), EXTSIDI (FLD (f_u6))));
    CPU (h_vbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MPY_L_U6__RA_) : /* mpy$_L$F $RA,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (MULSI (GET_H_CR (FLD (f_op_B)), FLD (f_u6)));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = NEDI (EXTSIDI (tmp_result), MULDI (EXTSIDI (GET_H_CR (FLD (f_op_B))), EXTSIDI (FLD (f_u6))));
    CPU (h_vbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MPY_L_R_R__RA__RC) : /* mpy$_L$F $RA,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (MULSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C))));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = NEDI (EXTSIDI (tmp_result), MULDI (EXTSIDI (GET_H_CR (FLD (f_op_B))), EXTSIDI (GET_H_CR (FLD (f_op_C)))));
    CPU (h_vbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MPY_CC__RA__RC) : /* mpy$Qcondi$F $RB,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (MULSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C))));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = NEDI (EXTSIDI (tmp_result), MULDI (EXTSIDI (GET_H_CR (FLD (f_op_B))), EXTSIDI (GET_H_CR (FLD (f_op_C)))));
    CPU (h_vbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MPYH_L_S12__RA_) : /* mpyh$_L$F $RB,$RB,$s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (SUBWORDDISI (MULDI (EXTSIDI (GET_H_CR (FLD (f_op_B))), EXTSIDI (FLD (f_s12))), 0));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = 0;
    CPU (h_vbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MPYH_CCU6__RA_) : /* mpyh$Qcondi$F $RB,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (SUBWORDDISI (MULDI (EXTSIDI (GET_H_CR (FLD (f_op_B))), EXTSIDI (FLD (f_u6))), 0));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = 0;
    CPU (h_vbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MPYH_L_U6__RA_) : /* mpyh$_L$F $RA,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (SUBWORDDISI (MULDI (EXTSIDI (GET_H_CR (FLD (f_op_B))), EXTSIDI (FLD (f_u6))), 0));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = 0;
    CPU (h_vbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MPYH_L_R_R__RA__RC) : /* mpyh$_L$F $RA,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (SUBWORDDISI (MULDI (EXTSIDI (GET_H_CR (FLD (f_op_B))), EXTSIDI (GET_H_CR (FLD (f_op_C)))), 0));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = 0;
    CPU (h_vbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MPYH_CC__RA__RC) : /* mpyh$Qcondi$F $RB,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (SUBWORDDISI (MULDI (EXTSIDI (GET_H_CR (FLD (f_op_B))), EXTSIDI (GET_H_CR (FLD (f_op_C)))), 0));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = 0;
    CPU (h_vbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MPYHU_L_S12__RA_) : /* mpyhu$_L$F $RB,$RB,$s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (SUBWORDDISI (MULDI (ZEXTSIDI (GET_H_CR (FLD (f_op_B))), ZEXTSIDI (FLD (f_s12))), 0));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = 0;
    CPU (h_vbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MPYHU_CCU6__RA_) : /* mpyhu$Qcondi$F $RB,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (SUBWORDDISI (MULDI (ZEXTSIDI (GET_H_CR (FLD (f_op_B))), ZEXTSIDI (FLD (f_u6))), 0));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = 0;
    CPU (h_vbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MPYHU_L_U6__RA_) : /* mpyhu$_L$F $RA,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (SUBWORDDISI (MULDI (ZEXTSIDI (GET_H_CR (FLD (f_op_B))), ZEXTSIDI (FLD (f_u6))), 0));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = 0;
    CPU (h_vbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MPYHU_L_R_R__RA__RC) : /* mpyhu$_L$F $RA,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (SUBWORDDISI (MULDI (ZEXTSIDI (GET_H_CR (FLD (f_op_B))), ZEXTSIDI (GET_H_CR (FLD (f_op_C)))), 0));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = 0;
    CPU (h_vbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MPYHU_CC__RA__RC) : /* mpyhu$Qcondi$F $RB,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (SUBWORDDISI (MULDI (ZEXTSIDI (GET_H_CR (FLD (f_op_B))), ZEXTSIDI (GET_H_CR (FLD (f_op_C)))), 0));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = 0;
    CPU (h_vbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MPYU_L_S12__RA_) : /* mpyu$_L$F $RB,$RB,$s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (MULSI (GET_H_CR (FLD (f_op_B)), FLD (f_s12)));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = NEDI (ZEXTSIDI (tmp_result), MULDI (ZEXTSIDI (GET_H_CR (FLD (f_op_B))), ZEXTSIDI (FLD (f_s12))));
    CPU (h_vbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MPYU_CCU6__RA_) : /* mpyu$Qcondi$F $RB,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (MULSI (GET_H_CR (FLD (f_op_B)), FLD (f_u6)));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = NEDI (ZEXTSIDI (tmp_result), MULDI (ZEXTSIDI (GET_H_CR (FLD (f_op_B))), ZEXTSIDI (FLD (f_u6))));
    CPU (h_vbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MPYU_L_U6__RA_) : /* mpyu$_L$F $RA,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (MULSI (GET_H_CR (FLD (f_op_B)), FLD (f_u6)));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = NEDI (ZEXTSIDI (tmp_result), MULDI (ZEXTSIDI (GET_H_CR (FLD (f_op_B))), ZEXTSIDI (FLD (f_u6))));
    CPU (h_vbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MPYU_L_R_R__RA__RC) : /* mpyu$_L$F $RA,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (MULSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C))));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = NEDI (ZEXTSIDI (tmp_result), MULDI (ZEXTSIDI (GET_H_CR (FLD (f_op_B))), ZEXTSIDI (GET_H_CR (FLD (f_op_C)))));
    CPU (h_vbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MPYU_CC__RA__RC) : /* mpyu$Qcondi$F $RB,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (MULSI (GET_H_CR (FLD (f_op_B)), GET_H_CR (FLD (f_op_C))));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = NEDI (ZEXTSIDI (tmp_result), MULDI (ZEXTSIDI (GET_H_CR (FLD (f_op_B))), ZEXTSIDI (GET_H_CR (FLD (f_op_C)))));
    CPU (h_vbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_J_L_R_R___RC_NOILINK_) : /* j$_L$F0 [$RC_noilink] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 22);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
{
  SI tmp_result;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  switch (FLD (f_op_Cj))
  {
  case 29 : {
if (FLD (f_F)) {
  {
    SI opval = GET_H_AUXR (((UINT) 11));
    SET_H_STATUS32 (((UINT) 0), opval);
    written |= (1 << 23);
    TRACE_RESULT (current_cpu, abuf, "status32", 'x', opval);
  }
} else {
cgen_rtx_error (current_cpu, "invalid insn");
}
}
    break;
  case 30 : {
if (FLD (f_F)) {
  {
    SI opval = GET_H_AUXR (((UINT) 12));
    SET_H_STATUS32 (((UINT) 0), opval);
    written |= (1 << 23);
    TRACE_RESULT (current_cpu, abuf, "status32", 'x', opval);
  }
} else {
cgen_rtx_error (current_cpu, "invalid insn");
}
}
    break;
  default : {
if (FLD (f_F)) {
cgen_rtx_error (current_cpu, "invalid insn");
}
}
    break;
  }
  {
    USI opval = GET_H_NOILINK (FLD (f_op_Cj));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 24);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}
 else if (0) {
{
  SI tmp_result;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  switch (FLD (f_op_Cj))
  {
  case 29 : {
if (FLD (f_F)) {
  {
    SI opval = GET_H_AUXR (((UINT) 11));
    SET_H_STATUS32 (((UINT) 0), opval);
    written |= (1 << 23);
    TRACE_RESULT (current_cpu, abuf, "status32", 'x', opval);
  }
} else {
cgen_rtx_error (current_cpu, "invalid insn");
}
}
    break;
  case 30 : {
if (FLD (f_F)) {
  {
    SI opval = GET_H_AUXR (((UINT) 12));
    SET_H_STATUS32 (((UINT) 0), opval);
    written |= (1 << 23);
    TRACE_RESULT (current_cpu, abuf, "status32", 'x', opval);
  }
} else {
cgen_rtx_error (current_cpu, "invalid insn");
}
}
    break;
  default : {
if (FLD (f_F)) {
cgen_rtx_error (current_cpu, "invalid insn");
}
}
    break;
  }
  {
    USI opval = GET_H_NOILINK (FLD (f_op_Cj));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 24);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 20);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 21);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 24);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
{
  SI tmp_result;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  switch (FLD (f_op_Cj))
  {
  case 29 : {
if (FLD (f_F)) {
  {
    SI opval = GET_H_AUXR (((UINT) 11));
    SET_H_STATUS32 (((UINT) 0), opval);
    written |= (1 << 23);
    TRACE_RESULT (current_cpu, abuf, "status32", 'x', opval);
  }
} else {
cgen_rtx_error (current_cpu, "invalid insn");
}
}
    break;
  case 30 : {
if (FLD (f_F)) {
  {
    SI opval = GET_H_AUXR (((UINT) 12));
    SET_H_STATUS32 (((UINT) 0), opval);
    written |= (1 << 23);
    TRACE_RESULT (current_cpu, abuf, "status32", 'x', opval);
  }
} else {
cgen_rtx_error (current_cpu, "invalid insn");
}
}
    break;
  default : {
if (FLD (f_F)) {
cgen_rtx_error (current_cpu, "invalid insn");
}
}
    break;
  }
  {
    USI opval = GET_H_NOILINK (FLD (f_op_Cj));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 24);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_J_CC___RC_NOILINK_) : /* j$Qcondi$F0 [$RC_noilink] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 20);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 23);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
{
  SI tmp_result;
((void) 0); /*nop*/
{
  switch (FLD (f_op_Cj))
  {
  case 29 : {
if (FLD (f_F)) {
  {
    SI opval = GET_H_AUXR (((UINT) 11));
    SET_H_STATUS32 (((UINT) 0), opval);
    written |= (1 << 24);
    TRACE_RESULT (current_cpu, abuf, "status32", 'x', opval);
  }
} else {
cgen_rtx_error (current_cpu, "invalid insn");
}
}
    break;
  case 30 : {
if (FLD (f_F)) {
  {
    SI opval = GET_H_AUXR (((UINT) 12));
    SET_H_STATUS32 (((UINT) 0), opval);
    written |= (1 << 24);
    TRACE_RESULT (current_cpu, abuf, "status32", 'x', opval);
  }
} else {
cgen_rtx_error (current_cpu, "invalid insn");
}
}
    break;
  default : {
if (FLD (f_F)) {
cgen_rtx_error (current_cpu, "invalid insn");
}
}
    break;
  }
  {
    USI opval = GET_H_NOILINK (FLD (f_op_Cj));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 25);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}
 else if (0) {
{
  SI tmp_result;
((void) 0); /*nop*/
{
  switch (FLD (f_op_Cj))
  {
  case 29 : {
if (FLD (f_F)) {
  {
    SI opval = GET_H_AUXR (((UINT) 11));
    SET_H_STATUS32 (((UINT) 0), opval);
    written |= (1 << 24);
    TRACE_RESULT (current_cpu, abuf, "status32", 'x', opval);
  }
} else {
cgen_rtx_error (current_cpu, "invalid insn");
}
}
    break;
  case 30 : {
if (FLD (f_F)) {
  {
    SI opval = GET_H_AUXR (((UINT) 12));
    SET_H_STATUS32 (((UINT) 0), opval);
    written |= (1 << 24);
    TRACE_RESULT (current_cpu, abuf, "status32", 'x', opval);
  }
} else {
cgen_rtx_error (current_cpu, "invalid insn");
}
}
    break;
  default : {
if (FLD (f_F)) {
cgen_rtx_error (current_cpu, "invalid insn");
}
}
    break;
  }
  {
    USI opval = GET_H_NOILINK (FLD (f_op_Cj));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 25);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 21);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 22);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 25);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
{
  SI tmp_result;
((void) 0); /*nop*/
{
  switch (FLD (f_op_Cj))
  {
  case 29 : {
if (FLD (f_F)) {
  {
    SI opval = GET_H_AUXR (((UINT) 11));
    SET_H_STATUS32 (((UINT) 0), opval);
    written |= (1 << 24);
    TRACE_RESULT (current_cpu, abuf, "status32", 'x', opval);
  }
} else {
cgen_rtx_error (current_cpu, "invalid insn");
}
}
    break;
  case 30 : {
if (FLD (f_F)) {
  {
    SI opval = GET_H_AUXR (((UINT) 12));
    SET_H_STATUS32 (((UINT) 0), opval);
    written |= (1 << 24);
    TRACE_RESULT (current_cpu, abuf, "status32", 'x', opval);
  }
} else {
cgen_rtx_error (current_cpu, "invalid insn");
}
}
    break;
  default : {
if (FLD (f_F)) {
cgen_rtx_error (current_cpu, "invalid insn");
}
}
    break;
  }
  {
    USI opval = GET_H_NOILINK (FLD (f_op_Cj));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 25);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_J_L_R_R___RC_ILINK_) : /* j$_L$F1F [$RC_ilink] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 22);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
{
  SI tmp_result;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  switch (FLD (f_op_Cj))
  {
  case 29 : {
if (FLD (f_F)) {
  {
    SI opval = GET_H_AUXR (((UINT) 11));
    SET_H_STATUS32 (((UINT) 0), opval);
    written |= (1 << 23);
    TRACE_RESULT (current_cpu, abuf, "status32", 'x', opval);
  }
} else {
cgen_rtx_error (current_cpu, "invalid insn");
}
}
    break;
  case 30 : {
if (FLD (f_F)) {
  {
    SI opval = GET_H_AUXR (((UINT) 12));
    SET_H_STATUS32 (((UINT) 0), opval);
    written |= (1 << 23);
    TRACE_RESULT (current_cpu, abuf, "status32", 'x', opval);
  }
} else {
cgen_rtx_error (current_cpu, "invalid insn");
}
}
    break;
  default : {
if (FLD (f_F)) {
cgen_rtx_error (current_cpu, "invalid insn");
}
}
    break;
  }
  {
    USI opval = GET_H_ILINKX (FLD (f_op_Cj));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 24);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}
 else if (0) {
{
  SI tmp_result;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  switch (FLD (f_op_Cj))
  {
  case 29 : {
if (FLD (f_F)) {
  {
    SI opval = GET_H_AUXR (((UINT) 11));
    SET_H_STATUS32 (((UINT) 0), opval);
    written |= (1 << 23);
    TRACE_RESULT (current_cpu, abuf, "status32", 'x', opval);
  }
} else {
cgen_rtx_error (current_cpu, "invalid insn");
}
}
    break;
  case 30 : {
if (FLD (f_F)) {
  {
    SI opval = GET_H_AUXR (((UINT) 12));
    SET_H_STATUS32 (((UINT) 0), opval);
    written |= (1 << 23);
    TRACE_RESULT (current_cpu, abuf, "status32", 'x', opval);
  }
} else {
cgen_rtx_error (current_cpu, "invalid insn");
}
}
    break;
  default : {
if (FLD (f_F)) {
cgen_rtx_error (current_cpu, "invalid insn");
}
}
    break;
  }
  {
    USI opval = GET_H_ILINKX (FLD (f_op_Cj));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 24);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 20);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 21);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 24);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
{
  SI tmp_result;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  switch (FLD (f_op_Cj))
  {
  case 29 : {
if (FLD (f_F)) {
  {
    SI opval = GET_H_AUXR (((UINT) 11));
    SET_H_STATUS32 (((UINT) 0), opval);
    written |= (1 << 23);
    TRACE_RESULT (current_cpu, abuf, "status32", 'x', opval);
  }
} else {
cgen_rtx_error (current_cpu, "invalid insn");
}
}
    break;
  case 30 : {
if (FLD (f_F)) {
  {
    SI opval = GET_H_AUXR (((UINT) 12));
    SET_H_STATUS32 (((UINT) 0), opval);
    written |= (1 << 23);
    TRACE_RESULT (current_cpu, abuf, "status32", 'x', opval);
  }
} else {
cgen_rtx_error (current_cpu, "invalid insn");
}
}
    break;
  default : {
if (FLD (f_F)) {
cgen_rtx_error (current_cpu, "invalid insn");
}
}
    break;
  }
  {
    USI opval = GET_H_ILINKX (FLD (f_op_Cj));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 24);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_J_CC___RC_ILINK_) : /* j$Qcondi$F1F [$RC_ilink] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 20);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 23);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
{
  SI tmp_result;
((void) 0); /*nop*/
{
  switch (FLD (f_op_Cj))
  {
  case 29 : {
if (FLD (f_F)) {
  {
    SI opval = GET_H_AUXR (((UINT) 11));
    SET_H_STATUS32 (((UINT) 0), opval);
    written |= (1 << 24);
    TRACE_RESULT (current_cpu, abuf, "status32", 'x', opval);
  }
} else {
cgen_rtx_error (current_cpu, "invalid insn");
}
}
    break;
  case 30 : {
if (FLD (f_F)) {
  {
    SI opval = GET_H_AUXR (((UINT) 12));
    SET_H_STATUS32 (((UINT) 0), opval);
    written |= (1 << 24);
    TRACE_RESULT (current_cpu, abuf, "status32", 'x', opval);
  }
} else {
cgen_rtx_error (current_cpu, "invalid insn");
}
}
    break;
  default : {
if (FLD (f_F)) {
cgen_rtx_error (current_cpu, "invalid insn");
}
}
    break;
  }
  {
    USI opval = GET_H_ILINKX (FLD (f_op_Cj));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 25);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}
 else if (0) {
{
  SI tmp_result;
((void) 0); /*nop*/
{
  switch (FLD (f_op_Cj))
  {
  case 29 : {
if (FLD (f_F)) {
  {
    SI opval = GET_H_AUXR (((UINT) 11));
    SET_H_STATUS32 (((UINT) 0), opval);
    written |= (1 << 24);
    TRACE_RESULT (current_cpu, abuf, "status32", 'x', opval);
  }
} else {
cgen_rtx_error (current_cpu, "invalid insn");
}
}
    break;
  case 30 : {
if (FLD (f_F)) {
  {
    SI opval = GET_H_AUXR (((UINT) 12));
    SET_H_STATUS32 (((UINT) 0), opval);
    written |= (1 << 24);
    TRACE_RESULT (current_cpu, abuf, "status32", 'x', opval);
  }
} else {
cgen_rtx_error (current_cpu, "invalid insn");
}
}
    break;
  default : {
if (FLD (f_F)) {
cgen_rtx_error (current_cpu, "invalid insn");
}
}
    break;
  }
  {
    USI opval = GET_H_ILINKX (FLD (f_op_Cj));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 25);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 21);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 22);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 25);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
{
  SI tmp_result;
((void) 0); /*nop*/
{
  switch (FLD (f_op_Cj))
  {
  case 29 : {
if (FLD (f_F)) {
  {
    SI opval = GET_H_AUXR (((UINT) 11));
    SET_H_STATUS32 (((UINT) 0), opval);
    written |= (1 << 24);
    TRACE_RESULT (current_cpu, abuf, "status32", 'x', opval);
  }
} else {
cgen_rtx_error (current_cpu, "invalid insn");
}
}
    break;
  case 30 : {
if (FLD (f_F)) {
  {
    SI opval = GET_H_AUXR (((UINT) 12));
    SET_H_STATUS32 (((UINT) 0), opval);
    written |= (1 << 24);
    TRACE_RESULT (current_cpu, abuf, "status32", 'x', opval);
  }
} else {
cgen_rtx_error (current_cpu, "invalid insn");
}
}
    break;
  default : {
if (FLD (f_F)) {
cgen_rtx_error (current_cpu, "invalid insn");
}
}
    break;
  }
  {
    USI opval = GET_H_ILINKX (FLD (f_op_Cj));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 25);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_J_L_S12_) : /* j$_L$F0 $s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
{
  SI tmp_result;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
if (FLD (f_F)) {
cgen_rtx_error (current_cpu, "invalid insn");
}
  {
    USI opval = FLD (f_s12);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}
 else if (0) {
{
  SI tmp_result;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
if (FLD (f_F)) {
cgen_rtx_error (current_cpu, "invalid insn");
}
  {
    USI opval = FLD (f_s12);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
{
  SI tmp_result;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
if (FLD (f_F)) {
cgen_rtx_error (current_cpu, "invalid insn");
}
  {
    USI opval = FLD (f_s12);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_J_CCU6_) : /* j$Qcondi$F0 $U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
{
  SI tmp_result;
((void) 0); /*nop*/
{
if (FLD (f_F)) {
cgen_rtx_error (current_cpu, "invalid insn");
}
  {
    USI opval = FLD (f_u6);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 20);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}
 else if (0) {
{
  SI tmp_result;
((void) 0); /*nop*/
{
if (FLD (f_F)) {
cgen_rtx_error (current_cpu, "invalid insn");
}
  {
    USI opval = FLD (f_u6);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 20);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 20);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
{
  SI tmp_result;
((void) 0); /*nop*/
{
if (FLD (f_F)) {
cgen_rtx_error (current_cpu, "invalid insn");
}
  {
    USI opval = FLD (f_u6);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 20);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_J_L_U6_) : /* j$_L$F0 $U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
{
  SI tmp_result;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
if (FLD (f_F)) {
cgen_rtx_error (current_cpu, "invalid insn");
}
  {
    USI opval = FLD (f_u6);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}
 else if (0) {
{
  SI tmp_result;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
if (FLD (f_F)) {
cgen_rtx_error (current_cpu, "invalid insn");
}
  {
    USI opval = FLD (f_u6);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
{
  SI tmp_result;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
if (FLD (f_F)) {
cgen_rtx_error (current_cpu, "invalid insn");
}
  {
    USI opval = FLD (f_u6);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_J_S) : /* j$_S [$R_b] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_mov_s_r_u7.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
{
  SI tmp_result;
((void) 0); /*nop*/
{
((void) 0); /*nop*/
  {
    USI opval = GET_H_CR16 (FLD (f_op__b));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}
 else if (0) {
{
  SI tmp_result;
((void) 0); /*nop*/
{
((void) 0); /*nop*/
  {
    USI opval = GET_H_CR16 (FLD (f_op__b));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
{
  SI tmp_result;
((void) 0); /*nop*/
{
((void) 0); /*nop*/
  {
    USI opval = GET_H_CR16 (FLD (f_op__b));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_J_S__S) : /* j$_S [$R31] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.fmt_empty.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
{
  SI tmp_result;
((void) 0); /*nop*/
{
((void) 0); /*nop*/
  {
    USI opval = GET_H_R31 ();
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}
 else if (0) {
{
  SI tmp_result;
((void) 0); /*nop*/
{
((void) 0); /*nop*/
  {
    USI opval = GET_H_R31 ();
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
{
  SI tmp_result;
((void) 0); /*nop*/
{
((void) 0); /*nop*/
  {
    USI opval = GET_H_R31 ();
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_J_SEQ__S) : /* jeq$_S [$R31] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.fmt_empty.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
{
  SI tmp_result;
((void) 0); /*nop*/
if (NEBI (CPU (h_zbit), 0)) {
{
((void) 0); /*nop*/
  {
    USI opval = GET_H_R31 ();
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}
}
 else if (0) {
{
  SI tmp_result;
((void) 0); /*nop*/
if (NEBI (CPU (h_zbit), 0)) {
{
((void) 0); /*nop*/
  {
    USI opval = GET_H_R31 ();
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
{
  SI tmp_result;
((void) 0); /*nop*/
if (NEBI (CPU (h_zbit), 0)) {
{
((void) 0); /*nop*/
  {
    USI opval = GET_H_R31 ();
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_J_SNE__S) : /* jne$_S [$R31] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.fmt_empty.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
{
  SI tmp_result;
((void) 0); /*nop*/
if (EQBI (CPU (h_zbit), 0)) {
{
((void) 0); /*nop*/
  {
    USI opval = GET_H_R31 ();
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}
}
 else if (0) {
{
  SI tmp_result;
((void) 0); /*nop*/
if (EQBI (CPU (h_zbit), 0)) {
{
((void) 0); /*nop*/
  {
    USI opval = GET_H_R31 ();
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
{
  SI tmp_result;
((void) 0); /*nop*/
if (EQBI (CPU (h_zbit), 0)) {
{
((void) 0); /*nop*/
  {
    USI opval = GET_H_R31 ();
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_J_L_S12_D_) : /* j$_L$F0.d $s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
{
  SI tmp_result;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  {
    USI opval = FLD (f_s12);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}
 else if (1) {
{
  SI tmp_result;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  {
    USI opval = FLD (f_s12);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
{
  SI tmp_result;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  {
    USI opval = FLD (f_s12);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_J_CCU6_D_) : /* j$Qcondi$F0.d $U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
{
  SI tmp_result;
((void) 0); /*nop*/
{
  {
    USI opval = FLD (f_u6);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}
 else if (1) {
{
  SI tmp_result;
((void) 0); /*nop*/
{
  {
    USI opval = FLD (f_u6);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
{
  SI tmp_result;
((void) 0); /*nop*/
{
  {
    USI opval = FLD (f_u6);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_J_L_U6_D_) : /* j$_L$F0.d $U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
{
  SI tmp_result;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  {
    USI opval = FLD (f_u6);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}
 else if (1) {
{
  SI tmp_result;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  {
    USI opval = FLD (f_u6);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
{
  SI tmp_result;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  {
    USI opval = FLD (f_u6);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_J_L_R_R_D___RC_) : /* j$_L$F0.d [$RC] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_st_abs.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
{
  SI tmp_result;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  {
    USI opval = GET_H_CR (FLD (f_op_C));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}
 else if (1) {
{
  SI tmp_result;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  {
    USI opval = GET_H_CR (FLD (f_op_C));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
{
  SI tmp_result;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  {
    USI opval = GET_H_CR (FLD (f_op_C));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_J_CC_D___RC_) : /* j$Qcondi$F0.d [$RC] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
{
  SI tmp_result;
((void) 0); /*nop*/
{
  {
    USI opval = GET_H_CR (FLD (f_op_C));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 20);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}
 else if (1) {
{
  SI tmp_result;
((void) 0); /*nop*/
{
  {
    USI opval = GET_H_CR (FLD (f_op_C));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 20);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 20);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
{
  SI tmp_result;
((void) 0); /*nop*/
{
  {
    USI opval = GET_H_CR (FLD (f_op_C));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 20);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_J_S_D) : /* j$_S.d [$R_b] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_mov_s_r_u7.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
{
  SI tmp_result;
((void) 0); /*nop*/
{
  {
    USI opval = GET_H_CR16 (FLD (f_op__b));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}
 else if (1) {
{
  SI tmp_result;
((void) 0); /*nop*/
{
  {
    USI opval = GET_H_CR16 (FLD (f_op__b));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
{
  SI tmp_result;
((void) 0); /*nop*/
{
  {
    USI opval = GET_H_CR16 (FLD (f_op__b));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_J_S__S_D) : /* j$_S.d [$R31] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.fmt_empty.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
{
  SI tmp_result;
((void) 0); /*nop*/
{
  {
    USI opval = GET_H_R31 ();
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}
 else if (1) {
{
  SI tmp_result;
((void) 0); /*nop*/
{
  {
    USI opval = GET_H_R31 ();
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
{
  SI tmp_result;
((void) 0); /*nop*/
{
  {
    USI opval = GET_H_R31 ();
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_JL_L_S12_) : /* jl$_L$F0 $s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
{
  SI tmp_result;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  {
    USI opval = FLD (f_s12);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
  {
    SI opval = ADDSI (pc, 4);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}
}
 else if (0) {
{
  SI tmp_result;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  {
    USI opval = FLD (f_s12);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
  {
    SI opval = ADDSI (pc, 4);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
{
  SI tmp_result;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  {
    USI opval = FLD (f_s12);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
  {
    SI opval = ADDSI (pc, 4);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_JL_CCU6_) : /* jl$Qcondi$F0 $U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
{
  SI tmp_result;
((void) 0); /*nop*/
{
  {
    USI opval = FLD (f_u6);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 20);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
  {
    SI opval = ADDSI (pc, 4);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}
}
 else if (0) {
{
  SI tmp_result;
((void) 0); /*nop*/
{
  {
    USI opval = FLD (f_u6);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 20);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
  {
    SI opval = ADDSI (pc, 4);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 20);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
{
  SI tmp_result;
((void) 0); /*nop*/
{
  {
    USI opval = FLD (f_u6);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 20);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
  {
    SI opval = ADDSI (pc, 4);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_JL_L_U6_) : /* jl$_L$F0 $U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
{
  SI tmp_result;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  {
    USI opval = FLD (f_u6);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
  {
    SI opval = ADDSI (pc, 4);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}
}
 else if (0) {
{
  SI tmp_result;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  {
    USI opval = FLD (f_u6);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
  {
    SI opval = ADDSI (pc, 4);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
{
  SI tmp_result;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  {
    USI opval = FLD (f_u6);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
  {
    SI opval = ADDSI (pc, 4);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_JL_S) : /* jl$_S [$R_b] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_mov_s_r_u7.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
{
  SI tmp_result;
((void) 0); /*nop*/
{
  {
    USI opval = GET_H_CR16 (FLD (f_op__b));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
  {
    SI opval = ADDSI (pc, 2);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}
}
 else if (0) {
{
  SI tmp_result;
((void) 0); /*nop*/
{
  {
    USI opval = GET_H_CR16 (FLD (f_op__b));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
  {
    SI opval = ADDSI (pc, 2);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
{
  SI tmp_result;
((void) 0); /*nop*/
{
  {
    USI opval = GET_H_CR16 (FLD (f_op__b));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
  {
    SI opval = ADDSI (pc, 2);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_JL_L_R_R___RC_NOILINK_) : /* jl$_L$F0 [$RC_noilink] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
{
  SI tmp_result;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  {
    USI opval = GET_H_NOILINK (FLD (f_op_Cj));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 20);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
  {
    SI opval = ADDSI (pc, ((EQSI (FLD (f_op_C), 62)) ? (8) : (4)));
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}
}
 else if (0) {
{
  SI tmp_result;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  {
    USI opval = GET_H_NOILINK (FLD (f_op_Cj));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 20);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
  {
    SI opval = ADDSI (pc, ((EQSI (FLD (f_op_C), 62)) ? (8) : (4)));
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 20);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
{
  SI tmp_result;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  {
    USI opval = GET_H_NOILINK (FLD (f_op_Cj));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 20);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
  {
    SI opval = ADDSI (pc, ((EQSI (FLD (f_op_C), 62)) ? (8) : (4)));
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_JL_CC___RC_NOILINK_) : /* jl$Qcondi$F0 [$RC_noilink] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 20);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
{
  SI tmp_result;
((void) 0); /*nop*/
{
  {
    USI opval = GET_H_NOILINK (FLD (f_op_Cj));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 21);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
  {
    SI opval = ADDSI (pc, ((EQSI (FLD (f_op_C), 62)) ? (8) : (4)));
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}
}
 else if (0) {
{
  SI tmp_result;
((void) 0); /*nop*/
{
  {
    USI opval = GET_H_NOILINK (FLD (f_op_Cj));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 21);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
  {
    SI opval = ADDSI (pc, ((EQSI (FLD (f_op_C), 62)) ? (8) : (4)));
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 21);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
{
  SI tmp_result;
((void) 0); /*nop*/
{
  {
    USI opval = GET_H_NOILINK (FLD (f_op_Cj));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 21);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
  {
    SI opval = ADDSI (pc, ((EQSI (FLD (f_op_C), 62)) ? (8) : (4)));
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_JL_L_S12_D_) : /* jl$_L$F0.d $s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
{
  SI tmp_result;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  HI tmp_nword;
  tmp_nword = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
if (ANDHI (ANDHI (tmp_nword, SRAHI (tmp_nword, 1)), 40960)) {
  {
    SI opval = ADDSI (pc, 6);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
} else {
  {
    SI opval = ADDSI (pc, 8);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
{
  {
    USI opval = FLD (f_s12);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}
}
 else if (1) {
{
  SI tmp_result;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  HI tmp_nword;
  tmp_nword = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
if (ANDHI (ANDHI (tmp_nword, SRAHI (tmp_nword, 1)), 40960)) {
  {
    SI opval = ADDSI (pc, 6);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
} else {
  {
    SI opval = ADDSI (pc, 8);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
{
  {
    USI opval = FLD (f_s12);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
{
  SI tmp_result;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  HI tmp_nword;
  tmp_nword = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
if (ANDHI (ANDHI (tmp_nword, SRAHI (tmp_nword, 1)), 40960)) {
  {
    SI opval = ADDSI (pc, 6);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
} else {
  {
    SI opval = ADDSI (pc, 8);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
{
  {
    USI opval = FLD (f_s12);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_JL_CCU6_D_) : /* jl$Qcondi$F0.d $U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
{
  SI tmp_result;
((void) 0); /*nop*/
{
  HI tmp_nword;
  tmp_nword = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
if (ANDHI (ANDHI (tmp_nword, SRAHI (tmp_nword, 1)), 40960)) {
  {
    SI opval = ADDSI (pc, 6);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
} else {
  {
    SI opval = ADDSI (pc, 8);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
{
  {
    USI opval = FLD (f_u6);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 20);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}
}
 else if (1) {
{
  SI tmp_result;
((void) 0); /*nop*/
{
  HI tmp_nword;
  tmp_nword = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
if (ANDHI (ANDHI (tmp_nword, SRAHI (tmp_nword, 1)), 40960)) {
  {
    SI opval = ADDSI (pc, 6);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
} else {
  {
    SI opval = ADDSI (pc, 8);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
{
  {
    USI opval = FLD (f_u6);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 20);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 20);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
{
  SI tmp_result;
((void) 0); /*nop*/
{
  HI tmp_nword;
  tmp_nword = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
if (ANDHI (ANDHI (tmp_nword, SRAHI (tmp_nword, 1)), 40960)) {
  {
    SI opval = ADDSI (pc, 6);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
} else {
  {
    SI opval = ADDSI (pc, 8);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
{
  {
    USI opval = FLD (f_u6);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 20);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_JL_L_U6_D_) : /* jl$_L$F0.d $U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
{
  SI tmp_result;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  HI tmp_nword;
  tmp_nword = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
if (ANDHI (ANDHI (tmp_nword, SRAHI (tmp_nword, 1)), 40960)) {
  {
    SI opval = ADDSI (pc, 6);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
} else {
  {
    SI opval = ADDSI (pc, 8);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
{
  {
    USI opval = FLD (f_u6);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}
}
 else if (1) {
{
  SI tmp_result;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  HI tmp_nword;
  tmp_nword = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
if (ANDHI (ANDHI (tmp_nword, SRAHI (tmp_nword, 1)), 40960)) {
  {
    SI opval = ADDSI (pc, 6);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
} else {
  {
    SI opval = ADDSI (pc, 8);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
{
  {
    USI opval = FLD (f_u6);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
{
  SI tmp_result;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  HI tmp_nword;
  tmp_nword = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
if (ANDHI (ANDHI (tmp_nword, SRAHI (tmp_nword, 1)), 40960)) {
  {
    SI opval = ADDSI (pc, 6);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
} else {
  {
    SI opval = ADDSI (pc, 8);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
{
  {
    USI opval = FLD (f_u6);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_JL_L_R_R_D___RC_) : /* jl$_L$F0.d [$RC] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_st_abs.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
{
  SI tmp_result;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  HI tmp_nword;
  tmp_nword = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
if (ANDHI (ANDHI (tmp_nword, SRAHI (tmp_nword, 1)), 40960)) {
  {
    SI opval = ADDSI (pc, 6);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
} else {
  {
    SI opval = ADDSI (pc, 8);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
{
  {
    USI opval = GET_H_CR (FLD (f_op_C));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 20);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}
}
 else if (1) {
{
  SI tmp_result;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  HI tmp_nword;
  tmp_nword = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
if (ANDHI (ANDHI (tmp_nword, SRAHI (tmp_nword, 1)), 40960)) {
  {
    SI opval = ADDSI (pc, 6);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
} else {
  {
    SI opval = ADDSI (pc, 8);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
{
  {
    USI opval = GET_H_CR (FLD (f_op_C));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 20);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 20);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
{
  SI tmp_result;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  HI tmp_nword;
  tmp_nword = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
if (ANDHI (ANDHI (tmp_nword, SRAHI (tmp_nword, 1)), 40960)) {
  {
    SI opval = ADDSI (pc, 6);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
} else {
  {
    SI opval = ADDSI (pc, 8);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
{
  {
    USI opval = GET_H_CR (FLD (f_op_C));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 20);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_JL_CC_D___RC_) : /* jl$Qcondi$F0.d [$RC] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 20);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
{
  SI tmp_result;
((void) 0); /*nop*/
{
  HI tmp_nword;
  tmp_nword = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
if (ANDHI (ANDHI (tmp_nword, SRAHI (tmp_nword, 1)), 40960)) {
  {
    SI opval = ADDSI (pc, 6);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
} else {
  {
    SI opval = ADDSI (pc, 8);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
{
  {
    USI opval = GET_H_CR (FLD (f_op_C));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 21);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}
}
 else if (1) {
{
  SI tmp_result;
((void) 0); /*nop*/
{
  HI tmp_nword;
  tmp_nword = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
if (ANDHI (ANDHI (tmp_nword, SRAHI (tmp_nword, 1)), 40960)) {
  {
    SI opval = ADDSI (pc, 6);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
} else {
  {
    SI opval = ADDSI (pc, 8);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
{
  {
    USI opval = GET_H_CR (FLD (f_op_C));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 21);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 21);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
{
  SI tmp_result;
((void) 0); /*nop*/
{
  HI tmp_nword;
  tmp_nword = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
if (ANDHI (ANDHI (tmp_nword, SRAHI (tmp_nword, 1)), 40960)) {
  {
    SI opval = ADDSI (pc, 6);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
} else {
  {
    SI opval = ADDSI (pc, 8);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
{
  {
    USI opval = GET_H_CR (FLD (f_op_C));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 21);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_JL_S_D) : /* jl$_S.d [$R_b] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_mov_s_r_u7.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
{
  SI tmp_result;
((void) 0); /*nop*/
{
  HI tmp_nword;
  tmp_nword = GETMEMHI (current_cpu, pc, ADDSI (pc, 2));
if (ANDHI (ANDHI (tmp_nword, SRAHI (tmp_nword, 1)), 40960)) {
  {
    SI opval = ADDSI (pc, 4);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
} else {
  {
    SI opval = ADDSI (pc, 6);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
{
  {
    USI opval = GET_H_CR16 (FLD (f_op__b));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}
}
 else if (1) {
{
  SI tmp_result;
((void) 0); /*nop*/
{
  HI tmp_nword;
  tmp_nword = GETMEMHI (current_cpu, pc, ADDSI (pc, 2));
if (ANDHI (ANDHI (tmp_nword, SRAHI (tmp_nword, 1)), 40960)) {
  {
    SI opval = ADDSI (pc, 4);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
} else {
  {
    SI opval = ADDSI (pc, 6);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
{
  {
    USI opval = GET_H_CR16 (FLD (f_op__b));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
{
  SI tmp_result;
((void) 0); /*nop*/
{
  HI tmp_nword;
  tmp_nword = GETMEMHI (current_cpu, pc, ADDSI (pc, 2));
if (ANDHI (ANDHI (tmp_nword, SRAHI (tmp_nword, 1)), 40960)) {
  {
    SI opval = ADDSI (pc, 4);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
} else {
  {
    SI opval = ADDSI (pc, 6);
    SET_H_CR (((UINT) 31), opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
{
  {
    USI opval = GET_H_CR16 (FLD (f_op__b));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LP_L_S12_) : /* lp$_L$F0 $s12x2 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_lp_L_s12_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
{
  SI tmp_result;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  {
    SI opval = ADDSI (ANDSI (pc, -4), FLD (f_s12x2));
    SET_H_AUXR (((UINT) 3), opval);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    SI opval = ADDSI (pc, 4);
    SET_H_AUXR (((UINT) 2), opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
}
}
}
}
 else if (0) {
{
  SI tmp_result;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  {
    SI opval = ADDSI (ANDSI (pc, -4), FLD (f_s12x2));
    SET_H_AUXR (((UINT) 3), opval);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    SI opval = ADDSI (pc, 4);
    SET_H_AUXR (((UINT) 2), opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
}
}
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 20);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
{
  SI tmp_result;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
  {
    SI opval = ADDSI (ANDSI (pc, -4), FLD (f_s12x2));
    SET_H_AUXR (((UINT) 3), opval);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    SI opval = ADDSI (pc, 4);
    SET_H_AUXR (((UINT) 2), opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
}
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LPCC_CCU6) : /* lp$Qcondi$F0 $U6x2 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_lpcc_ccu6.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 20);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
{
  SI tmp_result;
((void) 0); /*nop*/
{
  {
    SI opval = ADDSI (ANDSI (pc, -4), FLD (f_u6x2));
    SET_H_AUXR (((UINT) 3), opval);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    SI opval = ADDSI (pc, 4);
    SET_H_AUXR (((UINT) 2), opval);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
}
}
}
}
 else if (0) {
{
  SI tmp_result;
((void) 0); /*nop*/
{
  {
    SI opval = ADDSI (ANDSI (pc, -4), FLD (f_u6x2));
    SET_H_AUXR (((UINT) 3), opval);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    SI opval = ADDSI (pc, 4);
    SET_H_AUXR (((UINT) 2), opval);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
}
}
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 21);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
{
  SI tmp_result;
((void) 0); /*nop*/
{
  {
    SI opval = ADDSI (ANDSI (pc, -4), FLD (f_u6x2));
    SET_H_AUXR (((UINT) 3), opval);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    SI opval = ADDSI (pc, 4);
    SET_H_AUXR (((UINT) 2), opval);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
}
}
}
} else {
if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 20);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
  {
    USI opval = ADDSI (ANDSI (pc, -4), FLD (f_u6x2));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 21);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
 else if (0) {
  {
    USI opval = ADDSI (ANDSI (pc, -4), FLD (f_u6x2));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 21);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 21);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
  {
    USI opval = ADDSI (ANDSI (pc, -4), FLD (f_u6x2));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 21);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_FLAG_L_S12_) : /* flag$_L$F0 $s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
if (ANDSI (FLD (f_s12), 1)) {
sim_engine_halt (CPU_STATE (current_cpu), current_cpu, NULL, pc,			    sim_exited, a5f_h_cr_get (current_cpu, 0));}
  {
    SI opval = FLD (f_s12);
    SET_H_STATUS32 (((UINT) 0), opval);
    TRACE_RESULT (current_cpu, abuf, "status32", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_FLAG_CCU6_) : /* flag$Qcondi$F0 $U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
((void) 0); /*nop*/
{
if (ANDSI (FLD (f_u6), 1)) {
sim_engine_halt (CPU_STATE (current_cpu), current_cpu, NULL, pc,			    sim_exited, a5f_h_cr_get (current_cpu, 0));}
  {
    SI opval = FLD (f_u6);
    SET_H_STATUS32 (((UINT) 0), opval);
    written |= (1 << 6);
    TRACE_RESULT (current_cpu, abuf, "status32", 'x', opval);
  }
}
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_FLAG_L_U6_) : /* flag$_L$F0 $U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
if (ANDSI (FLD (f_u6), 1)) {
sim_engine_halt (CPU_STATE (current_cpu), current_cpu, NULL, pc,			    sim_exited, a5f_h_cr_get (current_cpu, 0));}
  {
    SI opval = FLD (f_u6);
    SET_H_STATUS32 (((UINT) 0), opval);
    TRACE_RESULT (current_cpu, abuf, "status32", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_FLAG_L_R_R__RC) : /* flag$_L$F0 $RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_st_abs.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
{
if (ANDSI (GET_H_CR (FLD (f_op_C)), 1)) {
sim_engine_halt (CPU_STATE (current_cpu), current_cpu, NULL, pc,			    sim_exited, a5f_h_cr_get (current_cpu, 0));}
  {
    SI opval = GET_H_CR (FLD (f_op_C));
    SET_H_STATUS32 (((UINT) 0), opval);
    TRACE_RESULT (current_cpu, abuf, "status32", 'x', opval);
  }
}
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_FLAG_CC__RC) : /* flag$Qcondi$F0 $RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
((void) 0); /*nop*/
{
if (ANDSI (GET_H_CR (FLD (f_op_C)), 1)) {
sim_engine_halt (CPU_STATE (current_cpu), current_cpu, NULL, pc,			    sim_exited, a5f_h_cr_get (current_cpu, 0));}
  {
    SI opval = GET_H_CR (FLD (f_op_C));
    SET_H_STATUS32 (((UINT) 0), opval);
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "status32", 'x', opval);
  }
}
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LR_L_R_R___RC_) : /* lr$_L$F0 $RB,[$RC] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_st_abs.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  {
    SI opval = GET_H_AUXR (GET_H_CR (FLD (f_op_C)));
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LR_L_S12_) : /* lr$_L$F0 $RB,[$s12] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  {
    SI opval = GET_H_AUXR (FLD (f_s12));
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LR_L_U6_) : /* lr$_L$F0 $RB,[$U6] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  {
    SI opval = GET_H_AUXR (FLD (f_u6));
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SR_L_R_R___RC_) : /* sr$_L$F0 $RB,[$RC] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_st_abs.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  {
    SI opval = GET_H_CR (FLD (f_op_B));
    SET_H_AUXR (GET_H_CR (FLD (f_op_C)), opval);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
}

  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SR_L_S12_) : /* sr$_L$F0 $RB,[$s12] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  {
    SI opval = GET_H_CR (FLD (f_op_B));
    SET_H_AUXR (FLD (f_s12), opval);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
}

  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SR_L_U6_) : /* sr$_L$F0 $RB,[$U6] */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  {
    SI opval = GET_H_CR (FLD (f_op_B));
    SET_H_AUXR (FLD (f_u6), opval);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
}

  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ASL_L_R_R__RC) : /* asl$_L$F $RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_C), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_C), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ADDSI (GET_H_CR (FLD (f_op_C)), GET_H_CR (FLD (f_op_C)));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ADDOFSI (GET_H_CR (FLD (f_op_C)), GET_H_CR (FLD (f_op_C)), 0);
    CPU (h_vbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = ADDCFSI (GET_H_CR (FLD (f_op_C)), GET_H_CR (FLD (f_op_C)), 0);
    CPU (h_cbit) = opval;
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ASL_L_U6_) : /* asl$_L$F $RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ADDSI (FLD (f_u6), FLD (f_u6));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ADDOFSI (FLD (f_u6), FLD (f_u6), 0);
    CPU (h_vbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = ADDCFSI (FLD (f_u6), FLD (f_u6), 0);
    CPU (h_cbit) = opval;
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_I16_GO_ASL_S_GO) : /* asl$_S $R_b,$R_b,$R_c */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
((void) 0); /*nop*/
  tmp_B = GET_H_CR16 (FLD (f_op__b));
  tmp_C = GET_H_CR16 (FLD (f_op__c));
  {
    SI opval = ADDSI (tmp_C, tmp_C);
    SET_H_CR16 (FLD (f_op__b), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ASR_L_R_R__RC) : /* asr$_L$F $RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_C), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_C), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = SRASI (GET_H_CR (FLD (f_op_C)), 1);
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ANDSI (GET_H_CR (FLD (f_op_C)), 1);
    CPU (h_cbit) = opval;
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ASR_L_U6_) : /* asr$_L$F $RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = SRASI (FLD (f_u6), 1);
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ANDSI (FLD (f_u6), 1);
    CPU (h_cbit) = opval;
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_I16_GO_ASR_S_GO) : /* asr$_S $R_b,$R_b,$R_c */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
((void) 0); /*nop*/
  tmp_B = GET_H_CR16 (FLD (f_op__b));
  tmp_C = GET_H_CR16 (FLD (f_op__c));
  {
    SI opval = SRASI (tmp_C, 1);
    SET_H_CR16 (FLD (f_op__b), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LSR_L_R_R__RC) : /* lsr$_L$F $RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_C), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_C), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = SRLSI (GET_H_CR (FLD (f_op_C)), 1);
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ANDSI (GET_H_CR (FLD (f_op_C)), 1);
    CPU (h_cbit) = opval;
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LSR_L_U6_) : /* lsr$_L$F $RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = SRLSI (FLD (f_u6), 1);
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ANDSI (FLD (f_u6), 1);
    CPU (h_cbit) = opval;
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_I16_GO_LSR_S_GO) : /* lsr$_S $R_b,$R_b,$R_c */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
((void) 0); /*nop*/
  tmp_B = GET_H_CR16 (FLD (f_op__b));
  tmp_C = GET_H_CR16 (FLD (f_op__c));
  {
    SI opval = SRLSI (tmp_C, 1);
    SET_H_CR16 (FLD (f_op__b), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ROR_L_R_R__RC) : /* ror$_L$F $RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_C), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_C), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ORSI (SRLSI (GET_H_CR (FLD (f_op_C)), 1), SLLSI (GET_H_CR (FLD (f_op_C)), 31));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ANDSI (GET_H_CR (FLD (f_op_C)), 1);
    CPU (h_cbit) = opval;
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ROR_L_U6_) : /* ror$_L$F $RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ORSI (SRLSI (FLD (f_u6), 1), SLLSI (FLD (f_u6), 31));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ANDSI (FLD (f_u6), 1);
    CPU (h_cbit) = opval;
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_RRC_L_R_R__RC) : /* rrc$_L$F $RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_C), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_C), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ORSI (SRLSI (GET_H_CR (FLD (f_op_C)), 1), SLLSI (ZEXTBISI (CPU (h_cbit)), 31));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ANDSI (GET_H_CR (FLD (f_op_C)), 1);
    CPU (h_cbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_RRC_L_U6_) : /* rrc$_L$F $RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ORSI (SRLSI (FLD (f_u6), 1), SLLSI (ZEXTBISI (CPU (h_cbit)), 31));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ANDSI (FLD (f_u6), 1);
    CPU (h_cbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SEXB_L_R_R__RC) : /* sexb$_L$F $RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_C), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_C), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = EXTQISI ((GET_H_CR (FLD (f_op_C))));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SEXB_L_U6_) : /* sexb$_L$F $RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = EXTQISI ((FLD (f_u6)));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_I16_GO_SEXB_S_GO) : /* sexb$_S $R_b,$R_b,$R_c */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
((void) 0); /*nop*/
  tmp_B = GET_H_CR16 (FLD (f_op__b));
  tmp_C = GET_H_CR16 (FLD (f_op__c));
  {
    SI opval = EXTQISI ((tmp_C));
    SET_H_CR16 (FLD (f_op__b), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SEXW_L_R_R__RC) : /* sexw$_L$F $RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_C), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_C), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = EXTHISI ((GET_H_CR (FLD (f_op_C))));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SEXW_L_U6_) : /* sexw$_L$F $RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = EXTHISI ((FLD (f_u6)));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_I16_GO_SEXW_S_GO) : /* sexw$_S $R_b,$R_b,$R_c */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
((void) 0); /*nop*/
  tmp_B = GET_H_CR16 (FLD (f_op__b));
  tmp_C = GET_H_CR16 (FLD (f_op__c));
  {
    SI opval = EXTHISI ((tmp_C));
    SET_H_CR16 (FLD (f_op__b), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_EXTB_L_R_R__RC) : /* extb$_L$F $RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_C), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_C), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ZEXTQISI ((GET_H_CR (FLD (f_op_C))));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_EXTB_L_U6_) : /* extb$_L$F $RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ZEXTQISI ((FLD (f_u6)));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_I16_GO_EXTB_S_GO) : /* extb$_S $R_b,$R_b,$R_c */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
((void) 0); /*nop*/
  tmp_B = GET_H_CR16 (FLD (f_op__b));
  tmp_C = GET_H_CR16 (FLD (f_op__c));
  {
    SI opval = ZEXTQISI ((tmp_C));
    SET_H_CR16 (FLD (f_op__b), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_EXTW_L_R_R__RC) : /* extw$_L$F $RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_C), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_C), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ZEXTHISI ((GET_H_CR (FLD (f_op_C))));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_EXTW_L_U6_) : /* extw$_L$F $RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ZEXTHISI ((FLD (f_u6)));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_I16_GO_EXTW_S_GO) : /* extw$_S $R_b,$R_b,$R_c */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
((void) 0); /*nop*/
  tmp_B = GET_H_CR16 (FLD (f_op__b));
  tmp_C = GET_H_CR16 (FLD (f_op__c));
  {
    SI opval = ZEXTHISI ((tmp_C));
    SET_H_CR16 (FLD (f_op__b), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ABS_L_R_R__RC) : /* abs$_L$F $RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_C), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_C), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ABSSI (({   SI tmp_res;
  tmp_res = GET_H_CR (FLD (f_op_C));
; tmp_res; }));
if (FLD (f_F)) {
{
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
  {
    BI opval = LTSI ((GET_H_CR (FLD (f_op_C))), 0);
    CPU (h_cbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
  {
    BI opval = EQSI (GET_H_CR (FLD (f_op_C)), 0x80000000);
    CPU (h_vbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = CPU (h_vbit);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ABS_L_U6_) : /* abs$_L$F $RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ABSSI (({   SI tmp_res;
  tmp_res = FLD (f_u6);
; tmp_res; }));
if (FLD (f_F)) {
{
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
  {
    BI opval = LTSI ((FLD (f_u6)), 0);
    CPU (h_cbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
  {
    BI opval = EQSI (FLD (f_u6), 0x80000000);
    CPU (h_vbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = CPU (h_vbit);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_I16_GO_ABS_S_GO) : /* abs$_S $R_b,$R_b,$R_c */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
((void) 0); /*nop*/
  tmp_B = GET_H_CR16 (FLD (f_op__b));
  tmp_C = GET_H_CR16 (FLD (f_op__c));
  {
    SI opval = ABSSI (({   SI tmp_res;
  tmp_res = tmp_C;
; tmp_res; }));
    SET_H_CR16 (FLD (f_op__b), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_NOT_L_R_R__RC) : /* not$_L$F $RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_C), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_C), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = INVSI (GET_H_CR (FLD (f_op_C)));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_NOT_L_U6_) : /* not$_L$F $RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = INVSI (FLD (f_u6));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_I16_GO_NOT_S_GO) : /* not$_S $R_b,$R_b,$R_c */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
((void) 0); /*nop*/
  tmp_B = GET_H_CR16 (FLD (f_op__b));
  tmp_C = GET_H_CR16 (FLD (f_op__c));
  {
    SI opval = INVSI (tmp_C);
    SET_H_CR16 (FLD (f_op__b), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_RLC_L_R_R__RC) : /* rlc$_L$F $RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_C), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_C), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ORSI (SLLSI (GET_H_CR (FLD (f_op_C)), 1), CPU (h_cbit));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = SRLSI (GET_H_CR (FLD (f_op_C)), 31);
    CPU (h_cbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_RLC_L_U6_) : /* rlc$_L$F $RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ORSI (SLLSI (FLD (f_u6), 1), CPU (h_cbit));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = SRLSI (FLD (f_u6), 31);
    CPU (h_cbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_I16_GO_NEG_S_GO) : /* neg$_S $R_b,$R_b,$R_c */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
((void) 0); /*nop*/
  tmp_B = GET_H_CR16 (FLD (f_op__b));
  tmp_C = GET_H_CR16 (FLD (f_op__c));
  {
    SI opval = NEGSI (tmp_C);
    SET_H_CR16 (FLD (f_op__b), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SWI) : /* swi */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.fmt_empty.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
{
; /*clobber*/
  {
    USI opval = arc_trap (current_cpu, pc, 4, 0);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
 else if (0) {
{
; /*clobber*/
  {
    USI opval = arc_trap (current_cpu, pc, 4, 0);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
{
; /*clobber*/
  {
    USI opval = arc_trap (current_cpu, pc, 4, 0);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_TRAP_S) : /* trap$_S $trapnum */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_trap_s.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (pc, 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
{
  {
    USI opval = arc_trap (current_cpu, pc, 2, FLD (f_trapnum));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
; /*clobber*/
}
}
}
 else if (0) {
{
  {
    USI opval = arc_trap (current_cpu, pc, 2, FLD (f_trapnum));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
; /*clobber*/
}
}
 else {
{
  {
    SI opval = pc;
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
{
  {
    USI opval = arc_trap (current_cpu, pc, 2, FLD (f_trapnum));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
; /*clobber*/
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BRK) : /* brk */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.fmt_empty.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

arc_breakpoint (current_cpu, pc, 4);

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_BRK_S) : /* brk_s */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.fmt_empty.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

arc_breakpoint (current_cpu, pc, 2);

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ASL_L_S12__RA_) : /* asl$_L$F $RB,$RB,$s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (SLLSI (GET_H_CR (FLD (f_op_B)), ANDSI (FLD (f_s12), 31)));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ((EQSI (ANDSI (FLD (f_s12), 31), 0)) ? (0) : (ANDSI (SRLSI (GET_H_CR (FLD (f_op_B)), SUBSI (32, ANDSI (FLD (f_s12), 31))), 1)));
    CPU (h_cbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ASL_CCU6__RA_) : /* asl$Qcondi$F $RB,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (SLLSI (GET_H_CR (FLD (f_op_B)), ANDSI (FLD (f_u6), 31)));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ((EQSI (ANDSI (FLD (f_u6), 31), 0)) ? (0) : (ANDSI (SRLSI (GET_H_CR (FLD (f_op_B)), SUBSI (32, ANDSI (FLD (f_u6), 31))), 1)));
    CPU (h_cbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ASL_L_U6__RA_) : /* asl$_L$F $RA,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (SLLSI (GET_H_CR (FLD (f_op_B)), ANDSI (FLD (f_u6), 31)));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ((EQSI (ANDSI (FLD (f_u6), 31), 0)) ? (0) : (ANDSI (SRLSI (GET_H_CR (FLD (f_op_B)), SUBSI (32, ANDSI (FLD (f_u6), 31))), 1)));
    CPU (h_cbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ASL_L_R_R__RA__RC) : /* asl$_L$F $RA,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (SLLSI (GET_H_CR (FLD (f_op_B)), ANDSI (GET_H_CR (FLD (f_op_C)), 31)));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ((EQSI (ANDSI (GET_H_CR (FLD (f_op_C)), 31), 0)) ? (0) : (ANDSI (SRLSI (GET_H_CR (FLD (f_op_B)), SUBSI (32, ANDSI (GET_H_CR (FLD (f_op_C)), 31))), 1)));
    CPU (h_cbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ASL_CC__RA__RC) : /* asl$Qcondi$F $RB,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (SLLSI (GET_H_CR (FLD (f_op_B)), ANDSI (GET_H_CR (FLD (f_op_C)), 31)));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ((EQSI (ANDSI (GET_H_CR (FLD (f_op_C)), 31), 0)) ? (0) : (ANDSI (SRLSI (GET_H_CR (FLD (f_op_B)), SUBSI (32, ANDSI (GET_H_CR (FLD (f_op_C)), 31))), 1)));
    CPU (h_cbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ASL_S_CBU3) : /* asl$_S $R_c,$R_b,$u3 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
((void) 0); /*nop*/
  tmp_B = GET_H_CR16 (FLD (f_op__b));
  tmp_C = FLD (f_u3);
  {
    SI opval = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (SLLSI (tmp_B, ANDSI (tmp_C, 31)));
    SET_H_CR16 (FLD (f_op__c), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ASL_S_SSB) : /* asl$_S $R_b,$R_b,$u5 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ldb_s_abu.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
((void) 0); /*nop*/
  tmp_B = GET_H_CR16 (FLD (f_op__b));
  tmp_C = FLD (f_u5);
  {
    SI opval = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (SLLSI (tmp_B, ANDSI (tmp_C, 31)));
    SET_H_CR16 (FLD (f_op__b), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_I16_GO_ASLM_S_GO) : /* asl$_S $R_b,$R_b,$R_c */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
((void) 0); /*nop*/
  tmp_B = GET_H_CR16 (FLD (f_op__b));
  tmp_C = GET_H_CR16 (FLD (f_op__c));
  {
    SI opval = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (SLLSI (tmp_B, ANDSI (tmp_C, 31)));
    SET_H_CR16 (FLD (f_op__b), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LSR_L_S12__RA_) : /* lsr$_L$F $RB,$RB,$s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (SRLSI (GET_H_CR (FLD (f_op_B)), ANDSI (FLD (f_s12), 31)));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ((EQSI (ANDSI (FLD (f_s12), 31), 0)) ? (0) : (ANDSI (SRLSI (GET_H_CR (FLD (f_op_B)), SUBSI (ANDSI (FLD (f_s12), 31), 1)), 1)));
    CPU (h_cbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LSR_CCU6__RA_) : /* lsr$Qcondi$F $RB,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (SRLSI (GET_H_CR (FLD (f_op_B)), ANDSI (FLD (f_u6), 31)));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ((EQSI (ANDSI (FLD (f_u6), 31), 0)) ? (0) : (ANDSI (SRLSI (GET_H_CR (FLD (f_op_B)), SUBSI (ANDSI (FLD (f_u6), 31), 1)), 1)));
    CPU (h_cbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LSR_L_U6__RA_) : /* lsr$_L$F $RA,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (SRLSI (GET_H_CR (FLD (f_op_B)), ANDSI (FLD (f_u6), 31)));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ((EQSI (ANDSI (FLD (f_u6), 31), 0)) ? (0) : (ANDSI (SRLSI (GET_H_CR (FLD (f_op_B)), SUBSI (ANDSI (FLD (f_u6), 31), 1)), 1)));
    CPU (h_cbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LSR_L_R_R__RA__RC) : /* lsr$_L$F $RA,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (SRLSI (GET_H_CR (FLD (f_op_B)), ANDSI (GET_H_CR (FLD (f_op_C)), 31)));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ((EQSI (ANDSI (GET_H_CR (FLD (f_op_C)), 31), 0)) ? (0) : (ANDSI (SRLSI (GET_H_CR (FLD (f_op_B)), SUBSI (ANDSI (GET_H_CR (FLD (f_op_C)), 31), 1)), 1)));
    CPU (h_cbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LSR_CC__RA__RC) : /* lsr$Qcondi$F $RB,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (SRLSI (GET_H_CR (FLD (f_op_B)), ANDSI (GET_H_CR (FLD (f_op_C)), 31)));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ((EQSI (ANDSI (GET_H_CR (FLD (f_op_C)), 31), 0)) ? (0) : (ANDSI (SRLSI (GET_H_CR (FLD (f_op_B)), SUBSI (ANDSI (GET_H_CR (FLD (f_op_C)), 31), 1)), 1)));
    CPU (h_cbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_LSR_S_SSB) : /* lsr$_S $R_b,$R_b,$u5 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ldb_s_abu.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
((void) 0); /*nop*/
  tmp_B = GET_H_CR16 (FLD (f_op__b));
  tmp_C = FLD (f_u5);
  {
    SI opval = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (SRLSI (tmp_B, ANDSI (tmp_C, 31)));
    SET_H_CR16 (FLD (f_op__b), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_I16_GO_LSRM_S_GO) : /* lsr$_S $R_b,$R_b,$R_c */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
((void) 0); /*nop*/
  tmp_B = GET_H_CR16 (FLD (f_op__b));
  tmp_C = GET_H_CR16 (FLD (f_op__c));
  {
    SI opval = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (SRLSI (tmp_B, ANDSI (tmp_C, 31)));
    SET_H_CR16 (FLD (f_op__b), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ASR_L_S12__RA_) : /* asr$_L$F $RB,$RB,$s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (SRASI (GET_H_CR (FLD (f_op_B)), ANDSI (FLD (f_s12), 31)));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ((EQSI (ANDSI (FLD (f_s12), 31), 0)) ? (0) : (ANDSI (SRLSI (GET_H_CR (FLD (f_op_B)), SUBSI (ANDSI (FLD (f_s12), 31), 1)), 1)));
    CPU (h_cbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ASR_CCU6__RA_) : /* asr$Qcondi$F $RB,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (SRASI (GET_H_CR (FLD (f_op_B)), ANDSI (FLD (f_u6), 31)));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ((EQSI (ANDSI (FLD (f_u6), 31), 0)) ? (0) : (ANDSI (SRLSI (GET_H_CR (FLD (f_op_B)), SUBSI (ANDSI (FLD (f_u6), 31), 1)), 1)));
    CPU (h_cbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ASR_L_U6__RA_) : /* asr$_L$F $RA,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (SRASI (GET_H_CR (FLD (f_op_B)), ANDSI (FLD (f_u6), 31)));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ((EQSI (ANDSI (FLD (f_u6), 31), 0)) ? (0) : (ANDSI (SRLSI (GET_H_CR (FLD (f_op_B)), SUBSI (ANDSI (FLD (f_u6), 31), 1)), 1)));
    CPU (h_cbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ASR_L_R_R__RA__RC) : /* asr$_L$F $RA,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (SRASI (GET_H_CR (FLD (f_op_B)), ANDSI (GET_H_CR (FLD (f_op_C)), 31)));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ((EQSI (ANDSI (GET_H_CR (FLD (f_op_C)), 31), 0)) ? (0) : (ANDSI (SRLSI (GET_H_CR (FLD (f_op_B)), SUBSI (ANDSI (GET_H_CR (FLD (f_op_C)), 31), 1)), 1)));
    CPU (h_cbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ASR_CC__RA__RC) : /* asr$Qcondi$F $RB,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (SRASI (GET_H_CR (FLD (f_op_B)), ANDSI (GET_H_CR (FLD (f_op_C)), 31)));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ((EQSI (ANDSI (GET_H_CR (FLD (f_op_C)), 31), 0)) ? (0) : (ANDSI (SRLSI (GET_H_CR (FLD (f_op_B)), SUBSI (ANDSI (GET_H_CR (FLD (f_op_C)), 31), 1)), 1)));
    CPU (h_cbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ASR_S_CBU3) : /* asr$_S $R_c,$R_b,$u3 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
((void) 0); /*nop*/
  tmp_B = GET_H_CR16 (FLD (f_op__b));
  tmp_C = FLD (f_u3);
  {
    SI opval = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (SRASI (tmp_B, ANDSI (tmp_C, 31)));
    SET_H_CR16 (FLD (f_op__c), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ASR_S_SSB) : /* asr$_S $R_b,$R_b,$u5 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_ldb_s_abu.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
((void) 0); /*nop*/
  tmp_B = GET_H_CR16 (FLD (f_op__b));
  tmp_C = FLD (f_u5);
  {
    SI opval = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (SRASI (tmp_B, ANDSI (tmp_C, 31)));
    SET_H_CR16 (FLD (f_op__b), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_I16_GO_ASRM_S_GO) : /* asr$_S $R_b,$R_b,$R_c */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  SI tmp_B;
  SI tmp_C;
((void) 0); /*nop*/
  tmp_B = GET_H_CR16 (FLD (f_op__b));
  tmp_C = GET_H_CR16 (FLD (f_op__c));
  {
    SI opval = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (SRASI (tmp_B, ANDSI (tmp_C, 31)));
    SET_H_CR16 (FLD (f_op__b), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ROR_L_S12__RA_) : /* ror$_L$F $RB,$RB,$s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (RORSI (GET_H_CR (FLD (f_op_B)), ANDSI (FLD (f_s12), 31)));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ((EQSI (ANDSI (FLD (f_s12), 31), 0)) ? (0) : (ANDSI (SRLSI (GET_H_CR (FLD (f_op_B)), SUBSI (ANDSI (FLD (f_s12), 31), 1)), 1)));
    CPU (h_cbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ROR_CCU6__RA_) : /* ror$Qcondi$F $RB,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (RORSI (GET_H_CR (FLD (f_op_B)), ANDSI (FLD (f_u6), 31)));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ((EQSI (ANDSI (FLD (f_u6), 31), 0)) ? (0) : (ANDSI (SRLSI (GET_H_CR (FLD (f_op_B)), SUBSI (ANDSI (FLD (f_u6), 31), 1)), 1)));
    CPU (h_cbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ROR_L_U6__RA_) : /* ror$_L$F $RA,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (RORSI (GET_H_CR (FLD (f_op_B)), ANDSI (FLD (f_u6), 31)));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ((EQSI (ANDSI (FLD (f_u6), 31), 0)) ? (0) : (ANDSI (SRLSI (GET_H_CR (FLD (f_op_B)), SUBSI (ANDSI (FLD (f_u6), 31), 1)), 1)));
    CPU (h_cbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ROR_L_R_R__RA__RC) : /* ror$_L$F $RA,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (RORSI (GET_H_CR (FLD (f_op_B)), ANDSI (GET_H_CR (FLD (f_op_C)), 31)));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ((EQSI (ANDSI (GET_H_CR (FLD (f_op_C)), 31), 0)) ? (0) : (ANDSI (SRLSI (GET_H_CR (FLD (f_op_B)), SUBSI (ANDSI (GET_H_CR (FLD (f_op_C)), 31), 1)), 1)));
    CPU (h_cbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ROR_CC__RA__RC) : /* ror$Qcondi$F $RB,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (RORSI (GET_H_CR (FLD (f_op_B)), ANDSI (GET_H_CR (FLD (f_op_C)), 31)));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ((EQSI (ANDSI (GET_H_CR (FLD (f_op_C)), 31), 0)) ? (0) : (ANDSI (SRLSI (GET_H_CR (FLD (f_op_B)), SUBSI (ANDSI (GET_H_CR (FLD (f_op_C)), 31), 1)), 1)));
    CPU (h_cbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MUL64_L_S12_) : /* mul64$_L$F1 $RB,$s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if ((0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (1)) {
{
  DI tmp_result;
  tmp_result = MULDI (EXTSIDI (GET_H_CR (FLD (f_op_B))), EXTSIDI (FLD (f_s12)));
  {
    SI opval = SUBWORDDISI (tmp_result, 1);
    SET_H_CR (((UINT) 57), opval);
    written |= (1 << 6);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = SUBWORDDISI (SRLDI (tmp_result, 16), 1);
    SET_H_CR (((UINT) 58), opval);
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = SUBWORDDISI (tmp_result, 0);
    SET_H_CR (((UINT) 59), opval);
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MUL64_CCU6_) : /* mul64$Qcondi$F1 $RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
((void) 0); /*nop*/
if ((0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (1)) {
{
  DI tmp_result;
  tmp_result = MULDI (EXTSIDI (GET_H_CR (FLD (f_op_B))), EXTSIDI (FLD (f_u6)));
  {
    SI opval = SUBWORDDISI (tmp_result, 1);
    SET_H_CR (((UINT) 57), opval);
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = SUBWORDDISI (SRLDI (tmp_result, 16), 1);
    SET_H_CR (((UINT) 58), opval);
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = SUBWORDDISI (tmp_result, 0);
    SET_H_CR (((UINT) 59), opval);
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MUL64_L_U6_) : /* mul64$_L$F1 $RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if ((0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (1)) {
{
  DI tmp_result;
  tmp_result = MULDI (EXTSIDI (GET_H_CR (FLD (f_op_B))), EXTSIDI (FLD (f_u6)));
  {
    SI opval = SUBWORDDISI (tmp_result, 1);
    SET_H_CR (((UINT) 57), opval);
    written |= (1 << 6);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = SUBWORDDISI (SRLDI (tmp_result, 16), 1);
    SET_H_CR (((UINT) 58), opval);
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = SUBWORDDISI (tmp_result, 0);
    SET_H_CR (((UINT) 59), opval);
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MUL64_L_R_R__RC) : /* mul64$_L$F1 $RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_st_abs.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if ((0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (1)) {
{
  DI tmp_result;
  tmp_result = MULDI (EXTSIDI (GET_H_CR (FLD (f_op_B))), EXTSIDI (GET_H_CR (FLD (f_op_C))));
  {
    SI opval = SUBWORDDISI (tmp_result, 1);
    SET_H_CR (((UINT) 57), opval);
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = SUBWORDDISI (SRLDI (tmp_result, 16), 1);
    SET_H_CR (((UINT) 58), opval);
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = SUBWORDDISI (tmp_result, 0);
    SET_H_CR (((UINT) 59), opval);
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MUL64_CC__RC) : /* mul64$Qcondi$F1 $RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
((void) 0); /*nop*/
if ((0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (1)) {
{
  DI tmp_result;
  tmp_result = MULDI (EXTSIDI (GET_H_CR (FLD (f_op_B))), EXTSIDI (GET_H_CR (FLD (f_op_C))));
  {
    SI opval = SUBWORDDISI (tmp_result, 1);
    SET_H_CR (((UINT) 57), opval);
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = SUBWORDDISI (SRLDI (tmp_result, 16), 1);
    SET_H_CR (((UINT) 58), opval);
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = SUBWORDDISI (tmp_result, 0);
    SET_H_CR (((UINT) 59), opval);
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MUL64_S_GO) : /* mul64$_S $R_b,$R_c */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
((void) 0); /*nop*/
if ((0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (1)) {
{
  DI tmp_result;
  tmp_result = MULDI (EXTSIDI (GET_H_CR16 (FLD (f_op__b))), EXTSIDI (GET_H_CR16 (FLD (f_op__c))));
  {
    SI opval = SUBWORDDISI (tmp_result, 1);
    SET_H_CR (((UINT) 57), opval);
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = SUBWORDDISI (SRLDI (tmp_result, 16), 1);
    SET_H_CR (((UINT) 58), opval);
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = SUBWORDDISI (tmp_result, 0);
    SET_H_CR (((UINT) 59), opval);
    written |= (1 << 4);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MULU64_L_S12_) : /* mulu64$_L$F1 $RB,$s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if ((0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (1)) {
{
  DI tmp_result;
  tmp_result = MULDI (ZEXTSIDI (GET_H_CR (FLD (f_op_B))), ZEXTSIDI (FLD (f_s12)));
  {
    SI opval = SUBWORDDISI (tmp_result, 1);
    SET_H_CR (((UINT) 57), opval);
    written |= (1 << 6);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = SUBWORDDISI (SRLDI (tmp_result, 16), 1);
    SET_H_CR (((UINT) 58), opval);
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = SUBWORDDISI (tmp_result, 0);
    SET_H_CR (((UINT) 59), opval);
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MULU64_CCU6_) : /* mulu64$Qcondi$F1 $RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
((void) 0); /*nop*/
if ((0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (1)) {
{
  DI tmp_result;
  tmp_result = MULDI (ZEXTSIDI (GET_H_CR (FLD (f_op_B))), ZEXTSIDI (FLD (f_u6)));
  {
    SI opval = SUBWORDDISI (tmp_result, 1);
    SET_H_CR (((UINT) 57), opval);
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = SUBWORDDISI (SRLDI (tmp_result, 16), 1);
    SET_H_CR (((UINT) 58), opval);
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = SUBWORDDISI (tmp_result, 0);
    SET_H_CR (((UINT) 59), opval);
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MULU64_L_U6_) : /* mulu64$_L$F1 $RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if ((0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (1)) {
{
  DI tmp_result;
  tmp_result = MULDI (ZEXTSIDI (GET_H_CR (FLD (f_op_B))), ZEXTSIDI (FLD (f_u6)));
  {
    SI opval = SUBWORDDISI (tmp_result, 1);
    SET_H_CR (((UINT) 57), opval);
    written |= (1 << 6);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = SUBWORDDISI (SRLDI (tmp_result, 16), 1);
    SET_H_CR (((UINT) 58), opval);
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = SUBWORDDISI (tmp_result, 0);
    SET_H_CR (((UINT) 59), opval);
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MULU64_L_R_R__RC) : /* mulu64$_L$F1 $RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_st_abs.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if ((0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (1)) {
{
  DI tmp_result;
  tmp_result = MULDI (ZEXTSIDI (GET_H_CR (FLD (f_op_B))), ZEXTSIDI (GET_H_CR (FLD (f_op_C))));
  {
    SI opval = SUBWORDDISI (tmp_result, 1);
    SET_H_CR (((UINT) 57), opval);
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = SUBWORDDISI (SRLDI (tmp_result, 16), 1);
    SET_H_CR (((UINT) 58), opval);
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = SUBWORDDISI (tmp_result, 0);
    SET_H_CR (((UINT) 59), opval);
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MULU64_CC__RC) : /* mulu64$Qcondi$F1 $RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
((void) 0); /*nop*/
if ((0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (1)) {
{
  DI tmp_result;
  tmp_result = MULDI (ZEXTSIDI (GET_H_CR (FLD (f_op_B))), ZEXTSIDI (GET_H_CR (FLD (f_op_C))));
  {
    SI opval = SUBWORDDISI (tmp_result, 1);
    SET_H_CR (((UINT) 57), opval);
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = SUBWORDDISI (SRLDI (tmp_result, 16), 1);
    SET_H_CR (((UINT) 58), opval);
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = SUBWORDDISI (tmp_result, 0);
    SET_H_CR (((UINT) 59), opval);
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ADDS_L_S12__RA_) : /* adds$_L$F $RB,$RB,$s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (({   DI tmp_tmp;
  tmp_tmp = ADDDI (EXTSIDI (GET_H_CR (FLD (f_op_B))), EXTSIDI (FLD (f_s12)));
; ((GTDI (tmp_tmp, 2147483647)) ? (  tmp_cur_s1bit = 1, 2147483647) : (LTDI (tmp_tmp, ADDSI (-2147483647, -1))) ? (  tmp_cur_s1bit = 1, ADDSI (-2147483647, -1)) : (  tmp_cur_s1bit = 0, tmp_tmp)); }));
if (FLD (f_F)) {
{
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
  {
    BI opval = 0;
    CPU (h_cbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ADDS_CCU6__RA_) : /* adds$Qcondi$F $RB,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (({   DI tmp_tmp;
  tmp_tmp = ADDDI (EXTSIDI (GET_H_CR (FLD (f_op_B))), EXTSIDI (FLD (f_u6)));
; ((GTDI (tmp_tmp, 2147483647)) ? (  tmp_cur_s1bit = 1, 2147483647) : (LTDI (tmp_tmp, ADDSI (-2147483647, -1))) ? (  tmp_cur_s1bit = 1, ADDSI (-2147483647, -1)) : (  tmp_cur_s1bit = 0, tmp_tmp)); }));
if (FLD (f_F)) {
{
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
  {
    BI opval = 0;
    CPU (h_cbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ADDS_L_U6__RA_) : /* adds$_L$F $RA,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (({   DI tmp_tmp;
  tmp_tmp = ADDDI (EXTSIDI (GET_H_CR (FLD (f_op_B))), EXTSIDI (FLD (f_u6)));
; ((GTDI (tmp_tmp, 2147483647)) ? (  tmp_cur_s1bit = 1, 2147483647) : (LTDI (tmp_tmp, ADDSI (-2147483647, -1))) ? (  tmp_cur_s1bit = 1, ADDSI (-2147483647, -1)) : (  tmp_cur_s1bit = 0, tmp_tmp)); }));
if (FLD (f_F)) {
{
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
  {
    BI opval = 0;
    CPU (h_cbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ADDS_L_R_R__RA__RC) : /* adds$_L$F $RA,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (({   DI tmp_tmp;
  tmp_tmp = ADDDI (EXTSIDI (GET_H_CR (FLD (f_op_B))), EXTSIDI (GET_H_CR (FLD (f_op_C))));
; ((GTDI (tmp_tmp, 2147483647)) ? (  tmp_cur_s1bit = 1, 2147483647) : (LTDI (tmp_tmp, ADDSI (-2147483647, -1))) ? (  tmp_cur_s1bit = 1, ADDSI (-2147483647, -1)) : (  tmp_cur_s1bit = 0, tmp_tmp)); }));
if (FLD (f_F)) {
{
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
  {
    BI opval = 0;
    CPU (h_cbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ADDS_CC__RA__RC) : /* adds$Qcondi$F $RB,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (({   DI tmp_tmp;
  tmp_tmp = ADDDI (EXTSIDI (GET_H_CR (FLD (f_op_B))), EXTSIDI (GET_H_CR (FLD (f_op_C))));
; ((GTDI (tmp_tmp, 2147483647)) ? (  tmp_cur_s1bit = 1, 2147483647) : (LTDI (tmp_tmp, ADDSI (-2147483647, -1))) ? (  tmp_cur_s1bit = 1, ADDSI (-2147483647, -1)) : (  tmp_cur_s1bit = 0, tmp_tmp)); }));
if (FLD (f_F)) {
{
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
  {
    BI opval = 0;
    CPU (h_cbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SUBS_L_S12__RA_) : /* subs$_L$F $RB,$RB,$s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (({   DI tmp_tmp;
  tmp_tmp = SUBDI (EXTSIDI (GET_H_CR (FLD (f_op_B))), EXTSIDI (FLD (f_s12)));
; ((GTDI (tmp_tmp, 2147483647)) ? (  tmp_cur_s1bit = 1, 2147483647) : (LTDI (tmp_tmp, ADDSI (-2147483647, -1))) ? (  tmp_cur_s1bit = 1, ADDSI (-2147483647, -1)) : (  tmp_cur_s1bit = 0, tmp_tmp)); }));
if (FLD (f_F)) {
{
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
  {
    BI opval = 0;
    CPU (h_cbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SUBS_CCU6__RA_) : /* subs$Qcondi$F $RB,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (({   DI tmp_tmp;
  tmp_tmp = SUBDI (EXTSIDI (GET_H_CR (FLD (f_op_B))), EXTSIDI (FLD (f_u6)));
; ((GTDI (tmp_tmp, 2147483647)) ? (  tmp_cur_s1bit = 1, 2147483647) : (LTDI (tmp_tmp, ADDSI (-2147483647, -1))) ? (  tmp_cur_s1bit = 1, ADDSI (-2147483647, -1)) : (  tmp_cur_s1bit = 0, tmp_tmp)); }));
if (FLD (f_F)) {
{
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
  {
    BI opval = 0;
    CPU (h_cbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SUBS_L_U6__RA_) : /* subs$_L$F $RA,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (({   DI tmp_tmp;
  tmp_tmp = SUBDI (EXTSIDI (GET_H_CR (FLD (f_op_B))), EXTSIDI (FLD (f_u6)));
; ((GTDI (tmp_tmp, 2147483647)) ? (  tmp_cur_s1bit = 1, 2147483647) : (LTDI (tmp_tmp, ADDSI (-2147483647, -1))) ? (  tmp_cur_s1bit = 1, ADDSI (-2147483647, -1)) : (  tmp_cur_s1bit = 0, tmp_tmp)); }));
if (FLD (f_F)) {
{
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
  {
    BI opval = 0;
    CPU (h_cbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SUBS_L_R_R__RA__RC) : /* subs$_L$F $RA,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (({   DI tmp_tmp;
  tmp_tmp = SUBDI (EXTSIDI (GET_H_CR (FLD (f_op_B))), EXTSIDI (GET_H_CR (FLD (f_op_C))));
; ((GTDI (tmp_tmp, 2147483647)) ? (  tmp_cur_s1bit = 1, 2147483647) : (LTDI (tmp_tmp, ADDSI (-2147483647, -1))) ? (  tmp_cur_s1bit = 1, ADDSI (-2147483647, -1)) : (  tmp_cur_s1bit = 0, tmp_tmp)); }));
if (FLD (f_F)) {
{
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
  {
    BI opval = 0;
    CPU (h_cbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SUBS_CC__RA__RC) : /* subs$Qcondi$F $RB,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (({   DI tmp_tmp;
  tmp_tmp = SUBDI (EXTSIDI (GET_H_CR (FLD (f_op_B))), EXTSIDI (GET_H_CR (FLD (f_op_C))));
; ((GTDI (tmp_tmp, 2147483647)) ? (  tmp_cur_s1bit = 1, 2147483647) : (LTDI (tmp_tmp, ADDSI (-2147483647, -1))) ? (  tmp_cur_s1bit = 1, ADDSI (-2147483647, -1)) : (  tmp_cur_s1bit = 0, tmp_tmp)); }));
if (FLD (f_F)) {
{
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
  {
    BI opval = 0;
    CPU (h_cbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_DIVAW_L_S12__RA_) : /* divaw$_L$F0 $RB,$RB,$s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (({   USI tmp_tmp;
  tmp_tmp = SLLSI (GET_H_CR (FLD (f_op_B)), 1);
; ((EQSI (ANDSI (SUBSI (tmp_tmp, FLD (f_s12)), 0x80000000), 0)) ? (ADDSI (SUBSI (tmp_tmp, FLD (f_s12)), 1)) : (tmp_tmp)); }));
if (FLD (f_F)) {
((void) 0); /*nop*/
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_DIVAW_CCU6__RA_) : /* divaw$Qcondi$F0 $RB,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (({   USI tmp_tmp;
  tmp_tmp = SLLSI (GET_H_CR (FLD (f_op_B)), 1);
; ((EQSI (ANDSI (SUBSI (tmp_tmp, FLD (f_u6)), 0x80000000), 0)) ? (ADDSI (SUBSI (tmp_tmp, FLD (f_u6)), 1)) : (tmp_tmp)); }));
if (FLD (f_F)) {
((void) 0); /*nop*/
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_DIVAW_L_U6__RA_) : /* divaw$_L$F0 $RA,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (({   USI tmp_tmp;
  tmp_tmp = SLLSI (GET_H_CR (FLD (f_op_B)), 1);
; ((EQSI (ANDSI (SUBSI (tmp_tmp, FLD (f_u6)), 0x80000000), 0)) ? (ADDSI (SUBSI (tmp_tmp, FLD (f_u6)), 1)) : (tmp_tmp)); }));
if (FLD (f_F)) {
((void) 0); /*nop*/
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_DIVAW_L_R_R__RA__RC) : /* divaw$_L$F0 $RA,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (({   USI tmp_tmp;
  tmp_tmp = SLLSI (GET_H_CR (FLD (f_op_B)), 1);
; ((EQSI (ANDSI (SUBSI (tmp_tmp, GET_H_CR (FLD (f_op_C))), 0x80000000), 0)) ? (ADDSI (SUBSI (tmp_tmp, GET_H_CR (FLD (f_op_C))), 1)) : (tmp_tmp)); }));
if (FLD (f_F)) {
((void) 0); /*nop*/
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_DIVAW_CC__RA__RC) : /* divaw$Qcondi$F0 $RB,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (({   USI tmp_tmp;
  tmp_tmp = SLLSI (GET_H_CR (FLD (f_op_B)), 1);
; ((EQSI (ANDSI (SUBSI (tmp_tmp, GET_H_CR (FLD (f_op_C))), 0x80000000), 0)) ? (ADDSI (SUBSI (tmp_tmp, GET_H_CR (FLD (f_op_C))), 1)) : (tmp_tmp)); }));
if (FLD (f_F)) {
((void) 0); /*nop*/
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ASLS_L_S12__RA_) : /* asls$_L$F $RB,$RB,$s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (({   DI tmp_b;
  tmp_b = EXTSIDI (GET_H_CR (FLD (f_op_B)));
;   tmp_b = (EQDI (tmp_b, 0)) ? (0) : (GTSI ((FLD (f_s12)), 31)) ? (SLLDI (tmp_b, 31)) : (LTSI ((FLD (f_s12)), -31)) ? (SRADI (tmp_b, 31)) : (GESI ((FLD (f_s12)), 0)) ? (SLLDI (tmp_b, (FLD (f_s12)))) : (SRASI (GET_H_CR (FLD (f_op_B)), NEGSI ((FLD (f_s12)))));
; ((GTDI (tmp_b, 2147483647)) ? (  tmp_cur_s1bit = 1, 2147483647) : (LTDI (tmp_b, ADDSI (-2147483647, -1))) ? (  tmp_cur_s1bit = 1, ADDSI (-2147483647, -1)) : (  tmp_cur_s1bit = 0, tmp_b)); }));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ASLS_CCU6__RA_) : /* asls$Qcondi$F $RB,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (({   DI tmp_b;
  tmp_b = EXTSIDI (GET_H_CR (FLD (f_op_B)));
;   tmp_b = (EQDI (tmp_b, 0)) ? (0) : (GTSI ((FLD (f_u6)), 31)) ? (SLLDI (tmp_b, 31)) : (LTSI ((FLD (f_u6)), -31)) ? (SRADI (tmp_b, 31)) : (GESI ((FLD (f_u6)), 0)) ? (SLLDI (tmp_b, (FLD (f_u6)))) : (SRASI (GET_H_CR (FLD (f_op_B)), NEGSI ((FLD (f_u6)))));
; ((GTDI (tmp_b, 2147483647)) ? (  tmp_cur_s1bit = 1, 2147483647) : (LTDI (tmp_b, ADDSI (-2147483647, -1))) ? (  tmp_cur_s1bit = 1, ADDSI (-2147483647, -1)) : (  tmp_cur_s1bit = 0, tmp_b)); }));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ASLS_L_U6__RA_) : /* asls$_L$F $RA,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (({   DI tmp_b;
  tmp_b = EXTSIDI (GET_H_CR (FLD (f_op_B)));
;   tmp_b = (EQDI (tmp_b, 0)) ? (0) : (GTSI ((FLD (f_u6)), 31)) ? (SLLDI (tmp_b, 31)) : (LTSI ((FLD (f_u6)), -31)) ? (SRADI (tmp_b, 31)) : (GESI ((FLD (f_u6)), 0)) ? (SLLDI (tmp_b, (FLD (f_u6)))) : (SRASI (GET_H_CR (FLD (f_op_B)), NEGSI ((FLD (f_u6)))));
; ((GTDI (tmp_b, 2147483647)) ? (  tmp_cur_s1bit = 1, 2147483647) : (LTDI (tmp_b, ADDSI (-2147483647, -1))) ? (  tmp_cur_s1bit = 1, ADDSI (-2147483647, -1)) : (  tmp_cur_s1bit = 0, tmp_b)); }));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ASLS_L_R_R__RA__RC) : /* asls$_L$F $RA,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (({   DI tmp_b;
  tmp_b = EXTSIDI (GET_H_CR (FLD (f_op_B)));
;   tmp_b = (EQDI (tmp_b, 0)) ? (0) : (GTSI ((GET_H_CR (FLD (f_op_C))), 31)) ? (SLLDI (tmp_b, 31)) : (LTSI ((GET_H_CR (FLD (f_op_C))), -31)) ? (SRADI (tmp_b, 31)) : (GESI ((GET_H_CR (FLD (f_op_C))), 0)) ? (SLLDI (tmp_b, (GET_H_CR (FLD (f_op_C))))) : (SRASI (GET_H_CR (FLD (f_op_B)), NEGSI ((GET_H_CR (FLD (f_op_C))))));
; ((GTDI (tmp_b, 2147483647)) ? (  tmp_cur_s1bit = 1, 2147483647) : (LTDI (tmp_b, ADDSI (-2147483647, -1))) ? (  tmp_cur_s1bit = 1, ADDSI (-2147483647, -1)) : (  tmp_cur_s1bit = 0, tmp_b)); }));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ASLS_CC__RA__RC) : /* asls$Qcondi$F $RB,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (({   DI tmp_b;
  tmp_b = EXTSIDI (GET_H_CR (FLD (f_op_B)));
;   tmp_b = (EQDI (tmp_b, 0)) ? (0) : (GTSI ((GET_H_CR (FLD (f_op_C))), 31)) ? (SLLDI (tmp_b, 31)) : (LTSI ((GET_H_CR (FLD (f_op_C))), -31)) ? (SRADI (tmp_b, 31)) : (GESI ((GET_H_CR (FLD (f_op_C))), 0)) ? (SLLDI (tmp_b, (GET_H_CR (FLD (f_op_C))))) : (SRASI (GET_H_CR (FLD (f_op_B)), NEGSI ((GET_H_CR (FLD (f_op_C))))));
; ((GTDI (tmp_b, 2147483647)) ? (  tmp_cur_s1bit = 1, 2147483647) : (LTDI (tmp_b, ADDSI (-2147483647, -1))) ? (  tmp_cur_s1bit = 1, ADDSI (-2147483647, -1)) : (  tmp_cur_s1bit = 0, tmp_b)); }));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ASRS_L_S12__RA_) : /* asrs$_L$F $RB,$RB,$s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (({   DI tmp_b;
  tmp_b = EXTSIDI (GET_H_CR (FLD (f_op_B)));
;   tmp_b = (EQDI (tmp_b, 0)) ? (0) : (GTSI ((FLD (f_s12)), 31)) ? (SRADI (tmp_b, 31)) : (LTSI ((FLD (f_s12)), -31)) ? (SLLDI (tmp_b, 31)) : (GESI ((FLD (f_s12)), 0)) ? (SRADI (tmp_b, (FLD (f_s12)))) : (SLLSI (GET_H_CR (FLD (f_op_B)), NEGSI ((FLD (f_s12)))));
; ((GTDI (tmp_b, 2147483647)) ? (  tmp_cur_s1bit = 1, 2147483647) : (LTDI (tmp_b, ADDSI (-2147483647, -1))) ? (  tmp_cur_s1bit = 1, ADDSI (-2147483647, -1)) : (  tmp_cur_s1bit = 0, tmp_b)); }));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ASRS_CCU6__RA_) : /* asrs$Qcondi$F $RB,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (({   DI tmp_b;
  tmp_b = EXTSIDI (GET_H_CR (FLD (f_op_B)));
;   tmp_b = (EQDI (tmp_b, 0)) ? (0) : (GTSI ((FLD (f_u6)), 31)) ? (SRADI (tmp_b, 31)) : (LTSI ((FLD (f_u6)), -31)) ? (SLLDI (tmp_b, 31)) : (GESI ((FLD (f_u6)), 0)) ? (SRADI (tmp_b, (FLD (f_u6)))) : (SLLSI (GET_H_CR (FLD (f_op_B)), NEGSI ((FLD (f_u6)))));
; ((GTDI (tmp_b, 2147483647)) ? (  tmp_cur_s1bit = 1, 2147483647) : (LTDI (tmp_b, ADDSI (-2147483647, -1))) ? (  tmp_cur_s1bit = 1, ADDSI (-2147483647, -1)) : (  tmp_cur_s1bit = 0, tmp_b)); }));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ASRS_L_U6__RA_) : /* asrs$_L$F $RA,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (({   DI tmp_b;
  tmp_b = EXTSIDI (GET_H_CR (FLD (f_op_B)));
;   tmp_b = (EQDI (tmp_b, 0)) ? (0) : (GTSI ((FLD (f_u6)), 31)) ? (SRADI (tmp_b, 31)) : (LTSI ((FLD (f_u6)), -31)) ? (SLLDI (tmp_b, 31)) : (GESI ((FLD (f_u6)), 0)) ? (SRADI (tmp_b, (FLD (f_u6)))) : (SLLSI (GET_H_CR (FLD (f_op_B)), NEGSI ((FLD (f_u6)))));
; ((GTDI (tmp_b, 2147483647)) ? (  tmp_cur_s1bit = 1, 2147483647) : (LTDI (tmp_b, ADDSI (-2147483647, -1))) ? (  tmp_cur_s1bit = 1, ADDSI (-2147483647, -1)) : (  tmp_cur_s1bit = 0, tmp_b)); }));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ASRS_L_R_R__RA__RC) : /* asrs$_L$F $RA,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (({   DI tmp_b;
  tmp_b = EXTSIDI (GET_H_CR (FLD (f_op_B)));
;   tmp_b = (EQDI (tmp_b, 0)) ? (0) : (GTSI ((GET_H_CR (FLD (f_op_C))), 31)) ? (SRADI (tmp_b, 31)) : (LTSI ((GET_H_CR (FLD (f_op_C))), -31)) ? (SLLDI (tmp_b, 31)) : (GESI ((GET_H_CR (FLD (f_op_C))), 0)) ? (SRADI (tmp_b, (GET_H_CR (FLD (f_op_C))))) : (SLLSI (GET_H_CR (FLD (f_op_B)), NEGSI ((GET_H_CR (FLD (f_op_C))))));
; ((GTDI (tmp_b, 2147483647)) ? (  tmp_cur_s1bit = 1, 2147483647) : (LTDI (tmp_b, ADDSI (-2147483647, -1))) ? (  tmp_cur_s1bit = 1, ADDSI (-2147483647, -1)) : (  tmp_cur_s1bit = 0, tmp_b)); }));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ASRS_CC__RA__RC) : /* asrs$Qcondi$F $RB,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (({   DI tmp_b;
  tmp_b = EXTSIDI (GET_H_CR (FLD (f_op_B)));
;   tmp_b = (EQDI (tmp_b, 0)) ? (0) : (GTSI ((GET_H_CR (FLD (f_op_C))), 31)) ? (SRADI (tmp_b, 31)) : (LTSI ((GET_H_CR (FLD (f_op_C))), -31)) ? (SLLDI (tmp_b, 31)) : (GESI ((GET_H_CR (FLD (f_op_C))), 0)) ? (SRADI (tmp_b, (GET_H_CR (FLD (f_op_C))))) : (SLLSI (GET_H_CR (FLD (f_op_B)), NEGSI ((GET_H_CR (FLD (f_op_C))))));
; ((GTDI (tmp_b, 2147483647)) ? (  tmp_cur_s1bit = 1, 2147483647) : (LTDI (tmp_b, ADDSI (-2147483647, -1))) ? (  tmp_cur_s1bit = 1, ADDSI (-2147483647, -1)) : (  tmp_cur_s1bit = 0, tmp_b)); }));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ADDSDW_L_S12__RA_) : /* addsdw$_L$F $RB,$RB,$s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (({   SI tmp_C_SI;
  HI tmp_res1;
  HI tmp_res2;
  tmp_res2 = ({   SI tmp_tmp;
  tmp_tmp = ADDHI (SUBWORDSIHI (GET_H_CR (FLD (f_op_B)), 1), SUBWORDSIHI ((FLD (f_s12)), 1));
; (GTSI (tmp_tmp, 32767)) ? (  tmp_cur_s1bit = 1, 32767) : (LTSI (tmp_tmp, -32768)) ? (  tmp_cur_s1bit = 1, -32768) : (tmp_tmp); });
;   tmp_cur_s2bit = tmp_cur_s1bit;
;   tmp_res1 = ({   SI tmp_tmp;
  tmp_tmp = ADDHI (SUBWORDSIHI (GET_H_CR (FLD (f_op_B)), 0), SUBWORDSIHI ((FLD (f_s12)), 0));
; (GTSI (tmp_tmp, 32767)) ? (  tmp_cur_s1bit = 1, 32767) : (LTSI (tmp_tmp, -32768)) ? (  tmp_cur_s1bit = 1, -32768) : (tmp_tmp); });
; ORHI (SLLHI (tmp_res1, 16), tmp_res2); }));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ORBI (tmp_cur_s1bit, tmp_cur_s2bit);
    CPU (h_vbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
}
if (tmp_cur_s2bit) {
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ADDSDW_CCU6__RA_) : /* addsdw$Qcondi$F $RB,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (({   SI tmp_C_SI;
  HI tmp_res1;
  HI tmp_res2;
  tmp_res2 = ({   SI tmp_tmp;
  tmp_tmp = ADDHI (SUBWORDSIHI (GET_H_CR (FLD (f_op_B)), 1), SUBWORDSIHI ((FLD (f_u6)), 1));
; (GTSI (tmp_tmp, 32767)) ? (  tmp_cur_s1bit = 1, 32767) : (LTSI (tmp_tmp, -32768)) ? (  tmp_cur_s1bit = 1, -32768) : (tmp_tmp); });
;   tmp_cur_s2bit = tmp_cur_s1bit;
;   tmp_res1 = ({   SI tmp_tmp;
  tmp_tmp = ADDHI (SUBWORDSIHI (GET_H_CR (FLD (f_op_B)), 0), SUBWORDSIHI ((FLD (f_u6)), 0));
; (GTSI (tmp_tmp, 32767)) ? (  tmp_cur_s1bit = 1, 32767) : (LTSI (tmp_tmp, -32768)) ? (  tmp_cur_s1bit = 1, -32768) : (tmp_tmp); });
; ORHI (SLLHI (tmp_res1, 16), tmp_res2); }));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ORBI (tmp_cur_s1bit, tmp_cur_s2bit);
    CPU (h_vbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
}
if (tmp_cur_s2bit) {
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ADDSDW_L_U6__RA_) : /* addsdw$_L$F $RA,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (({   SI tmp_C_SI;
  HI tmp_res1;
  HI tmp_res2;
  tmp_res2 = ({   SI tmp_tmp;
  tmp_tmp = ADDHI (SUBWORDSIHI (GET_H_CR (FLD (f_op_B)), 1), SUBWORDSIHI ((FLD (f_u6)), 1));
; (GTSI (tmp_tmp, 32767)) ? (  tmp_cur_s1bit = 1, 32767) : (LTSI (tmp_tmp, -32768)) ? (  tmp_cur_s1bit = 1, -32768) : (tmp_tmp); });
;   tmp_cur_s2bit = tmp_cur_s1bit;
;   tmp_res1 = ({   SI tmp_tmp;
  tmp_tmp = ADDHI (SUBWORDSIHI (GET_H_CR (FLD (f_op_B)), 0), SUBWORDSIHI ((FLD (f_u6)), 0));
; (GTSI (tmp_tmp, 32767)) ? (  tmp_cur_s1bit = 1, 32767) : (LTSI (tmp_tmp, -32768)) ? (  tmp_cur_s1bit = 1, -32768) : (tmp_tmp); });
; ORHI (SLLHI (tmp_res1, 16), tmp_res2); }));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ORBI (tmp_cur_s1bit, tmp_cur_s2bit);
    CPU (h_vbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
}
if (tmp_cur_s2bit) {
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ADDSDW_L_R_R__RA__RC) : /* addsdw$_L$F $RA,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (({   SI tmp_C_SI;
  HI tmp_res1;
  HI tmp_res2;
  tmp_res2 = ({   SI tmp_tmp;
  tmp_tmp = ADDHI (SUBWORDSIHI (GET_H_CR (FLD (f_op_B)), 1), SUBWORDSIHI ((GET_H_CR (FLD (f_op_C))), 1));
; (GTSI (tmp_tmp, 32767)) ? (  tmp_cur_s1bit = 1, 32767) : (LTSI (tmp_tmp, -32768)) ? (  tmp_cur_s1bit = 1, -32768) : (tmp_tmp); });
;   tmp_cur_s2bit = tmp_cur_s1bit;
;   tmp_res1 = ({   SI tmp_tmp;
  tmp_tmp = ADDHI (SUBWORDSIHI (GET_H_CR (FLD (f_op_B)), 0), SUBWORDSIHI ((GET_H_CR (FLD (f_op_C))), 0));
; (GTSI (tmp_tmp, 32767)) ? (  tmp_cur_s1bit = 1, 32767) : (LTSI (tmp_tmp, -32768)) ? (  tmp_cur_s1bit = 1, -32768) : (tmp_tmp); });
; ORHI (SLLHI (tmp_res1, 16), tmp_res2); }));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ORBI (tmp_cur_s1bit, tmp_cur_s2bit);
    CPU (h_vbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
}
if (tmp_cur_s2bit) {
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ADDSDW_CC__RA__RC) : /* addsdw$Qcondi$F $RB,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (({   SI tmp_C_SI;
  HI tmp_res1;
  HI tmp_res2;
  tmp_res2 = ({   SI tmp_tmp;
  tmp_tmp = ADDHI (SUBWORDSIHI (GET_H_CR (FLD (f_op_B)), 1), SUBWORDSIHI ((GET_H_CR (FLD (f_op_C))), 1));
; (GTSI (tmp_tmp, 32767)) ? (  tmp_cur_s1bit = 1, 32767) : (LTSI (tmp_tmp, -32768)) ? (  tmp_cur_s1bit = 1, -32768) : (tmp_tmp); });
;   tmp_cur_s2bit = tmp_cur_s1bit;
;   tmp_res1 = ({   SI tmp_tmp;
  tmp_tmp = ADDHI (SUBWORDSIHI (GET_H_CR (FLD (f_op_B)), 0), SUBWORDSIHI ((GET_H_CR (FLD (f_op_C))), 0));
; (GTSI (tmp_tmp, 32767)) ? (  tmp_cur_s1bit = 1, 32767) : (LTSI (tmp_tmp, -32768)) ? (  tmp_cur_s1bit = 1, -32768) : (tmp_tmp); });
; ORHI (SLLHI (tmp_res1, 16), tmp_res2); }));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ORBI (tmp_cur_s1bit, tmp_cur_s2bit);
    CPU (h_vbit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
}
if (tmp_cur_s2bit) {
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SUBSDW_L_S12__RA_) : /* subsdw$_L$F $RB,$RB,$s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (({   SI tmp_C_SI;
  HI tmp_res1;
  HI tmp_res2;
  tmp_res2 = ({   SI tmp_tmp;
  tmp_tmp = SUBHI (SUBWORDSIHI (GET_H_CR (FLD (f_op_B)), 1), SUBWORDSIHI ((FLD (f_s12)), 1));
; (GTSI (tmp_tmp, 32767)) ? (  tmp_cur_s1bit = 1, 32767) : (LTSI (tmp_tmp, -32768)) ? (  tmp_cur_s1bit = 1, -32768) : (tmp_tmp); });
;   tmp_cur_s2bit = tmp_cur_s1bit;
;   tmp_res1 = ({   SI tmp_tmp;
  tmp_tmp = SUBHI (SUBWORDSIHI (GET_H_CR (FLD (f_op_B)), 0), SUBWORDSIHI ((FLD (f_s12)), 0));
; (GTSI (tmp_tmp, 32767)) ? (  tmp_cur_s1bit = 1, 32767) : (LTSI (tmp_tmp, -32768)) ? (  tmp_cur_s1bit = 1, -32768) : (tmp_tmp); });
; ORHI (SLLHI (tmp_res1, 16), tmp_res2); }));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ORBI (tmp_cur_s1bit, tmp_cur_s2bit);
    CPU (h_vbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
}
if (tmp_cur_s2bit) {
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SUBSDW_CCU6__RA_) : /* subsdw$Qcondi$F $RB,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (({   SI tmp_C_SI;
  HI tmp_res1;
  HI tmp_res2;
  tmp_res2 = ({   SI tmp_tmp;
  tmp_tmp = SUBHI (SUBWORDSIHI (GET_H_CR (FLD (f_op_B)), 1), SUBWORDSIHI ((FLD (f_u6)), 1));
; (GTSI (tmp_tmp, 32767)) ? (  tmp_cur_s1bit = 1, 32767) : (LTSI (tmp_tmp, -32768)) ? (  tmp_cur_s1bit = 1, -32768) : (tmp_tmp); });
;   tmp_cur_s2bit = tmp_cur_s1bit;
;   tmp_res1 = ({   SI tmp_tmp;
  tmp_tmp = SUBHI (SUBWORDSIHI (GET_H_CR (FLD (f_op_B)), 0), SUBWORDSIHI ((FLD (f_u6)), 0));
; (GTSI (tmp_tmp, 32767)) ? (  tmp_cur_s1bit = 1, 32767) : (LTSI (tmp_tmp, -32768)) ? (  tmp_cur_s1bit = 1, -32768) : (tmp_tmp); });
; ORHI (SLLHI (tmp_res1, 16), tmp_res2); }));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ORBI (tmp_cur_s1bit, tmp_cur_s2bit);
    CPU (h_vbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
}
if (tmp_cur_s2bit) {
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SUBSDW_L_U6__RA_) : /* subsdw$_L$F $RA,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (({   SI tmp_C_SI;
  HI tmp_res1;
  HI tmp_res2;
  tmp_res2 = ({   SI tmp_tmp;
  tmp_tmp = SUBHI (SUBWORDSIHI (GET_H_CR (FLD (f_op_B)), 1), SUBWORDSIHI ((FLD (f_u6)), 1));
; (GTSI (tmp_tmp, 32767)) ? (  tmp_cur_s1bit = 1, 32767) : (LTSI (tmp_tmp, -32768)) ? (  tmp_cur_s1bit = 1, -32768) : (tmp_tmp); });
;   tmp_cur_s2bit = tmp_cur_s1bit;
;   tmp_res1 = ({   SI tmp_tmp;
  tmp_tmp = SUBHI (SUBWORDSIHI (GET_H_CR (FLD (f_op_B)), 0), SUBWORDSIHI ((FLD (f_u6)), 0));
; (GTSI (tmp_tmp, 32767)) ? (  tmp_cur_s1bit = 1, 32767) : (LTSI (tmp_tmp, -32768)) ? (  tmp_cur_s1bit = 1, -32768) : (tmp_tmp); });
; ORHI (SLLHI (tmp_res1, 16), tmp_res2); }));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ORBI (tmp_cur_s1bit, tmp_cur_s2bit);
    CPU (h_vbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
}
if (tmp_cur_s2bit) {
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SUBSDW_L_R_R__RA__RC) : /* subsdw$_L$F $RA,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (({   SI tmp_C_SI;
  HI tmp_res1;
  HI tmp_res2;
  tmp_res2 = ({   SI tmp_tmp;
  tmp_tmp = SUBHI (SUBWORDSIHI (GET_H_CR (FLD (f_op_B)), 1), SUBWORDSIHI ((GET_H_CR (FLD (f_op_C))), 1));
; (GTSI (tmp_tmp, 32767)) ? (  tmp_cur_s1bit = 1, 32767) : (LTSI (tmp_tmp, -32768)) ? (  tmp_cur_s1bit = 1, -32768) : (tmp_tmp); });
;   tmp_cur_s2bit = tmp_cur_s1bit;
;   tmp_res1 = ({   SI tmp_tmp;
  tmp_tmp = SUBHI (SUBWORDSIHI (GET_H_CR (FLD (f_op_B)), 0), SUBWORDSIHI ((GET_H_CR (FLD (f_op_C))), 0));
; (GTSI (tmp_tmp, 32767)) ? (  tmp_cur_s1bit = 1, 32767) : (LTSI (tmp_tmp, -32768)) ? (  tmp_cur_s1bit = 1, -32768) : (tmp_tmp); });
; ORHI (SLLHI (tmp_res1, 16), tmp_res2); }));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ORBI (tmp_cur_s1bit, tmp_cur_s2bit);
    CPU (h_vbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
}
if (tmp_cur_s2bit) {
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SUBSDW_CC__RA__RC) : /* subsdw$Qcondi$F $RB,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (({   SI tmp_C_SI;
  HI tmp_res1;
  HI tmp_res2;
  tmp_res2 = ({   SI tmp_tmp;
  tmp_tmp = SUBHI (SUBWORDSIHI (GET_H_CR (FLD (f_op_B)), 1), SUBWORDSIHI ((GET_H_CR (FLD (f_op_C))), 1));
; (GTSI (tmp_tmp, 32767)) ? (  tmp_cur_s1bit = 1, 32767) : (LTSI (tmp_tmp, -32768)) ? (  tmp_cur_s1bit = 1, -32768) : (tmp_tmp); });
;   tmp_cur_s2bit = tmp_cur_s1bit;
;   tmp_res1 = ({   SI tmp_tmp;
  tmp_tmp = SUBHI (SUBWORDSIHI (GET_H_CR (FLD (f_op_B)), 0), SUBWORDSIHI ((GET_H_CR (FLD (f_op_C))), 0));
; (GTSI (tmp_tmp, 32767)) ? (  tmp_cur_s1bit = 1, 32767) : (LTSI (tmp_tmp, -32768)) ? (  tmp_cur_s1bit = 1, -32768) : (tmp_tmp); });
; ORHI (SLLHI (tmp_res1, 16), tmp_res2); }));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = ORBI (tmp_cur_s1bit, tmp_cur_s2bit);
    CPU (h_vbit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
}
if (tmp_cur_s2bit) {
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SWAP_L_R_R__RC) : /* swap$_L$F $RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_C), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_C), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (RORSI (GET_H_CR (FLD (f_op_C)), 16));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_SWAP_L_U6_) : /* swap$_L$F $RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (RORSI (FLD (f_u6), 16));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_NORM_L_R_R__RC) : /* norm$_L$F $RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_C), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_C), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (({   SI tmp_val;
  SI tmp_bits;
  tmp_val = ((GESI ((GET_H_CR (FLD (f_op_C))), 0)) ? ((GET_H_CR (FLD (f_op_C)))) : (INVSI ((GET_H_CR (FLD (f_op_C))))));
;   tmp_bits = 31;
; if (GESI (tmp_val, SLLSI (1, SUBSI (SLLSI (1, 4), 1)))) {
  tmp_val = SRLSI (tmp_val, SLLSI (1, 4));
  tmp_bits = SUBSI (tmp_bits, SLLSI (1, 4));
}
; if (GESI (tmp_val, SLLSI (1, SUBSI (SLLSI (1, 3), 1)))) {
  tmp_val = SRLSI (tmp_val, SLLSI (1, 3));
  tmp_bits = SUBSI (tmp_bits, SLLSI (1, 3));
}
; if (GESI (tmp_val, SLLSI (1, SUBSI (SLLSI (1, 2), 1)))) {
  tmp_val = SRLSI (tmp_val, SLLSI (1, 2));
  tmp_bits = SUBSI (tmp_bits, SLLSI (1, 2));
}
; if (GESI (tmp_val, SLLSI (1, SUBSI (SLLSI (1, 1), 1)))) {
  tmp_val = SRLSI (tmp_val, SLLSI (1, 1));
  tmp_bits = SUBSI (tmp_bits, SLLSI (1, 1));
}
; if (GESI (tmp_val, SLLSI (1, SUBSI (SLLSI (1, 0), 1)))) {
  tmp_val = SRLSI (tmp_val, SLLSI (1, 0));
  tmp_bits = SUBSI (tmp_bits, SLLSI (1, 0));
}
; tmp_bits; }));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (GET_H_CR (FLD (f_op_C)), 0);
    CPU (h_nbit) = opval;
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (GET_H_CR (FLD (f_op_C)), 0);
    CPU (h_zbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_NORM_L_U6_) : /* norm$_L$F $RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (({   SI tmp_val;
  SI tmp_bits;
  tmp_val = ((GESI ((FLD (f_u6)), 0)) ? ((FLD (f_u6))) : (INVSI ((FLD (f_u6)))));
;   tmp_bits = 31;
; if (GESI (tmp_val, SLLSI (1, SUBSI (SLLSI (1, 4), 1)))) {
  tmp_val = SRLSI (tmp_val, SLLSI (1, 4));
  tmp_bits = SUBSI (tmp_bits, SLLSI (1, 4));
}
; if (GESI (tmp_val, SLLSI (1, SUBSI (SLLSI (1, 3), 1)))) {
  tmp_val = SRLSI (tmp_val, SLLSI (1, 3));
  tmp_bits = SUBSI (tmp_bits, SLLSI (1, 3));
}
; if (GESI (tmp_val, SLLSI (1, SUBSI (SLLSI (1, 2), 1)))) {
  tmp_val = SRLSI (tmp_val, SLLSI (1, 2));
  tmp_bits = SUBSI (tmp_bits, SLLSI (1, 2));
}
; if (GESI (tmp_val, SLLSI (1, SUBSI (SLLSI (1, 1), 1)))) {
  tmp_val = SRLSI (tmp_val, SLLSI (1, 1));
  tmp_bits = SUBSI (tmp_bits, SLLSI (1, 1));
}
; if (GESI (tmp_val, SLLSI (1, SUBSI (SLLSI (1, 0), 1)))) {
  tmp_val = SRLSI (tmp_val, SLLSI (1, 0));
  tmp_bits = SUBSI (tmp_bits, SLLSI (1, 0));
}
; tmp_bits; }));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (FLD (f_u6), 0);
    CPU (h_nbit) = opval;
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (FLD (f_u6), 0);
    CPU (h_zbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_RND16_L_R_R__RC) : /* rnd16$_L$F $RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_C), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_C), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (SRLSI (({   DI tmp_tmp;
  tmp_tmp = ADDDI (EXTSIDI (32768), EXTSIDI (GET_H_CR (FLD (f_op_C))));
; ((GTDI (tmp_tmp, 2147483647)) ? (  tmp_cur_s1bit = 1, 2147483647) : (LTDI (tmp_tmp, ADDSI (-2147483647, -1))) ? (  tmp_cur_s1bit = 1, ADDSI (-2147483647, -1)) : (  tmp_cur_s1bit = 0, tmp_tmp)); }), 16));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_RND16_L_U6_) : /* rnd16$_L$F $RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (SRLSI (({   DI tmp_tmp;
  tmp_tmp = ADDDI (EXTSIDI (32768), EXTSIDI (FLD (f_u6)));
; ((GTDI (tmp_tmp, 2147483647)) ? (  tmp_cur_s1bit = 1, 2147483647) : (LTDI (tmp_tmp, ADDSI (-2147483647, -1))) ? (  tmp_cur_s1bit = 1, ADDSI (-2147483647, -1)) : (  tmp_cur_s1bit = 0, tmp_tmp)); }), 16));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ABSSW_L_R_R__RC) : /* abssw$_L$F $RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_C), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_C), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (({   SI tmp_tmp;
  tmp_tmp = ABSSI (EXTHISI ((GET_H_CR (FLD (f_op_C)))));
; (GTSI (tmp_tmp, 32767)) ? (  tmp_cur_s1bit = 1, 32767) : (LTSI (tmp_tmp, -32768)) ? (  tmp_cur_s1bit = 1, -32768) : (tmp_tmp); }));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ABSSW_L_U6_) : /* abssw$_L$F $RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (({   SI tmp_tmp;
  tmp_tmp = ABSSI (EXTHISI ((FLD (f_u6))));
; (GTSI (tmp_tmp, 32767)) ? (  tmp_cur_s1bit = 1, 32767) : (LTSI (tmp_tmp, -32768)) ? (  tmp_cur_s1bit = 1, -32768) : (tmp_tmp); }));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ABSS_L_R_R__RC) : /* abss$_L$F $RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_C), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_C), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (((GESI ((GET_H_CR (FLD (f_op_C))), 0)) ? (GET_H_CR (FLD (f_op_C))) : (({   DI tmp_tmp;
  tmp_tmp = SUBDI (EXTSIDI (0), EXTSIDI (GET_H_CR (FLD (f_op_C))));
; ((GTDI (tmp_tmp, 2147483647)) ? (  tmp_cur_s1bit = 1, 2147483647) : (LTDI (tmp_tmp, ADDSI (-2147483647, -1))) ? (  tmp_cur_s1bit = 1, ADDSI (-2147483647, -1)) : (  tmp_cur_s1bit = 0, tmp_tmp)); }))));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ABSS_L_U6_) : /* abss$_L$F $RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (((GESI ((FLD (f_u6)), 0)) ? (FLD (f_u6)) : (({   DI tmp_tmp;
  tmp_tmp = SUBDI (EXTSIDI (0), EXTSIDI (FLD (f_u6)));
; ((GTDI (tmp_tmp, 2147483647)) ? (  tmp_cur_s1bit = 1, 2147483647) : (LTDI (tmp_tmp, ADDSI (-2147483647, -1))) ? (  tmp_cur_s1bit = 1, ADDSI (-2147483647, -1)) : (  tmp_cur_s1bit = 0, tmp_tmp)); }))));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_NEGSW_L_R_R__RC) : /* negsw$_L$F $RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_C), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_C), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (({   SI tmp_tmp;
  tmp_tmp = EXTHISI ((GET_H_CR (FLD (f_op_C))));
; (GTSI (tmp_tmp, 32767)) ? (  tmp_cur_s1bit = 1, 32767) : (LTSI (tmp_tmp, -32768)) ? (  tmp_cur_s1bit = 1, -32768) : (tmp_tmp); }));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_NEGSW_L_U6_) : /* negsw$_L$F $RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (({   SI tmp_tmp;
  tmp_tmp = EXTHISI ((FLD (f_u6)));
; (GTSI (tmp_tmp, 32767)) ? (  tmp_cur_s1bit = 1, 32767) : (LTSI (tmp_tmp, -32768)) ? (  tmp_cur_s1bit = 1, -32768) : (tmp_tmp); }));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_NEGS_L_R_R__RC) : /* negs$_L$F $RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_C), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_C), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (({   DI tmp_tmp;
  tmp_tmp = SUBDI (EXTSIDI (0), EXTSIDI (GET_H_CR (FLD (f_op_C))));
; ((GTDI (tmp_tmp, 2147483647)) ? (  tmp_cur_s1bit = 1, 2147483647) : (LTDI (tmp_tmp, ADDSI (-2147483647, -1))) ? (  tmp_cur_s1bit = 1, ADDSI (-2147483647, -1)) : (  tmp_cur_s1bit = 0, tmp_tmp)); }));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_NEGS_L_U6_) : /* negs$_L$F $RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (({   DI tmp_tmp;
  tmp_tmp = SUBDI (EXTSIDI (0), EXTSIDI (FLD (f_u6)));
; ((GTDI (tmp_tmp, 2147483647)) ? (  tmp_cur_s1bit = 1, 2147483647) : (LTDI (tmp_tmp, ADDSI (-2147483647, -1))) ? (  tmp_cur_s1bit = 1, ADDSI (-2147483647, -1)) : (  tmp_cur_s1bit = 0, tmp_tmp)); }));
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_NORMW_L_R_R__RC) : /* normw$_L$F $RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_C), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_C), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (({   SI tmp_val;
  SI tmp_bits;
  tmp_val = ((GESI (ORSI (SLLSI (GET_H_CR (FLD (f_op_C)), 16), ANDSI (GET_H_CR (FLD (f_op_C)), 65535)), 0)) ? (ORSI (SLLSI (GET_H_CR (FLD (f_op_C)), 16), ANDSI (GET_H_CR (FLD (f_op_C)), 65535))) : (INVSI (ORSI (SLLSI (GET_H_CR (FLD (f_op_C)), 16), ANDSI (GET_H_CR (FLD (f_op_C)), 65535)))));
;   tmp_bits = 31;
; if (GESI (tmp_val, SLLSI (1, SUBSI (SLLSI (1, 4), 1)))) {
  tmp_val = SRLSI (tmp_val, SLLSI (1, 4));
  tmp_bits = SUBSI (tmp_bits, SLLSI (1, 4));
}
; if (GESI (tmp_val, SLLSI (1, SUBSI (SLLSI (1, 3), 1)))) {
  tmp_val = SRLSI (tmp_val, SLLSI (1, 3));
  tmp_bits = SUBSI (tmp_bits, SLLSI (1, 3));
}
; if (GESI (tmp_val, SLLSI (1, SUBSI (SLLSI (1, 2), 1)))) {
  tmp_val = SRLSI (tmp_val, SLLSI (1, 2));
  tmp_bits = SUBSI (tmp_bits, SLLSI (1, 2));
}
; if (GESI (tmp_val, SLLSI (1, SUBSI (SLLSI (1, 1), 1)))) {
  tmp_val = SRLSI (tmp_val, SLLSI (1, 1));
  tmp_bits = SUBSI (tmp_bits, SLLSI (1, 1));
}
; if (GESI (tmp_val, SLLSI (1, SUBSI (SLLSI (1, 0), 1)))) {
  tmp_val = SRLSI (tmp_val, SLLSI (1, 0));
  tmp_bits = SUBSI (tmp_bits, SLLSI (1, 0));
}
; tmp_bits; }));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_NORMW_L_U6_) : /* normw$_L$F $RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (({   SI tmp_val;
  SI tmp_bits;
  tmp_val = ((GESI (ORSI (SLLSI (FLD (f_u6), 16), ANDSI (FLD (f_u6), 65535)), 0)) ? (ORSI (SLLSI (FLD (f_u6), 16), ANDSI (FLD (f_u6), 65535))) : (INVSI (ORSI (SLLSI (FLD (f_u6), 16), ANDSI (FLD (f_u6), 65535)))));
;   tmp_bits = 31;
; if (GESI (tmp_val, SLLSI (1, SUBSI (SLLSI (1, 4), 1)))) {
  tmp_val = SRLSI (tmp_val, SLLSI (1, 4));
  tmp_bits = SUBSI (tmp_bits, SLLSI (1, 4));
}
; if (GESI (tmp_val, SLLSI (1, SUBSI (SLLSI (1, 3), 1)))) {
  tmp_val = SRLSI (tmp_val, SLLSI (1, 3));
  tmp_bits = SUBSI (tmp_bits, SLLSI (1, 3));
}
; if (GESI (tmp_val, SLLSI (1, SUBSI (SLLSI (1, 2), 1)))) {
  tmp_val = SRLSI (tmp_val, SLLSI (1, 2));
  tmp_bits = SUBSI (tmp_bits, SLLSI (1, 2));
}
; if (GESI (tmp_val, SLLSI (1, SUBSI (SLLSI (1, 1), 1)))) {
  tmp_val = SRLSI (tmp_val, SLLSI (1, 1));
  tmp_bits = SUBSI (tmp_bits, SLLSI (1, 1));
}
; if (GESI (tmp_val, SLLSI (1, SUBSI (SLLSI (1, 0), 1)))) {
  tmp_val = SRLSI (tmp_val, SLLSI (1, 0));
  tmp_bits = SUBSI (tmp_bits, SLLSI (1, 0));
}
; tmp_bits; }));
if (FLD (f_F)) {
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_NOP_S) : /* nop_s */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.fmt_empty.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

((void) 0); /*nop*/

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_UNIMP_S) : /* unimp_s */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.fmt_empty.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

cgen_rtx_error (current_cpu, "invalid insn");

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_POP_S_B) : /* pop$_S $R_b */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_mov_s_r_u7.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  {
    SI opval = GETMEMSI (current_cpu, pc, GET_H_SP ());
    SET_H_CR16 (FLD (f_op__b), opval);
    TRACE_RESULT (current_cpu, abuf, "cr16", 'x', opval);
  }
  {
    SI opval = ADDSI (GET_H_SP (), 4);
    SET_H_SP (, opval);
    TRACE_RESULT (current_cpu, abuf, "sp", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_POP_S_BLINK) : /* pop$_S $R31 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.fmt_empty.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  {
    SI opval = GETMEMSI (current_cpu, pc, GET_H_SP ());
    SET_H_R31 (, opval);
    TRACE_RESULT (current_cpu, abuf, "r31", 'x', opval);
  }
  {
    SI opval = ADDSI (GET_H_SP (), 4);
    SET_H_SP (, opval);
    TRACE_RESULT (current_cpu, abuf, "sp", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_PUSH_S_B) : /* push$_S $R_b */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_mov_s_r_u7.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  {
    SI opval = ADDSI (GET_H_SP (), -4);
    SET_H_SP (, opval);
    TRACE_RESULT (current_cpu, abuf, "sp", 'x', opval);
  }
  {
    SI opval = GET_H_CR16 (FLD (f_op__b));
    SETMEMSI (current_cpu, pc, GET_H_SP (), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_PUSH_S_BLINK) : /* push$_S $R31 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.fmt_empty.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  {
    SI opval = ADDSI (GET_H_SP (), -4);
    SET_H_SP (, opval);
    TRACE_RESULT (current_cpu, abuf, "sp", 'x', opval);
  }
  {
    SI opval = GET_H_R31 ();
    SETMEMSI (current_cpu, pc, GET_H_SP (), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}

#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MULLW_L_S12__RA_) : /* mullw$_L$F $RB,$RB,$s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ({   DI tmp_tmp;
  tmp_tmp = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (MULDI (EXTSIDI (GET_H_CR (FLD (f_op_B))), EXTSIDI (ANDSI (FLD (f_s12), 65535))));
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 1);
    SET_H_CR (((UINT) 57), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 0);
    SET_H_CR (((UINT) 56), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
; ((GTDI (tmp_tmp, 2147483647)) ? (  tmp_cur_s1bit = 1, 2147483647) : (LTDI (tmp_tmp, ADDSI (-2147483647, -1))) ? (  tmp_cur_s1bit = 1, ADDSI (-2147483647, -1)) : (  tmp_cur_s1bit = 0, tmp_tmp)); });
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MULLW_CCU6__RA_) : /* mullw$Qcondi$F $RB,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = ({   DI tmp_tmp;
  tmp_tmp = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (MULDI (EXTSIDI (GET_H_CR (FLD (f_op_B))), EXTSIDI (ANDSI (FLD (f_u6), 65535))));
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 1);
    SET_H_CR (((UINT) 57), opval);
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 0);
    SET_H_CR (((UINT) 56), opval);
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
; ((GTDI (tmp_tmp, 2147483647)) ? (  tmp_cur_s1bit = 1, 2147483647) : (LTDI (tmp_tmp, ADDSI (-2147483647, -1))) ? (  tmp_cur_s1bit = 1, ADDSI (-2147483647, -1)) : (  tmp_cur_s1bit = 0, tmp_tmp)); });
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MULLW_L_U6__RA_) : /* mullw$_L$F $RA,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ({   DI tmp_tmp;
  tmp_tmp = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (MULDI (EXTSIDI (GET_H_CR (FLD (f_op_B))), EXTSIDI (ANDSI (FLD (f_u6), 65535))));
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 1);
    SET_H_CR (((UINT) 57), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 0);
    SET_H_CR (((UINT) 56), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
; ((GTDI (tmp_tmp, 2147483647)) ? (  tmp_cur_s1bit = 1, 2147483647) : (LTDI (tmp_tmp, ADDSI (-2147483647, -1))) ? (  tmp_cur_s1bit = 1, ADDSI (-2147483647, -1)) : (  tmp_cur_s1bit = 0, tmp_tmp)); });
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MULLW_L_R_R__RA__RC) : /* mullw$_L$F $RA,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ({   DI tmp_tmp;
  tmp_tmp = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (MULDI (EXTSIDI (GET_H_CR (FLD (f_op_B))), EXTSIDI (ANDSI (GET_H_CR (FLD (f_op_C)), 65535))));
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 1);
    SET_H_CR (((UINT) 57), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 0);
    SET_H_CR (((UINT) 56), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
; ((GTDI (tmp_tmp, 2147483647)) ? (  tmp_cur_s1bit = 1, 2147483647) : (LTDI (tmp_tmp, ADDSI (-2147483647, -1))) ? (  tmp_cur_s1bit = 1, ADDSI (-2147483647, -1)) : (  tmp_cur_s1bit = 0, tmp_tmp)); });
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MULLW_CC__RA__RC) : /* mullw$Qcondi$F $RB,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = ({   DI tmp_tmp;
  tmp_tmp = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (MULDI (EXTSIDI (GET_H_CR (FLD (f_op_B))), EXTSIDI (ANDSI (GET_H_CR (FLD (f_op_C)), 65535))));
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 1);
    SET_H_CR (((UINT) 57), opval);
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 0);
    SET_H_CR (((UINT) 56), opval);
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
; ((GTDI (tmp_tmp, 2147483647)) ? (  tmp_cur_s1bit = 1, 2147483647) : (LTDI (tmp_tmp, ADDSI (-2147483647, -1))) ? (  tmp_cur_s1bit = 1, ADDSI (-2147483647, -1)) : (  tmp_cur_s1bit = 0, tmp_tmp)); });
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MACLW_L_S12__RA_) : /* maclw$_L$F $RB,$RB,$s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ({   DI tmp_old;
  DI tmp_tmp;
  SI tmp_SItmp;
  tmp_old = ADDDI (SLLDI (ZEXTSIDI (GET_H_CR (((UINT) 56))), 32), ZEXTSIDI (GET_H_CR (((UINT) 57))));
;   tmp_tmp = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (MULDI (EXTSIDI (GET_H_CR (FLD (f_op_B))), EXTSIDI (ANDSI (FLD (f_s12), 65535))));
;   {
    BI opval = NOTDI (SRLDI (XORDI (tmp_old, tmp_tmp), 63));
    CPU (h_vbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
;   tmp_tmp = ADDDI (tmp_old, tmp_tmp);
;   {
    BI opval = ANDBI (CPU (h_vbit), SRLDI (XORDI (tmp_old, tmp_tmp), 63));
    CPU (h_vbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
; if (NEBI (CPU (h_vbit), 0)) {
  tmp_tmp = XORDI (SRADI (tmp_old, 63), SRLSI (-1, 1));
}
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 1);
    SET_H_CR (((UINT) 57), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 0);
    SET_H_CR (((UINT) 56), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
;   tmp_SItmp = ((GTDI (tmp_tmp, 2147483647)) ? (  tmp_cur_s1bit = 1, 2147483647) : (LTDI (tmp_tmp, ADDSI (-2147483647, -1))) ? (  tmp_cur_s1bit = 1, ADDSI (-2147483647, -1)) : (  tmp_cur_s1bit = 0, tmp_tmp));
;   tmp_cur_s1bit = ORBI (tmp_cur_s1bit, CPU (h_vbit));
; tmp_SItmp; });
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MACLW_CCU6__RA_) : /* maclw$Qcondi$F $RB,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = ({   DI tmp_old;
  DI tmp_tmp;
  SI tmp_SItmp;
  tmp_old = ADDDI (SLLDI (ZEXTSIDI (GET_H_CR (((UINT) 56))), 32), ZEXTSIDI (GET_H_CR (((UINT) 57))));
;   tmp_tmp = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (MULDI (EXTSIDI (GET_H_CR (FLD (f_op_B))), EXTSIDI (ANDSI (FLD (f_u6), 65535))));
;   {
    BI opval = NOTDI (SRLDI (XORDI (tmp_old, tmp_tmp), 63));
    CPU (h_vbit) = opval;
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
;   tmp_tmp = ADDDI (tmp_old, tmp_tmp);
;   {
    BI opval = ANDBI (CPU (h_vbit), SRLDI (XORDI (tmp_old, tmp_tmp), 63));
    CPU (h_vbit) = opval;
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
; if (NEBI (CPU (h_vbit), 0)) {
  tmp_tmp = XORDI (SRADI (tmp_old, 63), SRLSI (-1, 1));
}
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 1);
    SET_H_CR (((UINT) 57), opval);
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 0);
    SET_H_CR (((UINT) 56), opval);
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
;   tmp_SItmp = ((GTDI (tmp_tmp, 2147483647)) ? (  tmp_cur_s1bit = 1, 2147483647) : (LTDI (tmp_tmp, ADDSI (-2147483647, -1))) ? (  tmp_cur_s1bit = 1, ADDSI (-2147483647, -1)) : (  tmp_cur_s1bit = 0, tmp_tmp));
;   tmp_cur_s1bit = ORBI (tmp_cur_s1bit, CPU (h_vbit));
; tmp_SItmp; });
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MACLW_L_U6__RA_) : /* maclw$_L$F $RA,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ({   DI tmp_old;
  DI tmp_tmp;
  SI tmp_SItmp;
  tmp_old = ADDDI (SLLDI (ZEXTSIDI (GET_H_CR (((UINT) 56))), 32), ZEXTSIDI (GET_H_CR (((UINT) 57))));
;   tmp_tmp = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (MULDI (EXTSIDI (GET_H_CR (FLD (f_op_B))), EXTSIDI (ANDSI (FLD (f_u6), 65535))));
;   {
    BI opval = NOTDI (SRLDI (XORDI (tmp_old, tmp_tmp), 63));
    CPU (h_vbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
;   tmp_tmp = ADDDI (tmp_old, tmp_tmp);
;   {
    BI opval = ANDBI (CPU (h_vbit), SRLDI (XORDI (tmp_old, tmp_tmp), 63));
    CPU (h_vbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
; if (NEBI (CPU (h_vbit), 0)) {
  tmp_tmp = XORDI (SRADI (tmp_old, 63), SRLSI (-1, 1));
}
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 1);
    SET_H_CR (((UINT) 57), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 0);
    SET_H_CR (((UINT) 56), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
;   tmp_SItmp = ((GTDI (tmp_tmp, 2147483647)) ? (  tmp_cur_s1bit = 1, 2147483647) : (LTDI (tmp_tmp, ADDSI (-2147483647, -1))) ? (  tmp_cur_s1bit = 1, ADDSI (-2147483647, -1)) : (  tmp_cur_s1bit = 0, tmp_tmp));
;   tmp_cur_s1bit = ORBI (tmp_cur_s1bit, CPU (h_vbit));
; tmp_SItmp; });
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MACLW_L_R_R__RA__RC) : /* maclw$_L$F $RA,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ({   DI tmp_old;
  DI tmp_tmp;
  SI tmp_SItmp;
  tmp_old = ADDDI (SLLDI (ZEXTSIDI (GET_H_CR (((UINT) 56))), 32), ZEXTSIDI (GET_H_CR (((UINT) 57))));
;   tmp_tmp = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (MULDI (EXTSIDI (GET_H_CR (FLD (f_op_B))), EXTSIDI (ANDSI (GET_H_CR (FLD (f_op_C)), 65535))));
;   {
    BI opval = NOTDI (SRLDI (XORDI (tmp_old, tmp_tmp), 63));
    CPU (h_vbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
;   tmp_tmp = ADDDI (tmp_old, tmp_tmp);
;   {
    BI opval = ANDBI (CPU (h_vbit), SRLDI (XORDI (tmp_old, tmp_tmp), 63));
    CPU (h_vbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
; if (NEBI (CPU (h_vbit), 0)) {
  tmp_tmp = XORDI (SRADI (tmp_old, 63), SRLSI (-1, 1));
}
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 1);
    SET_H_CR (((UINT) 57), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 0);
    SET_H_CR (((UINT) 56), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
;   tmp_SItmp = ((GTDI (tmp_tmp, 2147483647)) ? (  tmp_cur_s1bit = 1, 2147483647) : (LTDI (tmp_tmp, ADDSI (-2147483647, -1))) ? (  tmp_cur_s1bit = 1, ADDSI (-2147483647, -1)) : (  tmp_cur_s1bit = 0, tmp_tmp));
;   tmp_cur_s1bit = ORBI (tmp_cur_s1bit, CPU (h_vbit));
; tmp_SItmp; });
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MACLW_CC__RA__RC) : /* maclw$Qcondi$F $RB,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = ({   DI tmp_old;
  DI tmp_tmp;
  SI tmp_SItmp;
  tmp_old = ADDDI (SLLDI (ZEXTSIDI (GET_H_CR (((UINT) 56))), 32), ZEXTSIDI (GET_H_CR (((UINT) 57))));
;   tmp_tmp = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (MULDI (EXTSIDI (GET_H_CR (FLD (f_op_B))), EXTSIDI (ANDSI (GET_H_CR (FLD (f_op_C)), 65535))));
;   {
    BI opval = NOTDI (SRLDI (XORDI (tmp_old, tmp_tmp), 63));
    CPU (h_vbit) = opval;
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
;   tmp_tmp = ADDDI (tmp_old, tmp_tmp);
;   {
    BI opval = ANDBI (CPU (h_vbit), SRLDI (XORDI (tmp_old, tmp_tmp), 63));
    CPU (h_vbit) = opval;
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
; if (NEBI (CPU (h_vbit), 0)) {
  tmp_tmp = XORDI (SRADI (tmp_old, 63), SRLSI (-1, 1));
}
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 1);
    SET_H_CR (((UINT) 57), opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 0);
    SET_H_CR (((UINT) 56), opval);
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
;   tmp_SItmp = ((GTDI (tmp_tmp, 2147483647)) ? (  tmp_cur_s1bit = 1, 2147483647) : (LTDI (tmp_tmp, ADDSI (-2147483647, -1))) ? (  tmp_cur_s1bit = 1, ADDSI (-2147483647, -1)) : (  tmp_cur_s1bit = 0, tmp_tmp));
;   tmp_cur_s1bit = ORBI (tmp_cur_s1bit, CPU (h_vbit));
; tmp_SItmp; });
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MACHLW_L_S12__RA_) : /* machlw$_L$F $RB,$RB,$s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ({   DI tmp_old;
  DI tmp_tmp;
  tmp_old = ADDDI (SLLDI (ZEXTSIDI (GET_H_CR (((UINT) 56))), 32), ZEXTSIDI (GET_H_CR (((UINT) 57))));
;   tmp_tmp = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (MULDI (EXTSIDI (GET_H_CR (FLD (f_op_B))), EXTSIDI (ANDSI (FLD (f_s12), -65536))));
;   {
    BI opval = NOTDI (SRLDI (XORDI (tmp_old, tmp_tmp), 63));
    CPU (h_vbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
;   tmp_tmp = ADDDI (tmp_old, tmp_tmp);
;   tmp_cur_s1bit = ANDBI (CPU (h_vbit), XORDI (tmp_old, tmp_tmp));
; if (tmp_cur_s1bit) {
  tmp_tmp = XORDI (SRADI (tmp_old, 63), SRLSI (-1, 1));
}
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 1);
    SET_H_CR (((UINT) 57), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 0);
    SET_H_CR (((UINT) 56), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
; SUBWORDDISI (tmp_tmp, 0); });
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MACHLW_CCU6__RA_) : /* machlw$Qcondi$F $RB,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = ({   DI tmp_old;
  DI tmp_tmp;
  tmp_old = ADDDI (SLLDI (ZEXTSIDI (GET_H_CR (((UINT) 56))), 32), ZEXTSIDI (GET_H_CR (((UINT) 57))));
;   tmp_tmp = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (MULDI (EXTSIDI (GET_H_CR (FLD (f_op_B))), EXTSIDI (ANDSI (FLD (f_u6), -65536))));
;   {
    BI opval = NOTDI (SRLDI (XORDI (tmp_old, tmp_tmp), 63));
    CPU (h_vbit) = opval;
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
;   tmp_tmp = ADDDI (tmp_old, tmp_tmp);
;   tmp_cur_s1bit = ANDBI (CPU (h_vbit), XORDI (tmp_old, tmp_tmp));
; if (tmp_cur_s1bit) {
  tmp_tmp = XORDI (SRADI (tmp_old, 63), SRLSI (-1, 1));
}
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 1);
    SET_H_CR (((UINT) 57), opval);
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 0);
    SET_H_CR (((UINT) 56), opval);
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
; SUBWORDDISI (tmp_tmp, 0); });
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MACHLW_L_U6__RA_) : /* machlw$_L$F $RA,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ({   DI tmp_old;
  DI tmp_tmp;
  tmp_old = ADDDI (SLLDI (ZEXTSIDI (GET_H_CR (((UINT) 56))), 32), ZEXTSIDI (GET_H_CR (((UINT) 57))));
;   tmp_tmp = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (MULDI (EXTSIDI (GET_H_CR (FLD (f_op_B))), EXTSIDI (ANDSI (FLD (f_u6), -65536))));
;   {
    BI opval = NOTDI (SRLDI (XORDI (tmp_old, tmp_tmp), 63));
    CPU (h_vbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
;   tmp_tmp = ADDDI (tmp_old, tmp_tmp);
;   tmp_cur_s1bit = ANDBI (CPU (h_vbit), XORDI (tmp_old, tmp_tmp));
; if (tmp_cur_s1bit) {
  tmp_tmp = XORDI (SRADI (tmp_old, 63), SRLSI (-1, 1));
}
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 1);
    SET_H_CR (((UINT) 57), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 0);
    SET_H_CR (((UINT) 56), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
; SUBWORDDISI (tmp_tmp, 0); });
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MACHLW_L_R_R__RA__RC) : /* machlw$_L$F $RA,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ({   DI tmp_old;
  DI tmp_tmp;
  tmp_old = ADDDI (SLLDI (ZEXTSIDI (GET_H_CR (((UINT) 56))), 32), ZEXTSIDI (GET_H_CR (((UINT) 57))));
;   tmp_tmp = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (MULDI (EXTSIDI (GET_H_CR (FLD (f_op_B))), EXTSIDI (ANDSI (GET_H_CR (FLD (f_op_C)), -65536))));
;   {
    BI opval = NOTDI (SRLDI (XORDI (tmp_old, tmp_tmp), 63));
    CPU (h_vbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
;   tmp_tmp = ADDDI (tmp_old, tmp_tmp);
;   tmp_cur_s1bit = ANDBI (CPU (h_vbit), XORDI (tmp_old, tmp_tmp));
; if (tmp_cur_s1bit) {
  tmp_tmp = XORDI (SRADI (tmp_old, 63), SRLSI (-1, 1));
}
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 1);
    SET_H_CR (((UINT) 57), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 0);
    SET_H_CR (((UINT) 56), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
; SUBWORDDISI (tmp_tmp, 0); });
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MACHLW_CC__RA__RC) : /* machlw$Qcondi$F $RB,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = ({   DI tmp_old;
  DI tmp_tmp;
  tmp_old = ADDDI (SLLDI (ZEXTSIDI (GET_H_CR (((UINT) 56))), 32), ZEXTSIDI (GET_H_CR (((UINT) 57))));
;   tmp_tmp = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (MULDI (EXTSIDI (GET_H_CR (FLD (f_op_B))), EXTSIDI (ANDSI (GET_H_CR (FLD (f_op_C)), -65536))));
;   {
    BI opval = NOTDI (SRLDI (XORDI (tmp_old, tmp_tmp), 63));
    CPU (h_vbit) = opval;
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
;   tmp_tmp = ADDDI (tmp_old, tmp_tmp);
;   tmp_cur_s1bit = ANDBI (CPU (h_vbit), XORDI (tmp_old, tmp_tmp));
; if (tmp_cur_s1bit) {
  tmp_tmp = XORDI (SRADI (tmp_old, 63), SRLSI (-1, 1));
}
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 1);
    SET_H_CR (((UINT) 57), opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 0);
    SET_H_CR (((UINT) 56), opval);
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
; SUBWORDDISI (tmp_tmp, 0); });
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MULULW_L_S12__RA_) : /* mululw$_L$F $RB,$RB,$s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ({   DI tmp_tmp;
  tmp_tmp = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (MULDI (ZEXTSIDI (GET_H_CR (FLD (f_op_B))), ZEXTSIDI (ANDSI (FLD (f_s12), 65535))));
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 1);
    SET_H_CR (((UINT) 57), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 0);
    SET_H_CR (((UINT) 56), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
; ((GTDI (tmp_tmp, 2147483647)) ? (  tmp_cur_s1bit = 1, 2147483647) : (LTDI (tmp_tmp, ADDSI (-2147483647, -1))) ? (  tmp_cur_s1bit = 1, ADDSI (-2147483647, -1)) : (  tmp_cur_s1bit = 0, tmp_tmp)); });
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MULULW_CCU6__RA_) : /* mululw$Qcondi$F $RB,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = ({   DI tmp_tmp;
  tmp_tmp = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (MULDI (ZEXTSIDI (GET_H_CR (FLD (f_op_B))), ZEXTSIDI (ANDSI (FLD (f_u6), 65535))));
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 1);
    SET_H_CR (((UINT) 57), opval);
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 0);
    SET_H_CR (((UINT) 56), opval);
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
; ((GTDI (tmp_tmp, 2147483647)) ? (  tmp_cur_s1bit = 1, 2147483647) : (LTDI (tmp_tmp, ADDSI (-2147483647, -1))) ? (  tmp_cur_s1bit = 1, ADDSI (-2147483647, -1)) : (  tmp_cur_s1bit = 0, tmp_tmp)); });
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MULULW_L_U6__RA_) : /* mululw$_L$F $RA,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ({   DI tmp_tmp;
  tmp_tmp = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (MULDI (ZEXTSIDI (GET_H_CR (FLD (f_op_B))), ZEXTSIDI (ANDSI (FLD (f_u6), 65535))));
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 1);
    SET_H_CR (((UINT) 57), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 0);
    SET_H_CR (((UINT) 56), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
; ((GTDI (tmp_tmp, 2147483647)) ? (  tmp_cur_s1bit = 1, 2147483647) : (LTDI (tmp_tmp, ADDSI (-2147483647, -1))) ? (  tmp_cur_s1bit = 1, ADDSI (-2147483647, -1)) : (  tmp_cur_s1bit = 0, tmp_tmp)); });
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MULULW_L_R_R__RA__RC) : /* mululw$_L$F $RA,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ({   DI tmp_tmp;
  tmp_tmp = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (MULDI (ZEXTSIDI (GET_H_CR (FLD (f_op_B))), ZEXTSIDI (ANDSI (GET_H_CR (FLD (f_op_C)), 65535))));
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 1);
    SET_H_CR (((UINT) 57), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 0);
    SET_H_CR (((UINT) 56), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
; ((GTDI (tmp_tmp, 2147483647)) ? (  tmp_cur_s1bit = 1, 2147483647) : (LTDI (tmp_tmp, ADDSI (-2147483647, -1))) ? (  tmp_cur_s1bit = 1, ADDSI (-2147483647, -1)) : (  tmp_cur_s1bit = 0, tmp_tmp)); });
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MULULW_CC__RA__RC) : /* mululw$Qcondi$F $RB,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = ({   DI tmp_tmp;
  tmp_tmp = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (MULDI (ZEXTSIDI (GET_H_CR (FLD (f_op_B))), ZEXTSIDI (ANDSI (GET_H_CR (FLD (f_op_C)), 65535))));
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 1);
    SET_H_CR (((UINT) 57), opval);
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 0);
    SET_H_CR (((UINT) 56), opval);
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
; ((GTDI (tmp_tmp, 2147483647)) ? (  tmp_cur_s1bit = 1, 2147483647) : (LTDI (tmp_tmp, ADDSI (-2147483647, -1))) ? (  tmp_cur_s1bit = 1, ADDSI (-2147483647, -1)) : (  tmp_cur_s1bit = 0, tmp_tmp)); });
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MACHULW_L_S12__RA_) : /* machulw$_L$F $RB,$RB,$s12 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ({   DI tmp_old;
  DI tmp_tmp;
  tmp_old = ADDDI (SLLDI (ZEXTSIDI (GET_H_CR (((UINT) 56))), 32), ZEXTSIDI (GET_H_CR (((UINT) 57))));
;   tmp_tmp = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (MULDI (ZEXTSIDI (GET_H_CR (FLD (f_op_B))), ZEXTSIDI (ANDSI (FLD (f_s12), -65536))));
;   tmp_tmp = ADDDI (tmp_old, tmp_tmp);
;   tmp_cur_s1bit = (GTUDI (tmp_old, tmp_tmp)) ? ((  tmp_tmp = -1, 1)) : (0);
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 1);
    SET_H_CR (((UINT) 57), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 0);
    SET_H_CR (((UINT) 56), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
; SUBWORDDISI (tmp_tmp, 0); });
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MACHULW_CCU6__RA_) : /* machulw$Qcondi$F $RB,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = ({   DI tmp_old;
  DI tmp_tmp;
  tmp_old = ADDDI (SLLDI (ZEXTSIDI (GET_H_CR (((UINT) 56))), 32), ZEXTSIDI (GET_H_CR (((UINT) 57))));
;   tmp_tmp = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (MULDI (ZEXTSIDI (GET_H_CR (FLD (f_op_B))), ZEXTSIDI (ANDSI (FLD (f_u6), -65536))));
;   tmp_tmp = ADDDI (tmp_old, tmp_tmp);
;   tmp_cur_s1bit = (GTUDI (tmp_old, tmp_tmp)) ? ((  tmp_tmp = -1, 1)) : (0);
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 1);
    SET_H_CR (((UINT) 57), opval);
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 0);
    SET_H_CR (((UINT) 56), opval);
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
; SUBWORDDISI (tmp_tmp, 0); });
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MACHULW_L_U6__RA_) : /* machulw$_L$F $RA,$RB,$U6 */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (EQSI (FLD (f_op_B), 62)) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (EQSI (FLD (f_op_B), 63)) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ({   DI tmp_old;
  DI tmp_tmp;
  tmp_old = ADDDI (SLLDI (ZEXTSIDI (GET_H_CR (((UINT) 56))), 32), ZEXTSIDI (GET_H_CR (((UINT) 57))));
;   tmp_tmp = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (MULDI (ZEXTSIDI (GET_H_CR (FLD (f_op_B))), ZEXTSIDI (ANDSI (FLD (f_u6), -65536))));
;   tmp_tmp = ADDDI (tmp_old, tmp_tmp);
;   tmp_cur_s1bit = (GTUDI (tmp_old, tmp_tmp)) ? ((  tmp_tmp = -1, 1)) : (0);
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 1);
    SET_H_CR (((UINT) 57), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 0);
    SET_H_CR (((UINT) 56), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
; SUBWORDDISI (tmp_tmp, 0); });
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MACHULW_L_R_R__RA__RC) : /* machulw$_L$F $RA,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
  tmp_result = ({   DI tmp_old;
  DI tmp_tmp;
  tmp_old = ADDDI (SLLDI (ZEXTSIDI (GET_H_CR (((UINT) 56))), 32), ZEXTSIDI (GET_H_CR (((UINT) 57))));
;   tmp_tmp = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (MULDI (ZEXTSIDI (GET_H_CR (FLD (f_op_B))), ZEXTSIDI (ANDSI (GET_H_CR (FLD (f_op_C)), -65536))));
;   tmp_tmp = ADDDI (tmp_old, tmp_tmp);
;   tmp_cur_s1bit = (GTUDI (tmp_old, tmp_tmp)) ? ((  tmp_tmp = -1, 1)) : (0);
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 1);
    SET_H_CR (((UINT) 57), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 0);
    SET_H_CR (((UINT) 56), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
; SUBWORDDISI (tmp_tmp, 0); });
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_A), opval);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_MACHULW_CC__RA__RC) : /* machulw$Qcondi$F $RB,$RB,$RC */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
{
if (ORBI (EQSI (FLD (f_op_B), 62), EQSI (FLD (f_op_C), 62))) {
{
  HI tmp_high;
  HI tmp_low;
  tmp_high = GETMEMHI (current_cpu, pc, ADDSI (pc, 4));
  tmp_low = GETMEMHI (current_cpu, pc, ADDSI (pc, ADDSI (4, 2)));
CPU (h_cr[((UINT) 62)]) = ORSI (SLLSI (ZEXTHISI (tmp_high), 16), ZEXTHISI (tmp_low));
}
}
if (ORBI (EQSI (FLD (f_op_B), 63), EQSI (FLD (f_op_C), 63))) {
CPU (h_cr[((UINT) 63)]) = ANDSI (pc, -4);
}
}
if (GET_H_QCONDB (FLD (f_cond_Q))) {
{
  SI tmp_result;
  BI tmp_cur_s1bit;
  BI tmp_cur_s2bit;
((void) 0); /*nop*/
  tmp_result = ({   DI tmp_old;
  DI tmp_tmp;
  tmp_old = ADDDI (SLLDI (ZEXTSIDI (GET_H_CR (((UINT) 56))), 32), ZEXTSIDI (GET_H_CR (((UINT) 57))));
;   tmp_tmp = (0) ? ((cgen_rtx_error (current_cpu, "invalid insn"), 0)) : (MULDI (ZEXTSIDI (GET_H_CR (FLD (f_op_B))), ZEXTSIDI (ANDSI (GET_H_CR (FLD (f_op_C)), -65536))));
;   tmp_tmp = ADDDI (tmp_old, tmp_tmp);
;   tmp_cur_s1bit = (GTUDI (tmp_old, tmp_tmp)) ? ((  tmp_tmp = -1, 1)) : (0);
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 1);
    SET_H_CR (((UINT) 57), opval);
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
;   {
    SI opval = SUBWORDDISI (tmp_tmp, 0);
    SET_H_CR (((UINT) 56), opval);
    written |= (1 << 12);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
; SUBWORDDISI (tmp_tmp, 0); });
if (FLD (f_F)) {
{
{
  {
    BI opval = LTSI (tmp_result, 0);
    CPU (h_nbit) = opval;
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_result, 0);
    CPU (h_zbit) = opval;
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
  {
    BI opval = tmp_cur_s1bit;
    CPU (h_vbit) = opval;
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
if (tmp_cur_s1bit) {
  {
    BI opval = 1;
    CPU (h_s1bit) = opval;
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "s1bit", 'x', opval);
  }
  {
    BI opval = 1;
    CPU (h_s2bit) = opval;
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "s2bit", 'x', opval);
  }
}
}
}
  {
    SI opval = tmp_result;
    SET_H_CR (FLD (f_op_B), opval);
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
}

  abuf->written = written;
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_CURRENT_LOOP_END) : /*  */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.fmt_empty.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (ANDBI (EQSI (pc, GET_H_AUXR (((UINT) 3))), NOTBI (CPU (h_lbit)))) {
{
  {
    SI opval = ADDSI (GET_H_CR (((UINT) 60)), -1);
    SET_H_CR (((UINT) 60), opval);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
if (GET_H_CR (((UINT) 60))) {
if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (GET_H_AUXR (((UINT) 2)), 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
  {
    USI opval = GET_H_AUXR (((UINT) 2));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
 else if (0) {
  {
    USI opval = GET_H_AUXR (((UINT) 2));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
 else {
{
  {
    SI opval = GET_H_AUXR (((UINT) 2));
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
  {
    USI opval = GET_H_AUXR (((UINT) 2));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_CURRENT_LOOP_END_AFTER_BRANCH) : /*  */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.fmt_empty.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (
#ifdef SEM_IN_SWITCH
pbb_br_type != SEM_BRANCH_UNTAKEN
#else
CPU_PBB_BR_NPC (current_cpu) != SEM_BRANCH_UNTAKEN
#endif
) {

#ifdef SEM_IN_SWITCH
npc = pbb_br_npc; br_type = pbb_br_type;
#else
npc = CPU_PBB_BR_NPC (current_cpu); br_type = CPU_PBB_BR_TYPE (current_cpu);
#endif
}
 else if (ANDBI (EQSI (pc, GET_H_AUXR (((UINT) 3))), NOTBI (CPU (h_lbit)))) {
{
  {
    SI opval = ADDSI (GET_H_CR (((UINT) 60)), -1);
    SET_H_CR (((UINT) 60), opval);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
if (GET_H_CR (((UINT) 60))) {
if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (GET_H_AUXR (((UINT) 2)), 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
  {
    USI opval = GET_H_AUXR (((UINT) 2));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
 else if (0) {
  {
    USI opval = GET_H_AUXR (((UINT) 2));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
 else {
{
  {
    SI opval = GET_H_AUXR (((UINT) 2));
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
  {
    USI opval = GET_H_AUXR (((UINT) 2));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);

  CASE (sem, INSN_ARC600_CURRENT_LOOP_END_AFTER_BRANCH) : /*  */
{
  SEM_ARG sem_arg = SEM_SEM_ARG (vpc, sc);
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
#define FLD(f) abuf->fields.fmt_empty.f
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (
#ifdef SEM_IN_SWITCH
pbb_br_type != SEM_BRANCH_UNTAKEN
#else
CPU_PBB_BR_NPC (current_cpu) != SEM_BRANCH_UNTAKEN
#endif
) {
{

#ifdef SEM_IN_SWITCH
npc = pbb_br_npc; br_type = pbb_br_type;
#else
npc = CPU_PBB_BR_NPC (current_cpu); br_type = CPU_PBB_BR_TYPE (current_cpu);
#endif
  {
    SI opval = ADDSI (GET_H_CR (((UINT) 60)), -1);
    SET_H_CR (((UINT) 60), opval);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
}
}
 else if (ANDBI (EQSI (pc, GET_H_AUXR (((UINT) 3))), NOTBI (CPU (h_lbit)))) {
{
  {
    SI opval = ADDSI (GET_H_CR (((UINT) 60)), -1);
    SET_H_CR (((UINT) 60), opval);
    written |= (1 << 16);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
if (GET_H_CR (((UINT) 60))) {
if (ANDIF (GESI (SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)])), 0), ANDIF (CPU (h_e1), ANDSI (GET_H_AUXR (((UINT) 34)), 1)))) {
{
  {
    SI opval = SUBSI (CPU_INSN_COUNT (current_cpu), CPU (h_timer_expire[((UINT) 0)]));
    SET_H_AUXR (((UINT) 33), opval);
    written |= (1 << 14);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
if (CPU (h_ubit)) {
{
  SI tmp_countp;
  UHI tmp_count;
  tmp_countp = ADDSI (ANDSI (SRLSI (GET_H_AUXR (((UINT) 2)), 1), -2), CPU (h_prof_offset[((UINT) 0)]));
  tmp_count = ADDHI (GETMEMUHI (current_cpu, pc, tmp_countp), 1);
if (tmp_count) {
  {
    UHI opval = tmp_count;
    SETMEMUHI (current_cpu, pc, tmp_countp, opval);
    written |= (1 << 18);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
  {
    USI opval = GET_H_AUXR (((UINT) 2));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
 else if (0) {
  {
    USI opval = GET_H_AUXR (((UINT) 2));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
 else {
{
  {
    SI opval = GET_H_AUXR (((UINT) 2));
    SET_H_CR (((UINT) 29), opval);
    written |= (1 << 15);
    TRACE_RESULT (current_cpu, abuf, "cr", 'x', opval);
  }
  {
    SI opval = GET_H_STATUS32 (((UINT) 0));
    SET_H_AUXR (((UINT) 11), opval);
    written |= (1 << 13);
    TRACE_RESULT (current_cpu, abuf, "auxr", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_e1) = opval;
    written |= (1 << 17);
    TRACE_RESULT (current_cpu, abuf, "e1", 'x', opval);
  }
  {
    USI opval = ADDSI (GET_H_AUXR (((UINT) 37)), 24);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
} else {
  {
    USI opval = GET_H_AUXR (((UINT) 2));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 19);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
}
}
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
#undef FLD
}
  NEXT (vpc);


    }
  ENDSWITCH (sem) /* End of semantic switch.  */

  /* At this point `vpc' contains the next insn to execute.  */
}

#undef DEFINE_SWITCH
#endif /* DEFINE_SWITCH */
