/* Definitions for irix4 hosting support.

Copyright (C) 1991, 1992 Free Software Foundation, Inc.

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

#include "mips/xm-irix3.h"

/* Irix defines psignal() in signal.h, in a way which conflicts with the
   definition in defs.h.  */

#define PSIGNAL_IN_SIGNAL_H

#define BROKEN_SIGINFO_H	/* <sys/siginfo.h> si_pid & si_uid are bogus */

/* Irix 4.0.1 and later have termios.  Not sure about earlier versions.  */
#undef HAVE_TERMIO
#define HAVE_TERMIOS
