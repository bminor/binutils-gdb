/* Definitions to make GDB run on a vax under 4.2bsd.
   Copyright 1986, 1987, 1989, 1991, 1993, 1994, 1996, 1998, 1999, 2000, 2002
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

/* Offset from address of function to start of its code.
   Zero on most machines.  */

#define FUNCTION_START_OFFSET 2

/* Advance PC across any function entry prologue instructions
   to reach some "real" code.  */

extern CORE_ADDR vax_skip_prologue (CORE_ADDR);
#define SKIP_PROLOGUE(pc) (vax_skip_prologue (pc))

/* Immediately after a function call, return the saved pc.
   Can't always go through the frames for this because on some machines
   the new frame is not set up until the new function executes
   some instructions.  */

#define SAVED_PC_AFTER_CALL(frame) FRAME_SAVED_PC(frame)

#define TARGET_UPAGES 14
#define TARGET_NBPG 512
#define STACK_END_ADDR (0x80000000 - (TARGET_UPAGES * TARGET_NBPG))

/* On the VAX, sigtramp is in the u area.  Can't check the exact
   addresses because for cross-debugging we don't have VAX include
   files around.  This should be close enough.  */
#define SIGTRAMP_START(pc)	STACK_END_ADDR
#define SIGTRAMP_END(pc)	0x80000000

/* Stack grows downward.  */

#define INNER_THAN(lhs,rhs) core_addr_lessthan ((lhs), (rhs))

/* Sequence of bytes for breakpoint instruction.  */

#define BREAKPOINT {3}

/* Amount PC must be decremented by after a breakpoint.
   This is often the number of bytes in BREAKPOINT
   but not always.  */

#define DECR_PC_AFTER_BREAK 0

/* Say how long (ordinary) registers are.  This is a piece of bogosity
   used in push_word and a few other places; REGISTER_RAW_SIZE is the
   real way to know how big a register is.  */

#define REGISTER_SIZE 4

/* Number of machine registers */

#define NUM_REGS 17

/* Return the name of the register specified by N.  */
#define REGISTER_NAME(N) vax_register_name ((N))
extern char *vax_register_name (int);

/* Register numbers of various important registers.
   Note that some of these values are "real" register numbers,
   and correspond to the general registers of the machine,
   and some are "phony" register numbers which are too large
   to be actual register numbers as far as the user is concerned
   but do serve to get the desired values when passed to read_register.  */

#define AP_REGNUM 12
#define FP_REGNUM 13		/* Contains address of executing stack frame */
#define SP_REGNUM 14		/* Contains address of top of stack */
#define PC_REGNUM 15		/* Contains program counter */
#define PS_REGNUM 16		/* Contains processor status */

/* Total amount of space needed to store our copies of the machine's
   register state, the array `registers'.  */
#define REGISTER_BYTES (17*4)

/* Index within `registers' of the first byte of the space for
   register N.  */
#define REGISTER_BYTE(N) vax_register_byte ((N))
extern int vax_register_byte (int);

/* Number of bytes of storage in the actual machine representation
   for register N.  On the vax, all regs are 4 bytes.  */
#define REGISTER_RAW_SIZE(N) vax_register_raw_size ((N))
extern int vax_register_raw_size (int);

/* Number of bytes of storage in the program's representation
   for register N.  On the vax, all regs are 4 bytes.  */
#define REGISTER_VIRTUAL_SIZE(N) vax_register_virtual_size ((N))
extern int vax_register_virtual_size (int);

/* Largest value REGISTER_RAW_SIZE can have.  */

#define MAX_REGISTER_RAW_SIZE 4

/* Largest value REGISTER_VIRTUAL_SIZE can have.  */

#define MAX_REGISTER_VIRTUAL_SIZE 4

/* Return the GDB type object for the "standard" data type
   of data in register N.  */
#define REGISTER_VIRTUAL_TYPE(N) vax_register_virtual_type ((N))
extern struct type *vax_register_virtual_type (int);

#define STORE_STRUCT_RETURN(ADDR, SP) vax_store_struct_return ((ADDR), (SP))
extern void vax_store_struct_return (CORE_ADDR, CORE_ADDR);

#define EXTRACT_RETURN_VALUE(TYPE,REGBUF,VALBUF) \
  vax_extract_return_value ((TYPE), (REGBUF), (VALBUF))
extern void vax_extract_return_value (struct type *, char *, char *);

#define STORE_RETURN_VALUE(TYPE,VALBUF) \
  vax_store_return_value ((TYPE), (VALBUF))
extern void vax_store_return_value (struct type *, char *);

#define EXTRACT_STRUCT_VALUE_ADDRESS(REGBUF) \
  vax_extract_struct_value_address ((REGBUF))
extern CORE_ADDR vax_extract_struct_value_address (char *);


#define FRAME_CHAIN(FI) vax_frame_chain ((FI))
extern CORE_ADDR vax_frame_chain (struct frame_info *);

#define FRAMELESS_FUNCTION_INVOCATION(FI) \
  generic_frameless_function_invocation_not ((FI))

/* Offset to saved PC in sigcontext, from <sys/signal.h>.  */
/* XXXJRT should go away */
#define SIGCONTEXT_PC_OFFSET 12

#define FRAME_SAVED_PC(FRAME) vax_frame_saved_pc ((FRAME))
extern CORE_ADDR vax_frame_saved_pc (struct frame_info *);

/* XXXJRT not yet under gdbarch control */
#define FRAME_ARGS_ADDRESS_CORRECT(fi) vax_frame_args_address ((fi))
extern CORE_ADDR vax_frame_args_address (struct frame_info *);

#define FRAME_ARGS_ADDRESS(fi) vax_frame_args_address ((fi))
extern CORE_ADDR vax_frame_args_address (struct frame_info *);

#define FRAME_LOCALS_ADDRESS(fi) vax_frame_locals_address ((fi))
extern CORE_ADDR vax_frame_locals_address (struct frame_info *);

extern int vax_frame_num_args (struct frame_info *fi);
#define FRAME_NUM_ARGS(fi) (vax_frame_num_args ((fi)))

/* Return number of bytes at start of arglist that are not really args.  */

#define FRAME_ARGS_SKIP 4

#define FRAME_INIT_SAVED_REGS(fi) vax_frame_init_saved_regs ((fi))
extern void vax_frame_init_saved_regs (struct frame_info *);

#define PUSH_DUMMY_FRAME vax_push_dummy_frame()
extern void vax_push_dummy_frame (void);

#define POP_FRAME vax_pop_frame()
extern void vax_pop_frame (void);

/* This sequence of words is the instructions
   calls #69, @#32323232
   bpt
   Note this is 8 bytes.  */

#define CALL_DUMMY {0x329f69fb, 0x03323232}

#define CALL_DUMMY_START_OFFSET 0	/* Start execution at beginning of dummy */

#define CALL_DUMMY_BREAKPOINT_OFFSET 7

/* Insert the specified number of args and function address
   into a call sequence of the above form stored at DUMMYNAME.  */

#define FIX_CALL_DUMMY(dummyname, pc, fun, nargs, args, type, gcc_p)   \
{ *((char *) dummyname + 1) = nargs;		\
  *(int *)((char *) dummyname + 3) = fun; }

/* If vax pcc says CHAR or SHORT, it provides the correct address.  */

#define BELIEVE_PCC_PROMOTION 1
