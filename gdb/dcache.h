/* Declarations for caching.  Typically used by remote back ends for
   caching remote memory.

   Copyright 1992, 1993 Free Software Foundation, Inc.

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

#ifndef DCACHE_H
#define DCACHE_H

#define LINE_SIZE_POWER (4)
/* eg 1<<3 == 8 */
#define LINE_SIZE (1 << LINE_SIZE_POWER)
/* Number of cache blocks */
#define DCACHE_SIZE (64)

struct dcache_block
{
  struct dcache_block *next, *last;
  unsigned int addr; /* Address for which data is recorded.  */
  int data[LINE_SIZE / sizeof (int)];
};

typedef int (*memxferfunc) PARAMS((CORE_ADDR memaddr,
			     unsigned char *myaddr,
			     int len));

typedef struct {
  /* Function to actually read the target memory. */
  memxferfunc read_memory;

  /* Function to actually write the target memory */
  memxferfunc write_memory;

  /* free list */
  struct dcache_block dcache_free;

  /* in use list */
  struct dcache_block dcache_valid;

  /* The cache itself. */
  struct dcache_block *the_cache;

} DCACHE;

int dcache_fetch PARAMS((DCACHE *dcache, CORE_ADDR addr));
void dcache_flush PARAMS((DCACHE *dcache));
DCACHE *dcache_init PARAMS((memxferfunc reading, memxferfunc writing));

/* Write the word at ADDR both in the data cache and in the remote machine.  */
void dcache_poke PARAMS((DCACHE *dcache, CORE_ADDR addr, int data));

#endif /* DCACHE_H */
