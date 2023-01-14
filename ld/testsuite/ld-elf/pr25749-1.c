#include <stdio.h>
#include <bfd_stdint.h>

extern intptr_t size (void);

int
main ()
{
  if (size () == 147)
    printf ("PASS\n");
  return 0;
}
