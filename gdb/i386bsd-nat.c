/* Native-dependent code for modern i386 BSD's.
   Copyright (C) 2000 Free Software Foundation, Inc.

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
#include "inferior.h"

#include <sys/types.h>
#include <sys/ptrace.h>
#include <machine/reg.h>
#include <machine/frame.h>

#ifdef HAVE_SYS_PROCFS_H
#include <sys/procfs.h>
#endif

#ifndef HAVE_GREGSET_T
typedef struct reg gregset_t;
#endif

#ifndef HAVE_FPREGSET_T
typedef struct fpreg fpregset_t;
#endif

#include "gregset.h"


/* In older BSD versions we cannot get at some of the segment
   registers.  FreeBSD for example didn't support the %fs and %gs
   registers until the 3.0 release.  We have autoconf checks for their
   presence, and deal gracefully with their absence.  */

/* Registers we shouldn't try to fetch.  */
#if !defined (CANNOT_FETCH_REGISTER)
#define CANNOT_FETCH_REGISTER(regno) cannot_fetch_register (regno)
#endif

/* Registers we shouldn't try to store.  */
#if !defined (CANNOT_STORE_REGISTER)
#define CANNOT_STORE_REGISTER(regno) cannot_fetch_register (regno)
#endif

/* Offset to the gregset_t location where REG is stored.  */
#define REG_OFFSET(reg) offsetof (gregset_t, reg)

/* At reg_offset[REGNO] you'll find the offset to the gregset_t
   location where the GDB register REGNO is stored.  Unsupported
   registers are marked with `-1'.  */
static int reg_offset[] =
{
  REG_OFFSET (r_eax),
  REG_OFFSET (r_ecx),
  REG_OFFSET (r_edx),
  REG_OFFSET (r_edx),
  REG_OFFSET (r_esp),
  REG_OFFSET (r_ebp),
  REG_OFFSET (r_esi),
  REG_OFFSET (r_edi),
  REG_OFFSET (r_eip),
  REG_OFFSET (r_eflags),
  REG_OFFSET (r_cs),
  REG_OFFSET (r_ss),
  REG_OFFSET (r_ds),
  REG_OFFSET (r_es),
#ifdef HAVE_R_FS
  REG_OFFSET (r_fs),
#else
  -1,
#endif
#ifdef HAVE_R_GS
  REG_OFFSET (r_gs)
#else
  -1
#endif
};

#define REG_ADDR(regset, regno) ((char *) (regset) + reg_offset[regno])

/* Return nonzero if we shouldn't try to fetch register REGNO.  */

static int
cannot_fetch_register (int regno)
{
  return (reg_offset[regno] == -1);
}


/* Transfering the registers between GDB, inferiors and core files.  */

/* Fill GDB's register array with the genereal-purpose register values
   in *GREGSETP.  */

void
supply_gregset (gregset_t *gregsetp)
{
  int i;

  for (i = 0; i < NUM_GREGS; i++)
    {
      if (CANNOT_FETCH_REGISTER (i))
	supply_register (i, NULL);
      else
	supply_register (i, REG_ADDR (gregsetp, i));
    }
}

/* Fill register REGNO (if it is a general-purpose register) in
   *GREGSETPS with the value in GDB's register array.  If REGNO is -1,
   do this for all registers.  */

void
fill_gregset (gregset_t *gregsetp, int regno)
{
  int i;

  for (i = 0; i < NUM_GREGS; i++)
    if ((regno == -1 || regno == i) && ! CANNOT_STORE_REGISTER (i))
      memcpy (REG_ADDR (gregsetp, i), &registers[REGISTER_BYTE (i)],
	      REGISTER_RAW_SIZE (i));
}

#include "i387-nat.h"

/* Fill GDB's register array with the floating-point register values
   in *FPREGSETP.  */

void
supply_fpregset (fpregset_t *fpregsetp)
{
  i387_supply_fsave ((char *) fpregsetp);
}

/* Fill register REGNO (if it is a floating-point register) in
   *FPREGSETP with the value in GDB's register array.  If REGNO is -1,
   do this for all registers.  */

void
fill_fpregset (fpregset_t *fpregsetp, int regno)
{
  i387_fill_fsave ((char *) fpregsetp, regno);
}

/* Fetch register REGNO from the inferior.  If REGNO is -1, do this
   for all registers (including the floating point registers).  */

void
fetch_inferior_registers (int regno)
{
  gregset_t gregs;

  if (ptrace (PT_GETREGS, inferior_pid, (PTRACE_ARG3_TYPE) &gregs, 0) == -1)
    perror_with_name ("Couldn't get registers");

  supply_gregset (&gregs);

  if (regno == -1 || regno >= FP0_REGNUM)
    {
      fpregset_t fpregs;

      if (ptrace (PT_GETFPREGS, inferior_pid,
		  (PTRACE_ARG3_TYPE) &fpregs, 0) == -1)
	perror_with_name ("Couldn't get floating point status");

      supply_fpregset (&fpregs);
    }
}

/* Store register REGNO back into the inferior.  If REGNO is -1, do
   this for all registers (including the floating point registers).  */

void
store_inferior_registers (int regno)
{
  gregset_t gregs;

  if (ptrace (PT_GETREGS, inferior_pid, (PTRACE_ARG3_TYPE) &gregs, 0) == -1)
    perror_with_name ("Couldn't get registers");

  fill_gregset (&gregs, regno);

  if (ptrace (PT_SETREGS, inferior_pid, (PTRACE_ARG3_TYPE) &gregs, 0) == -1)
    perror_with_name ("Couldn't write registers");

  if (regno == -1 || regno >= FP0_REGNUM)
    {
      fpregset_t fpregs;

      if (ptrace (PT_GETFPREGS, inferior_pid,
		  (PTRACE_ARG3_TYPE) &fpregs, 0) == -1)
	perror_with_name ("Couldn't get floating point status");

      fill_fpregset (&fpregs, regno);
  
      if (ptrace (PT_SETFPREGS, inferior_pid,
		  (PTRACE_ARG3_TYPE) &fpregs, 0) == -1)
	perror_with_name ("Couldn't write floating point status");
    }
}


/* Support for the user struct.  */

/* Return the address register REGNO.  BLOCKEND is the value of
   u.u_ar0, which should point to the registers.  */

CORE_ADDR
register_u_addr (CORE_ADDR blockend, int regno)
{
  return (CORE_ADDR) REG_ADDR (blockend, regno);
}

#include <sys/param.h>
#include <sys/user.h>

/* Return the size of the user struct.  */

int
kernel_u_size (void)
{
  return (sizeof (struct user));
}
