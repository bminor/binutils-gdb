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


#ifndef _EMUL_GENERIC_C_
#define _EMUL_GENERIC_C_

#include "emul_generic.h"

#ifndef STATIC_INLINE_EMUL_GENERIC
#define STATIC_INLINE_EMUL_GENERIC STATIC_INLINE
#endif


STATIC_INLINE_EMUL_GENERIC void
emul_syscall_enter(emul_syscall *emul,
		   int call,
		   int arg0,
		   cpu *processor,
		   unsigned_word cia)
{
  printf_filtered("%d:0x%lx:%s(",
		  cpu_nr(processor) + 1,
		  (long)cia, 
		  emul->syscall_descriptor[call].name);
}


STATIC_INLINE_EMUL_GENERIC void
emul_syscall_exit(emul_syscall *emul,
		  int call,
		  int arg0,
		  cpu *processor,
		  unsigned_word cia)
{
  int status = cpu_registers(processor)->gpr[3];
  int error = cpu_registers(processor)->gpr[0];
  printf_filtered(")=%d", status);
  if (error > 0 && error < emul->nr_error_names)
    printf_filtered("[%s]", emul->error_names[error]);
  printf_filtered("\n");
}


INLINE_EMUL_GENERIC unsigned64
emul_read_gpr64(cpu *processor,
		int g)
{
  unsigned32 hi;
  unsigned32 lo;
  if (CURRENT_TARGET_BYTE_ORDER == BIG_ENDIAN) {
    hi = cpu_registers(processor)->gpr[g];
    lo = cpu_registers(processor)->gpr[g+1];
  }
  else {
    lo = cpu_registers(processor)->gpr[g];
    hi = cpu_registers(processor)->gpr[g+1];
  }
  return (INSERTED64(hi, 0, 31) | INSERTED64(lo, 32, 63));
}


INLINE_EMUL_GENERIC void
emul_write_gpr64(cpu *processor,
		 int g,
		 unsigned64 val)
{
  unsigned32 hi = EXTRACTED64(val, 0, 31);
  unsigned32 lo = EXTRACTED64(val, 32, 63);
  if (CURRENT_TARGET_BYTE_ORDER == BIG_ENDIAN) {
    cpu_registers(processor)->gpr[g] = hi;
    cpu_registers(processor)->gpr[g+1] = lo;
  }
  else {
    cpu_registers(processor)->gpr[g] = lo;
    cpu_registers(processor)->gpr[g+1] = hi;
  }
}


INLINE_EMUL_GENERIC char *
emul_read_string(char *dest,
		 unsigned_word addr,
		 unsigned nr_bytes,
		 cpu *processor,
		 unsigned_word cia)
{
  unsigned nr_moved = 0;
  if (addr == 0)
    return NULL;
  while (1) {
    dest[nr_moved] = vm_data_map_read_1(cpu_data_map(processor),
					addr + nr_moved,
					processor, cia);
    if (dest[nr_moved] == '\0' || nr_moved >= nr_bytes)
      break;
    nr_moved++;
  }
  dest[nr_moved] = '\0';
  return dest;
}


INLINE_EMUL_GENERIC void
emul_write_status(cpu *processor,
		  int status,
		  int errno)
{
  cpu_registers(processor)->gpr[3] = status;
  if (status < 0)
    cpu_registers(processor)->gpr[0] = errno;
  else
    cpu_registers(processor)->gpr[0] = 0;
}


INLINE_EMUL_GENERIC unsigned_word
emul_read_word(unsigned_word addr,
	       cpu *processor,
	       unsigned_word cia)
{
  return vm_data_map_read_word(cpu_data_map(processor),
			       addr,
			       processor, cia);
}


INLINE_EMUL_GENERIC void
emul_write_word(unsigned_word addr,
		unsigned_word buf,
		cpu *processor,
		unsigned_word cia)
{
  vm_data_map_write_word(cpu_data_map(processor),
			 addr,
			 buf,
			 processor, cia);
}


INLINE_EMUL_GENERIC void
emul_write_buffer(const void *source,
		  unsigned_word addr,
		  unsigned nr_bytes,
		  cpu *processor,
		  unsigned_word cia)
{
  int nr_moved;
  for (nr_moved = 0; nr_moved < nr_bytes; nr_moved++) {
    vm_data_map_write_1(cpu_data_map(processor),
			addr + nr_moved,
			((const char*)source)[nr_moved],
			processor, cia);
  }
}


INLINE_EMUL_GENERIC void
emul_read_buffer(void *dest,
		 unsigned_word addr,
		 unsigned nr_bytes,
		 cpu *processor,
		 unsigned_word cia)
{
  int nr_moved;
  for (nr_moved = 0; nr_moved < nr_bytes; nr_moved++) {
    ((char*)dest)[nr_moved] = vm_data_map_read_1(cpu_data_map(processor),
						 addr + nr_moved,
						 processor, cia);
  }
}


INLINE_EMUL_GENERIC void
emul_do_system_call(os_emul_data *emul_data,
		    emul_syscall *emul,
		    unsigned call,
		    const int arg0,
		    cpu *processor,
		    unsigned_word cia)
{
  emul_syscall_handler *handler = NULL;
  if (call >= emul->nr_system_calls)
    error("do_call() os_emul call %d out-of-range\n", call);

  handler = emul->syscall_descriptor[call].handler;
  if (handler == NULL)
    error("do_call() unimplemented call %d\n", call);

  if (WITH_TRACE && ppc_trace[trace_os_emul])
    emul_syscall_enter(emul, call, arg0, processor, cia);

  cpu_registers(processor)->gpr[0] = 0; /* default success */
  handler(emul_data, call, arg0, processor, cia);

  if (WITH_TRACE && ppc_trace[trace_os_emul])
    emul_syscall_exit(emul, call, arg0, processor, cia);
}

#endif /* _SYSTEM_C_ */
