#include <stdio.h>
#include "pr12942a.h"

extern "C" void abort ();

test_t b(void);

int
main(void)
{
  if (test != b ())
    abort ();

  printf ("OK\n");
  return 0;
}
