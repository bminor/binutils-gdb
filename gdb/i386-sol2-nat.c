/* Native-dependent code for Solaris x86.

   Copyright 2004 Free Software Foundation, Inc.

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
#include "regcache.h"

#include <sys/procfs.h>
#include "gregset.h"

/* This file provids the (temporary) glue between the Solaris x86
   target dependent code and the machine independent SVR4 /proc
   support.  */

/* Solaris 10 (Solaris 2.10, SunOS 5.10) and up support two process
   data models, the traditional 32-bit data model (ILP32) and the
   64-bit data model (LP64).  The format of /proc depends on the data
   model of the observer (the controlling process, GDB in our case).
   The Solaris header files conveniently define PR_MODEL_NATIVE to the
   data model of the controlling process.  If its value is
   PR_MODEL_LP64, we know that GDB is being compiled as a 64-bit
   program.

   Note that a 32-bit GDB won't be able to debug a 64-bit target
   process using /proc on Solaris.  */

#if defined (PR_MODEL_NATIVE) && (PR_MODEL_NATIVE == PR_MODEL_LP64)

#include "amd64-nat.h"
#include "amd64-tdep.h"

void
supply_gregset (prgregset_t *gregs)
{
  amd64_supply_native_gregset (current_regcache, -1, gregs);
}

void
supply_fpregset (prfpregset_t *fpregs)
{
  amd64_supply_fxsave (current_regcache, -1, fpregs);
}

void
fill_gregset (prgregset_t *gregs, int regnum)
{
  amd64_collect_native_gregset (current_regcache, regnum, gregs);
}

void
fill_fpregset (prfpregset_t *fpregs, int regnum)
{
  amd64_collect_fxsave (current_regcache, regnum, fpregs);
}

#else

/* 32-bit Solaris x86 is very similar to SVR4.  */
#include "i386v4-nat.c"

#endif
