/* Target-specific definition for the Mitsubishi D10V
   Copyright (C) 1996 Free Software Foundation, Inc.

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


#define GDB_TARGET_IS_D10V


/* Define the bit, byte, and word ordering of the machine.  */

#define TARGET_BYTE_ORDER	BIG_ENDIAN


/* Offset from address of function to start of its code.
   Zero on most machines.  */

#define FUNCTION_START_OFFSET 0

/* Advance PC across any function entry prologue instructions
   to reach some "real" code.  */

extern CORE_ADDR d10v_skip_prologue ();
#define SKIP_PROLOGUE(ip) \
    {(ip) = d10v_skip_prologue(ip);}


/* Immediately after a function call, return the saved pc.
   Can't always go through the frames for this because on some machines
   the new frame is not set up until the new function executes
   some instructions. 

   The return address is the value saved in the PR register + 4  */

#define SAVED_PC_AFTER_CALL(frame) \
  (ADDR_BITS_REMOVE(read_register(PR_REGNUM)))

/* Stack grows downward.  */

#define INNER_THAN <

    /* for a breakpoint, use "dbt || nop" */
#define BREAKPOINT {0x2f, 0x90, 0x5e, 0x00} 

/* If your kernel resets the pc after the trap happens you may need to
   define this before including this file.  */
#define DECR_PC_AFTER_BREAK 0

#define REGISTER_NAMES \
{ "r0", "r1", "r2", "r3", "r4", "r5",  "r6", "r7", \
    "r8", "r9", "r10","r11","r12", "r13", "r14","sp",\
    "psw","bpsw","pc","bpc", "cr4", "cr5", "cr6", "rpt_c",\
    "rpt_s","rpt_e", "mod_s", "mod_e", "cr12", "cr13", "iba", "cr15",\
    "a0", "a1"\
    }

#define NUM_REGS 34

/* Register numbers of various important registers.
   Note that some of these values are "real" register numbers,
   and correspond to the general registers of the machine,
   and some are "phony" register numbers which are too large
   to be actual register numbers as far as the user is concerned
   but do serve to get the desired values when passed to read_register.  */

#define R0_REGNUM	0
#define LR_REGNUM 	13
#define SP_REGNUM 	15
#define FP_REGNUM	11
#define PC_REGNUM 	18
#define PR_REGNUM 	17
#define PSW_REGNUM 	16
#define A0_REGNUM 	32

/* Say how much memory is needed to store a copy of the register set */
#define REGISTER_BYTES    ((NUM_REGS-2)*2+16) 

/* Index within `registers' of the first byte of the space for
   register N.  */

#define REGISTER_BYTE(N)  \
( ((N) > A0_REGNUM) ? ( ((N)-A0_REGNUM)*8 + A0_REGNUM*2 ) : ((N) * 2) )

/* Number of bytes of storage in the actual machine representation
   for register N.  */

#define REGISTER_RAW_SIZE(N) ( ((N) >= A0_REGNUM) ? 8 : 2 )

/* Number of bytes of storage in the program's representation
   for register N.  */   
#define REGISTER_VIRTUAL_SIZE(N) ( ((N) >= A0_REGNUM) ? 8 : 2 )

/* Largest value REGISTER_RAW_SIZE can have.  */

#define MAX_REGISTER_RAW_SIZE 8

/* Largest value REGISTER_VIRTUAL_SIZE can have.  */

#define MAX_REGISTER_VIRTUAL_SIZE 8

/* Return the GDB type object for the "standard" data type
   of data in register N.  */

#define REGISTER_VIRTUAL_TYPE(N) \
( ((N) < A0_REGNUM ) ? builtin_type_short : builtin_type_long_long)


/* Store the address of the place in which to copy the structure the
   subroutine will return.  This is called from call_function. 

   We store structs through a pointer passed in R2 */

#define STORE_STRUCT_RETURN(ADDR, SP) \
    { write_register (2, (ADDR));  }

/* Extract from an array REGBUF containing the (raw) register state
   a function return value of type TYPE, and copy that, in virtual format,
   into VALBUF.  */

#define EXTRACT_RETURN_VALUE(TYPE,REGBUF,VALBUF) \
  memcpy (VALBUF, (char *)(REGBUF), TYPE_LENGTH(TYPE))


/* Write into appropriate registers a function return value
   of type TYPE, given in virtual format.  

   Things always get returned in R2/R3 */

#define STORE_RETURN_VALUE(TYPE,VALBUF) \
  write_register_bytes (REGISTER_BYTE(2), VALBUF, TYPE_LENGTH (TYPE))


/* Extract from an array REGBUF containing the (raw) register state
   the address in which a function should return its structure value,
   as a CORE_ADDR (or an expression that can be used as one).  */

#define EXTRACT_STRUCT_VALUE_ADDRESS(REGBUF) (*(CORE_ADDR *)(REGBUF))


/* Define other aspects of the stack frame. 
   we keep a copy of the worked out return pc lying around, since it
   is a useful bit of info */

#define EXTRA_FRAME_INFO \
    CORE_ADDR return_pc; \
    int size;

#define INIT_EXTRA_FRAME_INFO(fromleaf, fi) \
    d10v_init_extra_frame_info(fromleaf, fi) 

/* A macro that tells us whether the function invocation represented
   by FI does not have a frame on the stack associated with it.  If it
   does not, FRAMELESS is set to 1, else 0.  */

#define FRAMELESS_FUNCTION_INVOCATION(FI, FRAMELESS) \
  (FRAMELESS) = frameless_look_for_prologue(FI)

#define FRAME_CHAIN(FRAME)       d10v_frame_chain(FRAME)
#define FRAME_SAVED_PC(FRAME)    ((FRAME)->return_pc)   
#define FRAME_ARGS_ADDRESS(fi)   (fi)->frame
#define FRAME_LOCALS_ADDRESS(fi) (fi)->frame

/* Set VAL to the number of args passed to frame described by FI.
   Can set VAL to -1, meaning no way to tell.  */

/* We can't tell how many args there are */

#define FRAME_NUM_ARGS(val,fi) (val = -1)

/* Return number of bytes at start of arglist that are not really args.  */

#define FRAME_ARGS_SKIP 0

/* Put here the code to store, into a struct frame_saved_regs,
   the addresses of the saved registers of frame described by FRAME_INFO.
   This includes special registers such as pc and fp saved in special
   ways in the stack frame.  sp is even more special:
   the address we return for it IS the sp for the next frame.  */

#define FRAME_FIND_SAVED_REGS(frame_info, frame_saved_regs)	    \
   d10v_frame_find_saved_regs(frame_info, &(frame_saved_regs))

#define NAMES_HAVE_UNDERSCORE

typedef unsigned short INSN_WORD;

#define ADDR_BITS_REMOVE(addr) ((addr))

#define CALL_DUMMY_LENGTH 10

/* Discard from the stack the innermost frame,
   restoring all saved registers.  */

#define POP_FRAME d10v_pop_frame();

#define REGISTER_SIZE 2

#  ifdef CC_HAS_LONG_LONG
#    define LONGEST long long
#  else
#    define LONGEST long
#  endif 

void d10v_write_register_pid PARAMS (( int regno, LONGEST val, int pid));
CORE_ADDR d10v_read_register_pid PARAMS ((int regno, int pid));

#define TARGET_READ_PC(pid)		d10v_read_register_pid (PC_REGNUM, pid)
#define TARGET_WRITE_PC(val,pid)	d10v_write_register_pid (PC_REGNUM, val, pid)
