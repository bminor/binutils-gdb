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

#include <string.h>

#include "basics.h"
#include "device_tree.h"
#include "devices.h"

#include "bfd.h"

/* Any starting address less than this is assumed to be an OEA program
   rather than VEA.  */
#ifndef OEA_START_ADDRESS
#define	OEA_START_ADDRESS 4096
#endif

#ifndef OEA_MEMORY_SIZE
#define OEA_MEMORY_SIZE 0x100000
#endif

enum { clayton_memory_size = OEA_MEMORY_SIZE };

/* insert the address into the device_nodes sorted list of addresses */
INLINE_DEVICE_TREE void
device_node_add_address(device_node *node,
			unsigned_word lower_bound,
			unsigned size,
			device_access access,
			void *init)
{
  unsigned_word upper_bound = lower_bound + size;
  device_address *new_address;
  device_address **current_address;

  /* find the insertion point */
  current_address = &node->addresses;
  while (*current_address != NULL
	 && (*current_address)->upper_bound >= upper_bound) {
    current_address = &(*current_address)->next_address;
  }

  /* insert */
  new_address = ZALLOC(device_address);
  new_address->lower_bound = lower_bound;
  new_address->upper_bound = lower_bound + size;
  new_address->size = size;
  new_address->access = access;
  new_address->init = init;
  new_address->next_address = *current_address;
  *current_address = new_address;
}


/* create a new device tree optionally making it a child of the parent
   node */

INLINE_DEVICE_TREE device_node *
device_node_create(device_node *parent,
		   char *name,
		   device_type type,
		   device_callbacks *callbacks,
		   void *data)
{
  device_node *new_node;
  new_node = ZALLOC(device_node);
  new_node->parent = parent;
  new_node->name = name;
  new_node->type = type;
  new_node->callbacks = callbacks;
  new_node->data = data;
  if (parent != NULL) {
    new_node->sibling = parent->children;
    parent->children = new_node;
  }
  return new_node;
}


/* Binary file:

   The specified file is a binary, assume VEA is required, construct a
   fake device tree based on the addresses of the text / data segments
   requested by the binary */


/* Update the fake device tree so that memory is allocated for this
   section */
STATIC_INLINE_DEVICE_TREE void
update_memory_node_for_section(bfd *abfd,
			       asection *the_section,
			       PTR obj)
{
  unsigned_word section_vma;
  unsigned_word section_size;
  device_access section_access;
  void *section_init;
  device_node *memory = (device_node*)obj;

  /* skip the section if no memory to allocate */
  if (! (bfd_get_section_flags(abfd, the_section) & SEC_ALLOC))
    return;

  /* check/ignore any sections of size zero */
  section_size = bfd_get_section_size_before_reloc(the_section);
  if (section_size == 0)
    return;

  /* find where it is to go */
  section_vma = bfd_get_section_vma(abfd, the_section);

  TRACE(trace_device_tree,
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

  if (bfd_get_section_flags(abfd, the_section) & SEC_LOAD) {
    section_init = zalloc(section_size);
    if (!bfd_get_section_contents(abfd,
				  the_section,
				  section_init, 0,
				  section_size)) {
      bfd_perror("core:load_section()");
      error("load of data failed");
      return;
    }
  }
  else {
    section_init = NULL;
  }

  /* determine the devices access */
  if (bfd_get_section_flags(abfd, the_section) & SEC_CODE)
    section_access = (device_is_readable | device_is_executable);
  else if (bfd_get_section_flags(abfd, the_section) & SEC_READONLY)
    section_access = device_is_readable;
  else
    section_access = (device_is_readable | device_is_writeable);

  /* find our memory and add this section to its list of addresses */
  device_node_add_address(memory,
			  section_vma,
			  section_size,
			  section_access,
			  section_init);
}


/* construct the device tree from the executable */

STATIC_INLINE_DEVICE_TREE device_node *
create_option_device_node(device_node *root,
			  bfd *image)
{
  int oea = (bfd_get_start_address(image) < OEA_START_ADDRESS);
  int elf = (image->xvec->flavour == bfd_target_elf_flavour);
  device_node *option_node;
  
  /* the option node and than its members */
  option_node = device_node_create(root, "options", options_device,
				   NULL, NULL);

  /* which endian are we ? */
  device_node_create(option_node,
		     "little-endian?",
		     boolean_type_device,
		     NULL,
		     (void*)(image->xvec->byteorder_big_p ? 0 : -1));

  /* what is the initial entry point */
  device_node_create(option_node,
		     "program-counter",
		     integer_type_device,
		     NULL,
		     (void*)(bfd_get_start_address(image)));

  /* address of top of boot stack */
  TRACE(trace_tbd, ("create_optioin_device_node() - TBD - NT/OpenBoot?\n"));
  device_node_create(option_node,
		     "stack-pointer",
		     integer_type_device,
		     NULL,
		     (void *)((oea)
			     ? clayton_memory_size /* OEA */
			     : ((elf)
				? 0xe0000000 /* elf */
				: 0x20000000 /* xcoff */)));

  /* execution environment */
  device_node_create(option_node,
		     "vea?",
		     boolean_type_device,
		     NULL,
		     (void *)((oea) ? 0 : -1));

  /* what type of binary */
  TRACE(trace_tbd, ("create_optioin_device_node() - TBD - NT/OpenBoot?\n"));
  device_node_create(option_node,
		     "elf?",
		     boolean_type_device,
		     NULL,
		     (void *)((elf) ? -1 : 0));

  /* must all memory transfers be naturally aligned? */
  device_node_create(option_node,
		     "aligned?",
		     boolean_type_device,
		     NULL,
		     (void*)((WITH_ALIGNMENT == NONSTRICT_ALIGNMENT
			      || image->xvec->byteorder_big_p
			      || !oea)
			     ? 0
			     : -1));


  return option_node;
}


/* clatyon is a simple machine that does not require interrupts or any
   thing else */

STATIC_INLINE_DEVICE_TREE device_node *
create_clayton_device_tree(bfd *image)
{
  device_node *root;
  device_node *io_node;
  device_node *data_node;
  device_node *memory_node;

  /* the root */
  root = ZALLOC(device_node);

  /* memory - clayton has 2mb of RAM at location 0 */
  memory_node = device_node_create(root,
				   "memory",
				   memory_device,
				   NULL,
				   NULL);
  device_node_add_address(memory_node, 0x0, clayton_memory_size,
			  (device_is_readable
			   | device_is_writeable
			   | device_is_executable),
			  NULL);

  /* io address space */
  io_node = device_node_create(root, "io", bus_device, NULL, NULL);

  /* and IO devices */
  find_device_descriptor("console")
    ->creator(io_node, "console@0x400000,0");
  find_device_descriptor("halt")
    ->creator(io_node, "halt@0x500000,0");
  find_device_descriptor("icu")
    ->creator(io_node, "icu@0x600000,0");

  /* data to load */
  data_node = device_node_create(root, "image", data_device, NULL, NULL);
  bfd_map_over_sections(image,
			update_memory_node_for_section,
			(PTR)data_node);

  /* options */
  create_option_device_node(root, image);

  return root;
}


/* user mode executable build up a device tree that reflects this */

STATIC_INLINE_DEVICE_TREE device_node *
create_vea_device_tree(bfd *image)
{
  device_node *root;
  device_node *memory_node;
  device_node *option_node;

  /* the root */
  root = ZALLOC(device_node);

  /* memory */
  memory_node = device_node_create(root, "memory", memory_device,
				   NULL, NULL);
  bfd_map_over_sections(image,
			update_memory_node_for_section,
			(PTR)memory_node);
  /* options - only endian so far */
  option_node = create_option_device_node(root, image);

  return root;
}


/* create a device tree from the specified file */
INLINE_DEVICE_TREE device_node *
device_tree_create(const char *file_name)
{
  bfd *image;
  device_node *tree;

  bfd_init(); /* could be redundant but ... */

  /* open the file */
  image = bfd_openr(file_name, NULL);
  if (image == NULL) {
    bfd_perror("open failed:");
    error("nothing loaded\n");
    return NULL;
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
    if (bfd_get_start_address(image) == 0) {
      TRACE(trace_device_tree, ("create_device_tree() - clayton image\n"));
      tree = create_clayton_device_tree(image);
    }
    else if (bfd_get_start_address(image) > 0) {
      TRACE(trace_device_tree, ("create_device_tree() - vea image\n"));
      tree = create_vea_device_tree(image);
    }
    bfd_close(image);
  }
  else {
    error("TBD - create_device_tree() text file defining device tree\n");
    tree = NULL;
  }

  return tree;
}


/* traverse a device tree applying prefix/postfix functions to it */

INLINE_DEVICE_TREE void
device_tree_traverse(device_node *root,
		     device_tree_traverse_function *prefix,
		     device_tree_traverse_function *postfix,
		     void *data)
{
  device_node *child;
  if (prefix != NULL)
    prefix(root, data);
  for (child = root->children; child != NULL; child = child->sibling) {
    device_tree_traverse(child, prefix, postfix, data);
  }
  if (postfix != NULL)
    postfix(root, data);
}


/* query the device tree */

INLINE_DEVICE_TREE device_node *
device_tree_find_node(device_node *root,
		      const char *path)
{
  char *chp;
  int name_len;
  device_node *child;

  /* strip off any leading `/', `../' or `./' */
  while (1) {
    if (strncmp(path, "/", strlen("/")) == 0) {
      while (root->parent != NULL)
	root = root->parent;
      path += strlen("/");
    }
    else if (strncmp(path, "./", strlen("./")) == 0) {
      root = root;
      path += strlen("./");
    }
    else if (strncmp(path, "../", strlen("../")) == 0) {
      if (root->parent != NULL)
	root = root->parent;
      path += strlen("../");
    }
    else {
      break;
    }
  }

  /* find the qualified (with @) and unqualified names in the path */
  chp = strchr(path, '/');
  name_len = (chp == NULL
	      ? strlen(path)
	      : chp - path);

  /* search through children for a match */
  for (child = root->children;
       child != NULL;
       child = child->sibling) {
    if (strncmp(path, child->name, name_len) == 0
	&& (strlen(child->name) == name_len
	    || strchr(child->name, '@') == child->name + name_len)) {
      if (path[name_len] == '\0')
	return child;
      else
	return device_tree_find_node(child, path + name_len + 1);
    }
  }
  return NULL;
}

INLINE_DEVICE_TREE device_node *device_tree_find_next_node
(device_node *root,
 const char *path,
 device_node *last);

INLINE_DEVICE_TREE signed_word
device_tree_find_int(device_node *root,
		     const char *path)
{
  device_node *int_node = device_tree_find_node(root, path);
  if (int_node == NULL) {
    error("device_tree_find_int() - node %s does not exist\n", path);
    return 0;
  }
  else if (int_node->type != integer_type_device) {
    error("device_tree_find_int() - node %s is not an int\n", path);
    return 0;
  }
  else {
    return (signed_word)(int_node->data);
  }
}


INLINE_DEVICE_TREE const char *device_tree_find_string
(device_node *root,
 const char *path);

INLINE_DEVICE_TREE int
device_tree_find_boolean(device_node *root,
			 const char *path)
{
  device_node *int_node = device_tree_find_node(root, path);
  if (int_node == NULL) {
    error("device_tree_find_boolean() - node %s does not exist\n", path);
    return 0;
  }
  else if (int_node->type != boolean_type_device) {
    error("device_tree_find_boolean() - node %s is not a boolean\n", path);
    return 0;
  }
  else {
    return (signed_word)(int_node->data);
  }
}


INLINE_DEVICE_TREE void *device_tree_find_bytes
(device_node *root,
 const char *path);

/* dump out a device node and addresses */

INLINE_DEVICE_TREE void
device_tree_dump(device_node *device,
		 void *ignore_data_argument)
{
  printf_filtered("(device_node@0x%x\n", device);
  printf_filtered(" (parent 0x%x)\n", device->parent);
  printf_filtered(" (children 0x%x)\n", device->children);
  printf_filtered(" (sibling 0x%x)\n", device->sibling);
  printf_filtered(" (name %s)\n", device->name ? device->name : "(null)");
  printf_filtered(" (type %d)\n", device->type);
  printf_filtered(" (handlers 0x%x)\n", device->callbacks);
  printf_filtered(" (addresses %d)\n", device->addresses);
  printf_filtered(" (data %d)\n", device->data);
  printf_filtered(")\n");
}

#endif /* _DEVICE_TREE_C_ */
