/* Target dependent code for ARC arhitecture, for GDB.

   Copyright 2005-2016 Free Software Foundation, Inc.
   Contributed by Synopsys Inc.

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

#ifndef ARC_TDEP_H
#define ARC_TDEP_H

/* Need disassemble_info.  */
#include "dis-asm.h"

enum arc_regnum
  {
    /* Core registers.  */
    ARC_R0_REGNUM = 0,
    ARC_FIRST_CORE_REGNUM = ARC_R0_REGNUM,
    ARC_R1_REGNUM = 1,
    ARC_R4_REGNUM = 4,
    ARC_R7_REGNUM = 7,
    ARC_R9_REGNUM = 9,
    ARC_R13_REGNUM = 13,
    ARC_R16_REGNUM = 16,
    ARC_R25_REGNUM = 25,
    /* Global data pointer.  */
    ARC_GP_REGNUM,
    /* Frame pointer.  */
    ARC_FP_REGNUM,
    /* Stack pointer.  */
    ARC_SP_REGNUM,
    /* Return address from interrupt.  */
    ARC_ILINK_REGNUM,
    ARC_R30_REGNUM,
    /* Return address from function.  */
    ARC_BLINK_REGNUM,
    /* Zero-delay loop counter.  */
    ARC_LP_COUNT_REGNUM = 60,
    /* Program counter, aligned to 4-bytes, read-only.  */
    ARC_PCL_REGNUM,
    ARC_LAST_CORE_REGNUM = ARC_PCL_REGNUM,
    /* AUX registers.  */
    /* Actual program counter.  */
    ARC_PC_REGNUM,
    ARC_FIRST_AUX_REGNUM = ARC_PC_REGNUM,
    /* Status register.  */
    ARC_STATUS32_REGNUM,
    ARC_LAST_REGNUM = ARC_STATUS32_REGNUM,
    ARC_LAST_AUX_REGNUM = ARC_STATUS32_REGNUM,

    /* Additional ABI constants.  */
    ARC_FIRST_ARG_REGNUM = ARC_R0_REGNUM,
    ARC_LAST_ARG_REGNUM = ARC_R7_REGNUM,
    ARC_FIRST_CALLEE_SAVED_REGNUM = ARC_R13_REGNUM,
    ARC_LAST_CALLEE_SAVED_REGNUM = ARC_R25_REGNUM,
  };

/* Number of bytes in ARC register.  All ARC registers are considered 32-bit.
   Those registers, which are actually shorter has zero-on-read for extra bits.
   Longer registers are represented as pairs of 32-bit registers.  */
#define ARC_REGISTER_SIZE  4

#define arc_print(fmt, args...) fprintf_unfiltered (gdb_stdlog, fmt, ##args)

extern int arc_debug;

/* Target-dependent information.  */

struct gdbarch_tdep
{
  /* Offset to PC value in jump buffer.  If this is negative, longjmp
     support will be disabled.  */
  int jb_pc;
};

/* Utility functions used by other ARC-specific modules.  */

static inline int
arc_mach_is_arc600 (struct gdbarch *gdbarch)
{
  return (gdbarch_bfd_arch_info (gdbarch)->mach == bfd_mach_arc_arc600
	  || gdbarch_bfd_arch_info (gdbarch)->mach == bfd_mach_arc_arc601);
}

static inline int
arc_mach_is_arc700 (struct gdbarch *gdbarch)
{
  return gdbarch_bfd_arch_info (gdbarch)->mach == bfd_mach_arc_arc700;
}

static inline int
arc_mach_is_arcv2 (struct gdbarch *gdbarch)
{
  return gdbarch_bfd_arch_info (gdbarch)->mach == bfd_mach_arc_arcv2;
}

#endif /* ARC_TDEP_H */
