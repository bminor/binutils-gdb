/* Target-dependent code for HPUX running on PA-RISC, for GDB.
   Copyright 2002 Free Software Foundation, Inc.

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include "defs.h"
#include "arch-utils.h"
#include "osabi.h"

/* Forward declarations.  */
extern void _initialize_hppa_hpux_tdep (void);
extern initialize_file_ftype _initialize_hppa_hpux_tdep;

static void
hppa_hpux_som_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
}

static void
hppa_hpux_elf_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
}

void
_initialize_hppa_hpux_tdep (void)
{
  gdbarch_register_osabi (bfd_arch_hppa, 0, GDB_OSABI_HPUX_SOM,
                          hppa_hpux_som_init_abi);
  gdbarch_register_osabi (bfd_arch_hppa, 0, GDB_OSABI_HPUX_ELF,
                          hppa_hpux_elf_init_abi);
}
