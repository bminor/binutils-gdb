/* Target-dependent code for the Mitsubishi m32r for GDB, the GNU debugger.
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
#include "symfile.h"

struct dummy_frame
{
  struct dummy_frame *next;

  CORE_ADDR fp;
  CORE_ADDR sp;
  CORE_ADDR rp;
  CORE_ADDR pc;
};

void 
m32r_frame_find_saved_regs PARAMS ((struct frame_info *fi, 
				    struct frame_saved_regs *regaddr))
{
  *regaddr = fi->fsr;
}

static struct dummy_frame *dummy_frame_stack = NULL;

/* Find end of function prologue */

CORE_ADDR
m32r_skip_prologue (pc)
     CORE_ADDR pc;
{
  CORE_ADDR func_addr, func_end;
  struct symtab_and_line sal;

  /* See what the symbol table says */

  if (find_pc_partial_function (pc, NULL, &func_addr, &func_end))
    {
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

/* This function decodes the target function prologue to determine
   1) the size of the stack frame, and 2) which registers are saved on it.
   It saves the offsets of saved regs in the frame_saved_regs argument,
   and returns the frame size.
*/

static unsigned long
m32r_scan_prologue (fi, fsr)
     struct frame_info *fi;
     struct frame_saved_regs *fsr;
{
  struct symtab_and_line sal;
  CORE_ADDR prologue_start, prologue_end, current_pc;
  unsigned long framesize;

  /* this code essentially duplicates skip_prologue, 
     but we need the start address below.  */

  if (find_pc_partial_function (fi->pc, NULL, &prologue_start, &prologue_end))
    {
      sal = find_pc_line (prologue_start, 0);

      if (sal.line == 0)		/* no line info, use current PC */
	if (prologue_start != entry_point_address ())
	  prologue_end = fi->pc;
	else
	  return 0;			/* _start has no frame or prologue */
      else if (sal.end < prologue_end)	/* next line begins after fn end */
	prologue_end = sal.end;		/* (probably means no prologue)  */
    }
  else
    prologue_end = prologue_start + 40; /* We're in the boondocks: allow for */
					/* 16 pushes, an add, and "mv fp,sp" */

  prologue_end = min (prologue_end, fi->pc);

  /* Now, search the prologue looking for instructions that setup fp, save
     rp (and other regs), adjust sp and such. */ 

  framesize = 0;
  for (current_pc = prologue_start; current_pc < prologue_end; current_pc += 2)
    {
      int insn;
      int regno;

      insn = read_memory_unsigned_integer (current_pc, 2);
      if (insn & 0x8000)			/* Four byte instruction? */
	current_pc += 2;

      if ((insn & 0xf0ff) == 0x207f) {		/* st reg, @-sp */
	framesize += 4;
	regno = ((insn >> 8) & 0xf);
	if (fsr)				/* save_regs offset */
	  fsr->regs[regno] = framesize;
      }
      else if ((insn >> 8) == 0x4f)  		/* addi sp, xx */
	/* add 8 bit sign-extended offset */
	framesize += -((char) (insn & 0xff));
      else if (insn == 0x8faf)			/* add3 sp, sp, xxxx */
	/* add 16 bit sign-extended offset */
	framesize += -((short) read_memory_unsigned_integer (current_pc, 2));
      else if (((insn >> 8) == 0xe4) &&	    /* ld24 r4, xxxxxx ;  sub sp, r4 */
	       read_memory_unsigned_integer (current_pc + 2, 2) == 0x0f24)
	{ /* subtract 24 bit sign-extended negative-offset */
	  insn = read_memory_unsigned_integer (current_pc - 2, 4);
	  if (insn & 0x00800000)	/* sign extend */
	    insn  |= 0xff000000;	/* negative */
	  else
	    insn  &= 0x00ffffff;	/* positive */
	  framesize += insn;
	}
      else if (insn == 0x1d8f) {	/* mv fp, sp */
	fi->using_frame_pointer = 1;	/* fp is now valid */
	break;				/* end of stack adjustments */
      }
      else
	break;				/* anything else isn't prologue */
    }
  return framesize;
}

/* This function actually figures out the frame address for a given pc and
   sp.  This is tricky on the m32r because we sometimes don't use an explicit
   frame pointer, and the previous stack pointer isn't necessarily recorded
   on the stack.  The only reliable way to get this info is to
   examine the prologue.
*/

void
m32r_init_extra_frame_info (fi)
     struct frame_info *fi;
{
  int reg;

  if (fi->next)
    fi->pc = FRAME_SAVED_PC (fi->next);

  memset (fi->fsr.regs, '\000', sizeof fi->fsr.regs);
  fi->using_frame_pointer = 0;
  fi->framesize = m32r_scan_prologue (fi, &fi->fsr);
#if 0
  if (PC_IN_CALL_DUMMY (fi->pc, NULL, NULL))
    fi->frame = dummy_frame_stack->sp;
  else 
#endif
    if (!fi->next)
      if (fi->using_frame_pointer)
	fi->frame = read_register (FP_REGNUM);
      else
	fi->frame = read_register (SP_REGNUM);
    else 	/* fi->next means this is not the innermost frame */
      if (fi->using_frame_pointer)		/* we have an FP */
	if (fi->next->fsr.regs[FP_REGNUM] != 0)	/* caller saved our FP */
	  fi->frame = read_memory_integer (fi->next->fsr.regs[FP_REGNUM], 4);

  for (reg = 0; reg < NUM_REGS; reg++)
    if (fi->fsr.regs[reg] != 0)
      fi->fsr.regs[reg] = fi->frame + fi->framesize - fi->fsr.regs[reg];
}

/* Find the caller of this frame.  We do this by seeing if RP_REGNUM is saved
   in the stack anywhere, otherwise we get it from the registers. */

CORE_ADDR
m32r_find_callers_reg (fi, regnum)
     struct frame_info *fi;
     int regnum;
{
#if 0
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

#endif
  for (; fi; fi = fi->next)
    if (fi->fsr.regs[regnum] != 0)
      return read_memory_integer (fi->fsr.regs[regnum], 4);
  return read_register (regnum);
}

/* Given a GDB frame, determine the address of the calling function's frame.
   This will be used to create a new GDB frame struct, and then
   INIT_EXTRA_FRAME_INFO and INIT_FRAME_PC will be called for the new frame.
   For m32r, we save the frame size when we initialize the frame_info.
 */

CORE_ADDR
m32r_frame_chain (fi)
     struct frame_info *fi;
{
  CORE_ADDR fn_start;

  if (find_pc_partial_function (fi->pc, 0, &fn_start, 0))
    if (fn_start == entry_point_address ())
      return 0;		/* in _start fn, don't chain further */
  return fi->frame + fi->framesize;
}

/* All we do here is record SP and FP on the call dummy stack */

void
m32r_push_dummy_frame ()
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

/*
 * MISSING FUNCTION HEADER COMMENT
 */

int
m32r_pc_in_call_dummy (pc)
     CORE_ADDR pc;
{
#if 0
  return dummy_frame_stack
	 && pc >= CALL_DUMMY_ADDRESS ()
	 && pc <= CALL_DUMMY_ADDRESS () + DECR_PC_AFTER_BREAK;
#else
  return 0;
#endif
}

/* Discard from the stack the innermost frame,
   restoring all saved registers.  */

struct frame_info *
m32r_pop_frame (frame)
     struct frame_info *frame;
{
  int regnum;

#if 0
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

#endif
  write_register (PC_REGNUM, FRAME_SAVED_PC (frame));

  for (regnum = 0; regnum < NUM_REGS; regnum++)
    if (frame->fsr.regs[regnum] != 0)
      write_register (regnum, 
		      read_memory_integer (frame->fsr.regs[regnum], 4));

  write_register (SP_REGNUM, read_register (FP_REGNUM));
  if (read_register (PSW_REGNUM) & 0x80)
    write_register (SPU_REGNUM, read_register (SP_REGNUM));
  else
    write_register (SPI_REGNUM, read_register (SP_REGNUM));
  /*  registers_changed (); */
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
m32r_push_arguments (nargs, args, sp, struct_return, struct_addr)
     int nargs;
     value_ptr *args;
     CORE_ADDR sp;
     unsigned char struct_return;
     CORE_ADDR struct_addr;
{
  int argreg;
  int argnum;

  argreg = ARG0_REGNUM;

#if 0
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

#endif
  return sp;
}

void
_initialize_m32r_tdep ()
{
  tm_print_insn = print_insn_m32r;
}
