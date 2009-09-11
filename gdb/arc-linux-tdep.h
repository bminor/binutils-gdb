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
/*     This header file defines the type and operations for the Linux-uclibc  */
/*     variant of the ARC debugger.                                           */
/*                                                                            */
/******************************************************************************/

#ifndef ARC_LINUX_TDEP_H
#define ARC_LINUX_TDEP_H

/* gdb header files */
#include "defs.h"
#include "gdbarch.h"


/* Complete the structure definition here.  */
struct arc_variant_info
{
   /* There is currently no variant-specific info.  */
};


void arc_check_pc_defined (struct gdbarch *gdbarch);

#endif /* ARC_LINUX_TDEP_H */
/******************************************************************************/
