/* Definitions to make GDB run on a Sequent Symmetry under dynix 3.0,
   with Weitek 1167 and i387 support.
   Copyright 1986, 1987, 1989, 1992  Free Software Foundation, Inc.

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
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#ifdef _SEQUENT_
/* ptx */
#define PTRACE_READ_REGS(pid,regaddr) mptrace (XPT_RREGS, (pid), (regaddr), 0)
#define PTRACE_WRITE_REGS(pid,regaddr) \
  mptrace (XPT_WREGS, (pid), (regadddr), 0)
#else
/* dynix */
#define PTRACE_READ_REGS(pid,regaddr) ptrace (XPT_RREGS, (pid), (regaddr), 0)
#define PTRACE_WRITE_REGS(pid,regaddr) \
  ptrace (XPT_WREGS, (pid), (regadddr), 0)
#endif

/* Override copies of {fetch,store}_inferior_registers in infptrace.c.  */

#define FETCH_INFERIOR_REGISTERS

/* This is the amount to subtract from u.u_ar0
   to get the offset in the core file of the register values.  */

#ifdef _SEQUENT_
#define KERNEL_U_ADDR (0xffffe000) /* VA_UBLOCK */	/* ptx */
#else
#define KERNEL_U_ADDR (0x80000000 - (UPAGES * NBPG))	/* dynix */
#endif
