/* Common target dependent code for GDB on SPARC systems.
   Copyright 2002 Free Software Foundation, Inc.

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

/*
 * Some local macros that have multi-arch and non-multi-arch versions:
 */

#if (GDB_MULTI_ARCH > 0)

/* Does the target have Floating Point registers?  */
#define SPARC_HAS_FPU     (gdbarch_tdep (current_gdbarch)->has_fpu)
/* Number of bytes devoted to Floating Point registers: */
#define FP_REGISTER_BYTES (gdbarch_tdep (current_gdbarch)->fp_register_bytes)
/* Highest numbered Floating Point register.  */
#define FP_MAX_REGNUM     (gdbarch_tdep (current_gdbarch)->fp_max_regnum)
/* Size of a general (integer) register: */
#define SPARC_INTREG_SIZE (gdbarch_tdep (current_gdbarch)->intreg_size)
/* Offset within the call dummy stack of the saved registers.  */
#define DUMMY_REG_SAVE_OFFSET (gdbarch_tdep (current_gdbarch)->reg_save_offset)

#else /* non-multi-arch */

/* Does the target have Floating Point registers?  */
#if defined(TARGET_SPARCLET) || defined(TARGET_SPARCLITE)
#define SPARC_HAS_FPU 0
#else
#define SPARC_HAS_FPU 1
#endif

/* Number of bytes devoted to Floating Point registers: */
#if (GDB_TARGET_IS_SPARC64)
#define FP_REGISTER_BYTES (64 * 4)
#else
#if (SPARC_HAS_FPU)
#define FP_REGISTER_BYTES (32 * 4)
#else
#define FP_REGISTER_BYTES 0
#endif
#endif

/* Highest numbered Floating Point register.  */
#if (GDB_TARGET_IS_SPARC64)
#define FP_MAX_REGNUM (FP0_REGNUM + 48)
#else
#define FP_MAX_REGNUM (FP0_REGNUM + 32)
#endif

/* Size of a general (integer) register: */
#define SPARC_INTREG_SIZE (REGISTER_RAW_SIZE (G0_REGNUM))

/* Offset within the call dummy stack of the saved registers.  */
#if (GDB_TARGET_IS_SPARC64)
#define DUMMY_REG_SAVE_OFFSET (128 + 16)
#else
#define DUMMY_REG_SAVE_OFFSET 0x60
#endif

#endif /* GDB_MULTI_ARCH */

#ifndef CALL_DUMMY_CALL_OFFSET
#define CALL_DUMMY_CALL_OFFSET \
     (gdbarch_tdep (current_gdbarch)->call_dummy_call_offset)
#endif /* CALL_DUMMY_CALL_OFFSET */

struct gdbarch_tdep
  {
    int os_ident;
    int has_fpu;
    int fp_register_bytes;
    int y_regnum;
    int fp_max_regnum;
    int intreg_size;
    int reg_save_offset;
    int call_dummy_call_offset;
    int print_insn_mach;
  };

/* How a OS variant tells the SPARC generic code that it can handle an ABI
   type.  */
extern void sparc_gdbarch_register_os_abi (int os_ident,
					   void (*init_abi)(struct gdbarch_info,
							    struct gdbarch *));
