/* Definitions to make GDB run on an Altos 3068 (m68k running SVR2)
   Copyright (C) 1987,1989 Free Software Foundation, Inc.

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
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* Sequence of bytes for breakpoint instruction.  */

#define BREAKPOINT {0x4e, 0x4e}

/* Define this if the C compiler puts an underscore at the front
   of external names before giving them to the linker.  */

#undef NAMES_HAVE_UNDERSCORE

/* Exec files and symbol tables are in COFF format */

#define COFF_FORMAT
#define COFF_NO_LONG_FILE_NAMES

/* Address of end of stack space.  */

/*#define STACK_END_ADDR (0xffffff)*/
#define STACK_END_ADDR (0x1000000)

/* Amount PC must be decremented by after a breakpoint.
   This is often the number of bytes in BREAKPOINT
   but not always.
   On the Altos, the kernel resets the pc to the trap instr */

#define DECR_PC_AFTER_BREAK 0


/* Things needed for making the inferior call functions.  */

/* Push an empty stack frame, to record the current PC, etc.  */

#define PUSH_DUMMY_FRAME \
{ register CORE_ADDR sp = read_register (SP_REGNUM);			\
  register int regnum;							\
  char raw_buffer[12];							\
  sp = push_word (sp, read_register (PC_REGNUM));			\
  sp = push_word (sp, read_register (FP_REGNUM));			\
  write_register (FP_REGNUM, sp);					\
  for (regnum = FP0_REGNUM + 7; regnum >= FP0_REGNUM; regnum--)		\
    { read_register_bytes (REGISTER_BYTE (regnum), raw_buffer, 12);	\
      sp = push_bytes (sp, raw_buffer, 12); }				\
  for (regnum = FP_REGNUM - 1; regnum >= 0; regnum--)			\
    sp = push_word (sp, read_register (regnum));			\
  sp = push_word (sp, read_register (PS_REGNUM));			\
  write_register (SP_REGNUM, sp);  }

/* Discard from the stack the innermost frame, 
   restoring all saved registers.  */

#define POP_FRAME  \
{ register FRAME frame = get_current_frame ();				\
  register CORE_ADDR fp;						\
  register int regnum;							\
  struct frame_saved_regs fsr;						\
  struct frame_info *fi;						\
  char raw_buffer[12];							\
  fi = get_frame_info (frame);						\
  fp = fi->frame;							\
  get_frame_saved_regs (fi, &fsr);					\
  for (regnum = FP0_REGNUM + 7; regnum >= FP0_REGNUM; regnum--)		\
    if (fsr.regs[regnum])						\
      { read_memory (fsr.regs[regnum], raw_buffer, 12);			\
        write_register_bytes (REGISTER_BYTE (regnum), raw_buffer, 12); }\
  for (regnum = FP_REGNUM - 1; regnum >= 0; regnum--)			\
    if (fsr.regs[regnum])						\
      write_register (regnum, read_memory_integer (fsr.regs[regnum], 4)); \
  if (fsr.regs[PS_REGNUM])						\
    write_register (PS_REGNUM, read_memory_integer (fsr.regs[PS_REGNUM], 4)); \
  write_register (FP_REGNUM, read_memory_integer (fp, 4));		\
  write_register (PC_REGNUM, read_memory_integer (fp + 4, 4));  	\
  write_register (SP_REGNUM, fp + 8);					\
  flush_cached_frames ();						\
  set_current_frame (create_new_frame (read_register (FP_REGNUM),	\
					read_pc ())); }

/* This sequence of words is the instructions
     fmovem 0xff,-(sp)
     moveml 0xfffc,-(sp)
     clrw -(sp)
     movew ccr,-(sp)
     /..* The arguments are pushed at this point by GDB;
	no code is needed in the dummy for this.
	The CALL_DUMMY_START_OFFSET gives the position of 
	the following jsr instruction.  *../
     jsr @#32323232
     addl #69696969,sp
     bpt
     nop
Note this is 28 bytes.
We actually start executing at the jsr, since the pushing of the
registers is done by PUSH_DUMMY_FRAME.  If this were real code,
the arguments for the function called by the jsr would be pushed
between the moveml and the jsr, and we could allow it to execute through.
But the arguments have to be pushed by GDB after the PUSH_DUMMY_FRAME is done,
and we cannot allow the moveml to push the registers again lest they be
taken for the arguments.  */

#define CALL_DUMMY {0xf227e0ff, 0x48e7fffc, 0x426742e7, 0x4eb93232, 0x3232dffc, 0x69696969, 0x4e4e4e71}

#define CALL_DUMMY_LENGTH 28

#define CALL_DUMMY_START_OFFSET 12

/* Insert the specified number of args and function address
   into a call sequence of the above form stored at DUMMYNAME.  */

#define FIX_CALL_DUMMY(dummyname, pc, fun, nargs, args, type, gcc_p)     \
{ *(int *)((char *) dummyname + 20) = nargs * 4;  \
  *(int *)((char *) dummyname + 14) = fun; }

#include "tm-68k.h"
