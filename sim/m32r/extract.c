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
EX_FN_NAME (m32r,fmt_5_addv) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_5_addv.f
  EXTRACT_FMT_5_ADDV_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_5_ADDV_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_5_addv", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

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
EX_FN_NAME (m32r,fmt_6_addv3) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_6_addv3.f
  EXTRACT_FMT_6_ADDV3_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_FMT_6_ADDV3_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  FLD (f_simm16) = f_simm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_6_addv3", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, "simm16 0x%x", 'x', f_simm16, (char *) 0));

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
EX_FN_NAME (m32r,fmt_7_addx) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_7_addx.f
  EXTRACT_FMT_7_ADDX_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_7_ADDX_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_7_addx", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

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
EX_FN_NAME (m32r,fmt_8_bc8) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_8_bc8.f
  EXTRACT_FMT_8_BC8_VARS /* f-op1 f-r1 f-disp8 */

  EXTRACT_FMT_8_BC8_CODE

  /* Record the fields for the semantic handler.  */
  RECORD_IADDR (FLD (f_disp8), (pc & -4L) + f_disp8);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_8_bc8", "disp8 0x%x", 'x', f_disp8, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;
#undef FLD
}

void
EX_FN_NAME (m32r,fmt_9_bc24) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_9_bc24.f
  EXTRACT_FMT_9_BC24_VARS /* f-op1 f-r1 f-disp24 */

  EXTRACT_FMT_9_BC24_CODE

  /* Record the fields for the semantic handler.  */
  RECORD_IADDR (FLD (f_disp24), pc + f_disp24);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_9_bc24", "disp24 0x%x", 'x', f_disp24, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;
#undef FLD
}

void
EX_FN_NAME (m32r,fmt_10_beq) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_10_beq.f
  EXTRACT_FMT_10_BEQ_VARS /* f-op1 f-r1 f-op2 f-r2 f-disp16 */

  EXTRACT_FMT_10_BEQ_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  RECORD_IADDR (FLD (f_disp16), pc + f_disp16);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_10_beq", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, "disp16 0x%x", 'x', f_disp16, (char *) 0));

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
EX_FN_NAME (m32r,fmt_11_beqz) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_11_beqz.f
  EXTRACT_FMT_11_BEQZ_VARS /* f-op1 f-r1 f-op2 f-r2 f-disp16 */

  EXTRACT_FMT_11_BEQZ_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  RECORD_IADDR (FLD (f_disp16), pc + f_disp16);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_11_beqz", "src2 0x%x", 'x', f_r2, "disp16 0x%x", 'x', f_disp16, (char *) 0));

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
EX_FN_NAME (m32r,fmt_12_bl8) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_12_bl8.f
  EXTRACT_FMT_12_BL8_VARS /* f-op1 f-r1 f-disp8 */

  EXTRACT_FMT_12_BL8_CODE

  /* Record the fields for the semantic handler.  */
  RECORD_IADDR (FLD (f_disp8), (pc & -4L) + f_disp8);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_12_bl8", "disp8 0x%x", 'x', f_disp8, (char *) 0));

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
EX_FN_NAME (m32r,fmt_13_bl24) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_13_bl24.f
  EXTRACT_FMT_13_BL24_VARS /* f-op1 f-r1 f-disp24 */

  EXTRACT_FMT_13_BL24_CODE

  /* Record the fields for the semantic handler.  */
  RECORD_IADDR (FLD (f_disp24), pc + f_disp24);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_13_bl24", "disp24 0x%x", 'x', f_disp24, (char *) 0));

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
EX_FN_NAME (m32r,fmt_14_bra8) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_14_bra8.f
  EXTRACT_FMT_14_BRA8_VARS /* f-op1 f-r1 f-disp8 */

  EXTRACT_FMT_14_BRA8_CODE

  /* Record the fields for the semantic handler.  */
  RECORD_IADDR (FLD (f_disp8), (pc & -4L) + f_disp8);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_14_bra8", "disp8 0x%x", 'x', f_disp8, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;
#undef FLD
}

void
EX_FN_NAME (m32r,fmt_15_bra24) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_15_bra24.f
  EXTRACT_FMT_15_BRA24_VARS /* f-op1 f-r1 f-disp24 */

  EXTRACT_FMT_15_BRA24_CODE

  /* Record the fields for the semantic handler.  */
  RECORD_IADDR (FLD (f_disp24), pc + f_disp24);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_15_bra24", "disp24 0x%x", 'x', f_disp24, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;
#undef FLD
}

void
EX_FN_NAME (m32r,fmt_16_cmp) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_16_cmp.f
  EXTRACT_FMT_16_CMP_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_16_CMP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_16_cmp", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

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
EX_FN_NAME (m32r,fmt_17_cmpi) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_17_cmpi.f
  EXTRACT_FMT_17_CMPI_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_FMT_17_CMPI_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  FLD (f_simm16) = f_simm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_17_cmpi", "src2 0x%x", 'x', f_r2, "simm16 0x%x", 'x', f_simm16, (char *) 0));

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
EX_FN_NAME (m32r,fmt_18_cmpui) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_18_cmpui.f
  EXTRACT_FMT_18_CMPUI_VARS /* f-op1 f-r1 f-op2 f-r2 f-uimm16 */

  EXTRACT_FMT_18_CMPUI_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  FLD (f_uimm16) = f_uimm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_18_cmpui", "src2 0x%x", 'x', f_r2, "uimm16 0x%x", 'x', f_uimm16, (char *) 0));

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
EX_FN_NAME (m32r,fmt_19_div) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_19_div.f
  EXTRACT_FMT_19_DIV_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_FMT_19_DIV_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_19_div", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

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
EX_FN_NAME (m32r,fmt_20_jl) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_20_jl.f
  EXTRACT_FMT_20_JL_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_20_JL_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_20_jl", "sr 0x%x", 'x', f_r2, (char *) 0));

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
EX_FN_NAME (m32r,fmt_21_jmp) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_21_jmp.f
  EXTRACT_FMT_21_JMP_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_21_JMP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_21_jmp", "sr 0x%x", 'x', f_r2, (char *) 0));

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
EX_FN_NAME (m32r,fmt_22_ld) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_22_ld.f
  EXTRACT_FMT_22_LD_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_22_LD_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_22_ld", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

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
EX_FN_NAME (m32r,fmt_23_ld_d) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_23_ld_d.f
  EXTRACT_FMT_23_LD_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_FMT_23_LD_D_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  FLD (f_simm16) = f_simm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_23_ld_d", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, "slo16 0x%x", 'x', f_simm16, (char *) 0));

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
EX_FN_NAME (m32r,fmt_24_ldb) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_24_ldb.f
  EXTRACT_FMT_24_LDB_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_24_LDB_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_24_ldb", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

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
EX_FN_NAME (m32r,fmt_25_ldb_d) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_25_ldb_d.f
  EXTRACT_FMT_25_LDB_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_FMT_25_LDB_D_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  FLD (f_simm16) = f_simm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_25_ldb_d", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, "slo16 0x%x", 'x', f_simm16, (char *) 0));

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
EX_FN_NAME (m32r,fmt_26_ldh) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_26_ldh.f
  EXTRACT_FMT_26_LDH_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_26_LDH_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_26_ldh", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

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
EX_FN_NAME (m32r,fmt_27_ldh_d) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_27_ldh_d.f
  EXTRACT_FMT_27_LDH_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_FMT_27_LDH_D_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  FLD (f_simm16) = f_simm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_27_ldh_d", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, "slo16 0x%x", 'x', f_simm16, (char *) 0));

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
EX_FN_NAME (m32r,fmt_28_ld_plus) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_28_ld_plus.f
  EXTRACT_FMT_28_LD_PLUS_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_28_LD_PLUS_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_28_ld_plus", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

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
EX_FN_NAME (m32r,fmt_29_ld24) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_29_ld24.f
  EXTRACT_FMT_29_LD24_VARS /* f-op1 f-r1 f-uimm24 */

  EXTRACT_FMT_29_LD24_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_uimm24) = f_uimm24;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_29_ld24", "dr 0x%x", 'x', f_r1, "uimm24 0x%x", 'x', f_uimm24, (char *) 0));

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
EX_FN_NAME (m32r,fmt_30_ldi8) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_30_ldi8.f
  EXTRACT_FMT_30_LDI8_VARS /* f-op1 f-r1 f-simm8 */

  EXTRACT_FMT_30_LDI8_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_simm8) = f_simm8;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_30_ldi8", "dr 0x%x", 'x', f_r1, "simm8 0x%x", 'x', f_simm8, (char *) 0));

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
EX_FN_NAME (m32r,fmt_31_ldi16) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_31_ldi16.f
  EXTRACT_FMT_31_LDI16_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_FMT_31_LDI16_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_simm16) = f_simm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_31_ldi16", "dr 0x%x", 'x', f_r1, "slo16 0x%x", 'x', f_simm16, (char *) 0));

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
EX_FN_NAME (m32r,fmt_32_lock) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_32_lock.f
  EXTRACT_FMT_32_LOCK_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_32_LOCK_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_32_lock", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

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
EX_FN_NAME (m32r,fmt_33_machi) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_33_machi.f
  EXTRACT_FMT_33_MACHI_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_33_MACHI_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_33_machi", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

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
EX_FN_NAME (m32r,fmt_34_mulhi) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_34_mulhi.f
  EXTRACT_FMT_34_MULHI_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_34_MULHI_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_34_mulhi", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

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
EX_FN_NAME (m32r,fmt_35_mv) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_35_mv.f
  EXTRACT_FMT_35_MV_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_35_MV_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_35_mv", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

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
EX_FN_NAME (m32r,fmt_36_mvfachi) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_36_mvfachi.f
  EXTRACT_FMT_36_MVFACHI_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_36_MVFACHI_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_36_mvfachi", "dr 0x%x", 'x', f_r1, (char *) 0));

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
EX_FN_NAME (m32r,fmt_37_mvfc) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_37_mvfc.f
  EXTRACT_FMT_37_MVFC_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_37_MVFC_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = f_r2;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_37_mvfc", "dr 0x%x", 'x', f_r1, "scr 0x%x", 'x', f_r2, (char *) 0));

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
EX_FN_NAME (m32r,fmt_38_mvtachi) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_38_mvtachi.f
  EXTRACT_FMT_38_MVTACHI_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_38_MVTACHI_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_38_mvtachi", "src1 0x%x", 'x', f_r1, (char *) 0));

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
EX_FN_NAME (m32r,fmt_39_mvtc) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_39_mvtc.f
  EXTRACT_FMT_39_MVTC_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_39_MVTC_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = f_r1;
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_39_mvtc", "dcr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

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
EX_FN_NAME (m32r,fmt_40_nop) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_40_nop.f
  EXTRACT_FMT_40_NOP_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_40_NOP_CODE

  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_40_nop", (char *) 0));

  abuf->length = length;
  abuf->addr = pc;
#undef FLD
}

void
EX_FN_NAME (m32r,fmt_41_rac) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_41_rac.f
  EXTRACT_FMT_41_RAC_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_41_RAC_CODE

  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_41_rac", (char *) 0));

  abuf->length = length;
  abuf->addr = pc;
#undef FLD
}

void
EX_FN_NAME (m32r,fmt_42_rte) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_42_rte.f
  EXTRACT_FMT_42_RTE_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_42_RTE_CODE

  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_42_rte", (char *) 0));

  abuf->length = length;
  abuf->addr = pc;
#undef FLD
}

void
EX_FN_NAME (m32r,fmt_43_seth) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_43_seth.f
  EXTRACT_FMT_43_SETH_VARS /* f-op1 f-r1 f-op2 f-r2 f-hi16 */

  EXTRACT_FMT_43_SETH_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_hi16) = f_hi16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_43_seth", "dr 0x%x", 'x', f_r1, "hi16 0x%x", 'x', f_hi16, (char *) 0));

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
EX_FN_NAME (m32r,fmt_44_sll3) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_44_sll3.f
  EXTRACT_FMT_44_SLL3_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_FMT_44_SLL3_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  FLD (f_simm16) = f_simm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_44_sll3", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, "simm16 0x%x", 'x', f_simm16, (char *) 0));

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
EX_FN_NAME (m32r,fmt_45_slli) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_45_slli.f
  EXTRACT_FMT_45_SLLI_VARS /* f-op1 f-r1 f-shift-op2 f-uimm5 */

  EXTRACT_FMT_45_SLLI_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_uimm5) = f_uimm5;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_45_slli", "dr 0x%x", 'x', f_r1, "uimm5 0x%x", 'x', f_uimm5, (char *) 0));

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
EX_FN_NAME (m32r,fmt_46_st) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_46_st.f
  EXTRACT_FMT_46_ST_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_46_ST_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_46_st", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

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
EX_FN_NAME (m32r,fmt_47_st_d) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_47_st_d.f
  EXTRACT_FMT_47_ST_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_FMT_47_ST_D_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  FLD (f_simm16) = f_simm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_47_st_d", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, "slo16 0x%x", 'x', f_simm16, (char *) 0));

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
EX_FN_NAME (m32r,fmt_48_stb) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_48_stb.f
  EXTRACT_FMT_48_STB_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_48_STB_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_48_stb", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

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
EX_FN_NAME (m32r,fmt_49_stb_d) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_49_stb_d.f
  EXTRACT_FMT_49_STB_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_FMT_49_STB_D_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  FLD (f_simm16) = f_simm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_49_stb_d", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, "slo16 0x%x", 'x', f_simm16, (char *) 0));

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
EX_FN_NAME (m32r,fmt_50_sth) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_50_sth.f
  EXTRACT_FMT_50_STH_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_50_STH_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_50_sth", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

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
EX_FN_NAME (m32r,fmt_51_sth_d) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_51_sth_d.f
  EXTRACT_FMT_51_STH_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_FMT_51_STH_D_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  FLD (f_simm16) = f_simm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_51_sth_d", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, "slo16 0x%x", 'x', f_simm16, (char *) 0));

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
EX_FN_NAME (m32r,fmt_52_st_plus) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_52_st_plus.f
  EXTRACT_FMT_52_ST_PLUS_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_52_ST_PLUS_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_52_st_plus", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

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
EX_FN_NAME (m32r,fmt_53_trap) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_53_trap.f
  EXTRACT_FMT_53_TRAP_VARS /* f-op1 f-r1 f-op2 f-uimm4 */

  EXTRACT_FMT_53_TRAP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_uimm4) = f_uimm4;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_53_trap", "uimm4 0x%x", 'x', f_uimm4, (char *) 0));

  abuf->length = length;
  abuf->addr = pc;
#undef FLD
}

void
EX_FN_NAME (m32r,fmt_54_unlock) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_54_unlock.f
  EXTRACT_FMT_54_UNLOCK_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_54_UNLOCK_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_54_unlock", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

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
