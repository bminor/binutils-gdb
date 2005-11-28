#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

pid_t pids[4];

main()
{
  int i;

  for (i = 0; i < 4; i++)
    pids [i] = fork ();

  printf ("%d ready\n", getpid ());
  sleep (2);
  printf ("%d done\n", getpid ());
  exit (0);	/* Set exit breakpoint here.  */
}
