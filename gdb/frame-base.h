/* Definitions for a frame base, for GDB, the GNU debugger.

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

#if !defined (FRAME_BASE_H)
#define FRAME_BASE_H 1

struct frame_info;
struct frame_id;
struct frame_unwind;
struct frame_base;
struct gdbarch;
struct regcache;

/* Given the NEXT frame, take a wiff of THIS frame's registers (namely
   the PC and attributes) and if SELF is the applicable frame-base
   handler return non-zero.  Optionally also initialize
   THIS_BASE_CACHE.

   If THIS frame's unwinder is the same as frame-base.unwind, THIS
   frame's prologue-cache will be passed as THIS_BASE_CACHE.  */

typedef int (frame_base_sniffer) (const struct frame_base *self,
				  struct frame_info *next_frame,
				  void **this_base_cache);

/* Assuming the frame chain: (outer) prev <-> this <-> next (inner);
   and that this is a `normal frame'; use the NEXT frame, and its
   register unwind method, to determine the address of THIS frame's
   `base'.

   The exact meaning of `base' is highly dependant on the type of the
   debug info.  It is assumed that dwarf2, stabs, ... will each
   provide their own methods.

   A typical implmentation will return the same value for base,
   locals-base and args-base.  That value, however, will likely be
   different to the frame ID's stack address.

   If THIS frame's unwinder is the same as frame-base.unwind, THIS
   frame's prologue-cache will be passed as THIS_BASE_CACHE.  */

/* A generic base address.  */

typedef CORE_ADDR (frame_this_base_ftype) (struct frame_info *next_frame,
					   void **this_base_cache);

/* The base address of the frame's local variables.  */

typedef CORE_ADDR (frame_this_locals_ftype) (struct frame_info *next_frame,
					     void **this_base_cache);

/* The base address of the frame's arguments / parameters.  */

typedef CORE_ADDR (frame_this_args_ftype) (struct frame_info *next_frame,
					   void **this_base_cache);

struct frame_base
{
  /* If non-NULL, a low-level unwinder that shares its implementation
     with this high-level frame-base method.  */
  const struct frame_unwind *unwind;
  frame_this_base_ftype *this_base;
  frame_this_locals_ftype *this_locals;
  frame_this_args_ftype *this_args;
  const struct frame_data *base_data;
  frame_base_sniffer *sniffer;
};

/* Given the NEXT frame, return the frame base methods for THIS frame,
   or NULL if it can't handle THIS frame.  */

typedef const struct frame_base *(frame_base_sniffer_ftype) (struct frame_info *next_frame);

/* Append a frame base sniffer to the list.  The sniffers are polled
   in the order that they are appended.  */

extern void frame_base_append_sniffer (struct gdbarch *gdbarch,
				       frame_base_sniffer_ftype *sniffer);

/* Register a frame base, appending it to the list that need to be
   searched.  */
extern void frame_base_register_base (struct gdbarch *gdbarch,
				      const struct frame_base *base);


/* Set the default frame base.  If all else fails, this one is
   returned.  If this isn't set, the default is to use legacy code
   that uses things like the frame ID's base (ulgh!).  */

extern void frame_base_set_default (struct gdbarch *gdbarch,
				    const struct frame_base *def);

/* Iterate through the list of frame base handlers until one returns
   an implementation.  */

extern const struct frame_base *frame_base_find_by_frame (struct frame_info *next_frame,
							  void **this_base_cache,
							  const struct frame_unwind *unwinder,
							  void **this_prologue_cache);

#endif
