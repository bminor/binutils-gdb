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

/* The instruction descriptor array.
   This is computed at runtime.  Space for it is not malloc'd to save a
   teensy bit of cpu in the decoder.  Moving it to malloc space is trivial
   but won't be done until necessary (we don't currently support the runtime
   addition of instructions nor an SMP machine with different cpus).  */
static IDESC fr30bf_insn_data[FR30BF_INSN_MAX];

/* The INSN_ prefix is not here and is instead part of the `insn' argument
   to avoid collisions with header files (e.g. `AND' in ansidecl.h).  */
#define IDX(insn) CONCAT2 (FR30BF_,insn)
#define TYPE(insn) CONCAT2 (FR30_,insn)

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

/* Enum declaration for all instruction semantic formats.  */
typedef enum sfmt {
  FMT_EMPTY, FMT_ADD, FMT_ADDI, FMT_ADD2
 , FMT_ADDC, FMT_ADDN, FMT_ADDNI, FMT_ADDN2
 , FMT_CMP, FMT_CMPI, FMT_CMP2, FMT_AND
 , FMT_ANDM, FMT_ANDH, FMT_ANDB, FMT_BANDL
 , FMT_BTSTL, FMT_MUL, FMT_MULU, FMT_MULH
 , FMT_DIV0S, FMT_DIV0U, FMT_DIV1, FMT_DIV2
 , FMT_DIV3, FMT_DIV4S, FMT_LSL, FMT_LSLI
 , FMT_LDI8, FMT_LDI20, FMT_LDI32, FMT_LD
 , FMT_LDUH, FMT_LDUB, FMT_LDR13, FMT_LDR13UH
 , FMT_LDR13UB, FMT_LDR14, FMT_LDR14UH, FMT_LDR14UB
 , FMT_LDR15, FMT_LDR15GR, FMT_LDR15DR, FMT_LDR15PS
 , FMT_ST, FMT_STH, FMT_STB, FMT_STR13
 , FMT_STR13H, FMT_STR13B, FMT_STR14, FMT_STR14H
 , FMT_STR14B, FMT_STR15, FMT_STR15GR, FMT_STR15DR
 , FMT_STR15PS, FMT_MOV, FMT_MOVDR, FMT_MOVPS
 , FMT_MOV2DR, FMT_MOV2PS, FMT_JMP, FMT_CALLR
 , FMT_CALL, FMT_RET, FMT_INT, FMT_INTE
 , FMT_RETI, FMT_BRAD, FMT_BNOD, FMT_BEQD
 , FMT_BCD, FMT_BND, FMT_BVD, FMT_BLTD
 , FMT_BLED, FMT_BLSD, FMT_DMOVR13, FMT_DMOVR13H
 , FMT_DMOVR13B, FMT_DMOVR13PI, FMT_DMOVR13PIH, FMT_DMOVR13PIB
 , FMT_DMOVR15PI, FMT_DMOV2R13, FMT_DMOV2R13H, FMT_DMOV2R13B
 , FMT_DMOV2R13PI, FMT_DMOV2R13PIH, FMT_DMOV2R13PIB, FMT_DMOV2R15PD
 , FMT_LDRES, FMT_COPOP, FMT_COPLD, FMT_COPST
 , FMT_NOP, FMT_ANDCCR, FMT_STILM, FMT_ADDSP
 , FMT_EXTSB, FMT_EXTUB, FMT_EXTSH, FMT_EXTUH
 , FMT_LDM0, FMT_LDM1, FMT_STM0, FMT_STM1
 , FMT_ENTER, FMT_LEAVE, FMT_XCHB
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
fr30bf_decode (SIM_CPU *current_cpu, IADDR pc,
              CGEN_INSN_INT base_insn,
              ARGBUF *abuf)
{
  /* Result of decoder, used by extractor.  */
  const DECODE_DESC *idecode;

  /* First decode the instruction.  */

  {
#define I(insn) & fr30bf_insn_data[CONCAT2 (FR30BF_,insn)]
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
        && default_0, && default_0, && default_0, && case_0_23, 
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
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && case_0_151, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && case_0_159, 
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
      };
#endif
      static const DECODE_DESC insns[256] = {
        { I (INSN_LDR13), E (FMT_LDR13) }, { I (INSN_LDR13UH), E (FMT_LDR13UH) }, 
        { I (INSN_LDR13UB), E (FMT_LDR13UB) }, { I (INSN_LDR15), E (FMT_LDR15) }, 
        { I (INSN_LD), E (FMT_LD) }, { I (INSN_LDUH), E (FMT_LDUH) }, 
        { I (INSN_LDUB), E (FMT_LDUB) }, { 0 }, 
        { I (INSN_DMOV2R13), E (FMT_DMOV2R13) }, { I (INSN_DMOV2R13H), E (FMT_DMOV2R13H) }, 
        { I (INSN_DMOV2R13B), E (FMT_DMOV2R13B) }, { I (INSN_DMOV2R15PD), E (FMT_DMOV2R15PD) }, 
        { I (INSN_DMOV2R13PI), E (FMT_DMOV2R13PI) }, { I (INSN_DMOV2R13PIH), E (FMT_DMOV2R13PIH) }, 
        { I (INSN_DMOV2R13PIB), E (FMT_DMOV2R13PIB) }, { I (INSN_ENTER), E (FMT_ENTER) }, 
        { I (INSN_STR13), E (FMT_STR13) }, { I (INSN_STR13H), E (FMT_STR13H) }, 
        { I (INSN_STR13B), E (FMT_STR13B) }, { I (INSN_STR15), E (FMT_STR15) }, 
        { I (INSN_ST), E (FMT_ST) }, { I (INSN_STH), E (FMT_STH) }, 
        { I (INSN_STB), E (FMT_STB) }, { 0 }, 
        { I (INSN_DMOVR13), E (FMT_DMOVR13) }, { I (INSN_DMOVR13H), E (FMT_DMOVR13H) }, 
        { I (INSN_DMOVR13B), E (FMT_DMOVR13B) }, { I (INSN_DMOVR15PI), E (FMT_DMOVR15PI) }, 
        { I (INSN_DMOVR13PI), E (FMT_DMOVR13PI) }, { I (INSN_DMOVR13PIH), E (FMT_DMOVR13PIH) }, 
        { I (INSN_DMOVR13PIB), E (FMT_DMOVR13PIB) }, { I (INSN_INT), E (FMT_INT) }, 
        { I (INSN_LDR14), E (FMT_LDR14) }, { I (INSN_LDR14), E (FMT_LDR14) }, 
        { I (INSN_LDR14), E (FMT_LDR14) }, { I (INSN_LDR14), E (FMT_LDR14) }, 
        { I (INSN_LDR14), E (FMT_LDR14) }, { I (INSN_LDR14), E (FMT_LDR14) }, 
        { I (INSN_LDR14), E (FMT_LDR14) }, { I (INSN_LDR14), E (FMT_LDR14) }, 
        { I (INSN_LDR14), E (FMT_LDR14) }, { I (INSN_LDR14), E (FMT_LDR14) }, 
        { I (INSN_LDR14), E (FMT_LDR14) }, { I (INSN_LDR14), E (FMT_LDR14) }, 
        { I (INSN_LDR14), E (FMT_LDR14) }, { I (INSN_LDR14), E (FMT_LDR14) }, 
        { I (INSN_LDR14), E (FMT_LDR14) }, { I (INSN_LDR14), E (FMT_LDR14) }, 
        { I (INSN_STR14), E (FMT_STR14) }, { I (INSN_STR14), E (FMT_STR14) }, 
        { I (INSN_STR14), E (FMT_STR14) }, { I (INSN_STR14), E (FMT_STR14) }, 
        { I (INSN_STR14), E (FMT_STR14) }, { I (INSN_STR14), E (FMT_STR14) }, 
        { I (INSN_STR14), E (FMT_STR14) }, { I (INSN_STR14), E (FMT_STR14) }, 
        { I (INSN_STR14), E (FMT_STR14) }, { I (INSN_STR14), E (FMT_STR14) }, 
        { I (INSN_STR14), E (FMT_STR14) }, { I (INSN_STR14), E (FMT_STR14) }, 
        { I (INSN_STR14), E (FMT_STR14) }, { I (INSN_STR14), E (FMT_STR14) }, 
        { I (INSN_STR14), E (FMT_STR14) }, { I (INSN_STR14), E (FMT_STR14) }, 
        { I (INSN_LDR14UH), E (FMT_LDR14UH) }, { I (INSN_LDR14UH), E (FMT_LDR14UH) }, 
        { I (INSN_LDR14UH), E (FMT_LDR14UH) }, { I (INSN_LDR14UH), E (FMT_LDR14UH) }, 
        { I (INSN_LDR14UH), E (FMT_LDR14UH) }, { I (INSN_LDR14UH), E (FMT_LDR14UH) }, 
        { I (INSN_LDR14UH), E (FMT_LDR14UH) }, { I (INSN_LDR14UH), E (FMT_LDR14UH) }, 
        { I (INSN_LDR14UH), E (FMT_LDR14UH) }, { I (INSN_LDR14UH), E (FMT_LDR14UH) }, 
        { I (INSN_LDR14UH), E (FMT_LDR14UH) }, { I (INSN_LDR14UH), E (FMT_LDR14UH) }, 
        { I (INSN_LDR14UH), E (FMT_LDR14UH) }, { I (INSN_LDR14UH), E (FMT_LDR14UH) }, 
        { I (INSN_LDR14UH), E (FMT_LDR14UH) }, { I (INSN_LDR14UH), E (FMT_LDR14UH) }, 
        { I (INSN_STR14H), E (FMT_STR14H) }, { I (INSN_STR14H), E (FMT_STR14H) }, 
        { I (INSN_STR14H), E (FMT_STR14H) }, { I (INSN_STR14H), E (FMT_STR14H) }, 
        { I (INSN_STR14H), E (FMT_STR14H) }, { I (INSN_STR14H), E (FMT_STR14H) }, 
        { I (INSN_STR14H), E (FMT_STR14H) }, { I (INSN_STR14H), E (FMT_STR14H) }, 
        { I (INSN_STR14H), E (FMT_STR14H) }, { I (INSN_STR14H), E (FMT_STR14H) }, 
        { I (INSN_STR14H), E (FMT_STR14H) }, { I (INSN_STR14H), E (FMT_STR14H) }, 
        { I (INSN_STR14H), E (FMT_STR14H) }, { I (INSN_STR14H), E (FMT_STR14H) }, 
        { I (INSN_STR14H), E (FMT_STR14H) }, { I (INSN_STR14H), E (FMT_STR14H) }, 
        { I (INSN_LDR14UB), E (FMT_LDR14UB) }, { I (INSN_LDR14UB), E (FMT_LDR14UB) }, 
        { I (INSN_LDR14UB), E (FMT_LDR14UB) }, { I (INSN_LDR14UB), E (FMT_LDR14UB) }, 
        { I (INSN_LDR14UB), E (FMT_LDR14UB) }, { I (INSN_LDR14UB), E (FMT_LDR14UB) }, 
        { I (INSN_LDR14UB), E (FMT_LDR14UB) }, { I (INSN_LDR14UB), E (FMT_LDR14UB) }, 
        { I (INSN_LDR14UB), E (FMT_LDR14UB) }, { I (INSN_LDR14UB), E (FMT_LDR14UB) }, 
        { I (INSN_LDR14UB), E (FMT_LDR14UB) }, { I (INSN_LDR14UB), E (FMT_LDR14UB) }, 
        { I (INSN_LDR14UB), E (FMT_LDR14UB) }, { I (INSN_LDR14UB), E (FMT_LDR14UB) }, 
        { I (INSN_LDR14UB), E (FMT_LDR14UB) }, { I (INSN_LDR14UB), E (FMT_LDR14UB) }, 
        { I (INSN_STR14B), E (FMT_STR14B) }, { I (INSN_STR14B), E (FMT_STR14B) }, 
        { I (INSN_STR14B), E (FMT_STR14B) }, { I (INSN_STR14B), E (FMT_STR14B) }, 
        { I (INSN_STR14B), E (FMT_STR14B) }, { I (INSN_STR14B), E (FMT_STR14B) }, 
        { I (INSN_STR14B), E (FMT_STR14B) }, { I (INSN_STR14B), E (FMT_STR14B) }, 
        { I (INSN_STR14B), E (FMT_STR14B) }, { I (INSN_STR14B), E (FMT_STR14B) }, 
        { I (INSN_STR14B), E (FMT_STR14B) }, { I (INSN_STR14B), E (FMT_STR14B) }, 
        { I (INSN_STR14B), E (FMT_STR14B) }, { I (INSN_STR14B), E (FMT_STR14B) }, 
        { I (INSN_STR14B), E (FMT_STR14B) }, { I (INSN_STR14B), E (FMT_STR14B) }, 
        { I (INSN_BANDL), E (FMT_BANDL) }, { I (INSN_BANDH), E (FMT_BANDL) }, 
        { I (INSN_AND), E (FMT_AND) }, { I (INSN_ANDCCR), E (FMT_ANDCCR) }, 
        { I (INSN_ANDM), E (FMT_ANDM) }, { I (INSN_ANDH), E (FMT_ANDH) }, 
        { I (INSN_ANDB), E (FMT_ANDB) }, { I (INSN_STILM), E (FMT_STILM) }, 
        { I (INSN_BTSTL), E (FMT_BTSTL) }, { I (INSN_BTSTH), E (FMT_BTSTL) }, 
        { I (INSN_XCHB), E (FMT_XCHB) }, { I (INSN_MOV), E (FMT_MOV) }, 
        { I (INSN_LDM0), E (FMT_LDM0) }, { I (INSN_LDM1), E (FMT_LDM1) }, 
        { I (INSN_STM0), E (FMT_STM0) }, { I (INSN_STM1), E (FMT_STM1) }, 
        { I (INSN_BORL), E (FMT_BANDL) }, { I (INSN_BORH), E (FMT_BANDL) }, 
        { I (INSN_OR), E (FMT_AND) }, { I (INSN_ORCCR), E (FMT_ANDCCR) }, 
        { I (INSN_ORM), E (FMT_ANDM) }, { I (INSN_ORH), E (FMT_ANDH) }, 
        { I (INSN_ORB), E (FMT_ANDB) }, { 0 }, 
        { I (INSN_BEORL), E (FMT_BANDL) }, { I (INSN_BEORH), E (FMT_BANDL) }, 
        { I (INSN_EOR), E (FMT_AND) }, { I (INSN_LDI20), E (FMT_LDI20) }, 
        { I (INSN_EORM), E (FMT_ANDM) }, { I (INSN_EORH), E (FMT_ANDH) }, 
        { I (INSN_EORB), E (FMT_ANDB) }, { 0 }, 
        { I (INSN_ADDNI), E (FMT_ADDNI) }, { I (INSN_ADDN2), E (FMT_ADDN2) }, 
        { I (INSN_ADDN), E (FMT_ADDN) }, { I (INSN_ADDSP), E (FMT_ADDSP) }, 
        { I (INSN_ADDI), E (FMT_ADDI) }, { I (INSN_ADD2), E (FMT_ADD2) }, 
        { I (INSN_ADD), E (FMT_ADD) }, { I (INSN_ADDC), E (FMT_ADDC) }, 
        { I (INSN_CMPI), E (FMT_CMPI) }, { I (INSN_CMP2), E (FMT_CMP2) }, 
        { I (INSN_CMP), E (FMT_CMP) }, { I (INSN_MULU), E (FMT_MULU) }, 
        { I (INSN_SUB), E (FMT_ADD) }, { I (INSN_SUBC), E (FMT_ADDC) }, 
        { I (INSN_SUBN), E (FMT_ADDN) }, { I (INSN_MUL), E (FMT_MUL) }, 
        { I (INSN_LSRI), E (FMT_LSLI) }, { I (INSN_LSR2), E (FMT_LSLI) }, 
        { I (INSN_LSR), E (FMT_LSL) }, { I (INSN_MOV2DR), E (FMT_MOV2DR) }, 
        { I (INSN_LSLI), E (FMT_LSLI) }, { I (INSN_LSL2), E (FMT_LSLI) }, 
        { I (INSN_LSL), E (FMT_LSL) }, { I (INSN_MOVDR), E (FMT_MOVDR) }, 
        { I (INSN_ASRI), E (FMT_LSLI) }, { I (INSN_ASR2), E (FMT_LSLI) }, 
        { I (INSN_ASR), E (FMT_LSL) }, { I (INSN_MULUH), E (FMT_MULH) }, 
        { I (INSN_LDRES), E (FMT_LDRES) }, { I (INSN_STRES), E (FMT_LDRES) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_MULH), E (FMT_MULH) }, 
        { I (INSN_LDI8), E (FMT_LDI8) }, { I (INSN_LDI8), E (FMT_LDI8) }, 
        { I (INSN_LDI8), E (FMT_LDI8) }, { I (INSN_LDI8), E (FMT_LDI8) }, 
        { I (INSN_LDI8), E (FMT_LDI8) }, { I (INSN_LDI8), E (FMT_LDI8) }, 
        { I (INSN_LDI8), E (FMT_LDI8) }, { I (INSN_LDI8), E (FMT_LDI8) }, 
        { I (INSN_LDI8), E (FMT_LDI8) }, { I (INSN_LDI8), E (FMT_LDI8) }, 
        { I (INSN_LDI8), E (FMT_LDI8) }, { I (INSN_LDI8), E (FMT_LDI8) }, 
        { I (INSN_LDI8), E (FMT_LDI8) }, { I (INSN_LDI8), E (FMT_LDI8) }, 
        { I (INSN_LDI8), E (FMT_LDI8) }, { I (INSN_LDI8), E (FMT_LDI8) }, 
        { I (INSN_CALL), E (FMT_CALL) }, { I (INSN_CALL), E (FMT_CALL) }, 
        { I (INSN_CALL), E (FMT_CALL) }, { I (INSN_CALL), E (FMT_CALL) }, 
        { I (INSN_CALL), E (FMT_CALL) }, { I (INSN_CALL), E (FMT_CALL) }, 
        { I (INSN_CALL), E (FMT_CALL) }, { I (INSN_CALL), E (FMT_CALL) }, 
        { I (INSN_CALLD), E (FMT_CALL) }, { I (INSN_CALLD), E (FMT_CALL) }, 
        { I (INSN_CALLD), E (FMT_CALL) }, { I (INSN_CALLD), E (FMT_CALL) }, 
        { I (INSN_CALLD), E (FMT_CALL) }, { I (INSN_CALLD), E (FMT_CALL) }, 
        { I (INSN_CALLD), E (FMT_CALL) }, { I (INSN_CALLD), E (FMT_CALL) }, 
        { I (INSN_BRA), E (FMT_BRAD) }, { I (INSN_BNO), E (FMT_BNOD) }, 
        { I (INSN_BEQ), E (FMT_BEQD) }, { I (INSN_BNE), E (FMT_BEQD) }, 
        { I (INSN_BC), E (FMT_BCD) }, { I (INSN_BNC), E (FMT_BCD) }, 
        { I (INSN_BN), E (FMT_BND) }, { I (INSN_BP), E (FMT_BND) }, 
        { I (INSN_BV), E (FMT_BVD) }, { I (INSN_BNV), E (FMT_BVD) }, 
        { I (INSN_BLT), E (FMT_BLTD) }, { I (INSN_BGE), E (FMT_BLTD) }, 
        { I (INSN_BLE), E (FMT_BLED) }, { I (INSN_BGT), E (FMT_BLED) }, 
        { I (INSN_BLS), E (FMT_BLSD) }, { I (INSN_BHI), E (FMT_BLSD) }, 
        { I (INSN_BRAD), E (FMT_BRAD) }, { I (INSN_BNOD), E (FMT_BNOD) }, 
        { I (INSN_BEQD), E (FMT_BEQD) }, { I (INSN_BNED), E (FMT_BEQD) }, 
        { I (INSN_BCD), E (FMT_BCD) }, { I (INSN_BNCD), E (FMT_BCD) }, 
        { I (INSN_BND), E (FMT_BND) }, { I (INSN_BPD), E (FMT_BND) }, 
        { I (INSN_BVD), E (FMT_BVD) }, { I (INSN_BNVD), E (FMT_BVD) }, 
        { I (INSN_BLTD), E (FMT_BLTD) }, { I (INSN_BGED), E (FMT_BLTD) }, 
        { I (INSN_BLED), E (FMT_BLED) }, { I (INSN_BGTD), E (FMT_BLED) }, 
        { I (INSN_BLSD), E (FMT_BLSD) }, { I (INSN_BHID), E (FMT_BLSD) }, 
      };
      unsigned int val;
      val = (((insn >> 8) & (255 << 0)));
      DECODE_SWITCH (0, val)
        {
        CASE (0, 7) :
          {
            static const DECODE_DESC insns[16] = {
              { I (INSN_LDR15GR), E (FMT_LDR15GR) }, { I (INSN_MOV2PS), E (FMT_MOV2PS) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_LDR15DR), E (FMT_LDR15DR) }, { I (INSN_LDR15PS), E (FMT_LDR15PS) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
            };
            unsigned int val = (((insn >> 4) & (15 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        CASE (0, 23) :
          {
            static const DECODE_DESC insns[16] = {
              { I (INSN_STR15GR), E (FMT_STR15GR) }, { I (INSN_MOVPS), E (FMT_MOVPS) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_STR15DR), E (FMT_STR15DR) }, { I (INSN_STR15PS), E (FMT_STR15PS) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
            };
            unsigned int val = (((insn >> 4) & (15 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        CASE (0, 151) :
          {
            static const DECODE_DESC insns[16] = {
              { I (INSN_JMP), E (FMT_JMP) }, { I (INSN_CALLR), E (FMT_CALLR) }, 
              { I (INSN_RET), E (FMT_RET) }, { I (INSN_RETI), E (FMT_RETI) }, 
              { I (INSN_DIV0S), E (FMT_DIV0S) }, { I (INSN_DIV0U), E (FMT_DIV0U) }, 
              { I (INSN_DIV1), E (FMT_DIV1) }, { I (INSN_DIV2), E (FMT_DIV2) }, 
              { I (INSN_EXTSB), E (FMT_EXTSB) }, { I (INSN_EXTUB), E (FMT_EXTUB) }, 
              { I (INSN_EXTSH), E (FMT_EXTSH) }, { I (INSN_EXTUH), E (FMT_EXTUH) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
            };
            unsigned int val = (((insn >> 4) & (15 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        CASE (0, 159) :
          {
            static const DECODE_DESC insns[16] = {
              { I (INSN_JMPD), E (FMT_JMP) }, { I (INSN_CALLRD), E (FMT_CALLR) }, 
              { I (INSN_RET_D), E (FMT_RET) }, { I (INSN_INTE), E (FMT_INTE) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_DIV3), E (FMT_DIV3) }, { I (INSN_DIV4S), E (FMT_DIV4S) }, 
              { I (INSN_LDI32), E (FMT_LDI32) }, { I (INSN_LEAVE), E (FMT_LEAVE) }, 
              { I (INSN_NOP), E (FMT_NOP) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_COPOP), E (FMT_COPOP) }, { I (INSN_COPLD), E (FMT_COPLD) }, 
              { I (INSN_COPST), E (FMT_COPST) }, { I (INSN_COPSV), E (FMT_COPST) }, 
            };
            unsigned int val = (((insn >> 4) & (15 << 0)));
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
    CGEN_INSN_INT insn = base_insn;
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
  BREAK (ex);
  }

  CASE (ex, FMT_ADDI) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_ADD2) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_ADDC) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_ADDN) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_ADDNI) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_ADDN2) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_CMP) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_CMPI) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_CMP2) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_AND) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_ANDM) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_ANDH) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_ANDB) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_BANDL) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_BTSTL) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_MUL) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_MULU) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_MULH) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_DIV0S) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_DIV0U) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_div0u.f
  EXTRACT_IFMT_DIV0S_VARS /* f-op1 f-op2 f-op3 f-Ri */

  EXTRACT_IFMT_DIV0S_CODE

  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_div0u", (char *) 0));

#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_DIV1) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_DIV2) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_DIV3) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_div3.f
  EXTRACT_IFMT_DIV3_VARS /* f-op1 f-op2 f-op3 f-op4 */

  EXTRACT_IFMT_DIV3_CODE

  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_div3", (char *) 0));

#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_DIV4S) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_div4s.f
  EXTRACT_IFMT_DIV3_VARS /* f-op1 f-op2 f-op3 f-op4 */

  EXTRACT_IFMT_DIV3_CODE

  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_div4s", (char *) 0));

#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LSL) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_LSLI) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_LDI8) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_LDI20) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_LDI32) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_LD) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_LDUH) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_LDUB) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_LDR13) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_LDR13UH) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_LDR13UB) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_LDR14) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_LDR14UH) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_LDR14UB) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_LDR15) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_LDR15GR) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_LDR15DR) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_LDR15PS) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_ST) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_STH) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_STB) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_STR13) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_STR13H) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_STR13B) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_STR14) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_STR14H) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_STR14B) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_STR15) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_STR15GR) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_STR15DR) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_STR15PS) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_MOV) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_MOVDR) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_MOVPS) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_MOV2DR) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_MOV2PS) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_JMP) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_CALLR) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_CALL) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_RET) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_INT) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_INTE) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_RETI) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_BRAD) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_BNOD) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_bnod.f
  EXTRACT_IFMT_BRAD_VARS /* f-op1 f-cc f-rel9 */

  EXTRACT_IFMT_BRAD_CODE

  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_bnod", (char *) 0));

#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_BEQD) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_BCD) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_BND) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_BVD) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_BLTD) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_BLED) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_BLSD) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_DMOVR13) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_DMOVR13H) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_DMOVR13B) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_DMOVR13PI) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_DMOVR13PIH) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_DMOVR13PIB) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_DMOVR15PI) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_DMOV2R13) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_DMOV2R13H) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_DMOV2R13B) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_DMOV2R13PI) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_DMOV2R13PIH) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_DMOV2R13PIB) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_DMOV2R15PD) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_LDRES) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_COPOP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_copop.f
  EXTRACT_IFMT_COPOP_VARS /* f-op1 f-ccc f-op2 f-op3 f-CRj f-u4c f-CRi */

  EXTRACT_IFMT_COPOP_CODE

  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_copop", (char *) 0));

#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_COPLD) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_copld.f
  EXTRACT_IFMT_COPLD_VARS /* f-op1 f-ccc f-op2 f-op3 f-Rjc f-u4c f-CRi */

  EXTRACT_IFMT_COPLD_CODE

  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_copld", (char *) 0));

#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_COPST) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_copst.f
  EXTRACT_IFMT_COPST_VARS /* f-op1 f-ccc f-op2 f-op3 f-CRj f-u4c f-Ric */

  EXTRACT_IFMT_COPST_CODE

  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_copst", (char *) 0));

#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_NOP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_nop.f
  EXTRACT_IFMT_DIV3_VARS /* f-op1 f-op2 f-op3 f-op4 */

  EXTRACT_IFMT_DIV3_CODE

  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_nop", (char *) 0));

#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_ANDCCR) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_andccr.f
  EXTRACT_IFMT_INT_VARS /* f-op1 f-op2 f-u8 */

  EXTRACT_IFMT_INT_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_u8) = f_u8;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_andccr", "f_u8 0x%x", 'x', f_u8, (char *) 0));

#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STILM) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_stilm.f
  EXTRACT_IFMT_INT_VARS /* f-op1 f-op2 f-u8 */

  EXTRACT_IFMT_INT_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_u8) = f_u8;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stilm", "f_u8 0x%x", 'x', f_u8, (char *) 0));

#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_ADDSP) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_EXTSB) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_EXTUB) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_EXTSH) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_EXTUH) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_LDM0) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_LDM1) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_STM0) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_STM1) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_ENTER) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_LEAVE) :
  {
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
  BREAK (ex);
  }

  CASE (ex, FMT_XCHB) :
  {
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
  BREAK (ex);
  }


      }
    ENDSWITCH (ex)

  }

  return idecode->idesc;
}
