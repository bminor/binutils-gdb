/*  This file is part of the program psim.

    Copyright (C) 1994-1996, Andrew Cagney <cagney@highland.com.au>

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


#ifndef _HW_MEMORY_C_
#define _HW_MEMORY_C_

#ifndef STATIC_INLINE_HW_MEMORY
#define STATIC_INLINE_HW_MEMORY STATIC_INLINE
#endif

#include "device_table.h"

/* DEVICE

   memory - description of system memory

   DESCRIPTION

   This device describes the size and location of the banks of
   physical memory within the simulation.

   In addition, this device supports the "claim" and "release" methods
   that can be used by OpenBoot client programs to manage the
   allocation of physical memory.

   PROPERTIES

   reg = { <address> <size> } (required)

   Each pair specify one bank of memory.

   available = { <address> <size> } (automatic)

   Each pair specifies a block of memory that is currently unallocated.  

   */

typedef struct _memory_reg_spec {
  unsigned32 base;
  unsigned32 size;
} memory_reg_spec;

typedef struct _hw_memory_chunk hw_memory_chunk;
struct _hw_memory_chunk {
  unsigned_word address;
  unsigned_word size;
  unsigned_word alloc_address;
  unsigned_word alloc_size;
  int available;
  hw_memory_chunk *next;
};

typedef struct _hw_memory_device {
  hw_memory_chunk *heap;
} hw_memory_device;


static void *
hw_memory_create(const char *name,
		 const device_unit *unit_address,
		 const char *args)
{
  hw_memory_device *hw_memory = ZALLOC(hw_memory_device);
  return hw_memory;
}


static void
hw_memory_set_available(device *me,
			hw_memory_device *hw_memory)
{
  hw_memory_chunk *chunk = NULL;
  memory_reg_spec *available = NULL;
  int nr_available = 0;
  int curr = 0;
  int sizeof_available = 0;
  /* determine the nr of available chunks */
  chunk = hw_memory->heap;
  nr_available = 0;
  while (chunk != NULL) {
    if (chunk->available)
      nr_available += 1;
    chunk = chunk->next;
  }
  /* now create the available struct */
  ASSERT(nr_available > 0);
  sizeof_available = sizeof(memory_reg_spec) * nr_available;
  available = zalloc(sizeof_available);
  chunk = hw_memory->heap;
  curr = 0;
  while (chunk != NULL) {
    if (chunk->available) {
      available[curr].base = H2BE_4(chunk->address);
      available[curr].size = H2BE_4(chunk->size);
      curr += 1;
    }
    chunk = chunk->next;
  }
  /* update */
  device_set_array_property(me, "available", available, sizeof_available);
  zfree(available);
}


static void
hw_memory_init_address(device *me)
{
  hw_memory_device *hw_memory = (hw_memory_device*)device_data(me);
  const device_property *reg = device_find_array_property(me, "reg");
  const memory_reg_spec *spec = reg->array;
  int nr_entries = reg->sizeof_array / sizeof(*spec);

  /* sanity check reg property */
  if ((reg->sizeof_array % sizeof(*spec)) != 0)
    error("devices/%s reg property of incorrect size\n", device_name(me));

  /* free up any previous structures */
  {
    hw_memory_chunk *curr_chunk = hw_memory->heap;
    hw_memory->heap = NULL;
    while (curr_chunk != NULL) {
      hw_memory_chunk *dead_chunk = curr_chunk;
      curr_chunk = dead_chunk->next;
      dead_chunk->next = NULL;
      zfree(dead_chunk);
    }
  }

  /* count/allocate memory entries */
  {
    hw_memory_chunk **curr_chunk = &hw_memory->heap;
    while (nr_entries > 0) {
      hw_memory_chunk *new_chunk = ZALLOC(hw_memory_chunk);
      new_chunk->address = BE2H_4(spec->base);
      new_chunk->size = BE2H_4(spec->size);
      new_chunk->available = 1;
      device_attach_address(device_parent(me),
			    device_name(me),
			    attach_raw_memory,
			    0 /*address space*/,
			    new_chunk->address,
			    new_chunk->size,
			    access_read_write_exec,
			    me);
      spec++;
      nr_entries--;
      *curr_chunk = new_chunk;
      curr_chunk = &new_chunk->next;
    }
  }

  /* initialize the alloc property for this device */
  hw_memory_set_available(me, hw_memory);
}

static void
hw_memory_instance_delete(device_instance *instance)
{
  return;
}

static unsigned_word
hw_memory_instance_claim(device_instance *instance,
			 unsigned_word address,
			 unsigned_word size,
			 unsigned_word alignment)
{
  hw_memory_device *hw_memory = device_instance_data(instance);
  hw_memory_chunk *chunk = NULL;
  DTRACE(memory, ("claim - address=0x%lx size=0x%lx alignment=%d\n",
		  (unsigned long)address,
		  (unsigned long)size,
		  (int)alignment));
  /* find a chunk candidate, either according to address or alignment */
  if (alignment == 0) {
    chunk = hw_memory->heap;
    while (chunk != NULL
	   && (address+size) > (chunk->address+chunk->size))
      chunk = chunk->next;
    if (chunk == NULL || address < chunk->address || !chunk->available)
      error("hw_memory_instance_claim: failed to allocate @0x%lx, size %ld\n",
	    (unsigned long)address, (unsigned long)size);
  }
  else {
    chunk = hw_memory->heap;
    while (chunk != NULL && chunk->size < size)
      chunk = chunk->next;
    if (chunk == NULL || FLOOR_PAGE(alignment-1) > 0)
      error("hw_memory_instance_claim: failed to allocate %ld, align %ld\n",
	    (unsigned long)size, (unsigned long)size);
    address = chunk->address;
  }
  /* break of a part before this memory if needed */
  ASSERT(address >= chunk->address);
  if (FLOOR_PAGE(address) > chunk->address) {
    hw_memory_chunk *last_chunk = chunk;
    /* insert a new earlier chunk */
    chunk = ZALLOC(hw_memory_chunk);
    chunk->next = last_chunk->next;
    last_chunk->next = chunk;
    /* adjust the address/size */
    chunk->address = FLOOR_PAGE(address);
    chunk->size = last_chunk->size - (chunk->address - last_chunk->address);
    last_chunk->size = chunk->address - last_chunk->address;
  }
  ASSERT(FLOOR_PAGE(address) == chunk->address);
  /* break of a bit after this chunk if needed */
  if (ALIGN_PAGE(address+size) < chunk->address + chunk->size) {
    hw_memory_chunk *next_chunk = ZALLOC(hw_memory_chunk);
    /* insert it in to the list */
    next_chunk->next = chunk->next;
    chunk->next = next_chunk;
    next_chunk->available = 1;
    /* adjust the address/size */
    next_chunk->address = ALIGN_PAGE(address+size);
    next_chunk->size = chunk->address + chunk->size - next_chunk->address;
    chunk->size = next_chunk->address - chunk->address;
  }
  ASSERT(ALIGN_PAGE(address+size) == chunk->address + chunk->size);
  /* now allocate it */
  chunk->alloc_address = address;
  chunk->alloc_size = size;
  chunk->available = 0;
  hw_memory_set_available(device_instance_device(instance), hw_memory);
  return address;
}

static void
hw_memory_instance_release(device_instance *instance,
			   unsigned_word address,
			   unsigned_word length)
{
  hw_memory_device *hw_memory = device_instance_data(instance);
  hw_memory_chunk *chunk = hw_memory->heap;
  while (chunk != NULL) {
    if (chunk->alloc_address == address
	&& chunk->alloc_size == length
	&& chunk->available == 0) {
      /* free this chunk */
      chunk->available = 1;
      /* check for merge */
      chunk = hw_memory->heap;
      while (chunk != NULL) {
	if (chunk->available
	    && chunk->next != NULL && chunk->next->available) {
	  /* adjacent */
	  hw_memory_chunk *delete = chunk->next;
	  ASSERT(chunk->address + chunk->size == delete->address);
	  chunk->size += delete->size;
	  chunk->next = delete->next;
	  zfree(delete);
	}
      }
      /* update the corresponding property */
      hw_memory_set_available(device_instance_device(instance), hw_memory);
      return;
    }
    chunk = chunk->next;
  }
  error("hw_memory_instance_release: Address 0x%lx, size %ld not found\n",
	(unsigned long)address, (unsigned long)length);
  /* FIXME - dump allocated */
  /* FIXME - dump arguments */
}

static device_instance_callbacks const hw_memory_instance_callbacks = {
  hw_memory_instance_delete,
  NULL /*read*/, NULL /*write*/, NULL /*seek*/,
  hw_memory_instance_claim, hw_memory_instance_release
};

static device_instance *
hw_memory_create_instance(device *me,
			  const char *path,
			  const char *args)
{
  return device_create_instance_from(me, NULL,
				     device_data(me), /* nothing better */
				     path, args,
				     &hw_memory_instance_callbacks);
}

static device_callbacks const hw_memory_callbacks = {
  { hw_memory_init_address, },
  { NULL, }, /* address */
  { NULL, }, /* IO */
  { NULL, }, /* DMA */
  { NULL, }, /* interrupt */
  { NULL, }, /* unit */
  hw_memory_create_instance,
};

const device_descriptor hw_memory_device_descriptor[] = {
  { "memory", hw_memory_create, &hw_memory_callbacks },
  { NULL },
};

#endif /* _HW_MEMORY_C_ */
