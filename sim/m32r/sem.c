/* Simulator instruction semantics for m32rb.

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

#define WANT_CPU
#define WANT_CPU_M32RB

#include "sim-main.h"
#include "cgen-mem.h"
#include "cgen-ops.h"
#include "cpu-sim.h"

#if WITH_SCACHE

#undef GET_ATTR
#define GET_ATTR(cpu, num, attr) CGEN_INSN_ATTR (abuf->idesc->opcode, CGEN_INSN_##attr)

/* add: add $dr,$sr.  */

CIA
SEM_FN_NAME (m32rb,add) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = ADDSI (* FLD (f_r1), * FLD (f_r2));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* add3: add3 $dr,$sr,$hash$slo16.  */

CIA
SEM_FN_NAME (m32rb,add3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_add3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

  {
    SI opval = ADDSI (* FLD (f_r2), FLD (f_simm16));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* and: and $dr,$sr.  */

CIA
SEM_FN_NAME (m32rb,and) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = ANDSI (* FLD (f_r1), * FLD (f_r2));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* and3: and3 $dr,$sr,$uimm16.  */

CIA
SEM_FN_NAME (m32rb,and3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_and3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

  {
    SI opval = ANDSI (* FLD (f_r2), FLD (f_uimm16));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* or: or $dr,$sr.  */

CIA
SEM_FN_NAME (m32rb,or) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = ORSI (* FLD (f_r1), * FLD (f_r2));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* or3: or3 $dr,$sr,$hash$ulo16.  */

CIA
SEM_FN_NAME (m32rb,or3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_or3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

  {
    SI opval = ORSI (* FLD (f_r2), FLD (f_uimm16));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* xor: xor $dr,$sr.  */

CIA
SEM_FN_NAME (m32rb,xor) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = XORSI (* FLD (f_r1), * FLD (f_r2));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* xor3: xor3 $dr,$sr,$uimm16.  */

CIA
SEM_FN_NAME (m32rb,xor3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_and3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

  {
    SI opval = XORSI (* FLD (f_r2), FLD (f_uimm16));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* addi: addi $dr,$simm8.  */

CIA
SEM_FN_NAME (m32rb,addi) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_addi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = ADDSI (* FLD (f_r1), FLD (f_simm8));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* addv: addv $dr,$sr.  */

CIA
SEM_FN_NAME (m32rb,addv) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_addv.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

do {
  UBI temp1;SI temp0;
  temp0 = ADDSI (* FLD (f_r1), * FLD (f_r2));
  temp1 = ADDOFSI (* FLD (f_r1), * FLD (f_r2), 0);
  {
    SI opval = temp0;
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }
  {
    UBI opval = temp1;
    CPU (h_cond) = opval;
    TRACE_RESULT (current_cpu, "condbit", 'x', opval);
  }
} while (0);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* addv3: addv3 $dr,$sr,$simm16.  */

CIA
SEM_FN_NAME (m32rb,addv3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_addv3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

do {
  UBI temp1;SI temp0;
  temp0 = ADDSI (* FLD (f_r2), FLD (f_simm16));
  temp1 = ADDOFSI (* FLD (f_r2), FLD (f_simm16), 0);
  {
    SI opval = temp0;
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }
  {
    UBI opval = temp1;
    CPU (h_cond) = opval;
    TRACE_RESULT (current_cpu, "condbit", 'x', opval);
  }
} while (0);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* addx: addx $dr,$sr.  */

CIA
SEM_FN_NAME (m32rb,addx) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_addx.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

do {
  UBI temp1;SI temp0;
  temp0 = ADDCSI (* FLD (f_r1), * FLD (f_r2), CPU (h_cond));
  temp1 = ADDCFSI (* FLD (f_r1), * FLD (f_r2), CPU (h_cond));
  {
    SI opval = temp0;
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }
  {
    UBI opval = temp1;
    CPU (h_cond) = opval;
    TRACE_RESULT (current_cpu, "condbit", 'x', opval);
  }
} while (0);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* bc8: bc.s $disp8.  */

CIA
SEM_FN_NAME (m32rb,bc8) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_bc8.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  int taken_p = 0;

if (CPU (h_cond)) {
  {
    USI opval = FLD (f_disp8);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, opval));
    taken_p = 1;
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef FLD
}

/* bc24: bc.l $disp24.  */

CIA
SEM_FN_NAME (m32rb,bc24) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_bc24.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  int taken_p = 0;

if (CPU (h_cond)) {
  {
    USI opval = FLD (f_disp24);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, opval));
    taken_p = 1;
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef FLD
}

/* beq: beq $src1,$src2,$disp16.  */

CIA
SEM_FN_NAME (m32rb,beq) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_beq.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  int taken_p = 0;

if (EQSI (* FLD (f_r1), * FLD (f_r2))) {
  {
    USI opval = FLD (f_disp16);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, opval));
    taken_p = 1;
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef FLD
}

/* beqz: beqz $src2,$disp16.  */

CIA
SEM_FN_NAME (m32rb,beqz) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_beqz.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  int taken_p = 0;

if (EQSI (* FLD (f_r2), 0)) {
  {
    USI opval = FLD (f_disp16);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, opval));
    taken_p = 1;
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef FLD
}

/* bgez: bgez $src2,$disp16.  */

CIA
SEM_FN_NAME (m32rb,bgez) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_beqz.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  int taken_p = 0;

if (GESI (* FLD (f_r2), 0)) {
  {
    USI opval = FLD (f_disp16);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, opval));
    taken_p = 1;
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef FLD
}

/* bgtz: bgtz $src2,$disp16.  */

CIA
SEM_FN_NAME (m32rb,bgtz) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_beqz.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  int taken_p = 0;

if (GTSI (* FLD (f_r2), 0)) {
  {
    USI opval = FLD (f_disp16);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, opval));
    taken_p = 1;
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef FLD
}

/* blez: blez $src2,$disp16.  */

CIA
SEM_FN_NAME (m32rb,blez) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_beqz.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  int taken_p = 0;

if (LESI (* FLD (f_r2), 0)) {
  {
    USI opval = FLD (f_disp16);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, opval));
    taken_p = 1;
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef FLD
}

/* bltz: bltz $src2,$disp16.  */

CIA
SEM_FN_NAME (m32rb,bltz) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_beqz.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  int taken_p = 0;

if (LTSI (* FLD (f_r2), 0)) {
  {
    USI opval = FLD (f_disp16);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, opval));
    taken_p = 1;
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef FLD
}

/* bnez: bnez $src2,$disp16.  */

CIA
SEM_FN_NAME (m32rb,bnez) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_beqz.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  int taken_p = 0;

if (NESI (* FLD (f_r2), 0)) {
  {
    USI opval = FLD (f_disp16);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, opval));
    taken_p = 1;
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef FLD
}

/* bl8: bl.s $disp8.  */

CIA
SEM_FN_NAME (m32rb,bl8) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_bl8.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  int taken_p = 0;

do {
  {
    SI opval = ADDSI (ANDSI (CPU (h_pc), -4), 4);
    CPU (h_gr[((HOSTUINT) 14)]) = opval;
    TRACE_RESULT (current_cpu, "gr-14", 'x', opval);
  }
  {
    USI opval = FLD (f_disp8);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, opval));
    taken_p = 1;
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
} while (0);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef FLD
}

/* bl24: bl.l $disp24.  */

CIA
SEM_FN_NAME (m32rb,bl24) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_bl24.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  int taken_p = 0;

do {
  {
    SI opval = ADDSI (CPU (h_pc), 4);
    CPU (h_gr[((HOSTUINT) 14)]) = opval;
    TRACE_RESULT (current_cpu, "gr-14", 'x', opval);
  }
  {
    USI opval = FLD (f_disp24);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, opval));
    taken_p = 1;
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
} while (0);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef FLD
}

/* bnc8: bnc.s $disp8.  */

CIA
SEM_FN_NAME (m32rb,bnc8) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_bc8.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  int taken_p = 0;

if (NOTBI (CPU (h_cond))) {
  {
    USI opval = FLD (f_disp8);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, opval));
    taken_p = 1;
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef FLD
}

/* bnc24: bnc.l $disp24.  */

CIA
SEM_FN_NAME (m32rb,bnc24) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_bc24.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  int taken_p = 0;

if (NOTBI (CPU (h_cond))) {
  {
    USI opval = FLD (f_disp24);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, opval));
    taken_p = 1;
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef FLD
}

/* bne: bne $src1,$src2,$disp16.  */

CIA
SEM_FN_NAME (m32rb,bne) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_beq.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  int taken_p = 0;

if (NESI (* FLD (f_r1), * FLD (f_r2))) {
  {
    USI opval = FLD (f_disp16);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, opval));
    taken_p = 1;
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef FLD
}

/* bra8: bra.s $disp8.  */

CIA
SEM_FN_NAME (m32rb,bra8) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_bra8.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  int taken_p = 0;

  {
    USI opval = FLD (f_disp8);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, opval));
    taken_p = 1;
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef FLD
}

/* bra24: bra.l $disp24.  */

CIA
SEM_FN_NAME (m32rb,bra24) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_bra24.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  int taken_p = 0;

  {
    USI opval = FLD (f_disp24);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, opval));
    taken_p = 1;
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef FLD
}

/* cmp: cmp $src1,$src2.  */

CIA
SEM_FN_NAME (m32rb,cmp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_cmp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    UBI opval = LTSI (* FLD (f_r1), * FLD (f_r2));
    CPU (h_cond) = opval;
    TRACE_RESULT (current_cpu, "condbit", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* cmpi: cmpi $src2,$simm16.  */

CIA
SEM_FN_NAME (m32rb,cmpi) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_cmpi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

  {
    UBI opval = LTSI (* FLD (f_r2), FLD (f_simm16));
    CPU (h_cond) = opval;
    TRACE_RESULT (current_cpu, "condbit", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* cmpu: cmpu $src1,$src2.  */

CIA
SEM_FN_NAME (m32rb,cmpu) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_cmp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    UBI opval = LTUSI (* FLD (f_r1), * FLD (f_r2));
    CPU (h_cond) = opval;
    TRACE_RESULT (current_cpu, "condbit", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* cmpui: cmpui $src2,$simm16.  */

CIA
SEM_FN_NAME (m32rb,cmpui) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_cmpi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

  {
    UBI opval = LTUSI (* FLD (f_r2), FLD (f_simm16));
    CPU (h_cond) = opval;
    TRACE_RESULT (current_cpu, "condbit", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* div: div $dr,$sr.  */

CIA
SEM_FN_NAME (m32rb,div) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_div.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

if (NESI (* FLD (f_r2), 0)) {
  {
    SI opval = DIVSI (* FLD (f_r1), * FLD (f_r2));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* divu: divu $dr,$sr.  */

CIA
SEM_FN_NAME (m32rb,divu) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_div.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

if (NESI (* FLD (f_r2), 0)) {
  {
    SI opval = UDIVSI (* FLD (f_r1), * FLD (f_r2));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* rem: rem $dr,$sr.  */

CIA
SEM_FN_NAME (m32rb,rem) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_div.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

if (NESI (* FLD (f_r2), 0)) {
  {
    SI opval = MODSI (* FLD (f_r1), * FLD (f_r2));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* remu: remu $dr,$sr.  */

CIA
SEM_FN_NAME (m32rb,remu) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_div.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

if (NESI (* FLD (f_r2), 0)) {
  {
    SI opval = UMODSI (* FLD (f_r1), * FLD (f_r2));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* jl: jl $sr.  */

CIA
SEM_FN_NAME (m32rb,jl) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_jl.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  int taken_p = 0;

do {
  USI temp1;SI temp0;
  temp0 = ADDSI (ANDSI (CPU (h_pc), -4), 4);
  temp1 = ANDSI (* FLD (f_r2), -4);
  {
    SI opval = temp0;
    CPU (h_gr[((HOSTUINT) 14)]) = opval;
    TRACE_RESULT (current_cpu, "gr-14", 'x', opval);
  }
  {
    USI opval = temp1;
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_ADDR (sem_arg, opval));
    taken_p = 1;
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
} while (0);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef FLD
}

/* jmp: jmp $sr.  */

CIA
SEM_FN_NAME (m32rb,jmp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_jmp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  int taken_p = 0;

  {
    USI opval = ANDSI (* FLD (f_r2), -4);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_ADDR (sem_arg, opval));
    taken_p = 1;
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef FLD
}

/* ld: ld $dr,@$sr.  */

CIA
SEM_FN_NAME (m32rb,ld) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ld.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = GETMEMSI (current_cpu, * FLD (f_r2));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* ld-d: ld $dr,@($slo16,$sr).  */

CIA
SEM_FN_NAME (m32rb,ld_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ld_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

  {
    SI opval = GETMEMSI (current_cpu, ADDSI (* FLD (f_r2), FLD (f_simm16)));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* ldb: ldb $dr,@$sr.  */

CIA
SEM_FN_NAME (m32rb,ldb) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldb.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = EXTQISI (GETMEMQI (current_cpu, * FLD (f_r2)));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* ldb-d: ldb $dr,@($slo16,$sr).  */

CIA
SEM_FN_NAME (m32rb,ldb_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldb_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

  {
    SI opval = EXTQISI (GETMEMQI (current_cpu, ADDSI (* FLD (f_r2), FLD (f_simm16))));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* ldh: ldh $dr,@$sr.  */

CIA
SEM_FN_NAME (m32rb,ldh) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldh.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = EXTHISI (GETMEMHI (current_cpu, * FLD (f_r2)));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* ldh-d: ldh $dr,@($slo16,$sr).  */

CIA
SEM_FN_NAME (m32rb,ldh_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldh_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

  {
    SI opval = EXTHISI (GETMEMHI (current_cpu, ADDSI (* FLD (f_r2), FLD (f_simm16))));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* ldub: ldub $dr,@$sr.  */

CIA
SEM_FN_NAME (m32rb,ldub) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldb.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = ZEXTQISI (GETMEMQI (current_cpu, * FLD (f_r2)));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* ldub-d: ldub $dr,@($slo16,$sr).  */

CIA
SEM_FN_NAME (m32rb,ldub_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldb_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

  {
    SI opval = ZEXTQISI (GETMEMQI (current_cpu, ADDSI (* FLD (f_r2), FLD (f_simm16))));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* lduh: lduh $dr,@$sr.  */

CIA
SEM_FN_NAME (m32rb,lduh) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldh.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = ZEXTHISI (GETMEMHI (current_cpu, * FLD (f_r2)));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* lduh-d: lduh $dr,@($slo16,$sr).  */

CIA
SEM_FN_NAME (m32rb,lduh_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldh_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

  {
    SI opval = ZEXTHISI (GETMEMHI (current_cpu, ADDSI (* FLD (f_r2), FLD (f_simm16))));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* ld-plus: ld $dr,@$sr+.  */

CIA
SEM_FN_NAME (m32rb,ld_plus) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ld_plus.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

do {
  SI temp1;SI temp0;
  temp0 = GETMEMSI (current_cpu, * FLD (f_r2));
  temp1 = ADDSI (* FLD (f_r2), 4);
  {
    SI opval = temp0;
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }
  {
    SI opval = temp1;
    * FLD (f_r2) = opval;
    TRACE_RESULT (current_cpu, "sr", 'x', opval);
  }
} while (0);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* ld24: ld24 $dr,$uimm24.  */

CIA
SEM_FN_NAME (m32rb,ld24) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ld24.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

  {
    SI opval = FLD (f_uimm24);
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* ldi8: ldi8 $dr,$simm8.  */

CIA
SEM_FN_NAME (m32rb,ldi8) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldi8.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = FLD (f_simm8);
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* ldi16: ldi16 $dr,$hash$slo16.  */

CIA
SEM_FN_NAME (m32rb,ldi16) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldi16.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

  {
    SI opval = FLD (f_simm16);
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* lock: lock $dr,@$sr.  */

CIA
SEM_FN_NAME (m32rb,lock) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_lock.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

do {
  {
    UBI opval = 1;
    CPU (h_lock) = opval;
    TRACE_RESULT (current_cpu, "lock-0", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, * FLD (f_r2));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }
} while (0);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* machi: machi $src1,$src2.  */

CIA
SEM_FN_NAME (m32rb,machi) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_machi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    DI opval = SRADI (SLLDI (ADDDI (m32rb_h_accum_get (current_cpu), MULDI (EXTSIDI (ANDSI (* FLD (f_r1), 0xffff0000)), EXTHIDI (TRUNCSIHI (SRASI (* FLD (f_r2), 16))))), 8), 8);
    m32rb_h_accum_set (current_cpu, opval);
    TRACE_RESULT (current_cpu, "accum", 'D', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* maclo: maclo $src1,$src2.  */

CIA
SEM_FN_NAME (m32rb,maclo) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_machi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    DI opval = SRADI (SLLDI (ADDDI (m32rb_h_accum_get (current_cpu), MULDI (EXTSIDI (SLLSI (* FLD (f_r1), 16)), EXTHIDI (TRUNCSIHI (* FLD (f_r2))))), 8), 8);
    m32rb_h_accum_set (current_cpu, opval);
    TRACE_RESULT (current_cpu, "accum", 'D', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* macwhi: macwhi $src1,$src2.  */

CIA
SEM_FN_NAME (m32rb,macwhi) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_machi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    DI opval = SRADI (SLLDI (ADDDI (m32rb_h_accum_get (current_cpu), MULDI (EXTSIDI (* FLD (f_r1)), EXTHIDI (TRUNCSIHI (SRASI (* FLD (f_r2), 16))))), 8), 8);
    m32rb_h_accum_set (current_cpu, opval);
    TRACE_RESULT (current_cpu, "accum", 'D', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* macwlo: macwlo $src1,$src2.  */

CIA
SEM_FN_NAME (m32rb,macwlo) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_machi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    DI opval = SRADI (SLLDI (ADDDI (m32rb_h_accum_get (current_cpu), MULDI (EXTSIDI (* FLD (f_r1)), EXTHIDI (TRUNCSIHI (* FLD (f_r2))))), 8), 8);
    m32rb_h_accum_set (current_cpu, opval);
    TRACE_RESULT (current_cpu, "accum", 'D', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* mul: mul $dr,$sr.  */

CIA
SEM_FN_NAME (m32rb,mul) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = MULSI (* FLD (f_r1), * FLD (f_r2));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* mulhi: mulhi $src1,$src2.  */

CIA
SEM_FN_NAME (m32rb,mulhi) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulhi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    DI opval = SRADI (SLLDI (MULDI (EXTSIDI (ANDSI (* FLD (f_r1), 0xffff0000)), EXTHIDI (TRUNCSIHI (SRASI (* FLD (f_r2), 16)))), 16), 16);
    m32rb_h_accum_set (current_cpu, opval);
    TRACE_RESULT (current_cpu, "accum", 'D', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* mullo: mullo $src1,$src2.  */

CIA
SEM_FN_NAME (m32rb,mullo) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulhi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    DI opval = SRADI (SLLDI (MULDI (EXTSIDI (SLLSI (* FLD (f_r1), 16)), EXTHIDI (TRUNCSIHI (* FLD (f_r2)))), 16), 16);
    m32rb_h_accum_set (current_cpu, opval);
    TRACE_RESULT (current_cpu, "accum", 'D', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* mulwhi: mulwhi $src1,$src2.  */

CIA
SEM_FN_NAME (m32rb,mulwhi) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulhi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    DI opval = SRADI (SLLDI (MULDI (EXTSIDI (* FLD (f_r1)), EXTHIDI (TRUNCSIHI (SRASI (* FLD (f_r2), 16)))), 8), 8);
    m32rb_h_accum_set (current_cpu, opval);
    TRACE_RESULT (current_cpu, "accum", 'D', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* mulwlo: mulwlo $src1,$src2.  */

CIA
SEM_FN_NAME (m32rb,mulwlo) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulhi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    DI opval = SRADI (SLLDI (MULDI (EXTSIDI (* FLD (f_r1)), EXTHIDI (TRUNCSIHI (* FLD (f_r2)))), 8), 8);
    m32rb_h_accum_set (current_cpu, opval);
    TRACE_RESULT (current_cpu, "accum", 'D', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* mv: mv $dr,$sr.  */

CIA
SEM_FN_NAME (m32rb,mv) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mv.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = * FLD (f_r2);
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* mvfachi: mvfachi $dr.  */

CIA
SEM_FN_NAME (m32rb,mvfachi) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mvfachi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = TRUNCDISI (SRADI (m32rb_h_accum_get (current_cpu), 32));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* mvfaclo: mvfaclo $dr.  */

CIA
SEM_FN_NAME (m32rb,mvfaclo) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mvfachi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = TRUNCDISI (m32rb_h_accum_get (current_cpu));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* mvfacmi: mvfacmi $dr.  */

CIA
SEM_FN_NAME (m32rb,mvfacmi) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mvfachi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = TRUNCDISI (SRADI (m32rb_h_accum_get (current_cpu), 16));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* mvfc: mvfc $dr,$scr.  */

CIA
SEM_FN_NAME (m32rb,mvfc) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mvfc.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = m32rb_h_cr_get (current_cpu, FLD (f_r2));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* mvtachi: mvtachi $src1.  */

CIA
SEM_FN_NAME (m32rb,mvtachi) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mvtachi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    DI opval = ORDI (ANDDI (m32rb_h_accum_get (current_cpu), MAKEDI (0, 0xffffffff)), SLLDI (EXTSIDI (* FLD (f_r1)), 32));
    m32rb_h_accum_set (current_cpu, opval);
    TRACE_RESULT (current_cpu, "accum", 'D', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* mvtaclo: mvtaclo $src1.  */

CIA
SEM_FN_NAME (m32rb,mvtaclo) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mvtachi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    DI opval = ORDI (ANDDI (m32rb_h_accum_get (current_cpu), MAKEDI (0xffffffff, 0)), ZEXTSIDI (* FLD (f_r1)));
    m32rb_h_accum_set (current_cpu, opval);
    TRACE_RESULT (current_cpu, "accum", 'D', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* mvtc: mvtc $sr,$dcr.  */

CIA
SEM_FN_NAME (m32rb,mvtc) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mvtc.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    USI opval = * FLD (f_r2);
    m32rb_h_cr_set (current_cpu, FLD (f_r1), opval);
    TRACE_RESULT (current_cpu, "dcr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* neg: neg $dr,$sr.  */

CIA
SEM_FN_NAME (m32rb,neg) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mv.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = NEGSI (* FLD (f_r2));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* nop: nop.  */

CIA
SEM_FN_NAME (m32rb,nop) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_nop.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

PROFILE_COUNT_FILLNOPS (current_cpu, abuf->addr);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* not: not $dr,$sr.  */

CIA
SEM_FN_NAME (m32rb,not) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mv.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = INVSI (* FLD (f_r2));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* rac: rac.  */

CIA
SEM_FN_NAME (m32rb,rac) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_rac.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

do {
  DI tmp_tmp1;
  tmp_tmp1 = SLLDI (m32rb_h_accum_get (current_cpu), 1);
  tmp_tmp1 = ADDDI (tmp_tmp1, MAKEDI (0, 32768));
  {
    DI opval = (GTDI (tmp_tmp1, MAKEDI (32767, 0xffff0000))) ? (MAKEDI (32767, 0xffff0000)) : (LTDI (tmp_tmp1, MAKEDI (0xffff8000, 0))) ? (MAKEDI (0xffff8000, 0)) : (ANDDI (tmp_tmp1, MAKEDI (0xffffffff, 0xffff0000)));
    m32rb_h_accum_set (current_cpu, opval);
    TRACE_RESULT (current_cpu, "accum", 'D', opval);
  }
} while (0);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* rach: rach.  */

CIA
SEM_FN_NAME (m32rb,rach) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_rac.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

do {
  DI tmp_tmp1;
  tmp_tmp1 = ANDDI (m32rb_h_accum_get (current_cpu), MAKEDI (16777215, 0xffffffff));
if (ANDIF (GEDI (tmp_tmp1, MAKEDI (16383, 0x80000000)), LEDI (tmp_tmp1, MAKEDI (8388607, 0xffffffff)))) {
  tmp_tmp1 = MAKEDI (16383, 0x80000000);
} else {
if (ANDIF (GEDI (tmp_tmp1, MAKEDI (8388608, 0)), LEDI (tmp_tmp1, MAKEDI (16760832, 0)))) {
  tmp_tmp1 = MAKEDI (16760832, 0);
} else {
  tmp_tmp1 = ANDDI (ADDDI (m32rb_h_accum_get (current_cpu), MAKEDI (0, 1073741824)), MAKEDI (0xffffffff, 0x80000000));
}
}
  tmp_tmp1 = SLLDI (tmp_tmp1, 1);
  {
    DI opval = SRADI (SLLDI (tmp_tmp1, 7), 7);
    m32rb_h_accum_set (current_cpu, opval);
    TRACE_RESULT (current_cpu, "accum", 'D', opval);
  }
} while (0);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* rte: rte.  */

CIA
SEM_FN_NAME (m32rb,rte) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_rte.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  int taken_p = 0;

do {
  {
    UBI opval = CPU (h_bsm);
    CPU (h_sm) = opval;
    TRACE_RESULT (current_cpu, "sm-0", 'x', opval);
  }
  {
    UBI opval = CPU (h_bie);
    CPU (h_ie) = opval;
    TRACE_RESULT (current_cpu, "ie-0", 'x', opval);
  }
  {
    UBI opval = CPU (h_bcond);
    CPU (h_cond) = opval;
    TRACE_RESULT (current_cpu, "condbit", 'x', opval);
  }
  {
    USI opval = ANDSI (CPU (h_bpc), -4);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_ADDR (sem_arg, opval));
    taken_p = 1;
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
} while (0);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef FLD
}

/* seth: seth $dr,$hash$hi16.  */

CIA
SEM_FN_NAME (m32rb,seth) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_seth.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

  {
    SI opval = SLLSI (FLD (f_hi16), 16);
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* sll: sll $dr,$sr.  */

CIA
SEM_FN_NAME (m32rb,sll) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = SLLSI (* FLD (f_r1), ANDSI (* FLD (f_r2), 31));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* sll3: sll3 $dr,$sr,$simm16.  */

CIA
SEM_FN_NAME (m32rb,sll3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_sll3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

  {
    SI opval = SLLSI (* FLD (f_r2), ANDSI (FLD (f_simm16), 31));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* slli: slli $dr,$uimm5.  */

CIA
SEM_FN_NAME (m32rb,slli) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_slli.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = SLLSI (* FLD (f_r1), FLD (f_uimm5));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* sra: sra $dr,$sr.  */

CIA
SEM_FN_NAME (m32rb,sra) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = SRASI (* FLD (f_r1), ANDSI (* FLD (f_r2), 31));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* sra3: sra3 $dr,$sr,$simm16.  */

CIA
SEM_FN_NAME (m32rb,sra3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_sll3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

  {
    SI opval = SRASI (* FLD (f_r2), ANDSI (FLD (f_simm16), 31));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* srai: srai $dr,$uimm5.  */

CIA
SEM_FN_NAME (m32rb,srai) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_slli.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = SRASI (* FLD (f_r1), FLD (f_uimm5));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* srl: srl $dr,$sr.  */

CIA
SEM_FN_NAME (m32rb,srl) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = SRLSI (* FLD (f_r1), ANDSI (* FLD (f_r2), 31));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* srl3: srl3 $dr,$sr,$simm16.  */

CIA
SEM_FN_NAME (m32rb,srl3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_sll3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

  {
    SI opval = SRLSI (* FLD (f_r2), ANDSI (FLD (f_simm16), 31));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* srli: srli $dr,$uimm5.  */

CIA
SEM_FN_NAME (m32rb,srli) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_slli.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = SRLSI (* FLD (f_r1), FLD (f_uimm5));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* st: st $src1,@$src2.  */

CIA
SEM_FN_NAME (m32rb,st) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_st.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = * FLD (f_r1);
    SETMEMSI (current_cpu, * FLD (f_r2), opval);
    TRACE_RESULT (current_cpu, "memory", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* st-d: st $src1,@($slo16,$src2).  */

CIA
SEM_FN_NAME (m32rb,st_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_st_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

  {
    SI opval = * FLD (f_r1);
    SETMEMSI (current_cpu, ADDSI (* FLD (f_r2), FLD (f_simm16)), opval);
    TRACE_RESULT (current_cpu, "memory", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* stb: stb $src1,@$src2.  */

CIA
SEM_FN_NAME (m32rb,stb) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stb.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    QI opval = * FLD (f_r1);
    SETMEMQI (current_cpu, * FLD (f_r2), opval);
    TRACE_RESULT (current_cpu, "memory", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* stb-d: stb $src1,@($slo16,$src2).  */

CIA
SEM_FN_NAME (m32rb,stb_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stb_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

  {
    QI opval = * FLD (f_r1);
    SETMEMQI (current_cpu, ADDSI (* FLD (f_r2), FLD (f_simm16)), opval);
    TRACE_RESULT (current_cpu, "memory", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* sth: sth $src1,@$src2.  */

CIA
SEM_FN_NAME (m32rb,sth) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_sth.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    HI opval = * FLD (f_r1);
    SETMEMHI (current_cpu, * FLD (f_r2), opval);
    TRACE_RESULT (current_cpu, "memory", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* sth-d: sth $src1,@($slo16,$src2).  */

CIA
SEM_FN_NAME (m32rb,sth_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_sth_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);

  {
    HI opval = * FLD (f_r1);
    SETMEMHI (current_cpu, ADDSI (* FLD (f_r2), FLD (f_simm16)), opval);
    TRACE_RESULT (current_cpu, "memory", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* st-plus: st $src1,@+$src2.  */

CIA
SEM_FN_NAME (m32rb,st_plus) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_st_plus.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

do {
  SI tmp_new_src2;
  tmp_new_src2 = ADDSI (* FLD (f_r2), 4);
  {
    SI opval = * FLD (f_r1);
    SETMEMSI (current_cpu, tmp_new_src2, opval);
    TRACE_RESULT (current_cpu, "memory", 'x', opval);
  }
  {
    SI opval = tmp_new_src2;
    * FLD (f_r2) = opval;
    TRACE_RESULT (current_cpu, "src2", 'x', opval);
  }
} while (0);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* st-minus: st $src1,@-$src2.  */

CIA
SEM_FN_NAME (m32rb,st_minus) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_st_plus.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

do {
  SI tmp_new_src2;
  tmp_new_src2 = SUBSI (* FLD (f_r2), 4);
  {
    SI opval = * FLD (f_r1);
    SETMEMSI (current_cpu, tmp_new_src2, opval);
    TRACE_RESULT (current_cpu, "memory", 'x', opval);
  }
  {
    SI opval = tmp_new_src2;
    * FLD (f_r2) = opval;
    TRACE_RESULT (current_cpu, "src2", 'x', opval);
  }
} while (0);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* sub: sub $dr,$sr.  */

CIA
SEM_FN_NAME (m32rb,sub) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = SUBSI (* FLD (f_r1), * FLD (f_r2));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* subv: subv $dr,$sr.  */

CIA
SEM_FN_NAME (m32rb,subv) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_addv.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

do {
  UBI temp1;SI temp0;
  temp0 = SUBSI (* FLD (f_r1), * FLD (f_r2));
  temp1 = SUBOFSI (* FLD (f_r1), * FLD (f_r2), 0);
  {
    SI opval = temp0;
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }
  {
    UBI opval = temp1;
    CPU (h_cond) = opval;
    TRACE_RESULT (current_cpu, "condbit", 'x', opval);
  }
} while (0);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* subx: subx $dr,$sr.  */

CIA
SEM_FN_NAME (m32rb,subx) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_addx.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

do {
  UBI temp1;SI temp0;
  temp0 = SUBCSI (* FLD (f_r1), * FLD (f_r2), CPU (h_cond));
  temp1 = SUBCFSI (* FLD (f_r1), * FLD (f_r2), CPU (h_cond));
  {
    SI opval = temp0;
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }
  {
    UBI opval = temp1;
    CPU (h_cond) = opval;
    TRACE_RESULT (current_cpu, "condbit", 'x', opval);
  }
} while (0);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_mark_set_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

/* trap: trap $uimm4.  */

CIA
SEM_FN_NAME (m32rb,trap) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_trap.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  int taken_p = 0;

do {
  {
    USI opval = ADDSI (CPU (h_pc), 4);
    m32rb_h_cr_set (current_cpu, ((HOSTUINT) 6), opval);
    TRACE_RESULT (current_cpu, "cr-6", 'x', opval);
  }
  {
    USI opval = ANDSI (SLLSI (m32rb_h_cr_get (current_cpu, ((HOSTUINT) 0)), 8), 65408);
    m32rb_h_cr_set (current_cpu, ((HOSTUINT) 0), opval);
    TRACE_RESULT (current_cpu, "cr-0", 'x', opval);
  }
  {
    SI opval = m32r_trap (current_cpu, CPU (h_pc), FLD (f_uimm4));
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_ADDR (sem_arg, opval));
    taken_p = 1;
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
} while (0);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef FLD
}

/* unlock: unlock $src1,@$src2.  */

CIA
SEM_FN_NAME (m32rb,unlock) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_unlock.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);

do {
if (CPU (h_lock)) {
  {
    SI opval = * FLD (f_r1);
    SETMEMSI (current_cpu, * FLD (f_r2), opval);
    TRACE_RESULT (current_cpu, "memory", 'x', opval);
  }
}
  {
    UBI opval = 0;
    CPU (h_lock) = opval;
    TRACE_RESULT (current_cpu, "lock-0", 'x', opval);
  }
} while (0);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rb_model_mark_get_h_gr (current_cpu, abuf);
      m32rb_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef FLD
}

CIA
SEM_FN_NAME (m32rb,illegal) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
  sim_engine_illegal_insn (current_cpu, NULL_CIA /*FIXME*/);
  return 0;
}

#endif /* WANT_CPU */
