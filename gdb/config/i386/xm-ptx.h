/* Definitions to make GDB run on a Sequent Symmetry under ptx, with
   Weitek 1167 and i387 support.  Copyright 1986, 1987, 1989, 1992,
   1993 Free Software Foundation, Inc.

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

/* Symmetry version by Jay Vosburgh (fubar@sequent.com) */

#ifdef _SEQUENT_PTX4_
#include "xm-sysv4.h"
#endif /* _SEQUENT_PTX4_ */

/* This machine doesn't have the siginterrupt call.  */
#define NO_SIGINTERRUPT

#define HAVE_WAIT_STRUCT

#define HAVE_MMAP

#undef HAVE_TERMIO
#define HAVE_TERMIOS
#define USG

#define MEM_FNS_DECLARED

#define NEED_POSIX_SETPGID

#define USE_O_NOCTTY

#define HOST_BYTE_ORDER LITTLE_ENDIAN

/* We must fetch all the regs before storing, since we store all at once.  */

#define CHILD_PREPARE_TO_STORE() read_register_bytes (0, NULL, REGISTER_BYTES)
