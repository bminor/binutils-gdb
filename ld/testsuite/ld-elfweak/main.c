#include <stdio.h>

#pragma weak foo

extern void foo ();

void
foo ()
{
  printf ("weak foo\n");
}

int
main ()
{
  foo ();
  return 0;
}
