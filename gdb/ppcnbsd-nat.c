/* Native-dependent code for PowerPC's running NetBSD, for GDB.
   Copyright 1988, 1989, 1991, 1992, 1994, 1996, 2000 Free Software Foundation, Inc.

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

#include <sys/types.h>
#include <sys/ptrace.h>
#include <machine/reg.h>
#include <machine/frame.h>

#include "defs.h"
#include "inferior.h"
#include "gdbcore.h"

#define RF(dst, src) \
        memcpy(&registers[REGISTER_BYTE(dst)], &src, sizeof(src))
   
#define RS(src, dst) \
        memcpy(&dst, &registers[REGISTER_BYTE(src)], sizeof(dst))

void
fetch_inferior_registers (regno)
     int regno;
{
  struct reg inferior_registers;
  struct fpreg inferior_fp_registers;
  int i;

  ptrace (PT_GETREGS, inferior_pid,
	  (PTRACE_ARG3_TYPE) & inferior_registers, 0);
  for (i = 0; i < 32; i++)
    RF (i, inferior_registers.fixreg[i]);
  RF (LR_REGNUM, inferior_registers.lr);
  RF (CR_REGNUM, inferior_registers.cr);
  RF (XER_REGNUM, inferior_registers.xer);
  RF (CTR_REGNUM, inferior_registers.ctr);
  RF (PC_REGNUM, inferior_registers.pc);

  ptrace (PT_GETFPREGS, inferior_pid,
	  (PTRACE_ARG3_TYPE) & inferior_fp_registers, 0);
  for (i = 0; i < 32; i++)
    RF (FP0_REGNUM + i, inferior_fp_registers.r_regs[i]);

  registers_fetched ();
}

void
store_inferior_registers (regno)
     int regno;
{
  struct reg inferior_registers;
  struct fpreg inferior_fp_registers;
  int i;

  for (i = 0; i < 32; i++)
    RS (i, inferior_registers.fixreg[i]);
  RS (LR_REGNUM, inferior_registers.lr);
  RS (CR_REGNUM, inferior_registers.cr);
  RS (XER_REGNUM, inferior_registers.xer);
  RS (CTR_REGNUM, inferior_registers.ctr);
  RS (PC_REGNUM, inferior_registers.pc);

  ptrace (PT_SETREGS, inferior_pid,
	  (PTRACE_ARG3_TYPE) & inferior_registers, 0);

  for (i = 0; i < 32; i++)
    RS (FP0_REGNUM + i, inferior_fp_registers.r_regs[i]);
  ptrace (PT_SETFPREGS, inferior_pid,
	  (PTRACE_ARG3_TYPE) & inferior_fp_registers, 0);
}

struct md_core
{
  struct reg intreg;
  struct fpreg freg;
};

void
fetch_core_registers (core_reg_sect, core_reg_size, which, ignore)
     char *core_reg_sect;
     unsigned core_reg_size;
     int which;
     CORE_ADDR ignore;
{
  struct md_core *core_reg = (struct md_core *) core_reg_sect;
  int i;

  /* Integer registers */
  for (i = 0; i < 32; i++)
    RF (i, core_reg->intreg.fixreg[i]);
  RF (LR_REGNUM, core_reg->intreg.lr);
  RF (CR_REGNUM, core_reg->intreg.cr);
  RF (XER_REGNUM, core_reg->intreg.xer);
  RF (CTR_REGNUM, core_reg->intreg.ctr);
  RF (PC_REGNUM, core_reg->intreg.pc);

  /* Floating point registers */
  for (i = 0; i < 32; i++)
    RF (FP0_REGNUM + i, core_reg->freg.r_regs[i]);

  registers_fetched ();
}

/* Register that we are able to handle ppcnbsd core file formats.
   FIXME: is this really bfd_target_unknown_flavour? */

static struct core_fns ppcnbsd_core_fns =
{
  bfd_target_unknown_flavour,		/* core_flavour */
  default_check_format,			/* check_format */
  default_core_sniffer,			/* core_sniffer */
  fetch_core_registers,			/* core_read_registers */
  NULL					/* next */
};

void
_initialize_ppcnbsd_nat ()
{
  add_core_fns (&ppcnbsd_core_fns);
}
