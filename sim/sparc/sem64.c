/* Simulator instruction semantics for sparc64.

THIS FILE IS MACHINE GENERATED WITH CGEN.

Copyright (C) 1999 Cygnus Solutions, Inc.

This file is part of the Cygnus Simulators.


*/

#define WANT_CPU sparc64
#define WANT_CPU_SPARC64

#include "sim-main.h"
#include "cgen-mem.h"
#include "cgen-ops.h"

#undef GET_ATTR
#define GET_ATTR(cpu, num, attr) CGEN_ATTR_VALUE (NULL, abuf->idesc->attrs, CGEN_INSN_##attr)

/* x-invalid: --invalid-- */

SEM_STATUS
SEM_FN_NAME (sparc64,x_invalid) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 0);
  EXTRACT_IFMT_EMPTY_VARS /* */
  EXTRACT_IFMT_EMPTY_CODE

  {
#if WITH_SCACHE
    /* Update the recorded pc in the cpu state struct.  */
    SET_H_PC (pc);
#endif
    sim_engine_invalid_insn (current_cpu, pc);
    sim_io_error (CPU_STATE (current_cpu), "invalid insn not handled\n");
    /* NOTREACHED */
  }

  SEM_NBRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_VIRTUAL-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* x-after: --after-- */

SEM_STATUS
SEM_FN_NAME (sparc64,x_after) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 0);
  EXTRACT_IFMT_EMPTY_VARS /* */
  EXTRACT_IFMT_EMPTY_CODE

  {
#if WITH_SCACHE_PBB_SPARC64
    sparc64_pbb_after (current_cpu, sem_arg);
#endif
  }

  SEM_NBRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_PBB-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_VIRTUAL-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* x-before: --before-- */

SEM_STATUS
SEM_FN_NAME (sparc64,x_before) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 0);
  EXTRACT_IFMT_EMPTY_VARS /* */
  EXTRACT_IFMT_EMPTY_CODE

  {
#if WITH_SCACHE_PBB_SPARC64
    sparc64_pbb_before (current_cpu, sem_arg);
#endif
  }

  SEM_NBRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_PBB-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_VIRTUAL-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* x-cti-chain: --cti-chain-- */

SEM_STATUS
SEM_FN_NAME (sparc64,x_cti_chain) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 0);
  EXTRACT_IFMT_EMPTY_VARS /* */
  EXTRACT_IFMT_EMPTY_CODE

  {
#if WITH_SCACHE_PBB_SPARC64
#ifdef DEFINE_SWITCH
    vpc = sparc64_pbb_cti_chain (current_cpu, sem_arg,
			       pbb_br_npc_ptr, pbb_br_npc);
    BREAK (sem);
#else
    /* FIXME: Allow provision of explicit ifmt spec in insn spec.  */
    vpc = sparc64_pbb_cti_chain (current_cpu, sem_arg,
			       CPU_PBB_BR_NPC_PTR (current_cpu),
			       CPU_PBB_BR_NPC (current_cpu));
#endif
#endif
  }

  SEM_NBRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_PBB-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_VIRTUAL-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* x-chain: --chain-- */

SEM_STATUS
SEM_FN_NAME (sparc64,x_chain) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 0);
  EXTRACT_IFMT_EMPTY_VARS /* */
  EXTRACT_IFMT_EMPTY_CODE

  {
#if WITH_SCACHE_PBB_SPARC64
    vpc = sparc64_pbb_chain (current_cpu, sem_arg);
#ifdef DEFINE_SWITCH
    BREAK (sem);
#endif
#endif
  }

  SEM_NBRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_PBB-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_VIRTUAL-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* x-begin: --begin-- */

SEM_STATUS
SEM_FN_NAME (sparc64,x_begin) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 0);
  EXTRACT_IFMT_EMPTY_VARS /* */
  EXTRACT_IFMT_EMPTY_CODE

  {
#if WITH_SCACHE_PBB_SPARC64
#ifdef DEFINE_SWITCH
    /* In the switch case FAST_P is a constant, allowing several optimizations
       in any called inline functions.  */
    vpc = sparc64_pbb_begin (current_cpu, FAST_P);
#else
    vpc = sparc64_pbb_begin (current_cpu, STATE_RUN_FAST_P (CPU_STATE (current_cpu)));
#endif
#endif
  }

  SEM_NBRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_PBB-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_VIRTUAL-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* beqz: beqz$a$p $rs1,$disp16 */

SEM_STATUS
SEM_FN_NAME (sparc64,beqz) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_BEQZ_VARS /* f-disp16 f-rs1 f-p f-op2 f-fmt2-rcond f-bpr-res28-1 f-a f-op */
      IADDR i_disp16;
  EXTRACT_IFMT_BEQZ_CODE
  i_disp16 = f_disp16;

do {
if (EQSI (GET_H_GR (f_rs1), 0)) {
  {
    USI opval = i_disp16;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} else {
SEM_ANNUL_INSN (current_cpu, pc, f_a);
}
} while (0);

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* bgez: bgez$a$p $rs1,$disp16 */

SEM_STATUS
SEM_FN_NAME (sparc64,bgez) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_BEQZ_VARS /* f-disp16 f-rs1 f-p f-op2 f-fmt2-rcond f-bpr-res28-1 f-a f-op */
      IADDR i_disp16;
  EXTRACT_IFMT_BEQZ_CODE
  i_disp16 = f_disp16;

do {
if (GESI (GET_H_GR (f_rs1), 0)) {
  {
    USI opval = i_disp16;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} else {
SEM_ANNUL_INSN (current_cpu, pc, f_a);
}
} while (0);

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* bgtz: bgtz$a$p $rs1,$disp16 */

SEM_STATUS
SEM_FN_NAME (sparc64,bgtz) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_BEQZ_VARS /* f-disp16 f-rs1 f-p f-op2 f-fmt2-rcond f-bpr-res28-1 f-a f-op */
      IADDR i_disp16;
  EXTRACT_IFMT_BEQZ_CODE
  i_disp16 = f_disp16;

do {
if (GTSI (GET_H_GR (f_rs1), 0)) {
  {
    USI opval = i_disp16;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} else {
SEM_ANNUL_INSN (current_cpu, pc, f_a);
}
} while (0);

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* blez: blez$a$p $rs1,$disp16 */

SEM_STATUS
SEM_FN_NAME (sparc64,blez) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_BEQZ_VARS /* f-disp16 f-rs1 f-p f-op2 f-fmt2-rcond f-bpr-res28-1 f-a f-op */
      IADDR i_disp16;
  EXTRACT_IFMT_BEQZ_CODE
  i_disp16 = f_disp16;

do {
if (LESI (GET_H_GR (f_rs1), 0)) {
  {
    USI opval = i_disp16;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} else {
SEM_ANNUL_INSN (current_cpu, pc, f_a);
}
} while (0);

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* bltz: bltz$a$p $rs1,$disp16 */

SEM_STATUS
SEM_FN_NAME (sparc64,bltz) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_BEQZ_VARS /* f-disp16 f-rs1 f-p f-op2 f-fmt2-rcond f-bpr-res28-1 f-a f-op */
      IADDR i_disp16;
  EXTRACT_IFMT_BEQZ_CODE
  i_disp16 = f_disp16;

do {
if (LTSI (GET_H_GR (f_rs1), 0)) {
  {
    USI opval = i_disp16;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} else {
SEM_ANNUL_INSN (current_cpu, pc, f_a);
}
} while (0);

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* bnez: bnez$a$p $rs1,$disp16 */

SEM_STATUS
SEM_FN_NAME (sparc64,bnez) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_BEQZ_VARS /* f-disp16 f-rs1 f-p f-op2 f-fmt2-rcond f-bpr-res28-1 f-a f-op */
      IADDR i_disp16;
  EXTRACT_IFMT_BEQZ_CODE
  i_disp16 = f_disp16;

do {
if (NESI (GET_H_GR (f_rs1), 0)) {
  {
    USI opval = i_disp16;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} else {
SEM_ANNUL_INSN (current_cpu, pc, f_a);
}
} while (0);

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* bpcc-ba: ba$a$p %xcc,$disp19 */

SEM_STATUS
SEM_FN_NAME (sparc64,bpcc_ba) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_BPCC_BA_VARS /* f-disp19 f-p f-fmt2-cc0 f-fmt2-cc1 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BPCC_BA_CODE
  i_disp22 = f_disp22;

do {
do {
  {
    USI opval = i_disp22;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
SEM_ANNUL_INSN (current_cpu, pc, f_a);
} while (0);
} while (0);

  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_UNCOND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* bpcc-bn: bn$a$p %xcc,$disp19 */

SEM_STATUS
SEM_FN_NAME (sparc64,bpcc_bn) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_BPCC_BA_VARS /* f-disp19 f-p f-fmt2-cc0 f-fmt2-cc1 f-op2 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_BPCC_BA_CODE

do {
do {
do { } while (0); /*nop*/
SEM_ANNUL_INSN (current_cpu, pc, f_a);
} while (0);
} while (0);

  SEM_NBRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* bpcc-bne: bne$a$p %xcc,$disp19 */

SEM_STATUS
SEM_FN_NAME (sparc64,bpcc_bne) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_BPCC_BA_VARS /* f-disp19 f-p f-fmt2-cc0 f-fmt2-cc1 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BPCC_BA_CODE
  i_disp22 = f_disp22;

do {
if (NOTBI (CPU (h_xcc_z))) {
  {
    USI opval = i_disp22;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} else {
SEM_ANNUL_INSN (current_cpu, pc, f_a);
}
} while (0);

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* bpcc-be: be$a$p %xcc,$disp19 */

SEM_STATUS
SEM_FN_NAME (sparc64,bpcc_be) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_BPCC_BA_VARS /* f-disp19 f-p f-fmt2-cc0 f-fmt2-cc1 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BPCC_BA_CODE
  i_disp22 = f_disp22;

do {
if (CPU (h_xcc_z)) {
  {
    USI opval = i_disp22;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} else {
SEM_ANNUL_INSN (current_cpu, pc, f_a);
}
} while (0);

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* bpcc-bg: bg$a$p %xcc,$disp19 */

SEM_STATUS
SEM_FN_NAME (sparc64,bpcc_bg) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_BPCC_BA_VARS /* f-disp19 f-p f-fmt2-cc0 f-fmt2-cc1 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BPCC_BA_CODE
  i_disp22 = f_disp22;

do {
if (NOTBI (ORBI (CPU (h_xcc_z), XORBI (CPU (h_xcc_n), CPU (h_xcc_v))))) {
  {
    USI opval = i_disp22;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 5);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} else {
SEM_ANNUL_INSN (current_cpu, pc, f_a);
}
} while (0);

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* bpcc-ble: ble$a$p %xcc,$disp19 */

SEM_STATUS
SEM_FN_NAME (sparc64,bpcc_ble) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_BPCC_BA_VARS /* f-disp19 f-p f-fmt2-cc0 f-fmt2-cc1 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BPCC_BA_CODE
  i_disp22 = f_disp22;

do {
if (ORBI (CPU (h_xcc_z), XORBI (CPU (h_xcc_n), CPU (h_xcc_v)))) {
  {
    USI opval = i_disp22;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 5);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} else {
SEM_ANNUL_INSN (current_cpu, pc, f_a);
}
} while (0);

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* bpcc-bge: bge$a$p %xcc,$disp19 */

SEM_STATUS
SEM_FN_NAME (sparc64,bpcc_bge) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_BPCC_BA_VARS /* f-disp19 f-p f-fmt2-cc0 f-fmt2-cc1 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BPCC_BA_CODE
  i_disp22 = f_disp22;

do {
if (NOTBI (XORBI (CPU (h_xcc_n), CPU (h_xcc_v)))) {
  {
    USI opval = i_disp22;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 4);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} else {
SEM_ANNUL_INSN (current_cpu, pc, f_a);
}
} while (0);

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* bpcc-bl: bl$a$p %xcc,$disp19 */

SEM_STATUS
SEM_FN_NAME (sparc64,bpcc_bl) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_BPCC_BA_VARS /* f-disp19 f-p f-fmt2-cc0 f-fmt2-cc1 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BPCC_BA_CODE
  i_disp22 = f_disp22;

do {
if (XORBI (CPU (h_xcc_n), CPU (h_xcc_v))) {
  {
    USI opval = i_disp22;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 4);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} else {
SEM_ANNUL_INSN (current_cpu, pc, f_a);
}
} while (0);

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* bpcc-bgu: bgu$a$p %xcc,$disp19 */

SEM_STATUS
SEM_FN_NAME (sparc64,bpcc_bgu) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_BPCC_BA_VARS /* f-disp19 f-p f-fmt2-cc0 f-fmt2-cc1 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BPCC_BA_CODE
  i_disp22 = f_disp22;

do {
if (NOTBI (ORBI (CPU (h_xcc_c), CPU (h_xcc_z)))) {
  {
    USI opval = i_disp22;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 4);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} else {
SEM_ANNUL_INSN (current_cpu, pc, f_a);
}
} while (0);

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* bpcc-bleu: bleu$a$p %xcc,$disp19 */

SEM_STATUS
SEM_FN_NAME (sparc64,bpcc_bleu) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_BPCC_BA_VARS /* f-disp19 f-p f-fmt2-cc0 f-fmt2-cc1 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BPCC_BA_CODE
  i_disp22 = f_disp22;

do {
if (ORBI (CPU (h_xcc_c), CPU (h_xcc_z))) {
  {
    USI opval = i_disp22;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 4);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} else {
SEM_ANNUL_INSN (current_cpu, pc, f_a);
}
} while (0);

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* bpcc-bcc: bcc$a$p %xcc,$disp19 */

SEM_STATUS
SEM_FN_NAME (sparc64,bpcc_bcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_BPCC_BA_VARS /* f-disp19 f-p f-fmt2-cc0 f-fmt2-cc1 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BPCC_BA_CODE
  i_disp22 = f_disp22;

do {
if (NOTBI (CPU (h_xcc_c))) {
  {
    USI opval = i_disp22;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} else {
SEM_ANNUL_INSN (current_cpu, pc, f_a);
}
} while (0);

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* bpcc-bcs: bcs$a$p %xcc,$disp19 */

SEM_STATUS
SEM_FN_NAME (sparc64,bpcc_bcs) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_BPCC_BA_VARS /* f-disp19 f-p f-fmt2-cc0 f-fmt2-cc1 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BPCC_BA_CODE
  i_disp22 = f_disp22;

do {
if (CPU (h_xcc_c)) {
  {
    USI opval = i_disp22;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} else {
SEM_ANNUL_INSN (current_cpu, pc, f_a);
}
} while (0);

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* bpcc-bpos: bpos$a$p %xcc,$disp19 */

SEM_STATUS
SEM_FN_NAME (sparc64,bpcc_bpos) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_BPCC_BA_VARS /* f-disp19 f-p f-fmt2-cc0 f-fmt2-cc1 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BPCC_BA_CODE
  i_disp22 = f_disp22;

do {
if (NOTBI (CPU (h_xcc_n))) {
  {
    USI opval = i_disp22;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} else {
SEM_ANNUL_INSN (current_cpu, pc, f_a);
}
} while (0);

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* bpcc-bneg: bneg$a$p %xcc,$disp19 */

SEM_STATUS
SEM_FN_NAME (sparc64,bpcc_bneg) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_BPCC_BA_VARS /* f-disp19 f-p f-fmt2-cc0 f-fmt2-cc1 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BPCC_BA_CODE
  i_disp22 = f_disp22;

do {
if (CPU (h_xcc_n)) {
  {
    USI opval = i_disp22;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} else {
SEM_ANNUL_INSN (current_cpu, pc, f_a);
}
} while (0);

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* bpcc-bvc: bvc$a$p %xcc,$disp19 */

SEM_STATUS
SEM_FN_NAME (sparc64,bpcc_bvc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_BPCC_BA_VARS /* f-disp19 f-p f-fmt2-cc0 f-fmt2-cc1 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BPCC_BA_CODE
  i_disp22 = f_disp22;

do {
if (NOTBI (CPU (h_xcc_v))) {
  {
    USI opval = i_disp22;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} else {
SEM_ANNUL_INSN (current_cpu, pc, f_a);
}
} while (0);

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* bpcc-bvs: bvs$a$p %xcc,$disp19 */

SEM_STATUS
SEM_FN_NAME (sparc64,bpcc_bvs) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_BPCC_BA_VARS /* f-disp19 f-p f-fmt2-cc0 f-fmt2-cc1 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BPCC_BA_CODE
  i_disp22 = f_disp22;

do {
if (CPU (h_xcc_v)) {
  {
    USI opval = i_disp22;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} else {
SEM_ANNUL_INSN (current_cpu, pc, f_a);
}
} while (0);

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* done: done */

SEM_STATUS
SEM_FN_NAME (sparc64,done) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_DONE_VARS /* f-res-18-19 f-op3 f-fcn f-op */
  EXTRACT_IFMT_DONE_CODE

sparc64_done (current_cpu, pc);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* retry: done */

SEM_STATUS
SEM_FN_NAME (sparc64,retry) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_DONE_VARS /* f-res-18-19 f-op3 f-fcn f-op */
  EXTRACT_IFMT_DONE_CODE

sparc64_retry (current_cpu, pc);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* flush: flush */

SEM_STATUS
SEM_FN_NAME (sparc64,flush) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_FLUSH_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_FLUSH_CODE

sparc64_flush (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2)));

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* flush-imm: flush */

SEM_STATUS
SEM_FN_NAME (sparc64,flush_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_FLUSH_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_FLUSH_IMM_CODE

sparc64_flush (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), f_simm13));

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* flushw: flushw */

SEM_STATUS
SEM_FN_NAME (sparc64,flushw) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_FLUSHW_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_FLUSHW_CODE

sparc64_flushw (current_cpu, pc);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* impdep1: impdep1 $impdep5,$impdep19 */

SEM_STATUS
SEM_FN_NAME (sparc64,impdep1) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_IMPDEP1_VARS /* f-impdep19 f-op3 f-impdep5 f-op */
  EXTRACT_IFMT_IMPDEP1_CODE

sparc64_impdep1 (current_cpu, pc, f_impdep5, f_impdep19);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* impdep2: impdep2 $impdep5,$impdep19 */

SEM_STATUS
SEM_FN_NAME (sparc64,impdep2) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_IMPDEP1_VARS /* f-impdep19 f-op3 f-impdep5 f-op */
  EXTRACT_IFMT_IMPDEP1_CODE

sparc64_impdep2 (current_cpu, pc, f_impdep5, f_impdep19);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* membar: member $membarmask */

SEM_STATUS
SEM_FN_NAME (sparc64,membar) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MEMBAR_VARS /* f-membarmask f-membar-res12-6 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_MEMBAR_CODE

sparc64_membar (current_cpu, pc, f_membarmask);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* mova-icc-icc: mova-icc %icc,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,mova_icc_icc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE

  {
    SI opval = GET_H_GR (f_rs2);
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* mova-imm-icc-icc: mova-imm-icc %icc,$simm11,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,mova_imm_icc_icc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE

  {
    SI opval = f_simm11;
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* mova-xcc-xcc: mova-xcc %xcc,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,mova_xcc_xcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE

  {
    SI opval = GET_H_GR (f_rs2);
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* mova-imm-xcc-xcc: mova-imm-xcc %xcc,$simm11,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,mova_imm_xcc_xcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE

  {
    SI opval = f_simm11;
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movn-icc-icc: movn-icc %icc,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movn_icc_icc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE

do { } while (0); /*nop*/

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movn-imm-icc-icc: movn-imm-icc %icc,$simm11,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movn_imm_icc_icc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE

do { } while (0); /*nop*/

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movn-xcc-xcc: movn-xcc %xcc,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movn_xcc_xcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE

do { } while (0); /*nop*/

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movn-imm-xcc-xcc: movn-imm-xcc %xcc,$simm11,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movn_imm_xcc_xcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE

do { } while (0); /*nop*/

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movne-icc-icc: movne-icc %icc,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movne_icc_icc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE

if (NOTBI (CPU (h_icc_z))) {
  {
    SI opval = GET_H_GR (f_rs2);
    SET_H_GR (f_rd, opval);
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movne-imm-icc-icc: movne-imm-icc %icc,$simm11,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movne_imm_icc_icc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE

if (NOTBI (CPU (h_icc_z))) {
  {
    SI opval = f_simm11;
    SET_H_GR (f_rd, opval);
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movne-xcc-xcc: movne-xcc %xcc,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movne_xcc_xcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE

if (NOTBI (CPU (h_xcc_z))) {
  {
    SI opval = GET_H_GR (f_rs2);
    SET_H_GR (f_rd, opval);
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movne-imm-xcc-xcc: movne-imm-xcc %xcc,$simm11,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movne_imm_xcc_xcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE

if (NOTBI (CPU (h_xcc_z))) {
  {
    SI opval = f_simm11;
    SET_H_GR (f_rd, opval);
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* move-icc-icc: move-icc %icc,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,move_icc_icc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE

if (CPU (h_icc_z)) {
  {
    SI opval = GET_H_GR (f_rs2);
    SET_H_GR (f_rd, opval);
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* move-imm-icc-icc: move-imm-icc %icc,$simm11,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,move_imm_icc_icc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE

if (CPU (h_icc_z)) {
  {
    SI opval = f_simm11;
    SET_H_GR (f_rd, opval);
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* move-xcc-xcc: move-xcc %xcc,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,move_xcc_xcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE

if (CPU (h_xcc_z)) {
  {
    SI opval = GET_H_GR (f_rs2);
    SET_H_GR (f_rd, opval);
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* move-imm-xcc-xcc: move-imm-xcc %xcc,$simm11,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,move_imm_xcc_xcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE

if (CPU (h_xcc_z)) {
  {
    SI opval = f_simm11;
    SET_H_GR (f_rd, opval);
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movg-icc-icc: movg-icc %icc,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movg_icc_icc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE

if (NOTBI (ORBI (CPU (h_icc_z), XORBI (CPU (h_icc_n), CPU (h_icc_v))))) {
  {
    SI opval = GET_H_GR (f_rs2);
    SET_H_GR (f_rd, opval);
    written |= (1 << 4);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movg-imm-icc-icc: movg-imm-icc %icc,$simm11,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movg_imm_icc_icc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE

if (NOTBI (ORBI (CPU (h_icc_z), XORBI (CPU (h_icc_n), CPU (h_icc_v))))) {
  {
    SI opval = f_simm11;
    SET_H_GR (f_rd, opval);
    written |= (1 << 4);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movg-xcc-xcc: movg-xcc %xcc,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movg_xcc_xcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE

if (NOTBI (ORBI (CPU (h_xcc_z), XORBI (CPU (h_xcc_n), CPU (h_xcc_v))))) {
  {
    SI opval = GET_H_GR (f_rs2);
    SET_H_GR (f_rd, opval);
    written |= (1 << 4);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movg-imm-xcc-xcc: movg-imm-xcc %xcc,$simm11,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movg_imm_xcc_xcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE

if (NOTBI (ORBI (CPU (h_xcc_z), XORBI (CPU (h_xcc_n), CPU (h_xcc_v))))) {
  {
    SI opval = f_simm11;
    SET_H_GR (f_rd, opval);
    written |= (1 << 4);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movle-icc-icc: movle-icc %icc,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movle_icc_icc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE

if (ORBI (CPU (h_icc_z), XORBI (CPU (h_icc_n), CPU (h_icc_v)))) {
  {
    SI opval = GET_H_GR (f_rs2);
    SET_H_GR (f_rd, opval);
    written |= (1 << 4);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movle-imm-icc-icc: movle-imm-icc %icc,$simm11,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movle_imm_icc_icc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE

if (ORBI (CPU (h_icc_z), XORBI (CPU (h_icc_n), CPU (h_icc_v)))) {
  {
    SI opval = f_simm11;
    SET_H_GR (f_rd, opval);
    written |= (1 << 4);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movle-xcc-xcc: movle-xcc %xcc,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movle_xcc_xcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE

if (ORBI (CPU (h_xcc_z), XORBI (CPU (h_xcc_n), CPU (h_xcc_v)))) {
  {
    SI opval = GET_H_GR (f_rs2);
    SET_H_GR (f_rd, opval);
    written |= (1 << 4);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movle-imm-xcc-xcc: movle-imm-xcc %xcc,$simm11,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movle_imm_xcc_xcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE

if (ORBI (CPU (h_xcc_z), XORBI (CPU (h_xcc_n), CPU (h_xcc_v)))) {
  {
    SI opval = f_simm11;
    SET_H_GR (f_rd, opval);
    written |= (1 << 4);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movge-icc-icc: movge-icc %icc,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movge_icc_icc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE

if (NOTBI (XORBI (CPU (h_icc_n), CPU (h_icc_v)))) {
  {
    SI opval = GET_H_GR (f_rs2);
    SET_H_GR (f_rd, opval);
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movge-imm-icc-icc: movge-imm-icc %icc,$simm11,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movge_imm_icc_icc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE

if (NOTBI (XORBI (CPU (h_icc_n), CPU (h_icc_v)))) {
  {
    SI opval = f_simm11;
    SET_H_GR (f_rd, opval);
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movge-xcc-xcc: movge-xcc %xcc,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movge_xcc_xcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE

if (NOTBI (XORBI (CPU (h_xcc_n), CPU (h_xcc_v)))) {
  {
    SI opval = GET_H_GR (f_rs2);
    SET_H_GR (f_rd, opval);
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movge-imm-xcc-xcc: movge-imm-xcc %xcc,$simm11,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movge_imm_xcc_xcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE

if (NOTBI (XORBI (CPU (h_xcc_n), CPU (h_xcc_v)))) {
  {
    SI opval = f_simm11;
    SET_H_GR (f_rd, opval);
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movl-icc-icc: movl-icc %icc,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movl_icc_icc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE

if (XORBI (CPU (h_icc_n), CPU (h_icc_v))) {
  {
    SI opval = GET_H_GR (f_rs2);
    SET_H_GR (f_rd, opval);
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movl-imm-icc-icc: movl-imm-icc %icc,$simm11,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movl_imm_icc_icc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE

if (XORBI (CPU (h_icc_n), CPU (h_icc_v))) {
  {
    SI opval = f_simm11;
    SET_H_GR (f_rd, opval);
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movl-xcc-xcc: movl-xcc %xcc,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movl_xcc_xcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE

if (XORBI (CPU (h_xcc_n), CPU (h_xcc_v))) {
  {
    SI opval = GET_H_GR (f_rs2);
    SET_H_GR (f_rd, opval);
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movl-imm-xcc-xcc: movl-imm-xcc %xcc,$simm11,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movl_imm_xcc_xcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE

if (XORBI (CPU (h_xcc_n), CPU (h_xcc_v))) {
  {
    SI opval = f_simm11;
    SET_H_GR (f_rd, opval);
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movgu-icc-icc: movgu-icc %icc,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movgu_icc_icc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE

if (NOTBI (ORBI (CPU (h_icc_c), CPU (h_icc_z)))) {
  {
    SI opval = GET_H_GR (f_rs2);
    SET_H_GR (f_rd, opval);
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movgu-imm-icc-icc: movgu-imm-icc %icc,$simm11,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movgu_imm_icc_icc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE

if (NOTBI (ORBI (CPU (h_icc_c), CPU (h_icc_z)))) {
  {
    SI opval = f_simm11;
    SET_H_GR (f_rd, opval);
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movgu-xcc-xcc: movgu-xcc %xcc,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movgu_xcc_xcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE

if (NOTBI (ORBI (CPU (h_xcc_c), CPU (h_xcc_z)))) {
  {
    SI opval = GET_H_GR (f_rs2);
    SET_H_GR (f_rd, opval);
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movgu-imm-xcc-xcc: movgu-imm-xcc %xcc,$simm11,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movgu_imm_xcc_xcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE

if (NOTBI (ORBI (CPU (h_xcc_c), CPU (h_xcc_z)))) {
  {
    SI opval = f_simm11;
    SET_H_GR (f_rd, opval);
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movleu-icc-icc: movleu-icc %icc,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movleu_icc_icc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE

if (ORBI (CPU (h_icc_c), CPU (h_icc_z))) {
  {
    SI opval = GET_H_GR (f_rs2);
    SET_H_GR (f_rd, opval);
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movleu-imm-icc-icc: movleu-imm-icc %icc,$simm11,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movleu_imm_icc_icc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE

if (ORBI (CPU (h_icc_c), CPU (h_icc_z))) {
  {
    SI opval = f_simm11;
    SET_H_GR (f_rd, opval);
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movleu-xcc-xcc: movleu-xcc %xcc,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movleu_xcc_xcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE

if (ORBI (CPU (h_xcc_c), CPU (h_xcc_z))) {
  {
    SI opval = GET_H_GR (f_rs2);
    SET_H_GR (f_rd, opval);
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movleu-imm-xcc-xcc: movleu-imm-xcc %xcc,$simm11,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movleu_imm_xcc_xcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE

if (ORBI (CPU (h_xcc_c), CPU (h_xcc_z))) {
  {
    SI opval = f_simm11;
    SET_H_GR (f_rd, opval);
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movcc-icc-icc: movcc-icc %icc,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movcc_icc_icc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE

if (NOTBI (CPU (h_icc_c))) {
  {
    SI opval = GET_H_GR (f_rs2);
    SET_H_GR (f_rd, opval);
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movcc-imm-icc-icc: movcc-imm-icc %icc,$simm11,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movcc_imm_icc_icc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE

if (NOTBI (CPU (h_icc_c))) {
  {
    SI opval = f_simm11;
    SET_H_GR (f_rd, opval);
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movcc-xcc-xcc: movcc-xcc %xcc,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movcc_xcc_xcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE

if (NOTBI (CPU (h_xcc_c))) {
  {
    SI opval = GET_H_GR (f_rs2);
    SET_H_GR (f_rd, opval);
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movcc-imm-xcc-xcc: movcc-imm-xcc %xcc,$simm11,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movcc_imm_xcc_xcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE

if (NOTBI (CPU (h_xcc_c))) {
  {
    SI opval = f_simm11;
    SET_H_GR (f_rd, opval);
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movcs-icc-icc: movcs-icc %icc,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movcs_icc_icc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE

if (CPU (h_icc_c)) {
  {
    SI opval = GET_H_GR (f_rs2);
    SET_H_GR (f_rd, opval);
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movcs-imm-icc-icc: movcs-imm-icc %icc,$simm11,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movcs_imm_icc_icc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE

if (CPU (h_icc_c)) {
  {
    SI opval = f_simm11;
    SET_H_GR (f_rd, opval);
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movcs-xcc-xcc: movcs-xcc %xcc,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movcs_xcc_xcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE

if (CPU (h_xcc_c)) {
  {
    SI opval = GET_H_GR (f_rs2);
    SET_H_GR (f_rd, opval);
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movcs-imm-xcc-xcc: movcs-imm-xcc %xcc,$simm11,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movcs_imm_xcc_xcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE

if (CPU (h_xcc_c)) {
  {
    SI opval = f_simm11;
    SET_H_GR (f_rd, opval);
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movpos-icc-icc: movpos-icc %icc,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movpos_icc_icc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE

if (NOTBI (CPU (h_icc_n))) {
  {
    SI opval = GET_H_GR (f_rs2);
    SET_H_GR (f_rd, opval);
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movpos-imm-icc-icc: movpos-imm-icc %icc,$simm11,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movpos_imm_icc_icc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE

if (NOTBI (CPU (h_icc_n))) {
  {
    SI opval = f_simm11;
    SET_H_GR (f_rd, opval);
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movpos-xcc-xcc: movpos-xcc %xcc,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movpos_xcc_xcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE

if (NOTBI (CPU (h_xcc_n))) {
  {
    SI opval = GET_H_GR (f_rs2);
    SET_H_GR (f_rd, opval);
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movpos-imm-xcc-xcc: movpos-imm-xcc %xcc,$simm11,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movpos_imm_xcc_xcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE

if (NOTBI (CPU (h_xcc_n))) {
  {
    SI opval = f_simm11;
    SET_H_GR (f_rd, opval);
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movneg-icc-icc: movneg-icc %icc,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movneg_icc_icc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE

if (CPU (h_icc_n)) {
  {
    SI opval = GET_H_GR (f_rs2);
    SET_H_GR (f_rd, opval);
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movneg-imm-icc-icc: movneg-imm-icc %icc,$simm11,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movneg_imm_icc_icc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE

if (CPU (h_icc_n)) {
  {
    SI opval = f_simm11;
    SET_H_GR (f_rd, opval);
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movneg-xcc-xcc: movneg-xcc %xcc,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movneg_xcc_xcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE

if (CPU (h_xcc_n)) {
  {
    SI opval = GET_H_GR (f_rs2);
    SET_H_GR (f_rd, opval);
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movneg-imm-xcc-xcc: movneg-imm-xcc %xcc,$simm11,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movneg_imm_xcc_xcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE

if (CPU (h_xcc_n)) {
  {
    SI opval = f_simm11;
    SET_H_GR (f_rd, opval);
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movvc-icc-icc: movvc-icc %icc,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movvc_icc_icc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE

if (NOTBI (CPU (h_icc_v))) {
  {
    SI opval = GET_H_GR (f_rs2);
    SET_H_GR (f_rd, opval);
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movvc-imm-icc-icc: movvc-imm-icc %icc,$simm11,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movvc_imm_icc_icc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE

if (NOTBI (CPU (h_icc_v))) {
  {
    SI opval = f_simm11;
    SET_H_GR (f_rd, opval);
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movvc-xcc-xcc: movvc-xcc %xcc,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movvc_xcc_xcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE

if (NOTBI (CPU (h_xcc_v))) {
  {
    SI opval = GET_H_GR (f_rs2);
    SET_H_GR (f_rd, opval);
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movvc-imm-xcc-xcc: movvc-imm-xcc %xcc,$simm11,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movvc_imm_xcc_xcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE

if (NOTBI (CPU (h_xcc_v))) {
  {
    SI opval = f_simm11;
    SET_H_GR (f_rd, opval);
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movvs-icc-icc: movvs-icc %icc,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movvs_icc_icc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE

if (CPU (h_icc_v)) {
  {
    SI opval = GET_H_GR (f_rs2);
    SET_H_GR (f_rd, opval);
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movvs-imm-icc-icc: movvs-imm-icc %icc,$simm11,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movvs_imm_icc_icc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE

if (CPU (h_icc_v)) {
  {
    SI opval = f_simm11;
    SET_H_GR (f_rd, opval);
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movvs-xcc-xcc: movvs-xcc %xcc,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movvs_xcc_xcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE

if (CPU (h_xcc_v)) {
  {
    SI opval = GET_H_GR (f_rs2);
    SET_H_GR (f_rd, opval);
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* movvs-imm-xcc-xcc: movvs-imm-xcc %xcc,$simm11,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,movvs_imm_xcc_xcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE

if (CPU (h_xcc_v)) {
  {
    SI opval = f_simm11;
    SET_H_GR (f_rd, opval);
    written |= (1 << 2);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
}

  abuf->written = written;
  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* ldsb-reg+reg: ldsb [$rs1+$rs2],$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,ldsb_reg_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE

  {
    QI opval = GETMEMQI (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2)));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* ldsb-reg+imm: ldsb [$rs1+$simm13],$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,ldsb_reg_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE

  {
    QI opval = GETMEMQI (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), f_simm13));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* ldsb-reg+reg/asi: ldsb [$rs1+$rs2]$asi,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,ldsb_reg_reg_asi) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_ASI_CODE

  {
    QI opval = GETMEMQI (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2)));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* ldub-reg+reg: ldub [$rs1+$rs2],$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,ldub_reg_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE

  {
    QI opval = GETMEMQI (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2)));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* ldub-reg+imm: ldub [$rs1+$simm13],$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,ldub_reg_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE

  {
    QI opval = GETMEMQI (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), f_simm13));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* ldub-reg+reg/asi: ldub [$rs1+$rs2]$asi,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,ldub_reg_reg_asi) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_ASI_CODE

  {
    QI opval = GETMEMQI (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2)));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* ldsh-reg+reg: ldsh [$rs1+$rs2],$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,ldsh_reg_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE

  {
    HI opval = GETMEMHI (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2)));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* ldsh-reg+imm: ldsh [$rs1+$simm13],$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,ldsh_reg_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE

  {
    HI opval = GETMEMHI (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), f_simm13));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* ldsh-reg+reg/asi: ldsh [$rs1+$rs2]$asi,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,ldsh_reg_reg_asi) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_ASI_CODE

  {
    HI opval = GETMEMHI (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2)));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* lduh-reg+reg: lduh [$rs1+$rs2],$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,lduh_reg_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE

  {
    HI opval = GETMEMHI (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2)));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* lduh-reg+imm: lduh [$rs1+$simm13],$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,lduh_reg_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE

  {
    HI opval = GETMEMHI (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), f_simm13));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* lduh-reg+reg/asi: lduh [$rs1+$rs2]$asi,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,lduh_reg_reg_asi) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_ASI_CODE

  {
    HI opval = GETMEMHI (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2)));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* ldsw-reg+reg: ldsw [$rs1+$rs2],$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,ldsw_reg_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE

  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2)));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* ldsw-reg+imm: ldsw [$rs1+$simm13],$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,ldsw_reg_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE

  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), f_simm13));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* ldsw-reg+reg/asi: ldsw [$rs1+$rs2]$asi,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,ldsw_reg_reg_asi) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_ASI_CODE

  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2)));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* lduw-reg+reg: lduw [$rs1+$rs2],$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,lduw_reg_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE

  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2)));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* lduw-reg+imm: lduw [$rs1+$simm13],$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,lduw_reg_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE

  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), f_simm13));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* lduw-reg+reg/asi: lduw [$rs1+$rs2]$asi,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,lduw_reg_reg_asi) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_ASI_CODE

  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2)));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* ldx-reg+reg: ldx [$rs1+$rs2],$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,ldx_reg_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE

  {
    DI opval = GETMEMDI (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2)));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'D', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* ldx-reg+imm: ldx [$rs1+$simm13],$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,ldx_reg_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE

  {
    DI opval = GETMEMDI (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), f_simm13));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'D', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* ldx-reg+reg/asi: ldx [$rs1+$rs2]$asi,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,ldx_reg_reg_asi) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_ASI_CODE

  {
    DI opval = GETMEMDI (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2)));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'D', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* ldd-reg+reg: ldd [$rs1+$rs2],$rdd */

SEM_STATUS
SEM_FN_NAME (sparc64,ldd_reg_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDD_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDD_REG_REG_CODE

do {
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2)));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rdd", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), ADDSI (GET_H_GR (f_rs2), 4)));
    SET_H_GR (((f_rd) + (1)), opval);
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-regno-rdd-const:-WI-1", 'x', opval);
  }
} while (0);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* ldd-reg+imm: ldd [$rs1+$simm13],$rdd */

SEM_STATUS
SEM_FN_NAME (sparc64,ldd_reg_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDD_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDD_REG_IMM_CODE

do {
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), f_simm13));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rdd", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), ADDHI (f_simm13, 4)));
    SET_H_GR (((f_rd) + (1)), opval);
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-regno-rdd-const:-WI-1", 'x', opval);
  }
} while (0);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* ldd-reg+reg/asi: ldd [$rs1+$rs2]$asi,$rdd */

SEM_STATUS
SEM_FN_NAME (sparc64,ldd_reg_reg_asi) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDD_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDD_REG_REG_ASI_CODE

do {
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2)));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rdd", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), ADDSI (GET_H_GR (f_rs2), 4)));
    SET_H_GR (((f_rd) + (1)), opval);
    TRACE_RESULT (current_cpu, abuf, "gr-add:-VM-regno-rdd-const:-WI-1", 'x', opval);
  }
} while (0);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* stb-reg+reg: stb $rd,[$rs1+$rs2] */

SEM_STATUS
SEM_FN_NAME (sparc64,stb_reg_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE

  {
    QI opval = GET_H_GR (f_rd);
    SETMEMQI (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2)), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* stb-reg+imm: stb $rd,[$rs1+$simm13] */

SEM_STATUS
SEM_FN_NAME (sparc64,stb_reg_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE

  {
    QI opval = GET_H_GR (f_rd);
    SETMEMQI (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), f_simm13), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* stb-reg+reg/asi: stb $rd,[$rs1+$rs2]$asi */

SEM_STATUS
SEM_FN_NAME (sparc64,stb_reg_reg_asi) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_ASI_CODE

  {
    QI opval = GET_H_GR (f_rd);
    SETMEMQI (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2)), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* sth-reg+reg: sth $rd,[$rs1+$rs2] */

SEM_STATUS
SEM_FN_NAME (sparc64,sth_reg_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE

  {
    HI opval = GET_H_GR (f_rd);
    SETMEMHI (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2)), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* sth-reg+imm: sth $rd,[$rs1+$simm13] */

SEM_STATUS
SEM_FN_NAME (sparc64,sth_reg_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE

  {
    HI opval = GET_H_GR (f_rd);
    SETMEMHI (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), f_simm13), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* sth-reg+reg/asi: sth $rd,[$rs1+$rs2]$asi */

SEM_STATUS
SEM_FN_NAME (sparc64,sth_reg_reg_asi) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_ASI_CODE

  {
    HI opval = GET_H_GR (f_rd);
    SETMEMHI (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2)), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* st-reg+reg: st $rd,[$rs1+$rs2] */

SEM_STATUS
SEM_FN_NAME (sparc64,st_reg_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE

  {
    SI opval = GET_H_GR (f_rd);
    SETMEMSI (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2)), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* st-reg+imm: st $rd,[$rs1+$simm13] */

SEM_STATUS
SEM_FN_NAME (sparc64,st_reg_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE

  {
    SI opval = GET_H_GR (f_rd);
    SETMEMSI (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), f_simm13), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* st-reg+reg/asi: st $rd,[$rs1+$rs2]$asi */

SEM_STATUS
SEM_FN_NAME (sparc64,st_reg_reg_asi) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_ASI_CODE

  {
    SI opval = GET_H_GR (f_rd);
    SETMEMSI (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2)), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* stx-reg+reg: stx $rd,[$rs1+$rs2] */

SEM_STATUS
SEM_FN_NAME (sparc64,stx_reg_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE

  {
    DI opval = GET_H_GR (f_rd);
    SETMEMDI (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2)), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'D', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* stx-reg+imm: stx $rd,[$rs1+$simm13] */

SEM_STATUS
SEM_FN_NAME (sparc64,stx_reg_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE

  {
    DI opval = GET_H_GR (f_rd);
    SETMEMDI (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), f_simm13), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'D', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* stx-reg+reg/asi: stx $rd,[$rs1+$rs2]$asi */

SEM_STATUS
SEM_FN_NAME (sparc64,stx_reg_reg_asi) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_ASI_CODE

  {
    DI opval = GET_H_GR (f_rd);
    SETMEMDI (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2)), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'D', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* std-reg+reg: std $rdd,[$rs1+$rs2] */

SEM_STATUS
SEM_FN_NAME (sparc64,std_reg_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDD_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDD_REG_REG_CODE

do {
  {
    SI opval = GET_H_GR (f_rd);
    SETMEMSI (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2)), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = GET_H_GR (((f_rd) + (1)));
    SETMEMSI (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), ADDSI (GET_H_GR (f_rs2), 4)), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
} while (0);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* std-reg+imm: std $rdd,[$rs1+$simm13] */

SEM_STATUS
SEM_FN_NAME (sparc64,std_reg_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDD_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDD_REG_IMM_CODE

do {
  {
    SI opval = GET_H_GR (f_rd);
    SETMEMSI (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), f_simm13), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = GET_H_GR (((f_rd) + (1)));
    SETMEMSI (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), ADDHI (f_simm13, 4)), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
} while (0);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* std-reg+reg/asi: std $rdd,[$rs1+$rs2]$asi */

SEM_STATUS
SEM_FN_NAME (sparc64,std_reg_reg_asi) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDD_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDD_REG_REG_ASI_CODE

do {
  {
    SI opval = GET_H_GR (f_rd);
    SETMEMSI (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2)), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
  {
    SI opval = GET_H_GR (((f_rd) + (1)));
    SETMEMSI (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), ADDSI (GET_H_GR (f_rs2), 4)), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }
} while (0);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* fp-ld-reg+reg: ld [$rs1+$rs2],$frd */

SEM_STATUS
SEM_FN_NAME (sparc64,fp_ld_reg_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_FP_LD_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_FP_LD_REG_REG_CODE

do {
sparc64_hw_trap (current_cpu, pc, TRAP32_FP_DIS);
  {
    SF opval = GETMEMSF (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2)));
    CPU (h_fr[f_rd]) = opval;
    TRACE_RESULT (current_cpu, abuf, "frd", 'f', opval);
  }
} while (0);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* fp-ld-reg+imm: ld [$rs1+$simm13],$frd */

SEM_STATUS
SEM_FN_NAME (sparc64,fp_ld_reg_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_FP_LD_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_FP_LD_REG_IMM_CODE

do {
sparc64_hw_trap (current_cpu, pc, TRAP32_FP_DIS);
  {
    SF opval = GETMEMSF (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), f_simm13));
    CPU (h_fr[f_rd]) = opval;
    TRACE_RESULT (current_cpu, abuf, "frd", 'f', opval);
  }
} while (0);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* fp-ld-reg+reg/asi: ld [$rs1+$rs2]$asi,$frd */

SEM_STATUS
SEM_FN_NAME (sparc64,fp_ld_reg_reg_asi) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_FP_LD_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_FP_LD_REG_REG_ASI_CODE

do {
sparc64_hw_trap (current_cpu, pc, TRAP32_FP_DIS);
  {
    SF opval = GETMEMSF (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2)));
    CPU (h_fr[f_rd]) = opval;
    TRACE_RESULT (current_cpu, abuf, "frd", 'f', opval);
  }
} while (0);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* sethi: sethi $hi22,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,sethi) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_SETHI_VARS /* f-hi22 f-op2 f-rd f-op */
  EXTRACT_IFMT_SETHI_CODE

  {
    SI opval = SLLSI (f_hi22, 10);
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* add: add $rs1,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,add) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE

  {
    SI opval = ADDSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* add-imm: add $rs1,$simm13,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,add_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE

  {
    SI opval = ADDSI (GET_H_GR (f_rs1), f_simm13);
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* sub: sub $rs1,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,sub) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE

  {
    SI opval = SUBSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* sub-imm: sub $rs1,$simm13,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,sub_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE

  {
    SI opval = SUBSI (GET_H_GR (f_rs1), f_simm13);
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* addcc: addcc $rs1,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,addcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE

do {
do {
  SI tmp_x;
  tmp_x = ADDCSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2), 0);
  {
    BI opval = ADDCFSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2), 0);
    CPU (h_icc_c) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-c", 'x', opval);
  }
  {
    BI opval = ADDOFSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2), 0);
    CPU (h_icc_v) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-v", 'x', opval);
  }
  {
    BI opval = LTSI (tmp_x, 0);
    CPU (h_icc_n) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-n", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_x, 0);
    CPU (h_icc_z) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-z", 'x', opval);
  }
} while (0);
  {
    SI opval = ADDSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
} while (0);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* addcc-imm: addcc $rs1,$simm13,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,addcc_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE

do {
do {
  SI tmp_x;
  tmp_x = ADDCSI (GET_H_GR (f_rs1), f_simm13, 0);
  {
    BI opval = ADDCFSI (GET_H_GR (f_rs1), f_simm13, 0);
    CPU (h_icc_c) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-c", 'x', opval);
  }
  {
    BI opval = ADDOFSI (GET_H_GR (f_rs1), f_simm13, 0);
    CPU (h_icc_v) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-v", 'x', opval);
  }
  {
    BI opval = LTSI (tmp_x, 0);
    CPU (h_icc_n) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-n", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_x, 0);
    CPU (h_icc_z) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-z", 'x', opval);
  }
} while (0);
  {
    SI opval = ADDSI (GET_H_GR (f_rs1), f_simm13);
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
} while (0);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* subcc: subcc $rs1,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,subcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE

do {
do {
  SI tmp_x;
  tmp_x = SUBCSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2), 0);
  {
    BI opval = SUBCFSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2), 0);
    CPU (h_icc_c) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-c", 'x', opval);
  }
  {
    BI opval = SUBOFSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2), 0);
    CPU (h_icc_v) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-v", 'x', opval);
  }
  {
    BI opval = LTSI (tmp_x, 0);
    CPU (h_icc_n) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-n", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_x, 0);
    CPU (h_icc_z) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-z", 'x', opval);
  }
} while (0);
  {
    SI opval = SUBSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
} while (0);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* subcc-imm: subcc $rs1,$simm13,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,subcc_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE

do {
do {
  SI tmp_x;
  tmp_x = SUBCSI (GET_H_GR (f_rs1), f_simm13, 0);
  {
    BI opval = SUBCFSI (GET_H_GR (f_rs1), f_simm13, 0);
    CPU (h_icc_c) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-c", 'x', opval);
  }
  {
    BI opval = SUBOFSI (GET_H_GR (f_rs1), f_simm13, 0);
    CPU (h_icc_v) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-v", 'x', opval);
  }
  {
    BI opval = LTSI (tmp_x, 0);
    CPU (h_icc_n) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-n", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_x, 0);
    CPU (h_icc_z) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-z", 'x', opval);
  }
} while (0);
  {
    SI opval = SUBSI (GET_H_GR (f_rs1), f_simm13);
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
} while (0);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* addc: addc $rs1,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,addc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE

  {
    SI opval = ADDCSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2), CPU (h_icc_c));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* addc-imm: addc $rs1,$simm13,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,addc_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE

  {
    SI opval = ADDCSI (GET_H_GR (f_rs1), f_simm13, CPU (h_icc_c));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* subc: subc $rs1,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,subc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE

  {
    SI opval = SUBCSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2), CPU (h_icc_c));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* subc-imm: subc $rs1,$simm13,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,subc_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE

  {
    SI opval = SUBCSI (GET_H_GR (f_rs1), f_simm13, CPU (h_icc_c));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* addccc: addccc $rs1,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,addccc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE

do {
do {
  SI tmp_x;
  tmp_x = ADDCSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2), CPU (h_icc_c));
  {
    BI opval = ADDCFSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2), CPU (h_icc_c));
    CPU (h_icc_c) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-c", 'x', opval);
  }
  {
    BI opval = ADDOFSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2), CPU (h_icc_c));
    CPU (h_icc_v) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-v", 'x', opval);
  }
  {
    BI opval = LTSI (tmp_x, 0);
    CPU (h_icc_n) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-n", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_x, 0);
    CPU (h_icc_z) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-z", 'x', opval);
  }
} while (0);
  {
    SI opval = ADDCSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2), CPU (h_icc_c));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
} while (0);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* addccc-imm: addccc $rs1,$simm13,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,addccc_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE

do {
do {
  SI tmp_x;
  tmp_x = ADDCSI (GET_H_GR (f_rs1), f_simm13, CPU (h_icc_c));
  {
    BI opval = ADDCFSI (GET_H_GR (f_rs1), f_simm13, CPU (h_icc_c));
    CPU (h_icc_c) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-c", 'x', opval);
  }
  {
    BI opval = ADDOFSI (GET_H_GR (f_rs1), f_simm13, CPU (h_icc_c));
    CPU (h_icc_v) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-v", 'x', opval);
  }
  {
    BI opval = LTSI (tmp_x, 0);
    CPU (h_icc_n) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-n", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_x, 0);
    CPU (h_icc_z) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-z", 'x', opval);
  }
} while (0);
  {
    SI opval = ADDCSI (GET_H_GR (f_rs1), f_simm13, CPU (h_icc_c));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
} while (0);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* subccc: subccc $rs1,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,subccc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE

do {
do {
  SI tmp_x;
  tmp_x = SUBCSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2), CPU (h_icc_c));
  {
    BI opval = SUBCFSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2), CPU (h_icc_c));
    CPU (h_icc_c) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-c", 'x', opval);
  }
  {
    BI opval = SUBOFSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2), CPU (h_icc_c));
    CPU (h_icc_v) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-v", 'x', opval);
  }
  {
    BI opval = LTSI (tmp_x, 0);
    CPU (h_icc_n) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-n", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_x, 0);
    CPU (h_icc_z) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-z", 'x', opval);
  }
} while (0);
  {
    SI opval = SUBCSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2), CPU (h_icc_c));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
} while (0);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* subccc-imm: subccc $rs1,$simm13,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,subccc_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE

do {
do {
  SI tmp_x;
  tmp_x = SUBCSI (GET_H_GR (f_rs1), f_simm13, CPU (h_icc_c));
  {
    BI opval = SUBCFSI (GET_H_GR (f_rs1), f_simm13, CPU (h_icc_c));
    CPU (h_icc_c) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-c", 'x', opval);
  }
  {
    BI opval = SUBOFSI (GET_H_GR (f_rs1), f_simm13, CPU (h_icc_c));
    CPU (h_icc_v) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-v", 'x', opval);
  }
  {
    BI opval = LTSI (tmp_x, 0);
    CPU (h_icc_n) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-n", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_x, 0);
    CPU (h_icc_z) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-z", 'x', opval);
  }
} while (0);
  {
    SI opval = SUBCSI (GET_H_GR (f_rs1), f_simm13, CPU (h_icc_c));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
} while (0);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* and: and $rs1,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,and) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE

  {
    SI opval = ANDSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* and-imm: and $rs1,$simm13,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,and_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE

  {
    SI opval = ANDSI (GET_H_GR (f_rs1), f_simm13);
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* andcc: andcc $rs1,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,andcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE

do {
do {
  {
    BI opval = EQSI (ANDSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2)), 0);
    CPU (h_icc_z) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-z", 'x', opval);
  }
  {
    BI opval = LTSI (ANDSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2)), 0);
    CPU (h_icc_n) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-n", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_icc_c) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-c", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_icc_v) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-v", 'x', opval);
  }
} while (0);
  {
    SI opval = ANDSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
} while (0);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* andcc-imm: andcc $rs1,$simm13,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,andcc_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE

do {
do {
  {
    BI opval = EQSI (ANDSI (GET_H_GR (f_rs1), f_simm13), 0);
    CPU (h_icc_z) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-z", 'x', opval);
  }
  {
    BI opval = LTSI (ANDSI (GET_H_GR (f_rs1), f_simm13), 0);
    CPU (h_icc_n) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-n", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_icc_c) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-c", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_icc_v) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-v", 'x', opval);
  }
} while (0);
  {
    SI opval = ANDSI (GET_H_GR (f_rs1), f_simm13);
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
} while (0);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* or: or $rs1,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,or) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE

  {
    SI opval = ORSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* or-imm: or $rs1,$simm13,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,or_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE

  {
    SI opval = ORSI (GET_H_GR (f_rs1), f_simm13);
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* orcc: orcc $rs1,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,orcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE

do {
do {
  {
    BI opval = EQSI (ORSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2)), 0);
    CPU (h_icc_z) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-z", 'x', opval);
  }
  {
    BI opval = LTSI (ORSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2)), 0);
    CPU (h_icc_n) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-n", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_icc_c) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-c", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_icc_v) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-v", 'x', opval);
  }
} while (0);
  {
    SI opval = ORSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
} while (0);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* orcc-imm: orcc $rs1,$simm13,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,orcc_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE

do {
do {
  {
    BI opval = EQSI (ORSI (GET_H_GR (f_rs1), f_simm13), 0);
    CPU (h_icc_z) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-z", 'x', opval);
  }
  {
    BI opval = LTSI (ORSI (GET_H_GR (f_rs1), f_simm13), 0);
    CPU (h_icc_n) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-n", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_icc_c) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-c", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_icc_v) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-v", 'x', opval);
  }
} while (0);
  {
    SI opval = ORSI (GET_H_GR (f_rs1), f_simm13);
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
} while (0);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* xor: xor $rs1,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,xor) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE

  {
    SI opval = XORSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* xor-imm: xor $rs1,$simm13,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,xor_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE

  {
    SI opval = XORSI (GET_H_GR (f_rs1), f_simm13);
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* xorcc: xorcc $rs1,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,xorcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE

do {
do {
  {
    BI opval = EQSI (XORSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2)), 0);
    CPU (h_icc_z) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-z", 'x', opval);
  }
  {
    BI opval = LTSI (XORSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2)), 0);
    CPU (h_icc_n) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-n", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_icc_c) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-c", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_icc_v) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-v", 'x', opval);
  }
} while (0);
  {
    SI opval = XORSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
} while (0);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* xorcc-imm: xorcc $rs1,$simm13,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,xorcc_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE

do {
do {
  {
    BI opval = EQSI (XORSI (GET_H_GR (f_rs1), f_simm13), 0);
    CPU (h_icc_z) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-z", 'x', opval);
  }
  {
    BI opval = LTSI (XORSI (GET_H_GR (f_rs1), f_simm13), 0);
    CPU (h_icc_n) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-n", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_icc_c) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-c", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_icc_v) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-v", 'x', opval);
  }
} while (0);
  {
    SI opval = XORSI (GET_H_GR (f_rs1), f_simm13);
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
} while (0);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* andn: andn $rs1,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,andn) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE

  {
    SI opval = ANDSI (GET_H_GR (f_rs1), INVSI (GET_H_GR (f_rs2)));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* andn-imm: andn $rs1,$simm13,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,andn_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE

  {
    SI opval = ANDSI (GET_H_GR (f_rs1), INVHI (f_simm13));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* andncc: andncc $rs1,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,andncc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE

do {
do {
  {
    BI opval = EQSI (ANDSI (GET_H_GR (f_rs1), INVSI (GET_H_GR (f_rs2))), 0);
    CPU (h_icc_z) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-z", 'x', opval);
  }
  {
    BI opval = LTSI (ANDSI (GET_H_GR (f_rs1), INVSI (GET_H_GR (f_rs2))), 0);
    CPU (h_icc_n) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-n", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_icc_c) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-c", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_icc_v) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-v", 'x', opval);
  }
} while (0);
  {
    SI opval = ANDSI (GET_H_GR (f_rs1), INVSI (GET_H_GR (f_rs2)));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
} while (0);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* andncc-imm: andncc $rs1,$simm13,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,andncc_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE

do {
do {
  {
    BI opval = EQSI (ANDSI (GET_H_GR (f_rs1), INVHI (f_simm13)), 0);
    CPU (h_icc_z) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-z", 'x', opval);
  }
  {
    BI opval = LTSI (ANDSI (GET_H_GR (f_rs1), INVHI (f_simm13)), 0);
    CPU (h_icc_n) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-n", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_icc_c) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-c", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_icc_v) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-v", 'x', opval);
  }
} while (0);
  {
    SI opval = ANDSI (GET_H_GR (f_rs1), INVHI (f_simm13));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
} while (0);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* orn: orn $rs1,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,orn) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE

  {
    SI opval = ORSI (GET_H_GR (f_rs1), INVSI (GET_H_GR (f_rs2)));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* orn-imm: orn $rs1,$simm13,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,orn_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE

  {
    SI opval = ORSI (GET_H_GR (f_rs1), INVHI (f_simm13));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* orncc: orncc $rs1,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,orncc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE

do {
do {
  {
    BI opval = EQSI (ORSI (GET_H_GR (f_rs1), INVSI (GET_H_GR (f_rs2))), 0);
    CPU (h_icc_z) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-z", 'x', opval);
  }
  {
    BI opval = LTSI (ORSI (GET_H_GR (f_rs1), INVSI (GET_H_GR (f_rs2))), 0);
    CPU (h_icc_n) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-n", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_icc_c) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-c", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_icc_v) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-v", 'x', opval);
  }
} while (0);
  {
    SI opval = ORSI (GET_H_GR (f_rs1), INVSI (GET_H_GR (f_rs2)));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
} while (0);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* orncc-imm: orncc $rs1,$simm13,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,orncc_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE

do {
do {
  {
    BI opval = EQSI (ORSI (GET_H_GR (f_rs1), INVHI (f_simm13)), 0);
    CPU (h_icc_z) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-z", 'x', opval);
  }
  {
    BI opval = LTSI (ORSI (GET_H_GR (f_rs1), INVHI (f_simm13)), 0);
    CPU (h_icc_n) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-n", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_icc_c) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-c", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_icc_v) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-v", 'x', opval);
  }
} while (0);
  {
    SI opval = ORSI (GET_H_GR (f_rs1), INVHI (f_simm13));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
} while (0);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* xnor: xnor $rs1,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,xnor) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE

  {
    SI opval = XORSI (GET_H_GR (f_rs1), INVSI (GET_H_GR (f_rs2)));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* xnor-imm: xnor $rs1,$simm13,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,xnor_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE

  {
    SI opval = XORSI (GET_H_GR (f_rs1), INVHI (f_simm13));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* xnorcc: xnorcc $rs1,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,xnorcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE

do {
do {
  {
    BI opval = EQSI (XORSI (GET_H_GR (f_rs1), INVSI (GET_H_GR (f_rs2))), 0);
    CPU (h_icc_z) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-z", 'x', opval);
  }
  {
    BI opval = LTSI (XORSI (GET_H_GR (f_rs1), INVSI (GET_H_GR (f_rs2))), 0);
    CPU (h_icc_n) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-n", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_icc_c) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-c", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_icc_v) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-v", 'x', opval);
  }
} while (0);
  {
    SI opval = XORSI (GET_H_GR (f_rs1), INVSI (GET_H_GR (f_rs2)));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
} while (0);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* xnorcc-imm: xnorcc $rs1,$simm13,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,xnorcc_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE

do {
do {
  {
    BI opval = EQSI (XORSI (GET_H_GR (f_rs1), INVHI (f_simm13)), 0);
    CPU (h_icc_z) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-z", 'x', opval);
  }
  {
    BI opval = LTSI (XORSI (GET_H_GR (f_rs1), INVHI (f_simm13)), 0);
    CPU (h_icc_n) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-n", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_icc_c) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-c", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_icc_v) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-v", 'x', opval);
  }
} while (0);
  {
    SI opval = XORSI (GET_H_GR (f_rs1), INVHI (f_simm13));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
} while (0);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* sll: sll $rs1,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,sll) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE

  {
    SI opval = SLLSI (GET_H_GR (f_rs1), ANDSI (GET_H_GR (f_rs2), 31));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* sll-imm: sll $rs1,$simm13,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,sll_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE

  {
    SI opval = SLLSI (GET_H_GR (f_rs1), ANDHI (f_simm13, 31));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* srl: srl $rs1,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,srl) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE

  {
    SI opval = SRLSI (GET_H_GR (f_rs1), ANDSI (GET_H_GR (f_rs2), 31));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* srl-imm: srl $rs1,$simm13,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,srl_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE

  {
    SI opval = SRLSI (GET_H_GR (f_rs1), ANDHI (f_simm13, 31));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* sra: sra $rs1,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,sra) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE

  {
    SI opval = SRASI (GET_H_GR (f_rs1), ANDSI (GET_H_GR (f_rs2), 31));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* sra-imm: sra $rs1,$simm13,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,sra_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE

  {
    SI opval = SRASI (GET_H_GR (f_rs1), ANDHI (f_simm13, 31));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* smul: smul $rs1,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,smul) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE

do {
  DI tmp_res;
  tmp_res = MULDI (EXTSIDI (GET_H_GR (f_rs1)), EXTSIDI (GET_H_GR (f_rs2)));
  {
    SI opval = TRUNCDISI (SRLDI (tmp_res, 32));
    SET_H_Y (opval);
    TRACE_RESULT (current_cpu, abuf, "y-0", 'x', opval);
  }
  {
    SI opval = TRUNCDISI (tmp_res);
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
} while (0);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* smul-imm: smul $rs1,$simm13,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,smul_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE

do {
  DI tmp_res;
  tmp_res = MULDI (EXTSIDI (GET_H_GR (f_rs1)), EXTHIDI (f_simm13));
  {
    SI opval = TRUNCDISI (SRLDI (tmp_res, 32));
    SET_H_Y (opval);
    TRACE_RESULT (current_cpu, abuf, "y-0", 'x', opval);
  }
  {
    SI opval = TRUNCDISI (tmp_res);
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
} while (0);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* smul-cc: smulcc $rs1,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,smul_cc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE

do {
  DI tmp_res;
  tmp_res = MULDI (EXTSIDI (GET_H_GR (f_rs1)), EXTSIDI (GET_H_GR (f_rs2)));
  {
    SI opval = TRUNCDISI (SRLDI (tmp_res, 32));
    SET_H_Y (opval);
    TRACE_RESULT (current_cpu, abuf, "y-0", 'x', opval);
  }
  {
    SI opval = TRUNCDISI (tmp_res);
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
do {
  {
    BI opval = EQSI (TRUNCDISI (tmp_res), 0);
    CPU (h_icc_z) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-z", 'x', opval);
  }
  {
    BI opval = LTSI (TRUNCDISI (tmp_res), 0);
    CPU (h_icc_n) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-n", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_icc_c) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-c", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_icc_v) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-v", 'x', opval);
  }
} while (0);
} while (0);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* smul-cc-imm: smulcc $rs1,$simm13,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,smul_cc_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE

do {
  DI tmp_res;
  tmp_res = MULDI (EXTSIDI (GET_H_GR (f_rs1)), EXTHIDI (f_simm13));
  {
    SI opval = TRUNCDISI (SRLDI (tmp_res, 32));
    SET_H_Y (opval);
    TRACE_RESULT (current_cpu, abuf, "y-0", 'x', opval);
  }
  {
    SI opval = TRUNCDISI (tmp_res);
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
do {
  {
    BI opval = EQSI (TRUNCDISI (tmp_res), 0);
    CPU (h_icc_z) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-z", 'x', opval);
  }
  {
    BI opval = LTSI (TRUNCDISI (tmp_res), 0);
    CPU (h_icc_n) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-n", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_icc_c) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-c", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_icc_v) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-v", 'x', opval);
  }
} while (0);
} while (0);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* umul: umul $rs1,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,umul) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE

do {
  DI tmp_res;
  tmp_res = MULDI (ZEXTSIDI (GET_H_GR (f_rs1)), ZEXTSIDI (GET_H_GR (f_rs2)));
  {
    SI opval = TRUNCDISI (SRLDI (tmp_res, 32));
    SET_H_Y (opval);
    TRACE_RESULT (current_cpu, abuf, "y-0", 'x', opval);
  }
  {
    SI opval = TRUNCDISI (tmp_res);
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
} while (0);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* umul-imm: umul $rs1,$simm13,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,umul_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE

do {
  DI tmp_res;
  tmp_res = MULDI (ZEXTSIDI (GET_H_GR (f_rs1)), ZEXTHIDI (f_simm13));
  {
    SI opval = TRUNCDISI (SRLDI (tmp_res, 32));
    SET_H_Y (opval);
    TRACE_RESULT (current_cpu, abuf, "y-0", 'x', opval);
  }
  {
    SI opval = TRUNCDISI (tmp_res);
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
} while (0);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* umul-cc: umulcc $rs1,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,umul_cc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE

do {
  DI tmp_res;
  tmp_res = MULDI (ZEXTSIDI (GET_H_GR (f_rs1)), ZEXTSIDI (GET_H_GR (f_rs2)));
  {
    SI opval = TRUNCDISI (SRLDI (tmp_res, 32));
    SET_H_Y (opval);
    TRACE_RESULT (current_cpu, abuf, "y-0", 'x', opval);
  }
  {
    SI opval = TRUNCDISI (tmp_res);
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
do {
  {
    BI opval = EQSI (TRUNCDISI (tmp_res), 0);
    CPU (h_icc_z) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-z", 'x', opval);
  }
  {
    BI opval = LTSI (TRUNCDISI (tmp_res), 0);
    CPU (h_icc_n) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-n", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_icc_c) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-c", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_icc_v) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-v", 'x', opval);
  }
} while (0);
} while (0);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* umul-cc-imm: umulcc $rs1,$simm13,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,umul_cc_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE

do {
  DI tmp_res;
  tmp_res = MULDI (ZEXTSIDI (GET_H_GR (f_rs1)), ZEXTHIDI (f_simm13));
  {
    SI opval = TRUNCDISI (SRLDI (tmp_res, 32));
    SET_H_Y (opval);
    TRACE_RESULT (current_cpu, abuf, "y-0", 'x', opval);
  }
  {
    SI opval = TRUNCDISI (tmp_res);
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
do {
  {
    BI opval = EQSI (TRUNCDISI (tmp_res), 0);
    CPU (h_icc_z) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-z", 'x', opval);
  }
  {
    BI opval = LTSI (TRUNCDISI (tmp_res), 0);
    CPU (h_icc_n) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-n", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_icc_c) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-c", 'x', opval);
  }
  {
    BI opval = 0;
    CPU (h_icc_v) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-v", 'x', opval);
  }
} while (0);
} while (0);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* mulscc: mulscc $rs1,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,mulscc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE

do {
  SI tmp_rd_tmp;
  SI tmp_add_tmp;
  SI tmp_tmp;
  tmp_tmp = SRLSI (GET_H_GR (f_rs1), 1);
if (NEBI (XORBI (CPU (h_icc_n), CPU (h_icc_v)), 0)) {
  tmp_tmp = ORSI (tmp_tmp, 0x80000000);
}
if (NESI (ANDSI (GET_H_Y (), 1), 0)) {
  tmp_add_tmp = GET_H_GR (f_rs2);
} else {
  tmp_add_tmp = 0;
}
  tmp_rd_tmp = ADDSI (tmp_tmp, tmp_add_tmp);
do {
  SI tmp_x;
  tmp_x = ADDCSI (tmp_tmp, tmp_add_tmp, 0);
  {
    BI opval = ADDCFSI (tmp_tmp, tmp_add_tmp, 0);
    CPU (h_icc_c) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-c", 'x', opval);
  }
  {
    BI opval = ADDOFSI (tmp_tmp, tmp_add_tmp, 0);
    CPU (h_icc_v) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-v", 'x', opval);
  }
  {
    BI opval = LTSI (tmp_x, 0);
    CPU (h_icc_n) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-n", 'x', opval);
  }
  {
    BI opval = EQSI (tmp_x, 0);
    CPU (h_icc_z) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-z", 'x', opval);
  }
} while (0);
  {
    SI opval = SRLSI (GET_H_Y (), 1);
    SET_H_Y (opval);
    TRACE_RESULT (current_cpu, abuf, "y-0", 'x', opval);
  }
if (NESI (ANDSI (GET_H_GR (f_rs1), 1), 0)) {
  {
    SI opval = ORSI (GET_H_Y (), 0x80000000);
    SET_H_Y (opval);
    TRACE_RESULT (current_cpu, abuf, "y-0", 'x', opval);
  }
}
  {
    SI opval = tmp_rd_tmp;
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
} while (0);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* save: save $rs1,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,save) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE

  {
    SI opval = sparc64_do_save (current_cpu, pc, GET_H_GR (f_rs1), GET_H_GR (f_rs2));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* save-imm: save $rs1,$simm13,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,save_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE

  {
    SI opval = sparc64_do_save (current_cpu, pc, GET_H_GR (f_rs1), f_simm13);
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* restore: restore $rs1,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,restore) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE

  {
    SI opval = sparc64_do_restore (current_cpu, pc, GET_H_GR (f_rs1), GET_H_GR (f_rs2));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* restore-imm: restore $rs1,$simm13,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,restore_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE

  {
    SI opval = sparc64_do_restore (current_cpu, pc, GET_H_GR (f_rs1), f_simm13);
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* rett: rett $rs1,$rs2 */

SEM_STATUS
SEM_FN_NAME (sparc64,rett) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_FLUSH_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_FLUSH_CODE

do {
  {
    USI opval = sparc64_do_rett (current_cpu, pc, GET_H_GR (f_rs1), GET_H_GR (f_rs2));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} while (0);

  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_UNCOND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* rett-imm: rett $rs1,$simm13 */

SEM_STATUS
SEM_FN_NAME (sparc64,rett_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_FLUSH_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_FLUSH_IMM_CODE

do {
  {
    USI opval = sparc64_do_rett (current_cpu, pc, GET_H_GR (f_rs1), f_simm13);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} while (0);

  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_UNCOND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* unimp: unimp $imm22 */

SEM_STATUS
SEM_FN_NAME (sparc64,unimp) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_UNIMP_VARS /* f-imm22 f-op2 f-rd-res f-op */
  EXTRACT_IFMT_UNIMP_CODE

sparc_do_unimp (current_cpu, pc, f_imm22);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* call: call $disp30 */

SEM_STATUS
SEM_FN_NAME (sparc64,call) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_CALL_VARS /* f-disp30 f-op */
      IADDR i_disp30;
  EXTRACT_IFMT_CALL_CODE
  i_disp30 = f_disp30;

do {
  {
    SI opval = pc;
    SET_H_GR (((UINT) 15), opval);
    TRACE_RESULT (current_cpu, abuf, "gr-15", 'x', opval);
  }
do {
  {
    USI opval = i_disp30;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} while (0);
} while (0);

  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_UNCOND_CTI-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* jmpl: jmpl $rs1+$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,jmpl) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE

do {
  {
    SI opval = pc;
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
do {
  {
    USI opval = ADDSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} while (0);
} while (0);

  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_UNCOND_CTI-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* jmpl-imm: jmpl $rs1+$simm13,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,jmpl_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE

do {
  {
    SI opval = pc;
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
do {
  {
    USI opval = ADDSI (GET_H_GR (f_rs1), f_simm13);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} while (0);
} while (0);

  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_UNCOND_CTI-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* ba: ba$a $disp22 */

SEM_STATUS
SEM_FN_NAME (sparc64,ba) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_BA_VARS /* f-disp22 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BA_CODE
  i_disp22 = f_disp22;

do {
do {
  {
    USI opval = i_disp22;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
SEM_ANNUL_INSN (current_cpu, pc, f_a);
} while (0);
} while (0);

  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_V9_DEPRECATED-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_UNCOND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* ta: ta $rs1,$rs2 */

SEM_STATUS
SEM_FN_NAME (sparc64,ta) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_TA_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_CODE

  {
    USI opval = sparc64_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), GET_H_GR (f_rs2));
;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }

  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_TRAP-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_UNCOND_CTI-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* ta-imm: ta $rs1,$simm13 */

SEM_STATUS
SEM_FN_NAME (sparc64,ta_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_TA_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_IMM_CODE

  {
    USI opval = sparc64_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), f_simm13);
;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }

  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_TRAP-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_UNCOND_CTI-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* bn: bn$a $disp22 */

SEM_STATUS
SEM_FN_NAME (sparc64,bn) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_BA_VARS /* f-disp22 f-op2 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_BA_CODE

do {
do {
do { } while (0); /*nop*/
SEM_ANNUL_INSN (current_cpu, pc, f_a);
} while (0);
} while (0);

  SEM_NBRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_V9_DEPRECATED-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* tn: tn $rs1,$rs2 */

SEM_STATUS
SEM_FN_NAME (sparc64,tn) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_TA_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_CODE

do { } while (0); /*nop*/

  SEM_NBRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_TRAP-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* tn-imm: tn $rs1,$simm13 */

SEM_STATUS
SEM_FN_NAME (sparc64,tn_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_TA_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_IMM_CODE

do { } while (0); /*nop*/

  SEM_NBRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_TRAP-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* bne: bne$a $disp22 */

SEM_STATUS
SEM_FN_NAME (sparc64,bne) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_BA_VARS /* f-disp22 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BA_CODE
  i_disp22 = f_disp22;

do {
if (NOTBI (CPU (h_icc_z))) {
  {
    USI opval = i_disp22;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} else {
SEM_ANNUL_INSN (current_cpu, pc, f_a);
}
} while (0);

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_V9_DEPRECATED-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* tne: tne $rs1,$rs2 */

SEM_STATUS
SEM_FN_NAME (sparc64,tne) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_TA_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_CODE

if (NOTBI (CPU (h_icc_z))) {
  {
    USI opval = sparc64_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), GET_H_GR (f_rs2));
;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 4);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_TRAP-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* tne-imm: tne $rs1,$simm13 */

SEM_STATUS
SEM_FN_NAME (sparc64,tne_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_TA_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_IMM_CODE

if (NOTBI (CPU (h_icc_z))) {
  {
    USI opval = sparc64_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), f_simm13);
;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 4);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_TRAP-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* be: be$a $disp22 */

SEM_STATUS
SEM_FN_NAME (sparc64,be) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_BA_VARS /* f-disp22 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BA_CODE
  i_disp22 = f_disp22;

do {
if (CPU (h_icc_z)) {
  {
    USI opval = i_disp22;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} else {
SEM_ANNUL_INSN (current_cpu, pc, f_a);
}
} while (0);

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_V9_DEPRECATED-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* te: te $rs1,$rs2 */

SEM_STATUS
SEM_FN_NAME (sparc64,te) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_TA_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_CODE

if (CPU (h_icc_z)) {
  {
    USI opval = sparc64_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), GET_H_GR (f_rs2));
;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 4);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_TRAP-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* te-imm: te $rs1,$simm13 */

SEM_STATUS
SEM_FN_NAME (sparc64,te_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_TA_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_IMM_CODE

if (CPU (h_icc_z)) {
  {
    USI opval = sparc64_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), f_simm13);
;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 4);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_TRAP-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* bg: bg$a $disp22 */

SEM_STATUS
SEM_FN_NAME (sparc64,bg) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_BA_VARS /* f-disp22 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BA_CODE
  i_disp22 = f_disp22;

do {
if (NOTBI (ORBI (CPU (h_icc_z), XORBI (CPU (h_icc_n), CPU (h_icc_v))))) {
  {
    USI opval = i_disp22;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 5);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} else {
SEM_ANNUL_INSN (current_cpu, pc, f_a);
}
} while (0);

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_V9_DEPRECATED-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* tg: tg $rs1,$rs2 */

SEM_STATUS
SEM_FN_NAME (sparc64,tg) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_TA_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_CODE

if (NOTBI (ORBI (CPU (h_icc_z), XORBI (CPU (h_icc_n), CPU (h_icc_v))))) {
  {
    USI opval = sparc64_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), GET_H_GR (f_rs2));
;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 6);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_TRAP-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* tg-imm: tg $rs1,$simm13 */

SEM_STATUS
SEM_FN_NAME (sparc64,tg_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_TA_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_IMM_CODE

if (NOTBI (ORBI (CPU (h_icc_z), XORBI (CPU (h_icc_n), CPU (h_icc_v))))) {
  {
    USI opval = sparc64_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), f_simm13);
;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 6);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_TRAP-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* ble: ble$a $disp22 */

SEM_STATUS
SEM_FN_NAME (sparc64,ble) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_BA_VARS /* f-disp22 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BA_CODE
  i_disp22 = f_disp22;

do {
if (ORBI (CPU (h_icc_z), XORBI (CPU (h_icc_n), CPU (h_icc_v)))) {
  {
    USI opval = i_disp22;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 5);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} else {
SEM_ANNUL_INSN (current_cpu, pc, f_a);
}
} while (0);

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_V9_DEPRECATED-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* tle: tle $rs1,$rs2 */

SEM_STATUS
SEM_FN_NAME (sparc64,tle) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_TA_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_CODE

if (ORBI (CPU (h_icc_z), XORBI (CPU (h_icc_n), CPU (h_icc_v)))) {
  {
    USI opval = sparc64_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), GET_H_GR (f_rs2));
;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 6);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_TRAP-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* tle-imm: tle $rs1,$simm13 */

SEM_STATUS
SEM_FN_NAME (sparc64,tle_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_TA_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_IMM_CODE

if (ORBI (CPU (h_icc_z), XORBI (CPU (h_icc_n), CPU (h_icc_v)))) {
  {
    USI opval = sparc64_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), f_simm13);
;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 6);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_TRAP-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* bge: bge$a $disp22 */

SEM_STATUS
SEM_FN_NAME (sparc64,bge) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_BA_VARS /* f-disp22 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BA_CODE
  i_disp22 = f_disp22;

do {
if (NOTBI (XORBI (CPU (h_icc_n), CPU (h_icc_v)))) {
  {
    USI opval = i_disp22;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 4);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} else {
SEM_ANNUL_INSN (current_cpu, pc, f_a);
}
} while (0);

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_V9_DEPRECATED-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* tge: tge $rs1,$rs2 */

SEM_STATUS
SEM_FN_NAME (sparc64,tge) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_TA_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_CODE

if (NOTBI (XORBI (CPU (h_icc_n), CPU (h_icc_v)))) {
  {
    USI opval = sparc64_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), GET_H_GR (f_rs2));
;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 5);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_TRAP-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* tge-imm: tge $rs1,$simm13 */

SEM_STATUS
SEM_FN_NAME (sparc64,tge_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_TA_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_IMM_CODE

if (NOTBI (XORBI (CPU (h_icc_n), CPU (h_icc_v)))) {
  {
    USI opval = sparc64_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), f_simm13);
;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 5);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_TRAP-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* bl: bl$a $disp22 */

SEM_STATUS
SEM_FN_NAME (sparc64,bl) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_BA_VARS /* f-disp22 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BA_CODE
  i_disp22 = f_disp22;

do {
if (XORBI (CPU (h_icc_n), CPU (h_icc_v))) {
  {
    USI opval = i_disp22;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 4);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} else {
SEM_ANNUL_INSN (current_cpu, pc, f_a);
}
} while (0);

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_V9_DEPRECATED-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* tl: tl $rs1,$rs2 */

SEM_STATUS
SEM_FN_NAME (sparc64,tl) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_TA_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_CODE

if (XORBI (CPU (h_icc_n), CPU (h_icc_v))) {
  {
    USI opval = sparc64_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), GET_H_GR (f_rs2));
;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 5);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_TRAP-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* tl-imm: tl $rs1,$simm13 */

SEM_STATUS
SEM_FN_NAME (sparc64,tl_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_TA_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_IMM_CODE

if (XORBI (CPU (h_icc_n), CPU (h_icc_v))) {
  {
    USI opval = sparc64_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), f_simm13);
;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 5);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_TRAP-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* bgu: bgu$a $disp22 */

SEM_STATUS
SEM_FN_NAME (sparc64,bgu) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_BA_VARS /* f-disp22 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BA_CODE
  i_disp22 = f_disp22;

do {
if (NOTBI (ORBI (CPU (h_icc_c), CPU (h_icc_z)))) {
  {
    USI opval = i_disp22;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 4);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} else {
SEM_ANNUL_INSN (current_cpu, pc, f_a);
}
} while (0);

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_V9_DEPRECATED-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* tgu: tgu $rs1,$rs2 */

SEM_STATUS
SEM_FN_NAME (sparc64,tgu) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_TA_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_CODE

if (NOTBI (ORBI (CPU (h_icc_c), CPU (h_icc_z)))) {
  {
    USI opval = sparc64_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), GET_H_GR (f_rs2));
;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 5);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_TRAP-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* tgu-imm: tgu $rs1,$simm13 */

SEM_STATUS
SEM_FN_NAME (sparc64,tgu_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_TA_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_IMM_CODE

if (NOTBI (ORBI (CPU (h_icc_c), CPU (h_icc_z)))) {
  {
    USI opval = sparc64_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), f_simm13);
;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 5);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_TRAP-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* bleu: bleu$a $disp22 */

SEM_STATUS
SEM_FN_NAME (sparc64,bleu) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_BA_VARS /* f-disp22 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BA_CODE
  i_disp22 = f_disp22;

do {
if (ORBI (CPU (h_icc_c), CPU (h_icc_z))) {
  {
    USI opval = i_disp22;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 4);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} else {
SEM_ANNUL_INSN (current_cpu, pc, f_a);
}
} while (0);

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_V9_DEPRECATED-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* tleu: tleu $rs1,$rs2 */

SEM_STATUS
SEM_FN_NAME (sparc64,tleu) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_TA_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_CODE

if (ORBI (CPU (h_icc_c), CPU (h_icc_z))) {
  {
    USI opval = sparc64_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), GET_H_GR (f_rs2));
;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 5);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_TRAP-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* tleu-imm: tleu $rs1,$simm13 */

SEM_STATUS
SEM_FN_NAME (sparc64,tleu_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_TA_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_IMM_CODE

if (ORBI (CPU (h_icc_c), CPU (h_icc_z))) {
  {
    USI opval = sparc64_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), f_simm13);
;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 5);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_TRAP-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* bcc: bcc$a $disp22 */

SEM_STATUS
SEM_FN_NAME (sparc64,bcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_BA_VARS /* f-disp22 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BA_CODE
  i_disp22 = f_disp22;

do {
if (NOTBI (CPU (h_icc_c))) {
  {
    USI opval = i_disp22;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} else {
SEM_ANNUL_INSN (current_cpu, pc, f_a);
}
} while (0);

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_V9_DEPRECATED-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* tcc: tcc $rs1,$rs2 */

SEM_STATUS
SEM_FN_NAME (sparc64,tcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_TA_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_CODE

if (NOTBI (CPU (h_icc_c))) {
  {
    USI opval = sparc64_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), GET_H_GR (f_rs2));
;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 4);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_TRAP-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* tcc-imm: tcc $rs1,$simm13 */

SEM_STATUS
SEM_FN_NAME (sparc64,tcc_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_TA_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_IMM_CODE

if (NOTBI (CPU (h_icc_c))) {
  {
    USI opval = sparc64_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), f_simm13);
;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 4);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_TRAP-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* bcs: bcs$a $disp22 */

SEM_STATUS
SEM_FN_NAME (sparc64,bcs) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_BA_VARS /* f-disp22 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BA_CODE
  i_disp22 = f_disp22;

do {
if (CPU (h_icc_c)) {
  {
    USI opval = i_disp22;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} else {
SEM_ANNUL_INSN (current_cpu, pc, f_a);
}
} while (0);

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_V9_DEPRECATED-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* tcs: tcs $rs1,$rs2 */

SEM_STATUS
SEM_FN_NAME (sparc64,tcs) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_TA_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_CODE

if (CPU (h_icc_c)) {
  {
    USI opval = sparc64_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), GET_H_GR (f_rs2));
;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 4);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_TRAP-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* tcs-imm: tcs $rs1,$simm13 */

SEM_STATUS
SEM_FN_NAME (sparc64,tcs_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_TA_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_IMM_CODE

if (CPU (h_icc_c)) {
  {
    USI opval = sparc64_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), f_simm13);
;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 4);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_TRAP-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* bpos: bpos$a $disp22 */

SEM_STATUS
SEM_FN_NAME (sparc64,bpos) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_BA_VARS /* f-disp22 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BA_CODE
  i_disp22 = f_disp22;

do {
if (NOTBI (CPU (h_icc_n))) {
  {
    USI opval = i_disp22;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} else {
SEM_ANNUL_INSN (current_cpu, pc, f_a);
}
} while (0);

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_V9_DEPRECATED-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* tpos: tpos $rs1,$rs2 */

SEM_STATUS
SEM_FN_NAME (sparc64,tpos) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_TA_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_CODE

if (NOTBI (CPU (h_icc_n))) {
  {
    USI opval = sparc64_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), GET_H_GR (f_rs2));
;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 4);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_TRAP-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* tpos-imm: tpos $rs1,$simm13 */

SEM_STATUS
SEM_FN_NAME (sparc64,tpos_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_TA_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_IMM_CODE

if (NOTBI (CPU (h_icc_n))) {
  {
    USI opval = sparc64_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), f_simm13);
;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 4);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_TRAP-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* bneg: bneg$a $disp22 */

SEM_STATUS
SEM_FN_NAME (sparc64,bneg) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_BA_VARS /* f-disp22 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BA_CODE
  i_disp22 = f_disp22;

do {
if (CPU (h_icc_n)) {
  {
    USI opval = i_disp22;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} else {
SEM_ANNUL_INSN (current_cpu, pc, f_a);
}
} while (0);

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_V9_DEPRECATED-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* tneg: tneg $rs1,$rs2 */

SEM_STATUS
SEM_FN_NAME (sparc64,tneg) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_TA_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_CODE

if (CPU (h_icc_n)) {
  {
    USI opval = sparc64_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), GET_H_GR (f_rs2));
;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 4);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_TRAP-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* tneg-imm: tneg $rs1,$simm13 */

SEM_STATUS
SEM_FN_NAME (sparc64,tneg_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_TA_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_IMM_CODE

if (CPU (h_icc_n)) {
  {
    USI opval = sparc64_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), f_simm13);
;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 4);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_TRAP-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* bvc: bvc$a $disp22 */

SEM_STATUS
SEM_FN_NAME (sparc64,bvc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_BA_VARS /* f-disp22 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BA_CODE
  i_disp22 = f_disp22;

do {
if (NOTBI (CPU (h_icc_v))) {
  {
    USI opval = i_disp22;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} else {
SEM_ANNUL_INSN (current_cpu, pc, f_a);
}
} while (0);

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_V9_DEPRECATED-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* tvc: tvc $rs1,$rs2 */

SEM_STATUS
SEM_FN_NAME (sparc64,tvc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_TA_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_CODE

if (NOTBI (CPU (h_icc_v))) {
  {
    USI opval = sparc64_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), GET_H_GR (f_rs2));
;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 4);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_TRAP-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* tvc-imm: tvc $rs1,$simm13 */

SEM_STATUS
SEM_FN_NAME (sparc64,tvc_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_TA_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_IMM_CODE

if (NOTBI (CPU (h_icc_v))) {
  {
    USI opval = sparc64_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), f_simm13);
;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 4);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_TRAP-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* bvs: bvs$a $disp22 */

SEM_STATUS
SEM_FN_NAME (sparc64,bvs) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_BA_VARS /* f-disp22 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BA_CODE
  i_disp22 = f_disp22;

do {
if (CPU (h_icc_v)) {
  {
    USI opval = i_disp22;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 3);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} else {
SEM_ANNUL_INSN (current_cpu, pc, f_a);
}
} while (0);

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_V9_DEPRECATED-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* tvs: tvs $rs1,$rs2 */

SEM_STATUS
SEM_FN_NAME (sparc64,tvs) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_TA_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_CODE

if (CPU (h_icc_v)) {
  {
    USI opval = sparc64_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), GET_H_GR (f_rs2));
;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 4);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_TRAP-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* tvs-imm: tvs $rs1,$simm13 */

SEM_STATUS
SEM_FN_NAME (sparc64,tvs_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_TA_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_IMM_CODE

if (CPU (h_icc_v)) {
  {
    USI opval = sparc64_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), f_simm13);
;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    written |= (1 << 4);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
}

  abuf->written = written;
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_TRAP-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* ldstub-reg+reg: ldstub [$rs1+$rs2],$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,ldstub_reg_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE

sparc64_do_ldstub (current_cpu, pc, f_rd, GET_H_GR (f_rs1), GET_H_GR (f_rs2), -1);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* ldstub-reg+imm: ldstub [$rs1+$simm13],$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,ldstub_reg_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE

sparc64_do_ldstub (current_cpu, pc, f_rd, GET_H_GR (f_rs1), f_simm13, -1);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* ldstub-reg+reg/asi: ldstub [$rs1+$rs2]$asi,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,ldstub_reg_reg_asi) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_ASI_CODE

sparc64_do_ldstub (current_cpu, pc, f_rd, GET_H_GR (f_rs1), GET_H_GR (f_rs2), f_asi);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* swap-reg+reg: swap [$rs1+$rs2],$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,swap_reg_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE

sparc64_do_swap (current_cpu, pc, f_rd, GET_H_GR (f_rs1), GET_H_GR (f_rs2), -1);

  SEM_NBRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_V9_DEPRECATED-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* swap-reg+imm: swap [$rs1+$simm13],$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,swap_reg_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE

sparc64_do_swap (current_cpu, pc, f_rd, GET_H_GR (f_rs1), f_simm13, -1);

  SEM_NBRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_V9_DEPRECATED-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* swap-reg+reg/asi: swap [$rs1+$rs2]$asi,$rd */

SEM_STATUS
SEM_FN_NAME (sparc64,swap_reg_reg_asi) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSB_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_ASI_CODE

sparc64_do_swap (current_cpu, pc, f_rd, GET_H_GR (f_rs1), GET_H_GR (f_rs2), f_asi);

  SEM_NBRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_V9_DEPRECATED-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

