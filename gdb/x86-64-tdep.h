/* Target-dependent code for the x86-64.

   Copyright 2001, 2003
   Free Software Foundation, Inc.

   Contributed by Jiri Smid, SuSE Labs.

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

#ifndef X86_64_TDEP_H
#define X86_64_TDEP_H

struct gdbarch;
struct frame_info;

#include "i386-tdep.h"

/* Register numbers of x86-64 registers.  */

enum x86_64_regnums
{
  X86_64_RAX_REGNUM,
  X86_64_RBX_REGNUM,
  X86_64_RCX_REGNUM,
  X86_64_RDX_REGNUM,
  X86_64_RSI_REGNUM,
  X86_64_RDI_REGNUM,
  X86_64_RBP_REGNUM,
  X86_64_RSP_REGNUM,
  X86_64_R8_REGNUM,
  X86_64_R9_REGNUM,
  X86_64_R10_REGNUM,
  X86_64_R11_REGNUM,
  X86_64_R12_REGNUM,
  X86_64_R13_REGNUM,
  X86_64_R14_REGNUM,
  X86_64_R15_REGNUM,
  X86_64_RIP_REGNUM,
  X86_64_EFLAGS_REGNUM,
  X86_64_DS_REGNUM,
  X86_64_ES_REGNUM,
  X86_64_FS_REGNUM,
  X86_64_GS_REGNUM,
  X86_64_ST0_REGNUM,
  X86_64_ST1_REGNUM,
  X86_64_ST2_REGNUM,
  X86_64_ST3_REGNUM,
  X86_64_ST4_REGNUM,
  X86_64_ST5_REGNUM,
  X86_64_ST6_REGNUM,
  X86_64_ST7_REGNUM,
  X86_64_FCTRL_REGNUM,
  X86_64_FSTAT_REGNUM,
  X86_64_FTAG_REGNUM,
  X86_64_FISEG_REGNUM,
  X86_64_FIOFF_REGNUM,
  X86_64_FOSEG_REGNUM,
  X86_64_FOOFF_REGNUM,
  X86_64_FOP_REGNUM,
  X86_64_XMM0_REGNUM,
  X86_64_XMM1_REGNUM,
  X86_64_XMM2_REGNUM,
  X86_64_XMM3_REGNUM,
  X86_64_XMM4_REGNUM,
  X86_64_XMM5_REGNUM,
  X86_64_XMM6_REGNUM,
  X86_64_XMM7_REGNUM,
  X86_64_XMM8_REGNUM,
  X86_64_XMM9_REGNUM,
  X86_64_XMM10_REGNUM,
  X86_64_XMM11_REGNUM,
  X86_64_XMM12_REGNUM,
  X86_64_XMM13_REGNUM,
  X86_64_XMM14_REGNUM,
  X86_64_XMM15_REGNUM,
  X86_64_MXCSR_REGNUM
};

/* Number of general purpose registers.  */
#define X86_64_NUM_GREGS	22

void x86_64_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch);

/* Fill GDB's register array with the floating-point and SSE register
   values in *FXSAVE.  This function masks off any of the reserved
   bits in *FXSAVE.  */

void x86_64_supply_fxsave (char *fxsave);

/* Fill register REGNUM (if it is a floating-point or SSE register) in
   *FXSAVE with the value in GDB's register array.  If REGNUM is -1, do
   this for all registers.  This function doesn't touch any of the
   reserved bits in *FXSAVE.  */

void x86_64_fill_fxsave (char *fxsave, int regnum);


/* Variables exported from amd64fbsd-tdep.c.  */
extern CORE_ADDR amd64fbsd_sigtramp_start;
extern CORE_ADDR amd64fbsd_sigtramp_end;
extern int amd64fbsd_sc_reg_offset[];

#endif /* x86-64-tdep.h */
