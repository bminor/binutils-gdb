/* Simulator model support for sparc32.

THIS FILE IS MACHINE GENERATED WITH CGEN.

Copyright (C) 1999 Cygnus Solutions, Inc.

This file is part of the Cygnus Simulators.


*/

#define WANT_CPU sparc32
#define WANT_CPU_SPARC32

#include "sim-main.h"

/* The profiling data is recorded here, but is accessed via the profiling
   mechanism.  After all, this is information for profiling.  */

#if WITH_PROFILE_MODEL_P

/* Model handlers for each insn.  */

static int
model_sparc32_def_rd_asr (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_RD_ASR_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_RD_ASR_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_wr_asr (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_WR_ASR_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_WR_ASR_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_wr_asr_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_WR_ASR_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_WR_ASR_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_rd_psr (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_RD_PSR_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_RD_PSR_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_wr_psr (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_WR_PSR_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_WR_PSR_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_wr_psr_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_WR_PSR_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_WR_PSR_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_rd_wim (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_RD_PSR_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_RD_PSR_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_wr_wim (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_WR_PSR_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_WR_PSR_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_wr_wim_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_WR_PSR_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_WR_PSR_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_rd_tbr (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_RD_PSR_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_RD_PSR_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_wr_tbr (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_WR_PSR_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_WR_PSR_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_wr_tbr_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_WR_PSR_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_WR_PSR_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_ldstub_reg_reg (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_ldstub_reg_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_ldstub_reg_reg_asi (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_ASI_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_swap_reg_reg (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_swap_reg_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_swap_reg_reg_asi (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_ASI_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_ldsb_reg_reg (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_ldsb_reg_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_ldsb_reg_reg_asi (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_ASI_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_ldub_reg_reg (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_ldub_reg_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_ldub_reg_reg_asi (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_ASI_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_ldsh_reg_reg (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_ldsh_reg_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_ldsh_reg_reg_asi (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_ASI_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_lduh_reg_reg (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_lduh_reg_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_lduh_reg_reg_asi (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_ASI_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_ldsw_reg_reg (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_ldsw_reg_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_ldsw_reg_reg_asi (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_ASI_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_lduw_reg_reg (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_lduw_reg_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_lduw_reg_reg_asi (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_ASI_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_ldd_reg_reg (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDD_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDD_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_ldd_reg_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDD_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDD_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_ldd_reg_reg_asi (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDD_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDD_REG_REG_ASI_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_stb_reg_reg (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_stb_reg_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_stb_reg_reg_asi (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_ASI_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_sth_reg_reg (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_sth_reg_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_sth_reg_reg_asi (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_ASI_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_st_reg_reg (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_st_reg_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_st_reg_reg_asi (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_ASI_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_std_reg_reg (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDD_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDD_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_std_reg_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDD_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDD_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_std_reg_reg_asi (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDD_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDD_REG_REG_ASI_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_fp_ld_reg_reg (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_FP_LD_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_FP_LD_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_fp_ld_reg_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_FP_LD_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_FP_LD_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_fp_ld_reg_reg_asi (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_FP_LD_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_FP_LD_REG_REG_ASI_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_sethi (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_SETHI_VARS /* f-hi22 f-op2 f-rd f-op */
  EXTRACT_IFMT_SETHI_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_add (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_add_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_sub (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_sub_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_addcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_addcc_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_subcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_subcc_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_addx (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_addx_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_subx (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_subx_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_addxcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_addxcc_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_subxcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_subxcc_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_and (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_and_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_andcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_andcc_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_or (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_or_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_orcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_orcc_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_xor (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_xor_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_xorcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_xorcc_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_andn (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_andn_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_andncc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_andncc_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_orn (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_orn_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_orncc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_orncc_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_xnor (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_xnor_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_xnorcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_xnorcc_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_sll (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_sll_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_srl (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_srl_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_sra (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_sra_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_smul (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_smul_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_smul_cc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_smul_cc_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_umul (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_umul_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_umul_cc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_umul_cc_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_sdiv (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_sdiv_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_sdiv_cc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_sdiv_cc_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_udiv (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_udiv_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_udiv_cc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_udiv_cc_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_mulscc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_save (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_save_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_restore (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_restore_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_rett (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_WR_PSR_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_WR_PSR_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_rett_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_WR_PSR_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_WR_PSR_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_unimp (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_UNIMP_VARS /* f-imm22 f-op2 f-rd-res f-op */
  EXTRACT_IFMT_UNIMP_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_call (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_CALL_VARS /* f-disp30 f-op */
      IADDR i_disp30;
  EXTRACT_IFMT_CALL_CODE
  i_disp30 = f_disp30;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_jmpl (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_jmpl_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSTUB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSTUB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_ba (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_BA_VARS /* f-disp22 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BA_CODE
  i_disp22 = f_disp22;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_ta (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_TA_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_ta_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_TA_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_bn (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_BA_VARS /* f-disp22 f-op2 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_BA_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_tn (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_TA_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_tn_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_TA_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_bne (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_BA_VARS /* f-disp22 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BA_CODE
  i_disp22 = f_disp22;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_tne (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_TA_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_tne_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_TA_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_be (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_BA_VARS /* f-disp22 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BA_CODE
  i_disp22 = f_disp22;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_te (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_TA_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_te_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_TA_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_bg (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_BA_VARS /* f-disp22 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BA_CODE
  i_disp22 = f_disp22;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_tg (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_TA_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_tg_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_TA_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_ble (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_BA_VARS /* f-disp22 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BA_CODE
  i_disp22 = f_disp22;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_tle (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_TA_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_tle_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_TA_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_bge (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_BA_VARS /* f-disp22 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BA_CODE
  i_disp22 = f_disp22;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_tge (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_TA_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_tge_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_TA_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_bl (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_BA_VARS /* f-disp22 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BA_CODE
  i_disp22 = f_disp22;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_tl (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_TA_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_tl_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_TA_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_bgu (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_BA_VARS /* f-disp22 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BA_CODE
  i_disp22 = f_disp22;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_tgu (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_TA_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_tgu_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_TA_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_bleu (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_BA_VARS /* f-disp22 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BA_CODE
  i_disp22 = f_disp22;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_tleu (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_TA_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_tleu_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_TA_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_bcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_BA_VARS /* f-disp22 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BA_CODE
  i_disp22 = f_disp22;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_tcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_TA_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_tcc_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_TA_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_bcs (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_BA_VARS /* f-disp22 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BA_CODE
  i_disp22 = f_disp22;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_tcs (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_TA_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_tcs_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_TA_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_bpos (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_BA_VARS /* f-disp22 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BA_CODE
  i_disp22 = f_disp22;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_tpos (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_TA_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_tpos_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_TA_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_bneg (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_BA_VARS /* f-disp22 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BA_CODE
  i_disp22 = f_disp22;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_tneg (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_TA_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_tneg_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_TA_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_bvc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_BA_VARS /* f-disp22 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BA_CODE
  i_disp22 = f_disp22;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_tvc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_TA_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_tvc_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_TA_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_bvs (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_BA_VARS /* f-disp22 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BA_CODE
  i_disp22 = f_disp22;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_tvs (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_TA_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc32_def_tvs_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_TA_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_TA_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc32_model_sparc32_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

/* We assume UNIT_NONE == 0 because the tables don't always terminate
   entries with it.  */

/* Model timing data for `sparc32-def'.  */

static const INSN_TIMING sparc32_def_timing[] = {
  { SPARC32_INSN_X_INVALID, 0, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_X_AFTER, 0, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_X_BEFORE, 0, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_X_CTI_CHAIN, 0, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_X_CHAIN, 0, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_X_BEGIN, 0, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_RD_ASR, model_sparc32_def_rd_asr, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_WR_ASR, model_sparc32_def_wr_asr, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_WR_ASR_IMM, model_sparc32_def_wr_asr_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_RD_PSR, model_sparc32_def_rd_psr, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_WR_PSR, model_sparc32_def_wr_psr, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_WR_PSR_IMM, model_sparc32_def_wr_psr_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_RD_WIM, model_sparc32_def_rd_wim, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_WR_WIM, model_sparc32_def_wr_wim, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_WR_WIM_IMM, model_sparc32_def_wr_wim_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_RD_TBR, model_sparc32_def_rd_tbr, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_WR_TBR, model_sparc32_def_wr_tbr, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_WR_TBR_IMM, model_sparc32_def_wr_tbr_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_LDSTUB_REG_REG, model_sparc32_def_ldstub_reg_reg, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_LDSTUB_REG_IMM, model_sparc32_def_ldstub_reg_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_LDSTUB_REG_REG_ASI, model_sparc32_def_ldstub_reg_reg_asi, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_SWAP_REG_REG, model_sparc32_def_swap_reg_reg, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_SWAP_REG_IMM, model_sparc32_def_swap_reg_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_SWAP_REG_REG_ASI, model_sparc32_def_swap_reg_reg_asi, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_LDSB_REG_REG, model_sparc32_def_ldsb_reg_reg, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_LDSB_REG_IMM, model_sparc32_def_ldsb_reg_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_LDSB_REG_REG_ASI, model_sparc32_def_ldsb_reg_reg_asi, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_LDUB_REG_REG, model_sparc32_def_ldub_reg_reg, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_LDUB_REG_IMM, model_sparc32_def_ldub_reg_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_LDUB_REG_REG_ASI, model_sparc32_def_ldub_reg_reg_asi, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_LDSH_REG_REG, model_sparc32_def_ldsh_reg_reg, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_LDSH_REG_IMM, model_sparc32_def_ldsh_reg_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_LDSH_REG_REG_ASI, model_sparc32_def_ldsh_reg_reg_asi, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_LDUH_REG_REG, model_sparc32_def_lduh_reg_reg, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_LDUH_REG_IMM, model_sparc32_def_lduh_reg_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_LDUH_REG_REG_ASI, model_sparc32_def_lduh_reg_reg_asi, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_LDSW_REG_REG, model_sparc32_def_ldsw_reg_reg, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_LDSW_REG_IMM, model_sparc32_def_ldsw_reg_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_LDSW_REG_REG_ASI, model_sparc32_def_ldsw_reg_reg_asi, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_LDUW_REG_REG, model_sparc32_def_lduw_reg_reg, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_LDUW_REG_IMM, model_sparc32_def_lduw_reg_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_LDUW_REG_REG_ASI, model_sparc32_def_lduw_reg_reg_asi, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_LDD_REG_REG, model_sparc32_def_ldd_reg_reg, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_LDD_REG_IMM, model_sparc32_def_ldd_reg_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_LDD_REG_REG_ASI, model_sparc32_def_ldd_reg_reg_asi, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_STB_REG_REG, model_sparc32_def_stb_reg_reg, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_STB_REG_IMM, model_sparc32_def_stb_reg_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_STB_REG_REG_ASI, model_sparc32_def_stb_reg_reg_asi, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_STH_REG_REG, model_sparc32_def_sth_reg_reg, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_STH_REG_IMM, model_sparc32_def_sth_reg_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_STH_REG_REG_ASI, model_sparc32_def_sth_reg_reg_asi, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_ST_REG_REG, model_sparc32_def_st_reg_reg, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_ST_REG_IMM, model_sparc32_def_st_reg_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_ST_REG_REG_ASI, model_sparc32_def_st_reg_reg_asi, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_STD_REG_REG, model_sparc32_def_std_reg_reg, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_STD_REG_IMM, model_sparc32_def_std_reg_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_STD_REG_REG_ASI, model_sparc32_def_std_reg_reg_asi, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_FP_LD_REG_REG, model_sparc32_def_fp_ld_reg_reg, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_FP_LD_REG_IMM, model_sparc32_def_fp_ld_reg_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_FP_LD_REG_REG_ASI, model_sparc32_def_fp_ld_reg_reg_asi, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_SETHI, model_sparc32_def_sethi, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_ADD, model_sparc32_def_add, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_ADD_IMM, model_sparc32_def_add_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_SUB, model_sparc32_def_sub, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_SUB_IMM, model_sparc32_def_sub_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_ADDCC, model_sparc32_def_addcc, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_ADDCC_IMM, model_sparc32_def_addcc_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_SUBCC, model_sparc32_def_subcc, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_SUBCC_IMM, model_sparc32_def_subcc_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_ADDX, model_sparc32_def_addx, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_ADDX_IMM, model_sparc32_def_addx_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_SUBX, model_sparc32_def_subx, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_SUBX_IMM, model_sparc32_def_subx_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_ADDXCC, model_sparc32_def_addxcc, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_ADDXCC_IMM, model_sparc32_def_addxcc_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_SUBXCC, model_sparc32_def_subxcc, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_SUBXCC_IMM, model_sparc32_def_subxcc_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_AND, model_sparc32_def_and, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_AND_IMM, model_sparc32_def_and_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_ANDCC, model_sparc32_def_andcc, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_ANDCC_IMM, model_sparc32_def_andcc_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_OR, model_sparc32_def_or, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_OR_IMM, model_sparc32_def_or_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_ORCC, model_sparc32_def_orcc, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_ORCC_IMM, model_sparc32_def_orcc_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_XOR, model_sparc32_def_xor, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_XOR_IMM, model_sparc32_def_xor_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_XORCC, model_sparc32_def_xorcc, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_XORCC_IMM, model_sparc32_def_xorcc_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_ANDN, model_sparc32_def_andn, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_ANDN_IMM, model_sparc32_def_andn_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_ANDNCC, model_sparc32_def_andncc, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_ANDNCC_IMM, model_sparc32_def_andncc_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_ORN, model_sparc32_def_orn, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_ORN_IMM, model_sparc32_def_orn_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_ORNCC, model_sparc32_def_orncc, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_ORNCC_IMM, model_sparc32_def_orncc_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_XNOR, model_sparc32_def_xnor, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_XNOR_IMM, model_sparc32_def_xnor_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_XNORCC, model_sparc32_def_xnorcc, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_XNORCC_IMM, model_sparc32_def_xnorcc_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_SLL, model_sparc32_def_sll, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_SLL_IMM, model_sparc32_def_sll_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_SRL, model_sparc32_def_srl, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_SRL_IMM, model_sparc32_def_srl_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_SRA, model_sparc32_def_sra, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_SRA_IMM, model_sparc32_def_sra_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_SMUL, model_sparc32_def_smul, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_SMUL_IMM, model_sparc32_def_smul_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_SMUL_CC, model_sparc32_def_smul_cc, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_SMUL_CC_IMM, model_sparc32_def_smul_cc_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_UMUL, model_sparc32_def_umul, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_UMUL_IMM, model_sparc32_def_umul_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_UMUL_CC, model_sparc32_def_umul_cc, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_UMUL_CC_IMM, model_sparc32_def_umul_cc_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_SDIV, model_sparc32_def_sdiv, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_SDIV_IMM, model_sparc32_def_sdiv_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_SDIV_CC, model_sparc32_def_sdiv_cc, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_SDIV_CC_IMM, model_sparc32_def_sdiv_cc_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_UDIV, model_sparc32_def_udiv, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_UDIV_IMM, model_sparc32_def_udiv_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_UDIV_CC, model_sparc32_def_udiv_cc, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_UDIV_CC_IMM, model_sparc32_def_udiv_cc_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_MULSCC, model_sparc32_def_mulscc, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_SAVE, model_sparc32_def_save, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_SAVE_IMM, model_sparc32_def_save_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_RESTORE, model_sparc32_def_restore, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_RESTORE_IMM, model_sparc32_def_restore_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_RETT, model_sparc32_def_rett, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_RETT_IMM, model_sparc32_def_rett_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_UNIMP, model_sparc32_def_unimp, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_CALL, model_sparc32_def_call, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_JMPL, model_sparc32_def_jmpl, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_JMPL_IMM, model_sparc32_def_jmpl_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_BA, model_sparc32_def_ba, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_TA, model_sparc32_def_ta, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_TA_IMM, model_sparc32_def_ta_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_BN, model_sparc32_def_bn, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_TN, model_sparc32_def_tn, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_TN_IMM, model_sparc32_def_tn_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_BNE, model_sparc32_def_bne, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_TNE, model_sparc32_def_tne, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_TNE_IMM, model_sparc32_def_tne_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_BE, model_sparc32_def_be, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_TE, model_sparc32_def_te, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_TE_IMM, model_sparc32_def_te_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_BG, model_sparc32_def_bg, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_TG, model_sparc32_def_tg, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_TG_IMM, model_sparc32_def_tg_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_BLE, model_sparc32_def_ble, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_TLE, model_sparc32_def_tle, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_TLE_IMM, model_sparc32_def_tle_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_BGE, model_sparc32_def_bge, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_TGE, model_sparc32_def_tge, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_TGE_IMM, model_sparc32_def_tge_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_BL, model_sparc32_def_bl, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_TL, model_sparc32_def_tl, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_TL_IMM, model_sparc32_def_tl_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_BGU, model_sparc32_def_bgu, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_TGU, model_sparc32_def_tgu, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_TGU_IMM, model_sparc32_def_tgu_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_BLEU, model_sparc32_def_bleu, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_TLEU, model_sparc32_def_tleu, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_TLEU_IMM, model_sparc32_def_tleu_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_BCC, model_sparc32_def_bcc, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_TCC, model_sparc32_def_tcc, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_TCC_IMM, model_sparc32_def_tcc_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_BCS, model_sparc32_def_bcs, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_TCS, model_sparc32_def_tcs, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_TCS_IMM, model_sparc32_def_tcs_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_BPOS, model_sparc32_def_bpos, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_TPOS, model_sparc32_def_tpos, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_TPOS_IMM, model_sparc32_def_tpos_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_BNEG, model_sparc32_def_bneg, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_TNEG, model_sparc32_def_tneg, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_TNEG_IMM, model_sparc32_def_tneg_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_BVC, model_sparc32_def_bvc, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_TVC, model_sparc32_def_tvc, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_TVC_IMM, model_sparc32_def_tvc_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_BVS, model_sparc32_def_bvs, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_TVS, model_sparc32_def_tvs, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
  { SPARC32_INSN_TVS_IMM, model_sparc32_def_tvs_imm, { { (int) UNIT_SPARC32_DEF_U_EXEC, 1, 1 } } },
};

#endif /* WITH_PROFILE_MODEL_P */

static void
sparc32_def_model_init (SIM_CPU *cpu)
{
  CPU_MODEL_DATA (cpu) = (void *) zalloc (sizeof (MODEL_SPARC32_DEF_DATA));
}

#if WITH_PROFILE_MODEL_P
#define TIMING_DATA(td) td
#else
#define TIMING_DATA(td) 0
#endif

static const MODEL sparc_v8_models[] =
{
  { "sparc32-def", & sparc_v8_mach, MODEL_SPARC32_DEF, TIMING_DATA (& sparc32_def_timing[0]), sparc32_def_model_init },
  { 0 }
};

static const MODEL sparclite_models[] =
{
  { 0 }
};

/* The properties of this cpu's implementation.  */

static const MACH_IMP_PROPERTIES sparc32_imp_properties =
{
  sizeof (SIM_CPU),
#if WITH_SCACHE
  sizeof (SCACHE)
#else
  0
#endif
};


static void
sparc32_prepare_run (SIM_CPU *cpu)
{
  if (CPU_IDESC (cpu) == NULL)
    sparc32_init_idesc_table (cpu);
}

static const CGEN_INSN *
sparc32_get_idata (SIM_CPU *cpu, int inum)
{
  return CPU_IDESC (cpu) [inum].idata;
}

static void
sparc_v8_init_cpu (SIM_CPU *cpu)
{
  CPU_REG_FETCH (cpu) = sparc32_fetch_register;
  CPU_REG_STORE (cpu) = sparc32_store_register;
  CPU_PC_FETCH (cpu) = sparc32_h_pc_get;
  CPU_PC_STORE (cpu) = sparc32_h_pc_set;
  CPU_GET_IDATA (cpu) = sparc32_get_idata;
  CPU_MAX_INSNS (cpu) = SPARC32_INSN_MAX;
  CPU_INSN_NAME (cpu) = cgen_insn_name;
  CPU_FULL_ENGINE_FN (cpu) = sparc32_engine_run_full;
#if WITH_FAST
  CPU_FAST_ENGINE_FN (cpu) = sparc32_engine_run_fast;
#else
  CPU_FAST_ENGINE_FN (cpu) = sparc32_engine_run_full;
#endif
}

const MACH sparc_v8_mach =
{
  "sparc-v8", "sparc",
  32, 32, & sparc_v8_models[0], & sparc32_imp_properties,
  sparc_v8_init_cpu,
  sparc32_prepare_run
};

static void
sparclite_init_cpu (SIM_CPU *cpu)
{
  CPU_REG_FETCH (cpu) = sparc32_fetch_register;
  CPU_REG_STORE (cpu) = sparc32_store_register;
  CPU_PC_FETCH (cpu) = sparc32_h_pc_get;
  CPU_PC_STORE (cpu) = sparc32_h_pc_set;
  CPU_GET_IDATA (cpu) = sparc32_get_idata;
  CPU_MAX_INSNS (cpu) = SPARC32_INSN_MAX;
  CPU_INSN_NAME (cpu) = cgen_insn_name;
  CPU_FULL_ENGINE_FN (cpu) = sparc32_engine_run_full;
#if WITH_FAST
  CPU_FAST_ENGINE_FN (cpu) = sparc32_engine_run_fast;
#else
  CPU_FAST_ENGINE_FN (cpu) = sparc32_engine_run_full;
#endif
}

const MACH sparclite_mach =
{
  "sparclite", "sparc_sparclite",
  32, 32, & sparclite_models[0], & sparc32_imp_properties,
  sparclite_init_cpu,
  sparc32_prepare_run
};

