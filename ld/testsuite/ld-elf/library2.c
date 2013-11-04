#include <stdio.h>

void
__attribute__((weak))
bar (void)
{
#ifdef SHARED
  printf ("weak library bar (SHARED)\n");
#else
  printf ("weak library bar\n");
#endif
}
