/* This testcase is part of GDB, the GNU debugger.

   Copyright 2017-2018 Free Software Foundation, Inc.

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

static void
break_here (void)
{
}

static void *
thread_routine (void *arg)
{
  errno = 42;

  break_here ();

  while (1)
    sleep (1);

  return NULL;
}

int
main (int argc, char *argv)
{
  pthread_t the_thread;
  int err;

  err = pthread_create (&the_thread, NULL, thread_routine, NULL);
  if (err != 0)
    {
      fprintf (stderr, "pthread_create: %s (%d)\n", strerror (err), err);
      exit (EXIT_FAILURE);
    }

  errno = 23;

  err = pthread_join (the_thread, NULL);
  if (err != 0)
    {
      fprintf (stderr, "pthread_join: %s (%d)\n", strerror (err), err);
      exit (EXIT_FAILURE);
    }

  exit (EXIT_SUCCESS);
}
