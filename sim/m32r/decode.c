/* Simulator instruction decoder for m32rbf.

THIS FILE IS MACHINE GENERATED WITH CGEN.

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

/* The instruction descriptor array.
   This is computed at runtime.  Space for it is not malloc'd to save a
   teensy bit of cpu in the decoder.  Moving it to malloc space is trivial
   but won't be done until necessary (we don't currently support the runtime
   addition of instructions nor an SMP machine with different cpus).  */
static IDESC m32rbf_insn_data[M32RBF_INSN_MAX];

/* Instruction semantic handlers and support.
   This struct defines the part of an IDESC that can be computed at
   compile time.  */

struct insn_sem {
  /* The instruction type (a number that identifies each insn over the
     entire architecture).  */
  CGEN_INSN_TYPE type;

  /* Index in IDESC table.  */
  int index;

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
#define IDX(insn) CONCAT2 (M32RBF_,insn)
#define TYPE(insn) CONCAT2 (M32R_,insn)

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

/* Enum declaration for all instruction formats.  */
typedef enum ifmt {
  FMT_EMPTY, FMT_ADD, FMT_ADD3, FMT_AND3
 , FMT_OR3, FMT_ADDI, FMT_ADDV, FMT_ADDV3
 , FMT_ADDX, FMT_BC8, FMT_BC24, FMT_BEQ
 , FMT_BEQZ, FMT_BL8, FMT_BL24, FMT_BRA8
 , FMT_BRA24, FMT_CMP, FMT_CMPI, FMT_DIV
 , FMT_JL, FMT_JMP, FMT_LD, FMT_LD_D
 , FMT_LDB, FMT_LDB_D, FMT_LDH, FMT_LDH_D
 , FMT_LD_PLUS, FMT_LD24, FMT_LDI8, FMT_LDI16
 , FMT_LOCK, FMT_MACHI, FMT_MULHI, FMT_MV
 , FMT_MVFACHI, FMT_MVFC, FMT_MVTACHI, FMT_MVTC
 , FMT_NOP, FMT_RAC, FMT_RTE, FMT_SETH
 , FMT_SLL3, FMT_SLLI, FMT_ST, FMT_ST_D
 , FMT_STB, FMT_STB_D, FMT_STH, FMT_STH_D
 , FMT_ST_PLUS, FMT_TRAP, FMT_UNLOCK
} IFMT;

/* The decoder uses this to record insns and direct extraction handling.  */

typedef struct {
  const IDESC *idesc;
#ifdef __GNUC__
  void *ifmt;
#else
  enum ifmt ifmt;
#endif
} DECODE_DESC;

/* Macro to go from decode phase to extraction phase.  */

#ifdef __GNUC__
#define GOTO_EXTRACT(id) goto *(id)->ifmt
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
m32rbf_decode (SIM_CPU *current_cpu, PCADDR pc,
              CGEN_INSN_INT base_insn, CGEN_INSN_INT entire_insn,
              ARGBUF *abuf)
{
  /* Result of decoder, used by extractor.  */
  const DECODE_DESC *idecode;

  /* First decode the instruction.  */

  {
#define I(insn) & m32rbf_insn_data[CONCAT2 (M32RBF_,insn)]
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
        && default_0, && default_0, && default_0, && default_0, 
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
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
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
        { I (INSN_MULHI), E (FMT_MULHI) }, { I (INSN_MULLO), E (FMT_MULHI) }, 
        { I (INSN_MULWHI), E (FMT_MULHI) }, { I (INSN_MULWLO), E (FMT_MULHI) }, 
        { I (INSN_MACHI), E (FMT_MACHI) }, { I (INSN_MACLO), E (FMT_MACHI) }, 
        { I (INSN_MACWHI), E (FMT_MACHI) }, { I (INSN_MACWLO), E (FMT_MACHI) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
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
        { I (INSN_RACH), E (FMT_RAC) }, { I (INSN_RAC), E (FMT_RAC) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { 0 }, 
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
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_ADDV3), E (FMT_ADDV3) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_ADD3), E (FMT_ADD3) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_AND3), E (FMT_AND3) }, { I (INSN_XOR3), E (FMT_AND3) }, 
        { I (INSN_OR3), E (FMT_OR3) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_DIV), E (FMT_DIV) }, { I (INSN_DIVU), E (FMT_DIV) }, 
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
        CASE (0, 28) :
          {
            static const DECODE_DESC insns[16] = {
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_JL), E (FMT_JL) }, { I (INSN_JMP), E (FMT_JMP) }, 
            };
            unsigned int val = (((insn >> 8) & (15 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        CASE (0, 87) :
          {
            static const DECODE_DESC insns[16] = {
              { I (INSN_MVTACHI), E (FMT_MVTACHI) }, { I (INSN_MVTACLO), E (FMT_MVTACHI) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
            };
            unsigned int val = (((insn >> 0) & (15 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        CASE (0, 95) :
          {
            static const DECODE_DESC insns[16] = {
              { I (INSN_MVFACHI), E (FMT_MVFACHI) }, { I (INSN_MVFACLO), E (FMT_MVFACHI) }, 
              { I (INSN_MVFACMI), E (FMT_MVFACHI) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
            };
            unsigned int val = (((insn >> 0) & (15 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        CASE (0, 112) :
          {
            static const DECODE_DESC insns[16] = {
              { I (INSN_NOP), E (FMT_NOP) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
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
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_BC8), E (FMT_BC8) }, { I (INSN_BNC8), E (FMT_BC8) }, 
              { I (INSN_BL8), E (FMT_BL8) }, { I (INSN_BRA8), E (FMT_BRA8) }, 
            };
            unsigned int val = (((insn >> 8) & (15 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
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
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
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
    switch (idecode->ifmt)
#endif
      {

  CASE (ex, FMT_EMPTY) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_empty.f
  EXTRACT_FMT_EMPTY_VARS /* */

  EXTRACT_FMT_EMPTY_CODE

  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_empty", (char *) 0));

#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_ADD) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_add.f
  EXTRACT_FMT_ADD_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_ADD_CODE

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
  EXTRACT_FMT_ADD3_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_FMT_ADD3_CODE

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
  EXTRACT_FMT_AND3_VARS /* f-op1 f-r1 f-op2 f-r2 f-uimm16 */

  EXTRACT_FMT_AND3_CODE

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
  EXTRACT_FMT_OR3_VARS /* f-op1 f-r1 f-op2 f-r2 f-uimm16 */

  EXTRACT_FMT_OR3_CODE

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
  EXTRACT_FMT_ADDI_VARS /* f-op1 f-r1 f-simm8 */

  EXTRACT_FMT_ADDI_CODE

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
  EXTRACT_FMT_ADDV_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_ADDV_CODE

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
  EXTRACT_FMT_ADDV3_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_FMT_ADDV3_CODE

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
  EXTRACT_FMT_ADDX_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_ADDX_CODE

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
  EXTRACT_FMT_BC8_VARS /* f-op1 f-r1 f-disp8 */

  EXTRACT_FMT_BC8_CODE

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
  EXTRACT_FMT_BC24_VARS /* f-op1 f-r1 f-disp24 */

  EXTRACT_FMT_BC24_CODE

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
  EXTRACT_FMT_BEQ_VARS /* f-op1 f-r1 f-op2 f-r2 f-disp16 */

  EXTRACT_FMT_BEQ_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_src1) = & CPU (h_gr)[f_r1];
  FLD (i_src2) = & CPU (h_gr)[f_r2];
  FLD (i_disp16) = f_disp16;
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_beq", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, "disp16 0x%x", 'x', f_disp16, (char *) 0));

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
  EXTRACT_FMT_BEQZ_VARS /* f-op1 f-r1 f-op2 f-r2 f-disp16 */

  EXTRACT_FMT_BEQZ_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_src2) = & CPU (h_gr)[f_r2];
  FLD (i_disp16) = f_disp16;
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_beqz", "src2 0x%x", 'x', f_r2, "disp16 0x%x", 'x', f_disp16, (char *) 0));

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
  EXTRACT_FMT_BL8_VARS /* f-op1 f-r1 f-disp8 */

  EXTRACT_FMT_BL8_CODE

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
  EXTRACT_FMT_BL24_VARS /* f-op1 f-r1 f-disp24 */

  EXTRACT_FMT_BL24_CODE

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

  CASE (ex, FMT_BRA8) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_bra8.f
  EXTRACT_FMT_BRA8_VARS /* f-op1 f-r1 f-disp8 */

  EXTRACT_FMT_BRA8_CODE

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
  EXTRACT_FMT_BRA24_VARS /* f-op1 f-r1 f-disp24 */

  EXTRACT_FMT_BRA24_CODE

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
  EXTRACT_FMT_CMP_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_CMP_CODE

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
  EXTRACT_FMT_CMPI_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_FMT_CMPI_CODE

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

  CASE (ex, FMT_DIV) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_div.f
  EXTRACT_FMT_DIV_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_FMT_DIV_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_sr) = & CPU (h_gr)[f_r2];
  FLD (i_dr) = & CPU (h_gr)[f_r1];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_div", "sr 0x%x", 'x', f_r2, "dr 0x%x", 'x', f_r1, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_sr) = f_r2;
      FLD (in_dr) = f_r1;
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_JL) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_jl.f
  EXTRACT_FMT_JL_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_JL_CODE

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
  EXTRACT_FMT_JMP_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_JMP_CODE

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
  EXTRACT_FMT_LD_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_LD_CODE

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
  EXTRACT_FMT_LD_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_FMT_LD_D_CODE

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
  EXTRACT_FMT_LDB_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_LDB_CODE

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
  EXTRACT_FMT_LDB_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_FMT_LDB_D_CODE

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
  EXTRACT_FMT_LDH_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_LDH_CODE

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
  EXTRACT_FMT_LDH_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_FMT_LDH_D_CODE

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
  EXTRACT_FMT_LD_PLUS_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_LD_PLUS_CODE

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
  EXTRACT_FMT_LD24_VARS /* f-op1 f-r1 f-uimm24 */

  EXTRACT_FMT_LD24_CODE

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
  EXTRACT_FMT_LDI8_VARS /* f-op1 f-r1 f-simm8 */

  EXTRACT_FMT_LDI8_CODE

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
  EXTRACT_FMT_LDI16_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_FMT_LDI16_CODE

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
  EXTRACT_FMT_LOCK_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_LOCK_CODE

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

  CASE (ex, FMT_MACHI) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_machi.f
  EXTRACT_FMT_MACHI_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_MACHI_CODE

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
  BREAK (ex);
  }

  CASE (ex, FMT_MULHI) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_mulhi.f
  EXTRACT_FMT_MULHI_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_MULHI_CODE

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
  BREAK (ex);
  }

  CASE (ex, FMT_MV) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_mv.f
  EXTRACT_FMT_MV_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_MV_CODE

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

  CASE (ex, FMT_MVFACHI) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_mvfachi.f
  EXTRACT_FMT_MVFACHI_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_MVFACHI_CODE

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
  BREAK (ex);
  }

  CASE (ex, FMT_MVFC) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_mvfc.f
  EXTRACT_FMT_MVFC_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_MVFC_CODE

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

  CASE (ex, FMT_MVTACHI) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_mvtachi.f
  EXTRACT_FMT_MVTACHI_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_MVTACHI_CODE

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
  BREAK (ex);
  }

  CASE (ex, FMT_MVTC) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_mvtc.f
  EXTRACT_FMT_MVTC_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_MVTC_CODE

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
  EXTRACT_FMT_NOP_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_NOP_CODE

  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_nop", (char *) 0));

#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_RAC) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_rac.f
  EXTRACT_FMT_RAC_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_RAC_CODE

  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_rac", (char *) 0));

#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_RTE) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_rte.f
  EXTRACT_FMT_RTE_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_RTE_CODE

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
  EXTRACT_FMT_SETH_VARS /* f-op1 f-r1 f-op2 f-r2 f-hi16 */

  EXTRACT_FMT_SETH_CODE

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
  EXTRACT_FMT_SLL3_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_FMT_SLL3_CODE

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
  EXTRACT_FMT_SLLI_VARS /* f-op1 f-r1 f-shift-op2 f-uimm5 */

  EXTRACT_FMT_SLLI_CODE

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
  EXTRACT_FMT_ST_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_ST_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_src2) = & CPU (h_gr)[f_r2];
  FLD (i_src1) = & CPU (h_gr)[f_r1];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_st", "src2 0x%x", 'x', f_r2, "src1 0x%x", 'x', f_r1, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src2) = f_r2;
      FLD (in_src1) = f_r1;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_ST_D) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_st_d.f
  EXTRACT_FMT_ST_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_FMT_ST_D_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_simm16) = f_simm16;
  FLD (i_src2) = & CPU (h_gr)[f_r2];
  FLD (i_src1) = & CPU (h_gr)[f_r1];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_st_d", "f_simm16 0x%x", 'x', f_simm16, "src2 0x%x", 'x', f_r2, "src1 0x%x", 'x', f_r1, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src2) = f_r2;
      FLD (in_src1) = f_r1;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STB) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_stb.f
  EXTRACT_FMT_STB_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_STB_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_src2) = & CPU (h_gr)[f_r2];
  FLD (i_src1) = & CPU (h_gr)[f_r1];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stb", "src2 0x%x", 'x', f_r2, "src1 0x%x", 'x', f_r1, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src2) = f_r2;
      FLD (in_src1) = f_r1;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STB_D) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_stb_d.f
  EXTRACT_FMT_STB_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_FMT_STB_D_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_simm16) = f_simm16;
  FLD (i_src2) = & CPU (h_gr)[f_r2];
  FLD (i_src1) = & CPU (h_gr)[f_r1];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stb_d", "f_simm16 0x%x", 'x', f_simm16, "src2 0x%x", 'x', f_r2, "src1 0x%x", 'x', f_r1, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src2) = f_r2;
      FLD (in_src1) = f_r1;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STH) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_sth.f
  EXTRACT_FMT_STH_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_STH_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_src2) = & CPU (h_gr)[f_r2];
  FLD (i_src1) = & CPU (h_gr)[f_r1];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_sth", "src2 0x%x", 'x', f_r2, "src1 0x%x", 'x', f_r1, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src2) = f_r2;
      FLD (in_src1) = f_r1;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STH_D) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_sth_d.f
  EXTRACT_FMT_STH_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_FMT_STH_D_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_simm16) = f_simm16;
  FLD (i_src2) = & CPU (h_gr)[f_r2];
  FLD (i_src1) = & CPU (h_gr)[f_r1];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_sth_d", "f_simm16 0x%x", 'x', f_simm16, "src2 0x%x", 'x', f_r2, "src1 0x%x", 'x', f_r1, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src2) = f_r2;
      FLD (in_src1) = f_r1;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_ST_PLUS) :
  {
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.fmt_st_plus.f
  EXTRACT_FMT_ST_PLUS_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_ST_PLUS_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_src2) = & CPU (h_gr)[f_r2];
  FLD (i_src1) = & CPU (h_gr)[f_r1];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_st_plus", "src2 0x%x", 'x', f_r2, "src1 0x%x", 'x', f_r1, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src2) = f_r2;
      FLD (in_src1) = f_r1;
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
  EXTRACT_FMT_TRAP_VARS /* f-op1 f-r1 f-op2 f-uimm4 */

  EXTRACT_FMT_TRAP_CODE

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
  EXTRACT_FMT_UNLOCK_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_UNLOCK_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_src2) = & CPU (h_gr)[f_r2];
  FLD (i_src1) = & CPU (h_gr)[f_r1];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_unlock", "src2 0x%x", 'x', f_r2, "src1 0x%x", 'x', f_r1, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src2) = f_r2;
      FLD (in_src1) = f_r1;
    }
#endif
#undef FLD
  BREAK (ex);
  }


      }
    ENDSWITCH (ex)

  }

  return idecode->idesc;
}
