/* Parameters for execution on a Sun, for GDB, the GNU debugger.
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

/*
 * Configuration file for HP9000/300 series machine running
 * University of Utah's 4.3bsd port.  This is NOT for HP-UX.
 * Problems to hpbsd-bugs@cs.utah.edu
 */

#ifndef hp300
#define hp300
#endif

/* Watch out for NaNs */

#define IEEE_FLOAT

/* Get rid of any system-imposed stack limit if possible.  */

#define SET_STACK_LIMIT_HUGE

/* Define this if the C compiler puts an underscore at the front
   of external names before giving them to the linker.  */

#define NAMES_HAVE_UNDERSCORE

/* Debugger information will be in DBX format.  */

#define READ_DBX_FORMAT

/* Offset from address of function to start of its code.
   Zero on most machines.  */

#define FUNCTION_START_OFFSET 0

/* Advance PC across any function entry prologue instructions
   to reach some "real" code.  */

#define SKIP_PROLOGUE(pc)   \
{ register int op = read_memory_integer (pc, 2);	\
  if (op == 0047126)				\
    pc += 4;   /* Skip link #word */			\
  else if (op == 0044016)			\
    pc += 6;   /* Skip link #long */			\
}

/* Immediately after a function call, return the saved pc.
   Can't go through the frames for this because on some machines
   the new frame is not set up until the new function executes
   some instructions.  */

#define SAVED_PC_AFTER_CALL(frame) \
read_memory_integer (read_register (SP_REGNUM), 4)

/* This is the amount to subtract from u.u_ar0
   to get the offset in the core file of the register values.  */

#define KERNEL_U_ADDR 0x00917000

/* Address of end of stack space.  */

#define STACK_END_ADDR 0xFFF00000

/* Stack grows downward.  */

#define INNER_THAN <

/* Sequence of bytes for breakpoint instruction.  */

#define BREAKPOINT {0x4e, 0x42}

/* Amount PC must be decremented by after a breakpoint.
   This is often the number of bytes in BREAKPOINT
   but not always.  */

#define DECR_PC_AFTER_BREAK 2

/* Nonzero if instruction at PC is a return instruction.  */

#define ABOUT_TO_RETURN(pc) (read_memory_integer (pc, 2) == 0x4e75)

/* Return 1 if P points to an invalid floating point value.  */

#define INVALID_FLOAT(p, len) 0   /* Just a first guess; not checked */

/* Say how long (ordinary) registers are.  */

#define REGISTER_TYPE long

/* Number of machine registers */

#define NUM_REGS 29

/* Initializer for an array of names of registers.
   There should be NUM_REGS strings in this initializer.  */

#define REGISTER_NAMES  \
 {"d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7", \
  "a0", "a1", "a2", "a3", "a4", "a5", "a6", "sp", \
  "ps", "pc",  \
  "fp0", "fp1", "fp2", "fp3", "fp4", "fp5", "fp6", "fp7", \
  "fpcontrol", "fpstatus", "fpiaddr" }

/* Register numbers of various important registers.
   Note that some of these values are "real" register numbers,
   and correspond to the general registers of the machine,
   and some are "phony" register numbers which are too large
   to be actual register numbers as far as the user is concerned
   but do serve to get the desired values when passed to read_register.  */

#define FP_REGNUM 14		/* Contains address of executing stack frame */
#define SP_REGNUM 15		/* Contains address of top of stack */
#define PS_REGNUM 16		/* Contains processor status */
#define PC_REGNUM 17		/* Contains program counter */
#define FP0_REGNUM 18		/* Floating point register 0 */
#define FPC_REGNUM 26		/* 68881 control register */
#define FPS_REGNUM 27		/* 68881 status register */

/* Total amount of space needed to store our copies of the machine's
   register state, the array `registers'.  */
#define REGISTER_BYTES (16*4+8*12+8+12)

/* Index within `registers' of the first byte of the space for
   register N.  */

#define REGISTER_BYTE(N)  \
 ((N) >= FPC_REGNUM ? (((N) - FPC_REGNUM) * 4) + 168	\
  : (N) >= FP0_REGNUM ? (((N) - FP0_REGNUM) * 12) + 72	\
  : (N) * 4)

/* Number of bytes of storage in the actual machine representation
   for register N.  On the 68000, all regs are 4 bytes
   except the floating point regs which are 12 bytes.  */

#define REGISTER_RAW_SIZE(N) (((unsigned)(N) - FP0_REGNUM) < 8 ? 12 : 4)

/* Number of bytes of storage in the program's representation
   for register N.  On the 68000, all regs are 4 bytes
   except the floating point regs which are 8-byte doubles.  */

#define REGISTER_VIRTUAL_SIZE(N) (((unsigned)(N) - FP0_REGNUM) < 8 ? 8 : 4)

/* Largest value REGISTER_RAW_SIZE can have.  */

#define MAX_REGISTER_RAW_SIZE 12

/* Largest value REGISTER_VIRTUAL_SIZE can have.  */

#define MAX_REGISTER_VIRTUAL_SIZE 8

/* Nonzero if register N requires conversion
   from raw format to virtual format.  */

#define REGISTER_CONVERTIBLE(N) (((unsigned)(N) - FP0_REGNUM) < 8)

/* Convert data from raw format for register REGNUM
   to virtual format for register REGNUM.  */

#define REGISTER_CONVERT_TO_VIRTUAL(REGNUM,FROM,TO)	\
{ if ((REGNUM) >= FP0_REGNUM && (REGNUM) < FPC_REGNUM)	\
    convert_from_68881 ((FROM), (TO));	\
  else					\
    bcopy ((FROM), (TO), 4); }

/* Convert data from virtual format for register REGNUM
   to raw format for register REGNUM.  */

#define REGISTER_CONVERT_TO_RAW(REGNUM,FROM,TO)	\
{ if ((REGNUM) >= FP0_REGNUM && (REGNUM) < FPC_REGNUM)	\
    convert_to_68881 ((FROM), (TO));	\
  else					\
    bcopy ((FROM), (TO), 4); }

/* Return the GDB type object for the "standard" data type
   of data in register N.  */

#define REGISTER_VIRTUAL_TYPE(N) \
 (((unsigned)(N) - FP0_REGNUM) < 8 ? builtin_type_double : builtin_type_int)

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

/* Compensate for lack of `vprintf' function.  */ 
#define vprintf(format, ap) _doprnt (format, ap, stdout) 

/* This is a piece of magic that is given a register number REGNO
   and as BLOCKEND the address in the system of the end of the user structure
   and stores in ADDR the address in the kernel or core dump
   of that register.  */

#define REGISTER_U_ADDR(addr, blockend, regno)				\
{									\
  if (regno < PS_REGNUM)						\
    addr = (int) &((struct frame *)(blockend))->f_regs[regno];		\
  else if (regno == PS_REGNUM)						\
    addr = (int) &((struct frame *)(blockend))->f_stackadj;		\
  else if (regno == PC_REGNUM)						\
    addr = (int) &((struct frame *)(blockend))->f_pc;			\
  else if (regno < FPC_REGNUM)						\
    addr = (int)							\
      &((struct user *)0)->u_pcb.pcb_fpregs.fpf_regs[((regno)-FP0_REGNUM)*3];\
  else if (regno == FPC_REGNUM)						\
    addr = (int) &((struct user *)0)->u_pcb.pcb_fpregs.fpf_fpcr;	\
  else if (regno == FPS_REGNUM)						\
    addr = (int) &((struct user *)0)->u_pcb.pcb_fpregs.fpf_fpsr;	\
  else									\
    addr = (int) &((struct user *)0)->u_pcb.pcb_fpregs.fpf_fpiar;	\
}

/* It is safe to look for symsegs on a Sun, because Sun's ld
   does not screw up with random garbage at end of file.  */

#define READ_GDB_SYMSEGS

/* Describe the pointer in each stack frame to the previous stack frame
   (its caller).  */

/* FRAME_CHAIN takes a frame's nominal address
   and produces the frame's chain-pointer.

   FRAME_CHAIN_COMBINE takes the chain pointer and the frame's nominal address
   and produces the nominal address of the caller frame.

   However, if FRAME_CHAIN_VALID returns zero,
   it means the given frame is the outermost one and has no caller.
   In that case, FRAME_CHAIN_COMBINE is not used.  */

/* In the case of the Sun, the frame's nominal address
   is the address of a 4-byte word containing the calling frame's address.  */

#define FRAME_CHAIN(thisframe)  (read_memory_integer (thisframe, 4))

#define FRAME_CHAIN_VALID(chain, thisframe) \
  (chain != 0 && (FRAME_SAVED_PC (thisframe) >= first_object_file_end))

#define FRAME_CHAIN_COMBINE(chain, thisframe) (chain)

/* Define other aspects of the stack frame.  */

#define FRAME_SAVED_PC(frame) (read_memory_integer (frame + 4, 4))

#define FRAME_ARGS_ADDRESS(fi) (fi.frame)

#define FRAME_LOCALS_ADDRESS(fi) (fi.frame)

/* Set VAL to the number of args passed to frame described by FI.
   Can set VAL to -1, meaning no way to tell.  */

/* We can't tell how many args there are
   now that the C compiler delays popping them.  */
#define FRAME_NUM_ARGS(val,fi) (val = -1)

#if 0
#define FRAME_NUM_ARGS(val, fi)  \
{ register CORE_ADDR pc = FRAME_SAVED_PC (fi.frame);		\
  register int insn = 0177777 & read_memory_integer (pc, 2);	\
  val = 0;							\
  if (insn == 0047757 || insn == 0157374)  /* lea W(sp),sp or addaw #W,sp */ \
    val = read_memory_integer (pc + 2, 2);			\
  else if ((insn & 0170777) == 0050217 /* addql #N, sp */	\
	   || (insn & 0170777) == 0050117)  /* addqw */		\
    { val = (insn >> 9) & 7; if (val == 0) val = 8; }		\
  else if (insn == 0157774) /* addal #WW, sp */			\
    val = read_memory_integer (pc + 2, 4);			\
  val >>= 2; }
#endif

/* Return number of bytes at start of arglist that are not really args.  */

#define FRAME_ARGS_SKIP 8

/* Put here the code to store, into a struct frame_saved_regs,
   the addresses of the saved registers of frame described by FRAME_INFO.
   This includes special registers such as pc and fp saved in special
   ways in the stack frame.  sp is even more special:
   the address we return for it IS the sp for the next frame.  */

#define FRAME_FIND_SAVED_REGS(frame_info, frame_saved_regs)		\
{ register int regnum;							\
  register int regmask;							\
  register CORE_ADDR next_addr;						\
  register CORE_ADDR pc;						\
  int nextinsn;								\
  bzero (&frame_saved_regs, sizeof frame_saved_regs);			\
  if ((frame_info).pc >= (frame_info).frame - CALL_DUMMY_LENGTH - FP_REGNUM*4 - 8*12 - 4 \
      && (frame_info).pc <= (frame_info).frame)				\
    { next_addr = (frame_info).frame;					\
      pc = (frame_info).frame - CALL_DUMMY_LENGTH - FP_REGNUM * 4 - 8*12 - 4; }\
  else   								\
    { pc = get_pc_function_start ((frame_info).pc); 			\
      /* Verify we have a link a6 instruction next;			\
	 if not we lose.  If we win, find the address above the saved   \
	 regs using the amount of storage from the link instruction.  */\
      if (044016 == read_memory_integer (pc, 2))			\
	next_addr = (frame_info).frame + read_memory_integer (pc += 2, 4), pc+=4; \
      else if (047126 == read_memory_integer (pc, 2))			\
	next_addr = (frame_info).frame + read_memory_integer (pc += 2, 2), pc+=2; \
      else goto lose;							\
      /* If have an addal #-n, sp next, adjust next_addr.  */		\
      if ((0177777 & read_memory_integer (pc, 2)) == 0157774)		\
	next_addr += read_memory_integer (pc += 2, 4), pc += 4;		\
    }									\
  /* next should be a moveml to (sp) or -(sp) or a movl r,-(sp) */	\
  regmask = read_memory_integer (pc + 2, 2);				\
  /* But before that can come an fmovem.  Check for it.  */		\
  nextinsn = 0xffff & read_memory_integer (pc, 2);			\
  if (0xf227 == nextinsn						\
      && (regmask & 0xff00) == 0xe000)					\
    { pc += 4; /* Regmask's low bit is for register fp7, the first pushed */ \
      for (regnum = FP0_REGNUM + 7; regnum >= FP0_REGNUM; regnum--, regmask >>= 1)		\
	if (regmask & 1)						\
          (frame_saved_regs).regs[regnum] = (next_addr -= 12);		\
      regmask = read_memory_integer (pc + 2, 2); }			\
  if (0044327 == read_memory_integer (pc, 2))				\
    { pc += 4; /* Regmask's low bit is for register 0, the first written */ \
      for (regnum = 0; regnum < 16; regnum++, regmask >>= 1)		\
	if (regmask & 1)						\
          (frame_saved_regs).regs[regnum] = (next_addr += 4) - 4; }	\
  else if (0044347 == read_memory_integer (pc, 2))			\
    { pc += 4; /* Regmask's low bit is for register 15, the first pushed */ \
      for (regnum = 15; regnum >= 0; regnum--, regmask >>= 1)		\
	if (regmask & 1)						\
          (frame_saved_regs).regs[regnum] = (next_addr -= 4); }		\
  else if (0x2f00 == 0xfff0 & read_memory_integer (pc, 2))		\
    { regnum = 0xf & read_memory_integer (pc, 2); pc += 2;		\
      (frame_saved_regs).regs[regnum] = (next_addr -= 4); }		\
  /* fmovemx to index of sp may follow.  */				\
  regmask = read_memory_integer (pc + 2, 2);				\
  nextinsn = 0xffff & read_memory_integer (pc, 2);			\
  if (0xf236 == nextinsn						\
      && (regmask & 0xff00) == 0xf000)					\
    { pc += 10; /* Regmask's low bit is for register fp0, the first written */ \
      for (regnum = FP0_REGNUM + 7; regnum >= FP0_REGNUM; regnum--, regmask >>= 1)		\
	if (regmask & 1)						\
          (frame_saved_regs).regs[regnum] = (next_addr += 12) - 12;	\
      regmask = read_memory_integer (pc + 2, 2); }			\
  /* clrw -(sp); movw ccr,-(sp) may follow.  */				\
  if (0x426742e7 == read_memory_integer (pc, 4))			\
    (frame_saved_regs).regs[PS_REGNUM] = (next_addr -= 4);		\
  lose: ;								\
  (frame_saved_regs).regs[SP_REGNUM] = (frame_info).frame + 8;		\
  (frame_saved_regs).regs[FP_REGNUM] = (frame_info).frame;		\
  (frame_saved_regs).regs[PC_REGNUM] = (frame_info).frame + 4;		\
}

/* Things needed for making the inferior call functions.  */

/* Push an empty stack frame, to record the current PC, etc.  */

#define PUSH_DUMMY_FRAME \
{ register CORE_ADDR sp = read_register (SP_REGNUM);			\
  register int regnum;							\
  char raw_buffer[12];							\
  sp = push_word (sp, read_register (PC_REGNUM));			\
  sp = push_word (sp, read_register (FP_REGNUM));			\
  write_register (FP_REGNUM, sp);					\
  for (regnum = FP0_REGNUM + 7; regnum >= FP0_REGNUM; regnum--)		\
    { read_register_bytes (REGISTER_BYTE (regnum), raw_buffer, 12);	\
      sp = push_bytes (sp, raw_buffer, 12); }				\
  for (regnum = FP_REGNUM - 1; regnum >= 0; regnum--)			\
    sp = push_word (sp, read_register (regnum));			\
  sp = push_word (sp, read_register (PS_REGNUM));			\
  write_register (SP_REGNUM, sp);  }

/* Discard from the stack the innermost frame, 
   restoring all saved registers.  */

#define POP_FRAME  \
{ register CORE_ADDR fp = read_register (FP_REGNUM);		 \
  register int regnum;						 \
  struct frame_saved_regs fsr;					 \
  struct frame_info fi;						 \
  char raw_buffer[12];						 \
  fi = get_frame_info (fp);					 \
  get_frame_saved_regs (&fi, &fsr);				 \
  for (regnum = FP0_REGNUM + 7; regnum >= FP0_REGNUM; regnum--)	 \
    if (fsr.regs[regnum])					 \
      { read_memory (fsr.regs[regnum], raw_buffer, 12);		 \
        write_register_bytes (REGISTER_BYTE (regnum), raw_buffer, 12); }\
  for (regnum = FP_REGNUM - 1; regnum >= 0; regnum--)		 \
    if (fsr.regs[regnum])					 \
      write_register (regnum, read_memory_integer (fsr.regs[regnum], 4)); \
  if (fsr.regs[PS_REGNUM])					 \
    write_register (PS_REGNUM, read_memory_integer (fsr.regs[PS_REGNUM], 4)); \
  write_register (FP_REGNUM, read_memory_integer (fp, 4));	 \
  write_register (PC_REGNUM, read_memory_integer (fp + 4, 4));   \
  write_register (SP_REGNUM, fp + 8);				 \
  set_current_frame (read_register (FP_REGNUM)); }

/* This sequence of words is the instructions
     fmovem 0xff,-(sp)
     moveml 0xfffc,-(sp)
     clrw -(sp)
     movew ccr,-(sp)
     /..* The arguments are pushed at this point by GDB;
	no code is needed in the dummy for this.
	The CALL_DUMMY_START_OFFSET gives the position of 
	the following jsr instruction.  *../
     jsr @#32323232
     addl #69696969,sp
     trap #2
     nop
Note this is 28 bytes.
We actually start executing at the jsr, since the pushing of the
registers is done by PUSH_DUMMY_FRAME.  If this were real code,
the arguments for the function called by the jsr would be pushed
between the moveml and the jsr, and we could allow it to execute through.
But the arguments have to be pushed by GDB after the PUSH_DUMMY_FRAME is done,
and we cannot allow the moveml to push the registers again lest they be
taken for the arguments.  */

#define CALL_DUMMY {0xf227e0ff, 0x48e7fffc, 0x426742e7, 0x4eb93232, 0x3232dffc, 0x69696969, 0x4e424e71}

#define CALL_DUMMY_LENGTH 28

#define CALL_DUMMY_START_OFFSET 12

/* Insert the specified number of args and function address
   into a call sequence of the above form stored at DUMMYNAME.  */

#define FIX_CALL_DUMMY(dummyname, fun, nargs)     \
{ *(int *)((char *) dummyname + 20) = nargs * 4;  \
  *(int *)((char *) dummyname + 14) = fun; }

/* Interface definitions for kernel debugger KDB.  */

/* Map machine fault codes into signal numbers.
   First subtract 0, divide by 4, then index in a table.
   Faults for which the entry in this table is 0
   are not handled by KDB; the program's own trap handler
   gets to handle then.  */

#define FAULT_CODE_ORIGIN 0
#define FAULT_CODE_UNITS 4
#define FAULT_TABLE    \
{ 0, 0, 0, 0, SIGTRAP, 0, 0, 0, \
  0, SIGTRAP, 0, 0, 0, 0, 0, SIGKILL, \
  0, 0, 0, 0, 0, 0, 0, 0, \
  SIGILL }

/* Start running with a stack stretching from BEG to END.
   BEG and END should be symbols meaningful to the assembler.
   This is used only for kdb.  */

#define INIT_STACK(beg, end)  \
{ asm (".globl end");         \
  asm ("movel #end, sp");      \
  asm ("movel #0,a6"); }

/* Push the frame pointer register on the stack.  */
#define PUSH_FRAME_PTR        \
  asm ("movel a6,sp@-");

/* Copy the top-of-stack to the frame pointer register.  */
#define POP_FRAME_PTR  \
  asm ("movl sp@,a6");

/* After KDB is entered by a fault, push all registers
   that GDB thinks about (all NUM_REGS of them),
   so that they appear in order of ascending GDB register number.
   The fault code will be on the stack beyond the last register.  */

#define PUSH_REGISTERS        \
{ asm ("clrw -(sp)");	      \
  asm ("pea sp@(10)");	      \
  asm ("movem #0xfffe,sp@-"); }

/* Assuming the registers (including processor status) have been
   pushed on the stack in order of ascending GDB register number,
   restore them and return to the address in the saved PC register.  */

#define POP_REGISTERS          \
{ asm ("subil #8,sp@(28)");     \
  asm ("movem sp@,#0xffff"); \
  asm ("rte"); }
