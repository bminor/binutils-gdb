/* Native support for linux, for GDB, the GNU debugger.
   Copyright (C) 1996,1998 Free Software Foundation, Inc.

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#ifndef NM_LINUX_H
#define NM_LINUX_H

/* Return sizeof user struct to callers in less machine dependent routines */

#define KERNEL_U_SIZE kernel_u_size()
extern int kernel_u_size PARAMS ((void));

/* Tell gdb that we can attach and detach other processes */
#define ATTACH_DETACH

#define U_REGS_OFFSET 0

/* We define this if link.h is available, because with ELF we use SVR4 style
   shared libraries. */

#ifdef HAVE_LINK_H
#define SVR4_SHARED_LIBS
#include "solib.h"		/* Support for shared libraries. */
#endif

#define REGISTER_U_ADDR(addr, blockend, regno) \
	(addr) = m68k_linux_register_u_addr ((blockend),(regno));

extern int
m68k_linux_register_u_addr PARAMS ((int, int));

#endif /* #ifndef NM_LINUX_H */
