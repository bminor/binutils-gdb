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


#ifndef _CPU_C_
#define _CPU_C_

#ifndef STATIC_INLINE_CPU
#define STATIC_INLINE_CPU STATIC_INLINE
#endif

#include <setjmp.h>

#include "cpu.h"
#include "idecode.h"

struct _cpu {

  /* the registers */
  registers regs;

  /* current instruction address */
  unsigned_word program_counter;

  /* the memory maps */
  core *physical; /* all of memory */
  vm *virtual;
  vm_instruction_map *instruction_map; /* instructions */
  vm_data_map *data_map; /* data */

  /* current state of interrupt inputs */
  int external_exception_pending;

  /* the system this processor is contained within */
  psim *system;
  event_queue *events;
  int cpu_nr;

  /* if required, a cache to store decoded instructions */
#if WITH_IDECODE_CACHE
  idecode_cache icache[IDECODE_CACHE_SIZE];
#endif

  /* address reservation: keep the physical address and the contents
     of memory at that address */
  memory_reservation reservation;

  /* offset from event time to this cpu's idea of the local time */
  signed64 time_base_local_time;
  signed64 decrementer_local_time;
  event_entry_tag decrementer_event;

  /* Counts of number of instructions executed.  */
  long number_of_insns;
};


INLINE_CPU cpu *
cpu_create(psim *system,
	   core *memory,
	   event_queue *events,
	   int cpu_nr)
{
  cpu *processor = ZALLOC(cpu);

  /* create the virtual memory map from the core */
  processor->physical = memory;
  processor->virtual = vm_create(memory);
  processor->instruction_map = vm_create_instruction_map(processor->virtual);
  processor->data_map = vm_create_data_map(processor->virtual);

  /* link back to core system */
  processor->system = system;
  processor->events = events;
  processor->cpu_nr = cpu_nr;

  return processor;
}


/* find ones way home */

INLINE_CPU psim *
cpu_system(cpu *processor)
{
  return processor->system;
}

INLINE_CPU int
cpu_nr(cpu *processor)
{
  return processor->cpu_nr;
}

INLINE_CPU event_queue *
cpu_event_queue(cpu *processor)
{
  return processor->events;
}


/* The processors local concept of time */

INLINE_CPU signed64
cpu_get_time_base(cpu *processor)
{
  return (event_queue_time(processor->events)
	  + processor->time_base_local_time);
}

INLINE_CPU void
cpu_set_time_base(cpu *processor,
		  signed64 time_base)
{
  processor->time_base_local_time = (event_queue_time(processor->events)
				     - time_base);
}

INLINE_CPU signed32
cpu_get_decrementer(cpu *processor)
{
  return (processor->decrementer_local_time
	  - event_queue_time(processor->events));
}

STATIC_INLINE_CPU void
cpu_decrement_event(event_queue *queue,
		    void *data)
{
  cpu *processor = (cpu*)data;
  if (!decrementer_interrupt(processor)) {
    processor->decrementer_event = event_queue_schedule(processor->events,
							1, /* NOW! */
							cpu_decrement_event,
							processor);
  }
}

INLINE_CPU void
cpu_set_decrementer(cpu *processor,
		    signed32 decrementer)
{
  signed64 old_decrementer = (processor->decrementer_local_time
			      - event_queue_time(processor->events));
  event_queue_deschedule(processor->events, processor->decrementer_event);
  processor->decrementer_local_time = (event_queue_time(processor->events)
				       + decrementer);
  if (decrementer < 0 && old_decrementer >= 0)
    /* dec interrupt occures if the sign of the decrement reg is
       changed by the load operation */
    processor->decrementer_event = event_queue_schedule(processor->events,
							1, /* NOW! */
							cpu_decrement_event,
							processor);
  else if (decrementer >= 0)
    processor->decrementer_event = event_queue_schedule(processor->events,
							decrementer,
							cpu_decrement_event,
							processor);
}


/* program counter manipulation */

INLINE_CPU void
cpu_set_program_counter(cpu *processor,
			unsigned_word new_program_counter)
{
  processor->program_counter = new_program_counter;
}

INLINE_CPU unsigned_word
cpu_get_program_counter(cpu *processor)
{
  return processor->program_counter;
}

INLINE_CPU void
cpu_restart(cpu *processor,
	    unsigned_word nia)
{
  processor->program_counter = nia;
  psim_restart(processor->system, processor->cpu_nr);
}

INLINE_CPU void
cpu_halt(cpu *processor,
	 unsigned_word cia,
	 stop_reason reason,
	 int signal)
{
  processor->program_counter = cia;
  psim_halt(processor->system, processor->cpu_nr, cia, reason, signal);
}


#if WITH_IDECODE_CACHE
/* allow access to the cpu's instruction cache */
INLINE_CPU idecode_cache *
cpu_icache(cpu *processor)
{
  return processor->icache;
}
#endif


/* address map revelation */

INLINE_CPU vm_instruction_map *
cpu_instruction_map(cpu *processor)
{
  return processor->instruction_map;
}

INLINE_CPU vm_data_map *
cpu_data_map(cpu *processor)
{
  return processor->data_map;
}

INLINE_CPU core *
cpu_core(cpu *processor)
{
  return processor->physical;
}


/* reservation access */

INLINE_CPU memory_reservation *
cpu_reservation(cpu *processor)
{
  return &processor->reservation;
}


/* register access */

INLINE_CPU registers *
cpu_registers(cpu *processor)
{
  return &processor->regs;
}

INLINE_CPU void
cpu_synchronize_context(cpu *processor)
{
#if WITH_IDECODE_CACHE
  /* kill off the contents of the cache */
  int i;
  for (i = 0; i < IDECODE_CACHE_SIZE; i++)
    processor->icache[i].address = MASK(0,63);
#endif
  vm_synchronize_context(processor->virtual,
			 processor->regs.spr,
			 processor->regs.sr,
			 processor->regs.msr);
}


/* # of instructions counter access */

INLINE_CPU void
cpu_increment_number_of_insns(cpu *processor)
{
  processor->number_of_insns++;
}

INLINE_CPU long
cpu_get_number_of_insns(cpu *processor)
{
  return processor->number_of_insns;
}

INLINE_CPU void
cpu_print_info(cpu *processor, int verbose)
{
  printf_filtered("CPU %d executed %ld instructions.\n",
		  processor->cpu_nr+1,
		  processor->number_of_insns);
}

#endif /* _CPU_C_ */
