/* Simulator model support for m32rx.

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
#include "cpu-sim.h"
#include "cpu-opc.h"

/* The profiling data is recorded here, but is accessed via the profiling
   mechanism.  After all, this is information for profiling.  */

#if WITH_PROFILE_MODEL_P

/* Track function unit usage for an instruction.  */

void
m32rx_model_profile_insn (SIM_CPU *current_cpu, ARGBUF *abuf)
{
  const MODEL *model = CPU_MODEL (current_cpu);
  const INSN_TIMING *timing = MODEL_TIMING (model);
  const CGEN_INSN *insn = abuf->opcode;
  const UNIT *unit = &timing[CGEN_INSN_INDEX (insn)].units[0];
  const UNIT *unit_end = unit + MAX_UNITS;
  PROFILE_DATA *profile = CPU_PROFILE_DATA (current_cpu);

  do
    {
      switch (unit->name)
        {
        case UNIT_M32RX_U_EXEC :
          PROFILE_MODEL_CYCLE_COUNT (profile) += unit->done;
          break;
        }
      ++unit;
    }
  while (unit != unit_end && unit->name != UNIT_NONE);
}

/* Track function unit usage for an instruction.  */

void
m32rx_model_profile_cti_insn (SIM_CPU *current_cpu, ARGBUF *abuf, int taken_p)
{
  const MODEL *model = CPU_MODEL (current_cpu);
  const INSN_TIMING *timing = MODEL_TIMING (model);
  const CGEN_INSN *insn = abuf->opcode;
  const UNIT *unit = &timing[CGEN_INSN_INDEX (insn)].units[0];
  const UNIT *unit_end = unit + MAX_UNITS;
  PROFILE_DATA *profile = CPU_PROFILE_DATA (current_cpu);

  do
    {
      switch (unit->name)
        {
        case UNIT_M32RX_U_EXEC :
          PROFILE_MODEL_CYCLE_COUNT (profile) += unit->done;
          break;
        }
      if (taken_p)
        PROFILE_MODEL_TAKEN_COUNT (profile) += 1;
      else
        PROFILE_MODEL_UNTAKEN_COUNT (profile) += 1;
      ++unit;
    }
  while (unit != unit_end && unit->name != UNIT_NONE);
}

/* We assume UNIT_NONE == 0 because the tables don't always terminate
   entries with it.  */

/* Model timing data for `m32rx'.  */

static const INSN_TIMING m32rx_timing[] = {
  { { (UQI) UNIT_NONE } }, /* illegal insn */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* add */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* add3 */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* add3.a */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* and */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* and3 */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* and3.a */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* or */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* or3 */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* or3.a */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* xor */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* xor3 */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* xor3.a */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* addi */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* addi.a */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* addv */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* addv3 */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* addv3.a */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* addx */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* bc8 */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* bc8.s */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* bc24 */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* bc24.l */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* beq */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* beqz */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* bgez */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* bgtz */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* blez */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* bltz */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* bnez */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* bl8 */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* bl8.s */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* bl24 */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* bl24.l */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* bcl8 */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* bcl8.s */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* bcl24 */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* bcl24.l */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* bnc8 */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* bnc8.s */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* bnc24 */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* bnc24.l */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* bne */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* bra8 */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* bra8.s */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* bra24 */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* bra24.l */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* bncl8 */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* bncl8.s */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* bncl24 */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* bncl24.l */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* cmp */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* cmpi */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* cmpi.a */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* cmpu */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* cmpui */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* cmpui.a */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* cmpeq */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* cmpz */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* div */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* divu */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* rem */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* remu */
  { { (UQI) UNIT_M32RX_U_EXEC, 27, 27 } }, /* divh */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* jc */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* jnc */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* jl */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* jmp */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* ld */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* ld-2 */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* ld-d */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* ld-d2 */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* ldb */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* ldb-2 */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* ldb-d */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* ldb-d2 */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* ldh */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* ldh-2 */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* ldh-d */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* ldh-d2 */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* ldub */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* ldub-2 */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* ldub-d */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* ldub-d2 */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* lduh */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* lduh-2 */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* lduh-d */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* lduh-d2 */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* ld-plus */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* ld24 */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* ld24.a */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* ldi8 */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* ldi8.a */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* ldi8a */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* ldi8a.a */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* ldi16 */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* ldi16a */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* lock */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* machi-a */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* maclo-a */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* mul */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* mulhi-a */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* mullo-a */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* mv */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* mvfachi-a */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* mvfaclo-a */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* mvfacmi-a */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* mvfc */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* mvtachi-a */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* mvtaclo-a */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* mvtc */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* neg */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* nop */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* not */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* rac-d */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* rac-ds */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* rac-dsi */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* rach-d */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* rach-ds */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* rach-dsi */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* rte */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* seth */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* seth.a */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* sll */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* sll3 */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* sll3.a */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* slli */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* slli.a */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* sra */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* sra3 */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* sra3.a */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* srai */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* srai.a */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* srl */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* srl3 */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* srl3.a */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* srli */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* srli.a */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* st */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* st-2 */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* st-d */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* st-d2 */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* stb */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* stb-2 */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* stb-d */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* stb-d2 */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* sth */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* sth-2 */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* sth-d */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* sth-d2 */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* st-plus */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* st-minus */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* sub */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* subv */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* subx */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* trap */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* trap.a */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* unlock */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* push */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* pop */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* satb */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* sath */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* sat */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* pcmpbz */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* sadd */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* macwu1 */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* msblo */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* mulwu1 */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* maclh1 */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* sc */
  { { (UQI) UNIT_M32RX_U_EXEC, 1, 1 } }, /* snc */
};

#endif /* WITH_PROFILE_MODEL_P */

#if WITH_PROFILE_MODEL_P
#define TIMING_DATA(td) td
#else
#define TIMING_DATA(td) 0
#endif

const MODEL m32rx_models[] = {
  { "m32rx", &machs[MACH_M32RX], TIMING_DATA (& m32rx_timing[0]) },
  { 0 }
};

/* The properties of this cpu's implementation.  */

const IMP_PROPERTIES m32rx_imp_properties = {
  sizeof (SIM_CPU)
#if WITH_SCACHE
  , sizeof (SCACHE)
#endif
};

