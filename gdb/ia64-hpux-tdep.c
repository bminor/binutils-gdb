/* Target-dependent code for the IA-64 for GDB, the GNU debugger.

   Copyright (C) 2010 Free Software Foundation, Inc.

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
#include "ia64-tdep.h"
#include "ia64-hpux-tdep.h"
#include "osabi.h"
#include "gdbtypes.h"
#include "solib.h"

/* Return nonzero if the value of the register identified by REGNUM
   can be modified.  */

static int
ia64_hpux_can_store_ar_register (int regnum)
{
  switch (regnum)
    {
      case IA64_RSC_REGNUM:
      case IA64_RNAT_REGNUM:
      case IA64_CSD_REGNUM:
      case IA64_SSD_REGNUM:
      case IA64_CCV_REGNUM:
      case IA64_UNAT_REGNUM:
      case IA64_FPSR_REGNUM:
      case IA64_PFS_REGNUM:
      case IA64_LC_REGNUM:
      case IA64_EC_REGNUM:
         return 1;
         break;

      default:
         return 0;
         break;
    }
}

/* The "cannot_store_register" target_ops method.  */

static int
ia64_hpux_cannot_store_register (struct gdbarch *gdbarch, int regnum)
{
  /* General registers.  */

  if (regnum == IA64_GR0_REGNUM)
    return 1;

  /* FP register.  */

  if (regnum == IA64_FR0_REGNUM || regnum == IA64_FR1_REGNUM)
    return 1;

  /* Application registers.  */
  if (regnum >= IA64_AR0_REGNUM && regnum <= IA64_AR0_REGNUM + 127)
    return (!ia64_hpux_can_store_ar_register (regnum));

  /* We can store all other registers.  */
  return 0;
}

/* Should be set to non-NULL if the ia64-hpux solib module is linked in.
   This may not be the case because the shared library support code can
   only be compiled on ia64-hpux.  */

struct target_so_ops *ia64_hpux_so_ops = NULL;

static void
ia64_hpux_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  set_gdbarch_long_double_format (gdbarch, floatformats_ia64_quad);
  set_gdbarch_cannot_store_register (gdbarch, ia64_hpux_cannot_store_register);

  if (ia64_hpux_so_ops)
    set_solib_ops (gdbarch, ia64_hpux_so_ops);
}

/* Provide a prototype to silence -Wmissing-prototypes.  */
extern initialize_file_ftype _initialize_ia64_hpux_tdep;

void
_initialize_ia64_hpux_tdep (void)
{
  gdbarch_register_osabi (bfd_arch_ia64, 0, GDB_OSABI_HPUX_ELF,
			  ia64_hpux_init_abi);
}
