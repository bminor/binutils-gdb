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
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t g_condvar = PTHREAD_COND_INITIALIZER;

void *
worker_function (void *arg)
{
  printf ("In worker, about to notify\n");
  pthread_cond_signal (&g_condvar);

  while (true)
    sleep(1);

  return NULL;
}

int
main()
{
  pthread_t my_thread;

  int result = pthread_create (&my_thread, NULL, worker_function, NULL);
  assert (result == 0);

  pthread_mutex_lock (&g_mutex);
  pthread_cond_wait (&g_condvar, &g_mutex);

  printf ("In main, have been woken.\n");
  pthread_mutex_unlock (&g_mutex);

  result = pthread_kill (my_thread, SIGTRAP);
  assert (result == 0);

  result = pthread_join (my_thread, NULL);
  assert (result == 0);

  return 0;
}
