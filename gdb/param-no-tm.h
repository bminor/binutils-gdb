/* Copyright (C) 1990 Free Software Foundation, Inc.

This file is part of GDB.

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
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#if !defined (PARAM_H)
#define PARAM_H 1

/*  DO NOT #include "tm.h" -- a particular tm file has been inc'd by caller  */

#include "xm.h"

/* TARGET_BYTE_ORDER and HOST_BYTE_ORDER should be defined to one of these.  */
#if !defined (BIG_ENDIAN)
#define BIG_ENDIAN 4321
#endif

#if !defined (LITTLE_ENDIAN)
#define LITTLE_ENDIAN 1234
#endif

/* The bit byte-order has to do just with numbering of bits in
   debugging symbols and such.  Conceptually, it's quite separate
   from byte/word byte order.  */

#if !defined (BITS_BIG_ENDIAN)
#if TARGET_BYTE_ORDER == BIG_ENDIAN
#define BITS_BIG_ENDIAN 1
#endif /* Big endian.  */

#if TARGET_BYTE_ORDER == LITTLE_ENDIAN
#define BITS_BIG_ENDIAN 0
#endif /* Little endian.  */
#endif /* BITS_BIG_ENDIAN not defined.  */

/* Swap LEN bytes at BUFFER between target and host byte-order.  */
#if TARGET_BYTE_ORDER == HOST_BYTE_ORDER
#define SWAP_TARGET_AND_HOST(buffer,len)
#else /* Target and host byte order differ.  */
#define SWAP_TARGET_AND_HOST(buffer,len) \
  {	       	       	       	       	       	       	       	       	 \
    char tmp;								 \
    char *p = (char *)(buffer);						 \
    char *q = ((char *)(buffer)) + len - 1;		   		 \
    for (; p < q; p++, q--)				 		 \
      {									 \
        tmp = *q;							 \
        *q = *p;							 \
        *p = tmp;							 \
      }									 \
  }
#endif /* Target and host byte order differ.  */

/* On some machines there are bits in addresses which are not really
   part of the address, but are used by the kernel, the hardware, etc.
   for special purposes.  ADDR_BITS_REMOVE takes out any such bits
   so we get a "real" address such as one would find in a symbol
   table.  ADDR_BITS_SET sets those bits the way the system wants
   them.  */
#if !defined (ADDR_BITS_REMOVE)
#define ADDR_BITS_REMOVE(addr) (addr)
#define ADDR_BITS_SET(addr) (addr)
#endif /* No ADDR_BITS_REMOVE.  */

#if !defined (SYS_SIGLIST_MISSING)
#define SYS_SIGLIST_MISSING defined (USG)
#endif /* No SYS_SIGLIST_MISSING */

#endif /* param.h not already included.  */
