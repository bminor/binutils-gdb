// OBSOLETE /* Parameters for execution on a z8000 series machine.
// OBSOLETE 
// OBSOLETE    Copyright 1992, 1993, 1994, 1998, 1999, 2000, 2001, 2002 Free
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
// OBSOLETE /* NOTE: cagney/2002-11-24: This is a guess.  */
// OBSOLETE #define DEPRECATED_USE_GENERIC_DUMMY_FRAMES 0
// OBSOLETE #define CALL_DUMMY_LOCATION ON_STACK
// OBSOLETE #define DEPRECATED_PC_IN_CALL_DUMMY(pc, sp, frame_address) deprecated_pc_in_call_dummy_on_stack (pc, sp, frame_address)
// OBSOLETE 
// OBSOLETE #undef TARGET_INT_BIT
// OBSOLETE #undef TARGET_LONG_BIT
// OBSOLETE #undef TARGET_SHORT_BIT
// OBSOLETE #undef TARGET_PTR_BIT
// OBSOLETE 
// OBSOLETE #define TARGET_SHORT_BIT 16
// OBSOLETE #define TARGET_INT_BIT 16
// OBSOLETE #define TARGET_LONG_BIT 32
// OBSOLETE #define TARGET_PTR_BIT (BIG ? 32: 16)
// OBSOLETE 
// OBSOLETE /* Offset from address of function to start of its code.
// OBSOLETE    Zero on most machines.  */
// OBSOLETE 
// OBSOLETE #define FUNCTION_START_OFFSET 0
// OBSOLETE 
// OBSOLETE /* Advance PC across any function entry prologue instructions
// OBSOLETE    to reach some "real" code.  */
// OBSOLETE 
// OBSOLETE #define SKIP_PROLOGUE(ip)   (z8k_skip_prologue (ip))
// OBSOLETE extern CORE_ADDR z8k_skip_prologue (CORE_ADDR ip);
// OBSOLETE 
// OBSOLETE 
// OBSOLETE /* Immediately after a function call, return the saved pc.
// OBSOLETE    Can't always go through the frames for this because on some machines
// OBSOLETE    the new frame is not set up until the new function executes
// OBSOLETE    some instructions.  */
// OBSOLETE 
// OBSOLETE 
// OBSOLETE extern int z8k_saved_pc_after_call (struct frame_info *frame);
// OBSOLETE #define SAVED_PC_AFTER_CALL(frame) z8k_saved_pc_after_call(frame)
// OBSOLETE 
// OBSOLETE /* Stack grows downward.  */
// OBSOLETE 
// OBSOLETE #define INNER_THAN(lhs,rhs) ((lhs) < (rhs))
// OBSOLETE 
// OBSOLETE /* Sequence of bytes for breakpoint instruction. */
// OBSOLETE 
// OBSOLETE #define BREAKPOINT {0x36,0x00}
// OBSOLETE 
// OBSOLETE /* If your kernel resets the pc after the trap happens you may need to
// OBSOLETE    define this before including this file.  */
// OBSOLETE 
// OBSOLETE #define DECR_PC_AFTER_BREAK 0
// OBSOLETE 
// OBSOLETE /* Say how long registers are.  */
// OBSOLETE 
// OBSOLETE #define REGISTER_TYPE unsigned int
// OBSOLETE 
// OBSOLETE #define NUM_REGS 	23	/* 16 registers + 1 ccr + 1 pc + 3 debug
// OBSOLETE 				   regs + fake fp + fake sp */
// OBSOLETE #define REGISTER_BYTES  (NUM_REGS *4)
// OBSOLETE 
// OBSOLETE /* Index within `registers' of the first byte of the space for
// OBSOLETE    register N.  */
// OBSOLETE 
// OBSOLETE #define REGISTER_BYTE(N)  ((N)*4)
// OBSOLETE 
// OBSOLETE /* Number of bytes of storage in the actual machine representation
// OBSOLETE    for register N.  On the z8k, all but the pc are 2 bytes, but we
// OBSOLETE    keep them all as 4 bytes and trim them on I/O */
// OBSOLETE 
// OBSOLETE 
// OBSOLETE #define REGISTER_RAW_SIZE(N) (((N) < 16)? 2:4)
// OBSOLETE 
// OBSOLETE /* Number of bytes of storage in the program's representation
// OBSOLETE    for register N.  */
// OBSOLETE 
// OBSOLETE #define REGISTER_VIRTUAL_SIZE(N) REGISTER_RAW_SIZE(N)
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
// OBSOLETE #define REGISTER_VIRTUAL_TYPE(N) \
// OBSOLETE  (REGISTER_VIRTUAL_SIZE(N) == 2? builtin_type_unsigned_int : builtin_type_long)
// OBSOLETE 
// OBSOLETE #define DEPRECATED_INIT_FRAME_PC(x,y) (init_frame_pc_noop (x, y))
// OBSOLETE #define INIT_FRAME_PC_FIRST(x,y) (init_frame_pc_noop (x, y))
// OBSOLETE 
// OBSOLETE #define REGISTER_NAMES  \
// OBSOLETE  {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", \
// OBSOLETE   "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15", \
// OBSOLETE   "ccr", "pc", "cycles","insts","time","fp","sp"}
// OBSOLETE 
// OBSOLETE /* Register numbers of various important registers.
// OBSOLETE    Note that some of these values are "real" register numbers,
// OBSOLETE    and correspond to the general registers of the machine,
// OBSOLETE    and some are "phony" register numbers which are too large
// OBSOLETE    to be actual register numbers as far as the user is concerned
// OBSOLETE    but do serve to get the desired values when passed to read_register.  */
// OBSOLETE 
// OBSOLETE #define CCR_REGNUM 	16	/* Contains processor status */
// OBSOLETE #define PC_REGNUM 	17	/* Contains program counter */
// OBSOLETE #define CYCLES_REGNUM 	18
// OBSOLETE #define INSTS_REGNUM 	19
// OBSOLETE #define TIME_REGNUM 	20
// OBSOLETE #define FP_REGNUM 	21	/* Contains fp, whatever memory model */
// OBSOLETE #define SP_REGNUM 	22	/* Conatins sp, whatever memory model */
// OBSOLETE 
// OBSOLETE 
// OBSOLETE 
// OBSOLETE #define PTR_SIZE (BIG ? 4: 2)
// OBSOLETE #define PTR_MASK (BIG ? 0xff00ffff : 0x0000ffff)
// OBSOLETE 
// OBSOLETE /* Store the address of the place in which to copy the structure the
// OBSOLETE    subroutine will return.  This is called from call_function. */
// OBSOLETE 
// OBSOLETE #define STORE_STRUCT_RETURN(ADDR, SP) internal_error (__FILE__, __LINE__, "failed internal consistency check");
// OBSOLETE 
// OBSOLETE /* Extract from an array REGBUF containing the (raw) register state
// OBSOLETE    a function return value of type TYPE, and copy that, in virtual format,
// OBSOLETE    into VALBUF.  This is assuming that floating point values are returned
// OBSOLETE    as doubles in d0/d1.  */
// OBSOLETE 
// OBSOLETE 
// OBSOLETE #define DEPRECATED_EXTRACT_RETURN_VALUE(TYPE,REGBUF,VALBUF) \
// OBSOLETE   memcpy(VALBUF, REGBUF + REGISTER_BYTE(2), TYPE_LENGTH(TYPE));
// OBSOLETE 
// OBSOLETE /* Write into appropriate registers a function return value
// OBSOLETE    of type TYPE, given in virtual format. */
// OBSOLETE 
// OBSOLETE #define DEPRECATED_STORE_RETURN_VALUE(TYPE,VALBUF) internal_error (__FILE__, __LINE__, "failed internal consistency check");
// OBSOLETE 
// OBSOLETE /* Extract from an array REGBUF containing the (raw) register state
// OBSOLETE    the address in which a function should return its structure value,
// OBSOLETE    as a CORE_ADDR (or an expression that can be used as one).  */
// OBSOLETE 
// OBSOLETE #define DEPRECATED_EXTRACT_STRUCT_VALUE_ADDRESS(REGBUF) (*(CORE_ADDR *)(REGBUF))
// OBSOLETE 
// OBSOLETE /* Describe the pointer in each stack frame to the previous stack frame
// OBSOLETE    (its caller).  */
// OBSOLETE 
// OBSOLETE /* FRAME_CHAIN takes a frame's nominal address and produces the frame's
// OBSOLETE    chain-pointer.
// OBSOLETE    In the case of the Z8000, the frame's nominal address
// OBSOLETE    is the address of a ptr sized byte word containing the calling
// OBSOLETE    frame's address.  */
// OBSOLETE 
// OBSOLETE extern CORE_ADDR z8k_frame_chain (struct frame_info *thisframe);
// OBSOLETE #define FRAME_CHAIN(thisframe) z8k_frame_chain(thisframe);
// OBSOLETE 
// OBSOLETE 
// OBSOLETE 
// OBSOLETE /* Define other aspects of the stack frame.  */
// OBSOLETE 
// OBSOLETE /* A macro that tells us whether the function invocation represented
// OBSOLETE    by FI does not have a frame on the stack associated with it.  If it
// OBSOLETE    does not, FRAMELESS is set to 1, else 0.  */
// OBSOLETE #define FRAMELESS_FUNCTION_INVOCATION(FI) \
// OBSOLETE   (frameless_look_for_prologue (FI))
// OBSOLETE 
// OBSOLETE extern CORE_ADDR z8k_frame_saved_pc (struct frame_info *frame);
// OBSOLETE #define FRAME_SAVED_PC(FRAME) z8k_frame_saved_pc(FRAME)
// OBSOLETE 
// OBSOLETE #define FRAME_ARGS_ADDRESS(fi) ((fi)->frame)
// OBSOLETE 
// OBSOLETE #define FRAME_LOCALS_ADDRESS(fi) ((fi)->frame)
// OBSOLETE 
// OBSOLETE /* Set VAL to the number of args passed to frame described by FI.
// OBSOLETE    Can set VAL to -1, meaning no way to tell.  */
// OBSOLETE 
// OBSOLETE /* We can't tell how many args there are
// OBSOLETE    now that the C compiler delays popping them.  */
// OBSOLETE #if !defined (FRAME_NUM_ARGS)
// OBSOLETE #define FRAME_NUM_ARGS(fi) (-1)
// OBSOLETE #endif
// OBSOLETE 
// OBSOLETE /* Return number of bytes at start of arglist that are not really args.  */
// OBSOLETE 
// OBSOLETE #define FRAME_ARGS_SKIP 8
// OBSOLETE 
// OBSOLETE struct frame_info;
// OBSOLETE extern void z8k_frame_init_saved_regs (struct frame_info *);
// OBSOLETE #define FRAME_INIT_SAVED_REGS(fi) z8k_frame_init_saved_regs (fi)
// OBSOLETE 
// OBSOLETE 
// OBSOLETE /* Things needed for making the inferior call functions.
// OBSOLETE    It seems like every m68k based machine has almost identical definitions
// OBSOLETE    in the individual machine's configuration files.  Most other cpu types
// OBSOLETE    (mips, i386, etc) have routines in their *-tdep.c files to handle this
// OBSOLETE    for most configurations.  The m68k family should be able to do this as
// OBSOLETE    well.  These macros can still be overridden when necessary.  */
// OBSOLETE 
// OBSOLETE /* The CALL_DUMMY macro is the sequence of instructions, as disassembled
// OBSOLETE    by gdb itself:
// OBSOLETE 
// OBSOLETE    fmovemx fp0-fp7,sp@-                 0xf227 0xe0ff
// OBSOLETE    moveml d0-a5,sp@-                    0x48e7 0xfffc
// OBSOLETE    clrw sp@-                            0x4267
// OBSOLETE    movew ccr,sp@-                               0x42e7
// OBSOLETE 
// OBSOLETE    /..* The arguments are pushed at this point by GDB;
// OBSOLETE    no code is needed in the dummy for this.
// OBSOLETE    The CALL_DUMMY_START_OFFSET gives the position of 
// OBSOLETE    the following jsr instruction.  *../
// OBSOLETE 
// OBSOLETE    jsr @#0x32323232                     0x4eb9 0x3232 0x3232
// OBSOLETE    addal #0x69696969,sp                 0xdffc 0x6969 0x6969
// OBSOLETE    trap #<your BPT_VECTOR number here>  0x4e4?
// OBSOLETE    nop                                  0x4e71
// OBSOLETE 
// OBSOLETE    Note this is CALL_DUMMY_LENGTH bytes (28 for the above example).
// OBSOLETE    We actually start executing at the jsr, since the pushing of the
// OBSOLETE    registers is done by PUSH_DUMMY_FRAME.  If this were real code,
// OBSOLETE    the arguments for the function called by the jsr would be pushed
// OBSOLETE    between the moveml and the jsr, and we could allow it to execute through.
// OBSOLETE    But the arguments have to be pushed by GDB after the PUSH_DUMMY_FRAME is
// OBSOLETE    done, and we cannot allow the moveml to push the registers again lest
// OBSOLETE    they be taken for the arguments.  */
// OBSOLETE 
// OBSOLETE 
// OBSOLETE #define CALL_DUMMY { 0 }
// OBSOLETE #define CALL_DUMMY_LENGTH 24	/* Size of CALL_DUMMY */
// OBSOLETE #define CALL_DUMMY_START_OFFSET 8	/* Offset to jsr instruction */
// OBSOLETE 
// OBSOLETE 
// OBSOLETE /* Insert the specified number of args and function address
// OBSOLETE    into a call sequence of the above form stored at DUMMYNAME.
// OBSOLETE    We use the BFD routines to store a big-endian value of known size.  */
// OBSOLETE 
// OBSOLETE #define FIX_CALL_DUMMY(dummyname, pc, fun, nargs, args, type, gcc_p)     \
// OBSOLETE { bfd_putb32 (fun,     (char *) dummyname + CALL_DUMMY_START_OFFSET + 2);  \
// OBSOLETE   bfd_putb32 (nargs*4, (char *) dummyname + CALL_DUMMY_START_OFFSET + 8); }
// OBSOLETE 
// OBSOLETE /* Push an empty stack frame, to record the current PC, etc.  */
// OBSOLETE 
// OBSOLETE #define PUSH_DUMMY_FRAME	{ z8k_push_dummy_frame (); }
// OBSOLETE 
// OBSOLETE extern void z8k_push_dummy_frame (void);
// OBSOLETE 
// OBSOLETE extern void z8k_pop_frame (void);
// OBSOLETE 
// OBSOLETE /* Discard from the stack the innermost frame, restoring all registers.  */
// OBSOLETE 
// OBSOLETE #define POP_FRAME		{ z8k_pop_frame (); }
// OBSOLETE 
// OBSOLETE /* Offset from SP to first arg on stack at first instruction of a function */
// OBSOLETE 
// OBSOLETE #define SP_ARG0 (1 * 4)
// OBSOLETE 
// OBSOLETE extern CORE_ADDR z8k_addr_bits_remove (CORE_ADDR);
// OBSOLETE #define ADDR_BITS_REMOVE(addr) z8k_addr_bits_remove (addr)
// OBSOLETE int sim_z8001_mode;
// OBSOLETE #define BIG (sim_z8001_mode)
// OBSOLETE 
// OBSOLETE #define read_memory_short(x)  (read_memory_integer(x,2) & 0xffff)
// OBSOLETE 
// OBSOLETE #define NO_STD_REGS
// OBSOLETE 
// OBSOLETE extern void z8k_do_registers_info (int regnum, int all);
// OBSOLETE #define DEPRECATED_DO_REGISTERS_INFO(REGNUM,ALL) \
// OBSOLETE 	z8k_do_registers_info (REGNUM, ALL)
// OBSOLETE 
// OBSOLETE extern void z8k_set_pointer_size (int newsize);
// OBSOLETE #define INIT_EXTRA_SYMTAB_INFO \
// OBSOLETE   z8k_set_pointer_size(objfile->obfd->arch_info->bits_per_address);
// OBSOLETE 
// OBSOLETE #define REGISTER_SIZE 4
