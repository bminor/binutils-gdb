/* Parameters for execution on a Sony/NEWS, for GDB, the GNU debugger.
   Copyright (C) 1987, 1989 Free Software Foundation, Inc.

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

/* See following cpu type determination macro to get the machine type.
  
Here is an m-news.h file for gdb.  It supports the 68881 registers.
                                           by hikichi@srava.sra.junet
  
* Support Sun assembly format instead of Motorola one.
* Ptrace for handling floating register has a bug(before NEWS OS version 2.2),
* After NEWS OS version 3.2, some of ptrace's bug is fixed.
  But we cannot change the floating register(see adb(1) in OS 3.2) yet.  */

/* Define this if the C compiler puts an underscore at the front
   of external names before giving them to the linker.  */

#define NAMES_HAVE_UNDERSCORE

/* Symbols on this machine are in DBX format. */
#define READ_DBX_FORMAT

/* Use to compute STACK_END_ADDR.  */
#define TARGET_UPAGES 2
#define TARGET_NBPG 4096

/* Address of end of stack space.  */

#define STACK_END_ADDR (0x80000000 - TARGET_UPAGES * TARGET_NBPG)

/* Extract from an array REGBUF containing the (raw) register state
   a function return value of type TYPE, and copy that, in virtual format,
   into VALBUF.  */

/* when it return the floating value, use the FP0 in NEWS.  */
#define EXTRACT_RETURN_VALUE(TYPE,REGBUF,VALBUF) \
  { if (TYPE_CODE (TYPE) == TYPE_CODE_FLT) \
      { \
	REGISTER_CONVERT_TO_VIRTUAL (FP0_REGNUM, \
			       &REGBUF[REGISTER_BYTE (FP0_REGNUM)], VALBUF); \
      } \
    else \
      bcopy (REGBUF, VALBUF, TYPE_LENGTH (TYPE)); }

/* Write into appropriate registers a function return value
   of type TYPE, given in virtual format.  */

/* when it return the floating value, use the FP0 in NEWS.  */
#define STORE_RETURN_VALUE(TYPE,VALBUF) \
  { if (TYPE_CODE (TYPE) == TYPE_CODE_FLT) \
      { \
	char raw_buf[REGISTER_RAW_SIZE (FP0_REGNUM)]; \
	REGISTER_CONVERT_TO_RAW (FP0_REGNUM, VALBUF, raw_buf); \
	write_register_bytes (FP0_REGNUM, \
			      raw_buf, REGISTER_RAW_SIZE (FP0_REGNUM)); \
      } \
    else \
      write_register_bytes (0, VALBUF, TYPE_LENGTH (TYPE)); }

/* Return number of args passed to a frame.
   Can return -1, meaning no way to tell.  */

#define FRAME_NUM_ARGS(val, fi)  \
{ register CORE_ADDR pc = FRAME_SAVED_PC (fi);			\
  register int insn = 0177777 & read_memory_integer (pc, 2);	\
  val = 0;							\
  if (insn == 0047757 || insn == 0157374)  /* lea W(sp),sp or addaw #W,sp */ \
    val = read_memory_integer (pc + 2, 2);			\
  else if ((insn & 0170777) == 0050217 /* addql #N, sp */	\
	   || (insn & 0170777) == 0050117)  /* addqw */		\
    { val = (insn >> 9) & 7; if (val == 0) val = 8; }		\
  else if (insn == 0157774) /* addal #WW, sp */			\
    val = read_memory_integer (pc + 2, 4);			\
  val >>= 2; }

/* Things needed for making the inferior call functions.  */
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

/* Discard from the stack the innermost frame, restoring all registers.  */

#define POP_FRAME  \
{ register FRAME frame = get_current_frame ();			 	\
  register CORE_ADDR fp;					 	\
  register int regnum;							\
  struct frame_saved_regs fsr;						\
  struct frame_info *fi;						\
  char raw_buffer[12];							\
  fi = get_frame_info (frame);					 	\
  fp = fi->frame;						 	\
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
  flush_cached_frames ();					 	\
  set_current_frame (create_new_frame (read_register (FP_REGNUM),	\
				       read_pc ())); }

/* This sequence of words is the instructions
     fmove.m #<f0-f7>,-(sp)
     movem.l 0xfffc,-(sp)     ;; no save a6(fp) and a7(sp)
     clr.w -(sp)
     move.w ccr,-(sp)
     /..* The arguments are pushed at this point by GDB;
	no code is needed in the dummy for this.
	The CALL_DUMMY_START_OFFSET gives the position of 
	the following jsr instruction.  *../
     jbsr (#32323232)
     add.l #69696969,sp
     bpt
     nop
Note this is 24 bytes.
We actually start executing at the jsr, since the pushing of the
registers is done by PUSH_DUMMY_FRAME.  If this were real code,
the arguments for the function called by the jsr would be pushed
between the moveml and the jsr, and we could allow it to execute through.
But the arguments have to be pushed by GDB after the PUSH_DUMMY_FRAME is done,
and we cannot allow the moveml to push the registers again lest they be
taken for the arguments.  */

#define CALL_DUMMY {0xf227e0ff, 0x48e7fffc, 0x426742e7, 0x4eb93232, 0x3232dffc, 0x69696969, 0x4e4f4e71}

#define CALL_DUMMY_LENGTH 28

#define CALL_DUMMY_START_OFFSET 12

/* Insert the specified number of args and function address
   into a call sequence of the above form stored at DUMMYNAME.  */

#define FIX_CALL_DUMMY(dummyname, pc, fun, nargs, args, type, gcc_p)     \
{ *(int *)((char *) dummyname + 20) = nargs * 4;  \
  *(int *)((char *) dummyname + 14) = fun; }

#define HAVE_68881

#include "tm-68k.h"
