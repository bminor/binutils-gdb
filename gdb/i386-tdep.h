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

#if !defined (XMM0_REGNUM) || (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL)
#define XMM0_REGNUM FIRST_XMM_REGNUM
#endif
#if !defined (FIRST_FPU_REGNUM) || (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL)
#define FIRST_FPU_REGNUM FP0_REGNUM
#endif
#if !defined (LAST_FPU_REGNUM) || (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL)
#define LAST_FPU_REGNUM (gdbarch_tdep (current_gdbarch)->last_fpu_regnum)
#endif
#if !defined (FIRST_XMM_REGNUM) || (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL)
#define FIRST_XMM_REGNUM (gdbarch_tdep (current_gdbarch)->first_xmm_regnum)
#endif
#if !defined (LAST_XMM_REGNUM) || (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL)
#define LAST_XMM_REGNUM (gdbarch_tdep (current_gdbarch)->last_xmm_regnum)
#endif
#if !defined (MXCSR_REGNUM) || (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL)
#define MXCSR_REGNUM (gdbarch_tdep (current_gdbarch)->mxcsr_regnum)
#endif
#if !defined (FIRST_FPU_CTRL_REGNUM) || (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL)
#define FIRST_FPU_CTRL_REGNUM (gdbarch_tdep (current_gdbarch)->first_fpu_ctrl_regnum)
#endif
#if !defined (LAST_FPU_CTRL_REGNUM) || (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL)
#define LAST_FPU_CTRL_REGNUM (FIRST_FPU_CTRL_REGNUM + 7)
#endif

/* All of these control registers (except for FCOFF and FDOFF) are
   sixteen bits long (at most) in the FPU, but are zero-extended to
   thirty-two bits in GDB's register file.  This makes it easier to
   compute the size of the control register file, and somewhat easier
   to convert to and from the FSAVE instruction's 32-bit format.  */
/* FPU control word.  */
#if !defined (FCTRL_REGNUM) || (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL)
#define FCTRL_REGNUM (FIRST_FPU_CTRL_REGNUM)
#endif
/* FPU status word.  */
#if !defined (FSTAT_REGNUM) || (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL)
#define FSTAT_REGNUM (FIRST_FPU_CTRL_REGNUM + 1)
#endif
/* FPU register tag word.  */
#if !defined (FTAG_REGNUM) || (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL)
#define FTAG_REGNUM  (FIRST_FPU_CTRL_REGNUM + 2)
#endif
/* FPU instruction's code segment selector 16 bits, called "FPU Instruction
   Pointer Selector" in the x86 manuals.  */
#if !defined (FCS_REGNUM) || (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL)
#define FCS_REGNUM   (FIRST_FPU_CTRL_REGNUM + 3)
#endif
/* FPU instruction's offset within segment ("Fpu Code OFFset").  */
#if !defined (FCOFF_REGNUM) || (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL)
#define FCOFF_REGNUM (FIRST_FPU_CTRL_REGNUM + 4)
#endif
/* FPU operand's data segment.  */
#if !defined (FDS_REGNUM) || (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL)
#define FDS_REGNUM   (FIRST_FPU_CTRL_REGNUM + 5)
#endif
/* FPU operand's offset within segment.  */
#if !defined (FDOFF_REGNUM) || (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL)
#define FDOFF_REGNUM (FIRST_FPU_CTRL_REGNUM + 6)
#endif
/* FPU opcode, bottom eleven bits.  */
#if !defined (FOP_REGNUM) || (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL)
#define FOP_REGNUM   (FIRST_FPU_CTRL_REGNUM + 7)
#endif

/* i386 architecture specific information.  */
struct gdbarch_tdep
{
  int last_fpu_regnum;
  int first_xmm_regnum;
  int last_xmm_regnum;
  int mxcsr_regnum;		/* Streaming SIMD Extension control/status.  */
  int first_fpu_ctrl_regnum;
};

#if !defined (IS_FP_REGNUM) || (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL)
#define IS_FP_REGNUM(n) (FIRST_FPU_REGNUM <= (n) && (n) <= LAST_FPU_REGNUM)
#endif
#if !defined (IS_FPU_CTRL_REGNUM) || (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL)
#define IS_FPU_CTRL_REGNUM(n) (FIRST_FPU_CTRL_REGNUM <= (n) && (n) <= LAST_FPU_CTRL_REGNUM)
#endif
#if !defined (IS_SSE_REGNUM) || (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL)
#define IS_SSE_REGNUM(n) (FIRST_XMM_REGNUM <= (n) && (n) <= LAST_XMM_REGNUM)
#endif

#endif
