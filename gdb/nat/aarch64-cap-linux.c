/* Copyright (C) 2020-2021 Free Software Foundation, Inc.

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
#include "gdb_ptrace.h"
#include "aarch64-cap-linux.h"
#include <sys/uio.h>
#include "elf/common.h"

/* Helper function to display various possible errors when reading
   Morello capabilities from memory.  */

static void
morello_linux_peekcap_error (int error)
{
  switch (error)
    {
    case EIO:
    case EFAULT:
      warning (_("PTRACE_PEEKCAP: Couldn't fetch capability"));
      break;
    default:
      warning (_("PTRACE_PEEKCAP: Unknown error"));
      break;
    }
}

/* Helper function to display various possible errors when writing
   Morello capabilities to memory.  */

static void
morello_linux_pokecap_error (int error)
{
  switch (error)
    {
    case EIO:
    case EFAULT:
      warning (_("PTRACE_POKECAP: Couldn't store capability"));
      break;
    case EPERM:
      warning (_("PTRACE_POKECAP:: Couldn't store capability.\n"
		 "Make sure the sysctl flag cheri.ptrace_forge_cap is 1"));
      break;
    case EOPNOTSUPP:
      warning (_("PTRACE_POKECAP: Capability tags not enabled for"
		 " requested address"));
      break;
    default:
      warning (_("PTRACE_POKECAP: Unknown error"));
      break;
    }
}

/* See aarch64-cap-linux.h */

bool
aarch64_linux_read_capability (int tid, CORE_ADDR address,
			       user_cap &cap)
{
  cap.val = 0;
  cap.tag = 0;
  memset (cap.__reserved, 0, 15);

  /* Fetch the tag from ptrace.  */
  if (ptrace (PTRACE_PEEKCAP, tid, address, &cap) < 0)
    {
      morello_linux_peekcap_error (errno);
      return false;
    }

  return true;
}

/* See aarch64-cap-linux.h */

bool
aarch64_linux_write_capability (int tid, CORE_ADDR address,
				const user_cap &cap)
{
  /* Fetch the tag from ptrace.  */
  if (ptrace (PTRACE_POKECAP, tid, address, &cap) < 0)
    {
      morello_linux_pokecap_error (errno);
      return false;
    }

  return true;
}

/* See aach64-cap-linux.h */

bool
aarch64_supports_morello (int tid)
{
  struct user_morello_state cregset;
  struct iovec iovec;
  iovec.iov_base = &cregset;
  iovec.iov_len = sizeof (cregset);

  /* Attempt to fetch NT_ARM_MORELLO.  If it is supported, that means Morello
     features are supported and that PTRACE_PEEKCAP and PTRACE_POKECAP are
     also supported.  */
  if (ptrace (PTRACE_GETREGSET, tid, NT_ARM_MORELLO, &iovec) < 0)
    return false;

  return true;
}
