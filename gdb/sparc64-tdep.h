/* Target-dependent code for UltraSPARC.

   Copyright 2003 Free Software Foundation, Inc.

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

#ifndef SPARC64_TDEP_H
#define SPARC62_TDEP_H 1

/* Register numbers of various important registers.  */

enum sparc_regnum
{
  SPARC_G0_REGNUM,		/* %g0 */
  SPARC_G1_REGNUM,
  SPARC_G2_REGNUM,
  SPARC_G3_REGNUM,
  SPARC_G4_REGNUM,
  SPARC_G5_REGNUM,
  SPARC_G6_REGNUM,
  SPARC_G7_REGNUM,		/* %g7 */
  SPARC_O0_REGNUM,		/* %o0 */
  SPARC_O1_REGNUM,
  SPARC_O2_REGNUM,
  SPARC_O3_REGNUM,
  SPARC_O4_REGNUM,
  SPARC_O5_REGNUM,
  SPARC_SP_REGNUM,		/* %sp (%o6) */
  SPARC_O7_REGNUM,		/* %o7 */
  SPARC_L0_REGNUM,		/* %l0 */
  SPARC_L1_REGNUM,
  SPARC_L2_REGNUM,
  SPARC_L3_REGNUM,
  SPARC_L4_REGNUM,
  SPARC_L5_REGNUM,
  SPARC_L6_REGNUM,
  SPARC_L7_REGNUM,		/* %l7 */
  SPARC_I0_REGNUM,		/* %i0 */
  SPARC_I1_REGNUM,
  SPARC_I2_REGNUM,
  SPARC_I3_REGNUM,
  SPARC_I4_REGNUM,
  SPARC_I5_REGNUM,
  SPARC_FP_REGNUM,		/* %fp (%i6) */
  SPARC_I7_REGNUM,		/* %i7 */
  SPARC_F0_REGNUM,		/* %f0 */
  SPARC_F31_REGNUM = SPARC_F0_REGNUM + 31 /* %f31 */
};

enum sparc64_regnum
{
  SPARC64_F32_REGNUM = SPARC_F0_REGNUM + 32,	/* %f32 */
  SPARC64_F62_REGNUM = SPARC64_F32_REGNUM + 15,	/* %f62 */
  SPARC64_PC_REGNUM,		/* %pc */
  SPARC64_NPC_REGNUM,		/* %npc */
  SPARC64_STATE_REGNUM,
  SPARC64_FSR_REGNUM,		/* %fsr */
  SPARC64_FPRS_REGNUM,		/* %fprs */
  SPARC64_Y_REGNUM,		/* %y */
  
  /* Pseudo registers.  */
  SPARC64_CWP_REGNUM,		/* %cwp */
  SPARC64_PSTATE_REGNUM,	/* %pstate */
  SPARC64_ASI_REGNUM,		/* %asi */
  SPARC64_CCR_REGNUM,		/* %ccr */
  SPARC64_D0_REGNUM,		/* %d0 */
  SPARC64_D10_REGNUM = SPARC64_D0_REGNUM + 5, /* %d10 */
  SPARC64_D30_REGNUM = SPARC64_D0_REGNUM + 15, /* %d30 */
  SPARC64_D32_REGNUM = SPARC64_D0_REGNUM + 16, /* %d32 */
  SPARC64_D62_REGNUM = SPARC64_D0_REGNUM + 31, /* %d62 */
  SPARC64_Q0_REGNUM,		/* %q0 */
  SPARC64_Q8_REGNUM = SPARC64_Q0_REGNUM + 2, /* %q8 */
  SPARC64_Q28_REGNUM = SPARC64_Q0_REGNUM + 7, /* %q28 */
  SPARC64_Q32_REGNUM = SPARC64_Q0_REGNUM + 8, /* %q32 */
  SPARC64_Q60_REGNUM = SPARC64_Q0_REGNUM + 15 /* %q60 */
};

extern void sparc_supply_rwindow (CORE_ADDR sp, int regnum);
extern void sparc_fill_rwindow (CORE_ADDR sp, int regnum);

/* Functions exported from sparc64fbsd-tdep.c.  */

extern void sparc64fbsd_supply_reg (const char *regs, int regnum);
extern void sparc64fbsd_fill_reg (char *regs, int regnum);
extern void sparc64fbsd_supply_fpreg (const char *regs, int regnum);
extern void sparc64fbsd_fill_fpreg (char *regs, int regnum);

#endif /* sparc64-tdep.h */
