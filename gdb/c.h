#include "b.h"

inline c ()
{
  Foo x = 1;
  {
    Foo y = 2;
    {
      b ();
    }
    Foo z = 3;
  }
  Foo w = 4;
}
