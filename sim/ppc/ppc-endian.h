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


#ifndef _ENDIAN_H_
#define _ENDIAN_H_

/* C byte conversion functions */

extern unsigned_1 endian_h2t_1(unsigned_1 x);
extern unsigned_2 endian_h2t_2(unsigned_2 x);
extern unsigned_4 endian_h2t_4(unsigned_4 x);
extern unsigned_8 endian_h2t_8(unsigned_8 x);

extern unsigned_1 endian_t2h_1(unsigned_1 x);
extern unsigned_2 endian_t2h_2(unsigned_2 x);
extern unsigned_4 endian_t2h_4(unsigned_4 x);
extern unsigned_8 endian_t2h_8(unsigned_8 x);

/* Host dependant:

   The CPP below defines information about the compilation host.  In
   particular it defines the macro's:

   	WITH_HOST_BYTE_ORDER	The byte order of the host. Could
				be any of LITTLE_ENDIAN, BIG_ENDIAN
				or 0 (unknown).  Those macro's also
				need to be defined.

	WITH_NTOH		Network byte order macros defined.
				Possible value is 32 or (maybe one
				day 64 because some 64bit network
				byte order macro is defined.
 */


/* NetBSD:

   NetBSD is easy, everything you could ever want is in a header file
   (well almost :-) */

#if defined(__NetBSD__)
# include <machine/endian.h>
# define WITH_NTOH 32 /* what about alpha? */
# if (WITH_HOST_BYTE_ORDER == 0)
#  undef WITH_HOST_BYTE_ORDER
#  define WITH_HOST_BYTE_ORDER BYTE_ORDER
# endif
# if (BYTE_ORDER != WITH_HOST_BYTE_ORDER)
#  error "host endian incorrectly configured, check config.h"
# endif
#endif

/* Linux is similarly easy.  */

#if defined(__linux__)
# include <endian.h>
# include <asm/byteorder.h>
# if defined(__LITTLE_ENDIAN) && !defined(LITTLE_ENDIAN)
#  define LITTLE_ENDIAN __LITTLE_ENDIAN
# endif
# if defined(__BIG_ENDIAN) && !defined(BIG_ENDIAN)
#  define BIG_ENDIAN __BIG_ENDIAN
# endif
# if defined(__BYTE_ORDER) && !defined(BYTE_ORDER)
#  define BYTE_ORDER __BYTE_ORDER
# endif
# if !defined(__alpha__)
#  define WITH_NTOH 32 /* what about alpha? */
# endif
# if (WITH_HOST_BYTE_ORDER == 0)
#  undef WITH_HOST_BYTE_ORDER
#  define WITH_HOST_BYTE_ORDER BYTE_ORDER
# endif
# if (BYTE_ORDER != WITH_HOST_BYTE_ORDER)
#  error "host endian incorrectly configured, check config.h"
# endif
#endif

/* INSERT HERE - hosts that have available LITTLE_ENDIAN and
   BIG_ENDIAN macro's */


/* Some hosts don't define LITTLE_ENDIAN or BIG_ENDIAN, help them out */

#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN 1234
#endif
#ifndef BIG_ENDIAN
#define BIG_ENDIAN 4321
#endif


/* SunOS on SPARC:

   Big endian last time I looked */

#if defined(sparc) || defined(__sparc__)
# if (WITH_HOST_BYTE_ORDER == 0)
#  undef WITH_HOST_BYTE_ORDER
#  define WITH_HOST_BYTE_ORDER BIG_ENDIAN
# endif
# if (WITH_HOST_BYTE_ORDER != BIG_ENDIAN)
#  error "sun was big endian last time I looked ..."
# endif
#endif


/* Random x86

   Little endian last time I looked */

#if defined(i386) || defined(i486) || defined(i586) || defined(__i386__) || defined(__i486__) || defined(__i586__)
# if (WITH_HOST_BYTE_ORDER == 0)
#  undef WITH_HOST_BYTE_ORDER
#  define WITH_HOST_BYTE_ORDER LITTLE_ENDIAN
# endif
# if (WITH_HOST_BYTE_ORDER != LITTLE_ENDIAN)
#  error "x86 was little endian last time I looked ..."
# endif
#endif


/* INSERT HERE - additional hosts that do not have LITTLE_ENDIAN and
   BIG_ENDIAN definitions available.  */


/* SWAPPING:

   According to the following table:

                TARG BE   TARG LE   TARG ??
       HOST BE    ok        s/w      s/w
       HOST LE   htohl      ok      ok|ntohl
       HOST ??   ntohl      s/w      s/w

    define host <-> target byte order conversion macro's */


/* IN PLACE:

   These macro's given a variable argument swap its value in place if
   so required */

#define H2T(VARIABLE) \
do { \
  switch (sizeof(VARIABLE)) { \
  case 1: VARIABLE = H2T_1(VARIABLE); break; \
  case 2: VARIABLE = H2T_2(VARIABLE); break; \
  case 4: VARIABLE = H2T_4(VARIABLE); break; \
  case 8: VARIABLE = H2T_8(VARIABLE); break; \
  } \
} while (0)

#define T2H(VARIABLE) \
do { \
  switch (sizeof(VARIABLE)) { \
  case 1: VARIABLE = T2H_1(VARIABLE); break; \
  case 2: VARIABLE = T2H_2(VARIABLE); break; \
  case 4: VARIABLE = T2H_4(VARIABLE); break; \
  case 8: VARIABLE = T2H_8(VARIABLE); break; \
  } \
} while (0)


/* TARGET WORD:

   Byte swap a quantity the size of the targets word */

#if (WITH_TARGET_WORD_BITSIZE == 64)
#define H2T_word(X) H2T_8(X)
#define T2H_word(X) T2H_8(X)
#endif
#if (WITH_TARGET_WORD_BITSIZE == 32)
#define H2T_word(X) H2T_4(X)
#define T2H_word(X) T2H_4(X)
#endif


/* FUNCTIONS:

   Returns the value swapped according to the host/target byte order */

/* no need to swap */
#if (WITH_HOST_BYTE_ORDER \
     && WITH_TARGET_BYTE_ORDER \
     && WITH_HOST_BYTE_ORDER == WITH_TARGET_BYTE_ORDER )
#define H2T_1(X) (X)
#define H2T_2(X) (X)
#define H2T_4(X) (X)
#define H2T_8(X) (X)
#define T2H_1(X) (X)
#define T2H_2(X) (X)
#define T2H_4(X) (X)
#define T2H_8(X) (X)
#endif

/* have ntoh and big endian target */
#if (WITH_TARGET_BYTE_ORDER == BIG_ENDIAN \
     && WITH_HOST_BYTE_ORDER != BIG_ENDIAN \
     && WITH_NTOH)
#define H2T_8(X) endian_h2t_8(X)
#define H2T_4(X) htonl(X)
#define H2T_2(X) htons(X)
#define H2T_1(X) (X)
#define T2H_8(X) endian_t2h_8(X)
#define T2H_4(X) htonl(X)
#define T2H_2(X) htons(X)
#define T2H_1(X) (X)
#endif

#if (defined (__i486__) || defined (__i586__)) && defined(__GNUC__) && WITH_BSWAP && WITH_NTOH
#undef  htonl
#undef  ntohl
#define htonl(IN) __extension__ ({ int _out; __asm__ ("bswap %0" : "=r" (_out) : "0" (IN)); _out; })
#define ntohl(IN) __extension__ ({ int _out; __asm__ ("bswap %0" : "=r" (_out) : "0" (IN)); _out; })
#endif

/* have ntoh, little host and unknown target */
#if (WITH_HOST_BYTE_ORDER == LITTLE_ENDIAN \
     && WITH_TARGET_BYTE_ORDER == 0 \
     && WITH_NTOH)
#define H2T_8(X) (CURRENT_TARGET_BYTE_ORDER == CURRENT_HOST_BYTE_ORDER ? (X) : endian_h2t_8(X))
#define H2T_4(X) (CURRENT_TARGET_BYTE_ORDER == CURRENT_HOST_BYTE_ORDER ? (X) : htonl(X))
#define H2T_2(X) (CURRENT_TARGET_BYTE_ORDER == CURRENT_HOST_BYTE_ORDER ? (X) : htons(X))
#define H2T_1(X) (CURRENT_TARGET_BYTE_ORDER == CURRENT_HOST_BYTE_ORDER ? (X) : (X))
#define T2H_8(X) (CURRENT_TARGET_BYTE_ORDER == CURRENT_HOST_BYTE_ORDER ? (X) : endian_t2h_8(X))
#define T2H_4(X) (CURRENT_TARGET_BYTE_ORDER == CURRENT_HOST_BYTE_ORDER ? (X) : htonl(X))
#define T2H_2(X) (CURRENT_TARGET_BYTE_ORDER == CURRENT_HOST_BYTE_ORDER ? (X) : htons(X))
#define T2H_1(X) (CURRENT_TARGET_BYTE_ORDER == CURRENT_HOST_BYTE_ORDER ? (X) : (X))
#endif

/* if all else fails use software */
#ifndef H2T_1
#define H2T_1(X) (X)
#define H2T_2(X) endian_h2t_2(X)
#define H2T_4(X) endian_h2t_4(X)
#define H2T_8(X) endian_h2t_8(X)
#define T2H_1(X) (X)
#define T2H_2(X) endian_t2h_2(X)
#define T2H_4(X) endian_t2h_4(X)
#define T2H_8(X) endian_t2h_8(X)
#endif

#endif
