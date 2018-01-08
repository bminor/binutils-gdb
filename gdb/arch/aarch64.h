/* Common target-dependent functionality for AArch64.

   Copyright (C) 2017-2018 Free Software Foundation, Inc.

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

#ifndef ARCH_AARCH64_H
#define ARCH_AARCH64_H

#include "tdesc.h"

target_desc *aarch64_create_target_description ();

/* Register numbers of various important registers.  */
enum aarch64_regnum
{
  AARCH64_X0_REGNUM,		/* First integer register.  */
  AARCH64_FP_REGNUM = AARCH64_X0_REGNUM + 29,	/* Frame register, if used.  */
  AARCH64_LR_REGNUM = AARCH64_X0_REGNUM + 30,	/* Return address.  */
  AARCH64_SP_REGNUM,		/* Stack pointer.  */
  AARCH64_PC_REGNUM,		/* Program counter.  */
  AARCH64_CPSR_REGNUM,		/* Current Program Status Register.  */
  AARCH64_V0_REGNUM,		/* First fp/vec register.  */
  AARCH64_V31_REGNUM = AARCH64_V0_REGNUM + 31,	/* Last fp/vec register.  */
  AARCH64_FPSR_REGNUM,		/* Floating Point Status Register.  */
  AARCH64_FPCR_REGNUM,		/* Floating Point Control Register.  */

  /* Other useful registers.  */
  AARCH64_LAST_X_ARG_REGNUM = AARCH64_X0_REGNUM + 7,
  AARCH64_STRUCT_RETURN_REGNUM = AARCH64_X0_REGNUM + 8,
  AARCH64_LAST_V_ARG_REGNUM = AARCH64_V0_REGNUM + 7
};

#define AARCH64_X_REGS_NUM 31
#define AARCH64_V_REGS_NUM 32
#define AARCH64_NUM_REGS AARCH64_FPCR_REGNUM + 1

#endif /* ARCH_AARCH64_H */
