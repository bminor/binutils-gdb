/* OBSOLETE /* Parameters for execution on a TI TMS320C80 (MVP) processor. */
/* OBSOLETE    Copyright 1997, 1999, 2000 */
/* OBSOLETE    Free Software Foundation, Inc. */
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
/* OBSOLETE #ifndef TM_TIC80_H */
/* OBSOLETE #define TM_TIC80_H */
/* OBSOLETE  */
/* OBSOLETE #include "regcache.h" */
/* OBSOLETE  */
/* OBSOLETE /* Forward declare structs used in prototypes */ */
/* OBSOLETE struct frame_info; */
/* OBSOLETE struct type; */
/* OBSOLETE struct value; */
/* OBSOLETE struct symbol; */
/* OBSOLETE struct frame_saved_regs; */
/* OBSOLETE  */
/* OBSOLETE #define TARGET_BYTE_ORDER LITTLE_ENDIAN */
/* OBSOLETE  */
/* OBSOLETE #define NUM_REGS 38 */
/* OBSOLETE  */
/* OBSOLETE #define REGISTER_NAMES \ */
/* OBSOLETE { "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", \ */
/* OBSOLETE   "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15", \ */
/* OBSOLETE   "r16", "r17", "r18", "r19", "r20", "r21", "r22", "r23", \ */
/* OBSOLETE   "r24", "r25", "r26", "r27", "r28", "r29", "r30", "r31", \ */
/* OBSOLETE   "pc", "npc", \ */
/* OBSOLETE   "a0", "a1", "a2", "a3", \ */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* Various dedicated register numbers */
/* OBSOLETE    FIXME: Shadow updates in sim/tic80/sim-calls.c */ */
/* OBSOLETE  */
/* OBSOLETE #define SP_REGNUM 1		/* Contains address of top of stack */ */
/* OBSOLETE #define ARG0_REGNUM 2		/* Contains argument 1 (r3 has high word) */ */
/* OBSOLETE #define RET_REGNUM  2		/* Contains function return value */ */
/* OBSOLETE #define ARGLAST_REGNUM 12	/* Contains argument 6 (r13 has high word) */ */
/* OBSOLETE #define FP_REGNUM 30		/* Contains address of executing stack frame */ */
/* OBSOLETE #define LR_REGNUM 31		/* Contains address of caller (link register) */ */
/* OBSOLETE #define PC_REGNUM 32		/* Contains program counter (FIXME?) */ */
/* OBSOLETE #define NPC_REGNUM 33		/* Contains the next program counter (FIXME?) */ */
/* OBSOLETE #define A0_REGNUM 34		/* Accumulator register 0 */ */
/* OBSOLETE #define A3_REGNUM 37		/* Accumulator register 1 */ */
/* OBSOLETE  */
/* OBSOLETE #define R0_REGNUM 0		/* General Purpose Register 0 - for sim */ */
/* OBSOLETE #define Rn_REGNUM 31		/* Last General Purpose Register - for sim */ */
/* OBSOLETE #define An_REGNUM A3_REGNUM	/* Last Accumulator register - for sim */ */
/* OBSOLETE  */
/* OBSOLETE /* Total amount of space needed to store our copies of the machine's */
/* OBSOLETE    register state, the array `registers'.  */ */
/* OBSOLETE  */
/* OBSOLETE #define REGISTER_BYTES (((NUM_REGS - 4) * 4) + (4 * 8)) */
/* OBSOLETE  */
/* OBSOLETE /* Index within `registers' of the first byte of the space for */
/* OBSOLETE    register N.  */ */
/* OBSOLETE  */
/* OBSOLETE #define REGISTER_BYTE(N) \ */
/* OBSOLETE    (((N) >= A0_REGNUM) ? (((N) - A0_REGNUM) * 8 + A0_REGNUM * 4) : ((N) * 4)) */
/* OBSOLETE  */
/* OBSOLETE /* Most registers are 4 bytes */ */
/* OBSOLETE  */
/* OBSOLETE #define REGISTER_SIZE 4 */
/* OBSOLETE  */
/* OBSOLETE /* Some registers are 8 bytes.  */ */
/* OBSOLETE  */
/* OBSOLETE #define REGISTER_RAW_SIZE(N) \ */
/* OBSOLETE      (((N) >= A0_REGNUM) ? 8 : 4) */
/* OBSOLETE  */
/* OBSOLETE /* Largest value REGISTER_RAW_SIZE can have.  */ */
/* OBSOLETE  */
/* OBSOLETE #define MAX_REGISTER_RAW_SIZE (8) */
/* OBSOLETE  */
/* OBSOLETE /* All regs are 4 bytes.  */ */
/* OBSOLETE  */
/* OBSOLETE #define REGISTER_VIRTUAL_SIZE(N) (REGISTER_RAW_SIZE(N)) */
/* OBSOLETE  */
/* OBSOLETE /* Largest value REGISTER_VIRTUAL_SIZE can have.  */ */
/* OBSOLETE  */
/* OBSOLETE #define MAX_REGISTER_VIRTUAL_SIZE (MAX_REGISTER_RAW_SIZE) */
/* OBSOLETE  */
/* OBSOLETE /* Return the GDB type object for the "standard" data type */
/* OBSOLETE    of data in register N.  */ */
/* OBSOLETE  */
/* OBSOLETE #define REGISTER_VIRTUAL_TYPE(N) /* FIXME? */ \ */
/* OBSOLETE 	(((N) >= A0_REGNUM) ? builtin_type_float : builtin_type_int) */
/* OBSOLETE  */
/* OBSOLETE /* Offset from address of function to start of its code. */
/* OBSOLETE    Zero on most machines.  */ */
/* OBSOLETE  */
/* OBSOLETE #define FUNCTION_START_OFFSET 0 */
/* OBSOLETE  */
/* OBSOLETE /* Stack grows downward.  */ */
/* OBSOLETE  */
/* OBSOLETE #define INNER_THAN(lhs,rhs) ((lhs) < (rhs)) */
/* OBSOLETE  */
/* OBSOLETE /* Sequence of bytes for breakpoint instruction. */
/* OBSOLETE    This is padded out to the size of a machine word. */ */
/* OBSOLETE  */
/* OBSOLETE #define BREAKPOINT 		{0x49, 0x80, 0x00, 0x00}	/* FIXME! */ */
/* OBSOLETE  */
/* OBSOLETE /* Amount PC must be decremented by after a breakpoint. */
/* OBSOLETE    This is often the number of bytes in BREAKPOINT */
/* OBSOLETE    but not always.  */ */
/* OBSOLETE  */
/* OBSOLETE #define DECR_PC_AFTER_BREAK	0	/* FIXME! */ */
/* OBSOLETE  */
/* OBSOLETE /* Discard from the stack the innermost frame, restoring all registers.  */ */
/* OBSOLETE  */
/* OBSOLETE #define POP_FRAME tic80_pop_frame(get_current_frame ()) */
/* OBSOLETE extern struct frame_info *tic80_pop_frame (struct frame_info *frame); */
/* OBSOLETE  */
/* OBSOLETE /* Return number of bytes at start of arglist that are not really args.  */ */
/* OBSOLETE  */
/* OBSOLETE #define FRAME_ARGS_SKIP 0 */
/* OBSOLETE  */
/* OBSOLETE /* Set VAL to the number of args passed to frame described by FI. */
/* OBSOLETE    Can set VAL to -1, meaning no way to tell.  */ */
/* OBSOLETE /* We can't tell how many args there are */ */
/* OBSOLETE  */
/* OBSOLETE #define FRAME_NUM_ARGS(fi) (-1) */
/* OBSOLETE  */
/* OBSOLETE #define FRAME_ARGS_SKIP 0 */
/* OBSOLETE #define FRAME_ARGS_ADDRESS(fi)   (fi)->frame */
/* OBSOLETE #define FRAME_LOCALS_ADDRESS(fi) (fi)->frame */
/* OBSOLETE  */
/* OBSOLETE /* Define other aspects of the stack frame.  */
/* OBSOLETE    We keep the offsets of all saved registers, 'cause we need 'em a lot! */
/* OBSOLETE    We also keep the current size of the stack frame, and the offset of */
/* OBSOLETE    the frame pointer from the stack pointer (for frameless functions, and */
/* OBSOLETE    when we're still in the prologue of a function with a frame) */ */
/* OBSOLETE  */
/* OBSOLETE #define EXTRA_FRAME_INFO  	\ */
/* OBSOLETE   struct frame_saved_regs fsr;	\ */
/* OBSOLETE   int framesize;		\ */
/* OBSOLETE   int frameoffset;		\ */
/* OBSOLETE   int framereg; */
/* OBSOLETE  */
/* OBSOLETE extern void tic80_init_extra_frame_info (struct frame_info *fi); */
/* OBSOLETE #define INIT_EXTRA_FRAME_INFO(fromleaf, fi) tic80_init_extra_frame_info (fi) */
/* OBSOLETE #define INIT_FRAME_PC		/* Not necessary */ */
/* OBSOLETE  */
/* OBSOLETE /* Put here the code to store, into a struct frame_saved_regs, */
/* OBSOLETE    the addresses of the saved registers of frame described by FRAME_INFO. */
/* OBSOLETE    This includes special registers such as pc and fp saved in special */
/* OBSOLETE    ways in the stack frame.  sp is even more special: */
/* OBSOLETE    the address we return for it IS the sp for the next frame.  */ */
/* OBSOLETE  */
/* OBSOLETE #define FRAME_FIND_SAVED_REGS(frame_info, frame_saved_regs)	    \ */
/* OBSOLETE    tic80_frame_find_saved_regs(frame_info, &(frame_saved_regs)) */
/* OBSOLETE extern void tic80_frame_find_saved_regs (struct frame_info *, */
/* OBSOLETE 					 struct frame_saved_regs *); */
/* OBSOLETE  */
/* OBSOLETE /* Advance PC across any function entry prologue instructions */
/* OBSOLETE    to reach some "real" code.  */ */
/* OBSOLETE  */
/* OBSOLETE #define SKIP_PROLOGUE(pc) (tic80_skip_prologue (pc)) */
/* OBSOLETE extern CORE_ADDR tic80_skip_prologue (CORE_ADDR pc); */
/* OBSOLETE  */
/* OBSOLETE /* Immediately after a function call, return the saved pc. */
/* OBSOLETE    Can't always go through the frames for this because on some machines */
/* OBSOLETE    the new frame is not set up until the new function executes */
/* OBSOLETE    some instructions.  */ */
/* OBSOLETE  */
/* OBSOLETE #define SAVED_PC_AFTER_CALL(frame) read_register (LR_REGNUM) */
/* OBSOLETE  */
/* OBSOLETE /* Describe the pointer in each stack frame to the previous stack frame */
/* OBSOLETE    (its caller).  */ */
/* OBSOLETE  */
/* OBSOLETE /* FRAME_CHAIN takes a frame's nominal address */
/* OBSOLETE    and produces the frame's chain-pointer. */ */
/* OBSOLETE  */
/* OBSOLETE #define FRAME_CHAIN(thisframe) (CORE_ADDR) tic80_frame_chain (thisframe) */
/* OBSOLETE extern CORE_ADDR tic80_frame_chain (struct frame_info *); */
/* OBSOLETE  */
/* OBSOLETE #define FRAME_SAVED_PC(FRAME)	tic80_frame_saved_pc (FRAME) */
/* OBSOLETE extern CORE_ADDR tic80_frame_saved_pc (struct frame_info *); */
/* OBSOLETE  */
/* OBSOLETE /* Store the address of the place in which to copy the structure the */
/* OBSOLETE    subroutine will return.  This is called from call_function.  */
/* OBSOLETE  */
/* OBSOLETE    We store structs through a pointer passed in R2 */ */
/* OBSOLETE  */
/* OBSOLETE #define STORE_STRUCT_RETURN(STRUCT_ADDR, SP)	\ */
/* OBSOLETE 	write_register (ARG0_REGNUM, STRUCT_ADDR) */
/* OBSOLETE  */
/* OBSOLETE /* Extract from an array REGBUF containing the (raw) register state */
/* OBSOLETE    a function return value of type TYPE, and copy that, in virtual format, */
/* OBSOLETE    into VALBUF.  */ */
/* OBSOLETE  */
/* OBSOLETE #define EXTRACT_RETURN_VALUE(TYPE,REGBUF,VALBUF) \ */
/* OBSOLETE   memcpy ((VALBUF), \ */
/* OBSOLETE 	  (char *)(REGBUF) + REGISTER_BYTE (RET_REGNUM) + \ */
/* OBSOLETE 	  ((TYPE_LENGTH (TYPE) > 4 ? 8 : 4) - TYPE_LENGTH (TYPE)), \ */
/* OBSOLETE 	  TYPE_LENGTH (TYPE)) */
/* OBSOLETE  */
/* OBSOLETE /* Write into appropriate registers a function return value */
/* OBSOLETE    of type TYPE, given in virtual format.  */ */
/* OBSOLETE  */
/* OBSOLETE #define STORE_RETURN_VALUE(TYPE,VALBUF) \ */
/* OBSOLETE   write_register_bytes(REGISTER_BYTE (RET_REGNUM) + \ */
/* OBSOLETE 		       ((TYPE_LENGTH (TYPE) > 4 ? 8:4) - TYPE_LENGTH (TYPE)),\ */
/* OBSOLETE 		       (VALBUF), TYPE_LENGTH (TYPE)); */
/* OBSOLETE  */
/* OBSOLETE  */
/* OBSOLETE  */
/* OBSOLETE /* PUSH_ARGUMENTS */ */
/* OBSOLETE extern CORE_ADDR tic80_push_arguments (int nargs, */
/* OBSOLETE 				       struct value **args, */
/* OBSOLETE 				       CORE_ADDR sp, */
/* OBSOLETE 				       unsigned char struct_return, */
/* OBSOLETE 				       CORE_ADDR struct_addr); */
/* OBSOLETE  */
/* OBSOLETE #define PUSH_ARGUMENTS(NARGS, ARGS, SP, STRUCT_RETURN, STRUCT_ADDR) \ */
/* OBSOLETE   (tic80_push_arguments (NARGS, ARGS, SP, STRUCT_RETURN, STRUCT_ADDR)) */
/* OBSOLETE  */
/* OBSOLETE /* PUSH_RETURN_ADDRESS */ */
/* OBSOLETE extern CORE_ADDR tic80_push_return_address (CORE_ADDR, CORE_ADDR); */
/* OBSOLETE #define PUSH_RETURN_ADDRESS(PC, SP)	tic80_push_return_address (PC, SP) */
/* OBSOLETE  */
/* OBSOLETE /* override the standard get_saved_register function with  */
/* OBSOLETE    one that takes account of generic CALL_DUMMY frames */ */
/* OBSOLETE #define GET_SAVED_REGISTER(raw_buffer, optimized, addrp, frame, regnum, lval) \ */
/* OBSOLETE       generic_get_saved_register (raw_buffer, optimized, addrp, frame, regnum, lval) */
/* OBSOLETE  */
/* OBSOLETE #define USE_GENERIC_DUMMY_FRAMES 1 */
/* OBSOLETE #define CALL_DUMMY                   {0} */
/* OBSOLETE #define CALL_DUMMY_LENGTH            (0) */
/* OBSOLETE #define CALL_DUMMY_START_OFFSET      (0) */
/* OBSOLETE #define CALL_DUMMY_BREAKPOINT_OFFSET (0) */
/* OBSOLETE #define FIX_CALL_DUMMY(DUMMY1, STARTADDR, FUNADDR, NARGS, ARGS, TYPE, GCCP) */
/* OBSOLETE #define CALL_DUMMY_LOCATION          AT_ENTRY_POINT */
/* OBSOLETE #define CALL_DUMMY_ADDRESS()         entry_point_address () */
/* OBSOLETE  */
/* OBSOLETE /* generic dummy frame stuff */ */
/* OBSOLETE  */
/* OBSOLETE #define PUSH_DUMMY_FRAME             generic_push_dummy_frame () */
/* OBSOLETE #define PC_IN_CALL_DUMMY(PC, SP, FP) generic_pc_in_call_dummy (PC, SP, FP) */
/* OBSOLETE  */
/* OBSOLETE #endif /* TM_TIC80_H */ */
