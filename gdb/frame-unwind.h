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
struct frame_id;
struct frame_unwind;
struct gdbarch;
struct regcache;

/* Return the frame unwind methods for the function that contains PC,
   or NULL if this this unwinder can't handle this frame.  */

typedef const struct frame_unwind *(frame_unwind_p_ftype) (CORE_ADDR pc);

/* Add a frame unwinder to the list.  The predicates are polled in the
   order that they are appended.  The initial list contains the dummy
   frame's predicate.  */

extern void frame_unwind_append_predicate (struct gdbarch *gdbarch,
					   frame_unwind_p_ftype *p);

/* Iterate through the list of frame unwinders until one returns an
   implementation.  */

extern const struct frame_unwind *frame_unwind_find_by_pc (struct gdbarch
							   *gdbarch,
							   CORE_ADDR pc);

/* The following unwind functions all assume a frame chain like:
   (outer) prev <-> this <-> next (inner); Even though some unwind to
   THIS frame (frame ID) and others unwind the PREV frame, they are
   all, consistently passed NEXT frame and THIS cache.

   The intent is to clarify the relationship between NEXT frame and
   THIS cache.  It is, of course, at the expense of confusing somewhat
   the expected unwind behavior of PC/REG unwind VS ID unwind.  Sigh.  */

/* Assuming the frame chain: (outer) prev <-> this <-> next (inner);
   use the NEXT frame, and its register unwind method, to determine
   the frame ID of THIS frame.

   A frame ID provides an invariant that can be used to re-identify an
   instance of a frame.  It is a combination of the frame's `base' and
   the frame's function's code.

   Traditionally, THIS frame's ID was determined by examining THIS
   frame's function's prologue and identifying which register/offset
   are being used as THIS frame's base.

   THIS_CACHE can be used to share any prolog analysis data with the
   other unwind methods.  Memory for that cache should be allocated
   using frame_obstack_zalloc().  */

typedef void (frame_unwind_id_ftype) (struct frame_info *next_frame,
				      void **this_cache,
				      struct frame_id *this_id);

/* Assuming the frame chain: (outer) prev <-> this <-> next (inner);
   use the NEXT frame, and its register unwind method, to unwind THIS
   frame's registers, returning the value of REGNUM in PREV frame.

   Traditionally, THIS frame's registers were unwound by examining
   THIS frame's function's prologue and identifying which registers
   that prolog code saved on the stack.

   Ex: Assuming THIS is a frameless function that has saved the return
   address (to PREV) in R1, then: a request to unwind THIS's PC
   (returning the value of PC in PREV), becomes a request for the
   value of R1 in THIS (that is where the value was saved), which
   becomes a request to unwind R1 from NEXT.

   THIS_CACHE can be used to share any prologue analysis data with the
   other unwind methods.  Memory for that cache should be allocated
   using frame_obstack_zalloc().  */

typedef void (frame_unwind_reg_ftype) (struct frame_info *next_frame,
				       void **this_cache,
				       int prev_regnum,
				       int *optimized,
				       enum lval_type * lvalp,
				       CORE_ADDR *addrp,
				       int *realnump, void *valuep);

struct frame_unwind
{
  /* Should the frame's type go here? */
  /* Should an attribute indicating the frame's address-in-block go
     here?  */
  frame_unwind_id_ftype *id;
  frame_unwind_reg_ftype *reg;
};

#endif
