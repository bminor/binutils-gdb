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

#ifndef INLINE_DEVICE_TREE
#define INLINE_DEVICE_TREE
#endif


/* forward declaration of types */

typedef struct _device_node device_node;
typedef struct _device_address device_address;
typedef struct _device_callbacks device_callbacks;


/* Device callbacks: */


/* Memory operations: transfer data to/from a processor.

   These callbacks pass/return data in *host* byte order.

   Should a memory read/write operation cause an interrupt (external
   exception) then a device would typically pass an interrupt message
   to the devices parent.  Hopefully that is an interrupt controler
   and will know what to do with it.

   Devices normally never either restart a processor or issue an
   interrupt directly.  The only exception I've thought of could be
   machine check type event. */

typedef unsigned64 (device_reader_callback)
     (device_node *device,
      unsigned_word base,
      unsigned nr_bytes,
      cpu *processor,
      unsigned_word cia);

typedef void (device_writer_callback)
     (device_node *device,
      unsigned_word base,
      unsigned nr_bytes,
      unsigned64 val,
      cpu *processor,
      unsigned_word cia);

/* Interrupts:

   A child device uses the below to pass on to its parent changes in
   the state of a child devices interrupt lines.

   Typically, the parent being an interrupt control device, would, in
   responce, schedule an event at the start of the next clock cycle.
   On this event, the state of any cpu could be changed.  Other
   devices could either ignore or pass on the interrupt message */

typedef void (device_interrupt_callback)
     (device_node *me,
      int interrupt_status,
      device_node *device,
      cpu *processor,
      unsigned_word cia);

/* Create:

   DEVICE_CREATOR is called once, as part of building the device tree.
   This function gives the device the chance to attach any additional
   data to this particular device instance.

   DEVICE_INIT_CALLBACK is (re)called when ever the system is
   (re)initialised. */

typedef device_node *(device_creator)
     (device_node *parent,
      char *name);

typedef void (device_init_callback)
     (device_node *device);



/* constructs to describe the hardware's tree of devices */

typedef enum _device_type {
  /* default */
  unknown_device,
  /* typical devices */
  memory_device,
  sequential_device,
  block_device,
  bus_device,
  other_device,
  /* atypical devices, these are for data being loaded into ram/rom */
  data_device,
  options_device,
  /* types of primative nodes containing just data */
  boolean_type_device,
  integer_type_device,
  string_type_device,
  byte_type_device,
} device_type;

typedef enum _device_access {
  device_is_readable = 1,
  device_is_writeable = 2,
  device_is_read_write = 3,
  device_is_executable = 4,
  device_is_read_exec = 5,
  device_is_write_exec = 6,
  device_is_read_write_exec = 7,
} device_access;

struct _device_address {
  unsigned_word lower_bound;
  unsigned_word upper_bound;
  unsigned size; /* host limited */
  void *init; /* initial data */
  device_access access;
  device_address *next_address;
};

struct _device_callbacks {
  device_reader_callback *read_callback;
  device_writer_callback *write_callback;
  device_interrupt_callback *interrupt_callback;
  /* device_init_callback *init_callback; */
  /* device_init_hander *post_init_handler; */
};

struct _device_node {
  /* where i am */
  device_node *parent;
  device_node *children;
  device_node *sibling;
  /* what I am */
  char *name; /* eg rom@0x1234,0x40 */
  device_type type;
  device_callbacks *callbacks;
  device_address *addresses;
  void *data;
};


/* given the image to run, return its device tree */

INLINE_DEVICE_TREE device_node *device_tree_create
(const char *hardware_description);


/* traverse the tree eiter pre or post fix */

typedef void (device_tree_traverse_function)
     (device_node *device,
      void *data);

INLINE_DEVICE_TREE void device_tree_traverse
(device_node *root,
 device_tree_traverse_function *prefix,
 device_tree_traverse_function *postfix,
 void *data);


/* query the device tree */

INLINE_DEVICE_TREE device_node *device_tree_find_node
(device_node *root,
 const char *path);

INLINE_DEVICE_TREE device_node *device_tree_find_next_node
(device_node *root,
 const char *path,
 device_node *last);

INLINE_DEVICE_TREE signed_word device_tree_find_int
(device_node *root,
 const char *path);

INLINE_DEVICE_TREE const char *device_tree_find_string
(device_node *root,
 const char *path);

INLINE_DEVICE_TREE int device_tree_find_boolean
(device_node *root,
 const char *path);

INLINE_DEVICE_TREE void *device_tree_find_bytes
(device_node *root,
 const char *path);

/* add to the device tree */

INLINE_DEVICE_TREE device_node *device_node_create
(device_node *parent,
 char *name,
 device_type type,
 device_callbacks *callbacks,
 void *data);

INLINE_DEVICE_TREE void device_node_add_address
(device_node *node,
 unsigned_word lower_bound,
 unsigned size,
 device_access access,
 void *init);

/* dump a node, pass this to the device_tree_traverse() function to
   dump the tree */

INLINE_DEVICE_TREE void device_tree_dump
(device_node *device,
 void *ignore_data_argument);

#endif /* _DEVICE_TREE_H_ */
