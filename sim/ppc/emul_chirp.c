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

typedef unsigned_word (chirp_handler)
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

static unsigned_word
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
  if (T2H_4(args.n_args) != 1 || T2H_4(args.n_returns) != 1)
    return -1;
  emul_read_string(device_specifier,
		   T2H_4(args.device_specifier),
		   sizeof(device_specifier),
		   processor, cia);
  dev = device_tree_find_device(data->root,
				device_specifier);
  if (dev == (device*)0)
    args.phandle = -1;
  else
    args.phandle = cap_external(data->phandles, dev);
  emul_write_buffer(&args, data->arguments,
		    sizeof(args),
		    processor, cia);
  return 0;
}

static unsigned_word
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
  if (T2H_4(args.n_args) != 4 || T2H_4(args.n_returns) != 1)
    return -1;
  /* read in the arguments */
  dev = cap_internal(data->phandles, args.phandle);
  if (dev == (device*)0)
    return -1;
  emul_read_string(name,
		   T2H_4(args.name),
		   sizeof(name),
		   processor, cia);
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
  emul_write_buffer(&args, data->arguments,
		    sizeof(args),
		    processor, cia);
  return 0;
}

static unsigned_word
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
  if (T2H_4(args.n_args) != 3 || T2H_4(args.n_returns) != 1)
    return -1;
  /* read in the arguments */
  actual = T2H_4(args.len);
  if (actual > sizeof(buf))
    actual = sizeof(buf);
  emul_read_buffer(buf,
		   T2H_4(args.addr),
		   actual,
		   processor, cia);
  /* write it out */
  write(BE2H_4(args.ihandle), buf, actual);
  args.actual = H2T_4(actual);
  emul_write_buffer(&args, data->arguments,
		    sizeof(args),
		    processor, cia);
  return 0;
}

static unsigned_word
chirp_emul_exit(os_emul_data *data,
	     cpu *processor,
	     unsigned_word cia)
{
  error("chirp_emul_exit not implemnented\n");
  return 0;
}


chirp_services services[] = {
  { "finddevice", chirp_emul_finddevice },
  { "getprop", chirp_emul_getprop },
  { "write", chirp_emul_write },
  { "exit", chirp_emul_exit },
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
    const unsigned_word htab_va = code_va + sizeof_code;
    
    /* options */
    {
      device *options = device_tree_add_found(root, "/", "options");
      device_add_integer_property(options,
				  "smp",
				  MAX_NR_PROCESSORS);
      device_add_boolean_property(options,
				  "little-endian?",
				  !image->xvec->byteorder_big_p);
      device_add_string_property(options, 
				 "env",
				 "operating");
      device_add_boolean_property(options,
				  "strict-alignment?",
				  (WITH_ALIGNMENT == STRICT_ALIGNMENT
				   || !image->xvec->byteorder_big_p));
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
				    bfd_get_start_address(image));
	device_add_integer_property(init_register,
				    "sp",
				    stack_va + sizeof_stack - 16);
	
	/* init the code callback */
	device_add_integer_property(init_register,
				    "r5",
				    code_va);
	device_tree_add_found_uw_u_u(init, "", "data", code_ra, 4, 0x1);
	device_tree_add_found_uw_u_u(init, "", "data", code_ra+16, 4, 0x1);
	device_add_integer_property(init_register,
				    "msr",
				    (msr_machine_check_enable
				     | (real_mode
					? 0 
					: (msr_instruction_relocate
					   | msr_data_relocate))
				     | (image->xvec->byteorder_big_p
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
      device_add_integer_property(chosen,
				  "stdout",
				  1);
    }
    
    /* FIXME - should come from the device tree */
    data = ZALLOC(os_emul_data);
    data->serving_instruction_ea = CHIRP_START_ADDRESS + sizeof_stack;;
    data->catching_instruction_ea = CHIRP_START_ADDRESS + sizeof_stack + 16;
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
    /* verify then capture the current request */
    if (cia != emul_data->serving_instruction_ea)
      return 0;
    emul_data->return_address = LR;
    emul_data->arguments = cpu_registers(processor)->gpr[3];
    /* try to determine what to do */
    service_name_addr = emul_read_word(cpu_registers(processor)->gpr[3],
				       processor, cia);
    service_name = emul_read_string(service_buf, service_name_addr,
				    sizeof(service_buf), processor, cia);
    /* look it up */
    service = services;
    while (service->name != NULL && strcmp(service->name, service_name) != 0)
      service++;
    if (service->name == NULL) {
      cpu_registers(processor)->gpr[3] = 0;
      cpu_restart(processor, emul_data->return_address);
    }
    emul_data->service = service;
    TRACE(trace_os_emul, ("%s called from 0x%lx\n",
			  service->name, emul_data->return_address));
    /* call upon it */
    result = service->handler(emul_data, processor, cia);
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
