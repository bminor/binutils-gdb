// OBSOLETE /* Parameters for execution on a Mitsubishi m32r processor.
// OBSOLETE    Copyright 1996, 1997, 1998, 1999, 2000, 2001, 2002, 2003
// OBSOLETE    Free Software Foundation, Inc. 
// OBSOLETE 
// OBSOLETE    This file is part of GDB.
// OBSOLETE 
// OBSOLETE    This program is free software; you can redistribute it and/or modify
// OBSOLETE    it under the terms of the GNU General Public License as published by
// OBSOLETE    the Free Software Foundation; either version 2 of the License, or
// OBSOLETE    (at your option) any later version.
// OBSOLETE 
// OBSOLETE    This program is distributed in the hope that it will be useful,
// OBSOLETE    but WITHOUT ANY WARRANTY; without even the implied warranty of
// OBSOLETE    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// OBSOLETE    GNU General Public License for more details.
// OBSOLETE 
// OBSOLETE    You should have received a copy of the GNU General Public License
// OBSOLETE    along with this program; if not, write to the Free Software
// OBSOLETE    Foundation, Inc., 59 Temple Place - Suite 330,
// OBSOLETE    Boston, MA 02111-1307, USA.  */
// OBSOLETE 
// OBSOLETE #include "regcache.h"
// OBSOLETE 
// OBSOLETE /* Used by mswin.  */
// OBSOLETE #define TARGET_M32R 1
// OBSOLETE 
// OBSOLETE /* mvs_check REGISTER_NAMES */
// OBSOLETE #define REGISTER_NAMES \
// OBSOLETE { "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", \
// OBSOLETE   "r8", "r9", "r10", "r11", "r12", "fp", "lr", "sp", \
// OBSOLETE   "psw", "cbr", "spi", "spu", "bpc", "pc", "accl", "acch", \
// OBSOLETE     /*  "cond", "sm", "bsm", "ie", "bie", "bcarry",  */ \
// OBSOLETE }
// OBSOLETE /* mvs_check  NUM_REGS */
// OBSOLETE #define NUM_REGS 			24
// OBSOLETE 
// OBSOLETE /* mvs_check  REGISTER_SIZE */
// OBSOLETE #define REGISTER_SIZE 			4
// OBSOLETE /* mvs_check  MAX_REGISTER_RAW_SIZE */
// OBSOLETE #define MAX_REGISTER_RAW_SIZE		4
// OBSOLETE 
// OBSOLETE /* mvs_check  *_REGNUM */
// OBSOLETE #define R0_REGNUM	0
// OBSOLETE #define STRUCT_RETURN_REGNUM 0
// OBSOLETE #define ARG0_REGNUM	0
// OBSOLETE #define ARGLAST_REGNUM	3
// OBSOLETE #define V0_REGNUM	0
// OBSOLETE #define V1_REGNUM	1
// OBSOLETE #define FP_REGNUM	13
// OBSOLETE #define RP_REGNUM	14
// OBSOLETE #define SP_REGNUM	15
// OBSOLETE #define PSW_REGNUM	16
// OBSOLETE #define CBR_REGNUM	17
// OBSOLETE #define SPI_REGNUM	18
// OBSOLETE #define SPU_REGNUM	19
// OBSOLETE #define BPC_REGNUM	20
// OBSOLETE #define PC_REGNUM	21
// OBSOLETE #define ACCL_REGNUM	22
// OBSOLETE #define ACCH_REGNUM	23
// OBSOLETE 
// OBSOLETE /* mvs_check  REGISTER_BYTES */
// OBSOLETE #define REGISTER_BYTES			(NUM_REGS * 4)
// OBSOLETE 
// OBSOLETE /* mvs_check  REGISTER_VIRTUAL_TYPE */
// OBSOLETE #define REGISTER_VIRTUAL_TYPE(REG)	builtin_type_int
// OBSOLETE 
// OBSOLETE /* mvs_check  REGISTER_BYTE */
// OBSOLETE #define REGISTER_BYTE(REG) 		((REG) * 4)
// OBSOLETE /* mvs_check  REGISTER_VIRTUAL_SIZE */
// OBSOLETE #define REGISTER_VIRTUAL_SIZE(REG) 	4
// OBSOLETE /* mvs_check  REGISTER_RAW_SIZE */
// OBSOLETE #define REGISTER_RAW_SIZE(REG)     	4
// OBSOLETE 
// OBSOLETE /* mvs_check  MAX_REGISTER_VIRTUAL_SIZE */
// OBSOLETE #define MAX_REGISTER_VIRTUAL_SIZE 	4
// OBSOLETE 
// OBSOLETE /* mvs_check  BREAKPOINT */
// OBSOLETE #define BREAKPOINT {0x10, 0xf1}
// OBSOLETE 
// OBSOLETE /* mvs_no_check  FUNCTION_START_OFFSET */
// OBSOLETE #define FUNCTION_START_OFFSET 0
// OBSOLETE 
// OBSOLETE /* mvs_check  DECR_PC_AFTER_BREAK */
// OBSOLETE #define DECR_PC_AFTER_BREAK 0
// OBSOLETE 
// OBSOLETE /* mvs_check  INNER_THAN */
// OBSOLETE #define INNER_THAN(lhs,rhs) ((lhs) < (rhs))
// OBSOLETE 
// OBSOLETE /* mvs_check  SAVED_PC_AFTER_CALL */
// OBSOLETE #define SAVED_PC_AFTER_CALL(fi) read_register (RP_REGNUM)
// OBSOLETE 
// OBSOLETE struct frame_info;
// OBSOLETE struct frame_saved_regs;
// OBSOLETE struct type;
// OBSOLETE struct value;
// OBSOLETE 
// OBSOLETE /* Define other aspects of the stack frame. 
// OBSOLETE    We keep the offsets of all saved registers, 'cause we need 'em a lot!
// OBSOLETE    We also keep the current size of the stack frame, and whether 
// OBSOLETE    the frame pointer is valid (for frameless functions, and when we're
// OBSOLETE    still in the prologue of a function with a frame) */
// OBSOLETE 
// OBSOLETE /* mvs_check  EXTRA_FRAME_INFO */
// OBSOLETE #define EXTRA_FRAME_INFO  	\
// OBSOLETE   struct frame_saved_regs fsr;	\
// OBSOLETE   int framesize;		\
// OBSOLETE   int using_frame_pointer;
// OBSOLETE 
// OBSOLETE 
// OBSOLETE extern void m32r_init_extra_frame_info (struct frame_info *fi);
// OBSOLETE /* mvs_check  INIT_EXTRA_FRAME_INFO */
// OBSOLETE #define INIT_EXTRA_FRAME_INFO(fromleaf, fi) m32r_init_extra_frame_info (fi)
// OBSOLETE /* mvs_no_check  DEPRECATED_INIT_FRAME_PC */
// OBSOLETE #define DEPRECATED_INIT_FRAME_PC		init_frame_pc_noop
// OBSOLETE 
// OBSOLETE extern void
// OBSOLETE m32r_frame_find_saved_regs (struct frame_info *fi,
// OBSOLETE 			    struct frame_saved_regs *regaddr);
// OBSOLETE 
// OBSOLETE /* Put here the code to store, into a struct frame_saved_regs,
// OBSOLETE    the addresses of the saved registers of frame described by FRAME_INFO.
// OBSOLETE    This includes special registers such as pc and fp saved in special
// OBSOLETE    ways in the stack frame.  sp is even more special:
// OBSOLETE    the address we return for it IS the sp for the next frame.  */
// OBSOLETE 
// OBSOLETE /* mvs_check  FRAME_FIND_SAVED_REGS */
// OBSOLETE #define FRAME_FIND_SAVED_REGS(frame_info, frame_saved_regs)	    \
// OBSOLETE    m32r_frame_find_saved_regs(frame_info, &(frame_saved_regs))
// OBSOLETE 
// OBSOLETE extern CORE_ADDR m32r_frame_chain (struct frame_info *fi);
// OBSOLETE /* mvs_check  FRAME_CHAIN */
// OBSOLETE #define FRAME_CHAIN(fi) 		m32r_frame_chain (fi)
// OBSOLETE 
// OBSOLETE extern CORE_ADDR m32r_find_callers_reg (struct frame_info *fi, int regnum);
// OBSOLETE extern CORE_ADDR m32r_frame_saved_pc (struct frame_info *);
// OBSOLETE /* mvs_check  FRAME_SAVED_PC */
// OBSOLETE #define FRAME_SAVED_PC(fi)		m32r_frame_saved_pc (fi)
// OBSOLETE 
// OBSOLETE /* mvs_check  DEPRECATED_EXTRACT_RETURN_VALUE */
// OBSOLETE #define DEPRECATED_EXTRACT_RETURN_VALUE(TYPE, REGBUF, VALBUF) \
// OBSOLETE   memcpy ((VALBUF), \
// OBSOLETE 	  (char *)(REGBUF) + REGISTER_BYTE (V0_REGNUM) + \
// OBSOLETE 	  ((TYPE_LENGTH (TYPE) > 4 ? 8 : 4) - TYPE_LENGTH (TYPE)), \
// OBSOLETE 	  TYPE_LENGTH (TYPE))
// OBSOLETE 
// OBSOLETE /* mvs_check  DEPRECATED_STORE_RETURN_VALUE */
// OBSOLETE #define DEPRECATED_STORE_RETURN_VALUE(TYPE, VALBUF) \
// OBSOLETE   deprecated_write_register_bytes(REGISTER_BYTE (V0_REGNUM) + \
// OBSOLETE 		       ((TYPE_LENGTH (TYPE) > 4 ? 8:4) - TYPE_LENGTH (TYPE)),\
// OBSOLETE 		       (VALBUF), TYPE_LENGTH (TYPE));
// OBSOLETE 
// OBSOLETE extern CORE_ADDR m32r_skip_prologue (CORE_ADDR pc);
// OBSOLETE /* mvs_check  SKIP_PROLOGUE */
// OBSOLETE #define SKIP_PROLOGUE(pc) (m32r_skip_prologue (pc))
// OBSOLETE 
// OBSOLETE /* mvs_no_check  FRAME_ARGS_SKIP */
// OBSOLETE #define FRAME_ARGS_SKIP 0
// OBSOLETE 
// OBSOLETE /* mvs_no_check  FRAME_ARGS_ADDRESS */
// OBSOLETE #define FRAME_ARGS_ADDRESS(fi) ((fi)->frame)
// OBSOLETE /* mvs_no_check  FRAME_LOCALS_ADDRESS */
// OBSOLETE #define FRAME_LOCALS_ADDRESS(fi) ((fi)->frame)
// OBSOLETE /* mvs_no_check  FRAME_NUM_ARGS */
// OBSOLETE #define FRAME_NUM_ARGS(fi) (-1)
// OBSOLETE 
// OBSOLETE extern void m32r_write_sp (CORE_ADDR val);
// OBSOLETE #define TARGET_WRITE_SP m32r_write_sp
// OBSOLETE 
// OBSOLETE 
// OBSOLETE 
// OBSOLETE 
// OBSOLETE 
// OBSOLETE 
// OBSOLETE /* struct passing and returning stuff */
// OBSOLETE #define STORE_STRUCT_RETURN(STRUCT_ADDR, SP)	\
// OBSOLETE 	write_register (0, STRUCT_ADDR)
// OBSOLETE 
// OBSOLETE extern use_struct_convention_fn m32r_use_struct_convention;
// OBSOLETE #define USE_STRUCT_CONVENTION(GCC_P, TYPE) m32r_use_struct_convention (GCC_P, TYPE)
// OBSOLETE 
// OBSOLETE #define DEPRECATED_EXTRACT_STRUCT_VALUE_ADDRESS(REGBUF) \
// OBSOLETE   extract_address (REGBUF + REGISTER_BYTE (V0_REGNUM), \
// OBSOLETE 		   REGISTER_RAW_SIZE (V0_REGNUM))
// OBSOLETE 
// OBSOLETE #define REG_STRUCT_HAS_ADDR(gcc_p,type)     (TYPE_LENGTH (type) > 8)
// OBSOLETE 
// OBSOLETE 
// OBSOLETE /* generic dummy frame stuff */
// OBSOLETE 
// OBSOLETE #define PUSH_DUMMY_FRAME             generic_push_dummy_frame ()
// OBSOLETE #define DEPRECATED_PC_IN_CALL_DUMMY(PC, SP, FP) generic_pc_in_call_dummy (PC, SP, FP)
// OBSOLETE 
// OBSOLETE 
// OBSOLETE /* target-specific dummy_frame stuff */
// OBSOLETE 
// OBSOLETE extern struct frame_info *m32r_pop_frame (struct frame_info *frame);
// OBSOLETE /* mvs_check  POP_FRAME */
// OBSOLETE #define POP_FRAME m32r_pop_frame (get_current_frame ())
// OBSOLETE 
// OBSOLETE /* mvs_no_check  STACK_ALIGN */
// OBSOLETE /* #define STACK_ALIGN(x) ((x + 3) & ~3) */
// OBSOLETE 
// OBSOLETE extern CORE_ADDR m32r_push_return_address (CORE_ADDR, CORE_ADDR);
// OBSOLETE extern CORE_ADDR m32r_push_arguments (int nargs,
// OBSOLETE 				      struct value **args,
// OBSOLETE 				      CORE_ADDR sp,
// OBSOLETE 				      unsigned char struct_return,
// OBSOLETE 				      CORE_ADDR struct_addr);
// OBSOLETE 
// OBSOLETE 
// OBSOLETE 
// OBSOLETE /* mvs_no_check  PUSH_ARGUMENTS */
// OBSOLETE #define PUSH_ARGUMENTS(NARGS, ARGS, SP, STRUCT_RETURN, STRUCT_ADDR) \
// OBSOLETE   (m32r_push_arguments (NARGS, ARGS, SP, STRUCT_RETURN, STRUCT_ADDR))
// OBSOLETE 
// OBSOLETE #define PUSH_RETURN_ADDRESS(PC, SP)      m32r_push_return_address (PC, SP)
// OBSOLETE 
// OBSOLETE /* override the standard get_saved_register function with 
// OBSOLETE    one that takes account of generic CALL_DUMMY frames */
// OBSOLETE #define GET_SAVED_REGISTER(raw_buffer, optimized, addrp, frame, regnum, lval) \
// OBSOLETE      deprecated_generic_get_saved_register (raw_buffer, optimized, addrp, frame, regnum, lval)
// OBSOLETE 
// OBSOLETE 
// OBSOLETE #define DEPRECATED_USE_GENERIC_DUMMY_FRAMES 1
// OBSOLETE #define CALL_DUMMY                   {0}
// OBSOLETE #define CALL_DUMMY_LENGTH            (0)
// OBSOLETE #define CALL_DUMMY_START_OFFSET      (0)
// OBSOLETE #define CALL_DUMMY_BREAKPOINT_OFFSET (0)
// OBSOLETE #define FIX_CALL_DUMMY(DUMMY1, STARTADDR, FUNADDR, NARGS, ARGS, TYPE, GCCP)
// OBSOLETE #define CALL_DUMMY_LOCATION          AT_ENTRY_POINT
// OBSOLETE #define CALL_DUMMY_ADDRESS()         entry_point_address ()
