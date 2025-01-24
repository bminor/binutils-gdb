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

volatile int wait_for_gdb = 1;

#define NUM_THREADS 3

static pthread_barrier_t threads_started_barrier;

static pthread_barrier_t may_exit_barrier;

static void *
thread_func (void *arg)
{
  /* Wait until all threads have started.  */
  pthread_barrier_wait (&threads_started_barrier);

  /* Wait until the main thread lets us exit.  */
  pthread_barrier_wait (&may_exit_barrier);

  return NULL;
}

static void
threads_started (void)
{
}

int
main (void)
{
  pthread_t thread[NUM_THREADS];
  int i;

  alarm (30);

  pthread_barrier_init (&threads_started_barrier, NULL, NUM_THREADS + 1);
  pthread_barrier_init (&may_exit_barrier, NULL, NUM_THREADS + 1);

  for (i = 0; i < NUM_THREADS; i++)
    {
      int ret;

      ret = pthread_create (&thread[i], NULL, thread_func, NULL);
      assert (ret == 0);
    }

  pthread_barrier_wait (&threads_started_barrier);

  threads_started ();

  pthread_barrier_wait (&may_exit_barrier);

  for (i = 0; i < NUM_THREADS; i++)
    pthread_join (thread[i], NULL);

  return 0;
}
