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


#ifndef _DEVICE_TREE_C_
#define _DEVICE_TREE_C_

#ifndef STATIC_INLINE_DEVICE_TREE
#define STATIC_INLINE_DEVICE_TREE STATIC_INLINE
#endif

#include <stdio.h>

#include "basics.h"
#include "device_tree.h"

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

typedef enum {
  node_any = 0,
  node_device,
  node_integer,
  node_boolean,
  node_string
} node_type;

static char *node_type_names[] = {
  "any",
  "device",
  "integer",
  "boolean",
  "string",
  NULL,
};

struct _device_tree {
  /* where i am */
  device_tree *parent;
  device_tree *children;
  device_tree *sibling;
  /* what i am */
  node_type type;
  const char *name;
  /* the value */
  const device *device;
  int boolean;
  const char *string;
  signed_word integer;
};


STATIC_INLINE_DEVICE_TREE device_tree *
new_device_tree(device_tree *parent,
		const char *name,
		node_type type)
{
  device_tree *new_node;
  new_node = ZALLOC(device_tree);
  new_node->parent = parent;
  new_node->name = strdup(name);
  new_node->type = type;
  if (parent != NULL) {
    device_tree **sibling = &parent->children;
    while ((*sibling) != NULL)
      sibling = &(*sibling)->sibling;
    *sibling = new_node;
  }
  return new_node;
}


/* find/create a node in the device tree */

typedef enum {
  device_tree_grow = 1,
  device_tree_return_null = 2,
  device_tree_abort = 3,
} device_tree_action;

STATIC_INLINE_DEVICE_TREE device_tree *
device_tree_find_node(device_tree *root,
		      const char *path,
		      const char *full_path,
		      node_type type,
		      device_tree_action action)
{
  const char *chp;
  int name_len;
  device_tree *child;

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

  /* find the qualified (with @) and unqualified names in the path,
     remembering to skip any "\/" */
  chp = path;
  do {
    chp = strchr(chp+1, '/');
  } while (chp != NULL && chp[-1] == '\\');
  name_len = (chp == NULL
	      ? strlen(path)
	      : chp - path);

  /* leaf? and growing? */
  if (root != NULL) {
    for (child = root->children;
	 child != NULL;
	 child = child->sibling) {
      if (strncmp(path, child->name, name_len) == 0
	  && (strlen(child->name) == name_len
	      || (strchr(child->name, '@')
		  == child->name + name_len))) {
	if (path[name_len] == '\0') {
	  if (action == device_tree_grow)
	    error("device_tree_find_node() node %s already present\n",
		  full_path);
	  if (type != node_any && child->type != type) {
	    if (action == device_tree_return_null)
	      return NULL;
	    else
	      error("device_tree_find_node() node %s is not of type %s\n",
		    full_path, node_type_names[type]);
	  }
	  else
	    return child;
	}
	else
	  return device_tree_find_node(child,
				       path + name_len + 1,
				       full_path,
				       type,
				       action);
      }
    }
  }

  /* search failed, take default action */
  switch (action) {
  case device_tree_grow:
    if (path[name_len] != '\0')
      error("device_tree_find_node() a parent of %s missing\n",
	    full_path);
    return new_device_tree(root, path, type);
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

INLINE_DEVICE_TREE device_tree *
device_tree_add_passthrough(device_tree *root,
			    const char *path)
{
  device_tree *new_node;
  TRACE(trace_device_tree,
	("device_tree_add_passthrough(root=0x%x, path=%s)\n", root, path));
  new_node = device_tree_find_node(root,
				   path,
				   path, /*full_path*/
				   node_device,
				   device_tree_grow);
  new_node->device = device_create_from(new_node->name,
					path,
					NULL,
					passthrough_device_callbacks(),
					new_node->parent->device);
  
  TRACE(trace_device_tree,
	("device_tree_add_passthrough() = 0x%x\n", new_node));
  return new_node;
}


INLINE_DEVICE_TREE device_tree *
device_tree_add_device(device_tree *root,
		       const char *path,
		       const device *dev)
{
  device_tree *new_node;
  TRACE(trace_device_tree,
	("device_tree_add_device(root=0x%x, path=%s, dev=0x%x)\n",
	 root, path, dev));
  new_node = device_tree_find_node(root,
				   path,
				   path, /* full-path */
				   node_device,
				   device_tree_grow);
  new_node->device = dev;
  TRACE(trace_device_tree,
	("device_tree_add_device() = 0x%x\n", new_node));
  return new_node;
}

INLINE_DEVICE_TREE device_tree *
device_tree_add_integer(device_tree *root,
			const char *path,
			signed_word integer)
{
  device_tree *new_node;
  TRACE(trace_device_tree,
	("device_tree_add_integer(root=0x%x, path=%s, integer=%d)\n",
	 root, path, integer));
  new_node = device_tree_find_node(root,
				   path,
				   path, /* full-name */
				   node_integer,
				   device_tree_grow);
  new_node->integer = integer;
  TRACE(trace_device_tree,
	("device_tree_add_integer() = 0x%x\n", new_node));
  return new_node;
}

INLINE_DEVICE_TREE device_tree *
device_tree_add_string(device_tree *root,
		       const char *path,
		       const char *string)
{
  device_tree *new_node;
  TRACE(trace_device_tree,
	("device_tree_add_device(root=0x%x, path=%s, string=%s)\n",
	 root, path, string));
  new_node = device_tree_find_node(root,
				   path,
				   path, /* full-name */
				   node_string,
				   device_tree_grow);
  new_node->string = string;
  TRACE(trace_device_tree,
	("device_tree_add_string() = 0x%x\n", new_node));
  return new_node;
}

INLINE_DEVICE_TREE device_tree *
device_tree_add_boolean(device_tree *root,
			const char *path,
			int boolean)
{
  device_tree *new_node;
  TRACE(trace_device_tree,
	("device_tree_add_boolean(root=0x%x, path=%s, boolean=%d)\n",
	 root, path, boolean));
  new_node = device_tree_find_node(root,
				   path,
				   path, /* full-name */
				   node_boolean,
				   device_tree_grow);
  new_node->boolean = boolean;
  TRACE(trace_device_tree,
	("device_tree_add_boolean() = 0x%x\n", new_node));
  return new_node;
}

INLINE_DEVICE_TREE device_tree *
device_tree_add_found_device(device_tree *root,
			     const char *path)
{
  device_tree *new_node;
  TRACE(trace_device_tree,
	("device_tree_add_found_device(root=0x%x, path=%s)\n",
	 root, path));
  new_node = device_tree_add_device(root, path, NULL);
  new_node->device = device_create(new_node->name,
				   path,
				   new_node->parent->device);
  TRACE(trace_device_tree,
	("device_tree_add_found_device() = 0x%x\n", new_node));
  return new_node;
}


/* look up the device tree */

INLINE_DEVICE_TREE const device *
device_tree_find_device(device_tree *root,
			const char *path)
{
  device_tree *node;
  TRACE(trace_device_tree,
	("device_tree_find_device(root=0x%x, path=%s)\n", root, path));
  node = device_tree_find_node(root,
			       path,
			       path, /* full-name */
			       node_device,
			       device_tree_abort);
  TRACE(trace_device_tree,
	("device_tree_find_device() = 0x%x\n", node->device));
  return node->device;
}

INLINE_DEVICE_TREE signed_word
device_tree_find_integer(device_tree *root,
			 const char *path)
{
  device_tree *node;
  TRACE(trace_device_tree,
	("device_tree_find_integer(root=0x%x, path=%s)\n", root, path));
  node = device_tree_find_node(root,
			       path,
			       path, /* full-name */
			       node_integer,
			       device_tree_abort);
  TRACE(trace_device_tree,
	("device_tree_find_integer() = %d\n", node->integer));
  return node->integer;
}

INLINE_DEVICE_TREE const char *
device_tree_find_string(device_tree *root,
			const char *path)
{
  device_tree *node;
  TRACE(trace_device_tree,
	("device_tree_find_string(root=0x%x, path=%s)\n", root, path));
  node = device_tree_find_node(root,
			       path,
			       path, /* full-name */
			       node_string,
			       device_tree_abort);
  TRACE(trace_device_tree,
	("device_tree_find_string() = 0x%x\n", node->string));
  return node->string;
}

INLINE_DEVICE_TREE int
device_tree_find_boolean(device_tree *root,
			 const char *path)
{
  device_tree *node;
  TRACE(trace_device_tree,
	("device_tree_find_boolean(root=0x%x, path=%s)\n", root, path));
  node = device_tree_find_node(root,
			       path,
			       path, /* full-name */
			       node_boolean,
			       device_tree_abort);
  TRACE(trace_device_tree,
	("device_tree_find_boolean() = %d\n", node->boolean));
  return node->boolean;
}


/* init all the devices */

STATIC_INLINE_DEVICE_TREE void
device_tree_init_device(device_tree *root,
			void *data)
{
  psim *system;
  system = (psim*)data;
  if (root->type == node_device) {
    TRACE(trace_device_tree,
	  ("device_tree_init() initializing device=0x%x:%s\n",
	   root->device, root->device->full_name));
    root->device->callback->init(root->device, system);
  }
}


INLINE_DEVICE_TREE void
device_tree_init(device_tree *root,
		 psim *system)
{
  TRACE(trace_device_tree,
	("device_tree_init(root=0x%x, system=0x%x)\n", root, system));
  device_tree_traverse(root, device_tree_init_device, NULL, system);
  TRACE(trace_device_tree,
	("device_tree_init() = void\n"));
}


/* traverse a device tree applying prefix/postfix functions to it */

INLINE_DEVICE_TREE void
device_tree_traverse(device_tree *root,
		     device_tree_traverse_function *prefix,
		     device_tree_traverse_function *postfix,
		     void *data)
{
  device_tree *child;
  if (prefix != NULL)
    prefix(root, data);
  for (child = root->children; child != NULL; child = child->sibling) {
    device_tree_traverse(child, prefix, postfix, data);
  }
  if (postfix != NULL)
    postfix(root, data);
}


/* dump out a device node and addresses */

INLINE_DEVICE_TREE void
device_tree_dump(device_tree *device,
		 void *ignore_data_argument)
{
  printf_filtered("(device_tree@0x%x\n", device);
  printf_filtered(" (parent 0x%x)\n", device->parent);
  printf_filtered(" (children 0x%x)\n", device->children);
  printf_filtered(" (sibling 0x%x)\n", device->sibling);
  printf_filtered(" (type %d)\n", device->type);
  printf_filtered(" (name %s)\n", device->name);
  printf_filtered(" (device 0x%x)\n", device->device);
  printf_filtered(" (boolean %d)\n", device->boolean);
  printf_filtered(" (string %s)\n", device->string);
  printf_filtered(" (integer %d)\n", device->integer);
  printf_filtered(")\n");
}


/* Parse a device name, various formats */

#define SCAN_INIT(START, END, COUNT, NAME) \
  char *START = NULL; \
  char *END = strchr(NAME, '@'); \
  int COUNT = 0; \
  if (END == NULL) \
    return 0; \
  START = END + 1

#define SCAN_U(START, END, COUNT, U) \
do { \
  *U = strtoul(START, &END, 0); \
  if (START == END) \
    return COUNT; \
  COUNT++; \
  if (*END != ',') \
    return COUNT; \
  START = END + 1; \
} while (0)

#define SCAN_P(START, END, COUNT, P) \
do { \
  *P = (void*)(unsigned)strtouq(START, &END, 0); \
  if (START == END) \
    return COUNT; \
  COUNT++; \
  if (*END != ',') \
    return COUNT; \
  START = END + 1; \
} while (0)

#define SCAN_C(START, END, COUNT, C, SIZE) \
do { \
  char *chp = C; \
  END = START; \
  while (*END != '\0' && *END != ',') { \
    if (*END == '\\') \
      END++; \
    *chp = *END; \
    chp += 1; \
    END += 1; \
    if ((SIZE) <= ((END) - (START))) \
      return COUNT; /* overflow */ \
  } \
  *chp = '\0'; \
  if (START == END) \
    return COUNT; \
  COUNT++; \
  if (*END != ',') \
    return COUNT; \
  START = END + 1; \
} while (0)

INLINE_DEVICE_TREE int
scand_uw(const char *name,
	 unsigned_word *uw1)
{
  SCAN_INIT(start, end, count, name);
  SCAN_U(start, end, count, uw1);
  return count;
}

INLINE_DEVICE_TREE int
scand_uw_u(const char *name,
	   unsigned_word *uw1,
	   unsigned *u2)
{
  SCAN_INIT(start, end, count, name);
  SCAN_U(start, end, count, uw1);
  SCAN_U(start, end, count, u2);
  return count;
}

INLINE_DEVICE_TREE int
scand_uw_u_u(const char *name,
	     unsigned_word *uw1,
	     unsigned *u2,
	     unsigned *u3)
{
  SCAN_INIT(start, end, count, name);
  SCAN_U(start, end, count, uw1);
  SCAN_U(start, end, count, u2);
  SCAN_U(start, end, count, u3);
  return count;
}

INLINE_DEVICE_TREE int
scand_uw_uw_u(const char *name,
	      unsigned_word *uw1,
	      unsigned_word *uw2,
	      unsigned *u3)
{
  SCAN_INIT(start, end, count, name);
  SCAN_U(start, end, count, uw1);
  SCAN_U(start, end, count, uw2);
  SCAN_U(start, end, count, u3);
  return count;
}

INLINE_DEVICE_TREE int
scand_c(const char *name,
	char *c1, int c1size)
{
  SCAN_INIT(start, end, count, name);
  SCAN_C(start, end, count, c1, c1size);
  return count;
}

INLINE_DEVICE_TREE int
scand_c_uw_u(const char *name,
	     char *c1, int c1size,
	     unsigned_word *uw2,
	     unsigned *u3)
{
  SCAN_INIT(start, end, count, name);
  SCAN_C(start, end, count, c1, c1size);
  SCAN_U(start, end, count, uw2);
  SCAN_U(start, end, count, u3);
  return count;
}


STATIC_INLINE_DEVICE_TREE void
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

STATIC_INLINE_DEVICE_TREE void
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

STATIC_INLINE_DEVICE_TREE int
c_strlen(const char *c)
{
  int len = 0;
  while (*c) {
    if (*c == '/' || *c == ',')
      len++;
    len++;
    c++;
  }
  return len;
}

enum {
  strlen_unsigned = 10,
  strlen_unsigned_word = 18,
};

INLINE_DEVICE_TREE char *
printd_uw_u(const char *name,
	    unsigned_word uw1,
	    unsigned u2)
{
  int sizeof_buf = (strlen(name)
		    + strlen("@")
		    + strlen_unsigned_word
		    + strlen(",")
		    + strlen_unsigned
		    + 1);
  char *buf = (char*)zalloc(sizeof_buf);
  strcpy(buf, name);
  strcat(buf, "@");
  u_strcat(buf, uw1);
  strcat(buf, ",");
  u_strcat(buf, u2);
  ASSERT(strlen(buf) < sizeof_buf);
  return buf;
}

INLINE_DEVICE_TREE char *
printd_uw_u_u(const char *name,
	      unsigned_word uw1,
	      unsigned u2,
	      unsigned u3)
{
  int sizeof_buf = (strlen(name)
		    + strlen("@")
		    + strlen_unsigned_word
		    + strlen(",")
		    + strlen_unsigned
		    + strlen(",")
		    + strlen_unsigned
		    + 1);
  char *buf = (char*)zalloc(sizeof_buf);
  strcpy(buf, name);
  strcat(buf, "@");
  u_strcat(buf, uw1);
  strcat(buf, ",");
  u_strcat(buf, u2);
  strcat(buf, ",");
  u_strcat(buf, u3);
  ASSERT(strlen(buf) < sizeof_buf);
  return buf;
}

INLINE_DEVICE_TREE char *
printd_uw_u_u_c(const char *name,
		unsigned_word uw1,
		unsigned u2,
		unsigned u3,
		const char *c4)
{
  int sizeof_buf = (strlen(name)
		    + strlen("@")
		    + strlen_unsigned_word
		    + strlen(",")
		    + strlen_unsigned
		    + strlen(",")
		    + strlen_unsigned
		    + strlen(",")
		    + c_strlen(c4)
		    + 1);
  char *buf = (char*)zalloc(sizeof_buf);
  strcpy(buf, name);
  strcat(buf, "@");
  u_strcat(buf, uw1);
  strcat(buf, ",");
  u_strcat(buf, u2);
  strcat(buf, ",");
  u_strcat(buf, u3);
  strcat(buf, ",");
  c_strcat(buf, c4);
  ASSERT(strlen(buf) < sizeof_buf);
  return buf;
}

INLINE_DEVICE_TREE char *
printd_c(const char *name,
	 const char *c1)
{
  int sizeof_buf = (strlen(name)
		    + strlen("@")
		    + c_strlen(c1)
		    + 1);
  char *buf = (char*)zalloc(sizeof_buf);
  strcpy(buf, name);
  strcat(buf, "@");
  c_strcat(buf, c1);
  ASSERT(strlen(buf) < sizeof_buf);
  return buf;
}

INLINE_DEVICE_TREE char *
printd_c_uw(const char *name,
	    const char *c1,
	    unsigned_word uw2)
{
  int sizeof_buf = (strlen(name)
		    + strlen("@")
		    + c_strlen(c1)
		    + strlen(",")
		    + strlen_unsigned_word
		    + 1);
  char *buf = (char*)zalloc(sizeof_buf);
  strcpy(buf, name);
  strcat(buf, "@");
  c_strcat(buf, c1);
  strcat(buf, ",");
  u_strcat(buf, uw2);
  ASSERT(strlen(buf) < sizeof_buf);
  return buf;
}

#endif /* _DEVICE_TREE_C_ */
