/* Code dealing with dummy stack frames, for GDB, the GNU debugger.

   Copyright 1986, 1987, 1988, 1989, 1990, 1991, 1992, 1993, 1994,
   1995, 1996, 1997, 1998, 1999, 2000, 2001, 2002 Free Software
   Foundation, Inc.

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
#include "dummy-frame.h"
#include "regcache.h"
#include "frame.h"
#include "inferior.h"
#include "gdb_assert.h"

/* Dummy frame.  This saves the processor state just prior to setting
   up the inferior function call.  Older targets save the registers
   on the target stack (but that really slows down function calls).  */

struct dummy_frame
{
  struct dummy_frame *next;

  /* These values belong to the caller (the previous frame, the frame
     that this unwinds back to).  */
  CORE_ADDR pc;
  CORE_ADDR fp;
  CORE_ADDR sp;
  CORE_ADDR top;
  struct frame_id id;
  struct regcache *regcache;

  /* Address range of the call dummy code.  Look for PC in the range
     [LO..HI) (after allowing for DECR_PC_AFTER_BREAK).  */
  CORE_ADDR call_lo;
  CORE_ADDR call_hi;
};

static struct dummy_frame *dummy_frame_stack = NULL;

/* Function: find_dummy_frame(pc, fp, sp)

   Search the stack of dummy frames for one matching the given PC and
   FP/SP.  Unlike pc_in_dummy_frame(), this function doesn't need to
   adjust for DECR_PC_AFTER_BREAK.  This is because it is only legal
   to call this function after the PC has been adjusted.  */

static struct dummy_frame *
find_dummy_frame (CORE_ADDR pc, CORE_ADDR fp)
{
  struct dummy_frame *dummyframe;

  for (dummyframe = dummy_frame_stack; dummyframe != NULL;
       dummyframe = dummyframe->next)
    {
      /* Does the PC fall within the dummy frame's breakpoint
         instruction.  If not, discard this one.  */
      if (!(pc >= dummyframe->call_lo && pc < dummyframe->call_hi))
	continue;
      /* Does the FP match?  */
      if (dummyframe->top != 0)
	{
	  /* If the target architecture explicitly saved the
	     top-of-stack before the inferior function call, assume
	     that that same architecture will always pass in an FP
	     (frame base) value that eactly matches that saved TOS.
	     Don't check the saved SP and SP as they can lead to false
	     hits.  */
	  if (fp != dummyframe->top)
	    continue;
	}
      else
	{
	  /* An older target that hasn't explicitly or implicitly
             saved the dummy frame's top-of-stack.  Try matching the
             FP against the saved SP and FP.  NOTE: If you're trying
             to fix a problem with GDB not correctly finding a dummy
             frame, check the comments that go with FRAME_ALIGN() and
             SAVE_DUMMY_FRAME_TOS().  */
	  if (fp != dummyframe->fp && fp != dummyframe->sp)
	    continue;
	}
      /* The FP matches this dummy frame.  */
      return dummyframe;
    }

  return NULL;
}

struct dummy_frame *
cached_find_dummy_frame (struct frame_info *frame, void **cache)
{
  if ((*cache) == NULL)
    (*cache) = find_dummy_frame (get_frame_pc (frame), get_frame_base (frame));
  return (*cache);
}

struct regcache *
generic_find_dummy_frame (CORE_ADDR pc, CORE_ADDR fp)
{
  struct dummy_frame *dummy = find_dummy_frame (pc, fp);
  if (dummy != NULL)
    return dummy->regcache;
  else
    return NULL;
}

char *
deprecated_generic_find_dummy_frame (CORE_ADDR pc, CORE_ADDR fp)
{
  struct regcache *regcache = generic_find_dummy_frame (pc, fp);
  if (regcache == NULL)
    return NULL;
  return deprecated_grub_regcache_for_registers (regcache);
}

/* Function: pc_in_call_dummy (pc, sp, fp)

   Return true if the PC falls in a dummy frame created by gdb for an
   inferior call.  The code below which allows DECR_PC_AFTER_BREAK is
   for infrun.c, which may give the function a PC without that
   subtracted out.  */

int
generic_pc_in_call_dummy (CORE_ADDR pc, CORE_ADDR sp, CORE_ADDR fp)
{
  return pc_in_dummy_frame (pc);
}

/* Return non-zero if the PC falls in a dummy frame.

   The code below which allows DECR_PC_AFTER_BREAK is for infrun.c,
   which may give the function a PC without that subtracted out.

   FIXME: cagney/2002-11-23: This is silly.  Surely "infrun.c" can
   figure out what the real PC (as in the resume address) is BEFORE
   calling this function (Oh, and I'm not even sure that this function
   is called with an decremented PC, the call to pc_in_call_dummy() in
   that file is conditional on !CALL_DUMMY_BREAKPOINT_OFFSET_P yet
   generic dummy targets set CALL_DUMMY_BREAKPOINT_OFFSET. True?).  */

int
pc_in_dummy_frame (CORE_ADDR pc)
{
  struct dummy_frame *dummyframe;
  for (dummyframe = dummy_frame_stack;
       dummyframe != NULL;
       dummyframe = dummyframe->next)
    {
      if ((pc >= dummyframe->call_lo)
	  && (pc < dummyframe->call_hi + DECR_PC_AFTER_BREAK))
	return 1;
    }
  return 0;
}

/* Function: read_register_dummy 
   Find a saved register from before GDB calls a function in the inferior */

CORE_ADDR
deprecated_read_register_dummy (CORE_ADDR pc, CORE_ADDR fp, int regno)
{
  struct regcache *dummy_regs = generic_find_dummy_frame (pc, fp);

  if (dummy_regs)
    {
      /* NOTE: cagney/2002-08-12: Replaced a call to
	 regcache_raw_read_as_address() with a call to
	 regcache_cooked_read_unsigned().  The old, ...as_address
	 function was eventually calling extract_unsigned_integer (via
	 extract_address) to unpack the registers value.  The below is
	 doing an unsigned extract so that it is functionally
	 equivalent.  The read needs to be cooked as, otherwise, it
	 will never correctly return the value of a register in the
	 [NUM_REGS .. NUM_REGS+NUM_PSEUDO_REGS) range.  */
      ULONGEST val;
      regcache_cooked_read_unsigned (dummy_regs, regno, &val);
      return val;
    }
  else
    return 0;
}

/* Save all the registers on the dummy frame stack.  Most ports save the
   registers on the target stack.  This results in lots of unnecessary memory
   references, which are slow when debugging via a serial line.  Instead, we
   save all the registers internally, and never write them to the stack.  The
   registers get restored when the called function returns to the entry point,
   where a breakpoint is laying in wait.  */

void
generic_push_dummy_frame (void)
{
  struct dummy_frame *dummy_frame;
  CORE_ADDR fp = get_frame_base (get_current_frame ());

  /* check to see if there are stale dummy frames, 
     perhaps left over from when a longjump took us out of a 
     function that was called by the debugger */

  dummy_frame = dummy_frame_stack;
  while (dummy_frame)
    if (INNER_THAN (dummy_frame->fp, fp))	/* stale -- destroy! */
      {
	dummy_frame_stack = dummy_frame->next;
	regcache_xfree (dummy_frame->regcache);
	xfree (dummy_frame);
	dummy_frame = dummy_frame_stack;
      }
    else
      dummy_frame = dummy_frame->next;

  dummy_frame = xmalloc (sizeof (struct dummy_frame));
  dummy_frame->regcache = regcache_xmalloc (current_gdbarch);

  dummy_frame->pc = read_pc ();
  dummy_frame->sp = read_sp ();
  dummy_frame->top = 0;
  dummy_frame->fp = fp;
  dummy_frame->id = get_frame_id (get_current_frame ());
  regcache_cpy (dummy_frame->regcache, current_regcache);
  dummy_frame->next = dummy_frame_stack;
  dummy_frame_stack = dummy_frame;
}

void
generic_save_dummy_frame_tos (CORE_ADDR sp)
{
  dummy_frame_stack->top = sp;
}

/* Record the upper/lower bounds on the address of the call dummy.  */

void
generic_save_call_dummy_addr (CORE_ADDR lo, CORE_ADDR hi)
{
  dummy_frame_stack->call_lo = lo;
  dummy_frame_stack->call_hi = hi;
}

/* Restore the machine state from either the saved dummy stack or a
   real stack frame. */

void
generic_pop_current_frame (void (*popper) (struct frame_info * frame))
{
  struct frame_info *frame = get_current_frame ();
  if (get_frame_type (frame) == DUMMY_FRAME)
    /* NOTE: cagney/2002-22-23: Does this ever occure?  Surely a dummy
       frame will have already been poped by the "infrun.c" code.  */
    generic_pop_dummy_frame ();
  else
    (*popper) (frame);
}

/* Function: pop_dummy_frame
   Restore the machine state from a saved dummy stack frame. */

void
generic_pop_dummy_frame (void)
{
  struct dummy_frame *dummy_frame = dummy_frame_stack;

  /* FIXME: what if the first frame isn't the right one, eg..
     because one call-by-hand function has done a longjmp into another one? */

  if (!dummy_frame)
    error ("Can't pop dummy frame!");
  dummy_frame_stack = dummy_frame->next;
  regcache_cpy (current_regcache, dummy_frame->regcache);
  flush_cached_frames ();

  regcache_xfree (dummy_frame->regcache);
  xfree (dummy_frame);
}

/* Function: fix_call_dummy
   Stub function.  Generic dummy frames typically do not need to fix
   the frame being created */

void
generic_fix_call_dummy (char *dummy, CORE_ADDR pc, CORE_ADDR fun, int nargs,
			struct value **args, struct type *type, int gcc_p)
{
  return;
}

/* Given a call-dummy dummy-frame, return the registers.  Here the
   register value is taken from the local copy of the register buffer.  */

void
dummy_frame_register_unwind (struct frame_info *frame, void **cache,
			     int regnum, int *optimized,
			     enum lval_type *lvalp, CORE_ADDR *addrp,
			     int *realnum, void *bufferp)
{
  struct dummy_frame *dummy = cached_find_dummy_frame (frame, cache);
  gdb_assert (dummy != NULL);

  /* Describe the register's location.  Generic dummy frames always
     have the register value in an ``expression''.  */
  *optimized = 0;
  *lvalp = not_lval;
  *addrp = 0;
  *realnum = -1;

  /* If needed, find and return the value of the register.  */
  if (bufferp != NULL)
    {
      /* Return the actual value.  */
      /* Use the regcache_cooked_read() method so that it, on the fly,
         constructs either a raw or pseudo register from the raw
         register cache.  */
      regcache_cooked_read (dummy->regcache, regnum, bufferp);
    }
}

CORE_ADDR
dummy_frame_pc_unwind (struct frame_info *frame,
		       void **cache)
{
  struct dummy_frame *dummy = cached_find_dummy_frame (frame, cache);
  /* Oops!  In a dummy-frame but can't find the stack dummy.  Pretend
     that the frame doesn't unwind.  Should this function instead
     return a has-no-caller indication?  */
  if (dummy == NULL)
    return 0;
  return dummy->pc;
}


struct frame_id
dummy_frame_id_unwind (struct frame_info *frame,
		       void **cache)
{
  struct dummy_frame *dummy = cached_find_dummy_frame (frame, cache);
  /* Oops!  In a dummy-frame but can't find the stack dummy.  Pretend
     that the frame doesn't unwind.  Should this function instead
     return a has-no-caller indication?  */
  if (dummy == NULL)
    return null_frame_id;
  return dummy->id;
}

