/* Parameters for execution on a Matsushita mn10200 processor.
   Copyright 1997 Free Software Foundation, Inc. 

   Contributed by Geoffrey Noer <noer@cygnus.com>

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

#define TARGET_BYTE_ORDER LITTLE_ENDIAN

#define REGISTER_SIZE 3
#define MAX_REGISTER_RAW_SIZE REGISTER_SIZE
#define NUM_REGS 12

#define REGISTER_BYTES (NUM_REGS * REGISTER_SIZE)

#define REGISTER_NAMES \
{ "d0", "d1", "d2", "d3", "a0", "a1", "a2", "a3", \
  "sp", "pc", "mdr", "psw"}

#define FP_REGNUM 6
#define SP_REGNUM 7
#define PC_REGNUM 9
#define MDR_REGNUM 10
#define PSW_REGNUM 11
#define LIR_REGNUM 12
#define LAR_REGNUM 13

#define REGISTER_VIRTUAL_TYPE(REG) builtin_type_int

#define REGISTER_BYTE(REG) ((REG) * REGISTER_SIZE)
#define REGISTER_VIRTUAL_SIZE(REG) REGISTER_SIZE
#define REGISTER_RAW_SIZE(REG) REGISTER_SIZE

#define MAX_REGISTER_VIRTUAL_SIZE REGISTER_SIZE

#define BREAKPOINT {0xff}

#define FUNCTION_START_OFFSET 0

#define DECR_PC_AFTER_BREAK 0

#define INNER_THAN <

#define SAVED_PC_AFTER_CALL(frame) \
  read_memory_integer (read_register (SP_REGNUM), REGISTER_SIZE)

#ifdef __STDC__
struct frame_info;
struct frame_saved_regs;
struct type;
struct value;
#endif

#define EXTRA_FRAME_INFO struct frame_saved_regs fsr;

extern void mn10200_init_extra_frame_info PARAMS ((struct frame_info *fi));
#define INIT_EXTRA_FRAME_INFO(fromleaf, fi) mn10200_init_extra_frame_info (fi)
#define INIT_FRAME_PC		/* Not necessary */

extern void mn10200_frame_find_saved_regs PARAMS ((struct frame_info *fi, struct frame_saved_regs *regaddr));
#define FRAME_FIND_SAVED_REGS(fi, regaddr) regaddr = fi->fsr

extern CORE_ADDR mn10200_frame_chain PARAMS ((struct frame_info *fi));
#define FRAME_CHAIN(fi) mn10200_frame_chain (fi)
#define FRAME_CHAIN_VALID(FP, FI)	generic_frame_chain_valid (FP, FI)

extern CORE_ADDR mn10200_find_callers_reg PARAMS ((struct frame_info *fi, int regnum));
extern CORE_ADDR mn10200_frame_saved_pc   PARAMS ((struct frame_info *));
#define FRAME_SAVED_PC(FI) (mn10200_frame_saved_pc (FI))

/* Extract from an array REGBUF containing the (raw) register state
   a function return value of type TYPE, and copy that, in virtual format,
   into VALBUF. */

#define EXTRACT_RETURN_VALUE(TYPE, REGBUF, VALBUF) \
  memcpy (VALBUF, REGBUF + REGISTER_BYTE (0), TYPE_LENGTH (TYPE))

#define EXTRACT_STRUCT_VALUE_ADDRESS(REGBUF) \
  extract_address (REGBUF + REGISTER_BYTE (0), \
		   REGISTER_RAW_SIZE (0))

#define STORE_RETURN_VALUE(TYPE, VALBUF) \
  write_register_bytes(REGISTER_BYTE (0), VALBUF, TYPE_LENGTH (TYPE));

extern CORE_ADDR mn10200_skip_prologue PARAMS ((CORE_ADDR pc));
#define SKIP_PROLOGUE(pc) pc = mn10200_skip_prologue (pc)

#define FRAME_ARGS_SKIP 0

#define FRAME_ARGS_ADDRESS(fi) ((fi)->frame)
#define FRAME_LOCALS_ADDRESS(fi) ((fi)->frame)
#define FRAME_NUM_ARGS(val, fi) ((val) = -1)

extern void mn10200_pop_frame PARAMS ((struct frame_info *frame));
#define POP_FRAME mn10200_pop_frame (get_current_frame ())

#define USE_GENERIC_DUMMY_FRAMES
#define CALL_DUMMY                   {0}
#undef CALL_DUMMY /* for now, no function calls */
#define CALL_DUMMY_START_OFFSET      (0)
#define CALL_DUMMY_BREAKPOINT_OFFSET (0)
#define CALL_DUMMY_LOCATION          AT_ENTRY_POINT
#define FIX_CALL_DUMMY(DUMMY, START, FUNADDR, NARGS, ARGS, TYPE, GCCP)
#define CALL_DUMMY_ADDRESS()         entry_point_address ()

extern CORE_ADDR mn10200_push_return_address PARAMS ((CORE_ADDR, CORE_ADDR));
#define PUSH_RETURN_ADDRESS(PC, SP)  mn10200_push_return_address (PC, SP)

#define PUSH_DUMMY_FRAME	generic_push_dummy_frame ()

extern CORE_ADDR
mn10200_push_arguments PARAMS ((int nargs, struct value **args, CORE_ADDR sp,
			     unsigned char struct_return,
			     CORE_ADDR struct_addr));
#define PUSH_ARGUMENTS(NARGS, ARGS, SP, STRUCT_RETURN, STRUCT_ADDR) \
  (SP) = mn10200_push_arguments (NARGS, ARGS, SP, STRUCT_RETURN, STRUCT_ADDR)

#define STORE_STRUCT_RETURN(STRUCT_ADDR, SP)

#define PC_IN_CALL_DUMMY(PC, SP, FP) generic_pc_in_call_dummy (PC, SP)

#define USE_STRUCT_CONVENTION(GCC_P, TYPE) \
  	(TYPE_NFIELDS (TYPE) > 1 || TYPE_LENGTH (TYPE) > 4)

/* override the default get_saved_register function with
   one that takes account of generic CALL_DUMMY frames */
#define GET_SAVED_REGISTER

/* Define this for Wingdb */

#define TARGET_MN10200
