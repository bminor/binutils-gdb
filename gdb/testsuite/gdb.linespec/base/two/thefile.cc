/* The commented line must have the same line number in the other
   "thefile.c".  */

#define WANT_F2
#include "../../lspec.h"

static int dupname(int y)
{
 label: return y;
}

int n(int y)
{
  return dupname(y) - 23;	/* thefile breakpoint */
}

int NameSpace::overload(double x)
{
  return (int) x - 23;
}
