/* Target machine description for MIPS running SVR4, for GDB.
   Copyright 1994 Free Software Foundation, Inc.

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

#include "mips/tm-bigmips.h"
#include "tm-sysv4.h"

/* When calling functions on a MIPS SVR4 ABI compliant platform
   $25 must hold the function address.  Dest_Reg is a macro
   used in CALL_DUMMY in tm-mips.h.  */
#undef Dest_Reg
#define Dest_Reg 25

/* The signal handler trampoline is called _sigtramp.  */
#undef IN_SIGTRAMP
#define IN_SIGTRAMP(pc, name) ((name) && STREQ ("_sigtramp", name))
 
/* On entry to the signal handler trampoline, an ucontext is already
   pushed on the stack. We can get at the saved registers via the
   mcontext which is contained within the ucontext.  */
#define SIGFRAME_BASE	0
#define SIGFRAME_REGSAVE_OFF	(SIGFRAME_BASE + 40)
#define SIGFRAME_PC_OFF		(SIGFRAME_BASE + 40 + 35 * 4)

/* Use the alternate method of determining valid frame chains. */
#define FRAME_CHAIN_VALID_ALTERNATE
