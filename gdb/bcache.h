/* Include file cached obstack implementation.
   Written by Fred Fish (fnf@cygnus.com)
   Copyright 1995 Free Software Foundation, Inc.

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#ifndef BCACHE_H
#define BCACHE_H 1

#define BCACHE_HASHLENGTH	12	/* Number of bits in hash value */
#define BCACHE_HASHSIZE	(1 << BCACHE_HASHLENGTH)
#define BCACHE_MAXLENGTH	128

struct hashlink {
  struct hashlink *next;
  char data[1];
};

struct bcache {
  struct obstack cache;
  struct hashlink **indextable[BCACHE_MAXLENGTH];
  int cache_hits;
  int cache_misses;
  int cache_bytes;
  int cache_savings;
  int bcache_overflows;
};

extern void *
bcache PARAMS ((void *bytes, int count, struct bcache *bcachep));

#endif /* BCACHE_H */
