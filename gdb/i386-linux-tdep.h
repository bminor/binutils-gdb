/* Target-dependent code for GNU/Linux x86.

   Copyright 2002 Free Software Foundation, Inc.

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

#ifndef I386_LINUX_TDEP_H
#define I386_LINUX_TDEP_H

/* The Linux kernel pretends there is an additional "orig_eax"
   register.  Since GDB needs access to that register to be able to
   properly restart system calls when necessary (see
   i386-linux-tdep.c) we need our own versions of a number of
   functions that deal with GDB's register cache.  */

/* Register number for the "orig_eax" pseudo-register.  If this
   pseudo-register contains a value >= 0 it is interpreted as the
   system call number that the kernel is supposed to restart.  */
#define I386_LINUX_ORIG_EAX_REGNUM I386_SSE_NUM_REGS

/* Given REGNUM, return the corresponding offset, into GREG/user
   register area, or -1 if the map isn't applicable.  Given a -ve
   REGNUM, return the GREG's upper bound.  */
extern long i386_linux_greg_offset (int regnum);

/* Given a system dependant PTRACE buffer, supply GDB's regcache with
   the register values.  */
extern void i386_linux_supply_gregset (void *gregset);
extern void i386_linux_supply_fpregset (void *gregset);
extern void i386_linux_supply_fpxregset (void *gregset);

#endif /* i386-linux-tdep.h */
