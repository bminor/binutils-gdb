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


#ifndef _DEVICE_TREE_H_
#define _DEVICE_TREE_H_

#ifndef INLINE_DEVICE
#define INLINE_DEVICE
#endif



/* declared in basics.h, this object is used everywhere */
/* typedef struct _device device; */




/* Device Tree:

   All the devices in this model live in a tree. The following allow
   the location/manipulation of this tree */

INLINE_DEVICE(device *) device_parent
(device *me);

INLINE_DEVICE(device *) device_sibling
(device *me);

INLINE_DEVICE(device *) device_child
(device *me);

INLINE_DEVICE(const char *) device_name
(device *me);

INLINE_DEVICE(void *) device_data
(device *me);


/* Grow the device tree adding either a specific device or
   alternativly a device found in the device table */

INLINE_DEVICE(device *)device_tree_add_device
(device *root,
 const char *prefix,
 device *new_sub_tree);

INLINE_DEVICE(device *) device_tree_add_found
(device *root,
 const char *prefix,
 const char *name);

INLINE_DEVICE(device *) device_tree_add_found_c
(device *root,
 const char *prefix,
 const char *name,
 const char *c1);

INLINE_DEVICE(device *) device_tree_add_found_c_uw
(device *root,
 const char *prefix,
 const char *name,
 const char *c1,
 unsigned_word uw2);

INLINE_DEVICE(device *) device_tree_add_found_uw_u
(device *root,
 const char *prefix,
 const char *name,
 unsigned_word uw1,
 unsigned u2);

INLINE_DEVICE(device *) device_tree_add_found_uw_u_u
(device *root,
 const char *prefix,
 const char *name,
 unsigned_word uw1,
 unsigned u2,
 unsigned u3);

INLINE_DEVICE(device *) device_tree_add_found_uw_u_u_c
(device *root,
 const char *prefix,
 const char *name,
 unsigned_word uw1,
 unsigned u2,
 unsigned u3,
 const char *c4);

INLINE_DEVICE(device *) device_tree_add_found_uw_uw_u_u_c
(device *root,
 const char *prefix,
 const char *name,
 unsigned_word uw1,
 unsigned_word uw2,
 unsigned u3,
 unsigned u4,
 const char *c5);

INLINE_DEVICE(device *) device_tree_add_found_uw_uw_u_u_u
(device *root,
 const char *prefix,
 const char *name,
 unsigned_word uw1,
 unsigned_word uw2,
 unsigned u3,
 unsigned u4,
 unsigned u5);


/* Query the device tree, null is returned if the specified device is
   not found */

INLINE_DEVICE(device *) device_tree_find_device
(device *root,
 const char *path);


/* traverse the device tree visiting all notes (either pre or post
   fix) */

typedef void (device_tree_traverse_function)
     (device *device,
      void *data);

INLINE_DEVICE(void) device_tree_traverse
(device *root,
 device_tree_traverse_function *prefix,
 device_tree_traverse_function *postfix,
 void *data);


/* dump a node, this can be passed to the device_tree_traverse()
   function to dump out the entire device tree */

INLINE_DEVICE(void) device_tree_dump
(device *device,
 void *ignore_data_argument);




/* Device Properties:

   Attached to a device (typically by open boot firmware) are
   properties that profile the devices features.  The below allow the
   manipulation of device properties */

/* Each device can have associated properties.  Internal to
   psim those properties are strictly typed.  Within the simulation,
   no such control exists */

typedef enum {
  integer_property,
  boolean_property,
  string_property,
  array_property,
  null_property,
} device_property_type;

typedef struct _device_property device_property;
struct _device_property {
  device *owner;
  device_property_type type;
  unsigned sizeof_array;
  const void *array;
};


/* Basic operations used by software */

INLINE_DEVICE(const char *) device_find_next_property
(device *me,
 const char *previous);

/* INLINE_DEVICE void device_add_property
   No such external function, all properties, when added are explictly
   typed */

INLINE_DEVICE(void) device_add_array_property
(device *me,
 const char *property,
 const void *array,
 int sizeof_array);

INLINE_DEVICE(void) device_add_integer_property
(device *me,
 const char *property,
 signed_word integer);

INLINE_DEVICE(void) device_add_boolean_property
(device *me,
 const char *property,
 int bool);

INLINE_DEVICE(void) device_add_null_property
(device *me,
 const char *property);

INLINE_DEVICE(void) device_add_string_property
(device *me,
 const char *property,
 const char *string);


/* Locate a property returning its description.  Return NULL if the
   named property is not found */

INLINE_DEVICE(const device_property *) device_find_property
(device *me,
 const char *property);


/* Process all properties attached to the named device */

typedef void (device_traverse_property_function)
     (device *me,
      const char *name,
      void *data);

INLINE_DEVICE(void) device_traverse_properties
(device *me,
 device_traverse_property_function *traverse,
 void *data);
 

/* Similar to above except that the property *must* be in the device
   tree and *must* be of the specified type. */

INLINE_DEVICE(const device_property *) device_find_array_property
(device *me,
 const char *property);

INLINE_DEVICE(signed_word) device_find_integer_property
(device *me,
 const char *property);

INLINE_DEVICE(const char *) device_find_string_property
(device *me,
 const char *property);

INLINE_DEVICE(int) device_find_boolean_property
(device *me,
 const char *property);



/* Device Hardware:

   A device principaly is modeling real hardware that a processor can
   directly interact with via load/stores dma's and interrupts.  The
   interface below is used by the hardware side of the device
   model. */

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

INLINE_DEVICE(device *) device_create
(const char *name,
 device *parent);

/* some external functions want to create things */
typedef struct _device_callbacks device_callbacks;

INLINE_DEVICE(device *) device_create_from
(const char *name,
 void *data,
 const device_callbacks *callbacks,
 device *parent);

INLINE_DEVICE(void) device_init
(device *me,
 psim *system);

/* initialize the entire tree */

INLINE_DEVICE(void) device_tree_init
(device *root,
 psim *system);


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

INLINE_DEVICE(void) device_attach_address
(device *me,
 const char *name,
 attach_type attach,
 int space,
 unsigned_word addr,
 unsigned nr_bytes,
 access_type access,
 device *who); /*callback/default*/

INLINE_DEVICE(void) device_detach_address
(device *me,
 const char *name,
 attach_type attach,
 int space,
 unsigned_word addr,
 unsigned nr_bytes,
 access_type access,
 device *who); /*callback/default*/

INLINE_DEVICE(unsigned) device_io_read_buffer
(device *me,
 void *dest,
 int space,
 unsigned_word addr,
 unsigned nr_bytes,
 cpu *processor,
 unsigned_word cia);

INLINE_DEVICE(unsigned) device_io_write_buffer
(device *me,
 const void *source,
 int space,
 unsigned_word addr,
 unsigned nr_bytes,
 cpu *processor,
 unsigned_word cia);

INLINE_DEVICE(unsigned) device_dma_read_buffer
(device *me,
 void *dest,
 int space,
 unsigned_word addr,
 unsigned nr_bytes);

INLINE_DEVICE(unsigned) device_dma_write_buffer
(device *me,
 const void *source,
 int space,
 unsigned_word addr,
 unsigned nr_bytes,
 int violate_read_only_section);


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

INLINE_DEVICE(void) device_attach_interrupt
(device *me,
 device *who,
 int interrupt_line,
 const char *name);

INLINE_DEVICE(void) device_detach_interrupt
(device *me,
 device *who,
 int interrupt_line,
 const char *name);

INLINE_DEVICE(void) device_interrupt
(device *me,
 device *who,
 int interrupt_line,
 int interrupt_status,
 cpu *processor,
 unsigned_word cia);

INLINE_DEVICE(void) device_interrupt_ack
(device *me,
 int interrupt_line,
 int interrupt_status);


/* IOCTL:

   Very simply, a catch all for any thing that turns up that until now
   either hasn't been thought of or doesn't justify an extra function. */

EXTERN_DEVICE\
(void) device_ioctl
(device *me,
 psim *system,
 cpu *processor,
 unsigned_word cia,
 ...);
      


/* Device software - the instance

   Under development

   In addition to the processor directly manipulating a device via
   read/write operations.  A program may manipulate a device
   indirectly via OpenBoot calls.  The following provide a higher
   level software interface to the devices */

#if 0
INLINE_DEVICE(device_instance *)device_instance_open
(device *me,
 const char *device_specifier);

INLINE_DEVICE(void) device_instance_close
(device_instance *instance);

INLINE_DEVICE(int) device_instance_read
(device_instance *instance,
 void *addr,
 unsigned_word len);

INLINE_DEVICE(int) device_instance_write
(device_instance *instance,
 const void *addr,
 unsigned_word len);

INLINE_DEVICE(int) device_instance_seek
(device_instance *instance,
 unsigned_word pos_hi,
 unsigned_word pos_lo);

INLINE_DEVICE(device *) device_instance_device
(device_instance *instance);

INLINE_DEVICE(const char *) device_instance_name
(device_instance *instance);
#endif




/* Device dregs... */

/* Parse a device name, various formats:

   uw: unsigned_word
   u: unsigned
   c: string */

INLINE_DEVICE(int) scand_c
(const char *name,
 char *c1,
 unsigned c1size);

INLINE_DEVICE(int) scand_c_uw_u
(const char *name,
 char *c1,
 unsigned c1size,
 unsigned_word *uw2,
 unsigned *u3);
 
INLINE_DEVICE(int) scand_uw
(const char *name,
 unsigned_word *uw1);
 
INLINE_DEVICE(int) scand_uw_c
(const char *name,
 unsigned_word *uw1,
 char *c2,
 unsigned c2size);
 
INLINE_DEVICE(int) scand_uw_u
(const char *name,
 unsigned_word *uw1,
 unsigned *u2);
 
INLINE_DEVICE(int) scand_uw_u_u
(const char *name,
 unsigned_word *uw1,
 unsigned *u2,
 unsigned *u3);
 
INLINE_DEVICE(int) scand_uw_u_u_c
(const char *name,
 unsigned_word *uw1,
 unsigned *u2,
 unsigned *u3,
 char *c4,
 unsigned c4size);
 
INLINE_DEVICE(int) scand_uw_uw
(const char *name,
 unsigned_word *uw1,
 unsigned_word *uw2);

INLINE_DEVICE(int) scand_uw_uw_u
(const char *name,
 unsigned_word *uw1,
 unsigned_word *uw2,
 unsigned *u3);
 
INLINE_DEVICE(int) scand_uw_uw_u_u_c
(const char *name,
 unsigned_word *uw1,
 unsigned_word *uw2,
 unsigned *u3,
 unsigned *u4,
 char *c5,
 unsigned c5size);
 
INLINE_DEVICE(int) scand_uw_uw_u_u_u
(const char *name,
 unsigned_word *uw1,
 unsigned_word *uw2,
 unsigned *u3,
 unsigned *u4,
 unsigned *u5);
 
#endif /* _DEVICE_TREE_H_ */
