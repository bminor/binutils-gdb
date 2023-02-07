/* Common AArch64 Linux arch-specific definitions for the scalable
   extensions: SVE and SME.

   Copyright (C) 2023 Free Software Foundation, Inc.

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

#ifndef ARCH_AARCH64_SCALABLE_LINUX_H
#define ARCH_AARCH64_SCALABLE_LINUX_H

#include "gdbsupport/common-defs.h"

/* Feature check for Scalable Matrix Extension.  */
#ifndef HWCAP2_SME
#define HWCAP2_SME (1 << 23)
#endif

/* Streaming mode enabled/disabled bit.  */
#define SVCR_SM_BIT (1 << 0)
/* ZA enabled/disabled bit.  */
#define SVCR_ZA_BIT (1 << 1)
/* Mask including all valid SVCR bits.  */
#define SVCR_BIT_MASK (SVCR_SM_BIT | SVCR_ZA_BIT)

#endif /* ARCH_AARCH64_SCALABLE_LINUX_H */
