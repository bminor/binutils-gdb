/* Target machine definitions for GDB on a Sequent Symmetry under dynix 3.0,
   with Weitek 1167 and i387 support.
   Copyright 1986, 1987, 1989, 1991, 1992, 1993 Free Software Foundation, Inc.
   Symmetry version by Jay Vosburgh (fubar@sequent.com).

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

/* I don't know if this will work for cross-debugging, even if you do get
   a copy of the right include file.  */
#ifdef _SEQUENT_
/* ptx */
#include <sys/reg.h>
#else
/* dynix */
#include <machine/reg.h>
#endif

#ifdef _SEQUENT_
/* ptx, not dynix */
#define SDB_REG_TO_REGNUM(value) ptx_coff_regno_to_gdb(value)
extern int ptx_coff_regno_to_gdb();
#endif /* _SEQUENT_ */

#define START_INFERIOR_TRAPS_EXPECTED 2

/* Amount PC must be decremented by after a breakpoint.
   This is often the number of bytes in BREAKPOINT
   but not always.  */

#define DECR_PC_AFTER_BREAK 0

#include "i386/tm-i386v.h"

/* Nonzero if instruction at PC is a return instruction.  */
/* For Symmetry, this is really the 'leave' instruction, which */
/* is right before the ret */

#undef
#define ABOUT_TO_RETURN(pc) (read_memory_integer (pc, 1) == 0xc9)

#if 0
 --- this code can't be used unless we know we are running native,
     since it uses host specific ptrace calls.
/* code for 80387 fpu.  Functions are from i386-dep.c, copied into
 * symm-dep.c.
 */
#define FLOAT_INFO { i386_float_info(); }
#endif

/* Number of machine registers */
#undef NUM_REGS
#define NUM_REGS 49

/* Initializer for an array of names of registers.
   There should be NUM_REGS strings in this initializer.  */

/* Symmetry registers are in this weird order to match the register
   numbers in the symbol table entries.  If you change the order,
   things will probably break mysteriously for no apparent reason.
   Also note that the st(0)...st(7) 387 registers are represented as
   st0...st7.  */

#undef REGISTER_NAMES
#define REGISTER_NAMES { "eax", "edx", "ecx", "st0", "st1", \
			     "ebx", "esi", "edi", "st2", "st3", \
			     "st4", "st5", "st6", "st7", "esp", \
			     "ebp", "eip", "eflags", "fp1", "fp2", \
			     "fp3", "fp4", "fp5", "fp6", "fp7", \
			     "fp8", "fp9", "fp10", "fp11", "fp12", \
			     "fp13", "fp14", "fp15", "fp16", "fp17", \
			     "fp18", "fp19", "fp20", "fp21", "fp22", \
			     "fp23", "fp24", "fp25", "fp26", "fp27", \
			     "fp28", "fp29", "fp30", "fp31" }

/* Register numbers of various important registers.
   Note that some of these values are "real" register numbers,
   and correspond to the general registers of the machine,
   and some are "phony" register numbers which are too large
   to be actual register numbers as far as the user is concerned
   but do serve to get the desired values when passed to read_register.  */

#define EAX_REGNUM	0
#define EDX_REGNUM	1
#define ECX_REGNUM	2
#define ST0_REGNUM	3
#define ST1_REGNUM	4
#define EBX_REGNUM	5
#define ESI_REGNUM	6
#define EDI_REGNUM	7
#define ST2_REGNUM	8
#define ST3_REGNUM	9

#define ST4_REGNUM	10
#define ST5_REGNUM	11
#define ST6_REGNUM	12
#define ST7_REGNUM	13

#define FP1_REGNUM 18		/* first 1167 register */
/* Get %fp2 - %fp31 by addition, since they are contiguous */

#undef SP_REGNUM
#define SP_REGNUM 14		/* Contains address of top of stack */
#undef FP_REGNUM
#define FP_REGNUM 15		/* Contains address of executing stack frame */
#undef PC_REGNUM
#define PC_REGNUM 16		/* Contains program counter */
#undef PS_REGNUM
#define PS_REGNUM 17		/* Contains processor status */

#ifndef _SEQUENT_
/* dynix, not ptx.  For ptx, see register_addr in symm-tdep.c */

/* The magic numbers below are offsets into u_ar0 in the user struct.
 * They live in <machine/reg.h>.  Gdb calls this macro with blockend
 * holding u.u_ar0 - KERNEL_U_ADDR.  Only the registers listed are
 * saved in the u area (along with a few others that aren't useful
 * here.  See <machine/reg.h>).
 */

#define REGISTER_U_ADDR(addr, blockend, regno) \
{ struct user foo;	/* needed for finding fpu regs */ \
switch (regno) { \
    case 0: \
      addr = blockend + EAX * sizeof(int); break; \
  case 1: \
      addr = blockend + EDX * sizeof(int); break; \
  case 2: \
      addr = blockend + ECX * sizeof(int); break; \
  case 3:			/* st(0) */ \
      addr = blockend - \
	  ((int)&foo.u_fpusave.fpu_stack[0][0] - (int)&foo); \
      break; \
  case 4:			/* st(1) */ \
      addr = blockend - \
	  ((int) &foo.u_fpusave.fpu_stack[1][0] - (int)&foo); \
      break; \
  case 5: \
      addr = blockend + EBX * sizeof(int); break; \
  case 6: \
      addr = blockend + ESI * sizeof(int); break; \
  case 7: \
      addr = blockend + EDI * sizeof(int); break; \
  case 8:			/* st(2) */ \
      addr = blockend - \
	  ((int) &foo.u_fpusave.fpu_stack[2][0] - (int)&foo); \
      break; \
  case 9:			/* st(3) */ \
      addr = blockend - \
	  ((int) &foo.u_fpusave.fpu_stack[3][0] - (int)&foo); \
      break; \
  case 10:			/* st(4) */ \
      addr = blockend - \
	  ((int) &foo.u_fpusave.fpu_stack[4][0] - (int)&foo); \
      break; \
  case 11:			/* st(5) */ \
      addr = blockend - \
	  ((int) &foo.u_fpusave.fpu_stack[5][0] - (int)&foo); \
      break; \
  case 12:			/* st(6) */ \
      addr = blockend - \
	  ((int) &foo.u_fpusave.fpu_stack[6][0] - (int)&foo); \
      break; \
  case 13:			/* st(7) */ \
      addr = blockend - \
	  ((int) &foo.u_fpusave.fpu_stack[7][0] - (int)&foo); \
      break; \
  case 14: \
      addr = blockend + ESP * sizeof(int); break; \
  case 15: \
      addr = blockend + EBP * sizeof(int); break; \
  case 16: \
      addr = blockend + EIP * sizeof(int); break; \
  case 17: \
      addr = blockend + FLAGS * sizeof(int); break; \
  case 18:			/* fp1 */ \
  case 19:			/* fp2 */ \
  case 20:			/* fp3 */ \
  case 21:			/* fp4 */ \
  case 22:			/* fp5 */ \
  case 23:			/* fp6 */ \
  case 24:			/* fp7 */ \
  case 25:			/* fp8 */ \
  case 26:			/* fp9 */ \
  case 27:			/* fp10 */ \
  case 28:			/* fp11 */ \
  case 29:			/* fp12 */ \
  case 30:			/* fp13 */ \
  case 31:			/* fp14 */ \
  case 32:			/* fp15 */ \
  case 33:			/* fp16 */ \
  case 34:			/* fp17 */ \
  case 35:			/* fp18 */ \
  case 36:			/* fp19 */ \
  case 37:			/* fp20 */ \
  case 38:			/* fp21 */ \
  case 39:			/* fp22 */ \
  case 40:			/* fp23 */ \
  case 41:			/* fp24 */ \
  case 42:			/* fp25 */ \
  case 43:			/* fp26 */ \
  case 44:			/* fp27 */ \
  case 45:			/* fp28 */ \
  case 46:			/* fp29 */ \
  case 47:			/* fp30 */ \
  case 48:			/* fp31 */ \
     addr = blockend - \
	 ((int) &foo.u_fpasave.fpa_regs[(regno)-18] - (int)&foo); \
  } \
}
#endif /* _SEQUENT_ */

/* Total amount of space needed to store our copies of the machine's
   register state, the array `registers'.  */
/* 10 i386 registers, 8 i387 registers, and 31 Weitek 1167 registers */
#undef REGISTER_BYTES
#define REGISTER_BYTES ((10 * 4) + (8 * 10) + (31 * 4))

/* Index within `registers' of the first byte of the space for
   register N.  */

#undef REGISTER_BYTE
#define REGISTER_BYTE(N) 		\
((N < 3) ? (N * 4) :			\
(N < 5) ? (((N - 2) * 10) + 2) :	\
(N < 8) ? (((N - 5) * 4) + 32) :	\
(N < 14) ? (((N - 8) * 10) + 44) :	\
    (((N - 14) * 4) + 104))

/* Number of bytes of storage in the actual machine representation
 * for register N.  All registers are 4 bytes, except 387 st(0) - st(7),
 * which are 80 bits each. 
 */

#undef REGISTER_RAW_SIZE
#define REGISTER_RAW_SIZE(N) \
((N < 3) ? 4 :	\
(N < 5) ? 10 :	\
(N < 8) ? 4 :	\
(N < 14) ? 10 :	\
    4)

/* Largest value REGISTER_RAW_SIZE can have.  */

#undef MAX_REGISTER_RAW_SIZE
#define MAX_REGISTER_RAW_SIZE 10

/* Nonzero if register N requires conversion
   from raw format to virtual format.  */

#undef REGISTER_CONVERTIBLE
#define REGISTER_CONVERTIBLE(N) \
((N < 3) ? 0 : \
(N < 5) ? 1  : \
(N < 8) ? 0  : \
(N < 14) ? 1 : \
    0)

/* Convert data from raw format for register REGNUM
   to virtual format for register REGNUM.  */

#undef REGISTER_CONVERT_TO_VIRTUAL
#define REGISTER_CONVERT_TO_VIRTUAL(REGNUM,FROM,TO)	\
((REGNUM < 3) ? memcpy ((TO), (FROM), 4) : \
(REGNUM < 5) ? i387_to_double((FROM), (TO)) : \
(REGNUM < 8) ? memcpy ((TO), (FROM), 4) : \
(REGNUM < 14) ? i387_to_double((FROM), (TO)) : \
    memcpy ((TO), (FROM), 4))

extern void
i387_to_double PARAMS ((char *, char *));

/* Convert data from virtual format for register REGNUM
   to raw format for register REGNUM.  */

#undef REGISTER_CONVERT_TO_RAW
#define REGISTER_CONVERT_TO_RAW(REGNUM,FROM,TO)	\
((REGNUM < 3) ? memcpy ((TO), (FROM), 4) : \
(REGNUM < 5) ? double_to_i387((FROM), (TO)) : \
(REGNUM < 8) ? memcpy ((TO), (FROM), 4) : \
(REGNUM < 14) ? double_to_i387((FROM), (TO)) : \
    memcpy ((TO), (FROM), 4))

extern void
double_to_i387 PARAMS ((char *, char *));

/* Return the GDB type object for the "standard" data type
   of data in register N.  */

#undef REGISTER_VIRTUAL_TYPE
#define REGISTER_VIRTUAL_TYPE(N) \
((N < 3) ? builtin_type_int : \
(N < 5) ? builtin_type_double : \
(N < 8) ? builtin_type_int : \
(N < 14) ? builtin_type_double : \
    builtin_type_int)

/* from m-i386.h (now known as tm-i386v.h).  */
/* Store the address of the place in which to copy the structure the
   subroutine will return.  This is called from call_function.  FIXME:
   Why is it writing register 0?  Is the symmetry different from tm-i386v.h,
   or is it some sort of artifact?  FIXME.  */

#undef STORE_STRUCT_RETURN
#define STORE_STRUCT_RETURN(ADDR, SP) \
  { (SP) -= sizeof (ADDR);		\
    write_memory ((SP), &(ADDR), sizeof (ADDR)); \
    write_register(0, (ADDR)); }

/* Extract from an array REGBUF containing the (raw) register state
   a function return value of type TYPE, and copy that, in virtual format,
   into VALBUF.  */

#undef EXTRACT_RETURN_VALUE
#define EXTRACT_RETURN_VALUE(TYPE,REGBUF,VALBUF) \
  symmetry_extract_return_value(TYPE, REGBUF, VALBUF)


/* Things needed for making the inferior call functions.  FIXME: Merge
   this with the main 386 stuff.  */

#define PUSH_DUMMY_FRAME \
{  CORE_ADDR sp = read_register (SP_REGNUM); \
  int regnum; \
  sp = push_word (sp, read_register (PC_REGNUM)); \
  sp = push_word (sp, read_register (FP_REGNUM)); \
  write_register (FP_REGNUM, sp); \
  for (regnum = 0; regnum < NUM_REGS; regnum++) \
    sp = push_word (sp, read_register (regnum)); \
  write_register (SP_REGNUM, sp); \
}

#define POP_FRAME  \
{ \
  FRAME frame = get_current_frame (); \
  CORE_ADDR fp; \
  int regnum; \
  struct frame_saved_regs fsr; \
  struct frame_info *fi; \
  fi = get_frame_info (frame); \
  fp = fi->frame; \
  get_frame_saved_regs (fi, &fsr); \
  for (regnum = 0; regnum < NUM_REGS; regnum++) { \
      CORE_ADDR adr; \
      adr = fsr.regs[regnum]; \
      if (adr) \
	write_register (regnum, read_memory_integer (adr, 4)); \
  } \
  write_register (FP_REGNUM, read_memory_integer (fp, 4)); \
  write_register (PC_REGNUM, read_memory_integer (fp + 4, 4)); \
  write_register (SP_REGNUM, fp + 8); \
  flush_cached_frames (); \
  set_current_frame ( create_new_frame (read_register (FP_REGNUM), \
					read_pc ())); \
}

/* from i386-dep.c, worked better than my original... */
/* This sequence of words is the instructions
 * call (32-bit offset)
 * int 3
 * This is 6 bytes.
 */

#define CALL_DUMMY { 0x223344e8, 0xcc11 }

#define CALL_DUMMY_LENGTH 8

#define CALL_DUMMY_START_OFFSET 0  /* Start execution at beginning of dummy */

/* Insert the specified number of args and function address
   into a call sequence of the above form stored at DUMMYNAME.  */

#define FIX_CALL_DUMMY(dummyname, pc, fun, nargs, args, type, gcc_p)   \
{ \
	int from, to, delta, loc; \
	loc = (int)(read_register (SP_REGNUM) - CALL_DUMMY_LENGTH); \
	from = loc + 5; \
	to = (int)(fun); \
	delta = to - from; \
	*(int *)((char *)(dummyname) + 1) = delta; \
}

extern void
print_387_control_word PARAMS ((unsigned int));

extern void
print_387_status_word PARAMS ((unsigned int));
