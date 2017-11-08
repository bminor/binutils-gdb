#include <stdlib.h>

extern int *p_int_from_a_2;

int main (void) {
  if (*p_int_from_a_2 != 0x11223344)
    abort ();
  return 0;
}
