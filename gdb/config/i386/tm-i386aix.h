/* Macro defintions for IBM AIX PS/2 (i386).
   Copyright 1986, 1987, 1989, 1992, 1993 Free Software Foundation, Inc.

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

/* Changes for IBM AIX PS/2 by Minh Tran-Le (tranle@intellicorp.com).  */

#ifndef TM_I386AIX_H
#define TM_I386AIX_H 1

#include "i386/tm-i386v.h"
#include <sys/reg.h>

#ifndef I386
# define I386 1
#endif
#ifndef I386_AIX_TARGET
# define I386_AIX_TARGET 1
#endif

/* number of traps that happen between exec'ing the shell 
 * to run an inferior, and when we finally get to 
 * the inferior code.  This is 2 on most implementations.
 */
#undef  START_INFERIOR_TRAPS_EXPECTED
#define START_INFERIOR_TRAPS_EXPECTED 2

/* Number of machine registers */
#undef  NUM_REGS
#define NUM_REGS 24 /* 16+8 */

/* Initializer for an array of names of registers.
   There should be NUM_REGS strings in this initializer.  */

/* the order of the first 8 registers must match the compiler's 
 * numbering scheme (which is the same as the 386 scheme)
 * also, this table must match regmap in i386-pinsn.c.
 */
#undef  REGISTER_NAMES
#define REGISTER_NAMES { "eax", "ecx", "edx", "ebx", \
			 "esp", "ebp", "esi", "edi", \
			 "eip", "eflags", "cs", "ss", \
			 "ds", "es", "fs", "gs", \
			 "st0", "st1", "st2", "st3", \
			 "st4", "st5", "st6", "st7", \
			 }

/* Register numbers of various important registers.
   Note that some of these values are "real" register numbers,
   and correspond to the general registers of the machine,
   and some are "phony" register numbers which are too large
   to be actual register numbers as far as the user is concerned
   but do serve to get the desired values when passed to read_register.  */

#undef  FP_REGNUM
#define FP_REGNUM 5	/* (ebp) Contains address of executing stack frame */
#undef  SP_REGNUM
#define SP_REGNUM 4	/* (usp) Contains address of top of stack */

#undef  PC_REGNUM
#define PC_REGNUM 8	/* (eip) Contains program counter */
#undef  PS_REGNUM 
#define PS_REGNUM 9	/* (eflags) Contains processor status */

#define FP0_REGNUM 16   /*  (st0) 387 register */

/* Total amount of space needed to store our copies of the machine's
   register state, the array `registers'.  */
/* 16 i386 registers and 8 i387 registers */
#undef REGISTER_BYTES
#define REGISTER_BYTES (16*4 + 8*10)

/* Index within `registers' of the first byte of the space for
   register N.  */
#undef REGISTER_BYTE
#define REGISTER_BYTE(N) \
  ((N < FP0_REGNUM) ? (N * 4) : \
   (((N - FP0_REGNUM) * 10) + 64))
 
/* Number of bytes of storage in the actual machine representation
 * for register N.  All registers are 4 bytes, except 387 st(0) - st(7),
 * which are 80 bits each. 
 */
#undef REGISTER_RAW_SIZE
#define REGISTER_RAW_SIZE(N) \
  ((N < FP0_REGNUM) ? 4 : 10)

/* Largest value REGISTER_RAW_SIZE can have.  */
#undef MAX_REGISTER_RAW_SIZE
#define MAX_REGISTER_RAW_SIZE 10

/* Nonzero if register N requires conversion
   from raw format to virtual format.  */
#undef REGISTER_CONVERTIBLE
#define REGISTER_CONVERTIBLE(N) \
  ((N < FP0_REGNUM) ? 0 : 1)

/* Convert data from raw format for register REGNUM in buffer FROM
   to virtual format with type TYPE in buffer TO.  */

#undef REGISTER_CONVERT_TO_VIRTUAL
#define REGISTER_CONVERT_TO_VIRTUAL(REGNUM,TYPE,FROM,TO) \
{ \
  double val; \
  i387_to_double ((FROM), (char *)&val); \
  store_floating ((TO), TYPE_LENGTH (TYPE), val); \
}
extern void
i387_to_double PARAMS ((char *, char *));

/* Convert data from virtual format with type TYPE in buffer FROM
   to raw format for register REGNUM in buffer TO.  */

#undef REGISTER_CONVERT_TO_RAW
#define REGISTER_CONVERT_TO_RAW(TYPE,REGNUM,FROM,TO) \
{ \
  double val = extract_floating ((FROM), TYPE_LENGTH (TYPE)); \
  double_to_i387((char *)&val, (TO)); \
}
extern void
double_to_i387 PARAMS ((char *, char *));

/* Return the GDB type object for the "standard" data type
   of data in register N.  */
#undef REGISTER_VIRTUAL_TYPE
#define REGISTER_VIRTUAL_TYPE(N) \
  ((N < FP0_REGNUM) ? builtin_type_int : \
   builtin_type_double)

/* Extract from an array REGBUF containing the (raw) register state
   a function return value of type TYPE, and copy that, in virtual format,
   into VALBUF.  */
#undef  EXTRACT_RETURN_VALUE
#define EXTRACT_RETURN_VALUE(TYPE,REGBUF,VALBUF) \
  i386_extract_return_value (TYPE,REGBUF,VALBUF)

/* Write into appropriate registers a function return value
   of type TYPE, given in virtual format.  */
#undef STORE_RETURN_VALUE
#define STORE_RETURN_VALUE(TYPE,VALBUF) \
  {    	       	       	       	       	       	       	       	       	     \
    if (TYPE_CODE (TYPE) == TYPE_CODE_FLT)				     \
      write_register_bytes (REGISTER_BYTE (FP0_REGNUM), (VALBUF),	     \
			    TYPE_LENGTH (TYPE));			     \
    else								     \
      write_register_bytes (0, (VALBUF), TYPE_LENGTH (TYPE));  		     \
  }

#endif /* TM_I386AIX_H */
