/* Native definitions for alpha running OSF/1-2.x and higher, using procfs.
   Copyright (C) 1995 Free Software Foundation, Inc.

This file is part of GDB.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* Get generic OSF/1 definitions.  */
#include "alpha/nm-osf.h"

/* OSF/1-2.x has optional /proc support, try to use it instead of ptrace.  */
#define USE_PROC_FS
#define HAVE_OPTIONAL_PROC_FS

/* OSF/1 doesn't provide the standard fault definitions, so don't use them.  */
#define FAULTED_USE_SIGINFO

/* Work around some peculiarities in the OSF/1 procfs implementation.  */
#define PROCFS_DONT_TRACE_IFAULT
#define PROCFS_SIGPEND_OFFSET
#define PROCFS_NEED_PIOCSSIG_FOR_KILL
#define PROCFS_DONT_PIOCSSIG_CURSIG
