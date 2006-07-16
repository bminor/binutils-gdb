/* Target-dependent code for OpenBSD/arm.

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

#include "defs.h"
#include "osabi.h"

#include "arm-tdep.h"
#include "solib-svr4.h"

static void
armobsd_init_abi (struct gdbarch_info info,
		  struct gdbarch *gdbarch)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

  if (tdep->fp_model == ARM_FLOAT_AUTO)
    tdep->fp_model = ARM_FLOAT_SOFT_VFP;

  /* OpenBSD/arm uses SVR4-style shared libraries.  */
  set_solib_svr4_fetch_link_map_offsets
    (gdbarch, svr4_ilp32_fetch_link_map_offsets);

  tdep->jb_pc = 24;
  tdep->jb_elt_size = 4;
}

void
_initialize_armobsd_tdep (void)
{
  gdbarch_register_osabi (bfd_arch_arm, 0, GDB_OSABI_OPENBSD_ELF,
			  armobsd_init_abi);
}
