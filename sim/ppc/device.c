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


#ifndef _DEVICE_C_
#define _DEVICE_C_

#include <stdio.h>

#include "device_table.h"

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#else
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#endif

#include <ctype.h>



typedef struct _device_property_entry device_property_entry;
struct _device_property_entry {
  const char *name;
  device_property_entry *next;
  device_property *value;
};


/* A device */
struct _device {
  /* my name is ... */
  const char *name;
  const char *full_name;
  /* device tree */
  device *parent;
  device *children;
  device *sibling;
  /* hw/sw callbacks */
  void *data; /* device specific data */
  const device_callbacks *callback;
  /* device properties */
  device_property_entry *properties;
};


INLINE_DEVICE\
(device *)
device_create(const char *name,
	      device *parent)
{
  device_descriptor *descr;
  int name_len;
  char *chp;
  chp = strchr(name, '@');
  name_len = (chp == NULL ? strlen(name) : chp - name);
  for (descr = device_table; descr->name != NULL; descr++) {
    if (strncmp(name, descr->name, name_len) == 0
	&& (descr->name[name_len] == '\0'
	    || descr->name[name_len] == '@')) {
      void *data = (descr->creator != NULL
		    ? descr->creator(name, parent)
		    : NULL);
      return device_create_from(name, data, descr->callbacks, parent);
    }
  }
  error("device_create() unknown device %s\n", name);
  return NULL;
}

INLINE_DEVICE\
(device *)
device_create_from(const char *name,
		   void *data,
		   const device_callbacks *callbacks,
		   device *parent)
{
  device *new_device = ZALLOC(device);
  new_device->data = data;
  new_device->name = strdup(name);
  new_device->callback = callbacks;
  new_device->parent = parent;
  return new_device;
}


INLINE_DEVICE\
(device *)
device_parent(device *me)
{
  return me->parent;
}

INLINE_DEVICE\
(device *)
device_sibling(device *me)
{
  return me->sibling;
}

INLINE_DEVICE\
(device *)
device_child(device *me)
{
  return me->children;
}

INLINE_DEVICE\
(const char *)
device_name(device *me)
{
  return me->name;
}

INLINE_DEVICE\
(void *)
device_data(device *me)
{
  return me->data;
}

INLINE_DEVICE\
(void)
device_traverse_properties(device *me,
			   device_traverse_property_function *traverse,
			   void *data)
{
  device_property_entry *entry = me->properties;
  while (entry != NULL) {
    traverse(me, entry->name, data);
    entry = entry->next;
  }
}

INLINE_DEVICE\
(void)
device_init(device *me,
	    psim *system)
{
  me->callback->init(me, system);
}

INLINE_DEVICE\
(void)
device_attach_address(device *me,
		      const char *name,
		      attach_type attach,
		      int space,
		      unsigned_word addr,
		      unsigned nr_bytes,
		      access_type access,
		      device *who) /*callback/default*/
{
  me->callback->attach_address(me, name, attach, space,
				addr, nr_bytes, access, who);
}

INLINE_DEVICE\
(void)
device_detach_address(device *me,
		      const char *name,
		      attach_type attach,
		      int space,
		      unsigned_word addr,
		      unsigned nr_bytes,
		      access_type access,
		      device *who) /*callback/default*/
{
  me->callback->detach_address(me, name, attach, space,
				addr, nr_bytes, access, who);
}

INLINE_DEVICE\
(unsigned)
device_io_read_buffer(device *me,
		      void *dest,
		      int space,
		      unsigned_word addr,
		      unsigned nr_bytes,
		      cpu *processor,
		      unsigned_word cia)
{
  return me->callback->io_read_buffer(me, dest, space,
				       addr, nr_bytes,
				       processor, cia);
}

INLINE_DEVICE\
(unsigned)
device_io_write_buffer(device *me,
		       const void *source,
		       int space,
		       unsigned_word addr,
		       unsigned nr_bytes,
		       cpu *processor,
		       unsigned_word cia)
{
  return me->callback->io_write_buffer(me, source, space,
				       addr, nr_bytes,
				       processor, cia);
}

INLINE_DEVICE\
(unsigned)
device_dma_read_buffer(device *me,
		       void *dest,
		       int space,
		       unsigned_word addr,
		       unsigned nr_bytes)
{
  return me->callback->dma_read_buffer(me, dest, space,
				       addr, nr_bytes);
}

INLINE_DEVICE\
(unsigned)
device_dma_write_buffer(device *me,
			const void *source,
			int space,
			unsigned_word addr,
			unsigned nr_bytes,
			int violate_read_only_section)
{
  return me->callback->dma_write_buffer(me, source, space,
					addr, nr_bytes,
					violate_read_only_section);
}

INLINE_DEVICE\
(void)
device_attach_interrupt(device *me,
			device *who,
			int interrupt_line,
			const char *name)
{
  me->callback->attach_interrupt(me, who, interrupt_line, name);
}

INLINE_DEVICE\
(void)
device_detach_interrupt(device *me,
			device *who,
			int interrupt_line,
			const char *name)
{
  me->callback->detach_interrupt(me, who, interrupt_line, name);
}

INLINE_DEVICE\
(void)
device_interrupt(device *me,
		 device *who,
		 int interrupt_line,
		 int interrupt_status,
		 cpu *processor,
		 unsigned_word cia)
{
  me->callback->interrupt(me, who, interrupt_line, interrupt_status,
			   processor, cia);
}

INLINE_DEVICE\
(void)
device_interrupt_ack(device *me,
		     int interrupt_line,
		     int interrupt_status)
{
  me->callback->interrupt_ack(me, interrupt_line, interrupt_status);
}

EXTERN_DEVICE\
(void)
device_ioctl(device *me,
	     psim *system,
	     cpu *processor,
	     unsigned_word cia,
	     ...)
{
  va_list ap;
  va_start(ap, cia);
  me->callback->ioctl(me, system, processor, cia, ap);
  va_end(ap);
}
      

/* Manipulate properties attached to devices */

STATIC_INLINE_DEVICE\
(device_property *)
device_add_property(device *me,
		    const char *property,
		    device_property_type type,
		    const void *array,
		    int sizeof_array)
{
  device_property_entry *new_entry = 0;
  device_property *new_value = 0;
  void *new_array = 0;
  /* find the list end */
  device_property_entry **insertion_point = &me->properties;
  while (*insertion_point != NULL) {
    if (strcmp((**insertion_point).name, property) == 0)
      return (**insertion_point).value;
    insertion_point = &(**insertion_point).next;
  }
  /* alloc data for the new property */
  new_entry = ZALLOC(device_property_entry);
  new_value = ZALLOC(device_property);
  new_array = (sizeof_array > 0
	       ? zalloc(sizeof_array)
	       : (void*)0);
  /* insert the new property into the list */
  *insertion_point = new_entry;
  new_entry->name = strdup(property);
  new_entry->value = new_value;
  new_value->type = type;
  new_value->sizeof_array = sizeof_array;
  new_value->array = new_array;
  if (sizeof_array > 0)
    memcpy(new_array, array, sizeof_array);
  return new_value;
}

INLINE_DEVICE\
(void)
device_add_array_property(device *me,
			  const char *property,
			  const void *array,
			  int sizeof_array)
{
  TRACE(trace_devices,
	("device_add_array_property(me=0x%lx, property=%s, ...)\n",
	 (long)me, property));
  device_add_property(me, property,
		      array_property, array, sizeof_array);
}

INLINE_DEVICE\
(void)
device_add_integer_property(device *me,
			    const char *property,
			    signed32 integer)
{
  TRACE(trace_devices,
	("device_add_integer_property(me=0x%lx, property=%s, integer=%ld)\n",
	 (long)me, property, (long)integer));
  H2BE(integer);
  device_add_property(me, property, integer_property,
		      &integer, sizeof(integer));
}

INLINE_DEVICE\
(void)
device_add_boolean_property(device *me,
			    const char *property,
			    int boolean)
{
  signed32 new_boolean = (boolean ? -1 : 0);
  TRACE(trace_devices,
	("device_add_boolean(me=0x%lx, property=%s, boolean=%d)\n",
	 (long)me, property, boolean));
  device_add_property(me, property, boolean_property,
		      &new_boolean, sizeof(new_boolean));
}

INLINE_DEVICE\
(void)
device_add_null_property(device *me,
			 const char *property)
{
  TRACE(trace_devices,
	("device_add_null(me=0x%lx, property=%s)\n",
	 (long)me, property));
  device_add_property(me, property, null_property,
		      NULL, 0);
}

INLINE_DEVICE\
(void)
device_add_string_property(device *me,
			   const char *property,
			   const char *string)
{

  TRACE(trace_devices,
	("device_add_property(me=0x%lx, property=%s, string=%s)\n",
	 (long)me, property, string));
  device_add_property(me, property, string_property,
		      string, strlen(string) + 1);
}

INLINE_DEVICE\
(const device_property *)
device_find_property(device *me,
		     const char *property)
{
  if (me != (device*)0) {
    device_property_entry *entry = me->properties;
    while (entry != (device_property_entry*)0) {
      if (strcmp(entry->name, property) == 0)
	return entry->value;
      entry = entry->next;
    }
  }
  return (device_property*)0;
}

INLINE_DEVICE\
(const char *)
device_find_next_property(device *me,
			  const char *property)
{
  if (me != NULL) {
    if (property == NULL || strcmp(property, "") == 0) {
      return (me->properties != NULL
	      ? me->properties->name
	      : NULL);
    }
    else {
      device_property_entry *entry = me->properties;
      while (entry != NULL) {
	if (strcmp(entry->name, property) == 0)
	  return (entry->next != NULL
		  ? entry->next->name
		  : NULL);
	entry = entry->next;
      }
    }
  }
  return NULL;
}

INLINE_DEVICE\
(const device_property *)
device_find_array_property(device *me,
			   const char *property)
{
  const device_property *node;
  TRACE(trace_devices,
	("device_find_integer(me=0x%lx, property=%s)\n",
	 (long)me, property));
  node = device_find_property(me, property);
  if (node == (device_property*)0
      || node->type != array_property)
    error("%s property %s not found or of wrong type\n",
	  me->name, property);
  return node;
}

INLINE_DEVICE\
(signed_word)
device_find_integer_property(device *me,
			     const char *property)
{
  const device_property *node;
  signed32 integer;
  TRACE(trace_devices,
	("device_find_integer(me=0x%lx, property=%s)\n",
	 (long)me, property));
  node = device_find_property(me, property);
  if (node == (device_property*)0
      || node->type != integer_property)
    error("%s property %s not found or of wrong type\n",
	  me->name, property);
  ASSERT(sizeof(integer) == node->sizeof_array);
  memcpy(&integer, node->array, sizeof(integer));
  BE2H(integer);
  return integer;
}

INLINE_DEVICE\
(int)
device_find_boolean_property(device *me,
			     const char *property)
{
  const device_property *node;
  unsigned32 boolean;
  TRACE(trace_devices,
	("device_find_boolean(me=0x%lx, property=%s)\n",
	 (long)me, property));
  node = device_find_property(me, property);
  if (node == (device_property*)0
      || node->type != boolean_property)
    error("%s property %s not found or of wrong type\n",
	  me->name, property);
  ASSERT(sizeof(boolean) == node->sizeof_array);
  memcpy(&boolean, node->array, sizeof(boolean));
  return boolean;
}

INLINE_DEVICE\
(const char *)
device_find_string_property(device *me,
			    const char *property)
{
  const device_property *node;
  const char *string;
  TRACE(trace_devices,
	("device_find_string(me=0x%lx, property=%s)\n",
	 (long)me, property));
  node = device_find_property(me, property);
  if (node == (device_property*)0
      || node->type != string_property)
    error("%s property %s not found or of wrong type\n",
	  me->name, property);
  string = node->array;
  ASSERT(strlen(string) + 1 == node->sizeof_array);
  return string;
}


/* determine the full name of the device.  If buf is specified it is
   stored in there.  Failing that, a safe area of memory is allocated */
STATIC_INLINE_DEVICE\
(const char *)
device_tree_full_name(device *leaf,
		      char *buf,
		      unsigned sizeof_buf)
{
  /* get a buffer */
  char full_name[1024];
  if (buf == (char*)0) {
    buf = full_name;
    sizeof_buf = sizeof(full_name);
  }

  /* construct a name */
  if (leaf->parent == NULL) {
    if (sizeof_buf < 1)
      error("device_full_name() buffer overflow\n");
    *buf = '\0';
  }
  else {
    device_tree_full_name(leaf->parent, buf, sizeof_buf);
    if (strlen(buf) + strlen("/") + strlen(leaf->name) + 1 > sizeof_buf)
      error("device_full_name() buffer overflow\n");
    strcat(buf, "/");
    strcat(buf, leaf->name);
  }
  
  /* return it usefully */
  if (buf == full_name)
    buf = strdup(full_name);
  return buf;
}


/* find/create a node in the device tree */

typedef enum {
  device_tree_return_null = 2,
  device_tree_abort = 3,
} device_tree_action;

STATIC_INLINE_DEVICE\
(device *)
device_tree_find_node(device *root,
		      const char *path,
		      const char *full_path,
		      device_tree_action action)
{
  const char *name;
  int strlen_name;
  device *child;

  /* strip off any leading `/', `../' or `./' */
  while (1) {
    if (strncmp(path, "/", strlen("/")) == 0) {
      while (root != NULL && root->parent != NULL)
	root = root->parent;
      path += strlen("/");
    }
    else if (strncmp(path, "./", strlen("./")) == 0) {
      root = root;
      path += strlen("./");
    }
    else if (strncmp(path, "../", strlen("../")) == 0) {
      if (root != NULL && root->parent != NULL)
	root = root->parent;
      path += strlen("../");
    }
    else {
      break;
    }
  }

  /* parse the driver_name/unit-address */
  ASSERT(*path != '/');
  name = path;
  while (isalnum(*path)
	 || *path == ',' || *path == ',' || *path == '_'
	 || *path == '+' || *path == '-')
    path++;
  if ((*path != '/' && *path != '@' && *path != ':' && *path != '\0')
      || (name == path && *name != '\0'))
    error("device_tree: path %s invalid at %s\n", full_path, path);

  /* parse the unit-address */
  if (*path == '@') {
    path++;
    while ((*path != '\0' && *path != ':' && *path != '/')
	   || (*path == ':' && path[-1] == '\\')
	   || (*path == '/' && path[-1] == '\\'))
      path++;
  }
  strlen_name = path - name;

  /* skip the device-arguments */
  if (*path == ':') {
    path++;
    while ((*path != '\0' && *path != '/' && *path != ':' && *path != '@')
	   || (*path == '/' && path[-1] == '\\')
	   || (*path == ':' && path[-1] == '\\')
	   || (*path == '@' && path[-1] == '\\'))
      path++;
  }

  /* sanity checks */
  if (*path != '\0' && *path != '/')
    error("device_tree: path %s invalid at %s\n", full_path, path);

  /* leaf? and growing? */
  if (name[0] == '\0') {
    return root;
  }
  else if (root != NULL) {
    for (child = root->children;
	 child != NULL;
	 child = child->sibling) {
      if (strncmp(name, child->name, strlen_name) == 0
	  && strlen(child->name) >= strlen_name
	  && (child->name[strlen_name] == '\0'
	      || child->name[strlen_name] == '@')) {
	if (*path == '\0')
	  return child;
	else
	  return device_tree_find_node(child,
				       path + 1/* / */,
				       full_path,
				       action);
      }
    }
  }

  /* search failed, take default action */
  switch (action) {
  case device_tree_return_null:
    return NULL;
  case device_tree_abort:
    error("device_tree_find_node() could not find %s in tree\n",
	  full_path);
    return NULL;
  default:
    error("device_tree_find_node() invalid default action %d\n", action);
    return NULL;
  }
}


/* grow the device tree */

INLINE_DEVICE\
(device *)
device_tree_add_device(device *root,
		       const char *prefix,
		       device *new_sub_tree)
{
  device *parent;
  TRACE(trace_device_tree,
	("device_tree_add_device(root=0x%lx, prefix=%s, dev=0x%lx)\n",
	 (long)root, prefix, (long)new_sub_tree));

  /* find our parent */
  parent = device_tree_find_node(root,
				 prefix,
				 prefix, /* full-path */
				 device_tree_abort);

  /* create/insert a new child */
  new_sub_tree->parent = parent;
  if (parent != NULL) {
    device **sibling = &parent->children;
    while ((*sibling) != NULL)
      sibling = &(*sibling)->sibling;
    *sibling = new_sub_tree;
  }

  return new_sub_tree;
}

INLINE_DEVICE\
(device *)
device_tree_find_device(device *root,
			const char *path)
{
  device *node;
  TRACE(trace_device_tree,
	("device_tree_find_device_tree(root=0x%lx, path=%s)\n",
	 (long)root, path));
  node = device_tree_find_node(root,
			       path,
			       path, /* full-name */
			       device_tree_return_null);
  return node;
}


/* init all the devices */

STATIC_INLINE_DEVICE\
(void)
device_tree_init_device(device *root,
			void *data)
{
  psim *system;
  system = (psim*)data;
  TRACE(trace_device_tree,
	("device_tree_init() initializing device=0x%lx:%s\n",
	 (long)root, root->full_name));
  device_init(root, system);
}


INLINE_DEVICE\
(void)
device_tree_init(device *root,
		 psim *system)
{
  TRACE(trace_device_tree,
	("device_tree_init(root=0x%lx, system=0x%lx)\n", (long)root, (long)system));
  device_tree_traverse(root, device_tree_init_device, NULL, system);
  TRACE(trace_device_tree,
	("device_tree_init() = void\n"));
}


/* traverse a device tree applying prefix/postfix functions to it */

INLINE_DEVICE\
(void)
device_tree_traverse(device *root,
		     device_tree_traverse_function *prefix,
		     device_tree_traverse_function *postfix,
		     void *data)
{
  device *child;
  if (prefix != NULL)
    prefix(root, data);
  for (child = root->children; child != NULL; child = child->sibling) {
    device_tree_traverse(child, prefix, postfix, data);
  }
  if (postfix != NULL)
    postfix(root, data);
}


/* dump out a device node and addresses */

INLINE_DEVICE\
(void)
device_tree_dump(device *device,
		 void *ignore_data_argument)
{
  printf_filtered("(device_tree@0x%lx\n", (long)device);
  printf_filtered(" (parent 0x%lx)\n", (long)device->parent);
  printf_filtered(" (children 0x%lx)\n", (long)device->children);
  printf_filtered(" (sibling 0x%lx)\n", (long)device->sibling);
  printf_filtered(" (name %s)\n", device->name);
  error("FIXME - need to print out properties\n");
  printf_filtered(")\n");
}


/* lookup/create a device various formats */

STATIC_INLINE_DEVICE\
(void)
u_strcat(char *buf,
	 unsigned_word uw)
{
  if (MASKED64(uw, 32, 63) == uw
      || WITH_HOST_WORD_BITSIZE == 64) {
    char *end = strchr(buf, '\0');
    sprintf(end, "0x%x", (unsigned)uw);
  }
  else {
    char *end = strchr(buf, '\0');
    sprintf(end, "0x%x%08x",
	    (unsigned)EXTRACTED64(uw, 0, 31),
	    (unsigned)EXTRACTED64(uw, 32, 63));
  }
}

STATIC_INLINE_DEVICE\
(void)
c_strcat(char *buf,
	const char *c)
{
  char *end = strchr(buf, '\0');
  while (*c) {
    if (*c == '/' || *c == ',')
      *end++ = '\\';
    *end++ = *c++;
  }
  *end = '\0';
}

INLINE_DEVICE\
(device *)
device_tree_add_found(device *root,
		      const char *prefix,
		      const char *name)
{
  device *parent;
  device *new_device;
  device *new_node;
  TRACE(trace_device_tree,
	("device_tree_add_found(root=0x%lx, prefix=%s, name=%lx)\n",
	 (unsigned long)root, prefix, (unsigned long)name));
  parent = device_tree_find_node(root, prefix, prefix,
				 device_tree_abort);
  new_device = device_tree_find_device(parent, name);
  if (new_device != NULL)
    return new_device;
  else {
    new_device = device_create(name, parent);
    new_node = device_tree_add_device(parent, "", new_device);
    ASSERT(new_device == new_node);
    return new_node;
  }
}

INLINE_DEVICE\
(device *)
device_tree_add_found_c(device *root,
			const char *prefix,
			const char *name,
			const char *c1)
{
  char buf[1024];
  strcpy(buf, name);
  strcat(buf, "@");
  c_strcat(buf, c1);
  if (strlen(buf) + 1 >= sizeof(buf))
    error("device_tree_add_found_c - buffer overflow\n");
  return device_tree_add_found(root, prefix, buf);
}

INLINE_DEVICE\
(device *)
device_tree_add_found_c_uw(device *root,
			   const char *prefix,
			   const char *name,
			   const char *c1,
			   unsigned_word uw2)
{
  char buf[1024];
  strcpy(buf, name);
  strcat(buf, "@");
  c_strcat(buf, c1);
  strcat(buf, ",");
  u_strcat(buf, uw2);
  if (strlen(buf) + 1 >= sizeof(buf))
    error("device_tree_add_found_* - buffer overflow\n");
  return device_tree_add_found(root, prefix, buf);
}

INLINE_DEVICE\
(device *)
device_tree_add_found_uw_u(device *root,
			   const char *prefix,
			   const char *name,
			   unsigned_word uw1,
			   unsigned u2)
{
  char buf[1024];
  strcpy(buf, name);
  strcat(buf, "@");
  u_strcat(buf, uw1);
  strcat(buf, ",");
  u_strcat(buf, u2);
  if (strlen(buf) + 1 >= sizeof(buf))
    error("device_tree_add_found_* - buffer overflow\n");
  return device_tree_add_found(root, prefix, buf);
}

INLINE_DEVICE\
(device *)
device_tree_add_found_uw_u_u(device *root,
			     const char *prefix,
			     const char *name,
			     unsigned_word uw1,
			     unsigned u2,
			     unsigned u3)
{
  char buf[1024];
  strcpy(buf, name);
  strcat(buf, "@");
  u_strcat(buf, uw1);
  strcat(buf, ",");
  u_strcat(buf, u2);
  strcat(buf, ",");
  u_strcat(buf, u3);
  if (strlen(buf) + 1 >= sizeof(buf))
    error("device_tree_add_found_* - buffer overflow\n");
  return device_tree_add_found(root, prefix, buf);
}

INLINE_DEVICE\
(device *)
device_tree_add_found_uw_u_u_c(device *root,
			       const char *prefix,
			       const char *name,
			       unsigned_word uw1,
			       unsigned u2,
			       unsigned u3,
			       const char *c4)
{
  char buf[1024];
  strcpy(buf, name);
  strcat(buf, "@");
  u_strcat(buf, uw1);
  strcat(buf, ",");
  u_strcat(buf, u2);
  strcat(buf, ",");
  u_strcat(buf, u3);
  strcat(buf, ",");
  c_strcat(buf, c4);
  if (strlen(buf) + 1 >= sizeof(buf))
    error("device_tree_add_found_* - buffer overflow\n");
  return device_tree_add_found(root, prefix, buf);
}

INLINE_DEVICE\
(device *)
device_tree_add_found_uw_uw_u_u_c(device *root,
				  const char *prefix,
				  const char *name,
				  unsigned_word uw1,
				  unsigned_word uw2,
				  unsigned u3,
				  unsigned u4,
				  const char *c5)
{
  char buf[1024];
  strcpy(buf, name);
  strcat(buf, "@");
  u_strcat(buf, uw1);
  strcat(buf, ",");
  u_strcat(buf, uw2);
  strcat(buf, ",");
  u_strcat(buf, u3);
  strcat(buf, ",");
  u_strcat(buf, u4);
  strcat(buf, ",");
  c_strcat(buf, c5);
  if (strlen(buf) + 1 >= sizeof(buf))
    error("device_tree_add_found_* - buffer overflow\n");
  return device_tree_add_found(root, prefix, buf);
}

INLINE_DEVICE\
(device *)
device_tree_add_found_uw_uw_u_u_u(device *root,
				  const char *prefix,
				  const char *name,
				  unsigned_word uw1,
				  unsigned_word uw2,
				  unsigned u3,
				  unsigned u4,
				  unsigned u5)
{
  char buf[1024];
  strcpy(buf, name);
  strcat(buf, "@");
  u_strcat(buf, uw1);
  strcat(buf, ",");
  u_strcat(buf, uw2);
  strcat(buf, ",");
  u_strcat(buf, u3);
  strcat(buf, ",");
  u_strcat(buf, u4);
  strcat(buf, ",");
  u_strcat(buf, u5);
  if (strlen(buf) + 1 >= sizeof(buf))
    error("device_tree_add_found_* - buffer overflow\n");
  return device_tree_add_found(root, prefix, buf);
}


/* Parse a device name, various formats */

#define SCAN_INIT(NAME) \
  char *START = (char*)0; \
  char *END = (char*)0; \
  int COUNT = -1; \
  /* find the first element */ \
  END = strchr(NAME, '@'); \
  if (END == (char*)0) \
    return COUNT; \
  COUNT += 1; \
  START = END + 1

#define SCAN_END \
  return COUNT

#define SCAN_U(U) \
do { \
  *U = strtoul(START, &END, 0); \
  if (START == END) \
    return COUNT; \
  COUNT += 1; \
  if (*END != ',') \
    return COUNT; \
  START = END + 1; \
} while (0)

#define SCAN_P(P) \
do { \
  *P = (void*)(unsigned)strtouq(START, END, 0); \
  if (START == END) \
    return COUNT; \
  COUNT += 1; \
  if (*END != ',') \
    return COUNT; \
  START = END + 1; \
} while (0)

#define SCAN_C(C, SIZE) \
do { \
  char *chp = C; \
  END = START; \
  while (*END != '\0' && *END != ',') { \
    if (*END == '\\') \
      END += 1; \
    *chp = *END; \
    chp += 1; \
    END += 1; \
    if ((SIZE) <= ((END) - (START))) \
      return COUNT; /* overflow */ \
  } \
  *chp = '\0'; \
  if (START == END) \
    return COUNT; \
  COUNT += 1; \
  if (*END != ',') \
    return COUNT; \
  START = END + 1; \
} while (0)

INLINE_DEVICE\
(int)
scand_c(const char *name,
	char *c1,
	unsigned c1size)
{
  SCAN_INIT(name);
  SCAN_C(c1, c1size);
  SCAN_END;
}

INLINE_DEVICE\
(int)
scand_c_uw_u(const char *name,
	     char *c1,
	     unsigned c1size,
	     unsigned_word *uw2,
	     unsigned *u3)
{
  SCAN_INIT(name);
  SCAN_C(c1, c1size);
  SCAN_U(uw2);
  SCAN_U(u3);
  SCAN_END;
}

INLINE_DEVICE\
(int)
scand_uw(const char *name,
	 unsigned_word *uw1)
{
  SCAN_INIT(name);
  SCAN_U(uw1);
  SCAN_END;
}

INLINE_DEVICE\
(int)
scand_uw_c(const char *name,
	   unsigned_word *uw1,
	   char *c2,
	   unsigned c2size)
{
  SCAN_INIT(name);
  SCAN_U(uw1);
  SCAN_C(c2, c2size);
  SCAN_END;
}

INLINE_DEVICE\
(int)
scand_uw_u(const char *name,
	   unsigned_word *uw1,
	   unsigned *u2)
{
  SCAN_INIT(name);
  SCAN_U(uw1);
  SCAN_U(u2);
  SCAN_END;
}

INLINE_DEVICE\
(int)
scand_uw_u_u(const char *name,
	     unsigned_word *uw1,
	     unsigned *u2,
	     unsigned *u3)
{
  SCAN_INIT(name);
  SCAN_U(uw1);
  SCAN_U(u2);
  SCAN_U(u3);
  SCAN_END;
}

INLINE_DEVICE\
(int)
scand_uw_u_u_c(const char *name,
	       unsigned_word *uw1,
	       unsigned *u2,
	       unsigned *u3,
	       char *c4,
	       unsigned c4size)
{
  SCAN_INIT(name);
  SCAN_U(uw1);
  SCAN_U(u2);
  SCAN_U(u3);
  SCAN_C(c4, c4size);
  SCAN_END;
}

INLINE_DEVICE\
(int)
scand_uw_uw(const char *name,
	    unsigned_word *uw1,
	    unsigned_word *uw2)
{
  SCAN_INIT(name);
  SCAN_U(uw1);
  SCAN_U(uw2);
  SCAN_END;
}

INLINE_DEVICE\
(int)
scand_uw_uw_u(const char *name,
	      unsigned_word *uw1,
	      unsigned_word *uw2,
	      unsigned *u3)
{
  SCAN_INIT(name);
  SCAN_U(uw1);
  SCAN_U(uw2);
  SCAN_U(u3);
  SCAN_END;
}

INLINE_DEVICE\
(int)
scand_uw_uw_u_u_c(const char *name,
		  unsigned_word *uw1,
		  unsigned_word *uw2,
		  unsigned *u3,
		  unsigned *u4,
		  char *c5,
		  unsigned c5size)
{
  SCAN_INIT(name);
  SCAN_U(uw1);
  SCAN_U(uw2);
  SCAN_U(u3);
  SCAN_U(u4);
  SCAN_C(c5, c5size);
  SCAN_END;
}

INLINE_DEVICE\
(int)
scand_uw_uw_u_u_u(const char *name,
		  unsigned_word *uw1,
		  unsigned_word *uw2,
		  unsigned *u3,
		  unsigned *u4,
		  unsigned *u5)
{
  SCAN_INIT(name);
  SCAN_U(uw1);
  SCAN_U(uw2);
  SCAN_U(u3);
  SCAN_U(u4);
  SCAN_U(u5);
  SCAN_END;
}


#endif /* _DEVICE_C_ */
