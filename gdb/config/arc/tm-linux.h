/* Target dependent code for ARC processor family, for GDB, the GNU debugger.

   Copyright 2005, 2008, 2009 Free Software Foundation, Inc.

   Contributed by ARC International (www.arc.com)

   Authors:
      Codito Technologies Pvt. Ltd.
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
/*     This header file defines register numbers for the arc-linux-uclibc     */
/*     configuration of the ARC gdb.                                          */
/*                                                                            */
/******************************************************************************/

#ifndef ARC_TM_LINUX_H
#define ARC_TM_LINUX_H

#include "arc-linux-tdep.h"


#define CONFIG_OSABI        GDB_OSABI_LINUX

/* Do nothing.  */
#define CONFIG_INIT_TDEP    {}


enum arc700_linux_regnums
{
    /* Regnums 0 .. 26 are R0 .. R26 */
    ARC_BTA_REGNUM      = 27,
    ARC_LP_START_REGNUM = 28,
    ARC_LP_END_REGNUM   = 29,
    ARC_LP_COUNT_REGNUM = 30,
    ARC_STATUS32_REGNUM = 31,
    ARC_BLINK_REGNUM    = 32,
    ARC_FP_REGNUM       = 33,
    ARC_SP_REGNUM       = 34,
    ARC_EFA_REGNUM      = 35,
    ARC_RET_REGNUM      = 36,
    ARC_ORIG_R8_REGNUM  = 37,
    ARC_STOP_PC_REGNUM  = 38,

   /* end marker: this is not a register, but its integer value gives the number
    * of registers
    */
    ARC_REG_END_MARKER
};


/* Pseudo-registers.  */

enum arc700_linux_pseudo_regnums
{
    ARC_ILINK1_REGNUM = (int) ARC_REG_END_MARKER,
    ARC_ILINK2_REGNUM,
    ARC_ERET_REGNUM,
    ARC_STATUS32_L1_REGNUM,
    ARC_STATUS32_L2_REGNUM,
    ARC_ERSTATUS_REGNUM
};


#define ARC_NR_PSEUDO_REGS      6
#define ARC_NR_REGS             (int) ARC_REG_END_MARKER


#endif /* ARC_TM_LINUX_H */
/******************************************************************************/
