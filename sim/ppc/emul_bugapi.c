/*  This file is part of the program psim.

    Copyright (C) 1994-1996, Andrew Cagney <cagney@highland.com.au>

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


#ifndef _EMUL_BUGAPI_C_
#define _EMUL_BUGAPI_C_


/* from bug.S - Dale Rahn */
#define _INCHR		0x00
#define _INSTAT		0x01
#define _INLN		0x02
#define _READSTR	0x03
#define _READLN		0x04
#define _OUTCHR		0x20
#define _OUTSTR		0x21
#define _OUTLN		0x22
#define _DSKRD		0x10
#define _DSKWR		0x11
#define _DSKCFIG	0x12
#define _DSKFMT		0x14
#define _DSKCTRL	0x15
#define _WRITE		0x23
#define _WRITELN	0x24
#define _DELAY		0x43
#define _RTC_RD		0x53
#define _RETURN		0x63
#define _BRD_ID		0x70

/* Note: this module is called via a table.  There is no benefit in
   making it inline */

#include "emul_generic.h"
#include "emul_bugapi.h"


#ifndef OEA_START_ADDRESS
#define OEA_START_ADDRESS 0x100000
#endif



struct _os_emul_data {
  unsigned_word memory_size;
  unsigned_word top_of_stack;
  int interrupt_prefix;
  unsigned_word interrupt_vector_address;
  unsigned_word system_call_address;
  unsigned_word stall_cpu_loop_address;
  int little_endian;
  int floating_point_available;
};


static os_emul_data *
emul_bugapi_create(device *root,
		   bfd *image,
		   const char *name)
{
  int elf_binary;
  device *node;
  os_emul_data *bugapi;

  /* check it really is for us */
  if (name != NULL
      && strcmp(name, "bugapi") != 0
      && strcmp(name, "bug") != 0)
    return NULL;
  if (image != NULL
      && name == NULL
      && bfd_get_start_address(image) > OEA_START_ADDRESS)
    return NULL;

  bugapi = ZALLOC(os_emul_data);

  /* some defaults */
  elf_binary = image->xvec->flavour == bfd_target_elf_flavour;

  /* options */
  emul_add_tree_options(root, image, "bug", "oea",
			1 /*oea-interrupt-prefix*/);
  
  /* add some real hardware */
  emul_add_tree_hardware(root);

  bugapi->memory_size
    = device_find_integer_property(root, "/openprom/options/oea-memory-size");
  bugapi->interrupt_prefix =
    device_find_integer_property(root, "/openprom/options/oea-interrupt-prefix");
  bugapi->interrupt_vector_address = (bugapi->interrupt_prefix
				      ? MASK(0, 43)
				      : 0);
  bugapi->system_call_address = (bugapi->interrupt_vector_address + 0x00c00);
  bugapi->stall_cpu_loop_address = (bugapi->system_call_address + 0x000f0);
  bugapi->top_of_stack = bugapi->memory_size - 0x1000;
  bugapi->little_endian
    = device_find_boolean_property(root, "/options/little-endian?");
  bugapi->floating_point_available
    = device_find_boolean_property(root, "/openprom/options/floating-point?");

  /* initialization */
  device_tree_add_parsed(root, "/openprom/init/register/0.pc 0x%lx",
			 (unsigned long)bfd_get_start_address(image));
  device_tree_add_parsed(root, "/openprom/init/register/pc 0x%lx",
			 (unsigned long)bugapi->stall_cpu_loop_address);
  device_tree_add_parsed(root, "/openprom/init/register/sp 0x%lx",
			 (unsigned long)(bugapi->top_of_stack - 16));
  device_tree_add_parsed(root, "/openprom/init/register/msr 0x%x",
			 (msr_recoverable_interrupt
			  | (bugapi->little_endian
			     ? (msr_little_endian_mode
				| msr_interrupt_little_endian_mode)
			     : 0)
			  | (bugapi->floating_point_available
			     ? msr_floating_point_available
			     : 0)
			  | (bugapi->interrupt_prefix
			     ? msr_interrupt_prefix
			     : 0)
			  ));
  
  /* patch the system call instruction to call this emulation and then
     do an rfi */
  node = device_tree_add_parsed(root, "/openprom/init/data@0x%lx",
				(long)bugapi->system_call_address);
  device_tree_add_parsed(node, "./real-address 0x%lx",
			 (long)bugapi->system_call_address);
  device_tree_add_parsed(node, "./data 0x%x",
			 emul_call_instruction);
  node = device_tree_add_parsed(root, "/openprom/init/data@0x%lx",
				(long)(bugapi->system_call_address + 4));
  device_tree_add_parsed(node, "./real-address 0x%lx",
			 (long)(bugapi->system_call_address + 4));
  device_tree_add_parsed(node, "./data 0x%x",
			 emul_rfi_instruction);

  /* patch the end of the system call instruction so that it contains
     a loop to self instruction and point all the cpu's at this */
  node = device_tree_add_parsed(root, "/openprom/init/data@0x%lx",
				(unsigned long)bugapi->stall_cpu_loop_address);
  device_tree_add_parsed(node, "./real-address 0x%lx",
			 (unsigned long)bugapi->stall_cpu_loop_address);
  device_tree_add_parsed(node, "./data 0x%lx",
			 (unsigned long)emul_loop_instruction);
    
  device_tree_add_parsed(root, "/openprom/init/stack/stack-type %s",
			 elf_binary ? "elf" : "aix");
    
  device_tree_add_parsed(root, "/openprom/init/load-binary/file-name \"%s",
			 bfd_get_filename(image));

  return bugapi;
}

static void
emul_bugapi_init(os_emul_data *emul_data,
		 int nr_cpus)
{
  /* nothing happens here */
}

static int
emul_bugapi_instruction_call(cpu *processor,
			     unsigned_word cia,
			     unsigned_word ra,
			     os_emul_data *emul_data)
{
  const int call_id = cpu_registers(processor)->gpr[10];
  /* check that this isn't an invalid instruction */
  if (cia != emul_data->system_call_address)
    return 0;
  switch (call_id) {
  case _OUTCHR:
    printf_filtered("%c", (char)cpu_registers(processor)->gpr[3]);
    break;
  case _OUTLN:
    printf_filtered("\n");
    break;
  case _RETURN:
    cpu_halt(processor, cia, was_exited, 0); /* always succeeds */
    break;
  default:
    error("emul-bugapi: unimplemented bugapi call 0x%x from address 0x%lx\n",
	  call_id, SRR0);
    break;
  }
  return 1;
  /* the instruction following this one is a RFI.  Thus by just
     continuing the return from system call is performed */
}

const os_emul emul_bugapi = {
  "bugapi",
  emul_bugapi_create,
  emul_bugapi_init,
  0, /*system_call*/
  emul_bugapi_instruction_call,
  0 /*data*/
};

#endif
