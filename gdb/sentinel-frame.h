/* Code dealing with register stack frames, for GDB, the GNU debugger.

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

#if !defined (SENTINEL_FRAME_H)
#define SENTINEL_FRAME_H 1

struct frame_info;
struct frame_id;
struct frame_unwind_cache;

/* Implement the sentinel frame.  The sentinel frame terminates the
   inner most end of the frame chain.  If unwound, it returns the
   information need to construct an inner-most frame.  */

/* Pump prime the sentinel frame's cache.  Since this needs the
   REGCACHE provide that here.  */

struct frame_unwind_cache *sentinel_frame_cache (struct regcache *regcache);

/* Return the previous frames register value.  For a sentinel-frame,
   it is the value found in the register cache.  */

extern void sentinel_frame_register_unwind (struct frame_info *frame,
					    struct frame_unwind_cache **unwind_cache,
					    int regnum,
					    int *optimized,
					    enum lval_type *lvalp,
					    CORE_ADDR *addrp,
					    int *realnump,
					    void *valuep);

/* Return the resume address of the previous frame.  For the
   sentinel-frame, it is the threads resume address.  */

extern CORE_ADDR sentinel_frame_pc_unwind (struct frame_info *frame,
					   struct frame_unwind_cache **unwind_cache);

/* Return the frame ID of the previous frame.  For the sentinel-frame,
   it is the ID of the inner most frame.  */

extern void sentinel_frame_id_unwind (struct frame_info *frame,
				      struct frame_unwind_cache **unwind_cache,
				      struct frame_id *id);

extern const struct frame_unwind *sentinel_frame_unwind_p (CORE_ADDR pc);

#endif /* !defined (SENTINEL_FRAME_H)  */
