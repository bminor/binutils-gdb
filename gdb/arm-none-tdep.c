/* none on ARM target support.

   Copyright (C) 2020 Free Software Foundation, Inc.

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
#include "target.h"
#include "value.h"
#include "gdbtypes.h"
#include "gdbcore.h"
#include "frame.h"
#include "regcache.h"
#include "solib-svr4.h"
#include "osabi.h"
#include "regset.h"
#include "trad-frame.h"
#include "tramp-frame.h"
#include "breakpoint.h"
#include "auxv.h"

#include "aarch32-tdep.h"
#include "arch/arm.h"
#include "arm-tdep.h"
#include "arm-none-tdep.h"
#include "glibc-tdep.h"
#include "arch-utils.h"
#include "inferior.h"
#include "gdbthread.h"
#include "symfile.h"

#include "cli/cli-utils.h"
#include "stap-probe.h"
#include "parser-defs.h"
#include "user-regs.h"
#include <ctype.h>

#include "elf-bfd.h"
#include "coff/internal.h"
#include "elf/arm.h"
#include "elf/common.h"

/* The ARM none register setup is based on the ARM GNU/Linux design.  */

static void
arm_none_supply_gregset (const struct regset *regset,
			 struct regcache *regcache,
			 int regnum, const void *gregs_buf, size_t len)
{
  struct gdbarch *gdbarch = regcache->arch ();
  enum bfd_endian byte_order = gdbarch_byte_order (gdbarch);
  const gdb_byte *gregs = (const gdb_byte *) gregs_buf;
  int regno;
  CORE_ADDR reg_pc;
  gdb_byte pc_buf[ARM_INT_REGISTER_SIZE];

  for (regno = ARM_A1_REGNUM; regno < ARM_PC_REGNUM; regno++)
    if (regnum == -1 || regnum == regno)
      regcache->raw_supply (regno, gregs + ARM_INT_REGISTER_SIZE * regno);

  if (regnum == ARM_PS_REGNUM || regnum == -1)
    {
      if (arm_apcs_32)
	regcache->raw_supply (ARM_PS_REGNUM,
			      gregs + ARM_INT_REGISTER_SIZE * ARM_NONE_CPSR_GREGNUM);
      else
	regcache->raw_supply (ARM_PS_REGNUM,
			     gregs + ARM_INT_REGISTER_SIZE * ARM_PC_REGNUM);
    }

  if (regnum == ARM_PC_REGNUM || regnum == -1)
    {
      reg_pc = extract_unsigned_integer (
		 gregs + ARM_INT_REGISTER_SIZE * ARM_PC_REGNUM,
		 ARM_INT_REGISTER_SIZE, byte_order);
      reg_pc = gdbarch_addr_bits_remove (gdbarch, reg_pc);
      store_unsigned_integer (pc_buf, ARM_INT_REGISTER_SIZE, byte_order,
			      reg_pc);
      regcache->raw_supply (ARM_PC_REGNUM, pc_buf);
    }
}

static void
arm_none_collect_gregset (const struct regset *regset,
			  const struct regcache *regcache,
			  int regnum, void *gregs_buf, size_t len)
{
  gdb_byte *gregs = (gdb_byte *) gregs_buf;
  int regno;

  for (regno = ARM_A1_REGNUM; regno < ARM_PC_REGNUM; regno++)
    if (regnum == -1 || regnum == regno)
      regcache->raw_collect (regno,
			    gregs + ARM_INT_REGISTER_SIZE * regno);

  if (regnum == ARM_PS_REGNUM || regnum == -1)
    {
      if (arm_apcs_32)
	regcache->raw_collect (ARM_PS_REGNUM,
			      gregs + ARM_INT_REGISTER_SIZE * ARM_NONE_CPSR_GREGNUM);
      else
	regcache->raw_collect (ARM_PS_REGNUM,
			      gregs + ARM_INT_REGISTER_SIZE * ARM_PC_REGNUM);
    }

  if (regnum == ARM_PC_REGNUM || regnum == -1)
    regcache->raw_collect (ARM_PC_REGNUM,
			   gregs + ARM_INT_REGISTER_SIZE * ARM_PC_REGNUM);
}

/* Support VFP register format.  */

static void
arm_none_supply_vfp (const struct regset *regset,
		     struct regcache *regcache,
		     int regnum, const void *regs_buf, size_t len)
{
  const gdb_byte *regs = (const gdb_byte *) regs_buf;
  int regno;

  if (regnum == ARM_FPSCR_REGNUM || regnum == -1)
    regcache->raw_supply (ARM_FPSCR_REGNUM, regs + 32 * 8);

  for (regno = ARM_D0_REGNUM; regno <= ARM_D31_REGNUM; regno++)
    if (regnum == -1 || regnum == regno)
      regcache->raw_supply (regno, regs + (regno - ARM_D0_REGNUM) * 8);
}

static void
arm_none_collect_vfp (const struct regset *regset,
		      const struct regcache *regcache,
		      int regnum, void *regs_buf, size_t len)
{
  gdb_byte *regs = (gdb_byte *) regs_buf;
  int regno;

  if (regnum == ARM_FPSCR_REGNUM || regnum == -1)
    regcache->raw_collect (ARM_FPSCR_REGNUM, regs + 32 * 8);

  for (regno = ARM_D0_REGNUM; regno <= ARM_D31_REGNUM; regno++)
    if (regnum == -1 || regnum == regno)
      regcache->raw_collect (regno, regs + (regno - ARM_D0_REGNUM) * 8);
}

static const struct regset arm_none_gregset =
  {
    nullptr, arm_none_supply_gregset, arm_none_collect_gregset
  };

static const struct regset arm_none_vfpregset =
  {
    nullptr, arm_none_supply_vfp, arm_none_collect_vfp
  };

/* Iterate over core file register note sections.  */

void
arm_none_iterate_over_regset_sections (struct gdbarch *gdbarch,
				       iterate_over_regset_sections_cb *cb,
				       void *cb_data,
				       const struct regcache *regcache)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

  cb (".reg", ARM_NONE_SIZEOF_GREGSET, ARM_NONE_SIZEOF_GREGSET,
	&arm_none_gregset, nullptr, cb_data);

  if (tdep->vfp_register_count > 0)
    cb (".reg-arm-vfp", ARM_NONE_SIZEOF_VFP, ARM_NONE_SIZEOF_VFP,
	&arm_none_vfpregset, "VFP floating-point", cb_data);
}
