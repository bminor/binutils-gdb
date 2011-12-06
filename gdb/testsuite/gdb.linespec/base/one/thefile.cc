/* The commented line must have the same line number in the other
   "thefile.c".  */

#define WANT_F1
#include "../../lspec.h"






int m(int x)
{
  return x + 23;		/* thefile breakpoint */
}

int NameSpace::overload(int x)
{
  return x + 23;
}
