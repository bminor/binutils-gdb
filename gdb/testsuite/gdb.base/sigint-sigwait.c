/* This testcase is part of GDB, the GNU debugger.

   Copyright 2021 Free Software Foundation, Inc.

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

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

static void
handle_sigint (int signo)
{
}

static void
done ()
{
}

int
main (int argc, char **argv)
{
  sigset_t signal_set;
  struct sigaction s;

  alarm (30);

  s.sa_flags = SA_RESTART;
  s.sa_handler = handle_sigint;

  if (sigaction (SIGINT, &s, NULL) < 0)
    {
      perror ("<%s> Error sigaction: ");
      exit (2);
    }

  if (sigfillset (&signal_set) < 0)
    {
      perror ("<main> Error sigfillset(): ");
      exit (2);
    }

  done ();

  while (1)
    {
      int sig;

      /* Wait for queued signals.  .*/
      if (sigwait (&signal_set, &sig) != 0)
	{
	  perror ("<main> Error sigwait(): ");
	  exit (1);
	}

      /* Process signal.  */
      if (sig == SIGINT)
	{
	  printf ("Terminating.\n");
	  exit (0);
	}

      printf ("Unhandled signal [%s]\n", strsignal (sig));
    }
}
