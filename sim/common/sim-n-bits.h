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

#ifdef __STDC__
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
#define EXTRACTEDn XCONCAT2(EXTRACTED,N)
#define INSERTEDn XCONCAT2(INSERTED,N)
#define ROTn XCONCAT2(ROT,N)
#define ROTLn XCONCAT2(ROTL,N)
#define ROTRn XCONCAT2(ROTR,N)
#define SEXTn XCONCAT2(SEXT,N)


INLINE_SIM_BITS\
(unsignedN)
MASKEDn (unsignedN word,
	 unsigned start,
	 unsigned stop)
{
  return (word & MASKn (start, stop));
}


INLINE_SIM_BITS\
(unsignedN)
LSMASKEDn (unsignedN word,
	   unsigned nr_bits)
{
  return (word & LSMASKn (nr_bits));
}


INLINE_SIM_BITS\
(unsignedN)
MSMASKEDn (unsignedN word,
	   unsigned nr_bits)
{
  return (word & MSMASKn (nr_bits));
}


INLINE_SIM_BITS\
(unsignedN)
EXTRACTEDn (unsignedN val,
	    unsigned start,
	    unsigned stop)
{
  val <<= _MSB_SHIFT (N, start);
  val >>= (_MSB_SHIFT (N, start) + _LSB_SHIFT (N, stop));
  return val;
}


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
#undef EXTRACTEDn
#undef LSMASKEDn
#undef LSMASKn
#undef MSMASKEDn
#undef MSMASKn
#undef MASKn
#undef MASKEDn
#undef signedN
#undef unsignedN
