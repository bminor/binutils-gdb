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

#define NUM_REGS 65

#define REGISTER_NAMES \
{ "r0", "r1", "r2", "sp", "gp", "r5", "r6", "r7", \
  "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15", \
  "r16", "r17", "r18", "r19", "r20", "r21", "r22", "r23", \
  "r24", "r25", "r26", "r27", "r28", "r29", "ep", "r31", \
    \
  "eipc", "eipsw", "fepc", "fepsw", "ecr", "psw", "sr6", "sr7", \
  "sr8", "sr9", "sr10", "sr11", "sr12", "sr13", "sr14", "sr15", \
  "sr16", "sr17", "sr18", "sr19", "sr20", "sr21", "sr22", "sr23", \
  "sr24", "sr25", "sr26", "sr27", "sr28", "sr29", "sr30", "sr31", \
  "pc" }

#define REGISTER_BYTES (NUM_REGS * 4)

#define REGISTER_SIZE 4
#define MAX_REGISTER_RAW_SIZE 4

#define SP_REGNUM 3
#define FP_REGNUM 2
#define V0_REGNUM 10
#define V1_REGNUM 11
#define RP_REGNUM 31
#define PC_REGNUM 64

#define REGISTER_VIRTUAL_TYPE(REG) builtin_type_int

#define REGISTER_BYTE(REG) ((REG) * 4)
#define REGISTER_VIRTUAL_SIZE(REG) 4
#define REGISTER_RAW_SIZE(REG) 4

#define MAX_REGISTER_VIRTUAL_SIZE 4

#define BREAKPOINT {0xff, 0xff}

#define FUNCTION_START_OFFSET 0

#define DECR_PC_AFTER_BREAK 0

#define INNER_THAN <

#define SAVED_PC_AFTER_CALL(fi) read_register (RP_REGNUM)

#ifdef __STDC__
struct frame_info;
struct frame_saved_regs;
struct type;
#endif

#define EXTRA_FRAME_INFO struct frame_saved_regs fsr;

extern void v850_init_extra_frame_info PARAMS ((struct frame_info *fi));
#define INIT_EXTRA_FRAME_INFO(fromleaf, fi) v850_init_extra_frame_info (fi)
#define INIT_FRAME_PC		/* Not necessary */

extern void v850_frame_find_saved_regs PARAMS ((struct frame_info *fi, struct frame_saved_regs *regaddr));
#define FRAME_FIND_SAVED_REGS(fi, regaddr) regaddr = fi->fsr

extern CORE_ADDR v850_frame_chain PARAMS ((struct frame_info *fi));
#define FRAME_CHAIN(fi) v850_frame_chain (fi)

extern CORE_ADDR v850_find_callers_reg PARAMS ((struct frame_info *fi, int regnum));
#define FRAME_SAVED_PC(fi) (v850_find_callers_reg (fi, RP_REGNUM))

#define EXTRACT_RETURN_VALUE(TYPE, REGBUF, VALBUF) \
  memcpy (VALBUF, REGBUF + REGISTER_BYTE (V0_REGNUM), TYPE_LENGTH (TYPE))

#define EXTRACT_STRUCT_VALUE_ADDRESS(REGBUF) \
  (extract_address (REGBUF + REGISTER_BYTE (V0_REGNUM), \
		    REGISTER_RAW_SIZE (V0_REGNUM)))

#define STORE_RETURN_VALUE(TYPE, VALBUF) \
  write_register_bytes(REGISTER_BYTE (V0_REGNUM), VALBUF, TYPE_LENGTH (TYPE));

extern CORE_ADDR v850_skip_prologue PARAMS ((CORE_ADDR pc));
#define SKIP_PROLOGUE(pc) pc = v850_skip_prologue (pc)

#define FRAME_ARGS_SKIP 0

#define FRAME_ARGS_ADDRESS(fi) ((fi)->frame)
#define FRAME_LOCALS_ADDRESS(fi) ((fi)->frame)
#define FRAME_NUM_ARGS(val, fi) ((val) = -1)

extern struct frame_info *v850_pop_frame PARAMS ((struct frame_info *frame));
#define POP_FRAME v850_pop_frame (get_current_frame ())
