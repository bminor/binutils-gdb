/* Target-dependent code for GDB, the GNU debugger.
   Copyright 2001
   Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#ifndef I386_TDEP_H
#define I386_TDEP_H

#define FPU_REG_RAW_SIZE 10

#define XMM0_REGNUM FIRST_XMM_REGNUM
#define FIRST_FPU_REGNUM FP0_REGNUM
#define LAST_FPU_REGNUM (gdbarch_tdep (current_gdbarch)->last_fpu_regnum)
#define FIRST_XMM_REGNUM (gdbarch_tdep (current_gdbarch)->first_xmm_regnum)
#define LAST_XMM_REGNUM (gdbarch_tdep (current_gdbarch)->last_xmm_regnum)
#define MXCSR_REGNUM (gdbarch_tdep (current_gdbarch)->mxcsr_regnum)
#define FIRST_FPU_CTRL_REGNUM (gdbarch_tdep (current_gdbarch)->first_fpu_ctrl_regnum)
#define LAST_FPU_CTRL_REGNUM (FIRST_FPU_CTRL_REGNUM + 7)

/* All of these control registers (except for FCOFF and FDOFF) are
   sixteen bits long (at most) in the FPU, but are zero-extended to
   thirty-two bits in GDB's register file.  This makes it easier to
   compute the size of the control register file, and somewhat easier
   to convert to and from the FSAVE instruction's 32-bit format.  */
/* FPU control word.  */
#define FCTRL_REGNUM (FIRST_FPU_CTRL_REGNUM)
/* FPU status word.  */
#define FSTAT_REGNUM (FIRST_FPU_CTRL_REGNUM + 1)
/* FPU register tag word.  */
#define FTAG_REGNUM  (FIRST_FPU_CTRL_REGNUM + 2)
/* FPU instruction's code segment selector 16 bits, called "FPU Instruction
   Pointer Selector" in the x86 manuals.  */
#define FCS_REGNUM   (FIRST_FPU_CTRL_REGNUM + 3)
/* FPU instruction's offset within segment ("Fpu Code OFFset").  */
#define FCOFF_REGNUM (FIRST_FPU_CTRL_REGNUM + 4)
/* FPU operand's data segment.  */
#define FDS_REGNUM   (FIRST_FPU_CTRL_REGNUM + 5)
/* FPU operand's offset within segment.  */
#define FDOFF_REGNUM (FIRST_FPU_CTRL_REGNUM + 6)
/* FPU opcode, bottom eleven bits.  */
#define FOP_REGNUM   (FIRST_FPU_CTRL_REGNUM + 7)

/* i386 architecture specific information.  */
struct gdbarch_tdep
{
  int last_fpu_regnum;
  int first_xmm_regnum;
  int last_xmm_regnum;
  int mxcsr_regnum;		/* Streaming SIMD Extension control/status.  */
  int first_fpu_ctrl_regnum;
};

#define IS_FP_REGNUM(n) (FIRST_FPU_REGNUM <= (n) && (n) <= LAST_FPU_REGNUM)
#define IS_FPU_CTRL_REGNUM(n) (FIRST_FPU_CTRL_REGNUM <= (n) && (n) <= LAST_FPU_CTRL_REGNUM)
#define IS_SSE_REGNUM(n) (FIRST_XMM_REGNUM <= (n) && (n) <= LAST_XMM_REGNUM)

#endif
