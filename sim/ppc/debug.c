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


#ifndef _DEBUG_C_
#define _DEBUG_C_

#include "basics.h"

#ifdef HAVE_STDLIB
#include <stdlib.h>
#endif

int ppc_trace[nr_trace_options];
int print_info;

typedef struct _trace_option_descriptor {
  trace_options option;
  const char *name;
  const char *description;
} trace_option_descriptor;

static trace_option_descriptor trace_description[] = {
  { trace_gdb, "gdb", "calls made by gdb to the sim_calls.c file" },
  { trace_os_emul, "os-emul", "VEA mode sytem calls - like strace/spy" },
  /* decode/issue */
  { trace_semantics, "semantics", "Instruction execution (issue)" },
  { trace_idecode, "idecode", "instruction decode (when miss in cache)" },
  { trace_alu, "alu", "results of integer ALU" },
  { trace_load_store, "load-store", "transfers to from data registers" },
  /* devices */
  { trace_device_tree, "device-tree",  },
  { trace_devices, "devices" },
  { trace_pass_device, "pass-device" },
  { trace_console_device, "console-device" },
  { trace_icu_device, "icu-device" },
  { trace_halt_device, "halt-device" },
  { trace_register_device, "register-device" },
  { trace_vm_device, "vm-device" },
  { trace_memory_device, "memory-device" },
  { trace_htab_device, "htab-device" },
  { trace_pte_device, "pte-device" },
  { trace_binary_device, "binary-device" },
  { trace_file_device, "file-device" },
  { trace_core_device, "core-device" },
  { trace_stack_device, "stack-device" },
  /* misc */
  /* sentinal */
  { nr_trace_options, NULL },
};

extern void
trace_option(const char *option)
{
  int setting = 1;
  if (option[0] == '!') {
    setting = 0; /* clear it */
    option += 1;
  }
  if (strcmp(option, "all") == 0) {
    trace_options i;
    for (i = 0; i < nr_trace_options; i++)
      ppc_trace[i] = setting;
  }
  else {
    int i = 0;
    while (trace_description[i].option < nr_trace_options
	   && strcmp(option, trace_description[i].name) != 0)
      i++;
    if (trace_description[i].option < nr_trace_options)
      ppc_trace[trace_description[i].option] = setting;
    else {
      i = strtoul(option, 0, 0);
      if (i > 0 && i < nr_trace_options)
	ppc_trace[i] = setting;
      else
	error("Unknown trace option: %s\n", option);
    }
      
  }
}


extern void
trace_usage(void)
{
  const char *format = "\t%-18s%s\n";
  int i;
  printf_filtered("Possible <trace-option>s are:\n");
  printf_filtered(format, "!<trace-option>", "Disable the specified option");
  printf_filtered(format, "all", "enable all the trace options");
  for (i = 0; trace_description[i].option < nr_trace_options; i++)
    printf_filtered(format,
		    trace_description[i].name,
		    (trace_description[i].description
		     ? trace_description[i].description
		     : ""));
}

#endif /* _DEBUG_C_ */
