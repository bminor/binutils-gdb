/* Simulator instruction decoder for fr30bf.

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

#define WANT_CPU fr30bf
#define WANT_CPU_FR30BF

#include "sim-main.h"
#include "sim-assert.h"

/* FIXME: Need to review choices for the following.  */

#if WITH_SEM_SWITCH_FULL
#define FULL(fn)
#else
#define FULL(fn) CONCAT3 (fr30bf,_sem_,fn) ,
#endif

#if WITH_FAST
#if WITH_SEM_SWITCH_FAST
#define FAST(fn)
#else
#define FAST(fn) CONCAT3 (fr30bf,_semf_,fn) , /* f for fast */
#endif
#else
#define FAST(fn)
#endif

/* The INSN_ prefix is not here and is instead part of the `insn' argument
   to avoid collisions with header files (e.g. `AND' in ansidecl.h).  */
#define IDX(insn) CONCAT2 (FR30BF_,insn)
#define TYPE(insn) CONCAT2 (FR30_,insn)

/* The instruction descriptor array.
   This is computed at runtime.  Space for it is not malloc'd to save a
   teensy bit of cpu in the decoder.  Moving it to malloc space is trivial
   but won't be done until necessary (we don't currently support the runtime
   addition of instructions nor an SMP machine with different cpus).  */
static IDESC fr30bf_insn_data[FR30BF_INSN_MAX];

/* Commas between elements are contained in the macros.
   Some of these are conditionally compiled out.  */

static const struct insn_sem fr30bf_insn_sem[] =
{
  { VIRTUAL_INSN_X_INVALID, IDX (INSN_X_INVALID), FULL (x_invalid) FAST (x_invalid) },
  { VIRTUAL_INSN_X_AFTER, IDX (INSN_X_AFTER), FULL (x_after) FAST (x_after) },
  { VIRTUAL_INSN_X_BEFORE, IDX (INSN_X_BEFORE), FULL (x_before) FAST (x_before) },
  { VIRTUAL_INSN_X_CTI_CHAIN, IDX (INSN_X_CTI_CHAIN), FULL (x_cti_chain) FAST (x_cti_chain) },
  { VIRTUAL_INSN_X_CHAIN, IDX (INSN_X_CHAIN), FULL (x_chain) FAST (x_chain) },
  { VIRTUAL_INSN_X_BEGIN, IDX (INSN_X_BEGIN), FULL (x_begin) FAST (x_begin) },
  { TYPE (INSN_ADD), IDX (INSN_ADD), FULL (add) FAST (add) },
  { TYPE (INSN_ADDI), IDX (INSN_ADDI), FULL (addi) FAST (addi) },
  { TYPE (INSN_ADD2), IDX (INSN_ADD2), FULL (add2) FAST (add2) },
  { TYPE (INSN_ADDC), IDX (INSN_ADDC), FULL (addc) FAST (addc) },
  { TYPE (INSN_ADDN), IDX (INSN_ADDN), FULL (addn) FAST (addn) },
  { TYPE (INSN_ADDNI), IDX (INSN_ADDNI), FULL (addni) FAST (addni) },
  { TYPE (INSN_ADDN2), IDX (INSN_ADDN2), FULL (addn2) FAST (addn2) },
  { TYPE (INSN_SUB), IDX (INSN_SUB), FULL (sub) FAST (sub) },
  { TYPE (INSN_SUBC), IDX (INSN_SUBC), FULL (subc) FAST (subc) },
  { TYPE (INSN_SUBN), IDX (INSN_SUBN), FULL (subn) FAST (subn) },
  { TYPE (INSN_CMP), IDX (INSN_CMP), FULL (cmp) FAST (cmp) },
  { TYPE (INSN_CMPI), IDX (INSN_CMPI), FULL (cmpi) FAST (cmpi) },
  { TYPE (INSN_CMP2), IDX (INSN_CMP2), FULL (cmp2) FAST (cmp2) },
  { TYPE (INSN_AND), IDX (INSN_AND), FULL (and) FAST (and) },
  { TYPE (INSN_OR), IDX (INSN_OR), FULL (or) FAST (or) },
  { TYPE (INSN_EOR), IDX (INSN_EOR), FULL (eor) FAST (eor) },
  { TYPE (INSN_ANDM), IDX (INSN_ANDM), FULL (andm) FAST (andm) },
  { TYPE (INSN_ANDH), IDX (INSN_ANDH), FULL (andh) FAST (andh) },
  { TYPE (INSN_ANDB), IDX (INSN_ANDB), FULL (andb) FAST (andb) },
  { TYPE (INSN_ORM), IDX (INSN_ORM), FULL (orm) FAST (orm) },
  { TYPE (INSN_ORH), IDX (INSN_ORH), FULL (orh) FAST (orh) },
  { TYPE (INSN_ORB), IDX (INSN_ORB), FULL (orb) FAST (orb) },
  { TYPE (INSN_EORM), IDX (INSN_EORM), FULL (eorm) FAST (eorm) },
  { TYPE (INSN_EORH), IDX (INSN_EORH), FULL (eorh) FAST (eorh) },
  { TYPE (INSN_EORB), IDX (INSN_EORB), FULL (eorb) FAST (eorb) },
  { TYPE (INSN_BANDL), IDX (INSN_BANDL), FULL (bandl) FAST (bandl) },
  { TYPE (INSN_BORL), IDX (INSN_BORL), FULL (borl) FAST (borl) },
  { TYPE (INSN_BEORL), IDX (INSN_BEORL), FULL (beorl) FAST (beorl) },
  { TYPE (INSN_BANDH), IDX (INSN_BANDH), FULL (bandh) FAST (bandh) },
  { TYPE (INSN_BORH), IDX (INSN_BORH), FULL (borh) FAST (borh) },
  { TYPE (INSN_BEORH), IDX (INSN_BEORH), FULL (beorh) FAST (beorh) },
  { TYPE (INSN_BTSTL), IDX (INSN_BTSTL), FULL (btstl) FAST (btstl) },
  { TYPE (INSN_BTSTH), IDX (INSN_BTSTH), FULL (btsth) FAST (btsth) },
  { TYPE (INSN_MUL), IDX (INSN_MUL), FULL (mul) FAST (mul) },
  { TYPE (INSN_MULU), IDX (INSN_MULU), FULL (mulu) FAST (mulu) },
  { TYPE (INSN_MULH), IDX (INSN_MULH), FULL (mulh) FAST (mulh) },
  { TYPE (INSN_MULUH), IDX (INSN_MULUH), FULL (muluh) FAST (muluh) },
  { TYPE (INSN_DIV0S), IDX (INSN_DIV0S), FULL (div0s) FAST (div0s) },
  { TYPE (INSN_DIV0U), IDX (INSN_DIV0U), FULL (div0u) FAST (div0u) },
  { TYPE (INSN_DIV1), IDX (INSN_DIV1), FULL (div1) FAST (div1) },
  { TYPE (INSN_DIV2), IDX (INSN_DIV2), FULL (div2) FAST (div2) },
  { TYPE (INSN_DIV3), IDX (INSN_DIV3), FULL (div3) FAST (div3) },
  { TYPE (INSN_DIV4S), IDX (INSN_DIV4S), FULL (div4s) FAST (div4s) },
  { TYPE (INSN_LSL), IDX (INSN_LSL), FULL (lsl) FAST (lsl) },
  { TYPE (INSN_LSLI), IDX (INSN_LSLI), FULL (lsli) FAST (lsli) },
  { TYPE (INSN_LSL2), IDX (INSN_LSL2), FULL (lsl2) FAST (lsl2) },
  { TYPE (INSN_LSR), IDX (INSN_LSR), FULL (lsr) FAST (lsr) },
  { TYPE (INSN_LSRI), IDX (INSN_LSRI), FULL (lsri) FAST (lsri) },
  { TYPE (INSN_LSR2), IDX (INSN_LSR2), FULL (lsr2) FAST (lsr2) },
  { TYPE (INSN_ASR), IDX (INSN_ASR), FULL (asr) FAST (asr) },
  { TYPE (INSN_ASRI), IDX (INSN_ASRI), FULL (asri) FAST (asri) },
  { TYPE (INSN_ASR2), IDX (INSN_ASR2), FULL (asr2) FAST (asr2) },
  { TYPE (INSN_LDI8), IDX (INSN_LDI8), FULL (ldi8) FAST (ldi8) },
  { TYPE (INSN_LDI20), IDX (INSN_LDI20), FULL (ldi20) FAST (ldi20) },
  { TYPE (INSN_LDI32), IDX (INSN_LDI32), FULL (ldi32) FAST (ldi32) },
  { TYPE (INSN_LD), IDX (INSN_LD), FULL (ld) FAST (ld) },
  { TYPE (INSN_LDUH), IDX (INSN_LDUH), FULL (lduh) FAST (lduh) },
  { TYPE (INSN_LDUB), IDX (INSN_LDUB), FULL (ldub) FAST (ldub) },
  { TYPE (INSN_LDR13), IDX (INSN_LDR13), FULL (ldr13) FAST (ldr13) },
  { TYPE (INSN_LDR13UH), IDX (INSN_LDR13UH), FULL (ldr13uh) FAST (ldr13uh) },
  { TYPE (INSN_LDR13UB), IDX (INSN_LDR13UB), FULL (ldr13ub) FAST (ldr13ub) },
  { TYPE (INSN_LDR14), IDX (INSN_LDR14), FULL (ldr14) FAST (ldr14) },
  { TYPE (INSN_LDR14UH), IDX (INSN_LDR14UH), FULL (ldr14uh) FAST (ldr14uh) },
  { TYPE (INSN_LDR14UB), IDX (INSN_LDR14UB), FULL (ldr14ub) FAST (ldr14ub) },
  { TYPE (INSN_LDR15), IDX (INSN_LDR15), FULL (ldr15) FAST (ldr15) },
  { TYPE (INSN_LDR15GR), IDX (INSN_LDR15GR), FULL (ldr15gr) FAST (ldr15gr) },
  { TYPE (INSN_LDR15DR), IDX (INSN_LDR15DR), FULL (ldr15dr) FAST (ldr15dr) },
  { TYPE (INSN_LDR15PS), IDX (INSN_LDR15PS), FULL (ldr15ps) FAST (ldr15ps) },
  { TYPE (INSN_ST), IDX (INSN_ST), FULL (st) FAST (st) },
  { TYPE (INSN_STH), IDX (INSN_STH), FULL (sth) FAST (sth) },
  { TYPE (INSN_STB), IDX (INSN_STB), FULL (stb) FAST (stb) },
  { TYPE (INSN_STR13), IDX (INSN_STR13), FULL (str13) FAST (str13) },
  { TYPE (INSN_STR13H), IDX (INSN_STR13H), FULL (str13h) FAST (str13h) },
  { TYPE (INSN_STR13B), IDX (INSN_STR13B), FULL (str13b) FAST (str13b) },
  { TYPE (INSN_STR14), IDX (INSN_STR14), FULL (str14) FAST (str14) },
  { TYPE (INSN_STR14H), IDX (INSN_STR14H), FULL (str14h) FAST (str14h) },
  { TYPE (INSN_STR14B), IDX (INSN_STR14B), FULL (str14b) FAST (str14b) },
  { TYPE (INSN_STR15), IDX (INSN_STR15), FULL (str15) FAST (str15) },
  { TYPE (INSN_STR15GR), IDX (INSN_STR15GR), FULL (str15gr) FAST (str15gr) },
  { TYPE (INSN_STR15DR), IDX (INSN_STR15DR), FULL (str15dr) FAST (str15dr) },
  { TYPE (INSN_STR15PS), IDX (INSN_STR15PS), FULL (str15ps) FAST (str15ps) },
  { TYPE (INSN_MOV), IDX (INSN_MOV), FULL (mov) FAST (mov) },
  { TYPE (INSN_MOVDR), IDX (INSN_MOVDR), FULL (movdr) FAST (movdr) },
  { TYPE (INSN_MOVPS), IDX (INSN_MOVPS), FULL (movps) FAST (movps) },
  { TYPE (INSN_MOV2DR), IDX (INSN_MOV2DR), FULL (mov2dr) FAST (mov2dr) },
  { TYPE (INSN_MOV2PS), IDX (INSN_MOV2PS), FULL (mov2ps) FAST (mov2ps) },
  { TYPE (INSN_JMP), IDX (INSN_JMP), FULL (jmp) FAST (jmp) },
  { TYPE (INSN_JMPD), IDX (INSN_JMPD), FULL (jmpd) FAST (jmpd) },
  { TYPE (INSN_CALLR), IDX (INSN_CALLR), FULL (callr) FAST (callr) },
  { TYPE (INSN_CALLRD), IDX (INSN_CALLRD), FULL (callrd) FAST (callrd) },
  { TYPE (INSN_CALL), IDX (INSN_CALL), FULL (call) FAST (call) },
  { TYPE (INSN_CALLD), IDX (INSN_CALLD), FULL (calld) FAST (calld) },
  { TYPE (INSN_RET), IDX (INSN_RET), FULL (ret) FAST (ret) },
  { TYPE (INSN_RET_D), IDX (INSN_RET_D), FULL (ret_d) FAST (ret_d) },
  { TYPE (INSN_INT), IDX (INSN_INT), FULL (int) FAST (int) },
  { TYPE (INSN_INTE), IDX (INSN_INTE), FULL (inte) FAST (inte) },
  { TYPE (INSN_RETI), IDX (INSN_RETI), FULL (reti) FAST (reti) },
  { TYPE (INSN_BRAD), IDX (INSN_BRAD), FULL (brad) FAST (brad) },
  { TYPE (INSN_BRA), IDX (INSN_BRA), FULL (bra) FAST (bra) },
  { TYPE (INSN_BNOD), IDX (INSN_BNOD), FULL (bnod) FAST (bnod) },
  { TYPE (INSN_BNO), IDX (INSN_BNO), FULL (bno) FAST (bno) },
  { TYPE (INSN_BEQD), IDX (INSN_BEQD), FULL (beqd) FAST (beqd) },
  { TYPE (INSN_BEQ), IDX (INSN_BEQ), FULL (beq) FAST (beq) },
  { TYPE (INSN_BNED), IDX (INSN_BNED), FULL (bned) FAST (bned) },
  { TYPE (INSN_BNE), IDX (INSN_BNE), FULL (bne) FAST (bne) },
  { TYPE (INSN_BCD), IDX (INSN_BCD), FULL (bcd) FAST (bcd) },
  { TYPE (INSN_BC), IDX (INSN_BC), FULL (bc) FAST (bc) },
  { TYPE (INSN_BNCD), IDX (INSN_BNCD), FULL (bncd) FAST (bncd) },
  { TYPE (INSN_BNC), IDX (INSN_BNC), FULL (bnc) FAST (bnc) },
  { TYPE (INSN_BND), IDX (INSN_BND), FULL (bnd) FAST (bnd) },
  { TYPE (INSN_BN), IDX (INSN_BN), FULL (bn) FAST (bn) },
  { TYPE (INSN_BPD), IDX (INSN_BPD), FULL (bpd) FAST (bpd) },
  { TYPE (INSN_BP), IDX (INSN_BP), FULL (bp) FAST (bp) },
  { TYPE (INSN_BVD), IDX (INSN_BVD), FULL (bvd) FAST (bvd) },
  { TYPE (INSN_BV), IDX (INSN_BV), FULL (bv) FAST (bv) },
  { TYPE (INSN_BNVD), IDX (INSN_BNVD), FULL (bnvd) FAST (bnvd) },
  { TYPE (INSN_BNV), IDX (INSN_BNV), FULL (bnv) FAST (bnv) },
  { TYPE (INSN_BLTD), IDX (INSN_BLTD), FULL (bltd) FAST (bltd) },
  { TYPE (INSN_BLT), IDX (INSN_BLT), FULL (blt) FAST (blt) },
  { TYPE (INSN_BGED), IDX (INSN_BGED), FULL (bged) FAST (bged) },
  { TYPE (INSN_BGE), IDX (INSN_BGE), FULL (bge) FAST (bge) },
  { TYPE (INSN_BLED), IDX (INSN_BLED), FULL (bled) FAST (bled) },
  { TYPE (INSN_BLE), IDX (INSN_BLE), FULL (ble) FAST (ble) },
  { TYPE (INSN_BGTD), IDX (INSN_BGTD), FULL (bgtd) FAST (bgtd) },
  { TYPE (INSN_BGT), IDX (INSN_BGT), FULL (bgt) FAST (bgt) },
  { TYPE (INSN_BLSD), IDX (INSN_BLSD), FULL (blsd) FAST (blsd) },
  { TYPE (INSN_BLS), IDX (INSN_BLS), FULL (bls) FAST (bls) },
  { TYPE (INSN_BHID), IDX (INSN_BHID), FULL (bhid) FAST (bhid) },
  { TYPE (INSN_BHI), IDX (INSN_BHI), FULL (bhi) FAST (bhi) },
  { TYPE (INSN_DMOVR13), IDX (INSN_DMOVR13), FULL (dmovr13) FAST (dmovr13) },
  { TYPE (INSN_DMOVR13H), IDX (INSN_DMOVR13H), FULL (dmovr13h) FAST (dmovr13h) },
  { TYPE (INSN_DMOVR13B), IDX (INSN_DMOVR13B), FULL (dmovr13b) FAST (dmovr13b) },
  { TYPE (INSN_DMOVR13PI), IDX (INSN_DMOVR13PI), FULL (dmovr13pi) FAST (dmovr13pi) },
  { TYPE (INSN_DMOVR13PIH), IDX (INSN_DMOVR13PIH), FULL (dmovr13pih) FAST (dmovr13pih) },
  { TYPE (INSN_DMOVR13PIB), IDX (INSN_DMOVR13PIB), FULL (dmovr13pib) FAST (dmovr13pib) },
  { TYPE (INSN_DMOVR15PI), IDX (INSN_DMOVR15PI), FULL (dmovr15pi) FAST (dmovr15pi) },
  { TYPE (INSN_DMOV2R13), IDX (INSN_DMOV2R13), FULL (dmov2r13) FAST (dmov2r13) },
  { TYPE (INSN_DMOV2R13H), IDX (INSN_DMOV2R13H), FULL (dmov2r13h) FAST (dmov2r13h) },
  { TYPE (INSN_DMOV2R13B), IDX (INSN_DMOV2R13B), FULL (dmov2r13b) FAST (dmov2r13b) },
  { TYPE (INSN_DMOV2R13PI), IDX (INSN_DMOV2R13PI), FULL (dmov2r13pi) FAST (dmov2r13pi) },
  { TYPE (INSN_DMOV2R13PIH), IDX (INSN_DMOV2R13PIH), FULL (dmov2r13pih) FAST (dmov2r13pih) },
  { TYPE (INSN_DMOV2R13PIB), IDX (INSN_DMOV2R13PIB), FULL (dmov2r13pib) FAST (dmov2r13pib) },
  { TYPE (INSN_DMOV2R15PD), IDX (INSN_DMOV2R15PD), FULL (dmov2r15pd) FAST (dmov2r15pd) },
  { TYPE (INSN_LDRES), IDX (INSN_LDRES), FULL (ldres) FAST (ldres) },
  { TYPE (INSN_STRES), IDX (INSN_STRES), FULL (stres) FAST (stres) },
  { TYPE (INSN_COPOP), IDX (INSN_COPOP), FULL (copop) FAST (copop) },
  { TYPE (INSN_COPLD), IDX (INSN_COPLD), FULL (copld) FAST (copld) },
  { TYPE (INSN_COPST), IDX (INSN_COPST), FULL (copst) FAST (copst) },
  { TYPE (INSN_COPSV), IDX (INSN_COPSV), FULL (copsv) FAST (copsv) },
  { TYPE (INSN_NOP), IDX (INSN_NOP), FULL (nop) FAST (nop) },
  { TYPE (INSN_ANDCCR), IDX (INSN_ANDCCR), FULL (andccr) FAST (andccr) },
  { TYPE (INSN_ORCCR), IDX (INSN_ORCCR), FULL (orccr) FAST (orccr) },
  { TYPE (INSN_STILM), IDX (INSN_STILM), FULL (stilm) FAST (stilm) },
  { TYPE (INSN_ADDSP), IDX (INSN_ADDSP), FULL (addsp) FAST (addsp) },
  { TYPE (INSN_EXTSB), IDX (INSN_EXTSB), FULL (extsb) FAST (extsb) },
  { TYPE (INSN_EXTUB), IDX (INSN_EXTUB), FULL (extub) FAST (extub) },
  { TYPE (INSN_EXTSH), IDX (INSN_EXTSH), FULL (extsh) FAST (extsh) },
  { TYPE (INSN_EXTUH), IDX (INSN_EXTUH), FULL (extuh) FAST (extuh) },
  { TYPE (INSN_LDM0), IDX (INSN_LDM0), FULL (ldm0) FAST (ldm0) },
  { TYPE (INSN_LDM1), IDX (INSN_LDM1), FULL (ldm1) FAST (ldm1) },
  { TYPE (INSN_STM0), IDX (INSN_STM0), FULL (stm0) FAST (stm0) },
  { TYPE (INSN_STM1), IDX (INSN_STM1), FULL (stm1) FAST (stm1) },
  { TYPE (INSN_ENTER), IDX (INSN_ENTER), FULL (enter) FAST (enter) },
  { TYPE (INSN_LEAVE), IDX (INSN_LEAVE), FULL (leave) FAST (leave) },
  { TYPE (INSN_XCHB), IDX (INSN_XCHB), FULL (xchb) FAST (xchb) },
};

static const struct insn_sem fr30bf_insn_sem_invalid =
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
fr30bf_init_idesc_table (SIM_CPU *cpu)
{
  IDESC *id,*tabend;
  const struct insn_sem *t,*tend;
  int tabsize = FR30BF_INSN_MAX;
  IDESC *table = fr30bf_insn_data;

  memset (table, 0, tabsize * sizeof (IDESC));

  /* First set all entries to the `invalid insn'.  */
  t = & fr30bf_insn_sem_invalid;
  for (id = table, tabend = table + tabsize; id < tabend; ++id)
    init_idesc (cpu, id, t);

  /* Now fill in the values for the chosen cpu.  */
  for (t = fr30bf_insn_sem, tend = t + sizeof (fr30bf_insn_sem) / sizeof (*t);
       t != tend; ++t)
    {
      init_idesc (cpu, & table[t->index], t);
    }

  /* Link the IDESC table into the cpu.  */
  CPU_IDESC (cpu) = table;
}

/* Given an instruction, return a pointer to its IDESC entry.  */

const IDESC *
fr30bf_decode (SIM_CPU *current_cpu, IADDR pc,
              CGEN_INSN_INT base_insn,
              ARGBUF *abuf)
{
  /* Result of decoder.  */
  FR30BF_INSN_TYPE itype;

  {
    CGEN_INSN_INT insn = base_insn;

    {
      unsigned int val = (((insn >> 8) & (255 << 0)));
      switch (val)
      {
      case 0 : itype = FR30BF_INSN_LDR13; goto extract_fmt_ldr13;
      case 1 : itype = FR30BF_INSN_LDR13UH; goto extract_fmt_ldr13uh;
      case 2 : itype = FR30BF_INSN_LDR13UB; goto extract_fmt_ldr13ub;
      case 3 : itype = FR30BF_INSN_LDR15; goto extract_fmt_ldr15;
      case 4 : itype = FR30BF_INSN_LD; goto extract_fmt_ld;
      case 5 : itype = FR30BF_INSN_LDUH; goto extract_fmt_lduh;
      case 6 : itype = FR30BF_INSN_LDUB; goto extract_fmt_ldub;
      case 7 :
        {
          unsigned int val = (((insn >> 4) & (15 << 0)));
          switch (val)
          {
          case 0 : itype = FR30BF_INSN_LDR15GR; goto extract_fmt_ldr15gr;
          case 1 : itype = FR30BF_INSN_MOV2PS; goto extract_fmt_mov2ps;
          case 8 : itype = FR30BF_INSN_LDR15DR; goto extract_fmt_ldr15dr;
          case 9 : itype = FR30BF_INSN_LDR15PS; goto extract_fmt_ldr15ps;
          default : itype = FR30BF_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 8 : itype = FR30BF_INSN_DMOV2R13; goto extract_fmt_dmov2r13;
      case 9 : itype = FR30BF_INSN_DMOV2R13H; goto extract_fmt_dmov2r13h;
      case 10 : itype = FR30BF_INSN_DMOV2R13B; goto extract_fmt_dmov2r13b;
      case 11 : itype = FR30BF_INSN_DMOV2R15PD; goto extract_fmt_dmov2r15pd;
      case 12 : itype = FR30BF_INSN_DMOV2R13PI; goto extract_fmt_dmov2r13pi;
      case 13 : itype = FR30BF_INSN_DMOV2R13PIH; goto extract_fmt_dmov2r13pih;
      case 14 : itype = FR30BF_INSN_DMOV2R13PIB; goto extract_fmt_dmov2r13pib;
      case 15 : itype = FR30BF_INSN_ENTER; goto extract_fmt_enter;
      case 16 : itype = FR30BF_INSN_STR13; goto extract_fmt_str13;
      case 17 : itype = FR30BF_INSN_STR13H; goto extract_fmt_str13h;
      case 18 : itype = FR30BF_INSN_STR13B; goto extract_fmt_str13b;
      case 19 : itype = FR30BF_INSN_STR15; goto extract_fmt_str15;
      case 20 : itype = FR30BF_INSN_ST; goto extract_fmt_st;
      case 21 : itype = FR30BF_INSN_STH; goto extract_fmt_sth;
      case 22 : itype = FR30BF_INSN_STB; goto extract_fmt_stb;
      case 23 :
        {
          unsigned int val = (((insn >> 4) & (15 << 0)));
          switch (val)
          {
          case 0 : itype = FR30BF_INSN_STR15GR; goto extract_fmt_str15gr;
          case 1 : itype = FR30BF_INSN_MOVPS; goto extract_fmt_movps;
          case 8 : itype = FR30BF_INSN_STR15DR; goto extract_fmt_str15dr;
          case 9 : itype = FR30BF_INSN_STR15PS; goto extract_fmt_str15ps;
          default : itype = FR30BF_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 24 : itype = FR30BF_INSN_DMOVR13; goto extract_fmt_dmovr13;
      case 25 : itype = FR30BF_INSN_DMOVR13H; goto extract_fmt_dmovr13h;
      case 26 : itype = FR30BF_INSN_DMOVR13B; goto extract_fmt_dmovr13b;
      case 27 : itype = FR30BF_INSN_DMOVR15PI; goto extract_fmt_dmovr15pi;
      case 28 : itype = FR30BF_INSN_DMOVR13PI; goto extract_fmt_dmovr13pi;
      case 29 : itype = FR30BF_INSN_DMOVR13PIH; goto extract_fmt_dmovr13pih;
      case 30 : itype = FR30BF_INSN_DMOVR13PIB; goto extract_fmt_dmovr13pib;
      case 31 : itype = FR30BF_INSN_INT; goto extract_fmt_int;
      case 32 : /* fall through */
      case 33 : /* fall through */
      case 34 : /* fall through */
      case 35 : /* fall through */
      case 36 : /* fall through */
      case 37 : /* fall through */
      case 38 : /* fall through */
      case 39 : /* fall through */
      case 40 : /* fall through */
      case 41 : /* fall through */
      case 42 : /* fall through */
      case 43 : /* fall through */
      case 44 : /* fall through */
      case 45 : /* fall through */
      case 46 : /* fall through */
      case 47 : itype = FR30BF_INSN_LDR14; goto extract_fmt_ldr14;
      case 48 : /* fall through */
      case 49 : /* fall through */
      case 50 : /* fall through */
      case 51 : /* fall through */
      case 52 : /* fall through */
      case 53 : /* fall through */
      case 54 : /* fall through */
      case 55 : /* fall through */
      case 56 : /* fall through */
      case 57 : /* fall through */
      case 58 : /* fall through */
      case 59 : /* fall through */
      case 60 : /* fall through */
      case 61 : /* fall through */
      case 62 : /* fall through */
      case 63 : itype = FR30BF_INSN_STR14; goto extract_fmt_str14;
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
      case 79 : itype = FR30BF_INSN_LDR14UH; goto extract_fmt_ldr14uh;
      case 80 : /* fall through */
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
      case 95 : itype = FR30BF_INSN_STR14H; goto extract_fmt_str14h;
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
      case 111 : itype = FR30BF_INSN_LDR14UB; goto extract_fmt_ldr14ub;
      case 112 : /* fall through */
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
      case 127 : itype = FR30BF_INSN_STR14B; goto extract_fmt_str14b;
      case 128 : itype = FR30BF_INSN_BANDL; goto extract_fmt_bandl;
      case 129 : itype = FR30BF_INSN_BANDH; goto extract_fmt_bandl;
      case 130 : itype = FR30BF_INSN_AND; goto extract_fmt_and;
      case 131 : itype = FR30BF_INSN_ANDCCR; goto extract_fmt_andccr;
      case 132 : itype = FR30BF_INSN_ANDM; goto extract_fmt_andm;
      case 133 : itype = FR30BF_INSN_ANDH; goto extract_fmt_andh;
      case 134 : itype = FR30BF_INSN_ANDB; goto extract_fmt_andb;
      case 135 : itype = FR30BF_INSN_STILM; goto extract_fmt_stilm;
      case 136 : itype = FR30BF_INSN_BTSTL; goto extract_fmt_btstl;
      case 137 : itype = FR30BF_INSN_BTSTH; goto extract_fmt_btstl;
      case 138 : itype = FR30BF_INSN_XCHB; goto extract_fmt_xchb;
      case 139 : itype = FR30BF_INSN_MOV; goto extract_fmt_mov;
      case 140 : itype = FR30BF_INSN_LDM0; goto extract_fmt_ldm0;
      case 141 : itype = FR30BF_INSN_LDM1; goto extract_fmt_ldm1;
      case 142 : itype = FR30BF_INSN_STM0; goto extract_fmt_stm0;
      case 143 : itype = FR30BF_INSN_STM1; goto extract_fmt_stm1;
      case 144 : itype = FR30BF_INSN_BORL; goto extract_fmt_bandl;
      case 145 : itype = FR30BF_INSN_BORH; goto extract_fmt_bandl;
      case 146 : itype = FR30BF_INSN_OR; goto extract_fmt_and;
      case 147 : itype = FR30BF_INSN_ORCCR; goto extract_fmt_andccr;
      case 148 : itype = FR30BF_INSN_ORM; goto extract_fmt_andm;
      case 149 : itype = FR30BF_INSN_ORH; goto extract_fmt_andh;
      case 150 : itype = FR30BF_INSN_ORB; goto extract_fmt_andb;
      case 151 :
        {
          unsigned int val = (((insn >> 4) & (15 << 0)));
          switch (val)
          {
          case 0 : itype = FR30BF_INSN_JMP; goto extract_fmt_jmp;
          case 1 : itype = FR30BF_INSN_CALLR; goto extract_fmt_callr;
          case 2 : itype = FR30BF_INSN_RET; goto extract_fmt_ret;
          case 3 : itype = FR30BF_INSN_RETI; goto extract_fmt_reti;
          case 4 : itype = FR30BF_INSN_DIV0S; goto extract_fmt_div0s;
          case 5 : itype = FR30BF_INSN_DIV0U; goto extract_fmt_div0u;
          case 6 : itype = FR30BF_INSN_DIV1; goto extract_fmt_div1;
          case 7 : itype = FR30BF_INSN_DIV2; goto extract_fmt_div2;
          case 8 : itype = FR30BF_INSN_EXTSB; goto extract_fmt_extsb;
          case 9 : itype = FR30BF_INSN_EXTUB; goto extract_fmt_extub;
          case 10 : itype = FR30BF_INSN_EXTSH; goto extract_fmt_extsh;
          case 11 : itype = FR30BF_INSN_EXTUH; goto extract_fmt_extuh;
          default : itype = FR30BF_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 152 : itype = FR30BF_INSN_BEORL; goto extract_fmt_bandl;
      case 153 : itype = FR30BF_INSN_BEORH; goto extract_fmt_bandl;
      case 154 : itype = FR30BF_INSN_EOR; goto extract_fmt_and;
      case 155 : itype = FR30BF_INSN_LDI20; goto extract_fmt_ldi20;
      case 156 : itype = FR30BF_INSN_EORM; goto extract_fmt_andm;
      case 157 : itype = FR30BF_INSN_EORH; goto extract_fmt_andh;
      case 158 : itype = FR30BF_INSN_EORB; goto extract_fmt_andb;
      case 159 :
        {
          unsigned int val = (((insn >> 4) & (15 << 0)));
          switch (val)
          {
          case 0 : itype = FR30BF_INSN_JMPD; goto extract_fmt_jmp;
          case 1 : itype = FR30BF_INSN_CALLRD; goto extract_fmt_callr;
          case 2 : itype = FR30BF_INSN_RET_D; goto extract_fmt_ret;
          case 3 : itype = FR30BF_INSN_INTE; goto extract_fmt_inte;
          case 6 : itype = FR30BF_INSN_DIV3; goto extract_fmt_div3;
          case 7 : itype = FR30BF_INSN_DIV4S; goto extract_fmt_div4s;
          case 8 : itype = FR30BF_INSN_LDI32; goto extract_fmt_ldi32;
          case 9 : itype = FR30BF_INSN_LEAVE; goto extract_fmt_leave;
          case 10 : itype = FR30BF_INSN_NOP; goto extract_fmt_nop;
          case 12 : itype = FR30BF_INSN_COPOP; goto extract_fmt_copop;
          case 13 : itype = FR30BF_INSN_COPLD; goto extract_fmt_copld;
          case 14 : itype = FR30BF_INSN_COPST; goto extract_fmt_copst;
          case 15 : itype = FR30BF_INSN_COPSV; goto extract_fmt_copst;
          default : itype = FR30BF_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 160 : itype = FR30BF_INSN_ADDNI; goto extract_fmt_addni;
      case 161 : itype = FR30BF_INSN_ADDN2; goto extract_fmt_addn2;
      case 162 : itype = FR30BF_INSN_ADDN; goto extract_fmt_addn;
      case 163 : itype = FR30BF_INSN_ADDSP; goto extract_fmt_addsp;
      case 164 : itype = FR30BF_INSN_ADDI; goto extract_fmt_addi;
      case 165 : itype = FR30BF_INSN_ADD2; goto extract_fmt_add2;
      case 166 : itype = FR30BF_INSN_ADD; goto extract_fmt_add;
      case 167 : itype = FR30BF_INSN_ADDC; goto extract_fmt_addc;
      case 168 : itype = FR30BF_INSN_CMPI; goto extract_fmt_cmpi;
      case 169 : itype = FR30BF_INSN_CMP2; goto extract_fmt_cmp2;
      case 170 : itype = FR30BF_INSN_CMP; goto extract_fmt_cmp;
      case 171 : itype = FR30BF_INSN_MULU; goto extract_fmt_mulu;
      case 172 : itype = FR30BF_INSN_SUB; goto extract_fmt_add;
      case 173 : itype = FR30BF_INSN_SUBC; goto extract_fmt_addc;
      case 174 : itype = FR30BF_INSN_SUBN; goto extract_fmt_addn;
      case 175 : itype = FR30BF_INSN_MUL; goto extract_fmt_mul;
      case 176 : itype = FR30BF_INSN_LSRI; goto extract_fmt_lsli;
      case 177 : itype = FR30BF_INSN_LSR2; goto extract_fmt_lsli;
      case 178 : itype = FR30BF_INSN_LSR; goto extract_fmt_lsl;
      case 179 : itype = FR30BF_INSN_MOV2DR; goto extract_fmt_mov2dr;
      case 180 : itype = FR30BF_INSN_LSLI; goto extract_fmt_lsli;
      case 181 : itype = FR30BF_INSN_LSL2; goto extract_fmt_lsli;
      case 182 : itype = FR30BF_INSN_LSL; goto extract_fmt_lsl;
      case 183 : itype = FR30BF_INSN_MOVDR; goto extract_fmt_movdr;
      case 184 : itype = FR30BF_INSN_ASRI; goto extract_fmt_lsli;
      case 185 : itype = FR30BF_INSN_ASR2; goto extract_fmt_lsli;
      case 186 : itype = FR30BF_INSN_ASR; goto extract_fmt_lsl;
      case 187 : itype = FR30BF_INSN_MULUH; goto extract_fmt_mulh;
      case 188 : itype = FR30BF_INSN_LDRES; goto extract_fmt_ldres;
      case 189 : itype = FR30BF_INSN_STRES; goto extract_fmt_ldres;
      case 191 : itype = FR30BF_INSN_MULH; goto extract_fmt_mulh;
      case 192 : /* fall through */
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
      case 207 : itype = FR30BF_INSN_LDI8; goto extract_fmt_ldi8;
      case 208 : /* fall through */
      case 209 : /* fall through */
      case 210 : /* fall through */
      case 211 : /* fall through */
      case 212 : /* fall through */
      case 213 : /* fall through */
      case 214 : /* fall through */
      case 215 : itype = FR30BF_INSN_CALL; goto extract_fmt_call;
      case 216 : /* fall through */
      case 217 : /* fall through */
      case 218 : /* fall through */
      case 219 : /* fall through */
      case 220 : /* fall through */
      case 221 : /* fall through */
      case 222 : /* fall through */
      case 223 : itype = FR30BF_INSN_CALLD; goto extract_fmt_call;
      case 224 : itype = FR30BF_INSN_BRA; goto extract_fmt_brad;
      case 225 : itype = FR30BF_INSN_BNO; goto extract_fmt_bnod;
      case 226 : itype = FR30BF_INSN_BEQ; goto extract_fmt_beqd;
      case 227 : itype = FR30BF_INSN_BNE; goto extract_fmt_beqd;
      case 228 : itype = FR30BF_INSN_BC; goto extract_fmt_bcd;
      case 229 : itype = FR30BF_INSN_BNC; goto extract_fmt_bcd;
      case 230 : itype = FR30BF_INSN_BN; goto extract_fmt_bnd;
      case 231 : itype = FR30BF_INSN_BP; goto extract_fmt_bnd;
      case 232 : itype = FR30BF_INSN_BV; goto extract_fmt_bvd;
      case 233 : itype = FR30BF_INSN_BNV; goto extract_fmt_bvd;
      case 234 : itype = FR30BF_INSN_BLT; goto extract_fmt_bltd;
      case 235 : itype = FR30BF_INSN_BGE; goto extract_fmt_bltd;
      case 236 : itype = FR30BF_INSN_BLE; goto extract_fmt_bled;
      case 237 : itype = FR30BF_INSN_BGT; goto extract_fmt_bled;
      case 238 : itype = FR30BF_INSN_BLS; goto extract_fmt_blsd;
      case 239 : itype = FR30BF_INSN_BHI; goto extract_fmt_blsd;
      case 240 : itype = FR30BF_INSN_BRAD; goto extract_fmt_brad;
      case 241 : itype = FR30BF_INSN_BNOD; goto extract_fmt_bnod;
      case 242 : itype = FR30BF_INSN_BEQD; goto extract_fmt_beqd;
      case 243 : itype = FR30BF_INSN_BNED; goto extract_fmt_beqd;
      case 244 : itype = FR30BF_INSN_BCD; goto extract_fmt_bcd;
      case 245 : itype = FR30BF_INSN_BNCD; goto extract_fmt_bcd;
      case 246 : itype = FR30BF_INSN_BND; goto extract_fmt_bnd;
      case 247 : itype = FR30BF_INSN_BPD; goto extract_fmt_bnd;
      case 248 : itype = FR30BF_INSN_BVD; goto extract_fmt_bvd;
      case 249 : itype = FR30BF_INSN_BNVD; goto extract_fmt_bvd;
      case 250 : itype = FR30BF_INSN_BLTD; goto extract_fmt_bltd;
      case 251 : itype = FR30BF_INSN_BGED; goto extract_fmt_bltd;
      case 252 : itype = FR30BF_INSN_BLED; goto extract_fmt_bled;
      case 253 : itype = FR30BF_INSN_BGTD; goto extract_fmt_bled;
      case 254 : itype = FR30BF_INSN_BLSD; goto extract_fmt_blsd;
      case 255 : itype = FR30BF_INSN_BHID; goto extract_fmt_blsd;
      default : itype = FR30BF_INSN_X_INVALID; goto extract_fmt_empty;
      }
    }
  }

  /* The instruction has been decoded, now extract the fields.  */

 extract_fmt_empty:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
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
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_add.f
  EXTRACT_IFMT_ADD_VARS /* f-op1 f-op2 f-Rj f-Ri */

  EXTRACT_IFMT_ADD_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  FLD (i_Rj) = & CPU (h_gr)[f_Rj];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_add", "Ri 0x%x", 'x', f_Ri, "Rj 0x%x", 'x', f_Rj, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Ri) = f_Ri;
      FLD (in_Rj) = f_Rj;
      FLD (out_Ri) = f_Ri;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_addi:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_addi.f
  EXTRACT_IFMT_ADDI_VARS /* f-op1 f-op2 f-u4 f-Ri */

  EXTRACT_IFMT_ADDI_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_u4) = f_u4;
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_addi", "f_u4 0x%x", 'x', f_u4, "Ri 0x%x", 'x', f_Ri, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Ri) = f_Ri;
      FLD (out_Ri) = f_Ri;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_add2:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_add2.f
  EXTRACT_IFMT_ADD2_VARS /* f-op1 f-op2 f-m4 f-Ri */

  EXTRACT_IFMT_ADD2_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_m4) = f_m4;
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_add2", "f_m4 0x%x", 'x', f_m4, "Ri 0x%x", 'x', f_Ri, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Ri) = f_Ri;
      FLD (out_Ri) = f_Ri;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_addc:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_addc.f
  EXTRACT_IFMT_ADD_VARS /* f-op1 f-op2 f-Rj f-Ri */

  EXTRACT_IFMT_ADD_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  FLD (i_Rj) = & CPU (h_gr)[f_Rj];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_addc", "Ri 0x%x", 'x', f_Ri, "Rj 0x%x", 'x', f_Rj, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Ri) = f_Ri;
      FLD (in_Rj) = f_Rj;
      FLD (out_Ri) = f_Ri;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_addn:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_addn.f
  EXTRACT_IFMT_ADD_VARS /* f-op1 f-op2 f-Rj f-Ri */

  EXTRACT_IFMT_ADD_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  FLD (i_Rj) = & CPU (h_gr)[f_Rj];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_addn", "Ri 0x%x", 'x', f_Ri, "Rj 0x%x", 'x', f_Rj, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Ri) = f_Ri;
      FLD (in_Rj) = f_Rj;
      FLD (out_Ri) = f_Ri;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_addni:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_addni.f
  EXTRACT_IFMT_ADDI_VARS /* f-op1 f-op2 f-u4 f-Ri */

  EXTRACT_IFMT_ADDI_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_u4) = f_u4;
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_addni", "f_u4 0x%x", 'x', f_u4, "Ri 0x%x", 'x', f_Ri, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Ri) = f_Ri;
      FLD (out_Ri) = f_Ri;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_addn2:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_addn2.f
  EXTRACT_IFMT_ADD2_VARS /* f-op1 f-op2 f-m4 f-Ri */

  EXTRACT_IFMT_ADD2_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_m4) = f_m4;
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_addn2", "f_m4 0x%x", 'x', f_m4, "Ri 0x%x", 'x', f_Ri, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Ri) = f_Ri;
      FLD (out_Ri) = f_Ri;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_cmp:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_cmp.f
  EXTRACT_IFMT_ADD_VARS /* f-op1 f-op2 f-Rj f-Ri */

  EXTRACT_IFMT_ADD_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  FLD (i_Rj) = & CPU (h_gr)[f_Rj];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_cmp", "Ri 0x%x", 'x', f_Ri, "Rj 0x%x", 'x', f_Rj, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Ri) = f_Ri;
      FLD (in_Rj) = f_Rj;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_cmpi:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_cmpi.f
  EXTRACT_IFMT_ADDI_VARS /* f-op1 f-op2 f-u4 f-Ri */

  EXTRACT_IFMT_ADDI_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_u4) = f_u4;
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_cmpi", "f_u4 0x%x", 'x', f_u4, "Ri 0x%x", 'x', f_Ri, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Ri) = f_Ri;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_cmp2:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_cmp2.f
  EXTRACT_IFMT_ADD2_VARS /* f-op1 f-op2 f-m4 f-Ri */

  EXTRACT_IFMT_ADD2_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_m4) = f_m4;
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_cmp2", "f_m4 0x%x", 'x', f_m4, "Ri 0x%x", 'x', f_Ri, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Ri) = f_Ri;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_and:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_and.f
  EXTRACT_IFMT_ADD_VARS /* f-op1 f-op2 f-Rj f-Ri */

  EXTRACT_IFMT_ADD_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  FLD (i_Rj) = & CPU (h_gr)[f_Rj];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_and", "Ri 0x%x", 'x', f_Ri, "Rj 0x%x", 'x', f_Rj, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Ri) = f_Ri;
      FLD (in_Rj) = f_Rj;
      FLD (out_Ri) = f_Ri;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_andm:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_andm.f
  EXTRACT_IFMT_ADD_VARS /* f-op1 f-op2 f-Rj f-Ri */

  EXTRACT_IFMT_ADD_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  FLD (i_Rj) = & CPU (h_gr)[f_Rj];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_andm", "Ri 0x%x", 'x', f_Ri, "Rj 0x%x", 'x', f_Rj, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Ri) = f_Ri;
      FLD (in_Rj) = f_Rj;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_andh:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_andh.f
  EXTRACT_IFMT_ADD_VARS /* f-op1 f-op2 f-Rj f-Ri */

  EXTRACT_IFMT_ADD_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  FLD (i_Rj) = & CPU (h_gr)[f_Rj];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_andh", "Ri 0x%x", 'x', f_Ri, "Rj 0x%x", 'x', f_Rj, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Ri) = f_Ri;
      FLD (in_Rj) = f_Rj;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_andb:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_andb.f
  EXTRACT_IFMT_ADD_VARS /* f-op1 f-op2 f-Rj f-Ri */

  EXTRACT_IFMT_ADD_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  FLD (i_Rj) = & CPU (h_gr)[f_Rj];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_andb", "Ri 0x%x", 'x', f_Ri, "Rj 0x%x", 'x', f_Rj, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Ri) = f_Ri;
      FLD (in_Rj) = f_Rj;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_bandl:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_bandl.f
  EXTRACT_IFMT_ADDI_VARS /* f-op1 f-op2 f-u4 f-Ri */

  EXTRACT_IFMT_ADDI_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_u4) = f_u4;
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_bandl", "f_u4 0x%x", 'x', f_u4, "Ri 0x%x", 'x', f_Ri, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Ri) = f_Ri;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_btstl:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_btstl.f
  EXTRACT_IFMT_ADDI_VARS /* f-op1 f-op2 f-u4 f-Ri */

  EXTRACT_IFMT_ADDI_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_u4) = f_u4;
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_btstl", "f_u4 0x%x", 'x', f_u4, "Ri 0x%x", 'x', f_Ri, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Ri) = f_Ri;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_mul:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_mul.f
  EXTRACT_IFMT_ADD_VARS /* f-op1 f-op2 f-Rj f-Ri */

  EXTRACT_IFMT_ADD_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  FLD (i_Rj) = & CPU (h_gr)[f_Rj];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_mul", "Ri 0x%x", 'x', f_Ri, "Rj 0x%x", 'x', f_Rj, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Ri) = f_Ri;
      FLD (in_Rj) = f_Rj;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_mulu:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_mulu.f
  EXTRACT_IFMT_ADD_VARS /* f-op1 f-op2 f-Rj f-Ri */

  EXTRACT_IFMT_ADD_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  FLD (i_Rj) = & CPU (h_gr)[f_Rj];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_mulu", "Ri 0x%x", 'x', f_Ri, "Rj 0x%x", 'x', f_Rj, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Ri) = f_Ri;
      FLD (in_Rj) = f_Rj;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_mulh:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_mulh.f
  EXTRACT_IFMT_ADD_VARS /* f-op1 f-op2 f-Rj f-Ri */

  EXTRACT_IFMT_ADD_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  FLD (i_Rj) = & CPU (h_gr)[f_Rj];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_mulh", "Ri 0x%x", 'x', f_Ri, "Rj 0x%x", 'x', f_Rj, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Ri) = f_Ri;
      FLD (in_Rj) = f_Rj;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_div0s:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_div0s.f
  EXTRACT_IFMT_DIV0S_VARS /* f-op1 f-op2 f-op3 f-Ri */

  EXTRACT_IFMT_DIV0S_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_div0s", "Ri 0x%x", 'x', f_Ri, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Ri) = f_Ri;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_div0u:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_div0u.f
  EXTRACT_IFMT_DIV0S_VARS /* f-op1 f-op2 f-op3 f-Ri */

  EXTRACT_IFMT_DIV0S_CODE

  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_div0u", (char *) 0));

#undef FLD
    return idesc;
  }

 extract_fmt_div1:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_div1.f
  EXTRACT_IFMT_DIV0S_VARS /* f-op1 f-op2 f-op3 f-Ri */

  EXTRACT_IFMT_DIV0S_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_div1", "Ri 0x%x", 'x', f_Ri, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Ri) = f_Ri;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_div2:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_div2.f
  EXTRACT_IFMT_DIV0S_VARS /* f-op1 f-op2 f-op3 f-Ri */

  EXTRACT_IFMT_DIV0S_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_div2", "Ri 0x%x", 'x', f_Ri, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Ri) = f_Ri;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_div3:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_div3.f
  EXTRACT_IFMT_DIV3_VARS /* f-op1 f-op2 f-op3 f-op4 */

  EXTRACT_IFMT_DIV3_CODE

  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_div3", (char *) 0));

#undef FLD
    return idesc;
  }

 extract_fmt_div4s:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_div4s.f
  EXTRACT_IFMT_DIV3_VARS /* f-op1 f-op2 f-op3 f-op4 */

  EXTRACT_IFMT_DIV3_CODE

  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_div4s", (char *) 0));

#undef FLD
    return idesc;
  }

 extract_fmt_lsl:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_lsl.f
  EXTRACT_IFMT_ADD_VARS /* f-op1 f-op2 f-Rj f-Ri */

  EXTRACT_IFMT_ADD_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  FLD (i_Rj) = & CPU (h_gr)[f_Rj];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_lsl", "Ri 0x%x", 'x', f_Ri, "Rj 0x%x", 'x', f_Rj, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Ri) = f_Ri;
      FLD (in_Rj) = f_Rj;
      FLD (out_Ri) = f_Ri;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_lsli:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_lsli.f
  EXTRACT_IFMT_ADDI_VARS /* f-op1 f-op2 f-u4 f-Ri */

  EXTRACT_IFMT_ADDI_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_u4) = f_u4;
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_lsli", "f_u4 0x%x", 'x', f_u4, "Ri 0x%x", 'x', f_Ri, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Ri) = f_Ri;
      FLD (out_Ri) = f_Ri;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_ldi8:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldi8.f
  EXTRACT_IFMT_LDI8_VARS /* f-op1 f-i8 f-Ri */

  EXTRACT_IFMT_LDI8_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_i8) = f_i8;
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldi8", "f_i8 0x%x", 'x', f_i8, "Ri 0x%x", 'x', f_Ri, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_Ri) = f_Ri;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_ldi20:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldi20.f
  EXTRACT_IFMT_LDI20_VARS /* f-op1 f-i20 f-op2 f-Ri */

  EXTRACT_IFMT_LDI20_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_i20) = f_i20;
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldi20", "f_i20 0x%x", 'x', f_i20, "Ri 0x%x", 'x', f_Ri, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_Ri) = f_Ri;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_ldi32:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldi32.f
  EXTRACT_IFMT_LDI32_VARS /* f-op1 f-i32 f-op2 f-op3 f-Ri */

  EXTRACT_IFMT_LDI32_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_i32) = f_i32;
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldi32", "f_i32 0x%x", 'x', f_i32, "Ri 0x%x", 'x', f_Ri, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_Ri) = f_Ri;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_ld:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ld.f
  EXTRACT_IFMT_ADD_VARS /* f-op1 f-op2 f-Rj f-Ri */

  EXTRACT_IFMT_ADD_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_Rj) = & CPU (h_gr)[f_Rj];
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ld", "Rj 0x%x", 'x', f_Rj, "Ri 0x%x", 'x', f_Ri, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Rj) = f_Rj;
      FLD (out_Ri) = f_Ri;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_lduh:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_lduh.f
  EXTRACT_IFMT_ADD_VARS /* f-op1 f-op2 f-Rj f-Ri */

  EXTRACT_IFMT_ADD_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_Rj) = & CPU (h_gr)[f_Rj];
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_lduh", "Rj 0x%x", 'x', f_Rj, "Ri 0x%x", 'x', f_Ri, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Rj) = f_Rj;
      FLD (out_Ri) = f_Ri;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_ldub:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldub.f
  EXTRACT_IFMT_ADD_VARS /* f-op1 f-op2 f-Rj f-Ri */

  EXTRACT_IFMT_ADD_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_Rj) = & CPU (h_gr)[f_Rj];
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldub", "Rj 0x%x", 'x', f_Rj, "Ri 0x%x", 'x', f_Ri, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Rj) = f_Rj;
      FLD (out_Ri) = f_Ri;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_ldr13:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldr13.f
  EXTRACT_IFMT_ADD_VARS /* f-op1 f-op2 f-Rj f-Ri */

  EXTRACT_IFMT_ADD_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_Rj) = & CPU (h_gr)[f_Rj];
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldr13", "Rj 0x%x", 'x', f_Rj, "Ri 0x%x", 'x', f_Ri, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Rj) = f_Rj;
      FLD (in_h_gr_13) = 13;
      FLD (out_Ri) = f_Ri;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_ldr13uh:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldr13uh.f
  EXTRACT_IFMT_ADD_VARS /* f-op1 f-op2 f-Rj f-Ri */

  EXTRACT_IFMT_ADD_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_Rj) = & CPU (h_gr)[f_Rj];
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldr13uh", "Rj 0x%x", 'x', f_Rj, "Ri 0x%x", 'x', f_Ri, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Rj) = f_Rj;
      FLD (in_h_gr_13) = 13;
      FLD (out_Ri) = f_Ri;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_ldr13ub:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldr13ub.f
  EXTRACT_IFMT_ADD_VARS /* f-op1 f-op2 f-Rj f-Ri */

  EXTRACT_IFMT_ADD_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_Rj) = & CPU (h_gr)[f_Rj];
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldr13ub", "Rj 0x%x", 'x', f_Rj, "Ri 0x%x", 'x', f_Ri, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Rj) = f_Rj;
      FLD (in_h_gr_13) = 13;
      FLD (out_Ri) = f_Ri;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_ldr14:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldr14.f
  EXTRACT_IFMT_LDR14_VARS /* f-op1 f-disp10 f-Ri */

  EXTRACT_IFMT_LDR14_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_disp10) = f_disp10;
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldr14", "f_disp10 0x%x", 'x', f_disp10, "Ri 0x%x", 'x', f_Ri, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_h_gr_14) = 14;
      FLD (out_Ri) = f_Ri;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_ldr14uh:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldr14uh.f
  EXTRACT_IFMT_LDR14UH_VARS /* f-op1 f-disp9 f-Ri */

  EXTRACT_IFMT_LDR14UH_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_disp9) = f_disp9;
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldr14uh", "f_disp9 0x%x", 'x', f_disp9, "Ri 0x%x", 'x', f_Ri, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_h_gr_14) = 14;
      FLD (out_Ri) = f_Ri;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_ldr14ub:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldr14ub.f
  EXTRACT_IFMT_LDR14UB_VARS /* f-op1 f-disp8 f-Ri */

  EXTRACT_IFMT_LDR14UB_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_disp8) = f_disp8;
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldr14ub", "f_disp8 0x%x", 'x', f_disp8, "Ri 0x%x", 'x', f_Ri, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_h_gr_14) = 14;
      FLD (out_Ri) = f_Ri;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_ldr15:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldr15.f
  EXTRACT_IFMT_LDR15_VARS /* f-op1 f-op2 f-udisp6 f-Ri */

  EXTRACT_IFMT_LDR15_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_udisp6) = f_udisp6;
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldr15", "f_udisp6 0x%x", 'x', f_udisp6, "Ri 0x%x", 'x', f_Ri, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_h_gr_15) = 15;
      FLD (out_Ri) = f_Ri;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_ldr15gr:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldr15gr.f
  EXTRACT_IFMT_DIV0S_VARS /* f-op1 f-op2 f-op3 f-Ri */

  EXTRACT_IFMT_DIV0S_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_Ri) = f_Ri;
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldr15gr", "f_Ri 0x%x", 'x', f_Ri, "Ri 0x%x", 'x', f_Ri, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_h_gr_15) = 15;
      FLD (out_Ri) = f_Ri;
      FLD (out_h_gr_15) = 15;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_ldr15dr:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldr15dr.f
  EXTRACT_IFMT_LDR15DR_VARS /* f-op1 f-op2 f-op3 f-Rs2 */

  EXTRACT_IFMT_LDR15DR_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_Rs2) = f_Rs2;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldr15dr", "f_Rs2 0x%x", 'x', f_Rs2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_h_gr_15) = 15;
      FLD (out_h_gr_15) = 15;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_ldr15ps:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldr15ps.f
  EXTRACT_IFMT_DIV3_VARS /* f-op1 f-op2 f-op3 f-op4 */

  EXTRACT_IFMT_DIV3_CODE

  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldr15ps", (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_h_gr_15) = 15;
      FLD (out_h_gr_15) = 15;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_st:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_st.f
  EXTRACT_IFMT_ADD_VARS /* f-op1 f-op2 f-Rj f-Ri */

  EXTRACT_IFMT_ADD_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  FLD (i_Rj) = & CPU (h_gr)[f_Rj];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_st", "Ri 0x%x", 'x', f_Ri, "Rj 0x%x", 'x', f_Rj, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Ri) = f_Ri;
      FLD (in_Rj) = f_Rj;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_sth:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_sth.f
  EXTRACT_IFMT_ADD_VARS /* f-op1 f-op2 f-Rj f-Ri */

  EXTRACT_IFMT_ADD_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  FLD (i_Rj) = & CPU (h_gr)[f_Rj];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_sth", "Ri 0x%x", 'x', f_Ri, "Rj 0x%x", 'x', f_Rj, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Ri) = f_Ri;
      FLD (in_Rj) = f_Rj;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_stb:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_stb.f
  EXTRACT_IFMT_ADD_VARS /* f-op1 f-op2 f-Rj f-Ri */

  EXTRACT_IFMT_ADD_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  FLD (i_Rj) = & CPU (h_gr)[f_Rj];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stb", "Ri 0x%x", 'x', f_Ri, "Rj 0x%x", 'x', f_Rj, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Ri) = f_Ri;
      FLD (in_Rj) = f_Rj;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_str13:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_str13.f
  EXTRACT_IFMT_ADD_VARS /* f-op1 f-op2 f-Rj f-Ri */

  EXTRACT_IFMT_ADD_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  FLD (i_Rj) = & CPU (h_gr)[f_Rj];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_str13", "Ri 0x%x", 'x', f_Ri, "Rj 0x%x", 'x', f_Rj, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Ri) = f_Ri;
      FLD (in_Rj) = f_Rj;
      FLD (in_h_gr_13) = 13;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_str13h:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_str13h.f
  EXTRACT_IFMT_ADD_VARS /* f-op1 f-op2 f-Rj f-Ri */

  EXTRACT_IFMT_ADD_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  FLD (i_Rj) = & CPU (h_gr)[f_Rj];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_str13h", "Ri 0x%x", 'x', f_Ri, "Rj 0x%x", 'x', f_Rj, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Ri) = f_Ri;
      FLD (in_Rj) = f_Rj;
      FLD (in_h_gr_13) = 13;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_str13b:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_str13b.f
  EXTRACT_IFMT_ADD_VARS /* f-op1 f-op2 f-Rj f-Ri */

  EXTRACT_IFMT_ADD_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  FLD (i_Rj) = & CPU (h_gr)[f_Rj];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_str13b", "Ri 0x%x", 'x', f_Ri, "Rj 0x%x", 'x', f_Rj, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Ri) = f_Ri;
      FLD (in_Rj) = f_Rj;
      FLD (in_h_gr_13) = 13;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_str14:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_str14.f
  EXTRACT_IFMT_LDR14_VARS /* f-op1 f-disp10 f-Ri */

  EXTRACT_IFMT_LDR14_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_disp10) = f_disp10;
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_str14", "f_disp10 0x%x", 'x', f_disp10, "Ri 0x%x", 'x', f_Ri, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Ri) = f_Ri;
      FLD (in_h_gr_14) = 14;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_str14h:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_str14h.f
  EXTRACT_IFMT_LDR14UH_VARS /* f-op1 f-disp9 f-Ri */

  EXTRACT_IFMT_LDR14UH_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_disp9) = f_disp9;
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_str14h", "f_disp9 0x%x", 'x', f_disp9, "Ri 0x%x", 'x', f_Ri, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Ri) = f_Ri;
      FLD (in_h_gr_14) = 14;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_str14b:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_str14b.f
  EXTRACT_IFMT_LDR14UB_VARS /* f-op1 f-disp8 f-Ri */

  EXTRACT_IFMT_LDR14UB_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_disp8) = f_disp8;
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_str14b", "f_disp8 0x%x", 'x', f_disp8, "Ri 0x%x", 'x', f_Ri, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Ri) = f_Ri;
      FLD (in_h_gr_14) = 14;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_str15:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_str15.f
  EXTRACT_IFMT_LDR15_VARS /* f-op1 f-op2 f-udisp6 f-Ri */

  EXTRACT_IFMT_LDR15_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_udisp6) = f_udisp6;
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_str15", "f_udisp6 0x%x", 'x', f_udisp6, "Ri 0x%x", 'x', f_Ri, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Ri) = f_Ri;
      FLD (in_h_gr_15) = 15;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_str15gr:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_str15gr.f
  EXTRACT_IFMT_DIV0S_VARS /* f-op1 f-op2 f-op3 f-Ri */

  EXTRACT_IFMT_DIV0S_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_str15gr", "Ri 0x%x", 'x', f_Ri, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Ri) = f_Ri;
      FLD (in_h_gr_15) = 15;
      FLD (out_h_gr_15) = 15;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_str15dr:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_str15dr.f
  EXTRACT_IFMT_LDR15DR_VARS /* f-op1 f-op2 f-op3 f-Rs2 */

  EXTRACT_IFMT_LDR15DR_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_Rs2) = f_Rs2;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_str15dr", "f_Rs2 0x%x", 'x', f_Rs2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_h_gr_15) = 15;
      FLD (out_h_gr_15) = 15;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_str15ps:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_str15ps.f
  EXTRACT_IFMT_DIV3_VARS /* f-op1 f-op2 f-op3 f-op4 */

  EXTRACT_IFMT_DIV3_CODE

  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_str15ps", (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_h_gr_15) = 15;
      FLD (out_h_gr_15) = 15;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_mov:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_mov.f
  EXTRACT_IFMT_ADD_VARS /* f-op1 f-op2 f-Rj f-Ri */

  EXTRACT_IFMT_ADD_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_Rj) = & CPU (h_gr)[f_Rj];
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_mov", "Rj 0x%x", 'x', f_Rj, "Ri 0x%x", 'x', f_Ri, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Rj) = f_Rj;
      FLD (out_Ri) = f_Ri;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_movdr:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_movdr.f
  EXTRACT_IFMT_MOVDR_VARS /* f-op1 f-op2 f-Rs1 f-Ri */

  EXTRACT_IFMT_MOVDR_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_Rs1) = f_Rs1;
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_movdr", "f_Rs1 0x%x", 'x', f_Rs1, "Ri 0x%x", 'x', f_Ri, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_Ri) = f_Ri;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_movps:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_movps.f
  EXTRACT_IFMT_DIV0S_VARS /* f-op1 f-op2 f-op3 f-Ri */

  EXTRACT_IFMT_DIV0S_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_movps", "Ri 0x%x", 'x', f_Ri, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_Ri) = f_Ri;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_mov2dr:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_mov2dr.f
  EXTRACT_IFMT_MOVDR_VARS /* f-op1 f-op2 f-Rs1 f-Ri */

  EXTRACT_IFMT_MOVDR_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_Rs1) = f_Rs1;
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_mov2dr", "f_Rs1 0x%x", 'x', f_Rs1, "Ri 0x%x", 'x', f_Ri, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Ri) = f_Ri;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_mov2ps:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_mov2ps.f
  EXTRACT_IFMT_DIV0S_VARS /* f-op1 f-op2 f-op3 f-Ri */

  EXTRACT_IFMT_DIV0S_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_mov2ps", "Ri 0x%x", 'x', f_Ri, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Ri) = f_Ri;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_jmp:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_jmp.f
  EXTRACT_IFMT_DIV0S_VARS /* f-op1 f-op2 f-op3 f-Ri */

  EXTRACT_IFMT_DIV0S_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_jmp", "Ri 0x%x", 'x', f_Ri, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Ri) = f_Ri;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_callr:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_callr.f
  EXTRACT_IFMT_DIV0S_VARS /* f-op1 f-op2 f-op3 f-Ri */

  EXTRACT_IFMT_DIV0S_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_callr", "Ri 0x%x", 'x', f_Ri, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Ri) = f_Ri;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_call:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_call.f
  EXTRACT_IFMT_CALL_VARS /* f-op1 f-op5 f-rel12 */

  EXTRACT_IFMT_CALL_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_label12) = f_rel12;
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_call", "label12 0x%x", 'x', f_rel12, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_ret:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_ret.f
  EXTRACT_IFMT_DIV3_VARS /* f-op1 f-op2 f-op3 f-op4 */

  EXTRACT_IFMT_DIV3_CODE

  /* Record the fields for the semantic handler.  */
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ret", (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_int:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_int.f
  EXTRACT_IFMT_INT_VARS /* f-op1 f-op2 f-u8 */

  EXTRACT_IFMT_INT_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_u8) = f_u8;
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_int", "f_u8 0x%x", 'x', f_u8, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_inte:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_inte.f
  EXTRACT_IFMT_DIV3_VARS /* f-op1 f-op2 f-op3 f-op4 */

  EXTRACT_IFMT_DIV3_CODE

  /* Record the fields for the semantic handler.  */
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_inte", (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_reti:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_reti.f
  EXTRACT_IFMT_DIV3_VARS /* f-op1 f-op2 f-op3 f-op4 */

  EXTRACT_IFMT_DIV3_CODE

  /* Record the fields for the semantic handler.  */
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_reti", (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_brad:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_brad.f
  EXTRACT_IFMT_BRAD_VARS /* f-op1 f-cc f-rel9 */

  EXTRACT_IFMT_BRAD_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_label9) = f_rel9;
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_brad", "label9 0x%x", 'x', f_rel9, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_bnod:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_bnod.f
  EXTRACT_IFMT_BRAD_VARS /* f-op1 f-cc f-rel9 */

  EXTRACT_IFMT_BRAD_CODE

  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_bnod", (char *) 0));

#undef FLD
    return idesc;
  }

 extract_fmt_beqd:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_beqd.f
  EXTRACT_IFMT_BRAD_VARS /* f-op1 f-cc f-rel9 */

  EXTRACT_IFMT_BRAD_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_label9) = f_rel9;
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_beqd", "label9 0x%x", 'x', f_rel9, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_bcd:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_bcd.f
  EXTRACT_IFMT_BRAD_VARS /* f-op1 f-cc f-rel9 */

  EXTRACT_IFMT_BRAD_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_label9) = f_rel9;
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_bcd", "label9 0x%x", 'x', f_rel9, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_bnd:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_bnd.f
  EXTRACT_IFMT_BRAD_VARS /* f-op1 f-cc f-rel9 */

  EXTRACT_IFMT_BRAD_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_label9) = f_rel9;
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_bnd", "label9 0x%x", 'x', f_rel9, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_bvd:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_bvd.f
  EXTRACT_IFMT_BRAD_VARS /* f-op1 f-cc f-rel9 */

  EXTRACT_IFMT_BRAD_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_label9) = f_rel9;
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_bvd", "label9 0x%x", 'x', f_rel9, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_bltd:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_bltd.f
  EXTRACT_IFMT_BRAD_VARS /* f-op1 f-cc f-rel9 */

  EXTRACT_IFMT_BRAD_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_label9) = f_rel9;
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_bltd", "label9 0x%x", 'x', f_rel9, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_bled:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_bled.f
  EXTRACT_IFMT_BRAD_VARS /* f-op1 f-cc f-rel9 */

  EXTRACT_IFMT_BRAD_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_label9) = f_rel9;
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_bled", "label9 0x%x", 'x', f_rel9, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_blsd:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_blsd.f
  EXTRACT_IFMT_BRAD_VARS /* f-op1 f-cc f-rel9 */

  EXTRACT_IFMT_BRAD_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_label9) = f_rel9;
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_blsd", "label9 0x%x", 'x', f_rel9, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_dmovr13:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_dmovr13.f
  EXTRACT_IFMT_DMOVR13_VARS /* f-op1 f-op2 f-dir10 */

  EXTRACT_IFMT_DMOVR13_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_dir10) = f_dir10;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_dmovr13", "f_dir10 0x%x", 'x', f_dir10, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_h_gr_13) = 13;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_dmovr13h:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_dmovr13h.f
  EXTRACT_IFMT_DMOVR13H_VARS /* f-op1 f-op2 f-dir9 */

  EXTRACT_IFMT_DMOVR13H_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_dir9) = f_dir9;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_dmovr13h", "f_dir9 0x%x", 'x', f_dir9, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_h_gr_13) = 13;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_dmovr13b:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_dmovr13b.f
  EXTRACT_IFMT_DMOVR13B_VARS /* f-op1 f-op2 f-dir8 */

  EXTRACT_IFMT_DMOVR13B_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_dir8) = f_dir8;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_dmovr13b", "f_dir8 0x%x", 'x', f_dir8, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_h_gr_13) = 13;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_dmovr13pi:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_dmovr13pi.f
  EXTRACT_IFMT_DMOVR13_VARS /* f-op1 f-op2 f-dir10 */

  EXTRACT_IFMT_DMOVR13_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_dir10) = f_dir10;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_dmovr13pi", "f_dir10 0x%x", 'x', f_dir10, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_h_gr_13) = 13;
      FLD (out_h_gr_13) = 13;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_dmovr13pih:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_dmovr13pih.f
  EXTRACT_IFMT_DMOVR13H_VARS /* f-op1 f-op2 f-dir9 */

  EXTRACT_IFMT_DMOVR13H_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_dir9) = f_dir9;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_dmovr13pih", "f_dir9 0x%x", 'x', f_dir9, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_h_gr_13) = 13;
      FLD (out_h_gr_13) = 13;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_dmovr13pib:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_dmovr13pib.f
  EXTRACT_IFMT_DMOVR13B_VARS /* f-op1 f-op2 f-dir8 */

  EXTRACT_IFMT_DMOVR13B_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_dir8) = f_dir8;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_dmovr13pib", "f_dir8 0x%x", 'x', f_dir8, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_h_gr_13) = 13;
      FLD (out_h_gr_13) = 13;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_dmovr15pi:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_dmovr15pi.f
  EXTRACT_IFMT_DMOVR13_VARS /* f-op1 f-op2 f-dir10 */

  EXTRACT_IFMT_DMOVR13_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_dir10) = f_dir10;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_dmovr15pi", "f_dir10 0x%x", 'x', f_dir10, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_h_gr_15) = 15;
      FLD (out_h_gr_15) = 15;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_dmov2r13:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_dmov2r13.f
  EXTRACT_IFMT_DMOVR13_VARS /* f-op1 f-op2 f-dir10 */

  EXTRACT_IFMT_DMOVR13_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_dir10) = f_dir10;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_dmov2r13", "f_dir10 0x%x", 'x', f_dir10, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_h_gr_13) = 13;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_dmov2r13h:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_dmov2r13h.f
  EXTRACT_IFMT_DMOVR13H_VARS /* f-op1 f-op2 f-dir9 */

  EXTRACT_IFMT_DMOVR13H_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_dir9) = f_dir9;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_dmov2r13h", "f_dir9 0x%x", 'x', f_dir9, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_h_gr_13) = 13;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_dmov2r13b:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_dmov2r13b.f
  EXTRACT_IFMT_DMOVR13B_VARS /* f-op1 f-op2 f-dir8 */

  EXTRACT_IFMT_DMOVR13B_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_dir8) = f_dir8;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_dmov2r13b", "f_dir8 0x%x", 'x', f_dir8, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_h_gr_13) = 13;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_dmov2r13pi:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_dmov2r13pi.f
  EXTRACT_IFMT_DMOVR13_VARS /* f-op1 f-op2 f-dir10 */

  EXTRACT_IFMT_DMOVR13_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_dir10) = f_dir10;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_dmov2r13pi", "f_dir10 0x%x", 'x', f_dir10, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_h_gr_13) = 13;
      FLD (out_h_gr_13) = 13;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_dmov2r13pih:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_dmov2r13pih.f
  EXTRACT_IFMT_DMOVR13H_VARS /* f-op1 f-op2 f-dir9 */

  EXTRACT_IFMT_DMOVR13H_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_dir9) = f_dir9;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_dmov2r13pih", "f_dir9 0x%x", 'x', f_dir9, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_h_gr_13) = 13;
      FLD (out_h_gr_13) = 13;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_dmov2r13pib:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_dmov2r13pib.f
  EXTRACT_IFMT_DMOVR13B_VARS /* f-op1 f-op2 f-dir8 */

  EXTRACT_IFMT_DMOVR13B_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_dir8) = f_dir8;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_dmov2r13pib", "f_dir8 0x%x", 'x', f_dir8, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_h_gr_13) = 13;
      FLD (out_h_gr_13) = 13;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_dmov2r15pd:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_dmov2r15pd.f
  EXTRACT_IFMT_DMOVR13_VARS /* f-op1 f-op2 f-dir10 */

  EXTRACT_IFMT_DMOVR13_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_dir10) = f_dir10;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_dmov2r15pd", "f_dir10 0x%x", 'x', f_dir10, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_h_gr_15) = 15;
      FLD (out_h_gr_15) = 15;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_ldres:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldres.f
  EXTRACT_IFMT_ADDI_VARS /* f-op1 f-op2 f-u4 f-Ri */

  EXTRACT_IFMT_ADDI_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldres", "Ri 0x%x", 'x', f_Ri, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Ri) = f_Ri;
      FLD (out_Ri) = f_Ri;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_copop:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_copop.f
  EXTRACT_IFMT_COPOP_VARS /* f-op1 f-ccc f-op2 f-op3 f-CRj f-u4c f-CRi */

  EXTRACT_IFMT_COPOP_CODE

  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_copop", (char *) 0));

#undef FLD
    return idesc;
  }

 extract_fmt_copld:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_copld.f
  EXTRACT_IFMT_COPLD_VARS /* f-op1 f-ccc f-op2 f-op3 f-Rjc f-u4c f-CRi */

  EXTRACT_IFMT_COPLD_CODE

  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_copld", (char *) 0));

#undef FLD
    return idesc;
  }

 extract_fmt_copst:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_copst.f
  EXTRACT_IFMT_COPST_VARS /* f-op1 f-ccc f-op2 f-op3 f-CRj f-u4c f-Ric */

  EXTRACT_IFMT_COPST_CODE

  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_copst", (char *) 0));

#undef FLD
    return idesc;
  }

 extract_fmt_nop:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_nop.f
  EXTRACT_IFMT_DIV3_VARS /* f-op1 f-op2 f-op3 f-op4 */

  EXTRACT_IFMT_DIV3_CODE

  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_nop", (char *) 0));

#undef FLD
    return idesc;
  }

 extract_fmt_andccr:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_andccr.f
  EXTRACT_IFMT_INT_VARS /* f-op1 f-op2 f-u8 */

  EXTRACT_IFMT_INT_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_u8) = f_u8;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_andccr", "f_u8 0x%x", 'x', f_u8, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_fmt_stilm:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_stilm.f
  EXTRACT_IFMT_INT_VARS /* f-op1 f-op2 f-u8 */

  EXTRACT_IFMT_INT_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_u8) = f_u8;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stilm", "f_u8 0x%x", 'x', f_u8, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_fmt_addsp:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_addsp.f
  EXTRACT_IFMT_ADDSP_VARS /* f-op1 f-op2 f-s10 */

  EXTRACT_IFMT_ADDSP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_s10) = f_s10;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_addsp", "f_s10 0x%x", 'x', f_s10, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_h_gr_15) = 15;
      FLD (out_h_gr_15) = 15;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_extsb:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_extsb.f
  EXTRACT_IFMT_DIV0S_VARS /* f-op1 f-op2 f-op3 f-Ri */

  EXTRACT_IFMT_DIV0S_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_extsb", "Ri 0x%x", 'x', f_Ri, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Ri) = f_Ri;
      FLD (out_Ri) = f_Ri;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_extub:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_extub.f
  EXTRACT_IFMT_DIV0S_VARS /* f-op1 f-op2 f-op3 f-Ri */

  EXTRACT_IFMT_DIV0S_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_extub", "Ri 0x%x", 'x', f_Ri, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Ri) = f_Ri;
      FLD (out_Ri) = f_Ri;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_extsh:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_extsh.f
  EXTRACT_IFMT_DIV0S_VARS /* f-op1 f-op2 f-op3 f-Ri */

  EXTRACT_IFMT_DIV0S_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_extsh", "Ri 0x%x", 'x', f_Ri, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Ri) = f_Ri;
      FLD (out_Ri) = f_Ri;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_extuh:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_extuh.f
  EXTRACT_IFMT_DIV0S_VARS /* f-op1 f-op2 f-op3 f-Ri */

  EXTRACT_IFMT_DIV0S_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_extuh", "Ri 0x%x", 'x', f_Ri, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Ri) = f_Ri;
      FLD (out_Ri) = f_Ri;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_ldm0:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldm0.f
  EXTRACT_IFMT_LDM0_VARS /* f-op1 f-op2 f-reglist_low_ld */

  EXTRACT_IFMT_LDM0_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_reglist_low_ld) = f_reglist_low_ld;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldm0", "f_reglist_low_ld 0x%x", 'x', f_reglist_low_ld, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_h_gr_15) = 15;
      FLD (out_h_gr_0) = 0;
      FLD (out_h_gr_1) = 1;
      FLD (out_h_gr_15) = 15;
      FLD (out_h_gr_2) = 2;
      FLD (out_h_gr_3) = 3;
      FLD (out_h_gr_4) = 4;
      FLD (out_h_gr_5) = 5;
      FLD (out_h_gr_6) = 6;
      FLD (out_h_gr_7) = 7;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_ldm1:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldm1.f
  EXTRACT_IFMT_LDM1_VARS /* f-op1 f-op2 f-reglist_hi_ld */

  EXTRACT_IFMT_LDM1_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_reglist_hi_ld) = f_reglist_hi_ld;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldm1", "f_reglist_hi_ld 0x%x", 'x', f_reglist_hi_ld, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_h_gr_15) = 15;
      FLD (out_h_gr_10) = 10;
      FLD (out_h_gr_11) = 11;
      FLD (out_h_gr_12) = 12;
      FLD (out_h_gr_13) = 13;
      FLD (out_h_gr_14) = 14;
      FLD (out_h_gr_15) = 15;
      FLD (out_h_gr_8) = 8;
      FLD (out_h_gr_9) = 9;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_stm0:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_stm0.f
  EXTRACT_IFMT_STM0_VARS /* f-op1 f-op2 f-reglist_low_st */

  EXTRACT_IFMT_STM0_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_reglist_low_st) = f_reglist_low_st;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stm0", "f_reglist_low_st 0x%x", 'x', f_reglist_low_st, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_h_gr_0) = 0;
      FLD (in_h_gr_1) = 1;
      FLD (in_h_gr_15) = 15;
      FLD (in_h_gr_2) = 2;
      FLD (in_h_gr_3) = 3;
      FLD (in_h_gr_4) = 4;
      FLD (in_h_gr_5) = 5;
      FLD (in_h_gr_6) = 6;
      FLD (in_h_gr_7) = 7;
      FLD (out_h_gr_15) = 15;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_stm1:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_stm1.f
  EXTRACT_IFMT_STM1_VARS /* f-op1 f-op2 f-reglist_hi_st */

  EXTRACT_IFMT_STM1_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_reglist_hi_st) = f_reglist_hi_st;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stm1", "f_reglist_hi_st 0x%x", 'x', f_reglist_hi_st, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_h_gr_10) = 10;
      FLD (in_h_gr_11) = 11;
      FLD (in_h_gr_12) = 12;
      FLD (in_h_gr_13) = 13;
      FLD (in_h_gr_14) = 14;
      FLD (in_h_gr_15) = 15;
      FLD (in_h_gr_8) = 8;
      FLD (in_h_gr_9) = 9;
      FLD (out_h_gr_15) = 15;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_enter:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_enter.f
  EXTRACT_IFMT_ENTER_VARS /* f-op1 f-op2 f-u10 */

  EXTRACT_IFMT_ENTER_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_u10) = f_u10;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_enter", "f_u10 0x%x", 'x', f_u10, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_h_gr_14) = 14;
      FLD (in_h_gr_15) = 15;
      FLD (out_h_gr_14) = 14;
      FLD (out_h_gr_15) = 15;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_leave:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_leave.f
  EXTRACT_IFMT_DIV3_VARS /* f-op1 f-op2 f-op3 f-op4 */

  EXTRACT_IFMT_DIV3_CODE

  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_leave", (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_h_gr_14) = 14;
      FLD (in_h_gr_15) = 15;
      FLD (out_h_gr_14) = 14;
      FLD (out_h_gr_15) = 15;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_xchb:
  {
    const IDESC *idesc = &fr30bf_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_xchb.f
  EXTRACT_IFMT_ADD_VARS /* f-op1 f-op2 f-Rj f-Ri */

  EXTRACT_IFMT_ADD_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_Ri) = & CPU (h_gr)[f_Ri];
  FLD (i_Rj) = & CPU (h_gr)[f_Rj];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_xchb", "Ri 0x%x", 'x', f_Ri, "Rj 0x%x", 'x', f_Rj, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_Ri) = f_Ri;
      FLD (in_Rj) = f_Rj;
      FLD (out_Ri) = f_Ri;
    }
#endif
#undef FLD
    return idesc;
  }

}
