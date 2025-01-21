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
#include <stdlib.h>
#include <unistd.h>

static pthread_barrier_t barrier;

static __thread int local_var = 0;

volatile int wait_for_gdb_p = 1;

volatile int should_dump_core_p = 1;

void
crash_func (void)
{
  if (should_dump_core_p)
    abort ();
}

void
spin_forever (void)
{
  while (wait_for_gdb_p)
    sleep (1);
}

void *
thread_func (void *arg)
{
  local_var = *((int*) arg);

  pthread_barrier_wait (&barrier);

  spin_forever ();
}

int
main (void)
{
  pthread_t thr;

  if (pthread_barrier_init (&barrier, NULL, 2) != 0)
    abort ();

  local_var = 1;
  int i = 2;
  if (pthread_create (&thr, NULL, thread_func, &i) != 0)
    abort ();

  pthread_barrier_wait (&barrier);

  crash_func ();

  spin_forever ();

  return 0;
}
