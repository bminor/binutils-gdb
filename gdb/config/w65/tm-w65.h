/* OBSOLETE /* Parameters for execution on a WDC 65816 machine. */
/* OBSOLETE    Copyright 1995, 1998, 1999, 2000 Free Software Foundation, Inc. */
/* OBSOLETE  */
/* OBSOLETE    This file is part of GDB. */
/* OBSOLETE  */
/* OBSOLETE    This program is free software; you can redistribute it and/or modify */
/* OBSOLETE    it under the terms of the GNU General Public License as published by */
/* OBSOLETE    the Free Software Foundation; either version 2 of the License, or */
/* OBSOLETE    (at your option) any later version. */
/* OBSOLETE  */
/* OBSOLETE    This program is distributed in the hope that it will be useful, */
/* OBSOLETE    but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* OBSOLETE    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* OBSOLETE    GNU General Public License for more details. */
/* OBSOLETE  */
/* OBSOLETE    You should have received a copy of the GNU General Public License */
/* OBSOLETE    along with this program; if not, write to the Free Software */
/* OBSOLETE    Foundation, Inc., 59 Temple Place - Suite 330, */
/* OBSOLETE    Boston, MA 02111-1307, USA.  */ */
/* OBSOLETE  */
/* OBSOLETE #include "regcache.h" */
/* OBSOLETE  */
/* OBSOLETE /* Contributed by Steve Chamberlain sac@cygnus.com */ */
/* OBSOLETE  */
/* OBSOLETE #define GDB_TARGET_IS_W65 */
/* OBSOLETE  */
/* OBSOLETE #define IEEE_FLOAT (1) */
/* OBSOLETE  */
/* OBSOLETE /* Define the bit, byte, and word ordering of the machine.  */ */
/* OBSOLETE  */
/* OBSOLETE #define TARGET_BYTE_ORDER LITTLE_ENDIAN */
/* OBSOLETE  */
/* OBSOLETE  */
/* OBSOLETE /* Offset from address of function to start of its code. */
/* OBSOLETE    Zero on most machines.  */ */
/* OBSOLETE  */
/* OBSOLETE #define FUNCTION_START_OFFSET 0 */
/* OBSOLETE  */
/* OBSOLETE /* Advance PC across any function entry prologue instructions */
/* OBSOLETE    to reach some "real" code.  */ */
/* OBSOLETE  */
/* OBSOLETE extern CORE_ADDR w65_skip_prologue (); */
/* OBSOLETE  */
/* OBSOLETE #define SKIP_PROLOGUE(ip) \ */
/* OBSOLETE     {(ip) = w65_skip_prologue(ip);} */
/* OBSOLETE  */
/* OBSOLETE  */
/* OBSOLETE /* Immediately after a function call, return the saved pc. */
/* OBSOLETE    Can't always go through the frames for this because on some machines */
/* OBSOLETE    the new frame is not set up until the new function executes */
/* OBSOLETE    some instructions.  */
/* OBSOLETE  */
/* OBSOLETE    The return address is the value saved in the PR register + 4  */ */
/* OBSOLETE  */
/* OBSOLETE #define SAVED_PC_AFTER_CALL(frame) \ */
/* OBSOLETE   saved_pc_after_call(frame) */
/* OBSOLETE  */
/* OBSOLETE  */
/* OBSOLETE /* Stack grows downward.  */ */
/* OBSOLETE  */
/* OBSOLETE #define INNER_THAN(lhs,rhs) ((lhs) < (rhs)) */
/* OBSOLETE  */
/* OBSOLETE /* Illegal instruction - used by the simulator for breakpoint */
/* OBSOLETE    detection */ */
/* OBSOLETE  */
/* OBSOLETE #define BREAKPOINT {0xcb}	/* WAI */ */
/* OBSOLETE  */
/* OBSOLETE /* If your kernel resets the pc after the trap happens you may need to */
/* OBSOLETE    define this before including this file.  */ */
/* OBSOLETE #define DECR_PC_AFTER_BREAK 0 */
/* OBSOLETE  */
/* OBSOLETE /* Return 1 if P points to an invalid floating point value.  */ */
/* OBSOLETE  */
/* OBSOLETE #define INVALID_FLOAT(p, len) 0	/* Just a first guess; not checked */ */
/* OBSOLETE  */
/* OBSOLETE /* Say how long registers are.  */ */
/* OBSOLETE /*#define REGISTER_TYPE  int */ */
/* OBSOLETE  */
/* OBSOLETE /* Say how much memory is needed to store a copy of the register set */ */
/* OBSOLETE #define REGISTER_BYTES    (NUM_REGS*4) */
/* OBSOLETE  */
/* OBSOLETE /* Index within `registers' of the first byte of the space for */
/* OBSOLETE    register N.  */ */
/* OBSOLETE  */
/* OBSOLETE #define REGISTER_BYTE(N)  ((N)*4) */
/* OBSOLETE  */
/* OBSOLETE /* Number of bytes of storage in the actual machine representation */
/* OBSOLETE    for register N.  */ */
/* OBSOLETE  */
/* OBSOLETE #define REGISTER_RAW_SIZE(N)     (((N) < 16) ? 2 : 4) */
/* OBSOLETE  */
/* OBSOLETE #define REGISTER_VIRTUAL_SIZE(N)  REGISTER_RAW_SIZE(N) */
/* OBSOLETE  */
/* OBSOLETE /* Largest value REGISTER_RAW_SIZE can have.  */ */
/* OBSOLETE  */
/* OBSOLETE #define MAX_REGISTER_RAW_SIZE 4 */
/* OBSOLETE  */
/* OBSOLETE /* Largest value REGISTER_VIRTUAL_SIZE can have.  */ */
/* OBSOLETE  */
/* OBSOLETE #define MAX_REGISTER_VIRTUAL_SIZE 4 */
/* OBSOLETE  */
/* OBSOLETE /* Return the GDB type object for the "standard" data type */
/* OBSOLETE    of data in register N.  */ */
/* OBSOLETE  */
/* OBSOLETE #define REGISTER_VIRTUAL_TYPE(N) \ */
/* OBSOLETE   (REGISTER_VIRTUAL_SIZE(N) == 2 ? builtin_type_unsigned_short : builtin_type_unsigned_long) */
/* OBSOLETE  */
/* OBSOLETE /* Initializer for an array of names of registers. */
/* OBSOLETE    Entries beyond the first NUM_REGS are ignored.  */ */
/* OBSOLETE  */
/* OBSOLETE #define REGISTER_NAMES \ */
/* OBSOLETE   {"r0","r1","r2", "r3", "r4", "r5", "r6", "r7",  \ */
/* OBSOLETE    "r8","r9","r10","r11","r12","r13","r14","r15", \ */
/* OBSOLETE    "pc","a", "x",  "y",  "dbr","d",  "s",  "p",   \ */
/* OBSOLETE    "ticks","cycles","insts"} */
/* OBSOLETE  */
/* OBSOLETE /* Register numbers of various important registers. */
/* OBSOLETE    Note that some of these values are "real" register numbers, */
/* OBSOLETE    and correspond to the general registers of the machine, */
/* OBSOLETE    and some are "phony" register numbers which are too large */
/* OBSOLETE    to be actual register numbers as far as the user is concerned */
/* OBSOLETE    but do serve to get the desired values when passed to read_register.  */ */
/* OBSOLETE  */
/* OBSOLETE #define SP_REGNUM  22 */
/* OBSOLETE #define FP_REGNUM  15 */
/* OBSOLETE #define NUM_REGS   27 */
/* OBSOLETE #define PC_REGNUM  16 */
/* OBSOLETE #define P_REGNUM   23 */
/* OBSOLETE  */
/* OBSOLETE /* Store the address of the place in which to copy the structure the */
/* OBSOLETE    subroutine will return.  This is called from call_function.  */
/* OBSOLETE  */
/* OBSOLETE    We store structs through a pointer passed in R4 */ */
/* OBSOLETE  */
/* OBSOLETE #define STORE_STRUCT_RETURN(ADDR, SP) \ */
/* OBSOLETE     { write_register (4, (ADDR));  } */
/* OBSOLETE  */
/* OBSOLETE /* Extract from an array REGBUF containing the (raw) register state */
/* OBSOLETE    a function return value of type TYPE, and copy that, in virtual format, */
/* OBSOLETE    into VALBUF.  */ */
/* OBSOLETE  */
/* OBSOLETE #define EXTRACT_RETURN_VALUE(TYPE,REGBUF,VALBUF) \ */
/* OBSOLETE   memcpy (VALBUF, (char *)(REGBUF), TYPE_LENGTH(TYPE)) */
/* OBSOLETE  */
/* OBSOLETE  */
/* OBSOLETE /* Write into appropriate registers a function return value */
/* OBSOLETE    of type TYPE, given in virtual format.   */
/* OBSOLETE  */
/* OBSOLETE    Things always get returned in R4/R5 */ */
/* OBSOLETE  */
/* OBSOLETE #define STORE_RETURN_VALUE(TYPE,VALBUF) \ */
/* OBSOLETE   write_register_bytes (REGISTER_BYTE(4), VALBUF, TYPE_LENGTH (TYPE)) */
/* OBSOLETE  */
/* OBSOLETE  */
/* OBSOLETE /* Extract from an array REGBUF containing the (raw) register state */
/* OBSOLETE    the address in which a function should return its structure value, */
/* OBSOLETE    as a CORE_ADDR (or an expression that can be used as one).  */ */
/* OBSOLETE  */
/* OBSOLETE #define EXTRACT_STRUCT_VALUE_ADDRESS(REGBUF) (*(CORE_ADDR *)(REGBUF)) */
/* OBSOLETE  */
/* OBSOLETE /* A macro that tells us whether the function invocation represented */
/* OBSOLETE    by FI does not have a frame on the stack associated with it.  If it */
/* OBSOLETE    does not, FRAMELESS is set to 1, else 0.  */ */
/* OBSOLETE  */
/* OBSOLETE #define FRAMELESS_FUNCTION_INVOCATION(FI) \ */
/* OBSOLETE   (frameless_look_for_prologue (FI)) */
/* OBSOLETE  */
/* OBSOLETE #define FRAME_CHAIN(FRAME)       w65_frame_chain(FRAME) */
/* OBSOLETE #define FRAME_SAVED_PC(FRAME)    (w65_frame_saved_pc(FRAME)) */
/* OBSOLETE #define FRAME_ARGS_ADDRESS(fi)   (fi)->frame */
/* OBSOLETE #define FRAME_LOCALS_ADDRESS(fi) (fi)->frame */
/* OBSOLETE  */
/* OBSOLETE /* Set VAL to the number of args passed to frame described by FI. */
/* OBSOLETE    Can set VAL to -1, meaning no way to tell.  */ */
/* OBSOLETE  */
/* OBSOLETE /* We can't tell how many args there are */ */
/* OBSOLETE  */
/* OBSOLETE #define FRAME_NUM_ARGS(fi) (-1) */
/* OBSOLETE  */
/* OBSOLETE /* Return number of bytes at start of arglist that are not really args.  */ */
/* OBSOLETE  */
/* OBSOLETE #define FRAME_ARGS_SKIP 0 */
/* OBSOLETE  */
/* OBSOLETE /* Put here the code to store, into a struct frame_saved_regs, */
/* OBSOLETE    the addresses of the saved registers of frame described by FRAME_INFO. */
/* OBSOLETE    This includes special registers such as pc and fp saved in special */
/* OBSOLETE    ways in the stack frame.  sp is even more special: */
/* OBSOLETE    the address we return for it IS the sp for the next frame.  */ */
/* OBSOLETE  */
/* OBSOLETE #define FRAME_FIND_SAVED_REGS(frame_info, frame_saved_regs)	    \ */
/* OBSOLETE    frame_find_saved_regs(frame_info, &(frame_saved_regs)) */
/* OBSOLETE  */
/* OBSOLETE typedef unsigned short INSN_WORD; */
/* OBSOLETE  */
/* OBSOLETE extern CORE_ADDR w65_addr_bits_remove (CORE_ADDR); */
/* OBSOLETE #define ADDR_BITS_REMOVE(addr) w65_addr_bits_remove (addr) */
/* OBSOLETE  */
/* OBSOLETE #define CALL_DUMMY_LENGTH 10 */
/* OBSOLETE  */
/* OBSOLETE /* Discard from the stack the innermost frame, */
/* OBSOLETE    restoring all saved registers.  */ */
/* OBSOLETE  */
/* OBSOLETE #define POP_FRAME pop_frame(); */
/* OBSOLETE  */
/* OBSOLETE  */
/* OBSOLETE #define NOP   {0xea} */
/* OBSOLETE  */
/* OBSOLETE #define REGISTER_SIZE 4 */
/* OBSOLETE  */
/* OBSOLETE #define	PRINT_REGISTER_HOOK(regno) print_register_hook(regno) */
/* OBSOLETE  */
/* OBSOLETE #define TARGET_INT_BIT  16 */
/* OBSOLETE #define TARGET_LONG_BIT 32 */
/* OBSOLETE #define TARGET_PTR_BIT  32 */
