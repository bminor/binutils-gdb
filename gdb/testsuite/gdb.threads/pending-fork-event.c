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

#include <pthread.h>
#include <unistd.h>
#include <assert.h>

static volatile int release_forking_thread = 0;
static int x;
static pthread_barrier_t barrier;

static void
break_here (void)
{
  x++;
}

static void
do_fork (void)
{
  pthread_barrier_wait (&barrier);

  while (!release_forking_thread);

  if (FORK_FUNCTION () == 0)
    _exit (0);

}

static void *
thread_func (void *p)
{
#if defined(MAIN_THREAD_FORKS)
  break_here ();
#elif defined(OTHER_THREAD_FORKS)
  do_fork ();
#else
# error "MAIN_THREAD_FORKS or OTHER_THREAD_FORKS must be defined"
#endif
}


int
main (void)
{
  pthread_t thread;
  int ret;

  pthread_barrier_init (&barrier, NULL, 2);

  alarm (30);
  ret = pthread_create (&thread, NULL, thread_func, NULL);
  assert (ret == 0);

  pthread_barrier_wait (&barrier);

#if defined(MAIN_THREAD_FORKS)
  do_fork ();
#elif defined(OTHER_THREAD_FORKS)
  break_here ();
#else
# error "MAIN_THREAD_FORKS or OTHER_THREAD_FORKS must be defined"
#endif

  pthread_join (thread, NULL);

  return 0;
}
