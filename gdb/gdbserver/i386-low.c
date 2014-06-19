/* Debug register code for the i386.

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

#include "server.h"
#include "i386-low.h"

/* Clear the reference counts and forget everything we knew about the
   debug registers.  */

void
i386_low_init_dregs (struct i386_debug_reg_state *state)
{
  int i;

  ALL_DEBUG_REGISTERS (i)
    {
      state->dr_mirror[i] = 0;
      state->dr_ref_count[i] = 0;
    }
  state->dr_control_mirror = 0;
  state->dr_status_mirror  = 0;
}
