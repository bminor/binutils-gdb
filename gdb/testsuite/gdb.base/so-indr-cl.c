/* This program is linked against SOM shared libraries, which the loader
   automatically loads along with the program itself).
   */

#include <stdio.h>

extern int solib_main (int);

static int
solib_wrapper (function)
  int (*function)(int);
{
  return (*function)(100);
}


main ()
{
  int  result;

  /* This is an indirect call to solib_main. */
  result = solib_wrapper (solib_main);
}
