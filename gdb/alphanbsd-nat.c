/* Native-dependent code for Alpha NetBSD.
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
#include <sys/types.h>
#include <machine/reg.h>
#include <machine/frame.h>  
#include <machine/pcb.h>
#include "gdbcore.h"
#include "regcache.h"

#include "alpha-tdep.h"
#include "alphabsd-tdep.h"

static void
fetch_core_registers (char *core_reg_sect, unsigned core_reg_size, int which,
                      CORE_ADDR ignore)
{
  struct md_coredump *core_reg;
  char *regs;
  int regno;

  /* Table to map a gdb register number to a trapframe register index.  */
  static const int regmap[] =
  {
    FRAME_V0,  FRAME_T0,  FRAME_T1,  FRAME_T2,
    FRAME_T3,  FRAME_T4,  FRAME_T5,  FRAME_T6,
    FRAME_T7,  FRAME_S0,  FRAME_S1,  FRAME_S2,
    FRAME_S3,  FRAME_S4,  FRAME_S5,  FRAME_S6,
    FRAME_A0,  FRAME_A1,  FRAME_A2,  FRAME_A3,
    FRAME_A4,  FRAME_A5,  FRAME_T8,  FRAME_T9,
    FRAME_T10, FRAME_T11, FRAME_RA,  FRAME_T12,
    FRAME_AT,  FRAME_GP,  FRAME_SP
  };

  /* We get everything from one section.  */
  if (which != 0)
    return;

  core_reg = (struct md_coredump *) core_reg_sect;
  regs = (char *) &core_reg->md_tf;

  if (core_reg_size < sizeof (*core_reg))
    {
      warning ("Wrong size register set in core file.");
      return;
    }

  /* Integer registers.  */
  for (regno = 0; regno < ALPHA_ZERO_REGNUM; regno++)
    supply_register (regno, regs + (regmap[regno] * 8));
  supply_register (ALPHA_ZERO_REGNUM, NULL);
  supply_register (FP_REGNUM, NULL);
  supply_register (PC_REGNUM, regs + (FRAME_PC * 8));

  /* Floating point registers.  */
  alphabsd_supply_fpreg ((char *) &core_reg->md_fpstate, -1);
}

static void
fetch_elfcore_registers (char *core_reg_sect, unsigned core_reg_size, int which,
                         CORE_ADDR ignore)
{
  switch (which)
    {
    case 0:  /* Integer registers.  */
      if (core_reg_size != SIZEOF_STRUCT_REG)
	warning ("Wrong size register set in core file.");
      else
	alphabsd_supply_reg (core_reg_sect, -1);
      break;

    case 2:  /* Floating point registers.  */
      if (core_reg_size != SIZEOF_STRUCT_FPREG)
	warning ("Wrong size FP register set in core file.");
      else
	alphabsd_supply_fpreg (core_reg_sect, -1);
      break;

    default:
      /* Don't know what kind of register request this is; just ignore it.  */
      break;
    }
}

static struct core_fns alphanbsd_core_fns =
{
  bfd_target_unknown_flavour,		/* core_flavour */
  default_check_format,			/* check_format */
  default_core_sniffer,			/* core_sniffer */
  fetch_core_registers,			/* core_read_registers */
  NULL					/* next */
};

static struct core_fns alphanbsd_elfcore_fns =
{
  bfd_target_elf_flavour,		/* core_flavour */
  default_check_format,			/* check_format */
  default_core_sniffer,			/* core_sniffer */
  fetch_elfcore_registers,		/* core_read_registers */
  NULL					/* next */
};

void
_initialize_alphanbsd_nat (void)
{
  add_core_fns (&alphanbsd_core_fns);
  add_core_fns (&alphanbsd_elfcore_fns);
}
