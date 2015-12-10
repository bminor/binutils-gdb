/* Test the "interrupt" command.

   Copyright 2014 Free Software Foundation, Inc.

   This file is part of GDB.

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

#ifndef NR_THREADS
#define NR_THREADS 4
#endif

pthread_t threads[NR_THREADS];

static void *
thread_entry (void *unused)
{
  while (1)
    sleep (1);
}

static void
all_threads_running (void)
{
}

static void
do_something (void)
{
}

int
main (int argc, char *argv[])
{
  int i;

  alarm (60);

  for (i = 0; i < NR_THREADS; ++i)
    pthread_create (&threads[i], NULL, thread_entry, NULL);

  all_threads_running ();

  while (1)
    do_something ();

  return 0;
}
