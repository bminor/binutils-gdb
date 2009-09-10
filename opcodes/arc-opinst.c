/* Semantic operand instances for arc.

THIS FILE IS MACHINE GENERATED WITH CGEN.

Copyright 1996-2005 Free Software Foundation, Inc.

This file is part of the GNU Binutils and/or GDB, the GNU debugger.

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
51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.

*/

#include "sysdep.h"
#include "ansidecl.h"
#include "bfd.h"
#include "symcat.h"
#include "arc-desc.h"
#include "arc-opc.h"

/* Operand references.  */

#if defined (__STDC__) || defined (ALMOST_STDC) || defined (HAVE_STRINGIZE)
#define OP_ENT(op) ARC_OPERAND_##op
#else
#define OP_ENT(op) ARC_OPERAND_/**/op
#endif
#define INPUT CGEN_OPINST_INPUT
#define OUTPUT CGEN_OPINST_OUTPUT
#define END CGEN_OPINST_END
#define COND_REF CGEN_OPINST_COND_REF

static const CGEN_OPINST sfmt_empty_ops[] ATTRIBUTE_UNUSED = {
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_b_s_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "h_auxr_SI_34", HW_H_AUXR, CGEN_MODE_SI, 0, 34, COND_REF },
  { INPUT, "h_auxr_SI_37", HW_H_AUXR, CGEN_MODE_SI, 0, 37, COND_REF },
  { INPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { INPUT, "h_prof_offset_SI_0", HW_H_PROF_OFFSET, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_timer_expire_SI_0", HW_H_TIMER_EXPIRE, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_ubit_BI", HW_H_UBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "i2cond", HW_H_I2COND, CGEN_MODE_BI, OP_ENT (I2COND), 0, 0 },
  { INPUT, "label10", HW_H_IADDR, CGEN_MODE_USI, OP_ENT (LABEL10), 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "h_auxr_SI_11", HW_H_AUXR, CGEN_MODE_SI, 0, 11, COND_REF },
  { OUTPUT, "h_auxr_SI_33", HW_H_AUXR, CGEN_MODE_SI, 0, 33, COND_REF },
  { OUTPUT, "h_cr_SI_29", HW_H_CR, CGEN_MODE_SI, 0, 29, COND_REF },
  { OUTPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_bcc_s_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "h_auxr_SI_34", HW_H_AUXR, CGEN_MODE_SI, 0, 34, COND_REF },
  { INPUT, "h_auxr_SI_37", HW_H_AUXR, CGEN_MODE_SI, 0, 37, COND_REF },
  { INPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { INPUT, "h_prof_offset_SI_0", HW_H_PROF_OFFSET, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_timer_expire_SI_0", HW_H_TIMER_EXPIRE, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_ubit_BI", HW_H_UBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "i3cond", HW_H_I3COND, CGEN_MODE_BI, OP_ENT (I3COND), 0, 0 },
  { INPUT, "label7", HW_H_IADDR, CGEN_MODE_USI, OP_ENT (LABEL7), 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "h_auxr_SI_11", HW_H_AUXR, CGEN_MODE_SI, 0, 11, COND_REF },
  { OUTPUT, "h_auxr_SI_33", HW_H_AUXR, CGEN_MODE_SI, 0, 33, COND_REF },
  { OUTPUT, "h_cr_SI_29", HW_H_CR, CGEN_MODE_SI, 0, 29, COND_REF },
  { OUTPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_brcc_s_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "R_b", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_B), 0, COND_REF },
  { INPUT, "RccS", HW_H_RCCS, CGEN_MODE_BI, OP_ENT (RCCS), 0, 0 },
  { INPUT, "h_auxr_SI_34", HW_H_AUXR, CGEN_MODE_SI, 0, 34, COND_REF },
  { INPUT, "h_auxr_SI_37", HW_H_AUXR, CGEN_MODE_SI, 0, 37, COND_REF },
  { INPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { INPUT, "h_prof_offset_SI_0", HW_H_PROF_OFFSET, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_timer_expire_SI_0", HW_H_TIMER_EXPIRE, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_ubit_BI", HW_H_UBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "label8", HW_H_IADDR, CGEN_MODE_USI, OP_ENT (LABEL8), 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "h_auxr_SI_11", HW_H_AUXR, CGEN_MODE_SI, 0, 11, COND_REF },
  { OUTPUT, "h_auxr_SI_33", HW_H_AUXR, CGEN_MODE_SI, 0, 33, COND_REF },
  { OUTPUT, "h_cr_SI_29", HW_H_CR, CGEN_MODE_SI, 0, 29, COND_REF },
  { OUTPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_bcc_l_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "Qcondb", HW_H_QCONDB, CGEN_MODE_BI, OP_ENT (QCONDB), 0, 0 },
  { INPUT, "h_auxr_SI_34", HW_H_AUXR, CGEN_MODE_SI, 0, 34, COND_REF },
  { INPUT, "h_auxr_SI_37", HW_H_AUXR, CGEN_MODE_SI, 0, 37, COND_REF },
  { INPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { INPUT, "h_prof_offset_SI_0", HW_H_PROF_OFFSET, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_timer_expire_SI_0", HW_H_TIMER_EXPIRE, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_ubit_BI", HW_H_UBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "label21", HW_H_IADDR, CGEN_MODE_USI, OP_ENT (LABEL21), 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "h_auxr_SI_11", HW_H_AUXR, CGEN_MODE_SI, 0, 11, COND_REF },
  { OUTPUT, "h_auxr_SI_33", HW_H_AUXR, CGEN_MODE_SI, 0, 33, COND_REF },
  { OUTPUT, "h_cr_SI_29", HW_H_CR, CGEN_MODE_SI, 0, 29, COND_REF },
  { OUTPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_b_l_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "h_auxr_SI_34", HW_H_AUXR, CGEN_MODE_SI, 0, 34, 0 },
  { INPUT, "h_auxr_SI_37", HW_H_AUXR, CGEN_MODE_SI, 0, 37, COND_REF },
  { INPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, 0 },
  { INPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { INPUT, "h_prof_offset_SI_0", HW_H_PROF_OFFSET, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_timer_expire_SI_0", HW_H_TIMER_EXPIRE, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "h_ubit_BI", HW_H_UBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "label25", HW_H_IADDR, CGEN_MODE_USI, OP_ENT (LABEL25), 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "h_auxr_SI_11", HW_H_AUXR, CGEN_MODE_SI, 0, 11, COND_REF },
  { OUTPUT, "h_auxr_SI_33", HW_H_AUXR, CGEN_MODE_SI, 0, 33, COND_REF },
  { OUTPUT, "h_cr_SI_29", HW_H_CR, CGEN_MODE_SI, 0, 29, COND_REF },
  { OUTPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_brcc_RC_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, COND_REF },
  { INPUT, "Rcc", HW_H_RCC, CGEN_MODE_SI, OP_ENT (RCC), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_auxr_SI_34", HW_H_AUXR, CGEN_MODE_SI, 0, 34, 0 },
  { INPUT, "h_auxr_SI_37", HW_H_AUXR, CGEN_MODE_SI, 0, 37, COND_REF },
  { INPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { INPUT, "h_prof_offset_SI_0", HW_H_PROF_OFFSET, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_timer_expire_SI_0", HW_H_TIMER_EXPIRE, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "h_ubit_BI", HW_H_UBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "label9", HW_H_IADDR, CGEN_MODE_USI, OP_ENT (LABEL9), 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "h_auxr_SI_11", HW_H_AUXR, CGEN_MODE_SI, 0, 11, COND_REF },
  { OUTPUT, "h_auxr_SI_33", HW_H_AUXR, CGEN_MODE_SI, 0, 33, COND_REF },
  { OUTPUT, "h_cr_SI_29", HW_H_CR, CGEN_MODE_SI, 0, 29, COND_REF },
  { OUTPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_brcc_U6_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "Rcc", HW_H_RCC, CGEN_MODE_SI, OP_ENT (RCC), 0, COND_REF },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U6), 0, COND_REF },
  { INPUT, "h_auxr_SI_34", HW_H_AUXR, CGEN_MODE_SI, 0, 34, 0 },
  { INPUT, "h_auxr_SI_37", HW_H_AUXR, CGEN_MODE_SI, 0, 37, COND_REF },
  { INPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, 0 },
  { INPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { INPUT, "h_prof_offset_SI_0", HW_H_PROF_OFFSET, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_timer_expire_SI_0", HW_H_TIMER_EXPIRE, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "h_ubit_BI", HW_H_UBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "label9", HW_H_IADDR, CGEN_MODE_USI, OP_ENT (LABEL9), 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "h_auxr_SI_11", HW_H_AUXR, CGEN_MODE_SI, 0, 11, COND_REF },
  { OUTPUT, "h_auxr_SI_33", HW_H_AUXR, CGEN_MODE_SI, 0, 33, COND_REF },
  { OUTPUT, "h_cr_SI_29", HW_H_CR, CGEN_MODE_SI, 0, 29, COND_REF },
  { OUTPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_bl_s_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "h_auxr_SI_34", HW_H_AUXR, CGEN_MODE_SI, 0, 34, 0 },
  { INPUT, "h_auxr_SI_37", HW_H_AUXR, CGEN_MODE_SI, 0, 37, COND_REF },
  { INPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, 0 },
  { INPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { INPUT, "h_prof_offset_SI_0", HW_H_PROF_OFFSET, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_timer_expire_SI_0", HW_H_TIMER_EXPIRE, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "h_ubit_BI", HW_H_UBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "label13a", HW_H_IADDR, CGEN_MODE_USI, OP_ENT (LABEL13A), 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "h_auxr_SI_11", HW_H_AUXR, CGEN_MODE_SI, 0, 11, COND_REF },
  { OUTPUT, "h_auxr_SI_33", HW_H_AUXR, CGEN_MODE_SI, 0, 33, COND_REF },
  { OUTPUT, "h_cr_SI_29", HW_H_CR, CGEN_MODE_SI, 0, 29, COND_REF },
  { OUTPUT, "h_cr_SI_31", HW_H_CR, CGEN_MODE_SI, 0, 31, COND_REF },
  { OUTPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_blcc_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "Qcondb", HW_H_QCONDB, CGEN_MODE_BI, OP_ENT (QCONDB), 0, 0 },
  { INPUT, "h_auxr_SI_34", HW_H_AUXR, CGEN_MODE_SI, 0, 34, COND_REF },
  { INPUT, "h_auxr_SI_37", HW_H_AUXR, CGEN_MODE_SI, 0, 37, COND_REF },
  { INPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { INPUT, "h_prof_offset_SI_0", HW_H_PROF_OFFSET, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_timer_expire_SI_0", HW_H_TIMER_EXPIRE, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_ubit_BI", HW_H_UBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "label21a", HW_H_IADDR, CGEN_MODE_USI, OP_ENT (LABEL21A), 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "h_auxr_SI_11", HW_H_AUXR, CGEN_MODE_SI, 0, 11, COND_REF },
  { OUTPUT, "h_auxr_SI_33", HW_H_AUXR, CGEN_MODE_SI, 0, 33, COND_REF },
  { OUTPUT, "h_cr_SI_29", HW_H_CR, CGEN_MODE_SI, 0, 29, COND_REF },
  { OUTPUT, "h_cr_SI_31", HW_H_CR, CGEN_MODE_SI, 0, 31, COND_REF },
  { OUTPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_bl_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "h_auxr_SI_34", HW_H_AUXR, CGEN_MODE_SI, 0, 34, 0 },
  { INPUT, "h_auxr_SI_37", HW_H_AUXR, CGEN_MODE_SI, 0, 37, COND_REF },
  { INPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, 0 },
  { INPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { INPUT, "h_prof_offset_SI_0", HW_H_PROF_OFFSET, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_timer_expire_SI_0", HW_H_TIMER_EXPIRE, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "h_ubit_BI", HW_H_UBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "label25a", HW_H_IADDR, CGEN_MODE_USI, OP_ENT (LABEL25A), 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "h_auxr_SI_11", HW_H_AUXR, CGEN_MODE_SI, 0, 11, COND_REF },
  { OUTPUT, "h_auxr_SI_33", HW_H_AUXR, CGEN_MODE_SI, 0, 33, COND_REF },
  { OUTPUT, "h_cr_SI_29", HW_H_CR, CGEN_MODE_SI, 0, 29, COND_REF },
  { OUTPUT, "h_cr_SI_31", HW_H_CR, CGEN_MODE_SI, 0, 31, COND_REF },
  { OUTPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_bl_d_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "h_auxr_SI_34", HW_H_AUXR, CGEN_MODE_SI, 0, 34, 0 },
  { INPUT, "h_auxr_SI_37", HW_H_AUXR, CGEN_MODE_SI, 0, 37, COND_REF },
  { INPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { INPUT, "h_prof_offset_SI_0", HW_H_PROF_OFFSET, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_timer_expire_SI_0", HW_H_TIMER_EXPIRE, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "h_ubit_BI", HW_H_UBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "label25a", HW_H_IADDR, CGEN_MODE_USI, OP_ENT (LABEL25A), 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "h_auxr_SI_11", HW_H_AUXR, CGEN_MODE_SI, 0, 11, COND_REF },
  { OUTPUT, "h_auxr_SI_33", HW_H_AUXR, CGEN_MODE_SI, 0, 33, COND_REF },
  { OUTPUT, "h_cr_SI_29", HW_H_CR, CGEN_MODE_SI, 0, 29, COND_REF },
  { OUTPUT, "h_cr_SI_31", HW_H_CR, CGEN_MODE_SI, 0, 31, COND_REF },
  { OUTPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_ld_abs_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_SI_eaddr", HW_H_MEMORY, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { INPUT, "s9", HW_H_SINT, CGEN_MODE_INT, OP_ENT (S9), 0, 0 },
  { OUTPUT, "RA", HW_H_CR, CGEN_MODE_SI, OP_ENT (RA), 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_ld__AW_abs_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_SI_eaddr", HW_H_MEMORY, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { INPUT, "s9", HW_H_SINT, CGEN_MODE_INT, OP_ENT (S9), 0, 0 },
  { OUTPUT, "RA", HW_H_CR, CGEN_MODE_SI, OP_ENT (RA), 0, 0 },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_ld_abc_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_SI_eaddr", HW_H_MEMORY, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RA", HW_H_CR, CGEN_MODE_SI, OP_ENT (RA), 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_ld__AW_abc_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_SI_eaddr", HW_H_MEMORY, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RA", HW_H_CR, CGEN_MODE_SI, OP_ENT (RA), 0, 0 },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_ld_s_abc_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "R_b", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_B), 0, 0 },
  { INPUT, "R_c", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_C), 0, 0 },
  { INPUT, "h_memory_SI_eaddr", HW_H_MEMORY, CGEN_MODE_SI, 0, 0, 0 },
  { OUTPUT, "R_a", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_A), 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_ld_s_abu_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "R_b", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_B), 0, 0 },
  { INPUT, "h_memory_SI_eaddr", HW_H_MEMORY, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "sc_u5_", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (SC_U5_), 0, 0 },
  { OUTPUT, "R_c", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_C), 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_ld_s_absp_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "SP", HW_H_SP, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "h_memory_SI_eaddr", HW_H_MEMORY, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "u5x4", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U5X4), 0, 0 },
  { OUTPUT, "R_b", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_B), 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_ld_s_gprel_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "GP", HW_H_GP, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "h_memory_SI_eaddr", HW_H_MEMORY, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "sc_s9_", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (SC_S9_), 0, 0 },
  { OUTPUT, "R0", HW_H_R0, CGEN_MODE_SI, 0, 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_ld_s_pcrel_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "h_memory_SI_eaddr", HW_H_MEMORY, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, 0 },
  { INPUT, "u8x4", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U8X4), 0, 0 },
  { OUTPUT, "R_b", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_B), 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_ldb_abs_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_QI_eaddr", HW_H_MEMORY, CGEN_MODE_QI, 0, 0, 0 },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { INPUT, "s9", HW_H_SINT, CGEN_MODE_INT, OP_ENT (S9), 0, 0 },
  { OUTPUT, "RA", HW_H_CR, CGEN_MODE_SI, OP_ENT (RA), 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_ldb__AW_abs_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_QI_eaddr", HW_H_MEMORY, CGEN_MODE_QI, 0, 0, 0 },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { INPUT, "s9", HW_H_SINT, CGEN_MODE_INT, OP_ENT (S9), 0, 0 },
  { OUTPUT, "RA", HW_H_CR, CGEN_MODE_SI, OP_ENT (RA), 0, 0 },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_ldb_as_abs_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_QI_eaddr", HW_H_MEMORY, CGEN_MODE_QI, 0, 0, 0 },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RA", HW_H_CR, CGEN_MODE_SI, OP_ENT (RA), 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_ldb_abc_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_QI_eaddr", HW_H_MEMORY, CGEN_MODE_QI, 0, 0, 0 },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RA", HW_H_CR, CGEN_MODE_SI, OP_ENT (RA), 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_ldb__AW_abc_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_QI_eaddr", HW_H_MEMORY, CGEN_MODE_QI, 0, 0, 0 },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RA", HW_H_CR, CGEN_MODE_SI, OP_ENT (RA), 0, 0 },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_ldb_as_abc_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_QI_eaddr", HW_H_MEMORY, CGEN_MODE_QI, 0, 0, 0 },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RA", HW_H_CR, CGEN_MODE_SI, OP_ENT (RA), 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_ldb_s_abc_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "R_b", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_B), 0, 0 },
  { INPUT, "R_c", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_C), 0, 0 },
  { INPUT, "h_memory_QI_eaddr", HW_H_MEMORY, CGEN_MODE_QI, 0, 0, 0 },
  { OUTPUT, "R_a", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_A), 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_ldb_s_abu_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "R_b", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_B), 0, 0 },
  { INPUT, "h_memory_QI_eaddr", HW_H_MEMORY, CGEN_MODE_QI, 0, 0, 0 },
  { INPUT, "sc_u5b", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (SC_U5B), 0, 0 },
  { OUTPUT, "R_c", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_C), 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_ldb_s_absp_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "SP", HW_H_SP, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "h_memory_QI_eaddr", HW_H_MEMORY, CGEN_MODE_QI, 0, 0, 0 },
  { INPUT, "u5x4", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U5X4), 0, 0 },
  { OUTPUT, "R_b", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_B), 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_ldb_s_gprel_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "GP", HW_H_GP, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "h_memory_QI_eaddr", HW_H_MEMORY, CGEN_MODE_QI, 0, 0, 0 },
  { INPUT, "sc_s9b", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (SC_S9B), 0, 0 },
  { OUTPUT, "R0", HW_H_R0, CGEN_MODE_SI, 0, 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_ldw_abs_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_eaddr", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, 0 },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { INPUT, "s9", HW_H_SINT, CGEN_MODE_INT, OP_ENT (S9), 0, 0 },
  { OUTPUT, "RA", HW_H_CR, CGEN_MODE_SI, OP_ENT (RA), 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_ldw__AW_abs_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_eaddr", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, 0 },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { INPUT, "s9", HW_H_SINT, CGEN_MODE_INT, OP_ENT (S9), 0, 0 },
  { OUTPUT, "RA", HW_H_CR, CGEN_MODE_SI, OP_ENT (RA), 0, 0 },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_ldw_abc_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_eaddr", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, 0 },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RA", HW_H_CR, CGEN_MODE_SI, OP_ENT (RA), 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_ldw__AW_abc_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_eaddr", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, 0 },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RA", HW_H_CR, CGEN_MODE_SI, OP_ENT (RA), 0, 0 },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_ldw_s_abc_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "R_b", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_B), 0, 0 },
  { INPUT, "R_c", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_C), 0, 0 },
  { INPUT, "h_memory_HI_eaddr", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, 0 },
  { OUTPUT, "R_a", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_A), 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_ldw_s_abu_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "R_b", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_B), 0, 0 },
  { INPUT, "h_memory_HI_eaddr", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, 0 },
  { INPUT, "sc_u5w", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (SC_U5W), 0, 0 },
  { OUTPUT, "R_c", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_C), 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_ldw_s_gprel_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "GP", HW_H_GP, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "h_memory_HI_eaddr", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, 0 },
  { INPUT, "sc_s9w", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (SC_S9W), 0, 0 },
  { OUTPUT, "R0", HW_H_R0, CGEN_MODE_SI, 0, 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_st_abs_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { INPUT, "s9", HW_H_SINT, CGEN_MODE_INT, OP_ENT (S9), 0, 0 },
  { OUTPUT, "h_memory_SI_eaddr", HW_H_MEMORY, CGEN_MODE_SI, 0, 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_st__AW_abs_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { INPUT, "s9", HW_H_SINT, CGEN_MODE_INT, OP_ENT (S9), 0, 0 },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { OUTPUT, "h_memory_SI_eaddr", HW_H_MEMORY, CGEN_MODE_SI, 0, 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_st_s_abu_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "R_b", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_B), 0, 0 },
  { INPUT, "R_c", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_C), 0, 0 },
  { INPUT, "sc_u5_", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (SC_U5_), 0, 0 },
  { OUTPUT, "h_memory_SI_eaddr", HW_H_MEMORY, CGEN_MODE_SI, 0, 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_st_s_absp_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "R_b", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_B), 0, 0 },
  { INPUT, "SP", HW_H_SP, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "u5x4", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U5X4), 0, 0 },
  { OUTPUT, "h_memory_SI_eaddr", HW_H_MEMORY, CGEN_MODE_SI, 0, 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_stb_abs_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { INPUT, "s9", HW_H_SINT, CGEN_MODE_INT, OP_ENT (S9), 0, 0 },
  { OUTPUT, "h_memory_QI_eaddr", HW_H_MEMORY, CGEN_MODE_QI, 0, 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_stb__AW_abs_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { INPUT, "s9", HW_H_SINT, CGEN_MODE_INT, OP_ENT (S9), 0, 0 },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { OUTPUT, "h_memory_QI_eaddr", HW_H_MEMORY, CGEN_MODE_QI, 0, 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_stb_as_abs_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "h_memory_QI_eaddr", HW_H_MEMORY, CGEN_MODE_QI, 0, 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_stb_s_abu_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "R_b", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_B), 0, 0 },
  { INPUT, "R_c", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_C), 0, 0 },
  { INPUT, "sc_u5b", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (SC_U5B), 0, 0 },
  { OUTPUT, "h_memory_QI_eaddr", HW_H_MEMORY, CGEN_MODE_QI, 0, 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_stb_s_absp_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "R_b", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_B), 0, 0 },
  { INPUT, "SP", HW_H_SP, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "u5x4", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U5X4), 0, 0 },
  { OUTPUT, "h_memory_QI_eaddr", HW_H_MEMORY, CGEN_MODE_QI, 0, 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_stw_abs_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { INPUT, "s9", HW_H_SINT, CGEN_MODE_INT, OP_ENT (S9), 0, 0 },
  { OUTPUT, "h_memory_HI_eaddr", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_stw__AW_abs_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { INPUT, "s9", HW_H_SINT, CGEN_MODE_INT, OP_ENT (S9), 0, 0 },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { OUTPUT, "h_memory_HI_eaddr", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_stw_s_abu_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "R_b", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_B), 0, 0 },
  { INPUT, "R_c", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_C), 0, 0 },
  { INPUT, "sc_u5w", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (SC_U5W), 0, 0 },
  { OUTPUT, "h_memory_HI_eaddr", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_add_L_s12__RA__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { INPUT, "s12", HW_H_SINT, CGEN_MODE_INT, OP_ENT (S12), 0, 0 },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { OUTPUT, "cbit", HW_H_CBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_add_ccu6__RA__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, COND_REF },
  { INPUT, "Qcondb", HW_H_QCONDB, CGEN_MODE_BI, OP_ENT (QCONDB), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U6), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { OUTPUT, "cbit", HW_H_CBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_add_L_u6__RA__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U6), 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RA", HW_H_CR, CGEN_MODE_SI, OP_ENT (RA), 0, 0 },
  { OUTPUT, "cbit", HW_H_CBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_add_L_r_r__RA__RC_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RA", HW_H_CR, CGEN_MODE_SI, OP_ENT (RA), 0, 0 },
  { OUTPUT, "cbit", HW_H_CBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_add_cc__RA__RC_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, COND_REF },
  { INPUT, "Qcondb", HW_H_QCONDB, CGEN_MODE_BI, OP_ENT (QCONDB), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { OUTPUT, "cbit", HW_H_CBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_add_s_abc_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "R_b", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_B), 0, 0 },
  { INPUT, "R_c", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_C), 0, 0 },
  { OUTPUT, "R_a", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_A), 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_add_s_cbu3_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "R_b", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_B), 0, 0 },
  { INPUT, "u3", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U3), 0, 0 },
  { OUTPUT, "R_c", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_C), 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_add_s_mcah_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "R_b", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_B), 0, 0 },
  { INPUT, "Rh", HW_H_CR, CGEN_MODE_SI, OP_ENT (RH), 0, 0 },
  { INPUT, "f_op_h", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_2_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "R_b", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_B), 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_add_s_absp_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "SP", HW_H_SP, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "u5x4", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U5X4), 0, 0 },
  { OUTPUT, "R_b", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_B), 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_add_s_asspsp_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "SP", HW_H_SP, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "u5x4", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U5X4), 0, 0 },
  { OUTPUT, "SP", HW_H_SP, CGEN_MODE_SI, 0, 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_add_s_gp_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "GP", HW_H_GP, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "s9x4", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (S9X4), 0, 0 },
  { OUTPUT, "R0", HW_H_R0, CGEN_MODE_SI, 0, 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_add_s_r_u7_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "R_b", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_B), 0, 0 },
  { INPUT, "u7", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U7), 0, 0 },
  { OUTPUT, "R_b", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_B), 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_adc_L_s12__RA__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { INPUT, "cbit", HW_H_CBIT, CGEN_MODE_BI, 0, 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { INPUT, "s12", HW_H_SINT, CGEN_MODE_INT, OP_ENT (S12), 0, 0 },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { OUTPUT, "cbit", HW_H_CBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_adc_ccu6__RA__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, COND_REF },
  { INPUT, "Qcondb", HW_H_QCONDB, CGEN_MODE_BI, OP_ENT (QCONDB), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U6), 0, COND_REF },
  { INPUT, "cbit", HW_H_CBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { OUTPUT, "cbit", HW_H_CBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_adc_L_u6__RA__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U6), 0, 0 },
  { INPUT, "cbit", HW_H_CBIT, CGEN_MODE_BI, 0, 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RA", HW_H_CR, CGEN_MODE_SI, OP_ENT (RA), 0, 0 },
  { OUTPUT, "cbit", HW_H_CBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_adc_L_r_r__RA__RC_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, 0 },
  { INPUT, "cbit", HW_H_CBIT, CGEN_MODE_BI, 0, 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RA", HW_H_CR, CGEN_MODE_SI, OP_ENT (RA), 0, 0 },
  { OUTPUT, "cbit", HW_H_CBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_adc_cc__RA__RC_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, COND_REF },
  { INPUT, "Qcondb", HW_H_QCONDB, CGEN_MODE_BI, OP_ENT (QCONDB), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, COND_REF },
  { INPUT, "cbit", HW_H_CBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { OUTPUT, "cbit", HW_H_CBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_I16_GO_SUB_s_go_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "R_b", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_B), 0, 0 },
  { INPUT, "R_c", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_C), 0, 0 },
  { OUTPUT, "R_b", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_B), 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_sub_s_go_sub_ne_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "R_b", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_B), 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_sub_s_ssb_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "R_b", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_B), 0, 0 },
  { INPUT, "u5", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U5), 0, 0 },
  { OUTPUT, "R_b", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_B), 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_and_L_s12__RA__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { INPUT, "s12", HW_H_SINT, CGEN_MODE_INT, OP_ENT (S12), 0, 0 },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_and_ccu6__RA__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, COND_REF },
  { INPUT, "Qcondb", HW_H_QCONDB, CGEN_MODE_BI, OP_ENT (QCONDB), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U6), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_and_L_u6__RA__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U6), 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RA", HW_H_CR, CGEN_MODE_SI, OP_ENT (RA), 0, 0 },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_and_L_r_r__RA__RC_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RA", HW_H_CR, CGEN_MODE_SI, OP_ENT (RA), 0, 0 },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_and_cc__RA__RC_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, COND_REF },
  { INPUT, "Qcondb", HW_H_QCONDB, CGEN_MODE_BI, OP_ENT (QCONDB), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_mov_L_s12__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_SI, OP_ENT (F), 0, 0 },
  { INPUT, "s12", HW_H_SINT, CGEN_MODE_INT, OP_ENT (S12), 0, 0 },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_mov_ccu6__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_SI, OP_ENT (F), 0, COND_REF },
  { INPUT, "Qcondb", HW_H_QCONDB, CGEN_MODE_BI, OP_ENT (QCONDB), 0, 0 },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U6), 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_mov_L_u6__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_SI, OP_ENT (F), 0, 0 },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U6), 0, 0 },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_mov_L_r_r__RC_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_SI, OP_ENT (F), 0, 0 },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_mov_cc__RC_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_SI, OP_ENT (F), 0, COND_REF },
  { INPUT, "Qcondb", HW_H_QCONDB, CGEN_MODE_BI, OP_ENT (QCONDB), 0, 0 },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, COND_REF },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_mov_s_mcah_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "Rh", HW_H_CR, CGEN_MODE_SI, OP_ENT (RH), 0, 0 },
  { INPUT, "f_op_h", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_2_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "R_b", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_B), 0, 0 },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_mov_s_mcahb_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "R_b", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_B), 0, 0 },
  { OUTPUT, "Rh", HW_H_CR, CGEN_MODE_SI, OP_ENT (RH), 0, 0 },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_mov_s_r_u7_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "u8", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U8), 0, 0 },
  { OUTPUT, "R_b", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_B), 0, 0 },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_tst_L_s12__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { INPUT, "s12", HW_H_SINT, CGEN_MODE_INT, OP_ENT (S12), 0, 0 },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_tst_ccu6__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "Qcondb", HW_H_QCONDB, CGEN_MODE_BI, OP_ENT (QCONDB), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U6), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_tst_L_u6__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U6), 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_tst_L_r_r__RC_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_tst_cc__RC_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "Qcondb", HW_H_QCONDB, CGEN_MODE_BI, OP_ENT (QCONDB), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_tst_s_go_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "R_b", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_B), 0, 0 },
  { INPUT, "R_c", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_C), 0, 0 },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_cmp_L_s12__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { INPUT, "s12", HW_H_SINT, CGEN_MODE_INT, OP_ENT (S12), 0, 0 },
  { OUTPUT, "cbit", HW_H_CBIT, CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_cmp_ccu6__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "Qcondb", HW_H_QCONDB, CGEN_MODE_BI, OP_ENT (QCONDB), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U6), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "cbit", HW_H_CBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_cmp_L_u6__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U6), 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "cbit", HW_H_CBIT, CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_cmp_L_r_r__RC_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "cbit", HW_H_CBIT, CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_cmp_cc__RC_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "Qcondb", HW_H_QCONDB, CGEN_MODE_BI, OP_ENT (QCONDB), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "cbit", HW_H_CBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_cmp_s_mcah_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "R_b", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_B), 0, 0 },
  { INPUT, "Rh", HW_H_CR, CGEN_MODE_SI, OP_ENT (RH), 0, 0 },
  { INPUT, "f_op_h", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_2_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "cbit", HW_H_CBIT, CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_cmp_s_r_u7_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "R_b", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_B), 0, 0 },
  { INPUT, "u7", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U7), 0, 0 },
  { OUTPUT, "cbit", HW_H_CBIT, CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_btst_s_ssb_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "R_b", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_B), 0, 0 },
  { INPUT, "u5", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U5), 0, 0 },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_mpy_L_s12__RA__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { INPUT, "s12", HW_H_SINT, CGEN_MODE_INT, OP_ENT (S12), 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_mpy_ccu6__RA__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, COND_REF },
  { INPUT, "Qcondb", HW_H_QCONDB, CGEN_MODE_BI, OP_ENT (QCONDB), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U6), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_mpy_L_u6__RA__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U6), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RA", HW_H_CR, CGEN_MODE_SI, OP_ENT (RA), 0, 0 },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_mpy_L_r_r__RA__RC_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RA", HW_H_CR, CGEN_MODE_SI, OP_ENT (RA), 0, 0 },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_mpy_cc__RA__RC_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, COND_REF },
  { INPUT, "Qcondb", HW_H_QCONDB, CGEN_MODE_BI, OP_ENT (QCONDB), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_j_L_r_r___RC_noilink__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, COND_REF },
  { INPUT, "RC_noilink", HW_H_NOILINK, CGEN_MODE_SI, OP_ENT (RC_NOILINK), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_Cj", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_auxr_SI_11", HW_H_AUXR, CGEN_MODE_SI, 0, 11, COND_REF },
  { INPUT, "h_auxr_SI_12", HW_H_AUXR, CGEN_MODE_SI, 0, 12, COND_REF },
  { INPUT, "h_auxr_SI_34", HW_H_AUXR, CGEN_MODE_SI, 0, 34, 0 },
  { INPUT, "h_auxr_SI_37", HW_H_AUXR, CGEN_MODE_SI, 0, 37, COND_REF },
  { INPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { INPUT, "h_prof_offset_SI_0", HW_H_PROF_OFFSET, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_timer_expire_SI_0", HW_H_TIMER_EXPIRE, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "h_ubit_BI", HW_H_UBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "h_auxr_SI_11", HW_H_AUXR, CGEN_MODE_SI, 0, 11, COND_REF },
  { OUTPUT, "h_auxr_SI_33", HW_H_AUXR, CGEN_MODE_SI, 0, 33, COND_REF },
  { OUTPUT, "h_cr_SI_29", HW_H_CR, CGEN_MODE_SI, 0, 29, COND_REF },
  { OUTPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { OUTPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, COND_REF },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_j_cc___RC_noilink__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, COND_REF },
  { INPUT, "Qcondb", HW_H_QCONDB, CGEN_MODE_BI, OP_ENT (QCONDB), 0, 0 },
  { INPUT, "RC_noilink", HW_H_NOILINK, CGEN_MODE_SI, OP_ENT (RC_NOILINK), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_Cj", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_auxr_SI_11", HW_H_AUXR, CGEN_MODE_SI, 0, 11, COND_REF },
  { INPUT, "h_auxr_SI_12", HW_H_AUXR, CGEN_MODE_SI, 0, 12, COND_REF },
  { INPUT, "h_auxr_SI_34", HW_H_AUXR, CGEN_MODE_SI, 0, 34, COND_REF },
  { INPUT, "h_auxr_SI_37", HW_H_AUXR, CGEN_MODE_SI, 0, 37, COND_REF },
  { INPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { INPUT, "h_prof_offset_SI_0", HW_H_PROF_OFFSET, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_timer_expire_SI_0", HW_H_TIMER_EXPIRE, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_ubit_BI", HW_H_UBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "h_auxr_SI_11", HW_H_AUXR, CGEN_MODE_SI, 0, 11, COND_REF },
  { OUTPUT, "h_auxr_SI_33", HW_H_AUXR, CGEN_MODE_SI, 0, 33, COND_REF },
  { OUTPUT, "h_cr_SI_29", HW_H_CR, CGEN_MODE_SI, 0, 29, COND_REF },
  { OUTPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { OUTPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, COND_REF },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_j_L_r_r___RC_ilink__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, COND_REF },
  { INPUT, "RC_ilink", HW_H_ILINKX, CGEN_MODE_SI, OP_ENT (RC_ILINK), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_Cj", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_auxr_SI_11", HW_H_AUXR, CGEN_MODE_SI, 0, 11, COND_REF },
  { INPUT, "h_auxr_SI_12", HW_H_AUXR, CGEN_MODE_SI, 0, 12, COND_REF },
  { INPUT, "h_auxr_SI_34", HW_H_AUXR, CGEN_MODE_SI, 0, 34, 0 },
  { INPUT, "h_auxr_SI_37", HW_H_AUXR, CGEN_MODE_SI, 0, 37, COND_REF },
  { INPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { INPUT, "h_prof_offset_SI_0", HW_H_PROF_OFFSET, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_timer_expire_SI_0", HW_H_TIMER_EXPIRE, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "h_ubit_BI", HW_H_UBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "h_auxr_SI_11", HW_H_AUXR, CGEN_MODE_SI, 0, 11, COND_REF },
  { OUTPUT, "h_auxr_SI_33", HW_H_AUXR, CGEN_MODE_SI, 0, 33, COND_REF },
  { OUTPUT, "h_cr_SI_29", HW_H_CR, CGEN_MODE_SI, 0, 29, COND_REF },
  { OUTPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { OUTPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, COND_REF },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_j_cc___RC_ilink__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, COND_REF },
  { INPUT, "Qcondb", HW_H_QCONDB, CGEN_MODE_BI, OP_ENT (QCONDB), 0, 0 },
  { INPUT, "RC_ilink", HW_H_ILINKX, CGEN_MODE_SI, OP_ENT (RC_ILINK), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_Cj", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_auxr_SI_11", HW_H_AUXR, CGEN_MODE_SI, 0, 11, COND_REF },
  { INPUT, "h_auxr_SI_12", HW_H_AUXR, CGEN_MODE_SI, 0, 12, COND_REF },
  { INPUT, "h_auxr_SI_34", HW_H_AUXR, CGEN_MODE_SI, 0, 34, COND_REF },
  { INPUT, "h_auxr_SI_37", HW_H_AUXR, CGEN_MODE_SI, 0, 37, COND_REF },
  { INPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { INPUT, "h_prof_offset_SI_0", HW_H_PROF_OFFSET, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_timer_expire_SI_0", HW_H_TIMER_EXPIRE, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_ubit_BI", HW_H_UBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "h_auxr_SI_11", HW_H_AUXR, CGEN_MODE_SI, 0, 11, COND_REF },
  { OUTPUT, "h_auxr_SI_33", HW_H_AUXR, CGEN_MODE_SI, 0, 33, COND_REF },
  { OUTPUT, "h_cr_SI_29", HW_H_CR, CGEN_MODE_SI, 0, 29, COND_REF },
  { OUTPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { OUTPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, COND_REF },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_j_L_s12__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_auxr_SI_34", HW_H_AUXR, CGEN_MODE_SI, 0, 34, 0 },
  { INPUT, "h_auxr_SI_37", HW_H_AUXR, CGEN_MODE_SI, 0, 37, COND_REF },
  { INPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { INPUT, "h_prof_offset_SI_0", HW_H_PROF_OFFSET, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_timer_expire_SI_0", HW_H_TIMER_EXPIRE, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "h_ubit_BI", HW_H_UBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { INPUT, "s12", HW_H_SINT, CGEN_MODE_INT, OP_ENT (S12), 0, COND_REF },
  { OUTPUT, "h_auxr_SI_11", HW_H_AUXR, CGEN_MODE_SI, 0, 11, COND_REF },
  { OUTPUT, "h_auxr_SI_33", HW_H_AUXR, CGEN_MODE_SI, 0, 33, COND_REF },
  { OUTPUT, "h_cr_SI_29", HW_H_CR, CGEN_MODE_SI, 0, 29, COND_REF },
  { OUTPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_j_ccu6__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, COND_REF },
  { INPUT, "Qcondb", HW_H_QCONDB, CGEN_MODE_BI, OP_ENT (QCONDB), 0, 0 },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U6), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_auxr_SI_34", HW_H_AUXR, CGEN_MODE_SI, 0, 34, COND_REF },
  { INPUT, "h_auxr_SI_37", HW_H_AUXR, CGEN_MODE_SI, 0, 37, COND_REF },
  { INPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { INPUT, "h_prof_offset_SI_0", HW_H_PROF_OFFSET, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_timer_expire_SI_0", HW_H_TIMER_EXPIRE, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_ubit_BI", HW_H_UBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "h_auxr_SI_11", HW_H_AUXR, CGEN_MODE_SI, 0, 11, COND_REF },
  { OUTPUT, "h_auxr_SI_33", HW_H_AUXR, CGEN_MODE_SI, 0, 33, COND_REF },
  { OUTPUT, "h_cr_SI_29", HW_H_CR, CGEN_MODE_SI, 0, 29, COND_REF },
  { OUTPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_j_L_u6__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, COND_REF },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U6), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_auxr_SI_34", HW_H_AUXR, CGEN_MODE_SI, 0, 34, 0 },
  { INPUT, "h_auxr_SI_37", HW_H_AUXR, CGEN_MODE_SI, 0, 37, COND_REF },
  { INPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { INPUT, "h_prof_offset_SI_0", HW_H_PROF_OFFSET, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_timer_expire_SI_0", HW_H_TIMER_EXPIRE, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "h_ubit_BI", HW_H_UBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "h_auxr_SI_11", HW_H_AUXR, CGEN_MODE_SI, 0, 11, COND_REF },
  { OUTPUT, "h_auxr_SI_33", HW_H_AUXR, CGEN_MODE_SI, 0, 33, COND_REF },
  { OUTPUT, "h_cr_SI_29", HW_H_CR, CGEN_MODE_SI, 0, 29, COND_REF },
  { OUTPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_j_s_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "R_b", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_B), 0, COND_REF },
  { INPUT, "h_auxr_SI_34", HW_H_AUXR, CGEN_MODE_SI, 0, 34, 0 },
  { INPUT, "h_auxr_SI_37", HW_H_AUXR, CGEN_MODE_SI, 0, 37, COND_REF },
  { INPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, 0 },
  { INPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { INPUT, "h_prof_offset_SI_0", HW_H_PROF_OFFSET, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_timer_expire_SI_0", HW_H_TIMER_EXPIRE, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "h_ubit_BI", HW_H_UBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "h_auxr_SI_11", HW_H_AUXR, CGEN_MODE_SI, 0, 11, COND_REF },
  { OUTPUT, "h_auxr_SI_33", HW_H_AUXR, CGEN_MODE_SI, 0, 33, COND_REF },
  { OUTPUT, "h_cr_SI_29", HW_H_CR, CGEN_MODE_SI, 0, 29, COND_REF },
  { OUTPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_j_s__S_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "R31", HW_H_R31, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_auxr_SI_34", HW_H_AUXR, CGEN_MODE_SI, 0, 34, 0 },
  { INPUT, "h_auxr_SI_37", HW_H_AUXR, CGEN_MODE_SI, 0, 37, COND_REF },
  { INPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, 0 },
  { INPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { INPUT, "h_prof_offset_SI_0", HW_H_PROF_OFFSET, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_timer_expire_SI_0", HW_H_TIMER_EXPIRE, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "h_ubit_BI", HW_H_UBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "h_auxr_SI_11", HW_H_AUXR, CGEN_MODE_SI, 0, 11, COND_REF },
  { OUTPUT, "h_auxr_SI_33", HW_H_AUXR, CGEN_MODE_SI, 0, 33, COND_REF },
  { OUTPUT, "h_cr_SI_29", HW_H_CR, CGEN_MODE_SI, 0, 29, COND_REF },
  { OUTPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_j_seq__S_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "R31", HW_H_R31, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_auxr_SI_34", HW_H_AUXR, CGEN_MODE_SI, 0, 34, 0 },
  { INPUT, "h_auxr_SI_37", HW_H_AUXR, CGEN_MODE_SI, 0, 37, COND_REF },
  { INPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, 0 },
  { INPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { INPUT, "h_prof_offset_SI_0", HW_H_PROF_OFFSET, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_timer_expire_SI_0", HW_H_TIMER_EXPIRE, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "h_ubit_BI", HW_H_UBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { INPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "h_auxr_SI_11", HW_H_AUXR, CGEN_MODE_SI, 0, 11, COND_REF },
  { OUTPUT, "h_auxr_SI_33", HW_H_AUXR, CGEN_MODE_SI, 0, 33, COND_REF },
  { OUTPUT, "h_cr_SI_29", HW_H_CR, CGEN_MODE_SI, 0, 29, COND_REF },
  { OUTPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_j_L_s12_d__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_auxr_SI_34", HW_H_AUXR, CGEN_MODE_SI, 0, 34, 0 },
  { INPUT, "h_auxr_SI_37", HW_H_AUXR, CGEN_MODE_SI, 0, 37, COND_REF },
  { INPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { INPUT, "h_prof_offset_SI_0", HW_H_PROF_OFFSET, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_timer_expire_SI_0", HW_H_TIMER_EXPIRE, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "h_ubit_BI", HW_H_UBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { INPUT, "s12", HW_H_SINT, CGEN_MODE_INT, OP_ENT (S12), 0, COND_REF },
  { OUTPUT, "h_auxr_SI_11", HW_H_AUXR, CGEN_MODE_SI, 0, 11, COND_REF },
  { OUTPUT, "h_auxr_SI_33", HW_H_AUXR, CGEN_MODE_SI, 0, 33, COND_REF },
  { OUTPUT, "h_cr_SI_29", HW_H_CR, CGEN_MODE_SI, 0, 29, COND_REF },
  { OUTPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_j_ccu6_d__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "Qcondb", HW_H_QCONDB, CGEN_MODE_BI, OP_ENT (QCONDB), 0, 0 },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U6), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_auxr_SI_34", HW_H_AUXR, CGEN_MODE_SI, 0, 34, COND_REF },
  { INPUT, "h_auxr_SI_37", HW_H_AUXR, CGEN_MODE_SI, 0, 37, COND_REF },
  { INPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { INPUT, "h_prof_offset_SI_0", HW_H_PROF_OFFSET, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_timer_expire_SI_0", HW_H_TIMER_EXPIRE, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_ubit_BI", HW_H_UBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "h_auxr_SI_11", HW_H_AUXR, CGEN_MODE_SI, 0, 11, COND_REF },
  { OUTPUT, "h_auxr_SI_33", HW_H_AUXR, CGEN_MODE_SI, 0, 33, COND_REF },
  { OUTPUT, "h_cr_SI_29", HW_H_CR, CGEN_MODE_SI, 0, 29, COND_REF },
  { OUTPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_j_L_u6_d__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U6), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_auxr_SI_34", HW_H_AUXR, CGEN_MODE_SI, 0, 34, 0 },
  { INPUT, "h_auxr_SI_37", HW_H_AUXR, CGEN_MODE_SI, 0, 37, COND_REF },
  { INPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { INPUT, "h_prof_offset_SI_0", HW_H_PROF_OFFSET, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_timer_expire_SI_0", HW_H_TIMER_EXPIRE, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "h_ubit_BI", HW_H_UBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "h_auxr_SI_11", HW_H_AUXR, CGEN_MODE_SI, 0, 11, COND_REF },
  { OUTPUT, "h_auxr_SI_33", HW_H_AUXR, CGEN_MODE_SI, 0, 33, COND_REF },
  { OUTPUT, "h_cr_SI_29", HW_H_CR, CGEN_MODE_SI, 0, 29, COND_REF },
  { OUTPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_j_L_r_r_d___RC__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_auxr_SI_34", HW_H_AUXR, CGEN_MODE_SI, 0, 34, 0 },
  { INPUT, "h_auxr_SI_37", HW_H_AUXR, CGEN_MODE_SI, 0, 37, COND_REF },
  { INPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { INPUT, "h_prof_offset_SI_0", HW_H_PROF_OFFSET, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_timer_expire_SI_0", HW_H_TIMER_EXPIRE, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "h_ubit_BI", HW_H_UBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "h_auxr_SI_11", HW_H_AUXR, CGEN_MODE_SI, 0, 11, COND_REF },
  { OUTPUT, "h_auxr_SI_33", HW_H_AUXR, CGEN_MODE_SI, 0, 33, COND_REF },
  { OUTPUT, "h_cr_SI_29", HW_H_CR, CGEN_MODE_SI, 0, 29, COND_REF },
  { OUTPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_j_cc_d___RC__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "Qcondb", HW_H_QCONDB, CGEN_MODE_BI, OP_ENT (QCONDB), 0, 0 },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_auxr_SI_34", HW_H_AUXR, CGEN_MODE_SI, 0, 34, COND_REF },
  { INPUT, "h_auxr_SI_37", HW_H_AUXR, CGEN_MODE_SI, 0, 37, COND_REF },
  { INPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { INPUT, "h_prof_offset_SI_0", HW_H_PROF_OFFSET, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_timer_expire_SI_0", HW_H_TIMER_EXPIRE, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_ubit_BI", HW_H_UBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "h_auxr_SI_11", HW_H_AUXR, CGEN_MODE_SI, 0, 11, COND_REF },
  { OUTPUT, "h_auxr_SI_33", HW_H_AUXR, CGEN_MODE_SI, 0, 33, COND_REF },
  { OUTPUT, "h_cr_SI_29", HW_H_CR, CGEN_MODE_SI, 0, 29, COND_REF },
  { OUTPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_jl_L_s12__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_auxr_SI_34", HW_H_AUXR, CGEN_MODE_SI, 0, 34, 0 },
  { INPUT, "h_auxr_SI_37", HW_H_AUXR, CGEN_MODE_SI, 0, 37, COND_REF },
  { INPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { INPUT, "h_prof_offset_SI_0", HW_H_PROF_OFFSET, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_timer_expire_SI_0", HW_H_TIMER_EXPIRE, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "h_ubit_BI", HW_H_UBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { INPUT, "s12", HW_H_SINT, CGEN_MODE_INT, OP_ENT (S12), 0, COND_REF },
  { OUTPUT, "h_auxr_SI_11", HW_H_AUXR, CGEN_MODE_SI, 0, 11, COND_REF },
  { OUTPUT, "h_auxr_SI_33", HW_H_AUXR, CGEN_MODE_SI, 0, 33, COND_REF },
  { OUTPUT, "h_cr_SI_29", HW_H_CR, CGEN_MODE_SI, 0, 29, COND_REF },
  { OUTPUT, "h_cr_SI_31", HW_H_CR, CGEN_MODE_SI, 0, 31, COND_REF },
  { OUTPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_jl_ccu6__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "Qcondb", HW_H_QCONDB, CGEN_MODE_BI, OP_ENT (QCONDB), 0, 0 },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U6), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_auxr_SI_34", HW_H_AUXR, CGEN_MODE_SI, 0, 34, COND_REF },
  { INPUT, "h_auxr_SI_37", HW_H_AUXR, CGEN_MODE_SI, 0, 37, COND_REF },
  { INPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { INPUT, "h_prof_offset_SI_0", HW_H_PROF_OFFSET, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_timer_expire_SI_0", HW_H_TIMER_EXPIRE, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_ubit_BI", HW_H_UBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "h_auxr_SI_11", HW_H_AUXR, CGEN_MODE_SI, 0, 11, COND_REF },
  { OUTPUT, "h_auxr_SI_33", HW_H_AUXR, CGEN_MODE_SI, 0, 33, COND_REF },
  { OUTPUT, "h_cr_SI_29", HW_H_CR, CGEN_MODE_SI, 0, 29, COND_REF },
  { OUTPUT, "h_cr_SI_31", HW_H_CR, CGEN_MODE_SI, 0, 31, COND_REF },
  { OUTPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_jl_L_u6__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U6), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_auxr_SI_34", HW_H_AUXR, CGEN_MODE_SI, 0, 34, 0 },
  { INPUT, "h_auxr_SI_37", HW_H_AUXR, CGEN_MODE_SI, 0, 37, COND_REF },
  { INPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { INPUT, "h_prof_offset_SI_0", HW_H_PROF_OFFSET, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_timer_expire_SI_0", HW_H_TIMER_EXPIRE, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "h_ubit_BI", HW_H_UBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "h_auxr_SI_11", HW_H_AUXR, CGEN_MODE_SI, 0, 11, COND_REF },
  { OUTPUT, "h_auxr_SI_33", HW_H_AUXR, CGEN_MODE_SI, 0, 33, COND_REF },
  { OUTPUT, "h_cr_SI_29", HW_H_CR, CGEN_MODE_SI, 0, 29, COND_REF },
  { OUTPUT, "h_cr_SI_31", HW_H_CR, CGEN_MODE_SI, 0, 31, COND_REF },
  { OUTPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_jl_s_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "R_b", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_B), 0, COND_REF },
  { INPUT, "h_auxr_SI_34", HW_H_AUXR, CGEN_MODE_SI, 0, 34, 0 },
  { INPUT, "h_auxr_SI_37", HW_H_AUXR, CGEN_MODE_SI, 0, 37, COND_REF },
  { INPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, 0 },
  { INPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { INPUT, "h_prof_offset_SI_0", HW_H_PROF_OFFSET, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_timer_expire_SI_0", HW_H_TIMER_EXPIRE, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "h_ubit_BI", HW_H_UBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "h_auxr_SI_11", HW_H_AUXR, CGEN_MODE_SI, 0, 11, COND_REF },
  { OUTPUT, "h_auxr_SI_33", HW_H_AUXR, CGEN_MODE_SI, 0, 33, COND_REF },
  { OUTPUT, "h_cr_SI_29", HW_H_CR, CGEN_MODE_SI, 0, 29, COND_REF },
  { OUTPUT, "h_cr_SI_31", HW_H_CR, CGEN_MODE_SI, 0, 31, COND_REF },
  { OUTPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_jl_L_r_r___RC_noilink__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "RC_noilink", HW_H_NOILINK, CGEN_MODE_SI, OP_ENT (RC_NOILINK), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_auxr_SI_34", HW_H_AUXR, CGEN_MODE_SI, 0, 34, 0 },
  { INPUT, "h_auxr_SI_37", HW_H_AUXR, CGEN_MODE_SI, 0, 37, COND_REF },
  { INPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { INPUT, "h_prof_offset_SI_0", HW_H_PROF_OFFSET, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_timer_expire_SI_0", HW_H_TIMER_EXPIRE, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "h_ubit_BI", HW_H_UBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "h_auxr_SI_11", HW_H_AUXR, CGEN_MODE_SI, 0, 11, COND_REF },
  { OUTPUT, "h_auxr_SI_33", HW_H_AUXR, CGEN_MODE_SI, 0, 33, COND_REF },
  { OUTPUT, "h_cr_SI_29", HW_H_CR, CGEN_MODE_SI, 0, 29, COND_REF },
  { OUTPUT, "h_cr_SI_31", HW_H_CR, CGEN_MODE_SI, 0, 31, COND_REF },
  { OUTPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_jl_cc___RC_noilink__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "Qcondb", HW_H_QCONDB, CGEN_MODE_BI, OP_ENT (QCONDB), 0, 0 },
  { INPUT, "RC_noilink", HW_H_NOILINK, CGEN_MODE_SI, OP_ENT (RC_NOILINK), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_auxr_SI_34", HW_H_AUXR, CGEN_MODE_SI, 0, 34, COND_REF },
  { INPUT, "h_auxr_SI_37", HW_H_AUXR, CGEN_MODE_SI, 0, 37, COND_REF },
  { INPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { INPUT, "h_prof_offset_SI_0", HW_H_PROF_OFFSET, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_timer_expire_SI_0", HW_H_TIMER_EXPIRE, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_ubit_BI", HW_H_UBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "h_auxr_SI_11", HW_H_AUXR, CGEN_MODE_SI, 0, 11, COND_REF },
  { OUTPUT, "h_auxr_SI_33", HW_H_AUXR, CGEN_MODE_SI, 0, 33, COND_REF },
  { OUTPUT, "h_cr_SI_29", HW_H_CR, CGEN_MODE_SI, 0, 29, COND_REF },
  { OUTPUT, "h_cr_SI_31", HW_H_CR, CGEN_MODE_SI, 0, 31, COND_REF },
  { OUTPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_jl_L_r_r_d___RC__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_auxr_SI_34", HW_H_AUXR, CGEN_MODE_SI, 0, 34, 0 },
  { INPUT, "h_auxr_SI_37", HW_H_AUXR, CGEN_MODE_SI, 0, 37, COND_REF },
  { INPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { INPUT, "h_prof_offset_SI_0", HW_H_PROF_OFFSET, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_timer_expire_SI_0", HW_H_TIMER_EXPIRE, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "h_ubit_BI", HW_H_UBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "h_auxr_SI_11", HW_H_AUXR, CGEN_MODE_SI, 0, 11, COND_REF },
  { OUTPUT, "h_auxr_SI_33", HW_H_AUXR, CGEN_MODE_SI, 0, 33, COND_REF },
  { OUTPUT, "h_cr_SI_29", HW_H_CR, CGEN_MODE_SI, 0, 29, COND_REF },
  { OUTPUT, "h_cr_SI_31", HW_H_CR, CGEN_MODE_SI, 0, 31, COND_REF },
  { OUTPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_jl_cc_d___RC__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "Qcondb", HW_H_QCONDB, CGEN_MODE_BI, OP_ENT (QCONDB), 0, 0 },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_auxr_SI_34", HW_H_AUXR, CGEN_MODE_SI, 0, 34, COND_REF },
  { INPUT, "h_auxr_SI_37", HW_H_AUXR, CGEN_MODE_SI, 0, 37, COND_REF },
  { INPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { INPUT, "h_prof_offset_SI_0", HW_H_PROF_OFFSET, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_timer_expire_SI_0", HW_H_TIMER_EXPIRE, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_ubit_BI", HW_H_UBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "h_auxr_SI_11", HW_H_AUXR, CGEN_MODE_SI, 0, 11, COND_REF },
  { OUTPUT, "h_auxr_SI_33", HW_H_AUXR, CGEN_MODE_SI, 0, 33, COND_REF },
  { OUTPUT, "h_cr_SI_29", HW_H_CR, CGEN_MODE_SI, 0, 29, COND_REF },
  { OUTPUT, "h_cr_SI_31", HW_H_CR, CGEN_MODE_SI, 0, 31, COND_REF },
  { OUTPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_jl_s_d_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "R_b", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_B), 0, COND_REF },
  { INPUT, "h_auxr_SI_34", HW_H_AUXR, CGEN_MODE_SI, 0, 34, 0 },
  { INPUT, "h_auxr_SI_37", HW_H_AUXR, CGEN_MODE_SI, 0, 37, COND_REF },
  { INPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { INPUT, "h_prof_offset_SI_0", HW_H_PROF_OFFSET, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_timer_expire_SI_0", HW_H_TIMER_EXPIRE, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "h_ubit_BI", HW_H_UBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "h_auxr_SI_11", HW_H_AUXR, CGEN_MODE_SI, 0, 11, COND_REF },
  { OUTPUT, "h_auxr_SI_33", HW_H_AUXR, CGEN_MODE_SI, 0, 33, COND_REF },
  { OUTPUT, "h_cr_SI_29", HW_H_CR, CGEN_MODE_SI, 0, 29, COND_REF },
  { OUTPUT, "h_cr_SI_31", HW_H_CR, CGEN_MODE_SI, 0, 31, COND_REF },
  { OUTPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_lp_L_s12__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_auxr_SI_34", HW_H_AUXR, CGEN_MODE_SI, 0, 34, 0 },
  { INPUT, "h_auxr_SI_37", HW_H_AUXR, CGEN_MODE_SI, 0, 37, COND_REF },
  { INPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { INPUT, "h_prof_offset_SI_0", HW_H_PROF_OFFSET, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_timer_expire_SI_0", HW_H_TIMER_EXPIRE, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "h_ubit_BI", HW_H_UBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { INPUT, "s12x2", HW_H_SINT, CGEN_MODE_INT, OP_ENT (S12X2), 0, COND_REF },
  { OUTPUT, "h_auxr_SI_11", HW_H_AUXR, CGEN_MODE_SI, 0, 11, COND_REF },
  { OUTPUT, "h_auxr_SI_2", HW_H_AUXR, CGEN_MODE_SI, 0, 2, COND_REF },
  { OUTPUT, "h_auxr_SI_3", HW_H_AUXR, CGEN_MODE_SI, 0, 3, COND_REF },
  { OUTPUT, "h_auxr_SI_33", HW_H_AUXR, CGEN_MODE_SI, 0, 33, COND_REF },
  { OUTPUT, "h_cr_SI_29", HW_H_CR, CGEN_MODE_SI, 0, 29, COND_REF },
  { OUTPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_lpcc_ccu6_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "Qcondb", HW_H_QCONDB, CGEN_MODE_BI, OP_ENT (QCONDB), 0, 0 },
  { INPUT, "U6x2", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U6X2), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_auxr_SI_34", HW_H_AUXR, CGEN_MODE_SI, 0, 34, COND_REF },
  { INPUT, "h_auxr_SI_37", HW_H_AUXR, CGEN_MODE_SI, 0, 37, COND_REF },
  { INPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { INPUT, "h_prof_offset_SI_0", HW_H_PROF_OFFSET, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_timer_expire_SI_0", HW_H_TIMER_EXPIRE, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_ubit_BI", HW_H_UBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "h_auxr_SI_11", HW_H_AUXR, CGEN_MODE_SI, 0, 11, COND_REF },
  { OUTPUT, "h_auxr_SI_2", HW_H_AUXR, CGEN_MODE_SI, 0, 2, COND_REF },
  { OUTPUT, "h_auxr_SI_3", HW_H_AUXR, CGEN_MODE_SI, 0, 3, COND_REF },
  { OUTPUT, "h_auxr_SI_33", HW_H_AUXR, CGEN_MODE_SI, 0, 33, COND_REF },
  { OUTPUT, "h_cr_SI_29", HW_H_CR, CGEN_MODE_SI, 0, 29, COND_REF },
  { OUTPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_flag_L_s12__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { INPUT, "s12", HW_H_SINT, CGEN_MODE_INT, OP_ENT (S12), 0, 0 },
  { OUTPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_flag_ccu6__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "Qcondb", HW_H_QCONDB, CGEN_MODE_BI, OP_ENT (QCONDB), 0, 0 },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U6), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_flag_L_u6__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U6), 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_flag_L_r_r__RC_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_flag_cc__RC_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "Qcondb", HW_H_QCONDB, CGEN_MODE_BI, OP_ENT (QCONDB), 0, 0 },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_lr_L_r_r___RC__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "RC", HW_H_CR, CGEN_MODE_INT, OP_ENT (RC), 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_auxr_SI_RC", HW_H_AUXR, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_lr_L_s12__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_auxr_SI_s12", HW_H_AUXR, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { INPUT, "s12", HW_H_SINT, CGEN_MODE_INT, OP_ENT (S12), 0, 0 },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_lr_L_u6__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_INT, OP_ENT (U6), 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_auxr_SI_U6", HW_H_AUXR, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_sr_L_r_r___RC__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_INT, OP_ENT (RC), 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "h_auxr_SI_RC", HW_H_AUXR, CGEN_MODE_SI, 0, 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_sr_L_s12__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { INPUT, "s12", HW_H_SINT, CGEN_MODE_INT, OP_ENT (S12), 0, 0 },
  { OUTPUT, "h_auxr_SI_s12", HW_H_AUXR, CGEN_MODE_SI, 0, 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_sr_L_u6__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_INT, OP_ENT (U6), 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "h_auxr_SI_U6", HW_H_AUXR, CGEN_MODE_SI, 0, 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_asl_L_r_r__RC_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { OUTPUT, "cbit", HW_H_CBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_asl_L_u6__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U6), 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { OUTPUT, "cbit", HW_H_CBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_asr_L_r_r__RC_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { OUTPUT, "cbit", HW_H_CBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_asr_L_u6__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U6), 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { OUTPUT, "cbit", HW_H_CBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_rrc_L_r_r__RC_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, 0 },
  { INPUT, "cbit", HW_H_CBIT, CGEN_MODE_BI, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { OUTPUT, "cbit", HW_H_CBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_rrc_L_u6__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U6), 0, 0 },
  { INPUT, "cbit", HW_H_CBIT, CGEN_MODE_BI, 0, 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { OUTPUT, "cbit", HW_H_CBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_sexb_L_r_r__RC_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_QI, OP_ENT (RC), 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_sexb_L_u6__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_QI, OP_ENT (U6), 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_sexw_L_r_r__RC_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_HI, OP_ENT (RC), 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_sexw_L_u6__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_HI, OP_ENT (U6), 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_abs_L_r_r__RC_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { INPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { OUTPUT, "cbit", HW_H_CBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_abs_L_u6__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U6), 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { INPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { OUTPUT, "cbit", HW_H_CBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_not_L_r_r__RC_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_not_L_u6__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U6), 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_ex_L_r_r__RC_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_USI, OP_ENT (RC), 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_SI_RC", HW_H_MEMORY, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { OUTPUT, "h_memory_SI_RC", HW_H_MEMORY, CGEN_MODE_SI, 0, 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_ex_L_u6__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_USI, OP_ENT (U6), 0, 0 },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_SI_U6", HW_H_MEMORY, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { OUTPUT, "h_memory_SI_U6", HW_H_MEMORY, CGEN_MODE_SI, 0, 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_swi_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "h_auxr_SI_34", HW_H_AUXR, CGEN_MODE_SI, 0, 34, 0 },
  { INPUT, "h_auxr_SI_37", HW_H_AUXR, CGEN_MODE_SI, 0, 37, COND_REF },
  { INPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, 0 },
  { INPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { INPUT, "h_prof_offset_SI_0", HW_H_PROF_OFFSET, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_timer_expire_SI_0", HW_H_TIMER_EXPIRE, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "h_ubit_BI", HW_H_UBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "h_auxr_SI_11", HW_H_AUXR, CGEN_MODE_SI, 0, 11, COND_REF },
  { OUTPUT, "h_auxr_SI_33", HW_H_AUXR, CGEN_MODE_SI, 0, 33, COND_REF },
  { OUTPUT, "h_cr_SI_0", HW_H_CR, CGEN_MODE_SI, 0, 0, COND_REF },
  { OUTPUT, "h_cr_SI_29", HW_H_CR, CGEN_MODE_SI, 0, 29, COND_REF },
  { OUTPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_trap_s_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "h_auxr_SI_34", HW_H_AUXR, CGEN_MODE_SI, 0, 34, 0 },
  { INPUT, "h_auxr_SI_37", HW_H_AUXR, CGEN_MODE_SI, 0, 37, COND_REF },
  { INPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, 0 },
  { INPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { INPUT, "h_prof_offset_SI_0", HW_H_PROF_OFFSET, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_timer_expire_SI_0", HW_H_TIMER_EXPIRE, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "h_ubit_BI", HW_H_UBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { INPUT, "trapnum", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (TRAPNUM), 0, COND_REF },
  { OUTPUT, "h_auxr_SI_11", HW_H_AUXR, CGEN_MODE_SI, 0, 11, COND_REF },
  { OUTPUT, "h_auxr_SI_33", HW_H_AUXR, CGEN_MODE_SI, 0, 33, COND_REF },
  { OUTPUT, "h_cr_SI_0", HW_H_CR, CGEN_MODE_SI, 0, 0, COND_REF },
  { OUTPUT, "h_cr_SI_29", HW_H_CR, CGEN_MODE_SI, 0, 29, COND_REF },
  { OUTPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_brk_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_asl_L_s12__RA__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { INPUT, "s12", HW_H_SINT, CGEN_MODE_INT, OP_ENT (S12), 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { OUTPUT, "cbit", HW_H_CBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_asl_ccu6__RA__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, COND_REF },
  { INPUT, "Qcondb", HW_H_QCONDB, CGEN_MODE_BI, OP_ENT (QCONDB), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U6), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { OUTPUT, "cbit", HW_H_CBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_asl_L_u6__RA__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U6), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RA", HW_H_CR, CGEN_MODE_SI, OP_ENT (RA), 0, 0 },
  { OUTPUT, "cbit", HW_H_CBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_asl_L_r_r__RA__RC_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RA", HW_H_CR, CGEN_MODE_SI, OP_ENT (RA), 0, 0 },
  { OUTPUT, "cbit", HW_H_CBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_asl_cc__RA__RC_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, COND_REF },
  { INPUT, "Qcondb", HW_H_QCONDB, CGEN_MODE_BI, OP_ENT (QCONDB), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { OUTPUT, "cbit", HW_H_CBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_mul64_L_s12__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { INPUT, "s12", HW_H_SINT, CGEN_MODE_INT, OP_ENT (S12), 0, COND_REF },
  { OUTPUT, "h_cr_SI_57", HW_H_CR, CGEN_MODE_SI, 0, 57, COND_REF },
  { OUTPUT, "h_cr_SI_58", HW_H_CR, CGEN_MODE_SI, 0, 58, COND_REF },
  { OUTPUT, "h_cr_SI_59", HW_H_CR, CGEN_MODE_SI, 0, 59, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_mul64_ccu6__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "Qcondb", HW_H_QCONDB, CGEN_MODE_BI, OP_ENT (QCONDB), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U6), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "h_cr_SI_57", HW_H_CR, CGEN_MODE_SI, 0, 57, COND_REF },
  { OUTPUT, "h_cr_SI_58", HW_H_CR, CGEN_MODE_SI, 0, 58, COND_REF },
  { OUTPUT, "h_cr_SI_59", HW_H_CR, CGEN_MODE_SI, 0, 59, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_mul64_L_u6__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U6), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "h_cr_SI_57", HW_H_CR, CGEN_MODE_SI, 0, 57, COND_REF },
  { OUTPUT, "h_cr_SI_58", HW_H_CR, CGEN_MODE_SI, 0, 58, COND_REF },
  { OUTPUT, "h_cr_SI_59", HW_H_CR, CGEN_MODE_SI, 0, 59, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_mul64_L_r_r__RC_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "h_cr_SI_57", HW_H_CR, CGEN_MODE_SI, 0, 57, COND_REF },
  { OUTPUT, "h_cr_SI_58", HW_H_CR, CGEN_MODE_SI, 0, 58, COND_REF },
  { OUTPUT, "h_cr_SI_59", HW_H_CR, CGEN_MODE_SI, 0, 59, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_mul64_cc__RC_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "Qcondb", HW_H_QCONDB, CGEN_MODE_BI, OP_ENT (QCONDB), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "h_cr_SI_57", HW_H_CR, CGEN_MODE_SI, 0, 57, COND_REF },
  { OUTPUT, "h_cr_SI_58", HW_H_CR, CGEN_MODE_SI, 0, 58, COND_REF },
  { OUTPUT, "h_cr_SI_59", HW_H_CR, CGEN_MODE_SI, 0, 59, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_mul64_s_go_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "R_b", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_B), 0, COND_REF },
  { INPUT, "R_c", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_C), 0, COND_REF },
  { OUTPUT, "h_cr_SI_57", HW_H_CR, CGEN_MODE_SI, 0, 57, COND_REF },
  { OUTPUT, "h_cr_SI_58", HW_H_CR, CGEN_MODE_SI, 0, 58, COND_REF },
  { OUTPUT, "h_cr_SI_59", HW_H_CR, CGEN_MODE_SI, 0, 59, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_adds_L_s12__RA__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { INPUT, "s12", HW_H_SINT, CGEN_MODE_INT, OP_ENT (S12), 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { OUTPUT, "cbit", HW_H_CBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s1bit", HW_H_S1BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s2bit", HW_H_S2BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_adds_ccu6__RA__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, COND_REF },
  { INPUT, "Qcondb", HW_H_QCONDB, CGEN_MODE_BI, OP_ENT (QCONDB), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U6), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { OUTPUT, "cbit", HW_H_CBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s1bit", HW_H_S1BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s2bit", HW_H_S2BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_adds_L_u6__RA__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U6), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RA", HW_H_CR, CGEN_MODE_SI, OP_ENT (RA), 0, 0 },
  { OUTPUT, "cbit", HW_H_CBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s1bit", HW_H_S1BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s2bit", HW_H_S2BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_adds_L_r_r__RA__RC_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RA", HW_H_CR, CGEN_MODE_SI, OP_ENT (RA), 0, 0 },
  { OUTPUT, "cbit", HW_H_CBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s1bit", HW_H_S1BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s2bit", HW_H_S2BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_adds_cc__RA__RC_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, COND_REF },
  { INPUT, "Qcondb", HW_H_QCONDB, CGEN_MODE_BI, OP_ENT (QCONDB), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { OUTPUT, "cbit", HW_H_CBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s1bit", HW_H_S1BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s2bit", HW_H_S2BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_divaw_L_s12__RA__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { INPUT, "s12", HW_H_SINT, CGEN_MODE_SI, OP_ENT (S12), 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_divaw_ccu6__RA__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, COND_REF },
  { INPUT, "Qcondb", HW_H_QCONDB, CGEN_MODE_BI, OP_ENT (QCONDB), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_SI, OP_ENT (U6), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_divaw_L_u6__RA__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_SI, OP_ENT (U6), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RA", HW_H_CR, CGEN_MODE_SI, OP_ENT (RA), 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_divaw_L_r_r__RA__RC_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RA", HW_H_CR, CGEN_MODE_SI, OP_ENT (RA), 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_divaw_cc__RA__RC_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, COND_REF },
  { INPUT, "Qcondb", HW_H_QCONDB, CGEN_MODE_BI, OP_ENT (QCONDB), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_asls_L_s12__RA__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { INPUT, "s12", HW_H_SINT, CGEN_MODE_SI, OP_ENT (S12), 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s1bit", HW_H_S1BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s2bit", HW_H_S2BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_asls_ccu6__RA__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, COND_REF },
  { INPUT, "Qcondb", HW_H_QCONDB, CGEN_MODE_BI, OP_ENT (QCONDB), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_SI, OP_ENT (U6), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s1bit", HW_H_S1BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s2bit", HW_H_S2BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_asls_L_u6__RA__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_SI, OP_ENT (U6), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RA", HW_H_CR, CGEN_MODE_SI, OP_ENT (RA), 0, 0 },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s1bit", HW_H_S1BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s2bit", HW_H_S2BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_asls_L_r_r__RA__RC_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RA", HW_H_CR, CGEN_MODE_SI, OP_ENT (RA), 0, 0 },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s1bit", HW_H_S1BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s2bit", HW_H_S2BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_asls_cc__RA__RC_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, COND_REF },
  { INPUT, "Qcondb", HW_H_QCONDB, CGEN_MODE_BI, OP_ENT (QCONDB), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s1bit", HW_H_S1BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s2bit", HW_H_S2BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_swap_L_r_r__RC_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, COND_REF },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_swap_L_u6__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U6), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_norm_L_u6__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_SI, OP_ENT (U6), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_rnd16_L_r_r__RC_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, COND_REF },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s1bit", HW_H_S1BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s2bit", HW_H_S2BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_rnd16_L_u6__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U6), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s1bit", HW_H_S1BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s2bit", HW_H_S2BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_abssw_L_r_r__RC_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_HI, OP_ENT (RC), 0, COND_REF },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s1bit", HW_H_S1BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s2bit", HW_H_S2BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_abssw_L_u6__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_HI, OP_ENT (U6), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s1bit", HW_H_S1BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s2bit", HW_H_S2BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_abss_L_u6__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_SI, OP_ENT (U6), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s1bit", HW_H_S1BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s2bit", HW_H_S2BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_nop_s_ops[] ATTRIBUTE_UNUSED = {
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_pop_s_b_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "SP", HW_H_SP, CGEN_MODE_USI, 0, 0, 0 },
  { INPUT, "h_memory_SI_SP", HW_H_MEMORY, CGEN_MODE_SI, 0, 0, 0 },
  { OUTPUT, "R_b", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_B), 0, 0 },
  { OUTPUT, "SP", HW_H_SP, CGEN_MODE_SI, 0, 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_pop_s_blink_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "SP", HW_H_SP, CGEN_MODE_USI, 0, 0, 0 },
  { INPUT, "h_memory_SI_SP", HW_H_MEMORY, CGEN_MODE_SI, 0, 0, 0 },
  { OUTPUT, "R31", HW_H_R31, CGEN_MODE_SI, 0, 0, 0 },
  { OUTPUT, "SP", HW_H_SP, CGEN_MODE_SI, 0, 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_push_s_b_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "R_b", HW_H_CR16, CGEN_MODE_SI, OP_ENT (R_B), 0, 0 },
  { INPUT, "SP", HW_H_SP, CGEN_MODE_SI, 0, 0, 0 },
  { OUTPUT, "SP", HW_H_SP, CGEN_MODE_SI, 0, 0, 0 },
  { OUTPUT, "h_memory_SI_SP", HW_H_MEMORY, CGEN_MODE_SI, 0, 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_push_s_blink_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "R31", HW_H_R31, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "SP", HW_H_SP, CGEN_MODE_SI, 0, 0, 0 },
  { OUTPUT, "SP", HW_H_SP, CGEN_MODE_SI, 0, 0, 0 },
  { OUTPUT, "h_memory_SI_SP", HW_H_MEMORY, CGEN_MODE_SI, 0, 0, 0 },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_mullw_L_s12__RA__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { INPUT, "s12", HW_H_SINT, CGEN_MODE_INT, OP_ENT (S12), 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { OUTPUT, "h_cr_SI_56", HW_H_CR, CGEN_MODE_SI, 0, 56, 0 },
  { OUTPUT, "h_cr_SI_57", HW_H_CR, CGEN_MODE_SI, 0, 57, 0 },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s1bit", HW_H_S1BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s2bit", HW_H_S2BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_mullw_ccu6__RA__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, COND_REF },
  { INPUT, "Qcondb", HW_H_QCONDB, CGEN_MODE_BI, OP_ENT (QCONDB), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U6), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { OUTPUT, "h_cr_SI_56", HW_H_CR, CGEN_MODE_SI, 0, 56, COND_REF },
  { OUTPUT, "h_cr_SI_57", HW_H_CR, CGEN_MODE_SI, 0, 57, COND_REF },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s1bit", HW_H_S1BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s2bit", HW_H_S2BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_mullw_L_u6__RA__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U6), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RA", HW_H_CR, CGEN_MODE_SI, OP_ENT (RA), 0, 0 },
  { OUTPUT, "h_cr_SI_56", HW_H_CR, CGEN_MODE_SI, 0, 56, 0 },
  { OUTPUT, "h_cr_SI_57", HW_H_CR, CGEN_MODE_SI, 0, 57, 0 },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s1bit", HW_H_S1BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s2bit", HW_H_S2BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_mullw_L_r_r__RA__RC_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RA", HW_H_CR, CGEN_MODE_SI, OP_ENT (RA), 0, 0 },
  { OUTPUT, "h_cr_SI_56", HW_H_CR, CGEN_MODE_SI, 0, 56, 0 },
  { OUTPUT, "h_cr_SI_57", HW_H_CR, CGEN_MODE_SI, 0, 57, 0 },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s1bit", HW_H_S1BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s2bit", HW_H_S2BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_mullw_cc__RA__RC_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, COND_REF },
  { INPUT, "Qcondb", HW_H_QCONDB, CGEN_MODE_BI, OP_ENT (QCONDB), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { OUTPUT, "h_cr_SI_56", HW_H_CR, CGEN_MODE_SI, 0, 56, COND_REF },
  { OUTPUT, "h_cr_SI_57", HW_H_CR, CGEN_MODE_SI, 0, 57, COND_REF },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s1bit", HW_H_S1BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s2bit", HW_H_S2BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_maclw_L_s12__RA__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_cr_SI_56", HW_H_CR, CGEN_MODE_SI, 0, 56, 0 },
  { INPUT, "h_cr_SI_57", HW_H_CR, CGEN_MODE_SI, 0, 57, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { INPUT, "s12", HW_H_SINT, CGEN_MODE_INT, OP_ENT (S12), 0, COND_REF },
  { INPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { OUTPUT, "h_cr_SI_56", HW_H_CR, CGEN_MODE_SI, 0, 56, 0 },
  { OUTPUT, "h_cr_SI_57", HW_H_CR, CGEN_MODE_SI, 0, 57, 0 },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s1bit", HW_H_S1BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s2bit", HW_H_S2BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_maclw_ccu6__RA__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, COND_REF },
  { INPUT, "Qcondb", HW_H_QCONDB, CGEN_MODE_BI, OP_ENT (QCONDB), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U6), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_cr_SI_56", HW_H_CR, CGEN_MODE_SI, 0, 56, COND_REF },
  { INPUT, "h_cr_SI_57", HW_H_CR, CGEN_MODE_SI, 0, 57, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { INPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { OUTPUT, "h_cr_SI_56", HW_H_CR, CGEN_MODE_SI, 0, 56, COND_REF },
  { OUTPUT, "h_cr_SI_57", HW_H_CR, CGEN_MODE_SI, 0, 57, COND_REF },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s1bit", HW_H_S1BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s2bit", HW_H_S2BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_maclw_L_u6__RA__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U6), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_cr_SI_56", HW_H_CR, CGEN_MODE_SI, 0, 56, 0 },
  { INPUT, "h_cr_SI_57", HW_H_CR, CGEN_MODE_SI, 0, 57, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { INPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "RA", HW_H_CR, CGEN_MODE_SI, OP_ENT (RA), 0, 0 },
  { OUTPUT, "h_cr_SI_56", HW_H_CR, CGEN_MODE_SI, 0, 56, 0 },
  { OUTPUT, "h_cr_SI_57", HW_H_CR, CGEN_MODE_SI, 0, 57, 0 },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s1bit", HW_H_S1BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s2bit", HW_H_S2BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_maclw_L_r_r__RA__RC_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_cr_SI_56", HW_H_CR, CGEN_MODE_SI, 0, 56, 0 },
  { INPUT, "h_cr_SI_57", HW_H_CR, CGEN_MODE_SI, 0, 57, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { INPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "RA", HW_H_CR, CGEN_MODE_SI, OP_ENT (RA), 0, 0 },
  { OUTPUT, "h_cr_SI_56", HW_H_CR, CGEN_MODE_SI, 0, 56, 0 },
  { OUTPUT, "h_cr_SI_57", HW_H_CR, CGEN_MODE_SI, 0, 57, 0 },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s1bit", HW_H_S1BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s2bit", HW_H_S2BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_maclw_cc__RA__RC_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, COND_REF },
  { INPUT, "Qcondb", HW_H_QCONDB, CGEN_MODE_BI, OP_ENT (QCONDB), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_cr_SI_56", HW_H_CR, CGEN_MODE_SI, 0, 56, COND_REF },
  { INPUT, "h_cr_SI_57", HW_H_CR, CGEN_MODE_SI, 0, 57, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { INPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { OUTPUT, "h_cr_SI_56", HW_H_CR, CGEN_MODE_SI, 0, 56, COND_REF },
  { OUTPUT, "h_cr_SI_57", HW_H_CR, CGEN_MODE_SI, 0, 57, COND_REF },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s1bit", HW_H_S1BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s2bit", HW_H_S2BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_machulw_L_s12__RA__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_cr_SI_56", HW_H_CR, CGEN_MODE_SI, 0, 56, 0 },
  { INPUT, "h_cr_SI_57", HW_H_CR, CGEN_MODE_SI, 0, 57, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { INPUT, "s12", HW_H_SINT, CGEN_MODE_INT, OP_ENT (S12), 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, 0 },
  { OUTPUT, "h_cr_SI_56", HW_H_CR, CGEN_MODE_SI, 0, 56, 0 },
  { OUTPUT, "h_cr_SI_57", HW_H_CR, CGEN_MODE_SI, 0, 57, 0 },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s1bit", HW_H_S1BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s2bit", HW_H_S2BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_machulw_ccu6__RA__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, COND_REF },
  { INPUT, "Qcondb", HW_H_QCONDB, CGEN_MODE_BI, OP_ENT (QCONDB), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U6), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_cr_SI_56", HW_H_CR, CGEN_MODE_SI, 0, 56, COND_REF },
  { INPUT, "h_cr_SI_57", HW_H_CR, CGEN_MODE_SI, 0, 57, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { OUTPUT, "h_cr_SI_56", HW_H_CR, CGEN_MODE_SI, 0, 56, COND_REF },
  { OUTPUT, "h_cr_SI_57", HW_H_CR, CGEN_MODE_SI, 0, 57, COND_REF },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s1bit", HW_H_S1BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s2bit", HW_H_S2BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_machulw_L_u6__RA__ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "U6", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (U6), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_cr_SI_56", HW_H_CR, CGEN_MODE_SI, 0, 56, 0 },
  { INPUT, "h_cr_SI_57", HW_H_CR, CGEN_MODE_SI, 0, 57, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RA", HW_H_CR, CGEN_MODE_SI, OP_ENT (RA), 0, 0 },
  { OUTPUT, "h_cr_SI_56", HW_H_CR, CGEN_MODE_SI, 0, 56, 0 },
  { OUTPUT, "h_cr_SI_57", HW_H_CR, CGEN_MODE_SI, 0, 57, 0 },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s1bit", HW_H_S1BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s2bit", HW_H_S2BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_machulw_L_r_r__RA__RC_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_cr_SI_56", HW_H_CR, CGEN_MODE_SI, 0, 56, 0 },
  { INPUT, "h_cr_SI_57", HW_H_CR, CGEN_MODE_SI, 0, 57, 0 },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RA", HW_H_CR, CGEN_MODE_SI, OP_ENT (RA), 0, 0 },
  { OUTPUT, "h_cr_SI_56", HW_H_CR, CGEN_MODE_SI, 0, 56, 0 },
  { OUTPUT, "h_cr_SI_57", HW_H_CR, CGEN_MODE_SI, 0, 57, 0 },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s1bit", HW_H_S1BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s2bit", HW_H_S2BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_machulw_cc__RA__RC_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "F", HW_H_UFLAGS, CGEN_MODE_UINT, OP_ENT (F), 0, COND_REF },
  { INPUT, "Qcondb", HW_H_QCONDB, CGEN_MODE_BI, OP_ENT (QCONDB), 0, 0 },
  { INPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { INPUT, "RC", HW_H_CR, CGEN_MODE_SI, OP_ENT (RC), 0, COND_REF },
  { INPUT, "f_op_B", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "f_op_C", HW_H_UINT, CGEN_MODE_UINT, 0, 0, 0 },
  { INPUT, "h_cr_SI_56", HW_H_CR, CGEN_MODE_SI, 0, 56, COND_REF },
  { INPUT, "h_cr_SI_57", HW_H_CR, CGEN_MODE_SI, 0, 57, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_4", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "h_memory_HI_add__DFLT_pc_add__DFLT_4_2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "RB", HW_H_CR, CGEN_MODE_SI, OP_ENT (RB), 0, COND_REF },
  { OUTPUT, "h_cr_SI_56", HW_H_CR, CGEN_MODE_SI, 0, 56, COND_REF },
  { OUTPUT, "h_cr_SI_57", HW_H_CR, CGEN_MODE_SI, 0, 57, COND_REF },
  { OUTPUT, "nbit", HW_H_NBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s1bit", HW_H_S1BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "s2bit", HW_H_S2BIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "vbit", HW_H_VBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "zbit", HW_H_ZBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_current_loop_end_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "h_auxr_SI_2", HW_H_AUXR, CGEN_MODE_SI, 0, 2, COND_REF },
  { INPUT, "h_auxr_SI_3", HW_H_AUXR, CGEN_MODE_SI, 0, 3, 0 },
  { INPUT, "h_auxr_SI_34", HW_H_AUXR, CGEN_MODE_SI, 0, 34, COND_REF },
  { INPUT, "h_auxr_SI_37", HW_H_AUXR, CGEN_MODE_SI, 0, 37, COND_REF },
  { INPUT, "h_cr_SI_60", HW_H_CR, CGEN_MODE_SI, 0, 60, COND_REF },
  { INPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { INPUT, "h_prof_offset_SI_0", HW_H_PROF_OFFSET, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_timer_expire_SI_0", HW_H_TIMER_EXPIRE, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_ubit_BI", HW_H_UBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "lbit", HW_H_LBIT, CGEN_MODE_BI, 0, 0, 0 },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, 0 },
  { OUTPUT, "h_auxr_SI_11", HW_H_AUXR, CGEN_MODE_SI, 0, 11, COND_REF },
  { OUTPUT, "h_auxr_SI_33", HW_H_AUXR, CGEN_MODE_SI, 0, 33, COND_REF },
  { OUTPUT, "h_cr_SI_29", HW_H_CR, CGEN_MODE_SI, 0, 29, COND_REF },
  { OUTPUT, "h_cr_SI_60", HW_H_CR, CGEN_MODE_SI, 0, 60, COND_REF },
  { OUTPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

static const CGEN_OPINST sfmt_current_loop_end_after_branch_ops[] ATTRIBUTE_UNUSED = {
  { INPUT, "h_auxr_SI_2", HW_H_AUXR, CGEN_MODE_SI, 0, 2, COND_REF },
  { INPUT, "h_auxr_SI_3", HW_H_AUXR, CGEN_MODE_SI, 0, 3, COND_REF },
  { INPUT, "h_auxr_SI_34", HW_H_AUXR, CGEN_MODE_SI, 0, 34, COND_REF },
  { INPUT, "h_auxr_SI_37", HW_H_AUXR, CGEN_MODE_SI, 0, 37, COND_REF },
  { INPUT, "h_cr_SI_60", HW_H_CR, CGEN_MODE_SI, 0, 60, COND_REF },
  { INPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { INPUT, "h_prof_offset_SI_0", HW_H_PROF_OFFSET, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_status32_SI_0", HW_H_STATUS32, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_timer_expire_SI_0", HW_H_TIMER_EXPIRE, CGEN_MODE_SI, 0, 0, COND_REF },
  { INPUT, "h_ubit_BI", HW_H_UBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "lbit", HW_H_LBIT, CGEN_MODE_BI, 0, 0, COND_REF },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { OUTPUT, "h_auxr_SI_11", HW_H_AUXR, CGEN_MODE_SI, 0, 11, COND_REF },
  { OUTPUT, "h_auxr_SI_33", HW_H_AUXR, CGEN_MODE_SI, 0, 33, COND_REF },
  { OUTPUT, "h_cr_SI_29", HW_H_CR, CGEN_MODE_SI, 0, 29, COND_REF },
  { OUTPUT, "h_cr_SI_60", HW_H_CR, CGEN_MODE_SI, 0, 60, COND_REF },
  { OUTPUT, "h_e1_BI", HW_H_E1, CGEN_MODE_BI, 0, 0, COND_REF },
  { OUTPUT, "h_memory_UHI_countp", HW_H_MEMORY, CGEN_MODE_UHI, 0, 0, COND_REF },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { END, (const char *)0, (enum cgen_hw_type)0, (enum cgen_mode)0, (enum cgen_operand_type)0, 0, 0 }
};

#undef OP_ENT
#undef INPUT
#undef OUTPUT
#undef END
#undef COND_REF

/* Operand instance lookup table.  */

static const CGEN_OPINST *arc_cgen_opinst_table[MAX_INSNS] = {
  0,
  & sfmt_b_s_ops[0],
  & sfmt_bcc_s_ops[0],
  & sfmt_brcc_s_ops[0],
  & sfmt_bcc_l_ops[0],
  & sfmt_bcc_l_ops[0],
  & sfmt_b_l_ops[0],
  & sfmt_b_l_ops[0],
  & sfmt_brcc_RC_ops[0],
  & sfmt_brcc_RC_ops[0],
  & sfmt_brcc_U6_ops[0],
  & sfmt_brcc_U6_ops[0],
  & sfmt_bl_s_ops[0],
  & sfmt_blcc_ops[0],
  & sfmt_blcc_ops[0],
  & sfmt_bl_ops[0],
  & sfmt_bl_d_ops[0],
  & sfmt_ld_abs_ops[0],
  & sfmt_ld__AW_abs_ops[0],
  & sfmt_ld__AW_abs_ops[0],
  & sfmt_ld_abs_ops[0],
  & sfmt_ld_abc_ops[0],
  & sfmt_ld__AW_abc_ops[0],
  & sfmt_ld__AW_abc_ops[0],
  & sfmt_ld_abc_ops[0],
  & sfmt_ld_s_abc_ops[0],
  & sfmt_ld_s_abu_ops[0],
  & sfmt_ld_s_absp_ops[0],
  & sfmt_ld_s_gprel_ops[0],
  & sfmt_ld_s_pcrel_ops[0],
  & sfmt_ldb_abs_ops[0],
  & sfmt_ldb__AW_abs_ops[0],
  & sfmt_ldb__AW_abs_ops[0],
  & sfmt_ldb_as_abs_ops[0],
  & sfmt_ldb_abc_ops[0],
  & sfmt_ldb__AW_abc_ops[0],
  & sfmt_ldb__AW_abc_ops[0],
  & sfmt_ldb_as_abc_ops[0],
  & sfmt_ldb_s_abc_ops[0],
  & sfmt_ldb_s_abu_ops[0],
  & sfmt_ldb_s_absp_ops[0],
  & sfmt_ldb_s_gprel_ops[0],
  & sfmt_ldb_abs_ops[0],
  & sfmt_ldb__AW_abs_ops[0],
  & sfmt_ldb__AW_abs_ops[0],
  & sfmt_ldb_as_abs_ops[0],
  & sfmt_ldb_abc_ops[0],
  & sfmt_ldb__AW_abc_ops[0],
  & sfmt_ldb__AW_abc_ops[0],
  & sfmt_ldb_as_abc_ops[0],
  & sfmt_ldw_abs_ops[0],
  & sfmt_ldw__AW_abs_ops[0],
  & sfmt_ldw__AW_abs_ops[0],
  & sfmt_ldw_abs_ops[0],
  & sfmt_ldw_abc_ops[0],
  & sfmt_ldw__AW_abc_ops[0],
  & sfmt_ldw__AW_abc_ops[0],
  & sfmt_ldw_abc_ops[0],
  & sfmt_ldw_s_abc_ops[0],
  & sfmt_ldw_s_abu_ops[0],
  & sfmt_ldw_s_gprel_ops[0],
  & sfmt_ldw_abs_ops[0],
  & sfmt_ldw__AW_abs_ops[0],
  & sfmt_ldw__AW_abs_ops[0],
  & sfmt_ldw_abs_ops[0],
  & sfmt_ldw_abc_ops[0],
  & sfmt_ldw__AW_abc_ops[0],
  & sfmt_ldw__AW_abc_ops[0],
  & sfmt_ldw_abc_ops[0],
  & sfmt_ldw_s_abu_ops[0],
  & sfmt_st_abs_ops[0],
  & sfmt_st__AW_abs_ops[0],
  & sfmt_st__AW_abs_ops[0],
  & sfmt_st_abs_ops[0],
  & sfmt_st_s_abu_ops[0],
  & sfmt_st_s_absp_ops[0],
  & sfmt_stb_abs_ops[0],
  & sfmt_stb__AW_abs_ops[0],
  & sfmt_stb__AW_abs_ops[0],
  & sfmt_stb_as_abs_ops[0],
  & sfmt_stb_s_abu_ops[0],
  & sfmt_stb_s_absp_ops[0],
  & sfmt_stw_abs_ops[0],
  & sfmt_stw__AW_abs_ops[0],
  & sfmt_stw__AW_abs_ops[0],
  & sfmt_stw_abs_ops[0],
  & sfmt_stw_s_abu_ops[0],
  & sfmt_add_L_s12__RA__ops[0],
  & sfmt_add_ccu6__RA__ops[0],
  & sfmt_add_L_u6__RA__ops[0],
  & sfmt_add_L_r_r__RA__RC_ops[0],
  & sfmt_add_cc__RA__RC_ops[0],
  & sfmt_add_s_abc_ops[0],
  & sfmt_add_s_cbu3_ops[0],
  & sfmt_add_s_mcah_ops[0],
  & sfmt_add_s_absp_ops[0],
  & sfmt_add_s_asspsp_ops[0],
  & sfmt_add_s_gp_ops[0],
  & sfmt_add_s_r_u7_ops[0],
  & sfmt_adc_L_s12__RA__ops[0],
  & sfmt_adc_ccu6__RA__ops[0],
  & sfmt_adc_L_u6__RA__ops[0],
  & sfmt_adc_L_r_r__RA__RC_ops[0],
  & sfmt_adc_cc__RA__RC_ops[0],
  & sfmt_add_L_s12__RA__ops[0],
  & sfmt_add_ccu6__RA__ops[0],
  & sfmt_add_L_u6__RA__ops[0],
  & sfmt_add_L_r_r__RA__RC_ops[0],
  & sfmt_add_cc__RA__RC_ops[0],
  & sfmt_add_s_cbu3_ops[0],
  & sfmt_I16_GO_SUB_s_go_ops[0],
  & sfmt_sub_s_go_sub_ne_ops[0],
  & sfmt_sub_s_ssb_ops[0],
  & sfmt_add_s_asspsp_ops[0],
  & sfmt_adc_L_s12__RA__ops[0],
  & sfmt_adc_ccu6__RA__ops[0],
  & sfmt_adc_L_u6__RA__ops[0],
  & sfmt_adc_L_r_r__RA__RC_ops[0],
  & sfmt_adc_cc__RA__RC_ops[0],
  & sfmt_and_L_s12__RA__ops[0],
  & sfmt_and_ccu6__RA__ops[0],
  & sfmt_and_L_u6__RA__ops[0],
  & sfmt_and_L_r_r__RA__RC_ops[0],
  & sfmt_and_cc__RA__RC_ops[0],
  & sfmt_I16_GO_SUB_s_go_ops[0],
  & sfmt_and_L_s12__RA__ops[0],
  & sfmt_and_ccu6__RA__ops[0],
  & sfmt_and_L_u6__RA__ops[0],
  & sfmt_and_L_r_r__RA__RC_ops[0],
  & sfmt_and_cc__RA__RC_ops[0],
  & sfmt_I16_GO_SUB_s_go_ops[0],
  & sfmt_and_L_s12__RA__ops[0],
  & sfmt_and_ccu6__RA__ops[0],
  & sfmt_and_L_u6__RA__ops[0],
  & sfmt_and_L_r_r__RA__RC_ops[0],
  & sfmt_and_cc__RA__RC_ops[0],
  & sfmt_I16_GO_SUB_s_go_ops[0],
  & sfmt_and_L_s12__RA__ops[0],
  & sfmt_and_ccu6__RA__ops[0],
  & sfmt_and_L_u6__RA__ops[0],
  & sfmt_and_L_r_r__RA__RC_ops[0],
  & sfmt_and_cc__RA__RC_ops[0],
  & sfmt_I16_GO_SUB_s_go_ops[0],
  & sfmt_add_L_s12__RA__ops[0],
  & sfmt_add_ccu6__RA__ops[0],
  & sfmt_add_L_u6__RA__ops[0],
  & sfmt_add_L_r_r__RA__RC_ops[0],
  & sfmt_add_cc__RA__RC_ops[0],
  & sfmt_add_L_s12__RA__ops[0],
  & sfmt_add_ccu6__RA__ops[0],
  & sfmt_add_L_u6__RA__ops[0],
  & sfmt_add_L_r_r__RA__RC_ops[0],
  & sfmt_add_cc__RA__RC_ops[0],
  & sfmt_mov_L_s12__ops[0],
  & sfmt_mov_ccu6__ops[0],
  & sfmt_mov_L_u6__ops[0],
  & sfmt_mov_L_r_r__RC_ops[0],
  & sfmt_mov_cc__RC_ops[0],
  & sfmt_mov_s_mcah_ops[0],
  & sfmt_mov_s_mcahb_ops[0],
  & sfmt_mov_s_r_u7_ops[0],
  & sfmt_tst_L_s12__ops[0],
  & sfmt_tst_ccu6__ops[0],
  & sfmt_tst_L_u6__ops[0],
  & sfmt_tst_L_r_r__RC_ops[0],
  & sfmt_tst_cc__RC_ops[0],
  & sfmt_tst_s_go_ops[0],
  & sfmt_cmp_L_s12__ops[0],
  & sfmt_cmp_ccu6__ops[0],
  & sfmt_cmp_L_u6__ops[0],
  & sfmt_cmp_L_r_r__RC_ops[0],
  & sfmt_cmp_cc__RC_ops[0],
  & sfmt_cmp_s_mcah_ops[0],
  & sfmt_cmp_s_r_u7_ops[0],
  & sfmt_cmp_L_s12__ops[0],
  & sfmt_cmp_ccu6__ops[0],
  & sfmt_cmp_L_u6__ops[0],
  & sfmt_cmp_L_r_r__RC_ops[0],
  & sfmt_cmp_cc__RC_ops[0],
  & sfmt_add_L_s12__RA__ops[0],
  & sfmt_add_ccu6__RA__ops[0],
  & sfmt_add_L_u6__RA__ops[0],
  & sfmt_add_L_r_r__RA__RC_ops[0],
  & sfmt_add_cc__RA__RC_ops[0],
  & sfmt_and_L_s12__RA__ops[0],
  & sfmt_and_ccu6__RA__ops[0],
  & sfmt_and_L_u6__RA__ops[0],
  & sfmt_and_L_r_r__RA__RC_ops[0],
  & sfmt_and_cc__RA__RC_ops[0],
  & sfmt_sub_s_ssb_ops[0],
  & sfmt_and_L_s12__RA__ops[0],
  & sfmt_and_ccu6__RA__ops[0],
  & sfmt_and_L_u6__RA__ops[0],
  & sfmt_and_L_r_r__RA__RC_ops[0],
  & sfmt_and_cc__RA__RC_ops[0],
  & sfmt_sub_s_ssb_ops[0],
  & sfmt_tst_L_s12__ops[0],
  & sfmt_tst_ccu6__ops[0],
  & sfmt_tst_L_u6__ops[0],
  & sfmt_tst_L_r_r__RC_ops[0],
  & sfmt_tst_cc__RC_ops[0],
  & sfmt_btst_s_ssb_ops[0],
  & sfmt_and_L_s12__RA__ops[0],
  & sfmt_and_ccu6__RA__ops[0],
  & sfmt_and_L_u6__RA__ops[0],
  & sfmt_and_L_r_r__RA__RC_ops[0],
  & sfmt_and_cc__RA__RC_ops[0],
  & sfmt_and_L_s12__RA__ops[0],
  & sfmt_and_ccu6__RA__ops[0],
  & sfmt_and_L_u6__RA__ops[0],
  & sfmt_and_L_r_r__RA__RC_ops[0],
  & sfmt_and_cc__RA__RC_ops[0],
  & sfmt_sub_s_ssb_ops[0],
  & sfmt_add_L_s12__RA__ops[0],
  & sfmt_add_ccu6__RA__ops[0],
  & sfmt_add_L_u6__RA__ops[0],
  & sfmt_add_L_r_r__RA__RC_ops[0],
  & sfmt_add_cc__RA__RC_ops[0],
  & sfmt_I16_GO_SUB_s_go_ops[0],
  & sfmt_add_L_s12__RA__ops[0],
  & sfmt_add_ccu6__RA__ops[0],
  & sfmt_add_L_u6__RA__ops[0],
  & sfmt_add_L_r_r__RA__RC_ops[0],
  & sfmt_add_cc__RA__RC_ops[0],
  & sfmt_I16_GO_SUB_s_go_ops[0],
  & sfmt_add_L_s12__RA__ops[0],
  & sfmt_add_ccu6__RA__ops[0],
  & sfmt_add_L_u6__RA__ops[0],
  & sfmt_add_L_r_r__RA__RC_ops[0],
  & sfmt_add_cc__RA__RC_ops[0],
  & sfmt_I16_GO_SUB_s_go_ops[0],
  & sfmt_add_L_s12__RA__ops[0],
  & sfmt_add_ccu6__RA__ops[0],
  & sfmt_add_L_u6__RA__ops[0],
  & sfmt_add_L_r_r__RA__RC_ops[0],
  & sfmt_add_cc__RA__RC_ops[0],
  & sfmt_add_L_s12__RA__ops[0],
  & sfmt_add_ccu6__RA__ops[0],
  & sfmt_add_L_u6__RA__ops[0],
  & sfmt_add_L_r_r__RA__RC_ops[0],
  & sfmt_add_cc__RA__RC_ops[0],
  & sfmt_add_L_s12__RA__ops[0],
  & sfmt_add_ccu6__RA__ops[0],
  & sfmt_add_L_u6__RA__ops[0],
  & sfmt_add_L_r_r__RA__RC_ops[0],
  & sfmt_add_cc__RA__RC_ops[0],
  & sfmt_mpy_L_s12__RA__ops[0],
  & sfmt_mpy_ccu6__RA__ops[0],
  & sfmt_mpy_L_u6__RA__ops[0],
  & sfmt_mpy_L_r_r__RA__RC_ops[0],
  & sfmt_mpy_cc__RA__RC_ops[0],
  & sfmt_mpy_L_s12__RA__ops[0],
  & sfmt_mpy_ccu6__RA__ops[0],
  & sfmt_mpy_L_u6__RA__ops[0],
  & sfmt_mpy_L_r_r__RA__RC_ops[0],
  & sfmt_mpy_cc__RA__RC_ops[0],
  & sfmt_mpy_L_s12__RA__ops[0],
  & sfmt_mpy_ccu6__RA__ops[0],
  & sfmt_mpy_L_u6__RA__ops[0],
  & sfmt_mpy_L_r_r__RA__RC_ops[0],
  & sfmt_mpy_cc__RA__RC_ops[0],
  & sfmt_mpy_L_s12__RA__ops[0],
  & sfmt_mpy_ccu6__RA__ops[0],
  & sfmt_mpy_L_u6__RA__ops[0],
  & sfmt_mpy_L_r_r__RA__RC_ops[0],
  & sfmt_mpy_cc__RA__RC_ops[0],
  & sfmt_j_L_r_r___RC_noilink__ops[0],
  & sfmt_j_cc___RC_noilink__ops[0],
  & sfmt_j_L_r_r___RC_ilink__ops[0],
  & sfmt_j_cc___RC_ilink__ops[0],
  & sfmt_j_L_s12__ops[0],
  & sfmt_j_ccu6__ops[0],
  & sfmt_j_L_u6__ops[0],
  & sfmt_j_s_ops[0],
  & sfmt_j_s__S_ops[0],
  & sfmt_j_seq__S_ops[0],
  & sfmt_j_seq__S_ops[0],
  & sfmt_j_L_s12_d__ops[0],
  & sfmt_j_ccu6_d__ops[0],
  & sfmt_j_L_u6_d__ops[0],
  & sfmt_j_L_r_r_d___RC__ops[0],
  & sfmt_j_cc_d___RC__ops[0],
  & sfmt_j_s_ops[0],
  & sfmt_j_s__S_ops[0],
  & sfmt_jl_L_s12__ops[0],
  & sfmt_jl_ccu6__ops[0],
  & sfmt_jl_L_u6__ops[0],
  & sfmt_jl_s_ops[0],
  & sfmt_jl_L_r_r___RC_noilink__ops[0],
  & sfmt_jl_cc___RC_noilink__ops[0],
  & sfmt_jl_L_s12__ops[0],
  & sfmt_jl_ccu6__ops[0],
  & sfmt_jl_L_u6__ops[0],
  & sfmt_jl_L_r_r_d___RC__ops[0],
  & sfmt_jl_cc_d___RC__ops[0],
  & sfmt_jl_s_d_ops[0],
  & sfmt_lp_L_s12__ops[0],
  & sfmt_lpcc_ccu6_ops[0],
  & sfmt_flag_L_s12__ops[0],
  & sfmt_flag_ccu6__ops[0],
  & sfmt_flag_L_u6__ops[0],
  & sfmt_flag_L_r_r__RC_ops[0],
  & sfmt_flag_cc__RC_ops[0],
  & sfmt_lr_L_r_r___RC__ops[0],
  & sfmt_lr_L_s12__ops[0],
  & sfmt_lr_L_u6__ops[0],
  & sfmt_sr_L_r_r___RC__ops[0],
  & sfmt_sr_L_s12__ops[0],
  & sfmt_sr_L_u6__ops[0],
  & sfmt_asl_L_r_r__RC_ops[0],
  & sfmt_asl_L_u6__ops[0],
  & sfmt_I16_GO_SUB_s_go_ops[0],
  & sfmt_asr_L_r_r__RC_ops[0],
  & sfmt_asr_L_u6__ops[0],
  & sfmt_I16_GO_SUB_s_go_ops[0],
  & sfmt_asr_L_r_r__RC_ops[0],
  & sfmt_asr_L_u6__ops[0],
  & sfmt_I16_GO_SUB_s_go_ops[0],
  & sfmt_asr_L_r_r__RC_ops[0],
  & sfmt_asr_L_u6__ops[0],
  & sfmt_rrc_L_r_r__RC_ops[0],
  & sfmt_rrc_L_u6__ops[0],
  & sfmt_sexb_L_r_r__RC_ops[0],
  & sfmt_sexb_L_u6__ops[0],
  & sfmt_I16_GO_SUB_s_go_ops[0],
  & sfmt_sexw_L_r_r__RC_ops[0],
  & sfmt_sexw_L_u6__ops[0],
  & sfmt_I16_GO_SUB_s_go_ops[0],
  & sfmt_sexb_L_r_r__RC_ops[0],
  & sfmt_sexb_L_u6__ops[0],
  & sfmt_I16_GO_SUB_s_go_ops[0],
  & sfmt_sexw_L_r_r__RC_ops[0],
  & sfmt_sexw_L_u6__ops[0],
  & sfmt_I16_GO_SUB_s_go_ops[0],
  & sfmt_abs_L_r_r__RC_ops[0],
  & sfmt_abs_L_u6__ops[0],
  & sfmt_I16_GO_SUB_s_go_ops[0],
  & sfmt_not_L_r_r__RC_ops[0],
  & sfmt_not_L_u6__ops[0],
  & sfmt_I16_GO_SUB_s_go_ops[0],
  & sfmt_rrc_L_r_r__RC_ops[0],
  & sfmt_rrc_L_u6__ops[0],
  & sfmt_ex_L_r_r__RC_ops[0],
  & sfmt_ex_L_u6__ops[0],
  & sfmt_I16_GO_SUB_s_go_ops[0],
  & sfmt_swi_ops[0],
  & sfmt_trap_s_ops[0],
  & sfmt_brk_ops[0],
  & sfmt_brk_ops[0],
  & sfmt_asl_L_s12__RA__ops[0],
  & sfmt_asl_ccu6__RA__ops[0],
  & sfmt_asl_L_u6__RA__ops[0],
  & sfmt_asl_L_r_r__RA__RC_ops[0],
  & sfmt_asl_cc__RA__RC_ops[0],
  & sfmt_add_s_cbu3_ops[0],
  & sfmt_sub_s_ssb_ops[0],
  & sfmt_I16_GO_SUB_s_go_ops[0],
  & sfmt_asl_L_s12__RA__ops[0],
  & sfmt_asl_ccu6__RA__ops[0],
  & sfmt_asl_L_u6__RA__ops[0],
  & sfmt_asl_L_r_r__RA__RC_ops[0],
  & sfmt_asl_cc__RA__RC_ops[0],
  & sfmt_sub_s_ssb_ops[0],
  & sfmt_I16_GO_SUB_s_go_ops[0],
  & sfmt_asl_L_s12__RA__ops[0],
  & sfmt_asl_ccu6__RA__ops[0],
  & sfmt_asl_L_u6__RA__ops[0],
  & sfmt_asl_L_r_r__RA__RC_ops[0],
  & sfmt_asl_cc__RA__RC_ops[0],
  & sfmt_add_s_cbu3_ops[0],
  & sfmt_sub_s_ssb_ops[0],
  & sfmt_I16_GO_SUB_s_go_ops[0],
  & sfmt_asl_L_s12__RA__ops[0],
  & sfmt_asl_ccu6__RA__ops[0],
  & sfmt_asl_L_u6__RA__ops[0],
  & sfmt_asl_L_r_r__RA__RC_ops[0],
  & sfmt_asl_cc__RA__RC_ops[0],
  & sfmt_mul64_L_s12__ops[0],
  & sfmt_mul64_ccu6__ops[0],
  & sfmt_mul64_L_u6__ops[0],
  & sfmt_mul64_L_r_r__RC_ops[0],
  & sfmt_mul64_cc__RC_ops[0],
  & sfmt_mul64_s_go_ops[0],
  & sfmt_mul64_L_s12__ops[0],
  & sfmt_mul64_ccu6__ops[0],
  & sfmt_mul64_L_u6__ops[0],
  & sfmt_mul64_L_r_r__RC_ops[0],
  & sfmt_mul64_cc__RC_ops[0],
  & sfmt_adds_L_s12__RA__ops[0],
  & sfmt_adds_ccu6__RA__ops[0],
  & sfmt_adds_L_u6__RA__ops[0],
  & sfmt_adds_L_r_r__RA__RC_ops[0],
  & sfmt_adds_cc__RA__RC_ops[0],
  & sfmt_adds_L_s12__RA__ops[0],
  & sfmt_adds_ccu6__RA__ops[0],
  & sfmt_adds_L_u6__RA__ops[0],
  & sfmt_adds_L_r_r__RA__RC_ops[0],
  & sfmt_adds_cc__RA__RC_ops[0],
  & sfmt_divaw_L_s12__RA__ops[0],
  & sfmt_divaw_ccu6__RA__ops[0],
  & sfmt_divaw_L_u6__RA__ops[0],
  & sfmt_divaw_L_r_r__RA__RC_ops[0],
  & sfmt_divaw_cc__RA__RC_ops[0],
  & sfmt_asls_L_s12__RA__ops[0],
  & sfmt_asls_ccu6__RA__ops[0],
  & sfmt_asls_L_u6__RA__ops[0],
  & sfmt_asls_L_r_r__RA__RC_ops[0],
  & sfmt_asls_cc__RA__RC_ops[0],
  & sfmt_asls_L_s12__RA__ops[0],
  & sfmt_asls_ccu6__RA__ops[0],
  & sfmt_asls_L_u6__RA__ops[0],
  & sfmt_asls_L_r_r__RA__RC_ops[0],
  & sfmt_asls_cc__RA__RC_ops[0],
  & sfmt_asls_L_s12__RA__ops[0],
  & sfmt_asls_ccu6__RA__ops[0],
  & sfmt_asls_L_u6__RA__ops[0],
  & sfmt_asls_L_r_r__RA__RC_ops[0],
  & sfmt_asls_cc__RA__RC_ops[0],
  & sfmt_asls_L_s12__RA__ops[0],
  & sfmt_asls_ccu6__RA__ops[0],
  & sfmt_asls_L_u6__RA__ops[0],
  & sfmt_asls_L_r_r__RA__RC_ops[0],
  & sfmt_asls_cc__RA__RC_ops[0],
  & sfmt_swap_L_r_r__RC_ops[0],
  & sfmt_swap_L_u6__ops[0],
  & sfmt_swap_L_r_r__RC_ops[0],
  & sfmt_norm_L_u6__ops[0],
  & sfmt_rnd16_L_r_r__RC_ops[0],
  & sfmt_rnd16_L_u6__ops[0],
  & sfmt_abssw_L_r_r__RC_ops[0],
  & sfmt_abssw_L_u6__ops[0],
  & sfmt_rnd16_L_r_r__RC_ops[0],
  & sfmt_abss_L_u6__ops[0],
  & sfmt_abssw_L_r_r__RC_ops[0],
  & sfmt_abssw_L_u6__ops[0],
  & sfmt_rnd16_L_r_r__RC_ops[0],
  & sfmt_rnd16_L_u6__ops[0],
  & sfmt_swap_L_r_r__RC_ops[0],
  & sfmt_swap_L_u6__ops[0],
  & sfmt_nop_s_ops[0],
  & sfmt_nop_s_ops[0],
  & sfmt_pop_s_b_ops[0],
  & sfmt_pop_s_blink_ops[0],
  & sfmt_push_s_b_ops[0],
  & sfmt_push_s_blink_ops[0],
  & sfmt_mullw_L_s12__RA__ops[0],
  & sfmt_mullw_ccu6__RA__ops[0],
  & sfmt_mullw_L_u6__RA__ops[0],
  & sfmt_mullw_L_r_r__RA__RC_ops[0],
  & sfmt_mullw_cc__RA__RC_ops[0],
  & sfmt_maclw_L_s12__RA__ops[0],
  & sfmt_maclw_ccu6__RA__ops[0],
  & sfmt_maclw_L_u6__RA__ops[0],
  & sfmt_maclw_L_r_r__RA__RC_ops[0],
  & sfmt_maclw_cc__RA__RC_ops[0],
  & sfmt_maclw_L_s12__RA__ops[0],
  & sfmt_maclw_ccu6__RA__ops[0],
  & sfmt_maclw_L_u6__RA__ops[0],
  & sfmt_maclw_L_r_r__RA__RC_ops[0],
  & sfmt_maclw_cc__RA__RC_ops[0],
  & sfmt_mullw_L_s12__RA__ops[0],
  & sfmt_mullw_ccu6__RA__ops[0],
  & sfmt_mullw_L_u6__RA__ops[0],
  & sfmt_mullw_L_r_r__RA__RC_ops[0],
  & sfmt_mullw_cc__RA__RC_ops[0],
  & sfmt_machulw_L_s12__RA__ops[0],
  & sfmt_machulw_ccu6__RA__ops[0],
  & sfmt_machulw_L_u6__RA__ops[0],
  & sfmt_machulw_L_r_r__RA__RC_ops[0],
  & sfmt_machulw_cc__RA__RC_ops[0],
  & sfmt_current_loop_end_ops[0],
  & sfmt_current_loop_end_after_branch_ops[0],
  & sfmt_current_loop_end_after_branch_ops[0],
};

/* Function to call before using the operand instance table.  */

void
arc_cgen_init_opinst_table (cd)
     CGEN_CPU_DESC cd;
{
  int i;
  const CGEN_OPINST **oi = & arc_cgen_opinst_table[0];
  CGEN_INSN *insns = (CGEN_INSN *) cd->insn_table.init_entries;
  for (i = 0; i < MAX_INSNS; ++i)
    insns[i].opinst = oi[i];
}
