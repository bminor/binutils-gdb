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

#include "sim-main.h"
#include "sim-assert.h"


/* "core" module install handler.
   This is called via sim_module_install to install the "core" subsystem
   into the simulator.  */

EXTERN_SIM_CORE\
(SIM_RC)
sim_core_install (SIM_DESC sd)
{
  SIM_ASSERT (STATE_MAGIC (sd) == SIM_MAGIC_NUMBER);
  sim_module_add_uninstall_fn (sd, sim_core_uninstall);
  sim_module_add_init_fn (sd, sim_core_init);
  return SIM_RC_OK;
}


/* Uninstall the "core" subsystem from the simulator.  */

EXTERN_SIM_CORE\
(void)
sim_core_uninstall (SIM_DESC sd)
{
  /* FIXME: free buffers, etc. */
}


EXTERN_SIM_CORE\
(SIM_RC)
sim_core_init (SIM_DESC sd)
{
  sim_core *memory = STATE_CORE(sd);
  sim_core_maps map;
  for (map = 0;
       map < nr_sim_core_maps;
       map++) {
    /* blow away old mappings */
    sim_core_mapping *curr = memory->map[map].first;
    while (curr != NULL) {
      sim_core_mapping *tbd = curr;
      curr = curr->next;
      if (tbd->free_buffer) {
	SIM_ASSERT(tbd->buffer != NULL);
	zfree(tbd->buffer);
      }
      zfree(tbd);
    }
    memory->map[map].first = NULL;
  }
  return SIM_RC_OK;
}



#ifndef SIM_CORE_SIGNAL
#define SIM_CORE_SIGNAL(SD,CPU,CIA,MAP,NR_BYTES,ADDR,TRANSFER,ERROR) \
sim_core_signal ((SD), (CPU), (CIA), (MAP), (NR_BYTES), (ADDR), (TRANSFER), (ERROR))

static void
sim_core_signal (SIM_DESC sd,
		 sim_cpu *cpu,
		 sim_cia cia,
		 sim_core_maps map,
		 int nr_bytes,
		 address_word addr,
		 transfer_type transfer,
		 sim_core_signals sig)
{
  const char *copy = (transfer == read_transfer ? "read" : "write");
  switch (sig)
    {
    case sim_core_unmapped_signal:
      sim_engine_abort (sd, cpu, cia, "sim-core: %d byte %s to unmaped address 0x%lx",
			nr_bytes, copy, (unsigned long) addr);
      break;
    case sim_core_unaligned_signal:
      sim_engine_abort (sd, cpu, cia, "sim-core: %d byte misaligned %s to address 0x%lx",
			nr_bytes, copy, (unsigned long) addr);
      break;
    default:
      sim_engine_abort (sd, cpu, cia, "sim_core_signal - internal error - bad switch");
    }
}



#endif

STATIC_INLINE_SIM_CORE\
(const char *)
sim_core_map_to_str (sim_core_maps map)
{
  switch (map)
    {
    case sim_core_read_map: return "read";
    case sim_core_write_map: return "write";
    case sim_core_execute_map: return "exec";
    default: return "(invalid-map)";
    }
}


STATIC_INLINE_SIM_CORE\
(sim_core_mapping *)
new_sim_core_mapping(SIM_DESC sd,
		     attach_type attach,
		     int space,
		     unsigned_word addr,
		     unsigned nr_bytes,
		     device *device,
		     void *buffer,
		     int free_buffer)
{
  sim_core_mapping *new_mapping = ZALLOC(sim_core_mapping);
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
    sim_io_error (sd, "new_sim_core_mapping - internal error - unknown attach type %d\n",
		 attach);
  }
  return new_mapping;
}


STATIC_INLINE_SIM_CORE\
(void)
sim_core_map_attach(SIM_DESC sd,
		    sim_core_map *access_map,
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
  sim_core_mapping *next_mapping;
  sim_core_mapping **last_mapping;

  SIM_ASSERT((attach >= attach_callback && client != NULL && buffer == NULL && !free_buffer)
	 || (attach == attach_raw_memory && client == NULL && buffer != NULL));

  /* actually do occasionally get a zero size map */
  if (nr_bytes == 0) {
#if (WITH_DEVICES)
    device_error(client, "called on sim_core_map_attach with size zero");
#else
    sim_io_error (sd, "called on sim_core_map_attach with size zero");
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
  SIM_ASSERT(next_mapping == NULL || next_mapping->level >= attach);
  if (next_mapping != NULL && next_mapping->level == attach
      && next_mapping->base < (addr + (nr_bytes - 1))) {
#if (WITH_DEVICES)
    device_error(client, "map overlap when attaching %d:0x%lx (%ld)",
		 space, (long)addr, (long)nr_bytes);
#else
    sim_io_error (sd, "map overlap when attaching %d:0x%lx (%ld)",
		 space, (long)addr, (long)nr_bytes);
#endif
  }

  /* create/insert the new mapping */
  *last_mapping = new_sim_core_mapping(sd,
				   attach,
				   space, addr, nr_bytes,
				   client, buffer, free_buffer);
  (*last_mapping)->next = next_mapping;
}


INLINE_SIM_CORE\
(void)
sim_core_attach(SIM_DESC sd,
		sim_cpu *cpu,
		attach_type attach,
		access_type access,
		int space,
		unsigned_word addr,
		unsigned nr_bytes, /* host limited */
		device *client,
		void *optional_buffer)
{
  sim_core *memory = STATE_CORE(sd);
  sim_core_maps map;
  void *buffer;
  int buffer_freed;
  int i;

  /* check for for attempt to use unimplemented per-processor core map */
  if (cpu != NULL)
    sim_io_error (sd, "sim_core_map_attach - processor specific memory map not yet supported");

  if ((access & access_read_write_exec) == 0
      || (access & ~access_read_write_exec) != 0) {
#if (WITH_DEVICES)
    device_error(client, "invalid access for core attach");
#else
    sim_io_error (sd, "invalid access for core attach");
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
    device_error(client, "sim_core_attach - conflicting buffer and attach arguments");
#else
    sim_io_error (sd, "sim_core_attach - conflicting buffer and attach arguments");
#endif
    buffer = NULL;
    buffer_freed = 1;
  }
  /* attach the region to all applicable access maps */
  for (map = 0; 
       map < nr_sim_core_maps;
       map++) {
    switch (map) {
    case sim_core_read_map:
      if (access & access_read)
	sim_core_map_attach(sd, &memory->map[map],
			attach,
			space, addr, nr_bytes,
			client, buffer, !buffer_freed);
      buffer_freed ++;
      break;
    case sim_core_write_map:
      if (access & access_write)
	sim_core_map_attach(sd, &memory->map[map],
			attach,
			space, addr, nr_bytes,
			client, buffer, !buffer_freed);
      buffer_freed ++;
      break;
    case sim_core_execute_map:
      if (access & access_exec)
	sim_core_map_attach(sd, &memory->map[map],
			attach,
			space, addr, nr_bytes,
			client, buffer, !buffer_freed);
      buffer_freed ++;
      break;
    case nr_sim_core_maps:
      sim_io_error (sd, "sim_core_attach - internal error - bad switch");
      break;
    }
  }

  /* Just copy this map to each of the processor specific data structures.
     FIXME - later this will be replaced by true processor specific
     maps. */
  for (i = 0; i < MAX_NR_PROCESSORS; i++)
    *CPU_CORE (STATE_CPU (sd, i)) = *STATE_CORE (sd);
}


STATIC_INLINE_SIM_CORE\
(sim_core_mapping *)
sim_core_find_mapping(sim_core *core,
		      sim_core_maps map,
		      unsigned_word addr,
		      unsigned nr_bytes,
		      transfer_type transfer,
		      int abort, /*either 0 or 1 - hint to inline/-O */
		      sim_cpu *cpu, /* abort => cpu != NULL */
		      sim_cia cia)
{
  sim_core_mapping *mapping = core->map[map].first;
  ASSERT ((addr & (nr_bytes - 1)) == 0); /* must be aligned */
  ASSERT ((addr + (nr_bytes - 1)) >= addr); /* must not wrap */
  ASSERT (!abort || cpu != NULL); /* abort needs a non null CPU */
  while (mapping != NULL)
    {
      if (addr >= mapping->base
	  && (addr + (nr_bytes - 1)) <= mapping->bound)
	return mapping;
      mapping = mapping->next;
    }
  if (abort)
    {
      SIM_CORE_SIGNAL (CPU_STATE (cpu), cpu, cia, map, nr_bytes, addr, transfer,
		       sim_core_unmapped_signal);
    }
  return NULL;
}


STATIC_INLINE_SIM_CORE\
(void *)
sim_core_translate(sim_core_mapping *mapping,
	       unsigned_word addr)
{
  return (void *)(((char *)mapping->buffer) + addr - mapping->base);
}


INLINE_SIM_CORE\
(unsigned)
sim_core_read_buffer(SIM_DESC sd,
		     sim_core_maps map,
		     void *buffer,
		     unsigned_word addr,
		     unsigned len)
{
  unsigned count = 0;
  while (count < len) {
    unsigned_word raddr = addr + count;
    sim_core_mapping *mapping =
      sim_core_find_mapping(STATE_CORE (sd), map,
			    raddr, /*nr-bytes*/1,
			    read_transfer,
			    0, NULL, NULL_CIA); /*dont-abort*/
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
	  *(unsigned_1*)sim_core_translate(mapping, raddr);
	count += 1;
      }
  }
  return count;
}


INLINE_SIM_CORE\
(unsigned)
sim_core_write_buffer(SIM_DESC sd,
		      sim_core_maps map,
		      const void *buffer,
		      unsigned_word addr,
		      unsigned len)
{
  unsigned count = 0;
  while (count < len) {
    unsigned_word raddr = addr + count;
    sim_core_mapping *mapping = sim_core_find_mapping(STATE_CORE (sd), map,
						      raddr, /*nr-bytes*/1,
						      write_transfer,
						      0, NULL, NULL_CIA); /*dont-abort*/
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
	*(unsigned_1*)sim_core_translate(mapping, raddr) =
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
