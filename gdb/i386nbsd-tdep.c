/* Target-dependent code for NetBSD/i386, for GDB.
   Copyright 1988, 1989, 1991, 1992, 1994, 1996, 2000, 2001, 2002
   Free Software Foundation, Inc.

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
#include "gdbtypes.h"
#include "gdbcore.h"
#include "regcache.h"

#include "i387-tdep.h"

/* Map a GDB register number to an offset in the reg structure.  */
static int regmap[] =
{
  ( 0 * 4),		/* %eax */
  ( 1 * 4),		/* %ecx */
  ( 2 * 4),		/* %edx */
  ( 3 * 4),		/* %ebx */
  ( 4 * 4),		/* %esp */
  ( 5 * 4),		/* %epb */
  ( 6 * 4),		/* %esi */
  ( 7 * 4),		/* %edi */
  ( 8 * 4),		/* %eip */
  ( 9 * 4),		/* %eflags */
  (10 * 4),		/* %cs */
  (11 * 4),		/* %ss */
  (12 * 4),		/* %ds */
  (13 * 4),		/* %es */
  (14 * 4),		/* %fs */
  (15 * 4),		/* %gs */
};

#define SIZEOF_STRUCT_REG	(16 * 4)

static void
i386nbsd_supply_reg (char *regs, int regno)
{
  int i;

  for (i = 0; i <= 15; i++)
    if (regno == i || regno == -1)
      supply_register (i, regs + regmap[i]);
}

static void
fetch_core_registers (char *core_reg_sect, unsigned core_reg_size, int which,
                      CORE_ADDR ignore)
{
  char *regs, *fsave;

  /* We get everything from one section.  */
  if (which != 0)
    return;

  if (core_reg_size < (SIZEOF_STRUCT_REG + 108))
    {
      warning ("Wrong size register set in core file.");
      return;
    }

  regs = core_reg_sect;
  fsave = core_reg_sect + SIZEOF_STRUCT_REG;

  /* Integer registers.  */
  i386nbsd_supply_reg (regs, -1);

  /* Floating point registers.  */
  i387_supply_fsave (fsave);
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
	i386nbsd_supply_reg (core_reg_sect, -1);
      break;

    case 2:  /* Floating point registers.  */
      if (core_reg_size != 108)
	warning ("Wrong size FP register set in core file."); 
      else
	i387_supply_fsave (core_reg_sect);  
      break;

    case 3:  /* "Extended" floating point registers.  This is gdb-speak
		for SSE/SSE2. */
      if (core_reg_size != 512)
	warning ("Wrong size XMM register set in core file.");
      else
	i387_supply_fxsave (core_reg_sect);
      break;

    default:
      /* Don't know what kind of register request this is; just ignore it.  */
      break;
    }
}

static struct core_fns i386nbsd_core_fns =
{
  bfd_target_unknown_flavour,		/* core_flavour */
  default_check_format,			/* check_format */
  default_core_sniffer,			/* core_sniffer */
  fetch_core_registers,			/* core_read_registers */
  NULL					/* next */
};

static struct core_fns i386nbsd_elfcore_fns =
{
  bfd_target_elf_flavour,		/* core_flavour */
  default_check_format,			/* check_format */
  default_core_sniffer,			/* core_sniffer */
  fetch_elfcore_registers,		/* core_read_registers */
  NULL					/* next */
};

/* FIXME: should be multi-arch'd */
int
i386nbsd_aout_use_struct_convention (int gcc_p, struct type *type)
{
  return !(TYPE_LENGTH (type) == 1
	   || TYPE_LENGTH (type) == 2
	   || TYPE_LENGTH (type) == 4
	   || TYPE_LENGTH (type) == 8);
}

void
_initialize_i386nbsd_tdep (void)
{
  add_core_fns (&i386nbsd_core_fns);
  add_core_fns (&i386nbsd_elfcore_fns);
}
