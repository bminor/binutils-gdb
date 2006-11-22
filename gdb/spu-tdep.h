/* SPU target-dependent code for GDB, the GNU debugger.
   Copyright (C) 2006 Free Software Foundation, Inc.

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
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

#ifndef SPU_TDEP_H
#define SPU_TDEP_H

/* Number of registers.  */
#define SPU_NUM_REGS         130
#define SPU_NUM_PSEUDO_REGS  1
#define SPU_NUM_GPRS	     128

/* Register numbers of various important registers.  */
enum spu_regnum
{
  /* SPU calling convention.  */
  SPU_LR_REGNUM = 0,		/* Link register.  */
  SPU_RAW_SP_REGNUM = 1,	/* Stack pointer (full register).  */
  SPU_ARG1_REGNUM = 3,		/* First argument register.  */
  SPU_ARGN_REGNUM = 74,		/* Last argument register.  */
  SPU_SAVED1_REGNUM = 80,	/* First call-saved register.  */
  SPU_SAVEDN_REGNUM = 127,	/* Last call-saved register.  */
  SPU_FP_REGNUM = 127,		/* Frame pointer.  */

  /* Special registers.  */
  SPU_ID_REGNUM = 128,		/* SPU ID register.  */
  SPU_PC_REGNUM = 129,		/* Next program counter.  */
  SPU_SP_REGNUM = 130		/* Stack pointer (preferred slot).  */
};

/* Local store.  */
#define SPU_LS_SIZE          0x40000

#endif
