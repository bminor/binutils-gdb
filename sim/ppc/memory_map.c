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


#ifndef _MEMORY_MAP_C_
#define _MEMORY_MAP_C_

#ifndef STATIC_INLINE_MEMORY_MAP
#define STATIC_INLINE_MEMORY_MAP STATIC_INLINE
#endif


#include "basics.h"
#include "device_tree.h"
#include "memory_map.h"
#include "interrupts.h"


typedef struct _memory_mapping memory_mapping;
struct _memory_mapping {
  /* ram map */
  void *buffer;
  /* device map */
  device_node *device;
  device_reader_callback *reader;
  device_writer_callback *writer;
  /* common */
  unsigned_word base;
  unsigned_word bound;
  unsigned_word size;
  struct _memory_mapping *next;
};

struct _memory_map {
  memory_mapping *first;
};

INLINE_MEMORY_MAP memory_map *
new_memory_map(void)
{
  memory_map *new_map;
  new_map = ZALLOC(memory_map);
  return new_map;
}

STATIC_INLINE_MEMORY_MAP void
memory_map_add_memory(memory_map *map,
		      device_node *device,
		      device_reader_callback *reader,
		      device_writer_callback *writer,
		      void *buffer,
		      unsigned_word base,
		      unsigned size)
{
  memory_mapping *new_mapping;
  memory_mapping *next_mapping;
  memory_mapping **last_mapping;

  /* actually do occasionally get a zero size map */
  if (size == 0)
    return;

  new_mapping = ZALLOC(memory_mapping);

  /* ram */
  new_mapping->buffer = buffer;
  /* devices */
  new_mapping->device = device;
  new_mapping->reader = reader;
  new_mapping->writer = writer;
  /* common */
  new_mapping->base = base;
  new_mapping->size = size;
  new_mapping->bound = base + size;

  /* find the insertion point (between last/next) */
  next_mapping = map->first;
  last_mapping = &map->first;
  while(next_mapping != NULL && next_mapping->bound <= new_mapping->base) {
    /* assert: new_mapping->base > all bases before next_mapping */
    /* assert: new_mapping->bound >= all bounds before next_mapping */
    last_mapping = &next_mapping->next;
    next_mapping = next_mapping->next;
  }

  /* check insertion point correct */
  if (next_mapping != NULL && next_mapping->base < new_mapping->bound) {
    error("memory_map_add_callback_memory() internal error - map overlap\n");
  }

  /* insert the new mapping */
  *last_mapping = new_mapping;
  new_mapping->next = next_mapping;

}


INLINE_MEMORY_MAP void
memory_map_add_callback_memory(memory_map *map,
			       device_node *device,
			       device_reader_callback *reader,
			       device_writer_callback *writer,
			       unsigned_word base,
			       unsigned size)
{
  memory_map_add_memory(map, device, reader, writer, NULL, base, size);
}

INLINE_MEMORY_MAP void
memory_map_add_raw_memory(memory_map *map,
			  void *buffer,
			  unsigned_word base,
			  unsigned size)
{
  memory_map_add_memory(map, NULL, NULL, NULL, buffer, base, size);
}




STATIC_INLINE_MEMORY_MAP memory_mapping *
memory_map_find_mapping(memory_map *map,
			unsigned_word addr,
			unsigned nr_bytes,
			int abort,
			cpu *processor,
			unsigned_word cia)
{
  memory_mapping *mapping = map->first;
  ASSERT((addr & (nr_bytes-1)) == 0);
  while (1) {
    if (addr >= mapping->base
	&& (addr + nr_bytes) <= mapping->bound)
      break;
    mapping = mapping->next;
    if (mapping == NULL) {
      if (abort) {
	switch (CURRENT_ENVIRONMENT) {
	case VIRTUAL_ENVIRONMENT:
	  data_storage_interrupt(processor,
				 cia,
				 addr,
				 vea_storage_interrupt,
				 0/* doesnt matter */);
	  break;
	default:
	  error("memory_map_find_mapping() - %s%x%s%s%s%s%s",
		"access to undefined address 0x", addr, "\n",
		"this code should be passing back up the device tree an\n",
		"abort event (with processor attached).  Somewhere in\n",
		"the device tree this would be caught and either halt,\n",
		"interrupt, or reset the processor\n");
	}
	return NULL;
      }
      else
	return NULL;
    }
  }
  return mapping;
}


STATIC_INLINE_MEMORY_MAP void *
memory_map_translate(memory_mapping *mapping,
		     unsigned_word addr)
{
  return mapping->buffer + addr - mapping->base;
}


INLINE_MEMORY_MAP unsigned
memory_map_read_buffer(memory_map *map,
		       void *buffer,
		       unsigned_word addr,
		       unsigned len,
		       transfer_mode mode)
{
  unsigned count;
  unsigned_1 byte;
  for (count = 0; count < len; count++) {
    unsigned pos = 0;
    unsigned_word raddr = addr + count;
    memory_mapping *mapping =
      memory_map_find_mapping(map, raddr, 1,
			      0/*abort*/,
			      0, 0/*processor, cia*/);
    if (mapping == NULL)
      break;
    if (mode == raw_transfer ||
	CURRENT_TARGET_BYTE_ORDER == CURRENT_HOST_BYTE_ORDER)
      pos = count;
    else if (mode == cooked_transfer)
      pos = len-count-1;
    else
      error("memory_map_read_buffer() - transfer mode unknown\n");
    if (mapping->reader != NULL)
      /* hope it doesn't barf */
      byte = mapping->reader(mapping->device,
			     raddr - mapping->base,
			     1,
			     0, 0/*processor, cia*/);
    else
      byte = *(unsigned_1*)memory_map_translate(mapping,
						raddr);
    ((unsigned_1*)buffer)[pos] = T2H_1(byte);
  }
  return count;
}


INLINE_MEMORY_MAP unsigned
memory_map_write_buffer(memory_map *map,
			const void *buffer,
			unsigned_word addr,
			unsigned len,
			transfer_mode mode)
{
  unsigned count;
  unsigned_1 byte;
  for (count = 0; count < len; count++) {
    unsigned pos = 0;
    unsigned_word raddr = addr + count;
    memory_mapping *mapping =
      memory_map_find_mapping(map, raddr, 1,
			      0/*abort*/,
			      0, 0/*processor, cia*/);
    if (mapping == NULL)
      break;
    if (mode == raw_transfer || 
	CURRENT_TARGET_BYTE_ORDER == CURRENT_HOST_BYTE_ORDER)
      pos = count;
    else if (mode == cooked_transfer)
      pos = len-count-1;
    else
      error("memory_map_write_buffer() - transfer mode unknown\n");
    byte = H2T_1(((unsigned_1*)buffer)[pos]);
    if (mapping->writer != NULL)
      /* hope it doesn't barf */
      mapping->writer(mapping->device,
		      raddr - mapping->base,
		      1,
		      byte,
		      0, 0/*processor, cia*/);
    else
      *(unsigned_1*)memory_map_translate(mapping, raddr) = byte;
  }
  return count;
}


INLINE_MEMORY_MAP unsigned
memory_map_zero(memory_map *map,
		unsigned_word addr,
		unsigned len)
{
  unsigned pos;
  for (pos = 0; pos < len; pos++) {
    unsigned_word raddr = addr + pos;
    memory_mapping *mapping =
      memory_map_find_mapping(map, raddr, 1,
			      0/*abort*/,
			      0, 0/*processor, cia*/);
    if (mapping == NULL)
      break;
    if (mapping->writer != NULL)
      mapping->writer(mapping->device,
		      raddr - mapping->base,
		      1,
		      0,
		      0, 0/*processor, cia*/);
    else
      *(unsigned_1*)memory_map_translate(mapping, raddr) = 0;
  }
  return pos;
}


#define DEFINE_MEMORY_MAP_READ_N(N) \
INLINE_MEMORY_MAP unsigned_##N \
memory_map_read_##N(memory_map *map, \
		    unsigned_word addr, \
		    cpu *processor, \
		    unsigned_word cia) \
{ \
  memory_mapping *mapping = memory_map_find_mapping(map, addr, \
						    sizeof(unsigned_##N), \
						    1, \
						    processor, \
						    cia); \
  if (WITH_CALLBACK_MEMORY && mapping->reader != NULL) \
    return ((unsigned_##N) \
	    mapping->reader(mapping->device, \
			    addr - mapping->base, \
			    sizeof(unsigned_##N), \
			    processor, \
			    cia)); \
  else \
    return T2H_##N(*(unsigned_##N*)memory_map_translate(mapping, addr)); \
}

DEFINE_MEMORY_MAP_READ_N(1)
DEFINE_MEMORY_MAP_READ_N(2)
DEFINE_MEMORY_MAP_READ_N(4)
DEFINE_MEMORY_MAP_READ_N(8)
DEFINE_MEMORY_MAP_READ_N(word)

#define DEFINE_MEMORY_MAP_WRITE_N(N) \
INLINE_MEMORY_MAP void \
memory_map_write_##N(memory_map *map, \
		     unsigned_word addr, \
		     unsigned_##N val, \
		     cpu *processor, \
		     unsigned_word cia) \
{ \
  memory_mapping *mapping = memory_map_find_mapping(map, addr, \
						    sizeof(unsigned_##N), \
						    1, \
						    processor, \
						    cia); \
  if (WITH_CALLBACK_MEMORY && mapping->writer != NULL) \
    mapping->writer(mapping->device, \
		    addr - mapping->base, \
		    sizeof(unsigned_##N), \
		    val, \
		    processor, \
		    cia); \
  else \
    *(unsigned_##N*)memory_map_translate(mapping, addr) = H2T_##N(val); \
}

DEFINE_MEMORY_MAP_WRITE_N(1)
DEFINE_MEMORY_MAP_WRITE_N(2)
DEFINE_MEMORY_MAP_WRITE_N(4)
DEFINE_MEMORY_MAP_WRITE_N(8)
DEFINE_MEMORY_MAP_WRITE_N(word)

#endif /* _MEMORY_MAP_C_ */
