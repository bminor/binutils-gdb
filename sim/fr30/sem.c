/* Simulator instruction semantics for fr30bf.

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
#include "cgen-mem.h"
#include "cgen-ops.h"

#undef GET_ATTR
#define GET_ATTR(cpu, num, attr) CGEN_ATTR_VALUE (NULL, abuf->idesc->attrs, CGEN_INSN_##attr)

/* x-invalid: --invalid-- */

SEM_PC
SEM_FN_NAME (fr30bf,x_invalid) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
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
SEM_FN_NAME (fr30bf,x_after) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_empty.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 0);

  {
#if WITH_SCACHE_PBB_FR30BF
    fr30bf_pbb_after (current_cpu, sem_arg);
#endif
  }

  return vpc;
#undef FLD
}

/* x-before: --before-- */

SEM_PC
SEM_FN_NAME (fr30bf,x_before) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_empty.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 0);

  {
#if WITH_SCACHE_PBB_FR30BF
    fr30bf_pbb_before (current_cpu, sem_arg);
#endif
  }

  return vpc;
#undef FLD
}

/* x-cti-chain: --cti-chain-- */

SEM_PC
SEM_FN_NAME (fr30bf,x_cti_chain) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_empty.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 0);

  {
#if WITH_SCACHE_PBB_FR30BF
#ifdef DEFINE_SWITCH
    vpc = fr30bf_pbb_cti_chain (current_cpu, sem_arg,
			       pbb_br_npc_ptr, pbb_br_npc);
    BREAK (sem);
#else
    /* FIXME: Allow provision of explicit ifmt spec in insn spec.  */
    vpc = fr30bf_pbb_cti_chain (current_cpu, sem_arg,
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
SEM_FN_NAME (fr30bf,x_chain) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_empty.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 0);

  {
#if WITH_SCACHE_PBB_FR30BF
    vpc = fr30bf_pbb_chain (current_cpu, sem_arg);
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
SEM_FN_NAME (fr30bf,x_begin) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_empty.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 0);

  {
#if WITH_SCACHE_PBB_FR30BF
#ifdef DEFINE_SWITCH
    /* In the switch case FAST_P is a constant, allowing several optimizations
       in any called inline functions.  */
    vpc = fr30bf_pbb_begin (current_cpu, FAST_P);
#else
    vpc = fr30bf_pbb_begin (current_cpu, STATE_RUN_FAST_P (CPU_STATE (current_cpu)));
#endif
#endif
  }

  return vpc;
#undef FLD
}

/* add: add $Rj,$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,add) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  {
    BI opval = ADDOFSI (* FLD (i_Ri), * FLD (i_Rj), 0);
    CPU (h_vbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = ADDCFSI (* FLD (i_Ri), * FLD (i_Rj), 0);
    CPU (h_cbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
  {
    SI opval = ADDSI (* FLD (i_Ri), * FLD (i_Rj));
    * FLD (i_Ri) = opval;
    TRACE_RESULT (current_cpu, abuf, "Ri", 'x', opval);
  }
do {
  {
    BI opval = EQSI (* FLD (i_Ri), 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
  {
    BI opval = LTSI (* FLD (i_Ri), 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
} while (0);
} while (0);

  return vpc;
#undef FLD
}

/* addi: add $u4,$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,addi) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_addi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  {
    BI opval = ADDOFSI (* FLD (i_Ri), FLD (f_u4), 0);
    CPU (h_vbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = ADDCFSI (* FLD (i_Ri), FLD (f_u4), 0);
    CPU (h_cbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
  {
    SI opval = ADDSI (* FLD (i_Ri), FLD (f_u4));
    * FLD (i_Ri) = opval;
    TRACE_RESULT (current_cpu, abuf, "Ri", 'x', opval);
  }
do {
  {
    BI opval = EQSI (* FLD (i_Ri), 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
  {
    BI opval = LTSI (* FLD (i_Ri), 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
} while (0);
} while (0);

  return vpc;
#undef FLD
}

/* add2: add2 $m4,$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,add2) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_add2.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  {
    BI opval = ADDOFSI (* FLD (i_Ri), FLD (f_m4), 0);
    CPU (h_vbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = ADDCFSI (* FLD (i_Ri), FLD (f_m4), 0);
    CPU (h_cbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
  {
    SI opval = ADDSI (* FLD (i_Ri), FLD (f_m4));
    * FLD (i_Ri) = opval;
    TRACE_RESULT (current_cpu, abuf, "Ri", 'x', opval);
  }
do {
  {
    BI opval = EQSI (* FLD (i_Ri), 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
  {
    BI opval = LTSI (* FLD (i_Ri), 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
} while (0);
} while (0);

  return vpc;
#undef FLD
}

/* addc: addc $Rj,$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,addc) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_addc.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  SI tmp_tmp;
  tmp_tmp = ADDCSI (* FLD (i_Ri), * FLD (i_Rj), CPU (h_cbit));
  {
    BI opval = ADDOFSI (* FLD (i_Ri), * FLD (i_Rj), CPU (h_cbit));
    CPU (h_vbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = ADDCFSI (* FLD (i_Ri), * FLD (i_Rj), CPU (h_cbit));
    CPU (h_cbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
  {
    SI opval = tmp_tmp;
    * FLD (i_Ri) = opval;
    TRACE_RESULT (current_cpu, abuf, "Ri", 'x', opval);
  }
do {
  {
    BI opval = EQSI (* FLD (i_Ri), 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
  {
    BI opval = LTSI (* FLD (i_Ri), 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
} while (0);
} while (0);

  return vpc;
#undef FLD
}

/* addn: addn $Rj,$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,addn) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_addn.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = ADDSI (* FLD (i_Ri), * FLD (i_Rj));
    * FLD (i_Ri) = opval;
    TRACE_RESULT (current_cpu, abuf, "Ri", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* addni: addn $u4,$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,addni) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_addni.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = ADDSI (* FLD (i_Ri), FLD (f_u4));
    * FLD (i_Ri) = opval;
    TRACE_RESULT (current_cpu, abuf, "Ri", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* addn2: addn2 $m4,$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,addn2) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_addn2.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = ADDSI (* FLD (i_Ri), FLD (f_m4));
    * FLD (i_Ri) = opval;
    TRACE_RESULT (current_cpu, abuf, "Ri", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* sub: sub $Rj,$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,sub) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_add.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  {
    BI opval = SUBOFSI (* FLD (i_Ri), * FLD (i_Rj), 0);
    CPU (h_vbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = SUBCFSI (* FLD (i_Ri), * FLD (i_Rj), 0);
    CPU (h_cbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
  {
    SI opval = SUBSI (* FLD (i_Ri), * FLD (i_Rj));
    * FLD (i_Ri) = opval;
    TRACE_RESULT (current_cpu, abuf, "Ri", 'x', opval);
  }
do {
  {
    BI opval = EQSI (* FLD (i_Ri), 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
  {
    BI opval = LTSI (* FLD (i_Ri), 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
} while (0);
} while (0);

  return vpc;
#undef FLD
}

/* subc: subc $Rj,$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,subc) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_addc.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  SI tmp_tmp;
  tmp_tmp = SUBCSI (* FLD (i_Ri), * FLD (i_Rj), CPU (h_cbit));
  {
    BI opval = SUBOFSI (* FLD (i_Ri), * FLD (i_Rj), CPU (h_cbit));
    CPU (h_vbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = SUBCFSI (* FLD (i_Ri), * FLD (i_Rj), CPU (h_cbit));
    CPU (h_cbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
  {
    SI opval = tmp_tmp;
    * FLD (i_Ri) = opval;
    TRACE_RESULT (current_cpu, abuf, "Ri", 'x', opval);
  }
do {
  {
    BI opval = EQSI (* FLD (i_Ri), 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
  {
    BI opval = LTSI (* FLD (i_Ri), 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
} while (0);
} while (0);

  return vpc;
#undef FLD
}

/* subn: subn $Rj,$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,subn) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_addn.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = SUBSI (* FLD (i_Ri), * FLD (i_Rj));
    * FLD (i_Ri) = opval;
    TRACE_RESULT (current_cpu, abuf, "Ri", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* cmp: cmp $Rj,$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,cmp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_cmp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  SI tmp_tmp1;
  {
    BI opval = SUBOFSI (* FLD (i_Ri), * FLD (i_Rj), 0);
    CPU (h_vbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = SUBCFSI (* FLD (i_Ri), * FLD (i_Rj), 0);
    CPU (h_cbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
  tmp_tmp1 = SUBSI (* FLD (i_Ri), * FLD (i_Rj));
do {
  {
    BI opval = EQSI (tmp_tmp1, 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
  {
    BI opval = LTSI (tmp_tmp1, 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
} while (0);
} while (0);

  return vpc;
#undef FLD
}

/* cmpi: cmp $u4,$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,cmpi) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_cmpi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  SI tmp_tmp1;
  {
    BI opval = SUBOFSI (* FLD (i_Ri), FLD (f_u4), 0);
    CPU (h_vbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = SUBCFSI (* FLD (i_Ri), FLD (f_u4), 0);
    CPU (h_cbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
  tmp_tmp1 = SUBSI (* FLD (i_Ri), FLD (f_u4));
do {
  {
    BI opval = EQSI (tmp_tmp1, 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
  {
    BI opval = LTSI (tmp_tmp1, 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
} while (0);
} while (0);

  return vpc;
#undef FLD
}

/* cmp2: cmp2 $m4,$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,cmp2) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_cmp2.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  SI tmp_tmp1;
  {
    BI opval = SUBOFSI (* FLD (i_Ri), FLD (f_m4), 0);
    CPU (h_vbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
  {
    BI opval = SUBCFSI (* FLD (i_Ri), FLD (f_m4), 0);
    CPU (h_cbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
  tmp_tmp1 = SUBSI (* FLD (i_Ri), FLD (f_m4));
do {
  {
    BI opval = EQSI (tmp_tmp1, 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
  {
    BI opval = LTSI (tmp_tmp1, 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
} while (0);
} while (0);

  return vpc;
#undef FLD
}

/* and: and $Rj,$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,and) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_and.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  {
    SI opval = ANDSI (* FLD (i_Ri), * FLD (i_Rj));
    * FLD (i_Ri) = opval;
    TRACE_RESULT (current_cpu, abuf, "Ri", 'x', opval);
  }
do {
  {
    BI opval = EQSI (* FLD (i_Ri), 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
  {
    BI opval = LTSI (* FLD (i_Ri), 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
} while (0);
} while (0);

  return vpc;
#undef FLD
}

/* or: or $Rj,$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,or) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_and.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  {
    SI opval = ORSI (* FLD (i_Ri), * FLD (i_Rj));
    * FLD (i_Ri) = opval;
    TRACE_RESULT (current_cpu, abuf, "Ri", 'x', opval);
  }
do {
  {
    BI opval = EQSI (* FLD (i_Ri), 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
  {
    BI opval = LTSI (* FLD (i_Ri), 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
} while (0);
} while (0);

  return vpc;
#undef FLD
}

/* eor: eor $Rj,$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,eor) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_and.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  {
    SI opval = XORSI (* FLD (i_Ri), * FLD (i_Rj));
    * FLD (i_Ri) = opval;
    TRACE_RESULT (current_cpu, abuf, "Ri", 'x', opval);
  }
do {
  {
    BI opval = EQSI (* FLD (i_Ri), 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
  {
    BI opval = LTSI (* FLD (i_Ri), 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
} while (0);
} while (0);

  return vpc;
#undef FLD
}

/* andm: and $Rj,@$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,andm) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_andm.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  SI tmp_tmp;
  tmp_tmp = ANDSI (GETMEMSI (current_cpu, pc, * FLD (i_Ri)), * FLD (i_Rj));
do {
  {
    BI opval = EQSI (tmp_tmp, 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
  {
    BI opval = LTSI (tmp_tmp, 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
} while (0);
  {
    SI opval = tmp_tmp;
    SETMEMSI (current_cpu, pc, * FLD (i_Ri), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
} while (0);

  return vpc;
#undef FLD
}

/* andh: andh $Rj,@$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,andh) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_andh.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  HI tmp_tmp;
  tmp_tmp = ANDHI (GETMEMHI (current_cpu, pc, * FLD (i_Ri)), * FLD (i_Rj));
do {
  {
    BI opval = EQHI (tmp_tmp, 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
  {
    BI opval = LTHI (tmp_tmp, 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
} while (0);
  {
    HI opval = tmp_tmp;
    SETMEMHI (current_cpu, pc, * FLD (i_Ri), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
} while (0);

  return vpc;
#undef FLD
}

/* andb: andb $Rj,@$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,andb) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_andb.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  QI tmp_tmp;
  tmp_tmp = ANDQI (GETMEMQI (current_cpu, pc, * FLD (i_Ri)), * FLD (i_Rj));
do {
  {
    BI opval = EQQI (tmp_tmp, 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
  {
    BI opval = LTQI (tmp_tmp, 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
} while (0);
  {
    QI opval = tmp_tmp;
    SETMEMQI (current_cpu, pc, * FLD (i_Ri), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
} while (0);

  return vpc;
#undef FLD
}

/* orm: or $Rj,@$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,orm) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_andm.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  SI tmp_tmp;
  tmp_tmp = ORSI (GETMEMSI (current_cpu, pc, * FLD (i_Ri)), * FLD (i_Rj));
do {
  {
    BI opval = EQSI (tmp_tmp, 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
  {
    BI opval = LTSI (tmp_tmp, 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
} while (0);
  {
    SI opval = tmp_tmp;
    SETMEMSI (current_cpu, pc, * FLD (i_Ri), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
} while (0);

  return vpc;
#undef FLD
}

/* orh: orh $Rj,@$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,orh) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_andh.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  HI tmp_tmp;
  tmp_tmp = ORHI (GETMEMHI (current_cpu, pc, * FLD (i_Ri)), * FLD (i_Rj));
do {
  {
    BI opval = EQHI (tmp_tmp, 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
  {
    BI opval = LTHI (tmp_tmp, 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
} while (0);
  {
    HI opval = tmp_tmp;
    SETMEMHI (current_cpu, pc, * FLD (i_Ri), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
} while (0);

  return vpc;
#undef FLD
}

/* orb: orb $Rj,@$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,orb) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_andb.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  QI tmp_tmp;
  tmp_tmp = ORQI (GETMEMQI (current_cpu, pc, * FLD (i_Ri)), * FLD (i_Rj));
do {
  {
    BI opval = EQQI (tmp_tmp, 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
  {
    BI opval = LTQI (tmp_tmp, 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
} while (0);
  {
    QI opval = tmp_tmp;
    SETMEMQI (current_cpu, pc, * FLD (i_Ri), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
} while (0);

  return vpc;
#undef FLD
}

/* eorm: eor $Rj,@$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,eorm) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_andm.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  SI tmp_tmp;
  tmp_tmp = XORSI (GETMEMSI (current_cpu, pc, * FLD (i_Ri)), * FLD (i_Rj));
do {
  {
    BI opval = EQSI (tmp_tmp, 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
  {
    BI opval = LTSI (tmp_tmp, 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
} while (0);
  {
    SI opval = tmp_tmp;
    SETMEMSI (current_cpu, pc, * FLD (i_Ri), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
} while (0);

  return vpc;
#undef FLD
}

/* eorh: eorh $Rj,@$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,eorh) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_andh.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  HI tmp_tmp;
  tmp_tmp = XORHI (GETMEMHI (current_cpu, pc, * FLD (i_Ri)), * FLD (i_Rj));
do {
  {
    BI opval = EQHI (tmp_tmp, 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
  {
    BI opval = LTHI (tmp_tmp, 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
} while (0);
  {
    HI opval = tmp_tmp;
    SETMEMHI (current_cpu, pc, * FLD (i_Ri), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
} while (0);

  return vpc;
#undef FLD
}

/* eorb: eorb $Rj,@$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,eorb) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_andb.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  QI tmp_tmp;
  tmp_tmp = XORQI (GETMEMQI (current_cpu, pc, * FLD (i_Ri)), * FLD (i_Rj));
do {
  {
    BI opval = EQQI (tmp_tmp, 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
  {
    BI opval = LTQI (tmp_tmp, 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
} while (0);
  {
    QI opval = tmp_tmp;
    SETMEMQI (current_cpu, pc, * FLD (i_Ri), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
} while (0);

  return vpc;
#undef FLD
}

/* bandl: bandl $u4,@$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,bandl) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_bandl.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    QI opval = ANDQI (ORQI (FLD (f_u4), 240), GETMEMQI (current_cpu, pc, * FLD (i_Ri)));
    SETMEMQI (current_cpu, pc, * FLD (i_Ri), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* borl: borl $u4,@$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,borl) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_bandl.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    QI opval = ORQI (FLD (f_u4), GETMEMQI (current_cpu, pc, * FLD (i_Ri)));
    SETMEMQI (current_cpu, pc, * FLD (i_Ri), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* beorl: beorl $u4,@$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,beorl) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_bandl.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    QI opval = XORQI (FLD (f_u4), GETMEMQI (current_cpu, pc, * FLD (i_Ri)));
    SETMEMQI (current_cpu, pc, * FLD (i_Ri), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* bandh: bandh $u4,@$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,bandh) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_bandl.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    QI opval = ANDQI (ORQI (SLLQI (FLD (f_u4), 4), 15), GETMEMQI (current_cpu, pc, * FLD (i_Ri)));
    SETMEMQI (current_cpu, pc, * FLD (i_Ri), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* borh: borh $u4,@$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,borh) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_bandl.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    QI opval = ORQI (SLLQI (FLD (f_u4), 4), GETMEMQI (current_cpu, pc, * FLD (i_Ri)));
    SETMEMQI (current_cpu, pc, * FLD (i_Ri), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* beorh: beorh $u4,@$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,beorh) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_bandl.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    QI opval = XORQI (SLLQI (FLD (f_u4), 4), GETMEMQI (current_cpu, pc, * FLD (i_Ri)));
    SETMEMQI (current_cpu, pc, * FLD (i_Ri), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* btstl: btstl $u4,@$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,btstl) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_btstl.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  QI tmp_tmp;
  tmp_tmp = ANDQI (FLD (f_u4), GETMEMQI (current_cpu, pc, * FLD (i_Ri)));
  {
    BI opval = EQQI (tmp_tmp, 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
} while (0);

  return vpc;
#undef FLD
}

/* btsth: btsth $u4,@$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,btsth) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_btstl.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  QI tmp_tmp;
  tmp_tmp = ANDQI (SLLQI (FLD (f_u4), 4), GETMEMQI (current_cpu, pc, * FLD (i_Ri)));
  {
    BI opval = EQQI (tmp_tmp, 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
  {
    BI opval = LTQI (tmp_tmp, 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
} while (0);

  return vpc;
#undef FLD
}

/* mul: mul $Rj,$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,mul) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mul.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  DI tmp_tmp;
  tmp_tmp = MULDI (EXTSIDI (* FLD (i_Rj)), EXTSIDI (* FLD (i_Ri)));
  {
    SI opval = TRUNCDISI (tmp_tmp);
    SET_H_DR (((UINT) 5), opval);
    TRACE_RESULT (current_cpu, abuf, "dr-5", 'x', opval);
  }
  {
    SI opval = TRUNCDISI (SRLDI (tmp_tmp, 32));
    SET_H_DR (((UINT) 4), opval);
    TRACE_RESULT (current_cpu, abuf, "dr-4", 'x', opval);
  }
  {
    BI opval = LTSI (GET_H_DR (((UINT) 5)), 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQDI (tmp_tmp, MAKEDI (0, 0));
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
  {
    BI opval = ORIF (GTDI (tmp_tmp, MAKEDI (0, 2147483647)), LTDI (tmp_tmp, NEGDI (MAKEDI (0, 0x80000000))));
    CPU (h_vbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
} while (0);

  return vpc;
#undef FLD
}

/* mulu: mulu $Rj,$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,mulu) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulu.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  DI tmp_tmp;
  tmp_tmp = MULDI (ZEXTSIDI (* FLD (i_Rj)), ZEXTSIDI (* FLD (i_Ri)));
  {
    SI opval = TRUNCDISI (tmp_tmp);
    SET_H_DR (((UINT) 5), opval);
    TRACE_RESULT (current_cpu, abuf, "dr-5", 'x', opval);
  }
  {
    SI opval = TRUNCDISI (SRLDI (tmp_tmp, 32));
    SET_H_DR (((UINT) 4), opval);
    TRACE_RESULT (current_cpu, abuf, "dr-4", 'x', opval);
  }
  {
    BI opval = LTSI (GET_H_DR (((UINT) 4)), 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (GET_H_DR (((UINT) 5)), 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
  {
    BI opval = NESI (GET_H_DR (((UINT) 4)), 0);
    CPU (h_vbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "vbit", 'x', opval);
  }
} while (0);

  return vpc;
#undef FLD
}

/* mulh: mulh $Rj,$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,mulh) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulh.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  {
    SI opval = MULHI (TRUNCSIHI (* FLD (i_Rj)), TRUNCSIHI (* FLD (i_Ri)));
    SET_H_DR (((UINT) 5), opval);
    TRACE_RESULT (current_cpu, abuf, "dr-5", 'x', opval);
  }
  {
    BI opval = LTSI (GET_H_DR (((UINT) 5)), 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = GESI (GET_H_DR (((UINT) 5)), 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
} while (0);

  return vpc;
#undef FLD
}

/* muluh: muluh $Rj,$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,muluh) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mulh.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  {
    SI opval = MULSI (ANDSI (* FLD (i_Rj), 65535), ANDSI (* FLD (i_Ri), 65535));
    SET_H_DR (((UINT) 5), opval);
    TRACE_RESULT (current_cpu, abuf, "dr-5", 'x', opval);
  }
  {
    BI opval = LTSI (GET_H_DR (((UINT) 5)), 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = GESI (GET_H_DR (((UINT) 5)), 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
} while (0);

  return vpc;
#undef FLD
}

/* div0s: div0s $Ri */

SEM_PC
SEM_FN_NAME (fr30bf,div0s) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_div0s.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  {
    BI opval = LTSI (GET_H_DR (((UINT) 5)), 0);
    CPU (h_d0bit) = opval;
    TRACE_RESULT (current_cpu, abuf, "d0bit", 'x', opval);
  }
  {
    BI opval = XORBI (CPU (h_d0bit), LTSI (* FLD (i_Ri), 0));
    CPU (h_d1bit) = opval;
    TRACE_RESULT (current_cpu, abuf, "d1bit", 'x', opval);
  }
if (NEBI (CPU (h_d0bit), 0)) {
  {
    SI opval = 0xffffffff;
    SET_H_DR (((UINT) 4), opval);
    written |= (1 << 5);
    TRACE_RESULT (current_cpu, abuf, "dr-4", 'x', opval);
  }
} else {
  {
    SI opval = 0;
    SET_H_DR (((UINT) 4), opval);
    written |= (1 << 5);
    TRACE_RESULT (current_cpu, abuf, "dr-4", 'x', opval);
  }
}
} while (0);

  abuf->written = written;
  return vpc;
#undef FLD
}

/* div0u: div0u $Ri */

SEM_PC
SEM_FN_NAME (fr30bf,div0u) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_div0u.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  {
    BI opval = 0;
    CPU (h_d0bit) = opval;
    TRACE_RESULT (current_cpu, abuf, "d0bit", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_d1bit) = opval;
    TRACE_RESULT (current_cpu, abuf, "d1bit", 'x', opval);
  }
  {
    SI opval = 0;
    SET_H_DR (((UINT) 4), opval);
    TRACE_RESULT (current_cpu, abuf, "dr-4", 'x', opval);
  }
} while (0);

  return vpc;
#undef FLD
}

/* div1: div1 $Ri */

SEM_PC
SEM_FN_NAME (fr30bf,div1) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_div1.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  SI tmp_tmp;
  {
    SI opval = SLLSI (GET_H_DR (((UINT) 4)), 1);
    SET_H_DR (((UINT) 4), opval);
    TRACE_RESULT (current_cpu, abuf, "dr-4", 'x', opval);
  }
if (LTSI (GET_H_DR (((UINT) 5)), 0)) {
  {
    SI opval = ADDSI (GET_H_DR (((UINT) 4)), 1);
    SET_H_DR (((UINT) 4), opval);
    TRACE_RESULT (current_cpu, abuf, "dr-4", 'x', opval);
  }
}
  {
    SI opval = SLLSI (GET_H_DR (((UINT) 5)), 1);
    SET_H_DR (((UINT) 5), opval);
    TRACE_RESULT (current_cpu, abuf, "dr-5", 'x', opval);
  }
if (EQBI (CPU (h_d1bit), 1)) {
do {
  tmp_tmp = ADDSI (GET_H_DR (((UINT) 4)), * FLD (i_Ri));
  {
    BI opval = ADDCFSI (GET_H_DR (((UINT) 4)), * FLD (i_Ri), 0);
    CPU (h_cbit) = opval;
    written |= (1 << 6);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
} while (0);
} else {
do {
  tmp_tmp = SUBSI (GET_H_DR (((UINT) 4)), * FLD (i_Ri));
  {
    BI opval = SUBCFSI (GET_H_DR (((UINT) 4)), * FLD (i_Ri), 0);
    CPU (h_cbit) = opval;
    written |= (1 << 6);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
} while (0);
}
if (NOTBI (XORBI (XORBI (CPU (h_d0bit), CPU (h_d1bit)), CPU (h_cbit)))) {
do {
  {
    SI opval = tmp_tmp;
    SET_H_DR (((UINT) 4), opval);
    TRACE_RESULT (current_cpu, abuf, "dr-4", 'x', opval);
  }
  {
    SI opval = ORSI (GET_H_DR (((UINT) 5)), 1);
    SET_H_DR (((UINT) 5), opval);
    TRACE_RESULT (current_cpu, abuf, "dr-5", 'x', opval);
  }
} while (0);
}
  {
    BI opval = EQSI (GET_H_DR (((UINT) 4)), 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
} while (0);

  abuf->written = written;
  return vpc;
#undef FLD
}

/* div2: div2 $Ri */

SEM_PC
SEM_FN_NAME (fr30bf,div2) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_div2.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  SI tmp_tmp;
if (EQBI (CPU (h_d1bit), 1)) {
do {
  tmp_tmp = ADDSI (GET_H_DR (((UINT) 4)), * FLD (i_Ri));
  {
    BI opval = ADDCFSI (GET_H_DR (((UINT) 4)), * FLD (i_Ri), 0);
    CPU (h_cbit) = opval;
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
} while (0);
} else {
do {
  tmp_tmp = SUBSI (GET_H_DR (((UINT) 4)), * FLD (i_Ri));
  {
    BI opval = SUBCFSI (GET_H_DR (((UINT) 4)), * FLD (i_Ri), 0);
    CPU (h_cbit) = opval;
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
} while (0);
}
if (EQSI (tmp_tmp, 0)) {
do {
  {
    BI opval = 1;
    CPU (h_zbit) = opval;
    written |= (1 << 5);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
  {
    SI opval = 0;
    SET_H_DR (((UINT) 4), opval);
    written |= (1 << 4);
    TRACE_RESULT (current_cpu, abuf, "dr-4", 'x', opval);
  }
} while (0);
} else {
  {
    BI opval = 0;
    CPU (h_zbit) = opval;
    written |= (1 << 5);
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
}
} while (0);

  abuf->written = written;
  return vpc;
#undef FLD
}

/* div3: div3 */

SEM_PC
SEM_FN_NAME (fr30bf,div3) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_div3.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

if (EQBI (CPU (h_zbit), 1)) {
  {
    SI opval = ADDSI (GET_H_DR (((UINT) 5)), 1);
    SET_H_DR (((UINT) 5), opval);
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "dr-5", 'x', opval);
  }
}

  abuf->written = written;
  return vpc;
#undef FLD
}

/* div4s: div4s */

SEM_PC
SEM_FN_NAME (fr30bf,div4s) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_div4s.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

if (EQBI (CPU (h_d1bit), 1)) {
  {
    SI opval = NEGSI (GET_H_DR (((UINT) 5)));
    SET_H_DR (((UINT) 5), opval);
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "dr-5", 'x', opval);
  }
}

  abuf->written = written;
  return vpc;
#undef FLD
}

/* lsl: lsl $Rj,$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,lsl) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_lsl.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  SI tmp_shift;
  tmp_shift = ANDSI (* FLD (i_Rj), 31);
if (NESI (tmp_shift, 0)) {
do {
  {
    BI opval = NESI (ANDSI (* FLD (i_Ri), SLLSI (1, SUBSI (32, tmp_shift))), 0);
    CPU (h_cbit) = opval;
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
  {
    SI opval = SLLSI (* FLD (i_Ri), tmp_shift);
    * FLD (i_Ri) = opval;
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "Ri", 'x', opval);
  }
} while (0);
} else {
  {
    BI opval = 0;
    CPU (h_cbit) = opval;
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
  {
    BI opval = LTSI (* FLD (i_Ri), 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (* FLD (i_Ri), 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
} while (0);

  abuf->written = written;
  return vpc;
#undef FLD
}

/* lsli: lsl $u4,$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,lsli) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_lsli.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  SI tmp_shift;
  tmp_shift = FLD (f_u4);
if (NESI (tmp_shift, 0)) {
do {
  {
    BI opval = NESI (ANDSI (* FLD (i_Ri), SLLSI (1, SUBSI (32, tmp_shift))), 0);
    CPU (h_cbit) = opval;
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
  {
    SI opval = SLLSI (* FLD (i_Ri), tmp_shift);
    * FLD (i_Ri) = opval;
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "Ri", 'x', opval);
  }
} while (0);
} else {
  {
    BI opval = 0;
    CPU (h_cbit) = opval;
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
  {
    BI opval = LTSI (* FLD (i_Ri), 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (* FLD (i_Ri), 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
} while (0);

  abuf->written = written;
  return vpc;
#undef FLD
}

/* lsl2: lsl2 $u4,$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,lsl2) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_lsli.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  SI tmp_shift;
  tmp_shift = ADDSI (FLD (f_u4), 16);
if (NESI (tmp_shift, 0)) {
do {
  {
    BI opval = NESI (ANDSI (* FLD (i_Ri), SLLSI (1, SUBSI (32, tmp_shift))), 0);
    CPU (h_cbit) = opval;
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
  {
    SI opval = SLLSI (* FLD (i_Ri), tmp_shift);
    * FLD (i_Ri) = opval;
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "Ri", 'x', opval);
  }
} while (0);
} else {
  {
    BI opval = 0;
    CPU (h_cbit) = opval;
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
  {
    BI opval = LTSI (* FLD (i_Ri), 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (* FLD (i_Ri), 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
} while (0);

  abuf->written = written;
  return vpc;
#undef FLD
}

/* lsr: lsr $Rj,$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,lsr) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_lsl.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  SI tmp_shift;
  tmp_shift = ANDSI (* FLD (i_Rj), 31);
if (NESI (tmp_shift, 0)) {
do {
  {
    BI opval = NESI (ANDSI (* FLD (i_Ri), SLLSI (1, SUBSI (tmp_shift, 1))), 0);
    CPU (h_cbit) = opval;
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
  {
    SI opval = SRLSI (* FLD (i_Ri), tmp_shift);
    * FLD (i_Ri) = opval;
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "Ri", 'x', opval);
  }
} while (0);
} else {
  {
    BI opval = 0;
    CPU (h_cbit) = opval;
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
  {
    BI opval = LTSI (* FLD (i_Ri), 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (* FLD (i_Ri), 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
} while (0);

  abuf->written = written;
  return vpc;
#undef FLD
}

/* lsri: lsr $u4,$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,lsri) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_lsli.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  SI tmp_shift;
  tmp_shift = FLD (f_u4);
if (NESI (tmp_shift, 0)) {
do {
  {
    BI opval = NESI (ANDSI (* FLD (i_Ri), SLLSI (1, SUBSI (tmp_shift, 1))), 0);
    CPU (h_cbit) = opval;
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
  {
    SI opval = SRLSI (* FLD (i_Ri), tmp_shift);
    * FLD (i_Ri) = opval;
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "Ri", 'x', opval);
  }
} while (0);
} else {
  {
    BI opval = 0;
    CPU (h_cbit) = opval;
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
  {
    BI opval = LTSI (* FLD (i_Ri), 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (* FLD (i_Ri), 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
} while (0);

  abuf->written = written;
  return vpc;
#undef FLD
}

/* lsr2: lsr2 $u4,$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,lsr2) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_lsli.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  SI tmp_shift;
  tmp_shift = ADDSI (FLD (f_u4), 16);
if (NESI (tmp_shift, 0)) {
do {
  {
    BI opval = NESI (ANDSI (* FLD (i_Ri), SLLSI (1, SUBSI (tmp_shift, 1))), 0);
    CPU (h_cbit) = opval;
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
  {
    SI opval = SRLSI (* FLD (i_Ri), tmp_shift);
    * FLD (i_Ri) = opval;
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "Ri", 'x', opval);
  }
} while (0);
} else {
  {
    BI opval = 0;
    CPU (h_cbit) = opval;
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
  {
    BI opval = LTSI (* FLD (i_Ri), 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (* FLD (i_Ri), 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
} while (0);

  abuf->written = written;
  return vpc;
#undef FLD
}

/* asr: asr $Rj,$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,asr) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_lsl.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  SI tmp_shift;
  tmp_shift = ANDSI (* FLD (i_Rj), 31);
if (NESI (tmp_shift, 0)) {
do {
  {
    BI opval = NESI (ANDSI (* FLD (i_Ri), SLLSI (1, SUBSI (tmp_shift, 1))), 0);
    CPU (h_cbit) = opval;
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
  {
    SI opval = SRASI (* FLD (i_Ri), tmp_shift);
    * FLD (i_Ri) = opval;
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "Ri", 'x', opval);
  }
} while (0);
} else {
  {
    BI opval = 0;
    CPU (h_cbit) = opval;
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
  {
    BI opval = LTSI (* FLD (i_Ri), 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (* FLD (i_Ri), 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
} while (0);

  abuf->written = written;
  return vpc;
#undef FLD
}

/* asri: asr $u4,$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,asri) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_lsli.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  SI tmp_shift;
  tmp_shift = FLD (f_u4);
if (NESI (tmp_shift, 0)) {
do {
  {
    BI opval = NESI (ANDSI (* FLD (i_Ri), SLLSI (1, SUBSI (tmp_shift, 1))), 0);
    CPU (h_cbit) = opval;
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
  {
    SI opval = SRASI (* FLD (i_Ri), tmp_shift);
    * FLD (i_Ri) = opval;
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "Ri", 'x', opval);
  }
} while (0);
} else {
  {
    BI opval = 0;
    CPU (h_cbit) = opval;
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
  {
    BI opval = LTSI (* FLD (i_Ri), 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (* FLD (i_Ri), 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
} while (0);

  abuf->written = written;
  return vpc;
#undef FLD
}

/* asr2: asr2 $u4,$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,asr2) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_lsli.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  SI tmp_shift;
  tmp_shift = ADDSI (FLD (f_u4), 16);
if (NESI (tmp_shift, 0)) {
do {
  {
    BI opval = NESI (ANDSI (* FLD (i_Ri), SLLSI (1, SUBSI (tmp_shift, 1))), 0);
    CPU (h_cbit) = opval;
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
  {
    SI opval = SRASI (* FLD (i_Ri), tmp_shift);
    * FLD (i_Ri) = opval;
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "Ri", 'x', opval);
  }
} while (0);
} else {
  {
    BI opval = 0;
    CPU (h_cbit) = opval;
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "cbit", 'x', opval);
  }
}
  {
    BI opval = LTSI (* FLD (i_Ri), 0);
    CPU (h_nbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "nbit", 'x', opval);
  }
  {
    BI opval = EQSI (* FLD (i_Ri), 0);
    CPU (h_zbit) = opval;
    TRACE_RESULT (current_cpu, abuf, "zbit", 'x', opval);
  }
} while (0);

  abuf->written = written;
  return vpc;
#undef FLD
}

/* ldi8: ldi:8 $i8,$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,ldi8) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldi8.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = FLD (f_i8);
    * FLD (i_Ri) = opval;
    TRACE_RESULT (current_cpu, abuf, "Ri", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldi20: ldi:20 $i20,$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,ldi20) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldi20.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

  {
    SI opval = FLD (f_i20);
    * FLD (i_Ri) = opval;
    TRACE_RESULT (current_cpu, abuf, "Ri", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldi32: ldi:32 $i32,$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,ldi32) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldi32.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 6);

  {
    SI opval = FLD (f_i32);
    * FLD (i_Ri) = opval;
    TRACE_RESULT (current_cpu, abuf, "Ri", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ld: ld @$Rj,$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,ld) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ld.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = GETMEMSI (current_cpu, pc, * FLD (i_Rj));
    * FLD (i_Ri) = opval;
    TRACE_RESULT (current_cpu, abuf, "Ri", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* lduh: lduh @$Rj,$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,lduh) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_lduh.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = GETMEMUHI (current_cpu, pc, * FLD (i_Rj));
    * FLD (i_Ri) = opval;
    TRACE_RESULT (current_cpu, abuf, "Ri", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldub: ldub @$Rj,$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,ldub) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldub.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = GETMEMUQI (current_cpu, pc, * FLD (i_Rj));
    * FLD (i_Ri) = opval;
    TRACE_RESULT (current_cpu, abuf, "Ri", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldr13: ld @($R13,$Rj),$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,ldr13) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldr13.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (* FLD (i_Rj), CPU (h_gr[((UINT) 13)])));
    * FLD (i_Ri) = opval;
    TRACE_RESULT (current_cpu, abuf, "Ri", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldr13uh: lduh @($R13,$Rj),$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,ldr13uh) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldr13uh.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = GETMEMUHI (current_cpu, pc, ADDSI (* FLD (i_Rj), CPU (h_gr[((UINT) 13)])));
    * FLD (i_Ri) = opval;
    TRACE_RESULT (current_cpu, abuf, "Ri", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldr13ub: ldub @($R13,$Rj),$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,ldr13ub) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldr13ub.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = GETMEMUQI (current_cpu, pc, ADDSI (* FLD (i_Rj), CPU (h_gr[((UINT) 13)])));
    * FLD (i_Ri) = opval;
    TRACE_RESULT (current_cpu, abuf, "Ri", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldr14: ld @($R14,$disp10),$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,ldr14) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldr14.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (FLD (f_disp10), CPU (h_gr[((UINT) 14)])));
    * FLD (i_Ri) = opval;
    TRACE_RESULT (current_cpu, abuf, "Ri", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldr14uh: lduh @($R14,$disp9),$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,ldr14uh) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldr14uh.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = GETMEMUHI (current_cpu, pc, ADDSI (FLD (f_disp9), CPU (h_gr[((UINT) 14)])));
    * FLD (i_Ri) = opval;
    TRACE_RESULT (current_cpu, abuf, "Ri", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldr14ub: ldub @($R14,$disp8),$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,ldr14ub) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldr14ub.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = GETMEMUQI (current_cpu, pc, ADDSI (FLD (f_disp8), CPU (h_gr[((UINT) 14)])));
    * FLD (i_Ri) = opval;
    TRACE_RESULT (current_cpu, abuf, "Ri", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldr15: ld @($R15,$udisp6),$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,ldr15) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldr15.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (FLD (f_udisp6), CPU (h_gr[((UINT) 15)])));
    * FLD (i_Ri) = opval;
    TRACE_RESULT (current_cpu, abuf, "Ri", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldr15gr: ld @$R15+,$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,ldr15gr) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldr15gr.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  {
    SI opval = GETMEMSI (current_cpu, pc, CPU (h_gr[((UINT) 15)]));
    * FLD (i_Ri) = opval;
    TRACE_RESULT (current_cpu, abuf, "Ri", 'x', opval);
  }
if (NESI (FLD (f_Ri), 15)) {
  {
    SI opval = ADDSI (CPU (h_gr[((UINT) 15)]), 4);
    CPU (h_gr[((UINT) 15)]) = opval;
    written |= (1 << 4);
    TRACE_RESULT (current_cpu, abuf, "gr-15", 'x', opval);
  }
}
} while (0);

  abuf->written = written;
  return vpc;
#undef FLD
}

/* ldr15dr: ld @$R15+,$Rs2 */

SEM_PC
SEM_FN_NAME (fr30bf,ldr15dr) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldr15dr.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  SI tmp_tmp;
  tmp_tmp = GETMEMSI (current_cpu, pc, CPU (h_gr[((UINT) 15)]));
  {
    SI opval = ADDSI (CPU (h_gr[((UINT) 15)]), 4);
    CPU (h_gr[((UINT) 15)]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-15", 'x', opval);
  }
  {
    SI opval = tmp_tmp;
    SET_H_DR (FLD (f_Rs2), opval);
    TRACE_RESULT (current_cpu, abuf, "Rs2", 'x', opval);
  }
} while (0);

  return vpc;
#undef FLD
}

/* ldr15ps: ld @$R15+,$ps */

SEM_PC
SEM_FN_NAME (fr30bf,ldr15ps) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldr15ps.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  {
    USI opval = GETMEMSI (current_cpu, pc, CPU (h_gr[((UINT) 15)]));
    SET_H_PS (opval);
    TRACE_RESULT (current_cpu, abuf, "ps", 'x', opval);
  }
  {
    SI opval = ADDSI (CPU (h_gr[((UINT) 15)]), 4);
    CPU (h_gr[((UINT) 15)]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-15", 'x', opval);
  }
} while (0);

  return vpc;
#undef FLD
}

/* st: st $Ri,@$Rj */

SEM_PC
SEM_FN_NAME (fr30bf,st) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_st.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = * FLD (i_Ri);
    SETMEMSI (current_cpu, pc, * FLD (i_Rj), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* sth: sth $Ri,@$Rj */

SEM_PC
SEM_FN_NAME (fr30bf,sth) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_sth.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    HI opval = * FLD (i_Ri);
    SETMEMHI (current_cpu, pc, * FLD (i_Rj), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* stb: stb $Ri,@$Rj */

SEM_PC
SEM_FN_NAME (fr30bf,stb) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stb.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    QI opval = * FLD (i_Ri);
    SETMEMQI (current_cpu, pc, * FLD (i_Rj), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* str13: st $Ri,@($R13,$Rj) */

SEM_PC
SEM_FN_NAME (fr30bf,str13) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_str13.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = * FLD (i_Ri);
    SETMEMSI (current_cpu, pc, ADDSI (* FLD (i_Rj), CPU (h_gr[((UINT) 13)])), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* str13h: sth $Ri,@($R13,$Rj) */

SEM_PC
SEM_FN_NAME (fr30bf,str13h) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_str13h.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    HI opval = * FLD (i_Ri);
    SETMEMHI (current_cpu, pc, ADDSI (* FLD (i_Rj), CPU (h_gr[((UINT) 13)])), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* str13b: stb $Ri,@($R13,$Rj) */

SEM_PC
SEM_FN_NAME (fr30bf,str13b) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_str13b.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    QI opval = * FLD (i_Ri);
    SETMEMQI (current_cpu, pc, ADDSI (* FLD (i_Rj), CPU (h_gr[((UINT) 13)])), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* str14: st $Ri,@($R14,$disp10) */

SEM_PC
SEM_FN_NAME (fr30bf,str14) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_str14.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = * FLD (i_Ri);
    SETMEMSI (current_cpu, pc, ADDSI (FLD (f_disp10), CPU (h_gr[((UINT) 14)])), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* str14h: sth $Ri,@($R14,$disp9) */

SEM_PC
SEM_FN_NAME (fr30bf,str14h) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_str14h.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    HI opval = * FLD (i_Ri);
    SETMEMHI (current_cpu, pc, ADDSI (FLD (f_disp9), CPU (h_gr[((UINT) 14)])), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* str14b: stb $Ri,@($R14,$disp8) */

SEM_PC
SEM_FN_NAME (fr30bf,str14b) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_str14b.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    QI opval = * FLD (i_Ri);
    SETMEMQI (current_cpu, pc, ADDSI (FLD (f_disp8), CPU (h_gr[((UINT) 14)])), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* str15: st $Ri,@($R15,$udisp6) */

SEM_PC
SEM_FN_NAME (fr30bf,str15) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_str15.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = * FLD (i_Ri);
    SETMEMSI (current_cpu, pc, ADDSI (CPU (h_gr[((UINT) 15)]), FLD (f_udisp6)), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* str15gr: st $Ri,@-$R15 */

SEM_PC
SEM_FN_NAME (fr30bf,str15gr) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_str15gr.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  SI tmp_tmp;
  tmp_tmp = * FLD (i_Ri);
  {
    SI opval = SUBSI (CPU (h_gr[((UINT) 15)]), 4);
    CPU (h_gr[((UINT) 15)]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-15", 'x', opval);
  }
  {
    SI opval = tmp_tmp;
    SETMEMSI (current_cpu, pc, CPU (h_gr[((UINT) 15)]), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
} while (0);

  return vpc;
#undef FLD
}

/* str15dr: st $Rs2,@-$R15 */

SEM_PC
SEM_FN_NAME (fr30bf,str15dr) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_str15dr.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  SI tmp_tmp;
  tmp_tmp = GET_H_DR (FLD (f_Rs2));
  {
    SI opval = SUBSI (CPU (h_gr[((UINT) 15)]), 4);
    CPU (h_gr[((UINT) 15)]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-15", 'x', opval);
  }
  {
    SI opval = tmp_tmp;
    SETMEMSI (current_cpu, pc, CPU (h_gr[((UINT) 15)]), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
} while (0);

  return vpc;
#undef FLD
}

/* str15ps: st $ps,@-$R15 */

SEM_PC
SEM_FN_NAME (fr30bf,str15ps) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_str15ps.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  {
    SI opval = SUBSI (CPU (h_gr[((UINT) 15)]), 4);
    CPU (h_gr[((UINT) 15)]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-15", 'x', opval);
  }
  {
    SI opval = GET_H_PS ();
    SETMEMSI (current_cpu, pc, CPU (h_gr[((UINT) 15)]), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
} while (0);

  return vpc;
#undef FLD
}

/* mov: mov $Rj,$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,mov) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mov.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = * FLD (i_Rj);
    * FLD (i_Ri) = opval;
    TRACE_RESULT (current_cpu, abuf, "Ri", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* movdr: mov $Rs1,$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,movdr) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_movdr.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = GET_H_DR (FLD (f_Rs1));
    * FLD (i_Ri) = opval;
    TRACE_RESULT (current_cpu, abuf, "Ri", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* movps: mov $ps,$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,movps) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_movps.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = GET_H_PS ();
    * FLD (i_Ri) = opval;
    TRACE_RESULT (current_cpu, abuf, "Ri", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* mov2dr: mov $Ri,$Rs1 */

SEM_PC
SEM_FN_NAME (fr30bf,mov2dr) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mov2dr.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = * FLD (i_Ri);
    SET_H_DR (FLD (f_Rs1), opval);
    TRACE_RESULT (current_cpu, abuf, "Rs1", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* mov2ps: mov $Ri,$ps */

SEM_PC
SEM_FN_NAME (fr30bf,mov2ps) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_mov2ps.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    USI opval = * FLD (i_Ri);
    SET_H_PS (opval);
    TRACE_RESULT (current_cpu, abuf, "ps", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* jmp: jmp @$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,jmp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_jmp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    USI opval = * FLD (i_Ri);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }

  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* jmpd: jmp:d @$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,jmpd) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_jmp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  {
    USI opval = * FLD (i_Ri);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} while (0);

  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* callr: call @$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,callr) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_callr.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  {
    SI opval = ADDSI (pc, 2);
    SET_H_DR (((UINT) 1), opval);
    TRACE_RESULT (current_cpu, abuf, "dr-1", 'x', opval);
  }
  {
    USI opval = * FLD (i_Ri);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} while (0);

  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* callrd: call:d @$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,callrd) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_callr.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
do {
  {
    SI opval = ADDSI (pc, 4);
    SET_H_DR (((UINT) 1), opval);
    TRACE_RESULT (current_cpu, abuf, "dr-1", 'x', opval);
  }
  {
    USI opval = * FLD (i_Ri);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} while (0);
} while (0);

  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* call: call $label12 */

SEM_PC
SEM_FN_NAME (fr30bf,call) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_call.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  {
    SI opval = ADDSI (pc, 2);
    SET_H_DR (((UINT) 1), opval);
    TRACE_RESULT (current_cpu, abuf, "dr-1", 'x', opval);
  }
  {
    USI opval = FLD (i_label12);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} while (0);

  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* calld: call:d $label12 */

SEM_PC
SEM_FN_NAME (fr30bf,calld) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_call.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
do {
  {
    SI opval = ADDSI (pc, 4);
    SET_H_DR (((UINT) 1), opval);
    TRACE_RESULT (current_cpu, abuf, "dr-1", 'x', opval);
  }
  {
    USI opval = FLD (i_label12);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} while (0);
} while (0);

  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* ret: ret */

SEM_PC
SEM_FN_NAME (fr30bf,ret) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_ret.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    USI opval = GET_H_DR (((UINT) 1));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }

  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* ret:d: ret:d */

SEM_PC
SEM_FN_NAME (fr30bf,ret_d) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_ret.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  {
    USI opval = GET_H_DR (((UINT) 1));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} while (0);

  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* int: int $u8 */

SEM_PC
SEM_FN_NAME (fr30bf,int) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_int.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
; /*clobber*/
; /*clobber*/
; /*clobber*/
  {
    SI opval = fr30_int (current_cpu, pc, FLD (f_u8));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} while (0);

  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* inte: inte */

SEM_PC
SEM_FN_NAME (fr30bf,inte) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_inte.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
; /*clobber*/
; /*clobber*/
; /*clobber*/
  {
    SI opval = fr30_inte (current_cpu, pc);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} while (0);

  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* reti: reti */

SEM_PC
SEM_FN_NAME (fr30bf,reti) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_reti.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

if (EQBI (GET_H_SBIT (), 0)) {
do {
  {
    SI opval = GETMEMSI (current_cpu, pc, GET_H_DR (((UINT) 2)));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
  {
    SI opval = ADDSI (GET_H_DR (((UINT) 2)), 4);
    SET_H_DR (((UINT) 2), opval);
    written |= (1 << 5);
    TRACE_RESULT (current_cpu, abuf, "dr-2", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, GET_H_DR (((UINT) 2)));
    SET_H_PS (opval);
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "ps", 'x', opval);
  }
  {
    SI opval = ADDSI (GET_H_DR (((UINT) 2)), 4);
    SET_H_DR (((UINT) 2), opval);
    written |= (1 << 5);
    TRACE_RESULT (current_cpu, abuf, "dr-2", 'x', opval);
  }
} while (0);
} else {
do {
  {
    SI opval = GETMEMSI (current_cpu, pc, GET_H_DR (((UINT) 3)));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
  {
    SI opval = ADDSI (GET_H_DR (((UINT) 3)), 4);
    SET_H_DR (((UINT) 3), opval);
    written |= (1 << 6);
    TRACE_RESULT (current_cpu, abuf, "dr-3", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, GET_H_DR (((UINT) 3)));
    SET_H_PS (opval);
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "ps", 'x', opval);
  }
  {
    SI opval = ADDSI (GET_H_DR (((UINT) 3)), 4);
    SET_H_DR (((UINT) 3), opval);
    written |= (1 << 6);
    TRACE_RESULT (current_cpu, abuf, "dr-3", 'x', opval);
  }
} while (0);
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* brad: bra:d $label9 */

SEM_PC
SEM_FN_NAME (fr30bf,brad) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_brad.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  {
    USI opval = FLD (i_label9);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} while (0);

  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* bra: bra $label9 */

SEM_PC
SEM_FN_NAME (fr30bf,bra) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_brad.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    USI opval = FLD (i_label9);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }

  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* bnod: bno:d $label9 */

SEM_PC
SEM_FN_NAME (fr30bf,bnod) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_bnod.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
do { } while (0); /*nop*/
} while (0);

  return vpc;
#undef FLD
}

/* bno: bno $label9 */

SEM_PC
SEM_FN_NAME (fr30bf,bno) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_bnod.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do { } while (0); /*nop*/

  return vpc;
#undef FLD
}

/* beqd: beq:d $label9 */

SEM_PC
SEM_FN_NAME (fr30bf,beqd) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_beqd.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
if (CPU (h_zbit)) {
  {
    USI opval = FLD (i_label9);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
} while (0);

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* beq: beq $label9 */

SEM_PC
SEM_FN_NAME (fr30bf,beq) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_beqd.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

if (CPU (h_zbit)) {
  {
    USI opval = FLD (i_label9);
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

/* bned: bne:d $label9 */

SEM_PC
SEM_FN_NAME (fr30bf,bned) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_beqd.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
if (NOTBI (CPU (h_zbit))) {
  {
    USI opval = FLD (i_label9);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
} while (0);

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* bne: bne $label9 */

SEM_PC
SEM_FN_NAME (fr30bf,bne) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_beqd.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

if (NOTBI (CPU (h_zbit))) {
  {
    USI opval = FLD (i_label9);
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

/* bcd: bc:d $label9 */

SEM_PC
SEM_FN_NAME (fr30bf,bcd) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bcd.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
if (CPU (h_cbit)) {
  {
    USI opval = FLD (i_label9);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
} while (0);

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* bc: bc $label9 */

SEM_PC
SEM_FN_NAME (fr30bf,bc) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bcd.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

if (CPU (h_cbit)) {
  {
    USI opval = FLD (i_label9);
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

/* bncd: bnc:d $label9 */

SEM_PC
SEM_FN_NAME (fr30bf,bncd) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bcd.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
if (NOTBI (CPU (h_cbit))) {
  {
    USI opval = FLD (i_label9);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
} while (0);

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* bnc: bnc $label9 */

SEM_PC
SEM_FN_NAME (fr30bf,bnc) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bcd.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

if (NOTBI (CPU (h_cbit))) {
  {
    USI opval = FLD (i_label9);
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

/* bnd: bn:d $label9 */

SEM_PC
SEM_FN_NAME (fr30bf,bnd) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bnd.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
if (CPU (h_nbit)) {
  {
    USI opval = FLD (i_label9);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
} while (0);

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* bn: bn $label9 */

SEM_PC
SEM_FN_NAME (fr30bf,bn) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bnd.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

if (CPU (h_nbit)) {
  {
    USI opval = FLD (i_label9);
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

/* bpd: bp:d $label9 */

SEM_PC
SEM_FN_NAME (fr30bf,bpd) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bnd.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
if (NOTBI (CPU (h_nbit))) {
  {
    USI opval = FLD (i_label9);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
} while (0);

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* bp: bp $label9 */

SEM_PC
SEM_FN_NAME (fr30bf,bp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bnd.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

if (NOTBI (CPU (h_nbit))) {
  {
    USI opval = FLD (i_label9);
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

/* bvd: bv:d $label9 */

SEM_PC
SEM_FN_NAME (fr30bf,bvd) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bvd.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
if (CPU (h_vbit)) {
  {
    USI opval = FLD (i_label9);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
} while (0);

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* bv: bv $label9 */

SEM_PC
SEM_FN_NAME (fr30bf,bv) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bvd.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

if (CPU (h_vbit)) {
  {
    USI opval = FLD (i_label9);
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

/* bnvd: bnv:d $label9 */

SEM_PC
SEM_FN_NAME (fr30bf,bnvd) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bvd.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
if (NOTBI (CPU (h_vbit))) {
  {
    USI opval = FLD (i_label9);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
} while (0);

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* bnv: bnv $label9 */

SEM_PC
SEM_FN_NAME (fr30bf,bnv) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bvd.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

if (NOTBI (CPU (h_vbit))) {
  {
    USI opval = FLD (i_label9);
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

/* bltd: blt:d $label9 */

SEM_PC
SEM_FN_NAME (fr30bf,bltd) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bltd.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
if (XORBI (CPU (h_vbit), CPU (h_nbit))) {
  {
    USI opval = FLD (i_label9);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
} while (0);

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* blt: blt $label9 */

SEM_PC
SEM_FN_NAME (fr30bf,blt) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bltd.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

if (XORBI (CPU (h_vbit), CPU (h_nbit))) {
  {
    USI opval = FLD (i_label9);
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

/* bged: bge:d $label9 */

SEM_PC
SEM_FN_NAME (fr30bf,bged) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bltd.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
if (NOTBI (XORBI (CPU (h_vbit), CPU (h_nbit)))) {
  {
    USI opval = FLD (i_label9);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
} while (0);

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* bge: bge $label9 */

SEM_PC
SEM_FN_NAME (fr30bf,bge) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bltd.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

if (NOTBI (XORBI (CPU (h_vbit), CPU (h_nbit)))) {
  {
    USI opval = FLD (i_label9);
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

/* bled: ble:d $label9 */

SEM_PC
SEM_FN_NAME (fr30bf,bled) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bled.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
if (ORBI (XORBI (CPU (h_vbit), CPU (h_nbit)), CPU (h_zbit))) {
  {
    USI opval = FLD (i_label9);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 4);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
} while (0);

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* ble: ble $label9 */

SEM_PC
SEM_FN_NAME (fr30bf,ble) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bled.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

if (ORBI (XORBI (CPU (h_vbit), CPU (h_nbit)), CPU (h_zbit))) {
  {
    USI opval = FLD (i_label9);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 4);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* bgtd: bgt:d $label9 */

SEM_PC
SEM_FN_NAME (fr30bf,bgtd) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bled.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
if (NOTBI (ORBI (XORBI (CPU (h_vbit), CPU (h_nbit)), CPU (h_zbit)))) {
  {
    USI opval = FLD (i_label9);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 4);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
} while (0);

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* bgt: bgt $label9 */

SEM_PC
SEM_FN_NAME (fr30bf,bgt) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bled.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

if (NOTBI (ORBI (XORBI (CPU (h_vbit), CPU (h_nbit)), CPU (h_zbit)))) {
  {
    USI opval = FLD (i_label9);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 4);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* blsd: bls:d $label9 */

SEM_PC
SEM_FN_NAME (fr30bf,blsd) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_blsd.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
if (ORBI (CPU (h_cbit), CPU (h_zbit))) {
  {
    USI opval = FLD (i_label9);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
} while (0);

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* bls: bls $label9 */

SEM_PC
SEM_FN_NAME (fr30bf,bls) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_blsd.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

if (ORBI (CPU (h_cbit), CPU (h_zbit))) {
  {
    USI opval = FLD (i_label9);
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

/* bhid: bhi:d $label9 */

SEM_PC
SEM_FN_NAME (fr30bf,bhid) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_blsd.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
if (NOTBI (ORBI (CPU (h_cbit), CPU (h_zbit)))) {
  {
    USI opval = FLD (i_label9);
    SEM_BRANCH_VIA_CACHE (current_cpu, sem_arg, opval, vpc, SEM_BRANCH_ADDR_CACHE (sem_arg));
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}
} while (0);

  abuf->written = written;
  SEM_BRANCH_FINI (vpc);
  return vpc;
#undef FLD
}

/* bhi: bhi $label9 */

SEM_PC
SEM_FN_NAME (fr30bf,bhi) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.cti.fields.fmt_blsd.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

if (NOTBI (ORBI (CPU (h_cbit), CPU (h_zbit)))) {
  {
    USI opval = FLD (i_label9);
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

/* dmovr13: dmov $R13,@$dir10 */

SEM_PC
SEM_FN_NAME (fr30bf,dmovr13) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_dmovr13.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = CPU (h_gr[((UINT) 13)]);
    SETMEMSI (current_cpu, pc, FLD (f_dir10), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* dmovr13h: dmovh $R13,@$dir9 */

SEM_PC
SEM_FN_NAME (fr30bf,dmovr13h) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_dmovr13h.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    HI opval = CPU (h_gr[((UINT) 13)]);
    SETMEMHI (current_cpu, pc, FLD (f_dir9), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* dmovr13b: dmovb $R13,@$dir8 */

SEM_PC
SEM_FN_NAME (fr30bf,dmovr13b) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_dmovr13b.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    QI opval = CPU (h_gr[((UINT) 13)]);
    SETMEMQI (current_cpu, pc, FLD (f_dir8), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* dmovr13pi: dmov @$R13+,@$dir10 */

SEM_PC
SEM_FN_NAME (fr30bf,dmovr13pi) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_dmovr13pi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  {
    SI opval = GETMEMSI (current_cpu, pc, CPU (h_gr[((UINT) 13)]));
    SETMEMSI (current_cpu, pc, FLD (f_dir10), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = ADDSI (CPU (h_gr[((UINT) 13)]), 4);
    CPU (h_gr[((UINT) 13)]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-13", 'x', opval);
  }
} while (0);

  return vpc;
#undef FLD
}

/* dmovr13pih: dmovh @$R13+,@$dir9 */

SEM_PC
SEM_FN_NAME (fr30bf,dmovr13pih) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_dmovr13pih.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  {
    HI opval = GETMEMHI (current_cpu, pc, CPU (h_gr[((UINT) 13)]));
    SETMEMHI (current_cpu, pc, FLD (f_dir9), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = ADDSI (CPU (h_gr[((UINT) 13)]), 2);
    CPU (h_gr[((UINT) 13)]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-13", 'x', opval);
  }
} while (0);

  return vpc;
#undef FLD
}

/* dmovr13pib: dmovb @$R13+,@$dir8 */

SEM_PC
SEM_FN_NAME (fr30bf,dmovr13pib) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_dmovr13pib.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  {
    QI opval = GETMEMQI (current_cpu, pc, CPU (h_gr[((UINT) 13)]));
    SETMEMQI (current_cpu, pc, FLD (f_dir8), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = ADDSI (CPU (h_gr[((UINT) 13)]), 1);
    CPU (h_gr[((UINT) 13)]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-13", 'x', opval);
  }
} while (0);

  return vpc;
#undef FLD
}

/* dmovr15pi: dmov @$R15+,@$dir10 */

SEM_PC
SEM_FN_NAME (fr30bf,dmovr15pi) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_dmovr15pi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  {
    SI opval = GETMEMSI (current_cpu, pc, CPU (h_gr[((UINT) 15)]));
    SETMEMSI (current_cpu, pc, FLD (f_dir10), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = ADDSI (CPU (h_gr[((UINT) 15)]), 4);
    CPU (h_gr[((UINT) 15)]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-15", 'x', opval);
  }
} while (0);

  return vpc;
#undef FLD
}

/* dmov2r13: dmov @$dir10,$R13 */

SEM_PC
SEM_FN_NAME (fr30bf,dmov2r13) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_dmov2r13.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = GETMEMSI (current_cpu, pc, FLD (f_dir10));
    CPU (h_gr[((UINT) 13)]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-13", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* dmov2r13h: dmovh @$dir9,$R13 */

SEM_PC
SEM_FN_NAME (fr30bf,dmov2r13h) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_dmov2r13h.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = GETMEMHI (current_cpu, pc, FLD (f_dir9));
    CPU (h_gr[((UINT) 13)]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-13", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* dmov2r13b: dmovb @$dir8,$R13 */

SEM_PC
SEM_FN_NAME (fr30bf,dmov2r13b) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_dmov2r13b.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = GETMEMQI (current_cpu, pc, FLD (f_dir8));
    CPU (h_gr[((UINT) 13)]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-13", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* dmov2r13pi: dmov @$dir10,@$R13+ */

SEM_PC
SEM_FN_NAME (fr30bf,dmov2r13pi) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_dmov2r13pi.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  {
    SI opval = GETMEMSI (current_cpu, pc, FLD (f_dir10));
    SETMEMSI (current_cpu, pc, CPU (h_gr[((UINT) 13)]), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = ADDSI (CPU (h_gr[((UINT) 13)]), 4);
    CPU (h_gr[((UINT) 13)]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-13", 'x', opval);
  }
} while (0);

  return vpc;
#undef FLD
}

/* dmov2r13pih: dmovh @$dir9,@$R13+ */

SEM_PC
SEM_FN_NAME (fr30bf,dmov2r13pih) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_dmov2r13pih.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  {
    HI opval = GETMEMHI (current_cpu, pc, FLD (f_dir9));
    SETMEMHI (current_cpu, pc, CPU (h_gr[((UINT) 13)]), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = ADDSI (CPU (h_gr[((UINT) 13)]), 2);
    CPU (h_gr[((UINT) 13)]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-13", 'x', opval);
  }
} while (0);

  return vpc;
#undef FLD
}

/* dmov2r13pib: dmovb @$dir8,@$R13+ */

SEM_PC
SEM_FN_NAME (fr30bf,dmov2r13pib) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_dmov2r13pib.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  {
    QI opval = GETMEMQI (current_cpu, pc, FLD (f_dir8));
    SETMEMQI (current_cpu, pc, CPU (h_gr[((UINT) 13)]), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = ADDSI (CPU (h_gr[((UINT) 13)]), 1);
    CPU (h_gr[((UINT) 13)]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-13", 'x', opval);
  }
} while (0);

  return vpc;
#undef FLD
}

/* dmov2r15pd: dmov @$dir10,@-$R15 */

SEM_PC
SEM_FN_NAME (fr30bf,dmov2r15pd) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_dmov2r15pd.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  {
    SI opval = SUBSI (CPU (h_gr[((UINT) 15)]), 4);
    CPU (h_gr[((UINT) 15)]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-15", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, FLD (f_dir10));
    SETMEMSI (current_cpu, pc, CPU (h_gr[((UINT) 15)]), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
} while (0);

  return vpc;
#undef FLD
}

/* ldres: ldres @$Ri+,$u4 */

SEM_PC
SEM_FN_NAME (fr30bf,ldres) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldres.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = ADDSI (* FLD (i_Ri), 4);
    * FLD (i_Ri) = opval;
    TRACE_RESULT (current_cpu, abuf, "Ri", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* stres: stres $u4,@$Ri+ */

SEM_PC
SEM_FN_NAME (fr30bf,stres) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldres.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = ADDSI (* FLD (i_Ri), 4);
    * FLD (i_Ri) = opval;
    TRACE_RESULT (current_cpu, abuf, "Ri", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* copop: copop $u4c,$ccc,$CRj,$CRi */

SEM_PC
SEM_FN_NAME (fr30bf,copop) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_copop.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

do { } while (0); /*nop*/

  return vpc;
#undef FLD
}

/* copld: copld $u4c,$ccc,$Rjc,$CRi */

SEM_PC
SEM_FN_NAME (fr30bf,copld) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_copld.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

do { } while (0); /*nop*/

  return vpc;
#undef FLD
}

/* copst: copst $u4c,$ccc,$CRj,$Ric */

SEM_PC
SEM_FN_NAME (fr30bf,copst) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_copst.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

do { } while (0); /*nop*/

  return vpc;
#undef FLD
}

/* copsv: copsv $u4c,$ccc,$CRj,$Ric */

SEM_PC
SEM_FN_NAME (fr30bf,copsv) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_copst.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);

do { } while (0); /*nop*/

  return vpc;
#undef FLD
}

/* nop: nop */

SEM_PC
SEM_FN_NAME (fr30bf,nop) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_nop.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do { } while (0); /*nop*/

  return vpc;
#undef FLD
}

/* andccr: andccr $u8 */

SEM_PC
SEM_FN_NAME (fr30bf,andccr) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_andccr.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    UQI opval = ANDQI (GET_H_CCR (), FLD (f_u8));
    SET_H_CCR (opval);
    TRACE_RESULT (current_cpu, abuf, "ccr", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* orccr: orccr $u8 */

SEM_PC
SEM_FN_NAME (fr30bf,orccr) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_andccr.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    UQI opval = ORQI (GET_H_CCR (), FLD (f_u8));
    SET_H_CCR (opval);
    TRACE_RESULT (current_cpu, abuf, "ccr", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* stilm: stilm $u8 */

SEM_PC
SEM_FN_NAME (fr30bf,stilm) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stilm.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    UQI opval = ANDSI (FLD (f_u8), 31);
    SET_H_ILM (opval);
    TRACE_RESULT (current_cpu, abuf, "ilm", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* addsp: addsp $s10 */

SEM_PC
SEM_FN_NAME (fr30bf,addsp) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_addsp.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = ADDSI (CPU (h_gr[((UINT) 15)]), FLD (f_s10));
    CPU (h_gr[((UINT) 15)]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-15", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* extsb: extsb $Ri */

SEM_PC
SEM_FN_NAME (fr30bf,extsb) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_extsb.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = EXTQISI (ANDQI (* FLD (i_Ri), 255));
    * FLD (i_Ri) = opval;
    TRACE_RESULT (current_cpu, abuf, "Ri", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* extub: extub $Ri */

SEM_PC
SEM_FN_NAME (fr30bf,extub) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_extub.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = ZEXTQISI (ANDQI (* FLD (i_Ri), 255));
    * FLD (i_Ri) = opval;
    TRACE_RESULT (current_cpu, abuf, "Ri", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* extsh: extsh $Ri */

SEM_PC
SEM_FN_NAME (fr30bf,extsh) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_extsh.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = EXTHISI (ANDHI (* FLD (i_Ri), 65535));
    * FLD (i_Ri) = opval;
    TRACE_RESULT (current_cpu, abuf, "Ri", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* extuh: extuh $Ri */

SEM_PC
SEM_FN_NAME (fr30bf,extuh) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_extuh.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

  {
    SI opval = ZEXTHISI (ANDHI (* FLD (i_Ri), 65535));
    * FLD (i_Ri) = opval;
    TRACE_RESULT (current_cpu, abuf, "Ri", 'x', opval);
  }

  return vpc;
#undef FLD
}

/* ldm0: ldm0 ($reglist_low_ld) */

SEM_PC
SEM_FN_NAME (fr30bf,ldm0) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldm0.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
if (ANDSI (FLD (f_reglist_low_ld), 1)) {
do {
  {
    SI opval = GETMEMSI (current_cpu, pc, CPU (h_gr[((UINT) 15)]));
    CPU (h_gr[((UINT) 0)]) = opval;
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "gr-0", 'x', opval);
  }
  {
    SI opval = ADDSI (CPU (h_gr[((UINT) 15)]), 4);
    CPU (h_gr[((UINT) 15)]) = opval;
    written |= (1 << 5);
    TRACE_RESULT (current_cpu, abuf, "gr-15", 'x', opval);
  }
} while (0);
}
if (ANDSI (FLD (f_reglist_low_ld), 2)) {
do {
  {
    SI opval = GETMEMSI (current_cpu, pc, CPU (h_gr[((UINT) 15)]));
    CPU (h_gr[((UINT) 1)]) = opval;
    written |= (1 << 4);
    TRACE_RESULT (current_cpu, abuf, "gr-1", 'x', opval);
  }
  {
    SI opval = ADDSI (CPU (h_gr[((UINT) 15)]), 4);
    CPU (h_gr[((UINT) 15)]) = opval;
    written |= (1 << 5);
    TRACE_RESULT (current_cpu, abuf, "gr-15", 'x', opval);
  }
} while (0);
}
if (ANDSI (FLD (f_reglist_low_ld), 4)) {
do {
  {
    SI opval = GETMEMSI (current_cpu, pc, CPU (h_gr[((UINT) 15)]));
    CPU (h_gr[((UINT) 2)]) = opval;
    written |= (1 << 6);
    TRACE_RESULT (current_cpu, abuf, "gr-2", 'x', opval);
  }
  {
    SI opval = ADDSI (CPU (h_gr[((UINT) 15)]), 4);
    CPU (h_gr[((UINT) 15)]) = opval;
    written |= (1 << 5);
    TRACE_RESULT (current_cpu, abuf, "gr-15", 'x', opval);
  }
} while (0);
}
if (ANDSI (FLD (f_reglist_low_ld), 8)) {
do {
  {
    SI opval = GETMEMSI (current_cpu, pc, CPU (h_gr[((UINT) 15)]));
    CPU (h_gr[((UINT) 3)]) = opval;
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "gr-3", 'x', opval);
  }
  {
    SI opval = ADDSI (CPU (h_gr[((UINT) 15)]), 4);
    CPU (h_gr[((UINT) 15)]) = opval;
    written |= (1 << 5);
    TRACE_RESULT (current_cpu, abuf, "gr-15", 'x', opval);
  }
} while (0);
}
if (ANDSI (FLD (f_reglist_low_ld), 16)) {
do {
  {
    SI opval = GETMEMSI (current_cpu, pc, CPU (h_gr[((UINT) 15)]));
    CPU (h_gr[((UINT) 4)]) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "gr-4", 'x', opval);
  }
  {
    SI opval = ADDSI (CPU (h_gr[((UINT) 15)]), 4);
    CPU (h_gr[((UINT) 15)]) = opval;
    written |= (1 << 5);
    TRACE_RESULT (current_cpu, abuf, "gr-15", 'x', opval);
  }
} while (0);
}
if (ANDSI (FLD (f_reglist_low_ld), 32)) {
do {
  {
    SI opval = GETMEMSI (current_cpu, pc, CPU (h_gr[((UINT) 15)]));
    CPU (h_gr[((UINT) 5)]) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "gr-5", 'x', opval);
  }
  {
    SI opval = ADDSI (CPU (h_gr[((UINT) 15)]), 4);
    CPU (h_gr[((UINT) 15)]) = opval;
    written |= (1 << 5);
    TRACE_RESULT (current_cpu, abuf, "gr-15", 'x', opval);
  }
} while (0);
}
if (ANDSI (FLD (f_reglist_low_ld), 64)) {
do {
  {
    SI opval = GETMEMSI (current_cpu, pc, CPU (h_gr[((UINT) 15)]));
    CPU (h_gr[((UINT) 6)]) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "gr-6", 'x', opval);
  }
  {
    SI opval = ADDSI (CPU (h_gr[((UINT) 15)]), 4);
    CPU (h_gr[((UINT) 15)]) = opval;
    written |= (1 << 5);
    TRACE_RESULT (current_cpu, abuf, "gr-15", 'x', opval);
  }
} while (0);
}
if (ANDSI (FLD (f_reglist_low_ld), 128)) {
do {
  {
    SI opval = GETMEMSI (current_cpu, pc, CPU (h_gr[((UINT) 15)]));
    CPU (h_gr[((UINT) 7)]) = opval;
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "gr-7", 'x', opval);
  }
  {
    SI opval = ADDSI (CPU (h_gr[((UINT) 15)]), 4);
    CPU (h_gr[((UINT) 15)]) = opval;
    written |= (1 << 5);
    TRACE_RESULT (current_cpu, abuf, "gr-15", 'x', opval);
  }
} while (0);
}
} while (0);

  abuf->written = written;
  return vpc;
#undef FLD
}

/* ldm1: ldm1 ($reglist_hi_ld) */

SEM_PC
SEM_FN_NAME (fr30bf,ldm1) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_ldm1.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
if (ANDSI (FLD (f_reglist_hi_ld), 1)) {
do {
  {
    SI opval = GETMEMSI (current_cpu, pc, CPU (h_gr[((UINT) 15)]));
    CPU (h_gr[((UINT) 8)]) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "gr-8", 'x', opval);
  }
  {
    SI opval = ADDSI (CPU (h_gr[((UINT) 15)]), 4);
    CPU (h_gr[((UINT) 15)]) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "gr-15", 'x', opval);
  }
} while (0);
}
if (ANDSI (FLD (f_reglist_hi_ld), 2)) {
do {
  {
    SI opval = GETMEMSI (current_cpu, pc, CPU (h_gr[((UINT) 15)]));
    CPU (h_gr[((UINT) 9)]) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "gr-9", 'x', opval);
  }
  {
    SI opval = ADDSI (CPU (h_gr[((UINT) 15)]), 4);
    CPU (h_gr[((UINT) 15)]) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "gr-15", 'x', opval);
  }
} while (0);
}
if (ANDSI (FLD (f_reglist_hi_ld), 4)) {
do {
  {
    SI opval = GETMEMSI (current_cpu, pc, CPU (h_gr[((UINT) 15)]));
    CPU (h_gr[((UINT) 10)]) = opval;
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "gr-10", 'x', opval);
  }
  {
    SI opval = ADDSI (CPU (h_gr[((UINT) 15)]), 4);
    CPU (h_gr[((UINT) 15)]) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "gr-15", 'x', opval);
  }
} while (0);
}
if (ANDSI (FLD (f_reglist_hi_ld), 8)) {
do {
  {
    SI opval = GETMEMSI (current_cpu, pc, CPU (h_gr[((UINT) 15)]));
    CPU (h_gr[((UINT) 11)]) = opval;
    written |= (1 << 4);
    TRACE_RESULT (current_cpu, abuf, "gr-11", 'x', opval);
  }
  {
    SI opval = ADDSI (CPU (h_gr[((UINT) 15)]), 4);
    CPU (h_gr[((UINT) 15)]) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "gr-15", 'x', opval);
  }
} while (0);
}
if (ANDSI (FLD (f_reglist_hi_ld), 16)) {
do {
  {
    SI opval = GETMEMSI (current_cpu, pc, CPU (h_gr[((UINT) 15)]));
    CPU (h_gr[((UINT) 12)]) = opval;
    written |= (1 << 5);
    TRACE_RESULT (current_cpu, abuf, "gr-12", 'x', opval);
  }
  {
    SI opval = ADDSI (CPU (h_gr[((UINT) 15)]), 4);
    CPU (h_gr[((UINT) 15)]) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "gr-15", 'x', opval);
  }
} while (0);
}
if (ANDSI (FLD (f_reglist_hi_ld), 32)) {
do {
  {
    SI opval = GETMEMSI (current_cpu, pc, CPU (h_gr[((UINT) 15)]));
    CPU (h_gr[((UINT) 13)]) = opval;
    written |= (1 << 6);
    TRACE_RESULT (current_cpu, abuf, "gr-13", 'x', opval);
  }
  {
    SI opval = ADDSI (CPU (h_gr[((UINT) 15)]), 4);
    CPU (h_gr[((UINT) 15)]) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "gr-15", 'x', opval);
  }
} while (0);
}
if (ANDSI (FLD (f_reglist_hi_ld), 64)) {
do {
  {
    SI opval = GETMEMSI (current_cpu, pc, CPU (h_gr[((UINT) 15)]));
    CPU (h_gr[((UINT) 14)]) = opval;
    written |= (1 << 7);
    TRACE_RESULT (current_cpu, abuf, "gr-14", 'x', opval);
  }
  {
    SI opval = ADDSI (CPU (h_gr[((UINT) 15)]), 4);
    CPU (h_gr[((UINT) 15)]) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "gr-15", 'x', opval);
  }
} while (0);
}
if (ANDSI (FLD (f_reglist_hi_ld), 128)) {
  {
    SI opval = GETMEMSI (current_cpu, pc, CPU (h_gr[((UINT) 15)]));
    CPU (h_gr[((UINT) 15)]) = opval;
    written |= (1 << 8);
    TRACE_RESULT (current_cpu, abuf, "gr-15", 'x', opval);
  }
}
} while (0);

  abuf->written = written;
  return vpc;
#undef FLD
}

/* stm0: stm0 ($reglist_low_st) */

SEM_PC
SEM_FN_NAME (fr30bf,stm0) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stm0.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
if (ANDSI (FLD (f_reglist_low_st), 1)) {
do {
  {
    SI opval = SUBSI (CPU (h_gr[((UINT) 15)]), 4);
    CPU (h_gr[((UINT) 15)]) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "gr-15", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((UINT) 7)]);
    SETMEMSI (current_cpu, pc, CPU (h_gr[((UINT) 15)]), opval);
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
} while (0);
}
if (ANDSI (FLD (f_reglist_low_st), 2)) {
do {
  {
    SI opval = SUBSI (CPU (h_gr[((UINT) 15)]), 4);
    CPU (h_gr[((UINT) 15)]) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "gr-15", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((UINT) 6)]);
    SETMEMSI (current_cpu, pc, CPU (h_gr[((UINT) 15)]), opval);
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
} while (0);
}
if (ANDSI (FLD (f_reglist_low_st), 4)) {
do {
  {
    SI opval = SUBSI (CPU (h_gr[((UINT) 15)]), 4);
    CPU (h_gr[((UINT) 15)]) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "gr-15", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((UINT) 5)]);
    SETMEMSI (current_cpu, pc, CPU (h_gr[((UINT) 15)]), opval);
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
} while (0);
}
if (ANDSI (FLD (f_reglist_low_st), 8)) {
do {
  {
    SI opval = SUBSI (CPU (h_gr[((UINT) 15)]), 4);
    CPU (h_gr[((UINT) 15)]) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "gr-15", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((UINT) 4)]);
    SETMEMSI (current_cpu, pc, CPU (h_gr[((UINT) 15)]), opval);
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
} while (0);
}
if (ANDSI (FLD (f_reglist_low_st), 16)) {
do {
  {
    SI opval = SUBSI (CPU (h_gr[((UINT) 15)]), 4);
    CPU (h_gr[((UINT) 15)]) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "gr-15", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((UINT) 3)]);
    SETMEMSI (current_cpu, pc, CPU (h_gr[((UINT) 15)]), opval);
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
} while (0);
}
if (ANDSI (FLD (f_reglist_low_st), 32)) {
do {
  {
    SI opval = SUBSI (CPU (h_gr[((UINT) 15)]), 4);
    CPU (h_gr[((UINT) 15)]) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "gr-15", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((UINT) 2)]);
    SETMEMSI (current_cpu, pc, CPU (h_gr[((UINT) 15)]), opval);
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
} while (0);
}
if (ANDSI (FLD (f_reglist_low_st), 64)) {
do {
  {
    SI opval = SUBSI (CPU (h_gr[((UINT) 15)]), 4);
    CPU (h_gr[((UINT) 15)]) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "gr-15", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((UINT) 1)]);
    SETMEMSI (current_cpu, pc, CPU (h_gr[((UINT) 15)]), opval);
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
} while (0);
}
if (ANDSI (FLD (f_reglist_low_st), 128)) {
do {
  {
    SI opval = SUBSI (CPU (h_gr[((UINT) 15)]), 4);
    CPU (h_gr[((UINT) 15)]) = opval;
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "gr-15", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((UINT) 0)]);
    SETMEMSI (current_cpu, pc, CPU (h_gr[((UINT) 15)]), opval);
    written |= (1 << 11);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
} while (0);
}
} while (0);

  abuf->written = written;
  return vpc;
#undef FLD
}

/* stm1: stm1 ($reglist_hi_st) */

SEM_PC
SEM_FN_NAME (fr30bf,stm1) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_stm1.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
if (ANDSI (FLD (f_reglist_hi_st), 1)) {
do {
  SI tmp_save_r15;
  tmp_save_r15 = CPU (h_gr[((UINT) 15)]);
  {
    SI opval = SUBSI (CPU (h_gr[((UINT) 15)]), 4);
    CPU (h_gr[((UINT) 15)]) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "gr-15", 'x', opval);
  }
  {
    SI opval = tmp_save_r15;
    SETMEMSI (current_cpu, pc, CPU (h_gr[((UINT) 15)]), opval);
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
} while (0);
}
if (ANDSI (FLD (f_reglist_hi_st), 2)) {
do {
  {
    SI opval = SUBSI (CPU (h_gr[((UINT) 15)]), 4);
    CPU (h_gr[((UINT) 15)]) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "gr-15", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((UINT) 14)]);
    SETMEMSI (current_cpu, pc, CPU (h_gr[((UINT) 15)]), opval);
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
} while (0);
}
if (ANDSI (FLD (f_reglist_hi_st), 4)) {
do {
  {
    SI opval = SUBSI (CPU (h_gr[((UINT) 15)]), 4);
    CPU (h_gr[((UINT) 15)]) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "gr-15", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((UINT) 13)]);
    SETMEMSI (current_cpu, pc, CPU (h_gr[((UINT) 15)]), opval);
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
} while (0);
}
if (ANDSI (FLD (f_reglist_hi_st), 8)) {
do {
  {
    SI opval = SUBSI (CPU (h_gr[((UINT) 15)]), 4);
    CPU (h_gr[((UINT) 15)]) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "gr-15", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((UINT) 12)]);
    SETMEMSI (current_cpu, pc, CPU (h_gr[((UINT) 15)]), opval);
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
} while (0);
}
if (ANDSI (FLD (f_reglist_hi_st), 16)) {
do {
  {
    SI opval = SUBSI (CPU (h_gr[((UINT) 15)]), 4);
    CPU (h_gr[((UINT) 15)]) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "gr-15", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((UINT) 11)]);
    SETMEMSI (current_cpu, pc, CPU (h_gr[((UINT) 15)]), opval);
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
} while (0);
}
if (ANDSI (FLD (f_reglist_hi_st), 32)) {
do {
  {
    SI opval = SUBSI (CPU (h_gr[((UINT) 15)]), 4);
    CPU (h_gr[((UINT) 15)]) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "gr-15", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((UINT) 10)]);
    SETMEMSI (current_cpu, pc, CPU (h_gr[((UINT) 15)]), opval);
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
} while (0);
}
if (ANDSI (FLD (f_reglist_hi_st), 64)) {
do {
  {
    SI opval = SUBSI (CPU (h_gr[((UINT) 15)]), 4);
    CPU (h_gr[((UINT) 15)]) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "gr-15", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((UINT) 9)]);
    SETMEMSI (current_cpu, pc, CPU (h_gr[((UINT) 15)]), opval);
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
} while (0);
}
if (ANDSI (FLD (f_reglist_hi_st), 128)) {
do {
  {
    SI opval = SUBSI (CPU (h_gr[((UINT) 15)]), 4);
    CPU (h_gr[((UINT) 15)]) = opval;
    written |= (1 << 9);
    TRACE_RESULT (current_cpu, abuf, "gr-15", 'x', opval);
  }
  {
    SI opval = CPU (h_gr[((UINT) 8)]);
    SETMEMSI (current_cpu, pc, CPU (h_gr[((UINT) 15)]), opval);
    written |= (1 << 10);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
} while (0);
}
} while (0);

  abuf->written = written;
  return vpc;
#undef FLD
}

/* enter: enter $u10 */

SEM_PC
SEM_FN_NAME (fr30bf,enter) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_enter.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  SI tmp_tmp;
  tmp_tmp = SUBSI (CPU (h_gr[((UINT) 15)]), 4);
  {
    SI opval = CPU (h_gr[((UINT) 14)]);
    SETMEMSI (current_cpu, pc, tmp_tmp, opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = tmp_tmp;
    CPU (h_gr[((UINT) 14)]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-14", 'x', opval);
  }
  {
    SI opval = SUBSI (CPU (h_gr[((UINT) 15)]), FLD (f_u10));
    CPU (h_gr[((UINT) 15)]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-15", 'x', opval);
  }
} while (0);

  return vpc;
#undef FLD
}

/* leave: leave */

SEM_PC
SEM_FN_NAME (fr30bf,leave) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_leave.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  {
    SI opval = ADDSI (CPU (h_gr[((UINT) 14)]), 4);
    CPU (h_gr[((UINT) 15)]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-15", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, SUBSI (CPU (h_gr[((UINT) 15)]), 4));
    CPU (h_gr[((UINT) 14)]) = opval;
    TRACE_RESULT (current_cpu, abuf, "gr-14", 'x', opval);
  }
} while (0);

  return vpc;
#undef FLD
}

/* xchb: xchb @$Rj,$Ri */

SEM_PC
SEM_FN_NAME (fr30bf,xchb) (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
#define FLD(f) abuf->fields.fmt_xchb.f
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = abuf->addr;
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 2);

do {
  SI tmp_tmp;
  tmp_tmp = * FLD (i_Ri);
  {
    SI opval = GETMEMUQI (current_cpu, pc, * FLD (i_Rj));
    * FLD (i_Ri) = opval;
    TRACE_RESULT (current_cpu, abuf, "Ri", 'x', opval);
  }
  {
    UQI opval = tmp_tmp;
    SETMEMUQI (current_cpu, pc, * FLD (i_Rj), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
} while (0);

  return vpc;
#undef FLD
}

