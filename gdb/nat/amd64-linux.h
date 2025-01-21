/* Native-dependent code for GNU/Linux amd64.

   Copyright (C) 2025 Free Software Foundation, Inc.

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

#ifndef GDB_NAT_AMD64_LINUX_H
#define GDB_NAT_AMD64_LINUX_H

/* See nat/i386-linux.h for a full description of this constant.  This is
   the version used when GDB is compiled for amd64, and is running an
   executable compiled with -m32.  */

static inline constexpr int i386_initial_tls_gdt = 12;

#endif /* GDB_NAT_AMD64_LINUX_H */
