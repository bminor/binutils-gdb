/* Parameters for execution on an HP PA-RISC machine, running HPUX, for GDB.
   Copyright 1991, 1992 Free Software Foundation, Inc. 

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
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */


/* Actually, for a PA running HPUX the kernel calls the signal handler
   without an intermediate trampoline.  Luckily the kernel always sets
   the return pointer for the signal handler to point to _sigreturn.  */
#define IN_SIGTRAMP(pc, name) (name && STREQ ("_sigreturn", name))

/* For HPUX:

   The signal context structure pointer is always saved at the base
   of the frame which "calls" the signal handler.  We only want to find
   the hardware save state structure, which lives 10 32bit words into
   sigcontext structure.

   Within the hardware save state structure, registers are found in the
   same order as the register numbers in GDB.

   The kernel apparently sets %r31 in the saved state structure to point
   to the active instruction when the signal was taken.  Everything
   else looks fairly reasonable.  (I assume the kernel fixes %r31 from
   within _sigreturn?.  */

#define FRAME_SAVED_PC_IN_SIGTRAMP(FRAME, TMP) \
{ \
  *(TMP) = read_memory_integer ((FRAME)->frame + (41 * 4) , 4); \
}

#define FRAME_BASE_BEFORE_SIGTRAMP(FRAME, TMP) \
{ \
  *(TMP) = read_memory_integer ((FRAME)->frame + (40 * 4), 4); \
}

#define FRAME_FIND_SAVED_REGS_IN_SIGTRAMP(FRAME, FSR) \
{ \
  int i; \
  CORE_ADDR TMP; \
  TMP = (FRAME)->frame + (10 * 4); \
  for (i = 0; i < NUM_REGS; i++) \
    { \
      if (i == SP_REGNUM) \
	(FSR)->regs[SP_REGNUM] = read_memory_integer (TMP + SP_REGNUM * 4, 4); \
      else \
	(FSR)->regs[i] = TMP + i * 4; \
    } \
}

/* We need to figure out where the text region is so that we use the
   appropriate ptrace operator to manipulate text.  Simply reading/writing
   user space will crap out HPUX.  */

#define NEED_TEXT_START_END

/* Mostly it's common to all HPPA's.  */
#include "pa/tm-hppa.h"
