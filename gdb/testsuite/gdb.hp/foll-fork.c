#include <stdio.h>

void callee (i)
  int  i;
{
  printf("callee: %d\n", i);
}

main ()
{
  int  pid;
  int  v = 5;

  pid = fork ();
  if (pid == 0)
    {
      v++;
      /* printf ("I'm the child!\n"); */
    }
  else
    {
      v--;
      /* printf ("I'm the proud parent of child #%d!\n", pid); */
    }
}
