/* Traditional frame unwind support, for GDB the GNU Debugger.

   Copyright 2003, 2004 Free Software Foundation, Inc.

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

struct frame_info;
struct trad_frame;

#include "frame.h"	/* For frame_id.  */

/* A traditional saved regs table, indexed by REGNUM, encoding where
   the value of REGNUM for the previous frame can be found in this
   frame.

   The table is initialized with an identity encoding (ADDR == -1,
   REALREG == REGNUM) indicating that the value of REGNUM in the
   previous frame can be found in register REGNUM (== REALREG) in this
   frame.

   The initial encoding can then be changed:

   Modify ADDR (REALREG >= 0, ADDR != -1) to indicate that the value
   of register REGNUM in the previous frame can be found in memory at
   ADDR in this frame (addr_p, !realreg_p, !value_p).

   Modify REALREG (REALREG >= 0, ADDR == -1) to indicate that the
   value of register REGNUM in the previous frame is found in register
   REALREG in this frame (!addr_p, realreg_p, !value_p).

   Call trad_frame_set_value (REALREG == -1) to indicate that the
   value of register REGNUM in the previous frame is found in ADDR
   (!addr_p, !realreg_p, value_p).

   Call trad_frame_set_unknown (REALREG == -2) to indicate that the
   register's value is not known.  */

struct trad_frame_saved_reg
{
  LONGEST addr; /* A CORE_ADDR fits in a longest.  */
  int realreg;
};

struct trad_frame_cache
{
  struct frame_id this_id;
  CORE_ADDR this_base;
  struct trad_frame_saved_reg *saved_regs;
};

struct trad_frame_cache *trad_frame_cache_zalloc (struct frame_info *next_frame);


/* Encode REGNUM's value in the trad-frame.  */
void trad_frame_set_reg_value (struct trad_frame_cache *this_cache,
			       int regnum, LONGEST val);
void trad_frame_set_reg_addr (struct trad_frame_cache *this_cache,
			      int regnum, CORE_ADDR addr);
void trad_frame_set_reg_reg (struct trad_frame_cache *this_cache,
			      int regnum, int realreg);
void trad_frame_set_reg_unknown (struct trad_frame_cache *this_cache,
				 int regnum);
void trad_frame_set_this_id (struct trad_frame_cache *this_cache,
			     struct frame_id *this_id);
void trad_frame_set_this_base (struct trad_frame_cache *this_cache,
			       CORE_ADDR base);

/* Set the offset of a register, and then update all offsets.  Useful
   when the offset of a register is known before its absolute
   address.  */
void trad_frame_set_reg_offset (struct trad_frame_cache *this_cache,
				int regnum, LONGEST addr);
void trad_frame_add_reg_addr (struct trad_frame_cache *this_cache,
			      int regnum, CORE_ADDR addr);

/* Convenience functions, return non-zero if the register has been
   encoded as specified.  */
int trad_frame_reg_value_p (struct trad_frame_cache *this_cache,
			    int regnum);
int trad_frame_reg_addr_p (struct trad_frame_cache *this_cache,
			   int regnum);
int trad_frame_reg_reg_p (struct trad_frame_cache *this_cache,
			  int regnum);

typedef struct trad_frame_cache *(trad_frame_sniffer_ftype) (const struct trad_frame *self,
							     struct frame_info *next_frame);

struct trad_frame
{
  enum frame_type type;
  trad_frame_sniffer_ftype *sniffer;
  const struct trad_frame_data *trad_data;
};

void trad_frame_append (struct gdbarch *gdbarch,
			const struct trad_frame *trad_frame);

void trad_frame_this_id (struct trad_frame_cache *trad_cache,
			 struct frame_info *next_frame,
			 struct frame_id *this_id);

void trad_frame_prev_register (struct trad_frame_cache *trad_cache,
			       struct frame_info *next_frame,
			       int prev_regnum,
			       int *optimizedp,
			       enum lval_type * lvalp,
			       CORE_ADDR *addrp,
			       int *realnump, void *valuep);

#endif
