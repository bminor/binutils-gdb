/* Code dealing with legacy frames, for GDB, the GNU debugger.

   Copyright 2003 Free Software Foundation, Inc.

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
#include "legacy-frame.h"
#include "gdb_assert.h"
#include "frame.h"
#include "gdbcore.h"
#include "regcache.h"
#include "target.h"
#include "dummy-frame.h"	/* For generic_find_dummy_frame.  */
#include "frame-unwind.h"


/* Legacy frame.  This saves the processor state just prior to setting
   up the inferior function call.  Older targets save the registers
   on the target stack (but that really slows down function calls).  */

struct frame_unwind_cache
{
  void *regs[1];
};

/* Given a legacy-frame, return the register register saved in the
   simplistic ``saved_regs'' cache.  If the value isn't here AND a
   value is needed, try the next inner most frame.  */

void
legacy_frame_register_unwind (struct frame_info *frame,
			     struct frame_unwind_cache **cache,
			     int regnum, int *optimizedp,
			     enum lval_type *lvalp, CORE_ADDR *addrp,
			     int *realnump, void *bufferp)
{
  /* There is always a frame at this point.  And THIS is the frame
     we're interested in.  */
  gdb_assert (frame != NULL);

  /* If we're using generic dummy frames, we'd better not be in a call
     dummy.  (generic_call_dummy_register_unwind ought to have been called
     instead.)  */
  gdb_assert (!(DEPRECATED_USE_GENERIC_DUMMY_FRAMES
		&& (get_frame_type (frame) == DUMMY_FRAME)));

  /* Load the saved_regs register cache.  */
  if (get_frame_saved_regs (frame) == NULL)
    FRAME_INIT_SAVED_REGS (frame);

  if (get_frame_saved_regs (frame) != NULL
      && get_frame_saved_regs (frame)[regnum] != 0)
    {
      if (regnum == SP_REGNUM)
	{
	  /* SP register treated specially.  */
	  *optimizedp = 0;
	  *lvalp = not_lval;
	  *addrp = 0;
	  *realnump = -1;
	  if (bufferp != NULL)
	    store_address (bufferp, REGISTER_RAW_SIZE (regnum),
			   get_frame_saved_regs (frame)[regnum]);
	}
      else
	{
	  /* Any other register is saved in memory, fetch it but cache
             a local copy of its value.  */
	  *optimizedp = 0;
	  *lvalp = lval_memory;
	  *addrp = get_frame_saved_regs (frame)[regnum];
	  *realnump = -1;
	  if (bufferp != NULL)
	    {
#if 1
	      /* Save each register value, as it is read in, in a
                 frame based cache.  */
	      if ((*cache) == NULL)
		{
		  int sizeof_cache = ((NUM_REGS + NUM_PSEUDO_REGS)
				      * sizeof (void *));
		  (*cache) = frame_obstack_zalloc (sizeof_cache);
		}
	      if ((*cache)->regs[regnum] == NULL)
		{
		  (*cache)->regs[regnum]
		    = frame_obstack_zalloc (REGISTER_RAW_SIZE (regnum));
		  read_memory (get_frame_saved_regs (frame)[regnum],
			       (*cache)->regs[regnum],
			       REGISTER_RAW_SIZE (regnum));
		}
	      memcpy (bufferp, (*cache)->regs[regnum],
		      REGISTER_RAW_SIZE (regnum));
#else
	      /* Read the value in from memory.  */
	      read_memory (get_frame_saved_regs (frame)[regnum], bufferp,
			   REGISTER_RAW_SIZE (regnum));
#endif
	    }
	}
      return;
    }

  /* No luck, assume this and the next frame have the same register
     value.  If a value is needed, pass the request on down the chain;
     otherwise just return an indication that the value is in the same
     register as the next frame.  */
  if (bufferp == NULL)
    {
      *optimizedp = 0;
      *lvalp = lval_register;
      *addrp = REGISTER_BYTE (regnum);
      *realnump = regnum;
    }
  else
    {
      frame_register (frame, regnum, optimizedp, lvalp, addrp,
		      realnump, bufferp);
    }
}

CORE_ADDR
legacy_frame_pc_unwind (struct frame_info *frame,
			struct frame_unwind_cache **cache)
{
  return FRAME_SAVED_PC (frame);
}

void
legacy_frame_id_unwind (struct frame_info *frame,
			struct frame_unwind_cache **cache,
			struct frame_id *id)
{
  int fromleaf;

  if (get_next_frame (frame) == NULL)
    /* FIXME: 2002-11-09: Frameless functions can occure anywhere in
       the frame chain, not just the inner most frame!  The generic,
       per-architecture, frame code should handle this and the below
       should simply be removed.  */
    fromleaf = FRAMELESS_FUNCTION_INVOCATION (frame);
  else
    fromleaf = 0;

  if (fromleaf)
    /* A frameless inner-most frame.  The `FP' (which isn't an
       architecture frame-pointer register!) of the caller is the same
       as the callee.  */
    /* FIXME: 2002-11-09: There isn't any reason to special case this
       edge condition.  Instead the per-architecture code should hande
       it locally.  */
    id->base = get_frame_base (frame);
  else
    {
      /* Two macros defined in tm.h specify the machine-dependent
         actions to be performed here.

         First, get the frame's chain-pointer.

         If that is zero, the frame is the outermost frame or a leaf
         called by the outermost frame.  This means that if start
         calls main without a frame, we'll return 0 (which is fine
         anyway).

         Nope; there's a problem.  This also returns when the current
         routine is a leaf of main.  This is unacceptable.  We move
         this to after the ffi test; I'd rather have backtraces from
         start go curfluy than have an abort called from main not show
         main.  */
      id->base = FRAME_CHAIN (frame);

      if (!frame_chain_valid (id->base, frame))
	{
	  *id = null_frame_id;
	  return;
	}
    }
  if (id->base == 0)
    {
      *id = null_frame_id;
      return;
    }

  /* FIXME: cagney/2002-06-08: This should probably return the frame's
     function and not the PC (a.k.a. resume address).  */
  id->pc = frame_pc_unwind (frame);
  return;
}

/* Function: deprecated_generic_get_saved_register.

   Find register number REGNUM relative to FRAME and put its (raw,
   target format) contents in *RAW_BUFFER.

   Set *OPTIMIZED if the variable was optimized out (and thus can't be
   fetched).  Note that this is never set to anything other than zero
   in this implementation.

   Set *LVAL to lval_memory, lval_register, or not_lval, depending on
   whether the value was fetched from memory, from a register, or in a
   strange and non-modifiable way (e.g. a frame pointer which was
   calculated rather than fetched).  We will use not_lval for values
   fetched from generic dummy frames.

   Set *ADDRP to the address, either in memory or as a REGISTER_BYTE
   offset into the registers array.  If the value is stored in a dummy
   frame, set *ADDRP to zero.

   To use this implementation, define a function called
   "get_saved_register" in your target code, which simply passes all
   of its arguments to this function.

   The argument RAW_BUFFER must point to aligned memory.  */

void
deprecated_generic_get_saved_register (char *raw_buffer, int *optimized,
				       CORE_ADDR *addrp,
				       struct frame_info *frame, int regnum,
				       enum lval_type *lval)
{
  if (!target_has_registers)
    error ("No registers.");

  /* Normal systems don't optimize out things with register numbers.  */
  if (optimized != NULL)
    *optimized = 0;

  if (addrp)			/* default assumption: not found in memory */
    *addrp = 0;

  /* Note: since the current frame's registers could only have been
     saved by frames INTERIOR TO the current frame, we skip examining
     the current frame itself: otherwise, we would be getting the
     previous frame's registers which were saved by the current frame.  */

  while (frame && ((frame = get_next_frame (frame)) != NULL))
    {
      if (get_frame_type (frame) == DUMMY_FRAME)
	{
	  if (lval)		/* found it in a CALL_DUMMY frame */
	    *lval = not_lval;
	  if (raw_buffer)
	    /* FIXME: cagney/2002-06-26: This should be via the
	       gdbarch_register_read() method so that it, on the fly,
	       constructs either a raw or pseudo register from the raw
	       register cache.  */
	    regcache_raw_read (generic_find_dummy_frame (get_frame_pc (frame),
							 get_frame_base (frame)),
			       regnum, raw_buffer);
	  return;
	}

      FRAME_INIT_SAVED_REGS (frame);
      if (frame->saved_regs != NULL
	  && frame->saved_regs[regnum] != 0)
	{
	  if (lval)		/* found it saved on the stack */
	    *lval = lval_memory;
	  if (regnum == SP_REGNUM)
	    {
	      if (raw_buffer)	/* SP register treated specially */
		store_address (raw_buffer, REGISTER_RAW_SIZE (regnum),
			       frame->saved_regs[regnum]);
	    }
	  else
	    {
	      if (addrp)	/* any other register */
		*addrp = frame->saved_regs[regnum];
	      if (raw_buffer)
		read_memory (frame->saved_regs[regnum], raw_buffer,
			     REGISTER_RAW_SIZE (regnum));
	    }
	  return;
	}
    }

  /* If we get thru the loop to this point, it means the register was
     not saved in any frame.  Return the actual live-register value.  */

  if (lval)			/* found it in a live register */
    *lval = lval_register;
  if (addrp)
    *addrp = REGISTER_BYTE (regnum);
  if (raw_buffer)
    deprecated_read_register_gen (regnum, raw_buffer);
}


const struct frame_unwind legacy_frame_unwind =
{
  legacy_frame_pc_unwind,
  legacy_frame_id_unwind,
  legacy_frame_register_unwind
};

const struct frame_unwind *
legacy_frame_unwind_p (CORE_ADDR pc)
{
  return &legacy_frame_unwind;
}
