/* Native support for Linux/x86.
   Copyright 1986, 1987, 1989, 1992, 1993, 1994, 1995, 1996, 1997, 1998,
   1999, 2000, 2001
   Free Software Foundation, Inc.

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

#ifndef NM_LINUX_H
#define NM_LINUX_H

/* GNU/Linux supports the i386 hardware debugging registers.  */
#define I386_USE_GENERIC_WATCHPOINTS

#include "i386/nm-i386.h"
#include "nm-linux.h"

/* Return sizeof user struct to callers in less machine dependent
   routines.  */

extern int kernel_u_size (void);
#define KERNEL_U_SIZE kernel_u_size()

#define U_REGS_OFFSET 0

extern CORE_ADDR register_u_addr (CORE_ADDR blockend, int regnum);
#define REGISTER_U_ADDR(addr, blockend, regnum) \
  (addr) = register_u_addr (blockend, regnum)

/* Provide access to the i386 hardware debugging registers.  */

extern void i386_linux_dr_set_control (unsigned long control);
#define I386_DR_LOW_SET_CONTROL(control) \
  i386_linux_dr_set_control (control)

extern void i386_linux_dr_set_addr (int regnum, CORE_ADDR addr);
#define I386_DR_LOW_SET_ADDR(regnum, addr) \
  i386_linux_dr_set_addr (regnum, addr)

extern void i386_linux_dr_reset_addr (int regnum);
#define I386_DR_LOW_RESET_ADDR(regnum) \
  i386_linux_dr_reset_addr (regnum)

extern unsigned long i386_linux_dr_get_status (void);
#define I386_DR_LOW_GET_STATUS() \
  i386_linux_dr_get_status ()

/* We define this if link.h is available, because with ELF we use SVR4
   style shared libraries.  */

#ifdef HAVE_LINK_H
#define SVR4_SHARED_LIBS
#include "solib.h"		/* Support for shared libraries.  */
#endif

/* Override copies of {fetch,store}_inferior_registers in `infptrace.c'.  */
#define FETCH_INFERIOR_REGISTERS

/* Nevertheless, define CANNOT_{FETCH,STORE}_REGISTER, because we
   might fall back on the code `infptrace.c' (well a copy of that code
   in `i386-linux-nat.c' for now) and we can access only the
   general-purpose registers in that way.  */
extern int cannot_fetch_register (int regno);
extern int cannot_store_register (int regno);
#define CANNOT_FETCH_REGISTER(regno) cannot_store_register (regno)
#define CANNOT_STORE_REGISTER(regno) cannot_fetch_register (regno)

/* Override child_resume in `infptrace.c'.  */
#define CHILD_RESUME

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

#endif /* nm_linux.h */
