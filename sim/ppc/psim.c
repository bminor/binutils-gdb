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

#include <stdio.h>
#include <ctype.h>

#include "config.h"
#include "ppc-config.h"
#include "inline.h"

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifndef STATIC_INLINE_PSIM
#define STATIC_INLINE_PSIM STATIC_INLINE
#endif

#include <setjmp.h>

#include "cpu.h" /* includes psim.h */
#include "idecode.h"

#ifdef HAVE_STRING_H
#include <string.h>
#else
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#endif

#include "bfd.h"


#include "inline.c"

/* Any starting address less than this is assumed to be an OEA program
   rather than VEA.  */
#ifndef OEA_START_ADDRESS
#define OEA_START_ADDRESS 4096
#endif

/* Any starting address greater than this is assumed to be an OpenBoot
   rather than VEA */
#ifndef OPENBOOT_START_ADDRESS
#define OPENBOOT_START_ADDRESS 0x80000000
#endif

#ifndef OEA_MEMORY_SIZE
#define OEA_MEMORY_SIZE 0x100000
#endif


/* system structure, actual size of processor array determined at
   runtime */

struct _psim {
  event_queue *events;
  device_tree *devices;
  mon *monitor;
  core *memory;
  /* escape routine for inner functions */
  void *path_to_halt;
  void *path_to_restart;
  /* status from last halt */
  psim_status halt_status;
  /* the processes proper */
  int nr_cpus;
  int last_cpu; /* CPU that last (tried to) execute an instruction */
  cpu *processors[MAX_NR_PROCESSORS];
};


int current_target_byte_order;
int current_host_byte_order;
int current_environment;
int current_alignment;
int current_floating_point;


/* create a device tree from the image */



/* Raw hardware tree:

   A small default set of devices are configured.  Each section of the
   image is loaded directly into physical memory. */

STATIC_INLINE_PSIM void
create_hardware_device_tree(bfd *image,
			    device_tree *root)
{
  char *name;
  const memory_size = OEA_MEMORY_SIZE;

  /* options */
  device_tree_add_passthrough(root, "/options");
  device_tree_add_integer(root, "/options/smp",
			  MAX_NR_PROCESSORS);
  device_tree_add_boolean(root, "/options/little-endian?",
			  !image->xvec->byteorder_big_p);
  device_tree_add_string(root, "/options/environment-architecture",
			 "operating");
  device_tree_add_boolean(root, "/options/strict-alignment?",
			  (WITH_ALIGNMENT == STRICT_ALIGNMENT
			   || !image->xvec->byteorder_big_p));
  device_tree_add_boolean(root, "/options/floating-point?",
			  WITH_FLOATING_POINT);

  /* hardware */
  name = printd_uw_u_u("/memory", 0, memory_size, access_read_write_exec);
  device_tree_add_found_device(root, name);
  zfree(name);
  device_tree_add_found_device(root, "/iobus@0x400000");
  device_tree_add_found_device(root, "/iobus/console@0x000000,16");
  device_tree_add_found_device(root, "/iobus/halt@0x100000,4");
  device_tree_add_found_device(root, "/iobus/icu@0x200000,4");

  /* initialization */
  device_tree_add_passthrough(root, "/init");
  device_tree_add_found_device(root, "/init/register@pc,0x0");
  name = printd_c_uw("/init/register", "sp", memory_size);
  device_tree_add_found_device(root, name);
  zfree(name);
  name = printd_c_uw("/init/register", "msr",
		     (image->xvec->byteorder_big_p
		      ? 0
		      : msr_little_endian_mode));
  device_tree_add_found_device(root, name);
  zfree(name);
  /* AJC puts the PC at zero and wants a stack while MM puts it above
     zero and doesn't. Really there should be no stack *but* this
     makes testing easier */
  device_tree_add_found_device(root,
			       (bfd_get_start_address(image) == 0
				? "/init/stack@elf"
				: "/init/stack@none"));
  name = printd_c("/init/load-binary", bfd_get_filename(image));
  device_tree_add_found_device(root, name);
  zfree(name);
}


/* Openboot model (under development):

   An extension of the hardware model.  The image is read into memory
   as a single block.  Sections of the image are then mapped as
   required using a HTAB. */

STATIC_INLINE_PSIM void
create_openboot_device_tree(bfd *image,
			    device_tree *root)
{
  create_hardware_device_tree(image, root);
}


/* User mode model:

   Image sections loaded into virtual addresses as specified.  A
   (large) stack is reserved (but only allocated as needed). System
   calls that include suport for heap growth are attached. */

STATIC_INLINE_PSIM void
create_vea_device_tree(bfd *image,
		       device_tree *root)
{
  unsigned_word top_of_stack;
  unsigned stack_size;
  int elf_binary;
  char *name;

  /* establish a few defaults */
  if (image->xvec->flavour == bfd_target_elf_flavour) {
    elf_binary = 1;
    top_of_stack = 0xe0000000;
    stack_size =   0x00100000;
  }
  else {
    elf_binary = 0;
    top_of_stack = 0x20000000;
    stack_size =   0x00100000;
  }

  /* options */
  device_tree_add_passthrough(root, "/options");
  device_tree_add_integer(root, "/options/smp", 1); /* always */
  device_tree_add_boolean(root, "/options/little-endian?",
			  !image->xvec->byteorder_big_p);
  device_tree_add_string(root, "/options/environment-architecture",
			 (WITH_ENVIRONMENT == USER_ENVIRONMENT
			  ? "user" : "virtual"));
  device_tree_add_boolean(root, "/options/strict-alignment?",
			  (WITH_ALIGNMENT == STRICT_ALIGNMENT
			    || !image->xvec->byteorder_big_p));
  device_tree_add_boolean(root, "/options/floating-point?",
			  WITH_FLOATING_POINT);

  /* virtual memory - handles growth of stack/heap */
  name = printd_uw_u("/vm", top_of_stack - stack_size, stack_size);
  device_tree_add_found_device(root, name);
  zfree(name);
  name = printd_c("/vm/map-binary", bfd_get_filename(image));
  device_tree_add_found_device(root, name);
  zfree(name);

  /* finish the init */
  device_tree_add_passthrough(root, "/init");
  name = printd_c_uw("/init/register", "pc", bfd_get_start_address(image));
  device_tree_add_found_device(root, name); /*pc*/
  zfree(name);
  name = printd_c_uw("/init/register", "sp", top_of_stack);
  device_tree_add_found_device(root, name);
  zfree(name);
  name = printd_c_uw("/init/register", "msr",
		     (image->xvec->byteorder_big_p
		      ? 0
		      : msr_little_endian_mode));
  device_tree_add_found_device(root, name);
  zfree(name);
  device_tree_add_found_device(root, (elf_binary
				      ? "/init/stack@elf"
				      : "/init/stack@xcoff"));
}


/* File device:

   The file contains lines that specify the describe the device tree
   to be created, read them in and load them into the tree */

STATIC_INLINE_PSIM void
create_filed_device_tree(const char *file_name,
			 device_tree *root)
{
  FILE *description = fopen(file_name, "r");
  int line_nr = 0;
  char device_path[1000];
  while (fgets(device_path, sizeof(device_path), description)) {
    /* check all of line was read */
    {
      char *end = strchr(device_path, '\n');
      if (end == NULL) {
	fclose(description);
	error("create_filed_device_tree() line %d to long: %s\n",
	      line_nr, device_path);
      }
      line_nr++;
      *end = '\0';
    }
    /* check for leading comment */
    if (device_path[0] != '/')
      continue;
    /* enter it in varying ways */
    if (strchr(device_path, '@') != NULL) {
      device_tree_add_found_device(root, device_path);
    }
    else {
      char *space = strchr(device_path, ' ');
      if (space == NULL) {
	/* intermediate node */
	device_tree_add_passthrough(root, device_path);
      }
      else if (space[-1] == '?') {
	/* boolean */
	*space = '\0';
	device_tree_add_boolean(root, device_path, space[1] != '0');
      }
      else if (isdigit(space[1])) {
	/* integer */
	*space = '\0';
	device_tree_add_integer(root, device_path, strtoul(space+1, 0, 0));
      }
      else if (space[1] == '"') {
	/* quoted string */
	char *end = strchr(space+2, '\0');
	if (end[-1] == '"')
	  end[-1] = '\0';
	*space = '\0';
	device_tree_add_string(root, device_path, space + 2);
      }
      else {
	/* any thing else */
	space = '\0';
	device_tree_add_string(root, device_path, space + 1);
      }
    }
  }
  fclose(description);
}


/* Given the file containing the `image', create a device tree that
   defines the machine to be modeled */

STATIC_INLINE_PSIM device_tree *
create_device_tree(const char *file_name,
		   core *memory)
{
  bfd *image;
  const device *core_device = core_device_create(memory);
  device_tree *root = device_tree_add_device(NULL, "/", core_device);

  bfd_init(); /* could be redundant but ... */

  /* open the file */
  image = bfd_openr(file_name, NULL);
  if (image == NULL) {
    bfd_perror("open failed:");
    error("nothing loaded\n");
  }

  /* check it is valid */
  if (!bfd_check_format(image, bfd_object)) {
    printf_filtered("create_device_tree() - FIXME - should check more bfd bits\n");
    printf_filtered("create_device_tree() - %s not an executable, assume device file\n", file_name);
    bfd_close(image);
    image = NULL;
  }

  /* depending on what was found about the file, load it */
  if (image != NULL) {
    if (bfd_get_start_address(image) < OEA_START_ADDRESS) {
      TRACE(trace_device_tree, ("create_device_tree() - hardware image\n"));
      create_hardware_device_tree(image, root);
    }
    else if (bfd_get_start_address(image) < OPENBOOT_START_ADDRESS) {
      TRACE(trace_device_tree, ("create_device_tree() - vea image\n"));
      create_vea_device_tree(image, root);
    }
    else {
      TRACE(trace_device_tree, ("create_device_tree() - openboot? image\n"));
      create_openboot_device_tree(image, root);
    }
    bfd_close(image);
  }
  else {
    TRACE(trace_device_tree, ("create_device_tree() - text image\n"));
    create_filed_device_tree(file_name, root);
  }

  return root;
}



INLINE_PSIM psim *
psim_create(const char *file_name)
{
  int cpu_nr;
  const char *env;
  psim *system;

  /* create things */
  system = ZALLOC(psim);
  system->events = event_queue_create();
  system->memory = core_create();
  system->monitor = mon_create();
  system->devices = create_device_tree(file_name, system->memory);
  for (cpu_nr = 0; cpu_nr < MAX_NR_PROCESSORS; cpu_nr++) {
    system->processors[cpu_nr] = cpu_create(system,
					    system->memory,
					    system->events,
					    mon_cpu(system->monitor,
						    cpu_nr),
					    cpu_nr);
  }

  /* fill in the missing real number of CPU's */
  system->nr_cpus = device_tree_find_integer(system->devices,
					     "/options/smp");

  /* fill in the missing TARGET BYTE ORDER information */
  current_target_byte_order = (device_tree_find_boolean(system->devices,
							"/options/little-endian?")
			       ? LITTLE_ENDIAN
			       : BIG_ENDIAN);
  if (CURRENT_TARGET_BYTE_ORDER != current_target_byte_order)
    error("target byte order conflict\n");

  /* fill in the missing HOST BYTE ORDER information */
  current_host_byte_order = (current_host_byte_order = 1,
			     (*(char*)(&current_host_byte_order)
			      ? LITTLE_ENDIAN
			      : BIG_ENDIAN));
  if (CURRENT_HOST_BYTE_ORDER != current_host_byte_order)
    error("host byte order conflict\n");

  /* fill in the missing OEA/VEA information */
  env = device_tree_find_string(system->devices,
				"/options/environment-architecture");
  current_environment = (strcmp(env, "user") == 0
			 ? USER_ENVIRONMENT
			 : strcmp(env, "virtual") == 0
			 ? VIRTUAL_ENVIRONMENT
			 : strcmp(env, "operating") == 0
			 ? OPERATING_ENVIRONMENT
			 : 0);
  if (current_environment == 0)
    error("unreconized /options/environment-architecture\n");
  if (CURRENT_ENVIRONMENT != current_environment)
    error("target environment conflict\n");

  /* fill in the missing ALLIGNMENT information */
  current_alignment = (device_tree_find_boolean(system->devices,
						"/options/strict-alignment?")
		       ? STRICT_ALIGNMENT
		       : NONSTRICT_ALIGNMENT);
  if (CURRENT_ALIGNMENT != current_alignment)
    error("target alignment conflict\n");

  /* fill in the missing FLOATING POINT information */
  current_floating_point = (device_tree_find_boolean(system->devices,
						     "/options/floating-point?")
			    ? HARD_FLOATING_POINT
			    : SOFT_FLOATING_POINT);
  if (CURRENT_FLOATING_POINT != current_floating_point)
    error("target floating-point conflict\n");

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


const device *
psim_device(psim *system,
	    const char *path)
{
  return device_tree_find_device(system->devices, path);
}



INLINE_PSIM void
psim_init(psim *system)
{
  int cpu_nr;

  /* scrub the monitor */
  mon_init(system->monitor, system->nr_cpus);

  /* scrub all the cpus */
  for (cpu_nr = 0; cpu_nr < system->nr_cpus; cpu_nr++)
    cpu_init(system->processors[cpu_nr]);

  /* init all the devices */
  device_tree_init(system->devices, system);

  /* force loop to restart */
  system->last_cpu = system->nr_cpus - 1;
}

INLINE_PSIM void
psim_stack(psim *system,
	   char **argv,
	   char **envp)
{
  /* pass the stack device the argv/envp and let it work out what to
     do with it */
  const device *stack_device = device_tree_find_device(system->devices,
						       "/init/stack");
  unsigned_word stack_pointer;
  psim_read_register(system, 0, &stack_pointer, "sp", cooked_transfer);
  stack_device->callback->ioctl(stack_device,
				system,
				NULL, /*cpu*/
				0, /*cia*/
				stack_pointer,
				argv,
				envp);
}



/* EXECUTE REAL CODE: 

   Unfortunatly, there are multiple cases to consider vis:

   	<icache> X <smp> X <events> X <keep-running-flag> X ...

   Consequently this function is written in multiple different ways */

STATIC_INLINE_PSIM void
run_until_stop(psim *system,
	       volatile int *keep_running)
{
  jmp_buf halt;
  jmp_buf restart;
  int cpu_nr;
#if WITH_IDECODE_CACHE_SIZE
  for (cpu_nr = 0; cpu_nr < system->nr_cpus; cpu_nr++)
    cpu_flush_icache(system->processors[cpu_nr]);
#endif
  psim_set_halt_and_restart(system, &halt, &restart);

#if (!WITH_IDECODE_CACHE_SIZE && WITH_SMP == 0)

  /* CASE 1: No instruction cache and no SMP.

     In this case, we can take advantage of the fact that the current
     instruction address does not need to be returned to the cpu
     object after every execution of an instruction.  Instead it only
     needs to be saved when either A. the main loop exits or B. a
     cpu-{halt,restart} call forces the loop to be re-entered.  The
     later functions always save the current cpu instruction
     address. */

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
#endif


#if (WITH_IDECODE_CACHE_SIZE && WITH_SMP == 0)

  /* CASE 2: Instruction case but no SMP

     Here, the additional complexity comes from there being two
     different cache implementations.  A simple function address cache
     or a full cracked instruction cache */

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
	    idecode_cache *const cache_entry = cpu_icache_entry(processor,
								cia);
	    if (cache_entry->address == cia) {
	      idecode_semantic *const semantic = cache_entry->semantic;
	      cia = semantic(processor, cache_entry, cia);
	    }
	    else {
	      instruction_word const instruction
		= vm_instruction_map_read(cpu_instruction_map(processor),
					  processor,
					  cia);
	      idecode_semantic *const semantic = idecode(processor,
							 instruction,
							 cia,
							 cache_entry);
	      cache_entry->address = cia;
	      cache_entry->semantic = semantic;
	      cia = semantic(processor, cache_entry, cia);
	    }
	  }
	} while (keep_running == NULL || *keep_running);
	cpu_set_program_counter(processor, cia);
      }
    } while(keep_running == NULL || *keep_running);
  }
#endif


#if (!WITH_IDECODE_CACHE_SIZE && WITH_SMP > 0)

  /* CASE 3: No ICACHE but SMP

     The complexity here comes from needing to correctly restart the
     system when it is aborted.  In particular if cpu0 requests a
     restart, the next cpu is still cpu1.  Cpu0 being restarted after
     all the other CPU's and the event queue have been processed */

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
#endif

#if (WITH_IDECODE_CACHE_SIZE && WITH_SMP > 0)

  /* CASE 4: ICACHE and SMP ...

     This time, everything goes wrong.  Need to restart loops
     correctly, need to save the program counter and finally need to
     keep track of each processors current address! */

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
	  idecode_cache *cache_entry = cpu_icache_entry(processor, cia);
	  if (cache_entry->address == cia) {
	    idecode_semantic *semantic = cache_entry->semantic;
	    cpu_set_program_counter(processor,
				    semantic(processor, cache_entry, cia));
	  }
	  else {
	    instruction_word instruction =
	      vm_instruction_map_read(cpu_instruction_map(processor),
				      processor,
				      cia);
	    idecode_semantic *semantic = idecode(processor,
						 instruction,
						 cia,
						 cache_entry);
	    cache_entry->address = cia;
	    cache_entry->semantic = semantic;
	    cpu_set_program_counter(processor,
				    semantic(processor, cache_entry, cia));
	  }
	}
	if (!(keep_running == NULL || *keep_running))
	  break;
      }
    } while (keep_running == NULL || *keep_running);
  }
#endif

  psim_clear_halt_and_restart(system);
}


/* SIMULATE INSTRUCTIONS, various different ways of achieving the same
   thing */

INLINE_PSIM void
psim_step(psim *system)
{
  volatile int keep_running = 0;
  run_until_stop(system, &keep_running);
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
  if (which_cpu == MAX_NR_PROCESSORS)
    which_cpu = system->last_cpu;
  if (which_cpu < 0 || which_cpu >= system->nr_cpus)
    error("psim_read_register() - invalid processor %d\n", which_cpu);
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
  if (which_cpu == MAX_NR_PROCESSORS)
    which_cpu = system->last_cpu;
  if (which_cpu == -1) {
    int i;
    for (i = 0; i < system->nr_cpus; i++)
      psim_write_register(system, i, buf, reg, mode);
    return;
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
		 unsigned nr_bytes)
{
  cpu *processor;
  if (which_cpu == MAX_NR_PROCESSORS)
    which_cpu = system->last_cpu;
  if (which_cpu < 0 || which_cpu >= system->nr_cpus)
    error("psim_read_memory() invalid cpu\n");
  processor = system->processors[which_cpu];
  return vm_data_map_read_buffer(cpu_data_map(processor),
				 buffer, vaddr, nr_bytes);
}


INLINE_PSIM unsigned
psim_write_memory(psim *system,
		  int which_cpu,
		  const void *buffer,
		  unsigned_word vaddr,
		  unsigned nr_bytes,
		  int violate_read_only_section)
{
  cpu *processor;
  if (which_cpu == MAX_NR_PROCESSORS)
    which_cpu = system->last_cpu;
  if (which_cpu < 0 || which_cpu >= system->nr_cpus)
    error("psim_read_memory() invalid cpu\n");
  processor = system->processors[which_cpu];
  return vm_data_map_write_buffer(cpu_data_map(processor),
				  buffer, vaddr, nr_bytes, 1);
}


INLINE_PSIM void
psim_print_info(psim *system,
		int verbose)
{
  mon_print_info(system->monitor, verbose);
}


#endif /* _PSIM_C_ */
