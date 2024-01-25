/* Target description related code for GNU/Linux x86 (i386 and x86-64).

   Copyright (C) 2024 Free Software Foundation, Inc.

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

#ifndef NAT_X86_LINUX_TDESC_H
#define NAT_X86_LINUX_TDESC_H

#include "gdbsupport/function-view.h"

struct target_desc;

/* Return the target description for Linux thread TID.

   When *HAVE_PTRACE_GETREGSET is TRIBOOL_UNKNOWN then the current value of
   xcr0 is read using ptrace calls and stored into *XCR0_STORAGE.  Then
   XCR0_INIT_CB is called with the value of *XCR0_STORAGE and
   *HAVE_PTRACE_GETREGSET is set to TRIBOOL_TRUE.

   If the attempt to read xcr0 using ptrace fails then *XCR0_STORAGE is set
   to zero and *HAVE_PTRACE_GETREGSET is set to TRIBOOL_FALSE.

   The storage pointed to by XCR0_STORAGE must exist until the program
   terminates, this storage is used to cache the xcr0 value.  As such
   XCR0_INIT_CB will only be called once if xcr0 is successfully read using
   ptrace, or not at all if the ptrace call fails.

   This function returns a target description based on the extracted xcr0
   value along with other characteristics of the thread identified by TID.

   This function can return nullptr if we encounter a machine configuration
   for which a target_desc cannot be created.  Ideally this would not be
   the case, we should be able to create a target description for every
   possible machine configuration.  See amd64_linux_read_description and
   i386_linux_read_description for cases when nullptr might be
   returned.

   ERROR_MSG is using in an error() call if we try to create a target
   description for a 64-bit process but this is a 32-bit build of GDB.  */

extern const target_desc *
x86_linux_tdesc_for_tid (int tid, enum tribool *have_ptrace_getregset,
			 gdb::function_view<void (uint64_t)> xcr0_init_cb,
			 const char *error_msg, uint64_t *xcr0_storage);

#ifdef __x86_64__

/* Return the right amd64-linux target descriptions according to
   XCR0_FEATURES_BIT and IS_X32.  This is implemented separately in both
   GDB and gdbserver.  */

extern const target_desc *amd64_linux_read_description
	(uint64_t xcr0_features_bit, bool is_x32);

#endif

/* Return the target description according to XCR0.  This is implemented
   separately in both GDB and gdbserver.  */
extern const struct target_desc *i386_linux_read_description (uint64_t xcr0);

#endif /* NAT_X86_LINUX_TDESC_H */
