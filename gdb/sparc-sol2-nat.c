/* Native-dependent code for Solaris SPARC.

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
#include "gregset.h"
#include "regcache.h"

#include <sys/procfs.h>

#include "sparc-tdep.h"

/* These functions provide the (temporary) glue between the Solaris
   SPARC target dependent code and the machine independent SVR4 /proc
   support.  */

void
supply_gregset (prgregset_t *gregs)
{
  sparc_sol2_supply_gregset (current_regcache, -1, gregs);
}

void
supply_fpregset (prfpregset_t *fpregs)
{
  sparc_sol2_supply_fpregset (current_regcache, -1, fpregs);
}

void
fill_gregset (prgregset_t *gregs, int regnum)
{
  sparc_sol2_collect_gregset (current_regcache, regnum, gregs);
}

void
fill_fpregset (prfpregset_t *fpregs, int regnum)
{
  sparc_sol2_collect_fpregset (current_regcache, regnum, fpregs);
}
