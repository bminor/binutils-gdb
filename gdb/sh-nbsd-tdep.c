/* Target-dependent code for SuperH running NetBSD, for GDB.
   Copyright 2002 Free Software Foundation, Inc.
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
#include "regcache.h"
#include "value.h"
#include "solib-svr4.h"

#include "sh-nbsd-tdep.h"

/* Convert an r0-r15 register number into an offset into a ptrace
   register structure.  */
static const int regmap[] =
{
  (20 * 4),	/* r0 */
  (19 * 4),	/* r1 */
  (18 * 4),	/* r2 */ 
  (17 * 4),	/* r3 */ 
  (16 * 4),	/* r4 */
  (15 * 4),	/* r5 */
  (14 * 4),	/* r6 */
  (13 * 4),	/* r7 */
  (12 * 4),	/* r8 */ 
  (11 * 4),	/* r9 */
  (10 * 4),	/* r10 */
  ( 9 * 4),	/* r11 */
  ( 8 * 4),	/* r12 */
  ( 7 * 4),	/* r13 */
  ( 6 * 4),	/* r14 */
  ( 5 * 4),	/* r15 */
};

#define SIZEOF_STRUCT_REG (21 * 4)

void
sh_nbsd_supply_registers (char *regs)
{
  int regno;

  for (regno = R0_REGNUM; regno <= (R0_REGNUM + 15); regno++)
    supply_register (regno, regs + regmap[regno - R0_REGNUM]);

  supply_register (PC_REGNUM, regs + (0 * 4));
  supply_register (SR_REGNUM, regs + (1 * 4));
  supply_register (PR_REGNUM, regs + (2 * 4));
  supply_register (MACH_REGNUM, regs + (3 * 4));
  supply_register (MACL_REGNUM, regs + (4 * 4));
}

void
sh_nbsd_supply_register (char *regs, int regno)
{
  if (regno == -1)
    sh_nbsd_supply_registers (regs);

  if (regno == PC_REGNUM)
    supply_register (PC_REGNUM, regs + (0 * 4));
  else if (regno == SR_REGNUM)
    supply_register (SR_REGNUM, regs + (1 * 4));
  else if (regno == PR_REGNUM)
    supply_register (PR_REGNUM, regs + (2 * 4));
  else if (regno == MACH_REGNUM)
    supply_register (MACH_REGNUM, regs + (3 * 4));
  else if (regno == MACL_REGNUM)
    supply_register (MACL_REGNUM, regs + (4 * 4));
  else if (regno >= R0_REGNUM && regno <= (R0_REGNUM + 15))
    supply_register (regno, regs + regmap[regno - R0_REGNUM]);
}

void
sh_nbsd_fill_registers (char *regs)
{
  int regno;

  for (regno = R0_REGNUM; regno <= (R0_REGNUM + 15); regno++)
    regcache_collect (regno, regs + regmap[regno - R0_REGNUM]);

  regcache_collect (PC_REGNUM, regs + (0 * 4));
  regcache_collect (SR_REGNUM, regs + (1 * 4));
  regcache_collect (PR_REGNUM, regs + (2 * 4));
  regcache_collect (MACH_REGNUM, regs + (3 * 4));
  regcache_collect (MACL_REGNUM, regs + (4 * 4));
}

void
sh_nbsd_fill_register (char *regs, int regno)
{
  if (regno == -1)
    sh_nbsd_fill_registers (regs);

  if (regno == PC_REGNUM)
    regcache_collect (PC_REGNUM, regs + (0 * 4));
  else if (regno == SR_REGNUM)
    regcache_collect (SR_REGNUM, regs + (1 * 4));
  else if (regno == PR_REGNUM)
    regcache_collect (PR_REGNUM, regs + (2 * 4));
  else if (regno == MACH_REGNUM)
    regcache_collect (MACH_REGNUM, regs + (3 * 4));
  else if (regno == MACL_REGNUM)
    regcache_collect (MACL_REGNUM, regs + (4 * 4));
  else if (regno >= R0_REGNUM && regno <= (R0_REGNUM + 15))
    regcache_collect (regno, regs + regmap[regno - R0_REGNUM]);
}

/* Fetch (and possibly build) an appropriate link_map_offsets
   structure for NetBSD/sh targets using the struct offsets
   defined in <link.h> (but without actual reference to that file).

   This makes it possible to access NetBSD/sh shared libraries
   from a GDB that was not built on a NetBSD/sh host (for cross
   debugging).  */

static struct link_map_offsets *
sh_nbsd_solib_svr4_fetch_link_map_offsets (void)
{
  static struct link_map_offsets lmo;
  static struct link_map_offsets *lmp = NULL;

  if (lmp == NULL)
    {
      lmp = &lmo;

      lmo.r_debug_size = 16;

      lmo.r_map_offset = 4;
      lmo.r_map_size   = 4;

      lmo.link_map_size = 20;

      lmo.l_addr_offset = 0;
      lmo.l_addr_size   = 4;

      lmo.l_name_offset = 4;
      lmo.l_name_size   = 4;

      lmo.l_next_offset = 12;
      lmo.l_next_size   = 4;

      lmo.l_prev_offset = 16;
      lmo.l_prev_size   = 4;
    }

  return lmp;
}

static void
fetch_core_registers (char *core_reg_sect, unsigned core_reg_size,
                      int which, CORE_ADDR ignore)
{
  /* We get everything from the .reg section.  */
  if (which != 0)
    return;

  if (core_reg_size < SIZEOF_STRUCT_REG)
    {
      warning ("Wrong size register set in core file.");
      return;
    }

  /* Integer registers.  */
  sh_nbsd_supply_registers (core_reg_sect);
}

static void
fetch_elfcore_registers (char *core_reg_sect, unsigned core_reg_size,
                         int which, CORE_ADDR ignore)
{
  switch (which)
    {
    case 0:  /* Integer registers.  */
      if (core_reg_size != SIZEOF_STRUCT_REG)
	warning ("Wrong size register set in core file.");
      else
	sh_nbsd_supply_registers (core_reg_sect);
      break;

    default:
      /* Don't know what kind of register request this is; just ignore it.  */
      break;
    }
}

static struct core_fns sh_nbsd_core_fns =
{
  bfd_target_unknown_flavour,		/* core_flavour */
  default_check_format,			/* check_format */
  default_core_sniffer,			/* core_sniffer */
  fetch_core_registers,			/* core_read_registers */
  NULL					/* next */
};

static struct core_fns sh_nbsd_elfcore_fns =
{
  bfd_target_elf_flavour,		/* core_flavour */
  default_check_format,			/* check_format */
  default_core_sniffer,			/* core_sniffer */
  fetch_elfcore_registers,		/* core_read_registers */
  NULL					/* next */
};

static void
sh_nbsd_init_abi (struct gdbarch_info info,
                  struct gdbarch *gdbarch)
{
  set_solib_svr4_fetch_link_map_offsets (gdbarch,
		                   sh_nbsd_solib_svr4_fetch_link_map_offsets);
}

void
_initialize_sh_nbsd_tdep (void)
{
  add_core_fns (&sh_nbsd_core_fns);
  add_core_fns (&sh_nbsd_elfcore_fns);

  sh_gdbarch_register_os_abi (SH_OSABI_NETBSD_ELF, sh_nbsd_init_abi);
}
