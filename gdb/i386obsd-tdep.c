/* Target-dependent code for OpenBSD/i386.

   Copyright 1988, 1989, 1991, 1992, 1994, 1996, 2000, 2001, 2002, 2003
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
#include "arch-utils.h"
#include "gdbcore.h"
#include "regcache.h"
#include "regset.h"
#include "osabi.h"

#include "gdb_assert.h"
#include "gdb_string.h"

#include "i386-tdep.h"
#include "i387-tdep.h"

/* From <machine/reg.h>.  */
static int i386obsd_r_reg_offset[] =
{
  0 * 4,			/* %eax */
  1 * 4,			/* %ecx */
  2 * 4,			/* %edx */
  3 * 4,			/* %ebx */
  4 * 4,			/* %esp */
  5 * 4,			/* %ebp */
  6 * 4,			/* %esi */
  7 * 4,			/* %edi */
  8 * 4,			/* %eip */
  9 * 4,			/* %eflags */
  10 * 4,			/* %cs */
  11 * 4,			/* %ss */
  12 * 4,			/* %ds */
  13 * 4,			/* %es */
  14 * 4,			/* %fs */
  15 * 4			/* %gs */
};

static void
i386obsd_aout_supply_regset (const struct regset *regset,
			     struct regcache *regcache, int regnum,
			     const void *regs, size_t len)
{
  const struct gdbarch_tdep *tdep = regset->descr;

  gdb_assert (len >= tdep->sizeof_gregset + I387_SIZEOF_FSAVE);

  i386_supply_gregset (regset, regcache, regnum, regs, tdep->sizeof_gregset);
  i387_supply_fsave (regcache, regnum, (char *) regs + tdep->sizeof_gregset);
}

const struct regset *
i386obsd_aout_regset_from_core_section (struct gdbarch *gdbarch,
					const char *sect_name,
					size_t sect_size)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

  /* OpenBSD a.out core dumps don't use seperate register sets for the
     general-purpose and floating-point registers.  */

  if (strcmp (sect_name, ".reg") == 0
      && sect_size >= tdep->sizeof_gregset + I387_SIZEOF_FSAVE)
    {
      if (tdep->gregset == NULL)
	{
	  tdep->gregset = XMALLOC (struct regset);
	  tdep->gregset->descr = tdep;
	  tdep->gregset->supply_regset = i386obsd_aout_supply_regset;
	}
      return tdep->gregset;
    }

  return NULL;
}



CORE_ADDR i386obsd_sigtramp_start = 0xbfbfdf20;
CORE_ADDR i386obsd_sigtramp_end = 0xbfbfdff0;

/* From <machine/signal.h>.  */
int i386obsd_sc_reg_offset[I386_NUM_GREGS] =
{
  10 * 4,			/* %eax */
  9 * 4,			/* %ecx */
  8 * 4,			/* %edx */
  7 * 4,			/* %ebx */
  14 * 4,			/* %esp */
  6 * 4,			/* %ebp */
  5 * 4,			/* %esi */
  4 * 4,			/* %edi */
  11 * 4,			/* %eip */
  13 * 4,			/* %eflags */
  12 * 4,			/* %cs */
  15 * 4,			/* %ss */
  3 * 4,			/* %ds */
  2 * 4,			/* %es */
  1 * 4,			/* %fs */
  0 * 4				/* %gs */
};

static void 
i386obsd_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

  /* Obviously OpenBSD is BSD-based.  */
  i386bsd_init_abi (info, gdbarch);

  /* OpenBSD has a different `struct reg'.  */
  tdep->gregset_reg_offset = i386obsd_r_reg_offset;
  tdep->gregset_num_regs = ARRAY_SIZE (i386obsd_r_reg_offset);
  tdep->sizeof_gregset = 16 * 4;

  /* OpenBSD has a single register set.  */
  set_gdbarch_regset_from_core_section
    (gdbarch, i386obsd_aout_regset_from_core_section);

  /* OpenBSD uses -freg-struct-return by default.  */
  tdep->struct_return = reg_struct_return;

  /* OpenBSD uses a different memory layout.  */
  tdep->sigtramp_start = i386obsd_sigtramp_start;
  tdep->sigtramp_end = i386obsd_sigtramp_end;

  /* OpenBSD has a `struct sigcontext' that's different from the
     origional 4.3 BSD.  */
  tdep->sc_reg_offset = i386obsd_sc_reg_offset;
  tdep->sc_num_regs = ARRAY_SIZE (i386obsd_sc_reg_offset);
}


/* Provide a prototype to silence -Wmissing-prototypes.  */
void _initialize_i386obsd_tdep (void);

void
_initialize_i386obsd_tdep (void)
{
  /* FIXME: kettenis/20021020: Since OpenBSD/i386 binaries are
     indistingushable from NetBSD/i386 a.out binaries, building a GDB
     that should support both these targets will probably not work as
     expected.  */
#define GDB_OSABI_OPENBSD_AOUT GDB_OSABI_NETBSD_AOUT

  gdbarch_register_osabi (bfd_arch_i386, 0, GDB_OSABI_OPENBSD_AOUT,
			  i386obsd_init_abi);
}
