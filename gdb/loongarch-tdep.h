/* Target-dependent header for the LoongArch architecture, for GDB.

   Copyright (C) 2022 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef LOONGARCH_TDEP_H
#define LOONGARCH_TDEP_H

#include "gdbarch.h"
#include "arch/loongarch.h"
#include "regset.h"

#include "elf/loongarch.h"
#include "opcode/loongarch.h"

/* Register numbers of various important registers.  */
enum
{
  LOONGARCH_RA_REGNUM = 1,		/* Return Address.  */
  LOONGARCH_SP_REGNUM = 3,		/* Stack Pointer.  */
  LOONGARCH_A0_REGNUM = 4,		/* First Argument/Return Value.  */
  LOONGARCH_A7_REGNUM = 11,		/* Seventh Argument/Syscall Number.  */
  LOONGARCH_FP_REGNUM = 22,		/* Frame Pointer.  */
  LOONGARCH_ORIG_A0_REGNUM = 32,	/* Syscall's original arg0.  */
  LOONGARCH_PC_REGNUM = 33,		/* Program Counter.  */
  LOONGARCH_BADV_REGNUM = 34,		/* Bad Vaddr for Addressing Exception.  */
  LOONGARCH_LINUX_NUM_GREGSET = 45,	/* 32 GPR, ORIG_A0, PC, BADV, RESERVED 10.  */
};

/* Register set definitions.  */
extern const struct regset loongarch_gregset;

/* Target-dependent structure in gdbarch.  */
struct loongarch_gdbarch_tdep : gdbarch_tdep
{
  /* Features about the abi that impact how the gdbarch is configured.  */
  struct loongarch_gdbarch_features abi_features;

  /* Return the expected next PC if FRAME is stopped at a syscall instruction.  */
  CORE_ADDR (*syscall_next_pc) (struct frame_info *frame) = nullptr;
};

#endif /* LOONGARCH_TDEP_H  */
