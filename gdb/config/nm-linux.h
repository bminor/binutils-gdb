/* Native support for GNU/Linux.
   Copyright 1999, 2000, 2001
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

/* Linux is SVR4-ish but its /proc file system isn't.  */
#undef USE_PROC_FS

/* Tell GDB that we can attach and detach other processes.  */
#define ATTACH_DETACH

/* We define this if link.h is available, because with ELF we use SVR4
   style shared libraries.  */

#ifdef HAVE_LINK_H
#define SVR4_SHARED_LIBS
#include "solib.h"             /* Support for shared libraries.  */
#endif


/* FIXME: kettenis/2001-07-11: Stuff on this page is obsolete, and
   only used by the (unmaintained) sparc and m68k ports.  */

/* Support for the glibc LinuxThreads package.  */

struct objfile;

/* Hook to look at new objfiles (shared libraries).  */
extern void linuxthreads_new_objfile (struct objfile *objfile);

/* Method to print a human-readable thread description.  */
extern char *linuxthreads_pid_to_str (ptid_t ptid);

extern int linuxthreads_prepare_to_proceed (int step);
#define PREPARE_TO_PROCEED(select_it) linuxthreads_prepare_to_proceed (1)


/* Defined to make stepping-over-breakpoints be thread-atomic.  */
#define USE_THREAD_STEP_NEEDED 1

/* Use elf_gregset_t and elf_fpregset_t, rather than
   gregset_t and fpregset_t.  */

#define GDB_GREGSET_T  elf_gregset_t
#define GDB_FPREGSET_T elf_fpregset_t

/* Since we're building a native debugger, we can include <signal.h>
   to find the range of real-time signals.  */

#include <signal.h>

#ifdef __SIGRTMIN
#define REALTIME_LO	__SIGRTMIN
#define REALTIME_HI	(__SIGRTMAX + 1)
#endif
