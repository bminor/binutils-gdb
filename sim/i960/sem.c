/* Simulator instruction semantics for i960base.

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

#define WANT_CPU i960base
#define WANT_CPU_I960BASE

#include "sim-main.h"
#include "cgen-mem.h"
#include "cgen-ops.h"

#undef GET_ATTR
#define GET_ATTR(cpu, num, attr) CGEN_ATTR_VALUE (NULL, abuf->idesc->attrs, CGEN_INSN_##attr)

/* x-invalid: --invalid-- */

SEM_PC
SEM_FN_NAME (i960base,x_invalid) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_empty.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
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
SEM_FN_NAME (i960base,x_after) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_empty.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 0);

  {
#if WITH_SCACHE_PBB_I960BASE
    i960base_pbb_after (current_cpu, sem_arg);
#endif
  }

  return vpc;
#undef FLD
}

/* x-before: --before-- */

SEM_PC
SEM_FN_NAME (i960base,x_before) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_empty.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 0);

  {
#if WITH_SCACHE_PBB_I960BASE
    i960base_pbb_before (current_cpu, sem_arg);
#endif
  }

  return vpc;
#undef FLD
}

/* x-cti-chain: --cti-chain-- */

SEM_PC
SEM_FN_NAME (i960base,x_cti_chain) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_empty.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 0);

  {
#if WITH_SCACHE_PBB_I960BASE
#ifdef DEFINE_SWITCH
    vpc = i960base_pbb_cti_chain (current_cpu, sem_arg,
			       pbb_br_npc_ptr, pbb_br_npc);
    BREAK (sem);
#else
    /* FIXME: Allow provision of explicit ifmt spec in insn spec.  */
    vpc = i960base_pbb_cti_chain (current_cpu, sem_arg,
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
SEM_FN_NAME (i960base,x_chain) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_empty.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 0);

  {
#if WITH_SCACHE_PBB_I960BASE
    vpc = i960base_pbb_chain (current_cpu, sem_arg);
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
SEM_FN_NAME (i960base,x_begin) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_empty.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 0);

  {
#if WITH_SCACHE_PBB_I960BASE
#ifdef DEFINE_SWITCH
    /* In the switch case FAST_P is a constant, allowing several optimizations
       in any called inline functions.  */
    vpc = i960base_pbb_begin (current_cpu, FAST_P);
#else
    vpc = i960base_pbb_begin (current_cpu, STATE_RUN_FAST_P (CPU_STATE (current_cpu)));
#endif
#endif
  }

  return vpc;
#undef FLD
}

/* mulo: mulo $src1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,mulo) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = MULSI (* FLD (i_src1), * FLD (i_src2));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* mulo1: mulo $lit1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,mulo1) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo1.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = MULSI (FLD (f_src1), * FLD (i_src2));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* mulo2: mulo $src1, $lit2, $dst */

SEM_PC
SEM_FN_NAME (i960base,mulo2) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo2.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = MULSI (* FLD (i_src1), FLD (f_src2));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* mulo3: mulo $lit1, $lit2, $dst */

SEM_PC
SEM_FN_NAME (i960base,mulo3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = MULSI (FLD (f_src1), FLD (f_src2));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* remo: remo $src1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,remo) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = UMODSI (* FLD (i_src2), * FLD (i_src1));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* remo1: remo $lit1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,remo1) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo1.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = UMODSI (* FLD (i_src2), FLD (f_src1));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* remo2: remo $src1, $lit2, $dst */

SEM_PC
SEM_FN_NAME (i960base,remo2) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo2.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = UMODSI (FLD (f_src2), * FLD (i_src1));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* remo3: remo $lit1, $lit2, $dst */

SEM_PC
SEM_FN_NAME (i960base,remo3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = UMODSI (FLD (f_src2), FLD (f_src1));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* divo: divo $src1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,divo) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = UDIVSI (* FLD (i_src2), * FLD (i_src1));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* divo1: divo $lit1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,divo1) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo1.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = UDIVSI (* FLD (i_src2), FLD (f_src1));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* divo2: divo $src1, $lit2, $dst */

SEM_PC
SEM_FN_NAME (i960base,divo2) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo2.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = UDIVSI (FLD (f_src2), * FLD (i_src1));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* divo3: divo $lit1, $lit2, $dst */

SEM_PC
SEM_FN_NAME (i960base,divo3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = UDIVSI (FLD (f_src2), FLD (f_src1));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* remi: remi $src1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,remi) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = MODSI (* FLD (i_src2), * FLD (i_src1));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* remi1: remi $lit1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,remi1) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo1.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = MODSI (* FLD (i_src2), FLD (f_src1));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* remi2: remi $src1, $lit2, $dst */

SEM_PC
SEM_FN_NAME (i960base,remi2) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo2.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = MODSI (FLD (f_src2), * FLD (i_src1));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* remi3: remi $lit1, $lit2, $dst */

SEM_PC
SEM_FN_NAME (i960base,remi3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = MODSI (FLD (f_src2), FLD (f_src1));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* divi: divi $src1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,divi) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = DIVSI (* FLD (i_src2), * FLD (i_src1));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* divi1: divi $lit1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,divi1) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo1.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = DIVSI (* FLD (i_src2), FLD (f_src1));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* divi2: divi $src1, $lit2, $dst */

SEM_PC
SEM_FN_NAME (i960base,divi2) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo2.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = DIVSI (FLD (f_src2), * FLD (i_src1));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* divi3: divi $lit1, $lit2, $dst */

SEM_PC
SEM_FN_NAME (i960base,divi3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = DIVSI (FLD (f_src2), FLD (f_src1));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* addo: addo $src1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,addo) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = ADDSI (* FLD (i_src1), * FLD (i_src2));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* addo1: addo $lit1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,addo1) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo1.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = ADDSI (FLD (f_src1), * FLD (i_src2));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* addo2: addo $src1, $lit2, $dst */

SEM_PC
SEM_FN_NAME (i960base,addo2) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo2.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = ADDSI (* FLD (i_src1), FLD (f_src2));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* addo3: addo $lit1, $lit2, $dst */

SEM_PC
SEM_FN_NAME (i960base,addo3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = ADDSI (FLD (f_src1), FLD (f_src2));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* subo: subo $src1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,subo) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = SUBSI (* FLD (i_src2), * FLD (i_src1));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* subo1: subo $lit1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,subo1) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo1.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = SUBSI (* FLD (i_src2), FLD (f_src1));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* subo2: subo $src1, $lit2, $dst */

SEM_PC
SEM_FN_NAME (i960base,subo2) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo2.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = SUBSI (FLD (f_src2), * FLD (i_src1));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* subo3: subo $lit1, $lit2, $dst */

SEM_PC
SEM_FN_NAME (i960base,subo3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = SUBSI (FLD (f_src2), FLD (f_src1));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* notbit: notbit $src1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,notbit) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_notbit.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = XORSI (SLLSI (1, * FLD (i_src1)), * FLD (i_src2));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* notbit1: notbit $lit1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,notbit1) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_notbit1.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = XORSI (SLLSI (1, FLD (f_src1)), * FLD (i_src2));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* notbit2: notbit $src1, $lit2, $dst */

SEM_PC
SEM_FN_NAME (i960base,notbit2) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_notbit2.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = XORSI (SLLSI (1, * FLD (i_src1)), FLD (f_src2));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* notbit3: notbit $lit1, $lit2, $dst */

SEM_PC
SEM_FN_NAME (i960base,notbit3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_notbit3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = XORSI (SLLSI (1, FLD (f_src1)), FLD (f_src2));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* and: and $src1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,and) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = ANDSI (* FLD (i_src1), * FLD (i_src2));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* and1: and $lit1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,and1) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo1.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = ANDSI (FLD (f_src1), * FLD (i_src2));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* and2: and $src1, $lit2, $dst */

SEM_PC
SEM_FN_NAME (i960base,and2) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo2.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = ANDSI (* FLD (i_src1), FLD (f_src2));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* and3: and $lit1, $lit2, $dst */

SEM_PC
SEM_FN_NAME (i960base,and3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = ANDSI (FLD (f_src1), FLD (f_src2));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* andnot: andnot $src1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,andnot) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = ANDSI (* FLD (i_src2), INVSI (* FLD (i_src1)));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* andnot1: andnot $lit1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,andnot1) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo1.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = ANDSI (* FLD (i_src2), INVSI (FLD (f_src1)));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* andnot2: andnot $src1, $lit2, $dst */

SEM_PC
SEM_FN_NAME (i960base,andnot2) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo2.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = ANDSI (FLD (f_src2), INVSI (* FLD (i_src1)));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* andnot3: andnot $lit1, $lit2, $dst */

SEM_PC
SEM_FN_NAME (i960base,andnot3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = ANDSI (FLD (f_src2), INVSI (FLD (f_src1)));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* setbit: setbit $src1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,setbit) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_notbit.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = ORSI (SLLSI (1, * FLD (i_src1)), * FLD (i_src2));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* setbit1: setbit $lit1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,setbit1) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_notbit1.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = ORSI (SLLSI (1, FLD (f_src1)), * FLD (i_src2));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* setbit2: setbit $src1, $lit2, $dst */

SEM_PC
SEM_FN_NAME (i960base,setbit2) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_notbit2.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = ORSI (SLLSI (1, * FLD (i_src1)), FLD (f_src2));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* setbit3: setbit $lit1, $lit2, $dst */

SEM_PC
SEM_FN_NAME (i960base,setbit3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_notbit3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = ORSI (SLLSI (1, FLD (f_src1)), FLD (f_src2));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* notand: notand $src1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,notand) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = ANDSI (INVSI (* FLD (i_src2)), * FLD (i_src1));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* notand1: notand $lit1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,notand1) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo1.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = ANDSI (INVSI (* FLD (i_src2)), FLD (f_src1));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* notand2: notand $src1, $lit2, $dst */

SEM_PC
SEM_FN_NAME (i960base,notand2) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo2.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = ANDSI (INVSI (FLD (f_src2)), * FLD (i_src1));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* notand3: notand $lit1, $lit2, $dst */

SEM_PC
SEM_FN_NAME (i960base,notand3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = ANDSI (INVSI (FLD (f_src2)), FLD (f_src1));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* xor: xor $src1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,xor) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = XORSI (* FLD (i_src1), * FLD (i_src2));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* xor1: xor $lit1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,xor1) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo1.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = XORSI (FLD (f_src1), * FLD (i_src2));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* xor2: xor $src1, $lit2, $dst */

SEM_PC
SEM_FN_NAME (i960base,xor2) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo2.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = XORSI (* FLD (i_src1), FLD (f_src2));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* xor3: xor $lit1, $lit2, $dst */

SEM_PC
SEM_FN_NAME (i960base,xor3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = XORSI (FLD (f_src1), FLD (f_src2));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* or: or $src1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,or) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = ORSI (* FLD (i_src1), * FLD (i_src2));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* or1: or $lit1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,or1) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo1.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = ORSI (FLD (f_src1), * FLD (i_src2));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* or2: or $src1, $lit2, $dst */

SEM_PC
SEM_FN_NAME (i960base,or2) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo2.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = ORSI (* FLD (i_src1), FLD (f_src2));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* or3: or $lit1, $lit2, $dst */

SEM_PC
SEM_FN_NAME (i960base,or3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = ORSI (FLD (f_src1), FLD (f_src2));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* nor: nor $src1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,nor) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = ANDSI (INVSI (* FLD (i_src2)), INVSI (* FLD (i_src1)));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* nor1: nor $lit1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,nor1) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo1.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = ANDSI (INVSI (* FLD (i_src2)), INVSI (FLD (f_src1)));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* nor2: nor $src1, $lit2, $dst */

SEM_PC
SEM_FN_NAME (i960base,nor2) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo2.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = ANDSI (INVSI (FLD (f_src2)), INVSI (* FLD (i_src1)));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* nor3: nor $lit1, $lit2, $dst */

SEM_PC
SEM_FN_NAME (i960base,nor3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = ANDSI (INVSI (FLD (f_src2)), INVSI (FLD (f_src1)));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* xnor: xnor $src1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,xnor) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = INVSI (XORSI (* FLD (i_src1), * FLD (i_src2)));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* xnor1: xnor $lit1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,xnor1) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo1.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = INVSI (XORSI (FLD (f_src1), * FLD (i_src2)));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* xnor2: xnor $src1, $lit2, $dst */

SEM_PC
SEM_FN_NAME (i960base,xnor2) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo2.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = INVSI (XORSI (* FLD (i_src1), FLD (f_src2)));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* xnor3: xnor $lit1, $lit2, $dst */

SEM_PC
SEM_FN_NAME (i960base,xnor3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = INVSI (XORSI (FLD (f_src1), FLD (f_src2)));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* not: not $src1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,not) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_not.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = INVSI (* FLD (i_src1));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* not1: not $lit1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,not1) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_not1.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = INVSI (FLD (f_src1));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* not2: not $src1, $lit2, $dst */

SEM_PC
SEM_FN_NAME (i960base,not2) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_not2.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = INVSI (* FLD (i_src1));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* not3: not $lit1, $lit2, $dst */

SEM_PC
SEM_FN_NAME (i960base,not3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_not3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = INVSI (FLD (f_src1));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ornot: ornot $src1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,ornot) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = ORSI (* FLD (i_src2), INVSI (* FLD (i_src1)));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ornot1: ornot $lit1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,ornot1) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo1.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = ORSI (* FLD (i_src2), INVSI (FLD (f_src1)));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ornot2: ornot $src1, $lit2, $dst */

SEM_PC
SEM_FN_NAME (i960base,ornot2) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo2.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = ORSI (FLD (f_src2), INVSI (* FLD (i_src1)));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ornot3: ornot $lit1, $lit2, $dst */

SEM_PC
SEM_FN_NAME (i960base,ornot3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulo3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = ORSI (FLD (f_src2), INVSI (FLD (f_src1)));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* clrbit: clrbit $src1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,clrbit) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_notbit.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = ANDSI (INVSI (SLLSI (1, * FLD (i_src1))), * FLD (i_src2));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* clrbit1: clrbit $lit1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,clrbit1) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_notbit1.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = ANDSI (INVSI (SLLSI (1, FLD (f_src1))), * FLD (i_src2));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* clrbit2: clrbit $src1, $lit2, $dst */

SEM_PC
SEM_FN_NAME (i960base,clrbit2) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_notbit2.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = ANDSI (INVSI (SLLSI (1, * FLD (i_src1))), FLD (f_src2));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* clrbit3: clrbit $lit1, $lit2, $dst */

SEM_PC
SEM_FN_NAME (i960base,clrbit3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_notbit3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = ANDSI (INVSI (SLLSI (1, FLD (f_src1))), FLD (f_src2));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* shlo: shlo $src1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,shlo) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_shlo.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = (GEUSI (* FLD (i_src1), 32)) ? (0) : (SLLSI (* FLD (i_src2), * FLD (i_src1)));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* shlo1: shlo $lit1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,shlo1) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_shlo1.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = (GEUSI (FLD (f_src1), 32)) ? (0) : (SLLSI (* FLD (i_src2), FLD (f_src1)));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* shlo2: shlo $src1, $lit2, $dst */

SEM_PC
SEM_FN_NAME (i960base,shlo2) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_shlo2.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = (GEUSI (* FLD (i_src1), 32)) ? (0) : (SLLSI (FLD (f_src2), * FLD (i_src1)));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* shlo3: shlo $lit1, $lit2, $dst */

SEM_PC
SEM_FN_NAME (i960base,shlo3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_shlo3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = (GEUSI (FLD (f_src1), 32)) ? (0) : (SLLSI (FLD (f_src2), FLD (f_src1)));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* shro: shro $src1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,shro) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_shlo.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = (GEUSI (* FLD (i_src1), 32)) ? (0) : (SRLSI (* FLD (i_src2), * FLD (i_src1)));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* shro1: shro $lit1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,shro1) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_shlo1.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = (GEUSI (FLD (f_src1), 32)) ? (0) : (SRLSI (* FLD (i_src2), FLD (f_src1)));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* shro2: shro $src1, $lit2, $dst */

SEM_PC
SEM_FN_NAME (i960base,shro2) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_shlo2.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = (GEUSI (* FLD (i_src1), 32)) ? (0) : (SRLSI (FLD (f_src2), * FLD (i_src1)));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* shro3: shro $lit1, $lit2, $dst */

SEM_PC
SEM_FN_NAME (i960base,shro3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_shlo3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = (GEUSI (FLD (f_src1), 32)) ? (0) : (SRLSI (FLD (f_src2), FLD (f_src1)));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* shli: shli $src1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,shli) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_shlo.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = (GEUSI (* FLD (i_src1), 32)) ? (0) : (SLLSI (* FLD (i_src2), * FLD (i_src1)));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* shli1: shli $lit1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,shli1) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_shlo1.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = (GEUSI (FLD (f_src1), 32)) ? (0) : (SLLSI (* FLD (i_src2), FLD (f_src1)));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* shli2: shli $src1, $lit2, $dst */

SEM_PC
SEM_FN_NAME (i960base,shli2) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_shlo2.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = (GEUSI (* FLD (i_src1), 32)) ? (0) : (SLLSI (FLD (f_src2), * FLD (i_src1)));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* shli3: shli $lit1, $lit2, $dst */

SEM_PC
SEM_FN_NAME (i960base,shli3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_shlo3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = (GEUSI (FLD (f_src1), 32)) ? (0) : (SLLSI (FLD (f_src2), FLD (f_src1)));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* shri: shri $src1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,shri) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_shlo.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = (GEUSI (* FLD (i_src1), 32)) ? (SRASI (* FLD (i_src2), 31)) : (SRASI (* FLD (i_src2), * FLD (i_src1)));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* shri1: shri $lit1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,shri1) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_shlo1.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = (GEUSI (FLD (f_src1), 32)) ? (SRASI (* FLD (i_src2), 31)) : (SRASI (* FLD (i_src2), FLD (f_src1)));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* shri2: shri $src1, $lit2, $dst */

SEM_PC
SEM_FN_NAME (i960base,shri2) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_shlo2.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = (GEUSI (* FLD (i_src1), 32)) ? (SRASI (FLD (f_src2), 31)) : (SRASI (FLD (f_src2), * FLD (i_src1)));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* shri3: shri $lit1, $lit2, $dst */

SEM_PC
SEM_FN_NAME (i960base,shri3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_shlo3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = (GEUSI (FLD (f_src1), 32)) ? (SRASI (FLD (f_src2), 31)) : (SRASI (FLD (f_src2), FLD (f_src1)));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* emul: emul $src1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,emul) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_emul.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  DI tmp_temp;
  SI tmp_dregno;
  tmp_temp = MULDI (ZEXTSIDI (* FLD (i_src1)), ZEXTSIDI (* FLD (i_src2)));
  tmp_dregno = FLD (f_srcdst);
  {
    SI opval = TRUNCDISI (tmp_temp);
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }
  {
    SI opval = TRUNCDISI (SRLDI (tmp_temp, 32));
    CPU (h_gr[((FLD (f_srcdst)) + (1))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-1", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* emul1: emul $lit1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,emul1) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_emul1.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  DI tmp_temp;
  SI tmp_dregno;
  tmp_temp = MULDI (ZEXTSIDI (FLD (f_src1)), ZEXTSIDI (* FLD (i_src2)));
  tmp_dregno = FLD (f_srcdst);
  {
    SI opval = TRUNCDISI (tmp_temp);
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }
  {
    SI opval = TRUNCDISI (SRLDI (tmp_temp, 32));
    CPU (h_gr[((FLD (f_srcdst)) + (1))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-1", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* emul2: emul $src1, $lit2, $dst */

SEM_PC
SEM_FN_NAME (i960base,emul2) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_emul2.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  DI tmp_temp;
  SI tmp_dregno;
  tmp_temp = MULDI (ZEXTSIDI (* FLD (i_src1)), ZEXTSIDI (FLD (f_src2)));
  tmp_dregno = FLD (f_srcdst);
  {
    SI opval = TRUNCDISI (tmp_temp);
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }
  {
    SI opval = TRUNCDISI (SRLDI (tmp_temp, 32));
    CPU (h_gr[((FLD (f_srcdst)) + (1))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-1", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* emul3: emul $lit1, $lit2, $dst */

SEM_PC
SEM_FN_NAME (i960base,emul3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_emul3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  DI tmp_temp;
  SI tmp_dregno;
  tmp_temp = MULDI (ZEXTSIDI (FLD (f_src1)), ZEXTSIDI (FLD (f_src2)));
  tmp_dregno = FLD (f_srcdst);
  {
    SI opval = TRUNCDISI (tmp_temp);
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }
  {
    SI opval = TRUNCDISI (SRLDI (tmp_temp, 32));
    CPU (h_gr[((FLD (f_srcdst)) + (1))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-1", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* mov: mov $src1, $dst */

SEM_PC
SEM_FN_NAME (i960base,mov) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_not2.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = * FLD (i_src1);
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* mov1: mov $lit1, $dst */

SEM_PC
SEM_FN_NAME (i960base,mov1) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_not3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = FLD (f_src1);
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* movl: movl $src1, $dst */

SEM_PC
SEM_FN_NAME (i960base,movl) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_movl.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_dregno;
  SI tmp_sregno;
  tmp_dregno = FLD (f_srcdst);
  tmp_sregno = FLD (f_src1);
  {
    SI opval = * FLD (i_src1);
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_src1)) + (1))]);
    CPU (h_gr[((FLD (f_srcdst)) + (1))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-1", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* movl1: movl $lit1, $dst */

SEM_PC
SEM_FN_NAME (i960base,movl1) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_movl1.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_dregno;
  tmp_dregno = FLD (f_srcdst);
  {
    SI opval = FLD (f_src1);
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }
  {
    SI opval = 0;
    CPU (h_gr[((FLD (f_srcdst)) + (1))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-1", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* movt: movt $src1, $dst */

SEM_PC
SEM_FN_NAME (i960base,movt) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_movt.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_dregno;
  SI tmp_sregno;
  tmp_dregno = FLD (f_srcdst);
  tmp_sregno = FLD (f_src1);
  {
    SI opval = * FLD (i_src1);
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_src1)) + (1))]);
    CPU (h_gr[((FLD (f_srcdst)) + (1))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-1", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_src1)) + (2))]);
    CPU (h_gr[((FLD (f_srcdst)) + (2))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-2", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* movt1: movt $lit1, $dst */

SEM_PC
SEM_FN_NAME (i960base,movt1) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_movt1.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_dregno;
  tmp_dregno = FLD (f_srcdst);
  {
    SI opval = FLD (f_src1);
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }
  {
    SI opval = 0;
    CPU (h_gr[((FLD (f_srcdst)) + (1))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-1", 'x', opval);
  }
  {
    SI opval = 0;
    CPU (h_gr[((FLD (f_srcdst)) + (2))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-2", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* movq: movq $src1, $dst */

SEM_PC
SEM_FN_NAME (i960base,movq) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_movq.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_dregno;
  SI tmp_sregno;
  tmp_dregno = FLD (f_srcdst);
  tmp_sregno = FLD (f_src1);
  {
    SI opval = * FLD (i_src1);
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_src1)) + (1))]);
    CPU (h_gr[((FLD (f_srcdst)) + (1))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-1", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_src1)) + (2))]);
    CPU (h_gr[((FLD (f_srcdst)) + (2))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-2", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_src1)) + (3))]);
    CPU (h_gr[((FLD (f_srcdst)) + (3))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-3", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* movq1: movq $lit1, $dst */

SEM_PC
SEM_FN_NAME (i960base,movq1) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_movq1.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_dregno;
  tmp_dregno = FLD (f_srcdst);
  {
    SI opval = FLD (f_src1);
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }
  {
    SI opval = 0;
    CPU (h_gr[((FLD (f_srcdst)) + (1))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-1", 'x', opval);
  }
  {
    SI opval = 0;
    CPU (h_gr[((FLD (f_srcdst)) + (2))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-2", 'x', opval);
  }
  {
    SI opval = 0;
    CPU (h_gr[((FLD (f_srcdst)) + (3))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-3", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* modpc: modpc $src1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,modpc) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_modpc.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = * FLD (i_src2);
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* modac: modac $src1, $src2, $dst */

SEM_PC
SEM_FN_NAME (i960base,modac) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_modpc.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = * FLD (i_src2);
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* lda-offset: lda $offset, $dst */

SEM_PC
SEM_FN_NAME (i960base,lda_offset) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_lda_offset.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = FLD (f_offset);
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* lda-indirect-offset: lda $offset($abase), $dst */

SEM_PC
SEM_FN_NAME (i960base,lda_indirect_offset) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_lda_indirect_offset.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = ADDSI (FLD (f_offset), * FLD (i_abase));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* lda-indirect: lda ($abase), $dst */

SEM_PC
SEM_FN_NAME (i960base,lda_indirect) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_lda_indirect.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = * FLD (i_abase);
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* lda-indirect-index: lda ($abase)[$index*S$scale], $dst */

SEM_PC
SEM_FN_NAME (i960base,lda_indirect_index) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_lda_indirect_index.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = ADDSI (* FLD (i_abase), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale))));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* lda-disp: lda $optdisp, $dst */

SEM_PC
SEM_FN_NAME (i960base,lda_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_lda_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

  {
    SI opval = FLD (f_optdisp);
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* lda-indirect-disp: lda $optdisp($abase), $dst */

SEM_PC
SEM_FN_NAME (i960base,lda_indirect_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_lda_indirect_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

  {
    SI opval = ADDSI (FLD (f_optdisp), * FLD (i_abase));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* lda-index-disp: lda $optdisp[$index*S$scale], $dst */

SEM_PC
SEM_FN_NAME (i960base,lda_index_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_lda_index_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

  {
    SI opval = ADDSI (FLD (f_optdisp), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale))));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* lda-indirect-index-disp: lda $optdisp($abase)[$index*S$scale], $dst */

SEM_PC
SEM_FN_NAME (i960base,lda_indirect_index_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_lda_indirect_index_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

  {
    SI opval = ADDSI (FLD (f_optdisp), ADDSI (* FLD (i_abase), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale)))));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ld-offset: ld $offset, $dst */

SEM_PC
SEM_FN_NAME (i960base,ld_offset) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ld_offset.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = GETMEMSI (current_cpu, pc, FLD (f_offset));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ld-indirect-offset: ld $offset($abase), $dst */

SEM_PC
SEM_FN_NAME (i960base,ld_indirect_offset) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ld_indirect_offset.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (FLD (f_offset), * FLD (i_abase)));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ld-indirect: ld ($abase), $dst */

SEM_PC
SEM_FN_NAME (i960base,ld_indirect) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ld_indirect.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = GETMEMSI (current_cpu, pc, * FLD (i_abase));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ld-indirect-index: ld ($abase)[$index*S$scale], $dst */

SEM_PC
SEM_FN_NAME (i960base,ld_indirect_index) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ld_indirect_index.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (* FLD (i_abase), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale)))));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ld-disp: ld $optdisp, $dst */

SEM_PC
SEM_FN_NAME (i960base,ld_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ld_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

  {
    SI opval = GETMEMSI (current_cpu, pc, FLD (f_optdisp));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ld-indirect-disp: ld $optdisp($abase), $dst */

SEM_PC
SEM_FN_NAME (i960base,ld_indirect_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ld_indirect_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (FLD (f_optdisp), * FLD (i_abase)));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ld-index-disp: ld $optdisp[$index*S$scale], $dst */

SEM_PC
SEM_FN_NAME (i960base,ld_index_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ld_index_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (FLD (f_optdisp), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale)))));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ld-indirect-index-disp: ld $optdisp($abase)[$index*S$scale], $dst */

SEM_PC
SEM_FN_NAME (i960base,ld_indirect_index_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ld_indirect_index_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (FLD (f_optdisp), ADDSI (* FLD (i_abase), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale))))));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldob-offset: ldob $offset, $dst */

SEM_PC
SEM_FN_NAME (i960base,ldob_offset) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldob_offset.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = GETMEMUQI (current_cpu, pc, FLD (f_offset));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldob-indirect-offset: ldob $offset($abase), $dst */

SEM_PC
SEM_FN_NAME (i960base,ldob_indirect_offset) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldob_indirect_offset.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = GETMEMUQI (current_cpu, pc, ADDSI (FLD (f_offset), * FLD (i_abase)));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldob-indirect: ldob ($abase), $dst */

SEM_PC
SEM_FN_NAME (i960base,ldob_indirect) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldob_indirect.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = GETMEMUQI (current_cpu, pc, * FLD (i_abase));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldob-indirect-index: ldob ($abase)[$index*S$scale], $dst */

SEM_PC
SEM_FN_NAME (i960base,ldob_indirect_index) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldob_indirect_index.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = GETMEMUQI (current_cpu, pc, ADDSI (* FLD (i_abase), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale)))));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldob-disp: ldob $optdisp, $dst */

SEM_PC
SEM_FN_NAME (i960base,ldob_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldob_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

  {
    SI opval = GETMEMUQI (current_cpu, pc, FLD (f_optdisp));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldob-indirect-disp: ldob $optdisp($abase), $dst */

SEM_PC
SEM_FN_NAME (i960base,ldob_indirect_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldob_indirect_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

  {
    SI opval = GETMEMUQI (current_cpu, pc, ADDSI (FLD (f_optdisp), * FLD (i_abase)));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldob-index-disp: ldob $optdisp[$index*S$scale], $dst */

SEM_PC
SEM_FN_NAME (i960base,ldob_index_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldob_index_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

  {
    SI opval = GETMEMUQI (current_cpu, pc, ADDSI (FLD (f_optdisp), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale)))));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldob-indirect-index-disp: ldob $optdisp($abase)[$index*S$scale], $dst */

SEM_PC
SEM_FN_NAME (i960base,ldob_indirect_index_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldob_indirect_index_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

  {
    SI opval = GETMEMUQI (current_cpu, pc, ADDSI (FLD (f_optdisp), ADDSI (* FLD (i_abase), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale))))));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldos-offset: ldos $offset, $dst */

SEM_PC
SEM_FN_NAME (i960base,ldos_offset) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldos_offset.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = GETMEMUHI (current_cpu, pc, FLD (f_offset));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldos-indirect-offset: ldos $offset($abase), $dst */

SEM_PC
SEM_FN_NAME (i960base,ldos_indirect_offset) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldos_indirect_offset.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = GETMEMUHI (current_cpu, pc, ADDSI (FLD (f_offset), * FLD (i_abase)));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldos-indirect: ldos ($abase), $dst */

SEM_PC
SEM_FN_NAME (i960base,ldos_indirect) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldos_indirect.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = GETMEMUHI (current_cpu, pc, * FLD (i_abase));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldos-indirect-index: ldos ($abase)[$index*S$scale], $dst */

SEM_PC
SEM_FN_NAME (i960base,ldos_indirect_index) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldos_indirect_index.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = GETMEMUHI (current_cpu, pc, ADDSI (* FLD (i_abase), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale)))));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldos-disp: ldos $optdisp, $dst */

SEM_PC
SEM_FN_NAME (i960base,ldos_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldos_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

  {
    SI opval = GETMEMUHI (current_cpu, pc, FLD (f_optdisp));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldos-indirect-disp: ldos $optdisp($abase), $dst */

SEM_PC
SEM_FN_NAME (i960base,ldos_indirect_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldos_indirect_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

  {
    SI opval = GETMEMUHI (current_cpu, pc, ADDSI (FLD (f_optdisp), * FLD (i_abase)));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldos-index-disp: ldos $optdisp[$index*S$scale], $dst */

SEM_PC
SEM_FN_NAME (i960base,ldos_index_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldos_index_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

  {
    SI opval = GETMEMUHI (current_cpu, pc, ADDSI (FLD (f_optdisp), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale)))));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldos-indirect-index-disp: ldos $optdisp($abase)[$index*S$scale], $dst */

SEM_PC
SEM_FN_NAME (i960base,ldos_indirect_index_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldos_indirect_index_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

  {
    SI opval = GETMEMUHI (current_cpu, pc, ADDSI (FLD (f_optdisp), ADDSI (* FLD (i_abase), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale))))));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldib-offset: ldib $offset, $dst */

SEM_PC
SEM_FN_NAME (i960base,ldib_offset) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldib_offset.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = GETMEMQI (current_cpu, pc, FLD (f_offset));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldib-indirect-offset: ldib $offset($abase), $dst */

SEM_PC
SEM_FN_NAME (i960base,ldib_indirect_offset) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldib_indirect_offset.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = GETMEMQI (current_cpu, pc, ADDSI (FLD (f_offset), * FLD (i_abase)));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldib-indirect: ldib ($abase), $dst */

SEM_PC
SEM_FN_NAME (i960base,ldib_indirect) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldib_indirect.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = GETMEMQI (current_cpu, pc, * FLD (i_abase));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldib-indirect-index: ldib ($abase)[$index*S$scale], $dst */

SEM_PC
SEM_FN_NAME (i960base,ldib_indirect_index) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldib_indirect_index.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = GETMEMQI (current_cpu, pc, ADDSI (* FLD (i_abase), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale)))));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldib-disp: ldib $optdisp, $dst */

SEM_PC
SEM_FN_NAME (i960base,ldib_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldib_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

  {
    SI opval = GETMEMQI (current_cpu, pc, FLD (f_optdisp));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldib-indirect-disp: ldib $optdisp($abase), $dst */

SEM_PC
SEM_FN_NAME (i960base,ldib_indirect_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldib_indirect_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

  {
    SI opval = GETMEMQI (current_cpu, pc, ADDSI (FLD (f_optdisp), * FLD (i_abase)));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldib-index-disp: ldib $optdisp[$index*S$scale], $dst */

SEM_PC
SEM_FN_NAME (i960base,ldib_index_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldib_index_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

  {
    SI opval = GETMEMQI (current_cpu, pc, ADDSI (FLD (f_optdisp), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale)))));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldib-indirect-index-disp: ldib $optdisp($abase)[$index*S$scale], $dst */

SEM_PC
SEM_FN_NAME (i960base,ldib_indirect_index_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldib_indirect_index_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

  {
    SI opval = GETMEMQI (current_cpu, pc, ADDSI (FLD (f_optdisp), ADDSI (* FLD (i_abase), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale))))));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldis-offset: ldis $offset, $dst */

SEM_PC
SEM_FN_NAME (i960base,ldis_offset) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldis_offset.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = GETMEMHI (current_cpu, pc, FLD (f_offset));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldis-indirect-offset: ldis $offset($abase), $dst */

SEM_PC
SEM_FN_NAME (i960base,ldis_indirect_offset) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldis_indirect_offset.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = GETMEMHI (current_cpu, pc, ADDSI (FLD (f_offset), * FLD (i_abase)));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldis-indirect: ldis ($abase), $dst */

SEM_PC
SEM_FN_NAME (i960base,ldis_indirect) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldis_indirect.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = GETMEMHI (current_cpu, pc, * FLD (i_abase));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldis-indirect-index: ldis ($abase)[$index*S$scale], $dst */

SEM_PC
SEM_FN_NAME (i960base,ldis_indirect_index) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldis_indirect_index.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = GETMEMHI (current_cpu, pc, ADDSI (* FLD (i_abase), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale)))));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldis-disp: ldis $optdisp, $dst */

SEM_PC
SEM_FN_NAME (i960base,ldis_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldis_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

  {
    SI opval = GETMEMHI (current_cpu, pc, FLD (f_optdisp));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldis-indirect-disp: ldis $optdisp($abase), $dst */

SEM_PC
SEM_FN_NAME (i960base,ldis_indirect_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldis_indirect_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

  {
    SI opval = GETMEMHI (current_cpu, pc, ADDSI (FLD (f_optdisp), * FLD (i_abase)));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldis-index-disp: ldis $optdisp[$index*S$scale], $dst */

SEM_PC
SEM_FN_NAME (i960base,ldis_index_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldis_index_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

  {
    SI opval = GETMEMHI (current_cpu, pc, ADDSI (FLD (f_optdisp), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale)))));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldis-indirect-index-disp: ldis $optdisp($abase)[$index*S$scale], $dst */

SEM_PC
SEM_FN_NAME (i960base,ldis_indirect_index_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldis_indirect_index_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

  {
    SI opval = GETMEMHI (current_cpu, pc, ADDSI (FLD (f_optdisp), ADDSI (* FLD (i_abase), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale))))));
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldl-offset: ldl $offset, $dst */

SEM_PC
SEM_FN_NAME (i960base,ldl_offset) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldl_offset.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_temp;
  SI tmp_dregno;
  tmp_dregno = FLD (f_srcdst);
  tmp_temp = FLD (f_offset);
  {
    SI opval = GETMEMSI (current_cpu, pc, tmp_temp);
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (tmp_temp, 4));
    CPU (h_gr[((FLD (f_srcdst)) + (1))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-1", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* ldl-indirect-offset: ldl $offset($abase), $dst */

SEM_PC
SEM_FN_NAME (i960base,ldl_indirect_offset) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldl_indirect_offset.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_temp;
  SI tmp_dregno;
  tmp_dregno = FLD (f_srcdst);
  tmp_temp = ADDSI (FLD (f_offset), * FLD (i_abase));
  {
    SI opval = GETMEMSI (current_cpu, pc, tmp_temp);
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (tmp_temp, 4));
    CPU (h_gr[((FLD (f_srcdst)) + (1))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-1", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* ldl-indirect: ldl ($abase), $dst */

SEM_PC
SEM_FN_NAME (i960base,ldl_indirect) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldl_indirect.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_temp;
  SI tmp_dregno;
  tmp_dregno = FLD (f_srcdst);
  tmp_temp = * FLD (i_abase);
  {
    SI opval = GETMEMSI (current_cpu, pc, tmp_temp);
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (tmp_temp, 4));
    CPU (h_gr[((FLD (f_srcdst)) + (1))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-1", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* ldl-indirect-index: ldl ($abase)[$index*S$scale], $dst */

SEM_PC
SEM_FN_NAME (i960base,ldl_indirect_index) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldl_indirect_index.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_temp;
  SI tmp_dregno;
  tmp_dregno = FLD (f_srcdst);
  tmp_temp = ADDSI (* FLD (i_abase), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale))));
  {
    SI opval = GETMEMSI (current_cpu, pc, tmp_temp);
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (tmp_temp, 4));
    CPU (h_gr[((FLD (f_srcdst)) + (1))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-1", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* ldl-disp: ldl $optdisp, $dst */

SEM_PC
SEM_FN_NAME (i960base,ldl_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldl_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

{
  SI tmp_temp;
  SI tmp_dregno;
  tmp_dregno = FLD (f_srcdst);
  tmp_temp = FLD (f_optdisp);
  {
    SI opval = GETMEMSI (current_cpu, pc, tmp_temp);
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (tmp_temp, 4));
    CPU (h_gr[((FLD (f_srcdst)) + (1))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-1", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* ldl-indirect-disp: ldl $optdisp($abase), $dst */

SEM_PC
SEM_FN_NAME (i960base,ldl_indirect_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldl_indirect_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

{
  SI tmp_temp;
  SI tmp_dregno;
  tmp_dregno = FLD (f_srcdst);
  tmp_temp = ADDSI (FLD (f_optdisp), * FLD (i_abase));
  {
    SI opval = GETMEMSI (current_cpu, pc, tmp_temp);
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (tmp_temp, 4));
    CPU (h_gr[((FLD (f_srcdst)) + (1))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-1", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* ldl-index-disp: ldl $optdisp[$index*S$scale], $dst */

SEM_PC
SEM_FN_NAME (i960base,ldl_index_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldl_index_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

{
  SI tmp_temp;
  SI tmp_dregno;
  tmp_dregno = FLD (f_srcdst);
  tmp_temp = ADDSI (FLD (f_optdisp), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale))));
  {
    SI opval = GETMEMSI (current_cpu, pc, tmp_temp);
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (tmp_temp, 4));
    CPU (h_gr[((FLD (f_srcdst)) + (1))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-1", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* ldl-indirect-index-disp: ldl $optdisp($abase)[$index*S$scale], $dst */

SEM_PC
SEM_FN_NAME (i960base,ldl_indirect_index_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldl_indirect_index_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

{
  SI tmp_temp;
  SI tmp_dregno;
  tmp_dregno = FLD (f_srcdst);
  tmp_temp = ADDSI (FLD (f_optdisp), ADDSI (* FLD (i_abase), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale)))));
  {
    SI opval = GETMEMSI (current_cpu, pc, tmp_temp);
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (tmp_temp, 4));
    CPU (h_gr[((FLD (f_srcdst)) + (1))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-1", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* ldt-offset: ldt $offset, $dst */

SEM_PC
SEM_FN_NAME (i960base,ldt_offset) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldt_offset.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_temp;
  SI tmp_dregno;
  tmp_dregno = FLD (f_srcdst);
  tmp_temp = FLD (f_offset);
  {
    SI opval = GETMEMSI (current_cpu, pc, tmp_temp);
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (tmp_temp, 4));
    CPU (h_gr[((FLD (f_srcdst)) + (1))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-1", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (tmp_temp, 8));
    CPU (h_gr[((FLD (f_srcdst)) + (2))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-2", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* ldt-indirect-offset: ldt $offset($abase), $dst */

SEM_PC
SEM_FN_NAME (i960base,ldt_indirect_offset) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldt_indirect_offset.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_temp;
  SI tmp_dregno;
  tmp_dregno = FLD (f_srcdst);
  tmp_temp = ADDSI (FLD (f_offset), * FLD (i_abase));
  {
    SI opval = GETMEMSI (current_cpu, pc, tmp_temp);
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (tmp_temp, 4));
    CPU (h_gr[((FLD (f_srcdst)) + (1))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-1", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (tmp_temp, 8));
    CPU (h_gr[((FLD (f_srcdst)) + (2))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-2", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* ldt-indirect: ldt ($abase), $dst */

SEM_PC
SEM_FN_NAME (i960base,ldt_indirect) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldt_indirect.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_temp;
  SI tmp_dregno;
  tmp_dregno = FLD (f_srcdst);
  tmp_temp = * FLD (i_abase);
  {
    SI opval = GETMEMSI (current_cpu, pc, tmp_temp);
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (tmp_temp, 4));
    CPU (h_gr[((FLD (f_srcdst)) + (1))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-1", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (tmp_temp, 8));
    CPU (h_gr[((FLD (f_srcdst)) + (2))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-2", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* ldt-indirect-index: ldt ($abase)[$index*S$scale], $dst */

SEM_PC
SEM_FN_NAME (i960base,ldt_indirect_index) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldt_indirect_index.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_temp;
  SI tmp_dregno;
  tmp_dregno = FLD (f_srcdst);
  tmp_temp = ADDSI (* FLD (i_abase), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale))));
  {
    SI opval = GETMEMSI (current_cpu, pc, tmp_temp);
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (tmp_temp, 4));
    CPU (h_gr[((FLD (f_srcdst)) + (1))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-1", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (tmp_temp, 8));
    CPU (h_gr[((FLD (f_srcdst)) + (2))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-2", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* ldt-disp: ldt $optdisp, $dst */

SEM_PC
SEM_FN_NAME (i960base,ldt_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldt_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

{
  SI tmp_temp;
  SI tmp_dregno;
  tmp_dregno = FLD (f_srcdst);
  tmp_temp = FLD (f_optdisp);
  {
    SI opval = GETMEMSI (current_cpu, pc, tmp_temp);
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (tmp_temp, 4));
    CPU (h_gr[((FLD (f_srcdst)) + (1))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-1", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (tmp_temp, 8));
    CPU (h_gr[((FLD (f_srcdst)) + (2))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-2", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* ldt-indirect-disp: ldt $optdisp($abase), $dst */

SEM_PC
SEM_FN_NAME (i960base,ldt_indirect_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldt_indirect_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

{
  SI tmp_temp;
  SI tmp_dregno;
  tmp_dregno = FLD (f_srcdst);
  tmp_temp = ADDSI (FLD (f_optdisp), * FLD (i_abase));
  {
    SI opval = GETMEMSI (current_cpu, pc, tmp_temp);
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (tmp_temp, 4));
    CPU (h_gr[((FLD (f_srcdst)) + (1))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-1", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (tmp_temp, 8));
    CPU (h_gr[((FLD (f_srcdst)) + (2))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-2", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* ldt-index-disp: ldt $optdisp[$index*S$scale], $dst */

SEM_PC
SEM_FN_NAME (i960base,ldt_index_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldt_index_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

{
  SI tmp_temp;
  SI tmp_dregno;
  tmp_dregno = FLD (f_srcdst);
  tmp_temp = ADDSI (FLD (f_optdisp), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale))));
  {
    SI opval = GETMEMSI (current_cpu, pc, tmp_temp);
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (tmp_temp, 4));
    CPU (h_gr[((FLD (f_srcdst)) + (1))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-1", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (tmp_temp, 8));
    CPU (h_gr[((FLD (f_srcdst)) + (2))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-2", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* ldt-indirect-index-disp: ldt $optdisp($abase)[$index*S$scale], $dst */

SEM_PC
SEM_FN_NAME (i960base,ldt_indirect_index_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldt_indirect_index_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

{
  SI tmp_temp;
  SI tmp_dregno;
  tmp_dregno = FLD (f_srcdst);
  tmp_temp = ADDSI (FLD (f_optdisp), ADDSI (* FLD (i_abase), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale)))));
  {
    SI opval = GETMEMSI (current_cpu, pc, tmp_temp);
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (tmp_temp, 4));
    CPU (h_gr[((FLD (f_srcdst)) + (1))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-1", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (tmp_temp, 8));
    CPU (h_gr[((FLD (f_srcdst)) + (2))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-2", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* ldq-offset: ldq $offset, $dst */

SEM_PC
SEM_FN_NAME (i960base,ldq_offset) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldq_offset.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_temp;
  SI tmp_dregno;
  tmp_dregno = FLD (f_srcdst);
  tmp_temp = FLD (f_offset);
  {
    SI opval = GETMEMSI (current_cpu, pc, tmp_temp);
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (tmp_temp, 4));
    CPU (h_gr[((FLD (f_srcdst)) + (1))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-1", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (tmp_temp, 8));
    CPU (h_gr[((FLD (f_srcdst)) + (2))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-2", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (tmp_temp, 12));
    CPU (h_gr[((FLD (f_srcdst)) + (3))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-3", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* ldq-indirect-offset: ldq $offset($abase), $dst */

SEM_PC
SEM_FN_NAME (i960base,ldq_indirect_offset) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldq_indirect_offset.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_temp;
  SI tmp_dregno;
  tmp_dregno = FLD (f_srcdst);
  tmp_temp = ADDSI (FLD (f_offset), * FLD (i_abase));
  {
    SI opval = GETMEMSI (current_cpu, pc, tmp_temp);
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (tmp_temp, 4));
    CPU (h_gr[((FLD (f_srcdst)) + (1))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-1", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (tmp_temp, 8));
    CPU (h_gr[((FLD (f_srcdst)) + (2))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-2", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (tmp_temp, 12));
    CPU (h_gr[((FLD (f_srcdst)) + (3))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-3", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* ldq-indirect: ldq ($abase), $dst */

SEM_PC
SEM_FN_NAME (i960base,ldq_indirect) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldq_indirect.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_temp;
  SI tmp_dregno;
  tmp_dregno = FLD (f_srcdst);
  tmp_temp = * FLD (i_abase);
  {
    SI opval = GETMEMSI (current_cpu, pc, tmp_temp);
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (tmp_temp, 4));
    CPU (h_gr[((FLD (f_srcdst)) + (1))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-1", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (tmp_temp, 8));
    CPU (h_gr[((FLD (f_srcdst)) + (2))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-2", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (tmp_temp, 12));
    CPU (h_gr[((FLD (f_srcdst)) + (3))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-3", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* ldq-indirect-index: ldq ($abase)[$index*S$scale], $dst */

SEM_PC
SEM_FN_NAME (i960base,ldq_indirect_index) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldq_indirect_index.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_temp;
  SI tmp_dregno;
  tmp_dregno = FLD (f_srcdst);
  tmp_temp = ADDSI (* FLD (i_abase), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale))));
  {
    SI opval = GETMEMSI (current_cpu, pc, tmp_temp);
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (tmp_temp, 4));
    CPU (h_gr[((FLD (f_srcdst)) + (1))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-1", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (tmp_temp, 8));
    CPU (h_gr[((FLD (f_srcdst)) + (2))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-2", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (tmp_temp, 12));
    CPU (h_gr[((FLD (f_srcdst)) + (3))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-3", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* ldq-disp: ldq $optdisp, $dst */

SEM_PC
SEM_FN_NAME (i960base,ldq_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldq_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

{
  SI tmp_temp;
  SI tmp_dregno;
  tmp_dregno = FLD (f_srcdst);
  tmp_temp = FLD (f_optdisp);
  {
    SI opval = GETMEMSI (current_cpu, pc, tmp_temp);
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (tmp_temp, 4));
    CPU (h_gr[((FLD (f_srcdst)) + (1))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-1", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (tmp_temp, 8));
    CPU (h_gr[((FLD (f_srcdst)) + (2))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-2", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (tmp_temp, 12));
    CPU (h_gr[((FLD (f_srcdst)) + (3))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-3", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* ldq-indirect-disp: ldq $optdisp($abase), $dst */

SEM_PC
SEM_FN_NAME (i960base,ldq_indirect_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldq_indirect_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

{
  SI tmp_temp;
  SI tmp_dregno;
  tmp_dregno = FLD (f_srcdst);
  tmp_temp = ADDSI (FLD (f_optdisp), * FLD (i_abase));
  {
    SI opval = GETMEMSI (current_cpu, pc, tmp_temp);
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (tmp_temp, 4));
    CPU (h_gr[((FLD (f_srcdst)) + (1))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-1", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (tmp_temp, 8));
    CPU (h_gr[((FLD (f_srcdst)) + (2))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-2", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (tmp_temp, 12));
    CPU (h_gr[((FLD (f_srcdst)) + (3))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-3", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* ldq-index-disp: ldq $optdisp[$index*S$scale], $dst */

SEM_PC
SEM_FN_NAME (i960base,ldq_index_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldq_index_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

{
  SI tmp_temp;
  SI tmp_dregno;
  tmp_dregno = FLD (f_srcdst);
  tmp_temp = ADDSI (FLD (f_optdisp), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale))));
  {
    SI opval = GETMEMSI (current_cpu, pc, tmp_temp);
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (tmp_temp, 4));
    CPU (h_gr[((FLD (f_srcdst)) + (1))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-1", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (tmp_temp, 8));
    CPU (h_gr[((FLD (f_srcdst)) + (2))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-2", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (tmp_temp, 12));
    CPU (h_gr[((FLD (f_srcdst)) + (3))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-3", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* ldq-indirect-index-disp: ldq $optdisp($abase)[$index*S$scale], $dst */

SEM_PC
SEM_FN_NAME (i960base,ldq_indirect_index_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldq_indirect_index_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

{
  SI tmp_temp;
  SI tmp_dregno;
  tmp_dregno = FLD (f_srcdst);
  tmp_temp = ADDSI (FLD (f_optdisp), ADDSI (* FLD (i_abase), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale)))));
  {
    SI opval = GETMEMSI (current_cpu, pc, tmp_temp);
    * FLD (i_dst) = opval;
    TRACE_RESULT (current_cpu, abuf, "dst", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (tmp_temp, 4));
    CPU (h_gr[((FLD (f_srcdst)) + (1))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-1", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (tmp_temp, 8));
    CPU (h_gr[((FLD (f_srcdst)) + (2))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-2", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (tmp_temp, 12));
    CPU (h_gr[((FLD (f_srcdst)) + (3))]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-index-of-dst-3", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* st-offset: st $st_src, $offset */

SEM_PC
SEM_FN_NAME (i960base,st_offset) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_st_offset.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = * FLD (i_st_src);
    SETMEMSI (current_cpu, pc, FLD (f_offset), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* st-indirect-offset: st $st_src, $offset($abase) */

SEM_PC
SEM_FN_NAME (i960base,st_indirect_offset) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_st_indirect_offset.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = * FLD (i_st_src);
    SETMEMSI (current_cpu, pc, ADDSI (FLD (f_offset), * FLD (i_abase)), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* st-indirect: st $st_src, ($abase) */

SEM_PC
SEM_FN_NAME (i960base,st_indirect) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_st_indirect.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = * FLD (i_st_src);
    SETMEMSI (current_cpu, pc, * FLD (i_abase), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* st-indirect-index: st $st_src, ($abase)[$index*S$scale] */

SEM_PC
SEM_FN_NAME (i960base,st_indirect_index) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_st_indirect_index.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = * FLD (i_st_src);
    SETMEMSI (current_cpu, pc, ADDSI (* FLD (i_abase), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale)))), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* st-disp: st $st_src, $optdisp */

SEM_PC
SEM_FN_NAME (i960base,st_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_st_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

  {
    SI opval = * FLD (i_st_src);
    SETMEMSI (current_cpu, pc, FLD (f_optdisp), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* st-indirect-disp: st $st_src, $optdisp($abase) */

SEM_PC
SEM_FN_NAME (i960base,st_indirect_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_st_indirect_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

  {
    SI opval = * FLD (i_st_src);
    SETMEMSI (current_cpu, pc, ADDSI (FLD (f_optdisp), * FLD (i_abase)), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* st-index-disp: st $st_src, $optdisp[$index*S$scale */

SEM_PC
SEM_FN_NAME (i960base,st_index_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_st_index_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

  {
    SI opval = * FLD (i_st_src);
    SETMEMSI (current_cpu, pc, ADDSI (FLD (f_optdisp), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale)))), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* st-indirect-index-disp: st $st_src, $optdisp($abase)[$index*S$scale] */

SEM_PC
SEM_FN_NAME (i960base,st_indirect_index_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_st_indirect_index_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

  {
    SI opval = * FLD (i_st_src);
    SETMEMSI (current_cpu, pc, ADDSI (FLD (f_optdisp), ADDSI (* FLD (i_abase), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale))))), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* stob-offset: stob $st_src, $offset */

SEM_PC
SEM_FN_NAME (i960base,stob_offset) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stob_offset.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    QI opval = * FLD (i_st_src);
    SETMEMQI (current_cpu, pc, FLD (f_offset), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* stob-indirect-offset: stob $st_src, $offset($abase) */

SEM_PC
SEM_FN_NAME (i960base,stob_indirect_offset) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stob_indirect_offset.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    QI opval = * FLD (i_st_src);
    SETMEMQI (current_cpu, pc, ADDSI (FLD (f_offset), * FLD (i_abase)), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* stob-indirect: stob $st_src, ($abase) */

SEM_PC
SEM_FN_NAME (i960base,stob_indirect) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stob_indirect.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    QI opval = * FLD (i_st_src);
    SETMEMQI (current_cpu, pc, * FLD (i_abase), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* stob-indirect-index: stob $st_src, ($abase)[$index*S$scale] */

SEM_PC
SEM_FN_NAME (i960base,stob_indirect_index) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stob_indirect_index.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    QI opval = * FLD (i_st_src);
    SETMEMQI (current_cpu, pc, ADDSI (* FLD (i_abase), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale)))), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* stob-disp: stob $st_src, $optdisp */

SEM_PC
SEM_FN_NAME (i960base,stob_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stob_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

  {
    QI opval = * FLD (i_st_src);
    SETMEMQI (current_cpu, pc, FLD (f_optdisp), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* stob-indirect-disp: stob $st_src, $optdisp($abase) */

SEM_PC
SEM_FN_NAME (i960base,stob_indirect_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stob_indirect_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

  {
    QI opval = * FLD (i_st_src);
    SETMEMQI (current_cpu, pc, ADDSI (FLD (f_optdisp), * FLD (i_abase)), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* stob-index-disp: stob $st_src, $optdisp[$index*S$scale */

SEM_PC
SEM_FN_NAME (i960base,stob_index_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stob_index_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

  {
    QI opval = * FLD (i_st_src);
    SETMEMQI (current_cpu, pc, ADDSI (FLD (f_optdisp), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale)))), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* stob-indirect-index-disp: stob $st_src, $optdisp($abase)[$index*S$scale] */

SEM_PC
SEM_FN_NAME (i960base,stob_indirect_index_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stob_indirect_index_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

  {
    QI opval = * FLD (i_st_src);
    SETMEMQI (current_cpu, pc, ADDSI (FLD (f_optdisp), ADDSI (* FLD (i_abase), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale))))), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* stos-offset: stos $st_src, $offset */

SEM_PC
SEM_FN_NAME (i960base,stos_offset) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stos_offset.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    HI opval = * FLD (i_st_src);
    SETMEMHI (current_cpu, pc, FLD (f_offset), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* stos-indirect-offset: stos $st_src, $offset($abase) */

SEM_PC
SEM_FN_NAME (i960base,stos_indirect_offset) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stos_indirect_offset.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    HI opval = * FLD (i_st_src);
    SETMEMHI (current_cpu, pc, ADDSI (FLD (f_offset), * FLD (i_abase)), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* stos-indirect: stos $st_src, ($abase) */

SEM_PC
SEM_FN_NAME (i960base,stos_indirect) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stos_indirect.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    HI opval = * FLD (i_st_src);
    SETMEMHI (current_cpu, pc, * FLD (i_abase), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* stos-indirect-index: stos $st_src, ($abase)[$index*S$scale] */

SEM_PC
SEM_FN_NAME (i960base,stos_indirect_index) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stos_indirect_index.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    HI opval = * FLD (i_st_src);
    SETMEMHI (current_cpu, pc, ADDSI (* FLD (i_abase), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale)))), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* stos-disp: stos $st_src, $optdisp */

SEM_PC
SEM_FN_NAME (i960base,stos_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stos_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

  {
    HI opval = * FLD (i_st_src);
    SETMEMHI (current_cpu, pc, FLD (f_optdisp), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* stos-indirect-disp: stos $st_src, $optdisp($abase) */

SEM_PC
SEM_FN_NAME (i960base,stos_indirect_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stos_indirect_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

  {
    HI opval = * FLD (i_st_src);
    SETMEMHI (current_cpu, pc, ADDSI (FLD (f_optdisp), * FLD (i_abase)), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* stos-index-disp: stos $st_src, $optdisp[$index*S$scale */

SEM_PC
SEM_FN_NAME (i960base,stos_index_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stos_index_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

  {
    HI opval = * FLD (i_st_src);
    SETMEMHI (current_cpu, pc, ADDSI (FLD (f_optdisp), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale)))), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* stos-indirect-index-disp: stos $st_src, $optdisp($abase)[$index*S$scale] */

SEM_PC
SEM_FN_NAME (i960base,stos_indirect_index_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stos_indirect_index_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

  {
    HI opval = * FLD (i_st_src);
    SETMEMHI (current_cpu, pc, ADDSI (FLD (f_optdisp), ADDSI (* FLD (i_abase), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale))))), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* stl-offset: stl $st_src, $offset */

SEM_PC
SEM_FN_NAME (i960base,stl_offset) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stl_offset.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_sregno;
  tmp_sregno = FLD (f_srcdst);
  {
    SI opval = * FLD (i_st_src);
    SETMEMSI (current_cpu, pc, FLD (f_offset), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_srcdst)) + (1))]);
    SETMEMSI (current_cpu, pc, ADDSI (FLD (f_offset), 4), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* stl-indirect-offset: stl $st_src, $offset($abase) */

SEM_PC
SEM_FN_NAME (i960base,stl_indirect_offset) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stl_indirect_offset.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_sregno;
  tmp_sregno = FLD (f_srcdst);
  {
    SI opval = * FLD (i_st_src);
    SETMEMSI (current_cpu, pc, ADDSI (FLD (f_offset), * FLD (i_abase)), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_srcdst)) + (1))]);
    SETMEMSI (current_cpu, pc, ADDSI (ADDSI (FLD (f_offset), * FLD (i_abase)), 4), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* stl-indirect: stl $st_src, ($abase) */

SEM_PC
SEM_FN_NAME (i960base,stl_indirect) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stl_indirect.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_sregno;
  tmp_sregno = FLD (f_srcdst);
  {
    SI opval = * FLD (i_st_src);
    SETMEMSI (current_cpu, pc, * FLD (i_abase), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_srcdst)) + (1))]);
    SETMEMSI (current_cpu, pc, ADDSI (* FLD (i_abase), 4), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* stl-indirect-index: stl $st_src, ($abase)[$index*S$scale] */

SEM_PC
SEM_FN_NAME (i960base,stl_indirect_index) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stl_indirect_index.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_sregno;
  tmp_sregno = FLD (f_srcdst);
  {
    SI opval = * FLD (i_st_src);
    SETMEMSI (current_cpu, pc, ADDSI (* FLD (i_abase), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale)))), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_srcdst)) + (1))]);
    SETMEMSI (current_cpu, pc, ADDSI (ADDSI (* FLD (i_abase), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale)))), 4), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* stl-disp: stl $st_src, $optdisp */

SEM_PC
SEM_FN_NAME (i960base,stl_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stl_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

{
  SI tmp_sregno;
  tmp_sregno = FLD (f_srcdst);
  {
    SI opval = * FLD (i_st_src);
    SETMEMSI (current_cpu, pc, FLD (f_optdisp), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_srcdst)) + (1))]);
    SETMEMSI (current_cpu, pc, ADDSI (FLD (f_optdisp), 4), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* stl-indirect-disp: stl $st_src, $optdisp($abase) */

SEM_PC
SEM_FN_NAME (i960base,stl_indirect_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stl_indirect_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

{
  SI tmp_sregno;
  tmp_sregno = FLD (f_srcdst);
  {
    SI opval = * FLD (i_st_src);
    SETMEMSI (current_cpu, pc, ADDSI (FLD (f_optdisp), * FLD (i_abase)), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_srcdst)) + (1))]);
    SETMEMSI (current_cpu, pc, ADDSI (ADDSI (FLD (f_optdisp), * FLD (i_abase)), 4), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* stl-index-disp: stl $st_src, $optdisp[$index*S$scale */

SEM_PC
SEM_FN_NAME (i960base,stl_index_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stl_index_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

{
  SI tmp_sregno;
  tmp_sregno = FLD (f_srcdst);
  {
    SI opval = * FLD (i_st_src);
    SETMEMSI (current_cpu, pc, ADDSI (FLD (f_optdisp), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale)))), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_srcdst)) + (1))]);
    SETMEMSI (current_cpu, pc, ADDSI (ADDSI (FLD (f_optdisp), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale)))), 4), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* stl-indirect-index-disp: stl $st_src, $optdisp($abase)[$index*S$scale] */

SEM_PC
SEM_FN_NAME (i960base,stl_indirect_index_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stl_indirect_index_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

{
  SI tmp_sregno;
  tmp_sregno = FLD (f_srcdst);
  {
    SI opval = * FLD (i_st_src);
    SETMEMSI (current_cpu, pc, ADDSI (FLD (f_optdisp), ADDSI (* FLD (i_abase), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale))))), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_srcdst)) + (1))]);
    SETMEMSI (current_cpu, pc, ADDSI (ADDSI (FLD (f_optdisp), ADDSI (* FLD (i_abase), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale))))), 4), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* stt-offset: stt $st_src, $offset */

SEM_PC
SEM_FN_NAME (i960base,stt_offset) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stt_offset.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_sregno;
  tmp_sregno = FLD (f_srcdst);
  {
    SI opval = * FLD (i_st_src);
    SETMEMSI (current_cpu, pc, FLD (f_offset), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_srcdst)) + (1))]);
    SETMEMSI (current_cpu, pc, ADDSI (FLD (f_offset), 4), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_srcdst)) + (2))]);
    SETMEMSI (current_cpu, pc, ADDSI (FLD (f_offset), 8), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* stt-indirect-offset: stt $st_src, $offset($abase) */

SEM_PC
SEM_FN_NAME (i960base,stt_indirect_offset) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stt_indirect_offset.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_sregno;
  tmp_sregno = FLD (f_srcdst);
  {
    SI opval = * FLD (i_st_src);
    SETMEMSI (current_cpu, pc, ADDSI (FLD (f_offset), * FLD (i_abase)), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_srcdst)) + (1))]);
    SETMEMSI (current_cpu, pc, ADDSI (ADDSI (FLD (f_offset), * FLD (i_abase)), 4), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_srcdst)) + (2))]);
    SETMEMSI (current_cpu, pc, ADDSI (ADDSI (FLD (f_offset), * FLD (i_abase)), 8), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* stt-indirect: stt $st_src, ($abase) */

SEM_PC
SEM_FN_NAME (i960base,stt_indirect) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stt_indirect.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_sregno;
  tmp_sregno = FLD (f_srcdst);
  {
    SI opval = * FLD (i_st_src);
    SETMEMSI (current_cpu, pc, * FLD (i_abase), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_srcdst)) + (1))]);
    SETMEMSI (current_cpu, pc, ADDSI (* FLD (i_abase), 4), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_srcdst)) + (2))]);
    SETMEMSI (current_cpu, pc, ADDSI (* FLD (i_abase), 8), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* stt-indirect-index: stt $st_src, ($abase)[$index*S$scale] */

SEM_PC
SEM_FN_NAME (i960base,stt_indirect_index) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stt_indirect_index.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_sregno;
  tmp_sregno = FLD (f_srcdst);
  {
    SI opval = * FLD (i_st_src);
    SETMEMSI (current_cpu, pc, ADDSI (* FLD (i_abase), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale)))), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_srcdst)) + (1))]);
    SETMEMSI (current_cpu, pc, ADDSI (ADDSI (* FLD (i_abase), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale)))), 4), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_srcdst)) + (2))]);
    SETMEMSI (current_cpu, pc, ADDSI (ADDSI (* FLD (i_abase), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale)))), 8), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* stt-disp: stt $st_src, $optdisp */

SEM_PC
SEM_FN_NAME (i960base,stt_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stt_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

{
  SI tmp_sregno;
  tmp_sregno = FLD (f_srcdst);
  {
    SI opval = * FLD (i_st_src);
    SETMEMSI (current_cpu, pc, FLD (f_optdisp), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_srcdst)) + (1))]);
    SETMEMSI (current_cpu, pc, ADDSI (FLD (f_optdisp), 4), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_srcdst)) + (2))]);
    SETMEMSI (current_cpu, pc, ADDSI (FLD (f_optdisp), 8), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* stt-indirect-disp: stt $st_src, $optdisp($abase) */

SEM_PC
SEM_FN_NAME (i960base,stt_indirect_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stt_indirect_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

{
  SI tmp_sregno;
  tmp_sregno = FLD (f_srcdst);
  {
    SI opval = * FLD (i_st_src);
    SETMEMSI (current_cpu, pc, ADDSI (FLD (f_optdisp), * FLD (i_abase)), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_srcdst)) + (1))]);
    SETMEMSI (current_cpu, pc, ADDSI (ADDSI (FLD (f_optdisp), * FLD (i_abase)), 4), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_srcdst)) + (2))]);
    SETMEMSI (current_cpu, pc, ADDSI (ADDSI (FLD (f_optdisp), * FLD (i_abase)), 8), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* stt-index-disp: stt $st_src, $optdisp[$index*S$scale */

SEM_PC
SEM_FN_NAME (i960base,stt_index_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stt_index_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

{
  SI tmp_sregno;
  tmp_sregno = FLD (f_srcdst);
  {
    SI opval = * FLD (i_st_src);
    SETMEMSI (current_cpu, pc, ADDSI (FLD (f_optdisp), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale)))), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_srcdst)) + (1))]);
    SETMEMSI (current_cpu, pc, ADDSI (ADDSI (FLD (f_optdisp), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale)))), 4), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_srcdst)) + (2))]);
    SETMEMSI (current_cpu, pc, ADDSI (ADDSI (FLD (f_optdisp), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale)))), 8), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* stt-indirect-index-disp: stt $st_src, $optdisp($abase)[$index*S$scale] */

SEM_PC
SEM_FN_NAME (i960base,stt_indirect_index_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stt_indirect_index_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

{
  SI tmp_sregno;
  tmp_sregno = FLD (f_srcdst);
  {
    SI opval = * FLD (i_st_src);
    SETMEMSI (current_cpu, pc, ADDSI (FLD (f_optdisp), ADDSI (* FLD (i_abase), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale))))), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_srcdst)) + (1))]);
    SETMEMSI (current_cpu, pc, ADDSI (ADDSI (FLD (f_optdisp), ADDSI (* FLD (i_abase), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale))))), 4), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_srcdst)) + (2))]);
    SETMEMSI (current_cpu, pc, ADDSI (ADDSI (FLD (f_optdisp), ADDSI (* FLD (i_abase), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale))))), 8), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* stq-offset: stq $st_src, $offset */

SEM_PC
SEM_FN_NAME (i960base,stq_offset) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stq_offset.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_sregno;
  tmp_sregno = FLD (f_srcdst);
  {
    SI opval = * FLD (i_st_src);
    SETMEMSI (current_cpu, pc, FLD (f_offset), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_srcdst)) + (1))]);
    SETMEMSI (current_cpu, pc, ADDSI (FLD (f_offset), 4), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_srcdst)) + (2))]);
    SETMEMSI (current_cpu, pc, ADDSI (FLD (f_offset), 8), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_srcdst)) + (3))]);
    SETMEMSI (current_cpu, pc, ADDSI (FLD (f_offset), 12), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* stq-indirect-offset: stq $st_src, $offset($abase) */

SEM_PC
SEM_FN_NAME (i960base,stq_indirect_offset) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stq_indirect_offset.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_sregno;
  tmp_sregno = FLD (f_srcdst);
  {
    SI opval = * FLD (i_st_src);
    SETMEMSI (current_cpu, pc, ADDSI (FLD (f_offset), * FLD (i_abase)), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_srcdst)) + (1))]);
    SETMEMSI (current_cpu, pc, ADDSI (ADDSI (FLD (f_offset), * FLD (i_abase)), 4), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_srcdst)) + (2))]);
    SETMEMSI (current_cpu, pc, ADDSI (ADDSI (FLD (f_offset), * FLD (i_abase)), 8), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_srcdst)) + (3))]);
    SETMEMSI (current_cpu, pc, ADDSI (ADDSI (FLD (f_offset), * FLD (i_abase)), 12), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* stq-indirect: stq $st_src, ($abase) */

SEM_PC
SEM_FN_NAME (i960base,stq_indirect) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stq_indirect.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_sregno;
  tmp_sregno = FLD (f_srcdst);
  {
    SI opval = * FLD (i_st_src);
    SETMEMSI (current_cpu, pc, * FLD (i_abase), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_srcdst)) + (1))]);
    SETMEMSI (current_cpu, pc, ADDSI (* FLD (i_abase), 4), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_srcdst)) + (2))]);
    SETMEMSI (current_cpu, pc, ADDSI (* FLD (i_abase), 8), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_srcdst)) + (3))]);
    SETMEMSI (current_cpu, pc, ADDSI (* FLD (i_abase), 12), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* stq-indirect-index: stq $st_src, ($abase)[$index*S$scale] */

SEM_PC
SEM_FN_NAME (i960base,stq_indirect_index) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stq_indirect_index.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_sregno;
  tmp_sregno = FLD (f_srcdst);
  {
    SI opval = * FLD (i_st_src);
    SETMEMSI (current_cpu, pc, ADDSI (* FLD (i_abase), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale)))), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_srcdst)) + (1))]);
    SETMEMSI (current_cpu, pc, ADDSI (ADDSI (* FLD (i_abase), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale)))), 4), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_srcdst)) + (2))]);
    SETMEMSI (current_cpu, pc, ADDSI (ADDSI (* FLD (i_abase), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale)))), 8), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_srcdst)) + (3))]);
    SETMEMSI (current_cpu, pc, ADDSI (ADDSI (* FLD (i_abase), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale)))), 12), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* stq-disp: stq $st_src, $optdisp */

SEM_PC
SEM_FN_NAME (i960base,stq_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stq_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

{
  SI tmp_sregno;
  tmp_sregno = FLD (f_srcdst);
  {
    SI opval = * FLD (i_st_src);
    SETMEMSI (current_cpu, pc, FLD (f_optdisp), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_srcdst)) + (1))]);
    SETMEMSI (current_cpu, pc, ADDSI (FLD (f_optdisp), 4), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_srcdst)) + (2))]);
    SETMEMSI (current_cpu, pc, ADDSI (FLD (f_optdisp), 8), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_srcdst)) + (3))]);
    SETMEMSI (current_cpu, pc, ADDSI (FLD (f_optdisp), 12), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* stq-indirect-disp: stq $st_src, $optdisp($abase) */

SEM_PC
SEM_FN_NAME (i960base,stq_indirect_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stq_indirect_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

{
  SI tmp_sregno;
  tmp_sregno = FLD (f_srcdst);
  {
    SI opval = * FLD (i_st_src);
    SETMEMSI (current_cpu, pc, ADDSI (FLD (f_optdisp), * FLD (i_abase)), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_srcdst)) + (1))]);
    SETMEMSI (current_cpu, pc, ADDSI (ADDSI (FLD (f_optdisp), * FLD (i_abase)), 4), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_srcdst)) + (2))]);
    SETMEMSI (current_cpu, pc, ADDSI (ADDSI (FLD (f_optdisp), * FLD (i_abase)), 8), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_srcdst)) + (3))]);
    SETMEMSI (current_cpu, pc, ADDSI (ADDSI (FLD (f_optdisp), * FLD (i_abase)), 12), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* stq-index-disp: stq $st_src, $optdisp[$index*S$scale */

SEM_PC
SEM_FN_NAME (i960base,stq_index_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stq_index_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

{
  SI tmp_sregno;
  tmp_sregno = FLD (f_srcdst);
  {
    SI opval = * FLD (i_st_src);
    SETMEMSI (current_cpu, pc, ADDSI (FLD (f_optdisp), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale)))), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_srcdst)) + (1))]);
    SETMEMSI (current_cpu, pc, ADDSI (ADDSI (FLD (f_optdisp), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale)))), 4), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_srcdst)) + (2))]);
    SETMEMSI (current_cpu, pc, ADDSI (ADDSI (FLD (f_optdisp), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale)))), 8), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_srcdst)) + (3))]);
    SETMEMSI (current_cpu, pc, ADDSI (ADDSI (FLD (f_optdisp), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale)))), 12), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* stq-indirect-index-disp: stq $st_src, $optdisp($abase)[$index*S$scale] */

SEM_PC
SEM_FN_NAME (i960base,stq_indirect_index_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stq_indirect_index_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

{
  SI tmp_sregno;
  tmp_sregno = FLD (f_srcdst);
  {
    SI opval = * FLD (i_st_src);
    SETMEMSI (current_cpu, pc, ADDSI (FLD (f_optdisp), ADDSI (* FLD (i_abase), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale))))), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_srcdst)) + (1))]);
    SETMEMSI (current_cpu, pc, ADDSI (ADDSI (FLD (f_optdisp), ADDSI (* FLD (i_abase), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale))))), 4), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_srcdst)) + (2))]);
    SETMEMSI (current_cpu, pc, ADDSI (ADDSI (FLD (f_optdisp), ADDSI (* FLD (i_abase), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale))))), 8), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((FLD (f_srcdst)) + (3))]);
    SETMEMSI (current_cpu, pc, ADDSI (ADDSI (FLD (f_optdisp), ADDSI (* FLD (i_abase), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale))))), 12), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
}

  return vpc;
#undef FLD
}

/* cmpobe-reg: cmpobe $br_src1, $br_src2, $br_disp */

SEM_PC
SEM_FN_NAME (i960base,cmpobe_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_cmpobe_reg.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (EQSI (* FLD (i_br_src1), * FLD (i_br_src2))) {
  {
    USI opval = FLD (i_br_disp);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* cmpobe-lit: cmpobe $br_lit1, $br_src2, $br_disp */

SEM_PC
SEM_FN_NAME (i960base,cmpobe_lit) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_cmpobe_lit.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (EQSI (FLD (f_br_src1), * FLD (i_br_src2))) {
  {
    USI opval = FLD (i_br_disp);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* cmpobne-reg: cmpobne $br_src1, $br_src2, $br_disp */

SEM_PC
SEM_FN_NAME (i960base,cmpobne_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_cmpobe_reg.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (NESI (* FLD (i_br_src1), * FLD (i_br_src2))) {
  {
    USI opval = FLD (i_br_disp);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* cmpobne-lit: cmpobne $br_lit1, $br_src2, $br_disp */

SEM_PC
SEM_FN_NAME (i960base,cmpobne_lit) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_cmpobe_lit.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (NESI (FLD (f_br_src1), * FLD (i_br_src2))) {
  {
    USI opval = FLD (i_br_disp);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* cmpobl-reg: cmpobl $br_src1, $br_src2, $br_disp */

SEM_PC
SEM_FN_NAME (i960base,cmpobl_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_cmpobl_reg.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (LTUSI (* FLD (i_br_src1), * FLD (i_br_src2))) {
  {
    USI opval = FLD (i_br_disp);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* cmpobl-lit: cmpobl $br_lit1, $br_src2, $br_disp */

SEM_PC
SEM_FN_NAME (i960base,cmpobl_lit) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_cmpobl_lit.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (LTUSI (FLD (f_br_src1), * FLD (i_br_src2))) {
  {
    USI opval = FLD (i_br_disp);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* cmpoble-reg: cmpoble $br_src1, $br_src2, $br_disp */

SEM_PC
SEM_FN_NAME (i960base,cmpoble_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_cmpobl_reg.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (LEUSI (* FLD (i_br_src1), * FLD (i_br_src2))) {
  {
    USI opval = FLD (i_br_disp);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* cmpoble-lit: cmpoble $br_lit1, $br_src2, $br_disp */

SEM_PC
SEM_FN_NAME (i960base,cmpoble_lit) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_cmpobl_lit.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (LEUSI (FLD (f_br_src1), * FLD (i_br_src2))) {
  {
    USI opval = FLD (i_br_disp);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* cmpobg-reg: cmpobg $br_src1, $br_src2, $br_disp */

SEM_PC
SEM_FN_NAME (i960base,cmpobg_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_cmpobl_reg.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (GTUSI (* FLD (i_br_src1), * FLD (i_br_src2))) {
  {
    USI opval = FLD (i_br_disp);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* cmpobg-lit: cmpobg $br_lit1, $br_src2, $br_disp */

SEM_PC
SEM_FN_NAME (i960base,cmpobg_lit) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_cmpobl_lit.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (GTUSI (FLD (f_br_src1), * FLD (i_br_src2))) {
  {
    USI opval = FLD (i_br_disp);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* cmpobge-reg: cmpobge $br_src1, $br_src2, $br_disp */

SEM_PC
SEM_FN_NAME (i960base,cmpobge_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_cmpobl_reg.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (GEUSI (* FLD (i_br_src1), * FLD (i_br_src2))) {
  {
    USI opval = FLD (i_br_disp);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* cmpobge-lit: cmpobge $br_lit1, $br_src2, $br_disp */

SEM_PC
SEM_FN_NAME (i960base,cmpobge_lit) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_cmpobl_lit.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (GEUSI (FLD (f_br_src1), * FLD (i_br_src2))) {
  {
    USI opval = FLD (i_br_disp);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* cmpibe-reg: cmpibe $br_src1, $br_src2, $br_disp */

SEM_PC
SEM_FN_NAME (i960base,cmpibe_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_cmpobe_reg.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (EQSI (* FLD (i_br_src1), * FLD (i_br_src2))) {
  {
    USI opval = FLD (i_br_disp);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* cmpibe-lit: cmpibe $br_lit1, $br_src2, $br_disp */

SEM_PC
SEM_FN_NAME (i960base,cmpibe_lit) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_cmpobe_lit.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (EQSI (FLD (f_br_src1), * FLD (i_br_src2))) {
  {
    USI opval = FLD (i_br_disp);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* cmpibne-reg: cmpibne $br_src1, $br_src2, $br_disp */

SEM_PC
SEM_FN_NAME (i960base,cmpibne_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_cmpobe_reg.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (NESI (* FLD (i_br_src1), * FLD (i_br_src2))) {
  {
    USI opval = FLD (i_br_disp);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* cmpibne-lit: cmpibne $br_lit1, $br_src2, $br_disp */

SEM_PC
SEM_FN_NAME (i960base,cmpibne_lit) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_cmpobe_lit.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (NESI (FLD (f_br_src1), * FLD (i_br_src2))) {
  {
    USI opval = FLD (i_br_disp);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* cmpibl-reg: cmpibl $br_src1, $br_src2, $br_disp */

SEM_PC
SEM_FN_NAME (i960base,cmpibl_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_cmpobe_reg.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (LTSI (* FLD (i_br_src1), * FLD (i_br_src2))) {
  {
    USI opval = FLD (i_br_disp);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* cmpibl-lit: cmpibl $br_lit1, $br_src2, $br_disp */

SEM_PC
SEM_FN_NAME (i960base,cmpibl_lit) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_cmpobe_lit.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (LTSI (FLD (f_br_src1), * FLD (i_br_src2))) {
  {
    USI opval = FLD (i_br_disp);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* cmpible-reg: cmpible $br_src1, $br_src2, $br_disp */

SEM_PC
SEM_FN_NAME (i960base,cmpible_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_cmpobe_reg.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (LESI (* FLD (i_br_src1), * FLD (i_br_src2))) {
  {
    USI opval = FLD (i_br_disp);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* cmpible-lit: cmpible $br_lit1, $br_src2, $br_disp */

SEM_PC
SEM_FN_NAME (i960base,cmpible_lit) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_cmpobe_lit.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (LESI (FLD (f_br_src1), * FLD (i_br_src2))) {
  {
    USI opval = FLD (i_br_disp);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* cmpibg-reg: cmpibg $br_src1, $br_src2, $br_disp */

SEM_PC
SEM_FN_NAME (i960base,cmpibg_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_cmpobe_reg.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (GTSI (* FLD (i_br_src1), * FLD (i_br_src2))) {
  {
    USI opval = FLD (i_br_disp);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* cmpibg-lit: cmpibg $br_lit1, $br_src2, $br_disp */

SEM_PC
SEM_FN_NAME (i960base,cmpibg_lit) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_cmpobe_lit.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (GTSI (FLD (f_br_src1), * FLD (i_br_src2))) {
  {
    USI opval = FLD (i_br_disp);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* cmpibge-reg: cmpibge $br_src1, $br_src2, $br_disp */

SEM_PC
SEM_FN_NAME (i960base,cmpibge_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_cmpobe_reg.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (GESI (* FLD (i_br_src1), * FLD (i_br_src2))) {
  {
    USI opval = FLD (i_br_disp);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* cmpibge-lit: cmpibge $br_lit1, $br_src2, $br_disp */

SEM_PC
SEM_FN_NAME (i960base,cmpibge_lit) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_cmpobe_lit.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (GESI (FLD (f_br_src1), * FLD (i_br_src2))) {
  {
    USI opval = FLD (i_br_disp);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* bbc-reg: bbc $br_src1, $br_src2, $br_disp */

SEM_PC
SEM_FN_NAME (i960base,bbc_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bbc_reg.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (EQSI (ANDSI (SLLSI (1, * FLD (i_br_src1)), * FLD (i_br_src2)), 0)) {
  {
    USI opval = FLD (i_br_disp);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* bbc-lit: bbc $br_lit1, $br_src2, $br_disp */

SEM_PC
SEM_FN_NAME (i960base,bbc_lit) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bbc_lit.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (EQSI (ANDSI (SLLSI (1, FLD (f_br_src1)), * FLD (i_br_src2)), 0)) {
  {
    USI opval = FLD (i_br_disp);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* bbs-reg: bbs $br_src1, $br_src2, $br_disp */

SEM_PC
SEM_FN_NAME (i960base,bbs_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bbc_reg.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (NESI (ANDSI (SLLSI (1, * FLD (i_br_src1)), * FLD (i_br_src2)), 0)) {
  {
    USI opval = FLD (i_br_disp);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* bbs-lit: bbs $br_lit1, $br_src2, $br_disp */

SEM_PC
SEM_FN_NAME (i960base,bbs_lit) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bbc_lit.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (NESI (ANDSI (SLLSI (1, FLD (f_br_src1)), * FLD (i_br_src2)), 0)) {
  {
    USI opval = FLD (i_br_disp);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* cmpi: cmpi $src1, $src2 */

SEM_PC
SEM_FN_NAME (i960base,cmpi) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_cmpi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = (LTSI (* FLD (i_src1), * FLD (i_src2))) ? (4) : (EQSI (* FLD (i_src1), * FLD (i_src2))) ? (2) : (1);
    CPU (h_cc) = opval;
    TRACE_RESULT (current_cpu, abuf, "cc", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* cmpi1: cmpi $lit1, $src2 */

SEM_PC
SEM_FN_NAME (i960base,cmpi1) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_cmpi1.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = (LTSI (FLD (f_src1), * FLD (i_src2))) ? (4) : (EQSI (FLD (f_src1), * FLD (i_src2))) ? (2) : (1);
    CPU (h_cc) = opval;
    TRACE_RESULT (current_cpu, abuf, "cc", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* cmpi2: cmpi $src1, $lit2 */

SEM_PC
SEM_FN_NAME (i960base,cmpi2) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_cmpi2.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = (LTSI (* FLD (i_src1), FLD (f_src2))) ? (4) : (EQSI (* FLD (i_src1), FLD (f_src2))) ? (2) : (1);
    CPU (h_cc) = opval;
    TRACE_RESULT (current_cpu, abuf, "cc", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* cmpi3: cmpi $lit1, $lit2 */

SEM_PC
SEM_FN_NAME (i960base,cmpi3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_cmpi3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = (LTSI (FLD (f_src1), FLD (f_src2))) ? (4) : (EQSI (FLD (f_src1), FLD (f_src2))) ? (2) : (1);
    CPU (h_cc) = opval;
    TRACE_RESULT (current_cpu, abuf, "cc", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* cmpo: cmpo $src1, $src2 */

SEM_PC
SEM_FN_NAME (i960base,cmpo) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_cmpo.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = (LTUSI (* FLD (i_src1), * FLD (i_src2))) ? (4) : (EQSI (* FLD (i_src1), * FLD (i_src2))) ? (2) : (1);
    CPU (h_cc) = opval;
    TRACE_RESULT (current_cpu, abuf, "cc", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* cmpo1: cmpo $lit1, $src2 */

SEM_PC
SEM_FN_NAME (i960base,cmpo1) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_cmpo1.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = (LTUSI (FLD (f_src1), * FLD (i_src2))) ? (4) : (EQSI (FLD (f_src1), * FLD (i_src2))) ? (2) : (1);
    CPU (h_cc) = opval;
    TRACE_RESULT (current_cpu, abuf, "cc", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* cmpo2: cmpo $src1, $lit2 */

SEM_PC
SEM_FN_NAME (i960base,cmpo2) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_cmpo2.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = (LTUSI (* FLD (i_src1), FLD (f_src2))) ? (4) : (EQSI (* FLD (i_src1), FLD (f_src2))) ? (2) : (1);
    CPU (h_cc) = opval;
    TRACE_RESULT (current_cpu, abuf, "cc", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* cmpo3: cmpo $lit1, $lit2 */

SEM_PC
SEM_FN_NAME (i960base,cmpo3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_cmpo3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = (LTUSI (FLD (f_src1), FLD (f_src2))) ? (4) : (EQSI (FLD (f_src1), FLD (f_src2))) ? (2) : (1);
    CPU (h_cc) = opval;
    TRACE_RESULT (current_cpu, abuf, "cc", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* testno-reg: testno $br_src1 */

SEM_PC
SEM_FN_NAME (i960base,testno_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_testno_reg.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = EQSI (CPU (h_cc), 0);
    * FLD (i_br_src1) = opval;
    TRACE_RESULT (current_cpu, abuf, "br_src1", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* testg-reg: testg $br_src1 */

SEM_PC
SEM_FN_NAME (i960base,testg_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_testno_reg.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = NESI (ANDSI (CPU (h_cc), 1), 0);
    * FLD (i_br_src1) = opval;
    TRACE_RESULT (current_cpu, abuf, "br_src1", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* teste-reg: teste $br_src1 */

SEM_PC
SEM_FN_NAME (i960base,teste_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_testno_reg.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = NESI (ANDSI (CPU (h_cc), 2), 0);
    * FLD (i_br_src1) = opval;
    TRACE_RESULT (current_cpu, abuf, "br_src1", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* testge-reg: testge $br_src1 */

SEM_PC
SEM_FN_NAME (i960base,testge_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_testno_reg.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = NESI (ANDSI (CPU (h_cc), 3), 0);
    * FLD (i_br_src1) = opval;
    TRACE_RESULT (current_cpu, abuf, "br_src1", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* testl-reg: testl $br_src1 */

SEM_PC
SEM_FN_NAME (i960base,testl_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_testno_reg.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = NESI (ANDSI (CPU (h_cc), 4), 0);
    * FLD (i_br_src1) = opval;
    TRACE_RESULT (current_cpu, abuf, "br_src1", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* testne-reg: testne $br_src1 */

SEM_PC
SEM_FN_NAME (i960base,testne_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_testno_reg.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = NESI (ANDSI (CPU (h_cc), 5), 0);
    * FLD (i_br_src1) = opval;
    TRACE_RESULT (current_cpu, abuf, "br_src1", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* testle-reg: testle $br_src1 */

SEM_PC
SEM_FN_NAME (i960base,testle_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_testno_reg.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = NESI (ANDSI (CPU (h_cc), 6), 0);
    * FLD (i_br_src1) = opval;
    TRACE_RESULT (current_cpu, abuf, "br_src1", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* testo-reg: testo $br_src1 */

SEM_PC
SEM_FN_NAME (i960base,testo_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_testno_reg.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = NESI (ANDSI (CPU (h_cc), 7), 0);
    * FLD (i_br_src1) = opval;
    TRACE_RESULT (current_cpu, abuf, "br_src1", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* bno: bno $ctrl_disp */

SEM_PC
SEM_FN_NAME (i960base,bno) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bno.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (EQSI (CPU (h_cc), 0)) {
  {
    USI opval = FLD (i_ctrl_disp);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* bg: bg $ctrl_disp */

SEM_PC
SEM_FN_NAME (i960base,bg) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bno.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (NESI (ANDSI (CPU (h_cc), 1), 0)) {
  {
    USI opval = FLD (i_ctrl_disp);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* be: be $ctrl_disp */

SEM_PC
SEM_FN_NAME (i960base,be) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bno.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (NESI (ANDSI (CPU (h_cc), 2), 0)) {
  {
    USI opval = FLD (i_ctrl_disp);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* bge: bge $ctrl_disp */

SEM_PC
SEM_FN_NAME (i960base,bge) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bno.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (NESI (ANDSI (CPU (h_cc), 3), 0)) {
  {
    USI opval = FLD (i_ctrl_disp);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* bl: bl $ctrl_disp */

SEM_PC
SEM_FN_NAME (i960base,bl) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bno.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (NESI (ANDSI (CPU (h_cc), 4), 0)) {
  {
    USI opval = FLD (i_ctrl_disp);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* bne: bne $ctrl_disp */

SEM_PC
SEM_FN_NAME (i960base,bne) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bno.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (NESI (ANDSI (CPU (h_cc), 5), 0)) {
  {
    USI opval = FLD (i_ctrl_disp);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* ble: ble $ctrl_disp */

SEM_PC
SEM_FN_NAME (i960base,ble) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bno.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (NESI (ANDSI (CPU (h_cc), 6), 0)) {
  {
    USI opval = FLD (i_ctrl_disp);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* bo: bo $ctrl_disp */

SEM_PC
SEM_FN_NAME (i960base,bo) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bno.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

if (NESI (ANDSI (CPU (h_cc), 7), 0)) {
  {
    USI opval = FLD (i_ctrl_disp);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* b: b $ctrl_disp */

SEM_PC
SEM_FN_NAME (i960base,b) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_b.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    USI opval = FLD (i_ctrl_disp);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }

  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* bx-indirect-offset: bx $offset($abase) */

SEM_PC
SEM_FN_NAME (i960base,bx_indirect_offset) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bx_indirect_offset.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    USI opval = ADDSI (FLD (f_offset), * FLD (i_abase));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }

  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* bx-indirect: bx ($abase) */

SEM_PC
SEM_FN_NAME (i960base,bx_indirect) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bx_indirect.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    USI opval = * FLD (i_abase);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }

  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* bx-indirect-index: bx ($abase)[$index*S$scale] */

SEM_PC
SEM_FN_NAME (i960base,bx_indirect_index) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bx_indirect_index.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    USI opval = ADDSI (* FLD (i_abase), MULSI (* FLD (i_index), SLLSI (1, FLD (f_scale))));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }

  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* bx-disp: bx $optdisp */

SEM_PC
SEM_FN_NAME (i960base,bx_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bx_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

  {
    USI opval = FLD (f_optdisp);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }

  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* bx-indirect-disp: bx $optdisp($abase) */

SEM_PC
SEM_FN_NAME (i960base,bx_indirect_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bx_indirect_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

  {
    USI opval = ADDSI (FLD (f_optdisp), * FLD (i_abase));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }

  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* callx-disp: callx $optdisp */

SEM_PC
SEM_FN_NAME (i960base,callx_disp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_callx_disp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 8);

{
  SI tmp_temp;
  tmp_temp = ANDSI (ADDSI (CPU (h_gr[((UINT) 1)]), 63), INVSI (63));
  {
    SI opval = ADDSI (pc, 8);
    CPU (h_gr[((UINT) 2)]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-2", 'x', opval);
  }
SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 0), CPU (h_gr[((UINT) 0)]));
SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 4), CPU (h_gr[((UINT) 1)]));
SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 8), CPU (h_gr[((UINT) 2)]));
SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 12), CPU (h_gr[((UINT) 3)]));
SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 16), CPU (h_gr[((UINT) 4)]));
SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 20), CPU (h_gr[((UINT) 5)]));
SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 24), CPU (h_gr[((UINT) 6)]));
SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 28), CPU (h_gr[((UINT) 7)]));
SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 32), CPU (h_gr[((UINT) 8)]));
SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 36), CPU (h_gr[((UINT) 9)]));
SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 40), CPU (h_gr[((UINT) 10)]));
SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 44), CPU (h_gr[((UINT) 11)]));
SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 48), CPU (h_gr[((UINT) 12)]));
SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 52), CPU (h_gr[((UINT) 13)]));
SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 56), CPU (h_gr[((UINT) 14)]));
SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 60), CPU (h_gr[((UINT) 15)]));
  {
    USI opval = FLD (f_optdisp);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
CPU (h_gr[((UINT) 0)]) = 0xdeadbeef;
CPU (h_gr[((UINT) 1)]) = 0xdeadbeef;
CPU (h_gr[((UINT) 2)]) = 0xdeadbeef;
CPU (h_gr[((UINT) 3)]) = 0xdeadbeef;
CPU (h_gr[((UINT) 4)]) = 0xdeadbeef;
CPU (h_gr[((UINT) 5)]) = 0xdeadbeef;
CPU (h_gr[((UINT) 6)]) = 0xdeadbeef;
CPU (h_gr[((UINT) 7)]) = 0xdeadbeef;
CPU (h_gr[((UINT) 8)]) = 0xdeadbeef;
CPU (h_gr[((UINT) 9)]) = 0xdeadbeef;
CPU (h_gr[((UINT) 10)]) = 0xdeadbeef;
CPU (h_gr[((UINT) 11)]) = 0xdeadbeef;
CPU (h_gr[((UINT) 12)]) = 0xdeadbeef;
CPU (h_gr[((UINT) 13)]) = 0xdeadbeef;
CPU (h_gr[((UINT) 14)]) = 0xdeadbeef;
CPU (h_gr[((UINT) 15)]) = 0xdeadbeef;
  {
    SI opval = CPU (h_gr[((UINT) 31)]);
    CPU (h_gr[((UINT) 0)]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-0", 'x', opval);
  }
  {
    SI opval = tmp_temp;
    CPU (h_gr[((UINT) 31)]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-31", 'x', opval);
  }
  {
    SI opval = ADDSI (tmp_temp, 64);
    CPU (h_gr[((UINT) 1)]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-1", 'x', opval);
  }
}

  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* callx-indirect: callx ($abase) */

SEM_PC
SEM_FN_NAME (i960base,callx_indirect) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_callx_indirect.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_temp;
  tmp_temp = ANDSI (ADDSI (CPU (h_gr[((UINT) 1)]), 63), INVSI (63));
  {
    SI opval = ADDSI (pc, 4);
    CPU (h_gr[((UINT) 2)]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-2", 'x', opval);
  }
SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 0), CPU (h_gr[((UINT) 0)]));
SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 4), CPU (h_gr[((UINT) 1)]));
SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 8), CPU (h_gr[((UINT) 2)]));
SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 12), CPU (h_gr[((UINT) 3)]));
SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 16), CPU (h_gr[((UINT) 4)]));
SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 20), CPU (h_gr[((UINT) 5)]));
SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 24), CPU (h_gr[((UINT) 6)]));
SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 28), CPU (h_gr[((UINT) 7)]));
SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 32), CPU (h_gr[((UINT) 8)]));
SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 36), CPU (h_gr[((UINT) 9)]));
SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 40), CPU (h_gr[((UINT) 10)]));
SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 44), CPU (h_gr[((UINT) 11)]));
SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 48), CPU (h_gr[((UINT) 12)]));
SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 52), CPU (h_gr[((UINT) 13)]));
SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 56), CPU (h_gr[((UINT) 14)]));
SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 60), CPU (h_gr[((UINT) 15)]));
  {
    USI opval = * FLD (i_abase);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
CPU (h_gr[((UINT) 0)]) = 0xdeadbeef;
CPU (h_gr[((UINT) 1)]) = 0xdeadbeef;
CPU (h_gr[((UINT) 2)]) = 0xdeadbeef;
CPU (h_gr[((UINT) 3)]) = 0xdeadbeef;
CPU (h_gr[((UINT) 4)]) = 0xdeadbeef;
CPU (h_gr[((UINT) 5)]) = 0xdeadbeef;
CPU (h_gr[((UINT) 6)]) = 0xdeadbeef;
CPU (h_gr[((UINT) 7)]) = 0xdeadbeef;
CPU (h_gr[((UINT) 8)]) = 0xdeadbeef;
CPU (h_gr[((UINT) 9)]) = 0xdeadbeef;
CPU (h_gr[((UINT) 10)]) = 0xdeadbeef;
CPU (h_gr[((UINT) 11)]) = 0xdeadbeef;
CPU (h_gr[((UINT) 12)]) = 0xdeadbeef;
CPU (h_gr[((UINT) 13)]) = 0xdeadbeef;
CPU (h_gr[((UINT) 14)]) = 0xdeadbeef;
CPU (h_gr[((UINT) 15)]) = 0xdeadbeef;
  {
    SI opval = CPU (h_gr[((UINT) 31)]);
    CPU (h_gr[((UINT) 0)]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-0", 'x', opval);
  }
  {
    SI opval = tmp_temp;
    CPU (h_gr[((UINT) 31)]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-31", 'x', opval);
  }
  {
    SI opval = ADDSI (tmp_temp, 64);
    CPU (h_gr[((UINT) 1)]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-1", 'x', opval);
  }
}

  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* callx-indirect-offset: callx $offset($abase) */

SEM_PC
SEM_FN_NAME (i960base,callx_indirect_offset) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_callx_indirect_offset.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  SI tmp_temp;
  tmp_temp = ANDSI (ADDSI (CPU (h_gr[((UINT) 1)]), 63), INVSI (63));
  {
    SI opval = ADDSI (pc, 4);
    CPU (h_gr[((UINT) 2)]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-2", 'x', opval);
  }
SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 0), CPU (h_gr[((UINT) 0)]));
SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 4), CPU (h_gr[((UINT) 1)]));
SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 8), CPU (h_gr[((UINT) 2)]));
SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 12), CPU (h_gr[((UINT) 3)]));
SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 16), CPU (h_gr[((UINT) 4)]));
SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 20), CPU (h_gr[((UINT) 5)]));
SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 24), CPU (h_gr[((UINT) 6)]));
SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 28), CPU (h_gr[((UINT) 7)]));
SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 32), CPU (h_gr[((UINT) 8)]));
SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 36), CPU (h_gr[((UINT) 9)]));
SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 40), CPU (h_gr[((UINT) 10)]));
SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 44), CPU (h_gr[((UINT) 11)]));
SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 48), CPU (h_gr[((UINT) 12)]));
SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 52), CPU (h_gr[((UINT) 13)]));
SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 56), CPU (h_gr[((UINT) 14)]));
SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 60), CPU (h_gr[((UINT) 15)]));
  {
    USI opval = ADDSI (FLD (f_offset), * FLD (i_abase));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
CPU (h_gr[((UINT) 0)]) = 0xdeadbeef;
CPU (h_gr[((UINT) 1)]) = 0xdeadbeef;
CPU (h_gr[((UINT) 2)]) = 0xdeadbeef;
CPU (h_gr[((UINT) 3)]) = 0xdeadbeef;
CPU (h_gr[((UINT) 4)]) = 0xdeadbeef;
CPU (h_gr[((UINT) 5)]) = 0xdeadbeef;
CPU (h_gr[((UINT) 6)]) = 0xdeadbeef;
CPU (h_gr[((UINT) 7)]) = 0xdeadbeef;
CPU (h_gr[((UINT) 8)]) = 0xdeadbeef;
CPU (h_gr[((UINT) 9)]) = 0xdeadbeef;
CPU (h_gr[((UINT) 10)]) = 0xdeadbeef;
CPU (h_gr[((UINT) 11)]) = 0xdeadbeef;
CPU (h_gr[((UINT) 12)]) = 0xdeadbeef;
CPU (h_gr[((UINT) 13)]) = 0xdeadbeef;
CPU (h_gr[((UINT) 14)]) = 0xdeadbeef;
CPU (h_gr[((UINT) 15)]) = 0xdeadbeef;
  {
    SI opval = CPU (h_gr[((UINT) 31)]);
    CPU (h_gr[((UINT) 0)]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-0", 'x', opval);
  }
  {
    SI opval = tmp_temp;
    CPU (h_gr[((UINT) 31)]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-31", 'x', opval);
  }
  {
    SI opval = ADDSI (tmp_temp, 64);
    CPU (h_gr[((UINT) 1)]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-1", 'x', opval);
  }
}

  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* ret: ret */

SEM_PC
SEM_FN_NAME (i960base,ret) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_ret.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

{
  {
    SI opval = CPU (h_gr[((UINT) 0)]);
    CPU (h_gr[((UINT) 31)]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-31", 'x', opval);
  }
CPU (h_gr[((UINT) 0)]) = GETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 0));
CPU (h_gr[((UINT) 1)]) = GETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 4));
CPU (h_gr[((UINT) 2)]) = GETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 8));
CPU (h_gr[((UINT) 3)]) = GETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 12));
CPU (h_gr[((UINT) 4)]) = GETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 16));
CPU (h_gr[((UINT) 5)]) = GETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 20));
CPU (h_gr[((UINT) 6)]) = GETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 24));
CPU (h_gr[((UINT) 7)]) = GETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 28));
CPU (h_gr[((UINT) 8)]) = GETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 32));
CPU (h_gr[((UINT) 9)]) = GETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 36));
CPU (h_gr[((UINT) 10)]) = GETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 40));
CPU (h_gr[((UINT) 11)]) = GETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 44));
CPU (h_gr[((UINT) 12)]) = GETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 48));
CPU (h_gr[((UINT) 13)]) = GETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 52));
CPU (h_gr[((UINT) 14)]) = GETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 56));
CPU (h_gr[((UINT) 15)]) = GETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 31)]), 60));
  {
    USI opval = CPU (h_gr[((UINT) 2)]);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* calls: calls $src1 */

SEM_PC
SEM_FN_NAME (i960base,calls) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_calls.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = i960_trap (current_cpu, pc, * FLD (i_src1));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }

  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* fmark: fmark */

SEM_PC
SEM_FN_NAME (i960base,fmark) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_fmark.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = i960_breakpoint (current_cpu, pc);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }

  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* flushreg: flushreg */

SEM_PC
SEM_FN_NAME (i960base,flushreg) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_flushreg.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

do { } while (0); /*nop*/

  return vpc;
#undef FLD
}

