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

/* Any starting address less than this is assumed to be an OEA program
   rather than VEA.  */
#ifndef OEA_START_ADDRESS
#define OEA_START_ADDRESS 0x4000
#endif

#ifndef OEA_MEMORY_SIZE
#define OEA_MEMORY_SIZE 0x100000
#endif

/* All but CPU 0 are put into an infinate loop, this loop instruction
   is stored at the address below */
#ifndef OEA_STALL_CPU_LOOP_ADDRESS
#define OEA_STALL_CPU_LOOP_ADDRESS 0x00c10
#endif

/* At initiallization, the system call exception branches to the BUG
   emulation code */

#ifndef OEA_SYSTEM_CALL_ADDRESS
#define OEA_SYSTEM_CALL_ADDRESS 0x00c00
#endif


static os_emul_data *
emul_bugapi_create(device *root,
		   bfd *image,
		   const char *name)
{

  /* check it really is for us */
  if (name != NULL
      && strcmp(name, "bugapi") != 0
      && strcmp(name, "bug") != 0)
    return NULL;
  if (image != NULL
      && name == NULL
      && bfd_get_start_address(image) > OEA_START_ADDRESS)
    return NULL;

  {
    
    const memory_size = OEA_MEMORY_SIZE;
    const elf_binary = (image != NULL 
			&& image->xvec->flavour == bfd_target_elf_flavour);
#ifdef bfd_little_endian	/* new bfd */
    const little_endian = (image != NULL && bfd_little_endian(image));
#else
    const little_endian = (image != NULL &&
			   !image->xvec->byteorder_big_p);
#endif
    
    { /* options */
      device *options = device_tree_add_found(root, "/", "options");
      device_add_integer_property(options,
				  "smp",
				  MAX_NR_PROCESSORS);
      device_add_boolean_property(options,
				  "little-endian?",
				  little_endian);
      device_add_string_property(options,
				 "env",
				 "operating");
      device_add_boolean_property(options,
				  "strict-alignment?",
				  (WITH_ALIGNMENT == STRICT_ALIGNMENT
				   || little_endian));
      device_add_boolean_property(options,
				  "floating-point?",
				  WITH_FLOATING_POINT);
      device_add_string_property(options,
				 "os-emul",
				 "bugapi");
    }
    
    /* hardware */
    device_tree_add_found_uw_u_u(root, "/", "memory",
				 0, memory_size, access_read_write_exec);
    device_tree_add_found(root, "/", "iobus@0x400000");
    device_tree_add_found(root, "/iobus", "console@0x000000,16");
    device_tree_add_found(root, "/iobus", "halt@0x100000,4");
    device_tree_add_found(root, "/iobus", "icu@0x200000,4");
    
    { /* initialization */
      device *init = device_tree_add_found(root, "/", "init");
      {
	device *init_register = device_tree_add_found(init, "", "register");
	device_add_integer_property(init_register,
				    "pc",
				    OEA_STALL_CPU_LOOP_ADDRESS);
	device_add_integer_property(init_register,
				    "0.pc",
				    bfd_get_start_address(image));
	device_add_integer_property(init_register,
				    "sp",
				    memory_size-16);
	device_add_integer_property(init_register,
				    "msr",
				    (msr_recoverable_interrupt
				     | (little_endian
					? msr_little_endian_mode
					: 0)
				     ));
	device_tree_add_found_uw_u_u(init, "",
				     "data",
				     OEA_SYSTEM_CALL_ADDRESS,
				     4, 0x1); /*emul-call*/
	device_tree_add_found_uw_u_u(init, "",
				     "data",
				     OEA_SYSTEM_CALL_ADDRESS + 4,
				     4, 0x4c000064); /*rfi*/
	device_tree_add_found_uw_u_u(init, "",
				     "data",
				     OEA_STALL_CPU_LOOP_ADDRESS,
				     4, 0x48000000); /*b .*/
      }
      {
	device *init_stack = device_tree_add_found(init, "", "stack");
	device_add_null_property(init_stack,
				 (elf_binary
				  ? "elf"
				  : "aix"));
      }
      {
	device *init_load_binary = device_tree_add_found(init, "",
							"load-binary");
	device_add_null_property(init_load_binary,
				 bfd_get_filename(image));
      }
    }
  }
  
  return (os_emul_data*)-1;
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
  if (cia != OEA_SYSTEM_CALL_ADDRESS)
    return 0;
  switch (call_id) {
  case _OUTCHR:
    printf_filtered("%c", cpu_registers(processor)->gpr[3]);
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
