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


#ifndef _CORE_C_
#define _CORE_C_

#ifndef STATIC_INLINE_CORE
#define STATIC_INLINE_CORE STATIC_INLINE
#endif

#include "basics.h"
#include "device_tree.h"
#include "memory_map.h"
#include "core.h"


struct _core {
  /* attached devices */
  device_node *device_tree;
  /* different memory maps */
  memory_map *readable; /* really everything */
  memory_map *writeable;
  memory_map *executable;
  /* VEA model requires additional memory information */
  unsigned_word data_upper_bound;
  unsigned_word data_high_water;
  unsigned_word stack_upper_bound;
  unsigned_word stack_lower_bound;
  unsigned_word stack_low_water;
  /* misc */
  int trace;
};


STATIC_INLINE_CORE void
create_core_from_addresses(device_node *device,
			   void *data)
{
  core *memory = (core*)data;
  device_address *address;
  for (address = device->addresses;
       address != NULL;
       address = address->next_address) {
    switch (device->type) {
    case memory_device:
      {
	void *ram = zalloc(address->size);
	TRACE(trace_core,
	      ("create_core_from_addresses() adding memory at 0x%.8x-0x%.8x, size %8d\n",
	       address->lower_bound, address->lower_bound + address->size - 1, address->size));
	core_add_raw_memory(memory,
			    ram,
			    address->lower_bound,
			    address->size,
			    address->access);
      }
      break;
    case sequential_device:
    case block_device:
    case bus_device:
    case other_device:
      {
	TRACE(trace_core,
	      ("create_core_from_addresses() adding device at 0x%.8x-0x%.8x, size %8d\n",
	       address->lower_bound, address->lower_bound + address->size - 1, address->size));
	ASSERT(device->callbacks != NULL);
	core_add_callback_memory(memory,
				 device,
				 device->callbacks->read_callback,
				 device->callbacks->write_callback,
				 address->lower_bound,
				 address->size,
				 address->access);
      }
      break;
    default:
      TRACE(trace_core,
	    ("create_core_from_addresses() unknown type %d\n", (int)device->type));
      break;
      /* nothing happens here */
    }
  }
}


INLINE_CORE core *
core_create(device_node *root,
	    int trace)
{
  core *memory;

  /* Initialize things */
  memory = ZALLOC(core);
  memory->trace = trace;
  memory->device_tree = root;

  /* allocate space for the separate virtual to physical maps */
  memory->executable = new_memory_map();
  memory->readable = new_memory_map();
  memory->writeable = new_memory_map();

  /* initial values for the water marks */
  memory->data_high_water = 0;
  memory->stack_low_water = memory->data_high_water - sizeof(unsigned_word);

  /* go over the device tree looking for address ranges to add to
     memory */
  device_tree_traverse(root,
		       create_core_from_addresses,
		       NULL,
		       memory);

  /* return the created core object */
  return memory;
}


STATIC_INLINE_CORE void
zero_core_from_addresses(device_node *device,
			 void *data)
{
  core *memory = (core*)data;
  device_address *address;

  /* for memory nodes, copy or zero any data */
  if (device->type == memory_device) {
    for (address = device->addresses;
	 address != NULL;
	 address = address->next_address) {
      if (memory_map_zero(memory->readable,
			  address->lower_bound,
			  address->size) != address->size)
	error("init_core_from_addresses() - zero failed\n");
      /* adjust high water mark (sbrk) */
      if (memory->data_upper_bound < address->upper_bound)
	memory->data_upper_bound = address->upper_bound;
    }
  }
}

STATIC_INLINE_CORE void
load_core_from_addresses(device_node *device,
			 void *data)
{
  core *memory = (core*)data;
  device_address *address;

  /* initialize the address range with the value attached to the
     address.  Even works for devices! */
  for (address = device->addresses;
       address != NULL;
       address = address->next_address) {
    /* (re)init the address range.  I don't want to think about what
       this is doing to callback devices! */
    if (address->init) {
      if (memory_map_write_buffer(memory->readable,
				  address->init,
				  address->lower_bound,
				  address->size,
				  raw_transfer) != address->size)
	error("init_core_from_addresses() - write failed\n");
    }
  }
}

INLINE_CORE void
core_init(core *memory)
{
  unsigned nr_cleared;
  unsigned_word clear_base;
  unsigned_word clear_bound;

  /* for vea, several memory break points */
  memory->data_upper_bound = 0;
  memory->stack_upper_bound = device_tree_find_int(memory->device_tree,
						   "/options/stack-pointer");;
  memory->stack_lower_bound = memory->stack_upper_bound;

  /* (re) clear all of memory that is specified by memory-address
     entries.  While we're at it determine the upper bound for memory
     areas */
  device_tree_traverse(memory->device_tree,
		       NULL,
		       zero_core_from_addresses,
		       memory);

  /* May have grown the data sectioin (vea model), zero that too if
     present */
  clear_base = memory->data_upper_bound;
  clear_bound = memory->data_high_water;
  if (clear_bound > clear_base) {
    while ((nr_cleared = memory_map_zero(memory->readable,
					 clear_base,
					 clear_bound - clear_base)) > 0) {
      clear_base += nr_cleared;
    }
  }

  /* clear any part of the stack that was dynamically allocated */
  clear_base = memory->stack_low_water;
  clear_bound = memory->stack_upper_bound;
  if (clear_bound > clear_base) {
    while ((nr_cleared = memory_map_zero(memory->readable,
					 clear_base,
					 clear_bound - clear_base)) > 0) {
      clear_base += nr_cleared;
    }
  }

  /* with everything zero'ed, now (re) load any data sections */
  device_tree_traverse(memory->device_tree,
		       NULL,
		       load_core_from_addresses,
		       memory);

}



INLINE_CORE void
core_add_raw_memory(core *memory,
		    void *buffer,
		    unsigned_word base,
		    unsigned size,
		    device_access access)
{
  if (access & device_is_readable)
    memory_map_add_raw_memory(memory->readable,
			      buffer, base, size);
  if (access & device_is_writeable)
    memory_map_add_raw_memory(memory->writeable,
			      buffer, base, size);
  if (access & device_is_executable)
    memory_map_add_raw_memory(memory->executable,
			      buffer, base, size);
}


INLINE_CORE void
core_add_callback_memory(core *memory,
			 device_node *device,
			 device_reader_callback *reader,
			 device_writer_callback *writer,
			 unsigned_word base,
			 unsigned size,
			 device_access access)
{
  if (access & device_is_readable)
    memory_map_add_callback_memory(memory->readable,
				   device, reader, writer,
				   base, size);
  if (access & device_is_writeable)
    memory_map_add_callback_memory(memory->writeable,
				   device, reader, writer,
				   base, size);
  if (access & device_is_executable)
    memory_map_add_callback_memory(memory->executable,
				   device, reader, writer,
				   base, size);
}


STATIC_INLINE_CORE void
malloc_core_memory(core *memory,
		   unsigned_word base,
		   unsigned size,
		   device_access access)
{
  void *buffer = (void*)zalloc(size);
  core_add_raw_memory(memory, buffer, base, size, access);
}

INLINE_CORE unsigned_word
core_data_upper_bound(core *memory)
{
  return memory->data_upper_bound;
}


INLINE_CORE unsigned_word
core_stack_lower_bound(core *memory)
{
  return memory->stack_lower_bound;
}

INLINE_CORE unsigned_word
core_stack_size(core *memory)
{
  return (memory->stack_upper_bound - memory->stack_lower_bound);
}



INLINE_CORE void 
core_add_data(core *memory, unsigned_word incr)
{
  memory->data_upper_bound += incr;
  if (memory->data_upper_bound > memory->data_high_water) {
    malloc_core_memory(memory, memory->data_high_water, incr,
		     device_is_readable | device_is_writeable);
    memory->data_high_water = memory->data_upper_bound;
  }
}


INLINE_CORE void 
core_add_stack(core *memory, unsigned_word incr)
{
  memory->stack_lower_bound -= incr;
  if (memory->stack_lower_bound < memory->stack_low_water) {
    malloc_core_memory(memory, memory->stack_lower_bound, incr,
		       device_is_readable | device_is_writeable);
    memory->stack_low_water = memory->stack_lower_bound;
  }
}


INLINE_CORE memory_map *
core_readable(core *core)
{
  return core->readable;
}


INLINE_CORE memory_map *
core_writeable(core *core)
{
  return core->writeable;
}


INLINE_CORE memory_map *
core_executable(core *core)
{
  return core->executable;
}

#endif /* _CORE_ */
