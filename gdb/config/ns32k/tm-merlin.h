/* OBSOLETE /* Definitions to target GDB to a merlin under utek 2.1 */
/* OBSOLETE    Copyright 1986, 1987, 1989, 1991, 1993, 1994, 1998, 1999, 2000 */
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
/* OBSOLETE #include "regcache.h" */
/* OBSOLETE  */
/* OBSOLETE #define TARGET_BYTE_ORDER LITTLE_ENDIAN */
/* OBSOLETE  */
/* OBSOLETE /* Offset from address of function to start of its code. */
/* OBSOLETE    Zero on most machines.  */ */
/* OBSOLETE  */
/* OBSOLETE #define FUNCTION_START_OFFSET 0 */
/* OBSOLETE  */
/* OBSOLETE /* Advance PC across any function entry prologue instructions */
/* OBSOLETE    to reach some "real" code.  */ */
/* OBSOLETE  */
/* OBSOLETE extern CORE_ADDR merlin_skip_prologue (CORE_ADDR); */
/* OBSOLETE #define SKIP_PROLOGUE(pc) (merlin_skip_prologue (pc)) */
/* OBSOLETE  */
/* OBSOLETE /* Immediately after a function call, return the saved pc. */
/* OBSOLETE    Can't always go through the frames for this because on some machines */
/* OBSOLETE    the new frame is not set up until the new function executes */
/* OBSOLETE    some instructions.  */ */
/* OBSOLETE  */
/* OBSOLETE #define SAVED_PC_AFTER_CALL(frame) \ */
/* OBSOLETE 	read_memory_integer (read_register (SP_REGNUM), 4) */
/* OBSOLETE  */
/* OBSOLETE /* Address of end of stack space.  */ */
/* OBSOLETE  */
/* OBSOLETE #define STACK_END_ADDR (0x800000) */
/* OBSOLETE  */
/* OBSOLETE /* Stack grows downward.  */ */
/* OBSOLETE  */
/* OBSOLETE #define INNER_THAN(lhs,rhs) ((lhs) < (rhs)) */
/* OBSOLETE  */
/* OBSOLETE /* Sequence of bytes for breakpoint instruction.  */ */
/* OBSOLETE  */
/* OBSOLETE #define BREAKPOINT {0xf2} */
/* OBSOLETE  */
/* OBSOLETE /* Amount PC must be decremented by after a breakpoint. */
/* OBSOLETE    This is often the number of bytes in BREAKPOINT */
/* OBSOLETE    but not always.  */ */
/* OBSOLETE  */
/* OBSOLETE #define DECR_PC_AFTER_BREAK 0 */
/* OBSOLETE  */
/* OBSOLETE /* Define this to say that the "svc" insn is followed by */
/* OBSOLETE    codes in memory saying which kind of system call it is.  */ */
/* OBSOLETE  */
/* OBSOLETE #define NS32K_SVC_IMMED_OPERANDS */
/* OBSOLETE  */
/* OBSOLETE /* Say how long (ordinary) registers are.  This is a piece of bogosity */
/* OBSOLETE    used in push_word and a few other places; REGISTER_RAW_SIZE is the */
/* OBSOLETE    real way to know how big a register is.  */ */
/* OBSOLETE  */
/* OBSOLETE #define REGISTER_SIZE 4 */
/* OBSOLETE  */
/* OBSOLETE /* Number of machine registers */ */
/* OBSOLETE  */
/* OBSOLETE #define NUM_REGS		25 */
/* OBSOLETE  */
/* OBSOLETE #define NUM_GENERAL_REGS	8 */
/* OBSOLETE  */
/* OBSOLETE /* Initializer for an array of names of registers. */
/* OBSOLETE    There should be NUM_REGS strings in this initializer.  */ */
/* OBSOLETE  */
/* OBSOLETE #define REGISTER_NAMES {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",	\ */
/* OBSOLETE 			"pc", "sp", "fp", "ps",				\ */
/* OBSOLETE  			"fsr",						\ */
/* OBSOLETE  			"f0", "f1", "f2", "f3", "f4", "f5", "f6", "f7",	\ */
/* OBSOLETE 			"l0", "l1", "l2", "l3", "l4",			\ */
/* OBSOLETE  			} */
/* OBSOLETE  */
/* OBSOLETE /* Register numbers of various important registers. */
/* OBSOLETE    Note that some of these values are "real" register numbers, */
/* OBSOLETE    and correspond to the general registers of the machine, */
/* OBSOLETE    and some are "phony" register numbers which are too large */
/* OBSOLETE    to be actual register numbers as far as the user is concerned */
/* OBSOLETE    but do serve to get the desired values when passed to read_register.  */ */
/* OBSOLETE  */
/* OBSOLETE #define AP_REGNUM FP_REGNUM */
/* OBSOLETE #define FP_REGNUM 10		/* Contains address of executing stack frame */ */
/* OBSOLETE #define SP_REGNUM 9		/* Contains address of top of stack */ */
/* OBSOLETE #define PC_REGNUM 8		/* Contains program counter */ */
/* OBSOLETE #define PS_REGNUM 11		/* Contains processor status */ */
/* OBSOLETE #define FPS_REGNUM 12		/* Floating point status register */ */
/* OBSOLETE #define FP0_REGNUM 13		/* Floating point register 0 */ */
/* OBSOLETE #define LP0_REGNUM 21		/* Double register 0 (same as FP0) */ */
/* OBSOLETE  */
/* OBSOLETE /* Total amount of space needed to store our copies of the machine's */
/* OBSOLETE    register state, the array `registers'.  */ */
/* OBSOLETE #define REGISTER_BYTES ((NUM_REGS - 4) * sizeof (int) + 4 * sizeof (double)) */
/* OBSOLETE  */
/* OBSOLETE /* Index within `registers' of the first byte of the space for */
/* OBSOLETE    register N.  */ */
/* OBSOLETE  */
/* OBSOLETE #define REGISTER_BYTE(N) ((N) >= LP0_REGNUM ? \ */
/* OBSOLETE 	LP0_REGNUM * 4 + ((N) - LP0_REGNUM) * 8 : (N) * 4) */
/* OBSOLETE  */
/* OBSOLETE /* Number of bytes of storage in the actual machine representation */
/* OBSOLETE    for register N.  On the 32000, all regs are 4 bytes */
/* OBSOLETE    except for the doubled floating registers. */ */
/* OBSOLETE  */
/* OBSOLETE #define REGISTER_RAW_SIZE(N) ((N) >= LP0_REGNUM ? 8 : 4) */
/* OBSOLETE  */
/* OBSOLETE /* Number of bytes of storage in the program's representation */
/* OBSOLETE    for register N.  On the 32000, all regs are 4 bytes */
/* OBSOLETE    except for the doubled floating registers. */ */
/* OBSOLETE  */
/* OBSOLETE #define REGISTER_VIRTUAL_SIZE(N) ((N) >= LP0_REGNUM ? 8 : 4) */
/* OBSOLETE  */
/* OBSOLETE /* Largest value REGISTER_RAW_SIZE can have.  */ */
/* OBSOLETE  */
/* OBSOLETE #define MAX_REGISTER_RAW_SIZE 8 */
/* OBSOLETE  */
/* OBSOLETE /* Largest value REGISTER_VIRTUAL_SIZE can have.  */ */
/* OBSOLETE  */
/* OBSOLETE #define MAX_REGISTER_VIRTUAL_SIZE 8 */
/* OBSOLETE  */
/* OBSOLETE /* Return the GDB type object for the "standard" data type */
/* OBSOLETE    of data in register N.  */ */
/* OBSOLETE  */
/* OBSOLETE #define REGISTER_VIRTUAL_TYPE(N) \ */
/* OBSOLETE   ((N) >= FP0_REGNUM ?		\ */
/* OBSOLETE     ((N) >= LP0_REGNUM ?	\ */
/* OBSOLETE      builtin_type_double	\ */
/* OBSOLETE      : builtin_type_float)	\ */
/* OBSOLETE    : builtin_type_int) */
/* OBSOLETE  */
/* OBSOLETE /* Store the address of the place in which to copy the structure the */
/* OBSOLETE    subroutine will return.  This is called from call_function. */
/* OBSOLETE  */
/* OBSOLETE    On this machine this is a no-op, as gcc doesn't run on it yet. */
/* OBSOLETE    This calling convention is not used. */ */
/* OBSOLETE  */
/* OBSOLETE #define STORE_STRUCT_RETURN(ADDR, SP) */
/* OBSOLETE  */
/* OBSOLETE /* Extract from an array REGBUF containing the (raw) register state */
/* OBSOLETE    a function return value of type TYPE, and copy that, in virtual format, */
/* OBSOLETE    into VALBUF.  */ */
/* OBSOLETE  */
/* OBSOLETE #define EXTRACT_RETURN_VALUE(TYPE,REGBUF,VALBUF) \ */
/* OBSOLETE   memcpy (VALBUF, REGBUF, TYPE_LENGTH (TYPE)) */
/* OBSOLETE  */
/* OBSOLETE /* Write into appropriate registers a function return value */
/* OBSOLETE    of type TYPE, given in virtual format.  */ */
/* OBSOLETE  */
/* OBSOLETE #define STORE_RETURN_VALUE(TYPE,VALBUF) \ */
/* OBSOLETE   write_register_bytes (0, VALBUF, TYPE_LENGTH (TYPE)) */
/* OBSOLETE  */
/* OBSOLETE /* Extract from an array REGBUF containing the (raw) register state */
/* OBSOLETE    the address in which a function should return its structure value, */
/* OBSOLETE    as a CORE_ADDR (or an expression that can be used as one).  */ */
/* OBSOLETE  */
/* OBSOLETE #define EXTRACT_STRUCT_VALUE_ADDRESS(REGBUF) (*(int *)(REGBUF)) */
/* OBSOLETE  */
/* OBSOLETE /* Describe the pointer in each stack frame to the previous stack frame */
/* OBSOLETE    (its caller).  */ */
/* OBSOLETE  */
/* OBSOLETE /* FRAME_CHAIN takes a frame's nominal address */
/* OBSOLETE    and produces the frame's chain-pointer. */ */
/* OBSOLETE  */
/* OBSOLETE /* In the case of the Merlin, the frame's nominal address is the FP value, */
/* OBSOLETE    and at that address is saved previous FP value as a 4-byte word.  */ */
/* OBSOLETE  */
/* OBSOLETE #define FRAME_CHAIN(thisframe)  \ */
/* OBSOLETE   (!inside_entry_file ((thisframe)->pc) ? \ */
/* OBSOLETE    read_memory_integer ((thisframe)->frame, 4) :\ */
/* OBSOLETE    0) */
/* OBSOLETE  */
/* OBSOLETE /* Define other aspects of the stack frame.  */ */
/* OBSOLETE  */
/* OBSOLETE #define FRAME_SAVED_PC(FRAME) (read_memory_integer ((FRAME)->frame + 4, 4)) */
/* OBSOLETE  */
/* OBSOLETE /* compute base of arguments */ */
/* OBSOLETE #define FRAME_ARGS_ADDRESS(fi) ((fi)->frame) */
/* OBSOLETE  */
/* OBSOLETE #define FRAME_LOCALS_ADDRESS(fi) ((fi)->frame) */
/* OBSOLETE  */
/* OBSOLETE /* Return number of args passed to a frame. */
/* OBSOLETE    Can return -1, meaning no way to tell.  */ */
/* OBSOLETE  */
/* OBSOLETE extern int merlin_frame_num_args (struct frame_info *fi); */
/* OBSOLETE #define FRAME_NUM_ARGS(fi) (merlin_frame_num_args ((fi))) */
/* OBSOLETE  */
/* OBSOLETE /* Return number of bytes at start of arglist that are not really args.  */ */
/* OBSOLETE  */
/* OBSOLETE #define FRAME_ARGS_SKIP 8 */
/* OBSOLETE  */
/* OBSOLETE /* Put here the code to store, into a struct frame_saved_regs, */
/* OBSOLETE    the addresses of the saved registers of frame described by FRAME_INFO. */
/* OBSOLETE    This includes special registers such as pc and fp saved in special */
/* OBSOLETE    ways in the stack frame.  sp is even more special: */
/* OBSOLETE    the address we return for it IS the sp for the next frame.  */ */
/* OBSOLETE  */
/* OBSOLETE #define FRAME_FIND_SAVED_REGS(frame_info, frame_saved_regs) \ */
/* OBSOLETE { int regmask,regnum;						\ */
/* OBSOLETE   int localcount;						\ */
/* OBSOLETE   CORE_ADDR enter_addr;						\ */
/* OBSOLETE   CORE_ADDR next_addr;						\ */
/* OBSOLETE 								\ */
/* OBSOLETE   enter_addr = get_pc_function_start ((frame_info)->pc);	\ */
/* OBSOLETE   regmask = read_memory_integer (enter_addr+1, 1);		\ */
/* OBSOLETE   localcount = ns32k_localcount (enter_addr);			\ */
/* OBSOLETE   next_addr = (frame_info)->frame + localcount;			\ */
/* OBSOLETE   for (regnum = 0; regnum < 8; regnum++, regmask >>= 1)		\ */
/* OBSOLETE     (frame_saved_regs).regs[regnum]				\ */
/* OBSOLETE       = (regmask & 1) ? (next_addr -= 4) : 0;			\ */
/* OBSOLETE   (frame_saved_regs).regs[SP_REGNUM] = (frame_info)->frame + 4;	\ */
/* OBSOLETE   (frame_saved_regs).regs[PC_REGNUM] = (frame_info)->frame + 4;	\ */
/* OBSOLETE   (frame_saved_regs).regs[FP_REGNUM]				\ */
/* OBSOLETE      = read_memory_integer ((frame_info)->frame, 4); } */
/* OBSOLETE  */
/* OBSOLETE  */
/* OBSOLETE /* Things needed for making the inferior call functions.  */ */
/* OBSOLETE  */
/* OBSOLETE /* Push an empty stack frame, to record the current PC, etc.  */ */
/* OBSOLETE  */
/* OBSOLETE #define PUSH_DUMMY_FRAME \ */
/* OBSOLETE { register CORE_ADDR sp = read_register (SP_REGNUM);	\ */
/* OBSOLETE   register int regnum;					\ */
/* OBSOLETE   sp = push_word (sp, read_register (PC_REGNUM));	\ */
/* OBSOLETE   sp = push_word (sp, read_register (FP_REGNUM));	\ */
/* OBSOLETE   write_register (FP_REGNUM, sp);			\ */
/* OBSOLETE   for (regnum = 0; regnum < 8; regnum++)  		\ */
/* OBSOLETE     sp = push_word (sp, read_register (regnum));	\ */
/* OBSOLETE   write_register (SP_REGNUM, sp);  			\ */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* Discard from the stack the innermost frame, restoring all registers.  */ */
/* OBSOLETE  */
/* OBSOLETE #define POP_FRAME  \ */
/* OBSOLETE { register struct frame_info *frame = get_current_frame ();	 \ */
/* OBSOLETE   register CORE_ADDR fp;					 \ */
/* OBSOLETE   register int regnum;						 \ */
/* OBSOLETE   struct frame_saved_regs fsr;					 \ */
/* OBSOLETE   struct frame_info *fi;						 \ */
/* OBSOLETE   fp = frame->frame;						 \ */
/* OBSOLETE   get_frame_saved_regs (frame, &fsr);				 \ */
/* OBSOLETE   for (regnum = 0; regnum < 8; regnum++)			 \ */
/* OBSOLETE     if (fsr.regs[regnum])					 \ */
/* OBSOLETE       write_register (regnum, read_memory_integer (fsr.regs[regnum], 4)); \ */
/* OBSOLETE   write_register (FP_REGNUM, read_memory_integer (fp, 4));	 \ */
/* OBSOLETE   write_register (PC_REGNUM, read_memory_integer (fp + 4, 4));   \ */
/* OBSOLETE   write_register (SP_REGNUM, fp + 8);				 \ */
/* OBSOLETE   flush_cached_frames ();					 \ */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* This sequence of words is the instructions */
/* OBSOLETE    enter        0xff,0          82 ff 00 */
/* OBSOLETE    jsr  @0x00010203     7f ae c0 01 02 03 */
/* OBSOLETE    adjspd       0x69696969      7f a5 01 02 03 04 */
/* OBSOLETE    bpt                  f2 */
/* OBSOLETE    Note this is 16 bytes.  */ */
/* OBSOLETE  */
/* OBSOLETE #define CALL_DUMMY { 0x7f00ff82, 0x0201c0ae, 0x01a57f03, 0xf2040302 } */
/* OBSOLETE  */
/* OBSOLETE #define CALL_DUMMY_START_OFFSET	3 */
/* OBSOLETE #define CALL_DUMMY_LENGTH	16 */
/* OBSOLETE #define CALL_DUMMY_ADDR		5 */
/* OBSOLETE #define CALL_DUMMY_NARGS	11 */
/* OBSOLETE  */
/* OBSOLETE /* Insert the specified number of args and function address */
/* OBSOLETE    into a call sequence of the above form stored at DUMMYNAME.  */ */
/* OBSOLETE  */
/* OBSOLETE #define FIX_CALL_DUMMY(dummyname, pc, fun, nargs, args, type, gcc_p)   		\ */
/* OBSOLETE { int flipped = fun | 0xc0000000;				\ */
/* OBSOLETE   flip_bytes (&flipped, 4);					\ */
/* OBSOLETE   *((int *) (((char *) dummyname)+CALL_DUMMY_ADDR)) = flipped;	\ */
/* OBSOLETE   flipped = - nargs * 4;					\ */
/* OBSOLETE   flip_bytes (&flipped, 4);					\ */
/* OBSOLETE   *((int *) (((char *) dummyname)+CALL_DUMMY_NARGS)) = flipped;	\ */
/* OBSOLETE } */
