/* Simulator instruction decoder for sh64_compact.

THIS FILE IS MACHINE GENERATED WITH CGEN.

Copyright (C) 1996, 1997, 1998, 1999, 2000 Free Software Foundation, Inc.

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

#define WANT_CPU sh64
#define WANT_CPU_SH64

#include "sim-main.h"
#include "sim-assert.h"

/* The instruction descriptor array.
   This is computed at runtime.  Space for it is not malloc'd to save a
   teensy bit of cpu in the decoder.  Moving it to malloc space is trivial
   but won't be done until necessary (we don't currently support the runtime
   addition of instructions nor an SMP machine with different cpus).  */
static IDESC sh64_compact_insn_data[SH64_COMPACT_INSN_MAX];

/* Commas between elements are contained in the macros.
   Some of these are conditionally compiled out.  */

static const struct insn_sem sh64_compact_insn_sem[] =
{
  { VIRTUAL_INSN_X_INVALID, SH64_COMPACT_INSN_X_INVALID, SH64_COMPACT_SFMT_EMPTY },
  { VIRTUAL_INSN_X_AFTER, SH64_COMPACT_INSN_X_AFTER, SH64_COMPACT_SFMT_EMPTY },
  { VIRTUAL_INSN_X_BEFORE, SH64_COMPACT_INSN_X_BEFORE, SH64_COMPACT_SFMT_EMPTY },
  { VIRTUAL_INSN_X_CTI_CHAIN, SH64_COMPACT_INSN_X_CTI_CHAIN, SH64_COMPACT_SFMT_EMPTY },
  { VIRTUAL_INSN_X_CHAIN, SH64_COMPACT_INSN_X_CHAIN, SH64_COMPACT_SFMT_EMPTY },
  { VIRTUAL_INSN_X_BEGIN, SH64_COMPACT_INSN_X_BEGIN, SH64_COMPACT_SFMT_EMPTY },
  { SH_INSN_ADD_COMPACT, SH64_COMPACT_INSN_ADD_COMPACT, SH64_COMPACT_SFMT_ADD_COMPACT },
  { SH_INSN_ADDI_COMPACT, SH64_COMPACT_INSN_ADDI_COMPACT, SH64_COMPACT_SFMT_ADDI_COMPACT },
  { SH_INSN_ADDC_COMPACT, SH64_COMPACT_INSN_ADDC_COMPACT, SH64_COMPACT_SFMT_ADDC_COMPACT },
  { SH_INSN_ADDV_COMPACT, SH64_COMPACT_INSN_ADDV_COMPACT, SH64_COMPACT_SFMT_ADDV_COMPACT },
  { SH_INSN_AND_COMPACT, SH64_COMPACT_INSN_AND_COMPACT, SH64_COMPACT_SFMT_AND_COMPACT },
  { SH_INSN_ANDI_COMPACT, SH64_COMPACT_INSN_ANDI_COMPACT, SH64_COMPACT_SFMT_ANDI_COMPACT },
  { SH_INSN_ANDB_COMPACT, SH64_COMPACT_INSN_ANDB_COMPACT, SH64_COMPACT_SFMT_ANDB_COMPACT },
  { SH_INSN_BF_COMPACT, SH64_COMPACT_INSN_BF_COMPACT, SH64_COMPACT_SFMT_BF_COMPACT },
  { SH_INSN_BFS_COMPACT, SH64_COMPACT_INSN_BFS_COMPACT, SH64_COMPACT_SFMT_BF_COMPACT },
  { SH_INSN_BRA_COMPACT, SH64_COMPACT_INSN_BRA_COMPACT, SH64_COMPACT_SFMT_BRA_COMPACT },
  { SH_INSN_BRAF_COMPACT, SH64_COMPACT_INSN_BRAF_COMPACT, SH64_COMPACT_SFMT_BRAF_COMPACT },
  { SH_INSN_BRK_COMPACT, SH64_COMPACT_INSN_BRK_COMPACT, SH64_COMPACT_SFMT_BRK_COMPACT },
  { SH_INSN_BSR_COMPACT, SH64_COMPACT_INSN_BSR_COMPACT, SH64_COMPACT_SFMT_BSR_COMPACT },
  { SH_INSN_BSRF_COMPACT, SH64_COMPACT_INSN_BSRF_COMPACT, SH64_COMPACT_SFMT_BSRF_COMPACT },
  { SH_INSN_BT_COMPACT, SH64_COMPACT_INSN_BT_COMPACT, SH64_COMPACT_SFMT_BF_COMPACT },
  { SH_INSN_BTS_COMPACT, SH64_COMPACT_INSN_BTS_COMPACT, SH64_COMPACT_SFMT_BF_COMPACT },
  { SH_INSN_CLRMAC_COMPACT, SH64_COMPACT_INSN_CLRMAC_COMPACT, SH64_COMPACT_SFMT_CLRMAC_COMPACT },
  { SH_INSN_CLRS_COMPACT, SH64_COMPACT_INSN_CLRS_COMPACT, SH64_COMPACT_SFMT_CLRS_COMPACT },
  { SH_INSN_CLRT_COMPACT, SH64_COMPACT_INSN_CLRT_COMPACT, SH64_COMPACT_SFMT_CLRT_COMPACT },
  { SH_INSN_CMPEQ_COMPACT, SH64_COMPACT_INSN_CMPEQ_COMPACT, SH64_COMPACT_SFMT_CMPEQ_COMPACT },
  { SH_INSN_CMPEQI_COMPACT, SH64_COMPACT_INSN_CMPEQI_COMPACT, SH64_COMPACT_SFMT_CMPEQI_COMPACT },
  { SH_INSN_CMPGE_COMPACT, SH64_COMPACT_INSN_CMPGE_COMPACT, SH64_COMPACT_SFMT_CMPEQ_COMPACT },
  { SH_INSN_CMPGT_COMPACT, SH64_COMPACT_INSN_CMPGT_COMPACT, SH64_COMPACT_SFMT_CMPEQ_COMPACT },
  { SH_INSN_CMPHI_COMPACT, SH64_COMPACT_INSN_CMPHI_COMPACT, SH64_COMPACT_SFMT_CMPEQ_COMPACT },
  { SH_INSN_CMPHS_COMPACT, SH64_COMPACT_INSN_CMPHS_COMPACT, SH64_COMPACT_SFMT_CMPEQ_COMPACT },
  { SH_INSN_CMPPL_COMPACT, SH64_COMPACT_INSN_CMPPL_COMPACT, SH64_COMPACT_SFMT_CMPPL_COMPACT },
  { SH_INSN_CMPPZ_COMPACT, SH64_COMPACT_INSN_CMPPZ_COMPACT, SH64_COMPACT_SFMT_CMPPL_COMPACT },
  { SH_INSN_CMPSTR_COMPACT, SH64_COMPACT_INSN_CMPSTR_COMPACT, SH64_COMPACT_SFMT_CMPEQ_COMPACT },
  { SH_INSN_DIV0S_COMPACT, SH64_COMPACT_INSN_DIV0S_COMPACT, SH64_COMPACT_SFMT_DIV0S_COMPACT },
  { SH_INSN_DIV0U_COMPACT, SH64_COMPACT_INSN_DIV0U_COMPACT, SH64_COMPACT_SFMT_DIV0U_COMPACT },
  { SH_INSN_DIV1_COMPACT, SH64_COMPACT_INSN_DIV1_COMPACT, SH64_COMPACT_SFMT_DIV1_COMPACT },
  { SH_INSN_DMULSL_COMPACT, SH64_COMPACT_INSN_DMULSL_COMPACT, SH64_COMPACT_SFMT_DMULSL_COMPACT },
  { SH_INSN_DMULUL_COMPACT, SH64_COMPACT_INSN_DMULUL_COMPACT, SH64_COMPACT_SFMT_DMULSL_COMPACT },
  { SH_INSN_DT_COMPACT, SH64_COMPACT_INSN_DT_COMPACT, SH64_COMPACT_SFMT_DT_COMPACT },
  { SH_INSN_EXTSB_COMPACT, SH64_COMPACT_INSN_EXTSB_COMPACT, SH64_COMPACT_SFMT_EXTSB_COMPACT },
  { SH_INSN_EXTSW_COMPACT, SH64_COMPACT_INSN_EXTSW_COMPACT, SH64_COMPACT_SFMT_EXTSB_COMPACT },
  { SH_INSN_EXTUB_COMPACT, SH64_COMPACT_INSN_EXTUB_COMPACT, SH64_COMPACT_SFMT_EXTSB_COMPACT },
  { SH_INSN_EXTUW_COMPACT, SH64_COMPACT_INSN_EXTUW_COMPACT, SH64_COMPACT_SFMT_EXTSB_COMPACT },
  { SH_INSN_FABS_COMPACT, SH64_COMPACT_INSN_FABS_COMPACT, SH64_COMPACT_SFMT_FABS_COMPACT },
  { SH_INSN_FADD_COMPACT, SH64_COMPACT_INSN_FADD_COMPACT, SH64_COMPACT_SFMT_FADD_COMPACT },
  { SH_INSN_FCMPEQ_COMPACT, SH64_COMPACT_INSN_FCMPEQ_COMPACT, SH64_COMPACT_SFMT_FCMPEQ_COMPACT },
  { SH_INSN_FCMPGT_COMPACT, SH64_COMPACT_INSN_FCMPGT_COMPACT, SH64_COMPACT_SFMT_FCMPEQ_COMPACT },
  { SH_INSN_FCNVDS_COMPACT, SH64_COMPACT_INSN_FCNVDS_COMPACT, SH64_COMPACT_SFMT_FCNVDS_COMPACT },
  { SH_INSN_FCNVSD_COMPACT, SH64_COMPACT_INSN_FCNVSD_COMPACT, SH64_COMPACT_SFMT_FCNVSD_COMPACT },
  { SH_INSN_FDIV_COMPACT, SH64_COMPACT_INSN_FDIV_COMPACT, SH64_COMPACT_SFMT_FADD_COMPACT },
  { SH_INSN_FIPR_COMPACT, SH64_COMPACT_INSN_FIPR_COMPACT, SH64_COMPACT_SFMT_FIPR_COMPACT },
  { SH_INSN_FLDS_COMPACT, SH64_COMPACT_INSN_FLDS_COMPACT, SH64_COMPACT_SFMT_FLDS_COMPACT },
  { SH_INSN_FLDI0_COMPACT, SH64_COMPACT_INSN_FLDI0_COMPACT, SH64_COMPACT_SFMT_FLDI0_COMPACT },
  { SH_INSN_FLDI1_COMPACT, SH64_COMPACT_INSN_FLDI1_COMPACT, SH64_COMPACT_SFMT_FLDI0_COMPACT },
  { SH_INSN_FLOAT_COMPACT, SH64_COMPACT_INSN_FLOAT_COMPACT, SH64_COMPACT_SFMT_FLOAT_COMPACT },
  { SH_INSN_FMAC_COMPACT, SH64_COMPACT_INSN_FMAC_COMPACT, SH64_COMPACT_SFMT_FMAC_COMPACT },
  { SH_INSN_FMOV1_COMPACT, SH64_COMPACT_INSN_FMOV1_COMPACT, SH64_COMPACT_SFMT_FMOV1_COMPACT },
  { SH_INSN_FMOV2_COMPACT, SH64_COMPACT_INSN_FMOV2_COMPACT, SH64_COMPACT_SFMT_FMOV2_COMPACT },
  { SH_INSN_FMOV3_COMPACT, SH64_COMPACT_INSN_FMOV3_COMPACT, SH64_COMPACT_SFMT_FMOV3_COMPACT },
  { SH_INSN_FMOV4_COMPACT, SH64_COMPACT_INSN_FMOV4_COMPACT, SH64_COMPACT_SFMT_FMOV4_COMPACT },
  { SH_INSN_FMOV5_COMPACT, SH64_COMPACT_INSN_FMOV5_COMPACT, SH64_COMPACT_SFMT_FMOV5_COMPACT },
  { SH_INSN_FMOV6_COMPACT, SH64_COMPACT_INSN_FMOV6_COMPACT, SH64_COMPACT_SFMT_FMOV6_COMPACT },
  { SH_INSN_FMOV7_COMPACT, SH64_COMPACT_INSN_FMOV7_COMPACT, SH64_COMPACT_SFMT_FMOV7_COMPACT },
  { SH_INSN_FMUL_COMPACT, SH64_COMPACT_INSN_FMUL_COMPACT, SH64_COMPACT_SFMT_FADD_COMPACT },
  { SH_INSN_FNEG_COMPACT, SH64_COMPACT_INSN_FNEG_COMPACT, SH64_COMPACT_SFMT_FABS_COMPACT },
  { SH_INSN_FRCHG_COMPACT, SH64_COMPACT_INSN_FRCHG_COMPACT, SH64_COMPACT_SFMT_FRCHG_COMPACT },
  { SH_INSN_FSCHG_COMPACT, SH64_COMPACT_INSN_FSCHG_COMPACT, SH64_COMPACT_SFMT_FSCHG_COMPACT },
  { SH_INSN_FSQRT_COMPACT, SH64_COMPACT_INSN_FSQRT_COMPACT, SH64_COMPACT_SFMT_FABS_COMPACT },
  { SH_INSN_FSTS_COMPACT, SH64_COMPACT_INSN_FSTS_COMPACT, SH64_COMPACT_SFMT_FSTS_COMPACT },
  { SH_INSN_FSUB_COMPACT, SH64_COMPACT_INSN_FSUB_COMPACT, SH64_COMPACT_SFMT_FADD_COMPACT },
  { SH_INSN_FTRC_COMPACT, SH64_COMPACT_INSN_FTRC_COMPACT, SH64_COMPACT_SFMT_FTRC_COMPACT },
  { SH_INSN_FTRV_COMPACT, SH64_COMPACT_INSN_FTRV_COMPACT, SH64_COMPACT_SFMT_FTRV_COMPACT },
  { SH_INSN_JMP_COMPACT, SH64_COMPACT_INSN_JMP_COMPACT, SH64_COMPACT_SFMT_JMP_COMPACT },
  { SH_INSN_JSR_COMPACT, SH64_COMPACT_INSN_JSR_COMPACT, SH64_COMPACT_SFMT_BSRF_COMPACT },
  { SH_INSN_LDC_COMPACT, SH64_COMPACT_INSN_LDC_COMPACT, SH64_COMPACT_SFMT_LDC_COMPACT },
  { SH_INSN_LDCL_COMPACT, SH64_COMPACT_INSN_LDCL_COMPACT, SH64_COMPACT_SFMT_LDCL_COMPACT },
  { SH_INSN_LDS_FPSCR_COMPACT, SH64_COMPACT_INSN_LDS_FPSCR_COMPACT, SH64_COMPACT_SFMT_LDS_FPSCR_COMPACT },
  { SH_INSN_LDSL_FPSCR_COMPACT, SH64_COMPACT_INSN_LDSL_FPSCR_COMPACT, SH64_COMPACT_SFMT_LDSL_FPSCR_COMPACT },
  { SH_INSN_LDS_FPUL_COMPACT, SH64_COMPACT_INSN_LDS_FPUL_COMPACT, SH64_COMPACT_SFMT_LDS_FPUL_COMPACT },
  { SH_INSN_LDSL_FPUL_COMPACT, SH64_COMPACT_INSN_LDSL_FPUL_COMPACT, SH64_COMPACT_SFMT_LDSL_FPUL_COMPACT },
  { SH_INSN_LDS_MACH_COMPACT, SH64_COMPACT_INSN_LDS_MACH_COMPACT, SH64_COMPACT_SFMT_LDS_MACH_COMPACT },
  { SH_INSN_LDSL_MACH_COMPACT, SH64_COMPACT_INSN_LDSL_MACH_COMPACT, SH64_COMPACT_SFMT_LDSL_MACH_COMPACT },
  { SH_INSN_LDS_MACL_COMPACT, SH64_COMPACT_INSN_LDS_MACL_COMPACT, SH64_COMPACT_SFMT_LDS_MACL_COMPACT },
  { SH_INSN_LDSL_MACL_COMPACT, SH64_COMPACT_INSN_LDSL_MACL_COMPACT, SH64_COMPACT_SFMT_LDSL_MACL_COMPACT },
  { SH_INSN_LDS_PR_COMPACT, SH64_COMPACT_INSN_LDS_PR_COMPACT, SH64_COMPACT_SFMT_LDS_PR_COMPACT },
  { SH_INSN_LDSL_PR_COMPACT, SH64_COMPACT_INSN_LDSL_PR_COMPACT, SH64_COMPACT_SFMT_LDSL_PR_COMPACT },
  { SH_INSN_MACL_COMPACT, SH64_COMPACT_INSN_MACL_COMPACT, SH64_COMPACT_SFMT_MACL_COMPACT },
  { SH_INSN_MACW_COMPACT, SH64_COMPACT_INSN_MACW_COMPACT, SH64_COMPACT_SFMT_MACW_COMPACT },
  { SH_INSN_MOV_COMPACT, SH64_COMPACT_INSN_MOV_COMPACT, SH64_COMPACT_SFMT_MOV_COMPACT },
  { SH_INSN_MOVI_COMPACT, SH64_COMPACT_INSN_MOVI_COMPACT, SH64_COMPACT_SFMT_MOVI_COMPACT },
  { SH_INSN_MOVB1_COMPACT, SH64_COMPACT_INSN_MOVB1_COMPACT, SH64_COMPACT_SFMT_MOVB1_COMPACT },
  { SH_INSN_MOVB2_COMPACT, SH64_COMPACT_INSN_MOVB2_COMPACT, SH64_COMPACT_SFMT_MOVB2_COMPACT },
  { SH_INSN_MOVB3_COMPACT, SH64_COMPACT_INSN_MOVB3_COMPACT, SH64_COMPACT_SFMT_MOVB3_COMPACT },
  { SH_INSN_MOVB4_COMPACT, SH64_COMPACT_INSN_MOVB4_COMPACT, SH64_COMPACT_SFMT_MOVB4_COMPACT },
  { SH_INSN_MOVB5_COMPACT, SH64_COMPACT_INSN_MOVB5_COMPACT, SH64_COMPACT_SFMT_MOVB5_COMPACT },
  { SH_INSN_MOVB6_COMPACT, SH64_COMPACT_INSN_MOVB6_COMPACT, SH64_COMPACT_SFMT_MOVB6_COMPACT },
  { SH_INSN_MOVB7_COMPACT, SH64_COMPACT_INSN_MOVB7_COMPACT, SH64_COMPACT_SFMT_MOVB7_COMPACT },
  { SH_INSN_MOVB8_COMPACT, SH64_COMPACT_INSN_MOVB8_COMPACT, SH64_COMPACT_SFMT_MOVB8_COMPACT },
  { SH_INSN_MOVB9_COMPACT, SH64_COMPACT_INSN_MOVB9_COMPACT, SH64_COMPACT_SFMT_MOVB9_COMPACT },
  { SH_INSN_MOVB10_COMPACT, SH64_COMPACT_INSN_MOVB10_COMPACT, SH64_COMPACT_SFMT_MOVB10_COMPACT },
  { SH_INSN_MOVL1_COMPACT, SH64_COMPACT_INSN_MOVL1_COMPACT, SH64_COMPACT_SFMT_MOVB1_COMPACT },
  { SH_INSN_MOVL2_COMPACT, SH64_COMPACT_INSN_MOVL2_COMPACT, SH64_COMPACT_SFMT_MOVB2_COMPACT },
  { SH_INSN_MOVL3_COMPACT, SH64_COMPACT_INSN_MOVL3_COMPACT, SH64_COMPACT_SFMT_MOVB3_COMPACT },
  { SH_INSN_MOVL4_COMPACT, SH64_COMPACT_INSN_MOVL4_COMPACT, SH64_COMPACT_SFMT_MOVL4_COMPACT },
  { SH_INSN_MOVL5_COMPACT, SH64_COMPACT_INSN_MOVL5_COMPACT, SH64_COMPACT_SFMT_MOVL5_COMPACT },
  { SH_INSN_MOVL6_COMPACT, SH64_COMPACT_INSN_MOVL6_COMPACT, SH64_COMPACT_SFMT_MOVB6_COMPACT },
  { SH_INSN_MOVL7_COMPACT, SH64_COMPACT_INSN_MOVL7_COMPACT, SH64_COMPACT_SFMT_MOVL7_COMPACT },
  { SH_INSN_MOVL8_COMPACT, SH64_COMPACT_INSN_MOVL8_COMPACT, SH64_COMPACT_SFMT_MOVB8_COMPACT },
  { SH_INSN_MOVL9_COMPACT, SH64_COMPACT_INSN_MOVL9_COMPACT, SH64_COMPACT_SFMT_MOVL9_COMPACT },
  { SH_INSN_MOVL10_COMPACT, SH64_COMPACT_INSN_MOVL10_COMPACT, SH64_COMPACT_SFMT_MOVL10_COMPACT },
  { SH_INSN_MOVL11_COMPACT, SH64_COMPACT_INSN_MOVL11_COMPACT, SH64_COMPACT_SFMT_MOVL11_COMPACT },
  { SH_INSN_MOVW1_COMPACT, SH64_COMPACT_INSN_MOVW1_COMPACT, SH64_COMPACT_SFMT_MOVB1_COMPACT },
  { SH_INSN_MOVW2_COMPACT, SH64_COMPACT_INSN_MOVW2_COMPACT, SH64_COMPACT_SFMT_MOVB2_COMPACT },
  { SH_INSN_MOVW3_COMPACT, SH64_COMPACT_INSN_MOVW3_COMPACT, SH64_COMPACT_SFMT_MOVB3_COMPACT },
  { SH_INSN_MOVW4_COMPACT, SH64_COMPACT_INSN_MOVW4_COMPACT, SH64_COMPACT_SFMT_MOVW4_COMPACT },
  { SH_INSN_MOVW5_COMPACT, SH64_COMPACT_INSN_MOVW5_COMPACT, SH64_COMPACT_SFMT_MOVW5_COMPACT },
  { SH_INSN_MOVW6_COMPACT, SH64_COMPACT_INSN_MOVW6_COMPACT, SH64_COMPACT_SFMT_MOVB6_COMPACT },
  { SH_INSN_MOVW7_COMPACT, SH64_COMPACT_INSN_MOVW7_COMPACT, SH64_COMPACT_SFMT_MOVB7_COMPACT },
  { SH_INSN_MOVW8_COMPACT, SH64_COMPACT_INSN_MOVW8_COMPACT, SH64_COMPACT_SFMT_MOVB8_COMPACT },
  { SH_INSN_MOVW9_COMPACT, SH64_COMPACT_INSN_MOVW9_COMPACT, SH64_COMPACT_SFMT_MOVW9_COMPACT },
  { SH_INSN_MOVW10_COMPACT, SH64_COMPACT_INSN_MOVW10_COMPACT, SH64_COMPACT_SFMT_MOVW10_COMPACT },
  { SH_INSN_MOVW11_COMPACT, SH64_COMPACT_INSN_MOVW11_COMPACT, SH64_COMPACT_SFMT_MOVW11_COMPACT },
  { SH_INSN_MOVA_COMPACT, SH64_COMPACT_INSN_MOVA_COMPACT, SH64_COMPACT_SFMT_MOVA_COMPACT },
  { SH_INSN_MOVCAL_COMPACT, SH64_COMPACT_INSN_MOVCAL_COMPACT, SH64_COMPACT_SFMT_MOVCAL_COMPACT },
  { SH_INSN_MOVT_COMPACT, SH64_COMPACT_INSN_MOVT_COMPACT, SH64_COMPACT_SFMT_MOVT_COMPACT },
  { SH_INSN_MULL_COMPACT, SH64_COMPACT_INSN_MULL_COMPACT, SH64_COMPACT_SFMT_MULL_COMPACT },
  { SH_INSN_MULSW_COMPACT, SH64_COMPACT_INSN_MULSW_COMPACT, SH64_COMPACT_SFMT_MULL_COMPACT },
  { SH_INSN_MULUW_COMPACT, SH64_COMPACT_INSN_MULUW_COMPACT, SH64_COMPACT_SFMT_MULL_COMPACT },
  { SH_INSN_NEG_COMPACT, SH64_COMPACT_INSN_NEG_COMPACT, SH64_COMPACT_SFMT_EXTSB_COMPACT },
  { SH_INSN_NEGC_COMPACT, SH64_COMPACT_INSN_NEGC_COMPACT, SH64_COMPACT_SFMT_NEGC_COMPACT },
  { SH_INSN_NOP_COMPACT, SH64_COMPACT_INSN_NOP_COMPACT, SH64_COMPACT_SFMT_NOP_COMPACT },
  { SH_INSN_NOT_COMPACT, SH64_COMPACT_INSN_NOT_COMPACT, SH64_COMPACT_SFMT_MOV_COMPACT },
  { SH_INSN_OCBI_COMPACT, SH64_COMPACT_INSN_OCBI_COMPACT, SH64_COMPACT_SFMT_NOP_COMPACT },
  { SH_INSN_OCBP_COMPACT, SH64_COMPACT_INSN_OCBP_COMPACT, SH64_COMPACT_SFMT_NOP_COMPACT },
  { SH_INSN_OCBWB_COMPACT, SH64_COMPACT_INSN_OCBWB_COMPACT, SH64_COMPACT_SFMT_NOP_COMPACT },
  { SH_INSN_OR_COMPACT, SH64_COMPACT_INSN_OR_COMPACT, SH64_COMPACT_SFMT_AND_COMPACT },
  { SH_INSN_ORI_COMPACT, SH64_COMPACT_INSN_ORI_COMPACT, SH64_COMPACT_SFMT_ANDI_COMPACT },
  { SH_INSN_ORB_COMPACT, SH64_COMPACT_INSN_ORB_COMPACT, SH64_COMPACT_SFMT_ANDB_COMPACT },
  { SH_INSN_PREF_COMPACT, SH64_COMPACT_INSN_PREF_COMPACT, SH64_COMPACT_SFMT_NOP_COMPACT },
  { SH_INSN_ROTCL_COMPACT, SH64_COMPACT_INSN_ROTCL_COMPACT, SH64_COMPACT_SFMT_ROTCL_COMPACT },
  { SH_INSN_ROTCR_COMPACT, SH64_COMPACT_INSN_ROTCR_COMPACT, SH64_COMPACT_SFMT_ROTCL_COMPACT },
  { SH_INSN_ROTL_COMPACT, SH64_COMPACT_INSN_ROTL_COMPACT, SH64_COMPACT_SFMT_DT_COMPACT },
  { SH_INSN_ROTR_COMPACT, SH64_COMPACT_INSN_ROTR_COMPACT, SH64_COMPACT_SFMT_DT_COMPACT },
  { SH_INSN_RTS_COMPACT, SH64_COMPACT_INSN_RTS_COMPACT, SH64_COMPACT_SFMT_RTS_COMPACT },
  { SH_INSN_SETS_COMPACT, SH64_COMPACT_INSN_SETS_COMPACT, SH64_COMPACT_SFMT_CLRS_COMPACT },
  { SH_INSN_SETT_COMPACT, SH64_COMPACT_INSN_SETT_COMPACT, SH64_COMPACT_SFMT_CLRT_COMPACT },
  { SH_INSN_SHAD_COMPACT, SH64_COMPACT_INSN_SHAD_COMPACT, SH64_COMPACT_SFMT_SHAD_COMPACT },
  { SH_INSN_SHAL_COMPACT, SH64_COMPACT_INSN_SHAL_COMPACT, SH64_COMPACT_SFMT_DT_COMPACT },
  { SH_INSN_SHAR_COMPACT, SH64_COMPACT_INSN_SHAR_COMPACT, SH64_COMPACT_SFMT_DT_COMPACT },
  { SH_INSN_SHLD_COMPACT, SH64_COMPACT_INSN_SHLD_COMPACT, SH64_COMPACT_SFMT_SHAD_COMPACT },
  { SH_INSN_SHLL_COMPACT, SH64_COMPACT_INSN_SHLL_COMPACT, SH64_COMPACT_SFMT_DT_COMPACT },
  { SH_INSN_SHLL2_COMPACT, SH64_COMPACT_INSN_SHLL2_COMPACT, SH64_COMPACT_SFMT_SHLL2_COMPACT },
  { SH_INSN_SHLL8_COMPACT, SH64_COMPACT_INSN_SHLL8_COMPACT, SH64_COMPACT_SFMT_SHLL2_COMPACT },
  { SH_INSN_SHLL16_COMPACT, SH64_COMPACT_INSN_SHLL16_COMPACT, SH64_COMPACT_SFMT_SHLL2_COMPACT },
  { SH_INSN_SHLR_COMPACT, SH64_COMPACT_INSN_SHLR_COMPACT, SH64_COMPACT_SFMT_DT_COMPACT },
  { SH_INSN_SHLR2_COMPACT, SH64_COMPACT_INSN_SHLR2_COMPACT, SH64_COMPACT_SFMT_SHLL2_COMPACT },
  { SH_INSN_SHLR8_COMPACT, SH64_COMPACT_INSN_SHLR8_COMPACT, SH64_COMPACT_SFMT_SHLL2_COMPACT },
  { SH_INSN_SHLR16_COMPACT, SH64_COMPACT_INSN_SHLR16_COMPACT, SH64_COMPACT_SFMT_SHLL2_COMPACT },
  { SH_INSN_STC_GBR_COMPACT, SH64_COMPACT_INSN_STC_GBR_COMPACT, SH64_COMPACT_SFMT_STC_GBR_COMPACT },
  { SH_INSN_STCL_GBR_COMPACT, SH64_COMPACT_INSN_STCL_GBR_COMPACT, SH64_COMPACT_SFMT_STCL_GBR_COMPACT },
  { SH_INSN_STS_FPSCR_COMPACT, SH64_COMPACT_INSN_STS_FPSCR_COMPACT, SH64_COMPACT_SFMT_STS_FPSCR_COMPACT },
  { SH_INSN_STSL_FPSCR_COMPACT, SH64_COMPACT_INSN_STSL_FPSCR_COMPACT, SH64_COMPACT_SFMT_STSL_FPSCR_COMPACT },
  { SH_INSN_STS_FPUL_COMPACT, SH64_COMPACT_INSN_STS_FPUL_COMPACT, SH64_COMPACT_SFMT_STS_FPUL_COMPACT },
  { SH_INSN_STSL_FPUL_COMPACT, SH64_COMPACT_INSN_STSL_FPUL_COMPACT, SH64_COMPACT_SFMT_STSL_FPUL_COMPACT },
  { SH_INSN_STS_MACH_COMPACT, SH64_COMPACT_INSN_STS_MACH_COMPACT, SH64_COMPACT_SFMT_STS_MACH_COMPACT },
  { SH_INSN_STSL_MACH_COMPACT, SH64_COMPACT_INSN_STSL_MACH_COMPACT, SH64_COMPACT_SFMT_STSL_MACH_COMPACT },
  { SH_INSN_STS_MACL_COMPACT, SH64_COMPACT_INSN_STS_MACL_COMPACT, SH64_COMPACT_SFMT_STS_MACL_COMPACT },
  { SH_INSN_STSL_MACL_COMPACT, SH64_COMPACT_INSN_STSL_MACL_COMPACT, SH64_COMPACT_SFMT_STSL_MACL_COMPACT },
  { SH_INSN_STS_PR_COMPACT, SH64_COMPACT_INSN_STS_PR_COMPACT, SH64_COMPACT_SFMT_STS_PR_COMPACT },
  { SH_INSN_STSL_PR_COMPACT, SH64_COMPACT_INSN_STSL_PR_COMPACT, SH64_COMPACT_SFMT_STSL_PR_COMPACT },
  { SH_INSN_SUB_COMPACT, SH64_COMPACT_INSN_SUB_COMPACT, SH64_COMPACT_SFMT_ADD_COMPACT },
  { SH_INSN_SUBC_COMPACT, SH64_COMPACT_INSN_SUBC_COMPACT, SH64_COMPACT_SFMT_ADDC_COMPACT },
  { SH_INSN_SUBV_COMPACT, SH64_COMPACT_INSN_SUBV_COMPACT, SH64_COMPACT_SFMT_ADDV_COMPACT },
  { SH_INSN_SWAPB_COMPACT, SH64_COMPACT_INSN_SWAPB_COMPACT, SH64_COMPACT_SFMT_EXTSB_COMPACT },
  { SH_INSN_SWAPW_COMPACT, SH64_COMPACT_INSN_SWAPW_COMPACT, SH64_COMPACT_SFMT_EXTSB_COMPACT },
  { SH_INSN_TASB_COMPACT, SH64_COMPACT_INSN_TASB_COMPACT, SH64_COMPACT_SFMT_TASB_COMPACT },
  { SH_INSN_TRAPA_COMPACT, SH64_COMPACT_INSN_TRAPA_COMPACT, SH64_COMPACT_SFMT_TRAPA_COMPACT },
  { SH_INSN_TST_COMPACT, SH64_COMPACT_INSN_TST_COMPACT, SH64_COMPACT_SFMT_CMPEQ_COMPACT },
  { SH_INSN_TSTI_COMPACT, SH64_COMPACT_INSN_TSTI_COMPACT, SH64_COMPACT_SFMT_TSTI_COMPACT },
  { SH_INSN_TSTB_COMPACT, SH64_COMPACT_INSN_TSTB_COMPACT, SH64_COMPACT_SFMT_TSTB_COMPACT },
  { SH_INSN_XOR_COMPACT, SH64_COMPACT_INSN_XOR_COMPACT, SH64_COMPACT_SFMT_AND_COMPACT },
  { SH_INSN_XORI_COMPACT, SH64_COMPACT_INSN_XORI_COMPACT, SH64_COMPACT_SFMT_XORI_COMPACT },
  { SH_INSN_XORB_COMPACT, SH64_COMPACT_INSN_XORB_COMPACT, SH64_COMPACT_SFMT_ANDB_COMPACT },
  { SH_INSN_XTRCT_COMPACT, SH64_COMPACT_INSN_XTRCT_COMPACT, SH64_COMPACT_SFMT_ADD_COMPACT },
};

static const struct insn_sem sh64_compact_insn_sem_invalid = {
  VIRTUAL_INSN_X_INVALID, SH64_COMPACT_INSN_X_INVALID, SH64_COMPACT_SFMT_EMPTY
};

/* Initialize an IDESC from the compile-time computable parts.  */

static INLINE void
init_idesc (SIM_CPU *cpu, IDESC *id, const struct insn_sem *t)
{
  const CGEN_INSN *insn_table = CGEN_CPU_INSN_TABLE (CPU_CPU_DESC (cpu))->init_entries;

  id->num = t->index;
  id->sfmt = t->sfmt;
  if ((int) t->type <= 0)
    id->idata = & cgen_virtual_insn_table[- (int) t->type];
  else
    id->idata = & insn_table[t->type];
  id->attrs = CGEN_INSN_ATTRS (id->idata);
  /* Oh my god, a magic number.  */
  id->length = CGEN_INSN_BITSIZE (id->idata) / 8;

#if WITH_PROFILE_MODEL_P
  id->timing = & MODEL_TIMING (CPU_MODEL (cpu)) [t->index];
  {
    SIM_DESC sd = CPU_STATE (cpu);
    SIM_ASSERT (t->index == id->timing->num);
  }
#endif

  /* Semantic pointers are initialized elsewhere.  */
}

/* Initialize the instruction descriptor table.  */

void
sh64_compact_init_idesc_table (SIM_CPU *cpu)
{
  IDESC *id,*tabend;
  const struct insn_sem *t,*tend;
  int tabsize = SH64_COMPACT_INSN_MAX;
  IDESC *table = sh64_compact_insn_data;

  memset (table, 0, tabsize * sizeof (IDESC));

  /* First set all entries to the `invalid insn'.  */
  t = & sh64_compact_insn_sem_invalid;
  for (id = table, tabend = table + tabsize; id < tabend; ++id)
    init_idesc (cpu, id, t);

  /* Now fill in the values for the chosen cpu.  */
  for (t = sh64_compact_insn_sem, tend = t + sizeof (sh64_compact_insn_sem) / sizeof (*t);
       t != tend; ++t)
    {
      init_idesc (cpu, & table[t->index], t);
    }

  /* Link the IDESC table into the cpu.  */
  CPU_IDESC (cpu) = table;
}

/* Given an instruction, return a pointer to its IDESC entry.  */

const IDESC *
sh64_compact_decode (SIM_CPU *current_cpu, IADDR pc,
              CGEN_INSN_INT base_insn, CGEN_INSN_INT entire_insn,
              ARGBUF *abuf)
{
  /* Result of decoder.  */
  SH64_COMPACT_INSN_TYPE itype;

  {
    CGEN_INSN_INT insn = base_insn;

    {
      unsigned int val = (((insn >> 8) & (15 << 4)) | ((insn >> 0) & (15 << 0)));
      switch (val)
      {
      case 2 : itype = SH64_COMPACT_INSN_STC_GBR_COMPACT; goto extract_sfmt_stc_gbr_compact;      case 3 :
        {
          unsigned int val = (((insn >> 4) & (15 << 0)));
          switch (val)
          {
          case 0 : itype = SH64_COMPACT_INSN_BSRF_COMPACT; goto extract_sfmt_bsrf_compact;          case 2 : itype = SH64_COMPACT_INSN_BRAF_COMPACT; goto extract_sfmt_braf_compact;          case 8 : itype = SH64_COMPACT_INSN_PREF_COMPACT; goto extract_sfmt_nop_compact;          case 9 : itype = SH64_COMPACT_INSN_OCBI_COMPACT; goto extract_sfmt_nop_compact;          case 10 : itype = SH64_COMPACT_INSN_OCBP_COMPACT; goto extract_sfmt_nop_compact;          case 11 : itype = SH64_COMPACT_INSN_OCBWB_COMPACT; goto extract_sfmt_nop_compact;          case 12 : itype = SH64_COMPACT_INSN_MOVCAL_COMPACT; goto extract_sfmt_movcal_compact;          default : itype = SH64_COMPACT_INSN_X_INVALID; goto extract_sfmt_empty;
          }
        }
      case 4 : itype = SH64_COMPACT_INSN_MOVB3_COMPACT; goto extract_sfmt_movb3_compact;      case 5 : itype = SH64_COMPACT_INSN_MOVW3_COMPACT; goto extract_sfmt_movb3_compact;      case 6 : itype = SH64_COMPACT_INSN_MOVL3_COMPACT; goto extract_sfmt_movb3_compact;      case 7 : itype = SH64_COMPACT_INSN_MULL_COMPACT; goto extract_sfmt_mull_compact;      case 8 :
        {
          unsigned int val = (((insn >> 4) & (7 << 0)));
          switch (val)
          {
          case 0 : itype = SH64_COMPACT_INSN_CLRT_COMPACT; goto extract_sfmt_clrt_compact;          case 1 : itype = SH64_COMPACT_INSN_SETT_COMPACT; goto extract_sfmt_clrt_compact;          case 2 : itype = SH64_COMPACT_INSN_CLRMAC_COMPACT; goto extract_sfmt_clrmac_compact;          case 4 : itype = SH64_COMPACT_INSN_CLRS_COMPACT; goto extract_sfmt_clrs_compact;          case 5 : itype = SH64_COMPACT_INSN_SETS_COMPACT; goto extract_sfmt_clrs_compact;          default : itype = SH64_COMPACT_INSN_X_INVALID; goto extract_sfmt_empty;
          }
        }
      case 9 :
        {
          unsigned int val = (((insn >> 4) & (3 << 0)));
          switch (val)
          {
          case 0 : itype = SH64_COMPACT_INSN_NOP_COMPACT; goto extract_sfmt_nop_compact;          case 1 : itype = SH64_COMPACT_INSN_DIV0U_COMPACT; goto extract_sfmt_div0u_compact;          case 2 : itype = SH64_COMPACT_INSN_MOVT_COMPACT; goto extract_sfmt_movt_compact;          default : itype = SH64_COMPACT_INSN_X_INVALID; goto extract_sfmt_empty;
          }
        }
      case 10 :
        {
          unsigned int val = (((insn >> 4) & (7 << 0)));
          switch (val)
          {
          case 0 : itype = SH64_COMPACT_INSN_STS_MACH_COMPACT; goto extract_sfmt_sts_mach_compact;          case 1 : itype = SH64_COMPACT_INSN_STS_MACL_COMPACT; goto extract_sfmt_sts_macl_compact;          case 2 : itype = SH64_COMPACT_INSN_STS_PR_COMPACT; goto extract_sfmt_sts_pr_compact;          case 5 : itype = SH64_COMPACT_INSN_STS_FPUL_COMPACT; goto extract_sfmt_sts_fpul_compact;          case 6 : itype = SH64_COMPACT_INSN_STS_FPSCR_COMPACT; goto extract_sfmt_sts_fpscr_compact;          default : itype = SH64_COMPACT_INSN_X_INVALID; goto extract_sfmt_empty;
          }
        }
      case 11 :
        {
          unsigned int val = (((insn >> 4) & (3 << 0)));
          switch (val)
          {
          case 0 : itype = SH64_COMPACT_INSN_RTS_COMPACT; goto extract_sfmt_rts_compact;          case 3 : itype = SH64_COMPACT_INSN_BRK_COMPACT; goto extract_sfmt_brk_compact;          default : itype = SH64_COMPACT_INSN_X_INVALID; goto extract_sfmt_empty;
          }
        }
      case 12 : itype = SH64_COMPACT_INSN_MOVB8_COMPACT; goto extract_sfmt_movb8_compact;      case 13 : itype = SH64_COMPACT_INSN_MOVW8_COMPACT; goto extract_sfmt_movb8_compact;      case 14 : itype = SH64_COMPACT_INSN_MOVL8_COMPACT; goto extract_sfmt_movb8_compact;      case 15 : itype = SH64_COMPACT_INSN_MACL_COMPACT; goto extract_sfmt_macl_compact;      case 16 : /* fall through */
      case 17 : /* fall through */
      case 18 : /* fall through */
      case 19 : /* fall through */
      case 20 : /* fall through */
      case 21 : /* fall through */
      case 22 : /* fall through */
      case 23 : /* fall through */
      case 24 : /* fall through */
      case 25 : /* fall through */
      case 26 : /* fall through */
      case 27 : /* fall through */
      case 28 : /* fall through */
      case 29 : /* fall through */
      case 30 : /* fall through */
      case 31 : itype = SH64_COMPACT_INSN_MOVL5_COMPACT; goto extract_sfmt_movl5_compact;      case 32 : itype = SH64_COMPACT_INSN_MOVB1_COMPACT; goto extract_sfmt_movb1_compact;      case 33 : itype = SH64_COMPACT_INSN_MOVW1_COMPACT; goto extract_sfmt_movb1_compact;      case 34 : itype = SH64_COMPACT_INSN_MOVL1_COMPACT; goto extract_sfmt_movb1_compact;      case 36 : itype = SH64_COMPACT_INSN_MOVB2_COMPACT; goto extract_sfmt_movb2_compact;      case 37 : itype = SH64_COMPACT_INSN_MOVW2_COMPACT; goto extract_sfmt_movb2_compact;      case 38 : itype = SH64_COMPACT_INSN_MOVL2_COMPACT; goto extract_sfmt_movb2_compact;      case 39 : itype = SH64_COMPACT_INSN_DIV0S_COMPACT; goto extract_sfmt_div0s_compact;      case 40 : itype = SH64_COMPACT_INSN_TST_COMPACT; goto extract_sfmt_cmpeq_compact;      case 41 : itype = SH64_COMPACT_INSN_AND_COMPACT; goto extract_sfmt_and_compact;      case 42 : itype = SH64_COMPACT_INSN_XOR_COMPACT; goto extract_sfmt_and_compact;      case 43 : itype = SH64_COMPACT_INSN_OR_COMPACT; goto extract_sfmt_and_compact;      case 44 : itype = SH64_COMPACT_INSN_CMPSTR_COMPACT; goto extract_sfmt_cmpeq_compact;      case 45 : itype = SH64_COMPACT_INSN_XTRCT_COMPACT; goto extract_sfmt_add_compact;      case 46 : itype = SH64_COMPACT_INSN_MULUW_COMPACT; goto extract_sfmt_mull_compact;      case 47 : itype = SH64_COMPACT_INSN_MULSW_COMPACT; goto extract_sfmt_mull_compact;      case 48 : itype = SH64_COMPACT_INSN_CMPEQ_COMPACT; goto extract_sfmt_cmpeq_compact;      case 50 : itype = SH64_COMPACT_INSN_CMPHS_COMPACT; goto extract_sfmt_cmpeq_compact;      case 51 : itype = SH64_COMPACT_INSN_CMPGE_COMPACT; goto extract_sfmt_cmpeq_compact;      case 52 : itype = SH64_COMPACT_INSN_DIV1_COMPACT; goto extract_sfmt_div1_compact;      case 53 : itype = SH64_COMPACT_INSN_DMULUL_COMPACT; goto extract_sfmt_dmulsl_compact;      case 54 : itype = SH64_COMPACT_INSN_CMPHI_COMPACT; goto extract_sfmt_cmpeq_compact;      case 55 : itype = SH64_COMPACT_INSN_CMPGT_COMPACT; goto extract_sfmt_cmpeq_compact;      case 56 : itype = SH64_COMPACT_INSN_SUB_COMPACT; goto extract_sfmt_add_compact;      case 58 : itype = SH64_COMPACT_INSN_SUBC_COMPACT; goto extract_sfmt_addc_compact;      case 59 : itype = SH64_COMPACT_INSN_SUBV_COMPACT; goto extract_sfmt_addv_compact;      case 60 : itype = SH64_COMPACT_INSN_ADD_COMPACT; goto extract_sfmt_add_compact;      case 61 : itype = SH64_COMPACT_INSN_DMULSL_COMPACT; goto extract_sfmt_dmulsl_compact;      case 62 : itype = SH64_COMPACT_INSN_ADDC_COMPACT; goto extract_sfmt_addc_compact;      case 63 : itype = SH64_COMPACT_INSN_ADDV_COMPACT; goto extract_sfmt_addv_compact;      case 64 :
        {
          unsigned int val = (((insn >> 4) & (3 << 0)));
          switch (val)
          {
          case 0 : itype = SH64_COMPACT_INSN_SHLL_COMPACT; goto extract_sfmt_dt_compact;          case 1 : itype = SH64_COMPACT_INSN_DT_COMPACT; goto extract_sfmt_dt_compact;          case 2 : itype = SH64_COMPACT_INSN_SHAL_COMPACT; goto extract_sfmt_dt_compact;          default : itype = SH64_COMPACT_INSN_X_INVALID; goto extract_sfmt_empty;
          }
        }
      case 65 :
        {
          unsigned int val = (((insn >> 4) & (3 << 0)));
          switch (val)
          {
          case 0 : itype = SH64_COMPACT_INSN_SHLR_COMPACT; goto extract_sfmt_dt_compact;          case 1 : itype = SH64_COMPACT_INSN_CMPPZ_COMPACT; goto extract_sfmt_cmppl_compact;          case 2 : itype = SH64_COMPACT_INSN_SHAR_COMPACT; goto extract_sfmt_dt_compact;          default : itype = SH64_COMPACT_INSN_X_INVALID; goto extract_sfmt_empty;
          }
        }
      case 66 :
        {
          unsigned int val = (((insn >> 4) & (7 << 0)));
          switch (val)
          {
          case 0 : itype = SH64_COMPACT_INSN_STSL_MACH_COMPACT; goto extract_sfmt_stsl_mach_compact;          case 1 : itype = SH64_COMPACT_INSN_STSL_MACL_COMPACT; goto extract_sfmt_stsl_macl_compact;          case 2 : itype = SH64_COMPACT_INSN_STSL_PR_COMPACT; goto extract_sfmt_stsl_pr_compact;          case 5 : itype = SH64_COMPACT_INSN_STSL_FPUL_COMPACT; goto extract_sfmt_stsl_fpul_compact;          case 6 : itype = SH64_COMPACT_INSN_STSL_FPSCR_COMPACT; goto extract_sfmt_stsl_fpscr_compact;          default : itype = SH64_COMPACT_INSN_X_INVALID; goto extract_sfmt_empty;
          }
        }
      case 67 : itype = SH64_COMPACT_INSN_STCL_GBR_COMPACT; goto extract_sfmt_stcl_gbr_compact;      case 68 :
        {
          unsigned int val = (((insn >> 5) & (1 << 0)));
          switch (val)
          {
          case 0 : itype = SH64_COMPACT_INSN_ROTL_COMPACT; goto extract_sfmt_dt_compact;          case 1 : itype = SH64_COMPACT_INSN_ROTCL_COMPACT; goto extract_sfmt_rotcl_compact;          default : itype = SH64_COMPACT_INSN_X_INVALID; goto extract_sfmt_empty;
          }
        }
      case 69 :
        {
          unsigned int val = (((insn >> 4) & (3 << 0)));
          switch (val)
          {
          case 0 : itype = SH64_COMPACT_INSN_ROTR_COMPACT; goto extract_sfmt_dt_compact;          case 1 : itype = SH64_COMPACT_INSN_CMPPL_COMPACT; goto extract_sfmt_cmppl_compact;          case 2 : itype = SH64_COMPACT_INSN_ROTCR_COMPACT; goto extract_sfmt_rotcl_compact;          default : itype = SH64_COMPACT_INSN_X_INVALID; goto extract_sfmt_empty;
          }
        }
      case 70 :
        {
          unsigned int val = (((insn >> 4) & (7 << 0)));
          switch (val)
          {
          case 0 : itype = SH64_COMPACT_INSN_LDSL_MACH_COMPACT; goto extract_sfmt_ldsl_mach_compact;          case 1 : itype = SH64_COMPACT_INSN_LDSL_MACL_COMPACT; goto extract_sfmt_ldsl_macl_compact;          case 2 : itype = SH64_COMPACT_INSN_LDSL_PR_COMPACT; goto extract_sfmt_ldsl_pr_compact;          case 5 : itype = SH64_COMPACT_INSN_LDSL_FPUL_COMPACT; goto extract_sfmt_ldsl_fpul_compact;          case 6 : itype = SH64_COMPACT_INSN_LDSL_FPSCR_COMPACT; goto extract_sfmt_ldsl_fpscr_compact;          default : itype = SH64_COMPACT_INSN_X_INVALID; goto extract_sfmt_empty;
          }
        }
      case 71 : itype = SH64_COMPACT_INSN_LDCL_COMPACT; goto extract_sfmt_ldcl_compact;      case 72 :
        {
          unsigned int val = (((insn >> 4) & (3 << 0)));
          switch (val)
          {
          case 0 : itype = SH64_COMPACT_INSN_SHLL2_COMPACT; goto extract_sfmt_shll2_compact;          case 1 : itype = SH64_COMPACT_INSN_SHLL8_COMPACT; goto extract_sfmt_shll2_compact;          case 2 : itype = SH64_COMPACT_INSN_SHLL16_COMPACT; goto extract_sfmt_shll2_compact;          default : itype = SH64_COMPACT_INSN_X_INVALID; goto extract_sfmt_empty;
          }
        }
      case 73 :
        {
          unsigned int val = (((insn >> 4) & (3 << 0)));
          switch (val)
          {
          case 0 : itype = SH64_COMPACT_INSN_SHLR2_COMPACT; goto extract_sfmt_shll2_compact;          case 1 : itype = SH64_COMPACT_INSN_SHLR8_COMPACT; goto extract_sfmt_shll2_compact;          case 2 : itype = SH64_COMPACT_INSN_SHLR16_COMPACT; goto extract_sfmt_shll2_compact;          default : itype = SH64_COMPACT_INSN_X_INVALID; goto extract_sfmt_empty;
          }
        }
      case 74 :
        {
          unsigned int val = (((insn >> 4) & (7 << 0)));
          switch (val)
          {
          case 0 : itype = SH64_COMPACT_INSN_LDS_MACH_COMPACT; goto extract_sfmt_lds_mach_compact;          case 1 : itype = SH64_COMPACT_INSN_LDS_MACL_COMPACT; goto extract_sfmt_lds_macl_compact;          case 2 : itype = SH64_COMPACT_INSN_LDS_PR_COMPACT; goto extract_sfmt_lds_pr_compact;          case 5 : itype = SH64_COMPACT_INSN_LDS_FPUL_COMPACT; goto extract_sfmt_lds_fpul_compact;          case 6 : itype = SH64_COMPACT_INSN_LDS_FPSCR_COMPACT; goto extract_sfmt_lds_fpscr_compact;          default : itype = SH64_COMPACT_INSN_X_INVALID; goto extract_sfmt_empty;
          }
        }
      case 75 :
        {
          unsigned int val = (((insn >> 4) & (3 << 0)));
          switch (val)
          {
          case 0 : itype = SH64_COMPACT_INSN_JSR_COMPACT; goto extract_sfmt_bsrf_compact;          case 1 : itype = SH64_COMPACT_INSN_TASB_COMPACT; goto extract_sfmt_tasb_compact;          case 2 : itype = SH64_COMPACT_INSN_JMP_COMPACT; goto extract_sfmt_jmp_compact;          default : itype = SH64_COMPACT_INSN_X_INVALID; goto extract_sfmt_empty;
          }
        }
      case 76 : itype = SH64_COMPACT_INSN_SHAD_COMPACT; goto extract_sfmt_shad_compact;      case 77 : itype = SH64_COMPACT_INSN_SHLD_COMPACT; goto extract_sfmt_shad_compact;      case 78 : itype = SH64_COMPACT_INSN_LDC_COMPACT; goto extract_sfmt_ldc_compact;      case 79 : itype = SH64_COMPACT_INSN_MACW_COMPACT; goto extract_sfmt_macw_compact;      case 80 : /* fall through */
      case 81 : /* fall through */
      case 82 : /* fall through */
      case 83 : /* fall through */
      case 84 : /* fall through */
      case 85 : /* fall through */
      case 86 : /* fall through */
      case 87 : /* fall through */
      case 88 : /* fall through */
      case 89 : /* fall through */
      case 90 : /* fall through */
      case 91 : /* fall through */
      case 92 : /* fall through */
      case 93 : /* fall through */
      case 94 : /* fall through */
      case 95 : itype = SH64_COMPACT_INSN_MOVL11_COMPACT; goto extract_sfmt_movl11_compact;      case 96 : itype = SH64_COMPACT_INSN_MOVB6_COMPACT; goto extract_sfmt_movb6_compact;      case 97 : itype = SH64_COMPACT_INSN_MOVW6_COMPACT; goto extract_sfmt_movb6_compact;      case 98 : itype = SH64_COMPACT_INSN_MOVL6_COMPACT; goto extract_sfmt_movb6_compact;      case 99 : itype = SH64_COMPACT_INSN_MOV_COMPACT; goto extract_sfmt_mov_compact;      case 100 : itype = SH64_COMPACT_INSN_MOVB7_COMPACT; goto extract_sfmt_movb7_compact;      case 101 : itype = SH64_COMPACT_INSN_MOVW7_COMPACT; goto extract_sfmt_movb7_compact;      case 102 : itype = SH64_COMPACT_INSN_MOVL7_COMPACT; goto extract_sfmt_movl7_compact;      case 103 : itype = SH64_COMPACT_INSN_NOT_COMPACT; goto extract_sfmt_mov_compact;      case 104 : itype = SH64_COMPACT_INSN_SWAPB_COMPACT; goto extract_sfmt_extsb_compact;      case 105 : itype = SH64_COMPACT_INSN_SWAPW_COMPACT; goto extract_sfmt_extsb_compact;      case 106 : itype = SH64_COMPACT_INSN_NEGC_COMPACT; goto extract_sfmt_negc_compact;      case 107 : itype = SH64_COMPACT_INSN_NEG_COMPACT; goto extract_sfmt_extsb_compact;      case 108 : itype = SH64_COMPACT_INSN_EXTUB_COMPACT; goto extract_sfmt_extsb_compact;      case 109 : itype = SH64_COMPACT_INSN_EXTUW_COMPACT; goto extract_sfmt_extsb_compact;      case 110 : itype = SH64_COMPACT_INSN_EXTSB_COMPACT; goto extract_sfmt_extsb_compact;      case 111 : itype = SH64_COMPACT_INSN_EXTSW_COMPACT; goto extract_sfmt_extsb_compact;      case 112 : /* fall through */
      case 113 : /* fall through */
      case 114 : /* fall through */
      case 115 : /* fall through */
      case 116 : /* fall through */
      case 117 : /* fall through */
      case 118 : /* fall through */
      case 119 : /* fall through */
      case 120 : /* fall through */
      case 121 : /* fall through */
      case 122 : /* fall through */
      case 123 : /* fall through */
      case 124 : /* fall through */
      case 125 : /* fall through */
      case 126 : /* fall through */
      case 127 : itype = SH64_COMPACT_INSN_ADDI_COMPACT; goto extract_sfmt_addi_compact;      case 128 : /* fall through */
      case 129 : /* fall through */
      case 130 : /* fall through */
      case 131 : /* fall through */
      case 132 : /* fall through */
      case 133 : /* fall through */
      case 134 : /* fall through */
      case 135 : /* fall through */
      case 136 : /* fall through */
      case 137 : /* fall through */
      case 138 : /* fall through */
      case 139 : /* fall through */
      case 140 : /* fall through */
      case 141 : /* fall through */
      case 142 : /* fall through */
      case 143 :
        {
          unsigned int val = (((insn >> 8) & (15 << 0)));
          switch (val)
          {
          case 0 : itype = SH64_COMPACT_INSN_MOVB5_COMPACT; goto extract_sfmt_movb5_compact;          case 1 : itype = SH64_COMPACT_INSN_MOVW5_COMPACT; goto extract_sfmt_movw5_compact;          case 4 : itype = SH64_COMPACT_INSN_MOVB10_COMPACT; goto extract_sfmt_movb10_compact;          case 5 : itype = SH64_COMPACT_INSN_MOVW11_COMPACT; goto extract_sfmt_movw11_compact;          case 8 : itype = SH64_COMPACT_INSN_CMPEQI_COMPACT; goto extract_sfmt_cmpeqi_compact;          case 9 : itype = SH64_COMPACT_INSN_BT_COMPACT; goto extract_sfmt_bf_compact;          case 11 : itype = SH64_COMPACT_INSN_BF_COMPACT; goto extract_sfmt_bf_compact;          case 13 : itype = SH64_COMPACT_INSN_BTS_COMPACT; goto extract_sfmt_bf_compact;          case 15 : itype = SH64_COMPACT_INSN_BFS_COMPACT; goto extract_sfmt_bf_compact;          default : itype = SH64_COMPACT_INSN_X_INVALID; goto extract_sfmt_empty;
          }
        }
      case 144 : /* fall through */
      case 145 : /* fall through */
      case 146 : /* fall through */
      case 147 : /* fall through */
      case 148 : /* fall through */
      case 149 : /* fall through */
      case 150 : /* fall through */
      case 151 : /* fall through */
      case 152 : /* fall through */
      case 153 : /* fall through */
      case 154 : /* fall through */
      case 155 : /* fall through */
      case 156 : /* fall through */
      case 157 : /* fall through */
      case 158 : /* fall through */
      case 159 : itype = SH64_COMPACT_INSN_MOVW10_COMPACT; goto extract_sfmt_movw10_compact;      case 160 : /* fall through */
      case 161 : /* fall through */
      case 162 : /* fall through */
      case 163 : /* fall through */
      case 164 : /* fall through */
      case 165 : /* fall through */
      case 166 : /* fall through */
      case 167 : /* fall through */
      case 168 : /* fall through */
      case 169 : /* fall through */
      case 170 : /* fall through */
      case 171 : /* fall through */
      case 172 : /* fall through */
      case 173 : /* fall through */
      case 174 : /* fall through */
      case 175 : itype = SH64_COMPACT_INSN_BRA_COMPACT; goto extract_sfmt_bra_compact;      case 176 : /* fall through */
      case 177 : /* fall through */
      case 178 : /* fall through */
      case 179 : /* fall through */
      case 180 : /* fall through */
      case 181 : /* fall through */
      case 182 : /* fall through */
      case 183 : /* fall through */
      case 184 : /* fall through */
      case 185 : /* fall through */
      case 186 : /* fall through */
      case 187 : /* fall through */
      case 188 : /* fall through */
      case 189 : /* fall through */
      case 190 : /* fall through */
      case 191 : itype = SH64_COMPACT_INSN_BSR_COMPACT; goto extract_sfmt_bsr_compact;      case 192 : /* fall through */
      case 193 : /* fall through */
      case 194 : /* fall through */
      case 195 : /* fall through */
      case 196 : /* fall through */
      case 197 : /* fall through */
      case 198 : /* fall through */
      case 199 : /* fall through */
      case 200 : /* fall through */
      case 201 : /* fall through */
      case 202 : /* fall through */
      case 203 : /* fall through */
      case 204 : /* fall through */
      case 205 : /* fall through */
      case 206 : /* fall through */
      case 207 :
        {
          unsigned int val = (((insn >> 8) & (15 << 0)));
          switch (val)
          {
          case 0 : itype = SH64_COMPACT_INSN_MOVB4_COMPACT; goto extract_sfmt_movb4_compact;          case 1 : itype = SH64_COMPACT_INSN_MOVW4_COMPACT; goto extract_sfmt_movw4_compact;          case 2 : itype = SH64_COMPACT_INSN_MOVL4_COMPACT; goto extract_sfmt_movl4_compact;          case 3 : itype = SH64_COMPACT_INSN_TRAPA_COMPACT; goto extract_sfmt_trapa_compact;          case 4 : itype = SH64_COMPACT_INSN_MOVB9_COMPACT; goto extract_sfmt_movb9_compact;          case 5 : itype = SH64_COMPACT_INSN_MOVW9_COMPACT; goto extract_sfmt_movw9_compact;          case 6 : itype = SH64_COMPACT_INSN_MOVL9_COMPACT; goto extract_sfmt_movl9_compact;          case 7 : itype = SH64_COMPACT_INSN_MOVA_COMPACT; goto extract_sfmt_mova_compact;          case 8 : itype = SH64_COMPACT_INSN_TSTI_COMPACT; goto extract_sfmt_tsti_compact;          case 9 : itype = SH64_COMPACT_INSN_ANDI_COMPACT; goto extract_sfmt_andi_compact;          case 10 : itype = SH64_COMPACT_INSN_XORI_COMPACT; goto extract_sfmt_xori_compact;          case 11 : itype = SH64_COMPACT_INSN_ORI_COMPACT; goto extract_sfmt_andi_compact;          case 12 : itype = SH64_COMPACT_INSN_TSTB_COMPACT; goto extract_sfmt_tstb_compact;          case 13 : itype = SH64_COMPACT_INSN_ANDB_COMPACT; goto extract_sfmt_andb_compact;          case 14 : itype = SH64_COMPACT_INSN_XORB_COMPACT; goto extract_sfmt_andb_compact;          case 15 : itype = SH64_COMPACT_INSN_ORB_COMPACT; goto extract_sfmt_andb_compact;          default : itype = SH64_COMPACT_INSN_X_INVALID; goto extract_sfmt_empty;
          }
        }
      case 208 : /* fall through */
      case 209 : /* fall through */
      case 210 : /* fall through */
      case 211 : /* fall through */
      case 212 : /* fall through */
      case 213 : /* fall through */
      case 214 : /* fall through */
      case 215 : /* fall through */
      case 216 : /* fall through */
      case 217 : /* fall through */
      case 218 : /* fall through */
      case 219 : /* fall through */
      case 220 : /* fall through */
      case 221 : /* fall through */
      case 222 : /* fall through */
      case 223 : itype = SH64_COMPACT_INSN_MOVL10_COMPACT; goto extract_sfmt_movl10_compact;      case 224 : /* fall through */
      case 225 : /* fall through */
      case 226 : /* fall through */
      case 227 : /* fall through */
      case 228 : /* fall through */
      case 229 : /* fall through */
      case 230 : /* fall through */
      case 231 : /* fall through */
      case 232 : /* fall through */
      case 233 : /* fall through */
      case 234 : /* fall through */
      case 235 : /* fall through */
      case 236 : /* fall through */
      case 237 : /* fall through */
      case 238 : /* fall through */
      case 239 : itype = SH64_COMPACT_INSN_MOVI_COMPACT; goto extract_sfmt_movi_compact;      case 240 : itype = SH64_COMPACT_INSN_FADD_COMPACT; goto extract_sfmt_fadd_compact;      case 241 : itype = SH64_COMPACT_INSN_FSUB_COMPACT; goto extract_sfmt_fadd_compact;      case 242 : itype = SH64_COMPACT_INSN_FMUL_COMPACT; goto extract_sfmt_fadd_compact;      case 243 : itype = SH64_COMPACT_INSN_FDIV_COMPACT; goto extract_sfmt_fadd_compact;      case 244 : itype = SH64_COMPACT_INSN_FCMPEQ_COMPACT; goto extract_sfmt_fcmpeq_compact;      case 245 : itype = SH64_COMPACT_INSN_FCMPGT_COMPACT; goto extract_sfmt_fcmpeq_compact;      case 246 : itype = SH64_COMPACT_INSN_FMOV4_COMPACT; goto extract_sfmt_fmov4_compact;      case 247 : itype = SH64_COMPACT_INSN_FMOV7_COMPACT; goto extract_sfmt_fmov7_compact;      case 248 : itype = SH64_COMPACT_INSN_FMOV2_COMPACT; goto extract_sfmt_fmov2_compact;      case 249 : itype = SH64_COMPACT_INSN_FMOV3_COMPACT; goto extract_sfmt_fmov3_compact;      case 250 : itype = SH64_COMPACT_INSN_FMOV5_COMPACT; goto extract_sfmt_fmov5_compact;      case 251 : itype = SH64_COMPACT_INSN_FMOV6_COMPACT; goto extract_sfmt_fmov6_compact;      case 252 : itype = SH64_COMPACT_INSN_FMOV1_COMPACT; goto extract_sfmt_fmov1_compact;      case 253 :
        {
          unsigned int val = (((insn >> 4) & (15 << 0)));
          switch (val)
          {
          case 0 : itype = SH64_COMPACT_INSN_FSTS_COMPACT; goto extract_sfmt_fsts_compact;          case 1 : itype = SH64_COMPACT_INSN_FLDS_COMPACT; goto extract_sfmt_flds_compact;          case 2 : itype = SH64_COMPACT_INSN_FLOAT_COMPACT; goto extract_sfmt_float_compact;          case 3 : itype = SH64_COMPACT_INSN_FTRC_COMPACT; goto extract_sfmt_ftrc_compact;          case 4 : itype = SH64_COMPACT_INSN_FNEG_COMPACT; goto extract_sfmt_fabs_compact;          case 5 : itype = SH64_COMPACT_INSN_FABS_COMPACT; goto extract_sfmt_fabs_compact;          case 6 : itype = SH64_COMPACT_INSN_FSQRT_COMPACT; goto extract_sfmt_fabs_compact;          case 8 : itype = SH64_COMPACT_INSN_FLDI0_COMPACT; goto extract_sfmt_fldi0_compact;          case 9 : itype = SH64_COMPACT_INSN_FLDI1_COMPACT; goto extract_sfmt_fldi0_compact;          case 10 : itype = SH64_COMPACT_INSN_FCNVSD_COMPACT; goto extract_sfmt_fcnvsd_compact;          case 11 : itype = SH64_COMPACT_INSN_FCNVDS_COMPACT; goto extract_sfmt_fcnvds_compact;          case 14 : itype = SH64_COMPACT_INSN_FIPR_COMPACT; goto extract_sfmt_fipr_compact;          case 15 :
            {
              unsigned int val = (((insn >> 9) & (1 << 0)));
              switch (val)
              {
              case 0 : itype = SH64_COMPACT_INSN_FTRV_COMPACT; goto extract_sfmt_ftrv_compact;              case 1 :
                {
                  unsigned int val = (((insn >> 11) & (1 << 0)));
                  switch (val)
                  {
                  case 0 : itype = SH64_COMPACT_INSN_FSCHG_COMPACT; goto extract_sfmt_fschg_compact;                  case 1 : itype = SH64_COMPACT_INSN_FRCHG_COMPACT; goto extract_sfmt_frchg_compact;                  default : itype = SH64_COMPACT_INSN_X_INVALID; goto extract_sfmt_empty;
                  }
                }
              default : itype = SH64_COMPACT_INSN_X_INVALID; goto extract_sfmt_empty;
              }
            }
          default : itype = SH64_COMPACT_INSN_X_INVALID; goto extract_sfmt_empty;
          }
        }
      case 254 : itype = SH64_COMPACT_INSN_FMAC_COMPACT; goto extract_sfmt_fmac_compact;      default : itype = SH64_COMPACT_INSN_X_INVALID; goto extract_sfmt_empty;
      }
    }
  }

  /* The instruction has been decoded, now extract the fields.  */

 extract_sfmt_empty:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
#define FLD(f) abuf->fields.fmt_empty.f


  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_empty", (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_add_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movl5_compact.f
    UINT f_rn;
    UINT f_rm;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);
    f_rm = EXTRACT_LSB0_UINT (insn, 16, 7, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rm) = f_rm;
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_add_compact", "f_rm 0x%x", 'x', f_rm, "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_addi_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_addi_compact.f
    UINT f_rn;
    UINT f_imm8;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);
    f_imm8 = EXTRACT_LSB0_UINT (insn, 16, 7, 8);

  /* Record the fields for the semantic handler.  */
  FLD (f_imm8) = f_imm8;
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_addi_compact", "f_imm8 0x%x", 'x', f_imm8, "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_addc_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movl5_compact.f
    UINT f_rn;
    UINT f_rm;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);
    f_rm = EXTRACT_LSB0_UINT (insn, 16, 7, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rm) = f_rm;
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_addc_compact", "f_rm 0x%x", 'x', f_rm, "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_addv_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movl5_compact.f
    UINT f_rn;
    UINT f_rm;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);
    f_rm = EXTRACT_LSB0_UINT (insn, 16, 7, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rm) = f_rm;
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_addv_compact", "f_rm 0x%x", 'x', f_rm, "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_and_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movl5_compact.f
    UINT f_rn;
    UINT f_rm;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);
    f_rm = EXTRACT_LSB0_UINT (insn, 16, 7, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rm) = f_rm;
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_and_compact", "f_rm 0x%x", 'x', f_rm, "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_andi_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_addi_compact.f
    UINT f_imm8;

    f_imm8 = EXTRACT_LSB0_UINT (insn, 16, 7, 8);

  /* Record the fields for the semantic handler.  */
  FLD (f_imm8) = f_imm8;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_andi_compact", "f_imm8 0x%x", 'x', f_imm8, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_andb_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_addi_compact.f
    UINT f_imm8;

    f_imm8 = EXTRACT_LSB0_UINT (insn, 16, 7, 8);

  /* Record the fields for the semantic handler.  */
  FLD (f_imm8) = f_imm8;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_andb_compact", "f_imm8 0x%x", 'x', f_imm8, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_bf_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_bf_compact.f
    SI f_disp8;

    f_disp8 = ((((EXTRACT_LSB0_INT (insn, 16, 7, 8)) << (1))) + (((pc) + (4))));

  /* Record the fields for the semantic handler.  */
  FLD (i_disp8) = f_disp8;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_bf_compact", "disp8 0x%x", 'x', f_disp8, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_bra_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_bra_compact.f
    SI f_disp12;

    f_disp12 = ((((EXTRACT_LSB0_INT (insn, 16, 11, 12)) << (1))) + (((pc) + (4))));

  /* Record the fields for the semantic handler.  */
  FLD (i_disp12) = f_disp12;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_bra_compact", "disp12 0x%x", 'x', f_disp12, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_braf_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movw10_compact.f
    UINT f_rn;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_braf_compact", "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_brk_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
#define FLD(f) abuf->fields.fmt_empty.f


  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_brk_compact", (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_bsr_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_bra_compact.f
    SI f_disp12;

    f_disp12 = ((((EXTRACT_LSB0_INT (insn, 16, 11, 12)) << (1))) + (((pc) + (4))));

  /* Record the fields for the semantic handler.  */
  FLD (i_disp12) = f_disp12;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_bsr_compact", "disp12 0x%x", 'x', f_disp12, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_bsrf_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movw10_compact.f
    UINT f_rn;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_bsrf_compact", "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_clrmac_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
#define FLD(f) abuf->fields.fmt_empty.f


  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_clrmac_compact", (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_clrs_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
#define FLD(f) abuf->fields.fmt_empty.f


  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_clrs_compact", (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_clrt_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
#define FLD(f) abuf->fields.fmt_empty.f


  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_clrt_compact", (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_cmpeq_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movl5_compact.f
    UINT f_rn;
    UINT f_rm;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);
    f_rm = EXTRACT_LSB0_UINT (insn, 16, 7, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rm) = f_rm;
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_cmpeq_compact", "f_rm 0x%x", 'x', f_rm, "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_cmpeqi_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_addi_compact.f
    UINT f_imm8;

    f_imm8 = EXTRACT_LSB0_UINT (insn, 16, 7, 8);

  /* Record the fields for the semantic handler.  */
  FLD (f_imm8) = f_imm8;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_cmpeqi_compact", "f_imm8 0x%x", 'x', f_imm8, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_cmppl_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movw10_compact.f
    UINT f_rn;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_cmppl_compact", "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_div0s_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movl5_compact.f
    UINT f_rn;
    UINT f_rm;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);
    f_rm = EXTRACT_LSB0_UINT (insn, 16, 7, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rm) = f_rm;
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_div0s_compact", "f_rm 0x%x", 'x', f_rm, "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_div0u_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
#define FLD(f) abuf->fields.fmt_empty.f


  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_div0u_compact", (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_div1_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movl5_compact.f
    UINT f_rn;
    UINT f_rm;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);
    f_rm = EXTRACT_LSB0_UINT (insn, 16, 7, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rm) = f_rm;
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_div1_compact", "f_rm 0x%x", 'x', f_rm, "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_dmulsl_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movl5_compact.f
    UINT f_rn;
    UINT f_rm;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);
    f_rm = EXTRACT_LSB0_UINT (insn, 16, 7, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rm) = f_rm;
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_dmulsl_compact", "f_rm 0x%x", 'x', f_rm, "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_dt_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movw10_compact.f
    UINT f_rn;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_dt_compact", "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_extsb_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movl5_compact.f
    UINT f_rn;
    UINT f_rm;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);
    f_rm = EXTRACT_LSB0_UINT (insn, 16, 7, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rm) = f_rm;
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_extsb_compact", "f_rm 0x%x", 'x', f_rm, "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_fabs_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movw10_compact.f
    UINT f_rn;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_fabs_compact", "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_fadd_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movl5_compact.f
    UINT f_rn;
    UINT f_rm;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);
    f_rm = EXTRACT_LSB0_UINT (insn, 16, 7, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rm) = f_rm;
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_fadd_compact", "f_rm 0x%x", 'x', f_rm, "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_fcmpeq_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movl5_compact.f
    UINT f_rn;
    UINT f_rm;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);
    f_rm = EXTRACT_LSB0_UINT (insn, 16, 7, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rm) = f_rm;
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_fcmpeq_compact", "f_rm 0x%x", 'x', f_rm, "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_fcnvds_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_fcnvds_compact.f
    SI f_dn;

    f_dn = ((EXTRACT_LSB0_UINT (insn, 16, 11, 3)) << (1));

  /* Record the fields for the semantic handler.  */
  FLD (f_dn) = f_dn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_fcnvds_compact", "f_dn 0x%x", 'x', f_dn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_fcnvsd_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_fcnvds_compact.f
    SI f_dn;

    f_dn = ((EXTRACT_LSB0_UINT (insn, 16, 11, 3)) << (1));

  /* Record the fields for the semantic handler.  */
  FLD (f_dn) = f_dn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_fcnvsd_compact", "f_dn 0x%x", 'x', f_dn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_fipr_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_fipr_compact.f
    SI f_vn;
    SI f_vm;

    f_vn = ((EXTRACT_LSB0_UINT (insn, 16, 11, 2)) << (2));
    f_vm = ((EXTRACT_LSB0_UINT (insn, 16, 9, 2)) << (2));

  /* Record the fields for the semantic handler.  */
  FLD (f_vm) = f_vm;
  FLD (f_vn) = f_vn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_fipr_compact", "f_vm 0x%x", 'x', f_vm, "f_vn 0x%x", 'x', f_vn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_flds_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movw10_compact.f
    UINT f_rn;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_flds_compact", "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_fldi0_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movw10_compact.f
    UINT f_rn;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_fldi0_compact", "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_float_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movw10_compact.f
    UINT f_rn;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_float_compact", "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_fmac_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movl5_compact.f
    UINT f_rn;
    UINT f_rm;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);
    f_rm = EXTRACT_LSB0_UINT (insn, 16, 7, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rm) = f_rm;
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_fmac_compact", "f_rm 0x%x", 'x', f_rm, "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_fmov1_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movl5_compact.f
    UINT f_rn;
    UINT f_rm;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);
    f_rm = EXTRACT_LSB0_UINT (insn, 16, 7, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rm) = f_rm;
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_fmov1_compact", "f_rm 0x%x", 'x', f_rm, "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_fmov2_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movl5_compact.f
    UINT f_rn;
    UINT f_rm;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);
    f_rm = EXTRACT_LSB0_UINT (insn, 16, 7, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rn) = f_rn;
  FLD (f_rm) = f_rm;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_fmov2_compact", "f_rn 0x%x", 'x', f_rn, "f_rm 0x%x", 'x', f_rm, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_fmov3_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movl5_compact.f
    UINT f_rn;
    UINT f_rm;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);
    f_rm = EXTRACT_LSB0_UINT (insn, 16, 7, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rn) = f_rn;
  FLD (f_rm) = f_rm;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_fmov3_compact", "f_rn 0x%x", 'x', f_rn, "f_rm 0x%x", 'x', f_rm, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_fmov4_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movl5_compact.f
    UINT f_rn;
    UINT f_rm;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);
    f_rm = EXTRACT_LSB0_UINT (insn, 16, 7, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rn) = f_rn;
  FLD (f_rm) = f_rm;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_fmov4_compact", "f_rn 0x%x", 'x', f_rn, "f_rm 0x%x", 'x', f_rm, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_fmov5_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movl5_compact.f
    UINT f_rn;
    UINT f_rm;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);
    f_rm = EXTRACT_LSB0_UINT (insn, 16, 7, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rm) = f_rm;
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_fmov5_compact", "f_rm 0x%x", 'x', f_rm, "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_fmov6_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movl5_compact.f
    UINT f_rn;
    UINT f_rm;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);
    f_rm = EXTRACT_LSB0_UINT (insn, 16, 7, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rm) = f_rm;
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_fmov6_compact", "f_rm 0x%x", 'x', f_rm, "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_fmov7_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movl5_compact.f
    UINT f_rn;
    UINT f_rm;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);
    f_rm = EXTRACT_LSB0_UINT (insn, 16, 7, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rm) = f_rm;
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_fmov7_compact", "f_rm 0x%x", 'x', f_rm, "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_frchg_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
#define FLD(f) abuf->fields.fmt_empty.f


  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_frchg_compact", (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_fschg_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
#define FLD(f) abuf->fields.fmt_empty.f


  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_fschg_compact", (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_fsts_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movw10_compact.f
    UINT f_rn;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_fsts_compact", "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_ftrc_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movw10_compact.f
    UINT f_rn;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_ftrc_compact", "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_ftrv_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_fipr_compact.f
    SI f_vn;

    f_vn = ((EXTRACT_LSB0_UINT (insn, 16, 11, 2)) << (2));

  /* Record the fields for the semantic handler.  */
  FLD (f_vn) = f_vn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_ftrv_compact", "f_vn 0x%x", 'x', f_vn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_jmp_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movw10_compact.f
    UINT f_rn;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_jmp_compact", "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_ldc_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movw10_compact.f
    UINT f_rn;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_ldc_compact", "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_ldcl_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movw10_compact.f
    UINT f_rn;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_ldcl_compact", "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_lds_fpscr_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movw10_compact.f
    UINT f_rn;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_lds_fpscr_compact", "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_ldsl_fpscr_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movw10_compact.f
    UINT f_rn;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_ldsl_fpscr_compact", "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_lds_fpul_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movw10_compact.f
    UINT f_rn;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_lds_fpul_compact", "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_ldsl_fpul_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movw10_compact.f
    UINT f_rn;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_ldsl_fpul_compact", "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_lds_mach_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movw10_compact.f
    UINT f_rn;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_lds_mach_compact", "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_ldsl_mach_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movw10_compact.f
    UINT f_rn;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_ldsl_mach_compact", "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_lds_macl_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movw10_compact.f
    UINT f_rn;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_lds_macl_compact", "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_ldsl_macl_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movw10_compact.f
    UINT f_rn;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_ldsl_macl_compact", "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_lds_pr_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movw10_compact.f
    UINT f_rn;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_lds_pr_compact", "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_ldsl_pr_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movw10_compact.f
    UINT f_rn;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_ldsl_pr_compact", "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_macl_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movl5_compact.f
    UINT f_rn;
    UINT f_rm;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);
    f_rm = EXTRACT_LSB0_UINT (insn, 16, 7, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rm) = f_rm;
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_macl_compact", "f_rm 0x%x", 'x', f_rm, "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_macw_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movl5_compact.f
    UINT f_rn;
    UINT f_rm;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);
    f_rm = EXTRACT_LSB0_UINT (insn, 16, 7, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rm) = f_rm;
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_macw_compact", "f_rm 0x%x", 'x', f_rm, "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_mov_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movl5_compact.f
    UINT f_rn;
    UINT f_rm;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);
    f_rm = EXTRACT_LSB0_UINT (insn, 16, 7, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rm) = f_rm;
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_mov_compact", "f_rm 0x%x", 'x', f_rm, "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_movi_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_addi_compact.f
    UINT f_rn;
    UINT f_imm8;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);
    f_imm8 = EXTRACT_LSB0_UINT (insn, 16, 7, 8);

  /* Record the fields for the semantic handler.  */
  FLD (f_imm8) = f_imm8;
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_movi_compact", "f_imm8 0x%x", 'x', f_imm8, "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_movb1_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movl5_compact.f
    UINT f_rn;
    UINT f_rm;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);
    f_rm = EXTRACT_LSB0_UINT (insn, 16, 7, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rm) = f_rm;
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_movb1_compact", "f_rm 0x%x", 'x', f_rm, "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_movb2_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movl5_compact.f
    UINT f_rn;
    UINT f_rm;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);
    f_rm = EXTRACT_LSB0_UINT (insn, 16, 7, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rm) = f_rm;
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_movb2_compact", "f_rm 0x%x", 'x', f_rm, "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_movb3_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movl5_compact.f
    UINT f_rn;
    UINT f_rm;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);
    f_rm = EXTRACT_LSB0_UINT (insn, 16, 7, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rm) = f_rm;
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_movb3_compact", "f_rm 0x%x", 'x', f_rm, "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_movb4_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_addi_compact.f
    UINT f_imm8;

    f_imm8 = EXTRACT_LSB0_UINT (insn, 16, 7, 8);

  /* Record the fields for the semantic handler.  */
  FLD (f_imm8) = f_imm8;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_movb4_compact", "f_imm8 0x%x", 'x', f_imm8, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_movb5_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movb5_compact.f
    UINT f_rm;
    UINT f_imm4;

    f_rm = EXTRACT_LSB0_UINT (insn, 16, 7, 4);
    f_imm4 = EXTRACT_LSB0_UINT (insn, 16, 3, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_imm4) = f_imm4;
  FLD (f_rm) = f_rm;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_movb5_compact", "f_imm4 0x%x", 'x', f_imm4, "f_rm 0x%x", 'x', f_rm, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_movb6_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movl5_compact.f
    UINT f_rn;
    UINT f_rm;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);
    f_rm = EXTRACT_LSB0_UINT (insn, 16, 7, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rm) = f_rm;
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_movb6_compact", "f_rm 0x%x", 'x', f_rm, "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_movb7_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movl5_compact.f
    UINT f_rn;
    UINT f_rm;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);
    f_rm = EXTRACT_LSB0_UINT (insn, 16, 7, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rm) = f_rm;
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_movb7_compact", "f_rm 0x%x", 'x', f_rm, "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_movb8_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movl5_compact.f
    UINT f_rn;
    UINT f_rm;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);
    f_rm = EXTRACT_LSB0_UINT (insn, 16, 7, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rm) = f_rm;
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_movb8_compact", "f_rm 0x%x", 'x', f_rm, "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_movb9_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_addi_compact.f
    UINT f_imm8;

    f_imm8 = EXTRACT_LSB0_UINT (insn, 16, 7, 8);

  /* Record the fields for the semantic handler.  */
  FLD (f_imm8) = f_imm8;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_movb9_compact", "f_imm8 0x%x", 'x', f_imm8, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_movb10_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movb5_compact.f
    UINT f_rm;
    UINT f_imm4;

    f_rm = EXTRACT_LSB0_UINT (insn, 16, 7, 4);
    f_imm4 = EXTRACT_LSB0_UINT (insn, 16, 3, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_imm4) = f_imm4;
  FLD (f_rm) = f_rm;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_movb10_compact", "f_imm4 0x%x", 'x', f_imm4, "f_rm 0x%x", 'x', f_rm, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_movl4_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movl10_compact.f
    SI f_imm8x4;

    f_imm8x4 = ((EXTRACT_LSB0_UINT (insn, 16, 7, 8)) << (2));

  /* Record the fields for the semantic handler.  */
  FLD (f_imm8x4) = f_imm8x4;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_movl4_compact", "f_imm8x4 0x%x", 'x', f_imm8x4, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_movl5_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movl5_compact.f
    UINT f_rn;
    UINT f_rm;
    SI f_imm4x4;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);
    f_rm = EXTRACT_LSB0_UINT (insn, 16, 7, 4);
    f_imm4x4 = ((EXTRACT_LSB0_UINT (insn, 16, 3, 4)) << (2));

  /* Record the fields for the semantic handler.  */
  FLD (f_imm4x4) = f_imm4x4;
  FLD (f_rm) = f_rm;
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_movl5_compact", "f_imm4x4 0x%x", 'x', f_imm4x4, "f_rm 0x%x", 'x', f_rm, "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_movl7_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movl5_compact.f
    UINT f_rn;
    UINT f_rm;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);
    f_rm = EXTRACT_LSB0_UINT (insn, 16, 7, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rm) = f_rm;
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_movl7_compact", "f_rm 0x%x", 'x', f_rm, "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_movl9_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movl10_compact.f
    SI f_imm8x4;

    f_imm8x4 = ((EXTRACT_LSB0_UINT (insn, 16, 7, 8)) << (2));

  /* Record the fields for the semantic handler.  */
  FLD (f_imm8x4) = f_imm8x4;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_movl9_compact", "f_imm8x4 0x%x", 'x', f_imm8x4, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_movl10_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movl10_compact.f
    UINT f_rn;
    SI f_imm8x4;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);
    f_imm8x4 = ((EXTRACT_LSB0_UINT (insn, 16, 7, 8)) << (2));

  /* Record the fields for the semantic handler.  */
  FLD (f_imm8x4) = f_imm8x4;
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_movl10_compact", "f_imm8x4 0x%x", 'x', f_imm8x4, "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_movl11_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movl5_compact.f
    UINT f_rn;
    UINT f_rm;
    SI f_imm4x4;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);
    f_rm = EXTRACT_LSB0_UINT (insn, 16, 7, 4);
    f_imm4x4 = ((EXTRACT_LSB0_UINT (insn, 16, 3, 4)) << (2));

  /* Record the fields for the semantic handler.  */
  FLD (f_imm4x4) = f_imm4x4;
  FLD (f_rm) = f_rm;
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_movl11_compact", "f_imm4x4 0x%x", 'x', f_imm4x4, "f_rm 0x%x", 'x', f_rm, "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_movw4_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movw10_compact.f
    SI f_imm8x2;

    f_imm8x2 = ((EXTRACT_LSB0_UINT (insn, 16, 7, 8)) << (1));

  /* Record the fields for the semantic handler.  */
  FLD (f_imm8x2) = f_imm8x2;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_movw4_compact", "f_imm8x2 0x%x", 'x', f_imm8x2, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_movw5_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movw5_compact.f
    UINT f_rn;
    SI f_imm4x2;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);
    f_imm4x2 = ((EXTRACT_LSB0_UINT (insn, 16, 3, 4)) << (1));

  /* Record the fields for the semantic handler.  */
  FLD (f_imm4x2) = f_imm4x2;
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_movw5_compact", "f_imm4x2 0x%x", 'x', f_imm4x2, "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_movw9_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movw10_compact.f
    SI f_imm8x2;

    f_imm8x2 = ((EXTRACT_LSB0_UINT (insn, 16, 7, 8)) << (1));

  /* Record the fields for the semantic handler.  */
  FLD (f_imm8x2) = f_imm8x2;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_movw9_compact", "f_imm8x2 0x%x", 'x', f_imm8x2, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_movw10_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movw10_compact.f
    UINT f_rn;
    SI f_imm8x2;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);
    f_imm8x2 = ((EXTRACT_LSB0_UINT (insn, 16, 7, 8)) << (1));

  /* Record the fields for the semantic handler.  */
  FLD (f_imm8x2) = f_imm8x2;
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_movw10_compact", "f_imm8x2 0x%x", 'x', f_imm8x2, "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_movw11_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movw11_compact.f
    UINT f_rm;
    SI f_imm4x2;

    f_rm = EXTRACT_LSB0_UINT (insn, 16, 7, 4);
    f_imm4x2 = ((EXTRACT_LSB0_UINT (insn, 16, 3, 4)) << (1));

  /* Record the fields for the semantic handler.  */
  FLD (f_imm4x2) = f_imm4x2;
  FLD (f_rm) = f_rm;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_movw11_compact", "f_imm4x2 0x%x", 'x', f_imm4x2, "f_rm 0x%x", 'x', f_rm, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_mova_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movl10_compact.f
    SI f_imm8x4;

    f_imm8x4 = ((EXTRACT_LSB0_UINT (insn, 16, 7, 8)) << (2));

  /* Record the fields for the semantic handler.  */
  FLD (f_imm8x4) = f_imm8x4;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_mova_compact", "f_imm8x4 0x%x", 'x', f_imm8x4, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_movcal_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movw10_compact.f
    UINT f_rn;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_movcal_compact", "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_movt_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movw10_compact.f
    UINT f_rn;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_movt_compact", "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_mull_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movl5_compact.f
    UINT f_rn;
    UINT f_rm;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);
    f_rm = EXTRACT_LSB0_UINT (insn, 16, 7, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rm) = f_rm;
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_mull_compact", "f_rm 0x%x", 'x', f_rm, "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_negc_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movl5_compact.f
    UINT f_rn;
    UINT f_rm;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);
    f_rm = EXTRACT_LSB0_UINT (insn, 16, 7, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rm) = f_rm;
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_negc_compact", "f_rm 0x%x", 'x', f_rm, "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_nop_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
#define FLD(f) abuf->fields.fmt_empty.f


  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_nop_compact", (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_rotcl_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movw10_compact.f
    UINT f_rn;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_rotcl_compact", "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_rts_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
#define FLD(f) abuf->fields.fmt_empty.f


  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_rts_compact", (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_shad_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movl5_compact.f
    UINT f_rn;
    UINT f_rm;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);
    f_rm = EXTRACT_LSB0_UINT (insn, 16, 7, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rm) = f_rm;
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_shad_compact", "f_rm 0x%x", 'x', f_rm, "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_shll2_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movw10_compact.f
    UINT f_rn;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_shll2_compact", "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_stc_gbr_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movw10_compact.f
    UINT f_rn;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_stc_gbr_compact", "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_stcl_gbr_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movw10_compact.f
    UINT f_rn;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_stcl_gbr_compact", "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_sts_fpscr_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movw10_compact.f
    UINT f_rn;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_sts_fpscr_compact", "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_stsl_fpscr_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movw10_compact.f
    UINT f_rn;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_stsl_fpscr_compact", "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_sts_fpul_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movw10_compact.f
    UINT f_rn;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_sts_fpul_compact", "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_stsl_fpul_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movw10_compact.f
    UINT f_rn;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_stsl_fpul_compact", "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_sts_mach_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movw10_compact.f
    UINT f_rn;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_sts_mach_compact", "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_stsl_mach_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movw10_compact.f
    UINT f_rn;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_stsl_mach_compact", "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_sts_macl_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movw10_compact.f
    UINT f_rn;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_sts_macl_compact", "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_stsl_macl_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movw10_compact.f
    UINT f_rn;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_stsl_macl_compact", "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_sts_pr_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movw10_compact.f
    UINT f_rn;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_sts_pr_compact", "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_stsl_pr_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movw10_compact.f
    UINT f_rn;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_stsl_pr_compact", "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_tasb_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movw10_compact.f
    UINT f_rn;

    f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_rn) = f_rn;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_tasb_compact", "f_rn 0x%x", 'x', f_rn, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_trapa_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_addi_compact.f
    UINT f_imm8;

    f_imm8 = EXTRACT_LSB0_UINT (insn, 16, 7, 8);

  /* Record the fields for the semantic handler.  */
  FLD (f_imm8) = f_imm8;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_trapa_compact", "f_imm8 0x%x", 'x', f_imm8, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_tsti_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_addi_compact.f
    UINT f_imm8;

    f_imm8 = EXTRACT_LSB0_UINT (insn, 16, 7, 8);

  /* Record the fields for the semantic handler.  */
  FLD (f_imm8) = f_imm8;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_tsti_compact", "f_imm8 0x%x", 'x', f_imm8, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_tstb_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_addi_compact.f
    UINT f_imm8;

    f_imm8 = EXTRACT_LSB0_UINT (insn, 16, 7, 8);

  /* Record the fields for the semantic handler.  */
  FLD (f_imm8) = f_imm8;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_tstb_compact", "f_imm8 0x%x", 'x', f_imm8, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_xori_compact:
  {
    const IDESC *idesc = &sh64_compact_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_addi_compact.f
    UINT f_imm8;

    f_imm8 = EXTRACT_LSB0_UINT (insn, 16, 7, 8);

  /* Record the fields for the semantic handler.  */
  FLD (f_imm8) = f_imm8;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_xori_compact", "f_imm8 0x%x", 'x', f_imm8, (char *) 0));

#undef FLD
    return idesc;
  }

}
