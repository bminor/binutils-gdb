/* Target-dependent definitions for Intel x86 running DJGPP.
   Copyright 1995, 1996, 1997, 1999, 2000 Free Software Foundation, Inc.

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

#ifndef TM_GO32_H
#define TM_GO32_H

#define I386_DJGPP_TARGET
#undef HAVE_SSE_REGS	/* FIXME! go32-nat.c needs to support XMMi registers */
#define HAVE_I387_REGS

#include "i386/tm-i386.h"

/* The host and target are i386 machines and the compiler supports
   long doubles. Long doubles on the host therefore have the same
   layout as a 387 FPU stack register. */

#if defined(HAVE_LONG_DOUBLE) && defined(HOST_I386)
#undef LD_I387
#define LD_I387
#endif

/* Allow floating point numbers to be specified by a raw long double
   10 hex bytes number, e.g. 1.0 can be input as
   0x3fff8000000000000000 */

#ifdef LD_I387
#define HEX_LONG_DOUBLE_INPUT(base,p,len,target) \
  ((base) == 16 && (len) == 20 \
   && i387_hex_long_double_input ((p), (target)))
#endif

extern int i387_hex_long_double_input (char *p, long double *putithere);

#ifdef LD_I387	/* otherwise, definitions from tm-i386.h are good enough */

#undef REGISTER_CONVERT_TO_VIRTUAL
#define REGISTER_CONVERT_TO_VIRTUAL(REGNUM,TYPE,FROM,TO)	\
{								\
  long double val = *((long double *)(FROM));			\
  store_floating ((TO), TYPE_LENGTH (TYPE), val);		\
}

#undef REGISTER_CONVERT_TO_RAW
#define REGISTER_CONVERT_TO_RAW(TYPE,REGNUM,FROM,TO)			\
{									\
  long double val = extract_floating ((FROM), TYPE_LENGTH (TYPE));	\
  *((long double *)(TO)) = val;						\
}

#undef TARGET_LONG_DOUBLE_BIT
#define TARGET_LONG_DOUBLE_BIT 96

/* FRAME_CHAIN takes a frame's nominal address and produces the frame's
   chain-pointer.
   In the case of the i386, the frame's nominal address
   is the address of a 4-byte word containing the calling frame's address.
   DJGPP doesn't have any special frames for signal handlers, they are
   just normal C functions. */
#undef  FRAME_CHAIN
#define FRAME_CHAIN(thisframe) \
  (!inside_entry_file ((thisframe)->pc) ? \
   read_memory_integer ((thisframe)->frame, 4) :\
   0)

/* A macro that tells us whether the function invocation represented
   by FI does not have a frame on the stack associated with it.  If it
   does not, FRAMELESS is set to 1, else 0.  */
#undef  FRAMELESS_FUNCTION_INVOCATION
#define FRAMELESS_FUNCTION_INVOCATION(FI) \
     (frameless_look_for_prologue(FI))

#undef  FRAME_SAVED_PC
#define FRAME_SAVED_PC(FRAME) (read_memory_integer ((FRAME)->frame + 4, 4))

#endif /* LD_I387 */
#endif /* TM_GO32_H */
