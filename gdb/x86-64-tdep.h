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

/* Register numbers of various important registers.  */

#define X86_64_RAX_REGNUM	0 /* %rax */
#define X86_64_RDX_REGNUM	3 /* %rdx */
#define X86_64_RDI_REGNUM	5 /* %rdi */
#define X86_64_RBP_REGNUM	6 /* %rbp */
#define X86_64_RSP_REGNUM	7 /* %rsp */
#define X86_64_RIP_REGNUM	16 /* %rip */
#define X86_64_EFLAGS_REGNUM	17 /* %eflags */
#define X86_64_ST0_REGNUM	22 /* %st0 */
#define X86_64_XMM0_REGNUM	38 /* %xmm0 */
#define X86_64_XMM1_REGNUM	39 /* %xmm1 */

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

#endif /* x86-64-tdep.h */
