/* This testcase is part of GDB, the GNU debugger.

   Copyright 2021-2022 Free Software Foundation, Inc.

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
#include <stdlib.h>
#include "../lib/my-syscalls.h"

static void *
thread_func (void *arg)
{
  my_exit (0);

  /* my_exit above should exit the thread, we don't expect to reach
     here.  */
  abort ();
}

int
main (void)
{
  int i;

  /* Spawn and join a thread, 100 times.  */
  for (i = 0; i < 100; i++)
    {
      pthread_t thread;
      int ret;

      ret = pthread_create (&thread, NULL, thread_func, NULL);
      assert (ret == 0);

      ret = pthread_join (thread, NULL);
      assert (ret == 0);
    }

  return 0;
}
