/* Target-specific functions for ARM running under NetBSD.
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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include "defs.h"

#include "arm-tdep.h"

/* For compatibility with previous implemenations of GDB on arm/NetBSD,
   override the default little-endian breakpoint.  */
static const char arm_nbsd_arm_le_breakpoint[] = {0x11, 0x00, 0x00, 0xe6};

static void
arm_netbsd_init_abi_common (struct gdbarch_info info,
			    struct gdbarch *gdbarch)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

  tdep->lowest_pc = 0x8000;
  tdep->arm_breakpoint = arm_nbsd_arm_le_breakpoint;
  tdep->arm_breakpoint_size = sizeof (arm_nbsd_arm_le_breakpoint);
}
  
static void
arm_netbsd_aout_init_abi (struct gdbarch_info info, 
			  struct gdbarch *gdbarch)
{
  arm_netbsd_init_abi_common (info, gdbarch);
}

static void
arm_netbsd_elf_init_abi (struct gdbarch_info info, 
			 struct gdbarch *gdbarch)
{
  arm_netbsd_init_abi_common (info, gdbarch);
}

void
_initialize_arm_netbsd_tdep (void)
{
  arm_gdbarch_register_os_abi (ARM_ABI_NETBSD_AOUT, arm_netbsd_aout_init_abi);
  arm_gdbarch_register_os_abi (ARM_ABI_NETBSD_ELF, arm_netbsd_elf_init_abi);
}
