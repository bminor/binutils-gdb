/* This file defines the part of the interface between the standalone
   simaulator program - run - and simulator library - libsim.a - that
   is not used by GDB.  The GDB part is described in include/remote-sim.h.
   
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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#ifdef SIM_TARGET_SWITCHES
  /* Parse the command line, extracting any target specific switches
     before the generic simulator code gets a chance to complain
     about them.  Returns the adjusted value of argc.  */
int sim_target_parse_command_line PARAMS ((int, char **));

  /* Display a list of target specific switches supported by this
     target.  */
void sim_target_display_usage PARAMS ((void));
#endif
