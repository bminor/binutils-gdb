/* This testcase is part of GDB, the GNU debugger.

   Copyright 2004 Free Software Foundation, Inc.

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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

*/

#include <stdio.h>
#include <signal.h>
#include <sys/time.h>

static volatile int done;

static void
handler (int sig)
{
  done = 1;
} /* handler */

main ()
{
  /* Set up the signal handler.  */
  {
    struct sigaction action;
    memset (&action, 0, sizeof (action));
    action.sa_handler = handler;
    sigaction (SIGVTALRM, &action, NULL);
  }

  /* Set up a one-off timer.  A timer, rather than SIGSEGV, is used as
     after a timer handler finishes the interrupted code can safely
     resume.  */
  {
    struct itimerval itime;
    memset (&itime, 0, sizeof (itime));
    itime.it_value.tv_usec = 250 * 1000;
    setitimer (ITIMER_VIRTUAL, &itime, NULL);
  }

  /* Wait.  */
  while (!done);
  return 0;
} /* main */
