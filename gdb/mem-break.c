/* Simulate breakpoints by patching locations in the target system.
   Copyright (C) 1990 Free Software Foundation, Inc.

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

#include "defs.h"
#include "param.h"

#ifdef BREAKPOINT
/* This file is only useful if BREAKPOINT is set.  If not, we punt.  */

#include <stdio.h>
#include "breakpoint.h"
#include "inferior.h"
#include "target.h"

/* This is the sequence of bytes we insert for a breakpoint.  On some
   machines, breakpoints are handled by the target environment and we
   don't have to worry about them here.  */

static char break_insn[] = BREAKPOINT;

/* This is only to check that BREAKPOINT fits in BREAKPOINT_MAX bytes.  */

static char check_break_insn_size[BREAKPOINT_MAX] = BREAKPOINT;

/* Insert a breakpoint on machines that don't have any better breakpoint
   support.  We read the contents of the target location and stash it,
   then overwrite it with a breakpoint instruction.  ADDR is the target
   location in the target machine.  CONTENTS_CACHE is a pointer to 
   memory allocated for saving the target contents.  It is guaranteed
   by the caller to be long enough to save sizeof BREAKPOINT bytes.
   FIXME: This size is target_arch dependent and should be available in
   the target_arch transfer vector, if we ever have one...  */

int
memory_insert_breakpoint (addr, contents_cache)
     CORE_ADDR addr;
     char *contents_cache;
{
  int val;

  val = target_read_memory (addr, contents_cache, sizeof break_insn);

  if (val == 0)
    val = target_write_memory (addr, break_insn, sizeof break_insn);

  return val;
}


int
memory_remove_breakpoint (addr, contents_cache)
     CORE_ADDR addr;
     char *contents_cache;
{
  return target_write_memory (addr, contents_cache, sizeof break_insn);
}


#if 0
/* This should move back into breakpoint.c, sad to say.   Encapsulate
    sizeof (BREAKPOINT) by export it as an int from mem-break.c.  */

/* Like target_read_memory() but if breakpoints are inserted, return
   the shadow contents instead of the breakpoints themselves.  */
int
read_memory_nobpt (memaddr, myaddr, len)
     CORE_ADDR memaddr;
     char *myaddr;
     unsigned len;
{
  int status;
  struct breakpoint *b;
  ALL_BREAKPOINTS (b)
    {
      if (b->address == NULL || !b->inserted)
	continue;
      else if (b->address + sizeof (break_insn) <= memaddr)
	/* The breakpoint is entirely before the chunk of memory
	   we are reading.  */
	continue;
      else if (b->address >= memaddr + len)
	/* The breakpoint is entirely after the chunk of memory we
	   are reading.  */
	continue;
      else
	{
	  /* Copy the breakpoint from the shadow contents, and recurse
	     for the things before and after.  */
	  
	  /* Addresses and length of the part of the breakpoint that
	     we need to copy.  */
	  CORE_ADDR membpt = b->address;
	  unsigned int bptlen = sizeof (break_insn);
	  /* Offset within shadow_contents.  */
	  int bptoffset = 0;
	  
	  if (membpt < memaddr)
	    {
	      /* Only copy the second part of the breakpoint.  */
	      bptlen -= memaddr - membpt;
	      bptoffset = memaddr - membpt;
	      membpt = memaddr;
	    }

	  if (membpt + bptlen > memaddr + len)
	    {
	      /* Only copy the first part of the breakpoint.  */
	      bptlen -= (membpt + bptlen) - (memaddr + len);
	    }

	  bcopy (b->shadow_contents + bptoffset,
		 myaddr + membpt - memaddr, bptlen);

	  if (membpt > memaddr)
	    {
	      /* Copy the section of memory before the breakpoint.  */
	      status = read_memory_nobpt (memaddr, myaddr, membpt - memaddr);
	      if (status != 0)
		return status;
	    }

	  if (membpt + bptlen < memaddr + len)
	    {
	      /* Copy the section of memory after the breakpoint.  */
	      status = read_memory_nobpt
		(membpt + bptlen,
		 myaddr + membpt + bptlen - memaddr,
		 memaddr + len - (membpt + bptlen));
	      if (status != 0)
		return status;
	    }
	  return 0;
	}
    }
  /* Nothing overlaps.  Just call read_memory_noerr.  */
  return target_read_memory (memaddr, myaddr, len);
}
#endif /* 0 */

#else  /* BREAKPOINT */

char nogo[] = "Breakpoints not implemented for this target.";

int
memory_insert_breakpoint (addr, contents_cache)
     CORE_ADDR addr;
     char *contents_cache;
{
  error (nogo);
  return 0;	/* lint */
}

int
memory_remove_breakpoint (addr, contents_cache)
     CORE_ADDR addr;
     char *contents_cache;
{
  error (nogo);
  return 0;	/* lint */
}

#endif /* BREAKPOINT */
