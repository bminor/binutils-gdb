// OBSOLETE /* Parameters for execution on a H8/500 series machine.
// OBSOLETE 
// OBSOLETE    Copyright 1993, 1994, 1995, 1998, 1999, 2000, 2001, 2002, 2003 Free
// OBSOLETE    Software Foundation, Inc.
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
// OBSOLETE #include "regcache.h"
// OBSOLETE 
// OBSOLETE /* NOTE: cagney/2002-11-24: This is a guess.  */
// OBSOLETE #define DEPRECATED_USE_GENERIC_DUMMY_FRAMES 0
// OBSOLETE #define CALL_DUMMY_LOCATION ON_STACK
// OBSOLETE #define DEPRECATED_PC_IN_CALL_DUMMY(pc, sp, frame_address) deprecated_pc_in_call_dummy_on_stack (pc, sp, frame_address)
// OBSOLETE 
// OBSOLETE /* Contributed by Steve Chamberlain sac@cygnus.com */
// OBSOLETE 
// OBSOLETE #define GDB_TARGET_IS_H8500
// OBSOLETE 
// OBSOLETE /* Define the sizes of integers and pointers.  */
// OBSOLETE 
// OBSOLETE #define TARGET_INT_BIT 16
// OBSOLETE 
// OBSOLETE #define TARGET_LONG_BIT 32
// OBSOLETE 
// OBSOLETE #define TARGET_PTR_BIT (minimum_mode ? 16 : 32)
// OBSOLETE 
// OBSOLETE /* Offset from address of function to start of its code.
// OBSOLETE    Zero on most machines.  */
// OBSOLETE 
// OBSOLETE #define FUNCTION_START_OFFSET 0
// OBSOLETE 
// OBSOLETE /* Advance PC across any function entry prologue instructions
// OBSOLETE    to reach some "real" code.  */
// OBSOLETE 
// OBSOLETE #define SKIP_PROLOGUE(ip)   (h8500_skip_prologue (ip))
// OBSOLETE extern CORE_ADDR h8500_skip_prologue (CORE_ADDR);
// OBSOLETE 
// OBSOLETE /* Immediately after a function call, return the saved pc.
// OBSOLETE    Can't always go through the frames for this because on some machines
// OBSOLETE    the new frame is not set up until the new function executes
// OBSOLETE    some instructions.  */
// OBSOLETE 
// OBSOLETE #define SAVED_PC_AFTER_CALL(frame) saved_pc_after_call()
// OBSOLETE extern CORE_ADDR saved_pc_after_call (void);
// OBSOLETE 
// OBSOLETE /* Stack grows downward.  */
// OBSOLETE 
// OBSOLETE #define INNER_THAN(lhs,rhs) ((lhs) < (rhs))
// OBSOLETE 
// OBSOLETE /* Illegal instruction - used by the simulator for breakpoint
// OBSOLETE    detection */
// OBSOLETE 
// OBSOLETE #define BREAKPOINT {0x0b}
// OBSOLETE 
// OBSOLETE /* If your kernel resets the pc after the trap happens you may need to
// OBSOLETE    define this before including this file.  */
// OBSOLETE 
// OBSOLETE #define DECR_PC_AFTER_BREAK 0
// OBSOLETE 
// OBSOLETE /* Say how long registers are.  */
// OBSOLETE 
// OBSOLETE #define REGISTER_TYPE  unsigned long
// OBSOLETE 
// OBSOLETE /* Say how much memory is needed to store a copy of the register set */
// OBSOLETE 
// OBSOLETE #define REGISTER_BYTES    (NUM_REGS * 4)
// OBSOLETE 
// OBSOLETE /* Index within `registers' of the first byte of the space for
// OBSOLETE    register N.  */
// OBSOLETE 
// OBSOLETE #define REGISTER_BYTE(N)  ((N)*4)
// OBSOLETE 
// OBSOLETE /* Number of bytes of storage in the actual machine representation
// OBSOLETE    for register N.  */
// OBSOLETE 
// OBSOLETE #define REGISTER_RAW_SIZE(N) h8500_register_size(N)
// OBSOLETE extern int h8500_register_size (int regno);
// OBSOLETE 
// OBSOLETE #define REGISTER_SIZE 4
// OBSOLETE 
// OBSOLETE #define REGISTER_VIRTUAL_SIZE(N) h8500_register_size(N)
// OBSOLETE 
// OBSOLETE /* Largest value REGISTER_RAW_SIZE can have.  */
// OBSOLETE 
// OBSOLETE #define MAX_REGISTER_RAW_SIZE 4
// OBSOLETE 
// OBSOLETE /* Largest value REGISTER_VIRTUAL_SIZE can have.  */
// OBSOLETE 
// OBSOLETE #define MAX_REGISTER_VIRTUAL_SIZE 4
// OBSOLETE 
// OBSOLETE /* Return the GDB type object for the "standard" data type
// OBSOLETE    of data in register N.  */
// OBSOLETE 
// OBSOLETE #define REGISTER_VIRTUAL_TYPE(N) h8500_register_virtual_type(N)
// OBSOLETE extern struct type *h8500_register_virtual_type (int regno);
// OBSOLETE 
// OBSOLETE /* Initializer for an array of names of registers.
// OBSOLETE    Entries beyond the first NUM_REGS are ignored.  */
// OBSOLETE 
// OBSOLETE #define REGISTER_NAMES \
// OBSOLETE   { "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", \
// OBSOLETE    "pr0","pr1","pr2","pr3","pr4","pr5","pr6","pr7", \
// OBSOLETE     "cp", "dp", "ep", "tp", "sr", "pc"}
// OBSOLETE 
// OBSOLETE /* Register numbers of various important registers.  Note that some of
// OBSOLETE    these values are "real" register numbers, and correspond to the
// OBSOLETE    general registers of the machine, and some are "phony" register
// OBSOLETE    numbers which are too large to be actual register numbers as far as
// OBSOLETE    the user is concerned but do serve to get the desired values when
// OBSOLETE    passed to read_register.  */
// OBSOLETE 
// OBSOLETE #define R0_REGNUM	0
// OBSOLETE #define R1_REGNUM	1
// OBSOLETE #define R2_REGNUM	2
// OBSOLETE #define R3_REGNUM	3
// OBSOLETE #define R4_REGNUM	4
// OBSOLETE #define R5_REGNUM	5
// OBSOLETE #define R6_REGNUM	6
// OBSOLETE #define R7_REGNUM	7
// OBSOLETE 
// OBSOLETE #define PR0_REGNUM	8
// OBSOLETE #define PR1_REGNUM	9
// OBSOLETE #define PR2_REGNUM	10
// OBSOLETE #define PR3_REGNUM	11
// OBSOLETE #define PR4_REGNUM	12
// OBSOLETE #define PR5_REGNUM	13
// OBSOLETE #define PR6_REGNUM	14
// OBSOLETE #define PR7_REGNUM	15
// OBSOLETE 
// OBSOLETE #define SEG_C_REGNUM	16	/* Segment registers */
// OBSOLETE #define SEG_D_REGNUM	17
// OBSOLETE #define SEG_E_REGNUM	18
// OBSOLETE #define SEG_T_REGNUM	19
// OBSOLETE 
// OBSOLETE #define CCR_REGNUM      20	/* Contains processor status */
// OBSOLETE #define PC_REGNUM       21	/* Contains program counter */
// OBSOLETE 
// OBSOLETE #define NUM_REGS 	22
// OBSOLETE 
// OBSOLETE #define SP_REGNUM       PR7_REGNUM	/* Contains address of top of stack */
// OBSOLETE #define FP_REGNUM       PR6_REGNUM	/* Contains address of executing stack frame */
// OBSOLETE 
// OBSOLETE #define PTR_SIZE (minimum_mode ? 2 : 4)
// OBSOLETE #define PTR_MASK (minimum_mode ? 0x0000ffff : 0x00ffffff)
// OBSOLETE 
// OBSOLETE /* Store the address of the place in which to copy the structure the
// OBSOLETE    subroutine will return.  This is called from call_function. */
// OBSOLETE 
// OBSOLETE /*#define STORE_STRUCT_RETURN(ADDR, SP) \
// OBSOLETE    { write_register (0, (ADDR)); internal_error (__FILE__, __LINE__, "failed internal consistency check"); } */
// OBSOLETE 
// OBSOLETE /* Extract from an array REGBUF containing the (raw) register state
// OBSOLETE    a function return value of type TYPE, and copy that, in virtual format,
// OBSOLETE    into VALBUF.  */
// OBSOLETE 
// OBSOLETE #define DEPRECATED_EXTRACT_RETURN_VALUE(TYPE,REGBUF,VALBUF) \
// OBSOLETE   memcpy (VALBUF, (char *)(REGBUF), TYPE_LENGTH(TYPE))
// OBSOLETE 
// OBSOLETE /* Write into appropriate registers a function return value
// OBSOLETE    of type TYPE, given in virtual format.  */
// OBSOLETE 
// OBSOLETE #define DEPRECATED_STORE_RETURN_VALUE(TYPE,VALBUF) \
// OBSOLETE   deprecated_write_register_bytes (0, VALBUF, TYPE_LENGTH (TYPE))
// OBSOLETE 
// OBSOLETE /* Extract from an array REGBUF containing the (raw) register state
// OBSOLETE    the address in which a function should return its structure value,
// OBSOLETE    as a CORE_ADDR (or an expression that can be used as one).  */
// OBSOLETE 
// OBSOLETE #define DEPRECATED_EXTRACT_STRUCT_VALUE_ADDRESS(REGBUF) (*(CORE_ADDR *)(REGBUF))
// OBSOLETE 
// OBSOLETE 
// OBSOLETE /* Define other aspects of the stack frame.  */
// OBSOLETE 
// OBSOLETE /* A macro that tells us whether the function invocation represented
// OBSOLETE    by FI does not have a frame on the stack associated with it.  If it
// OBSOLETE    does not, FRAMELESS is set to 1, else 0.  */
// OBSOLETE 
// OBSOLETE #define FRAMELESS_FUNCTION_INVOCATION(FI) \
// OBSOLETE   (frameless_look_for_prologue (FI))
// OBSOLETE 
// OBSOLETE /* Any function with a frame looks like this
// OBSOLETE    SECOND ARG
// OBSOLETE    FIRST ARG
// OBSOLETE    RET PC
// OBSOLETE    SAVED R2
// OBSOLETE    SAVED R3
// OBSOLETE    SAVED FP   <-FP POINTS HERE
// OBSOLETE    LOCALS0
// OBSOLETE    LOCALS1    <-SP POINTS HERE
// OBSOLETE 
// OBSOLETE  */
// OBSOLETE 
// OBSOLETE #define INIT_EXTRA_FRAME_INFO(fromleaf, fci)  ;
// OBSOLETE /*       (fci)->frame |= read_register(SEG_T_REGNUM) << 16; */
// OBSOLETE 
// OBSOLETE #define FRAME_CHAIN(FRAME) h8500_frame_chain(FRAME)
// OBSOLETE struct frame_info;
// OBSOLETE extern CORE_ADDR h8500_frame_chain (struct frame_info *);
// OBSOLETE 
// OBSOLETE #define FRAME_SAVED_PC(FRAME) frame_saved_pc(FRAME)
// OBSOLETE extern CORE_ADDR frame_saved_pc (struct frame_info *frame);
// OBSOLETE 
// OBSOLETE #define FRAME_ARGS_ADDRESS(fi) (get_frame_base (fi))
// OBSOLETE 
// OBSOLETE #define FRAME_LOCALS_ADDRESS(fi) (get_frame_base (fi))
// OBSOLETE 
// OBSOLETE /* Set VAL to the number of args passed to frame described by FI.
// OBSOLETE    Can set VAL to -1, meaning no way to tell.  */
// OBSOLETE 
// OBSOLETE /* We can't tell how many args there are
// OBSOLETE    now that the C compiler delays popping them.  */
// OBSOLETE 
// OBSOLETE #define FRAME_NUM_ARGS(fi) (-1)
// OBSOLETE 
// OBSOLETE /* Return number of bytes at start of arglist that are not really args.  */
// OBSOLETE 
// OBSOLETE #define FRAME_ARGS_SKIP 0
// OBSOLETE 
// OBSOLETE /* Put here the code to store, into a struct frame_saved_regs,
// OBSOLETE    the addresses of the saved registers of frame described by FRAME_INFO.
// OBSOLETE    This includes special registers such as pc and fp saved in special
// OBSOLETE    ways in the stack frame.  sp is even more special:
// OBSOLETE    the address we return for it IS the sp for the next frame.  */
// OBSOLETE 
// OBSOLETE #define FRAME_FIND_SAVED_REGS(frame_info, frame_saved_regs)	    \
// OBSOLETE    frame_find_saved_regs(frame_info, &(frame_saved_regs))
// OBSOLETE struct frame_saved_regs;
// OBSOLETE extern void frame_find_saved_regs (struct frame_info *frame_info,
// OBSOLETE 				   struct frame_saved_regs *frame_saved_regs);
// OBSOLETE 
// OBSOLETE 
// OBSOLETE /* Discard from the stack the innermost frame, restoring all registers.  */
// OBSOLETE 
// OBSOLETE #define POP_FRAME { h8500_pop_frame (); }
// OBSOLETE extern void h8500_pop_frame (void);
// OBSOLETE 
// OBSOLETE #define SHORT_INT_MAX 32767
// OBSOLETE #define SHORT_INT_MIN -32768
// OBSOLETE 
// OBSOLETE typedef unsigned short INSN_WORD;
// OBSOLETE 
// OBSOLETE extern CORE_ADDR h8500_addr_bits_remove (CORE_ADDR);
// OBSOLETE #define ADDR_BITS_REMOVE(addr) h8500_addr_bits_remove (addr)
// OBSOLETE 
// OBSOLETE #define read_memory_short(x)  (read_memory_integer(x,2) & 0xffff)
// OBSOLETE 
// OBSOLETE extern void h8500_do_registers_info (int regnum, int all);
// OBSOLETE #define DEPRECATED_DO_REGISTERS_INFO(REGNUM,ALL) \
// OBSOLETE 	h8500_do_registers_info (REGNUM, ALL)
// OBSOLETE 
// OBSOLETE extern int minimum_mode;
// OBSOLETE 
// OBSOLETE #define CALL_DUMMY_LENGTH 10
// OBSOLETE 
// OBSOLETE /* Fake variables to make it easy to use 24 bit register pointers */
// OBSOLETE 
// OBSOLETE #define IS_TRAPPED_INTERNALVAR h8500_is_trapped_internalvar
// OBSOLETE extern int h8500_is_trapped_internalvar (char *name);
// OBSOLETE 
// OBSOLETE #define VALUE_OF_TRAPPED_INTERNALVAR h8500_value_of_trapped_internalvar
// OBSOLETE extern struct value *h8500_value_of_trapped_internalvar ( /* struct internalvar *var */ );
// OBSOLETE 
// OBSOLETE #define SET_TRAPPED_INTERNALVAR h8500_set_trapped_internalvar
// OBSOLETE extern void h8500_set_trapped_internalvar ( /* struct internalvar *var, value newval, int bitpos, int bitsize, int offset */ );
// OBSOLETE 
// OBSOLETE extern CORE_ADDR h8500_read_sp (void);
// OBSOLETE extern void h8500_write_sp (CORE_ADDR);
// OBSOLETE 
// OBSOLETE extern CORE_ADDR h8500_read_fp (void);
// OBSOLETE 
// OBSOLETE extern CORE_ADDR h8500_read_pc (ptid_t);
// OBSOLETE extern void h8500_write_pc (CORE_ADDR, ptid_t);
// OBSOLETE 
// OBSOLETE #define TARGET_READ_SP() h8500_read_sp()
// OBSOLETE #define TARGET_WRITE_SP(x) h8500_write_sp(x)
// OBSOLETE 
// OBSOLETE #define TARGET_READ_PC(pid) h8500_read_pc(pid)
// OBSOLETE #define TARGET_WRITE_PC(x,pid) h8500_write_pc(x,pid)
// OBSOLETE 
// OBSOLETE #define TARGET_READ_FP() h8500_read_fp()
