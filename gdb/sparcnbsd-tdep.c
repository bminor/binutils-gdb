/* Target-dependent code for NetBSD/sparc.

   Copyright 2002, 2003 Free Software Foundation, Inc.
   Contributed by Wasabi Systems, Inc.

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
#include "gdbcore.h"
#include "osabi.h"
#include "regcache.h"
#include "solib-svr4.h"

#include "gdb_string.h"

#include "sparc-tdep.h"
#include "nbsd-tdep.h"

const struct sparc_gregset sparc32nbsd_gregset =
{
  0 * 4,			/* %psr */
  1 * 4,			/* %pc */
  2 * 4,			/* %npc */
  3 * 4,			/* %y */
  -1,				/* %wim */
  -1,				/* %tbr */
  5 * 4,			/* %g1 */
  -1				/* %l0 */
};

/* Unlike other NetBSD implementations, the SPARC port historically
   used .reg and .reg2 (see bfd/netbsd-core.c), and as such, we can
   share one routine for a.out and ELF core files.  */

static void
fetch_core_registers (char *core_reg_sect, unsigned core_reg_size, int which,
                      CORE_ADDR ignore)
{
  int reg_size = 20 * 4;
  int fpreg_size = 33 * 4;

  switch (which)
    {
    case 0:  /* Integer registers.  */
      if (core_reg_size != reg_size)
	warning ("Wrong size register set in core file.");
      sparc32_supply_gregset (&sparc32nbsd_gregset, current_regcache,
			      -1, core_reg_sect);
      break;

    case 2:  /* Floating pointer registers.  */
      if (core_reg_size != fpreg_size)
	warning ("Wrong size FP register set in core file.");
      sparc32_supply_fpregset (current_regcache, -1, core_reg_sect);
      break;

    default:
      /* Don't know what kind of register request this is; just ignore it.  */
      break;
    }
}

static struct core_fns sparcnbsd_core_fns =
{
  bfd_target_unknown_flavour,		/* core_flavour */
  default_check_format,			/* check_format */
  default_core_sniffer,			/* core_sniffer */
  fetch_core_registers,			/* core_read_registers */
  NULL
};

static struct core_fns sparcnbsd_elfcore_fns =
{
  bfd_target_elf_flavour,		/* core_flavour */
  default_check_format,			/* check_format */
  default_core_sniffer,			/* core_sniffer */
  fetch_core_registers,			/* core_read_registers */
  NULL
};

/* Return non-zero if we are in a shared library trampoline code stub.  */

static int
sparcnbsd_aout_in_solib_call_trampoline (CORE_ADDR pc, char *name)
{
  return (name && !strcmp (name, "_DYNAMIC"));
}

static void
sparc32nbsd_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  /* NetBSD doesn't support the 128-bit `long double' from the psABI.  */
  set_gdbarch_long_double_bit (gdbarch, 64);
}

static void
sparc32nbsd_aout_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  sparc32nbsd_init_abi (info, gdbarch);

  set_gdbarch_in_solib_call_trampoline
    (gdbarch, sparcnbsd_aout_in_solib_call_trampoline);
}

static void
sparc32nbsd_elf_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  sparc32nbsd_init_abi (info, gdbarch);

  set_gdbarch_pc_in_sigtramp (gdbarch, nbsd_pc_in_sigtramp);

  set_solib_svr4_fetch_link_map_offsets
    (gdbarch, nbsd_ilp32_solib_svr4_fetch_link_map_offsets);
}

static enum gdb_osabi
sparcnbsd_aout_osabi_sniffer (bfd *abfd)
{
  if (strcmp (bfd_get_target (abfd), "a.out-sparc-netbsd") == 0)
    return GDB_OSABI_NETBSD_AOUT;

  return GDB_OSABI_UNKNOWN;
}


/* Provide a prototype to silence -Wmissing-prototypes.  */
void _initialize_sparcnbsd_tdep (void);

void
_initialize_sparnbsd_tdep (void)
{
  gdbarch_register_osabi_sniffer (bfd_arch_sparc, bfd_target_aout_flavour,
				  sparcnbsd_aout_osabi_sniffer);

  gdbarch_register_osabi (bfd_arch_sparc, 0, GDB_OSABI_NETBSD_AOUT,
			  sparc32nbsd_aout_init_abi);
  gdbarch_register_osabi (bfd_arch_sparc, 0, GDB_OSABI_NETBSD_ELF,
			  sparc32nbsd_elf_init_abi);

  add_core_fns (&sparcnbsd_core_fns);
  add_core_fns (&sparcnbsd_elfcore_fns);
}
