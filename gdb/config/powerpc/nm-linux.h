/* IBM PowerPC native-dependent macros for GDB, the GNU debugger.
   Copyright 1995 Free Software Foundation, Inc.

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

#ifndef NM_LINUX_H

#include "nm-linux.h"

#define NM_LINUX_H

/* Return sizeof user struct to callers in less machine dependent routines */

#define KERNEL_U_SIZE kernel_u_size()
extern int kernel_u_size (void);

#define U_REGS_OFFSET 0

#define REGISTER_U_ADDR(addr, blockend, regno) \
        (addr) = ppc_register_u_addr ((blockend),(regno));

/* No <sys/reg.h> */

#define NO_SYS_REG_H

/* FIXME: kettenis/2000-09-03: This should be moved to ../nm-linux.h
   once we have converted all Linux targets to use the new threads
   stuff (without the #undef of course).  */

extern int lin_lwp_prepare_to_proceed (void);
#undef PREPARE_TO_PROCEED
#define PREPARE_TO_PROCEED(select_it) lin_lwp_prepare_to_proceed ()

extern void lin_lwp_attach_lwp (int pid, int verbose);
#define ATTACH_LWP(pid, verbose) lin_lwp_attach_lwp ((pid), (verbose))

#include <signal.h>

extern void lin_thread_get_thread_signals (sigset_t *mask);
#define GET_THREAD_SIGNALS(mask) lin_thread_get_thread_signals (mask)

#endif /* #ifndef NM_LINUX_H */
