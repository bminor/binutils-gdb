/* Code dealing with legacy frames, for GDB, the GNU debugger.

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

#if !defined (LEGACY_FRAME_H)
#define LEGACY_FRAME_H 1

struct frame_info;
struct regcache;
struct frame_id;
struct frame_unwind_cache;

/* LEGACY FRAMES
  
   The original extra frame info implementation of the`struct
   frame_info' object.  These frames always initialize the entire
   frame object using extra frame info.  */

/* Assuming that FRAME is a legacy, return a register value for the
   previous frame.  */

extern void legacy_frame_register_unwind (struct frame_info *frame,
					  struct frame_unwind_cache **unwind_cache,
					  int regnum,
					  int *optimized,
					  enum lval_type *lvalp,
					  CORE_ADDR *addrp,
					  int *realnump,
					  void *valuep);

/* Assuming that FRAME is a legacy, return the resume address for the
   previous frame.  */

extern CORE_ADDR legacy_frame_pc_unwind (struct frame_info *frame,
					 struct frame_unwind_cache **unwind_cache);

/* Assuming that FRAME is a legacy, return the ID of the calling frame
   (the frame that the legacy has the saved state of).  */

extern void legacy_frame_id_unwind (struct frame_info *frame,
				    struct frame_unwind_cache **unwind_cache,
				    struct frame_id *id);

#endif
