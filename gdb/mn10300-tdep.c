/* Target-dependent code for the Matsushita MN10300 for GDB, the GNU debugger.

   Copyright 1996, 1997, 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005
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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include "defs.h"
#include "arch-utils.h"
#include "dis-asm.h"
#include "gdbtypes.h"
#include "regcache.h"
#include "gdb_string.h"
#include "gdb_assert.h"
#include "gdbcore.h"	/* for write_memory_unsigned_integer */
#include "value.h"
#include "gdbtypes.h"
#include "frame.h"
#include "frame-unwind.h"
#include "frame-base.h"
#include "trad-frame.h"
#include "symtab.h"
#include "dwarf2-frame.h"
#include "regcache.h"

#include "mn10300-tdep.h"

/* Forward decl.  */
extern struct trad_frame_cache *mn10300_frame_unwind_cache (struct frame_info*,
							    void **);

/* Compute the alignment required by a type.  */

static int
mn10300_type_align (struct type *type)
{
  int i, align = 1;

  switch (TYPE_CODE (type))
    {
    case TYPE_CODE_INT:
    case TYPE_CODE_ENUM:
    case TYPE_CODE_SET:
    case TYPE_CODE_RANGE:
    case TYPE_CODE_CHAR:
    case TYPE_CODE_BOOL:
    case TYPE_CODE_FLT:
    case TYPE_CODE_PTR:
    case TYPE_CODE_REF:
      return TYPE_LENGTH (type);

    case TYPE_CODE_COMPLEX:
      return TYPE_LENGTH (type) / 2;

    case TYPE_CODE_STRUCT:
    case TYPE_CODE_UNION:
      for (i = 0; i < TYPE_NFIELDS (type); i++)
	{
	  int falign = mn10300_type_align (TYPE_FIELD_TYPE (type, i));
	  while (align < falign)
	    align <<= 1;
	}
      return align;

    case TYPE_CODE_ARRAY:
      /* HACK!  Structures containing arrays, even small ones, are not
	 elligible for returning in registers.  */
      return 256;

    case TYPE_CODE_TYPEDEF:
      return mn10300_type_align (check_typedef (type));

    default:
      internal_error (__FILE__, __LINE__, _("bad switch"));
    }
}

/* MVS note this is deprecated.  */
/* Should call_function allocate stack space for a struct return?  */
/* gcc_p unused */
static int
mn10300_use_struct_convention (int gcc_p, struct type *type)
{
  /* Structures bigger than a pair of words can't be returned in
     registers.  */
  if (TYPE_LENGTH (type) > 8)
    return 1;

  switch (TYPE_CODE (type))
    {
    case TYPE_CODE_STRUCT:
    case TYPE_CODE_UNION:
      /* Structures with a single field are handled as the field
	 itself.  */
      if (TYPE_NFIELDS (type) == 1)
	return mn10300_use_struct_convention (gcc_p, 
					      TYPE_FIELD_TYPE (type, 0));

      /* Structures with word or double-word size are passed in memory, as
	 long as they require at least word alignment.  */
      if (mn10300_type_align (type) >= 4)
	return 0;

      return 1;

      /* Arrays are addressable, so they're never returned in
	 registers.  This condition can only hold when the array is
	 the only field of a struct or union.  */
    case TYPE_CODE_ARRAY:
      return 1;

    case TYPE_CODE_TYPEDEF:
      return mn10300_use_struct_convention (gcc_p, check_typedef (type));

    default:
      return 0;
    }
}

/* MVS note this is deprecated.  */
static void
mn10300_store_return_value (struct type *type,
			    struct regcache *regcache, const void *valbuf)
{
  struct gdbarch *gdbarch = get_regcache_arch (regcache);
  int len = TYPE_LENGTH (type);
  int reg, regsz;
  
  if (TYPE_CODE (type) == TYPE_CODE_PTR)
    reg = 4;
  else
    reg = 0;

  regsz = register_size (gdbarch, reg);

  if (len <= regsz)
    regcache_raw_write_part (regcache, reg, 0, len, valbuf);
  else if (len <= 2 * regsz)
    {
      regcache_raw_write (regcache, reg, valbuf);
      gdb_assert (regsz == register_size (gdbarch, reg + 1));
      regcache_raw_write_part (regcache, reg+1, 0,
			       len - regsz, (char *) valbuf + regsz);
    }
  else
    internal_error (__FILE__, __LINE__,
		    _("Cannot store return value %d bytes long."), len);
}

/* MVS note deprecated.  */
static void
mn10300_extract_return_value (struct type *type,
			      struct regcache *regcache, void *valbuf)
{
  struct gdbarch *gdbarch = get_regcache_arch (regcache);
  char buf[MAX_REGISTER_SIZE];
  int len = TYPE_LENGTH (type);
  int reg, regsz;

  if (TYPE_CODE (type) == TYPE_CODE_PTR)
    reg = 4;
  else
    reg = 0;

  regsz = register_size (gdbarch, reg);
  if (len <= regsz)
    {
      regcache_raw_read (regcache, reg, buf);
      memcpy (valbuf, buf, len);
    }
  else if (len <= 2 * regsz)
    {
      regcache_raw_read (regcache, reg, buf);
      memcpy (valbuf, buf, regsz);
      gdb_assert (regsz == register_size (gdbarch, reg + 1));
      regcache_raw_read (regcache, reg + 1, buf);
      memcpy ((char *) valbuf + regsz, buf, len - regsz);
    }
  else
    internal_error (__FILE__, __LINE__,
		    _("Cannot extract return value %d bytes long."), len);
}

static char *
register_name (int reg, char **regs, long sizeof_regs)
{
  if (reg < 0 || reg >= sizeof_regs / sizeof (regs[0]))
    return NULL;
  else
    return regs[reg];
}

static const char *
mn10300_generic_register_name (int reg)
{
  static char *regs[] =
  { "d0", "d1", "d2", "d3", "a0", "a1", "a2", "a3",
    "sp", "pc", "mdr", "psw", "lir", "lar", "", "",
    "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "fp"
  };
  return register_name (reg, regs, sizeof regs);
}


static const char *
am33_register_name (int reg)
{
  static char *regs[] =
  { "d0", "d1", "d2", "d3", "a0", "a1", "a2", "a3",
    "sp", "pc", "mdr", "psw", "lir", "lar", "",
    "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
    "ssp", "msp", "usp", "mcrh", "mcrl", "mcvf", "", "", ""
  };
  return register_name (reg, regs, sizeof regs);
}


static struct type *
mn10300_register_type (struct gdbarch *gdbarch, int reg)
{
  return builtin_type_int;
}

static CORE_ADDR
mn10300_read_pc (ptid_t ptid)
{
  return read_register_pid (E_PC_REGNUM, ptid);
}

static void
mn10300_write_pc (CORE_ADDR val, ptid_t ptid)
{
  return write_register_pid (E_PC_REGNUM, val, ptid);
}

/* The breakpoint instruction must be the same size as the smallest
   instruction in the instruction set.

   The Matsushita mn10x00 processors have single byte instructions
   so we need a single byte breakpoint.  Matsushita hasn't defined
   one, so we defined it ourselves.  */

const static unsigned char *
mn10300_breakpoint_from_pc (CORE_ADDR *bp_addr, int *bp_size)
{
  static char breakpoint[] = {0xff};
  *bp_size = 1;
  return breakpoint;
}

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

static CORE_ADDR
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

/* Function: skip_prologue
   Return the address of the first inst past the prologue of the function.  */

static CORE_ADDR
mn10300_skip_prologue (CORE_ADDR pc)
{
  return mn10300_analyze_prologue (NULL, NULL, pc);
}

/* Simple frame_unwind_cache.  
   This finds the "extra info" for the frame.  */
struct trad_frame_cache *
mn10300_frame_unwind_cache (struct frame_info *next_frame,
			    void **this_prologue_cache)
{
  struct trad_frame_cache *cache;
  CORE_ADDR pc, start, end;

  if (*this_prologue_cache)
    return (*this_prologue_cache);

  cache = trad_frame_cache_zalloc (next_frame);
  pc = gdbarch_unwind_pc (current_gdbarch, next_frame);
  mn10300_analyze_prologue (next_frame, (void **) &cache, pc);
  if (find_pc_partial_function (pc, NULL, &start, &end))
    trad_frame_set_id (cache, 
		       frame_id_build (trad_frame_get_this_base (cache), 
				       start));
  else
    trad_frame_set_id (cache, 
		       frame_id_build (trad_frame_get_this_base (cache), 
				       frame_func_unwind (next_frame)));

  (*this_prologue_cache) = cache;
  return cache;
}

/* Here is a dummy implementation.  */
static struct frame_id
mn10300_unwind_dummy_id (struct gdbarch *gdbarch,
			 struct frame_info *next_frame)
{
  return frame_id_build (frame_sp_unwind (next_frame), 
			 frame_pc_unwind (next_frame));
}

/* Trad frame implementation.  */
static void
mn10300_frame_this_id (struct frame_info *next_frame,
		       void **this_prologue_cache,
		       struct frame_id *this_id)
{
  struct trad_frame_cache *cache = 
    mn10300_frame_unwind_cache (next_frame, this_prologue_cache);

  trad_frame_get_id (cache, this_id);
}

static void
mn10300_frame_prev_register (struct frame_info *next_frame,
			     void **this_prologue_cache,
			     int regnum, int *optimizedp,
			     enum lval_type *lvalp, CORE_ADDR *addrp,
			     int *realnump, void *bufferp)
{
  struct trad_frame_cache *cache =
    mn10300_frame_unwind_cache (next_frame, this_prologue_cache);

  trad_frame_get_register (cache, next_frame, regnum, optimizedp, 
			   lvalp, addrp, realnump, bufferp);
  /* Or...
  trad_frame_get_prev_register (next_frame, cache->prev_regs, regnum, 
			   optimizedp, lvalp, addrp, realnump, bufferp);
  */
}

static const struct frame_unwind mn10300_frame_unwind = {
  NORMAL_FRAME,
  mn10300_frame_this_id, 
  mn10300_frame_prev_register
};

static CORE_ADDR
mn10300_frame_base_address (struct frame_info *next_frame,
			    void **this_prologue_cache)
{
  struct trad_frame_cache *cache = 
    mn10300_frame_unwind_cache (next_frame, this_prologue_cache);

  return trad_frame_get_this_base (cache);
}

static const struct frame_unwind *
mn10300_frame_sniffer (struct frame_info *next_frame)
{
  return &mn10300_frame_unwind;
}

static const struct frame_base mn10300_frame_base = {
  &mn10300_frame_unwind, 
  mn10300_frame_base_address, 
  mn10300_frame_base_address,
  mn10300_frame_base_address
};

static CORE_ADDR
mn10300_unwind_pc (struct gdbarch *gdbarch, struct frame_info *next_frame)
{
  ULONGEST pc;

  frame_unwind_unsigned_register (next_frame, E_PC_REGNUM, &pc);
  return pc;
}

static CORE_ADDR
mn10300_unwind_sp (struct gdbarch *gdbarch, struct frame_info *next_frame)
{
  ULONGEST sp;

  frame_unwind_unsigned_register (next_frame, E_SP_REGNUM, &sp);
  return sp;
}

static void
mn10300_frame_unwind_init (struct gdbarch *gdbarch)
{
  frame_unwind_append_sniffer (gdbarch, dwarf2_frame_sniffer);
  frame_unwind_append_sniffer (gdbarch, mn10300_frame_sniffer);
  frame_base_set_default (gdbarch, &mn10300_frame_base);
  set_gdbarch_unwind_dummy_id (gdbarch, mn10300_unwind_dummy_id);
  set_gdbarch_unwind_pc (gdbarch, mn10300_unwind_pc);
  set_gdbarch_unwind_sp (gdbarch, mn10300_unwind_sp);
}

/* Function: push_dummy_call
 *
 * Set up machine state for a target call, including
 * function arguments, stack, return address, etc.
 *
 */

static CORE_ADDR
mn10300_push_dummy_call (struct gdbarch *gdbarch, 
			 struct value *target_func,
			 struct regcache *regcache,
			 CORE_ADDR bp_addr, 
			 int nargs, struct value **args,
			 CORE_ADDR sp, 
			 int struct_return,
			 CORE_ADDR struct_addr)
{
  const int push_size = register_size (gdbarch, E_PC_REGNUM);
  int regs_used;
  int len, arg_len; 
  int stack_offset = 0;
  int argnum;
  char *val, valbuf[MAX_REGISTER_SIZE];

  /* This should be a nop, but align the stack just in case something
     went wrong.  Stacks are four byte aligned on the mn10300.  */
  sp &= ~3;

  /* Now make space on the stack for the args.

     XXX This doesn't appear to handle pass-by-invisible reference
     arguments.  */
  regs_used = struct_return ? 1 : 0;
  for (len = 0, argnum = 0; argnum < nargs; argnum++)
    {
      arg_len = (TYPE_LENGTH (value_type (args[argnum])) + 3) & ~3;
      while (regs_used < 2 && arg_len > 0)
	{
	  regs_used++;
	  arg_len -= push_size;
	}
      len += arg_len;
    }

  /* Allocate stack space.  */
  sp -= len;

  if (struct_return)
    {
      regs_used = 1;
      write_register (E_D0_REGNUM, struct_addr);
    }
  else
    regs_used = 0;

  /* Push all arguments onto the stack. */
  for (argnum = 0; argnum < nargs; argnum++)
    {
      /* FIXME what about structs?  Unions?  */
      if (TYPE_CODE (value_type (*args)) == TYPE_CODE_STRUCT
	  && TYPE_LENGTH (value_type (*args)) > 8)
	{
	  /* Change to pointer-to-type.  */
	  arg_len = push_size;
	  store_unsigned_integer (valbuf, push_size, 
				  VALUE_ADDRESS (*args));
	  val = &valbuf[0];
	}
      else
	{
	  arg_len = TYPE_LENGTH (value_type (*args));
	  val = (char *) value_contents (*args);
	}

      while (regs_used < 2 && arg_len > 0)
	{
	  write_register (regs_used, 
			  extract_unsigned_integer (val, push_size));
	  val += push_size;
	  arg_len -= push_size;
	  regs_used++;
	}

      while (arg_len > 0)
	{
	  write_memory (sp + stack_offset, val, push_size);
	  arg_len -= push_size;
	  val += push_size;
	  stack_offset += push_size;
	}

      args++;
    }

  /* Make space for the flushback area.  */
  sp -= 8;

  /* Push the return address that contains the magic breakpoint.  */
  sp -= 4;
  write_memory_unsigned_integer (sp, push_size, bp_addr);
  /* Update $sp.  */
  regcache_cooked_write_unsigned (regcache, E_SP_REGNUM, sp);
  return sp;
}


static struct gdbarch *
mn10300_gdbarch_init (struct gdbarch_info info,
		      struct gdbarch_list *arches)
{
  struct gdbarch *gdbarch;
  struct gdbarch_tdep *tdep;

  arches = gdbarch_list_lookup_by_info (arches, &info);
  if (arches != NULL)
    return arches->gdbarch;

  tdep = xmalloc (sizeof (struct gdbarch_tdep));
  gdbarch = gdbarch_alloc (&info, tdep);

  switch (info.bfd_arch_info->mach)
    {
    case 0:
    case bfd_mach_mn10300:
      set_gdbarch_register_name (gdbarch, mn10300_generic_register_name);
      tdep->am33_mode = 0;
      break;
    case bfd_mach_am33:
      set_gdbarch_register_name (gdbarch, am33_register_name);
      tdep->am33_mode = 1;
      break;
    default:
      internal_error (__FILE__, __LINE__,
		      _("mn10300_gdbarch_init: Unknown mn10300 variant"));
      break;
    }

  /* Registers.  */
  set_gdbarch_num_regs (gdbarch, E_NUM_REGS);
  set_gdbarch_register_type (gdbarch, mn10300_register_type);
  set_gdbarch_skip_prologue (gdbarch, mn10300_skip_prologue);
  set_gdbarch_read_pc (gdbarch, mn10300_read_pc);
  set_gdbarch_write_pc (gdbarch, mn10300_write_pc);
  set_gdbarch_pc_regnum (gdbarch, E_PC_REGNUM);
  set_gdbarch_sp_regnum (gdbarch, E_SP_REGNUM);

  /* Stack unwinding.  */
  set_gdbarch_inner_than (gdbarch, core_addr_lessthan);
  /* Breakpoints.  */
  set_gdbarch_breakpoint_from_pc (gdbarch, mn10300_breakpoint_from_pc);
  /* decr_pc_after_break? */
  /* Disassembly.  */
  set_gdbarch_print_insn (gdbarch, print_insn_mn10300);

  /* Stage 2 */
  /* MVS Note: at least the first one is deprecated!  */
  set_gdbarch_deprecated_use_struct_convention (gdbarch, 
						mn10300_use_struct_convention);
  set_gdbarch_store_return_value (gdbarch, mn10300_store_return_value);
  set_gdbarch_extract_return_value (gdbarch, mn10300_extract_return_value);
  
  /* Stage 3 -- get target calls working.  */
  set_gdbarch_push_dummy_call (gdbarch, mn10300_push_dummy_call);
  /* set_gdbarch_return_value (store, extract) */


  mn10300_frame_unwind_init (gdbarch);

  return gdbarch;
}
 
/* Dump out the mn10300 specific architecture information. */

static void
mn10300_dump_tdep (struct gdbarch *current_gdbarch, struct ui_file *file)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (current_gdbarch);
  fprintf_unfiltered (file, "mn10300_dump_tdep: am33_mode = %d\n",
		      tdep->am33_mode);
}

void
_initialize_mn10300_tdep (void)
{
  gdbarch_register (bfd_arch_mn10300, mn10300_gdbarch_init, mn10300_dump_tdep);
}

