/* Parameters for execution on an NEC V850 processor.
   Copyright 1996
   Free Software Foundation, Inc. 

This file is part of GDB.

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#define TARGET_BYTE_ORDER LITTLE_ENDIAN

#define NUM_REGS 64

#define REGISTER_NAMES \
{ "r0", "r1", "r2", "sp", "gp", "r5", "r6", "r7", \
  "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15", \
  "r16", "r17", "r18", "r19", "r20", "r21", "r22", "r23", \
  "r24", "r25", "r26", "r27", "r28", "r29", "ep", "r31", \
    \
  "eipc", "eipsw", "fepc", "fepsw", "ecr", "psw", "sr6", "sr7", \
  "sr8", "sr9", "sr10", "sr11", "sr12", "sr13", "sr14", "sr15", \
  "sr16", "sr17", "sr18", "sr19", "sr20", "sr21", "sr22", "sr23", \
  "sr24", "sr25", "sr26", "sr27", "sr28", "sr29", "sr30", "sr31" }

#define REGISTER_BYTES (NUM_REGS * 4)

#define REGISTER_SIZE 4
#define MAX_REGISTER_RAW_SIZE 4

#define SP_REGNUM 3
#define FP_REGNUM 2
#define RP_REGNUM 31
#define PC_REGNUM 32

#define REGISTER_VIRTUAL_TYPE(REG) builtin_type_int

#define REGISTER_BYTE(REG) ((REG) * 4)
#define REGISTER_VIRTUAL_SIZE(REG) 4
#define REGISTER_RAW_SIZE(REG) 4

#define MAX_REGISTER_VIRTUAL_SIZE 4

#define BREAKPOINT {0xff, 0xff}

#define FUNCTION_START_OFFSET 0

#define DECR_PC_AFTER_BREAK 0

#define POP_FRAME warning ("POP_FRAME not implemented yet!")

#define INNER_THAN <

#define FRAME_ARGS_SKIP 4

#define SAVED_PC_AFTER_CALL(fi) read_register (RP_REGNUM)

#define FRAME_CHAIN(fi) (read_memory_unsigned_integer ((fi)->frame - 8, 4))

#define FRAME_SAVED_PC(fi) (read_memory_unsigned_integer((fi)->frame - 4, 4))


#define SKIP_PROLOGUE(pc) pc+=8

#define FRAME_ARGS_ADDRESS(fi) ((fi)->frame)
#define FRAME_LOCALS_ADDRESS(fi) ((fi)->frame)
#define FRAME_NUM_ARGS(val, fi) ((val) = -1)
#define FRAME_FIND_SAVED_REGS(fi, regaddr) warning ("FRAME_FIND_SAVED_REGS not implemented yet!")

#define EXTRACT_RETURN_VALUE(TYPE, REGBUF, VALBUF) warning ("EXTRACT_RETURN_VALUE not implemented yet!")
#define STORE_RETURN_VALUE(TYPE, VALBUF) warning ("STORE_RETURN_VALUE not implemented yet!")

