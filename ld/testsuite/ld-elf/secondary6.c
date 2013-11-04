#include <stdio.h>

extern void xxx (void);

int
main (void)
{
  xxx ();
  printf ("OK\n");
  return 0;
}
