/* Terminal interface definitions for GDB, the GNU Debugger.
   Copyright (C) 1986, 1989, 1991 Free Software Foundation, Inc.

This file is part of GDB.

GDB is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GDB is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GDB; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */


/* Define a common set of macros -- BSD based -- and redefine whatever
   the system offers to make it look like that.  */

#ifdef HAVE_TERMIO

#include <termio.h>

#undef TIOCGETP
#define TIOCGETP TCGETA
#undef TIOCSETN
#define TIOCSETN TCSETA
#undef TIOCSETP
#define TIOCSETP TCSETAF
#define TERMINAL struct termio

#ifdef NO_JOB_CONTROL
# undef TIOCGPGRP
# undef TIOCGPGRP
#endif

#else /* no termio */

#include <fcntl.h>
#include <sgtty.h>
#include <sys/ioctl.h>
#define TERMINAL struct sgttyb

#endif /* no termio */

extern void new_tty ();
