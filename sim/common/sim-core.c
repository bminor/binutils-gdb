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


#ifndef _SIM_CORE_C_
#define _SIM_CORE_C_

#include "engine.h"


INLINE_SIM_CORE\
(void)
core_init(engine *system)
{
  core *memory = &system->memory;
  core_maps map;
  for (map = 0;
       map < nr_core_maps;
       map++) {
    /* blow away old mappings */
    core_mapping *curr = memory->map[map].first;
    while (curr != NULL) {
      core_mapping *tbd = curr;
      curr = curr->next;
      if (tbd->free_buffer) {
	ASSERT(tbd->buffer != NULL);
	zfree(tbd->buffer);
      }
      zfree(tbd);
    }
    memory->map[map].first = NULL;
  }
}



STATIC_INLINE_SIM_CORE\
(core_mapping *)
new_core_mapping(engine *system,
		 attach_type attach,
		 int space,
		 unsigned_word addr,
		 unsigned nr_bytes,
		 device *device,
		 void *buffer,
		 int free_buffer)
{
  core_mapping *new_mapping = ZALLOC(core_mapping);
  /* common */
  new_mapping->level = attach;
  new_mapping->space = space;
  new_mapping->base = addr;
  new_mapping->nr_bytes = nr_bytes;
  new_mapping->bound = addr + (nr_bytes - 1);
  if (attach == attach_raw_memory) {
    new_mapping->buffer = buffer;
    new_mapping->free_buffer = free_buffer;
  }
  else if (attach >= attach_callback) {
    new_mapping->device = device;
  }
  else {
    engine_error(system, "new_core_mapping - internal error - unknown attach type %d\n",
		 attach);
  }
  return new_mapping;
}


STATIC_INLINE_SIM_CORE\
(void)
core_map_attach(engine *system,
		core_map *access_map,
		attach_type attach,
		int space,
		unsigned_word addr,
		unsigned nr_bytes, /* host limited */
		device *client, /*callback/default*/
		void *buffer, /*raw_memory*/
		int free_buffer) /*raw_memory*/
{
  /* find the insertion point for this additional mapping and then
     insert */
  core_mapping *next_mapping;
  core_mapping **last_mapping;

  ASSERT((attach >= attach_callback && client != NULL && buffer == NULL && !free_buffer)
	 || (attach == attach_raw_memory && client == NULL && buffer != NULL));

  /* actually do occasionally get a zero size map */
  if (nr_bytes == 0) {
#if (WITH_DEVICES)
    device_error(client, "called on core_map_attach with size zero");
#else
    engine_error(system, "called on core_map_attach with size zero");
#endif
  }

  /* find the insertion point (between last/next) */
  next_mapping = access_map->first;
  last_mapping = &access_map->first;
  while(next_mapping != NULL
	&& (next_mapping->level < attach
	    || (next_mapping->level == attach
		&& next_mapping->bound < addr))) {
    /* provided levels are the same */
    /* assert: next_mapping->base > all bases before next_mapping */
    /* assert: next_mapping->bound >= all bounds before next_mapping */
    last_mapping = &next_mapping->next;
    next_mapping = next_mapping->next;
  }

  /* check insertion point correct */
  ASSERT(next_mapping == NULL || next_mapping->level >= attach);
  if (next_mapping != NULL && next_mapping->level == attach
      && next_mapping->base < (addr + (nr_bytes - 1))) {
#if (WITH_DEVICES)
    device_error(client, "map overlap when attaching %d:0x%lx (%ld)",
		 space, (long)addr, (long)nr_bytes);
#else
    engine_error(system, "map overlap when attaching %d:0x%lx (%ld)",
		 space, (long)addr, (long)nr_bytes);
#endif
  }

  /* create/insert the new mapping */
  *last_mapping = new_core_mapping(system,
				   attach,
				   space, addr, nr_bytes,
				   client, buffer, free_buffer);
  (*last_mapping)->next = next_mapping;
}


INLINE_SIM_CORE\
(void)
core_attach(engine *system,
	    attach_type attach,
	    access_type access,
	    int space,
	    unsigned_word addr,
	    unsigned nr_bytes, /* host limited */
	    device *client,
	    void *optional_buffer)
{
  core *memory = &system->memory;
  core_maps map;
  void *buffer;
  int buffer_freed;
  if ((access & access_read_write_exec) == 0
      || (access & ~access_read_write_exec) != 0) {
#if (WITH_DEVICES)
    device_error(client, "invalid access for core attach");
#else
    engine_error(system, "invalid access for core attach");
#endif
  }
  /* verify the attach type */
  if (attach == attach_raw_memory) {
    if (optional_buffer == NULL) {
      buffer = zalloc(nr_bytes);
      buffer_freed = 0;
    }
    else {
      buffer = optional_buffer;
      buffer_freed = 1;
    }
  }
  else if (attach >= attach_callback) {
    buffer = NULL;
    buffer_freed = 1;
  }
  else {
#if (WITH_DEVICES)
    device_error(client, "core_attach - conflicting buffer and attach arguments");
#else
    engine_error(system, "core_attach - conflicting buffer and attach arguments");
#endif
    buffer = NULL;
    buffer_freed = 1;
  }
  /* attach the region to all applicable access maps */
  for (map = 0; 
       map < nr_core_maps;
       map++) {
    switch (map) {
    case core_read_map:
      if (access & access_read)
	core_map_attach(system, &memory->map[map],
			attach,
			space, addr, nr_bytes,
			client, buffer, !buffer_freed);
      buffer_freed ++;
      break;
    case core_write_map:
      if (access & access_write)
	core_map_attach(system, &memory->map[map],
			attach,
			space, addr, nr_bytes,
			client, buffer, !buffer_freed);
      buffer_freed ++;
      break;
    case core_execute_map:
      if (access & access_exec)
	core_map_attach(system, &memory->map[map],
			attach,
			space, addr, nr_bytes,
			client, buffer, !buffer_freed);
      buffer_freed ++;
      break;
    case nr_core_maps:
      engine_error(system, "core_attach - internal error - bad switch");
      break;
    }
  }
}


STATIC_INLINE_SIM_CORE\
(core_mapping *)
core_map_find_mapping(engine *system,
		      core_maps map,
		      unsigned_word addr,
		      unsigned nr_bytes,
		      int abort) /*either 0 or 1 - helps inline */
{
  core_mapping *mapping = system->memory.map[map].first;
  ASSERT((addr & (nr_bytes - 1)) == 0); /* must be aligned */
  ASSERT((addr + (nr_bytes - 1)) >= addr); /* must not wrap */
  while (mapping != NULL) {
    if (addr >= mapping->base
	&& (addr + (nr_bytes - 1)) <= mapping->bound)
      return mapping;
    mapping = mapping->next;
  }
  if (abort)
    engine_error(system, "access to unmaped address 0x%x (%d bytes)\n",
		 addr, nr_bytes);
  return NULL;
}


STATIC_INLINE_SIM_CORE\
(void *)
core_translate(core_mapping *mapping,
	       unsigned_word addr)
{
  return (void *)(((char *)mapping->buffer) + addr - mapping->base);
}


INLINE_SIM_CORE\
(unsigned)
core_map_read_buffer(engine *system,
		     core_maps map,
		     void *buffer,
		     unsigned_word addr,
		     unsigned len)
{
  unsigned count = 0;
  while (count < len) {
    unsigned_word raddr = addr + count;
    core_mapping *mapping =
      core_map_find_mapping(system, map,
			    raddr, 1,
			    0); /*dont-abort*/
    if (mapping == NULL)
      break;
#if (WITH_DEVICES)
    if (mapping->device != NULL) {
      int nr_bytes = len - count;
      if (raddr + nr_bytes - 1> mapping->bound)
	nr_bytes = mapping->bound - raddr + 1;
      if (device_io_read_buffer(mapping->device,
				(unsigned_1*)buffer + count,
				mapping->space,
				raddr,
				nr_bytes) != nr_bytes)
	break;
      count += nr_bytes;
    }
    else
#endif
      {
	((unsigned_1*)buffer)[count] =
	  *(unsigned_1*)core_translate(mapping, raddr);
	count += 1;
      }
  }
  return count;
}


INLINE_SIM_CORE\
(unsigned)
core_map_write_buffer(engine *system,
		      core_maps map,
		      const void *buffer,
		      unsigned_word addr,
		      unsigned len)
{
  unsigned count = 0;
  while (count < len) {
    unsigned_word raddr = addr + count;
    core_mapping *mapping = core_map_find_mapping(system, map,
						  raddr, 1,
						  0); /*dont-abort*/
    if (mapping == NULL)
      break;
#if (WITH_DEVICES)
    if (WITH_CALLBACK_MEMORY
	&& mapping->device != NULL) {
      int nr_bytes = len - count;
      if (raddr + nr_bytes - 1 > mapping->bound)
	nr_bytes = mapping->bound - raddr + 1;
      if (device_io_write_buffer(mapping->device,
				 (unsigned_1*)buffer + count,
				 mapping->space,
				 raddr,
				 nr_bytes) != nr_bytes)
	break;
      count += nr_bytes;
    }
    else
#endif
      {
	*(unsigned_1*)core_translate(mapping, raddr) =
	  ((unsigned_1*)buffer)[count];
	count += 1;
      }
  }
  return count;
}


/* define the read/write 1/2/4/8/word functions */

#define N 1
#include "sim-n-core.h"
#undef N

#define N 2
#include "sim-n-core.h"
#undef N

#define N 4
#include "sim-n-core.h"
#undef N

#define N 8
#include "sim-n-core.h"
#undef N

#define N word
#include "sim-n-core.h"
#undef N

#endif
