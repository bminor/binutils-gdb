#include <stdio.h>
#include <unistd.h>
/*
 * This source is mainly to test
 * what happens when a watchpoint is removed
 * while another watchpoint, inserted later is
 * left active.
 */

int count = -1;
int ival1 = -1;
int ival2 = -1;
int ival3 = -1;
int ival4 = -1;

int 
main ()
{
#ifdef usestubs
  set_debug_traps();
  breakpoint();
#endif

  for (count = 0; count < 4; count++) {
    ival1 = count; ival2 = count;
    ival3 = count; ival4 = count;
  }

  ival1 = count; ival2 = count;  /* Outside loop */
  ival3 = count; ival4 = count;

  return 0;
}
