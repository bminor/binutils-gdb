/* Definitions for running gdb on a host machine running any flavor of SVR4.
   Copyright 1991, 1992 Free Software Foundation, Inc.
   Written by Fred Fish at Cygnus Support (fnf@cygnus.com).

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

/* Use SVR4 style shared library support */

#define SVR4_SHARED_LIBS

/* SVR4 has /proc support, so use it instead of ptrace. */

#define USE_PROC_FS

/* SVR4 has termio facilities. */

#define HAVE_TERMIO

/* SVR4 has mmap facilities */

#define HAVE_MMAP

/* TIOCGETC and TIOCGLTC are picked up somewhere, but struct tchars
   and struct ltchars are not.  This makes problems for inflow.c.
   It is unknown at this time if this is a generic SVR4 problem or
   one just limited to the initial SVR4 port host machine. */

#define TIOCGETC_BROKEN
#define TIOCGLTC_BROKEN

/* SVR4 is a derivative of System V Release 3 (USG) */

#define USG

/* Get rid of any system-imposed stack limit if possible.  */

/* #define SET_STACK_LIMIT_HUGE */

/* SVR4 machines can easily do attach and detach via /proc (procfs.c)
   support */

#define ATTACH_DETACH

/* If we are using SVR4 /proc instead of ptrace, use CREATE_INFERIOR_HOOK
   to do internal /proc initialization. */

#ifdef USE_PROC_FS
#define CREATE_INFERIOR_HOOK(pid) inferior_proc_init(pid)
#endif

/* Use setpgid(0,0) to run inferior in a separate process group */

#define NEED_POSIX_SETPGID

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
