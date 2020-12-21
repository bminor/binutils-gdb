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
#include "aarch64-cap-linux.h"
#include "gdb_ptrace.h"

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
    return false;

  return true;
}
