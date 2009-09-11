/* Target dependent code for ARC processor family, for GDB, the GNU debugger.

   Copyright 2008, 2009 Free Software Foundation, Inc.

   Contributed by ARC International (www.arc.com)

   Authors:
      Richard Stuckey <richard.stuckey@arc.com>

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

/******************************************************************************/
/*                                                                            */
/* Outline:                                                                   */
/*     This header file defines operations for setting up the command line    */
/*     arguments to the program which is being debugged.                      */
/*                                                                            */
/******************************************************************************/

#ifndef ARC_ARGUMENTS
#define ARC_ARGUMENTS

/* ARC header files */
#include "arc-support.h"


void arc_program_loaded (void);

Boolean arc_setup_arguments (char *args);

void arc_restore_stack_top_address (void);


#endif /* ARC_ARGUMENTS */
/******************************************************************************/
