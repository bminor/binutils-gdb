/* Internal interfaces for the NetBSD code.

   Copyright (C) 2006-2020 Free Software Foundation, Inc.

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

#ifndef NAT_NETBSD_NAT_H
#define NAT_NETBSD_NAT_H

#include <unistd.h>

namespace netbsd_nat
{

/* Return the executable file name of a process specified by PID.  Returns the
   string in a static buffer.  */

extern const char *pid_to_exec_file (pid_t pid);

}

#endif
