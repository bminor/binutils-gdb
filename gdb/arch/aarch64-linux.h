/* Common Linux target-dependent definitions for AArch64

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

#ifndef ARCH_AARCH64_LINUX_H
#define ARCH_AARCH64_LINUX_H

/* Feature check for Memory Tagging Extension.  */
#ifndef HWCAP2_MTE
#define HWCAP2_MTE  (1 << 18)
#endif

/* The MTE regset consists of 2 registers of 64-bit size.  */
#define AARCH64_LINUX_SIZEOF_MTE (2 * 64)

#endif /* ARCH_AARCH64_LINUX_H */
