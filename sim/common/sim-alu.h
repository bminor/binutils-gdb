/*  This file is part of the program psim.

    Copyright (C) 1994-1996, Andrew Cagney <cagney@highland.com.au>
    Copyright (C) 1997, Free Software Foundation, Inc.

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
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 
    */


#ifndef _SIM_ALU_H_
#define _SIM_ALU_H_

#include "sim-xcat.h"


/* Binary addition, carry and overflow:


   Overflow - method 1:

   Overflow occures when the sign of the two operands is identical but
   different to the sign of the result:

		SIGN_BIT (~(a ^ b) & ((a + b) ^ b))

   Note that, for subtraction, care must be taken with MIN_INTn.


   Overflow - method 2:

   The two N bit operands are sign extended to M>N bits and then
   added.  Overflow occures when SIGN_BIT<n> and SIGN_BIT<m> do not
   match.
  
		SIGN_BIT (r >> (M-N) ^ r)


   Overflow - method 3:

   The two N bit operands are sign extended to M>N bits and then
   added.  Overflow occures when the result is outside of signextended
   MIN_INTn, MAX_INTn.


   Overflow - method 4:

   Given the carry bit, the overflow can be computed using the
   equation:

		SIGN_BIT (((A ^ B) ^ R) ^ C)

   As shown in the table below:

         I  A  B  R  C | V | A^B  ^R  ^C
        ---------------+---+-------------
         0  0  0  0  0 | 0 |  0    0   0
         0  0  1  1  0 | 0 |  1    0   0
         0  1  0  1  0 | 0 |  1    0   0
         0  1  1  0  1 | 1 |  0    0   1
         1  0  0  1  0 | 1 |  0    1   1
         1  0  1  0  1 | 0 |  1    1   0
         1  1  0  0  1 | 0 |  1    1   0
         1  1  1  1  1 | 0 |  0    1   0



   Carry - method 1:

   Consider the truth table (carryIn, Result, Carryout, Result):

         I  A  B  R | C
        ------------+---
         0  0  0  0 | 0
         0  0  1  1 | 0
         0  1  0  1 | 0
         0  1  1  0 | 1
         1  0  0  1 | 0
         1  0  1  0 | 1
         1  1  0  0 | 1
         1  1  1  1 | 1

   Looking at the terms A, B and R we want an equation for C.

       AB\R  0  1
          +-------
       00 |  0  0 
       01 |  1  0
       11 |  1  1
       10 |  1  0

   This giving us the sum-of-prod equation:

		SIGN_BIT ((A & B) | (A & ~R) | (B & ~R))

   Verifying:

         I  A  B  R | C | A&B  A&~R B&~R 
        ------------+---+---------------
         0  0  0  0 | 0 |  0    0    0
         0  0  1  1 | 0 |  0    0    0
         0  1  0  1 | 0 |  0    0    0
         0  1  1  0 | 1 |  1    1    1
         1  0  0  1 | 0 |  0    0    0
         1  0  1  0 | 1 |  0    0    1
         1  1  0  0 | 1 |  0    1    0
         1  1  1  1 | 1 |  1    0    0



   Carry - method 2:

   Given two signed N bit numbers, a carry can be detected by treating
   the numbers as N bit unsigned and adding them using M>N unsigned
   arrithmetic.  Carry is indicated by bit (1 << N) being set (result
   >= 2**N).

		SIGN_BITm (r)


   Carry - method 3:

   Given the overflow bit.  The carry can be computed from:

		(~R&V) | (R&V)

   Carry - method 4:

   Add the two signed N bit numbers as unsigned N bit numbers, and then
   compare the result to either one of the inputs via unsigned compare.
   If the result is less than the inputs, carry occurred.   

	C = ((unsigned)(a+b)) < (unsigned)a	if adding
		(or)
	C = (unsigned)a < (unsigned)b		if subtracting
   */



/* 8 bit target expressions:

   Since the host's natural bitsize > 8 bits, carry method 2 and
   overflow method 2 are used. */

#define ALU8_BEGIN(VAL) \
signed alu8_cr = (unsigned8) (VAL); \
unsigned alu8_vr = (signed8) (alu8_cr)

#define ALU8_SET(VAL) \
alu8_cr = (unsigned8) (VAL); \
alu8_vr = (signed8) (alu8_cr)

#define ALU8_SET_CARRY(CARRY)						\
do {									\
  if (CARRY)								\
    alu8_cr |= ((signed)-1) << 8;					\
  else									\
    alu8_cr &= 0xff;							\
} while (0)
    
#define ALU8_HAD_CARRY (alu8_cr & LSBIT32(8))
#define ALU8_HAD_OVERFLOW (((alu8_vr >> 8) ^ alu8_vr) & LSBIT32 (8-1))

#define ALU8_RESULT ((unsigned8) alu8_cr)
#define ALU8_CARRY_RESULT ((unsigned8) alu8_cr)
#define ALU8_OVERFLOW_RESULT ((unsigned8) alu8_vr)

/* #define ALU8_END ????? - target dependant */



/* 16 bit target expressions:

   Since the host's natural bitsize > 16 bits, carry method 2 and
   overflow method 2 are used. */

#define ALU16_BEGIN(VAL) \
signed alu16_cr = (unsigned16) (VAL); \
unsigned alu16_vr = (signed16) (alu16_cr)

#define ALU16_SET(VAL) \
alu16_cr = (unsigned16) (VAL); \
alu16_vr = (signed16) (alu16_cr)

#define ALU16_SET_CARRY(CARRY)						\
do {									\
  if (CARRY)								\
    alu16_cr |= ((signed)-1) << 16;					\
  else									\
    alu16_cr &= 0xffff;							\
} while (0)

#define ALU16_HAD_CARRY (alu16_cr & LSBIT32(16))
#define ALU16_HAD_OVERFLOW (((alu16_vr >> 16) ^ alu16_vr) & LSBIT32 (16-1))

#define ALU16_RESULT ((unsigned16) alu16_cr)
#define ALU16_CARRY_RESULT ((unsigned16) alu16_cr)
#define ALU16_OVERFLOW_RESULT ((unsigned16) alu16_vr)

/* #define ALU16_END ????? - target dependant */



/* 32 bit target expressions:

   Since most hosts do not support 64 (> 32) bit arrithmetic, carry
   method 4 and overflow method 4 are used.

   FIXME: 64 bit hosts should use the same method as for the 16 bit
   ALU. */

#define ALU32_BEGIN(VAL) \
unsigned32 alu32_r = (VAL); \
int alu32_c = 0; \
int alu32_v = 0

#define ALU32_SET(VAL) \
alu32_r = (VAL); \
alu32_c = 0; \
alu32_v = 0

#define ALU32_SET_CARRY(CARRY) alu32_c = (CARRY)

#define ALU32_HAD_OVERFLOW (alu32_v)
#define ALU32_HAD_CARRY (alu32_c)

#define ALU32_RESULT (alu32_r)
#define ALU32_CARRY_RESULT (alu32_r)
#define ALU32_OVERFLOW_RESULT (alu32_r)



/* 64 bit target expressions:

   Even though the host typically doesn't support native 64 bit
   arrithmetic, it is still used. */

#define ALU64_BEGIN(VAL) \
natural64 alu64_r = (VAL); \
int alu64_c = 0; \
int alu64_v = 0

#define ALU64_SET(VAL) \
alu64_r = (VAL); \
alu64_c = 0; \
alu64_v = 0

#define ALU64_SET_CARRY(CARRY) alu64_c = (CARRY)

#define ALU64_HAD_CARRY (alu64_c)
#define ALU64_HAD_OVERFLOW (alu64_v)

#define ALU64_RESULT (alu64_r)
#define ALU64_CARRY_RESULT (alu64_r)
#define ALU64_OVERFLOW_RESULT (alu64_r)



/* Generic versions of above macros */

#define ALU_BEGIN	    XCONCAT3(ALU,WITH_TARGET_WORD_BITSIZE,_BEGIN)
#define ALU_SET		    XCONCAT3(ALU,WITH_TARGET_WORD_BITSIZE,_SET)
#define ALU_SET_CARRY	    XCONCAT3(ALU,WITH_TARGET_WORD_BITSIZE,_SET_CARRY)

#define ALU_HAD_OVERFLOW    XCONCAT3(ALU,WITH_TARGET_WORD_BITSIZE,_HAD_OVERFLOW)
#define ALU_HAD_CARRY       XCONCAT3(ALU,WITH_TARGET_WORD_BITSIZE,_HAD_CARRY)

#define ALU_RESULT          XCONCAT3(ALU,WITH_TARGET_WORD_BITSIZE,_RESULT)
#define ALU_OVERFLOW_RESULT XCONCAT3(ALU,WITH_TARGET_WORD_BITSIZE,_OVERFLOW_RESULT)
#define ALU_CARRY_RESULT    XCONCAT3(ALU,WITH_TARGET_WORD_BITSIZE,_CARRY_RESULT)



/* Basic operations */


#define ALU8_ADD(VAL)							\
do {									\
  unsigned8 alu8_tmp = (VAL);						\
  alu8_cr += (unsigned8)(alu8_tmp);					\
  alu8_vr += (signed8)(alu8_tmp);					\
} while (0)

#define ALU16_ADD(VAL)							\
do {									\
  unsigned16 alu16_tmp = (VAL);						\
  alu16_cr += (unsigned16)(alu16_tmp);					\
  alu16_vr += (signed16)(alu16_tmp);					\
} while (0)

#define ALU32_ADD(VAL)							\
do {									\
  unsigned32 alu32_tmp = (unsigned32) (VAL);				\
  unsigned32 alu32_sign = alu32_tmp ^ alu32_r;				\
  alu32_r += (alu32_tmp);						\
  alu32_c = (alu32_r < alu32_tmp);					\
  alu32_v = ((alu32_sign ^ - (unsigned32)alu32_c) ^ alu32_r) >> 31;	\
} while (0)

#define ALU64_ADD(VAL)							\
do {									\
  unsigned64 alu64_tmp = (unsigned64) (VAL);				\
  unsigned64 alu64_sign = alu64_tmp ^ alu64_r;				\
  alu64_r += (alu64_tmp);						\
  alu64_c = (alu64_r < alu64_tmp);					\
  alu64_v = ((alu64_sign ^ - (unsigned64)alu64_c) ^ alu64_r) >> 63;	\
} while (0)

#define ALU_ADD(VAL) XCONCAT3(ALU,WITH_TARGET_WORD_BITSIZE,_ADD)(VAL)



#define ALU8_ADD_CA(VAL)						\
do {									\
  unsigned8 alu8_ca_tmp = (VAL) + ALU8_HAD_CARRY;			\
  ALU8_ADD(alu8_ca_tmp);						\
} while (0)

#define ALU16_ADD_CA(VAL)						\
do {									\
  unsigned16 alu16_ca_tmp = (VAL) + ALU16_HAD_CARRY;			\
  ALU16_ADD(alu16_ca_tmp);						\
} while (0)

#define ALU32_ADD_CA(VAL)						\
do {									\
  unsigned32 alu32_ca_tmp = (VAL) + ALU32_HAD_CARRY;			\
  ALU32_ADD(alu32_ca_tmp);						\
} while (0)

#define ALU64_ADD_CA(VAL)						\
do {									\
  unsigned64 alu64_ca_tmp = (VAL) + ALU64_HAD_CARRY;			\
  ALU64_ADD(alu64_ca_tmp);						\
} while (0)

#define ALU_ADD_CA(VAL) XCONCAT3(ALU,WITH_TARGET_WORD_BITSIZE,_ADD_CA)(VAL)



/* Remember: Hardware implements subtract as an ADD with a carry in of
   1 into the least significant bit */

#define ALU8_SUB(VAL)							\
do {									\
  signed alu8sub_val = ~(VAL);						\
  ALU8_ADD (alu8sub_val);						\
  ALU8_ADD (1);								\
} while (0)

#define ALU16_SUB(VAL)							\
do {									\
  signed alu16sub_val = ~(VAL);						\
  ALU16_ADD (alu16sub_val);						\
  ALU16_ADD (1);							\
} while (0)

#define ALU32_SUB(VAL)							\
do {									\
  unsigned32 alu32_tmp = (unsigned32) (VAL);				\
  unsigned32 alu32_sign = alu32_tmp ^ alu32_r;				\
  alu32_c = (alu32_r < alu32_tmp);					\
  alu32_r -= (alu32_tmp);						\
  alu32_v = ((alu32_sign ^ - (unsigned32)alu32_c) ^ alu32_r) >> 31;	\
} while (0)

#define ALU64_SUB(VAL)							\
do {									\
  unsigned64 alu64_tmp = (unsigned64) (VAL);				\
  unsigned64 alu64_sign = alu64_tmp ^ alu64_r;				\
  alu64_c = (alu64_r < alu64_tmp);					\
  alu64_r -= (alu64_tmp);						\
  alu64_v = ((alu64_sign ^ - (unsigned64)alu64_c) ^ alu64_r) >> 63;	\
} while (0)

#define ALU_SUB(VAL) XCONCAT3(ALU,WITH_TARGET_WORD_BITSIZE,_SUB)(VAL)




#define ALU8_SUB_CA(VAL)						\
do {									\
  unsigned8 alu8_ca_tmp = (VAL) + ALU8_HAD_CARRY;			\
  ALU8_SUB(alu8_ca_tmp);						\
} while (0)

#define ALU16_SUB_CA(VAL)						\
do {									\
  unsigned16 alu16_ca_tmp = (VAL) + ALU16_HAD_CARRY;			\
  ALU16_SUB(alu16_ca_tmp);						\
} while (0)

#define ALU32_SUB_CA(VAL)						\
do {									\
  unsigned32 alu32_ca_tmp = (VAL) + ALU32_HAD_CARRY;			\
  ALU32_SUB(alu32_ca_tmp);						\
} while (0)

#define ALU64_SUB_CA(VAL)						\
do {									\
  unsigned64 alu64_ca_tmp = (VAL) + ALU64_HAD_CARRY;			\
  ALU64_SUB(alu64_ca_tmp);						\
} while (0)

#define ALU_SUB_CA(VAL) XCONCAT3(ALU,WITH_TARGET_WORD_BITSIZE,_SUB_CA)(VAL)



#define ALU16_OR(VAL)							\
do {									\
  error("ALU16_OR");							\
} while (0)

#define ALU32_OR(VAL)							\
do {									\
  alu32_r |= (VAL);							\
  alu32_c = 0;								\
  alu32_v = 0;								\
} while (0)

#define ALU64_OR(VAL)							\
do {									\
  alu64_r |= (VAL);							\
  alu64_c = 0;								\
  alu64_v = 0;								\
} while (0)

#define ALU_OR(VAL) XCONCAT3(ALU,WITH_TARGET_WORD_BITSIZE,_OR)(VAL)



#define ALU16_XOR(VAL)							\
do {									\
  error("ALU16_XOR");							\
} while (0)

#define ALU32_XOR(VAL)							\
do {									\
  alu32_r ^= (VAL);							\
  alu32_c = 0;								\
  alu32_v = 0;								\
} while (0)

#define ALU64_XOR(VAL)							\
do {									\
  alu64_r ^= (VAL);							\
  alu64_c = 0;								\
  alu64_v = 0;								\
} while (0)

#define ALU_XOR(VAL) XCONCAT3(ALU,WITH_TARGET_WORD_BITSIZE,_XOR)(VAL)




#define ALU8_NEGATE()							\
do {									\
  signed alu8neg_val = ~(ALU8_RESULT);					\
  ALU8_SET (1);								\
  ALU8_ADD (alu8neg_val);						\
} while (0)

#define ALU16_NEGATE()							\
do {									\
  signed alu16neg_val = ~(ALU16_RESULT);				\
  ALU16_SET (1);							\
  ALU16_ADD (alu16neg_val);						\
} while (0)

#define ALU32_NEGATE()							\
do {									\
  unsigned32 alu32_tmp_orig = alu32_r;					\
  ALU32_SET (0);							\
  ALU32_SUB (alu32_tmp_orig);						\
} while(0)

#define ALU64_NEGATE()							\
do {									\
  unsigned64 alu64_tmp_orig = alu64_r;					\
  ALU64_SET (0);							\
  ALU64_SUB (alu64_tmp_orig);						\
} while (0)

#define ALU_NEGATE XCONCAT3(ALU,WITH_TARGET_WORD_BITSIZE,_NEGATE)




#define ALU16_AND(VAL)							\
do {									\
  error("ALU_AND16");							\
} while (0)

#define ALU32_AND(VAL)							\
do {									\
  alu32_r &= (VAL);							\
  alu32_r = 0;								\
  alu32_v = 0;								\
} while (0)

#define ALU64_AND(VAL)							\
do {									\
  alu64_r &= (VAL);							\
  alu64_r = 0;								\
  alu64_v = 0;								\
} while (0)

#define ALU_AND(VAL) XCONCAT3(ALU,WITH_TARGET_WORD_BITSIZE,_AND)(VAL)




#define ALU16_NOT(VAL)							\
do {									\
  error("ALU_NOT16");							\
} while (0)

#define ALU32_NOT							\
do {									\
  alu32_r = ~alu32_r;							\
  alu32_c = 0;								\
  alu32_v = 0;								\
} while (0)

#define ALU64_NOT							\
do {									\
  alu64_r = ~alu64_r;							\
  alu64_c = 0;								\
  alu64_v = 0;								\
} while (0)

#define ALU_NOT XCONCAT3(ALU,WITH_TARGET_WORD_BITSIZE,_NOT)

#endif
