/* Simulator instruction semantics for m32rx.

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
#define WANT_CPU_M32RX

#include "sim-main.h"
#include "cgen-mem.h"
#include "cgen-ops.h"
#include "cpu-sim.h"

#if ! WITH_SCACHE

#undef GET_ATTR
#define GET_ATTR(cpu, num, attr) CGEN_INSN_ATTR (abuf->idesc->opcode, CGEN_INSN_##attr)

/* add: add $dr,$sr.  */

CIA
SEM_FN_NAME (m32rx,add) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_ADD_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_ADD_CODE

  {
    SI opval = ADDSI (OPRND (dr), OPRND (sr));
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* add3: add3 $dr,$sr,$hash$slo16.  */

CIA
SEM_FN_NAME (m32rx,add3) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_add3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_ADD3_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_ADD3_CODE

  {
    SI opval = ADDSI (OPRND (sr), OPRND (slo16));
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* and: and $dr,$sr.  */

CIA
SEM_FN_NAME (m32rx,and) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_ADD_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_ADD_CODE

  {
    SI opval = ANDSI (OPRND (dr), OPRND (sr));
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* and3: and3 $dr,$sr,$uimm16.  */

CIA
SEM_FN_NAME (m32rx,and3) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_and3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_AND3_VARS /* f-op1 f-r1 f-op2 f-r2 f-uimm16 */
  EXTRACT_FMT_AND3_CODE

  {
    SI opval = ANDSI (OPRND (sr), OPRND (uimm16));
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* or: or $dr,$sr.  */

CIA
SEM_FN_NAME (m32rx,or) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_ADD_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_ADD_CODE

  {
    SI opval = ORSI (OPRND (dr), OPRND (sr));
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* or3: or3 $dr,$sr,$hash$ulo16.  */

CIA
SEM_FN_NAME (m32rx,or3) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_or3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_OR3_VARS /* f-op1 f-r1 f-op2 f-r2 f-uimm16 */
  EXTRACT_FMT_OR3_CODE

  {
    SI opval = ORSI (OPRND (sr), OPRND (ulo16));
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* xor: xor $dr,$sr.  */

CIA
SEM_FN_NAME (m32rx,xor) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_ADD_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_ADD_CODE

  {
    SI opval = XORSI (OPRND (dr), OPRND (sr));
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* xor3: xor3 $dr,$sr,$uimm16.  */

CIA
SEM_FN_NAME (m32rx,xor3) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_and3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_AND3_VARS /* f-op1 f-r1 f-op2 f-r2 f-uimm16 */
  EXTRACT_FMT_AND3_CODE

  {
    SI opval = XORSI (OPRND (sr), OPRND (uimm16));
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* addi: addi $dr,$simm8.  */

CIA
SEM_FN_NAME (m32rx,addi) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_addi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_ADDI_VARS /* f-op1 f-r1 f-simm8 */
  EXTRACT_FMT_ADDI_CODE

  {
    SI opval = ADDSI (OPRND (dr), OPRND (simm8));
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* addv: addv $dr,$sr.  */

CIA
SEM_FN_NAME (m32rx,addv) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_addv.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_ADDV_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_ADDV_CODE

do {
  UBI temp1;SI temp0;
  temp0 = ADDSI (OPRND (dr), OPRND (sr));
  temp1 = ADDOFSI (OPRND (dr), OPRND (sr), 0);
  {
    SI opval = temp0;
    CPU (h_gr[f_r1]) = opval;
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
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* addv3: addv3 $dr,$sr,$simm16.  */

CIA
SEM_FN_NAME (m32rx,addv3) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_addv3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_ADDV3_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_ADDV3_CODE

do {
  UBI temp1;SI temp0;
  temp0 = ADDSI (OPRND (sr), OPRND (simm16));
  temp1 = ADDOFSI (OPRND (sr), OPRND (simm16), 0);
  {
    SI opval = temp0;
    CPU (h_gr[f_r1]) = opval;
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
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* addx: addx $dr,$sr.  */

CIA
SEM_FN_NAME (m32rx,addx) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_addx.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_ADDX_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_ADDX_CODE

do {
  UBI temp1;SI temp0;
  temp0 = ADDCSI (OPRND (dr), OPRND (sr), OPRND (condbit));
  temp1 = ADDCFSI (OPRND (dr), OPRND (sr), OPRND (condbit));
  {
    SI opval = temp0;
    CPU (h_gr[f_r1]) = opval;
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
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* bc8: bc.s $disp8.  */

CIA
SEM_FN_NAME (m32rx,bc8) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_bc8.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  int taken_p = 0;
  CIA UNUSED pc = PC;
  EXTRACT_FMT_BC8_VARS /* f-op1 f-r1 f-disp8 */
  EXTRACT_FMT_BC8_CODE

if (OPRND (condbit)) {
  {
    USI opval = OPRND (disp8);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_ADDR (sem_arg, opval));
    taken_p = 1;
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef OPRND
}

/* bc24: bc.l $disp24.  */

CIA
SEM_FN_NAME (m32rx,bc24) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_bc24.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  int taken_p = 0;
  CIA UNUSED pc = PC;
  EXTRACT_FMT_BC24_VARS /* f-op1 f-r1 f-disp24 */
  EXTRACT_FMT_BC24_CODE

if (OPRND (condbit)) {
  {
    USI opval = OPRND (disp24);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_ADDR (sem_arg, opval));
    taken_p = 1;
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef OPRND
}

/* beq: beq $src1,$src2,$disp16.  */

CIA
SEM_FN_NAME (m32rx,beq) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_beq.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  int taken_p = 0;
  CIA UNUSED pc = PC;
  EXTRACT_FMT_BEQ_VARS /* f-op1 f-r1 f-op2 f-r2 f-disp16 */
  EXTRACT_FMT_BEQ_CODE

if (EQSI (OPRND (src1), OPRND (src2))) {
  {
    USI opval = OPRND (disp16);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_ADDR (sem_arg, opval));
    taken_p = 1;
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* beqz: beqz $src2,$disp16.  */

CIA
SEM_FN_NAME (m32rx,beqz) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_beqz.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  int taken_p = 0;
  CIA UNUSED pc = PC;
  EXTRACT_FMT_BEQZ_VARS /* f-op1 f-r1 f-op2 f-r2 f-disp16 */
  EXTRACT_FMT_BEQZ_CODE

if (EQSI (OPRND (src2), 0)) {
  {
    USI opval = OPRND (disp16);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_ADDR (sem_arg, opval));
    taken_p = 1;
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* bgez: bgez $src2,$disp16.  */

CIA
SEM_FN_NAME (m32rx,bgez) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_beqz.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  int taken_p = 0;
  CIA UNUSED pc = PC;
  EXTRACT_FMT_BEQZ_VARS /* f-op1 f-r1 f-op2 f-r2 f-disp16 */
  EXTRACT_FMT_BEQZ_CODE

if (GESI (OPRND (src2), 0)) {
  {
    USI opval = OPRND (disp16);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_ADDR (sem_arg, opval));
    taken_p = 1;
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* bgtz: bgtz $src2,$disp16.  */

CIA
SEM_FN_NAME (m32rx,bgtz) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_beqz.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  int taken_p = 0;
  CIA UNUSED pc = PC;
  EXTRACT_FMT_BEQZ_VARS /* f-op1 f-r1 f-op2 f-r2 f-disp16 */
  EXTRACT_FMT_BEQZ_CODE

if (GTSI (OPRND (src2), 0)) {
  {
    USI opval = OPRND (disp16);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_ADDR (sem_arg, opval));
    taken_p = 1;
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* blez: blez $src2,$disp16.  */

CIA
SEM_FN_NAME (m32rx,blez) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_beqz.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  int taken_p = 0;
  CIA UNUSED pc = PC;
  EXTRACT_FMT_BEQZ_VARS /* f-op1 f-r1 f-op2 f-r2 f-disp16 */
  EXTRACT_FMT_BEQZ_CODE

if (LESI (OPRND (src2), 0)) {
  {
    USI opval = OPRND (disp16);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_ADDR (sem_arg, opval));
    taken_p = 1;
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* bltz: bltz $src2,$disp16.  */

CIA
SEM_FN_NAME (m32rx,bltz) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_beqz.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  int taken_p = 0;
  CIA UNUSED pc = PC;
  EXTRACT_FMT_BEQZ_VARS /* f-op1 f-r1 f-op2 f-r2 f-disp16 */
  EXTRACT_FMT_BEQZ_CODE

if (LTSI (OPRND (src2), 0)) {
  {
    USI opval = OPRND (disp16);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_ADDR (sem_arg, opval));
    taken_p = 1;
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* bnez: bnez $src2,$disp16.  */

CIA
SEM_FN_NAME (m32rx,bnez) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_beqz.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  int taken_p = 0;
  CIA UNUSED pc = PC;
  EXTRACT_FMT_BEQZ_VARS /* f-op1 f-r1 f-op2 f-r2 f-disp16 */
  EXTRACT_FMT_BEQZ_CODE

if (NESI (OPRND (src2), 0)) {
  {
    USI opval = OPRND (disp16);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_ADDR (sem_arg, opval));
    taken_p = 1;
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* bl8: bl.s $disp8.  */

CIA
SEM_FN_NAME (m32rx,bl8) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_bl8.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  int taken_p = 0;
  CIA UNUSED pc = PC;
  EXTRACT_FMT_BL8_VARS /* f-op1 f-r1 f-disp8 */
  EXTRACT_FMT_BL8_CODE

do {
  {
    SI opval = ADDSI (ANDSI (OPRND (pc), -4), 4);
    CPU (h_gr[((HOSTUINT) 14)]) = opval;
    TRACE_RESULT (current_cpu, "gr-14", 'x', opval);
  }
  {
    USI opval = OPRND (disp8);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_ADDR (sem_arg, opval));
    taken_p = 1;
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
} while (0);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* bl24: bl.l $disp24.  */

CIA
SEM_FN_NAME (m32rx,bl24) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_bl24.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  int taken_p = 0;
  CIA UNUSED pc = PC;
  EXTRACT_FMT_BL24_VARS /* f-op1 f-r1 f-disp24 */
  EXTRACT_FMT_BL24_CODE

do {
  {
    SI opval = ADDSI (OPRND (pc), 4);
    CPU (h_gr[((HOSTUINT) 14)]) = opval;
    TRACE_RESULT (current_cpu, "gr-14", 'x', opval);
  }
  {
    USI opval = OPRND (disp24);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_ADDR (sem_arg, opval));
    taken_p = 1;
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
} while (0);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* bcl8: bcl.s $disp8.  */

CIA
SEM_FN_NAME (m32rx,bcl8) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_bcl8.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  int taken_p = 0;
  CIA UNUSED pc = PC;
  EXTRACT_FMT_BCL8_VARS /* f-op1 f-r1 f-disp8 */
  EXTRACT_FMT_BCL8_CODE

if (OPRND (condbit)) {
do {
  {
    SI opval = ADDSI (ANDSI (OPRND (pc), -4), 4);
    CPU (h_gr[((HOSTUINT) 14)]) = opval;
    TRACE_RESULT (current_cpu, "gr-14", 'x', opval);
  }
  {
    USI opval = OPRND (disp8);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_ADDR (sem_arg, opval));
    taken_p = 1;
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
} while (0);
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* bcl24: bcl.l $disp24.  */

CIA
SEM_FN_NAME (m32rx,bcl24) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_bcl24.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  int taken_p = 0;
  CIA UNUSED pc = PC;
  EXTRACT_FMT_BCL24_VARS /* f-op1 f-r1 f-disp24 */
  EXTRACT_FMT_BCL24_CODE

if (OPRND (condbit)) {
do {
  {
    SI opval = ADDSI (OPRND (pc), 4);
    CPU (h_gr[((HOSTUINT) 14)]) = opval;
    TRACE_RESULT (current_cpu, "gr-14", 'x', opval);
  }
  {
    USI opval = OPRND (disp24);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_ADDR (sem_arg, opval));
    taken_p = 1;
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
} while (0);
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* bnc8: bnc.s $disp8.  */

CIA
SEM_FN_NAME (m32rx,bnc8) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_bc8.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  int taken_p = 0;
  CIA UNUSED pc = PC;
  EXTRACT_FMT_BC8_VARS /* f-op1 f-r1 f-disp8 */
  EXTRACT_FMT_BC8_CODE

if (NOTBI (OPRND (condbit))) {
  {
    USI opval = OPRND (disp8);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_ADDR (sem_arg, opval));
    taken_p = 1;
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef OPRND
}

/* bnc24: bnc.l $disp24.  */

CIA
SEM_FN_NAME (m32rx,bnc24) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_bc24.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  int taken_p = 0;
  CIA UNUSED pc = PC;
  EXTRACT_FMT_BC24_VARS /* f-op1 f-r1 f-disp24 */
  EXTRACT_FMT_BC24_CODE

if (NOTBI (OPRND (condbit))) {
  {
    USI opval = OPRND (disp24);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_ADDR (sem_arg, opval));
    taken_p = 1;
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef OPRND
}

/* bne: bne $src1,$src2,$disp16.  */

CIA
SEM_FN_NAME (m32rx,bne) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_beq.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  int taken_p = 0;
  CIA UNUSED pc = PC;
  EXTRACT_FMT_BEQ_VARS /* f-op1 f-r1 f-op2 f-r2 f-disp16 */
  EXTRACT_FMT_BEQ_CODE

if (NESI (OPRND (src1), OPRND (src2))) {
  {
    USI opval = OPRND (disp16);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_ADDR (sem_arg, opval));
    taken_p = 1;
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* bra8: bra.s $disp8.  */

CIA
SEM_FN_NAME (m32rx,bra8) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_bra8.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  int taken_p = 0;
  CIA UNUSED pc = PC;
  EXTRACT_FMT_BRA8_VARS /* f-op1 f-r1 f-disp8 */
  EXTRACT_FMT_BRA8_CODE

  {
    USI opval = OPRND (disp8);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_ADDR (sem_arg, opval));
    taken_p = 1;
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef OPRND
}

/* bra24: bra.l $disp24.  */

CIA
SEM_FN_NAME (m32rx,bra24) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_bra24.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  int taken_p = 0;
  CIA UNUSED pc = PC;
  EXTRACT_FMT_BRA24_VARS /* f-op1 f-r1 f-disp24 */
  EXTRACT_FMT_BRA24_CODE

  {
    USI opval = OPRND (disp24);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_ADDR (sem_arg, opval));
    taken_p = 1;
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef OPRND
}

/* bncl8: bncl.s $disp8.  */

CIA
SEM_FN_NAME (m32rx,bncl8) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_bcl8.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  int taken_p = 0;
  CIA UNUSED pc = PC;
  EXTRACT_FMT_BCL8_VARS /* f-op1 f-r1 f-disp8 */
  EXTRACT_FMT_BCL8_CODE

if (NOTBI (OPRND (condbit))) {
do {
  {
    SI opval = ADDSI (ANDSI (OPRND (pc), -4), 4);
    CPU (h_gr[((HOSTUINT) 14)]) = opval;
    TRACE_RESULT (current_cpu, "gr-14", 'x', opval);
  }
  {
    USI opval = OPRND (disp8);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_ADDR (sem_arg, opval));
    taken_p = 1;
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
} while (0);
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* bncl24: bncl.l $disp24.  */

CIA
SEM_FN_NAME (m32rx,bncl24) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_bcl24.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  int taken_p = 0;
  CIA UNUSED pc = PC;
  EXTRACT_FMT_BCL24_VARS /* f-op1 f-r1 f-disp24 */
  EXTRACT_FMT_BCL24_CODE

if (NOTBI (OPRND (condbit))) {
do {
  {
    SI opval = ADDSI (OPRND (pc), 4);
    CPU (h_gr[((HOSTUINT) 14)]) = opval;
    TRACE_RESULT (current_cpu, "gr-14", 'x', opval);
  }
  {
    USI opval = OPRND (disp24);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_ADDR (sem_arg, opval));
    taken_p = 1;
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
} while (0);
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* cmp: cmp $src1,$src2.  */

CIA
SEM_FN_NAME (m32rx,cmp) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_cmp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_CMP_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_CMP_CODE

  {
    UBI opval = LTSI (OPRND (src1), OPRND (src2));
    CPU (h_cond) = opval;
    TRACE_RESULT (current_cpu, "condbit", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* cmpi: cmpi $src2,$simm16.  */

CIA
SEM_FN_NAME (m32rx,cmpi) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_cmpi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_CMPI_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_CMPI_CODE

  {
    UBI opval = LTSI (OPRND (src2), OPRND (simm16));
    CPU (h_cond) = opval;
    TRACE_RESULT (current_cpu, "condbit", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* cmpu: cmpu $src1,$src2.  */

CIA
SEM_FN_NAME (m32rx,cmpu) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_cmp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_CMP_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_CMP_CODE

  {
    UBI opval = LTUSI (OPRND (src1), OPRND (src2));
    CPU (h_cond) = opval;
    TRACE_RESULT (current_cpu, "condbit", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* cmpui: cmpui $src2,$simm16.  */

CIA
SEM_FN_NAME (m32rx,cmpui) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_cmpi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_CMPI_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_CMPI_CODE

  {
    UBI opval = LTUSI (OPRND (src2), OPRND (simm16));
    CPU (h_cond) = opval;
    TRACE_RESULT (current_cpu, "condbit", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* cmpeq: cmpeq $src1,$src2.  */

CIA
SEM_FN_NAME (m32rx,cmpeq) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_cmp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_CMP_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_CMP_CODE

  {
    UBI opval = EQSI (OPRND (src1), OPRND (src2));
    CPU (h_cond) = opval;
    TRACE_RESULT (current_cpu, "condbit", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* cmpz: cmpz $src2.  */

CIA
SEM_FN_NAME (m32rx,cmpz) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_cmpz.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_CMPZ_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_CMPZ_CODE

  {
    UBI opval = EQSI (OPRND (src2), 0);
    CPU (h_cond) = opval;
    TRACE_RESULT (current_cpu, "condbit", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* div: div $dr,$sr.  */

CIA
SEM_FN_NAME (m32rx,div) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_div.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_DIV_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_DIV_CODE

if (NESI (OPRND (sr), 0)) {
  {
    SI opval = DIVSI (OPRND (dr), OPRND (sr));
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* divu: divu $dr,$sr.  */

CIA
SEM_FN_NAME (m32rx,divu) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_div.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_DIV_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_DIV_CODE

if (NESI (OPRND (sr), 0)) {
  {
    SI opval = UDIVSI (OPRND (dr), OPRND (sr));
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* rem: rem $dr,$sr.  */

CIA
SEM_FN_NAME (m32rx,rem) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_div.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_DIV_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_DIV_CODE

if (NESI (OPRND (sr), 0)) {
  {
    SI opval = MODSI (OPRND (dr), OPRND (sr));
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* remu: remu $dr,$sr.  */

CIA
SEM_FN_NAME (m32rx,remu) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_div.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_DIV_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_DIV_CODE

if (NESI (OPRND (sr), 0)) {
  {
    SI opval = UMODSI (OPRND (dr), OPRND (sr));
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* divh: divh $dr,$sr.  */

CIA
SEM_FN_NAME (m32rx,divh) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_div.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_DIV_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_DIV_CODE

if (NESI (OPRND (sr), 0)) {
  {
    SI opval = DIVSI (EXTHISI (TRUNCSIHI (OPRND (dr))), OPRND (sr));
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* jc: jc $sr.  */

CIA
SEM_FN_NAME (m32rx,jc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_jc.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  int taken_p = 0;
  CIA UNUSED pc = PC;
  EXTRACT_FMT_JC_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_JC_CODE

if (OPRND (condbit)) {
  {
    USI opval = ANDSI (OPRND (sr), -4);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_ADDR (sem_arg, opval));
    taken_p = 1;
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* jnc: jnc $sr.  */

CIA
SEM_FN_NAME (m32rx,jnc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_jc.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  int taken_p = 0;
  CIA UNUSED pc = PC;
  EXTRACT_FMT_JC_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_JC_CODE

if (NOTBI (OPRND (condbit))) {
  {
    USI opval = ANDSI (OPRND (sr), -4);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_ADDR (sem_arg, opval));
    taken_p = 1;
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* jl: jl $sr.  */

CIA
SEM_FN_NAME (m32rx,jl) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_jl.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  int taken_p = 0;
  CIA UNUSED pc = PC;
  EXTRACT_FMT_JL_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_JL_CODE

do {
  USI temp1;SI temp0;
  temp0 = ADDSI (ANDSI (OPRND (pc), -4), 4);
  temp1 = ANDSI (OPRND (sr), -4);
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
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef OPRND
}

/* jmp: jmp $sr.  */

CIA
SEM_FN_NAME (m32rx,jmp) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_jmp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  int taken_p = 0;
  CIA UNUSED pc = PC;
  EXTRACT_FMT_JMP_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_JMP_CODE

  {
    USI opval = ANDSI (OPRND (sr), -4);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_ADDR (sem_arg, opval));
    taken_p = 1;
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* ld: ld $dr,@$sr.  */

CIA
SEM_FN_NAME (m32rx,ld) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_ld.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_LD_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_LD_CODE

  {
    SI opval = OPRND (h_memory_sr);
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* ld-d: ld $dr,@($slo16,$sr).  */

CIA
SEM_FN_NAME (m32rx,ld_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_ld_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_LD_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_LD_D_CODE

  {
    SI opval = OPRND (h_memory_add__VM_sr_slo16);
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* ldb: ldb $dr,@$sr.  */

CIA
SEM_FN_NAME (m32rx,ldb) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_ldb.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_LDB_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_LDB_CODE

  {
    SI opval = EXTQISI (OPRND (h_memory_sr));
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* ldb-d: ldb $dr,@($slo16,$sr).  */

CIA
SEM_FN_NAME (m32rx,ldb_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_ldb_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_LDB_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_LDB_D_CODE

  {
    SI opval = EXTQISI (OPRND (h_memory_add__VM_sr_slo16));
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* ldh: ldh $dr,@$sr.  */

CIA
SEM_FN_NAME (m32rx,ldh) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_ldh.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_LDH_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_LDH_CODE

  {
    SI opval = EXTHISI (OPRND (h_memory_sr));
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* ldh-d: ldh $dr,@($slo16,$sr).  */

CIA
SEM_FN_NAME (m32rx,ldh_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_ldh_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_LDH_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_LDH_D_CODE

  {
    SI opval = EXTHISI (OPRND (h_memory_add__VM_sr_slo16));
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* ldub: ldub $dr,@$sr.  */

CIA
SEM_FN_NAME (m32rx,ldub) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_ldb.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_LDB_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_LDB_CODE

  {
    SI opval = ZEXTQISI (OPRND (h_memory_sr));
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* ldub-d: ldub $dr,@($slo16,$sr).  */

CIA
SEM_FN_NAME (m32rx,ldub_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_ldb_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_LDB_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_LDB_D_CODE

  {
    SI opval = ZEXTQISI (OPRND (h_memory_add__VM_sr_slo16));
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* lduh: lduh $dr,@$sr.  */

CIA
SEM_FN_NAME (m32rx,lduh) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_ldh.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_LDH_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_LDH_CODE

  {
    SI opval = ZEXTHISI (OPRND (h_memory_sr));
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* lduh-d: lduh $dr,@($slo16,$sr).  */

CIA
SEM_FN_NAME (m32rx,lduh_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_ldh_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_LDH_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_LDH_D_CODE

  {
    SI opval = ZEXTHISI (OPRND (h_memory_add__VM_sr_slo16));
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* ld-plus: ld $dr,@$sr+.  */

CIA
SEM_FN_NAME (m32rx,ld_plus) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_ld_plus.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_LD_PLUS_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_LD_PLUS_CODE

do {
  SI temp1;SI temp0;
  temp0 = OPRND (h_memory_sr);
  temp1 = ADDSI (OPRND (sr), 4);
  {
    SI opval = temp0;
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }
  {
    SI opval = temp1;
    CPU (h_gr[f_r2]) = opval;
    TRACE_RESULT (current_cpu, "sr", 'x', opval);
  }
} while (0);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* ld24: ld24 $dr,$uimm24.  */

CIA
SEM_FN_NAME (m32rx,ld24) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_ld24.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_LD24_VARS /* f-op1 f-r1 f-uimm24 */
  EXTRACT_FMT_LD24_CODE

  {
    SI opval = OPRND (uimm24);
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* ldi8: ldi8 $dr,$simm8.  */

CIA
SEM_FN_NAME (m32rx,ldi8) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_ldi8.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_LDI8_VARS /* f-op1 f-r1 f-simm8 */
  EXTRACT_FMT_LDI8_CODE

  {
    SI opval = OPRND (simm8);
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* ldi16: ldi16 $dr,$hash$slo16.  */

CIA
SEM_FN_NAME (m32rx,ldi16) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_ldi16.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_LDI16_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_LDI16_CODE

  {
    SI opval = OPRND (slo16);
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* lock: lock $dr,@$sr.  */

CIA
SEM_FN_NAME (m32rx,lock) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_lock.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_LOCK_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_LOCK_CODE

do {
  {
    UBI opval = 1;
    CPU (h_lock) = opval;
    TRACE_RESULT (current_cpu, "lock-0", 'x', opval);
  }
  {
    SI opval = OPRND (h_memory_sr);
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }
} while (0);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* machi-a: machi $src1,$src2,$acc.  */

CIA
SEM_FN_NAME (m32rx,machi_a) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_machi_a.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_MACHI_A_VARS /* f-op1 f-r1 f-acc f-op23 f-r2 */
  EXTRACT_FMT_MACHI_A_CODE

  {
    DI opval = SRADI (SLLDI (ADDDI (OPRND (acc), MULDI (EXTSIDI (ANDSI (OPRND (src1), 0xffff0000)), EXTHIDI (TRUNCSIHI (SRASI (OPRND (src2), 16))))), 8), 8);
    m32rx_h_accums_set (current_cpu, f_acc, opval);
    TRACE_RESULT (current_cpu, "acc", 'D', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* maclo-a: maclo $src1,$src2,$acc.  */

CIA
SEM_FN_NAME (m32rx,maclo_a) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_machi_a.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_MACHI_A_VARS /* f-op1 f-r1 f-acc f-op23 f-r2 */
  EXTRACT_FMT_MACHI_A_CODE

  {
    DI opval = SRADI (SLLDI (ADDDI (OPRND (acc), MULDI (EXTSIDI (SLLSI (OPRND (src1), 16)), EXTHIDI (TRUNCSIHI (OPRND (src2))))), 8), 8);
    m32rx_h_accums_set (current_cpu, f_acc, opval);
    TRACE_RESULT (current_cpu, "acc", 'D', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* macwhi: macwhi $src1,$src2.  */

CIA
SEM_FN_NAME (m32rx,macwhi) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_macwhi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_MACWHI_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_MACWHI_CODE

  {
    DI opval = SRADI (SLLDI (ADDDI (OPRND (accum), MULDI (EXTSIDI (OPRND (src1)), EXTHIDI (TRUNCSIHI (SRASI (OPRND (src2), 16))))), 8), 8);
    m32rx_h_accum_set (current_cpu, opval);
    TRACE_RESULT (current_cpu, "accum", 'D', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* macwlo: macwlo $src1,$src2.  */

CIA
SEM_FN_NAME (m32rx,macwlo) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_macwhi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_MACWHI_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_MACWHI_CODE

  {
    DI opval = SRADI (SLLDI (ADDDI (OPRND (accum), MULDI (EXTSIDI (OPRND (src1)), EXTHIDI (TRUNCSIHI (OPRND (src2))))), 8), 8);
    m32rx_h_accum_set (current_cpu, opval);
    TRACE_RESULT (current_cpu, "accum", 'D', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* mul: mul $dr,$sr.  */

CIA
SEM_FN_NAME (m32rx,mul) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_ADD_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_ADD_CODE

  {
    SI opval = MULSI (OPRND (dr), OPRND (sr));
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* mulhi-a: mulhi $src1,$src2,$acc.  */

CIA
SEM_FN_NAME (m32rx,mulhi_a) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_mulhi_a.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_MULHI_A_VARS /* f-op1 f-r1 f-acc f-op23 f-r2 */
  EXTRACT_FMT_MULHI_A_CODE

  {
    DI opval = SRADI (SLLDI (MULDI (EXTSIDI (ANDSI (OPRND (src1), 0xffff0000)), EXTHIDI (TRUNCSIHI (SRASI (OPRND (src2), 16)))), 16), 16);
    m32rx_h_accums_set (current_cpu, f_acc, opval);
    TRACE_RESULT (current_cpu, "acc", 'D', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* mullo-a: mullo $src1,$src2,$acc.  */

CIA
SEM_FN_NAME (m32rx,mullo_a) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_mulhi_a.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_MULHI_A_VARS /* f-op1 f-r1 f-acc f-op23 f-r2 */
  EXTRACT_FMT_MULHI_A_CODE

  {
    DI opval = SRADI (SLLDI (MULDI (EXTSIDI (SLLSI (OPRND (src1), 16)), EXTHIDI (TRUNCSIHI (OPRND (src2)))), 16), 16);
    m32rx_h_accums_set (current_cpu, f_acc, opval);
    TRACE_RESULT (current_cpu, "acc", 'D', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* mulwhi: mulwhi $src1,$src2.  */

CIA
SEM_FN_NAME (m32rx,mulwhi) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_mulwhi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_MULWHI_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_MULWHI_CODE

  {
    DI opval = SRADI (SLLDI (MULDI (EXTSIDI (OPRND (src1)), EXTHIDI (TRUNCSIHI (SRASI (OPRND (src2), 16)))), 8), 8);
    m32rx_h_accum_set (current_cpu, opval);
    TRACE_RESULT (current_cpu, "accum", 'D', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* mulwlo: mulwlo $src1,$src2.  */

CIA
SEM_FN_NAME (m32rx,mulwlo) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_mulwhi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_MULWHI_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_MULWHI_CODE

  {
    DI opval = SRADI (SLLDI (MULDI (EXTSIDI (OPRND (src1)), EXTHIDI (TRUNCSIHI (OPRND (src2)))), 8), 8);
    m32rx_h_accum_set (current_cpu, opval);
    TRACE_RESULT (current_cpu, "accum", 'D', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* mv: mv $dr,$sr.  */

CIA
SEM_FN_NAME (m32rx,mv) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_mv.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_MV_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_MV_CODE

  {
    SI opval = OPRND (sr);
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* mvfachi-a: mvfachi $dr,$accs.  */

CIA
SEM_FN_NAME (m32rx,mvfachi_a) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_mvfachi_a.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_MVFACHI_A_VARS /* f-op1 f-r1 f-op2 f-accs f-op3 */
  EXTRACT_FMT_MVFACHI_A_CODE

  {
    SI opval = TRUNCDISI (SRADI (OPRND (accs), 32));
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* mvfaclo-a: mvfaclo $dr,$accs.  */

CIA
SEM_FN_NAME (m32rx,mvfaclo_a) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_mvfachi_a.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_MVFACHI_A_VARS /* f-op1 f-r1 f-op2 f-accs f-op3 */
  EXTRACT_FMT_MVFACHI_A_CODE

  {
    SI opval = TRUNCDISI (OPRND (accs));
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* mvfacmi-a: mvfacmi $dr,$accs.  */

CIA
SEM_FN_NAME (m32rx,mvfacmi_a) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_mvfachi_a.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_MVFACHI_A_VARS /* f-op1 f-r1 f-op2 f-accs f-op3 */
  EXTRACT_FMT_MVFACHI_A_CODE

  {
    SI opval = TRUNCDISI (SRADI (OPRND (accs), 16));
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* mvfc: mvfc $dr,$scr.  */

CIA
SEM_FN_NAME (m32rx,mvfc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_mvfc.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_MVFC_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_MVFC_CODE

  {
    SI opval = OPRND (scr);
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* mvtachi-a: mvtachi $src1,$accs.  */

CIA
SEM_FN_NAME (m32rx,mvtachi_a) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_mvtachi_a.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_MVTACHI_A_VARS /* f-op1 f-r1 f-op2 f-accs f-op3 */
  EXTRACT_FMT_MVTACHI_A_CODE

  {
    DI opval = ORDI (ANDDI (OPRND (accs), MAKEDI (0, 0xffffffff)), SLLDI (EXTSIDI (OPRND (src1)), 32));
    m32rx_h_accums_set (current_cpu, f_accs, opval);
    TRACE_RESULT (current_cpu, "accs", 'D', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* mvtaclo-a: mvtaclo $src1,$accs.  */

CIA
SEM_FN_NAME (m32rx,mvtaclo_a) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_mvtachi_a.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_MVTACHI_A_VARS /* f-op1 f-r1 f-op2 f-accs f-op3 */
  EXTRACT_FMT_MVTACHI_A_CODE

  {
    DI opval = ORDI (ANDDI (OPRND (accs), MAKEDI (0xffffffff, 0)), ZEXTSIDI (OPRND (src1)));
    m32rx_h_accums_set (current_cpu, f_accs, opval);
    TRACE_RESULT (current_cpu, "accs", 'D', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* mvtc: mvtc $sr,$dcr.  */

CIA
SEM_FN_NAME (m32rx,mvtc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_mvtc.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_MVTC_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_MVTC_CODE

  {
    USI opval = OPRND (sr);
    m32rx_h_cr_set (current_cpu, f_r1, opval);
    TRACE_RESULT (current_cpu, "dcr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* neg: neg $dr,$sr.  */

CIA
SEM_FN_NAME (m32rx,neg) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_mv.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_MV_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_MV_CODE

  {
    SI opval = NEGSI (OPRND (sr));
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* nop: nop.  */

CIA
SEM_FN_NAME (m32rx,nop) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_nop.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_NOP_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_NOP_CODE

PROFILE_COUNT_FILLNOPS (current_cpu, abuf->addr);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* not: not $dr,$sr.  */

CIA
SEM_FN_NAME (m32rx,not) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_mv.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_MV_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_MV_CODE

  {
    SI opval = INVSI (OPRND (sr));
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* rac-dsi: rac $accd,$accs,$imm1.  */

CIA
SEM_FN_NAME (m32rx,rac_dsi) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_rac_dsi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_RAC_DSI_VARS /* f-op1 f-accd f-bits67 f-op2 f-accs f-bit14 f-imm1 */
  EXTRACT_FMT_RAC_DSI_CODE

do {
  DI tmp_tmp1;
  tmp_tmp1 = SLLDI (OPRND (accs), OPRND (imm1));
  tmp_tmp1 = ADDDI (tmp_tmp1, MAKEDI (0, 32768));
  {
    DI opval = (GTDI (tmp_tmp1, MAKEDI (32767, 0xffff0000))) ? (MAKEDI (32767, 0xffff0000)) : (LTDI (tmp_tmp1, MAKEDI (0xffff8000, 0))) ? (MAKEDI (0xffff8000, 0)) : (ANDDI (tmp_tmp1, MAKEDI (0xffffffff, 0xffff0000)));
    m32rx_h_accums_set (current_cpu, f_accd, opval);
    TRACE_RESULT (current_cpu, "accd", 'D', opval);
  }
} while (0);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* rach-dsi: rach $accd,$accs,$imm1.  */

CIA
SEM_FN_NAME (m32rx,rach_dsi) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_rac_dsi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_RAC_DSI_VARS /* f-op1 f-accd f-bits67 f-op2 f-accs f-bit14 f-imm1 */
  EXTRACT_FMT_RAC_DSI_CODE

do {
  DI tmp_tmp1;
  tmp_tmp1 = SLLDI (OPRND (accs), OPRND (imm1));
  tmp_tmp1 = ADDDI (tmp_tmp1, MAKEDI (0, 0x80000000));
  {
    DI opval = (GTDI (tmp_tmp1, MAKEDI (32767, 0))) ? (MAKEDI (32767, 0)) : (LTDI (tmp_tmp1, MAKEDI (0xffff8000, 0))) ? (MAKEDI (0xffff8000, 0)) : (ANDDI (tmp_tmp1, MAKEDI (0xffffffff, 0)));
    m32rx_h_accums_set (current_cpu, f_accd, opval);
    TRACE_RESULT (current_cpu, "accd", 'D', opval);
  }
} while (0);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* rte: rte.  */

CIA
SEM_FN_NAME (m32rx,rte) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_rte.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  int taken_p = 0;
  CIA UNUSED pc = PC;
  EXTRACT_FMT_RTE_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_RTE_CODE

do {
  {
    UBI opval = OPRND (h_bsm_0);
    CPU (h_sm) = opval;
    TRACE_RESULT (current_cpu, "sm-0", 'x', opval);
  }
  {
    UBI opval = OPRND (h_bie_0);
    CPU (h_ie) = opval;
    TRACE_RESULT (current_cpu, "ie-0", 'x', opval);
  }
  {
    UBI opval = OPRND (h_bcond_0);
    CPU (h_cond) = opval;
    TRACE_RESULT (current_cpu, "condbit", 'x', opval);
  }
  {
    USI opval = ANDSI (OPRND (h_bpc_0), -4);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_ADDR (sem_arg, opval));
    taken_p = 1;
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
} while (0);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef OPRND
}

/* seth: seth $dr,$hash$hi16.  */

CIA
SEM_FN_NAME (m32rx,seth) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_seth.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_SETH_VARS /* f-op1 f-r1 f-op2 f-r2 f-hi16 */
  EXTRACT_FMT_SETH_CODE

  {
    SI opval = SLLSI (OPRND (hi16), 16);
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* sll: sll $dr,$sr.  */

CIA
SEM_FN_NAME (m32rx,sll) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_ADD_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_ADD_CODE

  {
    SI opval = SLLSI (OPRND (dr), ANDSI (OPRND (sr), 31));
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* sll3: sll3 $dr,$sr,$simm16.  */

CIA
SEM_FN_NAME (m32rx,sll3) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_sll3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_SLL3_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_SLL3_CODE

  {
    SI opval = SLLSI (OPRND (sr), ANDSI (OPRND (simm16), 31));
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* slli: slli $dr,$uimm5.  */

CIA
SEM_FN_NAME (m32rx,slli) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_slli.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_SLLI_VARS /* f-op1 f-r1 f-shift-op2 f-uimm5 */
  EXTRACT_FMT_SLLI_CODE

  {
    SI opval = SLLSI (OPRND (dr), OPRND (uimm5));
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* sra: sra $dr,$sr.  */

CIA
SEM_FN_NAME (m32rx,sra) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_ADD_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_ADD_CODE

  {
    SI opval = SRASI (OPRND (dr), ANDSI (OPRND (sr), 31));
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* sra3: sra3 $dr,$sr,$simm16.  */

CIA
SEM_FN_NAME (m32rx,sra3) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_sll3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_SLL3_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_SLL3_CODE

  {
    SI opval = SRASI (OPRND (sr), ANDSI (OPRND (simm16), 31));
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* srai: srai $dr,$uimm5.  */

CIA
SEM_FN_NAME (m32rx,srai) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_slli.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_SLLI_VARS /* f-op1 f-r1 f-shift-op2 f-uimm5 */
  EXTRACT_FMT_SLLI_CODE

  {
    SI opval = SRASI (OPRND (dr), OPRND (uimm5));
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* srl: srl $dr,$sr.  */

CIA
SEM_FN_NAME (m32rx,srl) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_ADD_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_ADD_CODE

  {
    SI opval = SRLSI (OPRND (dr), ANDSI (OPRND (sr), 31));
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* srl3: srl3 $dr,$sr,$simm16.  */

CIA
SEM_FN_NAME (m32rx,srl3) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_sll3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_SLL3_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_SLL3_CODE

  {
    SI opval = SRLSI (OPRND (sr), ANDSI (OPRND (simm16), 31));
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* srli: srli $dr,$uimm5.  */

CIA
SEM_FN_NAME (m32rx,srli) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_slli.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_SLLI_VARS /* f-op1 f-r1 f-shift-op2 f-uimm5 */
  EXTRACT_FMT_SLLI_CODE

  {
    SI opval = SRLSI (OPRND (dr), OPRND (uimm5));
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* st: st $src1,@$src2.  */

CIA
SEM_FN_NAME (m32rx,st) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_st.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_ST_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_ST_CODE

  {
    SI opval = OPRND (src1);
    SETMEMSI (current_cpu, OPRND (src2), opval);
    TRACE_RESULT (current_cpu, "memory", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* st-d: st $src1,@($slo16,$src2).  */

CIA
SEM_FN_NAME (m32rx,st_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_st_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_ST_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_ST_D_CODE

  {
    SI opval = OPRND (src1);
    SETMEMSI (current_cpu, ADDSI (OPRND (src2), OPRND (slo16)), opval);
    TRACE_RESULT (current_cpu, "memory", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* stb: stb $src1,@$src2.  */

CIA
SEM_FN_NAME (m32rx,stb) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_stb.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_STB_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_STB_CODE

  {
    QI opval = OPRND (src1);
    SETMEMQI (current_cpu, OPRND (src2), opval);
    TRACE_RESULT (current_cpu, "memory", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* stb-d: stb $src1,@($slo16,$src2).  */

CIA
SEM_FN_NAME (m32rx,stb_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_stb_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_STB_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_STB_D_CODE

  {
    QI opval = OPRND (src1);
    SETMEMQI (current_cpu, ADDSI (OPRND (src2), OPRND (slo16)), opval);
    TRACE_RESULT (current_cpu, "memory", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* sth: sth $src1,@$src2.  */

CIA
SEM_FN_NAME (m32rx,sth) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_sth.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_STH_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_STH_CODE

  {
    HI opval = OPRND (src1);
    SETMEMHI (current_cpu, OPRND (src2), opval);
    TRACE_RESULT (current_cpu, "memory", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* sth-d: sth $src1,@($slo16,$src2).  */

CIA
SEM_FN_NAME (m32rx,sth_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_sth_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_STH_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */
  EXTRACT_FMT_STH_D_CODE

  {
    HI opval = OPRND (src1);
    SETMEMHI (current_cpu, ADDSI (OPRND (src2), OPRND (slo16)), opval);
    TRACE_RESULT (current_cpu, "memory", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* st-plus: st $src1,@+$src2.  */

CIA
SEM_FN_NAME (m32rx,st_plus) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_st_plus.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_ST_PLUS_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_ST_PLUS_CODE

do {
  SI tmp_new_src2;
  tmp_new_src2 = ADDSI (OPRND (src2), 4);
  {
    SI opval = OPRND (src1);
    SETMEMSI (current_cpu, tmp_new_src2, opval);
    TRACE_RESULT (current_cpu, "memory", 'x', opval);
  }
  {
    SI opval = tmp_new_src2;
    CPU (h_gr[f_r2]) = opval;
    TRACE_RESULT (current_cpu, "src2", 'x', opval);
  }
} while (0);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* st-minus: st $src1,@-$src2.  */

CIA
SEM_FN_NAME (m32rx,st_minus) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_st_plus.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_ST_PLUS_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_ST_PLUS_CODE

do {
  SI tmp_new_src2;
  tmp_new_src2 = SUBSI (OPRND (src2), 4);
  {
    SI opval = OPRND (src1);
    SETMEMSI (current_cpu, tmp_new_src2, opval);
    TRACE_RESULT (current_cpu, "memory", 'x', opval);
  }
  {
    SI opval = tmp_new_src2;
    CPU (h_gr[f_r2]) = opval;
    TRACE_RESULT (current_cpu, "src2", 'x', opval);
  }
} while (0);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* sub: sub $dr,$sr.  */

CIA
SEM_FN_NAME (m32rx,sub) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_ADD_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_ADD_CODE

  {
    SI opval = SUBSI (OPRND (dr), OPRND (sr));
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* subv: subv $dr,$sr.  */

CIA
SEM_FN_NAME (m32rx,subv) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_addv.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_ADDV_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_ADDV_CODE

do {
  UBI temp1;SI temp0;
  temp0 = SUBSI (OPRND (dr), OPRND (sr));
  temp1 = SUBOFSI (OPRND (dr), OPRND (sr), 0);
  {
    SI opval = temp0;
    CPU (h_gr[f_r1]) = opval;
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
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* subx: subx $dr,$sr.  */

CIA
SEM_FN_NAME (m32rx,subx) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_addx.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_ADDX_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_ADDX_CODE

do {
  UBI temp1;SI temp0;
  temp0 = SUBCSI (OPRND (dr), OPRND (sr), OPRND (condbit));
  temp1 = SUBCFSI (OPRND (dr), OPRND (sr), OPRND (condbit));
  {
    SI opval = temp0;
    CPU (h_gr[f_r1]) = opval;
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
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_mark_set_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* trap: trap $uimm4.  */

CIA
SEM_FN_NAME (m32rx,trap) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_trap.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  int taken_p = 0;
  CIA UNUSED pc = PC;
  EXTRACT_FMT_TRAP_VARS /* f-op1 f-r1 f-op2 f-uimm4 */
  EXTRACT_FMT_TRAP_CODE

do {
  {
    USI opval = ADDSI (OPRND (pc), 4);
    m32rx_h_cr_set (current_cpu, ((HOSTUINT) 6), opval);
    TRACE_RESULT (current_cpu, "cr-6", 'x', opval);
  }
  {
    USI opval = ANDSI (SLLSI (OPRND (h_cr_0), 8), 65408);
    m32rx_h_cr_set (current_cpu, ((HOSTUINT) 0), opval);
    TRACE_RESULT (current_cpu, "cr-0", 'x', opval);
  }
  {
    SI opval = m32r_trap (current_cpu, OPRND (uimm4));
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_ADDR (sem_arg, opval));
    taken_p = 1;
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
} while (0);

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_profile_cti_insn (current_cpu, abuf, taken_p);
    }
#endif

  return new_pc;
#undef OPRND
}

/* unlock: unlock $src1,@$src2.  */

CIA
SEM_FN_NAME (m32rx,unlock) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_unlock.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_UNLOCK_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_UNLOCK_CODE

do {
if (OPRND (h_lock_0)) {
  {
    SI opval = OPRND (src1);
    SETMEMSI (current_cpu, OPRND (src2), opval);
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
      m32rx_model_mark_get_h_gr (current_cpu, abuf);
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* satb: satb $dr,$sr.  */

CIA
SEM_FN_NAME (m32rx,satb) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_satb.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_SATB_VARS /* f-op1 f-r1 f-op2 f-r2 f-uimm16 */
  EXTRACT_FMT_SATB_CODE

  {
    SI opval = (GESI (OPRND (sr), 127)) ? (127) : (LESI (OPRND (sr), -128)) ? (-128) : (OPRND (sr));
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* sath: sath $dr,$sr.  */

CIA
SEM_FN_NAME (m32rx,sath) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_satb.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_SATB_VARS /* f-op1 f-r1 f-op2 f-r2 f-uimm16 */
  EXTRACT_FMT_SATB_CODE

  {
    SI opval = (GESI (OPRND (sr), 32767)) ? (32767) : (LESI (OPRND (sr), -32768)) ? (-32768) : (OPRND (sr));
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* sat: sat $dr,$sr.  */

CIA
SEM_FN_NAME (m32rx,sat) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_sat.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 4);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_SAT_VARS /* f-op1 f-r1 f-op2 f-r2 f-uimm16 */
  EXTRACT_FMT_SAT_CODE

  {
    SI opval = ((OPRND (condbit)) ? (((LTSI (OPRND (sr), 0)) ? (2147483647) : (0x80000000))) : (OPRND (sr)));
    CPU (h_gr[f_r1]) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* pcmpbz: pcmpbz $src2.  */

CIA
SEM_FN_NAME (m32rx,pcmpbz) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_cmpz.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_CMPZ_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_CMPZ_CODE

  {
    UBI opval = (EQSI (ANDSI (OPRND (src2), 255), 0)) ? (1) : (EQSI (ANDSI (OPRND (src2), 65280), 0)) ? (1) : (EQSI (ANDSI (OPRND (src2), 16711680), 0)) ? (1) : (EQSI (ANDSI (OPRND (src2), 0xff000000), 0)) ? (1) : (0);
    CPU (h_cond) = opval;
    TRACE_RESULT (current_cpu, "condbit", 'x', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* sadd: sadd.  */

CIA
SEM_FN_NAME (m32rx,sadd) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_sadd.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_SADD_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_SADD_CODE

  {
    DI opval = ADDDI (SRADI (OPRND (h_accums_1), 16), OPRND (h_accums_0));
    m32rx_h_accums_set (current_cpu, ((HOSTUINT) 0), opval);
    TRACE_RESULT (current_cpu, "accums-0", 'D', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* macwu1: macwu1 $src1,$src2.  */

CIA
SEM_FN_NAME (m32rx,macwu1) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_macwu1.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_MACWU1_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_MACWU1_CODE

  {
    DI opval = SRADI (SLLDI (ADDDI (OPRND (h_accums_1), MULDI (EXTSIDI (OPRND (src1)), EXTSIDI (ANDSI (OPRND (src2), 65535)))), 8), 8);
    m32rx_h_accums_set (current_cpu, ((HOSTUINT) 1), opval);
    TRACE_RESULT (current_cpu, "accums-1", 'D', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* msblo: msblo $src1,$src2.  */

CIA
SEM_FN_NAME (m32rx,msblo) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_macwhi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_MACWHI_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_MACWHI_CODE

  {
    DI opval = SRADI (SLLDI (SUBDI (OPRND (accum), SRADI (SLLDI (MULDI (EXTHIDI (TRUNCSIHI (OPRND (src1))), EXTHIDI (TRUNCSIHI (OPRND (src2)))), 32), 16)), 8), 8);
    m32rx_h_accum_set (current_cpu, opval);
    TRACE_RESULT (current_cpu, "accum", 'D', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* mulwu1: mulwu1 $src1,$src2.  */

CIA
SEM_FN_NAME (m32rx,mulwu1) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_mulwu1.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_MULWU1_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_MULWU1_CODE

  {
    DI opval = SRADI (SLLDI (MULDI (EXTSIDI (OPRND (src1)), EXTSIDI (ANDSI (OPRND (src2), 65535))), 16), 16);
    m32rx_h_accums_set (current_cpu, ((HOSTUINT) 1), opval);
    TRACE_RESULT (current_cpu, "accums-1", 'D', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* maclh1: maclh1 $src1,$src2.  */

CIA
SEM_FN_NAME (m32rx,maclh1) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_macwu1.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_MACWU1_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_MACWU1_CODE

  {
    DI opval = SRADI (SLLDI (ADDDI (OPRND (h_accums_1), SLLDI (EXTSIDI (MULSI (EXTHISI (TRUNCSIHI (OPRND (src1))), SRASI (OPRND (src2), 16))), 16)), 8), 8);
    m32rx_h_accums_set (current_cpu, ((HOSTUINT) 1), opval);
    TRACE_RESULT (current_cpu, "accums-1", 'D', opval);
  }

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

/* sc: sc.  */

CIA
SEM_FN_NAME (m32rx,sc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_sc.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_SC_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_SC_CODE

if (OPRND (condbit)) {
BRANCH_NEW_PC (new_pc, NEW_PC_SKIP);
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

#if WITH_PROFILE_MODEL_P
  if (PROFILE_MODEL_P (current_cpu))
    {
      m32rx_model_profile_insn (current_cpu, abuf);
    }
#endif

  return new_pc;
#undef OPRND
}

/* snc: snc.  */

CIA
SEM_FN_NAME (m32rx,snc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, PAREXEC *par_exec)
{
  insn_t insn = SEM_INSN (sem_arg);
#define OPRND(f) par_exec->operands.fmt_sc.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc = SEM_NEXT_PC (sem_arg, 2);
  CIA UNUSED pc = PC;
  EXTRACT_FMT_SC_VARS /* f-op1 f-r1 f-op2 f-r2 */
  EXTRACT_FMT_SC_CODE

if (NOTBI (OPRND (condbit))) {
BRANCH_NEW_PC (new_pc, NEW_PC_SKIP);
}

  PROFILE_COUNT_INSN (current_cpu, 0, abuf->idesc->num);

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

#endif /* WANT_CPU */
