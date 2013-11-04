#include <stdio.h>

extern void bar (void);

void
foo (void)
{
  printf ("foo\n");
  bar ();
}
