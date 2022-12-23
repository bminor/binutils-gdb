/* This testcase is part of GDB, the GNU debugger.

   Copyright 2024 Free Software Foundation, Inc.

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

/* Exercise AArch64's Scalable Vector Extension in a multi-threaded program.  */

#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/prctl.h>
#include <unistd.h>

/* For one of the tests, the thread needs to sleep after setting the vector
   length.  This variable is set by GDB.  */
volatile bool should_sleep = false;

/* Used to signal to the main thread that the additional thread's vector length
   was changed.  */
sem_t vl_changed;

/* Start routine for the additional thread.  Sets a new vector length, sleeps if
   requested then restores the original vector length.  */

static void *
thread_function (void *unused)
{
  unsigned int vl;
  int rc;

  rc = prctl (PR_SVE_GET_VL, 0, 0, 0, 0);
  if (rc < 0)
    {
      perror ("FAILED to PR_SVE_GET_VL");
      sem_post (&vl_changed);
      return NULL;
    }

  vl = rc & PR_SVE_VL_LEN_MASK;

  /* Decrease vector length by 16 bytes.  */
  vl -= 16;

  rc = prctl (PR_SVE_SET_VL, vl, 0, 0, 0);
  if (rc < 0)
    {
      perror ("FAILED to PR_SVE_SET_VL");
      sem_post (&vl_changed);
      return NULL;
    }

  /* Let the main thread continue.  */
  rc = sem_post (&vl_changed);
  if (rc != 0)
    {
      perror ("sem_post");
      return NULL;
    }

  if (should_sleep)
    sleep (10);

  /* Restore original vector length.  */
  vl += 16; /* break here 1 */

  rc = prctl (PR_SVE_SET_VL, vl, 0, 0, 0);
  if (rc < 0)
    {
      perror ("FAILED to PR_SVE_SET_VL");
      return NULL;
    }

  return NULL; /* break here 2 */
}

int
main (int argc, char **argv)
{
  pthread_t thread;
  int rc;

  rc = sem_init (&vl_changed, 0, 0);
  if (rc != 0)
    {
      perror ("sem_init");
      return 1;
    }

  rc = pthread_create (&thread, NULL, thread_function, NULL);
  if (rc != 0)
    {
      perror ("pthread_create");
      return 1;
    }

  /* Wait until the additional thread changes it's vector length.  */
  rc = sem_wait (&vl_changed);
  if (rc != 0)
    {
      perror ("sem_wait");
      return 1;
    }

  rc = pthread_join (thread, NULL); /* break here 3 */
  if (rc != 0)
    {
      perror ("pthread_join");
      return 1;
    }

  return 0;
}
