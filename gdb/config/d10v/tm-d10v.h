/* Target-specific definition for the Mitsubishi D10V
   Copyright (C) 1996 Free Software Foundation, Inc.

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

/* Contributed by Martin Hunt, hunt@cygnus.com */

#define GDB_TARGET_IS_D10V

/* Define the bit, byte, and word ordering of the machine.  */

#define TARGET_BYTE_ORDER	BIG_ENDIAN

/* Offset from address of function to start of its code.
   Zero on most machines.  */

#define FUNCTION_START_OFFSET 0

/* these are the addresses the D10V-EVA board maps data */
/* and instruction memory to. */

#define DMEM_START	0x0000000
#define IMEM_START	0x1000000
#define STACK_START	0x0007ffe

#ifdef __STDC__		/* Forward decls for prototypes */
struct frame_info;
struct frame_saved_regs; 
struct type;
struct value;
#endif

/* Advance PC across any function entry prologue instructions
   to reach some "real" code.  */

extern CORE_ADDR d10v_skip_prologue ();
#define SKIP_PROLOGUE(ip) \
    {(ip) = d10v_skip_prologue(ip);}


/* Stack grows downward.  */
#define INNER_THAN(lhs,rhs) ((lhs) < (rhs))

/* for a breakpoint, use "dbt || nop" */
#define BREAKPOINT {0x2f, 0x90, 0x5e, 0x00} 

/* If your kernel resets the pc after the trap happens you may need to
   define this before including this file.  */
#define DECR_PC_AFTER_BREAK 4

#define REGISTER_NAMES \
{ "r0", "r1", "r2", "r3", "r4", "r5",  "r6", "r7", \
    "r8", "r9", "r10","r11","r12", "r13", "r14","r15",\
    "psw","bpsw","pc","bpc", "cr4", "cr5", "cr6", "rpt_c",\
    "rpt_s","rpt_e", "mod_s", "mod_e", "cr12", "cr13", "iba", "cr15",\
    "imap0","imap1","dmap","a0", "a1"\
    }

#define NUM_REGS 37

/* Register numbers of various important registers.
   Note that some of these values are "real" register numbers,
   and correspond to the general registers of the machine,
   and some are "phony" register numbers which are too large
   to be actual register numbers as far as the user is concerned
   but do serve to get the desired values when passed to read_register.  */

#define R0_REGNUM	0
#define LR_REGNUM 	13
#define SP_REGNUM 	15
#define FP_REGNUM	11
#define PC_REGNUM 	18
#define PSW_REGNUM 	16
#define IMAP0_REGNUM	32
#define IMAP1_REGNUM	33
#define DMAP_REGNUM	34
#define A0_REGNUM 	35

/* Say how much memory is needed to store a copy of the register set */
#define REGISTER_BYTES    ((NUM_REGS-2)*2+16) 

/* Index within `registers' of the first byte of the space for
   register N.  */

#define REGISTER_BYTE(N)  \
( ((N) > A0_REGNUM) ? ( ((N)-A0_REGNUM)*8 + A0_REGNUM*2 ) : ((N) * 2) )

/* Number of bytes of storage in the actual machine representation
   for register N.  */

#define REGISTER_RAW_SIZE(N) ( ((N) >= A0_REGNUM) ? 8 : 2 )

/* Number of bytes of storage in the program's representation
   for register N.  */   
#define REGISTER_VIRTUAL_SIZE(N) ( ((N) >= A0_REGNUM) ? 8 : ( ((N) == PC_REGNUM || (N) == SP_REGNUM) ? 4 : 2 ))

/* Largest value REGISTER_RAW_SIZE can have.  */

#define MAX_REGISTER_RAW_SIZE 8

/* Largest value REGISTER_VIRTUAL_SIZE can have.  */

#define MAX_REGISTER_VIRTUAL_SIZE 8

/* Return the GDB type object for the "standard" data type
   of data in register N.  */

#define REGISTER_VIRTUAL_TYPE(N) \
( ((N) < A0_REGNUM ) ? ((N) == PC_REGNUM || (N) == SP_REGNUM ? builtin_type_long : builtin_type_short) : builtin_type_long_long)


/* convert $pc and $sp to/from virtual addresses */
#define REGISTER_CONVERTIBLE(N) ((N) == PC_REGNUM || (N) == SP_REGNUM)
#define REGISTER_CONVERT_TO_VIRTUAL(REGNUM,TYPE,FROM,TO) \
{ \
    ULONGEST x = extract_unsigned_integer ((FROM), REGISTER_RAW_SIZE (REGNUM)); \
    if (REGNUM == PC_REGNUM) x = (x << 2) | IMEM_START; \
    else x |= DMEM_START; \
    store_unsigned_integer ((TO), TYPE_LENGTH(TYPE), x); \
}
#define REGISTER_CONVERT_TO_RAW(TYPE,REGNUM,FROM,TO) \
{ \
    ULONGEST x = extract_unsigned_integer ((FROM), TYPE_LENGTH(TYPE)); \
    x &= 0x3ffff; \
    if (REGNUM == PC_REGNUM) x >>= 2; \
    store_unsigned_integer ((TO), 2, x); \
}

#define D10V_MAKE_DADDR(x) ((x) | DMEM_START)
#define D10V_MAKE_IADDR(x) (((x) << 2) | IMEM_START)

#define D10V_DADDR_P(X) (((X) & 0x3000000) == DMEM_START)
#define D10V_IADDR_P(X) (((X) & 0x3000000) == IMEM_START)

#define D10V_CONVERT_IADDR_TO_RAW(X) (((X) >> 2) & 0xffff)
#define D10V_CONVERT_DADDR_TO_RAW(X) ((X) & 0xffff)

#define ARG1_REGNUM R0_REGNUM
#define ARGN_REGNUM 3
#define RET1_REGNUM R0_REGNUM

/* Store the address of the place in which to copy the structure the
   subroutine will return.  This is called from call_function. 

   We store structs through a pointer passed in the first Argument
   register. */

#define STORE_STRUCT_RETURN(ADDR, SP) \
    { write_register (ARG1_REGNUM, (ADDR));  }


/* Write into appropriate registers a function return value
   of type TYPE, given in virtual format.  

   Things always get returned in RET1_REGNUM, RET2_REGNUM, ... */

#define STORE_RETURN_VALUE(TYPE,VALBUF) \
  write_register_bytes (REGISTER_BYTE(RET1_REGNUM), VALBUF, TYPE_LENGTH (TYPE))


/* Extract from an array REGBUF containing the (raw) register state
   the address in which a function should return its structure value,
   as a CORE_ADDR (or an expression that can be used as one).  */

#define EXTRACT_STRUCT_VALUE_ADDRESS(REGBUF) \
     (extract_address ((REGBUF) + REGISTER_BYTE (ARG1_REGNUM), REGISTER_RAW_SIZE (ARG1_REGNUM)) | DMEM_START)

/* Should we use EXTRACT_STRUCT_VALUE_ADDRESS instead of
   EXTRACT_RETURN_VALUE?  GCC_P is true if compiled with gcc
   and TYPE is the type (which is known to be struct, union or array).

   The d10v returns anything less than 8 bytes in size in
   registers. */

extern use_struct_convention_fn d10v_use_struct_convention;
#define USE_STRUCT_CONVENTION(gcc_p, type) d10v_use_struct_convention (gcc_p, type)



/* Define other aspects of the stack frame. 
   we keep a copy of the worked out return pc lying around, since it
   is a useful bit of info */

#define EXTRA_FRAME_INFO \
    CORE_ADDR return_pc; \
    int frameless; \
    int size;

#define INIT_EXTRA_FRAME_INFO(fromleaf, fi) \
    d10v_init_extra_frame_info(fromleaf, fi) 

extern void d10v_init_extra_frame_info PARAMS (( int fromleaf, struct frame_info *fi ));

/* A macro that tells us whether the function invocation represented
   by FI does not have a frame on the stack associated with it.  If it
   does not, FRAMELESS is set to 1, else 0.  */

#define FRAMELESS_FUNCTION_INVOCATION(FI, FRAMELESS) \
  (FRAMELESS) = frameless_look_for_prologue(FI)

#define FRAME_CHAIN(FRAME)       d10v_frame_chain(FRAME)
extern int d10v_frame_chain_valid PARAMS ((CORE_ADDR, struct frame_info *));
#define FRAME_CHAIN_VALID(chain, thisframe) d10v_frame_chain_valid (chain, thisframe)
#define FRAME_SAVED_PC(FRAME)    ((FRAME)->return_pc)   
#define FRAME_ARGS_ADDRESS(fi)   (fi)->frame
#define FRAME_LOCALS_ADDRESS(fi) (fi)->frame

/* Immediately after a function call, return the saved pc.  We can't */
/* use frame->return_pc beause that is determined by reading R13 off the */
/*stack and that may not be written yet. */

#define SAVED_PC_AFTER_CALL(frame) ((read_register(LR_REGNUM) << 2) | IMEM_START)
    
/* Set VAL to the number of args passed to frame described by FI.
   Can set VAL to -1, meaning no way to tell.  */
/* We can't tell how many args there are */

#define FRAME_NUM_ARGS(val,fi) (val = -1)

/* Return number of bytes at start of arglist that are not really args.  */

#define FRAME_ARGS_SKIP 0


/* Put here the code to store, into a struct frame_saved_regs,
   the addresses of the saved registers of frame described by FRAME_INFO.
   This includes special registers such as pc and fp saved in special
   ways in the stack frame.  sp is even more special:
   the address we return for it IS the sp for the next frame.  */

#define FRAME_FIND_SAVED_REGS(frame_info, frame_saved_regs)	    \
   d10v_frame_find_saved_regs(frame_info, &(frame_saved_regs))

extern void d10v_frame_find_saved_regs PARAMS ((struct frame_info *, struct frame_saved_regs *));

#define NAMES_HAVE_UNDERSCORE


/* DUMMY FRAMES.  Need these to support inferior function calls.  They
   work like this on D10V: First we set a breakpoint at 0 or __start.
   Then we push all the registers onto the stack.  Then put the
   function arguments in the proper registers and set r13 to our
   breakpoint address.  Finally, the PC is set to the start of the
   function being called (no JSR/BSR insn).  When it hits the
   breakpoint, clear the break point and pop the old register contents
   off the stack.  */

extern void d10v_pop_frame PARAMS ((struct frame_info *frame));
#define POP_FRAME generic_pop_current_frame (d10v_pop_frame)

#define USE_GENERIC_DUMMY_FRAMES
#define CALL_DUMMY                   {0}
#define CALL_DUMMY_START_OFFSET      (0)
#define CALL_DUMMY_BREAKPOINT_OFFSET (0)
#define CALL_DUMMY_LOCATION          AT_ENTRY_POINT
#define FIX_CALL_DUMMY(DUMMY, START, FUNADDR, NARGS, ARGS, TYPE, GCCP)
#define CALL_DUMMY_ADDRESS()         entry_point_address ()
extern CORE_ADDR d10v_push_return_address PARAMS ((CORE_ADDR pc, CORE_ADDR sp));
#define PUSH_RETURN_ADDRESS(PC, SP)  d10v_push_return_address (PC, SP)

#define PC_IN_CALL_DUMMY(PC, SP, FP) generic_pc_in_call_dummy (PC, SP)
/* #define PC_IN_CALL_DUMMY(pc, sp, frame_address) ( pc == IMEM_START + 4 ) */

#define PUSH_DUMMY_FRAME	generic_push_dummy_frame ()

/* override the default get_saved_register function with one that
   takes account of generic CALL_DUMMY frames */
#define GET_SAVED_REGISTER
#define get_saved_register generic_get_saved_register

#define PUSH_ARGUMENTS(nargs, args, sp, struct_return, struct_addr) \
    sp = d10v_push_arguments((nargs), (args), (sp), (struct_return), (struct_addr))
extern CORE_ADDR d10v_push_arguments PARAMS ((int, struct value **, CORE_ADDR, int, CORE_ADDR));


/* Extract from an array REGBUF containing the (raw) register state
   a function return value of type TYPE, and copy that, in virtual format,
   into VALBUF.  */

#define EXTRACT_RETURN_VALUE(TYPE,REGBUF,VALBUF) \
d10v_extract_return_value(TYPE, REGBUF, VALBUF)
  extern void
d10v_extract_return_value PARAMS ((struct type *, char *, char *));


#define REGISTER_SIZE 2

#ifdef CC_HAS_LONG_LONG
#  define LONGEST long long
#else
#  define LONGEST long
#endif 
#define ULONGEST unsigned LONGEST

void d10v_write_pc PARAMS ((CORE_ADDR val, int pid));
CORE_ADDR d10v_read_pc PARAMS ((int pid));
void d10v_write_sp PARAMS ((CORE_ADDR val));
CORE_ADDR d10v_read_sp PARAMS ((void));
void d10v_write_fp PARAMS ((CORE_ADDR val));
CORE_ADDR d10v_read_fp PARAMS ((void));

#define TARGET_READ_PC(pid)		d10v_read_pc (pid)
#define TARGET_WRITE_PC(val,pid)	d10v_write_pc (val, pid)
#define TARGET_READ_FP()		d10v_read_fp ()
#define TARGET_WRITE_FP(val)		d10v_write_fp (val)
#define TARGET_READ_SP()		d10v_read_sp ()
#define TARGET_WRITE_SP(val)		d10v_write_sp (val)

/* Number of bits in the appropriate type */
#define TARGET_INT_BIT (2 * TARGET_CHAR_BIT)
#define TARGET_PTR_BIT (4 * TARGET_CHAR_BIT)
#define TARGET_DOUBLE_BIT (4 * TARGET_CHAR_BIT)
#define TARGET_LONG_DOUBLE_BIT (8 * TARGET_CHAR_BIT)


/* For the d10v when talking to the remote d10v board, GDB addresses
   need to be translated into a format that the d10v rom monitor
   understands. */

int remote_d10v_translate_xfer_address PARAMS ((CORE_ADDR gdb_addr, int gdb_len, CORE_ADDR *rem_addr));
#define REMOTE_TRANSLATE_XFER_ADDRESS(GDB_ADDR, GDB_LEN, REM_ADDR, REM_LEN) \
(REM_LEN) = remote_d10v_translate_xfer_address ((GDB_ADDR), (GDB_LEN), &(REM_ADDR))

