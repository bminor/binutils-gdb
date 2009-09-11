/* Target dependent code for ARC processor family, for GDB, the GNU debugger.

   Copyright 2005, 2008, 2009 Free Software Foundation, Inc.

   Contributed by Codito Technologies Pvt. Ltd. (www.codito.com)

   Authors:
      Soam Vasani          <soam.vasani@codito.com>
      Ramana Radhakrishnan <ramana.radhakrishnan@codito.com>
      Richard Stuckey      <richard.stuckey@arc.com>

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

/******************************************************************************/
/*                                                                            */
/* Outline:                                                                   */
/*     This header file defines some target-dependent information which is    */
/*     specific to the ARC gdb port.                                          */
/*                                                                            */
/******************************************************************************/

#ifndef ARC_TDEP_H
#define ARC_TDEP_H

/* ARC header files */
#include "arc-support.h"


#define ARC_PC_REGNUM       (gdbarch_pc_regnum       (current_gdbarch))
#define ARC_NUM_REGS        (gdbarch_num_regs        (current_gdbarch))
#define ARC_NUM_PSEUDO_REGS (gdbarch_num_pseudo_regs (current_gdbarch))
#define ARC_TOTAL_REGS      (ARC_NUM_REGS + ARC_NUM_PSEUDO_REGS)


#define ARC_MAX_CORE_REGS                      64
#define ARC_FIRST_EXTENSION_CORE_REGISTER      32
#define ARC_LAST_EXTENSION_CORE_REGISTER       59
#define ARC_NUM_EXTENSION_CORE_REGS            (ARC_LAST_EXTENSION_CORE_REGISTER - ARC_FIRST_EXTENSION_CORE_REGISTER + 1)
#define ARC_NUM_STANDARD_CORE_REGS             (ARC_MAX_CORE_REGS - ARC_NUM_EXTENSION_CORE_REGS)


#define IS_EXTENSION_CORE_REGISTER(hw_regnum)  (ARC_FIRST_EXTENSION_CORE_REGISTER <= (hw_regnum) && (hw_regnum) <= ARC_LAST_EXTENSION_CORE_REGISTER)


/* ARC processor ABI-related registers:
      R0  .. R7  are the registers used to pass arguments in function calls
      R13 .. R26 are the callee-saved registers
      when a return value is stored in registers it is in either R0 or in the pair (R0,R1).  */

#define ARC_ABI_GLOBAL_POINTER                 26
#define ARC_ABI_FRAME_POINTER                  27
#define ARC_ABI_STACK_POINTER                  28

#define ARC_ABI_FIRST_CALLEE_SAVED_REGISTER    13
#define ARC_ABI_LAST_CALLEE_SAVED_REGISTER     26

#define ARC_ABI_FIRST_ARGUMENT_REGISTER         0
#define ARC_ABI_LAST_ARGUMENT_REGISTER          7
#define ARC_ABI_REGISTER_PARAMETER_SPACE        ((ARC_ABI_LAST_ARGUMENT_REGISTER - ARC_ABI_FIRST_ARGUMENT_REGISTER + 1) * BYTES_IN_REGISTER)

#define ARC_ABI_RETURN_REGNUM                   0
#define ARC_ABI_RETURN_LOW_REGNUM               0
#define ARC_ABI_RETURN_HIGH_REGNUM              1

#define IS_ARGUMENT_REGISTER(hw_regnum)         (ARC_ABI_FIRST_ARGUMENT_REGISTER <= (hw_regnum) && (hw_regnum) <= ARC_ABI_LAST_ARGUMENT_REGISTER)


/* This type is completed in the files arc-elf32-tdep.h and arc-linux-tdep.h,
   as apppropriate for the arc-elf2 and arc-uclinux builds of gdb.  */
typedef struct arc_variant_info ARC_VariantsInfo;


#define REGISTER_NOT_PRESENT   (-1)   // special value for sc_reg_offset[reg]


/* N.B. this assumes that the host is little-endian!  */
#define HOST_AND_TARGET_ENDIANNESS_DIFFER(arch)    (gdbarch_byte_order (arch) == BFD_ENDIAN_BIG)


/* This structure holds target-dependent information.

   N.B. this type is used in the target-independent gdb code, but it is treated
        as an opaque (or private) type: the only use of it is by pointers to
        objects of this type (passed as parameters or returned as results, or
        held in other structures); it is only the ARC-specific modules that
        have knowledge of the structure of this type and access its fields.  */

struct gdbarch_tdep
{
    /* Detect sigtramp.  */
    Boolean (*is_sigtramp) (struct frame_info*);

    /* Get address of sigcontext for sigtramp.  */
    CORE_ADDR (*sigcontext_addr) (struct frame_info*);

    /* Offset of registers in `struct sigcontext'.  */
    const int   *sc_reg_offset;
    unsigned int sc_num_regs;

    /* In our linux target, gdbarch_pc_regnum points to stop_pc, which is a
       register that is made up by the kernel and does not actually exist.
       stop_pc is NOT saved in the sigcontext; what is saved is the ret
       "register".  Since ret is a linux-only register, its regnum is visible
       only in arc-linux-tdep.c; hence initialize pc_regnum_in_sigcontext in
       arc-linux-tdep.c.  */
    int pc_regnum_in_sigcontext;

    /* Returns 0, 1, or -1:
          0 means the register is not in the group.
          1 means the register is in the group.
         -1 means the tdep has nothing to say about this register and group.  */
    int (*register_reggroup_p) (int regnum, struct reggroup *group);

    /* Breakpoint instruction to be used.  */
    const unsigned char *be_breakpoint_instruction;
    const unsigned char *le_breakpoint_instruction;
    unsigned int         breakpoint_size;

    /* For stopping backtraces.  */
    CORE_ADDR lowest_pc;

    /* ARC processor variant information (may be NULL).  */
    ARC_VariantsInfo *processor_variant_info;
};


/* Utility functions used by other ARC-specific modules.  */

void arc_initialize_disassembler (struct disassemble_info *info);

/* A global debug flag.  */
extern Boolean arc_debug_target;

#endif /* ARC_TDEP_H */
/******************************************************************************/
