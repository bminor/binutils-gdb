/* Simulator instruction extractor for m32r.

This file is machine generated.

Copyright (C) 1996, 1997 Free Software Foundation, Inc.

This file is part of the GNU Binutils and/or GDB, the GNU debugger.

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

#include "sim-main.h"
#include "decode.h"
#include "cpu-sim.h"

void
EX_FN_NAME (add) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_0.f
  /* Instruction fields.  */
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "add", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r1) | (1 << f_r2);
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (add3) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_1.f
  /* Instruction fields.  */
  int f_simm16;
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 4;
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16);
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  FLD (f_simm16) = f_simm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "add3", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, "slo16 0x%x", 'x', f_simm16, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r2);
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (and) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_0.f
  /* Instruction fields.  */
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "and", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r1) | (1 << f_r2);
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (and3) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_2.f
  /* Instruction fields.  */
  int f_uimm16;
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 4;
  f_uimm16 = EXTRACT_UNSIGNED (insn, 32, 16, 16);
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  FLD (f_uimm16) = f_uimm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "and3", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, "uimm16 0x%x", 'x', f_uimm16, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r2);
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (or) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_0.f
  /* Instruction fields.  */
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "or", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r1) | (1 << f_r2);
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (or3) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_3.f
  /* Instruction fields.  */
  int f_uimm16;
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 4;
  f_uimm16 = EXTRACT_UNSIGNED (insn, 32, 16, 16);
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  FLD (f_uimm16) = f_uimm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "or3", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, "ulo16 0x%x", 'x', f_uimm16, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r2);
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (xor) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_0.f
  /* Instruction fields.  */
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "xor", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r1) | (1 << f_r2);
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (xor3) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_2.f
  /* Instruction fields.  */
  int f_uimm16;
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 4;
  f_uimm16 = EXTRACT_UNSIGNED (insn, 32, 16, 16);
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  FLD (f_uimm16) = f_uimm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "xor3", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, "uimm16 0x%x", 'x', f_uimm16, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r2);
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (addi) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_4.f
  /* Instruction fields.  */
  int f_simm8;
  int f_r1;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_simm8 = EXTRACT_SIGNED (insn, 16, 8, 8);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_simm8) = f_simm8;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "addi", "dr 0x%x", 'x', f_r1, "simm8 0x%x", 'x', f_simm8, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r1);
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (addv) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_0.f
  /* Instruction fields.  */
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "addv", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r1) | (1 << f_r2);
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (addv3) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_5.f
  /* Instruction fields.  */
  int f_simm16;
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 4;
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16);
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  FLD (f_simm16) = f_simm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "addv3", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, "simm16 0x%x", 'x', f_simm16, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r2);
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (addx) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_0.f
  /* Instruction fields.  */
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "addx", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r1) | (1 << f_r2);
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (bc8) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_6.f
  /* Instruction fields.  */
  int f_disp8;
  int f_r1;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_disp8 = EXTRACT_SIGNED (insn, 16, 8, 8) << 2;
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  RECORD_IADDR (FLD (f_disp8), (pc & -4L) + f_disp8);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "bc8", "disp8 0x%x", 'x', f_disp8, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;
#undef FLD
}

void
EX_FN_NAME (bc24) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_7.f
  /* Instruction fields.  */
  int f_disp24;
  int f_r1;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 4;
  f_disp24 = EXTRACT_SIGNED (insn, 32, 8, 24) << 2;
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4);

  /* Record the fields for the semantic handler.  */
  RECORD_IADDR (FLD (f_disp24), pc + f_disp24);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "bc24", "disp24 0x%x", 'x', f_disp24, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;
#undef FLD
}

void
EX_FN_NAME (beq) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_8.f
  /* Instruction fields.  */
  int f_disp16;
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 4;
  f_disp16 = EXTRACT_SIGNED (insn, 32, 16, 16) << 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  RECORD_IADDR (FLD (f_disp16), pc + f_disp16);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "beq", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, "disp16 0x%x", 'x', f_disp16, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r1) | (1 << f_r2);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (beqz) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_9.f
  /* Instruction fields.  */
  int f_disp16;
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 4;
  f_disp16 = EXTRACT_SIGNED (insn, 32, 16, 16) << 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  RECORD_IADDR (FLD (f_disp16), pc + f_disp16);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "beqz", "src2 0x%x", 'x', f_r2, "disp16 0x%x", 'x', f_disp16, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r2);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (bgez) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_9.f
  /* Instruction fields.  */
  int f_disp16;
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 4;
  f_disp16 = EXTRACT_SIGNED (insn, 32, 16, 16) << 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  RECORD_IADDR (FLD (f_disp16), pc + f_disp16);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "bgez", "src2 0x%x", 'x', f_r2, "disp16 0x%x", 'x', f_disp16, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r2);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (bgtz) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_9.f
  /* Instruction fields.  */
  int f_disp16;
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 4;
  f_disp16 = EXTRACT_SIGNED (insn, 32, 16, 16) << 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  RECORD_IADDR (FLD (f_disp16), pc + f_disp16);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "bgtz", "src2 0x%x", 'x', f_r2, "disp16 0x%x", 'x', f_disp16, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r2);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (blez) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_9.f
  /* Instruction fields.  */
  int f_disp16;
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 4;
  f_disp16 = EXTRACT_SIGNED (insn, 32, 16, 16) << 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  RECORD_IADDR (FLD (f_disp16), pc + f_disp16);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "blez", "src2 0x%x", 'x', f_r2, "disp16 0x%x", 'x', f_disp16, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r2);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (bltz) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_9.f
  /* Instruction fields.  */
  int f_disp16;
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 4;
  f_disp16 = EXTRACT_SIGNED (insn, 32, 16, 16) << 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  RECORD_IADDR (FLD (f_disp16), pc + f_disp16);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "bltz", "src2 0x%x", 'x', f_r2, "disp16 0x%x", 'x', f_disp16, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r2);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (bnez) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_9.f
  /* Instruction fields.  */
  int f_disp16;
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 4;
  f_disp16 = EXTRACT_SIGNED (insn, 32, 16, 16) << 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  RECORD_IADDR (FLD (f_disp16), pc + f_disp16);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "bnez", "src2 0x%x", 'x', f_r2, "disp16 0x%x", 'x', f_disp16, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r2);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (bl8) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_6.f
  /* Instruction fields.  */
  int f_disp8;
  int f_r1;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_disp8 = EXTRACT_SIGNED (insn, 16, 8, 8) << 2;
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  RECORD_IADDR (FLD (f_disp8), (pc & -4L) + f_disp8);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "bl8", "disp8 0x%x", 'x', f_disp8, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_set = 0 | (1 << 14);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (bl24) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_7.f
  /* Instruction fields.  */
  int f_disp24;
  int f_r1;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 4;
  f_disp24 = EXTRACT_SIGNED (insn, 32, 8, 24) << 2;
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4);

  /* Record the fields for the semantic handler.  */
  RECORD_IADDR (FLD (f_disp24), pc + f_disp24);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "bl24", "disp24 0x%x", 'x', f_disp24, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_set = 0 | (1 << 14);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (bnc8) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_6.f
  /* Instruction fields.  */
  int f_disp8;
  int f_r1;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_disp8 = EXTRACT_SIGNED (insn, 16, 8, 8) << 2;
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  RECORD_IADDR (FLD (f_disp8), (pc & -4L) + f_disp8);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "bnc8", "disp8 0x%x", 'x', f_disp8, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;
#undef FLD
}

void
EX_FN_NAME (bnc24) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_7.f
  /* Instruction fields.  */
  int f_disp24;
  int f_r1;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 4;
  f_disp24 = EXTRACT_SIGNED (insn, 32, 8, 24) << 2;
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4);

  /* Record the fields for the semantic handler.  */
  RECORD_IADDR (FLD (f_disp24), pc + f_disp24);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "bnc24", "disp24 0x%x", 'x', f_disp24, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;
#undef FLD
}

void
EX_FN_NAME (bne) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_8.f
  /* Instruction fields.  */
  int f_disp16;
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 4;
  f_disp16 = EXTRACT_SIGNED (insn, 32, 16, 16) << 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  RECORD_IADDR (FLD (f_disp16), pc + f_disp16);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "bne", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, "disp16 0x%x", 'x', f_disp16, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r1) | (1 << f_r2);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (bra8) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_6.f
  /* Instruction fields.  */
  int f_disp8;
  int f_r1;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_disp8 = EXTRACT_SIGNED (insn, 16, 8, 8) << 2;
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  RECORD_IADDR (FLD (f_disp8), (pc & -4L) + f_disp8);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "bra8", "disp8 0x%x", 'x', f_disp8, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;
#undef FLD
}

void
EX_FN_NAME (bra24) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_7.f
  /* Instruction fields.  */
  int f_disp24;
  int f_r1;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 4;
  f_disp24 = EXTRACT_SIGNED (insn, 32, 8, 24) << 2;
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4);

  /* Record the fields for the semantic handler.  */
  RECORD_IADDR (FLD (f_disp24), pc + f_disp24);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "bra24", "disp24 0x%x", 'x', f_disp24, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;
#undef FLD
}

void
EX_FN_NAME (cmp) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_10.f
  /* Instruction fields.  */
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "cmp", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r1) | (1 << f_r2);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (cmpi) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_11.f
  /* Instruction fields.  */
  int f_simm16;
  int f_r2;
  int f_op2;
  int f_r1;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 4;
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16);
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  FLD (f_simm16) = f_simm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "cmpi", "src2 0x%x", 'x', f_r2, "simm16 0x%x", 'x', f_simm16, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r2);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (cmpu) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_10.f
  /* Instruction fields.  */
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "cmpu", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r1) | (1 << f_r2);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (cmpui) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_12.f
  /* Instruction fields.  */
  int f_uimm16;
  int f_r2;
  int f_op2;
  int f_r1;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 4;
  f_uimm16 = EXTRACT_UNSIGNED (insn, 32, 16, 16);
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  FLD (f_uimm16) = f_uimm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "cmpui", "src2 0x%x", 'x', f_r2, "uimm16 0x%x", 'x', f_uimm16, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r2);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (div) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_13.f
  /* Instruction fields.  */
  int f_simm16;
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 4;
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16);
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "div", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r1) | (1 << f_r2);
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (divu) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_13.f
  /* Instruction fields.  */
  int f_simm16;
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 4;
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16);
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "divu", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r1) | (1 << f_r2);
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (rem) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_13.f
  /* Instruction fields.  */
  int f_simm16;
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 4;
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16);
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "rem", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r1) | (1 << f_r2);
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (remu) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_13.f
  /* Instruction fields.  */
  int f_simm16;
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 4;
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16);
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "remu", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r1) | (1 << f_r2);
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (jl) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_14.f
  /* Instruction fields.  */
  int f_r2;
  int f_op2;
  int f_r1;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "jl", "sr 0x%x", 'x', f_r2, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r2);
      abuf->h_gr_set = 0 | (1 << 14);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (jmp) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_14.f
  /* Instruction fields.  */
  int f_r2;
  int f_op2;
  int f_r1;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "jmp", "sr 0x%x", 'x', f_r2, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r2);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (ld) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_0.f
  /* Instruction fields.  */
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "ld", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r2);
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (ld_d) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_1.f
  /* Instruction fields.  */
  int f_simm16;
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 4;
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16);
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  FLD (f_simm16) = f_simm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "ld_d", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, "slo16 0x%x", 'x', f_simm16, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r2);
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (ldb) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_0.f
  /* Instruction fields.  */
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "ldb", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r2);
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (ldb_d) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_1.f
  /* Instruction fields.  */
  int f_simm16;
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 4;
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16);
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  FLD (f_simm16) = f_simm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "ldb_d", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, "slo16 0x%x", 'x', f_simm16, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r2);
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (ldh) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_0.f
  /* Instruction fields.  */
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "ldh", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r2);
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (ldh_d) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_1.f
  /* Instruction fields.  */
  int f_simm16;
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 4;
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16);
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  FLD (f_simm16) = f_simm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "ldh_d", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, "slo16 0x%x", 'x', f_simm16, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r2);
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (ldub) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_0.f
  /* Instruction fields.  */
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "ldub", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r2);
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (ldub_d) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_1.f
  /* Instruction fields.  */
  int f_simm16;
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 4;
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16);
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  FLD (f_simm16) = f_simm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "ldub_d", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, "slo16 0x%x", 'x', f_simm16, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r2);
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (lduh) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_0.f
  /* Instruction fields.  */
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "lduh", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r2);
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (lduh_d) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_1.f
  /* Instruction fields.  */
  int f_simm16;
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 4;
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16);
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  FLD (f_simm16) = f_simm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "lduh_d", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, "slo16 0x%x", 'x', f_simm16, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r2);
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (ld_plus) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_0.f
  /* Instruction fields.  */
  int f_r2;
  int f_op2;
  int f_r1;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "ld_plus", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r2);
      abuf->h_gr_set = 0 | (1 << f_r1) | (1 << f_r2);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (ld24) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_15.f
  /* Instruction fields.  */
  int f_uimm24;
  int f_r1;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 4;
  f_uimm24 = EXTRACT_UNSIGNED (insn, 32, 8, 24);
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_uimm24) = f_uimm24;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "ld24", "dr 0x%x", 'x', f_r1, "uimm24 0x%x", 'x', f_uimm24, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (ldi8) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_4.f
  /* Instruction fields.  */
  int f_simm8;
  int f_r1;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_simm8 = EXTRACT_SIGNED (insn, 16, 8, 8);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_simm8) = f_simm8;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "ldi8", "dr 0x%x", 'x', f_r1, "simm8 0x%x", 'x', f_simm8, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (ldi16) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_16.f
  /* Instruction fields.  */
  int f_simm16;
  int f_r1;
  int f_r2;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 4;
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16);
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4);
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_simm16) = f_simm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "ldi16", "dr 0x%x", 'x', f_r1, "slo16 0x%x", 'x', f_simm16, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (lock) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_0.f
  /* Instruction fields.  */
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "lock", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r1) | (1 << f_r2);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (machi) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_10.f
  /* Instruction fields.  */
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "machi", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r1) | (1 << f_r2);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (maclo) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_10.f
  /* Instruction fields.  */
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "maclo", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r1) | (1 << f_r2);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (macwhi) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_10.f
  /* Instruction fields.  */
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "macwhi", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r1) | (1 << f_r2);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (macwlo) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_10.f
  /* Instruction fields.  */
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "macwlo", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r1) | (1 << f_r2);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (mul) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_0.f
  /* Instruction fields.  */
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "mul", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r1) | (1 << f_r2);
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (mulhi) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_10.f
  /* Instruction fields.  */
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "mulhi", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r1) | (1 << f_r2);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (mullo) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_10.f
  /* Instruction fields.  */
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "mullo", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r1) | (1 << f_r2);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (mulwhi) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_10.f
  /* Instruction fields.  */
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "mulwhi", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r1) | (1 << f_r2);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (mulwlo) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_10.f
  /* Instruction fields.  */
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "mulwlo", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r1) | (1 << f_r2);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (mv) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_0.f
  /* Instruction fields.  */
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "mv", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r2);
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (mvfachi) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_17.f
  /* Instruction fields.  */
  int f_r1;
  int f_r2;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "mvfachi", "dr 0x%x", 'x', f_r1, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (mvfaclo) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_17.f
  /* Instruction fields.  */
  int f_r1;
  int f_r2;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "mvfaclo", "dr 0x%x", 'x', f_r1, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (mvfacmi) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_17.f
  /* Instruction fields.  */
  int f_r1;
  int f_r2;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "mvfacmi", "dr 0x%x", 'x', f_r1, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (mvfc) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_18.f
  /* Instruction fields.  */
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = f_r2;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "mvfc", "dr 0x%x", 'x', f_r1, "scr 0x%x", 'x', f_r2, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (mvtachi) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_19.f
  /* Instruction fields.  */
  int f_r1;
  int f_r2;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "mvtachi", "src1 0x%x", 'x', f_r1, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (mvtaclo) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_19.f
  /* Instruction fields.  */
  int f_r1;
  int f_r2;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "mvtaclo", "src1 0x%x", 'x', f_r1, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (mvtc) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_20.f
  /* Instruction fields.  */
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = f_r1;
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "mvtc", "dcr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r2);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (neg) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_0.f
  /* Instruction fields.  */
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "neg", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r2);
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (nop) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_21.f
  /* Instruction fields.  */
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "nop", (char *) 0));

  abuf->length = length;
  abuf->addr = pc;
#undef FLD
}

void
EX_FN_NAME (not) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_0.f
  /* Instruction fields.  */
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "not", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r2);
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (rac) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_21.f
  /* Instruction fields.  */
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "rac", (char *) 0));

  abuf->length = length;
  abuf->addr = pc;
#undef FLD
}

void
EX_FN_NAME (rach) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_21.f
  /* Instruction fields.  */
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "rach", (char *) 0));

  abuf->length = length;
  abuf->addr = pc;
#undef FLD
}

void
EX_FN_NAME (rte) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_21.f
  /* Instruction fields.  */
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "rte", (char *) 0));

  abuf->length = length;
  abuf->addr = pc;
#undef FLD
}

void
EX_FN_NAME (seth) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_22.f
  /* Instruction fields.  */
  int f_hi16;
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 4;
  f_hi16 = EXTRACT_UNSIGNED (insn, 32, 16, 16);
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_hi16) = f_hi16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "seth", "dr 0x%x", 'x', f_r1, "hi16 0x%x", 'x', f_hi16, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (sll) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_0.f
  /* Instruction fields.  */
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "sll", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r1) | (1 << f_r2);
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (sll3) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_5.f
  /* Instruction fields.  */
  int f_simm16;
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 4;
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16);
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  FLD (f_simm16) = f_simm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "sll3", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, "simm16 0x%x", 'x', f_simm16, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r2);
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (slli) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_23.f
  /* Instruction fields.  */
  int f_uimm5;
  int f_r1;
  int f_shift_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_uimm5 = EXTRACT_UNSIGNED (insn, 16, 11, 5);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_shift_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 3);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_uimm5) = f_uimm5;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "slli", "dr 0x%x", 'x', f_r1, "uimm5 0x%x", 'x', f_uimm5, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r1);
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (sra) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_0.f
  /* Instruction fields.  */
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "sra", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r1) | (1 << f_r2);
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (sra3) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_5.f
  /* Instruction fields.  */
  int f_simm16;
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 4;
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16);
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  FLD (f_simm16) = f_simm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "sra3", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, "simm16 0x%x", 'x', f_simm16, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r2);
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (srai) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_23.f
  /* Instruction fields.  */
  int f_uimm5;
  int f_r1;
  int f_shift_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_uimm5 = EXTRACT_UNSIGNED (insn, 16, 11, 5);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_shift_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 3);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_uimm5) = f_uimm5;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "srai", "dr 0x%x", 'x', f_r1, "uimm5 0x%x", 'x', f_uimm5, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r1);
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (srl) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_0.f
  /* Instruction fields.  */
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "srl", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r1) | (1 << f_r2);
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (srl3) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_5.f
  /* Instruction fields.  */
  int f_simm16;
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 4;
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16);
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  FLD (f_simm16) = f_simm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "srl3", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, "simm16 0x%x", 'x', f_simm16, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r2);
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (srli) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_23.f
  /* Instruction fields.  */
  int f_uimm5;
  int f_r1;
  int f_shift_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_uimm5 = EXTRACT_UNSIGNED (insn, 16, 11, 5);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_shift_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 3);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_uimm5) = f_uimm5;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "srli", "dr 0x%x", 'x', f_r1, "uimm5 0x%x", 'x', f_uimm5, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r1);
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (st) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_10.f
  /* Instruction fields.  */
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "st", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r1) | (1 << f_r2);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (st_d) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_24.f
  /* Instruction fields.  */
  int f_simm16;
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 4;
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16);
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  FLD (f_simm16) = f_simm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "st_d", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, "slo16 0x%x", 'x', f_simm16, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r1) | (1 << f_r2);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (stb) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_10.f
  /* Instruction fields.  */
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "stb", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r1) | (1 << f_r2);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (stb_d) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_24.f
  /* Instruction fields.  */
  int f_simm16;
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 4;
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16);
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  FLD (f_simm16) = f_simm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "stb_d", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, "slo16 0x%x", 'x', f_simm16, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r1) | (1 << f_r2);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (sth) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_10.f
  /* Instruction fields.  */
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "sth", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r1) | (1 << f_r2);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (sth_d) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_24.f
  /* Instruction fields.  */
  int f_simm16;
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 4;
  f_simm16 = EXTRACT_SIGNED (insn, 32, 16, 16);
  f_r2 = EXTRACT_UNSIGNED (insn, 32, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 32, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 32, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 32, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  FLD (f_simm16) = f_simm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "sth_d", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, "slo16 0x%x", 'x', f_simm16, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r1) | (1 << f_r2);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (st_plus) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_10.f
  /* Instruction fields.  */
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "st_plus", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r1) | (1 << f_r2);
      abuf->h_gr_set = 0 | (1 << f_r2);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (st_minus) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_10.f
  /* Instruction fields.  */
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "st_minus", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r1) | (1 << f_r2);
      abuf->h_gr_set = 0 | (1 << f_r2);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (sub) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_0.f
  /* Instruction fields.  */
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "sub", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r1) | (1 << f_r2);
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (subv) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_0.f
  /* Instruction fields.  */
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "subv", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r1) | (1 << f_r2);
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (subx) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_0.f
  /* Instruction fields.  */
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "subx", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r1) | (1 << f_r2);
      abuf->h_gr_set = 0 | (1 << f_r1);
    }
#endif
#undef FLD
}

void
EX_FN_NAME (trap) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_25.f
  /* Instruction fields.  */
  int f_uimm4;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_uimm4 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_uimm4) = f_uimm4;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "trap", "uimm4 0x%x", 'x', f_uimm4, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;
#undef FLD
}

void
EX_FN_NAME (unlock) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_10.f
  /* Instruction fields.  */
  int f_r2;
  int f_r1;
  int f_op2;
  int f_op1;
  /* The length of the insn may be fixed or variable.  */
  int length;

  /* Extract the fields of the insn, computing the length as we go.  */
  length = 2;
  f_r2 = EXTRACT_UNSIGNED (insn, 16, 12, 4);
  f_r1 = EXTRACT_UNSIGNED (insn, 16, 4, 4);
  f_op2 = EXTRACT_UNSIGNED (insn, 16, 8, 4);
  f_op1 = EXTRACT_UNSIGNED (insn, 16, 0, 4);

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr[f_r1]);
  FLD (f_r2) = & CPU (h_gr[f_r2]);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "unlock", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      abuf->h_gr_get = 0 | (1 << f_r1) | (1 << f_r2);
    }
#endif
#undef FLD
}

