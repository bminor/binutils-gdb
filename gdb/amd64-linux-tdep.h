/* Target-dependent code for GNU/Linux AMD64.

   Copyright (C) 2006 Free Software Foundation, Inc.

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

#ifndef AMD64_LINUX_TDEP_H
#define AMD64_LINUX_TDEP_H

/* Like for i386 GNU/Linux, there is an extra "register"
   used to control syscall restarting.  */

/* Register number for the "orig_rax" register.  If this register
   contains a value >= 0 it is interpreted as the system call number
   that the kernel is supposed to restart.  */
#define AMD64_LINUX_ORIG_RAX_REGNUM (AMD64_MXCSR_REGNUM + 1)

/* Total number of registers for GNU/Linux.  */
#define AMD64_LINUX_NUM_REGS (AMD64_LINUX_ORIG_RAX_REGNUM + 1)

#endif /* amd64-linux-tdep.h */
