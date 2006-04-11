/* Simulate breakpoints by patching locations in the target system, for GDB.

   Copyright (C) 1990, 1991, 1992, 1993, 1995, 1997, 1998, 1999, 2000,
   2002 Free Software Foundation, Inc.

   Contributed by Cygnus Support.  Written by John Gilmore.

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
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

#include "defs.h"

/* This file is only useful if BREAKPOINT_FROM_PC is set.  If not, we
   punt.  */

#include "symtab.h"
#include "breakpoint.h"
#include "inferior.h"
#include "target.h"


/* Insert a breakpoint on targets that don't have any better breakpoint
   support.  We read the contents of the target location and stash it,
   then overwrite it with a breakpoint instruction.  ADDR is the target
   location in the target machine.  BPT is a pointer to the breakpoint
   location being inserted, which contains some memory allocated for
   saving the target contents.  It is guaranteed by the caller to be
   long enough to save BREAKPOINT_LEN bytes (this is accomplished via
   BREAKPOINT_MAX).  */

int
default_memory_insert_breakpoint (CORE_ADDR addr, struct bp_location *bpt)
{
  int val;
  const unsigned char *bp;
  int bplen;

  /* Determine appropriate breakpoint contents and size for this address.  */
  bp = BREAKPOINT_FROM_PC (&addr, &bplen);
  if (bp == NULL)
    error (_("Software breakpoints not implemented for this target."));

  /* Save the memory contents.  */
  bpt->placed_address = addr;
  bpt->placed_size = bplen;
  val = target_read_memory (addr, bpt->shadow_contents, bplen);

  /* Write the breakpoint.  */
  if (val == 0)
    val = target_write_memory (addr, bp, bplen);

  return val;
}


int
default_memory_remove_breakpoint (CORE_ADDR addr, struct bp_location *bpt)
{
  return target_write_memory (bpt->placed_address, bpt->shadow_contents,
			      bpt->placed_size);
}


int
memory_insert_breakpoint (CORE_ADDR addr, struct bp_location *bpt)
{
  return MEMORY_INSERT_BREAKPOINT (addr, bpt);
}

int
memory_remove_breakpoint (CORE_ADDR addr, struct bp_location *bpt)
{
  return MEMORY_REMOVE_BREAKPOINT (addr, bpt);
}
