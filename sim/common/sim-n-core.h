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
#ifndef M
#define M N
#endif

#include "sim-xcat.h"

/* NOTE: see end of file for #undef of these macros */

#define unsigned_M XCONCAT2(unsigned_,M)

#define T2H_M XCONCAT2(T2H_,M)
#define H2T_M XCONCAT2(H2T_,M)
#define SWAP_M XCONCAT2(SWAP_,M)

#define sim_core_read_aligned_N XCONCAT2(sim_core_read_aligned_,N)
#define sim_core_read_unaligned_N XCONCAT2(sim_core_read_unaligned_,N)
#define sim_core_read_misaligned_N XCONCAT2(sim_core_read_misaligned_,N)
#define sim_core_write_aligned_N XCONCAT2(sim_core_write_aligned_,N)
#define sim_core_write_unaligned_N XCONCAT2(sim_core_write_unaligned_,N)
#define sim_core_write_misaligned_N XCONCAT2(sim_core_write_misaligned_,N)
#define sim_core_trace_M XCONCAT2(sim_core_trace_,M)


/* TAGS: sim_core_trace_1 sim_core_trace_2 */
/* TAGS: sim_core_trace_4 sim_core_trace_8 */
/* TAGS: sim_core_trace_16 */

#if (M == N)
STATIC_SIM_CORE(void)
sim_core_trace_M (sim_cpu *cpu,
		  sim_cia cia,
		  int line_nr,
		  transfer_type type,
		  sim_core_maps map,
		  address_word addr,
		  unsigned_M val,
		  int nr_bytes)
{
  char *transfer = (type == read_transfer ? "read" : "write");
  char *direction = (type == read_transfer ? "->" : "<-");
#if (M == 16)
  trace_printf (CPU_STATE (cpu), cpu,
		"sim-n-core.h:%d: %s-%d %s:0x%08lx %s 0x%08lx%08lx%08lx%08lx\n",
		line_nr,
		transfer, nr_bytes,
		sim_core_map_to_str (map),
		(unsigned long) addr,
		direction,
		(unsigned long) V4_16 (val, 0),
		(unsigned long) V4_16 (val, 1),
		(unsigned long) V4_16 (val, 2),
		(unsigned long) V4_16 (val, 3));
#endif
#if (M == 8)
  trace_printf (CPU_STATE (cpu), cpu,
		"sim-n-core.h:%d: %s-%d %s:0x%08lx %s 0x%08lx%08lx\n",
		line_nr,
		transfer, nr_bytes,
		sim_core_map_to_str (map),
		(unsigned long) addr,
		direction,
		(unsigned long) V4_8 (val, 0),
		(unsigned long) V4_8 (val, 1));
#endif
#if (M == 4)
  trace_printf (CPU_STATE (cpu), cpu,
		"sim-n-core.h:%d: %s-%d %s:0x%08lx %s 0x%08lx\n",
		line_nr,
		transfer,
		nr_bytes,
		sim_core_map_to_str (map),
		(unsigned long) addr,
		direction,
		(unsigned long) val);
#endif
#if (M == 2)
  trace_printf (CPU_STATE (cpu), cpu,
		"sim-n-core.h:%d: %s-%d %s:0x%08lx %s 0x%04lx\n",
		line_nr,
		transfer,
		nr_bytes,
		sim_core_map_to_str (map),
		(unsigned long) addr,
		direction,
		(unsigned long) val);
#endif
#if (M == 1)
  trace_printf (CPU_STATE (cpu), cpu,
		"sim-n-core.h:%d: %s-%d %s:0x%08lx %s 0x%02lx\n",
		line_nr,
		transfer,
		nr_bytes,
		sim_core_map_to_str (map),
		(unsigned long) addr,
		direction,
		(unsigned long) val);
#endif
}
#endif

  
/* TAGS: sim_core_read_aligned_1 sim_core_read_aligned_2 */
/* TAGS: sim_core_read_aligned_4 sim_core_read_aligned_8 */
/* TAGS: sim_core_read_aligned_16 */

#if (M == N)
INLINE_SIM_CORE(unsigned_M)
sim_core_read_aligned_N(sim_cpu *cpu,
			sim_cia cia,
			sim_core_maps map,
			address_word xaddr)
{
  sim_cpu_core *cpu_core = CPU_CORE (cpu);
  sim_core_common *core = &cpu_core->common;
  unsigned_M val;
  sim_core_mapping *mapping;
  address_word addr;
#if WITH_XOR_ENDIAN != 0
  if (WITH_XOR_ENDIAN)
    addr = xaddr ^ cpu_core->xor[(N - 1) % WITH_XOR_ENDIAN];
  else
#endif
    addr = xaddr;
  mapping = sim_core_find_mapping (core, map, addr, N, read_transfer, 1 /*abort*/, cpu, cia);
#if (WITH_DEVICES)
  if (WITH_CALLBACK_MEMORY && mapping->device != NULL) {
    unsigned_M data;
    if (device_io_read_buffer (mapping->device, &data, mapping->space, addr, N) != N)
      device_error (mapping->device, "internal error - %s - io_read_buffer should not fail",
		    XSTRING (sim_core_read_aligned_N));
    val = T2H_M (data);
  }
  else
#endif
    val = T2H_M (*(unsigned_M*) sim_core_translate (mapping, addr));
  PROFILE_COUNT_CORE (cpu, addr, N, map);
  if (TRACE_P (cpu, TRACE_CORE_IDX))
    sim_core_trace_M (cpu, cia, __LINE__, read_transfer, map, addr, val, N);
  return val;
}
#endif

/* TAGS: sim_core_read_unaligned_1 sim_core_read_unaligned_2 */
/* TAGS: sim_core_read_unaligned_4 sim_core_read_unaligned_8 */
/* TAGS: sim_core_read_unaligned_16 */

#if (M == N && N > 1)
INLINE_SIM_CORE(unsigned_M)
sim_core_read_unaligned_N(sim_cpu *cpu,
			  sim_cia cia,
			  sim_core_maps map,
			  address_word addr)
{
  int alignment = N - 1;
  /* if hardwired to forced alignment just do it */
  if (WITH_ALIGNMENT == FORCED_ALIGNMENT)
    return sim_core_read_aligned_N (cpu, cia, map, addr & ~alignment);
  else if ((addr & alignment) == 0)
    return sim_core_read_aligned_N (cpu, cia, map, addr);
  else
    switch (CURRENT_ALIGNMENT)
      {
      case STRICT_ALIGNMENT:
	SIM_CORE_SIGNAL (CPU_STATE (cpu), cpu, cia, map, N, addr,
			 read_transfer, sim_core_unaligned_signal);
      case NONSTRICT_ALIGNMENT:
	{
	  unsigned_M val;
	  if (sim_core_xor_read_buffer (CPU_STATE (cpu), cpu, map, &val, addr, N) != N)
	    SIM_CORE_SIGNAL (CPU_STATE (cpu), cpu, cia, map, N, addr,
			     read_transfer, sim_core_unaligned_signal);
	  val = T2H_M(val);
	  PROFILE_COUNT_CORE (cpu, addr, N, map);
	  if (TRACE_P (cpu, TRACE_CORE_IDX))
	    sim_core_trace_M (cpu, cia, __LINE__, read_transfer, map, addr, val, N);
	  return val;
	}
      case FORCED_ALIGNMENT:
	return sim_core_read_aligned_N (cpu, cia, map, addr & ~alignment);
      case MIXED_ALIGNMENT:
	sim_engine_abort (CPU_STATE (cpu), cpu, cia,
			  "internal error - %s - mixed alignment",
			  XSTRING (sim_core_read_unaligned_N));
      default:
	sim_engine_abort (CPU_STATE (cpu), cpu, cia,
			  "internal error - %s - bad switch",
			  XSTRING (sim_core_read_unaligned_N));
	/* to keep some compilers happy, we return a dummy */
	{
	  unsigned_M val[1] = { };
	  return val[0];
	}
      }
}
#endif

/* TAGS: sim_core_read_misaligned_3 sim_core_read_misaligned_5 */
/* TAGS: sim_core_read_misaligned_6 sim_core_read_misaligned_7 */

#if (M != N)
INLINE_SIM_CORE(unsigned_M)
sim_core_read_misaligned_N(sim_cpu *cpu,
			  sim_cia cia,
			  sim_core_maps map,
			  address_word addr)
{
  unsigned_M val = 0;
  if (sim_core_xor_read_buffer (CPU_STATE (cpu), cpu, map, &val, addr, N) != N)
    SIM_CORE_SIGNAL (CPU_STATE (cpu), cpu, cia, map, N, addr,
		     read_transfer, sim_core_unaligned_signal);
  if (CURRENT_HOST_BYTE_ORDER != CURRENT_TARGET_BYTE_ORDER)
    val = SWAP_M (val);
  if (CURRENT_TARGET_BYTE_ORDER == BIG_ENDIAN)
    val >>= (M - N) * 8;
  PROFILE_COUNT_CORE (cpu, addr, N, map);
  if (TRACE_P (cpu, TRACE_CORE_IDX))
    sim_core_trace_M (cpu, cia, __LINE__, read_transfer, map, addr, val, N);
  return val;
}
#endif

/* TAGS: sim_core_write_aligned_1 sim_core_write_aligned_2 */
/* TAGS: sim_core_write_aligned_4 sim_core_write_aligned_8 */
/* TAGS: sim_core_write_aligned_16 */

#if (M == N)
INLINE_SIM_CORE(void)
sim_core_write_aligned_N(sim_cpu *cpu,
			 sim_cia cia,
			 sim_core_maps map,
			 address_word xaddr,
			 unsigned_M val)
{
  sim_cpu_core *cpu_core = CPU_CORE (cpu);
  sim_core_common *core = &cpu_core->common;
  sim_core_mapping *mapping;
  address_word addr;
#if WITH_XOR_ENDIAN != 0
  if (WITH_XOR_ENDIAN)
    addr = xaddr ^ cpu_core->xor[(N - 1) % WITH_XOR_ENDIAN];
  else
#endif
    addr = xaddr;
  mapping = sim_core_find_mapping (core, map, addr, N, write_transfer, 1 /*abort*/, cpu, cia);
#if (WITH_DEVICES)
  if (WITH_CALLBACK_MEMORY && mapping->device != NULL) {
    unsigned_M data = H2T_M (val);
    if (device_io_write_buffer (mapping->device, &data, mapping->space, addr, N, cpu, cia) != N)
      device_error (mapping->device, "internal error - %s - io_write_buffer should not fail",
		    XSTRING (sim_core_write_aligned_N));
  }
  else
#endif
    *(unsigned_M*) sim_core_translate (mapping, addr) = H2T_M (val);
  PROFILE_COUNT_CORE (cpu, addr, N, map);
  if (TRACE_P (cpu, TRACE_CORE_IDX))
    sim_core_trace_M (cpu, cia, __LINE__, write_transfer, map, addr, val, N);
}
#endif

/* TAGS: sim_core_write_unaligned_1 sim_core_write_unaligned_2 */
/* TAGS: sim_core_write_unaligned_4 sim_core_write_unaligned_8 */
/* TAGS: sim_core_write_unaligned_16 */

#if (M == N && N > 1)
INLINE_SIM_CORE(void)
sim_core_write_unaligned_N(sim_cpu *cpu,
			   sim_cia cia,
			   sim_core_maps map,
			   address_word addr,
			   unsigned_M val)
{
  int alignment = N - 1;
  /* if hardwired to forced alignment just do it */
  if (WITH_ALIGNMENT == FORCED_ALIGNMENT)
    sim_core_write_aligned_N (cpu, cia, map, addr & ~alignment, val);
  else if ((addr & alignment) == 0)
    sim_core_write_aligned_N (cpu, cia, map, addr, val);
  else
    switch (CURRENT_ALIGNMENT)
      {
      case STRICT_ALIGNMENT:
	SIM_CORE_SIGNAL (CPU_STATE (cpu), cpu, cia, map, N, addr,
			 write_transfer, sim_core_unaligned_signal);
	break;
      case NONSTRICT_ALIGNMENT:
	{
	  unsigned_M data = H2T_M (val);
	  if (sim_core_xor_write_buffer (CPU_STATE (cpu), cpu, map, &data, addr, N) != N)
	    SIM_CORE_SIGNAL (CPU_STATE (cpu), cpu, cia, map, N, addr,
			     write_transfer, sim_core_unaligned_signal);
	  PROFILE_COUNT_CORE (cpu, addr, N, map);
	  if (TRACE_P (cpu, TRACE_CORE_IDX))
	    sim_core_trace_M (cpu, cia, __LINE__, write_transfer, map, addr, val, N);
	  break;
	}
      case FORCED_ALIGNMENT:
	sim_core_write_aligned_N (cpu, cia, map, addr & ~alignment, val);
	break;
      case MIXED_ALIGNMENT:
	sim_engine_abort (CPU_STATE (cpu), cpu, cia,
			  "internal error - %s - mixed alignment",
			  XSTRING (sim_core_write_unaligned_N));
	break;
      default:
	sim_engine_abort (CPU_STATE (cpu), cpu, cia,
			  "internal error - %s - bad switch",
			  XSTRING (sim_core_write_unaligned_N));
	break;
      }
}
#endif

/* TAGS: sim_core_write_misaligned_3 sim_core_write_misaligned_5 */
/* TAGS: sim_core_write_misaligned_6 sim_core_write_misaligned_7 */

#if (M != N)
INLINE_SIM_CORE(void)
sim_core_write_misaligned_N(sim_cpu *cpu,
			   sim_cia cia,
			   sim_core_maps map,
			   address_word addr,
			   unsigned_M val)
{
  unsigned_M data = val;
  if (CURRENT_TARGET_BYTE_ORDER == BIG_ENDIAN)
    data <<= (M - N) * 8;
  if (CURRENT_HOST_BYTE_ORDER != CURRENT_TARGET_BYTE_ORDER)
    data = SWAP_M (data);
  if (sim_core_xor_write_buffer (CPU_STATE (cpu), cpu, map, &data, addr, N) != N)
    SIM_CORE_SIGNAL (CPU_STATE (cpu), cpu, cia, map, N, addr,
		     write_transfer, sim_core_unaligned_signal);
  PROFILE_COUNT_CORE (cpu, addr, N, map);
  if (TRACE_P (cpu, TRACE_CORE_IDX))
    sim_core_trace_M (cpu, cia, __LINE__, write_transfer, map, addr, val, N);
}
#endif


/* NOTE: see start of file for #define of these macros */
#undef unsigned_M
#undef T2H_M
#undef H2T_M
#undef SWAP_M
#undef sim_core_read_aligned_N
#undef sim_core_read_unaligned_N
#undef sim_core_read_misaligned_N
#undef sim_core_write_aligned_N
#undef sim_core_write_unaligned_N
#undef sim_core_write_misaligned_N
#undef sim_core_trace_M
#undef M
#undef N
