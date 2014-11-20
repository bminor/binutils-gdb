#include <stdio.h>

extern int foo(void);
extern int bar(void);

int (*foo_ptr)(void) = foo;

int
main (void)
{
  int (*bar_ptr)(void) = bar;
  if (bar_ptr != bar)
    __builtin_abort ();
  if (bar_ptr() != -1)
    __builtin_abort ();
  if (bar() != -1)
    __builtin_abort ();

  if (foo_ptr != foo)
    __builtin_abort ();
  if (foo_ptr() != 1)
    __builtin_abort ();
  if (foo() != 1)
    __builtin_abort ();

  printf ("OK\n");

  return 0;
}
