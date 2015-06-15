/* Simulator system call support.

   Copyright 2002-2015 Free Software Foundation, Inc.

   This file is part of simulators.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include "config.h"

#include "sim-main.h"
#include "sim-syscall.h"

/* Read/write functions for system call interface.  */

int
sim_syscall_read_mem (host_callback *cb ATTRIBUTE_UNUSED, struct cb_syscall *sc,
		      unsigned long taddr, char *buf, int bytes)
{
  SIM_DESC sd = (SIM_DESC) sc->p1;
  SIM_CPU *cpu = (SIM_CPU *) sc->p2;

  TRACE_MEMORY (cpu, "READ (syscall) %i bytes @ 0x%08lx", bytes, taddr);

  return sim_core_read_buffer (sd, cpu, read_map, buf, taddr, bytes);
}

int
sim_syscall_write_mem (host_callback *cb ATTRIBUTE_UNUSED, struct cb_syscall *sc,
		       unsigned long taddr, const char *buf, int bytes)
{
  SIM_DESC sd = (SIM_DESC) sc->p1;
  SIM_CPU *cpu = (SIM_CPU *) sc->p2;

  TRACE_MEMORY (cpu, "WRITE (syscall) %i bytes @ 0x%08lx", bytes, taddr);

  return sim_core_write_buffer (sd, cpu, write_map, buf, taddr, bytes);
}
