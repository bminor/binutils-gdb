/* Cache and manage frames for GDB, the GNU debugger.

   Copyright 1986, 1987, 1989, 1991, 1994, 1995, 1996, 1998, 2000,
   2001, 2002 Free Software Foundation, Inc.

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
#include "frame.h"
#include "target.h"
#include "value.h"
#include "inferior.h"	/* for inferior_ptid */
#include "regcache.h"
#include "gdb_assert.h"
#include "gdb_string.h"
#include "builtin-regs.h"
#include "gdb_obstack.h"
#include "dummy-frame.h"
#include "gdbcore.h"
#include "annotate.h"

/* Return a frame uniq ID that can be used to, later re-find the
   frame.  */

void
get_frame_id (struct frame_info *fi, struct frame_id *id)
{
  if (fi == NULL)
    {
      id->base = 0;
      id->pc = 0;
    }
  else
    {
      id->base = FRAME_FP (fi);
      id->pc = fi->pc;
    }
}

struct frame_info *
frame_find_by_id (struct frame_id id)
{
  struct frame_info *frame;

  /* ZERO denotes the null frame, let the caller decide what to do
     about it.  Should it instead return get_current_frame()?  */
  if (id.base == 0 && id.pc == 0)
    return NULL;

  for (frame = get_current_frame ();
       frame != NULL;
       frame = get_prev_frame (frame))
    {
      if (INNER_THAN (FRAME_FP (frame), id.base))
	/* ``inner/current < frame < id.base''.  Keep looking along
           the frame chain.  */
	continue;
      if (INNER_THAN (id.base, FRAME_FP (frame)))
	/* ``inner/current < id.base < frame''.  Oops, gone past it.
           Just give up.  */
	return NULL;
      /* FIXME: cagney/2002-04-21: This isn't sufficient.  It should
	 use id.pc to check that the two frames belong to the same
	 function.  Otherwise we'll do things like match dummy frames
	 or mis-match frameless functions.  However, until someone
	 notices, stick with the existing behavour.  */
      return frame;
    }
  return NULL;
}

CORE_ADDR
frame_pc_unwind (struct frame_info *frame)
{
  if (!frame->pc_unwind_cache_p)
    {
      frame->pc_unwind_cache = frame->pc_unwind (frame, &frame->unwind_cache);
      frame->pc_unwind_cache_p = 1;
    }
  return frame->pc_unwind_cache;
}

void
frame_register_unwind (struct frame_info *frame, int regnum,
		       int *optimizedp, enum lval_type *lvalp,
		       CORE_ADDR *addrp, int *realnump, void *bufferp)
{
  struct frame_unwind_cache *cache;

  /* Require all but BUFFERP to be valid.  A NULL BUFFERP indicates
     that the value proper does not need to be fetched.  */
  gdb_assert (optimizedp != NULL);
  gdb_assert (lvalp != NULL);
  gdb_assert (addrp != NULL);
  gdb_assert (realnump != NULL);
  /* gdb_assert (bufferp != NULL); */

  /* NOTE: cagney/2002-04-14: It would be nice if, instead of a
     special case, there was always an inner frame dedicated to the
     hardware registers.  Unfortunatly, there is too much unwind code
     around that looks up/down the frame chain while making the
     assumption that each frame level is using the same unwind code.  */

  if (frame == NULL)
    {
      /* We're in the inner-most frame, get the value direct from the
	 register cache.  */
      *optimizedp = 0;
      *lvalp = lval_register;
      /* ULGH!  Code uses the offset into the raw register byte array
         as a way of identifying a register.  */
      *addrp = REGISTER_BYTE (regnum);
      /* Should this code test ``register_cached (regnum) < 0'' and do
         something like set realnum to -1 when the register isn't
         available?  */
      *realnump = regnum;
      if (bufferp)
	deprecated_read_register_gen (regnum, bufferp);
      return;
    }

  /* Ask this frame to unwind its register.  */
  frame->register_unwind (frame, &frame->unwind_cache, regnum,
			  optimizedp, lvalp, addrp, realnump, bufferp);
}

void
frame_register (struct frame_info *frame, int regnum,
		int *optimizedp, enum lval_type *lvalp,
		CORE_ADDR *addrp, int *realnump, void *bufferp)
{
  /* Require all but BUFFERP to be valid.  A NULL BUFFERP indicates
     that the value proper does not need to be fetched.  */
  gdb_assert (optimizedp != NULL);
  gdb_assert (lvalp != NULL);
  gdb_assert (addrp != NULL);
  gdb_assert (realnump != NULL);
  /* gdb_assert (bufferp != NULL); */

  /* Ulgh!  Old code that, for lval_register, sets ADDRP to the offset
     of the register in the register cache.  It should instead return
     the REGNUM corresponding to that register.  Translate the .  */
  if (GET_SAVED_REGISTER_P ())
    {
      GET_SAVED_REGISTER (bufferp, optimizedp, addrp, frame, regnum, lvalp);
      /* Compute the REALNUM if the caller wants it.  */
      if (*lvalp == lval_register)
	{
	  int regnum;
	  for (regnum = 0; regnum < NUM_REGS + NUM_PSEUDO_REGS; regnum++)
	    {
	      if (*addrp == register_offset_hack (current_gdbarch, regnum))
		{
		  *realnump = regnum;
		  return;
		}
	    }
	  internal_error (__FILE__, __LINE__,
			  "Failed to compute the register number corresponding"
			  " to 0x%s", paddr_d (*addrp));
	}
      *realnump = -1;
      return;
    }

  /* Reached the the bottom (youngest, inner most) of the frame chain
     (youngest, inner most) frame, go direct to the hardware register
     cache (do not pass go, do not try to cache the value, ...).  The
     unwound value would have been cached in frame->next but that
     doesn't exist.  This doesn't matter as the hardware register
     cache is stopping any unnecessary accesses to the target.  */

  /* NOTE: cagney/2002-04-14: It would be nice if, instead of a
     special case, there was always an inner frame dedicated to the
     hardware registers.  Unfortunatly, there is too much unwind code
     around that looks up/down the frame chain while making the
     assumption that each frame level is using the same unwind code.  */

  if (frame == NULL)
    frame_register_unwind (NULL, regnum, optimizedp, lvalp, addrp, realnump,
			   bufferp);
  else
    frame_register_unwind (frame->next, regnum, optimizedp, lvalp, addrp,
			   realnump, bufferp);
}

void
frame_unwind_signed_register (struct frame_info *frame, int regnum,
			      LONGEST *val)
{
  int optimized;
  CORE_ADDR addr;
  int realnum;
  enum lval_type lval;
  void *buf = alloca (MAX_REGISTER_RAW_SIZE);
  frame_register_unwind (frame, regnum, &optimized, &lval, &addr,
			 &realnum, buf);
  (*val) = extract_signed_integer (buf, REGISTER_VIRTUAL_SIZE (regnum));
}

void
frame_unwind_unsigned_register (struct frame_info *frame, int regnum,
				ULONGEST *val)
{
  int optimized;
  CORE_ADDR addr;
  int realnum;
  enum lval_type lval;
  void *buf = alloca (MAX_REGISTER_RAW_SIZE);
  frame_register_unwind (frame, regnum, &optimized, &lval, &addr,
			 &realnum, buf);
  (*val) = extract_unsigned_integer (buf, REGISTER_VIRTUAL_SIZE (regnum));
}

void
frame_read_unsigned_register (struct frame_info *frame, int regnum,
			      ULONGEST *val)
{
  /* NOTE: cagney/2002-10-31: There is a bit of dogma here - there is
     always a frame.  Both this, and the equivalent
     frame_read_signed_register() function, can only be called with a
     valid frame.  If, for some reason, this function is called
     without a frame then the problem isn't here, but rather in the
     caller.  It should of first created a frame and then passed that
     in.  */
  /* NOTE: cagney/2002-10-31: As a side bar, keep in mind that the
     ``current_frame'' should not be treated as a special case.  While
     ``get_next_frame (current_frame) == NULL'' currently holds, it
     should, as far as possible, not be relied upon.  In the future,
     ``get_next_frame (current_frame)'' may instead simply return a
     normal frame object that simply always gets register values from
     the register cache.  Consequently, frame code should try to avoid
     tests like ``if get_next_frame() == NULL'' and instead just rely
     on recursive frame calls (like the below code) when manipulating
     a frame chain.  */
  gdb_assert (frame != NULL);
  frame_unwind_unsigned_register (get_next_frame (frame), regnum, val);
}

void
frame_read_signed_register (struct frame_info *frame, int regnum,
			    LONGEST *val)
{
  /* See note in frame_read_unsigned_register().  */
  gdb_assert (frame != NULL);
  frame_unwind_signed_register (get_next_frame (frame), regnum, val);
}

static void
generic_unwind_get_saved_register (char *raw_buffer,
				   int *optimizedp,
				   CORE_ADDR *addrp,
				   struct frame_info *frame,
				   int regnum,
				   enum lval_type *lvalp)
{
  int optimizedx;
  CORE_ADDR addrx;
  int realnumx;
  enum lval_type lvalx;

  if (!target_has_registers)
    error ("No registers.");

  /* Keep things simple, ensure that all the pointers (except valuep)
     are non NULL.  */
  if (optimizedp == NULL)
    optimizedp = &optimizedx;
  if (lvalp == NULL)
    lvalp = &lvalx;
  if (addrp == NULL)
    addrp = &addrx;

  /* Reached the the bottom (youngest, inner most) of the frame chain
     (youngest, inner most) frame, go direct to the hardware register
     cache (do not pass go, do not try to cache the value, ...).  The
     unwound value would have been cached in frame->next but that
     doesn't exist.  This doesn't matter as the hardware register
     cache is stopping any unnecessary accesses to the target.  */

  /* NOTE: cagney/2002-04-14: It would be nice if, instead of a
     special case, there was always an inner frame dedicated to the
     hardware registers.  Unfortunatly, there is too much unwind code
     around that looks up/down the frame chain while making the
     assumption that each frame level is using the same unwind code.  */

  if (frame == NULL)
    frame_register_unwind (NULL, regnum, optimizedp, lvalp, addrp, &realnumx,
			   raw_buffer);
  else
    frame_register_unwind (frame->next, regnum, optimizedp, lvalp, addrp,
			   &realnumx, raw_buffer);
}

void
get_saved_register (char *raw_buffer,
		    int *optimized,
		    CORE_ADDR *addrp,
		    struct frame_info *frame,
		    int regnum,
		    enum lval_type *lval)
{
  if (GET_SAVED_REGISTER_P ())
    {
      GET_SAVED_REGISTER (raw_buffer, optimized, addrp, frame, regnum, lval);
      return;
    }
  generic_unwind_get_saved_register (raw_buffer, optimized, addrp, frame,
				     regnum, lval);
}

/* frame_register_read ()

   Find and return the value of REGNUM for the specified stack frame.
   The number of bytes copied is REGISTER_RAW_SIZE (REGNUM).

   Returns 0 if the register value could not be found.  */

int
frame_register_read (struct frame_info *frame, int regnum, void *myaddr)
{
  int optimized;
  enum lval_type lval;
  CORE_ADDR addr;
  int realnum;
  frame_register (frame, regnum, &optimized, &lval, &addr, &realnum, myaddr);

  /* FIXME: cagney/2002-05-15: This test, is just bogus.

     It indicates that the target failed to supply a value for a
     register because it was "not available" at this time.  Problem
     is, the target still has the register and so get saved_register()
     may be returning a value saved on the stack.  */

  if (register_cached (regnum) < 0)
    return 0;			/* register value not available */

  return !optimized;
}


/* Map between a frame register number and its name.  A frame register
   space is a superset of the cooked register space --- it also
   includes builtin registers.  */

int
frame_map_name_to_regnum (const char *name, int len)
{
  int i;

  /* Search register name space. */
  for (i = 0; i < NUM_REGS + NUM_PSEUDO_REGS; i++)
    if (REGISTER_NAME (i) && len == strlen (REGISTER_NAME (i))
	&& strncmp (name, REGISTER_NAME (i), len) == 0)
      {
	return i;
      }

  /* Try builtin registers.  */
  i = builtin_reg_map_name_to_regnum (name, len);
  if (i >= 0)
    {
      /* A builtin register doesn't fall into the architecture's
         register range.  */
      gdb_assert (i >= NUM_REGS + NUM_PSEUDO_REGS);
      return i;
    }

  return -1;
}

const char *
frame_map_regnum_to_name (int regnum)
{
  if (regnum < 0)
    return NULL;
  if (regnum < NUM_REGS + NUM_PSEUDO_REGS)
    return REGISTER_NAME (regnum);
  return builtin_reg_map_regnum_to_name (regnum);
}

/* Info about the innermost stack frame (contents of FP register) */

static struct frame_info *current_frame;

/* Cache for frame addresses already read by gdb.  Valid only while
   inferior is stopped.  Control variables for the frame cache should
   be local to this module.  */

static struct obstack frame_cache_obstack;

void *
frame_obstack_alloc (unsigned long size)
{
  return obstack_alloc (&frame_cache_obstack, size);
}

void
frame_saved_regs_zalloc (struct frame_info *fi)
{
  fi->saved_regs = (CORE_ADDR *)
    frame_obstack_alloc (SIZEOF_FRAME_SAVED_REGS);
  memset (fi->saved_regs, 0, SIZEOF_FRAME_SAVED_REGS);
}


/* Return the innermost (currently executing) stack frame.  */

struct frame_info *
get_current_frame (void)
{
  if (current_frame == NULL)
    {
      if (target_has_stack)
	current_frame = create_new_frame (read_fp (), read_pc ());
      else
	error ("No stack.");
    }
  return current_frame;
}

void
set_current_frame (struct frame_info *frame)
{
  current_frame = frame;
}

/* Return the register saved in the simplistic ``saved_regs'' cache.
   If the value isn't here AND a value is needed, try the next inner
   most frame.  */

static void
frame_saved_regs_register_unwind (struct frame_info *frame, void **cache,
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
  gdb_assert (!(USE_GENERIC_DUMMY_FRAMES
                && PC_IN_CALL_DUMMY (frame->pc, frame->frame, frame->frame)));

  /* Load the saved_regs register cache.  */
  if (frame->saved_regs == NULL)
    FRAME_INIT_SAVED_REGS (frame);

  if (frame->saved_regs != NULL
      && frame->saved_regs[regnum] != 0)
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
			   frame->saved_regs[regnum]);
	}
      else
	{
	  /* Any other register is saved in memory, fetch it but cache
             a local copy of its value.  */
	  *optimizedp = 0;
	  *lvalp = lval_memory;
	  *addrp = frame->saved_regs[regnum];
	  *realnump = -1;
	  if (bufferp != NULL)
	    {
#if 1
	      /* Save each register value, as it is read in, in a
                 frame based cache.  */
	      void **regs = (*cache);
	      if (regs == NULL)
		{
		  int sizeof_cache = ((NUM_REGS + NUM_PSEUDO_REGS)
				      * sizeof (void *));
		  regs = frame_obstack_alloc (sizeof_cache);
		  memset (regs, 0, sizeof_cache);
		  (*cache) = regs;
		}
	      if (regs[regnum] == NULL)
		{
		  regs[regnum]
		    = frame_obstack_alloc (REGISTER_RAW_SIZE (regnum));
		  read_memory (frame->saved_regs[regnum], regs[regnum],
			       REGISTER_RAW_SIZE (regnum));
		}
	      memcpy (bufferp, regs[regnum], REGISTER_RAW_SIZE (regnum));
#else
	      /* Read the value in from memory.  */
	      read_memory (frame->saved_regs[regnum], bufferp,
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
      *addrp = 0;
      *realnump = regnum;
    }
  else
    {
      frame_register_unwind (frame->next, regnum, optimizedp, lvalp, addrp,
			     realnump, bufferp);
    }
}

static CORE_ADDR
frame_saved_regs_pc_unwind (struct frame_info *frame, void **cache)
{
  return FRAME_SAVED_PC (frame);
}
	
/* Function: get_saved_register
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

  while (frame && ((frame = frame->next) != NULL))
    {
      if (PC_IN_CALL_DUMMY (frame->pc, frame->frame, frame->frame))
	{
	  if (lval)		/* found it in a CALL_DUMMY frame */
	    *lval = not_lval;
	  if (raw_buffer)
	    /* FIXME: cagney/2002-06-26: This should be via the
	       gdbarch_register_read() method so that it, on the fly,
	       constructs either a raw or pseudo register from the raw
	       register cache.  */
	    regcache_raw_read (generic_find_dummy_frame (frame->pc,
							 frame->frame),
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

/* Using the PC, select a mechanism for unwinding a frame returning
   the previous frame.  The register unwind function should, on
   demand, initialize the ->context object.  */

static void
set_unwind_by_pc (CORE_ADDR pc, CORE_ADDR fp,
		  frame_register_unwind_ftype **unwind_register,
		  frame_pc_unwind_ftype **unwind_pc)
{
  if (!USE_GENERIC_DUMMY_FRAMES)
    {
      /* Still need to set this to something.  The ``info frame'' code
	 calls this function to find out where the saved registers are.
	 Hopefully this is robust enough to stop any core dumps and
	 return vaguely correct values..  */
      *unwind_register = frame_saved_regs_register_unwind;
      *unwind_pc = frame_saved_regs_pc_unwind;
    }
  else if (PC_IN_CALL_DUMMY (pc, fp, fp))
    {
      *unwind_register = dummy_frame_register_unwind;
      *unwind_pc = dummy_frame_pc_unwind;
    }
  else
    {
      *unwind_register = frame_saved_regs_register_unwind;
      *unwind_pc = frame_saved_regs_pc_unwind;
    }
}

/* Create an arbitrary (i.e. address specified by user) or innermost frame.
   Always returns a non-NULL value.  */

struct frame_info *
create_new_frame (CORE_ADDR addr, CORE_ADDR pc)
{
  struct frame_info *fi;
  enum frame_type type;

  fi = (struct frame_info *)
    obstack_alloc (&frame_cache_obstack,
		   sizeof (struct frame_info));

  /* Zero all fields by default.  */
  memset (fi, 0, sizeof (struct frame_info));

  fi->frame = addr;
  fi->pc = pc;
  /* NOTE: cagney/2002-11-18: The code segments, found in
     create_new_frame and get_prev_frame(), that initializes the
     frames type is subtly different.  The latter only updates ->type
     when it encounters a SIGTRAMP_FRAME or DUMMY_FRAME.  This stops
     get_prev_frame() overriding the frame's type when the INIT code
     has previously set it.  This is really somewhat bogus.  The
     initialization, as seen in create_new_frame(), should occur
     before the INIT function has been called.  */
  if (USE_GENERIC_DUMMY_FRAMES && PC_IN_CALL_DUMMY (fi->pc, 0, 0))
    /* NOTE: cagney/2002-11-11: Does this even occure?  */
    type = DUMMY_FRAME;
  else
    {
      char *name;
      find_pc_partial_function (pc, &name, NULL, NULL);
      if (PC_IN_SIGTRAMP (fi->pc, name))
	type = SIGTRAMP_FRAME;
      else
	type = NORMAL_FRAME;
    }
  fi->type = type;

  if (INIT_EXTRA_FRAME_INFO_P ())
    INIT_EXTRA_FRAME_INFO (0, fi);

  /* Select/initialize an unwind function.  */
  set_unwind_by_pc (fi->pc, fi->frame, &fi->register_unwind,
		    &fi->pc_unwind);

  return fi;
}

/* Return the frame that FRAME calls (NULL if FRAME is the innermost
   frame).  */

struct frame_info *
get_next_frame (struct frame_info *frame)
{
  return frame->next;
}

/* Flush the entire frame cache.  */

void
flush_cached_frames (void)
{
  /* Since we can't really be sure what the first object allocated was */
  obstack_free (&frame_cache_obstack, 0);
  obstack_init (&frame_cache_obstack);

  current_frame = NULL;		/* Invalidate cache */
  select_frame (NULL);
  annotate_frames_invalid ();
}

/* Flush the frame cache, and start a new one if necessary.  */

void
reinit_frame_cache (void)
{
  flush_cached_frames ();

  /* FIXME: The inferior_ptid test is wrong if there is a corefile.  */
  if (PIDGET (inferior_ptid) != 0)
    {
      select_frame (get_current_frame ());
    }
}

/* Return a structure containing various interesting information
   about the frame that called NEXT_FRAME.  Returns NULL
   if there is no such frame.  */

struct frame_info *
get_prev_frame (struct frame_info *next_frame)
{
  CORE_ADDR address = 0;
  struct frame_info *prev;
  int fromleaf;

  /* Return the inner-most frame, when the caller passes in NULL.  */
  /* NOTE: cagney/2002-11-09: Not sure how this would happen.  The
     caller should have previously obtained a valid frame using
     get_selected_frame() and then called this code - only possibility
     I can think of is code behaving badly.  */
  if (next_frame == NULL)
    {
      /* NOTE: cagney/2002-11-09: There was a code segment here that
	 would error out when CURRENT_FRAME was NULL.  The comment
	 that went with it made the claim ...

	 ``This screws value_of_variable, which just wants a nice
	 clean NULL return from block_innermost_frame if there are no
	 frames.  I don't think I've ever seen this message happen
	 otherwise.  And returning NULL here is a perfectly legitimate
	 thing to do.''

         Per the above, this code shouldn't even be called with a NULL
         NEXT_FRAME.  */
      return current_frame;
    }

  /* Only try to do the unwind once.  */
  if (next_frame->prev_p)
    return next_frame->prev;
  next_frame->prev_p = 1;

  /* On some machines it is possible to call a function without
     setting up a stack frame for it.  On these machines, we
     define this macro to take two args; a frameinfo pointer
     identifying a frame and a variable to set or clear if it is
     or isn't leafless.  */

  /* Still don't want to worry about this except on the innermost
     frame.  This macro will set FROMLEAF if NEXT_FRAME is a frameless
     function invocation.  */
  if (next_frame->next == NULL)
    /* FIXME: 2002-11-09: Frameless functions can occure anywhere in
       the frame chain, not just the inner most frame!  The generic,
       per-architecture, frame code should handle this and the below
       should simply be removed.  */
    fromleaf = FRAMELESS_FUNCTION_INVOCATION (next_frame);
  else
    fromleaf = 0;

  if (fromleaf)
    /* A frameless inner-most frame.  The `FP' (which isn't an
       architecture frame-pointer register!) of the caller is the same
       as the callee.  */
    /* FIXME: 2002-11-09: There isn't any reason to special case this
       edge condition.  Instead the per-architecture code should hande
       it locally.  */
    address = FRAME_FP (next_frame);
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
      address = FRAME_CHAIN (next_frame);

      /* FIXME: cagney/2002-06-08: There should be two tests here.
         The first would check for a valid frame chain based on a user
         selectable policy.  The default being ``stop at main'' (as
         implemented by generic_func_frame_chain_valid()).  Other
         policies would be available - stop at NULL, ....  The second
         test, if provided by the target architecture, would check for
         more exotic cases - most target architectures wouldn't bother
         with this second case.  */
      if (!FRAME_CHAIN_VALID (address, next_frame))
	return 0;
    }
  if (address == 0)
    return 0;

  /* Create an initially zero previous frame.  */
  prev = (struct frame_info *)
    obstack_alloc (&frame_cache_obstack,
		   sizeof (struct frame_info));
  memset (prev, 0, sizeof (struct frame_info));

  /* Link it in.  */
  next_frame->prev = prev;
  prev->next = next_frame;
  prev->frame = address;
  prev->level = next_frame->level + 1;
  /* FIXME: cagney/2002-11-18: Should be setting the frame's type
     here, before anything else, and not last.  Various INIT functions
     are full of work-arounds for the frames type not being set
     correctly from the word go.  Ulgh!  */
  prev->type = NORMAL_FRAME;

  /* This change should not be needed, FIXME!  We should determine
     whether any targets *need* INIT_FRAME_PC to happen after
     INIT_EXTRA_FRAME_INFO and come up with a simple way to express
     what goes on here.

     INIT_EXTRA_FRAME_INFO is called from two places: create_new_frame
     (where the PC is already set up) and here (where it isn't).
     INIT_FRAME_PC is only called from here, always after
     INIT_EXTRA_FRAME_INFO.

     The catch is the MIPS, where INIT_EXTRA_FRAME_INFO requires the
     PC value (which hasn't been set yet).  Some other machines appear
     to require INIT_EXTRA_FRAME_INFO before they can do
     INIT_FRAME_PC.  Phoo.

     We shouldn't need INIT_FRAME_PC_FIRST to add more complication to
     an already overcomplicated part of GDB.  gnu@cygnus.com, 15Sep92.

     Assuming that some machines need INIT_FRAME_PC after
     INIT_EXTRA_FRAME_INFO, one possible scheme:

     SETUP_INNERMOST_FRAME(): Default version is just create_new_frame
     (read_fp ()), read_pc ()).  Machines with extra frame info would
     do that (or the local equivalent) and then set the extra fields.

     SETUP_ARBITRARY_FRAME(argc, argv): Only change here is that
     create_new_frame would no longer init extra frame info;
     SETUP_ARBITRARY_FRAME would have to do that.

     INIT_PREV_FRAME(fromleaf, prev) Replace INIT_EXTRA_FRAME_INFO and
     INIT_FRAME_PC.  This should also return a flag saying whether to
     keep the new frame, or whether to discard it, because on some
     machines (e.g.  mips) it is really awkward to have
     FRAME_CHAIN_VALID called *before* INIT_EXTRA_FRAME_INFO (there is
     no good way to get information deduced in FRAME_CHAIN_VALID into
     the extra fields of the new frame).  std_frame_pc(fromleaf, prev)

     This is the default setting for INIT_PREV_FRAME.  It just does
     what the default INIT_FRAME_PC does.  Some machines will call it
     from INIT_PREV_FRAME (either at the beginning, the end, or in the
     middle).  Some machines won't use it.

     kingdon@cygnus.com, 13Apr93, 31Jan94, 14Dec94.  */

  /* NOTE: cagney/2002-11-09: Just ignore the above!  There is no
     reason for things to be this complicated.

     The trick is to assume that there is always a frame.  Instead of
     special casing the inner-most frame, create fake frame
     (containing the hardware registers) that is inner to the
     user-visible inner-most frame (...) and then unwind from that.
     That way architecture code can use use the standard
     frame_XX_unwind() functions and not differentiate between the
     inner most and any other case.

     Since there is always a frame to unwind from, there is always
     somewhere (NEXT_FRAME) to store all the info needed to construct
     a new (previous) frame without having to first create it.  This
     means that the convolution below - needing to carefully order a
     frame's initialization - isn't needed.

     The irony here though, is that FRAME_CHAIN(), at least for a more
     up-to-date architecture, always calls FRAME_SAVED_PC(), and
     FRAME_SAVED_PC() computes the PC but without first needing the
     frame!  Instead of the convolution below, we could have simply
     called FRAME_SAVED_PC() and been done with it!  Note that
     FRAME_SAVED_PC() is being superseed by frame_pc_unwind() and that
     function does have somewhere to cache that PC value.  */

  INIT_FRAME_PC_FIRST (fromleaf, prev);

  if (INIT_EXTRA_FRAME_INFO_P ())
    INIT_EXTRA_FRAME_INFO (fromleaf, prev);

  /* This entry is in the frame queue now, which is good since
     FRAME_SAVED_PC may use that queue to figure out its value (see
     tm-sparc.h).  We want the pc saved in the inferior frame. */
  INIT_FRAME_PC (fromleaf, prev);

  /* If ->frame and ->pc are unchanged, we are in the process of
     getting ourselves into an infinite backtrace.  Some architectures
     check this in FRAME_CHAIN or thereabouts, but it seems like there
     is no reason this can't be an architecture-independent check.  */
  if (prev->frame == next_frame->frame
      && prev->pc == next_frame->pc)
    {
      next_frame->prev = NULL;
      obstack_free (&frame_cache_obstack, prev);
      return NULL;
    }

  /* Initialize the code used to unwind the frame PREV based on the PC
     (and probably other architectural information).  The PC lets you
     check things like the debug info at that point (dwarf2cfi?) and
     use that to decide how the frame should be unwound.  */
  set_unwind_by_pc (prev->pc, prev->frame, &prev->register_unwind,
		    &prev->pc_unwind);

  /* NOTE: cagney/2002-11-18: The code segments, found in
     create_new_frame and get_prev_frame(), that initializes the
     frames type is subtly different.  The latter only updates ->type
     when it encounters a SIGTRAMP_FRAME or DUMMY_FRAME.  This stops
     get_prev_frame() overriding the frame's type when the INIT code
     has previously set it.  This is really somewhat bogus.  The
     initialization, as seen in create_new_frame(), should occur
     before the INIT function has been called.  */
  if (USE_GENERIC_DUMMY_FRAMES
      && PC_IN_CALL_DUMMY (prev->pc, 0, 0))
    prev->type = DUMMY_FRAME;
  else
    {
      /* FIXME: cagney/2002-11-10: This should be moved to before the
	 INIT code above so that the INIT code knows what the frame's
	 type is (in fact, for a [generic] dummy-frame, the type can
	 be set and then the entire initialization can be skipped.
	 Unforunatly, its the INIT code that sets the PC (Hmm, catch
	 22).  */
      char *name;
      find_pc_partial_function (prev->pc, &name, NULL, NULL);
      if (PC_IN_SIGTRAMP (prev->pc, name))
	prev->type = SIGTRAMP_FRAME;
      /* FIXME: cagney/2002-11-11: Leave prev->type alone.  Some
         architectures are forcing the frame's type in INIT so we
         don't want to override it here.  Remember, NORMAL_FRAME == 0,
         so it all works (just :-/).  Once this initialization is
         moved to the start of this function, all this nastness will
         go away.  */
    }

  return prev;
}

CORE_ADDR
get_frame_pc (struct frame_info *frame)
{
  return frame->pc;
}

/* Level of the selected frame: 0 for innermost, 1 for its caller, ...
   or -1 for a NULL frame.  */

int
frame_relative_level (struct frame_info *fi)
{
  if (fi == NULL)
    return -1;
  else
    return fi->level;
}

enum frame_type
get_frame_type (struct frame_info *frame)
{
  /* Some targets still don't use [generic] dummy frames.  Catch them
     here.  */
  if (!USE_GENERIC_DUMMY_FRAMES
      && deprecated_frame_in_dummy (frame))
    return DUMMY_FRAME;
  return frame->type;
}

void
deprecated_set_frame_type (struct frame_info *frame, enum frame_type type)
{
  /* Arrrg!  See comment in "frame.h".  */
  frame->type = type;
}

#ifdef FRAME_FIND_SAVED_REGS
/* XXX - deprecated.  This is a compatibility function for targets
   that do not yet implement FRAME_INIT_SAVED_REGS.  */
/* Find the addresses in which registers are saved in FRAME.  */

void
get_frame_saved_regs (struct frame_info *frame,
		      struct frame_saved_regs *saved_regs_addr)
{
  if (frame->saved_regs == NULL)
    {
      frame->saved_regs = (CORE_ADDR *)
	frame_obstack_alloc (SIZEOF_FRAME_SAVED_REGS);
    }
  if (saved_regs_addr == NULL)
    {
      struct frame_saved_regs saved_regs;
      FRAME_FIND_SAVED_REGS (frame, saved_regs);
      memcpy (frame->saved_regs, &saved_regs, SIZEOF_FRAME_SAVED_REGS);
    }
  else
    {
      FRAME_FIND_SAVED_REGS (frame, *saved_regs_addr);
      memcpy (frame->saved_regs, saved_regs_addr, SIZEOF_FRAME_SAVED_REGS);
    }
}
#endif

void
_initialize_frame (void)
{
  obstack_init (&frame_cache_obstack);
}
