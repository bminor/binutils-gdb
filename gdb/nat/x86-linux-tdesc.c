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

#include "gdbsupport/common-defs.h"
#include "nat/x86-linux-tdesc.h"
#ifdef __x86_64__
#include "arch/amd64.h"
#endif
#include "arch/i386.h"

#include "gdbsupport/common-defs.h"
#include "nat/x86-linux.h"
#include "nat/x86-linux-dregs.h"
#include "nat/gdb_ptrace.h"
#include "nat/x86-xstate.h"
#include "nat/x86-linux-tdesc.h"

#include <cstdint>
#include <cstdlib>
#include <linux/uio.h>
#include <elf.h>
#include <sys/user.h>
#include <sys/user.h>

/* See nat/x86-linux-tdesc.h.  */

const target_desc *
x86_linux_tdesc_for_tid (int tid, enum tribool *have_ptrace_getregset,
			 gdb::function_view<void (uint64_t)> xcr0_init_cb,
			 const char *error_msg, uint64_t *xcr0_storage)
{
#ifdef __x86_64__

  x86_linux_arch_size arch_size = x86_linux_ptrace_get_arch_size (tid);
  bool is_64bit = arch_size.is_64bit ();
  bool is_x32 = arch_size.is_x32 ();

  if (sizeof (void *) == 4 && is_64bit && !is_x32)
    error ("%s", error_msg);

#elif HAVE_PTRACE_GETFPXREGS
  if (have_ptrace_getfpxregs == -1)
    {
      elf_fpxregset_t fpxregs;

      if (ptrace (PTRACE_GETFPXREGS, tid, 0, (int) &fpxregs) < 0)
	{
	  have_ptrace_getfpxregs = 0;
	  *have_ptrace_getregset = TRIBOOL_FALSE;
	  return i386_linux_read_description (X86_XSTATE_X87_MASK);
	}
    }
#endif

  if (*have_ptrace_getregset == TRIBOOL_UNKNOWN)
    {
      uint64_t xstateregs[(X86_XSTATE_SSE_SIZE / sizeof (uint64_t))];
      struct iovec iov;

      iov.iov_base = xstateregs;
      iov.iov_len = sizeof (xstateregs);

      /* Check if PTRACE_GETREGSET works.  */
      if (ptrace (PTRACE_GETREGSET, tid,
		  (unsigned int) NT_X86_XSTATE, &iov) < 0)
	{
	  *have_ptrace_getregset = TRIBOOL_FALSE;
	  *xcr0_storage = 0;
	}
      else
	{
	  *have_ptrace_getregset = TRIBOOL_TRUE;

	  /* Get XCR0 from XSAVE extended state.  */
	  *xcr0_storage = xstateregs[(I386_LINUX_XSAVE_XCR0_OFFSET
				      / sizeof (uint64_t))];

#ifdef __x86_64__
	  /* No MPX on x32.  */
	  if (is_64bit && is_x32)
	    *xcr0_storage &= ~X86_XSTATE_MPX;
#endif /* __x86_64__ */

	  xcr0_init_cb (*xcr0_storage);
	}
    }

  /* Check the native XCR0 only if PTRACE_GETREGSET is available.  If
     PTRACE_GETREGSET is not available then set xcr0_features_bits to
     zero so that the "no-features" descriptions are returned by the
     switches below.  */
  uint64_t xcr0_features_bits;
  if (*have_ptrace_getregset == TRIBOOL_TRUE)
    xcr0_features_bits = *xcr0_storage & X86_XSTATE_ALL_MASK;
  else
    xcr0_features_bits = 0;

#ifdef __x86_64__
  if (is_64bit)
    {
      return amd64_linux_read_description (xcr0_features_bits, is_x32);
    }
  else
#endif
    return i386_linux_read_description (xcr0_features_bits);

  gdb_assert_not_reached ("failed to return tdesc");
}
