/*
 * Changes for 80386 by Pace Willisson (pace@prep.ai.mit.edu)
 * July 1988
 */

/*
   Copyright (C) 1986, 1987 Free Software Foundation, Inc.

GDB is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY.  No author or distributor accepts responsibility to anyone
for the consequences of using it or for whether it serves any
particular purpose or works at all, unless he says so in writing.
Refer to the GDB General Public License for full details.

Everyone is granted permission to copy, modify and redistribute GDB,
but only under the conditions described in the GDB General Public
License.  A copy of this license is supposed to have been given to you
along with GDB so you can know your rights and responsibilities.  It
should be in a file named COPYING.  Among other things, the copyright
notice and this notice must be preserved on all copies.

In other words, go ahead and share GDB, but don't try to stop
anyone else from sharing it farther.  Help stamp out software hoarding!
*/

#ifndef i386
#define i386
#endif

/* define USG if you are using sys5 /usr/include's */
#define USG

/* USG systems need these */
#define vfork() fork()
#define MAXPATHLEN 500

/* define this if you don't have the extension to coff that allows
 * file names to appear in the string table
 * (aux.x_file.x_foff)
 */
#define COFF_NO_LONG_FILE_NAMES

/* turn this on when rest of gdb is ready */
/* #define IEEE_FLOAT */

#define NBPG NBPC
#define UPAGES USIZE

#define HAVE_TERMIO

/* Get rid of any system-imposed stack limit if possible.  */

/* #define SET_STACK_LIMIT_HUGE not in sys5 */

/* Define this if the C compiler puts an underscore at the front
   of external names before giving them to the linker.  */

/* #define NAMES_HAVE_UNDERSCORE */

/* Specify debugger information format.  */

/* #define READ_DBX_FORMAT */
#define COFF_FORMAT

/* number of traps that happen between exec'ing the shell 
 * to run an inferior, and when we finally get to 
 * the inferior code.  This is 2 on most implementations.
 */
#define START_INFERIOR_TRAPS_EXPECTED 4

/* Offset from address of function to start of its code.
   Zero on most machines.  */

#define FUNCTION_START_OFFSET 0

/* Advance PC across any function entry prologue instructions
   to reach some "real" code.  */

#define SKIP_PROLOGUE(frompc)   {(frompc) = i386_skip_prologue((frompc));}

/* Immediately after a function call, return the saved pc.
   Can't always go through the frames for this because on some machines
   the new frame is not set up until the new function executes
   some instructions.  */

#define SAVED_PC_AFTER_CALL(frame) \
  (read_memory_integer (read_register (SP_REGNUM), 4))

/* This is the amount to subtract from u.u_ar0
   to get the offset in the core file of the register values.  */

#define KERNEL_U_ADDR 0xe0000000

/* Address of end of stack space.  */

#define STACK_END_ADDR 0x80000000

/* Stack grows downward.  */

#define INNER_THAN <

/* Sequence of bytes for breakpoint instruction.  */

#define BREAKPOINT {0xcc}

/* Amount PC must be decremented by after a breakpoint.
   This is often the number of bytes in BREAKPOINT
   but not always.  */

#define DECR_PC_AFTER_BREAK 1

/* Nonzero if instruction at PC is a return instruction.  */

#define ABOUT_TO_RETURN(pc) (read_memory_integer (pc, 1) == 0xc3)

/* Return 1 if P points to an invalid floating point value.
   LEN is the length in bytes -- not relevant on the 386.  */

#define INVALID_FLOAT(p, len) (0)

/* code to execute to print interesting information about the
 * floating point processor (if any)
 * No need to define if there is nothing to do.
 */
#define FLOAT_INFO { i386_float_info (); }


/* Largest integer type */
#define LONGEST long

/* Name of the builtin type for the LONGEST type above. */
#define BUILTIN_TYPE_LONGEST builtin_type_long

/* Say how long (ordinary) registers are.  */

#define REGISTER_TYPE long

/* Number of machine registers */

#define NUM_REGS 16

/* Initializer for an array of names of registers.
   There should be NUM_REGS strings in this initializer.  */

/* the order of the first 8 registers must match the compiler's 
 * numbering scheme (which is the same as the 386 scheme)
 * also, this table must match regmap in i386-pinsn.c.
 */
#define REGISTER_NAMES { "eax", "ecx", "edx", "ebx", \
			 "esp", "ebp", "esi", "edi", \
			 "eip", "ps", "cs", "ss", \
			 "ds", "es", "fs", "gs", \
			 }

/* Register numbers of various important registers.
   Note that some of these values are "real" register numbers,
   and correspond to the general registers of the machine,
   and some are "phony" register numbers which are too large
   to be actual register numbers as far as the user is concerned
   but do serve to get the desired values when passed to read_register.  */

#define FP_REGNUM 5		/* Contains address of executing stack frame */
#define SP_REGNUM 4		/* Contains address of top of stack */

#define PC_REGNUM 8
#define PS_REGNUM 9

#define REGISTER_U_ADDR(addr, blockend, regno) \
	(addr) = i386_register_u_addr ((blockend),(regno));

/* Total amount of space needed to store our copies of the machine's
   register state, the array `registers'.  */
#define REGISTER_BYTES (NUM_REGS * 4)

/* Index within `registers' of the first byte of the space for
   register N.  */

#define REGISTER_BYTE(N) ((N)*4)

/* Number of bytes of storage in the actual machine representation
   for register N.  */

#define REGISTER_RAW_SIZE(N) (4)

/* Number of bytes of storage in the program's representation
   for register N. */

#define REGISTER_VIRTUAL_SIZE(N) (4)

/* Largest value REGISTER_RAW_SIZE can have.  */

#define MAX_REGISTER_RAW_SIZE 4

/* Largest value REGISTER_VIRTUAL_SIZE can have.  */

#define MAX_REGISTER_VIRTUAL_SIZE 4

/* Nonzero if register N requires conversion
   from raw format to virtual format.  */

#define REGISTER_CONVERTIBLE(N) (0)

/* Convert data from raw format for register REGNUM
   to virtual format for register REGNUM.  */

#define REGISTER_CONVERT_TO_VIRTUAL(REGNUM,FROM,TO) {bcopy ((FROM), (TO), 4);}

/* Convert data from virtual format for register REGNUM
   to raw format for register REGNUM.  */

#define REGISTER_CONVERT_TO_RAW(REGNUM,FROM,TO) {bcopy ((FROM), (TO), 4);}

/* Return the GDB type object for the "standard" data type
   of data in register N.  */

#define REGISTER_VIRTUAL_TYPE(N) (builtin_type_int)

/* Store the address of the place in which to copy the structure the
   subroutine will return.  This is called from call_function. */

#define STORE_STRUCT_RETURN(ADDR, SP) \
  { (SP) -= sizeof (ADDR);		\
    write_memory ((SP), &(ADDR), sizeof (ADDR)); }

/* Extract from an array REGBUF containing the (raw) register state
   a function return value of type TYPE, and copy that, in virtual format,
   into VALBUF.  */

#define EXTRACT_RETURN_VALUE(TYPE,REGBUF,VALBUF) \
  bcopy (REGBUF, VALBUF, TYPE_LENGTH (TYPE))

/* Write into appropriate registers a function return value
   of type TYPE, given in virtual format.  */

#define STORE_RETURN_VALUE(TYPE,VALBUF) \
  write_register_bytes (0, VALBUF, TYPE_LENGTH (TYPE))

/* Extract from an array REGBUF containing the (raw) register state
   the address in which a function should return its structure value,
   as a CORE_ADDR (or an expression that can be used as one).  */

#define EXTRACT_STRUCT_VALUE_ADDRESS(REGBUF) (*(int *)(REGBUF))


/* Describe the pointer in each stack frame to the previous stack frame
   (its caller).  */

/* FRAME_CHAIN takes a frame's nominal address
   and produces the frame's chain-pointer.

   FRAME_CHAIN_COMBINE takes the chain pointer and the frame's nominal address
   and produces the nominal address of the caller frame.

   However, if FRAME_CHAIN_VALID returns zero,
   it means the given frame is the outermost one and has no caller.
   In that case, FRAME_CHAIN_COMBINE is not used.  */

#define FRAME_CHAIN(thisframe) (read_memory_integer ((thisframe)->frame, 4))

#define FRAME_CHAIN_VALID(chain, thisframe) \
  (chain != 0 && (FRAME_SAVED_PC (thisframe) >= first_object_file_end))

#define FRAME_CHAIN_COMBINE(chain, thisframe) (chain)

/* Define other aspects of the stack frame.  */

#define FRAME_SAVED_PC(FRAME) (read_memory_integer ((FRAME)->frame + 4, 4))

#define FRAME_ARGS_ADDRESS(fi) ((fi)->frame)

#define FRAME_LOCALS_ADDRESS(fi) ((fi)->frame)

/* Return number of args passed to a frame.
   Can return -1, meaning no way to tell.  */

/* on the 386, the instruction following the call could be:
 *  popl %ecx        -  one arg
 *  addl $imm, %esp  -  imm/4 args; imm may be 8 or 32 bits
 *  anything else    -  zero args
 */
#define FRAME_NUM_ARGS(numargs, fi)  { \
  int retpc;						\
  unsigned char op;					\
  struct frame_info *pfi;				\
  pfi = get_prev_frame_info ((fi));			\
  retpc = pfi->pc;					\
  numargs = 0;						\
  op = read_memory_integer (retpc, 1);			\
  if (op == 0x59)					\
	  /* pop %ecx */				\
	  (numargs) = 1;				\
  else if (op == 0x83) {				\
	  op = read_memory_integer (retpc+1, 1);	\
	  if (op == 0xc4)				\
		  /* addl $<signed imm 8 bits>, %esp */	\
		  (numargs) = (read_memory_integer (retpc+2,1)&0xff)/4;\
  } else if (op == 0x81) { /* add with 32 bit immediate */\
	  op = read_memory_integer (retpc+1, 1);	\
	  if (op == 0xc4)				\
		  /* addl $<imm 32>, %esp */		\
		  (numargs) = read_memory_integer (retpc+2, 4) / 4;\
  }							\
}

/* Return number of bytes at start of arglist that are not really args.  */

#define FRAME_ARGS_SKIP 8

/* Put here the code to store, into a struct frame_saved_regs,
   the addresses of the saved registers of frame described by FRAME_INFO.
   This includes special registers such as pc and fp saved in special
   ways in the stack frame.  sp is even more special:
   the address we return for it IS the sp for the next frame.  */

#define FRAME_FIND_SAVED_REGS(frame_info, frame_saved_regs) \
{ i386_frame_find_saved_regs ((frame_info), &(frame_saved_regs)); }


/* Things needed for making the inferior call functions.  */

/* Push an empty stack frame, to record the current PC, etc.  */

#define PUSH_DUMMY_FRAME { i386_push_dummy_frame (); }

/* Discard from the stack the innermost frame, restoring all registers.  */

#define POP_FRAME  { i386_pop_frame (); }

/* this is 
 *   call 11223344 (32 bit relative)
 *   int3
 */

#define CALL_DUMMY { 0x223344e8, 0xcc11 }

#define CALL_DUMMY_LENGTH 8

#define CALL_DUMMY_START_OFFSET 0  /* Start execution at beginning of dummy */

/* Insert the specified number of args and function address
   into a call sequence of the above form stored at DUMMYNAME.  */

#define FIX_CALL_DUMMY(dummyname, pc, fun, nargs, type)   \
{ \
	int from, to, delta, loc; \
	loc = (int)(read_register (SP_REGNUM) - CALL_DUMMY_LENGTH); \
	from = loc + 5; \
	to = (int)(fun); \
	delta = to - from; \
	*(int *)((char *)(dummyname) + 1) = delta; \
}


#if 0
/* Interface definitions for kernel debugger KDB.  */

/* Map machine fault codes into signal numbers.
   First subtract 0, divide by 4, then index in a table.
   Faults for which the entry in this table is 0
   are not handled by KDB; the program's own trap handler
   gets to handle then.  */

#define FAULT_CODE_ORIGIN 0
#define FAULT_CODE_UNITS 4
#define FAULT_TABLE    \
{ 0, 0, 0, 0, 0, 0, 0, 0, \
  0, 0, 0, 0, 0, 0, 0, 0, \
  0, 0, 0, 0, 0, 0, 0, 0}

/* Start running with a stack stretching from BEG to END.
   BEG and END should be symbols meaningful to the assembler.
   This is used only for kdb.  */

#define INIT_STACK(beg, end)  {}

/* Push the frame pointer register on the stack.  */
#define PUSH_FRAME_PTR        {}

/* Copy the top-of-stack to the frame pointer register.  */
#define POP_FRAME_PTR  {}

/* After KDB is entered by a fault, push all registers
   that GDB thinks about (all NUM_REGS of them),
   so that they appear in order of ascending GDB register number.
   The fault code will be on the stack beyond the last register.  */

#define PUSH_REGISTERS        {}

/* Assuming the registers (including processor status) have been
   pushed on the stack in order of ascending GDB register number,
   restore them and return to the address in the saved PC register.  */

#define POP_REGISTERS      {}
#endif
