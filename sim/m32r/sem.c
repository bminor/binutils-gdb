/* Simulator instruction semantics for m32r.

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
#define WANT_CPU_M32R

#include "sim-main.h"
#include "cgen-mem.h"
#include "cgen-ops.h"
#include "cpu-sim.h"

#if WITH_SCACHE

#undef GET_ATTR
#define GET_ATTR(cpu, num, attr) CGEN_INSN_ATTR (abuf->idesc->opcode, CGEN_INSN_##attr)

/* add: add $dr,$sr.  */

CIA
SEM_FN_NAME (m32r,add) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

* FLD (f_r1) = ADDSI (* FLD (f_r1), * FLD (f_r2));
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* add3: add3 $dr,$sr,$hash$slo16.  */

CIA
SEM_FN_NAME (m32r,add3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_add3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

* FLD (f_r1) = ADDSI (* FLD (f_r2), FLD (f_simm16));
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* and: and $dr,$sr.  */

CIA
SEM_FN_NAME (m32r,and) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

* FLD (f_r1) = ANDSI (* FLD (f_r1), * FLD (f_r2));
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* and3: and3 $dr,$sr,$uimm16.  */

CIA
SEM_FN_NAME (m32r,and3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_and3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

* FLD (f_r1) = ANDSI (* FLD (f_r2), FLD (f_uimm16));
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* or: or $dr,$sr.  */

CIA
SEM_FN_NAME (m32r,or) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

* FLD (f_r1) = ORSI (* FLD (f_r1), * FLD (f_r2));
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* or3: or3 $dr,$sr,$hash$ulo16.  */

CIA
SEM_FN_NAME (m32r,or3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_or3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

* FLD (f_r1) = ORSI (* FLD (f_r2), FLD (f_uimm16));
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* xor: xor $dr,$sr.  */

CIA
SEM_FN_NAME (m32r,xor) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

* FLD (f_r1) = XORSI (* FLD (f_r1), * FLD (f_r2));
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* xor3: xor3 $dr,$sr,$uimm16.  */

CIA
SEM_FN_NAME (m32r,xor3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_and3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

* FLD (f_r1) = XORSI (* FLD (f_r2), FLD (f_uimm16));
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* addi: addi $dr,$simm8.  */

CIA
SEM_FN_NAME (m32r,addi) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_addi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

* FLD (f_r1) = ADDSI (* FLD (f_r1), FLD (f_simm8));
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* addv: addv $dr,$sr.  */

CIA
SEM_FN_NAME (m32r,addv) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_addv.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

do {
  BI temp1;SI temp0;
  temp0 = ADDSI (* FLD (f_r1), * FLD (f_r2));
  temp1 = ADDOFSI (* FLD (f_r1), * FLD (f_r2), 0);
* FLD (f_r1) = temp0;
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));
  CPU (h_cond) = temp1;
  TRACE_RESULT (current_cpu, "condbit", 'x', CPU (h_cond));
} while (0);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* addv3: addv3 $dr,$sr,$simm16.  */

CIA
SEM_FN_NAME (m32r,addv3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_addv3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

do {
  BI temp1;SI temp0;
  temp0 = ADDSI (* FLD (f_r2), FLD (f_simm16));
  temp1 = ADDOFSI (* FLD (f_r2), FLD (f_simm16), 0);
* FLD (f_r1) = temp0;
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));
  CPU (h_cond) = temp1;
  TRACE_RESULT (current_cpu, "condbit", 'x', CPU (h_cond));
} while (0);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* addx: addx $dr,$sr.  */

CIA
SEM_FN_NAME (m32r,addx) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_addx.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

do {
  BI temp1;SI temp0;
  temp0 = ADDCSI (* FLD (f_r1), * FLD (f_r2), CPU (h_cond));
  temp1 = ADDCFSI (* FLD (f_r1), * FLD (f_r2), CPU (h_cond));
* FLD (f_r1) = temp0;
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));
  CPU (h_cond) = temp1;
  TRACE_RESULT (current_cpu, "condbit", 'x', CPU (h_cond));
} while (0);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* bc8: bc.s $disp8.  */

CIA
SEM_FN_NAME (m32r,bc8) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_bc8.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  int taken_p = 0;

if (CPU (h_cond)) {
  BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, FLD (f_disp8)));
  taken_p = 1;
  TRACE_RESULT (current_cpu, "pc", 'x', new_pc);
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef FLD
}

/* bc24: bc.l $disp24.  */

CIA
SEM_FN_NAME (m32r,bc24) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_bc24.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  int taken_p = 0;

if (CPU (h_cond)) {
  BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, FLD (f_disp24)));
  taken_p = 1;
  TRACE_RESULT (current_cpu, "pc", 'x', new_pc);
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef FLD
}

/* beq: beq $src1,$src2,$disp16.  */

CIA
SEM_FN_NAME (m32r,beq) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_beq.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  int taken_p = 0;

if (EQSI (* FLD (f_r1), * FLD (f_r2))) {
  BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, FLD (f_disp16)));
  taken_p = 1;
  TRACE_RESULT (current_cpu, "pc", 'x', new_pc);
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef FLD
}

/* beqz: beqz $src2,$disp16.  */

CIA
SEM_FN_NAME (m32r,beqz) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_beqz.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  int taken_p = 0;

if (EQSI (* FLD (f_r2), 0)) {
  BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, FLD (f_disp16)));
  taken_p = 1;
  TRACE_RESULT (current_cpu, "pc", 'x', new_pc);
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef FLD
}

/* bgez: bgez $src2,$disp16.  */

CIA
SEM_FN_NAME (m32r,bgez) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_beqz.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  int taken_p = 0;

if (GESI (* FLD (f_r2), 0)) {
  BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, FLD (f_disp16)));
  taken_p = 1;
  TRACE_RESULT (current_cpu, "pc", 'x', new_pc);
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef FLD
}

/* bgtz: bgtz $src2,$disp16.  */

CIA
SEM_FN_NAME (m32r,bgtz) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_beqz.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  int taken_p = 0;

if (GTSI (* FLD (f_r2), 0)) {
  BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, FLD (f_disp16)));
  taken_p = 1;
  TRACE_RESULT (current_cpu, "pc", 'x', new_pc);
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef FLD
}

/* blez: blez $src2,$disp16.  */

CIA
SEM_FN_NAME (m32r,blez) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_beqz.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  int taken_p = 0;

if (LESI (* FLD (f_r2), 0)) {
  BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, FLD (f_disp16)));
  taken_p = 1;
  TRACE_RESULT (current_cpu, "pc", 'x', new_pc);
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef FLD
}

/* bltz: bltz $src2,$disp16.  */

CIA
SEM_FN_NAME (m32r,bltz) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_beqz.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  int taken_p = 0;

if (LTSI (* FLD (f_r2), 0)) {
  BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, FLD (f_disp16)));
  taken_p = 1;
  TRACE_RESULT (current_cpu, "pc", 'x', new_pc);
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef FLD
}

/* bnez: bnez $src2,$disp16.  */

CIA
SEM_FN_NAME (m32r,bnez) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_beqz.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  int taken_p = 0;

if (NESI (* FLD (f_r2), 0)) {
  BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, FLD (f_disp16)));
  taken_p = 1;
  TRACE_RESULT (current_cpu, "pc", 'x', new_pc);
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef FLD
}

/* bl8: bl.s $disp8.  */

CIA
SEM_FN_NAME (m32r,bl8) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_bl8.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  int taken_p = 0;

do {
  CPU (h_gr[14]) = ADDSI (ANDSI (CPU (h_pc), -4), 4);
  TRACE_RESULT (current_cpu, "gr-14", 'x', CPU (h_gr[14]));
  BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, FLD (f_disp8)));
  taken_p = 1;
  TRACE_RESULT (current_cpu, "pc", 'x', new_pc);
} while (0);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef FLD
}

/* bl24: bl.l $disp24.  */

CIA
SEM_FN_NAME (m32r,bl24) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_bl24.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  int taken_p = 0;

do {
  CPU (h_gr[14]) = ADDSI (CPU (h_pc), 4);
  TRACE_RESULT (current_cpu, "gr-14", 'x', CPU (h_gr[14]));
  BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, FLD (f_disp24)));
  taken_p = 1;
  TRACE_RESULT (current_cpu, "pc", 'x', new_pc);
} while (0);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef FLD
}

/* bnc8: bnc.s $disp8.  */

CIA
SEM_FN_NAME (m32r,bnc8) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_bc8.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  int taken_p = 0;

if (NOTBI (CPU (h_cond))) {
  BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, FLD (f_disp8)));
  taken_p = 1;
  TRACE_RESULT (current_cpu, "pc", 'x', new_pc);
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef FLD
}

/* bnc24: bnc.l $disp24.  */

CIA
SEM_FN_NAME (m32r,bnc24) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_bc24.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  int taken_p = 0;

if (NOTBI (CPU (h_cond))) {
  BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, FLD (f_disp24)));
  taken_p = 1;
  TRACE_RESULT (current_cpu, "pc", 'x', new_pc);
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef FLD
}

/* bne: bne $src1,$src2,$disp16.  */

CIA
SEM_FN_NAME (m32r,bne) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_beq.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  int taken_p = 0;

if (NESI (* FLD (f_r1), * FLD (f_r2))) {
  BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, FLD (f_disp16)));
  taken_p = 1;
  TRACE_RESULT (current_cpu, "pc", 'x', new_pc);
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef FLD
}

/* bra8: bra.s $disp8.  */

CIA
SEM_FN_NAME (m32r,bra8) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_bra8.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  int taken_p = 0;

  BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, FLD (f_disp8)));
  taken_p = 1;
  TRACE_RESULT (current_cpu, "pc", 'x', new_pc);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef FLD
}

/* bra24: bra.l $disp24.  */

CIA
SEM_FN_NAME (m32r,bra24) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_bra24.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  int taken_p = 0;

  BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, FLD (f_disp24)));
  taken_p = 1;
  TRACE_RESULT (current_cpu, "pc", 'x', new_pc);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef FLD
}

/* cmp: cmp $src1,$src2.  */

CIA
SEM_FN_NAME (m32r,cmp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_cmp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

  CPU (h_cond) = LTSI (* FLD (f_r1), * FLD (f_r2));
  TRACE_RESULT (current_cpu, "condbit", 'x', CPU (h_cond));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* cmpi: cmpi $src2,$simm16.  */

CIA
SEM_FN_NAME (m32r,cmpi) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_cmpi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

  CPU (h_cond) = LTSI (* FLD (f_r2), FLD (f_simm16));
  TRACE_RESULT (current_cpu, "condbit", 'x', CPU (h_cond));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* cmpu: cmpu $src1,$src2.  */

CIA
SEM_FN_NAME (m32r,cmpu) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_cmp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

  CPU (h_cond) = LTUSI (* FLD (f_r1), * FLD (f_r2));
  TRACE_RESULT (current_cpu, "condbit", 'x', CPU (h_cond));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* cmpui: cmpui $src2,$simm16.  */

CIA
SEM_FN_NAME (m32r,cmpui) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_cmpi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

  CPU (h_cond) = LTUSI (* FLD (f_r2), FLD (f_simm16));
  TRACE_RESULT (current_cpu, "condbit", 'x', CPU (h_cond));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* div: div $dr,$sr.  */

CIA
SEM_FN_NAME (m32r,div) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_div.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

if (NESI (* FLD (f_r2), 0)) {
* FLD (f_r1) = DIVSI (* FLD (f_r1), * FLD (f_r2));
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* divu: divu $dr,$sr.  */

CIA
SEM_FN_NAME (m32r,divu) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_div.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

if (NESI (* FLD (f_r2), 0)) {
* FLD (f_r1) = UDIVSI (* FLD (f_r1), * FLD (f_r2));
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* rem: rem $dr,$sr.  */

CIA
SEM_FN_NAME (m32r,rem) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_div.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

if (NESI (* FLD (f_r2), 0)) {
* FLD (f_r1) = MODSI (* FLD (f_r1), * FLD (f_r2));
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* remu: remu $dr,$sr.  */

CIA
SEM_FN_NAME (m32r,remu) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_div.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

if (NESI (* FLD (f_r2), 0)) {
* FLD (f_r1) = UMODSI (* FLD (f_r1), * FLD (f_r2));
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* jl: jl $sr.  */

CIA
SEM_FN_NAME (m32r,jl) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_jl.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  int taken_p = 0;

do {
  SI temp1;SI temp0;
  temp0 = ADDSI (ANDSI (CPU (h_pc), -4), 4);
  temp1 = * FLD (f_r2);
  CPU (h_gr[14]) = temp0;
  TRACE_RESULT (current_cpu, "gr-14", 'x', CPU (h_gr[14]));
  BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_ADDR (sem_arg, temp1));
  taken_p = 1;
  TRACE_RESULT (current_cpu, "pc", 'x', new_pc);
} while (0);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef FLD
}

/* jmp: jmp $sr.  */

CIA
SEM_FN_NAME (m32r,jmp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_jmp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  int taken_p = 0;

  BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_ADDR (sem_arg, * FLD (f_r2)));
  taken_p = 1;
  TRACE_RESULT (current_cpu, "pc", 'x', new_pc);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef FLD
}

/* ld: ld $dr,@$sr.  */

CIA
SEM_FN_NAME (m32r,ld) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ld.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

* FLD (f_r1) = GETMEMSI (current_cpu, * FLD (f_r2));
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* ld-d: ld $dr,@($slo16,$sr).  */

CIA
SEM_FN_NAME (m32r,ld_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ld_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

* FLD (f_r1) = GETMEMSI (current_cpu, ADDSI (* FLD (f_r2), FLD (f_simm16)));
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* ldb: ldb $dr,@$sr.  */

CIA
SEM_FN_NAME (m32r,ldb) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldb.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

* FLD (f_r1) = EXTQISI (GETMEMQI (current_cpu, * FLD (f_r2)));
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* ldb-d: ldb $dr,@($slo16,$sr).  */

CIA
SEM_FN_NAME (m32r,ldb_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldb_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

* FLD (f_r1) = EXTQISI (GETMEMQI (current_cpu, ADDSI (* FLD (f_r2), FLD (f_simm16))));
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* ldh: ldh $dr,@$sr.  */

CIA
SEM_FN_NAME (m32r,ldh) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldh.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

* FLD (f_r1) = EXTHISI (GETMEMHI (current_cpu, * FLD (f_r2)));
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* ldh-d: ldh $dr,@($slo16,$sr).  */

CIA
SEM_FN_NAME (m32r,ldh_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldh_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

* FLD (f_r1) = EXTHISI (GETMEMHI (current_cpu, ADDSI (* FLD (f_r2), FLD (f_simm16))));
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* ldub: ldub $dr,@$sr.  */

CIA
SEM_FN_NAME (m32r,ldub) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldb.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

* FLD (f_r1) = ZEXTQISI (GETMEMQI (current_cpu, * FLD (f_r2)));
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* ldub-d: ldub $dr,@($slo16,$sr).  */

CIA
SEM_FN_NAME (m32r,ldub_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldb_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

* FLD (f_r1) = ZEXTQISI (GETMEMQI (current_cpu, ADDSI (* FLD (f_r2), FLD (f_simm16))));
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* lduh: lduh $dr,@$sr.  */

CIA
SEM_FN_NAME (m32r,lduh) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldh.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

* FLD (f_r1) = ZEXTHISI (GETMEMHI (current_cpu, * FLD (f_r2)));
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* lduh-d: lduh $dr,@($slo16,$sr).  */

CIA
SEM_FN_NAME (m32r,lduh_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldh_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

* FLD (f_r1) = ZEXTHISI (GETMEMHI (current_cpu, ADDSI (* FLD (f_r2), FLD (f_simm16))));
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* ld-plus: ld $dr,@$sr+.  */

CIA
SEM_FN_NAME (m32r,ld_plus) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ld_plus.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

do {
  SI temp1;SI temp0;
  temp0 = GETMEMSI (current_cpu, * FLD (f_r2));
  temp1 = ADDSI (* FLD (f_r2), 4);
* FLD (f_r1) = temp0;
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));
* FLD (f_r2) = temp1;
  TRACE_RESULT (current_cpu, "sr", 'x', * FLD (f_r2));
} while (0);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* ld24: ld24 $dr,$uimm24.  */

CIA
SEM_FN_NAME (m32r,ld24) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ld24.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

* FLD (f_r1) = FLD (f_uimm24);
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* ldi8: ldi8 $dr,$simm8.  */

CIA
SEM_FN_NAME (m32r,ldi8) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldi8.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

* FLD (f_r1) = FLD (f_simm8);
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* ldi16: ldi16 $dr,$hash$slo16.  */

CIA
SEM_FN_NAME (m32r,ldi16) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldi16.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

* FLD (f_r1) = FLD (f_simm16);
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* lock: lock $dr,@$sr.  */

CIA
SEM_FN_NAME (m32r,lock) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_lock.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

do {
  CPU (h_lock) = 1;
  TRACE_RESULT (current_cpu, "lock-0", 'x', CPU (h_lock));
* FLD (f_r1) = GETMEMSI (current_cpu, * FLD (f_r2));
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));
} while (0);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* machi: machi $src1,$src2.  */

CIA
SEM_FN_NAME (m32r,machi) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_machi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

m32r_h_accum_set (current_cpu, SRADI (SLLDI (ADDDI (m32r_h_accum_get (current_cpu), MULDI (EXTSIDI (ANDSI (* FLD (f_r1), 0xffff0000)), EXTHIDI (TRUNCSIHI (SRASI (* FLD (f_r2), 16))))), 8), 8));
  TRACE_RESULT (current_cpu, "accum", 'D', m32r_h_accum_get (current_cpu));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* maclo: maclo $src1,$src2.  */

CIA
SEM_FN_NAME (m32r,maclo) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_machi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

m32r_h_accum_set (current_cpu, SRADI (SLLDI (ADDDI (m32r_h_accum_get (current_cpu), MULDI (EXTSIDI (SLLSI (* FLD (f_r1), 16)), EXTHIDI (TRUNCSIHI (* FLD (f_r2))))), 8), 8));
  TRACE_RESULT (current_cpu, "accum", 'D', m32r_h_accum_get (current_cpu));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* macwhi: macwhi $src1,$src2.  */

CIA
SEM_FN_NAME (m32r,macwhi) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_machi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

m32r_h_accum_set (current_cpu, SRADI (SLLDI (ADDDI (m32r_h_accum_get (current_cpu), MULDI (EXTSIDI (* FLD (f_r1)), EXTHIDI (TRUNCSIHI (SRASI (* FLD (f_r2), 16))))), 8), 8));
  TRACE_RESULT (current_cpu, "accum", 'D', m32r_h_accum_get (current_cpu));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* macwlo: macwlo $src1,$src2.  */

CIA
SEM_FN_NAME (m32r,macwlo) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_machi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

m32r_h_accum_set (current_cpu, SRADI (SLLDI (ADDDI (m32r_h_accum_get (current_cpu), MULDI (EXTSIDI (* FLD (f_r1)), EXTHIDI (TRUNCSIHI (* FLD (f_r2))))), 8), 8));
  TRACE_RESULT (current_cpu, "accum", 'D', m32r_h_accum_get (current_cpu));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* mul: mul $dr,$sr.  */

CIA
SEM_FN_NAME (m32r,mul) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

* FLD (f_r1) = MULSI (* FLD (f_r1), * FLD (f_r2));
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* mulhi: mulhi $src1,$src2.  */

CIA
SEM_FN_NAME (m32r,mulhi) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulhi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

m32r_h_accum_set (current_cpu, SRADI (SLLDI (MULDI (EXTSIDI (ANDSI (* FLD (f_r1), 0xffff0000)), EXTHIDI (TRUNCSIHI (SRASI (* FLD (f_r2), 16)))), 16), 16));
  TRACE_RESULT (current_cpu, "accum", 'D', m32r_h_accum_get (current_cpu));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* mullo: mullo $src1,$src2.  */

CIA
SEM_FN_NAME (m32r,mullo) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulhi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

m32r_h_accum_set (current_cpu, SRADI (SLLDI (MULDI (EXTSIDI (SLLSI (* FLD (f_r1), 16)), EXTHIDI (TRUNCSIHI (* FLD (f_r2)))), 16), 16));
  TRACE_RESULT (current_cpu, "accum", 'D', m32r_h_accum_get (current_cpu));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* mulwhi: mulwhi $src1,$src2.  */

CIA
SEM_FN_NAME (m32r,mulwhi) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulhi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

m32r_h_accum_set (current_cpu, SRADI (SLLDI (MULDI (EXTSIDI (* FLD (f_r1)), EXTHIDI (TRUNCSIHI (SRASI (* FLD (f_r2), 16)))), 8), 8));
  TRACE_RESULT (current_cpu, "accum", 'D', m32r_h_accum_get (current_cpu));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* mulwlo: mulwlo $src1,$src2.  */

CIA
SEM_FN_NAME (m32r,mulwlo) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulhi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

m32r_h_accum_set (current_cpu, SRADI (SLLDI (MULDI (EXTSIDI (* FLD (f_r1)), EXTHIDI (TRUNCSIHI (* FLD (f_r2)))), 8), 8));
  TRACE_RESULT (current_cpu, "accum", 'D', m32r_h_accum_get (current_cpu));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* mv: mv $dr,$sr.  */

CIA
SEM_FN_NAME (m32r,mv) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mv.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

* FLD (f_r1) = * FLD (f_r2);
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* mvfachi: mvfachi $dr.  */

CIA
SEM_FN_NAME (m32r,mvfachi) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mvfachi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

* FLD (f_r1) = TRUNCDISI (SRADI (m32r_h_accum_get (current_cpu), 32));
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* mvfaclo: mvfaclo $dr.  */

CIA
SEM_FN_NAME (m32r,mvfaclo) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mvfachi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

* FLD (f_r1) = TRUNCDISI (m32r_h_accum_get (current_cpu));
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* mvfacmi: mvfacmi $dr.  */

CIA
SEM_FN_NAME (m32r,mvfacmi) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mvfachi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

* FLD (f_r1) = TRUNCDISI (SRADI (m32r_h_accum_get (current_cpu), 16));
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* mvfc: mvfc $dr,$scr.  */

CIA
SEM_FN_NAME (m32r,mvfc) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mvfc.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

* FLD (f_r1) = m32r_h_cr_get (current_cpu, FLD (f_r2));
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* mvtachi: mvtachi $src1.  */

CIA
SEM_FN_NAME (m32r,mvtachi) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mvtachi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

m32r_h_accum_set (current_cpu, ORDI (ANDDI (m32r_h_accum_get (current_cpu), MAKEDI (0, 0xffffffff)), SLLDI (EXTSIDI (* FLD (f_r1)), 32)));
  TRACE_RESULT (current_cpu, "accum", 'D', m32r_h_accum_get (current_cpu));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* mvtaclo: mvtaclo $src1.  */

CIA
SEM_FN_NAME (m32r,mvtaclo) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mvtachi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

m32r_h_accum_set (current_cpu, ORDI (ANDDI (m32r_h_accum_get (current_cpu), MAKEDI (0xffffffff, 0)), ZEXTSIDI (* FLD (f_r1))));
  TRACE_RESULT (current_cpu, "accum", 'D', m32r_h_accum_get (current_cpu));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* mvtc: mvtc $sr,$dcr.  */

CIA
SEM_FN_NAME (m32r,mvtc) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mvtc.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

m32r_h_cr_set (current_cpu, FLD (f_r1), * FLD (f_r2));
  TRACE_RESULT (current_cpu, "dcr", 'x', m32r_h_cr_get (current_cpu, FLD (f_r1)));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* neg: neg $dr,$sr.  */

CIA
SEM_FN_NAME (m32r,neg) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mv.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

* FLD (f_r1) = NEGSI (* FLD (f_r2));
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* nop: nop.  */

CIA
SEM_FN_NAME (m32r,nop) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_nop.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

PROFILE_COUNT_FILLNOPS (current_cpu, abuf->addr);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* not: not $dr,$sr.  */

CIA
SEM_FN_NAME (m32r,not) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mv.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

* FLD (f_r1) = INVSI (* FLD (f_r2));
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* rac: rac.  */

CIA
SEM_FN_NAME (m32r,rac) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_rac.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

do {
  DI tmp_tmp1;
  tmp_tmp1 = SLLDI (m32r_h_accum_get (current_cpu), 1);
  tmp_tmp1 = ADDDI (tmp_tmp1, MAKEDI (0, 32768));
m32r_h_accum_set (current_cpu, (GTDI (tmp_tmp1, MAKEDI (32767, 0xffff0000))) ? (MAKEDI (32767, 0xffff0000)) : (LTDI (tmp_tmp1, MAKEDI (0xffff8000, 0))) ? (MAKEDI (0xffff8000, 0)) : (ANDDI (tmp_tmp1, MAKEDI (0xffffffff, 0xffff0000))));
  TRACE_RESULT (current_cpu, "accum", 'D', m32r_h_accum_get (current_cpu));
} while (0);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* rach: rach.  */

CIA
SEM_FN_NAME (m32r,rach) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_rac.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

do {
  DI tmp_tmp1;
  tmp_tmp1 = ANDDI (m32r_h_accum_get (current_cpu), MAKEDI (16777215, 0xffffffff));
if (ANDIFSI (GEDI (tmp_tmp1, MAKEDI (16383, 0x80000000)), LEDI (tmp_tmp1, MAKEDI (8388607, 0xffffffff)))) {
  tmp_tmp1 = MAKEDI (16383, 0x80000000);
} else {
if (ANDIFSI (GEDI (tmp_tmp1, MAKEDI (8388608, 0)), LEDI (tmp_tmp1, MAKEDI (16760832, 0)))) {
  tmp_tmp1 = MAKEDI (16760832, 0);
} else {
  tmp_tmp1 = ANDDI (ADDDI (m32r_h_accum_get (current_cpu), MAKEDI (0, 1073741824)), MAKEDI (0xffffffff, 0x80000000));
}
}
  tmp_tmp1 = SLLDI (tmp_tmp1, 1);
m32r_h_accum_set (current_cpu, SRADI (SLLDI (tmp_tmp1, 7), 7));
  TRACE_RESULT (current_cpu, "accum", 'D', m32r_h_accum_get (current_cpu));
} while (0);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* rte: rte.  */

CIA
SEM_FN_NAME (m32r,rte) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_rte.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  int taken_p = 0;

do {
  CPU (h_sm) = CPU (h_bsm);
  TRACE_RESULT (current_cpu, "sm-0", 'x', CPU (h_sm));
  CPU (h_ie) = CPU (h_bie);
  TRACE_RESULT (current_cpu, "ie-0", 'x', CPU (h_ie));
  CPU (h_cond) = CPU (h_bcond);
  TRACE_RESULT (current_cpu, "condbit", 'x', CPU (h_cond));
  BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_ADDR (sem_arg, ANDSI (CPU (h_bpc), -4)));
  taken_p = 1;
  TRACE_RESULT (current_cpu, "pc", 'x', new_pc);
} while (0);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef FLD
}

/* seth: seth $dr,$hash$hi16.  */

CIA
SEM_FN_NAME (m32r,seth) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_seth.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

* FLD (f_r1) = SLLSI (FLD (f_hi16), 16);
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* sll: sll $dr,$sr.  */

CIA
SEM_FN_NAME (m32r,sll) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

* FLD (f_r1) = SLLSI (* FLD (f_r1), ANDSI (* FLD (f_r2), 31));
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* sll3: sll3 $dr,$sr,$simm16.  */

CIA
SEM_FN_NAME (m32r,sll3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_sll3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

* FLD (f_r1) = SLLSI (* FLD (f_r2), ANDSI (FLD (f_simm16), 31));
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* slli: slli $dr,$uimm5.  */

CIA
SEM_FN_NAME (m32r,slli) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_slli.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

* FLD (f_r1) = SLLSI (* FLD (f_r1), FLD (f_uimm5));
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* sra: sra $dr,$sr.  */

CIA
SEM_FN_NAME (m32r,sra) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

* FLD (f_r1) = SRASI (* FLD (f_r1), ANDSI (* FLD (f_r2), 31));
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* sra3: sra3 $dr,$sr,$simm16.  */

CIA
SEM_FN_NAME (m32r,sra3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_sll3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

* FLD (f_r1) = SRASI (* FLD (f_r2), ANDSI (FLD (f_simm16), 31));
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* srai: srai $dr,$uimm5.  */

CIA
SEM_FN_NAME (m32r,srai) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_slli.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

* FLD (f_r1) = SRASI (* FLD (f_r1), FLD (f_uimm5));
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* srl: srl $dr,$sr.  */

CIA
SEM_FN_NAME (m32r,srl) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

* FLD (f_r1) = SRLSI (* FLD (f_r1), ANDSI (* FLD (f_r2), 31));
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* srl3: srl3 $dr,$sr,$simm16.  */

CIA
SEM_FN_NAME (m32r,srl3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_sll3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

* FLD (f_r1) = SRLSI (* FLD (f_r2), ANDSI (FLD (f_simm16), 31));
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* srli: srli $dr,$uimm5.  */

CIA
SEM_FN_NAME (m32r,srli) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_slli.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

* FLD (f_r1) = SRLSI (* FLD (f_r1), FLD (f_uimm5));
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* st: st $src1,@$src2.  */

CIA
SEM_FN_NAME (m32r,st) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_st.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

SETMEMSI (current_cpu, * FLD (f_r2), * FLD (f_r1));
  TRACE_RESULT (current_cpu, "memory", 'x', GETMEMSI (current_cpu, * FLD (f_r2)));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* st-d: st $src1,@($slo16,$src2).  */

CIA
SEM_FN_NAME (m32r,st_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_st_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

SETMEMSI (current_cpu, ADDSI (* FLD (f_r2), FLD (f_simm16)), * FLD (f_r1));
  TRACE_RESULT (current_cpu, "memory", 'x', GETMEMSI (current_cpu, ADDSI (* FLD (f_r2), FLD (f_simm16))));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* stb: stb $src1,@$src2.  */

CIA
SEM_FN_NAME (m32r,stb) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stb.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

SETMEMQI (current_cpu, * FLD (f_r2), * FLD (f_r1));
  TRACE_RESULT (current_cpu, "memory", 'x', GETMEMQI (current_cpu, * FLD (f_r2)));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* stb-d: stb $src1,@($slo16,$src2).  */

CIA
SEM_FN_NAME (m32r,stb_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stb_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

SETMEMQI (current_cpu, ADDSI (* FLD (f_r2), FLD (f_simm16)), * FLD (f_r1));
  TRACE_RESULT (current_cpu, "memory", 'x', GETMEMQI (current_cpu, ADDSI (* FLD (f_r2), FLD (f_simm16))));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* sth: sth $src1,@$src2.  */

CIA
SEM_FN_NAME (m32r,sth) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_sth.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

SETMEMHI (current_cpu, * FLD (f_r2), * FLD (f_r1));
  TRACE_RESULT (current_cpu, "memory", 'x', GETMEMHI (current_cpu, * FLD (f_r2)));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* sth-d: sth $src1,@($slo16,$src2).  */

CIA
SEM_FN_NAME (m32r,sth_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_sth_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

SETMEMHI (current_cpu, ADDSI (* FLD (f_r2), FLD (f_simm16)), * FLD (f_r1));
  TRACE_RESULT (current_cpu, "memory", 'x', GETMEMHI (current_cpu, ADDSI (* FLD (f_r2), FLD (f_simm16))));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* st-plus: st $src1,@+$src2.  */

CIA
SEM_FN_NAME (m32r,st_plus) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_st_plus.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

do {
  SI tmp_new_src2;
  tmp_new_src2 = ADDSI (* FLD (f_r2), 4);
SETMEMSI (current_cpu, tmp_new_src2, * FLD (f_r1));
  TRACE_RESULT (current_cpu, "memory", 'x', GETMEMSI (current_cpu, tmp_new_src2));
* FLD (f_r2) = tmp_new_src2;
  TRACE_RESULT (current_cpu, "src2", 'x', * FLD (f_r2));
} while (0);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* st-minus: st $src1,@-$src2.  */

CIA
SEM_FN_NAME (m32r,st_minus) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_st_plus.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

do {
  SI tmp_new_src2;
  tmp_new_src2 = SUBSI (* FLD (f_r2), 4);
SETMEMSI (current_cpu, tmp_new_src2, * FLD (f_r1));
  TRACE_RESULT (current_cpu, "memory", 'x', GETMEMSI (current_cpu, tmp_new_src2));
* FLD (f_r2) = tmp_new_src2;
  TRACE_RESULT (current_cpu, "src2", 'x', * FLD (f_r2));
} while (0);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* sub: sub $dr,$sr.  */

CIA
SEM_FN_NAME (m32r,sub) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

* FLD (f_r1) = SUBSI (* FLD (f_r1), * FLD (f_r2));
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* subv: subv $dr,$sr.  */

CIA
SEM_FN_NAME (m32r,subv) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_addv.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

do {
  BI temp1;SI temp0;
  temp0 = SUBSI (* FLD (f_r1), * FLD (f_r2));
  temp1 = SUBOFSI (* FLD (f_r1), * FLD (f_r2), 0);
* FLD (f_r1) = temp0;
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));
  CPU (h_cond) = temp1;
  TRACE_RESULT (current_cpu, "condbit", 'x', CPU (h_cond));
} while (0);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* subx: subx $dr,$sr.  */

CIA
SEM_FN_NAME (m32r,subx) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_addx.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

do {
  BI temp1;SI temp0;
  temp0 = SUBCSI (* FLD (f_r1), * FLD (f_r2), CPU (h_cond));
  temp1 = SUBCFSI (* FLD (f_r1), * FLD (f_r2), CPU (h_cond));
* FLD (f_r1) = temp0;
  TRACE_RESULT (current_cpu, "dr", 'x', * FLD (f_r1));
  CPU (h_cond) = temp1;
  TRACE_RESULT (current_cpu, "condbit", 'x', CPU (h_cond));
} while (0);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_mark_set_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* trap: trap $uimm4.  */

CIA
SEM_FN_NAME (m32r,trap) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_trap.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  int taken_p = 0;

do {
m32r_h_cr_set (current_cpu, 6, ADDSI (CPU (h_pc), 4));
  TRACE_RESULT (current_cpu, "cr-6", 'x', m32r_h_cr_get (current_cpu, 6));
m32r_h_cr_set (current_cpu, 0, ANDSI (SLLSI (m32r_h_cr_get (current_cpu, 0), 8), 65408));
  TRACE_RESULT (current_cpu, "cr-0", 'x', m32r_h_cr_get (current_cpu, 0));
  BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_ADDR (sem_arg, do_trap (current_cpu, FLD (f_uimm4))));
  taken_p = 1;
  TRACE_RESULT (current_cpu, "pc", 'x', new_pc);
} while (0);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef FLD
}

/* unlock: unlock $src1,@$src2.  */

CIA
SEM_FN_NAME (m32r,unlock) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_unlock.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

do {
if (CPU (h_lock)) {
SETMEMSI (current_cpu, * FLD (f_r2), * FLD (f_r1));
  TRACE_RESULT (current_cpu, "memory", 'x', GETMEMSI (current_cpu, * FLD (f_r2)));
}
  CPU (h_lock) = 0;
  TRACE_RESULT (current_cpu, "lock-0", 'x', CPU (h_lock));
} while (0);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32r_model_mark_get_h_gr (current_cpu, abuf);
      m32r_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

CIA
SEM_FN_NAME (m32r,illegal) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
  sim_engine_illegal_insn (current_cpu, NULL_CIA /*FIXME*/);
  return 0;
}

#endif /* WITH_SCACHE */
