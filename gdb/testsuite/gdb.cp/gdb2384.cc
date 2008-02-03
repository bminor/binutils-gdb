#include "gdb2384-base.h"

class derived : public base
{
 public:
  derived (int);
};

derived::derived (int _x)
  : base (_x)
{
}

int g;

int
main ()
{
  derived d (42);
  g = d.meth (); // set breakpoint here
  return 0;
}
