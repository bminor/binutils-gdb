/* Target-dependent code for the NEC V850 for GDB, the GNU debugger.
   Copyright 1986, 1996
   Free Software Foundation, Inc.

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

/* ??? Support for calling functions from gdb in sparc64 is unfinished.  */

#include "defs.h"
#include "frame.h"
#include "inferior.h"
#include "obstack.h"
#include "target.h"
#include "value.h"
#include "bfd.h"
#include "gdb_string.h"

#include "gdbcore.h"


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
    fi->pc = v850_find_callers_reg (fi->next, RP_REGNUM);

  /* First, figure out the bounds of the prologue so that we can limit the
     search to something reasonable.  */

  if (find_pc_partial_function (fi->pc, NULL, &func_addr, NULL))
    {
      sal = find_pc_line (func_addr, 0);

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

#if 0
  if (!fi->next)
    fi->frame = read_register (framereg);
  else
    if (framereg == SP_REGNUM)
      fi->frame = fi->next->frame;
    else
      fi->frame = v850_find_callers_reg (fi, framereg);

  if (framereg == SP_REGNUM)
    fi->frame -= frameoffset;
#else
  if (!fi->next && framereg == SP_REGNUM)
    fi->frame = read_register (framereg) - frameoffset;
#endif

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
  struct frame_info temp_fi;
  CORE_ADDR func_addr, prologue_end, current_pc;
  int frameoffset;

  /* First, find out who called us */

  callers_pc = v850_find_callers_reg (fi, RP_REGNUM);

  /* Next, figure out where his prologue is.  */

  if (find_pc_partial_function (callers_pc, NULL, &func_addr, NULL))
    {
      struct symtab_and_line sal;

      sal = find_pc_line (func_addr, 0);

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

      if (sal.end < func_end)
	return sal.end;
      else
	/* The line after the prologue is after the end of the function.  In
	   this case, there probably isn't a prologue.  */
	return pc;
    }

/* We can't find the start of this function, so there's nothing we can do. */
  return pc;
}

struct frame_info *
v850_pop_frame (frame)
     struct frame_info *frame;
{
  int regnum;

  write_register (PC_REGNUM, FRAME_SAVED_PC (frame));

  for (regnum = 0; regnum < NUM_REGS; regnum++)
    if (frame->fsr.regs[regnum] != 0)
      write_register (regnum, read_memory_integer (frame->fsr.regs[regnum], 4));

  write_register (SP_REGNUM, FRAME_FP (frame));
  flush_cached_frames ();

  return NULL;
}

void
_initialize_sparc_tdep ()
{
  tm_print_insn = print_insn_v850;
}
