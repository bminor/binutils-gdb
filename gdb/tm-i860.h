/* Parameters for execution on the Intel I860 for GDB, the GNU debugger.
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

#ifndef i860
#define i860 1
#endif

#define HAVE_TERMIO
#define vfork fork

/* Debugger information will be in COFF format.  */

/* #define COFF_FORMAT */
#define COFF_NO_LONG_FILE_NAMES

/* Offset from address of function to start of its code.
   Zero on most machines.  */

#define FUNCTION_START_OFFSET 0

/* The call instruction puts its return address in register r1 and does
   not change the stack pointer */

#define RETURN_ADDR_IN_REGISTER

/* Advance PC across any function entry prologue instructions
   to reach some "real" code.  */

#define SKIP_PROLOGUE(pc) \
  { pc = skip_prologue (pc); }

/* Immediately after a function call, return the saved pc.
   Can't go through the frames for this because on some machines
   the new frame is not set up until the new function executes
   some instructions.  */

#define SAVED_PC_AFTER_CALL(frame) \
	(read_register(RP_REGNUM))

/* Address of end of stack space.  */

#define STACK_END_ADDR 0xfc000000

/* Stack grows downward.  */

#define INNER_THAN <

/* Stack has strict alignment.  */

#define ALIGN_STACK(ADDR) (((ADDR)+15)&-16)


/* Sequence of bytes for breakpoint instruction.  */
#if defined(i860) && defined(BIG_ENDIAN)
#define BREAKPOINT {0x44, 0x00, 0x00, 0x00}
#define BREAKFLOAT {0x22, 0x06, 0x00, 0x48}
#else
#define BREAKPOINT {0x00, 0x00, 0x00, 0x44}
#define BREAKFLOAT {0x48, 0x00, 0x06, 0x22}
#endif
/* Amount PC must be decremented by after a breakpoint.
   This is often the number of bytes in BREAKPOINT
   but not always.  */

#define DECR_PC_AFTER_BREAK 0

/* Nonzero if instruction at PC is a return instruction.  */
/* should be "bri r1"   */
#define ABOUT_TO_RETURN(pc) \
  (adj_read_memory_integer (pc) == 0x40000800)


/* Return 1 if P points to an invalid floating point value.  */

#define INVALID_FLOAT(p, len) 0   /* Just a first guess; not checked */

/* Largest integer type */
#define LONGEST long

/* Name of the builtin type for the LONGEST type above. */
#define BUILTIN_TYPE_LONGEST builtin_type_long

/* Say how long (ordinary) registers are.  */

#define REGISTER_TYPE long

/* Number of machine registers */

#define NUM_REGS i860REGCOUNT

/* Initializer for an array of names of registers.
   There should be NUM_REGS strings in this initializer.  */
#define REGISTER_NAMES  						\
{									\
"psr", "fir", "trap", "dbrk", 						\
"r0",  "r1",  "sp",  "fp",  "r4",  "r5",  "r6",  "r7",			\
"r8",  "r9",  "r10", "r11", "r12", "r13", "r14", "r15",			\
"r16", "r17", "r18", "r19", "r20", "r21", "r22", "r23",			\
"r24", "r25", "r26", "r27", "r28", "r29", "r30", "r31",			\
									\
"f0",  "f1",  "f2",  "f3",  "f4",  "f5",  "f6",  "f7",			\
"f8",  "f9",  "f10", "f11", "f12", "f13", "f14", "f15",			\
"f16", "f17", "f18", "f19", "f20", "f21", "f22", "f23",			\
"f24", "f25", "f26", "f27", "f28", "f29", "f30", "f31",			\
									\
"fsr",									\
"fsr1",			"fsr2",			"fsr3",			\
"mul1", "",		"mul2", "",		"mul3", "",		\
"add1", "",		"add2", "",		"add3", "",		\
"KI", "",		"KR", "",		"T",  "",		\
"MERGE", "",								\
"pfld1", "", "", "",	"pfld2", "", "", "", 	"pfld3", "", "", "",	\
"graph1", "", 								\
"fpe_fp1","",		"fpe_fp2", "", 		"fpe_fp3", "",  	\
"fpe_fp4", "",								\
"ieee_stat", "fpe_trapped_op", 						\
"fpe_fp5","",		"fpe_fp6", "",					\
"fpe_rfsr", "","","",							\
"epsr",									\
"ccr", "bear", "err"							\
}
/* Register numbers of various important registers.
   Note that some of these values are "real" register numbers,
   and correspond to the general registers of the machine,
   and some are "phony" register numbers which are too large
   to be actual register numbers as far as the user is concerned
   but do serve to get the desired values when passed to read_register.  */

#define RP_REGNUM R1		/* Contains return address value */
#define SP_REGNUM SP		/* Contains address of top of stack, */
#define FP_REGNUM FP		/* Contains address of executing stack frame */
				/* which is also the bottom of the frame.  */
#define RTRN RP_REGNUM
#define R_FP   FP             /* used by dwarfread.c */
#define Y_REGNUM R31		/* Temp register for address calc., etc.  */
#define PC_REGNUM PC		/* Contains program counter */
#define PS_REGNUM PSR		/* Contains processor status */
#define FP0_REGNUM FREGS	/* Floating point register 0 */
#define FPS_REGNUM FSR		/* Floating point status register */
#define DB_REGNUM DB		/* Debug register */

/* Total amount of space needed to store our copies of the machine's
   register state, the array `registers'.  */
#define REGISTER_BYTES (NUM_REGS * 4)

/* Index within `registers' of the first byte of the space for
   register N.  */

#define REGISTER_BYTE(N) ((N)*4)

/* Number of bytes of storage in the actual machine representation
   for register N.  */

/* On the i860, all regs are 4 bytes.  */

#define REGISTER_RAW_SIZE(N) (4)

/* Number of bytes of storage in the program's representation
   for register N.  */

/* On the i860, all regs are 4 bytes.  */

#define REGISTER_VIRTUAL_SIZE(N) (4)

/* Largest value REGISTER_RAW_SIZE can have.  */

#define MAX_REGISTER_RAW_SIZE 8

/* Largest value REGISTER_VIRTUAL_SIZE can have.  */

#define MAX_REGISTER_VIRTUAL_SIZE 8

/* Nonzero if register N requires conversion
   from raw format to virtual format.  */

#define REGISTER_CONVERTIBLE(N) (0)

/* Convert data from raw format for register REGNUM
   to virtual format for register REGNUM.  */

#define REGISTER_CONVERT_TO_VIRTUAL(REGNUM,FROM,TO) \
{ bcopy ((FROM), (TO), 4); }

/* Convert data from virtual format for register REGNUM
   to raw format for register REGNUM.  */

#define REGISTER_CONVERT_TO_RAW(REGNUM,FROM,TO)	\
{ bcopy ((FROM), (TO), 4); }

/* Return the GDB type object for the "standard" data type
   of data in register N.  */

#define REGISTER_VIRTUAL_TYPE(N) \
 ((N) < F0 ? builtin_type_int : (N) <= F31 ? builtin_type_float : builtin_type_int)

/* Store the address of the place in which to copy the structure the
   subroutine will return.  This is called from call_function. */

#define STORE_STRUCT_RETURN(ADDR, SP)  { write_register (R16, (ADDR)); }

/* Extract from an array REGBUF containing the (raw) register state
   a function return value of type TYPE, and copy that, in virtual format,
   into VALBUF.  */

#define EXTRACT_RETURN_VALUE(TYPE,REGBUF,VALBUF) \
 i860_extract_return_value(TYPE, REGBUF, VALBUF)

/* Write into appropriate registers a function return value
   of type TYPE, given in virtual format.  */
/* On i860, values are returned in register r16.  */
#define STORE_RETURN_VALUE(TYPE,VALBUF) \
   i860_store_return_value(TYPE, VALBUF)

/* Extract from an array REGBUF containing the (raw) register state
   the address in which a function should return its structure value,
   as a CORE_ADDR (or an expression that can be used as one).  */

#define EXTRACT_STRUCT_VALUE_ADDRESS(REGBUF) \
  (*(int *) ((REGBUF) + REGISTER_BYTE(R16)))


/* Describe the pointer in each stack frame to the previous stack frame
   (its caller).  */
#include <sys/types.h>
#include <sys/regset.h>
#include "i860_reg.h"

/* FRAME_CHAIN takes a frame's nominal address
   and produces the frame's chain-pointer.

   FRAME_CHAIN_COMBINE takes the chain pointer and the frame's nominal address
   and produces the nominal address of the caller frame.

   However, if FRAME_CHAIN_VALID returns zero,
   it means the given frame is the outermost one and has no caller.
   In that case, FRAME_CHAIN_COMBINE is not used.  */

/* In the case of the i860, the frame-chain's nominal address
   is held in the frame pointer register.

   On the i860 the frame (in %fp) points to %fp for the previous frame.
 */

/* our FRAME_CHAIN requires a pointer to all the frame info (e.g. pc)

 */

#define FRAME_CHAIN(thisframe)  frame_chain(thisframe)

#define FRAME_CHAIN_COMBINE(chain, thisframe) (chain)

/* Define other aspects of the stack frame.  */

#define FRAME_SAVED_PC(frame_struct) frame_saved_pc(frame_struct)

#define FRAME_ARGS_ADDRESS(fi) ((fi)->frame)

#define FRAME_LOCALS_ADDRESS(fi) ((fi)->frame)

/* Set VAL to the number of args passed to frame described by FI.
   Can set VAL to -1, meaning no way to tell.  */

/* We can't tell how many args there are */
 
#define FRAME_NUM_ARGS(val,fi) (val = -1)

#define FRAME_STRUCT_ARGS_ADDRESS(fi) ((fi)->frame)

/*	extra stuff in frame structure to support
 *	backtraceing thru non fp frame based frames
 *	(trace stuff thats not -g compiled)
 */

#define EXTRA_FRAME_INFO int is_minus_g; CORE_ADDR sp; CORE_ADDR rp;

#define INIT_EXTRA_FRAME_INFO(fromleaf,fci)  (fci)->is_minus_g = 0; (fci)->sp = 0; (fci)->rp = 0; 

/* Return number of bytes at start of arglist that are not really args.  */

#define FRAME_ARGS_SKIP 8

/* Put here the code to store, into a struct frame_saved_regs,
   the addresses of the saved registers of frame described by FRAME_INFO.
   This includes special registers such as pc and fp saved in special
   ways in the stack frame.  sp is even more special:
   the address we return for it IS the sp for the next frame.  */

/* Grind through the various st.l rx,Y(fp) and fst.z fx,Y(fp) */

#define FRAME_FIND_SAVED_REGS(frame_info, frame_saved_regs)	\
	frame_find_saved_regs(frame_info, &(frame_saved_regs))

/* Things needed for making the inferior call functions.  */

/* Push an empty stack frame, to record the current PC, etc.  */
/* We have this frame with fp pointing to a block where all GDB-visible
   registers are stored in the order GDB knows them, and sp at the next
   alignment point below fp.  Note: fp + NUM_REGS*4 was the old sp
   */

#define PUSH_DUMMY_FRAME i860_push_frame()

/* Discard from the stack the innermost frame, 
   restoring all saved registers.  */

#define POP_FRAME  i860_pop_frame()

/* This sequence of words is the instructions:
	*../
--
     nop
     calli r31
     nop
     trap r0,r0,r0
--
Note this is 16 bytes.
Saving of the registers is done by PUSH_DUMMY_FRAME.  The address of the
function to call will be placed in register r31 prior to starting.
The arguments have to be put into the parameter registers by GDB after 
the PUSH_DUMMY_FRAME is done.  NOTE: GDB expects to push arguments, but
i860 takes them in registers */
#ifdef BIG_ENDIAN
/* account for reversal of instructions in memory */

#define CALL_DUMMY { \
     0x4c00f802, 0xa0000000,  0x44000000, 0xa0000000 }
#else
#define CALL_DUMMY { \
	0xa0000000,	0x4c00F802,	0xa0000000,	0x44000000 }
#endif

/* This setup is somewhat unusual.  PUSH_DUMMY_FRAME and 
   FRAME_FIND_SAVED_REGS conspire to handle dummy frames differently.
   Therefore, CALL_DUMMY can be minimal. We put the address of the called
   function in r31 and let'er rip */

#define CALL_DUMMY_LENGTH 16

/* Actually start at the calli */

#define CALL_DUMMY_START_OFFSET 4

/* Normally, GDB will patch the dummy code here to put in the function
   address, etc., but we only need to put the call adddress in r31 */
#define FIX_CALL_DUMMY(dummyname, pc, fun, nargs, args, type, gcc_p) \
	write_register(R31, fun)


/* i860 has no reliable single step ptrace call */

#define NO_SINGLE_STEP 1

#define KERNEL_U_ADDR 0xfc7fe530
#define REGISTER_U_ADDR(addr,ar0,regno) 	\
	addr = (REGISTER_BYTE(regno) + ar0 );

/* How many bytes are pushed on the stack for the argument list
 */
#define STACK_ARG_BYTES(RESULT,ARGS,NARGS,STRUCT_RETURN) {(RESULT) = 0;}

/* Pass arguments to a function
 */

#define PUSH_ARGUMENTS(NARGS,ARGS,STACKP,STRUCT_RETURN,STRUCT_ADDR)       \
  {    \
   STACKP = (STACKP) & (-16);                                               \
   STACKP = i860_arg_coerce(NARGS, ARGS, STRUCT_RETURN,STACKP);                                      \
   STACKP =pass_function_arguments(ARGS,NARGS,STRUCT_RETURN, STRUCT_ADDR, STACKP);   }

#define CALL_DUMMY_LOCATION AFTER_TEXT_END

#define NEED_TEXT_START_END

#define NONSTANDARD_MEMORY_LAYOUT

/* We support pseudo-frame structure so actual frame pointer value must be 
 * read from the register.
 */
#define FP_FROM_REGISTER

/* Support a quit from info register command because we have so many regs
 */
#define INFO_REGISTER_QUIT

/* Identify text or absolute symbols to put in misc function table.  In the
 * i860 case, only text symbols not starting with _L00
 */
#define MISC_FUNCTION(cs) ((cs)->c_secnum == 1 && (cs)->c_value && \
			strncmp((cs)->c_name, "_L00", 4) != 0)

/* Define our a.out magic number and undefine the 386 one if it's defined
 */
#ifdef I860MAGIC
#define AOUT_MAGIC I860MAGIC
#else
#define AOUT_MAGIC 0515
#endif

#ifdef I386MAGIC
#undef I386MAGIC
#endif

/* The aouthdr may be larger than defined in a.out.h
 */
#define AOUTHDR_VARIABLE

/* We have a bug where lineno information is not in symbol table correctly
 */
#define LINENO_BUG

/* Similarly, the Endndx of a structure definition is bogus in ld860
 */
	/* #define STRUCT_SYMBOL_BUG */

/* Also, we see T_ARG types when we shouldn't
 */
#define T_ARG_BUG

/* Macro to sign extend to 32 bits */
#define SIGN_EXT(n,x)	(((int) (x) << (32-n)) >> (32-n))

#define SIGN_EXT16(x)	(((int) (x) << (16)) >> (16))

#define FRAMELESS_FUNCTION_INVOCATION(FI,FRAMELESS) frameless_look_for_prologue(FI)
#if defined(i860) && defined(BIG_ENDIAN)
#define ADJ_PC(pc)      ((unsigned)pc ^ 4)
#define ADJ_FREG(freg)  (F0 + (((freg) - F0) ^ 1))
#define ADJ_FREG_BYTE(freg) (REGISTER_BYTE(F0) + (((freg) - REGISTER_BYTE(F0)) ^ REGISTER_BYTE(1)))
#else
#define ADJ_PC(pc)      (pc)
#define ADJ_FREG(freg)  (freg)
#define ADJ_FREG_BYTE(freg) (freg)
#endif
/* define DO_REGISTERS_INFO to do i860-specific formatting */
#define DO_REGISTERS_INFO(regnum,fpregs) i860_do_registers_info(regnum,fpregs)
/* the two low order bits of the return pointer are ignored when returning; some systems store information
   in them.  To avoid confusing the stack trace, provide this macro to "clean" the two low order bits */
#define CLEAN_PC(pc)   ((unsigned) pc & 0xfffffffc) 
#define adj_target_read_memory(memaddr,myaddr,len) target_read_memory(ADJ_PC(memaddr),myaddr,len)
#define adj_target_write_memory(memaddr,myaddr,len) target_write_memory(ADJ_PC(memaddr),myaddr,len)
#define adj_read_memory_integer(memaddr)     read_memory_integer(ADJ_PC(memaddr),4)
#define adj_read_memory(memaddr,myaddr,len)  read_memory(ADJ_PC(memaddr),myaddr,len)
#define adj_write_memory(memaddr,myaddr,len) write_memory(ADJ_PC(memaddr),myaddr,len)
#define INIT_FRAME_PC(fromleaf, prev) \
 { struct frame_info *nextframe = prev->next; \
 if (nextframe->rp) \
   { prev->pc = CLEAN_PC( nextframe ->rp); \
  prev->rp = 0;	} \
  else    prev->pc = CLEAN_PC((fromleaf ? SAVED_PC_AFTER_CALL (prev->next) : \
	      prev->next ? FRAME_SAVED_PC (prev->next) : read_pc ())); }

#define INTERNAL_REGISTER_VAL(regno)    regno+R0
/* used to indicate that the misc function vector has size information about
   misc functions -- currently true for elf format only -- mlf */
#define MISC_FUNCTIONS_HAS_SIZE
#define FRAME_GET_BASEREG_VALUE(frame,basereg)   get_saved_basereg(frame, basereg);

