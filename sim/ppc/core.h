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


#ifndef _CORE_H_
#define _CORE_H_

#ifndef INLINE_CORE
#define INLINE_CORE
#endif

/* the base type */

typedef struct _core core;


/* create the hardware's core (memory and devices) from the device
   tree */
INLINE_CORE core *core_create
(device_node *root,
 int trace);


/* given a created core object, (re)initialize it from the
   information provided in it's associated device tree */

INLINE_CORE void core_init
(core *memory);


/* from this core extract out the three different types of memory -
   executable, readable, writeable */

INLINE_CORE memory_map *core_readable
(core *memory);

INLINE_CORE memory_map *core_writeable
(core *memory);

INLINE_CORE memory_map *core_executable
(core *memory);


/* operators to grow memory on the fly */

INLINE_CORE void core_add_raw_memory
(core *memory,
 void *buffer,
 unsigned_word base,
 unsigned size,
 device_access access);

INLINE_CORE void core_add_callback_memory
(core *memory,
 device_node *device,
 device_reader_callback *reader,
 device_writer_callback *writer,
 unsigned_word base,
 unsigned size,
 device_access access);


/* In the VEA model, memory grow's after it is created.  Operators
   below grow memory as required.

   FIXME - should this be inside of vm? */

INLINE_CORE unsigned_word core_data_upper_bound
(core *memory);

INLINE_CORE unsigned_word core_stack_lower_bound
(core *memory);

INLINE_CORE unsigned_word core_stack_size
(core *memory);

INLINE_CORE void core_add_data
(core *memory,
 unsigned_word incr);

INLINE_CORE void core_add_stack
(core *memory,
 unsigned_word incr);


#endif /* _CORE_ */
