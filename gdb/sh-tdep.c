/* Target-machine dependent code for Hitachi Super-H, for GDB.
   Copyright (C) 1993 Free Software Foundation, Inc.

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

/*
 Contributed by Steve Chamberlain
                sac@cygnus.com
 */

#include "defs.h"
#include "frame.h"
#include "obstack.h"
#include "symtab.h"
#include "gdbtypes.h"
#include "gdbcmd.h"
#include "value.h"
#include "dis-asm.h"
#include "../opcodes/sh-opc.h"




/* Prologue looks like
   [mov.l	<regs>,@-r15]...
   [sts.l	pr,@-r15]
   [mov.l	r14,@-r15]
   [mov		r15,r14]
*/

#define IS_STS(x)  		((x) == 0x4f22)
#define IS_PUSH(x) 		(((x) & 0xff0f) == 0x2f06)
#define GET_PUSHED_REG(x)  	(((x) >> 4) & 0xf)
#define IS_MOV_SP_FP(x)  	((x) == 0x6ef3)
#define IS_ADD_SP(x) 		(((x) & 0xff00) == 0x7f00)


/* Skip any prologue before the guts of a function */

CORE_ADDR
sh_skip_prologue (start_pc)
     CORE_ADDR start_pc;

{
  int w;

  w = read_memory_integer (start_pc, 2);
  while (IS_STS (w)
	 || IS_PUSH (w)
	 || IS_MOV_SP_FP (w))
    {
      start_pc += 2;
      w = read_memory_integer (start_pc, 2);
    }

  return start_pc;
}

/* Disassemble an instruction */

int
print_insn (memaddr, stream)
     CORE_ADDR memaddr;
     GDB_FILE *stream;
{
  disassemble_info info;
  GDB_INIT_DISASSEMBLE_INFO (info, stream);
  return print_insn_sh (memaddr, &info);
}

/* Given a GDB frame, determine the address of the calling function's frame.
   This will be used to create a new GDB frame struct, and then
   INIT_EXTRA_FRAME_INFO and INIT_FRAME_PC will be called for the new frame.

   For us, the frame address is its stack pointer value, so we look up
   the function prologue to determine the caller's sp value, and return it.  */

FRAME_ADDR
sh_frame_chain (thisframe)
     FRAME thisframe;
{
  if (!inside_entry_file (thisframe->pc))
    return (read_memory_integer (FRAME_FP (thisframe), 4));
  else
    return 0;
}

/* Put here the code to store, into a struct frame_saved_regs,
   the addresses of the saved registers of frame described by FRAME_INFO.
   This includes special registers such as pc and fp saved in special
   ways in the stack frame.  sp is even more special:
   the address we return for it IS the sp for the next frame. */


void
frame_find_saved_regs (fi, fsr)
     struct frame_info *fi;
     struct frame_saved_regs *fsr;
{
  int where[NUM_REGS];
  int rn;
  int have_fp = 0;
  int depth;
  int pc;
  int opc;
  int insn;

  opc = pc = get_pc_function_start (fi->pc);

  insn = read_memory_integer (pc, 2);

  for (rn = 0; rn < NUM_REGS; rn++)
    where[rn] = -1;

  depth = 0;

  /* Loop around examining the prologue insns, but give up
     after 15 of them, since we're getting silly then */
  while (pc < opc + 15 * 2)
    {
      /* See where the registers will be saved to */
      if (IS_PUSH (insn))
	{
	  pc += 2;
	  rn = GET_PUSHED_REG (insn);
	  where[rn] = depth;
	  insn = read_memory_integer (pc, 2);
	  depth += 4;
	}
      else if (IS_STS (insn))
	{
	  pc += 2;
	  where[PR_REGNUM] = depth;
	  insn = read_memory_integer (pc, 2);
	  depth += 4;
	}
      else if (IS_ADD_SP (insn))
	{
	  pc += 2;
	  depth += -((char) (insn & 0xff));
	  insn = read_memory_integer (pc, 2);
	}
      else
	break;
    }

  /* Now we know how deep things are, we can work out their addresses */

  for (rn = 0; rn < NUM_REGS; rn++)
    {
      if (where[rn] >= 0)
	{
	  if (rn == FP_REGNUM)
	    have_fp = 1;

	  fsr->regs[rn] = fi->frame - where[rn] + depth - 4;
	}
      else
	{
	  fsr->regs[rn] = 0;
	}
    }

  if (have_fp)
    {
      fsr->regs[SP_REGNUM] = read_memory_integer (fsr->regs[FP_REGNUM], 4);
    }
  else
    {
      fsr->regs[SP_REGNUM] = fi->frame - 4;
    }


  /* Work out the return pc - either from the saved pr or the pr
     value */

  if (fsr->regs[PR_REGNUM])
    {
      fi->return_pc = read_memory_integer (fsr->regs[PR_REGNUM], 4) + 4;
    }
  else
    {
      fi->return_pc = read_register (PR_REGNUM) + 4;
    }
}

/* initialize the extra info saved in a FRAME */

void
init_extra_frame_info (fromleaf, fi)
     int fromleaf;
     struct frame_info *fi;
{
  struct frame_saved_regs dummy;
  frame_find_saved_regs (fi, &dummy);
}


/* Discard from the stack the innermost frame,
   restoring all saved registers.  */

void
pop_frame ()
{
  register FRAME frame = get_current_frame ();
  register CORE_ADDR fp;
  register int regnum;
  struct frame_saved_regs fsr;
  struct frame_info *fi;

  fi = get_frame_info (frame);
  fp = fi->frame;
  get_frame_saved_regs (fi, &fsr);

  /* Copy regs from where they were saved in the frame */
  for (regnum = 0; regnum < NUM_REGS; regnum++)
    {
      if (fsr.regs[regnum])
	{
	  write_register (regnum, read_memory_integer (fsr.regs[regnum], 4));
	}
    }

  write_register (PC_REGNUM, fi->return_pc);
  write_register (SP_REGNUM, fp + 4);
  flush_cached_frames ();
  set_current_frame (create_new_frame (read_register (FP_REGNUM),
				       read_pc ()));
}

/* Print the registers in a form similar to the E7000 */
static void
show_regs (args, from_tty)
char *args;
int from_tty;
{
  printf_filtered("PC=%08x SR=%08x PR=%08x MACH=%08x MACHL=%08x\n",
		  read_register(PC_REGNUM),
		  read_register(SR_REGNUM),
		  read_register(PR_REGNUM),
		  read_register(MACH_REGNUM),
		  read_register(MACL_REGNUM));

  printf_filtered("R0-R7  %08x %08x %08x %08x %08x %08x %08x %08x\n",
		  read_register(0),
		  read_register(1),
		  read_register(2),
		  read_register(3),
		  read_register(4),
		  read_register(5),
		  read_register(6),
		  read_register(7));
  printf_filtered("R8-R15 %08x %08x %08x %08x %08x %08x %08x %08x\n",
		  read_register(8),
		  read_register(9),
		  read_register(10),
		  read_register(11),
		  read_register(12),
		  read_register(13),
		  read_register(14),
		  read_register(15));
}


void
_initialize_sh_tdep ()
{
  extern int sim_memory_size;
  /* FIXME, there should be a way to make a CORE_ADDR variable settable. */
  add_show_from_set
    (add_set_cmd ("memory_size", class_support, var_uinteger,
		  (char *) &sim_memory_size,
		"Set simulated memory size of simulator target.", &setlist),
     &showlist);

  add_com("regs", class_vars, show_regs, "Print all registers");
}
