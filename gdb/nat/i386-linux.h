/* Native-dependent code for GNU/Linux i386.

   Copyright (C) 2024-2025 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef GDB_NAT_I386_LINUX_H
#define GDB_NAT_I386_LINUX_H

/* Does the current host support the GETFPXREGS request?  The system header
   file may or may not define it, but even if it is defined, the kernel
   will return EIO if it's running on a pre-SSE processor.

   Initially this will be TRIBOOL_UNKNOWN and should be changed to
   TRIBOOL_FALSE if the ptrace call is attempted and fails or changed to
   TRIBOOL_TRUE if the ptrace call is attempted and succeeds.

   My instinct is to attach this to some architecture- or target-specific
   data structure, but really, a particular GDB process can only run on top
   of one kernel at a time.  So it's okay - for this to be a global
   variable.  */
extern tribool have_ptrace_getfpxregs;

/* This constant defines the first GDT (Global Descriptor Table) entry
   that the kernel allocates for holding TLS descriptors.  There are three
   entries, starting at this index which can be accessed using the
   PTRACE_GET_THREAD_AREA and PTRACE_SET_THREAD_AREA ptrace calls.  This
   constant is only valid for true i386 kernels.  For amd64 kernels
   running in 32-bit mode (i.e. executables compiled -m32) there is a
   different constant, see nat/amd64-linux.h.  */

static inline constexpr int i386_initial_tls_gdt = 6;

#endif /* GDB_NAT_I386_LINUX_H */
