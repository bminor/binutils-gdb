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
struct frame_unwind;
struct frame_id;

/* If the PC falls in a dummy frame, return a dummy frame
   unwinder.  */

extern const struct frame_unwind *dummy_frame_sniffer (struct frame_info *next_frame);

/* Return the regcache that belongs to the dummy-frame identifed by PC
   and FP, or NULL if no such frame exists.  */
/* FIXME: cagney/2002-11-08: The function only exists because of
   deprecated_generic_get_saved_register.  Eliminate that function and
   this, to, can go.  */

extern struct regcache *deprecated_find_dummy_frame_regcache (CORE_ADDR pc,
							      CORE_ADDR fp);
#endif /* !defined (DUMMY_FRAME_H)  */
