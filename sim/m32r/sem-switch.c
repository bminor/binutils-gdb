/* Simulator instruction semantics for m32rb.

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

#ifdef DEFINE_LABELS
#undef DEFINE_LABELS

  /* The labels have the case they have because the enum of insn types
     is all uppercase and in the non-stdc case the insn symbol is built
     into the enum name.  */

  static struct {
    int index;
    void *label;
  } labels[] = {
    { M32RB_XINSN_ILLEGAL, && case_sem_INSN_ILLEGAL },
    { M32RB_XINSN_ADD, && case_sem_INSN_ADD },
    { M32RB_XINSN_ADD3, && case_sem_INSN_ADD3 },
    { M32RB_XINSN_AND, && case_sem_INSN_AND },
    { M32RB_XINSN_AND3, && case_sem_INSN_AND3 },
    { M32RB_XINSN_OR, && case_sem_INSN_OR },
    { M32RB_XINSN_OR3, && case_sem_INSN_OR3 },
    { M32RB_XINSN_XOR, && case_sem_INSN_XOR },
    { M32RB_XINSN_XOR3, && case_sem_INSN_XOR3 },
    { M32RB_XINSN_ADDI, && case_sem_INSN_ADDI },
    { M32RB_XINSN_ADDV, && case_sem_INSN_ADDV },
    { M32RB_XINSN_ADDV3, && case_sem_INSN_ADDV3 },
    { M32RB_XINSN_ADDX, && case_sem_INSN_ADDX },
    { M32RB_XINSN_BC8, && case_sem_INSN_BC8 },
    { M32RB_XINSN_BC24, && case_sem_INSN_BC24 },
    { M32RB_XINSN_BEQ, && case_sem_INSN_BEQ },
    { M32RB_XINSN_BEQZ, && case_sem_INSN_BEQZ },
    { M32RB_XINSN_BGEZ, && case_sem_INSN_BGEZ },
    { M32RB_XINSN_BGTZ, && case_sem_INSN_BGTZ },
    { M32RB_XINSN_BLEZ, && case_sem_INSN_BLEZ },
    { M32RB_XINSN_BLTZ, && case_sem_INSN_BLTZ },
    { M32RB_XINSN_BNEZ, && case_sem_INSN_BNEZ },
    { M32RB_XINSN_BL8, && case_sem_INSN_BL8 },
    { M32RB_XINSN_BL24, && case_sem_INSN_BL24 },
    { M32RB_XINSN_BNC8, && case_sem_INSN_BNC8 },
    { M32RB_XINSN_BNC24, && case_sem_INSN_BNC24 },
    { M32RB_XINSN_BNE, && case_sem_INSN_BNE },
    { M32RB_XINSN_BRA8, && case_sem_INSN_BRA8 },
    { M32RB_XINSN_BRA24, && case_sem_INSN_BRA24 },
    { M32RB_XINSN_CMP, && case_sem_INSN_CMP },
    { M32RB_XINSN_CMPI, && case_sem_INSN_CMPI },
    { M32RB_XINSN_CMPU, && case_sem_INSN_CMPU },
    { M32RB_XINSN_CMPUI, && case_sem_INSN_CMPUI },
    { M32RB_XINSN_DIV, && case_sem_INSN_DIV },
    { M32RB_XINSN_DIVU, && case_sem_INSN_DIVU },
    { M32RB_XINSN_REM, && case_sem_INSN_REM },
    { M32RB_XINSN_REMU, && case_sem_INSN_REMU },
    { M32RB_XINSN_JL, && case_sem_INSN_JL },
    { M32RB_XINSN_JMP, && case_sem_INSN_JMP },
    { M32RB_XINSN_LD, && case_sem_INSN_LD },
    { M32RB_XINSN_LD_D, && case_sem_INSN_LD_D },
    { M32RB_XINSN_LDB, && case_sem_INSN_LDB },
    { M32RB_XINSN_LDB_D, && case_sem_INSN_LDB_D },
    { M32RB_XINSN_LDH, && case_sem_INSN_LDH },
    { M32RB_XINSN_LDH_D, && case_sem_INSN_LDH_D },
    { M32RB_XINSN_LDUB, && case_sem_INSN_LDUB },
    { M32RB_XINSN_LDUB_D, && case_sem_INSN_LDUB_D },
    { M32RB_XINSN_LDUH, && case_sem_INSN_LDUH },
    { M32RB_XINSN_LDUH_D, && case_sem_INSN_LDUH_D },
    { M32RB_XINSN_LD_PLUS, && case_sem_INSN_LD_PLUS },
    { M32RB_XINSN_LD24, && case_sem_INSN_LD24 },
    { M32RB_XINSN_LDI8, && case_sem_INSN_LDI8 },
    { M32RB_XINSN_LDI16, && case_sem_INSN_LDI16 },
    { M32RB_XINSN_LOCK, && case_sem_INSN_LOCK },
    { M32RB_XINSN_MACHI, && case_sem_INSN_MACHI },
    { M32RB_XINSN_MACLO, && case_sem_INSN_MACLO },
    { M32RB_XINSN_MACWHI, && case_sem_INSN_MACWHI },
    { M32RB_XINSN_MACWLO, && case_sem_INSN_MACWLO },
    { M32RB_XINSN_MUL, && case_sem_INSN_MUL },
    { M32RB_XINSN_MULHI, && case_sem_INSN_MULHI },
    { M32RB_XINSN_MULLO, && case_sem_INSN_MULLO },
    { M32RB_XINSN_MULWHI, && case_sem_INSN_MULWHI },
    { M32RB_XINSN_MULWLO, && case_sem_INSN_MULWLO },
    { M32RB_XINSN_MV, && case_sem_INSN_MV },
    { M32RB_XINSN_MVFACHI, && case_sem_INSN_MVFACHI },
    { M32RB_XINSN_MVFACLO, && case_sem_INSN_MVFACLO },
    { M32RB_XINSN_MVFACMI, && case_sem_INSN_MVFACMI },
    { M32RB_XINSN_MVFC, && case_sem_INSN_MVFC },
    { M32RB_XINSN_MVTACHI, && case_sem_INSN_MVTACHI },
    { M32RB_XINSN_MVTACLO, && case_sem_INSN_MVTACLO },
    { M32RB_XINSN_MVTC, && case_sem_INSN_MVTC },
    { M32RB_XINSN_NEG, && case_sem_INSN_NEG },
    { M32RB_XINSN_NOP, && case_sem_INSN_NOP },
    { M32RB_XINSN_NOT, && case_sem_INSN_NOT },
    { M32RB_XINSN_RAC, && case_sem_INSN_RAC },
    { M32RB_XINSN_RACH, && case_sem_INSN_RACH },
    { M32RB_XINSN_RTE, && case_sem_INSN_RTE },
    { M32RB_XINSN_SETH, && case_sem_INSN_SETH },
    { M32RB_XINSN_SLL, && case_sem_INSN_SLL },
    { M32RB_XINSN_SLL3, && case_sem_INSN_SLL3 },
    { M32RB_XINSN_SLLI, && case_sem_INSN_SLLI },
    { M32RB_XINSN_SRA, && case_sem_INSN_SRA },
    { M32RB_XINSN_SRA3, && case_sem_INSN_SRA3 },
    { M32RB_XINSN_SRAI, && case_sem_INSN_SRAI },
    { M32RB_XINSN_SRL, && case_sem_INSN_SRL },
    { M32RB_XINSN_SRL3, && case_sem_INSN_SRL3 },
    { M32RB_XINSN_SRLI, && case_sem_INSN_SRLI },
    { M32RB_XINSN_ST, && case_sem_INSN_ST },
    { M32RB_XINSN_ST_D, && case_sem_INSN_ST_D },
    { M32RB_XINSN_STB, && case_sem_INSN_STB },
    { M32RB_XINSN_STB_D, && case_sem_INSN_STB_D },
    { M32RB_XINSN_STH, && case_sem_INSN_STH },
    { M32RB_XINSN_STH_D, && case_sem_INSN_STH_D },
    { M32RB_XINSN_ST_PLUS, && case_sem_INSN_ST_PLUS },
    { M32RB_XINSN_ST_MINUS, && case_sem_INSN_ST_MINUS },
    { M32RB_XINSN_SUB, && case_sem_INSN_SUB },
    { M32RB_XINSN_SUBV, && case_sem_INSN_SUBV },
    { M32RB_XINSN_SUBX, && case_sem_INSN_SUBX },
    { M32RB_XINSN_TRAP, && case_sem_INSN_TRAP },
    { M32RB_XINSN_UNLOCK, && case_sem_INSN_UNLOCK },
    { 0, 0 }
  };
  int i;

  for (i = 0; labels[i].label != 0; ++i)
    CPU_IDESC (current_cpu) [labels[i].index].sem_fast_lab = labels[i].label;

#endif /* DEFINE_LABELS */

#ifdef DEFINE_SWITCH
#undef DEFINE_SWITCH

/* If hyper-fast [well not unnecessarily slow] execution is selected, turn
   off frills like tracing and profiling.  */
/* FIXME: A better way would be to have TRACE_RESULT check for something
   that can cause it to be optimized out.  */

#if FAST_P
#undef TRACE_RESULT
#define TRACE_RESULT(cpu, name, type, val)
#endif

#undef GET_ATTR
#define GET_ATTR(cpu, num, attr) CGEN_INSN_ATTR (abuf->idesc->opcode, CGEN_INSN_##attr)

{
  SEM_ARG sem_arg = sc;
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  CIA new_pc;

  SWITCH (sem, sem_arg->semantic.sem_case)
    {

  CASE (sem, INSN_ILLEGAL) :
{
  sim_engine_halt (CPU_STATE (current_cpu), current_cpu, NULL, NULL_CIA/*FIXME*/,
		   sim_stopped, SIM_SIGILL);
  BREAK (sem);
}

  CASE (sem, INSN_ADD) : /* add $dr,$sr */
{
#define FLD(f) abuf->fields.fmt_add.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = ADDSI (* FLD (f_r1), * FLD (f_r2));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_ADD3) : /* add3 $dr,$sr,$hash$slo16 */
{
#define FLD(f) abuf->fields.fmt_add3.f
  new_pc = SEM_NEXT_PC (sem_arg, 4);

  {
    SI opval = ADDSI (* FLD (f_r2), FLD (f_simm16));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_AND) : /* and $dr,$sr */
{
#define FLD(f) abuf->fields.fmt_add.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = ANDSI (* FLD (f_r1), * FLD (f_r2));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_AND3) : /* and3 $dr,$sr,$uimm16 */
{
#define FLD(f) abuf->fields.fmt_and3.f
  new_pc = SEM_NEXT_PC (sem_arg, 4);

  {
    SI opval = ANDSI (* FLD (f_r2), FLD (f_uimm16));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_OR) : /* or $dr,$sr */
{
#define FLD(f) abuf->fields.fmt_add.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = ORSI (* FLD (f_r1), * FLD (f_r2));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_OR3) : /* or3 $dr,$sr,$hash$ulo16 */
{
#define FLD(f) abuf->fields.fmt_or3.f
  new_pc = SEM_NEXT_PC (sem_arg, 4);

  {
    SI opval = ORSI (* FLD (f_r2), FLD (f_uimm16));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_XOR) : /* xor $dr,$sr */
{
#define FLD(f) abuf->fields.fmt_add.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = XORSI (* FLD (f_r1), * FLD (f_r2));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_XOR3) : /* xor3 $dr,$sr,$uimm16 */
{
#define FLD(f) abuf->fields.fmt_and3.f
  new_pc = SEM_NEXT_PC (sem_arg, 4);

  {
    SI opval = XORSI (* FLD (f_r2), FLD (f_uimm16));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_ADDI) : /* addi $dr,$simm8 */
{
#define FLD(f) abuf->fields.fmt_addi.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = ADDSI (* FLD (f_r1), FLD (f_simm8));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_ADDV) : /* addv $dr,$sr */
{
#define FLD(f) abuf->fields.fmt_addv.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

do {
  UBI temp1;SI temp0;
  temp0 = ADDSI (* FLD (f_r1), * FLD (f_r2));
  temp1 = ADDOFSI (* FLD (f_r1), * FLD (f_r2), 0);
  {
    SI opval = temp0;
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }
  {
    UBI opval = temp1;
    CPU (h_cond) = opval;
    TRACE_RESULT (current_cpu, "condbit", 'x', opval);
  }
} while (0);

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_ADDV3) : /* addv3 $dr,$sr,$simm16 */
{
#define FLD(f) abuf->fields.fmt_addv3.f
  new_pc = SEM_NEXT_PC (sem_arg, 4);

do {
  UBI temp1;SI temp0;
  temp0 = ADDSI (* FLD (f_r2), FLD (f_simm16));
  temp1 = ADDOFSI (* FLD (f_r2), FLD (f_simm16), 0);
  {
    SI opval = temp0;
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }
  {
    UBI opval = temp1;
    CPU (h_cond) = opval;
    TRACE_RESULT (current_cpu, "condbit", 'x', opval);
  }
} while (0);

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_ADDX) : /* addx $dr,$sr */
{
#define FLD(f) abuf->fields.fmt_addx.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

do {
  UBI temp1;SI temp0;
  temp0 = ADDCSI (* FLD (f_r1), * FLD (f_r2), CPU (h_cond));
  temp1 = ADDCFSI (* FLD (f_r1), * FLD (f_r2), CPU (h_cond));
  {
    SI opval = temp0;
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }
  {
    UBI opval = temp1;
    CPU (h_cond) = opval;
    TRACE_RESULT (current_cpu, "condbit", 'x', opval);
  }
} while (0);

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_BC8) : /* bc.s $disp8 */
{
#define FLD(f) abuf->fields.fmt_bc8.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

if (CPU (h_cond)) {
  {
    USI opval = FLD (f_disp8);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, opval));
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
}

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_BC24) : /* bc.l $disp24 */
{
#define FLD(f) abuf->fields.fmt_bc24.f
  new_pc = SEM_NEXT_PC (sem_arg, 4);

if (CPU (h_cond)) {
  {
    USI opval = FLD (f_disp24);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, opval));
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
}

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_BEQ) : /* beq $src1,$src2,$disp16 */
{
#define FLD(f) abuf->fields.fmt_beq.f
  new_pc = SEM_NEXT_PC (sem_arg, 4);

if (EQSI (* FLD (f_r1), * FLD (f_r2))) {
  {
    USI opval = FLD (f_disp16);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, opval));
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
}

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_BEQZ) : /* beqz $src2,$disp16 */
{
#define FLD(f) abuf->fields.fmt_beqz.f
  new_pc = SEM_NEXT_PC (sem_arg, 4);

if (EQSI (* FLD (f_r2), 0)) {
  {
    USI opval = FLD (f_disp16);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, opval));
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
}

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_BGEZ) : /* bgez $src2,$disp16 */
{
#define FLD(f) abuf->fields.fmt_beqz.f
  new_pc = SEM_NEXT_PC (sem_arg, 4);

if (GESI (* FLD (f_r2), 0)) {
  {
    USI opval = FLD (f_disp16);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, opval));
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
}

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_BGTZ) : /* bgtz $src2,$disp16 */
{
#define FLD(f) abuf->fields.fmt_beqz.f
  new_pc = SEM_NEXT_PC (sem_arg, 4);

if (GTSI (* FLD (f_r2), 0)) {
  {
    USI opval = FLD (f_disp16);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, opval));
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
}

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_BLEZ) : /* blez $src2,$disp16 */
{
#define FLD(f) abuf->fields.fmt_beqz.f
  new_pc = SEM_NEXT_PC (sem_arg, 4);

if (LESI (* FLD (f_r2), 0)) {
  {
    USI opval = FLD (f_disp16);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, opval));
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
}

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_BLTZ) : /* bltz $src2,$disp16 */
{
#define FLD(f) abuf->fields.fmt_beqz.f
  new_pc = SEM_NEXT_PC (sem_arg, 4);

if (LTSI (* FLD (f_r2), 0)) {
  {
    USI opval = FLD (f_disp16);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, opval));
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
}

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_BNEZ) : /* bnez $src2,$disp16 */
{
#define FLD(f) abuf->fields.fmt_beqz.f
  new_pc = SEM_NEXT_PC (sem_arg, 4);

if (NESI (* FLD (f_r2), 0)) {
  {
    USI opval = FLD (f_disp16);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, opval));
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
}

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_BL8) : /* bl.s $disp8 */
{
#define FLD(f) abuf->fields.fmt_bl8.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

do {
  {
    SI opval = ADDSI (ANDSI (CPU (h_pc), -4), 4);
    CPU (h_gr[((HOSTUINT) 14)]) = opval;
    TRACE_RESULT (current_cpu, "gr-14", 'x', opval);
  }
  {
    USI opval = FLD (f_disp8);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, opval));
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
} while (0);

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_BL24) : /* bl.l $disp24 */
{
#define FLD(f) abuf->fields.fmt_bl24.f
  new_pc = SEM_NEXT_PC (sem_arg, 4);

do {
  {
    SI opval = ADDSI (CPU (h_pc), 4);
    CPU (h_gr[((HOSTUINT) 14)]) = opval;
    TRACE_RESULT (current_cpu, "gr-14", 'x', opval);
  }
  {
    USI opval = FLD (f_disp24);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, opval));
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
} while (0);

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_BNC8) : /* bnc.s $disp8 */
{
#define FLD(f) abuf->fields.fmt_bc8.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

if (NOTBI (CPU (h_cond))) {
  {
    USI opval = FLD (f_disp8);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, opval));
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
}

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_BNC24) : /* bnc.l $disp24 */
{
#define FLD(f) abuf->fields.fmt_bc24.f
  new_pc = SEM_NEXT_PC (sem_arg, 4);

if (NOTBI (CPU (h_cond))) {
  {
    USI opval = FLD (f_disp24);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, opval));
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
}

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_BNE) : /* bne $src1,$src2,$disp16 */
{
#define FLD(f) abuf->fields.fmt_beq.f
  new_pc = SEM_NEXT_PC (sem_arg, 4);

if (NESI (* FLD (f_r1), * FLD (f_r2))) {
  {
    USI opval = FLD (f_disp16);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, opval));
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
}

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_BRA8) : /* bra.s $disp8 */
{
#define FLD(f) abuf->fields.fmt_bra8.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    USI opval = FLD (f_disp8);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, opval));
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_BRA24) : /* bra.l $disp24 */
{
#define FLD(f) abuf->fields.fmt_bra24.f
  new_pc = SEM_NEXT_PC (sem_arg, 4);

  {
    USI opval = FLD (f_disp24);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_CACHE (sem_arg, opval));
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_CMP) : /* cmp $src1,$src2 */
{
#define FLD(f) abuf->fields.fmt_cmp.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    UBI opval = LTSI (* FLD (f_r1), * FLD (f_r2));
    CPU (h_cond) = opval;
    TRACE_RESULT (current_cpu, "condbit", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_CMPI) : /* cmpi $src2,$simm16 */
{
#define FLD(f) abuf->fields.fmt_cmpi.f
  new_pc = SEM_NEXT_PC (sem_arg, 4);

  {
    UBI opval = LTSI (* FLD (f_r2), FLD (f_simm16));
    CPU (h_cond) = opval;
    TRACE_RESULT (current_cpu, "condbit", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_CMPU) : /* cmpu $src1,$src2 */
{
#define FLD(f) abuf->fields.fmt_cmp.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    UBI opval = LTUSI (* FLD (f_r1), * FLD (f_r2));
    CPU (h_cond) = opval;
    TRACE_RESULT (current_cpu, "condbit", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_CMPUI) : /* cmpui $src2,$simm16 */
{
#define FLD(f) abuf->fields.fmt_cmpi.f
  new_pc = SEM_NEXT_PC (sem_arg, 4);

  {
    UBI opval = LTUSI (* FLD (f_r2), FLD (f_simm16));
    CPU (h_cond) = opval;
    TRACE_RESULT (current_cpu, "condbit", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_DIV) : /* div $dr,$sr */
{
#define FLD(f) abuf->fields.fmt_div.f
  new_pc = SEM_NEXT_PC (sem_arg, 4);

if (NESI (* FLD (f_r2), 0)) {
  {
    SI opval = DIVSI (* FLD (f_r1), * FLD (f_r2));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }
}

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_DIVU) : /* divu $dr,$sr */
{
#define FLD(f) abuf->fields.fmt_div.f
  new_pc = SEM_NEXT_PC (sem_arg, 4);

if (NESI (* FLD (f_r2), 0)) {
  {
    SI opval = UDIVSI (* FLD (f_r1), * FLD (f_r2));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }
}

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_REM) : /* rem $dr,$sr */
{
#define FLD(f) abuf->fields.fmt_div.f
  new_pc = SEM_NEXT_PC (sem_arg, 4);

if (NESI (* FLD (f_r2), 0)) {
  {
    SI opval = MODSI (* FLD (f_r1), * FLD (f_r2));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }
}

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_REMU) : /* remu $dr,$sr */
{
#define FLD(f) abuf->fields.fmt_div.f
  new_pc = SEM_NEXT_PC (sem_arg, 4);

if (NESI (* FLD (f_r2), 0)) {
  {
    SI opval = UMODSI (* FLD (f_r1), * FLD (f_r2));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }
}

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_JL) : /* jl $sr */
{
#define FLD(f) abuf->fields.fmt_jl.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

do {
  USI temp1;SI temp0;
  temp0 = ADDSI (ANDSI (CPU (h_pc), -4), 4);
  temp1 = ANDSI (* FLD (f_r2), -4);
  {
    SI opval = temp0;
    CPU (h_gr[((HOSTUINT) 14)]) = opval;
    TRACE_RESULT (current_cpu, "gr-14", 'x', opval);
  }
  {
    USI opval = temp1;
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_ADDR (sem_arg, opval));
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
} while (0);

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_JMP) : /* jmp $sr */
{
#define FLD(f) abuf->fields.fmt_jmp.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    USI opval = ANDSI (* FLD (f_r2), -4);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_ADDR (sem_arg, opval));
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_LD) : /* ld $dr,@$sr */
{
#define FLD(f) abuf->fields.fmt_ld.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = GETMEMSI (current_cpu, * FLD (f_r2));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_LD_D) : /* ld $dr,@($slo16,$sr) */
{
#define FLD(f) abuf->fields.fmt_ld_d.f
  new_pc = SEM_NEXT_PC (sem_arg, 4);

  {
    SI opval = GETMEMSI (current_cpu, ADDSI (* FLD (f_r2), FLD (f_simm16)));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_LDB) : /* ldb $dr,@$sr */
{
#define FLD(f) abuf->fields.fmt_ldb.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = EXTQISI (GETMEMQI (current_cpu, * FLD (f_r2)));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_LDB_D) : /* ldb $dr,@($slo16,$sr) */
{
#define FLD(f) abuf->fields.fmt_ldb_d.f
  new_pc = SEM_NEXT_PC (sem_arg, 4);

  {
    SI opval = EXTQISI (GETMEMQI (current_cpu, ADDSI (* FLD (f_r2), FLD (f_simm16))));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_LDH) : /* ldh $dr,@$sr */
{
#define FLD(f) abuf->fields.fmt_ldh.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = EXTHISI (GETMEMHI (current_cpu, * FLD (f_r2)));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_LDH_D) : /* ldh $dr,@($slo16,$sr) */
{
#define FLD(f) abuf->fields.fmt_ldh_d.f
  new_pc = SEM_NEXT_PC (sem_arg, 4);

  {
    SI opval = EXTHISI (GETMEMHI (current_cpu, ADDSI (* FLD (f_r2), FLD (f_simm16))));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_LDUB) : /* ldub $dr,@$sr */
{
#define FLD(f) abuf->fields.fmt_ldb.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = ZEXTQISI (GETMEMQI (current_cpu, * FLD (f_r2)));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_LDUB_D) : /* ldub $dr,@($slo16,$sr) */
{
#define FLD(f) abuf->fields.fmt_ldb_d.f
  new_pc = SEM_NEXT_PC (sem_arg, 4);

  {
    SI opval = ZEXTQISI (GETMEMQI (current_cpu, ADDSI (* FLD (f_r2), FLD (f_simm16))));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_LDUH) : /* lduh $dr,@$sr */
{
#define FLD(f) abuf->fields.fmt_ldh.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = ZEXTHISI (GETMEMHI (current_cpu, * FLD (f_r2)));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_LDUH_D) : /* lduh $dr,@($slo16,$sr) */
{
#define FLD(f) abuf->fields.fmt_ldh_d.f
  new_pc = SEM_NEXT_PC (sem_arg, 4);

  {
    SI opval = ZEXTHISI (GETMEMHI (current_cpu, ADDSI (* FLD (f_r2), FLD (f_simm16))));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_LD_PLUS) : /* ld $dr,@$sr+ */
{
#define FLD(f) abuf->fields.fmt_ld_plus.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

do {
  SI temp1;SI temp0;
  temp0 = GETMEMSI (current_cpu, * FLD (f_r2));
  temp1 = ADDSI (* FLD (f_r2), 4);
  {
    SI opval = temp0;
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }
  {
    SI opval = temp1;
    * FLD (f_r2) = opval;
    TRACE_RESULT (current_cpu, "sr", 'x', opval);
  }
} while (0);

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_LD24) : /* ld24 $dr,$uimm24 */
{
#define FLD(f) abuf->fields.fmt_ld24.f
  new_pc = SEM_NEXT_PC (sem_arg, 4);

  {
    SI opval = FLD (f_uimm24);
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_LDI8) : /* ldi8 $dr,$simm8 */
{
#define FLD(f) abuf->fields.fmt_ldi8.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = FLD (f_simm8);
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_LDI16) : /* ldi16 $dr,$hash$slo16 */
{
#define FLD(f) abuf->fields.fmt_ldi16.f
  new_pc = SEM_NEXT_PC (sem_arg, 4);

  {
    SI opval = FLD (f_simm16);
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_LOCK) : /* lock $dr,@$sr */
{
#define FLD(f) abuf->fields.fmt_lock.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

do {
  {
    UBI opval = 1;
    CPU (h_lock) = opval;
    TRACE_RESULT (current_cpu, "lock-0", 'x', opval);
  }
  {
    SI opval = GETMEMSI (current_cpu, * FLD (f_r2));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }
} while (0);

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_MACHI) : /* machi $src1,$src2 */
{
#define FLD(f) abuf->fields.fmt_machi.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    DI opval = SRADI (SLLDI (ADDDI (m32rb_h_accum_get (current_cpu), MULDI (EXTSIDI (ANDSI (* FLD (f_r1), 0xffff0000)), EXTHIDI (TRUNCSIHI (SRASI (* FLD (f_r2), 16))))), 8), 8);
    m32rb_h_accum_set (current_cpu, opval);
    TRACE_RESULT (current_cpu, "accum", 'D', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_MACLO) : /* maclo $src1,$src2 */
{
#define FLD(f) abuf->fields.fmt_machi.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    DI opval = SRADI (SLLDI (ADDDI (m32rb_h_accum_get (current_cpu), MULDI (EXTSIDI (SLLSI (* FLD (f_r1), 16)), EXTHIDI (TRUNCSIHI (* FLD (f_r2))))), 8), 8);
    m32rb_h_accum_set (current_cpu, opval);
    TRACE_RESULT (current_cpu, "accum", 'D', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_MACWHI) : /* macwhi $src1,$src2 */
{
#define FLD(f) abuf->fields.fmt_machi.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    DI opval = SRADI (SLLDI (ADDDI (m32rb_h_accum_get (current_cpu), MULDI (EXTSIDI (* FLD (f_r1)), EXTHIDI (TRUNCSIHI (SRASI (* FLD (f_r2), 16))))), 8), 8);
    m32rb_h_accum_set (current_cpu, opval);
    TRACE_RESULT (current_cpu, "accum", 'D', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_MACWLO) : /* macwlo $src1,$src2 */
{
#define FLD(f) abuf->fields.fmt_machi.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    DI opval = SRADI (SLLDI (ADDDI (m32rb_h_accum_get (current_cpu), MULDI (EXTSIDI (* FLD (f_r1)), EXTHIDI (TRUNCSIHI (* FLD (f_r2))))), 8), 8);
    m32rb_h_accum_set (current_cpu, opval);
    TRACE_RESULT (current_cpu, "accum", 'D', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_MUL) : /* mul $dr,$sr */
{
#define FLD(f) abuf->fields.fmt_add.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = MULSI (* FLD (f_r1), * FLD (f_r2));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_MULHI) : /* mulhi $src1,$src2 */
{
#define FLD(f) abuf->fields.fmt_mulhi.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    DI opval = SRADI (SLLDI (MULDI (EXTSIDI (ANDSI (* FLD (f_r1), 0xffff0000)), EXTHIDI (TRUNCSIHI (SRASI (* FLD (f_r2), 16)))), 16), 16);
    m32rb_h_accum_set (current_cpu, opval);
    TRACE_RESULT (current_cpu, "accum", 'D', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_MULLO) : /* mullo $src1,$src2 */
{
#define FLD(f) abuf->fields.fmt_mulhi.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    DI opval = SRADI (SLLDI (MULDI (EXTSIDI (SLLSI (* FLD (f_r1), 16)), EXTHIDI (TRUNCSIHI (* FLD (f_r2)))), 16), 16);
    m32rb_h_accum_set (current_cpu, opval);
    TRACE_RESULT (current_cpu, "accum", 'D', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_MULWHI) : /* mulwhi $src1,$src2 */
{
#define FLD(f) abuf->fields.fmt_mulhi.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    DI opval = SRADI (SLLDI (MULDI (EXTSIDI (* FLD (f_r1)), EXTHIDI (TRUNCSIHI (SRASI (* FLD (f_r2), 16)))), 8), 8);
    m32rb_h_accum_set (current_cpu, opval);
    TRACE_RESULT (current_cpu, "accum", 'D', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_MULWLO) : /* mulwlo $src1,$src2 */
{
#define FLD(f) abuf->fields.fmt_mulhi.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    DI opval = SRADI (SLLDI (MULDI (EXTSIDI (* FLD (f_r1)), EXTHIDI (TRUNCSIHI (* FLD (f_r2)))), 8), 8);
    m32rb_h_accum_set (current_cpu, opval);
    TRACE_RESULT (current_cpu, "accum", 'D', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_MV) : /* mv $dr,$sr */
{
#define FLD(f) abuf->fields.fmt_mv.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = * FLD (f_r2);
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_MVFACHI) : /* mvfachi $dr */
{
#define FLD(f) abuf->fields.fmt_mvfachi.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = TRUNCDISI (SRADI (m32rb_h_accum_get (current_cpu), 32));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_MVFACLO) : /* mvfaclo $dr */
{
#define FLD(f) abuf->fields.fmt_mvfachi.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = TRUNCDISI (m32rb_h_accum_get (current_cpu));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_MVFACMI) : /* mvfacmi $dr */
{
#define FLD(f) abuf->fields.fmt_mvfachi.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = TRUNCDISI (SRADI (m32rb_h_accum_get (current_cpu), 16));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_MVFC) : /* mvfc $dr,$scr */
{
#define FLD(f) abuf->fields.fmt_mvfc.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = m32rb_h_cr_get (current_cpu, FLD (f_r2));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_MVTACHI) : /* mvtachi $src1 */
{
#define FLD(f) abuf->fields.fmt_mvtachi.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    DI opval = ORDI (ANDDI (m32rb_h_accum_get (current_cpu), MAKEDI (0, 0xffffffff)), SLLDI (EXTSIDI (* FLD (f_r1)), 32));
    m32rb_h_accum_set (current_cpu, opval);
    TRACE_RESULT (current_cpu, "accum", 'D', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_MVTACLO) : /* mvtaclo $src1 */
{
#define FLD(f) abuf->fields.fmt_mvtachi.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    DI opval = ORDI (ANDDI (m32rb_h_accum_get (current_cpu), MAKEDI (0xffffffff, 0)), ZEXTSIDI (* FLD (f_r1)));
    m32rb_h_accum_set (current_cpu, opval);
    TRACE_RESULT (current_cpu, "accum", 'D', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_MVTC) : /* mvtc $sr,$dcr */
{
#define FLD(f) abuf->fields.fmt_mvtc.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    USI opval = * FLD (f_r2);
    m32rb_h_cr_set (current_cpu, FLD (f_r1), opval);
    TRACE_RESULT (current_cpu, "dcr", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_NEG) : /* neg $dr,$sr */
{
#define FLD(f) abuf->fields.fmt_mv.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = NEGSI (* FLD (f_r2));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_NOP) : /* nop */
{
#define FLD(f) abuf->fields.fmt_nop.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

PROFILE_COUNT_FILLNOPS (current_cpu, abuf->addr);

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_NOT) : /* not $dr,$sr */
{
#define FLD(f) abuf->fields.fmt_mv.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = INVSI (* FLD (f_r2));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_RAC) : /* rac */
{
#define FLD(f) abuf->fields.fmt_rac.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

do {
  DI tmp_tmp1;
  tmp_tmp1 = SLLDI (m32rb_h_accum_get (current_cpu), 1);
  tmp_tmp1 = ADDDI (tmp_tmp1, MAKEDI (0, 32768));
  {
    DI opval = (GTDI (tmp_tmp1, MAKEDI (32767, 0xffff0000))) ? (MAKEDI (32767, 0xffff0000)) : (LTDI (tmp_tmp1, MAKEDI (0xffff8000, 0))) ? (MAKEDI (0xffff8000, 0)) : (ANDDI (tmp_tmp1, MAKEDI (0xffffffff, 0xffff0000)));
    m32rb_h_accum_set (current_cpu, opval);
    TRACE_RESULT (current_cpu, "accum", 'D', opval);
  }
} while (0);

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_RACH) : /* rach */
{
#define FLD(f) abuf->fields.fmt_rac.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

do {
  DI tmp_tmp1;
  tmp_tmp1 = ANDDI (m32rb_h_accum_get (current_cpu), MAKEDI (16777215, 0xffffffff));
if (ANDIF (GEDI (tmp_tmp1, MAKEDI (16383, 0x80000000)), LEDI (tmp_tmp1, MAKEDI (8388607, 0xffffffff)))) {
  tmp_tmp1 = MAKEDI (16383, 0x80000000);
} else {
if (ANDIF (GEDI (tmp_tmp1, MAKEDI (8388608, 0)), LEDI (tmp_tmp1, MAKEDI (16760832, 0)))) {
  tmp_tmp1 = MAKEDI (16760832, 0);
} else {
  tmp_tmp1 = ANDDI (ADDDI (m32rb_h_accum_get (current_cpu), MAKEDI (0, 1073741824)), MAKEDI (0xffffffff, 0x80000000));
}
}
  tmp_tmp1 = SLLDI (tmp_tmp1, 1);
  {
    DI opval = SRADI (SLLDI (tmp_tmp1, 7), 7);
    m32rb_h_accum_set (current_cpu, opval);
    TRACE_RESULT (current_cpu, "accum", 'D', opval);
  }
} while (0);

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_RTE) : /* rte */
{
#define FLD(f) abuf->fields.fmt_rte.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

do {
  {
    UBI opval = CPU (h_bsm);
    CPU (h_sm) = opval;
    TRACE_RESULT (current_cpu, "sm-0", 'x', opval);
  }
  {
    UBI opval = CPU (h_bie);
    CPU (h_ie) = opval;
    TRACE_RESULT (current_cpu, "ie-0", 'x', opval);
  }
  {
    UBI opval = CPU (h_bcond);
    CPU (h_cond) = opval;
    TRACE_RESULT (current_cpu, "condbit", 'x', opval);
  }
  {
    USI opval = ANDSI (CPU (h_bpc), -4);
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_ADDR (sem_arg, opval));
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
} while (0);

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_SETH) : /* seth $dr,$hash$hi16 */
{
#define FLD(f) abuf->fields.fmt_seth.f
  new_pc = SEM_NEXT_PC (sem_arg, 4);

  {
    SI opval = SLLSI (FLD (f_hi16), 16);
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_SLL) : /* sll $dr,$sr */
{
#define FLD(f) abuf->fields.fmt_add.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = SLLSI (* FLD (f_r1), ANDSI (* FLD (f_r2), 31));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_SLL3) : /* sll3 $dr,$sr,$simm16 */
{
#define FLD(f) abuf->fields.fmt_sll3.f
  new_pc = SEM_NEXT_PC (sem_arg, 4);

  {
    SI opval = SLLSI (* FLD (f_r2), ANDSI (FLD (f_simm16), 31));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_SLLI) : /* slli $dr,$uimm5 */
{
#define FLD(f) abuf->fields.fmt_slli.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = SLLSI (* FLD (f_r1), FLD (f_uimm5));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_SRA) : /* sra $dr,$sr */
{
#define FLD(f) abuf->fields.fmt_add.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = SRASI (* FLD (f_r1), ANDSI (* FLD (f_r2), 31));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_SRA3) : /* sra3 $dr,$sr,$simm16 */
{
#define FLD(f) abuf->fields.fmt_sll3.f
  new_pc = SEM_NEXT_PC (sem_arg, 4);

  {
    SI opval = SRASI (* FLD (f_r2), ANDSI (FLD (f_simm16), 31));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_SRAI) : /* srai $dr,$uimm5 */
{
#define FLD(f) abuf->fields.fmt_slli.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = SRASI (* FLD (f_r1), FLD (f_uimm5));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_SRL) : /* srl $dr,$sr */
{
#define FLD(f) abuf->fields.fmt_add.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = SRLSI (* FLD (f_r1), ANDSI (* FLD (f_r2), 31));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_SRL3) : /* srl3 $dr,$sr,$simm16 */
{
#define FLD(f) abuf->fields.fmt_sll3.f
  new_pc = SEM_NEXT_PC (sem_arg, 4);

  {
    SI opval = SRLSI (* FLD (f_r2), ANDSI (FLD (f_simm16), 31));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_SRLI) : /* srli $dr,$uimm5 */
{
#define FLD(f) abuf->fields.fmt_slli.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = SRLSI (* FLD (f_r1), FLD (f_uimm5));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_ST) : /* st $src1,@$src2 */
{
#define FLD(f) abuf->fields.fmt_st.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = * FLD (f_r1);
    SETMEMSI (current_cpu, * FLD (f_r2), opval);
    TRACE_RESULT (current_cpu, "memory", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_ST_D) : /* st $src1,@($slo16,$src2) */
{
#define FLD(f) abuf->fields.fmt_st_d.f
  new_pc = SEM_NEXT_PC (sem_arg, 4);

  {
    SI opval = * FLD (f_r1);
    SETMEMSI (current_cpu, ADDSI (* FLD (f_r2), FLD (f_simm16)), opval);
    TRACE_RESULT (current_cpu, "memory", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_STB) : /* stb $src1,@$src2 */
{
#define FLD(f) abuf->fields.fmt_stb.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    QI opval = * FLD (f_r1);
    SETMEMQI (current_cpu, * FLD (f_r2), opval);
    TRACE_RESULT (current_cpu, "memory", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_STB_D) : /* stb $src1,@($slo16,$src2) */
{
#define FLD(f) abuf->fields.fmt_stb_d.f
  new_pc = SEM_NEXT_PC (sem_arg, 4);

  {
    QI opval = * FLD (f_r1);
    SETMEMQI (current_cpu, ADDSI (* FLD (f_r2), FLD (f_simm16)), opval);
    TRACE_RESULT (current_cpu, "memory", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_STH) : /* sth $src1,@$src2 */
{
#define FLD(f) abuf->fields.fmt_sth.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    HI opval = * FLD (f_r1);
    SETMEMHI (current_cpu, * FLD (f_r2), opval);
    TRACE_RESULT (current_cpu, "memory", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_STH_D) : /* sth $src1,@($slo16,$src2) */
{
#define FLD(f) abuf->fields.fmt_sth_d.f
  new_pc = SEM_NEXT_PC (sem_arg, 4);

  {
    HI opval = * FLD (f_r1);
    SETMEMHI (current_cpu, ADDSI (* FLD (f_r2), FLD (f_simm16)), opval);
    TRACE_RESULT (current_cpu, "memory", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_ST_PLUS) : /* st $src1,@+$src2 */
{
#define FLD(f) abuf->fields.fmt_st_plus.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

do {
  SI tmp_new_src2;
  tmp_new_src2 = ADDSI (* FLD (f_r2), 4);
  {
    SI opval = * FLD (f_r1);
    SETMEMSI (current_cpu, tmp_new_src2, opval);
    TRACE_RESULT (current_cpu, "memory", 'x', opval);
  }
  {
    SI opval = tmp_new_src2;
    * FLD (f_r2) = opval;
    TRACE_RESULT (current_cpu, "src2", 'x', opval);
  }
} while (0);

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_ST_MINUS) : /* st $src1,@-$src2 */
{
#define FLD(f) abuf->fields.fmt_st_plus.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

do {
  SI tmp_new_src2;
  tmp_new_src2 = SUBSI (* FLD (f_r2), 4);
  {
    SI opval = * FLD (f_r1);
    SETMEMSI (current_cpu, tmp_new_src2, opval);
    TRACE_RESULT (current_cpu, "memory", 'x', opval);
  }
  {
    SI opval = tmp_new_src2;
    * FLD (f_r2) = opval;
    TRACE_RESULT (current_cpu, "src2", 'x', opval);
  }
} while (0);

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_SUB) : /* sub $dr,$sr */
{
#define FLD(f) abuf->fields.fmt_add.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

  {
    SI opval = SUBSI (* FLD (f_r1), * FLD (f_r2));
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_SUBV) : /* subv $dr,$sr */
{
#define FLD(f) abuf->fields.fmt_addv.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

do {
  UBI temp1;SI temp0;
  temp0 = SUBSI (* FLD (f_r1), * FLD (f_r2));
  temp1 = SUBOFSI (* FLD (f_r1), * FLD (f_r2), 0);
  {
    SI opval = temp0;
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }
  {
    UBI opval = temp1;
    CPU (h_cond) = opval;
    TRACE_RESULT (current_cpu, "condbit", 'x', opval);
  }
} while (0);

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_SUBX) : /* subx $dr,$sr */
{
#define FLD(f) abuf->fields.fmt_addx.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

do {
  UBI temp1;SI temp0;
  temp0 = SUBCSI (* FLD (f_r1), * FLD (f_r2), CPU (h_cond));
  temp1 = SUBCFSI (* FLD (f_r1), * FLD (f_r2), CPU (h_cond));
  {
    SI opval = temp0;
    * FLD (f_r1) = opval;
    TRACE_RESULT (current_cpu, "dr", 'x', opval);
  }
  {
    UBI opval = temp1;
    CPU (h_cond) = opval;
    TRACE_RESULT (current_cpu, "condbit", 'x', opval);
  }
} while (0);

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_TRAP) : /* trap $uimm4 */
{
#define FLD(f) abuf->fields.fmt_trap.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

do {
  {
    USI opval = ADDSI (CPU (h_pc), 4);
    m32rb_h_cr_set (current_cpu, ((HOSTUINT) 6), opval);
    TRACE_RESULT (current_cpu, "cr-6", 'x', opval);
  }
  {
    USI opval = ANDSI (SLLSI (m32rb_h_cr_get (current_cpu, ((HOSTUINT) 0)), 8), 65408);
    m32rb_h_cr_set (current_cpu, ((HOSTUINT) 0), opval);
    TRACE_RESULT (current_cpu, "cr-0", 'x', opval);
  }
  {
    SI opval = m32r_trap (current_cpu, CPU (h_pc), FLD (f_uimm4));
    BRANCH_NEW_PC (new_pc, SEM_BRANCH_VIA_ADDR (sem_arg, opval));
    TRACE_RESULT (current_cpu, "pc", 'x', opval);
  }
} while (0);

#undef FLD
}
  BREAK (sem);

  CASE (sem, INSN_UNLOCK) : /* unlock $src1,@$src2 */
{
#define FLD(f) abuf->fields.fmt_unlock.f
  new_pc = SEM_NEXT_PC (sem_arg, 2);

do {
if (CPU (h_lock)) {
  {
    SI opval = * FLD (f_r1);
    SETMEMSI (current_cpu, * FLD (f_r2), opval);
    TRACE_RESULT (current_cpu, "memory", 'x', opval);
  }
}
  {
    UBI opval = 0;
    CPU (h_lock) = opval;
    TRACE_RESULT (current_cpu, "lock-0", 'x', opval);
  }
} while (0);

#undef FLD
}
  BREAK (sem);


    }
  ENDSWITCH (sem) /* End of semantic switch.  */

  PC = new_pc;
}

#endif /* DEFINE_SWITCH */
