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


#ifndef _ENDIAN_C_
#define _ENDIAN_C_

#ifndef STATIC_INLINE_ENDIAN
#define STATIC_INLINE_ENDIAN STATIC_INLINE
#endif


#include "config.h"
#include "ppc-config.h"
#include "words.h"
#include "ppc-endian.h"
#include "sim_callbacks.h"

#if !defined(SWAP_2) && (WITH_HOST_BYTE_ORDER == LITTLE_ENDIAN) && WITH_NTOH
#define SWAP_2(SET,RAW) SET htons (RAW)
#endif

#ifndef	SWAP_2
#define SWAP_2(SET,RAW) SET (((RAW) >> 8) | ((RAW) << 8))
#endif

#if !defined(SWAP_4) && (WITH_HOST_BYTE_ORDER == LITTLE_ENDIAN) && WITH_NTOH
#define SWAP_4(SET,RAW) SET htonl (RAW)
#endif

#ifndef SWAP_4
#define	SWAP_4(SET,RAW) SET (((RAW) << 24) | (((RAW) & 0xff00) << 8) | (((RAW) & 0xff0000) >> 8) | ((RAW) >> 24))
#endif

#ifndef SWAP_8
#define SWAP_8(SET,RAW)							\
  union { unsigned_8 dword; unsigned_4 words[2]; } in, out;		\
  in.dword = RAW;							\
  SWAP_4 (out.words[0] =, in.words[1]);					\
  SWAP_4 (out.words[1] =, in.words[0]);					\
  SET out.dword;
#endif

#ifndef ENDIAN_N
#define ENDIAN_N(NAME,BYTE_SIZE) \
INLINE unsigned_##BYTE_SIZE \
endian_##NAME##_##BYTE_SIZE(unsigned_##BYTE_SIZE raw_in) \
{ \
  if (CURRENT_TARGET_BYTE_ORDER == CURRENT_HOST_BYTE_ORDER) { \
    return raw_in; \
  } \
  else { \
    SWAP_##BYTE_SIZE(return,raw_in); \
  } \
}
#endif

ENDIAN_N(h2t, 2)
ENDIAN_N(h2t, 4)
ENDIAN_N(h2t, 8)
ENDIAN_N(t2h, 2)
ENDIAN_N(t2h, 4)
ENDIAN_N(t2h, 8)

#endif /* _ENDIAN_C_ */
