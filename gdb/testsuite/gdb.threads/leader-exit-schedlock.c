/* This testcase is part of GDB, the GNU debugger.

   Copyright 2025 Free Software Foundation, Inc.

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

#include <pthread.h>
#include <assert.h>
#include <unistd.h>

static pthread_barrier_t threads_started_barrier;

static void *
start (void *arg)
{
  pthread_barrier_wait (&threads_started_barrier);

  while (1)
    sleep (1);

  return NULL;
}

#define NTHREADS 10

int
main (void)
{
  int i;

  pthread_barrier_init (&threads_started_barrier, NULL, NTHREADS + 1);

  for (i = 0; i < NTHREADS; i++)
    {
      pthread_t thread;
      int res;

      res = pthread_create (&thread, NULL, start, NULL);
      assert (res == 0);
    }

  pthread_barrier_wait (&threads_started_barrier);

  return 0; /* break-here */
}
