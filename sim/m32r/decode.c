/* Simulator instruction decoder for m32rbf.

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

#define WANT_CPU m32rbf
#define WANT_CPU_M32RBF

#include "sim-main.h"
#include "sim-assert.h"

/* FIXME: Need to review choices for the following.  */

#if WITH_SEM_SWITCH_FULL
#define FULL(fn)
#else
#define FULL(fn) CONCAT3 (m32rbf,_sem_,fn) ,
#endif

#if WITH_FAST
#if WITH_SEM_SWITCH_FAST
#define FAST(fn)
#else
#define FAST(fn) CONCAT3 (m32rbf,_semf_,fn) , /* f for fast */
#endif
#else
#define FAST(fn)
#endif

/* The INSN_ prefix is not here and is instead part of the `insn' argument
   to avoid collisions with header files (e.g. `AND' in ansidecl.h).  */
#define IDX(insn) CONCAT2 (M32RBF_,insn)
#define TYPE(insn) CONCAT2 (M32R_,insn)

/* The instruction descriptor array.
   This is computed at runtime.  Space for it is not malloc'd to save a
   teensy bit of cpu in the decoder.  Moving it to malloc space is trivial
   but won't be done until necessary (we don't currently support the runtime
   addition of instructions nor an SMP machine with different cpus).  */
static IDESC m32rbf_insn_data[M32RBF_INSN_MAX];

/* Commas between elements are contained in the macros.
   Some of these are conditionally compiled out.  */

static const struct insn_sem m32rbf_insn_sem[] =
{
  { VIRTUAL_INSN_X_INVALID, IDX (INSN_X_INVALID), FULL (x_invalid) FAST (x_invalid) },
  { VIRTUAL_INSN_X_AFTER, IDX (INSN_X_AFTER), FULL (x_after) FAST (x_after) },
  { VIRTUAL_INSN_X_BEFORE, IDX (INSN_X_BEFORE), FULL (x_before) FAST (x_before) },
  { VIRTUAL_INSN_X_CTI_CHAIN, IDX (INSN_X_CTI_CHAIN), FULL (x_cti_chain) FAST (x_cti_chain) },
  { VIRTUAL_INSN_X_CHAIN, IDX (INSN_X_CHAIN), FULL (x_chain) FAST (x_chain) },
  { VIRTUAL_INSN_X_BEGIN, IDX (INSN_X_BEGIN), FULL (x_begin) FAST (x_begin) },
  { TYPE (INSN_ADD), IDX (INSN_ADD), FULL (add) FAST (add) },
  { TYPE (INSN_ADD3), IDX (INSN_ADD3), FULL (add3) FAST (add3) },
  { TYPE (INSN_AND), IDX (INSN_AND), FULL (and) FAST (and) },
  { TYPE (INSN_AND3), IDX (INSN_AND3), FULL (and3) FAST (and3) },
  { TYPE (INSN_OR), IDX (INSN_OR), FULL (or) FAST (or) },
  { TYPE (INSN_OR3), IDX (INSN_OR3), FULL (or3) FAST (or3) },
  { TYPE (INSN_XOR), IDX (INSN_XOR), FULL (xor) FAST (xor) },
  { TYPE (INSN_XOR3), IDX (INSN_XOR3), FULL (xor3) FAST (xor3) },
  { TYPE (INSN_ADDI), IDX (INSN_ADDI), FULL (addi) FAST (addi) },
  { TYPE (INSN_ADDV), IDX (INSN_ADDV), FULL (addv) FAST (addv) },
  { TYPE (INSN_ADDV3), IDX (INSN_ADDV3), FULL (addv3) FAST (addv3) },
  { TYPE (INSN_ADDX), IDX (INSN_ADDX), FULL (addx) FAST (addx) },
  { TYPE (INSN_BC8), IDX (INSN_BC8), FULL (bc8) FAST (bc8) },
  { TYPE (INSN_BC24), IDX (INSN_BC24), FULL (bc24) FAST (bc24) },
  { TYPE (INSN_BEQ), IDX (INSN_BEQ), FULL (beq) FAST (beq) },
  { TYPE (INSN_BEQZ), IDX (INSN_BEQZ), FULL (beqz) FAST (beqz) },
  { TYPE (INSN_BGEZ), IDX (INSN_BGEZ), FULL (bgez) FAST (bgez) },
  { TYPE (INSN_BGTZ), IDX (INSN_BGTZ), FULL (bgtz) FAST (bgtz) },
  { TYPE (INSN_BLEZ), IDX (INSN_BLEZ), FULL (blez) FAST (blez) },
  { TYPE (INSN_BLTZ), IDX (INSN_BLTZ), FULL (bltz) FAST (bltz) },
  { TYPE (INSN_BNEZ), IDX (INSN_BNEZ), FULL (bnez) FAST (bnez) },
  { TYPE (INSN_BL8), IDX (INSN_BL8), FULL (bl8) FAST (bl8) },
  { TYPE (INSN_BL24), IDX (INSN_BL24), FULL (bl24) FAST (bl24) },
  { TYPE (INSN_BNC8), IDX (INSN_BNC8), FULL (bnc8) FAST (bnc8) },
  { TYPE (INSN_BNC24), IDX (INSN_BNC24), FULL (bnc24) FAST (bnc24) },
  { TYPE (INSN_BNE), IDX (INSN_BNE), FULL (bne) FAST (bne) },
  { TYPE (INSN_BRA8), IDX (INSN_BRA8), FULL (bra8) FAST (bra8) },
  { TYPE (INSN_BRA24), IDX (INSN_BRA24), FULL (bra24) FAST (bra24) },
  { TYPE (INSN_CMP), IDX (INSN_CMP), FULL (cmp) FAST (cmp) },
  { TYPE (INSN_CMPI), IDX (INSN_CMPI), FULL (cmpi) FAST (cmpi) },
  { TYPE (INSN_CMPU), IDX (INSN_CMPU), FULL (cmpu) FAST (cmpu) },
  { TYPE (INSN_CMPUI), IDX (INSN_CMPUI), FULL (cmpui) FAST (cmpui) },
  { TYPE (INSN_DIV), IDX (INSN_DIV), FULL (div) FAST (div) },
  { TYPE (INSN_DIVU), IDX (INSN_DIVU), FULL (divu) FAST (divu) },
  { TYPE (INSN_REM), IDX (INSN_REM), FULL (rem) FAST (rem) },
  { TYPE (INSN_REMU), IDX (INSN_REMU), FULL (remu) FAST (remu) },
  { TYPE (INSN_JL), IDX (INSN_JL), FULL (jl) FAST (jl) },
  { TYPE (INSN_JMP), IDX (INSN_JMP), FULL (jmp) FAST (jmp) },
  { TYPE (INSN_LD), IDX (INSN_LD), FULL (ld) FAST (ld) },
  { TYPE (INSN_LD_D), IDX (INSN_LD_D), FULL (ld_d) FAST (ld_d) },
  { TYPE (INSN_LDB), IDX (INSN_LDB), FULL (ldb) FAST (ldb) },
  { TYPE (INSN_LDB_D), IDX (INSN_LDB_D), FULL (ldb_d) FAST (ldb_d) },
  { TYPE (INSN_LDH), IDX (INSN_LDH), FULL (ldh) FAST (ldh) },
  { TYPE (INSN_LDH_D), IDX (INSN_LDH_D), FULL (ldh_d) FAST (ldh_d) },
  { TYPE (INSN_LDUB), IDX (INSN_LDUB), FULL (ldub) FAST (ldub) },
  { TYPE (INSN_LDUB_D), IDX (INSN_LDUB_D), FULL (ldub_d) FAST (ldub_d) },
  { TYPE (INSN_LDUH), IDX (INSN_LDUH), FULL (lduh) FAST (lduh) },
  { TYPE (INSN_LDUH_D), IDX (INSN_LDUH_D), FULL (lduh_d) FAST (lduh_d) },
  { TYPE (INSN_LD_PLUS), IDX (INSN_LD_PLUS), FULL (ld_plus) FAST (ld_plus) },
  { TYPE (INSN_LD24), IDX (INSN_LD24), FULL (ld24) FAST (ld24) },
  { TYPE (INSN_LDI8), IDX (INSN_LDI8), FULL (ldi8) FAST (ldi8) },
  { TYPE (INSN_LDI16), IDX (INSN_LDI16), FULL (ldi16) FAST (ldi16) },
  { TYPE (INSN_LOCK), IDX (INSN_LOCK), FULL (lock) FAST (lock) },
  { TYPE (INSN_MACHI), IDX (INSN_MACHI), FULL (machi) FAST (machi) },
  { TYPE (INSN_MACLO), IDX (INSN_MACLO), FULL (maclo) FAST (maclo) },
  { TYPE (INSN_MACWHI), IDX (INSN_MACWHI), FULL (macwhi) FAST (macwhi) },
  { TYPE (INSN_MACWLO), IDX (INSN_MACWLO), FULL (macwlo) FAST (macwlo) },
  { TYPE (INSN_MUL), IDX (INSN_MUL), FULL (mul) FAST (mul) },
  { TYPE (INSN_MULHI), IDX (INSN_MULHI), FULL (mulhi) FAST (mulhi) },
  { TYPE (INSN_MULLO), IDX (INSN_MULLO), FULL (mullo) FAST (mullo) },
  { TYPE (INSN_MULWHI), IDX (INSN_MULWHI), FULL (mulwhi) FAST (mulwhi) },
  { TYPE (INSN_MULWLO), IDX (INSN_MULWLO), FULL (mulwlo) FAST (mulwlo) },
  { TYPE (INSN_MV), IDX (INSN_MV), FULL (mv) FAST (mv) },
  { TYPE (INSN_MVFACHI), IDX (INSN_MVFACHI), FULL (mvfachi) FAST (mvfachi) },
  { TYPE (INSN_MVFACLO), IDX (INSN_MVFACLO), FULL (mvfaclo) FAST (mvfaclo) },
  { TYPE (INSN_MVFACMI), IDX (INSN_MVFACMI), FULL (mvfacmi) FAST (mvfacmi) },
  { TYPE (INSN_MVFC), IDX (INSN_MVFC), FULL (mvfc) FAST (mvfc) },
  { TYPE (INSN_MVTACHI), IDX (INSN_MVTACHI), FULL (mvtachi) FAST (mvtachi) },
  { TYPE (INSN_MVTACLO), IDX (INSN_MVTACLO), FULL (mvtaclo) FAST (mvtaclo) },
  { TYPE (INSN_MVTC), IDX (INSN_MVTC), FULL (mvtc) FAST (mvtc) },
  { TYPE (INSN_NEG), IDX (INSN_NEG), FULL (neg) FAST (neg) },
  { TYPE (INSN_NOP), IDX (INSN_NOP), FULL (nop) FAST (nop) },
  { TYPE (INSN_NOT), IDX (INSN_NOT), FULL (not) FAST (not) },
  { TYPE (INSN_RAC), IDX (INSN_RAC), FULL (rac) FAST (rac) },
  { TYPE (INSN_RACH), IDX (INSN_RACH), FULL (rach) FAST (rach) },
  { TYPE (INSN_RTE), IDX (INSN_RTE), FULL (rte) FAST (rte) },
  { TYPE (INSN_SETH), IDX (INSN_SETH), FULL (seth) FAST (seth) },
  { TYPE (INSN_SLL), IDX (INSN_SLL), FULL (sll) FAST (sll) },
  { TYPE (INSN_SLL3), IDX (INSN_SLL3), FULL (sll3) FAST (sll3) },
  { TYPE (INSN_SLLI), IDX (INSN_SLLI), FULL (slli) FAST (slli) },
  { TYPE (INSN_SRA), IDX (INSN_SRA), FULL (sra) FAST (sra) },
  { TYPE (INSN_SRA3), IDX (INSN_SRA3), FULL (sra3) FAST (sra3) },
  { TYPE (INSN_SRAI), IDX (INSN_SRAI), FULL (srai) FAST (srai) },
  { TYPE (INSN_SRL), IDX (INSN_SRL), FULL (srl) FAST (srl) },
  { TYPE (INSN_SRL3), IDX (INSN_SRL3), FULL (srl3) FAST (srl3) },
  { TYPE (INSN_SRLI), IDX (INSN_SRLI), FULL (srli) FAST (srli) },
  { TYPE (INSN_ST), IDX (INSN_ST), FULL (st) FAST (st) },
  { TYPE (INSN_ST_D), IDX (INSN_ST_D), FULL (st_d) FAST (st_d) },
  { TYPE (INSN_STB), IDX (INSN_STB), FULL (stb) FAST (stb) },
  { TYPE (INSN_STB_D), IDX (INSN_STB_D), FULL (stb_d) FAST (stb_d) },
  { TYPE (INSN_STH), IDX (INSN_STH), FULL (sth) FAST (sth) },
  { TYPE (INSN_STH_D), IDX (INSN_STH_D), FULL (sth_d) FAST (sth_d) },
  { TYPE (INSN_ST_PLUS), IDX (INSN_ST_PLUS), FULL (st_plus) FAST (st_plus) },
  { TYPE (INSN_ST_MINUS), IDX (INSN_ST_MINUS), FULL (st_minus) FAST (st_minus) },
  { TYPE (INSN_SUB), IDX (INSN_SUB), FULL (sub) FAST (sub) },
  { TYPE (INSN_SUBV), IDX (INSN_SUBV), FULL (subv) FAST (subv) },
  { TYPE (INSN_SUBX), IDX (INSN_SUBX), FULL (subx) FAST (subx) },
  { TYPE (INSN_TRAP), IDX (INSN_TRAP), FULL (trap) FAST (trap) },
  { TYPE (INSN_UNLOCK), IDX (INSN_UNLOCK), FULL (unlock) FAST (unlock) },
};

static const struct insn_sem m32rbf_insn_sem_invalid =
{
  VIRTUAL_INSN_X_INVALID, IDX (INSN_X_INVALID), FULL (x_invalid) FAST (x_invalid)
};

#undef FMT
#undef FULL
#undef FAST
#undef IDX
#undef TYPE

/* Initialize an IDESC from the compile-time computable parts.  */

static INLINE void
init_idesc (SIM_CPU *cpu, IDESC *id, const struct insn_sem *t)
{
  const CGEN_INSN *insn_table = CGEN_CPU_INSN_TABLE (CPU_CPU_DESC (cpu))->init_entries;

  id->num = t->index;
  if ((int) t->type <= 0)
    id->idata = & cgen_virtual_insn_table[- (int) t->type];
  else
    id->idata = & insn_table[t->type];
  id->attrs = CGEN_INSN_ATTRS (id->idata);
  /* Oh my god, a magic number.  */
  id->length = CGEN_INSN_BITSIZE (id->idata) / 8;
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
m32rbf_init_idesc_table (SIM_CPU *cpu)
{
  IDESC *id,*tabend;
  const struct insn_sem *t,*tend;
  int tabsize = M32RBF_INSN_MAX;
  IDESC *table = m32rbf_insn_data;

  memset (table, 0, tabsize * sizeof (IDESC));

  /* First set all entries to the `invalid insn'.  */
  t = & m32rbf_insn_sem_invalid;
  for (id = table, tabend = table + tabsize; id < tabend; ++id)
    init_idesc (cpu, id, t);

  /* Now fill in the values for the chosen cpu.  */
  for (t = m32rbf_insn_sem, tend = t + sizeof (m32rbf_insn_sem) / sizeof (*t);
       t != tend; ++t)
    {
      init_idesc (cpu, & table[t->index], t);
    }

  /* Link the IDESC table into the cpu.  */
  CPU_IDESC (cpu) = table;
}

/* Given an instruction, return a pointer to its IDESC entry.  */

const IDESC *
m32rbf_decode (SIM_CPU *current_cpu, IADDR pc,
              CGEN_INSN_INT base_insn, CGEN_INSN_INT entire_insn,
              ARGBUF *abuf)
{
  /* Result of decoder.  */
  M32RBF_INSN_TYPE itype;

  {
    CGEN_INSN_INT insn = base_insn;

    {
      unsigned int val = (((insn >> 8) & (15 << 4)) | ((insn >> 4) & (15 << 0)));
      switch (val)
      {
      case 0 : itype = M32RBF_INSN_SUBV; goto extract_fmt_addv;
      case 1 : itype = M32RBF_INSN_SUBX; goto extract_fmt_addx;
      case 2 : itype = M32RBF_INSN_SUB; goto extract_fmt_add;
      case 3 : itype = M32RBF_INSN_NEG; goto extract_fmt_mv;
      case 4 : itype = M32RBF_INSN_CMP; goto extract_fmt_cmp;
      case 5 : itype = M32RBF_INSN_CMPU; goto extract_fmt_cmp;
      case 8 : itype = M32RBF_INSN_ADDV; goto extract_fmt_addv;
      case 9 : itype = M32RBF_INSN_ADDX; goto extract_fmt_addx;
      case 10 : itype = M32RBF_INSN_ADD; goto extract_fmt_add;
      case 11 : itype = M32RBF_INSN_NOT; goto extract_fmt_mv;
      case 12 : itype = M32RBF_INSN_AND; goto extract_fmt_add;
      case 13 : itype = M32RBF_INSN_XOR; goto extract_fmt_add;
      case 14 : itype = M32RBF_INSN_OR; goto extract_fmt_add;
      case 16 : itype = M32RBF_INSN_SRL; goto extract_fmt_add;
      case 18 : itype = M32RBF_INSN_SRA; goto extract_fmt_add;
      case 20 : itype = M32RBF_INSN_SLL; goto extract_fmt_add;
      case 22 : itype = M32RBF_INSN_MUL; goto extract_fmt_add;
      case 24 : itype = M32RBF_INSN_MV; goto extract_fmt_mv;
      case 25 : itype = M32RBF_INSN_MVFC; goto extract_fmt_mvfc;
      case 26 : itype = M32RBF_INSN_MVTC; goto extract_fmt_mvtc;
      case 28 :
        {
          unsigned int val = (((insn >> 8) & (15 << 0)));
          switch (val)
          {
          case 14 : itype = M32RBF_INSN_JL; goto extract_fmt_jl;
          case 15 : itype = M32RBF_INSN_JMP; goto extract_fmt_jmp;
          default : itype = M32RBF_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 29 : itype = M32RBF_INSN_RTE; goto extract_fmt_rte;
      case 31 : itype = M32RBF_INSN_TRAP; goto extract_fmt_trap;
      case 32 : itype = M32RBF_INSN_STB; goto extract_fmt_stb;
      case 34 : itype = M32RBF_INSN_STH; goto extract_fmt_sth;
      case 36 : itype = M32RBF_INSN_ST; goto extract_fmt_st;
      case 37 : itype = M32RBF_INSN_UNLOCK; goto extract_fmt_unlock;
      case 38 : itype = M32RBF_INSN_ST_PLUS; goto extract_fmt_st_plus;
      case 39 : itype = M32RBF_INSN_ST_MINUS; goto extract_fmt_st_plus;
      case 40 : itype = M32RBF_INSN_LDB; goto extract_fmt_ldb;
      case 41 : itype = M32RBF_INSN_LDUB; goto extract_fmt_ldb;
      case 42 : itype = M32RBF_INSN_LDH; goto extract_fmt_ldh;
      case 43 : itype = M32RBF_INSN_LDUH; goto extract_fmt_ldh;
      case 44 : itype = M32RBF_INSN_LD; goto extract_fmt_ld;
      case 45 : itype = M32RBF_INSN_LOCK; goto extract_fmt_lock;
      case 46 : itype = M32RBF_INSN_LD_PLUS; goto extract_fmt_ld_plus;
      case 48 : itype = M32RBF_INSN_MULHI; goto extract_fmt_mulhi;
      case 49 : itype = M32RBF_INSN_MULLO; goto extract_fmt_mulhi;
      case 50 : itype = M32RBF_INSN_MULWHI; goto extract_fmt_mulhi;
      case 51 : itype = M32RBF_INSN_MULWLO; goto extract_fmt_mulhi;
      case 52 : itype = M32RBF_INSN_MACHI; goto extract_fmt_machi;
      case 53 : itype = M32RBF_INSN_MACLO; goto extract_fmt_machi;
      case 54 : itype = M32RBF_INSN_MACWHI; goto extract_fmt_machi;
      case 55 : itype = M32RBF_INSN_MACWLO; goto extract_fmt_machi;
      case 64 : /* fall through */
      case 65 : /* fall through */
      case 66 : /* fall through */
      case 67 : /* fall through */
      case 68 : /* fall through */
      case 69 : /* fall through */
      case 70 : /* fall through */
      case 71 : /* fall through */
      case 72 : /* fall through */
      case 73 : /* fall through */
      case 74 : /* fall through */
      case 75 : /* fall through */
      case 76 : /* fall through */
      case 77 : /* fall through */
      case 78 : /* fall through */
      case 79 : itype = M32RBF_INSN_ADDI; goto extract_fmt_addi;
      case 80 : /* fall through */
      case 81 : itype = M32RBF_INSN_SRLI; goto extract_fmt_slli;
      case 82 : /* fall through */
      case 83 : itype = M32RBF_INSN_SRAI; goto extract_fmt_slli;
      case 84 : /* fall through */
      case 85 : itype = M32RBF_INSN_SLLI; goto extract_fmt_slli;
      case 87 :
        {
          unsigned int val = (((insn >> 0) & (15 << 0)));
          switch (val)
          {
          case 0 : itype = M32RBF_INSN_MVTACHI; goto extract_fmt_mvtachi;
          case 1 : itype = M32RBF_INSN_MVTACLO; goto extract_fmt_mvtachi;
          default : itype = M32RBF_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 88 : itype = M32RBF_INSN_RACH; goto extract_fmt_rac;
      case 89 : itype = M32RBF_INSN_RAC; goto extract_fmt_rac;
      case 95 :
        {
          unsigned int val = (((insn >> 0) & (15 << 0)));
          switch (val)
          {
          case 0 : itype = M32RBF_INSN_MVFACHI; goto extract_fmt_mvfachi;
          case 1 : itype = M32RBF_INSN_MVFACLO; goto extract_fmt_mvfachi;
          case 2 : itype = M32RBF_INSN_MVFACMI; goto extract_fmt_mvfachi;
          default : itype = M32RBF_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 96 : /* fall through */
      case 97 : /* fall through */
      case 98 : /* fall through */
      case 99 : /* fall through */
      case 100 : /* fall through */
      case 101 : /* fall through */
      case 102 : /* fall through */
      case 103 : /* fall through */
      case 104 : /* fall through */
      case 105 : /* fall through */
      case 106 : /* fall through */
      case 107 : /* fall through */
      case 108 : /* fall through */
      case 109 : /* fall through */
      case 110 : /* fall through */
      case 111 : itype = M32RBF_INSN_LDI8; goto extract_fmt_ldi8;
      case 112 :
        {
          unsigned int val = (((insn >> 8) & (15 << 0)));
          switch (val)
          {
          case 0 : itype = M32RBF_INSN_NOP; goto extract_fmt_nop;
          case 12 : itype = M32RBF_INSN_BC8; goto extract_fmt_bc8;
          case 13 : itype = M32RBF_INSN_BNC8; goto extract_fmt_bc8;
          case 14 : itype = M32RBF_INSN_BL8; goto extract_fmt_bl8;
          case 15 : itype = M32RBF_INSN_BRA8; goto extract_fmt_bra8;
          default : itype = M32RBF_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
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
      case 127 :
        {
          unsigned int val = (((insn >> 8) & (15 << 0)));
          switch (val)
          {
          case 12 : itype = M32RBF_INSN_BC8; goto extract_fmt_bc8;
          case 13 : itype = M32RBF_INSN_BNC8; goto extract_fmt_bc8;
          case 14 : itype = M32RBF_INSN_BL8; goto extract_fmt_bl8;
          case 15 : itype = M32RBF_INSN_BRA8; goto extract_fmt_bra8;
          default : itype = M32RBF_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 132 : itype = M32RBF_INSN_CMPI; goto extract_fmt_cmpi;
      case 133 : itype = M32RBF_INSN_CMPUI; goto extract_fmt_cmpi;
      case 136 : itype = M32RBF_INSN_ADDV3; goto extract_fmt_addv3;
      case 138 : itype = M32RBF_INSN_ADD3; goto extract_fmt_add3;
      case 140 : itype = M32RBF_INSN_AND3; goto extract_fmt_and3;
      case 141 : itype = M32RBF_INSN_XOR3; goto extract_fmt_and3;
      case 142 : itype = M32RBF_INSN_OR3; goto extract_fmt_or3;
      case 144 : itype = M32RBF_INSN_DIV; goto extract_fmt_div;
      case 145 : itype = M32RBF_INSN_DIVU; goto extract_fmt_div;
      case 146 : itype = M32RBF_INSN_REM; goto extract_fmt_div;
      case 147 : itype = M32RBF_INSN_REMU; goto extract_fmt_div;
      case 152 : itype = M32RBF_INSN_SRL3; goto extract_fmt_sll3;
      case 154 : itype = M32RBF_INSN_SRA3; goto extract_fmt_sll3;
      case 156 : itype = M32RBF_INSN_SLL3; goto extract_fmt_sll3;
      case 159 : itype = M32RBF_INSN_LDI16; goto extract_fmt_ldi16;
      case 160 : itype = M32RBF_INSN_STB_D; goto extract_fmt_stb_d;
      case 162 : itype = M32RBF_INSN_STH_D; goto extract_fmt_sth_d;
      case 164 : itype = M32RBF_INSN_ST_D; goto extract_fmt_st_d;
      case 168 : itype = M32RBF_INSN_LDB_D; goto extract_fmt_ldb_d;
      case 169 : itype = M32RBF_INSN_LDUB_D; goto extract_fmt_ldb_d;
      case 170 : itype = M32RBF_INSN_LDH_D; goto extract_fmt_ldh_d;
      case 171 : itype = M32RBF_INSN_LDUH_D; goto extract_fmt_ldh_d;
      case 172 : itype = M32RBF_INSN_LD_D; goto extract_fmt_ld_d;
      case 176 : itype = M32RBF_INSN_BEQ; goto extract_fmt_beq;
      case 177 : itype = M32RBF_INSN_BNE; goto extract_fmt_beq;
      case 184 : itype = M32RBF_INSN_BEQZ; goto extract_fmt_beqz;
      case 185 : itype = M32RBF_INSN_BNEZ; goto extract_fmt_beqz;
      case 186 : itype = M32RBF_INSN_BLTZ; goto extract_fmt_beqz;
      case 187 : itype = M32RBF_INSN_BGEZ; goto extract_fmt_beqz;
      case 188 : itype = M32RBF_INSN_BLEZ; goto extract_fmt_beqz;
      case 189 : itype = M32RBF_INSN_BGTZ; goto extract_fmt_beqz;
      case 220 : itype = M32RBF_INSN_SETH; goto extract_fmt_seth;
      case 224 : /* fall through */
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
      case 239 : itype = M32RBF_INSN_LD24; goto extract_fmt_ld24;
      case 240 : /* fall through */
      case 241 : /* fall through */
      case 242 : /* fall through */
      case 243 : /* fall through */
      case 244 : /* fall through */
      case 245 : /* fall through */
      case 246 : /* fall through */
      case 247 : /* fall through */
      case 248 : /* fall through */
      case 249 : /* fall through */
      case 250 : /* fall through */
      case 251 : /* fall through */
      case 252 : /* fall through */
      case 253 : /* fall through */
      case 254 : /* fall through */
      case 255 :
        {
          unsigned int val = (((insn >> 8) & (15 << 0)));
          switch (val)
          {
          case 12 : itype = M32RBF_INSN_BC24; goto extract_fmt_bc24;
          case 13 : itype = M32RBF_INSN_BNC24; goto extract_fmt_bc24;
          case 14 : itype = M32RBF_INSN_BL24; goto extract_fmt_bl24;
          case 15 : itype = M32RBF_INSN_BRA24; goto extract_fmt_bra24;
          default : itype = M32RBF_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      default : itype = M32RBF_INSN_X_INVALID; goto extract_fmt_empty;
      }
    }
  }

  /* The instruction has been decoded, now extract the fields.  */

 extract_fmt_empty:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_empty.f
  EXTRACT_IFMT_EMPTY_VARS /* */

  EXTRACT_IFMT_EMPTY_CODE

  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_empty", (char *) 0));

#undef FLD
    return idesc;
  }

 extract_fmt_add:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_add3:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_and3:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_or3:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_addi:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_addv:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_addv3:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_addx:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_bc8:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_bc24:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_beq:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_beqz:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_bl8:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_bl24:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_bra8:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_bra24:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_cmp:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_cmpi:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_div:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_jl:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_jl.f
  EXTRACT_IFMT_JL_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_IFMT_JL_CODE

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
    return idesc;
  }

 extract_fmt_jmp:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_jmp.f
  EXTRACT_IFMT_JL_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_IFMT_JL_CODE

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
    return idesc;
  }

 extract_fmt_ld:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ld_d:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ldb:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ldb_d:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ldh:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ldh_d:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ld_plus:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ld24:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ldi8:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ldi16:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_lock:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_machi:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_machi.f
  EXTRACT_IFMT_CMP_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_IFMT_CMP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_src1) = & CPU (h_gr)[f_r1];
  FLD (i_src2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_machi", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src1) = f_r1;
      FLD (in_src2) = f_r2;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_mulhi:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_mulhi.f
  EXTRACT_IFMT_CMP_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_IFMT_CMP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_src1) = & CPU (h_gr)[f_r1];
  FLD (i_src2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_mulhi", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src1) = f_r1;
      FLD (in_src2) = f_r2;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_mv:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_mvfachi:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_mvfachi.f
  EXTRACT_IFMT_MVFACHI_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_IFMT_MVFACHI_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_dr) = & CPU (h_gr)[f_r1];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_mvfachi", "dr 0x%x", 'x', f_r1, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_mvfc:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_mvtachi:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_mvtachi.f
  EXTRACT_IFMT_MVTACHI_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_IFMT_MVTACHI_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_src1) = & CPU (h_gr)[f_r1];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_mvtachi", "src1 0x%x", 'x', f_r1, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src1) = f_r1;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_mvtc:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_nop:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_nop.f
  EXTRACT_IFMT_NOP_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_IFMT_NOP_CODE

  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_nop", (char *) 0));

#undef FLD
    return idesc;
  }

 extract_fmt_rac:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_rac.f
  EXTRACT_IFMT_NOP_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_IFMT_NOP_CODE

  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_rac", (char *) 0));

#undef FLD
    return idesc;
  }

 extract_fmt_rte:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_seth:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_sll3:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_slli:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_st:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_st_d:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_stb:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_stb_d:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_sth:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_sth_d:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_st_plus:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_trap:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_unlock:
  {
    const IDESC *idesc = &m32rbf_insn_data[itype];
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
    return idesc;
  }

}
