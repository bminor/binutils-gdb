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


#ifndef _SIM_CORE_H_
#define _SIM_CORE_H_


/* core signals (error conditions) */

typedef enum {
  sim_core_unmapped_signal,
  sim_core_unaligned_signal,
  nr_sim_core_signals,
} sim_core_signals;

/* define SIM_CORE_SIGNAL to catch these signals - see sim-core.c for
   details */



/* basic types */

typedef struct _sim_core_mapping sim_core_mapping;
struct _sim_core_mapping {
  /* common */
  int level;
  int space;
  unsigned_word base;
  unsigned_word bound;
  unsigned_word nr_bytes;
  unsigned mask;
  /* memory map */
  void *free_buffer;
  void *buffer;
  /* callback map */
  device *device;
  /* tracing */
  int trace;
  /* growth */
  sim_core_mapping *next;
};

typedef struct _sim_core_map sim_core_map;
struct _sim_core_map {
  sim_core_mapping *first;
};

typedef enum {
  sim_core_read_map,
  sim_core_write_map,
  sim_core_execute_map,
  nr_sim_core_maps,
} sim_core_maps;


typedef struct _sim_core_common {
  sim_core_map map[nr_sim_core_maps];
} sim_core_common;


/* Main core structure */

typedef struct _sim_core sim_core;
struct _sim_core {
  sim_core_common common;
  address_word byte_xor; /* apply xor universally */
};


/* Per CPU distributed component of the core.  At present this is
   mostly a clone of the global core data structure. */

typedef struct _sim_cpu_core {
  sim_core_common common;
  address_word xor[WITH_XOR_ENDIAN + 1]; /* +1 to avoid zero-sized array */
} sim_cpu_core;


/* Install the "core" module.  */

EXTERN_SIM_CORE\
(SIM_RC) sim_core_install (SIM_DESC sd);



/* Create a memory space within the core.

   The CPU option (when non NULL) specifes the single processor that
   the memory space is to be attached to. (UNIMPLEMENTED).

   */

EXTERN_SIM_CORE\
(void) sim_core_attach
(SIM_DESC sd,
 sim_cpu *cpu,
 attach_type attach,
 access_type access,
 int address_space,
 address_word addr,
 address_word nr_bytes,
 unsigned modulo, /* Power of two, zero for none. */
 device *client,
 void *optional_buffer);

/* Delete a memory space within the core.

 */

EXTERN_SIM_CORE\
(void) sim_core_detach
(SIM_DESC sd,
 sim_cpu *cpu,
 attach_type attach,
 int address_space,
 address_word addr);


/* Variable sized read/write

   Transfer a variable sized block of raw data between the host and
   target.  Should any problems occure, the number of bytes
   successfully transfered is returned.

   No host/target byte endian conversion is performed.  No xor-endian
   conversion is performed.

   If CPU argument, when non NULL, specifies the processor specific
   address map that is to be used in the transfer. */


EXTERN_SIM_CORE\
(unsigned) sim_core_read_buffer
(SIM_DESC sd,
 sim_cpu *cpu,
 sim_core_maps map,
 void *buffer,
 address_word addr,
 unsigned nr_bytes);

EXTERN_SIM_CORE\
(unsigned) sim_core_write_buffer
(SIM_DESC sd,
 sim_cpu *cpu,
 sim_core_maps map,
 const void *buffer,
 address_word addr,
 unsigned nr_bytes);



/* Configure the core's XOR endian transfer mode.  Only applicable
   when WITH_XOR_ENDIAN is enabled.

   Targets suporting XOR endian, shall notify the core of any changes
   in state via this call.

   The CPU argument, when non NULL, specifes the single processor that
   the xor-endian configuration is to be applied to. */

EXTERN_SIM_CORE\
(void) sim_core_set_xor\
(SIM_DESC sd,
 sim_cpu *cpu,
 int is_xor);


/* XOR version of variable sized read/write.

   Transfer a variable sized block of raw data between the host and
   target.  Should any problems occure, the number of bytes
   successfully transfered is returned.

   No host/target byte endian conversion is performed.  If applicable
   (WITH_XOR_ENDIAN and xor-endian set), xor-endian conversion *is*
   performed.

   If CPU argument, when non NULL, specifies the processor specific
   address map that is to be used in the transfer. */

EXTERN_SIM_CORE\
(unsigned) sim_core_xor_read_buffer
(SIM_DESC sd,
 sim_cpu *cpu,
 sim_core_maps map,
 void *buffer,
 address_word addr,
 unsigned nr_bytes);

EXTERN_SIM_CORE\
(unsigned) sim_core_xor_write_buffer
(SIM_DESC sd,
 sim_cpu *cpu,
 sim_core_maps map,
 const void *buffer,
 address_word addr,
 unsigned nr_bytes);



/* Fixed sized, processor oriented, read/write.

   Transfer a fixed amout of memory between the host and target.  The
   data transfered is translated from/to host to/from target byte
   order (including xor endian).  Should the transfer fail, the
   operation shall abort (no return).

   The aligned alternative makes the assumption that that the address
   is N byte aligned (no alignment checks are made).

   The unaligned alternative checks the address for correct byte
   alignment.  Action, as defined by WITH_ALIGNMENT, being taken
   should the check fail.

   Misaligned xor-endian accesses are broken into a sequence of
   transfers each <= WITH_XOR_ENDIAN bytes */


#define DECLARE_SIM_CORE_WRITE_N(ALIGNMENT,N) \
INLINE_SIM_CORE\
(void) sim_core_write_##ALIGNMENT##_##N \
(sim_cpu *cpu, \
 sim_cia cia, \
 sim_core_maps map, \
 address_word addr, \
 unsigned_##N val);

DECLARE_SIM_CORE_WRITE_N(aligned,1)
DECLARE_SIM_CORE_WRITE_N(aligned,2)
DECLARE_SIM_CORE_WRITE_N(aligned,4)
DECLARE_SIM_CORE_WRITE_N(aligned,8)
DECLARE_SIM_CORE_WRITE_N(aligned,16)
DECLARE_SIM_CORE_WRITE_N(aligned,word)

DECLARE_SIM_CORE_WRITE_N(unaligned,1)
DECLARE_SIM_CORE_WRITE_N(unaligned,2)
DECLARE_SIM_CORE_WRITE_N(unaligned,4)
DECLARE_SIM_CORE_WRITE_N(unaligned,8)
DECLARE_SIM_CORE_WRITE_N(unaligned,16)
DECLARE_SIM_CORE_WRITE_N(unaligned,word)

#define sim_core_write_1 sim_core_write_aligned_1
#define sim_core_write_2 sim_core_write_aligned_2
#define sim_core_write_4 sim_core_write_aligned_4
#define sim_core_write_16 sim_core_write_aligned_16

#undef DECLARE_SIM_CORE_WRITE_N


#define DECLARE_SIM_CORE_READ_N(ALIGNMENT,N) \
INLINE_SIM_CORE\
(unsigned_##N) sim_core_read_##ALIGNMENT##_##N \
(sim_cpu *cpu, \
 sim_cia cia, \
 sim_core_maps map, \
 address_word addr);

DECLARE_SIM_CORE_READ_N(aligned,1)
DECLARE_SIM_CORE_READ_N(aligned,2)
DECLARE_SIM_CORE_READ_N(aligned,4)
DECLARE_SIM_CORE_READ_N(aligned,8)
DECLARE_SIM_CORE_READ_N(aligned,16)
DECLARE_SIM_CORE_READ_N(aligned,word)

DECLARE_SIM_CORE_READ_N(unaligned,1)
DECLARE_SIM_CORE_READ_N(unaligned,2)
DECLARE_SIM_CORE_READ_N(unaligned,4)
DECLARE_SIM_CORE_READ_N(unaligned,8)
DECLARE_SIM_CORE_READ_N(unaligned,16)
DECLARE_SIM_CORE_READ_N(unaligned,word)

#define sim_core_read_1 sim_core_read_aligned_1
#define sim_core_read_2 sim_core_read_aligned_2
#define sim_core_read_4 sim_core_read_aligned_4
#define sim_core_read_8 sim_core_read_aligned_8
#define sim_core_read_16 sim_core_read_aligned_16

#undef DECLARE_SIM_CORE_READ_N

#endif
