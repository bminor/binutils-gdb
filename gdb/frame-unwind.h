/* Definitions for a frame unwinder, for GDB, the GNU debugger.

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

#if !defined (FRAME_UNWIND_H)
#define FRAME_UNWIND_H 1

struct frame_info;
struct frame_unwind_cache;
struct frame_unwind;
struct frame_id;
struct gdbarch;

/* Return the corresponding frame descriptor this method is capable of
   unwinding the frame containing PC.  */

typedef const struct frame_unwind *(frame_unwind_p_ftype) (CORE_ADDR pc);

/* Append a descriptor predicate.  Descriptors are polled in append
   order.  The list is initialized with just the dummy frame.  */

extern void frame_unwind_append_predicate (struct gdbarch *gdbarch,
					   frame_unwind_p_ftype *p);

/* Iterate through the list of frame descriptor predicates for the
   first one to return a frame descriptor.  */

extern const struct frame_unwind *frame_unwind_find_by_pc (struct gdbarch *gdbarch,
							   CORE_ADDR pc);

/* Return the location (and possibly value) of REGNUM for the previous
   (older, up) frame.  All parameters except VALUEP can be assumed to
   be non NULL.  When VALUEP is NULL, just the location of the
   register should be returned.

   UNWIND_CACHE is provided as mechanism for implementing a per-frame
   local cache.  It's initial value being NULL.  Memory for that cache
   should be allocated using frame_obstack_zalloc().

   Register window architectures (eg SPARC) should note that REGNUM
   identifies the register for the previous frame.  For instance, a
   request for the value of "o1" for the previous frame would be found
   in the register "i1" in this FRAME.  */

typedef void (frame_register_unwind_ftype) (struct frame_info *frame,
					    struct frame_unwind_cache **unwind_cache,
					    int regnum,
					    int *optimized,
					    enum lval_type *lvalp,
					    CORE_ADDR *addrp,
					    int *realnump,
					    void *valuep);

/* Same as for registers above, but return the address at which the
   calling frame would resume.  */

typedef CORE_ADDR (frame_pc_unwind_ftype) (struct frame_info *frame,
					   struct frame_unwind_cache **unwind_cache);

/* Same as for registers above, but return the ID of the frame that
   called this one.  */

typedef void (frame_id_unwind_ftype) (struct frame_info *frame,
				      struct frame_unwind_cache **unwind_cache,
				      struct frame_id *id);

struct frame_unwind
{
  /* FIXME: Should the frame's type go here? */
  frame_pc_unwind_ftype *pc;
  frame_id_unwind_ftype *id;
  frame_register_unwind_ftype *reg;
};

#endif
