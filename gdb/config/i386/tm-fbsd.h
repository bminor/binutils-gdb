/* Target-dependent definitions for FreeBSD/i386.
   Copyright (C) 1997, 2000 Free Software Foundation, Inc.

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

#ifndef TM_FBSD_H
#define TM_FBSD_H

#define HAVE_I387_REGS
#include "i386/tm-i386.h"

/* FreeBSD uses the old gcc convention for struct returns.  */

#define USE_STRUCT_CONVENTION(gcc_p, type) \
  generic_use_struct_convention (1, type)


/* Support for longjmp.  */

/* Details about jmp_buf.  It's supposed to be an array of integers.  */

#define JB_ELEMENT_SIZE 4	/* Size of elements in jmp_buf.  */
#define JB_PC		0	/* Array index of saved PC.  */

/* Figure out where the longjmp will land.  Store the address that
   longjmp will jump to in *ADDR, and return non-zero if successful.  */

extern int get_longjmp_target (CORE_ADDR *addr);
#define GET_LONGJMP_TARGET(addr) get_longjmp_target (addr)


/* Support for signal handlers.  */

/* The sigtramp is above the user stack and immediately below
   the user area.  Using constants here allows for cross debugging.
   These are tested for FreeBSD 3.4.  */

#define SIGTRAMP_START(pc)	0xbfbfdf20
#define SIGTRAMP_END(pc)	0xbfbfdff0

/* Offset to saved PC in sigcontext, from <sys/signal.h>.  */
#define SIGCONTEXT_PC_OFFSET 20


/* Shared library support.  */

#ifndef SVR4_SHARED_LIBS

/* Return non-zero if we are in a shared library trampoline code stub.  */

#define IN_SOLIB_CALL_TRAMPOLINE(pc, name) \
  (name && !strcmp(name, "_DYNAMIC"))

#endif /* !SVR4_SHARED_LIBS */

#endif /* TM_FBSD_H */
