#include <stdio.h>

extern void foo (void);
extern void bar (void);

void
foo (void)
{
  bar ();
  printf ("base\n");
}
