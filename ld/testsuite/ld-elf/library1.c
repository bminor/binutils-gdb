#include <stdio.h>

void
bar (void)
{
#ifdef SHARED
  printf ("library bar (SHARED)\n");
#else
  printf ("library bar\n");
#endif
}
