/* Simulator instruction semantics for m32rbf.

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
#define WANT_CPU_M32RBF

#include "sim-main.h"
#include "cgen-mem.h"
#include "cgen-ops.h"

#undef GET_ATTR
#define GET_ATTR(cpu, num, attr) CGEN_INSN_ATTR (abuf->idesc->opcode, CGEN_INSN_##attr)

/* x-invalid: --invalid-- */

SEM_PC
SEM_FN_NAME (m32rbf,x_invalid) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_empty.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 0);

  {
#if WITH_SCACHE
    /* Update the recorded pc in the cpu state struct.  */
    SET_H_PC (pc);
#endif
    sim_engine_invalid_insn (current_cpu, pc);
    sim_io_error (CPU_STATE (current_cpu), "invalid insn not handled\n");
    /* NOTREACHED */
  }

  return vpc;
#undef FLD
}

/* x-after: --after-- */

SEM_PC
SEM_FN_NAME (m32rbf,x_after) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_empty.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 0);

  {
#if WITH_SCACHE_PBB
    m32rbf_pbb_after (current_cpu, sem_arg);
#endif
  }

  return vpc;
#undef FLD
}

/* x-before: --before-- */

SEM_PC
SEM_FN_NAME (m32rbf,x_before) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_empty.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 0);

  {
#if WITH_SCACHE_PBB
    m32rbf_pbb_before (current_cpu, sem_arg);
#endif
  }

  return vpc;
#undef FLD
}

/* x-cti-chain: --cti-chain-- */

SEM_PC
SEM_FN_NAME (m32rbf,x_cti_chain) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_empty.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 0);

  {
#if WITH_SCACHE_PBB
#ifdef DEFINE_SWITCH
    vpc = m32rbf_pbb_cti_chain (current_cpu, sem_arg,
			       pbb_br_npc_ptr, pbb_br_npc);
    BREAK (sem);
#else
    /* FIXME: Allow provision of explicit ifmt spec in insn spec.  */
    vpc = m32rbf_pbb_cti_chain (current_cpu, sem_arg,
			       CPU_PBB_BR_NPC_PTR (current_cpu),
			       CPU_PBB_BR_NPC (current_cpu));
#endif
#endif
  }

  return vpc;
#undef FLD
}

/* x-chain: --chain-- */

SEM_PC
SEM_FN_NAME (m32rbf,x_chain) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_empty.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 0);

  {
#if WITH_SCACHE_PBB
    vpc = m32rbf_pbb_chain (current_cpu, sem_arg);
#ifdef DEFINE_SWITCH
    BREAK (sem);
#endif
#endif
  }

  return vpc;
#undef FLD
}

/* x-begin: --begin-- */

SEM_PC
SEM_FN_NAME (m32rbf,x_begin) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_empty.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 0);

  {
#if WITH_SCACHE_PBB
#ifdef DEFINE_SWITCH
    /* In the switch case FAST_P is a constant, allowing several optimizations
       in any called inline functions.  */
    vpc = m32rbf_pbb_begin (current_cpu, FAST_P);
#else
    vpc = m32rbf_pbb_begin (current_cpu, STATE_RUN_FAST_P (CPU_STATE (current_cpu)));
#endif
#endif
  }

  return vpc;
#undef FLD
}

/* add: add $dr,$sr */

SEM_PC
SEM_FN_NAME (m32rbf,add) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = ADDSI (* FLD (i_dr), * FLD (i_sr));
    * FLD (i_dr) = opval;
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* add3: add3 $dr,$sr,$hash$slo16 */

SEM_PC
SEM_FN_NAME (m32rbf,add3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_add3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = ADDSI (* FLD (i_sr), FLD (f_simm16));
    * FLD (i_dr) = opval;
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* and: and $dr,$sr */

SEM_PC
SEM_FN_NAME (m32rbf,and) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = ANDSI (* FLD (i_dr), * FLD (i_sr));
    * FLD (i_dr) = opval;
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* and3: and3 $dr,$sr,$uimm16 */

SEM_PC
SEM_FN_NAME (m32rbf,and3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_and3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = ANDSI (* FLD (i_sr), FLD (f_uimm16));
    * FLD (i_dr) = opval;
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* or: or $dr,$sr */

SEM_PC
SEM_FN_NAME (m32rbf,or) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = ORSI (* FLD (i_dr), * FLD (i_sr));
    * FLD (i_dr) = opval;
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* or3: or3 $dr,$sr,$hash$ulo16 */

SEM_PC
SEM_FN_NAME (m32rbf,or3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_or3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = ORSI (* FLD (i_sr), FLD (f_uimm16));
    * FLD (i_dr) = opval;
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* xor: xor $dr,$sr */

SEM_PC
SEM_FN_NAME (m32rbf,xor) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = XORSI (* FLD (i_dr), * FLD (i_sr));
    * FLD (i_dr) = opval;
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* xor3: xor3 $dr,$sr,$uimm16 */

SEM_PC
SEM_FN_NAME (m32rbf,xor3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_and3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = XORSI (* FLD (i_sr), FLD (f_uimm16));
    * FLD (i_dr) = opval;
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* addi: addi $dr,$simm8 */

SEM_PC
SEM_FN_NAME (m32rbf,addi) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_addi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = ADDSI (* FLD (i_dr), FLD (f_simm8));
    * FLD (i_dr) = opval;
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* addv: addv $dr,$sr */

SEM_PC
SEM_FN_NAME (m32rbf,addv) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_addv.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  BI temp1;SI temp0;
  temp0 = ADDSI (* FLD (i_dr), * FLD (i_sr));
  temp1 = ADDOFSI (* FLD (i_dr), * FLD (i_sr), 0);
  {
    SI opval = temp0;
    * FLD (i_dr) = opval;
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }
  {
    BI opval = temp1;
    CPU (h_cond) = opval;
    TRACE_RESULT (current_cpu, abuf, "condbit", 'x', opval);
  }
} while (0);

  return vpc;
#undef FLD
}

/* addv3: addv3 $dr,$sr,$simm16 */

SEM_PC
SEM_FN_NAME (m32rbf,addv3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_addv3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

do {
  BI temp1;SI temp0;
  temp0 = ADDSI (* FLD (i_sr), FLD (f_simm16));
  temp1 = ADDOFSI (* FLD (i_sr), FLD (f_simm16), 0);
  {
    SI opval = temp0;
    * FLD (i_dr) = opval;
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }
  {
    BI opval = temp1;
    CPU (h_cond) = opval;
    TRACE_RESULT (current_cpu, abuf, "condbit", 'x', opval);
  }
} while (0);

  return vpc;
#undef FLD
}

/* addx: addx $dr,$sr */

SEM_PC
SEM_FN_NAME (m32rbf,addx) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_addx.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  BI temp1;SI temp0;
  temp0 = ADDCSI (* FLD (i_dr), * FLD (i_sr), CPU (h_cond));
  temp1 = ADDCFSI (* FLD (i_dr), * FLD (i_sr), CPU (h_cond));
  {
    SI opval = temp0;
    * FLD (i_dr) = opval;
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }
  {
    BI opval = temp1;
    CPU (h_cond) = opval;
    TRACE_RESULT (current_cpu, abuf, "condbit", 'x', opval);
  }
} while (0);

  return vpc;
#undef FLD
}

/* bc8: bc.s $disp8 */

SEM_PC
SEM_FN_NAME (m32rbf,bc8) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bc8.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

if (CPU (h_cond)) {
  {
    USI opval = FLD (i_disp8);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI
  return vpc;
#undef FLD
}

/* bc24: bc.l $disp24 */

SEM_PC
SEM_FN_NAME (m32rbf,bc24) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bc24.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (CPU (h_cond)) {
  {
    USI opval = FLD (i_disp24);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI
  return vpc;
#undef FLD
}

/* beq: beq $src1,$src2,$disp16 */

SEM_PC
SEM_FN_NAME (m32rbf,beq) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_beq.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (EQSI (* FLD (i_src1), * FLD (i_src2))) {
  {
    USI opval = FLD (i_disp16);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI
  return vpc;
#undef FLD
}

/* beqz: beqz $src2,$disp16 */

SEM_PC
SEM_FN_NAME (m32rbf,beqz) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_beqz.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (EQSI (* FLD (i_src2), 0)) {
  {
    USI opval = FLD (i_disp16);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI
  return vpc;
#undef FLD
}

/* bgez: bgez $src2,$disp16 */

SEM_PC
SEM_FN_NAME (m32rbf,bgez) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_beqz.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (GESI (* FLD (i_src2), 0)) {
  {
    USI opval = FLD (i_disp16);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI
  return vpc;
#undef FLD
}

/* bgtz: bgtz $src2,$disp16 */

SEM_PC
SEM_FN_NAME (m32rbf,bgtz) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_beqz.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (GTSI (* FLD (i_src2), 0)) {
  {
    USI opval = FLD (i_disp16);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI
  return vpc;
#undef FLD
}

/* blez: blez $src2,$disp16 */

SEM_PC
SEM_FN_NAME (m32rbf,blez) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_beqz.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (LESI (* FLD (i_src2), 0)) {
  {
    USI opval = FLD (i_disp16);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI
  return vpc;
#undef FLD
}

/* bltz: bltz $src2,$disp16 */

SEM_PC
SEM_FN_NAME (m32rbf,bltz) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_beqz.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (LTSI (* FLD (i_src2), 0)) {
  {
    USI opval = FLD (i_disp16);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI
  return vpc;
#undef FLD
}

/* bnez: bnez $src2,$disp16 */

SEM_PC
SEM_FN_NAME (m32rbf,bnez) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_beqz.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (NESI (* FLD (i_src2), 0)) {
  {
    USI opval = FLD (i_disp16);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI
  return vpc;
#undef FLD
}

/* bl8: bl.s $disp8 */

SEM_PC
SEM_FN_NAME (m32rbf,bl8) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bl8.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  {
    SI opval = ADDSI (ANDSI (pc, -4), 4);
    CPU (h_gr[((UINT) 14)]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-14", 'x', opval);
  }
  {
    USI opval = FLD (i_disp8);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} while (0);

  SEM_BRANCH_FINI
  return vpc;
#undef FLD
}

/* bl24: bl.l $disp24 */

SEM_PC
SEM_FN_NAME (m32rbf,bl24) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bl24.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

do {
  {
    SI opval = ADDSI (pc, 4);
    CPU (h_gr[((UINT) 14)]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-14", 'x', opval);
  }
  {
    USI opval = FLD (i_disp24);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} while (0);

  SEM_BRANCH_FINI
  return vpc;
#undef FLD
}

/* bnc8: bnc.s $disp8 */

SEM_PC
SEM_FN_NAME (m32rbf,bnc8) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bc8.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

if (NOTBI (CPU (h_cond))) {
  {
    USI opval = FLD (i_disp8);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI
  return vpc;
#undef FLD
}

/* bnc24: bnc.l $disp24 */

SEM_PC
SEM_FN_NAME (m32rbf,bnc24) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bc24.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (NOTBI (CPU (h_cond))) {
  {
    USI opval = FLD (i_disp24);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI
  return vpc;
#undef FLD
}

/* bne: bne $src1,$src2,$disp16 */

SEM_PC
SEM_FN_NAME (m32rbf,bne) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_beq.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (NESI (* FLD (i_src1), * FLD (i_src2))) {
  {
    USI opval = FLD (i_disp16);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI
  return vpc;
#undef FLD
}

/* bra8: bra.s $disp8 */

SEM_PC
SEM_FN_NAME (m32rbf,bra8) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bra8.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    USI opval = FLD (i_disp8);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }

  SEM_BRANCH_FINI
  return vpc;
#undef FLD
}

/* bra24: bra.l $disp24 */

SEM_PC
SEM_FN_NAME (m32rbf,bra24) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bra24.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    USI opval = FLD (i_disp24);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }

  SEM_BRANCH_FINI
  return vpc;
#undef FLD
}

/* cmp: cmp $src1,$src2 */

SEM_PC
SEM_FN_NAME (m32rbf,cmp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_cmp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    BI opval = LTSI (* FLD (i_src1), * FLD (i_src2));
    CPU (h_cond) = opval;
    TRACE_RESULT (current_cpu, abuf, "condbit", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* cmpi: cmpi $src2,$simm16 */

SEM_PC
SEM_FN_NAME (m32rbf,cmpi) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_cmpi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    BI opval = LTSI (* FLD (i_src2), FLD (f_simm16));
    CPU (h_cond) = opval;
    TRACE_RESULT (current_cpu, abuf, "condbit", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* cmpu: cmpu $src1,$src2 */

SEM_PC
SEM_FN_NAME (m32rbf,cmpu) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_cmp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    BI opval = LTUSI (* FLD (i_src1), * FLD (i_src2));
    CPU (h_cond) = opval;
    TRACE_RESULT (current_cpu, abuf, "condbit", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* cmpui: cmpui $src2,$simm16 */

SEM_PC
SEM_FN_NAME (m32rbf,cmpui) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_cmpi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    BI opval = LTUSI (* FLD (i_src2), FLD (f_simm16));
    CPU (h_cond) = opval;
    TRACE_RESULT (current_cpu, abuf, "condbit", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* div: div $dr,$sr */

SEM_PC
SEM_FN_NAME (m32rbf,div) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_div.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (NESI (* FLD (i_sr), 0)) {
  {
    SI opval = DIVSI (* FLD (i_dr), * FLD (i_sr));
    * FLD (i_dr) = opval;
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }
}

  abuf->written = written;
  return vpc;
#undef FLD
}

/* divu: divu $dr,$sr */

SEM_PC
SEM_FN_NAME (m32rbf,divu) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_div.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (NESI (* FLD (i_sr), 0)) {
  {
    SI opval = UDIVSI (* FLD (i_dr), * FLD (i_sr));
    * FLD (i_dr) = opval;
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }
}

  abuf->written = written;
  return vpc;
#undef FLD
}

/* rem: rem $dr,$sr */

SEM_PC
SEM_FN_NAME (m32rbf,rem) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_div.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (NESI (* FLD (i_sr), 0)) {
  {
    SI opval = MODSI (* FLD (i_dr), * FLD (i_sr));
    * FLD (i_dr) = opval;
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }
}

  abuf->written = written;
  return vpc;
#undef FLD
}

/* remu: remu $dr,$sr */

SEM_PC
SEM_FN_NAME (m32rbf,remu) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_div.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (NESI (* FLD (i_sr), 0)) {
  {
    SI opval = UMODSI (* FLD (i_dr), * FLD (i_sr));
    * FLD (i_dr) = opval;
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }
}

  abuf->written = written;
  return vpc;
#undef FLD
}

/* jl: jl $sr */

SEM_PC
SEM_FN_NAME (m32rbf,jl) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_jl.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  USI temp1;SI temp0;
  temp0 = ADDSI (ANDSI (pc, -4), 4);
  temp1 = ANDSI (* FLD (i_sr), -4);
  {
    SI opval = temp0;
    CPU (h_gr[((UINT) 14)]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-14", 'x', opval);
  }
  {
    USI opval = temp1;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} while (0);

  SEM_BRANCH_FINI
  return vpc;
#undef FLD
}

/* jmp: jmp $sr */

SEM_PC
SEM_FN_NAME (m32rbf,jmp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_jmp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    USI opval = ANDSI (* FLD (i_sr), -4);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }

  SEM_BRANCH_FINI
  return vpc;
#undef FLD
}

/* ld: ld $dr,@$sr */

SEM_PC
SEM_FN_NAME (m32rbf,ld) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ld.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = GETMEMSI (current_cpu, * FLD (i_sr));
    * FLD (i_dr) = opval;
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ld-d: ld $dr,@($slo16,$sr) */

SEM_PC
SEM_FN_NAME (m32rbf,ld_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ld_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = GETMEMSI (current_cpu, ADDSI (* FLD (i_sr), FLD (f_simm16)));
    * FLD (i_dr) = opval;
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldb: ldb $dr,@$sr */

SEM_PC
SEM_FN_NAME (m32rbf,ldb) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldb.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = EXTQISI (GETMEMQI (current_cpu, * FLD (i_sr)));
    * FLD (i_dr) = opval;
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldb-d: ldb $dr,@($slo16,$sr) */

SEM_PC
SEM_FN_NAME (m32rbf,ldb_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldb_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = EXTQISI (GETMEMQI (current_cpu, ADDSI (* FLD (i_sr), FLD (f_simm16))));
    * FLD (i_dr) = opval;
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldh: ldh $dr,@$sr */

SEM_PC
SEM_FN_NAME (m32rbf,ldh) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldh.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = EXTHISI (GETMEMHI (current_cpu, * FLD (i_sr)));
    * FLD (i_dr) = opval;
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldh-d: ldh $dr,@($slo16,$sr) */

SEM_PC
SEM_FN_NAME (m32rbf,ldh_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldh_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = EXTHISI (GETMEMHI (current_cpu, ADDSI (* FLD (i_sr), FLD (f_simm16))));
    * FLD (i_dr) = opval;
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldub: ldub $dr,@$sr */

SEM_PC
SEM_FN_NAME (m32rbf,ldub) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldb.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = ZEXTQISI (GETMEMQI (current_cpu, * FLD (i_sr)));
    * FLD (i_dr) = opval;
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldub-d: ldub $dr,@($slo16,$sr) */

SEM_PC
SEM_FN_NAME (m32rbf,ldub_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldb_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = ZEXTQISI (GETMEMQI (current_cpu, ADDSI (* FLD (i_sr), FLD (f_simm16))));
    * FLD (i_dr) = opval;
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* lduh: lduh $dr,@$sr */

SEM_PC
SEM_FN_NAME (m32rbf,lduh) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldh.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = ZEXTHISI (GETMEMHI (current_cpu, * FLD (i_sr)));
    * FLD (i_dr) = opval;
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* lduh-d: lduh $dr,@($slo16,$sr) */

SEM_PC
SEM_FN_NAME (m32rbf,lduh_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldh_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = ZEXTHISI (GETMEMHI (current_cpu, ADDSI (* FLD (i_sr), FLD (f_simm16))));
    * FLD (i_dr) = opval;
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ld-plus: ld $dr,@$sr+ */

SEM_PC
SEM_FN_NAME (m32rbf,ld_plus) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ld_plus.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  SI temp1;SI temp0;
  temp0 = GETMEMSI (current_cpu, * FLD (i_sr));
  temp1 = ADDSI (* FLD (i_sr), 4);
  {
    SI opval = temp0;
    * FLD (i_dr) = opval;
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }
  {
    SI opval = temp1;
    * FLD (i_sr) = opval;
    TRACE_RESULT (current_cpu, abuf, "sr", 'x', opval);
  }
} while (0);

  return vpc;
#undef FLD
}

/* ld24: ld24 $dr,$uimm24 */

SEM_PC
SEM_FN_NAME (m32rbf,ld24) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ld24.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = FLD (i_uimm24);
    * FLD (i_dr) = opval;
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldi8: ldi8 $dr,$simm8 */

SEM_PC
SEM_FN_NAME (m32rbf,ldi8) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldi8.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = FLD (f_simm8);
    * FLD (i_dr) = opval;
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldi16: ldi16 $dr,$hash$slo16 */

SEM_PC
SEM_FN_NAME (m32rbf,ldi16) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldi16.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = FLD (f_simm16);
    * FLD (i_dr) = opval;
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* lock: lock $dr,@$sr */

SEM_PC
SEM_FN_NAME (m32rbf,lock) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_lock.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  {
    BI opval = 1;
    CPU (h_lock) = opval;
    TRACE_RESULT (current_cpu, abuf, "lock-0", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, * FLD (i_sr));
    * FLD (i_dr) = opval;
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }
} while (0);

  return vpc;
#undef FLD
}

/* machi: machi $src1,$src2 */

SEM_PC
SEM_FN_NAME (m32rbf,machi) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_machi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    DI opval = SRADI (SLLDI (ADDDI (m32rbf_h_accum_get (current_cpu), MULDI (EXTSIDI (ANDSI (* FLD (i_src1), 0xffff0000)), EXTHIDI (TRUNCSIHI (SRASI (* FLD (i_src2), 16))))), 8), 8);
    m32rbf_h_accum_set (current_cpu, opval);
    TRACE_RESULT (current_cpu, abuf, "accum", 'D', opval);
  }

  return vpc;
#undef FLD
}

/* maclo: maclo $src1,$src2 */

SEM_PC
SEM_FN_NAME (m32rbf,maclo) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_machi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    DI opval = SRADI (SLLDI (ADDDI (m32rbf_h_accum_get (current_cpu), MULDI (EXTSIDI (SLLSI (* FLD (i_src1), 16)), EXTHIDI (TRUNCSIHI (* FLD (i_src2))))), 8), 8);
    m32rbf_h_accum_set (current_cpu, opval);
    TRACE_RESULT (current_cpu, abuf, "accum", 'D', opval);
  }

  return vpc;
#undef FLD
}

/* macwhi: macwhi $src1,$src2 */

SEM_PC
SEM_FN_NAME (m32rbf,macwhi) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_machi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    DI opval = SRADI (SLLDI (ADDDI (m32rbf_h_accum_get (current_cpu), MULDI (EXTSIDI (* FLD (i_src1)), EXTHIDI (TRUNCSIHI (SRASI (* FLD (i_src2), 16))))), 8), 8);
    m32rbf_h_accum_set (current_cpu, opval);
    TRACE_RESULT (current_cpu, abuf, "accum", 'D', opval);
  }

  return vpc;
#undef FLD
}

/* macwlo: macwlo $src1,$src2 */

SEM_PC
SEM_FN_NAME (m32rbf,macwlo) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_machi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    DI opval = SRADI (SLLDI (ADDDI (m32rbf_h_accum_get (current_cpu), MULDI (EXTSIDI (* FLD (i_src1)), EXTHIDI (TRUNCSIHI (* FLD (i_src2))))), 8), 8);
    m32rbf_h_accum_set (current_cpu, opval);
    TRACE_RESULT (current_cpu, abuf, "accum", 'D', opval);
  }

  return vpc;
#undef FLD
}

/* mul: mul $dr,$sr */

SEM_PC
SEM_FN_NAME (m32rbf,mul) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = MULSI (* FLD (i_dr), * FLD (i_sr));
    * FLD (i_dr) = opval;
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* mulhi: mulhi $src1,$src2 */

SEM_PC
SEM_FN_NAME (m32rbf,mulhi) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulhi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    DI opval = SRADI (SLLDI (MULDI (EXTSIDI (ANDSI (* FLD (i_src1), 0xffff0000)), EXTHIDI (TRUNCSIHI (SRASI (* FLD (i_src2), 16)))), 16), 16);
    m32rbf_h_accum_set (current_cpu, opval);
    TRACE_RESULT (current_cpu, abuf, "accum", 'D', opval);
  }

  return vpc;
#undef FLD
}

/* mullo: mullo $src1,$src2 */

SEM_PC
SEM_FN_NAME (m32rbf,mullo) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulhi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    DI opval = SRADI (SLLDI (MULDI (EXTSIDI (SLLSI (* FLD (i_src1), 16)), EXTHIDI (TRUNCSIHI (* FLD (i_src2)))), 16), 16);
    m32rbf_h_accum_set (current_cpu, opval);
    TRACE_RESULT (current_cpu, abuf, "accum", 'D', opval);
  }

  return vpc;
#undef FLD
}

/* mulwhi: mulwhi $src1,$src2 */

SEM_PC
SEM_FN_NAME (m32rbf,mulwhi) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulhi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    DI opval = SRADI (SLLDI (MULDI (EXTSIDI (* FLD (i_src1)), EXTHIDI (TRUNCSIHI (SRASI (* FLD (i_src2), 16)))), 8), 8);
    m32rbf_h_accum_set (current_cpu, opval);
    TRACE_RESULT (current_cpu, abuf, "accum", 'D', opval);
  }

  return vpc;
#undef FLD
}

/* mulwlo: mulwlo $src1,$src2 */

SEM_PC
SEM_FN_NAME (m32rbf,mulwlo) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulhi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    DI opval = SRADI (SLLDI (MULDI (EXTSIDI (* FLD (i_src1)), EXTHIDI (TRUNCSIHI (* FLD (i_src2)))), 8), 8);
    m32rbf_h_accum_set (current_cpu, opval);
    TRACE_RESULT (current_cpu, abuf, "accum", 'D', opval);
  }

  return vpc;
#undef FLD
}

/* mv: mv $dr,$sr */

SEM_PC
SEM_FN_NAME (m32rbf,mv) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mv.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = * FLD (i_sr);
    * FLD (i_dr) = opval;
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* mvfachi: mvfachi $dr */

SEM_PC
SEM_FN_NAME (m32rbf,mvfachi) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mvfachi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = TRUNCDISI (SRADI (m32rbf_h_accum_get (current_cpu), 32));
    * FLD (i_dr) = opval;
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* mvfaclo: mvfaclo $dr */

SEM_PC
SEM_FN_NAME (m32rbf,mvfaclo) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mvfachi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = TRUNCDISI (m32rbf_h_accum_get (current_cpu));
    * FLD (i_dr) = opval;
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* mvfacmi: mvfacmi $dr */

SEM_PC
SEM_FN_NAME (m32rbf,mvfacmi) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mvfachi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = TRUNCDISI (SRADI (m32rbf_h_accum_get (current_cpu), 16));
    * FLD (i_dr) = opval;
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* mvfc: mvfc $dr,$scr */

SEM_PC
SEM_FN_NAME (m32rbf,mvfc) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mvfc.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = m32rbf_h_cr_get (current_cpu, FLD (f_r2));
    * FLD (i_dr) = opval;
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* mvtachi: mvtachi $src1 */

SEM_PC
SEM_FN_NAME (m32rbf,mvtachi) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mvtachi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    DI opval = ORDI (ANDDI (m32rbf_h_accum_get (current_cpu), MAKEDI (0, 0xffffffff)), SLLDI (EXTSIDI (* FLD (i_src1)), 32));
    m32rbf_h_accum_set (current_cpu, opval);
    TRACE_RESULT (current_cpu, abuf, "accum", 'D', opval);
  }

  return vpc;
#undef FLD
}

/* mvtaclo: mvtaclo $src1 */

SEM_PC
SEM_FN_NAME (m32rbf,mvtaclo) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mvtachi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    DI opval = ORDI (ANDDI (m32rbf_h_accum_get (current_cpu), MAKEDI (0xffffffff, 0)), ZEXTSIDI (* FLD (i_src1)));
    m32rbf_h_accum_set (current_cpu, opval);
    TRACE_RESULT (current_cpu, abuf, "accum", 'D', opval);
  }

  return vpc;
#undef FLD
}

/* mvtc: mvtc $sr,$dcr */

SEM_PC
SEM_FN_NAME (m32rbf,mvtc) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mvtc.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    USI opval = * FLD (i_sr);
    m32rbf_h_cr_set (current_cpu, FLD (f_r1), opval);
    TRACE_RESULT (current_cpu, abuf, "dcr", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* neg: neg $dr,$sr */

SEM_PC
SEM_FN_NAME (m32rbf,neg) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mv.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = NEGSI (* FLD (i_sr));
    * FLD (i_dr) = opval;
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* nop: nop */

SEM_PC
SEM_FN_NAME (m32rbf,nop) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_nop.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

PROFILE_COUNT_FILLNOPS (current_cpu, abuf->addr);

  return vpc;
#undef FLD
}

/* not: not $dr,$sr */

SEM_PC
SEM_FN_NAME (m32rbf,not) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mv.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = INVSI (* FLD (i_sr));
    * FLD (i_dr) = opval;
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* rac: rac */

SEM_PC
SEM_FN_NAME (m32rbf,rac) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_rac.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  DI tmp_tmp1;
  tmp_tmp1 = SLLDI (m32rbf_h_accum_get (current_cpu), 1);
  tmp_tmp1 = ADDDI (tmp_tmp1, MAKEDI (0, 32768));
  {
    DI opval = (GTDI (tmp_tmp1, MAKEDI (32767, 0xffff0000))) ? (MAKEDI (32767, 0xffff0000)) : (LTDI (tmp_tmp1, MAKEDI (0xffff8000, 0))) ? (MAKEDI (0xffff8000, 0)) : (ANDDI (tmp_tmp1, MAKEDI (0xffffffff, 0xffff0000)));
    m32rbf_h_accum_set (current_cpu, opval);
    TRACE_RESULT (current_cpu, abuf, "accum", 'D', opval);
  }
} while (0);

  return vpc;
#undef FLD
}

/* rach: rach */

SEM_PC
SEM_FN_NAME (m32rbf,rach) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_rac.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  DI tmp_tmp1;
  tmp_tmp1 = ANDDI (m32rbf_h_accum_get (current_cpu), MAKEDI (16777215, 0xffffffff));
if (ANDIF (GEDI (tmp_tmp1, MAKEDI (16383, 0x80000000)), LEDI (tmp_tmp1, MAKEDI (8388607, 0xffffffff)))) {
  tmp_tmp1 = MAKEDI (16383, 0x80000000);
} else {
if (ANDIF (GEDI (tmp_tmp1, MAKEDI (8388608, 0)), LEDI (tmp_tmp1, MAKEDI (16760832, 0)))) {
  tmp_tmp1 = MAKEDI (16760832, 0);
} else {
  tmp_tmp1 = ANDDI (ADDDI (m32rbf_h_accum_get (current_cpu), MAKEDI (0, 1073741824)), MAKEDI (0xffffffff, 0x80000000));
}
}
  tmp_tmp1 = SLLDI (tmp_tmp1, 1);
  {
    DI opval = SRADI (SLLDI (tmp_tmp1, 7), 7);
    m32rbf_h_accum_set (current_cpu, opval);
    TRACE_RESULT (current_cpu, abuf, "accum", 'D', opval);
  }
} while (0);

  return vpc;
#undef FLD
}

/* rte: rte */

SEM_PC
SEM_FN_NAME (m32rbf,rte) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_rte.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  {
    USI opval = ANDSI (m32rbf_h_cr_get (current_cpu, ((UINT) 6)), -4);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
  {
    USI opval = m32rbf_h_cr_get (current_cpu, ((UINT) 14));
    m32rbf_h_cr_set (current_cpu, ((UINT) 6), opval);
    TRACE_RESULT (current_cpu, abuf, "cr-6", 'x', opval);
  }
  {
    UQI opval = CPU (h_bpsw);
    m32rbf_h_psw_set (current_cpu, opval);
    TRACE_RESULT (current_cpu, abuf, "psw-0", 'x', opval);
  }
  {
    UQI opval = CPU (h_bbpsw);
    CPU (h_bpsw) = opval;
    TRACE_RESULT (current_cpu, abuf, "bpsw-0", 'x', opval);
  }
} while (0);

  SEM_BRANCH_FINI
  return vpc;
#undef FLD
}

/* seth: seth $dr,$hash$hi16 */

SEM_PC
SEM_FN_NAME (m32rbf,seth) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_seth.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = SLLSI (FLD (f_hi16), 16);
    * FLD (i_dr) = opval;
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* sll: sll $dr,$sr */

SEM_PC
SEM_FN_NAME (m32rbf,sll) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = SLLSI (* FLD (i_dr), ANDSI (* FLD (i_sr), 31));
    * FLD (i_dr) = opval;
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* sll3: sll3 $dr,$sr,$simm16 */

SEM_PC
SEM_FN_NAME (m32rbf,sll3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_sll3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = SLLSI (* FLD (i_sr), ANDSI (FLD (f_simm16), 31));
    * FLD (i_dr) = opval;
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* slli: slli $dr,$uimm5 */

SEM_PC
SEM_FN_NAME (m32rbf,slli) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_slli.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = SLLSI (* FLD (i_dr), FLD (f_uimm5));
    * FLD (i_dr) = opval;
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* sra: sra $dr,$sr */

SEM_PC
SEM_FN_NAME (m32rbf,sra) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = SRASI (* FLD (i_dr), ANDSI (* FLD (i_sr), 31));
    * FLD (i_dr) = opval;
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* sra3: sra3 $dr,$sr,$simm16 */

SEM_PC
SEM_FN_NAME (m32rbf,sra3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_sll3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = SRASI (* FLD (i_sr), ANDSI (FLD (f_simm16), 31));
    * FLD (i_dr) = opval;
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* srai: srai $dr,$uimm5 */

SEM_PC
SEM_FN_NAME (m32rbf,srai) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_slli.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = SRASI (* FLD (i_dr), FLD (f_uimm5));
    * FLD (i_dr) = opval;
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* srl: srl $dr,$sr */

SEM_PC
SEM_FN_NAME (m32rbf,srl) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = SRLSI (* FLD (i_dr), ANDSI (* FLD (i_sr), 31));
    * FLD (i_dr) = opval;
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* srl3: srl3 $dr,$sr,$simm16 */

SEM_PC
SEM_FN_NAME (m32rbf,srl3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_sll3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = SRLSI (* FLD (i_sr), ANDSI (FLD (f_simm16), 31));
    * FLD (i_dr) = opval;
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* srli: srli $dr,$uimm5 */

SEM_PC
SEM_FN_NAME (m32rbf,srli) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_slli.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = SRLSI (* FLD (i_dr), FLD (f_uimm5));
    * FLD (i_dr) = opval;
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* st: st $src1,@$src2 */

SEM_PC
SEM_FN_NAME (m32rbf,st) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_st.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = * FLD (i_src1);
    SETMEMSI (current_cpu, * FLD (i_src2), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* st-d: st $src1,@($slo16,$src2) */

SEM_PC
SEM_FN_NAME (m32rbf,st_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_st_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = * FLD (i_src1);
    SETMEMSI (current_cpu, ADDSI (* FLD (i_src2), FLD (f_simm16)), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* stb: stb $src1,@$src2 */

SEM_PC
SEM_FN_NAME (m32rbf,stb) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stb.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    QI opval = * FLD (i_src1);
    SETMEMQI (current_cpu, * FLD (i_src2), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* stb-d: stb $src1,@($slo16,$src2) */

SEM_PC
SEM_FN_NAME (m32rbf,stb_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stb_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    QI opval = * FLD (i_src1);
    SETMEMQI (current_cpu, ADDSI (* FLD (i_src2), FLD (f_simm16)), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* sth: sth $src1,@$src2 */

SEM_PC
SEM_FN_NAME (m32rbf,sth) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_sth.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    HI opval = * FLD (i_src1);
    SETMEMHI (current_cpu, * FLD (i_src2), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* sth-d: sth $src1,@($slo16,$src2) */

SEM_PC
SEM_FN_NAME (m32rbf,sth_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_sth_d.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    HI opval = * FLD (i_src1);
    SETMEMHI (current_cpu, ADDSI (* FLD (i_src2), FLD (f_simm16)), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* st-plus: st $src1,@+$src2 */

SEM_PC
SEM_FN_NAME (m32rbf,st_plus) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_st_plus.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  SI tmp_new_src2;
  tmp_new_src2 = ADDSI (* FLD (i_src2), 4);
  {
    SI opval = * FLD (i_src1);
    SETMEMSI (current_cpu, tmp_new_src2, opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = tmp_new_src2;
    * FLD (i_src2) = opval;
    TRACE_RESULT (current_cpu, abuf, "src2", 'x', opval);
  }
} while (0);

  return vpc;
#undef FLD
}

/* st-minus: st $src1,@-$src2 */

SEM_PC
SEM_FN_NAME (m32rbf,st_minus) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_st_plus.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  SI tmp_new_src2;
  tmp_new_src2 = SUBSI (* FLD (i_src2), 4);
  {
    SI opval = * FLD (i_src1);
    SETMEMSI (current_cpu, tmp_new_src2, opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = tmp_new_src2;
    * FLD (i_src2) = opval;
    TRACE_RESULT (current_cpu, abuf, "src2", 'x', opval);
  }
} while (0);

  return vpc;
#undef FLD
}

/* sub: sub $dr,$sr */

SEM_PC
SEM_FN_NAME (m32rbf,sub) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = SUBSI (* FLD (i_dr), * FLD (i_sr));
    * FLD (i_dr) = opval;
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* subv: subv $dr,$sr */

SEM_PC
SEM_FN_NAME (m32rbf,subv) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_addv.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  BI temp1;SI temp0;
  temp0 = SUBSI (* FLD (i_dr), * FLD (i_sr));
  temp1 = SUBOFSI (* FLD (i_dr), * FLD (i_sr), 0);
  {
    SI opval = temp0;
    * FLD (i_dr) = opval;
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }
  {
    BI opval = temp1;
    CPU (h_cond) = opval;
    TRACE_RESULT (current_cpu, abuf, "condbit", 'x', opval);
  }
} while (0);

  return vpc;
#undef FLD
}

/* subx: subx $dr,$sr */

SEM_PC
SEM_FN_NAME (m32rbf,subx) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_addx.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  BI temp1;SI temp0;
  temp0 = SUBCSI (* FLD (i_dr), * FLD (i_sr), CPU (h_cond));
  temp1 = SUBCFSI (* FLD (i_dr), * FLD (i_sr), CPU (h_cond));
  {
    SI opval = temp0;
    * FLD (i_dr) = opval;
    TRACE_RESULT (current_cpu, abuf, "dr", 'x', opval);
  }
  {
    BI opval = temp1;
    CPU (h_cond) = opval;
    TRACE_RESULT (current_cpu, abuf, "condbit", 'x', opval);
  }
} while (0);

  return vpc;
#undef FLD
}

/* trap: trap $uimm4 */

SEM_PC
SEM_FN_NAME (m32rbf,trap) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_trap.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  {
    USI opval = m32rbf_h_cr_get (current_cpu, ((UINT) 6));
    m32rbf_h_cr_set (current_cpu, ((UINT) 14), opval);
    TRACE_RESULT (current_cpu, abuf, "cr-14", 'x', opval);
  }
  {
    USI opval = ADDSI (pc, 4);
    m32rbf_h_cr_set (current_cpu, ((UINT) 6), opval);
    TRACE_RESULT (current_cpu, abuf, "cr-6", 'x', opval);
  }
  {
    UQI opval = CPU (h_bpsw);
    CPU (h_bbpsw) = opval;
    TRACE_RESULT (current_cpu, abuf, "bbpsw-0", 'x', opval);
  }
  {
    UQI opval = m32rbf_h_psw_get (current_cpu);
    CPU (h_bpsw) = opval;
    TRACE_RESULT (current_cpu, abuf, "bpsw-0", 'x', opval);
  }
  {
    UQI opval = ANDQI (m32rbf_h_psw_get (current_cpu), 128);
    m32rbf_h_psw_set (current_cpu, opval);
    TRACE_RESULT (current_cpu, abuf, "psw-0", 'x', opval);
  }
  {
    SI opval = m32r_trap (current_cpu, pc, FLD (f_uimm4));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} while (0);

  SEM_BRANCH_FINI
  return vpc;
#undef FLD
}

/* unlock: unlock $src1,@$src2 */

SEM_PC
SEM_FN_NAME (m32rbf,unlock) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_unlock.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  PCADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
if (CPU (h_lock)) {
  {
    SI opval = * FLD (i_src1);
    SETMEMSI (current_cpu, * FLD (i_src2), opval);
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}
  {
    BI opval = 0;
    CPU (h_lock) = opval;
    TRACE_RESULT (current_cpu, abuf, "lock-0", 'x', opval);
  }
} while (0);

  abuf->written = written;
  return vpc;
#undef FLD
}

