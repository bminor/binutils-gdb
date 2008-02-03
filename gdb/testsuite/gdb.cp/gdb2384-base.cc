#include "gdb2384-base.h"

base::base (int _x)
  : x (_x)
{
}

int
base::meth ()
{
  return x;
}
