/* Parameters for execution on any Hewlett-Packard PA-RISC machine.

   Copyright 1986, 1987, 1989, 1990, 1991, 1992, 1993, 1994, 1995,
   1996, 1998, 1999, 2000, 2001, 2002, 2003, 2004 Free Software
   Foundation, Inc.

   Contributed by the Center for Software Science at the
   University of Utah (pa-gdb-bugs@cs.utah.edu).

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

#define GDB_MULTI_ARCH 1

extern int hppa_pc_requires_run_before_use (CORE_ADDR pc);
#define PC_REQUIRES_RUN_BEFORE_USE(pc) hppa_pc_requires_run_before_use (pc)

/* Register numbers of various important registers.
   Note that some of these values are "real" register numbers,
   and correspond to the general registers of the machine,
   and some are "phony" register numbers which are too large
   to be actual register numbers as far as the user is concerned
   but do serve to get the desired values when passed to read_register.  */

#define R0_REGNUM 0		/* Doesn't actually exist, used as base for
				   other r registers.  */
#define FLAGS_REGNUM 0		/* Various status flags */
#define RP_REGNUM 2		/* return pointer */
#define HPPA_FP_REGNUM 3	/* The ABI's frame pointer, when used */
#define HPPA_SP_REGNUM 30	/* Stack pointer.  */
#define SAR_REGNUM 32		/* Shift Amount Register */
#define IPSW_REGNUM 41		/* Interrupt Processor Status Word */
#define PCOQ_HEAD_REGNUM 33	/* instruction offset queue head */
#define PCSQ_HEAD_REGNUM 34	/* instruction space queue head */
#define PCOQ_TAIL_REGNUM 35	/* instruction offset queue tail */
#define PCSQ_TAIL_REGNUM 36	/* instruction space queue tail */
#define EIEM_REGNUM 37		/* External Interrupt Enable Mask */
#define IIR_REGNUM 38		/* Interrupt Instruction Register */
#define IOR_REGNUM 40		/* Interrupt Offset Register */
#define SR4_REGNUM 43		/* space register 4 */
#define RCR_REGNUM 51		/* Recover Counter (also known as cr0) */
#define CCR_REGNUM 54		/* Coprocessor Configuration Register */
#define TR0_REGNUM 57		/* Temporary Registers (cr24 -> cr31) */
#define CR27_REGNUM 60		/* Base register for thread-local storage, cr27 */
#define HPPA_FP0_REGNUM 64	/* First floating-point.  */
#define FP4_REGNUM 72

#define ARG0_REGNUM 26		/* The first argument of a callee. */
#define ARG1_REGNUM 25		/* The second argument of a callee. */
#define ARG2_REGNUM 24		/* The third argument of a callee. */
#define ARG3_REGNUM 23		/* The fourth argument of a callee. */

/* When fetching register values from an inferior or a core file,
   clean them up using this macro.  BUF is a char pointer to
   the raw value of the register in the registers[] array.  */

#define	DEPRECATED_CLEAN_UP_REGISTER_VALUE(regno, buf) \
  do {	\
    if ((regno) == PCOQ_HEAD_REGNUM || (regno) == PCOQ_TAIL_REGNUM) \
      (buf)[sizeof(CORE_ADDR) -1] &= ~0x3; \
  } while (0)

/* PA specific macro to see if the current instruction is nullified. */
#ifndef INSTRUCTION_NULLIFIED
extern int hppa_instruction_nullified (void);
#define INSTRUCTION_NULLIFIED hppa_instruction_nullified ()
#endif

/* Here's how to step off a permanent breakpoint.  */
#define SKIP_PERMANENT_BREAKPOINT (hppa_skip_permanent_breakpoint)
extern void hppa_skip_permanent_breakpoint (void);

/* On HP-UX, certain system routines (millicode) have names beginning
   with $ or $$, e.g. $$dyncall, which handles inter-space procedure
   calls on PA-RISC.  Tell the expression parser to check for those
   when parsing tokens that begin with "$".  */
#define SYMBOLS_CAN_START_WITH_DOLLAR (1)
