// OBSOLETE /* Parameters for execution on a Matsushita mn10200 processor.
// OBSOLETE 
// OBSOLETE    Copyright 1997, 1998, 1999, 2000, 2001, 2002, 2003 Free Software
// OBSOLETE    Foundation, Inc.
// OBSOLETE 
// OBSOLETE    Contributed by Geoffrey Noer <noer@cygnus.com>
// OBSOLETE 
// OBSOLETE    This file is part of GDB.
// OBSOLETE 
// OBSOLETE    This program is free software; you can redistribute it and/or modify
// OBSOLETE    it under the terms of the GNU General Public License as published by
// OBSOLETE    the Free Software Foundation; either version 2 of the License, or
// OBSOLETE    (at your option) any later version.
// OBSOLETE 
// OBSOLETE    This program is distributed in the hope that it will be useful,
// OBSOLETE    but WITHOUT ANY WARRANTY; without even the implied warranty of
// OBSOLETE    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// OBSOLETE    GNU General Public License for more details.
// OBSOLETE 
// OBSOLETE    You should have received a copy of the GNU General Public License
// OBSOLETE    along with this program; if not, write to the Free Software
// OBSOLETE    Foundation, Inc., 59 Temple Place - Suite 330,
// OBSOLETE    Boston, MA 02111-1307, USA.  */
// OBSOLETE 
// OBSOLETE /* FIXME: cagney/2001-03-01: The below macros refer to functions
// OBSOLETE    declared in "regcache.h".  The ``correct fix'' is to convert those
// OBSOLETE    macros into functions.  */
// OBSOLETE #include "regcache.h"
// OBSOLETE #include "symtab.h"		/* For namespace_enum.  */
// OBSOLETE #include "symfile.h"		/* For entry_point_address().  */
// OBSOLETE 
// OBSOLETE /* ints are only 16bits on the mn10200.  */
// OBSOLETE #undef TARGET_INT_BIT
// OBSOLETE #define TARGET_INT_BIT 16
// OBSOLETE 
// OBSOLETE /* The mn10200 doesn't support long long types.  */
// OBSOLETE #undef TARGET_LONG_LONG_BIT
// OBSOLETE #define TARGET_LONG_LONG_BIT 32
// OBSOLETE 
// OBSOLETE /* The mn10200 doesn't support double or long double either.  */
// OBSOLETE #undef TARGET_DOUBLE_BIT
// OBSOLETE #undef TARGET_LONG_DOUBLE_BIT
// OBSOLETE #define TARGET_DOUBLE_BIT 32
// OBSOLETE #define TARGET_LONG_DOUBLE_BIT 32
// OBSOLETE 
// OBSOLETE /* Not strictly correct, but the machine independent code is not
// OBSOLETE    ready to handle any of the basic sizes not being a power of two.  */
// OBSOLETE #undef TARGET_PTR_BIT
// OBSOLETE #define TARGET_PTR_BIT 32
// OBSOLETE 
// OBSOLETE /* The mn10200 really has 24 bit registers but the simulator reads/writes
// OBSOLETE    them as 32bit values, so we claim they're 32bits each.  This may have
// OBSOLETE    to be tweaked if the Matsushita emulator/board really deals with them
// OBSOLETE    as 24bits each.  */
// OBSOLETE #define REGISTER_SIZE 4
// OBSOLETE 
// OBSOLETE #define MAX_REGISTER_RAW_SIZE REGISTER_SIZE
// OBSOLETE #define NUM_REGS 11
// OBSOLETE 
// OBSOLETE #define REGISTER_BYTES (NUM_REGS * REGISTER_SIZE)
// OBSOLETE 
// OBSOLETE #define REGISTER_NAMES \
// OBSOLETE { "d0", "d1", "d2", "d3", "a0", "a1", "a2", "sp", \
// OBSOLETE   "pc", "mdr", "psw"}
// OBSOLETE 
// OBSOLETE #define FP_REGNUM 6
// OBSOLETE #define SP_REGNUM 7
// OBSOLETE #define PC_REGNUM 8
// OBSOLETE #define MDR_REGNUM 9
// OBSOLETE #define PSW_REGNUM 10
// OBSOLETE 
// OBSOLETE /* Treat the registers as 32bit values.  */
// OBSOLETE #define REGISTER_VIRTUAL_TYPE(REG) builtin_type_long
// OBSOLETE 
// OBSOLETE #define REGISTER_BYTE(REG) ((REG) * REGISTER_SIZE)
// OBSOLETE #define REGISTER_VIRTUAL_SIZE(REG) REGISTER_SIZE
// OBSOLETE #define REGISTER_RAW_SIZE(REG) REGISTER_SIZE
// OBSOLETE 
// OBSOLETE #define MAX_REGISTER_VIRTUAL_SIZE REGISTER_SIZE
// OBSOLETE 
// OBSOLETE /* The breakpoint instruction must be the same size as te smallest
// OBSOLETE    instruction in the instruction set.
// OBSOLETE 
// OBSOLETE    The Matsushita mn10x00 processors have single byte instructions
// OBSOLETE    so we need a single byte breakpoint.  Matsushita hasn't defined
// OBSOLETE    one, so we defined it ourselves.
// OBSOLETE 
// OBSOLETE    0xff is the only available single byte insn left on the mn10200.  */
// OBSOLETE #define BREAKPOINT {0xff}
// OBSOLETE 
// OBSOLETE #define FUNCTION_START_OFFSET 0
// OBSOLETE 
// OBSOLETE #define DECR_PC_AFTER_BREAK 0
// OBSOLETE 
// OBSOLETE /* Stacks grow the normal way.  */
// OBSOLETE #define INNER_THAN(lhs,rhs) ((lhs) < (rhs))
// OBSOLETE 
// OBSOLETE #define SAVED_PC_AFTER_CALL(frame) \
// OBSOLETE   (read_memory_integer (read_register (SP_REGNUM), REGISTER_SIZE) & 0xffffff)
// OBSOLETE 
// OBSOLETE struct frame_info;
// OBSOLETE struct frame_saved_regs;
// OBSOLETE struct type;
// OBSOLETE struct value;
// OBSOLETE 
// OBSOLETE #define EXTRA_FRAME_INFO struct frame_saved_regs fsr; int status; int stack_size;
// OBSOLETE 
// OBSOLETE extern void mn10200_init_extra_frame_info (struct frame_info *);
// OBSOLETE #define INIT_EXTRA_FRAME_INFO(fromleaf, fi) mn10200_init_extra_frame_info (fi)
// OBSOLETE #define DEPRECATED_INIT_FRAME_PC(x,y) (init_frame_pc_noop (x, y))
// OBSOLETE #define INIT_FRAME_PC_FIRST(x,y) (init_frame_pc_noop (x, y))
// OBSOLETE 
// OBSOLETE extern void mn10200_frame_find_saved_regs (struct frame_info *,
// OBSOLETE 					   struct frame_saved_regs *);
// OBSOLETE #define FRAME_FIND_SAVED_REGS(fi, regaddr) regaddr = fi->fsr
// OBSOLETE 
// OBSOLETE extern CORE_ADDR mn10200_frame_chain (struct frame_info *);
// OBSOLETE #define FRAME_CHAIN(fi) mn10200_frame_chain (fi)
// OBSOLETE 
// OBSOLETE extern CORE_ADDR mn10200_find_callers_reg (struct frame_info *, int);
// OBSOLETE extern CORE_ADDR mn10200_frame_saved_pc (struct frame_info *);
// OBSOLETE #define FRAME_SAVED_PC(FI) (mn10200_frame_saved_pc (FI))
// OBSOLETE 
// OBSOLETE /* Extract from an array REGBUF containing the (raw) register state
// OBSOLETE    a function return value of type TYPE, and copy that, in virtual format,
// OBSOLETE    into VALBUF.  */
// OBSOLETE 
// OBSOLETE #define DEPRECATED_EXTRACT_RETURN_VALUE(TYPE, REGBUF, VALBUF) \
// OBSOLETE   { \
// OBSOLETE     if (TYPE_LENGTH (TYPE) > 8) \
// OBSOLETE       internal_error (__FILE__, __LINE__, "failed internal consistency check"); \
// OBSOLETE     else if (TYPE_LENGTH (TYPE) > 2 && TYPE_CODE (TYPE) != TYPE_CODE_PTR) \
// OBSOLETE       { \
// OBSOLETE 	memcpy (VALBUF, REGBUF + REGISTER_BYTE (0), 2); \
// OBSOLETE 	memcpy (VALBUF + 2, REGBUF + REGISTER_BYTE (1), 2); \
// OBSOLETE       } \
// OBSOLETE     else if (TYPE_CODE (TYPE) == TYPE_CODE_PTR)\
// OBSOLETE       { \
// OBSOLETE         memcpy (VALBUF, REGBUF + REGISTER_BYTE (4), TYPE_LENGTH (TYPE)); \
// OBSOLETE       } \
// OBSOLETE     else \
// OBSOLETE       { \
// OBSOLETE         memcpy (VALBUF, REGBUF + REGISTER_BYTE (0), TYPE_LENGTH (TYPE)); \
// OBSOLETE       } \
// OBSOLETE   }
// OBSOLETE 
// OBSOLETE #define DEPRECATED_EXTRACT_STRUCT_VALUE_ADDRESS(REGBUF) \
// OBSOLETE   extract_address (REGBUF + REGISTER_BYTE (4), \
// OBSOLETE 		   REGISTER_RAW_SIZE (4))
// OBSOLETE 
// OBSOLETE #define DEPRECATED_STORE_RETURN_VALUE(TYPE, VALBUF) \
// OBSOLETE   { \
// OBSOLETE     if (TYPE_LENGTH (TYPE) > 8) \
// OBSOLETE       internal_error (__FILE__, __LINE__, "failed internal consistency check"); \
// OBSOLETE     else if (TYPE_LENGTH (TYPE) > 2 && TYPE_CODE (TYPE) != TYPE_CODE_PTR) \
// OBSOLETE       { \
// OBSOLETE 	deprecated_write_register_bytes (REGISTER_BYTE (0), VALBUF, 2); \
// OBSOLETE 	deprecated_write_register_bytes (REGISTER_BYTE (1), VALBUF + 2, 2); \
// OBSOLETE       } \
// OBSOLETE     else if (TYPE_CODE (TYPE) == TYPE_CODE_PTR)\
// OBSOLETE       { \
// OBSOLETE         deprecated_write_register_bytes (REGISTER_BYTE (4), VALBUF, TYPE_LENGTH (TYPE)); \
// OBSOLETE       } \
// OBSOLETE     else \
// OBSOLETE       { \
// OBSOLETE         deprecated_write_register_bytes (REGISTER_BYTE (0), VALBUF, TYPE_LENGTH (TYPE)); \
// OBSOLETE       } \
// OBSOLETE   }
// OBSOLETE 
// OBSOLETE 
// OBSOLETE extern CORE_ADDR mn10200_store_struct_return (CORE_ADDR addr, CORE_ADDR sp);
// OBSOLETE #define STORE_STRUCT_RETURN(STRUCT_ADDR, SP) \
// OBSOLETE   (SP) = mn10200_store_struct_return (STRUCT_ADDR, SP)
// OBSOLETE 
// OBSOLETE extern CORE_ADDR mn10200_skip_prologue (CORE_ADDR);
// OBSOLETE #define SKIP_PROLOGUE(pc) (mn10200_skip_prologue (pc))
// OBSOLETE 
// OBSOLETE #define FRAME_ARGS_SKIP 0
// OBSOLETE 
// OBSOLETE #define FRAME_ARGS_ADDRESS(fi) (get_frame_base (fi))
// OBSOLETE #define FRAME_LOCALS_ADDRESS(fi) (get_frame_base (fi))
// OBSOLETE #define FRAME_NUM_ARGS(fi) (-1)
// OBSOLETE 
// OBSOLETE extern void mn10200_pop_frame (struct frame_info *);
// OBSOLETE #define POP_FRAME mn10200_pop_frame (get_current_frame ())
// OBSOLETE 
// OBSOLETE #define DEPRECATED_USE_GENERIC_DUMMY_FRAMES 1
// OBSOLETE #define CALL_DUMMY                   {0}
// OBSOLETE #define CALL_DUMMY_START_OFFSET      (0)
// OBSOLETE #define CALL_DUMMY_BREAKPOINT_OFFSET (0)
// OBSOLETE #define CALL_DUMMY_LOCATION          AT_ENTRY_POINT
// OBSOLETE #define FIX_CALL_DUMMY(DUMMY, START, FUNADDR, NARGS, ARGS, TYPE, GCCP)
// OBSOLETE #define CALL_DUMMY_ADDRESS()         entry_point_address ()
// OBSOLETE 
// OBSOLETE extern CORE_ADDR mn10200_push_return_address (CORE_ADDR, CORE_ADDR);
// OBSOLETE #define PUSH_RETURN_ADDRESS(PC, SP)  mn10200_push_return_address (PC, SP)
// OBSOLETE 
// OBSOLETE #define PUSH_DUMMY_FRAME	generic_push_dummy_frame ()
// OBSOLETE 
// OBSOLETE extern CORE_ADDR mn10200_push_arguments (int, struct value **, CORE_ADDR,
// OBSOLETE 					 unsigned char, CORE_ADDR);
// OBSOLETE #define PUSH_ARGUMENTS(NARGS, ARGS, SP, STRUCT_RETURN, STRUCT_ADDR) \
// OBSOLETE   (mn10200_push_arguments (NARGS, ARGS, SP, STRUCT_RETURN, STRUCT_ADDR))
// OBSOLETE 
// OBSOLETE #define DEPRECATED_PC_IN_CALL_DUMMY(PC, SP, FP) generic_pc_in_call_dummy (PC, SP, FP)
// OBSOLETE 
// OBSOLETE #define REG_STRUCT_HAS_ADDR(gcc_p,TYPE) \
// OBSOLETE   	(TYPE_LENGTH (TYPE) > 8)
// OBSOLETE 
// OBSOLETE extern use_struct_convention_fn mn10200_use_struct_convention;
// OBSOLETE #define USE_STRUCT_CONVENTION(GCC_P, TYPE) mn10200_use_struct_convention (GCC_P, TYPE)
// OBSOLETE 
// OBSOLETE /* Define this for Wingdb */
// OBSOLETE #define TARGET_MN10200
