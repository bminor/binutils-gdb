/* Code dealing with dummy stack frames, for GDB, the GNU debugger.

   Copyright 1986, 1987, 1988, 1989, 1990, 1991, 1992, 1993, 1994,
   1995, 1996, 1997, 1998, 1999, 2000, 2001, 2002, 2003, 2004 Free
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
#include "dummy-frame.h"
#include "regcache.h"
#include "frame.h"
#include "inferior.h"
#include "gdb_assert.h"
#include "frame-unwind.h"
#include "command.h"
#include "gdbcmd.h"

static int pc_in_dummy_frame (CORE_ADDR pc);

/* Dummy frame.  This saves the processor state just prior to setting
   up the inferior function call.  Older targets save the registers
   on the target stack (but that really slows down function calls).  */

struct dummy_frame
{
  struct dummy_frame *next;

  /* These values belong to the caller (the previous frame, the frame
     that this unwinds back to).  */
  CORE_ADDR pc;
  CORE_ADDR top;
  struct frame_id id;
  struct regcache *regcache;

  /* Address range of the call dummy code.  Look for PC in the range
     [LO..HI) (after allowing for DECR_PC_AFTER_BREAK).  */
  CORE_ADDR call_lo;
  CORE_ADDR call_hi;
};

static struct dummy_frame *dummy_frame_stack = NULL;

/* Function: pc_in_call_dummy (pc)

   Return true if the PC falls in a dummy frame created by gdb for an
   inferior call.  The code below which allows DECR_PC_AFTER_BREAK is
   for infrun.c, which may give the function a PC without that
   subtracted out.  */

int
deprecated_pc_in_call_dummy (CORE_ADDR pc)
{
  return pc_in_dummy_frame (pc);
}

/* Return non-zero if the PC falls in a dummy frame.

   The code below which allows DECR_PC_AFTER_BREAK is for infrun.c,
   which may give the function a PC without that subtracted out.

   FIXME: cagney/2002-11-23: This is silly.  Surely "infrun.c" can
   figure out what the real PC (as in the resume address) is BEFORE
   calling this function.  */

static int
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
    if (gdbarch_inner_than (current_gdbarch, dummy_frame->top, fp))
      /* stale -- destroy! */
      {
	dummy_frame_stack = dummy_frame->next;
	regcache_xfree (dummy_frame->regcache);
	xfree (dummy_frame);
	dummy_frame = dummy_frame_stack;
      }
    else
      dummy_frame = dummy_frame->next;

  dummy_frame = xmalloc (sizeof (struct dummy_frame));
  dummy_frame->regcache = frame_save_as_regcache (get_current_frame ());

  dummy_frame->pc = read_pc ();
  dummy_frame->top = 0;
  dummy_frame->id = get_frame_id (get_current_frame ());
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

/* Return the dummy frame cache, it contains both the ID, and a
   pointer to the regcache.  */
struct dummy_frame_cache
{
  struct frame_id this_id;
  struct regcache *prev_regcache;
};

int
dummy_frame_sniffer (const struct frame_unwind *self,
		     struct frame_info *next_frame,
		     void **this_prologue_cache)
{
  struct dummy_frame *dummyframe;
  struct frame_id this_id;

  /* When unwinding a normal frame, the stack structure is determined
     by analyzing the frame's function's code (be it using brute force
     prologue analysis, or the dwarf2 CFI).  In the case of a dummy
     frame, that simply isn't possible.  The PC is either the program
     entry point, or some random address on the stack.  Trying to use
     that PC to apply standard frame ID unwind techniques is just
     asking for trouble.  */
  /* Use an architecture specific method to extract the prev's dummy
     ID from the next frame.  Note that this method uses
     frame_register_unwind to obtain the register values needed to
     determine the dummy frame's ID.  */
  this_id = gdbarch_unwind_dummy_id (get_frame_arch (next_frame), next_frame);

  /* Use that ID to find the corresponding cache entry.  */
  for (dummyframe = dummy_frame_stack;
       dummyframe != NULL;
       dummyframe = dummyframe->next)
    {
      /* Does the PC fall within the dummy frame's breakpoint
         instruction.  If not, discard this one.  */
      if (!(this_id.code_addr >= dummyframe->call_lo
	    && this_id.code_addr < dummyframe->call_hi))
	continue;
      /* Does the FP match?  "infcall.c" explicitly saved the
	 top-of-stack before the inferior function call, assume
	 unwind_dummy_id() returns that same stack value.  */
      if (this_id.stack_addr != dummyframe->top)
	continue;
      /* The FP matches this dummy frame.  */
      {
	struct dummy_frame_cache *cache;
	cache = FRAME_OBSTACK_ZALLOC (struct dummy_frame_cache);
	cache->prev_regcache = dummyframe->regcache;
	cache->this_id = this_id;
	(*this_prologue_cache) = cache;
	return 1;
      }
    }
  return 0;
}

/* Given a call-dummy dummy-frame, return the registers.  Here the
   register value is taken from the local copy of the register buffer.  */

static void
dummy_frame_prev_register (struct frame_info *next_frame,
			   void **this_prologue_cache,
			   int regnum, int *optimized,
			   enum lval_type *lvalp, CORE_ADDR *addrp,
			   int *realnum, void *bufferp)
{
  /* The dummy-frame sniffer always fills in the cache.  */
  struct dummy_frame_cache *cache = (*this_prologue_cache);
  gdb_assert (cache != NULL);

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
      regcache_cooked_read (cache->prev_regcache, regnum, bufferp);
    }
}

/* Assuming that THIS frame is a dummy (remember, the NEXT and not
   THIS frame is passed in), return the ID of THIS frame.  That ID is
   determined by examining the NEXT frame's unwound registers using
   the method unwind_dummy_id().  As a side effect, THIS dummy frame's
   dummy cache is located and and saved in THIS_PROLOGUE_CACHE.  */

static void
dummy_frame_this_id (struct frame_info *next_frame,
		     void **this_prologue_cache,
		     struct frame_id *this_id)
{
  /* The dummy-frame sniffer always fills in the cache.  */
  struct dummy_frame_cache *cache = (*this_prologue_cache);
  gdb_assert (cache != NULL);
  (*this_id) = cache->this_id;
}

static const struct frame_unwind dummy_frame_unwinder =
{
  DUMMY_FRAME,
  dummy_frame_this_id,
  dummy_frame_prev_register,
  NULL,
  dummy_frame_sniffer,
};

const struct frame_unwind *const dummy_frame_unwind = {
  &dummy_frame_unwinder
};

static void
fprint_dummy_frames (struct ui_file *file)
{
  struct dummy_frame *s;
  for (s = dummy_frame_stack; s != NULL; s = s->next)
    {
      gdb_print_host_address (s, file);
      fprintf_unfiltered (file, ":");
      fprintf_unfiltered (file, " pc=0x%s", paddr (s->pc));
      fprintf_unfiltered (file, " top=0x%s", paddr (s->top));
      fprintf_unfiltered (file, " id=");
      fprint_frame_id (file, s->id);
      fprintf_unfiltered (file, " call_lo=0x%s", paddr (s->call_lo));
      fprintf_unfiltered (file, " call_hi=0x%s", paddr (s->call_hi));
      fprintf_unfiltered (file, "\n");
    }
}

static void
maintenance_print_dummy_frames (char *args, int from_tty)
{
  if (args == NULL)
    fprint_dummy_frames (gdb_stdout);
  else
    {
      struct ui_file *file = gdb_fopen (args, "w");
      if (file == NULL)
	perror_with_name ("maintenance print dummy-frames");
      fprint_dummy_frames (file);    
      ui_file_delete (file);
    }
}

extern void _initialize_dummy_frame (void);

void
_initialize_dummy_frame (void)
{
  add_cmd ("dummy-frames", class_maintenance, maintenance_print_dummy_frames,
	   "Print the contents of the internal dummy-frame stack.",
	   &maintenanceprintlist);

}
