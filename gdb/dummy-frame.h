/* Code dealing with dummy stack frames, for GDB, the GNU debugger.

   Copyright 2002 Free Software Foundation, Inc.

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

#if !defined (DUMMY_FRAME_H)
#define DUMMY_FRAME_H 1

struct frame_info;
struct regcache;

/* GENERIC DUMMY FRAMES
  
   The following code serves to maintain the dummy stack frames for
   inferior function calls (ie. when gdb calls into the inferior via
   call_function_by_hand).  This code saves the machine state before
   the call in host memory, so we must maintain an independent stack
   and keep it consistant etc.  I am attempting to make this code
   generic enough to be used by many targets.
 
   The cheapest and most generic way to do CALL_DUMMY on a new target
   is probably to define CALL_DUMMY to be empty, CALL_DUMMY_LENGTH to
   zero, and CALL_DUMMY_LOCATION to AT_ENTRY.  Then you must remember
   to define PUSH_RETURN_ADDRESS, because no call instruction will be
   being executed by the target.  Also FRAME_CHAIN_VALID as
   generic_{file,func}_frame_chain_valid and FIX_CALL_DUMMY as
   generic_fix_call_dummy.  */

/* Assuming that FRAME is a dummy, return a register value for the
   previous frame.  */

extern void dummy_frame_register_unwind (struct frame_info *frame,
					 void **unwind_cache,
					 int regnum,
					 int *optimized,
					 enum lval_type *lvalp,
					 CORE_ADDR *addrp,
					 int *realnump,
					 void *valuep);

/* Return the regcache that belongs to the dummy-frame identifed by PC
   and FP, or NULL if no such frame exists.  */
/* FIXME: cagney/2002-11-08: The function only exists because of
   deprecated_generic_get_saved_register.  Eliminate that function and
   this, to, can go.  */

extern struct regcache *generic_find_dummy_frame (CORE_ADDR pc,
						  CORE_ADDR fp);
#endif /* !defined (DUMMY_FRAME_H)  */
