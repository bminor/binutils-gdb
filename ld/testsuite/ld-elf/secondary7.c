#include <stdio.h>

extern void abort (void);
extern int xxx (void);

int
main (void)
{
  if (xxx () != 0)
    abort ();
  printf ("OK\n");
  return 0;
}
