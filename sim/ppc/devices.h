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


#ifndef _DEVICES_H_
#define _DEVICES_H_

#ifndef INLINE_DEVICES
#define INLINE_DEVICES
#endif

#ifndef STATIC_DEVICES
#define STATIC_DEVICES
#endif


/* forward declaration of types */
/* typedef struct _device device; -- in devices.h */


/* Address access attributes that can be attached to a devices address
   range */
typedef enum _access_type {
  access_invalid = 0,
  access_read = 1,
  access_write = 2,
  access_read_write = 3,
  access_exec = 4,
  access_read_exec = 5,
  access_write_exec = 6,
  access_read_write_exec = 7,
} access_type;

/* Address attachement types */
typedef enum _attach_type {
  attach_invalid,
  attach_callback,
  attach_default,
  attach_raw_memory,
} attach_type;


/* Operators on devices: */


/* Initialization:

   A device is made fully functional in two stages.

   1. It is created. A device is created _before_ it is entered into
   the device tree.  During creation any permenant structures needed
   by the device should be created/initialized.

   2. It is initialized.  Before a simulation run, each device in the
   device tree is initialized in prefix order.  As part of this
   initialization, a device should (re)attach its self to its parent
   as needed.

   */

typedef void (device_init_callback)
     (const device *me,
      psim *system);

#define DTRACE_INIT(OBJECT) \
     DTRACE(OBJECT, \
	    (#OBJECT "_init(me=0x%lx:%s system=0x%lx)\n", \
	     (long)me, me->full_name, (long)system))

/* Data transfers:

   A device may permit the reading/writing (IO) of its registers in
   one or more address spaces.  For instance, a PCI device may have
   config registers in its config space and control registers in both
   the io and memory spaces of a PCI bus.

   Similarly, a device may initiate a data transfer (DMA) by passing
   such a request up to its parent.

   Init:

   As part of its initialization (not creation) and possibly also as a
   consequence of IO a device may attach its self to one or more of
   the address spaces of its parent device.

   For instance, a PCI device, during initialization would attach its
   config registers (space=0?, base=0, nr_bytes=64) to its parent PCI
   bridge.  Later, due to a write to this config space, the same
   device may in turn find it necessary to also attach its self to
   it's parent's `memory' or `io' space.

   To perform these operations, a device will call upon its parent
   using either device_attach_address or device_detach_address.

   * Any address specified is according to what the device expects to
   see.

   * Any detach operation must exactly match a previous attach.

   * included with the attach or detach is the devices name, the
   parent may use this as part of determining how to map map between a
   child's address + space and its own.

   * at any time, at most one device can have a default mapping
   registered.


   IO:

   A device receives requests to perform reads/writes to its registers
   or memory either A. from a processor or B. from a parent device.

   The device may then in turn either A. resolve the IO request
   locally by processing the data or trigering an exception or
   B. re-mapping the access onto one of its local address spaces and
   then in turn passing that on to one of its children.

   * Any address passed is relative to the local device.  Eg for PCI
   config registers, the address would (normally) be in the range of 0
   to 63.

   * Any exception situtation triggered by an IO operation (processor
   != NULL) is handled in one of the following ways: 1. Machine check
   (and similar): issued immediatly by restarting the cpu; 2. External
   exception: issue delayed (using events.h) until the current
   instruction execution cycle is completed; 3. Slave device (and
   similar): the need for the interrupt is passed on to the devices
   parent (which being an interrupt control unit will in turn take one
   of the actions described here); 4. Forget it.

   * Any exception situtation trigered by a non IO operation
   (processor == NULL) is handled buy returning 0.

   * Transfers of size <= 8 and of a power of 2 *must* be correctly
   aligned and should be treated as a `single cycle' transfer.

   DMA:

   A device initiates a DMA transfer by calling its parent with the
   request.  At the top level (if not done earlier) this is reflected
   back down the tree as io read/writes to the target device.

   This function is subject to change ...

   */

typedef void (device_config_address_callback)
     (const device *me,
      const char *name,
      attach_type attach,
      int space,
      unsigned_word addr,
      unsigned nr_bytes,
      access_type access,
      const device *who); /*callback/default*/

#define DTRACE_ATTACH_ADDRESS(OBJECT) \
     DTRACE(OBJECT, \
	    (#OBJECT "_attach_address(me=0x%lx:%s, name=%s, attach=%ld, space=%ld, addr=0x%lx, nr_bytes=%ld, access=%ld, who=0x%lx)\n", \
	     (long)me, me->full_name, name, (long)attach, (long)space, \
	     (long)addr, (long)nr_bytes, (long)access, (long)who))
#define DTRACE_DETACH_ADDRESS(OBJECT) \
     DTRACE(OBJECT, \
	    (#OBJECT "_detach_address(me=0x%lx:%s, name=%s, attach=%ld, space=%ld, addr=0x%lx, nr_bytes=%ld, access=%ld, who=0x%lx)\n", \
	     (long)me, me->full_name, name, (long)attach, (long)space, \
	     (long)addr, (long)nr_bytes, (long)access, (long)who))


typedef unsigned (device_io_read_buffer_callback)
     (const device *me,
      void *dest,
      int space,
      unsigned_word addr,
      unsigned nr_bytes,
      cpu *processor,
      unsigned_word cia);

typedef unsigned (device_io_write_buffer_callback)
     (const device *me,
      const void *source,
      int space,
      unsigned_word addr,
      unsigned nr_bytes,
      cpu *processor,
      unsigned_word cia);

#define DTRACE_IO_READ_BUFFER(OBJECT) \
     DTRACE(OBJECT, \
	    (#OBJECT "_io_read_buffer(me=0x%lx:%s dest=0x%lx space=%ld addr=0x%lx nr_bytes=%ld processor=0x%lx cia=0x%lx)\n", \
	     (long)me, me->full_name, (long)dest, (long)space, (long)addr, \
	     (long)nr_bytes, (long)processor, (long)cia))
#define DTRACE_IO_WRITE_BUFFER(OBJECT) \
     DTRACE(OBJECT, \
	    (#OBJECT "_io_write_buffer(me=0x%lx:%s source=0x%lx space=%ld addr=0x%lx nr_bytes=%ld processor=0x%lx cia=0x%lx)\n", \
	     (long)me, me->full_name, (long)source, (long)space, (long)addr, \
	     (long)nr_bytes, (long)processor, (long)cia))


typedef unsigned (device_dma_read_buffer_callback)
     (const device *me,
      void *dest,
      int space,
      unsigned_word addr,
      unsigned nr_bytes);

typedef unsigned (device_dma_write_buffer_callback)
     (const device *me,
      const void *source,
      int space,
      unsigned_word addr,
      unsigned nr_bytes,
      int violate_read_only_section);

#define DTRACE_DMA_READ_BUFFER(OBJECT) \
     DTRACE(OBJECT, \
	    (#OBJECT "_dma_read_buffer(me=0x%lx:%s dest=0x%lx space=%ld addr=0x%lx nr_bytes=%ld)\n", \
	     (long)me, me->full_name, (long)dest, (long)space, (long)addr, (long)nr_bytes))
#define DTRACE_DMA_WRITE_BUFFER(OBJECT) \
     DTRACE(OBJECT, \
	    (#OBJECT "_dma_write_buffer(me=0x%lx:%s source=0x%lx space=%ld addr=0x%lx nr_bytes=%ld)\n", \
	     (long)me, me->full_name, (long)source, (long)space, (long)addr, (long)nr_bytes))


/* Interrupts:

   As mentioned above.  Instead of handling an interrupt directly, a
   device may instead pass the need to interrupt on to its parent.

   Init:

   Before passing interrupts up to is parent, a device must first
   attach its interrupt lines to the parent device.  To do this, the
   device uses the parents attach/detach calls.
  
   Interrupts:

   A child notifies a parent of a change in an interrupt lines status
   using the interrupt call.  Similarly, a parent may notify a child
   of any `interrupt ack' sequence using the interrupt_ack call.

   */

typedef void (device_config_interrupt_callback)
     (const device *me,
      const device *who,
      int interrupt_line,
      const char *name);

typedef void (device_interrupt_ack_callback)
     (const device *me,
      int interrupt_line,
      int interrupt_status);

typedef void (device_interrupt_callback)
     (const device *me,
      const device *who,
      int interrupt_line,
      int interrupt_status,
      cpu *processor,
      unsigned_word cia);


/* IOCTL:

   Very simply, a catch all for any thing that turns up that until now
   either hasn't been thought of or doesn't justify an extra function. */


typedef void (device_ioctl_callback)
     (const device *me,
      psim *system,
      cpu *processor,
      unsigned_word cia,
      ...);
      


/* the callbacks */

typedef struct _device_callbacks {
  /* initialization */
  device_init_callback *init;
  /* address/data config - from child */
  device_config_address_callback *attach_address;
  device_config_address_callback *detach_address;
  /* address/data transfer - to child */
  device_io_read_buffer_callback *io_read_buffer;
  device_io_write_buffer_callback *io_write_buffer;
  /* address/data transfer - from child */
  device_dma_read_buffer_callback *dma_read_buffer;
  device_dma_write_buffer_callback *dma_write_buffer;
  /* interrupt config - from child */
  device_config_interrupt_callback *attach_interrupt;
  device_config_interrupt_callback *detach_interrupt;
  /* interrupt transfer - from child */
  device_interrupt_callback *interrupt;
  /* interrupt transfer - to child */
  device_interrupt_ack_callback *interrupt_ack;
  /* back door to anything we've forgot */
  device_ioctl_callback *ioctl;
} device_callbacks;

/* A device */
struct _device {
  const char *name; /* eg rom@0x1234,0x400 */
  const char *full_name; /* eg /isa/rom@0x1234,0x400 */
  void *data; /* device specific data */
  const device_callbacks *callback;
  const device *parent;
};


/* Create a new device, finding it in the builtin device table */

INLINE_DEVICES const device *device_create
(const char *name,
 const char *full_name,
 const device *parent);

/* create a new device using the parameterized data */

INLINE_DEVICES const device *device_create_from
(const char *name,
 const char *full_name,
 void *data,
 const device_callbacks *callback,
 const device *parent);


/* Unimplemented call back functions.  These abort the simulation */

INLINE_DEVICES device_init_callback unimp_device_init;
INLINE_DEVICES device_config_address_callback unimp_device_attach_address;
INLINE_DEVICES device_config_address_callback unimp_device_detach_address;
INLINE_DEVICES device_io_read_buffer_callback unimp_device_io_read_buffer;
INLINE_DEVICES device_io_write_buffer_callback unimp_device_io_write_buffer;
INLINE_DEVICES device_dma_read_buffer_callback unimp_device_dma_read_buffer;
INLINE_DEVICES device_dma_write_buffer_callback unimp_device_dma_write_buffer;
INLINE_DEVICES device_config_interrupt_callback unimp_device_attach_interrupt;
INLINE_DEVICES device_config_interrupt_callback unimp_device_detach_interrupt;
INLINE_DEVICES device_interrupt_callback unimp_device_interrupt;
INLINE_DEVICES device_interrupt_ack_callback unimp_device_interrupt_ack;
STATIC_DEVICES device_ioctl_callback unimp_device_ioctl;

/* Pass through and ignore callback functions.  A call going towards
   the root device are passed on up, local calls are ignored and call
   downs abort */

INLINE_DEVICES device_init_callback ignore_device_init;
INLINE_DEVICES device_config_address_callback pass_device_attach_address;
INLINE_DEVICES device_config_address_callback pass_device_detach_address;
INLINE_DEVICES device_dma_read_buffer_callback pass_device_dma_read_buffer;
INLINE_DEVICES device_dma_write_buffer_callback pass_device_dma_write_buffer;
INLINE_DEVICES device_config_interrupt_callback pass_device_attach_interrupt;
INLINE_DEVICES device_config_interrupt_callback pass_device_detach_interrupt;
INLINE_DEVICES device_interrupt_callback pass_device_interrupt;

INLINE_DEVICES const device_callbacks *passthrough_device_callbacks
(void);



#endif /* _DEVICES_H_ */
