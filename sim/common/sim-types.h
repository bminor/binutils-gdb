/* This file is part of psim (model of the PowerPC(tm) architecture)

   Copyright (C) 1994-1997, Andrew Cagney <cagney@highland.com.au>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License
   as published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.
 
   This library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
 
   You should have received a copy of the GNU Library General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 
   --

   PowerPC is a trademark of International Business Machines Corporation. */


/* Basic type sizes for the PowerPC */

#ifndef _SIM_TYPES_H_
#define _SIM_TYPES_H_




/* INTEGER QUANTITIES:

   TYPES:

     natural*	sign determined by host
     signed*    signed type of the given size
     unsigned*  The corresponding insigned type

   SIZES

     *NN	Size based on the number of bits
     *_NN       Size according to the number of bytes
     *_word     Size based on the target architecture's word
     		word size (32/64 bits)
     *_cell     Size based on the target architecture's
     		IEEE 1275 cell size (almost always 32 bits)

*/


/* bit based */
typedef char natural8;
typedef short natural16;
typedef long natural32;

typedef signed char signed8;
typedef signed short signed16;
typedef signed long signed32;

typedef unsigned char unsigned8;
typedef unsigned short unsigned16;
typedef unsigned long unsigned32;

#if defined __GNUC__ || defined _MSC_VER
#ifdef __GNUC__

typedef long long natural64;
typedef signed long long signed64;
typedef unsigned long long unsigned64;

#define UNSIGNED64(X) (X##ULL)
#define SIGNED64(X) (X##LL)

#define UNSIGNED32(X) (X##UL)
#define SIGNED32(X) (X##L)

#else	/* _MSC_VER */

typedef __int64 natural64;
typedef signed __int64 signed64;
typedef unsigned __int64 unsigned64;

#define UNSIGNED64(X) (X##ui64)
#define SIGNED64(X) (X##i64)

#define SIGNED32(X) (X##ui32)
#define UNSIGNED32(X) (X##i32)

#endif /* _MSC_VER */

typedef struct { unsigned64 a[2]; } unsigned128;
typedef struct { signed64 a[2]; } signed128;

#else /* Not GNUC or _MSC_VER */
/* Not supported */
#endif

/* byte based */
typedef natural8 natural_1;
typedef natural16 natural_2;
typedef natural32 natural_4;
typedef natural64 natural_8;
/* typedef natural64 natural_8; */

typedef signed8 signed_1;
typedef signed16 signed_2;
typedef signed32 signed_4;
typedef signed64 signed_8;
typedef signed128 signed_16;

typedef unsigned8 unsigned_1;
typedef unsigned16 unsigned_2;
typedef unsigned32 unsigned_4;
typedef unsigned64 unsigned_8;
typedef unsigned128 unsigned_16;


/* for general work, the following are defined */
/* unsigned: >= 32 bits */
/* signed:   >= 32 bits */
/* long:     >= 32 bits, sign undefined */
/* int:      small indicator */

/* target architecture based */
#if (WITH_TARGET_WORD_BITSIZE == 64)
typedef natural64 natural_word;
typedef unsigned64 unsigned_word;
typedef signed64 signed_word;
#endif
#if (WITH_TARGET_WORD_BITSIZE == 32)
typedef natural32 natural_word;
typedef unsigned32 unsigned_word;
typedef signed32 signed_word;
#endif


/* Other instructions */
#if (WITH_TARGET_ADDRESS_BITSIZE == 64)
typedef unsigned64 address_word;
#endif
#if (WITH_TARGET_ADDRESS_BITSIZE == 32)
typedef unsigned32 address_word;
#endif

/* IEEE 1275 cell size */
#if (WITH_TARGET_CELL_BITSIZE == 64)
typedef natural64 natural_cell;
typedef unsigned64 unsigned_cell;
typedef signed64 signed_cell;
#endif
#if (WITH_TARGET_CELL_BITSIZE == 32)
typedef natural32 natural_cell;
typedef unsigned32 unsigned_cell;
typedef signed32 signed_cell;
#endif

#endif /* _SIM_TYPES_H_ */
