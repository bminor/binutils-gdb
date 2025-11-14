#include <stdio.h>

extern void foo (void);
extern void bar (void);

void
bar (void)
{
  printf ("bar\n");
}

int
main ()
{
  foo ();
  return 0;
}
