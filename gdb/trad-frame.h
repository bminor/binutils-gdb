/* Traditional frame unwind support, for GDB the GNU Debugger.

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

#ifndef TRAD_FRAME_H
#define TRAD_FRAME_H

/* A traditional saved regs table, indexed by REGNUM, encoding where
   the value of REGNUM for the previous frame can be found in this
   frame.

   The table is initialized with an identity encoding (ADDR == 0,
   REALNUM == REGNUM) indicating that the value of REGNUM in the
   previous frame can be found in register REGNUM (== REALNUM) in this
   frame.

   The initial encoding can then be changed:

   Modify ADDR (REALNUM >= 0, ADDR != 0) to indicate that the value of
   register REGNUM in the previous frame can be found in memory at
   ADDR in this frame.

   Modify REALNUM (REALNUM >= 0, ADDR == 0) to indicate that the value
   of register REGNUM in the previous frame is found in register
   REALNUM in this frame.

   Call trad_frame_register_value (REALNUM < 0) to indicate that the
   value of register REGNUM in the previous frame is found in ADDR.  */

struct trad_frame_saved_reg
{
  LONGEST addr; /* A CORE_ADDR fits in a longest.  */
  int realnum;
};

/* Convenience function, encode REGNUM's location in the trad-frame.  */
void trad_frame_register_value (struct trad_frame_saved_reg this_saved_regs[],
				int regnum, LONGEST val);

/* Return a freshly allocated (and initialized) trad_frame array.  */
struct trad_frame_saved_reg *trad_frame_alloc_saved_regs (struct frame_info *next_frame);

/* Given the trad_frame info, return the location of the specified
   register.  */
void trad_frame_prev_register (struct frame_info *next_frame,
			       struct trad_frame_saved_reg this_saved_regs[],
			       int regnum, int *optimizedp,
			       enum lval_type *lvalp, CORE_ADDR *addrp,
			       int *realnump, void *bufferp);

#endif
