/* Macro definitions for GDB on an Intel i386 running SVR4.
   Copyright (C) 1991, 1994 Free Software Foundation, Inc.
   Written by Fred Fish at Cygnus Support (fnf@cygint)

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

/* Use the alternate method of determining valid frame chains. */

#define FRAME_CHAIN_VALID_ALTERNATE

/* number of traps that happen between exec'ing the shell 
 * to run an inferior, and when we finally get to 
 * the inferior code.  This is 2 on most implementations.
 */
#define START_INFERIOR_TRAPS_EXPECTED 2

/* Pick up most of what we need from the generic i386 target include file. */

#include "i386/tm-i386v.h"

/* Pick up more stuff from the generic SVR4 host include file. */

#include "tm-sysv4.h"

/* We can't tell how many args there are
   now that the C compiler delays popping them.  */

#undef FRAME_NUM_ARGS
#define FRAME_NUM_ARGS(val,fi) (val = -1)

/* Offsets (in target ints) into jmp_buf.  Not defined in any system header
   file, so we have to step through setjmp/longjmp with a debugger and figure
   them out.  Note that <setjmp> defines _JBLEN as 10, which is the default
   if no specific machine is selected, even though we only use 6 slots. */

#define JB_ELEMENT_SIZE sizeof(int)	/* jmp_buf[_JBLEN] is array of ints */

#define JB_EBX	0
#define JB_ESI	1
#define JB_EDI	2
#define JB_EBP	3
#define JB_ESP	4
#define JB_EDX	5

#define JB_PC	JB_EDX	/* Setjmp()'s return PC saved in EDX */

/* Figure out where the longjmp will land.  Slurp the args out of the stack.
   We expect the first arg to be a pointer to the jmp_buf structure from which
   we extract the pc (JB_PC) that we will land at.  The pc is copied into ADDR.
   This routine returns true on success */

extern int
get_longjmp_target PARAMS ((CORE_ADDR *));

#define GET_LONGJMP_TARGET(ADDR) get_longjmp_target(ADDR)

/* The following redefines make backtracing through sigtramp work.
   They manufacture a fake sigtramp frame and obtain the saved pc in sigtramp
   from the ucontext structure which is pushed by the kernel on the
   user stack. Unfortunately there are three variants of sigtramp handlers.  */

#define I386V4_SIGTRAMP_SAVED_PC
#define IN_SIGTRAMP(pc, name) ((name)					\
			       && (STREQ ("_sigreturn", name)		\
				   || STREQ ("_sigacthandler", name)	\
				   || STREQ ("sigvechandler", name)))

/* FRAME_CHAIN takes a frame's nominal address and produces the frame's
   chain-pointer.
   In the case of the i386, the frame's nominal address
   is the address of a 4-byte word containing the calling frame's address.  */
#undef FRAME_CHAIN
#define FRAME_CHAIN(thisframe)  \
  ((thisframe)->signal_handler_caller \
   ? (thisframe)->frame \
   : (!inside_entry_file ((thisframe)->pc) \
      ? read_memory_integer ((thisframe)->frame, 4) \
      : 0))

/* A macro that tells us whether the function invocation represented
   by FI does not have a frame on the stack associated with it.  If it
   does not, FRAMELESS is set to 1, else 0.  */
#undef FRAMELESS_FUNCTION_INVOCATION
#define FRAMELESS_FUNCTION_INVOCATION(FI, FRAMELESS) \
  do { \
    if ((FI)->signal_handler_caller) \
      (FRAMELESS) = 0; \
    else \
      (FRAMELESS) = frameless_look_for_prologue(FI); \
  } while (0)

/* Saved Pc.  Get it from ucontext if within sigtramp.  */

#undef FRAME_SAVED_PC
#define FRAME_SAVED_PC(FRAME) \
  (((FRAME)->signal_handler_caller \
    ? i386v4_sigtramp_saved_pc (FRAME) \
    : read_memory_integer ((FRAME)->frame + 4, 4)) \
   )
extern CORE_ADDR i386v4_sigtramp_saved_pc PARAMS ((struct frame_info *));
