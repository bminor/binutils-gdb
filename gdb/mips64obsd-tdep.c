/* Target-dependent code for OpenBSD/mips64.

   Copyright 2004 Free Software Foundation, Inc.

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
#include "osabi.h"
#include "regcache.h"
#include "regset.h"

#include "gdb_string.h"

#include "mips-tdep.h"
#include "solib-svr4.h"

#define MIPS64OBSD_NUM_REGS 73

/* Core file support.  */

/* Supply register REGNUM from the buffer specified by GREGS and LEN
   in the general-purpose register set REGSET to register cache
   REGCACHE.  If REGNUM is -1, do this for all registers in REGSET.  */

static void
mips64obsd_supply_gregset (const struct regset *regset,
			   struct regcache *regcache, int regnum,
			   const void *gregs, size_t len)
{
  const char *regs = gregs;
  int i;

  for (i = 0; i < MIPS64OBSD_NUM_REGS; i++)
    {
      if (regnum == i || regnum == -1)
	regcache_raw_supply (regcache, i, regs + i * 8);
    }
}

/* OpenBSD/mips64 register set.  */

static struct regset mips64obsd_gregset =
{
  NULL,
  mips64obsd_supply_gregset
};

/* Return the appropriate register set for the core section identified
   by SECT_NAME and SECT_SIZE.  */

static const struct regset *
mips64obsd_regset_from_core_section (struct gdbarch *gdbarch,
				     const char *sect_name, size_t sect_size)
{
  if (strcmp (sect_name, ".reg") == 0 && sect_size >= MIPS64OBSD_NUM_REGS * 8)
    return &mips64obsd_gregset;

  return NULL;
}


static void
mips64obsd_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  set_gdbarch_regset_from_core_section
    (gdbarch, mips64obsd_regset_from_core_section);

  set_gdbarch_software_single_step (gdbarch, mips_software_single_step);

  set_solib_svr4_fetch_link_map_offsets
    (gdbarch, svr4_lp64_fetch_link_map_offsets);
}


/* Provide a prototype to silence -Wmissing-prototypes.  */
void _initialize_mips64obsd_tdep (void);

void
_initialize_mips64obsd_tdep (void)
{
  gdbarch_register_osabi (bfd_arch_mips, 0, GDB_OSABI_OPENBSD_ELF,
			  mips64obsd_init_abi);
}
