/* Copyright (C) 1991 Free Software Foundation, Inc.

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

/* This is for the iris. */

#define HAVE_TERMIO

#define U_REGS_OFFSET 0

#include "xm-bigmips.h"

/* Override register locations in upage for SGI machines */
#undef REGISTER_U_ADDR
#define REGISTER_U_ADDR(addr, blockend, regno) 		\
  if (regno < PC_REGNUM)				\
      addr = regno;					\
  else							\
      addr = regno + NSIG_HNDLRS; /* Skip over signal handlers */

/* Don't need special routines for the SGI -- we can use infptrace.c */
#undef FETCH_INFERIOR_REGISTERS

/* BEGIN GW MODS */
/*
 * Let's use /debug instead of all this dangerous mucking about
 * with ptrace(), which seems *extremely* fragile, anyway.
 */
#define USE_PROC_FS
#define PROC_NAME_FMT "/debug/%d"

/* If we are using SVR4 /proc instead of ptrace, use CREATE_INFERIOR_HOOK
   to do internal /proc initialization. */
#ifdef USE_PROC_FS
#define CREATE_INFERIOR_HOOK(pid) inferior_proc_init(pid)
#endif

/* Irix defines psignal() in signal.h, which gets gcc rather angry at us
 * because their definition is markedly different.
 */
#define PSIGNAL_IN_SIGNAL_H

/* If gdb's signal handling changes (due to a "handle" command), then
   this macro expands to an action to perform to notify other parts of
   gdb that might care, that signal handling has changed.  For hosts using
   the /proc interface, gdb has more control over which signals cause the
   inferior to stop and which do not.  In some cases, it is desirable to
   have signals delivered directly to the inferior without involving the
   debugger at all. */
#ifdef USE_PROC_FS
#define NOTICE_SIGNAL_HANDLING_CHANGE proc_signal_handling_change()
#endif

#define BROKEN_SIGINFO_H	/* <sys/siginfo.h> si_pid & si_uid are bogus */
