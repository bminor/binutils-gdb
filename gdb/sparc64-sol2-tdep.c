/* Target-dependent code for Solaris UltraSPARC.

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
#include "gdbarch.h"
#include "symtab.h"
#include "objfiles.h"
#include "osabi.h"

#include "sparc64-tdep.h"

/* From <sys/regset.h>.  */
const struct sparc_gregset sparc64_sol2_gregset =
{
  32 * 8,			/* "tstate" */
  33 * 8,			/* %pc */
  34 * 8,			/* %npc */
  35 * 8,			/* %y */
  -1,				/* %wim */
  -1,				/* %tbr */
  1 * 8,			/* %g1 */
  16 * 8,			/* %l0 */
  8				/* sizeof (%y) */
};


void
sparc64_sol2_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

  sparc64_init_abi (info, gdbarch);

  /* Solaris has SVR4-style shared libraries...  */
  set_gdbarch_in_solib_call_trampoline (gdbarch, in_plt_section);
  set_gdbarch_skip_trampoline_code (gdbarch, find_solib_trampoline_target);

  /* ...which means that we need some special handling when doing
     prologue analysis.  */
  tdep->plt_entry_size = 16;

  /* Solaris has kernel-assisted single-stepping support.  */
  set_gdbarch_software_single_step (gdbarch, NULL);
}


/* Provide a prototype to silence -Wmissing-prototypes.  */
void _initialize_sparc64_sol2_tdep (void);

void
_initialize_sparc64_sol2_tdep (void)
{
  gdbarch_register_osabi (bfd_arch_sparc, bfd_mach_sparc_v9,
			  GDB_OSABI_SOLARIS, sparc64_sol2_init_abi);
}
