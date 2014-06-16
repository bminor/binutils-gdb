/* Misc. low level support for i386.

   Copyright (C) 2009-2014 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */


#include "nat/i386-dregs.h"

/* Initialize STATE.  */
extern void i386_low_init_dregs (struct i386_debug_reg_state *state);


/* Each target needs to provide several low-level functions
   that will be called to insert watchpoints and hardware breakpoints
   into the inferior, remove them, and check their status.  These
   functions are:

      i386_dr_low_set_control  -- set the debug control (DR7)
				  register to a given value

      i386_dr_low_set_addr     -- put an address into one debug register

      i386_dr_low_get_status   -- return the value of the debug
				  status (DR6) register.
*/

/* Can we update the inferior's debug registers?  */
#define i386_dr_low_can_set_addr() 1

/* Update the inferior's debug register REGNUM from STATE.  */
extern void i386_dr_low_set_addr (const struct i386_debug_reg_state *state,
				  int regnum);

/* Return the inferior's debug register REGNUM.  */
extern CORE_ADDR i386_dr_low_get_addr (int regnum);

/* Can we update the inferior's DR7 control register?  */
#define i386_dr_low_can_set_control() 1

/* Update the inferior's DR7 debug control register from STATE.  */
extern void i386_dr_low_set_control (const struct i386_debug_reg_state *state);

/* Return the value of the inferior's DR7 debug control register.  */
extern unsigned i386_dr_low_get_control (void);

/* Return the value of the inferior's DR6 debug status register.  */
extern unsigned i386_dr_low_get_status (void);

/* Return the debug register size, in bytes.  */
/* Note that sizeof (long) == 4 on win64.  */
#define i386_get_debug_register_length() (sizeof (void *))
