/* Target-dependent code for Motorola 68HC11
   Copyright (C) 1999, 2000 Free Software Foundation, Inc.
   Contributed by Stephane Carrez, stcarrez@worldnet.fr

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

#if 0
/* FIXME: This is from tm-m68hc1.h */

#define GDB_TARGET_IS_M6811

/* Define the bit, byte, and word ordering of the machine.  */

#define TARGET_BYTE_ORDER	BIG_ENDIAN

/* Offset from address of function to start of its code.
   Zero on most machines.  */

#define FUNCTION_START_OFFSET 0

#ifdef __STDC__			/* Forward decls for prototypes */
struct frame_info;
struct frame_saved_regs;
struct type;
struct value;
#endif

/* Advance PC across any function entry prologue instructions
   to reach some "real" code.  */

extern CORE_ADDR m68hc11_skip_prologue ();
#define SKIP_PROLOGUE(ip) \
    m68hc11_skip_prologue (ip)


/* Stack grows downward.  */

#define INNER_THAN(lhs,rhs) ((lhs) < (rhs))

/* For a breakpoint, use "test".  This is also the breakpoint
   instruction on the 68HC12.  */
#define BREAKPOINT {0x0}

/* If your kernel resets the pc after the trap happens you may need to
   define this before including this file.  */
#define DECR_PC_AFTER_BREAK 0

extern char *m68hc11_register_names[];
#define REGISTER_NAME(i) m68hc11_register_names[i]

#define REGISTER_SIZE   2

/* Register numbers of various important registers.
   Note that some of these values are "real" register numbers,
   and correspond to the general registers of the machine,
   and some are "phony" register numbers which are too large
   to be actual register numbers as far as the user is concerned
   but do serve to get the desired values when passed to read_register.  */

#define X_REGNUM 	0
#define D_REGNUM	1
#define Y_REGNUM        2
#define SP_REGNUM 	3
#define PC_REGNUM 	4
#define A_REGNUM        5
#define B_REGNUM        6
#define PSW_REGNUM 	7
#define Z_REGNUM        8
#define FP_REGNUM       9
#define TMP_REGNUM     10
#define ZS_REGNUM      11
#define XY_REGNUM      12
#define ZD1_REGNUM     13
#define ZD32_REGNUM    (ZD1_REGNUM+31)

#define NUM_REGS       (ZD32_REGNUM+1)

#include "opcode/m68hc11.h"

/* Say how much memory is needed to store a copy of the register set */
#define REGISTER_BYTES    ((NUM_REGS)*2)

/* Index within `registers' of the first byte of the space for
   register N.  */

#define REGISTER_BYTE(N)  ((N) * 2)

/* Number of bytes of storage in the actual machine representation
   for register N.  */

#define REGISTER_RAW_SIZE(N) (2)

/* Number of bytes of storage in the program's representation
   for register N.  */

#define REGISTER_VIRTUAL_SIZE(N) (2)

/* Largest value REGISTER_RAW_SIZE can have.  */

#define MAX_REGISTER_RAW_SIZE 8

/* Largest value REGISTER_VIRTUAL_SIZE can have.  */

#define MAX_REGISTER_VIRTUAL_SIZE 8

/* Return the GDB type object for the "standard" data type
   of data in register N.  */

#define REGISTER_VIRTUAL_TYPE(N) builtin_type_uint16

/* Store the address of the place in which to copy the structure the
   subroutine will return.  This is called from call_function. 

   We store structs through a pointer passed in D */

#define STORE_STRUCT_RETURN(ADDR, SP) \
    { write_register (D_REGNUM, (ADDR));  }


/* Write into appropriate registers a function return value
   of type TYPE, given in virtual format.  

   Things always get returned in D/X */

#define STORE_RETURN_VALUE(TYPE,VALBUF) \
  write_register_bytes (REGISTER_BYTE (D_REGNUM), VALBUF, TYPE_LENGTH (TYPE))


/* Extract from an array REGBUF containing the (raw) register state
   the address in which a function should return its structure value,
   as a CORE_ADDR (or an expression that can be used as one).  */

#define EXTRACT_STRUCT_VALUE_ADDRESS(REGBUF) (*(CORE_ADDR *)(REGBUF))


/* Define other aspects of the stack frame. 
   we keep a copy of the worked out return pc lying around, since it
   is a useful bit of info */

#define EXTRA_FRAME_INFO \
    int frame_reg; \
    CORE_ADDR return_pc; \
    CORE_ADDR dummy; \
    int frameless; \
    int size;

/* There's a mess in stack frame creation.  See comments in blockframe.c
   near reference to INIT_FRAME_PC_FIRST.  */

#define	INIT_FRAME_PC(fromleaf, prev)	/* nada */

#define INIT_FRAME_PC_FIRST(fromleaf, prev) \
  (prev)->pc = ((fromleaf) ? SAVED_PC_AFTER_CALL ((prev)->next) : \
	      (prev)->next ? FRAME_SAVED_PC ((prev)->next) : read_pc ());

#define INIT_EXTRA_FRAME_INFO(fromleaf, fi) \
    m68hc11_init_extra_frame_info (fromleaf, fi)

extern void m68hc11_init_extra_frame_info (int fromleaf,
                                           struct frame_info * fi);

/* A macro that tells us whether the function invocation represented
   by FI does not have a frame on the stack associated with it.  If it
   does not, FRAMELESS is set to 1, else 0.  */

#define FRAMELESS_FUNCTION_INVOCATION(FI) \
  frameless_look_for_prologue (FI)

#define FRAME_CHAIN(FRAME)       m68hc11_frame_chain (FRAME)
#define FRAME_CHAIN_VALID(chain,frame)	\
      ((chain) != 0 && (frame) != 0)
#define FRAME_SAVED_PC(FRAME)    ((FRAME)->return_pc)
#define FRAME_ARGS_ADDRESS(fi)   (fi)->frame
#define FRAME_LOCALS_ADDRESS(fi) (fi)->frame

#define SAVED_PC_AFTER_CALL(frame) m68hc11_saved_pc_after_call (frame)

/* Set VAL to the number of args passed to frame described by FI.
   Can set VAL to -1, meaning no way to tell.  */
/* We can't tell how many args there are */

#define FRAME_NUM_ARGS(fi) (-1)

/* Return number of bytes at start of arglist that are not really args.  */

#define FRAME_ARGS_SKIP 0


/* Put here the code to store, into a struct frame_saved_regs,
   the addresses of the saved registers of frame described by FRAME_INFO.
   This includes special registers such as pc and fp saved in special
   ways in the stack frame.  sp is even more special:
   the address we return for it IS the sp for the next frame.  */

#define FRAME_FIND_SAVED_REGS(frame_info, frame_saved_regs)	    \
   m68hc11_frame_find_saved_regs (frame_info, &(frame_saved_regs))

extern void m68hc11_frame_find_saved_regs (struct frame_info *,
                                           struct frame_saved_regs *);

#define CALL_DUMMY		{ 0 }
#define PUSH_DUMMY_FRAME
#define CALL_DUMMY_START_OFFSET	0
#define CALL_DUMMY_BREAKPOINT_OFFSET (0)

extern CORE_ADDR m68hc11_call_dummy_address (void);
#define CALL_DUMMY_ADDRESS() m68hc11_call_dummy_address ()

#define FIX_CALL_DUMMY(dummyname, pc, fun, nargs, args, type, gcc_p) \
sp = m68hc11_fix_call_dummy (dummyname, pc, fun, nargs, args, type, gcc_p)

extern CORE_ADDR m68hc11_fix_call_dummy (char *, CORE_ADDR, CORE_ADDR,
                                         int, struct value **,
                                         struct type *, int);
#define PUSH_ARGUMENTS(nargs, args, sp, struct_return, struct_addr) \
    sp = m68hc11_push_arguments ((nargs), (args), (sp), \
                                 (struct_return), (struct_addr))
extern CORE_ADDR m68hc11_push_arguments (int, struct value **,
                                         CORE_ADDR, int, CORE_ADDR);


/* Extract from an array REGBUF containing the (raw) register state
   a function return value of type TYPE, and copy that, in virtual format,
   into VALBUF.  */

#define EXTRACT_RETURN_VALUE(TYPE,REGBUF,VALBUF) \
m68hc11_extract_return_value(TYPE, REGBUF, VALBUF)
extern void m68hc11_extract_return_value (struct type *, char *, char *);


/* Discard from the stack the innermost frame,
   restoring all saved registers.  */
#define POP_FRAME m68hc11_pop_frame();
extern void m68hc11_pop_frame (void);


/* Number of bits in the appropriate type.  */

#define TARGET_INT_BIT         (2 * TARGET_CHAR_BIT)
#define TARGET_PTR_BIT         (2 * TARGET_CHAR_BIT)
#define TARGET_DOUBLE_BIT      (4 * TARGET_CHAR_BIT)
#define TARGET_LONG_DOUBLE_BIT (8 * TARGET_CHAR_BIT)

#endif

#include "defs.h"
#include "frame.h"
#include "obstack.h"
#include "symtab.h"
#include "gdbtypes.h"
#include "gdbcmd.h"
#include "gdbcore.h"
#include "gdb_string.h"
#include "value.h"
#include "inferior.h"
#include "dis-asm.h"  
#include "symfile.h"
#include "objfiles.h"

/* NOTE: This port is not finished. Several operations are not implemented
   and will raise an error. Most of these functions concern the calling
   of a function by GDB itself (command 'call') and retrieving data pushed
   on the stack.  */

void m68hc11_frame_find_saved_regs (struct frame_info *fi,
                                    struct frame_saved_regs *fsr);
static void m68hc11_pop_dummy_frame (struct frame_info *fi);

/* Table of registers for 68HC11.  This includes the hard registers
   and the pseudo hard registers used by GCC.  */
char*
m68hc11_register_names[] =
{
  "x",    "d",    "y",    "sp",   "pc",   "a",    "b",
  "ccr",  "z",    "frame","tmp",  "zs",   "xy",
  "ZD1",  "ZD2",  "ZD3",  "ZD4",  "ZD5",  "ZD6",  "ZD7",
  "ZD8",  "ZD9",  "ZD10", "ZD11", "ZD12", "ZD13", "ZD14",
  "ZD15", "ZD16", "ZD17", "ZD18", "ZD19", "ZD20", "ZD21",
  "ZD22", "ZD23", "ZD24", "ZD25", "ZD26", "ZD27", "ZD28",
  "ZD29", "ZD30", "ZD31", "ZD32"
};

static int reg_last    = 32 * 2 + 6;
static int frame_index = 6;

/* Raise an error for operations which are not yet provided.  */
static void
m68hc11_not_yet (const char *operation)
{
  error ("Operation '%s' is not yet implemented\n", operation);
}

/* Immediately after a function call, return the saved pc before the frame
   is setup.  For sun3's, we check for the common case of being inside of a
   system call, and if so, we know that Sun pushes the call # on the stack
   prior to doing the trap. */

CORE_ADDR
m68hc11_saved_pc_after_call (struct frame_info *frame)
{
  unsigned addr = frame->frame + 1 + 2;

  addr = read_register (SP_REGNUM) + 1;
  addr &= 0x0ffff;
  return read_memory_integer (addr, 2) & 0x0FFFF;
}

/* Discard from the stack the innermost frame, restoring all saved
   registers.  */

void
m68hc11_pop_frame ()
{
  m68hc11_not_yet ("m68hc11_pop_frame");
}

/* Analyze the function prologue to find some information
   about the function:
    - the PC of the first line (for m68hc11_skip_prologue)
    - the offset of the previous frame saved address (from current frame)
    - the soft registers which are pushed.  */
static void
m68hc11_guess_from_prologue (CORE_ADDR pc, CORE_ADDR* first_line,
                             int* frame_offset, int* pushed_regs)
{
  CORE_ADDR func_end;
  unsigned char op0, op1, op2;
  int add_sp_mode;
  int sp_adjust;
  int size;
  int found_frame_point;
  int found_load;
  CORE_ADDR first_pc;
  int reg_saved;
  
  first_pc = get_pc_function_start (pc);
  size = 0;

  if (first_pc == 0)
    {
      *frame_offset = 0;
      *pushed_regs  = 0;
      *first_line   = pc;
      return;
    }

#define OP_PAGE2 (0x18)
#define OP_LDX  (0xde)
#define OP_LDY  (0xde)
#define OP_PSHX (0x3c)
#define OP_PSHY (0x3c)
#define OP_STS  (0x9f)
#define OP_TSX  (0x30)
#define OP_TSY  (0x30)
#define OP_XGDX (0x8f)
#define OP_XGDY (0x8f)
#define OP_ADDD (0xc3)
#define OP_TXS  (0x35)
#define OP_TYS  (0x35)

  /* The 68hc11 stack is as follows:


     |           |
     +-----------+
     |           |
     | args      |
     |           |
     +-----------+
     | PC-return |
     +-----------+
     | Old frame |
     +-----------+
     |           |
     | Locals    |
     |           |
     +-----------+ <--- current frame
     |           |

     With most processors (like 68K) the previous frame can be computed
     easily because it is always at a fixed offset (see link/unlink).
     That is, locals are accessed with negative offsets, arguments are
     accessed with positive ones.  Since 68hc11 only supports offsets
     in the range [0..255], the frame is defined at the bottom of
     locals (see picture).

     The purpose of the analysis made here is to find out the size
     of locals in this function.  An alternative to this is to use
     DWARF2 info.  This would be better but I don't know how to
     access dwarf2 debug from this function.
     
     Walk from the function entry point to the point where we save
     the frame.  While walking instructions, compute the size of bytes
     which are pushed.  This gives us the index to access the previous
     frame.

     We limit the search to 128 bytes so that the algorithm is bounded
     in case of random and wrong code.  We also stop and abort if
     we find an instruction which is not supposed to appear in the
     prologue (as generated by gcc 2.95, 2.96).
  */
  pc = first_pc;
  func_end = pc + 128;
  add_sp_mode = 0;
  found_frame_point = 0;
  while (pc + 2 < func_end)
    {
      op0 = read_memory_unsigned_integer (pc, 1);
      op1 = read_memory_unsigned_integer (pc + 1, 1);
      op2 = read_memory_unsigned_integer (pc + 2, 1);
      
      /* ldx *frame */
      if (op0 == OP_LDX && op1 == frame_index)
        {
          pc += 2;
        }

      /* ldy *frame */
      else if (op0 == OP_PAGE2 && op1 == OP_LDY && op2 == frame_index)
        {
          pc += 3;
        }

      /* pshx */
      else if (op0 == OP_PSHX)
        {
          pc += 1;
          size += 2;
        }

      /* pshy */
      else if (op0 == OP_PAGE2 && op1 == OP_PSHX)
        {
          pc += 2;
          size += 2;
        }

      /* sts *frame */
      else if (op0 == OP_STS && op1 == frame_index)
        {
          found_frame_point = 1;
          pc += 2;
          break;
        }
      else if (op0 == OP_TSX && op1 == OP_XGDX)
        {
          add_sp_mode  = 1;
          pc += 2;
        }
      else if (op0 == OP_PAGE2 && op1 == OP_TSY && op2 == OP_PAGE2)
        {
          op0 = read_memory_unsigned_integer (pc + 3, 1);
          if (op0 != OP_XGDY)
            break;
          
          add_sp_mode  = 2;
          pc += 4;
        }
      else if (add_sp_mode && op0 == OP_ADDD)
        {
          sp_adjust = read_memory_unsigned_integer (pc + 1, 2);
          if (sp_adjust & 0x8000)
            sp_adjust |= 0xffff0000L;

          sp_adjust = -sp_adjust;
          add_sp_mode |= 4;
          pc += 3;
        }
      else if (add_sp_mode == (1 | 4) && op0 == OP_XGDX
               && op1 == OP_TXS)
        {
          size += sp_adjust;
          pc += 2;
          add_sp_mode = 0;
        }
      else if (add_sp_mode == (2 | 4) && op0 == OP_PAGE2
               && op1 == OP_XGDY && op2 == OP_PAGE2)
        {
          op0 = read_memory_unsigned_integer (pc + 3, 1);
          if (op0 != OP_TYS)
            break;

          size += sp_adjust;
          pc += 4;
          add_sp_mode = 0;
        }
      else
        {
          break;
        }
    }

  if (found_frame_point == 0)
    {
      *frame_offset = 0;
    }
  else
    {
      *frame_offset = size;
    }

  /* Now, look forward to see how many registers are pushed on the stack.
     We look only for soft registers so there must be a first LDX *REG
     before a PSHX.  */
  reg_saved = 0;
  found_load = 0;
  while (pc + 2 < func_end)
    {
      op0 = read_memory_unsigned_integer (pc, 1);
      op1 = read_memory_unsigned_integer (pc + 1, 1);
      op2 = read_memory_unsigned_integer (pc + 2, 1);
      if (op0 == OP_LDX && op1 > frame_index && op1 <= reg_last)
        {
          found_load = 1;
          pc += 2;
        }
      else if (op0 == OP_PAGE2 && op1 == OP_LDY
               && op2 > frame_index && op2 < reg_last)
        {
          found_load = 1;
          pc += 3;
        }
      else if (op0 == OP_PSHX)
        {
          /* If there was no load, this is a push for a function call.  */
          if (found_load == 0)
            break;
          
          reg_saved += 2;
          pc += 1;
          found_load = 0;
        }
      else if (op0 == OP_PAGE2 && op1 == OP_PSHY)
        {
          if (found_load == 0)
            break;
          
          reg_saved += 2;
          pc += 2;
          found_load = 0;
        }
      else
        {
          break;
        }
    }
  *pushed_regs = reg_saved;
  *first_line  = pc;
}


CORE_ADDR
m68hc11_skip_prologue (CORE_ADDR pc)
{
  CORE_ADDR func_addr, func_end;
  struct symtab_and_line sal;
  int frame_offset;
  int pushed_args;

  /* If we have line debugging information, then the end of the.  */
  /* prologue should be the first assembly instruction of the
     first source line.  */
  if (find_pc_partial_function (pc, NULL, &func_addr, &func_end))
    {
      sal = find_pc_line (func_addr, 0);
      if (sal.end && sal.end < func_end)
	return sal.end;
    }

  m68hc11_guess_from_prologue (pc, &pc, &frame_offset, &pushed_args);
  return pc;
}

/* Given a GDB frame, determine the address of the calling function's frame.
   This will be used to create a new GDB frame struct, and then
   INIT_EXTRA_FRAME_INFO and INIT_FRAME_PC will be called for the new frame.
*/

CORE_ADDR
m68hc11_frame_chain (struct frame_info *frame)
{
  unsigned addr;

  if (frame->return_pc == 0 || inside_entry_file(frame->return_pc))
    return (CORE_ADDR)0;

  if (frame->frame == 0)
    {
      return (CORE_ADDR) 0;
    }

  addr = frame->frame + frame->size + 1 - 2;
  addr = read_memory_unsigned_integer (addr, 2) & 0x0FFFF;
  if (addr == 0)
    {
      return (CORE_ADDR)0;
    }
    
  return addr;
}  

/* Put here the code to store, into a struct frame_saved_regs, the
   addresses of the saved registers of frame described by FRAME_INFO.
   This includes special registers such as pc and fp saved in special
   ways in the stack frame.   sp is even more special: the address we
   return for it IS the sp for the next frame.  */
void
m68hc11_frame_find_saved_regs (struct frame_info *fi,
                               struct frame_saved_regs *fsr)
{
  CORE_ADDR pc;
  int saved;
  
  pc = fi->pc;
  memset (fsr, 0, sizeof (*fsr));
  m68hc11_guess_from_prologue (pc, &pc, &fi->size, &saved);
}

void
m68hc11_init_extra_frame_info (int fromleaf, struct frame_info *fi)
{
  unsigned addr;
  struct frame_saved_regs dummy;

  m68hc11_frame_find_saved_regs (fi, &dummy);

  if (fromleaf)
    {
      fi->return_pc = m68hc11_saved_pc_after_call (fi);
    }
  else
    {
      addr = fi->frame + fi->size + 1;
      fi->return_pc = read_memory_unsigned_integer (addr, 2) & 0x0ffff;

#if 0
      printf ("Pc@0x%04x, FR 0x%04x, size %d, read ret @0x%04x -> 0x%04x\n",
              fi->pc,
              fi->frame, fi->size,
              addr & 0x0ffff,
              fi->return_pc);
#endif
    }
}

/* Same as 'info reg' but prints the registers in a different way.  */
static void
show_regs (char *args, int from_tty)
{
  int ccr = read_register (PSW_REGNUM);
  int i;
    
  printf_filtered ("PC=%04x SP=%04x FP=%04x CCR=%02x %c%c%c%c%c%c%c%c\n",
		   read_register (PC_REGNUM),
		   read_register (SP_REGNUM),
		   read_register (FP_REGNUM),
		   ccr,
		   ccr & M6811_S_BIT ? 'S' : '-',
		   ccr & M6811_X_BIT ? 'X' : '-',
		   ccr & M6811_H_BIT ? 'H' : '-',
		   ccr & M6811_I_BIT ? 'I' : '-',
		   ccr & M6811_N_BIT ? 'N' : '-',
		   ccr & M6811_Z_BIT ? 'Z' : '-',
		   ccr & M6811_V_BIT ? 'V' : '-',
		   ccr & M6811_C_BIT ? 'C' : '-');

  printf_filtered ("D=%04x IX=%04x IY=%04x\n",
		   read_register (D_REGNUM),
		   read_register (X_REGNUM),
		   read_register (Y_REGNUM));
  for (i = ZD1_REGNUM; i <= ZD32_REGNUM; i++)
    {
      printf_filtered ("ZD%d=%04x",
                       i - ZD1_REGNUM + 1,
                       read_register (i));
      if (((i - ZD1_REGNUM) % 8) == 7)
        printf_filtered ("\n");
      else
        printf_filtered (" ");
    }
}

CORE_ADDR
m68hc11_fix_call_dummy (char *dummyname,
                        CORE_ADDR start_sp,
                        CORE_ADDR fun,
                        int nargs,
                        value_ptr *args,
                        struct type *type,
                        int gcc_p)
{
  m68hc11_not_yet ("m68hc11_fix_call_dummy");
  return 0;
}

static void
m68hc11_pop_dummy_frame (struct frame_info *fi)
{
  m68hc11_not_yet ("m68hc11_pop_dummy_frame");
}


CORE_ADDR
m68hc11_push_arguments (int nargs,
                        value_ptr *args,
                        CORE_ADDR sp,
                        int struct_return,
                        CORE_ADDR struct_addr)
{
  m68hc11_not_yet ("m68hc11_push_arguments");
  return 0;
}


CORE_ADDR
m68hc11_call_dummy_address ()
{
  m68hc11_not_yet ("m68hc11_call_dummy_address");
  return 0;  
}

/* Given a return value in `regbuf' with a type `valtype', 
   extract and copy its value into `valbuf'.  */

void
m68hc11_extract_return_value (struct type *valtype,
                              char *regbuf,
                              char *valbuf)
{
  m68hc11_not_yet ("m68hc11_extract_return_value");
}

void
_initialize_m68hc11_tdep ()
{
  tm_print_insn = print_insn_m68hc11;

  add_com ("regs", class_vars, show_regs, "Print all registers");
} 

