#include <stdio.h>

#include "config.h"

#ifndef HAVE_PTHREAD_H

/* Don't even try to compile.  In fact, cause a syntax error that we can
   look for as a compiler error message and know that we have no pthread
   support.  In that case we can just suppress the test completely. */

#error "no posix threads support"

#else

/* OK.  We have the right header.  If we try to compile this and fail, then
   there is something wrong and the user should know about it so the testsuite
   should issue an ERROR result.. */

#ifdef __linux__
#define  _MIT_POSIX_THREADS 1	/* GNU/Linux (or at least RedHat 4.0)
                                   needs this */
#endif

#include <pthread.h>

/* Under OSF 2.0 & 3.0 and HPUX 10, the second arg of pthread_create
   is prototyped to be just a "pthread_attr_t", while under Solaris it
   is a "pthread_attr_t *".  Arg! */

#if defined (__osf__) || defined (__hpux__)
#define PTHREAD_CREATE_ARG2(arg) arg
#define PTHREAD_CREATE_NULL_ARG2 null_attr
static pthread_attr_t null_attr;
#else
#define PTHREAD_CREATE_ARG2(arg) &arg
#define PTHREAD_CREATE_NULL_ARG2 NULL
#endif

void *
routine (void *arg)
{
  sleep (9);
  printf ("hello thread\n");
}

/* Marker function for the testsuite */
void
done_making_threads (void)
{
  /* Nothing */
};

void
create_thread (void)
{
  pthread_t tid;

  if (pthread_create (&tid, PTHREAD_CREATE_NULL_ARG2, routine, (void *) 0xfeedface))
    {
      perror ("pthread_create 1");
      exit (1);
    }
}

int
main (int argc, char *argv[])
{
  int i;

  /* Create a few threads */
  for (i = 0; i < 5; i++)
    create_thread ();
  done_making_threads ();

  printf ("hello\n");
  printf ("hello\n");
  return 0;
}

#endif /* ifndef HAVE_PTHREAD_H */
