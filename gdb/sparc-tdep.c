/* Machine-dependent code which would otherwise be in inflow.c and core.c,
   for GDB, the GNU debugger.
   Copyright (C) 1986, 1987, 1989 Free Software Foundation, Inc.
   This code is for the sparc cpu.

This file is part of GDB.

GDB is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GDB is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GDB; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include <stdio.h>
#include "defs.h"
#include "param.h"
#include "frame.h"
#include "inferior.h"
#include "obstack.h"
#include "signame.h"
#include "target.h"
#include "ieee-float.h"

#include <sys/param.h>
#include <sys/dir.h>
#include <sys/user.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include <sys/ptrace.h>

#include <sys/file.h>
#include <sys/stat.h>
#include <sys/core.h>

#include "gdbcore.h"

/* From infrun.c */
extern int stop_after_trap;

typedef enum
{
  Error, not_branch, bicc, bicca, ba, baa, ticc, ta,
} branch_type;

/* Simulate single-step ptrace call for sun4.  Code written by Gary
   Beihl (beihl@mcc.com).  */

/* npc4 and next_pc describe the situation at the time that the
   step-breakpoint was set, not necessary the current value of NPC_REGNUM.  */
static CORE_ADDR next_pc, npc4, target;
static int brknpc4, brktrg;
typedef char binsn_quantum[BREAKPOINT_MAX];
static binsn_quantum break_mem[3];

/* Non-zero if we just simulated a single-step ptrace call.  This is
   needed because we cannot remove the breakpoints in the inferior
   process until after the `wait' in `wait_for_inferior'.  Used for
   sun4. */

int one_stepped;

void
single_step (signal)
     int signal;
{
  branch_type br, isannulled();
  CORE_ADDR pc;
  long pc_instruction;

  if (!one_stepped)
    {
      /* Always set breakpoint for NPC.  */
      next_pc = read_register (NPC_REGNUM);
      npc4 = next_pc + 4; /* branch not taken */

      target_insert_breakpoint (next_pc, break_mem[0]);
      /* printf ("set break at %x\n",next_pc); */

      pc = read_register (PC_REGNUM);
      pc_instruction = read_memory_integer (pc, sizeof(pc_instruction));
      br = isannulled (pc_instruction, pc, &target);
      brknpc4 = brktrg = 0;

      if (br == bicca)
	{
	  /* Conditional annulled branch will either end up at
	     npc (if taken) or at npc+4 (if not taken).
	     Trap npc+4.  */
	  brknpc4 = 1;
	  target_insert_breakpoint (npc4, break_mem[1]);
	}
      else if (br == baa && target != next_pc)
	{
	  /* Unconditional annulled branch will always end up at
	     the target.  */
	  brktrg = 1;
	  target_insert_breakpoint (target, break_mem[2]);
	}

      /* Let it go */
      ptrace (7, inferior_pid, 1, signal);
      one_stepped = 1;
      return;
    }
  else
    {
      /* Remove breakpoints */
      target_remove_breakpoint (next_pc, break_mem[0]);

      if (brknpc4)
	target_remove_breakpoint (npc4, break_mem[1]);

      if (brktrg)
	target_remove_breakpoint (target, break_mem[2]);

      one_stepped = 0;
    }
}

/*
 * Find the pc saved in frame FRAME.  
 */
CORE_ADDR
frame_saved_pc (frame)
     FRAME frame;
{
  CORE_ADDR prev_pc;

  /* If it's at the bottom, the return value's stored in i7/rp */
  if (get_current_frame () == frame)
    prev_pc = GET_RWINDOW_REG (read_register (SP_REGNUM), rw_in[7]);
  else
    /* Wouldn't this always work?  This would allow this routine to
       be completely a macro.  */
    prev_pc = GET_RWINDOW_REG (frame->bottom, rw_in[7]);
  
  return PC_ADJUST (prev_pc);
}

/*
 * Since an individual frame in the frame cache is defined by two
 * arguments (a frame pointer and a stack pointer), we need two
 * arguments to get info for an arbitrary stack frame.  This routine
 * takes two arguments and makes the cached frames look as if these
 * two arguments defined a frame on the cache.  This allows the rest
 * of info frame to extract the important arguments without
 * difficulty. 
 */
FRAME
setup_arbitrary_frame (frame, stack)
     FRAME_ADDR frame, stack;
{
  FRAME fid = create_new_frame (frame, 0);

  if (!fid)
    fatal ("internal: create_new_frame returned invalid frame id");
  
  fid->bottom = stack;

  return fid;
}

/* This code was written by Gary Beihl (beihl@mcc.com).
   It was modified by Michael Tiemann (tiemann@corto.inria.fr).  */

/*
 * This routine appears to be passed a size by which to increase the
 * stack.  It then executes a save instruction in the inferior to
 * increase the stack by this amount.  Only the register window system
 * should be affected by this; the program counter & etc. will not be.
 *
 * This instructions used for this purpose are:
 *
 * 	sethi %hi(0x0),g1                    *
 * 	add g1,0x1ee0,g1                     *
 * 	save sp,g1,sp                        
 * 	sethi %hi(0x0),g1                    *
 * 	add g1,0x1ee0,g1                     *
 * 	t g0,0x1,o0
 * 	sethi %hi(0x0),g0                    (nop)
 *
 *  I presume that these set g1 to be the negative of the size, do a
 * save (putting the stack pointer at sp - size) and restore the
 * original contents of g1.  A * indicates that the actual value of
 * the instruction is modified below.
 */
static int save_insn_opcodes[] = {
  0x03000000, 0x82007ee0, 0x9de38001, 0x03000000,
  0x82007ee0, 0x91d02001, 0x01000000 };

/* Neither do_save_insn or do_restore_insn save stack configuration
   (current_frame, etc),
   since the stack is in an indeterminate state through the call to
   each of them.  That responsibility of the routine which calls them.  */

static void
do_save_insn (size)
     int size;
{
  int g1 = read_register (G1_REGNUM);
  CORE_ADDR sp = read_register (SP_REGNUM);
  CORE_ADDR pc = read_register (PC_REGNUM);
  CORE_ADDR npc = read_register (NPC_REGNUM);
  CORE_ADDR fake_pc = sp - sizeof (save_insn_opcodes);
  struct inferior_status inf_status;

  save_inferior_status (&inf_status, 0); /* Don't restore stack info */
  /*
   * See above.
   */
  save_insn_opcodes[0] = 0x03000000 | ((-size >> 10) & 0x3fffff);
  save_insn_opcodes[1] = 0x82006000 | (-size & 0x3ff);
  save_insn_opcodes[3] = 0x03000000 | ((g1 >> 10) & 0x3fffff);
  save_insn_opcodes[4] = 0x82006000 | (g1 & 0x3ff);
  write_memory (fake_pc, (char *)save_insn_opcodes, sizeof (save_insn_opcodes));

  clear_proceed_status ();
  stop_after_trap = 1;
  proceed (fake_pc, 0, 0);

  write_register (PC_REGNUM, pc);
  write_register (NPC_REGNUM, npc);
  restore_inferior_status (&inf_status);
}

/*
 * This routine takes a program counter value.  It restores the
 * register window system to the frame above the current one.
 * THIS ROUTINE CLOBBERS PC AND NPC IN THE TARGET!
 */

/*    The following insns translate to:
 
 	restore %g0,%g0,%g0
 	t %g0,1
 	sethi %hi(0),%g0	*/

static int restore_insn_opcodes[] = { 0x81e80000, 0x91d02001, 0x01000000 };

static void
do_restore_insn ()
{
  CORE_ADDR sp = read_register (SP_REGNUM);
  CORE_ADDR fake_pc = sp - sizeof (restore_insn_opcodes);
  struct inferior_status inf_status;

  save_inferior_status (&inf_status, 0); /* Don't restore stack info */

  write_memory (fake_pc, (char *)restore_insn_opcodes,
		sizeof (restore_insn_opcodes));

  clear_proceed_status ();
  stop_after_trap = 1;
  proceed (fake_pc, 0, 0);

  restore_inferior_status (&inf_status);
}

/* This routine should be more specific in it's actions; making sure
   that it uses the same register in the initial prologue section.  */
CORE_ADDR 
skip_prologue (start_pc)
     CORE_ADDR start_pc;
{
  union
    {
      unsigned long int code;
      struct
	{
	  unsigned int op:2;
	  unsigned int rd:5;
	  unsigned int op2:3;
	  unsigned int imm22:22;
	} sethi;
      struct
	{
	  unsigned int op:2;
	  unsigned int rd:5;
	  unsigned int op3:6;
	  unsigned int rs1:5;
	  unsigned int i:1;
	  unsigned int simm13:13;
	} add;
      int i;
    } x;
  int dest = -1;
  CORE_ADDR pc = start_pc;
  /* Have we found a save instruction?  */
  int found_save = 0;

  x.i = read_memory_integer (pc, 4);

  /* Recognize the `sethi' insn and record its destination.  */
  if (x.sethi.op == 0 && x.sethi.op2 == 4)
    {
      dest = x.sethi.rd;
      pc += 4;
      x.i = read_memory_integer (pc, 4);
    }

  /* Recognize an add immediate value to register to either %g1 or
     the destination register recorded above.  Actually, this might
     well recognize several different arithmetic operations.
     It doesn't check that rs1 == rd because in theory "sub %g0, 5, %g1"
     followed by "save %sp, %g1, %sp" is a valid prologue (Not that
     I imagine any compiler really does that, however).  */
  if (x.add.op == 2 && x.add.i && (x.add.rd == 1 || x.add.rd == dest))
    {
      pc += 4;
      x.i = read_memory_integer (pc, 4);
    }

  /* This recognizes any SAVE insn.  But why do the XOR and then
     the compare?  That's identical to comparing against 60 (as long
     as there isn't any sign extension).  */
  if (x.add.op == 2 && (x.add.op3 ^ 32) == 28)
    {
      found_save = 1;
      pc += 4;
      x.i = read_memory_integer (pc, 4);
    }

  /* Now we need to recognize stores into the frame from the input
     registers.  This recognizes all non alternate stores of input
     register, into a location offset from the frame pointer.  */
  while (x.add.op == 3
	 && (x.add.op3 & 0x3c) == 4 /* Store, non-alternate.  */
	 && (x.add.rd & 0x18) == 0x18 /* Input register.  */
	 && x.add.i		/* Immediate mode.  */
	 && x.add.rs1 == 30	/* Off of frame pointer.  */
	 /* Into reserved stack space.  */
	 && x.add.simm13 >= 0x44
	 && x.add.simm13 < 0x5b)
    {
      pc += 4;
      x.i = read_memory_integer (pc, 4);
    }
  if (found_save)
    return pc;
  else
    /* Without a save instruction, it's not a prologue.  */
    return start_pc;
}

/* Check instruction at ADDR to see if it is an annulled branch.
   All other instructions will go to NPC or will trap.
   Set *TARGET if we find a canidate branch; set to zero if not. */
   
branch_type
isannulled (instruction, addr, target)
     long instruction;
     CORE_ADDR addr, *target;
{
  branch_type val = not_branch;
  long int offset;		/* Must be signed for sign-extend.  */
  union
    {
      unsigned long int code;
      struct
	{
	  unsigned int op:2;
	  unsigned int a:1;
	  unsigned int cond:4;
	  unsigned int op2:3;
	  unsigned int disp22:22;
	} b;
    } insn;

  *target = 0;
  insn.code = instruction;

  if (insn.b.op == 0
      && (insn.b.op2 == 2 || insn.b.op2 == 6 || insn.b.op2 == 7))
    {
      if (insn.b.cond == 8)
	val = insn.b.a ? baa : ba;
      else
	val = insn.b.a ? bicca : bicc;
      offset = 4 * ((int) (insn.b.disp22 << 10) >> 10);
      *target = addr + offset;
    }

  return val;
}

/* sparc_frame_find_saved_regs ()

   Stores, into a struct frame_saved_regs,
   the addresses of the saved registers of frame described by FRAME_INFO.
   This includes special registers such as pc and fp saved in special
   ways in the stack frame.  sp is even more special:
   the address we return for it IS the sp for the next frame.

   Note that on register window machines, we are currently making the
   assumption that window registers are being saved somewhere in the
   frame in which they are being used.  If they are stored in an
   inferior frame, find_saved_register will break.

   On the Sun 4, the only time all registers are saved is when
   a dummy frame is involved.  Otherwise, the only saved registers
   are the LOCAL and IN registers which are saved as a result
   of the "save/restore" opcodes.  This condition is determined
   by address rather than by value.

   The "pc" is not stored in a frame on the SPARC.  (What is stored
   is a return address minus 8.)  sparc_pop_frame knows how to
   deal with that.  Other routines might or might not.

   See tm-sparc.h (PUSH_FRAME and friends) for CRITICAL information
   about how this works.  */

void
sparc_frame_find_saved_regs (fi, saved_regs_addr)
     struct frame_info *fi;
     struct frame_saved_regs *saved_regs_addr;
{
  register int regnum;
  FRAME_ADDR frame = read_register (FP_REGNUM);
  FRAME fid = FRAME_INFO_ID (fi);

  if (!fid)
    fatal ("Bad frame info struct in FRAME_FIND_SAVED_REGS");

  bzero (saved_regs_addr, sizeof (*saved_regs_addr));

  /* Old test.
  if (fi->pc >= frame - CALL_DUMMY_LENGTH - 0x140
      && fi->pc <= frame) */

  if (fi->pc >= (fi->bottom ? fi->bottom :
		   read_register (SP_REGNUM))
      && fi->pc <= FRAME_FP(fi))
    {
      /* Dummy frame.  All but the window regs are in there somewhere. */
      for (regnum = G1_REGNUM; regnum < G1_REGNUM+7; regnum++)
	saved_regs_addr->regs[regnum] =
	  frame + (regnum - G0_REGNUM) * 4 - 0xa0;
      for (regnum = I0_REGNUM; regnum < I0_REGNUM+8; regnum++)
	saved_regs_addr->regs[regnum] =
	  frame + (regnum - I0_REGNUM) * 4 - 0xc0;
      for (regnum = FP0_REGNUM; regnum < FP0_REGNUM + 32; regnum++)
	saved_regs_addr->regs[regnum] =
	  frame + (regnum - FP0_REGNUM) * 4 - 0x80;
      for (regnum = Y_REGNUM; regnum < NUM_REGS; regnum++)
	saved_regs_addr->regs[regnum] =
	  frame + (regnum - Y_REGNUM) * 4 - 0xe0;
      frame = fi->bottom ?
	fi->bottom : read_register (SP_REGNUM);
    }
  else
    {
      /* Normal frame.  Just Local and In registers */
      frame = fi->bottom ?
	fi->bottom : read_register (SP_REGNUM);
      for (regnum = L0_REGNUM; regnum < L0_REGNUM+16; regnum++)
	saved_regs_addr->regs[regnum] = frame + (regnum-L0_REGNUM) * 4;
    }
  if (fi->next)
    {
      /* Pull off either the next frame pointer or the stack pointer */
      FRAME_ADDR next_next_frame =
	(fi->next->bottom ?
	 fi->next->bottom :
	 read_register (SP_REGNUM));
      for (regnum = O0_REGNUM; regnum < O0_REGNUM+8; regnum++)
	saved_regs_addr->regs[regnum] = next_next_frame + regnum * 4;
    }
  /* Otherwise, whatever we would get from ptrace(GETREGS) is accurate */
  saved_regs_addr->regs[SP_REGNUM] = FRAME_FP (fi);
}

/* Push an empty stack frame, and record in it the current PC, regs, etc.

   Note that the write's are of registers in the context of the newly
   pushed frame.  Thus the the fp*'s, the g*'s, the i*'s, and
   the randoms, of the new frame, are being saved.  The locals and outs
   are new; they don't need to be saved. The i's and l's of
   the last frame were saved by the do_save_insn in the register
   file (now on the stack, since a context switch happended imm after).

   The return pointer register %i7 does not have
   the pc saved into it (return from this frame will be accomplished
   by a POP_FRAME).  In fact, we must leave it unclobbered, since we
   must preserve it in the calling routine except across call instructions.  */

/* Definitely see tm-sparc.h for more doc of the frame format here.  */

void
sparc_push_dummy_frame ()
{
  CORE_ADDR fp;
  char register_temp[REGISTER_BYTES];

  do_save_insn (0x140); /* FIXME where does this value come from? */
  fp = read_register (FP_REGNUM);

  read_register_bytes (REGISTER_BYTE (FP0_REGNUM), register_temp, 32 * 4);
  write_memory (fp - 0x80, register_temp, 32 * 4);

  read_register_bytes (REGISTER_BYTE (G0_REGNUM), register_temp, 8 * 4);
  write_memory (fp - 0xa0, register_temp, 8 * 4);

  read_register_bytes (REGISTER_BYTE (I0_REGNUM), register_temp, 8 * 4);
  write_memory (fp - 0xc0, register_temp, 8 * 4);

  /* Y, PS, WIM, TBR, PC, NPC, FPS, CPS regs */
  read_register_bytes (REGISTER_BYTE (Y_REGNUM), register_temp, 8 * 4);
  write_memory (fp - 0xe0, register_temp, 8 * 4);
}

/* Discard from the stack the innermost frame, restoring all saved registers.

   Note that the values stored in fsr by get_frame_saved_regs are *in
   the context of the called frame*.  What this means is that the i
   regs of fsr must be restored into the o regs of the (calling) frame that
   we pop into.  We don't care about the output regs of the calling frame,
   since unless it's a dummy frame, it won't have any output regs in it.

   We never have to bother with %l (local) regs, since the called routine's
   locals get tossed, and the calling routine's locals are already saved
   on its stack.  */

/* Definitely see tm-sparc.h for more doc of the frame format here.  */

void
sparc_pop_frame ()
{
  register FRAME frame = get_current_frame ();
  register CORE_ADDR pc;
  struct frame_saved_regs fsr;
  struct frame_info *fi;
  char raw_buffer[REGISTER_BYTES];

  fi = get_frame_info (frame);
  get_frame_saved_regs (fi, &fsr);
  do_restore_insn ();
  if (fsr.regs[FP0_REGNUM])
    {
      read_memory (fsr.regs[FP0_REGNUM], raw_buffer, 32 * 4);
      write_register_bytes (REGISTER_BYTE (FP0_REGNUM), raw_buffer, 32 * 4);
    }
  if (fsr.regs[G1_REGNUM])
    {
      read_memory (fsr.regs[G1_REGNUM], raw_buffer, 7 * 4);
      write_register_bytes (REGISTER_BYTE (G1_REGNUM), raw_buffer, 7 * 4);
    }
  if (fsr.regs[I0_REGNUM])
    {
      read_memory (fsr.regs[I0_REGNUM], raw_buffer, 8 * 4);
      write_register_bytes (REGISTER_BYTE (O0_REGNUM), raw_buffer, 8 * 4);
    }
  if (fsr.regs[PS_REGNUM])
    write_register (PS_REGNUM, read_memory_integer (fsr.regs[PS_REGNUM], 4));
  if (fsr.regs[Y_REGNUM])
    write_register (Y_REGNUM, read_memory_integer (fsr.regs[Y_REGNUM], 4));
  if (fsr.regs[PC_REGNUM])
    {
      /* Explicitly specified PC (and maybe NPC) -- just restore them. */
      write_register (PC_REGNUM, read_memory_integer (fsr.regs[PC_REGNUM], 4));
      if (fsr.regs[NPC_REGNUM])
	write_register (NPC_REGNUM,
			read_memory_integer (fsr.regs[NPC_REGNUM], 4));
    }
  else if (fsr.regs[I7_REGNUM])
    {
      /* Return address in %i7 -- adjust it, then restore PC and NPC from it */
      pc = PC_ADJUST (read_memory_integer (fsr.regs[I7_REGNUM], 4));
      write_register (PC_REGNUM,  pc);
      write_register (NPC_REGNUM, pc + 4);
    }
  flush_cached_frames ();
  set_current_frame ( create_new_frame (read_register (FP_REGNUM),
					read_pc ()));
}

/* Structure of SPARC extended floating point numbers.
   This information is not currently used by GDB, since no current SPARC
   implementations support extended float.  */

const struct ext_format ext_format_sparc[] = {
/* tot sbyte smask expbyte manbyte */
 { 16, 0,    0x80, 0,1,	   4,8	},		/* sparc */
};
