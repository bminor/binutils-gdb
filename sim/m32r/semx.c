/* Simulator instruction semantics for m32rx.

This file is machine generated with CGEN.

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
SEM_FN_NAME (m32rx,add) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_0_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_0_ADD_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_0_ADD_CODE

  CPU (h_gr[f_r1]) = ADDSI (OPRND (dr), OPRND (sr));
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));

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
}

/* Perform add3: add3 $dr,$sr,#$slo16.  */
CIA
SEM_FN_NAME (m32rx,add3) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_1_add3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 4;
  EXTRACT_FMT_1_ADD3_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_1_ADD3_CODE

  CPU (h_gr[f_r1]) = ADDSI (OPRND (sr), OPRND (slo16));
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));

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
}

/* Perform and: and $dr,$sr.  */
CIA
SEM_FN_NAME (m32rx,and) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_0_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_0_ADD_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_0_ADD_CODE

  CPU (h_gr[f_r1]) = ANDSI (OPRND (dr), OPRND (sr));
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));

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
}

/* Perform and3: and3 $dr,$sr,#$uimm16.  */
CIA
SEM_FN_NAME (m32rx,and3) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_2_and3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 4;
  EXTRACT_FMT_2_AND3_VARS /* f-op1 f-r1 f-op2 f-r2 f-uimm16 */
  EXTRACT_FMT_2_AND3_CODE

  CPU (h_gr[f_r1]) = ANDSI (OPRND (sr), OPRND (uimm16));
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));

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
}

/* Perform or: or $dr,$sr.  */
CIA
SEM_FN_NAME (m32rx,or) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_0_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_0_ADD_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_0_ADD_CODE

  CPU (h_gr[f_r1]) = ORSI (OPRND (dr), OPRND (sr));
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));

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
}

/* Perform or3: or3 $dr,$sr,#$ulo16.  */
CIA
SEM_FN_NAME (m32rx,or3) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_3_or3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 4;
  EXTRACT_FMT_3_OR3_VARS /* f-op1 f-r1 f-op2 f-r2 f-uimm16 */
  EXTRACT_FMT_3_OR3_CODE

  CPU (h_gr[f_r1]) = ORSI (OPRND (sr), OPRND (ulo16));
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));

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
}

/* Perform xor: xor $dr,$sr.  */
CIA
SEM_FN_NAME (m32rx,xor) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_0_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_0_ADD_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_0_ADD_CODE

  CPU (h_gr[f_r1]) = XORSI (OPRND (dr), OPRND (sr));
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));

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
}

/* Perform xor3: xor3 $dr,$sr,#$uimm16.  */
CIA
SEM_FN_NAME (m32rx,xor3) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_2_and3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 4;
  EXTRACT_FMT_2_AND3_VARS /* f-op1 f-r1 f-op2 f-r2 f-uimm16 */
  EXTRACT_FMT_2_AND3_CODE

  CPU (h_gr[f_r1]) = XORSI (OPRND (sr), OPRND (uimm16));
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));

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
}

/* Perform addi: addi $dr,#$simm8.  */
CIA
SEM_FN_NAME (m32rx,addi) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_4_addi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_4_ADDI_VARS /* f-op1 f-r1 f-simm8 */
  EXTRACT_FMT_4_ADDI_CODE

  CPU (h_gr[f_r1]) = ADDSI (OPRND (dr), OPRND (simm8));
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));

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
}

/* Perform addv: addv $dr,$sr.  */
CIA
SEM_FN_NAME (m32rx,addv) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_5_addv.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_5_ADDV_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_5_ADDV_CODE

do {
  BI temp1;SI temp0;
  temp0 = ADDSI (OPRND (dr), OPRND (sr));
  temp1 = ADDOFSI (OPRND (dr), OPRND (sr), 0);
  CPU (h_gr[f_r1]) = temp0;
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));
  CPU (h_cond) = temp1;
  TRACE_RESULT (current_cpu, "condbit", 'x', CPU (h_cond));
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
}

/* Perform addv3: addv3 $dr,$sr,#$simm16.  */
CIA
SEM_FN_NAME (m32rx,addv3) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_6_addv3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 4;
  EXTRACT_FMT_6_ADDV3_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_6_ADDV3_CODE

do {
  BI temp1;SI temp0;
  temp0 = ADDSI (OPRND (sr), OPRND (simm16));
  temp1 = ADDOFSI (OPRND (sr), OPRND (simm16), 0);
  CPU (h_gr[f_r1]) = temp0;
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));
  CPU (h_cond) = temp1;
  TRACE_RESULT (current_cpu, "condbit", 'x', CPU (h_cond));
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
}

/* Perform addx: addx $dr,$sr.  */
CIA
SEM_FN_NAME (m32rx,addx) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_7_addx.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_7_ADDX_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_7_ADDX_CODE

do {
  BI temp1;SI temp0;
  temp0 = ADDCSI (OPRND (dr), OPRND (sr), OPRND (condbit));
  temp1 = ADDCFSI (OPRND (dr), OPRND (sr), OPRND (condbit));
  CPU (h_gr[f_r1]) = temp0;
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));
  CPU (h_cond) = temp1;
  TRACE_RESULT (current_cpu, "condbit", 'x', CPU (h_cond));
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
}

/* Perform bc8: bc $disp8.  */
CIA
SEM_FN_NAME (m32rx,bc8) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_8_bc8.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  int taken_p = 0;
  EXTRACT_FMT_8_BC8_VARS /* f-op1 f-r1 f-disp8 */
  EXTRACT_FMT_8_BC8_CODE

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
}

/* Perform bc24: bc $disp24.  */
CIA
SEM_FN_NAME (m32rx,bc24) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_9_bc24.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 4;
  int taken_p = 0;
  EXTRACT_FMT_9_BC24_VARS /* f-op1 f-r1 f-disp24 */
  EXTRACT_FMT_9_BC24_CODE

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
}

/* Perform beq: beq $src1,$src2,$disp16.  */
CIA
SEM_FN_NAME (m32rx,beq) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_10_beq.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 4;
  int taken_p = 0;
  EXTRACT_FMT_10_BEQ_VARS /* f-op1 f-r1 f-op2 f-r2 f-disp16 */
  EXTRACT_FMT_10_BEQ_CODE

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
}

/* Perform beqz: beqz $src2,$disp16.  */
CIA
SEM_FN_NAME (m32rx,beqz) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_11_beqz.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 4;
  int taken_p = 0;
  EXTRACT_FMT_11_BEQZ_VARS /* f-op1 f-r1 f-op2 f-r2 f-disp16 */
  EXTRACT_FMT_11_BEQZ_CODE

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
}

/* Perform bgez: bgez $src2,$disp16.  */
CIA
SEM_FN_NAME (m32rx,bgez) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_11_beqz.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 4;
  int taken_p = 0;
  EXTRACT_FMT_11_BEQZ_VARS /* f-op1 f-r1 f-op2 f-r2 f-disp16 */
  EXTRACT_FMT_11_BEQZ_CODE

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
}

/* Perform bgtz: bgtz $src2,$disp16.  */
CIA
SEM_FN_NAME (m32rx,bgtz) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_11_beqz.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 4;
  int taken_p = 0;
  EXTRACT_FMT_11_BEQZ_VARS /* f-op1 f-r1 f-op2 f-r2 f-disp16 */
  EXTRACT_FMT_11_BEQZ_CODE

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
}

/* Perform blez: blez $src2,$disp16.  */
CIA
SEM_FN_NAME (m32rx,blez) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_11_beqz.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 4;
  int taken_p = 0;
  EXTRACT_FMT_11_BEQZ_VARS /* f-op1 f-r1 f-op2 f-r2 f-disp16 */
  EXTRACT_FMT_11_BEQZ_CODE

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
}

/* Perform bltz: bltz $src2,$disp16.  */
CIA
SEM_FN_NAME (m32rx,bltz) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_11_beqz.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 4;
  int taken_p = 0;
  EXTRACT_FMT_11_BEQZ_VARS /* f-op1 f-r1 f-op2 f-r2 f-disp16 */
  EXTRACT_FMT_11_BEQZ_CODE

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
}

/* Perform bnez: bnez $src2,$disp16.  */
CIA
SEM_FN_NAME (m32rx,bnez) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_11_beqz.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 4;
  int taken_p = 0;
  EXTRACT_FMT_11_BEQZ_VARS /* f-op1 f-r1 f-op2 f-r2 f-disp16 */
  EXTRACT_FMT_11_BEQZ_CODE

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
}

/* Perform bl8: bl $disp8.  */
CIA
SEM_FN_NAME (m32rx,bl8) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_12_bl8.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  int taken_p = 0;
  EXTRACT_FMT_12_BL8_VARS /* f-op1 f-r1 f-disp8 */
  EXTRACT_FMT_12_BL8_CODE

do {
  CPU (h_gr[14]) = ADDSI (ANDSI (OPRND (pc), -4), 4);
  TRACE_RESULT (current_cpu, "h-gr-14", 'x', CPU (h_gr[14]));
  BRANCH_NEW_PC (current_cpu, new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, OPRND (disp8)));
} while (0);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform bl24: bl $disp24.  */
CIA
SEM_FN_NAME (m32rx,bl24) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_13_bl24.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 4;
  int taken_p = 0;
  EXTRACT_FMT_13_BL24_VARS /* f-op1 f-r1 f-disp24 */
  EXTRACT_FMT_13_BL24_CODE

do {
  CPU (h_gr[14]) = ADDSI (OPRND (pc), 4);
  TRACE_RESULT (current_cpu, "h-gr-14", 'x', CPU (h_gr[14]));
  BRANCH_NEW_PC (current_cpu, new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, OPRND (disp24)));
} while (0);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform bcl8: bcl $disp8.  */
CIA
SEM_FN_NAME (m32rx,bcl8) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_14_bcl8.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  int taken_p = 0;
  EXTRACT_FMT_14_BCL8_VARS /* f-op1 f-r1 f-disp8 */
  EXTRACT_FMT_14_BCL8_CODE

if (OPRND (condbit)) {
do {
  CPU (h_gr[14]) = ADDSI (ANDSI (OPRND (pc), -4), 4);
  TRACE_RESULT (current_cpu, "h-gr-14", 'x', CPU (h_gr[14]));
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
}

/* Perform bcl24: bcl $disp24.  */
CIA
SEM_FN_NAME (m32rx,bcl24) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_15_bcl24.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 4;
  int taken_p = 0;
  EXTRACT_FMT_15_BCL24_VARS /* f-op1 f-r1 f-disp24 */
  EXTRACT_FMT_15_BCL24_CODE

if (OPRND (condbit)) {
do {
  CPU (h_gr[14]) = ADDSI (ANDSI (OPRND (pc), -4), 4);
  TRACE_RESULT (current_cpu, "h-gr-14", 'x', CPU (h_gr[14]));
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
}

/* Perform bnc8: bnc $disp8.  */
CIA
SEM_FN_NAME (m32rx,bnc8) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_8_bc8.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  int taken_p = 0;
  EXTRACT_FMT_8_BC8_VARS /* f-op1 f-r1 f-disp8 */
  EXTRACT_FMT_8_BC8_CODE

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
}

/* Perform bnc24: bnc $disp24.  */
CIA
SEM_FN_NAME (m32rx,bnc24) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_9_bc24.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 4;
  int taken_p = 0;
  EXTRACT_FMT_9_BC24_VARS /* f-op1 f-r1 f-disp24 */
  EXTRACT_FMT_9_BC24_CODE

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
}

/* Perform bne: bne $src1,$src2,$disp16.  */
CIA
SEM_FN_NAME (m32rx,bne) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_10_beq.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 4;
  int taken_p = 0;
  EXTRACT_FMT_10_BEQ_VARS /* f-op1 f-r1 f-op2 f-r2 f-disp16 */
  EXTRACT_FMT_10_BEQ_CODE

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
}

/* Perform bra8: bra $disp8.  */
CIA
SEM_FN_NAME (m32rx,bra8) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_16_bra8.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  int taken_p = 0;
  EXTRACT_FMT_16_BRA8_VARS /* f-op1 f-r1 f-disp8 */
  EXTRACT_FMT_16_BRA8_CODE

  BRANCH_NEW_PC (current_cpu, new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, OPRND (disp8)));

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform bra24: bra $disp24.  */
CIA
SEM_FN_NAME (m32rx,bra24) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_17_bra24.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 4;
  int taken_p = 0;
  EXTRACT_FMT_17_BRA24_VARS /* f-op1 f-r1 f-disp24 */
  EXTRACT_FMT_17_BRA24_CODE

  BRANCH_NEW_PC (current_cpu, new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, OPRND (disp24)));

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform bncl8: bncl $disp8.  */
CIA
SEM_FN_NAME (m32rx,bncl8) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_14_bcl8.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  int taken_p = 0;
  EXTRACT_FMT_14_BCL8_VARS /* f-op1 f-r1 f-disp8 */
  EXTRACT_FMT_14_BCL8_CODE

if (NOTBI (OPRND (condbit))) {
do {
  CPU (h_gr[14]) = ADDSI (ANDSI (OPRND (pc), -4), 4);
  TRACE_RESULT (current_cpu, "h-gr-14", 'x', CPU (h_gr[14]));
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
}

/* Perform bncl24: bncl $disp24.  */
CIA
SEM_FN_NAME (m32rx,bncl24) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_15_bcl24.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 4;
  int taken_p = 0;
  EXTRACT_FMT_15_BCL24_VARS /* f-op1 f-r1 f-disp24 */
  EXTRACT_FMT_15_BCL24_CODE

if (NOTBI (OPRND (condbit))) {
do {
  CPU (h_gr[14]) = ADDSI (ANDSI (OPRND (pc), -4), 4);
  TRACE_RESULT (current_cpu, "h-gr-14", 'x', CPU (h_gr[14]));
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
}

/* Perform cmp: cmp $src1,$src2.  */
CIA
SEM_FN_NAME (m32rx,cmp) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_18_cmp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_18_CMP_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_18_CMP_CODE

  CPU (h_cond) = LTSI (OPRND (src1), OPRND (src2));
  TRACE_RESULT (current_cpu, "condbit", 'x', CPU (h_cond));

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform cmpi: cmpi $src2,#$simm16.  */
CIA
SEM_FN_NAME (m32rx,cmpi) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_19_cmpi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 4;
  EXTRACT_FMT_19_CMPI_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_19_CMPI_CODE

  CPU (h_cond) = LTSI (OPRND (src2), OPRND (simm16));
  TRACE_RESULT (current_cpu, "condbit", 'x', CPU (h_cond));

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform cmpu: cmpu $src1,$src2.  */
CIA
SEM_FN_NAME (m32rx,cmpu) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_18_cmp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_18_CMP_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_18_CMP_CODE

  CPU (h_cond) = LTUSI (OPRND (src1), OPRND (src2));
  TRACE_RESULT (current_cpu, "condbit", 'x', CPU (h_cond));

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform cmpui: cmpui $src2,#$uimm16.  */
CIA
SEM_FN_NAME (m32rx,cmpui) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_20_cmpui.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 4;
  EXTRACT_FMT_20_CMPUI_VARS /* f-op1 f-r1 f-op2 f-r2 f-uimm16 */
  EXTRACT_FMT_20_CMPUI_CODE

  CPU (h_cond) = LTUSI (OPRND (src2), OPRND (uimm16));
  TRACE_RESULT (current_cpu, "condbit", 'x', CPU (h_cond));

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform cmpeq: cmpeq $src1,$src2.  */
CIA
SEM_FN_NAME (m32rx,cmpeq) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_18_cmp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_18_CMP_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_18_CMP_CODE

  CPU (h_cond) = EQSI (OPRND (src1), OPRND (src2));
  TRACE_RESULT (current_cpu, "condbit", 'x', CPU (h_cond));

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform cmpz: cmpz $src2.  */
CIA
SEM_FN_NAME (m32rx,cmpz) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_21_cmpz.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_21_CMPZ_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_21_CMPZ_CODE

  CPU (h_cond) = EQSI (OPRND (src2), 0);
  TRACE_RESULT (current_cpu, "condbit", 'x', CPU (h_cond));

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform div: div $dr,$sr.  */
CIA
SEM_FN_NAME (m32rx,div) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_22_div.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 4;
  EXTRACT_FMT_22_DIV_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_22_DIV_CODE

if (NESI (OPRND (sr), 0)) {
  CPU (h_gr[f_r1]) = DIVSI (OPRND (dr), OPRND (sr));
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));
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
}

/* Perform divu: divu $dr,$sr.  */
CIA
SEM_FN_NAME (m32rx,divu) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_22_div.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 4;
  EXTRACT_FMT_22_DIV_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_22_DIV_CODE

if (NESI (OPRND (sr), 0)) {
  CPU (h_gr[f_r1]) = UDIVSI (OPRND (dr), OPRND (sr));
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));
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
}

/* Perform rem: rem $dr,$sr.  */
CIA
SEM_FN_NAME (m32rx,rem) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_22_div.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 4;
  EXTRACT_FMT_22_DIV_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_22_DIV_CODE

if (NESI (OPRND (sr), 0)) {
  CPU (h_gr[f_r1]) = MODSI (OPRND (dr), OPRND (sr));
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));
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
}

/* Perform remu: remu $dr,$sr.  */
CIA
SEM_FN_NAME (m32rx,remu) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_22_div.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 4;
  EXTRACT_FMT_22_DIV_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_22_DIV_CODE

if (NESI (OPRND (sr), 0)) {
  CPU (h_gr[f_r1]) = UMODSI (OPRND (dr), OPRND (sr));
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));
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
}

/* Perform divh: divh $dr,$sr.  */
CIA
SEM_FN_NAME (m32rx,divh) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_22_div.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 4;
  EXTRACT_FMT_22_DIV_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_22_DIV_CODE

if (NESI (OPRND (sr), 0)) {
  CPU (h_gr[f_r1]) = DIVSI (EXTHISI (TRUNCSIHI (OPRND (dr))), OPRND (sr));
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));
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
}

/* Perform jc: jc $sr.  */
CIA
SEM_FN_NAME (m32rx,jc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_23_jc.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  int taken_p = 0;
  EXTRACT_FMT_23_JC_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_23_JC_CODE

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
}

/* Perform jnc: jnc $sr.  */
CIA
SEM_FN_NAME (m32rx,jnc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_23_jc.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  int taken_p = 0;
  EXTRACT_FMT_23_JC_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_23_JC_CODE

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
}

/* Perform jl: jl $sr.  */
CIA
SEM_FN_NAME (m32rx,jl) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_24_jl.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  int taken_p = 0;
  EXTRACT_FMT_24_JL_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_24_JL_CODE

do {
  SI temp1;SI temp0;
  temp0 = ADDSI (ANDSI (OPRND (pc), -4), 4);
  temp1 = OPRND (sr);
  CPU (h_gr[14]) = temp0;
  TRACE_RESULT (current_cpu, "h-gr-14", 'x', CPU (h_gr[14]));
  BRANCH_NEW_PC (current_cpu, new_pc, SEM_BRANCH_VIA_ADDR (sem_arg, temp1));
} while (0);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform jmp: jmp $sr.  */
CIA
SEM_FN_NAME (m32rx,jmp) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_25_jmp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  int taken_p = 0;
  EXTRACT_FMT_25_JMP_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_25_JMP_CODE

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
}

/* Perform ld: ld $dr,@$sr.  */
CIA
SEM_FN_NAME (m32rx,ld) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_26_ld.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_26_LD_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_26_LD_CODE

  CPU (h_gr[f_r1]) = OPRND (h_memory_sr);
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));

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
}

/* Perform ld-d: ld $dr,@($slo16,$sr).  */
CIA
SEM_FN_NAME (m32rx,ld_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_27_ld_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 4;
  EXTRACT_FMT_27_LD_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_27_LD_D_CODE

  CPU (h_gr[f_r1]) = OPRND (h_memory_add_WI_sr_slo16);
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));

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
}

/* Perform ldb: ldb $dr,@$sr.  */
CIA
SEM_FN_NAME (m32rx,ldb) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_28_ldb.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_28_LDB_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_28_LDB_CODE

  CPU (h_gr[f_r1]) = EXTQISI (OPRND (h_memory_sr));
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));

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
}

/* Perform ldb-d: ldb $dr,@($slo16,$sr).  */
CIA
SEM_FN_NAME (m32rx,ldb_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_29_ldb_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 4;
  EXTRACT_FMT_29_LDB_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_29_LDB_D_CODE

  CPU (h_gr[f_r1]) = EXTQISI (OPRND (h_memory_add_WI_sr_slo16));
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));

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
}

/* Perform ldh: ldh $dr,@$sr.  */
CIA
SEM_FN_NAME (m32rx,ldh) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_30_ldh.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_30_LDH_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_30_LDH_CODE

  CPU (h_gr[f_r1]) = EXTHISI (OPRND (h_memory_sr));
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));

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
}

/* Perform ldh-d: ldh $dr,@($slo16,$sr).  */
CIA
SEM_FN_NAME (m32rx,ldh_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_31_ldh_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 4;
  EXTRACT_FMT_31_LDH_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_31_LDH_D_CODE

  CPU (h_gr[f_r1]) = EXTHISI (OPRND (h_memory_add_WI_sr_slo16));
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));

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
}

/* Perform ldub: ldub $dr,@$sr.  */
CIA
SEM_FN_NAME (m32rx,ldub) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_28_ldb.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_28_LDB_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_28_LDB_CODE

  CPU (h_gr[f_r1]) = ZEXTQISI (OPRND (h_memory_sr));
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));

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
}

/* Perform ldub-d: ldub $dr,@($slo16,$sr).  */
CIA
SEM_FN_NAME (m32rx,ldub_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_29_ldb_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 4;
  EXTRACT_FMT_29_LDB_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_29_LDB_D_CODE

  CPU (h_gr[f_r1]) = ZEXTQISI (OPRND (h_memory_add_WI_sr_slo16));
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));

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
}

/* Perform lduh: lduh $dr,@$sr.  */
CIA
SEM_FN_NAME (m32rx,lduh) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_30_ldh.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_30_LDH_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_30_LDH_CODE

  CPU (h_gr[f_r1]) = ZEXTHISI (OPRND (h_memory_sr));
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));

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
}

/* Perform lduh-d: lduh $dr,@($slo16,$sr).  */
CIA
SEM_FN_NAME (m32rx,lduh_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_31_ldh_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 4;
  EXTRACT_FMT_31_LDH_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_31_LDH_D_CODE

  CPU (h_gr[f_r1]) = ZEXTHISI (OPRND (h_memory_add_WI_sr_slo16));
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));

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
}

/* Perform ld-plus: ld $dr,@$sr+.  */
CIA
SEM_FN_NAME (m32rx,ld_plus) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_32_ld_plus.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_32_LD_PLUS_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_32_LD_PLUS_CODE

do {
  SI temp1;SI temp0;
  temp0 = OPRND (h_memory_sr);
  temp1 = ADDSI (OPRND (sr), 4);
  CPU (h_gr[f_r1]) = temp0;
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));
  CPU (h_gr[f_r2]) = temp1;
  TRACE_RESULT (current_cpu, "sr", 'x', CPU (h_gr[f_r2]));
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
}

/* Perform ld24: ld24 $dr,#$uimm24.  */
CIA
SEM_FN_NAME (m32rx,ld24) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_33_ld24.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 4;
  EXTRACT_FMT_33_LD24_VARS /* f-op1 f-r1 f-uimm24 */
  EXTRACT_FMT_33_LD24_CODE

  CPU (h_gr[f_r1]) = OPRND (uimm24);
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform ldi8: ldi $dr,#$simm8.  */
CIA
SEM_FN_NAME (m32rx,ldi8) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_34_ldi8.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_34_LDI8_VARS /* f-op1 f-r1 f-simm8 */
  EXTRACT_FMT_34_LDI8_CODE

  CPU (h_gr[f_r1]) = OPRND (simm8);
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform ldi16: ldi $dr,$slo16.  */
CIA
SEM_FN_NAME (m32rx,ldi16) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_35_ldi16.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 4;
  EXTRACT_FMT_35_LDI16_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_35_LDI16_CODE

  CPU (h_gr[f_r1]) = OPRND (slo16);
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform lock: lock $dr,@$sr.  */
CIA
SEM_FN_NAME (m32rx,lock) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_36_lock.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_36_LOCK_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_36_LOCK_CODE

do_lock (current_cpu, OPRND (dr), OPRND (sr));

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform machi-a: machi $src1,$src2,$acc.  */
CIA
SEM_FN_NAME (m32rx,machi_a) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_37_machi_a.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_37_MACHI_A_VARS /* f-op1 f-r1 f-acc f-op23 f-r2 */
  EXTRACT_FMT_37_MACHI_A_CODE

m32rx_h_accums_set (current_cpu, f_acc, SRADI (SLLDI (ADDDI (OPRND (acc), MULDI (EXTSIDI (ANDSI (OPRND (src1), 0xffff0000)), EXTHIDI (TRUNCSIHI (SRASI (OPRND (src2), 16))))), 8), 8));
  TRACE_RESULT (current_cpu, "acc", 'D', m32rx_h_accums_get (current_cpu, f_acc));

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform maclo-a: maclo $src1,$src2,$acc.  */
CIA
SEM_FN_NAME (m32rx,maclo_a) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_37_machi_a.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_37_MACHI_A_VARS /* f-op1 f-r1 f-acc f-op23 f-r2 */
  EXTRACT_FMT_37_MACHI_A_CODE

m32rx_h_accums_set (current_cpu, f_acc, SRADI (SLLDI (ADDDI (OPRND (acc), MULDI (EXTSIDI (SLLSI (OPRND (src1), 16)), EXTHIDI (TRUNCSIHI (OPRND (src2))))), 8), 8));
  TRACE_RESULT (current_cpu, "acc", 'D', m32rx_h_accums_get (current_cpu, f_acc));

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform mul: mul $dr,$sr.  */
CIA
SEM_FN_NAME (m32rx,mul) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_0_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_0_ADD_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_0_ADD_CODE

  CPU (h_gr[f_r1]) = MULSI (OPRND (dr), OPRND (sr));
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));

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
}

/* Perform mulhi-a: mulhi $src1,$src2,$acc.  */
CIA
SEM_FN_NAME (m32rx,mulhi_a) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_38_mulhi_a.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_38_MULHI_A_VARS /* f-op1 f-r1 f-acc f-op23 f-r2 */
  EXTRACT_FMT_38_MULHI_A_CODE

m32rx_h_accums_set (current_cpu, f_acc, SRADI (SLLDI (MULDI (EXTSIDI (ANDSI (OPRND (src1), 0xffff0000)), EXTHIDI (TRUNCSIHI (SRASI (OPRND (src2), 16)))), 16), 16));
  TRACE_RESULT (current_cpu, "acc", 'D', m32rx_h_accums_get (current_cpu, f_acc));

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform mullo-a: mullo $src1,$src2,$acc.  */
CIA
SEM_FN_NAME (m32rx,mullo_a) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_38_mulhi_a.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_38_MULHI_A_VARS /* f-op1 f-r1 f-acc f-op23 f-r2 */
  EXTRACT_FMT_38_MULHI_A_CODE

m32rx_h_accums_set (current_cpu, f_acc, SRADI (SLLDI (MULDI (EXTSIDI (SLLSI (OPRND (src1), 16)), EXTHIDI (TRUNCSIHI (OPRND (src2)))), 16), 16));
  TRACE_RESULT (current_cpu, "acc", 'D', m32rx_h_accums_get (current_cpu, f_acc));

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform mv: mv $dr,$sr.  */
CIA
SEM_FN_NAME (m32rx,mv) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_39_mv.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_39_MV_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_39_MV_CODE

  CPU (h_gr[f_r1]) = OPRND (sr);
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));

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
}

/* Perform mvfachi-a: mvfachi $dr,$accs.  */
CIA
SEM_FN_NAME (m32rx,mvfachi_a) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_40_mvfachi_a.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_40_MVFACHI_A_VARS /* f-op1 f-r1 f-op2 f-accs f-op3 */
  EXTRACT_FMT_40_MVFACHI_A_CODE

  CPU (h_gr[f_r1]) = TRUNCDISI (SRADI (OPRND (accs), 32));
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform mvfaclo-a: mvfaclo $dr,$accs.  */
CIA
SEM_FN_NAME (m32rx,mvfaclo_a) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_40_mvfachi_a.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_40_MVFACHI_A_VARS /* f-op1 f-r1 f-op2 f-accs f-op3 */
  EXTRACT_FMT_40_MVFACHI_A_CODE

  CPU (h_gr[f_r1]) = TRUNCDISI (OPRND (accs));
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform mvfacmi-a: mvfacmi $dr,$accs.  */
CIA
SEM_FN_NAME (m32rx,mvfacmi_a) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_40_mvfachi_a.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_40_MVFACHI_A_VARS /* f-op1 f-r1 f-op2 f-accs f-op3 */
  EXTRACT_FMT_40_MVFACHI_A_CODE

  CPU (h_gr[f_r1]) = TRUNCDISI (SRADI (OPRND (accs), 16));
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform mvfc: mvfc $dr,$scr.  */
CIA
SEM_FN_NAME (m32rx,mvfc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_41_mvfc.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_41_MVFC_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_41_MVFC_CODE

  CPU (h_gr[f_r1]) = OPRND (scr);
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform mvtachi-a: mvtachi $src1,$accs.  */
CIA
SEM_FN_NAME (m32rx,mvtachi_a) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_42_mvtachi_a.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_42_MVTACHI_A_VARS /* f-op1 f-r1 f-op2 f-accs f-op3 */
  EXTRACT_FMT_42_MVTACHI_A_CODE

m32rx_h_accums_set (current_cpu, f_accs, ORDI (ANDDI (OPRND (accs), MAKEDI (0, 0xffffffff)), SLLDI (EXTSIDI (OPRND (src1)), 32)));
  TRACE_RESULT (current_cpu, "accs", 'D', m32rx_h_accums_get (current_cpu, f_accs));

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform mvtaclo-a: mvtaclo $src1,$accs.  */
CIA
SEM_FN_NAME (m32rx,mvtaclo_a) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_42_mvtachi_a.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_42_MVTACHI_A_VARS /* f-op1 f-r1 f-op2 f-accs f-op3 */
  EXTRACT_FMT_42_MVTACHI_A_CODE

m32rx_h_accums_set (current_cpu, f_accs, ORDI (ANDDI (OPRND (accs), MAKEDI (0xffffffff, 0)), ZEXTSIDI (OPRND (src1))));
  TRACE_RESULT (current_cpu, "accs", 'D', m32rx_h_accums_get (current_cpu, f_accs));

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform mvtc: mvtc $sr,$dcr.  */
CIA
SEM_FN_NAME (m32rx,mvtc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_43_mvtc.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_43_MVTC_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_43_MVTC_CODE

m32rx_h_cr_set (current_cpu, f_r1, OPRND (sr));
  TRACE_RESULT (current_cpu, "dcr", 'x', m32rx_h_cr_get (current_cpu, f_r1));

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform neg: neg $dr,$sr.  */
CIA
SEM_FN_NAME (m32rx,neg) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_39_mv.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_39_MV_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_39_MV_CODE

  CPU (h_gr[f_r1]) = NEGSI (OPRND (sr));
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));

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
}

/* Perform nop: nop.  */
CIA
SEM_FN_NAME (m32rx,nop) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_44_nop.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_44_NOP_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_44_NOP_CODE

PROFILE_COUNT_FILLNOPS (current_cpu, abuf->addr);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform not: not $dr,$sr.  */
CIA
SEM_FN_NAME (m32rx,not) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_39_mv.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_39_MV_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_39_MV_CODE

  CPU (h_gr[f_r1]) = INVSI (OPRND (sr));
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));

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
}

/* Perform rac-d: rac $accd.  */
CIA
SEM_FN_NAME (m32rx,rac_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_45_rac_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_45_RAC_D_VARS /* f-op1 f-accd f-bits67 f-op2 f-accs f-bit14 f-imm1 */
  EXTRACT_FMT_45_RAC_D_CODE

do {
  DI tmp_tmp1;
  tmp_tmp1 = SLLDI (OPRND (accum), 1);
  tmp_tmp1 = ADDDI (tmp_tmp1, MAKEDI (0, 32768));
m32rx_h_accums_set (current_cpu, f_accd, (GTDI (tmp_tmp1, MAKEDI (32767, 0xffff0000))) ? (MAKEDI (32767, 0xffff0000)) : (LTDI (tmp_tmp1, MAKEDI (0xffff8000, 0))) ? (MAKEDI (0xffff8000, 0)) : (ANDDI (tmp_tmp1, MAKEDI (0xffffffff, 0xffff0000))));
  TRACE_RESULT (current_cpu, "accd", 'D', m32rx_h_accums_get (current_cpu, f_accd));
} while (0);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform rac-ds: rac $accd,$accs.  */
CIA
SEM_FN_NAME (m32rx,rac_ds) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_46_rac_ds.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_46_RAC_DS_VARS /* f-op1 f-accd f-bits67 f-op2 f-accs f-bit14 f-imm1 */
  EXTRACT_FMT_46_RAC_DS_CODE

do {
  DI tmp_tmp1;
  tmp_tmp1 = SLLDI (OPRND (accs), 1);
  tmp_tmp1 = ADDDI (tmp_tmp1, MAKEDI (0, 32768));
m32rx_h_accums_set (current_cpu, f_accd, (GTDI (tmp_tmp1, MAKEDI (32767, 0xffff0000))) ? (MAKEDI (32767, 0xffff0000)) : (LTDI (tmp_tmp1, MAKEDI (0xffff8000, 0))) ? (MAKEDI (0xffff8000, 0)) : (ANDDI (tmp_tmp1, MAKEDI (0xffffffff, 0xffff0000))));
  TRACE_RESULT (current_cpu, "accd", 'D', m32rx_h_accums_get (current_cpu, f_accd));
} while (0);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform rac-dsi: rac $accd,$accs,#$imm1.  */
CIA
SEM_FN_NAME (m32rx,rac_dsi) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_47_rac_dsi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_47_RAC_DSI_VARS /* f-op1 f-accd f-bits67 f-op2 f-accs f-bit14 f-imm1 */
  EXTRACT_FMT_47_RAC_DSI_CODE

do {
  DI tmp_tmp1;
  tmp_tmp1 = SLLDI (OPRND (accs), OPRND (imm1));
  tmp_tmp1 = ADDDI (tmp_tmp1, MAKEDI (0, 32768));
m32rx_h_accums_set (current_cpu, f_accd, (GTDI (tmp_tmp1, MAKEDI (32767, 0xffff0000))) ? (MAKEDI (32767, 0xffff0000)) : (LTDI (tmp_tmp1, MAKEDI (0xffff8000, 0))) ? (MAKEDI (0xffff8000, 0)) : (ANDDI (tmp_tmp1, MAKEDI (0xffffffff, 0xffff0000))));
  TRACE_RESULT (current_cpu, "accd", 'D', m32rx_h_accums_get (current_cpu, f_accd));
} while (0);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform rach-d: rach $accd.  */
CIA
SEM_FN_NAME (m32rx,rach_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_45_rac_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_45_RAC_D_VARS /* f-op1 f-accd f-bits67 f-op2 f-accs f-bit14 f-imm1 */
  EXTRACT_FMT_45_RAC_D_CODE

do {
  DI tmp_tmp1;
  tmp_tmp1 = SLLDI (OPRND (accum), 1);
  tmp_tmp1 = ADDDI (tmp_tmp1, MAKEDI (0, 0x80000000));
m32rx_h_accums_set (current_cpu, f_accd, (GTDI (tmp_tmp1, MAKEDI (32767, 0))) ? (MAKEDI (32767, 0)) : (LTDI (tmp_tmp1, MAKEDI (0xffff8000, 0))) ? (MAKEDI (0xffff8000, 0)) : (ANDDI (tmp_tmp1, MAKEDI (0xffffffff, 0))));
  TRACE_RESULT (current_cpu, "accd", 'D', m32rx_h_accums_get (current_cpu, f_accd));
} while (0);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform rach-ds: rach $accd,$accs.  */
CIA
SEM_FN_NAME (m32rx,rach_ds) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_46_rac_ds.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_46_RAC_DS_VARS /* f-op1 f-accd f-bits67 f-op2 f-accs f-bit14 f-imm1 */
  EXTRACT_FMT_46_RAC_DS_CODE

do {
  DI tmp_tmp1;
  tmp_tmp1 = SLLDI (OPRND (accs), 1);
  tmp_tmp1 = ADDDI (tmp_tmp1, MAKEDI (0, 0x80000000));
m32rx_h_accums_set (current_cpu, f_accd, (GTDI (tmp_tmp1, MAKEDI (32767, 0))) ? (MAKEDI (32767, 0)) : (LTDI (tmp_tmp1, MAKEDI (0xffff8000, 0))) ? (MAKEDI (0xffff8000, 0)) : (ANDDI (tmp_tmp1, MAKEDI (0xffffffff, 0))));
  TRACE_RESULT (current_cpu, "accd", 'D', m32rx_h_accums_get (current_cpu, f_accd));
} while (0);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform rach-dsi: rach $accd,$accs,#$imm1.  */
CIA
SEM_FN_NAME (m32rx,rach_dsi) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_47_rac_dsi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_47_RAC_DSI_VARS /* f-op1 f-accd f-bits67 f-op2 f-accs f-bit14 f-imm1 */
  EXTRACT_FMT_47_RAC_DSI_CODE

do {
  DI tmp_tmp1;
  tmp_tmp1 = SLLDI (OPRND (accs), OPRND (imm1));
  tmp_tmp1 = ADDDI (tmp_tmp1, MAKEDI (0, 0x80000000));
m32rx_h_accums_set (current_cpu, f_accd, (GTDI (tmp_tmp1, MAKEDI (32767, 0))) ? (MAKEDI (32767, 0)) : (LTDI (tmp_tmp1, MAKEDI (0xffff8000, 0))) ? (MAKEDI (0xffff8000, 0)) : (ANDDI (tmp_tmp1, MAKEDI (0xffffffff, 0))));
  TRACE_RESULT (current_cpu, "accd", 'D', m32rx_h_accums_get (current_cpu, f_accd));
} while (0);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform rte: rte.  */
CIA
SEM_FN_NAME (m32rx,rte) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_48_rte.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  int taken_p = 0;
  EXTRACT_FMT_48_RTE_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_48_RTE_CODE

do {
  CPU (h_sm) = OPRND (h_bsm_0);
  TRACE_RESULT (current_cpu, "h-sm-0", 'x', CPU (h_sm));
  CPU (h_ie) = OPRND (h_bie_0);
  TRACE_RESULT (current_cpu, "h-ie-0", 'x', CPU (h_ie));
  CPU (h_cond) = OPRND (h_bcond_0);
  TRACE_RESULT (current_cpu, "condbit", 'x', CPU (h_cond));
  BRANCH_NEW_PC (current_cpu, new_pc, SEM_BRANCH_VIA_ADDR (sem_arg, OPRND (h_bpc_0)));
  TRACE_RESULT (current_cpu, "pc", 'x', CPU (h_pc));
} while (0);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform seth: seth $dr,#$hi16.  */
CIA
SEM_FN_NAME (m32rx,seth) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_49_seth.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 4;
  EXTRACT_FMT_49_SETH_VARS /* f-op1 f-r1 f-op2 f-r2 f-hi16 */
  EXTRACT_FMT_49_SETH_CODE

  CPU (h_gr[f_r1]) = SLLSI (OPRND (hi16), 16);
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform sll: sll $dr,$sr.  */
CIA
SEM_FN_NAME (m32rx,sll) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_0_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_0_ADD_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_0_ADD_CODE

  CPU (h_gr[f_r1]) = SLLSI (OPRND (dr), ANDSI (OPRND (sr), 31));
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));

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
}

/* Perform sll3: sll3 $dr,$sr,#$simm16.  */
CIA
SEM_FN_NAME (m32rx,sll3) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_50_sll3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 4;
  EXTRACT_FMT_50_SLL3_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_50_SLL3_CODE

  CPU (h_gr[f_r1]) = SLLSI (OPRND (sr), ANDSI (OPRND (simm16), 31));
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));

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
}

/* Perform slli: slli $dr,#$uimm5.  */
CIA
SEM_FN_NAME (m32rx,slli) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_51_slli.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_51_SLLI_VARS /* f-op1 f-r1 f-shift-op2 f-uimm5 */
  EXTRACT_FMT_51_SLLI_CODE

  CPU (h_gr[f_r1]) = SLLSI (OPRND (dr), OPRND (uimm5));
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));

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
}

/* Perform sra: sra $dr,$sr.  */
CIA
SEM_FN_NAME (m32rx,sra) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_0_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_0_ADD_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_0_ADD_CODE

  CPU (h_gr[f_r1]) = SRASI (OPRND (dr), ANDSI (OPRND (sr), 31));
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));

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
}

/* Perform sra3: sra3 $dr,$sr,#$simm16.  */
CIA
SEM_FN_NAME (m32rx,sra3) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_50_sll3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 4;
  EXTRACT_FMT_50_SLL3_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_50_SLL3_CODE

  CPU (h_gr[f_r1]) = SRASI (OPRND (sr), ANDSI (OPRND (simm16), 31));
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));

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
}

/* Perform srai: srai $dr,#$uimm5.  */
CIA
SEM_FN_NAME (m32rx,srai) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_51_slli.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_51_SLLI_VARS /* f-op1 f-r1 f-shift-op2 f-uimm5 */
  EXTRACT_FMT_51_SLLI_CODE

  CPU (h_gr[f_r1]) = SRASI (OPRND (dr), OPRND (uimm5));
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));

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
}

/* Perform srl: srl $dr,$sr.  */
CIA
SEM_FN_NAME (m32rx,srl) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_0_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_0_ADD_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_0_ADD_CODE

  CPU (h_gr[f_r1]) = SRLSI (OPRND (dr), ANDSI (OPRND (sr), 31));
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));

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
}

/* Perform srl3: srl3 $dr,$sr,#$simm16.  */
CIA
SEM_FN_NAME (m32rx,srl3) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_50_sll3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 4;
  EXTRACT_FMT_50_SLL3_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_50_SLL3_CODE

  CPU (h_gr[f_r1]) = SRLSI (OPRND (sr), ANDSI (OPRND (simm16), 31));
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));

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
}

/* Perform srli: srli $dr,#$uimm5.  */
CIA
SEM_FN_NAME (m32rx,srli) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_51_slli.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_51_SLLI_VARS /* f-op1 f-r1 f-shift-op2 f-uimm5 */
  EXTRACT_FMT_51_SLLI_CODE

  CPU (h_gr[f_r1]) = SRLSI (OPRND (dr), OPRND (uimm5));
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));

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
}

/* Perform st: st $src1,@$src2.  */
CIA
SEM_FN_NAME (m32rx,st) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_52_st.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_52_ST_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_52_ST_CODE

SETMEMSI (current_cpu, OPRND (src2), OPRND (src1));
  TRACE_RESULT (current_cpu, "h-memory-src2", 'x', GETMEMSI (current_cpu, OPRND (src2)));

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform st-d: st $src1,@($slo16,$src2).  */
CIA
SEM_FN_NAME (m32rx,st_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_53_st_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 4;
  EXTRACT_FMT_53_ST_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_53_ST_D_CODE

SETMEMSI (current_cpu, ADDSI (OPRND (src2), OPRND (slo16)), OPRND (src1));
  TRACE_RESULT (current_cpu, "h-memory-add-WI-src2-slo16", 'x', GETMEMSI (current_cpu, ADDSI (OPRND (src2), OPRND (slo16))));

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform stb: stb $src1,@$src2.  */
CIA
SEM_FN_NAME (m32rx,stb) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_54_stb.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_54_STB_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_54_STB_CODE

SETMEMQI (current_cpu, OPRND (src2), OPRND (src1));
  TRACE_RESULT (current_cpu, "h-memory-src2", 'x', GETMEMQI (current_cpu, OPRND (src2)));

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform stb-d: stb $src1,@($slo16,$src2).  */
CIA
SEM_FN_NAME (m32rx,stb_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_55_stb_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 4;
  EXTRACT_FMT_55_STB_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_55_STB_D_CODE

SETMEMQI (current_cpu, ADDSI (OPRND (src2), OPRND (slo16)), OPRND (src1));
  TRACE_RESULT (current_cpu, "h-memory-add-WI-src2-slo16", 'x', GETMEMQI (current_cpu, ADDSI (OPRND (src2), OPRND (slo16))));

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform sth: sth $src1,@$src2.  */
CIA
SEM_FN_NAME (m32rx,sth) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_56_sth.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_56_STH_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_56_STH_CODE

SETMEMHI (current_cpu, OPRND (src2), OPRND (src1));
  TRACE_RESULT (current_cpu, "h-memory-src2", 'x', GETMEMHI (current_cpu, OPRND (src2)));

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform sth-d: sth $src1,@($slo16,$src2).  */
CIA
SEM_FN_NAME (m32rx,sth_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_57_sth_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 4;
  EXTRACT_FMT_57_STH_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_57_STH_D_CODE

SETMEMHI (current_cpu, ADDSI (OPRND (src2), OPRND (slo16)), OPRND (src1));
  TRACE_RESULT (current_cpu, "h-memory-add-WI-src2-slo16", 'x', GETMEMHI (current_cpu, ADDSI (OPRND (src2), OPRND (slo16))));

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform st-plus: st $src1,@+$src2.  */
CIA
SEM_FN_NAME (m32rx,st_plus) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_58_st_plus.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_58_ST_PLUS_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_58_ST_PLUS_CODE

do {
  CPU (h_gr[f_r2]) = ADDSI (OPRND (src2), 4);
  TRACE_RESULT (current_cpu, "src2", 'x', CPU (h_gr[f_r2]));
SETMEMSI (current_cpu, OPRND (src2), OPRND (src1));
  TRACE_RESULT (current_cpu, "h-memory-src2", 'x', GETMEMSI (current_cpu, OPRND (src2)));
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
}

/* Perform st-minus: st $src1,@-$src2.  */
CIA
SEM_FN_NAME (m32rx,st_minus) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_58_st_plus.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_58_ST_PLUS_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_58_ST_PLUS_CODE

do {
  CPU (h_gr[f_r2]) = SUBSI (OPRND (src2), 4);
  TRACE_RESULT (current_cpu, "src2", 'x', CPU (h_gr[f_r2]));
SETMEMSI (current_cpu, OPRND (src2), OPRND (src1));
  TRACE_RESULT (current_cpu, "h-memory-src2", 'x', GETMEMSI (current_cpu, OPRND (src2)));
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
}

/* Perform sub: sub $dr,$sr.  */
CIA
SEM_FN_NAME (m32rx,sub) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_0_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_0_ADD_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_0_ADD_CODE

  CPU (h_gr[f_r1]) = SUBSI (OPRND (dr), OPRND (sr));
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));

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
}

/* Perform subv: subv $dr,$sr.  */
CIA
SEM_FN_NAME (m32rx,subv) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_5_addv.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_5_ADDV_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_5_ADDV_CODE

do {
  BI temp1;SI temp0;
  temp0 = SUBSI (OPRND (dr), OPRND (sr));
  temp1 = SUBOFSI (OPRND (dr), OPRND (sr), 0);
  CPU (h_gr[f_r1]) = temp0;
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));
  CPU (h_cond) = temp1;
  TRACE_RESULT (current_cpu, "condbit", 'x', CPU (h_cond));
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
}

/* Perform subx: subx $dr,$sr.  */
CIA
SEM_FN_NAME (m32rx,subx) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_7_addx.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_7_ADDX_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_7_ADDX_CODE

do {
  BI temp1;SI temp0;
  temp0 = SUBCSI (OPRND (dr), OPRND (sr), OPRND (condbit));
  temp1 = SUBCFSI (OPRND (dr), OPRND (sr), OPRND (condbit));
  CPU (h_gr[f_r1]) = temp0;
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));
  CPU (h_cond) = temp1;
  TRACE_RESULT (current_cpu, "condbit", 'x', CPU (h_cond));
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
}

/* Perform trap: trap #$uimm4.  */
CIA
SEM_FN_NAME (m32rx,trap) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_59_trap.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  int taken_p = 0;
  EXTRACT_FMT_59_TRAP_VARS /* f-op1 f-r1 f-op2 f-uimm4 */
  EXTRACT_FMT_59_TRAP_CODE

do_trap (current_cpu, OPRND (uimm4));

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform unlock: unlock $src1,@$src2.  */
CIA
SEM_FN_NAME (m32rx,unlock) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_60_unlock.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_60_UNLOCK_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_60_UNLOCK_CODE

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
}

/* Perform satb: satb $dr,$sr.  */
CIA
SEM_FN_NAME (m32rx,satb) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_61_satb.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 4;
  EXTRACT_FMT_61_SATB_VARS /* f-op1 f-r1 f-op2 f-r2 f-uimm16 */
  EXTRACT_FMT_61_SATB_CODE

  CPU (h_gr[f_r1]) = (GESI (OPRND (sr), 127)) ? (127) : (LESI (OPRND (sr), -128)) ? (-128) : (OPRND (sr));
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));

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
}

/* Perform sath: sath $dr,$sr.  */
CIA
SEM_FN_NAME (m32rx,sath) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_61_satb.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 4;
  EXTRACT_FMT_61_SATB_VARS /* f-op1 f-r1 f-op2 f-r2 f-uimm16 */
  EXTRACT_FMT_61_SATB_CODE

  CPU (h_gr[f_r1]) = (GESI (OPRND (sr), 32767)) ? (32767) : (LESI (OPRND (sr), -32768)) ? (-32768) : (OPRND (sr));
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));

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
}

/* Perform sat: sat $dr,$sr.  */
CIA
SEM_FN_NAME (m32rx,sat) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_62_sat.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 4;
  EXTRACT_FMT_62_SAT_VARS /* f-op1 f-r1 f-op2 f-r2 f-uimm16 */
  EXTRACT_FMT_62_SAT_CODE

  CPU (h_gr[f_r1]) = (OPRND (condbit)) ? ((LTSI (OPRND (sr), 0)) ? (2147483647) : (0x80000000)) : (OPRND (sr));
  TRACE_RESULT (current_cpu, "dr", 'x', CPU (h_gr[f_r1]));

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
}

/* Perform pcmpbz: pcmpbz $src2.  */
CIA
SEM_FN_NAME (m32rx,pcmpbz) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_21_cmpz.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_21_CMPZ_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_21_CMPZ_CODE

  CPU (h_cond) = (EQSI (ANDSI (OPRND (src2), 255), 0)) ? (1) : (EQSI (ANDSI (OPRND (src2), 65280), 0)) ? (1) : (EQSI (ANDSI (OPRND (src2), 16711680), 0)) ? (1) : (EQSI (ANDSI (OPRND (src2), 0xff000000), 0)) ? (1) : (0);
  TRACE_RESULT (current_cpu, "condbit", 'x', CPU (h_cond));

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform sadd: sadd.  */
CIA
SEM_FN_NAME (m32rx,sadd) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_63_sadd.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_63_SADD_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_63_SADD_CODE

m32rx_h_accums_set (current_cpu, 0, ADDDI (SRADI (OPRND (h_accums_1), 16), OPRND (h_accums_0)));
  TRACE_RESULT (current_cpu, "h-accums-0", 'D', m32rx_h_accums_get (current_cpu, 0));

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform macwu1: macwu1 $src1,$src2.  */
CIA
SEM_FN_NAME (m32rx,macwu1) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_64_macwu1.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_64_MACWU1_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_64_MACWU1_CODE

m32rx_h_accums_set (current_cpu, 1, SRADI (SLLDI (ADDDI (OPRND (h_accums_1), MULDI (EXTSIDI (OPRND (src1)), EXTSIDI (ANDSI (OPRND (src2), 65535)))), 8), 8));
  TRACE_RESULT (current_cpu, "h-accums-1", 'D', m32rx_h_accums_get (current_cpu, 1));

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform msblo: msblo $src1,$src2.  */
CIA
SEM_FN_NAME (m32rx,msblo) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_65_msblo.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_65_MSBLO_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_65_MSBLO_CODE

  CPU (h_accum) = SRADI (SLLDI (SUBDI (OPRND (accum), SRADI (SLLDI (MULDI (EXTHIDI (TRUNCSIHI (OPRND (src1))), EXTHIDI (TRUNCSIHI (OPRND (src2)))), 32), 16)), 8), 8);
  TRACE_RESULT (current_cpu, "accum", 'D', CPU (h_accum));

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform mulwu1: mulwu1 $src1,$src2.  */
CIA
SEM_FN_NAME (m32rx,mulwu1) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_66_mulwu1.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_66_MULWU1_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_66_MULWU1_CODE

m32rx_h_accums_set (current_cpu, 1, SRADI (SLLDI (MULDI (EXTSIDI (OPRND (src1)), EXTSIDI (ANDSI (OPRND (src2), 65535))), 16), 16));
  TRACE_RESULT (current_cpu, "h-accums-1", 'D', m32rx_h_accums_get (current_cpu, 1));

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform maclh1: maclh1 $src1,$src2.  */
CIA
SEM_FN_NAME (m32rx,maclh1) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_64_macwu1.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_64_MACWU1_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_64_MACWU1_CODE

m32rx_h_accums_set (current_cpu, 1, SRADI (SLLDI (ADDDI (OPRND (h_accums_1), SRADI (SLLDI (MULDI (EXTSIDI (SRASI (OPRND (src1), 16)), EXTHIDI (TRUNCSIHI (OPRND (src2)))), 32), 16)), 8), 8));
  TRACE_RESULT (current_cpu, "h-accums-1", 'D', m32rx_h_accums_get (current_cpu, 1));

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform sc: sc.  */
CIA
SEM_FN_NAME (m32rx,sc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_67_sc.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_67_SC_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_67_SC_CODE

if (OPRND (condbit)) {
  CPU (h_abort) = 1;
  TRACE_RESULT (current_cpu, "abort-parallel-execution", 'x', CPU (h_abort));
}

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* Perform snc: snc.  */
CIA
SEM_FN_NAME (m32rx,snc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_67_sc.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = CPU (h_pc) + 2;
  EXTRACT_FMT_67_SC_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_67_SC_CODE

if (NOTBI (OPRND (condbit))) {
  CPU (h_abort) = 1;
  TRACE_RESULT (current_cpu, "abort-parallel-execution", 'x', CPU (h_abort));
}

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

CIA
SEM_FN_NAME (m32rx,illegal) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  sim_engine_illegal_insn (current_cpu, NULL_CIA /*FIXME*/);
  return 0;
}

#endif /* ! defined (SCACHE_P) || (defined (SCACHE_P) && WITH_SCACHE) */
