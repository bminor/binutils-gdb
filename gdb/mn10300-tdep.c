/* Target-dependent code for the NEC MN10300 for GDB, the GNU debugger.
   Copyright 1996, Free Software Foundation, Inc.

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. */

/* Contributed by Geoffrey Noer, noer@cygnus.com */

#include "defs.h"
#include "frame.h"
#include "inferior.h"
#include "obstack.h"
#include "target.h"
#include "value.h"
#include "bfd.h"
#include "gdb_string.h"
#include "gdbcore.h"
#include "symfile.h"

/* Info gleaned from scanning a function's prologue.  */

struct pifsr			/* Info about one saved reg */
{
  int framereg;			/* Frame reg (SP or FP) */
  int offset;			/* Offset from framereg */
  int reg;			/* Saved register number */
};

struct prologue_info
{
  int framereg;
  int frameoffset;
  int start_function;
  struct pifsr *pifsrs;
};

static CORE_ADDR
mn10300_scan_prologue (pc, pi)
     CORE_ADDR pc;
     struct prologue_info *pi;
{
}

void
mn10300_init_extra_frame_info (fi)
     struct frame_info *fi;
{
}

CORE_ADDR
mn10300_frame_chain (fi)
     struct frame_info *fi;
{
}

CORE_ADDR
mn10300_find_callers_reg (fi, regnum)
     struct frame_info *fi;
     int regnum;
{
}

CORE_ADDR
mn10300_skip_prologue (pc)
     CORE_ADDR pc;
{
}

/* Function: pop_frame
   This routine gets called when either the user uses the `return'
   command, or the call dummy breakpoint gets hit.  */

void
mn10300_pop_frame (frame)
     struct frame_info *frame;
{
}

CORE_ADDR
mn10300_push_arguments (nargs, args, sp, struct_return, struct_addr)
     int nargs;
     value_ptr *args;
     CORE_ADDR sp;
     unsigned char struct_return;
     CORE_ADDR struct_addr;
{
}

CORE_ADDR
mn10300_push_return_address (pc, sp)
     CORE_ADDR pc;
     CORE_ADDR sp;
{
}
 
CORE_ADDR
mn10300_frame_saved_pc (fi)
     struct frame_info *fi;
{
}

void
get_saved_register (raw_buffer, optimized, addrp, frame, regnum, lval)
     char *raw_buffer;
     int *optimized;
     CORE_ADDR *addrp;
     struct frame_info *frame;
     int regnum;
     enum lval_type *lval;
{
  generic_get_saved_register (raw_buffer, optimized, addrp, 
			      frame, regnum, lval);
}

int
mn10300_fix_call_dummy (dummy, sp, fun, nargs, args, type, gcc_p)
     char *dummy;
     CORE_ADDR sp;
     CORE_ADDR fun;
     int nargs;
     value_ptr *args;
     struct type *type;
     int gcc_p;
{
}

void
_initialize_mn10300_tdep ()
{
  tm_print_insn = print_insn_mn10300;
}
