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


#ifndef _MEMORY_MAP_H_
#define _MEMORY_MAP_H_

#ifndef INLINE_MEMORY_MAP
#define INLINE_MEMORY_MAP
#endif

/* basic types */

typedef struct _memory_map memory_map;


/* constructor */

INLINE_MEMORY_MAP memory_map *new_memory_map
(void);


/* operators to add memory to a memory map

   callback-memory:

   includes a callback routine that is called upon for the data.
   Useful when modeling memory mapped devices.

   raw-memory:

   normal base and bound memory map used to model ram or mapped memory
   pages */

INLINE_MEMORY_MAP void memory_map_add_callback_memory
(memory_map *map,
 device_node *device,
 device_reader_callback *reader,
 device_writer_callback *writer,
 unsigned_word base,
 unsigned size); /* host limited */

INLINE_MEMORY_MAP void memory_map_add_raw_memory
(memory_map *map,
 void *buffer,
 unsigned_word base,
 unsigned size/*host limited*/);


/* Variable sized read/write/zero:

   Transfer (zero) a variable size block of data between the host and
   target (possibly byte swapping it).  Should any problems occure,
   the number of bytes actually transfered is returned. */

INLINE_MEMORY_MAP unsigned memory_map_read_buffer
(memory_map *map,
 void *buffer,
 unsigned_word addr,
 unsigned len,
 transfer_mode mode);

INLINE_MEMORY_MAP unsigned memory_map_write_buffer
(memory_map *map,
 const void *buffer,
 unsigned_word addr,
 unsigned len,
 transfer_mode mode);

INLINE_MEMORY_MAP unsigned memory_map_zero
(memory_map *map,
 unsigned_word addr,
 unsigned len);


/* Fixed sized read/write:

   Transfer a fixed amout of memory between the host and target.  The
   memory always being translated and the operation always aborting
   should a problem occure */

#define DECLARE_MEMORY_MAP_WRITE_N(N) \
INLINE_MEMORY_MAP void memory_map_write_##N \
(memory_map *map, \
 unsigned_word addr, \
 unsigned_##N val, \
 cpu *processor, \
 unsigned_word cia);

DECLARE_MEMORY_MAP_WRITE_N(1)
DECLARE_MEMORY_MAP_WRITE_N(2)
DECLARE_MEMORY_MAP_WRITE_N(4)
DECLARE_MEMORY_MAP_WRITE_N(8)
DECLARE_MEMORY_MAP_WRITE_N(word)

#define DECLARE_MEMORY_MAP_READ_N(N) \
INLINE_MEMORY_MAP unsigned_##N memory_map_read_##N \
(memory_map *map, \
 unsigned_word addr, \
 cpu *processor, \
 unsigned_word cia);

DECLARE_MEMORY_MAP_READ_N(1)
DECLARE_MEMORY_MAP_READ_N(2)
DECLARE_MEMORY_MAP_READ_N(4)
DECLARE_MEMORY_MAP_READ_N(8)
DECLARE_MEMORY_MAP_READ_N(word)

#endif
