/* Semantic operand instances for m32r.

THIS FILE IS MACHINE GENERATED WITH CGEN.

Copyright (C) 1996, 1997, 1998, 1999 Free Software Foundation, Inc.

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
59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

*/

#include "sysdep.h"
#include "ansidecl.h"
#include "bfd.h"
#include "symcat.h"
#include "m32r-desc.h"
#include "m32r-opc.h"

/* Operand references.  */

#define INPUT CGEN_OPINST_INPUT
#define OUTPUT CGEN_OPINST_OUTPUT
#define END CGEN_OPINST_END
#define COND_REF CGEN_OPINST_COND_REF
#define OP_ENT(op) CONCAT2 (M32R_OPERAND_,op)

static const CGEN_OPINST fmt_empty_ops[] = {
  { END }
};

static const CGEN_OPINST fmt_add_ops[] = {
  { INPUT, "dr", HW_H_GR, CGEN_MODE_SI, OP_ENT (DR), 0, 0 },
  { INPUT, "sr", HW_H_GR, CGEN_MODE_SI, OP_ENT (SR), 0, 0 },
  { OUTPUT, "dr", HW_H_GR, CGEN_MODE_SI, OP_ENT (DR), 0, 0 },
  { END }
};

static const CGEN_OPINST fmt_add3_ops[] = {
  { INPUT, "slo16", HW_H_SLO16, CGEN_MODE_INT, OP_ENT (SLO16), 0, 0 },
  { INPUT, "sr", HW_H_GR, CGEN_MODE_SI, OP_ENT (SR), 0, 0 },
  { OUTPUT, "dr", HW_H_GR, CGEN_MODE_SI, OP_ENT (DR), 0, 0 },
  { END }
};

static const CGEN_OPINST fmt_and3_ops[] = {
  { INPUT, "sr", HW_H_GR, CGEN_MODE_SI, OP_ENT (SR), 0, 0 },
  { INPUT, "uimm16", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (UIMM16), 0, 0 },
  { OUTPUT, "dr", HW_H_GR, CGEN_MODE_SI, OP_ENT (DR), 0, 0 },
  { END }
};

static const CGEN_OPINST fmt_or3_ops[] = {
  { INPUT, "sr", HW_H_GR, CGEN_MODE_SI, OP_ENT (SR), 0, 0 },
  { INPUT, "ulo16", HW_H_ULO16, CGEN_MODE_UINT, OP_ENT (ULO16), 0, 0 },
  { OUTPUT, "dr", HW_H_GR, CGEN_MODE_SI, OP_ENT (DR), 0, 0 },
  { END }
};

static const CGEN_OPINST fmt_addi_ops[] = {
  { INPUT, "dr", HW_H_GR, CGEN_MODE_SI, OP_ENT (DR), 0, 0 },
  { INPUT, "simm8", HW_H_SINT, CGEN_MODE_INT, OP_ENT (SIMM8), 0, 0 },
  { OUTPUT, "dr", HW_H_GR, CGEN_MODE_SI, OP_ENT (DR), 0, 0 },
  { END }
};

static const CGEN_OPINST fmt_addv_ops[] = {
  { INPUT, "dr", HW_H_GR, CGEN_MODE_SI, OP_ENT (DR), 0, 0 },
  { INPUT, "sr", HW_H_GR, CGEN_MODE_SI, OP_ENT (SR), 0, 0 },
  { OUTPUT, "condbit", HW_H_COND, CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "dr", HW_H_GR, CGEN_MODE_SI, OP_ENT (DR), 0, 0 },
  { END }
};

static const CGEN_OPINST fmt_addv3_ops[] = {
  { INPUT, "simm16", HW_H_SINT, CGEN_MODE_INT, OP_ENT (SIMM16), 0, 0 },
  { INPUT, "sr", HW_H_GR, CGEN_MODE_SI, OP_ENT (SR), 0, 0 },
  { OUTPUT, "condbit", HW_H_COND, CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "dr", HW_H_GR, CGEN_MODE_SI, OP_ENT (DR), 0, 0 },
  { END }
};

static const CGEN_OPINST fmt_addx_ops[] = {
  { INPUT, "condbit", HW_H_COND, CGEN_MODE_BI, 0, 0, 0 },
  { INPUT, "dr", HW_H_GR, CGEN_MODE_SI, OP_ENT (DR), 0, 0 },
  { INPUT, "sr", HW_H_GR, CGEN_MODE_SI, OP_ENT (SR), 0, 0 },
  { OUTPUT, "condbit", HW_H_COND, CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "dr", HW_H_GR, CGEN_MODE_SI, OP_ENT (DR), 0, 0 },
  { END }
};

static const CGEN_OPINST fmt_bc8_ops[] = {
  { INPUT, "condbit", HW_H_COND, CGEN_MODE_BI, 0, 0, 0 },
  { INPUT, "disp8", HW_H_IADDR, CGEN_MODE_USI, OP_ENT (DISP8), 0, COND_REF },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { END }
};

static const CGEN_OPINST fmt_bc24_ops[] = {
  { INPUT, "condbit", HW_H_COND, CGEN_MODE_BI, 0, 0, 0 },
  { INPUT, "disp24", HW_H_IADDR, CGEN_MODE_USI, OP_ENT (DISP24), 0, COND_REF },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { END }
};

static const CGEN_OPINST fmt_beq_ops[] = {
  { INPUT, "disp16", HW_H_IADDR, CGEN_MODE_USI, OP_ENT (DISP16), 0, COND_REF },
  { INPUT, "src1", HW_H_GR, CGEN_MODE_SI, OP_ENT (SRC1), 0, 0 },
  { INPUT, "src2", HW_H_GR, CGEN_MODE_SI, OP_ENT (SRC2), 0, 0 },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { END }
};

static const CGEN_OPINST fmt_beqz_ops[] = {
  { INPUT, "disp16", HW_H_IADDR, CGEN_MODE_USI, OP_ENT (DISP16), 0, COND_REF },
  { INPUT, "src2", HW_H_GR, CGEN_MODE_SI, OP_ENT (SRC2), 0, 0 },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, COND_REF },
  { END }
};

static const CGEN_OPINST fmt_bl8_ops[] = {
  { INPUT, "disp8", HW_H_IADDR, CGEN_MODE_USI, OP_ENT (DISP8), 0, 0 },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, 0 },
  { OUTPUT, "h_gr_14", HW_H_GR, CGEN_MODE_SI, 0, 14, 0 },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, 0 },
  { END }
};

static const CGEN_OPINST fmt_bl24_ops[] = {
  { INPUT, "disp24", HW_H_IADDR, CGEN_MODE_USI, OP_ENT (DISP24), 0, 0 },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, 0 },
  { OUTPUT, "h_gr_14", HW_H_GR, CGEN_MODE_SI, 0, 14, 0 },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, 0 },
  { END }
};

static const CGEN_OPINST fmt_bra8_ops[] = {
  { INPUT, "disp8", HW_H_IADDR, CGEN_MODE_USI, OP_ENT (DISP8), 0, 0 },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, 0 },
  { END }
};

static const CGEN_OPINST fmt_bra24_ops[] = {
  { INPUT, "disp24", HW_H_IADDR, CGEN_MODE_USI, OP_ENT (DISP24), 0, 0 },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, 0 },
  { END }
};

static const CGEN_OPINST fmt_cmp_ops[] = {
  { INPUT, "src1", HW_H_GR, CGEN_MODE_SI, OP_ENT (SRC1), 0, 0 },
  { INPUT, "src2", HW_H_GR, CGEN_MODE_SI, OP_ENT (SRC2), 0, 0 },
  { OUTPUT, "condbit", HW_H_COND, CGEN_MODE_BI, 0, 0, 0 },
  { END }
};

static const CGEN_OPINST fmt_cmpi_ops[] = {
  { INPUT, "simm16", HW_H_SINT, CGEN_MODE_INT, OP_ENT (SIMM16), 0, 0 },
  { INPUT, "src2", HW_H_GR, CGEN_MODE_SI, OP_ENT (SRC2), 0, 0 },
  { OUTPUT, "condbit", HW_H_COND, CGEN_MODE_BI, 0, 0, 0 },
  { END }
};

static const CGEN_OPINST fmt_div_ops[] = {
  { INPUT, "dr", HW_H_GR, CGEN_MODE_SI, OP_ENT (DR), 0, COND_REF },
  { INPUT, "sr", HW_H_GR, CGEN_MODE_SI, OP_ENT (SR), 0, 0 },
  { OUTPUT, "dr", HW_H_GR, CGEN_MODE_SI, OP_ENT (DR), 0, COND_REF },
  { END }
};

static const CGEN_OPINST fmt_jl_ops[] = {
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, 0 },
  { INPUT, "sr", HW_H_GR, CGEN_MODE_SI, OP_ENT (SR), 0, 0 },
  { OUTPUT, "h_gr_14", HW_H_GR, CGEN_MODE_SI, 0, 14, 0 },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, 0 },
  { END }
};

static const CGEN_OPINST fmt_jmp_ops[] = {
  { INPUT, "sr", HW_H_GR, CGEN_MODE_SI, OP_ENT (SR), 0, 0 },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, 0 },
  { END }
};

static const CGEN_OPINST fmt_ld_ops[] = {
  { INPUT, "h_memory_sr", HW_H_MEMORY, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "sr", HW_H_GR, CGEN_MODE_USI, OP_ENT (SR), 0, 0 },
  { OUTPUT, "dr", HW_H_GR, CGEN_MODE_SI, OP_ENT (DR), 0, 0 },
  { END }
};

static const CGEN_OPINST fmt_ld_d_ops[] = {
  { INPUT, "h_memory_add__VM_sr_slo16", HW_H_MEMORY, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "slo16", HW_H_SLO16, CGEN_MODE_INT, OP_ENT (SLO16), 0, 0 },
  { INPUT, "sr", HW_H_GR, CGEN_MODE_SI, OP_ENT (SR), 0, 0 },
  { OUTPUT, "dr", HW_H_GR, CGEN_MODE_SI, OP_ENT (DR), 0, 0 },
  { END }
};

static const CGEN_OPINST fmt_ldb_ops[] = {
  { INPUT, "h_memory_sr", HW_H_MEMORY, CGEN_MODE_QI, 0, 0, 0 },
  { INPUT, "sr", HW_H_GR, CGEN_MODE_USI, OP_ENT (SR), 0, 0 },
  { OUTPUT, "dr", HW_H_GR, CGEN_MODE_SI, OP_ENT (DR), 0, 0 },
  { END }
};

static const CGEN_OPINST fmt_ldb_d_ops[] = {
  { INPUT, "h_memory_add__VM_sr_slo16", HW_H_MEMORY, CGEN_MODE_QI, 0, 0, 0 },
  { INPUT, "slo16", HW_H_SLO16, CGEN_MODE_INT, OP_ENT (SLO16), 0, 0 },
  { INPUT, "sr", HW_H_GR, CGEN_MODE_SI, OP_ENT (SR), 0, 0 },
  { OUTPUT, "dr", HW_H_GR, CGEN_MODE_SI, OP_ENT (DR), 0, 0 },
  { END }
};

static const CGEN_OPINST fmt_ldh_ops[] = {
  { INPUT, "h_memory_sr", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, 0 },
  { INPUT, "sr", HW_H_GR, CGEN_MODE_USI, OP_ENT (SR), 0, 0 },
  { OUTPUT, "dr", HW_H_GR, CGEN_MODE_SI, OP_ENT (DR), 0, 0 },
  { END }
};

static const CGEN_OPINST fmt_ldh_d_ops[] = {
  { INPUT, "h_memory_add__VM_sr_slo16", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, 0 },
  { INPUT, "slo16", HW_H_SLO16, CGEN_MODE_INT, OP_ENT (SLO16), 0, 0 },
  { INPUT, "sr", HW_H_GR, CGEN_MODE_SI, OP_ENT (SR), 0, 0 },
  { OUTPUT, "dr", HW_H_GR, CGEN_MODE_SI, OP_ENT (DR), 0, 0 },
  { END }
};

static const CGEN_OPINST fmt_ld_plus_ops[] = {
  { INPUT, "h_memory_sr", HW_H_MEMORY, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "sr", HW_H_GR, CGEN_MODE_USI, OP_ENT (SR), 0, 0 },
  { OUTPUT, "dr", HW_H_GR, CGEN_MODE_SI, OP_ENT (DR), 0, 0 },
  { OUTPUT, "sr", HW_H_GR, CGEN_MODE_SI, OP_ENT (SR), 0, 0 },
  { END }
};

static const CGEN_OPINST fmt_ld24_ops[] = {
  { INPUT, "uimm24", HW_H_ADDR, CGEN_MODE_USI, OP_ENT (UIMM24), 0, 0 },
  { OUTPUT, "dr", HW_H_GR, CGEN_MODE_SI, OP_ENT (DR), 0, 0 },
  { END }
};

static const CGEN_OPINST fmt_ldi8_ops[] = {
  { INPUT, "simm8", HW_H_SINT, CGEN_MODE_INT, OP_ENT (SIMM8), 0, 0 },
  { OUTPUT, "dr", HW_H_GR, CGEN_MODE_SI, OP_ENT (DR), 0, 0 },
  { END }
};

static const CGEN_OPINST fmt_ldi16_ops[] = {
  { INPUT, "slo16", HW_H_SLO16, CGEN_MODE_INT, OP_ENT (SLO16), 0, 0 },
  { OUTPUT, "dr", HW_H_GR, CGEN_MODE_SI, OP_ENT (DR), 0, 0 },
  { END }
};

static const CGEN_OPINST fmt_lock_ops[] = {
  { INPUT, "h_memory_sr", HW_H_MEMORY, CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "sr", HW_H_GR, CGEN_MODE_USI, OP_ENT (SR), 0, 0 },
  { OUTPUT, "dr", HW_H_GR, CGEN_MODE_SI, OP_ENT (DR), 0, 0 },
  { OUTPUT, "h_lock", HW_H_LOCK, CGEN_MODE_BI, 0, 0, 0 },
  { END }
};

static const CGEN_OPINST fmt_machi_ops[] = {
  { INPUT, "accum", HW_H_ACCUM, CGEN_MODE_DI, 0, 0, 0 },
  { INPUT, "src1", HW_H_GR, CGEN_MODE_SI, OP_ENT (SRC1), 0, 0 },
  { INPUT, "src2", HW_H_GR, CGEN_MODE_SI, OP_ENT (SRC2), 0, 0 },
  { OUTPUT, "accum", HW_H_ACCUM, CGEN_MODE_DI, 0, 0, 0 },
  { END }
};

static const CGEN_OPINST fmt_mulhi_ops[] = {
  { INPUT, "src1", HW_H_GR, CGEN_MODE_SI, OP_ENT (SRC1), 0, 0 },
  { INPUT, "src2", HW_H_GR, CGEN_MODE_SI, OP_ENT (SRC2), 0, 0 },
  { OUTPUT, "accum", HW_H_ACCUM, CGEN_MODE_DI, 0, 0, 0 },
  { END }
};

static const CGEN_OPINST fmt_mv_ops[] = {
  { INPUT, "sr", HW_H_GR, CGEN_MODE_SI, OP_ENT (SR), 0, 0 },
  { OUTPUT, "dr", HW_H_GR, CGEN_MODE_SI, OP_ENT (DR), 0, 0 },
  { END }
};

static const CGEN_OPINST fmt_mvfachi_ops[] = {
  { INPUT, "accum", HW_H_ACCUM, CGEN_MODE_DI, 0, 0, 0 },
  { OUTPUT, "dr", HW_H_GR, CGEN_MODE_SI, OP_ENT (DR), 0, 0 },
  { END }
};

static const CGEN_OPINST fmt_mvfc_ops[] = {
  { INPUT, "scr", HW_H_CR, CGEN_MODE_USI, OP_ENT (SCR), 0, 0 },
  { OUTPUT, "dr", HW_H_GR, CGEN_MODE_SI, OP_ENT (DR), 0, 0 },
  { END }
};

static const CGEN_OPINST fmt_mvtachi_ops[] = {
  { INPUT, "accum", HW_H_ACCUM, CGEN_MODE_DI, 0, 0, 0 },
  { INPUT, "src1", HW_H_GR, CGEN_MODE_SI, OP_ENT (SRC1), 0, 0 },
  { OUTPUT, "accum", HW_H_ACCUM, CGEN_MODE_DI, 0, 0, 0 },
  { END }
};

static const CGEN_OPINST fmt_mvtc_ops[] = {
  { INPUT, "sr", HW_H_GR, CGEN_MODE_SI, OP_ENT (SR), 0, 0 },
  { OUTPUT, "dcr", HW_H_CR, CGEN_MODE_USI, OP_ENT (DCR), 0, 0 },
  { END }
};

static const CGEN_OPINST fmt_nop_ops[] = {
  { END }
};

static const CGEN_OPINST fmt_rac_ops[] = {
  { INPUT, "accum", HW_H_ACCUM, CGEN_MODE_DI, 0, 0, 0 },
  { OUTPUT, "accum", HW_H_ACCUM, CGEN_MODE_DI, 0, 0, 0 },
  { END }
};

static const CGEN_OPINST fmt_rte_ops[] = {
  { INPUT, "h_bbpsw", HW_H_BBPSW, CGEN_MODE_UQI, 0, 0, 0 },
  { INPUT, "h_bpsw", HW_H_BPSW, CGEN_MODE_UQI, 0, 0, 0 },
  { INPUT, "h_cr_14", HW_H_CR, CGEN_MODE_USI, 0, 14, 0 },
  { INPUT, "h_cr_6", HW_H_CR, CGEN_MODE_USI, 0, 6, 0 },
  { OUTPUT, "h_bpsw", HW_H_BPSW, CGEN_MODE_UQI, 0, 0, 0 },
  { OUTPUT, "h_cr_6", HW_H_CR, CGEN_MODE_USI, 0, 6, 0 },
  { OUTPUT, "h_psw", HW_H_PSW, CGEN_MODE_UQI, 0, 0, 0 },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, 0 },
  { END }
};

static const CGEN_OPINST fmt_seth_ops[] = {
  { INPUT, "hi16", HW_H_HI16, CGEN_MODE_SI, OP_ENT (HI16), 0, 0 },
  { OUTPUT, "dr", HW_H_GR, CGEN_MODE_SI, OP_ENT (DR), 0, 0 },
  { END }
};

static const CGEN_OPINST fmt_sll3_ops[] = {
  { INPUT, "simm16", HW_H_SINT, CGEN_MODE_SI, OP_ENT (SIMM16), 0, 0 },
  { INPUT, "sr", HW_H_GR, CGEN_MODE_SI, OP_ENT (SR), 0, 0 },
  { OUTPUT, "dr", HW_H_GR, CGEN_MODE_SI, OP_ENT (DR), 0, 0 },
  { END }
};

static const CGEN_OPINST fmt_slli_ops[] = {
  { INPUT, "dr", HW_H_GR, CGEN_MODE_SI, OP_ENT (DR), 0, 0 },
  { INPUT, "uimm5", HW_H_UINT, CGEN_MODE_INT, OP_ENT (UIMM5), 0, 0 },
  { OUTPUT, "dr", HW_H_GR, CGEN_MODE_SI, OP_ENT (DR), 0, 0 },
  { END }
};

static const CGEN_OPINST fmt_st_ops[] = {
  { INPUT, "src1", HW_H_GR, CGEN_MODE_SI, OP_ENT (SRC1), 0, 0 },
  { INPUT, "src2", HW_H_GR, CGEN_MODE_USI, OP_ENT (SRC2), 0, 0 },
  { OUTPUT, "h_memory_src2", HW_H_MEMORY, CGEN_MODE_SI, 0, 0, 0 },
  { END }
};

static const CGEN_OPINST fmt_st_d_ops[] = {
  { INPUT, "slo16", HW_H_SLO16, CGEN_MODE_INT, OP_ENT (SLO16), 0, 0 },
  { INPUT, "src1", HW_H_GR, CGEN_MODE_SI, OP_ENT (SRC1), 0, 0 },
  { INPUT, "src2", HW_H_GR, CGEN_MODE_SI, OP_ENT (SRC2), 0, 0 },
  { OUTPUT, "h_memory_add__VM_src2_slo16", HW_H_MEMORY, CGEN_MODE_SI, 0, 0, 0 },
  { END }
};

static const CGEN_OPINST fmt_stb_ops[] = {
  { INPUT, "src1", HW_H_GR, CGEN_MODE_QI, OP_ENT (SRC1), 0, 0 },
  { INPUT, "src2", HW_H_GR, CGEN_MODE_USI, OP_ENT (SRC2), 0, 0 },
  { OUTPUT, "h_memory_src2", HW_H_MEMORY, CGEN_MODE_QI, 0, 0, 0 },
  { END }
};

static const CGEN_OPINST fmt_stb_d_ops[] = {
  { INPUT, "slo16", HW_H_SLO16, CGEN_MODE_INT, OP_ENT (SLO16), 0, 0 },
  { INPUT, "src1", HW_H_GR, CGEN_MODE_QI, OP_ENT (SRC1), 0, 0 },
  { INPUT, "src2", HW_H_GR, CGEN_MODE_SI, OP_ENT (SRC2), 0, 0 },
  { OUTPUT, "h_memory_add__VM_src2_slo16", HW_H_MEMORY, CGEN_MODE_QI, 0, 0, 0 },
  { END }
};

static const CGEN_OPINST fmt_sth_ops[] = {
  { INPUT, "src1", HW_H_GR, CGEN_MODE_HI, OP_ENT (SRC1), 0, 0 },
  { INPUT, "src2", HW_H_GR, CGEN_MODE_USI, OP_ENT (SRC2), 0, 0 },
  { OUTPUT, "h_memory_src2", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, 0 },
  { END }
};

static const CGEN_OPINST fmt_sth_d_ops[] = {
  { INPUT, "slo16", HW_H_SLO16, CGEN_MODE_INT, OP_ENT (SLO16), 0, 0 },
  { INPUT, "src1", HW_H_GR, CGEN_MODE_HI, OP_ENT (SRC1), 0, 0 },
  { INPUT, "src2", HW_H_GR, CGEN_MODE_SI, OP_ENT (SRC2), 0, 0 },
  { OUTPUT, "h_memory_add__VM_src2_slo16", HW_H_MEMORY, CGEN_MODE_HI, 0, 0, 0 },
  { END }
};

static const CGEN_OPINST fmt_st_plus_ops[] = {
  { INPUT, "src1", HW_H_GR, CGEN_MODE_SI, OP_ENT (SRC1), 0, 0 },
  { INPUT, "src2", HW_H_GR, CGEN_MODE_SI, OP_ENT (SRC2), 0, 0 },
  { OUTPUT, "h_memory_new_src2", HW_H_MEMORY, CGEN_MODE_SI, 0, 0, 0 },
  { OUTPUT, "src2", HW_H_GR, CGEN_MODE_SI, OP_ENT (SRC2), 0, 0 },
  { END }
};

static const CGEN_OPINST fmt_trap_ops[] = {
  { INPUT, "h_bpsw", HW_H_BPSW, CGEN_MODE_UQI, 0, 0, 0 },
  { INPUT, "h_cr_6", HW_H_CR, CGEN_MODE_USI, 0, 6, 0 },
  { INPUT, "h_psw", HW_H_PSW, CGEN_MODE_UQI, 0, 0, 0 },
  { INPUT, "pc", HW_H_PC, CGEN_MODE_USI, 0, 0, 0 },
  { INPUT, "uimm4", HW_H_UINT, CGEN_MODE_UINT, OP_ENT (UIMM4), 0, 0 },
  { OUTPUT, "h_bbpsw", HW_H_BBPSW, CGEN_MODE_UQI, 0, 0, 0 },
  { OUTPUT, "h_bpsw", HW_H_BPSW, CGEN_MODE_UQI, 0, 0, 0 },
  { OUTPUT, "h_cr_14", HW_H_CR, CGEN_MODE_USI, 0, 14, 0 },
  { OUTPUT, "h_cr_6", HW_H_CR, CGEN_MODE_USI, 0, 6, 0 },
  { OUTPUT, "h_psw", HW_H_PSW, CGEN_MODE_UQI, 0, 0, 0 },
  { OUTPUT, "pc", HW_H_PC, CGEN_MODE_SI, 0, 0, 0 },
  { END }
};

static const CGEN_OPINST fmt_unlock_ops[] = {
  { INPUT, "h_lock", HW_H_LOCK, CGEN_MODE_BI, 0, 0, 0 },
  { INPUT, "src1", HW_H_GR, CGEN_MODE_SI, OP_ENT (SRC1), 0, COND_REF },
  { INPUT, "src2", HW_H_GR, CGEN_MODE_USI, OP_ENT (SRC2), 0, COND_REF },
  { OUTPUT, "h_lock", HW_H_LOCK, CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "h_memory_src2", HW_H_MEMORY, CGEN_MODE_SI, 0, 0, COND_REF },
  { END }
};

#undef INPUT
#undef OUTPUT
#undef END
#undef COND_REF
#undef OP_ENT

/* Operand instance lookup table.  */

static const CGEN_OPINST *m32r_cgen_opinst_table[MAX_INSNS] = {
  0,
  & fmt_add_ops[0],
  & fmt_add3_ops[0],
  & fmt_add_ops[0],
  & fmt_and3_ops[0],
  & fmt_add_ops[0],
  & fmt_or3_ops[0],
  & fmt_add_ops[0],
  & fmt_and3_ops[0],
  & fmt_addi_ops[0],
  & fmt_addv_ops[0],
  & fmt_addv3_ops[0],
  & fmt_addx_ops[0],
  & fmt_bc8_ops[0],
  & fmt_bc24_ops[0],
  & fmt_beq_ops[0],
  & fmt_beqz_ops[0],
  & fmt_beqz_ops[0],
  & fmt_beqz_ops[0],
  & fmt_beqz_ops[0],
  & fmt_beqz_ops[0],
  & fmt_beqz_ops[0],
  & fmt_bl8_ops[0],
  & fmt_bl24_ops[0],
  & fmt_bc8_ops[0],
  & fmt_bc24_ops[0],
  & fmt_beq_ops[0],
  & fmt_bra8_ops[0],
  & fmt_bra24_ops[0],
  & fmt_cmp_ops[0],
  & fmt_cmpi_ops[0],
  & fmt_cmp_ops[0],
  & fmt_cmpi_ops[0],
  & fmt_div_ops[0],
  & fmt_div_ops[0],
  & fmt_div_ops[0],
  & fmt_div_ops[0],
  & fmt_jl_ops[0],
  & fmt_jmp_ops[0],
  & fmt_ld_ops[0],
  & fmt_ld_d_ops[0],
  & fmt_ldb_ops[0],
  & fmt_ldb_d_ops[0],
  & fmt_ldh_ops[0],
  & fmt_ldh_d_ops[0],
  & fmt_ldb_ops[0],
  & fmt_ldb_d_ops[0],
  & fmt_ldh_ops[0],
  & fmt_ldh_d_ops[0],
  & fmt_ld_plus_ops[0],
  & fmt_ld24_ops[0],
  & fmt_ldi8_ops[0],
  & fmt_ldi16_ops[0],
  & fmt_lock_ops[0],
  & fmt_machi_ops[0],
  & fmt_machi_ops[0],
  & fmt_machi_ops[0],
  & fmt_machi_ops[0],
  & fmt_add_ops[0],
  & fmt_mulhi_ops[0],
  & fmt_mulhi_ops[0],
  & fmt_mulhi_ops[0],
  & fmt_mulhi_ops[0],
  & fmt_mv_ops[0],
  & fmt_mvfachi_ops[0],
  & fmt_mvfachi_ops[0],
  & fmt_mvfachi_ops[0],
  & fmt_mvfc_ops[0],
  & fmt_mvtachi_ops[0],
  & fmt_mvtachi_ops[0],
  & fmt_mvtc_ops[0],
  & fmt_mv_ops[0],
  & fmt_nop_ops[0],
  & fmt_mv_ops[0],
  & fmt_rac_ops[0],
  & fmt_rac_ops[0],
  & fmt_rte_ops[0],
  & fmt_seth_ops[0],
  & fmt_add_ops[0],
  & fmt_sll3_ops[0],
  & fmt_slli_ops[0],
  & fmt_add_ops[0],
  & fmt_sll3_ops[0],
  & fmt_slli_ops[0],
  & fmt_add_ops[0],
  & fmt_sll3_ops[0],
  & fmt_slli_ops[0],
  & fmt_st_ops[0],
  & fmt_st_d_ops[0],
  & fmt_stb_ops[0],
  & fmt_stb_d_ops[0],
  & fmt_sth_ops[0],
  & fmt_sth_d_ops[0],
  & fmt_st_plus_ops[0],
  & fmt_st_plus_ops[0],
  & fmt_add_ops[0],
  & fmt_addv_ops[0],
  & fmt_addx_ops[0],
  & fmt_trap_ops[0],
  & fmt_unlock_ops[0],
};

/* Function to call before using the operand instance table.  */

void
m32r_cgen_init_opinst_table (cd)
     CGEN_CPU_DESC cd;
{
  int i;
  const CGEN_OPINST **oi = & m32r_cgen_opinst_table[0];
  CGEN_INSN *insns = (CGEN_INSN *) cd->insn_table.init_entries;
  for (i = 0; i < MAX_INSNS; ++i)
    insns[i].opinst = oi[i];
}
