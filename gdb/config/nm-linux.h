/* Native support for GNU/Linux, for GDB, the GNU debugger.
   Copyright (C) 1999
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

/* Linux is svr4ish but not that much */
#undef USE_PROC_FS

/* Tell gdb that we can attach and detach other processes */
#define ATTACH_DETACH

/* We define this if link.h is available, because with ELF we use SVR4 style
   shared libraries. */

#ifdef HAVE_LINK_H
#define SVR4_SHARED_LIBS
#include "solib.h"             /* Support for shared libraries. */
#endif

/* Support for the glibc linuxthreads package. */

struct objfile;

extern void
linuxthreads_new_objfile PARAMS ((struct objfile *objfile));
#define target_new_objfile(OBJFILE) linuxthreads_new_objfile (OBJFILE)

extern char *
linuxthreads_pid_to_str PARAMS ((int pid));
#define target_pid_to_str(PID) linuxthreads_pid_to_str (PID)

extern int
linuxthreads_prepare_to_proceed PARAMS ((int step));
#define PREPARE_TO_PROCEED(select_it) linuxthreads_prepare_to_proceed (1)
