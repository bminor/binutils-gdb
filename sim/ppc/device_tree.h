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


#include "devices.h"

typedef struct _device_tree device_tree;


/* extend the device tree, each function returns the address of the
   new node */

INLINE_DEVICE_TREE device_tree *device_tree_add_passthrough
(device_tree *root,
 const char *path);

INLINE_DEVICE_TREE device_tree *device_tree_add_device
(device_tree *root,
 const char *path,
 const device *dev);
 
INLINE_DEVICE_TREE device_tree *device_tree_add_integer
(device_tree *root,
 const char *path,
 signed_word integer);

INLINE_DEVICE_TREE device_tree *device_tree_add_string
(device_tree *root,
 const char *path,
 const char *string);

INLINE_DEVICE_TREE device_tree *device_tree_add_boolean
(device_tree *root,
 const char *path,
 int bool);

INLINE_DEVICE_TREE device_tree *device_tree_add_found_device
(device_tree *root,
 const char *path);

/* query the device tree */

INLINE_DEVICE_TREE const device *device_tree_find_device
(device_tree *root,
 const char *path);

INLINE_DEVICE_TREE signed_word device_tree_find_integer
(device_tree *root,
 const char *path);

INLINE_DEVICE_TREE const char *device_tree_find_string
(device_tree *root,
 const char *path);

INLINE_DEVICE_TREE int device_tree_find_boolean
(device_tree *root,
 const char *path);


/* initialize the entire tree */

INLINE_DEVICE_TREE void device_tree_init
(device_tree *root,
 psim *system);


/* traverse the tree eiter pre or post fix */

typedef void (device_tree_traverse_function)
     (device_tree *device,
      void *data);

INLINE_DEVICE_TREE void device_tree_traverse
(device_tree *root,
 device_tree_traverse_function *prefix,
 device_tree_traverse_function *postfix,
 void *data);


/* dump a node, this can be passed to the device_tree_traverse()
   function to dump out the entire device tree */

INLINE_DEVICE_TREE void device_tree_dump
(device_tree *device,
 void *ignore_data_argument);


/* Parse a device name, various formats:

   uw: unsigned_word
   u: unsigned
   c: string */

INLINE_DEVICE_TREE int scand_uw
(const char *name,
 unsigned_word *uw1);
 
INLINE_DEVICE_TREE int scand_uw_u
(const char *name,
 unsigned_word *uw1,
 unsigned *u2);
 
INLINE_DEVICE_TREE int scand_uw_u_u
(const char *name,
 unsigned_word *uw1,
 unsigned *u2,
 unsigned *u3);
 
INLINE_DEVICE_TREE int scand_uw_uw_u
(const char *name,
 unsigned_word *uw1,
 unsigned_word *uw2,
 unsigned *u3);
 
INLINE_DEVICE_TREE int scand_c
(const char *name,
 char *c1, int c1size);

INLINE_DEVICE_TREE int scand_c_uw_u
(const char *name,
 char *c1, int c1size,
 unsigned_word *uw2,
 unsigned *u3);

INLINE_DEVICE_TREE char *printd_uw_u
(const char *name,
 unsigned_word uw1,
 unsigned u2);

INLINE_DEVICE_TREE char *printd_uw_u_u
(const char *name,
 unsigned_word uw1,
 unsigned u2,
 unsigned u3);

INLINE_DEVICE_TREE char *printd_uw_u_u_c
(const char *name,
 unsigned_word uw1,
 unsigned u2,
 unsigned u3,
 const char *c4);

INLINE_DEVICE_TREE char *printd_c
(const char *name,
 const char *c1);

INLINE_DEVICE_TREE char *printd_c_uw
(const char *name,
 const char *c1,
 unsigned_word uw2);

#endif /* _DEVICE_TREE_H_ */
