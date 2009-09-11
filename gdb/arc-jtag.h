/* Target dependent code for ARC processor family, for GDB, the GNU debugger.

   Copyright 2005, 2008, 2009 Free Software Foundation, Inc.

   Contributed by Codito Technologies Pvt. Ltd. (www.codito.com)

   Authors:
      Soam Vasani     <soam.vasani@codito.com>
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
/*     This header file defines some operations provided by the ARC JTAG      */
/*     module.                                                                */
/*                                                                            */
/******************************************************************************/

#ifndef ARC_JTAG_H
#define ARC_JTAG_H

/* ARC header files */
#include "arc-support.h"


/* Operations for reading/writing core/auxiliary registers; these must be used
   when access to the registers *specifically* via the JTAG i/f is required.

   N.B. the register contents returned by these functions, or supplied to them,
        are in host byte order.  */

Boolean arc_read_jtag_core_register  (ARC_RegisterNumber    hw_regno,
                                      ARC_RegisterContents *contents,
                                      Boolean               warn_on_failure);

Boolean arc_write_jtag_core_register (ARC_RegisterNumber    hw_regno,
                                      ARC_RegisterContents  contents,
                                      Boolean               warn_on_failure);

Boolean arc_read_jtag_aux_register   (ARC_RegisterNumber    hw_regno,
                                      ARC_RegisterContents *contents,
                                      Boolean               warn_on_failure);

Boolean arc_write_jtag_aux_register  (ARC_RegisterNumber    hw_regno,
                                      ARC_RegisterContents  contents,
                                      Boolean               warn_on_failure);

#endif /* ARC_JTAG_H */
/******************************************************************************/
