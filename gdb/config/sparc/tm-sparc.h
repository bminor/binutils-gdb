/* Target machine sub-parameters for SPARC, for GDB, the GNU debugger.
   This is included by other tm-*.h files to define SPARC cpu-related info.
   Copyright 1986, 1987, 1989, 1991, 1992, 1993, 1994, 1995, 1996, 1997,
   1998, 1999, 2000, 2001, 2002, 2003
   Free Software Foundation, Inc.
   Contributed by Michael Tiemann (tiemann@mcc.com)

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

struct type;
struct value;
struct frame_info;

/*
 * The following enums are purely for the convenience of the GDB 
 * developer, when debugging GDB.
 */

enum {	/* Sparc general registers, for all sparc versions.  */
  G0_REGNUM, G1_REGNUM, G2_REGNUM, G3_REGNUM, 
  G4_REGNUM, G5_REGNUM, G6_REGNUM, G7_REGNUM, 
  O0_REGNUM, O1_REGNUM, O2_REGNUM, O3_REGNUM, 
  O4_REGNUM, O5_REGNUM, O6_REGNUM, O7_REGNUM,
  L0_REGNUM, L1_REGNUM, L2_REGNUM, L3_REGNUM, 
  L4_REGNUM, L5_REGNUM, L6_REGNUM, L7_REGNUM, 
  I0_REGNUM, I1_REGNUM, I2_REGNUM, I3_REGNUM, 
  I4_REGNUM, I5_REGNUM, I6_REGNUM, I7_REGNUM,
  FP0_REGNUM 		/* Floating point register 0 */
};

enum {	/* Sparc general registers, alternate names.  */
  R0_REGNUM,  R1_REGNUM,  R2_REGNUM,  R3_REGNUM, 
  R4_REGNUM,  R5_REGNUM,  R6_REGNUM,  R7_REGNUM, 
  R8_REGNUM,  R9_REGNUM,  R10_REGNUM, R11_REGNUM, 
  R12_REGNUM, R13_REGNUM, R14_REGNUM, R15_REGNUM, 
  R16_REGNUM, R17_REGNUM, R18_REGNUM, R19_REGNUM, 
  R20_REGNUM, R21_REGNUM, R22_REGNUM, R23_REGNUM, 
  R24_REGNUM, R25_REGNUM, R26_REGNUM, R27_REGNUM, 
  R28_REGNUM, R29_REGNUM, R30_REGNUM, R31_REGNUM
};

enum {			/* Sparc32 control registers.  */
  PS_REGNUM  = 65, 	/* PC, NPC, and Y are omitted because */
  WIM_REGNUM = 66,	/* they have different values depending on */
  TBR_REGNUM = 67,	/* 32-bit / 64-bit mode.  */
  FPS_REGNUM = 70,
  CPS_REGNUM = 71
};

/* v9 misc. and priv. regs */

/* Note: specifying values explicitly for documentation purposes.  */
enum {	/* Sparc64 control registers, excluding Y, PC, and NPC.  */
  CCR_REGNUM = 82,		/* Condition Code Register (%xcc,%icc) */
  FSR_REGNUM = 83,		/* Floating Point State */
  FPRS_REGNUM = 84,		/* Floating Point Registers State */
  ASI_REGNUM = 86,		/* Alternate Space Identifier */
  VER_REGNUM = 87,		/* Version register */
  TICK_REGNUM = 88,		/* Tick register */
  PIL_REGNUM = 89, 		/* Processor Interrupt Level */
  PSTATE_REGNUM = 90,		/* Processor State */
  TSTATE_REGNUM = 91,		/* Trap State */
  TBA_REGNUM = 92,		/* Trap Base Address */
  TL_REGNUM = 93,		/* Trap Level */
  TT_REGNUM = 94,		/* Trap Type */
  TPC_REGNUM = 95,		/* Trap pc */
  TNPC_REGNUM = 96,		/* Trap npc */
  WSTATE_REGNUM = 97,		/* Window State */
  CWP_REGNUM = 98,		/* Current Window Pointer */
  CANSAVE_REGNUM = 99,		/* Savable Windows */
  CANRESTORE_REGNUM = 100,	/* Restorable Windows */
  CLEANWIN_REGNUM = 101,	/* Clean Windows */
  OTHERWIN_REGNUM = 102,	/* Other Windows */
  ASR16_REGNUM = 103,		/* Ancillary State Registers */
  ASR17_REGNUM = 104,
  ASR18_REGNUM = 105,
  ASR19_REGNUM = 106,
  ASR20_REGNUM = 107,
  ASR21_REGNUM = 108,
  ASR22_REGNUM = 109,
  ASR23_REGNUM = 110,
  ASR24_REGNUM = 111,
  ASR25_REGNUM = 112,
  ASR26_REGNUM = 113,
  ASR27_REGNUM = 114,
  ASR28_REGNUM = 115,
  ASR29_REGNUM = 116,
  ASR30_REGNUM = 117,
  ASR31_REGNUM = 118,
  ICC_REGNUM   = 119,		/* 32 bit condition codes */
  XCC_REGNUM   = 120,		/* 64 bit condition codes */
  FCC0_REGNUM  = 121,		/* fp cc reg 0 */
  FCC1_REGNUM  = 122,		/* fp cc reg 1 */
  FCC2_REGNUM  = 123,		/* fp cc reg 2 */
  FCC3_REGNUM  = 124		/* fp cc reg 3 */
};

/*
 * Make sparc target multi-archable: April 2000
 */

/* Multi-arch definition of TARGET_IS_SPARC64, TARGET_ELF64 */
#undef  GDB_TARGET_IS_SPARC64
#define GDB_TARGET_IS_SPARC64 \
     (sparc_intreg_size () == 8)
#undef  TARGET_ELF64
#define TARGET_ELF64 \
     (sparc_intreg_size () == 8)
extern int sparc_intreg_size (void);

/*
 * The following defines should ONLY appear for MULTI_ARCH.
 */

/* Multi-arch the nPC and Y registers.  */
#define Y_REGNUM              (sparc_y_regnum ())

/* On the Sun 4 under SunOS, the compile will leave a fake insn which
   encodes the structure size being returned.  If we detect such
   a fake insn, step past it.  */

#define PC_ADJUST(PC) sparc_pc_adjust (PC)
extern CORE_ADDR sparc_pc_adjust (CORE_ADDR);

/* If an argument is declared "register", Sun cc will keep it in a register,
   never saving it onto the stack.  So we better not believe the "p" symbol
   descriptor stab.  */

#define USE_REGISTER_NOT_ARG

/* For acc, there's no need to correct LBRAC entries by guessing how
   they should work.  In fact, this is harmful because the LBRAC
   entries now all appear at the end of the function, not intermixed
   with the SLINE entries.  n_opt_found detects acc for Solaris binaries;
   function_stab_type detects acc for SunOS4 binaries.

   For binary from SunOS4 /bin/cc, need to correct LBRAC's.

   For gcc, like acc, don't correct.  */

#define	SUN_FIXED_LBRAC_BUG \
  (n_opt_found \
   || function_stab_type == N_STSYM \
   || function_stab_type == N_GSYM \
   || processing_gcc_compilation)

/* Do variables in the debug stabs occur after the N_LBRAC or before it?
   acc: after, gcc: before, SunOS4 /bin/cc: before.  */

#define VARIABLES_INSIDE_BLOCK(desc, gcc_p) \
  (!(gcc_p) \
   && (n_opt_found \
       || function_stab_type == N_STSYM \
       || function_stab_type == N_GSYM))

/* Sequence of bytes for breakpoint instruction (ta 1). */

extern const unsigned char *sparc_breakpoint_from_pc (CORE_ADDR *pc, int *len);
#define BREAKPOINT_FROM_PC(PC,LEN) sparc_breakpoint_from_pc ((PC), (LEN))

/* Register numbers of various important registers.
   Note that some of these values are "real" register numbers,
   and correspond to the general registers of the machine,
   and some are "phony" register numbers which are too large
   to be actual register numbers as far as the user is concerned
   but do serve to get the desired values when passed to read_register.  */

#define	G0_REGNUM 0		/* %g0 */
#define	G1_REGNUM 1		/* %g1 */
#define O0_REGNUM 8		/* %o0 */
#define	RP_REGNUM 15		/* Contains return address value, *before* \
				   any windows get switched.  */
#define	O7_REGNUM 15		/* Last local reg not saved on stack frame */
#define	L0_REGNUM 16		/* First local reg that's saved on stack frame
				   rather than in machine registers */
#define	I0_REGNUM 24		/* %i0 */
#define	I7_REGNUM 31		/* Last local reg saved on stack frame */
#define	PS_REGNUM 65		/* Contains processor status */
#define PS_FLAG_CARRY 0x100000	/* Carry bit in PS */
#define	WIM_REGNUM 66		/* Window Invalid Mask (not really supported) */
#define	TBR_REGNUM 67		/* Trap Base Register (not really supported) */
#define	FPS_REGNUM 70		/* Floating point status register */
#define	CPS_REGNUM 71		/* Coprocessor status register */

/* Writing to %g0 is a noop (not an error or exception or anything like
   that, however).  */

#define CANNOT_STORE_REGISTER(regno) ((regno) == G0_REGNUM)

#define	PRINT_EXTRA_FRAME_INFO(FI) \
     sparc_print_extra_frame_info (FI)
extern void sparc_print_extra_frame_info (struct frame_info *);

/* DEPRECATED_INIT_EXTRA_FRAME_INFO needs the PC to detect flat
   frames.  */

#define DEPRECATED_INIT_FRAME_PC_FIRST(FROMLEAF, PREV) \
  ((FROMLEAF) ? DEPRECATED_SAVED_PC_AFTER_CALL ((PREV)->next) : \
	      (PREV)->next ? DEPRECATED_FRAME_SAVED_PC ((PREV)->next) : read_pc ())

/* Define other aspects of the stack frame.  */

/* The location of I0 w.r.t SP.  This is actually dependent on how the
   system's window overflow/underflow routines are written.  Most
   vendors save the L regs followed by the I regs (at the higher
   address).  Some vendors get it wrong.  */

#define	FRAME_SAVED_L0	0
#define	FRAME_SAVED_I0	(8 * DEPRECATED_REGISTER_RAW_SIZE (L0_REGNUM))

#define FRAME_STRUCT_ARGS_ADDRESS(FI) (get_frame_base (FI))

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
 * executed (its function is superceeded by
 * DEPRECATED_PUSH_DUMMY_FRAME; they are designed to do the same
 * thing).
 *
 *   DEPRECATED_PUSH_DUMMY_FRAME saves the registers above sp' and
 * pushes the * register file stack down one.
 *
 *   call_function then writes CALL_DUMMY, pushes the args onto the
 * stack, and adjusts the stack pointer.
 
   call_function_by_hand then starts execution (in the middle of
   CALL_DUMMY, as directed by call_function).  */

#ifndef CALL_DUMMY
/* This sequence of words is the instructions

   00:   bc 10 00 01     mov  %g1, %fp
   04:   9d e3 80 00     save  %sp, %g0, %sp
   08:   bc 10 00 02     mov  %g2, %fp
   0c:   be 10 00 03     mov  %g3, %i7
   10:   da 03 a0 58     ld  [ %sp + 0x58 ], %o5
   14:   d8 03 a0 54     ld  [ %sp + 0x54 ], %o4
   18:   d6 03 a0 50     ld  [ %sp + 0x50 ], %o3
   1c:   d4 03 a0 4c     ld  [ %sp + 0x4c ], %o2
   20:   d2 03 a0 48     ld  [ %sp + 0x48 ], %o1
   24:   40 00 00 00     call  <fun>
   28:   d0 03 a0 44     ld  [ %sp + 0x44 ], %o0
   2c:   01 00 00 00     nop 
   30:   91 d0 20 01     ta  1
   34:   01 00 00 00     nop

   NOTES:
   * the first four instructions are necessary only on the simulator.
   * this is a multiple of 8 (not only 4) bytes.
   * the `call' insn is a relative, not an absolute call.
   * the `nop' at the end is needed to keep the trap from
     clobbering things (if NPC pointed to garbage instead).
 */

#endif /* CALL_DUMMY */

/* Sparc has no reliable single step ptrace call */

#define SOFTWARE_SINGLE_STEP_P() 1
extern void sparc_software_single_step (enum target_signal, int);
#define SOFTWARE_SINGLE_STEP(sig,bp_p) sparc_software_single_step (sig,bp_p)

/* We need more arguments in a frame specification for the
   "frame" or "info frame" command.  */

#define SETUP_ARBITRARY_FRAME(argc, argv) setup_arbitrary_frame (argc, argv)
extern struct frame_info *setup_arbitrary_frame (int, CORE_ADDR *);

extern void sparc_do_registers_info (int regnum, int all);
#undef DEPRECATED_DO_REGISTERS_INFO
#define DEPRECATED_DO_REGISTERS_INFO(REGNUM,ALL) sparc_do_registers_info (REGNUM, ALL)

/* Optimization for storing registers to the inferior.  The hook
   DO_DEFERRED_STORES
   actually executes any deferred stores.  It is called any time
   we are going to proceed the child, or read its registers.
   The hook CLEAR_DEFERRED_STORES is called when we want to throw
   away the inferior process, e.g. when it dies or we kill it.
   FIXME, this does not handle remote debugging cleanly.  */

extern int deferred_stores;
#define	DO_DEFERRED_STORES	\
  if (deferred_stores)		\
    target_store_registers (-2);
#define	CLEAR_DEFERRED_STORES	\
  deferred_stores = 0;

/* Select the sparc disassembler */

#define TM_PRINT_INSN_MACH bfd_mach_sparc

#define DEPRECATED_EXTRA_STACK_ALIGNMENT_NEEDED 1
