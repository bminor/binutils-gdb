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

#include "basics.h"
#include "device_tree.h"
#include "devices.h"
#include "events.h"

#include "cpu.h" /* drats */

/* Helper functions */

STATIC_INLINE_DEVICES void
parse_device_address(char *name,
		     unsigned *base,
		     unsigned *flags)
{
  /* extract the two arguments */
  name = strchr(name, '@');
  if (name == NULL)
    error("missing address for device %s\n", name);
  name++;
  *base = strtol(name, &name, 0);
  *flags = (*name == ','
	    ? strtol(name+1, &name, 0)
	    : 0);
}


/* Simple console device:

   Implements a simple text output device that is attached to stdout
   of the process running the simulation.  The devices has four
   word registers:

   0: read
   4: read-status
   8: write
   c: write-status

   Where a nonzero status register indicates that the device is ready
   (input fifo contains a character or output fifo has space).

   Illustrates: Mapping read/write to device operations onto actual
   registers.

   */

typedef struct _console_buffer {
  char buffer;
  int status;
  event_entry_tag event_tag;
} console_buffer;

typedef struct _console_device {
  unsigned_word my_base_address;
  int interrupt_delay;
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


STATIC_INLINE_DEVICES unsigned64
console_read_callback(device_node *device,
		      unsigned_word base,
		      unsigned nr_bytes,
		      cpu *processor,
		      unsigned_word cia)
{
  console_device *con = (console_device*)device->data;
  TRACE(trace_console_device,
	("device=0x%x, base=0x%x, nr_bytes=%d\n",
	 device, base, nr_bytes));

  /* handle the request */

  switch (base & console_offset_mask) {

  case console_read_buffer:
    return con->input.buffer;

  case console_read_status:
    { /* check for input */
      int flags;
      int status;
      /* get the old status */
      flags = fcntl(0, F_GETFL, 0);
      if (flags == -1) {
	perror("console");
	return 0;
      }
      /* temp, disable blocking IO */
      status = fcntl(0, F_SETFL, flags | O_NDELAY);
      if (status == -1) {
	perror("console");
	return 0;
      }
      /* try for input */
      status = read(0, &con->input.buffer, 1);
      if (status == 1) {
	con->input.status = 1;
      }
      else {
	con->input.status = 0;
      }
      /* return to regular vewing */
      fcntl(0, F_SETFL, flags);
    }
    return con->input.status;

  case console_write_buffer:
    return con->output.buffer;

  case console_write_status:
    return con->output.status;

  default:
    error("console_read_callback() internal error\n");
    return 0;

  }

}

STATIC_INLINE_DEVICES void
console_write_callback(device_node *device,
		       unsigned_word base,
		       unsigned nr_bytes,
		       unsigned64 val,
		       cpu *processor,
		       unsigned_word cia)
{
  console_device *con = (console_device*)device->data;

  TRACE(trace_console_device,
	("device=0x%x, base=0x%x, nr_bytes=%d, val=0x%x\n",
	 device, base, nr_bytes, val));

  /* check for bus error */
  if (base & 0x3) {
    error("%s - misaligned base address, base=0x%x, nr_bytes=%d\n",
	  "console_write_callback", base, nr_bytes);
  }

  switch (base & console_offset_mask) {
  case console_read_buffer: con->input.buffer = val; break;
  case console_read_status: con->input.status = val; break;
  case console_write_buffer:
    TRACE(trace_console_device,
	  ("<%c:%d>", val, val));
    printf_filtered("%c", val);
    con->output.buffer = val;
    con->output.status = 1;
    break;
  case console_write_status:
    con->output.status = val;
    break;
  }

}

static device_callbacks console_callbacks = {
  console_read_callback,
  console_write_callback,
};

STATIC_INLINE_DEVICES device_node *
console_create(device_node *parent,
	       char *name)
{
  device_node *device;
  unsigned address_base;
  unsigned address_flags;

  /* create the descriptor */
  console_device *console = ZALLOC(console_device);

  /* extract the two arguments */
  parse_device_address(name, &address_base, &address_flags);

  /* fill in the details */
  console->my_base_address = address_base;
  console->interrupt_delay = address_flags;
  console->output.status = 1;
  console->output.buffer = '\0';
  console->input.status = 0;
  console->input.buffer = '\0';

  /* insert into the device tree along with its address info */
  device = device_node_create(parent, name, sequential_device,
			      &console_callbacks, console);
  device_node_add_address(device,
			  address_base,
			  console_size,
			  device_is_read_write_exec,
			  NULL);

  return device;
}

static device_descriptor console_descriptor = {
  "console",
  console_create,
};


/* ICU device:

   Single 4 byte register.  Read returns processor number.  Write
   interrupts specified processor.

   Illustrates passing of events to parent device. Passing of
   interrupts to parent bus.

   NB: For the sake of illustrating the passing of interrupts.  This
   device doesn't pass interrupt events to its parent.  Instead it
   passes them back to its self. */

STATIC_INLINE_DEVICES unsigned64
icu_read_callback(device_node *device,
		  unsigned_word base,
		  unsigned nr_bytes,
		  cpu *processor,
		  unsigned_word cia)
{
  TRACE(trace_icu_device,
	("device=0x%x, base=0x%x, nr_bytes=%d\n",
	 device, base, nr_bytes));
  return cpu_nr(processor);
}

STATIC_INLINE_DEVICES void
icu_write_callback(device_node *device,
		   unsigned_word base,
		   unsigned nr_bytes,
		   unsigned64 val,
		   cpu *processor,
		   unsigned_word cia)
{
  psim *system = cpu_system(processor);
  device_node *parent = device; /* NB: normally would be device->parent */
  TRACE(trace_icu_device,
	("device=0x%x, base=0x%x, nr_bytes=%d, val=0x%x\n",
	 device, base, nr_bytes, val));
  /* tell the parent device that the interrupt lines have changed.
     For this fake ICU.  The interrupt lines just indicate the cpu to
     interrupt next */
  parent->callbacks->interrupt_callback(parent, val, device, processor, cia);
}

STATIC_INLINE_DEVICES void
icu_do_interrupt(event_queue *queue,
		 void *data)
{
  cpu *target = (cpu*)data;
  /* try to interrupt the processor.  If the attempt fails, try again
     on the next tick */
  if (!external_interrupt(target))
    event_queue_schedule(queue, 1, icu_do_interrupt, target);
}

STATIC_INLINE_DEVICES void
icu_interrupt_callback(device_node *me,
		       int interrupt_status,
		       device_node *device,
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
    event_queue_schedule(events, 1, icu_do_interrupt, target);
  }
}

static device_callbacks icu_callbacks = {
  icu_read_callback,
  icu_write_callback,
  icu_interrupt_callback,
};

STATIC_INLINE_DEVICES device_node *
icu_create(device_node *parent,
	       char *name)
{
  device_node *device;
  unsigned address_base;
  unsigned address_flags;

  /* extract the two arguments */
  parse_device_address(name, &address_base, &address_flags);

  /* insert into the device tree along with its address info */
  device = device_node_create(parent, name, sequential_device,
			      &icu_callbacks, 0);
  device_node_add_address(device,
			  address_base,
			  4,
			  device_is_read_write_exec,
			  NULL);

  return device;
}

static device_descriptor icu_descriptor = {
  "icu",
  icu_create,
};





/* HALT device:

   With real hardware, the processor operation is normally terminated
   through a reset.  This device illustrates how a reset device could
   be attached to an address */

STATIC_INLINE_DEVICES unsigned64
halt_read_callback(device_node *device,
		   unsigned_word base,
		   unsigned nr_bytes,
		   cpu *processor,
		   unsigned_word cia)
{
  cpu_halt(processor, cia, was_exited, 0);
  return 0;
}

STATIC_INLINE_DEVICES void
halt_write_callback(device_node *device,
		    unsigned_word base,
		    unsigned nr_bytes,
		    unsigned64 val,
		    cpu *processor,
		    unsigned_word cia)
{
  cpu_halt(processor, cia, was_exited, 0);
}


static device_callbacks halt_callbacks = {
  halt_read_callback,
  halt_write_callback,
};

STATIC_INLINE_DEVICES device_node *
halt_create(device_node *parent,
	    char *name)
{
  device_node *device;
  unsigned address_base;
  unsigned address_flags;

  parse_device_address(name, &address_base, &address_flags);
  device = device_node_create(parent, name, other_device,
			      &halt_callbacks, NULL);
  device_node_add_address(device,
			  address_base,
			  4,
			  device_is_read_write_exec,
			  NULL);
  return device;
}

static device_descriptor halt_descriptor = {
  "halt",
  halt_create,
};


static device_descriptor *devices[] = {
  &console_descriptor,
  &halt_descriptor,
  &icu_descriptor,
  NULL,
};


INLINE_DEVICES device_descriptor *
find_device_descriptor(char *name)
{
  device_descriptor **device;
  int name_len;
  char *chp;
  chp = strchr(name, '@');
  name_len = (chp == NULL ? strlen(name) : chp - name);
  for (device = devices; *device != NULL; device++) {
    if (strncmp(name, (*device)->name, name_len) == 0
	&& ((*device)->name[name_len] == '\0'
	    || (*device)->name[name_len] == '@'))
      return *device;
  }
  return NULL;
}

#endif /* _DEVICES_C_ */
