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

/* IEEE format floating point.  */
#define TARGET_DOUBLE_FORMAT  (TARGET_BYTE_ORDER == BFD_ENDIAN_BIG \
			       ? &floatformat_ieee_double_big	 \
			       : &floatformat_ieee_double_littlebyte_bigword)

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

/* The system C compiler uses a similar structure return convention to gcc */
extern use_struct_convention_fn arm_use_struct_convention;
#define USE_STRUCT_CONVENTION(gcc_p, type) \
     arm_use_struct_convention (gcc_p, type)

/* Extract from an array REGBUF containing the (raw) register state
   the address in which a function should return its structure value,
   as a CORE_ADDR (or an expression that can be used as one).  */

#define EXTRACT_STRUCT_VALUE_ADDRESS(REGBUF) \
  (extract_address ((PTR)(REGBUF), REGISTER_RAW_SIZE(0)))

/* Specify that for the native compiler variables for a particular
   lexical context are listed after the beginning LBRAC instead of
   before in the executables list of symbols.  */
#define VARIABLES_INSIDE_BLOCK(desc, gcc_p) (!(gcc_p))

/* XXX This is NOT multi-arch compatible.  */
#define CALL_DUMMY_BREAKPOINT_OFFSET arm_call_dummy_breakpoint_offset()
extern int arm_call_dummy_breakpoint_offset (void);

/* The first 0x20 bytes are the trap vectors.  */
#define LOWEST_PC	0x20

#endif /* TM_ARM_H */
