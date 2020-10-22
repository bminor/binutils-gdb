/* none on ARM target support, prototypes.

   Copyright (C) 2020 Free Software Foundation, Inc.

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

#ifndef ARM_NONE_TDEP_H
#define ARM_NONE_TDEP_H

#include "bfd.h"
#include "regset.h"

struct regcache;

/* Core file and register set support.  */
#define ARM_NONE_SIZEOF_GREGSET (18 * ARM_INT_REGISTER_SIZE)

/* Support VFP register format.  */
#define ARM_NONE_SIZEOF_VFP (32 * 8 + 4)

/* The index to access CSPR in user_regs as defined in GLIBC.  */
#define ARM_NONE_CPSR_GREGNUM 16

void
  arm_none_iterate_over_regset_sections (struct gdbarch *gdbarch,
					 iterate_over_regset_sections_cb *cb,
					 void *cb_data,
					 const struct regcache *regcache);
#endif /* ARM_NONE_TDEP_H */
