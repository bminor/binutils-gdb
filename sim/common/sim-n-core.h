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

#define sim_core_read_aligned_N XCONCAT2(sim_core_read_aligned_,N)
#define sim_core_write_aligned_N XCONCAT2(sim_core_write_aligned_,N)
#define sim_core_read_unaligned_N XCONCAT2(sim_core_read_unaligned_,N)
#define sim_core_write_unaligned_N XCONCAT2(sim_core_write_unaligned_,N)


INLINE_SIM_CORE(unsigned_N)
sim_core_read_aligned_N(sim_cpu *cpu,
			sim_cia cia,
			sim_core_maps map,
			unsigned_word addr)
{
  unsigned_N val;
  sim_core_mapping *mapping = sim_core_find_mapping (CPU_CORE (cpu), map,
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
  if (TRACE_P (cpu, TRACE_CORE_IDX))
    trace_printf (CPU_STATE (cpu), cpu,
		  "sim-n-core.c:%d: read-%d %s:0x%08lx -> 0x%lx\n",
		  __LINE__,
		  sizeof (unsigned_N),
		  sim_core_map_to_str (map),
		  (unsigned long) addr,
		  (unsigned long) val);
  return val;
}


INLINE_SIM_CORE(unsigned_N)
sim_core_read_unaligned_N(sim_cpu *cpu,
			  sim_cia cia,
			  sim_core_maps map,
			  unsigned_word addr)
{
  int alignment = sizeof (unsigned_N) - 1;
  /* if hardwired to forced alignment just do it */
  if (WITH_ALIGNMENT == FORCED_ALIGNMENT)
    return sim_core_read_aligned_N (cpu, cia, map, addr & ~alignment);
  else if ((addr & alignment) == 0)
    return sim_core_read_aligned_N (cpu, cia, map, addr);
  else
    switch (CURRENT_ALIGNMENT)
      {
      case STRICT_ALIGNMENT:
	/* FIXME - notify abort handler */
	sim_io_error (CPU_STATE (cpu), "read-%d - misaligned access to 0x%lx",
		      sizeof (unsigned_N), (unsigned long) addr);
	return -1;
      case NONSTRICT_ALIGNMENT:
	{
	  unsigned_N val;
	  if (sim_core_read_buffer (CPU_STATE (cpu), map, &val, addr,
				    sizeof(unsigned_N))
	      != sizeof(unsigned_N))
	    sim_io_error(CPU_STATE (cpu), "misaligned %d byte read to 0x%lx failed",
			 sizeof(unsigned_N), (unsigned long) addr);
	  val = T2H_N(val);
	  return val;
	}
      case FORCED_ALIGNMENT:
	return sim_core_read_aligned_N (cpu, cia, map, addr & ~alignment);
      case MIXED_ALIGNMENT:
	sim_io_error (CPU_STATE (cpu), "internal error - sim_core_read_unaligned_N - mixed alignment");
	return 0;
      default:
	sim_io_error (CPU_STATE (cpu), "internal error - sim_core_read_unaligned_N - bad switch");
	return 0;
      }
}


INLINE_SIM_CORE(void)
sim_core_write_aligned_N(sim_cpu *cpu,
			 sim_cia cia,
			 sim_core_maps map,
			 unsigned_word addr,
			 unsigned_N val)
{
  sim_core_mapping *mapping = sim_core_find_mapping(CPU_CORE (cpu), map,
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
  if (TRACE_P (cpu, TRACE_CORE_IDX))
    trace_printf (CPU_STATE (cpu), cpu,
		  "sim-n-core.c:%d: write-%d %s:0x%08lx <- 0x%lx\n",
		  __LINE__,
		  sizeof (unsigned_N),
		  sim_core_map_to_str (map),
		  (unsigned long) addr,
		  (unsigned long) val);
}


INLINE_SIM_CORE(void)
sim_core_write_unaligned_N(sim_cpu *cpu,
			   sim_cia cia,
			   sim_core_maps map,
			   unsigned_word addr,
			   unsigned_N val)
{
  int alignment = sizeof (unsigned_N) - 1;
  /* if hardwired to forced alignment just do it */
  if (WITH_ALIGNMENT == FORCED_ALIGNMENT)
    sim_core_write_aligned_N (cpu, cia, map, addr & ~alignment, val);
  else if ((addr & alignment) == 0)
    sim_core_write_aligned_N (cpu, cia, map, addr, val);
  else
    switch (CURRENT_ALIGNMENT)
      {
      case STRICT_ALIGNMENT:
	/* FIXME - notify abort handler */
	sim_io_error (CPU_STATE (cpu),
		      "write-%d - misaligned access to 0x%lx",
		      sizeof (unsigned_N), (unsigned long) addr);
	break;
      case NONSTRICT_ALIGNMENT:
	{
	  val = T2H_N(val);
	  if (sim_core_write_buffer (CPU_STATE (cpu), map, &val, addr,
				     sizeof(unsigned_N))
	      != sizeof(unsigned_N))
	    sim_io_error(CPU_STATE (cpu),
			 "misaligned %d byte read to 0x%lx failed",
			 sizeof(unsigned_N), (unsigned long) addr);
	  break;
	}
      case FORCED_ALIGNMENT:
	sim_core_write_aligned_N (cpu, cia, map, addr & ~alignment, val);
      case MIXED_ALIGNMENT:
	sim_io_error (CPU_STATE (cpu), "internal error - sim_core_write_unaligned_N - mixed alignment");
	break;
      default:
	sim_io_error (CPU_STATE (cpu), "internal error - sim_core_write_unaligned_N - bad switch");
	break;
      }
}


/* NOTE: see start of file for #define of these macros */
#undef unsigned_N
#undef T2H_N
#undef H2T_N
#undef sim_core_read_aligned_N
#undef sim_core_write_aligned_N
#undef sim_core_read_unaligned_N
#undef sim_core_write_unaligned_N
