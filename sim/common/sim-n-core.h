/*  This file is part of the program psim.

    Copyright (C) 1994-1997, Andrew Cagney <cagney@highland.com.au>

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


#ifndef N
#error "N must be #defined"
#endif

#include "sim-xcat.h"

/* NOTE: see end of file for #undef of these macros */
#define unsigned_N XCONCAT2(unsigned_,N)
#define T2H_N XCONCAT2(T2H_,N)
#define H2T_N XCONCAT2(H2T_,N)

#define sim_core_read_N XCONCAT2(sim_core_read_,N)
#define sim_core_write_N XCONCAT2(sim_core_write_,N)


INLINE_SIM_CORE(unsigned_N)
sim_core_read_N(SIM_DESC sd,
		sim_core_maps map,
		unsigned_word addr,
		sim_cpu *cpu,
		sim_cia cia)
{
  unsigned_N val;
  sim_core_mapping *mapping = sim_core_find_mapping (sd, map,
						     addr,
						     sizeof (unsigned_N),
						     1,
						     cpu, cia); /*abort*/
#if (WITH_DEVICES)
  if (WITH_CALLBACK_MEMORY && mapping->device != NULL) {
    unsigned_N data;
    if (device_io_read_buffer (mapping->device,
			       &data,
			       mapping->space,
			       addr,
			       sizeof (unsigned_N)) != sizeof (unsigned_N))
      device_error (mapping->device, "internal error - sim_core_read_N() - io_read_buffer should not fail");
    val = T2H_N (data);
  }
  else
#endif
    val = T2H_N (*(unsigned_N*) sim_core_translate (mapping, addr));
  if (STATE_CORE (sd)->trace)
    trace_printf (sd, cpu, "sim-n-core.c:%d: read-%d %s:0x%08lx -> 0x%lx\n",
		  __LINE__,
		  sizeof (unsigned_N),
		  sim_core_map_to_str (map),
		  (unsigned long) addr,
		  (unsigned long) val);
  return val;
}



INLINE_SIM_CORE(void)
sim_core_write_N(SIM_DESC sd,
		 sim_core_maps map,
		 unsigned_word addr,
		 unsigned_N val,
		 sim_cpu *cpu,
		 sim_cia cia)
{
  sim_core_mapping *mapping = sim_core_find_mapping(sd, map,
						    addr,
						    sizeof (unsigned_N),
						    1,
						    cpu, cia); /*abort*/
#if (WITH_DEVICES)
  if (WITH_CALLBACK_MEMORY && mapping->device != NULL) {
    unsigned_N data = H2T_N (val);
    if (device_io_write_buffer (mapping->device,
				&data,
				mapping->space,
				addr,
				sizeof (unsigned_N), /* nr_bytes */
				cpu,
				cia) != sizeof (unsigned_N))
      device_error (mapping->device, "internal error - sim_core_write_N() - io_write_buffer should not fail");
  }
  else
#endif
    *(unsigned_N*) sim_core_translate (mapping, addr) = H2T_N (val);
  if (STATE_CORE (sd)->trace)
    trace_printf (sd, cpu, "sim-n-core.c:%d: write-%d %s:0x%08lx <- 0x%lx\n",
		  __LINE__,
		  sizeof (unsigned_N),
		  sim_core_map_to_str (map),
		  (unsigned long) addr,
		  (unsigned long) val);
}


/* NOTE: see start of file for #define of these macros */
#undef unsigned_N
#undef T2H_N
#undef H2T_N
#undef sim_core_read_N
#undef sim_core_write_N
