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


#ifndef _DEVICES_C_
#define _DEVICES_C_

#ifndef STATIC_INLINE_DEVICES
#define STATIC_INLINE_DEVICES STATIC_INLINE
#endif

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <stdarg.h>

#include "basics.h"
#include "devices.h"
#include "events.h"

#include "cpu.h"

#include "bfd.h"

/* Helper functions */

/* Generic device init: Attaches the device of size <nr_bytes> (taken
   from <name>@<int>,<nr_bytes>) to its parent at address zero and
   with read/write access. */

STATIC_INLINE_DEVICES void
generic_init_callback(const device *me,
		      psim *system)
{
  unsigned_word addr;
  unsigned nr_bytes;
  if (scand_uw_u(me->name, &addr, &nr_bytes) != 2)
    error("generic_init_callback() invalid nr_bytes in %s\n", me->name);
  me->parent->callback->attach_address(me->parent,
				       me->name,
				       attach_callback,
				       0 /*space*/,
				       addr,
				       nr_bytes,
				       access_read_write,
				       me);
}



/* inimplemented versions of each function */

INLINE_DEVICES void
unimp_device_init(const device *me,
		  psim *system)
{
  error("device_init_callback for %s not implemented\n", me->name);
}

INLINE_DEVICES void
unimp_device_attach_address(const device *me,
			    const char *name,
			    attach_type type,
			    int space,
			    unsigned_word addr,
			    unsigned nr_bytes,
			    access_type access,
			    const device *who) /*callback/default*/
{
  error("device_attach_address_callback for %s not implemented\n", me->name);
}

INLINE_DEVICES void
unimp_device_detach_address(const device *me,
			    const char *name,
			    attach_type type,
			    int space,
			    unsigned_word addr,
			    unsigned nr_bytes,
			    access_type access,
			    const device *who) /*callback/default*/
{
  error("device_detach_address_callback for %s not implemented\n", me->name);
}

INLINE_DEVICES unsigned
unimp_device_io_read_buffer(const device *me,
			    void *dest,
			    int space,
			    unsigned_word addr,
			    unsigned nr_bytes,
			    cpu *processor,
			    unsigned_word cia)
{
  error("device_io_read_buffer_callback for %s not implemented\n", me->name);
  return 0;
}

INLINE_DEVICES unsigned
unimp_device_io_write_buffer(const device *me,
			     const void *source,
			     int space,
			     unsigned_word addr,
			     unsigned nr_bytes,
			     cpu *processor,
			     unsigned_word cia)
{
  error("device_io_write_buffer_callback for %s not implemented\n", me->name);
  return 0;
}

INLINE_DEVICES unsigned
unimp_device_dma_read_buffer(const device *me,
			     void *target,
			     int space,
			     unsigned_word addr,
			     unsigned nr_bytes)
{
  error("device_dma_read_buffer_callback for %s not implemented\n", me->name);
  return 0;
}

INLINE_DEVICES unsigned
unimp_device_dma_write_buffer(const device *me,
			      const void *source,
			      int space,
			      unsigned_word addr,
			      unsigned nr_bytes,
			      int violate_read_only_section)
{
  error("device_dma_write_buffer_callback for %s not implemented\n", me->name);
  return 0;
}

INLINE_DEVICES void
unimp_device_attach_interrupt(const device *me,
			      const device *who,
			      int interrupt_line,
			      const char *name)
{
  error("device_attach_interrupt_callback for %s not implemented\n", me->name);
}

INLINE_DEVICES void
unimp_device_detach_interrupt(const device *me,
			      const device *who,
			      int interrupt_line,
			      const char *name)
{
  error("device_detach_interrupt_callback for %s not implemented\n", me->name);
}

INLINE_DEVICES void
unimp_device_interrupt(const device *me,
		       const device *who,
		       int interrupt_line,
		       int interrupt_status,
		       cpu *processor,
		       unsigned_word cia)
{
  error("device_interrupt_callback for %s not implemented\n", me->name);
}

INLINE_DEVICES void
unimp_device_interrupt_ack(const device *me,
			   int interrupt_line,
			   int interrupt_status)
{
  error("device_interrupt_ack_callback for %s not implemented\n", me->name);
}

STATIC_DEVICES void
unimp_device_ioctl(const device *me,
		   psim *system,
		   cpu *processor,
		   unsigned_word cia,
		   ...)
{
  error("device_ioctl_callback for %s not implemented\n", me->name);
}



/* ignore/passthrough versions of each function */

INLINE_DEVICES void
ignore_device_init(const device *me,
		   psim *system)
{
  /*null*/
}

INLINE_DEVICES void
pass_device_attach_address(const device *me,
			   const char *name,
			   attach_type attach,
			   int space,
			   unsigned_word addr,
			   unsigned nr_bytes,
			   access_type access,
			   const device *who) /*callback/default*/
{
  DTRACE_ATTACH_ADDRESS(pass);
  me->parent->callback->attach_address(me->parent, name, attach,
				       space, addr, nr_bytes,
				       access,
				       who);
}

INLINE_DEVICES void
pass_device_detach_address(const device *me,
			   const char *name,
			   attach_type attach,
			   int space,
			   unsigned_word addr,
			   unsigned nr_bytes,
			   access_type access,
			   const device *who) /*callback/default*/
{
  DTRACE_DETACH_ADDRESS(pass);
  me->parent->callback->detach_address(me->parent, name, attach,
				       space, addr, nr_bytes, access,
				       who);
}

INLINE_DEVICES unsigned
pass_device_dma_read_buffer(const device *me,
			    void *dest,
			    int space,
			    unsigned_word addr,
			    unsigned nr_bytes)
{
  DTRACE_DMA_READ_BUFFER(pass);
  return me->parent->callback->dma_read_buffer(me->parent, dest,
					       space, addr, nr_bytes);
}

INLINE_DEVICES unsigned
pass_device_dma_write_buffer(const device *me,
			     const void *source,
			     int space,
			     unsigned_word addr,
			     unsigned nr_bytes,
			     int violate_read_only_section)
{
  DTRACE_DMA_WRITE_BUFFER(pass);
  return me->parent->callback->dma_write_buffer(me->parent, source,
						space, addr,
						nr_bytes,
						violate_read_only_section);
}

INLINE_DEVICES void
pass_device_attach_interrupt(const device *me,
			     const device *who,
			     int interrupt_line,
			     const char *name)
{
  me->parent->callback->attach_interrupt(me->parent, who,
					 interrupt_line, name);
}

INLINE_DEVICES void
pass_device_detach_interrupt(const device *me,
			     const device *who,
			     int interrupt_line,
			     const char *name)
{
  me->parent->callback->detach_interrupt(me->parent, who,
					 interrupt_line, name);
}


INLINE_DEVICES void
pass_device_interrupt(const device *me,
		      const device *who,
		      int interrupt_line,
		      int interrupt_status,
		      cpu *processor,
		      unsigned_word cia)
{
  me->parent->callback->interrupt(me->parent, who,
				  interrupt_line, interrupt_status,
				  processor, cia);
}



/* Simple console device: console@)x<address>,16

   Input characters are taken from the keyboard, output characters
   sent to the terminal.  Echoing of characters is not disabled.

   The device has four registers:

   0x0: read
   0x4: read-status
   0x8: write
   0xC: write-status

   Where a nonzero status register indicates that the device is ready
   (input fifo contains a character or output fifo has space). */

typedef struct _console_buffer {
  char buffer;
  int status;
  event_entry_tag event_tag;
} console_buffer;

typedef struct _console_device {
  console_buffer input;
  console_buffer output;
} console_device;

typedef enum {
  console_read_buffer = 0,
  console_read_status = 4,
  console_write_buffer = 8,
  console_write_status = 12,
  console_offset_mask = 0xc,
  console_size = 16,
} console_offsets;


STATIC_INLINE_DEVICES unsigned
console_io_read_buffer_callback(const device *me,
				void *dest,
				int space,
				unsigned_word addr,
				unsigned nr_bytes,
				cpu *processor,
				unsigned_word cia)
{
  console_device *console = (console_device*)me->data;
  unsigned_1 val;
  DTRACE_IO_READ_BUFFER(console);

  /* determine what was read */

  switch (addr) {

  case console_read_buffer:
    val = console->input.buffer;
    break;

  case console_read_status:
    { /* check for input */
      int flags;
      int status;
      /* get the old status */
      flags = fcntl(0, F_GETFL, 0);
      if (flags == -1) {
	perror("console");
	val = 0;
	break;
      }
      /* temp, disable blocking IO */
      status = fcntl(0, F_SETFL, flags | O_NDELAY);
      if (status == -1) {
	perror("console");
	val = 0;
	break;
      }
      /* try for input */
      status = read(0, &console->input.buffer, 1);
      if (status == 1) {
	console->input.status = 1;
      }
      else {
	console->input.status = 0;
      }
      /* return to regular vewing */
      fcntl(0, F_SETFL, flags);
    }
    val = console->input.status;
    break;

  case console_write_buffer:
    val = console->output.buffer;
    break;

  case console_write_status:
    val = console->output.status;
    break;

  default:
    error("console_read_callback() internal error\n");
    val = 0;
    break;

  }

  bzero(dest, nr_bytes);
  *(unsigned_1*)dest = val;
  return nr_bytes;
}

STATIC_INLINE_DEVICES unsigned
console_io_write_buffer_callback(const device *me,
				 const void *source,
				 int space,
				 unsigned_word addr,
				 unsigned nr_bytes,
				 cpu *processor,
				 unsigned_word cia)
{
  console_device *console = (console_device*)me->data;
  unsigned_1 val = *(unsigned8*)source;
  DTRACE_IO_WRITE_BUFFER(console);

  switch (addr) {
  case console_read_buffer:
    console->input.buffer = val;
    break;
  case console_read_status:
    console->input.status = val;
    break;
  case console_write_buffer:
    DTRACE(console, ("<%c:%d>", val, val));
    printf_filtered("%c",val) ;
    console->output.buffer = val;
    console->output.status = 1;
    break;
  case console_write_status:
    console->output.status = val;
    break;
  default:
    error("console_write_callback() internal error\n");
  }
	 
  return nr_bytes;
}


static device_callbacks const console_callbacks = {
  generic_init_callback,
  unimp_device_attach_address,
  unimp_device_detach_address,
  console_io_read_buffer_callback,
  console_io_write_buffer_callback,
  unimp_device_dma_read_buffer,
  unimp_device_dma_write_buffer,
  unimp_device_attach_interrupt,
  unimp_device_detach_interrupt,
  unimp_device_interrupt,
  unimp_device_interrupt_ack,
  unimp_device_ioctl,
};


STATIC_INLINE_DEVICES const device *
console_create(const char *name,
	       const char *full_name,
	       const device *parent)
{
  /* create the descriptor */
  console_device *console = ZALLOC(console_device);

  /* fill in the details */
  console->output.status = 1;
  console->output.buffer = '\0';
  console->input.status = 0;
  console->input.buffer = '\0';

  /* insert into the device tree along with its address info */
  return device_create_from(name,
			    full_name,
			    console, /* data */
			    &console_callbacks,
			    parent);
}



/* ICU device: icu@0x<address>,4

   Single 4 byte register.  Read returns processor number.  Write
   interrupts specified processor.

   Illustrates passing of events to parent device. Passing of
   interrupts to parent bus.

   NB: For the sake of illustrating the passing of interrupts.  This
   device doesn't pass interrupt events to its parent.  Instead it
   passes them back to its self. */

STATIC_INLINE_DEVICES unsigned
icu_io_read_buffer_callback(const device *me,
			    void *dest,
			    int space,
			    unsigned_word addr,
			    unsigned nr_bytes,
			    cpu *processor,
			    unsigned_word cia)
{
  unsigned_1 val;
  DTRACE_IO_READ_BUFFER(icu);
  val = cpu_nr(processor);
  bzero(dest, nr_bytes);
  *(unsigned_1*)dest = val;
  return nr_bytes;
}


STATIC_INLINE_DEVICES unsigned
icu_io_write_buffer_callback(const device *me,
			     const void *source,
			     int space,
			     unsigned_word addr,
			     unsigned nr_bytes,
			     cpu *processor,
			     unsigned_word cia)
{
  psim *system = cpu_system(processor);
  unsigned_1 val = H2T_1(*(unsigned_1*)source);
  DTRACE_IO_WRITE_BUFFER(icu);
  /* tell the parent device that the interrupt lines have changed.
     For this fake ICU.  The interrupt lines just indicate the cpu to
     interrupt next */
  me->parent->callback->interrupt(me->parent, me,
				  val, val,
				  processor, cia);
  return nr_bytes;
}


static device_callbacks const icu_callbacks = {
  generic_init_callback,
  unimp_device_attach_address,
  unimp_device_detach_address,
  icu_io_read_buffer_callback,
  icu_io_write_buffer_callback,
  unimp_device_dma_read_buffer,
  unimp_device_dma_write_buffer,
  unimp_device_attach_interrupt,
  unimp_device_detach_interrupt,
  unimp_device_interrupt,
  unimp_device_interrupt_ack,
  unimp_device_ioctl,
};



/* HALT device: halt@0x<address>,4

   With real hardware, the processor operation is normally terminated
   through a reset.  This device illustrates how a reset device could
   be attached to an address */


STATIC_INLINE_DEVICES unsigned
halt_io_read_buffer_callback(const device *me,
			     void *dest,
			     int space,
			     unsigned_word addr,
			     unsigned nr_bytes,
			     cpu *processor,
			     unsigned_word cia)
{
  DTRACE_IO_READ_BUFFER(halt);
  cpu_halt(processor, cia, was_exited, 0);
  return 0;
}


STATIC_INLINE_DEVICES unsigned
halt_io_write_buffer_callback(const device *me,
			      const void *source,
			      int space,
			      unsigned_word addr,
			      unsigned nr_bytes,
			      cpu *processor,
			      unsigned_word cia)
{
  DTRACE_IO_WRITE_BUFFER(halt);
  cpu_halt(processor, cia, was_exited, 0);
  return 0;
}


static device_callbacks const halt_callbacks = {
  generic_init_callback,
  unimp_device_attach_address,
  unimp_device_detach_address,
  halt_io_read_buffer_callback,
  halt_io_write_buffer_callback,
  unimp_device_dma_read_buffer,
  unimp_device_dma_write_buffer,
  unimp_device_attach_interrupt,
  unimp_device_detach_interrupt,
  unimp_device_interrupt,
  unimp_device_interrupt_ack,
  unimp_device_ioctl,
};



/* Register init device: register@<name>,0x<value>[,<processor>]

   This strange device is used to initialize the processors registers
   as part of the initialization. */

STATIC_INLINE_DEVICES void
register_init_callback(const device *me,
		       psim *system)
{
  char name[100];
  unsigned_word value;
  unsigned which_cpu;
  int status;
  DTRACE_INIT(register);
  status = scand_c_uw_u(me->name, name, sizeof(name), &value, &which_cpu);
  switch (status) {
  case 2: /* register@<name>,<value> */
    psim_write_register(system, -1, &value, name, cooked_transfer);
    break;
  case 3: /* register@<name>,<value>,<processor> */
    psim_write_register(system, which_cpu, &value, name, cooked_transfer);
    break;
  default:
    error("register_init_callback() invalid register init %s\n", me->name);
    break;
  }
}


static device_callbacks const register_callbacks = {
  register_init_callback,
  unimp_device_attach_address,
  unimp_device_detach_address,
  unimp_device_io_read_buffer,
  unimp_device_io_write_buffer,
  unimp_device_dma_read_buffer,
  unimp_device_dma_write_buffer,
  unimp_device_attach_interrupt,
  unimp_device_detach_interrupt,
  unimp_device_interrupt,
  unimp_device_interrupt_ack,
  unimp_device_ioctl,
};



/* VEA VM device: vm@0x<stack-base>,<nr_bytes>

   A VEA mode device. This sets its self up as the default memory
   device capturing all accesses (reads/writes) to currently unmapped
   addresses.  If the unmaped access falls within unallocated stack or
   heap address ranges then memory is allocated and the access is
   allowed to continue.

   During init phase, this device expects to receive `attach' requests
   from its children for the text/data/bss memory areas.  Typically,
   this would be done by the binary device.

   STACK: The location of the stack in memory is specified as part of
   the devices name.  Unmaped accesses that fall within the stack
   space result in the allocated stack being grown downwards so that
   it includes the page of the culprit access.

   HEAP: During initialization, the vm device monitors all `attach'
   operations from its children using this to determine the initial
   location of the heap.  The heap is then extended by system calls
   that frob the heap upper bound variable (see system.c). */


typedef struct _vm_device {
  /* area of memory valid for stack addresses */
  unsigned_word stack_base; /* min possible stack value */
  unsigned_word stack_bound;
  unsigned_word stack_lower_limit;
  /* area of memory valid for heap addresses */
  unsigned_word heap_base;
  unsigned_word heap_bound;
  unsigned_word heap_upper_limit;
} vm_device;


STATIC_INLINE_DEVICES void
vm_init_callback(const device *me,
		 psim *system)
{
  vm_device *vm = (vm_device*)me->data;
  DTRACE_INIT(vm);

  /* revert the stack/heap variables to their defaults */
  vm->stack_lower_limit = vm->stack_bound;
  vm->heap_base = 0;
  vm->heap_bound = 0;
  vm->heap_upper_limit = 0;

  /* establish this device as the default memory handler */
  me->parent->callback->attach_address(me->parent,
				       me->name,
				       attach_default,
				       0 /*address space - ignore*/,
				       0 /*addr - ignore*/,
				       0 /*nr_bytes - ignore*/,
				       access_read_write /*access*/,
				       me);
}


STATIC_INLINE_DEVICES void
vm_attach_address(const device *me,
		  const char *name,
		  attach_type attach,
		  int space,
		  unsigned_word addr,
		  unsigned nr_bytes,
		  access_type access,
		  const device *who) /*callback/default*/
{
  vm_device *vm = (vm_device*)me->data;
  DTRACE_ATTACH_ADDRESS(vm);
  /* update end of bss if necessary */
  if (vm->heap_base < addr + nr_bytes) {
    vm->heap_base = addr + nr_bytes;
    vm->heap_bound = addr + nr_bytes;
    vm->heap_upper_limit = addr + nr_bytes;
  }
  me->parent->callback->attach_address(me->parent,
				       "vm@0x0,0", /* stop remap */
				       attach_raw_memory,
				       0 /*address space*/,
				       addr,
				       nr_bytes,
				       access,
				       me);
}


STATIC_INLINE_DEVICES unsigned
add_vm_space(const device *me,
	     unsigned_word addr,
	     unsigned nr_bytes,
	     cpu *processor,
	     unsigned_word cia)
{
  vm_device *vm = (vm_device*)me->data;
  unsigned_word block_addr;
  unsigned block_nr_bytes;

  /* an address in the stack area, allocate just down to the addressed
     page */
  if (addr >= vm->stack_base && addr < vm->stack_lower_limit) {
    block_addr = FLOOR_PAGE(addr);
    block_nr_bytes = vm->stack_lower_limit - block_addr;
    vm->stack_lower_limit = block_addr;
  }
  /* an address in the heap area, allocate all of the required heap */
  else if (addr >= vm->heap_upper_limit && addr < vm->heap_bound) {
    block_addr = vm->heap_upper_limit;
    block_nr_bytes = vm->heap_bound - vm->heap_upper_limit;
    vm->heap_upper_limit = vm->heap_bound;
  }
  /* oops - an invalid address - abort the cpu */
  else if (processor != NULL) {
    cpu_halt(processor, cia, was_signalled, SIGSEGV);
    return 0;
  }
  /* 2*oops - an invalid address and no processor */
  else {
    return 0;
  }

  /* got the parameters, allocate the space */
  me->parent->callback->attach_address(me->parent,
				       "vm@0x0,0", /* stop remap */
				       attach_raw_memory,
				       0 /*address space*/,
				       block_addr,
				       block_nr_bytes,
				       access_read_write,
				       me);
  return block_nr_bytes;
}


STATIC_INLINE_DEVICES unsigned
vm_io_read_buffer_callback(const device *me,
			   void *dest,
			   int space,
			   unsigned_word addr,
			   unsigned nr_bytes,
			   cpu *processor,
			   unsigned_word cia)
{
  DTRACE_IO_READ_BUFFER(vm);
  if (add_vm_space(me, addr, nr_bytes, processor, cia) >= nr_bytes) {
    bzero(dest, nr_bytes); /* always initialized to zero */
    return nr_bytes;
  }
  else 
    return 0;
}


STATIC_INLINE_DEVICES unsigned
vm_io_write_buffer_callback(const device *me,
			    const void *source,
			    int space,
			    unsigned_word addr,
			    unsigned nr_bytes,
			    cpu *processor,
			    unsigned_word cia)
{
  DTRACE_IO_WRITE_BUFFER(vm);
  if (add_vm_space(me, addr, nr_bytes, processor, cia) >= nr_bytes) {
    return me->parent->callback->dma_write_buffer(me->parent, source,
						  space, addr,
						  nr_bytes,
						  0/*violate_read_only*/);
  }
  else
    return 0;
}


static void
vm_ioctl_callback(const device *me,
		  psim *system,
		  cpu *processor,
		  unsigned_word cia,
		  ...)
{
  /* While the caller is notified that the heap has grown by the
     requested amount, the heap is infact extended out to a page
     boundary. */
  vm_device *vm = (vm_device*)me->data;
  unsigned_word new_break = ALIGN_8(cpu_registers(processor)->gpr[3]);
  unsigned_word old_break = vm->heap_bound;
  signed_word delta = new_break - old_break;
  if (delta > 0)
    vm->heap_bound = ALIGN_PAGE(new_break);
  cpu_registers(processor)->gpr[0] = 0;
  cpu_registers(processor)->gpr[3] = new_break;
}


static device_callbacks const vm_callbacks = {
  vm_init_callback,
  vm_attach_address,
  pass_device_detach_address,
  vm_io_read_buffer_callback,
  vm_io_write_buffer_callback,
  unimp_device_dma_read_buffer,
  pass_device_dma_write_buffer,
  unimp_device_attach_interrupt,
  unimp_device_detach_interrupt,
  unimp_device_interrupt,
  unimp_device_interrupt_ack,
  vm_ioctl_callback,
};


STATIC_INLINE_DEVICES const device *
vea_vm_create(const char *name,
	      const char *full_name,
	      const device *parent)
{
  vm_device *vm = ZALLOC(vm_device);
  unsigned_word addr;
  unsigned nr_bytes;

  /* extract out the stack parameters */
  if (scand_uw_u(name, &addr, &nr_bytes) != 2)
    error("vm_device_create() invalid vm device %s\n", name);
  vm->stack_base = addr;
  vm->stack_bound = addr + nr_bytes;

  /* insert in the tree including the buffer */
  return device_create_from(name,
			    full_name,
			    vm, /* data */
			    &vm_callbacks,
			    parent);
}



/* Memory init device: memory@0x<addr>,<size>,<access>

   This strange device is used create sections of memory */

STATIC_INLINE_DEVICES void
memory_init_callback(const device *me,
		     psim *system)
{
  unsigned_word addr;
  unsigned nr_bytes;
  unsigned access;
  int nr_args;
  DTRACE_INIT(memory);

  nr_args = scand_uw_u_u(me->name, &addr, &nr_bytes, &access);
  switch (nr_args) {
  case 2:
    access = access_read_write_exec;
    break;
  case 3:
    break;
  default:
    error("memory_init_callback() invalid memory device %s\n", me->name);
    break;
  }

  me->parent->callback->attach_address(me->parent,
				       me->name,
				       attach_raw_memory,
				       0 /*address space*/,
				       addr,
				       nr_bytes,
				       (access_type)access,
				       me);
}


static device_callbacks const memory_callbacks = {
  memory_init_callback,
  unimp_device_attach_address,
  unimp_device_detach_address,
  unimp_device_io_read_buffer,
  unimp_device_io_write_buffer,
  unimp_device_dma_read_buffer,
  unimp_device_dma_write_buffer,
  unimp_device_attach_interrupt,
  unimp_device_detach_interrupt,
  unimp_device_interrupt,
  unimp_device_interrupt_ack,
  unimp_device_ioctl,
};



/* IOBUS device: iobus@<address>

   Simple bus on which some IO devices live */

STATIC_INLINE_DEVICES void
iobus_attach_address_callback(const device *me,
			      const char *name,
			      attach_type type,
			      int space,
			      unsigned_word addr,
			      unsigned nr_bytes,
			      access_type access,
			      const device *who) /*callback/default*/
{
  unsigned_word iobus_addr;
  /* sanity check */
  if (type == attach_default)
    error("iobus_attach_address_callback() no default for %s/%s\n",
	  me->name, name);
  if (space != 0)
    error("iobus_attach_address_callback() no space for %s/%s\n",
	  me->name, name);
  /* get the bus address */
  if (scand_uw(me->name, &iobus_addr) != 1)
    error("iobus_attach_address_callback() invalid address for %s\n",
	  me->name);
  me->parent->callback->attach_address(me->parent,
				       me->name,
				       type,
				       0 /*space*/,
				       iobus_addr + addr,
				       nr_bytes,
				       access,
				       who);
}


STATIC_INLINE_DEVICES void
iobus_do_interrupt(event_queue *queue,
		   void *data)
{
  cpu *target = (cpu*)data;
  /* try to interrupt the processor.  If the attempt fails, try again
     on the next tick */
  if (!external_interrupt(target))
    event_queue_schedule(queue, 1, iobus_do_interrupt, target);
}


STATIC_INLINE_DEVICES void
iobus_interrupt_callback(const device *me,
			 const device *who,
			 int interrupt_line,
			 int interrupt_status,
			 cpu *processor,
			 unsigned_word cia)
{
  /* the interrupt controler can't interrupt a cpu at any time.
     Rather it must synchronize with the system clock before
     performing an interrupt on the given processor */
  psim *system = cpu_system(processor);
  cpu *target = psim_cpu(system, interrupt_status);
  if (target != NULL) {
    event_queue *events = cpu_event_queue(target);
    event_queue_schedule(events, 1, iobus_do_interrupt, target);
  }
}


static device_callbacks const iobus_callbacks = {
  ignore_device_init,
  iobus_attach_address_callback,
  unimp_device_detach_address,
  unimp_device_io_read_buffer,
  unimp_device_io_write_buffer,
  unimp_device_dma_read_buffer,
  unimp_device_dma_write_buffer,
  unimp_device_attach_interrupt,
  unimp_device_detach_interrupt,
  iobus_interrupt_callback,
  unimp_device_interrupt_ack,
  unimp_device_ioctl,
};



/* FILE device: file@0x<address>,<file-name>
   (later - file@0x<address>,<size>,<file-offset>,<file-name>)

   Specifies a file to read directly into memory starting at <address> */


STATIC_INLINE_DEVICES void
file_init_callback(const device *me,
		   psim *system)
{
  unsigned_word addr;
  unsigned count;
  char *file_name;
  char buf;
  FILE *image;
  DTRACE_INIT(file);

  if ((file_name = strchr(me->name, ',')) == NULL
      || scand_uw(me->name, &addr) != 1)
    error("file_init_callback() invalid file device %s\n", me->name);

  /* open the file to load */
  file_name++; /* skip the `,' */
  image = fopen(file_name, "r");
  if (image == NULL)
    error("file_init_callback() file open failed for %s\n", me->name);

  /* read it in slowly */
  count = 0;
  while (fread(&buf, 1, 1, image) > 0) {
    if (me->parent->callback->dma_write_buffer(me->parent,
					       &buf,
					       0 /*address-space*/,
					       addr+count,
					       1 /*nr-bytes*/,
					       1 /*violate ro*/) != 1)
      error("file_init_callback() failed to write to address 0x%x, offset %d\n",
	    addr+count, count);
    count++;
  }

  /* close down again */
  fclose(image);
}


static device_callbacks const file_callbacks = {
  file_init_callback,
  unimp_device_attach_address,
  unimp_device_detach_address,
  unimp_device_io_read_buffer,
  unimp_device_io_write_buffer,
  unimp_device_dma_read_buffer,
  unimp_device_dma_write_buffer,
  unimp_device_attach_interrupt,
  unimp_device_detach_interrupt,
  unimp_device_interrupt,
  unimp_device_interrupt_ack,
  unimp_device_ioctl,
};



/* HTAB: htab@0x<address>,<nr_bytes>
   PTE: pte@0x<effective-address>,0x<real-address>,<nr_bytes>

   HTAB defines the location (in physical memory) of a HASH table.
   PTE (as a child of HTAB) defines a mapping that is to be entered
   into that table.

   NB: All the work in this device is done during init by the PTE.
   The pte, looks up its parent to determine the address of the HTAB
   and then uses DMA calls to establish the required mapping. */


STATIC_INLINE_DEVICES void
htab_init_callback(const device *me,
		   psim *system)
{
  DTRACE_INIT(htab);
  /* only the pte does work */
  if (strncmp(me->name, "pte@", strlen("pte@")) == 0) {
    unsigned_word htab_ra;
    unsigned htab_nr_bytes;
    unsigned_word pte_ea;
    unsigned_word pte_ra;
    unsigned pte_nr_bytes;
    /* determine the location/size of the hash table */
    if (scand_uw_u(me->parent->name, &htab_ra, &htab_nr_bytes) != 2)
      error("htab_init_callback() htab entry %s invalid\n",
	    me->parent->name);
    /* determine the location/size of the mapping */
    if (scand_uw_uw_u(me->name, &pte_ea, &pte_ra, &pte_nr_bytes) != 3)
      error("htab_init_callback() pte entry %s invalid\n", me->name);
    error("Map ea=0x%x, ra=0x%x, nr_bytes=%d using htab=0x%x, nr_bytes=%d\n",
	  pte_ea, pte_ra, pte_nr_bytes, htab_ra, htab_nr_bytes);
  }
}


static device_callbacks const htab_callbacks = {
  htab_init_callback,
  unimp_device_attach_address,
  unimp_device_detach_address,
  unimp_device_io_read_buffer,
  unimp_device_io_write_buffer,
  unimp_device_dma_read_buffer,
  unimp_device_dma_write_buffer,
  unimp_device_attach_interrupt,
  unimp_device_detach_interrupt,
  unimp_device_interrupt,
  unimp_device_interrupt_ack,
  unimp_device_ioctl,
};



/* Simulator device: sim@0x<address>,<nr_bytes>

   Eventually gives access to the hardware configuration.  For
   instance, it could allow the setting (on the fly) of variables such
   as hardware floating-point or strict-alignment.

   It's intended use is as part of testing the simulators
   functionality */

static device_callbacks const sim_callbacks = {
  ignore_device_init,
  unimp_device_attach_address,
  unimp_device_detach_address,
  unimp_device_io_read_buffer,
  unimp_device_io_write_buffer,
  unimp_device_dma_read_buffer,
  unimp_device_dma_write_buffer,
  unimp_device_attach_interrupt,
  unimp_device_detach_interrupt,
  unimp_device_interrupt,
  unimp_device_interrupt_ack,
  unimp_device_ioctl,
};



/* Load device: *binary@<file-name>

   Assuming that <file-name> is an executable file understood by BFD,
   this device loads or maps the relevant text/data segments into
   memory using dma. */

STATIC_INLINE_DEVICES void
update_for_binary_section(bfd *abfd,
			  asection *the_section,
			  PTR obj)
{
  unsigned_word section_vma;
  unsigned_word section_size;
  access_type access;
  device *me = (device*)obj;

  /* skip the section if no memory to allocate */
  if (! (bfd_get_section_flags(abfd, the_section) & SEC_ALLOC))
    return;

  /* check/ignore any sections of size zero */
  section_size = bfd_get_section_size_before_reloc(the_section);
  if (section_size == 0)
    return;

  /* find where it is to go */
  section_vma = bfd_get_section_vma(abfd, the_section);

  DTRACE(binary,
	 ("name=%-7s, vma=0x%.8x, size=%6d, flags=%3x(%s%s%s%s )\n",
	  bfd_get_section_name(abfd, the_section),
	  section_vma, section_size,
	  bfd_get_section_flags(abfd, the_section),
	  bfd_get_section_flags(abfd, the_section) & SEC_LOAD ? " LOAD" : "",
	  bfd_get_section_flags(abfd, the_section) & SEC_CODE ? " CODE" : "",
	  bfd_get_section_flags(abfd, the_section) & SEC_DATA ? " DATA" : "",
	  bfd_get_section_flags(abfd, the_section) & SEC_ALLOC ? " ALLOC" : "",
	  bfd_get_section_flags(abfd, the_section) & SEC_READONLY ? " READONLY" : ""
	  ));

  /* determine the devices access */
  access = access_read;
  if (bfd_get_section_flags(abfd, the_section) & SEC_CODE)
    access |= access_exec;
  if (!(bfd_get_section_flags(abfd, the_section) & SEC_READONLY))
    access |= access_write;

  /* if a map, pass up a request to create the memory in core */
  if (strncmp(me->name, "map-binary@", strlen("map-binary@")) == 0)
    me->parent->callback->attach_address(me->parent,
					 me->name,
					 attach_raw_memory,
					 0 /*address space*/,
					 section_vma,
					 section_size,
					 access,
					 me);

  /* if a load dma in the required data */
  if (bfd_get_section_flags(abfd, the_section) & SEC_LOAD) {
    void *section_init = zalloc(section_size);
    if (!bfd_get_section_contents(abfd,
				  the_section,
				  section_init, 0,
				  section_size)) {
      bfd_perror("core:load_section()");
      error("load of data failed");
      return;
    }
    if (me->parent->callback->dma_write_buffer(me->parent,
					       section_init,
					       0 /*space*/,
					       section_vma,
					       section_size,
					       1 /*violate_read_only*/)
	!= section_size)
      error("data_init_callback() broken transfer for %s\n", me->name);
    zfree(section_init); /* only free if load */
  }
}


STATIC_INLINE_DEVICES void
binary_init_callback(const device *me,
		     psim *system)
{
  char file_name[100];
  bfd *image;
  DTRACE_INIT(binary);

  /* get a file name */
  if (scand_c(me->name, file_name, sizeof(file_name)) != 1)
    error("load_binary_init_callback() invalid load-binary device %s\n",
	  me->name);

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

  /* and the data sections */
  bfd_map_over_sections(image,
			update_for_binary_section,
			(PTR)me);

  bfd_close(image);
}


static device_callbacks const binary_callbacks = {
  binary_init_callback,
  unimp_device_attach_address,
  unimp_device_detach_address,
  unimp_device_io_read_buffer,
  unimp_device_io_write_buffer,
  unimp_device_dma_read_buffer,
  unimp_device_dma_write_buffer,
  unimp_device_attach_interrupt,
  unimp_device_detach_interrupt,
  unimp_device_interrupt,
  unimp_device_interrupt_ack,
  unimp_device_ioctl,
};



/* Stack device: stack@<type>

   Has a single IOCTL to create a stack frame of the specified type.
   If <type> is elf or xcoff then a corresponding stack is created.
   Any other value of type is ignored.

   The IOCTL takes the additional arguments:

     unsigned_word stack_end -- where the stack should come down from
     char **argv -- ...
     char **envp -- ...

   */

STATIC_INLINE_DEVICES int
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

STATIC_INLINE_DEVICES int
number_of_arguments(char **arg)
{
  int nr;
  if (arg == NULL)
    return 0;
  for (nr = 0; *arg != NULL; arg++, nr++);
  return nr;
}

STATIC_INLINE_DEVICES int
sizeof_arguments(char **arg)
{
  return ALIGN_8((number_of_arguments(arg) + 1) * sizeof(unsigned_word));
}

STATIC_INLINE_DEVICES void
write_stack_arguments(psim *system,
		      char **arg,
		      unsigned_word start_block,
		      unsigned_word end_block,
		      unsigned_word start_arg,
		      unsigned_word end_arg)
{
  DTRACE(stack,
	("write_stack_arguments(system=0x%x, arg=0x%x, start_block=0x%x, end_block=0x%x, start_arg=0x%x, end_arg=0x%x)\n",
	 system, arg, start_block, end_block, start_arg, end_arg));
  if (arg == NULL)
    error("write_arguments: character array NULL\n");
  /* only copy in arguments, memory is already zero */
  for (; *arg != NULL; arg++) {
    int len = strlen(*arg)+1;
    unsigned_word target_start_block;
    DTRACE(stack,
	  ("write_stack_arguments() write %s=%s at %s=0x%x %s=0x%x %s=0x%x\n",
	   "**arg", *arg, "start_block", start_block,
	   "len", len, "start_arg", start_arg));
    if (psim_write_memory(system, 0, *arg,
			  start_block, len,
			  0/*violate_readonly*/) != len)
      error("write_stack_arguments() - write of **arg (%s) at 0x%x failed\n",
	    *arg, start_block);
    target_start_block = H2T_word(start_block);
    if (psim_write_memory(system, 0, &target_start_block,
			  start_arg, sizeof(target_start_block),
			  0) != sizeof(target_start_block))
      error("write_stack_arguments() - write of *arg failed\n");
    start_block += ALIGN_8(len);
    start_arg += sizeof(start_block);
  }
  start_arg += sizeof(start_block); /*the null at the end*/
  if (start_block != end_block
      || ALIGN_8(start_arg) != end_arg)
    error("write_stack_arguments - possible corruption\n");
  DTRACE(stack,
	 ("write_stack_arguments() = void\n"));
}

STATIC_INLINE_DEVICES void
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

  /* install arguments on stack */
  write_stack_arguments(system, envp,
			start_envp_block, bottom_of_stack,
			start_envp, start_aux);
  write_stack_arguments(system, argv,
			start_argv_block, start_envp_block,
			start_argv, start_envp);

  /* set up the registers */
  psim_write_register(system, -1,
		      &top_of_stack, "sp", cooked_transfer);
  psim_write_register(system, -1,
		      &argc, "r3", cooked_transfer);
  psim_write_register(system, -1,
		      &start_argv, "r4", cooked_transfer);
  psim_write_register(system, -1,
		      &start_envp, "r5", cooked_transfer);
  psim_write_register(system, -1,
		      &start_aux, "r6", cooked_transfer);
}

STATIC_INLINE_DEVICES void
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

  /* extract arguments from registers */
  error("create_aix_stack_frame() - what happens next?\n");
}



static void
stack_ioctl_callback(const device *me,
		     psim *system,
		     cpu *processor,
		     unsigned_word cia,
		     ...)
{
  va_list ap;
  unsigned_word stack_pointer;
  char **argv;
  char **envp;
  va_start(ap, cia);
  stack_pointer = va_arg(ap, unsigned_word);
  argv = va_arg(ap, char **);
  envp = va_arg(ap, char **);
  va_end(ap);
  DTRACE(stack,
	 ("stack_ioctl_callback(me=0x%x:%s, system=0x%x, processor=0x%x, cia=0x%x, argv=0x%x, envp=0x%x)\n",
	  me, me->full_name, system, processor, cia, argv, envp));
  if (strcmp(me->name, "stack@elf") == 0)
    create_elf_stack_frame(system, stack_pointer, argv, envp);
  else if (strcmp(me->name, "stack@xcoff") == 0)
    create_aix_stack_frame(system, stack_pointer, argv, envp);
  DTRACE(stack, 
	 ("stack_ioctl_callback() = void\n"));
}


static device_callbacks const stack_callbacks = {
  ignore_device_init,
  unimp_device_attach_address,
  unimp_device_detach_address,
  unimp_device_io_read_buffer,
  unimp_device_io_write_buffer,
  unimp_device_dma_read_buffer,
  unimp_device_dma_write_buffer,
  unimp_device_attach_interrupt,
  unimp_device_detach_interrupt,
  unimp_device_interrupt,
  unimp_device_interrupt_ack,
  stack_ioctl_callback,
};



/* Table of all the devices and a function to lookup/create a device
   from its name */

typedef const device *(device_creator)
     (const char *name,
      const char *full_name,
      const device *parent);

typedef struct _device_descriptor device_descriptor;
struct _device_descriptor {
  const char *name;
  device_creator *creator;
  const device_callbacks *callbacks;
};

static device_descriptor devices[] = {
  { "console", console_create, NULL },
  { "memory", NULL, &memory_callbacks },
  { "vm", vea_vm_create, NULL },
  { "halt", NULL, &halt_callbacks },
  { "icu", NULL, &icu_callbacks },
  { "register", NULL, &register_callbacks },
  { "iobus", NULL, &iobus_callbacks },
  { "file", NULL, &file_callbacks },
  { "htab", NULL, &htab_callbacks },
  { "pte", NULL, &htab_callbacks }, /* yep - uses htab's table */
  { "stack", NULL, &stack_callbacks },
  { "sim", NULL, &sim_callbacks },
  { "load-binary", NULL, &binary_callbacks },
  { "map-binary", NULL, &binary_callbacks },
  { NULL },
};


INLINE_DEVICES const device *
device_create(const char *name,
	      const char *full_name,
	      const device *parent)
{
  device_descriptor *device;
  int name_len;
  char *chp;
  chp = strchr(name, '@');
  name_len = (chp == NULL ? strlen(name) : chp - name);
  for (device = devices; device->name != NULL; device++) {
    if (strncmp(name, device->name, name_len) == 0
	&& (device->name[name_len] == '\0'
	    || device->name[name_len] == '@'))
      if (device->creator != NULL)
	return device->creator(name, full_name, parent);
      else
	return device_create_from(name,
				  full_name,
				  NULL /* data */,
				  device->callbacks,
				  parent);
  }
  error("device_create() unknown device %s\n", name);
  return NULL;
}


INLINE_DEVICES const device *
device_create_from(const char *name,
		   const char *full_name,
		   void *data,
		   const device_callbacks *callback,
		   const device *parent)
{
  device *me = ZALLOC(device);
  me->name = strdup(name);
  me->full_name = strdup(full_name);
  me->data = data;
  me->callback = callback;
  me->parent = parent;
  return me;
}


INLINE_DEVICES const device_callbacks *
passthrough_device_callbacks(void)
{
  static const device_callbacks callbacks = {
    ignore_device_init,
    pass_device_attach_address,
    pass_device_detach_address,
    unimp_device_io_read_buffer,
    unimp_device_io_write_buffer,
    pass_device_dma_read_buffer,
    pass_device_dma_write_buffer,
    pass_device_attach_interrupt,
    pass_device_detach_interrupt,
    pass_device_interrupt,
    unimp_device_interrupt_ack,
    unimp_device_ioctl,
  };
  return &callbacks;
}



#endif /* _DEVICES_C_ */
