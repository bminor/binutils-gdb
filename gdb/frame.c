/* Cache and manage frames for GDB, the GNU debugger.

   Copyright 1986, 1987, 1989, 1991, 1994, 1995, 1996, 1998, 2000,
   2001, 2002, 2003 Free Software Foundation, Inc.

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
#include "sentinel-frame.h"
#include "gdbcore.h"
#include "annotate.h"
#include "language.h"
#include "frame-unwind.h"
#include "command.h"
#include "gdbcmd.h"

/* Flag to indicate whether backtraces should stop at main.  */

static int backtrace_below_main;

/* Return a frame uniq ID that can be used to, later, re-find the
   frame.  */

struct frame_id
get_frame_id (struct frame_info *fi)
{
  if (fi == NULL)
    {
      return null_frame_id;
    }
  else
    {
      struct frame_id id;
      id.base = fi->frame;
      id.pc = fi->pc;
      return id;
    }
}

const struct frame_id null_frame_id; /* All zeros.  */

struct frame_id
frame_id_build (CORE_ADDR base, CORE_ADDR func_or_pc)
{
  struct frame_id id;
  id.base = base;
  id.pc = func_or_pc;
  return id;
}

int
frame_id_p (struct frame_id l)
{
  /* The .func can be NULL but the .base cannot.  */
  return (l.base != 0);
}

int
frame_id_eq (struct frame_id l, struct frame_id r)
{
  /* If .base is different, the frames are different.  */
  if (l.base != r.base)
    return 0;
  /* Add a test to check that the frame ID's are for the same function
     here.  */
  return 1;
}

int
frame_id_inner (struct frame_id l, struct frame_id r)
{
  /* Only return non-zero when strictly inner than.  Note that, per
     comment in "frame.h", there is some fuzz here.  Frameless
     functions are not strictly inner than (same .base but different
     .func).  */
  return INNER_THAN (l.base, r.base);
}

struct frame_info *
frame_find_by_id (struct frame_id id)
{
  struct frame_info *frame;

  /* ZERO denotes the null frame, let the caller decide what to do
     about it.  Should it instead return get_current_frame()?  */
  if (!frame_id_p (id))
    return NULL;

  for (frame = get_current_frame ();
       frame != NULL;
       frame = get_prev_frame (frame))
    {
      struct frame_id this = get_frame_id (frame);
      if (frame_id_eq (id, this))
	/* An exact match.  */
	return frame;
      if (frame_id_inner (id, this))
	/* Gone to far.  */
	return NULL;
      /* Either, we're not yet gone far enough out along the frame
         chain (inner(this,id), or we're comparing frameless functions
         (same .base, different .func, no test available).  Struggle
         on until we've definitly gone to far.  */
    }
  return NULL;
}

CORE_ADDR
frame_pc_unwind (struct frame_info *frame)
{
  if (!frame->pc_unwind_cache_p)
    {
      frame->pc_unwind_cache = frame->unwind->pc (frame, &frame->unwind_cache);
      frame->pc_unwind_cache_p = 1;
    }
  return frame->pc_unwind_cache;
}

struct frame_id
frame_id_unwind (struct frame_info *frame)
{
  if (!frame->id_unwind_cache_p)
    {
      frame->unwind->id (frame, &frame->unwind_cache, &frame->id_unwind_cache);
      frame->id_unwind_cache_p = 1;
    }
  return frame->id_unwind_cache;
}

void
frame_pop (struct frame_info *frame)
{
  /* FIXME: cagney/2003-01-18: There is probably a chicken-egg problem
     with passing in current_regcache.  The pop function needs to be
     written carefully so as to not overwrite registers whose [old]
     values are needed to restore other registers.  Instead, this code
     should pass in a scratch cache and, as a second step, restore the
     registers using that.  */
  frame->unwind->pop (frame, &frame->unwind_cache, current_regcache);
  flush_cached_frames ();
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

  /* NOTE: cagney/2002-11-27: A program trying to unwind a NULL frame
     is broken.  There is always a frame.  If there, for some reason,
     isn't, there is some pretty busted code as it should have
     detected the problem before calling here.  */
  gdb_assert (frame != NULL);

  /* Ask this frame to unwind its register.  */
  frame->unwind->reg (frame, &frame->unwind_cache, regnum,
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

  /* Obtain the register value by unwinding the register from the next
     (more inner frame).  */
  gdb_assert (frame != NULL && frame->next != NULL);
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
  gdb_assert (frame != NULL && frame->next != NULL);
  frame_unwind_unsigned_register (frame->next, regnum, val);
}

void
frame_read_signed_register (struct frame_info *frame, int regnum,
			    LONGEST *val)
{
  /* See note above in frame_read_unsigned_register().  */
  gdb_assert (frame != NULL && frame->next != NULL);
  frame_unwind_signed_register (frame->next, regnum, val);
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

  gdb_assert (frame != NULL && frame->next != NULL);
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

/* Create a sentinel frame.  */

struct frame_info *
create_sentinel_frame (struct regcache *regcache)
{
  struct frame_info *frame = FRAME_OBSTACK_ZALLOC (struct frame_info);
  frame->type = NORMAL_FRAME;
  frame->level = -1;
  /* Explicitly initialize the sentinel frame's cache.  Provide it
     with the underlying regcache.  In the future additional
     information, such as the frame's thread will be added.  */
  frame->unwind_cache = sentinel_frame_cache (regcache);
  /* For the moment there is only one sentinel frame implementation.  */
  frame->unwind = sentinel_frame_unwind;
  /* Link this frame back to itself.  The frame is self referential
     (the unwound PC is the same as the pc), so make it so.  */
  frame->next = frame;
  /* Always unwind the PC as part of creating this frame.  This
     ensures that the frame's PC points at something valid.  */
  /* FIXME: cagney/2003-01-10: Problem here.  Unwinding a sentinel
     frame's PC may require information such as the frame's thread's
     stop reason.  Is it possible to get to that?  */
  frame->pc = frame_pc_unwind (frame);
  return frame;
}

/* Info about the innermost stack frame (contents of FP register) */

static struct frame_info *current_frame;

/* Cache for frame addresses already read by gdb.  Valid only while
   inferior is stopped.  Control variables for the frame cache should
   be local to this module.  */

static struct obstack frame_cache_obstack;

void *
frame_obstack_zalloc (unsigned long size)
{
  void *data = obstack_alloc (&frame_cache_obstack, size);
  memset (data, 0, size);
  return data;
}

CORE_ADDR *
frame_saved_regs_zalloc (struct frame_info *fi)
{
  fi->saved_regs = (CORE_ADDR *)
    frame_obstack_zalloc (SIZEOF_FRAME_SAVED_REGS);
  return fi->saved_regs;
}

CORE_ADDR *
get_frame_saved_regs (struct frame_info *fi)
{
  return fi->saved_regs;
}

/* Return the innermost (currently executing) stack frame.  This is
   split into two functions.  The function unwind_to_current_frame()
   is wrapped in catch exceptions so that, even when the unwind of the
   sentinel frame fails, the function still returns a stack frame.  */

static int
unwind_to_current_frame (struct ui_out *ui_out, void *args)
{
  struct frame_info *frame = get_prev_frame (args);
  /* A sentinel frame can fail to unwind, eg, because it's PC value
     lands in somewhere like start.  */
  if (frame == NULL)
    return 1;
  current_frame = frame;
  return 0;
}

struct frame_info *
get_current_frame (void)
{
  if (!target_has_stack)
    error ("No stack.");
  if (!target_has_registers)
    error ("No registers.");
  if (!target_has_memory)
    error ("No memory.");
  if (current_frame == NULL)
    {
      struct frame_info *sentinel_frame =
	create_sentinel_frame (current_regcache);
      if (catch_exceptions (uiout, unwind_to_current_frame, sentinel_frame,
			    NULL, RETURN_MASK_ERROR) != 0)
	{
	  /* Oops! Fake a current frame?  Is this useful?  It has a PC
             of zero, for instance.  */
	  current_frame = sentinel_frame;
	}
    }
  return current_frame;
}

/* The "selected" stack frame is used by default for local and arg
   access.  May be zero, for no selected frame.  */

struct frame_info *deprecated_selected_frame;

/* Return the selected frame.  Always non-null (unless there isn't an
   inferior sufficient for creating a frame) in which case an error is
   thrown.  */

struct frame_info *
get_selected_frame (void)
{
  if (deprecated_selected_frame == NULL)
    /* Hey!  Don't trust this.  It should really be re-finding the
       last selected frame of the currently selected thread.  This,
       though, is better than nothing.  */
    select_frame (get_current_frame ());
  /* There is always a frame.  */
  gdb_assert (deprecated_selected_frame != NULL);
  return deprecated_selected_frame;
}

/* Select frame FI (or NULL - to invalidate the current frame).  */

void
select_frame (struct frame_info *fi)
{
  register struct symtab *s;

  deprecated_selected_frame = fi;
  /* NOTE: cagney/2002-05-04: FI can be NULL.  This occures when the
     frame is being invalidated.  */
  if (selected_frame_level_changed_hook)
    selected_frame_level_changed_hook (frame_relative_level (fi));

  /* FIXME: kseitz/2002-08-28: It would be nice to call
     selected_frame_level_changed_event right here, but due to limitations
     in the current interfaces, we would end up flooding UIs with events
     because select_frame is used extensively internally.

     Once we have frame-parameterized frame (and frame-related) commands,
     the event notification can be moved here, since this function will only
     be called when the users selected frame is being changed. */

  /* Ensure that symbols for this frame are read in.  Also, determine the
     source language of this frame, and switch to it if desired.  */
  if (fi)
    {
      s = find_pc_symtab (fi->pc);
      if (s
	  && s->language != current_language->la_language
	  && s->language != language_unknown
	  && language_mode == language_mode_auto)
	{
	  set_language (s->language);
	}
    }
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
  gdb_assert (!(DEPRECATED_USE_GENERIC_DUMMY_FRAMES
		&& (get_frame_type (frame) == DUMMY_FRAME)));

  /* Only (older) architectures that implement the
     FRAME_INIT_SAVED_REGS method should be using this function.  */
  gdb_assert (FRAME_INIT_SAVED_REGS_P ());

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
	      void **regs = (*cache);
	      if (regs == NULL)
		{
		  int sizeof_cache = ((NUM_REGS + NUM_PSEUDO_REGS)
				      * sizeof (void *));
		  regs = frame_obstack_zalloc (sizeof_cache);
		  (*cache) = regs;
		}
	      if (regs[regnum] == NULL)
		{
		  regs[regnum]
		    = frame_obstack_zalloc (REGISTER_RAW_SIZE (regnum));
		  read_memory (get_frame_saved_regs (frame)[regnum], regs[regnum],
			       REGISTER_RAW_SIZE (regnum));
		}
	      memcpy (bufferp, regs[regnum], REGISTER_RAW_SIZE (regnum));
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
     value.  Pass the request down the frame chain to the next frame.
     Hopefully that will find the register's location, either in a
     register or in memory.  */
  frame_register (frame, regnum, optimizedp, lvalp, addrp, realnump,
		  bufferp);
}

static CORE_ADDR
frame_saved_regs_pc_unwind (struct frame_info *frame, void **cache)
{
  gdb_assert (FRAME_SAVED_PC_P ());
  return FRAME_SAVED_PC (frame);
}
	
static void
frame_saved_regs_id_unwind (struct frame_info *next_frame, void **cache,
			    struct frame_id *id)
{
  int fromleaf;
  CORE_ADDR base;
  CORE_ADDR pc;

  /* Start out by assuming it's NULL.  */
  (*id) = null_frame_id;

  if (frame_relative_level (next_frame) <= 0)
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
    base = get_frame_base (next_frame);
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
      gdb_assert (FRAME_CHAIN_P ());
      base = FRAME_CHAIN (next_frame);

      if (!frame_chain_valid (base, next_frame))
	return;
    }
  if (base == 0)
    return;

  /* FIXME: cagney/2002-06-08: This should probably return the frame's
     function and not the PC (a.k.a. resume address).  */
  pc = frame_pc_unwind (next_frame);
  id->pc = pc;
  id->base = base;
}
	
static void
frame_saved_regs_pop (struct frame_info *fi, void **cache,
		      struct regcache *regcache)
{
  gdb_assert (POP_FRAME_P ());
  POP_FRAME;
}

const struct frame_unwind trad_frame_unwinder = {
  frame_saved_regs_pop,
  frame_saved_regs_pc_unwind,
  frame_saved_regs_id_unwind,
  frame_saved_regs_register_unwind
};
const struct frame_unwind *trad_frame_unwind = &trad_frame_unwinder;


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

  gdb_assert (FRAME_INIT_SAVED_REGS_P ());

  /* Normal systems don't optimize out things with register numbers.  */
  if (optimized != NULL)
    *optimized = 0;

  if (addrp)			/* default assumption: not found in memory */
    *addrp = 0;

  /* Note: since the current frame's registers could only have been
     saved by frames INTERIOR TO the current frame, we skip examining
     the current frame itself: otherwise, we would be getting the
     previous frame's registers which were saved by the current frame.  */

  if (frame != NULL)
    {
      for (frame = get_next_frame (frame);
	   frame_relative_level (frame) >= 0;
	   frame = get_next_frame (frame))
	{
	  if (get_frame_type (frame) == DUMMY_FRAME)
	    {
	      if (lval)		/* found it in a CALL_DUMMY frame */
		*lval = not_lval;
	      if (raw_buffer)
		/* FIXME: cagney/2002-06-26: This should be via the
		   gdbarch_register_read() method so that it, on the
		   fly, constructs either a raw or pseudo register
		   from the raw register cache.  */
		regcache_raw_read
		  (generic_find_dummy_frame (get_frame_pc (frame),
					     get_frame_base (frame)),
		   regnum, raw_buffer);
	      return;
	    }

	  FRAME_INIT_SAVED_REGS (frame);
	  if (get_frame_saved_regs (frame) != NULL
	      && get_frame_saved_regs (frame)[regnum] != 0)
	    {
	      if (lval)		/* found it saved on the stack */
		*lval = lval_memory;
	      if (regnum == SP_REGNUM)
		{
		  if (raw_buffer)	/* SP register treated specially */
		    store_address (raw_buffer, REGISTER_RAW_SIZE (regnum),
				   get_frame_saved_regs (frame)[regnum]);
		}
	      else
		{
		  if (addrp)	/* any other register */
		    *addrp = get_frame_saved_regs (frame)[regnum];
		  if (raw_buffer)
		    read_memory (get_frame_saved_regs (frame)[regnum], raw_buffer,
				 REGISTER_RAW_SIZE (regnum));
		}
	      return;
	    }
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

/* Determine the frame's type based on its PC.  */

static enum frame_type
frame_type_from_pc (CORE_ADDR pc)
{
  /* FIXME: cagney/2002-11-24: Can't yet directly call
     pc_in_dummy_frame() as some architectures don't set
     PC_IN_CALL_DUMMY() to generic_pc_in_call_dummy() (remember the
     latter is implemented by simply calling pc_in_dummy_frame).  */
  if (DEPRECATED_USE_GENERIC_DUMMY_FRAMES
      && DEPRECATED_PC_IN_CALL_DUMMY (pc, 0, 0))
    return DUMMY_FRAME;
  else
    {
      char *name;
      find_pc_partial_function (pc, &name, NULL, NULL);
      if (PC_IN_SIGTRAMP (pc, name))
	return SIGTRAMP_FRAME;
      else
	return NORMAL_FRAME;
    }
}

/* Create an arbitrary (i.e. address specified by user) or innermost frame.
   Always returns a non-NULL value.  */

struct frame_info *
create_new_frame (CORE_ADDR addr, CORE_ADDR pc)
{
  struct frame_info *fi;

  fi = frame_obstack_zalloc (sizeof (struct frame_info));

  fi->frame = addr;
  fi->pc = pc;
  fi->next = create_sentinel_frame (current_regcache);
  fi->type = frame_type_from_pc (pc);

  if (INIT_EXTRA_FRAME_INFO_P ())
    INIT_EXTRA_FRAME_INFO (0, fi);

  /* Select/initialize an unwind function.  */
  fi->unwind = frame_unwind_find_by_pc (current_gdbarch, fi->pc);

  return fi;
}

/* Return the frame that FRAME calls (NULL if FRAME is the innermost
   frame).  Be careful to not fall off the bottom of the frame chain
   and onto the sentinel frame.  */

struct frame_info *
get_next_frame (struct frame_info *frame)
{
  if (frame->level > 0)
    return frame->next;
  else
    return NULL;
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

/* Create the previous frame using the deprecated methods
   INIT_EXTRA_INFO, INIT_FRAME_PC and INIT_FRAME_PC_FIRST.  */

static struct frame_info *
legacy_get_prev_frame (struct frame_info *next_frame)
{
  CORE_ADDR address = 0;
  struct frame_info *prev;
  int fromleaf;

  /* This code only works on normal frames.  A sentinel frame, where
     the level is -1, should never reach this code.  */
  gdb_assert (next_frame->level >= 0);

  /* On some machines it is possible to call a function without
     setting up a stack frame for it.  On these machines, we
     define this macro to take two args; a frameinfo pointer
     identifying a frame and a variable to set or clear if it is
     or isn't leafless.  */

  /* Still don't want to worry about this except on the innermost
     frame.  This macro will set FROMLEAF if NEXT_FRAME is a frameless
     function invocation.  */
  if (next_frame->level == 0)
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
    address = get_frame_base (next_frame);
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
      gdb_assert (FRAME_CHAIN_P ());
      address = FRAME_CHAIN (next_frame);

      if (!frame_chain_valid (address, next_frame))
	return 0;
    }
  if (address == 0)
    return 0;

  /* Create an initially zero previous frame.  */
  prev = frame_obstack_zalloc (sizeof (struct frame_info));

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
     whether any targets *need* DEPRECATED_INIT_FRAME_PC to happen
     after INIT_EXTRA_FRAME_INFO and come up with a simple way to
     express what goes on here.

     INIT_EXTRA_FRAME_INFO is called from two places: create_new_frame
     (where the PC is already set up) and here (where it isn't).
     DEPRECATED_INIT_FRAME_PC is only called from here, always after
     INIT_EXTRA_FRAME_INFO.

     The catch is the MIPS, where INIT_EXTRA_FRAME_INFO requires the
     PC value (which hasn't been set yet).  Some other machines appear
     to require INIT_EXTRA_FRAME_INFO before they can do
     DEPRECATED_INIT_FRAME_PC.  Phoo.

     We shouldn't need DEPRECATED_INIT_FRAME_PC_FIRST to add more
     complication to an already overcomplicated part of GDB.
     gnu@cygnus.com, 15Sep92.

     Assuming that some machines need DEPRECATED_INIT_FRAME_PC after
     INIT_EXTRA_FRAME_INFO, one possible scheme:

     SETUP_INNERMOST_FRAME(): Default version is just create_new_frame
     (read_fp ()), read_pc ()).  Machines with extra frame info would
     do that (or the local equivalent) and then set the extra fields.

     SETUP_ARBITRARY_FRAME(argc, argv): Only change here is that
     create_new_frame would no longer init extra frame info;
     SETUP_ARBITRARY_FRAME would have to do that.

     INIT_PREV_FRAME(fromleaf, prev) Replace INIT_EXTRA_FRAME_INFO and
     DEPRECATED_INIT_FRAME_PC.  This should also return a flag saying
     whether to keep the new frame, or whether to discard it, because
     on some machines (e.g.  mips) it is really awkward to have
     FRAME_CHAIN_VALID called *before* INIT_EXTRA_FRAME_INFO (there is
     no good way to get information deduced in FRAME_CHAIN_VALID into
     the extra fields of the new frame).  std_frame_pc(fromleaf, prev)

     This is the default setting for INIT_PREV_FRAME.  It just does
     what the default DEPRECATED_INIT_FRAME_PC does.  Some machines
     will call it from INIT_PREV_FRAME (either at the beginning, the
     end, or in the middle).  Some machines won't use it.

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

  if (DEPRECATED_INIT_FRAME_PC_FIRST_P ())
    prev->pc = (DEPRECATED_INIT_FRAME_PC_FIRST (fromleaf, prev));

  if (INIT_EXTRA_FRAME_INFO_P ())
    INIT_EXTRA_FRAME_INFO (fromleaf, prev);

  /* This entry is in the frame queue now, which is good since
     FRAME_SAVED_PC may use that queue to figure out its value (see
     tm-sparc.h).  We want the pc saved in the inferior frame. */
  if (DEPRECATED_INIT_FRAME_PC_P ())
    prev->pc = DEPRECATED_INIT_FRAME_PC (fromleaf, prev);

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
  prev->unwind = frame_unwind_find_by_pc (current_gdbarch, prev->pc);

  /* NOTE: cagney/2002-11-18: The code segments, found in
     create_new_frame and get_prev_frame(), that initializes the
     frames type is subtly different.  The latter only updates ->type
     when it encounters a SIGTRAMP_FRAME or DUMMY_FRAME.  This stops
     get_prev_frame() overriding the frame's type when the INIT code
     has previously set it.  This is really somewhat bogus.  The
     initialization, as seen in create_new_frame(), should occur
     before the INIT function has been called.  */
  if (DEPRECATED_USE_GENERIC_DUMMY_FRAMES
      && (DEPRECATED_PC_IN_CALL_DUMMY_P ()
	  ? DEPRECATED_PC_IN_CALL_DUMMY (prev->pc, 0, 0)
	  : pc_in_dummy_frame (prev->pc)))
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

/* Return a structure containing various interesting information
   about the frame that called NEXT_FRAME.  Returns NULL
   if there is no such frame.  */

struct frame_info *
get_prev_frame (struct frame_info *next_frame)
{
  struct frame_info *prev_frame;

  /* Return the inner-most frame, when the caller passes in NULL.  */
  /* NOTE: cagney/2002-11-09: Not sure how this would happen.  The
     caller should have previously obtained a valid frame using
     get_selected_frame() and then called this code - only possibility
     I can think of is code behaving badly.

     NOTE: cagney/2003-01-10: Talk about code behaving badly.  Check
     block_innermost_frame().  It does the sequence: frame = NULL;
     while (1) { frame = get_prev_frame (frame); .... }.  Ulgh!  Why
     it couldn't be written better, I don't know.

     NOTE: cagney/2003-01-11: I suspect what is happening is
     block_innermost_frame() is, when the target has no state
     (registers, memory, ...), still calling this function.  The
     assumption being that this function will return NULL indicating
     that a frame isn't possible, rather than checking that the target
     has state and then calling get_current_frame() and
     get_prev_frame().  This is a guess mind.  */
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

  /* There is always a frame.  If this assertion fails, suspect that
     something should be calling get_selected_frame() or
     get_current_frame().  */
  gdb_assert (next_frame != NULL);

  if (next_frame->level >= 0
      && !backtrace_below_main
      && inside_main_func (get_frame_pc (next_frame)))
    /* Don't unwind past main(), bug always unwind the sentinel frame.
       Note, this is done _before_ the frame has been marked as
       previously unwound.  That way if the user later decides to
       allow unwinds past main(), that just happens.  */
    return NULL;

  /* Only try to do the unwind once.  */
  if (next_frame->prev_p)
    return next_frame->prev;
  next_frame->prev_p = 1;

  /* If we're inside the entry file, it isn't valid.  */
  /* NOTE: drow/2002-12-25: should there be a way to disable this
     check?  It assumes a single small entry file, and the way some
     debug readers (e.g.  dbxread) figure out which object is the
     entry file is somewhat hokey.  */
  /* NOTE: cagney/2003-01-10: If there is a way of disabling this test
     then it should probably be moved to before the ->prev_p test,
     above.  */
  if (inside_entry_file (get_frame_pc (next_frame)))
      return NULL;

  /* If any of the old frame initialization methods are around, use
     the legacy get_prev_frame method.  Just don't try to unwind a
     sentinel frame using that method - it doesn't work.  All sentinal
     frames use the new unwind code.  */
  if ((DEPRECATED_INIT_FRAME_PC_P ()
       || DEPRECATED_INIT_FRAME_PC_FIRST_P ()
       || INIT_EXTRA_FRAME_INFO_P ()
       || FRAME_CHAIN_P ())
      && next_frame->level >= 0)
    return legacy_get_prev_frame (next_frame);

  /* Allocate the new frame but do not wire it in to the frame chain.
     Some (bad) code in INIT_FRAME_EXTRA_INFO tries to look along
     frame->next to pull some fancy tricks (of course such code is, by
     definition, recursive).  Try to prevent it.

     There is no reason to worry about memory leaks, should the
     remainder of the function fail.  The allocated memory will be
     quickly reclaimed when the frame cache is flushed, and the `we've
     been here before' check above will stop repeated memory
     allocation calls.  */
  prev_frame = FRAME_OBSTACK_ZALLOC (struct frame_info);
  prev_frame->level = next_frame->level + 1;

  /* Try to unwind the PC.  If that doesn't work, assume we've reached
     the oldest frame and simply return.  Is there a better sentinal
     value?  The unwound PC value is then used to initialize the new
     previous frame's type.

     Note that the pc-unwind is intentionally performed before the
     frame chain.  This is ok since, for old targets, both
     frame_pc_unwind (nee, FRAME_SAVED_PC) and FRAME_CHAIN()) assume
     NEXT_FRAME's data structures have already been initialized (using
     INIT_EXTRA_FRAME_INFO) and hence the call order doesn't matter.

     By unwinding the PC first, it becomes possible to, in the case of
     a dummy frame, avoid also unwinding the frame ID.  This is
     because (well ignoring the PPC) a dummy frame can be located
     using NEXT_FRAME's frame ID.  */

  prev_frame->pc = frame_pc_unwind (next_frame);
  if (prev_frame->pc == 0)
    /* The allocated PREV_FRAME will be reclaimed when the frame
       obstack is next purged.  */
    return NULL;
  prev_frame->type = frame_type_from_pc (prev_frame->pc);

  /* Set the unwind functions based on that identified PC.  */
  prev_frame->unwind = frame_unwind_find_by_pc (current_gdbarch,
						prev_frame->pc);

  /* FIXME: cagney/2003-01-13: A dummy frame doesn't need to unwind
     the frame ID because the frame ID comes from the previous frame.
     The other frames do though.  True?  */
  {
    /* FIXME: cagney/2002-12-18: Instead of this hack, should just
       save the frame ID directly.  */
    struct frame_id id = frame_id_unwind (next_frame);
    if (!frame_id_p (id))
      return NULL;
    prev_frame->frame = id.base;
  }

  /* Link it in.  */
  next_frame->prev = prev_frame;
  prev_frame->next = next_frame;

  /* FIXME: cagney/2002-01-19: This call will go away.  Instead of
     initializing extra info, all frames will use the frame_cache
     (passed to the unwind functions) to store additional frame info.
     Unfortunatly legacy targets can't use legacy_get_prev_frame() to
     unwind the sentinel frame and, consequently, are forced to take
     this code path and rely on the below call to INIT_EXTR_FRAME_INFO
     to initialize the inner-most frame.  */
  if (INIT_EXTRA_FRAME_INFO_P ())
    {
      gdb_assert (prev_frame->level == 0);
      INIT_EXTRA_FRAME_INFO (0, prev_frame);
    }

  return prev_frame;
}

CORE_ADDR
get_frame_pc (struct frame_info *frame)
{
  return frame->pc;
}

static int
pc_notcurrent (struct frame_info *frame)
{
  /* If FRAME is not the innermost frame, that normally means that
     FRAME->pc points at the return instruction (which is *after* the
     call instruction), and we want to get the line containing the
     call (because the call is where the user thinks the program is).
     However, if the next frame is either a SIGTRAMP_FRAME or a
     DUMMY_FRAME, then the next frame will contain a saved interrupt
     PC and such a PC indicates the current (rather than next)
     instruction/line, consequently, for such cases, want to get the
     line containing fi->pc.  */
  struct frame_info *next = get_next_frame (frame);
  int notcurrent = (next != NULL && get_frame_type (next) == NORMAL_FRAME);
  return notcurrent;
}

void
find_frame_sal (struct frame_info *frame, struct symtab_and_line *sal)
{
  (*sal) = find_pc_line (frame->pc, pc_notcurrent (frame));
}

/* Per "frame.h", return the ``address'' of the frame.  Code should
   really be using get_frame_id().  */
CORE_ADDR
get_frame_base (struct frame_info *fi)
{
  return fi->frame;
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
  if (!DEPRECATED_USE_GENERIC_DUMMY_FRAMES
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
deprecated_get_frame_saved_regs (struct frame_info *frame,
				 struct frame_saved_regs *saved_regs_addr)
{
  if (frame->saved_regs == NULL)
    {
      frame->saved_regs = (CORE_ADDR *)
	frame_obstack_zalloc (SIZEOF_FRAME_SAVED_REGS);
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

struct frame_extra_info *
get_frame_extra_info (struct frame_info *fi)
{
  return fi->extra_info;
}

struct frame_extra_info *
frame_extra_info_zalloc (struct frame_info *fi, long size)
{
  fi->extra_info = frame_obstack_zalloc (size);
  return fi->extra_info;
}

void
deprecated_update_frame_pc_hack (struct frame_info *frame, CORE_ADDR pc)
{
  /* See comment in "frame.h".  */
  gdb_assert (frame->next != NULL);
  frame->pc = pc;
}

void
deprecated_update_frame_base_hack (struct frame_info *frame, CORE_ADDR base)
{
  /* See comment in "frame.h".  */
  frame->frame = base;
}

void
deprecated_set_frame_saved_regs_hack (struct frame_info *frame,
				      CORE_ADDR *saved_regs)
{
  frame->saved_regs = saved_regs;
}

void
deprecated_set_frame_extra_info_hack (struct frame_info *frame,
				      struct frame_extra_info *extra_info)
{
  frame->extra_info = extra_info;
}

void
deprecated_set_frame_next_hack (struct frame_info *fi,
				struct frame_info *next)
{
  fi->next = next;
}

void
deprecated_set_frame_prev_hack (struct frame_info *fi,
				struct frame_info *prev)
{
  fi->prev = prev;
}

struct context *
deprecated_get_frame_context (struct frame_info *fi)
{
  return fi->context;
}

void
deprecated_set_frame_context (struct frame_info *fi,
			      struct context *context)
{
  fi->context = context;
}

struct frame_info *
deprecated_frame_xmalloc (void)
{
  struct frame_info *frame = XMALLOC (struct frame_info);
  memset (frame, 0, sizeof (struct frame_info));
  return frame;
}

struct frame_info *
deprecated_frame_xmalloc_with_cleanup (long sizeof_saved_regs,
				       long sizeof_extra_info)
{
  struct frame_info *frame = deprecated_frame_xmalloc ();
  make_cleanup (xfree, frame);
  if (sizeof_saved_regs > 0)
    {
      frame->saved_regs = xcalloc (1, sizeof_saved_regs);
      make_cleanup (xfree, frame->saved_regs);
    }
  if (sizeof_extra_info > 0)
    {
      frame->extra_info = xcalloc (1, sizeof_extra_info);
      make_cleanup (xfree, frame->extra_info);
    }
  return frame;
}

void
_initialize_frame (void)
{
  obstack_init (&frame_cache_obstack);

  /* FIXME: cagney/2003-01-19: This command needs a rename.  Suggest
     `set backtrace {past,beyond,...}-main'.  Also suggest adding `set
     backtrace ...-start' to control backtraces past start.  The
     problem with `below' is that it stops the `up' command.  */

  add_setshow_boolean_cmd ("backtrace-below-main", class_obscure,
			   &backtrace_below_main, "\
Set whether backtraces should continue past \"main\".\n\
Normally the caller of \"main\" is not of interest, so GDB will terminate\n\
the backtrace at \"main\".  Set this variable if you need to see the rest\n\
of the stack trace.", "\
Show whether backtraces should continue past \"main\".\n\
Normally the caller of \"main\" is not of interest, so GDB will terminate\n\
the backtrace at \"main\".  Set this variable if you need to see the rest\n\
of the stack trace.",
			   NULL, NULL, &setlist, &showlist);
}
