/* Simulator model support for sparc64.

THIS FILE IS MACHINE GENERATED WITH CGEN.

Copyright (C) 1999 Cygnus Solutions, Inc.

This file is part of the Cygnus Simulators.


*/

#define WANT_CPU sparc64
#define WANT_CPU_SPARC64

#include "sim-main.h"

/* The profiling data is recorded here, but is accessed via the profiling
   mechanism.  After all, this is information for profiling.  */

#if WITH_PROFILE_MODEL_P

/* Model handlers for each insn.  */

static int
model_sparc64_def_beqz (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_BEQZ_VARS /* f-disp16 f-rs1 f-p f-op2 f-fmt2-rcond f-bpr-res28-1 f-a f-op */
      IADDR i_disp16;
  EXTRACT_IFMT_BEQZ_CODE
  i_disp16 = f_disp16;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_bgez (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_BEQZ_VARS /* f-disp16 f-rs1 f-p f-op2 f-fmt2-rcond f-bpr-res28-1 f-a f-op */
      IADDR i_disp16;
  EXTRACT_IFMT_BEQZ_CODE
  i_disp16 = f_disp16;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_bgtz (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_BEQZ_VARS /* f-disp16 f-rs1 f-p f-op2 f-fmt2-rcond f-bpr-res28-1 f-a f-op */
      IADDR i_disp16;
  EXTRACT_IFMT_BEQZ_CODE
  i_disp16 = f_disp16;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_blez (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_BEQZ_VARS /* f-disp16 f-rs1 f-p f-op2 f-fmt2-rcond f-bpr-res28-1 f-a f-op */
      IADDR i_disp16;
  EXTRACT_IFMT_BEQZ_CODE
  i_disp16 = f_disp16;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_bltz (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_BEQZ_VARS /* f-disp16 f-rs1 f-p f-op2 f-fmt2-rcond f-bpr-res28-1 f-a f-op */
      IADDR i_disp16;
  EXTRACT_IFMT_BEQZ_CODE
  i_disp16 = f_disp16;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_bnez (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_BEQZ_VARS /* f-disp16 f-rs1 f-p f-op2 f-fmt2-rcond f-bpr-res28-1 f-a f-op */
      IADDR i_disp16;
  EXTRACT_IFMT_BEQZ_CODE
  i_disp16 = f_disp16;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_bpcc_ba (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_BPCC_BA_VARS /* f-disp19 f-p f-fmt2-cc0 f-fmt2-cc1 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BPCC_BA_CODE
  i_disp22 = f_disp22;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_bpcc_bn (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_BPCC_BA_VARS /* f-disp19 f-p f-fmt2-cc0 f-fmt2-cc1 f-op2 f-fmt2-cond f-a f-op */
  EXTRACT_IFMT_BPCC_BA_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_bpcc_bne (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_BPCC_BA_VARS /* f-disp19 f-p f-fmt2-cc0 f-fmt2-cc1 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BPCC_BA_CODE
  i_disp22 = f_disp22;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_bpcc_be (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_BPCC_BA_VARS /* f-disp19 f-p f-fmt2-cc0 f-fmt2-cc1 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BPCC_BA_CODE
  i_disp22 = f_disp22;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_bpcc_bg (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_BPCC_BA_VARS /* f-disp19 f-p f-fmt2-cc0 f-fmt2-cc1 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BPCC_BA_CODE
  i_disp22 = f_disp22;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_bpcc_ble (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_BPCC_BA_VARS /* f-disp19 f-p f-fmt2-cc0 f-fmt2-cc1 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BPCC_BA_CODE
  i_disp22 = f_disp22;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_bpcc_bge (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_BPCC_BA_VARS /* f-disp19 f-p f-fmt2-cc0 f-fmt2-cc1 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BPCC_BA_CODE
  i_disp22 = f_disp22;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_bpcc_bl (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_BPCC_BA_VARS /* f-disp19 f-p f-fmt2-cc0 f-fmt2-cc1 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BPCC_BA_CODE
  i_disp22 = f_disp22;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_bpcc_bgu (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_BPCC_BA_VARS /* f-disp19 f-p f-fmt2-cc0 f-fmt2-cc1 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BPCC_BA_CODE
  i_disp22 = f_disp22;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_bpcc_bleu (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_BPCC_BA_VARS /* f-disp19 f-p f-fmt2-cc0 f-fmt2-cc1 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BPCC_BA_CODE
  i_disp22 = f_disp22;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_bpcc_bcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_BPCC_BA_VARS /* f-disp19 f-p f-fmt2-cc0 f-fmt2-cc1 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BPCC_BA_CODE
  i_disp22 = f_disp22;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_bpcc_bcs (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_BPCC_BA_VARS /* f-disp19 f-p f-fmt2-cc0 f-fmt2-cc1 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BPCC_BA_CODE
  i_disp22 = f_disp22;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_bpcc_bpos (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_BPCC_BA_VARS /* f-disp19 f-p f-fmt2-cc0 f-fmt2-cc1 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BPCC_BA_CODE
  i_disp22 = f_disp22;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_bpcc_bneg (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_BPCC_BA_VARS /* f-disp19 f-p f-fmt2-cc0 f-fmt2-cc1 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BPCC_BA_CODE
  i_disp22 = f_disp22;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_bpcc_bvc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_BPCC_BA_VARS /* f-disp19 f-p f-fmt2-cc0 f-fmt2-cc1 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BPCC_BA_CODE
  i_disp22 = f_disp22;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_bpcc_bvs (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_BPCC_BA_VARS /* f-disp19 f-p f-fmt2-cc0 f-fmt2-cc1 f-op2 f-fmt2-cond f-a f-op */
      IADDR i_disp22;
  EXTRACT_IFMT_BPCC_BA_CODE
  i_disp22 = f_disp22;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_done (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_DONE_VARS /* f-res-18-19 f-op3 f-fcn f-op */
  EXTRACT_IFMT_DONE_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_retry (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_DONE_VARS /* f-res-18-19 f-op3 f-fcn f-op */
  EXTRACT_IFMT_DONE_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_flush (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_FLUSH_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_FLUSH_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_flush_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_FLUSH_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_FLUSH_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_flushw (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_FLUSHW_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_FLUSHW_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_impdep1 (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_IMPDEP1_VARS /* f-impdep19 f-op3 f-impdep5 f-op */
  EXTRACT_IFMT_IMPDEP1_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_impdep2 (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_IMPDEP1_VARS /* f-impdep19 f-op3 f-impdep5 f-op */
  EXTRACT_IFMT_IMPDEP1_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_membar (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MEMBAR_VARS /* f-membarmask f-membar-res12-6 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_MEMBAR_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_mova_icc_icc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_mova_imm_icc_icc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_mova_xcc_xcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_mova_imm_xcc_xcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movn_icc_icc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movn_imm_icc_icc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movn_xcc_xcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movn_imm_xcc_xcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movne_icc_icc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movne_imm_icc_icc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movne_xcc_xcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movne_imm_xcc_xcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_move_icc_icc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_move_imm_icc_icc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_move_xcc_xcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_move_imm_xcc_xcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movg_icc_icc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movg_imm_icc_icc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movg_xcc_xcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movg_imm_xcc_xcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movle_icc_icc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movle_imm_icc_icc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movle_xcc_xcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movle_imm_xcc_xcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movge_icc_icc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movge_imm_icc_icc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movge_xcc_xcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movge_imm_xcc_xcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movl_icc_icc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movl_imm_icc_icc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movl_xcc_xcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movl_imm_xcc_xcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movgu_icc_icc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movgu_imm_icc_icc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movgu_xcc_xcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movgu_imm_xcc_xcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movleu_icc_icc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movleu_imm_icc_icc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movleu_xcc_xcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movleu_imm_xcc_xcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movcc_icc_icc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movcc_imm_icc_icc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movcc_xcc_xcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movcc_imm_xcc_xcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movcs_icc_icc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movcs_imm_icc_icc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movcs_xcc_xcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movcs_imm_xcc_xcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movpos_icc_icc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movpos_imm_icc_icc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movpos_xcc_xcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movpos_imm_xcc_xcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movneg_icc_icc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movneg_imm_icc_icc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movneg_xcc_xcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movneg_imm_xcc_xcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movvc_icc_icc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movvc_imm_icc_icc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movvc_xcc_xcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movvc_imm_xcc_xcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movvs_icc_icc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movvs_imm_icc_icc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movvs_xcc_xcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_ICC_ICC_VARS /* f-rs2 f-fmt4-res10-6 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_movvs_imm_xcc_xcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_VARS /* f-simm11 f-fmt4-cc1-0 f-i f-fmt4-cc2 f-op3 f-fmt2-cond f-rd f-op */
  EXTRACT_IFMT_MOVA_IMM_ICC_ICC_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_ldsb_reg_reg (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_ldsb_reg_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_ldsb_reg_reg_asi (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_ASI_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_ldub_reg_reg (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_ldub_reg_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_ldub_reg_reg_asi (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_ASI_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_ldsh_reg_reg (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_ldsh_reg_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_ldsh_reg_reg_asi (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_ASI_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_lduh_reg_reg (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_lduh_reg_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_lduh_reg_reg_asi (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_ASI_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_ldsw_reg_reg (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_ldsw_reg_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_ldsw_reg_reg_asi (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_ASI_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_lduw_reg_reg (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_lduw_reg_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_lduw_reg_reg_asi (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_ASI_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_ldx_reg_reg (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_ldx_reg_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_ldx_reg_reg_asi (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_ASI_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_ldd_reg_reg (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_ldd_reg_imm (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_ldd_reg_reg_asi (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_stb_reg_reg (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_stb_reg_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_stb_reg_reg_asi (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_ASI_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_sth_reg_reg (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_sth_reg_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_sth_reg_reg_asi (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_ASI_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_st_reg_reg (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_st_reg_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_st_reg_reg_asi (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_ASI_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_stx_reg_reg (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_stx_reg_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_stx_reg_reg_asi (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_ASI_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_std_reg_reg (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_std_reg_imm (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_std_reg_reg_asi (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_fp_ld_reg_reg (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_fp_ld_reg_imm (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_fp_ld_reg_reg_asi (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_sethi (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_add (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_add_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_sub (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_sub_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_addcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_addcc_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_subcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_subcc_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_addc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_addc_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_subc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_subc_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_addccc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_addccc_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_subccc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_subccc_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_and (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_and_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_andcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_andcc_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_or (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_or_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_orcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_orcc_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_xor (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_xor_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_xorcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_xorcc_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_andn (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_andn_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_andncc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_andncc_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_orn (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_orn_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_orncc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_orncc_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_xnor (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_xnor_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_xnorcc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_xnorcc_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_sll (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_sll_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_srl (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_srl_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_sra (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_sra_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_smul (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_smul_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_smul_cc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_smul_cc_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_umul (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_umul_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_umul_cc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_umul_cc_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_mulscc (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_save (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_save_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_restore (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_restore_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_rett (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_FLUSH_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_FLUSH_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_rett_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_FLUSH_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_FLUSH_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_unimp (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_call (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_jmpl (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_jmpl_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_ba (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_ta (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_ta_imm (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_bn (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_tn (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_tn_imm (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_bne (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_tne (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_tne_imm (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_be (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_te (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_te_imm (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_bg (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_tg (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_tg_imm (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_ble (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_tle (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_tle_imm (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_bge (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_tge (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_tge_imm (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_bl (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_tl (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_tl_imm (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_bgu (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_tgu (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_tgu_imm (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_bleu (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_tleu (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_tleu_imm (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_bcc (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_tcc (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_tcc_imm (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_bcs (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_tcs (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_tcs_imm (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_bpos (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_tpos (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_tpos_imm (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_bneg (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_tneg (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_tneg_imm (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_bvc (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_tvc (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_tvc_imm (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_bvs (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_tvs (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_tvs_imm (SIM_CPU *current_cpu, void *sem_arg)
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
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_ldstub_reg_reg (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_ldstub_reg_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_ldstub_reg_reg_asi (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_ASI_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_swap_reg_reg (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_VARS /* f-rs2 f-res-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_swap_reg_imm (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_IMM_VARS /* f-simm13 f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_IMM_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

static int
model_sparc64_def_swap_reg_reg_asi (SIM_CPU *current_cpu, void *sem_arg)
{
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  IADDR UNUSED pc = GET_H_PC ();
  CGEN_INSN_INT insn = abuf->insn;
  EXTRACT_IFMT_LDSB_REG_REG_ASI_VARS /* f-rs2 f-asi f-i f-rs1 f-op3 f-rd f-op */
  EXTRACT_IFMT_LDSB_REG_REG_ASI_CODE
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    cycles += sparc64_model_sparc64_def_u_exec (current_cpu, idesc, 0, referenced);
  }
  return cycles;
}

/* We assume UNIT_NONE == 0 because the tables don't always terminate
   entries with it.  */

/* Model timing data for `sparc64-def'.  */

static const INSN_TIMING sparc64_def_timing[] = {
  { SPARC64_INSN_X_INVALID, 0, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_X_AFTER, 0, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_X_BEFORE, 0, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_X_CTI_CHAIN, 0, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_X_CHAIN, 0, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_X_BEGIN, 0, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_BEQZ, model_sparc64_def_beqz, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_BGEZ, model_sparc64_def_bgez, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_BGTZ, model_sparc64_def_bgtz, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_BLEZ, model_sparc64_def_blez, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_BLTZ, model_sparc64_def_bltz, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_BNEZ, model_sparc64_def_bnez, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_BPCC_BA, model_sparc64_def_bpcc_ba, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_BPCC_BN, model_sparc64_def_bpcc_bn, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_BPCC_BNE, model_sparc64_def_bpcc_bne, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_BPCC_BE, model_sparc64_def_bpcc_be, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_BPCC_BG, model_sparc64_def_bpcc_bg, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_BPCC_BLE, model_sparc64_def_bpcc_ble, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_BPCC_BGE, model_sparc64_def_bpcc_bge, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_BPCC_BL, model_sparc64_def_bpcc_bl, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_BPCC_BGU, model_sparc64_def_bpcc_bgu, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_BPCC_BLEU, model_sparc64_def_bpcc_bleu, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_BPCC_BCC, model_sparc64_def_bpcc_bcc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_BPCC_BCS, model_sparc64_def_bpcc_bcs, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_BPCC_BPOS, model_sparc64_def_bpcc_bpos, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_BPCC_BNEG, model_sparc64_def_bpcc_bneg, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_BPCC_BVC, model_sparc64_def_bpcc_bvc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_BPCC_BVS, model_sparc64_def_bpcc_bvs, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_DONE, model_sparc64_def_done, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_RETRY, model_sparc64_def_retry, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_FLUSH, model_sparc64_def_flush, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_FLUSH_IMM, model_sparc64_def_flush_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_FLUSHW, model_sparc64_def_flushw, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_IMPDEP1, model_sparc64_def_impdep1, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_IMPDEP2, model_sparc64_def_impdep2, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MEMBAR, model_sparc64_def_membar, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVA_ICC_ICC, model_sparc64_def_mova_icc_icc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVA_IMM_ICC_ICC, model_sparc64_def_mova_imm_icc_icc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVA_XCC_XCC, model_sparc64_def_mova_xcc_xcc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVA_IMM_XCC_XCC, model_sparc64_def_mova_imm_xcc_xcc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVN_ICC_ICC, model_sparc64_def_movn_icc_icc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVN_IMM_ICC_ICC, model_sparc64_def_movn_imm_icc_icc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVN_XCC_XCC, model_sparc64_def_movn_xcc_xcc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVN_IMM_XCC_XCC, model_sparc64_def_movn_imm_xcc_xcc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVNE_ICC_ICC, model_sparc64_def_movne_icc_icc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVNE_IMM_ICC_ICC, model_sparc64_def_movne_imm_icc_icc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVNE_XCC_XCC, model_sparc64_def_movne_xcc_xcc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVNE_IMM_XCC_XCC, model_sparc64_def_movne_imm_xcc_xcc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVE_ICC_ICC, model_sparc64_def_move_icc_icc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVE_IMM_ICC_ICC, model_sparc64_def_move_imm_icc_icc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVE_XCC_XCC, model_sparc64_def_move_xcc_xcc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVE_IMM_XCC_XCC, model_sparc64_def_move_imm_xcc_xcc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVG_ICC_ICC, model_sparc64_def_movg_icc_icc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVG_IMM_ICC_ICC, model_sparc64_def_movg_imm_icc_icc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVG_XCC_XCC, model_sparc64_def_movg_xcc_xcc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVG_IMM_XCC_XCC, model_sparc64_def_movg_imm_xcc_xcc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVLE_ICC_ICC, model_sparc64_def_movle_icc_icc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVLE_IMM_ICC_ICC, model_sparc64_def_movle_imm_icc_icc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVLE_XCC_XCC, model_sparc64_def_movle_xcc_xcc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVLE_IMM_XCC_XCC, model_sparc64_def_movle_imm_xcc_xcc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVGE_ICC_ICC, model_sparc64_def_movge_icc_icc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVGE_IMM_ICC_ICC, model_sparc64_def_movge_imm_icc_icc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVGE_XCC_XCC, model_sparc64_def_movge_xcc_xcc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVGE_IMM_XCC_XCC, model_sparc64_def_movge_imm_xcc_xcc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVL_ICC_ICC, model_sparc64_def_movl_icc_icc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVL_IMM_ICC_ICC, model_sparc64_def_movl_imm_icc_icc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVL_XCC_XCC, model_sparc64_def_movl_xcc_xcc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVL_IMM_XCC_XCC, model_sparc64_def_movl_imm_xcc_xcc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVGU_ICC_ICC, model_sparc64_def_movgu_icc_icc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVGU_IMM_ICC_ICC, model_sparc64_def_movgu_imm_icc_icc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVGU_XCC_XCC, model_sparc64_def_movgu_xcc_xcc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVGU_IMM_XCC_XCC, model_sparc64_def_movgu_imm_xcc_xcc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVLEU_ICC_ICC, model_sparc64_def_movleu_icc_icc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVLEU_IMM_ICC_ICC, model_sparc64_def_movleu_imm_icc_icc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVLEU_XCC_XCC, model_sparc64_def_movleu_xcc_xcc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVLEU_IMM_XCC_XCC, model_sparc64_def_movleu_imm_xcc_xcc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVCC_ICC_ICC, model_sparc64_def_movcc_icc_icc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVCC_IMM_ICC_ICC, model_sparc64_def_movcc_imm_icc_icc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVCC_XCC_XCC, model_sparc64_def_movcc_xcc_xcc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVCC_IMM_XCC_XCC, model_sparc64_def_movcc_imm_xcc_xcc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVCS_ICC_ICC, model_sparc64_def_movcs_icc_icc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVCS_IMM_ICC_ICC, model_sparc64_def_movcs_imm_icc_icc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVCS_XCC_XCC, model_sparc64_def_movcs_xcc_xcc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVCS_IMM_XCC_XCC, model_sparc64_def_movcs_imm_xcc_xcc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVPOS_ICC_ICC, model_sparc64_def_movpos_icc_icc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVPOS_IMM_ICC_ICC, model_sparc64_def_movpos_imm_icc_icc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVPOS_XCC_XCC, model_sparc64_def_movpos_xcc_xcc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVPOS_IMM_XCC_XCC, model_sparc64_def_movpos_imm_xcc_xcc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVNEG_ICC_ICC, model_sparc64_def_movneg_icc_icc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVNEG_IMM_ICC_ICC, model_sparc64_def_movneg_imm_icc_icc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVNEG_XCC_XCC, model_sparc64_def_movneg_xcc_xcc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVNEG_IMM_XCC_XCC, model_sparc64_def_movneg_imm_xcc_xcc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVVC_ICC_ICC, model_sparc64_def_movvc_icc_icc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVVC_IMM_ICC_ICC, model_sparc64_def_movvc_imm_icc_icc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVVC_XCC_XCC, model_sparc64_def_movvc_xcc_xcc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVVC_IMM_XCC_XCC, model_sparc64_def_movvc_imm_xcc_xcc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVVS_ICC_ICC, model_sparc64_def_movvs_icc_icc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVVS_IMM_ICC_ICC, model_sparc64_def_movvs_imm_icc_icc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVVS_XCC_XCC, model_sparc64_def_movvs_xcc_xcc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MOVVS_IMM_XCC_XCC, model_sparc64_def_movvs_imm_xcc_xcc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_LDSB_REG_REG, model_sparc64_def_ldsb_reg_reg, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_LDSB_REG_IMM, model_sparc64_def_ldsb_reg_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_LDSB_REG_REG_ASI, model_sparc64_def_ldsb_reg_reg_asi, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_LDUB_REG_REG, model_sparc64_def_ldub_reg_reg, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_LDUB_REG_IMM, model_sparc64_def_ldub_reg_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_LDUB_REG_REG_ASI, model_sparc64_def_ldub_reg_reg_asi, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_LDSH_REG_REG, model_sparc64_def_ldsh_reg_reg, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_LDSH_REG_IMM, model_sparc64_def_ldsh_reg_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_LDSH_REG_REG_ASI, model_sparc64_def_ldsh_reg_reg_asi, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_LDUH_REG_REG, model_sparc64_def_lduh_reg_reg, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_LDUH_REG_IMM, model_sparc64_def_lduh_reg_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_LDUH_REG_REG_ASI, model_sparc64_def_lduh_reg_reg_asi, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_LDSW_REG_REG, model_sparc64_def_ldsw_reg_reg, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_LDSW_REG_IMM, model_sparc64_def_ldsw_reg_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_LDSW_REG_REG_ASI, model_sparc64_def_ldsw_reg_reg_asi, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_LDUW_REG_REG, model_sparc64_def_lduw_reg_reg, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_LDUW_REG_IMM, model_sparc64_def_lduw_reg_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_LDUW_REG_REG_ASI, model_sparc64_def_lduw_reg_reg_asi, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_LDX_REG_REG, model_sparc64_def_ldx_reg_reg, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_LDX_REG_IMM, model_sparc64_def_ldx_reg_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_LDX_REG_REG_ASI, model_sparc64_def_ldx_reg_reg_asi, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_LDD_REG_REG, model_sparc64_def_ldd_reg_reg, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_LDD_REG_IMM, model_sparc64_def_ldd_reg_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_LDD_REG_REG_ASI, model_sparc64_def_ldd_reg_reg_asi, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_STB_REG_REG, model_sparc64_def_stb_reg_reg, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_STB_REG_IMM, model_sparc64_def_stb_reg_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_STB_REG_REG_ASI, model_sparc64_def_stb_reg_reg_asi, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_STH_REG_REG, model_sparc64_def_sth_reg_reg, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_STH_REG_IMM, model_sparc64_def_sth_reg_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_STH_REG_REG_ASI, model_sparc64_def_sth_reg_reg_asi, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_ST_REG_REG, model_sparc64_def_st_reg_reg, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_ST_REG_IMM, model_sparc64_def_st_reg_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_ST_REG_REG_ASI, model_sparc64_def_st_reg_reg_asi, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_STX_REG_REG, model_sparc64_def_stx_reg_reg, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_STX_REG_IMM, model_sparc64_def_stx_reg_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_STX_REG_REG_ASI, model_sparc64_def_stx_reg_reg_asi, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_STD_REG_REG, model_sparc64_def_std_reg_reg, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_STD_REG_IMM, model_sparc64_def_std_reg_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_STD_REG_REG_ASI, model_sparc64_def_std_reg_reg_asi, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_FP_LD_REG_REG, model_sparc64_def_fp_ld_reg_reg, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_FP_LD_REG_IMM, model_sparc64_def_fp_ld_reg_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_FP_LD_REG_REG_ASI, model_sparc64_def_fp_ld_reg_reg_asi, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_SETHI, model_sparc64_def_sethi, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_ADD, model_sparc64_def_add, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_ADD_IMM, model_sparc64_def_add_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_SUB, model_sparc64_def_sub, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_SUB_IMM, model_sparc64_def_sub_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_ADDCC, model_sparc64_def_addcc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_ADDCC_IMM, model_sparc64_def_addcc_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_SUBCC, model_sparc64_def_subcc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_SUBCC_IMM, model_sparc64_def_subcc_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_ADDC, model_sparc64_def_addc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_ADDC_IMM, model_sparc64_def_addc_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_SUBC, model_sparc64_def_subc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_SUBC_IMM, model_sparc64_def_subc_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_ADDCCC, model_sparc64_def_addccc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_ADDCCC_IMM, model_sparc64_def_addccc_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_SUBCCC, model_sparc64_def_subccc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_SUBCCC_IMM, model_sparc64_def_subccc_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_AND, model_sparc64_def_and, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_AND_IMM, model_sparc64_def_and_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_ANDCC, model_sparc64_def_andcc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_ANDCC_IMM, model_sparc64_def_andcc_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_OR, model_sparc64_def_or, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_OR_IMM, model_sparc64_def_or_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_ORCC, model_sparc64_def_orcc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_ORCC_IMM, model_sparc64_def_orcc_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_XOR, model_sparc64_def_xor, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_XOR_IMM, model_sparc64_def_xor_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_XORCC, model_sparc64_def_xorcc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_XORCC_IMM, model_sparc64_def_xorcc_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_ANDN, model_sparc64_def_andn, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_ANDN_IMM, model_sparc64_def_andn_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_ANDNCC, model_sparc64_def_andncc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_ANDNCC_IMM, model_sparc64_def_andncc_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_ORN, model_sparc64_def_orn, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_ORN_IMM, model_sparc64_def_orn_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_ORNCC, model_sparc64_def_orncc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_ORNCC_IMM, model_sparc64_def_orncc_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_XNOR, model_sparc64_def_xnor, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_XNOR_IMM, model_sparc64_def_xnor_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_XNORCC, model_sparc64_def_xnorcc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_XNORCC_IMM, model_sparc64_def_xnorcc_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_SLL, model_sparc64_def_sll, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_SLL_IMM, model_sparc64_def_sll_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_SRL, model_sparc64_def_srl, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_SRL_IMM, model_sparc64_def_srl_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_SRA, model_sparc64_def_sra, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_SRA_IMM, model_sparc64_def_sra_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_SMUL, model_sparc64_def_smul, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_SMUL_IMM, model_sparc64_def_smul_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_SMUL_CC, model_sparc64_def_smul_cc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_SMUL_CC_IMM, model_sparc64_def_smul_cc_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_UMUL, model_sparc64_def_umul, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_UMUL_IMM, model_sparc64_def_umul_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_UMUL_CC, model_sparc64_def_umul_cc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_UMUL_CC_IMM, model_sparc64_def_umul_cc_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_MULSCC, model_sparc64_def_mulscc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_SAVE, model_sparc64_def_save, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_SAVE_IMM, model_sparc64_def_save_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_RESTORE, model_sparc64_def_restore, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_RESTORE_IMM, model_sparc64_def_restore_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_RETT, model_sparc64_def_rett, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_RETT_IMM, model_sparc64_def_rett_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_UNIMP, model_sparc64_def_unimp, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_CALL, model_sparc64_def_call, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_JMPL, model_sparc64_def_jmpl, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_JMPL_IMM, model_sparc64_def_jmpl_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_BA, model_sparc64_def_ba, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_TA, model_sparc64_def_ta, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_TA_IMM, model_sparc64_def_ta_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_BN, model_sparc64_def_bn, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_TN, model_sparc64_def_tn, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_TN_IMM, model_sparc64_def_tn_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_BNE, model_sparc64_def_bne, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_TNE, model_sparc64_def_tne, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_TNE_IMM, model_sparc64_def_tne_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_BE, model_sparc64_def_be, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_TE, model_sparc64_def_te, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_TE_IMM, model_sparc64_def_te_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_BG, model_sparc64_def_bg, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_TG, model_sparc64_def_tg, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_TG_IMM, model_sparc64_def_tg_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_BLE, model_sparc64_def_ble, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_TLE, model_sparc64_def_tle, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_TLE_IMM, model_sparc64_def_tle_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_BGE, model_sparc64_def_bge, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_TGE, model_sparc64_def_tge, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_TGE_IMM, model_sparc64_def_tge_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_BL, model_sparc64_def_bl, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_TL, model_sparc64_def_tl, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_TL_IMM, model_sparc64_def_tl_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_BGU, model_sparc64_def_bgu, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_TGU, model_sparc64_def_tgu, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_TGU_IMM, model_sparc64_def_tgu_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_BLEU, model_sparc64_def_bleu, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_TLEU, model_sparc64_def_tleu, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_TLEU_IMM, model_sparc64_def_tleu_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_BCC, model_sparc64_def_bcc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_TCC, model_sparc64_def_tcc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_TCC_IMM, model_sparc64_def_tcc_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_BCS, model_sparc64_def_bcs, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_TCS, model_sparc64_def_tcs, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_TCS_IMM, model_sparc64_def_tcs_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_BPOS, model_sparc64_def_bpos, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_TPOS, model_sparc64_def_tpos, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_TPOS_IMM, model_sparc64_def_tpos_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_BNEG, model_sparc64_def_bneg, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_TNEG, model_sparc64_def_tneg, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_TNEG_IMM, model_sparc64_def_tneg_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_BVC, model_sparc64_def_bvc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_TVC, model_sparc64_def_tvc, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_TVC_IMM, model_sparc64_def_tvc_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_BVS, model_sparc64_def_bvs, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_TVS, model_sparc64_def_tvs, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_TVS_IMM, model_sparc64_def_tvs_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_LDSTUB_REG_REG, model_sparc64_def_ldstub_reg_reg, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_LDSTUB_REG_IMM, model_sparc64_def_ldstub_reg_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_LDSTUB_REG_REG_ASI, model_sparc64_def_ldstub_reg_reg_asi, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_SWAP_REG_REG, model_sparc64_def_swap_reg_reg, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_SWAP_REG_IMM, model_sparc64_def_swap_reg_imm, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
  { SPARC64_INSN_SWAP_REG_REG_ASI, model_sparc64_def_swap_reg_reg_asi, { { (int) UNIT_SPARC64_DEF_U_EXEC, 1, 1 } } },
};

#endif /* WITH_PROFILE_MODEL_P */

static void
sparc64_def_model_init (SIM_CPU *cpu)
{
  CPU_MODEL_DATA (cpu) = (void *) zalloc (sizeof (MODEL_SPARC64_DEF_DATA));
}

#if WITH_PROFILE_MODEL_P
#define TIMING_DATA(td) td
#else
#define TIMING_DATA(td) 0
#endif

static const MODEL sparc_v9_models[] =
{
  { "sparc64-def", & sparc_v9_mach, MODEL_SPARC64_DEF, TIMING_DATA (& sparc64_def_timing[0]), sparc64_def_model_init },
  { 0 }
};

/* The properties of this cpu's implementation.  */

static const MACH_IMP_PROPERTIES sparc64_imp_properties =
{
  sizeof (SIM_CPU),
#if WITH_SCACHE
  sizeof (SCACHE)
#else
  0
#endif
};


static void
sparc64_prepare_run (SIM_CPU *cpu)
{
  if (CPU_IDESC (cpu) == NULL)
    sparc64_init_idesc_table (cpu);
}

static const CGEN_INSN *
sparc64_get_idata (SIM_CPU *cpu, int inum)
{
  return CPU_IDESC (cpu) [inum].idata;
}

static void
sparc_v9_init_cpu (SIM_CPU *cpu)
{
  CPU_REG_FETCH (cpu) = sparc64_fetch_register;
  CPU_REG_STORE (cpu) = sparc64_store_register;
  CPU_PC_FETCH (cpu) = sparc64_h_pc_get;
  CPU_PC_STORE (cpu) = sparc64_h_pc_set;
  CPU_GET_IDATA (cpu) = sparc64_get_idata;
  CPU_MAX_INSNS (cpu) = SPARC64_INSN_MAX;
  CPU_INSN_NAME (cpu) = cgen_insn_name;
  CPU_FULL_ENGINE_FN (cpu) = sparc64_engine_run_full;
#if WITH_FAST
  CPU_FAST_ENGINE_FN (cpu) = sparc64_engine_run_fast;
#else
  CPU_FAST_ENGINE_FN (cpu) = sparc64_engine_run_full;
#endif
}

const MACH sparc_v9_mach =
{
  "sparc-v9", "sparc_v9",
  64, 64, & sparc_v9_models[0], & sparc64_imp_properties,
  sparc_v9_init_cpu,
  sparc64_prepare_run
};

