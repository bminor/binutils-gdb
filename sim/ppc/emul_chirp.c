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


#ifndef _EMUL_CHIRP_C_
#define _EMUL_CHIRP_C_

/* Note: this module is called via a table.  There is no benefit in
   making it inline */

#include "emul_generic.h"
#include "emul_chirp.h"

#include "cap.h"

#ifdef HAVE_STRING_H
#include <string.h>
#else
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#endif

#include <unistd.h>

#ifndef STATIC_INLINE_EMUL_CHIRP
#define STATIC_INLINE_EMUL_CHIRP STATIC_INLINE
#endif


/* Descriptor of the open boot services being emulated */

typedef int (chirp_handler)
     (os_emul_data *data,
      cpu *processor,
      unsigned_word cia);
typedef struct _chirp_services {
  const char *name;
  chirp_handler *handler;
} chirp_services;


/* The OpenBoot emulation is, at any time either waiting for a client
   request or waiting on a client callback */
typedef enum {
  serving,
  faulting,
  catching,
} chirp_emul_state;

struct _os_emul_data {
  chirp_emul_state state;
  unsigned_word return_address;
  unsigned_word arguments;
  chirp_services *service;
  unsigned_word serving_instruction_ea;
  unsigned_word catching_instruction_ea;
  cap *phandles;
  device *root;
};


/* OpenBoot emulation functions */

static int
chirp_emul_child(os_emul_data *data,
		 cpu *processor,
		 unsigned_word cia)
{
  struct child_args {
    unsigned32 service;
    unsigned32 n_args;
    unsigned32 n_returns;
    /*in*/
    unsigned32 phandle;
    /*out*/
    unsigned32 child_phandle;
  } args;
  device *dev;
  device *child_dev;
  emul_read_buffer(&args, data->arguments,
		   sizeof(args),
		   processor, cia);
  if (T2H_4(args.n_args) != 1 || T2H_4(args.n_returns) != 1) {
    TRACE(trace_os_emul, ("child - invalid nr - n_args=%ld, n_returns=%ld\n",
			  (long)T2H_4(args.n_args),
			  (long)T2H_4(args.n_returns)));
    return -1;
  }
  /* read in the arguments */
  dev = cap_internal(data->phandles, args.phandle);
  TRACE(trace_os_emul, ("child - in - phandle=0x%lx(0x%lx`%s')\n",
			(unsigned long)T2H_4(args.phandle),
			(unsigned long)dev,
			(dev == NULL ? "" : device_name(dev))));
  if (dev == (device*)0)
    return -1;
  child_dev = device_child(dev);
  if (child_dev == NULL)
    args.child_phandle = 0;
  else
    args.child_phandle = cap_external(data->phandles, child_dev);
  TRACE(trace_os_emul, ("child - out - child_phandle=0x%lx(0x%lx`%s')\n",
			(unsigned long)T2H_4(args.child_phandle),
			(unsigned long)child_dev,
			(child_dev == NULL ? "" : device_name(child_dev))));
  emul_write_buffer(&args, data->arguments,
		    sizeof(args),
		    processor, cia);
  return 0;
}

static int
chirp_emul_exit(os_emul_data *data,
	     cpu *processor,
	     unsigned_word cia)
{
  cpu_halt(processor, cia, was_exited, 0); /* always succeeds */
  return 0;
}

static int
chirp_emul_finddevice(os_emul_data *data,
		      cpu *processor,
		      unsigned_word cia)
{
  struct finddevice_args {
    unsigned32 service;
    unsigned32 n_args;
    unsigned32 n_returns;
    /*in*/
    unsigned32 device_specifier;
    /*out*/
    unsigned32 phandle;
  } args;
  char device_specifier[1024];
  device *dev;
  emul_read_buffer(&args, data->arguments,
		   sizeof(args),
		   processor, cia);
  if (T2H_4(args.n_args) != 1 || T2H_4(args.n_returns) != 1) {
    TRACE(trace_os_emul, ("finddevice - invalid nr - n_args=%ld, n_returns=%ld\n",
			  (long)T2H_4(args.n_args),
			  (long)T2H_4(args.n_returns)));
    return -1;
  }
  emul_read_string(device_specifier,
		   T2H_4(args.device_specifier),
		   sizeof(device_specifier),
		   processor, cia);
  TRACE(trace_os_emul, ("finddevice - in - device_specifier=`%s'\n",
			device_specifier));
  dev = device_tree_find_device(data->root,
				device_specifier);
  if (dev == (device*)0)
    args.phandle = -1;
  else
    args.phandle = cap_external(data->phandles, dev);
  TRACE(trace_os_emul, ("finddevice - out - phandle=0x%lx(0x%lx`%s')\n",
			(unsigned long)T2H_4(args.phandle),
			(unsigned long)dev,
			(dev == NULL ? "" : device_name(dev))));
  emul_write_buffer(&args, data->arguments,
		    sizeof(args),
		    processor, cia);
  return 0;
}

static int
chirp_emul_getprop(os_emul_data *data,
		   cpu *processor,
		   unsigned_word cia)
{
  struct getprop_args {
    unsigned32 service;
    unsigned32 n_args;
    unsigned32 n_returns;
    /*in*/
    unsigned32 phandle;
    unsigned32 name;
    unsigned32 buf;
    unsigned32 buflen;
    /*out*/
    unsigned32 size;
  } args;
  char name[32];
  device *dev;
  const device_property *prop;
  emul_read_buffer(&args, data->arguments,
		   sizeof(args),
		   processor, cia);
  if (T2H_4(args.n_args) != 4 || T2H_4(args.n_returns) != 1) {
    TRACE(trace_os_emul, ("getprop - invalid nr - n_args=%ld, n_returns=%ld\n",
			  (long)T2H_4(args.n_args),
			  (long)T2H_4(args.n_returns)));
    return -1;
  }
  /* read in the arguments */
  dev = cap_internal(data->phandles, args.phandle);
  emul_read_string(name,
		   T2H_4(args.name),
		   sizeof(name),
		   processor, cia);
  TRACE(trace_os_emul, ("getprop - in - phandle=0x%lx(0x%lx`%s') name=`%s' buf=0x%lx buflen=%ld\n",
			(unsigned long)T2H_4(args.phandle),
			(unsigned long)dev,
			(dev == NULL ? "" : device_name(dev)),
			name,
			(unsigned long)T2H_4(args.buf),
			(unsigned long)T2H_4(args.buflen)));
  if (dev == (device*)0)
    return -1;
  prop = device_find_property(dev, name);
  if (prop == (device_property*)0) {
    args.size = -1;
  }
  else {
    int size = T2H_4(args.buflen);
    if (size > prop->sizeof_array)
      size = prop->sizeof_array;
    emul_write_buffer(prop->array, T2H_4(args.buf),
		      size,
		      processor, cia);
    args.size = H2T_4(size);
  }
  switch (prop->type) {
  case string_property:
    TRACE(trace_os_emul, ("getprop - value=`%s' (string)\n",
			  (char*)prop->array));
    break;
  default:
    break;
  }
  TRACE(trace_os_emul, ("getprop - out - size=%ld\n",
			(unsigned long)T2H_4(args.size)));
  emul_write_buffer(&args, data->arguments,
		    sizeof(args),
		    processor, cia);
  return 0;
}

static int
chirp_emul_getproplen(os_emul_data *data,
		      cpu *processor,
		      unsigned_word cia)
{
  struct getproplen_args {
    unsigned32 service;
    unsigned32 n_args;
    unsigned32 n_returns;
    /*in*/
    unsigned32 phandle;
    unsigned32 name;
    /*out*/
    unsigned32 proplen;
  } args;
  char name[32];
  device *dev;
  const device_property *prop;
  emul_read_buffer(&args, data->arguments,
		   sizeof(args),
		   processor, cia);
  if (T2H_4(args.n_args) != 2 || T2H_4(args.n_returns) != 1) {
    TRACE(trace_os_emul, ("getproplen - invalid nr - n_args=%ld, n_returns=%ld\n",
			  (long)T2H_4(args.n_args),
			  (long)T2H_4(args.n_returns)));
    return -1;
  }
  /* read in the arguments */
  dev = cap_internal(data->phandles, args.phandle);
  if (dev == (device*)0)
    return -1;
  emul_read_string(name,
		   T2H_4(args.name),
		   sizeof(name),
		   processor, cia);
  TRACE(trace_os_emul, ("getproplen - in - phandle=0x%lx(0x%lx`%s') name=`%s'\n",
			(unsigned long)T2H_4(args.phandle),
			(unsigned long)dev,
			(dev == NULL ? "" : device_name(dev)),
			name));
  prop = device_find_property(dev, name);
  if (prop == (device_property*)0) {
    args.proplen = -1;
  }
  else {
    args.proplen = H2T_4(prop->sizeof_array);
  }
  TRACE(trace_os_emul, ("getproplen - out - proplen=%ld\n",
			(unsigned long)T2H_4(args.proplen)));
  emul_write_buffer(&args, data->arguments,
		    sizeof(args),
		    processor, cia);
  return 0;
}

static int
chirp_emul_open(os_emul_data *data,
		cpu *processor,
		unsigned_word cia)
{
  struct open_args {
    unsigned32 service;
    unsigned32 n_args;
    unsigned32 n_returns;
    /*in*/
    unsigned32 device_specifier;
    /*out*/
    unsigned32 ihandle;
  } args;
  char name[1024];
  emul_read_buffer(&args, data->arguments,
		   sizeof(args),
		   processor, cia);
  if (T2H_4(args.n_args) != 1 || T2H_4(args.n_returns) != 1) {
    TRACE(trace_os_emul, ("open - invalid nr - n_args=%ld, n_returns=%ld\n",
			  (long)T2H_4(args.n_args),
			  (long)T2H_4(args.n_returns)));
    return -1;
  }
  /* read in the arguments */
  emul_read_string(name,
		   T2H_4(args.device_specifier),
		   sizeof(name),
		   processor, cia);
  TRACE(trace_os_emul, ("open - in - device_specifier=`%s'\n",
			name));
  printf_filtered("OpenBoot - open unimplemented for %s\n", name);
  args.ihandle = -1;
  TRACE(trace_os_emul, ("open - out - ihandle=0x%lx\n",
			(unsigned long)T2H_4(args.ihandle)));
  emul_write_buffer(&args, data->arguments,
		    sizeof(args),
		    processor, cia);
  return 0;
}

static int
chirp_emul_parent(os_emul_data *data,
		  cpu *processor,
		  unsigned_word cia)
{
  struct parent_args {
    unsigned32 service;
    unsigned32 n_args;
    unsigned32 n_returns;
    /*in*/
    unsigned32 phandle;
    /*out*/
    unsigned32 parent_phandle;
  } args;
  device *dev;
  device *parent_dev;
  emul_read_buffer(&args, data->arguments,
		   sizeof(args),
		   processor, cia);
  if (T2H_4(args.n_args) != 1 || T2H_4(args.n_returns) != 1) {
    TRACE(trace_os_emul, ("parent - invalid nr - n_args=%ld, n_returns=%ld\n",
			  (long)T2H_4(args.n_args),
			  (long)T2H_4(args.n_returns)));
    return -1;
  }
  /* read in the arguments */
  dev = cap_internal(data->phandles, args.phandle);
  TRACE(trace_os_emul, ("parent - in - phandle=0x%lx(0x%lx`%s')\n",
			(unsigned long)T2H_4(args.phandle),
			(unsigned long)dev,
			(dev == NULL ? "" : device_name(dev))));
  if (dev == (device*)0)
    return -1;
  parent_dev = device_parent(dev);
  if (parent_dev == NULL)
    args.parent_phandle = 0;
  else
    args.parent_phandle = cap_external(data->phandles, parent_dev);
  TRACE(trace_os_emul, ("parent - out - parent_phandle=0x%lx(0x%lx`%s')\n",
			(unsigned long)T2H_4(args.parent_phandle),
			(unsigned long)parent_dev,
			(parent_dev == NULL ? "" : device_name(parent_dev))));
  emul_write_buffer(&args, data->arguments,
		    sizeof(args),
		    processor, cia);
  return 0;
}

static int
chirp_emul_peer(os_emul_data *data,
		cpu *processor,
		unsigned_word cia)
{
  struct peer_args {
    unsigned32 service;
    unsigned32 n_args;
    unsigned32 n_returns;
    /*in*/
    unsigned32 phandle;
    /*out*/
    unsigned32 sibling_phandle;
  } args;
  device *dev;
  device *sibling_dev = NULL;
  emul_read_buffer(&args, data->arguments,
		   sizeof(args),
		   processor, cia);
  if (T2H_4(args.n_args) != 1 || T2H_4(args.n_returns) != 1) {
    TRACE(trace_os_emul, ("peer - invalid nr - n_args=%ld, n_returns=%ld\n",
			  (long)T2H_4(args.n_args),
			  (long)T2H_4(args.n_returns)));
    return -1;
  }
  /* read in the arguments */
  dev = cap_internal(data->phandles, args.phandle);
  TRACE(trace_os_emul, ("peer - in - phandle=0x%lx(0x%lx`%s')\n",
			(unsigned long)T2H_4(args.phandle),
			(unsigned long)dev,
			(dev == NULL ? "" : device_name(dev))));
  if (dev == NULL && args.phandle != 0)
    return -1;
  if (args.phandle == 0)
    sibling_dev = data->root;
  else
    sibling_dev = device_sibling(dev);
  if (sibling_dev == NULL)
    args.sibling_phandle = 0;
  else
    args.sibling_phandle = cap_external(data->phandles, sibling_dev);
  TRACE(trace_os_emul, ("peer - out - sibling_phandle=0x%lx(0x%lx`%s')\n",
			(unsigned long)T2H_4(args.sibling_phandle),
			(unsigned long)sibling_dev,
			(sibling_dev == NULL ? "" : device_name(sibling_dev))));
  emul_write_buffer(&args, data->arguments,
		    sizeof(args),
		    processor, cia);
  return 0;
}

static int
chirp_emul_read(os_emul_data *data,
		cpu *processor,
		unsigned_word cia)
{
  struct read_args {
    unsigned32 service;
    unsigned32 n_args;
    unsigned32 n_returns;
    /*in*/
    unsigned32 ihandle;
    unsigned32 addr;
    unsigned32 len;
    /*out*/
    unsigned32 actual;
  } args;
  char buf[1024];
  int actual;
  emul_read_buffer(&args, data->arguments,
		   sizeof(args),
		   processor, cia);
  if (T2H_4(args.n_args) != 3 || T2H_4(args.n_returns) != 1) {
    TRACE(trace_os_emul, ("read - invalid nr - n_args=%ld, n_returns=%ld\n",
			  (long)T2H_4(args.n_args),
			  (long)T2H_4(args.n_returns)));
    return -1;
  }
  /* read in the arguments */
  actual = T2H_4(args.len);
  if (actual >= sizeof(buf))
    actual = sizeof(buf) - 1;
  emul_read_buffer(buf,
		   T2H_4(args.addr),
		   actual,
		   processor, cia);
  buf[actual] = '\0';
  /* read it in */
  TRACE(trace_os_emul, ("read - in - ihandle=0x%lx `%s' (%ld)\n",
			(unsigned long)args.ihandle, buf, (long)actual));
  read(BE2H_4(args.ihandle), buf, actual);
  args.actual = H2T_4(actual);
  TRACE(trace_os_emul, ("read - out - actual=%ld\n",
			(long)T2H_4(args.actual)));
  emul_write_buffer(&args, data->arguments,
		    sizeof(args),
		    processor, cia);
  return 0;
}

static int
chirp_emul_write(os_emul_data *data,
		cpu *processor,
		unsigned_word cia)
{
  struct write_args {
    unsigned32 service;
    unsigned32 n_args;
    unsigned32 n_returns;
    /*in*/
    unsigned32 ihandle;
    unsigned32 addr;
    unsigned32 len;
    /*out*/
    unsigned32 actual;
  } args;
  char buf[1024];
  int actual;
  emul_read_buffer(&args, data->arguments,
		   sizeof(args),
		   processor, cia);
  if (T2H_4(args.n_args) != 3 || T2H_4(args.n_returns) != 1) {
    TRACE(trace_os_emul, ("write - invalid nr - n_args=%ld, n_returns=%ld\n",
			  (long)T2H_4(args.n_args),
			  (long)T2H_4(args.n_returns)));
    return -1;
  }
  /* read in the arguments */
  actual = T2H_4(args.len);
  if (actual >= sizeof(buf))
    actual = sizeof(buf) - 1;
  emul_read_buffer(buf,
		   T2H_4(args.addr),
		   actual,
		   processor, cia);
  buf[actual] = '\0';
  /* write it out */
  TRACE(trace_os_emul, ("write - in - ihandle=0x%lx `%s' (%ld)\n",
			(unsigned long)args.ihandle, buf, (long)actual));
  write(BE2H_4(args.ihandle), buf, actual);
  args.actual = H2T_4(actual);
  TRACE(trace_os_emul, ("write - out - actual=%ld\n",
			(long)T2H_4(args.actual)));
  emul_write_buffer(&args, data->arguments,
		    sizeof(args),
		    processor, cia);
  return 0;
}


chirp_services services[] = {
  { "child", chirp_emul_child },
  { "exit", chirp_emul_exit },
  { "finddevice", chirp_emul_finddevice },
  { "getprop", chirp_emul_getprop },
  { "getproplen", chirp_emul_getproplen },
  { "open", chirp_emul_open },
  { "parent", chirp_emul_parent },
  { "peer", chirp_emul_peer },
  { "read", chirp_emul_read },
  { "write", chirp_emul_write },
  { 0, /* sentinal */ },
};


/* main handlers */

/* Any starting address greater than this is assumed to be an Chirp
   rather than VEA */

#ifndef CHIRP_START_ADDRESS
#define CHIRP_START_ADDRESS 0x80000000
#endif

typedef struct _chirp_note_desc {
  signed32 real_mode;
  signed32 real_base;
  signed32 real_size;
  signed32 virt_base;
  signed32 virt_size;
} chirp_note_desc;

typedef struct _chirp_note {
  chirp_note_desc desc;
  int found;
} chirp_note;

typedef struct _chirp_note_head {
  unsigned32 namesz;
  unsigned32 descsz;
  unsigned32 type;
} chirp_note_head;

static void
map_over_chirp_note(bfd *image,
		    asection *sect,
		    PTR obj)
{
  chirp_note *note = (chirp_note*)obj;
  if (strcmp(sect->name, ".note") == 0) {
    chirp_note_head head;
    char name[16];
    /* check the head */
    if (!bfd_get_section_contents(image, sect,
				  &head, 0, sizeof(head)))
      return;
    head.namesz = bfd_get_32(image, (void*)&head.namesz);
    head.descsz = bfd_get_32(image, (void*)&head.descsz);
    head.type = bfd_get_32(image, (void*)&head.type);
    if (head.type != 0x1275)
      return;
    note->found = 1;
    /* check the name field */
    if (head.namesz > sizeof(name))
      return;
    if (!bfd_get_section_contents(image, sect,
				  name, sizeof(head), head.namesz))
      return;
    if (strcmp(name, "PowerPC") != 0)
      return;
    /* get the contents */
    if (!bfd_get_section_contents(image, sect,
				  &note->desc, sizeof(head) + head.namesz,
				  head.descsz))
      return;
    note->desc.real_mode = bfd_get_32(image, (void*)&note->desc.real_mode);
    note->desc.real_base = bfd_get_32(image, (void*)&note->desc.real_base);
    note->desc.real_size = bfd_get_32(image, (void*)&note->desc.real_size);
    note->desc.virt_base = bfd_get_32(image, (void*)&note->desc.virt_base);
    note->desc.virt_size = bfd_get_32(image, (void*)&note->desc.virt_size);
    note->found = 2;
  }
}


static os_emul_data *
emul_chirp_create(device *root,
		  bfd *image,
		  const char *name)
{
  os_emul_data *data;
  chirp_note note;
  int big_endian;

  /* Sanity check that this really is the chosen emulation */
  if (name == NULL && image == NULL)
    return NULL;
  if (name != NULL
      && strcmp(name, "ob") != 0
      && strcmp(name, "ieee1274") != 0
      && strcmp(name, "chrp") != 0
      && strcmp(name, "chirp") != 0
      && strcmp(name, "openboot") != 0)
    return NULL;

  /* look for an elf note section */
  memset(&note, 0, sizeof(note));
  if (image != NULL)
    bfd_map_over_sections(image, map_over_chirp_note, &note);
  if (name == NULL && image != NULL && !note.found)
    return NULL;

  /* the root node */
  device_add_string_property(root,
			     "name",
			     "gpl,clayton");

  {
    const unsigned_word memory_size = 0x200000;
    
    /* the hash table */
    const unsigned nr_page_table_entry_groups = (memory_size < 0x800000
						 ? 1024 /* min allowed */
						 : (memory_size / 4096 / 2));
    const unsigned sizeof_htab = nr_page_table_entry_groups * 64;
    const unsigned_word htab_ra = memory_size - sizeof_htab;
    
    /* a page for firmware calls */
    const unsigned_word sizeof_code = 4096;
    const unsigned_word code_ra = htab_ra - sizeof_code;

    /* the stack */
    const unsigned sizeof_stack = 32 * 1024;
    const unsigned_word stack_ra = code_ra - sizeof_stack;
    
    /* the firmware's home */
    const int real_mode = 0;
    /*  const unsigned_word real_base = stack_ra; */
    /*  const unsigned real_size = memory_size - real_base; */
    const unsigned_word virt_base = CHIRP_START_ADDRESS;
    /*  const unsigned virt_size = real_size;*/
    
    /* the virtual addresses */
    const unsigned_word stack_va = virt_base;
    const unsigned_word code_va = stack_va + sizeof_stack;
    const unsigned_word code_client_va = code_va;
    const unsigned_word code_callback_va = code_client_va + 16;
    const unsigned_word code_loop_va = code_callback_va + 16;
    const unsigned_word htab_va = code_va + sizeof_code;

#ifdef bfd_big_endian	/* new bfd */
    big_endian = bfd_big_endian(image);
#else
    big_endian = image->xvec->byteorder_big_p;
#endif

    /* options */
    {
      device *options = device_tree_add_found(root, "/", "options");
      device_add_integer_property(options,
				  "smp",
				  MAX_NR_PROCESSORS);
      device_add_boolean_property(options,
				  "little-endian?",
				  !big_endian);
      device_add_string_property(options, 
				 "env",
				 "operating");
      device_add_boolean_property(options,
				  "strict-alignment?",
				  (WITH_ALIGNMENT == STRICT_ALIGNMENT
				   || !big_endian));
      device_add_boolean_property(options,
				  "floating-point?",
				  WITH_FLOATING_POINT);
      device_add_string_property(options,
				 "os-emul",
				 "chirp");
    }
    
    /* hardware */
    device_tree_add_found_uw_u_u(root, "/", "memory",
				 0, memory_size, access_read_write_exec);
    
    /* initialization */
    {
      device *init = device_tree_add_found(root, "/", "init");
      {
	device *init_register = device_tree_add_found(init, "", "register");
	device_add_integer_property(init_register,
				    "pc",
				    code_loop_va);
	device_add_integer_property(init_register,
				    "0.pc",
				    bfd_get_start_address(image));
	device_add_integer_property(init_register,
				    "sp",
				    stack_va + sizeof_stack - 16);
	
	/* init the code callback along with a loop for the unused cpu's */
	device_add_integer_property(init_register,
				    "r5",
				    code_client_va);
	device_tree_add_found_uw_u_u(init, "",
				     "data",
				     code_ra + (code_client_va - code_va),
				     4, 0x1); /*emul-call*/
	device_tree_add_found_uw_u_u(init, "",
				     "data",
				     code_ra + (code_callback_va - code_va),
				     4, 0x1); /*emul-call*/
	device_tree_add_found_uw_u_u(init, "",
				     "data",
				     code_ra + (code_loop_va - code_va),
				     4, 0x48000000); /*b .*/
	device_add_integer_property(init_register,
				    "msr",
				    (msr_machine_check_enable
				     | (real_mode
					? 0 
					: (msr_instruction_relocate
					   | msr_data_relocate))
				     | (big_endian
					? 0
					: (msr_little_endian_mode
					   | msr_interrupt_little_endian_mode
					   ))));
	device_add_integer_property(init_register,
				    "sdr1",
				    (htab_ra
				     | MASK32(16, 22)
				     | ((sizeof_htab - 1) >> 16)));
	/* FIXME */
	device_add_integer_property(init_register,
				    "sr8",
				    0x00fffff8);
	device_add_integer_property(init_register,
				    "sr9",
				    0x00fffff9);
	
	{ /* hash table and vm */
	  device *htab_root = device_tree_add_found_uw_u(init, "", "htab",
							 htab_ra, sizeof_htab);
	  device_tree_add_found_uw_uw_u_u_u(htab_root, "", "pte",
					    stack_ra, stack_va, sizeof_stack,
					    0x7/*wimg*/, 0x2/*pp*/);
	  device_tree_add_found_uw_uw_u_u_u(htab_root, "", "pte",
					    code_ra, code_va, sizeof_code,
					    0x7/*wimg*/, 0x2/*pp*/);
	  device_tree_add_found_uw_uw_u_u_u(htab_root, "", "pte",
					    htab_ra, htab_va, sizeof_htab,
					    0x7/*wimg*/, 0x2/*pp*/);
	  device_tree_add_found_uw_u_u_c(htab_root, "", "pte",
					 0x4000, /*magic*/
					 0x7/*wimg*/, 0x2/*pp*/,
					 bfd_get_filename (image));
	}
      }
    }
    
    { /* chosen options */
      device *chosen = device_tree_add_found(root, "/", "chosen");
      device_add_string_property(chosen,
				 "name",
				 "chosen");
      device_add_integer_property(chosen,
				  "stdin",
				  0); /* FIXME: ihandle of stdin */
      device_add_integer_property(chosen,
				  "stdout",
				  1); /* FIXME: ihandle of stdout */
      device_add_string_property(chosen,
				 "bootpath",
				 "/disk@0:\\boot");
      device_add_string_property(chosen,
				 "bootargs",
				 "");
#if 0
      device_add_integer_property(chosen,
				  "memory",
				  0); /* FIXME: ihandle of memory */
      device_add_integer_property(chosen,
				  "mmu",
				  0);
#endif
    }
    
    /* FIXME - should come from the device tree */
    data = ZALLOC(os_emul_data);
    data->serving_instruction_ea = code_client_va;
    data->catching_instruction_ea = code_callback_va;
    data->phandles = cap_create("chirp");
    data->root = root;
    return data;
  }
}

static void
emul_chirp_init(os_emul_data *emul_data,
		int nr_cpus)
{
  emul_data->state = serving;
  cap_init(emul_data->phandles);
}

static int
emul_chirp_instruction_call(cpu *processor,
			    unsigned_word cia,
			    unsigned_word ra,
			    os_emul_data *emul_data)
{
  unsigned_word service_name_addr;
  unsigned_word result;
  char service_buf[32];
  char *service_name;
  chirp_services *service;

  switch (emul_data->state) {

  case serving:
    /* we are waiting on an OpenBoot request from the client program
       via the client interface */
    if (cia != emul_data->serving_instruction_ea)
      return 0;
    emul_data->return_address = LR;
    emul_data->arguments = cpu_registers(processor)->gpr[3];
    /* try to determine what to do */
    service_name_addr = emul_read_word(cpu_registers(processor)->gpr[3],
				       processor, cia);
    service_name = emul_read_string(service_buf, service_name_addr,
				    sizeof(service_buf), processor, cia);
    TRACE(trace_os_emul, ("%s called from 0x%lx with args 0x%lx\n",
			  service_name,
			  (unsigned long)emul_data->return_address,
			  (unsigned long)emul_data->arguments));
    /* look it up */
    service = services;
    while (service->name != NULL && strcmp(service->name, service_name) != 0)
      service++;
    if (service->name == NULL) {
      error("OpenBoot service `%s' not found\n", service_name);
      TRACE(trace_os_emul, ("%s not found\n", service_name));
      cpu_registers(processor)->gpr[3] = 0;
      cpu_restart(processor, emul_data->return_address);
    }
    emul_data->service = service;
    /* call upon it */
    result = service->handler(emul_data, processor, cia);
    break;

  case faulting:
    /* We were handling a client request but encountered a page fault
       (or other exception).  The fault needs to be passed back to the
       client using the the client suplied call back interface */
    error("emul_chirp_instruction_call() faulting unimplemented\n");
    result = -1;
    break;

  case catching:
    /* Have called the client (via the callback interface) because
       some fault (hash table miss) occured.  The client has finished
       handling this and is now returning */
    error("emul_chirp_instruction_call() catching unimplemented\n");
    result = -1;
    break;

  default:
    error("emul_chirp_instruction_call() unknown internal state\n");
    result = -1;
    break;

  }

  /* return to caller */
  cpu_registers(processor)->gpr[3] = result;
  cpu_restart(processor, emul_data->return_address);
  return 1;
}

const os_emul emul_chirp = {
  "chirp",
  emul_chirp_create,
  emul_chirp_init,
  NULL, /*system_call*/
  emul_chirp_instruction_call,
  0 /*data*/
};

#endif
