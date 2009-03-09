/* Simulator model support for arc600f.

THIS FILE IS MACHINE GENERATED WITH CGEN.

Copyright 1996-2007 Free Software Foundation, Inc.

This file is part of the GNU simulators.

   This file is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   It is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.

*/

#define WANT_CPU arc600f
#define WANT_CPU_ARC600F

#include "sim-main.h"

/* The profiling data is recorded here, but is accessed via the profiling
   mechanism.  After all, this is information for profiling.  */

#if WITH_PROFILE_MODEL_P

/* Model handlers for each insn.  */

static int
model_ARC600_b_s (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_b_s.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_bcc_s (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_bcc_s.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_brcc_s (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_brcc_s.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_bcc_l (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_bcc_l.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_bcc_l_d (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_bcc_l.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_b_l (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_b_l.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_b_l_d (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_b_l.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_brcc_RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_brcc_RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_brcc_RC_d (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_brcc_RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_brcc_U6 (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_brcc_U6.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_brcc_U6_d (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_brcc_U6.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_bl_s (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_bl_s.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_blcc (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_blcc.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_blcc_d (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_blcc.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_bl (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_bl.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_bl_d (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_bl.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ld_abs (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ld_abs.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ld__AW_abs (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ld_abs.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ld_ab_abs (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ld_abs.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ld_as_abs (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ld_abs.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ld_abc (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ld__AW_abc (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ld_ab_abc (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ld_as_abc (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ld_s_abc (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ld_s_abc.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ld_s_abu (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ld_s_abu.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ld_s_absp (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ld_s_abu.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ld_s_gprel (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ld_s_gprel.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ld_s_pcrel (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ld_s_pcrel.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ldb_abs (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ld_abs.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ldb__AW_abs (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ld_abs.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ldb_ab_abs (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ld_abs.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ldb_as_abs (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ld_abs.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ldb_abc (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ldb__AW_abc (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ldb_ab_abc (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ldb_as_abc (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ldb_s_abc (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ld_s_abc.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ldb_s_abu (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ldb_s_abu.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ldb_s_absp (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ld_s_abu.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ldb_s_gprel (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ldb_s_gprel.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ldb_x_abs (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ld_abs.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ldb__AW_x_abs (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ld_abs.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ldb_ab_x_abs (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ld_abs.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ldb_as_x_abs (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ld_abs.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ldb_x_abc (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ldb__AW_x_abc (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ldb_ab_x_abc (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ldb_as_x_abc (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ldw_abs (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ld_abs.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ldw__AW_abs (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ld_abs.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ldw_ab_abs (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ld_abs.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ldw_as_abs (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ld_abs.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ldw_abc (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ldw__AW_abc (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ldw_ab_abc (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ldw_as_abc (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ldw_s_abc (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ld_s_abc.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ldw_s_abu (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ldw_s_abu.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ldw_s_gprel (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ldw_s_gprel.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ldw_x_abs (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ld_abs.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ldw__AW_x_abs (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ld_abs.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ldw_ab_x_abs (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ld_abs.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ldw_as_x_abs (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ld_abs.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ldw_x_abc (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ldw__AW_x_abc (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ldw_ab_x_abc (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ldw_as_x_abc (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ldw_s_x_abu (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ldw_s_abu.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_st_abs (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_st_abs.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_st__AW_abs (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_st_abs.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_st_ab_abs (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_st_abs.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_st_as_abs (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_st_abs.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_st_s_abu (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ld_s_abu.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_st_s_absp (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ld_s_abu.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_stb_abs (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_st_abs.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_stb__AW_abs (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_st_abs.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_stb_ab_abs (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_st_abs.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_stb_as_abs (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_st_abs.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_stb_s_abu (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ldb_s_abu.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_stb_s_absp (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ld_s_abu.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_stw_abs (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_st_abs.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_stw__AW_abs (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_st_abs.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_stw_ab_abs (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_st_abs.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_stw_as_abs (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_st_abs.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_stw_s_abu (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ldw_s_abu.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_add_L_s12__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_add_ccu6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_add_L_u6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_add_L_r_r__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_add_cc__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_add_s_abc (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ld_s_abc.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_add_s_cbu3 (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_add_s_mcah (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_s_mcah.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_add_s_absp (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ld_s_abu.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_add_s_asspsp (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ld_s_abu.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_add_s_gp (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ld_s_gprel.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_add_s_r_u7 (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_s_r_u7.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_adc_L_s12__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_adc_ccu6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_adc_L_u6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_adc_L_r_r__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_adc_cc__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_sub_L_s12__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_sub_ccu6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_sub_L_u6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_sub_L_r_r__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_sub_cc__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_sub_s_cbu3 (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_I16_GO_SUB_s_go (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_sub_s_go_sub_ne (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_mov_s_r_u7.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_sub_s_ssb (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ldb_s_abu.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_sub_s_asspsp (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ld_s_abu.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_sbc_L_s12__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_sbc_ccu6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_sbc_L_u6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_sbc_L_r_r__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_sbc_cc__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_and_L_s12__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_and_ccu6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_and_L_u6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_and_L_r_r__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_and_cc__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_I16_GO_AND_s_go (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_or_L_s12__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_or_ccu6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_or_L_u6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_or_L_r_r__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_or_cc__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_I16_GO_OR_s_go (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_bic_L_s12__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_bic_ccu6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_bic_L_u6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_bic_L_r_r__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_bic_cc__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_I16_GO_BIC_s_go (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_xor_L_s12__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_xor_ccu6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_xor_L_u6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_xor_L_r_r__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_xor_cc__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_I16_GO_XOR_s_go (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_max_L_s12__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_max_ccu6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_max_L_u6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_max_L_r_r__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_max_cc__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_min_L_s12__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_min_ccu6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_min_L_u6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_min_L_r_r__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_min_cc__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mov_L_s12_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mov_ccu6_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mov_L_u6_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mov_L_r_r__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mov_cc__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mov_s_mcah (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_s_mcah.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mov_s_mcahb (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_s_mcah.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mov_s_r_u7 (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_mov_s_r_u7.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_tst_L_s12_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_tst_ccu6_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_tst_L_u6_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_tst_L_r_r__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_st_abs.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_tst_cc__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_tst_s_go (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_cmp_L_s12_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_cmp_ccu6_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_cmp_L_u6_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_cmp_L_r_r__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_st_abs.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_cmp_cc__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_cmp_s_mcah (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_s_mcah.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_cmp_s_r_u7 (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_s_r_u7.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_rcmp_L_s12_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_rcmp_ccu6_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_rcmp_L_u6_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_rcmp_L_r_r__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_st_abs.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_rcmp_cc__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_rsub_L_s12__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_rsub_ccu6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_rsub_L_u6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_rsub_L_r_r__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_rsub_cc__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_bset_L_s12__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_bset_ccu6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_bset_L_u6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_bset_L_r_r__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_bset_cc__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_bset_s_ssb (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ldb_s_abu.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_bclr_L_s12__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_bclr_ccu6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_bclr_L_u6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_bclr_L_r_r__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_bclr_cc__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_bclr_s_ssb (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ldb_s_abu.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_btst_L_s12_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_btst_ccu6_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_btst_L_u6_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_btst_L_r_r__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_st_abs.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_btst_cc__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_btst_s_ssb (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ldb_s_abu.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_bxor_L_s12__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_bxor_ccu6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_bxor_L_u6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_bxor_L_r_r__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_bxor_cc__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_bmsk_L_s12__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_bmsk_ccu6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_bmsk_L_u6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_bmsk_L_r_r__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_bmsk_cc__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_bmsk_s_ssb (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ldb_s_abu.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_add1_L_s12__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_add1_ccu6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_add1_L_u6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_add1_L_r_r__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_add1_cc__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_I16_GO_ADD1_s_go (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_add2_L_s12__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_add2_ccu6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_add2_L_u6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_add2_L_r_r__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_add2_cc__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_I16_GO_ADD2_s_go (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_add3_L_s12__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_add3_ccu6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_add3_L_u6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_add3_L_r_r__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_add3_cc__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_I16_GO_ADD3_s_go (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_sub1_L_s12__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_sub1_ccu6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_sub1_L_u6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_sub1_L_r_r__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_sub1_cc__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_sub2_L_s12__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_sub2_ccu6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_sub2_L_u6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_sub2_L_r_r__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_sub2_cc__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_sub3_L_s12__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_sub3_ccu6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_sub3_L_u6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_sub3_L_r_r__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_sub3_cc__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mpy_L_s12__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mpy_ccu6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mpy_L_u6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mpy_L_r_r__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mpy_cc__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mpyh_L_s12__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mpyh_ccu6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mpyh_L_u6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mpyh_L_r_r__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mpyh_cc__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mpyhu_L_s12__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mpyhu_ccu6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mpyhu_L_u6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mpyhu_L_r_r__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mpyhu_cc__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mpyu_L_s12__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mpyu_ccu6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mpyu_L_u6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mpyu_L_r_r__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mpyu_cc__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_j_L_r_r___RC_noilink_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_j_cc___RC_noilink_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_j_L_r_r___RC_ilink_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_j_cc___RC_ilink_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_j_L_s12_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_j_ccu6_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_j_L_u6_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_j_s (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_mov_s_r_u7.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_j_s__S (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.fmt_empty.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_j_seq__S (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.fmt_empty.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_j_sne__S (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.fmt_empty.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_j_L_s12_d_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_j_ccu6_d_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_j_L_u6_d_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_j_L_r_r_d___RC_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_st_abs.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_j_cc_d___RC_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_j_s_d (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_mov_s_r_u7.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_j_s__S_d (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.fmt_empty.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_jl_L_s12_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_jl_ccu6_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_jl_L_u6_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_jl_s (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_mov_s_r_u7.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_jl_L_r_r___RC_noilink_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_jl_cc___RC_noilink_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_jl_L_s12_d_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_jl_ccu6_d_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_jl_L_u6_d_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_jl_L_r_r_d___RC_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_st_abs.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_jl_cc_d___RC_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_jl_s_d (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_mov_s_r_u7.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_lp_L_s12_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_lp_L_s12_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_lpcc_ccu6 (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_lpcc_ccu6.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_flag_L_s12_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_flag_ccu6_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_flag_L_u6_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_flag_L_r_r__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_st_abs.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_flag_cc__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_lr_L_r_r___RC_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_st_abs.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_lr_L_s12_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_lr_L_u6_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_sr_L_r_r___RC_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_st_abs.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_sr_L_s12_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_sr_L_u6_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_asl_L_r_r__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_asl_L_u6_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_I16_GO_ASL_s_go (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_asr_L_r_r__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_asr_L_u6_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_I16_GO_ASR_s_go (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_lsr_L_r_r__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_lsr_L_u6_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_I16_GO_LSR_s_go (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ror_L_r_r__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ror_L_u6_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_rrc_L_r_r__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_rrc_L_u6_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_sexb_L_r_r__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_sexb_L_u6_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_I16_GO_SEXB_s_go (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_sexw_L_r_r__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_sexw_L_u6_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_I16_GO_SEXW_s_go (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_extb_L_r_r__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_extb_L_u6_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_I16_GO_EXTB_s_go (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_extw_L_r_r__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_extw_L_u6_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_I16_GO_EXTW_s_go (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_abs_L_r_r__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_abs_L_u6_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_I16_GO_ABS_s_go (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_not_L_r_r__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_not_L_u6_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_I16_GO_NOT_s_go (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_rlc_L_r_r__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_rlc_L_u6_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_I16_GO_NEG_s_go (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_swi (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.fmt_empty.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_trap_s (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_trap_s.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_brk (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.fmt_empty.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_brk_s (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.fmt_empty.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_asl_L_s12__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_asl_ccu6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_asl_L_u6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_asl_L_r_r__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_asl_cc__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_asl_s_cbu3 (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_asl_s_ssb (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ldb_s_abu.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_I16_GO_ASLM_s_go (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_lsr_L_s12__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_lsr_ccu6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_lsr_L_u6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_lsr_L_r_r__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_lsr_cc__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_lsr_s_ssb (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ldb_s_abu.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_I16_GO_LSRM_s_go (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_asr_L_s12__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_asr_ccu6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_asr_L_u6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_asr_L_r_r__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_asr_cc__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_asr_s_cbu3 (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_asr_s_ssb (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_ldb_s_abu.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_I16_GO_ASRM_s_go (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ror_L_s12__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ror_ccu6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ror_L_u6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ror_L_r_r__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_ror_cc__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mul64_L_s12_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mul64_ccu6_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mul64_L_u6_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mul64_L_r_r__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_st_abs.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mul64_cc__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mul64_s_go (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_s_cbu3.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mulu64_L_s12_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mulu64_ccu6_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mulu64_L_u6_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mulu64_L_r_r__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_st_abs.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mulu64_cc__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_adds_L_s12__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_adds_ccu6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_adds_L_u6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_adds_L_r_r__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_adds_cc__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_subs_L_s12__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_subs_ccu6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_subs_L_u6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_subs_L_r_r__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_subs_cc__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_divaw_L_s12__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_divaw_ccu6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_divaw_L_u6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_divaw_L_r_r__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_divaw_cc__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_asls_L_s12__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_asls_ccu6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_asls_L_u6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_asls_L_r_r__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_asls_cc__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_asrs_L_s12__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_asrs_ccu6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_asrs_L_u6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_asrs_L_r_r__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_asrs_cc__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_addsdw_L_s12__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_addsdw_ccu6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_addsdw_L_u6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_addsdw_L_r_r__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_addsdw_cc__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_subsdw_L_s12__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_subsdw_ccu6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_subsdw_L_u6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_subsdw_L_r_r__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_subsdw_cc__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_swap_L_r_r__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_swap_L_u6_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_norm_L_r_r__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_norm_L_u6_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_rnd16_L_r_r__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_rnd16_L_u6_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_abssw_L_r_r__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_abssw_L_u6_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_abss_L_r_r__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_abss_L_u6_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_negsw_L_r_r__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_negsw_L_u6_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_negs_L_r_r__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_negs_L_u6_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_normw_L_r_r__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_normw_L_u6_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_nop_s (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.fmt_empty.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_unimp_s (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.fmt_empty.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_pop_s_b (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_mov_s_r_u7.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_pop_s_blink (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.fmt_empty.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_push_s_b (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_mov_s_r_u7.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_push_s_blink (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.fmt_empty.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mullw_L_s12__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mullw_ccu6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mullw_L_u6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mullw_L_r_r__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mullw_cc__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_maclw_L_s12__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_maclw_ccu6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_maclw_L_u6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_maclw_L_r_r__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_maclw_cc__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_machlw_L_s12__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_machlw_ccu6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_machlw_L_u6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_machlw_L_r_r__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_machlw_cc__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mululw_L_s12__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mululw_ccu6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mululw_L_u6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mululw_L_r_r__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_mululw_cc__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_machulw_L_s12__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_s12__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_machulw_ccu6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_ccu6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_machulw_L_u6__RA_ (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_u6__RA_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_machulw_L_r_r__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_add_L_r_r__RA__RC.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_machulw_cc__RA__RC (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.sfmt_j_cc___RC_noilink_.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_current_loop_end (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.fmt_empty.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_current_loop_end_after_branch (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.fmt_empty.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

static int
model_ARC600_arc600_current_loop_end_after_branch (SIM_CPU *current_cpu, void *sem_arg)
{
#define FLD(f) abuf->fields.fmt_empty.f
  const ARGBUF * UNUSED abuf = SEM_ARGBUF ((SEM_ARG) sem_arg);
  const IDESC * UNUSED idesc = abuf->idesc;
  int cycles = 0;
  {
    int referenced = 0;
    int UNUSED insn_referenced = abuf->written;
    INT in_b = -1;
    INT in_c = -1;
    INT out_a = -1;
    cycles += arc600f_model_ARC600_u_exec (current_cpu, idesc, 0, referenced, in_b, in_c, out_a);
  }
  return cycles;
#undef FLD
}

/* We assume UNIT_NONE == 0 because the tables don't always terminate
   entries with it.  */

/* Model timing data for `ARC600'.  */

static const INSN_TIMING ARC600_timing[] = {
  { ARC600F_INSN_X_INVALID, 0, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_X_AFTER, 0, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_X_BEFORE, 0, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_X_CTI_CHAIN, 0, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_X_CHAIN, 0, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_X_BEGIN, 0, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_B_S, model_ARC600_b_s, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BCC_S, model_ARC600_bcc_s, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BRCC_S, model_ARC600_brcc_s, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BCC_L, model_ARC600_bcc_l, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BCC_L_D, model_ARC600_bcc_l_d, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_B_L, model_ARC600_b_l, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_B_L_D, model_ARC600_b_l_d, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BRCC_RC, model_ARC600_brcc_RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BRCC_RC_D, model_ARC600_brcc_RC_d, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BRCC_U6, model_ARC600_brcc_U6, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BRCC_U6_D, model_ARC600_brcc_U6_d, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BL_S, model_ARC600_bl_s, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BLCC, model_ARC600_blcc, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BLCC_D, model_ARC600_blcc_d, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BL, model_ARC600_bl, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BL_D, model_ARC600_bl_d, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LD_ABS, model_ARC600_ld_abs, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LD__AW_ABS, model_ARC600_ld__AW_abs, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LD_AB_ABS, model_ARC600_ld_ab_abs, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LD_AS_ABS, model_ARC600_ld_as_abs, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LD_ABC, model_ARC600_ld_abc, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LD__AW_ABC, model_ARC600_ld__AW_abc, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LD_AB_ABC, model_ARC600_ld_ab_abc, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LD_AS_ABC, model_ARC600_ld_as_abc, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LD_S_ABC, model_ARC600_ld_s_abc, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LD_S_ABU, model_ARC600_ld_s_abu, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LD_S_ABSP, model_ARC600_ld_s_absp, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LD_S_GPREL, model_ARC600_ld_s_gprel, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LD_S_PCREL, model_ARC600_ld_s_pcrel, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LDB_ABS, model_ARC600_ldb_abs, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LDB__AW_ABS, model_ARC600_ldb__AW_abs, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LDB_AB_ABS, model_ARC600_ldb_ab_abs, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LDB_AS_ABS, model_ARC600_ldb_as_abs, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LDB_ABC, model_ARC600_ldb_abc, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LDB__AW_ABC, model_ARC600_ldb__AW_abc, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LDB_AB_ABC, model_ARC600_ldb_ab_abc, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LDB_AS_ABC, model_ARC600_ldb_as_abc, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LDB_S_ABC, model_ARC600_ldb_s_abc, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LDB_S_ABU, model_ARC600_ldb_s_abu, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LDB_S_ABSP, model_ARC600_ldb_s_absp, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LDB_S_GPREL, model_ARC600_ldb_s_gprel, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LDB_X_ABS, model_ARC600_ldb_x_abs, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LDB__AW_X_ABS, model_ARC600_ldb__AW_x_abs, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LDB_AB_X_ABS, model_ARC600_ldb_ab_x_abs, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LDB_AS_X_ABS, model_ARC600_ldb_as_x_abs, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LDB_X_ABC, model_ARC600_ldb_x_abc, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LDB__AW_X_ABC, model_ARC600_ldb__AW_x_abc, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LDB_AB_X_ABC, model_ARC600_ldb_ab_x_abc, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LDB_AS_X_ABC, model_ARC600_ldb_as_x_abc, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LDW_ABS, model_ARC600_ldw_abs, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LDW__AW_ABS, model_ARC600_ldw__AW_abs, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LDW_AB_ABS, model_ARC600_ldw_ab_abs, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LDW_AS_ABS, model_ARC600_ldw_as_abs, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LDW_ABC, model_ARC600_ldw_abc, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LDW__AW_ABC, model_ARC600_ldw__AW_abc, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LDW_AB_ABC, model_ARC600_ldw_ab_abc, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LDW_AS_ABC, model_ARC600_ldw_as_abc, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LDW_S_ABC, model_ARC600_ldw_s_abc, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LDW_S_ABU, model_ARC600_ldw_s_abu, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LDW_S_GPREL, model_ARC600_ldw_s_gprel, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LDW_X_ABS, model_ARC600_ldw_x_abs, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LDW__AW_X_ABS, model_ARC600_ldw__AW_x_abs, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LDW_AB_X_ABS, model_ARC600_ldw_ab_x_abs, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LDW_AS_X_ABS, model_ARC600_ldw_as_x_abs, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LDW_X_ABC, model_ARC600_ldw_x_abc, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LDW__AW_X_ABC, model_ARC600_ldw__AW_x_abc, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LDW_AB_X_ABC, model_ARC600_ldw_ab_x_abc, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LDW_AS_X_ABC, model_ARC600_ldw_as_x_abc, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LDW_S_X_ABU, model_ARC600_ldw_s_x_abu, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ST_ABS, model_ARC600_st_abs, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ST__AW_ABS, model_ARC600_st__AW_abs, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ST_AB_ABS, model_ARC600_st_ab_abs, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ST_AS_ABS, model_ARC600_st_as_abs, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ST_S_ABU, model_ARC600_st_s_abu, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ST_S_ABSP, model_ARC600_st_s_absp, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_STB_ABS, model_ARC600_stb_abs, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_STB__AW_ABS, model_ARC600_stb__AW_abs, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_STB_AB_ABS, model_ARC600_stb_ab_abs, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_STB_AS_ABS, model_ARC600_stb_as_abs, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_STB_S_ABU, model_ARC600_stb_s_abu, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_STB_S_ABSP, model_ARC600_stb_s_absp, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_STW_ABS, model_ARC600_stw_abs, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_STW__AW_ABS, model_ARC600_stw__AW_abs, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_STW_AB_ABS, model_ARC600_stw_ab_abs, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_STW_AS_ABS, model_ARC600_stw_as_abs, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_STW_S_ABU, model_ARC600_stw_s_abu, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ADD_L_S12__RA_, model_ARC600_add_L_s12__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ADD_CCU6__RA_, model_ARC600_add_ccu6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ADD_L_U6__RA_, model_ARC600_add_L_u6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ADD_L_R_R__RA__RC, model_ARC600_add_L_r_r__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ADD_CC__RA__RC, model_ARC600_add_cc__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ADD_S_ABC, model_ARC600_add_s_abc, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ADD_S_CBU3, model_ARC600_add_s_cbu3, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ADD_S_MCAH, model_ARC600_add_s_mcah, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ADD_S_ABSP, model_ARC600_add_s_absp, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ADD_S_ASSPSP, model_ARC600_add_s_asspsp, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ADD_S_GP, model_ARC600_add_s_gp, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ADD_S_R_U7, model_ARC600_add_s_r_u7, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ADC_L_S12__RA_, model_ARC600_adc_L_s12__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ADC_CCU6__RA_, model_ARC600_adc_ccu6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ADC_L_U6__RA_, model_ARC600_adc_L_u6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ADC_L_R_R__RA__RC, model_ARC600_adc_L_r_r__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ADC_CC__RA__RC, model_ARC600_adc_cc__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SUB_L_S12__RA_, model_ARC600_sub_L_s12__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SUB_CCU6__RA_, model_ARC600_sub_ccu6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SUB_L_U6__RA_, model_ARC600_sub_L_u6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SUB_L_R_R__RA__RC, model_ARC600_sub_L_r_r__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SUB_CC__RA__RC, model_ARC600_sub_cc__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SUB_S_CBU3, model_ARC600_sub_s_cbu3, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_I16_GO_SUB_S_GO, model_ARC600_I16_GO_SUB_s_go, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SUB_S_GO_SUB_NE, model_ARC600_sub_s_go_sub_ne, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SUB_S_SSB, model_ARC600_sub_s_ssb, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SUB_S_ASSPSP, model_ARC600_sub_s_asspsp, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SBC_L_S12__RA_, model_ARC600_sbc_L_s12__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SBC_CCU6__RA_, model_ARC600_sbc_ccu6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SBC_L_U6__RA_, model_ARC600_sbc_L_u6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SBC_L_R_R__RA__RC, model_ARC600_sbc_L_r_r__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SBC_CC__RA__RC, model_ARC600_sbc_cc__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_AND_L_S12__RA_, model_ARC600_and_L_s12__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_AND_CCU6__RA_, model_ARC600_and_ccu6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_AND_L_U6__RA_, model_ARC600_and_L_u6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_AND_L_R_R__RA__RC, model_ARC600_and_L_r_r__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_AND_CC__RA__RC, model_ARC600_and_cc__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_I16_GO_AND_S_GO, model_ARC600_I16_GO_AND_s_go, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_OR_L_S12__RA_, model_ARC600_or_L_s12__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_OR_CCU6__RA_, model_ARC600_or_ccu6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_OR_L_U6__RA_, model_ARC600_or_L_u6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_OR_L_R_R__RA__RC, model_ARC600_or_L_r_r__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_OR_CC__RA__RC, model_ARC600_or_cc__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_I16_GO_OR_S_GO, model_ARC600_I16_GO_OR_s_go, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BIC_L_S12__RA_, model_ARC600_bic_L_s12__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BIC_CCU6__RA_, model_ARC600_bic_ccu6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BIC_L_U6__RA_, model_ARC600_bic_L_u6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BIC_L_R_R__RA__RC, model_ARC600_bic_L_r_r__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BIC_CC__RA__RC, model_ARC600_bic_cc__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_I16_GO_BIC_S_GO, model_ARC600_I16_GO_BIC_s_go, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_XOR_L_S12__RA_, model_ARC600_xor_L_s12__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_XOR_CCU6__RA_, model_ARC600_xor_ccu6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_XOR_L_U6__RA_, model_ARC600_xor_L_u6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_XOR_L_R_R__RA__RC, model_ARC600_xor_L_r_r__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_XOR_CC__RA__RC, model_ARC600_xor_cc__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_I16_GO_XOR_S_GO, model_ARC600_I16_GO_XOR_s_go, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MAX_L_S12__RA_, model_ARC600_max_L_s12__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MAX_CCU6__RA_, model_ARC600_max_ccu6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MAX_L_U6__RA_, model_ARC600_max_L_u6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MAX_L_R_R__RA__RC, model_ARC600_max_L_r_r__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MAX_CC__RA__RC, model_ARC600_max_cc__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MIN_L_S12__RA_, model_ARC600_min_L_s12__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MIN_CCU6__RA_, model_ARC600_min_ccu6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MIN_L_U6__RA_, model_ARC600_min_L_u6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MIN_L_R_R__RA__RC, model_ARC600_min_L_r_r__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MIN_CC__RA__RC, model_ARC600_min_cc__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MOV_L_S12_, model_ARC600_mov_L_s12_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MOV_CCU6_, model_ARC600_mov_ccu6_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MOV_L_U6_, model_ARC600_mov_L_u6_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MOV_L_R_R__RC, model_ARC600_mov_L_r_r__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MOV_CC__RC, model_ARC600_mov_cc__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MOV_S_MCAH, model_ARC600_mov_s_mcah, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MOV_S_MCAHB, model_ARC600_mov_s_mcahb, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MOV_S_R_U7, model_ARC600_mov_s_r_u7, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_TST_L_S12_, model_ARC600_tst_L_s12_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_TST_CCU6_, model_ARC600_tst_ccu6_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_TST_L_U6_, model_ARC600_tst_L_u6_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_TST_L_R_R__RC, model_ARC600_tst_L_r_r__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_TST_CC__RC, model_ARC600_tst_cc__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_TST_S_GO, model_ARC600_tst_s_go, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_CMP_L_S12_, model_ARC600_cmp_L_s12_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_CMP_CCU6_, model_ARC600_cmp_ccu6_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_CMP_L_U6_, model_ARC600_cmp_L_u6_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_CMP_L_R_R__RC, model_ARC600_cmp_L_r_r__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_CMP_CC__RC, model_ARC600_cmp_cc__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_CMP_S_MCAH, model_ARC600_cmp_s_mcah, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_CMP_S_R_U7, model_ARC600_cmp_s_r_u7, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_RCMP_L_S12_, model_ARC600_rcmp_L_s12_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_RCMP_CCU6_, model_ARC600_rcmp_ccu6_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_RCMP_L_U6_, model_ARC600_rcmp_L_u6_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_RCMP_L_R_R__RC, model_ARC600_rcmp_L_r_r__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_RCMP_CC__RC, model_ARC600_rcmp_cc__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_RSUB_L_S12__RA_, model_ARC600_rsub_L_s12__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_RSUB_CCU6__RA_, model_ARC600_rsub_ccu6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_RSUB_L_U6__RA_, model_ARC600_rsub_L_u6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_RSUB_L_R_R__RA__RC, model_ARC600_rsub_L_r_r__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_RSUB_CC__RA__RC, model_ARC600_rsub_cc__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BSET_L_S12__RA_, model_ARC600_bset_L_s12__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BSET_CCU6__RA_, model_ARC600_bset_ccu6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BSET_L_U6__RA_, model_ARC600_bset_L_u6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BSET_L_R_R__RA__RC, model_ARC600_bset_L_r_r__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BSET_CC__RA__RC, model_ARC600_bset_cc__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BSET_S_SSB, model_ARC600_bset_s_ssb, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BCLR_L_S12__RA_, model_ARC600_bclr_L_s12__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BCLR_CCU6__RA_, model_ARC600_bclr_ccu6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BCLR_L_U6__RA_, model_ARC600_bclr_L_u6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BCLR_L_R_R__RA__RC, model_ARC600_bclr_L_r_r__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BCLR_CC__RA__RC, model_ARC600_bclr_cc__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BCLR_S_SSB, model_ARC600_bclr_s_ssb, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BTST_L_S12_, model_ARC600_btst_L_s12_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BTST_CCU6_, model_ARC600_btst_ccu6_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BTST_L_U6_, model_ARC600_btst_L_u6_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BTST_L_R_R__RC, model_ARC600_btst_L_r_r__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BTST_CC__RC, model_ARC600_btst_cc__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BTST_S_SSB, model_ARC600_btst_s_ssb, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BXOR_L_S12__RA_, model_ARC600_bxor_L_s12__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BXOR_CCU6__RA_, model_ARC600_bxor_ccu6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BXOR_L_U6__RA_, model_ARC600_bxor_L_u6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BXOR_L_R_R__RA__RC, model_ARC600_bxor_L_r_r__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BXOR_CC__RA__RC, model_ARC600_bxor_cc__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BMSK_L_S12__RA_, model_ARC600_bmsk_L_s12__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BMSK_CCU6__RA_, model_ARC600_bmsk_ccu6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BMSK_L_U6__RA_, model_ARC600_bmsk_L_u6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BMSK_L_R_R__RA__RC, model_ARC600_bmsk_L_r_r__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BMSK_CC__RA__RC, model_ARC600_bmsk_cc__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BMSK_S_SSB, model_ARC600_bmsk_s_ssb, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ADD1_L_S12__RA_, model_ARC600_add1_L_s12__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ADD1_CCU6__RA_, model_ARC600_add1_ccu6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ADD1_L_U6__RA_, model_ARC600_add1_L_u6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ADD1_L_R_R__RA__RC, model_ARC600_add1_L_r_r__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ADD1_CC__RA__RC, model_ARC600_add1_cc__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_I16_GO_ADD1_S_GO, model_ARC600_I16_GO_ADD1_s_go, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ADD2_L_S12__RA_, model_ARC600_add2_L_s12__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ADD2_CCU6__RA_, model_ARC600_add2_ccu6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ADD2_L_U6__RA_, model_ARC600_add2_L_u6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ADD2_L_R_R__RA__RC, model_ARC600_add2_L_r_r__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ADD2_CC__RA__RC, model_ARC600_add2_cc__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_I16_GO_ADD2_S_GO, model_ARC600_I16_GO_ADD2_s_go, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ADD3_L_S12__RA_, model_ARC600_add3_L_s12__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ADD3_CCU6__RA_, model_ARC600_add3_ccu6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ADD3_L_U6__RA_, model_ARC600_add3_L_u6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ADD3_L_R_R__RA__RC, model_ARC600_add3_L_r_r__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ADD3_CC__RA__RC, model_ARC600_add3_cc__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_I16_GO_ADD3_S_GO, model_ARC600_I16_GO_ADD3_s_go, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SUB1_L_S12__RA_, model_ARC600_sub1_L_s12__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SUB1_CCU6__RA_, model_ARC600_sub1_ccu6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SUB1_L_U6__RA_, model_ARC600_sub1_L_u6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SUB1_L_R_R__RA__RC, model_ARC600_sub1_L_r_r__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SUB1_CC__RA__RC, model_ARC600_sub1_cc__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SUB2_L_S12__RA_, model_ARC600_sub2_L_s12__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SUB2_CCU6__RA_, model_ARC600_sub2_ccu6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SUB2_L_U6__RA_, model_ARC600_sub2_L_u6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SUB2_L_R_R__RA__RC, model_ARC600_sub2_L_r_r__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SUB2_CC__RA__RC, model_ARC600_sub2_cc__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SUB3_L_S12__RA_, model_ARC600_sub3_L_s12__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SUB3_CCU6__RA_, model_ARC600_sub3_ccu6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SUB3_L_U6__RA_, model_ARC600_sub3_L_u6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SUB3_L_R_R__RA__RC, model_ARC600_sub3_L_r_r__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SUB3_CC__RA__RC, model_ARC600_sub3_cc__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MPY_L_S12__RA_, model_ARC600_mpy_L_s12__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MPY_CCU6__RA_, model_ARC600_mpy_ccu6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MPY_L_U6__RA_, model_ARC600_mpy_L_u6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MPY_L_R_R__RA__RC, model_ARC600_mpy_L_r_r__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MPY_CC__RA__RC, model_ARC600_mpy_cc__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MPYH_L_S12__RA_, model_ARC600_mpyh_L_s12__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MPYH_CCU6__RA_, model_ARC600_mpyh_ccu6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MPYH_L_U6__RA_, model_ARC600_mpyh_L_u6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MPYH_L_R_R__RA__RC, model_ARC600_mpyh_L_r_r__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MPYH_CC__RA__RC, model_ARC600_mpyh_cc__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MPYHU_L_S12__RA_, model_ARC600_mpyhu_L_s12__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MPYHU_CCU6__RA_, model_ARC600_mpyhu_ccu6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MPYHU_L_U6__RA_, model_ARC600_mpyhu_L_u6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MPYHU_L_R_R__RA__RC, model_ARC600_mpyhu_L_r_r__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MPYHU_CC__RA__RC, model_ARC600_mpyhu_cc__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MPYU_L_S12__RA_, model_ARC600_mpyu_L_s12__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MPYU_CCU6__RA_, model_ARC600_mpyu_ccu6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MPYU_L_U6__RA_, model_ARC600_mpyu_L_u6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MPYU_L_R_R__RA__RC, model_ARC600_mpyu_L_r_r__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MPYU_CC__RA__RC, model_ARC600_mpyu_cc__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_J_L_R_R___RC_NOILINK_, model_ARC600_j_L_r_r___RC_noilink_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_J_CC___RC_NOILINK_, model_ARC600_j_cc___RC_noilink_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_J_L_R_R___RC_ILINK_, model_ARC600_j_L_r_r___RC_ilink_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_J_CC___RC_ILINK_, model_ARC600_j_cc___RC_ilink_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_J_L_S12_, model_ARC600_j_L_s12_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_J_CCU6_, model_ARC600_j_ccu6_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_J_L_U6_, model_ARC600_j_L_u6_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_J_S, model_ARC600_j_s, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_J_S__S, model_ARC600_j_s__S, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_J_SEQ__S, model_ARC600_j_seq__S, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_J_SNE__S, model_ARC600_j_sne__S, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_J_L_S12_D_, model_ARC600_j_L_s12_d_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_J_CCU6_D_, model_ARC600_j_ccu6_d_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_J_L_U6_D_, model_ARC600_j_L_u6_d_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_J_L_R_R_D___RC_, model_ARC600_j_L_r_r_d___RC_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_J_CC_D___RC_, model_ARC600_j_cc_d___RC_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_J_S_D, model_ARC600_j_s_d, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_J_S__S_D, model_ARC600_j_s__S_d, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_JL_L_S12_, model_ARC600_jl_L_s12_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_JL_CCU6_, model_ARC600_jl_ccu6_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_JL_L_U6_, model_ARC600_jl_L_u6_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_JL_S, model_ARC600_jl_s, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_JL_L_R_R___RC_NOILINK_, model_ARC600_jl_L_r_r___RC_noilink_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_JL_CC___RC_NOILINK_, model_ARC600_jl_cc___RC_noilink_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_JL_L_S12_D_, model_ARC600_jl_L_s12_d_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_JL_CCU6_D_, model_ARC600_jl_ccu6_d_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_JL_L_U6_D_, model_ARC600_jl_L_u6_d_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_JL_L_R_R_D___RC_, model_ARC600_jl_L_r_r_d___RC_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_JL_CC_D___RC_, model_ARC600_jl_cc_d___RC_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_JL_S_D, model_ARC600_jl_s_d, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LP_L_S12_, model_ARC600_lp_L_s12_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LPCC_CCU6, model_ARC600_lpcc_ccu6, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_FLAG_L_S12_, model_ARC600_flag_L_s12_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_FLAG_CCU6_, model_ARC600_flag_ccu6_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_FLAG_L_U6_, model_ARC600_flag_L_u6_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_FLAG_L_R_R__RC, model_ARC600_flag_L_r_r__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_FLAG_CC__RC, model_ARC600_flag_cc__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LR_L_R_R___RC_, model_ARC600_lr_L_r_r___RC_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LR_L_S12_, model_ARC600_lr_L_s12_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LR_L_U6_, model_ARC600_lr_L_u6_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SR_L_R_R___RC_, model_ARC600_sr_L_r_r___RC_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SR_L_S12_, model_ARC600_sr_L_s12_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SR_L_U6_, model_ARC600_sr_L_u6_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ASL_L_R_R__RC, model_ARC600_asl_L_r_r__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ASL_L_U6_, model_ARC600_asl_L_u6_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_I16_GO_ASL_S_GO, model_ARC600_I16_GO_ASL_s_go, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ASR_L_R_R__RC, model_ARC600_asr_L_r_r__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ASR_L_U6_, model_ARC600_asr_L_u6_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_I16_GO_ASR_S_GO, model_ARC600_I16_GO_ASR_s_go, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LSR_L_R_R__RC, model_ARC600_lsr_L_r_r__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LSR_L_U6_, model_ARC600_lsr_L_u6_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_I16_GO_LSR_S_GO, model_ARC600_I16_GO_LSR_s_go, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ROR_L_R_R__RC, model_ARC600_ror_L_r_r__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ROR_L_U6_, model_ARC600_ror_L_u6_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_RRC_L_R_R__RC, model_ARC600_rrc_L_r_r__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_RRC_L_U6_, model_ARC600_rrc_L_u6_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SEXB_L_R_R__RC, model_ARC600_sexb_L_r_r__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SEXB_L_U6_, model_ARC600_sexb_L_u6_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_I16_GO_SEXB_S_GO, model_ARC600_I16_GO_SEXB_s_go, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SEXW_L_R_R__RC, model_ARC600_sexw_L_r_r__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SEXW_L_U6_, model_ARC600_sexw_L_u6_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_I16_GO_SEXW_S_GO, model_ARC600_I16_GO_SEXW_s_go, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_EXTB_L_R_R__RC, model_ARC600_extb_L_r_r__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_EXTB_L_U6_, model_ARC600_extb_L_u6_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_I16_GO_EXTB_S_GO, model_ARC600_I16_GO_EXTB_s_go, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_EXTW_L_R_R__RC, model_ARC600_extw_L_r_r__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_EXTW_L_U6_, model_ARC600_extw_L_u6_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_I16_GO_EXTW_S_GO, model_ARC600_I16_GO_EXTW_s_go, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ABS_L_R_R__RC, model_ARC600_abs_L_r_r__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ABS_L_U6_, model_ARC600_abs_L_u6_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_I16_GO_ABS_S_GO, model_ARC600_I16_GO_ABS_s_go, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_NOT_L_R_R__RC, model_ARC600_not_L_r_r__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_NOT_L_U6_, model_ARC600_not_L_u6_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_I16_GO_NOT_S_GO, model_ARC600_I16_GO_NOT_s_go, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_RLC_L_R_R__RC, model_ARC600_rlc_L_r_r__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_RLC_L_U6_, model_ARC600_rlc_L_u6_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_I16_GO_NEG_S_GO, model_ARC600_I16_GO_NEG_s_go, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SWI, model_ARC600_swi, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_TRAP_S, model_ARC600_trap_s, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BRK, model_ARC600_brk, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_BRK_S, model_ARC600_brk_s, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ASL_L_S12__RA_, model_ARC600_asl_L_s12__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ASL_CCU6__RA_, model_ARC600_asl_ccu6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ASL_L_U6__RA_, model_ARC600_asl_L_u6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ASL_L_R_R__RA__RC, model_ARC600_asl_L_r_r__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ASL_CC__RA__RC, model_ARC600_asl_cc__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ASL_S_CBU3, model_ARC600_asl_s_cbu3, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ASL_S_SSB, model_ARC600_asl_s_ssb, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_I16_GO_ASLM_S_GO, model_ARC600_I16_GO_ASLM_s_go, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LSR_L_S12__RA_, model_ARC600_lsr_L_s12__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LSR_CCU6__RA_, model_ARC600_lsr_ccu6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LSR_L_U6__RA_, model_ARC600_lsr_L_u6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LSR_L_R_R__RA__RC, model_ARC600_lsr_L_r_r__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LSR_CC__RA__RC, model_ARC600_lsr_cc__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_LSR_S_SSB, model_ARC600_lsr_s_ssb, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_I16_GO_LSRM_S_GO, model_ARC600_I16_GO_LSRM_s_go, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ASR_L_S12__RA_, model_ARC600_asr_L_s12__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ASR_CCU6__RA_, model_ARC600_asr_ccu6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ASR_L_U6__RA_, model_ARC600_asr_L_u6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ASR_L_R_R__RA__RC, model_ARC600_asr_L_r_r__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ASR_CC__RA__RC, model_ARC600_asr_cc__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ASR_S_CBU3, model_ARC600_asr_s_cbu3, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ASR_S_SSB, model_ARC600_asr_s_ssb, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_I16_GO_ASRM_S_GO, model_ARC600_I16_GO_ASRM_s_go, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ROR_L_S12__RA_, model_ARC600_ror_L_s12__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ROR_CCU6__RA_, model_ARC600_ror_ccu6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ROR_L_U6__RA_, model_ARC600_ror_L_u6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ROR_L_R_R__RA__RC, model_ARC600_ror_L_r_r__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ROR_CC__RA__RC, model_ARC600_ror_cc__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MUL64_L_S12_, model_ARC600_mul64_L_s12_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MUL64_CCU6_, model_ARC600_mul64_ccu6_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MUL64_L_U6_, model_ARC600_mul64_L_u6_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MUL64_L_R_R__RC, model_ARC600_mul64_L_r_r__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MUL64_CC__RC, model_ARC600_mul64_cc__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MUL64_S_GO, model_ARC600_mul64_s_go, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MULU64_L_S12_, model_ARC600_mulu64_L_s12_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MULU64_CCU6_, model_ARC600_mulu64_ccu6_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MULU64_L_U6_, model_ARC600_mulu64_L_u6_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MULU64_L_R_R__RC, model_ARC600_mulu64_L_r_r__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MULU64_CC__RC, model_ARC600_mulu64_cc__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ADDS_L_S12__RA_, model_ARC600_adds_L_s12__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ADDS_CCU6__RA_, model_ARC600_adds_ccu6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ADDS_L_U6__RA_, model_ARC600_adds_L_u6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ADDS_L_R_R__RA__RC, model_ARC600_adds_L_r_r__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ADDS_CC__RA__RC, model_ARC600_adds_cc__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SUBS_L_S12__RA_, model_ARC600_subs_L_s12__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SUBS_CCU6__RA_, model_ARC600_subs_ccu6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SUBS_L_U6__RA_, model_ARC600_subs_L_u6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SUBS_L_R_R__RA__RC, model_ARC600_subs_L_r_r__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SUBS_CC__RA__RC, model_ARC600_subs_cc__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_DIVAW_L_S12__RA_, model_ARC600_divaw_L_s12__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_DIVAW_CCU6__RA_, model_ARC600_divaw_ccu6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_DIVAW_L_U6__RA_, model_ARC600_divaw_L_u6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_DIVAW_L_R_R__RA__RC, model_ARC600_divaw_L_r_r__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_DIVAW_CC__RA__RC, model_ARC600_divaw_cc__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ASLS_L_S12__RA_, model_ARC600_asls_L_s12__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ASLS_CCU6__RA_, model_ARC600_asls_ccu6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ASLS_L_U6__RA_, model_ARC600_asls_L_u6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ASLS_L_R_R__RA__RC, model_ARC600_asls_L_r_r__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ASLS_CC__RA__RC, model_ARC600_asls_cc__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ASRS_L_S12__RA_, model_ARC600_asrs_L_s12__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ASRS_CCU6__RA_, model_ARC600_asrs_ccu6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ASRS_L_U6__RA_, model_ARC600_asrs_L_u6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ASRS_L_R_R__RA__RC, model_ARC600_asrs_L_r_r__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ASRS_CC__RA__RC, model_ARC600_asrs_cc__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ADDSDW_L_S12__RA_, model_ARC600_addsdw_L_s12__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ADDSDW_CCU6__RA_, model_ARC600_addsdw_ccu6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ADDSDW_L_U6__RA_, model_ARC600_addsdw_L_u6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ADDSDW_L_R_R__RA__RC, model_ARC600_addsdw_L_r_r__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ADDSDW_CC__RA__RC, model_ARC600_addsdw_cc__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SUBSDW_L_S12__RA_, model_ARC600_subsdw_L_s12__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SUBSDW_CCU6__RA_, model_ARC600_subsdw_ccu6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SUBSDW_L_U6__RA_, model_ARC600_subsdw_L_u6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SUBSDW_L_R_R__RA__RC, model_ARC600_subsdw_L_r_r__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SUBSDW_CC__RA__RC, model_ARC600_subsdw_cc__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SWAP_L_R_R__RC, model_ARC600_swap_L_r_r__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_SWAP_L_U6_, model_ARC600_swap_L_u6_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_NORM_L_R_R__RC, model_ARC600_norm_L_r_r__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_NORM_L_U6_, model_ARC600_norm_L_u6_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_RND16_L_R_R__RC, model_ARC600_rnd16_L_r_r__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_RND16_L_U6_, model_ARC600_rnd16_L_u6_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ABSSW_L_R_R__RC, model_ARC600_abssw_L_r_r__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ABSSW_L_U6_, model_ARC600_abssw_L_u6_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ABSS_L_R_R__RC, model_ARC600_abss_L_r_r__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ABSS_L_U6_, model_ARC600_abss_L_u6_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_NEGSW_L_R_R__RC, model_ARC600_negsw_L_r_r__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_NEGSW_L_U6_, model_ARC600_negsw_L_u6_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_NEGS_L_R_R__RC, model_ARC600_negs_L_r_r__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_NEGS_L_U6_, model_ARC600_negs_L_u6_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_NORMW_L_R_R__RC, model_ARC600_normw_L_r_r__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_NORMW_L_U6_, model_ARC600_normw_L_u6_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_NOP_S, model_ARC600_nop_s, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_UNIMP_S, model_ARC600_unimp_s, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_POP_S_B, model_ARC600_pop_s_b, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_POP_S_BLINK, model_ARC600_pop_s_blink, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_PUSH_S_B, model_ARC600_push_s_b, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_PUSH_S_BLINK, model_ARC600_push_s_blink, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MULLW_L_S12__RA_, model_ARC600_mullw_L_s12__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MULLW_CCU6__RA_, model_ARC600_mullw_ccu6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MULLW_L_U6__RA_, model_ARC600_mullw_L_u6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MULLW_L_R_R__RA__RC, model_ARC600_mullw_L_r_r__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MULLW_CC__RA__RC, model_ARC600_mullw_cc__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MACLW_L_S12__RA_, model_ARC600_maclw_L_s12__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MACLW_CCU6__RA_, model_ARC600_maclw_ccu6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MACLW_L_U6__RA_, model_ARC600_maclw_L_u6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MACLW_L_R_R__RA__RC, model_ARC600_maclw_L_r_r__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MACLW_CC__RA__RC, model_ARC600_maclw_cc__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MACHLW_L_S12__RA_, model_ARC600_machlw_L_s12__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MACHLW_CCU6__RA_, model_ARC600_machlw_ccu6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MACHLW_L_U6__RA_, model_ARC600_machlw_L_u6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MACHLW_L_R_R__RA__RC, model_ARC600_machlw_L_r_r__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MACHLW_CC__RA__RC, model_ARC600_machlw_cc__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MULULW_L_S12__RA_, model_ARC600_mululw_L_s12__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MULULW_CCU6__RA_, model_ARC600_mululw_ccu6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MULULW_L_U6__RA_, model_ARC600_mululw_L_u6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MULULW_L_R_R__RA__RC, model_ARC600_mululw_L_r_r__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MULULW_CC__RA__RC, model_ARC600_mululw_cc__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MACHULW_L_S12__RA_, model_ARC600_machulw_L_s12__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MACHULW_CCU6__RA_, model_ARC600_machulw_ccu6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MACHULW_L_U6__RA_, model_ARC600_machulw_L_u6__RA_, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MACHULW_L_R_R__RA__RC, model_ARC600_machulw_L_r_r__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_MACHULW_CC__RA__RC, model_ARC600_machulw_cc__RA__RC, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_CURRENT_LOOP_END, model_ARC600_current_loop_end, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_CURRENT_LOOP_END_AFTER_BRANCH, model_ARC600_current_loop_end_after_branch, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
  { ARC600F_INSN_ARC600_CURRENT_LOOP_END_AFTER_BRANCH, model_ARC600_arc600_current_loop_end_after_branch, { { (int) UNIT_ARC600_U_EXEC, 1, 1 } } },
};

#endif /* WITH_PROFILE_MODEL_P */

static void
ARC600_model_init (SIM_CPU *cpu)
{
  CPU_MODEL_DATA (cpu) = (void *) zalloc (sizeof (MODEL_ARC600_DATA));
}

#if WITH_PROFILE_MODEL_P
#define TIMING_DATA(td) td
#else
#define TIMING_DATA(td) 0
#endif

static const MODEL arc600_models[] =
{
  { "ARC600", & arc600_mach, MODEL_ARC600, TIMING_DATA (& ARC600_timing[0]), ARC600_model_init },
  { 0 }
};

/* The properties of this cpu's implementation.  */

static const MACH_IMP_PROPERTIES arc600f_imp_properties =
{
  sizeof (SIM_CPU),
#if WITH_SCACHE
  sizeof (SCACHE)
#else
  0
#endif
};


static void
arc600f_prepare_run (SIM_CPU *cpu)
{
  if (CPU_IDESC (cpu) == NULL)
    arc600f_init_idesc_table (cpu);
}

static const CGEN_INSN *
arc600f_get_idata (SIM_CPU *cpu, int inum)
{
  return CPU_IDESC (cpu) [inum].idata;
}

static void
arc600_init_cpu (SIM_CPU *cpu)
{
  CPU_REG_FETCH (cpu) = arc600f_fetch_register;
  CPU_REG_STORE (cpu) = arc600f_store_register;
  CPU_PC_FETCH (cpu) = arc600f_h_pc_get;
  CPU_PC_STORE (cpu) = arc600f_h_pc_set;
  CPU_GET_IDATA (cpu) = arc600f_get_idata;
  CPU_MAX_INSNS (cpu) = ARC600F_INSN__MAX;
  CPU_INSN_NAME (cpu) = cgen_insn_name;
  CPU_FULL_ENGINE_FN (cpu) = arc600f_engine_run_full;
#if WITH_FAST
  CPU_FAST_ENGINE_FN (cpu) = arc600f_engine_run_fast;
#else
  CPU_FAST_ENGINE_FN (cpu) = arc600f_engine_run_full;
#endif
}

const MACH arc600_mach =
{
  "arc600", "ARC600", MACH_ARC600,
  32, 32, & arc600_models[0], & arc600f_imp_properties,
  arc600_init_cpu,
  arc600f_prepare_run
};

