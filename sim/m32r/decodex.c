/* Simulator instruction decoder for m32rxf.

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

#define WANT_CPU m32rxf
#define WANT_CPU_M32RXF

#include "sim-main.h"
#include "sim-assert.h"

#ifdef __GNUC__
#define FMT(n)
#else
#define FMT(n) CONCAT2 (M32RXF_,n) ,
#endif

/* FIXME: Need to review choices for the following.  */

#if WITH_SEM_SWITCH_FULL
#define FULL(fn)
#else
#define FULL(fn) CONCAT3 (m32rxf,_sem_,fn) ,
#endif

#if WITH_FAST
#if WITH_SEM_SWITCH_FAST
#define FAST(fn)
#else
#define FAST(fn) CONCAT3 (m32rxf,_semf_,fn) , /* f for fast */
#endif
#else
#define FAST(fn)
#endif

/* The instruction descriptor array.
   This is computed at runtime.  Space for it is not malloc'd to save a
   teensy bit of cpu in the decoder.  Moving it to malloc space is trivial
   but won't be done until necessary (we don't currently support the runtime
   addition of instructions nor an SMP machine with different cpus).  */
static IDESC m32rxf_insn_data[M32RXF_INSN_MAX];

/* Instruction semantic handlers and support.
   This struct defines the part of an IDESC that can be computed at
   compile time.  */

struct insn_sem {
  /* The instruction type (a number that identifies each insn over the
     entire architecture).  */
  CGEN_INSN_TYPE type;

  /* Index in IDESC table.  */
  int index;

  /* Index in IDESC table of parallel handler.  */
  int par_index;

  /* Index in IDESC table of writeback handler.  */
  int write_index;

  /* Routines to execute the insn.
     The full version has all features (profiling,tracing) compiled in.
     The fast version has none of that.  */
#if ! WITH_SEM_SWITCH_FULL
  SEMANTIC_FN *sem_full;
#endif
#if WITH_FAST && ! WITH_SEM_SWITCH_FAST
  SEMANTIC_FN *sem_fast;
#endif

};
/* The INSN_ prefix is not here and is instead part of the `insn' argument
   to avoid collisions with header files (e.g. `AND' in ansidecl.h).  */
#define IDX(insn) CONCAT2 (M32RXF_,insn)
#define TYPE(insn) CONCAT2 (M32R_,insn)

/* Insn can't be executed in parallel.
   Or is that "do NOt Pass to Air defense Radar"? :-) */
#define NOPAR (-1)

/* Commas between elements are contained in the macros.
   Some of these are conditionally compiled out.  */

static const struct insn_sem m32rxf_insn_sem[] =
{
  { VIRTUAL_INSN_X_INVALID, IDX (INSN_X_INVALID), NOPAR, NOPAR, FULL (x_invalid) FAST (x_invalid) },
  { VIRTUAL_INSN_X_AFTER, IDX (INSN_X_AFTER), NOPAR, NOPAR, FULL (x_after) FAST (x_after) },
  { VIRTUAL_INSN_X_BEFORE, IDX (INSN_X_BEFORE), NOPAR, NOPAR, FULL (x_before) FAST (x_before) },
  { VIRTUAL_INSN_X_CTI_CHAIN, IDX (INSN_X_CTI_CHAIN), NOPAR, NOPAR, FULL (x_cti_chain) FAST (x_cti_chain) },
  { VIRTUAL_INSN_X_CHAIN, IDX (INSN_X_CHAIN), NOPAR, NOPAR, FULL (x_chain) FAST (x_chain) },
  { VIRTUAL_INSN_X_BEGIN, IDX (INSN_X_BEGIN), NOPAR, NOPAR, FULL (x_begin) FAST (x_begin) },
  { TYPE (INSN_ADD), IDX (INSN_ADD), IDX (INSN_PAR_ADD), IDX (INSN_WRITE_ADD), FULL (add) FAST (add) },
  { TYPE (INSN_ADD3), IDX (INSN_ADD3), NOPAR, NOPAR, FULL (add3) FAST (add3) },
  { TYPE (INSN_AND), IDX (INSN_AND), IDX (INSN_PAR_AND), IDX (INSN_WRITE_AND), FULL (and) FAST (and) },
  { TYPE (INSN_AND3), IDX (INSN_AND3), NOPAR, NOPAR, FULL (and3) FAST (and3) },
  { TYPE (INSN_OR), IDX (INSN_OR), IDX (INSN_PAR_OR), IDX (INSN_WRITE_OR), FULL (or) FAST (or) },
  { TYPE (INSN_OR3), IDX (INSN_OR3), NOPAR, NOPAR, FULL (or3) FAST (or3) },
  { TYPE (INSN_XOR), IDX (INSN_XOR), IDX (INSN_PAR_XOR), IDX (INSN_WRITE_XOR), FULL (xor) FAST (xor) },
  { TYPE (INSN_XOR3), IDX (INSN_XOR3), NOPAR, NOPAR, FULL (xor3) FAST (xor3) },
  { TYPE (INSN_ADDI), IDX (INSN_ADDI), IDX (INSN_PAR_ADDI), IDX (INSN_WRITE_ADDI), FULL (addi) FAST (addi) },
  { TYPE (INSN_ADDV), IDX (INSN_ADDV), IDX (INSN_PAR_ADDV), IDX (INSN_WRITE_ADDV), FULL (addv) FAST (addv) },
  { TYPE (INSN_ADDV3), IDX (INSN_ADDV3), NOPAR, NOPAR, FULL (addv3) FAST (addv3) },
  { TYPE (INSN_ADDX), IDX (INSN_ADDX), IDX (INSN_PAR_ADDX), IDX (INSN_WRITE_ADDX), FULL (addx) FAST (addx) },
  { TYPE (INSN_BC8), IDX (INSN_BC8), IDX (INSN_PAR_BC8), IDX (INSN_WRITE_BC8), FULL (bc8) FAST (bc8) },
  { TYPE (INSN_BC24), IDX (INSN_BC24), NOPAR, NOPAR, FULL (bc24) FAST (bc24) },
  { TYPE (INSN_BEQ), IDX (INSN_BEQ), NOPAR, NOPAR, FULL (beq) FAST (beq) },
  { TYPE (INSN_BEQZ), IDX (INSN_BEQZ), NOPAR, NOPAR, FULL (beqz) FAST (beqz) },
  { TYPE (INSN_BGEZ), IDX (INSN_BGEZ), NOPAR, NOPAR, FULL (bgez) FAST (bgez) },
  { TYPE (INSN_BGTZ), IDX (INSN_BGTZ), NOPAR, NOPAR, FULL (bgtz) FAST (bgtz) },
  { TYPE (INSN_BLEZ), IDX (INSN_BLEZ), NOPAR, NOPAR, FULL (blez) FAST (blez) },
  { TYPE (INSN_BLTZ), IDX (INSN_BLTZ), NOPAR, NOPAR, FULL (bltz) FAST (bltz) },
  { TYPE (INSN_BNEZ), IDX (INSN_BNEZ), NOPAR, NOPAR, FULL (bnez) FAST (bnez) },
  { TYPE (INSN_BL8), IDX (INSN_BL8), IDX (INSN_PAR_BL8), IDX (INSN_WRITE_BL8), FULL (bl8) FAST (bl8) },
  { TYPE (INSN_BL24), IDX (INSN_BL24), NOPAR, NOPAR, FULL (bl24) FAST (bl24) },
  { TYPE (INSN_BCL8), IDX (INSN_BCL8), IDX (INSN_PAR_BCL8), IDX (INSN_WRITE_BCL8), FULL (bcl8) FAST (bcl8) },
  { TYPE (INSN_BCL24), IDX (INSN_BCL24), NOPAR, NOPAR, FULL (bcl24) FAST (bcl24) },
  { TYPE (INSN_BNC8), IDX (INSN_BNC8), IDX (INSN_PAR_BNC8), IDX (INSN_WRITE_BNC8), FULL (bnc8) FAST (bnc8) },
  { TYPE (INSN_BNC24), IDX (INSN_BNC24), NOPAR, NOPAR, FULL (bnc24) FAST (bnc24) },
  { TYPE (INSN_BNE), IDX (INSN_BNE), NOPAR, NOPAR, FULL (bne) FAST (bne) },
  { TYPE (INSN_BRA8), IDX (INSN_BRA8), IDX (INSN_PAR_BRA8), IDX (INSN_WRITE_BRA8), FULL (bra8) FAST (bra8) },
  { TYPE (INSN_BRA24), IDX (INSN_BRA24), NOPAR, NOPAR, FULL (bra24) FAST (bra24) },
  { TYPE (INSN_BNCL8), IDX (INSN_BNCL8), IDX (INSN_PAR_BNCL8), IDX (INSN_WRITE_BNCL8), FULL (bncl8) FAST (bncl8) },
  { TYPE (INSN_BNCL24), IDX (INSN_BNCL24), NOPAR, NOPAR, FULL (bncl24) FAST (bncl24) },
  { TYPE (INSN_CMP), IDX (INSN_CMP), IDX (INSN_PAR_CMP), IDX (INSN_WRITE_CMP), FULL (cmp) FAST (cmp) },
  { TYPE (INSN_CMPI), IDX (INSN_CMPI), NOPAR, NOPAR, FULL (cmpi) FAST (cmpi) },
  { TYPE (INSN_CMPU), IDX (INSN_CMPU), IDX (INSN_PAR_CMPU), IDX (INSN_WRITE_CMPU), FULL (cmpu) FAST (cmpu) },
  { TYPE (INSN_CMPUI), IDX (INSN_CMPUI), NOPAR, NOPAR, FULL (cmpui) FAST (cmpui) },
  { TYPE (INSN_CMPEQ), IDX (INSN_CMPEQ), IDX (INSN_PAR_CMPEQ), IDX (INSN_WRITE_CMPEQ), FULL (cmpeq) FAST (cmpeq) },
  { TYPE (INSN_CMPZ), IDX (INSN_CMPZ), IDX (INSN_PAR_CMPZ), IDX (INSN_WRITE_CMPZ), FULL (cmpz) FAST (cmpz) },
  { TYPE (INSN_DIV), IDX (INSN_DIV), NOPAR, NOPAR, FULL (div) FAST (div) },
  { TYPE (INSN_DIVU), IDX (INSN_DIVU), NOPAR, NOPAR, FULL (divu) FAST (divu) },
  { TYPE (INSN_REM), IDX (INSN_REM), NOPAR, NOPAR, FULL (rem) FAST (rem) },
  { TYPE (INSN_REMU), IDX (INSN_REMU), NOPAR, NOPAR, FULL (remu) FAST (remu) },
  { TYPE (INSN_DIVH), IDX (INSN_DIVH), NOPAR, NOPAR, FULL (divh) FAST (divh) },
  { TYPE (INSN_JC), IDX (INSN_JC), IDX (INSN_PAR_JC), IDX (INSN_WRITE_JC), FULL (jc) FAST (jc) },
  { TYPE (INSN_JNC), IDX (INSN_JNC), IDX (INSN_PAR_JNC), IDX (INSN_WRITE_JNC), FULL (jnc) FAST (jnc) },
  { TYPE (INSN_JL), IDX (INSN_JL), IDX (INSN_PAR_JL), IDX (INSN_WRITE_JL), FULL (jl) FAST (jl) },
  { TYPE (INSN_JMP), IDX (INSN_JMP), IDX (INSN_PAR_JMP), IDX (INSN_WRITE_JMP), FULL (jmp) FAST (jmp) },
  { TYPE (INSN_LD), IDX (INSN_LD), IDX (INSN_PAR_LD), IDX (INSN_WRITE_LD), FULL (ld) FAST (ld) },
  { TYPE (INSN_LD_D), IDX (INSN_LD_D), NOPAR, NOPAR, FULL (ld_d) FAST (ld_d) },
  { TYPE (INSN_LDB), IDX (INSN_LDB), IDX (INSN_PAR_LDB), IDX (INSN_WRITE_LDB), FULL (ldb) FAST (ldb) },
  { TYPE (INSN_LDB_D), IDX (INSN_LDB_D), NOPAR, NOPAR, FULL (ldb_d) FAST (ldb_d) },
  { TYPE (INSN_LDH), IDX (INSN_LDH), IDX (INSN_PAR_LDH), IDX (INSN_WRITE_LDH), FULL (ldh) FAST (ldh) },
  { TYPE (INSN_LDH_D), IDX (INSN_LDH_D), NOPAR, NOPAR, FULL (ldh_d) FAST (ldh_d) },
  { TYPE (INSN_LDUB), IDX (INSN_LDUB), IDX (INSN_PAR_LDUB), IDX (INSN_WRITE_LDUB), FULL (ldub) FAST (ldub) },
  { TYPE (INSN_LDUB_D), IDX (INSN_LDUB_D), NOPAR, NOPAR, FULL (ldub_d) FAST (ldub_d) },
  { TYPE (INSN_LDUH), IDX (INSN_LDUH), IDX (INSN_PAR_LDUH), IDX (INSN_WRITE_LDUH), FULL (lduh) FAST (lduh) },
  { TYPE (INSN_LDUH_D), IDX (INSN_LDUH_D), NOPAR, NOPAR, FULL (lduh_d) FAST (lduh_d) },
  { TYPE (INSN_LD_PLUS), IDX (INSN_LD_PLUS), IDX (INSN_PAR_LD_PLUS), IDX (INSN_WRITE_LD_PLUS), FULL (ld_plus) FAST (ld_plus) },
  { TYPE (INSN_LD24), IDX (INSN_LD24), NOPAR, NOPAR, FULL (ld24) FAST (ld24) },
  { TYPE (INSN_LDI8), IDX (INSN_LDI8), IDX (INSN_PAR_LDI8), IDX (INSN_WRITE_LDI8), FULL (ldi8) FAST (ldi8) },
  { TYPE (INSN_LDI16), IDX (INSN_LDI16), NOPAR, NOPAR, FULL (ldi16) FAST (ldi16) },
  { TYPE (INSN_LOCK), IDX (INSN_LOCK), IDX (INSN_PAR_LOCK), IDX (INSN_WRITE_LOCK), FULL (lock) FAST (lock) },
  { TYPE (INSN_MACHI_A), IDX (INSN_MACHI_A), IDX (INSN_PAR_MACHI_A), IDX (INSN_WRITE_MACHI_A), FULL (machi_a) FAST (machi_a) },
  { TYPE (INSN_MACLO_A), IDX (INSN_MACLO_A), IDX (INSN_PAR_MACLO_A), IDX (INSN_WRITE_MACLO_A), FULL (maclo_a) FAST (maclo_a) },
  { TYPE (INSN_MACWHI_A), IDX (INSN_MACWHI_A), IDX (INSN_PAR_MACWHI_A), IDX (INSN_WRITE_MACWHI_A), FULL (macwhi_a) FAST (macwhi_a) },
  { TYPE (INSN_MACWLO_A), IDX (INSN_MACWLO_A), IDX (INSN_PAR_MACWLO_A), IDX (INSN_WRITE_MACWLO_A), FULL (macwlo_a) FAST (macwlo_a) },
  { TYPE (INSN_MUL), IDX (INSN_MUL), IDX (INSN_PAR_MUL), IDX (INSN_WRITE_MUL), FULL (mul) FAST (mul) },
  { TYPE (INSN_MULHI_A), IDX (INSN_MULHI_A), IDX (INSN_PAR_MULHI_A), IDX (INSN_WRITE_MULHI_A), FULL (mulhi_a) FAST (mulhi_a) },
  { TYPE (INSN_MULLO_A), IDX (INSN_MULLO_A), IDX (INSN_PAR_MULLO_A), IDX (INSN_WRITE_MULLO_A), FULL (mullo_a) FAST (mullo_a) },
  { TYPE (INSN_MULWHI_A), IDX (INSN_MULWHI_A), IDX (INSN_PAR_MULWHI_A), IDX (INSN_WRITE_MULWHI_A), FULL (mulwhi_a) FAST (mulwhi_a) },
  { TYPE (INSN_MULWLO_A), IDX (INSN_MULWLO_A), IDX (INSN_PAR_MULWLO_A), IDX (INSN_WRITE_MULWLO_A), FULL (mulwlo_a) FAST (mulwlo_a) },
  { TYPE (INSN_MV), IDX (INSN_MV), IDX (INSN_PAR_MV), IDX (INSN_WRITE_MV), FULL (mv) FAST (mv) },
  { TYPE (INSN_MVFACHI_A), IDX (INSN_MVFACHI_A), IDX (INSN_PAR_MVFACHI_A), IDX (INSN_WRITE_MVFACHI_A), FULL (mvfachi_a) FAST (mvfachi_a) },
  { TYPE (INSN_MVFACLO_A), IDX (INSN_MVFACLO_A), IDX (INSN_PAR_MVFACLO_A), IDX (INSN_WRITE_MVFACLO_A), FULL (mvfaclo_a) FAST (mvfaclo_a) },
  { TYPE (INSN_MVFACMI_A), IDX (INSN_MVFACMI_A), IDX (INSN_PAR_MVFACMI_A), IDX (INSN_WRITE_MVFACMI_A), FULL (mvfacmi_a) FAST (mvfacmi_a) },
  { TYPE (INSN_MVFC), IDX (INSN_MVFC), IDX (INSN_PAR_MVFC), IDX (INSN_WRITE_MVFC), FULL (mvfc) FAST (mvfc) },
  { TYPE (INSN_MVTACHI_A), IDX (INSN_MVTACHI_A), IDX (INSN_PAR_MVTACHI_A), IDX (INSN_WRITE_MVTACHI_A), FULL (mvtachi_a) FAST (mvtachi_a) },
  { TYPE (INSN_MVTACLO_A), IDX (INSN_MVTACLO_A), IDX (INSN_PAR_MVTACLO_A), IDX (INSN_WRITE_MVTACLO_A), FULL (mvtaclo_a) FAST (mvtaclo_a) },
  { TYPE (INSN_MVTC), IDX (INSN_MVTC), IDX (INSN_PAR_MVTC), IDX (INSN_WRITE_MVTC), FULL (mvtc) FAST (mvtc) },
  { TYPE (INSN_NEG), IDX (INSN_NEG), IDX (INSN_PAR_NEG), IDX (INSN_WRITE_NEG), FULL (neg) FAST (neg) },
  { TYPE (INSN_NOP), IDX (INSN_NOP), IDX (INSN_PAR_NOP), IDX (INSN_WRITE_NOP), FULL (nop) FAST (nop) },
  { TYPE (INSN_NOT), IDX (INSN_NOT), IDX (INSN_PAR_NOT), IDX (INSN_WRITE_NOT), FULL (not) FAST (not) },
  { TYPE (INSN_RAC_DSI), IDX (INSN_RAC_DSI), IDX (INSN_PAR_RAC_DSI), IDX (INSN_WRITE_RAC_DSI), FULL (rac_dsi) FAST (rac_dsi) },
  { TYPE (INSN_RACH_DSI), IDX (INSN_RACH_DSI), IDX (INSN_PAR_RACH_DSI), IDX (INSN_WRITE_RACH_DSI), FULL (rach_dsi) FAST (rach_dsi) },
  { TYPE (INSN_RTE), IDX (INSN_RTE), IDX (INSN_PAR_RTE), IDX (INSN_WRITE_RTE), FULL (rte) FAST (rte) },
  { TYPE (INSN_SETH), IDX (INSN_SETH), NOPAR, NOPAR, FULL (seth) FAST (seth) },
  { TYPE (INSN_SLL), IDX (INSN_SLL), IDX (INSN_PAR_SLL), IDX (INSN_WRITE_SLL), FULL (sll) FAST (sll) },
  { TYPE (INSN_SLL3), IDX (INSN_SLL3), NOPAR, NOPAR, FULL (sll3) FAST (sll3) },
  { TYPE (INSN_SLLI), IDX (INSN_SLLI), IDX (INSN_PAR_SLLI), IDX (INSN_WRITE_SLLI), FULL (slli) FAST (slli) },
  { TYPE (INSN_SRA), IDX (INSN_SRA), IDX (INSN_PAR_SRA), IDX (INSN_WRITE_SRA), FULL (sra) FAST (sra) },
  { TYPE (INSN_SRA3), IDX (INSN_SRA3), NOPAR, NOPAR, FULL (sra3) FAST (sra3) },
  { TYPE (INSN_SRAI), IDX (INSN_SRAI), IDX (INSN_PAR_SRAI), IDX (INSN_WRITE_SRAI), FULL (srai) FAST (srai) },
  { TYPE (INSN_SRL), IDX (INSN_SRL), IDX (INSN_PAR_SRL), IDX (INSN_WRITE_SRL), FULL (srl) FAST (srl) },
  { TYPE (INSN_SRL3), IDX (INSN_SRL3), NOPAR, NOPAR, FULL (srl3) FAST (srl3) },
  { TYPE (INSN_SRLI), IDX (INSN_SRLI), IDX (INSN_PAR_SRLI), IDX (INSN_WRITE_SRLI), FULL (srli) FAST (srli) },
  { TYPE (INSN_ST), IDX (INSN_ST), IDX (INSN_PAR_ST), IDX (INSN_WRITE_ST), FULL (st) FAST (st) },
  { TYPE (INSN_ST_D), IDX (INSN_ST_D), NOPAR, NOPAR, FULL (st_d) FAST (st_d) },
  { TYPE (INSN_STB), IDX (INSN_STB), IDX (INSN_PAR_STB), IDX (INSN_WRITE_STB), FULL (stb) FAST (stb) },
  { TYPE (INSN_STB_D), IDX (INSN_STB_D), NOPAR, NOPAR, FULL (stb_d) FAST (stb_d) },
  { TYPE (INSN_STH), IDX (INSN_STH), IDX (INSN_PAR_STH), IDX (INSN_WRITE_STH), FULL (sth) FAST (sth) },
  { TYPE (INSN_STH_D), IDX (INSN_STH_D), NOPAR, NOPAR, FULL (sth_d) FAST (sth_d) },
  { TYPE (INSN_ST_PLUS), IDX (INSN_ST_PLUS), IDX (INSN_PAR_ST_PLUS), IDX (INSN_WRITE_ST_PLUS), FULL (st_plus) FAST (st_plus) },
  { TYPE (INSN_ST_MINUS), IDX (INSN_ST_MINUS), IDX (INSN_PAR_ST_MINUS), IDX (INSN_WRITE_ST_MINUS), FULL (st_minus) FAST (st_minus) },
  { TYPE (INSN_SUB), IDX (INSN_SUB), IDX (INSN_PAR_SUB), IDX (INSN_WRITE_SUB), FULL (sub) FAST (sub) },
  { TYPE (INSN_SUBV), IDX (INSN_SUBV), IDX (INSN_PAR_SUBV), IDX (INSN_WRITE_SUBV), FULL (subv) FAST (subv) },
  { TYPE (INSN_SUBX), IDX (INSN_SUBX), IDX (INSN_PAR_SUBX), IDX (INSN_WRITE_SUBX), FULL (subx) FAST (subx) },
  { TYPE (INSN_TRAP), IDX (INSN_TRAP), IDX (INSN_PAR_TRAP), IDX (INSN_WRITE_TRAP), FULL (trap) FAST (trap) },
  { TYPE (INSN_UNLOCK), IDX (INSN_UNLOCK), IDX (INSN_PAR_UNLOCK), IDX (INSN_WRITE_UNLOCK), FULL (unlock) FAST (unlock) },
  { TYPE (INSN_SATB), IDX (INSN_SATB), NOPAR, NOPAR, FULL (satb) FAST (satb) },
  { TYPE (INSN_SATH), IDX (INSN_SATH), NOPAR, NOPAR, FULL (sath) FAST (sath) },
  { TYPE (INSN_SAT), IDX (INSN_SAT), NOPAR, NOPAR, FULL (sat) FAST (sat) },
  { TYPE (INSN_PCMPBZ), IDX (INSN_PCMPBZ), IDX (INSN_PAR_PCMPBZ), IDX (INSN_WRITE_PCMPBZ), FULL (pcmpbz) FAST (pcmpbz) },
  { TYPE (INSN_SADD), IDX (INSN_SADD), IDX (INSN_PAR_SADD), IDX (INSN_WRITE_SADD), FULL (sadd) FAST (sadd) },
  { TYPE (INSN_MACWU1), IDX (INSN_MACWU1), IDX (INSN_PAR_MACWU1), IDX (INSN_WRITE_MACWU1), FULL (macwu1) FAST (macwu1) },
  { TYPE (INSN_MSBLO), IDX (INSN_MSBLO), IDX (INSN_PAR_MSBLO), IDX (INSN_WRITE_MSBLO), FULL (msblo) FAST (msblo) },
  { TYPE (INSN_MULWU1), IDX (INSN_MULWU1), IDX (INSN_PAR_MULWU1), IDX (INSN_WRITE_MULWU1), FULL (mulwu1) FAST (mulwu1) },
  { TYPE (INSN_MACLH1), IDX (INSN_MACLH1), IDX (INSN_PAR_MACLH1), IDX (INSN_WRITE_MACLH1), FULL (maclh1) FAST (maclh1) },
  { TYPE (INSN_SC), IDX (INSN_SC), IDX (INSN_PAR_SC), IDX (INSN_WRITE_SC), FULL (sc) FAST (sc) },
  { TYPE (INSN_SNC), IDX (INSN_SNC), IDX (INSN_PAR_SNC), IDX (INSN_WRITE_SNC), FULL (snc) FAST (snc) },
};

static const struct insn_sem m32rxf_insn_sem_invalid =
{
  VIRTUAL_INSN_X_INVALID, IDX (INSN_X_INVALID), IDX (INSN_X_INVALID), 0 /*unused*/, FULL (x_invalid) FAST (x_invalid)
};

#undef IDX
#undef TYPE

/* Initialize an IDESC from the compile-time computable parts.  */

static INLINE void
init_idesc (SIM_CPU *cpu, IDESC *id, const struct insn_sem *t)
{
  const CGEN_INSN *opcode_table = m32r_cgen_insn_table_entries;

  id->num = t->index;
  if ((int) t->type <= 0)
    id->opcode = & cgen_virtual_opcode_table[- t->type];
  else
    id->opcode = & opcode_table[t->type];
#if ! WITH_SEM_SWITCH_FULL
  id->sem_full = t->sem_full;
#endif
#if WITH_FAST && ! WITH_SEM_SWITCH_FAST
  id->sem_fast = t->sem_fast;
#endif
#if WITH_PROFILE_MODEL_P
  id->timing = & MODEL_TIMING (CPU_MODEL (cpu)) [t->index];
  {
    SIM_DESC sd = CPU_STATE (cpu);
    SIM_ASSERT (t->index == id->timing->num);
  }
#endif
}

/* Initialize the instruction descriptor table.  */

void
m32rxf_init_idesc_table (SIM_CPU *cpu)
{
  IDESC *id,*tabend;
  const struct insn_sem *t,*tend;
  int tabsize = M32RXF_INSN_MAX;
  IDESC *table = m32rxf_insn_data;

  memset (table, 0, tabsize * sizeof (IDESC));

  /* First set all entries to the `invalid insn'.  */
  t = & m32rxf_insn_sem_invalid;
  for (id = table, tabend = table + tabsize; id < tabend; ++id)
    init_idesc (cpu, id, t);

  /* Now fill in the values for the chosen cpu.  */
  for (t = m32rxf_insn_sem, tend = t + sizeof (m32rxf_insn_sem) / sizeof (*t);
       t != tend; ++t)
    {
      init_idesc (cpu, & table[t->index], t);
      if (t->par_index != NOPAR)
	{
	  init_idesc (cpu, &table[t->par_index], t);
	  table[t->index].par_idesc = &table[t->par_index];
	}
      if (t->par_index != NOPAR)
	{
	  init_idesc (cpu, &table[t->write_index], t);
	  table[t->par_index].par_idesc = &table[t->write_index];
	}
    }

  /* Link the IDESC table into the cpu.  */
  CPU_IDESC (cpu) = table;
}

/* Enum declaration for all instruction semantic formats.  */
typedef enum sfmt {
  FMT_EMPTY, FMT_ADD, FMT_ADD3, FMT_AND3
 , FMT_OR3, FMT_ADDI, FMT_ADDV, FMT_ADDV3
 , FMT_ADDX, FMT_BC8, FMT_BC24, FMT_BEQ
 , FMT_BEQZ, FMT_BL8, FMT_BL24, FMT_BCL8
 , FMT_BCL24, FMT_BRA8, FMT_BRA24, FMT_CMP
 , FMT_CMPI, FMT_CMPZ, FMT_DIV, FMT_JC
 , FMT_JL, FMT_JMP, FMT_LD, FMT_LD_D
 , FMT_LDB, FMT_LDB_D, FMT_LDH, FMT_LDH_D
 , FMT_LD_PLUS, FMT_LD24, FMT_LDI8, FMT_LDI16
 , FMT_LOCK, FMT_MACHI_A, FMT_MULHI_A, FMT_MV
 , FMT_MVFACHI_A, FMT_MVFC, FMT_MVTACHI_A, FMT_MVTC
 , FMT_NOP, FMT_RAC_DSI, FMT_RTE, FMT_SETH
 , FMT_SLL3, FMT_SLLI, FMT_ST, FMT_ST_D
 , FMT_STB, FMT_STB_D, FMT_STH, FMT_STH_D
 , FMT_ST_PLUS, FMT_TRAP, FMT_UNLOCK, FMT_SATB
 , FMT_SAT, FMT_SADD, FMT_MACWU1, FMT_MSBLO
 , FMT_MULWU1, FMT_SC
} SFMT;

/* The decoder uses this to record insns and direct extraction handling.  */

typedef struct {
  const IDESC *idesc;
#ifdef __GNUC__
  void *sfmt;
#else
  enum sfmt sfmt;
#endif
} DECODE_DESC;

/* Macro to go from decode phase to extraction phase.  */

#ifdef __GNUC__
#define GOTO_EXTRACT(id) goto *(id)->sfmt
#else
#define GOTO_EXTRACT(id) goto extract
#endif

/* The decoder needs a slightly different computed goto switch control.  */
#ifdef __GNUC__
#define DECODE_SWITCH(N, X) goto *labels_##N[X];
#else
#define DECODE_SWITCH(N, X) switch (X)
#endif

/* Given an instruction, return a pointer to its IDESC entry.  */

const IDESC *
m32rxf_decode (SIM_CPU *current_cpu, IADDR pc,
              CGEN_INSN_INT base_insn, CGEN_INSN_INT entire_insn,
              ARGBUF *abuf)
{
  /* Result of decoder, used by extractor.  */
  const DECODE_DESC *idecode;

  /* First decode the instruction.  */

  {
#define I(insn) & m32rxf_insn_data[CONCAT2 (M32RXF_,insn)]
#ifdef __GNUC__
#define E(fmt) && case_ex_##fmt
#else
#define E(fmt) fmt
#endif
    CGEN_INSN_INT insn = base_insn;
    static const DECODE_DESC idecode_invalid = { I (INSN_X_INVALID), E (FMT_EMPTY) };

    {
#ifdef __GNUC__
      static const void *labels_0[256] = {
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && case_0_7, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && case_0_28, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && case_0_87, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && case_0_95, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && case_0_112, && case_0_113, && case_0_114, && case_0_115, 
        && case_0_116, && case_0_117, && case_0_118, && case_0_119, 
        && case_0_120, && case_0_121, && case_0_122, && case_0_123, 
        && case_0_124, && case_0_125, && case_0_126, && case_0_127, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && case_0_134, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && case_0_144, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && case_0_240, && case_0_241, && case_0_242, && case_0_243, 
        && case_0_244, && case_0_245, && case_0_246, && case_0_247, 
        && case_0_248, && case_0_249, && case_0_250, && case_0_251, 
        && case_0_252, && case_0_253, && case_0_254, && case_0_255, 
      };
#endif
      static const DECODE_DESC insns[256] = {
        { I (INSN_SUBV), E (FMT_ADDV) }, { I (INSN_SUBX), E (FMT_ADDX) }, 
        { I (INSN_SUB), E (FMT_ADD) }, { I (INSN_NEG), E (FMT_MV) }, 
        { I (INSN_CMP), E (FMT_CMP) }, { I (INSN_CMPU), E (FMT_CMP) }, 
        { I (INSN_CMPEQ), E (FMT_CMP) }, { 0 }, 
        { I (INSN_ADDV), E (FMT_ADDV) }, { I (INSN_ADDX), E (FMT_ADDX) }, 
        { I (INSN_ADD), E (FMT_ADD) }, { I (INSN_NOT), E (FMT_MV) }, 
        { I (INSN_AND), E (FMT_ADD) }, { I (INSN_XOR), E (FMT_ADD) }, 
        { I (INSN_OR), E (FMT_ADD) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_SRL), E (FMT_ADD) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_SRA), E (FMT_ADD) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_SLL), E (FMT_ADD) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_MUL), E (FMT_ADD) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_MV), E (FMT_MV) }, { I (INSN_MVFC), E (FMT_MVFC) }, 
        { I (INSN_MVTC), E (FMT_MVTC) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { 0 }, { I (INSN_RTE), E (FMT_RTE) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_TRAP), E (FMT_TRAP) }, 
        { I (INSN_STB), E (FMT_STB) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_STH), E (FMT_STH) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_ST), E (FMT_ST) }, { I (INSN_UNLOCK), E (FMT_UNLOCK) }, 
        { I (INSN_ST_PLUS), E (FMT_ST_PLUS) }, { I (INSN_ST_MINUS), E (FMT_ST_PLUS) }, 
        { I (INSN_LDB), E (FMT_LDB) }, { I (INSN_LDUB), E (FMT_LDB) }, 
        { I (INSN_LDH), E (FMT_LDH) }, { I (INSN_LDUH), E (FMT_LDH) }, 
        { I (INSN_LD), E (FMT_LD) }, { I (INSN_LOCK), E (FMT_LOCK) }, 
        { I (INSN_LD_PLUS), E (FMT_LD_PLUS) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_MULHI_A), E (FMT_MULHI_A) }, { I (INSN_MULLO_A), E (FMT_MULHI_A) }, 
        { I (INSN_MULWHI_A), E (FMT_MULHI_A) }, { I (INSN_MULWLO_A), E (FMT_MULHI_A) }, 
        { I (INSN_MACHI_A), E (FMT_MACHI_A) }, { I (INSN_MACLO_A), E (FMT_MACHI_A) }, 
        { I (INSN_MACWHI_A), E (FMT_MACHI_A) }, { I (INSN_MACWLO_A), E (FMT_MACHI_A) }, 
        { I (INSN_MULHI_A), E (FMT_MULHI_A) }, { I (INSN_MULLO_A), E (FMT_MULHI_A) }, 
        { I (INSN_MULWHI_A), E (FMT_MULHI_A) }, { I (INSN_MULWLO_A), E (FMT_MULHI_A) }, 
        { I (INSN_MACHI_A), E (FMT_MACHI_A) }, { I (INSN_MACLO_A), E (FMT_MACHI_A) }, 
        { I (INSN_MACWHI_A), E (FMT_MACHI_A) }, { I (INSN_MACWLO_A), E (FMT_MACHI_A) }, 
        { I (INSN_ADDI), E (FMT_ADDI) }, { I (INSN_ADDI), E (FMT_ADDI) }, 
        { I (INSN_ADDI), E (FMT_ADDI) }, { I (INSN_ADDI), E (FMT_ADDI) }, 
        { I (INSN_ADDI), E (FMT_ADDI) }, { I (INSN_ADDI), E (FMT_ADDI) }, 
        { I (INSN_ADDI), E (FMT_ADDI) }, { I (INSN_ADDI), E (FMT_ADDI) }, 
        { I (INSN_ADDI), E (FMT_ADDI) }, { I (INSN_ADDI), E (FMT_ADDI) }, 
        { I (INSN_ADDI), E (FMT_ADDI) }, { I (INSN_ADDI), E (FMT_ADDI) }, 
        { I (INSN_ADDI), E (FMT_ADDI) }, { I (INSN_ADDI), E (FMT_ADDI) }, 
        { I (INSN_ADDI), E (FMT_ADDI) }, { I (INSN_ADDI), E (FMT_ADDI) }, 
        { I (INSN_SRLI), E (FMT_SLLI) }, { I (INSN_SRLI), E (FMT_SLLI) }, 
        { I (INSN_SRAI), E (FMT_SLLI) }, { I (INSN_SRAI), E (FMT_SLLI) }, 
        { I (INSN_SLLI), E (FMT_SLLI) }, { I (INSN_SLLI), E (FMT_SLLI) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { 0 }, 
        { I (INSN_RACH_DSI), E (FMT_RAC_DSI) }, { I (INSN_RAC_DSI), E (FMT_RAC_DSI) }, 
        { I (INSN_MULWU1), E (FMT_MULWU1) }, { I (INSN_MACWU1), E (FMT_MACWU1) }, 
        { I (INSN_MACLH1), E (FMT_MACWU1) }, { I (INSN_MSBLO), E (FMT_MSBLO) }, 
        { I (INSN_SADD), E (FMT_SADD) }, { 0 }, 
        { I (INSN_LDI8), E (FMT_LDI8) }, { I (INSN_LDI8), E (FMT_LDI8) }, 
        { I (INSN_LDI8), E (FMT_LDI8) }, { I (INSN_LDI8), E (FMT_LDI8) }, 
        { I (INSN_LDI8), E (FMT_LDI8) }, { I (INSN_LDI8), E (FMT_LDI8) }, 
        { I (INSN_LDI8), E (FMT_LDI8) }, { I (INSN_LDI8), E (FMT_LDI8) }, 
        { I (INSN_LDI8), E (FMT_LDI8) }, { I (INSN_LDI8), E (FMT_LDI8) }, 
        { I (INSN_LDI8), E (FMT_LDI8) }, { I (INSN_LDI8), E (FMT_LDI8) }, 
        { I (INSN_LDI8), E (FMT_LDI8) }, { I (INSN_LDI8), E (FMT_LDI8) }, 
        { I (INSN_LDI8), E (FMT_LDI8) }, { I (INSN_LDI8), E (FMT_LDI8) }, 
        { 0 }, { 0 }, 
        { 0 }, { 0 }, 
        { 0 }, { 0 }, 
        { 0 }, { 0 }, 
        { 0 }, { 0 }, 
        { 0 }, { 0 }, 
        { 0 }, { 0 }, 
        { 0 }, { 0 }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_CMPI), E (FMT_CMPI) }, { I (INSN_CMPUI), E (FMT_CMPI) }, 
        { 0 }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_ADDV3), E (FMT_ADDV3) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_ADD3), E (FMT_ADD3) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_AND3), E (FMT_AND3) }, { I (INSN_XOR3), E (FMT_AND3) }, 
        { I (INSN_OR3), E (FMT_OR3) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { 0 }, { I (INSN_DIVU), E (FMT_DIV) }, 
        { I (INSN_REM), E (FMT_DIV) }, { I (INSN_REMU), E (FMT_DIV) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_SRL3), E (FMT_SLL3) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_SRA3), E (FMT_SLL3) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_SLL3), E (FMT_SLL3) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_LDI16), E (FMT_LDI16) }, 
        { I (INSN_STB_D), E (FMT_STB_D) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_STH_D), E (FMT_STH_D) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_ST_D), E (FMT_ST_D) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_LDB_D), E (FMT_LDB_D) }, { I (INSN_LDUB_D), E (FMT_LDB_D) }, 
        { I (INSN_LDH_D), E (FMT_LDH_D) }, { I (INSN_LDUH_D), E (FMT_LDH_D) }, 
        { I (INSN_LD_D), E (FMT_LD_D) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_BEQ), E (FMT_BEQ) }, { I (INSN_BNE), E (FMT_BEQ) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_BEQZ), E (FMT_BEQZ) }, { I (INSN_BNEZ), E (FMT_BEQZ) }, 
        { I (INSN_BLTZ), E (FMT_BEQZ) }, { I (INSN_BGEZ), E (FMT_BEQZ) }, 
        { I (INSN_BLEZ), E (FMT_BEQZ) }, { I (INSN_BGTZ), E (FMT_BEQZ) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_SETH), E (FMT_SETH) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_LD24), E (FMT_LD24) }, { I (INSN_LD24), E (FMT_LD24) }, 
        { I (INSN_LD24), E (FMT_LD24) }, { I (INSN_LD24), E (FMT_LD24) }, 
        { I (INSN_LD24), E (FMT_LD24) }, { I (INSN_LD24), E (FMT_LD24) }, 
        { I (INSN_LD24), E (FMT_LD24) }, { I (INSN_LD24), E (FMT_LD24) }, 
        { I (INSN_LD24), E (FMT_LD24) }, { I (INSN_LD24), E (FMT_LD24) }, 
        { I (INSN_LD24), E (FMT_LD24) }, { I (INSN_LD24), E (FMT_LD24) }, 
        { I (INSN_LD24), E (FMT_LD24) }, { I (INSN_LD24), E (FMT_LD24) }, 
        { I (INSN_LD24), E (FMT_LD24) }, { I (INSN_LD24), E (FMT_LD24) }, 
        { 0 }, { 0 }, 
        { 0 }, { 0 }, 
        { 0 }, { 0 }, 
        { 0 }, { 0 }, 
        { 0 }, { 0 }, 
        { 0 }, { 0 }, 
        { 0 }, { 0 }, 
        { 0 }, { 0 }, 
      };
      unsigned int val;
      val = (((insn >> 8) & (15 << 4)) | ((insn >> 4) & (15 << 0)));
      DECODE_SWITCH (0, val)
        {
        CASE (0, 7) :
          {
            static const DECODE_DESC insns[16] = {
              { I (INSN_CMPZ), E (FMT_CMPZ) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_PCMPBZ), E (FMT_CMPZ) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
            };
            unsigned int val = (((insn >> 8) & (15 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        CASE (0, 28) :
          {
            static const DECODE_DESC insns[16] = {
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_JC), E (FMT_JC) }, { I (INSN_JNC), E (FMT_JC) }, 
              { I (INSN_JL), E (FMT_JL) }, { I (INSN_JMP), E (FMT_JMP) }, 
            };
            unsigned int val = (((insn >> 8) & (15 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        CASE (0, 87) :
          {
            static const DECODE_DESC insns[4] = {
              { I (INSN_MVTACHI_A), E (FMT_MVTACHI_A) }, { I (INSN_MVTACLO_A), E (FMT_MVTACHI_A) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
            };
            unsigned int val = (((insn >> 0) & (3 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        CASE (0, 95) :
          {
            static const DECODE_DESC insns[4] = {
              { I (INSN_MVFACHI_A), E (FMT_MVFACHI_A) }, { I (INSN_MVFACLO_A), E (FMT_MVFACHI_A) }, 
              { I (INSN_MVFACMI_A), E (FMT_MVFACHI_A) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
            };
            unsigned int val = (((insn >> 0) & (3 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        CASE (0, 112) :
          {
            static const DECODE_DESC insns[16] = {
              { I (INSN_NOP), E (FMT_NOP) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_SC), E (FMT_SC) }, { I (INSN_SNC), E (FMT_SC) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_BCL8), E (FMT_BCL8) }, { I (INSN_BNCL8), E (FMT_BCL8) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_BC8), E (FMT_BC8) }, { I (INSN_BNC8), E (FMT_BC8) }, 
              { I (INSN_BL8), E (FMT_BL8) }, { I (INSN_BRA8), E (FMT_BRA8) }, 
            };
            unsigned int val = (((insn >> 8) & (15 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        CASE (0, 113) : /* fall through */
        CASE (0, 114) : /* fall through */
        CASE (0, 115) : /* fall through */
        CASE (0, 116) : /* fall through */
        CASE (0, 117) : /* fall through */
        CASE (0, 118) : /* fall through */
        CASE (0, 119) : /* fall through */
        CASE (0, 120) : /* fall through */
        CASE (0, 121) : /* fall through */
        CASE (0, 122) : /* fall through */
        CASE (0, 123) : /* fall through */
        CASE (0, 124) : /* fall through */
        CASE (0, 125) : /* fall through */
        CASE (0, 126) : /* fall through */
        CASE (0, 127) :
          {
            static const DECODE_DESC insns[16] = {
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_BCL8), E (FMT_BCL8) }, { I (INSN_BNCL8), E (FMT_BCL8) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_BC8), E (FMT_BC8) }, { I (INSN_BNC8), E (FMT_BC8) }, 
              { I (INSN_BL8), E (FMT_BL8) }, { I (INSN_BRA8), E (FMT_BRA8) }, 
            };
            unsigned int val = (((insn >> 8) & (15 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        CASE (0, 134) :
          {
#ifdef __GNUC__
            static const void *labels_0_134[16] = {
              && case_0_134_0, && default_0_134, && default_0_134, && default_0_134, 
              && default_0_134, && default_0_134, && default_0_134, && default_0_134, 
              && default_0_134, && default_0_134, && default_0_134, && default_0_134, 
              && default_0_134, && default_0_134, && default_0_134, && default_0_134, 
            };
#endif
            static const DECODE_DESC insns[16] = {
              { 0 }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
            };
            unsigned int val;
            /* Must fetch more bits.  */
            insn = GETIMEMUHI (current_cpu, pc + 2);
            val = (((insn >> 12) & (15 << 0)));
            DECODE_SWITCH (0_134, val)
              {
              CASE (0_134, 0) :
                {
                  static const DECODE_DESC insns[16] = {
                    { I (INSN_SAT), E (FMT_SAT) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_SATH), E (FMT_SATB) }, { I (INSN_SATB), E (FMT_SATB) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                  };
                  unsigned int val = (((insn >> 8) & (15 << 0)));
                  idecode = &insns[val];
                  GOTO_EXTRACT (idecode);
                }
              DEFAULT (0_134) :
                idecode = &insns[val];
                GOTO_EXTRACT (idecode);
              }
            ENDSWITCH (0_134)
          }
        CASE (0, 144) :
          {
#ifdef __GNUC__
            static const void *labels_0_144[16] = {
              && case_0_144_0, && default_0_144, && default_0_144, && default_0_144, 
              && default_0_144, && default_0_144, && default_0_144, && default_0_144, 
              && default_0_144, && default_0_144, && default_0_144, && default_0_144, 
              && default_0_144, && default_0_144, && default_0_144, && default_0_144, 
            };
#endif
            static const DECODE_DESC insns[16] = {
              { 0 }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
            };
            unsigned int val;
            /* Must fetch more bits.  */
            insn = GETIMEMUHI (current_cpu, pc + 2);
            val = (((insn >> 12) & (15 << 0)));
            DECODE_SWITCH (0_144, val)
              {
              CASE (0_144, 0) :
                {
#ifdef __GNUC__
                  static const void *labels_0_144_0[16] = {
                    && case_0_144_0_0, && default_0_144_0, && default_0_144_0, && default_0_144_0, 
                    && default_0_144_0, && default_0_144_0, && default_0_144_0, && default_0_144_0, 
                    && default_0_144_0, && default_0_144_0, && default_0_144_0, && default_0_144_0, 
                    && default_0_144_0, && default_0_144_0, && default_0_144_0, && default_0_144_0, 
                  };
#endif
                  static const DECODE_DESC insns[16] = {
                    { 0 }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                  };
                  unsigned int val;
                  val = (((insn >> 8) & (15 << 0)));
                  DECODE_SWITCH (0_144_0, val)
                    {
                    CASE (0_144_0, 0) :
                      {
                        static const DECODE_DESC insns[16] = {
                          { I (INSN_DIV), E (FMT_DIV) }, { I (INSN_DIVH), E (FMT_DIV) }, 
                          { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                          { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                          { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                          { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                          { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                          { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                          { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                        };
                        unsigned int val = (((insn >> 4) & (15 << 0)));
                        idecode = &insns[val];
                        GOTO_EXTRACT (idecode);
                      }
                    DEFAULT (0_144_0) :
                      idecode = &insns[val];
                      GOTO_EXTRACT (idecode);
                    }
                  ENDSWITCH (0_144_0)
                }
              DEFAULT (0_144) :
                idecode = &insns[val];
                GOTO_EXTRACT (idecode);
              }
            ENDSWITCH (0_144)
          }
        CASE (0, 240) : /* fall through */
        CASE (0, 241) : /* fall through */
        CASE (0, 242) : /* fall through */
        CASE (0, 243) : /* fall through */
        CASE (0, 244) : /* fall through */
        CASE (0, 245) : /* fall through */
        CASE (0, 246) : /* fall through */
        CASE (0, 247) : /* fall through */
        CASE (0, 248) : /* fall through */
        CASE (0, 249) : /* fall through */
        CASE (0, 250) : /* fall through */
        CASE (0, 251) : /* fall through */
        CASE (0, 252) : /* fall through */
        CASE (0, 253) : /* fall through */
        CASE (0, 254) : /* fall through */
        CASE (0, 255) :
          {
            static const DECODE_DESC insns[16] = {
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_BCL24), E (FMT_BCL24) }, { I (INSN_BNCL24), E (FMT_BCL24) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_BC24), E (FMT_BC24) }, { I (INSN_BNC24), E (FMT_BC24) }, 
              { I (INSN_BL24), E (FMT_BL24) }, { I (INSN_BRA24), E (FMT_BRA24) }, 
            };
            unsigned int val = (((insn >> 8) & (15 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        DEFAULT (0) :
          idecode = &insns[val];
          GOTO_EXTRACT (idecode);
        }
      ENDSWITCH (0)
    }
#undef I
#undef E
  }

  /* The instruction has been decoded, now extract the fields.  */

 extract:
  {
#ifndef __GNUC__
    switch (idecode->sfmt)
#endif
      {

  CASE (ex, FMT_EMPTY) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_empty.f
  EXTRACT_IFMT_EMPTY_VARS /* */

  EXTRACT_IFMT_EMPTY_CODE

  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_empty", (char *) 0));

#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_ADD) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_add.f
  EXTRACT_IFMT_ADD_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_IFMT_ADD_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_dr) = & CPU (h_gr)[f_r1];
  FLD (i_sr) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_add", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_dr) = f_r1;
      FLD (in_sr) = f_r2;
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_ADD3) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_add3.f
  EXTRACT_IFMT_ADD3_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_IFMT_ADD3_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_simm16) = f_simm16;
  FLD (i_sr) = & CPU (h_gr)[f_r2];
  FLD (i_dr) = & CPU (h_gr)[f_r1];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_add3", "f_simm16 0x%x", 'x', f_simm16, "sr 0x%x", 'x', f_r2, "dr 0x%x", 'x', f_r1, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_sr) = f_r2;
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_AND3) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_and3.f
  EXTRACT_IFMT_AND3_VARS /* f-op1 f-r1 f-op2 f-r2 f-uimm16 */

  EXTRACT_IFMT_AND3_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_uimm16) = f_uimm16;
  FLD (i_sr) = & CPU (h_gr)[f_r2];
  FLD (i_dr) = & CPU (h_gr)[f_r1];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_and3", "f_uimm16 0x%x", 'x', f_uimm16, "sr 0x%x", 'x', f_r2, "dr 0x%x", 'x', f_r1, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_sr) = f_r2;
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_OR3) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_or3.f
  EXTRACT_IFMT_OR3_VARS /* f-op1 f-r1 f-op2 f-r2 f-uimm16 */

  EXTRACT_IFMT_OR3_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_uimm16) = f_uimm16;
  FLD (i_sr) = & CPU (h_gr)[f_r2];
  FLD (i_dr) = & CPU (h_gr)[f_r1];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_or3", "f_uimm16 0x%x", 'x', f_uimm16, "sr 0x%x", 'x', f_r2, "dr 0x%x", 'x', f_r1, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_sr) = f_r2;
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_ADDI) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_addi.f
  EXTRACT_IFMT_ADDI_VARS /* f-op1 f-r1 f-simm8 */

  EXTRACT_IFMT_ADDI_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_simm8) = f_simm8;
  FLD (i_dr) = & CPU (h_gr)[f_r1];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_addi", "f_simm8 0x%x", 'x', f_simm8, "dr 0x%x", 'x', f_r1, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_dr) = f_r1;
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_ADDV) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_addv.f
  EXTRACT_IFMT_ADD_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_IFMT_ADD_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_dr) = & CPU (h_gr)[f_r1];
  FLD (i_sr) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_addv", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_dr) = f_r1;
      FLD (in_sr) = f_r2;
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_ADDV3) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_addv3.f
  EXTRACT_IFMT_ADDV3_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_IFMT_ADDV3_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_simm16) = f_simm16;
  FLD (i_sr) = & CPU (h_gr)[f_r2];
  FLD (i_dr) = & CPU (h_gr)[f_r1];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_addv3", "f_simm16 0x%x", 'x', f_simm16, "sr 0x%x", 'x', f_r2, "dr 0x%x", 'x', f_r1, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_sr) = f_r2;
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_ADDX) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_addx.f
  EXTRACT_IFMT_ADD_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_IFMT_ADD_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_dr) = & CPU (h_gr)[f_r1];
  FLD (i_sr) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_addx", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_dr) = f_r1;
      FLD (in_sr) = f_r2;
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_BC8) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_bc8.f
  EXTRACT_IFMT_BC8_VARS /* f-op1 f-r1 f-disp8 */

  EXTRACT_IFMT_BC8_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_disp8) = f_disp8;
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_bc8", "disp8 0x%x", 'x', f_disp8, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_BC24) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_bc24.f
  EXTRACT_IFMT_BC24_VARS /* f-op1 f-r1 f-disp24 */

  EXTRACT_IFMT_BC24_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_disp24) = f_disp24;
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_bc24", "disp24 0x%x", 'x', f_disp24, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_BEQ) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_beq.f
  EXTRACT_IFMT_BEQ_VARS /* f-op1 f-r1 f-op2 f-r2 f-disp16 */

  EXTRACT_IFMT_BEQ_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_disp16) = f_disp16;
  FLD (i_src1) = & CPU (h_gr)[f_r1];
  FLD (i_src2) = & CPU (h_gr)[f_r2];
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_beq", "disp16 0x%x", 'x', f_disp16, "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src1) = f_r1;
      FLD (in_src2) = f_r2;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_BEQZ) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_beqz.f
  EXTRACT_IFMT_BEQZ_VARS /* f-op1 f-r1 f-op2 f-r2 f-disp16 */

  EXTRACT_IFMT_BEQZ_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_disp16) = f_disp16;
  FLD (i_src2) = & CPU (h_gr)[f_r2];
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_beqz", "disp16 0x%x", 'x', f_disp16, "src2 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src2) = f_r2;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_BL8) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_bl8.f
  EXTRACT_IFMT_BC8_VARS /* f-op1 f-r1 f-disp8 */

  EXTRACT_IFMT_BC8_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_disp8) = f_disp8;
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_bl8", "disp8 0x%x", 'x', f_disp8, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_h_gr_14) = 14;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_BL24) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_bl24.f
  EXTRACT_IFMT_BC24_VARS /* f-op1 f-r1 f-disp24 */

  EXTRACT_IFMT_BC24_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_disp24) = f_disp24;
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_bl24", "disp24 0x%x", 'x', f_disp24, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_h_gr_14) = 14;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_BCL8) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_bcl8.f
  EXTRACT_IFMT_BC8_VARS /* f-op1 f-r1 f-disp8 */

  EXTRACT_IFMT_BC8_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_disp8) = f_disp8;
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_bcl8", "disp8 0x%x", 'x', f_disp8, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_h_gr_14) = 14;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_BCL24) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_bcl24.f
  EXTRACT_IFMT_BC24_VARS /* f-op1 f-r1 f-disp24 */

  EXTRACT_IFMT_BC24_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_disp24) = f_disp24;
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_bcl24", "disp24 0x%x", 'x', f_disp24, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_h_gr_14) = 14;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_BRA8) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_bra8.f
  EXTRACT_IFMT_BC8_VARS /* f-op1 f-r1 f-disp8 */

  EXTRACT_IFMT_BC8_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_disp8) = f_disp8;
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_bra8", "disp8 0x%x", 'x', f_disp8, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_BRA24) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_bra24.f
  EXTRACT_IFMT_BC24_VARS /* f-op1 f-r1 f-disp24 */

  EXTRACT_IFMT_BC24_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_disp24) = f_disp24;
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_bra24", "disp24 0x%x", 'x', f_disp24, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_CMP) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_cmp.f
  EXTRACT_IFMT_CMP_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_IFMT_CMP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_src1) = & CPU (h_gr)[f_r1];
  FLD (i_src2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_cmp", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src1) = f_r1;
      FLD (in_src2) = f_r2;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_CMPI) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_cmpi.f
  EXTRACT_IFMT_CMPI_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_IFMT_CMPI_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_simm16) = f_simm16;
  FLD (i_src2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_cmpi", "f_simm16 0x%x", 'x', f_simm16, "src2 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src2) = f_r2;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_CMPZ) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_cmpz.f
  EXTRACT_IFMT_CMPZ_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_IFMT_CMPZ_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_src2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_cmpz", "src2 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src2) = f_r2;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_DIV) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_div.f
  EXTRACT_IFMT_DIV_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_IFMT_DIV_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_dr) = & CPU (h_gr)[f_r1];
  FLD (i_sr) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_div", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_dr) = f_r1;
      FLD (in_sr) = f_r2;
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_JC) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_jc.f
  EXTRACT_IFMT_JC_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_IFMT_JC_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_sr) = & CPU (h_gr)[f_r2];
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_jc", "sr 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_sr) = f_r2;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_JL) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_jl.f
  EXTRACT_IFMT_JC_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_IFMT_JC_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_sr) = & CPU (h_gr)[f_r2];
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_jl", "sr 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_sr) = f_r2;
      FLD (out_h_gr_14) = 14;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_JMP) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_jmp.f
  EXTRACT_IFMT_JC_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_IFMT_JC_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_sr) = & CPU (h_gr)[f_r2];
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_jmp", "sr 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_sr) = f_r2;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LD) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_ld.f
  EXTRACT_IFMT_ADD_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_IFMT_ADD_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_sr) = & CPU (h_gr)[f_r2];
  FLD (i_dr) = & CPU (h_gr)[f_r1];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ld", "sr 0x%x", 'x', f_r2, "dr 0x%x", 'x', f_r1, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_sr) = f_r2;
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LD_D) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_ld_d.f
  EXTRACT_IFMT_ADD3_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_IFMT_ADD3_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_simm16) = f_simm16;
  FLD (i_sr) = & CPU (h_gr)[f_r2];
  FLD (i_dr) = & CPU (h_gr)[f_r1];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ld_d", "f_simm16 0x%x", 'x', f_simm16, "sr 0x%x", 'x', f_r2, "dr 0x%x", 'x', f_r1, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_sr) = f_r2;
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDB) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_ldb.f
  EXTRACT_IFMT_ADD_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_IFMT_ADD_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_sr) = & CPU (h_gr)[f_r2];
  FLD (i_dr) = & CPU (h_gr)[f_r1];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldb", "sr 0x%x", 'x', f_r2, "dr 0x%x", 'x', f_r1, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_sr) = f_r2;
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDB_D) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_ldb_d.f
  EXTRACT_IFMT_ADD3_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_IFMT_ADD3_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_simm16) = f_simm16;
  FLD (i_sr) = & CPU (h_gr)[f_r2];
  FLD (i_dr) = & CPU (h_gr)[f_r1];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldb_d", "f_simm16 0x%x", 'x', f_simm16, "sr 0x%x", 'x', f_r2, "dr 0x%x", 'x', f_r1, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_sr) = f_r2;
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDH) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_ldh.f
  EXTRACT_IFMT_ADD_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_IFMT_ADD_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_sr) = & CPU (h_gr)[f_r2];
  FLD (i_dr) = & CPU (h_gr)[f_r1];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldh", "sr 0x%x", 'x', f_r2, "dr 0x%x", 'x', f_r1, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_sr) = f_r2;
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDH_D) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_ldh_d.f
  EXTRACT_IFMT_ADD3_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_IFMT_ADD3_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_simm16) = f_simm16;
  FLD (i_sr) = & CPU (h_gr)[f_r2];
  FLD (i_dr) = & CPU (h_gr)[f_r1];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldh_d", "f_simm16 0x%x", 'x', f_simm16, "sr 0x%x", 'x', f_r2, "dr 0x%x", 'x', f_r1, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_sr) = f_r2;
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LD_PLUS) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_ld_plus.f
  EXTRACT_IFMT_ADD_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_IFMT_ADD_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_sr) = & CPU (h_gr)[f_r2];
  FLD (i_dr) = & CPU (h_gr)[f_r1];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ld_plus", "sr 0x%x", 'x', f_r2, "dr 0x%x", 'x', f_r1, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_sr) = f_r2;
      FLD (out_dr) = f_r1;
      FLD (out_sr) = f_r2;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LD24) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_ld24.f
  EXTRACT_IFMT_LD24_VARS /* f-op1 f-r1 f-uimm24 */

  EXTRACT_IFMT_LD24_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_uimm24) = f_uimm24;
  FLD (i_dr) = & CPU (h_gr)[f_r1];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ld24", "uimm24 0x%x", 'x', f_uimm24, "dr 0x%x", 'x', f_r1, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDI8) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_ldi8.f
  EXTRACT_IFMT_ADDI_VARS /* f-op1 f-r1 f-simm8 */

  EXTRACT_IFMT_ADDI_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_simm8) = f_simm8;
  FLD (i_dr) = & CPU (h_gr)[f_r1];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldi8", "f_simm8 0x%x", 'x', f_simm8, "dr 0x%x", 'x', f_r1, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDI16) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_ldi16.f
  EXTRACT_IFMT_LDI16_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_IFMT_LDI16_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_simm16) = f_simm16;
  FLD (i_dr) = & CPU (h_gr)[f_r1];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldi16", "f_simm16 0x%x", 'x', f_simm16, "dr 0x%x", 'x', f_r1, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LOCK) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_lock.f
  EXTRACT_IFMT_ADD_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_IFMT_ADD_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_sr) = & CPU (h_gr)[f_r2];
  FLD (i_dr) = & CPU (h_gr)[f_r1];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_lock", "sr 0x%x", 'x', f_r2, "dr 0x%x", 'x', f_r1, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_sr) = f_r2;
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_MACHI_A) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_machi_a.f
  EXTRACT_IFMT_MACHI_A_VARS /* f-op1 f-r1 f-acc f-op23 f-r2 */

  EXTRACT_IFMT_MACHI_A_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_acc) = f_acc;
  FLD (i_src1) = & CPU (h_gr)[f_r1];
  FLD (i_src2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_machi_a", "f_acc 0x%x", 'x', f_acc, "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src1) = f_r1;
      FLD (in_src2) = f_r2;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_MULHI_A) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_mulhi_a.f
  EXTRACT_IFMT_MACHI_A_VARS /* f-op1 f-r1 f-acc f-op23 f-r2 */

  EXTRACT_IFMT_MACHI_A_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_acc) = f_acc;
  FLD (i_src1) = & CPU (h_gr)[f_r1];
  FLD (i_src2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_mulhi_a", "f_acc 0x%x", 'x', f_acc, "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src1) = f_r1;
      FLD (in_src2) = f_r2;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_MV) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_mv.f
  EXTRACT_IFMT_ADD_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_IFMT_ADD_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_sr) = & CPU (h_gr)[f_r2];
  FLD (i_dr) = & CPU (h_gr)[f_r1];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_mv", "sr 0x%x", 'x', f_r2, "dr 0x%x", 'x', f_r1, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_sr) = f_r2;
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_MVFACHI_A) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_mvfachi_a.f
  EXTRACT_IFMT_MVFACHI_A_VARS /* f-op1 f-r1 f-op2 f-accs f-op3 */

  EXTRACT_IFMT_MVFACHI_A_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_accs) = f_accs;
  FLD (i_dr) = & CPU (h_gr)[f_r1];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_mvfachi_a", "f_accs 0x%x", 'x', f_accs, "dr 0x%x", 'x', f_r1, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_MVFC) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_mvfc.f
  EXTRACT_IFMT_MVFC_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_IFMT_MVFC_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r2) = f_r2;
  FLD (i_dr) = & CPU (h_gr)[f_r1];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_mvfc", "f_r2 0x%x", 'x', f_r2, "dr 0x%x", 'x', f_r1, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_MVTACHI_A) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_mvtachi_a.f
  EXTRACT_IFMT_MVTACHI_A_VARS /* f-op1 f-r1 f-op2 f-accs f-op3 */

  EXTRACT_IFMT_MVTACHI_A_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_accs) = f_accs;
  FLD (i_src1) = & CPU (h_gr)[f_r1];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_mvtachi_a", "f_accs 0x%x", 'x', f_accs, "src1 0x%x", 'x', f_r1, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src1) = f_r1;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_MVTC) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_mvtc.f
  EXTRACT_IFMT_MVTC_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_IFMT_MVTC_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = f_r1;
  FLD (i_sr) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_mvtc", "f_r1 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_sr) = f_r2;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_NOP) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_nop.f
  EXTRACT_IFMT_NOP_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_IFMT_NOP_CODE

  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_nop", (char *) 0));

#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_RAC_DSI) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_rac_dsi.f
  EXTRACT_IFMT_RAC_DSI_VARS /* f-op1 f-accd f-bits67 f-op2 f-accs f-bit14 f-imm1 */

  EXTRACT_IFMT_RAC_DSI_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_accs) = f_accs;
  FLD (f_imm1) = f_imm1;
  FLD (f_accd) = f_accd;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_rac_dsi", "f_accs 0x%x", 'x', f_accs, "f_imm1 0x%x", 'x', f_imm1, "f_accd 0x%x", 'x', f_accd, (char *) 0));

#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_RTE) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_rte.f
  EXTRACT_IFMT_NOP_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_IFMT_NOP_CODE

  /* Record the fields for the semantic handler.  */
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_rte", (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_SETH) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_seth.f
  EXTRACT_IFMT_SETH_VARS /* f-op1 f-r1 f-op2 f-r2 f-hi16 */

  EXTRACT_IFMT_SETH_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_hi16) = f_hi16;
  FLD (i_dr) = & CPU (h_gr)[f_r1];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_seth", "f_hi16 0x%x", 'x', f_hi16, "dr 0x%x", 'x', f_r1, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_SLL3) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_sll3.f
  EXTRACT_IFMT_ADDV3_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_IFMT_ADDV3_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_simm16) = f_simm16;
  FLD (i_sr) = & CPU (h_gr)[f_r2];
  FLD (i_dr) = & CPU (h_gr)[f_r1];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_sll3", "f_simm16 0x%x", 'x', f_simm16, "sr 0x%x", 'x', f_r2, "dr 0x%x", 'x', f_r1, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_sr) = f_r2;
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_SLLI) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_slli.f
  EXTRACT_IFMT_SLLI_VARS /* f-op1 f-r1 f-shift-op2 f-uimm5 */

  EXTRACT_IFMT_SLLI_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_uimm5) = f_uimm5;
  FLD (i_dr) = & CPU (h_gr)[f_r1];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_slli", "f_uimm5 0x%x", 'x', f_uimm5, "dr 0x%x", 'x', f_r1, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_dr) = f_r1;
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_ST) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_st.f
  EXTRACT_IFMT_CMP_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_IFMT_CMP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_src1) = & CPU (h_gr)[f_r1];
  FLD (i_src2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_st", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src1) = f_r1;
      FLD (in_src2) = f_r2;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_ST_D) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_st_d.f
  EXTRACT_IFMT_ST_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_IFMT_ST_D_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_simm16) = f_simm16;
  FLD (i_src1) = & CPU (h_gr)[f_r1];
  FLD (i_src2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_st_d", "f_simm16 0x%x", 'x', f_simm16, "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src1) = f_r1;
      FLD (in_src2) = f_r2;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STB) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_stb.f
  EXTRACT_IFMT_CMP_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_IFMT_CMP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_src1) = & CPU (h_gr)[f_r1];
  FLD (i_src2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stb", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src1) = f_r1;
      FLD (in_src2) = f_r2;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STB_D) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_stb_d.f
  EXTRACT_IFMT_ST_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_IFMT_ST_D_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_simm16) = f_simm16;
  FLD (i_src1) = & CPU (h_gr)[f_r1];
  FLD (i_src2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stb_d", "f_simm16 0x%x", 'x', f_simm16, "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src1) = f_r1;
      FLD (in_src2) = f_r2;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STH) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_sth.f
  EXTRACT_IFMT_CMP_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_IFMT_CMP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_src1) = & CPU (h_gr)[f_r1];
  FLD (i_src2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_sth", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src1) = f_r1;
      FLD (in_src2) = f_r2;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STH_D) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_sth_d.f
  EXTRACT_IFMT_ST_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_IFMT_ST_D_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_simm16) = f_simm16;
  FLD (i_src1) = & CPU (h_gr)[f_r1];
  FLD (i_src2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_sth_d", "f_simm16 0x%x", 'x', f_simm16, "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src1) = f_r1;
      FLD (in_src2) = f_r2;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_ST_PLUS) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_st_plus.f
  EXTRACT_IFMT_CMP_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_IFMT_CMP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_src1) = & CPU (h_gr)[f_r1];
  FLD (i_src2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_st_plus", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src1) = f_r1;
      FLD (in_src2) = f_r2;
      FLD (out_src2) = f_r2;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_TRAP) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_trap.f
  EXTRACT_IFMT_TRAP_VARS /* f-op1 f-r1 f-op2 f-uimm4 */

  EXTRACT_IFMT_TRAP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_uimm4) = f_uimm4;
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_trap", "f_uimm4 0x%x", 'x', f_uimm4, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_UNLOCK) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_unlock.f
  EXTRACT_IFMT_CMP_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_IFMT_CMP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_src1) = & CPU (h_gr)[f_r1];
  FLD (i_src2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_unlock", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src1) = f_r1;
      FLD (in_src2) = f_r2;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_SATB) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_satb.f
  EXTRACT_IFMT_SATB_VARS /* f-op1 f-r1 f-op2 f-r2 f-uimm16 */

  EXTRACT_IFMT_SATB_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_sr) = & CPU (h_gr)[f_r2];
  FLD (i_dr) = & CPU (h_gr)[f_r1];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_satb", "sr 0x%x", 'x', f_r2, "dr 0x%x", 'x', f_r1, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_sr) = f_r2;
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_SAT) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_sat.f
  EXTRACT_IFMT_SATB_VARS /* f-op1 f-r1 f-op2 f-r2 f-uimm16 */

  EXTRACT_IFMT_SATB_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_sr) = & CPU (h_gr)[f_r2];
  FLD (i_dr) = & CPU (h_gr)[f_r1];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_sat", "sr 0x%x", 'x', f_r2, "dr 0x%x", 'x', f_r1, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_sr) = f_r2;
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_SADD) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_sadd.f
  EXTRACT_IFMT_NOP_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_IFMT_NOP_CODE

  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_sadd", (char *) 0));

#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_MACWU1) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_macwu1.f
  EXTRACT_IFMT_CMP_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_IFMT_CMP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_src1) = & CPU (h_gr)[f_r1];
  FLD (i_src2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_macwu1", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src1) = f_r1;
      FLD (in_src2) = f_r2;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_MSBLO) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_msblo.f
  EXTRACT_IFMT_CMP_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_IFMT_CMP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_src1) = & CPU (h_gr)[f_r1];
  FLD (i_src2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_msblo", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src1) = f_r1;
      FLD (in_src2) = f_r2;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_MULWU1) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_mulwu1.f
  EXTRACT_IFMT_CMP_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_IFMT_CMP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_src1) = & CPU (h_gr)[f_r1];
  FLD (i_src2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_mulwu1", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src1) = f_r1;
      FLD (in_src2) = f_r2;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_SC) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_sc.f
  EXTRACT_IFMT_NOP_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_IFMT_NOP_CODE

  /* Record the fields for the semantic handler.  */
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_sc", (char *) 0));

#undef FLD
  BREAK (ex);
  }


      }
    ENDSWITCH (ex)

  }

  return idecode->idesc;
}
