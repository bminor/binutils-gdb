/* Native-dependent code for GNU/Linux x86 (i386 and x86-64).

   Copyright (C) 1999-2024 Free Software Foundation, Inc.

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

#ifndef NAT_X86_LINUX_H
#define NAT_X86_LINUX_H

#include "nat/linux-nat.h"

/* Format of XSAVE extended state is:
	struct
	{
	  fxsave_bytes[0..463]
	  sw_usable_bytes[464..511]
	  xstate_hdr_bytes[512..575]
	  extended state regions (AVX, MPX, AVX512, PKRU, etc.)
	};

  Same memory layout will be used for the coredump NT_X86_XSTATE
  representing the XSAVE extended state registers.

  The first 8 bytes of the sw_usable_bytes[464..467] is the OS enabled
  extended state mask, which is the same as the extended control register
  0 (the XFEATURE_ENABLED_MASK register), XCR0.  We can use this mask
  together with the mask saved in the xstate_hdr_bytes to determine what
  states the processor/OS supports and what state, used or initialized,
  the process/thread is in.  */
#define I386_LINUX_XSAVE_XCR0_OFFSET 464

/* Set whether our local mirror of LWP's debug registers has been
   changed since the values were last written to the thread.  Nonzero
   indicates that a change has been made, zero indicates no change.  */

extern void lwp_set_debug_registers_changed (struct lwp_info *lwp,
					     int value);

/* Return nonzero if our local mirror of LWP's debug registers has
   been changed since the values were last written to the thread,
   zero otherwise.  */

extern int lwp_debug_registers_changed (struct lwp_info *lwp);

/* Function to call when a new thread is detected.  */

extern void x86_linux_new_thread (struct lwp_info *lwp);

/* Function to call when a thread is being deleted.  */

extern void x86_linux_delete_thread (struct arch_lwp_info *arch_lwp);

/* Function to call prior to resuming a thread.  */

extern void x86_linux_prepare_to_resume (struct lwp_info *lwp);

/* Return value from x86_linux_ptrace_get_arch_size function.  Indicates if
   a thread is 32-bit, 64-bit, or x32.  */

struct x86_linux_arch_size
{
  explicit x86_linux_arch_size (bool is_64bit, bool is_x32)
    : m_is_64bit (is_64bit),
      m_is_x32 (is_x32)
  {
    /* Nothing.  */
  }

  bool is_64bit () const
  { return m_is_64bit; }

  bool is_x32 () const
  { return m_is_x32; }

private:
  bool m_is_64bit = false;
  bool m_is_x32 = false;
};

/* Use ptrace calls to figure out if thread TID is 32-bit, 64-bit, or
   64-bit running in x32 mode.  */

extern x86_linux_arch_size x86_linux_ptrace_get_arch_size (int tid);

#endif /* NAT_X86_LINUX_H */
