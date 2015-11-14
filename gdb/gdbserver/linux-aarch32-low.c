/* Copyright (C) 1995-2015 Free Software Foundation, Inc.

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

#include "server.h"
#include "arch/arm.h"
#include "linux-low.h"
#include "linux-aarch32-low.h"

#include <sys/ptrace.h>
/* Don't include elf.h if linux/elf.h got included by gdb_proc_service.h.
   On Bionic elf.h and linux/elf.h have conflicting definitions.  */
#ifndef ELFMAG0
#include <elf.h>
#endif

/* Some older versions of GNU/Linux and Android do not define
   the following macros.  */
#ifndef NT_ARM_VFP
#define NT_ARM_VFP 0x400
#endif

/* Collect GP registers from REGCACHE to buffer BUF.  */

void
arm_fill_gregset (struct regcache *regcache, void *buf)
{
  int i;
  uint32_t *regs = (uint32_t *) buf;

  for (i = ARM_A1_REGNUM; i <= ARM_PC_REGNUM; i++)
    collect_register (regcache, i, &regs[i]);

  collect_register (regcache, ARM_PS_REGNUM, &regs[16]);
}

/* Supply GP registers contents, stored in BUF, to REGCACHE.  */

void
arm_store_gregset (struct regcache *regcache, const void *buf)
{
  int i;
  char zerobuf[8];
  const uint32_t *regs = (const uint32_t *) buf;

  memset (zerobuf, 0, 8);
  for (i = ARM_A1_REGNUM; i <= ARM_PC_REGNUM; i++)
    supply_register (regcache, i, &regs[i]);

  for (; i < ARM_PS_REGNUM; i++)
    supply_register (regcache, i, zerobuf);

  supply_register (regcache, ARM_PS_REGNUM, &regs[16]);
}

/* Collect NUM number of VFP registers from REGCACHE to buffer BUF.  */

void
arm_fill_vfpregset_num (struct regcache *regcache, void *buf, int num)
{
  int i, base;

  gdb_assert (num == 16 || num == 32);

  base = find_regno (regcache->tdesc, "d0");
  for (i = 0; i < num; i++)
    collect_register (regcache, base + i, (char *) buf + i * 8);

  collect_register_by_name (regcache, "fpscr", (char *) buf + 32 * 8);
}

/* Supply NUM number of VFP registers contents, stored in BUF, to
   REGCACHE.  */

void
arm_store_vfpregset_num (struct regcache *regcache, const void *buf, int num)
{
  int i, base;

  gdb_assert (num == 16 || num == 32);

  base = find_regno (regcache->tdesc, "d0");
  for (i = 0; i < num; i++)
    supply_register (regcache, base + i, (char *) buf + i * 8);

  supply_register_by_name (regcache, "fpscr", (char *) buf + 32 * 8);
}

static void
arm_fill_vfpregset (struct regcache *regcache, void *buf)
{
  arm_fill_vfpregset_num (regcache, buf, 32);
}

static void
arm_store_vfpregset (struct regcache *regcache, const void *buf)
{
  arm_store_vfpregset_num (regcache, buf, 32);
}

/* Register sets with using PTRACE_GETREGSET.  */

static struct regset_info aarch32_regsets[] = {
  { PTRACE_GETREGSET, PTRACE_SETREGSET, NT_PRSTATUS, 18 * 4,
    GENERAL_REGS,
    arm_fill_gregset, arm_store_gregset },
  { PTRACE_GETREGSET, PTRACE_SETREGSET, NT_ARM_VFP, 32 * 8 + 4,
    EXTENDED_REGS,
    arm_fill_vfpregset, arm_store_vfpregset },
  NULL_REGSET
};

static struct regsets_info aarch32_regsets_info =
  {
    aarch32_regsets, /* regsets */
    0, /* num_regsets */
    NULL, /* disabled_regsets */
  };

struct regs_info regs_info_aarch32 =
  {
    NULL, /* regset_bitmap */
    NULL, /* usrregs */
    &aarch32_regsets_info
  };

void
initialize_low_arch_aarch32 (void)
{
  init_registers_arm_with_neon ();

  initialize_regsets_info (&aarch32_regsets_info);
}
