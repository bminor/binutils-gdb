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

/* 32bit target expressions:

   Each calculation is performed three times using each of the
   signed64, unsigned64 and long integer types.  The macro ALU_END
   (in _ALU_RESULT_VAL) then selects which of the three alternative
   results will be used in the final assignment of the target
   register.  As this selection is determined at compile time by
   fields in the instruction (OE, EA, Rc) the compiler has sufficient
   information to firstly simplify the selection code into a single
   case and then back anotate the equations and hence eliminate any
   resulting dead code.  That dead code being the calculations that,
   as it turned out were not in the end needed.

   64bit arrithemetic is used firstly because it allows the use of
   gcc's efficient long long operators (typically efficiently output
   inline) and secondly because the resultant answer will contain in
   the low 32bits the answer while in the high 32bits is either carry
   or status information. */



/* 16bit target expressions:

   These are a simplified version of the 32bit target expressions */



/* 64bit target expressions:

   Unfortunatly 128bit arrithemetic isn't that common.  Consequently
   the 32/64 bit trick can not be used.  Instead all calculations are
   required to retain carry/overflow information in separate
   variables.  Even with this restriction it is still possible for the
   trick of letting the compiler discard the calculation of unneeded
   values */


/* Start a section of ALU code */

#define ALU16_BEGIN(VAL) \
{ \
  signed_word alu_carry_val; \
  unsigned_word alu_overflow_val; \
  ALU16_SET(VAL)

#define ALU32_BEGIN(VAL) \
{ \
  natural_word alu_val; \
  unsigned64 alu_carry_val; \
  signed64 alu_overflow_val; \
  ALU32_SET(VAL)

#define ALU64_BEGIN(VAL) \
{ \
  natural64 alu_val; \
  unsigned64 alu_carry_val; \
  signed64 alu_overflow_val; \
  ALU64_SET(VAL)


#define ALU_BEGIN(VAL) XCONCAT3(ALU,WITH_TARGET_WORD_BITSIZE,_BEGIN)(VAL)

/* More basic alu operations */



#define ALU16_SET(VAL) \
do { \
  alu_carry_val = (unsigned16)(VAL); \
  alu_overflow_val = (signed16)(VAL); \
} while (0)

#define ALU32_SET(VAL) \
do { \
  alu_val = (unsigned32)(VAL); \
  alu_carry_val = (unsigned32)(alu_val); \
  alu_overflow_val = (signed32)(alu_val); \
} while (0)

#define ALU64_SET(VAL) \
do { \
  alu_val = (VAL); \
  alu_carry_val = ((unsigned64)alu_val) >> 32; \
  alu_overflow_val = ((signed64)alu_val) >> 32; \
} while (0)

#define ALU_SET(VAL) XCONCAT3(ALU,WITH_TARGET_WORD_BITSIZE,_SET)(VAL)




#define ALU16_ADD(VAL) \
do { \
  alu_carry_val += (unsigned16)(VAL); \
  alu_overflow_val += (signed16)(VAL); \
} while (0)

#define ALU32_ADD(VAL) \
do { \
  alu_val += (VAL); \
  alu_carry_val += (unsigned32)(VAL); \
  alu_overflow_val += (signed32)(VAL); \
} while (0)

#define ALU64_ADD(VAL) \
do { \
  unsigned64 val = (VAL); \
  unsigned64 alu_lo = alu_val + val; \
  signed alu_carry = ((alu_lo & LSBIT64 (31)) != 0); \
  alu_carry_val = (alu_carry_val \
		   + MSEXTRACTED64 (val, 0, 31) \
		   + alu_carry); \
  alu_overflow_val = (alu_overflow_val \
		      + MSEXTRACTED64 (val, 0, 31) \
		      + alu_carry); \
  alu_val = alu_val + val; \
} while (0)

#define ALU_ADD(VAL) XCONCAT3(ALU,WITH_TARGET_WORD_BITSIZE,_ADD)(VAL)




#define ALU16_ADD_CA \
do { \
  signed carry = ALU_CARRY; \
  ALU16_ADD(carry); \
} while (0)

#define ALU32_ADD_CA \
do { \
  signed carry = ALU_CARRY; \
  ALU32_ADD(carry); \
} while (0)

#define ALU64_ADD_CA \
do { \
  signed carry = ALU_CARRY; \
  ALU64_ADD(carry); \
} while (0)

#define ALU_ADD_CA XCONCAT3(ALU,WITH_TARGET_WORD_BITSIZE,_ADD_CA)




#define ALU16_SUB(VAL) \
do { \
  alu_carry_val -= (unsigned16)(VAL); \
  alu_overflow_val -= (signed16)(VAL); \
} while (0)

#define ALU32_SUB(VAL) \
do { \
  alu_val -= (VAL); \
  alu_carry_val -= (unsigned32)(VAL); \
  alu_overflow_val -= (signed32)(VAL); \
} while (0)

#define ALU64_SUB(VAL) \
do { \
  signed64 subval = -(VAL); /* -MININT? */ \
  ALU64_ADD (subval); \
} while (0)

#define ALU_SUB(VAL) XCONCAT3(ALU,WITH_TARGET_WORD_BITSIZE,_SUB)(VAL)




#define ALU16_SUB_CA \
do { \
  signed carry = ALU_CARRY; \
  ALU16_SUB(carry); \
} while (0)

#define ALU32_SUB_CA \
do { \
  signed carry = ALU_CARRY; \
  ALU32_SUB(carry); \
} while (0)

#define ALU64_SUB_CA \
do { \
  signed carry = ALU_CARRY; \
  ALU64_SUB(carry); \
} while (0)

#define ALU_SUB_CA XCONCAT3(ALU,WITH_TARGET_WORD_BITSIZE,_SUB_CA)




#define ALU16_OR(VAL) \
do { \
  error("ALU16_OR"); \
} while (0)

#define ALU32_OR(VAL) \
do { \
  alu_val |= (VAL); \
  alu_carry_val = (unsigned32)(alu_val); \
  alu_overflow_val = (signed32)(alu_val); \
} while (0)

#define ALU64_OR(VAL) \
do { \
  error("ALU_OR64"); \
} while (0)

#define ALU_OR(VAL) XCONCAT3(ALU,WITH_TARGET_WORD_BITSIZE,_OR)(VAL)




#define ALU16_XOR(VAL) \
do { \
  error("ALU16_XOR"); \
} while (0)

#define ALU32_XOR(VAL) \
do { \
  alu_val ^= (VAL); \
  alu_carry_val = (unsigned32)(alu_val); \
  alu_overflow_val = (signed32)(alu_val); \
} while (0)

#define ALU64_XOR(VAL) \
do { \
  error("ALU_XOR64"); \
} while (0)

#define ALU_XOR(VAL) XCONCAT3(ALU,WITH_TARGET_WORD_BITSIZE,_XOR)(VAL)




#define ALU16_NEGATE \
do { \
  error("ALU_NEGATE16"); \
} while (0)

#define ALU32_NEGATE \
do { \
  alu_val = -alu_val; \
  alu_carry_val = -alu_carry_val; \
  alu_overflow_val = -alu_overflow_val; \
} while(0)

#define ALU64_NEGATE \
do { \
  error("ALU_NEGATE64"); \
} while (0)

#define ALU_NEGATE XCONCAT3(ALU,WITH_TARGET_WORD_BITSIZE,_NEGATE)




#define ALU16_AND(VAL) \
do { \
  error("ALU_AND16"); \
} while (0)

#define ALU32_AND(VAL) \
do { \
  alu_val &= (VAL); \
  alu_carry_val = (unsigned32)(alu_val); \
  alu_overflow_val = (signed32)(alu_val); \
} while (0)

#define ALU64_AND(VAL) \
do { \
  error("ALU_AND64"); \
} while (0)

#define ALU_AND(VAL) XCONCAT3(ALU,WITH_TARGET_WORD_BITSIZE,_AND)(VAL)




#define ALU16_NOT(VAL) \
do { \
  error("ALU_NOT16"); \
} while (0)

#define ALU32_NOT \
do { \
  signed64 new_alu_val = ~alu_val; \
  ALU_SET(new_alu_val); \
} while (0)

#define ALU64_NOT \
do { \
  error("ALU_NOT64"); \
} while (0)

#define ALU_NOT XCONCAT3(ALU,WITH_TARGET_WORD_BITSIZE,_NOT)



/* Make available various results */


/* overflow occures if the sign bit differs from the carry bit */

#define ALU16_HAD_OVERFLOW \
  (!(alu_overflow_val & MSBIT32 (0)) != !(alu_overflow_val & MSBIT32 (16)))
  
#define ALU32_HAD_OVERFLOW \
  ((((unsigned64)(alu_overflow_val & MSBIT64(0))) >> 32) \
   != (unsigned64)(alu_overflow_val & MSBIT64(32)))

#define ALU64_HAD_OVERFLOW \
  ((alu_val & MSBIT64 (0)) != (alu_overflow_val & MSBIT64 (0)))

#define ALU_HAD_OVERFLOW XCONCAT3(ALU,WITH_TARGET_WORD_BITSIZE,_HAD_OVERFLOW)


/* carry found in bit before sign */

#define ALU16_HAD_CARRY \
    (alu_carry_val & MSBIT32(16))

#define ALU32_HAD_CARRY \
    (alu_carry_val & MSBIT64(31))


#endif
