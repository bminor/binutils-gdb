/* Signal handler definitions for GDB, the GNU Debugger.
   Copyright (C) 1986, 1989 Free Software Foundation, Inc.

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


/*  This file is almost the same as including <signal.h> except that it
    eliminates certain signal names when job control is not supported,
    (or, on some systems, when job control is there but doesn't work
    the way GDB expects it to work).  */

#include <signal.h>

#ifdef NO_JOB_CONTROL
# undef SIGTSTP
# undef SIGSTOP
# undef SIGCONT
# undef SIGTTIN
# undef SIGTTOU
#endif
