/* Native-dependent code for FreeBSD/sparc64.

   Copyright 2003, 2004, 2005 Free Software Foundation, Inc.

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
#include "target.h"

#include "fbsd-nat.h"
#include "sparc64-tdep.h"
#include "sparc-nat.h"

/* Provide a prototype to silence -Wmissing-prototypes.  */
void _initialize_sparc64fbsd_nat (void);

void
_initialize_sparc64fbsd_nat (void)
{
  struct target_ops *t;

  /* Add some extra features to the generic SPARC target.  */
  t = sparc_target ();
  t->to_pid_to_exec_file = fbsd_pid_to_exec_file;
  t->to_find_memory_regions = fbsd_find_memory_regions;
  t->to_make_corefile_notes = fbsd_make_corefile_notes;
  add_target (t);

  sparc_gregset = &sparc64fbsd_gregset;
}
