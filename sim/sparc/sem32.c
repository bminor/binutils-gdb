/* Simulator instruction semantics for sparc32.

THIS FILE IS MACHINE GENERATED WITH CGEN.

Copyright (C) 1999 Cygnus Solutions, Inc.

This file is part of the Cygnus Simulators.


*/

#define WANT_CPU sparc32
#define WANT_CPU_SPARC32

#include "sim-main.h"
#include "cgen-mem.h"
#include "cgen-ops.h"

#undef GET_ATTR
#define GET_ATTR(cpu, num, attr) CGEN_ATTR_VALUE (NULL, abuf->idesc->attrs, CGEN_INSN_##attr)

/* x-invalid: --invalid-- */

SEM_STATUS
SEM_FN_NAME (sparc32,x_invalid) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
SEM_FN_NAME (sparc32,x_after) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 0);
  EXTRACT_IFMT_EMPTY_VARS /* */
  EXTRACT_IFMT_EMPTY_CODE

  {
#if WITH_SCACHE_PBB_SPARC32
    sparc32_pbb_after (current_cpu, sem_arg);
#endif
  }

  SEM_NBRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_PBB-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_VIRTUAL-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* x-before: --before-- */

SEM_STATUS
SEM_FN_NAME (sparc32,x_before) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 0);
  EXTRACT_IFMT_EMPTY_VARS /* */
  EXTRACT_IFMT_EMPTY_CODE

  {
#if WITH_SCACHE_PBB_SPARC32
    sparc32_pbb_before (current_cpu, sem_arg);
#endif
  }

  SEM_NBRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_PBB-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_VIRTUAL-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* x-cti-chain: --cti-chain-- */

SEM_STATUS
SEM_FN_NAME (sparc32,x_cti_chain) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 0);
  EXTRACT_IFMT_EMPTY_VARS /* */
  EXTRACT_IFMT_EMPTY_CODE

  {
#if WITH_SCACHE_PBB_SPARC32
#ifdef DEFINE_SWITCH
    vpc = sparc32_pbb_cti_chain (current_cpu, sem_arg,
			       pbb_br_npc_ptr, pbb_br_npc);
    BREAK (sem);
#else
    /* FIXME: Allow provision of explicit ifmt spec in insn spec.  */
    vpc = sparc32_pbb_cti_chain (current_cpu, sem_arg,
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
SEM_FN_NAME (sparc32,x_chain) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 0);
  EXTRACT_IFMT_EMPTY_VARS /* */
  EXTRACT_IFMT_EMPTY_CODE

  {
#if WITH_SCACHE_PBB_SPARC32
    vpc = sparc32_pbb_chain (current_cpu, sem_arg);
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
SEM_FN_NAME (sparc32,x_begin) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 0);
  EXTRACT_IFMT_EMPTY_VARS /* */
  EXTRACT_IFMT_EMPTY_CODE

  {
#if WITH_SCACHE_PBB_SPARC32
#ifdef DEFINE_SWITCH
    /* In the switch case FAST_P is a constant, allowing several optimizations
       in any called inline functions.  */
    vpc = sparc32_pbb_begin (current_cpu, FAST_P);
#else
    vpc = sparc32_pbb_begin (current_cpu, STATE_RUN_FAST_P (CPU_STATE (current_cpu)));
#endif
#endif
  }

  SEM_NBRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_PBB-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_VIRTUAL-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* rd-asr: rd $rdasr,$rd */

SEM_STATUS
SEM_FN_NAME (sparc32,rd_asr) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_RD_ASR_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_RD_ASR_CODE

  {
    SI opval = CPU (h_asr[f_rs1]);
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* wr-asr: wr $rs1,$rs2,$wrasr */

SEM_STATUS
SEM_FN_NAME (sparc32,wr_asr) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_WR_ASR_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_WR_ASR_CODE

  {
    SI opval = XORSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2));
    CPU (h_asr[f_rd]) = opval;
    TRACE_RESULT (current_cpu, abuf, "wrasr", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* wr-asr-imm: wr $rs1,$simm13,$wrasr */

SEM_STATUS
SEM_FN_NAME (sparc32,wr_asr_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_WR_ASR_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_WR_ASR_IMM_CODE

  {
    SI opval = XORSI (GET_H_GR (f_rs1), f_simm13);
    CPU (h_asr[f_rd]) = opval;
    TRACE_RESULT (current_cpu, abuf, "wrasr", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* rd-psr: rd %psr,$rd */

SEM_STATUS
SEM_FN_NAME (sparc32,rd_psr) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_RD_PSR_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_RD_PSR_CODE

  {
    SI opval = GET_H_PSR ();
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* wr-psr: wr $rs1,$rs2,%psr */

SEM_STATUS
SEM_FN_NAME (sparc32,wr_psr) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_WR_PSR_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_WR_PSR_CODE

  {
    SI opval = XORSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2));
    SET_H_PSR (opval);
    TRACE_RESULT (current_cpu, abuf, "psr-0", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* wr-psr-imm: wr $rs1,$simm13,%psr */

SEM_STATUS
SEM_FN_NAME (sparc32,wr_psr_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_WR_PSR_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_WR_PSR_IMM_CODE

  {
    SI opval = XORSI (GET_H_GR (f_rs1), f_simm13);
    SET_H_PSR (opval);
    TRACE_RESULT (current_cpu, abuf, "psr-0", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* rd-wim: rd %wim,$rd */

SEM_STATUS
SEM_FN_NAME (sparc32,rd_wim) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_RD_PSR_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_RD_PSR_CODE

  {
    SI opval = GET_H_WIM ();
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* wr-wim: wr $rs1,$rs2,%wim */

SEM_STATUS
SEM_FN_NAME (sparc32,wr_wim) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_WR_PSR_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_WR_PSR_CODE

  {
    SI opval = XORSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2));
    SET_H_WIM (opval);
    TRACE_RESULT (current_cpu, abuf, "wim-0", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* wr-wim-imm: wr $rs1,$simm13,%wim */

SEM_STATUS
SEM_FN_NAME (sparc32,wr_wim_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_WR_PSR_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_WR_PSR_IMM_CODE

  {
    SI opval = XORSI (GET_H_GR (f_rs1), f_simm13);
    SET_H_WIM (opval);
    TRACE_RESULT (current_cpu, abuf, "wim-0", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* rd-tbr: rd %tbr,$rd */

SEM_STATUS
SEM_FN_NAME (sparc32,rd_tbr) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_RD_PSR_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_RD_PSR_CODE

  {
    SI opval = GET_H_TBR ();
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* wr-tbr: wr $rs1,$rs2,%tbr */

SEM_STATUS
SEM_FN_NAME (sparc32,wr_tbr) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_WR_PSR_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_WR_PSR_CODE

  {
    SI opval = XORSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2));
    SET_H_TBR (opval);
    TRACE_RESULT (current_cpu, abuf, "tbr-0", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* wr-tbr-imm: wr $rs1,$simm13,%tbr */

SEM_STATUS
SEM_FN_NAME (sparc32,wr_tbr_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_WR_PSR_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_WR_PSR_IMM_CODE

  {
    SI opval = XORSI (GET_H_GR (f_rs1), f_simm13);
    SET_H_TBR (opval);
    TRACE_RESULT (current_cpu, abuf, "tbr-0", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* ldstub-reg+reg: ldstub [$rs1+$rs2],$rd */

SEM_STATUS
SEM_FN_NAME (sparc32,ldstub_reg_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE

sparc32_do_ldstub (current_cpu, pc, f_rd, GET_H_GR (f_rs1), GET_H_GR (f_rs2), -1);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* ldstub-reg+imm: ldstub [$rs1+$simm13],$rd */

SEM_STATUS
SEM_FN_NAME (sparc32,ldstub_reg_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE

sparc32_do_ldstub (current_cpu, pc, f_rd, GET_H_GR (f_rs1), f_simm13, -1);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* ldstub-reg+reg/asi: ldstub [$rs1+$rs2]$asi,$rd */

SEM_STATUS
SEM_FN_NAME (sparc32,ldstub_reg_reg_asi) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_ASI_CODE

sparc32_do_ldstub (current_cpu, pc, f_rd, GET_H_GR (f_rs1), GET_H_GR (f_rs2), f_asi);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* swap-reg+reg: swap [$rs1+$rs2],$rd */

SEM_STATUS
SEM_FN_NAME (sparc32,swap_reg_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE

sparc32_do_swap (current_cpu, pc, f_rd, GET_H_GR (f_rs1), GET_H_GR (f_rs2), -1);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* swap-reg+imm: swap [$rs1+$simm13],$rd */

SEM_STATUS
SEM_FN_NAME (sparc32,swap_reg_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE

sparc32_do_swap (current_cpu, pc, f_rd, GET_H_GR (f_rs1), f_simm13, -1);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* swap-reg+reg/asi: swap [$rs1+$rs2]$asi,$rd */

SEM_STATUS
SEM_FN_NAME (sparc32,swap_reg_reg_asi) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_ASI_CODE

sparc32_do_swap (current_cpu, pc, f_rd, GET_H_GR (f_rs1), GET_H_GR (f_rs2), f_asi);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* ldsb-reg+reg: ldsb [$rs1+$rs2],$rd */

SEM_STATUS
SEM_FN_NAME (sparc32,ldsb_reg_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE

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
SEM_FN_NAME (sparc32,ldsb_reg_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE

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
SEM_FN_NAME (sparc32,ldsb_reg_reg_asi) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_ASI_CODE

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
SEM_FN_NAME (sparc32,ldub_reg_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE

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
SEM_FN_NAME (sparc32,ldub_reg_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE

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
SEM_FN_NAME (sparc32,ldub_reg_reg_asi) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_ASI_CODE

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
SEM_FN_NAME (sparc32,ldsh_reg_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE

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
SEM_FN_NAME (sparc32,ldsh_reg_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE

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
SEM_FN_NAME (sparc32,ldsh_reg_reg_asi) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_ASI_CODE

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
SEM_FN_NAME (sparc32,lduh_reg_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE

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
SEM_FN_NAME (sparc32,lduh_reg_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE

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
SEM_FN_NAME (sparc32,lduh_reg_reg_asi) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_ASI_CODE

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
SEM_FN_NAME (sparc32,ldsw_reg_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE

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
SEM_FN_NAME (sparc32,ldsw_reg_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE

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
SEM_FN_NAME (sparc32,ldsw_reg_reg_asi) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_ASI_CODE

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
SEM_FN_NAME (sparc32,lduw_reg_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE

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
SEM_FN_NAME (sparc32,lduw_reg_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE

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
SEM_FN_NAME (sparc32,lduw_reg_reg_asi) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_ASI_CODE

  {
    SI opval = GETMEMSI (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2)));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* ldd-reg+reg: ldd [$rs1+$rs2],$rdd */

SEM_STATUS
SEM_FN_NAME (sparc32,ldd_reg_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
SEM_FN_NAME (sparc32,ldd_reg_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
SEM_FN_NAME (sparc32,ldd_reg_reg_asi) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
SEM_FN_NAME (sparc32,stb_reg_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE

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
SEM_FN_NAME (sparc32,stb_reg_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE

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
SEM_FN_NAME (sparc32,stb_reg_reg_asi) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_ASI_CODE

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
SEM_FN_NAME (sparc32,sth_reg_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE

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
SEM_FN_NAME (sparc32,sth_reg_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE

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
SEM_FN_NAME (sparc32,sth_reg_reg_asi) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_ASI_CODE

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
SEM_FN_NAME (sparc32,st_reg_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE

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
SEM_FN_NAME (sparc32,st_reg_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE

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
SEM_FN_NAME (sparc32,st_reg_reg_asi) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_ASI_CODE

  {
    SI opval = GET_H_GR (f_rd);
    SETMEMSI (current_cpu, pc, ADDSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2)), opval);
    TRACE_RESULT (current_cpu, abuf, "memory", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* std-reg+reg: std $rdd,[$rs1+$rs2] */

SEM_STATUS
SEM_FN_NAME (sparc32,std_reg_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
SEM_FN_NAME (sparc32,std_reg_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
SEM_FN_NAME (sparc32,std_reg_reg_asi) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
SEM_FN_NAME (sparc32,fp_ld_reg_reg) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_FP_LD_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_FP_LD_REG_REG_CODE

do {
sparc32_hw_trap (current_cpu, pc, TRAP32_FP_DIS);
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
SEM_FN_NAME (sparc32,fp_ld_reg_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_FP_LD_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_FP_LD_REG_IMM_CODE

do {
sparc32_hw_trap (current_cpu, pc, TRAP32_FP_DIS);
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
SEM_FN_NAME (sparc32,fp_ld_reg_reg_asi) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_FP_LD_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_FP_LD_REG_REG_ASI_CODE

do {
sparc32_hw_trap (current_cpu, pc, TRAP32_FP_DIS);
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
SEM_FN_NAME (sparc32,sethi) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
SEM_FN_NAME (sparc32,add) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE

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
SEM_FN_NAME (sparc32,add_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE

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
SEM_FN_NAME (sparc32,sub) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE

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
SEM_FN_NAME (sparc32,sub_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE

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
SEM_FN_NAME (sparc32,addcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE

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
SEM_FN_NAME (sparc32,addcc_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE

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
SEM_FN_NAME (sparc32,subcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE

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
SEM_FN_NAME (sparc32,subcc_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE

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

/* addx: addx $rs1,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc32,addx) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE

  {
    SI opval = ADDCSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2), CPU (h_icc_c));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* addx-imm: addx $rs1,$simm13,$rd */

SEM_STATUS
SEM_FN_NAME (sparc32,addx_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE

  {
    SI opval = ADDCSI (GET_H_GR (f_rs1), f_simm13, CPU (h_icc_c));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* subx: subx $rs1,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc32,subx) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE

  {
    SI opval = SUBCSI (GET_H_GR (f_rs1), GET_H_GR (f_rs2), CPU (h_icc_c));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* subx-imm: subx $rs1,$simm13,$rd */

SEM_STATUS
SEM_FN_NAME (sparc32,subx_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE

  {
    SI opval = SUBCSI (GET_H_GR (f_rs1), f_simm13, CPU (h_icc_c));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* addxcc: addxcc $rs1,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc32,addxcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE

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

/* addxcc-imm: addxcc $rs1,$simm13,$rd */

SEM_STATUS
SEM_FN_NAME (sparc32,addxcc_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE

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

/* subxcc: subxcc $rs1,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc32,subxcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE

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

/* subxcc-imm: subxcc $rs1,$simm13,$rd */

SEM_STATUS
SEM_FN_NAME (sparc32,subxcc_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE

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
SEM_FN_NAME (sparc32,and) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE

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
SEM_FN_NAME (sparc32,and_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE

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
SEM_FN_NAME (sparc32,andcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE

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
SEM_FN_NAME (sparc32,andcc_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE

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
SEM_FN_NAME (sparc32,or) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE

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
SEM_FN_NAME (sparc32,or_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE

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
SEM_FN_NAME (sparc32,orcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE

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
SEM_FN_NAME (sparc32,orcc_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE

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
SEM_FN_NAME (sparc32,xor) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE

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
SEM_FN_NAME (sparc32,xor_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE

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
SEM_FN_NAME (sparc32,xorcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE

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
SEM_FN_NAME (sparc32,xorcc_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE

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
SEM_FN_NAME (sparc32,andn) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE

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
SEM_FN_NAME (sparc32,andn_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE

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
SEM_FN_NAME (sparc32,andncc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE

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
SEM_FN_NAME (sparc32,andncc_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE

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
SEM_FN_NAME (sparc32,orn) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE

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
SEM_FN_NAME (sparc32,orn_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE

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
SEM_FN_NAME (sparc32,orncc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE

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
SEM_FN_NAME (sparc32,orncc_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE

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
SEM_FN_NAME (sparc32,xnor) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE

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
SEM_FN_NAME (sparc32,xnor_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE

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
SEM_FN_NAME (sparc32,xnorcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE

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
SEM_FN_NAME (sparc32,xnorcc_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE

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
SEM_FN_NAME (sparc32,sll) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE

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
SEM_FN_NAME (sparc32,sll_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE

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
SEM_FN_NAME (sparc32,srl) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE

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
SEM_FN_NAME (sparc32,srl_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE

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
SEM_FN_NAME (sparc32,sra) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE

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
SEM_FN_NAME (sparc32,sra_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE

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
SEM_FN_NAME (sparc32,smul) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE

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
SEM_FN_NAME (sparc32,smul_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE

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
SEM_FN_NAME (sparc32,smul_cc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE

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
SEM_FN_NAME (sparc32,smul_cc_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE

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
SEM_FN_NAME (sparc32,umul) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE

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
SEM_FN_NAME (sparc32,umul_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE

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
SEM_FN_NAME (sparc32,umul_cc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE

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
SEM_FN_NAME (sparc32,umul_cc_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE

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

/* sdiv: sdiv $rs1,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc32,sdiv) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE

do {
  DI tmp_dividend;
  tmp_dividend = MAKEDI (GET_H_Y (), GET_H_GR (f_rs1));
  {
    SI opval = TRUNCDISI (DIVDI (tmp_dividend, EXTSIDI (GET_H_GR (f_rs2))));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
} while (0);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* sdiv-imm: sdiv $rs1,$simm13,$rd */

SEM_STATUS
SEM_FN_NAME (sparc32,sdiv_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE

do {
  DI tmp_dividend;
  tmp_dividend = MAKEDI (GET_H_Y (), GET_H_GR (f_rs1));
  {
    SI opval = TRUNCDISI (DIVDI (tmp_dividend, EXTHIDI (f_simm13)));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
} while (0);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* sdiv-cc: sdivcc $rs1,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc32,sdiv_cc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE

do {
  DI tmp_dividend;
  tmp_dividend = MAKEDI (GET_H_Y (), GET_H_GR (f_rs1));
  {
    SI opval = TRUNCDISI (DIVDI (tmp_dividend, EXTSIDI (GET_H_GR (f_rs2))));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
do {
  {
    BI opval = EQSI (GET_H_GR (f_rd), 0);
    CPU (h_icc_z) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-z", 'x', opval);
  }
  {
    BI opval = LTSI (GET_H_GR (f_rd), 0);
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

/* sdiv-cc-imm: sdivcc $rs1,$simm13,$rd */

SEM_STATUS
SEM_FN_NAME (sparc32,sdiv_cc_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE

do {
  DI tmp_dividend;
  tmp_dividend = MAKEDI (GET_H_Y (), GET_H_GR (f_rs1));
  {
    SI opval = TRUNCDISI (DIVDI (tmp_dividend, EXTHIDI (f_simm13)));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
do {
  {
    BI opval = EQSI (GET_H_GR (f_rd), 0);
    CPU (h_icc_z) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-z", 'x', opval);
  }
  {
    BI opval = LTSI (GET_H_GR (f_rd), 0);
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

/* udiv: udiv $rs1,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc32,udiv) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE

do {
  DI tmp_dividend;
  tmp_dividend = MAKEDI (GET_H_Y (), GET_H_GR (f_rs1));
  {
    SI opval = TRUNCDISI (DIVDI (tmp_dividend, ZEXTSIDI (GET_H_GR (f_rs2))));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
} while (0);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* udiv-imm: udiv $rs1,$simm13,$rd */

SEM_STATUS
SEM_FN_NAME (sparc32,udiv_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE

do {
  DI tmp_dividend;
  tmp_dividend = MAKEDI (GET_H_Y (), GET_H_GR (f_rs1));
  {
    SI opval = TRUNCDISI (DIVDI (tmp_dividend, ZEXTHIDI (f_simm13)));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
} while (0);

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* udiv-cc: udivcc $rs1,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc32,udiv_cc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE

do {
  DI tmp_dividend;
  tmp_dividend = MAKEDI (GET_H_Y (), GET_H_GR (f_rs1));
  {
    SI opval = TRUNCDISI (DIVDI (tmp_dividend, ZEXTSIDI (GET_H_GR (f_rs2))));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
do {
  {
    BI opval = EQSI (GET_H_GR (f_rd), 0);
    CPU (h_icc_z) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-z", 'x', opval);
  }
  {
    BI opval = LTSI (GET_H_GR (f_rd), 0);
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

/* udiv-cc-imm: udivcc $rs1,$simm13,$rd */

SEM_STATUS
SEM_FN_NAME (sparc32,udiv_cc_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE

do {
  DI tmp_dividend;
  tmp_dividend = MAKEDI (GET_H_Y (), GET_H_GR (f_rs1));
  {
    SI opval = TRUNCDISI (DIVDI (tmp_dividend, ZEXTHIDI (f_simm13)));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }
do {
  {
    BI opval = EQSI (GET_H_GR (f_rd), 0);
    CPU (h_icc_z) = opval;
    TRACE_RESULT (current_cpu, abuf, "icc-z", 'x', opval);
  }
  {
    BI opval = LTSI (GET_H_GR (f_rd), 0);
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
SEM_FN_NAME (sparc32,mulscc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE

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
SEM_FN_NAME (sparc32,save) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE

  {
    SI opval = sparc32_do_save (current_cpu, pc, GET_H_GR (f_rs1), GET_H_GR (f_rs2));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* save-imm: save $rs1,$simm13,$rd */

SEM_STATUS
SEM_FN_NAME (sparc32,save_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE

  {
    SI opval = sparc32_do_save (current_cpu, pc, GET_H_GR (f_rs1), f_simm13);
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* restore: restore $rs1,$rs2,$rd */

SEM_STATUS
SEM_FN_NAME (sparc32,restore) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE

  {
    SI opval = sparc32_do_restore (current_cpu, pc, GET_H_GR (f_rs1), GET_H_GR (f_rs2));
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* restore-imm: restore $rs1,$simm13,$rd */

SEM_STATUS
SEM_FN_NAME (sparc32,restore_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE

  {
    SI opval = sparc32_do_restore (current_cpu, pc, GET_H_GR (f_rs1), f_simm13);
    SET_H_GR (f_rd, opval);
    TRACE_RESULT (current_cpu, abuf, "rd", 'x', opval);
  }

  SEM_NBRANCH_FINI (vpc, 0);
  return status;
}

/* rett: rett $rs1,$rs2 */

SEM_STATUS
SEM_FN_NAME (sparc32,rett) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_WR_PSR_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_WR_PSR_CODE

do {
  {
    USI opval = sparc32_do_rett (current_cpu, pc, GET_H_GR (f_rs1), GET_H_GR (f_rs2));
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} while (0);

  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_UNCOND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* rett-imm: rett $rs1,$simm13 */

SEM_STATUS
SEM_FN_NAME (sparc32,rett_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_WR_PSR_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_WR_PSR_IMM_CODE

do {
  {
    USI opval = sparc32_do_rett (current_cpu, pc, GET_H_GR (f_rs1), f_simm13);
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }
} while (0);

  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_UNCOND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* unimp: unimp $imm22 */

SEM_STATUS
SEM_FN_NAME (sparc32,unimp) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
SEM_FN_NAME (sparc32,call) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
SEM_FN_NAME (sparc32,jmpl) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE

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
SEM_FN_NAME (sparc32,jmpl_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
{
  SEM_STATUS status = 0;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int UNUSED written = 0;
  IADDR UNUSED pc = GET_H_PC ();
  SEM_BRANCH_INIT
  SEM_PC vpc = SEM_NEXT_VPC (sem_arg, pc, 4);
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE

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
SEM_FN_NAME (sparc32,ba) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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

  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_V9_DEPRECATED-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_ANNUL-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_UNCOND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* ta: ta $rs1,$rs2 */

SEM_STATUS
SEM_FN_NAME (sparc32,ta) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
    USI opval = sparc32_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), GET_H_GR (f_rs2));
;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }

  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_TRAP-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_UNCOND_CTI-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* ta-imm: ta $rs1,$simm13 */

SEM_STATUS
SEM_FN_NAME (sparc32,ta_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
    USI opval = sparc32_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), f_simm13);
;
    SEM_BRANCH_VIA_ADDR (current_cpu, sem_arg, opval, vpc);
    TRACE_RESULT (current_cpu, abuf, "pc", 'x', opval);
  }

  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_TRAP-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_UNCOND_CTI-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* bn: bn$a $disp22 */

SEM_STATUS
SEM_FN_NAME (sparc32,bn) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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

  SEM_NBRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_V9_DEPRECATED-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_ANNUL-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* tn: tn $rs1,$rs2 */

SEM_STATUS
SEM_FN_NAME (sparc32,tn) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
SEM_FN_NAME (sparc32,tn_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
SEM_FN_NAME (sparc32,bne) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_V9_DEPRECATED-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_ANNUL-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* tne: tne $rs1,$rs2 */

SEM_STATUS
SEM_FN_NAME (sparc32,tne) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
    USI opval = sparc32_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), GET_H_GR (f_rs2));
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
SEM_FN_NAME (sparc32,tne_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
    USI opval = sparc32_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), f_simm13);
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
SEM_FN_NAME (sparc32,be) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_V9_DEPRECATED-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_ANNUL-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* te: te $rs1,$rs2 */

SEM_STATUS
SEM_FN_NAME (sparc32,te) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
    USI opval = sparc32_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), GET_H_GR (f_rs2));
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
SEM_FN_NAME (sparc32,te_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
    USI opval = sparc32_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), f_simm13);
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
SEM_FN_NAME (sparc32,bg) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_V9_DEPRECATED-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_ANNUL-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* tg: tg $rs1,$rs2 */

SEM_STATUS
SEM_FN_NAME (sparc32,tg) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
    USI opval = sparc32_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), GET_H_GR (f_rs2));
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
SEM_FN_NAME (sparc32,tg_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
    USI opval = sparc32_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), f_simm13);
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
SEM_FN_NAME (sparc32,ble) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_V9_DEPRECATED-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_ANNUL-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* tle: tle $rs1,$rs2 */

SEM_STATUS
SEM_FN_NAME (sparc32,tle) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
    USI opval = sparc32_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), GET_H_GR (f_rs2));
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
SEM_FN_NAME (sparc32,tle_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
    USI opval = sparc32_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), f_simm13);
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
SEM_FN_NAME (sparc32,bge) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_V9_DEPRECATED-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_ANNUL-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* tge: tge $rs1,$rs2 */

SEM_STATUS
SEM_FN_NAME (sparc32,tge) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
    USI opval = sparc32_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), GET_H_GR (f_rs2));
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
SEM_FN_NAME (sparc32,tge_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
    USI opval = sparc32_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), f_simm13);
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
SEM_FN_NAME (sparc32,bl) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_V9_DEPRECATED-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_ANNUL-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* tl: tl $rs1,$rs2 */

SEM_STATUS
SEM_FN_NAME (sparc32,tl) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
    USI opval = sparc32_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), GET_H_GR (f_rs2));
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
SEM_FN_NAME (sparc32,tl_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
    USI opval = sparc32_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), f_simm13);
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
SEM_FN_NAME (sparc32,bgu) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_V9_DEPRECATED-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_ANNUL-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* tgu: tgu $rs1,$rs2 */

SEM_STATUS
SEM_FN_NAME (sparc32,tgu) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
    USI opval = sparc32_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), GET_H_GR (f_rs2));
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
SEM_FN_NAME (sparc32,tgu_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
    USI opval = sparc32_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), f_simm13);
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
SEM_FN_NAME (sparc32,bleu) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_V9_DEPRECATED-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_ANNUL-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* tleu: tleu $rs1,$rs2 */

SEM_STATUS
SEM_FN_NAME (sparc32,tleu) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
    USI opval = sparc32_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), GET_H_GR (f_rs2));
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
SEM_FN_NAME (sparc32,tleu_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
    USI opval = sparc32_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), f_simm13);
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
SEM_FN_NAME (sparc32,bcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_V9_DEPRECATED-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_ANNUL-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* tcc: tcc $rs1,$rs2 */

SEM_STATUS
SEM_FN_NAME (sparc32,tcc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
    USI opval = sparc32_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), GET_H_GR (f_rs2));
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
SEM_FN_NAME (sparc32,tcc_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
    USI opval = sparc32_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), f_simm13);
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
SEM_FN_NAME (sparc32,bcs) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_V9_DEPRECATED-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_ANNUL-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* tcs: tcs $rs1,$rs2 */

SEM_STATUS
SEM_FN_NAME (sparc32,tcs) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
    USI opval = sparc32_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), GET_H_GR (f_rs2));
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
SEM_FN_NAME (sparc32,tcs_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
    USI opval = sparc32_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), f_simm13);
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
SEM_FN_NAME (sparc32,bpos) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_V9_DEPRECATED-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_ANNUL-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* tpos: tpos $rs1,$rs2 */

SEM_STATUS
SEM_FN_NAME (sparc32,tpos) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
    USI opval = sparc32_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), GET_H_GR (f_rs2));
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
SEM_FN_NAME (sparc32,tpos_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
    USI opval = sparc32_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), f_simm13);
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
SEM_FN_NAME (sparc32,bneg) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_V9_DEPRECATED-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_ANNUL-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* tneg: tneg $rs1,$rs2 */

SEM_STATUS
SEM_FN_NAME (sparc32,tneg) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
    USI opval = sparc32_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), GET_H_GR (f_rs2));
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
SEM_FN_NAME (sparc32,tneg_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
    USI opval = sparc32_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), f_simm13);
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
SEM_FN_NAME (sparc32,bvc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_V9_DEPRECATED-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_ANNUL-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* tvc: tvc $rs1,$rs2 */

SEM_STATUS
SEM_FN_NAME (sparc32,tvc) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
    USI opval = sparc32_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), GET_H_GR (f_rs2));
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
SEM_FN_NAME (sparc32,tvc_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
    USI opval = sparc32_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), f_simm13);
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
SEM_FN_NAME (sparc32,bvs) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
  SEM_BRANCH_FINI (vpc, 0|(1<<(CGEN_INSN_V9_DEPRECATED-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_ANNUL-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_COND_CTI-CGEN_ATTR_BOOL_OFFSET))|(1<<(CGEN_INSN_DELAY_SLOT-CGEN_ATTR_BOOL_OFFSET)));
  return status;
}

/* tvs: tvs $rs1,$rs2 */

SEM_STATUS
SEM_FN_NAME (sparc32,tvs) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
    USI opval = sparc32_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), GET_H_GR (f_rs2));
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
SEM_FN_NAME (sparc32,tvs_imm) (SIM_CPU *current_cpu, SEM_ARG sem_arg, CGEN_INSN_INT insn)
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
    USI opval = sparc32_sw_trap (current_cpu, pc, GET_H_GR (f_rs1), f_simm13);
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

