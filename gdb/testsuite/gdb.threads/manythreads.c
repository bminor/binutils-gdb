#include <pthread.h>
#include <stdio.h>

void *
thread_function (void *arg)
{
  int x = (int)arg;

  printf ("Thread <%d> executing\n", x);

  return NULL;
}

int 
main (int argc, char **argv)
{
  pthread_attr_t attr;
  pthread_t threads[256];
  int i, j;

  pthread_attr_init (&attr);

  /* Create a ton of quick-executing threads, then wait for them to
     complete.  */
  for (i = 0; i < 1000; ++i) 
    {
      for (j = 0; j < 256; ++j)
	{
	  pthread_create (&threads[j], &attr, thread_function, 
			  (void *)(i * 1000 + j));
	}

      for (j = 0; j < 256; ++j)
	{
	  pthread_join (threads[j], NULL);
	}
    }

  pthread_attr_destroy (&attr);

  return 0;
}
