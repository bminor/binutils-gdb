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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include "i386/tm-i386v.h"

/* Number of machine registers. */

#undef NUM_FREGS
#define NUM_FREGS 15
#undef NUM_REGS
#define NUM_REGS (16+NUM_FREGS)

/* Initializer for an array of names of registers.  There should be
   NUM_REGS strings in this initializer.  */

/* The order of the first 8 registers must match the compiler's
   numbering scheme (which is the same as the 386 scheme).  */

#undef REGISTER_NAMES
#define REGISTER_NAMES { "eax",  "ecx",   "edx",  "ebx",  \
			 "esp",  "ebp",   "esi",  "edi",  \
			 "eip",  "eflags","cs",   "ss",   \
			 "ds",   "es",    "fs",   "gs",   \
			 "st0",  "st1",   "st2",  "st3",  \
                         "st4",  "st5",   "st6",  "st7",  \
			 "fctrl","fstat", "ftag", "fcs",  \
			 "fopsel","fip",  "fopoff" }

#undef FP_REGNUM
#define FP_REGNUM  5		/* (ebp) Contains addr of stack frame */
#undef  SP_REGNUM
#define SP_REGNUM  4		/* (usp) Contains address of top of stack */
#undef  PS_REGNUM
#define PS_REGNUM  9		/* (ps)  Contains processor status */
#undef  PC_REGNUM
#define PC_REGNUM  8		/* (eip) Contains program counter */
#undef  FP0_REGNUM
#define FP0_REGNUM 16		/* Floating point register 0 */
#undef  FPC_REGNUM
#define FPC_REGNUM 24		/* 80387 control register */
#undef  FPCWD_REGNUM
#define FPCWD_REGNUM FPC_REGNUM
#undef  FPSWD_REGNUM
#define FPSWD_REGNUM 25		/* 80387 status register */
#undef  FPTWD_REGNUM
#define FPTWD_REGNUM 26		/* 80387 tag register */
#undef  FPIPO_REGNUM
#define FPIPO_REGNUM 29		/* 80387 instruction pointer offset reg */
#undef  FPIPS_REGNUM
#define FPIPS_REGNUM 27		/* 80387 instruction pointer selector reg */
#undef  FPOOS_REGNUM
#define FPOOS_REGNUM 30		/* 80387 operand pointer offset reg */
#undef  FPOPS_REGNUM
#define FPOPS_REGNUM 28		/* 80387 operand pointer selector reg */

/* Total amount of space needed to store our copies of the machine's
   register state, the array `registers'.  */

#undef REGISTER_BYTES
#define REGISTER_BYTES (10*4 + 6*2 + 8*10 + 5*2 + 2*4)

/* Index within `registers' of the first byte of the space for
   register N.  */

#undef REGISTER_BYTE
#define REGBYTE_0  0
#define REGBYTE_10 (REGBYTE_0+10*4)
#define REGBYTE_16 (REGBYTE_10+6*2)
#define REGBYTE_24 (REGBYTE_16+8*10)
#define REGBYTE_29 (REGBYTE_24+5*2)
#define REGISTER_BYTE(N) (((N) < 10) ? (N) * 4 : \
                          (N) < 16 ? REGBYTE_10 +((N) - 10) * 2 : \
                          (N) < 24 ? REGBYTE_16 +((N) - 16) * 10 : \
                          (N) < 29 ? REGBYTE_24 +((N) - 24) * 2 : \
                          REGBYTE_29 + ((N) - 29) * 4)

/* Number of bytes of storage in the actual machine representation
   for register N.  */

#undef REGISTER_RAW_SIZE
#define REGISTER_RAW_SIZE(N) ((N) < 10 ? 4 : (N) < 16 ? 2 : (N) < 24 ? 10 : \
                              (N) < 29 ? 2 : 4)

/* Number of bytes of storage in the program's representation
   for register N. */

#undef REGISTER_VIRTUAL_SIZE
#define REGISTER_VIRTUAL_SIZE(N) REGISTER_RAW_SIZE(N)

/* Largest value REGISTER_RAW_SIZE can have.  */

#undef MAX_REGISTER_RAW_SIZE
#define MAX_REGISTER_RAW_SIZE 10

/* Largest value REGISTER_VIRTUAL_SIZE can have.  */

#undef MAX_REGISTER_VIRTUAL_SIZE
#define MAX_REGISTER_VIRTUAL_SIZE 10

/* Nonzero if register N requires conversion
   from raw format to virtual format.  */

#undef REGISTER_CONVERTIBLE
#define REGISTER_CONVERTIBLE(N) ((N) < FP0_REGNUM ? 0 :\
                                 (N) < FPC_REGNUM ? 1 : 0)

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

#undef REGISTER_CONVERT_TO_VIRTUAL
#ifdef LD_I387
#define REGISTER_CONVERT_TO_VIRTUAL(REGNUM,TYPE,FROM,TO) \
{ \
  if (TYPE == REGISTER_VIRTUAL_TYPE (REGNUM)) \
    { \
      memcpy (TO, FROM, TYPE_LENGTH (TYPE)); \
    } \
  else \
    { \
      long double val = *((long double *)FROM); \
      store_floating ((TO), TYPE_LENGTH (TYPE), val); \
    } \
}
#else
/* Convert data from raw format for register REGNUM in buffer FROM to
   virtual format with type TYPE in buffer TO.  */
#define REGISTER_CONVERT_TO_VIRTUAL(REGNUM,TYPE,FROM,TO) \
{ \
  double val; \
  i387_to_double ((FROM), (char *)&val); \
  store_floating ((TO), TYPE_LENGTH (TYPE), val); \
}
#endif

extern void i387_to_double PARAMS ((char *, char *));

#undef REGISTER_CONVERT_TO_RAW
#ifdef LD_I387
#define REGISTER_CONVERT_TO_RAW(TYPE,REGNUM,FROM,TO) \
{ \
  if (TYPE == REGISTER_VIRTUAL_TYPE (REGNUM)) \
    { \
      memcpy (TO, FROM, TYPE_LENGTH (TYPE)); \
    } \
  else \
    { \
      long double val = extract_floating ((FROM), TYPE_LENGTH (TYPE)); \
      *((long double *)TO) = val; \
    } \
}
#else
#define REGISTER_CONVERT_TO_RAW(TYPE,REGNUM,FROM,TO) \
{ \
  double val = extract_floating ((FROM), TYPE_LENGTH (TYPE)); \
  double_to_i387((char *)&val, (TO)); \
}
#endif

extern void double_to_i387 PARAMS ((char *, char *));

/* Return the GDB type object for the "standard" data type of data in
   register N.  */

#undef REGISTER_VIRTUAL_TYPE
#ifdef LD_I387
#define REGISTER_VIRTUAL_TYPE(N) \
  ((N < FP0_REGNUM) ? builtin_type_int : \
   (N < FPC_REGNUM) ? builtin_type_long_double : builtin_type_int)
#else
#define REGISTER_VIRTUAL_TYPE(N) \
  ((N < FP0_REGNUM) ? builtin_type_int : \
   (N < FPC_REGNUM) ? builtin_type_double : builtin_type_int)
#endif

#undef TARGET_LONG_DOUBLE_BIT
#define TARGET_LONG_DOUBLE_BIT 96
