/* This testcase is part of GDB, the GNU debugger.

   Copyright 2020-2025 Free Software Foundation, Inc.

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
#include <pthread.h>
#include <unistd.h>
#include <assert.h>

/* Used for thread synchronisation.  */

pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t g_cond = PTHREAD_COND_INITIALIZER;

/* This is set by GDB.  */

volatile int wait_for_gdb = 1;

/* This is used to create some work for GDB to step through.  */

volatile int global_var = 0;

/* A simple thread worker function.  */

void*
worker_thread_func (void *arg)
{
  int res;

  /* Grab the mutex.  This completes once the main thread is waiting.  */
  res = pthread_mutex_lock (&g_mutex);
  assert (res == 0);

  /* Wake the main thread, letting it know that we are here.  At this
     point the main thread is still blocked as we hold G_MUTEX.  */
  res = pthread_cond_signal (&g_cond);

  /* Now we wait.  This releases G_MUTEX and allows the main thread to
     continue.  */
  res = pthread_cond_wait (&g_cond, &g_mutex);
  assert (res == 0);

  /* Unlock the mutex.  We're all done now.  */
  res = pthread_mutex_unlock (&g_mutex);
  assert (res == 0);

  return NULL;
}

int
main (void)
{
  pthread_t thr;
  int res;

  /* Lock G_MUTEX before creating the worker thread.  */
  pthread_mutex_lock (&g_mutex);

  res = pthread_create (&thr, NULL, worker_thread_func, NULL);
  assert (res == 0);

  /* Release G_MUTEX and wait for the worker thread.  */
  res = pthread_cond_wait (&g_cond, &g_mutex);
  assert (res == 0);

  global_var++;	/* Break here.  */
  global_var++;	/* Second.  */
  global_var++;	/* Third.  */

  while (wait_for_gdb)
    sleep(1);

  /* Notify the worker thread, it will exit once G_MUTEX is released.  */
  pthread_cond_signal (&g_cond);
  pthread_mutex_unlock (&g_mutex);

  /* Wait for the worker to actually exit.  */
  res = pthread_join (thr, NULL);
  assert (res == 0);

  /* Clean up the mutex and condition variable.  */
  pthread_mutex_destroy (&g_mutex);
  pthread_cond_destroy (&g_cond);

  return 0;
}
