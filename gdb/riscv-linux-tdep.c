/* Target-dependent code for GNU/Linux on RISC-V processors.
   Copyright (C) 2018 Free Software Foundation, Inc.

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
#include "riscv-tdep.h"
#include "osabi.h"
#include "glibc-tdep.h"
#include "linux-tdep.h"
#include "solib-svr4.h"
#include "regset.h"

/* Define the general register mapping.  The kernel puts the PC at offset 0,
   gdb puts it at offset 32.  Register x0 is always 0 and can be ignored.
   Registers x1 to x31 are in the same place.  */

static const struct regcache_map_entry riscv_linux_gregmap[] =
{
  { 1,  RISCV_PC_REGNUM, 0 },
  { 31, RISCV_RA_REGNUM, 0 }, /* x1 to x31 */
  { 0 }
};

/* Define the general register regset.  */

static const struct regset riscv_linux_gregset =
{
  riscv_linux_gregmap, regcache_supply_regset, regcache_collect_regset
};

/* Define hook for core file support.  */

static void
riscv_linux_iterate_over_regset_sections (struct gdbarch *gdbarch,
                                          iterate_over_regset_sections_cb *cb,
                                          void *cb_data,
                                          const struct regcache *regcache)
{
  cb (".reg", (32 * riscv_isa_xlen (gdbarch)), (32 * riscv_isa_xlen (gdbarch)),
      &riscv_linux_gregset, NULL, cb_data);

  /* TODO: Add FP register support.  */
}

/* Initialize RISC-V Linux ABI info.  */

static void
riscv_linux_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  linux_init_abi (info, gdbarch);

  set_gdbarch_software_single_step (gdbarch, riscv_software_single_step);

  set_solib_svr4_fetch_link_map_offsets (gdbarch,
					 (riscv_isa_xlen (gdbarch) == 4
					  ? svr4_ilp32_fetch_link_map_offsets
					  : svr4_lp64_fetch_link_map_offsets));

  /* GNU/Linux uses SVR4-style shared libraries.  */
  set_gdbarch_skip_trampoline_code (gdbarch, find_solib_trampoline_target);

  /* GNU/Linux uses the dynamic linker included in the GNU C Library.  */
  set_gdbarch_skip_solib_resolver (gdbarch, glibc_skip_solib_resolver);

  /* Enable TLS support.  */
  set_gdbarch_fetch_tls_load_module_address (gdbarch,
                                             svr4_fetch_objfile_link_map);

  set_gdbarch_iterate_over_regset_sections
    (gdbarch, riscv_linux_iterate_over_regset_sections);
}

/* Initialize RISC-V Linux target support.  */

void
_initialize_riscv_linux_tdep (void)
{
  gdbarch_register_osabi (bfd_arch_riscv, 0, GDB_OSABI_LINUX,
			  riscv_linux_init_abi);
}
