/* Simulator instruction semantics for m32rx.

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

#define WANT_CPU
#define WANT_CPU_M32RX

#include "sim-main.h"
#include "cgen-mem.h"
#include "cgen-ops.h"
#include "cpu-sim.h"

#if ! defined (SCACHE_P) || (defined (SCACHE_P) && WITH_SCACHE)

#undef GET_ATTR
#define GET_ATTR(cpu, num, attr) CGEN_INSN_ATTR (abuf->opcode, CGEN_INSN_##attr)

/* Perform add: add $dr,$sr.  */
CIA
SEM_FN_NAME (m32rx,add) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_0_add.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_0_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_gr[f_r1]) = ADDSI (OPRND (dr), OPRND (sr));
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform add3: add3 $dr,$sr,#$slo16.  */
CIA
SEM_FN_NAME (m32rx,add3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_1_add3.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_1_add3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_gr[f_r1]) = ADDSI (OPRND (sr), OPRND (slo16));
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform and: and $dr,$sr.  */
CIA
SEM_FN_NAME (m32rx,and) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_0_add.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_0_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_gr[f_r1]) = ANDSI (OPRND (dr), OPRND (sr));
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform and3: and3 $dr,$sr,#$uimm16.  */
CIA
SEM_FN_NAME (m32rx,and3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_2_and3.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_2_and3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_gr[f_r1]) = ANDSI (OPRND (sr), OPRND (uimm16));
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform or: or $dr,$sr.  */
CIA
SEM_FN_NAME (m32rx,or) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_0_add.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_0_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_gr[f_r1]) = ORSI (OPRND (dr), OPRND (sr));
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform or3: or3 $dr,$sr,#$ulo16.  */
CIA
SEM_FN_NAME (m32rx,or3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_3_or3.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_3_or3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_gr[f_r1]) = ORSI (OPRND (sr), OPRND (ulo16));
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform xor: xor $dr,$sr.  */
CIA
SEM_FN_NAME (m32rx,xor) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_0_add.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_0_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_gr[f_r1]) = XORSI (OPRND (dr), OPRND (sr));
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform xor3: xor3 $dr,$sr,#$uimm16.  */
CIA
SEM_FN_NAME (m32rx,xor3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_2_and3.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_2_and3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_gr[f_r1]) = XORSI (OPRND (sr), OPRND (uimm16));
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform addi: addi $dr,#$simm8.  */
CIA
SEM_FN_NAME (m32rx,addi) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_4_addi.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_4_addi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_gr[f_r1]) = ADDSI (OPRND (dr), OPRND (simm8));
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform addv: addv $dr,$sr.  */
CIA
SEM_FN_NAME (m32rx,addv) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_0_add.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_0_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
do {
  BI temp1;SI temp0;
  temp0 = ADDSI (OPRND (dr), OPRND (sr));
  temp1 = ADDOFSI (OPRND (dr), OPRND (sr), 0);
  CPU (h_gr[f_r1]) = temp0;
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
  CPU (h_cond) = temp1;
  TRACE_RESULT (current_cpu, "condbit", 'x', OPRND (condbit));
} while (0);
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform addv3: addv3 $dr,$sr,#$simm16.  */
CIA
SEM_FN_NAME (m32rx,addv3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_5_addv3.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_5_addv3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
do {
  BI temp1;SI temp0;
  temp0 = ADDSI (OPRND (sr), OPRND (simm16));
  temp1 = ADDOFSI (OPRND (sr), OPRND (simm16), 0);
  CPU (h_gr[f_r1]) = temp0;
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
  CPU (h_cond) = temp1;
  TRACE_RESULT (current_cpu, "condbit", 'x', OPRND (condbit));
} while (0);
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform addx: addx $dr,$sr.  */
CIA
SEM_FN_NAME (m32rx,addx) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_6_addx.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_6_addx.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
do {
  BI temp1;SI temp0;
  temp0 = ADDCSI (OPRND (dr), OPRND (sr), OPRND (condbit));
  temp1 = ADDCFSI (OPRND (dr), OPRND (sr), OPRND (condbit));
  CPU (h_gr[f_r1]) = temp0;
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
  CPU (h_cond) = temp1;
  TRACE_RESULT (current_cpu, "condbit", 'x', OPRND (condbit));
} while (0);
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform bc8: bc $disp8.  */
CIA
SEM_FN_NAME (m32rx,bc8) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_7_bc8.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_7_bc8.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  int taken_p = 0;
if (OPRND (condbit)) {
  BRANCH_NEW_PC (current_cpu, new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, OPRND (disp8)));
}
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform bc24: bc $disp24.  */
CIA
SEM_FN_NAME (m32rx,bc24) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_8_bc24.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_8_bc24.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  int taken_p = 0;
if (OPRND (condbit)) {
  BRANCH_NEW_PC (current_cpu, new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, OPRND (disp24)));
}
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform beq: beq $src1,$src2,$disp16.  */
CIA
SEM_FN_NAME (m32rx,beq) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_9_beq.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_9_beq.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  int taken_p = 0;
if (EQSI (OPRND (src1), OPRND (src2))) {
  BRANCH_NEW_PC (current_cpu, new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, OPRND (disp16)));
}
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform beqz: beqz $src2,$disp16.  */
CIA
SEM_FN_NAME (m32rx,beqz) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_10_beqz.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_10_beqz.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  int taken_p = 0;
if (EQSI (OPRND (src2), 0)) {
  BRANCH_NEW_PC (current_cpu, new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, OPRND (disp16)));
}
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform bgez: bgez $src2,$disp16.  */
CIA
SEM_FN_NAME (m32rx,bgez) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_10_beqz.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_10_beqz.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  int taken_p = 0;
if (GESI (OPRND (src2), 0)) {
  BRANCH_NEW_PC (current_cpu, new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, OPRND (disp16)));
}
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform bgtz: bgtz $src2,$disp16.  */
CIA
SEM_FN_NAME (m32rx,bgtz) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_10_beqz.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_10_beqz.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  int taken_p = 0;
if (GTSI (OPRND (src2), 0)) {
  BRANCH_NEW_PC (current_cpu, new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, OPRND (disp16)));
}
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform blez: blez $src2,$disp16.  */
CIA
SEM_FN_NAME (m32rx,blez) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_10_beqz.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_10_beqz.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  int taken_p = 0;
if (LESI (OPRND (src2), 0)) {
  BRANCH_NEW_PC (current_cpu, new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, OPRND (disp16)));
}
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform bltz: bltz $src2,$disp16.  */
CIA
SEM_FN_NAME (m32rx,bltz) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_10_beqz.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_10_beqz.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  int taken_p = 0;
if (LTSI (OPRND (src2), 0)) {
  BRANCH_NEW_PC (current_cpu, new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, OPRND (disp16)));
}
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform bnez: bnez $src2,$disp16.  */
CIA
SEM_FN_NAME (m32rx,bnez) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_10_beqz.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_10_beqz.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  int taken_p = 0;
if (NESI (OPRND (src2), 0)) {
  BRANCH_NEW_PC (current_cpu, new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, OPRND (disp16)));
}
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform bl8: bl $disp8.  */
CIA
SEM_FN_NAME (m32rx,bl8) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_11_bl8.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_11_bl8.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  int taken_p = 0;
do {
  CPU (h_gr[14]) = ADDSI (ANDSI (OPRND (pc), -4), 4);
  TRACE_RESULT (current_cpu, "h-gr", 'x', OPRND (h_gr));
  BRANCH_NEW_PC (current_cpu, new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, OPRND (disp8)));
} while (0);
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform bl24: bl $disp24.  */
CIA
SEM_FN_NAME (m32rx,bl24) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_12_bl24.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_12_bl24.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  int taken_p = 0;
do {
  CPU (h_gr[14]) = ADDSI (OPRND (pc), 4);
  TRACE_RESULT (current_cpu, "h-gr", 'x', OPRND (h_gr));
  BRANCH_NEW_PC (current_cpu, new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, OPRND (disp24)));
} while (0);
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform bcl8: bcl $disp8.  */
CIA
SEM_FN_NAME (m32rx,bcl8) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_13_bcl8.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_13_bcl8.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  int taken_p = 0;
if (OPRND (condbit)) {
do {
  CPU (h_gr[14]) = ADDSI (ANDSI (OPRND (pc), -4), 4);
  TRACE_RESULT (current_cpu, "h-gr", 'x', OPRND (h_gr));
  BRANCH_NEW_PC (current_cpu, new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, OPRND (disp8)));
} while (0);
}
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform bcl24: bcl $disp24.  */
CIA
SEM_FN_NAME (m32rx,bcl24) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_14_bcl24.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_14_bcl24.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  int taken_p = 0;
if (OPRND (condbit)) {
do {
  CPU (h_gr[14]) = ADDSI (ANDSI (OPRND (pc), -4), 4);
  TRACE_RESULT (current_cpu, "h-gr", 'x', OPRND (h_gr));
  BRANCH_NEW_PC (current_cpu, new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, OPRND (disp24)));
} while (0);
}
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform bnc8: bnc $disp8.  */
CIA
SEM_FN_NAME (m32rx,bnc8) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_7_bc8.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_7_bc8.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  int taken_p = 0;
if (NOTBI (OPRND (condbit))) {
  BRANCH_NEW_PC (current_cpu, new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, OPRND (disp8)));
}
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform bnc24: bnc $disp24.  */
CIA
SEM_FN_NAME (m32rx,bnc24) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_8_bc24.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_8_bc24.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  int taken_p = 0;
if (NOTBI (OPRND (condbit))) {
  BRANCH_NEW_PC (current_cpu, new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, OPRND (disp24)));
}
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform bne: bne $src1,$src2,$disp16.  */
CIA
SEM_FN_NAME (m32rx,bne) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_9_beq.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_9_beq.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  int taken_p = 0;
if (NESI (OPRND (src1), OPRND (src2))) {
  BRANCH_NEW_PC (current_cpu, new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, OPRND (disp16)));
}
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform bra8: bra $disp8.  */
CIA
SEM_FN_NAME (m32rx,bra8) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_15_bra8.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_15_bra8.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  int taken_p = 0;
  BRANCH_NEW_PC (current_cpu, new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, OPRND (disp8)));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform bra24: bra $disp24.  */
CIA
SEM_FN_NAME (m32rx,bra24) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_16_bra24.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_16_bra24.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  int taken_p = 0;
  BRANCH_NEW_PC (current_cpu, new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, OPRND (disp24)));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform bncl8: bncl $disp8.  */
CIA
SEM_FN_NAME (m32rx,bncl8) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_13_bcl8.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_13_bcl8.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  int taken_p = 0;
if (NOTBI (OPRND (condbit))) {
do {
  CPU (h_gr[14]) = ADDSI (ANDSI (OPRND (pc), -4), 4);
  TRACE_RESULT (current_cpu, "h-gr", 'x', OPRND (h_gr));
  BRANCH_NEW_PC (current_cpu, new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, OPRND (disp8)));
} while (0);
}
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform bncl24: bncl $disp24.  */
CIA
SEM_FN_NAME (m32rx,bncl24) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_14_bcl24.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_14_bcl24.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  int taken_p = 0;
if (NOTBI (OPRND (condbit))) {
do {
  CPU (h_gr[14]) = ADDSI (ANDSI (OPRND (pc), -4), 4);
  TRACE_RESULT (current_cpu, "h-gr", 'x', OPRND (h_gr));
  BRANCH_NEW_PC (current_cpu, new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, OPRND (disp24)));
} while (0);
}
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform cmp: cmp $src1,$src2.  */
CIA
SEM_FN_NAME (m32rx,cmp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_17_cmp.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_17_cmp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_cond) = LTSI (OPRND (src1), OPRND (src2));
  TRACE_RESULT (current_cpu, "condbit", 'x', OPRND (condbit));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform cmpi: cmpi $src2,#$simm16.  */
CIA
SEM_FN_NAME (m32rx,cmpi) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_18_cmpi.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_18_cmpi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_cond) = LTSI (OPRND (src2), OPRND (simm16));
  TRACE_RESULT (current_cpu, "condbit", 'x', OPRND (condbit));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform cmpu: cmpu $src1,$src2.  */
CIA
SEM_FN_NAME (m32rx,cmpu) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_17_cmp.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_17_cmp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_cond) = LTUSI (OPRND (src1), OPRND (src2));
  TRACE_RESULT (current_cpu, "condbit", 'x', OPRND (condbit));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform cmpui: cmpui $src2,#$uimm16.  */
CIA
SEM_FN_NAME (m32rx,cmpui) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_19_cmpui.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_19_cmpui.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_cond) = LTUSI (OPRND (src2), OPRND (uimm16));
  TRACE_RESULT (current_cpu, "condbit", 'x', OPRND (condbit));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform cmpeq: cmpeq $src1,$src2.  */
CIA
SEM_FN_NAME (m32rx,cmpeq) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_17_cmp.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_17_cmp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_cond) = EQSI (OPRND (src1), OPRND (src2));
  TRACE_RESULT (current_cpu, "condbit", 'x', OPRND (condbit));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform cmpz: cmpz $src2.  */
CIA
SEM_FN_NAME (m32rx,cmpz) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_20_cmpz.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_20_cmpz.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_cond) = EQSI (OPRND (src2), 0);
  TRACE_RESULT (current_cpu, "condbit", 'x', OPRND (condbit));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform div: div $dr,$sr.  */
CIA
SEM_FN_NAME (m32rx,div) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_21_div.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_21_div.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
if (NESI (OPRND (sr), 0)) {
  CPU (h_gr[f_r1]) = DIVSI (OPRND (dr), OPRND (sr));
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
}
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform divu: divu $dr,$sr.  */
CIA
SEM_FN_NAME (m32rx,divu) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_21_div.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_21_div.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
if (NESI (OPRND (sr), 0)) {
  CPU (h_gr[f_r1]) = UDIVSI (OPRND (dr), OPRND (sr));
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
}
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform rem: rem $dr,$sr.  */
CIA
SEM_FN_NAME (m32rx,rem) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_21_div.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_21_div.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
if (NESI (OPRND (sr), 0)) {
  CPU (h_gr[f_r1]) = MODSI (OPRND (dr), OPRND (sr));
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
}
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform remu: remu $dr,$sr.  */
CIA
SEM_FN_NAME (m32rx,remu) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_21_div.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_21_div.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
if (NESI (OPRND (sr), 0)) {
  CPU (h_gr[f_r1]) = UMODSI (OPRND (dr), OPRND (sr));
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
}
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform jc: jc $sr.  */
CIA
SEM_FN_NAME (m32rx,jc) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_22_jc.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_22_jc.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  int taken_p = 0;
if (OPRND (condbit)) {
  BRANCH_NEW_PC (current_cpu, new_pc, SEM_BRANCH_VIA_ADDR (sem_arg, ANDSI (OPRND (sr), -4)));
}
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform jnc: jnc $sr.  */
CIA
SEM_FN_NAME (m32rx,jnc) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_22_jc.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_22_jc.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  int taken_p = 0;
if (NOTBI (OPRND (condbit))) {
  BRANCH_NEW_PC (current_cpu, new_pc, SEM_BRANCH_VIA_ADDR (sem_arg, ANDSI (OPRND (sr), -4)));
}
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform jl: jl $sr.  */
CIA
SEM_FN_NAME (m32rx,jl) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_23_jl.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_23_jl.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  int taken_p = 0;
do {
  USI temp1;SI temp0;
  temp0 = ADDSI (ANDSI (OPRND (pc), -4), 4);
  temp1 = OPRND (sr);
  CPU (h_gr[14]) = temp0;
  TRACE_RESULT (current_cpu, "h-gr", 'x', OPRND (h_gr));
  BRANCH_NEW_PC (current_cpu, new_pc, SEM_BRANCH_VIA_ADDR (sem_arg, temp1));
} while (0);
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform jmp: jmp $sr.  */
CIA
SEM_FN_NAME (m32rx,jmp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_24_jmp.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_24_jmp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  int taken_p = 0;
  BRANCH_NEW_PC (current_cpu, new_pc, SEM_BRANCH_VIA_ADDR (sem_arg, OPRND (sr)));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform ld: ld $dr,@$sr.  */
CIA
SEM_FN_NAME (m32rx,ld) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_25_ld.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_25_ld.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_gr[f_r1]) = OPRND (h_memory);
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform ld-d: ld $dr,@($slo16,$sr).  */
CIA
SEM_FN_NAME (m32rx,ld_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_26_ld_d.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_26_ld_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_gr[f_r1]) = OPRND (h_memory);
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform ldb: ldb $dr,@$sr.  */
CIA
SEM_FN_NAME (m32rx,ldb) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_27_ldb.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_27_ldb.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_gr[f_r1]) = EXTQISI (OPRND (h_memory));
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform ldb-d: ldb $dr,@($slo16,$sr).  */
CIA
SEM_FN_NAME (m32rx,ldb_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_28_ldb_d.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_28_ldb_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_gr[f_r1]) = EXTQISI (OPRND (h_memory));
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform ldh: ldh $dr,@$sr.  */
CIA
SEM_FN_NAME (m32rx,ldh) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_29_ldh.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_29_ldh.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_gr[f_r1]) = EXTHISI (OPRND (h_memory));
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform ldh-d: ldh $dr,@($slo16,$sr).  */
CIA
SEM_FN_NAME (m32rx,ldh_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_30_ldh_d.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_30_ldh_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_gr[f_r1]) = EXTHISI (OPRND (h_memory));
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform ldub: ldub $dr,@$sr.  */
CIA
SEM_FN_NAME (m32rx,ldub) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_27_ldb.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_27_ldb.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_gr[f_r1]) = ZEXTQISI (OPRND (h_memory));
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform ldub-d: ldub $dr,@($slo16,$sr).  */
CIA
SEM_FN_NAME (m32rx,ldub_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_28_ldb_d.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_28_ldb_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_gr[f_r1]) = ZEXTQISI (OPRND (h_memory));
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform lduh: lduh $dr,@$sr.  */
CIA
SEM_FN_NAME (m32rx,lduh) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_29_ldh.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_29_ldh.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_gr[f_r1]) = ZEXTHISI (OPRND (h_memory));
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform lduh-d: lduh $dr,@($slo16,$sr).  */
CIA
SEM_FN_NAME (m32rx,lduh_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_30_ldh_d.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_30_ldh_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_gr[f_r1]) = ZEXTHISI (OPRND (h_memory));
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform ld-plus: ld $dr,@$sr+.  */
CIA
SEM_FN_NAME (m32rx,ld_plus) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_25_ld.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_25_ld.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
do {
  SI temp1;SI temp0;
  temp0 = OPRND (h_memory);
  temp1 = ADDSI (OPRND (sr), 4);
  CPU (h_gr[f_r1]) = temp0;
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
  CPU (h_gr[f_r2]) = temp1;
  TRACE_RESULT (current_cpu, "sr", 'x', OPRND (sr));
} while (0);
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform ld24: ld24 $dr,#$uimm24.  */
CIA
SEM_FN_NAME (m32rx,ld24) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_31_ld24.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_31_ld24.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_gr[f_r1]) = OPRND (uimm24);
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform ldi8: ldi $dr,#$simm8.  */
CIA
SEM_FN_NAME (m32rx,ldi8) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_32_ldi8.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_32_ldi8.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_gr[f_r1]) = OPRND (simm8);
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform ldi16: ldi $dr,$slo16.  */
CIA
SEM_FN_NAME (m32rx,ldi16) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_33_ldi16.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_33_ldi16.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_gr[f_r1]) = OPRND (slo16);
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform lock: lock $dr,@$sr.  */
CIA
SEM_FN_NAME (m32rx,lock) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_0_add.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_0_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
do_lock (current_cpu, OPRND (dr), OPRND (sr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform machi: machi $src1,$src2.  */
CIA
SEM_FN_NAME (m32rx,machi) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_34_machi.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_34_machi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_accum) = SRADI (SLLDI (ADDDI (OPRND (accum), MULDI (EXTSIDI (ANDSI (OPRND (src1), 0xffff0000)), EXTHIDI (TRUNCSIHI (SRASI (OPRND (src2), 16))))), 8), 8);
  TRACE_RESULT (current_cpu, "accum", 'D', OPRND (accum));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform machi-a: machi $src1,$src2,$acc.  */
CIA
SEM_FN_NAME (m32rx,machi_a) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_35_machi_a.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_35_machi_a.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
m32rx_h_accums_set (current_cpu, f_acc, SRADI (SLLDI (ADDDI (OPRND (acc), MULDI (EXTSIDI (ANDSI (OPRND (src1), 0xffff0000)), EXTHIDI (TRUNCSIHI (SRASI (OPRND (src2), 16))))), 8), 8));
  TRACE_RESULT (current_cpu, "acc", 'D', OPRND (acc));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform maclo: maclo $src1,$src2.  */
CIA
SEM_FN_NAME (m32rx,maclo) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_34_machi.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_34_machi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_accum) = SRADI (SLLDI (ADDDI (OPRND (accum), MULDI (EXTSIDI (SLLSI (OPRND (src1), 16)), EXTHIDI (TRUNCSIHI (OPRND (src2))))), 8), 8);
  TRACE_RESULT (current_cpu, "accum", 'D', OPRND (accum));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform maclo-a: maclo $src1,$src2,$acc.  */
CIA
SEM_FN_NAME (m32rx,maclo_a) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_35_machi_a.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_35_machi_a.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
m32rx_h_accums_set (current_cpu, f_acc, SRADI (SLLDI (ADDDI (OPRND (acc), MULDI (EXTSIDI (SLLSI (OPRND (src1), 16)), EXTHIDI (TRUNCSIHI (OPRND (src2))))), 8), 8));
  TRACE_RESULT (current_cpu, "acc", 'D', OPRND (acc));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform macwhi: macwhi $src1,$src2.  */
CIA
SEM_FN_NAME (m32rx,macwhi) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_34_machi.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_34_machi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_accum) = SRADI (SLLDI (ADDDI (OPRND (accum), MULDI (EXTSIDI (OPRND (src1)), EXTHIDI (TRUNCSIHI (SRASI (OPRND (src2), 16))))), 8), 8);
  TRACE_RESULT (current_cpu, "accum", 'D', OPRND (accum));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform macwhi-a: macwhi $src1,$src2,$acc.  */
CIA
SEM_FN_NAME (m32rx,macwhi_a) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_35_machi_a.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_35_machi_a.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
m32rx_h_accums_set (current_cpu, f_acc, SRADI (SLLDI (ADDDI (OPRND (acc), MULDI (EXTSIDI (OPRND (src1)), EXTHIDI (TRUNCSIHI (SRASI (OPRND (src2), 16))))), 8), 8));
  TRACE_RESULT (current_cpu, "acc", 'D', OPRND (acc));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform macwlo: macwlo $src1,$src2.  */
CIA
SEM_FN_NAME (m32rx,macwlo) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_34_machi.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_34_machi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_accum) = SRADI (SLLDI (ADDDI (OPRND (accum), MULDI (EXTSIDI (OPRND (src1)), EXTHIDI (TRUNCSIHI (OPRND (src2))))), 8), 8);
  TRACE_RESULT (current_cpu, "accum", 'D', OPRND (accum));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform macwlo-a: macwlo $src1,$src2,$acc.  */
CIA
SEM_FN_NAME (m32rx,macwlo_a) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_35_machi_a.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_35_machi_a.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
m32rx_h_accums_set (current_cpu, f_acc, SRADI (SLLDI (ADDDI (OPRND (acc), MULDI (EXTSIDI (OPRND (src1)), EXTHIDI (TRUNCSIHI (OPRND (src2))))), 8), 8));
  TRACE_RESULT (current_cpu, "acc", 'D', OPRND (acc));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform mul: mul $dr,$sr.  */
CIA
SEM_FN_NAME (m32rx,mul) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_0_add.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_0_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_gr[f_r1]) = MULSI (OPRND (dr), OPRND (sr));
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform mulhi: mulhi $src1,$src2.  */
CIA
SEM_FN_NAME (m32rx,mulhi) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_17_cmp.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_17_cmp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_accum) = SRADI (SLLDI (MULDI (EXTSIDI (ANDSI (OPRND (src1), 0xffff0000)), EXTHIDI (TRUNCSIHI (SRASI (OPRND (src2), 16)))), 16), 16);
  TRACE_RESULT (current_cpu, "accum", 'D', OPRND (accum));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform mulhi-a: mulhi $src1,$src2,$acc.  */
CIA
SEM_FN_NAME (m32rx,mulhi_a) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_36_mulhi_a.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_36_mulhi_a.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
m32rx_h_accums_set (current_cpu, f_acc, SRADI (SLLDI (MULDI (EXTSIDI (ANDSI (OPRND (src1), 0xffff0000)), EXTHIDI (TRUNCSIHI (SRASI (OPRND (src2), 16)))), 16), 16));
  TRACE_RESULT (current_cpu, "acc", 'D', OPRND (acc));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform mullo: mullo $src1,$src2.  */
CIA
SEM_FN_NAME (m32rx,mullo) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_17_cmp.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_17_cmp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_accum) = SRADI (SLLDI (MULDI (EXTSIDI (SLLSI (OPRND (src1), 16)), EXTHIDI (TRUNCSIHI (OPRND (src2)))), 16), 16);
  TRACE_RESULT (current_cpu, "accum", 'D', OPRND (accum));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform mullo-a: mullo $src1,$src2,$acc.  */
CIA
SEM_FN_NAME (m32rx,mullo_a) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_36_mulhi_a.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_36_mulhi_a.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
m32rx_h_accums_set (current_cpu, f_acc, SRADI (SLLDI (MULDI (EXTSIDI (SLLSI (OPRND (src1), 16)), EXTHIDI (TRUNCSIHI (OPRND (src2)))), 16), 16));
  TRACE_RESULT (current_cpu, "acc", 'D', OPRND (acc));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform mulwhi: mulwhi $src1,$src2.  */
CIA
SEM_FN_NAME (m32rx,mulwhi) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_17_cmp.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_17_cmp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_accum) = SRADI (SLLDI (MULDI (EXTSIDI (OPRND (src1)), EXTHIDI (TRUNCSIHI (SRASI (OPRND (src2), 16)))), 8), 8);
  TRACE_RESULT (current_cpu, "accum", 'D', OPRND (accum));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform mulwhi-a: mulwhi $src1,$src2,$acc.  */
CIA
SEM_FN_NAME (m32rx,mulwhi_a) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_36_mulhi_a.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_36_mulhi_a.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
m32rx_h_accums_set (current_cpu, f_acc, SRADI (SLLDI (MULDI (EXTSIDI (OPRND (src1)), EXTHIDI (TRUNCSIHI (SRASI (OPRND (src2), 16)))), 8), 8));
  TRACE_RESULT (current_cpu, "acc", 'D', OPRND (acc));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform mulwlo: mulwlo $src1,$src2.  */
CIA
SEM_FN_NAME (m32rx,mulwlo) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_17_cmp.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_17_cmp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_accum) = SRADI (SLLDI (MULDI (EXTSIDI (OPRND (src1)), EXTHIDI (TRUNCSIHI (OPRND (src2)))), 8), 8);
  TRACE_RESULT (current_cpu, "accum", 'D', OPRND (accum));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform mulwlo-a: mulwlo $src1,$src2,$acc.  */
CIA
SEM_FN_NAME (m32rx,mulwlo_a) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_36_mulhi_a.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_36_mulhi_a.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
m32rx_h_accums_set (current_cpu, f_acc, SRADI (SLLDI (MULDI (EXTSIDI (OPRND (src1)), EXTHIDI (TRUNCSIHI (OPRND (src2)))), 8), 8));
  TRACE_RESULT (current_cpu, "acc", 'D', OPRND (acc));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform mv: mv $dr,$sr.  */
CIA
SEM_FN_NAME (m32rx,mv) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_37_mv.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_37_mv.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_gr[f_r1]) = OPRND (sr);
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform mvfachi: mvfachi $dr.  */
CIA
SEM_FN_NAME (m32rx,mvfachi) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_38_mvfachi.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_38_mvfachi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_gr[f_r1]) = TRUNCDISI (SRADI (OPRND (accum), 32));
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform mvfachi-a: mvfachi $dr,$accs.  */
CIA
SEM_FN_NAME (m32rx,mvfachi_a) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_39_mvfachi_a.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_39_mvfachi_a.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_gr[f_r1]) = TRUNCDISI (SRADI (OPRND (accs), 32));
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform mvfaclo: mvfaclo $dr.  */
CIA
SEM_FN_NAME (m32rx,mvfaclo) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_38_mvfachi.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_38_mvfachi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_gr[f_r1]) = TRUNCDISI (OPRND (accum));
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform mvfaclo-a: mvfaclo $dr,$accs.  */
CIA
SEM_FN_NAME (m32rx,mvfaclo_a) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_39_mvfachi_a.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_39_mvfachi_a.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_gr[f_r1]) = TRUNCDISI (OPRND (accs));
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform mvfacmi: mvfacmi $dr.  */
CIA
SEM_FN_NAME (m32rx,mvfacmi) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_38_mvfachi.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_38_mvfachi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_gr[f_r1]) = TRUNCDISI (SRADI (OPRND (accum), 16));
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform mvfacmi-a: mvfacmi $dr,$accs.  */
CIA
SEM_FN_NAME (m32rx,mvfacmi_a) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_39_mvfachi_a.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_39_mvfachi_a.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_gr[f_r1]) = TRUNCDISI (SRADI (OPRND (accs), 16));
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform mvfc: mvfc $dr,$scr.  */
CIA
SEM_FN_NAME (m32rx,mvfc) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_40_mvfc.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_40_mvfc.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_gr[f_r1]) = OPRND (scr);
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform mvtachi: mvtachi $src1.  */
CIA
SEM_FN_NAME (m32rx,mvtachi) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_41_mvtachi.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_41_mvtachi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_accum) = ORDI (ANDDI (OPRND (accum), MAKEDI (0, 0xffffffff)), SLLDI (EXTSIDI (OPRND (src1)), 32));
  TRACE_RESULT (current_cpu, "accum", 'D', OPRND (accum));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform mvtachi-a: mvtachi $src1,$accs.  */
CIA
SEM_FN_NAME (m32rx,mvtachi_a) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_42_mvtachi_a.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_42_mvtachi_a.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
m32rx_h_accums_set (current_cpu, f_accs, ORDI (ANDDI (OPRND (accs), MAKEDI (0, 0xffffffff)), SLLDI (EXTSIDI (OPRND (src1)), 32)));
  TRACE_RESULT (current_cpu, "accs", 'D', OPRND (accs));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform mvtaclo: mvtaclo $src1.  */
CIA
SEM_FN_NAME (m32rx,mvtaclo) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_41_mvtachi.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_41_mvtachi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_accum) = ORDI (ANDDI (OPRND (accum), MAKEDI (0xffffffff, 0)), EXTSIDI (OPRND (src1)));
  TRACE_RESULT (current_cpu, "accum", 'D', OPRND (accum));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform mvtaclo-a: mvtaclo $src1,$accs.  */
CIA
SEM_FN_NAME (m32rx,mvtaclo_a) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_42_mvtachi_a.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_42_mvtachi_a.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
m32rx_h_accums_set (current_cpu, f_accs, ORDI (ANDDI (OPRND (accs), MAKEDI (0xffffffff, 0)), EXTSIDI (OPRND (src1))));
  TRACE_RESULT (current_cpu, "accs", 'D', OPRND (accs));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform mvtc: mvtc $sr,$dcr.  */
CIA
SEM_FN_NAME (m32rx,mvtc) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_43_mvtc.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_43_mvtc.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
m32rx_h_cr_set (current_cpu, f_r1, OPRND (sr));
  TRACE_RESULT (current_cpu, "dcr", 'x', OPRND (dcr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform neg: neg $dr,$sr.  */
CIA
SEM_FN_NAME (m32rx,neg) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_37_mv.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_37_mv.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_gr[f_r1]) = NEGSI (OPRND (sr));
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform nop: nop.  */
CIA
SEM_FN_NAME (m32rx,nop) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_44_nop.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_44_nop.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
PROFILE_COUNT_FILLNOPS (current_cpu, abuf->addr);
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform not: not $dr,$sr.  */
CIA
SEM_FN_NAME (m32rx,not) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_37_mv.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_37_mv.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_gr[f_r1]) = INVSI (OPRND (sr));
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform rac: rac.  */
CIA
SEM_FN_NAME (m32rx,rac) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_45_rac.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_45_rac.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
do {
  DI tmp_tmp1;
  tmp_tmp1 = ANDDI (OPRND (accum), MAKEDI (16777215, 0xffffffff));
if (ANDIFSI (GEDI (tmp_tmp1, MAKEDI (16383, 0xffff8000)), LEDI (tmp_tmp1, MAKEDI (8388607, 0xffffffff)))) {
  tmp_tmp1 = MAKEDI (16383, 0xffff8000);
} else {
if (ANDIFSI (GEDI (tmp_tmp1, MAKEDI (8388608, 0)), LEDI (tmp_tmp1, MAKEDI (16760832, 0)))) {
  tmp_tmp1 = MAKEDI (16760832, 0);
} else {
  tmp_tmp1 = ANDDI (ADDDI (OPRND (accum), MAKEDI (0, 16384)), MAKEDI (16777215, 0xffff8000));
}
}
  tmp_tmp1 = SLLDI (tmp_tmp1, 1);
  CPU (h_accum) = SRADI (SLLDI (tmp_tmp1, 7), 7);
  TRACE_RESULT (current_cpu, "accum", 'D', OPRND (accum));
} while (0);
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform rac-a: rac $accs.  */
CIA
SEM_FN_NAME (m32rx,rac_a) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_46_rac_a.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_46_rac_a.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
do {
  DI tmp_tmp1;
  tmp_tmp1 = ANDDI (OPRND (accs), MAKEDI (16777215, 0xffffffff));
if (ANDIFSI (GEDI (tmp_tmp1, MAKEDI (16383, 0xffff8000)), LEDI (tmp_tmp1, MAKEDI (8388607, 0xffffffff)))) {
  tmp_tmp1 = MAKEDI (16383, 0xffff8000);
} else {
if (ANDIFSI (GEDI (tmp_tmp1, MAKEDI (8388608, 0)), LEDI (tmp_tmp1, MAKEDI (16760832, 0)))) {
  tmp_tmp1 = MAKEDI (16760832, 0);
} else {
  tmp_tmp1 = ANDDI (ADDDI (OPRND (accs), MAKEDI (0, 16384)), MAKEDI (16777215, 0xffff8000));
}
}
  tmp_tmp1 = SLLDI (tmp_tmp1, 1);
m32rx_h_accums_set (current_cpu, f_accs, SRADI (SLLDI (tmp_tmp1, 7), 7));
  TRACE_RESULT (current_cpu, "accs", 'D', OPRND (accs));
} while (0);
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform rach: rach.  */
CIA
SEM_FN_NAME (m32rx,rach) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_45_rac.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_45_rac.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
do {
  DI tmp_tmp1;
  tmp_tmp1 = ANDDI (OPRND (accum), MAKEDI (16777215, 0xffffffff));
if (ANDIFSI (GEDI (tmp_tmp1, MAKEDI (16383, 0x80000000)), LEDI (tmp_tmp1, MAKEDI (8388607, 0xffffffff)))) {
  tmp_tmp1 = MAKEDI (16383, 0x80000000);
} else {
if (ANDIFSI (GEDI (tmp_tmp1, MAKEDI (8388608, 0)), LEDI (tmp_tmp1, MAKEDI (16760832, 0)))) {
  tmp_tmp1 = MAKEDI (16760832, 0);
} else {
  tmp_tmp1 = ANDDI (ADDDI (OPRND (accum), MAKEDI (0, 1073741824)), MAKEDI (0xffffffff, 0x80000000));
}
}
  tmp_tmp1 = SLLDI (tmp_tmp1, 1);
  CPU (h_accum) = SRADI (SLLDI (tmp_tmp1, 7), 7);
  TRACE_RESULT (current_cpu, "accum", 'D', OPRND (accum));
} while (0);
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform rach-a: rach $accs.  */
CIA
SEM_FN_NAME (m32rx,rach_a) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_46_rac_a.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_46_rac_a.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
do {
  DI tmp_tmp1;
  tmp_tmp1 = ANDDI (OPRND (accs), MAKEDI (16777215, 0xffffffff));
if (ANDIFSI (GEDI (tmp_tmp1, MAKEDI (16383, 0x80000000)), LEDI (tmp_tmp1, MAKEDI (8388607, 0xffffffff)))) {
  tmp_tmp1 = MAKEDI (16383, 0x80000000);
} else {
if (ANDIFSI (GEDI (tmp_tmp1, MAKEDI (8388608, 0)), LEDI (tmp_tmp1, MAKEDI (16760832, 0)))) {
  tmp_tmp1 = MAKEDI (16760832, 0);
} else {
  tmp_tmp1 = ANDDI (ADDDI (OPRND (accs), MAKEDI (0, 1073741824)), MAKEDI (0xffffffff, 0x80000000));
}
}
  tmp_tmp1 = SLLDI (tmp_tmp1, 1);
m32rx_h_accums_set (current_cpu, f_accs, SRADI (SLLDI (tmp_tmp1, 7), 7));
  TRACE_RESULT (current_cpu, "accs", 'D', OPRND (accs));
} while (0);
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform rte: rte.  */
CIA
SEM_FN_NAME (m32rx,rte) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_44_nop.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_44_nop.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  int taken_p = 0;
do {
  CPU (h_sm) = OPRND (h_bsm);
  TRACE_RESULT (current_cpu, "h-sm", 'x', OPRND (h_sm));
  CPU (h_ie) = OPRND (h_bie);
  TRACE_RESULT (current_cpu, "h-ie", 'x', OPRND (h_ie));
  CPU (h_cond) = OPRND (h_bcond);
  TRACE_RESULT (current_cpu, "condbit", 'x', OPRND (condbit));
  BRANCH_NEW_PC (current_cpu, new_pc, SEM_BRANCH_VIA_ADDR (sem_arg, OPRND (h_bpc)));
  TRACE_RESULT (current_cpu, "pc", 'x', OPRND (pc));
} while (0);
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform seth: seth $dr,$hi16.  */
CIA
SEM_FN_NAME (m32rx,seth) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_47_seth.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_47_seth.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_gr[f_r1]) = SLLSI (OPRND (hi16), 16);
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform sll: sll $dr,$sr.  */
CIA
SEM_FN_NAME (m32rx,sll) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_0_add.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_0_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_gr[f_r1]) = SLLSI (OPRND (dr), ANDSI (OPRND (sr), 31));
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform sll3: sll3 $dr,$sr,#$simm16.  */
CIA
SEM_FN_NAME (m32rx,sll3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_5_addv3.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_5_addv3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_gr[f_r1]) = SLLSI (OPRND (sr), ANDSI (OPRND (simm16), 31));
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform slli: slli $dr,#$uimm5.  */
CIA
SEM_FN_NAME (m32rx,slli) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_48_slli.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_48_slli.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_gr[f_r1]) = SLLSI (OPRND (dr), OPRND (uimm5));
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform sra: sra $dr,$sr.  */
CIA
SEM_FN_NAME (m32rx,sra) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_0_add.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_0_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_gr[f_r1]) = SRASI (OPRND (dr), ANDSI (OPRND (sr), 31));
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform sra3: sra3 $dr,$sr,#$simm16.  */
CIA
SEM_FN_NAME (m32rx,sra3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_5_addv3.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_5_addv3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_gr[f_r1]) = SRASI (OPRND (sr), ANDSI (OPRND (simm16), 31));
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform srai: srai $dr,#$uimm5.  */
CIA
SEM_FN_NAME (m32rx,srai) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_48_slli.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_48_slli.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_gr[f_r1]) = SRASI (OPRND (dr), OPRND (uimm5));
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform srl: srl $dr,$sr.  */
CIA
SEM_FN_NAME (m32rx,srl) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_0_add.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_0_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_gr[f_r1]) = SRLSI (OPRND (dr), ANDSI (OPRND (sr), 31));
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform srl3: srl3 $dr,$sr,#$simm16.  */
CIA
SEM_FN_NAME (m32rx,srl3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_5_addv3.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_5_addv3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_gr[f_r1]) = SRLSI (OPRND (sr), ANDSI (OPRND (simm16), 31));
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform srli: srli $dr,#$uimm5.  */
CIA
SEM_FN_NAME (m32rx,srli) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_48_slli.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_48_slli.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_gr[f_r1]) = SRLSI (OPRND (dr), OPRND (uimm5));
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform st: st $src1,@$src2.  */
CIA
SEM_FN_NAME (m32rx,st) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_17_cmp.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_17_cmp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
SETMEMSI (current_cpu, OPRND (src2), OPRND (src1));
  TRACE_RESULT (current_cpu, "h-memory", 'x', OPRND (h_memory));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform st-d: st $src1,@($slo16,$src2).  */
CIA
SEM_FN_NAME (m32rx,st_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_49_st_d.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_49_st_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
SETMEMSI (current_cpu, ADDSI (OPRND (src2), OPRND (slo16)), OPRND (src1));
  TRACE_RESULT (current_cpu, "h-memory", 'x', OPRND (h_memory));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform stb: stb $src1,@$src2.  */
CIA
SEM_FN_NAME (m32rx,stb) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_17_cmp.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_17_cmp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
SETMEMQI (current_cpu, OPRND (src2), OPRND (src1));
  TRACE_RESULT (current_cpu, "h-memory", 'x', OPRND (h_memory));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform stb-d: stb $src1,@($slo16,$src2).  */
CIA
SEM_FN_NAME (m32rx,stb_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_49_st_d.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_49_st_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
SETMEMQI (current_cpu, ADDSI (OPRND (src2), OPRND (slo16)), OPRND (src1));
  TRACE_RESULT (current_cpu, "h-memory", 'x', OPRND (h_memory));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform sth: sth $src1,@$src2.  */
CIA
SEM_FN_NAME (m32rx,sth) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_17_cmp.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_17_cmp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
SETMEMHI (current_cpu, OPRND (src2), OPRND (src1));
  TRACE_RESULT (current_cpu, "h-memory", 'x', OPRND (h_memory));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform sth-d: sth $src1,@($slo16,$src2).  */
CIA
SEM_FN_NAME (m32rx,sth_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_49_st_d.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_49_st_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
SETMEMHI (current_cpu, ADDSI (OPRND (src2), OPRND (slo16)), OPRND (src1));
  TRACE_RESULT (current_cpu, "h-memory", 'x', OPRND (h_memory));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform st-plus: st $src1,@+$src2.  */
CIA
SEM_FN_NAME (m32rx,st_plus) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_17_cmp.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_17_cmp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
do {
  CPU (h_gr[f_r2]) = ADDSI (OPRND (src2), 4);
  TRACE_RESULT (current_cpu, "src2", 'x', OPRND (src2));
SETMEMSI (current_cpu, OPRND (src2), OPRND (src1));
  TRACE_RESULT (current_cpu, "h-memory", 'x', OPRND (h_memory));
} while (0);
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform st-minus: st $src1,@-$src2.  */
CIA
SEM_FN_NAME (m32rx,st_minus) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_17_cmp.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_17_cmp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
do {
  CPU (h_gr[f_r2]) = SUBSI (OPRND (src2), 4);
  TRACE_RESULT (current_cpu, "src2", 'x', OPRND (src2));
SETMEMSI (current_cpu, OPRND (src2), OPRND (src1));
  TRACE_RESULT (current_cpu, "h-memory", 'x', OPRND (h_memory));
} while (0);
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform sub: sub $dr,$sr.  */
CIA
SEM_FN_NAME (m32rx,sub) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_0_add.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_0_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_gr[f_r1]) = SUBSI (OPRND (dr), OPRND (sr));
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform subv: subv $dr,$sr.  */
CIA
SEM_FN_NAME (m32rx,subv) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_0_add.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_0_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
do {
  BI temp1;SI temp0;
  temp0 = SUBSI (OPRND (dr), OPRND (sr));
  temp1 = SUBOFSI (OPRND (dr), OPRND (sr), 0);
  CPU (h_gr[f_r1]) = temp0;
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
  CPU (h_cond) = temp1;
  TRACE_RESULT (current_cpu, "condbit", 'x', OPRND (condbit));
} while (0);
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform subx: subx $dr,$sr.  */
CIA
SEM_FN_NAME (m32rx,subx) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_6_addx.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_6_addx.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
do {
  BI temp1;SI temp0;
  temp0 = SUBCSI (OPRND (dr), OPRND (sr), OPRND (condbit));
  temp1 = SUBCFSI (OPRND (dr), OPRND (sr), OPRND (condbit));
  CPU (h_gr[f_r1]) = temp0;
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
  CPU (h_cond) = temp1;
  TRACE_RESULT (current_cpu, "condbit", 'x', OPRND (condbit));
} while (0);
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform trap: trap #$uimm4.  */
CIA
SEM_FN_NAME (m32rx,trap) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_50_trap.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_50_trap.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  int taken_p = 0;
do_trap (current_cpu, OPRND (uimm4));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform unlock: unlock $src1,@$src2.  */
CIA
SEM_FN_NAME (m32rx,unlock) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_17_cmp.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_17_cmp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
do_unlock (current_cpu, OPRND (src1), OPRND (src2));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform satb: satb $dr,$src2.  */
CIA
SEM_FN_NAME (m32rx,satb) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_51_satb.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_51_satb.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_gr[f_r1]) = (GESI (OPRND (src2), 127)) ? (127) : (LESI (OPRND (src2), -128)) ? (-128) : (OPRND (src2));
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform sath: sath $dr,$src2.  */
CIA
SEM_FN_NAME (m32rx,sath) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_51_satb.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_51_satb.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_gr[f_r1]) = (GESI (OPRND (src2), 32767)) ? (32767) : (LESI (OPRND (src2), -32768)) ? (-32768) : (OPRND (src2));
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform sat: sat $dr,$src2.  */
CIA
SEM_FN_NAME (m32rx,sat) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_51_satb.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_51_satb.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_gr[f_r1]) = (OPRND (condbit)) ? ((LTSI (OPRND (src2), 0)) ? (2147483647) : (0x80000000)) : (OPRND (src2));
  TRACE_RESULT (current_cpu, "dr", 'x', OPRND (dr));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform pcmpbz: pcmpbz $src2.  */
CIA
SEM_FN_NAME (m32rx,pcmpbz) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_52_pcmpbz.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_52_pcmpbz.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_cond) = (EQSI (ANDSI (OPRND (src2), 255), 0)) ? (1) : (EQSI (ANDSI (OPRND (src2), 65280), 0)) ? (1) : (EQSI (ANDSI (OPRND (src2), 16711680), 0)) ? (1) : (EQSI (ANDSI (OPRND (src2), 0xff000000), 0)) ? (1) : (0);
  TRACE_RESULT (current_cpu, "condbit", 'x', OPRND (condbit));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform sadd: sadd.  */
CIA
SEM_FN_NAME (m32rx,sadd) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_53_sadd.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_53_sadd.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
m32rx_h_accums_set (current_cpu, 0, ADDDI (SRADI (OPRND (h_accums), 16), OPRND (h_accums)));
  TRACE_RESULT (current_cpu, "h-accums", 'D', OPRND (h_accums));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform macwu1: macwu1 $src1,$src2.  */
CIA
SEM_FN_NAME (m32rx,macwu1) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_54_macwu1.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_54_macwu1.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
m32rx_h_accums_set (current_cpu, 1, SRADI (SLLDI (ADDDI (OPRND (h_accums), MULDI (EXTSIDI (OPRND (src1)), EXTSIDI (ANDSI (OPRND (src2), 65535)))), 8), 8));
  TRACE_RESULT (current_cpu, "h-accums", 'D', OPRND (h_accums));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform msblo: msblo $src1,$src2.  */
CIA
SEM_FN_NAME (m32rx,msblo) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_34_machi.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_34_machi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
  CPU (h_accum) = SRADI (SLLDI (SUBDI (OPRND (accum), SRADI (SLLDI (MULDI (EXTHIDI (TRUNCSIHI (OPRND (src1))), EXTHIDI (TRUNCSIHI (OPRND (src2)))), 32), 16)), 8), 8);
  TRACE_RESULT (current_cpu, "accum", 'D', OPRND (accum));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform mulwu1: mulwu1 $src1,$src2.  */
CIA
SEM_FN_NAME (m32rx,mulwu1) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_17_cmp.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_17_cmp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
m32rx_h_accums_set (current_cpu, 1, SRADI (SLLDI (MULDI (EXTSIDI (OPRND (src1)), EXTSIDI (ANDSI (OPRND (src2), 65535))), 16), 16));
  TRACE_RESULT (current_cpu, "h-accums", 'D', OPRND (h_accums));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform machl1: machl1 $src1,$src2.  */
CIA
SEM_FN_NAME (m32rx,machl1) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_54_macwu1.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_54_macwu1.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
m32rx_h_accums_set (current_cpu, 1, SRADI (SLLDI (ADDDI (OPRND (h_accums), SRADI (SLLDI (MULDI (EXTSIDI (SRASI (OPRND (src1), 16)), EXTHIDI (TRUNCSIHI (OPRND (src2)))), 32), 16)), 8), 8));
  TRACE_RESULT (current_cpu, "h-accums", 'D', OPRND (h_accums));
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform sc: sc.  */
CIA
SEM_FN_NAME (m32rx,sc) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_55_sc.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_55_sc.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
if (OPRND (condbit)) {
  CPU (h_abort) = 1;
  TRACE_RESULT (current_cpu, "abort-parallel-execution", 'x', OPRND (abort_parallel_execution));
}
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* Perform snc: snc.  */
CIA
SEM_FN_NAME (m32rx,snc) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_55_sc.f
#define OPRND(f) CPU_PAR_EXEC (current_cpu)->operands.fmt_55_sc.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg);
if (NOTBI (OPRND (condbit))) {
  CPU (h_abort) = 1;
  TRACE_RESULT (current_cpu, "abort-parallel-execution", 'x', OPRND (abort_parallel_execution));
}
#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif
  return new_pc;
#undef OPRND
#undef FLD
}

/* FIXME: Add "no return" attribute to illegal insn handlers.
   They all call longjmp.  */

PCADDR
SEM_FN_NAME (m32rx,illegal) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
  sim_engine_illegal_insn (current_cpu, NULL_CIA /*FIXME*/);
  return 0;
}

#endif /* ! defined (SCACHE_P) || (defined (SCACHE_P) && WITH_SCACHE) */
