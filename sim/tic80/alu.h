/* Texas Instruments TMS320C80 (MVP) Simulator.
   Copyright (C) 1997 Free Software Foundation, Inc.
   Contributed by Cygnus Support.

This file is part of GDB, the GNU debugger.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */



#ifndef _TIC80_ALU_H_
#define _TIC80_ALU_H_

#define ALU_CARRY 0 /* FIXME */

#define ALU32_END(TARG) \
{ \
  (TARG) = (unsigned32)alu_carry_val; /* FIXME */ \
}}

#define ALU_END(TARG) ALU32_END(TARG)

#include "sim-alu.h"



/* Bring data in from the cold */

#define IMEM(EA) \
(sim_core_read_4(sd, sim_core_execute_map, (EA), \
 STATE_CPU (sd, 0), cia))

#define MEM(SIGN, EA, NR_BYTES) \
((SIGN##_##NR_BYTES) sim_core_read_##NR_BYTES (SD, sim_core_read_map, \
                                               (EA) & ~(NR_BYTES - 1), \
                                               STATE_CPU (sd, 0), cia))

#define STORE(EA, NR_BYTES, VAL) \
do { \
  sim_core_write_##NR_BYTES (SD, sim_core_write_map, \
                             (EA) & ~(NR_BYTES - 1), (VAL), \
                             STATE_CPU (sd, 0), cia); \
} while (0)


#define long_immediate(VARIABLE) \
     unsigned_word VARIABLE = MEM (unsigned, nia.ip, 4); \
     nia.ip += sizeof (instruction_word); \
     nia.dp += sizeof (instruction_word);



/* Floating point support */

#include "sim-fpu.h"


#endif
