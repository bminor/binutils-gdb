/* Target-dependent code for the Matsushita MN10300 for GDB, the GNU debugger.
   Prologue analysis module, extracted from mn10300-tdep.c, Oct. 1, 2004.

   Copyright 1996, 1997, 1998, 1999, 2000, 2001, 2002, 2003, 2004 Free
   Software Foundation, Inc.

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
#include "symtab.h"
#include "inferior.h"
#include "gdbcore.h"
#include "gdb_string.h"
#include "trad-frame.h"
#include "mn10300-tdep.h"

enum movm_register_bits {
  movm_exother_bit = 0x01,
  movm_exreg1_bit  = 0x02,
  movm_exreg0_bit  = 0x04,
  movm_other_bit   = 0x08,
  movm_a3_bit      = 0x10,
  movm_a2_bit      = 0x20,
  movm_d3_bit      = 0x40,
  movm_d2_bit      = 0x80
};

/* Values for frame_info.status */

enum frame_kind {
  MY_FRAME_IN_SP = 0x1,
  MY_FRAME_IN_FP = 0x2,
  NO_MORE_FRAMES = 0x4
};

/* 
 * Frame Extra Info:
 *
 *   status -- actually frame type (SP, FP, or last frame)
 *   stack size -- offset to the next frame
 * 
 * The former might ultimately be stored in the frame_base.
 * Seems like there'd be a way to store the later too.
 *
 * Temporarily supply empty stub functions as place holders.
 */

static void
my_frame_is_in_sp (struct frame_info *fi, void **this_cache)
{
  struct trad_frame_cache *cache = mn10300_frame_unwind_cache (fi, this_cache);
  trad_frame_set_this_base (cache, 
			    frame_unwind_register_unsigned (fi, 
							    E_SP_REGNUM));
}

static void
my_frame_is_in_fp (struct frame_info *fi, void **this_cache)
{
  struct trad_frame_cache *cache = mn10300_frame_unwind_cache (fi, this_cache);
  trad_frame_set_this_base (cache, 
			    frame_unwind_register_unsigned (fi, 
							    E_A3_REGNUM));
}

static void
my_frame_is_last (struct frame_info *fi)
{
}

static int
is_my_frame_in_sp (struct frame_info *fi)
{
  return 0;
}

static int
is_my_frame_in_fp (struct frame_info *fi)
{
  return 0;
}

static int
is_my_frame_last (struct frame_info *fi)
{
  return 0;
}

static void
set_my_stack_size (struct frame_info *fi, CORE_ADDR size)
{
}


/* Set offsets of registers saved by movm instruction.
   This is a helper function for mn10300_analyze_prologue.  */

static void
set_movm_offsets (struct frame_info *fi, 
		  void **this_cache, 
		  int movm_args)
{
  struct trad_frame_cache *cache;
  int offset = 0;
  CORE_ADDR base;

  if (fi == NULL || this_cache == NULL)
    return;

  cache = mn10300_frame_unwind_cache (fi, this_cache);
  if (cache == NULL)
    return;

  base = trad_frame_get_this_base (cache);
  if (movm_args & movm_other_bit)
    {
      /* The `other' bit leaves a blank area of four bytes at the
         beginning of its block of saved registers, making it 32 bytes
         long in total.  */
      trad_frame_set_reg_addr (cache, E_LAR_REGNUM,    base + offset + 4);
      trad_frame_set_reg_addr (cache, E_LIR_REGNUM,    base + offset + 8);
      trad_frame_set_reg_addr (cache, E_MDR_REGNUM,    base + offset + 12);
      trad_frame_set_reg_addr (cache, E_A0_REGNUM + 1, base + offset + 16);
      trad_frame_set_reg_addr (cache, E_A0_REGNUM,     base + offset + 20);
      trad_frame_set_reg_addr (cache, E_D0_REGNUM + 1, base + offset + 24);
      trad_frame_set_reg_addr (cache, E_D0_REGNUM,     base + offset + 28);
      offset += 32;
    }

  if (movm_args & movm_a3_bit)
    {
      trad_frame_set_reg_addr (cache, E_A3_REGNUM, base + offset);
      offset += 4;
    }
  if (movm_args & movm_a2_bit)
    {
      trad_frame_set_reg_addr (cache, E_A2_REGNUM, base + offset);
      offset += 4;
    }
  if (movm_args & movm_d3_bit)
    {
      trad_frame_set_reg_addr (cache, E_D3_REGNUM, base + offset);
      offset += 4;
    }
  if (movm_args & movm_d2_bit)
    {
      trad_frame_set_reg_addr (cache, E_D2_REGNUM, base + offset);
      offset += 4;
    }
  if (AM33_MODE)
    {
      if (movm_args & movm_exother_bit)
        {
	  trad_frame_set_reg_addr (cache, E_MCVF_REGNUM, base + offset);
	  trad_frame_set_reg_addr (cache, E_MCRL_REGNUM, base + offset + 4);
	  trad_frame_set_reg_addr (cache, E_MCRH_REGNUM, base + offset + 8);
	  trad_frame_set_reg_addr (cache, E_MDRQ_REGNUM, base + offset + 12);
	  trad_frame_set_reg_addr (cache, E_E1_REGNUM,   base + offset + 16);
	  trad_frame_set_reg_addr (cache, E_E0_REGNUM,   base + offset + 20);
          offset += 24;
        }
      if (movm_args & movm_exreg1_bit)
        {
	  trad_frame_set_reg_addr (cache, E_E7_REGNUM, base + offset);
	  trad_frame_set_reg_addr (cache, E_E6_REGNUM, base + offset + 4);
	  trad_frame_set_reg_addr (cache, E_E5_REGNUM, base + offset + 8);
	  trad_frame_set_reg_addr (cache, E_E4_REGNUM, base + offset + 12);
          offset += 16;
        }
      if (movm_args & movm_exreg0_bit)
        {
	  trad_frame_set_reg_addr (cache, E_E3_REGNUM, base + offset);
	  trad_frame_set_reg_addr (cache, E_E2_REGNUM, base + offset + 4);
          offset += 8;
        }
    }
  /* The last (or first) thing on the stack will be the PC.  */
  trad_frame_set_reg_addr (cache, E_PC_REGNUM, base + offset);
  /* Save the SP in the 'traditional' way.  
     This will be the same location where the PC is saved.  */
  trad_frame_set_reg_value (cache, E_SP_REGNUM, base + offset);
}

/* The main purpose of this file is dealing with prologues to extract
   information about stack frames and saved registers.

   In gcc/config/mn13000/mn10300.c, the expand_prologue prologue
   function is pretty readable, and has a nice explanation of how the
   prologue is generated.  The prologues generated by that code will
   have the following form (NOTE: the current code doesn't handle all
   this!):

   + If this is an old-style varargs function, then its arguments
     need to be flushed back to the stack:
     
        mov d0,(4,sp)
        mov d1,(4,sp)

   + If we use any of the callee-saved registers, save them now.
     
        movm [some callee-saved registers],(sp)

   + If we have any floating-point registers to save:

     - Decrement the stack pointer to reserve space for the registers.
       If the function doesn't need a frame pointer, we may combine
       this with the adjustment that reserves space for the frame.

        add -SIZE, sp

     - Save the floating-point registers.  We have two possible
       strategies:

       . Save them at fixed offset from the SP:

        fmov fsN,(OFFSETN,sp)
        fmov fsM,(OFFSETM,sp)
        ...

       Note that, if OFFSETN happens to be zero, you'll get the
       different opcode: fmov fsN,(sp)

       . Or, set a0 to the start of the save area, and then use
       post-increment addressing to save the FP registers.

        mov sp, a0
        add SIZE, a0
        fmov fsN,(a0+)
        fmov fsM,(a0+)
        ...

   + If the function needs a frame pointer, we set it here.

        mov sp, a3

   + Now we reserve space for the stack frame proper.  This could be
     merged into the `add -SIZE, sp' instruction for FP saves up
     above, unless we needed to set the frame pointer in the previous
     step, or the frame is so large that allocating the whole thing at
     once would put the FP register save slots out of reach of the
     addressing mode (128 bytes).
      
        add -SIZE, sp        

   One day we might keep the stack pointer constant, that won't
   change the code for prologues, but it will make the frame
   pointerless case much more common.  */

/* Analyze the prologue to determine where registers are saved,
   the end of the prologue, etc etc.  Return the end of the prologue
   scanned.

   We store into FI (if non-null) several tidbits of information:

   * stack_size -- size of this stack frame.  Note that if we stop in
   certain parts of the prologue/epilogue we may claim the size of the
   current frame is zero.  This happens when the current frame has
   not been allocated yet or has already been deallocated.

   * fsr -- Addresses of registers saved in the stack by this frame.

   * status -- A (relatively) generic status indicator.  It's a bitmask
   with the following bits: 

   MY_FRAME_IN_SP: The base of the current frame is actually in
   the stack pointer.  This can happen for frame pointerless
   functions, or cases where we're stopped in the prologue/epilogue
   itself.  For these cases mn10300_analyze_prologue will need up
   update fi->frame before returning or analyzing the register
   save instructions.

   MY_FRAME_IN_FP: The base of the current frame is in the
   frame pointer register ($a3).

   NO_MORE_FRAMES: Set this if the current frame is "start" or
   if the first instruction looks like mov <imm>,sp.  This tells
   frame chain to not bother trying to unwind past this frame.  */

CORE_ADDR
mn10300_analyze_prologue (struct frame_info *fi, 
			  void **this_cache, 
			  CORE_ADDR pc)
{
  CORE_ADDR func_addr, func_end, addr, stop;
  long      stack_size;
  int imm_size;
  unsigned char buf[4];
  int status, movm_args = 0;
  char *name;

  /* Use the PC in the frame if it's provided to look up the
     start of this function.

     Note: kevinb/2003-07-16: We used to do the following here:
	pc = (fi ? get_frame_pc (fi) : pc);
     But this is (now) badly broken when called from analyze_dummy_frame().
  */
  if (fi)
    {
      pc = (pc ? pc : get_frame_pc (fi));
      /* At the start of a function our frame is in the stack pointer.  */
      my_frame_is_in_sp (fi, this_cache);
    }

  /* Find the start of this function.  */
  status = find_pc_partial_function (pc, &name, &func_addr, &func_end);

  /* Do nothing if we couldn't find the start of this function 

     MVS: comment went on to say "or if we're stopped at the first
     instruction in the prologue" -- but code doesn't reflect that, 
     and I don't want to do that anyway.  */
  if (status == 0)
    {
      return pc;
    }

  /* If we're in start, then give up.  */
  if (strcmp (name, "start") == 0)
    {
      if (fi != NULL)
	my_frame_is_last (fi);
      return pc;
    }

#if 0
  /* Get the next two bytes into buf, we need two because rets is a two
     byte insn and the first isn't enough to uniquely identify it.  */
  status = deprecated_read_memory_nobpt (pc, buf, 2);
  if (status != 0)
    return pc;

  /* Note: kevinb/2003-07-16: We shouldn't be making these sorts of
     changes to the frame in prologue examination code.  */
  /* If we're physically on an "rets" instruction, then our frame has
     already been deallocated.  Note this can also be true for retf
     and ret if they specify a size of zero.

     In this case fi->frame is bogus, we need to fix it.  */
  if (fi && buf[0] == 0xf0 && buf[1] == 0xfc)
    {
      if (get_next_frame (fi) == NULL)
	deprecated_update_frame_base_hack (fi, read_sp ());
      return get_frame_pc (fi);
    }

  /* Similarly if we're stopped on the first insn of a prologue as our
     frame hasn't been allocated yet.  */
  if (fi && get_frame_pc (fi) == func_addr)
    {
      if (get_next_frame (fi) == NULL)
	deprecated_update_frame_base_hack (fi, read_sp ());
      return get_frame_pc (fi);
    }
#endif

  /* NOTE: from here on, we don't want to return without jumping to
     finish_prologue.  */


  /* Figure out where to stop scanning.  */
  stop = fi ? pc : func_end;

  /* Don't walk off the end of the function.  */
  stop = stop > func_end ? func_end : stop;

  /* Start scanning on the first instruction of this function.  */
  addr = func_addr;

  /* Suck in two bytes.  */
  if (addr + 2 >= stop
      || (status = deprecated_read_memory_nobpt (addr, buf, 2)) != 0)
    goto finish_prologue;

  /* First see if this insn sets the stack pointer from a register; if
     so, it's probably the initialization of the stack pointer in _start,
     so mark this as the bottom-most frame.  */
  if (buf[0] == 0xf2 && (buf[1] & 0xf3) == 0xf0)
    {
      if (fi)
	my_frame_is_last (fi);
      goto finish_prologue;
    }

  /* Now look for movm [regs],sp, which saves the callee saved registers.

     At this time we don't know if fi->frame is valid, so we only note
     that we encountered a movm instruction.  Later, we'll set the entries
     in fsr.regs as needed.  */
  if (buf[0] == 0xcf)
    {
      /* Extract the register list for the movm instruction.  */
      movm_args = buf[1];

      addr += 2;

      /* Quit now if we're beyond the stop point.  */
      if (addr >= stop)
	goto finish_prologue;

      /* Get the next two bytes so the prologue scan can continue.  */
      status = deprecated_read_memory_nobpt (addr, buf, 2);
      if (status != 0)
	goto finish_prologue;
    }

  /* Now see if we set up a frame pointer via "mov sp,a3" */
  if (buf[0] == 0x3f)
    {
      addr += 1;

      /* The frame pointer is now valid.  */
      if (fi)
	{
	  my_frame_is_in_fp (fi, this_cache);
	}

      /* Quit now if we're beyond the stop point.  */
      if (addr >= stop)
	goto finish_prologue;

      /* Get two more bytes so scanning can continue.  */
      status = deprecated_read_memory_nobpt (addr, buf, 2);
      if (status != 0)
	goto finish_prologue;
    }

  /* Next we should allocate the local frame.  No more prologue insns
     are found after allocating the local frame.

     Search for add imm8,sp (0xf8feXX)
     or add imm16,sp (0xfafeXXXX)
     or add imm32,sp (0xfcfeXXXXXXXX).

     If none of the above was found, then this prologue has no 
     additional stack.  */

  imm_size = 0;
  if (buf[0] == 0xf8 && buf[1] == 0xfe)
    imm_size = 1;
  else if (buf[0] == 0xfa && buf[1] == 0xfe)
    imm_size = 2;
  else if (buf[0] == 0xfc && buf[1] == 0xfe)
    imm_size = 4;

  if (imm_size != 0)
    {
      /* Suck in imm_size more bytes, they'll hold the size of the
         current frame.  */
      status = deprecated_read_memory_nobpt (addr + 2, buf, imm_size);
      if (status != 0)
	goto finish_prologue;

      /* Note the size of the stack in the frame info structure.  */
      stack_size = extract_signed_integer (buf, imm_size);
      if (fi)
	set_my_stack_size (fi, stack_size);

      /* We just consumed 2 + imm_size bytes.  */
      addr += 2 + imm_size;

      /* No more prologue insns follow, so begin preparation to return.  */
      goto finish_prologue;
    }
  /* Do the essentials and get out of here.  */
 finish_prologue:
  /* Note if/where callee saved registers were saved.  */
  if (fi)
    set_movm_offsets (fi, this_cache, movm_args);
  return addr;
}


