/* Definitions to make GDB run on an ARM based machine under GNU/Linux.
   Copyright 1999, 2000, 2001 Free Software Foundation, Inc.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#ifndef NM_ARMLINUX_H
#define NM_ARMLINUX_H

#include "nm-linux.h"

/* Return sizeof user struct to callers in less machine dependent routines */
extern int kernel_u_size (void);
#define KERNEL_U_SIZE	arm_linux_kernel_u_size()

/* Override copies of {fetch,store}_inferior_registers in infptrace.c.  */
#define FETCH_INFERIOR_REGISTERS

/* FIXME: kettenis/2000-09-03: This should be moved to ../nm-linux.h
   once we have converted all Linux targets to use the new threads
   stuff (without the #undef of course).  */

extern int lin_lwp_prepare_to_proceed (void);
#undef PREPARE_TO_PROCEED
#define PREPARE_TO_PROCEED(select_it) lin_lwp_prepare_to_proceed ()

extern void lin_lwp_attach_lwp (ptid_t ptid, int verbose);
#define ATTACH_LWP(ptid, verbose) lin_lwp_attach_lwp ((ptid), (verbose))

#include <signal.h>

extern void lin_thread_get_thread_signals (sigset_t *mask);
#define GET_THREAD_SIGNALS(mask) lin_thread_get_thread_signals (mask)

#endif /* NM_ARMLINUX_H */
