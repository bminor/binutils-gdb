/* This program is linked against SOM shared libraries, which the loader
   automatically loads along with the program itself).
   */

#include <stdio.h>

extern int solib_main (int);

main ()
{
  int  result;

  /* Call a shlib function. */
  result = solib_main (100);

  /* Call it again. */
  result = solib_main (result);
}
