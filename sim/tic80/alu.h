#ifndef _TIC80_ALU_H_
#define _TIC80_ALU_H_

#define ALU_CARRY 0 /* FIXME */

#include "sim-alu.h"

#define ALU32_END(TARG) \
{ \
  (TARG) = (unsigned32)alu_carry_val; /* FIXME */ \
}}

#define ALU_END(TARG) ALU32_END(TARG)


/* Bring data in from the cold */

#define IMEM(EA) sim_core_read_4(sd, sim_core_execute_map, (EA))

#define MEM(SIGN, EA, NR_BYTES) \
((SIGN##_##NR_BYTES) sim_core_read_##NR_BYTES(sd, sim_core_read_map, (EA)))

#define STORE(EA, NR_BYTES, VAL) \
do { \
  sim_core_write_##NR_BYTES(sd, sim_core_write_map, (EA), (VAL)); \
} while (0)


#define long_immediate(VARIABLE) \
     unsigned_word VARIABLE = MEM (unsigned, nia.ip, 4); \
     nia.ip += sizeof (instruction_word); \
     nia.dp += sizeof (instruction_word);

#endif
