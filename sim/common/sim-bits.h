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


#ifndef _SIM_BITS_H_
#define _SIM_BITS_H_


/* bit manipulation routines:

   Bit numbering: The bits are numbered according to the target ISA's
   convention.  That being controlled by WITH_TARGET_WORD_MSB.  For
   the PowerPC (WITH_TARGET_WORD_MSB == 0) the numbering is 0..31
   while for the MIPS (WITH_TARGET_WORD_MSB == 31) it is 31..0.

   Size convention: Each macro is in three forms - <MACRO>32 which
   operates in 32bit quantity (bits are numbered 0..31); <MACRO>64
   which operates using 64bit quantites (and bits are numbered 0..63);
   and <MACRO> which operates using the bit size of the target
   architecture (bits are still numbered 0..63), with 32bit
   architectures ignoring the first 32bits leaving bit 32 as the most
   significant.

   NB: Use EXTRACTED, MSEXTRACTED and LSEXTRACTED as a guideline for
   naming.  LSMASK and LSMASKED are wrong.

   BIT*(POS): Constant with just 1 bit set.

   LSBIT*(OFFSET): Constant with just 1 bit set - LS bit is zero.

   MSBIT*(OFFSET): Constant with just 1 bit set - MS bit is zero.

   MASK*(FIRST, LAST): Constant with bits [FIRST .. LAST] set. The
   <MACRO> (no size) version permits FIRST >= LAST and generates a
   wrapped bit mask vis ([0..LAST] | [FIRST..LSB]).

   LSMASK*(NR_BITS): Like MASK only NR least significant bits are set.

   MSMASK*(NR_BITS): Like MASK only NR most significant bits are set.

   MASKED*(VALUE, FIRST, LAST): Masks out all but bits [FIRST
   .. LAST].

   LSMASKED*(VALUE, NR_BITS): Mask out all but the least significant
   NR_BITS of the value.

   MSMASKED*(VALUE, NR_BITS): Mask out all but the most significant
   NR_BITS of the value.

   EXTRACTED*(VALUE, FIRST, LAST): Masks out bits [FIRST .. LAST] but
   also right shifts the masked value so that bit LAST becomes the
   least significant (right most).

   LSEXTRACTED*(VALUE, FIRST, LAST): Same as extracted - LS bit is
   zero.

   MSEXTRACTED*(VALUE, FIRST, LAST): Same as extracted - MS bit is
   zero.

   SHUFFLED**(VALUE, OLD, NEW): Mask then move a single bit from OLD
   new NEW.

   MOVED**(VALUE, OLD_FIRST, OLD_LAST, NEW_FIRST, NEW_LAST): Moves
   things around so that bits OLD_FIRST..OLD_LAST are masked then
   moved to NEW_FIRST..NEW_LAST.

   INSERTED*(VALUE, FIRST, LAST): Takes VALUE and `inserts' the (LAST
   - FIRST + 1) least significant bits into bit positions [ FIRST
   .. LAST ].  This is almost the complement to EXTRACTED.

   IEA_MASKED(SHOULD_MASK, ADDR): Convert the address to the targets
   natural size.  If in 32bit mode, discard the high 32bits.

   EXTENDED(VALUE): Convert VALUE (32bits of it) to the targets
   natural size.  If in 64bit mode, sign extend the value.

   ALIGN_*(VALUE): Round upwards the value so that it is aligned.

   FLOOR_*(VALUE): Truncate the value so that it is aligned.

   ROTL*(VALUE, NR_BITS): Return the value rotated by NR_BITS left.

   ROTR*(VALUE, NR_BITS): Return the value rotated by NR_BITS right.

   SEXT*(VAL, SIGN_BIT): Treat SIGN_BIT as the sign, extend it.

   Note: Only the BIT* and MASK* macros return a constant that can be
   used in variable declarations.

   */


/* compute the number of bits between START and STOP */

#if (WITH_TARGET_WORD_MSB == 0)
#define _MAKE_WIDTH(START, STOP) (STOP - START + 1)
#else
#define _MAKE_WIDTH(START, STOP) (START - STOP + 1)
#endif



/* compute the number shifts required to move a bit between LSB (MSB)
   and POS */

#if (WITH_TARGET_WORD_MSB == 0)
#define _LSB_SHIFT(WIDTH, POS) (WIDTH - 1 - POS)
#else
#define _LSB_SHIFT(WIDTH, POS) (POS)
#endif

#if (WITH_TARGET_WORD_MSB == 0)
#define _MSB_SHIFT(WIDTH, POS) (POS)
#else
#define _MSB_SHIFT(WIDTH, POS) (WIDTH - 1 - POS)
#endif


/* compute the absolute bit position given the OFFSET from the MSB(LSB)
   NB: _MAKE_xxx_POS (WIDTH, _MAKE_xxx_SHIFT (WIDTH, POS)) == POS */

#if (WITH_TARGET_WORD_MSB == 0)
#define _MSB_POS(WIDTH, SHIFT) (SHIFT)
#else
#define _MSB_POS(WIDTH, SHIFT) (WIDTH - 1 - SHIFT)
#endif

#if (WITH_TARGET_WORD_MSB == 0)
#define _LSB_POS(WIDTH, SHIFT) (WIDTH - 1 - SHIFT)
#else
#define _LSB_POS(WIDTH, SHIFT) (SHIFT)
#endif


/* convert a 64 bit position into a corresponding 32bit position. MSB
   pos handles the posibility that the bit lies beyond the 32bit
   boundary */

#if (WITH_TARGET_WORD_MSB == 0)
#define _MSB_32(START, STOP) (START <= STOP \
			      ? (START < 32 ? 0 : START - 32) \
			      : (STOP < 32 ? 0 : STOP - 32))
#else
#define _MSB_32(START, STOP) (START >= STOP \
			      ? (START >= 32 ? 31 : START) \
			      : (STOP >= 32 ? 31 : STOP))
#endif

#if (WITH_TARGET_WORD_MSB == 0)
#define _LSB_32(START, STOP) (START <= STOP \
			      ? (STOP < 32 ? 0 : STOP - 32) \
			      : (START < 32 ? 0 : START - 32))
#else
#define _LSB_32(START, STOP) (START >= STOP \
			      ? (STOP >= 32 ? 31 : STOP) \
			      : (START >= 32 ? 31 : START))
#endif

#if (WITH_TARGET_WORD_MSB == 0)
#define _MSB(START, STOP) (START <= STOP ? START : STOP)
#else
#define _MSB(START, STOP) (START >= STOP ? START : STOP)
#endif

#if (WITH_TARGET_WORD_MSB == 0)
#define _LSB(START, STOP) (START <= STOP ? STOP : START)
#else
#define _LSB(START, STOP) (START >= STOP ? STOP : START)
#endif


/* Bit operations */

#define _BITn(WIDTH, POS) ((natural##WIDTH)1 \
			   << _LSB_SHIFT (WIDTH, POS))

#define BIT4(POS)  (1 << _LSB_SHIFT (4, (POS)))
#define BIT5(POS)  (1 << _LSB_SHIFT (5, (POS)))
#define BIT8(POS)  (1 << _LSB_SHIFT (8, (POS)))
#define BIT10(POS) (1 << _LSB_SHIFT (10, (POS)))
#define BIT16(POS) _BITn (16, (POS))
#define BIT32(POS) _BITn (32, (POS))
#define BIT64(POS) _BITn (64, (POS))

#if (WITH_TARGET_WORD_BITSIZE == 64)
#define BIT(POS) BIT64(POS)
#endif
#if (WITH_TARGET_WORD_BITSIZE == 32)
#if (WITH_TARGET_WORD_MSB == 0)
#define BIT(POS) ((POS) < 32 \
		  ? 0 \
		  : (1 << ((POS) < 32 ? 0 : _LSB_SHIFT(64, (POS)))))
#else
#define BIT(POS) ((POS) >= 32 \
		  ? 0 \
		  : (1 << ((POS) >= 32 ? 0 : (POS))))
#endif
#endif
#if !defined (BIT)
#error "BIT never defined"
#endif


/* LS/MS Bit operations */

#define LSBIT8(POS) ((unsigned8)1 << (POS))
#define LSBIT16(POS) ((unsigned16)1 << (POS))
#define LSBIT32(POS) ((unsigned32)1 << (POS))
#define LSBIT64(POS) ((unsigned64)1 << (POS))
#define LSBIT(POS) ((unsigned_word)1 << (POS))

#define MSBIT8(POS) ((unsigned8)1 << (8 - 1 - (POS)))
#define MSBIT16(POS) ((unsigned16)1 << (16 - 1 - (POS)))
#define MSBIT32(POS) ((unsigned32)1 << (32 - 1 - (POS)))
#define MSBIT64(POS) ((unsigned64)1 << (64 - 1 - (POS)))
#define MSBIT(POS) ((unsigned_word)1 << (WITH_TARGET_WORD_BITSIZE - 1 - (POS)))



/* multi bit mask */

/* 111111 -> mmll11 -> mm11ll */
#define _MASKn(WIDTH, START, STOP) (((unsigned##WIDTH)(-1) \
				     >> (_MSB_SHIFT (WIDTH, START) \
					 + _LSB_SHIFT (WIDTH, STOP))) \
				    << _LSB_SHIFT (WIDTH, STOP))

#define MASK16(START, STOP)   _MASKn(16, (START), (STOP))
#define MASK32(START, STOP)   _MASKn(32, (START), (STOP))
#define MASK64(START, STOP)   _MASKn(64, (START), (STOP))

#if (WITH_TARGET_WORD_MSB == 0)
#define _POS_LE(START, STOP) (START <= STOP)
#else
#define _POS_LE(START, STOP) (STOP <= START)
#endif

#if (WITH_TARGET_WORD_BITSIZE == 64)
#define MASK(START, STOP) \
     (_POS_LE ((START), (STOP)) \
      ? _MASKn(64, \
	       _MSB ((START), (STOP)), \
	       _LSB ((START), (STOP)) ) \
      : (_MASKn(64, _MSB_POS (64, 0), (STOP)) \
	 | _MASKn(64, (START), _LSB_POS (64, 0))))
#endif
#if (WITH_TARGET_WORD_BITSIZE == 32)
#define MASK(START, STOP) \
     (_POS_LE ((START), (STOP)) \
      ? (_POS_LE ((STOP), _MSB_POS (64, 31)) \
	 ? 0 \
	 : _MASKn (32, \
		   _MSB_32 ((START), (STOP)), \
		   _LSB_32 ((START), (STOP)))) \
      : (_MASKn (32, \
		 _LSB_32 ((START), (STOP)), \
		 _LSB_POS (32, 0)) \
	 | (_POS_LE ((STOP), _MSB_POS (64, 31)) \
	    ? 0 \
	    : _MASKn (32, \
		      _MSB_POS (32, 0), \
		      _MSB_32 ((START), (STOP))))))
#endif
#if !defined (MASK)
#error "MASK never undefined"
#endif



/* Multi-bit mask on least significant bits */

#if (WITH_TARGET_WORD_MSB == 0)
#define _LSMASKn(WIDTH, NR_BITS) _MASKn(WIDTH, (WIDTH - NR_BITS), (WIDTH - 1))
#else
#define _LSMASKn(WIDTH, NR_BITS) _MASKn(WIDTH, (NR_BITS - 1), 0)
#endif

#define LSMASK16(NR_BITS)  _LSMASKn (16, (NR_BITS))
#define LSMASK32(NR_BITS)  _LSMASKn (32, (NR_BITS))
#define LSMASK64(NR_BITS)  _LSMASKn (64, (NR_BITS))

#if (WITH_TARGET_WORD_BITSIZE == 64)
#define LSMASK(NR_BITS) ((NR_BITS) < 1 \
			 ? 0 \
			 : _MASKn (64, \
				   _LSB_POS (64, \
					     ((NR_BITS) < 1 ? 0 \
					      : (NR_BITS) - 1)), \
				   _LSB_POS (64, 0)))
#endif
#if (WITH_TARGET_WORD_BITSIZE == 32)
#define LSMASK(NR_BITS) ((NR_BITS) < 1 \
			 ? 0 \
			 : _MASKn (32, \
				   _LSB_POS (32, \
					     ((NR_BITS) > 32 ? 31 \
					      : (NR_BITS) < 1 ? 0 \
					      : ((NR_BITS) - 1))), \
				   _LSB_POS (32, 0)))
#endif
#if !defined (LSMASK)
#error "LSMASK never defined"
#endif


/* Multi-bit mask on most significant bits */

#if (WITH_TARGET_WORD_MSB == 0)
#define _MSMASKn(WIDTH, NR_BITS) _MASKn (WIDTH, 0, (NR_BITS - 1))
#else
#define _MSMASKn(WIDTH, NR_BITS) _MASKn (WIDTH, (WIDTH - 1), (WIDTH - NR_BITS))
#endif

#define MSMASK16(NR_BITS) _MSMASKn (16, (NR_BITS))
#define MSMASK32(NR_BITS) _MSMASKn (32, (NR_BITS))
#define MSMASK64(NR_BITS) _MSMASKn (64, (NR_BITS))

#if (WITH_TARGET_WORD_BITSIZE == 64)
#define MSMASK(NR_BITS) (NR_BITS < 1 \
			 ? 0 \
			 : _MASKn (64, \
				   _MSB_POS (64, 0), \
				   _MSB_POS (64, \
					     ((NR_BITS) < 1 ? 0 \
					      : (NR_BITS) - 1))))
#endif
#if (WITH_TARGET_WORD_BITSIZE == 32)
#define MSMASK(NR_BITS) (NR_BITS <= 32 \
			 ? 0 \
			 : _MASKn (32, \
				   _MSB_POS (32, 0), \
				   _MSB_POS (32, \
					     ((NR_BITS) <= 32 ? 0 \
					      : (NR_BITS) - 33))))
#endif
#if !defined (MSMASK)
#error "MSMASK never defined"
#endif


/* mask the required bits, leaving them in place */

INLINE_SIM_BITS(unsigned16) MASKED16 (unsigned16 word, unsigned start, unsigned stop);
INLINE_SIM_BITS(unsigned32) MASKED32 (unsigned32 word, unsigned start, unsigned stop);
INLINE_SIM_BITS(unsigned64) MASKED64 (unsigned64 word, unsigned start, unsigned stop);

INLINE_SIM_BITS(unsigned_word) MASKED (unsigned_word word, unsigned start, unsigned stop);


/* Ditto but nr of ls-bits specified */

INLINE_SIM_BITS(unsigned16) LSMASKED16 (unsigned16 word, unsigned nr_bits);
INLINE_SIM_BITS(unsigned32) LSMASKED32 (unsigned32 word, unsigned nr_bits);
INLINE_SIM_BITS(unsigned64) LSMASKED64 (unsigned64 word, unsigned nr_bits);

INLINE_SIM_BITS(unsigned_word) LSMASKED (unsigned_word word, unsigned nr_bits);


/* Ditto but nr of ms-bits specified */

INLINE_SIM_BITS(unsigned16) MSMASKED16 (unsigned16 word, unsigned nr_bits);
INLINE_SIM_BITS(unsigned32) MSMASKED32 (unsigned32 word, unsigned nr_bits);
INLINE_SIM_BITS(unsigned64) MSMASKED64 (unsigned64 word, unsigned nr_bits);

INLINE_SIM_BITS(unsigned_word) MSMASKED (unsigned_word word, unsigned nr_bits);



/* extract the required bits aligning them with the lsb */

INLINE_SIM_BITS(unsigned16) LSEXTRACTED16 (unsigned16 val, unsigned start, unsigned stop);
INLINE_SIM_BITS(unsigned32) LSEXTRACTED32 (unsigned32 val, unsigned start, unsigned stop);
INLINE_SIM_BITS(unsigned64) LSEXTRACTED64 (unsigned64 val, unsigned start, unsigned stop);

INLINE_SIM_BITS(unsigned16) MSEXTRACTED16 (unsigned16 val, unsigned start, unsigned stop);
INLINE_SIM_BITS(unsigned32) MSEXTRACTED32 (unsigned32 val, unsigned start, unsigned stop);
INLINE_SIM_BITS(unsigned64) MSEXTRACTED64 (unsigned64 val, unsigned start, unsigned stop);

#if (WITH_TARGET_WORD_MSB == 0)
#define EXTRACTED16 MSEXTRACTED32
#define EXTRACTED32 MSEXTRACTED32
#define EXTRACTED64 MSEXTRACTED32
#else
#define EXTRACTED16 LSEXTRACTED32
#define EXTRACTED32 LSEXTRACTED32
#define EXTRACTED64 LSEXTRACTED32
#endif




INLINE_SIM_BITS(unsigned_word) EXTRACTED (unsigned_word val, unsigned start, unsigned stop);



/* move a single bit around */
/* NB: the wierdness (N>O?N-O:0) is to stop a warning from GCC */
#define _SHUFFLEDn(N, WORD, OLD, NEW) \
((OLD) < (NEW) \
 ? (((unsigned##N)(WORD) \
     >> (((NEW) > (OLD)) ? ((NEW) - (OLD)) : 0)) \
    & MASK32((NEW), (NEW))) \
 : (((unsigned##N)(WORD) \
     << (((OLD) > (NEW)) ? ((OLD) - (NEW)) : 0)) \
    & MASK32((NEW), (NEW))))

#define SHUFFLED32(WORD, OLD, NEW) _SHUFFLEDn (32, WORD, OLD, NEW)
#define SHUFFLED64(WORD, OLD, NEW) _SHUFFLEDn (64, WORD, OLD, NEW)

#define SHUFFLED(WORD, OLD, NEW) _SHUFFLEDn (_word, WORD, OLD, NEW)


/* move a group of bits around */

INLINE_SIM_BITS(unsigned16) INSERTED16 (unsigned16 val, unsigned start, unsigned stop);
INLINE_SIM_BITS(unsigned32) INSERTED32 (unsigned32 val, unsigned start, unsigned stop);
INLINE_SIM_BITS(unsigned64) INSERTED64 (unsigned64 val, unsigned start, unsigned stop);

INLINE_SIM_BITS(unsigned_word) INSERTED (unsigned_word val, unsigned start, unsigned stop);



/* depending on MODE return a 64bit or 32bit (sign extended) value */
#if (WITH_TARGET_WORD_BITSIZE == 64)
#define EXTENDED(X)     ((signed64)(signed32)(X))
#endif
#if (WITH_TARGET_WORD_BITSIZE == 32)
#define EXTENDED(X)     (X)
#endif


/* memory alignment macro's */
#define _ALIGNa(A,X)  (((X) + ((A) - 1)) & ~((A) - 1))
#define _FLOORa(A,X)  ((X) & ~((A) - 1))

#define ALIGN_8(X)	_ALIGNa (8, X)
#define ALIGN_16(X)	_ALIGNa (16, X)

#define ALIGN_PAGE(X)	_ALIGNa (0x1000, X)
#define FLOOR_PAGE(X)   ((X) & ~(0x1000 - 1))


/* bit bliting macro's */
#define BLIT32(V, POS, BIT) \
do { \
  if (BIT) \
    V |= BIT32 (POS); \
  else \
    V &= ~BIT32 (POS); \
} while (0)
#define MBLIT32(V, LO, HI, VAL) \
do { \
  (V) = (((V) & ~MASK32 ((LO), (HI))) \
	 | INSERTED32 (VAL, LO, HI)); \
} while (0)



/* some rotate functions.  The generic macro's ROT, ROTL, ROTR are
   intentionally omited. */


INLINE_SIM_BITS(unsigned16) ROT16 (unsigned16 val, int shift);
INLINE_SIM_BITS(unsigned32) ROT32 (unsigned32 val, int shift);
INLINE_SIM_BITS(unsigned64) ROT64 (unsigned64 val, int shift);


INLINE_SIM_BITS(unsigned16) ROTL16 (unsigned16 val, unsigned shift);
INLINE_SIM_BITS(unsigned32) ROTL32 (unsigned32 val, unsigned shift);
INLINE_SIM_BITS(unsigned64) ROTL64 (unsigned64 val, unsigned shift);


INLINE_SIM_BITS(unsigned16) ROTR16 (unsigned16 val, unsigned shift);
INLINE_SIM_BITS(unsigned32) ROTR32 (unsigned32 val, unsigned shift);
INLINE_SIM_BITS(unsigned64) ROTR64 (unsigned64 val, unsigned shift);



/* Sign extension operations */

INLINE_SIM_BITS(unsigned16) SEXT16 (signed16 val, unsigned sign_bit);
INLINE_SIM_BITS(unsigned32) SEXT32 (signed32 val, unsigned sign_bit);
INLINE_SIM_BITS(unsigned64) SEXT64 (signed64 val, unsigned sign_bit);

INLINE_SIM_BITS(unsigned_word) SEXT (signed_word val, unsigned sign_bit);



#if ((SIM_BITS_INLINE & INCLUDE_MODULE) && (SIM_BITS_INLINE & INCLUDED_BY_MODULE))
#include "sim-bits.c"
#endif

#endif /* _SIM_BITS_H_ */
