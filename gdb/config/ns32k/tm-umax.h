/* Definitions to make GDB run on an encore under umax 4.2
   Copyright 1987, 1989, 1991, 1993, 1994, 1998, 1999, 2000, 2001, 2002
   Free Software Foundation, Inc.

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

#include "regcache.h"

/* Need to get function ends by adding this to epilogue address from .bf
   record, not using x_fsize field.  */
#define FUNCTION_EPILOGUE_SIZE 4

/* Offset from address of function to start of its code.
   Zero on most machines.  */

#define FUNCTION_START_OFFSET 0

/* Advance PC across any function entry prologue instructions
   to reach some "real" code.  */

extern CORE_ADDR umax_skip_prologue (CORE_ADDR);
#define SKIP_PROLOGUE(pc) (umax_skip_prologue (pc))

/* Immediately after a function call, return the saved pc.
   Can't always go through the frames for this because on some machines
   the new frame is not set up until the new function executes
   some instructions.  */

#define SAVED_PC_AFTER_CALL(frame) \
	read_memory_integer (read_register (SP_REGNUM), 4)

/* Address of end of stack space.  */

#ifndef STACK_END_ADDR
#define STACK_END_ADDR (0xfffff000)
#endif

/* Stack grows downward.  */

#define INNER_THAN(lhs,rhs) core_addr_lessthan ((lhs), (rhs))

extern const unsigned char *ns32k_breakpoint_from_pc (CORE_ADDR *, int *);
#define BREAKPOINT_FROM_PC(PCP, LENP) ns32k_breakpoint_from_pc ((PCP), (LENP))

/* Amount PC must be decremented by after a breakpoint.
   This is often the number of bytes in BREAKPOINT
   but not always.  */

#define DECR_PC_AFTER_BREAK 0

/* Say how long (ordinary) registers are.  This is a piece of bogosity
   used in push_word and a few other places; REGISTER_RAW_SIZE is the
   real way to know how big a register is.  */

#define REGISTER_SIZE 4

/* Number of machine registers */

#define NUM_REGS		25

#define NUM_GENERAL_REGS	8

extern char *ns32k_register_name_32082 (int);
extern char *ns32k_register_name_32382 (int);

#define REGISTER_NAME(REGNUM) ns32k_register_name_32082(REGNUM)

/* Register numbers of various important registers.
   Note that some of these values are "real" register numbers,
   and correspond to the general registers of the machine,
   and some are "phony" register numbers which are too large
   to be actual register numbers as far as the user is concerned
   but do serve to get the desired values when passed to read_register.  */

#define	R0_REGNUM 0		/* General register 0 */
#define FP0_REGNUM 8		/* Floating point register 0 */
#define SP_REGNUM 16		/* Contains address of top of stack */
#define AP_REGNUM FP_REGNUM
#define FP_REGNUM 17		/* Contains address of executing stack frame */
#define PC_REGNUM 18		/* Contains program counter */
#define PS_REGNUM 19		/* Contains processor status */
#define FPS_REGNUM 20		/* Floating point status register */
#define LP0_REGNUM 21		/* Double register 0 (same as FP0) */

/* Total amount of space needed to store our copies of the machine's
   register state, the array `registers'.  */
#define REGISTER_BYTES \
  ((NUM_REGS - 4) * REGISTER_RAW_SIZE(R0_REGNUM) \
   + 4            * REGISTER_RAW_SIZE(LP0_REGNUM))

/* Index within `registers' of the first byte of the space for
   register N.  */
extern int ns32k_register_byte_32082 (int);
extern int ns32k_register_byte_32382 (int);
#define REGISTER_BYTE(N) ns32k_register_byte_32082 ((N))

extern int ns32k_register_raw_size (int);
#define REGISTER_RAW_SIZE(N) ns32k_register_raw_size ((N))

extern int ns32k_register_virtual_size (int);
#define REGISTER_VIRTUAL_SIZE(N) ns32k_register_virtual_size ((N))

/* Largest value REGISTER_RAW_SIZE can have.  */

#define MAX_REGISTER_RAW_SIZE 8

/* Largest value REGISTER_VIRTUAL_SIZE can have.  */

#define MAX_REGISTER_VIRTUAL_SIZE 8

struct type *ns32k_register_virtual_type (int);
#define REGISTER_VIRTUAL_TYPE(N) ns32k_register_virtual_type ((N))

/* Store the address of the place in which to copy the structure the
   subroutine will return.  This is called from call_function.

   On this machine this is a no-op, because gcc isn't used on it
   yet.  So this calling convention is not used. */

#define STORE_STRUCT_RETURN(ADDR, SP)

/* Extract from an array REGBUF containing the (raw) register state
   a function return value of type TYPE, and copy that, in virtual format,
   into VALBUF.  */

#define EXTRACT_RETURN_VALUE(TYPE,REGBUF,VALBUF) \
  memcpy (VALBUF, REGBUF+REGISTER_BYTE (TYPE_CODE (TYPE) == TYPE_CODE_FLT ? FP0_REGNUM : 0), TYPE_LENGTH (TYPE))

/* Write into appropriate registers a function return value
   of type TYPE, given in virtual format.  */

#define STORE_RETURN_VALUE(TYPE,VALBUF) \
  write_register_bytes (REGISTER_BYTE (TYPE_CODE (TYPE) == TYPE_CODE_FLT ? FP0_REGNUM : 0), VALBUF, TYPE_LENGTH (TYPE))

/* Extract from an array REGBUF containing the (raw) register state
   the address in which a function should return its structure value,
   as a CORE_ADDR (or an expression that can be used as one).  */

#define EXTRACT_STRUCT_VALUE_ADDRESS(REGBUF) (*(int *)(REGBUF))

/* Describe the pointer in each stack frame to the previous stack frame
   (its caller).  */

CORE_ADDR ns32k_frame_chain (struct frame_info *);
#define FRAME_CHAIN(thisframe) ns32k_frame_chain ((thisframe))

CORE_ADDR ns32k_frame_saved_pc (struct frame_info *);
#define FRAME_SAVED_PC(FRAME) ns32k_frame_saved_pc ((FRAME))

CORE_ADDR ns32k_frame_args_address (struct frame_info *);
#define FRAME_ARGS_ADDRESS(fi) ns32k_frame_args_address ((fi))

CORE_ADDR ns32k_frame_locals_address (struct frame_info *);
#define FRAME_LOCALS_ADDRESS(fi) ns32k_frame_locals_address ((fi))

/* Return number of args passed to a frame.
   Can return -1, meaning no way to tell.  */

extern int umax_frame_num_args (struct frame_info *);
#define FRAME_NUM_ARGS(fi) (umax_frame_num_args ((fi)))

/* Return number of bytes at start of arglist that are not really args.  */

#define FRAME_ARGS_SKIP 8

void ns32k_frame_init_saved_regs (struct frame_info *);
#define FRAME_INIT_SAVED_REGS(FI) ns32k_frame_init_saved_regs ((FI))


/* Things needed for making the inferior call functions.  */

extern void ns32k_push_dummy_frame (void);
#define PUSH_DUMMY_FRAME ns32k_push_dummy_frame ()

/* Discard from the stack the innermost frame, restoring all registers.  */

extern void ns32k_pop_frame (void);
#define POP_FRAME ns32k_pop_frame ()

/* This sequence of words is the instructions
   enter        0xff,0          82 ff 00
   jsr  @0x00010203     7f ae c0 01 02 03
   adjspd       0x69696969      7f a5 01 02 03 04
   bpt                  f2
   Note this is 16 bytes.  */

#define CALL_DUMMY { 0x7f00ff82, 0x0201c0ae, 0x01a57f03, 0xf2040302 }

#define CALL_DUMMY_START_OFFSET	3
#define CALL_DUMMY_LENGTH	16
#define CALL_DUMMY_ADDR		5
#define CALL_DUMMY_NARGS	11

/* Insert the specified number of args and function address
   into a call sequence of the above form stored at DUMMYNAME.  */

void flip_bytes (void *ptr, int count);

#define FIX_CALL_DUMMY(dummyname, pc, fun, nargs, args, type, gcc_p)   		\
{								\
	int	flipped;					\
	flipped = fun | 0xc0000000;				\
	flip_bytes (&flipped, 4);				\
	*((int *) (((char *) dummyname)+CALL_DUMMY_ADDR)) = flipped;	\
	flipped = - nargs * 4;					\
	flip_bytes (&flipped, 4);				\
	*((int *) (((char *) dummyname)+CALL_DUMMY_NARGS)) = flipped;	\
}
