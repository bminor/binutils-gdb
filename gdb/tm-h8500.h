/* Parameters for execution on a H8/500 series machine.
   Copyright (C) 1993 Free Software Foundation, Inc.

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
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* Contributed by Steve Chamberlain sac@cygnus.com */



#define IEEE_FLOAT 1

/* Define the bit, byte, and word ordering of the machine.  */

#define TARGET_BYTE_ORDER BIG_ENDIAN

#undef TARGET_INT_BIT
#define TARGET_INT_BIT 16

#undef TARGET_PTR_BIT
#define TARGET_PTR_BIT (minimum_mode ? 16 : 32)


/* Offset from address of function to start of its code.
   Zero on most machines.  */

#define FUNCTION_START_OFFSET 0

/* Advance PC across any function entry prologue instructions
   to reach some "real" code.  */

#define SKIP_PROLOGUE(ip)   {(ip) = h8500_skip_prologue(ip);}
extern CORE_ADDR h8500_skip_prologue ();


/* Immediately after a function call, return the saved pc.
   Can't always go through the frames for this because on some machines
   the new frame is not set up until the new function executes
   some instructions.  */

#define SAVED_PC_AFTER_CALL(frame) saved_pc_after_call(frame)


/* Stack grows downward.  */

#define INNER_THAN <

/* Illegal instruction - used by the simulator for breakpoint
   detection */
#define BREAKPOINT {0x0b}


/* If your kernel resets the pc after the trap happens you may need to
   define this before including this file.  */

#define DECR_PC_AFTER_BREAK 0


/* Nonzero if instruction at PC is a return instruction.  */

#define ABOUT_TO_RETURN(pc) about_to_return(pc)

/* Return 1 if P points to an invalid floating point value.  */

#define INVALID_FLOAT(p, len) 0   /* Just a first guess; not checked */

/* Say how long registers are.  */

#define REGISTER_TYPE  unsigned long

/* Say how much memory is needed to store a copy of the register set */

#define REGISTER_BYTES    ((NUM_REGS)*4) 

/* Index within `registers' of the first byte of the space for
   register N.  */

#define REGISTER_BYTE(N)  ((N)*4)

/* Number of bytes of storage in the actual machine representation
   for register N.  */

#define REGISTER_RAW_SIZE(N)  register_raw_size(N)

#define REGISTER_VIRTUAL_SIZE(N) register_virtual_size(N)

/* Largest value REGISTER_RAW_SIZE can have.  */

#define MAX_REGISTER_RAW_SIZE 4

/* Largest value REGISTER_VIRTUAL_SIZE can have.  */

#define MAX_REGISTER_VIRTUAL_SIZE 4

/* Nonzero if register N requires conversion
   from raw format to virtual format.  */

#define REGISTER_CONVERTIBLE(N) 1

/* Convert data from raw format for register REGNUM
   to virtual format for register REGNUM.  */

#define REGISTER_CONVERT_TO_VIRTUAL(REGNUM,FROM,TO)  \
     register_convert_to_virtual(REGNUM, FROM, TO)

/* Convert data from virtual format for register REGNUM
   to raw format for register REGNUM.  */

#define REGISTER_CONVERT_TO_RAW(REGNUM,FROM,TO)   \
   register_convert_to_raw(REGNUM, FROM, TO)

/* Return the GDB type object for the "standard" data type
   of data in register N.  */
struct type *register_virtual_type();

#define REGISTER_VIRTUAL_TYPE(N) register_virtual_type(N)


/* Initializer for an array of names of registers.
   Entries beyond the first NUM_REGS are ignored.  */

#define REGISTER_NAMES \
  {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", \
   "pr0", "pr1","pr2","pr3","pr4","pr5","pr6","pr7", \
   "ccr","pc", \
    "cp","dp","ep","tp" }

/* Register numbers of various important registers.
   Note that some of these values are "real" register numbers,
   and correspond to the general registers of the machine,
   and some are "phony" register numbers which are too large
   to be actual register numbers as far as the user is concerned
   but do serve to get the desired values when passed to read_register.  */


#define R0  		0
#define R1  		1
#define R2  		2
#define R3  		3
#define R4  		4
#define R5  		5
#define R6  		6
#define R7  		7

#define PR0  		8      /* R0-R7 with seg prefixed */
#define PR1  		9
#define PR2  		10
#define PR3  		11
#define PR4  		12
#define PR5  		13
#define PR6  		14
#define PR7  		15

#define SP_REGNUM       PR7	/* Contains address of top of stack */
#define FP_REGNUM       PR6	/* Contains address of executing stack frame */

#define CCR_REGNUM      16	/* Contains processor status */
#define PC_REGNUM       17	/* Contains program counter */

#define SEG_C  		18	/* Segment registers */
#define SEG_D  		19
#define SEG_E  		20
#define SEG_T  		21

#define NUM_REGS 	22

#define PTR_SIZE (minimum_mode ? 2: 4)
#define PTR_MASK (minimum_mode ? 0x0000ffff : 0x00ffffff)

/* Store the address of the place in which to copy the structure the
   subroutine will return.  This is called from call_function. */

/*#define STORE_STRUCT_RETURN(ADDR, SP) \
  { write_register (0, (ADDR)); abort(); }*/

/* Extract from an array REGBUF containing the (raw) register state
   a function return value of type TYPE, and copy that, in virtual format,
   into VALBUF.  */

#define EXTRACT_RETURN_VALUE(TYPE,REGBUF,VALBUF) \
  bcopy ((char *)(REGBUF), VALBUF, TYPE_LENGTH(TYPE))


/* Write into appropriate registers a function return value
   of type TYPE, given in virtual format.  Assumes floats are passed
   in d0/d1.  */


#define STORE_RETURN_VALUE(TYPE,VALBUF) \
  write_register_bytes (0, VALBUF, TYPE_LENGTH (TYPE))


/* Extract from an array REGBUF containing the (raw) register state
   the address in which a function should return its structure value,
   as a CORE_ADDR (or an expression that can be used as one).  */

#define EXTRACT_STRUCT_VALUE_ADDRESS(REGBUF) (*(CORE_ADDR *)(REGBUF))


/* Define other aspects of the stack frame.  */

/* A macro that tells us whether the function invocation represented
   by FI does not have a frame on the stack associated with it.  If it
   does not, FRAMELESS is set to 1, else 0.  */
#define FRAMELESS_FUNCTION_INVOCATION(FI, FRAMELESS) \
  (FRAMELESS) = frameless_look_for_prologue(FI)

/* Any function with a frame looks like this
   SECOND ARG
   FIRST ARG
   RET PC
   SAVED R2
   SAVED R3
   SAVED FP   <-FP POINTS HERE
   LOCALS0
   LOCALS1    <-SP POINTS HERE
   
   */
#define FRAME_SAVED_PC(FRAME) frame_saved_pc(FRAME)

#define FRAME_ARGS_ADDRESS(fi) frame_args_address(fi)

#define FRAME_LOCALS_ADDRESS(fi) frame_locals_address(fi);

/* Set VAL to the number of args passed to frame described by FI.
   Can set VAL to -1, meaning no way to tell.  */

/* We can't tell how many args there are
   now that the C compiler delays popping them.  */

#define FRAME_NUM_ARGS(val,fi) (val = -1)

/* Return number of bytes at start of arglist that are not really args.  */

#define FRAME_ARGS_SKIP 0

/* Put here the code to store, into a struct frame_saved_regs,
   the addresses of the saved registers of frame described by FRAME_INFO.
   This includes special registers such as pc and fp saved in special
   ways in the stack frame.  sp is even more special:
   the address we return for it IS the sp for the next frame.  */

#define FRAME_FIND_SAVED_REGS(frame_info, frame_saved_regs)	    \
   frame_find_saved_regs(frame_info, &(frame_saved_regs))


/* Push an empty stack frame, to record the current PC, etc.  */

/*#define PUSH_DUMMY_FRAME	{ h8300_push_dummy_frame (); }*/

/* Discard from the stack the innermost frame, restoring all registers.  */

#define POP_FRAME		{ h8300_pop_frame (); }

#define SHORT_INT_MAX 32767
#define SHORT_INT_MIN -32768



#define	BEFORE_MAIN_LOOP_HOOK	\
  hms_before_main_loop();


#define NAMES_HAVE_UNDERSCORE

typedef unsigned short INSN_WORD;

#define ADDR_BITS_REMOVE(addr) ((addr) & 0xffffff)
#define ADDR_BITS_SET(addr) (((addr)))

#define read_memory_short(x)  (read_memory_integer(x,2) & 0xffff)
#define DONT_USE_REMOTE


#define	PRINT_REGISTER_HOOK(regno) print_register_hook(regno)


int minimum_mode;

#define CALL_DUMMY_LENGTH 10
