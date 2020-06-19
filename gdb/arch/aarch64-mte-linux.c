/* Common Linux target-dependent functionality for AArch64 MTE

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

#include "arch/aarch64-mte-linux.h"

/* See arch/aarch64-mte-linux.h */

size_t
get_tag_granules (CORE_ADDR addr, size_t len, size_t granule_size)
{
  /* Start address */
  CORE_ADDR s_addr = align_down (addr, granule_size);
  /* End address */
  CORE_ADDR e_addr = align_down (addr + len, granule_size);

  /* We always have at least 1 granule.  */
  return 1 + (e_addr - s_addr) / granule_size;
}
