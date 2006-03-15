/* Native-dependent code for GNU/Linux on MIPS processors.

   Copyright (C) 2001, 2002, 2003, 2004, 2005, 2006
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
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

#include "defs.h"
#include "mips-tdep.h"
#include "target.h"
#include "linux-nat.h"

#include "gdb_proc_service.h"

#ifndef PTRACE_GET_THREAD_AREA
#define PTRACE_GET_THREAD_AREA 25
#endif

/* Pseudo registers can not be read.  ptrace does not provide a way to
   read (or set) MIPS_PS_REGNUM, and there's no point in reading or
   setting MIPS_ZERO_REGNUM.  We also can not set BADVADDR, CAUSE, or
   FCRIR via ptrace().  */

int
mips_linux_cannot_fetch_register (int regno)
{
  if (regno > MIPS_ZERO_REGNUM && regno < MIPS_ZERO_REGNUM + 32)
    return 0;
  else if (regno >= mips_regnum (current_gdbarch)->fp0
	   && regno <= mips_regnum (current_gdbarch)->fp0 + 32)
    return 0;
  else if (regno == mips_regnum (current_gdbarch)->lo
	   || regno == mips_regnum (current_gdbarch)->hi
	   || regno == mips_regnum (current_gdbarch)->badvaddr
	   || regno == mips_regnum (current_gdbarch)->cause
	   || regno == mips_regnum (current_gdbarch)->pc
	   || regno == mips_regnum (current_gdbarch)->fp_control_status
	   || regno == mips_regnum (current_gdbarch)->fp_implementation_revision)
    return 0;
  else
    return 1;
}

int
mips_linux_cannot_store_register (int regno)
{
  if (regno > MIPS_ZERO_REGNUM && regno < MIPS_ZERO_REGNUM + 32)
    return 0;
  else if (regno >= FP0_REGNUM && regno <= FP0_REGNUM + 32)
    return 0;
  else if (regno == mips_regnum (current_gdbarch)->lo
	   || regno == mips_regnum (current_gdbarch)->hi
	   || regno == mips_regnum (current_gdbarch)->pc
	   || regno == mips_regnum (current_gdbarch)->fp_control_status)
    return 0;
  else
    return 1;
}

/* Fetch the thread-local storage pointer for libthread_db.  */

ps_err_e
ps_get_thread_area (const struct ps_prochandle *ph,
                    lwpid_t lwpid, int idx, void **base)
{
  if (ptrace (PTRACE_GET_THREAD_AREA, lwpid, NULL, base) != 0)
    return PS_ERR;

  /* IDX is the bias from the thread pointer to the beginning of the
     thread descriptor.  It has to be subtracted due to implementation
     quirks in libthread_db.  */
  *base = (void *) ((char *)*base - idx);

  return PS_OK;
}

void _initialize_mips_linux_nat (void);

void
_initialize_mips_linux_nat (void)
{
  add_target (linux_target ());
}
