#include <stdio.h>

#pragma weak undef_data

extern int undef_data (void);
int (*ptr_to_data)(void) = undef_data;

int
main (void)
{
  if (ptr_to_data == NULL)
    printf ("PASSED\n");

  return 0;
}
