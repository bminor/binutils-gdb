/* Target-dependent code for the Matsushita MN10200 for GDB, the GNU debugger.
   Copyright 1997 Free Software Foundation, Inc.

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

/* Info gleaned from scanning a function's prologue.  */

struct pifsr			/* Info about one saved reg */
{
  int framereg;			/* Frame reg (SP or FP) */
  int offset;			/* Offset from framereg */
  int reg;			/* Saved register number */
};

struct prologue_info
{
  int framereg;
  int frameoffset;
  int start_function;
  struct pifsr *pifsrs;
};

/* Function: frame_chain
   Figure out and return the caller's frame pointer given current
   frame_info struct.

   We start out knowing the current pc, current sp, current fp.
   We want to determine the caller's fp and caller's pc.  To do this
   correctly, we have to be able to handle the case where we are in the
   middle of the prologue which involves scanning the prologue.

   We don't handle dummy frames yet but we would probably just return the
   stack pointer that was in use at the time the function call was made?
*/

CORE_ADDR
mn10200_frame_chain (fi)
     struct frame_info *fi;
{
  struct prologue_info pi;
  CORE_ADDR callers_pc, callers_fp, curr_sp;
  CORE_ADDR past_prologue_addr;
  int past_prologue = 1; /* default to being past prologue */
  int n_movm_args = 4;

  struct pifsr *pifsr, *pifsr_tmp;

  /* current pc is fi->pc */
  /* current fp is fi->frame */  
  /* current sp is: */
  curr_sp = read_register (SP_REGNUM);

/*
  printf("curr pc = 0x%x ; curr fp = 0x%x ; curr sp = 0x%x\n",
	 fi->pc, fi->frame, curr_sp);
*/

  /* first inst after prologue is: */
  past_prologue_addr = mn10200_skip_prologue (fi->pc);

  /* Are we in the prologue? */
  /* Yes if mn10200_skip_prologue returns an address after the
     current pc in which case we have to scan prologue */
  if (fi->pc < mn10200_skip_prologue (fi->pc))
      past_prologue = 0;

  /* scan prologue if we're not past it */
  if (!past_prologue)
    {
	/* printf("scanning prologue\n"); */
	/* FIXME -- fill out this case later */
        return 0x0; /* bogus value */
    }

  if (past_prologue) /* if we don't need to scan the prologue */
    {
      callers_pc = fi->frame - REGISTER_SIZE;
      callers_fp = fi->frame - (4 * REGISTER_SIZE);

#if 0
      printf("callers_pc = 0x%x ; callers_fp = 0x%x\n",
	     callers_pc, callers_fp);
      printf("*callers_pc = 0x%x ; *callers_fp = 0x%x\n",
	     read_memory_integer(callers_pc, REGISTER_SIZE),
	     read_memory_integer(callers_fp, REGISTER_SIZE));
#endif

      return read_memory_integer(callers_fp, REGISTER_SIZE);
    }

  /* we don't get here */
}

/* Function: find_callers_reg
   Find REGNUM on the stack.  Otherwise, it's in an active register.
   One thing we might want to do here is to check REGNUM against the
   clobber mask, and somehow flag it as invalid if it isn't saved on
   the stack somewhere.  This would provide a graceful failure mode
   when trying to get the value of caller-saves registers for an inner
   frame.  */

CORE_ADDR
mn10200_find_callers_reg (fi, regnum)
     struct frame_info *fi;
     int regnum;
{
/*  printf("mn10200_find_callers_reg\n"); */

  for (; fi; fi = fi->next)
    if (PC_IN_CALL_DUMMY (fi->pc, fi->frame, fi->frame))
      return generic_read_register_dummy (fi->pc, fi->frame, regnum);
    else if (fi->fsr.regs[regnum] != 0)
      return read_memory_unsigned_integer (fi->fsr.regs[regnum], 
					   REGISTER_RAW_SIZE(regnum));

  return read_register (regnum);
}

/* Function: skip_prologue
   Return the address of the first inst past the prologue of the function.
*/

CORE_ADDR
mn10200_skip_prologue (pc)
     CORE_ADDR pc;
{
  CORE_ADDR func_addr, func_end;

/*  printf("mn10200_skip_prologue\n"); */

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

/* Function: pop_frame
   This routine gets called when either the user uses the `return'
   command, or the call dummy breakpoint gets hit.  */

void
mn10200_pop_frame (frame)
     struct frame_info *frame;
{
  int regnum;

/*  printf("mn10200_pop_frame start\n"); */

  if (PC_IN_CALL_DUMMY(frame->pc, frame->frame, frame->frame))
    generic_pop_dummy_frame ();
  else
    {
      write_register (PC_REGNUM, FRAME_SAVED_PC (frame));

      for (regnum = 0; regnum < NUM_REGS; regnum++)
	if (frame->fsr.regs[regnum] != 0)
	  write_register (regnum,
			  read_memory_unsigned_integer (frame->fsr.regs[regnum],
							REGISTER_RAW_SIZE(regnum)));

      write_register (SP_REGNUM, FRAME_FP (frame));
    }

  flush_cached_frames ();

/*  printf("mn10200_pop_frame end\n"); */
}

/* Function: push_arguments
   Setup arguments for a call to the target.  Arguments go in
   order on the stack.
*/

CORE_ADDR
mn10200_push_arguments (nargs, args, sp, struct_return, struct_addr)
     int nargs;
     value_ptr *args;
     CORE_ADDR sp;
     unsigned char struct_return;
     CORE_ADDR struct_addr;
{
  int argnum = 0;
  int len = 0;
  int stack_offset = 0;  /* copy args to this offset onto stack */

/*  printf("mn10200_push_arguments start\n"); */

  /* First, just for safety, make sure stack is aligned */
  sp &= ~3;

  /* Now make space on the stack for the args. */
  for (argnum = 0; argnum < nargs; argnum++)
    len += ((TYPE_LENGTH(VALUE_TYPE(args[argnum])) + 3) & ~3);

  sp -= len;

  /* Push all arguments onto the stack. */
  for (argnum = 0; argnum < nargs; argnum++)
    {
      int len;
      char *val;

      if (TYPE_CODE (VALUE_TYPE (*args)) == TYPE_CODE_STRUCT
	  && TYPE_LENGTH (VALUE_TYPE (*args)) > 8)
	{
	  /* for now, pretend structs aren't special */
          len = TYPE_LENGTH (VALUE_TYPE (*args));
          val = (char *)VALUE_CONTENTS (*args);
	}
      else
	{
	  len = TYPE_LENGTH (VALUE_TYPE (*args));
	  val = (char *)VALUE_CONTENTS (*args);
	}

      while (len > 0)
	{
	  write_memory (sp + stack_offset, val, 4);

	  len -= 4;
	  val += 4;
	  stack_offset += 4;
	}
      args++;
    }

/*  printf"mn10200_push_arguments end\n"); */

  return sp;
}

/* Function: push_return_address (pc)
   Set up the return address for the inferior function call.
   Needed for targets where we don't actually execute a JSR/BSR instruction */
 
CORE_ADDR
mn10200_push_return_address (pc, sp)
     CORE_ADDR pc;
     CORE_ADDR sp;
{
/*  printf("mn10200_push_return_address\n"); */

  /* write_register (RP_REGNUM, CALL_DUMMY_ADDRESS ()); */
  return sp;
}
 
/* Function: frame_saved_pc 
   Find the caller of this frame.  We do this by seeing if RP_REGNUM
   is saved in the stack anywhere, otherwise we get it from the
   registers.  If the inner frame is a dummy frame, return its PC
   instead of RP, because that's where "caller" of the dummy-frame
   will be found.  */

CORE_ADDR
mn10200_frame_saved_pc (fi)
     struct frame_info *fi;
{
/*  printf("mn10200_frame_saved_pc\n"); */

  return (read_memory_integer(fi->frame - REGISTER_SIZE, REGISTER_SIZE));
}

void
get_saved_register (raw_buffer, optimized, addrp, frame, regnum, lval)
     char *raw_buffer;
     int *optimized;
     CORE_ADDR *addrp;
     struct frame_info *frame;
     int regnum;
     enum lval_type *lval;
{
/*  printf("get_saved_register\n"); */

  generic_get_saved_register (raw_buffer, optimized, addrp, 
			      frame, regnum, lval);
}

/* Function: init_extra_frame_info
   Setup the frame's frame pointer, pc, and frame addresses for saved
   registers.  Most of the work is done in frame_chain().

   Note that when we are called for the last frame (currently active frame),
   that fi->pc and fi->frame will already be setup.  However, fi->frame will
   be valid only if this routine uses FP.  For previous frames, fi-frame will
   always be correct (since that is derived from v850_frame_chain ()).

   We can be called with the PC in the call dummy under two circumstances.
   First, during normal backtracing, second, while figuring out the frame
   pointer just prior to calling the target function (see run_stack_dummy).
*/

void
mn10200_init_extra_frame_info (fi)
     struct frame_info *fi;
{
  struct prologue_info pi;
  struct pifsr pifsrs[NUM_REGS + 1], *pifsr;
  int reg;

  if (fi->next)
    fi->pc = FRAME_SAVED_PC (fi->next);

  memset (fi->fsr.regs, '\000', sizeof fi->fsr.regs);

  /* The call dummy doesn't save any registers on the stack, so we can return
     now.  */
/*
  if (PC_IN_CALL_DUMMY (fi->pc, fi->frame, fi->frame))
      return;

  pi.pifsrs = pifsrs;
*/

  /* v850_scan_prologue (fi->pc, &pi); */
/*
  if (!fi->next && pi.framereg == SP_REGNUM)
    fi->frame = read_register (pi.framereg) - pi.frameoffset;

  for (pifsr = pifsrs; pifsr->framereg; pifsr++)
    {
      fi->fsr.regs[pifsr->reg] = pifsr->offset + fi->frame;

      if (pifsr->framereg == SP_REGNUM)
	fi->fsr.regs[pifsr->reg] += pi.frameoffset;
    }
*/
/*   printf("init_extra_frame_info\n"); */
}

void
_initialize_mn10200_tdep ()
{
/*  printf("_initialize_mn10200_tdep\n"); */

  tm_print_insn = print_insn_mn10200;
}

