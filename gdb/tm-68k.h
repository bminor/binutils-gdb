/* Parameters for execution on a 68000 series machine.
   Copyright (C) 1986, 1987, 1989, 1990 Free Software Foundation, Inc.

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
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* Generic 68000 stuff, to be included by other m-*.h files.
   Define HAVE_68881 if that is the case.  */

#if defined (HAVE_68881)
#define IEEE_FLOAT 1
#endif

/* Define the bit, byte, and word ordering of the machine.  */
#define TARGET_BYTE_ORDER BIG_ENDIAN

/* Offset from address of function to start of its code.
   Zero on most machines.  */

#define FUNCTION_START_OFFSET 0

/* Advance PC across any function entry prologue instructions
   to reach some "real" code.  */

#define SKIP_PROLOGUE(pc)   \
{ register int op = read_memory_integer (pc, 2);	\
  if (op == 0047126)					\
    pc += 4;   /* Skip link #word */			\
  else if (op == 0044016)				\
    pc += 6;   /* Skip link #long */			\
  /* Not sure why branches are here.  */                \
  /* From m-isi.h, m-altos.h */                         \
  else if (op == 0060000)				\
    pc += 4;   /* Skip bra #word */			\
  else if (op == 00600377)				\
    pc += 6;   /* skip bra #long */			\
  else if ((op & 0177400) == 0060000)			\
    pc += 2;   /* skip bra #char */			\
}

/* Immediately after a function call, return the saved pc.
   Can't always go through the frames for this because on some machines
   the new frame is not set up until the new function executes
   some instructions.  */

#define SAVED_PC_AFTER_CALL(frame) \
read_memory_integer (read_register (SP_REGNUM), 4)

/* Stack grows downward.  */

#define INNER_THAN <

/* Sequence of bytes for breakpoint instruction.
   This is a TRAP instruction.  The last 4 bits (0xf below) is the
   vector.  Systems which don't use 0xf should define BREAKPOINT
   themselves before including this file.  */

#if !defined (BREAKPOINT)
#define BREAKPOINT {0x4e, 0x4f}
#endif

/* If your kernel resets the pc after the trap happens you may need to
   define this in m-68k.h.  */

#if !defined (DECR_PC_AFTER_BREAK)
#define DECR_PC_AFTER_BREAK 2
#endif

/* Nonzero if instruction at PC is a return instruction.  */
/* Allow any of the return instructions, including a trapv and a return
   from interupt.  */

#define ABOUT_TO_RETURN(pc) ((read_memory_integer (pc, 2) & ~0x3) == 0x4e74)

/* Return 1 if P points to an invalid floating point value.  */

#define INVALID_FLOAT(p, len) 0   /* Just a first guess; not checked */

/* Say how long registers are.  */

#define REGISTER_TYPE long

#if defined (HAVE_68881)
#  if defined (GDB_TARGET_IS_SUN3)
    /* Sun3 status includes fpflags, which shows whether the FPU has been used
       by the process, and whether the FPU was done with an instruction or 
       was interrupted in the middle of a long instruction.  See
       <machine/reg.h>.  */
    /*                      a&d, pc,sr, fp, fpstat, fpflags   */
#    define NUM_REGS 31
#    define REGISTER_BYTES (16*4 + 8 + 8*12 + 3*4 + 4)
#  else /* Not sun3.  */
#    define NUM_REGS 29
#    define REGISTER_BYTES (16*4 + 8 + 8*12 + 3*4)
#  endif /* Not sun3.  */
#else /* No 68881.  */
#  define NUM_REGS 18
#  define REGISTER_BYTES (16*4 + 8)
#endif /* No 68881.  */

/* Index within `registers' of the first byte of the space for
   register N.  */

#if defined (HAVE_68881)
#define REGISTER_BYTE(N)  \
 ((N) >= FPC_REGNUM ? (((N) - FPC_REGNUM) * 4) + 168	\
  : (N) >= FP0_REGNUM ? (((N) - FP0_REGNUM) * 12) + 72	\
  : (N) * 4)

/* Number of bytes of storage in the actual machine representation
   for register N.  On the 68000, all regs are 4 bytes
   except the floating point regs which are 12 bytes.  */
/* Note that the unsigned cast here forces the result of the
   subtraction to very high positive values if N < FP0_REGNUM */

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

/* Put the declaration out here because if it's in the macros, PCC
   will complain.  */
extern struct ext_format ext_format_68881 [];

/* Convert data from raw format for register REGNUM
   to virtual format for register REGNUM.  */

#define REGISTER_CONVERT_TO_VIRTUAL(REGNUM,FROM,TO)	\
{ \
  if ((REGNUM) >= FP0_REGNUM && (REGNUM) < FPC_REGNUM)	\
    ieee_extended_to_double (ext_format_68881, (FROM), (TO));	\
  else					\
    bcopy ((FROM), (TO), 4);	\
}

/* Convert data from virtual format for register REGNUM
   to raw format for register REGNUM.  */

#define REGISTER_CONVERT_TO_RAW(REGNUM,FROM,TO)	\
{ \
  if ((REGNUM) >= FP0_REGNUM && (REGNUM) < FPC_REGNUM)	\
    double_to_ieee_extended (ext_format_68881, (FROM), (TO));	\
  else					\
    bcopy ((FROM), (TO), 4);	\
}

/* Return the GDB type object for the "standard" data type
   of data in register N.  */
/* Note, for registers which contain addresses return
   pointer to void, not pointer to char, because we don't
   want to attempt to print the string after printing the address.  */
#define REGISTER_VIRTUAL_TYPE(N) \
 (((unsigned)(N) - FP0_REGNUM) < 8 ? builtin_type_double :           \
  (N) == PC_REGNUM || (N) == FP_REGNUM || (N) == SP_REGNUM ?         \
  lookup_pointer_type (builtin_type_void) : builtin_type_int)

#else /* no 68881.  */
/* Index within `registers' of the first byte of the space for
   register N.  */

#define REGISTER_BYTE(N)  ((N) * 4)

/* Number of bytes of storage in the actual machine representation
   for register N.  On the 68000, all regs are 4 bytes.  */

#define REGISTER_RAW_SIZE(N) 4

/* Number of bytes of storage in the program's representation
   for register N.  On the 68000, all regs are 4 bytes.  */

#define REGISTER_VIRTUAL_SIZE(N) 4

/* Largest value REGISTER_RAW_SIZE can have.  */

#define MAX_REGISTER_RAW_SIZE 4

/* Largest value REGISTER_VIRTUAL_SIZE can have.  */

#define MAX_REGISTER_VIRTUAL_SIZE 4

/* Nonzero if register N requires conversion
   from raw format to virtual format.  */

#define REGISTER_CONVERTIBLE(N) 0

/* Convert data from raw format for register REGNUM
   to virtual format for register REGNUM.  */

#define REGISTER_CONVERT_TO_VIRTUAL(REGNUM,FROM,TO)  bcopy ((FROM), (TO), 4);

/* Convert data from virtual format for register REGNUM
   to raw format for register REGNUM.  */

#define REGISTER_CONVERT_TO_RAW(REGNUM,FROM,TO)  bcopy ((FROM), (TO), 4);

/* Return the GDB type object for the "standard" data type
   of data in register N.  */

#define REGISTER_VIRTUAL_TYPE(N)  builtin_type_int

#endif /* No 68881.  */

/* Initializer for an array of names of registers.
   Entries beyond the first NUM_REGS are ignored.  */

#define REGISTER_NAMES  \
 {"d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7", \
  "a0", "a1", "a2", "a3", "a4", "a5", "fp", "sp", \
  "ps", "pc",  \
  "fp0", "fp1", "fp2", "fp3", "fp4", "fp5", "fp6", "fp7", \
  "fpcontrol", "fpstatus", "fpiaddr", "fpcode", "fpflags" }

/* Register numbers of various important registers.
   Note that some of these values are "real" register numbers,
   and correspond to the general registers of the machine,
   and some are "phony" register numbers which are too large
   to be actual register numbers as far as the user is concerned
   but do serve to get the desired values when passed to read_register.  */

#define A1_REGNUM 9
#define FP_REGNUM 14		/* Contains address of executing stack frame */
#define SP_REGNUM 15		/* Contains address of top of stack */
#define PS_REGNUM 16		/* Contains processor status */
#define PC_REGNUM 17		/* Contains program counter */
#if defined (HAVE_68881)
#define FP0_REGNUM 18		/* Floating point register 0 */
#define FPC_REGNUM 26		/* 68881 control register */
#define FPS_REGNUM 27		/* 68881 status register */
#endif /* 68881.  */

/* Store the address of the place in which to copy the structure the
   subroutine will return.  This is called from call_function. */

#define STORE_STRUCT_RETURN(ADDR, SP) \
  { write_register (A1_REGNUM, (ADDR)); }

/* Extract from an array REGBUF containing the (raw) register state
   a function return value of type TYPE, and copy that, in virtual format,
   into VALBUF.  This is assuming that floating point values are returned
   as doubles in d0/d1.  */

#if !defined (EXTRACT_RETURN_VALUE)
#define EXTRACT_RETURN_VALUE(TYPE,REGBUF,VALBUF) \
  bcopy (REGBUF, VALBUF, TYPE_LENGTH (TYPE))
#endif

/* Write into appropriate registers a function return value
   of type TYPE, given in virtual format.  Assumes floats are passed
   in d0/d1.  */

#if !defined (STORE_RETURN_VALUE)
#define STORE_RETURN_VALUE(TYPE,VALBUF) \
  write_register_bytes (0, VALBUF, TYPE_LENGTH (TYPE))
#endif

/* Extract from an array REGBUF containing the (raw) register state
   the address in which a function should return its structure value,
   as a CORE_ADDR (or an expression that can be used as one).  */

#define EXTRACT_STRUCT_VALUE_ADDRESS(REGBUF) (*(CORE_ADDR *)(REGBUF))

/* Describe the pointer in each stack frame to the previous stack frame
   (its caller).  */

/* FRAME_CHAIN takes a frame's nominal address
   and produces the frame's chain-pointer.

   FRAME_CHAIN_COMBINE takes the chain pointer and the frame's nominal address
   and produces the nominal address of the caller frame.

   However, if FRAME_CHAIN_VALID returns zero,
   it means the given frame is the outermost one and has no caller.
   In that case, FRAME_CHAIN_COMBINE is not used.  */

/* In the case of the 68000, the frame's nominal address
   is the address of a 4-byte word containing the calling frame's address.  */

#define FRAME_CHAIN(thisframe)  \
  (outside_startup_file ((thisframe)->pc) ? \
   read_memory_integer ((thisframe)->frame, 4) :\
   0)

#define FRAME_CHAIN_VALID(chain, thisframe) \
  (chain != 0 && outside_startup_file (FRAME_SAVED_PC (thisframe)))

#define FRAME_CHAIN_COMBINE(chain, thisframe) (chain)

/* Define other aspects of the stack frame.  */

/* A macro that tells us whether the function invocation represented
   by FI does not have a frame on the stack associated with it.  If it
   does not, FRAMELESS is set to 1, else 0.  */
#define FRAMELESS_FUNCTION_INVOCATION(FI, FRAMELESS) \
  (FRAMELESS) = frameless_look_for_prologue(FI)

#define FRAME_SAVED_PC(FRAME) (read_memory_integer ((FRAME)->frame + 4, 4))

#define FRAME_ARGS_ADDRESS(fi) ((fi)->frame)

#define FRAME_LOCALS_ADDRESS(fi) ((fi)->frame)

/* Set VAL to the number of args passed to frame described by FI.
   Can set VAL to -1, meaning no way to tell.  */

/* We can't tell how many args there are
   now that the C compiler delays popping them.  */
#if !defined (FRAME_NUM_ARGS)
#define FRAME_NUM_ARGS(val,fi) (val = -1)
#endif

/* Return number of bytes at start of arglist that are not really args.  */

#define FRAME_ARGS_SKIP 8

/* Put here the code to store, into a struct frame_saved_regs,
   the addresses of the saved registers of frame described by FRAME_INFO.
   This includes special registers such as pc and fp saved in special
   ways in the stack frame.  sp is even more special:
   the address we return for it IS the sp for the next frame.  */

#if !defined (FRAME_FIND_SAVED_REGS)
#if defined (HAVE_68881)
#define FRAME_FIND_SAVED_REGS(frame_info, frame_saved_regs)		\
{ register int regnum;							\
  register int regmask;							\
  register CORE_ADDR next_addr;						\
  register CORE_ADDR pc;						\
  int nextinsn;								\
  bzero (&frame_saved_regs, sizeof frame_saved_regs);			\
  if ((frame_info)->pc >= (frame_info)->frame - CALL_DUMMY_LENGTH - FP_REGNUM*4 - 8*12 - 4 \
      && (frame_info)->pc <= (frame_info)->frame)				\
    { next_addr = (frame_info)->frame;					\
      pc = (frame_info)->frame - CALL_DUMMY_LENGTH - FP_REGNUM * 4 - 8*12 - 4; }\
  else   								\
    { pc = get_pc_function_start ((frame_info)->pc); 			\
      /* Verify we have a link a6 instruction next;			\
	 if not we lose.  If we win, find the address above the saved   \
	 regs using the amount of storage from the link instruction.  */\
      if (044016 == read_memory_integer (pc, 2))			\
	next_addr = (frame_info)->frame + read_memory_integer (pc += 2, 4), pc+=4; \
      else if (047126 == read_memory_integer (pc, 2))			\
	next_addr = (frame_info)->frame + read_memory_integer (pc += 2, 2), pc+=2; \
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
  else if (0x2f00 == (0xfff0 & read_memory_integer (pc, 2)))		\
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
  (frame_saved_regs).regs[SP_REGNUM] = (frame_info)->frame + 8;		\
  (frame_saved_regs).regs[FP_REGNUM] = (frame_info)->frame;		\
  (frame_saved_regs).regs[PC_REGNUM] = (frame_info)->frame + 4;		\
}
#else /* no 68881.  */
#define FRAME_FIND_SAVED_REGS(frame_info, frame_saved_regs)		\
{ register int regnum;							\
  register int regmask;							\
  register CORE_ADDR next_addr;						\
  register CORE_ADDR pc;						\
  bzero (&frame_saved_regs, sizeof frame_saved_regs);			\
  if ((frame_info)->pc >= (frame_info)->frame - CALL_DUMMY_LENGTH - FP_REGNUM*4 - 4 \
      && (frame_info)->pc <= (frame_info)->frame)				\
    { next_addr = (frame_info)->frame;					\
      pc = (frame_info)->frame - CALL_DUMMY_LENGTH - FP_REGNUM * 4 - 4; }\
  else   								\
    { pc = get_pc_function_start ((frame_info)->pc); 			\
      /* Verify we have a link a6 instruction next;			\
	 if not we lose.  If we win, find the address above the saved   \
	 regs using the amount of storage from the link instruction.  */\
      if (044016 == read_memory_integer (pc, 2))			\
	next_addr = (frame_info)->frame + read_memory_integer (pc += 2, 4), pc+=4; \
      else if (047126 == read_memory_integer (pc, 2))			\
	next_addr = (frame_info)->frame + read_memory_integer (pc += 2, 2), pc+=2; \
      else goto lose;							\
      /* If have an addal #-n, sp next, adjust next_addr.  */		\
      if ((0177777 & read_memory_integer (pc, 2)) == 0157774)		\
	next_addr += read_memory_integer (pc += 2, 4), pc += 4;		\
    }									\
  /* next should be a moveml to (sp) or -(sp) or a movl r,-(sp) */	\
  regmask = read_memory_integer (pc + 2, 2);				\
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
  /* clrw -(sp); movw ccr,-(sp) may follow.  */				\
  if (0x426742e7 == read_memory_integer (pc, 4))			\
    (frame_saved_regs).regs[PS_REGNUM] = (next_addr -= 4);		\
  lose: ;								\
  (frame_saved_regs).regs[SP_REGNUM] = (frame_info)->frame + 8;		\
  (frame_saved_regs).regs[FP_REGNUM] = (frame_info)->frame;		\
  (frame_saved_regs).regs[PC_REGNUM] = (frame_info)->frame + 4;		\
}
#endif /* no 68881.  */
#endif /* no FIND_FRAME_SAVED_REGS.  */

/* Note that stuff for calling inferior functions is not in this file
   because the call dummy is different for different breakpoint
   instructions, which are different on different systems.  Perhaps
   they could be merged, but I haven't bothered.  */
