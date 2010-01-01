/* Copyright (C) 2009, 2010 Free Software Foundation, Inc.

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

#include "defs.h"
#include "osabi.h"
#include "amd64-tdep.h"
#include "solib.h"
#include "solib-target.h"

static void
amd64_windows_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  amd64_init_abi (info, gdbarch);

  /* On Windows, "long"s are only 32bit.  */
  set_gdbarch_long_bit (gdbarch, 32);

  set_solib_ops (gdbarch, &solib_target_so_ops);
}

void
_initialize_amd64_windows_tdep (void)
{
  gdbarch_register_osabi (bfd_arch_i386, bfd_mach_x86_64, GDB_OSABI_CYGWIN,
                          amd64_windows_init_abi);
}

