/* Target-specific definition for the Mitsubishi D10V
   Copyright (C) 1996,1999 Free Software Foundation, Inc.

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

/* Contributed by Martin Hunt, hunt@cygnus.com */


/* #define GDB_TARGET_IS_D10V - moved to gdbarch.h */

/* Define the bit, byte, and word ordering of the machine.  */

#define TARGET_BYTE_ORDER	BIG_ENDIAN

/* Offset from address of function to start of its code.
   Zero on most machines.  */

#define FUNCTION_START_OFFSET 0

/* Advance PC across any function entry prologue instructions
   to reach some "real" code.  */

extern CORE_ADDR d10v_skip_prologue ();
#define SKIP_PROLOGUE(ip) (d10v_skip_prologue (ip))

/* Stack grows downward.  */
#define INNER_THAN(lhs,rhs) (core_addr_lessthan ((lhs), (rhs)))

/* for a breakpoint, use "dbt || nop" */
extern breakpoint_from_pc_fn d10v_breakpoint_from_pc;
#define BREAKPOINT_FROM_PC(PCPTR,LENPTR) (d10v_breakpoint_from_pc ((PCPTR), (LENPTR)))

/* If your kernel resets the pc after the trap happens you may need to
   define this before including this file.  */
#define DECR_PC_AFTER_BREAK 4

extern char *d10v_register_name PARAMS ((int reg_nr));
#define REGISTER_NAME(NR) (d10v_register_name (NR))

#define NUM_REGS 37

/* Register numbers of various important registers.
   Note that some of these values are "real" register numbers,
   and correspond to the general registers of the machine,
   and some are "phony" register numbers which are too large
   to be actual register numbers as far as the user is concerned
   but do serve to get the desired values when passed to read_register.  */

/* Used by both d10v-tdep.c and remote-d10v.c */

#define R0_REGNUM	0
#define LR_REGNUM 	13
#define SP_REGNUM 	15
#define FP_REGNUM	11
#define PC_REGNUM 	18
#define PSW_REGNUM 	16
#define IMAP0_REGNUM	32
#define IMAP1_REGNUM	33
#define DMAP_REGNUM	34
#define A0_REGNUM 	35

/* ??? */
#define REGISTER_SIZE 2

/* Say how much memory is needed to store a copy of the register set */
#define REGISTER_BYTES    ((37/*NUM_REGS*/-2)*2+16) 

/* Index within `registers' of the first byte of the space for
   register N.  */
extern int d10v_register_byte PARAMS ((int reg_nr));
#define REGISTER_BYTE(N) (d10v_register_byte (N))

/* Number of bytes of storage in the actual machine representation
   for register N.  */
extern int d10v_register_raw_size PARAMS ((int reg_nr));
#define REGISTER_RAW_SIZE(N) (d10v_register_raw_size (N))

/* Number of bytes of storage in the program's representation
   for register N.  */   
extern int d10v_register_virtual_size PARAMS ((int reg_nr));
#define REGISTER_VIRTUAL_SIZE(N) (d10v_register_virtual_size (N))

/* Largest value REGISTER_RAW_SIZE can have.  */

#define MAX_REGISTER_RAW_SIZE 8

/* Largest value REGISTER_VIRTUAL_SIZE can have.  */

#define MAX_REGISTER_VIRTUAL_SIZE 8

/* Return the GDB type object for the "standard" data type
   of data in register N.  */

extern struct type *d10v_register_virtual_type PARAMS ((int reg_nr));
#define REGISTER_VIRTUAL_TYPE(N) (d10v_register_virtual_type (N))

/* convert $pc and $sp to/from virtual addresses */
extern int d10v_register_convertible PARAMS ((int nr));
extern void d10v_register_convert_to_virtual PARAMS ((int regnum, struct type *type, char *from, char *to));
extern void d10v_register_convert_to_raw PARAMS ((struct type *type, int regnum, char *from, char *to));
#define REGISTER_CONVERTIBLE(N) (d10v_register_convertible ((N)))
#define REGISTER_CONVERT_TO_VIRTUAL(REGNUM,TYPE,FROM,TO) \
  d10v_register_convert_to_virtual ((REGNUM), (TYPE), (FROM), (TO))
#define REGISTER_CONVERT_TO_RAW(TYPE,REGNUM,FROM,TO) \
  d10v_register_convert_to_raw ((TYPE), (REGNUM), (FROM), (TO))

extern CORE_ADDR d10v_make_daddr PARAMS ((CORE_ADDR x));
#define D10V_MAKE_DADDR(x) (d10v_make_daddr (x))
extern CORE_ADDR d10v_make_iaddr PARAMS ((CORE_ADDR x));
#define D10V_MAKE_IADDR(x) (d10v_make_iaddr (x))

extern int d10v_daddr_p PARAMS ((CORE_ADDR x));
#define D10V_DADDR_P(X) (d10v_daddr_p (X))
extern int d10v_iaddr_p PARAMS ((CORE_ADDR x));
#define D10V_IADDR_P(X)  (d10v_iaddr_p (X))

extern CORE_ADDR d10v_convert_daddr_to_raw PARAMS ((CORE_ADDR x));
#define D10V_CONVERT_DADDR_TO_RAW(X) (d10v_convert_daddr_to_raw (X))
extern CORE_ADDR d10v_convert_iaddr_to_raw PARAMS ((CORE_ADDR x));
#define D10V_CONVERT_IADDR_TO_RAW(X) (d10v_convert_iaddr_to_raw (X))

/* Store the address of the place in which to copy the structure the
   subroutine will return.  This is called from call_function. 

   We store structs through a pointer passed in the first Argument
   register. */

extern void d10v_store_struct_return PARAMS ((CORE_ADDR addr, CORE_ADDR sp));
#define STORE_STRUCT_RETURN(ADDR, SP) d10v_store_struct_return ((ADDR), (SP))


/* Write into appropriate registers a function return value
   of type TYPE, given in virtual format.  

   Things always get returned in RET1_REGNUM, RET2_REGNUM, ... */

extern void d10v_store_return_value PARAMS ((struct type *type, char *valbuf));
#define STORE_RETURN_VALUE(TYPE,VALBUF) d10v_store_return_value ((TYPE), (VALBUF))


/* Extract from an array REGBUF containing the (raw) register state
   the address in which a function should return its structure value,
   as a CORE_ADDR (or an expression that can be used as one).  */

extern CORE_ADDR d10v_extract_struct_value_address PARAMS ((char *regbuf));
#define EXTRACT_STRUCT_VALUE_ADDRESS(REGBUF) (d10v_extract_struct_value_address ((REGBUF)))

/* Should we use EXTRACT_STRUCT_VALUE_ADDRESS instead of
   EXTRACT_RETURN_VALUE?  GCC_P is true if compiled with gcc
   and TYPE is the type (which is known to be struct, union or array).

   The d10v returns anything less than 8 bytes in size in
   registers. */

extern use_struct_convention_fn d10v_use_struct_convention;
#define USE_STRUCT_CONVENTION(gcc_p, type) d10v_use_struct_convention (gcc_p, type)


/* Define other aspects of the stack frame. 
   we keep a copy of the worked out return pc lying around, since it
   is a useful bit of info */

extern void d10v_init_extra_frame_info PARAMS ((int fromleaf, struct frame_info *fi));
#define INIT_EXTRA_FRAME_INFO(fromleaf, fi) \
    d10v_init_extra_frame_info(fromleaf, fi) 

/* A macro that tells us whether the function invocation represented
   by FI does not have a frame on the stack associated with it.  If it
   does not, FRAMELESS is set to 1, else 0.  */

#define FRAMELESS_FUNCTION_INVOCATION(FI) \
  (frameless_look_for_prologue (FI))

extern CORE_ADDR d10v_frame_chain PARAMS ((struct frame_info *frame));
#define FRAME_CHAIN(FRAME)       d10v_frame_chain(FRAME)
extern int d10v_frame_chain_valid PARAMS ((CORE_ADDR, struct frame_info *));
#define FRAME_CHAIN_VALID(chain, thisframe) d10v_frame_chain_valid (chain, thisframe)
extern CORE_ADDR d10v_frame_saved_pc PARAMS ((struct frame_info *fi));
#define FRAME_SAVED_PC(fi) (d10v_frame_saved_pc ((fi)))
extern CORE_ADDR d10v_frame_args_address PARAMS ((struct frame_info *fi));
#define FRAME_ARGS_ADDRESS(fi) (d10v_frame_args_address ((fi)))
extern CORE_ADDR d10v_frame_locals_address PARAMS ((struct frame_info *fi));
#define FRAME_LOCALS_ADDRESS(fi) (d10v_frame_locals_address ((fi)))

/* Immediately after a function call, return the saved pc.  We can't */
/* use frame->return_pc beause that is determined by reading R13 off the */
/*stack and that may not be written yet. */

extern CORE_ADDR d10v_saved_pc_after_call PARAMS ((struct frame_info *frame));
#define SAVED_PC_AFTER_CALL(frame) (d10v_saved_pc_after_call ((frame)))
    
/* Set VAL to the number of args passed to frame described by FI.
   Can set VAL to -1, meaning no way to tell.  */
/* We can't tell how many args there are */

#define FRAME_NUM_ARGS(fi) (-1)

/* Return number of bytes at start of arglist that are not really args.  */

#define FRAME_ARGS_SKIP 0

/* Put here the code to store, into frame_info->saved_regs, the
   addresses of the saved registers of frame described by FRAME_INFO.
   This includes special registers such as pc and fp saved in special
   ways in the stack frame.  sp is even more special: the address we
   return for it IS the sp for the next frame.  */

extern void d10v_frame_init_saved_regs PARAMS ((struct frame_info *));
#define FRAME_INIT_SAVED_REGS(frame_info)	    \
   d10v_frame_init_saved_regs(frame_info)

/* DUMMY FRAMES.  Need these to support inferior function calls.  They
   work like this on D10V: First we set a breakpoint at 0 or __start.
   Then we push all the registers onto the stack.  Then put the
   function arguments in the proper registers and set r13 to our
   breakpoint address.  Finally, the PC is set to the start of the
   function being called (no JSR/BSR insn).  When it hits the
   breakpoint, clear the break point and pop the old register contents
   off the stack.  */

extern void d10v_pop_frame PARAMS ((void));
#define POP_FRAME d10v_pop_frame ()

#define USE_GENERIC_DUMMY_FRAMES 1
#define CALL_DUMMY                   {0}
#define CALL_DUMMY_START_OFFSET      (0)
#define CALL_DUMMY_BREAKPOINT_OFFSET (0)
#define CALL_DUMMY_LOCATION          AT_ENTRY_POINT
#define FIX_CALL_DUMMY(DUMMY, START, FUNADDR, NARGS, ARGS, TYPE, GCCP)
#define CALL_DUMMY_ADDRESS()         entry_point_address ()
extern CORE_ADDR d10v_push_return_address PARAMS ((CORE_ADDR pc, CORE_ADDR sp));
#define PUSH_RETURN_ADDRESS(PC, SP)  d10v_push_return_address (PC, SP)

#define PC_IN_CALL_DUMMY(PC, SP, FP) generic_pc_in_call_dummy (PC, SP, FP)
/* #define PC_IN_CALL_DUMMY(pc, sp, frame_address) ( pc == IMEM_START + 4 ) */

#define PUSH_DUMMY_FRAME	generic_push_dummy_frame ()

/* override the default get_saved_register function with one that
   takes account of generic CALL_DUMMY frames */
#define GET_SAVED_REGISTER(raw_buffer, optimized, addrp, frame, regnum, lval) \
	generic_get_saved_register (raw_buffer, optimized, addrp, frame, regnum, lval)

#define PUSH_ARGUMENTS(nargs, args, sp, struct_return, struct_addr) \
  (d10v_push_arguments((nargs), (args), (sp), (struct_return), (struct_addr)))
extern CORE_ADDR d10v_push_arguments PARAMS ((int, struct value **, CORE_ADDR, int, CORE_ADDR));


/* Extract from an array REGBUF containing the (raw) register state
   a function return value of type TYPE, and copy that, in virtual format,
   into VALBUF.  */

#define EXTRACT_RETURN_VALUE(TYPE,REGBUF,VALBUF) \
d10v_extract_return_value(TYPE, REGBUF, VALBUF)
extern void d10v_extract_return_value PARAMS ((struct type *, char *, char *));


void d10v_write_pc PARAMS ((CORE_ADDR val, int pid));
CORE_ADDR d10v_read_pc PARAMS ((int pid));
void d10v_write_sp PARAMS ((CORE_ADDR val));
CORE_ADDR d10v_read_sp PARAMS ((void));
void d10v_write_fp PARAMS ((CORE_ADDR val));
CORE_ADDR d10v_read_fp PARAMS ((void));

#define TARGET_READ_PC(pid)		d10v_read_pc (pid)
#define TARGET_WRITE_PC(val,pid)	d10v_write_pc (val, pid)
#define TARGET_READ_FP()		d10v_read_fp ()
#define TARGET_WRITE_FP(val)		d10v_write_fp (val)
#define TARGET_READ_SP()		d10v_read_sp ()
#define TARGET_WRITE_SP(val)		d10v_write_sp (val)

/* Number of bits in the appropriate type */
#define TARGET_INT_BIT (2 * TARGET_CHAR_BIT)
#define TARGET_PTR_BIT (4 * TARGET_CHAR_BIT)
#define TARGET_DOUBLE_BIT (4 * TARGET_CHAR_BIT)
#define TARGET_LONG_DOUBLE_BIT (8 * TARGET_CHAR_BIT)


/* For the d10v when talking to the remote d10v board, GDB addresses
   need to be translated into a format that the d10v rom monitor
   understands. */

extern void remote_d10v_translate_xfer_address PARAMS ((CORE_ADDR gdb_addr, int gdb_len, CORE_ADDR *rem_addr, int *rem_len));
#define REMOTE_TRANSLATE_XFER_ADDRESS(GDB_ADDR, GDB_LEN, REM_ADDR, REM_LEN) \
 remote_d10v_translate_xfer_address ((GDB_ADDR), (GDB_LEN), (REM_ADDR), (REM_LEN))

