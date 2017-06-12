/* Job control and terminal related functions, for GDB and gdbserver
   when running under Unix.

   Copyright (C) 1986-2017 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include "common-defs.h"
#include "job-control.h"
#include "gdb_termios.h"

/* Nonzero if we have job control.  */
int job_control;

/* Set the process group ID of the inferior.

   Just using job_control only does part of it because setpgid or
   setpgrp might not exist on a system without job control.

   For a more clean implementation, in libiberty, put a setpgid which merely
   calls setpgrp and a setpgrp which does nothing (any system with job control
   will have one or the other).  */

int
gdb_setpgid ()
{
  int retval = 0;

  if (job_control)
    {
#if defined (HAVE_TERMIOS) || defined (TIOCGPGRP)
#ifdef HAVE_SETPGID
      /* The call setpgid (0, 0) is supposed to work and mean the same
         thing as this, but on Ultrix 4.2A it fails with EPERM (and
         setpgid (getpid (), getpid ()) succeeds).  */
      retval = setpgid (getpid (), getpid ());
#else
#ifdef HAVE_SETPGRP
#ifdef SETPGRP_VOID
      retval = setpgrp ();
#else
      retval = setpgrp (getpid (), getpid ());
#endif
#endif /* HAVE_SETPGRP */
#endif /* HAVE_SETPGID */
#endif /* defined (HAVE_TERMIOS) || defined (TIOCGPGRP) */
    }

  return retval;
}

/* See common/common-terminal.h.  */

void
have_job_control ()
{
  /* OK, figure out whether we have job control.  If neither termios nor
     sgtty (i.e. termio or go32), leave job_control 0.  */
#if defined (HAVE_TERMIOS)
  /* Do all systems with termios have the POSIX way of identifying job
     control?  I hope so.  */
#ifdef _POSIX_JOB_CONTROL
  job_control = 1;
#else
#ifdef _SC_JOB_CONTROL
  job_control = sysconf (_SC_JOB_CONTROL);
#else
  job_control = 0;		/* Have to assume the worst.  */
#endif /* _SC_JOB_CONTROL */
#endif /* _POSIX_JOB_CONTROL */
#endif /* HAVE_TERMIOS */

#ifdef HAVE_SGTTY
#ifdef TIOCGPGRP
  job_control = 1;
#else
  job_control = 0;
#endif /* TIOCGPGRP */
#endif /* sgtty */
}
