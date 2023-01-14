/* Target-dependent code for GNU/Linux on LoongArch processors.

   Copyright (C) 2022 Free Software Foundation, Inc.
   Contributed by Loongson Ltd.

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
#include "glibc-tdep.h"
#include "inferior.h"
#include "linux-tdep.h"
#include "loongarch-tdep.h"
#include "solib-svr4.h"
#include "target-descriptions.h"

/* Unpack an elf_gregset_t into GDB's register cache.  */

static void
loongarch_supply_gregset (const struct regset *r,
			  struct regcache *regcache, int regno,
			  const void *gprs, size_t len)
{
  loongarch_gdbarch_tdep *tdep
    = (loongarch_gdbarch_tdep *) gdbarch_tdep (regcache->arch ());
  auto regs = tdep->regs;

  int regsize = register_size (regcache->arch (), regs.r);
  const gdb_byte *buf = nullptr;

  if (regno == -1)
    {
      /* Set $r0 = 0.  */
      regcache->raw_supply_zeroed (regs.r);

      for (int i = 1; i < 32; i++)
	{
	  buf = (const gdb_byte*) gprs + regsize * i;
	  regcache->raw_supply (regs.r + i, (const void *) buf);
	}

      /* Size base (pc) = regsize * regs.pc.  */
      buf = (const gdb_byte*) gprs + regsize * regs.pc;
      regcache->raw_supply (regs.pc, (const void *) buf);

      /* Size base (badv) = regsize * regs.badv.  */
      buf = (const gdb_byte*) gprs + regsize * regs.badv;
      regcache->raw_supply (regs.badv, (const void *) buf);
    }
  else if (regs.r == regno)
    regcache->raw_supply_zeroed (regno);
  else if ((regs.r < regno && regno < regs.r + 32)
	   || regs.pc == regno || regs.badv == regno)
    {
      /* Offset offset (regno) = regsize * (regno - regs.r).  */
      buf = (const gdb_byte*) gprs + regsize * (regno - regs.r);
      regcache->raw_supply (regno, (const void *) buf);
    }
}

/* Pack the GDB's register cache value into an elf_gregset_t.  */

static void
loongarch_fill_gregset (const struct regset *r,
			const struct regcache *regcache, int regno,
			void *gprs, size_t len)
{
  loongarch_gdbarch_tdep *tdep
    = (loongarch_gdbarch_tdep *) gdbarch_tdep (regcache->arch ());
  auto regs = tdep->regs;
  int regsize = register_size (regcache->arch (), regs.r);
  gdb_byte *buf = nullptr;

  if (regno == -1)
    {
      for (int i = 0; i < 32; i++)
	{
	  buf = (gdb_byte *) gprs + regsize * i;
	  regcache->raw_collect (regs.r + i, (void *) buf);
	}

      /* Size base (pc) = regsize * regs.pc.  */
      buf = (gdb_byte *) gprs + regsize * regs.pc;
      regcache->raw_collect (regs.pc, (void *) buf);

      /* Size base (badv) = regsize * regs.badv.  */
      buf = (gdb_byte *) gprs + regsize * regs.badv;
      regcache->raw_collect (regs.badv, (void *) buf);
    }
  else if ((regs.r <= regno && regno < regs.r + 32)
	   || regs.pc == regno || regs.badv == regno)
    {
      /* Offset offset (regno) = regsize * (regno - regs.r).  */
      buf = (gdb_byte *) gprs + regsize * (regno - regs.r);
      regcache->raw_collect (regno, (void *) buf);
    }
}

/* Register set definitions.  */

const struct regset loongarch_gregset =
{
  nullptr,
  loongarch_supply_gregset,
  loongarch_fill_gregset,
};

/* Initialize LoongArch Linux ABI info.  */

static void
loongarch_linux_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  linux_init_abi (info, gdbarch, 0);

  set_solib_svr4_fetch_link_map_offsets (gdbarch,
					 info.bfd_arch_info->bits_per_address == 32
					 ? linux_ilp32_fetch_link_map_offsets
					 : linux_lp64_fetch_link_map_offsets);

  /* GNU/Linux uses SVR4-style shared libraries.  */
  set_gdbarch_skip_trampoline_code (gdbarch, find_solib_trampoline_target);

  /* GNU/Linux uses the dynamic linker included in the GNU C Library.  */
  set_gdbarch_skip_solib_resolver (gdbarch, glibc_skip_solib_resolver);
  /* Enable TLS support.  */
  set_gdbarch_fetch_tls_load_module_address (gdbarch, svr4_fetch_objfile_link_map);
}

/* Initialize LoongArch Linux target support.  */

void _initialize_loongarch_linux_tdep ();
void
_initialize_loongarch_linux_tdep ()
{
  gdbarch_register_osabi (bfd_arch_loongarch, bfd_mach_loongarch32,
                          GDB_OSABI_LINUX, loongarch_linux_init_abi);
  gdbarch_register_osabi (bfd_arch_loongarch, bfd_mach_loongarch64,
                          GDB_OSABI_LINUX, loongarch_linux_init_abi);
}
