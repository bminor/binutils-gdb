/* Native-dependent code for FreeBSD/sparc64.

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

#include "defs.h"

#include "sparc64-tdep.h"
#include "sparcbsd-nat.h"

/* Determine whether `struct reg' contains register REGNUM.  */

static int
sparc64fbsd_reg_supplies_p (int regnum)
{
  /* Integer registers.  */
  if ((regnum >= SPARC_G0_REGNUM && regnum <= SPARC_G7_REGNUM)
      || (regnum >= SPARC_O0_REGNUM && regnum <= SPARC_O7_REGNUM)
      || (regnum >= SPARC_L0_REGNUM && regnum <= SPARC_L7_REGNUM)
      || (regnum >= SPARC_I0_REGNUM && regnum <= SPARC_I7_REGNUM))
    return 1;

  /* Control registers.  */
  if (regnum == SPARC64_PC_REGNUM
      || regnum == SPARC64_NPC_REGNUM
      || regnum == SPARC64_STATE_REGNUM
      || regnum == SPARC64_FPRS_REGNUM
      || regnum == SPARC64_Y_REGNUM)
    return 1;

  return 0;
}

/* Determine whether `struct fpreg' contains register REGNUM.  */

static int
sparc64fbsd_fpreg_supplies_p (int regnum)
{
  /* Floating-point registers.  */
  if ((regnum >= SPARC_F0_REGNUM && regnum <= SPARC_F31_REGNUM)
      || (regnum >= SPARC64_F32_REGNUM && regnum <= SPARC64_F62_REGNUM))
    return 1;

  /* Control registers.  */
  if (regnum == SPARC64_FSR_REGNUM)
    return 1;

  return 0;
}

/* Provide a prototype to silence -Wmissing-prototypes.  */
void _initialize_sparc64fbsd_nat (void);

void
_initialize_sparc64fbsd_nat (void)
{
  sparcbsd_supply_reg = sparc64fbsd_supply_reg;
  sparcbsd_fill_reg = sparc64fbsd_fill_reg;
  sparcbsd_supply_fpreg = sparc64fbsd_supply_fpreg;
  sparcbsd_fill_fpreg = sparc64fbsd_fill_fpreg;

  sparcbsd_reg_supplies_p = sparc64fbsd_reg_supplies_p;
  sparcbsd_fpreg_supplies_p = sparc64fbsd_fpreg_supplies_p;
}
