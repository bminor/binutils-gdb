/* Target-dependent code for Mitsubishi D10V, for GDB.

   Copyright 1996, 1997, 1998, 1999, 2000, 2001, 2002, 2003 Free Software
   Foundation, Inc.

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

#ifndef D10V_TDEP_H
#define D10V_TDEP_H

/* d10v register names. */

enum
{
  D10V_R0_REGNUM = 0,
  D10V_R3_REGNUM = 3,
  D10V_FP_REGNUM = 11,
  D10V_LR_REGNUM = 13,
  D10V_SP_REGNUM = 15,
  D10V_PSW_REGNUM = 16,
  D10V_PC_REGNUM = 18,
  D10V_NR_IMAP_REGS = 2,
  D10V_NR_A_REGS = 2,
  D10V_TS2_NUM_REGS = 37,
  D10V_TS3_NUM_REGS = 42,
  /* d10v calling convention. */
  D10V_ARG1_REGNUM = D10V_R0_REGNUM,
  D10V_ARGN_REGNUM = D10V_R3_REGNUM,
  D10V_RET1_REGNUM = D10V_R0_REGNUM,
};

/* These are the addresses the D10V-EVA board maps data and
   instruction memory to. */

enum memspace {
  D10V_DMEM_START  = 0x2000000,
  D10V_IMEM_START  = 0x1000000,
  D10V_STACK_START = 0x200bffe
};

extern CORE_ADDR d10v_make_iaddr (CORE_ADDR x);
extern CORE_ADDR d10v_make_daddr (CORE_ADDR x);
extern CORE_ADDR d10v_read_sp (void);
extern CORE_ADDR d10v_read_fp (void);
extern int d10v_a0_regnum (struct gdbarch *gdbarch);

#endif
