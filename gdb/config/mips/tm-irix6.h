/* Target machine description for SGI Iris under Irix 6.x, for GDB.

   Copyright 2001, 2002 Free Software Foundation, Inc.

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

#include "mips/tm-mips64.h"
#include "solib.h"

/* Redefine register numbers for SGI. */

#undef MIPS_REGISTER_NAMES
#undef FP0_REGNUM
#undef PC_REGNUM
#undef HI_REGNUM
#undef LO_REGNUM
#undef CAUSE_REGNUM
#undef BADVADDR_REGNUM
#undef FCRCS_REGNUM
#undef FCRIR_REGNUM

/* Initializer for an array of names for registers 32 and above.
   There should be NUM_REGS-32 strings in this initializer.  */

#define MIPS_REGISTER_NAMES 	\
    {	"f0",   "f1",   "f2",   "f3",   "f4",   "f5",   "f6",   "f7", \
	"f8",   "f9",   "f10",  "f11",  "f12",  "f13",  "f14",  "f15", \
	"f16",  "f17",  "f18",  "f19",  "f20",  "f21",  "f22",  "f23",\
	"f24",  "f25",  "f26",  "f27",  "f28",  "f29",  "f30",  "f31",\
	"pc",	"cause", "bad",	"hi",	"lo",	"fsr",  "fir" \
    }

/* Register numbers of various important registers.
   Note that some of these values are "real" register numbers,
   and correspond to the general registers of the machine,
   and some are "phony" register numbers which are too large
   to be actual register numbers as far as the user is concerned
   but do serve to get the desired values when passed to read_register.  */

#define FP0_REGNUM 32		/* Floating point register 0 (single float) */
#define PC_REGNUM 64		/* Contains program counter */
#define CAUSE_REGNUM 65		/* describes last exception */
#define BADVADDR_REGNUM 66	/* bad vaddr for addressing exception */
#define HI_REGNUM 67		/* Multiple/divide temp */
#define LO_REGNUM 68		/* ... */
#define FCRCS_REGNUM 69		/* FP control/status */
#define FCRIR_REGNUM 70		/* FP implementation/revision */


#undef  MIPS_REGISTER_BYTE
#define MIPS_REGISTER_BYTE(N) \
     (((N) < FP0_REGNUM) ? (N) * MIPS_REGSIZE : \
      ((N) < FP0_REGNUM + 32) ?     \
      FP0_REGNUM * MIPS_REGSIZE + \
      ((N) - FP0_REGNUM) * sizeof(double) : \
      32 * sizeof(double) + ((N) - 32) * MIPS_REGSIZE)

/* The signal handler trampoline is called _sigtramp.  */
#undef IN_SIGTRAMP
#define IN_SIGTRAMP(pc, name) ((name) && STREQ ("_sigtramp", name))

/* Offsets for register values in _sigtramp frame.
   sigcontext is immediately above the _sigtramp frame on Irix.  */
#undef SIGFRAME_BASE
#define SIGFRAME_BASE		0

/* Irix 5 saves a full 64 bits for each register.  We skip 2 * 4 to
   get to the saved PC (the register mask and status register are both
   32 bits) and then another 4 to get to the lower 32 bits.  We skip
   the same 4 bytes, plus the 8 bytes for the PC to get to the
   registers, and add another 4 to get to the lower 32 bits.  We skip
   8 bytes per register.  */
#undef SIGFRAME_PC_OFF
#define SIGFRAME_PC_OFF		(SIGFRAME_BASE + 2 * 4 + 4)
#undef SIGFRAME_REGSAVE_OFF
#define SIGFRAME_REGSAVE_OFF	(SIGFRAME_BASE + 2 * 4 + 8 + 4)
#undef SIGFRAME_FPREGSAVE_OFF
#define SIGFRAME_FPREGSAVE_OFF	(SIGFRAME_BASE + 2 * 4 + 8 + 32 * 8 + 4)
#define SIGFRAME_REG_SIZE	8

/* Undefine those methods which have been multiarched.  */

/* Undefine MIPS_REGISTER_TYPE, so that GDB uses real C code in
   mips_register_type() to return the register type, instead of
   relying on this macro.  */
#undef MIPS_REGISTER_TYPE
