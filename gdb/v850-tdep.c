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
#include "symfile.h"

/* Dummy frame.  This saves the processor state just prior to setting up the
   inferior function call.  On most targets, the registers are saved on the
   target stack, but that really slows down function calls.  */

struct dummy_frame
{
  struct dummy_frame *next;

  char regs[REGISTER_BYTES];
};

static struct dummy_frame *dummy_frame_stack = NULL;

static CORE_ADDR read_register_dummy PARAMS ((int regno));

/* Info gleaned from scanning a function's prologue.  */

struct prologue_info
{
  int framereg;
  int frameoffset;
  int start_function;
  struct frame_saved_regs *fsr;
};

static CORE_ADDR scan_prologue PARAMS ((CORE_ADDR pc, struct prologue_info *fs));

/* Scan the prologue of the function that contains PC, and record what we find
   in PI.  PI->fsr must be zeroed by the called.  Returns the pc after the
   prologue.  Note that the addresses saved in pi->fsr are actually just frame
   relative (negative offsets from the frame pointer).  This is because we
   don't know the actual value of the frame pointer yet.  In some
   circumstances, the frame pointer can't be determined till after we have
   scanned the prologue.  */

static CORE_ADDR
scan_prologue (pc, pi)
     CORE_ADDR pc;
     struct prologue_info *pi;
{
  CORE_ADDR func_addr, prologue_end, current_pc;
  int fp_used;

  /* First, figure out the bounds of the prologue so that we can limit the
     search to something reasonable.  */

  if (find_pc_partial_function (pc, NULL, &func_addr, NULL))
    {
      struct symtab_and_line sal;

      sal = find_pc_line (func_addr, 0);

      if (func_addr == entry_point_address ())
	pi->start_function = 1;
      else
	pi->start_function = 0;

      if (sal.line == 0)
	prologue_end = pc;
      else
	prologue_end = sal.end;
    }
  else
    {				/* We're in the boondocks */
      func_addr = pc - 100;
      prologue_end = pc;
    }

  prologue_end = min (prologue_end, pc);

  /* Now, search the prologue looking for instructions that setup fp, save
     rp, adjust sp and such.  We also record the frame offset of any saved
     registers. */ 

  pi->frameoffset = 0;
  pi->framereg = SP_REGNUM;
  fp_used = 0;

  for (current_pc = func_addr; current_pc < prologue_end; current_pc += 2)
    {
      int insn;

      insn = read_memory_unsigned_integer (current_pc, 2);

      if ((insn & 0xffe0) == ((SP_REGNUM << 11) | 0x0240)) /* add <imm>,sp */
	pi->frameoffset = ((insn & 0x1f) ^ 0x10) - 0x10;
      else if (insn == ((SP_REGNUM << 11) | 0x0600 | SP_REGNUM)) /* addi <imm>,sp,sp */
	pi->frameoffset = read_memory_integer (current_pc + 2, 2);
      else if (insn == ((FP_REGNUM << 11) | 0x0000 | 12)) /* mov r12,fp */
	{
	  fp_used = 1;
	  pi->framereg = FP_REGNUM;
	}
      else if ((insn & 0x07ff) == (0x0760 | SP_REGNUM)	/* st.w <reg>,<offset>[sp] */
	       || (fp_used
		   && (insn & 0x07ff) == (0x0760 | FP_REGNUM))) /* st.w <reg>,<offset>[fp] */
	if (pi->fsr)
	  {
	    int framereg;
	    int reg;
	    int offset;

	    framereg = insn & 0x1f;
	    reg = (insn >> 11) & 0x1f; /* Extract <reg> */

	    offset = read_memory_integer (current_pc + 2, 2) & ~1;

	    if (framereg == SP_REGNUM) /* Using SP? */
	      offset += pi->frameoffset; /* Yes, correct for frame size */

	    pi->fsr->regs[reg] = offset;
	  }

      if ((insn & 0x0780) >= 0x0600) /* Four byte instruction? */
	current_pc += 2;
    }

  return current_pc;
}

/* Setup the frame frame pointer, pc, and frame addresses for saved registers.
   Most of the work is done in scan_prologue().

   Note that when we are called for the last frame (currently active frame),
   that fi->pc and fi->frame will already be setup.  However, fi->frame will
   be valid only if this routine uses FP.  For previous frames, fi-frame will
   always be correct (since that is derived from v850_frame_chain ()).

   We can be called with the PC in the call dummy under two circumstances.
   First, during normal backtracing, second, while figuring out the frame
   pointer just prior to calling the target function (see run_stack_dummy).
   */

void
v850_init_extra_frame_info (fi)
     struct frame_info *fi;
{
  struct prologue_info pi;
  int reg;

  if (fi->next)
    fi->pc = FRAME_SAVED_PC (fi->next);

  memset (fi->fsr.regs, '\000', sizeof fi->fsr.regs);

  /* The call dummy doesn't save any registers on the stack, so we can return
     now.  */
  if (PC_IN_CALL_DUMMY (fi->pc, NULL, NULL))
    {
      /* We need to setup fi->frame here because run_stack_dummy gets it wrong
	 by assuming it's always FP.  */
      fi->frame = read_register_dummy (SP_REGNUM);
      return;
    }

  pi.fsr = &fi->fsr;

  scan_prologue (fi->pc, &pi);

 if (!fi->next && pi.framereg == SP_REGNUM)
   fi->frame = read_register (pi.framereg) - pi.frameoffset;

  for (reg = 0; reg < NUM_REGS; reg++)
    if (fi->fsr.regs[reg] != 0)
      fi->fsr.regs[reg] += fi->frame;
}

/* Figure out the frame prior to FI.  Unfortunately, this involves scanning the
   prologue of the caller, which will also be done shortly by
   v850_init_extra_frame_info.  For the dummy frame, we just return the stack
   pointer that was in use at the time the function call was made.  */

CORE_ADDR
v850_frame_chain (fi)
     struct frame_info *fi;
{
  CORE_ADDR callers_pc;
  struct prologue_info pi;

  /* First, find out who called us */

  callers_pc = FRAME_SAVED_PC (fi);

  if (PC_IN_CALL_DUMMY (callers_pc, NULL, NULL))
    return read_register_dummy (SP_REGNUM); /* XXX Won't work if multiple dummy frames on stack! */

  pi.fsr = NULL;

  scan_prologue (callers_pc, &pi);

  if (pi.start_function)
    return 0;			/* Don't chain beyond the start function */

  if (pi.framereg == FP_REGNUM)
    return v850_find_callers_reg (fi, pi.framereg);

  return fi->frame - pi.frameoffset;
}

/* Find REGNUM on the stack.  Otherwise, it's in an active register.  One thing
   we might want to do here is to check REGNUM against the clobber mask, and
   somehow flag it as invalid if it isn't saved on the stack somewhere.  This
   would provide a graceful failure mode when trying to get the value of
   caller-saves registers for an inner frame.  */

CORE_ADDR
v850_find_callers_reg (fi, regnum)
     struct frame_info *fi;
     int regnum;
{
  /* XXX - Won't work if multiple dummy frames are active */
  /* When the caller requests RP from the dummy frame, we return PC because
     that's where the previous routine appears to have done a call from. */
  if (PC_IN_CALL_DUMMY (fi->pc, NULL, NULL))
    if (regnum == RP_REGNUM)
      regnum = PC_REGNUM;

  for (; fi; fi = fi->next)
    if (PC_IN_CALL_DUMMY (fi->pc, NULL, NULL))
      return read_register_dummy (regnum);
    else if (fi->fsr.regs[regnum] != 0)
      return read_memory_integer (fi->fsr.regs[regnum], 4);

  return read_register (regnum);
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

/* Save all the registers on the dummy frame stack.  Most ports save the
   registers on the target stack.  This results in lots of unnecessary memory
   references, which are slow when debugging via a serial line.  Instead, we
   save all the registers internally, and never write them to the stack.  The
   registers get restored when the called function returns to the entry point,
   where a breakpoint is laying in wait.  */

void
v850_push_dummy_frame ()
{
  struct dummy_frame *dummy_frame;

  dummy_frame = xmalloc (sizeof (struct dummy_frame));

  read_register_bytes (0, dummy_frame->regs, REGISTER_BYTES);

  dummy_frame->next = dummy_frame_stack;
  dummy_frame_stack = dummy_frame;
}

/* Read registers from the topmost dummy frame.  */

CORE_ADDR
read_register_dummy (regno)
     int regno;
{
  return extract_address (&dummy_frame_stack->regs[REGISTER_BYTE (regno)],
			  REGISTER_RAW_SIZE(regno));
}

int
v850_pc_in_call_dummy (pc)
     CORE_ADDR pc;
{
  return dummy_frame_stack
	 && pc >= CALL_DUMMY_ADDRESS ()
	 && pc <= CALL_DUMMY_ADDRESS () + DECR_PC_AFTER_BREAK;
}

/* This routine gets called when either the user uses the `return' command, or
   the call dummy breakpoint gets hit.  */

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

      write_register_bytes (0, dummy_frame->regs, REGISTER_BYTES);

      free (dummy_frame);
    }
  else
    {
      write_register (PC_REGNUM, FRAME_SAVED_PC (frame));

      for (regnum = 0; regnum < NUM_REGS; regnum++)
	if (frame->fsr.regs[regnum] != 0)
	  write_register (regnum,
			  read_memory_integer (frame->fsr.regs[regnum], 4));

      write_register (SP_REGNUM, FRAME_FP (frame));
    }

  flush_cached_frames ();

  return NULL;
}

/* Setup arguments and RP for a call to the target.  First four args go in
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
