/*  This file is part of the program psim.

    Copyright (C) 1994-1995, Andrew Cagney <cagney@highland.com.au>

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


#ifndef _BITS_H_
#define _BITS_H_

/* bit manipulation routines:

   Bit numbering: The bits are numbered according to the PowerPC
   convention - the left most (or most significant) is bit 0 while the
   right most (least significant) is bit 1.

   Size convention: Each macro is in three forms - <MACRO>32 which
   operates in 32bit quantity (bits are numbered 0..31); <MACRO>64
   which operates using 64bit quantites (and bits are numbered 0..64);
   and <MACRO> which operates using the bit size of the target
   architecture (bits are still numbered 0..63), with 32bit
   architectures ignoring the first 32bits having bit 32 as the most
   significant.

   BIT*(POS): Quantity with just 1 bit set.

   MASK*(FIRST, LAST): Create a constant bit mask of the specified
   size with bits [FIRST .. LAST] set.

   MASKED*(VALUE, FIRST, LAST): Masks out all but bits [FIRST
   .. LAST].

   EXTRACTED*(VALUE, FIRST, LAST): Masks out bits [FIRST .. LAST] but
   also right shifts the masked value so that bit LAST becomes the
   least significant (right most).

   SHUFFLE*(VALUE, OLD, NEW): Moves things around so that bit pos OLD
   is extracted and than moved to bit pos NEW.

   

   IEA_MASKED(SHOULD_MASK, ADDR): Convert the address to the targets
   natural size.  If in 32bit mode, discard the high 32bits.

   EXTENDED(VALUE): Convert VALUE (32bits of it) to the targets
   natural size.  If in 64bit mode, sign extend the value.

   */

/* Bit operators */
#define BIT4(POS)  (1 << _MAKE_SHIFT(4, POS))
#define BIT5(POS)  (1 << _MAKE_SHIFT(5, POS))
#define BIT10(POS)  (1 << _MAKE_SHIFT(10, POS))
#define BIT32(POS) _BITn(32, POS)
#define BIT64(POS) _BITn(64, POS)

#if (WITH_64BIT_TARGET)
#define BIT(POS)   BIT64(POS)
#else
#define BIT(POS)   (((POS) < 32) ? 0 : _BITn(32, (POS)-32))
#endif


/* multi bit mask */

#define MASK32(START, STOP)   _MASKn(32, START, STOP)
#define MASK64(START, STOP)   _MASKn(64, START, STOP)

#if (WITH_64BIT_TARGET)
#define MASK(START, STOP) (((START) <= (STOP)) \
			   ? _MASKn(64, START, STOP) \
			   : (_MASKn(64, 0, STOP) \
			      | _MASKn(64, START, 63)))
#else
#define MASK(START, STOP)  (((START) <= (STOP)) \
			    ? (((STOP) < 32) \
			       ? 0 \
			       : _MASKn(32, \
					(START) < 32 ? 0 : (START) - 32, \
					(STOP)-32)) \
			    : (_MASKn(32, \
				      (START) < 32 ? 0 : (START) - 32, \
				      31) \
			       | (((STOP) < 32) \
				  ? 0 \
				  : _MASKn(32, \
					   0, \
					   (STOP) - 32))))
#endif


#define MASKED32(WORD, START, STOP)    _MASKEDn(32, WORD, START, STOP)
#define MASKED64(WORD, START, STOP)    _MASKEDn(64, WORD, START, STOP)
#define MASKED10(WORD, START, STOP)    _MASKEDn(10, WORD, START, STOP)
#define EXTRACTED32(WORD, START, STOP) _EXTRACTEDn(32, WORD, START, STOP)
#define EXTRACTED64(WORD, START, STOP) _EXTRACTEDn(64, WORD, START, STOP)
#define EXTRACTED10(WORD, START, STOP) _EXTRACTEDn(10, WORD, START, STOP)

#define MASKED(WORD, START, STOP)   ((natural_word)(WORD) & MASK(START, STOP))
#if (WITH_64BITS_TARGET)
#define EXTRACTED(WORD, START, STOP)   _EXTRACTEDn(64, WORD, START, STOP)
#else
#define EXTRACTED(WORD, START, STOP) (STOP < 32 \
				      ? 0 \
				      : (((natural_word)WORD \
					  >> (63 - (STOP))) \
					 && MASK(START+(63-STOP), 63)))
#endif


#define SHUFFLE32(WORD, OLD, NEW) _SHUFFLEn(32, WORD, OLD, NEW)
#define SHUFFLE64(WORD, OLD, NEW) _SHUFFLEn(64, WORD, OLD, NEW)
#define SHUFFLE(WORD, OLD, NEW) _SHUFFLEn(_word, WORD, OLD, NEW)
/* NB: the wierdness (N>O?N-O:0) is to stop a warning from GCC */
#define _SHUFFLEn(N, WORD, OLD, NEW) \
((OLD) < (NEW) \
 ? (((unsigned##N)(WORD) \
     >> (((NEW) > (OLD)) ? ((NEW) - (OLD)) : 0)) \
    & MASK32((NEW), (NEW))) \
 : (((unsigned##N)(WORD) \
     << (((OLD) > (NEW)) ? ((OLD) - (NEW)) : 0)) \
    & MASK32((NEW), (NEW))))



/* depending on MODE return a 64bit or 32bit (sign extended) value */
#if (WITH_64BIT_TARGET)
#define EXTENDED(X)     ((signed64)(signed32)(X))
#else
#define EXTENDED(X)     (X)
#endif




/* memory alignment macro's */
#define _ALIGNa(A,X)  (((X) + ((A)-1)) & ~((A)-1))
#define ALIGN_8(X)	_ALIGNa(8, X)
#define ALIGN_16(X)	_ALIGNa(16, X)
#define ALIGN_PAGE(X)	_ALIGNa(0x1000, X)
#define FLOOR_PAGE(X)   ((X) & ~(0x1000 - 1))

/* bit bliting macro's */
#define BLIT32(V, POS, BIT) \
do { \
  if (BIT) \
    V |= BIT32(POS); \
  else \
    V &= ~BIT32(POS); \
} while (0)
#define MLIT32(V, LO, HI, VAL) \
do { \
  (V) = (((V) & ~MASK32((LO), (HI))) \
	 | ((VAL) << _MAKE_SHIFT(32,HI))); \
} while (0)



/* Things for creating single bit set values */
/* MakeBit */
#define _MAKE_SHIFT(WIDTH, pos) (WIDTH - 1 - (pos))
#define _BITn(WIDTH, pos) (((natural##WIDTH)(1)) \
			   << _MAKE_SHIFT(WIDTH, pos))
/* MakeBitMask */
#define _MASKn(WIDTH, START, STOP) (((((unsigned##WIDTH)0) - 1) \
				     >> (WIDTH - ((STOP) - (START) + 1))) \
				    << (WIDTH - 1 - (STOP)))



/* mask the required bits, leaving them in place */
#define _MASKEDn(WIDTH, WORD, START, STOP) \
(((natural##WIDTH)(WORD)) & MASK##WIDTH(START, STOP))

/* extract the required bits aligning them with the lsb */
#define _EXTRACTEDn(WIDTH, WORD, START, STOP) \
((((natural##WIDTH)(WORD)) >> (WIDTH - (STOP) - 1)) \
 & _MASKn(WIDTH, WIDTH-1+(START)-(STOP), WIDTH-1))

#endif /* _BITS_H_ */
