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

/* FIND_SAVED_REGISTER ()

   Return the address in which frame FRAME's value of register REGNUM
   has been saved in memory.  Or return zero if it has not been saved.
   If REGNUM specifies the SP, the value we return is actually
   the SP value, not an address where it was saved.  */

CORE_ADDR
find_saved_register (struct frame_info *frame, int regnum)
{
  register struct frame_info *frame1 = NULL;
  register CORE_ADDR addr = 0;

  if (frame == NULL)		/* No regs saved if want current frame */
    return 0;

  /* Note that this next routine assumes that registers used in
     frame x will be saved only in the frame that x calls and
     frames interior to it.  This is not true on the sparc, but the
     above macro takes care of it, so we should be all right. */
  while (1)
    {
      QUIT;
      frame1 = get_next_frame (frame);
      if (frame1 == 0)
	break;
      frame = frame1;
      FRAME_INIT_SAVED_REGS (frame1);
      if (frame1->saved_regs[regnum])
	addr = frame1->saved_regs[regnum];
    }

  return addr;
}

/* DEFAULT_GET_SAVED_REGISTER ()

   Find register number REGNUM relative to FRAME and put its (raw,
   target format) contents in *RAW_BUFFER.  Set *OPTIMIZED if the
   variable was optimized out (and thus can't be fetched).  Set *LVAL
   to lval_memory, lval_register, or not_lval, depending on whether
   the value was fetched from memory, from a register, or in a strange
   and non-modifiable way (e.g. a frame pointer which was calculated
   rather than fetched).  Set *ADDRP to the address, either in memory
   on as a REGISTER_BYTE offset into the registers array.

   Note that this implementation never sets *LVAL to not_lval.  But
   it can be replaced by defining GET_SAVED_REGISTER and supplying
   your own.

   The argument RAW_BUFFER must point to aligned memory.  */

static void
default_get_saved_register (char *raw_buffer,
			    int *optimized,
			    CORE_ADDR *addrp,
			    struct frame_info *frame,
			    int regnum,
			    enum lval_type *lval)
{
  CORE_ADDR addr;

  if (!target_has_registers)
    error ("No registers.");

  /* Normal systems don't optimize out things with register numbers.  */
  if (optimized != NULL)
    *optimized = 0;
  addr = find_saved_register (frame, regnum);
  if (addr != 0)
    {
      if (lval != NULL)
	*lval = lval_memory;
      if (regnum == SP_REGNUM)
	{
	  if (raw_buffer != NULL)
	    {
	      /* Put it back in target format.  */
	      store_address (raw_buffer, REGISTER_RAW_SIZE (regnum),
			     (LONGEST) addr);
	    }
	  if (addrp != NULL)
	    *addrp = 0;
	  return;
	}
      if (raw_buffer != NULL)
	target_read_memory (addr, raw_buffer, REGISTER_RAW_SIZE (regnum));
    }
  else
    {
      if (lval != NULL)
	*lval = lval_register;
      addr = REGISTER_BYTE (regnum);
      if (raw_buffer != NULL)
	read_register_gen (regnum, raw_buffer);
    }
  if (addrp != NULL)
    *addrp = addr;
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
      *addrp = 0;
      /* Should this code test ``register_cached (regnum) < 0'' and do
         something like set realnum to -1 when the register isn't
         available?  */
      *realnump = regnum;
      if (bufferp)
	read_register_gen (regnum, bufferp);
      return;
    }

  /* Ask this frame to unwind its register.  */
  frame->register_unwind (frame, &frame->register_unwind_cache, regnum,
			  optimizedp, lvalp, addrp, realnump, bufferp);
}


void
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

#if !defined (GET_SAVED_REGISTER)
#define GET_SAVED_REGISTER(raw_buffer, optimized, addrp, frame, regnum, lval) \
  default_get_saved_register(raw_buffer, optimized, addrp, frame, regnum, lval)
#endif

void
get_saved_register (char *raw_buffer,
		    int *optimized,
		    CORE_ADDR *addrp,
		    struct frame_info *frame,
		    int regnum,
		    enum lval_type *lval)
{
  GET_SAVED_REGISTER (raw_buffer, optimized, addrp, frame, regnum, lval);
}

/* frame_register_read ()

   Find and return the value of REGNUM for the specified stack frame.
   The number of bytes copied is REGISTER_RAW_SIZE (REGNUM).

   Returns 0 if the register value could not be found.  */

int
frame_register_read (struct frame_info *frame, int regnum, void *myaddr)
{
  int optim;
  get_saved_register (myaddr, &optim, (CORE_ADDR *) NULL, frame,
		      regnum, (enum lval_type *) NULL);

  /* FIXME: cagney/2002-05-15: This test, is just bogus.

     It indicates that the target failed to supply a value for a
     register because it was "not available" at this time.  Problem
     is, the target still has the register and so get saved_register()
     may be returning a value saved on the stack.  */

  if (register_cached (regnum) < 0)
    return 0;			/* register value not available */

  return !optim;
}
