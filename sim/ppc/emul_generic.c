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
 
    ----

    Code to output system call traces Copyright (C) 1991 Gordon Irlam.
    All rights reserved.

    This tool is part of the Spa(7) package.  The Spa(7) package
    may  be redistributed and/or modified under the terms of the
    GNU General Public License Version 2 (GPL(7))  as  published
    by the Free Software Foundation.

    */


#ifndef _EMUL_GENERIC_C_
#define _EMUL_GENERIC_C_

#include "emul_generic.h"

#ifndef STATIC_INLINE_EMUL_GENERIC
#define STATIC_INLINE_EMUL_GENERIC STATIC_INLINE
#endif


INLINE_EMUL_GENERIC void
emul_enter_call(emulation *emul,
		int call,
		int arg0,
		cpu *processor,
		unsigned_word cia)
{
  printf_filtered("%d:0x%x:%s(",
		  cpu_nr(processor) + 1,
		  cia, 
		  emul->call_descriptor[call].name);
}


INLINE_EMUL_GENERIC void
emul_exit_call(emulation *emul,
	       int call,
	       int arg0,
	       cpu *processor,
	       unsigned_word cia)
{
  int status = cpu_registers(processor)->gpr[3];
  int error = cpu_registers(processor)->gpr[0];
  printf_filtered(")=%d", status);
  if (error > 0 && error < emul->nr_error_names)
    printf_filtered("[%s]",
		    emul->error_names[cpu_registers(processor)->gpr[0]]);
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
    if (vm_data_map_read_buffer(cpu_data_map(processor),
				&dest[nr_moved],
				addr + nr_moved,
				sizeof(dest[nr_moved]))
	!= sizeof(dest[nr_moved]))
      return NULL;
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


INLINE_EMUL_GENERIC void
emul_write_word(unsigned_word addr,
		unsigned_word buf,
		cpu *processor,
		unsigned_word cia)
{
  int nr_moved;
  H2T(buf);
  nr_moved = vm_data_map_write_buffer(cpu_data_map(processor),
				      &buf,
				      addr,
				      sizeof(buf),
				      0/*violate_ro*/);
  if (nr_moved != sizeof(buf)) {
    printf_filtered("emul_write_word() write failed, %d out of %d written\n",
		    nr_moved, sizeof(buf));
    cpu_halt(processor, cia, was_exited, 14/*EFAULT*/);
  }
}


INLINE_EMUL_GENERIC void
emul_write_buffer(const void *source,
		  unsigned_word addr,
		  unsigned nr_bytes,
		  cpu *processor,
		  unsigned_word cia)
{
  int nr_moved = vm_data_map_write_buffer(cpu_data_map(processor),
					  source,
					  addr,
					  nr_bytes,
					  0/*violate_ro*/);
  if (nr_moved != nr_bytes) {
    printf_filtered("emul_write_buffer() write failed %d out of %d written\n",
		    nr_moved, nr_bytes);
    cpu_halt(processor, cia, was_exited, 14/*EFAULT*/);
  }
}


INLINE_EMUL_GENERIC void
emul_read_buffer(void *dest,
		 unsigned_word addr,
		 unsigned nr_bytes,
		 cpu *processor,
		 unsigned_word cia)
{
  int nr_moved = vm_data_map_read_buffer(cpu_data_map(processor),
					 dest,
					 addr,
					 nr_bytes);
  if (nr_moved != nr_bytes) {
    printf_filtered("emul_read_buffer() read failed %d out of %d read\n",
		    nr_moved, nr_bytes);
    cpu_halt(processor, cia, was_exited, 14/*EFAULT*/);
  }
}


INLINE_EMUL_GENERIC void
emul_do_call(emulation *emul,
	     unsigned call,
	     const int arg0,
	     cpu *processor,
	     unsigned_word cia)
{
  emul_call_handler *handler = NULL;
  if (call >= emul->nr_system_calls)
    error("do_call() os_emul call %d out-of-range\n", call);

  handler = emul->call_descriptor[call].handler;
  if (handler == NULL)
    error("do_call() unimplemented call %d\n", call);

  ENTER_CALL;
  cpu_registers(processor)->gpr[0] = 0; /* default success */
  handler(emul, call, arg0, processor, cia);
  EXIT_CALL;
}


#endif /* _SYSTEM_C_ */
