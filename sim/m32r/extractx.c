/* Simulator instruction extractor for m32r.

THIS FILE IS MACHINE GENERATED WITH CGEN.

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
#define WANT_CPU_M32RXF

#include "sim-main.h"
#include "cgen-ops.h"
#include "cpu-sim.h"

void
EX_FN_NAME (m32rxf,fmt_add) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_add.f
  EXTRACT_FMT_ADD_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_ADD_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_add", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_dr) = f_r1;
      FLD (in_sr) = f_r2;
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_add3) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_add3.f
  EXTRACT_FMT_ADD3_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_FMT_ADD3_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  FLD (f_simm16) = f_simm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_add3", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, "slo16 0x%x", 'x', f_simm16, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_sr) = f_r2;
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_and3) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_and3.f
  EXTRACT_FMT_AND3_VARS /* f-op1 f-r1 f-op2 f-r2 f-uimm16 */

  EXTRACT_FMT_AND3_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  FLD (f_uimm16) = f_uimm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_and3", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, "uimm16 0x%x", 'x', f_uimm16, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_sr) = f_r2;
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_or3) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_or3.f
  EXTRACT_FMT_OR3_VARS /* f-op1 f-r1 f-op2 f-r2 f-uimm16 */

  EXTRACT_FMT_OR3_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  FLD (f_uimm16) = f_uimm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_or3", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, "ulo16 0x%x", 'x', f_uimm16, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_sr) = f_r2;
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_addi) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_addi.f
  EXTRACT_FMT_ADDI_VARS /* f-op1 f-r1 f-simm8 */

  EXTRACT_FMT_ADDI_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_simm8) = f_simm8;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_addi", "dr 0x%x", 'x', f_r1, "simm8 0x%x", 'x', f_simm8, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_dr) = f_r1;
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_addv) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_addv.f
  EXTRACT_FMT_ADDV_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_ADDV_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_addv", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_dr) = f_r1;
      FLD (in_sr) = f_r2;
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_addv3) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_addv3.f
  EXTRACT_FMT_ADDV3_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_FMT_ADDV3_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  FLD (f_simm16) = f_simm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_addv3", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, "simm16 0x%x", 'x', f_simm16, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_sr) = f_r2;
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_addx) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_addx.f
  EXTRACT_FMT_ADDX_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_ADDX_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_addx", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_dr) = f_r1;
      FLD (in_sr) = f_r2;
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_bc8) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bc8.f
  EXTRACT_FMT_BC8_VARS /* f-op1 f-r1 f-disp8 */

  EXTRACT_FMT_BC8_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_disp8) = f_disp8;
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_bc8", "disp8 0x%x", 'x', f_disp8, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_bc24) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bc24.f
  EXTRACT_FMT_BC24_VARS /* f-op1 f-r1 f-disp24 */

  EXTRACT_FMT_BC24_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_disp24) = f_disp24;
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_bc24", "disp24 0x%x", 'x', f_disp24, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_beq) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.cti.fields.fmt_beq.f
  EXTRACT_FMT_BEQ_VARS /* f-op1 f-r1 f-op2 f-r2 f-disp16 */

  EXTRACT_FMT_BEQ_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  FLD (f_disp16) = f_disp16;
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_beq", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, "disp16 0x%x", 'x', f_disp16, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src1) = f_r1;
      FLD (in_src2) = f_r2;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_beqz) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.cti.fields.fmt_beqz.f
  EXTRACT_FMT_BEQZ_VARS /* f-op1 f-r1 f-op2 f-r2 f-disp16 */

  EXTRACT_FMT_BEQZ_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  FLD (f_disp16) = f_disp16;
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_beqz", "src2 0x%x", 'x', f_r2, "disp16 0x%x", 'x', f_disp16, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src2) = f_r2;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_bl8) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bl8.f
  EXTRACT_FMT_BL8_VARS /* f-op1 f-r1 f-disp8 */

  EXTRACT_FMT_BL8_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_disp8) = f_disp8;
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_bl8", "disp8 0x%x", 'x', f_disp8, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_h_gr_14) = 14;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_bl24) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bl24.f
  EXTRACT_FMT_BL24_VARS /* f-op1 f-r1 f-disp24 */

  EXTRACT_FMT_BL24_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_disp24) = f_disp24;
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_bl24", "disp24 0x%x", 'x', f_disp24, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_h_gr_14) = 14;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_bcl8) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bcl8.f
  EXTRACT_FMT_BCL8_VARS /* f-op1 f-r1 f-disp8 */

  EXTRACT_FMT_BCL8_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_disp8) = f_disp8;
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_bcl8", "disp8 0x%x", 'x', f_disp8, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_h_gr_14) = 14;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_bcl24) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bcl24.f
  EXTRACT_FMT_BCL24_VARS /* f-op1 f-r1 f-disp24 */

  EXTRACT_FMT_BCL24_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_disp24) = f_disp24;
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_bcl24", "disp24 0x%x", 'x', f_disp24, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_h_gr_14) = 14;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_bra8) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bra8.f
  EXTRACT_FMT_BRA8_VARS /* f-op1 f-r1 f-disp8 */

  EXTRACT_FMT_BRA8_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_disp8) = f_disp8;
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_bra8", "disp8 0x%x", 'x', f_disp8, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_bra24) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.cti.fields.fmt_bra24.f
  EXTRACT_FMT_BRA24_VARS /* f-op1 f-r1 f-disp24 */

  EXTRACT_FMT_BRA24_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_disp24) = f_disp24;
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_bra24", "disp24 0x%x", 'x', f_disp24, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_cmp) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_cmp.f
  EXTRACT_FMT_CMP_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_CMP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_cmp", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src1) = f_r1;
      FLD (in_src2) = f_r2;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_cmpi) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_cmpi.f
  EXTRACT_FMT_CMPI_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_FMT_CMPI_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  FLD (f_simm16) = f_simm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_cmpi", "src2 0x%x", 'x', f_r2, "simm16 0x%x", 'x', f_simm16, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src2) = f_r2;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_cmpz) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_cmpz.f
  EXTRACT_FMT_CMPZ_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_CMPZ_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_cmpz", "src2 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src2) = f_r2;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_div) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_div.f
  EXTRACT_FMT_DIV_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_FMT_DIV_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_div", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_sr) = f_r2;
      FLD (in_dr) = f_r1;
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_jc) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.cti.fields.fmt_jc.f
  EXTRACT_FMT_JC_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_JC_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_jc", "sr 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_sr) = f_r2;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_jl) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.cti.fields.fmt_jl.f
  EXTRACT_FMT_JL_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_JL_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_jl", "sr 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_sr) = f_r2;
      FLD (out_h_gr_14) = 14;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_jmp) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.cti.fields.fmt_jmp.f
  EXTRACT_FMT_JMP_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_JMP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_jmp", "sr 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_sr) = f_r2;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_ld) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_ld.f
  EXTRACT_FMT_LD_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_LD_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_ld", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_sr) = f_r2;
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_ld_d) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_ld_d.f
  EXTRACT_FMT_LD_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_FMT_LD_D_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  FLD (f_simm16) = f_simm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_ld_d", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, "slo16 0x%x", 'x', f_simm16, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_sr) = f_r2;
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_ldb) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_ldb.f
  EXTRACT_FMT_LDB_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_LDB_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_ldb", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_sr) = f_r2;
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_ldb_d) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_ldb_d.f
  EXTRACT_FMT_LDB_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_FMT_LDB_D_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  FLD (f_simm16) = f_simm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_ldb_d", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, "slo16 0x%x", 'x', f_simm16, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_sr) = f_r2;
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_ldh) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_ldh.f
  EXTRACT_FMT_LDH_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_LDH_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_ldh", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_sr) = f_r2;
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_ldh_d) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_ldh_d.f
  EXTRACT_FMT_LDH_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_FMT_LDH_D_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  FLD (f_simm16) = f_simm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_ldh_d", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, "slo16 0x%x", 'x', f_simm16, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_sr) = f_r2;
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_ld_plus) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_ld_plus.f
  EXTRACT_FMT_LD_PLUS_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_LD_PLUS_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_ld_plus", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_sr) = f_r2;
      FLD (out_dr) = f_r1;
      FLD (out_sr) = f_r2;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_ld24) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_ld24.f
  EXTRACT_FMT_LD24_VARS /* f-op1 f-r1 f-uimm24 */

  EXTRACT_FMT_LD24_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_uimm24) = f_uimm24;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_ld24", "dr 0x%x", 'x', f_r1, "uimm24 0x%x", 'x', f_uimm24, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_ldi8) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_ldi8.f
  EXTRACT_FMT_LDI8_VARS /* f-op1 f-r1 f-simm8 */

  EXTRACT_FMT_LDI8_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_simm8) = f_simm8;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_ldi8", "dr 0x%x", 'x', f_r1, "simm8 0x%x", 'x', f_simm8, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_ldi16) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_ldi16.f
  EXTRACT_FMT_LDI16_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_FMT_LDI16_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_simm16) = f_simm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_ldi16", "dr 0x%x", 'x', f_r1, "slo16 0x%x", 'x', f_simm16, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_lock) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_lock.f
  EXTRACT_FMT_LOCK_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_LOCK_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_lock", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_sr) = f_r2;
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_machi_a) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_machi_a.f
  EXTRACT_FMT_MACHI_A_VARS /* f-op1 f-r1 f-acc f-op23 f-r2 */

  EXTRACT_FMT_MACHI_A_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_acc) = f_acc;
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_machi_a", "src1 0x%x", 'x', f_r1, "acc 0x%x", 'x', f_acc, "src2 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src1) = f_r1;
      FLD (in_src2) = f_r2;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_mulhi_a) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_mulhi_a.f
  EXTRACT_FMT_MULHI_A_VARS /* f-op1 f-r1 f-acc f-op23 f-r2 */

  EXTRACT_FMT_MULHI_A_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_acc) = f_acc;
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_mulhi_a", "src1 0x%x", 'x', f_r1, "acc 0x%x", 'x', f_acc, "src2 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src1) = f_r1;
      FLD (in_src2) = f_r2;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_mv) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_mv.f
  EXTRACT_FMT_MV_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_MV_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_mv", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_sr) = f_r2;
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_mvfachi_a) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_mvfachi_a.f
  EXTRACT_FMT_MVFACHI_A_VARS /* f-op1 f-r1 f-op2 f-accs f-op3 */

  EXTRACT_FMT_MVFACHI_A_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_accs) = f_accs;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_mvfachi_a", "dr 0x%x", 'x', f_r1, "accs 0x%x", 'x', f_accs, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_mvfc) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_mvfc.f
  EXTRACT_FMT_MVFC_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_MVFC_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = f_r2;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_mvfc", "dr 0x%x", 'x', f_r1, "scr 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_mvtachi_a) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_mvtachi_a.f
  EXTRACT_FMT_MVTACHI_A_VARS /* f-op1 f-r1 f-op2 f-accs f-op3 */

  EXTRACT_FMT_MVTACHI_A_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_accs) = f_accs;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_mvtachi_a", "src1 0x%x", 'x', f_r1, "accs 0x%x", 'x', f_accs, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src1) = f_r1;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_mvtc) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_mvtc.f
  EXTRACT_FMT_MVTC_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_MVTC_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = f_r1;
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_mvtc", "dcr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_sr) = f_r2;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_nop) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_nop.f
  EXTRACT_FMT_NOP_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_NOP_CODE

  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_nop", (char *) 0));

#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_rac_dsi) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_rac_dsi.f
  EXTRACT_FMT_RAC_DSI_VARS /* f-op1 f-accd f-bits67 f-op2 f-accs f-bit14 f-imm1 */

  EXTRACT_FMT_RAC_DSI_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_accd) = f_accd;
  FLD (f_accs) = f_accs;
  FLD (f_imm1) = f_imm1;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_rac_dsi", "accd 0x%x", 'x', f_accd, "accs 0x%x", 'x', f_accs, "imm1 0x%x", 'x', f_imm1, (char *) 0));

#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_rte) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.cti.fields.fmt_rte.f
  EXTRACT_FMT_RTE_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_RTE_CODE

  /* Record the fields for the semantic handler.  */
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_rte", (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_seth) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_seth.f
  EXTRACT_FMT_SETH_VARS /* f-op1 f-r1 f-op2 f-r2 f-hi16 */

  EXTRACT_FMT_SETH_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_hi16) = f_hi16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_seth", "dr 0x%x", 'x', f_r1, "hi16 0x%x", 'x', f_hi16, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_sll3) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_sll3.f
  EXTRACT_FMT_SLL3_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_FMT_SLL3_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  FLD (f_simm16) = f_simm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_sll3", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, "simm16 0x%x", 'x', f_simm16, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_sr) = f_r2;
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_slli) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_slli.f
  EXTRACT_FMT_SLLI_VARS /* f-op1 f-r1 f-shift-op2 f-uimm5 */

  EXTRACT_FMT_SLLI_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_uimm5) = f_uimm5;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_slli", "dr 0x%x", 'x', f_r1, "uimm5 0x%x", 'x', f_uimm5, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_dr) = f_r1;
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_st) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_st.f
  EXTRACT_FMT_ST_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_ST_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_st", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src2) = f_r2;
      FLD (in_src1) = f_r1;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_st_d) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_st_d.f
  EXTRACT_FMT_ST_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_FMT_ST_D_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  FLD (f_simm16) = f_simm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_st_d", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, "slo16 0x%x", 'x', f_simm16, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src2) = f_r2;
      FLD (in_src1) = f_r1;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_stb) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_stb.f
  EXTRACT_FMT_STB_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_STB_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_stb", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src2) = f_r2;
      FLD (in_src1) = f_r1;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_stb_d) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_stb_d.f
  EXTRACT_FMT_STB_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_FMT_STB_D_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  FLD (f_simm16) = f_simm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_stb_d", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, "slo16 0x%x", 'x', f_simm16, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src2) = f_r2;
      FLD (in_src1) = f_r1;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_sth) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_sth.f
  EXTRACT_FMT_STH_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_STH_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_sth", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src2) = f_r2;
      FLD (in_src1) = f_r1;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_sth_d) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_sth_d.f
  EXTRACT_FMT_STH_D_VARS /* f-op1 f-r1 f-op2 f-r2 f-simm16 */

  EXTRACT_FMT_STH_D_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  FLD (f_simm16) = f_simm16;
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_sth_d", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, "slo16 0x%x", 'x', f_simm16, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src2) = f_r2;
      FLD (in_src1) = f_r1;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_st_plus) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_st_plus.f
  EXTRACT_FMT_ST_PLUS_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_ST_PLUS_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_st_plus", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src2) = f_r2;
      FLD (in_src1) = f_r1;
      FLD (out_src2) = f_r2;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_trap) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.cti.fields.fmt_trap.f
  EXTRACT_FMT_TRAP_VARS /* f-op1 f-r1 f-op2 f-uimm4 */

  EXTRACT_FMT_TRAP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_uimm4) = f_uimm4;
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_trap", "uimm4 0x%x", 'x', f_uimm4, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_unlock) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_unlock.f
  EXTRACT_FMT_UNLOCK_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_UNLOCK_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_unlock", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src2) = f_r2;
      FLD (in_src1) = f_r1;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_satb) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_satb.f
  EXTRACT_FMT_SATB_VARS /* f-op1 f-r1 f-op2 f-r2 f-uimm16 */

  EXTRACT_FMT_SATB_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_satb", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_sr) = f_r2;
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_sat) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_sat.f
  EXTRACT_FMT_SAT_VARS /* f-op1 f-r1 f-op2 f-r2 f-uimm16 */

  EXTRACT_FMT_SAT_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_sat", "dr 0x%x", 'x', f_r1, "sr 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_sr) = f_r2;
      FLD (out_dr) = f_r1;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_sadd) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_sadd.f
  EXTRACT_FMT_SADD_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_SADD_CODE

  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_sadd", (char *) 0));

#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_macwu1) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_macwu1.f
  EXTRACT_FMT_MACWU1_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_MACWU1_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_macwu1", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src1) = f_r1;
      FLD (in_src2) = f_r2;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_msblo) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_msblo.f
  EXTRACT_FMT_MSBLO_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_MSBLO_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_msblo", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src1) = f_r1;
      FLD (in_src2) = f_r2;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_mulwu1) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_mulwu1.f
  EXTRACT_FMT_MULWU1_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_MULWU1_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_r1) = & CPU (h_gr)[f_r1];
  FLD (f_r2) = & CPU (h_gr)[f_r2];
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_mulwu1", "src1 0x%x", 'x', f_r1, "src2 0x%x", 'x', f_r2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src1) = f_r1;
      FLD (in_src2) = f_r2;
    }
#endif
#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_sc) (SIM_CPU *current_cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
#define FLD(f) abuf->fields.fmt_sc.f
  EXTRACT_FMT_SC_VARS /* f-op1 f-r1 f-op2 f-r2 */

  EXTRACT_FMT_SC_CODE

  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, (current_cpu, pc, "fmt_sc", (char *) 0));

#undef FLD
}

void
EX_FN_NAME (m32rxf,fmt_empty) (SIM_CPU *cpu, PCADDR pc, insn_t insn, ARGBUF *abuf)
{
  /* Empty format handler for virtual insns.  */
}
