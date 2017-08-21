#include <stdio.h>

extern int foo (void);

extern int __start___verbose[];
extern int __stop___verbose[];
static int my_var __attribute__((used, section("__verbose"))) = 6;
int bar (void)
{
  if (__start___verbose == __stop___verbose)
    return -1;

  if (__start___verbose[0] != 6)
    return -2;
  else
    return 0;
}

int main()
{
  if (bar () == 0 && foo () == 0)
    printf ("PASS\n");
  return 0;
}
