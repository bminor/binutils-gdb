/* Definitions to target GDB to ARM targets.
   Copyright 1986, 1987, 1988, 1989, 1991, 1993, 1994, 1995, 1996, 1997,
   1998, 1999, 2000, 2001, 2002 Free Software Foundation, Inc.

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

#ifndef TM_ARM_H
#define TM_ARM_H

#ifndef GDB_MULTI_ARCH
#define GDB_MULTI_ARCH 1
#endif

#include "regcache.h"
#include "floatformat.h"

/* Forward declarations for prototypes.  */
struct type;
struct value;

/* IEEE format floating point.  */
#define TARGET_DOUBLE_FORMAT  (TARGET_BYTE_ORDER == BFD_ENDIAN_BIG \
			       ? &floatformat_ieee_double_big	 \
			       : &floatformat_ieee_double_littlebyte_bigword)

CORE_ADDR arm_smash_text_address(CORE_ADDR);
#define SMASH_TEXT_ADDRESS(ADDR) arm_smash_text_address (ADDR)

CORE_ADDR arm_addr_bits_remove (CORE_ADDR);
#define ADDR_BITS_REMOVE(VAL)	arm_addr_bits_remove (VAL)

/* Offset from address of function to start of its code.  Zero on most
   machines.  */

#define FUNCTION_START_OFFSET	0

/* Advance PC across any function entry prologue instructions to reach
   some "real" code.  */

extern CORE_ADDR arm_skip_prologue (CORE_ADDR pc);

#define SKIP_PROLOGUE(pc)  (arm_skip_prologue (pc))

/* Immediately after a function call, return the saved pc.  Can't
   always go through the frames for this because on some machines the
   new frame is not set up until the new function executes some
   instructions.  */

#define SAVED_PC_AFTER_CALL(frame)  arm_saved_pc_after_call (frame)
struct frame_info;
extern CORE_ADDR arm_saved_pc_after_call (struct frame_info *);

/* The following define instruction sequences that will cause ARM
   cpu's to take an undefined instruction trap.  These are used to
   signal a breakpoint to GDB.
   
   The newer ARMv4T cpu's are capable of operating in ARM or Thumb
   modes.  A different instruction is required for each mode.  The ARM
   cpu's can also be big or little endian.  Thus four different
   instructions are needed to support all cases.
   
   Note: ARMv4 defines several new instructions that will take the
   undefined instruction trap.  ARM7TDMI is nominally ARMv4T, but does
   not in fact add the new instructions.  The new undefined
   instructions in ARMv4 are all instructions that had no defined
   behaviour in earlier chips.  There is no guarantee that they will
   raise an exception, but may be treated as NOP's.  In practice, it
   may only safe to rely on instructions matching:
   
   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 
   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
   C C C C 0 1 1 x x x x x x x x x x x x x x x x x x x x 1 x x x x
   
   Even this may only true if the condition predicate is true. The
   following use a condition predicate of ALWAYS so it is always TRUE.
   
   There are other ways of forcing a breakpoint.  ARM Linux, RISC iX,
   and NetBSD will all use a software interrupt rather than an
   undefined instruction to force a trap.  This can be handled by
   redefining some or all of the following in a target dependent
   fashion.  */

#define ARM_LE_BREAKPOINT {0xFE,0xDE,0xFF,0xE7}
#define ARM_BE_BREAKPOINT {0xE7,0xFF,0xDE,0xFE}
#define THUMB_LE_BREAKPOINT {0xfe,0xdf}
#define THUMB_BE_BREAKPOINT {0xdf,0xfe}

/* Stack grows downward.  */

#define INNER_THAN(lhs,rhs) ((lhs) < (rhs))

/* !!!! if we're using RDP, then we're inserting breakpoints and
   storing their handles instread of what was in memory.  It is nice
   that this is the same size as a handle - otherwise remote-rdp will
   have to change. */

/* BREAKPOINT_FROM_PC uses the program counter value to determine
   whether a 16- or 32-bit breakpoint should be used.  It returns a
   pointer to a string of bytes that encode a breakpoint instruction,
   stores the length of the string to *lenptr, and adjusts the pc (if
   necessary) to point to the actual memory location where the
   breakpoint should be inserted.  */

extern breakpoint_from_pc_fn arm_breakpoint_from_pc;
#define BREAKPOINT_FROM_PC(pcptr, lenptr) arm_breakpoint_from_pc (pcptr, lenptr)

/* Amount PC must be decremented by after a breakpoint.  This is often
   the number of bytes in BREAKPOINT but not always.  */

#define DECR_PC_AFTER_BREAK 0

void arm_print_float_info (void);
#define PRINT_FLOAT_INFO()	arm_print_float_info ()

/* Say how long (ordinary) registers are.  This is a piece of bogosity
   used in push_word and a few other places; REGISTER_RAW_SIZE is the
   real way to know how big a register is.  */

#define REGISTER_SIZE	4

/* Say how long FP registers are.  Used for documentation purposes and
   code readability in this header.  IEEE extended doubles are 80
   bits.  DWORD aligned they use 96 bits.  */
#define FP_REGISTER_RAW_SIZE	12

/* GCC doesn't support long doubles (extended IEEE values).  The FP
   register virtual size is therefore 64 bits.  Used for documentation
   purposes and code readability in this header.  */
#define FP_REGISTER_VIRTUAL_SIZE	8

/* Status registers are the same size as general purpose registers.
   Used for documentation purposes and code readability in this
   header.  */
#define STATUS_REGISTER_SIZE	REGISTER_SIZE

/* Number of machine registers.  The only define actually required 
   is NUM_REGS.  The other definitions are used for documentation
   purposes and code readability.  */
/* For 26 bit ARM code, a fake copy of the PC is placed in register 25 (PS)
   (and called PS for processor status) so the status bits can be cleared
   from the PC (register 15).  For 32 bit ARM code, a copy of CPSR is placed
   in PS.  */
#define NUM_FREGS	8	/* Number of floating point registers.  */
#define NUM_SREGS	2	/* Number of status registers.  */
#define NUM_GREGS	16	/* Number of general purpose registers.  */
#define NUM_REGS	(NUM_GREGS + NUM_FREGS + NUM_SREGS)

/* An array of names of registers. */
extern char **arm_register_names;

#define REGISTER_NAME(i) arm_register_name(i)
char *arm_register_name (int);

/* Register numbers of various important registers.  Note that some of
   these values are "real" register numbers, and correspond to the
   general registers of the machine, and some are "phony" register
   numbers which are too large to be actual register numbers as far as
   the user is concerned but do serve to get the desired values when
   passed to read_register.  */

#define A1_REGNUM 0		/* first integer-like argument */
#define A4_REGNUM 3		/* last integer-like argument */
#define AP_REGNUM 11
#define FP_REGNUM 11		/* Contains address of executing stack frame */
#define SP_REGNUM 13		/* Contains address of top of stack */
#define LR_REGNUM 14		/* address to return to from a function call */
#define PC_REGNUM 15		/* Contains program counter */
#define F0_REGNUM 16		/* first floating point register */
#define F3_REGNUM 19		/* last floating point argument register */
#define F7_REGNUM 23		/* last floating point register */
#define FPS_REGNUM 24		/* floating point status register */
#define PS_REGNUM 25		/* Contains processor status */

#define THUMB_FP_REGNUM 7	/* R7 is frame register on Thumb */

#define ARM_NUM_ARG_REGS 	4
#define ARM_LAST_ARG_REGNUM 	A4_REGNUM
#define ARM_NUM_FP_ARG_REGS 	4
#define ARM_LAST_FP_ARG_REGNUM	F3_REGNUM

/* Instruction condition field values.  */
#define INST_EQ		0x0
#define INST_NE		0x1
#define INST_CS		0x2
#define INST_CC		0x3
#define INST_MI		0x4
#define INST_PL		0x5
#define INST_VS		0x6
#define INST_VC		0x7
#define INST_HI		0x8
#define INST_LS		0x9
#define INST_GE		0xa
#define INST_LT		0xb
#define INST_GT		0xc
#define INST_LE		0xd
#define INST_AL		0xe
#define INST_NV		0xf

#define FLAG_N		0x80000000
#define FLAG_Z		0x40000000
#define FLAG_C		0x20000000
#define FLAG_V		0x10000000



/* Total amount of space needed to store our copies of the machine's
   register state, the array `registers'.  */

#define REGISTER_BYTES ((NUM_GREGS * REGISTER_SIZE) + \
			(NUM_FREGS * FP_REGISTER_RAW_SIZE) + \
			(NUM_SREGS * STATUS_REGISTER_SIZE))

/* Index within `registers' of the first byte of the space for
   register N.  */

#define REGISTER_BYTE(N) \
     ((N) < F0_REGNUM \
      ? (N) * REGISTER_SIZE \
      : ((N) < PS_REGNUM \
	 ? (NUM_GREGS * REGISTER_SIZE + \
	    ((N) - F0_REGNUM) * FP_REGISTER_RAW_SIZE) \
	 : (NUM_GREGS * REGISTER_SIZE + \
	    NUM_FREGS * FP_REGISTER_RAW_SIZE + \
	    ((N) - FPS_REGNUM) * STATUS_REGISTER_SIZE)))

/* Number of bytes of storage in the actual machine representation for
   register N.  All registers are 4 bytes, except fp0 - fp7, which are
   12 bytes in length.  */
#define REGISTER_RAW_SIZE(N) \
     ((N) < F0_REGNUM ? REGISTER_SIZE : \
      (N) < FPS_REGNUM ? FP_REGISTER_RAW_SIZE : STATUS_REGISTER_SIZE)

/* Number of bytes of storage in a program's representation
   for register N.  */
#define REGISTER_VIRTUAL_SIZE(N) \
	((N) < F0_REGNUM ? REGISTER_SIZE : \
	 (N) < FPS_REGNUM ? FP_REGISTER_VIRTUAL_SIZE : STATUS_REGISTER_SIZE)

/* Largest value REGISTER_RAW_SIZE can have.  */

#define MAX_REGISTER_RAW_SIZE FP_REGISTER_RAW_SIZE

/* Largest value REGISTER_VIRTUAL_SIZE can have.  */
#define MAX_REGISTER_VIRTUAL_SIZE FP_REGISTER_VIRTUAL_SIZE

/* Return the GDB type object for the "standard" data type of data in
   register N.  */

extern struct type *arm_register_type (int regnum);
#define REGISTER_VIRTUAL_TYPE(N) arm_register_type (N)

/* The system C compiler uses a similar structure return convention to gcc */
extern use_struct_convention_fn arm_use_struct_convention;
#define USE_STRUCT_CONVENTION(gcc_p, type) \
     arm_use_struct_convention (gcc_p, type)

/* Store the address of the place in which to copy the structure the
   subroutine will return.  This is called from call_function. */

#define STORE_STRUCT_RETURN(ADDR, SP) \
     write_register (A1_REGNUM, (ADDR))

/* Extract from an array REGBUF containing the (raw) register state a
   function return value of type TYPE, and copy that, in virtual
   format, into VALBUF.  */

extern void arm_extract_return_value (struct type *, char[], char *);
#define EXTRACT_RETURN_VALUE(TYPE,REGBUF,VALBUF) \
     arm_extract_return_value ((TYPE), (REGBUF), (VALBUF))

/* Write into appropriate registers a function return value of type
   TYPE, given in virtual format.  */

extern void convert_to_extended (void *dbl, void *ptr);
#define STORE_RETURN_VALUE(TYPE,VALBUF) \
  if (TYPE_CODE (TYPE) == TYPE_CODE_FLT) {				\
    char _buf[MAX_REGISTER_RAW_SIZE];					\
    convert_to_extended (VALBUF, _buf); 					\
    write_register_bytes (REGISTER_BYTE (F0_REGNUM), _buf, MAX_REGISTER_RAW_SIZE); \
  } else								\
    write_register_bytes (0, VALBUF, TYPE_LENGTH (TYPE))

/* Extract from an array REGBUF containing the (raw) register state
   the address in which a function should return its structure value,
   as a CORE_ADDR (or an expression that can be used as one).  */

#define EXTRACT_STRUCT_VALUE_ADDRESS(REGBUF) \
  (extract_address ((PTR)(REGBUF), REGISTER_RAW_SIZE(0)))

/* Specify that for the native compiler variables for a particular
   lexical context are listed after the beginning LBRAC instead of
   before in the executables list of symbols.  */
#define VARIABLES_INSIDE_BLOCK(desc, gcc_p) (!(gcc_p))

#define CALL_DUMMY_WORDS arm_call_dummy_words
extern LONGEST arm_call_dummy_words[];

#define SIZEOF_CALL_DUMMY_WORDS (3 * sizeof (LONGEST))

#define CALL_DUMMY_START_OFFSET	 0	/* Start execution at beginning of dummy */

#define CALL_DUMMY_BREAKPOINT_OFFSET arm_call_dummy_breakpoint_offset()
extern int arm_call_dummy_breakpoint_offset (void);

/* Insert the specified number of args and function address into a
   call sequence of the above form stored at DUMMYNAME.  */

#define FIX_CALL_DUMMY(dummyname, pc, fun, nargs, args, type, gcc_p) \
   arm_fix_call_dummy ((dummyname), (pc), (fun), (nargs), (args), (type), (gcc_p))

void arm_fix_call_dummy (char *dummy, CORE_ADDR pc, CORE_ADDR fun,
			 int nargs, struct value ** args,
			 struct type * type, int gcc_p);

/* Most ARMs don't have single stepping capability, so provide a 
   single-stepping mechanism by default */
#undef SOFTWARE_SINGLE_STEP_P
#define SOFTWARE_SINGLE_STEP_P() 1

#define SOFTWARE_SINGLE_STEP(sig,bpt) arm_software_single_step((sig), (bpt))
void arm_software_single_step (int, int);

struct minimal_symbol;

void arm_elf_make_msymbol_special(asymbol *, struct minimal_symbol *);
#define ELF_MAKE_MSYMBOL_SPECIAL(SYM,MSYM) \
	arm_elf_make_msymbol_special (SYM, MSYM)

void arm_coff_make_msymbol_special(int, struct minimal_symbol *);
#define COFF_MAKE_MSYMBOL_SPECIAL(VAL,MSYM) \
	arm_coff_make_msymbol_special (VAL, MSYM)

/* The first 0x20 bytes are the trap vectors.  */
#define LOWEST_PC	0x20

#endif /* TM_ARM_H */
