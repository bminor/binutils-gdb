/* Common native Linux definitions for AArch64 MTE.

   Copyright (C) 2018-2020 Free Software Foundation, Inc.

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

#ifndef NAT_AARCH64_MTE_LINUX_PTRACE_H
#define NAT_AARCH64_MTE_LINUX_PTRACE_H

/* MTE allocation tag access */

#ifndef PTRACE_PEEKMTETAGS
#define PTRACE_PEEKMTETAGS	  33
#endif

#ifndef PTRACE_POKEMTETAGS
#define PTRACE_POKEMTETAGS	  34
#endif

#endif /* NAT_AARCH64_MTE_LINUX_PTRACE_H */
