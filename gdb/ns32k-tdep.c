/* Print NS 32000 instructions for GDB, the GNU debugger.
   Copyright 1986, 1988, 1991, 1992, 1994, 1995, 1998, 1999, 2000, 2001,
   2002 Free Software Foundation, Inc.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include "defs.h"
#include "frame.h"
#include "gdbcore.h"

static int sign_extend (int value, int bits);
static CORE_ADDR ns32k_get_enter_addr (CORE_ADDR);
static int ns32k_localcount (CORE_ADDR enter_pc);
static void flip_bytes (void *, int);

char *
ns32k_register_name_32082 (int regno)
{
  static char *register_names[] =
  {
    "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
    "f0", "f1", "f2", "f3", "f4", "f5", "f6", "f7",
    "sp", "fp", "pc", "ps",
    "l0", "l1", "l2", "l3", "xx",
  };

  if (regno < 0)
    return NULL;
  if (regno >= sizeof (register_names) / sizeof (*register_names))
    return NULL;

  return (register_names[regno]);
}

char *
ns32k_register_name_32382 (int regno)
{
  static char *register_names[] =
  {
    "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
    "f0", "f1", "f2", "f3", "f4", "f5", "f6", "f7",
    "sp", "fp", "pc", "ps",
    "fsr",
    "l0", "l1", "l2", "l3", "l4", "l5", "l6", "l7", "xx",
  };

  if (regno < 0)
    return NULL;
  if (regno >= sizeof (register_names) / sizeof (*register_names))
    return NULL;

  return (register_names[regno]);
}

int
ns32k_register_byte_32082 (int regno)
{
  if (regno >= LP0_REGNUM)
    return (LP0_REGNUM * 4) + ((regno - LP0_REGNUM) * 8);

  return (regno * 4);
}

int
ns32k_register_byte_32382 (int regno)
{
  /* This is a bit yuk.  The even numbered double precision floating
     point long registers occupy the same space as the even:odd numbered
     single precision floating point registers, but the extra 32381 FPU
     registers are at the end.  Doing it this way is compatible for both
     32081 and 32381 equipped machines.  */

  return ((regno < LP0_REGNUM ? regno
           : (regno - LP0_REGNUM) & 1 ? regno - 1
           : (regno - LP0_REGNUM + FP0_REGNUM)) * 4);
}

int
ns32k_register_raw_size (int regno)
{
  /* All registers are 4 bytes, except for the doubled floating
     registers.  */

  return ((regno >= LP0_REGNUM) ? 8 : 4);
}

int
ns32k_register_virtual_size (int regno)
{
  return ((regno >= LP0_REGNUM) ? 8 : 4);
}

struct type *
ns32k_register_virtual_type (int regno)
{
  if (regno < FP0_REGNUM)
    return (builtin_type_int);

  if (regno < FP0_REGNUM + 8)
    return (builtin_type_float);

  if (regno < LP0_REGNUM)
    return (builtin_type_int); 

  return (builtin_type_double);
}

/* Advance PC across any function entry prologue instructions
   to reach some "real" code.  */

CORE_ADDR
umax_skip_prologue (CORE_ADDR pc)
{
  register unsigned char op = read_memory_integer (pc, 1);
  if (op == 0x82)
    {
      op = read_memory_integer (pc + 2, 1);
      if ((op & 0x80) == 0)
	pc += 3;
      else if ((op & 0xc0) == 0x80)
	pc += 4;
      else
	pc += 6;
    }
  return pc;
}

const unsigned char *
ns32k_breakpoint_from_pc (CORE_ADDR *pcp, int *lenp)
{
  static const unsigned char breakpoint_insn[] = { 0xf2 };

  *lenp = sizeof (breakpoint_insn);
  return breakpoint_insn;
}

/* Return number of args passed to a frame.
   Can return -1, meaning no way to tell.
   Encore's C compiler often reuses same area on stack for args,
   so this will often not work properly.  If the arg names
   are known, it's likely most of them will be printed. */

int
umax_frame_num_args (struct frame_info *fi)
{
  int numargs;
  CORE_ADDR pc;
  CORE_ADDR enter_addr;
  unsigned int insn;
  unsigned int addr_mode;
  int width;

  numargs = -1;
  enter_addr = ns32k_get_enter_addr ((fi)->pc);
  if (enter_addr > 0)
    {
      pc = ((enter_addr == 1)
	    ? SAVED_PC_AFTER_CALL (fi)
	    : FRAME_SAVED_PC (fi));
      insn = read_memory_integer (pc, 2);
      addr_mode = (insn >> 11) & 0x1f;
      insn = insn & 0x7ff;
      if ((insn & 0x7fc) == 0x57c
	  && addr_mode == 0x14)	/* immediate */
	{
	  if (insn == 0x57c)	/* adjspb */
	    width = 1;
	  else if (insn == 0x57d)	/* adjspw */
	    width = 2;
	  else if (insn == 0x57f)	/* adjspd */
	    width = 4;
	  else
	    internal_error (__FILE__, __LINE__, "bad else");
	  numargs = read_memory_integer (pc + 2, width);
	  if (width > 1)
	    flip_bytes (&numargs, width);
	  numargs = -sign_extend (numargs, width * 8) / 4;
	}
    }
  return numargs;
}

static int
sign_extend (int value, int bits)
{
  value = value & ((1 << bits) - 1);
  return (value & (1 << (bits - 1))
	  ? value | (~((1 << bits) - 1))
	  : value);
}

static void
flip_bytes (void *p, int count)
{
  char tmp;
  char *ptr = 0;

  while (count > 0)
    {
      tmp = *ptr;
      ptr[0] = ptr[count - 1];
      ptr[count - 1] = tmp;
      ptr++;
      count -= 2;
    }
}

/* Return the number of locals in the current frame given a
   pc pointing to the enter instruction.  This is used by
   ns32k_frame_init_saved_regs.  */

static int
ns32k_localcount (CORE_ADDR enter_pc)
{
  unsigned char localtype;
  int localcount;

  localtype = read_memory_integer (enter_pc + 2, 1);
  if ((localtype & 0x80) == 0)
    localcount = localtype;
  else if ((localtype & 0xc0) == 0x80)
    localcount = (((localtype & 0x3f) << 8)
		  | (read_memory_integer (enter_pc + 3, 1) & 0xff));
  else
    localcount = (((localtype & 0x3f) << 24)
		  | ((read_memory_integer (enter_pc + 3, 1) & 0xff) << 16)
		  | ((read_memory_integer (enter_pc + 4, 1) & 0xff) << 8)
		  | (read_memory_integer (enter_pc + 5, 1) & 0xff));
  return localcount;
}


/* Nonzero if instruction at PC is a return instruction.  */

static int
ns32k_about_to_return (CORE_ADDR pc)
{
  return (read_memory_integer (pc, 1) == 0x12);
}

/* Get the address of the enter opcode for this function, if it is active.
   Returns positive address > 1 if pc is between enter/exit, 
   1 if pc before enter or after exit, 0 otherwise. */
static CORE_ADDR
ns32k_get_enter_addr (CORE_ADDR pc)
{
  CORE_ADDR enter_addr;
  unsigned char op;

  if (pc == 0)
    return 0;

  if (ns32k_about_to_return (pc))
    return 1;			/* after exit */

  enter_addr = get_pc_function_start (pc);

  if (pc == enter_addr)
    return 1;			/* before enter */

  op = read_memory_integer (enter_addr, 1);

  if (op != 0x82)
    return 0;			/* function has no enter/exit */

  return enter_addr;		/* pc is between enter and exit */
}

CORE_ADDR
ns32k_frame_chain (struct frame_info *frame)
{
  /* In the case of the NS32000 series, the frame's nominal address is the
     FP value, and that address is saved at the previous FP value as a
     4-byte word.  */

  if (inside_entry_file (frame->pc))
    return 0;

  return (read_memory_integer (frame->frame, 4));
}

CORE_ADDR
ns32k_frame_saved_pc (struct frame_info *frame)
{
  if (frame->signal_handler_caller)
    return (sigtramp_saved_pc (frame)); /* XXXJRT */

  return (read_memory_integer (frame->frame + 4, 4));
}

CORE_ADDR
ns32k_frame_args_address (struct frame_info *frame)
{
  if (ns32k_get_enter_addr (frame->pc) > 1)
    return (frame->frame);

  return (read_register (SP_REGNUM) - 4);
}

CORE_ADDR
ns32k_frame_locals_address (struct frame_info *frame)
{
  return (frame->frame);
}

/* Code to initialize the addresses of the saved registers of frame described
   by FRAME_INFO.  This includes special registers such as pc and fp saved in
   special ways in the stack frame.  sp is even more special: the address we
   return for it IS the sp for the next frame.  */

void
ns32k_frame_init_saved_regs (struct frame_info *frame)
{
  int regmask, regnum;
  int localcount;
  CORE_ADDR enter_addr, next_addr;

  if (frame->saved_regs)
    return;

  frame_saved_regs_zalloc (frame);

  enter_addr = ns32k_get_enter_addr (frame->pc);
  if (enter_addr > 1)
    {
      regmask = read_memory_integer (enter_addr + 1, 1) & 0xff;
      localcount = ns32k_localcount (enter_addr);
      next_addr = frame->frame + localcount;

      for (regnum = 0; regnum < 8; regnum++)
	{
          if (regmask & (1 << regnum))
	    frame->saved_regs[regnum] = next_addr -= 4;
	}

      frame->saved_regs[SP_REGNUM] = frame->frame + 4;
      frame->saved_regs[PC_REGNUM] = frame->frame + 4;
      frame->saved_regs[FP_REGNUM] = read_memory_integer (frame->frame, 4);
    }
  else if (enter_addr == 1)
    {
      CORE_ADDR sp = read_register (SP_REGNUM);
      frame->saved_regs[PC_REGNUM] = sp;
      frame->saved_regs[SP_REGNUM] = sp + 4;
    }
}

void
ns32k_push_dummy_frame (void)
{
  CORE_ADDR sp = read_register (SP_REGNUM);
  int regnum;

  sp = push_word (sp, read_register (PC_REGNUM));
  sp = push_word (sp, read_register (FP_REGNUM));
  write_register (FP_REGNUM, sp);

  for (regnum = 0; regnum < 8; regnum++)
    sp = push_word (sp, read_register (regnum));

  write_register (SP_REGNUM, sp);
}

void
ns32k_pop_frame (void)
{
  struct frame_info *frame = get_current_frame ();
  CORE_ADDR fp;
  int regnum;

  fp = frame->frame;
  FRAME_INIT_SAVED_REGS (frame);

  for (regnum = 0; regnum < 8; regnum++)
    if (frame->saved_regs[regnum])
      write_register (regnum,
		      read_memory_integer (frame->saved_regs[regnum], 4));

  write_register (FP_REGNUM, read_memory_integer (fp, 4));
  write_register (PC_REGNUM, read_memory_integer (fp + 4, 4));
  write_register (SP_REGNUM, fp + 8);
  flush_cached_frames ();
}

/* The NS32000 call dummy sequence:

	enter	0xff,0			82 ff 00
	jsr	@0x00010203		7f ae c0 01 02 03
	adjspd	0x69696969		7f a5 01 02 03 04
	bpt				f2

   It is 16 bytes long.  */

LONGEST ns32k_call_dummy_words[] =
{
  0x7f00ff82,
  0x0201c0ae,
  0x01a57f03,
  0xf2040302
};
int sizeof_ns32k_call_dummy_words = sizeof (ns32k_call_dummy_words);

#define NS32K_CALL_DUMMY_ADDR         5
#define NS32K_CALL_DUMMY_NARGS        11

void
ns32k_fix_call_dummy (char *dummy, CORE_ADDR pc, CORE_ADDR fun, int nargs,
                      struct value **args, struct type *type, int gcc_p)
{
  int flipped;

  flipped = fun | 0xc0000000;
  flip_bytes (&flipped, 4);
  store_unsigned_integer (dummy + NS32K_CALL_DUMMY_ADDR, 4, flipped);

  flipped = - nargs * 4;
  flip_bytes (&flipped, 4);
  store_unsigned_integer (dummy + NS32K_CALL_DUMMY_NARGS, 4, flipped);
}

void
_initialize_ns32k_tdep (void)
{
  tm_print_insn = print_insn_ns32k;
}
