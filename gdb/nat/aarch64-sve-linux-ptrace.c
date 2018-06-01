/* Common target dependent for AArch64 systems.

   Copyright (C) 2018 Free Software Foundation, Inc.

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

#include <sys/utsname.h>
#include <sys/uio.h>
#include "common-defs.h"
#include "elf/external.h"
#include "elf/common.h"
#include "aarch64-sve-linux-ptrace.h"
#include "arch/aarch64.h"

/* See nat/aarch64-sve-linux-ptrace.h.  */

uint64_t
aarch64_sve_get_vq (int tid)
{
  struct iovec iovec;
  struct user_sve_header header;

  iovec.iov_len = sizeof (header);
  iovec.iov_base = &header;

  /* Ptrace gives the vector length in bytes.  Convert it to VQ, the number of
     128bit chunks in a Z register.  We use VQ because 128bits is the minimum
     a Z register can increase in size.  */

  if (ptrace (PTRACE_GETREGSET, tid, NT_ARM_SVE, &iovec) < 0)
    {
      /* SVE is not supported.  */
      return 0;
    }

  long vq = sve_vq_from_vl (header.vl);

  if (!sve_vl_valid (header.vl))
    {
      warning (_("Invalid SVE state from kernel; SVE disabled."));
      return 0;
    }

  return vq;
}
