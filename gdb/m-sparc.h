/* Parameters for execution on a Sun 4, for GDB, the GNU debugger.
   Copyright (C) 1986, 1987, 1989 Free Software Foundation, Inc.
   Contributed by Michael Tiemann (tiemann@mcc.com)
This file is part of GDB.

GDB is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GDB is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GDB; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#ifndef sun4
#define sun4
#endif

/* Define the bit, byte, and word ordering of the machine.  */
#define BITS_BIG_ENDIAN
#define BYTES_BIG_ENDIAN
#define WORDS_BIG_ENDIAN

/* Floating point is IEEE compatible.  */
#define IEEE_FLOAT

/* Get rid of any system-imposed stack limit if possible.  */

#define SET_STACK_LIMIT_HUGE

/* Define this if the C compiler puts an underscore at the front
   of external names before giving them to the linker.  */

#define NAMES_HAVE_UNDERSCORE

/* Debugger information will be in DBX format.  */

#define READ_DBX_FORMAT

/* When passing a structure to a function, Sun cc passes the address
   in a register, not the structure itself.  It (under SunOS4) creates
   two symbols, so we get a LOC_ARG saying the address is on the stack
   (a lie, and a serious one since we don't know which register to
   use), and a LOC_REGISTER saying that the struct is in a register
   (sort of a lie, but fixable with REG_STRUCT_HAS_ADDR).

   This still doesn't work if the argument is not one passed in a
   register (i.e. it's the 7th or later argument).  */
#define REG_STRUCT_HAS_ADDR(gcc_p) (!gcc_p)
#define STRUCT_ARG_SYM_GARBAGE(gcc_p) (!gcc_p)

/* If Pcc says that a parameter is a short, it's a short.  This is
   because the parameter does get passed in in a register as an int,
   but pcc puts it onto the stack frame as a short (not nailing
   whatever else might be there.  I'm not sure that I consider this
   swift.  Sigh.)

   No, don't do this.  The problem here is that pcc says that the
   argument is in the upper half of the word reserved on the stack,
   but puts it in the lower half.  */
/* #define BELIEVE_PCC_PROMOTION 1 */
/* OK, I've added code to dbxread.c to deal with this case.  */
#define BELIEVE_PCC_PROMOTION_TYPE

/* Offset from address of function to start of its code.
   Zero on most machines.  */

#define FUNCTION_START_OFFSET 0

/* Advance PC across any function entry prologue instructions
   to reach some "real" code.  */

#define SKIP_PROLOGUE(pc) \
  { pc = skip_prologue (pc); }

/* Immediately after a function call, return the saved pc.
   Can't go through the frames for this because on some machines
   the new frame is not set up until the new function executes
   some instructions.  */

/* On the Sun 4 under SunOS, the compile will leave a fake insn which
   encodes the structure size being returned.  If we detect such
   a fake insn, step past it.  */

#define PC_ADJUST(pc) ((read_memory_integer (pc + 8, 4) & 0xfffffe00) == 0 ? \
		       pc+12 : pc+8)

#define SAVED_PC_AFTER_CALL(frame) PC_ADJUST (read_register (RP_REGNUM))

/* Address of end of stack space.  */
#include <sys/types.h>
#include <machine/vmparam.h>
#define STACK_END_ADDR USRSTACK

#define INNER_THAN <

/* Stack has strict alignment.  */

#define STACK_ALIGN(ADDR) (((ADDR)+7)&-8)

/* Sequence of bytes for breakpoint instruction.  */

#define BREAKPOINT {0x91, 0xd0, 0x20, 0x01}

/* Amount PC must be decremented by after a breakpoint.
   This is often the number of bytes in BREAKPOINT
   but not always.  */

#define DECR_PC_AFTER_BREAK 0

/* Nonzero if instruction at PC is a return instruction.  */
/* For SPARC, this is either a "jmpl %o7+8,%g0" or "jmpl %i7+8,%g0".

   Note: this does not work for functions returning structures under SunOS.  */
#define ABOUT_TO_RETURN(pc) \
  ((read_memory_integer (pc, 4)|0x00040000) == 0x81c7e008)

/* Return 1 if P points to an invalid floating point value.  */

#define INVALID_FLOAT(p, len) 0   /* Just a first guess; not checked */

/* Largest integer type */
#define LONGEST long

/* Name of the builtin type for the LONGEST type above. */
#define BUILTIN_TYPE_LONGEST builtin_type_long

/* Say how long (ordinary) registers are.  */

#define REGISTER_TYPE long

/* Number of machine registers */

#define NUM_REGS 72

/* Initializer for an array of names of registers.
   There should be NUM_REGS strings in this initializer.  */

#define REGISTER_NAMES  \
{ "g0", "g1", "g2", "g3", "g4", "g5", "g6", "g7",	\
  "o0", "o1", "o2", "o3", "o4", "o5", "sp", "o7",	\
  "l0", "l1", "l2", "l3", "l4", "l5", "l6", "l7",	\
  "i0", "i1", "i2", "i3", "i4", "i5", "fp", "i7",	\
								\
  "f0", "f1", "f2", "f3", "f4", "f5", "f6", "f7",	\
  "f8", "f9", "f10", "f11", "f12", "f13", "f14", "f15",	\
  "f16", "f17", "f18", "f19", "f20", "f21", "f22", "f23",	\
  "f24", "f25", "f26", "f27", "f28", "f29", "f30", "f31",	\
                                                                \
  "y", "psr", "wim", "tbr", "pc", "npc", "fpsr", "cpsr" };

/* Register numbers of various important registers.
   Note that some of these values are "real" register numbers,
   and correspond to the general registers of the machine,
   and some are "phony" register numbers which are too large
   to be actual register numbers as far as the user is concerned
   but do serve to get the desired values when passed to read_register.  */

#define FP_REGNUM 30		/* Contains address of executing stack frame */
#define RP_REGNUM 15		/* Contains return address value, *before* \
				   any windows get switched.  */
#define SP_REGNUM 14		/* Contains address of top of stack, \
				   which is also the bottom of the frame.  */
#define Y_REGNUM 64		/* Temp register for multiplication, etc.  */
#define PS_REGNUM 65		/* Contains processor status */
#define PC_REGNUM 68		/* Contains program counter */
#define NPC_REGNUM 69           /* Contains next PC */
#define FP0_REGNUM 32		/* Floating point register 0 */
#define FPS_REGNUM 70		/* Floating point status register */
#define CPS_REGNUM 71		/* Coprocessor status register */

/* Total amount of space needed to store our copies of the machine's
   register state, the array `registers'.  */
#define REGISTER_BYTES (32*4+32*4+8*4)

/* Index within `registers' of the first byte of the space for
   register N.  */
/* ?? */
#define REGISTER_BYTE(N)  ((N)*4)

/* The SPARC processor has register windows.  */

#define HAVE_REGISTER_WINDOWS

/* Is this register part of the register window system?  A yes answer
   implies that 1) The name of this register will not be the same in
   other frames, and 2) This register is automatically "saved" (out
   registers shifting into ins counts) upon subroutine calls and thus
   there is no need to search more than one stack frame for it. */

#define REGISTER_IN_WINDOW_P(regnum)	\
  ((regnum) >= 8 && (regnum) < 32)

/* Number of bytes of storage in the actual machine representation
   for register N.  */

/* On the SPARC, all regs are 4 bytes.  */

#define REGISTER_RAW_SIZE(N) (4)

/* Number of bytes of storage in the program's representation
   for register N.  */

/* On the SPARC, all regs are 4 bytes.  */

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
 ((N) < 32 ? builtin_type_int : (N) < 64 ? builtin_type_float : \
  builtin_type_int)

/* Store the address of the place in which to copy the structure the
   subroutine will return.  This is called from call_function. */

#define STORE_STRUCT_RETURN(ADDR, SP) \
  { write_memory ((SP)+(16*4), &(ADDR), 4); }

/* Extract from an array REGBUF containing the (raw) register state
   a function return value of type TYPE, and copy that, in virtual format,
   into VALBUF.  */

#define EXTRACT_RETURN_VALUE(TYPE,REGBUF,VALBUF)	      \
  {      	       	       	       	       	       	       	           \
    if (TYPE_CODE (TYPE) == TYPE_CODE_FLT)		       		   \
      {							       		   \
	bcopy (((int *)(REGBUF))+FP0_REGNUM,		       		   \
	       (VALBUF), TYPE_LENGTH(TYPE));		       		   \
      }							       		   \
    else						       		   \
      bcopy (((int *)(REGBUF))+8, (VALBUF), TYPE_LENGTH (TYPE));           \
  }

/* Write into appropriate registers a function return value
   of type TYPE, given in virtual format.  */
/* On sparc, values are returned in register %o0.  */
#define STORE_RETURN_VALUE(TYPE,VALBUF) \
  {    	       	       	       	       	       	       	       	       	     \
    if (TYPE_CODE (TYPE) = TYPE_CODE_FLT)				     \
      /* Floating-point values are returned in the register pair */          \
      /* formed by %f0 and %f1 (doubles are, anyway).  */                    \
      write_register_bytes (REGISTER_BYTE (FP0_REGNUM), (VALBUF),	     \
			    TYPE_LENGTH (TYPE));			     \
    else								     \
      /* Other values are returned in register %o0.  */                      \
      write_register_bytes (REGISTER_BYTE (8), VALBUF, TYPE_LENGTH (TYPE));  \
  }

/* Extract from an array REGBUF containing the (raw) register state
   the address in which a function should return its structure value,
   as a CORE_ADDR (or an expression that can be used as one).  */

#define EXTRACT_STRUCT_VALUE_ADDRESS(REGBUF) \
  (read_memory_integer (((int *)(REGBUF))[SP_REGNUM]+(16*4), 4))

/* Enable use of alternate code to read and write registers.  */

#define NEW_SUN_PTRACE

/* Enable use of alternate code for Sun's format of core dump file.  */

#define NEW_SUN_CORE

/* Do implement the attach and detach commands.  */

#define ATTACH_DETACH


/* Describe the pointer in each stack frame to the previous stack frame
   (its caller).  */
#include <machine/reg.h>

#define GET_RWINDOW_REG(FRAME, REG) \
  (read_memory_integer (&((struct rwindow *)FRAME)->REG, 4))

/* FRAME_CHAIN takes a frame's nominal address
   and produces the frame's chain-pointer.

   FRAME_CHAIN_COMBINE takes the chain pointer and the frame's nominal address
   and produces the nominal address of the caller frame.

   However, if FRAME_CHAIN_VALID returns zero,
   it means the given frame is the outermost one and has no caller.
   In that case, FRAME_CHAIN_COMBINE is not used.  */

/* In the case of the Sun 4, the frame-chain's nominal address
   is held in the frame pointer register.

   On the Sun4, the frame (in %fp) is %sp for the previous frame.
   From the previous frame's %sp, we can find the previous frame's
   %fp: it is in the save area just above the previous frame's %sp.

   If we are setting up an arbitrary frame, we'll need to know where
   it ends.  Hence the following.  This part of the frame cache
   structure should be checked before it is assumed that this frame's
   bottom is in the stack pointer.

   If there isn't a frame below this one, the bottom of this frame is
   in the stack pointer.

   If there is a frame below this one, and the frame pointers are
   identical, it's a leaf frame and the bottoms are the same also.

   Otherwise the bottom of this frame is the top of the next frame.  */

#define EXTRA_FRAME_INFO	FRAME_ADDR bottom;
#define INIT_EXTRA_FRAME_INFO(fci)  \
  (fci)->bottom =					\
   ((fci)->next ?					\
    ((fci)->frame == (fci)->next_frame ?		\
     (fci)->next->bottom : (fci)->next->frame) :	\
    read_register (SP_REGNUM));

#define FRAME_CHAIN(thisframe) \
   GET_RWINDOW_REG ((thisframe)->frame, rw_in[6])

#define FRAME_CHAIN_VALID(chain, thisframe) \
  (chain != 0 && (outside_startup_file (FRAME_SAVED_PC (thisframe))))

#define FRAME_CHAIN_COMBINE(chain, thisframe) (chain)

/* Define other aspects of the stack frame.  */

/* A macro that tells us whether the function invocation represented
   by FI does not have a frame on the stack associated with it.  If it
   does not, FRAMELESS is set to 1, else 0.  */
#define FRAMELESS_FUNCTION_INVOCATION(FI, FRAMELESS) \
  FRAMELESS_LOOK_FOR_PROLOGUE(FI, FRAMELESS)

/* Where is the PC for a specific frame */

#define FRAME_SAVED_PC(FRAME) frame_saved_pc (FRAME)

/* If the argument is on the stack, it will be here.  */
#define FRAME_ARGS_ADDRESS(fi) ((fi)->frame)

#define FRAME_STRUCT_ARGS_ADDRESS(fi) ((fi)->frame)

#define FRAME_LOCALS_ADDRESS(fi) ((fi)->frame)

/* Set VAL to the number of args passed to frame described by FI.
   Can set VAL to -1, meaning no way to tell.  */

/* We can't tell how many args there are
   now that the C compiler delays popping them.  */
#define FRAME_NUM_ARGS(val,fi) (val = -1)

/* Return number of bytes at start of arglist that are not really args.  */

#define FRAME_ARGS_SKIP 68

/* Put here the code to store, into a struct frame_saved_regs,
   the addresses of the saved registers of frame described by FRAME_INFO.
   This includes special registers such as pc and fp saved in special
   ways in the stack frame.  sp is even more special:
   the address we return for it IS the sp for the next frame.

   Note that on register window machines, we are currently making the
   assumption that window registers are being saved somewhere in the
   frame in which they are being used.  If they are stored in an
   inferior frame, find_saved_register will break.

   On the Sun 4, the only time all registers are saved is when
   a dummy frame is involved.  Otherwise, the only saved registers
   are the LOCAL and IN registers which are saved as a result
   of the "save/restore" opcodes.  This condition is determined
   by address rather than by value.  */

#define FRAME_FIND_SAVED_REGS(fi, frame_saved_regs)    	    \
{ register int regnum;							\
  register CORE_ADDR pc;						\
  FRAME_ADDR frame = read_register (FP_REGNUM);				\
  FRAME fid = FRAME_INFO_ID (fi);					\
  if (!fid) fatal ("Bad frame info struct in FRAME_FIND_SAVED_REGS");	\
  bzero (&(frame_saved_regs), sizeof (frame_saved_regs));		\
  /* Old test.								\
  if ((fi)->pc >= frame - CALL_DUMMY_LENGTH - 0x140			\
      && (fi)->pc <= frame) */						\
  if ((fi)->pc >= ((fi)->bottom ? (fi)->bottom :			\
		   read_register (SP_REGNUM))				\
      && (fi)->pc <= FRAME_FP(fi))					\
    {									\
      for (regnum = 1; regnum < 8; regnum++)				\
	(frame_saved_regs).regs[regnum] =				\
	  frame + regnum * 4 - 0xa0;					\
      for (regnum = 24; regnum < 32; regnum++)				\
	(frame_saved_regs).regs[regnum] =				\
	  frame + (regnum - 24) * 4 - 0xc0;				\
      for (regnum = FP0_REGNUM; regnum < FP0_REGNUM + 32; regnum++)	\
	(frame_saved_regs).regs[regnum] =				\
	  frame + (regnum - FP0_REGNUM) * 4 - 0x80;			\
      for (regnum = 64; regnum < NUM_REGS; regnum++)			\
	(frame_saved_regs).regs[regnum] =				\
	  frame + (regnum - 64) * 4 - 0xe0;				\
      frame = (fi)->bottom ?						\
	(fi)->bottom : read_register (SP_REGNUM);			\
    }									\
  else									\
    {									\
      frame = (fi)->bottom ?						\
	(fi)->bottom : read_register (SP_REGNUM);			\
      for (regnum = 16; regnum < 32; regnum++)				\
	(frame_saved_regs).regs[regnum] = frame + (regnum-16) * 4;	\
    }									\
  if ((fi)->next)							\
    {									\
      /* Pull off either the next frame pointer or			\
	 the stack pointer */						\
      FRAME_ADDR next_next_frame =					\
	((fi)->next->bottom ?						\
	 (fi)->next->bottom :						\
	 read_register (SP_REGNUM));					\
      for (regnum = 8; regnum < 16; regnum++)				\
	(frame_saved_regs).regs[regnum] = next_next_frame + regnum * 4;	\
    }									\
  /* Otherwise, whatever we would get from ptrace(GETREGS) */		\
  /* is accurate */							\
  for (regnum = 30; regnum < 32; regnum++)				\
    (frame_saved_regs).regs[regnum] = frame + (regnum-16) * 4;		\
  (frame_saved_regs).regs[SP_REGNUM] = FRAME_FP (fi);			\
  (frame_saved_regs).regs[PC_REGNUM] = frame + 15*4;			\
}

/* Things needed for making the inferior call functions.  */
/*
 * First of all, let me give my opinion of what the DUMMY_FRAME
 * actually looks like.
 *
 *               |                                 |
 *               |                                 |
 *               + - - - - - - - - - - - - - - - - +<-- fp (level 0)
 *               |                                 |
 *               |                                 |
 *               |                                 |
 *               |                                 |
 *               |  Frame of innermost program     |
 *               |           function              |
 *               |                                 |
 *               |                                 |
 *               |                                 |
 *               |                                 |
 *               |                                 |
 *               |---------------------------------|<-- sp (level 0), fp (c)
 *               |                                 |
 *     DUMMY     |             fp0-31              |
 *               |                                 |
 *               |             ------              |<-- fp - 0x80
 *     FRAME     |              g0-7               |<-- fp - 0xa0
 *               |              i0-7               |<-- fp - 0xc0
 *               |             other               |<-- fp - 0xe0
 *               |               ?                 |
 *               |               ?                 |
 *               |---------------------------------|<-- sp' = fp - 0x140
 *               |                                 |
 * xcution start |                                 |
 * sp' + 0x94 -->|        CALL_DUMMY (x code)      |
 *               |                                 |
 *               |                                 |
 *               |---------------------------------|<-- sp'' = fp - 0x200
 *               |  align sp to 8 byte boundary    |
 *               |     ==> args to fn <==          |
 *  Room for     |                                 |
 * i & l's + agg | CALL_DUMMY_STACK_ADJUST = 0x0x44|
 *               |---------------------------------|<-- final sp (variable)
 *               |                                 |
 *               |   Where function called will    |
 *               |           build frame.          |
 *               |                                 |
 *               |                                 |
 *
 *   I understand everything in this picture except what the space
 * between fp - 0xe0 and fp - 0x140 is used for.  Oh, and I don't
 * understand why there's a large chunk of CALL_DUMMY that never gets
 * executed (its function is superceeded by PUSH_DUMMY_FRAME; they
 * are designed to do the same thing).
 *
 *   PUSH_DUMMY_FRAME saves the registers above sp' and pushes the
 * register file stack down one.
 *
 *   call_function then writes CALL_DUMMY, pushes the args onto the
 * stack, and adjusts the stack pointer.
 *
 *   run_stack_dummy then starts execution (in the middle of
 * CALL_DUMMY, as directed by call_function).
 */

/* Push an empty stack frame, to record the current PC, etc.  */

/* Note: to be perfectly correct, we have to restore the
   IN registers (which were the OUT registers of the calling frame).  */
/* Note that the write's are of registers in the context of the newly
   pushed frame.  Thus the the fp*'s, the g*'s, the i*'s, and
   the others, of the new frame, are being saved.
   The locals are new; they don't need to be saved. The i's and l's of
   the last frame were saved by the do_save_insn in the register
   file (ie. on the stack, since a context switch happended imm after)  */
/* We note that the return pointer register does not *need* to have
   the pc saved into it (return from this frame will be accomplished
   by a POP_FRAME), however, just in case it might be needed, we will
   leave it.  However, we will write the original value of RP into the
   location on the stack for saving i7 (what rp turns into upon call);
   this way we don't loose the value with our function call. */
/* Note that the pc saved must be 8 less than the actual pc, since
   both POP_FRAME and the normal return sequence on the sparc return
   to 8 more than the value of RP_REGNUM */

#define PUSH_DUMMY_FRAME \
{ extern char registers[];						\
  register int regnum;							\
  CORE_ADDR fp = read_register (FP_REGNUM);				\
  CORE_ADDR pc = read_register (PC_REGNUM) - 8;				\
  CORE_ADDR rp = read_register (RP_REGNUM);				\
  void do_save_insn ();							\
  supply_register (RP_REGNUM, &pc);					\
  do_save_insn (0x140);							\
  fp = read_register (FP_REGNUM);					\
  write_memory (fp - 0x80, &registers[REGISTER_BYTE (FP0_REGNUM)], 32 * 4);\
  write_memory (fp - 0xa0, &registers[REGISTER_BYTE (0)], 8 * 4);	\
  write_memory (fp - 0xc0, &registers[REGISTER_BYTE (24)], 7 * 4);	\
  write_memory (fp - 0xa4, &rp, 4);					\
  write_memory (fp - 0xe0, &registers[REGISTER_BYTE (64)], 8 * 4);	\
}

/* Discard from the stack the innermost frame,
   restoring all saved registers.
   Note that the values stored in fsr by get_frame_saved_regs are *in
   the context of the inferior frame*.  What this means is that the i
   regs of fsr must be restored into the o regs of the frame popped
   into.  We don't care about the output regs of the inferior frame.

   This is true for dummy frames.  Is it true for normal frames?  It
   really does appear so. */

#define POP_FRAME  \
{ register FRAME frame = get_current_frame ();				\
  register CORE_ADDR fp;						\
  register CORE_ADDR pc;						\
  register int regnum;			    				\
  struct frame_saved_regs fsr;		    				\
  struct frame_info *fi;		    				\
  char raw_buffer[REGISTER_BYTES];	    				\
  void do_restore_insn ();		    				\
  fi = get_frame_info (frame);						\
  fp = fi->frame;							\
  get_frame_saved_regs (fi, &fsr);	    				\
  pc = read_memory_integer (fsr.regs[PC_REGNUM], 4);			\
  do_restore_insn (PC_ADJUST (pc));					\
  if (fsr.regs[FP0_REGNUM])		    				\
    {					    				\
      read_memory (fsr.regs[FP0_REGNUM], raw_buffer, 32 * 4);		\
      write_register_bytes (REGISTER_BYTE (FP0_REGNUM), raw_buffer, 32 * 4); \
    }					    				\
  if (fsr.regs[1])			    				\
    {					    				\
      read_memory (fsr.regs[1], raw_buffer, 7 * 4);			\
      write_register_bytes (REGISTER_BYTE (1), raw_buffer, 7 * 4);	\
    }					    				\
  if (fsr.regs[24])			    				\
    {					    				\
      read_memory (fsr.regs[24], raw_buffer, 8 * 4);			\
      write_register_bytes (REGISTER_BYTE (8), raw_buffer, 8 * 4);	\
    }									\
  if (fsr.regs[PS_REGNUM])						\
    write_register (PS_REGNUM, read_memory_integer (fsr.regs[PS_REGNUM], 4)); \
  if (fsr.regs[Y_REGNUM])						\
    write_register (Y_REGNUM, read_memory_integer (fsr.regs[Y_REGNUM], 4)); \
  if (fsr.regs[NPC_REGNUM])						\
    write_register (NPC_REGNUM, read_memory_integer (fsr.regs[NPC_REGNUM], 4)); \
  flush_cached_frames ();						\
  set_current_frame ( create_new_frame (read_register (FP_REGNUM),	\
					read_pc ())); }

/* This sequence of words is the instructions

   save %sp,-0x140,%sp
   std	%f30,[%fp-0x08]
   std	%f28,[%fp-0x10]
   std	%f26,[%fp-0x18]
   std	%f24,[%fp-0x20]
   std	%f22,[%fp-0x28]
   std	%f20,[%fp-0x30]
   std	%f18,[%fp-0x38]
   std	%f16,[%fp-0x40]
   std	%f14,[%fp-0x48]
   std	%f12,[%fp-0x50]
   std	%f10,[%fp-0x58]
   std	%f8,[%fp-0x60]
   std	%f6,[%fp-0x68]
   std	%f4,[%fp-0x70]
   std	%f2,[%fp-0x78]
   std	%f0,[%fp-0x80]
   std	%g6,[%fp-0x88]
   std	%g4,[%fp-0x90]
   std	%g2,[%fp-0x98]
   std	%g0,[%fp-0xa0]
   std	%i6,[%fp-0xa8]
   std	%i4,[%fp-0xb0]
   std	%i2,[%fp-0xb8]
   std	%i0,[%fp-0xc0]
   nop ! stcsr	[%fp-0xc4]
   nop ! stfsr	[%fp-0xc8]
   nop ! wr	%npc,[%fp-0xcc]
   nop ! wr	%pc,[%fp-0xd0]
   rd	%tbr,%o0
   st	%o0,[%fp-0xd4]
   rd	%wim,%o1
   st	%o0,[%fp-0xd8]
   rd	%psr,%o0
   st	%o0,[%fp-0xdc]
   rd	%y,%o0
   st	%o0,[%fp-0xe0]

     /..* The arguments are pushed at this point by GDB;
	no code is needed in the dummy for this.
	The CALL_DUMMY_START_OFFSET gives the position of
	the following ld instruction.  *../

   ld	[%sp+0x58],%o5
   ld	[%sp+0x54],%o4
   ld	[%sp+0x50],%o3
   ld	[%sp+0x4c],%o2
   ld	[%sp+0x48],%o1
   call 0x00000000
   ld	[%sp+0x44],%o0
   nop
   ta 1
   nop

   note that this is 192 bytes, which is a multiple of 8 (not only 4) bytes.
   note that the `call' insn is a relative, not an absolute call.
   note that the `nop' at the end is needed to keep the trap from
        clobbering things (if NPC pointed to garbage instead).

We actually start executing at the `sethi', since the pushing of the
registers (as arguments) is done by PUSH_DUMMY_FRAME.  If this were
real code, the arguments for the function called by the CALL would be
pushed between the list of ST insns and the CALL, and we could allow
it to execute through.  But the arguments have to be pushed by GDB
after the PUSH_DUMMY_FRAME is done, and we cannot allow these ST
insns to be performed again, lest the registers saved be taken for
arguments.  */

#define CALL_DUMMY { 0x9de3bee0, 0xfd3fbff8, 0xf93fbff0, 0xf53fbfe8,	\
		     0xf13fbfe0, 0xed3fbfd8, 0xe93fbfd0, 0xe53fbfc8,	\
		     0xe13fbfc0, 0xdd3fbfb8, 0xd93fbfb0, 0xd53fbfa8,	\
		     0xd13fbfa0, 0xcd3fbf98, 0xc93fbf90, 0xc53fbf88,	\
		     0xc13fbf80, 0xcc3fbf78, 0xc83fbf70, 0xc43fbf68,	\
		     0xc03fbf60, 0xfc3fbf58, 0xf83fbf50, 0xf43fbf48,	\
		     0xf03fbf40, 0x01000000, 0x01000000, 0x01000000,	\
		     0x01000000, 0x91580000, 0xd027bf50, 0x93500000,	\
		     0xd027bf4c, 0x91480000, 0xd027bf48, 0x91400000,	\
		     0xd027bf44, 0xda03a058, 0xd803a054, 0xd603a050,	\
		     0xd403a04c, 0xd203a048, 0x40000000, 0xd003a044,	\
		     0x01000000, 0x91d02001, 0x01000000, 0x01000000}

#define CALL_DUMMY_LENGTH 192

#define CALL_DUMMY_START_OFFSET 148

#define CALL_DUMMY_STACK_ADJUST 68

/* Insert the specified number of args and function address
   into a call sequence of the above form stored at DUMMYNAME.  */

#define FIX_CALL_DUMMY(dummyname, pc, fun, nargs, type)     \
{									\
  *(int *)((char *) dummyname+168) = (0x40000000|((fun-(pc+168))>>2));	\
  if (TYPE_CODE (type) == TYPE_CODE_STRUCT				\
      || TYPE_CODE (type) == TYPE_CODE_UNION)	                        \
    *(int *)((char *) dummyname+176) = (TYPE_LENGTH (type) & 0x1fff);	\
}


/* Sparc has no reliable single step ptrace call */

#define NO_SINGLE_STEP 1

/* It does have a wait structure, and it might help things out . . . */

#define HAVE_WAIT_STRUCT

/* Handle a feature in the sun4 compiler ("call .stret4" at the end of
   functions returning structures).  */

#define SUN4_COMPILER_FEATURE

/* We need two arguments (in general) to the "info frame" command.
   Note that the definition of this macro implies that there exists a
   function "setup_arbitrary_frame" in mach-dep.c */

#define FRAME_SPECIFICATION_DYADIC

/* KDB stuff flushed for now.  */
