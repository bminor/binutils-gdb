/* Architecture commands for GDB, the GNU debugger.
   Copyright 1998-1999, Free Software Foundation, Inc.

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#ifndef GDBARCH_H
#define GDBARCH_H

#ifdef __STDC__
struct frame_info;
struct value;
enum lval_type;
#endif

/* The target-system-dependant byte order is dynamic */

/* TARGET_BYTE_ORDER_SELECTABLE_P determines if the target endianness
   is selectable at runtime.  The user can use the ``set endian''
   command to change it.  TARGET_BYTE_ORDER_AUTO is nonzero when
   target_byte_order should be auto-detected (from the program image
   say). */

#ifndef TARGET_BYTE_ORDER_SELECTABLE_P
/* compat - Catch old targets that define TARGET_BYTE_ORDER_SLECTABLE
   when they should have defined TARGET_BYTE_ORDER_SELECTABLE_P 1 */
#ifdef TARGET_BYTE_ORDER_SELECTABLE
#define TARGET_BYTE_ORDER_SELECTABLE_P 1
#else
#define TARGET_BYTE_ORDER_SELECTABLE_P 0
#endif
#endif

extern int target_byte_order;
#ifdef TARGET_BYTE_ORDER_SELECTABLE
/* compat - Catch old targets that define TARGET_BYTE_ORDER_SELECTABLE
   and expect defs.h to re-define TARGET_BYTE_ORDER. */
#undef TARGET_BYTE_ORDER
#endif
#ifndef TARGET_BYTE_ORDER
#define TARGET_BYTE_ORDER (target_byte_order + 0)
#endif

extern int target_byte_order_auto;
#ifndef TARGET_BYTE_ORDER_AUTO
#define TARGET_BYTE_ORDER_AUTO (target_byte_order_auto + 0)
#endif



/* The target-system-dependant BFD architecture is dynamic */

extern int target_architecture_auto;
#ifndef TARGET_ARCHITECTURE_AUTO
#define TARGET_ARCHITECTURE_AUTO (target_architecture_auto + 0)
#endif

extern const struct bfd_arch_info *target_architecture;
#ifndef TARGET_ARCHITECTURE
#define TARGET_ARCHITECTURE (target_architecture + 0)
#endif

/* Notify the target dependant backend of a change to the selected
   architecture. A zero return status indicates that the target did
   not like the change. */

extern int (*target_architecture_hook) PARAMS ((const struct bfd_arch_info *)); 



/* The target-system-dependant disassembler is semi-dynamic */

#include "dis-asm.h"		/* Get defs for disassemble_info */

extern int dis_asm_read_memory PARAMS ((bfd_vma memaddr, bfd_byte *myaddr,
					int len, disassemble_info *info));

extern void dis_asm_memory_error PARAMS ((int status, bfd_vma memaddr,
					  disassemble_info *info));

extern void dis_asm_print_address PARAMS ((bfd_vma addr,
					   disassemble_info *info));

extern int (*tm_print_insn) PARAMS ((bfd_vma, disassemble_info*));
extern disassemble_info tm_print_insn_info;
#ifndef TARGET_PRINT_INSN
#define TARGET_PRINT_INSN(vma, info) (*tm_print_insn) (vma, info)
#endif
#ifndef TARGET_PRINT_INSN_INFO
#define TARGET_PRINT_INSN_INFO (&tm_print_insn_info)
#endif



/* Explicit test for D10V architecture.
   USE of these macro's is *STRONGLY* discouraged. */

#define GDB_TARGET_IS_D10V (TARGET_ARCHITECTURE->arch == bfd_arch_d10v)
#ifndef D10V_MAKE_DADDR
#define D10V_MAKE_DADDR(X) (abort (), 0)
#endif
#ifndef D10V_MAKE_IADDR
#define D10V_MAKE_IADDR(X) (abort (), 0)
#endif


/* Fallback definition of FRAMELESS_FUNCTION_INVOCATION */
#ifndef FRAMELESS_FUNCTION_INVOCATION
#define FRAMELESS_FUNCTION_INVOCATION(FI) (0)
#endif


/* Fallback definition of REGISTER_CONVERTIBLE etc */
#ifndef REGISTER_CONVERTIBLE
#define REGISTER_CONVERTIBLE(x) (0)
#endif
#ifndef REGISTER_CONVERT_TO_VIRTUAL
#define REGISTER_CONVERT_TO_VIRTUAL(x, y, z, a)
#endif
#ifndef REGISTER_CONVERT_TO_RAW
#define REGISTER_CONVERT_TO_RAW(x, y, z, a)
#endif


/* Fallback definition for EXTRACT_STRUCT_VALUE_ADDRESS */
#ifndef EXTRACT_STRUCT_VALUE_ADDRESS
#define EXTRACT_STRUCT_VALUE_ADDRESS_P (0)
#define EXTRACT_STRUCT_VALUE_ADDRESS(X) (abort (), 0)
#else
#ifndef EXTRACT_STRUCT_VALUE_ADDRESS_P
#define EXTRACT_STRUCT_VALUE_ADDRESS_P (1)
#endif
#endif


/* Fallback definition for REGISTER_NAME for systems still defining
   REGISTER_NAMES. */
#ifndef REGISTER_NAME
extern char *gdb_register_names[];
#define REGISTER_NAME(i) gdb_register_names[i]
#endif


/* Set the dynamic target-system-dependant parameters (architecture,
   byte-order, ...) using information found in the BFD */

extern void set_gdbarch_from_file PARAMS ((bfd *));


/* Explicitly set the dynamic target-system-dependant parameters based
   on bfd_architecture and machine. */

extern void set_architecture_from_arch_mach PARAMS ((enum bfd_architecture, unsigned long));


/* Helper function for targets that don't know how my arguments are
   being passed */

extern int frame_num_args_unknown PARAMS ((struct frame_info *fi));


/* gdbarch trace variable */
extern int gdbarch_debug;

#endif
