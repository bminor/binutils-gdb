/*  This file is part of the program psim.

    Copyright (C) 1994-1997, Andrew Cagney <cagney@highland.com.au>

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


#ifndef _SIM_BITS_C_
#define _SIM_BITS_C_

#include "sim-basics.h"


INLINE_SIM_BITS\
(unsigned_word)
MASKED(unsigned_word val,
       unsigned start,
       unsigned stop)
{
  return ((val) & MASK(start, stop));
}



INLINE_SIM_BITS\
(unsigned_word)
LSMASKED(unsigned_word val,
	 unsigned nr_bits)
{
  return MASKED(val,
		WITH_TARGET_WORD_BITSIZE - nr_bits,
		WITH_TARGET_WORD_BITSIZE - 1);
}



INLINE_SIM_BITS\
(unsigned_word)
EXTRACTED(unsigned_word val,
	  unsigned start,
	  unsigned stop)
{
  ASSERT(start <= stop);
#if (WITH_TARGET_WORD_BITSIZE == 64)
  return EXTRACTED64(val, start, stop);
#endif
#if (WITH_TARGET_WORD_BITSIZE == 32)
  if (stop < 32)
    return 0;
  else
    return ((val >> (63 - stop))
	    & MASK(start+(63-stop), 63));
#endif
}


INLINE_SIM_BITS\
(unsigned_word)
INSERTED(unsigned_word val,
	 unsigned start,
	 unsigned stop)
{
  ASSERT(start <= stop);
#if (WITH_TARGET_WORD_BITSIZE == 64)
  return INSERTED64(val, start, stop);
#endif
#if (WITH_TARGET_WORD_BITSIZE == 32)
  if (stop < 32)
    return 0;
  else
    return ((val & MASK(start+(63-stop), 63))
	    << (63 - stop));
#endif
}




#define N 16
#include "sim-n-bits.h"
#undef N

#define N 32
#include "sim-n-bits.h"
#undef N

#define N 64
#include "sim-n-bits.h"
#undef N

#endif /* _SIM_BITS_C_ */
