/* Target-dependent code for GDB, the GNU debugger.
   Copyright 2001
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

#include "i386-tdep.h"
#include <sys/reg.h>

/* Mapping between the general-purpose registers in `struct user'
   format and GDB's register array layout.  */

static int x86_64_regmap[] = {
  RAX, RDX, RCX, RBX,
  RSI, RDI, RBP, RSP,
  R8, R9, R10, R11,
  R12, R13, R14, R15,
  RIP, EFLAGS
};

/* Number of all registers */
#define X86_64_NUM_REGS (51)

/* Number of general registers.  */
#define X86_64_NUM_GREGS (18)

gdbarch_frame_saved_pc_ftype x86_64_linux_frame_saved_pc;
gdbarch_saved_pc_after_call_ftype x86_64_linux_saved_pc_after_call;

#endif
