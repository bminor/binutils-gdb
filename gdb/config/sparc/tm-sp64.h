/* Target machine sub-parameters for SPARC64, for GDB, the GNU debugger.
   This is included by other tm-*.h files to define SPARC cpu-related info.
   Copyright 1986, 1987, 1989, 1991, 1992, 1993 Free Software Foundation, Inc.
   This is (obviously) based on the SPARC Vn (n<9) port.
   Contributed by Doug Evans (dje@cygnus.com).

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

#include "sparc/tm-sparc.h"

/* We have long longs.  */
#define LONG_LONG

/* When passing a structure to a function, Sun cc passes the address
   in a register, not the structure itself.  It (under SunOS4) creates
   two symbols, so we get a LOC_ARG saying the address is on the stack
   (a lie, and a serious one since we don't know which register to
   use), and a LOC_REGISTER saying that the struct is in a register
   (sort of a lie, but fixable with REG_STRUCT_HAS_ADDR).  Gcc version
   two (as of 1.92) behaves like sun cc.  REG_STRUCT_HAS_ADDR is smart
   enough to distinguish between Sun cc, gcc version 1 and gcc version 2.

   This still doesn't work if the argument is not one passed in a
   register (i.e. it's the 7th or later argument).  */

#undef  REG_STRUCT_HAS_ADDR
#define REG_STRUCT_HAS_ADDR(gcc_p) (gcc_p != 1)
#undef  STRUCT_ARG_SYM_GARBAGE
#define STRUCT_ARG_SYM_GARBAGE(gcc_p) (gcc_p != 1)

/* Stack has strict alignment.  */

#undef  STACK_ALIGN
#define STACK_ALIGN(ADDR) (((ADDR)+15)&-16)

/* Nonzero if instruction at PC is a return instruction.  */
/* For SPARC, this is either a "jmpl %o7+8,%g0" or "jmpl %i7+8,%g0".

   Note: this does not work for functions returning structures under SunOS.
   This should work for v9, however.  */
#undef  ABOUT_TO_RETURN
#define ABOUT_TO_RETURN(pc) \
  ((read_memory_integer (pc, 4)|0x00040000) == 0x81c7e008)

/* Say how long (ordinary) registers are.  */

#undef  REGISTER_TYPE
#define REGISTER_TYPE long long

/* Number of machine registers */

#undef  NUM_REGS
#define NUM_REGS 127

/* Initializer for an array of names of registers.
   There should be NUM_REGS strings in this initializer.  */
/* Some of these registers are only accessible from priviledged mode.
   They are here for kernel debuggers, etc.  */
/* FIXME: icc and xcc are currently considered separate registers.
   This may have to change and consider them as just one (ccr).
   Let's postpone this as long as we can.  */
/* FIXME: fcc0-3 are currently separate, even though they are also part of
   fsr.  May have to remove them but let's postpone this as long as
   possible.  */
/* FIXME: cle and tle are new registers that haven't entered the docs yet.
   They stand for "current little endian format" and "trap little endian
   format".  */

#undef  REGISTER_NAMES
#define REGISTER_NAMES  \
{ "g0", "g1", "g2", "g3", "g4", "g5", "g6", "g7",	\
  "o0", "o1", "o2", "o3", "o4", "o5", "sp", "o7",	\
  "l0", "l1", "l2", "l3", "l4", "l5", "l6", "l7",	\
  "i0", "i1", "i2", "i3", "i4", "i5", "fp", "i7",	\
								\
  "f0", "f1", "f2", "f3", "f4", "f5", "f6", "f7",		\
  "f8", "f9", "f10", "f11", "f12", "f13", "f14", "f15",		\
  "f16", "f17", "f18", "f19", "f20", "f21", "f22", "f23",	\
  "f24", "f25", "f26", "f27", "f28", "f29", "f30", "f31",	\
  "f32", "f34", "f36", "f38", "f40", "f42", "f44", "f46",	\
  "f48", "f50", "f52", "f54", "f56", "f58", "f60", "f62",	\
                                                                \
  "ccr", "y", "pc", "npc", "asi", "cle", "tle",			\
  "fsr", "fprs", "ver", "tick", "pil", "pstate", "wstate",	\
  "tba", "tl", "tt", "tpc", "tnpc", "tstate",			\
  "cwp", "cansave", "canrestore", "cleanwin", "otherwin",	\
  "asr16", "asr17", "asr18", "asr19", "asr20", "asr21",		\
  "asr22", "asr23", "asr24", "asr25", "asr26", "asr27",		\
  "asr28", "asr29", "asr30", "asr31",				\
  /* These are here at the end to simplify removing them if we have to.  */ \
  "icc", "xcc", "fcc0", "fcc1", "fcc2", "fcc3"			\
}

/* Register numbers of various important registers.
   Note that some of these values are "real" register numbers,
   and correspond to the general registers of the machine,
   and some are "phony" register numbers which are too large
   to be actual register numbers as far as the user is concerned
   but do serve to get the desired values when passed to read_register.  */

#if 0 /* defined in tm-sparc.h, replicated for doc purposes */
#define	G0_REGNUM 0             /* %g0 */
#define	G1_REGNUM 1		/* %g1 */
#define O0_REGNUM 8		/* %o0 */
#define	SP_REGNUM 14		/* Contains address of top of stack, \
				   which is also the bottom of the frame.  */
#define	RP_REGNUM 15		/* Contains return address value, *before* \
				   any windows get switched.  */
#define	O7_REGNUM 15		/* Last local reg not saved on stack frame */
#define	L0_REGNUM 16		/* First local reg that's saved on stack frame
				   rather than in machine registers */
#define	I0_REGNUM 24		/* %i0 */
#define	FP_REGNUM 30		/* Contains address of executing stack frame */
#define	I7_REGNUM 31		/* Last local reg saved on stack frame */
#define	FP0_REGNUM 32		/* Floating point register 0 */
#endif

#define FP_MAX_REGNUM 80	/* 1 + last fp reg number */

/* #undef v8 misc. regs */

#undef Y_REGNUM
#undef PS_REGNUM
#undef WIM_REGNUM
#undef TBR_REGNUM
#undef PC_REGNUM
#undef NPC_REGNUM
#undef FPS_REGNUM
#undef CPS_REGNUM

/* v9 misc. and priv. regs */

#define C0_REGNUM 80				/* Start of control registers */
#define CCR_REGNUM (C0_REGNUM + 0)		/* Condition Code Register (%xcc,%icc) */
#define	Y_REGNUM (C0_REGNUM + 1)		/* Temp register for multiplication, etc.  */
#define PC_REGNUM (C0_REGNUM + 2)		/* floating point condition code reg 2 */
#define NPC_REGNUM (C0_REGNUM + 3)		/* floating point condition code reg 2 */
#define ASI_REGNUM (C0_REGNUM + 4)		/* Alternate Space Identifier */
#define CLE_REGNUM (C0_REGNUM + 5)		/* Current Little Endian format */
#define TLE_REGNUM (C0_REGNUM + 6)		/* Trap Little Endian format */
#define FSR_REGNUM (C0_REGNUM + 7)		/* Floating Point State */
#define FPRS_REGNUM (C0_REGNUM + 8)		/* Floating Point Registers State */
#define VER_REGNUM (C0_REGNUM + 9)		/* Version register */
#define TICK_REGNUM (C0_REGNUM + 10)		/* Tick register */
#define PIL_REGNUM (C0_REGNUM + 11)		/* Processor Interrupt Level */
#define PSTATE_REGNUM (C0_REGNUM + 12)		/* Processor State */
#define WSTATE_REGNUM (C0_REGNUM + 13)		/* Window State */
#define TBA_REGNUM (C0_REGNUM + 14)		/* floating point condition code reg 3 */
#define TL_REGNUM (C0_REGNUM + 15)		/* Trap Level */
#define TT_REGNUM (C0_REGNUM + 16)		/* Trap Type */
#define TPC_REGNUM (C0_REGNUM + 17)		/* Trap pc */
#define TNPC_REGNUM (C0_REGNUM + 18)		/* Trap npc */
#define TSTATE_REGNUM (C0_REGNUM + 19)		/* Trap State */
#define CWP_REGNUM (C0_REGNUM + 20)		/* Current Window Pointer */
#define CANSAVE_REGNUM (C0_REGNUM + 21)		/* Savable Windows */
#define CANRESTORE_REGNUM (C0_REGNUM + 22)	/* Restorable Windows */
#define CLEANWIN_REGNUM (C0_REGNUM + 23)	/* Clean Windows */
#define OTHERWIN_REGNUM (C0_REGNUM + 24)	/* Other Windows */
#define ASR_REGNUM(n) (C0_REGNUM+(25-16)+(n))	/* Ancillary State Register
						   (n = 16...31) */
#define ICC_REGNUM (C0_REGNUM + 41)		/* 32 bit condition codes */
#define XCC_REGNUM (C0_REGNUM + 42)		/* 64 bit condition codes */
#define FCC0_REGNUM (C0_REGNUM + 43)		/* floatpoint condition code reg 0 */
#define FCC1_REGNUM (C0_REGNUM + 44)		/* floating point condition code reg 0 */
#define FCC2_REGNUM (C0_REGNUM + 45)		/* floating point condition code reg 1 */
#define FCC3_REGNUM (C0_REGNUM + 46)		/* floating point condition code reg 2 */

/* FIXME: PS_REGNUM, FPS_REGNUM, CPS_REGNUM are for priviledged v8 registers
   which don't exist in v9 (in the same form).  We use bits of sparc-tdep.c
   which requires these, so define them here to be unused ASR regs so
   sparc-tdep.c will compile.  What we really want to do is put some
   conditionals in sparc-tdep.c (run time or compile time) or separate the v8
   stuff out of sparc-tdep.c.  */

#define PS_REGNUM (ASR_REGNUM (29))
#define FPS_REGNUM (ASR_REGNUM (30))
#define CPS_REGNUM (ASR_REGNUM (31))

/* Total amount of space needed to store our copies of the machine's
   register state, the array `registers'.
   Some of the registers aren't 64 bits, but it's a lot simpler just to assume
   they all are (since most of them are).  */
#undef  REGISTER_BYTES
#define REGISTER_BYTES (32*8+32*8+47*8)

/* Index within `registers' of the first byte of the space for
   register N.  */
#undef  REGISTER_BYTE
#define REGISTER_BYTE(N) \
  ((N) < 32 ? (N)*8				\
   : (N) < 64 ? 32*8 + ((N)-32)*4		\
   : (N) < C0_REGNUM ? 32*8 + 32*4 + ((N)-64)*8	\
   : 64*8 + ((N)-C0_REGNUM)*8)

/* Number of bytes of storage in the actual machine representation
   for register N.  */

#undef  REGISTER_RAW_SIZE
#define REGISTER_RAW_SIZE(N) (8)

/* Number of bytes of storage in the program's representation
   for register N.  */

#undef  REGISTER_VIRTUAL_SIZE
#define REGISTER_VIRTUAL_SIZE(N) (8)

/* Largest value REGISTER_RAW_SIZE can have.  */
/* tm-sparc.h defines this as 8, but play it safe.  */

#undef  MAX_REGISTER_RAW_SIZE
#define MAX_REGISTER_RAW_SIZE (8)

/* Largest value REGISTER_VIRTUAL_SIZE can have.  */
/* tm-sparc.h defines this as 8, but play it safe.  */

#undef  MAX_REGISTER_VIRTUAL_SIZE
#define MAX_REGISTER_VIRTUAL_SIZE (8)

/* Convert data from raw format for register REGNUM
   to virtual format for register REGNUM.  */
/* ??? Remove when tm-sparc.h is fixed.  */

#undef  REGISTER_CONVERT_TO_VIRTUAL
#define REGISTER_CONVERT_TO_VIRTUAL(REGNUM,FROM,TO) \
{ memcpy ((TO), (FROM), REGISTER_RAW_SIZE (0)); }

/* Convert data from virtual format for register REGNUM
   to raw format for register REGNUM.  */
/* ??? Remove when tm-sparc.h is fixed.  */

#undef  REGISTER_CONVERT_TO_RAW
#define REGISTER_CONVERT_TO_RAW(REGNUM,FROM,TO)	\
{ memcpy ((TO), (FROM), REGISTER_RAW_SIZE (0)); }

/* Return the GDB type object for the "standard" data type
   of data in register N.  */

#undef  REGISTER_VIRTUAL_TYPE
#define REGISTER_VIRTUAL_TYPE(N) \
 ((N) < 32 ? builtin_type_long_long : (N) < 80 ? builtin_type_float : \
  builtin_type_long_long)

/* We use to support both 32 bit and 64 bit pointers.
   We can't anymore because TARGET_PTR_BIT must now be a constant.  */
#undef  TARGET_PTR_BIT
#define TARGET_PTR_BIT 64

/* Store the address of the place in which to copy the structure the
   subroutine will return.  This is called from call_function. */
/* FIXME: V9 uses %o0 for this.  */

#undef  STORE_STRUCT_RETURN
#define STORE_STRUCT_RETURN(ADDR, SP) \
  { target_write_memory ((SP)+(16*8), (char *)&(ADDR), 8); }

/* Extract from an array REGBUF containing the (raw) register state
   a function return value of type TYPE, and copy that, in virtual format,
   into VALBUF.  */ /* FIXME */

#undef  EXTRACT_RETURN_VALUE
#define EXTRACT_RETURN_VALUE(TYPE,REGBUF,VALBUF)	      \
  {      	       	       	       	       	       	       	           \
    if (TYPE_CODE (TYPE) == TYPE_CODE_FLT)		       		   \
      {							       		   \
	memcpy ((VALBUF), ((int *)(REGBUF))+FP0_REGNUM, TYPE_LENGTH(TYPE));\
      }							       		   \
    else						       		   \
      memcpy ((VALBUF),						   	   \
	      (char *)(REGBUF) + 8 * 8 +				   \
	      (TYPE_LENGTH(TYPE) >= 8 ? 0 : 8 - TYPE_LENGTH(TYPE)),	   \
	      TYPE_LENGTH(TYPE));					   \
  }

/* Extract from an array REGBUF containing the (raw) register state
   the address in which a function should return its structure value,
   as a CORE_ADDR (or an expression that can be used as one).  */

#undef  EXTRACT_STRUCT_VALUE_ADDRESS
#define EXTRACT_STRUCT_VALUE_ADDRESS(REGBUF) \
  (sparc64_extract_struct_value_address (REGBUF))

extern CORE_ADDR
sparc64_extract_struct_value_address PARAMS ((char [REGISTER_BYTES]));

/* Return number of bytes at start of arglist that are not really args.  */

#undef  FRAME_ARGS_SKIP
#define FRAME_ARGS_SKIP 136

/* Sparc has no reliable single step ptrace call */

#undef  NO_SINGLE_STEP
#define NO_SINGLE_STEP 1
extern void single_step ();

/* We need two arguments (in general) to the "info frame" command.
   Note that the definition of this macro implies that there exists a
   function "setup_arbitrary_frame" in sparc-tdep.c */

#undef  SETUP_ARBITRARY_FRAME		/*FIXME*/
#undef  FRAME_SPECIFICATION_DYADIC
#define FRAME_SPECIFICATION_DYADIC

/* To print every pair of float registers as a double, we use this hook.
   We also print the condition code registers in a readable format
   (FIXME: can expand this to all control regs).  */

#undef 	PRINT_REGISTER_HOOK
#define	PRINT_REGISTER_HOOK(regno)	\
  sparc_print_register_hook (regno)

/* Offsets into jmp_buf.
   FIXME: This was borrowed from the v8 stuff and will probably have to change
   for v9.  */

#define JB_ELEMENT_SIZE 8	/* Size of each element in jmp_buf */

#define JB_ONSSTACK 0
#define JB_SIGMASK 1
#define JB_SP 2
#define JB_PC 3
#define JB_NPC 4
#define JB_PSR 5
#define JB_G1 6
#define JB_O0 7
#define JB_WBCNT 8

/* Figure out where the longjmp will land.  We expect that we have just entered
   longjmp and haven't yet setup the stack frame, so the args are still in the
   output regs.  %o0 (O0_REGNUM) points at the jmp_buf structure from which we
   extract the pc (JB_PC) that we will land at.  The pc is copied into ADDR.
   This routine returns true on success */

extern int
get_longjmp_target PARAMS ((CORE_ADDR *));

#define GET_LONGJMP_TARGET(ADDR) get_longjmp_target(ADDR)
