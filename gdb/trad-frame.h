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

struct trad_frame
{
  /* If non-zero (and regnum >= 0), the stack address at which the
     register is saved.  By default, it is assumed that the register
     was not saved (addr == 0).  Remember, a LONGEST can always fit a
     CORE_ADDR.  */
  LONGEST addr;
  /* else, if regnum >=0 (and addr == 0), the REGNUM that contains
     this registers value.  By default, it is assumed that the
     registers are not moved (the register's value is still in that
     register and regnum == the index).  */
  int regnum;
  /* else, if regnum < 0, ADDR is the registers value.  */
};

/* Convenience function, encode the register's value in the
   trad-frame.  */
void trad_frame_register_value (struct trad_frame this_saved_regs[],
				int regnum, LONGEST val);

/* Return a freshly allocated (and initialized) trad_frame array.  */
struct trad_frame *trad_frame_alloc_saved_regs (struct frame_info *next_frame);

/* Given the trad_frame info, return the location of the specified
   register.  */
void trad_frame_prev_register (struct frame_info *next_frame,
			       struct trad_frame this_saved_regs[],
			       int regnum, int *optimizedp,
			       enum lval_type *lvalp, CORE_ADDR *addrp,
			       int *realnump, void *bufferp);

#endif
