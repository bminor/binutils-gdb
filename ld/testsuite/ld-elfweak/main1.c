#include <stdio.h>

#pragma weak deallocate_foo
int deallocate_foo = 1;

extern int * bar ();
extern int * foo ();
extern void abort ();

int
main ()
{
  if (&deallocate_foo != bar () || &deallocate_foo != foo ())
    abort ();

  if (deallocate_foo)
    printf ("weak deallocate_foo\n");
  else
    printf ("strong deallocate_foo\n");

  return 0;
}
