/* Target-dependent definitions for Intel x86 running DJGPP.
   Copyright 1995, 1996, 1997 Free Software Foundation, Inc.

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

/* Return the GDB type object for the "standard" data type of data in 
   register N.  Perhaps si and di should go here, but potentially they
   could be used for things other than address.  */

#define REGISTER_VIRTUAL_TYPE(N)				\
  (((N) == PC_REGNUM || (N) == FP_REGNUM || (N) == SP_REGNUM)	\
   ? lookup_pointer_type (builtin_type_void)			\
   : IS_FP_REGNUM(N) ? builtin_type_long_double 		\
   : IS_SSE_REGNUM(N) ? builtin_type_v4sf			\
   : builtin_type_int)

#endif /* LD_I387 */

#undef TARGET_LONG_DOUBLE_BIT
#define TARGET_LONG_DOUBLE_BIT 96
