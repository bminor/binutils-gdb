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


#ifndef _PSIM_C_
#define _PSIM_C_

#include "config.h"
#include "ppc-config.h"
#include "inline.h"

#ifndef STATIC_INLINE_PSIM
#define STATIC_INLINE_PSIM STATIC_INLINE
#endif

#include <string.h>
#include <setjmp.h>

#include "cpu.h" /* includes psim.h */
#include "idecode.h"

#include "inline.c"


/* system structure, actual size of processor array determined at
   runtime */

struct _psim {
  event_queue *events;
  device_node *devices;
  core *memory;
  /* escape routine for inner functions */
  void *path_to_halt;
  void *path_to_restart;
  /* status from last halt */
  psim_status halt_status;
  /* the processes proper */
  int nr_cpus;
  int last_cpu; /* CPU that last (tried to) execute an instruction */
  cpu *processors[0];
};


int current_target_byte_order;
int current_host_byte_order;
int current_environment;
int current_alignment;

INLINE_PSIM psim *
psim_create(const char *file_name,
	    int nr_processors)
{
  int cpu_nr;
  psim *system;

  /* sanity check */
  if (nr_processors <= 0
      || (!WITH_SMP && nr_processors != 1))
    error("psim_create() invalid number of cpus\n");

  /* create things */
  system = (psim*)zalloc(sizeof(psim)
			 + sizeof(cpu*) * (nr_processors + 1));
  system->nr_cpus = nr_processors;
  system->events = event_queue_create();
  system->devices = device_tree_create(file_name);
  system->memory = core_create(system->devices, 0);
  for (cpu_nr = 0; cpu_nr < nr_processors; cpu_nr++) {
    system->processors[cpu_nr] = cpu_create(system,
					    system->memory,
					    system->events,
					    cpu_nr);
  }

  /* fill in the missing endian information */
  current_target_byte_order
    = (device_tree_find_boolean(system->devices, "/options/little-endian?")
       ? LITTLE_ENDIAN
       : BIG_ENDIAN);
  if (WITH_TARGET_BYTE_ORDER
      && WITH_TARGET_BYTE_ORDER != current_target_byte_order)
    error("target byte order conflict\n");

  current_host_byte_order = 1;
  current_host_byte_order = (*(char*)(&current_host_byte_order)
			     ? LITTLE_ENDIAN
			     : BIG_ENDIAN);
  if (WITH_HOST_BYTE_ORDER
      && WITH_HOST_BYTE_ORDER != current_host_byte_order)
    error("host byte order conflict\n");

  /* fill in the missing OEA/VEA information */
  current_environment = (device_tree_find_boolean(system->devices,
						  "/options/vea?")
			 ? VIRTUAL_ENVIRONMENT
			 : OPERATING_ENVIRONMENT);

  /* fill in the missing ALLIGNMENT information */
  current_alignment = (device_tree_find_boolean(system->devices,
						"/options/aligned?")
		       ? STRICT_ALIGNMENT
		       : NONSTRICT_ALIGNMENT);
  if (WITH_ALIGNMENT
      && CURRENT_ALIGNMENT != WITH_ALIGNMENT)
    error("target alignment support conflict\n");

  return system;
}


/* allow the simulation to stop/restart abnormaly */

STATIC_INLINE_PSIM void
psim_set_halt_and_restart(psim *system,
			  void *halt_jmp_buf,
			  void *restart_jmp_buf)
{
  system->path_to_halt = halt_jmp_buf;
  system->path_to_restart = restart_jmp_buf;
}

STATIC_INLINE_PSIM void
psim_clear_halt_and_restart(psim *system)
{
  system->path_to_halt = NULL;
  system->path_to_restart = NULL;
}

INLINE_PSIM void
psim_restart(psim *system,
	     int current_cpu)
{
  system->last_cpu = current_cpu;
  longjmp(*(jmp_buf*)(system->path_to_restart), current_cpu + 1);
}


INLINE_PSIM void
psim_halt(psim *system,
	  int current_cpu,
	  unsigned_word cia,
	  stop_reason reason,
	  int signal)
{
  system->last_cpu = current_cpu;
  system->halt_status.cpu_nr = current_cpu;
  system->halt_status.reason = reason;
  system->halt_status.signal = signal;
  system->halt_status.program_counter = cia;
  longjmp(*(jmp_buf*)(system->path_to_halt), current_cpu + 1);
}

INLINE_PSIM psim_status
psim_get_status(psim *system)
{
  return system->halt_status;
}


cpu *
psim_cpu(psim *system,
	 int cpu_nr)
{
  if (cpu_nr < 0 || cpu_nr >= system->nr_cpus)
    return NULL;
  else
    return system->processors[cpu_nr];
}



STATIC_INLINE_PSIM int
sizeof_argument_strings(char **arg)
{
  int sizeof_strings = 0;

  /* robust */
  if (arg == NULL)
    return 0;

  /* add up all the string sizes (padding as we go) */
  for (; *arg != NULL; arg++) {
    int len = strlen(*arg) + 1;
    sizeof_strings += ALIGN_8(len);
  }

  return sizeof_strings;
}

STATIC_INLINE_PSIM int
number_of_arguments(char **arg)
{
  int nr;
  if (arg == NULL)
    return 0;
  for (nr = 0; *arg != NULL; arg++, nr++);
  return nr;
}

STATIC_INLINE_PSIM int
sizeof_arguments(char **arg)
{
  return ALIGN_8((number_of_arguments(arg) + 1) * sizeof(unsigned_word));
}

STATIC_INLINE_PSIM void
write_stack_arguments(psim *system,
		      char **arg,
		      unsigned_word start_block,
		      unsigned_word start_arg)
{
  if (CURRENT_ENVIRONMENT != VIRTUAL_ENVIRONMENT)
    {
      TRACE(trace_create_stack, ("write_stack_arguments() - skipping, OEA program\n"));
      return;
    }

  TRACE(trace_create_stack,
	("write_stack_arguments() - %s=0x%x %s=0x%x %s=0x%x %s=0x%x\n",
	 "system", system, "arg", arg,
	 "start_block", start_block, "start_arg", start_arg));
  if (arg == NULL)
    error("write_arguments: character array NULL\n");
  /* only copy in arguments, memory is already zero */
  for (; *arg != NULL; arg++) {
    int len = strlen(*arg)+1;
    TRACE(trace_create_stack,
	  ("write_stack_arguments - write %s=%s at %s=0x%x %s=0x%x %s=0x%x\n",
	   "**arg", *arg, "start_block", start_block,
	   "len", len, "start_arg", start_arg));
    if (psim_write_memory(system, 0, *arg,
			  start_block, len,
			  raw_transfer, 0) != len)
      error("write_arguments() - write of **arg (%s) at 0x%x failed\n",
	    *arg, start_block);
    if (psim_write_memory(system, 0, &start_block,
			  start_arg, sizeof(start_block),
			  cooked_transfer, 0) != sizeof(start_block))
      error("write_arguments() - write of *arg failed\n");
    start_block += ALIGN_8(len);
    start_arg += sizeof(start_block);
  }
}

STATIC_INLINE_PSIM void
create_elf_stack_frame(psim *system,
		       unsigned_word bottom_of_stack,
		       char **argv,
		       char **envp)
{
  /* fixme - this is over aligned */

  /* information block */
  const unsigned sizeof_envp_block = sizeof_argument_strings(envp);
  const unsigned_word start_envp_block = bottom_of_stack - sizeof_envp_block;
  const unsigned sizeof_argv_block = sizeof_argument_strings(argv);
  const unsigned_word start_argv_block = start_envp_block - sizeof_argv_block;

  /* auxiliary vector - contains only one entry */
  const unsigned sizeof_aux_entry = 2*sizeof(unsigned_word); /* magic */
  const unsigned_word start_aux = start_argv_block - ALIGN_8(sizeof_aux_entry);

  /* environment points (including null sentinal) */
  const unsigned sizeof_envp = sizeof_arguments(envp);
  const unsigned_word start_envp = start_aux - sizeof_envp;

  /* argument pointers (including null sentinal) */
  const int argc = number_of_arguments(argv);
  const unsigned sizeof_argv = sizeof_arguments(argv);
  const unsigned_word start_argv = start_envp - sizeof_argv;

  /* link register save address - alligned to a 16byte boundary */
  const unsigned_word top_of_stack = ((start_argv
				       - 2 * sizeof(unsigned_word))
				      & ~0xf);

  /* force some stack space */
  if (CURRENT_ENVIRONMENT == VIRTUAL_ENVIRONMENT
      && core_stack_lower_bound(system->memory) > top_of_stack) {
    unsigned_word extra_stack_space = (core_stack_lower_bound(system->memory)
				       - FLOOR_PAGE(top_of_stack));
    TRACE(trace_create_stack,
	  ("create_elf_stack_frame() - growing stack by 0x%x\n",
	   extra_stack_space));
    core_add_stack(system->memory, extra_stack_space);
  }

  /* install arguments on stack */
  write_stack_arguments(system, envp, start_envp_block, start_envp);
  write_stack_arguments(system, argv, start_argv_block, start_argv);

  /* set up the registers */
  psim_write_register(system, -1,
		      &top_of_stack, "r1", cooked_transfer);
  psim_write_register(system, -1,
		      &argc, "r3", cooked_transfer);
  psim_write_register(system, -1,
		      &start_argv, "r4", cooked_transfer);
  psim_write_register(system, -1,
		      &start_envp, "r5", cooked_transfer);
  psim_write_register(system, -1,
		      &start_aux, "r6", cooked_transfer);
}

STATIC_INLINE_PSIM void
create_aix_stack_frame(psim *system,
		       unsigned_word bottom_of_stack,
		       char **argv,
		       char **envp)
{
  unsigned_word core_envp;
  unsigned_word core_argv;
  unsigned_word core_argc;
  unsigned_word core_aux;
  unsigned_word top_of_stack;

  /* cheat - create an elf stack frame */
  create_elf_stack_frame(system, bottom_of_stack, argv, envp);
  
  /* extract argument addresses from registers */
  psim_read_register(system, 0, &top_of_stack, "r1", cooked_transfer);
  psim_read_register(system, 0, &core_argc, "r3", cooked_transfer);
  psim_read_register(system, 0, &core_argv, "r4", cooked_transfer);
  psim_read_register(system, 0, &core_envp, "r5", cooked_transfer);
  psim_read_register(system, 0, &core_aux, "r6", cooked_transfer);

  /* check stack fits at least this much */
  if (CURRENT_ENVIRONMENT == VIRTUAL_ENVIRONMENT
      && core_stack_lower_bound(system->memory) > top_of_stack) {
    unsigned_word extra_stack_space = (core_stack_lower_bound(system->memory)
				       - FLOOR_PAGE(top_of_stack));
    TRACE(trace_create_stack,
	  ("create_aix_stack_frame() - growing stack by 0x%x\n",
	   extra_stack_space));
    core_add_stack(system->memory, extra_stack_space);
  }

  /* extract arguments from registers */
  error("create_aix_stack_frame() - what happens next?\n");
}


INLINE_PSIM void
psim_load(psim *system)
{
  unsigned_word program_counter;
  msreg msr;

  /* load in core data */
  core_init(system->memory);

  /* set up all processor entry points (to same thing).  Maybe
     someday, the device tree could include information specifying the
     entry point for each processor, one day */
  TRACE(trace_tbd,
	("TBD - device tree specifying entry point of each processor\n"));
  program_counter = device_tree_find_int(system->devices,
					 "/options/program-counter");
  psim_write_register(system, -1,
		      &program_counter,
		      "pc", cooked_transfer);
  system->last_cpu = system->nr_cpus - 1; /* force loop to restart */

  /* set up the MSR for at least be/le mode */
  msr = (device_tree_find_boolean(system->devices,
				  "/options/little-endian?")
	 ? msr_little_endian_mode
	 : 0);
  psim_write_register(system, -1,
		      &msr,
		      "msr", cooked_transfer);
}

INLINE_PSIM void
psim_stack(psim *system,
	   char **argv,
	   char **envp)
{
  unsigned_word stack_pointer = device_tree_find_int(system->devices,
						     "/options/stack-pointer");
  if (device_tree_find_boolean(system->devices,
			       "/options/elf?"))
    create_elf_stack_frame(system, stack_pointer, argv, envp);
  else
    create_aix_stack_frame(system, stack_pointer, argv, envp);
}



/* EXECUTE REAL CODE: 

   Unfortunatly, there are multiple cases to consider vis:

   	<icache> X <smp> X <events> X <keep-running-flag> X ...

   Consequently this function is written in multiple different ways */

STATIC_INLINE_PSIM void
run_until_stop(psim *system,
	       volatile int *keep_running)
{

#if (WITH_IDECODE_CACHE == 0 && WITH_SMP == 0)

  /* CASE 1: No instruction cache and no SMP.

     In this case, we can take advantage of the fact that the current
     instruction address does not need to be returned to the cpu
     object after every execution of an instruction.  Instead it only
     needs to be saved when either A. the main loop exits or B. a
     cpu-{halt,restart} call forces the loop to be re-entered.  The
     later functions always save the current cpu instruction
     address. */

  jmp_buf halt;
  jmp_buf restart;
  psim_set_halt_and_restart(system, &halt, &restart);
  if (!setjmp(halt)) {
    do {
      if (!setjmp(restart)) {
	cpu *const processor = system->processors[0];
	unsigned_word cia = cpu_get_program_counter(processor);
	do {
	  if (WITH_EVENTS) {
	    if (event_queue_tick(system->events)) {
	      cpu_set_program_counter(processor, cia);
	      event_queue_process(system->events);
	      cia = cpu_get_program_counter(processor);
	    }
	  }
	  {
	    instruction_word const instruction
	      = vm_instruction_map_read(cpu_instruction_map(processor),
					processor, cia);
	    cia = idecode_issue(processor, instruction, cia);
	  }
	} while (keep_running == NULL || *keep_running);
	cpu_set_program_counter(processor, cia);
      }
    } while(keep_running == NULL || *keep_running);
  }
  psim_clear_halt_and_restart(system);
#endif


#if (WITH_IDECODE_CACHE > 0 && WITH_SMP == 0)

  /* CASE 2: Instruction case but no SMP

     Here, the additional complexity comes from there being two
     different cache implementations.  A simple function address cache
     or a full cracked instruction cache */

  jmp_buf halt;
  jmp_buf restart;
  psim_set_halt_and_restart(system, &halt, &restart);
  if (!setjmp(halt)) {
    do {
      if (!setjmp(restart)) {
	cpu *const processor = system->processors[0];
	unsigned_word cia = cpu_get_program_counter(processor);
	do {
	  if (WITH_EVENTS)
	    if (event_queue_tick(system->events)) {
	      cpu_set_program_counter(processor, cia);
	      event_queue_process(system->events);
	      cia = cpu_get_program_counter(processor);
	    }
	  { 
	    idecode_cache *const cache_entry
	      = cpu_icache(processor) + (cia / 4 % IDECODE_CACHE_SIZE);
	    if (cache_entry->address == cia) {
	      idecode_semantic *const semantic = cache_entry->semantic;
#if WITH_IDECODE_CACHE == 1
	      cia = semantic(processor, cache_entry->instruction, cia);
#else
	      cia = semantic(processor, cache_entry, cia);
#endif
	    }
	    else {
	      instruction_word const instruction
		= vm_instruction_map_read(cpu_instruction_map(processor),
					  processor,
					  cia);
#if WITH_IDECODE_CACHE == 1
	      idecode_semantic *const semantic = idecode(processor,
							 instruction,
							 cia);
#else
	      idecode_semantic *const semantic = idecode(processor,
							 instruction,
							 cia,
							 cache_entry);
#endif
	      cache_entry->address = cia;
	      cache_entry->semantic = semantic;
#if WITH_IDECODE_CACHE == 1
	      cache_entry->instruction = instruction;
	      cia = semantic(processor, instruction, cia);
#else
	      cia = semantic(processor, cache_entry, cia);
#endif
	    }
	  }
	} while (keep_running == NULL || *keep_running);
	cpu_set_program_counter(processor, cia);
      }
    } while(keep_running == NULL || *keep_running);
  }
  psim_clear_halt_and_restart(system);
#endif


#if (WITH_IDECODE_CACHE == 0 && WITH_SMP > 0)

  /* CASE 3: No ICACHE but SMP

     The complexity here comes from needing to correctly restart the
     system when it is aborted.  In particular if cpu0 requests a
     restart, the next cpu is still cpu1.  Cpu0 being restarted after
     all the other CPU's and the event queue have been processed */

  jmp_buf halt;
  jmp_buf restart;
  psim_set_halt_and_restart(system, &halt, &restart);

  if (!setjmp(halt)) {
    int first_cpu = setjmp(restart);
    if (first_cpu == 0)
      first_cpu = system->last_cpu + 1;
    do {
      int current_cpu;
      for (current_cpu = first_cpu, first_cpu = 0;
	   current_cpu < system->nr_cpus + (WITH_EVENTS ? 1 : 0);
	   current_cpu++) {
	if (WITH_EVENTS && current_cpu == system->nr_cpus) {
	  if (event_queue_tick(system->events))
	    event_queue_process(system->events);
	}
	else {
	  cpu *const processor = system->processors[current_cpu];
	  unsigned_word const cia = cpu_get_program_counter(processor);
	  instruction_word instruction =
	    vm_instruction_map_read(cpu_instruction_map(processor),
				    processor,
				    cia);
	  cpu_set_program_counter(processor,
				  idecode_issue(processor, instruction, cia));
	}
	if (!(keep_running == NULL || *keep_running)) {
	  system->last_cpu = current_cpu;
	  break;
	}
      }
    } while (keep_running == NULL || *keep_running);
  }
  psim_clear_halt_and_restart(system);
#endif

#if (WITH_IDECODE_CACHE > 0 && WITH_SMP > 0)

  /* CASE 4: ICACHE and SMP ...

     This time, everything goes wrong.  Need to restart loops
     correctly, need to save the program counter and finally need to
     keep track of each processors current address! */

  jmp_buf halt;
  jmp_buf restart;
  psim_set_halt_and_restart(system, &halt, &restart);

  if (!setjmp(halt)) {
    int first_cpu = setjmp(restart);
    if (!first_cpu)
      first_cpu = system->last_cpu + 1;
    do {
      int current_cpu;
      for (current_cpu = first_cpu, first_cpu = 0;
	   current_cpu < system->nr_cpus + (WITH_EVENTS ? 1 : 0);
	   current_cpu++) {
	if (WITH_EVENTS && current_cpu == system->nr_cpus) {
	  if (event_queue_tick(system->events))
	    event_queue_process(system->events);
	}
	else {
	  cpu *processor = system->processors[current_cpu];
	  unsigned_word const cia = cpu_get_program_counter(processor);
	  idecode_cache *cache_entry
	    = (cpu_icache(processor) + (cia / 4 % IDECODE_CACHE_SIZE));
	  if (cache_entry->address == cia) {
	    idecode_semantic *semantic = cache_entry->semantic;
#if WITH_IDECODE_CACHE == 1
	    cpu_set_program_counter(processor,
				    semantic(processor,
					     cache_entry->instruction,
					     cia);
#else
	    cpu_set_program_counter(processor,
				    semantic(processor,
					     cache_entry,
					     cia);
#endif
	  }
	  else {
	    instruction_word instruction =
	      vm_instruction_map_read(cpu_instruction_map(processor),
				      processor,
				      cia);
#if WITH_IDECODE_CACHE == 1
	    idecode_semantic *semantic = idecode(processor,
						 instruction,
						 cia);
#else
	    idecode_semantic *semantic = idecode(processor,
						 instruction,
						 cia,
						 cache_entry);
#endif
	    cache_entry->address = cia;
	    cache_entry->semantic = semantic;
#if WITH_IDECODE_CACHE == 1
	    cache_entry->instruction = instruction;
	    cpu_set_program_counter(processor,
				    semantic(processor, instruction, cia));
#else
	    cpu_set_program_counter(processor,
				    semantic(processor, cache_entry, cia);
#endif
	  }
	}
	if (!(keep_running == NULL || *keep_running))
	  break;
      }
    } while (keep_running == NULL || *keep_running);
  }
  psim_clear_halt_and_restart(system);
#endif
}


/* SIMULATE INSTRUCTIONS, various different ways of achieving the same
   thing */

INLINE_PSIM void
psim_step(psim *system)
{
  volatile int keep_running = 0;
  psim_run_until_stop(system, &keep_running);
}

INLINE_PSIM void
psim_run(psim *system)
{
  run_until_stop(system, NULL);
}

INLINE_PSIM void
psim_run_until_stop(psim *system,
		    volatile int *keep_running)
{
  run_until_stop(system, keep_running);
}



/* storage manipulation functions */

INLINE_PSIM void
psim_read_register(psim *system,
		   int which_cpu,
		   void *buf,
		   const char reg[],
		   transfer_mode mode)
{
  register_descriptions description;
  char cooked_buf[sizeof(natural_word)];
  cpu *processor;

  /* find our processor */
  if (which_cpu < 0 || which_cpu > system->nr_cpus)
    error("psim_read_register() - invalid processor %d\n", which_cpu);
  if (which_cpu == system->nr_cpus)
    which_cpu = system->last_cpu;
  processor = system->processors[which_cpu];

  /* find the register description */
  description = register_description(reg);
  if (description.type == reg_invalid)
    error("psim_read_register() invalid register name `%s'\n", reg);

  /* get the cooked value */
  switch (description.type) {

  case reg_gpr:
    *(gpreg*)cooked_buf = cpu_registers(processor)->gpr[description.index];
    break;

  case reg_spr:
    *(spreg*)cooked_buf = cpu_registers(processor)->spr[description.index];
    break;
    
  case reg_sr:
    *(sreg*)cooked_buf = cpu_registers(processor)->sr[description.index];
    break;

  case reg_fpr:
    *(fpreg*)cooked_buf = cpu_registers(processor)->fpr[description.index];
    break;

  case reg_pc:
    *(unsigned_word*)cooked_buf = cpu_get_program_counter(processor);
    break;

  case reg_cr:
    *(creg*)cooked_buf = cpu_registers(processor)->cr;
    break;

  case reg_msr:
    *(msreg*)cooked_buf = cpu_registers(processor)->msr;
    break;

  default:
    printf_filtered("psim_read_register(processor=0x%x,buf=0x%x,reg=%s) %s\n",
		    processor, buf, reg,
		    "read of this register unimplemented");
    break;

  }

  /* the PSIM internal values are in host order.  To fetch raw data,
     they need to be converted into target order and then returned */
  if (mode == raw_transfer) {
    /* FIXME - assumes that all registers are simple integers */
    switch (description.size) {
    case 1: 
      *(unsigned_1*)buf = H2T_1(*(unsigned_1*)cooked_buf);
      break;
    case 2:
      *(unsigned_2*)buf = H2T_2(*(unsigned_2*)cooked_buf);
      break;
    case 4:
      *(unsigned_4*)buf = H2T_4(*(unsigned_4*)cooked_buf);
      break;
    case 8:
      *(unsigned_8*)buf = H2T_8(*(unsigned_8*)cooked_buf);
      break;
    }
  }
  else {
    bcopy(cooked_buf, buf, description.size);
  }

}



INLINE_PSIM void
psim_write_register(psim *system,
		    int which_cpu,
		    const void *buf,
		    const char reg[],
		    transfer_mode mode)
{
  cpu *processor;
  register_descriptions description;
  char cooked_buf[sizeof(natural_word)];

  /* find our processor */
  if (which_cpu == -1) {
    int i;
    for (i = 0; i < system->nr_cpus; i++)
      psim_write_register(system, i, buf, reg, mode);
    return;
  }
  else if (which_cpu == system->nr_cpus) {
    which_cpu = system->last_cpu;
  }
  else if (which_cpu < 0 || which_cpu >= system->nr_cpus) {
    error("psim_read_register() - invalid processor %d\n", which_cpu);
  }

  processor = system->processors[which_cpu];

  /* find the description of the register */
  description = register_description(reg);
  if (description.type == reg_invalid)
    error("psim_write_register() invalid register name %s\n", reg);

  /* If the data is comming in raw (target order), need to cook it
     into host order before putting it into PSIM's internal structures */
  if (mode == raw_transfer) {
    switch (description.size) {
    case 1: 
      *(unsigned_1*)cooked_buf = T2H_1(*(unsigned_1*)buf);
      break;
    case 2:
      *(unsigned_2*)cooked_buf = T2H_2(*(unsigned_2*)buf);
      break;
    case 4:
      *(unsigned_4*)cooked_buf = T2H_4(*(unsigned_4*)buf);
      break;
    case 8:
      *(unsigned_8*)cooked_buf = T2H_8(*(unsigned_8*)buf);
      break;
    }
  }
  else {
    bcopy(buf, cooked_buf, description.size);
  }

  /* put the cooked value into the register */
  switch (description.type) {

  case reg_gpr:
    cpu_registers(processor)->gpr[description.index] = *(gpreg*)cooked_buf;
    break;

  case reg_fpr:
    cpu_registers(processor)->fpr[description.index] = *(fpreg*)cooked_buf;
    break;

  case reg_pc:
    cpu_set_program_counter(processor, *(unsigned_word*)cooked_buf);
    break;

  case reg_spr:
    cpu_registers(processor)->spr[description.index] = *(spreg*)cooked_buf;
    break;

  case reg_sr:
    cpu_registers(processor)->sr[description.index] = *(sreg*)cooked_buf;
    break;

  case reg_cr:
    cpu_registers(processor)->cr = *(creg*)cooked_buf;
    break;

  case reg_msr:
    cpu_registers(processor)->msr = *(msreg*)cooked_buf;
    break;

  default:
    printf_filtered("psim_write_register(processor=0x%x,cooked_buf=0x%x,reg=%s) %s\n",
		    processor, cooked_buf, reg,
		    "read of this register unimplemented");
    break;

  }

}



INLINE_PSIM unsigned
psim_read_memory(psim *system,
		 int which_cpu,
		 void *buffer,
		 unsigned_word vaddr,
		 unsigned len,
		 transfer_mode mode)
{
  cpu *processor;
  if (which_cpu < 0 || which_cpu > system->nr_cpus)
    error("psim_read_memory() invalid cpu\n");
  if (which_cpu == system->nr_cpus)
    which_cpu = system->last_cpu;
  processor = system->processors[which_cpu];
  return vm_data_map_read_buffer(cpu_data_map(processor),
				 buffer, vaddr, len, mode);
}


INLINE_PSIM unsigned
psim_write_memory(psim *system,
		  int which_cpu,
		  const void *buffer,
		  unsigned_word vaddr,
		  unsigned len,
		  transfer_mode mode,
		  int violate_read_only_section)
{
  cpu *processor;
  if (which_cpu < 0 || which_cpu > system->nr_cpus)
    error("psim_read_memory() invalid cpu\n");
  if (which_cpu == system->nr_cpus)
    which_cpu = system->last_cpu;
  processor = system->processors[which_cpu];
  return vm_data_map_write_buffer(cpu_data_map(processor),
				  buffer, vaddr, len, mode, 1);
}


INLINE_PSIM void
psim_print_info(psim *system, int verbose)
{
  psim_status status;
  int i;


  status = psim_get_status(system);
  switch (status.reason) {
  default:
    break;	/* our caller will print an appropriate error message */

  case was_exited:
    printf ("Exit status = %d\n", status.signal);
    break;

  case was_signalled:
    printf ("Got signal %d\n", status.signal);
    break;
  }

  for (i = 0; i < system->nr_cpus; i++)
    cpu_print_info (system->processors[i], verbose);
}

#endif /* _PSIM_C_ */
