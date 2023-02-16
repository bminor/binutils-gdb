#include "solib-lib1.h"

unsigned int
adder(unsigned int a, unsigned int b, int (*call)(int))
{
  (void)(*call)(a+b);
  return (a+b);
}
