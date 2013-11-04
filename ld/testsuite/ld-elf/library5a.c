#include <stdio.h>

asm (".secondary bar");
asm (".weak bar");

void
bar (void)
{
  printf ("secondary bar\n");
}

void
xxx (void)
{
  printf ("xxx\n");
}
