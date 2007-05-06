/* Native-dependent code for GNU/Linux on MIPS processors.

   Copyright (C) 2001, 2002, 2003, 2004, 2005, 2006, 2007
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
#include "inferior.h"
#include "mips-tdep.h"
#include "target.h"
#include "regcache.h"
#include "linux-nat.h"
#include "mips-linux-tdep.h"

#include "gdb_proc_service.h"
#include "gregset.h"

#include <sys/ptrace.h>

#ifndef PTRACE_GET_THREAD_AREA
#define PTRACE_GET_THREAD_AREA 25
#endif

/* Assume that we have PTRACE_GETREGS et al. support.  If we do not,
   we'll clear this and use PTRACE_PEEKUSER instead.  */
static int have_ptrace_regsets = 1;

/* Saved function pointers to fetch and store a single register using
   PTRACE_PEEKUSER and PTRACE_POKEUSER.  */

void (*super_fetch_registers) (int);
void (*super_store_registers) (int);

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

/* Map gdb internal register number to ptrace ``address''.
   These ``addresses'' are normally defined in <asm/ptrace.h>.  */

static CORE_ADDR
mips_linux_register_addr (int regno)
{
  int regaddr;

  if (regno < 0 || regno >= NUM_REGS)
    error (_("Bogon register number %d."), regno);

  if (regno < 32)
    regaddr = regno;
  else if ((regno >= mips_regnum (current_gdbarch)->fp0)
	   && (regno < mips_regnum (current_gdbarch)->fp0 + 32))
    regaddr = FPR_BASE + (regno - mips_regnum (current_gdbarch)->fp0);
  else if (regno == mips_regnum (current_gdbarch)->pc)
    regaddr = PC;
  else if (regno == mips_regnum (current_gdbarch)->cause)
    regaddr = CAUSE;
  else if (regno == mips_regnum (current_gdbarch)->badvaddr)
    regaddr = BADVADDR;
  else if (regno == mips_regnum (current_gdbarch)->lo)
    regaddr = MMLO;
  else if (regno == mips_regnum (current_gdbarch)->hi)
    regaddr = MMHI;
  else if (regno == mips_regnum (current_gdbarch)->fp_control_status)
    regaddr = FPC_CSR;
  else if (regno == mips_regnum (current_gdbarch)->fp_implementation_revision)
    regaddr = FPC_EIR;
  else
    error (_("Unknowable register number %d."), regno);

  return regaddr;
}

static CORE_ADDR
mips64_linux_register_addr (int regno)
{
  int regaddr;

  if (regno < 0 || regno >= NUM_REGS)
    error (_("Bogon register number %d."), regno);

  if (regno < 32)
    regaddr = regno;
  else if ((regno >= mips_regnum (current_gdbarch)->fp0)
	   && (regno < mips_regnum (current_gdbarch)->fp0 + 32))
    regaddr = MIPS64_FPR_BASE + (regno - FP0_REGNUM);
  else if (regno == mips_regnum (current_gdbarch)->pc)
    regaddr = MIPS64_PC;
  else if (regno == mips_regnum (current_gdbarch)->cause)
    regaddr = MIPS64_CAUSE;
  else if (regno == mips_regnum (current_gdbarch)->badvaddr)
    regaddr = MIPS64_BADVADDR;
  else if (regno == mips_regnum (current_gdbarch)->lo)
    regaddr = MIPS64_MMLO;
  else if (regno == mips_regnum (current_gdbarch)->hi)
    regaddr = MIPS64_MMHI;
  else if (regno == mips_regnum (current_gdbarch)->fp_control_status)
    regaddr = MIPS64_FPC_CSR;
  else if (regno == mips_regnum (current_gdbarch)->fp_implementation_revision)
    regaddr = MIPS64_FPC_EIR;
  else
    error (_("Unknowable register number %d."), regno);

  return regaddr;
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

/* Wrapper functions.  These are only used by libthread_db.  */

void
supply_gregset (struct regcache *regcache, const gdb_gregset_t *gregsetp)
{
  if (mips_isa_regsize (current_gdbarch) == 4)
    mips_supply_gregset (regcache, (const mips_elf_gregset_t *) gregsetp);
  else
    mips64_supply_gregset (regcache, (const mips64_elf_gregset_t *) gregsetp);
}

void
fill_gregset (const struct regcache *regcache,
	      gdb_gregset_t *gregsetp, int regno)
{
  if (mips_isa_regsize (current_gdbarch) == 4)
    mips_fill_gregset (regcache, (mips_elf_gregset_t *) gregsetp, regno);
  else
    mips64_fill_gregset (regcache, (mips64_elf_gregset_t *) gregsetp, regno);
}

void
supply_fpregset (struct regcache *regcache, const gdb_fpregset_t *fpregsetp)
{
  if (mips_isa_regsize (current_gdbarch) == 4)
    mips_supply_fpregset (regcache, (const mips_elf_fpregset_t *) fpregsetp);
  else
    mips64_supply_fpregset (regcache, (const mips64_elf_fpregset_t *) fpregsetp);
}

void
fill_fpregset (const struct regcache *regcache,
	       gdb_fpregset_t *fpregsetp, int regno)
{
  if (mips_isa_regsize (current_gdbarch) == 4)
    mips_fill_fpregset (regcache, (mips_elf_fpregset_t *) fpregsetp, regno);
  else
    mips64_fill_fpregset (regcache, (mips64_elf_fpregset_t *) fpregsetp, regno);
}


/* Fetch REGNO (or all registers if REGNO == -1) from the target
   using PTRACE_GETREGS et al.  */

static void
mips64_linux_regsets_fetch_registers (int regno)
{
  int is_fp;
  int tid;

  if (regno >= mips_regnum (current_gdbarch)->fp0
      && regno <= mips_regnum (current_gdbarch)->fp0 + 32)
    is_fp = 1;
  else if (regno == mips_regnum (current_gdbarch)->fp_control_status)
    is_fp = 1;
  else if (regno == mips_regnum (current_gdbarch)->fp_implementation_revision)
    is_fp = 1;
  else
    is_fp = 0;

  tid = ptid_get_lwp (inferior_ptid);
  if (tid == 0)
    tid = ptid_get_pid (inferior_ptid);

  if (regno == -1 || !is_fp)
    {
      mips64_elf_gregset_t regs;

      if (ptrace (PTRACE_GETREGS, tid, 0L, (PTRACE_TYPE_ARG3) &regs) == -1)
	{
	  if (errno == EIO)
	    {
	      have_ptrace_regsets = 0;
	      return;
	    }
	  perror_with_name (_("Couldn't get registers"));
	}

      mips64_supply_gregset (current_regcache,
			     (const mips64_elf_gregset_t *) &regs);
    }

  if (regno == -1 || is_fp)
    {
      mips64_elf_fpregset_t fp_regs;

      if (ptrace (PTRACE_GETFPREGS, tid, 0L,
		  (PTRACE_TYPE_ARG3) &fp_regs) == -1)
	{
	  if (errno == EIO)
	    {
	      have_ptrace_regsets = 0;
	      return;
	    }
	  perror_with_name (_("Couldn't get FP registers"));
	}

      mips64_supply_fpregset (current_regcache,
			      (const mips64_elf_fpregset_t *) &fp_regs);
    }
}

/* Store REGNO (or all registers if REGNO == -1) to the target
   using PTRACE_SETREGS et al.  */

static void
mips64_linux_regsets_store_registers (int regno)
{
  int is_fp;
  int tid;

  if (regno >= mips_regnum (current_gdbarch)->fp0
      && regno <= mips_regnum (current_gdbarch)->fp0 + 32)
    is_fp = 1;
  else if (regno == mips_regnum (current_gdbarch)->fp_control_status)
    is_fp = 1;
  else if (regno == mips_regnum (current_gdbarch)->fp_implementation_revision)
    is_fp = 1;
  else
    is_fp = 0;

  tid = ptid_get_lwp (inferior_ptid);
  if (tid == 0)
    tid = ptid_get_pid (inferior_ptid);

  if (regno == -1 || !is_fp)
    {
      mips64_elf_gregset_t regs;

      if (ptrace (PTRACE_GETREGS, tid, 0L, (PTRACE_TYPE_ARG3) &regs) == -1)
	perror_with_name (_("Couldn't get registers"));

      mips64_fill_gregset (current_regcache, &regs, regno);

      if (ptrace (PTRACE_SETREGS, tid, 0L, (PTRACE_TYPE_ARG3) &regs) == -1)
	perror_with_name (_("Couldn't set registers"));
    }

  if (regno == -1 || is_fp)
    {
      mips64_elf_fpregset_t fp_regs;

      if (ptrace (PTRACE_GETFPREGS, tid, 0L,
		  (PTRACE_TYPE_ARG3) &fp_regs) == -1)
	perror_with_name (_("Couldn't get FP registers"));

      mips64_fill_fpregset (current_regcache, &fp_regs, regno);

      if (ptrace (PTRACE_SETFPREGS, tid, 0L,
		  (PTRACE_TYPE_ARG3) &fp_regs) == -1)
	perror_with_name (_("Couldn't set FP registers"));
    }
}

/* Fetch REGNO (or all registers if REGNO == -1) from the target
   using any working method.  */

static void
mips64_linux_fetch_registers (int regnum)
{
  /* Unless we already know that PTRACE_GETREGS does not work, try it.  */
  if (have_ptrace_regsets)
    mips64_linux_regsets_fetch_registers (regnum);

  /* If we know, or just found out, that PTRACE_GETREGS does not work, fall
     back to PTRACE_PEEKUSER.  */
  if (!have_ptrace_regsets)
    super_fetch_registers (regnum);
}

/* Store REGNO (or all registers if REGNO == -1) to the target
   using any working method.  */

static void
mips64_linux_store_registers (int regnum)
{
  /* Unless we already know that PTRACE_GETREGS does not work, try it.  */
  if (have_ptrace_regsets)
    mips64_linux_regsets_store_registers (regnum);

  /* If we know, or just found out, that PTRACE_GETREGS does not work, fall
     back to PTRACE_PEEKUSER.  */
  if (!have_ptrace_regsets)
    super_store_registers (regnum);
}

/* Return the address in the core dump or inferior of register
   REGNO.  */

static CORE_ADDR
mips_linux_register_u_offset (int regno)
{
  if (mips_abi_regsize (current_gdbarch) == 8)
    return mips64_linux_register_addr (regno);
  else
    return mips_linux_register_addr (regno);
}

void _initialize_mips_linux_nat (void);

void
_initialize_mips_linux_nat (void)
{
  struct target_ops *t = linux_trad_target (mips_linux_register_u_offset);

  super_fetch_registers = t->to_fetch_registers;
  super_store_registers = t->to_store_registers;

  t->to_fetch_registers = mips64_linux_fetch_registers;
  t->to_store_registers = mips64_linux_store_registers;

  linux_nat_add_target (t);
}
