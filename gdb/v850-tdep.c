/* Target-dependent code for the NEC V850 for GDB, the GNU debugger.
   Copyright 1996, Free Software Foundation, Inc.

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

#include "defs.h"
#include "frame.h"
#include "inferior.h"
#include "obstack.h"
#include "target.h"
#include "value.h"
#include "bfd.h"
#include "gdb_string.h"
#include "gdbcore.h"

struct dummy_frame
{
  struct dummy_frame *next;

  CORE_ADDR fp;
  CORE_ADDR sp;
  CORE_ADDR rp;
  CORE_ADDR pc;
};

static struct dummy_frame *dummy_frame_stack = NULL;

/* This function actually figures out the frame address for a given pc and
   sp.  This is tricky on the v850 because we only use an explicit frame
   pointer when using alloca().  The only reliable way to get this info is to
   examine the prologue.
*/

void
v850_init_extra_frame_info (fi)
     struct frame_info *fi;
{
  struct symtab_and_line sal;
  CORE_ADDR func_addr, prologue_end, current_pc;
  int reg;
  int frameoffset;
  int framereg;

  if (fi->next)
    fi->pc = FRAME_SAVED_PC (fi->next);

  /* First, figure out the bounds of the prologue so that we can limit the
     search to something reasonable.  */

  if (find_pc_partial_function (fi->pc, NULL, &func_addr, NULL))
    {
      sal = find_pc_line (func_addr, 0);

      if (sal.line == 0)
	prologue_end = fi->pc;
      else
	prologue_end = sal.end;
    }
  else
    prologue_end = func_addr + 100; /* We're in the boondocks */

  prologue_end = min (prologue_end, fi->pc);

  /* Now, search the prologue looking for instructions that setup fp, save
     rp, adjust sp and such. */ 

  framereg = SP_REGNUM;
  frameoffset = 0;
  memset (fi->fsr.regs, '\000', sizeof fi->fsr.regs);

  for (current_pc = func_addr; current_pc < prologue_end; current_pc += 2)
    {
      int insn;

      insn = read_memory_integer (current_pc, 2);

      if ((insn & 0xffe0) == ((SP_REGNUM << 11) | 0x0240)) /* add <imm>,sp */
	frameoffset = (insn & 0x1f) | ~0x1f;
      else if (insn == ((SP_REGNUM << 11) | 0x0600 | SP_REGNUM)) /* addi <imm>,sp,sp */
	{
	  current_pc += 2;

	  frameoffset = read_memory_integer (current_pc, 2);
	}
      else if (insn == ((FP_REGNUM << 11) | 0x0000 | 12)) /* mov r12,r2 */
	framereg = FP_REGNUM;	/* Setting up fp */
      else if ((insn & 0x07ff) == (0x0760 | SP_REGNUM))	/* st.w <reg>,<offset>[sp] */
	{
	  reg = (insn >> 11) & 0x1f;	/* Extract <reg> */
	  current_pc += 2;

	  insn = read_memory_integer (current_pc, 2) & ~1;

	  fi->fsr.regs[reg] = insn + frameoffset;
	}
      else if ((insn & 0x07ff) == (0x0760 | FP_REGNUM))	/* st.w <reg>,<offset>[fp] */
	{
	  reg = (insn >> 11) & 0x1f;	/* Extract <reg> */
	  current_pc += 2;

	  insn = read_memory_integer (current_pc, 2) & ~1;

	  fi->fsr.regs[reg] = insn;
	}
    }

  if (PC_IN_CALL_DUMMY (fi->pc, NULL, NULL))
    fi->frame = dummy_frame_stack->sp;
  else if (!fi->next && framereg == SP_REGNUM)
    fi->frame = read_register (framereg) - frameoffset;

  for (reg = 0; reg < NUM_REGS; reg++)
    if (fi->fsr.regs[reg] != 0)
      fi->fsr.regs[reg] += fi->frame;
}

/* Find the caller of this frame.  We do this by seeing if RP_REGNUM is saved
   in the stack anywhere, otherwise we get it from the registers. */

CORE_ADDR
v850_find_callers_reg (fi, regnum)
     struct frame_info *fi;
     int regnum;
{
  /* XXX - Won't work if multiple dummy frames are active */
  if (PC_IN_CALL_DUMMY (fi->pc, NULL, NULL))
    switch (regnum)
      {
      case SP_REGNUM:
	return dummy_frame_stack->sp;
	break;
      case FP_REGNUM:
	return dummy_frame_stack->fp;
	break;
      case RP_REGNUM:
	return dummy_frame_stack->pc;
	break;
      case PC_REGNUM:
	return dummy_frame_stack->pc;
	break;
      }

  for (; fi; fi = fi->next)
    if (fi->fsr.regs[regnum] != 0)
      return read_memory_integer (fi->fsr.regs[regnum], 4);

  return read_register (regnum);
}

CORE_ADDR
v850_frame_chain (fi)
     struct frame_info *fi;
{
  CORE_ADDR callers_pc, callers_sp;
  CORE_ADDR func_addr, prologue_end, current_pc;
  int frameoffset;

  /* First, find out who called us */

  callers_pc = FRAME_SAVED_PC (fi);

  if (PC_IN_CALL_DUMMY (callers_pc, NULL, NULL))
    return dummy_frame_stack->sp; /* XXX Won't work if multiple dummy frames on stack! */

  /* Next, figure out where his prologue is.  */

  if (find_pc_partial_function (callers_pc, NULL, &func_addr, NULL))
    {
      struct symtab_and_line sal;

      /* Stop when the caller is the entry point function */
      if (func_addr == entry_point_address ())
	return 0;

      sal = find_pc_line (func_addr, 0);

      if (sal.line == 0)
	prologue_end = callers_pc;
      else
	prologue_end = sal.end;
    }
  else
    prologue_end = func_addr + 100; /* We're in the boondocks */

  prologue_end = min (prologue_end, callers_pc);

  /* Now, figure out the frame location of the caller by examining his prologue.
     We're looking for either a load of the frame pointer register, or a stack
     adjustment. */

  frameoffset = 0;

  for (current_pc = func_addr; current_pc < prologue_end; current_pc += 2)
    {
      int insn;

      insn = read_memory_integer (current_pc, 2);

      if ((insn & 0xffe0) == ((SP_REGNUM << 11) | 0x0240)) /* add <imm>,sp */
	frameoffset = (insn & 0x1f) | ~0x1f;
      else if (insn == ((SP_REGNUM << 11) | 0x0600 | SP_REGNUM)) /* addi <imm>,sp,sp */
	{
	  current_pc += 2;

	  frameoffset = read_memory_integer (current_pc, 2);
	}
      else if (insn == ((FP_REGNUM << 11) | 0x0000 | 12)) /* mov r12,r2 */
	return v850_find_callers_reg (fi, FP_REGNUM); /* It's using a frame pointer reg */
      else if ((insn & 0x07ff) == (0x0760 | SP_REGNUM))	/* st.w <reg>,<offset>[sp] */
	current_pc += 2;
      else if ((insn & 0x07ff) == (0x0760 | FP_REGNUM))	/* st.w <reg>,<offset>[fp] */
	current_pc += 2;
    }

  return fi->frame - frameoffset;
}

CORE_ADDR
v850_skip_prologue (pc)
     CORE_ADDR pc;
{
  CORE_ADDR func_addr, func_end;

  /* See what the symbol table says */

  if (find_pc_partial_function (pc, NULL, &func_addr, &func_end))
    {
      struct symtab_and_line sal;

      sal = find_pc_line (func_addr, 0);

      if (sal.line != 0 && sal.end < func_end)
	return sal.end;
      else
	/* Either there's no line info, or the line after the prologue is after
	   the end of the function.  In this case, there probably isn't a
	   prologue.  */
	return pc;
    }

/* We can't find the start of this function, so there's nothing we can do. */
  return pc;
}

/* All we do here is record SP and FP on the call dummy stack */

void
v850_push_dummy_frame ()
{
  struct dummy_frame *dummy_frame;

  dummy_frame = xmalloc (sizeof (struct dummy_frame));

  dummy_frame->fp = read_register (FP_REGNUM);
  dummy_frame->sp = read_register (SP_REGNUM);
  dummy_frame->rp = read_register (RP_REGNUM);
  dummy_frame->pc = read_register (PC_REGNUM);
  dummy_frame->next = dummy_frame_stack;
  dummy_frame_stack = dummy_frame;
}

int
v850_pc_in_call_dummy (pc)
     CORE_ADDR pc;
{
  return dummy_frame_stack
	 && pc >= CALL_DUMMY_ADDRESS ()
	 && pc <= CALL_DUMMY_ADDRESS () + DECR_PC_AFTER_BREAK;
}

struct frame_info *
v850_pop_frame (frame)
     struct frame_info *frame;
{
  int regnum;

  if (PC_IN_CALL_DUMMY (frame->pc, NULL, NULL))
    {
      struct dummy_frame *dummy_frame;
      
      dummy_frame = dummy_frame_stack;
      if (!dummy_frame)
	error ("Can't pop dummy frame!");

      dummy_frame_stack = dummy_frame->next;

      write_register (FP_REGNUM, dummy_frame->fp);
      write_register (SP_REGNUM, dummy_frame->sp);
      write_register (RP_REGNUM, dummy_frame->rp);
      write_register (PC_REGNUM, dummy_frame->pc);

      free (dummy_frame);

      flush_cached_frames ();

      return NULL;
    }

  write_register (PC_REGNUM, FRAME_SAVED_PC (frame));

  for (regnum = 0; regnum < NUM_REGS; regnum++)
    if (frame->fsr.regs[regnum] != 0)
      write_register (regnum, read_memory_integer (frame->fsr.regs[regnum], 4));

  write_register (SP_REGNUM, FRAME_FP (frame));
  flush_cached_frames ();

  return NULL;
}

/* Put arguments in the right places, and setup return address register (RP) to
   point at a convenient place to put a breakpoint.  First four args go in
   R6->R9, subsequent args go into sp + 16 -> sp + ...  Structs are passed by
   reference.  64 bit quantities (doubles and long longs) may be split between
   the regs and the stack.  When calling a function that returns a struct, a
   pointer to the struct is passed in as a secret first argument (always in R6).

   By the time we get here, stack space has been allocated for the args, but
   not for the struct return pointer.  */

CORE_ADDR
v850_push_arguments (nargs, args, sp, struct_return, struct_addr)
     int nargs;
     value_ptr *args;
     CORE_ADDR sp;
     unsigned char struct_return;
     CORE_ADDR struct_addr;
{
  int argreg;
  int argnum;

  argreg = ARG0_REGNUM;

  if (struct_return)
    {
      write_register (argreg++, struct_addr);
      sp -= 4;
    }

  for (argnum = 0; argnum < nargs; argnum++)
    {
      int len;
      char *val;
      char valbuf[4];

      if (TYPE_CODE (VALUE_TYPE (*args)) == TYPE_CODE_STRUCT
	  && TYPE_LENGTH (VALUE_TYPE (*args)) > 8)
	{
	  store_address (valbuf, 4, VALUE_ADDRESS (*args));
	  len = 4;
	  val = valbuf;
	}
      else
	{
	  len = TYPE_LENGTH (VALUE_TYPE (*args));
	  val = (char *)VALUE_CONTENTS (*args);
	}

      while (len > 0)
	if  (argreg <= ARGLAST_REGNUM)
	  {
	    CORE_ADDR regval;

	    regval = extract_address (val, REGISTER_RAW_SIZE (argreg));
	    write_register (argreg, regval);

	    len -= REGISTER_RAW_SIZE (argreg);
	    val += REGISTER_RAW_SIZE (argreg);
	    argreg++;
	  }
	else
	  {
	    write_memory (sp + argnum * 4, val, 4);

	    len -= 4;
	    val += 4;
	  }
      args++;
    }

  write_register (RP_REGNUM, entry_point_address ());

  return sp;
}

void
_initialize_sparc_tdep ()
{
  tm_print_insn = print_insn_v850;
}
