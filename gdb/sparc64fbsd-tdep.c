/* Target-dependent code for FreeBSD/sparc64.

   Copyright 2003 Free Software Foundation, Inc.

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
#include "target.h"

#include "gdb_string.h"

#include "sparc64-tdep.h"

/* From <machine/reg.h>.  */

/* Offset of registers in `struct reg'.  */
int sparc64fbsd_r_global_offset = (0 * 8);
int sparc64fbsd_r_out_offset = (8 * 8);
int sparc64fbsd_r_fprs_offset = (16 * 8);
int sparc64fbsd_r_tnpc_offset = (24 * 8);
int sparc64fbsd_r_tpc_offset = (25 * 8);
int sparc64fbsd_r_tstate_offset = (26 * 8);
int sparc64fbsd_r_y_offset = (28 * 8);

/* Size of `struct reg' and `struct fpreg'.  */
int sparc64fbsd_sizeof_struct_reg = 256;
int sparc64fbsd_sizeof_struct_fpreg = 272;

void
sparc64fbsd_supply_reg (const char *regs, int regnum)
{
  char buf[8];
  int i;

  if (regnum == SPARC64_PC_REGNUM || regnum == -1)
    supply_register (SPARC64_PC_REGNUM, regs + sparc64fbsd_r_tpc_offset);

  if (regnum == SPARC64_NPC_REGNUM || regnum == -1)
    supply_register (SPARC64_NPC_REGNUM, regs + sparc64fbsd_r_tnpc_offset);

  if (regnum == SPARC64_STATE_REGNUM || regnum == -1)
    supply_register (SPARC64_STATE_REGNUM, regs + sparc64fbsd_r_tstate_offset);

  if (regnum == SPARC64_FPRS_REGNUM || regnum == -1)
    supply_register (SPARC64_FPRS_REGNUM, regs + sparc64fbsd_r_fprs_offset);

  if (regnum == SPARC64_Y_REGNUM || regnum == -1)
    supply_register (SPARC64_Y_REGNUM, regs + sparc64fbsd_r_y_offset);

  if ((regnum >= SPARC_G0_REGNUM && regnum <= SPARC_G7_REGNUM) || regnum == -1)
    {
      if (regnum == SPARC_G0_REGNUM || regnum == -1)
	supply_register (SPARC_G0_REGNUM, NULL); /* %g0 is always zero.  */
      for (i = SPARC_G1_REGNUM; i <= SPARC_G7_REGNUM; i++)
	{
	  if (regnum == i || regnum == -1)
	    supply_register (i, (regs + sparc64fbsd_r_global_offset
				 + ((i - SPARC_G0_REGNUM) * 8)));
	}
    }

  if ((regnum >= SPARC_O0_REGNUM && regnum <= SPARC_O7_REGNUM) || regnum == -1)
    {
      for (i = SPARC_O0_REGNUM; i <= SPARC_O7_REGNUM; i++)
	{
	  if (regnum == i || regnum == -1)
	    supply_register (i, (regs + sparc64fbsd_r_out_offset
				 + ((i - SPARC_O0_REGNUM) * 8)));
        }
    }

  /* Inputs and Locals are stored onto the stack by by the kernel.  */
  if ((regnum >= SPARC_L0_REGNUM && regnum <= SPARC_I7_REGNUM) || regnum == -1)
    {
      ULONGEST sp;

      regcache_cooked_read_unsigned (current_regcache, SPARC_SP_REGNUM, &sp);
      sparc_supply_rwindow (sp, regnum);
    }
}

void
sparc64fbsd_fill_reg (char *regs, int regnum)
{
  char buf[8];
  int i;

  if (regnum == SPARC64_PC_REGNUM || regnum == -1)
    regcache_collect (SPARC64_PC_REGNUM, regs + sparc64fbsd_r_tpc_offset);

  if (regnum == SPARC64_NPC_REGNUM || regnum == -1)
    regcache_collect (SPARC64_NPC_REGNUM, regs + sparc64fbsd_r_tnpc_offset);

  if (regnum == SPARC64_FPRS_REGNUM || regnum == -1)
    regcache_collect (SPARC64_FPRS_REGNUM, regs + sparc64fbsd_r_fprs_offset);

  if (regnum == SPARC64_Y_REGNUM || regnum == -1)
    regcache_collect (SPARC64_Y_REGNUM, regs + sparc64fbsd_r_y_offset);

  if ((regnum >= SPARC_G0_REGNUM && regnum <= SPARC_G7_REGNUM) || regnum == -1)
    {
      /* %g0 is always zero.  */
      for (i = SPARC_G1_REGNUM; i <= SPARC_G7_REGNUM; i++)
	{
	  if (regnum == i || regnum == -1)
	    regcache_collect (i, (regs + sparc64fbsd_r_global_offset
				  + ((i - SPARC_G0_REGNUM) * 8)));
	}
    }

  if ((regnum >= SPARC_O0_REGNUM && regnum <= SPARC_O7_REGNUM) || regnum == -1)
    {
      for (i = SPARC_O0_REGNUM; i <= SPARC_O7_REGNUM; i++)
	{
	  if (regnum == i || regnum == -1)
	    regcache_collect (i, (regs + sparc64fbsd_r_out_offset
				  + ((i - SPARC_O0_REGNUM) * 8)));
        }
    }

  /* Responsibility for the stack regs is pushed off onto the caller.  */
}

void
sparc64fbsd_supply_fpreg (const char *fpregs, int regnum)
{
  int i;

  for (i = 0; i < 32; i++)
    {
      if (regnum == (SPARC_F0_REGNUM + i) || regnum == -1)
	supply_register (SPARC_F0_REGNUM + i, fpregs + (i * 4));
    }

  for (i = 0; i < 16; i++)
    {
      if (regnum == (SPARC64_F32_REGNUM + i) || regnum == -1)
	supply_register (SPARC64_F32_REGNUM + i, fpregs + (32 * 4) + (i * 8));
    }

  if (regnum == SPARC64_FSR_REGNUM || regnum == -1)
    supply_register (SPARC64_FSR_REGNUM, fpregs + (32 * 4) + (16 * 8));
}

void
sparc64fbsd_fill_fpreg (char *fpregs, int regnum)
{
  int i;

  for (i = 0; i < 32; i++)
    {
      if (regnum == (SPARC_F0_REGNUM + i) || regnum == -1)
	regcache_collect (SPARC_F0_REGNUM + i, fpregs + (i * 4));
    }

  for (i = 0; i < 16; i++)
    {
      if (regnum == (SPARC64_F32_REGNUM + i) || regnum == -1)
	regcache_collect (SPARC64_F32_REGNUM + i, fpregs + (32 * 4) + (i * 8));
    }

  if (regnum == SPARC64_FSR_REGNUM || regnum == -1)
    regcache_collect (SPARC64_FSR_REGNUM, fpregs + (32 * 4) + (16 * 8));
}


static void
fetch_core_registers (char *core_reg_sect, unsigned core_reg_size, int which,
                      CORE_ADDR ignore)
{
  switch (which)
    {
    case 0:  /* Integer registers */
      if (core_reg_size != sparc64fbsd_sizeof_struct_reg)
	warning ("Wrong size register set in core file.");
      else
	sparc64fbsd_supply_reg (core_reg_sect, -1);
      break;

    case 2:  /* Floating pointer registers */
      if (core_reg_size != sparc64fbsd_sizeof_struct_fpreg)
	warning ("Wrong size FP register set in core file.");
      else
	sparc64fbsd_supply_fpreg (core_reg_sect, -1);
      break;

    default:
      /* Don't know what kind of register request this is; just ignore it.  */
      break;
    }
}

static struct core_fns sparc64fbsd_core_fns =
{
  bfd_target_elf_flavour,		/* core_flavour */
  default_check_format,			/* check_format */
  default_core_sniffer,			/* core_sniffer */
  fetch_core_registers,			/* core_read_registers */
  NULL
};


static void
sparc64fbsd_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  /* Nothing yet.  */
}

/* Provide a prototype to silence -Wmissing-prototypes.  */
void _initialize_sparc64fbsd_tdep (void);

void
_initialize_sparc64fbsd_tdep (void)
{
  gdbarch_register_osabi (bfd_arch_sparc, bfd_mach_sparc_v9,
			  GDB_OSABI_FREEBSD_ELF, sparc64fbsd_init_abi);

  add_core_fns (&sparc64fbsd_core_fns);
}
