/* Simulator instruction extractor for m32r.

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
#define WANT_CPU_M32R

#include "sim-main.h"
#include "cpu-sim.h"

void
EX_FN_NAME (m32r,fmt_0_add) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_0_add.f
  EXTRACT_FMT_0_ADD_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_0_ADD_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_0_add", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

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
EX_FN_NAME (m32r,fmt_1_add3) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_1_add3.f
  EXTRACT_FMT_1_ADD3_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_FMT_1_ADD3_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  FLD (f_simm16) = f_simm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_1_add3", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, "slo16 0x%x", 'x', f_simm16, (char *) 0));

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
EX_FN_NAME (m32r,fmt_2_and3) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_2_and3.f
  EXTRACT_FMT_2_AND3_VARS /* f-op1 f-r1 f-op2 f-r2 f-uimm16 */

  EXTRACT_FMT_2_AND3_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  FLD (f_uimm16) = f_uimm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_2_and3", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, "uimm16 0x%x", 'x', f_uimm16, (char *) 0));

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
EX_FN_NAME (m32r,fmt_3_or3) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_3_or3.f
  EXTRACT_FMT_3_OR3_VARS /* f-op1 f-r1 f-op2 f-r2 f-uimm16 */

  EXTRACT_FMT_3_OR3_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  FLD (f_uimm16) = f_uimm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_3_or3", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, "ulo16 0x%x", 'x', f_uimm16, (char *) 0));

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
EX_FN_NAME (m32r,fmt_4_addi) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_4_addi.f
  EXTRACT_FMT_4_ADDI_VARS /* f-op1 f-r1 f-simm8 */

  EXTRACT_FMT_4_ADDI_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_simm8) = f_simm8;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_4_addi", "dr 0x%x", 'x', f_r1, "simm8 0x%x", 'x', f_simm8, (char *) 0));

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
EX_FN_NAME (m32r,fmt_5_addv3) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_5_addv3.f
  EXTRACT_FMT_5_ADDV3_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_FMT_5_ADDV3_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  FLD (f_simm16) = f_simm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_5_addv3", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, "simm16 0x%x", 'x', f_simm16, (char *) 0));

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
EX_FN_NAME (m32r,fmt_6_addx) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_6_addx.f
  EXTRACT_FMT_6_ADDX_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_6_ADDX_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_6_addx", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

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
EX_FN_NAME (m32r,fmt_7_bc8) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_7_bc8.f
  EXTRACT_FMT_7_BC8_VARS /* f-op1 f-r1 f-disp8 */

  EXTRACT_FMT_7_BC8_CODE

  /* Record the fields for the semantic handler.  */
  RECORD_IADDR (FLD (f_disp8), (pc & -4L) + f_disp8);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_7_bc8", "disp8 0x%x", 'x', f_disp8, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;
#undef FLD
}

void
EX_FN_NAME (m32r,fmt_8_bc24) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_8_bc24.f
  EXTRACT_FMT_8_BC24_VARS /* f-op1 f-r1 f-disp24 */

  EXTRACT_FMT_8_BC24_CODE

  /* Record the fields for the semantic handler.  */
  RECORD_IADDR (FLD (f_disp24), pc + f_disp24);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_8_bc24", "disp24 0x%x", 'x', f_disp24, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;
#undef FLD
}

void
EX_FN_NAME (m32r,fmt_9_beq) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_9_beq.f
  EXTRACT_FMT_9_BEQ_VARS /* f-op1 f-r1 f-op2 f-r2 f-disp16 */

  EXTRACT_FMT_9_BEQ_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  RECORD_IADDR (FLD (f_disp16), pc + f_disp16);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_9_beq", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, "disp16 0x%x", 'x', f_disp16, (char *) 0));

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
EX_FN_NAME (m32r,fmt_10_beqz) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_10_beqz.f
  EXTRACT_FMT_10_BEQZ_VARS /* f-op1 f-r1 f-op2 f-r2 f-disp16 */

  EXTRACT_FMT_10_BEQZ_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  RECORD_IADDR (FLD (f_disp16), pc + f_disp16);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_10_beqz", "src2 0x%x", 'x', f_r2, "disp16 0x%x", 'x', f_disp16, (char *) 0));

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
EX_FN_NAME (m32r,fmt_11_bl8) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_11_bl8.f
  EXTRACT_FMT_11_BL8_VARS /* f-op1 f-r1 f-disp8 */

  EXTRACT_FMT_11_BL8_CODE

  /* Record the fields for the semantic handler.  */
  RECORD_IADDR (FLD (f_disp8), (pc & -4L) + f_disp8);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_11_bl8", "disp8 0x%x", 'x', f_disp8, (char *) 0));

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
EX_FN_NAME (m32r,fmt_12_bl24) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_12_bl24.f
  EXTRACT_FMT_12_BL24_VARS /* f-op1 f-r1 f-disp24 */

  EXTRACT_FMT_12_BL24_CODE

  /* Record the fields for the semantic handler.  */
  RECORD_IADDR (FLD (f_disp24), pc + f_disp24);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_12_bl24", "disp24 0x%x", 'x', f_disp24, (char *) 0));

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
EX_FN_NAME (m32r,fmt_13_bra8) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_13_bra8.f
  EXTRACT_FMT_13_BRA8_VARS /* f-op1 f-r1 f-disp8 */

  EXTRACT_FMT_13_BRA8_CODE

  /* Record the fields for the semantic handler.  */
  RECORD_IADDR (FLD (f_disp8), (pc & -4L) + f_disp8);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_13_bra8", "disp8 0x%x", 'x', f_disp8, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;
#undef FLD
}

void
EX_FN_NAME (m32r,fmt_14_bra24) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_14_bra24.f
  EXTRACT_FMT_14_BRA24_VARS /* f-op1 f-r1 f-disp24 */

  EXTRACT_FMT_14_BRA24_CODE

  /* Record the fields for the semantic handler.  */
  RECORD_IADDR (FLD (f_disp24), pc + f_disp24);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_14_bra24", "disp24 0x%x", 'x', f_disp24, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;
#undef FLD
}

void
EX_FN_NAME (m32r,fmt_15_cmp) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_15_cmp.f
  EXTRACT_FMT_15_CMP_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_15_CMP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_15_cmp", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

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
EX_FN_NAME (m32r,fmt_16_cmpi) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_16_cmpi.f
  EXTRACT_FMT_16_CMPI_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_FMT_16_CMPI_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  FLD (f_simm16) = f_simm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_16_cmpi", "src2 0x%x", 'x', f_r2, "simm16 0x%x", 'x', f_simm16, (char *) 0));

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
EX_FN_NAME (m32r,fmt_17_cmpui) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_17_cmpui.f
  EXTRACT_FMT_17_CMPUI_VARS /* f-op1 f-r1 f-op2 f-r2 f-uimm16 */

  EXTRACT_FMT_17_CMPUI_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  FLD (f_uimm16) = f_uimm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_17_cmpui", "src2 0x%x", 'x', f_r2, "uimm16 0x%x", 'x', f_uimm16, (char *) 0));

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
EX_FN_NAME (m32r,fmt_18_div) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_18_div.f
  EXTRACT_FMT_18_DIV_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_FMT_18_DIV_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_18_div", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

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
EX_FN_NAME (m32r,fmt_19_jl) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_19_jl.f
  EXTRACT_FMT_19_JL_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_19_JL_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_19_jl", "sr 0x%x", 'x', f_r2, (char *) 0));

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
EX_FN_NAME (m32r,fmt_20_jmp) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_20_jmp.f
  EXTRACT_FMT_20_JMP_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_20_JMP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_20_jmp", "sr 0x%x", 'x', f_r2, (char *) 0));

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
EX_FN_NAME (m32r,fmt_21_ld) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_21_ld.f
  EXTRACT_FMT_21_LD_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_21_LD_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_21_ld", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

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
EX_FN_NAME (m32r,fmt_22_ld_d) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_22_ld_d.f
  EXTRACT_FMT_22_LD_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_FMT_22_LD_D_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  FLD (f_simm16) = f_simm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_22_ld_d", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, "slo16 0x%x", 'x', f_simm16, (char *) 0));

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
EX_FN_NAME (m32r,fmt_23_ldb) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_23_ldb.f
  EXTRACT_FMT_23_LDB_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_23_LDB_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_23_ldb", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

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
EX_FN_NAME (m32r,fmt_24_ldb_d) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_24_ldb_d.f
  EXTRACT_FMT_24_LDB_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_FMT_24_LDB_D_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  FLD (f_simm16) = f_simm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_24_ldb_d", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, "slo16 0x%x", 'x', f_simm16, (char *) 0));

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
EX_FN_NAME (m32r,fmt_25_ldh) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_25_ldh.f
  EXTRACT_FMT_25_LDH_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_25_LDH_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_25_ldh", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

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
EX_FN_NAME (m32r,fmt_26_ldh_d) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_26_ldh_d.f
  EXTRACT_FMT_26_LDH_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_FMT_26_LDH_D_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  FLD (f_simm16) = f_simm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_26_ldh_d", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, "slo16 0x%x", 'x', f_simm16, (char *) 0));

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
EX_FN_NAME (m32r,fmt_27_ld24) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_27_ld24.f
  EXTRACT_FMT_27_LD24_VARS /* f-op1 f-r1 f-uimm24 */

  EXTRACT_FMT_27_LD24_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_uimm24) = f_uimm24;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_27_ld24", "dr 0x%x", 'x', f_r1, "uimm24 0x%x", 'x', f_uimm24, (char *) 0));

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
EX_FN_NAME (m32r,fmt_28_ldi8) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_28_ldi8.f
  EXTRACT_FMT_28_LDI8_VARS /* f-op1 f-r1 f-simm8 */

  EXTRACT_FMT_28_LDI8_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_simm8) = f_simm8;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_28_ldi8", "dr 0x%x", 'x', f_r1, "simm8 0x%x", 'x', f_simm8, (char *) 0));

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
EX_FN_NAME (m32r,fmt_29_ldi16) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_29_ldi16.f
  EXTRACT_FMT_29_LDI16_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_FMT_29_LDI16_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_simm16) = f_simm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_29_ldi16", "dr 0x%x", 'x', f_r1, "slo16 0x%x", 'x', f_simm16, (char *) 0));

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
EX_FN_NAME (m32r,fmt_30_machi) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_30_machi.f
  EXTRACT_FMT_30_MACHI_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_30_MACHI_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_30_machi", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

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
EX_FN_NAME (m32r,fmt_31_mv) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_31_mv.f
  EXTRACT_FMT_31_MV_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_31_MV_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_31_mv", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

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
EX_FN_NAME (m32r,fmt_32_mvfachi) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_32_mvfachi.f
  EXTRACT_FMT_32_MVFACHI_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_32_MVFACHI_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_32_mvfachi", "dr 0x%x", 'x', f_r1, (char *) 0));

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
EX_FN_NAME (m32r,fmt_33_mvfc) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_33_mvfc.f
  EXTRACT_FMT_33_MVFC_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_33_MVFC_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = f_r2;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_33_mvfc", "dr 0x%x", 'x', f_r1, "scr 0x%x", 'x', f_r2, (char *) 0));

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
EX_FN_NAME (m32r,fmt_34_mvtachi) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_34_mvtachi.f
  EXTRACT_FMT_34_MVTACHI_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_34_MVTACHI_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_34_mvtachi", "src1 0x%x", 'x', f_r1, (char *) 0));

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
EX_FN_NAME (m32r,fmt_35_mvtc) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_35_mvtc.f
  EXTRACT_FMT_35_MVTC_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_35_MVTC_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = f_r1;
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_35_mvtc", "dcr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

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
EX_FN_NAME (m32r,fmt_36_nop) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_36_nop.f
  EXTRACT_FMT_36_NOP_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_36_NOP_CODE

  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_36_nop", (char *) 0));

  abuf->length = length;
  abuf->addr = pc;
#undef FLD
}

void
EX_FN_NAME (m32r,fmt_37_rac) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_37_rac.f
  EXTRACT_FMT_37_RAC_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_37_RAC_CODE

  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_37_rac", (char *) 0));

  abuf->length = length;
  abuf->addr = pc;
#undef FLD
}

void
EX_FN_NAME (m32r,fmt_38_rte) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_38_rte.f
  EXTRACT_FMT_38_RTE_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_38_RTE_CODE

  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_38_rte", (char *) 0));

  abuf->length = length;
  abuf->addr = pc;
#undef FLD
}

void
EX_FN_NAME (m32r,fmt_39_seth) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_39_seth.f
  EXTRACT_FMT_39_SETH_VARS /* f-op1 f-r1 f-op2 f-r2 f-hi16 */

  EXTRACT_FMT_39_SETH_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_hi16) = f_hi16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_39_seth", "dr 0x%x", 'x', f_r1, "hi16 0x%x", 'x', f_hi16, (char *) 0));

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
EX_FN_NAME (m32r,fmt_40_slli) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_40_slli.f
  EXTRACT_FMT_40_SLLI_VARS /* f-op1 f-r1 f-shift-op2 f-uimm5 */

  EXTRACT_FMT_40_SLLI_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_uimm5) = f_uimm5;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_40_slli", "dr 0x%x", 'x', f_r1, "uimm5 0x%x", 'x', f_uimm5, (char *) 0));

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
EX_FN_NAME (m32r,fmt_41_st_d) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_41_st_d.f
  EXTRACT_FMT_41_ST_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_FMT_41_ST_D_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  FLD (f_simm16) = f_simm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_41_st_d", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, "slo16 0x%x", 'x', f_simm16, (char *) 0));

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
EX_FN_NAME (m32r,fmt_42_trap) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_42_trap.f
  EXTRACT_FMT_42_TRAP_VARS /* f-op1 f-r1 f-op2 f-uimm4 */

  EXTRACT_FMT_42_TRAP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_uimm4) = f_uimm4;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_42_trap", "uimm4 0x%x", 'x', f_uimm4, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;
#undef FLD
}

void
EX_FN_NAME (m32r,illegal) (SIM_CPU *cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
  abuf->length = CGEN_BASE_INSN_SIZE;
  abuf->addr = pc;
  /* Leave signalling to semantic fn.  */
}

#if 0 /*wip*/
void
EXC_FN_NAME (m32r,illegal) (SIM_CPU *cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
  abuf->length = CGEN_BASE_INSN_SIZE;
  abuf->addr = pc;
  /* Leave signalling to semantic fn.  */
}
#endif
