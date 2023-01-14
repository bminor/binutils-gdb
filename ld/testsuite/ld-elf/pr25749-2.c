#include <stdio.h>
#include <bfd_stdint.h>

extern intptr_t size;

int
main ()
{
  if (size == 137)
    printf ("PASS\n");
  return 0;
}
