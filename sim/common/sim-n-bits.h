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


#ifndef N
#error "N must be #defined"
#endif

#include "sim-xcat.h"

#if defined(__STDC__) && defined(signed)
/* If signed were defined to be say __signed (ie, some versions of Linux),
   then the signedN macro would not work correctly.  If we have a standard
   compiler, we have signed.  */
#undef signed
#endif

/* NOTE: See end of file for #undef */
#define unsignedN XCONCAT2(unsigned,N)
#define signedN XCONCAT2(signed,N)
#define MASKEDn XCONCAT2(MASKED,N)
#define MASKn XCONCAT2(MASK,N)
#define LSMASKEDn XCONCAT2(LSMASKED,N)
#define LSMASKn XCONCAT2(LSMASK,N)
#define MSMASKEDn XCONCAT2(MSMASKED,N)
#define MSMASKn XCONCAT2(MSMASK,N)
#define LSEXTRACTEDn XCONCAT2(LSEXTRACTED,N)
#define MSEXTRACTEDn XCONCAT2(MSEXTRACTED,N)
#define INSERTEDn XCONCAT2(INSERTED,N)
#define ROTn XCONCAT2(ROT,N)
#define ROTLn XCONCAT2(ROTL,N)
#define ROTRn XCONCAT2(ROTR,N)
#define SEXTn XCONCAT2(SEXT,N)

/* TAGS: MASKED16 MASKED32 MASKED64 */

INLINE_SIM_BITS\
(unsignedN)
MASKEDn (unsignedN word,
	 unsigned start,
	 unsigned stop)
{
  return (word & MASKn (start, stop));
}

/* TAGS: LSMASKED16 LSMASKED32 LSMASKED64 */

INLINE_SIM_BITS\
(unsignedN)
LSMASKEDn (unsignedN word,
	   unsigned nr_bits)
{
  return (word & LSMASKn (nr_bits));
}

/* TAGS: MSMASKED16 MSMASKED32 MSMASKED64 */

INLINE_SIM_BITS\
(unsignedN)
MSMASKEDn (unsignedN word,
	   unsigned nr_bits)
{
  return (word & MSMASKn (nr_bits));
}

/* TAGS: LSEXTRACTED16 LSEXTRACTED32 LSEXTRACTED64 */

INLINE_SIM_BITS\
(unsignedN)
LSEXTRACTEDn (unsignedN val,
	      unsigned start,
	      unsigned stop)
{
  val <<= (N - 1 - start); /* drop high bits */
  val >>= (N - 1 - start) + (stop); /* drop low bits */
  return val;
}

/* TAGS: MSEXTRACTED16 MSEXTRACTED32 MSEXTRACTED64 */

INLINE_SIM_BITS\
(unsignedN)
MSEXTRACTEDn (unsignedN val,
	      unsigned start,
	      unsigned stop)
{
  val <<= (start); /* drop high bits */
  val >>= (start) + (N - 1 - stop); /* drop low bits */
  return val;
}

/* TAGS: INSERTED16 INSERTED32 INSERTED64 */

INLINE_SIM_BITS\
(unsignedN)
INSERTEDn (unsignedN val,
	   unsigned start,
	   unsigned stop)
{
  val &= LSMASKn (_MAKE_WIDTH (start, stop));
  val <<= _LSB_SHIFT (N, stop);
  return val;
}

/* TAGS: ROT16 ROT32 ROT64 */

INLINE_SIM_BITS\
(unsignedN)
ROTn (unsignedN val,
      int shift)
{
  if (shift > 0)
    return ROTRn (val, shift);
  else if (shift < 0)
    return ROTLn (val, -shift);
  else
    return val;
}

/* TAGS: ROTL16 ROTL32 ROTL64 */

INLINE_SIM_BITS\
(unsignedN)
ROTLn (unsignedN val,
       unsigned shift)
{
  unsignedN result;
  ASSERT (shift <= N);
  result = (((val) << (shift)) | ((val) >> ((N)-(shift))));
  return result;
}

/* TAGS: ROTR16 ROTR32 ROTR64 */

INLINE_SIM_BITS\
(unsignedN)
ROTRn (unsignedN val,
       unsigned shift)
{
  unsignedN result;
  ASSERT (shift <= N);
  result = (((val) >> (shift)) | ((val) << ((N)-(shift))));
  return result;
}

/* TAGS: SEXT16 SEXT32 SEXT64 */

INLINE_SIM_BITS\
(unsignedN)
SEXTn (signedN val,
       unsigned sign_bit)
{
  /* make the sign-bit most significant and then smear it back into
     position */
  ASSERT (sign_bit < N);
  val <<= _MSB_SHIFT (N, sign_bit);
  val >>= _MSB_SHIFT (N, sign_bit);
  return val;
}


/* NOTE: See start of file for #define */
#undef SEXTn
#undef ROTLn
#undef ROTRn
#undef ROTn
#undef INSERTEDn
#undef LSEXTRACTEDn
#undef MSEXTRACTEDn
#undef LSMASKEDn
#undef LSMASKn
#undef MSMASKEDn
#undef MSMASKn
#undef MASKn
#undef MASKEDn
#undef signedN
#undef unsignedN
