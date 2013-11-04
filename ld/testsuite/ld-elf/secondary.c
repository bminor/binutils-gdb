#include <stdio.h>

asm (".secondary bar");

void
bar (void)
{
  printf ("secondary bar\n");
}
